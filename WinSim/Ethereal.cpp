/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      Ethereal.cpp
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    08.01.2008 Content made conditional on Ethernet availability
    15.02.2009 Moved check of file open to before attempted read         {1}

*/

#include "config.h"

#ifdef ETH_INTERFACE
#include "WinSim.h"
#include "Fcntl.h"
#include "io.h"
#include <sys/stat.h>
#if _VC80_UPGRADE>=0x0600
  #include <share.h>
#endif

static int iSimActive = 0;
static unsigned char ucLastTimeStamp[] = {0,0,0,0,0,0,0,0};
static int iFirstTime = 0;
static unsigned long ulOldSecs;
static unsigned long ulOldMicro;

void fnReactivateNIC(void);

static int fnTimeDelay(unsigned char *ptrTime) 
{
    unsigned long ulSeconds = 0;
    unsigned long ulMicros = 0;
    int i = 0;
    int iDelay;

    while (i < 4) {
	    ulSeconds >>= 8;
	    ulSeconds |= (unsigned long)(*ptrTime << 24);
	    ucLastTimeStamp[i++] = *ptrTime++;
    }

    while (i < 8) {
	    ulMicros >>= 8;
	    ulMicros |= (unsigned long)(*ptrTime << 24);
	    ucLastTimeStamp[i++] = *ptrTime++;
    }

    if (!iFirstTime) {
	    iFirstTime = 1;
	    ulOldSecs = ulSeconds;
	    ulOldMicro = ulMicros;
	    return 1;
    }


    if (ulMicros >= ulOldMicro) {
      iDelay = (ulMicros - ulOldMicro)/50000;  
    }
    else {
      iDelay = ((1000000 - ulOldMicro) + ulMicros)/50000;
      ulOldSecs++;
    }

    if (ulSeconds >= ulOldSecs) {
      iDelay += ((ulSeconds - ulOldSecs)*(1000/50));  
    }
    else {
      iDelay += (((0xffffffff - ulOldSecs) + ulSeconds + 1)*(1000/50));
    }

    ulOldSecs = ulSeconds;
    ulOldMicro = ulMicros;

    if (iDelay) {
	    return iDelay;                                                   // next best multiple of 50ms periods to wait
    }
    else {
	    return 1;
    }
}

// We open a recording and play the data in
//
static int fnDoEtherealSim(CHAR *szEthernetFileName)
{
	static int iFileEth = 0;
	static unsigned long ulDelayed = 0;
	static unsigned char ucMacSave[6];
	unsigned char ucData[2000];
	unsigned char ucNewTime[8];
	unsigned long ulLen;
    int iNextDelay = 0;

	if (iFileEth <= 0) {
#if _VC80_UPGRADE<0x0600
		iFileEth = _open(szEthernetFileName, (_O_RDONLY | _O_BINARY));
#else
		_sopen_s(&iFileEth, szEthernetFileName, (_O_RDONLY | _O_BINARY), _SH_DENYWR, _S_IREAD);
#endif
        if (iFileEth < 0) {
            return 0;                                                    // {1}
        }
        _read(iFileEth, ucData, 24);                                     // the first 24 are header
	}

	while (1) {
        if (!ulDelayed) {		                                         // get frame number  
			if (_read(iFileEth, ucNewTime, 8) <= 0) break;               // time
			if (_read(iFileEth, ucData, 4) <= 0) break;                  // packet length
			ulLen = (ucData[0] + (ucData[1] << 8) + (ucData[2] << 16) + (ucData[3] << 24));
			if (_read(iFileEth, ucData, 4) <= 0) break;                  // capture length
			if (_read(iFileEth, &ucData[3], 6) <= 0) break;              // MAC of frame
		}
		else {	            
			memcpy(&ucData[3], ucMacSave, 6);			
		    ulLen = ulDelayed + 6;
		}

        iNextDelay = 1;

        if (!ulDelayed) {
            iNextDelay = fnTimeDelay(ucNewTime);


            if (iNextDelay <= 0) {
                iNextDelay = 1;
            }
        }

        if (iNextDelay == 1) {
            if (_read(iFileEth, &ucData[3 + 6], ulLen-6) <= 0) break;    // frame
            fnInjectFrame(ucData + 3, (unsigned short)(ulLen));
            ulDelayed = 0;
        }
        else {
            memcpy(ucMacSave, &ucData[3], 6); 			                 // we must wait for time delay to complete before sending frame
            ulDelayed = (ulLen-6);
        }
        return (iNextDelay);
	}
	_close(iFileEth);
	iFirstTime = 0;
    iFileEth = 0;
	ulDelayed = 0;
    fnReactivateNIC();
    return 0;
}

extern void fnDoEthereal(int iStart, CHAR *szEthernetFileName)
{
	if ((iSimActive == 1) || (iStart)) {
      if ((iStart) && (iSimActive)) return;                              // ignore start when already active

	  iSimActive = fnDoEtherealSim(szEthernetFileName);
	}
	else {
		if (iSimActive) {
			iSimActive--;
		}
	}
}
#endif