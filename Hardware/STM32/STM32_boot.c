/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        STM32_boot.c
    Project:     Single Chip Embedded Internet - boot loader hardware support
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    06.03.2012 Add start_application()                                   {1}
    12.12.2012 Correct flash erase routine for F1 parts                  {2}
    02.10.2013 Correct STM32F1xx flash control                           {3}
    20.01.2017 Add 2MByte Flash support                                  {4}
    18.06.2018 Change uMemset() to match memset() parameters             {5}
    01.11.2018 User flash driver instead of local code                   {6}
    01.11.2018 Use an include file for the clock configuration           {7}
    28.11.2018 Add automatic flash option configuration option           {8}


*/

#if defined _STM32

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */


#if defined _WINDOWS
    #include "config.h"
    #define INITHW  extern
    #define START_CODE 0
    #if (defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES)
        extern int iFetchingInternalMemory = 0;
    #endif
    extern unsigned char vector_ram[sizeof(VECTOR_TABLE)];               // vector table in simulated RAM (long word aligned)
#else
    #define STM32_LowLevelInit main
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #include "config.h"
    #if defined _COMPILE_KEIL
        extern void __main(void);                                        // Keil library initialisation routine
        #define START_CODE __main
    #elif defined COMPILE_IAR
        extern void __iar_program_start(void);                           // IAR library initialisation routine
        #define START_CODE __iar_program_start
      //#define _main main
    #elif defined ROWLEY && !defined ROWLEY_2                            // if project uses Rowley < V1.7 build 17
        #define START_CODE _main2
    #else                                                                // general GCC
        #define START_CODE main
    #endif
    #define SIM_DMA(x)
#endif

#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined SUPPORT_PROGRAM_ONCE // {6}
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */
    #include "stm32_FLASH.h"                                             // include FLASH driver code
#endif


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);
}


#if !defined _COMPILE_KEIL                                               // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #if !defined _WINDOWS
        #if defined ARM_MATH_CM0PLUS                                     // {67} cortex-M0+ assembler code
    asm(" ldr r1, [r0,#0]");                                             // get the stack pointer value from the program's reset vector
    asm(" mov sp, r1");                                                  // copy the value to the stack pointer
    asm(" ldr r0, [r0,#4]");                                             // get the program counter value from the program's reset vector
    asm(" blx r0");                                                      // jump to the start address
        #else                                                            // cortex-M3/M4/M7 assembler code
    asm(" ldr sp, [r0,#0]");                                             // load the stack pointer value from the program's reset vector
    asm(" ldr pc, [r0,#4]");                                             // load the program counter value from the program's reset vector to cause operation to continue from there
        #endif
    #endif
}
#endif

// The boot loader doesn't use interrupts so these routines are dummy
//
extern void uDisable_Interrupt(void)
{
}

extern void uEnable_Interrupt(void)
{
}


// CRC-16 routine
//
extern unsigned short fnCRC16(unsigned short usCRC, unsigned char *ptrInput, unsigned short usBlockSize)
{
    while (usBlockSize--) {
        usCRC = (unsigned char)(usCRC >> 8) | (usCRC << 8);
        usCRC ^= *ptrInput++;
        usCRC ^= (unsigned char)(usCRC & 0xff) >> 4;
        usCRC ^= (usCRC << 8) << 4;
        usCRC ^= ((usCRC & 0xff) << 4) << 1;
    }
    return usCRC;
}


#if defined SPI_SW_UPLOAD
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_STM32_atmel.h"
            #include "spi_flash_STM32_stmicro.h"
            #include "spi_flash_STM32_sst25.h"
        #undef _SPI_DEFINES
    #endif

    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        unsigned long ulChipSelect[SPI_FLASH_DEVICE_COUNT] = {
            CS0_LINE,
            CS1_LINE                                                     // at least 2 expected when multiple devices are defined
        #ifdef CS2_LINE
            ,CS2_LINE
            #ifdef CS3_LINE
            ,CS3_LINE
            #endif
        #endif
        };
        #define EXTENDED_CS , &iChipSelect
        #define _EXTENDED_CS  ptrAccessDetails->ucDeviceNumber,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent SPI Flash driver code
        #include "spi_flash_STM32_atmel.h"
        #include "spi_flash_STM32_stmicro.h"
        #include "spi_flash_STM32_sst25.h"
    #undef _SPI_FLASH_INTERFACE
#endif


// memcpy implementation
//
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr1 = (unsigned char *)ptrTo;
    unsigned char *ptr2 = (unsigned char *)ptrFrom;

    while (Size-- != 0) {
        *ptr1++ = *ptr2++;
    }

    return buffer;
}


#if defined SPI_SW_UPLOAD
// This routine reads data from the defined device into a buffer. The access details inform of the length to be read (already limited to maximum possible length for the device)
// as well as the address in the specific device
//
static void fnReadSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    #if !defined SPI_FLASH_SST25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(ptrAccessDetails->ulOffset - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif

    #if defined SPI_FLASH_ST
    fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #elif defined SPI_FLASH_SST25
    fnSPI_command(READ_DATA_BYTES, ptrAccessDetails->ulOffset, _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #else                                                                // ATMEL
        #if SPI_FLASH_PAGE_LENGTH >= 1024
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #elif SPI_FLASH_PAGE_LENGTH >= 512
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #endif
    #endif
}

// The routine is used to delete an area in SPI Flash, whereby the caller has set the address to the start of a page and limited the erase to a single storage area and device
//
static MAX_FILE_LENGTH fnDeleteSPI(ACCESS_DETAILS *ptrAccessDetails)
{
    MAX_FILE_LENGTH BlockLength = SPI_FLASH_PAGE_LENGTH;
    #if !defined SPI_FLASH_ST
    unsigned char   ucCommand;
    #endif
    #if !defined SPI_FLASH_SST25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    #endif
    #if defined SPI_FLASH_ST
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // enable the write
        #ifdef SPI_DATA_FLASH
    fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
    BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
        #else
    fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
    BlockLength = SPI_FLASH_SECTOR_LENGTH;
        #endif
    #elif defined SPI_FLASH_SST25
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow byte programming
        #ifndef SST25_A_VERSION
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_SECTOR_LENGTH - 1)) == 0)) { // if a complete 64k sector can be deleted
        ucCommand = SECTOR_ERASE;                                        // delete block of 64k
        BlockLength = SPI_FLASH_SECTOR_LENGTH;
    }
    else 
        #endif
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
        ucCommand = HALF_SECTOR_ERASE;                                   // delete block of 32k
        BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
    }
    else {
        ucCommand = SUB_SECTOR_ERASE;                                    // delete smallest sector of 4k
        BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
    }
    fnSPI_command(ucCommand, ptrAccessDetails->ulOffset, _EXTENDED_CS 0, 0);    
    #else                                                                // ATMEL
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber % 8 == 0)) { // if delete range corresponds to a block, use faster block delete
        BlockLength = SPI_FLASH_BLOCK_LENGTH;
        ucCommand = BLOCK_ERASE;
    }
    else {
        BlockLength = SPI_FLASH_PAGE_LENGTH;
        ucCommand = PAGE_ERASE;
    }
    fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);           // delete appropriate page/block
    #endif
    return (BlockLength);
}

#endif

extern int uFileErase(unsigned char *ptrFile, MAX_FILE_LENGTH FileLength)
{
    return fnEraseFlashSector(ptrFile, FileLength);
}


extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#if defined SPI_SW_UPLOAD
    if (ParLocation >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // if in SPI flash
        ACCESS_DETAILS AccessDetails;
        AccessDetails.ulOffset = (unsigned long)(ParLocation - (FLASH_START_ADDRESS + SIZE_OF_FLASH)); // offset in SPI flash
        AccessDetails.BlockLength = Size;
        fnReadSPI(&AccessDetails, ptrValue);                             // read from the SPI device
        return;
    }
#endif
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // directly copy memory since this must be a pointer to code (embedded file)
}


#ifdef SPI_SW_UPLOAD
extern int fnConfigSPIFileSystem(void)
{
    POWER_UP_SPI_FLASH_INTERFACE();
    CONFIGURE_SPI_FLASH_INTERFACE();
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code to detect the SPI Flash devices
        #include "spi_flash_STM32_atmel.h"
        #include "spi_flash_STM32_stmicro.h"
        #include "spi_flash_STM32_sst25.h"
    #undef _CHECK_SPI_CHIPS
    return (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE);
}
#endif












#if !defined _MINIMUM_IRQ_INITIALISATION
// Serious error interrupts
//
static void irq_hard_fault(void)
{
}

static void irq_memory_man(void)
{
}

static void irq_bus_fault(void)
{
}

static void irq_usage_fault(void)
{
}

static void irq_debug_monitor(void)
{
}

static void irq_NMI(void)
{
}

static void irq_pend_sv(void)
{
}

static void irq_SVCall(void)
{
}
#endif

// Default handler to indicate processor input from unconfigured source
//
static void irq_default(void)
{
}

// Delay loop for simple but accurate short delays (eg. for stabilisation delays)
//
extern void fnDelayLoop(unsigned long ulDelay_us)
{
    #define LOOP_FACTOR  14000000                                        // tuned
    volatile unsigned long ulDelay = ((SYSCLK/LOOP_FACTOR) * ulDelay_us);
    while (ulDelay-- != 0) {}                                            // simple loop tuned to perform us timing
}


#if defined _GNU
    extern const RESET_VECTOR reset_vect;                                // force GCC to link the reset table
    volatile void *ptrTest1;
#endif

#if defined (_GNU)
extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

extern void __init_gnu_data(void)
{
    unsigned char *ptrData = &__data_start__;
    const unsigned char *ptrFlash = &__data_load_start__;
    unsigned long ulInitDataLength = (&__data_end__ - &__data_start__);
    while (ulInitDataLength--) {                                         // initialise data
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__text_start__;
    ptrFlash = &__text_load_start__;
    if (ptrData != ptrFlash) {
        ulInitDataLength = (&__text_end__ - &__text_start__);
        while (ulInitDataLength--) {                                     // initialise text
            *ptrData++ = *ptrFlash++;
        }
    }

    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }

    ptrTest1 = (void *)&reset_vect;                                      // force GCC to link the reset table
}
#endif


// Perform very low level initialisation - called by the start up code
//
extern int STM32_LowLevelInit(void)
{
#if !defined _MINIMUM_IRQ_INITIALISATION
    void ( **processor_ints )( void );
#endif
    VECTOR_TABLE *ptrVect;
#include "stm32_CLOCK.h"                                                 // {7} clock configuration
#if defined FLASH_OPTION_SETTING && defined FLASH_OPTCR                  // {8} program a flash configuration option (this is only performed when the setting causes a change to that already programmed)
    #if defined FLASH_OPTION_SETTING_1 && defined FLASH_OPTCR1
    fnSetFlashOption(FLASH_OPTION_SETTING, FLASH_OPTION_SETTING_1, FLASH_OPTION_MASK);
    #elif defined FLASH_OPTCR1
    fnSetFlashOption(FLASH_OPTION_SETTING, DEFAULT_FLASH_OPTION_SETTING_1, FLASH_OPTION_MASK);
    #else
    fnSetFlashOption(FLASH_OPTION_SETTING, 0, FLASH_OPTION_MASK);
    #endif
#endif
#if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
#else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
#endif
    VECTOR_TABLE_OFFSET_REG   = (TBLBASE_IN_RAM);                        // position the vector table at the bottom of RAM
#if !defined _MINIMUM_IRQ_INITIALISATION
    ptrVect->ptrHardFault     = irq_hard_fault;
    ptrVect->ptrMemManagement = irq_memory_man;
    ptrVect->ptrBusFault      = irq_bus_fault;
    ptrVect->ptrUsageFault    = irq_usage_fault;
    ptrVect->ptrDebugMonitor  = irq_debug_monitor;
    ptrVect->ptrNMI           = irq_NMI;
    ptrVect->ptrPendSV        = irq_pend_sv;
    ptrVect->ptrSVCall        = irq_SVCall;
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;    // fill all processor specific interrupts with a default handler
    do {
        *processor_ints = irq_default;
        if (processor_ints == (void (**)(void))&ptrVect->processor_interrupts.LAST_PROCESSOR_IRQ) {
            break;
        }
        processor_ints++;
    } FOREVER_LOOP();
#else
    ptrVect->ptrHardFault     = irq_default;
    ptrVect->ptrMemManagement = irq_default;
    ptrVect->ptrBusFault      = irq_default;
    ptrVect->ptrUsageFault    = irq_default;
    ptrVect->ptrDebugMonitor  = irq_default;
    ptrVect->ptrNMI           = irq_default;
    ptrVect->ptrPendSV        = irq_default;
    ptrVect->ptrSVCall        = irq_default;
#endif
#if defined DMA_MEMCPY_SET                                               // if uMemcpy()/uMemset() is to use DMA enble the DMA controller
    #if MEMCPY_CHANNEL > 7
    POWER_UP(AHB1, RCC_AHB1ENR_DMA2EN);
    #else
    POWER_UP(AHB1, RCC_AHB1ENR_DMA1EN);
    #endif
#endif
#if defined (_GNU)
    __init_gnu_data();
#endif
#ifdef _WINDOWS                                                          // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Check the size fo interrupt vectors required by the processor!!");
    }
#endif
#ifndef _WINDOWS
    uTaskerBoot();
#endif
    return 0;
}


#if defined ROWLEY && !defined ROWLEY_2                                  // Rowley project requires extra initialisation for debugger to work correctly before V1.7 build 17
static void _main2(void)
{
    asm("mov lr, #0");
    asm("b _main");
}
#endif



// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined COMPILE_IAR
__root const RESET_VECTOR __vector_table @ ".intvec"                     // __root forces the function to be linked in IAR project
#elif defined _GNU
const RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _COMPILE_KEIL
__attribute__((section("RESET"))) const RESET_VECTOR reset_vect
#else
const RESET_VECTOR reset_vect
#endif
= {
    (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - 4),                       // stack pointer to top of RAM (reserving one long word for random number)
    (void (*)(void))START_CODE
};





















#if defined _WINDOWS
// The following routines are only for simulator compatibility

extern void *fnGetHeapStart(void) { return 0; }

// memset implementation
//
extern void *uMemset(void *ptrTo, int iValue, size_t Size)               // {5}
{
    unsigned char ucValue = (unsigned char)iValue;
    void *buffer = ptrTo;
    unsigned char *ptr = (unsigned char *)ptrTo;

    while (Size-- != 0) {
        *ptr++ = ucValue;
    }

    return buffer;
}

// Convert a MAC address to a string
//
// Convert a MAC address to a string
//
signed char *fnMACStr(unsigned char *ptrMAC, signed char *cStr)
{
    signed char cDummyMac[] = "--:--:--:--:--:--";
    int i = 0;
    while (cDummyMac[i] != 0) {
        *cStr++ = cDummyMac[i++];
    }
    *cStr = 0;
    return (cStr);
}

// Convert an IP address to a string
//
signed char *fnIPStr(unsigned char *ptrIP, signed char *cStr)
{
    signed char cDummyIP[] = "---.---.---.---";
    int i = 0;
    while (cDummyIP[i] != 0) {
        *cStr++ = cDummyIP[i++];
    }
    *cStr = 0;
    return (cStr);
}

extern unsigned char *fnGetTxBufferAdd(int iBufNr) { return 0;}

extern void DMA0_handler(void) {}
extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen) {return 0;}
TASK_LIMIT uTaskerStart(const UTASKTABLEINIT *ptATaskTable, const signed char *a_node_descriptions, const PHYSICAL_Q_LIMIT nr_physicalQueues) {return 0;}
void fnInitialiseHeap(const HEAP_NEEDS *ctOurHeap, void *start_heap ){}


// Hardware specific port reset states - if the hardware has pullups set '1' for each bit, else '0'
// (assumed all inputs have pullups)
#define QS_DEFAULT_INPUT           0xff
#define AN_DEFAULT_INPUT           0xff
#define LD_DEFAULT_INPUT           0xff
#define NQ_DEFAULT_INPUT           0xff
#define TA_DEFAULT_INPUT           0xff
#define TC_DEFAULT_INPUT           0xff
#define TD_DEFAULT_INPUT           0xff
#define UA_DEFAULT_INPUT           0xff
#define UB_DEFAULT_INPUT           0xff
#define UC_DEFAULT_INPUT           0xff
#define AS_DEFAULT_INPUT           0xff
#define GP_DEFAULT_INPUT           0xff
#define DD_DEFAULT_INPUT           0xff

INITHW void fnInitHW(void)
{
	extern unsigned long __VECTOR_RAM[];
    unsigned char ucPortPullups[] = {
        QS_DEFAULT_INPUT,                                                // set the port states out of reset in the project file app_hw_str91xf.h
        AN_DEFAULT_INPUT,
        LD_DEFAULT_INPUT,
        NQ_DEFAULT_INPUT,
        TA_DEFAULT_INPUT,
        TC_DEFAULT_INPUT,
        TD_DEFAULT_INPUT,
        UA_DEFAULT_INPUT,
        UB_DEFAULT_INPUT,
        UC_DEFAULT_INPUT,
        AS_DEFAULT_INPUT,
        GP_DEFAULT_INPUT,
        DD_DEFAULT_INPUT
    };

//  __VECTOR_RAM[PIT0_VECTOR] = (unsigned long)fnDummyTick;

    fnInitialiseDevice((void *)ucPortPullups);
    STM32_LowLevelInit();
}


extern void uTaskerBoot(void);
extern void uTaskerSchedule( void )
{
    static int iDone = 0;

    if (iDone == 0) {
        iDone = 1;
        uTaskerBoot();
    }
}
#endif
#endif
