/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_ports.cpp
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    16.04.2012 New method for K20, K61, K70                              {1}
    18.04.2012 Add dedicated ADC display                                 {2}
    27.10.2012 New method for K60                                        {3}
    27.10.2012 Handle external port mask                                 {4}
    27.10.2012 Add dedicated pin names to the port header                {5}
    27.10.2012 Only display analogue voltage when pin is valid           {6}
    25.01.2014 Add Kinetis KL and KE support                             {7}
    03.12.2015 Add K80 support                                           {8}

*/

#include "config.h"

#if defined _KINETIS

#define BUF1SIZE 200

#if _VC80_UPGRADE >= 0x0600
    #define STRCPY(a, b) strcpy_s (a, BUF1SIZE, b)
    #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
    #define STRCAT(a, b) strcat_s(a, BUF1SIZE, b)
#else
    #define STRCPY strcpy
    #define SPRINTF sprintf
    #define STRCAT strcat
#endif

#if defined KINETIS_K00
    #include "kinetis_port_k00.h"
#elif defined KINETIS_K10
    #include "kinetis_port_k10.h"
#elif defined KINETIS_K26 || defined KINETIS_K65 || defined KINETIS_K66
    #include "kinetis_port_K26_K65_K66.h"
#elif defined KINETIS_K20 || defined KINETIS_K21 || defined KINETIS_K22
    #include "kinetis_port_k20.h"
#elif defined KINETIS_K30
    #include "kinetis_port_k30.h"
#elif defined KINETIS_K40
    #include "kinetis_port_k40.h"
#elif defined KINETIS_K51
    #include "kinetis_port_k51.h"
#elif defined KINETIS_K52
    #include "kinetis_port_k52.h"
#elif defined KINETIS_K53
    #include "kinetis_port_k53.h"
#elif defined KINETIS_K50
    #include "kinetis_port_k50.h"
#elif defined KINETIS_K61
    #include "kinetis_port_k61.h"
#elif defined KINETIS_K70
    #include "kinetis_port_k70.h"
#elif defined KINETIS_K80
    #include "kinetis_port_k80.h"                                        // {8}
#elif defined KINETIS_KE                                                 // {7}
    #include "kinetis_port_ke.h"
#elif defined KINETIS_KL                                                 // {7}
    #include "kinetis_port_kl.h"
#elif defined KINETIS_KV                                                 // {7}
    #include "kinetis_port_kv.h"
#elif defined KINETIS_KW2X
    #include "kinetis_port_kw.h"
#else
    #include "kinetis_port.h"                                            // K60
#endif

#if defined SUPPORT_ADC 
    static void fnAddVoltage(int iPort, char *cPortDetails, int iBit);
#endif

static unsigned char *_ptrPerFunctions;
#if defined SUPPORT_ADC                                                  // {2}
    typedef struct stPorts
    {
        unsigned char  *ports;
        unsigned short *adcs;
    } PORTS;

    static unsigned short *_ptrADC;
#endif

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
#if defined KINETIS_K00 || defined KINETIS_K20 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K70 || defined KINETIS_K80 || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV || defined KINETIS_KW2X // {1}{3}{7}
    int i;
#endif
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE + 1];
    iBit = (iPortLength - iBit - 1);                                     // bit position
    if (iBit < 0) {
        return;
    }
    
    switch (iPort) {
    case _PORTA:
        STRCPY(cPortDetails, "Port A");
        break;
#if defined KINETIS_KE || (PORTS_AVAILABLE > 1)
    case _PORTB:
        STRCPY(cPortDetails, "Port B");
        break;
#endif
#if defined KINETIS_KE || (PORTS_AVAILABLE > 2)
    case _PORTC:
        STRCPY(cPortDetails, "Port C");
        break;
#endif
#if defined KINETIS_KE || (PORTS_AVAILABLE > 3)
    case _PORTD:
        STRCPY(cPortDetails, "Port D");
        break;
#endif
#if defined KINETIS_KE || (PORTS_AVAILABLE > 4)
    case _PORTE:
        STRCPY(cPortDetails, "Port E");
        break;
#endif
#if defined KINETIS_KE || (PORTS_AVAILABLE > 5)
    case _PORTF:
        STRCPY(cPortDetails, "Port F");
        break;
#endif
#if defined KINETIS_KE
    case _PORTG:
        STRCPY(cPortDetails, "Port G");
        break;
    case _PORTH:
        STRCPY(cPortDetails, "Port H");
        break;
    case _PORTI:
        STRCPY(cPortDetails, "Port I");
        break;
#else
    case (PORTS_AVAILABLE):                                              // {2}
        STRCPY(cPortDetails, "Analog: ");
    #if defined _PIN_COUNT
        if (*cPinNumber[iPort][iBit][_PIN_COUNT] == '-') {
            return;
        }
        STRCAT(cPortDetails, "Pin: ");
        STRCAT(cPortDetails, cPinNumber[iPort][iBit][_PIN_COUNT]);
        STRCAT(cPortDetails, " - ");
    #endif
    #if defined _DEDICATED_PINS
        if (cDedicated[iBit] != 0) {
            STRCAT(cPortDetails, cDedicated[iBit]);                      // {5}
        }
    #endif
    #if defined SUPPORT_ADC 
        fnAddVoltage(_GPIO_ADC, cPortDetails, iBit);                     // display the voltage applied to ADC inputs
    #endif
        return;
#endif
    default:
      //STRCPY(cPortDetails, "?????");
        return;
    }

#if defined KINETIS_K00 || defined KINETIS_K20 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K70 || defined KINETIS_K80 || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV || defined KINETIS_KW2X // {1}{3}{7}
    SPRINTF(cBuf, " Bit %i Pin: ", iBit);
    STRCAT(cPortDetails, cBuf);
    if (*cPinNumber[iPort][iBit][_PIN_COUNT] == '-') {
        return;
    }
    STRCPY(cBuf, cPinNumber[iPort][iBit][_PIN_COUNT]);
    STRCAT(cPortDetails, cBuf);
    STRCAT(cPortDetails, " {");
    STRCAT(cPortDetails, cPer[iPort][iBit][0]);

    for (i = 1; i <= ALTERNATIVE_FUNCTIONS; i++) {
        if (*cPer[iPort][iBit][i] != '-') {
            STRCAT(cPortDetails, "/");
            STRCAT(cPortDetails, cPer[iPort][iBit][i]);
        }
    }
    STRCAT(cPortDetails, "} ");

    if (ulPortPeripheral[iPort] & (0x01 << iBit)) {
        unsigned char *ptrList = _ptrPerFunctions;
        int _iPort = iPort;
        int _iBit = iBit;
        while (_iPort--) {
            ptrList += (PORT_WIDTH);
        }
        while (_iBit--) {
            ptrList++;
        }
        if (*ptrList > ALTERNATIVE_FUNCTIONS) {
            STRCPY(&cBuf[0], "??");
        }
        else {
            if (*cPer[iPort][iBit][*ptrList] == '-') {
                STRCPY(&cBuf[0], "INVALID");
            }
            else {
                STRCPY(&cBuf[0], cPer[iPort][iBit][*ptrList]);
            }
        }
        STRCAT(cPortDetails, cBuf);
    }
    else {
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
    #if defined KINETIS_KL05_
            if ((iPort == 0) && (iBit == 5)) {                           // special case for PTA5
                if ((SIM_SOPT1 & SIM_SOPT1_OSC32KSEL_MASK) == SIM_SOPT1_OSC32KSEL_RTC_CLKIN) { // input connected to RTC clock input
                    STRCAT(cPortDetails, " (RTC_CLKIN)");
                }
            }
    #endif
        }
    }
#else
    SPRINTF(cBuf, " Bit %i Pin: ", iBit);
    STRCAT(cPortDetails, cBuf);
    STRCPY(cBuf, cPinNumber[iPort][iBit]);
    STRCAT(cPortDetails, cBuf);
    if (!strcmp(cBuf, "NA")) {
        return;
    }

    if (ulPortPeripheral[iPort] & (0x01 << iBit)) {
        unsigned char *ptrList = _ptrPerFunctions;
        int _iPort = iPort;
        int _iBit = iBit;
        while (_iPort--) {
            ptrList += (PORT_WIDTH);
        }
        while (_iBit--) {
            ptrList++;
        }
        cBuf[0] = ' ';
        if (*ptrList > 7) {
            STRCPY(&cBuf[1], "??");
        }
        else {
            STRCPY(&cBuf[1], cPer[iPort][iBit][*ptrList]);
        }
        STRCAT(cPortDetails, cBuf);
    }
    else {
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
        }
    }
#endif
}

#if defined KINETIS_K00 || defined KINETIS_K20 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K70 || defined KINETIS_K80 || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV || defined KINETIS_KW2X // {1}{3}{7}
extern unsigned long fnGetPortMask(int iPortNumber)
{
    unsigned long ulPortMask = 0x00000000;
    unsigned long ulMaskBit = 0x00000001;
    int i;
#if defined KINETIS_KE
    if (iPortNumber >= PORTS_AVAILABLE_8_BIT)
#else
    if (iPortNumber >= (PORTS_AVAILABLE + 1))
#endif
    {                                                                    // {4} handle external port mask
    #if defined _EXT_PORT_16_BIT
        return 0xffff0000;
    #else
        return 0xffffff00;
    #endif
        return 0;
    }
    for (i = 0; i < PORT_WIDTH; i++) {
        if (*cPinNumber[iPortNumber][i][_PIN_COUNT] == '-') {
            ulPortMask |= ulMaskBit;
        }
        ulMaskBit <<= 1;
    }
    return ulPortMask;
} 
#endif

extern "C" int fnGetADC_sim_channel(int iPort, int iBit)
{
#if defined _PIN_COUNT
    if (iPort == _GPIO_ADC) {                                            // dedicated analog
        if (ADC_DEDICATED_MODULE[iBit] == 0) {                           // not assigned
            return -1;                                                   // not ADC function
        }
        return (((ADC_DEDICATED_MODULE[iBit] - 1) * ADC_CHANNELS) + ADC_DEDICATED_CHANNEL[iBit]);
    }
    else {                                                               // multiplexed port
    #if defined KINETIS_KE
        if (ADC_MUX_CHANNEL[iPort][7 - iBit] == 0) {
            return -1;                                                   // not ADC function
        }
    #else
        if (ADC_MUX_CHANNEL[iPort][31 - iBit] == 0) {
            return -1;                                                   // not ADC function
        }
    #endif
    }
#endif
    return 0;
}

#if defined SUPPORT_ADC 
static void fnAddVoltage(int iPort, char *cPortDetails, int iBit)
{
    #if defined _PIN_COUNT
    char cBuf[BUF1SIZE];
    int iAdc = fnGetADC_sim_channel(iPort, (31 - iBit));
    if (iAdc < (ADC_CHANNELS * ADC_CONTROLLERS)) {                        // {6}
        SPRINTF(cBuf, " [%fV]", (((float)_ptrADC[iAdc]*((float)ADC_REFERENCE_VOLTAGE/(float)1000))/(float)0xffff));
        STRCAT(cPortDetails, cBuf);
    }
    #endif
}
#endif


// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
    #if defined SUPPORT_ADC                                              // {2}
    PORTS *_ports = (PORTS *)hw_table;
    _ptrPerFunctions = _ports->ports;
    _ptrADC          = _ports->adcs;
    #else
    _ptrPerFunctions = (unsigned char *)hw_table;
    #endif
}
#endif
