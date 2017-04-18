#ifndef _WINSIMTHREADS_H_
#define _WINSIMTHREADS_H_

/* Thread state  */
#define ACTIVE		        1
#define SUSPENDED       	2
#define TERMINATED	        3

/* Thread idices */
#define NUM_THREADS         1

#define THR_WINPCAP_READ    0
#define THR_UTASKER         1
#define THR_COM             2


typedef struct _ThrArgs
{
	HANDLE hTermEvent;  /* termination event    */
	HWND hWnd;          /* window we belong to  */
	
}ThrArgs,*pThrArgs;


/* Event Handles */
HANDLE hTermWinPcapRead;
HANDLE hTermUTasker;
HANDLE hTermCom;

/* Thread Handles */
HANDLE hThreadRead;

/* Thread IDs */
DWORD ThreadIDRead;

/* Thread arguments are define in local module */

int nThreadState[NUM_THREADS];

#endif