/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      hardware.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    01.03.2007 fnGetFlashAdd() added
    30.03.2007 fnPutFlashAdd() added
    19.05.2007 Add FLASH protection functions fnUnprotectSector() and fnProtectSector()
    19.05.2007 Add simulation functions fnGetSectorSize() and fnIsProtected()
    10.06.2007 Add fnGetRndHW()
    11.08.2007 Add fnSPI_Flash_available() and fnConfigureRTC()
    12.09.2007 Add fnSPI_FlashExt_available
    23.09.2007 Change prototype of fnEraseFlashSector() to include a length.
               For code compatibility a length of 0 can be set which causes one sector to be erased.
    12.10.2007 Add USB interface prototypes
    22.10.2007 Add STMicroelectronics SPI FLASH parts                    {1}
    10.04.2008 Add fnSetHardwareTimer() and fnStopHW_Timer()
    23.05.2008 Add fnGetDMACount()
    02.06.2008 Add fnUpdateADC()
    28.06.2008 Add SST SPI FLASH parts                                   {2}
    27.08.2008 Modify fnTxI2C Channel parameter to QUEUE_HANDLE          {3}
    06.10.2008 Move USB simulator defines to this file                   {4}
    18.10.2008 Add USB FIFO routines                                     {5}
    13.02.2009 Add fnDelayLoop() for simple but accurate us delay loops  {6}
    14.02.2009 Change fnControlLine() and fnControlLineInterrupt() operating parameters to UART_MODE_CONFIG {7}
    26.02.2009 Add fnUARTFrameTermination()                              {8}
    03.03.2009 Add fnSciRxIdle()                                         {9}
    04.03.2009 Change fnInitialiseRND() use to allow the seed to be directly set by the routine (it can then be used by the routine to generate a further random seed) {10}
    24.03.2009 fnTxByteDMA() changed to return the transmission length possible {11}
    10.04.2009 Add fnRemainingDMA_tx() and fnAbortTxDMA()                {12}
    30.04.2009 fnEthernetEvent() made conditional on Ethernet            {13}
    01.05.2009 Add fnMassEraseFlash() for unsecuring a chip              {14}
    07.06.2009 Add start_application()                                   {15}
    22.06.2009 Add LCD simulation routines                               {16}
    17.08.2009 Add SSC interface                                         {17}
    10.09.2009 Add fnGetMultiDropByte()                                  {18}
    15.10.2009 Add AT45DB011D, AT45DB021D, STM25P128, STM25P20, STM25P10 and STM25P05 {19}
    04.11.2009 Add backlight control interface                           {20}
    01.02.2010 Add fnConfigureMulticastIPV6()                            {21}
    25.03.2010 Add fnUnhaltEndpoint()                                    {22}
    18.04.2010 Add fnAllocateUSBBuffer() and fnPrepareUSBOutData()       {23}
    10.06.2011 Add fnSetMultiDropByte()                                  {24}
    18.07.2011 Add fnClearSLCD()                                         {25}
    03.08.2011 Change return value of CollectCommand()                   {26}
    07.08.2011 Modify fnConfigureRTC() with return value                 {27}
    02.10.2011 Add STORAGE_AREA_ENTRY and ACCESS_DETAILS                 {28}
    02.10.2011 Add fnGetExtFlashAdd() and fnPutExtFlashAdd()             {29}
    10.11.2011 Add return value to fnConfigEthernet()                    {30}
    04.11.2011 Add channel parameter to fnCAN_tx() and fnCAN_get_rx()    {31}
    16.04.2012 Add fnGetExtPortDirection(), fnGetExtPortState() and fnSetI2CPort() {32}
    16.12.2012 Add fnCheckEthLinkState()                                 {33}
    10.01.2013 Add _STORAGE_NVRAM                                        {34}
    20.01.2013 Add fnWriteBytesFlashNonBlocking() and fnReadBytesFlashNonBlocking() {35}
    30.05.2013 Add fnEnableMagicFrames() and fnMagicFrame()              {36}
    23.06.2013 Add fnSetTailTagMode() and fnGetTailTagMode()             {37}
    23.06.2013 Add fnSetTailTagPort() and fnGetTailTagPort()             {38}
    14.08.2013 Add high speed USB interface prototypes                   {39}
    28.02.2014 Add fnHostEndpoint()                                      {40}
    05.03.2014 Add fnUSB_error_counters()                                {41}
    16.04.2014 Add fnModifyMulticastFilter()                             {42}
    22.07.2014 Add fnGetLowPowerMode() and fnSetLowPowerMode()           {43}
    04.10.2015 Change fnGetUSB_HW() parameter                            {44}
    23.12.2015 Add parameter to fnActivateHWEndpoint()                   {45}
    12.01.2016 Add fnSetFragmentMode()                                   {46}
    15.02.2016 Add fnDMA_BufferReset()                                   {47}
    31.01.2017 Add fnVirtualWakeupInterruptHandler()                     {48}

*/

#if !defined _HARDWARE
#define _HARDWARE


// It is expected that the hardware support delivers the following functions
//
extern void fnStartTick(void);                                           // initialise and start a TICK timer interrupt
extern void fnInitialiseRND(unsigned short *);                           // {10}
extern unsigned short fnGetRndHW(void);                                  // get a random value from the hardware
extern void fnRetriggerWatchdog(void);                                   // retrigger a hardware watchdog
extern void fnDelayLoop(unsigned long ulDelay_us);                       // {6}

// Serial interface support
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars);           // configure serial interface
extern int  fnTxByte(QUEUE_HANDLE channel, unsigned char ucTxByte);      // transmit a serial byte
extern QUEUE_TRANSFER fnTxByteDMA(QUEUE_HANDLE ucChannel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length); // {11} transmit serial bytes per DMA
extern QUEUE_TRANSFER fnRemainingDMA_tx(QUEUE_HANDLE channel);           // {12}
extern QUEUE_TRANSFER fnAbortTxDMA(QUEUE_HANDLE channel, QUEQUE *ptrQueue); // {12}
extern void fnPrepareRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length);
extern QUEUE_TRANSFER fnGetDMACount(QUEUE_HANDLE channel, QUEUE_TRANSFER max_count);
extern void fnClearTxInt(QUEUE_HANDLE channel);                          // clear serial interface Tx interrupt
extern void fnStartBreak(QUEUE_HANDLE channel);                          // start a break transmission
extern void fnStopBreak(QUEUE_HANDLE channel);                           // stop a break transmission
extern void fnRxOn(QUEUE_HANDLE Channel);                                // enable rx
extern void fnRxOff(QUEUE_HANDLE Channel);                               // disable rx
extern void fnTxOn(QUEUE_HANDLE Channel);                                // enable tx
extern void fnTxOff(QUEUE_HANDLE Channel);                               // disable tx
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode); // {7} modify state of control lines
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode); // {7}
extern void fnUARTFrameTermination(QUEUE_HANDLE channel);                // {8} - to be supplied by user when used
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE channel);           // {18} - retrieve extended byte in multi-drop/9-bit mode
extern void fnSetMultiDropByte(QUEUE_HANDLE channel, unsigned char ucMSB); // {24} - prepare extended byte in multi-drop/9-bit mode

// Synchronous Serial Communication                                      // {17}
//
extern void fnConfigSSC(QUEUE_HANDLE Channel, SSCTABLE *pars);           // configure SSC interface
extern void fnClearSSCTxInt(QUEUE_HANDLE channel);                       // clear SSC Tx interrupt
extern int  fnTxSSCword(QUEUE_HANDLE channel, unsigned long ulTxDate);   // transmit a data word
extern void fnSSCRxWord(unsigned long ulWord, QUEUE_HANDLE Channel);     // receive data word/frame
extern void fnSSC_RxOn(QUEUE_HANDLE Channel);                            // enable rx
extern void fnSSC_RxOff(QUEUE_HANDLE Channel);                           // disable rx
extern void fnSSC_TxOn(QUEUE_HANDLE Channel);                            // enable tx
extern void fnSSC_TxOff(QUEUE_HANDLE Channel);                           // disable tx
extern void fnSSCTxWord(QUEUE_HANDLE Channel);                           // send next queued data for this interface
extern QUEUE_TRANSFER fnTxSCCWordDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length);
extern void fnPrepareSSCRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length);

// USB interface support
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars);           // configure USB interface
#if defined USB_INTERFACE
    extern int  fnGetUSB_HW(int iEndpoint, USB_HW **ptrHW);              // {44} pass pointer to pointer rather than pointer
        #define ENDPOINT_FREE        0
        #define ENDPOINT_NOT_ACTIVE  1
        #define ENDPOINT_NOT_FREE    2
    extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength, USB_ENDPOINT *ptrEndpoint); // {45}
    extern int  fnConsumeUSB_out(unsigned char ucEndpointRef);
        #define USB_BUFFER_FREED     0                                   // return values
        #define USB_BUFFER_NO_DATA   1
        #define USB_BUFFER_BLOCKED   2

    extern void fnSendUSB_FIFO(unsigned char *ptrData, unsigned short usLength, int iEndpoint, USB_HW *ptrUSB_HW); // {5}
    extern unsigned char *fnReadUSB_FIFO(volatile unsigned char *fifo_addr, unsigned short length, unsigned short Offset); // {5}

    extern void fnUnhaltEndpoint(unsigned char ucEndpoint);              // {22}

    extern int fnAllocateUSBBuffer(QUEUE_HANDLE DriverID, USB_ENDPOINT *usb_endpoint_queue, QUEUE_DIMENSIONS *ptrQueueDimensions); // {23}
    extern QUEUE_TRANSFER fnPrepareUSBOutData(USBQUE *ptrQueue, unsigned short usLength, int iEndpoint, USB_HW *ptrUSB_HW); // {23}

    #if defined USB_HOST_SUPPORT
        extern void fnResetDataToggle(int iEndpoint, USB_HW *ptrUSB_HW);
    #endif

    #if defined _WINDOWS                                                 // {4}
        #define _SIM_USB(x,y,z)  fnSimUSB(x,y,z)
    #else
        #define _SIM_USB(x,y,z)
    #endif
    #if defined USB_HS_INTERFACE                                         // {39}
        extern void fnTxUSBHS(unsigned char *pData, unsigned short usLen, int iEndpoint, USB_HW *ptrUSB_HW);
    #endif
    extern int fnHostEndpoint(unsigned char ucEndpoint, int iCommand, int iEvent); // {40}
        #define IN_POLLING 0
    extern unsigned long fnUSB_error_counters(int iValue);
        #define USB_CRC_5_ERROR_COUNTER     0
        #define USB_CRC_16_ERROR_COUNTER    1
        #define USB_ERRORS_RESET           -1
#endif

// I2C interface support
//
extern void fnConfigI2C(I2CTABLE *pars);                                 // configure I2C interface
extern void fnTxI2C(I2CQue *ptI2CQue, QUEUE_HANDLE Channel);             // start transmission of an I2C buffer {3}

// I2C device simulation                                                 {32}
//
extern unsigned long fnGetExtPortDirection(int iExtPortReference);
extern unsigned long fnGetExtPortState(int iExtPortReference);
extern void fnSetI2CPort(int port, int iChange, unsigned long bit);

// Ethernet LAN support
//
extern int fnConfigEthernet(ETHTABLE *);                                 // {30}
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put);
extern int fnWaitTxFree(void);
extern void fnPutInBuffer(unsigned char *ptrOut, unsigned char *ptrIn, QUEUE_TRANSFER nr_of_bytes);
extern int fnGetQuantityRxBuf(void);
extern void fnFreeEthernetBuffer(int);
extern unsigned char *fnGetRxBufferAdd(int);
extern unsigned char *fnGetTxBufferAdd(int);
#if defined ETH_INTERFACE                                                // {13}
    extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame);
//  extern void fnConfigureMulticastIPV6(void);                          // {21}
    extern void fnEnableMagicFrames(int iOnOff);                         // {36}
    extern void fnMagicFrame(unsigned char ucType, unsigned char usOptionalDate[32]); // {36}
        #define MAGIC_RESET    0x01
#endif
extern void fnCheckEthLinkState(void);                                   // {33}
extern void fnSetTailTagMode(int iPort);                                 // {37} pass 0 to disable tail tagging or non-0 to enable. When simulating, the value controls the port that ethernet frames are received on
extern int  fnGetTailTagMode(void);                                      // {37}
extern void fnSetTailTagPort(unsigned char ucPort);                      // {38} set the port(s) for Ethernet frame transmission
extern unsigned char fnGetTailTagPort(void);                             // {38}
    #define ETH_TX_PORT_SWITCH     0                                     // transmissions are sent in switch mode
    #define ETH_TX_PORT_0          1                                     // transmissions are sent to port 0 only
    #define ETH_TX_PORT_1          2                                     // transmissions are sent to port 1 only
    #define ETH_TX_PORT_BOTH       3                                     // transmissions are sent to both ports
extern void fnModifyMulticastFilter(QUEUE_TRANSFER action, unsigned char *ptrIP); // {42}
extern void fnSetFragmentMode(int iMode);                                // {46}

// SPI interface support
//
extern void fnConfigSPI(SPITABLE *pars);
extern void fnSendSPIMessage(unsigned char *ptrData, QUEUE_TRANSFER Length);
 #define PREPARE_READ          0x00
 #define PREPARE_PAGE_WRITE    PREPARE_READ
 #define TERMINATE_WRITE       0x01

extern int  fnTxSPIByte(QUEUE_HANDLE channel, unsigned char ucTxByte);
extern void fnSPIRxByte(unsigned char ch, QUEUE_HANDLE Channel );
extern void fnSPITxByte(QUEUE_HANDLE Channel );

extern void fnGetSPI_bytes(unsigned char *, MAX_FILE_LENGTH);

// CAN interface support
//
extern void fnInitCAN(CANTABLE *pars);
extern void fnConfigCAN(QUEUE_HANDLE DriverID, CANTABLE *pars);
extern unsigned char fnCAN_tx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter); // {31}
extern unsigned char fnCAN_get_rx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter); // {31}

// Interrupt configuration support
//
extern void fnConfigureInterrupt(void *interrupt_settings);

// RTC support
//
extern int fnConfigureRTC(void *ptrSettings);                            // {27}
  #define WAIT_STABILISING_DELAY   1

// ADC
//
extern void fnUpdateADC(int iChannel);

// Hardware Timers
//
extern DELAY_LIMIT fnSetHardwareTimer(DELAY_LIMIT *hw_delay);            // start the hardware timer with this ms delay value
extern DELAY_LIMIT fnStopHW_Timer(void);                                 // stop the hardware timer

// Buffer DMA                             
//
extern void fnDMA_BufferReset(int iChannel, int iAction);                // {47}
    #define DMA_BUFFER_RESET     0                                       // disable DMA and reset the buffer (but don't re-enable yet)
    #define DMA_BUFFER_START     1                                       // start DMA operation (must have been configured previously)
    #define DMA_BUFFER_RESTART   2                                       // reset buffer and restart immediately

// Low power
//
extern int  fnGetLowPowerMode(void);                                     // {43}
extern void fnSetLowPowerMode(int);
#if defined LOW_POWER_CYCLING_MODE
    extern int fnVirtualWakeupInterruptHandler(int iDeepSleep);          // {48}
    extern int iLowPowerLoopMode;
    #define LOW_POWER_CYCLING_DISABLED 0
    #define LOW_POWER_CYCLING_PAUSED   1
    #define LOW_POWER_CYCLING_ENABLED  2
#endif

extern void fnResetBoard(void);
#if !defined start_application
    extern void start_application(unsigned long);                        // {15} assembler jump to application
#endif

// These functions can be called by hardware routines
//
extern void fnSciRxByte( unsigned char ch, QUEUE_HANDLE Channel );       // put a received byte in input queue
extern void fnSciTxByte( QUEUE_HANDLE Channel );                         // transmit next byte from queue
extern void fnSciRxMsg( QUEUE_HANDLE Channel );                          // terminate a rx message in buffer
extern int  fnSciRxIdle( QUEUE_HANDLE Channel );                         // {9} rx idle line detected for defined period of time

// FLASH support
//
extern int  fnEraseFlashSector(MEMORY_RANGE_POINTER ptrSector, MAX_FILE_LENGTH Length);
extern int  fnWriteBytesFlash(MEMORY_RANGE_POINTER ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length);
extern int  fnReadBytesFlashNonBlocking(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size); // {35} only available when using MANAGED_FILE_READ - non-blocking read that can return MEDIA_BUSY
extern int  fnWriteBytesFlashNonBlocking(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length); // {35} only available when using MANAGED_FILE_WRITE - non-blocking write that can return MEDIA_BUSY
extern void fnProtectFlash(unsigned char *ptrSector, unsigned char ucProtection);
  #define PROTECT_SECTOR   1
  #define UNPROTECT_SECTOR 0
extern int fnMassEraseFlash(void);                                       // {14}

#if defined _WINDOWS
    extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd);           // routine to map real hardware address to simulated FLASH
    extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd);           // inverse of fnGetFlashAdd
    extern int  fnIsProtected(unsigned char *ptrSectorPointer);
    extern const unsigned long fnGetSectorSize(unsigned char *ptrSector);
#else
    #define fnGetFlashAdd(x) x                                           // macro to map to direct FLASH memory access -dummy on target
    #define fnPutFlashAdd(x) x                                           // macro to map to direct FLASH memory access - dummy on target
#endif
extern void fnUnprotectSector(volatile unsigned short *usSectorPointer);
extern void fnProtectSector(volatile unsigned short *usSectorPointer);

extern unsigned char *fnGetExtFlashAdd(unsigned char *ucAdd);            // routine to map real external hardware address to simulated external FLASH
extern unsigned char *fnPutExtFlashAdd(unsigned char *ucAdd);            // inverse of fnGetExtFlashAdd

extern unsigned char fnSPI_Flash_available(void);
extern unsigned char fnSPI_FlashExt_available(int iExtension);
    #define NO_SPI_FLASH_AVAILABLE    0                                  // No SPI FLASH detected
    #define AT45DB011B                1                                  // B-version of this device has been detected
    #define AT45DB021B                2                                  // B-version of this device has been detected
    #define AT45DB041B                3                                  // B-version of this device has been detected
    #define AT45DB081B                4                                  // B-version of this device has been detected
    #define AT45DB011D                5                                  // D-version of this device has been detected {19}
    #define AT45DB021D                6                                  // D-version of this device has been detected {19}
    #define AT45DB041D                7                                  // D-version of this device has been detected
    #define AT45DB081D                8                                  // D-version of this device has been detected
    #define AT45DB161D                9                                  // D-version of this device has been detected
    #define AT45DB321C                10                                 // C-version of this device has been detected
    #define AT45DB321D                11                                 // D-version of this device has been detected
    #define AT45DB641D                12                                 // D-version of this device has been detected

    #define STM25P05                  1                                  // STMicroelectronics SPI FLASH parts {1}{19}
    #define STM25P10                  2
    #define STM25P20                  3
    #define STM25P40                  4
    #define STM25P80                  5
    #define STM25P16                  6
    #define STM25P32                  7
    #define STM25P64                  8
    #define STM25P128                 9 

    #define SST25VF010A               1
    #define SST25LF020A               2
    #define SST25LF040A               3
    #define SST25VF040B               4                                  // SST parts {2}
    #define SST25VF080B               5
    #define SST25VF016B               6
    #define SST25VF032B               7

    #define W25Q10                    1                                  // Winbond W25Q SPI Flash types {25}
    #define W25Q20                    2
    #define W25Q40                    3
    #define W25Q80                    4
    #define W25Q16                    5
    #define W25Q32                    6
    #define W25Q64                    7
    #define W25Q128                   8

    #define S25FL116K                 1                                  // Spansion S25FL1-K
    #define S25FL132K                 2
    #define S25FL164K                 3


typedef struct stSTORAGE_AREA_ENTRY                                      // {28}
{
    void          *ptrNext;                                              // pointer to the next storage type available, or zero at end of list
    unsigned char *ptrMemoryStart;                                       // pointer to the first address in the storage area
    unsigned char *ptrMemoryEnd;                                         // pointer to last location at the end of the storage area (total area when multiple devices) [the reason why the address of last byte is used - and not following address - is to allow the storage area to end at the last possible address in 32 bit memory space; otherwise there would be an overflow to zero]
    unsigned char ucStorageType;                                         // the storage type, such as internal Flash, SPI Flash etc.
    unsigned char ucDeviceCount;                                         // the number of individual devices the storage area is made up of (it is assumed that all devices are the same type and size)
} STORAGE_AREA_ENTRY;

#define _STORAGE_INVALID         0x80                                    // storage device types
#define _STORAGE_SPI_EEPROM      0x41
#define _STORAGE_I2C_EEPROM      0x42
#define _STORAGE_PARALLEL_NVRAM  0x43
#define _STORAGE_INTERNAL_FLASH  0x01
#define _STORAGE_SPI_FLASH       0x02
#define _STORAGE_PARALLEL_FLASH  0x03
#define _STORAGE_NVRAM           0x04                                    // {34}
#define _STORAGE_PROGRAM         0x20

// Bit information
//
#define EEPROM_CHARACTERISTICS   0x40


typedef struct stACCESS_DETAILS
{
    unsigned long   ulOffset;                                            // location offset referenced to the start of the storage device (individual device when multiple devices in the storage area)
    MAX_FILE_LENGTH BlockLength;                                         // the block length that is to be read, written or erased
    unsigned char   ucDeviceNumber;                                      // the device number, when multiple devices in the storage area
} ACCESS_DETAILS;

extern STORAGE_AREA_ENTRY *UserStorageListPtr;

extern unsigned char fnGetStorageType(unsigned char *memory_pointer, ACCESS_DETAILS *ptrAccessDetails);


// Parameter support
//
extern void fnGetParsFile(MEMORY_RANGE_POINTER ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size);
extern unsigned char fnGetValidPars(unsigned char ucValid);
   #define PRESENT_VALID_BLOCK 1
   #define PRESENT_TEMP_BLOCK  0
extern int  fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength);
extern int  fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength);
   #define TEMP_PARS 0x80
   #define BLOCK_INVALID 0x40

extern void fnDoLowPower(void);

extern void fnUserHWInit(void);                                          // user must supply this - even if only empty

// SLCD
//
extern void fnClearSLCD(void);                                           // {25}

// LCD
//
#if defined _WINDOWS                                                     // {16} LCD simulation interface
    extern int CollectCommand(int bRS, unsigned long ulByte);            // {26} return value changed from void to int
    extern unsigned long ReadDisplay(int bRS);
    extern void fnSimBackLights(int iLightState, int iPercentage);       // {20}
#endif



#define REMOTE_RF_INTERFACE  1
    #define REMOTE_RF_DISABLE_RX_TX       1
    #define REMOTE_RF_ENABLE_RX_TX        2
    #define REMOTE_RF_W_COMMAND           3
    #define REMOTE_RF_W_REGISTER          4
    #define REMOTE_RF_W_REGISTER_VALUE    5

    #define REMOTE_RF_IRQ_EVENT           1                              // interrupt has fired - 10 bytes of register content included
#define REMOTE_ETH_INTERFACE 2
    #define REMOTE_ETH_CMD_WITHOUT_DATA   29
    #define REMOTE_ETH_CMD                30
    #define REMOTE_ETH_DATA               31
    #define REMOTE_ETH_DATA_BUFFER        32
    #define REMOTE_ETH_DATA_RX            33

    #define REMOTE_ETH_RX_DATA            1
    #define REMOTE_ETH_INTERRUPT          2
#endif


