/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      ip_utils.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    01.03.2007 Add file name termination when ? found. This is useful when sending files in html text where this identifies the end of a name.
    26.05.2007 Improve MIME type search                                  {2}
    16.04.2009 Enable uStrEquiv() when INTERNAL_USER_FILES is defined    {3}
    01.07.2009 Adapt for compatibility with STRING_OPTIMISATION (fnDebugDec() replaced by fnBufferDec()) {4}
    02.02.2010 Add IPV6 routines                                         {5}
    05.02.2011 Allow uStrEquiv() to be used with project define STR_EQUIV_ON {6}
    02.04.2012 Modify return pointer from fnStrIPV6() for compatibility with fnStrIP() {7}
    14.06.2012 Add IPV6_STRING_ROUTINES option                           {8}
    18.07.2014 Modify fnStrIP() to allow any demimite dto terminate IP address fields {9}
    20.11.2014 Add trailing zero to IPv6 addresses ending with multiple zeroes {10}

*/

    
#include "config.h"


#if defined STRING_OPTIMISATION                                          // {4}
    #define _fnBufferDec(x, y, z) fnBufferDec(x, y, z)                   // new decimal string routine
#else
    #define _fnBufferDec(x, y, z) fnDebugDec(x, y, z)                    // original decimal string routine
#endif

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
// Convert an input string to a MAC address
//
extern CHAR *fnSetMAC(CHAR *ptr_input, unsigned char *ptrMac)
{
    unsigned char ucBackup;
    unsigned char ucNewValue[MAC_LENGTH];
    int iLoop;
    int i;

    for (i = 0; i < MAC_LENGTH; i++) {
        iLoop = 0;
        while ((*ptr_input >= '0') 
    #if MAC_DELIMITER >= '0'
            && !(*ptr_input == MAC_DELIMITER)
    #endif
            ) {
            ptr_input++;
            iLoop++;
        }
        if (!iLoop || (iLoop > 2)) {
            return 0;                                                    // invalid format
        }
        ucBackup = *ptr_input;
        *ptr_input = 0;                                                  // terminate with null  to create short string segment
        ucNewValue[i] = (unsigned char)fnHexStrHex(ptr_input - iLoop);
        *ptr_input++ = ucBackup;
    }
    uMemcpy(ptrMac, ucNewValue, MAC_LENGTH);                             // save the new MAC address to the specified location
    return ptr_input;                                                    // OK - point to next location in input string
}
#endif

#if defined USE_IP
// Convert a MAC address to a string
//
extern CHAR *fnMACStr(unsigned char *ptrMAC, CHAR *cStr)
{
    int i = MAC_LENGTH;

    while (--i) {
        cStr = fnBufferHex(*ptrMAC++, (unsigned char)(1 | NO_LEADIN | NO_TERMINATOR), cStr);
        *cStr++ = MAC_DELIMITER;
    }
    return (fnBufferHex(*ptrMAC++, (unsigned char)(1 | NO_LEADIN | WITH_TERMINATOR), cStr));
}

// Convert an input string to IP address
//
extern CHAR *fnStrIP(CHAR *ptr_input, unsigned char *ucIP_address)
{
    unsigned char ucBackup;
    int iLoop;
    int i;

    for (i=0; i < IPV4_LENGTH; i++) {
        iLoop = 0;
      //while ((*ptr_input) && !(*ptr_input <= '.'))
        while ((*ptr_input >= '0') && (*ptr_input <= '9'))               // {9} allow any non-digit to terminate
        {
            ptr_input++;
            iLoop++;
        }
        if (!iLoop || (iLoop > 3)) {
            return 0;                                                    // invalid format
        }
        ucBackup = *ptr_input;
        *ptr_input = 0;                                                  // terminate with null  to create short string segment
        *ucIP_address++ = (unsigned char)fnDecStrHex(ptr_input - iLoop); // convert the decimal value to hex
        *ptr_input++ = ucBackup;                                         // return original
    }
    return ptr_input;                                                    // OK - point to next location in input string
}

// Convert an IP address to a string
//
extern CHAR *fnIPStr(unsigned char *ptrIP, CHAR *cStr)
{
    int i = IPV4_LENGTH;

    while (--i) {
        cStr = _fnBufferDec(*ptrIP++, NO_TERMINATOR, cStr);
        *cStr++ = '.';
    }
    return (_fnBufferDec(*ptrIP, WITH_TERMINATOR, cStr));
}
#endif

#if defined USE_IP || defined INTERNAL_USER_FILES || defined STR_EQUIV_ON// {3}{6}
// Tries to match a string, where lower and upper case are treated as equal
//
extern unsigned short uStrEquiv(const CHAR *cInput, const CHAR *cMatch)
{
    unsigned short usMatch = 0;
    CHAR cReference;

    while ((cReference = *cMatch) != 0) {
        if (*cInput != cReference) {
            if (cReference >= 'a') {                                     // verify that it is not the case which doesn't match
                cReference -= ('a' - 'A');                               // try capital match
            }
            else if (cReference >= 'A') {
                cReference += ('a' - 'A');                               // try small match
            }
            if (*cInput != cReference) {                                 // last chance
                return 0;
            }
        }
        cMatch++;
        cInput++;
        usMatch++;
    }
    return usMatch;                                                      // return the length of match
}
#endif

#if defined USE_IP && defined SUPPORT_MIME_IDENTIFIER

// Interpret a file extension and determine a MIME type
//
extern unsigned char fnGetMimeType(CHAR *ptrFileName) 
{
    unsigned char ucMimeType = 0;
    int iExtensionFound = 0;
    while ((*ptrFileName >= ' ') && (*ptrFileName != '?')) {             // {1}
        if ((*ptrFileName == ' ') && (iExtensionFound != 0)) {           // {2}
            break;
        }
        if (*ptrFileName == '.') {
            iExtensionFound = 1;                                         // mark that we have found an extension seperator
        }
        ptrFileName++;
    }                                                                    // search to the end of the file name

    if (iExtensionFound) {
        while (*ptrFileName != '.') {
            ptrFileName--;
        }

        while (ucMimeType < UNKNOWN_MIME) {
            if (uStrEquiv((ptrFileName+1), cMimeTable[ucMimeType])) {
                return ucMimeType;
            }
            ucMimeType++;
        }
    }

    return UNKNOWN_MIME;
}
#endif

#if defined USE_IPV6 || defined IPV6_STRING_ROUTINES                     // {8}
// Convert an IPV6 address to a string - the string buffer is assumed to have adequate space for the worst case
//
extern CHAR *fnIPV6Str(unsigned char *ptrIP, CHAR *cStr)
{
    int i;
    int iZeroOmitted = 0;
    int iReduceStringLength;
    unsigned short usPair;

    for (i = 0; i < (IPV6_LENGTH/2); i++) {
        usPair = *ptrIP++;                                               // hexadecimal pair
        usPair <<= 8;
        usPair |= *ptrIP++;
        if (usPair == 0) {
            iZeroOmitted++;
            continue;
        }
        if (iZeroOmitted != 0) {
            *cStr++ = ':';
            iZeroOmitted = 0;
        }
        if ((usPair & 0xf000) == 0) {
            if ((usPair & 0xff00) == 0) {
                if ((usPair & 0xfff0) == 0) {
                    iReduceStringLength = 3;
                }
                else {
                    iReduceStringLength = 2;
                }
            }
            else {
                iReduceStringLength = 1;
            }
        }
        else {
            iReduceStringLength = 0;
        }
        cStr = fnBufferHex(usPair, (sizeof(usPair) | NO_TERMINATOR), cStr);
        if (iReduceStringLength != 0) {
            uMemcpy((cStr - 4), (cStr - (4 - iReduceStringLength)), (4 - iReduceStringLength));
        }
        cStr -= iReduceStringLength;
        *cStr++ = ':';
    }
    cStr--;
    if (iZeroOmitted != 0) {
        *cStr++ = ':';
        *cStr++ = '0';                                                   // {10}
    }
    *cStr = 0;
    return cStr;                                                         // return pointer to the string terminator
}

// Convert an input string to IPV6 address
//
extern CHAR *fnStrIPV6(CHAR *ptr_input, unsigned char *ucIP_address)
{
    int i;
    int iEnd = IPV6_LENGTH;
    int iShortHandRecognised = 0;
    unsigned long ulNextPair;
    for (i = 0; i < IPV6_LENGTH;) {                                      // for each possible byte pair
        ulNextPair = fnHexStrHex(ptr_input);
        if (ulNextPair > 0xffff) {
            return 0;                                                    // invalid format
        }
        *ucIP_address++ = (unsigned char)(ulNextPair >> 8);
        *ucIP_address++ = (unsigned char)(ulNextPair);
        while (*ptr_input != IPV6_DELIMITER) {
            if (*ptr_input == 0) {                                       // terminator detected
                if ((iShortHandRecognised == 0) && (i < (IPV6_LENGTH - 2))) {
                    return 0;                                            // incomplete input
                }
                return (ptr_input + 1);                                  // {7} return pointer to terminator, indicating valid
            }
            ptr_input++;            
        }
        ptr_input++;
        i += 2;
        iEnd -= 2;
        if (iShortHandRecognised != 0) {
            uMemcpy((ucIP_address - iShortHandRecognised - 2), (ucIP_address - iShortHandRecognised), iShortHandRecognised);
            iShortHandRecognised += 2;
            ucIP_address -= 2;
        }
        else if (*ptr_input == IPV6_DELIMITER) {                         // double :: detected - only expected once
            uMemset(ucIP_address, 0, iEnd);                              // fill zeros to end
            ucIP_address += (iEnd - 2);                                  // set to last output pair
            ptr_input++;
            iShortHandRecognised = 2;
        }
    }
    return 0;                                                            // no terminator found, therefore assume invalid input
}
#endif

