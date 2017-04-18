#ifndef _WINPCAP_H_
#define _WINPCAP_H_

#include "stdafx.h"



extern void fnUpdateText(HWND hWnd);

extern void fnWinPcapStopLink(HWND hWnd);
extern void fnWinPcapSelectLAN(int NIC_nr);
extern void fnWinPcapOpenAdapter(void);
extern void fnWinPcapClose();
extern int fnShowNICs(HWND pulldown);

extern HWND ghWnd;           // global Window Handle assigned at CREATE

#endif