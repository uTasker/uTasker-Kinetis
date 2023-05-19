/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      FreeMaster.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    This file contains a partial uTasker redevelopment of the FreeMaster implementation which
    is operational on UART and USB-CDC.
    Presently it includes also original Freescale code to achieve operation, which will be reworked during further development.

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined FREEMASTER_CDC || defined FREEMASTER_UART
// FreeMaster interface

/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_serial.c
*
* @brief  FreeMASTER SCI communication routines
*
* @version 1.1.1.0
* 
* @date May-4-2012
* 
*******************************************************************************/


/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster.h
*
* @brief  FreeMASTER Driver main API header file, Kxx(40/60) platform
*
* @version 1.0.1.0
* 
* @date May-4-2012
* 
*******************************************************************************
*
* This is the only header file needed to be included by the user application
* to implement the FreeMASTER interface. In addition, user has to write her 
* "freemaster_cfg.h" configuration file and put it anywhere on the #include path
*
*******************************************************************************/

#ifndef __FREEMASTER_H
#define __FREEMASTER_H

/* identify our current platform */
#define FMSTR_PLATFORM_KXX 1

/* user configuration */
//#include "freemaster_cfg.h"
/******************************************************************************
*
* freemaster_cfg.h
*
* FreeMASTER Serial Communication Driver configuration file
*
*******************************************************************************/

#ifndef __FREEMASTER_CFG_H
#define __FREEMASTER_CFG_H

/******************************************************************************
* Select interrupt or poll-driven serial communication
******************************************************************************/

#define FMSTR_LONG_INTR    0        // complete message processing in interrupt
#define FMSTR_SHORT_INTR   0        // SCI FIFO-queuing done in interrupt
#define FMSTR_POLL_DRIVEN  1        // no interrupt needed, polling only

/*****************************************************************************
* Select communication interface (SCI or CAN)
******************************************************************************/

#define FMSTR_SCI_BASE 0x4006B000u // UART1 base
#define FMSTR_SCI_INTERRUPT 29

//#define FMSTR_CAN_BASE 0x40024000     // FlexCAN0 base on K40
#define FMSTR_CAN_BASE 0x400A4000     // MSCAN1 base on K40
#define FMSTR_CAN_RX_INTERRUPT 38 // HC12 only, MSCAN RX interrupt number (use PRM if not defined)
#define FMSTR_CAN_TX_INTERRUPT 39 // HC12 only, MSCAN TX interrupt number (use PRM if not defined) 

#define FMSTR_SCI_TWOWIRE_ONLY   1

#define FMSTR_USE_FLEXCAN 0       // To selecet FlexCAN communication interface

/******************************************************************************
* Input/output communication buffer size
******************************************************************************/

#define FMSTR_COMM_BUFFER_SIZE 0    // set to 0 for "automatic"

/******************************************************************************
* Receive FIFO queue size (use with FMSTR_SHORT_INTR only)
******************************************************************************/

#define FMSTR_COMM_RQUEUE_SIZE 32   // set to 0 for "default"

/*****************************************************************************
* Support for Application Commands 
******************************************************************************/

//#define FMSTR_USE_APPCMD          // enable/disable App.Commands support
#define FMSTR_APPCMD_BUFF_SIZE 32   // App.Command data buffer size
#define FMSTR_MAX_APPCMD_CALLS 4    // how many app.cmd callbacks? (0=disable)

/*****************************************************************************
* Oscilloscope support
******************************************************************************/

#define FMSTR_USE_SCOPE             // enable/disable scope support
#define FMSTR_MAX_SCOPE_VARS  8     // max. number of scope variables (2..8)

/*****************************************************************************
* Recorder support
******************************************************************************/

#define FMSTR_USE_RECORDER          // enable/disable recorder support
#define FMSTR_MAX_REC_VARS    2     // max. number of recorder variables (2..8)
#define FMSTR_REC_OWNBUFF     0     // use user-allocated rec. buffer (1=yes)

// built-in recorder buffer (use when FMSTR_REC_OWNBUFF is 0)
#define FMSTR_REC_BUFF_SIZE   2048  // built-in buffer size

// recorder time base, specifies how often the recorder is called in the user app.
//#define FMSTR_REC_TIMEBASE    FMSTR_REC_BASE_MILLISEC(0) // 0 = "unknown"
#define FMSTR_REC_TIMEBASE    FMSTR_REC_BASE_MICROSEC(50) // 0 = "unknown"


/*****************************************************************************
* Target-side address translation (TSA)
******************************************************************************/

//#define FMSTR_USE_TSA             // enable TSA functionality
#define FMSTR_USE_TSA_SAFETY  0     // enable access to TSA variables only
#define FMSTR_USE_TSA_INROM   0     // TSA tables declared as const (put to ROM)

/*****************************************************************************
* Enable/Disable read/write memory commands
******************************************************************************/

#define FMSTR_USE_READMEM      1    // enable read memory commands
#define FMSTR_USE_WRITEMEM     1    // enable write memory commands
#define FMSTR_USE_WRITEMEMMASK 1    // enable write memory bits commands

/*****************************************************************************
* Enable/Disable read/write variable commands (a bit faster than Read Mem)
******************************************************************************/

#define FMSTR_USE_READVAR      0    // enable read variable fast commands
#define FMSTR_USE_WRITEVAR     0    // enable write variable fast commands
#define FMSTR_USE_WRITEVARMASK 0    // enable write variable bits fast commands

#define FMSTR_BUFFER_ACCESS_BY_FUNCT 1

#endif /* __FREEMASTER_CFG_H */


/*****************************************************************************
* Global types
******************************************************************************/

typedef unsigned char* FMSTR_ADDR;   /* CPU address type (4bytes) */
typedef unsigned short FMSTR_SIZE;   /* general size type (at least 16 bits) */
typedef unsigned char FMSTR_BOOL;    /* general boolean type  */

/* application command-specific types */
typedef unsigned char FMSTR_APPCMD_CODE;
typedef unsigned char FMSTR_APPCMD_DATA, *FMSTR_APPCMD_PDATA;
typedef unsigned char FMSTR_APPCMD_RESULT;

/* pointer to application command callback handler */
typedef FMSTR_APPCMD_RESULT (*FMSTR_PAPPCMDFUNC)(FMSTR_APPCMD_CODE,FMSTR_APPCMD_PDATA,FMSTR_SIZE);

/*****************************************************************************
* TSA-related user types and macros
******************************************************************************/

/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_tsa.h
*
* @brief  FreeMASTER Driver TSA feature
*
* @version 1.0.1.0
* 
* @date May-4-2012
* 
*******************************************************************************/

#ifndef __FREEMASTER_TSA_H
#define __FREEMASTER_TSA_H

//#include "freemaster.h"
//#include "freemaster_cfg.h"

/*****************************************************************************
 Target-side Address translation structures and macros
******************************************************************************/

/* current TSA version  */
#define FMSTR_TSA_VERSION 2U

/* TSA flags carried in TSA_ENTRY.info (except the first entry in table) */
#define FMSTR_TSA_INFO_ENTRYTYPE_MASK 0x0003U    /* flags reserved for TSA_ENTRY use */
#define FMSTR_TSA_INFO_STRUCT         0x0000U    /* ENTRYTYPE: structure parent type */
#define FMSTR_TSA_INFO_RO_VAR         0x0001U    /* ENTRYTYPE: read-only variable */
#define FMSTR_TSA_INFO_MEMBER         0x0002U    /* ENTRYTYPE: structure member */
#define FMSTR_TSA_INFO_RW_VAR         0x0003U    /* ENTRYTYPE: read-write variable */
#define FMSTR_TSA_INFO_VAR_FLAG       0x0001U    /* ENTRYTYPE: FLAG: any variable */
#define FMSTR_TSA_INFO_RWV_FLAG       0x0002U    /* ENTRYTYPE: FLAG: R/W access */

/* TSA table index and size (both unsigned, at least 16 bit wide) */
typedef FMSTR_SIZE FMSTR_TSA_TINDEX;
typedef FMSTR_SIZE FMSTR_TSA_TSIZE;

/* pointer types used in TSA tables can be overridden in freemaster.h */
/* (this is why macros are used instead of typedefs) */
#ifndef FMSTR_TSATBL_STRPTR 
#define FMSTR_TSATBL_STRPTR  const char*
#endif
#ifndef FMSTR_TSATBL_STRPTR_CAST
#define FMSTR_TSATBL_STRPTR_CAST(x) ((FMSTR_TSATBL_STRPTR)(x))
#endif
#ifndef FMSTR_TSATBL_VOIDPTR 
#define FMSTR_TSATBL_VOIDPTR const void*
#endif
#ifndef FMSTR_TSATBL_VOIDPTR_CAST 
#define FMSTR_TSATBL_VOIDPTR_CAST(x) ((FMSTR_TSATBL_VOIDPTR)(x))
#endif

/* TSA table entry. The unions inside assures variables sized enough to */
/* accomodate both the C-pointer and the user-requested size (FMSTR_ADDR) */
typedef struct
{
    union { FMSTR_TSATBL_STRPTR  p; FMSTR_ADDR n; } name;
    union { FMSTR_TSATBL_STRPTR  p; FMSTR_ADDR n; } type;
    union { FMSTR_TSATBL_VOIDPTR p; FMSTR_ADDR n; } addr;
    union { FMSTR_TSATBL_VOIDPTR p; FMSTR_ADDR n; } info;
} FMSTR_TSA_ENTRY;

/* TSA table allocation modifier */
#ifndef FMSTR_USE_TSA_INROM
#define FMSTR_USE_TSA_INROM 0
#endif
#if FMSTR_USE_TSA_INROM
#define FMSTR_TSA_CDECL const
#else
#define FMSTR_TSA_CDECL 
#endif

/*//////////////////////////////////////////////// */
/* single table-building macros */

#define FMSTR_TSA_FUNC(id)  FMSTR_TsaGetTable_##id
#define FMSTR_TSA_FUNC_PROTO(id) const FMSTR_TSA_ENTRY* FMSTR_TSA_FUNC(id) (FMSTR_TSA_TSIZE* pTableSize)

#define FMSTR_TSA_TABLE_BEGIN(id) \
    FMSTR_TSA_FUNC_PROTO(id); \
    FMSTR_TSA_FUNC_PROTO(id) { \
        static FMSTR_TSA_CDECL FMSTR_TSA_ENTRY fmstr_tsatable[] = { 

/* entry info  */
#define FMSTR_TSA_INFO1(elem, flags) FMSTR_TSATBL_VOIDPTR_CAST(((sizeof(elem))<<2)|(flags))
#define FMSTR_TSA_INFO2(size, flags) FMSTR_TSATBL_VOIDPTR_CAST(((size)<<2)|(flags))

#define FMSTR_TSA_STRUCT(name)  \
    { FMSTR_TSATBL_STRPTR_CAST(#name), FMSTR_TSATBL_STRPTR_CAST(NULL), FMSTR_TSATBL_VOIDPTR_CAST(NULL), FMSTR_TSA_INFO1(name, FMSTR_TSA_INFO_STRUCT) },
    
#define FMSTR_TSA_MEMBER(parenttype,name,type) \
    { FMSTR_TSATBL_STRPTR_CAST(#name), FMSTR_TSATBL_STRPTR_CAST(type), FMSTR_TSATBL_VOIDPTR_CAST(&((parenttype*)0)->name), FMSTR_TSA_INFO1(((parenttype*)0)->name, FMSTR_TSA_INFO_MEMBER) },
    
#define FMSTR_TSA_RO_VAR(name,type) \
    { FMSTR_TSATBL_STRPTR_CAST(#name), FMSTR_TSATBL_STRPTR_CAST(type), FMSTR_TSATBL_VOIDPTR_CAST(&(name)), FMSTR_TSA_INFO1(name, FMSTR_TSA_INFO_RO_VAR) },

#define FMSTR_TSA_RW_VAR(name,type) \
    { FMSTR_TSATBL_STRPTR_CAST(#name), FMSTR_TSATBL_STRPTR_CAST(type), FMSTR_TSATBL_VOIDPTR_CAST(&(name)), FMSTR_TSA_INFO1(name, FMSTR_TSA_INFO_RW_VAR) },

#define FMSTR_TSA_RO_MEM(name,type,addr,size) \
    { FMSTR_TSATBL_STRPTR_CAST(#name), FMSTR_TSATBL_STRPTR_CAST(type), FMSTR_TSATBL_VOIDPTR_CAST(addr), FMSTR_TSA_INFO2(size, FMSTR_TSA_INFO_RO_VAR) },

#define FMSTR_TSA_RW_MEM(name,type,addr,size) \
    { FMSTR_TSATBL_STRPTR_CAST(#name), FMSTR_TSATBL_STRPTR_CAST(type), FMSTR_TSATBL_VOIDPTR_CAST(addr), FMSTR_TSA_INFO2(size, FMSTR_TSA_INFO_RW_VAR) },

#define FMSTR_TSA_TABLE_END() }; \
    if(pTableSize) *pTableSize = sizeof(fmstr_tsatable); \
    return fmstr_tsatable; }

/*///////////////////////////////////////////////////////////////////// */
/* TSA "Base Types", all are implemented as a one-char strings */
/* retrieved by PC and parsed according to the binary scheme */
/* "111STTZZ" where TT=type[int,frac,fp,x] S=signed ZZ=size[1,2,4,8] */

#define FMSTR_TSA_UINT8   "\xE0"
#define FMSTR_TSA_UINT16  "\xE1"
#define FMSTR_TSA_UINT32  "\xE2"
#define FMSTR_TSA_UINT64  "\xE3"
#define FMSTR_TSA_SINT8   "\xF0"
#define FMSTR_TSA_SINT16  "\xF1"
#define FMSTR_TSA_SINT32  "\xF2"
#define FMSTR_TSA_SINT64  "\xF3"
#define FMSTR_TSA_UFRAC16 "\xE5"
#define FMSTR_TSA_UFRAC32 "\xE6"
#define FMSTR_TSA_FRAC16  "\xF5"
#define FMSTR_TSA_FRAC32  "\xF6"
#define FMSTR_TSA_FLOAT   "\xFA"
#define FMSTR_TSA_DOUBLE  "\xFB"

/* macro used to describe "User Type" */
#define FMSTR_TSA_USERTYPE(type) #type

/* macro used to describe pure memory space */
#define FMSTR_TSA_MEMORY NULL


/*//////////////////////////////////////////////// */
/* master TSA table-retrival building macros */

#define FMSTR_TSA_TABLE_LIST_BEGIN() \
    const FMSTR_TSA_ENTRY* FMSTR_TsaGetTable(FMSTR_TSA_TINDEX nTableIndex, FMSTR_TSA_TSIZE* pTableSize) {
        
#define FMSTR_TSA_TABLE(id) \
    if(!nTableIndex--) { \
        FMSTR_TSA_FUNC_PROTO(id); \
        return FMSTR_TSA_FUNC(id)(pTableSize); \
    } else
        
#define FMSTR_TSA_TABLE_LIST_END() \
    { return NULL; } }

/*****************************************************************************
 Target-side Address translation functions
******************************************************************************/

/* master TSA table-retrival function */
const FMSTR_TSA_ENTRY* FMSTR_TsaGetTable(FMSTR_TSA_TINDEX nTableIndex, FMSTR_TSA_TSIZE* pTableSize);

#endif /* __FREEMASTER_TSA_H */


//#include "freemaster_tsa.h"

/*****************************************************************************
* Constants
******************************************************************************/

/* application command status information  */
#define FMSTR_APPCMDRESULT_NOCMD      0xff
#define FMSTR_APPCMDRESULT_RUNNING    0xfe
#define MFSTR_APPCMDRESULT_LASTVALID  0xf7  /* F8-FF are reserved  */

/* recorder time base declaration helpers */
#define FMSTR_REC_BASE_SECONDS(x)  ((x) & 0x3fff)
#define FMSTR_REC_BASE_MILLISEC(x) (((x) & 0x3fff) | 0x4000)
#define FMSTR_REC_BASE_MICROSEC(x) (((x) & 0x3fff) | 0x8000)
#define FMSTR_REC_BASE_NANOSEC(x)  (((x) & 0x3fff) | 0xc000)

/*****************************************************************************
* Global functions 
******************************************************************************/

/* FreeMASTER serial communication API */
//void FMSTR_Poll(void);    /* polling call, use in SHORT_INTR and POLL_DRIVEN modes */

void FMSTR_Isr(void);     /* interrupt handler for LONG_INTR and SHORT_INTR modes */
void FMSTR_Isr2(void);    /* the second interrupt handler (when RX,TX vecotrs separate) */

/* Recorder API */
void FMSTR_Recorder(void);
void FMSTR_TriggerRec(void);
void FMSTR_SetUpRecBuff(FMSTR_ADDR nBuffAddr, FMSTR_SIZE nBuffSize);

/* Application commands API */
FMSTR_APPCMD_CODE  FMSTR_GetAppCmd(void);
FMSTR_APPCMD_PDATA FMSTR_GetAppCmdData(FMSTR_SIZE* pDataLen);
FMSTR_BOOL         FMSTR_RegisterAppCmdCall(FMSTR_APPCMD_CODE nAppCmdCode, FMSTR_PAPPCMDFUNC pCallbackFunc);

void FMSTR_AppCmdAck(FMSTR_APPCMD_RESULT nResultCode);
void FMSTR_AppCmdSetResponseData(FMSTR_ADDR nResultDataAddr, FMSTR_SIZE nResultDataLen);

#endif /* __FREEMASTER_H */



/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2011 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_Kxx.h
*
* @brief  FreeMASTER Driver hardware dependent stuff
*
* @version 1.0.7.0
* 
* @date Aug-18-2011
* 
*******************************************************************************/

#ifndef __FREEMASTER_KXX_H
#define __FREEMASTER_KXX_H

/******************************************************************************
 * Supress warnings generated by IAR EWBARM compiler
 ******************************************************************************/
#if defined _COMPILE_IAR
#pragma diag_suppress=Pe174
#pragma diag_suppress=Pe177
#pragma diag_suppress=Pe174
#pragma diag_suppress=Pe550
#endif

/******************************************************************************
 * platform-specific default configuration
 ******************************************************************************/

/* use 32-bit (EX) commands by default */
#ifndef FMSTR_USE_EX_CMDS
#define FMSTR_USE_EX_CMDS 1
#endif

/* do not use 16-bit (no-EX) commands by default */
#ifndef FMSTR_USE_NOEX_CMDS
#define FMSTR_USE_NOEX_CMDS 0
#endif

/* at least one of EX or no-EX command handling must be enabled */
#if !FMSTR_USE_EX_CMDS && !FMSTR_USE_NOEX_CMDS
    #error At least one of EX or no-EX command handling must be enabled (please set FMSTR_USE_EX_CMDS)
    #undef  FMSTR_USE_EX_CMDS
    #define FMSTR_USE_EX_CMDS 1
#endif

/*****************************************************************************
* Board configuration information 
******************************************************************************/

#define FMSTR_PROT_VER           3      /* protocol version 3 */
#define FMSTR_CFG_FLAGS          0      /* board info flags */
#define FMSTR_CFG_BUS_WIDTH      1      /* data bus width */
#define FMSTR_GLOB_VERSION_MAJOR 2      /* driver version */
#define FMSTR_GLOB_VERSION_MINOR 0
#define FMSTR_IDT_STRING "Kxx FreeMASTER"
#define FMSTR_TSA_FLAGS          0

/******************************************************************************
* platform-specific types
******************************************************************************/

typedef unsigned char  FMSTR_U8;         /* smallest memory entity */
typedef unsigned short FMSTR_U16;        /* 16bit value */
typedef unsigned long  FMSTR_U32;        /* 32bit value */

typedef signed char    FMSTR_S8;         /* signed 8bit value */
typedef signed short   FMSTR_S16;        /* signed 16bit value */
typedef signed long    FMSTR_S32;        /* signed 32bit value */

#if FMSTR_REC_FLOAT_TRIG
typedef float          FMSTR_FLOAT;        /* float value */
#endif

typedef unsigned char  FMSTR_FLAGS;      /* type to be union-ed with flags (at least 8 bits) */
typedef unsigned char  FMSTR_SIZE8;      /* one-byte size value */
typedef signed short   FMSTR_INDEX;      /* general for-loop index (must be signed) */

typedef unsigned char  FMSTR_BCHR;       /* type of a single character in comm.buffer */
typedef unsigned char* FMSTR_BPTR;       /* pointer within a communication buffer */

typedef unsigned char  FMSTR_SCISR;      /* data type to store SCI status register */

/******************************************************************************
* communication buffer access functions
******************************************************************************/

void FMSTR_CopyMemory(FMSTR_ADDR nDestAddr, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_CopyToBuffer(FMSTR_BPTR pDestBuff, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_CopyFromBuffer(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize);
void FMSTR_CopyFromBufferWithMask(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize);

/* mixed EX and non-EX commands may occur */
#if FMSTR_USE_EX_CMDS && FMSTR_USE_NOEX_CMDS || (FMSTR_BUFFER_ACCESS_BY_FUNCT)
void FMSTR_SetExAddr(FMSTR_BOOL bNextAddrIsEx);
#else
/* otherwise, we always know what addresses are used, (ignore FMSTR_SetExAddr) */
#define FMSTR_SetExAddr(bNextAddrIsEx) 
#endif

#if (FMSTR_BUFFER_ACCESS_BY_FUNCT)
FMSTR_BPTR FMSTR_ValueFromBuffer16(FMSTR_U16* pDest, FMSTR_BPTR pSrc);
FMSTR_BPTR FMSTR_ValueFromBuffer32(FMSTR_U32* pDest, FMSTR_BPTR pSrc);
FMSTR_BPTR FMSTR_ValueToBuffer16(FMSTR_BPTR pDest, FMSTR_U16 src);
FMSTR_BPTR FMSTR_ValueToBuffer32(FMSTR_BPTR pDest, FMSTR_U32 src);
#endif

/*********************************************************************************
* communication buffer access functions. Most of them are trivial simple on KXX
*********************************************************************************/

#define FMSTR_ValueFromBuffer8(pDest, pSrc) \
    ( (*((FMSTR_U8*)(pDest)) = *(FMSTR_U8*)(pSrc)), (((FMSTR_BPTR)(pSrc))+1) )

#if !(FMSTR_BUFFER_ACCESS_BY_FUNCT)
#define FMSTR_ValueFromBuffer16(pDest, pSrc) \
    ( (*((FMSTR_U16*)(pDest)) = *(FMSTR_U16*)(pSrc)), (((FMSTR_BPTR)(pSrc))+2) )

#define FMSTR_ValueFromBuffer32(pDest, pSrc) \
    ( (*((FMSTR_U32*)(pDest)) = *(FMSTR_U32*)(pSrc)), (((FMSTR_BPTR)(pSrc))+4) )
#endif

#define FMSTR_ValueToBuffer8(pDest, src) \
    ( (*((FMSTR_U8*)(pDest)) = (FMSTR_U8)(src)), (((FMSTR_BPTR)(pDest))+1) )

#if !(FMSTR_BUFFER_ACCESS_BY_FUNCT)
#define FMSTR_ValueToBuffer16(pDest, src) \
    ( (*((FMSTR_U16*)(pDest)) = (FMSTR_U16)(src)), (((FMSTR_BPTR)(pDest))+2) )

#define FMSTR_ValueToBuffer32(pDest, src) \
    ( (*((FMSTR_U32*)(pDest)) = (FMSTR_U32)(src)), (((FMSTR_BPTR)(pDest))+4) )
#endif

#define FMSTR_SkipInBuffer(pDest, nSize) \
    ( ((FMSTR_BPTR)(pDest)) + (nSize) )


#define FMSTR_ConstToBuffer8  FMSTR_ValueToBuffer8
#define FMSTR_ConstToBuffer16 FMSTR_ValueToBuffer16

/* EX address used only: fetching 32bit word */
#if FMSTR_USE_EX_CMDS && !FMSTR_USE_NOEX_CMDS && !(FMSTR_BUFFER_ACCESS_BY_FUNCT)
    #define FMSTR_AddressFromBuffer(pDest, pSrc) \
        FMSTR_ValueFromBuffer32(pDest, pSrc)
    #define FMSTR_AddressToBuffer(pDest, nAddr) \
        FMSTR_ValueToBuffer32(pDest, nAddr)
        
/* no-EX address used only: fetching 16bit word  */
#elif !FMSTR_USE_EX_CMDS && FMSTR_USE_NOEX_CMDS && !(FMSTR_BUFFER_ACCESS_BY_FUNCT)
    #define FMSTR_AddressFromBuffer(pDest, pSrc) \
        FMSTR_ValueFromBuffer16(pDest, pSrc)
    #define FMSTR_AddressToBuffer(pDest, nAddr) \
        FMSTR_ValueToBuffer16(pDest, nAddr)
        
/* mixed addresses used, need to process it programatically */
#else
    FMSTR_BPTR FMSTR_AddressFromBuffer(FMSTR_ADDR* pAddr, FMSTR_BPTR pSrc);
    FMSTR_BPTR FMSTR_AddressToBuffer(FMSTR_BPTR pDest, FMSTR_ADDR nAddr);
#endif

#define FMSTR_GetS8(addr)  ( *(FMSTR_S8*)(addr) )
#define FMSTR_GetU8(addr)  ( *(FMSTR_U8*)(addr) )
#define FMSTR_GetS16(addr) ( *(FMSTR_S16*)(addr) )
#define FMSTR_GetU16(addr) ( *(FMSTR_U16*)(addr) )
#define FMSTR_GetS32(addr) ( *(FMSTR_S32*)(addr) )
#define FMSTR_GetU32(addr) ( *(FMSTR_U32*)(addr) )

#if FMSTR_REC_FLOAT_TRIG
#define FMSTR_GetFloat(addr) ( *(FMSTR_FLOAT*)(addr) )
#endif

/****************************************************************************************
* Other helper macros
*****************************************************************************************/

/* This macro assigns C pointer to FMSTR_ADDR-typed variable */
#define FMSTR_PTR2ADDR(tmpAddr,ptr) ( tmpAddr = (FMSTR_ADDR) (FMSTR_U8*) ptr )
#define FMSTR_ARR2ADDR FMSTR_PTR2ADDR

/****************************************************************************************
* Platform-specific configuration
*****************************************************************************************/

/* FlexCAN functionality tested on KXX */
#define FMSTR_CANHW_FLEXCAN 1 

/****************************************************************************************
* General peripheral space access macros
*****************************************************************************************/

#define FMSTR_SETREG8(base, offset, value)    (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG8(base, offset)           (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETBIT8(base, offset, bit)      (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)) |= bit)
#define FMSTR_CLRBIT8(base, offset, bit)      (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)) &= (FMSTR_U16)~((FMSTR_U16)(bit)))
#define FMSTR_SETREG16(base, offset, value)   (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG16(base, offset)          (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETBIT16(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) |= bit)
#define FMSTR_CLRBIT16(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) &= (FMSTR_U16)~((FMSTR_U16)(bit)))
#define FMSTR_TSTBIT16(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) & (bit))
#define FMSTR_SETREG32(base, offset, value)   (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG32(base, offset)          (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETBIT32(base, offset, bit)     ((*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset))) |= bit)
#define FMSTR_CLRBIT32(base, offset, bit)     ((*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset))) &= ~(bit))
#define FMSTR_TSTBIT32(base, offset, bit)     (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)) & (bit))

/****************************************************************************************
* SCI module constants
*****************************************************************************************/

/* SCI module registers */
#define FMSTR_SCIBDH_OFFSET 0
#define FMSTR_SCIBDL_OFFSET 1
#define FMSTR_SCIC1_OFFSET 2
#define FMSTR_SCIC2_OFFSET 3
#define FMSTR_SCIS1_OFFSET 4
#define FMSTR_SCIS2_OFFSET 5
#define FMSTR_SCIC3_OFFSET 6
#define FMSTR_SCIDR_OFFSET 7

/* SCI Control Register bits */
#define FMSTR_SCIC1_LOOPS     0x80
#define FMSTR_SCIC1_SWAI      0x40
#define FMSTR_SCIC1_RSRC      0x20
#define FMSTR_SCIC1_M         0x10
#define FMSTR_SCIC1_WAKE      0x08
#define FMSTR_SCIC1_ILT       0x04
#define FMSTR_SCIC1_PE        0x02
#define FMSTR_SCIC1_PT        0x01
#define FMSTR_SCIC2_TIE       0x80
#define FMSTR_SCIC2_TCIE      0x40
#define FMSTR_SCIC2_RIE       0x20
#define FMSTR_SCIC2_ILIE      0x10
#define FMSTR_SCIC2_TE        0x08
#define FMSTR_SCIC2_RE        0x04
#define FMSTR_SCIC2_RWU       0x02
#define FMSTR_SCIC2_SBK       0x01

/* SCI Status registers bits */
#define FMSTR_SCISR_TDRE       0x80
#define FMSTR_SCISR_TC         0x40
#define FMSTR_SCISR_RDRF       0x20
#define FMSTR_SCISR_IDLE       0x10
#define FMSTR_SCISR_OR         0x08
#define FMSTR_SCISR_NF         0x04
#define FMSTR_SCISR_FE         0x02
#define FMSTR_SCISR_PF         0x01
#define FMSTR_SCISR2_BRK13     0x04
#define FMSTR_SCISR2_TXDIR     0x02
#define FMSTR_SCISR2_RAF       0x01

/*******************************************************************************************
* SCI access macros
*****************************************************************************************/

/* transmitter enable/disable */
#define FMSTR_SCI_TE() FMSTR_SETBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_TE)
#define FMSTR_SCI_TD() FMSTR_CLRBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_TE)

/* receiver enable/disable */
#define FMSTR_SCI_RE() FMSTR_SETBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_RE)
#define FMSTR_SCI_RD() FMSTR_CLRBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_RE)

#define FMSTR_SCI_TE_RE() FMSTR_SETBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_RE | FMSTR_SCIC2_TE)

/* Transmitter-empty interrupt enable/disable */
#define FMSTR_SCI_ETXI() FMSTR_SETBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_TIE)
#define FMSTR_SCI_DTXI() FMSTR_CLRBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_TIE)

/* Receiver-full interrupt enable/disable */
#define FMSTR_SCI_ERXI() FMSTR_SETBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_RIE)
#define FMSTR_SCI_DRXI() FMSTR_CLRBIT8(FMSTR_SCI_BASE, FMSTR_SCIC2_OFFSET, FMSTR_SCIC2_RIE)

/* Tranmsit character */
#define FMSTR_SCI_PUTCHAR(ch) FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_SCIDR_OFFSET, ch)

/* Get received character */
#define FMSTR_SCI_GETCHAR() FMSTR_GETREG8(FMSTR_SCI_BASE, FMSTR_SCIDR_OFFSET)

/* read status register */
#define FMSTR_SCI_GETSR()   FMSTR_GETREG8(FMSTR_SCI_BASE, FMSTR_SCIS1_OFFSET)

/* read & clear status register */
#define FMSTR_SCI_RDCLRSR() FMSTR_GETREG8(FMSTR_SCI_BASE, FMSTR_SCIS1_OFFSET)

/****************************************************************************************
* FCAN module constants
*****************************************************************************************/

/* FCAN module MB CODEs */
#define FMSTR_FCANMB_CODE_MASK      0x0F   /* defines mask of codes */

#define FMSTR_FCANMB_CRXVOID        0x00   /* buffer void after received data read-out */
#define FMSTR_FCANMB_CRXEMPTY       0x04   /* active and empty */

#define FMSTR_FCANMB_CTXTRANS_ONCE  0x0C   /* Initialize transmitting data from buffer */
#define FMSTR_FCANMB_CTXREADY       0x08   /* Message buffer not ready for transmit */

/* FCAN module registers offsets */
#define FMSTR_FCANTMR_OFFSET   0x08
#define FMSTR_FCANIER2_OFFSET  0x24
#define FMSTR_FCANIER1_OFFSET  0x28
#define FMSTR_FCANIFR2_OFFSET  0x2C
#define FMSTR_FCANIFR1_OFFSET  0x30
#define FMSTR_FCANRXFG_OFFSET  (0x80 + ((FMSTR_FLEXCAN_RXMB) * 0x10))
#define FMSTR_FCANTXFG_OFFSET  (0x80 + ((FMSTR_FLEXCAN_TXMB) * 0x10))

/* FCAN MB registers offsets (must also add FCANxxFG_OFFSET) */
#define FMSTR_FCMBCSR   0x00
#define FMSTR_FCMBIDR0  0x04
#define FMSTR_FCMBIDR1  0x05
#define FMSTR_FCMBIDR2  0x06
#define FMSTR_FCMBIDR3  0x07
#define FMSTR_FCMBDSR0  0x0B
#define FMSTR_FCMBDSR1  0x0A
#define FMSTR_FCMBDSR2  0x09
#define FMSTR_FCMBDSR3  0x08
#define FMSTR_FCMBDSR4  0x0F
#define FMSTR_FCMBDSR5  0x0E
#define FMSTR_FCMBDSR6  0x0D
#define FMSTR_FCMBDSR7  0x0C

/* FCAN CANMSCSR */
#define FMSTR_FCANCTRL_IDE     0x20
#define FMSTR_FCANCTRL_STD_RTR 0x10
#define FMSTR_FCANCTRL_EXT_RTR 0x10
#define FMSTR_FCANCTRL_EXT_SRR 0x40

/* FCAN ID flags */
#define FMSTR_FCANID0_EXT_FLG  0x80

/* FCAN: enable/disable CAN RX/TX interrupts */
#define FMSTR_FCAN_ETXI() ( ((FMSTR_FLEXCAN_TXMB)&0x20) ? \
                            FMSTR_SETBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER2_OFFSET, (1<<((FMSTR_FLEXCAN_TXMB)-32))):\
                            FMSTR_SETBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER1_OFFSET, (1<<(FMSTR_FLEXCAN_TXMB))) )
#define FMSTR_FCAN_DTXI() ( ((FMSTR_FLEXCAN_TXMB)&0x20) ? \
                            FMSTR_CLRBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER2_OFFSET, (1<<((FMSTR_FLEXCAN_TXMB)-32))):\
                            FMSTR_CLRBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER1_OFFSET, (1<<(FMSTR_FLEXCAN_TXMB))) )
#define FMSTR_FCAN_ERXI() ( ((FMSTR_FLEXCAN_RXMB)&0x20) ? \
                            FMSTR_SETBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER2_OFFSET, (1<<((FMSTR_FLEXCAN_RXMB)-32))):\
                            FMSTR_SETBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER1_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB))) )
#define FMSTR_FCAN_DRXI() ( ((FMSTR_FLEXCAN_RXMB)&0x20) ? \
                            FMSTR_CLRBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER2_OFFSET, (1<<((FMSTR_FLEXCAN_RXMB)-32))):\
                            FMSTR_CLRBIT32(FMSTR_CAN_BASE, FMSTR_FCANIER1_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB))) )

/* FCAN: read RX status register */
#define FMSTR_FCAN_TEST_RXFLG() ( ((FMSTR_FLEXCAN_RXMB)&0x20) ? \
                            FMSTR_TSTBIT32(FMSTR_CAN_BASE, FMSTR_FCANIFR2_OFFSET, (1<<((FMSTR_FLEXCAN_RXMB)-32))):\
                            FMSTR_TSTBIT32(FMSTR_CAN_BASE, FMSTR_FCANIFR1_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB))) )
#define FMSTR_FCAN_CLEAR_RXFLG() ( ((FMSTR_FLEXCAN_RXMB)&0x20) ? \
                            FMSTR_SETREG32(FMSTR_CAN_BASE, FMSTR_FCANIFR2_OFFSET, (1<<((FMSTR_FLEXCAN_RXMB)-32))):\
                            FMSTR_SETREG32(FMSTR_CAN_BASE, FMSTR_FCANIFR1_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB))) )

/* FCAN: read TX status register */
#define FMSTR_FCAN_TEST_TXFLG() ( ((FMSTR_FLEXCAN_TXMB)&0x20) ? \
                            FMSTR_TSTBIT32(FMSTR_CAN_BASE, FMSTR_FCANIFR2_OFFSET, (1<<((FMSTR_FLEXCAN_TXMB)-32))):\
                            FMSTR_TSTBIT32(FMSTR_CAN_BASE, FMSTR_FCANIFR1_OFFSET, (1<<(FMSTR_FLEXCAN_TXMB))) )

/* FCAN: read TX MB status register */
#define FMSTR_FCAN_GET_MBSTATUS() (FMSTR_GETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBCSR + 3)&FMSTR_FCANMB_CODE_MASK)

/* FCAN: id to idr translation */
#define FMSTR_FCAN_MAKEIDR0(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? ((((id)>>24)&0x1f) | FMSTR_FCANID0_EXT_FLG) : (((id)>>6)&0x1f) ))
#define FMSTR_FCAN_MAKEIDR1(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? ((id)>>16) : ((id)<<2) ))
#define FMSTR_FCAN_MAKEIDR2(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? ((id)>>8) : 0 ))
#define FMSTR_FCAN_MAKEIDR3(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? (id) : 0 ))

/* FCAN reception, configuring the buffer, just once at the initialization phase */
#define FMSTR_FCAN_RINIT(idr0, idr1, idr2, idr3) \
    FMSTR_MACROCODE_BEGIN() \
        (((idr0)&FMSTR_FCANID0_EXT_FLG) ? \
        (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBCSR + 2, (FMSTR_FCANMB_CRXVOID<<8 | FMSTR_FCANCTRL_IDE | FMSTR_FCANCTRL_EXT_SRR))) : \
        (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBCSR + 2, (FMSTR_FCANMB_CRXVOID<<8 |  FMSTR_FCANCTRL_EXT_SRR))));\
        FMSTR_SETREG32(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBIDR0, ((idr0)<<24) | ((idr1)<<16) | ((idr2)<<8) | (idr3) );\
    FMSTR_MACROCODE_END()

/* FCAN transmission, configuring the buffer, just once at the initialization phase */
#define FMSTR_FCAN_TINIT(idr0, idr1, idr2, idr3) \
    FMSTR_MACROCODE_BEGIN() \
      (((idr0)&FMSTR_FCANID0_EXT_FLG) ? \
      (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBCSR + 2, (FMSTR_FCANMB_CTXREADY<<8 | FMSTR_FCANCTRL_IDE))) : \
      (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBCSR + 2, (FMSTR_FCANMB_CTXREADY<<8 ))));\
    FMSTR_MACROCODE_END()

/* FCAN reception, configuring the buffer for receiving (each time receiver is re-enabled) */
#define FMSTR_FCAN_RCFG() \
    FMSTR_SETREG8(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBCSR + 3, FMSTR_FCANMB_CRXEMPTY)

/* FCAN: CAN transmission */
typedef struct
{
    FMSTR_U8 nDataIx;
} FMSTR_FCAN_TCTX;

/* FCAN transmission, put one data byte into buffer */
#define FMSTR_FCAN_TLEN(pctx, len) \
    FMSTR_SETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET+FMSTR_FCMBCSR+2, (FMSTR_U8)((len & 0x0f) | \
        (FMSTR_GETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET+FMSTR_FCMBCSR+2)&(FMSTR_FCANCTRL_IDE | FMSTR_FCANCTRL_EXT_SRR | FMSTR_FCANCTRL_EXT_RTR))))

/* FCAN transmission, put one data byte into buffer */
#define FMSTR_FCAN_PUTBYTE(pctx, dataByte) \
    FMSTR_MACROCODE_BEGIN() \
        FMSTR_SETREG8(FMSTR_CAN_BASE, (FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBDSR0) - (0x3&((pctx)->nDataIx)) + (0x4&((pctx)->nDataIx)), (dataByte) ); \
        (pctx)->nDataIx++; \
    FMSTR_MACROCODE_END()

/* FCAN: CAN transmission, configuring the buffer before each transmission */
#define FMSTR_FCAN_TCFG(pctx) \
    FMSTR_MACROCODE_BEGIN() \
        (pctx)->nDataIx = 0; \
    FMSTR_MACROCODE_END()

/* FCAN: CAN transmission, preparing the buffer before each transmission */
#define FMSTR_FCAN_TID(pctx, idr0, idr1, idr2, idr3) \
    FMSTR_MACROCODE_BEGIN() \
        FMSTR_SETREG32(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET+FMSTR_FCMBIDR0, ((idr0)<<24) | ((idr1)<<16) | ((idr2)<<8) | (idr3) ); \
    FMSTR_MACROCODE_END()

/* FCAN transmission, set transmit priority */
#define FMSTR_FCAN_TPRI(pctx, txPri) /* in FCAN module is not implemented */

/* FCAN transmission, final firing of the buffer */
#define FMSTR_FCAN_TX(pctx) \
        FMSTR_SETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBCSR + 3, (FMSTR_FCANMB_CTXTRANS_ONCE & 0x0f) )

/* FCAN reception */
typedef struct
{
    FMSTR_U8 nDataIx;
} FMSTR_FCAN_RCTX;

/* FCAN reception, lock frame */
#define FMSTR_FCAN_RX(pctx) \
    (pctx)->nDataIx = 0;

/* FCAN reception, test if received message ID matches the one given, TRUE if matching */
#define FMSTR_FCAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3) \
    ( (idr0 & FMSTR_FCANID0_EXT_FLG) ? \
        /* ext id compare */ \
        ( ((((idr0)<<24) | ((idr1)<<16) | ((idr2)<<8) | (idr3))&0x1f000000)==((FMSTR_GETREG32(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET+FMSTR_FCMBIDR0))&0x1f000000) ) : \
        /* std id compare */ \
        ( (((idr0)<<8) | (idr1))==((FMSTR_GETREG16(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET+FMSTR_FCMBIDR0+2))&0x1ffc) ) )

/* FCAN reception, get received frame length */
#define FMSTR_FCAN_RLEN(pctx) \
    (FMSTR_GETREG8(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET+FMSTR_FCMBCSR+2) & 0x0f)

/* FCAN reception, get one received byte */
#define FMSTR_FCAN_GETBYTE(pctx) \
        ((FMSTR_U8) (FMSTR_GETREG8(FMSTR_CAN_BASE, (FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBDSR0) - (0x3&((pctx)->nDataIx)) + (0x4&((pctx)->nDataIx)) ))); \
        (pctx)->nDataIx++

/* FCAN reception, unlock the buffer */
#define FMSTR_FCAN_RFINISH(pctx) \
    FMSTR_SETBIT16(FMSTR_CAN_BASE, FMSTR_FCANTMR_OFFSET, 0)

#endif /* __FREEMASTER_KXX_H */


//#include "freemaster.h"
//#include "freemaster_private.h"
//#include "freemaster_protocol.h"

#if defined FREEMASTER_UART

/***********************************
*  local variables 
***********************************/
 

/* FreeMASTER runtime flags */
/*lint -e{960} using union */
typedef volatile union 
{
    FMSTR_FLAGS all;
    
    struct
    {
        unsigned bTxActive : 1;         /* response is being transmitted */
        unsigned bTxWaitTC : 1;         /* response sent, wait for transmission complete */
        unsigned bTxLastCharSOB : 1;    /* last transmitted char was equal to SOB  */
        unsigned bRxLastCharSOB : 1;    /* last received character was SOB */
        unsigned bRxMsgLengthNext : 1;  /* expect the length byte next time */
        unsigned bJtagRIEPending : 1;   /* JTAG RIE bit failed to be set, try again later */
    } flg;
    
} FMSTR_SERIAL_FLAGS;

static FMSTR_SERIAL_FLAGS pcm_wFlags;

/* receive and transmit buffers and counters */
//static FMSTR_SIZE8 pcm_nTxTodo;     /* transmission to-do counter (0 when tx is idle) */
static FMSTR_SIZE8 pcm_nRxTodo;     /* reception to-do counter (0 when rx is idle) */
//static FMSTR_BPTR  pcm_pTxBuff;     /* pointer to next byte to transmit */
static FMSTR_BPTR  pcm_pRxBuff;     /* pointer to next free place in RX buffer */
static FMSTR_BCHR  pcm_nRxCheckSum; /* checksum of data being received */

/* SHORT_INTR receive queue (circular buffer) */
#if FMSTR_SHORT_INTR
static FMSTR_BCHR  pcm_pRQueueBuffer[FMSTR_COMM_RQUEUE_SIZE];
static FMSTR_BPTR  pcm_pRQueueRP;   /* SHORT_INTR queue read-pointer */
static FMSTR_BPTR  pcm_pRQueueWP;   /* SHORT_INTR queue write-pointer */
#endif

#if FMSTR_USE_JTAG
static FMSTR_U32 pcm_wJtagTxData;   /* four bytes buffer to be sent over JTAG (LSB first) */
static FMSTR_SIZE8 pcm_wJtagTxCtr;  /* counter of bytes in pcm_wJtagTxData */
#endif

/***********************************
*  local function prototypes
***********************************/

static void FMSTR_Listen(void);
static void FMSTR_SendError(QUEUE_HANDLE FreeMasterPort, FMSTR_BCHR nErrCode);



/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_private.h
*
* @brief  FreeMASTER driver private declarations, used internally by the driver
*
* @version 1.0.1.0
* 
* @date May-4-2012
* 
*******************************************************************************/

#ifndef __FREEMASTER_PRIVATE_H
#define __FREEMASTER_PRIVATE_H

#ifndef __FREEMASTER_H
#error Please include the freemaster.h master header file before the freemaster_private.h
#endif

//#include "freemaster.h"
//#include "freemaster_cfg.h"

#if FMSTR_USE_MQX_IO
    /* include MQX headers */
    #include <mqx.h>
    #include <bsp.h>
#endif


/******************************************************************************
* Platform-dependent types, macros and functions
******************************************************************************/

#undef FMSTR_PLATFORM

/* platform macro FMSTR_PLATFORM_xxxxxxx should be defined  */
/* as "non-zero" in the "freemaster.h" file */

#if defined(FMSTR_PLATFORM_56F8xx)
    #if FMSTR_PLATFORM_56F8xx
    #include "freemaster_56F8xx.h"
    #define FMSTR_PLATFORM 56F8xx
    #else
    #undef FMSTR_PLATFORM_56F8xx
    #endif
#endif

#if defined(FMSTR_PLATFORM_56F8xxx)
    #if FMSTR_PLATFORM_56F8xxx
    #include "freemaster_56F8xxx.h"
    #define FMSTR_PLATFORM 56F8xxx
    #else
    #undef FMSTR_PLATFORM_56F8xxx
    #endif
#endif

#if defined(FMSTR_PLATFORM_HC12)
    #if FMSTR_PLATFORM_HC12
    #include "freemaster_HC12.h"
    #define FMSTR_PLATFORM HC12
    #else
    #undef FMSTR_PLATFORM_HC12
    #endif
#endif

#if defined(FMSTR_PLATFORM_HC08)
    #if FMSTR_PLATFORM_HC08
    #include "freemaster_HC08.h"
    #define FMSTR_PLATFORM HC08
    #else
    #undef FMSTR_PLATFORM_HC08
    #endif
#endif

#if defined(FMSTR_PLATFORM_MPC55xx)
    #if FMSTR_PLATFORM_MPC55xx
    #include "freemaster_MPC55xx.h"
    #define FMSTR_PLATFORM MPC55xx
    #else
    #undef FMSTR_PLATFORM_MPC55xx
    #endif
#endif

#if defined(FMSTR_PLATFORM_MPC5xx)
    #if FMSTR_PLATFORM_MPC5xx
    #include "freemaster_MPC5xx.h"
    #define FMSTR_PLATFORM MPC5xx
    #else
    #undef FMSTR_PLATFORM_MPC5xx
    #endif
#endif

#if defined(FMSTR_PLATFORM_MCF51xx)
    #if FMSTR_PLATFORM_MCF51xx
    #include "freemaster_MCF51xx.h"
    #define FMSTR_PLATFORM MCF51xx
    #else
    #undef FMSTR_PLATFORM_MCF51xx
    #endif
#endif

#if defined(FMSTR_PLATFORM_MCF52xx)
    #if FMSTR_PLATFORM_MCF52xx
    #include "freemaster_MCF52xx.h"
    #define FMSTR_PLATFORM MCF52xx
    #else
    #undef FMSTR_PLATFORM_MCF52xx
    #endif
#endif

#if defined(FMSTR_PLATFORM_MPC56xx)
    #if FMSTR_PLATFORM_MPC56xx
    #include "freemaster_MPC56xx.h"
    #define FMSTR_PLATFORM MPC56xx
    #else
    #undef FMSTR_PLATFORM_MPC56xx
    #endif
#endif

#if defined(FMSTR_PLATFORM_MQX)
    #if FMSTR_PLATFORM_MQX
    #include "freemaster_MQX.h"
    #define FMSTR_PLATFORM MQX
    #else
    #undef FMSTR_PLATFORM_MQX
    #endif
#endif

#if defined(FMSTR_PLATFORM_KXX)
    #if FMSTR_PLATFORM_KXX
//    #include "freemaster_Kxx.h"
    #define FMSTR_PLATFORM KXX
    #else
    #undef FMSTR_PLATFORM_KXX
    #endif
#endif

#ifndef FMSTR_PLATFORM
#error Unknown FreeMASTER driver platform
#endif

/******************************************************************************
* NULL needed
******************************************************************************/

#ifndef NULL
#define NULL ((void *) 0)
#endif

/******************************************************************************
* Boolean values
******************************************************************************/

#ifndef FMSTR_TRUE
#define FMSTR_TRUE (1U)
#endif

#ifndef FMSTR_FALSE
#define FMSTR_FALSE (0U)
#endif

/******************************************************************************
* CAN-related constants
******************************************************************************/

#ifdef FMSTR_CAN_EXTID 
#if FMSTR_CAN_EXTID != 0x80000000U
#error FMSTR_CAN_EXTID must be defined as 0x80000000
#undef FMSTR_CAN_EXTID 
#endif
#endif

#ifndef FMSTR_CAN_EXTID
#define FMSTR_CAN_EXTID 0x80000000U
#endif

/******************************************************************************
* inline functions 
******************************************************************************/

/* we do not assume the inline is always supported by compiler
  rather each platform header defines its FMSTR_INLINE */
#ifndef FMSTR_INLINE
#define FMSTR_INLINE static
#endif

/* building macro-based inline code */
#define FMSTR_MACROCODE_BEGIN()     do{
#define FMSTR_MACROCODE_END()       }while(0)

/******************************************************************************
* Global non-API functions (used internally in FreeMASTER driver)
******************************************************************************/

void FMSTR_InitSerial(void);
FMSTR_BOOL FMSTR_InitMQX(void);
void FMSTR_SendResponse(QUEUE_HANDLE FreeMasterPort, FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nLength);
void FMSTR_ProcessSCI(void);
void FMSTR_ProcessJTAG(void);

FMSTR_BOOL FMSTR_ProtocolDecoder(QUEUE_HANDLE FreeMasterPort, FMSTR_BPTR pMessageIO);
//FMSTR_BPTR FMSTR_GetBoardInfo(FMSTR_BPTR pMessageIO);

FMSTR_BPTR FMSTR_ReadMem(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_ReadVar(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_WriteMem(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_WriteVar(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_WriteVarMask(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_WriteMemMask(FMSTR_BPTR pMessageIO);

void FMSTR_InitAppCmds(void);
FMSTR_BPTR FMSTR_StoreAppCmd(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetAppCmdStatus(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetAppCmdRespData(FMSTR_BPTR pMessageIO);

void FMSTR_InitScope(void);
FMSTR_BPTR FMSTR_SetUpScope(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_ReadScope(FMSTR_BPTR pMessageIO);

void FMSTR_InitRec(void);
FMSTR_BPTR FMSTR_SetUpRec(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_StartRec(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_StopRec(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetRecStatus(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetRecBuff(FMSTR_BPTR pMessageIO);
FMSTR_BOOL FMSTR_IsInRecBuffer(FMSTR_ADDR nAddr, FMSTR_SIZE8 nSize);
FMSTR_SIZE FMSTR_GetRecBuffSize(void);

void FMSTR_InitTsa(void);
FMSTR_BPTR FMSTR_GetTsaInfo(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetStringLen(FMSTR_BPTR pMessageIO);
FMSTR_BOOL FMSTR_CheckTsaSpace(FMSTR_ADDR nAddr, FMSTR_SIZE8 nSize, FMSTR_BOOL bWriteAccess);
FMSTR_U16  FMSTR_StrLen(FMSTR_ADDR nAddr);

void FMSTR_InitSfio(void);
FMSTR_BPTR FMSTR_SfioFrame(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_SfioGetResp(FMSTR_BPTR pMessageIO);

void FMSTR_InitPipes(void);
FMSTR_BPTR FMSTR_PipeFrame(FMSTR_BPTR pMessageIO);

void FMSTR_InitCan(void);
void FMSTR_SetCanCmdID(FMSTR_U32 canID);
void FMSTR_SetCanRespID(FMSTR_U32 canID);
FMSTR_BOOL FMSTR_TxCan(void);
FMSTR_BOOL FMSTR_RxCan(void);
void FMSTR_ProcessCanRx(void);
void FMSTR_ProcessCanTx(void);

/****************************************************************************************
* Potentialy unused variable declaration
*****************************************************************************************/

#ifdef  _lint
#define FMSTR_UNUSED(sym) /*lint -esym(715,sym) -esym(818,sym) -esym(529,sym) -e{960} */
#else
#define FMSTR_UNUSED(sym) ((sym),0)
#endif

/******************************************************************************
* Configuration check 
******************************************************************************/

/* polling mode as default when nothing selected */
#if !defined(FMSTR_POLL_DRIVEN) && !defined(FMSTR_LONG_INTR) && !defined(FMSTR_SHORT_INTR)
    #define FMSTR_LONG_INTR   0
    #define FMSTR_SHORT_INTR  0
    #define FMSTR_POLL_DRIVEN 1
#endif

/* otherwise, "undefined" means false for all three options */
#ifndef FMSTR_POLL_DRIVEN
#define FMSTR_POLL_DRIVEN 0
#endif
#ifndef FMSTR_LONG_INTR
#define FMSTR_LONG_INTR 0
#endif
#ifndef FMSTR_SHORT_INTR
#define FMSTR_SHORT_INTR 0
#endif

#if (FMSTR_LONG_INTR && (FMSTR_SHORT_INTR || FMSTR_POLL_DRIVEN)) || \
    (FMSTR_SHORT_INTR && (FMSTR_LONG_INTR || FMSTR_POLL_DRIVEN)) || \
    (FMSTR_POLL_DRIVEN && (FMSTR_LONG_INTR || FMSTR_SHORT_INTR)) || \
    !(FMSTR_POLL_DRIVEN || FMSTR_LONG_INTR || FMSTR_SHORT_INTR)
    /* mismatch in interrupt modes, only one can be selected */
    #error You have to enable exctly one of FMSTR_LONG_INTR or FMSTR_SHORT_INTR or FMSTR_POLL_DRIVEN
#endif

#if FMSTR_SHORT_INTR
    /* default short-interrupt FIFO size */
    #ifndef FMSTR_COMM_RQUEUE_SIZE
    #define FMSTR_COMM_RQUEUE_SIZE 32
    #endif

    #if !FMSTR_COMM_RQUEUE_SIZE
    #undef  FMSTR_COMM_RQUEUE_SIZE
    #define FMSTR_COMM_RQUEUE_SIZE 32
    #endif
    
    #if FMSTR_COMM_RQUEUE_SIZE < 1
    #error Error in FMSTR_COMM_RQUEUE_SIZE value.
    #endif
#endif

/* select JTAG, SCI or CAN interface */
/* one of these is typically already enabled in freemaster_cfg.h */
#ifndef FMSTR_USE_JTAG
#define FMSTR_USE_JTAG 0
#endif

#ifndef FMSTR_USE_MSCAN
    #define FMSTR_USE_MSCAN 0
#endif

#ifndef FMSTR_USE_FLEXCAN
    #define FMSTR_USE_FLEXCAN 0
#endif

#ifndef FMSTR_USE_CAN
    #if ((FMSTR_USE_MSCAN) || (FMSTR_USE_FLEXCAN))
    #define FMSTR_USE_CAN 1
    #else
  //#define FMSTR_USE_CAN 0
    #endif
#endif

#ifndef FMSTR_USE_MQX_IO
    #define FMSTR_USE_MQX_IO 0
#endif




/* SCI does not have a transmission double buffer (kind of queue) */
/* (if not specified differently in platform-dependent header file) */
#if defined FREEMASTER_UART
    #ifndef FMSTR_SCI_BASE
    #error FMSTR_SCI_BASE must be defined when using SCI interface
    #endif

    #ifndef FMSTR_SCI_HAS_TXQUEUE
    #define FMSTR_SCI_HAS_TXQUEUE 0
    #endif
    
    #ifndef FMSTR_SCI_TE_RE
    #define FMSTR_SCI_TE_RE() do {FMSTR_SCI_TE(); FMSTR_SCI_RE();} while(0)
    #endif

#endif

/* CAN is MSCAN or FLEXCAN */
#if FMSTR_USE_CAN
   
    /* if CANHW is not specified in freemaster.h, the platform is not yet tested with CAN */
    #if (!defined(FMSTR_CANHW_MSCAN)) && FMSTR_USE_MSCAN
    #warning MSCAN support not yet fully tested on this platform
    #endif

    /* if CANHW is not specified in freemaster.h, the platform is not yet tested with CAN */
    #if (!defined(FMSTR_CANHW_FLEXCAN)) && FMSTR_USE_FLEXCAN
    #warning FlexCAN support not yet fully tested on this platform
    #endif

   
    #ifndef FMSTR_CAN_BASE
    #error FMSTR_CAN_BASE must be defined when is using CAN interface
    #endif
    
    /* can't be both*/
    #if (FMSTR_USE_MSCAN) && (FMSTR_USE_FLEXCAN)
    #error Can not configure FreeMASTER to use both MSCAN and FlexCAN
    #endif
    
    /* should be at least one */
    #if !(FMSTR_USE_MSCAN) && !(FMSTR_USE_FLEXCAN)
    #error Please select if MSCAN or FlexCAN will be used
    #endif

    /* flexcan needs to know the transmit and receive MB number */
    #if FMSTR_USE_FLEXCAN
        /* Flexcan TX message buffer must be defined */
        #ifndef FMSTR_FLEXCAN_TXMB
        //#error FlexCAN transmit buffer needs to be specified (use FMSTR_FLEXCAN_TXMB)
        #warning FlexCAN Message Buffer 0 is used for transmit messages
        #define FMSTR_FLEXCAN_TXMB 0
        #endif
        /* Flexcan RX message buffer must be defined */
        #ifndef FMSTR_FLEXCAN_RXMB
        //#error FlexCAN receive buffer needs to be specified (use FMSTR_FLEXCAN_RXMB)
        #warning FlexCAN Message Buffer 1 is used for receive messages
        #define FMSTR_FLEXCAN_RXMB 1
        #endif

        #if FMSTR_FLEXCAN_TXMB == FMSTR_FLEXCAN_RXMB
            #warning FCAN RX and FCAN TX are using same Message Buffer. FreeMASTER CAN driver doesnt support this configuration. Please change number of Message Buffer in FMSTR_FLEXCAN_TXMB or FMSTR_FLEXCAN_RXMB macros.
        #endif
    #endif

  
    /* incoming (command) CAN message ID */
    #ifndef FMSTR_CAN_CMDID
    #define FMSTR_CAN_CMDID 0x7aa
    #endif

    /* command ID can be changed in runtime (before FMSTR_Init) */
    #ifndef FMSTR_CAN_CMDID_DYNAMIC
    #define FMSTR_CAN_CMDID_DYNAMIC 0  /* disabled by default */
    #endif
    
    /* response CAN message ID, may be the same as command ID */
    #ifndef FMSTR_CAN_RESPID
    #define FMSTR_CAN_RESPID 0x7aa
    #endif
    
    /* response ID can be changed in runtime (before FMSTR_Init) */
    #ifndef FMSTR_CAN_RESPID_DYNAMIC
    #define FMSTR_CAN_RESPID_DYNAMIC 0  /* disabled by default */
    #endif

#endif




/* read memory commands are ENABLED by default */
#ifndef FMSTR_USE_READMEM
#define FMSTR_USE_READMEM 1
#endif
#ifndef FMSTR_USE_WRITEMEM
#define FMSTR_USE_WRITEMEM 1
#endif
#ifndef FMSTR_USE_WRITEMEMMASK 
#define FMSTR_USE_WRITEMEMMASK 1
#endif

/* read variable commands are DISABLED by default */
#ifndef FMSTR_USE_READVAR
#define FMSTR_USE_READVAR 0
#endif
#ifndef FMSTR_USE_WRITEVAR
#define FMSTR_USE_WRITEVAR 0
#endif
#ifndef FMSTR_USE_WRITEVARMASK 
#define FMSTR_USE_WRITEVARMASK 0
#endif

/* default scope settings */


#ifndef FMSTR_MAX_SCOPE_VARS
#define FMSTR_MAX_SCOPE_VARS 8
#endif

/* check scope settings */
#if defined FMSTR_USE_SCOPE
    #if FMSTR_MAX_SCOPE_VARS > 8 || FMSTR_MAX_SCOPE_VARS < 2
        #error Error in FMSTR_MAX_SCOPE_VARS value. Use a value in range 2..8
    #endif
#endif  

/* default recorder settings */


#ifndef FMSTR_MAX_REC_VARS
#define FMSTR_MAX_REC_VARS 8
#endif

#ifndef FMSTR_REC_FARBUFF
#define FMSTR_REC_FARBUFF 0
#endif

#ifndef FMSTR_REC_OWNBUFF
#define FMSTR_REC_OWNBUFF 0
#endif

#ifndef FMSTR_USE_FASTREC
#define FMSTR_USE_FASTREC 0
#endif

/* Enable code size optimalization */
#ifndef FMSTR_LIGHT_VERSION
#define FMSTR_LIGHT_VERSION 0
#endif

/* Always report sigle error code from recorder routines  */
#ifndef FMSTR_REC_COMMON_ERR_CODES
#define FMSTR_REC_COMMON_ERR_CODES FMSTR_LIGHT_VERSION
#endif

/* Remove code for single wire communication */
#ifndef FMSTR_SCI_TWOWIRE_ONLY
#define FMSTR_SCI_TWOWIRE_ONLY FMSTR_LIGHT_VERSION
#endif

/* Number of recorder post-trigger samples is by default controlled by PC */
#ifndef FMSTR_REC_STATIC_POSTTRIG
#define FMSTR_REC_STATIC_POSTTRIG 0
#endif

/* Recorder divisor is by default controlled by PC */
#ifndef FMSTR_REC_STATIC_DIVISOR
#define FMSTR_REC_STATIC_DIVISOR 0
#endif

/* check recorder settings */
#if defined FMSTR_USE_RECORDER || FMSTR_USE_FASTREC
    #if FMSTR_MAX_REC_VARS > 8 || FMSTR_MAX_REC_VARS < 2
        #error Error in FMSTR_MAX_REC_VARS value. Use a value in range 2..8
    #endif
    
    /* 0 means recorder time base is "unknown" */
    #ifndef FMSTR_REC_TIMEBASE
    #define FMSTR_REC_TIMEBASE 0 
    #endif
    
    /* default recorder buffer size is 256 */
    #ifndef FMSTR_REC_BUFF_SIZE
    #define FMSTR_REC_BUFF_SIZE 256
    #endif

    #if !FMSTR_USE_READMEM
    #error Recorder needs the FMSTR_USE_READMEM feature
    #endif
#endif  

/* fast recorder requires its own allocation of recorder buffer */
#if FMSTR_USE_FASTREC
    #if FMSTR_REC_OWNBUFF
        #error Fast recorder requires its own buffer allocation
    #endif
#endif

/* default app.cmds settings */
//#define FMSTR_USE_APPCMD 0

#ifndef FMSTR_APPCMD_BUFF_SIZE
#define FMSTR_APPCMD_BUFF_SIZE 16
#endif

#ifndef FMSTR_MAX_APPCMD_CALLS
#define FMSTR_MAX_APPCMD_CALLS 0
#endif

/* TSA configuration check */
//#define FMSTR_USE_TSA 0

#ifndef FMSTR_USE_TSA_SAFETY
#define FMSTR_USE_TSA_SAFETY 0
#endif

#if defined FMSTR_USE_TSA
    #if !FMSTR_USE_READMEM
        #error TSA needs the FMSTR_USE_READMEM feature
    #endif
#endif

/* SFIO not used by default */
//#define FMSTR_USE_SFIO


/* check SFIO settings */
#if FMSTR_USE_SFIO

    /* The SFIO configuration files (sfio.h and optionally also the sfio_cfg.h) exist 
       in project to define SFIO parameters. */
    #include "sfio.h"
    
    #ifndef SFIO_MAX_INPUT_DATA_LENGTH
    #error  SFIO_MAX_INPUT_DATA_LENGTH was not defined in sfio_cfg.h
    #endif
    #ifndef SFIO_MAX_OUTPUT_DATA_LENGTH
    #error  SFIO_MAX_OUTPUT_DATA_LENGTH was not defined in sfio_cfg.h
    #endif
    
#endif    

/* use transport "pipe" functionality */
#if !defined FMSTR_USE_PIPES
  //#define FMSTR_USE_PIPES
#endif

/* "pipe" putstring formatting (enabled by default) */
#ifndef FMSTR_USE_PIPE_PRINTF
#define FMSTR_USE_PIPE_PRINTF 1
#endif

/* "pipe" variable-argument printf (enabled by default) */
#ifndef FMSTR_USE_PIPE_PRINTF_VARG
#define FMSTR_USE_PIPE_PRINTF_VARG FMSTR_USE_PIPE_PRINTF
#endif

#if defined FMSTR_USE_PIPES

    #ifdef FMSTR_PIPES_EXPERIMENTAL
    #warning The "pipes" feature is now in experimental code phase. Not yet tested on this platform.
    #endif        

    /* one pipe by default */    
    #ifndef FMSTR_MAX_PIPES_COUNT
    #define FMSTR_MAX_PIPES_COUNT 1
    #endif

    /* must enable printf for vararg printf */
    #if !(FMSTR_USE_PIPE_PRINTF) && (FMSTR_USE_PIPE_PRINTF_VARG)
    #error You must enable pipe printf for vararg printf (see FMSTR_USE_PIPE_PRINTF)
    #endif
    
    /* pipe printf buffer */    
    #ifndef FMSTR_PIPES_PRINTF_BUFF_SIZE
    #define FMSTR_PIPES_PRINTF_BUFF_SIZE 48
    #endif

    /* at least one */
    #if FMSTR_MAX_PIPES_COUNT < 1
    #warning No sense to allocate zero-count pipes. Disabling pipes.
    #undef  FMSTR_USE_PIPES
    //#define FMSTR_USE_PIPES 0
    #endif

    /* printf buffer should accept one integer printed */    
    #if FMSTR_PIPES_PRINTF_BUFF_SIZE < 8
    #error Pipe printf buffer should be at least 8 (see FMSTR_PIPES_PRINTF_BUFF_SIZE)
    #endif
    #if FMSTR_PIPES_PRINTF_BUFF_SIZE > 255
    #error Pipe printf buffer should not exceed 255 (see FMSTR_PIPES_PRINTF_BUFF_SIZE)
    #endif
    
#endif

/* what kind of board information structure will be sent? */
#ifndef FMSTR_USE_BRIEFINFO
    #if defined FMSTR_USE_RECORDER
        /* recorder requires full info */
        #define FMSTR_USE_BRIEFINFO 0
    #else
        /* otherwise no brief info is enough */
        #define FMSTR_USE_BRIEFINFO 1
    #endif
#endif

/* check what kind of board info is sent */
#if FMSTR_USE_BRIEFINFO
    #if defined FMSTR_USE_RECORDER
        #warning The full information structure must be used when recorder is to be used
        #undef  FMSTR_USE_BRIEFINFO
        #define FMSTR_USE_BRIEFINFO 1
    #endif
#endif

/* automatic buffer size by default */
#ifndef FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE 0
#endif

/* check minimal buffer size required for all enabled features */
#if FMSTR_COMM_BUFFER_SIZE
    /* basic commands (getinfobrief, write/read memory etc.) */
    #if FMSTR_USE_BRIEFINFO && FMSTR_COMM_BUFFER_SIZE < 11 
    #error FMSTR_COMM_BUFFER_SIZE set too small for basic FreeMASTER commands (11 bytes)
    #endif

    /* full info required */
    #if !(FMSTR_USE_BRIEFINFO) && FMSTR_COMM_BUFFER_SIZE < 35
    #error FMSTR_COMM_BUFFER_SIZE set too small for GETINFO command (size 35)
    #endif

    /* application commands */
    #if FMSTR_USE_APPCMD && FMSTR_COMM_BUFFER_SIZE < ((FMSTR_APPCMD_BUFF_SIZE)+1)
    #error FMSTR_COMM_BUFFER_SIZE set too small for SENDAPPCMD command (size FMSTR_APPCMD_BUFF_SIZE+1)
    #endif

    /* configuring scope (EX) */
    #if defined FMSTR_USE_SCOPE && FMSTR_COMM_BUFFER_SIZE < ((FMSTR_MAX_SCOPE_VARS)*5+1)
    #error FMSTR_COMM_BUFFER_SIZE set too small for SETUPSCOPEEX command (size FMSTR_MAX_SCOPE_VARS*5+1)
    #endif

    /* configuring recorder (EX) */
    #if defined FMSTR_USE_RECORDER && FMSTR_COMM_BUFFER_SIZE < ((FMSTR_MAX_REC_VARS)*5+18)
    #error FMSTR_COMM_BUFFER_SIZE set too small for SETUPRECEX command (size FMSTR_MAX_REC_VARS*5+18)
    #endif
    
    /* SFIO encapsulation */
    #if FMSTR_USE_SFIO
    #if (FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_INPUT_DATA_LENGTH)+1)) || \
        (FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1))
    #error FMSTR_COMM_BUFFER_SIZE set too small for SFIO encapsulation (see SFIO_MAX_xxx_DATA_LENGTH)
    #endif
    #endif

/* automatic: determine required buffer size based on features enabled */
#else
    /* smallest for basic commands (getinfobrief, write/read memory etc.) */
    #undef  FMSTR_COMM_BUFFER_SIZE
    #define FMSTR_COMM_BUFFER_SIZE 11

    /* full info required */
    #if !(FMSTR_USE_BRIEFINFO) && FMSTR_COMM_BUFFER_SIZE < 35
    #undef  FMSTR_COMM_BUFFER_SIZE
    #define FMSTR_COMM_BUFFER_SIZE 35
    #endif

    /* using application commands (must accomodate maximal app.cmd data length) */
    #if FMSTR_USE_APPCMD && FMSTR_COMM_BUFFER_SIZE < ((FMSTR_APPCMD_BUFF_SIZE)+1)
    #undef  FMSTR_COMM_BUFFER_SIZE
    #define FMSTR_COMM_BUFFER_SIZE ((FMSTR_APPCMD_BUFF_SIZE)+1)
    #endif

    /* configuring scope (EX) */
    #if defined FMSTR_USE_SCOPE && FMSTR_COMM_BUFFER_SIZE < ((FMSTR_MAX_SCOPE_VARS)*5+1)
    #undef  FMSTR_COMM_BUFFER_SIZE
    #define FMSTR_COMM_BUFFER_SIZE ((FMSTR_MAX_SCOPE_VARS)*5+1)
    #endif

    /* configuring recorder (EX) */
    #if defined FMSTR_USE_RECORDER && FMSTR_COMM_BUFFER_SIZE < ((FMSTR_MAX_REC_VARS)*5+18)
    #undef  FMSTR_COMM_BUFFER_SIZE
    #define FMSTR_COMM_BUFFER_SIZE ((FMSTR_MAX_REC_VARS)*5+18)
    #endif

    /* SFIO encapsulation (in buffer) */
    #if FMSTR_USE_SFIO
    #if FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_INPUT_DATA_LENGTH)+1)
    #undef  FMSTR_COMM_BUFFER_SIZE
    #define FMSTR_COMM_BUFFER_SIZE ((SFIO_MAX_INPUT_DATA_LENGTH)+1)
    #endif
    #endif
    
    /* SFIO encapsulation (out buffer) */
    #if FMSTR_USE_SFIO
    #if FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1)
    #undef  FMSTR_COMM_BUFFER_SIZE
    #define FMSTR_COMM_BUFFER_SIZE ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1)
    #endif
    #endif
    
#endif

#endif /* __FREEMASTER_PRIVATE_H */


/* FreeMASTER communication buffer (in/out) plus the STS and LEN bytes */
static FMSTR_BCHR pcm_pCommBuffer[FMSTR_COMM_BUFFER_SIZE+3];   


/**************************************************************************//*!
*
* @brief    Serial communication initialization
*
******************************************************************************/

void FMSTR_InitSerial(void)
{   
    /* initialize all state variables */
    pcm_wFlags.all = 0U;
  //pcm_nTxTodo = 0U;

    
#if defined FREEMASTER_UART && FMSTR_SCI_TWOWIRE_ONLY
    /* to enable TX and RX together in FreeMASTER initialization */
//    FMSTR_SCI_TE_RE();
#endif
    
#if FMSTR_SHORT_INTR
    pcm_pRQueueRP = pcm_pRQueueBuffer;
    pcm_pRQueueWP = pcm_pRQueueBuffer;
#endif

    /* start listening for commands */
    FMSTR_Listen();
}

/**************************************************************************//*!
*
* @brief    Start listening on a serial line
*
* Reset the receiver machine and start listening on a serial line
*
******************************************************************************/

static void FMSTR_Listen(void)
{
    pcm_nRxTodo = 0U;

    /* disable transmitter state machine */
    pcm_wFlags.flg.bTxActive = 0U;
    pcm_wFlags.flg.bTxWaitTC = 0U;

    /* disable transmitter, enable receiver (enables single-wire connection) */
#if defined FREEMASTER_UART && !FMSTR_SCI_TWOWIRE_ONLY
    FMSTR_SCI_TD();
    FMSTR_SCI_RE();
#endif

    /* disable transmit, enable receive interrupts */
#if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
#if defined FREEMASTER_UART
    FMSTR_SCI_DTXI();   /* disable SCI transmit interrupt */
    FMSTR_SCI_ERXI();   /* enable SCI recieve interrupt */
    
#elif FMSTR_USE_JTAG
    FMSTR_JTAG_DTXI();  /* disable JTAG transmit interrupt  */
    FMSTR_JTAG_ERXI();  /* enable JTAG recieve interrupt  */

    /* RIE bit is forced low by HW until EONCE is first accesed, we will try again in FMSTR_Poll */
    if(!FMSTR_JTAG_ERXI_CHECK())
        pcm_wFlags.flg.bJtagRIEPending = 1;
        
#endif
#endif  
}

/**************************************************************************//*!
*
* @brief    Send response of given error code (no data) 
*
* @param    nErrCode - error code to be sent
*
******************************************************************************/

static void FMSTR_SendError(QUEUE_HANDLE FreeMasterPort, FMSTR_BCHR nErrCode)
{
    /* fill & send single-byte response */
    *pcm_pCommBuffer = nErrCode;
    FMSTR_SendResponse(FreeMasterPort, pcm_pCommBuffer, 1U);
}


/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_protocol.h
*
* @brief  FreeMASTER protocol header file
*
* @version 1.0.1.0
* 
* @date May-4-2012
* 
*******************************************************************************/

#ifndef __FREEMASTER_PROTOCOL_H
#define __FREEMASTER_PROTOCOL_H

/*-------------------------------------
  command message - standard commands
  -------------------------------------*/
  
#define FMSTR_CMD_READMEM           0x01U
#define FMSTR_CMD_WRITEMEM          0x02U
#define FMSTR_CMD_WRITEMEMMASK      0x03U
#define FMSTR_CMD_READMEM_EX        0x04U    /* read a block of memory */
#define FMSTR_CMD_WRITEMEM_EX       0x05U    /* write a block of memory */   
#define FMSTR_CMD_WRITEMEMMASK_EX   0x06U    /* write block of memory with bit mask */   
#define FMSTR_CMD_SETUPSCOPE        0x08U
#define FMSTR_CMD_SETUPREC          0x09U
#define FMSTR_CMD_SETUPSCOPE_EX     0x0aU    /* setup the osciloscope */ 
#define FMSTR_CMD_SETUPREC_EX       0x0bU    /* setup the recorder */     
#define FMSTR_CMD_SENDAPPCMD        0x10U    /* send the application command */  
#define FMSTR_CMD_GETTSAINFO        0x11U    /* get TSA info */
#define FMSTR_CMD_GETTSAINFO_EX     0x12U    /* get TSA info 32bit */
#define FMSTR_CMD_SFIOFRAME_1       0x13U    /* deliver & execute SFIO frame (even) */
#define FMSTR_CMD_SFIOFRAME_0       0x14U    /* deliver & execute SFIO frame (odd) */
#define FMSTR_CMD_PIPE              0x15U    /* read/write pipe data */

/*-------------------------------------
  command message - Fast Commands
  -------------------------------------*/
  
/* no data part */
#define FMSTR_CMD_GETINFO           0xc0U    /* retrieve board information structure */  
#define FMSTR_CMD_STARTREC          0xc1U    /* start data recorder */   
#define FMSTR_CMD_STOPREC           0xc2U    /* stop data recorder */    
#define FMSTR_CMD_GETRECSTS         0xc3U    /* get the recorder status */   
#define FMSTR_CMD_GETRECBUFF        0xc4U
#define FMSTR_CMD_READSCOPE         0xc5U    /* read the scope data */   
#define FMSTR_CMD_GETAPPCMDSTS      0xc6U    /* get the application command status */    
#define FMSTR_CMD_GETINFOBRIEF      0xc8U    /* retrieve a subset of board information structure */  
#define FMSTR_CMD_GETRECBUFF_EX     0xc9U    /* get the recorder data */ 
#define FMSTR_CMD_SFIOGETRESP_0     0xcaU    /* retry to get last SFIO response (even) */
#define FMSTR_CMD_SFIOGETRESP_1     0xcbU    /* retry to get last SFIO response (odd) */

/* 2 bytes data part */
#define FMSTR_CMD_READVAR8          0xD0U
#define FMSTR_CMD_READVAR16         0xD1U
#define FMSTR_CMD_READVAR32         0xD2U
#define FMSTR_CMD_GETAPPCMDDATA     0xD3U    /* get the application command data */
#define FMSTR_CMD_GETSTRLEN         0xD4U    /* get string length (required by TSA) */

/* 4 bytes data part */
#define FMSTR_CMD_READVAR8_EX       0xe0U    /* read byte variable */    
#define FMSTR_CMD_READVAR16_EX      0xe1U    /* read word variable */    
#define FMSTR_CMD_READVAR32_EX      0xe2U    /* read dword variable */   
#define FMSTR_CMD_WRITEVAR8         0xe3U    /* write byte variable */   
#define FMSTR_CMD_WRITEVAR16        0xe4U    /* write word variable */   
#define FMSTR_CMD_WRITEVAR8MASK     0xe5U    /* write specified bits in byte variable  */    
#define FMSTR_CMD_GETSTRLEN_EX      0xe6U    /* get string length (required by TSA) */

/* 6 bytes data part */
#define FMSTR_CMD_WRITEVAR32        0xf0U    /* write dword variable */  
#define FMSTR_CMD_WRITEVAR16MASK    0xf1U    /* write specified bits in word variable */ 

/*-------------------------------------
  response message - status byte
  -------------------------------------*/

/* flags in response codes */
#define FMSTR_STSF_ERROR            0x80U    /* FLAG: error answer (no response data) */  
#define FMSTR_STSF_VARLEN           0x40U    /* FLAG: variable-length answer (length byte) */  
#define FMSTR_STSF_EVENT            0x20U    /* FLAG: reserved */  
  
/* confirmation codes */
#define FMSTR_STS_OK                0x00U    /* operation finished successfully */    
#define FMSTR_STS_RECRUN            0x01U    /* data recorder is running */  
#define FMSTR_STS_RECDONE           0x02U    /* data recorder is stopped */  

/* error codes */
#define FMSTR_STC_INVCMD            0x81U    /* unknown command code */  
#define FMSTR_STC_CMDCSERR          0x82U    /* command checksum error */    
#define FMSTR_STC_CMDTOOLONG        0x83U    /* comand is too long */    
#define FMSTR_STC_RSPBUFFOVF        0x84U    /* the response would not fit into transmit buffer */   
#define FMSTR_STC_INVBUFF           0x85U    /* invalid buffer length or operation */    
#define FMSTR_STC_INVSIZE           0x86U    /* invalid size specified */    
#define FMSTR_STC_SERVBUSY          0x87U    /* service is busy */   
#define FMSTR_STC_NOTINIT           0x88U    /* service is not initialised */    
#define FMSTR_STC_EACCESS           0x89U    /* access is denied */  
#define FMSTR_STC_SFIOERR           0x8AU    /* Error in SFIO frame */  
#define FMSTR_STC_SFIOUNMATCH       0x8BU    /* Even/odd mismatch in SFIO transaction */  
#define FMSTR_STC_PIPEERR           0x8CU    /* Pipe error */  
#define FMSTR_STC_FASTRECERR        0x8DU    /* Feature not implemented in Fast Recorder */  
#define FMSTR_STC_CANTGLERR         0x8EU    /* CAN fragmentation (toggle bit) error */
#define FMSTR_STC_CANMSGERR         0x8FU    /* CAN message format error */

/******************************************************************************
* Protocol constants 
*******************************************************************************/

#define FMSTR_SOB                   0x2bU    /* '+' - start of message*/
#define FMSTR_FASTCMD               0xc0U    /* code of fast cmd 0xc0> */
#define FMSTR_FASTCMD_DATALEN_MASK  0x30U    /* mask of data length part of fast command */
#define FMSTR_FASTCMD_DATALEN_SHIFT 3
#define FMSTR_DESCR_SIZE            25U      /* length board desription string */

/* Board configuration flags  */
#define FMSTR_CFGFLAG_BIGENDIAN       0x01U      /*/< used when CPU is big endian */

/* TSA-global flags  */
#define FMSTR_TSA_INFO_VERSION_MASK   0x000fU    /*/< TSA version  */
#define FMSTR_TSA_INFO_32BIT          0x0100U    /*/< TSA address format (16/32 bit) */
#define FMSTR_TSA_INFO_HV2BA          0x0200U    /*/< TSA HawkV2 byte-addressing mode */

/******************************************************************************
* CAN Protocol constants 
*******************************************************************************/

/* control byte (the first byte in each CAN message) */
#define FMSTR_CANCTL_TGL 0x80   /* toggle bit, first message clear, then toggles */
#define FMSTR_CANCTL_M2S 0x40   /* master to slave direction */
#define FMSTR_CANCTL_FST 0x20   /* first CAN message of FreeMASTER packet */
#define FMSTR_CANCTL_LST 0x10   /* last CAN message of FreeMASTER packet */
#define FMSTR_CANCTL_SPC 0x08   /* special command (in data[1], handled by CAN sublayer (no FM protocol decode) */
#define FMSTR_CANCTL_LEN_MASK 0x07   /* number of data bytes after the CTL byte (0..7) */

/* special commands */
#define FMSTR_CANSPC_PING 0xc0


#endif /* __FREEMASTER_PROTOCOL_H */




/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting 
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer 
* (inlcuding the status byte). It computes the check sum and kicks on tx.
*
******************************************************************************/

void FMSTR_SendResponse(QUEUE_HANDLE FreeMasterPort, FMSTR_BPTR pResponse, FMSTR_SIZE8 nLength)
{
    FMSTR_U16 chSum = 0U;
    FMSTR_U8 i, c;

    /* remeber the buffer to be sent */
  //pcm_pTxBuff = pResponse;
    
    /* status byte and data are already there, compute checksum only     */
    for (i=0U; i<nLength; i++)
    {
        c = 0U;
        pResponse = FMSTR_ValueFromBuffer8(&c, pResponse);
        /* add character to checksum */
        chSum += c;
        /* prevent saturation to happen on DSP platforms */
        chSum &= 0xffU;
    }
    
    /* store checksum after the message */
    pResponse = FMSTR_ValueToBuffer8(pResponse, (FMSTR_U8) (((FMSTR_U16)~(chSum)) + 1U));

    /* send the message and the checksum and the SOB */
  //pcm_nTxTodo = (FMSTR_SIZE8) (nLength + 1U); 
    
    /* now transmitting the response */
    pcm_wFlags.flg.bTxActive = 1U;
    pcm_wFlags.flg.bTxWaitTC = 0U;

    /* do not replicate the initial SOB  */
    pcm_wFlags.flg.bTxLastCharSOB = 0U;
    
#if defined FREEMASTER_UART       
    {
        /*lint -esym(550, dummySR) */        
      //volatile FMSTR_SCISR dummySR;
        unsigned char cFMSTR_SOB = FMSTR_SOB;

        /* disable receiver, enable transmitter (single-wire communication) */
#if !FMSTR_SCI_TWOWIRE_ONLY
        FMSTR_SCI_RD();
        FMSTR_SCI_TE();
#endif        
        /* kick on the SCI transmission (also clears TX Empty flag on some platforms) */
        fnWrite(FreeMasterPort, &cFMSTR_SOB, 1);
        nLength++;
        fnWrite(FreeMasterPort, (pResponse - nLength), (nLength + 1));
      //dummySR = FMSTR_SCI_GETSR();
      //FMSTR_SCI_PUTCHAR(FMSTR_SOB);
    }
    
#elif FMSTR_USE_JTAG
    /* kick on the JTAG transmission */
    pcm_wJtagTxData = FMSTR_SOB;
    pcm_wJtagTxCtr = 1U;
    
    /* send the next two bytes immediatelly (we can be sure there are two bytes) */
    FMSTR_Tx();
    FMSTR_Tx();
    
    /* send the third byte (if any) or flush the 32bit JTAG word */
    FMSTR_Tx();
        
#endif

    /* TX interrupt enable, RX interrupt disable */
#if FMSTR_LONG_INTR || FMSTR_SHORT_INTR
#if defined FREEMASTER_UART       
    FMSTR_SCI_DRXI();
    FMSTR_SCI_ETXI();
    
#elif FMSTR_USE_JTAG
#if FMSTR_USE_JTAG_TXFIX
    /* in TX-bugfix mode, keep the RX interrupt enabled as it */
    /* is used as "able-to-TX" notification from the PC */
    FMSTR_JTAG_ERXI();
#else
    /* otherwise, JTAG is very same as the SCI */
    FMSTR_JTAG_DRXI();
    FMSTR_JTAG_ETXI();
#endif              

#endif
#endif      
}

/**************************************************************************//*!
*
* @brief    Output buffer transmission
*
* This function sends one character of the transmit buffer. It handles 
* replicating of the SOB characted inside the message body.
*
******************************************************************************/
#if 0
static void FMSTR_Tx(void)
{
    FMSTR_U8 ch = 0U;
    
    if (pcm_nTxTodo)
    {
        /* fetch & send character ready to transmit */
        /*lint -e{534} ignoring return value */
        FMSTR_ValueFromBuffer8(&ch, pcm_pTxBuff);
        
#if defined FREEMASTER_UART       
        /* just put the byte into the SCI transmit buffer */
        FMSTR_SCI_PUTCHAR((FMSTR_U8) ch);
        
#elif FMSTR_USE_JTAG
        /* put byte to 32bit JTAG buffer */
        pcm_wJtagTxData = (pcm_wJtagTxData << 8) | ch;

        /* another byte  */
        pcm_wJtagTxCtr++;
        
        /* all four bytes ready?  */
        if(pcm_wJtagTxCtr & 0x4U)
        {
            FMSTR_JTAG_PUTDWORD(pcm_wJtagTxData);
            pcm_wJtagTxCtr = 0U;
        }
            
#endif

        /* first, handle the replicated SOB characters */
        if (ch == FMSTR_SOB)
        {
            pcm_wFlags.flg.bTxLastCharSOB ^= 1U;
            if ((pcm_wFlags.flg.bTxLastCharSOB))
            {
            /* yes, repeat the SOB next time */
            goto FMSTR_Tx_Exit;
            }
        }
        /* no, advance tx buffer pointer */
        pcm_nTxTodo--;
        pcm_pTxBuff = FMSTR_SkipInBuffer(pcm_pTxBuff, 1U);
    }
#if FMSTR_USE_JTAG  
    /* on JTAG, the some bytes may still be pending in a 32bit buffer */
    else if(pcm_wJtagTxCtr > 0U)
    {
        /* add padding bytes */
        while(!(pcm_wJtagTxCtr & 4U))
        {
            pcm_wJtagTxData = (pcm_wJtagTxData << 8U) | 0xffU;
            pcm_wJtagTxCtr++;
        }

        /* send the word just completed */
        FMSTR_JTAG_PUTDWORD(pcm_wJtagTxData);

        /* done, bTxActive will be deactivated next time */
        pcm_wJtagTxCtr = 0U;
    }
#endif  
    /* transmission finished, start listening again */
    else
    {
        /* when SCI TX buffering is enabled, we must first wait until all 
          characters are physically transmitted (before disabling transmitter) */
#if defined FREEMASTER_UART && FMSTR_SCI_HAS_TXQUEUE
        pcm_wFlags.flg.bTxWaitTC = 1;

        /* wait for SCI TC interrupt */
        #if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
        FMSTR_SCI_ETCI();
        #endif
#else
        /* start listening immediatelly */
        FMSTR_Listen();
#endif
    }
FMSTR_Tx_Exit: ;
}
#endif

/**************************************************************************//*!
*
* @brief  Handle received character 
*
* @param  nRxChar  The character to be processed 
* 
* Handle the character received and -if the message is complete- call the 
* protocol decode routine. 
*
******************************************************************************/
#if 0
static void FMSTR_Rx(FMSTR_BCHR nRxChar)
{
    FMSTR_SERIAL_FLAGS * pflg = &pcm_wFlags;
    /* first, handle the replicated SOB characters */
    if(nRxChar == FMSTR_SOB)
    {
        pflg->flg.bRxLastCharSOB ^= 1;
        if(pflg->flg.bRxLastCharSOB)
        {
            /* this is either the first byte of replicated SOB or a  */
            /* real Start-of-Block mark - we will decide next time in FMSTR_Rx */
            return;
        }
    }
    
    /* we have got a common character preceeded by the SOB -  */
    /* this is the command code! */
    if(pflg->flg.bRxLastCharSOB)
    {
        /* reset reciving process */
        pcm_pRxBuff = pcm_pCommBuffer;
        *pcm_pRxBuff++ = nRxChar;
        
        /* start computing the checksum */
        pcm_nRxCheckSum = nRxChar;
        pcm_nRxTodo = 0U;
        
        /* if the standard command was received, the message length will come in next byte */
        pflg->flg.bRxMsgLengthNext = 1U;

        /* fast command? */
        if(!((~nRxChar) & FMSTR_FASTCMD))
        {
            /* fast command received, there will be no length information */
            pflg->flg.bRxMsgLengthNext = 0U;
            /* as it is encoded in the command byte directly */
            pcm_nRxTodo = (FMSTR_SIZE8) 
                (((((FMSTR_SIZE8)nRxChar) & FMSTR_FASTCMD_DATALEN_MASK) >> FMSTR_FASTCMD_DATALEN_SHIFT) + 1U);
        }

        /* command code stored & processed */
        pflg->flg.bRxLastCharSOB = 0U;
        return;
    }

    /* we are waiting for the length byte */
    if(pflg->flg.bRxMsgLengthNext)
    {
        /* this byte, total data length and the checksum */
        pcm_nRxTodo = (FMSTR_SIZE8) (1U + ((FMSTR_SIZE8)nRxChar) + 1U);
        /* now read the data bytes */
        pflg->flg.bRxMsgLengthNext = 0U;

    }

    /* waiting for a data byte? */
    if(pcm_nRxTodo)
    {
        /* add this byte to checksum */
        pcm_nRxCheckSum += nRxChar;
        
        /* decrease number of expected bytes */
        pcm_nRxTodo--;
        /* was it the last byte of the message (checksum)? */
        if(!pcm_nRxTodo)
        {
            /* receive buffer overflow? */
            if(pcm_pRxBuff == NULL)
            {
                FMSTR_SendError(FreeMasterPort, FMSTR_STC_CMDTOOLONG);
            }
            /* checksum error? */
            else if((pcm_nRxCheckSum & 0xffU) != 0U)
            {
                FMSTR_SendError(FreeMasterPort, FMSTR_STC_CMDCSERR);
            }
            /* message is okay */
            else 
            {
                /* do decode now! */

                FMSTR_ProtocolDecoder(pcm_pCommBuffer);
            }
        }
        /* not the last character yet */
        else 
        {   
            /* is there still a space in the buffer? */
            if(pcm_pRxBuff)
            {
                /*lint -e{946} pointer arithmetic is okay here (same array) */
                if(pcm_pRxBuff < (pcm_pCommBuffer + FMSTR_COMM_BUFFER_SIZE))
                {
                    /* store byte  */
                    *pcm_pRxBuff++ = nRxChar;
                }
                /* buffer is full! */
                else
                {
                    /* NULL rx pointer means buffer overflow - but we still need */
                    /* to receive all message characters (for the single-wire mode) */
                    /* so keep "receiving" - but throw away all characters from now */
                    pcm_pRxBuff = NULL;
                }
            }
        }
    }
}
#endif

/*******************************************************************************
*
* @brief    Routine to quick-receive a character (put to a queue only)
*
* This function puts received character into a queue and exits as soon as possible.
*
*******************************************************************************/

#if FMSTR_SHORT_INTR

static void FMSTR_RxQueue(FMSTR_BCHR nRxChar)
{ 
    /* future value of write pointer */
    FMSTR_BPTR wpnext = pcm_pRQueueWP + 1;
    
    /*lint -e{946} pointer arithmetic is okay here (same array) */
    if(wpnext >= (pcm_pRQueueBuffer + FMSTR_COMM_RQUEUE_SIZE))
    {
        wpnext = pcm_pRQueueBuffer;
    }
    
    /* any space in queue? */
    if(wpnext != pcm_pRQueueRP)
    {
        *pcm_pRQueueWP = (FMSTR_U8) nRxChar;
        pcm_pRQueueWP = wpnext;
    }
}

#endif /* FMSTR_SHORT_INTR  */

/*******************************************************************************
*
* @brief    Late processing of queued characters
*
* This function takes the queued characters and calls FMSTR_Rx() for each of them,
* just like as the characters would be received from SCI one by one.
*
*******************************************************************************/

#if FMSTR_SHORT_INTR

static void FMSTR_RxDequeue(void)
{ 
    FMSTR_BCHR nChar = 0U;
    
    /* get all queued characters */
    while(pcm_pRQueueRP != pcm_pRQueueWP)
    {
        nChar = *pcm_pRQueueRP++;

        /*lint -e{946} pointer arithmetic is okay here (same array) */
        if(pcm_pRQueueRP >= (pcm_pRQueueBuffer + FMSTR_COMM_RQUEUE_SIZE))
        {
            pcm_pRQueueRP = pcm_pRQueueBuffer;
        }
        
        /* emulate the SCI receive event */
        if(!pcm_wFlags.flg.bTxActive)
        {
            FMSTR_Rx(nChar);
        }
    }
}

#endif /* FMSTR_SHORT_INTR */

/**************************************************************************//*!
*
* @brief    Handle SCI communication (both TX and RX)
*
* This function checks the SCI flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from SCI ISR or from the polling routine
*
******************************************************************************/

#if defined FREEMASTER_UART
#if 0
void FMSTR_ProcessSCI(void)
{
    /* read & clear status     */
    FMSTR_SCISR wSciSR = FMSTR_SCI_RDCLRSR();

    /* transmitter active and empty? */
    if (pcm_wFlags.flg.bTxActive)
    {
        /* able to accept another character? */
        if(wSciSR & FMSTR_SCISR_TDRE)
        {
            FMSTR_Tx();
        }
        
#if FMSTR_SCI_HAS_TXQUEUE
        /* waiting for transmission complete flag? */
        if(pcm_wFlags.flg.bTxWaitTC && (wSciSR & FMSTR_SCISR_TC))
        {
            /* after TC, we can switch to listen mode safely */
            FMSTR_Listen();
        }
#endif


#if !FMSTR_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        if(wSciSR & FMSTR_SCISR_RDRF)
        {
            /*lint -esym(550, nRxChar) */
            volatile FMSTR_U16 nRxChar;
            nRxChar = FMSTR_SCI_GETCHAR(); 
        }
#endif
    }
    /* transmitter not active, able to receive */
    else
    {
        /* data byte received? */
        if (wSciSR & FMSTR_SCISR_RDRF)
        {
            FMSTR_BCHR nRxChar = 0U;
            nRxChar = (FMSTR_BCHR) FMSTR_SCI_GETCHAR();

#if FMSTR_SHORT_INTR
            FMSTR_RxQueue(nRxChar);
#else
            FMSTR_Rx(nRxChar);  
#endif          
        }
    }
}
#endif

#endif

/**************************************************************************//*!
*
* @brief    Handle JTAG communication (both TX and RX)
*
* This function checks the JTAG flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from JTAG ISR or from the polling routine
*
******************************************************************************/

#if FMSTR_USE_JTAG

void FMSTR_ProcessJTAG(void)
{
    /* read & clear status     */
    register FMSTR_U16 wJtagSR = FMSTR_JTAG_GETSR();

    /* transmitter active? */
    if (pcm_wFlags.flg.bTxActive)
    {
        /* able to transmit a new character? (TX must be empty = read-out by PC) */
        if(!(wJtagSR & FMSTR_JTAG_OTXRXSR_TDF))
        {
        
#if FMSTR_USE_JTAG_TXFIX
            /* if TDF bit is useless due to silicon bug, use the RX flag */
            /* instead (PC sends us a dummy word to kick the RX flag on) */
            if(wJtagSR & FMSTR_JTAG_OTXRXSR_RDF)
#endif
            {
                /* send one byte always */
                FMSTR_Tx();
                
                /* try to fill-up the full 32bit JTAG word */
                while(pcm_wFlags.flg.bTxActive && pcm_wJtagTxCtr)
                {
                    FMSTR_Tx();
                }
            }               
        }

        /* ignore (read-out) the JTAG-received word */
        if(wJtagSR & FMSTR_JTAG_OTXRXSR_RDF)
        {
            /*lint -esym(550, nRxWord) */
            volatile FMSTR_U16 nRxWord;
            nRxWord = FMSTR_JTAG_GETWORD();
        }
    }
    /* transmitter not active */
    else
    {
        /* JTAG 32bit word (four bytes) received? */
        if(wJtagSR & FMSTR_JTAG_OTXRXSR_RDF)
        {
            register FMSTR_U32 nRxDWord;
            FMSTR_INDEX i;
            
            nRxDWord = FMSTR_JTAG_GETDWORD();
            
            /* process all bytes, MSB first */
            for(i=0; i<4; i++)
            {
#if FMSTR_SHORT_INTR
                FMSTR_RxQueue((FMSTR_BCHR)((nRxDWord >> 24U) & 0xffU));
                
#else
                FMSTR_Rx((FMSTR_BCHR)((nRxDWord >> 24U) & 0xffU));
            
                /* ignore the rest if previous bytes triggered a transmission */
                /* (i.e. the packet was complete and only filled-up to 32bit word) */
                if(pcm_wFlags.flg.bTxActive)
                {
                    break;
                }
#endif          
                /* next byte of 32bit word */
                nRxDWord = nRxDWord << 8;
            }
        }
    }
}

#endif

/*******************************************************************************
*
* @brief    API: Main "Polling" call from the application main loop
*
* This function either handles all the SCI communictaion (polling-only mode = 
* FMSTR_POLL_DRIVEN) or decodes messages received on the background by SCI interrupt
* (short-interrupt mode = FMSTR_SHORT_INTR). 
*
* In the JTAG interrupt-driven mode (both short and long), this function also checks
* if setting the JTAG RIE bit failed recently. This may happen because of the 
* RIE is held low by the EONCE hardware until the EONCE is first accessed from host.
* FMSTR_Init (->FMSTR_Listen) is often called while the PC-side FreeMASTER is still 
* turned off. So really, the JTAG is not enabled by this time and RIE bit is not set.
* This problem is detected (see how bJtagRIEPending is set above in FSMTR_Listen)
* and it is tried to be fixed periodically here in FMSTR_Poll.
*
*******************************************************************************/
#if 0
void FMSTR_Poll(void)
{ 
#if !FMSTR_POLL_DRIVEN && FMSTR_USE_JTAG
    /* in the interrupt-driven JTAG mode, the JTAG RIE may have failed to be set recently */
    if(pcm_wFlags.flg.bJtagRIEPending)
    {
        FMSTR_JTAG_ERXI();  /* try to enable JTAG recieve interrupt now */

        /* succeeded? */
        if(FMSTR_JTAG_ERXI_CHECK())
            pcm_wFlags.flg.bJtagRIEPending = 0; /* yes!, enough until it fails again (never?) */
    }
#endif    

#if FMSTR_POLL_DRIVEN

#if defined FREEMASTER_UART
    /* polled SCI mode */
    FMSTR_ProcessSCI(); 
    
#elif FMSTR_USE_JTAG
    /* polled JTAG mode */
    FMSTR_ProcessJTAG(); 
    
#endif
    
#elif FMSTR_SHORT_INTR

    /* process queued SCI characters */
    FMSTR_RxDequeue(); 
  
#endif
}
#endif
#else /* FMSTR_USE_JTAG */

/*lint -efile(766, freemaster_protocol.h) include file is not used in this case */

#endif /* FMSTR_USE_JTAG */



/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_protocol.c
*
* @brief  FreeMASTER protocol handler
*
* @version 1.0.1.0
* 
* @date May-4-2012
* 
*******************************************************************************
*
* This file contains the FreeMASTER protocol decoder and also the handlers 
* of basic protocol commands (read/write memory etc).
* 
*******************************************************************************/

//#include "freemaster.h"
//#include "freemaster_private.h"
//#include "freemaster_protocol.h"



/**************************************************************************//*!
*
* @brief    Decodes the FreeMASTER protocol and calls appropriate handlers 
*
* @param    pMessageIO - message in/out buffer
*
* @return   TRUE if frame was valid and any output was generated to IO buffer
*
* This Function decodes given message and invokes proper command handler
* which fills in the response. The response transmission is initiated
* in this call as well.
*
******************************************************************************/

FMSTR_BOOL FMSTR_ProtocolDecoder(QUEUE_HANDLE FreeMasterPort, FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponseEnd;
    FMSTR_U8 nCmd;
    
    /* no EX access by default */
    FMSTR_SetExAddr(FMSTR_FALSE);
    
    /* command code comes first in the message */
    /*lint -e{534} return value is not used */
    FMSTR_ValueFromBuffer8(&nCmd, pMessageIO);

    /* process command   */
    switch (nCmd)
    {
    
#if FMSTR_USE_READVAR

        /* read byte */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_READVAR8_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_READVAR8:
#endif          
            pResponseEnd = FMSTR_ReadVar(pMessageIO, 1U);
            break;
            
        /* read word */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_READVAR16_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_READVAR16:
#endif
            pResponseEnd = FMSTR_ReadVar(pMessageIO, 2U);
            break;
            
        /* read dword */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_READVAR32_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_READVAR32:
#endif
            pResponseEnd = FMSTR_ReadVar(pMessageIO, 4U);
            break;
#endif /* FMSTR_USE_READVAR */

#if FMSTR_USE_READMEM

        /* read a block of memory */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_READMEM_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_READMEM:
#endif
            pResponseEnd = FMSTR_ReadMem(pMessageIO);
            break;
            
#endif /* FMSTR_USE_READMEM */

#if defined FMSTR_USE_SCOPE

        /* prepare scope variables */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_SETUPSCOPE_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_SETUPSCOPE:
#endif
            pResponseEnd = FMSTR_SetUpScope(pMessageIO);
            break;  
            
        case FMSTR_CMD_READSCOPE:
            pResponseEnd = FMSTR_ReadScope(pMessageIO);
            break;
#endif /* FMSTR_USE_SCOPE */

#if defined FMSTR_USE_RECORDER

        /* get recorder status */
        case FMSTR_CMD_GETRECSTS:
            pResponseEnd = FMSTR_GetRecStatus(pMessageIO);
            break;

        /* start recorder */
        case FMSTR_CMD_STARTREC:
            pResponseEnd = FMSTR_StartRec(pMessageIO);
            break;

        /* stop recorder */
        case FMSTR_CMD_STOPREC:
            pResponseEnd = FMSTR_StopRec(pMessageIO);
            break;

        /* setup recorder */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_SETUPREC_EX:                 
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_SETUPREC:                    
#endif
            pResponseEnd = FMSTR_SetUpRec(pMessageIO);
            break;
            
        /* get recorder buffer information (force EX instead of non-EX) */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_GETRECBUFF_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#elif FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_GETRECBUFF:                  
#endif
            pResponseEnd = FMSTR_GetRecBuff(pMessageIO);
            break; 
#endif /* FMSTR_USE_RECORDER */
 
#if FMSTR_USE_APPCMD

        /* accept the application command */
        case FMSTR_CMD_SENDAPPCMD:
            pResponseEnd = FMSTR_StoreAppCmd(pMessageIO);
            break;

        /* get the application command status */
        case FMSTR_CMD_GETAPPCMDSTS:
            pResponseEnd = FMSTR_GetAppCmdStatus(pMessageIO);
            break;

        /* get the application command data */
        case FMSTR_CMD_GETAPPCMDDATA:
            pResponseEnd = FMSTR_GetAppCmdRespData(pMessageIO);
            break;
#endif /* FMSTR_USE_APPCMD */

#if FMSTR_USE_WRITEMEM

        /* write a block of memory */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_WRITEMEM_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif          
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_WRITEMEM:
#endif
            pResponseEnd = FMSTR_WriteMem(pMessageIO);
            break;
#endif /* FMSTR_USE_WRITEMEM */

#if FMSTR_USE_WRITEMEMMASK

        /* write block of memory with a bit mask */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_WRITEMEMMASK_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_WRITEMEMMASK:
#endif
            pResponseEnd = FMSTR_WriteMemMask(pMessageIO);
            break;
#endif /* FMSTR_USE_WRITEMEMMASK */
            
#if FMSTR_USE_WRITEVAR && FMSTR_USE_NOEX_CMDS

        /* write byte */
        case FMSTR_CMD_WRITEVAR8:
            pResponseEnd = FMSTR_WriteVar(pMessageIO, 1U);
            break;

        /* write word */
        case FMSTR_CMD_WRITEVAR16:
            pResponseEnd = FMSTR_WriteVar(pMessageIO, 2U);
            break;

        /* write dword */
        case FMSTR_CMD_WRITEVAR32:
            pResponseEnd = FMSTR_WriteVar(pMessageIO, 4U);
            break;
#endif /* FMSTR_USE_WRITEVAR && FMSTR_USE_NOEX_CMDS */

#if FMSTR_USE_WRITEVARMASK && FMSTR_USE_NOEX_CMDS

        /* write byte with mask */
        case FMSTR_CMD_WRITEVAR8MASK:
            pResponseEnd = FMSTR_WriteVarMask(pMessageIO, 1U);
            break;

        /* write word with mask */
        case FMSTR_CMD_WRITEVAR16MASK:
            pResponseEnd = FMSTR_WriteVarMask(pMessageIO, 2U);
            break;

#endif /* FMSTR_USE_WRITEVARMASK && FMSTR_USE_NOEX_CMDS */

#if defined FMSTR_USE_TSA

        /* get TSA table (force EX instead of non-EX) */
#if FMSTR_USE_EX_CMDS       
        case FMSTR_CMD_GETTSAINFO_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#elif FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_GETTSAINFO:
#endif
            pResponseEnd = FMSTR_GetTsaInfo(pMessageIO);
            break;
            
#if FMSTR_USE_EX_CMDS
        case FMSTR_CMD_GETSTRLEN_EX:
            FMSTR_SetExAddr(FMSTR_TRUE);
#endif
#if FMSTR_USE_NOEX_CMDS
        /*lint -fallthrough */
        case FMSTR_CMD_GETSTRLEN:
#endif
            pResponseEnd = FMSTR_GetStringLen(pMessageIO);
            break;

#endif /* FMSTR_USE_TSA */
#if 0
#if FMSTR_USE_BRIEFINFO
        /* retrieve a subset of board information structure */
        case FMSTR_CMD_GETINFOBRIEF:
#else   
        /* retrieve board information structure */
        case FMSTR_CMD_GETINFO: 
#endif
            pResponseEnd = FMSTR_GetBoardInfo(pMessageIO);
            break;
#endif

#if FMSTR_USE_SFIO
        case FMSTR_CMD_SFIOFRAME_0:
        case FMSTR_CMD_SFIOFRAME_1:
            pResponseEnd = FMSTR_SfioFrame(pMessageIO);
            break;
        case FMSTR_CMD_SFIOGETRESP_0:
        case FMSTR_CMD_SFIOGETRESP_1:
            pResponseEnd = FMSTR_SfioGetResp(pMessageIO);
            break;
#endif /* FMSTR_USE_SFIO */

#if defined FMSTR_USE_PIPES
        case FMSTR_CMD_PIPE:
            pResponseEnd = FMSTR_PipeFrame(pMessageIO);
            break;
#endif /* FMSTR_USE_PIPES */

        /* unknown command */
        default:
            pResponseEnd = FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STC_INVCMD);
            break;          
    }
    
    /* anything to send back? */
    if(pResponseEnd != pMessageIO)
    {
        /*lint -e{946,960} subtracting pointers is appropriate here */
        FMSTR_SIZE8 nSize = (FMSTR_SIZE8)(pResponseEnd - pMessageIO);
        FMSTR_SendResponse(FreeMasterPort, pMessageIO, nSize);
        return FMSTR_TRUE;
    }
    else
    {
        /* nothing sent out */
        return FMSTR_FALSE;
    }
}

/**************************************************************************//*!
*
* @brief    Handling GETINFO or GETINFO_BRIEF
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/
#if 0
FMSTR_BPTR FMSTR_GetBoardInfo(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_U16 wTmp;
    FMSTR_U8 *pStr;

    pResponse = FMSTR_ConstToBuffer8(pResponse, FMSTR_STS_OK);              
    pResponse = FMSTR_ConstToBuffer8(pResponse, (FMSTR_U8)(FMSTR_PROT_VER));            /* protVer */
    pResponse = FMSTR_ConstToBuffer8(pResponse, (FMSTR_U8)(FMSTR_CFG_FLAGS));           /* cfgFlags */
    pResponse = FMSTR_ConstToBuffer8(pResponse, (FMSTR_U8)(FMSTR_CFG_BUS_WIDTH));       /* dataBusWdt */
    pResponse = FMSTR_ConstToBuffer8(pResponse, (FMSTR_U8)(FMSTR_GLOB_VERSION_MAJOR));  /* globVerMajor */
    pResponse = FMSTR_ConstToBuffer8(pResponse, (FMSTR_U8)(FMSTR_GLOB_VERSION_MINOR));  /* globVerMinor */
    pResponse = FMSTR_ConstToBuffer8(pResponse, (FMSTR_U8)(FMSTR_COMM_BUFFER_SIZE));    /* cmdBuffSize  */

    /* that is all for brief info */
#if FMSTR_USE_BRIEFINFO
    FMSTR_UNUSED(pStr);
    FMSTR_UNUSED(wTmp);
    
#else /* FMSTR_USE_BRIEFINFO */

#if defined FMSTR_USE_RECORDER

    /* recorder buffer size is always mesured in bytes */
    wTmp = FMSTR_GetRecBuffSize();
    wTmp *= FMSTR_CFG_BUS_WIDTH;

    /* send size and timebase    */
    pResponse = FMSTR_ValueToBuffer16(pResponse, wTmp);
    pResponse = FMSTR_ConstToBuffer16(pResponse, (FMSTR_U16) FMSTR_REC_TIMEBASE);
#else /* FMSTR_USE_RECORDER */

    FMSTR_UNUSED(wTmp);

    /* recorder info zeroed */
    pResponse = FMSTR_ConstToBuffer16(pResponse, 0);
    pResponse = FMSTR_ConstToBuffer16(pResponse, 0);
#endif /* FMSTR_USE_RECORDER */

#if FMSTR_LIGHT_VERSION
FMSTR_UNUSED(pStr);
    pResponse = FMSTR_SkipInBuffer(pResponse, (FMSTR_U8)FMSTR_DESCR_SIZE);
#else
    /* description string */
    pStr = (FMSTR_U8*)  FMSTR_IDT_STRING;
    for(wTmp = 0U; wTmp < (FMSTR_U8)(FMSTR_DESCR_SIZE); wTmp++)
    {
        pResponse = FMSTR_ValueToBuffer8(pResponse, *pStr);

        /* terminating zero used to clear the remainder of the buffer */        
        if(*pStr)
        {
            pStr ++;
        }
    }
#endif /* SEND_IDT_STRING */
    
#endif /* FMSTR_USE_BRIEFINFO */

    return pResponse;   
}
#endif

/**************************************************************************//*!
*
* @brief    Handling READMEM and READMEM_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_ReadMem(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_ADDR nAddr;
    FMSTR_U8 nSize;

    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = FMSTR_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = FMSTR_AddressFromBuffer(&nAddr, pMessageIO);

#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
    if(!FMSTR_CheckTsaSpace(nAddr, (FMSTR_SIZE8) nSize, FMSTR_FALSE))
    {
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_EACCESS);
    }
#endif

    /* check the response will safely fit into comm buffer */
    if(nSize > (FMSTR_U8)FMSTR_COMM_BUFFER_SIZE)
    {
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_RSPBUFFOVF);
    }
    
    /* success  */
    pResponse = FMSTR_ConstToBuffer8(pResponse, FMSTR_STS_OK);
    
    return FMSTR_CopyToBuffer(pResponse, nAddr, (FMSTR_SIZE8) nSize);
}   

/**************************************************************************//*!
*
* @brief    Handling READVAR and READVAR_EX commands (for all sizes 1,2,4)
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_ReadVar(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_ADDR nAddr;

    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 1U);
    pMessageIO = FMSTR_AddressFromBuffer(&nAddr, pMessageIO);

#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
    if(!FMSTR_CheckTsaSpace(nAddr, nSize, FMSTR_FALSE))
    {
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_EACCESS);
    }
#endif

    /* success  */
    pResponse = FMSTR_ConstToBuffer8(pResponse, FMSTR_STS_OK);
    
    return FMSTR_CopyToBuffer(pResponse, nAddr, nSize);
}   

/**************************************************************************//*!
*
* @brief    Handling WRITEMEM and WRITEMEM_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_WriteMem(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_ADDR nAddr;
    FMSTR_U8 nSize,nResponseCode;

    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = FMSTR_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = FMSTR_AddressFromBuffer(&nAddr, pMessageIO);

#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
    if(!FMSTR_CheckTsaSpace(nAddr, (FMSTR_SIZE8) nSize, FMSTR_TRUE))
    {
        nResponseCode = FMSTR_STC_EACCESS;
        goto FMSTR_WriteMem_exit;
    }
#endif

    /*lint -e{534} ignoring function return value */
    FMSTR_CopyFromBuffer(nAddr, pMessageIO, (FMSTR_SIZE8) nSize);
    nResponseCode = FMSTR_STS_OK;

FMSTR_WriteMem_exit:
    return FMSTR_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling WRITEVAR command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
* @param    nSize - variable size
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_WriteVar(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_ADDR nAddr;
    FMSTR_U8 nResponseCode;

    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 1U);
    pMessageIO = FMSTR_AddressFromBuffer(&nAddr, pMessageIO);

#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
    if(!FMSTR_CheckTsaSpace(nAddr, nSize, FMSTR_TRUE))
    {
        nResponseCode = FMSTR_STC_EACCESS;
        goto FMSTR_WriteVar_exit;
    }
#endif

    /*lint -e{534} ignoring function return value */
    FMSTR_CopyFromBuffer(nAddr, pMessageIO, nSize);
    nResponseCode = FMSTR_STS_OK;

FMSTR_WriteVar_exit:
    return FMSTR_ConstToBuffer8(pResponse, nResponseCode);
}


/**************************************************************************//*!
*
* @brief    Handling WRITEMEMMASK and WRITEMEMMASK_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_WriteMemMask(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_ADDR nAddr;
    FMSTR_U8 nSize,nResponseCode;

    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = FMSTR_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = FMSTR_AddressFromBuffer(&nAddr, pMessageIO);

#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
    if(!FMSTR_CheckTsaSpace(nAddr, (FMSTR_SIZE8)nSize, FMSTR_TRUE))
    {
        nResponseCode = FMSTR_STC_EACCESS;
        goto FMSTR_WriteMemMask_exit;
    }
#endif

#if FMSTR_CFG_BUS_WIDTH > 1U
    /* size must be divisible by bus width (mask must not begin in half of memory word) */
    if(nSize % FMSTR_CFG_BUS_WIDTH)
    {
        nResponseCode = FMSTR_STC_INVSIZE;
        goto FMSTR_WriteMemMask_exit;
    }
#endif

    /* put the data */
    FMSTR_CopyFromBufferWithMask(nAddr, pMessageIO, (FMSTR_SIZE8)nSize);
    nResponseCode = FMSTR_STS_OK;

FMSTR_WriteMemMask_exit:
    return FMSTR_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling WRITEVARMASK command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
* @param    nSize - variable size
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_WriteVarMask(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_ADDR nAddr;
    FMSTR_U8 nResponseCode;
    
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 1U);
    pMessageIO = FMSTR_AddressFromBuffer(&nAddr, pMessageIO);

#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
    if(!FMSTR_CheckTsaSpace(nAddr, nSize, FMSTR_TRUE))
    {
        nResponseCode = FMSTR_STC_EACCESS;
        goto FMSTR_WriteVarMask_exit;
    }
#endif

    /* put the data */
    FMSTR_CopyFromBufferWithMask(nAddr, pMessageIO, nSize);         
    nResponseCode = FMSTR_STS_OK;

FMSTR_WriteVarMask_exit:
    return FMSTR_ConstToBuffer8(pResponse, nResponseCode);
}


/**************************************************************************//*!
*
* @brief    Private inline implementation of "strlen" used by TSA and Pipes
*
******************************************************************************/

FMSTR_U16 FMSTR_StrLen(FMSTR_ADDR nAddr)
{
    const FMSTR_U8* pStr;
    FMSTR_U16 nLen = 0U;

    #ifdef __HCS12X__
    /* convert from logical to global if needed */
    nAddr = FMSTR_FixHcs12xAddr(nAddr);
    #endif

    /*lint -e{923} casting address value to pointer */
    pStr = (const FMSTR_U8*) nAddr;
    
    while(*pStr++)
    {
        nLen++;
    }
        
    return nLen;
}


/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2011 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_rec.c
*
* @brief  FreeMASTER Recorder implementation.
*
* @version 1.0.3.0
* 
* @date Feb-6-2012
* 
*******************************************************************************/

//#include "freemaster.h"
//#include "freemaster_private.h"
//#include "freemaster_protocol.h"

#if defined FMSTR_USE_RECORDER

//#include "freemaster_rec.h"
/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_rec.h
*
* @brief  FreeMASTER Recorder implementation.
*
* @version 1.0.1.0
* 
* @date Mar-15-2011
* 
*******************************************************************************/

#ifndef __FREEMASTER_REC_H
#define __FREEMASTER_REC_H

#ifndef __FREEMASTER_H
#error Please include freemaster_rec.h after freemaster.h main header file.
#endif

//#include "freemaster.h"
//#include "freemaster_private.h"

/***********************************
*  global recorder types
***********************************/

/* recorder runtime flags */
typedef volatile union 
{
    FMSTR_FLAGS all;
    
    struct 
    {
        unsigned bIsConfigured : 1;     /* recorder is configured */
        unsigned bIsRunning : 1;        /* recorder is running */
        unsigned bIsStopping : 1;       /* trigger activated, sample countdown */
        unsigned bInvirginCycle : 1;    /* virgin cycle of the circular buffer in-progress */
        unsigned bTrgCrossActive : 1;   /* trigger trheshold was crossed */
    } flg;
    
} FMSTR_REC_FLAGS;

/* the same flags for optimized asm access (see fast recorder) */
#if defined(FMSTR_PLATFORM_56F8xxx) /* flag allocation tested on 56f8xxx only */
#define FMSTR_REC_FLAG_bIsConfigured   0x01
#define FMSTR_REC_FLAG_bIsRunning      0x02
#define FMSTR_REC_FLAG_bIsStopping     0x04
#define FMSTR_REC_FLAG_bInvirginCycle  0x08
#define FMSTR_REC_FLAG_bTrgCrossActive 0x10
#endif

/***********************************
*  global recorder variables 
***********************************/

/* configuration variables */
extern FMSTR_U16  pcm_wRecTotalSmps;        /* number of samples to measure */
extern FMSTR_U16  pcm_wRecPostTrigger;      /* number of post-trigger samples to keep */
extern FMSTR_U8   pcm_nRecTriggerMode;      /* triger mode (0 = disabled, 1 = _/, 2 = \_) */
extern FMSTR_U16  pcm_wRecTimeDiv;          /* divisor of recorder "clock" */

extern FMSTR_U8    pcm_nRecVarCount;        /* number of active recorder variables */
extern FMSTR_ADDR  pcm_pRecVarAddr[FMSTR_MAX_SCOPE_VARS]; /* addresses of recorded variables */
extern FMSTR_SIZE8 pcm_pRecVarSize[FMSTR_MAX_SCOPE_VARS]; /* sizes of recorded variables */

/* runtime variables  */
extern FMSTR_U16  pcm_wRecBuffStartIx;      /* first sample index */

extern FMSTR_ADDR pcm_dwRecWritePtr;        /* write pointer in recorder buffer */
extern FMSTR_ADDR pcm_dwRecEndBuffPtr;      /* pointer to end of active recorder buffer */
extern FMSTR_U16  pcm_wRecTimeDivCtr;       /* recorder "clock" divisor counter */
extern FMSTR_U16  pcm_wStoprecCountDown;    /* post-trigger countdown counter */

extern FMSTR_REC_FLAGS pcm_wRecFlags;


#endif /* __FREEMASTER_REC_H */


#if FMSTR_USE_FASTREC
#include "freemaster_fastrec.h"
#endif
void FMSTR_AbortRec(void);
/********************************************************
*  global variables (shared with FastRecorder if used)
********************************************************/

/* configuration variables */
FMSTR_U16  pcm_wRecTotalSmps;        /* number of samples to measure */

#if FMSTR_REC_STATIC_POSTTRIG == 0
FMSTR_U16  pcm_wRecPostTrigger;      /* number of post-trigger samples to keep */
#endif

#if (FMSTR_USE_FASTREC) == 0
FMSTR_U8   pcm_nRecTriggerMode;      /* triger mode (0 = disabled, 1 = _/, 2 = \_) */
#endif

#if (FMSTR_REC_STATIC_DIVISOR) == 0
FMSTR_U16  pcm_wRecTimeDiv;          /* divisor of recorder "clock" */
#endif

FMSTR_U8    pcm_nRecVarCount;        /* number of active recorder variables */
FMSTR_ADDR  pcm_pRecVarAddr[FMSTR_MAX_SCOPE_VARS]; /* addresses of recorded variables */
FMSTR_SIZE8 pcm_pRecVarSize[FMSTR_MAX_SCOPE_VARS]; /* sizes of recorded variables */

/* runtime variables  */
#if (FMSTR_REC_STATIC_DIVISOR) != 1
FMSTR_U16  pcm_wRecTimeDivCtr;       /* recorder "clock" divisor counter */
#endif

FMSTR_U16  pcm_wStoprecCountDown;    /* post-trigger countdown counter */

/* recorder flags */
FMSTR_REC_FLAGS pcm_wRecFlags;

/***********************************
*  local variables 
***********************************/

#if (FMSTR_USE_FASTREC) == 0
FMSTR_U16   pcm_wRecBuffStartIx;     /* first sample index */

/* Recorder buffer pointers */
FMSTR_ADDR pcm_dwRecWritePtr;        /* write pointer in recorder buffer */
FMSTR_ADDR pcm_dwRecEndBuffPtr;      /* pointer to end of active recorder buffer */

/* configuration variables */
static FMSTR_ADDR pcm_nTrgVarAddr;          /* trigger variable address */
static FMSTR_U8   pcm_nTrgVarSize;          /* trigger variable threshold size */
static FMSTR_U8   pcm_bTrgVarSigned;        /* trigger compare mode (0 = unsigned, 1 = signed) */

/*lint -e{960} using union */
static union 
{
#if FMSTR_CFG_BUS_WIDTH == 1
    FMSTR_U8  u8; 
    FMSTR_S8  s8;
#endif
    FMSTR_U16 u16;
    FMSTR_S16 s16;
    FMSTR_U32 u32;
    FMSTR_S32 s32;
} pcm_uTrgThreshold;                        /* trigger threshold level (1,2 or 4 bytes) */
#endif /* (FMSTR_USE_FASTREC) == 0 */

static FMSTR_ADDR  pcm_nRecBuffAddr;        /* recorder buffer address */
#if FMSTR_REC_OWNBUFF
static FMSTR_SIZE  pcm_wRecBuffSize;        /* recorder buffer size */
#endif

/* compare functions prototype */
typedef FMSTR_BOOL (*FMSTR_PCOMPAREFUNC)(void);

/*/ pointer to active compare function */
static FMSTR_PCOMPAREFUNC pcm_pCompareFunc;

#if !FMSTR_REC_OWNBUFF && (FMSTR_USE_FASTREC) == 0
/* put buffer into far memory ? */
#if FMSTR_REC_FARBUFF
#pragma section fardata begin
#endif /* FMSTR_REC_FARBUFF */
/* statically allocated recorder buffer (FMSTR_REC_OWNBUFF is FALSE) */
static FMSTR_U8 pcm_pOwnRecBuffer[FMSTR_REC_BUFF_SIZE];
/* end of far memory section */
#if FMSTR_REC_FARBUFF
#pragma section fardata end
#endif /* FMSTR_REC_FARBUFF */
#endif /* FMSTR_REC_OWNBUFF */

/***********************************
*  local functions
***********************************/

static FMSTR_BOOL FMSTR_Compare8S(void);
static FMSTR_BOOL FMSTR_Compare8U(void);
static FMSTR_BOOL FMSTR_Compare16S(void);
static FMSTR_BOOL FMSTR_Compare16U(void);
static FMSTR_BOOL FMSTR_Compare32S(void);
static FMSTR_BOOL FMSTR_Compare32U(void);
static void FMSTR_Recorder2(void);

/**************************************************************************//*!
*
* @brief    Recorder Initialization
*
******************************************************************************/

void FMSTR_InitRec(void)
{   
    /* initialize Recorder flags*/
    pcm_wRecFlags.all = 0U;

    /* setup buffer pointer and size so IsInRecBuffer works even  
       before the recorder is first initialized and used */
    
#if FMSTR_REC_OWNBUFF
    /* user wants to use his own buffer */
    pcm_nRecBuffAddr = 0U;
    pcm_wRecBuffSize = 0U;
#elif FMSTR_USE_FASTREC
    /* Initialize Fast Recorder Buffer  */
    FMSTR_InitFastRec();
#else
    /* size in native sizeof units (=bytes on most platforms) */
    FMSTR_ARR2ADDR(pcm_nRecBuffAddr, pcm_pOwnRecBuffer);
    
    /*lint -esym(528, pcm_pOwnRecBuffer) this symbol is used outside of lint sight */
#endif

}

/**************************************************************************//*!
*
* @brief    Abort and de-initialize recorder
*
******************************************************************************/
/*
#if defined(FMSTR_PLATFORM_HC08) || defined(FMSTR_PLATFORM_HC12)
#pragma INLINE
#else 
  #if defined(__ARMCC_VERSION)
__inline
  #else
inline
  #endif
#endif*/
void FMSTR_AbortRec(void)
{
    /* clear flags */
    pcm_wRecFlags.all = 0U;
}

/**************************************************************************//*!
*
* @brief    API: Replacing the recorder buffer with the user's one
*
* @param    pBuffer - user buffer pointer
* @param    wBuffSize - buffer size
*
* @note Use the FMSTR_SetUpBuff32 to pass the forced 32bit address in SDM
*
******************************************************************************/

void FMSTR_SetUpRecBuff(FMSTR_ADDR pBuffer, FMSTR_SIZE nBuffSize)
{    
#if FMSTR_REC_OWNBUFF
    pcm_nRecBuffAddr = pBuffer;
    pcm_wRecBuffSize = nBuffSize;
#else
    FMSTR_UNUSED(pBuffer);
    FMSTR_UNUSED(nBuffSize);    
#endif
}

/**************************************************************************//*!
*
* @brief    Handling SETUPREC and SETUPREC_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_SetUpRec(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_SIZE8 nRecVarsetSize;
    FMSTR_SIZE blen;
    FMSTR_U8 i, sz;
    FMSTR_U8 nResponseCode;
    
    /* de-initialize first   */
    FMSTR_AbortRec();

#if FMSTR_REC_OWNBUFF
    /* user wants to use his own buffer, check if it is valid */
    if(!pcm_nRecBuffAddr || !pcm_wRecBuffSize)
    {
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_INVBUFF);
    }
#elif (FMSTR_USE_FASTREC) == 0
    /* size in native sizeof units (=bytes on most platforms) */
    FMSTR_ARR2ADDR(pcm_nRecBuffAddr, pcm_pOwnRecBuffer);
#endif

    /* seek the setup data */
#if (FMSTR_USE_FASTREC) == 0
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = FMSTR_ValueFromBuffer8(&pcm_nRecTriggerMode, pMessageIO);
#else /* (FMSTR_USE_FASTREC) == 0 */
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 3U);
#endif /* (FMSTR_USE_FASTREC) == 0 */

    pMessageIO = FMSTR_ValueFromBuffer16(&pcm_wRecTotalSmps, pMessageIO);

#if (FMSTR_REC_STATIC_POSTTRIG) == 0
    pMessageIO = FMSTR_ValueFromBuffer16(&pcm_wRecPostTrigger, pMessageIO);
#else /* (FMSTR_REC_STATIC_POSTTRIG) == 0 */
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 2U);
#endif /* (FMSTR_REC_STATIC_POSTTRIG) == 0 */

#if (FMSTR_REC_STATIC_DIVISOR) == 0
    pMessageIO = FMSTR_ValueFromBuffer16(&pcm_wRecTimeDiv, pMessageIO);
#else /* (FMSTR_REC_STATIC_DIVISOR) == 0 */
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 2U);
#endif /* (FMSTR_REC_STATIC_DIVISOR) == 0 */

#if (FMSTR_USE_FASTREC) == 0
    /* address & size of trigger variable */
    pMessageIO = FMSTR_AddressFromBuffer(&pcm_nTrgVarAddr, pMessageIO);
    pMessageIO = FMSTR_ValueFromBuffer8(&pcm_nTrgVarSize, pMessageIO);

    /* trigger compare mode  */
    pMessageIO = FMSTR_ValueFromBuffer8(&pcm_bTrgVarSigned, pMessageIO);

    /* threshold value  */
    pMessageIO = FMSTR_ValueFromBuffer32(&pcm_uTrgThreshold.u32, pMessageIO);
#else /* (FMSTR_USE_FASTREC) == 0 */
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 8U);
#endif /* (FMSTR_USE_FASTREC) == 0 */
    
    /* recorder variable count */
    pMessageIO = FMSTR_ValueFromBuffer8(&pcm_nRecVarCount, pMessageIO);

    /* rec variable information must fit into our buffers */
    if(!pcm_nRecVarCount || pcm_nRecVarCount > (FMSTR_U8)FMSTR_MAX_REC_VARS)
    {
#if FMSTR_REC_COMMON_ERR_CODES
        goto FMSTR_SetUpRec_exit_error;
#else
        nResponseCode = FMSTR_STC_INVBUFF;
        goto FMSTR_SetUpRec_exit;
#endif
    }

    /* calculate sum of sizes of all variables */
    nRecVarsetSize = 0U;

    /* get all addresses and sizes */
    for(i=0U; i<pcm_nRecVarCount; i++)
    {
        /* variable size */
        pMessageIO = FMSTR_ValueFromBuffer8(&sz, pMessageIO);
        
        pcm_pRecVarSize[i] = sz;
        nRecVarsetSize += sz;
        
        /* variable address */
        pMessageIO = FMSTR_AddressFromBuffer(&pcm_pRecVarAddr[i], pMessageIO);

        /* valid numeric variable sizes only */
        if(sz == 0U || sz > 8U)
        {
#if FMSTR_REC_COMMON_ERR_CODES
            goto FMSTR_SetUpRec_exit_error;
#else
            nResponseCode = FMSTR_STC_INVSIZE;
            goto FMSTR_SetUpRec_exit;
#endif
        }

#if FMSTR_CFG_BUS_WIDTH > 1U
        /* even sizes only */
        if(sz & 0x1)
        {
#if FMSTR_REC_COMMON_ERR_CODES
            goto FMSTR_SetUpRec_exit_error;
#else
            nResponseCode = FMSTR_STC_INVSIZE;
            goto FMSTR_SetUpRec_exit;
#endif
        }
#endif /* FMSTR_CFG_BUS_WIDTH > 1U */
        
#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
        if(!FMSTR_CheckTsaSpace(pcm_pRecVarAddr[i], (FMSTR_SIZE8)sz, 0U))
        {
#if FMSTR_REC_COMMON_ERR_CODES
            goto FMSTR_SetUpRec_exit_error;
#else
            nResponseCode = FMSTR_STC_EACCESS;
            goto FMSTR_SetUpRec_exit;
#endif
        }
#endif /* FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY */
    }

    /* fast recorder handles trigger by itself */
#if (FMSTR_USE_FASTREC) == 0
    /* any trigger? */
    pcm_pCompareFunc = NULL;
    if(pcm_nRecTriggerMode)
    {
        /* access to trigger variable? */
#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
        if(!FMSTR_CheckTsaSpace(pcm_nTrgVarAddr, (FMSTR_SIZE8)pcm_nTrgVarSize, 0U))
        {
#if FMSTR_REC_COMMON_ERR_CODES
            goto FMSTR_SetUpRec_exit_error;
#else
            nResponseCode = FMSTR_STC_EACCESS;
            goto FMSTR_SetUpRec_exit;
#endif
        }
#endif /* FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY */
        /* get compare function */
        switch(pcm_nTrgVarSize)
        {
#if FMSTR_CFG_BUS_WIDTH == 1U
        case 1: pcm_pCompareFunc = pcm_bTrgVarSigned ? FMSTR_Compare8S : FMSTR_Compare8U; break;
#endif
        case 2: pcm_pCompareFunc = pcm_bTrgVarSigned ? FMSTR_Compare16S : FMSTR_Compare16U; break;
        case 4: pcm_pCompareFunc = pcm_bTrgVarSigned ? FMSTR_Compare32S : FMSTR_Compare32U; break;
        
        /* invalid trigger variable size  */
        default:
#if FMSTR_REC_COMMON_ERR_CODES
            goto FMSTR_SetUpRec_exit_error;
#else
            nResponseCode = FMSTR_STC_INVSIZE;
            goto FMSTR_SetUpRec_exit;
#endif
        }
    }
#endif /* (FMSTR_USE_FASTREC) == 0 */
    
    /* total recorder buffer length in native sizeof units (=bytes on most platforms) */
    blen = (FMSTR_SIZE) (pcm_wRecTotalSmps * nRecVarsetSize / FMSTR_CFG_BUS_WIDTH);

    /* recorder memory available? */
    if(blen > FMSTR_GetRecBuffSize())
    {
#if FMSTR_REC_COMMON_ERR_CODES
        goto FMSTR_SetUpRec_exit_error;
#else
        nResponseCode = FMSTR_STC_INVSIZE;
        goto FMSTR_SetUpRec_exit;
#endif
    }

#if (FMSTR_USE_FASTREC) == 0
    /* remember the effective end of circular buffer */
    pcm_dwRecEndBuffPtr = pcm_nRecBuffAddr + blen;
#endif /* (FMSTR_USE_FASTREC) == 0 */

#if FMSTR_USE_FASTREC
    if(!FMSTR_SetUpFastRec())
    {
#if FMSTR_REC_COMMON_ERR_CODES
        goto FMSTR_SetUpRec_exit_error;
#else /* FMSTR_REC_COMMON_ERR_CODES */
        nResponseCode = FMSTR_STC_FASTRECERR;
        goto FMSTR_SetUpRec_exit;
#endif /* FMSTR_REC_COMMON_ERR_CODES */
    }
#endif /* FMSTR_USE_FASTREC */

    /* everything is okay    */
    pcm_wRecFlags.flg.bIsConfigured = 1U;
    nResponseCode = FMSTR_STS_OK;
#if FMSTR_REC_COMMON_ERR_CODES
    goto FMSTR_SetUpRec_exit;
FMSTR_SetUpRec_exit_error:
    nResponseCode = FMSTR_STC_INVSIZE;
#endif
FMSTR_SetUpRec_exit:
    return FMSTR_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    API: Pull the trigger of the recorder
*
* This function starts the post-trigger stop countdown
*
******************************************************************************/

void FMSTR_TriggerRec(void)
{
    if(!pcm_wRecFlags.flg.bIsStopping)
    {
        pcm_wRecFlags.flg.bIsStopping = 1U;
#if (FMSTR_REC_STATIC_POSTTRIG) == 0
        pcm_wStoprecCountDown = pcm_wRecPostTrigger;
#else
        pcm_wStoprecCountDown = FMSTR_REC_STATIC_POSTTRIG;
#endif
    }
}

/**************************************************************************//*!
*
* @brief    Handling STARTREC command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the length
*           of the response filled into the buffer (including status byte)
*
* This function starts recording (initializes internal recording variables 
* and flags)
*
******************************************************************************/

FMSTR_BPTR FMSTR_StartRec(FMSTR_BPTR pMessageIO)
{
     FMSTR_U8 nResponseCode;   
    /* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
#if FMSTR_REC_COMMON_ERR_CODES
        goto FMSTR_StartRec_exit_error;
#else
        nResponseCode = FMSTR_STC_NOTINIT;
        goto FMSTR_StartRec_exit;
#endif
    }
        
    /* already running ? */
    if(pcm_wRecFlags.flg.bIsRunning)
    {
#if FMSTR_REC_COMMON_ERR_CODES
        goto FMSTR_StartRec_exit_error;
#else
        nResponseCode = FMSTR_STS_RECRUN;
        goto FMSTR_StartRec_exit;
#endif
    }

#if (FMSTR_USE_FASTREC) == 0
    /* initialize write pointer */
    pcm_dwRecWritePtr = pcm_nRecBuffAddr;

    /* current (first) sample index */
    pcm_wRecBuffStartIx = 0U;
#endif /* (FMSTR_USE_FASTREC) == 0 */

    /* initialize time divisor */
#if (FMSTR_REC_STATIC_DIVISOR) != 1
    pcm_wRecTimeDivCtr = 0U;
#endif

    /* initiate virgin cycle */
    pcm_wRecFlags.flg.bIsStopping = 0U;          /* no trigger active */
    pcm_wRecFlags.flg.bTrgCrossActive = 0U;      /* waiting for threshold crossing */
    pcm_wRecFlags.flg.bInvirginCycle = 1U;       /* initial cycle */
    /* run now */

    /* start fast recorder */
#if FMSTR_USE_FASTREC
    FMSTR_StartFastRec();
#endif /* (MSTR_USE_FASTREC */

    /* run now */
    pcm_wRecFlags.flg.bIsRunning = 1U;           /* is running now! */

    nResponseCode = FMSTR_STS_OK;
#if FMSTR_REC_COMMON_ERR_CODES
    goto FMSTR_StartRec_exit;
FMSTR_StartRec_exit_error:
    nResponseCode = FMSTR_STC_NOTINIT;
#endif

FMSTR_StartRec_exit:
    return FMSTR_ConstToBuffer8(pMessageIO, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling STOPREC command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the length
*           of the response filled into the buffer (including status byte)
*
* This function stops recording (same as manual trigger)
*
******************************************************************************/

FMSTR_BPTR FMSTR_StopRec(FMSTR_BPTR pMessageIO)
{
    FMSTR_U8 nResponseCode;
    /* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
        nResponseCode = FMSTR_STC_NOTINIT;
        goto FMSTR_StopRec_exit;
    }
        
    /* already stopped ? */
    if(!pcm_wRecFlags.flg.bIsRunning)
    {
        nResponseCode = FMSTR_STS_RECDONE;
        goto FMSTR_StopRec_exit;
    }
    
    /* simulate trigger */
    FMSTR_TriggerRec();
    nResponseCode = FMSTR_STS_OK;
    
FMSTR_StopRec_exit:
    return FMSTR_ConstToBuffer8(pMessageIO, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling GETRECSTS command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
* This function returns current recorder status
*
******************************************************************************/

FMSTR_BPTR FMSTR_GetRecStatus(FMSTR_BPTR pMessageIO)
{
    FMSTR_U16 nResponseCode = (FMSTR_U16) (pcm_wRecFlags.flg.bIsRunning ? 
        FMSTR_STS_RECRUN : FMSTR_STS_RECDONE);
    
    /* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
        nResponseCode = FMSTR_STC_NOTINIT;
    }
        
    /* get run/stop status */
    return FMSTR_ConstToBuffer8(pMessageIO, (FMSTR_U8) nResponseCode);
}

/* now follows the recorder only routines, skip that if FastRecorder is used */
#if (FMSTR_USE_FASTREC) == 0

/**************************************************************************//*!
*
* @brief    Get recorder memory size 
*
* @return   Recorder memory size in native sizeof units (=bytes on most platforms)
*
******************************************************************************/

FMSTR_SIZE FMSTR_GetRecBuffSize()
{
#if FMSTR_REC_OWNBUFF
    return pcm_wRecBuffSize;
#else
    return (FMSTR_SIZE) FMSTR_REC_BUFF_SIZE;
#endif
}

/**************************************************************************//*!
*
* @brief    Check wether given memory region is inside the recorder buffer
*
* @param    dwAddr - address of the memory to be checked
* @param    wSize  - size of the memory to be checked
*
* @return   This function returns non-zero if user space is in recorder buffer
*
* This function is called as a pert of TSA-checking process when the PC host
* is requesting memory contents
*
******************************************************************************/

FMSTR_BOOL FMSTR_IsInRecBuffer(FMSTR_ADDR dwAddr, FMSTR_SIZE8 nSize)
{
    FMSTR_BOOL bRet = 0U;
    
    if(dwAddr >= pcm_nRecBuffAddr)
    {
        bRet = (FMSTR_BOOL)((dwAddr + nSize) <= (pcm_nRecBuffAddr + FMSTR_GetRecBuffSize()) ? FMSTR_TRUE : FMSTR_FALSE);    
    }
    
    return bRet;
}


/**************************************************************************//*!
*
* @brief    Handling GETRECBUFF and GETRECBUFF_EX command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
* This function returns recorder buffer information
*
******************************************************************************/

FMSTR_BPTR FMSTR_GetRecBuff(FMSTR_BPTR pMessageIO)
{
	 volatile FMSTR_BPTR pResponse;
	/* must be configured */
    if(!pcm_wRecFlags.flg.bIsConfigured)
    {
        return FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STC_NOTINIT);
    }
    
    /* must be stopped */
    if(pcm_wRecFlags.flg.bIsRunning)
    {
        return FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STC_SERVBUSY);
    }
    
    /* fill the return info */
    pResponse = FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STS_OK);
    pResponse = FMSTR_AddressToBuffer(pResponse, pcm_nRecBuffAddr);
    return FMSTR_ValueToBuffer16(pResponse, pcm_wRecBuffStartIx);
}

/**************************************************************************//*!
*
* @brief    Compare macro used in trigger detection
*
* @param    v - original command 
* @param    t - response buffer
*
* @return   zero when value is lower than threshold.
* @return   non-zero when value is greater than or equal as treshold
*
******************************************************************************/

#define CMP(v,t) ((FMSTR_BOOL)(((v) < (t)) ? 0 : 1))

#if FMSTR_CFG_BUS_WIDTH == 1U

static FMSTR_BOOL FMSTR_Compare8S()
{
    return CMP(FMSTR_GetS8(pcm_nTrgVarAddr), pcm_uTrgThreshold.s8);
}

static FMSTR_BOOL FMSTR_Compare8U()
{
    return CMP(FMSTR_GetU8(pcm_nTrgVarAddr), pcm_uTrgThreshold.u8);
}

#endif

static FMSTR_BOOL FMSTR_Compare16S()
{
    return CMP(FMSTR_GetS16(pcm_nTrgVarAddr), pcm_uTrgThreshold.s16);
}

static FMSTR_BOOL FMSTR_Compare16U()
{
    return CMP(FMSTR_GetU16(pcm_nTrgVarAddr), pcm_uTrgThreshold.u16);
}

static FMSTR_BOOL FMSTR_Compare32S()
{
    return CMP(FMSTR_GetS32(pcm_nTrgVarAddr), pcm_uTrgThreshold.s32);
}

static FMSTR_BOOL FMSTR_Compare32U()
{
    return CMP(FMSTR_GetU32(pcm_nTrgVarAddr), pcm_uTrgThreshold.u32);
}

/**************************************************************************//*!
*
* @brief    API: Recorder worker routine - can be called from application's timer ISR
*
*
* This returns quickly if recorder is not running, otherwise it calls quite lengthy 
* recorder routine which does all the recorder work (sampling, triggering)
*
******************************************************************************/

#if defined(FMSTR_PLATFORM_56F8xxx) || defined(FMSTR_PLATFORM_56F8xx)
#pragma interrupt called
#endif

void FMSTR_Recorder(void)
{
    /* recorder not active */
    if(!pcm_wRecFlags.flg.bIsRunning)
    {
        return ;
    }
    
    /* do the hard work      */
    FMSTR_Recorder2();
}

/**************************************************************************//*!
*
* @brief    Recorder function called when recorder is active
*
******************************************************************************/

#if defined(FMSTR_PLATFORM_56F8xxx) || defined(FMSTR_PLATFORM_56F8xx)
#pragma interrupt called
#endif

static void FMSTR_Recorder2(void)
{
    FMSTR_SIZE8 sz;
    FMSTR_BOOL cmp;
    FMSTR_U8 i;

#if (FMSTR_REC_STATIC_DIVISOR) != 1
    /* skip this call ? */
    if(pcm_wRecTimeDivCtr)
    {
        /* maybe next time... */
        pcm_wRecTimeDivCtr--;
        return;
    }
    
    /* re-initialize divider */
#if (FMSTR_REC_STATIC_DIVISOR) == 0
    pcm_wRecTimeDivCtr = pcm_wRecTimeDiv;
#else 
    pcm_wRecTimeDivCtr = FMSTR_REC_STATIC_DIVISOR;
#endif /* (FMSTR_REC_STATIC_DIVISOR) == 0 */
#endif /* (FMSTR_REC_STATIC_DIVISOR) != 1 */

    /* take snapshot of variable values */
    for (i=0U; i<pcm_nRecVarCount; i++)
    {
        sz = pcm_pRecVarSize[i];
        FMSTR_CopyMemory(pcm_dwRecWritePtr, pcm_pRecVarAddr[i], sz);
        sz /= FMSTR_CFG_BUS_WIDTH;
        pcm_dwRecWritePtr += sz;
    }
    
    /* another sample taken (startIx "points" after sample just taken) */
    /* i.e. it points to the oldest sample */
    pcm_wRecBuffStartIx++;
    
    /* wrap around (circular buffer) ? */
    if(pcm_dwRecWritePtr >= pcm_dwRecEndBuffPtr)
    {   
        pcm_dwRecWritePtr = pcm_nRecBuffAddr;
        pcm_wRecFlags.flg.bInvirginCycle = 0U;
        pcm_wRecBuffStartIx = 0U;
    }

    /* no trigger testing in virgin cycle */
    if(pcm_wRecFlags.flg.bInvirginCycle)
    {
        return;
    }
    
    /* test trigger condition if still running */
    if(!pcm_wRecFlags.flg.bIsStopping && pcm_pCompareFunc != NULL)
    {
        /* compare trigger threshold */
        cmp = pcm_pCompareFunc();
        
        /* negated logic (falling-edge) ? */
        if(pcm_nRecTriggerMode == 2U)
        {
            cmp = (FMSTR_BOOL) !cmp;
        }
        
        /* above threshold ? */
        if(cmp)
        {
            /* were we at least once below threshold ? */
            if(pcm_wRecFlags.flg.bTrgCrossActive)
            {
                /* EDGE TRIGGER ! */
                FMSTR_TriggerRec();
            }
        }
        else
        {
            /* we got bellow threshold, now wait for being above threshold */
            pcm_wRecFlags.flg.bTrgCrossActive = 1U;
        }
    }
    
    /* in stopping mode ? (note that this bit might have been set just above!) */
    if(pcm_wRecFlags.flg.bIsStopping)
    {
        /* count down post-trigger samples expired ? */
        if(!pcm_wStoprecCountDown)
        {
            /* STOP RECORDER */
            pcm_wRecFlags.flg.bIsRunning = 0U;
            return;
        }
        
        /* perhaps next time */
        pcm_wStoprecCountDown--;
    }
}

#endif /* (FMSTR_USE_FASTREC) == 0 */

#else /* FMSTR_USE_RECORDER */

/* use void recorder API functions */
void FMSTR_Recorder(void) 
{ 
}

void FMSTR_TriggerRec(void) 
{ 
}

void FMSTR_SetUpRecBuff(FMSTR_ADDR pBuffer, FMSTR_SIZE wBuffSize) 
{ 
    FMSTR_UNUSED(pBuffer);
    FMSTR_UNUSED(wBuffSize);
}

/*lint -efile(766, freemaster_protocol.h) include file is not used in this case */

#endif /* FMSTR_USE_RECORDER */


/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_scope.c
*
* @brief  FreeMASTER Oscilloscope implementation
*
* @version 1.0.1.0
* 
* @date May-4-2012
* 
*******************************************************************************/

//#include "freemaster.h"
//#include "freemaster_private.h"
//#include "freemaster_protocol.h"

#if defined FMSTR_USE_SCOPE

/***********************************
*  local variables 
***********************************/

static FMSTR_U8  pcm_nScopeVarCount;        /* number of active scope variables */
static FMSTR_ADDR  pcm_pScopeVarAddr[FMSTR_MAX_SCOPE_VARS]; /* addresses of scope variables */
static FMSTR_SIZE8 pcm_pScopeVarSize[FMSTR_MAX_SCOPE_VARS]; /* sizes of scope variables */

/**************************************************************************//*!
*
* @brief    Scope Initialization
*
******************************************************************************/

void FMSTR_InitScope(void)
{   
}

/**************************************************************************//*!
*
* @brief    Handling SETUPSCOPE and SETUPSCOPE_EX command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_SetUpScope(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_U8 i, sz, nVarCnt;

    /* uninitialize scope */
    pcm_nScopeVarCount = 0U;

    /* seek the setup data */
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 2U);
    
    /* scope variable count  */
    pMessageIO = FMSTR_ValueFromBuffer8(&nVarCnt, pMessageIO);

    /* scope variable information must fit into our buffers */
    if(!nVarCnt || nVarCnt > (FMSTR_U8)FMSTR_MAX_SCOPE_VARS)
    {
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_INVBUFF);
    }
    
    /* get all addresses and sizes */
    for(i=0U; i<nVarCnt; i++)
    {
        /* variable size */
        pMessageIO = FMSTR_ValueFromBuffer8(&sz, pMessageIO);
        pcm_pScopeVarSize[i] = sz;
        
        /* variable address */
        pMessageIO = FMSTR_AddressFromBuffer(&pcm_pScopeVarAddr[i], pMessageIO);

        /* valid numeric variable sizes only */
        if(sz == 0U || sz > 8U)
        {
            return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_INVSIZE);
        }
        
#if FMSTR_CFG_BUS_WIDTH > 1U
        /* even sizes only */
        if(sz & 0x1)
        {
            return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_INVSIZE);
        }
#endif
        
#if defined FMSTR_USE_TSA && FMSTR_USE_TSA_SAFETY
        if(!FMSTR_CheckTsaSpace(pcm_pScopeVarAddr[i], (FMSTR_SIZE8) sz, 0U))
        {
            return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_EACCESS);
        }
#endif
        
    }

    /* activate scope */
    pcm_nScopeVarCount = nVarCnt;

    /* return just a status */
    return FMSTR_ConstToBuffer8(pResponse, FMSTR_STS_OK);
}

/**************************************************************************//*!
*
* @brief    Handling READSCOPE command
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

FMSTR_BPTR FMSTR_ReadScope(FMSTR_BPTR pMessageIO)
{
    FMSTR_U8 i;
    
    if(!pcm_nScopeVarCount)
    {
        return FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STC_NOTINIT);
    }
    
    /* success */
    pMessageIO = FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STS_OK);
    
    for (i=0U; i<pcm_nScopeVarCount; i++)
    {
        pMessageIO = FMSTR_CopyToBuffer(pMessageIO, pcm_pScopeVarAddr[i], pcm_pScopeVarSize[i]);
    } 
        
    /* return end position */
    return pMessageIO;  
}

#else  /* FMSTR_USE_SCOPE */

/*lint -efile(766, freemaster_protocol.h) include file is not used in this case */

#endif /* FMSTR_USE_SCOPE */


/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2011 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   freemaster_Kxx.c
*
* @brief  FreeMASTER Driver Kxx-hardware dependent stuff
*
* @version 1.0.3.0
* 
* @date Aug-4-2011
* 
*******************************************************************************/

//#include "freemaster.h"
//#include "freemaster_private.h"
//#include "freemaster_Kxx.h"

#if !FMSTR_DISABLE
/*******************************************************************************
*
* @brief    API: Main SCI/CAN Interrupt handler call
*
* This Interrupt Service Routine handles the SCI/CAN interrupts for the FreeMASTER 
* driver. In case you want to handle the interrupt in the application yourselves,
* call the FMSTR_ProcessSCI/FMSTR_ProcessCanRx and FMSTR_ProcessCanTx which does 
* the same job but is not compiled as an Interrupt Service Routine.
*
* In poll-driven mode (FMSTR_POLL_DRIVEN) this function does nothing.
*
*******************************************************************************/

void FMSTR_Isr(void)
{
#if FMSTR_LONG_INTR || FMSTR_SHORT_INTR

/* process serial interface */
#if defined FREEMASTER_UART
    FMSTR_ProcessSCI(); 

/* process CAN interface */    
#elif FMSTR_USE_CAN
    FMSTR_ProcessCanRx();
    FMSTR_ProcessCanTx();
#endif
    
#endif /* FMSTR_LONG_INTR || FMSTR_SHORT_INTR */
}

/**************************************************************************//*!
*
* @brief    The "memcpy" used internally in FreeMASTER driver
*
* @param    nDestAddr - destination memory address
* @param    nSrcAddr  - source memory address
* @param    nSize     - memory size (always in bytes)
*
******************************************************************************/

void FMSTR_CopyMemory(FMSTR_ADDR nDestAddr, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) nSrcAddr;
    FMSTR_U8* pd = (FMSTR_U8*) nDestAddr;
    
    while(nSize--)
        *pd++ = *ps++;
}

/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

FMSTR_BPTR FMSTR_CopyToBuffer(FMSTR_BPTR pDestBuff, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) nSrcAddr;
    FMSTR_U8* pd = (FMSTR_U8*) pDestBuff;
#if defined _WINDOWS
        while(nSize--)
        *pd++ = 0;
#else
    while(nSize--)
        *pd++ = *ps++;
#endif
        
    return (FMSTR_BPTR) pd;
}

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

FMSTR_BPTR FMSTR_CopyFromBuffer(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) pSrcBuff;
    FMSTR_U8* pd = (FMSTR_U8*) nDestAddr;
    
    while(nSize--)
        *pd++ = *ps++;
        
    return (FMSTR_BPTR) ps;
}

#if (FMSTR_BUFFER_ACCESS_BY_FUNCT)
FMSTR_BPTR FMSTR_ValueFromBuffer16(FMSTR_U16* pDest, FMSTR_BPTR pSrc)
{
    return FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pDest, pSrc, 2);
}

FMSTR_BPTR FMSTR_ValueFromBuffer32(FMSTR_U32* pDest, FMSTR_BPTR pSrc)
{
    return FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pDest, pSrc, 4);
}

FMSTR_BPTR FMSTR_ValueToBuffer16(FMSTR_BPTR pDest, FMSTR_U16 src)
{
    return FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)(FMSTR_U8*)&src, 2);
}

FMSTR_BPTR FMSTR_ValueToBuffer32(FMSTR_BPTR pDest, FMSTR_U32 src)
{
    return FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)(FMSTR_U8*)&src, 4);
}
#endif

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

void FMSTR_CopyFromBufferWithMask(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize)
{
    FMSTR_U8* ps = (FMSTR_U8*) pSrcBuff;
    FMSTR_U8* pd = (FMSTR_U8*) nDestAddr;
    FMSTR_U8* pm = ps + nSize;
    FMSTR_U8 mask, stmp, dtmp;
    
    while(nSize--) 
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;
        
        /* perform AND-masking */
        stmp = (FMSTR_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back       */
        *pd++ = stmp;
    }
}

/******************************************************************************/

/* mixed EX and no-EX commands? */
#if FMSTR_USE_EX_CMDS && FMSTR_USE_NOEX_CMDS || (FMSTR_BUFFER_ACCESS_BY_FUNCT)

/**************************************************************************//*!
*
* @brief  When mixed EX and no-EX command may occur, this variable is
*         here to remember what command is just being handled.
*
******************************************************************************/

static FMSTR_BOOL pcm_bNextAddrIsEx;

void FMSTR_SetExAddr(FMSTR_BOOL bNextAddrIsEx)
{
    pcm_bNextAddrIsEx = bNextAddrIsEx;
}

/**************************************************************************//*!
*
* @brief  Store address to communication buffer. The address may be 
*         32 or 16 bit wide (based on previous call to FMSTR_SetExAddr)
*
******************************************************************************/

FMSTR_BPTR FMSTR_AddressToBuffer(FMSTR_BPTR pDest, FMSTR_ADDR nAddr)
{
    if(pcm_bNextAddrIsEx)
    {
        /* fill in the 32bit address */
      pDest = FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)&nAddr, 4);
     // *(FMSTR_U32*) pDest = ((FMSTR_U32)nAddr);
     //   pDest += 4;
    }
    else
    {
        /* fill in the 16bit address (never used) */
      pDest = FMSTR_CopyToBuffer(pDest, (FMSTR_ADDR)&nAddr, 2);
      //*(FMSTR_U16*) pDest = ((FMSTR_U16)nAddr);
      //  pDest += 2; 
    }
    
    return pDest;
}

/**************************************************************************//*!
*
* @brief  Fetch address from communication buffer
*
******************************************************************************/

FMSTR_BPTR FMSTR_AddressFromBuffer(FMSTR_ADDR* pAddr, FMSTR_BPTR pSrc)
{
    if(pcm_bNextAddrIsEx)
    {
         pSrc = FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pAddr, pSrc, 4);
//      *pAddr = (FMSTR_ADDR) *((FMSTR_U32*) pSrc);
//        pSrc += 4;
    }
    else
    {
         pSrc = FMSTR_CopyFromBuffer((FMSTR_ADDR)(FMSTR_U8*)pAddr, pSrc, 2);
      //*pAddr = (FMSTR_ADDR) *((FMSTR_U16*) pSrc);
      // pSrc += 2;
    }

    return pSrc;
}

#endif /* mixed EX and no-EX commands */
#endif /* !FMSTR_DISABLE */








#if defined FREEMASTER_CDC || defined FREEMASTER_UART

#define FREEMASTER_START_OF_MESSAGE            0x2b

#define FREEMASTER_FAST_COMMAND                0xc0
#define FREEMASTER_FAST_COMMAND_MASK           0xc0
#define FREEMASTER_FAST_COMMAND_GET_INFO       0xc0
#define FREEMASTER_FAST_COMMAND_START_REC      0xc1
#define FREEMASTER_FAST_COMMAND_STOP_REC       0xc2
#define FREEMASTER_FAST_COMMAND_GET_REC_STATUS 0xc3
#define FREEMASTER_FAST_COMMAND_GET_REC_BUFFER 0xc4
#define FREEMASTER_FAST_COMMAND_GET_SCOPE_DATA 0xc5
#define FREEMASTER_FAST_COMMAND_GET_CMD_STATUS 0xc6
#define FREEMASTER_FAST_COMMAND_GET_BRIEF_INFO 0xc8
#define FREEMASTER_FAST_COMMAND_GET_REC_DATA   0xc9
#define FREEMASTER_FAST_COMMAND_GET_LAST_SFIO0 0xca
#define FREEMASTER_FAST_COMMAND_GET_LAST_SFIO1 0xcb

#define FREEMASTER_READ_MEM                    0x01
#define FREEMASTER_WRITE_MEM                   0x02
#define FREEMASTER_WRITE_MEMMASK               0x03
#define FREEMASTER_READ_MEM_BLOCK              0x04
#define FREEMASTER_WRITE_MEM_BLOCK             0x05
#define FREEMASTER_WRITE_MEM_ASK_BLOCK         0x06
#define FREEMASTER_SETUP_SCOPE                 0x08
#define FREEMASTER_SETUP_RECORDER              0x09
#define FREEMASTER_SETUP_SCOPE_EXTENDED        0x0a
#define FREEMASTER_SETUP_RECORDER_EXTENDED     0x0b
#define FREEMASTER_SEND_APP_COMMAND            0x10
#define FREEMASTER_GET_TAS_INFO                0x11
#define FREEMASTER_GET_TAS_INFO_32             0x12
#define FREEMASTER_SEND_EXECUTE_SFIO1          0x13
#define FREEMASTER_SEND_EXECUTE_SFIO0          0x14
#define FREEMASTER_READ_WRITE_PIPE_DATA        0x15

#define FREEMASTER_OPERATION_COMPLETED_OK      0x00
#define FREEMASTER_DATA_RECORDER_RUNNING       0x01
#define FREEMASTER_DATA_RECORDER_STOPPED       0x02

#define FREEMASTER_ERROR_UNKNOWN_COMMAND       0x81
#define FREEMASTER_ERROR_CHECKSUM_ERROR        0x82
#define FREEMASTER_ERROR_COMMAND_TOO_LONG      0x83

#define FREEMASTER_RX_STATE_HUNTING            0
#define FREEMASTER_RX_STATE_RECEIVING_CMD      1
#define FREEMASTER_RX_STATE_RECEIVING_LENGTH   2
#define FREEMASTER_RX_STATE_RECEIVING          3

#define FREEMASTER_PROTOCOL_VERSION            3
#define FREEMASTER_CONFIG_LITTLE_ENDIAN        0x00
#define FREEMASTER_CONFIG_BIG_ENDIAN           0x01
#define FREEMASTER_CONFIG_REC_LARGE_MODE       0x10
#define FREEMASTER_CONFIG_BUS_WIDTH            1
#define FREEMASTER_GLOBAL_VERSION_MAJOR        2
#define FREEMASTER_GLOBAL_VERSION_MINOR        0

#define RECODER_BUFFER_SIZE                    512
#define RECORDER_TIME_BASE                     16384                     // ms

#define FREEMASTER_TSA_VERSION                 2
#define FREEMASTER_TSA_INFO_16BIT              0x0000
#define FREEMASTER_TSA_INFO_32BIT              0x0100

#define TSA_FLAGS                              (FREEMASTER_TSA_INFO_32BIT | FREEMASTER_TSA_VERSION)

static unsigned long fnGetTableByIndex(unsigned short usTableIndex, unsigned short *ptr_usTableSize)
{
    // Dummy at the moment
    //
    if (usTableIndex == 0) {
        *ptr_usTableSize =  0x1b0;
        return 0x000070d0;
    }
    else {
        *ptr_usTableSize = 0;
        return 0;
    }
}

static int fnCheckFreeMasterCRC(unsigned char *ptr_ucFreemasterRxBuffer, int iFreemasterRxCnt)
{
    unsigned char ucCheckSum = 0;
    while (iFreemasterRxCnt-- != 0) {
        ucCheckSum += *ptr_ucFreemasterRxBuffer++;
    }
    return (ucCheckSum != 0);
}

static void fnFreeMasterResponse(QUEUE_HANDLE comHandle, unsigned char *ptr_ucFreemasterBuffer, int iLength)
{
    unsigned char ucChecksum = FREEMASTER_OPERATION_COMPLETED_OK;
    int iCalculateLength = iLength;
    unsigned char *ptrData = ptr_ucFreemasterBuffer;
    *ptr_ucFreemasterBuffer++ = FREEMASTER_START_OF_MESSAGE;
    *ptr_ucFreemasterBuffer++ = FREEMASTER_OPERATION_COMPLETED_OK;
    while (iCalculateLength-- != 0) {
        ucChecksum += *ptr_ucFreemasterBuffer++;
    }
    ucChecksum = ~ucChecksum;
    *ptr_ucFreemasterBuffer = (ucChecksum + 1);                          // add the CRC to the end
    fnWrite(comHandle, ptrData, (QUEUE_TRANSFER)(iLength + 3));          // length plus start bytes, status and CRC
}

static void fnFreeMasterErrorResponse(QUEUE_HANDLE comHandle, unsigned char ucCommand)
{
    unsigned char ucErrorMessage[3];
    ucErrorMessage[0] = FREEMASTER_START_OF_MESSAGE;
    ucErrorMessage[1] = ucCommand;
    ucErrorMessage[2] = (~ucCommand + 1);
    fnWrite(comHandle, ucErrorMessage, sizeof(ucErrorMessage));
}

// Handle reception from the FreeMaster run-time debug host (UART or USB-CDC interface)
//
extern void fnHandleFreeMaster(QUEUE_HANDLE comHandle, unsigned char *ptr_ucBuffer, QUEUE_TRANSFER Length)
{
    static int iFreemasterRxState = FREEMASTER_RX_STATE_HUNTING;
    static int iFreemasterRxCnt = 0;
    static int iRemainingRx = 0;
    static unsigned char ucFreemasterBuffer[128];
    while (Length--) {                                                   // for each byte in this received block
        switch (iFreemasterRxState) {
        case FREEMASTER_RX_STATE_HUNTING:
            if (*ptr_ucBuffer == FREEMASTER_START_OF_MESSAGE) {          // searching for the start byte
                iFreemasterRxState = FREEMASTER_RX_STATE_RECEIVING_CMD;
                iFreemasterRxCnt = 0;
            }
            break;
        case FREEMASTER_RX_STATE_RECEIVING_CMD:
            ucFreemasterBuffer[iFreemasterRxCnt++] = *ptr_ucBuffer;      // save to the input buffer
            if ((*ptr_ucBuffer & FREEMASTER_FAST_COMMAND_MASK) == FREEMASTER_FAST_COMMAND) { // single byte fast command
                iFreemasterRxState = FREEMASTER_RX_STATE_RECEIVING;
                iRemainingRx = (((*ptr_ucBuffer & 0x30) >> 3) + 1);
            }
            else {
                iFreemasterRxState = FREEMASTER_RX_STATE_RECEIVING_LENGTH;
            }
            break;
        case FREEMASTER_RX_STATE_RECEIVING_LENGTH:
            iRemainingRx = (*ptr_ucBuffer + 1);
            ucFreemasterBuffer[iFreemasterRxCnt++] = *ptr_ucBuffer;      // save length to the input buffer
            iFreemasterRxState = FREEMASTER_RX_STATE_RECEIVING;
            break;
        case FREEMASTER_RX_STATE_RECEIVING:
            if (iFreemasterRxCnt >= (sizeof(ucFreemasterBuffer) - 1)) {  // protect the input buffer
                fnFreeMasterErrorResponse(comHandle, FREEMASTER_ERROR_COMMAND_TOO_LONG);
                iFreemasterRxState = FREEMASTER_RX_STATE_HUNTING;
            }
            else {
                ucFreemasterBuffer[iFreemasterRxCnt++] = *ptr_ucBuffer;  // save to the input buffer
                if (--iRemainingRx == 0) {                               // final CRC byte received
                    if (fnCheckFreeMasterCRC(ucFreemasterBuffer, iFreemasterRxCnt) != 0) {
                        fnFreeMasterErrorResponse(comHandle, FREEMASTER_ERROR_CHECKSUM_ERROR);
                    }
                    else {
                        switch (ucFreemasterBuffer[0]) {                 // recognise and handle the command
                        case FREEMASTER_FAST_COMMAND_GET_BRIEF_INFO:
                        case FREEMASTER_FAST_COMMAND_GET_INFO:
                            {
                                uMemset(&ucFreemasterBuffer[8], 0, (37 - 8)); // fill with zeroes to end of message buffer
                                ucFreemasterBuffer[2] = FREEMASTER_PROTOCOL_VERSION;
                                ucFreemasterBuffer[3] = FREEMASTER_CONFIG_LITTLE_ENDIAN;
                                ucFreemasterBuffer[4] = FREEMASTER_CONFIG_BUS_WIDTH;
                                ucFreemasterBuffer[5] = FREEMASTER_GLOBAL_VERSION_MAJOR;
                                ucFreemasterBuffer[6] = FREEMASTER_GLOBAL_VERSION_MINOR;
                                ucFreemasterBuffer[7] = (unsigned char)(sizeof(ucFreemasterBuffer) - 4);
    #if RECODER_BUFFER_SIZE != 0
                                ucFreemasterBuffer[8] = (unsigned char)(RECODER_BUFFER_SIZE * 1);
                                ucFreemasterBuffer[9] = (unsigned char)((RECODER_BUFFER_SIZE * 1) >> 8);
                                ucFreemasterBuffer[10] = (unsigned char)(RECORDER_TIME_BASE);
                                ucFreemasterBuffer[11] = (unsigned char)(RECORDER_TIME_BASE >> 8);
    #endif
                                uStrcpy((CHAR *)&ucFreemasterBuffer[12], "Kxx FreeMASTER"); // maximum 23 character string
                                fnFreeMasterResponse(comHandle, ucFreemasterBuffer, 35);
                            }
                            break;
                        case FREEMASTER_FAST_COMMAND_START_REC:
                        case FREEMASTER_FAST_COMMAND_STOP_REC:
                        case FREEMASTER_FAST_COMMAND_GET_REC_STATUS:
                        case FREEMASTER_FAST_COMMAND_GET_REC_BUFFER:
                        case FREEMASTER_FAST_COMMAND_GET_SCOPE_DATA:
                        case FREEMASTER_FAST_COMMAND_GET_CMD_STATUS:
                        case FREEMASTER_FAST_COMMAND_GET_REC_DATA:
                        case FREEMASTER_FAST_COMMAND_GET_LAST_SFIO0:
                        case FREEMASTER_FAST_COMMAND_GET_LAST_SFIO1:

                        case FREEMASTER_READ_MEM:
                        case FREEMASTER_WRITE_MEM:
                        case FREEMASTER_WRITE_MEMMASK:
                        case FREEMASTER_WRITE_MEM_BLOCK:
                        case FREEMASTER_WRITE_MEM_ASK_BLOCK:
                        case FREEMASTER_SETUP_SCOPE:
                        case FREEMASTER_SETUP_RECORDER:
                        case FREEMASTER_SETUP_SCOPE_EXTENDED:
                        case FREEMASTER_SETUP_RECORDER_EXTENDED:
                        case FREEMASTER_SEND_APP_COMMAND:
                        case FREEMASTER_GET_TAS_INFO:
                        case FREEMASTER_SEND_EXECUTE_SFIO1:
                        case FREEMASTER_SEND_EXECUTE_SFIO0:
                        case FREEMASTER_READ_WRITE_PIPE_DATA:
                        default:
    #if defined FREESCALE_FREEMASTER
                            FMSTR_ProtocolDecoder(ucFreemasterBuffer);   // original Freescale handling
    #else
                            fnFreeMasterErrorResponse(comHandle, FREEMASTER_ERROR_UNKNOWN_COMMAND);
    #endif
                            break;
                        case FREEMASTER_READ_MEM_BLOCK:
    #if defined FREESCALE_FREEMASTER
                            FMSTR_ProtocolDecoder(ucFreemasterBuffer);   // original Freescale handling
    #else
                            {
                                unsigned long ulLocation;
                                unsigned char ucBufferLength = ucFreemasterBuffer[2]; // this will be the size of the bufffer that we reported n response to FREEMASTER_FAST_COMMAND_GET_INFO
                                ulLocation = (ucFreemasterBuffer[3] | (ucFreemasterBuffer[4] << 8) | (ucFreemasterBuffer[5] << 16) | (ucFreemasterBuffer[6] << 24));
    #if defined FREEMASTER_STORAGE_ACCESS
                                fnGetParsFile((unsigned char *)ulLocation, &ucFreemasterBuffer[2], ucBufferLength); // using storage collection allows reading in virtual memory mapped medium such as SPI Flash
    #else
                                uMemcpy(&ucFreemasterBuffer[2], (const void *)ulLocation, ucBufferLength); // memory mapped access
    #endif
                                fnFreeMasterResponse(comHandle, ucFreemasterBuffer, ucBufferLength);
                            }
    #endif
                            break;
                        case FREEMASTER_GET_TAS_INFO_32:
    #if defined FREESCALE_FREEMASTER
                            FMSTR_ProtocolDecoder(ucFreemasterBuffer);   // original Freescale handling
    #else
                            {
                                unsigned short usTableIndex = (ucFreemasterBuffer[2] | (ucFreemasterBuffer[3] << 8)); // index of the table being requested
                                unsigned short usTableSize;
                                unsigned long ulTableLocation = fnGetTableByIndex(usTableIndex, &usTableSize);
                                ucFreemasterBuffer[2] = (unsigned char)(TSA_FLAGS);
                                ucFreemasterBuffer[3] = (unsigned char)(TSA_FLAGS >> 8);
                                ucFreemasterBuffer[4] = (unsigned char)usTableSize;
                                ucFreemasterBuffer[5] = (unsigned char)(usTableSize >> 8);
                                ucFreemasterBuffer[6] = (unsigned char)ulTableLocation;
                                ucFreemasterBuffer[7] = (unsigned char)(ulTableLocation >> 8);
                                ucFreemasterBuffer[8] = (unsigned char)(ulTableLocation >> 16);
                                ucFreemasterBuffer[9] = (unsigned char)(ulTableLocation >> 24);
                                fnFreeMasterResponse(comHandle, ucFreemasterBuffer, 8);
                            }
    #endif
                            break;
                        }
                    }
                    iFreemasterRxState = FREEMASTER_RX_STATE_HUNTING;
                }
            }
            break;
        default:
            break;
        }
        ptr_ucBuffer++;
    }
}
#endif

#if defined FREEMASTER_UART
extern QUEUE_HANDLE fnOpenFreeMasterUART(void)
{
    QUEUE_HANDLE FreemasterPortID;
    TTYTABLE tInterfaceParameters;                                       // table for passing information to driver
    tInterfaceParameters.Channel = FREEMASTER_UART_CH;                   // set UART channel for serial use
    tInterfaceParameters.ucSpeed = SERIAL_BAUD_57600;                    // baud rate
    tInterfaceParameters.Rx_tx_sizes.RxQueueSize = 64;                   // input buffer size
    tInterfaceParameters.Rx_tx_sizes.TxQueueSize = 128;                  // output buffer size
    tInterfaceParameters.Task_to_wake = TASK_APPLICATION;                // wake application task when messages have been received
    #if defined SUPPORT_FLOW_HIGH_LOW
    tInterfaceParameters.ucFlowHighWater = 80;                           // set the flow control high and low water levels in %
    tInterfaceParameters.ucFlowLowWater = 20;
    #endif
    tInterfaceParameters.Config = (CHAR_8 + NO_PARITY + ONE_STOP + CHAR_MODE);
    #if defined SERIAL_SUPPORT_DMA
    tInterfaceParameters.ucDMAConfig = UART_TX_DMA;                      // activate DMA on transmission
    #endif
    if ((FreemasterPortID = fnOpen(TYPE_TTY, FOR_I_O, &tInterfaceParameters)) != NO_ID_ALLOCATED) { // open the channel with defined configurations (initially inactive)
        fnDriver(FreemasterPortID, (TX_ON | RX_ON), 0);                  // enable rx and tx
    #if defined FMSTR_USE_TSA
        FMSTR_InitTsa();                                                 // initialize target side address translation
    #endif
    #if defined FMSTR_USE_SCOPE
        FMSTR_InitScope();                                               // initialise scope
    #endif
    #if defined FMSTR_USE_RECORDER
        FMSTR_InitRec();                                                 // initialise recorder
    #endif
    #if defined FMSTR_USE_APPCMD
        FMSTR_InitAppCmds();                                             // initialise application commands
    #endif
    #if defined FMSTR_USE_SFIO
        FMSTR_InitSfio();                                                // initialise SFIO encapsulation layer
    #endif
    #if defined FMSTR_USE_PIPES
        FMSTR_InitPipes();                                               // initialise PIPES interface
    #endif
    #if defined FREEMASTER_UART
        FMSTR_InitSerial();                                              // initialise communication and start listening for commands
    #endif  
    #if defined FMSTR_USE_CAN
        FMSTR_InitCan();                                                 // initialise CAN communication
    #endif
    }
    return FreemasterPortID;
}
#endif
#endif
