/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      WinPcap.cpp
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    08.01.2008 Made content conditional on Ethernet interface availability
    16.07.2008 Wait before injecting Ethernet frames if the main task is active {1}
    01.05.2010 Add local define _EXCLUDE_WINDOWS_

*/

#define _EXCLUDE_WINDOWS_
#include "config.h"

#if defined ETH_INTERFACE
#include "WinPcap.h"
#include "WinSim.h"
#include "WinSimThreads.h"
#include "resource.h"

#include <pcap.h>
#include <winsock2.h>


#if _VC80_UPGRADE>=0x0600
    #define STRCPY strcpy_s
#else
    #define STRCPY strcpy
#endif

extern "C" int iWinPcapSending = 0;                                      // export flag to stop scheduling when the WinPcap thread is sending data
extern int iRxActivity = 0, iTxActivity = 0;

static char szDefaultDevice[256];
static char szDevice[256];                                               // holds Ethernet device name
static int  iSelectedNIC = -1;
static BOOL bWinPcapActive = FALSE;
static int  iNicCount = 0;                                               // valid NIC count of machine
static pcap_t *hDev;                                                     // handle to open adapter

extern int main(int argc, char *argv[]);

// Send a Ethernet packet
//
extern "C" void fnWinPcapSendPkt(int len, unsigned char * ptr)
{                                                                        // this is called by WinSim.c
    if (bWinPcapActive == TRUE) {
        iWinPcapSending = 1;
        iTxActivity = 2;
        pcap_sendpacket(hDev,ptr,len);
        iWinPcapSending = 0;
    }
}

static pcap_if_t *fnGetNicList(void)
{
    pcap_if_t *devs;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&devs, errbuf) == -1) {                         // retrieve the local device list
        return(NULL);
    }
    return (devs);
}

static void fnFreeNicList(pcap_if_t *devs)
{
    pcap_freealldevs(devs);
}



// Display a combo box list of available NICs and return a cursel to the active one
//
extern int fnShowNICs(HWND pulldown)
{
    pcap_if_t *devs = NULL;
    pcap_if_t *d;
    int iNumber;

    iNumber = SendMessage(pulldown, CB_INSERTSTRING, 0, (LPARAM)"No Device used for simulation");
    if ( (devs = fnGetNicList()) == NULL )
        return -1;

    for ( d=devs; d; d=d->next ) {
        if ( d->name ) {
            if ( d->description ) {
                 iNumber = SendMessage(pulldown, CB_INSERTSTRING, iNumber+1, (LPARAM)d->description);
            }
        }
    }
    return iSelectedNIC;
}

// Check the NICs available and select the one defined by the user
//
extern void fnWinPcapSelectLAN(int NIC_nr) 
{
    pcap_if_t *devs = NULL;
    pcap_if_t *d;

    if (NIC_nr < 0) {
        return;                                                          // no NIC defined
    }

    if ((devs = fnGetNicList()) == NULL) {
        return;
    }

    iNicCount = 0;
    for (d=devs; d; d=d->next) {
        if (d->name != 0) {
            if (NIC_nr == iNicCount) {
                iSelectedNIC = NIC_nr;
                STRCPY(szDefaultDevice, d->name);
                break;
            }
        }
        iNicCount++;
    }

    fnFreeNicList(devs);

    hTermWinPcapRead = CreateEvent(NULL,                                 // create resources for threads
                                   TRUE,                                 // manual reset event
                                   FALSE,                                // initial state
                                   "TerminateWinPcapRead"); 

    nThreadState[THR_WINPCAP_READ] = TERMINATED;
}


static void ReadDevice(ThrArgs *hArgs)
{
    DWORD status;
    HANDLE hTermEvent = hArgs->hTermEvent;
    HWND hWnd = hArgs->hWnd;
    struct pcap_pkthdr *header;
    const u_char *pkt_data;
    char *ptr[3];
    int res;

    while (1) {
        status = WaitForSingleObject(hTermEvent,0);
        if (status == WAIT_OBJECT_0) {
            ResetEvent(hTermEvent);
            ExitThread(1);
        }

        res = pcap_next_ex(hDev, &header, &pkt_data);                    // read the packets

        if (res == 0) {                                                  // timeout elapsed
            continue;
        }

        if (res == -1) {
            fnWinPcapClose();
            Sleep(2000);                                                 // wait for thread to finish
            ExitThread(1);
        }

        ptr[0] = (char *)&header->len;
        ptr[1] = (char *)pkt_data;
        ptr[2] = 0;

#if defined ETH_INTERFACE
    #if defined SUPPORT_VLAN
        #define __LAN_BUFFER_SIZE (LAN_BUFFER_SIZE + 4)
    #else
        #define __LAN_BUFFER_SIZE  LAN_BUFFER_SIZE
    #endif
        if (header->len <= __LAN_BUFFER_SIZE) {                          // frames too large for the user's receive buffer are discarded
            while (main(RX_ETHERNET, ptr) == WAIT_WHILE_BUSY) {          // {1}
                Sleep(1);
            }
        }

        if (ptr[2] != 0) {
            iRxActivity = 2;
        }
#endif
    }
}

static void fnWinPcapStartLink(HWND hWnd)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    if (strlen(szDevice) != 0) {
        fnWinPcapClose();                                                // close if open

        if ((hDev = pcap_open_live(szDevice,                             // name of the device
                                    65536,                               // portion of the packet to capture. 
                                                                         // 65536 grants that the whole packet will be captured on all the MACs.
                                    TRUE,                                // promiscuous mode (nonzero means promiscuous)
                                    1,                                   // read timeout
                                    errbuf                               // error buffer
                                   )) == NULL)
        {
            return;
        }
        bWinPcapActive = TRUE;

        if (nThreadState[THR_WINPCAP_READ] == TERMINATED) {
            ThrArgs ReadArgs;                                            // used to pass arguments into thread

            ReadArgs.hTermEvent = hTermWinPcapRead;
            ReadArgs.hWnd = hWnd;

            hThreadRead = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)ReadDevice,
                                        (LPVOID)&ReadArgs, 0, (LPDWORD)&ThreadIDRead);
            nThreadState[THR_WINPCAP_READ] = ACTIVE;
        }
    }
}

extern "C" void fnOpenDefaultHostAdapter(void)
{
    if (strlen(szDefaultDevice) != 0) {
        STRCPY(szDevice, szDefaultDevice);

        if (ghWnd != NULL) {
            fnWinPcapStartLink(ghWnd);
        }
    }
}

extern void fnWinPcapOpenAdapter(void)
{
    fnOpenDefaultHostAdapter();
}

extern void fnWinPcapClose(void)
{
    if ((nThreadState[THR_WINPCAP_READ] == ACTIVE ) || (bWinPcapActive == TRUE)) {
        SetEvent(hTermWinPcapRead);
        Sleep(1000);                                                     // wait for thread to finish
        hThreadRead = NULL;
        nThreadState[THR_WINPCAP_READ] = TERMINATED;
    }

    if (bWinPcapActive == TRUE) {
        pcap_close(hDev);
    }

    bWinPcapActive = FALSE;
}


extern void fnWinPcapStopLink(HWND hWnd)
{
    if (strlen(szDevice) != 0) {
        if (bWinPcapActive == TRUE) {
            fnWinPcapClose();
        }
    }
}
#endif
