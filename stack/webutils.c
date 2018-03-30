/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      webutils.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    16.02.2007 Added fnEncode64() to support SMTP login
    01.06.2007 Changed use of user name and password checking            {1}
    23.08.2007 Allow displaying short strings (less than 4 bytes) without inserting spaces (improves formular field inputs) (INSERT_SHORT_STRINGS) {2}
    04.10.2007 Introduce WEB_ESCAPE_LEN for variable escaping size       {3}
    06.01.2008 Add defines for user information pointers                 {4}
    06.01.2008 Add tx buffer end to fnInsertHTMLString() and allow buffer growth {5}
    06.01.2008 Correct a buffer end check                                {6}
    12.01.2008 Apply INSERT_SHORT_STRINGS support always (define removed)
    18.05.2008 Modify fnEncode64() to allow general purpose encoding     {7}
    02.09.2008 Correct fnInsertHTMLString() buffer growth limit          {8}
    29.11.2008 Change fnWebStrcpy() to accept zero pointer to control its behaviour {9}
    01.12.2008 Change use of ucNewInput rather than cReference to allows user name and password to be zero terminated {10}
    14.10.2009 Don't accept empty user name/password inputs              {11}
    11.03.2010 Don't accept partical password matches with unrestricted reference length {12}
    25.06.2010 Modify fnWebStrcpy() to accept ? as terminator            {13}
    13.10.2013 Allow use of uReverseMemcpy() for buffer right shifting   {14}
    10.02.2016 Add string length return value to fnDecode64()            {15}
    13.12.2017 Correct fnBase64DecodeAlphabet() when input is '?'        {16}

*/

#include "config.h"


#if !defined WEB_ESCAPE_LEN                                              // {3}
    #define WEB_ESCAPE_LEN 4                                             // standard escape length, eg. "£xyz"
#endif

#if defined HTTP_AUTHENTICATION || defined USE_SMTP_AUTHENTICATION || defined SUPPORT_WEBSOCKET

#define USER_NAME_AND_PASS

#define B64_INVALID           0
#define B64_NEW_VALUE_READY   1
#define B64_PADDING_FOUND     2

static CHAR fnBase64DecodeAlphabet(CHAR cInput, CHAR *cRes)
{
    if (cInput == '/') {
        *cRes = '?';
        return B64_NEW_VALUE_READY;
    }
    else if (cInput == '+') {                                            // {16}
        *cRes = '>';
        return B64_NEW_VALUE_READY;
    }

    if ((cInput < '0') || (cInput > 'z')) {
        return B64_INVALID;                                              // invalid - ignore (this may be white space at the beginning or end of the string)
    }

    if (cInput <= '9') {                                                 // Input is '0'..'9'
        *cRes = cInput + ('>' - '0' - 10);
        return B64_NEW_VALUE_READY;
    }

    if (cInput >= 'a') {                                                 // Input is 'a'..'z'
        *cRes = cInput + ('>' - 'z' - 11);
        return B64_NEW_VALUE_READY;
    }

    if (cInput > 'Z') {
        return B64_INVALID;                                              // invalid - ignore
    }

    if (cInput >= 'A') {                                                 // Input is 'A'..'Z'
        *cRes = cInput + ('>' - ('Z' - 'A' + 12 + 'Z'));
        return B64_NEW_VALUE_READY;
    }

    if (cInput == '=') {
        return B64_PADDING_FOUND;                                        // padding (often found at the end)
    }

    return B64_INVALID;
}

#if defined USE_SMTP_AUTHENTICATION || defined SUPPORT_WEBSOCKET

static CHAR fnBase64EncodeAlphabet(CHAR cInputByte)
{
    if (cInputByte <= 25) {                                              // 0..25
        return (cInputByte + 'A');
    }
    if (cInputByte <= 51) {                                              // 26..51
        return ((cInputByte - 26) + 'a');
    }
    if (cInputByte <= 61) {                                              // 52..61
        return ((cInputByte - 52) + '0');
    }
    if (cInputByte == 62) {                                              // 62
        return ('+');
    }
    return ('/');                                                        // 63
}

// Perform base 64 encoding of input string, which is terminated by <= '&' character. Return the number of bytes in output buffer.
//
extern MAX_FILE_LENGTH fnEncode64(unsigned char *ptrInput, CHAR *ptrOutput, MAX_FILE_LENGTH input_length) // {7} optional length passed, input now unsigned char and return value can be as long as file length limit
{
    MAX_FILE_LENGTH len = input_length;                                  // ensure the length is in a register
    MAX_FILE_LENGTH Length = 0;    
    CHAR cNewByte;

    if (len == 0) {                                                      // limit length on web string
        while (*ptrInput > '&') {                                        // do a web string length count {7}
            len++;
            ptrInput++;
        }
        ptrInput -= len;                                                 // set input pointer back to beginning
    }

    while (len != 0) {
        cNewByte = (*ptrInput >> 2);
        *ptrOutput++ = fnBase64EncodeAlphabet(cNewByte);
        cNewByte = ((*ptrInput++ & 0x03) << 4);
        if (--len != 0) {
            cNewByte |= (*ptrInput >> 4);
        }
        Length += 2;                                                     // minumum count for single byte
        *ptrOutput++ = fnBase64EncodeAlphabet(cNewByte);
        if (len == 0) {
            break;
        }
        cNewByte = (*ptrInput++ & 0x0f) << 2;
        if (--len != 0) {
            cNewByte |= (*ptrInput >> 6);
        }
        Length++;
        *ptrOutput++ = fnBase64EncodeAlphabet(cNewByte);
        if (len == 0) {
            break;
        }
        Length++;
        cNewByte = (*ptrInput++ & 0x3f);
        *ptrOutput++ = fnBase64EncodeAlphabet(cNewByte);
        len--;
    }
    while ((Length & 0x3) != 0) {                                        // pad if necessary
        *ptrOutput++ = '=';
        Length++;
    }
    return Length;
}
#endif

#if defined HTTP_AUTHENTICATION
// Perform base 64 decoding of input string. A '&' is added at end of decoded string as terminator
//
extern int fnDecode64(CHAR *ptrInput, CHAR *ptrOutput)                   // {15}
{
    CHAR cNewByte = 0;
    unsigned long ulCollect = 0;
    unsigned char ucResult;
    unsigned char ucCollectCnt = 0;
    unsigned char ucPaddingCnt = 0;
    int iStringLength = 0;

    while (*ptrInput != '\r') {
        if (B64_INVALID != (ucResult = fnBase64DecodeAlphabet(*ptrInput++, &cNewByte))) {
            ulCollect <<= 6;                                             // make room for 6 new bits
            ulCollect += cNewByte;                                       // insert 6 new bits

            if (B64_PADDING_FOUND == ucResult) {
                ucPaddingCnt++;
            }

            if (ucCollectCnt >= 3) {                                     // 4 input bytes interpreted
                *ptrOutput++ = (unsigned char)(ulCollect >> 16);         // 3 decoded bytes available

                if (ucPaddingCnt >= 2) {
                    iStringLength++;
                    break;                                               // only one byte has been collected - rest padding and we assume end already found
                }

                *ptrOutput++ = (unsigned char)(ulCollect >> 8);

                if (ucPaddingCnt >= 1) {
                    iStringLength += 2;
                    break;                                               // only 2 bytes have been collected - last is padding and we assume end already found
                }

                *ptrOutput++ = (unsigned char)(ulCollect);
                iStringLength += 3;
                ucCollectCnt = 0;
            }
            else {
                ++ucCollectCnt;
            }
        }
    }
    *ptrOutput = '&';                                                    // terminate the decoded string
    return iStringLength;                                                // {15} length of decoded string
}
#endif
#endif

#if defined USE_HTTP && defined WEB_PARAMETER_GENERATION
// This routine is used to insert a string in place of the £XABCD... marker, which is assumed to be always WEB_ESCAPE_LEN bytes long
// Typically this routine is used to set check boxes or selected fields, etc.
// When not specifically specified, WEB_ESCAPE_LEN defaults to 4 "£XAB" {3}
//
extern int fnInsertHTMLString(CHAR *cToAdd, unsigned short usAddLength, unsigned char **ptrBuffer, unsigned short *usMaxLen, unsigned short *usLen, unsigned char *ptrBufferEnd) // {5}
{
    int iRtn = 0;                                                        // {5}
    unsigned char *ptrEnd;
    unsigned char *ptrShift;
    unsigned short usLength = *usLen - 1;

    ptrEnd = *ptrBuffer + usLength;                                      // set pointer to end of buffer
    ptrShift = ptrEnd - usAddLength;                                     // set pointer to end after insertion

    if ((*ptrBuffer - WEB_ESCAPE_LEN + usAddLength) > ptrEnd) {          // no space to insert in this frame {6} (was previously >= this was not serious but > is more correct and so more efficient when the length is identical to the buffer length)
        if ((ptrEnd + usAddLength - WEB_ESCAPE_LEN) < ptrBufferEnd)      // {8} check whether the buffer has room to grow
      //if ((*ptrBuffer + (WEB_ESCAPE_LEN - 2) + usAddLength) < ptrBufferEnd)  original check which was corrected (for monitoring purposes)
        { // {5} there is still space in the physical buffer so make use of it
            unsigned short usLengthIncrease = (usAddLength - WEB_ESCAPE_LEN);
            ptrEnd += usLengthIncrease;                                  // increase buffer length
            ptrShift += usLengthIncrease;
            usLength += usLengthIncrease;
            *usLen += usLengthIncrease;
            *usMaxLen += usLengthIncrease;
            iRtn = -usLengthIncrease;
        }
        else {
            int iFrameReduction = ptrEnd - *ptrBuffer + WEB_ESCAPE_LEN;
            *usMaxLen -= (usLength + WEB_ESCAPE_LEN);
            *ptrBuffer = 0;                                              // present frame terminated so mark by clearing pointer
            return (iFrameReduction);
        }
    }

    if (usAddLength >= WEB_ESCAPE_LEN) {
        usLength -= (usAddLength - WEB_ESCAPE_LEN);                      // the remaining length after insertion
        ptrShift += WEB_ESCAPE_LEN;
        if (ptrEnd != ptrShift) {                                        // only shift if necessary
    #if defined UREVERSEMEMCPY                                           // {14} allow some processors to benefit from DMA controlled reverse copy
            ptrEnd -= usLength;
            ptrShift -= usLength;
            uReverseMemcpy(ptrEnd, ptrShift, usLength);                  // shift remaining bytes along in buffer, making room for the insertion
    #else
            while (usLength--) {
                *(--ptrEnd) = *(--ptrShift);                             // shift remaining bytes along in buffer, making room for the insertion
            }
    #endif
        }
    }
    else {
        ptrShift = *ptrBuffer - (WEB_ESCAPE_LEN - usAddLength);
        ptrEnd = *ptrBuffer;
        uMemcpy(ptrShift, ptrEnd, usLength);
    }

    ptrShift = *ptrBuffer - WEB_ESCAPE_LEN;
    uMemcpy(ptrShift, cToAdd, usAddLength);                              // insert the string
    *ptrBuffer = ptrShift + usAddLength;

    if (usAddLength >= WEB_ESCAPE_LEN) {
        usAddLength -= WEB_ESCAPE_LEN;
        *usLen -= (usAddLength);                                         // correct lengths after insertion
        *usMaxLen -= usAddLength;
    }
    else {
        return (WEB_ESCAPE_LEN - usAddLength);                           // shorten complete file by this amount
    }
    return iRtn;                                                         // {5}
}
#endif


#if defined USER_NAME_AND_PASS
// This checks clear text passwords using & as terminator for the input and & or null terminator for reference
//
extern int fnCheckPass(CHAR *cReference, CHAR *ucNewInput)
{
    while ((*cReference != '&') && (*cReference != 0) && (*ucNewInput != '&')) { // {10}{12}
        if (*cReference++ != *ucNewInput++) {
            return 1;                                                    // bad password!!
        }
    }   
    if (((*cReference != '&') && (*cReference != 0)) || (*ucNewInput != '&')) { // {12}
        return 1;                                                        // bad password!!
    }
    return 0;                                                            // password is OK
}


// Check user name and user password
//
extern int fnVerifyUser(CHAR *cDecodedUser, unsigned char ucCheckUser)   // {1}
{
    CHAR *cPass = cDecodedUser;
    CHAR cTermination = ':';

    if (FTP_PASS_CHECK & ucCheckUser) {
        cTermination = 0x0d;
    }

    while ((*cPass) && (*cPass != cTermination)) {
        cPass++;
    }
    if ((HTML_PASS_CHECK & ucCheckUser) && (*cPass != ':')) {
        return CREDENTIALS_REQUIRED;
    }

    *cPass++ = '&';                                                      // terminate
    if ((HTML_PASS_CHECK & ucCheckUser) == 0) {
        cPass = cDecodedUser;
    }
    if ((DO_CHECK_USER_NAME & ucCheckUser) != 0) {                       // {1}
        if (fnCheckPass(POINTER_USER_NAME, cDecodedUser)) {
            return CREDENTIALS_REQUIRED;                                 // return if user name match not valid {4}
        }
    }
    if ((DO_CHECK_PASSWORD & ucCheckUser) != 0) {                        // {1}
        if (fnCheckPass(POINTER_USER_PASS, cPass) != 0) {
            return CREDENTIALS_REQUIRED;                                 // return if user password is not good {4}
        }
    }
    return 0;                                                            // authorised successfully
    // Note that the routine could control the web side displayed on login (or failure) by returning a valid file reference...
}

// Convert a string as sent over the web into a normal null-terminated string
//
extern CHAR *fnWebStrcpy(CHAR *cStrOut, CHAR *cStrIn)
{
    int iNullTerminate = 0;
    if (cStrOut == 0) {                                                  // {9}
        cStrOut = cStrIn;                                                // automatically use input buffer as output buffer
        iNullTerminate = 1;                                              // in this case we switch to pure null termination mode
    }
    FOREVER_LOOP() {
    #if !defined WEB_PLUS_NO_CODING                                      // it has been found that + is not always coded so using this define allows it to be used directly
        if (*cStrIn == '+') {                                            // space
            *cStrOut = ' ';
        }
        else 
    #endif
        if (*cStrIn == '%') {                                            // we have to replace the HEX ASCII value following with its hex value 
            unsigned char ucValue1 = *(++cStrIn) - '0';
            unsigned char ucValue2 = *(++cStrIn) - '0';
            if (ucValue1 > 9) {
                ucValue1 -= 'A' - '9' - 1;
                ucValue1 &= 0x0f;                                        // handle upper case
            }
            if (ucValue2 > 9) {
                ucValue2 -= 'A' - '9' - 1;
                ucValue2 &= 0x0f;                                        // handle upper case
            }
            ucValue1 <<= 4;
            ucValue1 |= ucValue2;
            *cStrOut = ucValue1;                                         // replace with its hex value
        }
        else if ((*cStrIn <= '&') || (*cStrIn == '?')) {                 // {13}
            if (iNullTerminate != 0) {                                   // {9} if pure zero terminate mode
                if (*cStrIn == 0) {
                    break;
                }
                *cStrOut = *cStrIn;                                      // not a true termination so continue
            }
            else {
                break;
            }
        }
        else {
            *cStrOut = *cStrIn;
        }
        cStrIn++;
        cStrOut++;
    }
    *cStrOut++ = 0;                                                      // terminate the string
    return cStrOut;
}
#endif
