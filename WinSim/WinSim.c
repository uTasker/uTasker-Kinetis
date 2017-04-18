/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      WinSim.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    28.04.2007 Added function fnGetEEPROMSize()                          {1}
    11.08.2007 Added SPI Data FLASH to support the AT45DBXXX             {2}
    23.08.2007 Set initialised flag                                      {3}
    12.09.2007 Add multiple SPI FLASH support                            {4}
    23.09.2007 Add page delete command to SPI FLASH                      {5}
    21.10.2007 Add test sequence for USB enumeration                     {6}
    02.11.2007 Add RTS control support                                   {7}
    17.11.2007 Allow SPI and FLASH files system to work together         {8}
    15.12.2007 Add I2C speed support for improved simulation accuracy    {9}
    15.12.2007 Simplified UART speed limitiation                         {10}
    23.12.2007 Extend UART speed limitiation to DMA                      {11}
    25.12.2007 Extend UART support to 4 UARTS                            {12}
    08.01.2008 Ethernet code made conditional on Ethernet availability   {13}
    12.02.2008 Add support for SPI FLASH STM25P64                        {14}
    18.02.2008 Use memset/memcpy rather than uMemset/uMemcpy to avoid limitations when DMA used {15}
    18.03.2008 Remove chip select parameter from fnSimAT45DBXXX() and fnSimSTM25Pxxx() and control it locally {16}
    21.03.2008 Add BULK ERASE support to STM25Pxxx SPI FLASH             {17}
    25.03.2008 Improve AT45DBXXX flash write accuracy by using & rather than copy {18}
    28.04.2008 Add negative toggle (for ADC use)                         {19}
    08.05.2008 Protect WinPCap from main thread by ignoring TICK if WinPCap is simulating {20} - invalidated with {35}
    23.05.2008 Add AT45BDxxxD page size simulation support and make manufacturing ID answer type dependent {21}
    28.06.2008 Add SST SPI FLASH support                                 {22}
    16.07.2008 Add USB tx logging                                        {23}
    16.07.2008 Protect main thread from Ethernet frame injections        {24} - invalidated by {35}
    16.07.2008 Close all log files on exit                               {25}
    19.07.2008 Add USB disconnect and OUT data simulations               {26}
    19.07.2008 Add UART break condition                                  {27}
    31.08.2008 Extend I2C support to 3 channels                          {28}
    19.09.2008 Add USB host support                                      {29}
    19.09.2008 Add USB set up data injection                             {30}
    20.10.2008 Add ATMEL SPI FLASH auto-page rewrite command             {31}
    25.10.2008 Complete SST SPI FLASH simulation support
    09.01.2009 Test USB transaction interruption during enumeration      {32}
    09.01.2009 Add missing status stage during enumeration sequence      {33}
    17.01.2009 Don't schedule on not accepted Ethernet frames            {34}
    18.01.2009 Generally return WAIT_WHILE_BUSY when the simulator is already working {35}
    18.01.2009 Protect all interrupt injections                          {36}
    25.01.2009 Correct AT45 SP FLASH address mask                        {37}
    03.02.2009 Only close USB logs when they are open                    {38}
    08.02.2009 Extra parameter passed with fnSimulateEthernetIn()        {39}
    24.02.2009 Change UART channel configuration to UART_MODE_CONFIG     {40}
    24.02.2009 Extend RTS control support to UARTs 1,2 and 3             {41}
    03.03.2009 Add iForce parameter to fnPortChanges()                   {42}
    14.03.2009 Allow zero length USB data to be injected                 {43}
    17.03.2009 Add Rx UART3                                              {44}
    10.06.2009 Rename usConfig to Config in UART configuration           {45}
    01.07.2009 Adapt for compatibility with STRING_OPTIMISATION          {46}
    24.08.2009 Enable DMA when SSC is active and optionally log SSC output {47}
    15.10.2009 Add additional AT45DB and ST25M SPI FLASH sizes           {48}
    22.10.2009 Add additional 4 UARTS                                    {49}
    19.12.2009 Allow CR and LF to be logged to serial output file        {50}
    10.04.2010 Adapt PORT_CHANGE message to support more ports           {51}
    14.09.2010 Correctly recognise the size of large ATMEL SPI FLASH devices {52}
    22.09.2010 Add power of twos mode for ATMEL SPI FLASH                {53}
    28.09.2010 Only send opens of external UARTs when a mapped port is defined {54}
    10.10.2010 Add EFCR register support to external UART interface      {55}
    31.07.2011 Extend to 6 internal UARTs                                {56}
    03.08.2011 Add zero-configuration state display                      {57}
    02.10.2011 Add fnInitExtFlash(), fnGetExtFlashStart() and fnGetExtFlashSize() {58}
    24.12.2011 Close KOMODO CAN DUO when exiting CAN simulation          {59}
    12.01.2012 Correct ucGetDescriptorDevice content                     {60}
    26.01.2012 Adapt USB enumeration to support HID mouse                {61}
    04.04.2012 Add external port display                                 {62}
    04.04.2012 Adapt network parameters for compatibility with multiple networks {63}
    04.06.2013 Avoid overwriting last MAC address digit when displaying DHCP state {64}
    05.08.2013 Add dedicated analogue port for Kinetis                   {65}
    23.11.2013 Add USB-CDC line coding test after enumeration            {66}
    28.02.2014 Add token parameter to fnLogUSB() and display setup frames{67}
    14.04.2014 Display multiple network IP/MAC addresses                 {68}
    16.05.2015 USB-CDC line coding test performed for each CDC interface {69}
    15.01.2016 Add SPI_FLASH_W25Q128 IDs                                 {70}
    24.12.2016 Add I2C data injection                                    {71}
    02.02.2017 Adapt for us tick resolution
    13.02.2017 Get endpoint size of Host from endpoint 0 (kinetis)       {72}
    28.02.2017 Increase UARTs from 6 to 8                                {73}
 
*/   
#include <windows.h>
#include "conio.h"
#include "Fcntl.h"
#include "io.h"
#include <sys/stat.h>
#if _VC80_UPGRADE>=0x0600
    #include <share.h>
#endif


#define OPSYS_CONFIG
#define _EXCLUDE_WINDOWS_
#define _WINSIM_INCLUDE_
#include "config.h"

#include <stdlib.h>

extern void fnInitHW(void);

#if !defined USB_CDC_COUNT
    #define USB_CDC_COUNT          1
#endif

#if !defined _EXTERNAL_PORT_COUNT                                        // {62} for compatibility
    #define _EXTERNAL_PORT_COUNT   0
#endif

#if !defined IP_NETWORK_COUNT                                            // {68}
    #define IP_NETWORK_COUNT 1
#endif

#if defined _KINETIS                                                     // {65}
    #if defined KINETIS_KE
        #define _PORTS_AVAILABLE PORTS_AVAILABLE_8_BIT
    #else
        #define _PORTS_AVAILABLE (PORTS_AVAILABLE + 1)                   // add dedicated ADC port
    #endif
#else
    #define _PORTS_AVAILABLE PORTS_AVAILABLE
#endif

extern int iInts = 0;
extern int iMasks = 0;
extern int iDMA = 0;

static const unsigned char ucHelloWorld[] = "Hello, World!!";
static const char cText1[]                = "ETHERNET TX :";

static int            iEthTxFile = 0;
static unsigned long  ulActions = 0;
static unsigned long  ulActions_2 = 0;
static unsigned long  ulChannel0Speed;
static UART_MODE_CONFIG Channel0Config;                                  // {40}
static unsigned long  ulChannel1Speed;
static UART_MODE_CONFIG Channel1Config;                                  // {40}
static unsigned long  ulChannel2Speed;
static UART_MODE_CONFIG Channel2Config;                                  // {40}
static unsigned long  ulChannel3Speed;                                   // {12}
static UART_MODE_CONFIG Channel3Config;                                  // {40}
static unsigned long  ulChannel4Speed;                                   // {56}
static UART_MODE_CONFIG Channel4Config;
static unsigned long  ulChannel5Speed;
static UART_MODE_CONFIG Channel5Config;
static unsigned long  ulChannel6Speed;                                   // {73}
static UART_MODE_CONFIG Channel6Config;
static unsigned long  ulChannel7Speed;
static UART_MODE_CONFIG Channel7Config;
static int iChannel0Speed = 0;                                           // {10}
static int iChannel1Speed = 0;
static int iChannel2Speed = 0;
static int iChannel3Speed = 0;                                           // {12}
static int iChannel4Speed = 0;                                           // {56}
static int iChannel5Speed = 0;
static int iChannel6Speed = 0;                                           // {73}
static int iChannel7Speed = 0;
                                                 
static int iI2C_Channel0Speed = 0;                                       // {9}
static int iI2C_Channel1Speed = 0;
static int iI2C_Channel2Speed = 0;                                       // {28}
static int iI2C_Channel3Speed = 0;

#if NUMBER_EXTERNAL_SERIAL > 0                                           // {49}
    static unsigned long  ulExtChannel0Speed;
    static unsigned long  ulExtChannel1Speed;
    static unsigned long  ulExtChannel2Speed;
    static unsigned long  ulExtChannel3Speed;
    static UART_MODE_CONFIG ExtChannel0Config;
    static UART_MODE_CONFIG ExtChannel1Config;
    static UART_MODE_CONFIG ExtChannel2Config;
    static UART_MODE_CONFIG ExtChannel3Config;
    static int iExtChannel0Speed = 0;
    static int iExtChannel1Speed = 0;
    static int iExtChannel2Speed = 0;
    static int iExtChannel3Speed = 0;
#endif

#if defined USB_INTERFACE
    #define MAX_USB_LENGTH 1024
    static int iUSB_Log[NUMBER_OF_USB_ENDPOINTS] = {0};
    static unsigned char  ucUSB_input[NUMBER_OF_USB_ENDPOINTS][MAX_USB_LENGTH];
    static unsigned short usUSB_length[NUMBER_OF_USB_ENDPOINTS] = {0};
    static const CHAR cUSB_reset[] = "USB Bus Reset..\r\n";
    static const CHAR cUSB_stall[] = "USB Stall\r\n";
    #if defined USB_HOST_SUPPORT
        static int iHostMode = 0;
    #endif
#endif


#define UART_BUFFER_LENGTH 10000                                         // buffer length for single UART message

static unsigned long  ulCom0Len = 0;
static unsigned char  ucCom0Data[UART_BUFFER_LENGTH];
static unsigned long  ulCom1Len = 0;
static unsigned char  ucCom1Data[UART_BUFFER_LENGTH];
static unsigned long  ulCom2Len = 0;
static unsigned char  ucCom2Data[UART_BUFFER_LENGTH];
static unsigned long  ulCom3Len = 0;                                     // {12}
static unsigned char  ucCom3Data[UART_BUFFER_LENGTH];
static unsigned long  ulCom4Len = 0;                                     // {56}
static unsigned char  ucCom4Data[UART_BUFFER_LENGTH];
static unsigned long  ulCom5Len = 0;                                     // {56}
static unsigned char  ucCom5Data[UART_BUFFER_LENGTH];
static unsigned long  ulCom6Len = 0;                                     // {73}
static unsigned char  ucCom6Data[UART_BUFFER_LENGTH];
static unsigned long  ulCom7Len = 0;
static unsigned char  ucCom7Data[UART_BUFFER_LENGTH];

#if NUMBER_EXTERNAL_SERIAL > 0                                           // {49}
    static unsigned long  ulExtCom0Len = 0;
    static unsigned char  ucExtCom0Data[UART_BUFFER_LENGTH];
    static unsigned long  ulExtCom1Len = 0;
    static unsigned char  ucExtCom1Data[UART_BUFFER_LENGTH];
    static unsigned long  ulExtCom2Len = 0;
    static unsigned char  ucExtCom2Data[UART_BUFFER_LENGTH];
    static unsigned long  ulExtCom3Len = 0;
    static unsigned char  ucExtCom3Data[UART_BUFFER_LENGTH];
#endif

static int  fnSimulateActions(char *argv[]);
static void fnCloseAll(void);
extern void RealTimeInterrupt(void);

#if defined ETH_INTERFACE                                                // {13}
    #if LOSE_ACKS > 0
    static int fnRandomPacketLoss(int iLossRate);
    #endif
    extern int iWinPcapSending;
#else
    static int iWinPcapSending = 0;
#endif


extern int main(int argc, char *argv[])
{
    int iRun = 0;
    int iSimulate = 0;
    int iReset;
    static iInitialised = 0;
    static int iOpSysActive = 0;
    static CRITICAL_SECTION cs;
    static CRITICAL_SECTION *ptrcs = 0;
#if defined MULTISTART
    static MULTISTART_TABLE *ptrNewstart = 0;
    unsigned char *pucHeapStart;
#endif

    if (ptrcs == 0) {
        ptrcs = &cs;
        InitializeCriticalSection(ptrcs);                                // start of critical region
    }
    EnterCriticalSection(ptrcs);                                         // protect from task switching
    if (iOpSysActive != 0) {                                             // {35}
        LeaveCriticalSection(ptrcs);
        return WAIT_WHILE_BUSY;                                          // event can not be handled at the moment so it should wait
    }
    iOpSysActive = 1;                                                    // {36} flag that the simulator is busy
    LeaveCriticalSection(ptrcs);

    switch (argc) {
    case INITIALISE_OP_SYSTEM:
        fnInitTime(argv);
        fnSetProjectDetails(++argv);
	    fnInitHW();                                                      // initialise hardware  
        fnSimPorts();                                                    // ensure simulator is aware of any hardware port initialisations
#if defined MULTISTART
        if (ptMultiStartTable == 0) {                                    // normal startup
_abort_multi:
            fnInitialiseHeap(ctOurHeap, fnGetHeapStart());               // create standard heap
	        uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES);// start the operating system with standard table
            iInitialised = 1;                                            // {3}
            iOpSysActive = 0;                                            // {36}
            return 0;
        }
        else {                                                           // start using information from the application table
            if (ptMultiStartTable->new_hw_init) {
    #if defined DYNAMIC_MULTISTART
                pucHeapStart = ptMultiStartTable->new_hw_init(JumpTable, &ptMultiStartTable, OurConfigNr);
    #else
                pucHeapStart = ptMultiStartTable->new_hw_init(JumpTable);
    #endif
                if (pucHeapStart == 0) {
                    goto _abort_multi;                                   // this can happen if the jump table version doesn't match - prefer to stay in boot mode than start an application which will crash
                }
            }
            else {
                pucHeapStart = fnGetHeapStart();
            }
            fnInitialiseHeap(ptMultiStartTable->ptHeapNeed, pucHeapStart); // initialise next heap
                                                                         // re-start the operating system
            uTaskerStart((UTASKTABLEINIT *)ptMultiStartTable->ptTaskTable, ptMultiStartTable->ptNodesTable, PHYSICAL_QUEUES);
        }
#else
        fnInitialiseHeap(ctOurHeap, fnGetHeapStart());                   // create heap
	    uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system
#endif
        iRun = 5;
        iInitialised = 1;
        break;

#if defined MULTISTART
    case INITIALISE_OP_SYS_2:
        if (ptrNewstart->new_hw_init) {
    #if defined DYNAMIC_MULTISTART
            pucHeapStart = ptrNewstart->new_hw_init(JumpTable, &ptrNewstart, OurConfigNr);
    #else
            pucHeapStart = ptrNewstart->new_hw_init(JumpTable);
    #endif
        }
        else {
            pucHeapStart = fnGetHeapStart();
        }
        fnInitialiseHeap(ptrNewstart->ptHeapNeed, pucHeapStart);         // initialise next heap
                                                                         // re-start the operating system
        uTaskerStart((UTASKTABLEINIT *)ptrNewstart->ptTaskTable, ptrNewstart->ptNodesTable, PHYSICAL_QUEUES);
        break;
#endif

    case TICK_CALL:
        if (iReset = fnSimTimers()) {                                    // simulate timers and check for watchdog timer
            iOpSysActive = 0;                                            // {36}
            return iReset;                                               // reset commanded or Watchdog fired
        }
        RealTimeInterrupt();                                             // simulate a timer tick
        iRun = 10;                                                       // allow the scheduler to run a few times to handle inter-task events
        iSimulate = 1;                                                   // flag that simulation flags should be handled
        break;

#if defined SUPPORT_KEY_SCAN
    case KEY_CHANGE:
        fnSimulateKeyChange((int *)argv[0]);
        break;
#endif

    case INPUT_TOGGLE_NEG_ANALOG:
        fnSimulateInputChange((unsigned char)*argv[0], (unsigned char)*argv[1], (TOGGLE_INPUT_NEG | TOGGLE_INPUT_ANALOG));
        break;
    case INPUT_TOGGLE_POS_ANALOG:
        fnSimulateInputChange((unsigned char)*argv[0], (unsigned char)*argv[1], (TOGGLE_INPUT_POS | TOGGLE_INPUT_ANALOG));
        break;
    case INPUT_TOGGLE_POS:
        fnSimulateInputChange((unsigned char)*argv[0], (unsigned char)*argv[1], TOGGLE_INPUT_POS);
        break;
    case INPUT_TOGGLE_NEG:                                               // {19}
        fnSimulateInputChange((unsigned char)*argv[0], (unsigned char)*argv[1], TOGGLE_INPUT_NEG);
        break;
    case INPUT_TOGGLE_ANALOG:
        fnSimulateInputChange((unsigned char)*argv[0], (unsigned char)*argv[1], (TOGGLE_INPUT | TOGGLE_INPUT_ANALOG));
        break;
    case INPUT_TOGGLE:
        fnSimulateInputChange((unsigned char)*argv[0], (unsigned char)*argv[1], TOGGLE_INPUT);
        break;

    case RX_COM0:                                                        // the first 6 UART inputs are assigned to internal UARTs (there may be less available)
        fnSimulateSerialIn(0, (unsigned char *)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_COM1:
        fnSimulateSerialIn(1, (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_COM2:
        fnSimulateSerialIn(2, (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_COM3:                                                        // {44}
        fnSimulateSerialIn(3, (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_COM4:                                                        // {56}
        fnSimulateSerialIn(4, (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_COM5:                                                        // {56}
        fnSimulateSerialIn(5, (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
#if NUMBER_EXTERNAL_SERIAL > 0                                           //  {49}
    case RX_EXT_COM0:                                                    // external UART inputs  (there may be less available)
        fnSimulateSerialIn(NUMBER_SERIAL, (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_EXT_COM1:
        fnSimulateSerialIn((NUMBER_SERIAL + 1), (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_EXT_COM2:
        fnSimulateSerialIn((NUMBER_SERIAL + 2), (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
    case RX_EXT_COM3:
        fnSimulateSerialIn((NUMBER_SERIAL + 3), (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
#endif
#if defined SPI_SIM_INTERFACE
    case RX_SPI0:
        fnSimulateSPIIn(0, (unsigned char *)argv[1], *(unsigned short *)argv[0]);
        break;

    case RX_SPI1:
        fnSimulateSPIIn(1, (unsigned char*)argv[1], *(unsigned short *)argv[0]);
        break;
#endif
#if defined SUPPORT_HW_FLOW                                              // {7}
    case MODEM_COM_0:
        fnSimulateModemChange(0, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;

    case MODEM_COM_1:
        fnSimulateModemChange(1, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;

    case MODEM_COM_2:
        fnSimulateModemChange(2, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;

    case MODEM_COM_3:                                                    // {44}
        fnSimulateModemChange(3, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;
    case MODEM_COM_4:                                                    // {56}
        fnSimulateModemChange(4, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;
    case MODEM_COM_5:                                                    // {56}
        fnSimulateModemChange(5, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;
    case MODEM_COM_6:                                                    // {73}
        fnSimulateModemChange(6, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;
    case MODEM_COM_7:                                                    // {73}
        fnSimulateModemChange(7, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;
    #if NUMBER_EXTERNAL_SERIAL > 0                                       //  {49}
    case MODEM_EXT_COM_0:
        fnSimulateModemChange(NUMBER_SERIAL, *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;

    case MODEM_EXT_COM_1:
        fnSimulateModemChange((NUMBER_SERIAL + 1), *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;

    case MODEM_EXT_COM_2:
        fnSimulateModemChange((NUMBER_SERIAL + 2), *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;

    case MODEM_EXT_COM_3:                                                    // {44}
        fnSimulateModemChange((NUMBER_SERIAL + 3), *(unsigned long *)argv[0], *(unsigned long *)argv[1]);
        break;
    #endif
#endif
    case RX_ETHERNET:
        if (iInitialised != 0) {
#if LOSE_ACKS > 0
            if (fnRandomPacketLoss(LOSE_ACKS)) {
                break;
            }
#endif
            argv[2] = (char *)fnSimulateEthernetIn((unsigned char*)argv[1], *(unsigned short *)argv[0], 0); // {39}
            if (argv[2] == 0) {                                          // {34}
                iOpSysActive = 0;                                        // {36}
                return 0;                                                // Ethernet frame not for us
            }
            iRun = 5;                                                    // allow the simulator to treat process frames immediately
        }
        break;

    case SIM_TEST_RX_0:
        fnSimulateSerialIn(0, (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 0
        break;
#if NUMBER_SERIAL > 1
    case SIM_TEST_RX_1:
        fnSimulateSerialIn(1, (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 1
        break;
#endif
#if NUMBER_SERIAL > 2
    case SIM_TEST_RX_2:
        fnSimulateSerialIn(2, (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 2
        break;
#endif
#if NUMBER_SERIAL > 3
    case SIM_TEST_RX_3:                                                  // {12}
        fnSimulateSerialIn(3, (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 3
        break;
#endif
#if NUMBER_SERIAL > 4
    case SIM_TEST_RX_4:                                                  // {56}
        fnSimulateSerialIn(4, (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 3
        break;
#endif
#if NUMBER_SERIAL > 5
    case SIM_TEST_RX_5:                                                  // {56}
        fnSimulateSerialIn(5, (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 3
        break;
#endif
#if NUMBER_EXTERNAL_SERIAL > 0 
    case SIM_TEST_EXT_RX_0:                                              // {49}
        fnSimulateSerialIn(NUMBER_SERIAL, (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 3
        break;

    case SIM_TEST_EXT_RX_1:
        fnSimulateSerialIn((NUMBER_SERIAL + 1), (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 3
        break;

    case SIM_TEST_EXT_RX_2:
        fnSimulateSerialIn((NUMBER_SERIAL + 2), (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 3
        break;

    case SIM_TEST_EXT_RX_3:
        fnSimulateSerialIn((NUMBER_SERIAL + 3), (unsigned char*)ucHelloWorld, (sizeof(ucHelloWorld) - 1)); // we want to simulate a message being received on the serial interface number 3
        break;
#endif
    case SIM_UART_BREAK:                                                 // {27}
        fnSimulateBreak((int)argv[0]);
        break;
    case SIM_UART_CTS:
        fnSimulateModemChange((int)argv[0], (int)argv[1], ((~(int)argv[1]) & MS_CTS_ON));
        break;
#if defined I2C_INTERFACE
    case SIM_I2C_OUT_REPEATED:
    case SIM_I2C_OUT:                                                    // {71}
        fnSimulateI2C((int)argv[0], (unsigned char *)argv[2], (unsigned short)argv[1], (argc == SIM_I2C_OUT_REPEATED));
        break;
#endif
#if defined USB_INTERFACE
    case SIM_TEST_DISCONNECT:                                            // {26}
    #if defined USB_HOST_SUPPORT
        if (iHostMode != 0) {
            fnSimulateUSB(0, 0, 0, 0, USB_RESET_CMD);                    // USB reset condition
        }
        else {
    #endif
            fnSimulateUSB(0, 0, 0, 0, USB_SLEEP_CMD);                    // USB suspend condition
    #if defined USB_HOST_SUPPORT
        }
    #endif
        memset(usUSB_length, 0x00, sizeof(usUSB_length));                // reset local USB queues
        break;

    case SIM_USB_OUT:                                                    // {26}
        {
            int iEndpoint = (int)argv[0];
            unsigned char ucPID = OUT_PID;                               // {30}
            unsigned short usLength = (unsigned short)argv[1];
            unsigned char *ptrData = (unsigned char *)argv[2];
            unsigned short usEndpointSize;
            if (ptrData == 0) {                                          // not a specific injection so check whether there is queued data
                static int fnGetQueuedUSB(int *iEndpoint, unsigned short *usLength, unsigned char **ptrData);
                if (fnGetQueuedUSB(&iEndpoint, &usLength, &ptrData) != 0) {
                    break;;
                }
            }
            if ((iEndpoint & USB_SETUP_FLAG) != 0) {                     // {30} identify that a setup frame is being simulated
                iEndpoint &= ~(USB_SETUP_FLAG);                          // remove the identifier
                ucPID = SETUP_PID;                                       // change from default OUT to SETUP PID
            }
    #if defined USB_HOST_SUPPORT && defined _KINETIS
            if (iHostMode != 0) {                                        // {72}
                usEndpointSize = fnGetEndpointInfo(0);                   // the host mode always receives on its 0 endpoint
            }
            else {
                usEndpointSize = fnGetEndpointInfo(iEndpoint);           // get the endpoint buffer length     
            }
    #else
            usEndpointSize = fnGetEndpointInfo(iEndpoint);               // get the endpoint buffer length            
    #endif
            if (usEndpointSize != 0) {                                   // if the endpoint can accept data
                static void fnQueueUSB(int iEndpoint, unsigned char *ptrData, unsigned short usLength);
                unsigned short usFrameLength;
                while (1 != 0) {                                         // {43}
                    if (usLength < usEndpointSize) {
                        usFrameLength = usLength;
                    }
                    else {
                        usFrameLength = usEndpointSize;
                    }
                    if (fnSimulateUSB(0xff, iEndpoint, ucPID, ptrData, usFrameLength) != 0) { // inject DATA packet
                        fnQueueUSB(iEndpoint, ptrData, usLength);        // put the remaining data to a queue to be injected later
                        break;                                           // not all reception was possible due to the receiver being busy
                    }                    
                    ptrData += usFrameLength;
                    fnCheckUSBOut(0, iEndpoint);                         // consume any returned data
                    if (usLength <= usFrameLength) {                     // completed
                        break;
                    }
                    usLength -= usFrameLength;
                }
            }
        }
        break;

    case SIM_TEST_ENUM:                                                  // start a test of the enumeration sequence {6}
        {
            #define ZERO_FRAME  (unsigned char *)1
    #if defined USE_USB_CDC                                              // {66}
            int i;
    #endif
            unsigned char ucGetDescriptorDevice[]            = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00}; // {60} 
            unsigned char ucGetDescriptorConfiguration[]     = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x09, 0x00}; // partial
            unsigned char ucGetDescriptorStrLang[]           = {0x80, 0x06, 0x00, 0x03, 0x00, 0x00, 0x02, 0x00};
            unsigned char ucGetDescriptorStrProd[]           = {0x80, 0x06, 0x02, 0x03, 0x09, 0x04, 0xff, 0x00};
            unsigned char ucGetDescriptorStrSN[]             = {0x80, 0x06, 0x03, 0x03, 0x09, 0x04, 0xff, 0x00};
    #if defined MICROSOFT_OS_STRING_DESCRIPTOR
            unsigned char ucGetDescriptorStrMOS[]            = {0x80, 0x06, 0xee, 0x03, 0x00, 0x00, 0x12, 0x00};
            unsigned char ucGetMSextended[]                  = {0xc0, 0x01, 0x00, 0x00, 0x04, 0x00, 0x28, 0x00}; // MOS vendor code 1
    #endif
            unsigned char ucGetDescriptorConfigurationFull[] = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0xff, 0x00}; // full
            unsigned char ucRequestVendor[]                  = {0x40, 0x02, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00}; // vendor request IN
            unsigned char ucGetDeviceQualifier[]             = {0x80, 0x06, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00}; // a USB2.0 mode device which is operating at full speed (rather than high speed), receives this request
            unsigned char ucSetAddress_1[]                   = {0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
            unsigned char ucSetFeatureWakeup[]               = {0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}; // set feature - remote wakeup
            unsigned char ucSetConfiguration[]               = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}; // set configuration 1
            unsigned char ucGetConfiguration[]               = {0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}; // get configuration
            unsigned char ucClassRequestOUT[]                = {0x21, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            unsigned char ucGetDescriptorClass[]             = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x74, 0x00};
            unsigned char ucSetInterface0[]                  = {0x01, 0x0b, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}; // set alternative interface 0 setting (interface 3)
            unsigned char ucSetInterface1[]                  = {0x01, 0x0b, 0x01, 0x00, 0x03, 0x00, 0x00, 0x00}; // set alternative interface 1 setting (interface 3)
            unsigned char ucGetInterface[]                   = {0x81, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
            unsigned char ucGetDeviceStatus[]                = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00};
            unsigned char ucGetInterfaceStatus[]             = {0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00};
            unsigned char ucGetEndpointStatus[]              = {0x83, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00};
            unsigned char ucData1[]                          = {0x00, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x08};
    #if defined USE_USB_HID_MOUSE || defined USE_USB_HID_RAW             // {61}
            unsigned char ucSetIdleMouse[]                   = {0x21, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            unsigned char ucGetReportDescriptorMouse[]       = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x84, 0x00};
    #endif
    #if defined USE_USB_MSD
            unsigned char ucClassPartitions[]                = {0xa1, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
            unsigned char ucClearFeature[]                   = {0x02, 0x01, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00};
    #endif
    #if defined USE_USB_HID_KEYBOARD
            unsigned char ucSetIdleKeyboard[]                = {0x21, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            unsigned char ucGetReportDescriptorKeyboard[]    = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x81, 0x00};
    #endif
    #if defined USE_USB_CDC                                              // {66}
            unsigned char ucSetLineCodingSetup[]             = {0x21, 0x20, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
            unsigned char ucSetLineCoding[]                  = {0x00, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x08}; // 57600 / 8 bits
            unsigned char ucGetLineCoding[]                  = {0xa1, 0x21, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00}; // request line coding
            unsigned char ucSetControlLineState[]            = {0x21, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // set control line stte
    #endif
            fnSimulateUSB(0, 0, 0, 0, USB_RESET_CMD);                    // USB reset condition
            fnSimulateUSB(0, 0, SETUP_PID, ucGetDescriptorDevice, sizeof(ucGetDescriptorDevice)); // inject SETUP packet
          //fnCheckUSBOut(0, 0);                                         // {32} get any returned data
            fnSimulateUSB(0, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator to test interruption of multiple short frames
            fnSimulateUSB(0, 0, 0, 0, USB_RESET_CMD);                    // USB reset condition
            fnSimulateUSB(0, 0, SETUP_PID, ucSetAddress_1, sizeof(ucSetAddress_1)); // inject SETUP packet
            fnCheckUSBOut(0, 0);                                         // get any returned data - this will cause the new address to be assigned
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorDevice, sizeof(ucGetDescriptorDevice)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorConfiguration, sizeof(ucGetDescriptorConfiguration)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorStrLang, sizeof(ucGetDescriptorStrLang)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorStrSN, sizeof(ucGetDescriptorStrSN)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorConfigurationFull, sizeof(ucGetDescriptorConfigurationFull)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorStrLang, sizeof(ucGetDescriptorStrLang)); // inject SETUP packet
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorStrProd, sizeof(ucGetDescriptorStrProd)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucSetFeatureWakeup, sizeof(ucSetFeatureWakeup)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data        
    #if USB_SPEC_VERSION == USB_SPEC_VERSION_2_0
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDeviceQualifier, sizeof(ucGetDeviceQualifier)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
    #endif
            fnSimulateUSB(1, 0, SETUP_PID, ucSetConfiguration, sizeof(ucSetConfiguration)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetConfiguration, sizeof(ucGetConfiguration)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // {33} inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetInterface, sizeof(ucGetInterface)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data 
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
    #if defined MICROSOFT_OS_STRING_DESCRIPTOR
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDescriptorStrMOS, sizeof(ucGetDescriptorStrMOS)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data 
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetMSextended, sizeof(ucGetMSextended)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data 
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
    #endif
    #if defined USE_USB_HID_MOUSE || defined USE_USB_HID_RAW             // {61}
            fnSimulateUSB(1, 0, SETUP_PID, ucSetIdleMouse, sizeof(ucSetIdleMouse)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetReportDescriptorMouse, sizeof(ucGetReportDescriptorMouse)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
    #endif
    #if defined USE_USB_MSD
            fnSimulateUSB(1, 0, SETUP_PID, ucClassPartitions, sizeof(ucClassPartitions)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucClearFeature, sizeof(ucClearFeature)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
    #endif
    #if  defined USE_USB_HID_KEYBOARD 
            fnSimulateUSB(1, 0, SETUP_PID, ucSetIdleKeyboard, sizeof(ucSetIdleKeyboard)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetReportDescriptorKeyboard, sizeof(ucGetReportDescriptorKeyboard)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
    #endif
    #if defined USE_USB_CDC                                              // {66}
            for (i = 0; i < USB_CDC_COUNT; i++) {                        // {69} check each USB-CDC
                unsigned char ucFrame[8];
                memcpy(ucFrame, ucSetLineCodingSetup, sizeof(ucSetLineCodingSetup));
                ucFrame[4] = (i * 2);                                    // set the CDC interface count
                fnSimulateUSB(1, 0, SETUP_PID, ucFrame, sizeof(ucSetLineCodingSetup)); // inject SETUP packet
                fnSimulateUSB(1, 0, OUT_PID,  ucSetLineCoding, sizeof(ucSetLineCoding)); // inject DATA packet
                fnCheckUSBOut(1, 0);                                         // get any returned data
                memcpy(ucFrame, ucGetLineCoding, sizeof(ucGetLineCoding));
                ucFrame[4] = (i * 2);                                    // set the CDC interface count
                fnSimulateUSB(1, 0, SETUP_PID, ucFrame, sizeof(ucGetLineCoding)); // inject SETUP packet
                fnCheckUSBOut(1, 0);                                         // get any returned data
                memcpy(ucFrame, ucSetControlLineState, sizeof(ucSetControlLineState));
                ucFrame[4] = (i * 2);                                    // set the CDC interface count
                fnSimulateUSB(1, 0, SETUP_PID, ucFrame, sizeof(ucSetControlLineState)); // inject SETUP packet
            }
    #endif
            fnSimulateUSB(1, 0, SETUP_PID, ucGetDeviceStatus, sizeof(ucGetDeviceStatus)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetInterfaceStatus, sizeof(ucGetInterfaceStatus)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data 
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
            fnSimulateUSB(1, 0, SETUP_PID, ucGetEndpointStatus, sizeof(ucGetEndpointStatus)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data 
            fnSimulateUSB(1, 0, OUT_PID,  ZERO_FRAME, 0);                // inject zero terminator
    #if defined USE_USB_AUDIO
            fnSimulateUSB(1, 0, SETUP_PID, ucSetInterface0, sizeof(ucSetInterface0)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data 
            fnSimulateUSB(1, 0, SETUP_PID, ucSetInterface1, sizeof(ucSetInterface1)); // inject SETUP packet
            fnCheckUSBOut(1, 0);                                         // get any returned data 
    #endif
        }
        break;
    #if defined USB_HOST_SUPPORT                                         // {29}
    case SIM_TEST_LOWSPEED_DEVICE:
        iHostMode = 1;
        fnSimulateUSB(0, 0, 0, 0, USB_LOWSPEED_ATTACH_CMD);              // low speed device attach
        break;
    case SIM_TEST_FULLSPEED_DEVICE:
        iHostMode = 1;
        fnSimulateUSB(0, 0, 0, 0, USB_FULLSPEED_ATTACH_CMD);             // full speed device attach
        break;
    #endif
#endif

    case  EXITING_CALL:
        fnSaveFlashToFile();                                             // in normal exit save present FLASH contents to a file for restore on next start
        fnCloseAll();                                                    // close all open output log files {25}
        iOpSysActive = 0;                                                // {36}
        return 0;

    default:
        break;
    }

    if (iRun != 0) {
        do {
            if (iWinPcapSending == 0) {
    #if defined MULTISTART
                ptrNewstart =
    #endif
                uTaskerSchedule();                                       // let the tasker run a few more times to allow internal message passing to be processed and free running tasks to run a while
                fnSimPorts();
    #if defined MULTISTART
                if (ptrNewstart != 0) {
                    iOpSysActive = 0;                                    // {36}
                    return RESTART;
                }
    #endif
            }
        } while (--iRun != 0);
    }
    if (iSimulate != 0) {                                                // system tick
        int iRtn = fnSimulateActions(argv);
        iOpSysActive = 0;                                                // {36}
        return (iRtn);
    }
    iOpSysActive = 0;                                                    // {36}
	return 0;
}

extern void fnControlBreak(unsigned char ucChannel, int iOnOff)
{
    switch (ucChannel) {
    case 0:
      if (iOnOff != 0) {
          ulActions |= SET_BREAK_COM_0;
      }
      else {
          ulActions |= CLR_BREAK_COM_0;
      }
      break;

    case 1:
      if (iOnOff != 0) {
          ulActions |= SET_BREAK_COM_1;
      }
      else {
          ulActions |= CLR_BREAK_COM_1;
      }      
      break;

    case 2:
      if (iOnOff != 0) {
          ulActions |= SET_BREAK_COM_2;
      }
      else {
          ulActions |= CLR_BREAK_COM_2;
      }
      break;
    }
}

extern void fnUpdateIPConfig(void)
{
#if defined USE_IP
    ulActions_2 |= IP_CONFIG_CHANGED;                                    // mark that we should display new IP details
#endif
}

#if defined USE_IP
    #if defined USE_DHCP_CLIENT
    extern unsigned char fnGetDHCP_State(int iNetwork);
    #endif
    #if defined USE_ZERO_CONFIG
    extern unsigned char fnGetZeroConfig_State(int iNetwork);
    #endif
extern unsigned char fnAddIPData(unsigned char *ucDataBuffer)
{
    int i;
    unsigned char *ptrLen = ucDataBuffer++;
    unsigned char ucLength;
    #if defined USE_DHCP_CLIENT
    unsigned char ucDHCP_state;
    #endif
    #if defined USE_ZERO_CONFIG                                          // {57}
    unsigned char ucZeroConfig_state;
    #endif

    for (i = 0; i < IP_NETWORK_COUNT; i++) {                             // {68}
    #if defined USE_DHCP_CLIENT
        ucDHCP_state = fnGetDHCP_State(i);
    #endif
    #if defined USE_ZERO_CONFIG                                          // {57}
        ucZeroConfig_state = fnGetZeroConfig_State(i);
    #endif
        ucDataBuffer = fnIPStr(&network[i].ucOurIP[0], ucDataBuffer);    // {63}

        *ucDataBuffer++ = ' ';
        *ucDataBuffer++ = ':';
        *ucDataBuffer++ = ' ';

        ucDataBuffer = fnMACStr(&network[i].ucOurMAC[0], ucDataBuffer);  // {63}
    #if !defined STRING_OPTIMISATION                                     // {64}
        ucDataBuffer--;                                                  // for compatibility
    #endif

        #if defined USE_DHCP_CLIENT                                      // if we are using DHCP, display the state of resolution
        switch (ucDHCP_state) {
        case DHCP_INIT:
            break;

        case DHCP_STATE_INIT_REBOOT:
        case DHCP_STATE_INIT:
        case DHCP_STATE_REBOOTING:
        case DHCP_STATE_SELECTING:
        case DHCP_STATE_REQUESTING:
            ucDataBuffer = uStrcpy(ucDataBuffer, " (Resolving)");
            break;

        case DHCP_STATE_BOUND:
            ucDataBuffer = uStrcpy(ucDataBuffer, " (Resolved)");
            break;

        case DHCP_STATE_RENEWING:
        case DHCP_STATE_REBINDING:
            ucDataBuffer = uStrcpy(ucDataBuffer, " (Rebinding)");
            break;
        }
        #endif
        #if defined USE_ZERO_CONFIG
        switch (ucZeroConfig_state) {                                    // {57}
        case ZERO_CONFIG_OFF:
            break;
        case ZERO_CONFIG_PROBING:
            ucDataBuffer = uStrcpy(ucDataBuffer, " AUTO-IP (probing)");
            break;
        case ZERO_CONFIG_ACTIVE_DEFENDED:
            ucDataBuffer = uStrcpy(ucDataBuffer, " AUTO-IP (defended)");
            break;
        case ZERO_CONFIG_ACTIVE:
            ucDataBuffer = uStrcpy(ucDataBuffer, " AUTO-IP (active)");
            break;
        }
        #endif
        #if defined STRING_OPTIMISATION                                  // {46}
        ucDataBuffer++;
        #endif
        *(ucDataBuffer - 1) = '|';                                       // {68} separater for multiple IP addresses
    }
    ucLength = (unsigned char)(ucDataBuffer - ptrLen - 1);

    *ptrLen = ucLength;

    return (ucLength);
}
#endif

// Open or reconfigure a serial interface
//
extern void fnConfigSimSCI(QUEUE_HANDLE Channel, unsigned long ulSpeed, TTYTABLE *pars)
{
    if (pars == 0) {                                                     // interpret as modem control {7}
        ulActions |= ulSpeed;
        return;
    }

#if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        switch (Channel - NUMBER_SERIAL) {
        case 0:
            ulActions_2 |= OPEN_EXT_COM_0;                               // signal we want a COM port mapped to this UART
            ulExtChannel0Speed = ulSpeed;
            ExtChannel0Config = pars->Config;
            iExtChannel0Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);  // approx. max. characters capable of transmitting in a tick period {10}
            if (iExtChannel0Speed == 0) {
                iExtChannel0Speed = 1;
            }
            break;
        case 1:
            ulActions_2 |= OPEN_EXT_COM_1;                               // signal we want a COM port mapped to this UART
            ulExtChannel1Speed = ulSpeed;
            ExtChannel1Config = pars->Config;
            iExtChannel1Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);  // approx. max. characters capable of transmitting in a tick period {10}
            if (iExtChannel1Speed == 0) {
                iExtChannel1Speed = 1;
            }
            break;
        case 2:
            ulActions_2 |= OPEN_EXT_COM_2;                               // signal we want a COM port mapped to this UART
            ulExtChannel2Speed = ulSpeed;
            ExtChannel2Config = pars->Config;
            iExtChannel2Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);  // approx. max. characters capable of transmitting in a tick period {10}
            if (iExtChannel2Speed == 0) {
                iExtChannel2Speed = 1;
            }
            break;
        case 3:
            ulActions_2 |= OPEN_EXT_COM_3;                               // signal we want a COM port mapped to this UART
            ulExtChannel3Speed = ulSpeed;
            ExtChannel3Config = pars->Config;
            iExtChannel3Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);  // approx. max. characters capable of transmitting in a tick period {10}
            if (iExtChannel3Speed == 0) {
                iExtChannel3Speed = 1;
            }
            break;
        }
        return;
    }
#endif
    switch (Channel) {
    case 0:
        ulActions_2 |= OPEN_COM_0;                                       // signal we want a COM port mapped to this UART
        ulChannel0Speed = ulSpeed;
        Channel0Config = pars->Config;                                   // {45}
        iChannel0Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);         // approx. max. characters capable of transmitting in a tick period {10}
        if (iChannel0Speed == 0) {
            iChannel0Speed = 1;
        }
        break;

    case 1:
        ulActions_2 |= OPEN_COM_1;                                       // signal we want a COM port mapped to this UART
        ulChannel1Speed = ulSpeed;
        Channel1Config = pars->Config;                                   // {45}
        iChannel1Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);         // approx. max. characters capable of transmitting in a tick period {10}
        if (iChannel1Speed == 0) {
            iChannel1Speed = 1;
        }
        break;

    case 2:
        ulActions_2 |= OPEN_COM_2;                                       // signal we want a COM port mapped to this UART
        ulChannel2Speed = ulSpeed;
        Channel2Config = pars->Config;                                   // {45}
        iChannel2Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);         // approx. max. characters capable of transmitting in a tick period {10}
        if (iChannel2Speed == 0) {
            iChannel2Speed = 1;
        }
        break;

    case 3:                                                              // {12}
        ulActions_2 |= OPEN_COM_3;                                       // signal we want a COM port mapped to this UART
        ulChannel3Speed = ulSpeed;
        Channel3Config = pars->Config;                                   // {45}
        iChannel3Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);         // approx. max. characters capable of transmitting in a tick period {10}
        if (iChannel3Speed == 0) {
            iChannel3Speed = 1;
        }
        break;

    case 4:                                                              // {56}
        ulActions_2 |= OPEN_COM_4;                                       // signal we want a COM port mapped to this UART
        ulChannel4Speed = ulSpeed;
        Channel4Config = pars->Config;
        iChannel4Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);         // approx. max. characters capable of transmitting in a tick period {10}
        if (iChannel4Speed == 0) {
            iChannel4Speed = 1;
        }
        break;

    case 5:                                                              // {56}
        ulActions_2 |= OPEN_COM_5;                                       // signal we want a COM port mapped to this UART
        ulChannel5Speed = ulSpeed;
        Channel5Config = pars->Config;
        iChannel5Speed = ((ulSpeed * TICK_RESOLUTION)/10000000);         // approx. max. characters capable of transmitting in a tick period {10}
        if (iChannel5Speed == 0) {
            iChannel5Speed = 1;
        }
        break;
    }
}

#if defined I2C_INTERFACE                                                // {9}
// Open or reconfigure an I2C interface
//
extern void fnConfigSimI2C(QUEUE_HANDLE Channel, unsigned long ulSpeed)
{
    switch (Channel) {
    case 0:
        iI2C_Channel0Speed = ulSpeed/9/(10000000/TICK_RESOLUTION);       // approx. max. I2C bytes capable of transmitting/receiving in a tick period;
        break;
    case 1:
        iI2C_Channel1Speed = ulSpeed/9/(1000000/TICK_RESOLUTION);        // approx. max. I2C bytes capable of transmitting/receiving in a tick period;;
        break;
    case 2:                                                              // {28}
        iI2C_Channel2Speed = ulSpeed/9/(1000000/TICK_RESOLUTION);        // approx. max. I2C bytes capable of transmitting/receiving in a tick period;;
        break;
    case 3:
        iI2C_Channel3Speed = ulSpeed/9/(1000000/TICK_RESOLUTION);        // approx. max. I2C bytes capable of transmitting/receiving in a tick period;;
        break;
    }
}
#endif

unsigned char *fnGetNextDoPlace(char *argv[])
{
    unsigned char *ucNextLocation = (unsigned char*)argv[0];

    while (*ucNextLocation) {
        if (DISPLAY_PORT_CHANGE == *ucNextLocation) {                    // {51}
            ucNextLocation++;
            ucNextLocation += ((*ucNextLocation * 3 * sizeof(unsigned long)) + 1);
        }
        else {
            ucNextLocation++;
            ucNextLocation += (*ucNextLocation + 1);
        }
    }
    return ucNextLocation;
}


unsigned char *fnInsertValue(unsigned char *ucDo, unsigned long ulValue, int iLen)
{
    while (iLen--) {
        *ucDo++ = (unsigned char)(ulValue >> iLen*8);
    }
    return ucDo;
}

static int fnSimulateActions(char *argv[])
{
    int iReturn = 0;
    unsigned char *ucDo;
    char *argv2[13];
    int iThroughPut[16];

    iThroughPut[THROUGHPUT_UART0] = (iChannel0Speed + 1);                // start with internal UARTs 0..5
    iThroughPut[THROUGHPUT_UART1] = (iChannel1Speed + 1);
    iThroughPut[THROUGHPUT_UART2] = (iChannel2Speed + 1);
    iThroughPut[THROUGHPUT_UART3] = (iChannel3Speed + 1);
    iThroughPut[THROUGHPUT_UART4] = (iChannel4Speed + 1);
    iThroughPut[THROUGHPUT_UART5] = (iChannel5Speed + 1);
    iThroughPut[THROUGHPUT_UART6] = (iChannel6Speed + 1);                // {73}
    iThroughPut[THROUGHPUT_UART7] = (iChannel7Speed + 1);

    iThroughPut[THROUGHPUT_I2C0] = iI2C_Channel0Speed;                   // {9} - followed by 4 I2C interfaces
    iThroughPut[THROUGHPUT_I2C1] = iI2C_Channel1Speed;
    iThroughPut[THROUGHPUT_I2C2] = iI2C_Channel2Speed;                   // {28}
    iThroughPut[THROUGHPUT_I2C3] = iI2C_Channel3Speed;

#if NUMBER_EXTERNAL_SERIAL > 0                                           // {49} - then 4 external UARTs
    iThroughPut[THROUGHPUT_EXT_UART0] = iExtChannel0Speed+1;
    iThroughPut[THROUGHPUT_EXT_UART1] = iExtChannel1Speed+1;
    iThroughPut[THROUGHPUT_EXT_UART2] = iExtChannel2Speed+1;
    iThroughPut[THROUGHPUT_EXT_UART3] = iExtChannel3Speed+1;
#endif
                                                                         // limit the number of serial tx interrupts to that which is possible in the TICK period
    argv2[THROUGHPUT_UART0] = (char *)&iThroughPut[THROUGHPUT_UART0];    // {10}
    argv2[THROUGHPUT_UART1] = (char *)&iThroughPut[THROUGHPUT_UART1];
    argv2[THROUGHPUT_UART2] = (char *)&iThroughPut[THROUGHPUT_UART2];
    argv2[THROUGHPUT_UART3] = (char *)&iThroughPut[THROUGHPUT_UART3];
    argv2[THROUGHPUT_UART4] = (char *)&iThroughPut[THROUGHPUT_UART4];
    argv2[THROUGHPUT_UART5] = (char *)&iThroughPut[THROUGHPUT_UART5];
    argv2[THROUGHPUT_UART6] = (char *)&iThroughPut[THROUGHPUT_UART6];
    argv2[THROUGHPUT_UART7] = (char *)&iThroughPut[THROUGHPUT_UART7];
    argv2[THROUGHPUT_I2C0] = (char *)&iThroughPut[THROUGHPUT_I2C0];      // {28}
    argv2[THROUGHPUT_I2C1] = (char *)&iThroughPut[THROUGHPUT_I2C1];
    argv2[THROUGHPUT_I2C2] = (char *)&iThroughPut[THROUGHPUT_I2C2];
    argv2[THROUGHPUT_I2C3] = (char *)&iThroughPut[THROUGHPUT_I2C3];
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {49}
    argv2[THROUGHPUT_EXT_UART0] = (char *)&iThroughPut[THROUGHPUT_EXT_UART0];
    argv2[THROUGHPUT_EXT_UART1] = (char *)&iThroughPut[THROUGHPUT_EXT_UART1];
    argv2[THROUGHPUT_EXT_UART2] = (char *)&iThroughPut[THROUGHPUT_EXT_UART2];
    argv2[THROUGHPUT_EXT_UART3] = (char *)&iThroughPut[THROUGHPUT_EXT_UART3];
#endif

    while ((iInts & ~iMasks) != 0) {                                     // process any interrupts which we want to simulate here
        ulActions |= fnSimInts(argv2);
    }
    iMasks = 0;

#if defined SERIAL_INTERFACE || defined SSC_INTERFACE                    // {47}
    while ((iDMA & ~iMasks) != 0) {                                      // {11} process DMA transfers we want to simulate here
        ulActions |= fnSimDMA(argv2);
    }
    iMasks = 0;
#endif

    ulActions_2 |= fnPortChanges(0);                                     // {42} unforced (due to tick)

    if (ulActions_2 != 0) {
        if ((ulActions_2 & OPEN_COM_0) != 0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM0;                                      // we inform that we want UART 0 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel0Speed) + sizeof(Channel0Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel0Speed, sizeof(ulChannel0Speed));
            ucDo = fnInsertValue(ucDo, Channel0Config, sizeof(Channel0Config));
            iReturn = 1;
        }
        if ((ulActions_2 & OPEN_COM_1) != 0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM1;                                      // we inform that we want UART 1 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel1Speed) + sizeof(Channel1Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel1Speed, sizeof(ulChannel1Speed));
            ucDo = fnInsertValue(ucDo, Channel1Config, sizeof(Channel1Config));
            iReturn = 1;
        }
        if ((ulActions_2 & OPEN_COM_2) != 0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM2;                                      // we inform that we want UART 2 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel2Speed) + sizeof(Channel2Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel2Speed, sizeof(ulChannel2Speed));
            ucDo = fnInsertValue(ucDo, Channel2Config, sizeof(Channel2Config));
            iReturn = 1;
        }
        if ((ulActions_2 & OPEN_COM_3) != 0) {                           // {12}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM3;                                      // we inform that we want UART 3 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel3Speed) + sizeof(Channel3Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel3Speed, sizeof(ulChannel3Speed));
            ucDo = fnInsertValue(ucDo, Channel3Config, sizeof(Channel3Config));
            iReturn = 1;
        }
        if ((ulActions_2 & OPEN_COM_4) != 0) {                           // {56}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM4;                                      // we inform that we want UART 4 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel4Speed) + sizeof(Channel4Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel4Speed, sizeof(ulChannel4Speed));
            ucDo = fnInsertValue(ucDo, Channel4Config, sizeof(Channel4Config));
            iReturn = 1;
        }
        if ((ulActions_2 & OPEN_COM_5) != 0) {                           // {56}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM5;                                      // we inform that we want UART 5 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel5Speed) + sizeof(Channel5Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel5Speed, sizeof(ulChannel5Speed));
            ucDo = fnInsertValue(ucDo, Channel5Config, sizeof(Channel5Config));
            iReturn = 1;
        }
        if ((ulActions_2 & OPEN_COM_6) != 0) {                           // {73}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM6;                                      // we inform that we want UART 6 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel6Speed) + sizeof(Channel6Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel6Speed, sizeof(ulChannel6Speed));
            ucDo = fnInsertValue(ucDo, Channel6Config, sizeof(Channel6Config));
            iReturn = 1;
        }
        if ((ulActions_2 & OPEN_COM_7) != 0) {                           // {73}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_COM7;                                      // we inform that we want UART 7 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulChannel7Speed) + sizeof(Channel7Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulChannel7Speed, sizeof(ulChannel7Speed));
            ucDo = fnInsertValue(ucDo, Channel7Config, sizeof(Channel7Config));
            iReturn = 1;
        }
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {49}
    #if defined SERIAL_PORT_EXT_0                                        // {54}
        if (ulActions_2 & OPEN_EXT_COM_0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_EXT_COM0;                                  // we inform that we want EXT UART 0 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulExtChannel0Speed) + sizeof(ExtChannel0Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulExtChannel0Speed, sizeof(ulExtChannel0Speed));
            ucDo = fnInsertValue(ucDo, ExtChannel0Config, sizeof(ExtChannel0Config));
            iReturn = 1;
        }
        if (ulActions_2 & SEND_EXT_COM_0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_EXT_COM0;                                  // we inform that we want to send a message over EXT UART 0 COM
            *ucDo++ = (unsigned char)(sizeof(ulExtCom0Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulExtCom0Len, sizeof(ulExtCom0Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucExtCom0Data, sizeof(unsigned char *));
            iReturn = 1;
            ulExtCom0Len = 0;
        }
    #endif
    #if defined SERIAL_PORT_EXT_1
        if (ulActions_2 & OPEN_EXT_COM_1) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_EXT_COM1;                                  // we inform that we want EXT UART 1 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulExtChannel1Speed) + sizeof(ExtChannel1Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulExtChannel1Speed, sizeof(ulExtChannel1Speed));
            ucDo = fnInsertValue(ucDo, ExtChannel1Config, sizeof(ExtChannel1Config));
            iReturn = 1;
        }
        if (ulActions_2 & SEND_EXT_COM_1) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_EXT_COM1;                                  // we inform that we want to send a message over EXT UART 0 COM
            *ucDo++ = (unsigned char)(sizeof(ulExtCom1Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulExtCom1Len, sizeof(ulExtCom1Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucExtCom1Data, sizeof(unsigned char *));
            iReturn = 1;
            ulExtCom1Len = 0;
        }
    #endif
    #if defined SERIAL_PORT_EXT_2
        if (ulActions_2 & OPEN_EXT_COM_2) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_EXT_COM2;                                  // we inform that we want EXT UART 2 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulExtChannel2Speed) + sizeof(ExtChannel2Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulExtChannel2Speed, sizeof(ulExtChannel2Speed));
            ucDo = fnInsertValue(ucDo, ExtChannel2Config, sizeof(ExtChannel2Config));
            iReturn = 1;
        }
        if (ulActions_2 & SEND_EXT_COM_2) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_EXT_COM2;                                  // we inform that we want to send a message over EXT UART 0 COM
            *ucDo++ = (unsigned char)(sizeof(ulExtCom2Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulExtCom2Len, sizeof(ulExtCom2Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucExtCom2Data, sizeof(unsigned char *));
            iReturn = 1;
            ulExtCom2Len = 0;
        }
    #endif
    #if defined SERIAL_PORT_EXT_3
        if (ulActions_2 & OPEN_EXT_COM_3) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = OPEN_PC_EXT_COM3;                                  // we inform that we want EXT UART 3 COM to be opened for us
            *ucDo++ = (unsigned char)(1 + sizeof(ulExtChannel3Speed) + sizeof(ExtChannel3Config)); // length of command and data
            ucDo = fnInsertValue(ucDo, ulExtChannel3Speed, sizeof(ulExtChannel3Speed));
            ucDo = fnInsertValue(ucDo, ExtChannel3Config, sizeof(ExtChannel3Config));
            iReturn = 1;
        }
        if (ulActions_2 & SEND_EXT_COM_3) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_EXT_COM3;                                  // we inform that we want to send a message over EXT UART 0 COM
            *ucDo++ = (unsigned char)(sizeof(ulExtCom3Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulExtCom3Len, sizeof(ulExtCom3Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucExtCom3Data, sizeof(unsigned char *));
            iReturn = 1;
            ulExtCom3Len = 0;
        }
    #endif
#endif
        if (ulActions_2 & PORT_CHANGE) {
            int iPorts = 0;
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = DISPLAY_PORT_CHANGE;                               // we inform that we may want to display a port change
            *ucDo++ = (_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT); //(_PORTS_AVAILABLE * 3 * sizeof(unsigned long));// {51}{62}
            while (iPorts < (_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT)) { // {62}
                ucDo = fnInsertValue(ucDo, fnGetPresentPortState(++iPorts), sizeof(unsigned long));
                ucDo = fnInsertValue(ucDo, fnGetPresentPortDir(iPorts), sizeof(unsigned long));
                ucDo = fnInsertValue(ucDo, fnGetPresentPortPeriph(iPorts), sizeof(unsigned long));
            }
            iReturn = 1;
        }
#if defined USE_IP
        if (ulActions_2 & IP_CONFIG_CHANGED) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = IP_CHANGE;
            ucDo += fnAddIPData(ucDo);
            iReturn = 1;
        }
#endif
        ulActions_2 = 0;
    }

    if (ulActions) {                                                     // we have some actions to be performed by windows
        if (ulActions & (ASSERT_RTS_COM_0 | NEGATE_RTS_COM_0 | ASSERT_RTS_COM_1 | NEGATE_RTS_COM_1 | ASSERT_RTS_COM_2 | NEGATE_RTS_COM_2 | ASSERT_RTS_COM_3 | NEGATE_RTS_COM_3)) { // {41} if a RTS line change
            if (ulActions & ASSERT_RTS_COM_0) {                          // {7}
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 0, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 1, 1);                        // enable
                iReturn = 1;
            }
            if (ulActions & NEGATE_RTS_COM_0) {
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 0, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 0, 1);                        // disable
                iReturn = 1;
            }
            if (ulActions & ASSERT_RTS_COM_1) {                          // {41}
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 1, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 1, 1);                        // enable
                iReturn = 1;
            }
            if (ulActions & NEGATE_RTS_COM_1) {
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 1, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 0, 1);                        // disable
                iReturn = 1;
            }
            if (ulActions & ASSERT_RTS_COM_2) {                          // {41}
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 2, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 1, 1);                        // enable
                iReturn = 1;
            }
            if (ulActions & NEGATE_RTS_COM_2) {
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 2, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 0, 1);                        // disable
                iReturn = 1;
            }
            if (ulActions & ASSERT_RTS_COM_3) {                          // {41}
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 3, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 1, 1);                        // enable
                iReturn = 1;
            }
            if (ulActions & NEGATE_RTS_COM_3) {
                ucDo = fnGetNextDoPlace(argv);
                *ucDo++ = MODEM_SIGNAL_CHANGE;                           // we inform that we want the RTS line to be enabled
                *ucDo++ = (unsigned char)(3);                            // length of command and data
                ucDo = fnInsertValue(ucDo, 3, 1);                        // UART 0
                ucDo = fnInsertValue(ucDo, 0, 1);                        // disable
                iReturn = 1;
            }
        }
        if ((ulActions & SEND_COM_0) != 0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM0;                                      // we inform that we want to send a message over UART 0 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom0Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom0Len, sizeof(ulCom0Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom0Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom0Len = 0;
        }
        if ((ulActions & SEND_COM_1) != 0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM1;                                      // we inform that we want to send a message over UART 1 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom1Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom1Len, sizeof(ulCom1Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom1Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom1Len = 0;
        }
        if (ulActions & SEND_COM_2) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM2;                                      // we inform that we want to send a message over UART 2 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom2Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom2Len, sizeof(ulCom2Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom2Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom2Len = 0;
        }
        if ((ulActions & SEND_COM_3) != 0) {                             // {12}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM3;                                      // we inform that we want to send a message over UART 2 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom3Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom3Len, sizeof(ulCom3Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom3Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom3Len = 0;
        }
        if ((ulActions & SEND_COM_4) != 0) {                             // {56}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM4;                                      // we inform that we want to send a message over UART 2 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom4Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom4Len, sizeof(ulCom4Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom4Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom4Len = 0;
        }
        if ((ulActions & SEND_COM_5) != 0) {                             // {56}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM5;                                      // we inform that we want to send a message over UART 2 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom5Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom5Len, sizeof(ulCom5Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom5Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom5Len = 0;
        }
        if ((ulActions & SEND_COM_6) != 0) {                             // {73}
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM6;                                      // we inform that we want to send a message over UART 2 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom6Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom6Len, sizeof(ulCom6Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom6Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom6Len = 0;
        }
        if ((ulActions & SEND_COM_7) != 0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SEND_PC_COM7;                                      // we inform that we want to send a message over UART 2 COM
            *ucDo++ = (unsigned char)(sizeof(ulCom7Len) + sizeof(unsigned char *) + 1);
            ucDo = fnInsertValue(ucDo, ulCom7Len, sizeof(ulCom7Len));
            ucDo = fnInsertValue(ucDo, (unsigned long)&ucCom7Data, sizeof(unsigned char *));
            iReturn = 1;
            ulCom7Len = 0;
        }
        if (ulActions & SET_BREAK_COM_0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SET_COM_BREAK_0;
            *ucDo = 1;
            iReturn = 1;
        }
        if (ulActions & CLR_BREAK_COM_0) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = CLR_COM_BREAK_0;
            *ucDo = 1;
            iReturn = 1;
        }
        if (ulActions & SET_BREAK_COM_1) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SET_COM_BREAK_1;
            *ucDo = 1;
            iReturn = 1;
        }
        if (ulActions & CLR_BREAK_COM_1) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = CLR_COM_BREAK_1;
            *ucDo = 1;
            iReturn = 1;
        }
        if (ulActions & SET_BREAK_COM_2) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SET_COM_BREAK_2;
            *ucDo = 1;
            iReturn = 1;
        }
        if (ulActions & CLR_BREAK_COM_2) {
            ucDo = fnGetNextDoPlace(argv);
            *ucDo++ = SET_COM_BREAK_2;
            *ucDo = 1;
            iReturn = 1;
        }
        ulActions = 0;
    }
    return iReturn; 
}


static int iUART_File0 = 0;
extern void fnLogTx0(unsigned char ucTxByte)
{
#if defined LOG_UART0
	if (iUART_File0 == 0) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File0 = _open("UART0.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE); 
    #else
	    _sopen_s(&iUART_File0, "UART0.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE); 
    #endif
	}
#endif

    if (ulCom0Len >= UART_BUFFER_LENGTH) {
        return;
    }
    ucCom0Data[ulCom0Len++] = ucTxByte;
#if defined LOG_UART0
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File0, buf, 6);
    }
    else {
        _write(iUART_File0, &ucTxByte, 1);
    }
#endif
}

static int iUART_File1 = 0;
extern void fnLogTx1(unsigned char ucTxByte)
{
#if defined LOG_UART1
	if (!iUART_File1) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File1 = _open("UART1.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iUART_File1, "UART1.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE); 
    #endif
	}
#endif

    if (ulCom1Len >= UART_BUFFER_LENGTH) {
        return;
    }

    ucCom1Data[ulCom1Len++] = ucTxByte;

#if defined LOG_UART1
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File1, buf, 6);
    }
    else {
        _write(iUART_File1, &ucTxByte, 1);
    }
#endif
}

static int iUART_File2 = 0;
extern void fnLogTx2(unsigned char ucTxByte)
{
#if defined LOG_UART2
	if (!iUART_File2) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File2 = _open("UART2.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iUART_File2, "UART2.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
	}
#endif

    if (ulCom2Len >= UART_BUFFER_LENGTH) {
        return;
    }

    ucCom2Data[ulCom2Len++] = ucTxByte;

#if defined LOG_UART2
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File2, buf, 6);
    }
    else {
        _write(iUART_File2, &ucTxByte, 1);
    }
#endif
}

static int iUART_File3 = 0;
extern void fnLogTx3(unsigned char ucTxByte)                             // {12}
{
#if defined LOG_UART3
	if (!iUART_File3) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File3 = _open("UART3.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iUART_File3, "UART3.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
	}
#endif

    if (ulCom3Len >= UART_BUFFER_LENGTH) {
        return;
    }

    ucCom3Data[ulCom3Len++] = ucTxByte;

#if defined LOG_UART3
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File3, buf, 6);
    }
    else {
        _write(iUART_File3, &ucTxByte, 1);
    }
#endif
}

static int iUART_File4 = 0;
extern void fnLogTx4(unsigned char ucTxByte)                             // {56}
{
#if defined LOG_UART4
	if (!iUART_File4) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File4 = _open("UART4.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iUART_File4, "UART4.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
	}
#endif

    if (ulCom4Len >= UART_BUFFER_LENGTH) {
        return;
    }

    ucCom4Data[ulCom4Len++] = ucTxByte;

#if defined LOG_UART4
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) {
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File4, buf, 6);
    }
    else {
        _write(iUART_File4, &ucTxByte, 1);
    }
#endif
}

static int iUART_File5 = 0;
extern void fnLogTx5(unsigned char ucTxByte)                             // {56}
{
#if defined LOG_UART5
	if (!iUART_File5) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File5 = _open("UART5.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iUART_File5, "UART5.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
	}
#endif

    if (ulCom5Len >= UART_BUFFER_LENGTH) {
        return;
    }

    ucCom5Data[ulCom5Len++] = ucTxByte;

#if defined LOG_UART5
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) {
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File5, buf, 6);
    }
    else {
        _write(iUART_File5, &ucTxByte, 1);
    }
#endif
}

static int iUART_File6 = 0;
extern void fnLogTx6(unsigned char ucTxByte)                             // {73}
{
#if defined LOG_UART6
	if (iUART_File6 == 0) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File6 = _open("UART6.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iUART_File6, "UART6.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
	}
#endif

    if (ulCom6Len >= UART_BUFFER_LENGTH) {
        return;
    }

    ucCom6Data[ulCom6Len++] = ucTxByte;

#if defined LOG_UART6
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) {
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File6, buf, 6);
    }
    else {
        _write(iUART_File6, &ucTxByte, 1);
    }
#endif
}

static int iUART_File7 = 0;
extern void fnLogTx7(unsigned char ucTxByte)
{
#if defined LOG_UART7
	if (iUART_File7 == 0) {
    #if _VC80_UPGRADE<0x0600
	    iUART_File7 = _open("UART7.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iUART_File7, "UART7.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
	}
#endif

    if (ulCom7Len >= UART_BUFFER_LENGTH) {
        return;
    }

    ucCom7Data[ulCom7Len++] = ucTxByte;

#if defined LOG_UART7
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) {
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iUART_File7, buf, 6);
    }
    else {
        _write(iUART_File7, &ucTxByte, 1);
    }
#endif
}

#if NUMBER_EXTERNAL_SERIAL > 0                                           // {49}
static int fnCollectTxExtSCI(int iChannel, unsigned char *ucData);

static int iExt_UART_File0 = 0;
extern int fnLogExtTx0(void)
{
    unsigned char ucTxByte;
    int iInterrupt = fnCollectTxExtSCI(0, &ucTxByte);
#if defined LOG_EXT_UART0
	if (!iExt_UART_File0) {
    #if _VC80_UPGRADE<0x0600
	    iExt_UART_File0 = _open("EXT_UART0.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE); 
    #else
	    _sopen_s(&iExt_UART_File0, "EXT_UART0.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE); 
    #endif
	}
#endif

    if (ulExtCom0Len >= UART_BUFFER_LENGTH) {
        return iInterrupt;
    }
    ucExtCom0Data[ulExtCom0Len++] = ucTxByte;
#if defined LOG_EXT_UART0
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iExt_UART_File0, buf, 6);
    }
    else {
        _write(iExt_UART_File0, &ucTxByte, 1);
    }
#endif
    return iInterrupt;
}

static int iExt_UART_File1 = 0;
extern int fnLogExtTx1(void)
{
    unsigned char ucTxByte;
    int iInterrupt = fnCollectTxExtSCI(1, &ucTxByte);
#if defined LOG_EXT_UART1
	if (iExt_UART_File1 == 0) {
    #if _VC80_UPGRADE < 0x0600
	    iExt_UART_File1 = _open("EXT_UART1.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE); 
    #else
	    _sopen_s(&iExt_UART_File1, "EXT_UART1.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE); 
    #endif
	}
#endif

    if (ulExtCom1Len >= UART_BUFFER_LENGTH) {
        return iInterrupt;
    }
    ucExtCom1Data[ulExtCom1Len++] = ucTxByte;
#if defined LOG_EXT_UART1
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iExt_UART_File1, buf, 6);
    }
    else {
        _write(iExt_UART_File1, &ucTxByte, 1);
    }
#endif
    return iInterrupt;
}

static int iExt_UART_File2 = 0;
extern int fnLogExtTx2(void)
{
    unsigned char ucTxByte;
    int iInterrupt = fnCollectTxExtSCI(2, &ucTxByte);
#if defined LOG_EXT_UART2
	if (iExt_UART_File2 == 0) {
    #if _VC80_UPGRADE < 0x0600
	    iExt_UART_File2 = _open("EXT_UART2.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE); 
    #else
	    _sopen_s(&iExt_UART_File2, "EXT_UART2.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE); 
    #endif
	}
#endif

    if (ulExtCom2Len >= UART_BUFFER_LENGTH) {
        return iInterrupt;
    }
    ucExtCom2Data[ulExtCom2Len++] = ucTxByte;
#if defined LOG_EXT_UART2
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iExt_UART_File2, buf, 6);
    }
    else {
        _write(iExt_UART_File2, &ucTxByte, 1);
    }
#endif
    return iInterrupt;
}

static int iExt_UART_File3 = 0;
extern int fnLogExtTx3(void)
{
    unsigned char ucTxByte;
    int iInterrupt = fnCollectTxExtSCI(3, &ucTxByte);
#if defined LOG_EXT_UART3
	if (iExt_UART_File3 == 0) {
    #if _VC80_UPGRADE < 0x0600
	    iExt_UART_File3 = _open("EXT_UART3.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE); 
    #else
	    _sopen_s(&iExt_UART_File3, "EXT_UART3.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE); 
    #endif
	}
#endif

    if (ulExtCom3Len >= UART_BUFFER_LENGTH) {
        return iInterrupt;
    }
    ucExtCom3Data[ulExtCom3Len++] = ucTxByte;
#if defined LOG_EXT_UART3
    if ((((ucTxByte < 0x20) && (ucTxByte != 0x0d) && (ucTxByte != 0x0a)) || (ucTxByte > 0x7f))) { // {50}
        signed char buf[] = "[0xXX]";
        fnBufferHex(ucTxByte, (CODE_CAPITALS | NO_TERMINATOR| 1), &buf[3]);
        _write(iExt_UART_File3, buf, 6);
    }
    else {
        _write(iExt_UART_File3, &ucTxByte, 1);
    }
#endif
    return iInterrupt;
}
#endif

static int iSSC_File0 = 0;
extern void fnLogSSC0(unsigned long ulTxByte, unsigned char ucWordWidth) // {47}
{
#if defined LOG_SSC0
    CHAR buf[] = "[0xXX]]]]]]]";
	if (iSSC_File0 == 0) {
    #if _VC80_UPGRADE < 0x0600
	    iSSC_File0 = _open("SSC0.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	    _sopen_s(&iSSC_File0, "SSC0.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
	}
    if (ucWordWidth == 3) {
        fnBufferHex(ulTxByte, (CODE_CAPITALS | NO_TERMINATOR | 4), &buf[1]);
        buf[1] = '0';
        buf[2] = 'x';
    }
    else if (ucWordWidth == 0) {
        buf[0] = '\n';
        _write(iSSC_File0, buf, 1);
        return;
    }
    else {
        fnBufferHex(ulTxByte, (unsigned char)(CODE_CAPITALS | NO_TERMINATOR | ucWordWidth), &buf[3]);
    }
    _write(iSSC_File0, buf, ((ucWordWidth * 2) + 4));
#endif
}

#if defined USB_INTERFACE
// Write data to a file if enabled
//
extern void fnLogUSB(int iEndpoint, unsigned char ucToken, unsigned short usLength, unsigned char *ptrUSBData, int iDataToggle) // {23}{67}
{
    #if defined LOG_USB_TX
    if (ptrUSBData == (unsigned char *)0xffffffff) {                     // indicates reset detected
        if (usLength == 0) {
            if (iUSB_Log[iEndpoint] != 0) {                              // if a log file is open for the endpoint
                _write(iUSB_Log[iEndpoint], cUSB_reset, (sizeof(cUSB_reset) - 1));
            }
        }
        else {                                                           // stall assumed
            if (iUSB_Log[iEndpoint] != 0) {                              // if a log file is open for the endpoint
                _write(iUSB_Log[iEndpoint], cUSB_stall, (sizeof(cUSB_stall) - 1));
            }
        }
    }
    else {
        CHAR cHexValue[] = "0x00 ";
        if (iUSB_Log[iEndpoint] == 0) {                                  // if no file open for this endpoint
            CHAR cFileName[] = "USB_Endpoint_00.txt";
            if (iEndpoint > 9) {                                         // endpoints from 0..19 are handled
                cFileName[13] = '1';
                cFileName[14] = (iEndpoint - 10) + '0';
            }
            else {
                cFileName[14] = iEndpoint + '0';
            }
    #if _VC80_UPGRADE<0x0600
	        iUSB_Log[iEndpoint] = _open(cFileName, (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
    #else
	        _sopen_s(&iUSB_Log[iEndpoint], cFileName, (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
    #endif
        }
  //#if defined USB_HOST_SUPPORT                                         // {67}
        if (ucToken == SETUP_PID) {
            _write(iUSB_Log[iEndpoint], "SETUP ", 8);
        }
        else {
            _write(iUSB_Log[iEndpoint], "      ", 8);
        }
  //#endif
        if (iDataToggle != 0) {
            _write(iUSB_Log[iEndpoint], "[DATA1] ", 8);
        }
        else {
            _write(iUSB_Log[iEndpoint], "[DATA0] ", 8);
        }
        if (usLength == 0) {
            _write(iUSB_Log[iEndpoint], "ZERO-DATA", 9);
        }
        while (usLength--) {
            unsigned char ucTemp1 = (*ptrUSBData >> 4);
            unsigned char ucTemp2 = (*ptrUSBData & 0x0f);
            if (ucTemp1 <= 9) {
                cHexValue[2] = ucTemp1 + '0';
            }
            else {
                cHexValue[2] = (ucTemp1 - 10) + 'A';
            }
            if (ucTemp2 <= 9) {
                cHexValue[3] = ucTemp2 + '0';
            }
            else {
                cHexValue[3] = (ucTemp2 - 10) + 'A';
            }
            _write(iUSB_Log[iEndpoint], cHexValue, 5);
            ptrUSBData++;
        }
        _write(iUSB_Log[iEndpoint], "\r\n", 2);
    }
    #endif
}

// Queue USB data for later injection
//  
static void fnQueueUSB(int iEndpoint, unsigned char *ptrData, unsigned short usLength)
{
    if (usLength > MAX_USB_LENGTH) {
        usLength = MAX_USB_LENGTH;
    }
    memcpy(ucUSB_input[iEndpoint], ptrData, usLength);
    usUSB_length[iEndpoint] = usLength;
}

// Check for queued USB data
//
static int fnGetQueuedUSB(int *iEndpoint, unsigned short *usLength, unsigned char **ptrData)
{
    int iEndpt;
    for (iEndpt = 0; iEndpt < NUMBER_OF_USB_ENDPOINTS; iEndpt++) {
        if (usUSB_length[iEndpt] != 0) {
            *iEndpoint = iEndpt;
            *usLength = usUSB_length[iEndpt];
            *ptrData = ucUSB_input[iEndpt];
            usUSB_length[iEndpt] = 0;
            return 0;                                                    // this queued data is to be injected
        }
    }
    return 1;                                                            // nothing waiting
}

#endif

#if defined ETH_INTERFACE                                                // {13}
extern void fnWinPcapSendPkt(int len, unsigned char * ptr);

extern void fnSimulateEthTx(int iLen, unsigned char *ptr)
{
    #if defined LOG_ETHERNET
	signed char cVal[5];
    #endif
	fnWinPcapSendPkt(iLen, ptr);                                         // send the Ethernet frame
    #if defined LOG_ETHERNET
	cVal[0] = ' ';
	cVal[1] = '0';
	cVal[2] = 'x';

	if (iEthTxFile == 0) {
        #if _VC80_UPGRADE<0x0600
	    iEthTxFile = _open("Eth_tx.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _S_IWRITE);
        #else
	    _sopen_s(&iEthTxFile, "Eth_tx.txt", (_O_TRUNC  | _O_CREAT | _O_WRONLY), _SH_DENYWR, _S_IWRITE);
        #endif
	}
    _write(iEthTxFile, cText1, (strlen(cText1)-1));
	while (iLen--) {
        if ((cVal[3] = (signed char)((*ptr>>4) + '0')) > '9') cVal[3] += ('A' - '9' - 1);
        if ((cVal[4] = (signed char)(((*ptr++) & 0x0f) + '0')) > '9') cVal[4] += ('A' - '9' - 1);
        _write(iEthTxFile, cVal, 5);
	}
    _write(iEthTxFile, "\n", 1);
    #endif
}
#endif


static void fnCloseAll(void)
{
#if defined USB_INTERFACE
    int iUSB_endpoints;
    for (iUSB_endpoints = 0; iUSB_endpoints < NUMBER_OF_USB_ENDPOINTS; iUSB_endpoints++) {
        if (iUSB_Log[iUSB_endpoints] > 0) {                              // {38}
            _close(iUSB_Log[iUSB_endpoints]);
        }
    }
#endif
	if (iUART_File0) {
	    _close(iUART_File0);
	}
	if (iUART_File1) {
	    _close(iUART_File1);
	}
	if (iUART_File2) {
	    _close(iUART_File2);
	}
	if (iUART_File3) {
	    _close(iUART_File3);
	}
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {49}
	if (iExt_UART_File0) {
	    _close(iExt_UART_File0);
	}
	if (iExt_UART_File1) {
	    _close(iExt_UART_File1);
	}
	if (iExt_UART_File2) {
	    _close(iExt_UART_File2);
	}
	if (iExt_UART_File3) {
	    _close(iExt_UART_File3);
	}
#endif
	if (iEthTxFile) {
	    _close(iEthTxFile);
	}
    if (iSSC_File0) {                                                    // {47}
	    _close(iSSC_File0);        
    }
#if defined SDCARD_SUPPORT
    fnSaveSDcard();
#endif
#if defined CAN_INTERFACE && defined SIM_KOMODO                          // {59}
    fnSimCAN(0, 0, CAN_SIM_TERMINATE);
#endif
}

#if LOSE_ACKS > 0
static int fnRandomPacketLoss(int iLossRate)
{
    iLossRate *= (RAND_MAX/100);                                         // generate a pseudo random number and see whether we should discard the received packet

    if (iLossRate > rand()) {
        return 1;
    }
    return 0;
}
#endif

#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // {8}

#define EEPROM_SIZE FILE_SYSTEM_SIZE
static unsigned char ucM95XXX[64*1024];                                  // 64k to support largest SPI EEPROM

extern unsigned char *fnGetEEPROM(unsigned short usOffset)
{
    return (&ucM95XXX[usOffset]);
}

extern void fnInitSPI_Flash(void)
{
    memset(ucM95XXX, 0xff, sizeof(ucM95XXX));                            // start with erased SPI flash
}

static int iEEPROM_not_safe = 0;

extern void fnEEPROM_safe(void)
{
    iEEPROM_not_safe = 0;                                                // user has just waited for a write to complete
}

extern unsigned long fnGetEEPROMSize(void)                               // {1}
{
    return (sizeof(ucM95XXX));
}

extern unsigned char fnSimM95xxx(int iSimType, unsigned char ucTxByte, unsigned long ulSSEL)
{
    static char ucChipCommand = 0;
    static int iState = 0;
    static unsigned short usAddress = 0;

    if (ulSSEL) {
        ucChipCommand = 0;
        iState = 0;
        return 0xff;                                                     // chip not selected, return idle
    }

    if (iSimType == M95XXX_WRITE) {
        if (ucChipCommand == 0) {
            ucChipCommand = ucTxByte;                                    // we interpret a command
        }
        else if ((ucChipCommand == 0x03)|| (ucChipCommand == 0x02)) {    // read from array or write to array
            if (iState == 0) {
                iState = 1;
                usAddress = (ucTxByte<<8);
            }
            else if (iState == 1) {
                iState = 2;
                usAddress |= (ucTxByte);                                 // complete address is available
            }
            else {
                ucM95XXX[usAddress++] = ucTxByte;                        // we are writing
                if (usAddress >= EEPROM_SIZE) {
                    usAddress = 0;                                       // pointer overflow
                }
                iEEPROM_not_safe = 1;                                    // monitor that that the user waits for write to complete before reading data...
            }
        }
    }
    else if (M95XXX_READ == iSimType) {                                  // read
        if (ucChipCommand == 0x03) {                                     // read from array
            if (iState == 2) {
                unsigned char ucRetByte = ucM95XXX[usAddress++];
                if (usAddress >= EEPROM_SIZE) {
                    usAddress = 0;                                       // pointer overflow - wrap around
                }
                if (iEEPROM_not_safe) {
                    ucRetByte = 0xad;                                    // return a strange value
                }
                return (ucRetByte);
            }
        }
        else if (ucChipCommand == 0x05) {                                // read from status register
            return (0);
        }
    }
    return 0xff;
}
#endif

#if defined EXT_FLASH_FILE_SYSTEM                                        // {58}
static unsigned char ucParallelFlash[SIZE_OF_EXTERNAL_FLASH];            // all external parallel FLASH devices in one buffer

extern void fnInitExtFlash(void)
{
    memset(ucParallelFlash, 0xff, sizeof(ucParallelFlash));
}

extern unsigned char *fnGetExtFlashStart(void)
{
    return ucParallelFlash;
}

extern unsigned long fnGetExtFlashSize(void)
{
    return SIZE_OF_EXTERNAL_FLASH;
}
#endif



#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {2}{8}

#ifndef SPI_FLASH_DEVICE_COUNT                                           // {4}
    #define SPI_FLASH_DEVICE_COUNT         1
#endif

    #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25 && !defined SPI_FLASH_W25Q && !defined SPI_FLASH_S25FL1_K // {14}

static unsigned char ucAT45DBXXX[SPI_DATA_FLASH_SIZE];                   // all SPI FLASH devices in one buffer

static unsigned char ucAT45BDXXX_buffer1[SPI_FLASH_DEVICE_COUNT][SPI_FLASH_PAGE_LENGTH];


extern void fnInitSPI_DataFlash(void)
{
    memset(ucAT45DBXXX, 0xff, sizeof(ucAT45DBXXX));                      // {15}
}

extern unsigned char *fnGetDataFlashStart(void)
{
    return ucAT45DBXXX;
}

extern unsigned long fnGetDataFlashSize(void)
{
    return SPI_DATA_FLASH_SIZE;
}


extern unsigned char fnSimAT45DBXXX(int iSimType, unsigned char ucTxByte)// {16}{48}
{
    #if SPI_FLASH_PAGES == 512
        #define FLASH_SIZE 0x0c                                          // 1MBit
        #define FLASH_ID   0x22
    #elif SPI_FLASH_PAGES == 1024
        #define FLASH_SIZE 0x14                                          // 2MBit
        #define FLASH_ID   0x23
    #elif SPI_FLASH_PAGES == (2 * 1024)
        #define FLASH_SIZE 0x1c                                          // 4MBit
        #define FLASH_ID   0x24
    #elif SPI_FLASH_PAGES == (4 * 1024)
        #if SPI_FLASH_PAGE_LENGTH >= 512                                 // {52}
            #define FLASH_SIZE 0x2c                                      // 16MBit
            #define FLASH_ID   0x26
        #else
            #define FLASH_SIZE 0x24                                      // 8MBit
            #define FLASH_ID   0x25
        #endif
    #elif SPI_FLASH_PAGES == (8 * 1024)
        #if SPI_FLASH_PAGE_LENGTH >= 1024                                // {52}
            #define FLASH_SIZE 0x3c                                      // 64MBit
            #define FLASH_ID   0x28
        #else
            #define FLASH_SIZE 0x34                                      // 32MBit
            #define FLASH_ID   0x27
        #endif
    #endif
    static unsigned char ucChipCommand[SPI_FLASH_DEVICE_COUNT] = {0};
    static int iState[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned short usPage[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned short usOffset[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned long  ulMainAddress[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned short usAddress[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned char  ucPageSize = 0;                                // {21}
    int iSel = 0;
#if defined SPI_FLASH_MULTIPLE_CHIPS
    int iCntCS = 0;
    unsigned long ulDeviceOffset = 0;
    if (SPI_CS0_PORT & CS0_LINE) {                                       // {16}
        ucChipCommand[0] = 0;
        iState[0] = 0;
    }
    else {
        iCntCS++;
    }
    if (SPI_CS1_PORT & CS1_LINE) {
        ucChipCommand[1] = 0;
        iState[1] = 0;
    }
    else {
        iCntCS++;
        iSel = 1;
        ulDeviceOffset = SPI_DATA_FLASH_0_SIZE;
    }
    #if defined QSPI_CS2_LINE || defined CS2_LINE
        #if defined QSPI_CS2_LINE
    if (SPI_CS2_PORT & QSPI_CS2_LINE) 
        #else
    if (SPI_CS2_PORT & CS2_LINE) 
        #endif
    {
        ucChipCommand[2] = 0;
        iState[2] = 0;
    }
    else {
        iCntCS++;
        iSel = 2;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    }
    #endif
    #if defined QSPI_CS3_LINE || defined CS3_LINE
        #if defined QSPI_CS3_LINE
    if (SPI_CS3_PORT & QSPI_CS3_LINE) 
        #else
    if (SPI_CS3_PORT & CS3_LINE) 
        #endif
    {
        ucChipCommand[3] = 0;
        iState[3] = 0;
    }
    else {
        iCntCS++;
        iSel = 3;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE);
    }
    #endif
    if (iCntCS > 1) {
        *(unsigned char *)(0) = 0;                                       // 2 CS selected at same time - serious error
    }
    else if (iCntCS == 0) {
        return 0xff;                                                     // chip not selected, return idle
    }
#else
    #define ulDeviceOffset 0
    if (SPI_CS0_PORT & CS0_LINE) {                                       // {16}
        ucChipCommand[0] = 0;
        iState[0] = 0;
        return 0xff;                                                     // chip not selected, return idle
    }
#endif

    if (iSimType == AT45DBXXX_WRITE) {
        if (ucChipCommand[iSel] == 0) {
            ucChipCommand[iSel] = ucTxByte;                              // we interpret a command
        }
        else {                                                           // in command
            switch (ucChipCommand[iSel]) {
#if defined SPI_FLASH_AT45XXXXD_TYPE                                     // {21} only interpret if D-type
            case 0x3d:                                                   // possible sequence to set power of 2 mode
                switch (iState[iSel]++) {
                case 0:
                    if (ucTxByte != 0x2a) {
                        iState[iSel] = 100;                              // invalid
                    }
                    break;
                case 1:
                    if (ucTxByte != 0x80) {
                        iState[iSel] = 100;                              // invalid
                    }
                    break;
                case 2:
                    if (ucTxByte == 0xa6) {
                        ucPageSize = 0x01;                               // mark that page is set to power of 2 mode
                    }
                    else {
                        iState[iSel] = 100;                              // invalid
                    }
                    break;
                default:
                    iState[iSel] = 100;                                 // ignore invalid sequence
                    break;
                }
                break;
#endif
            case 0x9f:                                                   // read manufacturer's ID
                break;
            case 0x53:                                                   // transfer main memory to buffer 1
            case 0x88:                                                   // program page to main memory from buffer 1
            case 0x83:                                                   // erase and program
                if (iState[iSel] == 0) {
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024                                    // {53}
                    usPage[iSel] = (ucTxByte);                           // collect page number
                    usPage[iSel] <<= 6;
                    usPage[iSel] &= 0x3fc0;
    #else
                    usPage[iSel] = (ucTxByte);                           // collect page number
                    usPage[iSel] <<= 5;
                    usPage[iSel] &= 0x1fe0;
    #endif
#elif SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512                                     // {53}
                    usPage[iSel] = (ucTxByte);                           // collect page number
                    usPage[iSel] <<= 7;
                    usPage[iSel] &= 0x7f80;
    #else
                    usPage[iSel] = (ucTxByte);                           // collect page number
                    usPage[iSel] <<= 6;
                    usPage[iSel] &= 0x3fc0;
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256                                     // {53}
                    usPage[iSel] = (ucTxByte & 0x1f);                    // {37} collect page number
                    usPage[iSel] <<= 8;
                    usPage[iSel] &= 0xff00;
    #else
                    usPage[iSel] = (ucTxByte & 0x1f);                    // {37} collect page number
                    usPage[iSel] <<= 7;
                    usPage[iSel] &= 0x7f80;
    #endif
#endif
                }
                else if (iState[iSel] == 1) {
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024
                    usPage[iSel] |= (ucTxByte >> 2);                     // collect page
    #else
                    usPage[iSel] |= (ucTxByte >> 3);                     // collect page
    #endif
#elif SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512
                    usPage[iSel] |= (ucTxByte >> 1);                     // collect page
    #else
                    usPage[iSel] |= (ucTxByte >> 2);                     // collect page
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256
                    usPage[iSel] |= ucTxByte;                            // collect page
    #else
                    usPage[iSel] |= (ucTxByte >> 1);                     // collect page
    #endif
#endif
                }
                else {
                    iState[iSel] = 0;                                    // last is don't care but causes transfer to start
                    switch (ucChipCommand[iSel]) {
                    case 0x53:
                        memcpy(ucAT45BDXXX_buffer1[iSel], &ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset], SPI_FLASH_PAGE_LENGTH); // {15}
                        break;
                    case 0x83:
                        memset(&ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset], 0xff, SPI_FLASH_PAGE_LENGTH); // first delete {15}
                        // fall through
                    case 0x88:
                        {
                            int i = SPI_FLASH_PAGE_LENGTH;
                            unsigned char *ptrFlashContent = &ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset];
                            unsigned char *ptrBufferSource = ucAT45BDXXX_buffer1[iSel];
                            while (i--) {
                                *ptrFlashContent++ &= *ptrBufferSource++;// {18} only program '1' to '0'
                            }

                        }
                        //memcpy(&ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset], ucAT45BDXXX_buffer1[iSel], SPI_FLASH_PAGE_LENGTH); // {15}
                        break;
                    }
                    break;
                }
                iState[iSel]++;
                break;

            case 0x58:                                                   // {31} auto-page rewrite
            case 0xe8:
            case 0x03:                                                   // read array
                if (iState[iSel] == 0) {
                    usPage[iSel] = ucTxByte;                             // collect page
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024
                    usPage[iSel] <<= 6;
    #else
                    usPage[iSel] <<= 5;
    #endif
#elif  SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512
                    usPage[iSel] <<= 7;
    #else
                    usPage[iSel] <<= 6;
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256
                    usPage[iSel] <<= 8;
    #else
                    usPage[iSel] <<= 7;
    #endif
#endif
                }
                else if (iState[iSel] == 1) {
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024
                    usPage[iSel] |= (ucTxByte >> 2);                     // collect page and offset
                    usOffset[iSel] = (ucTxByte & 0x03);
                    usOffset[iSel] <<= 8;
    #else
                    usPage[iSel] |= (ucTxByte >> 3);                     // collect page and offset
                    usOffset[iSel] = (ucTxByte & 0x07);
                    usOffset[iSel] <<= 8;
    #endif
#elif  SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512
                    usPage[iSel] |= (ucTxByte >> 1);                     // collect page and offset
                    usOffset[iSel] = (ucTxByte & 0x01);
                    usOffset[iSel] <<= 8;
    #else
                    usPage[iSel] |= (ucTxByte >> 2);                     // collect page and offset
                    usOffset[iSel] = (ucTxByte & 0x03);
                    usOffset[iSel] <<= 8;
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256
                    usPage[iSel] |= ucTxByte;                            // collect page and offset
                    usOffset[iSel] = 0;
    #else
                    usPage[iSel] |= (ucTxByte >> 1);                     // collect page and offset
                    usOffset[iSel] = (ucTxByte & 0x01);
                    usOffset[iSel] <<= 8;
    #endif
#endif
                }
                else if (iState[iSel] == 2) {
                    usOffset[iSel] |= ucTxByte;                          // collect buffer offset
                    if (ucChipCommand[iSel] == 0x58) {                   // {31} auto-page rewrite - the action actually task place when the CS line rises
                                                                         // the device first copies the main page to the buffer and then program the same buffer back tp the main memory. This takes about 15ms.
                        memcpy(ucAT45BDXXX_buffer1, &ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH)], sizeof(ucAT45BDXXX_buffer1)); // just overwrite the internal buffer
                    }
                }
                iState[iSel]++;
                break;
                
            case 0x50:                                                   // delete block
            case 0x81:                                                   // delete page {5}
                if (iState[iSel] == 0) {
                    usPage[iSel] = ucTxByte;                             // collect page
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024
                    usPage[iSel] <<= 6;
                    usPage[iSel] &= 0x3fc0;
    #else
                    usPage[iSel] <<= 5;
                    usPage[iSel] &= 0x1fe0;
    #endif
#elif SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512
                    usPage[iSel] <<= 7;
                    usPage[iSel] &= 0x7f80;
    #else
                    usPage[iSel] <<= 6;
                    usPage[iSel] &= 0x3fc0;
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256
                    usPage[iSel] <<= 8;
                    usPage[iSel] &= 0xff00;
    #else
                    usPage[iSel] <<= 7;
                    usPage[iSel] &= 0x7f80;
    #endif
#endif
                }
                else if (iState[iSel] == 1) {
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024
                    usPage[iSel] |= (ucTxByte >> 2);                     // collect page
    #else
                    usPage[iSel] |= (ucTxByte >> 3);                     // collect page
    #endif
#elif SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512
                    usPage[iSel] |= (ucTxByte >> 1);                     // collect page
    #else
                    usPage[iSel] |= (ucTxByte >> 2);                     // collect page
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256
                    usPage[iSel] |= ucTxByte;                            // collect page
    #else
                    usPage[iSel] |= (ucTxByte >> 1);                     // collect page
    #endif
#endif
                }
                else if (iState[iSel] == 2) {
                    if (ucChipCommand[iSel] == 0x50) {
                        usPage[iSel] &= ~0x0007;                         // delete block
                        memset(&ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset], 0xff, (8*SPI_FLASH_PAGE_LENGTH)); // {15}
                    }
                    else {                                               // delete page
                        memset(&ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset], 0xff, (SPI_FLASH_PAGE_LENGTH)); // {15}
                    }
                    iState[iSel] = 0;
                    break;
                }
                iState[iSel]++;
                break;   
            
            case 0x84:                                                   // write to buffer 1
                if (iState[iSel] == 0) {
                    usOffset[iSel] = 0;                                  // first byte is dummy
                }
                else if (iState[iSel] == 1) {                            // collect buffer offset 
#if SPI_FLASH_PAGE_LENGTH >= 1024              
    #if SPI_FLASH_PAGE_LENGTH == 1024
                    usOffset[iSel] = (ucTxByte & 0x03);
    #else
                    usOffset[iSel] = (ucTxByte & 0x07);
    #endif
#elif SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512
                    usOffset[iSel] = (ucTxByte & 0x01);
    #else
                    usOffset[iSel] = (ucTxByte & 0x03);
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256
                    usOffset[iSel] = 0;
    #else
                    usOffset[iSel] = (ucTxByte & 0x01);
    #endif
#endif
                    usOffset[iSel] <<= 8;
                }
                else if (iState[iSel] == 2) {
                    usOffset[iSel] |= ucTxByte;                          // collect buffer offset
                }
                else {
                    ucAT45BDXXX_buffer1[iSel][usOffset[iSel]] = ucTxByte;// put into buffer
                    if (++usOffset[iSel] >= SPI_FLASH_PAGE_LENGTH) {
                        usOffset[iSel] = 0;                              // wraparound
                    }
                    break;
                }
                iState[iSel]++;
                break;  
            }
        }
    }
    else if (AT45DBXXX_READ == iSimType) {                               // read
        switch (ucChipCommand[iSel]) {
#if defined SPI_FLASH_AT45XXXXD_TYPE                                     // {21} only respond if D-type
        case 0x9f:                                                       // read manufacturer's ID (only D-device)
            iState[iSel]++;
            if (iState[iSel] == 1) {
                return 0x1f;                                             // ATMEL
            }
            else if (iState[iSel] == 2) {
                return (FLASH_ID);                                       // the ID
            }
            else if (iState[iSel] == 3) {
                return 0;                                                // 1-bit per cell technology / initial version
            }
            else {
                iState[iSel] = 0;
                return 0;
            }
            break;
#endif

        case 0xd7:                                                       // read status
            return (0x80 + FLASH_SIZE + ucPageSize);                     // {21}

        default:                                                         // assume continuous array read
            {
                unsigned char ucValue = ucAT45DBXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + usOffset[iSel] + ulDeviceOffset];
                if (++usOffset[iSel] >= SPI_FLASH_PAGE_LENGTH) {
                    usOffset[iSel] = 0;
                    if (++usPage[iSel] >= SPI_FLASH_PAGES) {
                        usPage[iSel] = 0;
                    }
                }
                return ucValue;
            }
            break;
        }
    }
    return 0xff;
}
    #elif defined SPI_FLASH_ST                                           // {14}{48}
        #define MANUFACTURER_ST     0x20
        #define MEMORY_TYPE         0x20

        #if defined SPI_FLASH_STM25P05
            #define MEMORY_CAPACITY 0x10                                 // 256k Bit
        #elif defined SPI_FLASH_STM25P10
            #define MEMORY_CAPACITY 0x11                                 // 1M Bit
        #elif defined SPI_FLASH_STM25P20
            #define MEMORY_CAPACITY 0x12                                 // 2M Bit
        #elif defined SPI_FLASH_STM25P40
            #define MEMORY_CAPACITY 0x13                                 // 4M Bit
        #elif defined SPI_FLASH_STM25P80
            #define MEMORY_CAPACITY 0x14                                 // 8M Bit
        #elif defined SPI_FLASH_STM25P16
            #define MEMORY_CAPACITY 0x15                                 // 16M Bit
        #elif defined SPI_FLASH_STM25P32
            #define MEMORY_CAPACITY 0x16                                 // 32M Bit
        #elif defined SPI_FLASH_STM25P64
            #define MEMORY_CAPACITY 0x17                                 // 64M Bit
        #elif defined SPI_FLASH_STM25P128
            #define MEMORY_CAPACITY 0x18                                 // 128M Bit
        #endif
 
static unsigned char ucSTM25PXXX[SPI_DATA_FLASH_SIZE];                   // all SPI FLASH devices in one buffer

static unsigned char ucSTM25PXXX_buffer[SPI_FLASH_DEVICE_COUNT][SPI_FLASH_PAGE_LENGTH];


extern void fnInitSPI_DataFlash(void)
{
    memset(ucSTM25PXXX, 0xff, sizeof(ucSTM25PXXX));
}

extern unsigned char *fnGetDataFlashStart(void)
{
    return ucSTM25PXXX;
}

extern unsigned long fnGetDataFlashSize(void)
{
    return SPI_DATA_FLASH_SIZE;
}

extern unsigned char fnSimSTM25Pxxx(int iSimType, unsigned char ucTxByte)
{
    static unsigned char  ucChipCommand[SPI_FLASH_DEVICE_COUNT] = {0};
    static int            iState[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned short usPage[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned short usOffset[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned long  ulMainAddress[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned short usAddress[SPI_FLASH_DEVICE_COUNT] = {0};
    static unsigned char  WEL[SPI_FLASH_DEVICE_COUNT] = {0};             // write enable latches

    int iSel = 0;
        #if defined SPI_FLASH_MULTIPLE_CHIPS
    int iCntCS = 0;
    unsigned long ulDeviceOffset = 0;
    if (SPI_CS0_PORT & CS0_LINE) {
        if ((ucChipCommand[0] == 0x02) || (ucChipCommand[0] == 0xD8)) {
            WEL[0] = 0;
        }
        else if (ucChipCommand[0] == 0xC7) {                             // bulk erase chip {17}
            memset(&ucSTM25PXXX[0], 0xff, SPI_DATA_FLASH_0_SIZE);
        }
        ucChipCommand[0] = 0;
        iState[0] = 0;
    }
    else {
        iCntCS++;
    }
    if (SPI_CS1_PORT & CS1_LINE) {
        if ((ucChipCommand[1] == 0x02) || (ucChipCommand[1] == 0xD8)) {
            WEL[1] = 0;
        }
        else if (ucChipCommand[1] == 0xC7) {                             // bulk erase chip {17}
            memset(&ucSTM25PXXX[SPI_DATA_FLASH_0_SIZE], 0xff, SPI_DATA_FLASH_1_SIZE);
        }
        ucChipCommand[1] = 0;
        iState[1] = 0;
    }
    else {
        iCntCS++;
        iSel = 1;
        ulDeviceOffset = SPI_DATA_FLASH_0_SIZE;
    }
            #if defined QSPI_CS2_LINE || defined CS2_LINE
                #if defined QSPI_CS2_LINE
    if (SPI_CS2_PORT & CS2_LINE) 
            #else
    if (SPI_CS2_PORT & CS2_LINE) 
            #endif
    {
        if ((ucChipCommand[2] == 0x02) || (ucChipCommand[2] == 0xD8)) {
            WEL[2] = 0;
        }
        else if (ucChipCommand[2] == 0xC7) {                             // bulk erase chip {17}
            memset(&ucSTM25PXXX[SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE], 0xff, SPI_DATA_FLASH_2_SIZE);
        }
        ucChipCommand[2] = 0;
        iState[2] = 0;
    }
    else {
        iCntCS++;
        iSel = 2;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    }
        #endif
        #if defined QSPI_CS3_LINE || defined CS3_LINE
            #if defined QSPI_CS3_LINE
    if (SPI_CS3_PORT & CS3_LINE) 
            #else
    if (SPI_CS3_PORT & CS3_LINE) 
            #endif
    {
        if ((ucChipCommand[3] == 0x03) || (ucChipCommand[3] == 0xD8)) {
            WEL[3] = 0;
        }
        else if (ucChipCommand[3] == 0xC7) {                             // bulk erase chip {17}
            memset(&ucSTM25PXXX[SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE], 0xff, SPI_DATA_FLASH_3_SIZE);
        }
        ucChipCommand[3] = 0;
        iState[3] = 0;
    }
    else {
        iCntCS++;
        iSel = 3;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE);
    }
        #endif
    if (iCntCS > 1) {
        *(unsigned char *)(0) = 0;                                       // 2 CS selected at same time - serious error
    }
    else if (iCntCS == 0) {
        return 0xff;                                                     // chip not selected, return idle
    }
    #else
    #define ulDeviceOffset 0
    if (SPI_CS0_PORT & CS0_LINE) {
        ucChipCommand[0] = 0;
        iState[0] = 0;
        return 0xff;                                                     // chip not selected, return idle
    }
    #endif

    if (iSimType == STM25PXXX_WRITE) {
        if (ucChipCommand[iSel] == 0) {
            ucChipCommand[iSel] = ucTxByte;                              // we interpret a command
            if (ucTxByte == 0x06) {                                      // write enable
                WEL[iSel] = 1;
            }
        }
        else {                                                           // in command
            switch (ucChipCommand[iSel]) {
            case 0x9f:                                                   // read manufacturer's ID
                break;
            case 0x02:                                                   // page program
                if (iState[iSel] == 0) {
                    usPage[iSel] = ucTxByte;                             // collect page number
                    usPage[iSel] <<= 8;
                }
                else if (iState[iSel] == 1) {
                    usPage[iSel] |= ucTxByte;                            // collect page number
                }
                else if (iState[iSel] == 2) {
                    usOffset[iSel] = ucTxByte;                           // collect offset
                }
                else {
                    if (WEL[iSel]) {
                        ucSTM25PXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + usOffset[iSel] + ulDeviceOffset] &= ucTxByte;
                    }
                    if (++usOffset[iSel] == 256) {
                        usOffset[iSel] = 0;
                    }
                }
                iState[iSel]++;
                break;
                
            case 0x03:                                                   // read array
                if (iState[iSel] == 0) {
                    usPage[iSel] = ucTxByte;                             // collect page number
                    usPage[iSel] <<= 8;
                }
                else if (iState[iSel] == 1) {
                    usPage[iSel] |= ucTxByte;                            // collect page number
                }
                else if (iState[iSel] == 2) {
                    usOffset[iSel] = ucTxByte;                           // collect offset
                }
                iState[iSel]++;
                break;

#if defined SPI_DATA_FLASH
            case 0x20:                                                   // sub-sector erase
#endif
            case 0xd8:                                                   // erase
                if (iState[iSel] == 0) {
                    usPage[iSel] = ucTxByte;                             // collect page
                    usPage[iSel] <<= 8;
                }
                else if (iState[iSel] == 1) {
                    usPage[iSel] |= ucTxByte;                            // collect page
                }
                else if (iState[iSel] == 2) {
                    usOffset[iSel] = ucTxByte;
                    if (ucChipCommand[iSel] == 0x20) {
                        memset(&ucSTM25PXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset], 0xff, (16*SPI_FLASH_PAGE_LENGTH)); // delete sub-sector
                    }
                    else {
                        memset(&ucSTM25PXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + ulDeviceOffset], 0xff, (256*SPI_FLASH_PAGE_LENGTH)); // delete sector
                    }
                    iState[iSel] = 0;
                    break;
                }
                iState[iSel]++;
                break;   
            
            }
        }
    }
    else if (STM25PXXX_READ == iSimType) {                               // read
        switch (ucChipCommand[iSel]) {
        case 0x9f:                                                       // read manufacturer's ID
            iState[iSel]++;
            if (iState[iSel] == 1) {
                return (MANUFACTURER_ST);                                // ST
            }
            else if (iState[iSel] == 2) {
                return (MEMORY_TYPE);
            }
            else if (iState[iSel] == 3) {
                return (MEMORY_CAPACITY);
            }
            else {
                iState[iSel] = 0;
                return 0;
            }
            break;

        case 0x05:                                                       // read status 
            return (0x00);

        default:                                                         // assume continuous array read
            {
            unsigned char ucValue = ucSTM25PXXX[(usPage[iSel] * SPI_FLASH_PAGE_LENGTH) + usOffset[iSel] + ulDeviceOffset];
            if (++usOffset[iSel] >= SPI_FLASH_PAGE_LENGTH) {
                usOffset[iSel] = 0;
                if (++usPage[iSel] >= SPI_FLASH_PAGES) {
                    usPage[iSel] = 0;
                }
            }
            return ucValue;
            }
            break;
        }
    }
    return 0xff;
}
    #elif defined SPI_FLASH_W25Q
        #define MANUFACTURER_WB     0xef

        #if defined SPI_FLASH_W25Q128                                    // {70}
            #define MEMORY_TYPE     0x60
            #define MEMORY_CAPACITY 0x18                                 // 128M Bit
        #elif defined SPI_FLASH_W25Q16
            #define MEMORY_TYPE     0x40
            #define MEMORY_CAPACITY 0x15                                 // 16M Bit
        #endif

 
static unsigned char ucW25Q[SPI_DATA_FLASH_SIZE];                        // all SPI FLASH devices in one buffer

static unsigned char  ucStatus[SPI_FLASH_DEVICE_COUNT] = {
    0x1c,
        #if SPI_FLASH_DEVICE_COUNT > 1
    0x1c,
        #endif
        #if SPI_FLASH_DEVICE_COUNT > 2
    0x1c,
        #endif
        #if SPI_FLASH_DEVICE_COUNT > 3
    0x1c,
        #endif
};

static unsigned char  ucChipCommand[SPI_FLASH_DEVICE_COUNT] = {0};
static int            iState[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned long  ulAccessAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usPage[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usOffset[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned long  ulMainAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned char  WEL[SPI_FLASH_DEVICE_COUNT] = {0};                 // write enable latches
static unsigned char  ucProgramBuffer[SPI_FLASH_DEVICE_COUNT][256] = {{0}};
static unsigned char  ucStatusReg[SPI_FLASH_DEVICE_COUNT] = {0};


extern void fnInitSPI_DataFlash(void)
{
    memset(ucW25Q, 0xff, sizeof(ucW25Q));
}

extern unsigned char *fnGetDataFlashStart(void)
{
    return ucW25Q;
}

extern unsigned long fnGetDataFlashSize(void)
{
    return SPI_DATA_FLASH_SIZE;
}

static int fnAddressAllowed(int iDev, unsigned long ulAddress)
{
    unsigned char ucLimit = (ucStatus[iDev] >> 2);
    ucLimit &= 0x07;
        #if defined SPI_FLASH_SST25VF032B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x3f0000) {                                      // upper 1/64
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x3e0000) {                                      // upper 1/32
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0x3c0000) {                                      // upper 1/16
            return 0;
        }
        break;
    case 0x04:
        if (ulAddress > 0x380000) {                                      // upper 1/8
            return 0;
        }
        break;
    case 0x05:
        if (ulAddress > 0x300000) {                                      // upper 1/4
            return 0;
        }
        break;
    case 0x06:
        if (ulAddress > 0x20000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF016B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x1f0000) {                                      // upper 1/32
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x1e0000) {                                      // upper 1/16
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0x1c0000) {                                      // upper 1/8
            return 0;
        }
        break;
    case 0x04:
        if (ulAddress > 0x180000) {                                      // upper 1/4
            return 0;
        }
        break;
    case 0x05:
        if (ulAddress > 0x100000) {                                      // upper 1/2
            return 0;
        }
        break;
    case 0x06:
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF080B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0xf0000) {                                       // upper 1/16
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0xe0000) {                                       // upper 1/8
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0xc0000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x04:
        if (ulAddress > 0x80000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x05:
    case 0x06:
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF040B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x70000) {                                       // upper 1/8
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x60000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0x40000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25LF040A
    ucLimit &= 0x03;
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x60000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x40000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x03:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25LF020A
    ucLimit &= 0x03;
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x30000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x20000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x03:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF010A
    ucLimit &= 0x03;
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x18000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x10000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x03:
        return 0;                                                        // all blocks protected
    }
        #endif
    return 1;
}

static void fnActionW25Q(int iSel, unsigned long ulDeviceOffset)
{
    if ((ucChipCommand[iSel] == 2) && (WEL[iSel] != 0) && (iState[iSel] == 4)) { // page write to be performed
        unsigned long ulAdd = (ulAccessAddress[iSel] & ~0x000000ff);     // start address of present page
        if (fnAddressAllowed(0, ulAdd)) {                                // check whether area is protected
            int iOffset = 0;
            while (iOffset < 256) {
                ucW25Q[ulAdd + iOffset] &= ucProgramBuffer[iSel][iOffset]; // set bits low in complete page
                iOffset++;
            }
        }
    }
    else if ((WEL[iSel] == 1) && (iState[iSel] == 3)) {                  // if erase enabled
        if (ucChipCommand[iSel] == 0x20) {                               // delete sub-sector
            memset(&ucW25Q[(ulAccessAddress[iSel] & ~(SPI_FLASH_SUB_SECTOR_LENGTH - 1)) + ulDeviceOffset], 0xff, SPI_FLASH_SUB_SECTOR_LENGTH); // delete sub-sector
        }
        else if (ucChipCommand[iSel] == 0x52) {                          // delete half sector
            memset(&ucW25Q[(ulAccessAddress[iSel] & ~(SPI_FLASH_HALF_SECTOR_LENGTH - 1)) + ulDeviceOffset], 0xff, SPI_FLASH_HALF_SECTOR_LENGTH); // delete half sector
        }
        else if (ucChipCommand[iSel] == 0xd8) {                          // delete sector
            memset(&ucW25Q[(ulAccessAddress[iSel] & ~(SPI_FLASH_SECTOR_LENGTH - 1)) + ulDeviceOffset], 0xff, SPI_FLASH_SECTOR_LENGTH); // delete sector
        }
        ucStatus[iSel] &= ~0x02;
        WEL[iSel] = 0;
    }
    else if (ucChipCommand[iSel] == 0x04) {                              // write disable
        ucStatus[iSel] &= ~0x42;
        WEL[iSel] = 0;
    }
    else if (ucChipCommand[iSel] == 0x01) {
        ucStatus[iSel] &= ~0x1c;
        ucStatus[iSel] |= (ucStatusReg[iSel] & 0x1c);
    }
    ucChipCommand[iSel] = 0;
    iState[iSel] = 0;
}

extern unsigned char fnSimW25Qxx(int iSimType, unsigned char ucTxByte)
{
    int iSel = 0;
        #if defined SPI_FLASH_MULTIPLE_CHIPS
    int iCntCS = 0;
    unsigned long ulDeviceOffset = 0;
    if (SPI_CS0_PORT & CS0_LINE) {
        fnActionSST25(0, 0);
    }
    else {
        iCntCS++;
    }
    if (SPI_CS1_PORT & CS1_LINE) {
        fnActionSST25(1, SPI_DATA_FLASH_0_SIZE);
    }
    else {
        iCntCS++;
        iSel = 1;
        ulDeviceOffset = SPI_DATA_FLASH_0_SIZE;
    }
            #if defined QSPI_CS2_LINE || defined CS2_LINE
    if (SPI_CS2_PORT & CS2_LINE) {
        fnActionSST25(2, (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE));
    }
    else {
        iCntCS++;
        iSel = 2;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    }
            #endif
            #if defined QSPI_CS3_LINE || defined CS3_LINE
    if (SPI_CS3_PORT & CS3_LINE) {
        fnActionSST25(3, (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE));
    }
    else {
        iCntCS++;
        iSel = 3;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE);
    }
            #endif
    if (iCntCS > 1) {
        *(unsigned char *)(0) = 0;                                       // 2 CS selected at same time - serious error
    }
    else if (iCntCS == 0) {
        return 0xff;                                                     // chip not selected, return idle
    }
        #else
        #define ulDeviceOffset 0
    if (SPI_CS0_PORT & CS0_LINE) {                                       // CS0 line negated
        fnActionW25Q(0, 0);
        return 0xff;                                                     // chip not selected, return idle
    }
        #endif

    if (iSimType == W25Q_WRITE) {
        if (ucChipCommand[iSel] == 0) {
            ucChipCommand[iSel] = ucTxByte;                              // we interpret a command
            if (ucTxByte == 0x06) {                                      // write enable
                ucStatus[iSel] |= 0x02;
                WEL[iSel] = 1;
            }/*
            else if (ucTxByte == AAI_WRITE) {
                ucStatus[iSel] |= 0x40;                                  // mark in AAI mode
            }*/
        }
        else {                                                           // in command
            switch (ucChipCommand[iSel]) {
            case 0x01:                                                   // set status register
                ucStatusReg[iSel] = ucTxByte;
                break;
            case 0x9f:                                                   // read manufacturer's ID
                break;
            case 0x02:                                                   // program in page
                if (iState[iSel] == 3) {
                    memset(ucProgramBuffer[iSel], 0xff, 256);            // clear the buffer before starting collecting new data
                    iState[iSel] = 4;
                }
                if (iState[iSel] == 4) {
                    unsigned char ucProgramAddress = (ulAccessAddress[iSel] & 0xff);
                    ucProgramBuffer[iSel][ucProgramAddress++] = ucTxByte;// collect the data to be programmed
                    if (ucProgramAddress == 0) {
                        ulAccessAddress[iSel] &= ~0x000000ff;
                    }
                    else {
                        ulAccessAddress[iSel]++;
                    }
                    break;
                }
            case 0x90:                                                   // read ID
            case 0xab:
            case 0x20:                                                   // sub-sector erase
            case 0x52:                                                   // half-sector erase
        #ifndef SST25_A_VERSION
            case 0xd8:                                                   // sector erase - not available on A device
        #endif
                // Else fall through
                //
            case 0x03:                                                   // read array
                if (iState[iSel] == 0) {
                    ulAccessAddress[iSel] = ucTxByte;                    // collect access address
                    ulAccessAddress[iSel] <<= 8;
                }
                else if (iState[iSel] == 1) {
                    ulAccessAddress[iSel] |= ucTxByte;                   // collect access address
                    ulAccessAddress[iSel] <<= 8;
                }
                else if (iState[iSel] == 2) {
                    ulAccessAddress[iSel] |= ucTxByte;                   // access address
                }
                iState[iSel]++;
                break; 
/*                
            case AAI_WRITE:                                              // AAI write sequence
                if (WEL[iSel] != 0) {                                    // write protect has been removed
                    if (WEL[iSel] == 1) {                                // collecting address
                        if (iState[iSel] == 0) {
                            ulAccessAddress[iSel] = ucTxByte;            // collect access address
                            ulAccessAddress[iSel] <<= 8;
                        }
                        else if (iState[iSel] == 1) {
                            ulAccessAddress[iSel] |= ucTxByte;           // collect access address
                            ulAccessAddress[iSel] <<= 8;
                        }
                        else if (iState[iSel] == 2) {
                            ulAccessAddress[iSel] |= (ucTxByte & ~0x01); // access address 
                            WEL[iSel] = 2;                               // address collected and in AAI sequence
                        }
                        iState[iSel]++;
                    }
                    else {
                        if (WEL[iSel] > 3) {                             // check for incorrect use
                            *(unsigned char *)0 = 0;                     // exception because CS should be removed before writing mode data
                        }
                        ucWord[iSel][WEL[iSel] - 2] = ucTxByte;          // collect data pair
                        WEL[iSel]++;
                    }
                }
                break;
            */
            }
        }
    }
    else if (W25Q_READ == iSimType) {                                    // read
        switch (ucChipCommand[iSel]) {
        case 0x90:                                                       // read ID
        case 0xab:
            if (ulAccessAddress[iSel] == 0) {
                ulAccessAddress[iSel] = 1;
                return (MANUFACTURER_WB);                                // Winbond
            }
            else if (ulAccessAddress[iSel] == 0x1) {
                ulAccessAddress[iSel] = 0;
                return (MEMORY_CAPACITY);
            }
            break;
        case 0x9f:                                                       // read manufacturer's ID
            iState[iSel]++;
            if (iState[iSel] == 1) {
                return (MANUFACTURER_WB);                                // Winbond
            }
            else if (iState[iSel] == 2) {
                return (MEMORY_TYPE);
            }
            else if (iState[iSel] == 3) {
                iState[iSel] = 0;                                        // command complete
                return (MEMORY_CAPACITY);
            }
            break;
        case 0x05:                                                       // read status 
            return (ucStatus[iSel]);

        default:                                                         // assume continuous array read
            {
            unsigned char ucValue = ucW25Q[ulAccessAddress[iSel] + ulDeviceOffset];
        #if defined SPI_FLASH_MULTIPLE_CHIPS
            switch (iSel) {
            case 0:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_0_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 1:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_1_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 2:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_2_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 3:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_3_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            }
        #else
            if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_0_SIZE) {
                ulAccessAddress[iSel] = 0;
            }
        #endif
            return ucValue;
            }
            break;
        }
    }
    return 0xff;
}
    #elif defined SPI_FLASH_S25FL1_K

static unsigned char ucS25FLI_K[SPI_DATA_FLASH_SIZE] = {0};

static unsigned char  ucChipCommand[SPI_FLASH_DEVICE_COUNT] = {0};
static int            iState[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned long  ulAccessAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usPage[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usOffset[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned long  ulMainAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned char  WEL[SPI_FLASH_DEVICE_COUNT] = {0};                 // write enable latches
static unsigned char  WESR[SPI_FLASH_DEVICE_COUNT] = {0};                // write enable status register
static unsigned char  ucProgramBuffer[SPI_FLASH_DEVICE_COUNT][256] = {{0}};
static unsigned char  ucStatus[SPI_FLASH_DEVICE_COUNT][3] = {0};
static unsigned char  ucWord[SPI_FLASH_DEVICE_COUNT][2] = {0};

#define MANUFACTURER_SPANSION 0x01
#define MEMORY_TYPE           0x40
#if defined SPI_FLASH_S25FL164K
    #define MEMORY_CAPACITY   0x17
#elif defined SPI_FLASH_S25FL132K
    #define MEMORY_CAPACITY   0x16
#else
    #define MEMORY_CAPACITY   0x16                                       // S25FL116K
#endif

extern void fnInitSPI_DataFlash(void)
{
    memset(ucS25FLI_K, 0xff, sizeof(ucS25FLI_K));                        // initialise SPI flash content to deleted state
}

extern unsigned char *fnGetDataFlashStart(void)
{
    return ucS25FLI_K;                                                   // return a pointer to the start of the simulated SPI flash data content
}

extern unsigned long fnGetDataFlashSize(void)
{
    return SPI_DATA_FLASH_SIZE;                                          // return the size of the SPI flash content
}


static int fnAddressAllowed(int iDev, unsigned long ulAddress)
{
    return 0;
}

static void fnActionS25FL1_K(int iSel, unsigned long ulDeviceOffset)
{
    if (ucChipCommand[iSel] == 0x02) {                                   // page write
        if (WEL[iSel] != 0) {                                            // if write enabled
            uMemcpy(&ucS25FLI_K[ulAccessAddress[iSel] & ~(256 - 1)], ucProgramBuffer[iSel], 256); // write page to flash
            WEL[iSel] = 0;                                               // automatically clear the write enable
        }
    }
    else if ((WEL[iSel] == 1) && (iState[iSel] == 3)) {                  // if erase enabled
        if (ucChipCommand[iSel] == 0x20) {                               // delete sector
            memset(&ucS25FLI_K[(ulAccessAddress[iSel] & ~(SPI_FLASH_SECTOR_LENGTH - 1)) + ulDeviceOffset], 0xff, SPI_FLASH_SECTOR_LENGTH); // delete sector
        }
        else if (ucChipCommand[iSel] == 0xd8) {                          // delete 64k block
            memset(&ucS25FLI_K[(ulAccessAddress[iSel] & ~((64 * 1024) - 1)) + ulDeviceOffset], 0xff, (64 * 1024)); // delete sector
        }
        ucStatus[iSel][0] &= ~0x02;                                      // automatically clear the write enable
        WEL[iSel] = 0;
    }
    else if (ucChipCommand[iSel] == 0x04) {                              // write disable
        if (WESR[iSel] != 0) {                                           // if writes to the status register are enabled
            ucStatus[iSel][0] = ucWord[iSel][0];
        }
    }
    else if (ucChipCommand[iSel] == 0x01) {                              // write to the status register 0
        ucStatus[iSel][0] &= ~0x42;
        WEL[iSel] = 0;
    }
    else if (ucChipCommand[iSel] == 0x50) {                              // write enable status register
        WESR[iSel] = 1;
    }
    ucChipCommand[iSel] = 0;
    iState[iSel] = 0;
}

extern unsigned char fnSimS25FL1_K(int iSimType, unsigned char ucTxByte)
{
    int iSel = 0;
        #if defined SPI_FLASH_MULTIPLE_CHIPS
    int iCntCS = 0;
    unsigned long ulDeviceOffset = 0;
    if ((SPI_CS0_PORT & CS0_LINE) != 0) {                                // CS0 is negated
        fnActionS25FL1_K(0, 0);
    }
    else {                                                               // CS0 is asserted
        iCntCS++;
    }
    if ((SPI_CS1_PORT & CS1_LINE) != 0) {                                // CS1 is negated
        fnActionS25FL1_K(1, SPI_DATA_FLASH_0_SIZE);
    }
    else {                                                               // CS1 is asserted
        iCntCS++;
        iSel = 1;
        ulDeviceOffset = SPI_DATA_FLASH_0_SIZE;
    }
            #if defined QSPI_CS2_LINE || defined CS2_LINE
    if ((SPI_CS2_PORT & CS2_LINE) != 0) {                                // CS2 is negated
        fnActionS25FL1_K(2, (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE));
    }
    else {                                                               // CS2 is asserted
        iCntCS++;
        iSel = 2;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    }
            #endif
            #if defined QSPI_CS3_LINE || defined CS3_LINE
    if ((SPI_CS3_PORT & CS3_LINE) != 0) {                                // CS3 is negated
        fnActionS25FL1_K(3, (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE));
    }
    else {                                                               // CS is asserted
        iCntCS++;
        iSel = 3;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE);
    }
            #endif
    if (iCntCS > 1) {
        *(unsigned char *)(0) = 0;                                       // 2 CS selected at same time - serious error
    }
    else if (iCntCS == 0) {
        return 0xff;                                                     // chip not selected, return idle
    }
        #else
        #define ulDeviceOffset 0
    if (SPI_CS0_PORT & CS0_LINE) {                                       // CS0 line negated
        fnActionS25FL1_K(0, 0);
        return 0xff;                                                     // chip not selected, return idle
    }
        #endif

    if (iSimType == S25FL1_K_WRITE) {
        if (ucChipCommand[iSel] == 0) {                                  // command byte being received
            ucChipCommand[iSel] = ucTxByte;                              // we interpret a command
            if (ucTxByte == 0x06) {                                      // write enable
                ucStatus[iSel][0] |= 0x02;                               // set the write enable latch flag in the status register 0
                WEL[iSel] = 1;
            }
        }
        else {                                                           // in command
            switch (ucChipCommand[iSel]) {
            case 0x01:                                                   // set status register
                ucWord[iSel][0] = ucTxByte;
                break;
            case 0x9f:                                                   // read manufacturer's ID
                break;
            case 0x02:                                                   // page program  
                if (iState[iSel] >= 3) {
                    unsigned char *ptrPageContent = &ucProgramBuffer[iSel][(ulAccessAddress[iSel] + (iState[iSel] - 3)) & (256 - 1)];
                    if (~(*ptrPageContent) & ucTxByte) {                 // is a bit being set from '0' to '1'?
                        _EXCEPTION("Writing a '1' to a bit that is already programmed to '0'!!!");
                    }
                    *ptrPageContent = ucTxByte;                          // collect the bytes to be programmed in the program buffer
                    iState[iSel]++;
                    if (iState[iSel] >= (256 + 3)) {                     // if more than a page is written wrap back to the beginning
                        iState[iSel] = 3;
                    }
                    break;
                }
            case 0x90:                                                   // read ID
            case 0xab:
            case 0x20:                                                   // sub-sector erase
            case 0x52:                                                   // half-sector erase
        #ifndef SST25_A_VERSION
            case 0xd8:                                                   // sector erase - not available on A device
        #endif
                // Else fall through
                //
            case 0x03:                                                   // read array
                if (iState[iSel] == 0) {
                    ulAccessAddress[iSel] = ucTxByte;                    // collect access address
                    ulAccessAddress[iSel] <<= 8;
                }
                else if (iState[iSel] == 1) {
                    ulAccessAddress[iSel] |= ucTxByte;                   // collect access address
                    ulAccessAddress[iSel] <<= 8;
                }
                else if (iState[iSel] == 2) {
                    ulAccessAddress[iSel] |= ucTxByte;                   // access address complete
                    if (ucChipCommand[iSel] == 0x02) {                   // page program command
                        uMemcpy(ucProgramBuffer[iSel], &ucS25FLI_K[ulDeviceOffset + (ulAccessAddress[iSel] & ~(256 - 1))], 256); // load the present page content to the program buffer
                    }
                }
                iState[iSel]++;
                break;             
            }
        }
    }
    else if (S25FL1_K_READ == iSimType) {                                // read
        switch (ucChipCommand[iSel]) {
        case 0x90:                                                       // read ID
        case 0xab:
            if (ulAccessAddress[iSel] == 0) {
                ulAccessAddress[iSel] = 1;
                return (MANUFACTURER_SPANSION);                          // Spansion
            }
            else if (ulAccessAddress[iSel] == 0x1) {
                ulAccessAddress[iSel] = 0;
                return (MEMORY_CAPACITY);
            }
            break;
        case 0x9f:                                                       // read JEDEC ID
            iState[iSel]++;
            if (iState[iSel] == 1) {
                return (MANUFACTURER_SPANSION);                          // Spansion
            }
            else if (iState[iSel] == 2) {
                return (MEMORY_TYPE);
            }
            else if (iState[iSel] == 3) {
                iState[iSel] = 0;                                        // command complete
                return (MEMORY_CAPACITY);
            }
            break;
        case 0x05:                                                       // read status register 1
            return (ucStatus[iSel][0]);
        case 0x35:                                                       // read status register 2
            return (ucStatus[iSel][1]);
        case 0x33:                                                       // read status register 3
            return (ucStatus[iSel][2]);

        default:                                                         // assume continuous array read
            {
            unsigned char ucValue = ucS25FLI_K[ulAccessAddress[iSel] + ulDeviceOffset];
        #if defined SPI_FLASH_MULTIPLE_CHIPS
            switch (iSel) {
            case 0:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_0_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 1:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_1_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 2:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_2_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 3:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_3_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            }
        #else
            if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_0_SIZE) {
                ulAccessAddress[iSel] = 0;
            }
        #endif
            return ucValue;
            }
            break;
        }
    }
    return 0xff;
}
    #elif defined SPI_FLASH_SST25                                        // {22}

        #define MANUFACTURER_SST    0xbf
        #define MEMORY_TYPE         0x25

        #if defined SPI_FLASH_SST25VF010A
            #define MEMORY_CAPACITY 0x49                                 // 1M Bit A type
        #elif defined SPI_FLASH_SST25LF020A
            #define MEMORY_CAPACITY 0x43                                 // 2M Bit A type
        #elif defined SPI_FLASH_SST25LF040A
            #define MEMORY_CAPACITY 0x44                                 // 4M Bit A type
        #elif defined SPI_FLASH_SST25VF040B
            #define MEMORY_CAPACITY 0x8d                                 // 4M Bit
        #elif defined SPI_FLASH_SST25VF080B
            #define MEMORY_CAPACITY 0x8e                                 // 8M Bit
        #elif defined SPI_FLASH_SST25VF016B
            #define MEMORY_CAPACITY 0x41                                 // 16M Bit
        #elif defined SPI_FLASH_SST25VF032B
            #define MEMORY_CAPACITY 0x4a                                 // 32M Bit
        #endif

        #if defined SST25_A_VERSION
            #define AAI_WRITE     0xaf                                   // the auto-increment write command has a different value for the A devices
        #else
            #define AAI_WRITE     0xad
        #endif
 
static unsigned char ucSST25[SPI_DATA_FLASH_SIZE];                       // all SPI FLASH devices in one buffer

static unsigned char ucSST25_buffer[SPI_FLASH_DEVICE_COUNT][SPI_FLASH_PAGE_LENGTH];

static unsigned char  ucStatus[SPI_FLASH_DEVICE_COUNT] = {
    0x1c,
        #if SPI_FLASH_DEVICE_COUNT > 1
    0x1c,
        #endif
        #if SPI_FLASH_DEVICE_COUNT > 2
    0x1c,
        #endif
        #if SPI_FLASH_DEVICE_COUNT > 3
    0x1c,
        #endif
};

static unsigned char  ucChipCommand[SPI_FLASH_DEVICE_COUNT] = {0};
static int            iState[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned long  ulAccessAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usPage[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usOffset[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned long  ulMainAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned short usAddress[SPI_FLASH_DEVICE_COUNT] = {0};
static unsigned char  WEL[SPI_FLASH_DEVICE_COUNT] = {0};                 // write enable latches
static unsigned char  WESR[SPI_FLASH_DEVICE_COUNT] = {0};                // write enable status register
static unsigned char  ucWord[SPI_FLASH_DEVICE_COUNT][2] = {0};


extern void fnInitSPI_DataFlash(void)
{
    memset(ucSST25, 0xff, sizeof(ucSST25));
}

extern unsigned char *fnGetDataFlashStart(void)
{
    return ucSST25;
}

extern unsigned long fnGetDataFlashSize(void)
{
    return SPI_DATA_FLASH_SIZE;
}

static int fnAddressAllowed(int iDev, unsigned long ulAddress)
{
    unsigned char ucLimit = (ucStatus[iDev] >> 2);
    ucLimit &= 0x07;
        #if defined SPI_FLASH_SST25VF032B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x3f0000) {                                      // upper 1/64
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x3e0000) {                                      // upper 1/32
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0x3c0000) {                                      // upper 1/16
            return 0;
        }
        break;
    case 0x04:
        if (ulAddress > 0x380000) {                                      // upper 1/8
            return 0;
        }
        break;
    case 0x05:
        if (ulAddress > 0x300000) {                                      // upper 1/4
            return 0;
        }
        break;
    case 0x06:
        if (ulAddress > 0x20000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF016B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x1f0000) {                                      // upper 1/32
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x1e0000) {                                      // upper 1/16
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0x1c0000) {                                      // upper 1/8
            return 0;
        }
        break;
    case 0x04:
        if (ulAddress > 0x180000) {                                      // upper 1/4
            return 0;
        }
        break;
    case 0x05:
        if (ulAddress > 0x100000) {                                      // upper 1/2
            return 0;
        }
        break;
    case 0x06:
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF080B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0xf0000) {                                       // upper 1/16
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0xe0000) {                                       // upper 1/8
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0xc0000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x04:
        if (ulAddress > 0x80000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x05:
    case 0x06:
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF040B
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x70000) {                                       // upper 1/8
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x60000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x03:
        if (ulAddress > 0x40000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25LF040A
    ucLimit &= 0x03;
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x60000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x40000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x03:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25LF020A
    ucLimit &= 0x03;
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x30000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x20000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x03:
        return 0;                                                        // all blocks protected
    }
        #elif defined SPI_FLASH_SST25VF010A
    ucLimit &= 0x03;
    switch (ucLimit) {
    case 0x00:
        break;                                                           // no protection
    case 0x01:
        if (ulAddress > 0x18000) {                                       // upper 1/4
            return 0;
        }
        break;
    case 0x02:
        if (ulAddress > 0x10000) {                                       // upper 1/2
            return 0;
        }
        break;
    case 0x03:
        return 0;                                                        // all blocks protected
    }
        #endif
    return 1;
}

static void fnActionSST25(int iSel, unsigned long ulDeviceOffset)
{
        #if defined SST25_A_VERSION
    if ((ucChipCommand[iSel] == AAI_WRITE) && (WEL[iSel] == 3)) 
        #else
    if ((ucChipCommand[iSel] == AAI_WRITE) && (WEL[iSel] == 4)) 
        #endif
    {                                                                    // an AAI write pair program should be started on CS negation
        if (fnAddressAllowed(iSel, ulAccessAddress[iSel])) {
            ucSST25[ulAccessAddress[iSel] + ulDeviceOffset] &= ucWord[iSel][0];
        }
        ulAccessAddress[iSel]++;
        #ifndef SST25_A_VERSION
        if (fnAddressAllowed(0, ulAccessAddress[iSel])) {
            ucSST25[ulAccessAddress[iSel] + ulDeviceOffset] &= ucWord[iSel][1];
        }
        ulAccessAddress[iSel]++;
        #endif
        WEL[iSel] = 2;                                                   // still in AAI sequence
        if (ulAccessAddress[iSel] >= SPI_DATA_FLASH_0_SIZE) {            // when highest location reached
            ucStatus[iSel] &= ~0x42;                                     // reset mode
            ucChipCommand[iSel] = 0;
            WEL[iSel] = 0;
        }
    }
    else if ((ucChipCommand[iSel] == 2) && (WEL[iSel] != 0) && (iState[iSel] == 4)) { // single byte write to be performed
        if (fnAddressAllowed(0, ulAccessAddress[iSel])) {
            ucSST25[ulAccessAddress[iSel] + ulDeviceOffset] &= ucWord[iSel][0];
        }
    }
    else if ((WEL[iSel] == 1) && (iState[iSel] == 3)) {                  // if erase enabled
        if (ucChipCommand[iSel] == 0x20) {                               // delete sub-sector
            memset(&ucSST25[(ulAccessAddress[iSel] & ~(SPI_FLASH_SUB_SECTOR_LENGTH - 1)) + ulDeviceOffset], 0xff, SPI_FLASH_SUB_SECTOR_LENGTH); // delete sub-sector
        }
        else if (ucChipCommand[iSel] == 0x52) {                          // delete half sector
            memset(&ucSST25[(ulAccessAddress[iSel] & ~(SPI_FLASH_HALF_SECTOR_LENGTH - 1)) + ulDeviceOffset], 0xff, SPI_FLASH_HALF_SECTOR_LENGTH); // delete half sector
        }
        #ifndef SST25_A_VERSION
        else if (ucChipCommand[iSel] == 0xd8) {                          // delete sector
            memset(&ucSST25[(ulAccessAddress[iSel] & ~(SPI_FLASH_SECTOR_LENGTH - 1)) + ulDeviceOffset], 0xff, SPI_FLASH_SECTOR_LENGTH); // delete sector
        }
        #endif
        ucStatus[iSel] &= ~0x02;
        WEL[iSel] = 0;
    }
    else if (ucChipCommand[iSel] == 0x04) {                              // write disable
        ucStatus[iSel] &= ~0x42;
        WEL[iSel] = 0;
    }
    else if (ucChipCommand[iSel] == 0x50) {                              // write enable status register
        WESR[iSel] = 1;
    }
    else if ((ucChipCommand[iSel] == 0x01) && (WESR[iSel] != 0)) {
        ucStatus[iSel] &= ~0x1c;
        ucStatus[iSel] |= ucWord[iSel][0] & 0x1c;
        WESR[iSel] = 0;
    }
    ucChipCommand[iSel] = 0;
    iState[iSel] = 0;
}

extern unsigned char fnSimSST25(int iSimType, unsigned char ucTxByte)
{
    int iSel = 0;
        #if defined SPI_FLASH_MULTIPLE_CHIPS
    int iCntCS = 0;
    unsigned long ulDeviceOffset = 0;
    if (SPI_CS0_PORT & CS0_LINE) {
        fnActionSST25(0, 0);
    }
    else {
        iCntCS++;
    }
    if (SPI_CS1_PORT & CS1_LINE) {
        fnActionSST25(1, SPI_DATA_FLASH_0_SIZE);
    }
    else {
        iCntCS++;
        iSel = 1;
        ulDeviceOffset = SPI_DATA_FLASH_0_SIZE;
    }
            #if defined QSPI_CS2_LINE || defined CS2_LINE
    if (SPI_CS2_PORT & CS2_LINE) {
        fnActionSST25(2, (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE));
    }
    else {
        iCntCS++;
        iSel = 2;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    }
            #endif
            #if defined QSPI_CS3_LINE || defined CS3_LINE
    if (SPI_CS3_PORT & CS3_LINE) {
        fnActionSST25(3, (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE));
    }
    else {
        iCntCS++;
        iSel = 3;
        ulDeviceOffset = (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE);
    }
            #endif
    if (iCntCS > 1) {
        *(unsigned char *)(0) = 0;                                       // 2 CS selected at same time - serious error
    }
    else if (iCntCS == 0) {
        return 0xff;                                                     // chip not selected, return idle
    }
        #else
        #define ulDeviceOffset 0
    if (SPI_CS0_PORT & CS0_LINE) {                                       // CS0 line negated
        fnActionSST25(0, 0);
        return 0xff;                                                     // chip not selected, return idle
    }
        #endif

    if (iSimType == SST25_WRITE) {
        if (ucChipCommand[iSel] == 0) {
            ucChipCommand[iSel] = ucTxByte;                              // we interpret a command
            if (ucTxByte == 0x06) {                                      // write enable
                ucStatus[iSel] |= 0x02;
                WEL[iSel] = 1;
            }
            else if (ucTxByte == AAI_WRITE) {
                ucStatus[iSel] |= 0x40;                                  // mark in AAI mode
            }
        }
        else {                                                           // in command
            switch (ucChipCommand[iSel]) {
            case 0x01:                                                   // set status register
                ucWord[iSel][0] = ucTxByte;
                break;
            case 0x9f:                                                   // read manufacturer's ID
                break;
            case 0x02:                                                   // byte program  
                if (iState[iSel] == 3) {
                    ucWord[iSel][0] = ucTxByte;                          // collect the byte to be programmed
                    iState[iSel] = 4;
                    break;
                }
            case 0x90:                                                   // read ID
            case 0xab:
            case 0x20:                                                   // sub-sector erase
            case 0x52:                                                   // half-sector erase
        #ifndef SST25_A_VERSION
            case 0xd8:                                                   // sector erase - not available on A device
        #endif
                // Else fall through
                //
            case 0x03:                                                   // read array
                if (iState[iSel] == 0) {
                    ulAccessAddress[iSel] = ucTxByte;                    // collect access address
                    ulAccessAddress[iSel] <<= 8;
                }
                else if (iState[iSel] == 1) {
                    ulAccessAddress[iSel] |= ucTxByte;                   // collect access address
                    ulAccessAddress[iSel] <<= 8;
                }
                else if (iState[iSel] == 2) {
                    ulAccessAddress[iSel] |= ucTxByte;                   // access address
                }
                iState[iSel]++;
                break; 
                
            case AAI_WRITE:                                              // AAI write sequence
                if (WEL[iSel] != 0) {                                    // write protect has been removed
                    if (WEL[iSel] == 1) {                                // collecting address
                        if (iState[iSel] == 0) {
                            ulAccessAddress[iSel] = ucTxByte;            // collect access address
                            ulAccessAddress[iSel] <<= 8;
                        }
                        else if (iState[iSel] == 1) {
                            ulAccessAddress[iSel] |= ucTxByte;           // collect access address
                            ulAccessAddress[iSel] <<= 8;
                        }
                        else if (iState[iSel] == 2) {
                            ulAccessAddress[iSel] |= (ucTxByte & ~0x01); // access address 
                            WEL[iSel] = 2;                               // address collected and in AAI sequence
                        }
                        iState[iSel]++;
                    }
                    else {
                        if (WEL[iSel] > 3) {                             // check for incorrect use
                            _EXCEPTION("SPI Flash driver didn't removed CS line before writing mode data!!!!");
                        }
                        ucWord[iSel][WEL[iSel] - 2] = ucTxByte;          // collect data pair
                        WEL[iSel]++;
                    }
                }
                break;
            
            }
        }
    }
    else if (SST25_READ == iSimType) {                                   // read
        switch (ucChipCommand[iSel]) {
        case 0x90:                                                       // read ID
        case 0xab:
            if (ulAccessAddress[iSel] == 0) {
                ulAccessAddress[iSel] = 1;
                return (MANUFACTURER_SST);                               // SST
            }
            else if (ulAccessAddress[iSel] == 0x1) {
                ulAccessAddress[iSel] = 0;
                return (MEMORY_CAPACITY);
            }
            break;
        case 0x9f:                                                       // read manufacturer's ID
        #ifndef SST25_A_VERSION                                          // only supported from B-types
            iState[iSel]++;
            if (iState[iSel] == 1) {
                return (MANUFACTURER_SST);                               // SST
            }
            else if (iState[iSel] == 2) {
                return (MEMORY_TYPE);
            }
            else if (iState[iSel] == 3) {
                iState[iSel] = 0;                                        // command complete
                return (MEMORY_CAPACITY);
            }
        #endif
            break;
        case 0x05:                                                       // read status 
            return (ucStatus[iSel]);

        default:                                                         // assume continuous array read
            {
            unsigned char ucValue = ucSST25[ulAccessAddress[iSel] + ulDeviceOffset];
        #if defined SPI_FLASH_MULTIPLE_CHIPS
            switch (iSel) {
            case 0:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_0_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 1:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_1_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 2:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_2_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            case 3:
                if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_3_SIZE) {
                    ulAccessAddress[iSel] = 0;
                }
                break;
            }
        #else
            if (++ulAccessAddress[iSel] >= SPI_DATA_FLASH_0_SIZE) {
                ulAccessAddress[iSel] = 0;
            }
        #endif
            return ucValue;
            }
            break;
        }
    }
    return 0xff;
}
    #endif                                                               // end SST25 SPI FLASH
#endif


#if defined SERIAL_INTERFACE && (NUMBER_EXTERNAL_SERIAL > 0) && defined EXT_UART_SC16IS7XX

typedef struct stSC16IS7XX
{     
	unsigned char  chip_state;
    unsigned char  RHR;
    unsigned char  THR;
    unsigned char  IER;
    unsigned char  FCR;
    unsigned char  IIR;
    unsigned char  LCR;
    unsigned char  MCR;
    unsigned char  LSR;
    unsigned char  MSR;
    unsigned char  SPR;
    unsigned char  _TCR;
    unsigned char  TLR;
    unsigned char  TXLVL;
    unsigned char  RXLVL;
    unsigned char  IODir;
    unsigned char  IOState;
    unsigned char  IPIntEna;
    unsigned char  IOControl;
    unsigned char  EFCR;
    unsigned char  DLL;
    unsigned char  DLH;
    unsigned char  EFR;
    unsigned char  XON1;
    unsigned char  XON2;
    unsigned char  XOFF1;
    unsigned char  XOFF2;

    unsigned char  ucFIFO_rx[64];
    unsigned char  ucFIFO_in_rx;
    unsigned char  ucFIFO_out_rx;
    unsigned char  ucFIFO_cnt_rx;
    unsigned char  ucFIFO_tx[64];
    unsigned char  ucFIFO_in_tx;
    unsigned char  ucFIFO_out_tx;
    unsigned char  ucFIFO_cnt_tx;
} SC16IS7XX;


static SC16IS7XX sc16IS7X_sim[NUMBER_EXTERNAL_SERIAL] = {0};


extern int fnCheckExtinterrupts(int iChannel)
{
    int iRtn = 0x0;
    if (iChannel >= NUMBER_EXTERNAL_SERIAL) {
        return 0;
    }
    if (sc16IS7X_sim[iChannel].MSR & 0x0f) {                             // modem status change
        sc16IS7X_sim[iChannel].IIR = 0x00;                               // mark that there is a modem status interrupt pending
        iRtn = 0x4;
    }
    if (sc16IS7X_sim[iChannel].ucFIFO_cnt_tx != 0) {
        sc16IS7X_sim[iChannel].IIR = 0x02;                               // mark that there is an THR interrupt pending
        iRtn |= 0x2;
    }
    if (sc16IS7X_sim[iChannel].ucFIFO_cnt_rx != 0) {
        sc16IS7X_sim[iChannel].IIR = 0x04;                               // mark that there is an RHR interrupt pending
        iRtn |= 0x1;
    }
    if (iRtn == 0) {
        sc16IS7X_sim[iChannel].IIR = 0x01;                               // no more interrupts waiting
    }
    return (iRtn & 0x5);                                                 // only return 1 when there is an rx or modem status change interrupt
}

extern unsigned long _fnSimExtSCI(QUEUE_HANDLE Channel, unsigned char ucAddress, unsigned char ucData)
{
    int iUART = ((Channel/2) + ((ucAddress >> 1) & 0x01));               // the UART channel being addressed
    if (sc16IS7X_sim[Channel].chip_state == 0) {
        sc16IS7X_sim[Channel].chip_state = 1;                            // channel reset completed
        sc16IS7X_sim[Channel].IIR = 0x01;
        sc16IS7X_sim[Channel].LCR = 0x1d;
        sc16IS7X_sim[Channel].LSR = 0x60;
        sc16IS7X_sim[Channel].TXLVL = 0x40;
        sc16IS7X_sim[Channel].MSR = 0x00;                                // all control lines initially negated
    }
    if (ucAddress & 0x80) {                                              // read
        switch ((ucAddress >> 3) & 0x0f) {
        case 0x00:                                                       // RHR - receive holding register
            if (sc16IS7X_sim[Channel].ucFIFO_cnt_rx != 0) {              // reception bytes in input FIFO
                sc16IS7X_sim[Channel].RHR = sc16IS7X_sim[Channel].ucFIFO_rx[sc16IS7X_sim[Channel].ucFIFO_out_rx++];
                sc16IS7X_sim[Channel].ucFIFO_cnt_rx--;
                if (sc16IS7X_sim[Channel].ucFIFO_out_rx >= 64) {         // handle ring buffer
                    sc16IS7X_sim[Channel].ucFIFO_out_rx = 0;
                }
            }
            return (sc16IS7X_sim[Channel].RHR);
        case 0x01:                                                       // IER - interrupt enable register
            return (sc16IS7X_sim[Channel].IER);
        case 0x02:                                                       // IIR - interrupt identification register
            return (sc16IS7X_sim[Channel].IIR);
        case 0x04:                                                       // MCR - modem control register
            return (sc16IS7X_sim[Channel].MCR);
        case 0x06:                                                       // MSR - modem status register
            {
                unsigned char ucState = sc16IS7X_sim[Channel].MSR;
                sc16IS7X_sim[Channel].MSR &= ~0x0f;                      // clear change flags in read
                return (ucState);
            }
        case 0x0f:                                                       // {55} EFCR
            return (sc16IS7X_sim[Channel].EFCR);
        default:
            _EXCEPTION("External SPI UART detected unsupported command!!");
            break;
        }
    }
    else {                                                               // write
        switch ((ucAddress >> 3) & 0x0f) {
        case 0x00:                                                       // THR - transmit holding register
            if (sc16IS7X_sim[Channel].LCR & 0x80) {
                sc16IS7X_sim[Channel].DLL = ucData;
            }
            else {
                sc16IS7X_sim[Channel].ucFIFO_tx[sc16IS7X_sim[Channel].ucFIFO_in_tx++] = ucData;
                sc16IS7X_sim[Channel].ucFIFO_cnt_tx++;
                if (sc16IS7X_sim[Channel].ucFIFO_in_tx >= 64) {          // handle ring buffer
                    sc16IS7X_sim[Channel].ucFIFO_in_tx = 0;
                }
                fnCheckExtinterrupts(Channel);
            }
            break;
        case 0x01:                                                       // IER - interrupt enable register
            if (sc16IS7X_sim[Channel].LCR & 0x80) {
                sc16IS7X_sim[Channel].DLH = ucData;
            }
            else {
                sc16IS7X_sim[Channel].IER = ucData;
            }
            break;
        case 0x02:                                                       // FCR - FIFO control register
            if (sc16IS7X_sim[Channel].LCR == 0xbf) {
                sc16IS7X_sim[Channel].EFR = ucData;
            }
            else {
                sc16IS7X_sim[Channel].FCR = ucData;
            }
            break;
        case 0x03:                                                       // LCR - line control register
            sc16IS7X_sim[Channel].LCR = ucData;
            break;
        case 0x04:                                                       // MCR - mode control register
            sc16IS7X_sim[Channel].MCR = ucData;
            break;
        case 0x0f:                                                       // {55} EFCR
            sc16IS7X_sim[Channel].EFCR = ucData;
            break;
        default:
            _EXCEPTION("External SPI UART detected unsupported command!!");
            break;
        }
    }
    return 0;
}

extern int fnRxExtSCI(int iChannel, unsigned char *ptrData, unsigned short usLength)
{
    int iHandled = 0;
    if (iChannel >= NUMBER_EXTERNAL_SERIAL) {
        return 0;
    }
    while (sc16IS7X_sim[iChannel].ucFIFO_cnt_rx < 64) {
        if (usLength == 0) {
            break;
        }
        sc16IS7X_sim[iChannel].ucFIFO_rx[sc16IS7X_sim[iChannel].ucFIFO_in_rx++] = *ptrData++;
        sc16IS7X_sim[iChannel].ucFIFO_cnt_rx++;
        iHandled++;
        if (sc16IS7X_sim[iChannel].ucFIFO_in_rx >= 64) {                 // handle ring buffer
            sc16IS7X_sim[iChannel].ucFIFO_in_rx = 0;
        }
        usLength--;
    }
    fnCheckExtinterrupts(iChannel);
    return iHandled;
}

static int fnCollectTxExtSCI(int iChannel, unsigned char *ucData)
{
    fnCheckExtinterrupts(iChannel);                                      // check for interrupts before removing the byte
    if (sc16IS7X_sim[iChannel].ucFIFO_cnt_tx != 0) {                     // transmission bytes in output FIFO
        sc16IS7X_sim[iChannel].THR = *ucData = sc16IS7X_sim[iChannel].ucFIFO_tx[sc16IS7X_sim[iChannel].ucFIFO_out_tx++];
        sc16IS7X_sim[iChannel].ucFIFO_cnt_tx--;
        if (sc16IS7X_sim[iChannel].ucFIFO_out_tx >= 64) {                // handle ring buffer
            sc16IS7X_sim[iChannel].ucFIFO_out_tx = 0;
        }
    }
    return (sc16IS7X_sim[iChannel].IER & 0x02);                          // return whether the tx interrupt is enabled
}

extern int fnExt_CTS_change(int iUART, int iChange)
{
    unsigned char ucOriginal = sc16IS7X_sim[iUART].MSR;
    if (iChange != 0) {
        sc16IS7X_sim[iUART].MSR &= ~0x10;
    }
    else {
        sc16IS7X_sim[iUART].MSR |= 0x10;
    }
    if (sc16IS7X_sim[iUART].MSR ^ ucOriginal) {
        sc16IS7X_sim[iUART].MSR |= 0x01;                                 // signal that the CTS has changed
        if (sc16IS7X_sim[iUART].IER & 0x08) {                            // interrupt enabled
             return (fnCheckExtinterrupts(iUART));
        }
    }
    return 0;
}
#endif

#if defined nRF24L01_INTERFACE || defined ENC424J600_INTERFACE


    #if defined nRF24L01_INTERFACE
    #endif
    #if defined ENC424J600_INTERFACE
    #endif
#endif