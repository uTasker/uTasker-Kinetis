/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    02.03.2012 Improve endpoint NAK control and unblocking               {1}
    06.03.2012 Add start_application()                                   {3}
    12.09.2012 Enable unicast address along with multicast               {4}
    12.09.2012 Add I2C channel 3 and alternative I2C pin multiplexing    {5}
    18.10.2012 Add Ethernet RMII mode defines                            {6}
    26.10.2012 Add ETHERNET_RELEASE_AFTER_EVERY_FRAME support            {7}
    18.03.2013 Check I2C busy before starting I2C transfers              {8}
    18.03.2013 Use error interrupt to terminate I2C transactions to slaves that don't respond {9}
    22.03.2013 Add optional phy HW reset input control                   {10}
    22.03.2013 Add PHY_POLL_LINK support                                 {11}
    22.03.2013 Add _LAN8720 support                                      {12}
    10.06.2013 Adapted USB FS initialisation for STM32F1 parts           {13}
    26.09.2013 Add STM32F1xx RMII mode configuration (optionally using PLL3 as RMII clock) {14}
    02.10.2013 Correct STM32F1xx flash control                           {15}
    09.01.2014 Add pull-up input options for UART Rx and CTS inputs      {16}
    18.01.2014 Extend interupts to 0..95                                 {17}
    13.02.2014 Add fnSetMultiDropByte() and fnGetMultiDropByte()         {18}
    09.03.2014 Add PARAMETER_NO_ALIGNMENT option to parameter flash      {19}
    09.03.2014 Add interface number to Ethernet when working with multiple IP interfaces {20}
    09.03.2014 Add support for magic reset Ethernet reception frame      {21}
    09.03.2014 Moved RANDOM_SEED_LOCATION and BOOT_MAIL_BOX to STM32.h
    20.03.2014 Correct identification of first F2/F4 flash program sector{22}
    21.05.2014 Add fnModifyMulticastFilter() for IGMP use                {23}
    28.05.2014 Add fnUSB_error_counters()                                {24}
    28.05.2014 Add volatile to ensure that interrupt flags are not optimised away when code in-lining takes place {25}
    02.06.2014 Add TIMER_STOP_TIMER command                              {26}
    15.06.2014 Add F2/F4 DMA uMemset()/uMemcpy()                         {27}
    11.08.2014 Add additional I2C1 pin configuration option              {28}
    02.09.2014 Move remaining byte copy in DMA based uMemcpy() to after DMA termination {29}
    29.01.2015 Add additional I2C1 pin configuration option              {30}
    23.05.2015 Add additional timers (TIM10,11,13,14)                    {31}
    26.05.2015 Add _KS8721 and _ST802RT1B support                        {32}
    26.05.2015 Add hardware operating details for simulator display      {33}
    18.06.2015 Add _fnMIIread() and _fnMIIwrite()                        {34}
    06.05.2017 Modify external port referecne from bit number to bit mask{35}
    31.08.2017 Add ADC                                                   {36}
    11.04.2018 Change uMemset() to match memset() parameters             {37}
    01.05.2018 Write to SYSTICK_CURRENT to synchronise the Systick counter after configuration {38}
    01.11.2018 Use an include file for the clock configuration           {39}
    28.11.2018 Add automatic flash option configuration option           {40}
    13.01.2019 Include USB-OTG header                                    {41}

*/

#if defined _STM32

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _WINDOWS
    #include "config.h"
    extern void fnUpdateOperatingDetails(void);                          // {33}
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    #define _SIM_START_BIT     *ulReg |= I2C_START_BIT;
    #define CLEAR_ALL_INTS     0x00000000
    #define START_CODE 0
    #define SIM_DMA(x) fnSimDMA(x);
    extern unsigned char vector_ram[sizeof(VECTOR_TABLE)];               // vector table in simulated RAM (long word aligned)
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define _SIM_START_BIT
    #include "config.h"
    #define CLEAR_ALL_INTS     0xffffffff
    #if defined (_GNU)
        extern void __str_disable_interrupt(void);
        extern void __str_enable_interrupt(void);
    #endif
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



/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if defined COMPILE_IAR                                                  // interrupt vector configuration as required by IAR compiler
    #pragma language=extended
#endif

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #define SPI_FLASH_ENABLED 
#endif

#if !defined UART_RX_INPUT_TYPE                                          // {16}
    #define UART_RX_INPUT_TYPE  FLOATING_INPUT                           // if no user override configure UART Rx inputs to floating state
#endif
#if !defined UART_CTS_INPUT_TYPE                                         // {16}
    #define UART_CTS_INPUT_TYPE FLOATING_INPUT                           // if no user override configure UART CTS inputs to floating state
#endif

#if defined _APPLICATION_VALIDATION
    #define _RESET_VECTOR  RESET_VECTOR_VALIDATION
#elif defined INTERRUPT_VECTORS_IN_FLASH
    #define _RESET_VECTOR   VECTOR_TABLE
    const _RESET_VECTOR __vector_table;
#else
    #define _RESET_VECTOR  RESET_VECTOR
#endif

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

// CAN interface queues
//
typedef struct stCANQue
{
    unsigned long  ulPartnerID;                                          // default transmission ID (msb signifies extended)
    unsigned long  ulOwnID;                                              // our own ID (msb signifies extended)
    QUEUE_HANDLE   DriverID;                                             // handle this buffer belongs to
    unsigned char  ucMode;
    unsigned char  ucErrors;                                             // tx error counter per buffer
    UTASK_TASK     TaskToWake;                                           // task responsible for processing rx frames, errors or tx frame complete
} CANQUE;        


/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if !defined _WINDOWS
    static void fnInitHW(void);
#endif


static void STM32_LowLevelInit(void);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if !defined ONLY_INTERNAL_FLASH_STORAGE
    static const STORAGE_AREA_ENTRY default_flash = {
        0,                                                               // end of list
        (unsigned char *)(FLASH_START_ADDRESS),                          // start address of internal flash
        (unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH - 1)),    // end of internal flash
        _STORAGE_INTERNAL_FLASH,                                         // type
        0                                                                // not multiple devices
    };

    STORAGE_AREA_ENTRY *UserStorageListPtr = (STORAGE_AREA_ENTRY *)&default_flash; // default entry
#endif

#if (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    static void fnConfigSPIFileSystem(void);
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25 && !defined SPI_FLASH_MX66L
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_STM32_atmel.h"
            #include "spi_flash_STM32_stmicro.h"
            #include "spi_flash_STM32_sst25.h"
            #include "spi_flash_STM32_MX66L.h"
        #undef _SPI_DEFINES
    #endif
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined RANDOM_NUMBER_GENERATOR
    unsigned short *ptrSeed;
#endif

static volatile int iInterruptLevel = 0;

#if defined _WINDOWS
    static unsigned short usPhyMode = 0;
    #if (defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES)
        extern int iFetchingInternalMemory = 0;
    #endif
#endif

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

    #if defined SPI_FLASH_MULTIPLE_CHIPS
        unsigned long ulChipSelect[SPI_FLASH_DEVICE_COUNT] = {
            CS0_LINE,
            CS1_LINE                                                     // at least 2 expected when multiple devices are defined
        #if defined CS2_LINE
            ,CS2_LINE
            #if defined CS3_LINE
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
        #include "spi_flash_STM32_MX66L.h"
    #undef _SPI_FLASH_INTERFACE
#endif

#if defined CAN_INTERFACE
    static CANQUE can_tx_queue[NUMBER_OF_CAN_INTERFACES][NUMBER_CAN_TX_MAILBOXES] = {{{0}}};
    static CANQUE can_rx_queue[NUMBER_OF_CAN_INTERFACES][NUMBER_CAN_RX_MAILBOXES] = {{{0}}};
    static CAN_MAILBOX can_rx_mailboxes[NUMBER_OF_CAN_INTERFACES][NUMBER_CAN_RX_MAILBOXES];
#endif


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined I2C_INTERFACE
    extern I2CQue *I2C_rx_control[NUMBER_I2C];
    extern I2CQue *I2C_tx_control[NUMBER_I2C];
#endif


/* =================================================================== */
/*                      STARTUP CODE                                   */
/* =================================================================== */


// This is the interrupt vector table for the processor including the reset vector (entrypoint)
// It is at a fixed address
//
typedef void (*tIsrFunc)(void);

#if defined _WINDOWS
    #define __disable_interrupt() STM32.CORTEX_M3_REGS.ulPRIMASK = INTERRUPT_MASKED
    #define __enable_interrupt()  STM32.CORTEX_M3_REGS.ulPRIMASK = 0
#else
    extern void __segment_init(void);
    #if defined  COMPILE_IAR
        #pragma segment=".data"
        #pragma segment=".bss"
        static unsigned char *ptrTopOfUsedMemory = 0;
        #define HEAP_START_ADDRESS ptrTopOfUsedMemory                    // IAR6 compiler - last location of static variables
        extern void __disable_interrupt(void);
        extern void __enable_interrupt(void);
        #define  __sleep_mode() __WFI()                                  // IAR intrinsic
    #elif defined _COMPILE_KEIL
        #define HEAP_START_ADDRESS    _keil_ram_size()
        #define __disable_interrupt() __disable_irq()                    // KEIL intrinsics
        #define __enable_interrupt()  __enable_irq()
      //#define __sleep_mode()                                           //__asm("wfi"); Keil doesn't support inline assembly in Thumb mode so this is defined in the startup assembler file
        extern void __sleep_mode(void);
    #else                                                                // disable interrupt in assembler code
        extern unsigned char __heap_end__;
        #if defined ROWLEY_2
            #define asm(x) __asm__(x)
        #endif
        #define HEAP_START_ADDRESS &__heap_end__                         // GNU last location of static variables
        #define __disable_interrupt() asm("cpsid   i")
        #define __enable_interrupt()  asm("cpsie   i")
        #define __sleep_mode()        asm("wfi")
    #endif

    #if defined _COMPILE_KEIL

typedef struct stREGION_TABLE
{
    unsigned char *ptrConst;                                             // const variables belonging to block
    unsigned char *ptrDestination;                                       // destination in RAM
    unsigned long  ulLength;                                             // length of the block in SRAM
    unsigned long  ulUnknown;                                            // entry unknown
} REGION_TABLE;

// Calculate the end of used SRAM from the Keil linker information
//
static unsigned char *_keil_ram_size(void)
{
    extern REGION_TABLE Region$$Table$$Base;                            // table location supplied by linker
    extern REGION_TABLE Region$$Table$$Limit;                           // end of table list
    REGION_TABLE *ptrEntries = &Region$$Table$$Base;                    // first block
    unsigned char *ptrRam = ptrEntries->ptrDestination;                 // RAM address
    do {
        ptrRam += ptrEntries->ulLength;                                 // add length
        ptrEntries++;                                                   // move to next block
    } while (ptrEntries != &Region$$Table$$Limit);
    return ptrRam;
}
    #endif


// main entry for the target code
//
extern int main(void)
{
#if defined MULTISTART
    MULTISTART_TABLE *ptrInfo;
    unsigned char *pucHeapStart;
#endif
#if defined RANDOM_NUMBER_GENERATOR   
    ptrSeed = RANDOM_SEED_LOCATION;                                      // we put an uninitialised variable on to the stack for use as a random seed
#endif
#if defined COMPILE_IAR
    if (__sfe(".bss") > __sfe(".data")) {                                // set last used SRAM address
        ptrTopOfUsedMemory = __sfe(".bss");
    }
    else {
        ptrTopOfUsedMemory = __sfe(".data");
    }
#endif			
    fnInitHW();                                                          // perform hardware initialisation (note that we do not have heap yet)   
#if defined MULTISTART
    ptrInfo = ptMultiStartTable;                                         // if the user has already set to alternative start configuration
    if (ptrInfo == 0) {                                                  // no special start required
_abort_multi:
        fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                 // initialise heap     
        uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system
        while ((ptrInfo = (MULTISTART_TABLE*)uTaskerSchedule()) == 0) {} // schedule uTasker
    }

    do {
        pucHeapStart = HEAP_START_ADDRESS;
        if (ptrInfo->new_hw_init) {                                      // info to next task configuration available
    #if defined DYNAMIC_MULTISTART
            pucHeapStart = ptrInfo->new_hw_init(JumpTable, (void **)&ptrInfo, OurConfigNr); // get heap details from next configuration
    #else
            pucHeapStart = ptrInfo->new_hw_init(JumpTable);              // get heap details from next configuration
    #endif
            if (pucHeapStart == 0) {
                goto _abort_multi;                                       // this can happen if the jump table version doesn't match - prefer to stay in boot mode than start an application which will crash
            }
        }

        fnInitialiseHeap(ptrInfo->ptHeapNeed, pucHeapStart);             // initialise the new heap
                                                                         // start the operating system with next configuration
        uTaskerStart((UTASKTABLEINIT *)ptrInfo->ptTaskTable, ptrInfo->ptNodesTable, PHYSICAL_QUEUES);

        while ((ptrInfo = (MULTISTART_TABLE*)uTaskerSchedule()) == 0) {} // schedule uTasker

    } FOREVER_LOOP();
#else
    fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                     // initialise heap
    uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system

    FOREVER_LOOP() {
        uTaskerSchedule();                                               // schedule uTasker
    }
#endif
    return 0;                                                            // we never return....
}
#endif


#if defined RANDOM_NUMBER_GENERATOR
// How the random number seed is set depends on the hardware possibilities available.
//
extern void fnInitialiseRND(unsigned short *usSeedValue)
{
    if ((*usSeedValue = *ptrSeed) == 0) {                                // we hope that the content of the random seed is random after a power up
        *usSeedValue = 0x127b;                                           // we must not set a zero - so we set a fixed value
    }                                                                    // after a reset it should be well random since it will contain the value at the reset time
    *ptrSeed = fnRandom();                                               // ensure that the seed value is modified between resets
}
#endif



// Support watchdog re-triggering of specific hardware
//
extern void fnRetriggerWatchdog(void)
{
    RETRIGGER_WATCHDOG();                                                // retrigger the watchdog
                                                                         // flash an LED for visualisation
    TOGGLE_WATCHDOG_LED();                                               // define in project defines whether this is performed and where
}


// Routine to disable interrupts during critical region
//
extern void uDisable_Interrupt(void)
{
    __disable_interrupt();                                               // disable interrupts to core
    iInterruptLevel++;                                                   // monitor the level of disable nesting
}

// Routine to re-enable interrupts on leaving a critical region (IAR uses intrinsic function)
//
extern void uEnable_Interrupt(void)
{
#if defined _WINDOWS
    if (0 == iInterruptLevel) {
        *(int *)0 = 0;                                                   // basic error - cause simulator exception
        // A routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
        //
    }
#endif
    if ((--iInterruptLevel) == 0) {                                      // only when no more interrupt nesting,
        __enable_interrupt();                                            // enable processor interrupts
    }
}

// Routine to change interrupt level mask
//
#if !defined _COMPILE_KEIL
    #if defined _GNU
        #define DONT_INLINE __attribute__((noinline))
    #else
        #define DONT_INLINE
    #endif
extern void DONT_INLINE uMask_Interrupt(unsigned char ucMaskLevel) // {102}
{
    #if !defined ARM_MATH_CM0PLUS                                         // mask not supported by Cortex-m0+
        #if defined _WINDOWS
    STM32.CORTEX_M3_REGS.ulBASEPRI = ucMaskLevel;                         // value 0 has no effect - non-zero defines the base priority for exception processing (the processor does not process any exception with a priority value greater than or equal to BASEPRI))
        #else
    asm("msr basepri, r0");                                               // modify the base priority to block interrupts with a lower priority than this level
    asm("bx lr");                                                         // return
        #endif
    #endif
}
#endif


// Function used to enter processor interrupts
//
extern void fnEnterInterrupt(int iInterruptID, unsigned char ucPriority, void (*InterruptFunc)(void))
{
    volatile unsigned long *ptrIntSet = IRQ0_31_SER_ADD;                 // {25}
#if defined ARM_MATH_CM0PLUS || defined ARM_MATH_CM0                     // only long word accesses are possible to the priority registers
    volatile unsigned long *ptrPriority = (unsigned long *)IRQ0_3_PRIORITY_REGISTER_ADD;
    int iShift;
#else
    volatile unsigned char *ptrPriority = IRQ0_3_PRIORITY_REGISTER_ADD;  // {25}
#endif
#if !defined INTERRUPT_VECTORS_IN_FLASH
    VECTOR_TABLE *ptrVect;
    void (**processor_ints)(void);
#endif
#if defined _WINDOWS                                                     // back up the present enabled interrupt registers
    unsigned long ulState0 = IRQ0_31_SER;
    unsigned long ulState1 = IRQ32_63_SER;
    unsigned long ulState2 = IRQ64_95_SER;                               // {17}
    IRQ0_31_SER = IRQ32_63_SER = IRQ0_31_CER = IRQ32_63_CER = IRQ64_95_CER = 0; // {17} reset registers
#endif
#if !defined INTERRUPT_VECTORS_IN_FLASH
    #if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
    #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
    #endif
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;
    processor_ints += iInterruptID;
    *processor_ints = InterruptFunc;
#endif
    ptrIntSet += (iInterruptID/32);
    *ptrIntSet = (0x01 << (iInterruptID%32));                            // enable the interrupt
#if defined _WINDOWS                                                     // check for valid interrupt priority range
    #if defined ARM_MATH_CM0PLUS || defined ARM_MATH_CM0
    if (ucPriority >= 4) {
        _EXCEPTION("Invalid Cortex-M0+ priority being used!!");
    }
    #else
    if (ucPriority >= 16) {
        _EXCEPTION("Invalid Cortex-M4 priority being used!!");
    }
    #endif
#endif
#if defined ARM_MATH_CM0PLUS || defined ARM_MATH_CM0
    ptrPriority += (iInterruptID/4);                                     // move to the priority location used by this interrupt
    iShift = ((iInterruptID % 4) * 8);
    *ptrPriority = ((*ptrPriority & ~(0xff << iShift)) | (ucPriority << (iShift + __NVIC_PRIORITY_SHIFT)));
#else
    ptrPriority += iInterruptID;
    *ptrPriority = (ucPriority << __NVIC_PRIORITY_SHIFT);                // define the interrupt's priority
#endif
#if defined _WINDOWS
    IRQ0_31_SER  |= ulState0;                                            // synchronise the interrupt masks
    IRQ32_63_SER |= ulState1;
    IRQ64_95_SER |= ulState2;                                            // {17}
    IRQ0_31_CER   = IRQ0_31_SER;
    IRQ32_63_CER  = IRQ32_63_SER;
    IRQ64_95_CER  = IRQ64_95_SER;                                        // {17}
#endif
}


// TICK interrupt
//
static __interrupt void _RealTimeInterrupt(void)
{
    INT_CONT_STATE_REG = PENDSTCLR;                                      // reset interrupt
#if defined _WINDOWS
    INT_CONT_STATE_REG &= ~(PENDSTSET | PENDSTCLR);
#endif
    uDisable_Interrupt();                                                // ensure tick handler can not be interrupted
        fnRtmkSystemTick();                                              // operating system tick
    uEnable_Interrupt();
}


// Routine to initialise the Tick interrupt (uses Cortex M3 SysTick timer)
//
#define REQUIRED_US (1000000/(TICK_RESOLUTION))                          // the TICK frequency we require in MHz
#if defined SYSTICK_DIVIDE_8
    #define TICK_DIVIDE ((((HCLK/8) + REQUIRED_US/2)/REQUIRED_US) - 1)   // the divide ratio required - systick is clocked from HCLK/8
#else
    #define TICK_DIVIDE (((HCLK + REQUIRED_US/2)/REQUIRED_US) - 1)       // the divide ratio required - systick is clocked from HCLK
#endif
#if TICK_DIVIDE > 0x00ffffff
    #error "TICK value cannot be achieved with SYSTICK at this core frequency!!"
#endif

// Routine to initialise the tick interrupt (uses Cortex M7/M4/M0+ SysTick timer)
//
extern void fnStartTick(void)
{
#if !defined INTERRUPT_VECTORS_IN_FLASH
    VECTOR_TABLE *ptrVect;
    #if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
    #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
    #endif
    ptrVect->ptrSysTick = _RealTimeInterrupt;                            // enter interrupt handler
#endif
    SYSTICK_RELOAD = TICK_DIVIDE;                                        // set reload value to determine the period
    SYSTICK_CURRENT = TICK_DIVIDE;                                       // {38}
    SYSTEM_HANDLER_12_15_PRIORITY_REGISTER |= (unsigned long)(SYSTICK_PRIORITY << (24 + __NVIC_PRIORITY_SHIFT)); // enter the SYSTICK priority
#if defined SYSTICK_DIVIDE_8
    SYSTICK_CSR = (SYSTICK_ENABLE | SYSTICK_TICKINT);                    // enable timer and its interrupt (clock form HCLK/8)
#else
    SYSTICK_CSR = (SYSTICK_CORE_CLOCK | SYSTICK_ENABLE | SYSTICK_TICKINT); // enable timer and its interrupt (clock from HCLK)
#endif
#if defined _WINDOWS
    SYSTICK_RELOAD &= SYSTICK_COUNT_MASK;                                // mask any values which are out of range
    SYSTICK_CURRENT = SYSTICK_RELOAD;                                    // prime the reload count
#endif
}


#if (defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET)
/* =================================================================== */
/*                          Ethernet Controller                        */
/* =================================================================== */
    #include "stm32_ENET.h"                                              // include Ethernet controller hardware driver code
#endif

#if defined SERIAL_INTERFACE
/* =================================================================== */
/*                    Serial Interface - USART/UART                    */
/* =================================================================== */
    #include "stm32_UART.h"                                            // include USART/UART hardware driver code
#endif

#if defined I2C_INTERFACE
/* =================================================================== */
/*                            I2C Interface                            */
/* =================================================================== */
    #include "stm32_I2C.h"                                               // include I2C hardware driver code
#endif

#if defined SUPPORT_TIMER
/* =================================================================== */
/*                                  Timers                             */
/* =================================================================== */
    #define _TIMER_CODE
        #include "stm32_TIMER.h"                                         // include timer driver code
    #undef _TIMER_CODE
#endif

#if defined SUPPORT_PORT_INTERRUPTS
/* =================================================================== */
/*                           Port Interrupts                           */
/* =================================================================== */
    #define _PORT_INTERRUPT_CODE
        #include "stm32_PORTS.h"                                         // include port interrupt driver code
    #undef _PORT_INTERRUPT_CODE
#endif

#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined SUPPORT_PROGRAM_ONCE
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */
    #include "stm32_FLASH.h"                                             // include FLASH driver code
#endif

#if defined SUPPORT_ADC                                                  // {36}
/* =================================================================== */
/*                                 ADC                                 */
/* =================================================================== */
    #define _ADC_INTERRUPT_CODE
    #include "stm32_ADC.h"                                               // include driver code for ADC
    #undef _ADC_INTERRUPT_CODE
#endif

#if defined USB_INTERFACE                                                // {41}
/* =================================================================== */
/*                                 USB                                 */
/* =================================================================== */
    #if defined USB_OTG_AVAILABLE
        #define _USB_OTG_DRIVER_CODE
        #include "stm32_USB_OTG.h"                                       // include driver code for USB-OTG
        #undef _USB_OTG_DRIVER_CODE
    #endif
    #if defined USB_DEVICE_AVAILABLE
        #define _USB_DEVICE_DRIVER_CODE
        #include "stm32_USB_device.h"                                    // include driver code for USB device
        #undef _USB_DEVICE_DRIVER_CODE
    #endif
#endif

#if defined CAN_INTERFACE

#define PRIORITY_CAN1_TX   7
#define PRIORITY_CAN1_RX0  6
#define PRIORITY_CAN1_RX1  6
#define PRIORITY_CAN1_SCE  5

#define PRIORITY_CAN2_TX   7
#define PRIORITY_CAN2_RX0  6
#define PRIORITY_CAN2_RX1  6
#define PRIORITY_CAN2_SCE  5

// Release transmission buffer holding remote response message
//
static void fnRemoteResponse(int iChannel, CAN_MAILBOX *ptrMailbox)
{
    CANQUE *ptrTxQueue = can_tx_queue[iChannel];
    int i;
    for (i = 0; i < NUMBER_CAN_TX_MAILBOXES; i++) {
        if (ptrTxQueue->ucMode & CAN_TX_BUF_REMOTE) {                    // find the transmission mailbox being used as remote transmission mailbox
            ptrMailbox->CAN_TIR |= CAN_TIR_TXRQ;                         // start transmission process
    #if defined _WINDOWS
            fnSimCAN(iChannel, i, 0);                                    // simulate the buffer operation
    #endif
            return;
        }
        ptrTxQueue++;
        ptrMailbox++;
    }
}


// Handle a message that has been received in one of the receiver FIFOs
//
static void fnCAN_RxFifo(int iChannel, int iFIFO)
{
    static unsigned char can_int_message[HEADER_LENGTH] = {INTERNAL_ROUTE, INTERNAL_ROUTE, 0, INTERRUPT_EVENT, CAN_RX_MSG};
    STM_CAN_CONTROL *ptrCAN_control;
    CANQUE *ptrCanRxQue;
    unsigned char ucUserMailbox;
    CAN_MAILBOX can_rx_mailbox;                                          // temporary copy of mailbox content
    if (iChannel == 0) {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
        if (iFIFO != 0) {
            uMemcpy(&can_rx_mailbox, (void *)ADDR_CAN1_MAILBOX_1_RX, sizeof(CAN_MAILBOX)); // copy locally
            ptrCAN_control->CAN_RF1R |= CAN_RFR_RFOM;                    // release fifo output buffer
        }
        else {
            uMemcpy(&can_rx_mailbox, (void *)ADDR_CAN1_MAILBOX_0_RX, sizeof(CAN_MAILBOX));
            ptrCAN_control->CAN_RF0R |= CAN_RFR_RFOM;                    // release fifo output buffer
        }
        ucUserMailbox = (unsigned char)((can_rx_mailbox.CAN_TDTR >> CAN_RDTR_FMI_SHIFT) * 2); // the filter match that caused reception
        ucUserMailbox += iFIFO;
        if (ucUserMailbox == (((CAN1_FMR & CAN_FMR_CAN2SB_MASK) >> CAN_FMR_CAN2SB_SHIFT) - 1)) { // last filter is a remote ID
            fnRemoteResponse(iChannel, ADDR_CAN1_MAILBOX_TX);
            return;
        }
        ptrCanRxQue = can_rx_queue[0];
    }
    else {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
        if (iFIFO != 0) {
            uMemcpy(&can_rx_mailbox, (void *)ADDR_CAN2_MAILBOX_1_RX, sizeof(CAN_MAILBOX));
            ptrCAN_control->CAN_RF1R |= CAN_RFR_RFOM;                    // release fifo output buffer
        }
        else {
            uMemcpy(&can_rx_mailbox, (void *)ADDR_CAN2_MAILBOX_0_RX, sizeof(CAN_MAILBOX));
            ptrCAN_control->CAN_RF0R |= CAN_RFR_RFOM;                    // release fifo output buffer
        }
        ucUserMailbox = (unsigned char)((can_rx_mailbox.CAN_TDTR >> CAN_RDTR_FMI_SHIFT) * 2); // the filter match that caused reception
        ucUserMailbox += iFIFO;
        if (ucUserMailbox == (NUMBER_CAN_RX_FILTERS - 1)) {              // last filter is a remote ID
            fnRemoteResponse(iChannel, ADDR_CAN2_MAILBOX_TX);
            return;
        }
        ptrCanRxQue = can_rx_queue[1];
    }

    ptrCanRxQue += ucUserMailbox;                                        // match mailbox to a receive queue

    if (!(ptrCanRxQue->ucMode & CAN_RX_BUF_FULL)) {                      // as long as the the user has not still to read a previous message in the mailbox
        uMemcpy(&can_rx_mailboxes[iChannel][ucUserMailbox], &can_rx_mailbox, sizeof(CAN_MAILBOX)); // copy temporary message to user's mailbox
        can_int_message[MSG_DESTINATION_TASK] = ptrCanRxQue->TaskToWake; // task to inform of reception
        ptrCanRxQue->ucMode |= CAN_RX_BUF_FULL;                          // message waiting
        uDisable_Interrupt();                                            // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char *)can_int_message, HEADER_LENGTH); // inform the handling task of event
        uEnable_Interrupt();
        return;
    }
    else {
        // The user has still to read a previous message from this mailbox so use another belonging to the channel if available
        //
        int i;
        QUEUE_HANDLE DriverID = ptrCanRxQue->DriverID;
        ptrCanRxQue -= ucUserMailbox;                                    // start at the first mailbox
        for (i = 0; i < NUMBER_CAN_RX_MAILBOXES; i++) {                  // check through all receive mailboxes
            if (ptrCanRxQue->DriverID == DriverID) {                     // if it belongs to the channel
                if (!(ptrCanRxQue->ucMode & CAN_RX_BUF_FULL)) {          // as long as the the user has not still to read a previous message in the mailbox
                    uMemcpy(&can_rx_mailboxes[iChannel][ucUserMailbox], &can_rx_mailbox, sizeof(CAN_MAILBOX)); // copy temporary message to user's mailbox
                    can_int_message[MSG_DESTINATION_TASK] = ptrCanRxQue->TaskToWake; // task to inform of reception
                    ptrCanRxQue->ucMode |= CAN_RX_BUF_FULL;              // message waiting
                    uDisable_Interrupt();                                // ensure interrupts remain blocked when putting message to queue
                    fnWrite(INTERNAL_ROUTE, (unsigned char *)can_int_message, HEADER_LENGTH); // inform the handling task of event
                    uEnable_Interrupt();
                    return;
                }
            }
            ptrCanRxQue++;
        }
    }
}                                                                        // note that the receive FIFO is always cleared and it is expected that the message could be copied to a free channel buffer
                                                                         // if there were no free buffers the message will be lost


// Error interrupt handling (no acknowledge of transmission will result in CAN_TSR_TERRx being set in CANx_TSR)
//
static void fnCAN_error(int iChannel)
{
    static unsigned char can_error_int_message[HEADER_LENGTH];
    STM_CAN_CONTROL *ptrCAN_control;
    unsigned long ulTxErrors;
    int iMailBoxNumber;
    #if defined _WINDOWS
    CAN_MAILBOX *ptrMailbox;
    #endif

    can_error_int_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;
    can_error_int_message[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;
  //can_error_int_message[MSG_DESTINATION_TASK] = CAN_ERROR_TASK;
    can_error_int_message[MSG_SOURCE_TASK]      = INTERRUPT_EVENT;

    if (iChannel == 0) {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
    #if defined _WINDOWS
        ptrMailbox = ADDR_CAN1_MAILBOX_TX;                               // the first of 3 transmit mailboxes
    #endif
    }
    else {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
    #if defined _WINDOWS
        ptrMailbox = ADDR_CAN2_MAILBOX_TX;                               // the first of 3 transmit mailboxes
    #endif
    }
    #if defined _WINDOWS
    ptrCAN_control->CAN_MSR &= ~(CAN_MSR_ERRI);
    #else
    ptrCAN_control->CAN_MSR = (CAN_MSR_ERRI);                            // clear the error interrupt
    #endif
    ulTxErrors = ptrCAN_control->CAN_TSR;                                // initial error state
    while (ulTxErrors & (CAN_TSR_TERR0 | CAN_TSR_TERR1 | CAN_TSR_TERR2)) { // transmit error occurred
        if (ulTxErrors & CAN_TSR_TERR0) {
            ptrCAN_control->CAN_TSR = (CAN_TSR_ABRQ0);                   // request abort of mailbox 0
    #if defined _WINDOWS
            ptrCAN_control->CAN_TSR &= ~(CAN_TSR_ABRQ0);
    #endif
            ulTxErrors &= ~(CAN_TSR_TERR0);
            iMailBoxNumber = 0;
        }
        else if (ulTxErrors & CAN_TSR_TERR1) {
            ptrCAN_control->CAN_TSR = (CAN_TSR_ABRQ1);                   // request abort of mailbox 1
            ulTxErrors &= ~(CAN_TSR_TERR1);
            iMailBoxNumber = 1;
    #if defined _WINDOWS
        ptrCAN_control->CAN_TSR &= ~(CAN_TSR_ABRQ1);
    #endif
        }
        else /*if (ulTxErrors & CAN_TSR_TERR2)*/ {
            ptrCAN_control->CAN_TSR = (CAN_TSR_ABRQ2);                   // request abort of mailbox 2
            ulTxErrors &= ~(CAN_TSR_TERR2);
            iMailBoxNumber = 2;
    #if defined _WINDOWS
        ptrCAN_control->CAN_TSR &= ~(CAN_TSR_ABRQ2);
    #endif
        }
        can_error_int_message[MSG_DESTINATION_TASK] = can_tx_queue[iChannel][iMailBoxNumber].TaskToWake;
        if (can_tx_queue[iChannel][iMailBoxNumber].ucMode & CAN_TX_BUF_REMOTE) {
            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_REMOTE_ERROR;
            can_tx_queue[iChannel][iMailBoxNumber].ucMode = (CAN_TX_BUF | CAN_RX_BUF_FULL | CAN_TX_BUF_REMOTE); // mark that it is an errored transmission buffer
        }
        else {
            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_ERROR;
            can_tx_queue[iChannel][iMailBoxNumber].ucMode = (CAN_TX_BUF | CAN_RX_BUF_FULL); // mark that it is an errored transmission buffer
        }
        uDisable_Interrupt();                                            // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, can_error_int_message, HEADER_LENGTH);   // inform the handling task of event
        uEnable_Interrupt();
    }
    #if defined _WINDOWS
    (ptrMailbox + iMailBoxNumber)->CAN_TIR &= ~CAN_TIR_TXRQ;
    #endif
}

// Acknowledge a successful CAN transmission if required
//
static void fnAckCANTx(int iController, int iMailbox)
{
    static unsigned char can_int_message[HEADER_LENGTH] = {INTERNAL_ROUTE, INTERNAL_ROUTE, 0, INTERRUPT_EVENT, 0};
    CANQUE *ptrCanTxQue = &can_tx_queue[iController][iMailbox];
    int iNoWake = 0;
    if (CAN_TX_BUF_REMOTE & ptrCanTxQue->ucMode) {
        if (ptrCanTxQue->ucMode & CAN_TX_ACK_ON) {
            can_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_REMOTE_ACK;    // remote frame transmission message successfully acked
            ptrCanTxQue->ucMode &= ~(CAN_TX_ACK_ON);                     // we only ack once but the buffer remains blocked until cancelled
        }
        else {
            iNoWake = 1;
        }
    }
    else {
        if (CAN_TX_ACK_ON & ptrCanTxQue->ucMode) {
            can_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_ACK;           // tx message successfully acked
        }
        else {
            iNoWake = 1;                                                 // no acknowledgement required
        }
        ptrCanTxQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);            // indicate successful transmission, buffer free for further use
    }
    if (!iNoWake) {
        can_int_message[MSG_DESTINATION_TASK] = ptrCanTxQue->TaskToWake; // the task that should be woken when this buffer has been successfully sent
        uDisable_Interrupt();                                            // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char *)can_int_message, HEADER_LENGTH); // inform the handling task of event
        uEnable_Interrupt();
    }
}

// A transmit interrupt has occurred due to RQCPx (request complete) being set in CAN1_TSR
//
__interrupt void CAN1_Tx_Interrupt(void)
{
    while (CAN1_TSR & (CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2)) { // while there is a transmit interrupt pending
        if (CAN1_TSR & CAN_TSR_RQCP0) {                                  // mailbox 0
            CAN1_TSR = CAN_TSR_RQCP0;                                    // clear by writing '1'
    #if defined _WINDOWS
            CAN1_TSR &= ~(CAN_TSR_RQCP0);
    #endif
            fnAckCANTx(0, 0);                                            // acknowledge transmission if required
        }
        else if (CAN1_TSR & CAN_TSR_RQCP1) {                             // mailbox 1
            CAN1_TSR = CAN_TSR_RQCP1;                                    // clear by writing '1'
    #if defined _WINDOWS
            CAN1_TSR &= ~(CAN_TSR_RQCP1);
    #endif
            fnAckCANTx(0, 1);                                            // acknowledge transmission if required
        }
        else if (CAN1_TSR & CAN_TSR_RQCP2) {                             // mailbox 3
            CAN1_TSR = CAN_TSR_RQCP1;                                    // clear by writing '1'
    #if defined _WINDOWS
            CAN1_TSR &= ~(CAN_TSR_RQCP2);
    #endif
            fnAckCANTx(0, 2);                                            // acknowledge transmission if required
        }
    }
}

__interrupt void CAN1_Rx0_Interrupt(void)
{
    fnCAN_RxFifo(0, 0);
}

__interrupt void CAN1_Rx1_Interrupt(void)
{
    fnCAN_RxFifo(0, 1);
}

__interrupt void CAN1_SCE_Interrupt(void)
{
    fnCAN_error(0);
}

// A transmit interrupt has occurred due to RQCPx (request complete) being set in CAN2_TSR
//
__interrupt void CAN2_Tx_Interrupt(void)
{
    while (CAN2_TSR & (CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2)) { // while there is a transmit interrupt pending
        if (CAN2_TSR & CAN_TSR_RQCP0) {                                  // mailbox 0
            CAN2_TSR = CAN_TSR_RQCP0;                                    // clear by writing '1'
    #if defined _WINDOWS
            CAN2_TSR &= ~(CAN_TSR_RQCP0);
    #endif
            fnAckCANTx(1, 0);                                            // acknowledge transmission if required
        }
        else if (CAN2_TSR & CAN_TSR_RQCP1) {                             // mailbox 1
            CAN2_TSR = CAN_TSR_RQCP1;                                    // clear by writing '1'
    #if defined _WINDOWS
            CAN2_TSR &= ~(CAN_TSR_RQCP1);
    #endif
            fnAckCANTx(1, 1);                                            // acknowledge transmission if required
        }
        else if (CAN2_TSR & CAN_TSR_RQCP2) {                             // mailbox 3
            CAN2_TSR = CAN_TSR_RQCP1;                                    // clear by writing '1'
    #if defined _WINDOWS
            CAN2_TSR &= ~(CAN_TSR_RQCP2);
    #endif
            fnAckCANTx(1, 2);                                            // acknowledge transmission if required
        }
    }
}

__interrupt void CAN2_Rx0_Interrupt(void)
{
    fnCAN_RxFifo(1, 0);
}

__interrupt void CAN2_Rx1_Interrupt(void)
{
    fnCAN_RxFifo(1, 1);
}

__interrupt void CAN2_SCE_Interrupt(void)
{
    fnCAN_error(1);
}


// The CAN clock source is always the peripheral clock 1 - this routine tries to achieve best settings using highest time quanta resolution. 
//
// There are up to 25 time quanta in a CAN bit time and the bit frequency is equal to the clock frequency divided by the quanta number (8..25 time quanta range)
// There is always a single time quanta at the start of a bit period called the SYNC_SEG which can not be changed (transitions are expected to occur on the bus during this period)
// The sampling occurs after time segment 1, which is made up of a propagation segment (1..8 time quanta) plus a phase buffer segment 1 (1..8 time quanta),
// followed by time segment 2, made up of phase buffer segment 2 (2..8 time quanta)
//
// CAN standard compliant bit segment settings give the following ranges (re-synchronisation jump width of 2 is used since it is compliant with all)
// Time segment 1 should be 5..10 when time segment 2 is 2 (min/max time quanta per bit is 8/13)
// Time segment 1 should be 4..11 when time segment 2 is 3 (min/max time quanta per bit is 8/15)
// Time segment 1 should be 5..12 when time segment 2 is 4 (min/max time quanta per bit is 10/17)
// Time segment 1 should be 6..13 when time segment 2 is 5 (min/max time quanta per bit is 12/19)
// Time segment 1 should be 7..14 when time segment 2 is 6 (min/max time quanta per bit is 14/21)
// Time segment 1 should be 8..15 when time segment 2 is 7 (min/max time quanta per bit is 16/23)
// Time segment 1 should be 9..16 when time segment 2 is 8 (min/max time quanta per bit is 18/25)
//
static unsigned long fnOptimalCAN_clock(unsigned short usMode, unsigned long ulSpeed)
{
    unsigned long ulClockSpeed;
    unsigned long ulLowestError = 0xffffffff;
    unsigned long ulCanSpeed;
    unsigned long ulError;
    unsigned long ulPrescaler;
    int iTimeQuanta = 25;                                                // highest value for highest control resolution
    int iBestTimeQuanta = 25;
    unsigned long ulBestPrescaler;
    if (CAN_USER_SETTINGS & usMode) {
        return ulSpeed;                                                  // the user is passing optimal configuration settings directly
    }
    ulClockSpeed = PCLK1;
    while (iTimeQuanta >= 8) {                                           // test for best time quanta
        ulCanSpeed = (ulClockSpeed/iTimeQuanta);                         // speed without prescaler
        ulPrescaler = ((ulCanSpeed + (ulSpeed/2))/ulSpeed);              // best prescale value
        if (ulPrescaler > 256) {
            ulPrescaler = 256;                                           // maximum possible prescale divider
        }
        ulCanSpeed /= ulPrescaler;
        if (ulCanSpeed >= ulSpeed) {                                     // determine the absolute error value with this quanta setting
            ulError = (ulCanSpeed - ulSpeed);
        }
        else {
            ulError = (ulSpeed - ulCanSpeed);
        }
        if (ulError < ulLowestError) {                                   // if this is an improvement
            ulLowestError = ulError;
            iBestTimeQuanta = iTimeQuanta;                               // best time quanta value
            ulBestPrescaler = ulPrescaler;
        }
        iTimeQuanta--;
    }
    ulBestPrescaler--;
    if (iBestTimeQuanta >= 18) {                                         // determine the phase buffer length value
        ulBestPrescaler |= CAN_BTR_TS2_8;
        iBestTimeQuanta -= (8 + 1);                                      // remaining time quanta (time segment 1) after removal of the time segment 2 and the SYN_SEG
    }
    else if (iBestTimeQuanta >= 16) {
        ulBestPrescaler |= CAN_BTR_TS2_7;
        iBestTimeQuanta -= (7 + 1);
    }
    else if (iBestTimeQuanta >= 14) {
        ulBestPrescaler |= CAN_BTR_TS2_6;
        iBestTimeQuanta -= (6 + 1);
    }
    else if (iBestTimeQuanta >= 12) {
        ulBestPrescaler |= CAN_BTR_TS2_5;
        iBestTimeQuanta -= (5 + 1);
    }
    else if (iBestTimeQuanta >= 10) {
        ulBestPrescaler |= CAN_BTR_TS2_4;
        iBestTimeQuanta -= (4 + 1);
    }
    else {
        ulBestPrescaler |= CAN_BTR_TS2_3;
        iBestTimeQuanta -= (3 + 1);
    }
    return (CAN_BTR_SJW_2 | ((iBestTimeQuanta - 1) << CAN_BTR_TS1_SHIFT)| ulBestPrescaler); // initialise the CAN controller with the required speed and parameters
}

// Hardware initialisation of CAN controller
//
extern void fnInitCAN(CANTABLE *pars)
{
    STM_CAN_CONTROL *ptrCAN_control;
    // Enable CAN controller and pins to be used
    //
    if (pars->Channel == 0) {                                            // CAN 1 (master)
        POWER_UP(APB1, RCC_APB1ENR_CAN1EN);                              // enable clocks to CAN 1
        ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX) && defined CAN1_ON_PORT_I_H
        _CONFIG_PERIPHERAL_INPUT(I, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTI_BIT9),  (INPUT_PULL_UP)); // CAN 1 RX on I9
        _CONFIG_PERIPHERAL_INPUT(H, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTH_BIT13), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 1 TX on H13
    #elif defined CAN1_ON_PORT_A
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTA_BIT11), (INPUT_PULL_UP)); // CAN 1 RX on PA11
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTA_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 1 TX on PA12
    #elif defined CAN1_ONPORT_B
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT9),  (INPUT_PULL_UP)); // CAN 1 RX on PB8
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT8),  (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 1 TX on PB9
    #else
        _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTD_BIT0),  (INPUT_PULL_UP)); // CAN 1 RX on PD0
        _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTD_BIT1),  (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 1 TX on PD1
    #endif
        fnEnterInterrupt(irq_CAN1_TX_ID,  PRIORITY_CAN1_TX,  CAN1_Tx_Interrupt); // enter CAN 1 interrupt handlers
        fnEnterInterrupt(irq_CAN1_RX0_ID, PRIORITY_CAN1_RX0, CAN1_Rx0_Interrupt);
        fnEnterInterrupt(irq_CAN1_RX1_ID, PRIORITY_CAN1_RX1, CAN1_Rx1_Interrupt);
        fnEnterInterrupt(irq_CAN1_SCE_ID, PRIORITY_CAN1_SCE, CAN1_SCE_Interrupt);
    }
    else {                                                               // CAN 2 (slave)
        POWER_UP(APB1, (RCC_APB1ENR_CAN1EN | RCC_APB1ENR_CAN2EN));       // enable clocks to CAN 1 and 2 (slave requires master)
        ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
    #if defined CAN2_OPTION_1
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT5), (INPUT_PULL_UP)); // CAN 2 RX on PB5
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 2 TX on PB6
    #elif defined CAN2_OPTION_2
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT12), (INPUT_PULL_UP)); // CAN 2 RX on PB12
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT13), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 2 TX on PB13
    #elif defined CAN2_OPTION_3
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT12), (INPUT_PULL_UP)); // CAN 2 RX on PB12
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 2 TX on PB6
    #else
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT5), (INPUT_PULL_UP)); // CAN 2 RX on PB5
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT13), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // CAN 2 TX on PB13
    #endif
        fnEnterInterrupt(irq_CAN2_TX_ID,  PRIORITY_CAN2_TX,  CAN2_Tx_Interrupt); // enter CAN 2 interrupt handlers
        fnEnterInterrupt(irq_CAN2_RX0_ID, PRIORITY_CAN2_RX0, CAN2_Rx0_Interrupt);
        fnEnterInterrupt(irq_CAN2_RX1_ID, PRIORITY_CAN2_RX1, CAN2_Rx1_Interrupt);
        fnEnterInterrupt(irq_CAN2_SCE_ID, PRIORITY_CAN2_SCE, CAN2_SCE_Interrupt);
    }

    ptrCAN_control->CAN_MCR = (CAN_MCR_DBF | CAN_MCR_INRQ | CAN_MCR_TTCM); // exit sleep mode and request move to initialisation mode (enable also time stamps)
    while (!(ptrCAN_control->CAN_MSR & CAN_MSR_INAK)) {                  // wait until the controller has confirmed that it is in the initialised mode
    #if defined _WINDOWS
        ptrCAN_control->CAN_MSR &= ~(CAN_MSR_SLAK);
        ptrCAN_control->CAN_MSR |= CAN_MSR_INAK;
    #endif
    }

    ptrCAN_control->CAN_BTR = fnOptimalCAN_clock(pars->usMode, pars->ulSpeed); // write the clock setting
}


// Hardware configuration of CAN controller
//
extern void fnConfigCAN(QUEUE_HANDLE DriverID, CANTABLE *pars)
{
    int i;
    int iFilterOffset;
    unsigned char ucTxCnt = pars->ucTxBuffers;
    unsigned char ucRxCnt = pars->ucRxBuffers;
    CAN_MAILBOX *ptrRxMailbox;
    CANQUE *ptrCanTxQue, *ptrCanRxQue;
    STM_CAN_CONTROL *ptrCAN_control;
    unsigned long *ptrFilters;
    switch (pars->Channel) {
    case 0:
        ptrCanTxQue = can_tx_queue[0];
        ptrCanRxQue = can_rx_queue[0];
        ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
        ptrRxMailbox = ADDR_CAN1_MAILBOX_0_RX;                           // the first of 2 receive mailboxes
        ptrFilters = CAN1_F0R1_ADDR;
        iFilterOffset = 0;
        break;
    case 1:
        ptrCanTxQue = can_tx_queue[1];
        ptrCanRxQue = can_rx_queue[1];
        ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
        ptrRxMailbox = ADDR_CAN2_MAILBOX_0_RX;                           // the first of 2 receive mailboxes
        ptrFilters = CAN1_F14R1_ADDR;
        iFilterOffset = 14;
        break;
    default:
        return;
    }

    for (i = 0; i < NUMBER_CAN_TX_MAILBOXES; i++) {                      // initialise the requested number of transmit buffers
        if (ucTxCnt == 0) {
            break;
        }
        if (!(ptrCanTxQue->DriverID)) {                                  // not yet allocated
            ptrCanTxQue->DriverID = DriverID;                            // assign this mailbox to the virtual interface
            ptrCanTxQue->TaskToWake = pars->Task_to_wake;                // the mailbox owner
            ptrCanTxQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);
            if (pars->ulTxID & CAN_EXTENDED_ID) {                        // if extended ID used as default destination
                ptrCanTxQue->ulPartnerID = ((pars->ulTxID << CAN_IR_EXTENDED_ID_SHIFT) | CAN_IR_IDE); // enter default extended destination  ID
            }
            else {
                ptrCanTxQue->ulPartnerID = ((pars->ulTxID << CAN_IR_STANDARD_ID_SHIFT) | CAN_IR_STD); // enter default standard ID for the buffer
            }
            ptrCanTxQue->ulOwnID = pars->ulRxID;                         // default destination ID
            if (pars->ulRxID & CAN_EXTENDED_ID) {                        // if an extended ID is used for the reception channel
                ptrCanTxQue->ulOwnID = ((pars->ulRxID << CAN_IR_EXTENDED_ID_SHIFT) | CAN_IR_IDE);
            }
            else {
                ptrCanTxQue->ulOwnID = ((pars->ulRxID << CAN_IR_STANDARD_ID_SHIFT) | CAN_IR_STD);
            }
            ucTxCnt--;
        }
        ptrCanTxQue++;
    }

    CAN1_FMR = ((14 << CAN_FMR_CAN2SB_SHIFT) | CAN_FMR_FINIT);           // enable filter writes
    CAN1_FS1R = 0x0fffffff;                                              // set 32 bit filter for all filters
    CAN1_FFA1R = 0x0aaaaaaa;                                             // assign alternatively between FIFO 0 and 1 (sequence 0,1,0,1,0,1..)
    for (i = 0; i < NUMBER_CAN_RX_MAILBOXES; i++) {                      // initialise the requested number of receive mailboxes
        if (ucRxCnt == 0) {                                              // all requested filters initialised
            break;
        }
        if (!(ptrCanRxQue->DriverID)) {                                  // not yet allocated
            ptrCanRxQue->DriverID = DriverID;                            // assign the receive mailbox to the virtual interface
            ptrCanRxQue->TaskToWake = pars->Task_to_wake;                // task to wake on receptions in this mailbox
            CAN1_FM1R &= ~(CAN_FM1R_FBM0_LIST_MODE << iFilterOffset);    // select mask mode
            if (pars->ulRxID & CAN_EXTENDED_ID) {                        // if the mailbox uses an extended ID
                *ptrFilters = ((pars->ulRxID << CAN_IR_EXTENDED_ID_SHIFT) | CAN_IR_IDE); // enter extended filter ID
                *(ptrFilters + 1) = (CAN_IR_EXTENDED_ID_MASK | CAN_IR_IDE | CAN_IR_RTR); // set mask
            }
            else {                                                       // else the mailbox uses a standard ID
                *ptrFilters = ((pars->ulRxID << CAN_IR_STANDARD_ID_SHIFT)); // enter standard filter ID
                *(ptrFilters + 1) = (CAN_IR_STANDARD_ID_MASK | CAN_IR_IDE | CAN_IR_RTR); // set mask
            }
            CAN1_FA1R |= (CAN_FA1R_FACT0ON << iFilterOffset);            // activate the filter
            ucRxCnt--;
            ptrFilters += 2;                                             // move to next filter
        }
        ptrCanRxQue++;                                                   // move to next channel
        iFilterOffset++;
    }

    CAN1_FMR = (14 << CAN_FMR_CAN2SB_SHIFT);                             // filters locked
    ptrCAN_control->CAN_MCR &= ~(CAN_MCR_INRQ);                          // request move to normal mode
    while ((ptrCAN_control->CAN_MSR & CAN_MSR_INAK) != 0) {              // wait until the controller has confirmed that it is in the normal mode
    #if defined _WINDOWS
        ptrCAN_control->CAN_MSR &= ~CAN_MSR_INAK;
    #endif
    }

    ptrCAN_control->CAN_IER = (CAN_IER_TMEIE | (CAN_IER_FMPIE0 | CAN_IER_FMPIE1 | CAN_IER_LECIE | CAN_IER_ERRIE)); // enable interrupt when transmit mailbox is empty, input is in one of the FIFOs and on passive errors (eg. no acknowledgement to a transmission)


    #if defined _WINDOWS
    fnSimCAN(pars->Channel, 0, CAN_SIM_INITIALISE);
    #endif
}

static void fnRemoteFilter(int Channel, unsigned long ulPartnerID)
{
    unsigned long *ptrFilters = CAN1_F0R1_ADDR;
    int iFilterOffset;
    // We set a filter to monitor the remote frame ID, using the last filter (which is expected to be free)
    //
    if (Channel == 0) {
        iFilterOffset = (((CAN1_FMR & CAN_FMR_CAN2SB_MASK) >> CAN_FMR_CAN2SB_SHIFT) - 1);
    }
    else {
        iFilterOffset = (NUMBER_CAN_RX_FILTERS - 1);
    }
    ptrFilters += (iFilterOffset * 2);
    CAN1_FA1R &= ~(CAN_FA1R_FACT0ON << iFilterOffset);                   // ensure filter deactivated for change
    if (ulPartnerID == 0) {
        return;                                                          // disable only
    }
    CAN1_FM1R &= ~(CAN_FM1R_FBM0_LIST_MODE << iFilterOffset);            // select mask mode
    *ptrFilters = ulPartnerID;                                           // set filter on remote request
    if (ulPartnerID & CAN_IR_IDE) {                                      // if extended ID
        *(ptrFilters + 1) = (CAN_IR_EXTENDED_ID_MASK | CAN_IR_IDE | CAN_IR_RTR); // set extended mask with remote
    }
    else {                                                               // else standard ID
        *(ptrFilters + 1) = (CAN_IR_STANDARD_ID_MASK | CAN_IR_IDE | CAN_IR_RTR); // set standard mask with remote
    }
    CAN1_FA1R |= (CAN_FA1R_FACT0ON << iFilterOffset);                    // activate filter
}

// The CAN driver calls this to send a message
//
extern unsigned char fnCAN_tx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter)
{
    int i;
    CAN_MAILBOX *ptrMailbox;
    unsigned char ucTxMode = (unsigned char)Counter & CAN_TX_MSG_MASK;
    CANQUE *ptrCanTxQue;
    unsigned char ucRtnCnt; 
    unsigned long ulExtendedID = 0;

    if (Channel == 0) {
        ptrCanTxQue = &can_tx_queue[0][0];
        ptrMailbox = ADDR_CAN1_MAILBOX_TX;                               // the first of 3 transmit mailboxes
    }
    else {
        ptrCanTxQue = &can_tx_queue[1][0];
        ptrMailbox = ADDR_CAN2_MAILBOX_TX;                               // the first of 3 transmit mailboxes
    }

    Counter &= ~CAN_TX_MSG_MASK;                                         // data length
    ucRtnCnt = (unsigned char)Counter;

    if (ucTxMode & (TX_REMOTE_FRAME | TX_REMOTE_STOP)) {                 // only one remote transmit buffer allowed
        for (i = 0; i < NUMBER_CAN_TX_MAILBOXES; i++) { 
            if (ptrCanTxQue->DriverID == DriverID) {                     // find a buffer belonging to us
                if (ptrCanTxQue->ucMode & CAN_TX_BUF_REMOTE) {           // active remote buffer found
                    ptrCanTxQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);// buffer remains inactive tx buffer
                    if (ucTxMode & TX_REMOTE_STOP) {
                        fnRemoteFilter(Channel, 0);                      // deactivate the remote filter
    #if defined _WINDOWS
                        fnSimCAN(Channel, i, CAN_SIM_FREE_BUFFER);
    #endif
                        return 1;                                        // remote frame transmission disabled
                    }
                    break;                                               // the remote buffer has been found
                }
            }
            ptrCanTxQue++;
        }
        if (i >= NUMBER_CAN_TX_MAILBOXES) {                              // no remote buffer present
            i = 0;                                                       // reset ready for restart
            if (Channel != 0) {
                ptrCanTxQue = can_tx_queue[1];
            }
            else {
                ptrCanTxQue = can_tx_queue[0];
            }
        }
    }

    for (i = 0; i < NUMBER_CAN_TX_MAILBOXES; i++) {                      // for each possible transmit mailbox
        if (ptrCanTxQue->DriverID == DriverID) {                         // find a mailbox belonging to us
            if ((ptrCanTxQue->ucMode & CAN_TX_BUF_FREE) && (!(TX_REMOTE_STOP & ucTxMode))) { // if the transmit buffer is free and not stopping transmission
                if (TX_REMOTE_FRAME & ucTxMode) {                        // depositing a message to be sent on a remote request
                    ulExtendedID = ptrCanTxQue->ulPartnerID;
                    fnRemoteFilter(Channel, (ulExtendedID | CAN_IR_RTR));// set up temporary remote filter to receive the remote request
                    ptrCanTxQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_ACTIVE | CAN_TX_BUF_REMOTE | (CAN_TX_ACK_ON & ucTxMode)); // mark that the buffer is in use
                }
                else {
                    if (ucTxMode & SPECIFIED_ID) {                       // is the user specifying a destination ID or can default be used?
                        unsigned long ulPartnerID = 0;
                        ulPartnerID = *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        if (ulPartnerID & CAN_EXTENDED_ID) {
                            ulExtendedID = ((ulPartnerID << CAN_IR_EXTENDED_ID_SHIFT) | CAN_IR_IDE); // send to specified extended ID address
                        }
                        else {
                            ulExtendedID = (ulPartnerID << CAN_IR_STANDARD_ID_SHIFT); // send to specified standard ID address
                        }
                        Counter -= 4;
                    }
                    else {                                               // transmission to default ID
                        if ((ptBuffer == 0) && (Counter == 0)) {         // remote frame request
                            ulExtendedID = (ptrCanTxQue->ulOwnID | CAN_IR_RTR);
                        }
                        else {
                            ulExtendedID = ptrCanTxQue->ulPartnerID;
                        }
                    }
                    ptrCanTxQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_ACTIVE | (CAN_TX_ACK_ON & ucTxMode)); // mark that the buffer is in use for transmission
                }
                if (Counter > 8) {                                       // limit the count to maximum
                    Counter = 8;
                }                    
                ptrMailbox->CAN_TDTR = Counter;                          // the data length 
                // The data needs to be stored as long words in little-endian format
                //
                if (ptBuffer != 0) {
                    ptrMailbox->CAN_TDLR = ((*(ptBuffer + 3) << 24) | (*(ptBuffer + 2) << 16) | (*(ptBuffer + 1) << 8) | (*(ptBuffer)));
                    ptrMailbox->CAN_TDHR = ((*(ptBuffer + 7) << 24) | (*(ptBuffer + 6) << 16) | (*(ptBuffer + 5) << 8) | (*(ptBuffer + 4)));
                }
                ptrCanTxQue->ucErrors = 0;
                if (TX_REMOTE_FRAME & ucTxMode) {
                    ptrMailbox->CAN_TIR = ulExtendedID;                  // prepare the buffer but don't send yet
                }
                else {
                    ptrMailbox->CAN_TIR = (ulExtendedID | CAN_TIR_TXRQ); // start the process
                }
    #if defined _WINDOWS
                fnSimCAN(Channel, i, 0);                                 // simulate the buffer operation
    #endif
                return (unsigned char)ucRtnCnt;
            }
        }
        ptrMailbox++;
        ptrCanTxQue++;
    }
    return 0;                                                            // no free buffer was found
}

// Extract a message from a mailbox and optionally add time stamp and ID to it
//
static unsigned char fnExtractMessage(CAN_MAILBOX *ptrMailbox, unsigned char *ptBuffer, unsigned char ucCommand)
{
    unsigned char ucLength = 0;
    unsigned long ulDLC;
    unsigned long ulID = ptrMailbox->CAN_TIR;
    if (ucCommand & GET_CAN_RX_TIME_STAMP) {                             // insert (reception) time stamp
        *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDTR >> 24);
        *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDTR >> 16);
        ucLength += 2;
    }    
    if (ucCommand & GET_CAN_RX_ID) {
        if (ulID & CAN_IR_IDE) {                                         // if extended ID
            ulID >>= CAN_IR_EXTENDED_ID_SHIFT;
            if (ucCommand & GET_CAN_TX_ERROR) {
                *ptBuffer++ = (unsigned char)(ulID >> 24);               // write ID to buffer (no extended ID user bit
            }
            else {
                *ptBuffer++ = (unsigned char)((ulID | CAN_EXTENDED_ID) >> 24); // write ID to buffer with extended ID user bit set
            }
            *ptBuffer++ = (unsigned char)(ulID >> 16);
        }
        else {                                                           // else standard ID
            ulID >>= CAN_IR_STANDARD_ID_SHIFT;
            *ptBuffer++ = 0;
            *ptBuffer++ = 0;
        }
        ucLength += 4;
    }
    *ptBuffer++ = (unsigned char)(ulID >> 8);
    *ptBuffer++ = (unsigned char)(ulID);
    ulDLC = (ptrMailbox->CAN_TDTR & CAN_TDTR_DLC_MASK);
    ucLength = (unsigned char)(ulDLC + ucLength);
    if (ulDLC > 0) {
        *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDLR);
        if (ulDLC > 1) {
            *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDLR >> 8);
            if (ulDLC > 2) {
                *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDLR >> 16);
                if (ulDLC > 3) {
                    *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDLR >> 24);
                    if (ulDLC > 4) {
                        *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDHR);
                        if (ulDLC > 5) {
                            *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDHR >> 8);
                            if (ulDLC > 6) {
                                *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDHR >> 16);
                                if (ulDLC > 7) {
                                    *ptBuffer++ = (unsigned char)(ptrMailbox->CAN_TDHR >> 24);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return ucLength;
}

// The CAN driver calls this to collect a received message
//
extern unsigned char fnCAN_get_rx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter)
{
    int i;
    CANQUE *ptrCanQue;
    STM_CAN_CONTROL *ptrCAN_control;
    unsigned char ucCommand = (unsigned char)Counter;

    if (ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR)) {
        CAN_MAILBOX *ptrMailbox;
        if (Channel == 0) {
            ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
            ptrCanQue = &can_tx_queue[0][0];
            ptrMailbox = ADDR_CAN1_MAILBOX_TX;                           // the first of 3 transmit mailboxes
        }
        else {
            ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
            ptrCanQue = &can_tx_queue[1][0];
            ptrMailbox = ADDR_CAN2_MAILBOX_TX;                           // the first of 3 transmit mailboxes
        }
        for (i = 0; i < NUMBER_CAN_TX_MAILBOXES; i++) {                  // search through transmit buffers
            if (ptrCanQue->DriverID == DriverID) {                       // find a buffer belonging to us
                if (ptrCanQue->ucMode & CAN_RX_BUF_FULL) {               // if the buffer contains data (due to a failure)
                    ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);  // the buffer may be used again since the lost data will been rescued
                    return (fnExtractMessage(ptrMailbox, ptBuffer, GET_CAN_RX_ID)); // extract the message, including its ID
                }
            }
            ptrCanQue++;
        }
        return 0;                                                        // no data was found
    }
    else  {                                                              // get reception data
        if (Channel == 0) {
            ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
            ptrCanQue = &can_rx_queue[0][0];
        }
        else {
            ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
            ptrCanQue = &can_rx_queue[1][0];
        }

        for (i = 0; i < NUMBER_CAN_RX_MAILBOXES; i++) {                  // search through receive buffers
            if (ptrCanQue->DriverID == DriverID) {                       // find a buffer belonging to us
                if ((ucCommand & FREE_CAN_RX_REMOTE) && (ptrCanQue->ucMode & CAN_RX_REMOTE_RX)) { // release a waiting remote frame receiver buffer
                    ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);  // buffer is now a normal tx buffer again
    #if defined _WINDOWS
                    fnSimCAN(Channel, ((NUMBER_CAN_TX_MAILBOXES - 1) - i), CAN_SIM_FREE_BUFFER);
    #endif
                    return 1;
                }

                if (ptrCanQue->ucMode & CAN_RX_BUF_FULL) {               // if the buffer contains data
                    *ptBuffer = CAN_MSG_RX;                              // status
                    ptrCanQue->ucMode &= (~CAN_RX_BUF_FULL);             // buffer is now free for next message
                    return (fnExtractMessage(&can_rx_mailboxes[Channel][i], (ptBuffer + 1), ucCommand) + 1); // extract the message
                }
            }
            ptrCanQue++;
        }
    }
    return 0;                                                            // no data was found
}
#endif


/************************************************************************************************************/

#if defined SDCARD_SUPPORT && defined SD_CONTROLLER_AVAILABLE

    #if defined SD_CONTROLLER_DMA
static unsigned long *ulTempBuffer = 0;                                  // intermediate buffer used by DMA when the it can't work with the user's buffer directly
    #endif


extern void fnSetSD_clock(unsigned long ulSpeed)
{
    _EXCEPTION("TO DO!!");
#if 0
    SDHC_SYSCTL &= ~SDHC_SYSCTL_SDCLKEN;                                 // stop clock to set frequency
    SDHC_SYSCTL = (SDHC_SYSCTL_DTOCV_227 | ulSpeed);                     // set new speed
    while (!(SDHC_PRSSTAT & SDHC_PRSSTAT_SDSTB)) {                       // wait for new speed to stabilise
#if defined _WINDOWS
        SDHC_PRSSTAT |= SDHC_PRSSTAT_SDSTB;
#endif
    }
    fnDelayLoop(50);                                                     // the SDHC_PRSSTAT_SDSTB doesn't seem to behave correctly so ensure with a delay
    SDHC_SYSCTL = (SDHC_SYSCTL_SDCLKEN | SDHC_SYSCTL_DTOCV_227 | ulSpeed); // enable clock
#endif
}


// Initialise the SD controllers interface with low speed clock during initailiation sequence
//
extern void fnInitSDCardInterface(void)
{
    POWER_UP(APB2, RCC_APB2ENR_SDIOEN);                                  // power up the SDIO controller

    // Configure the pins for the hardware
    //
    _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_FSMC_SDIO_OTG), (PORTC_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // SDIO_CK
    _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_FSMC_SDIO_OTG), (PORTD_BIT2),  (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // SDIO_CMD
    _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_FSMC_SDIO_OTG), (PORTC_BIT8 | PORTC_BIT9 | PORTC_BIT10 | PORTC_BIT11), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // SDIO_D0..D3    
    SDIO_CLKCR = (SDIO_POWER_HWFC_EN | SDIO_POWER_POSEDGE | SDIO_POWER_BUS_1BIT | SDIO_POWER_PWRSAV | (((PLL_VCO_FREQUENCY/PLL_Q_VALUE)/300000) - 2)); // set about 300kHz
}


// Send a command to the SD-card and return a result, plus optional returned arguments
//
extern int fnSendSD_command(const unsigned char ucCommand[6], unsigned char *ucResult, unsigned char *ptrReturnData)
{
    #define MAX_POLL_COUNT   100000
    #define MAX_WAIT_COUNT   20
    static int iCommandYieldCount = 0;
    static int iCommandState = 0;
    int iPollCount = 0;
    int iRtn = 0;


    if (iCommandYieldCount > MAX_POLL_COUNT) {                           // the present access is taking too long - quit with SD card error
    #if !defined UTFAT_DISABLE_DEBUG_OUT
        fnDebugMsg("TIMEOUT!!\r\n");
    #endif
        iRtn = ERROR_CARD_TIMEOUT;
    }
    else {
        switch (iCommandState) {
        case 0:
            SDIO_ARG = ((ucCommand[1] << 24) | (ucCommand[2] << 16) | (ucCommand[3] << 8) | ucCommand[4]); // set the argument of the command
            switch (ucCommand[0]) {                                      // case on command type
            case GO_IDLE_STATE_CMD0:
                SDIO_CMD  = ((ucCommand[0] & SDIO_CMD_CMDINDEX_MASK) | SDIO_CMD_NO_RESPONSE | SDIO_CMD_NO_WAIT | SDIO_CMD_CPSMEN); // launch the command (no response)
    #if defined _WINDOWS
                SDIO_STA = SDIO_STA_CMDSENT;                             // since there is no response, just set the sent flag
    #endif
                break;
            case SEND_CSD_CMD9:
            case SEND_CID_CMD2:
                SDIO_CMD  = ((ucCommand[0] & SDIO_CMD_CMDINDEX_MASK) | SDIO_CMD_LONG_RESPONSE | SDIO_CMD_NO_WAIT | SDIO_CMD_CPSMEN); // launch the command with long response
    #if defined _WINDOWS
                SDIO_STA = SDIO_STA_CMDREND;                             // since there is no response, just set the sent flag
    #endif
                break;
            case WRITE_BLOCK_CMD24:                                      // single sector write
                SDIO_CMD = ((ucCommand[0] & SDIO_CMD_CMDINDEX_MASK) | SDIO_CMD_CPSMEN | SDIO_CMD_NO_WAIT | SDIO_CMD_SHORT_RESPONSE);
    #if defined _WINDOWS
                SDIO_STA = SDIO_STA_CMDREND;
    #endif
                break;
            case READ_SINGLE_BLOCK_CMD17:                                // single sector read
                SDIO_DTIMER = 0xffffffff;                                            // set data timeout
                SDIO_DLEN = 512;
                SDIO_DCTRL = (SDIO_DCTRL_DTDIR_READ | SDIO_DCTRL_DBLOCKSIZE_512 | SDIO_DCTRL_DTMODE_BLOCK | SDIO_DCTRL_DTEN);

                SDIO_CMD = ((ucCommand[0] & SDIO_CMD_CMDINDEX_MASK) | SDIO_CMD_CPSMEN | SDIO_CMD_NO_WAIT | SDIO_CMD_SHORT_RESPONSE);
    #if defined _WINDOWS
                SDIO_STA = SDIO_STA_CMDREND;
    #endif
                break;
            default:
                SDIO_CMD  = ((ucCommand[0] & SDIO_CMD_CMDINDEX_MASK) | SDIO_CMD_SHORT_RESPONSE | SDIO_CMD_NO_WAIT | SDIO_CMD_CPSMEN); // launch the command (short response)
    #if defined _WINDOWS
                if (SEND_OP_COND_ACMD_CMD41 == ucCommand[0]) {
                    SDIO_STA = (SDIO_STA_CMDREND | SDIO_STA_CCRFAIL);    // this response has no valid CRC
                }
                else {
                    SDIO_STA = SDIO_STA_CMDREND;                         // set the successful response flag
                }
    #endif
                break;
            }
    #if defined _WINDOWS
            _fnSimSD_write((unsigned char)(ucCommand[0] | 0x40));_fnSimSD_write(ucCommand[1]);_fnSimSD_write(ucCommand[2]);_fnSimSD_write(ucCommand[3]);_fnSimSD_write(ucCommand[4]);_fnSimSD_write(0);_fnSimSD_write(0xff);
    #endif
        case 6:
            do {                                                         // loop until the command has been transferred
                if (SDIO_STA & SDIO_STA_CTIMEOUT) {                      // timeout occurred while waiting for command to complete
                    SDIO_ICR = SDIO_STA_CTIMEOUT;                        // clear flag
                    iCommandState = 0;                                   // allow resend of command
                    iCommandYieldCount++;
                    uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_GO);   // switch to polling mode of operation
                    return CARD_BUSY_WAIT;
                }
                if (SDIO_STA & (SDIO_STA_CMDSENT | SDIO_STA_CMDREND | SDIO_STA_CCRFAIL)) { // command has completed
                    iCommandYieldCount = 0;
                    if (GO_IDLE_STATE_CMD0 == ucCommand[0]) {            // this command receives no response
                        *ucResult = R1_IN_IDLE_STATE;                    // dummy response for compatibility
                    }
                    else if ((READ_SINGLE_BLOCK_CMD17 == ucCommand[0]) || (WRITE_BLOCK_CMD24 == ucCommand[0])) {
                        *ucResult = 0;                                   // for compatibility
                    }
                    else {
                        if (SDIO_STA & SDIO_STA_CCRFAIL) {               // CRC error detected
                            if (ucCommand[0] == SEND_OP_COND_ACMD_CMD41) { // this responds with command value 0x3f and CRC-7 0xff
                                *ucResult = 0;                           // for compatibility
                            }
                            else {                                       // other CRC errors cause retry
                                *ucResult = R1_COMMAND_CRC_ERROR;
                            }
                            SDIO_ICR = (SDIO_STA_CMDSENT | SDIO_STA_CMDREND | SDIO_STA_CCRFAIL | SDIO_STA_STBIRERR); // clear flags
                            break;
                        }

                        if (!(SDIO_STA & (0 /* ?? */))) { // if matching response and no error
                            switch (ucCommand[0]) {
                            case SEND_IF_COND_CMD8:
                                *ucResult = SDC_CARD_VERSION_2PLUS;      // for compatibility
                                break;
                            case APP_CMD_CMD55:
                                *ucResult = R1_IN_IDLE_STATE;            // for compatibility
                                break;
                            default:
                                *ucResult = 0;                           // for compatibility
                                break;
                            }
                        }
                    }
                    SDIO_ICR = (SDIO_STA_CMDSENT | SDIO_STA_CMDREND | SDIO_STA_CCRFAIL | SDIO_STA_STBIRERR); // clear flags
                    break;
                }
                else {                                                   // the command/response is still being executed
                    iCommandState = 6;                                   // waiting for completion
                    if (++iPollCount >= MAX_WAIT_COUNT) {                // count the loops polled
                        iCommandYieldCount++;
                        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_GO); // switch to polling mode of operation                        
                        return CARD_BUSY_WAIT;                           // yield after short polling period
                    }
                }
            } FOREVER_LOOP();
            if (ptrReturnData != 0) {                                    // if the caller requests data, read it here
    #if defined _WINDOWS
                _fnSimSD_write(0xff);_fnSimSD_write(0xff);               // for compatibility
                SDIO_RESP1  = (_fnSimSD_write(0xff) << 24);
                SDIO_RESP1 |= (_fnSimSD_write(0xff) << 16);
                SDIO_RESP1 |= (_fnSimSD_write(0xff) << 8);
                SDIO_RESP1 |=  _fnSimSD_write(0xff);
    #endif
                *ptrReturnData++ = (unsigned char)(SDIO_RESP1 >> 24);
                *ptrReturnData++ = (unsigned char)(SDIO_RESP1 >> 16);
                *ptrReturnData++ = (unsigned char)(SDIO_RESP1 >> 8);
                *ptrReturnData   = (unsigned char)SDIO_RESP1;
                if ((ucCommand[0] == SEND_CSD_CMD9) || (ucCommand[0] == SEND_CID_CMD2)) { // exception requiring 16 bytes
    #if defined _WINDOWS
                    SDIO_RESP2  = (_fnSimSD_write(0xff) << 24);
                    SDIO_RESP2 |= (_fnSimSD_write(0xff) << 16);
                    SDIO_RESP2 |= (_fnSimSD_write(0xff) << 8);
                    SDIO_RESP2 |=  _fnSimSD_write(0xff);
    #endif
                    ptrReturnData++;
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP2 >> 24);
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP2 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP2 >> 8);
                    *ptrReturnData++ = (unsigned char)SDIO_RESP2;
    #if defined _WINDOWS
                    SDIO_RESP3 = (_fnSimSD_write(0xff) << 24);
                    SDIO_RESP3 |= (_fnSimSD_write(0xff) << 16);
                    SDIO_RESP3 |= (_fnSimSD_write(0xff) << 8);
                    SDIO_RESP3 |=  _fnSimSD_write(0xff);
    #endif
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP3 >> 24);
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP3 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP3 >> 8);
                    *ptrReturnData++ = (unsigned char)SDIO_RESP3;
    #if defined _WINDOWS
                    SDIO_RESP4 = (_fnSimSD_write(0xff) << 24);
                    SDIO_RESP4 |= (_fnSimSD_write(0xff) << 16);
                    SDIO_RESP4 |= (_fnSimSD_write(0xff) << 8);
                    SDIO_RESP4 |=  _fnSimSD_write(0xff);
    #endif
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP4 >> 24);
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP4 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDIO_RESP4 >> 8);
                    *ptrReturnData   = (unsigned char)SDIO_RESP4;
                }
            }
            break;
        }
    }
    if (iCommandYieldCount != 0) {                                       // process yielded so switch back to event-driven mode
        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_STOP);             // switch to event mode of operation since write has completed
        iCommandYieldCount = 0;
    }
    iCommandState = 0;
    return iRtn;
}


// Read a sector from SD card into the specified data buffer
//
extern int fnGetSector(unsigned char *ptrBuf)
{
    unsigned long *ptrData = (unsigned long *)ptrBuf;                    // the STM32 driver ensures that the buffer is long word aligned
    int i = (512/sizeof(unsigned long));
    #if defined _WINDOWS
    _fnSimSD_write(0xff);                                                // dummy for simulator compatibility
    #endif
    while (i != 0) {
        if (SDIO_STA & (SDIO_STA_RXOVERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT | SDIO_STA_STBIRERR | SDIO_STA_CTIMEOUT)) { // check for read errors
            SDIO_ICR = (SDIO_STA_RXOVERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT | SDIO_STA_STBIRERR | SDIO_STA_CTIMEOUT);
            return UTFAT_DISK_READ_ERROR;                                // return error
        }
        if (!(SDIO_STA & SDIO_STA_RXFIFOE)) {                            // if data to be read
    #if defined _WINDOWS
            SDIO_FIFO0 =   _fnSimSD_write(0xff);
            SDIO_FIFO0 |= (_fnSimSD_write(0xff) << 8);
            SDIO_FIFO0 |= (_fnSimSD_write(0xff) << 16);
            SDIO_FIFO0 |= (_fnSimSD_write(0xff) << 24);
    #endif
            *ptrData++ = SDIO_FIFO0;
            i--;
        }
    }
    #if defined _WINDOWS
    _fnSimSD_write(0xff);_fnSimSD_write(0xff);                           // dummy for simulator compatibility
    #endif
    SDIO_ICR = (SDIO_STA_RXDAVL | SDIO_STA_RXFIFOF | SDIO_STA_RXFIFOHE | SDIO_STA_RXACT | SDIO_STA_BDCKEND | SDIO_STA_DATAEND); // reset flags
    return UTFAT_SUCCESS;                                                // read successfully terminated
}

// Read a specified amount of data from present SD card sector into the specified data buffer (usStart and usStop are offset from start of sector and avoid other data outside of this range being overwritted)
//
extern int fnReadPartialSector(unsigned char *ptrBuf, unsigned short usStart, unsigned short usStop)
{
    int iReadState;
    unsigned long ulTempBuffer[512/sizeof(unsigned long)];               // since the SD controller will always read a complete sector we use an intermediate buffer
    if ((iReadState = fnGetSector((unsigned char *)ulTempBuffer)) == UTFAT_SUCCESS) { // fill the temporary buffer with the sector data
        uMemcpy(ptrBuf, (((unsigned char *)ulTempBuffer) + usStart), (usStop - usStart)); // just copy the part required
    }
    return iReadState;
}

// Write present sector with buffer data
//
extern int fnPutSector(unsigned char *ptrBuf)
{
    unsigned long *ptrData = (unsigned long *)ptrBuf;                    // the STM32 driver ensures that the buffer is long word aligned
    int i = (512/sizeof(unsigned long));
    SDIO_DTIMER = 0xffffffff;                                            // set data timeout
    SDIO_DLEN = 512;
    #if defined _ERRATE_REV_A_Z                                          // avoid hardware flow control 
    SDIO_CLKCR &= ~(SDIO_POWER_HWFC_EN);                                 // disable hardware flow control to avoid possible glitches which can result in CRC errors
    uDisable_Interrupt();                                                // to avoid potential transmitter underruns disable interrupts for the copy
    #endif
    SDIO_DCTRL = (SDIO_DCTRL_DTDIR_WRITE | SDIO_DCTRL_DBLOCKSIZE_512 | SDIO_DCTRL_DTMODE_BLOCK | SDIO_DCTRL_DTEN);
    #if defined _WINDOWS
    _fnSimSD_write(0xfe);                                                // dummy for simulator compatibility
    #endif
    while (i != 0) {
        if (SDIO_STA & (SDIO_STA_TXUNDERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT | SDIO_STA_STBIRERR)) { // check for write errors
            SDIO_ICR = (SDIO_STA_TXUNDERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT | SDIO_STA_STBIRERR);
    #if defined _ERRATE_REV_A_Z
            SDIO_CLKCR |= (SDIO_POWER_HWFC_EN);                          // enable hardware flow control again
            uEnable_Interrupt();
    #endif
            return UTFAT_DISK_WRITE_ERROR;                               // return error
        }
        if (!(SDIO_STA & SDIO_STA_TXFIFOF)) {                            // if space in transmit FIFO
            SDIO_FIFO0 = *ptrData++;
    #if defined _WINDOWS
            _fnSimSD_write((unsigned char)SDIO_FIFO0);
            _fnSimSD_write((unsigned char)(SDIO_FIFO0 >> 8));
            _fnSimSD_write((unsigned char)(SDIO_FIFO0 >> 16));
            _fnSimSD_write((unsigned char)(SDIO_FIFO0 >> 24));
    #endif
            i--;
        }
    }
    #if defined _ERRATE_REV_A_Z
    SDIO_CLKCR |= (SDIO_POWER_HWFC_EN);                                  // enable hardware flow control again
    uEnable_Interrupt();
    #endif
    #if defined _WINDOWS
    _fnSimSD_write(0xff);_fnSimSD_write(0xff);_fnSimSD_write(0xff);      // dummy for simulator compatibility
    #endif
    while (SDIO_STA & SDIO_STA_TXACT) {}                                 // while data transfer still in progress
  //while ((SDIO_STA & SDIO_STA_TXDAVL) != 0) {}                         // wait for all data to be transmitted
    SDIO_ICR = (SDIO_STA_RXDAVL | SDIO_STA_RXFIFOF | SDIO_STA_RXFIFOHE | SDIO_STA_RXACT | SDIO_STA_BDCKEND | SDIO_STA_DATAEND | SDIO_STA_DCRCFAIL); // reset flags
    return UTFAT_SUCCESS;                                                // write successfully terminated
}
#endif


/************************************************************************************************************/



#if defined SUPPORT_RTC

static void (*rtc_interrupt_handler[6])(void) = {0};                     // RTC interrupt handlers

static int rtc_interrupts = 0;
static CHAR rtc_stopwatch = -1;

// Virtual interupts with compatibility with Coldfire RTCs
//
#define RTC_STOPWATCH     0x00000001                                     // stopwatch timer out interrupt
#define RTC_MINUTE_INT    0x00000002                                     // minute interrupt
#define RTC_ALARM_INT     0x00000004                                     // alarm interrupt
#define RTC_DAY_INT       0x00000008                                     // day interrupt
#define RTC_SECOND_INT    0x00000010                                     // second interrupt
#define RTC_HOUR_INT      0x00000020                                     // hour interrupt


// Presently handles seconds interrupts only
//
static __interrupt void _rtc_handler(void)
{
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    register unsigned long ulRegister1 = RTC_TR;
    register unsigned long ulRegister2 = RTC_DR;
    #if defined _WINDOWS
    EXTI_PR &= ~0x00020000;                                              // clear interrupt source
    #else
    EXTI_PR = 0x00020000;                                                // clear interrupt source
    #endif
    if (RTC_ISR & RTC_ISR_ALRAF) {                                       // if seconds interrupt
        if ((ulRegister1 & (RTC_TR_SU | RTC_TR_ST)) == 0) {              // minute passed
            if ((ulRegister1 & (RTC_TR_HU | RTC_TR_HT)) == 0) {          // day passed
                if (rtc_interrupts & RTC_DAY_INT) {                      // days interrupt enabled
                    if (rtc_interrupt_handler[3] != 0) {                 // days interrupt call back
                        uDisable_Interrupt();
                        rtc_interrupt_handler[3]();                      // call handling function (days interrupt)
                        uEnable_Interrupt();
                    }
                }
            }
            if ((ulRegister1 & (RTC_TR_MNU | RTC_TR_MNT)) == 0) {        // hour passed
                if (rtc_interrupts & RTC_HOUR_INT) {                     // hours interrupt enabled
                    if (rtc_interrupt_handler[5] != 0) {                 // hours interrupt call back
                        uDisable_Interrupt();
                        rtc_interrupt_handler[5]();                      // call handling function (hours interrupt)
                        uEnable_Interrupt();
                    }
                }
            }
            if (rtc_interrupts & RTC_MINUTE_INT) {                       // minutes interrupt enabled
                if (rtc_interrupt_handler[1] != 0) {                     // minutes interrupt call back
                    uDisable_Interrupt();
                    rtc_interrupt_handler[1]();                          // call handling function (minutes interrupt)
                    uEnable_Interrupt();
                }
            }
            if (rtc_stopwatch >= 0) {
                rtc_stopwatch--;
                if (rtc_interrupts & RTC_STOPWATCH) {                    // stopwatch interrupt enabled
                    if (rtc_stopwatch == -1) {
                        if (rtc_interrupt_handler[0] != 0) {             // stopwatch interrupt call back
                            uDisable_Interrupt();
                            rtc_interrupt_handler[0]();                  // call handling function (minutes interrupt)
                            uEnable_Interrupt();
                        }
                    }
                }
            }
        }
        if (rtc_interrupt_handler[4] != 0) {                             // seconds interrupt call back
            uDisable_Interrupt();
            rtc_interrupt_handler[4]();                                  // call handling function (seconds interrupt)
            uEnable_Interrupt();
        }
    }
    if ((RTC_ISR & RTC_ISR_ALRBF) && (rtc_interrupts & RTC_ALARM_INT)) { // alarm date / time match    
        if (rtc_interrupt_handler[2] != 0) {
            uDisable_Interrupt();
            rtc_interrupt_handler[2]();                                  // call handling function (alarm match interrupt)
            uEnable_Interrupt();
        }
    }
    RTC_ISR = 0;                                                         // clear flags
#else
#endif
}

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
static void fnSetNextSecAlarm(unsigned long ulRegisterValue)
{
    ulRegisterValue &= (RTC_TR_SU | RTC_TR_ST);
    ulRegisterValue++;                                                   // add 1 second
    if ((ulRegisterValue & RTC_TR_SU) > 9) {                             // overflow to 10s
        ulRegisterValue &= ~(RTC_TR_SU);
        ulRegisterValue += 0x00000010;                                   // add tens
        if ((ulRegisterValue >> 4) > 5) {                                // check for tens overflow
            ulRegisterValue &= ~(RTC_TR_ST);
        }
    }
    RTC_ALRMAR = (ulRegisterValue | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4); // set alarm interrupt on next second count
}
#endif


// This routine attempts a high level of compatibility with the Coldfire V2 RTC - it presently handles only seconds interrupt
//
extern int fnConfigureRTC(void *ptrSettings)
{
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    register unsigned long ulRegisterValue;
    #endif
    int iIRQ = 0;
    RTC_SETUP *ptr_rtc_setup = (RTC_SETUP *)ptrSettings;
    switch (ptr_rtc_setup->command & ~(RTC_DISABLE | RTC_INITIALISATION)) {
    case RTC_TIME_SETTING:                                               // set time to RTC
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        RTC_WPR = RTC_WPR_KEY1; RTC_WPR = RTC_WPR_KEY2;                  // enable RTC register writes
        RTC_ISR = (RTC_ISR_INIT);                                        // temporarily stop counting
        RTC_CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRAIE);                       // temporarily disable alarm
        ulRegisterValue = (((ptr_rtc_setup->ucSeconds/10) << 4) | (ptr_rtc_setup->ucSeconds%10) | ((ptr_rtc_setup->ucMinutes%10) << 8) | ((ptr_rtc_setup->ucMinutes/10) << 12) | ((ptr_rtc_setup->ucHours%10) << 16) | ((ptr_rtc_setup->ucHours/10) << 20));
        while (!(RTC_ISR & RTC_ISR_INITF)) {                             // wait until the initialisation mode is entered (2 RTCCLK cycles)
        #if defined _WINDOWS
            RTC_ISR = (RTC_ISR_INIT | RTC_ISR_INITF);
        #endif
        }
        RTC_SSR = 0;
        RTC_TR = ulRegisterValue;                                        // set time
        if (RTC_CR & (RTC_CR_ALRAIE)) {                                  // if seconds interrupt enabled
            fnSetNextSecAlarm(ulRegisterValue);                          // set alarm interrupt after 1 second
        }
        ulRegisterValue = (((ptr_rtc_setup->ucDayOfMonth/10) << 4) | (ptr_rtc_setup->ucDayOfMonth%10) | ((ptr_rtc_setup->ucMonthOfYear%10) << 8) | ((ptr_rtc_setup->ucMonthOfYear/10) << 12) | (ptr_rtc_setup->ucDayOfWeek << 13) | (((ptr_rtc_setup->usYear - 2000)%10) << 16) | (((ptr_rtc_setup->usYear - 2000)/10) << 20));
        RTC_DR = ulRegisterValue;                                        // set date
        RTC_ISR = 0;                                                     // start counting again
        RTC_WPR = 0xff;                                                  // re-enable RTC write protection
    #else
    #endif
        break;

    case RTC_GET_TIME:                                                   // get the present time
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        ulRegisterValue = RTC_TR;                                        // higher order shadow register are locked until read
        ptr_rtc_setup->ucSeconds = (unsigned char)((ulRegisterValue & RTC_TR_SU) + (((ulRegisterValue & RTC_TR_ST) >> 4) * 10));
        ptr_rtc_setup->ucMinutes = (unsigned char)(((ulRegisterValue & RTC_TR_MNU) >> 8) + (((ulRegisterValue & RTC_TR_MNT) >> 12) * 10));
        ptr_rtc_setup->ucHours = (unsigned char)(((ulRegisterValue & RTC_TR_HU) >> 16) + (((ulRegisterValue & RTC_TR_HT) >> 20) * 10));
        ulRegisterValue = RTC_DR;                                        // this unlocks the shadow register operation
        ptr_rtc_setup->ucDayOfMonth = (unsigned char)((ulRegisterValue & RTC_DR_DU) + (((ulRegisterValue & RTC_DR_DT) >> 4) * 10));
        ptr_rtc_setup->ucDayOfWeek = (unsigned char)((ulRegisterValue & RTC_DR_WDU) >> 13);
        ptr_rtc_setup->ucMonthOfYear = (unsigned char)(((ulRegisterValue & RTC_DR_MU) >> 8) + (((ulRegisterValue & RTC_DR_MT) >> 12) * 10));
        ptr_rtc_setup->usYear = (unsigned short)(2000 + (((ulRegisterValue & RTC_DR_YU) >> 16) + (((ulRegisterValue & RTC_DR_YT) >> 20) * 10)));
    #else
    #endif
        break;

    case RTC_TICK_HOUR:                                                  // interrupt on each hour
        iIRQ++;
    case RTC_TICK_SEC:                                                   // interrupt on each second
        iIRQ++;
    case RTC_TICK_DAY:                                                   // interrupt on each day
        iIRQ++;
    case RTC_ALARM_TIME:                                                 // interrupt at specific date and time
        if (iIRQ == 0) {                                                 // RTC_ALARM_TIME
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
            RTC_WPR = RTC_WPR_KEY1; RTC_WPR = RTC_WPR_KEY2;              // enable RTC register writes
            RTC_CR &= ~(RTC_CR_ALRBE | RTC_CR_ALRBIE);                   // ensure that alarm is not enabled
            rtc_interrupts &= ~RTC_ALARM_INT;                            // disable ALARM interrupt
            ulRegisterValue = (((ptr_rtc_setup->ucSeconds/10) << 4) | (ptr_rtc_setup->ucSeconds%10) | ((ptr_rtc_setup->ucMinutes/10) << 8) | ((ptr_rtc_setup->ucMinutes%10) << 12) | ((ptr_rtc_setup->ucHours/10) << 16) | ((ptr_rtc_setup->ucHours%10) << 20) | ((ptr_rtc_setup->ucHours/10) << 24) | ((ptr_rtc_setup->ucHours%10) << 28));
            RTC_ALRMBR = ulRegisterValue;                                // set the alarm for a certain time on a certain day of month
            RTC_CR |= (RTC_CR_ALRBE | RTC_CR_ALRBIE);                    // enable the alarm and interrupt
    #endif
        }
        iIRQ++;
    case RTC_TICK_MIN:                                                   // interrupt each minute
        iIRQ++;
    case RTC_STOPWATCH_GO:                                               // interrupt after a certain number of minutes
        if (iIRQ == 0) {                                                 // RTC_STOPWATCH_GO            
            rtc_stopwatch = (CHAR)((ptr_rtc_setup->ucMinutes - 1) & 0x3f); // set a stop watch interrupt after this many additional minutes (1 to 64 minutes as for the Coldfire)
        }
        if (RTC_DISABLE & (ptr_rtc_setup->command)) {
            rtc_interrupts &= ~(0x01 << iIRQ);                           // disable interrupt
            return 0;                                                    // disable function rather than enable
        }
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        if (ptr_rtc_setup->command & RTC_INITIALISATION) {
            POWER_UP(APB1, (RCC_APB1ENR_PWREN));                         // ensure that the power control module is enabled
            PWR_CR |= PWR_CR_DBP;                                        // disable backup domain write protection
            if ((RTC_ISR & RTC_ISR_INITS) == 0) {                        // if the calendar has not yet been initialised (this in fact checks whether the year file is still at its default 0 value - which is the year 2000 - meaning that 2000 should never be set for this to be accurate)
                if (!(RCC_BDCR & RCC_BDCR_LSEON)) {                      // if the external oscillator hasn't been enabled yet
                    RCC_BDCR |= RCC_BDCR_BDRST;                          // reset backup domain
                    RCC_BDCR &= ~(RCC_BDCR_BDRST | RCC_BDCR_LSEON | RCC_BDCR_LSEBYP); // remove reset
                    RCC_BDCR |= RCC_BDCR_LSEON;                          // enable the external oscillator
                }
                if (!(RCC_BDCR & RCC_BDCR_LSERDY)) {                     // wait for oscillator to stabilise
        #if defined _WINDOWS
                    RCC_BDCR |= (RCC_BDCR_LSERDY);
        #endif
                    return WAIT_STABILISING_DELAY;                       // it typically takes 2s before the oscillator has stabilised when first enabled
                }
                RCC_BDCR = ((RCC_BDCR & ~RCC_BDCR_RTCSEC_MASK) | (RCC_BDCR_RTCSEC_LSE | RCC_BDCR_RTCEN)); // external 32.768 crystal oscillator as source
                RTC_WPR = RTC_WPR_KEY1; RTC_WPR = RTC_WPR_KEY2;          // enable RTC register writes
                RTC_ISR = (RTC_ISR_INIT);                                // enter initialisation mode
                while (!(RTC_ISR & RTC_ISR_INITF)) {                     // wait until the initialisation mode is entered (2 RTCCLK cycles)
        #if defined _WINDOWS
                    RTC_ISR = (RTC_ISR_INIT | RTC_ISR_INITF);
        #endif
                }
                RTC_PRER = ((((32768/512) - 1) << RTC_PRER_ASYN_SHIFT) | ((512 - 1))); // set 1Hz from external oscillator
                RTC_PRER = ((((32768/512) - 1) << RTC_PRER_ASYN_SHIFT) | ((512 - 1))); // set 1Hz from external oscillator - two writes are required
                RTC_TR = 0;
                RTC_DR = 0x00112101;                                     // time and date register set to 2011 as default (this allows it to be recognised as configured)
                RTC_CR = (RTC_CR_FMT_24_HOUR);
                RTC_ISR = 0;                                             // exist initialisation mode - the counting starts after 4 RTCCLK cycles
                RTC_WPR = 0xff;                                          // re-enable RTC write protection
            }
        }
        rtc_interrupt_handler[iIRQ] = ptr_rtc_setup->int_handler;        // enter the user's interrupt call-back
        if (ptr_rtc_setup->int_handler) {                                // if one has been defined configure a 1s interrupt
            RTC_ISR = (RTC_ISR_INIT);                                    // temporarily stop counting
            RTC_CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRAIE);                   // temporarily disable alarm
            while (!(RTC_ISR & RTC_ISR_INITF)) {                         // wait until the initialisation mode is entered (2 RTCCLK cycles)
        #if defined _WINDOWS
                RTC_ISR = (RTC_ISR_INIT | RTC_ISR_INITF);
        #endif
            }
            fnEnterInterrupt(irq_RTCAlarm_ID, PRIORITY_RTC, _rtc_handler); // enter interrupt handler
            rtc_interrupts |= (0x01 << iIRQ);                            // flag interrupt(s) enabled
            while (!(RTC_ISR & RTC_ISR_RSF)) {                           // ensure that the shadow registers are synchronised before reading present time
        #if defined _WINDOWS
                RTC_ISR |= RTC_ISR_RSF;
        #endif
            }
            fnSetNextSecAlarm(RTC_TR);
            EXTI_RTSR |= 0x00020000;                                     // enable rising edge trigger of EXTI17 (which RTC interrupt arrives on)
            EXTI_IMR |= 0x00020000;
            RTC_CR |= (RTC_CR_ALRAE | RTC_CR_ALRAIE);                    // enable the alarm and interrupt
            RTC_ISR = 0;                                                 // start counting again
        }
    #else
    #endif
        break;
    }
    return 0;
}

    #if defined SDCARD_SUPPORT && defined SUPPORT_FILE_TIME_STAMP
// This routine supports date and time stamping of files and directories. If the information is not available it could return 1
// so that a fixed stamp is set. It is assumed that the RTC in the STM32 will be used if the support is activated
//
extern int fnGetLocalFileTime(unsigned short *ptr_usCreationTime, unsigned short *ptr_usCreationDate)
{
    RTC_SETUP rtc_setup;
    rtc_setup.command = RTC_GET_TIME;
    fnConfigureRTC(&rtc_setup);                                          // get the present time and date

    if (rtc_setup.usYear < (2012 - 2000)) {                              // if the year is invalid report that there is no valid time/date
        return 1;                                                        // utFAT will take fixed timestamp instead
    }
    *ptr_usCreationTime = (rtc_setup.ucSeconds | (rtc_setup.ucMinutes << 5) | (rtc_setup.ucHours << 11));
    *ptr_usCreationDate = (rtc_setup.ucDayOfMonth | (rtc_setup.ucMonthOfYear << 5) | ((rtc_setup.usYear + 2000) << 9));
    return 0;                                                            // successful
}
    #endif
#endif


// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
#if defined SUPPORT_PORT_INTERRUPTS
    case PORT_INTERRUPT:
    #define _PORT_INT_CONFIG_CODE
        #include "stm32_PORTS.h"                                         // include port interrupt configuration code
    #undef _PORT_INT_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_TIMER 
    case TIMER_INTERRUPT:
    #define _TIMER_CONFIG_CODE
        #include "stm32_TIMER.h"                                         // include timer configuration code
    #undef _TIMER_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_ADC                                                  // {36}
    case ADC_INTERRUPT:
    #define _ADC_CONFIG_CODE
        #include "stm32_ADC.h"                                           // include ADC configuration code
    #undef _ADC_CONFIG_CODE
        break;
#endif
    default:
        _EXCEPTION("Attempting configuration of interrupt interface without appropriate support enabled!!");
        break;
    }
}


#if defined SUPPORT_LOW_POWER
// This routine switches to low power mode. It is always called with disabled interrupts
// - when the routine is called there were no system events pending so the system can be set to a low power mode until a masked interrupt becomes pending
//
extern void fnDoLowPower(void)
{
    #if !defined _WINDOWS
    __sleep_mode();                                                      // enter low power mode using wait for interrupt processor state
    #endif
    uEnable_Interrupt();                                                 // enable interrupts so that the masked interrupt that became pending can be taken
}
#endif

// Determine the present low power mode that is configured (not yet implemented)
//
extern int fnGetLowPowerMode(void)
{
    return 0;
}

// Set a new low power mode (not yet implemented)
//
extern void fnSetLowPowerMode(int new_lp_mode)
{
}

// This routine is called to reset the card - it uses the sysreset
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);
#if !defined _WINDOWS
    while ((int)1 != (int)0) {}
#endif
}


#if defined DMA_MEMCPY_SET && !defined DEVICE_WITHOUT_DMA
#define SMALLEST_DMA_COPY 20                                             // smaller copies have no DMA advantage
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr1 = (unsigned char *)ptrTo;
    unsigned char *ptr2 = (unsigned char *)ptrFrom;
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= 0xffff) && (DMA2_S0CR == 0)) { // {27} if large enough to be worth while and if not already in use
        unsigned short usTransferSize;
        while (((unsigned long)ptr1) & 0x3) {                            // move to a long word boundary (the source is not guaranteed to be on a boundary, which can make the lomng word copy less efficient)
            *ptr1++ = *ptr2++;
            Size--;
        }
        //  DMA2 is used since DMA1 cannot perform memory to memory transfers
        //
        DMA2_S1PAR = (unsigned long)ptr2;                                // address of source
        DMA2_S1M0AR = (unsigned long)ptr1;                               // address of destination
        if (((unsigned long)ptr2 & 0x3) == 0) {                          // if both source and destination are long word aligned
            usTransferSize = ((unsigned short)(Size/sizeof(unsigned long))); // the number of long words to transfer by DMA
            DMA2_S1NDTR = usTransferSize;                                // the number of byte transfers to be made (max. 0xffff)
            DMA2_S1CR = (DMA_SxCR_PINC | DMA_SxCR_MINC | DMA_SxCR_PSIZE_32 | DMA_SxCR_MSIZE_32 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
            usTransferSize *= sizeof(unsigned long);                     // the number of bytes being transferred by the DMA process
        }
        else if (((unsigned long)ptr2 & 0x1) == 0) {                     // if both source and destination are short word aligned
            usTransferSize = ((unsigned short)(Size/sizeof(unsigned short))); // the number of short words to transfer by DMA
            DMA2_S1NDTR = usTransferSize;                                // the number of byte transfers to be made (max. 0xffff)
            DMA2_S1CR = (DMA_SxCR_PINC | DMA_SxCR_MINC | DMA_SxCR_PSIZE_16 | DMA_SxCR_MSIZE_16 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
            usTransferSize *= sizeof(unsigned short);                    // the number of bytes being transferred by the DMA process
        }
        else {
        #if defined _WINDOWS
            if (Size > 0xffff) {
                _EXCEPTION("DMA transfer doesn't support more than 64k!!");
            }
        #endif
            usTransferSize = (unsigned short)Size;
            DMA2_S1NDTR = usTransferSize;                                // the number of byte transfers to be made (max. 0xffff)
            DMA2_S1CR = (DMA_SxCR_PINC | DMA_SxCR_MINC | DMA_SxCR_PSIZE_8 | DMA_SxCR_MSIZE_8 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
        }
        DMA2_S1CR |= DMA_SxCR_EN;                                        // start operation
        ptr1 += usTransferSize;                                          // move the destination pointer to beyond the transfer
        ptr2 += usTransferSize;                                          // move the source pointer to beyond the transfer
        Size -= usTransferSize;                                          // bytes remaining
        while ((DMA2_LISR & DMA_LISR_TCIF1) == 0) { SIM_DMA(0) };        // wait until the transfer has terminated
        DMA2_LIFCR = (DMA_LISR_TCIF1 | DMA_LISR_HTIF1 | DMA_LISR_DMEIF1 | DMA_LISR_FEIFO1 | DMA_LISR_DMEIF1); // clear flags
        while (Size-- != 0) {                                            // {29}
            *ptr1++ = *ptr2++;
        }
        #if defined _WINDOWS
        DMA2_LISR = 0;
        #endif
        DMA2_S0CR = 0;                                                   // mark that the DMA stream is free for use again
    }
    #else
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= 0xffff) && (DMA_CNDTR_MEMCPY == 0)) { // if large enough to be worthwhile and if not already in use
        DMA_CNDTR_MEMCPY = ((unsigned long)(Size));                      // the number of byte transfers to be made (max 0xffff)
        DMA_CMAR_MEMCPY  = (unsigned long)ptrFrom;                       // address of first byte to be transferred
        DMA_CPAR_MEMCPY  = (unsigned long)ptrTo;                         // address of first destination byte
        DMA_CCR_MEMCPY   = (DMA1_CCR1_EN | DMA1_CCR1_PINC | DMA1_CCR1_MINC | DMA1_CCR1_PSIZE_8 | DMA1_CCR1_MSIZE_8 | DMA1_CCR1_PL_MEDIUM | DMA1_CCR1_MEM2MEM | DMA1_CCR1_DIR); // set up DMA operation and start DMA transfer
        while (DMA_CNDTR_MEMCPY != 0) { SIM_DMA(0) };                    // wait until the transfer has terminated
        DMA_CCR_MEMCPY = 0;
    }
    #endif
    else {                                                               // normal memcpy method
        while (Size--) {
            *ptr1++ = *ptr2++;
        }
  }
    return buffer; 
}

// memset implementation
//
extern void *uMemset(void *ptrTo, int iValue, size_t Size)               // {37}
{
    void *buffer = ptrTo;
    unsigned char ucValue = (unsigned char)iValue;                       // {37}
    unsigned char *ptr = (unsigned char *)ptrTo;
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= (0xffff * sizeof(unsigned long))) && (DMA2_S0CR == 0)) { // {27} if large enough to be worth while and if not already in use
        volatile unsigned long ulToCopy = (ucValue | (ucValue << 8) | (ucValue << 16) | (ucValue << 24));
        unsigned short usTransferSize;
        while (((unsigned long)ptr) & 0x3) {                             // move to a long word bounday
            *ptr++ = ucValue;
            Size--;
        }
        usTransferSize = ((unsigned short)(Size/sizeof(unsigned long))); // the number of long words to transfer by DMA
        DMA2_S1NDTR = usTransferSize;                                    // the number of long word transfers to be made (max. 0xffff)
        DMA2_S1PAR = (unsigned long)&ulToCopy;                           // address of long word to be transfered
        DMA2_S1M0AR = (unsigned long)ptr;                                // address of first destination long word
        DMA2_S1CR = (DMA_SxCR_MINC | DMA_SxCR_PSIZE_32 | DMA_SxCR_MSIZE_32 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
        DMA2_S1CR |= DMA_SxCR_EN;                                        // start operation
        usTransferSize *= sizeof(unsigned long);                         // the number of bytes being transferred by the DMA process
        ptr += usTransferSize;                                           // move the destination pointer to beyond the transfer
        Size -= usTransferSize;                                          // bytes remaining
        while (Size--) {
            *ptr++ = ucValue;
        }
        while ((DMA2_LISR & DMA_LISR_TCIF1) == 0) { SIM_DMA(0) };        // wait until the DMA transfer has terminated
        DMA2_LIFCR = (DMA_LISR_TCIF1 | DMA_LISR_HTIF1 | DMA_LISR_DMEIF1 | DMA_LISR_FEIFO1 | DMA_LISR_DMEIF1); // clear flags
        #if defined _WINDOWS
        DMA2_LISR = 0;
        #endif
        DMA2_S0CR = 0;                                                   // mark that the DMA stream is free for use again
    }
    #else
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= 0xffff) && (DMA_CNDTR_MEMCPY == 0)) { // if large enought to be worth while and if not already in use
        volatile unsigned char ucToCopy = ucValue;
        DMA_CNDTR_MEMCPY = ((unsigned long)(Size));                      // the number of byte transfers to be made (max. 0xffff)
        DMA_CMAR_MEMCPY  = (unsigned long)&ucToCopy;                     // address of byte to be transfered
        DMA_CPAR_MEMCPY  = (unsigned long)ptr;                           // address of first destination byte
        DMA_CCR_MEMCPY   = (DMA1_CCR1_EN | DMA1_CCR1_PINC | DMA1_CCR1_PSIZE_8 | DMA1_CCR1_MSIZE_8 | DMA1_CCR1_PL_MEDIUM | DMA1_CCR1_MEM2MEM | DMA1_CCR1_DIR); // set up DMA operation and start DMA transfer       
        while (DMA_CNDTR_MEMCPY != 0) { SIM_DMA(0) };                    // wait until the transfer has terminated
        DMA_CCR_MEMCPY = 0;
    }
    #endif
    else {                                                               // normal memset method
        while (Size--) {
            *ptr++ = ucValue;
        }
    }
    return buffer;
}
#endif



// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               // perform hardware initialisation
{
#if defined _WINDOWS
    unsigned short usPortPullups[] = {
        GPIO_DEFAULT_INPUT_A,                                            // set the port states out of reset in the project file app_hw_stm32.h
        GPIO_DEFAULT_INPUT_B,
        GPIO_DEFAULT_INPUT_C,
        GPIO_DEFAULT_INPUT_D,
        GPIO_DEFAULT_INPUT_E,
    #if PORTS_AVAILABLE > 5
        GPIO_DEFAULT_INPUT_F,
        GPIO_DEFAULT_INPUT_G,
        GPIO_DEFAULT_INPUT_H,
        GPIO_DEFAULT_INPUT_I,
    #endif
    #if PORTS_AVAILABLE > 9
        GPIO_DEFAULT_INPUT_J,
        GPIO_DEFAULT_INPUT_K,
    #endif
    #if defined SUPPORT_ADC                                              // {5}
        ((ADC123_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC123_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC123_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC123_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC123_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC123_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC123_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC123_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC12_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
    #endif
    };
    #if defined RANDOM_NUMBER_GENERATOR                                  // note that the target works differently
    static unsigned short usRandomSeed = 0;
    ptrSeed = &usRandomSeed;
    #endif
    fnInitialiseDevice((void *)usPortPullups);
#endif
    STM32_LowLevelInit();                                                // perform low-level initialisation
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
    INIT_WATCHDOG_DISABLE();                                             // initialise ready for checking of watchdog diabled
    if (WATCHDOG_DISABLE() == 0) {
        IWDG_KR = IWDG_KR_START;                                         // start independent watchdog operation, if not automatically started
                                                                         // enabling the IWDG automatically enables the 40kHz RC oscillator
        IWDG_KR = IWDG_KR_MODIFY;                                        // enable modification or prescaler and reload values
        CONFIGURE_WATCHDOG();                                            // configure the required timeout
    }
#if defined USB_DEVICE_AVAILABLE && defined USB_INTERFACE && defined _STM32F103X
  //_CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, PORTA_BIT12, (OUTPUT_FAST | OUTPUT_PUSH_PULL), 0); // drive the USBDP pin as port with value '0' to avoid emumeration attempt by host
#endif
    fnUserHWInit();                                  _SIM_PORT_CHANGE    // allow the user to initialise hardware specific things
#if (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    fnConfigSPIFileSystem();                                             // configure SPI interface for maximum possible speed
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code to detect the SPI Flash devices
        #include "spi_flash_STM32_atmel.h"
        #include "spi_flash_STM32_stmicro.h"
        #include "spi_flash_STM32_sst25.h"
        #include "spi_flash_STM32_MX66L.h"
    #undef _CHECK_SPI_CHIPS
    #endif
#endif
#if defined _WINDOWS_
    {
        unsigned char ucTest[8] = {0};
        fnEraseFlashSector((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH), 0);
        fnGetParsFile((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH), ucTest, sizeof(ucTest));
        ucTest[0] = 0;
        ucTest[1] = 1;
        ucTest[2] = 2;
        ucTest[3] = 3;
        ucTest[4] = 4;
        fnWriteBytesFlash((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH), ucTest, 5);
        uMemset(ucTest, 0xff, sizeof(ucTest));
        fnGetParsFile((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH), ucTest, sizeof(ucTest));
        uMemset(ucTest, 0xff, sizeof(ucTest));
    }
#endif
}



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
    _EXCEPTION("undefined interrupt!!!");
}

// Delay loop for simple but accurate short delays (eg. for stabilisation delays)
//
extern void fnDelayLoop(unsigned long ulDelay_us)
{
#if !defined TICK_USES_LPTMR && !defined TICK_USES_RTC                   // if the SYSTICK is operating we use it as a us timer for best independence of code execution speed and compiler (KL typically +15% longer then requested value between 100us and 10ms)
    #if defined SYSTICK_DIVIDE_8
        #define CORE_US (HCLK/8/1000000)                                 // the number of core clocks in a us
    #else
        #define CORE_US (HCLK/1000000)                                   // the number of core clocks in a us
    #endif
    register unsigned long ulMatch;
    register unsigned long _ulDelay_us = ulDelay_us;                     // ensure that the compiler puts the variable in a register rather than work with it on the stack
    if (_ulDelay_us == 0) {                                              // minimum delay is 1us
        _ulDelay_us = 1;
    }
    (void)SYSTICK_CSR;                                                   // clear the SysTick reload flag
    ulMatch = ((SYSTICK_CURRENT - CORE_US) & SYSTICK_COUNT_MASK);        // first 1us match value (SysTick counts down)
    do {
    #if !defined _WINDOWS
        while (SYSTICK_CURRENT > ulMatch) {                              // wait until the us period has expired
            if ((SYSTICK_CSR & SYSTICK_COUNTFLAG) != 0) {                // if we missed a reload (that is, the SysTick was reloaded with its reload value after reaching zero)
                (void)SYSTICK_CSR;                                       // clear the SysTick reload flag
                break;                                                   // assume a single us period expired
            }
        }
        ulMatch -= CORE_US;                                              // set the next 1us match
        ulMatch &= SYSTICK_COUNT_MASK;                                   // respect SysTick 24 bit counter mask
    #endif
    } while (--_ulDelay_us != 0);                                        // one us period has expired so count down the requested periods until zero
#else
    #define LOOP_FACTOR  14000000                                        // tuned
    volatile unsigned long ulDelay = ((SYSCLK/LOOP_FACTOR) * ulDelay_us);
    while (ulDelay-- != 0) {}                                            // simple loop tuned to perform us timing
#endif
}


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
}
#endif


// Perform very low level initialisation - called by the start up code
//
static void STM32_LowLevelInit(void)
{
#if !defined INTERRUPT_VECTORS_IN_FLASH
    #if !defined _MINIMUM_IRQ_INITIALISATION
    void(**processor_ints)(void);
    #endif
    VECTOR_TABLE *ptrVect;
#endif
#include "stm32_CLOCK.h"                                                 // {39} clock configuration
#if defined FLASH_PROGRAMMING_OPTION_SETTING && defined FLASH_OPTCR      // {40} program a flash configuration option (this is only performed when the setting causes a change to that already programmed)
    #if defined FLASH_PROGRAMMING_OPTION_SETTING && defined FLASH_OPTCR1
    fnSetFlashOption(FLASH_PROGRAMMING_OPTION_SETTING, FLASH_PROGRAMMING_OPTION_SETTING_1, FLASH_PROGRAMMING_OPTION_MASK);
    #elif defined FLASH_OPTCR1
    fnSetFlashOption(FLASH_PROGRAMMING_OPTION_SETTING, DEFAULT_FLASH_OPTION_SETTING_1, FLASH_PROGRAMMING_OPTION_MASK);
    #else
    fnSetFlashOption(FLASH_PROGRAMMING_OPTION_SETTING, 0, FLASH_PROGRAMMING_OPTION_MASK);
    #endif
#endif
#if defined INTERRUPT_VECTORS_IN_FLASH                                   // {111}
    VECTOR_TABLE_OFFSET_REG = ((unsigned long)&__vector_table);
#else
    #if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
    #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);                       // position the vector table at the bottom of RAM
    #endif
    VECTOR_TABLE_OFFSET_REG = (unsigned long)ptrVect; 
    #if !defined _MINIMUM_IRQ_INITIALISATION
    ptrVect->ptrHardFault     = irq_hard_fault;
    ptrVect->ptrMemManagement = irq_memory_man;
    ptrVect->ptrBusFault      = irq_bus_fault;
    ptrVect->ptrUsageFault    = irq_usage_fault;
    ptrVect->ptrDebugMonitor  = irq_debug_monitor;
    ptrVect->ptrNMI           = irq_NMI;
    #if defined RUN_IN_FREE_RTOS
    ptrVect->ptrPendSV        = xPortPendSVHandler;                      // FreeRTOS's PendSV handler
    ptrVect->ptrSVCall        = vPortSVCHandler;                         // FreeRTOS's SCV handler
    ptrVect->reset_vect.ptrResetSP = (void *)(RAM_START_ADDRESS + (SIZE_OF_RAM - NON_INITIALISED_RAM_SIZE)); // the stack pointer value will be taken from the vector base area so enter it in SRAM
    #else
    ptrVect->ptrPendSV        = irq_pend_sv;
    ptrVect->ptrSVCall        = irq_SVCall;
    #endif
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
#endif
#if defined DMA_MEMCPY_SET && !defined DEVICE_WITHOUT_DMA                // if uMemcpy()/uMemset() is to use DMA enable the DMA controller used by it
    #if MEMCPY_CHANNEL > 7
    POWER_UP(AHB1, RCC_AHB1ENR_DMA2EN);
    #else
    POWER_UP(AHB1, RCC_AHB1ENR_DMA1EN);
    #endif
#endif
#if defined (_GNU)
    __init_gnu_data();                                                   // initialise variables
#endif
#if defined _WINDOWS                                                     // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Check the size of interrupt vectors required by the processor!!");
    }
#endif
#if defined _WINDOWS
    fnUpdateOperatingDetails();                                          // {33} update operating details to be displayed in the simulator
#endif
#if defined MCO_CONNECTED_TO_MSI || defined MCO_CONNECTED_TO_HSI
    #if defined MCO_CONNECTED_TO_MSI
        #define _RCC_CFGR_MCOSEL_INPUT      RCC_CFGR_MCOSEL_MSI          // selected MSI
    #elif defined MCO_CONNECTED_TO_HSI
        #define _RCC_CFGR_MCOSEL_INPUT      RCC_CFGR_MCOSEL_HSICLK       // selet HSI
    #endif
    RCC_CFGR &= ~(RCC_CFGR_MCOSEL_MASK | RCC_CFGR_MCOPRE_MASK);          // ensure the fields are initially masked
    #if defined MCO_DIVIDE
        #if MCO_DIVIDE == 1
    RCC_CFGR |= _RCC_CFGR_MCOSEL_INPUT;                                  // connect undivided MSI clock to MCO output
        #elif MCO_DIVIDE == 2
    RCC_CFGR |= (RCC_CFGR_MCOPRE_2 | _RCC_CFGR_MCOSEL_INPUT);            // divide by 2 and connect selected clock to MCO output
        #elif MCO_DIVIDE == 4
    RCC_CFGR |= (RCC_CFGR_MCOPRE_4 | _RCC_CFGR_MCOSEL_INPUT);            // divide by 4 and connect selected clock to MCO output
        #elif MCO_DIVIDE == 8
    RCC_CFGR |= (RCC_CFGR_MCOPRE_8 | _RCC_CFGR_MCOSEL_INPUT);            // divide by 8 and connect selected clock to MCO output
        #elif MCO_DIVIDE == 16
    RCC_CFGR |= (RCC_CFGR_MCOPRE_16 | _RCC_CFGR_MCOSEL_INPUT);           // divide by 16 and connect selected clock to MCO output
        #elif (MCO_DIVIDE_MAX == 128) && (MCO_DIVIDE == 32)
    RCC_CFGR |= (RCC_CFGR_MCOPRE_32 | _RCC_CFGR_MCOSEL_INPUT);           // divide by 32 and connect selected clock to MCO output
        #elif (MCO_DIVIDE_MAX == 128) && (MCO_DIVIDE == 64)
    RCC_CFGR |= (RCC_CFGR_MCOPRE_64 | _RCC_CFGR_MCOSEL_INPUT);           // divide by 64 and connect selected clock to MCO output
        #elif (MCO_DIVIDE_MAX == 128) && (MCO_DIVIDE == 128)
    RCC_CFGR |= (RCC_CFGR_MCOPRE_128 | _RCC_CFGR_MCOSEL_INPUT);          // divide by 128 and connect selected clock to MCO output
        #else
    #error "Invalid MCO prescale value!"
        #endif
    #else
    RCC_CFGR |= _RCC_CFGR_MCOSEL_INPUT;                                  // connect undivided selected clock to MCO output
    #endif
    #if defined MCO_OUT_ON_B
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_SYS), (PORTB_BIT13), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // MCO on PB13
    #elif defined MCO_OUT_ON_PA9
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_SYS), (PORTA_BIT9), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // MCO on PA9
    #else
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_SYS), (PORTA_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // MCO on PA8
    #endif
#endif
#if defined DWT_CYCCNT && defined USE_CORTEX_CYCLE_COUNTER
    DEMCR |= DHCSR_TRCENA;                                               // enable trace for DWT features
    DWT_CYCCNT = 0;                                                      // reset the cycle count value
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;                                      // enable the cycle counter
#endif
}


#if defined ROWLEY && !defined ROWLEY_2                                  // Rowley project requires extra initialisation for debugger to work correctly before V1.7 build 17
static void _main2(void)
{
    asm("mov lr, #0");
    asm("b _main");
}
#endif

#if !defined _COMPILE_KEIL                                               // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #if defined ARM_MATH_CM0PLUS || defined ARM_MATH_CM0                 // {67} cortex-m0/cortex-M0+ assembler code
        #if !defined _WINDOWS
    asm(" ldr r1, [r0,#0]");                                             // get the stack pointer value from the program's reset vector
    asm(" mov sp, r1");                                                  // copy the value to the stack pointer
    asm(" ldr r0, [r0,#4]");                                             // get the program counter value from the program's reset vector
    asm(" blx r0");                                                      // jump to the start address
        #endif
    #else                                                                // cortex-M3/M4 assembler code
        #if !defined _WINDOWS
    asm(" ldr sp, [r0,#0]");                                             // load the stack pointer value from the program's reset vector
    asm(" ldr pc, [r0,#4]");                                             // load the program counter value from the program's reset vector to cause operation to continue from there
        #endif
    #endif
}
#endif

#if !defined FREERTOS_NOT_COMPILED /*defined RUN_IN_FREE_RTOS || defined _WINDOWS*/ // to satisfy FreeRTOS callbacks - even when FreeRTOS not linked
extern void *pvPortMalloc(int iSize)
{
    return uMalloc((MAX_MALLOC)iSize);                                   // use uMalloc() which assumes that memory is never freed
}
extern void vPortFree(void *free)
{
    _EXCEPTION("Unexpected free call!!");
}
extern void vApplicationStackOverflowHook(void)
{
}
extern void vApplicationTickHook(void)                                   // FreeRTOS tick interrupt
{
}
extern void vMainConfigureTimerForRunTimeStats(void)
{
}
extern unsigned long ulMainGetRunTimeCounterValue(void)
{
    return uTaskerSystemTick;
}
extern void vApplicationIdleHook(void)
{
}
extern void prvSetupHardware(void)
{
}
#endif

// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined COMPILE_IAR
__root const _RESET_VECTOR __vector_table @ ".intvec"                    // __root forces the function to be linked in IAR project
#elif defined _GNU
const _RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _COMPILE_KEIL
__attribute__((section("RESET"))) const _RESET_VECTOR reset_vect
#else
const _RESET_VECTOR __vector_table
#endif
= {
#if defined INTERRUPT_VECTORS_IN_FLASH
    {
#endif
    (void *)(RAM_START_ADDRESS + (SIZE_OF_RAM - NON_INITIALISED_RAM_SIZE)), // stack pointer to top of RAM
    (void (*)(void))START_CODE
#if defined INTERRUPT_VECTORS_IN_FLASH
    },
#endif
#if defined _APPLICATION_VALIDATION 
    { 0x87654321, 0xffffffff },                                          // signal that this application supports validation
    { 0xffffffff, 0xffffffff },                                          // overwrite first location with 0x55aa33cc to validate the application
#elif defined INTERRUPT_VECTORS_IN_FLASH                                 // presently used only by the _STM32L011 (vectors in flash to save space when little SRAM resources available)
    #if defined _MINIMUM_IRQ_INITIALISATION
    irq_default,
    irq_default,
    irq_default,
    irq_default,
    irq_default,
    #else
    irq_NMI,
    irq_hard_fault,
    irq_memory_man,
    irq_bus_fault,
    irq_usage_fault,
    #endif
    0,
    0,
    0,
    0,
    #if defined RUN_IN_FREE_RTOS
    vPortSVCHandler,                                                     // FreeRTOS's SCV handler
    irq_debug_monitor,
    0,
    xPortPendSVHandler,                                                  // FreeRTOS's PendSV handler
    #elif defined _MINIMUM_IRQ_INITIALISATION
    irq_default,
    irq_default,
    0,
    irq_default,
    #else
    irq_SVCall,
    irq_debug_monitor,
    0,
    irq_pend_sv,
    #endif
    _RealTimeInterrupt,                                                  // systick
    {                                                                    // processor specific interrupts
    irq_default,                                                         // 0
    irq_default,                                                         // 1
    irq_default,                                                         // 2
    irq_default,                                                         // 3
    irq_default,                                                         // 4
    irq_default,                                                         // 5
    irq_default,                                                         // 6
    irq_default,                                                         // 7
    irq_default,                                                         // 8
    irq_default,                                                         // 9
    irq_default,                                                         // 10
    irq_default,                                                         // 11
    irq_default,                                                         // 12
    irq_default,                                                         // 13
    irq_default,                                                         // 14
    irq_default,                                                         // 15
    irq_default,                                                         // 16
    irq_default,                                                         // 17
    irq_default,                                                         // 18
    irq_default,                                                         // 19
    irq_default,                                                         // 20
    irq_default,                                                         // 21
    irq_default,                                                         // 22
	irq_default,                                                         // 23
    irq_default,                                                         // 24
    irq_default,                                                         // 25
    irq_default,                                                         // 26
    irq_default,                                                         // 27
    #if defined SERIAL_INTERFACE
    SCI2_Interrupt,                                                      // 28
    #endif
    irq_default                                                          // 29
    }
#endif
};
#endif
