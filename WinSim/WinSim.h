/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      WinSim.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    28.04.2007 Add function fnGetEEPROMSize(void)                        {1}
    11.08.2007 Add fnInitSPI_DataFlash(), fnGetDataFlashStart(), fnGetDataFlashSize() and fnSimAT45DBXXX() {2}
    15.09.2007 Add Luminary LM3SXXXX support                             {3}
    22.09.2007 Add M5222X and M5221X support
    21.10.2007 Add SIM_TEST_ENUM and prototype fnSimulateUSB()
    02.11.2007 Add RTS control commands                                  {4}
    14.11.2007 Add port script interpreter fnDoPortSim()                 {5}
    14.11.2007 Add fnInjectSerial(), fnInjectPortValue() and fnInjectSPI() {6}
    14.11.2007 Add RX_SPI0 and RX_SPI1 defines plus fnSimulateSPIIn()    {7}
    17.11.2007 Add fnSimFPGAPeriod()                                     {8}
    15.12.2007 Add fnConfigSimI2C()                                      {9}
    23.12.2007 Extended DMA channel support                              {10}
    25.12.2007 Extended UART support to 4 channels                       {11}
    26.12.2007 Add LPC21XX support                                       {12}
    22.02.2008 Add support for SPI FLASH STM25P64                        {13}
    18.03.2008 Remove chip select parameter from fnSimAT45DBXXX() and fnSimSTM25Pxxx {14}
    28.03.2008 Add INPUT_TOGGLE_NEG and INPUT_TOGGLE_NEG                 {15}
    28.06.2008 Add fnSimSST25()                                          {16}
    16.07.2008 Add USB_INT                                               {17}
    19.07.2008 Add parameters to fnInjectSerial()                        {18}
    19.07.2008 Add fnInjectUSB()                                         {19}
    19.07.2008 Add break condition and CTS change simulation events      {20}
    13.08.2008 Add M5225X support                                        {21}
    19.09.2008 Add USB host support                                      {22}
    08.12.2008 Add file string parameter to fnDoEthereal() and fnDoPortSim {23}
    17.12.2008 Remove PORTS_AVAILABLE and PORT_WIDTH defines to processor files
    18.01.2009 Add WAIT_WHILE_BUSY                                       {24}
    27.01.2009 Add parameter to fnGetFileSystemStart() and fnGetFlashSize() {25}
    30.01.2009 Add I2C_INT2                                              {26}
    10.02.2009 Add iForce parameter to fnSimulateEthernetIn()            {27}
    03.03.2009 Add iForce parameter to fnPortChanges()                   {28}
    03.03.2009 Add pseudo interrupt                                      {29}
    24.03.2009 Further extended DMA channel support                      {30}
    17.08.2009 Add SSC interrupt flags and fnLogSSC0()                   {31}
    22.10.2009 Add _fnSimExtSCI() and  fnLogExtTxX()                     {32}
    23.10.2009 Add CHANNEL_X_EXT_SERIAL_INT interrupt flags              {33}
    23.10.2009 Add new list of extended action flags                     {34}
    03.11.2009 Add fnGetBatteryRAMContent() and fnPutBatteryRAMContent() {35}
    11.12.2009 Add SD-card                                               {36}
    18.09.2010 Add fnSaveUserData() to allow users to save data to disk  {37}
    31.07.2011 Extend UART flags to include 6 internal UARTs             {38}
    27.08.2011 Add fnInitI2C_EEPROM(), fnGetI2CEEPROMStart() and fnGetI2CEEPROMSize() {39}
    02.10.2011 Add fnInitExtFlash(), fnGetExtFlashStart() and fnGetExtFlashSize() {40}
    03.12.2011 Add channel number to fnSimCAN() and fnGetCANOwner()      {41}
    24.12.2011 Add additional CAN simulation flags                       {42}
    20.02.2012 Add SD card sizes                                         {43}
    08.03.2012 Add SD card state defines                                 {44}
    20.03.2012 Add fnSound()                                             {45}
    23.02.2013 Add USBHS_INT                                             {46}
    24.02.2014 Add USB_SIM_SOF                                           {47}
    28.02.2014 Add token parameter to fnLogUSB()                         {48}
    21.11.2015 Add fnGetSimDiskData()                                    {49}
    02.09.2016 Add TOGGLE_INPUT_POS, TOGGLE_INPUT_ANALOG and INPUT_TOGGLE_POS {50}
    24.12.2016 Add SIM_I2C_OUT and fnInjectI2C()                         {51}
    28.02.2017 Add UARTs 6 and 7                                         {52}

*/
 
#include <stdlib.h>	
#include <stdio.h>
#include <string.h>


#define MAX_ETHERNET_BUFFER  1514

#define INITIALISE_OP_SYSTEM 0
#define TICK_CALL            1
#define RX_COM0              2
#define RX_COM1              3
#define RX_COM2              4
#define RX_COM3              5                                           // {11}
#define RX_COM4              6
#define RX_COM5              7 
#define RX_ETHERNET          8
#define EXITING_CALL         9
#define KEY_CHANGE           10
#define INPUT_TOGGLE         11
#define INITIALISE_OP_SYS_2  12 
#define RX_SPI0              13                                          // {7}
#define RX_SPI1              14
#define INPUT_TOGGLE_NEG     15                                          // {15}
#define RX_EXT_COM0          16
#define RX_EXT_COM1          17
#define RX_EXT_COM2          18
#define RX_EXT_COM3          19

#define SIM_TEST_RX_0        50
#define SIM_TEST_RX_1        51
#define SIM_TEST_RX_2        52
#define SIM_TEST_RX_3        53                                          // {11}
#define SIM_TEST_RX_4        54
#define SIM_TEST_RX_5        55
#define SIM_TEST_EXT_RX_0    56
#define SIM_TEST_EXT_RX_1    57
#define SIM_TEST_EXT_RX_2    58
#define SIM_TEST_EXT_RX_3    59

#define SIM_I2C_OUT          65                                          // {51}
#define SIM_I2C_OUT_REPEATED 66

#define SIM_TEST_ENUM        80
#define SIM_TEST_DISCONNECT  81
#define SIM_USB_OUT          82
#define SIM_TEST_LOWSPEED_DEVICE   83                                    // {22}
#define SIM_TEST_FULLSPEED_DEVICE  84

#define MODEM_COM_0          100                                         // {4}
#define MODEM_COM_1          101
#define MODEM_COM_2          102
#define MODEM_COM_3          103                                         // {11}
#define MODEM_COM_4          104
#define MODEM_COM_5          105
#define MODEM_COM_6          106                                         // {52}
#define MODEM_COM_7          107
#define MODEM_EXT_COM_0      108                                         // {4}
#define MODEM_EXT_COM_1      109
#define MODEM_EXT_COM_2      110
#define MODEM_EXT_COM_3      111 

#define SIM_UART_BREAK       112                                         // {20}
#define SIM_UART_CTS         113

#define INPUT_TOGGLE_POS     200                                         // {50}
#define INPUT_TOGGLE_NEG_ANALOG  201
#define INPUT_TOGGLE_POS_ANALOG  202
#define INPUT_TOGGLE_ANALOG      203

#define USB_SETUP_FLAG       0x80000000                                  // {22}

#define ANALOGUE_SWITCH_INPUT 0x80000000
#define POSITIVE_SWITCH_INPUT 0x40000000
#define SWITCH_PORT_REF_MASK  ~(ANALOGUE_SWITCH_INPUT | POSITIVE_SWITCH_INPUT)

// Throughput parameters                                                 {38}
//
#define THROUGHPUT_UART0     0
#define THROUGHPUT_UART1     1
#define THROUGHPUT_UART2     2
#define THROUGHPUT_UART3     3
#define THROUGHPUT_UART4     4
#define THROUGHPUT_UART5     5
#define THROUGHPUT_UART6     6                                           // {52}
#define THROUGHPUT_UART7     7
#define THROUGHPUT_I2C0      8
#define THROUGHPUT_I2C1      9
#define THROUGHPUT_I2C2      10
#define THROUGHPUT_I2C3      11
#define THROUGHPUT_EXT_UART0 12
#define THROUGHPUT_EXT_UART1 13
#define THROUGHPUT_EXT_UART2 14
#define THROUGHPUT_EXT_UART3 15

// Do defines
//
#define OPEN_PC_COM0         1
#define SEND_PC_COM0         2
#define OPEN_PC_COM1         3
#define SEND_PC_COM1         4
#define OPEN_PC_COM2         5
#define SEND_PC_COM2         6
#define OPEN_PC_COM3         7                                           // {11}
#define SEND_PC_COM3         8
#define OPEN_PC_COM4         9                                           // {38}
#define SEND_PC_COM4         10
#define OPEN_PC_COM5         11
#define SEND_PC_COM5         12
#define OPEN_PC_COM6         13                                          // {52}
#define SEND_PC_COM6         14
#define OPEN_PC_COM7         15
#define SEND_PC_COM7         16
#define DISPLAY_PORT_CHANGE  17
#define SET_COM_BREAK_0      18
#define CLR_COM_BREAK_0      19
#define SET_COM_BREAK_1      20
#define CLR_COM_BREAK_1      21
#define SET_COM_BREAK_2      22
#define CLR_COM_BREAK_2      23
#define SET_COM_BREAK_3      24                                          // {11}
#define CLR_COM_BREAK_3      25
#define SET_COM_BREAK_4      26
#define CLR_COM_BREAK_4      27
#define SET_COM_BREAK_5      28
#define CLR_COM_BREAK_5      29
#define IP_CHANGE            30
#define MODEM_SIGNAL_CHANGE  31                                          // {4}
#define OPEN_PC_EXT_COM0     32
#define OPEN_PC_EXT_COM1     33
#define OPEN_PC_EXT_COM2     34
#define OPEN_PC_EXT_COM3     35
#define SEND_PC_EXT_COM0     36
#define SEND_PC_EXT_COM1     37
#define SEND_PC_EXT_COM2     38
#define SEND_PC_EXT_COM3     39

#define CHANNEL_0_SERIAL_INT 0x00000001
#define CHANNEL_1_SERIAL_INT 0x00000002
#define CHANNEL_2_SERIAL_INT 0x00000004
#define CHANNEL_3_SERIAL_INT 0x00000008                                  // {11}
#define CHANNEL_4_SERIAL_INT 0x00000010                                  // {38}
#define CHANNEL_5_SERIAL_INT 0x00000020
#define CHANNEL_6_SERIAL_INT 0x00000040                                  // {52}
#define CHANNEL_7_SERIAL_INT 0x00000080
#define I2C_INT0             0x00000100
#define I2C_INT1             0x00000200
#define I2C_INT2             0x00000400                                  // {26}
#define I2C_INT3             0x00000800
#define USB_INT              0x00001000                                  // {17}
#define PSEUDO_INT           0x00002000                                  // {29}
#define CHANNEL_0_SSC_INT    0x00004000                                  // {31}
#define CHANNEL_1_SSC_INT    0x00008000

#define CHANNEL_0_EXT_SERIAL_INT 0x00040000                              // {33}
#define CHANNEL_1_EXT_SERIAL_INT 0x00080000
#define CHANNEL_2_EXT_SERIAL_INT 0x00100000
#define CHANNEL_3_EXT_SERIAL_INT 0x00200000
#define USBHS_INT                0x00400000                              // {46}

#define DMA_CONTROLLER_0     0x00000001
#define DMA_CONTROLLER_1     0x00000002
#define DMA_CONTROLLER_2     0x00000004
#define DMA_CONTROLLER_3     0x00000008                                  // {10}
#define DMA_CONTROLLER_4     0x00000010
#define DMA_CONTROLLER_5     0x00000020
#define DMA_CONTROLLER_6     0x00000040
#define DMA_CONTROLLER_7     0x00000080
#define DMA_CONTROLLER_8     0x00000100
#define DMA_CONTROLLER_9     0x00000200
#define DMA_CONTROLLER_10    0x00000400
#define DMA_CONTROLLER_11    0x00000800
#define DMA_CONTROLLER_12    0x00001000
#define DMA_CONTROLLER_13    0x00002000
#define DMA_CONTROLLER_14    0x00004000
#define DMA_CONTROLLER_15    0x00008000
#define DMA_CONTROLLER_16    0x00010000
#define DMA_CONTROLLER_17    0x00020000
#define DMA_CONTROLLER_18    0x00040000
#define DMA_CONTROLLER_19    0x00080000
#define DMA_CONTROLLER_20    0x00100000                                  // {30}
#define DMA_CONTROLLER_21    0x00200000
#define DMA_CONTROLLER_22    0x00400000
#define DMA_CONTROLLER_23    0x00800000
#define DMA_CONTROLLER_24    0x01000000
#define DMA_CONTROLLER_25    0x02000000
#define DMA_CONTROLLER_26    0x04000000
#define DMA_CONTROLLER_27    0x08000000
#define DMA_CONTROLLER_28    0x10000000
#define DMA_CONTROLLER_29    0x20000000
#define DMA_CONTROLLER_30    0x40000000
#define DMA_CONTROLLER_31    0x80000000

#define SEND_COM_0           0x00000001
#define SEND_COM_1           0x00000002
#define SEND_COM_2           0x00000004
#define SEND_COM_3           0x00000008
#define SEND_COM_4           0x00000010                                  // {38}
#define SEND_COM_5           0x00000020
#define SEND_COM_6           0x00000040                                  // {52}
#define SEND_COM_7           0x00000080
#define SET_BREAK_COM_0      0x00000100
#define CLR_BREAK_COM_0      0x00000200
#define SET_BREAK_COM_1      0x00000400
#define CLR_BREAK_COM_1      0x00000800
#define SET_BREAK_COM_2      0x00001000
#define CLR_BREAK_COM_2      0x00002000
#define SET_BREAK_COM_3      0x00004000                                 // {11}
#define CLR_BREAK_COM_3      0x00008000
#define SET_BREAK_COM_4      0x00010000                                 // {38}
#define CLR_BREAK_COM_4      0x00020000
#define SET_BREAK_COM_5      0x00040000
#define CLR_BREAK_COM_5      0x00080000
#define ASSERT_RTS_COM_0     0x00100000                                 // {4}
#define NEGATE_RTS_COM_0     0x00200000
#define ASSERT_RTS_COM_1     0x00400000
#define NEGATE_RTS_COM_1     0x00800000
#define ASSERT_RTS_COM_2     0x01000000
#define NEGATE_RTS_COM_2     0x02000000
#define ASSERT_RTS_COM_3     0x04000000                                 // {11}
#define NEGATE_RTS_COM_3     0x08000000
#define ASSERT_RTS_COM_4     0x10000000                                 // {38}
#define NEGATE_RTS_COM_4     0x20000000
#define ASSERT_RTS_COM_5     0x40000000
#define NEGATE_RTS_COM_5     0x80000000

#define OPEN_COM_0           0x00000001
#define OPEN_COM_1           0x00000002
#define OPEN_COM_2           0x00000004
#define OPEN_COM_3           0x00000008                                  // {38}
#define OPEN_COM_4           0x00000010
#define OPEN_COM_5           0x00000020
#define OPEN_COM_6           0x00000040                                  // {52}
#define OPEN_COM_7           0x00000080
#define OPEN_EXT_COM_0       0x00000100                                  // {34}
#define OPEN_EXT_COM_1       0x00000200
#define OPEN_EXT_COM_2       0x00000400
#define OPEN_EXT_COM_3       0x00000800
#define SEND_EXT_COM_0       0x00001000                                  // {38} - moved to ulActions_2
#define SEND_EXT_COM_1       0x00002000
#define SEND_EXT_COM_2       0x00004000
#define SEND_EXT_COM_3       0x00008000

#define PORT_CHANGE          0x10000000
#define IP_CONFIG_CHANGED    0x20000000

#define RESTART              0x80000001
#define RESET_SIM_CARD       0x80000002
#define RESET_CARD_WATCHDOG  0x80000003
#define WAIT_WHILE_BUSY      0x80000004                                  // {24}

extern void *fnGetHeapStart(void);

extern void fnDoEthereal(int iStart, CHAR *file);                        // {23}
extern void fnDoPortSim(int iStart,  CHAR *file);                        // {23}{5}

extern void fnInjectFrame(unsigned char *ptrData, unsigned short usLength);
extern void fnInjectSerial(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber); // {6}
    #define UART_BREAK_CONDITION 0                                       // {18}
    #define UART_CTS_ACTIVATED   1
    #define UART_CTS_NEGATED     2
extern void fnInjectPortValue(int iPort, unsigned long ulMask, unsigned long ulValue); // {6}
extern void fnInjectSPI(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber); // {6}
extern void fnInjectUSB(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber); // {19}
extern void fnInjectI2C(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber, int iRepeatedStart); // {51}

extern void fnConfigSimI2C(QUEUE_HANDLE Channel, unsigned long ulSpeed); // {9}

extern void fnSetProjectDetails(signed char **);

extern void fnConfigSimSCI(QUEUE_HANDLE Channel, unsigned long ulSpeed, TTYTABLE *pars); 
extern void fnControlBreak(unsigned char ucChannel, int iOnOff);
extern void fnSendByte(unsigned char, unsigned short);                   // let simulator handle sent byte
extern unsigned long fnSimInts(char *argv[]);
extern unsigned long fnSimDMA(char *argv[]);

extern void fnSimulateRx(unsigned char *ucData, int iLen);

extern void fnSimSPITx(unsigned char uc);
extern void fnSimNextSPI(void);

extern void fnPrimeFileSystem(void);
extern void fnSaveFlashToFile(void);
extern unsigned char *fnGetFileSystemStart(int);                         // {25}
extern unsigned long fnGetFlashSize(int);                                // {25}
extern int  fnGetBatteryRAMContent(unsigned char *, unsigned long);      // {35}
extern int  fnPutBatteryRAMContent(unsigned char, unsigned long);

extern void fnLogTx0(unsigned char ucTxByte);
extern void fnLogTx1(unsigned char ucTxByte);
extern void fnLogTx2(unsigned char ucTxByte);
extern void fnLogTx3(unsigned char ucTxByte);                            // {11}
extern void fnLogTx4(unsigned char ucTxByte);                            // {38}
extern void fnLogTx5(unsigned char ucTxByte);
extern void fnLogTx6(unsigned char ucTxByte);                            // {52}
extern void fnLogTx7(unsigned char ucTxByte);
extern int  fnLogExtTx0(void);                                           // {32}
extern int  fnLogExtTx1(void);
extern int  fnLogExtTx2(void);
extern int  fnLogExtTx3(void);
extern void fnFollowOnFrame(void);
extern void fnSimPhyInt(void);

extern void fnLogSSC0(unsigned long ulTxByte, unsigned char ucWordWidth);// {31}

extern void fnSaveUserData(void);                                        // {37}

extern int iInts, iMasks;
extern unsigned char ucSimRxABuf[1536];                                  // simulate reserves space for max buffer sizes
extern unsigned char ucSimRxBBuf[1536];

extern int iDMA;

// Serial interfaces
//
extern void SCI0_Interrupt(void);                                        // Interrupt routines
extern void SCI1_Interrupt(void);
extern void SCI2_Interrupt(void);
extern void SCI3_Interrupt(void);                                        // {11}

// SPI interfaces
//
extern void fnSimulateSPIIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen); // simulation routine {7}

// I2C interface
//
extern void I2C_Interrupt(void);                                         // interrupt routine
extern void fnSimulateI2C(int iPort, unsigned char *ptrDebugIn, unsigned short usLen, int iRepeatedStart);

// UART
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen); // simulation routine
extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState);
extern void fnSimulateBreak(int iPort);                                  // {20}

extern unsigned long _fnSimExtSCI(QUEUE_HANDLE Channel, unsigned char ucAddress, unsigned char ucData); // {32}

// Special Hardware
//
extern void fnSimFPGAPeriod(void);

extern int  fnCheckRTC(void);

extern void fnSimulateKeyChange(int *intTable);
#if defined _EXCLUDE_WINDOWS_ && !defined _WINSIM_INCLUDE_
    extern "C" void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange);
#else
    extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange);
#endif
    #define CLEAR_INPUT         0x00
    #define SET_INPUT           0x01
    #define TOGGLE_INPUT        0x02
    #define TOGGLE_INPUT_NEG    0x04                                     // {15}
    #define TOGGLE_INPUT_POS    0x08                                     // {50}
    #define TOGGLE_INPUT_ANALOG 0x10                                     // {50}

extern void fnSimMatrixKB(void);

#define KEY_CHANGED       0x01
#define INPUT_CHANGED     0x02

// Ethernet interface
//
extern unsigned char *fnGetSimTxBufferAdd(void);
extern int  fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce); // {27}
extern void fnSimulateLinkUp(void);
extern void fnUpdateIPConfig(void);

// USB interface
//
#ifdef USB_INTERFACE
    extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent);
      #define USB_RESET_CMD             0x0001
      #define USB_SLEEP_CMD             0x0002
      #define USB_RESUME_CMD            0x0004
      #define USB_IN_SUCCESS            0x0008
      #define USB_DATA_ACK              0x0010
      #define USB_IN_TOKEN              0x0020
      #define USB_LOWSPEED_ATTACH_CMD   0x0040                           // {22}
      #define USB_FULLSPEED_ATTACH_CMD  0x0080
      #define USB_STALL_SUCCESS         0x0100                           // {22}
      #define USB_SOF_EVENT             0x0200
    extern void fnSimUSB(int iType, int iEndpoint, USB_HW *ptrUSB_HW);
      #define USB_SIM_TX           1
      #define USB_SIM_RESET        2
      #define USB_SIM_ENUMERATED   3
      #define USB_SIM_STALL        4
      #define USB_SIM_SUSPEND      5
      #define USB_SIM_SOF          6                                     // {47}
    extern unsigned short fnGetEndpointInfo(int iEndpoint);
    extern void fnCheckUSBOut(int iDevice, int iEndPoint);
    extern void fnLogUSB(int iEndpoint, unsigned char ucToken, unsigned short usLength, unsigned char *ptrUSBData, int iDataToggle); // {48}
#endif

// CAN interface
//
extern void fnSimCAN(int iChannel, int iBufferNumber, int iSpecial);     // {41}
  #define CAN_SIM_DEFAULT     0
  #define CAN_SIM_FREE_BUFFER 1
  #define CAN_SIM_INITIALISE  2
  #define CAN_SIM_CHECK_RX    3                                          // {42}
  #define CAN_SIM_TERMINATE   4
extern signed char fnGetCANOwner(int iChannel, int i);                   // {41}



extern void fnInitTime(char *argv[]);
extern void fnSimPorts(void);
extern int  fnSimTimers(void);
extern int  fnPortChanges(int);                                          // {28}
extern unsigned long fnGetPresentPortState(int);
extern unsigned long fnGetPresentPortDir(int);
extern unsigned long fnGetPresentPortPeriph(int);
extern void fnSetPortDetails(char *cPortDetails, int iLastPort, int iLastBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength);

extern unsigned char ucFLASH[];
extern void fnDeleteFlashSector(unsigned char *ptrSector);
extern void fnInitialiseDevice(void *);


// SD-card
//
extern unsigned char _fnSimSD_write(unsigned char);                      // {36}
extern void fnSaveSDcard(void);
extern int fnGetSimDiskData(unsigned char *ptrBuffer, unsigned char ucLUN, unsigned long ulLBA); // {49}
extern int fnPutSimDiskData(unsigned char *ptrBuffer, unsigned char ucLUN, unsigned long ulLBA); // {49}

#define SDCARD_SIZE_1G   0                                               // {43}
#define SDCARD_SIZE_2G   1
#define SDCARD_SIZE_4G   2
#define SDCARD_SIZE_8G   3
#define SDCARD_SIZE_16G  4
#define SDCARD_SIZE_32G  5
#define SDCARD_SIZE_64G  6

// I2C devices
//
unsigned char fnSimI2C_devices(unsigned char ucType, unsigned char ucData);
    #define I2C_ADDRESS     0
    #define I2C_TX_DATA     1
    #define I2C_RX_DATA     2
    #define I2C_RX_COMPLETE 3
    #define I2C_TX_COMPLETE 4

extern void fnInitI2C_EEPROM(void);                                      // {39}
extern unsigned char *fnGetI2CEEPROMStart(void);
extern unsigned long fnGetI2CEEPROMSize(void);

extern void fnInitExtFlash(void);                                        // {40}
extern unsigned char *fnGetExtFlashStart(void);
extern unsigned long fnGetExtFlashSize(void);

extern void fnInitSPI_Flash(void);
extern unsigned char fnSimM95xxx(int iSimType, unsigned char ucTxByte, unsigned long ulSSEL);
    #define M95XXX_READ     0
    #define M95XXX_WRITE    1
    #define M95XXX_CHECK_SS 2

extern void fnInitSPI_DataFlash(void);                                   // {2}
extern unsigned char *fnGetDataFlashStart(void);
extern unsigned long fnGetDataFlashSize(void);
extern unsigned char fnSimAT45DBXXX(int iSimType, unsigned char ucTxByte); // {14}
    #define AT45DBXXX_READ     0
    #define AT45DBXXX_WRITE    1
    #define AT45DBXXX_CHECK_SS 2

extern unsigned char fnSimSTM25Pxxx(int iSimType, unsigned char ucTxByte); // {13}{14}
    #define STM25PXXX_READ     0
    #define STM25PXXX_WRITE    1
    #define STM25PXXX_CHECK_SS 2

extern unsigned char fnSimSST25(int iSimType, unsigned char ucTxByte);   // {16}
    #define SST25_READ         0
    #define SST25_WRITE        1
    #define SST25_CHECK_SS     2

extern unsigned char fnSimW25Qxx(int iSimType, unsigned char ucTxByte);
    #define W25Q_READ          0
    #define W25Q_WRITE         1
    #define W25Q_CHECK_SS      2

extern unsigned char fnSimS25FL1_K(int iSimType, unsigned char ucTxByte);
    #define S25FL1_K_READ      0
    #define S25FL1_K_WRITE     1
    #define S25FL1_K_CHECK_SS  2

extern void fnEEPROM_safe(void);
extern unsigned long fnGetEEPROMSize(void);                              // {1}


#define PORT_DESC_LENGTH        20                                       // length of strings used for describing peripheral functions in simulator

// SD card
//
#if !defined _EXCLUDE_WINDOWS_
    #if defined NAND_FLASH_FAT
        #define SD_card_state(a,b)
    #else
        extern int SD_card_state(int iSetState, int iClearState);        // {44}
    #endif
#endif
#define SDCARD_REMOVED         0x00000000                                // physically not inserted
#define SDCARD_INSERTED        0x00000001                                // physically inserted
#define SDCARD_MOUNTED         0x00000004                                // SD card responding
#define SDCARD_FORMATTED_16    0x00000008                                // formatted with FAT16
#define SDCARD_FORMATTED_32    0x00000010                                // formatted with FAT32
#define SDCARD_WR_PROTECTED    0x00000020                                // SD card is write protected


#if !defined _EXCLUDE_WINDOWS_
    extern void fnSound(int iFrequency);                                 // {45}
    extern unsigned long fnRemoteSimulationInterface(int iInterfaceReference, unsigned char ucCommand, unsigned char *ptrData, unsigned short usDataLength, int iCollect);
    extern int fnCheckENC424J600(unsigned char *ucData, unsigned short usLen, int iForce);
    extern void fnSimulateEthTx(int, unsigned char *);
#endif

