/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    23.02.2012 Allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined {1}
    25.02.2012 Mask PIT old mode before setting new one                  {2}
    03.03.2012 Add K70 UART2 alternative port mapping                    {3}
    09.03.2012 Disable PIT before configuring load value so that it is retriggerable {4}
    12.03.2012 Add ADC support                                           {5}
    05.04.2012 Add UART DMA support for Tx                               {6}
    06.04.2012 Modify control of RTS line when not using automatic RTS control on transmission {7}
    06.04.2012 Add UART DMA support for Rx                               {8}
    07.04.2012 DMA channel used by uMemset() and uMemcpy() configurable  {9}
    07.04.2012 Adapt Flash interface for FPU devices                     {10}
    21.04.2012 Use link-local IPv6 address for multicast configuration   {11}
    29.04.2012 Add LAN_TX_FPU_WORKAROUND to workaround a problem with the Ethernet controller missing a queued buffer when caching is enabled {12}
    08.05.2012 Add UART3 option on port F for K70                        {12}
    18.06.2012 Add Ethernet parameter to enable passing received CRC-32 to application {13}
    23.06.2012 Enable RMII clock from external pin                       {14}
    24.06.2012 Add SERIAL_SUPPORT_DMA_RX_FREERUN support                 {15}
    24.06.2012 Correct some RTS/CTS pins                                 {16}
    25.06.2012 Enable access to FPU                                      {17}
    14.07.2012 Add _fnMIIread() and _fnMIIwrite()                        {18}
    14.07.2012 Add Micrel SMI mode                                       {19}
    22.07.2012 Add interface for OTP area                                {20}
    04.08.2012 Configure EMAC buffer descriptor operation for little-endian when this is possible in the device used {21}
    13.08.2012 Add support for magic reset Ethernet reception frame      {22}
    14.08.2012 Modify the random number location when not working with RND HW {23}
    12.09.2012 Allow zero length Flash write (together with SPI Flash) to ensure closing outstanding flash buffer {24}
    14.09.2012 Add UART alternative port options to K61                  {25}
    24.09.2012 Ensure port interrupt handler is entered before configuring interrupt {26}
    27.09.2012 Correct LINK UP event                                     {27}
    09.10.2012 Extend I2C speed settings                                 {28}
    19.10.2012 Exception only on multicast MAC address                   {29}
    17.12.2012 Add interface number to Ethernet when working with multiple IP interfaces {30}
    23.01.2013 Correct long word programming when simulating non-FPU devices (simulation error introduced with {10}) {31}
    26.01.2013 Add _APPLICATION_VALIDATION                               {32}
    28.03.2013 Add USB HS support                                        {33}
    03.03.2013 Moved RANDOM_SEED_LOCATION and BOOT_MAIL_BOX to kinetis.h
    21.03.2013 Add PARAMETER_NO_ALIGNMENT option to parameter flash      {35}
    24.03.2013 Add check of flash write validity when simulating         {36}
    07.04.2013 Add PHY_POLL_LINK support                                 {37}
    24.05.2013 Add SDRAM initialisation                                  {38}
    03.06.2013 Add DAC interface in SW triggered mode                    {39}
    03.06.2013 Allow user defined startup code in IAR initialisation     {40}
    03.06.2013 Add ADC result to interrupt call-back                     {41}
    04.06.2013 Added USB_FS_MALLOC(), USB_FS_MALLOC_ALIGN(), USB_HS_MALLOC(), USB_HS_MALLOC_ALIGN(), ETH_BD_MALLOC_ALIGN(), ETH_RX_BUF_MALLOC_ALIGN() and ETH_TX_BUF_MALLOC_ALIGN() defaults {42}
    23.06.2013 Add _PHY_KSZ8863 mode                                     {43}
    23.06.2013 Add PHY tail tagging support                              {44}
    08.07.2013 Add full duplex event type                                {45}
    09.07.2013 Implement fnFlashRoutine() as assemble code to avoid possibility of compilers in-lining it {46}
    27.07.2013 Ensure that SDHC read is not performed using a command that can't save to misaligned memory {47}
    28.07.2013 Correct interrupt priority entry                          {48}
    28.07.2013 Allow the PHY interrupt handler to be interrupted by higher priority interrupts {49}
    05.08.2013 Correct SPI flash page delete alignment when page size if not power of 2 size {50}
    05.08.2013 Add UTC option when setting and retrieving time/date      {51}
    17.08.2013 Adjust PHY interface when _DP83849I is used               {52}
    26.09.2013 Correct flash address when simulating internal flash erase together with other memory types {53}
    30.09.2013 Add ADC A/B input multiplexer control                     {54}
    01.10.2013 Make fnEnterInterrupt() global                            {55}
    23.10.2013 Ensure source address remains stable in free-running UART mode {56}
    27.10.2013 Add ADC DMA configuration                                 {57}
    27.10.2013 Add PDB interface                                         {58}
    17.11.2013 Add uReverseMemcpy() using DMA                            {59}
    19.11.2013 Add DAC DMA configuration                                 {60}
    19.11.2013 Add DAC triggering via PDB                                {61}
    18.12.2013 Add monitoring of SDHC transmission buffer and break on stall {62}
    18.12.2013 Reset SDHC flags at re-initialisation to ensure no old error flags remain {63}
    26.12.2013 Add RANDOM_NUMBER_GENERATOR_B for devices without RNGA    {64}
    11.01.2013 Set CAN controller clock source before moving to freeze mode {65}
    24.01.2014 Add crystal gain mode control                             {66}
    25.01.2014 Add Kinetis KL support                                    {67}
    04.02.2014 Power up PIT module after setting mode to protect from power down from another PIT channel interrupt - also protect mode variable changes from interrupts {68}
    18.02.2014 Correct fnConvertSecondsTime() calculation                {69}
    04.03.2014 Correct CAN time stamp request option                     {70}
    05.03.2014 Add USB error counters and clear errors in the USB interrupt routine {71}
    05.04.2014 Add FlexTimer monostable/periodic timer support           {72}
    10.04.2014 Add volatile to ensure that interrupt flags are not optimised away when code in-lining takes place {73}
    16.04.2014 Add fnModifyMulticastFilter() for IGMP use                {74}
    17.04.2014 Use RTC seconds interrupt rather than alarm interrupt, when available {75}
    21.04.2014 Add KL PIT support                                        {76}
    24.04.2014 Add KL RTC support based on LPO clock or external 32kHz oscillator {77}
    25.04.2014 Add automatic I2C lockup detection and recovery           {78}
    28.04.2014 Make some flash drivers conditional on FLASH_ROUTINES so that they can be used without any file system support {79}
    06.05.2014 Add KL DMA based uMemcpy() and uMemset()                  {80}
    06.05.2014 Add KL DMA based UART transmission                        {81}
    11.05.2014 Add FlexNVM data flash mode                               {82}
    28.05.2014 Move re-enable of USB SIE token processing to after input handling to avoid sporadic SETUP frame loss {83}
    02.06.2014 Add _KSZ8081RNA status register settings                  {84}
    09.06.2014 Add Crossbar Switch configuration                         {85}
    15.06.2014 Add low power timer                                       {86}
    17.06.2014 Optimised DMA based uMemset()/uMemcpy()/uReverseMemcpy() to utilise widest transfer sizes possible {87}
    21.06.2014 Don't use different ADC result register when channel B mux input is used {88}
    21.06.2014 Adjust FlexTimer/TPM use of FTM_SC_TOF to clear interrupt correctly {89}
    24.06.2014 Add USB clock from PLL1 as option                         {90}
    25.06.2014 Add hardware operating details for simulator display      {91}
    05.07.2014 Rework of UART interrupt handler including receiver overrun checking {92}
    15.07.2014 Add optional power mode setting after basic initialisation {93}
    19.07.2014 Add option to run system TICK from low power timer        {94}
    19.07.2014 Add option to clock the processor directly from the external clock without using the PLL {95}
    19.07.2014 Add UART low power stop mode support                      {96}
    21.07.2014 Add PIT errate e2682 workaround for some older devices    {97}
    22.07.2014 Add fnGetLowPowerMode() and fnSetLowPowerMode()           {98}
    22.07.2014 Add clock source selection to TPM                         {99}
    02.08.2014 Add option to run system TICK from the RTC (KE)           {100}
    19.08.2014 Add clock divider configuration when running directly from the external clock {101}
    22.08.2014 Add uMask_Interrupt()                                     {102}
    02.09.2014 Move remaining byte copy in DMA based uMemcpy() to after DMA termination {103}
    30.10.2014 Add optional crystal-less USB clock for K64, K63, K22 and K24 {104}
    07.11.2014 Add section programming where possible (faster programming of larger Flash blocks) {105}
    17.11.2014 Add KL03 LPUART support                                   {106}
    04.12.2014 Ensure source and destination addresses remain stable in UART tx DMA mode {107}
    12.12.2014 Add power of 2s ATMEL flash support                       {108}
    29.12.2014 Add KE02 EEPROM support                                   {109}
    30.12.2014 Add Green Hills project support (_COMPILE_GHS)            {110}
    01.01.2015 Allow interrupt vectors to be fixed in flash              {111}
    11.01.2015 Add wake-up interrupt support                             {112}
    28.01.2015 Add COSMIC project support (_COMPILE_COSMIC)              {113}
    08.02.2015 Time keeping routines moved to uTasker\time_keeper.c      {114}
    18.02.2015 fnConnectGPIO() modified to allow modifying only parts of pin characteristics {115}
    24.03.2015 Peripheral drivers removed to their own include files
    19.07.2015 Correct SysTick priority shift                            {116}
    26.07.2015 Respect that SMC_PMPROT is one-time write by setting maximum level once {117}
    13.10.2015 Set high speed run mode when required                     {118}
    16.10.2015 Write to SYSTICK_CURRENT to synchronise the Systick counter after configuration {119}
    16.10.2015 Add routine to drive CLKOUT for measurment purposes       {120}
    23.10.2015 Use kinetis_USB_OTG.h for FS device/host                  {121}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _WINDOWS
    #include "config.h"
    extern void fnUpdateOperatingDetails(void);                          // {91}
    #define INITHW  extern
    extern void fec_txf_isr(void);
    extern void fnSimulateDMA(int channel);
    #define START_CODE 0
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #include "config.h"
    #define  fnSimulateDMA(x)
    #if defined _COMPILE_KEIL
        extern void __main(void);
        #define START_CODE _init
    #elif defined _COMPILE_IAR
        extern void __iar_program_start(void);                           // IAR library initialisation routine
        #define START_CODE disable_watchdog
    #elif defined _COMPILE_GHS                                           // {110}
        extern void _start_T(void);                                      // GHS library initialisation routine
        #define START_CODE disable_watchdog
    #else
        #define START_CODE main
    #endif
#endif

#if defined _KINETIS

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define USER_STARTUP _LowLevelInit

#if defined _COMPILE_COSMIC
    #define __interrupt @interrupt
#else
    #define __interrupt
#endif

#if defined _APPLICATION_VALIDATION                                      // {32}
    #define _RESET_VECTOR  RESET_VECTOR_VALIDATION
#elif defined INTERRUPT_VECTORS_IN_FLASH                                 // {111}
    #define _RESET_VECTOR   VECTOR_TABLE
    const _RESET_VECTOR __vector_table;
#else
    #define _RESET_VECTOR  RESET_VECTOR
#endif

#define UART0_TX_CLK_REQUIRED 0x00000001
#define UART1_TX_CLK_REQUIRED 0x00000002
#define UART2_TX_CLK_REQUIRED 0x00000004
#define UART3_TX_CLK_REQUIRED 0x00000008
#define UART4_TX_CLK_REQUIRED 0x00000010
#define UART5_TX_CLK_REQUIRED 0x00000020
#define UART0_RX_CLK_REQUIRED 0x00000040
#define UART1_RX_CLK_REQUIRED 0x00000080
#define UART2_RX_CLK_REQUIRED 0x00000100
#define UART3_RX_CLK_REQUIRED 0x00000200
#define UART4_RX_CLK_REQUIRED 0x00000400
#define UART5_RX_CLK_REQUIRED 0x00000800


/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void _LowLevelInit(void);

#if defined SUPPORT_LOW_POWER
    static int fnPresentLP_mode(void);
#endif


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

#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25 && !defined SPI_FLASH_W25Q && !defined SPI_FLASH_S25FL1_K
        #define SPI_FLASH_ATMEL                                          // default if not otherwise defined
    #endif
    #define _SPI_DEFINES
        #include "spi_flash_kinetis_atmel.h"
        #include "spi_flash_kinetis_stmicro.h"
        #include "spi_flash_kinetis_sst25.h"
        #include "spi_flash_w25q.h"
        #include "spi_flash_kinetis_s25fl1-k.h"
    #undef _SPI_DEFINES
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static int iInterruptLevel = 0;                                          // present level of disable nesting

#if defined SUPPORT_LOW_POWER
    static unsigned long ulPeripheralNeedsClock = 0;                     // {96} stop mode is blocked if a peripheral is in use that needs a clock that will be stopped
#endif
#if defined RANDOM_NUMBER_GENERATOR && !defined RND_HW_SUPPORT
    unsigned short *ptrSeed;
#endif
#if defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK
    static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)] = {0}; // FLASH row backup buffer (on word boundary)
#endif
#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static unsigned long SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0}; // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT] = {0}; // list of attached FLASH devices

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
#endif


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined _WINDOWS                              
    extern unsigned char vector_ram[sizeof(VECTOR_TABLE)];               // vector table in simulated RAM (long word aligned)
    #if defined SERIAL_INTERFACE
        extern unsigned char ucTxLast[NUMBER_SERIAL] = {0};
    #endif
#endif

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

#if !defined _WINDOWS
    static void fnInitHW(void);
#endif


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */

#if !defined _WINDOWS
    extern __interrupt void _start(void);                                // reset vector location
#endif

#define _SPI_FLASH_INTERFACE                                             // insert manufacturer dependent SPI Flash driver code
    #include "spi_flash_kinetis_atmel.h"
    #include "spi_flash_kinetis_stmicro.h"
    #include "spi_flash_kinetis_sst25.h"
    #include "spi_flash_w25q.h"
    #include "spi_flash_kinetis_s25fl1-k.h"
#undef _SPI_FLASH_INTERFACE


/* =================================================================== */
/*                            STARTUP CODE                             */
/* =================================================================== */


#if !defined _WINDOWS
    extern void __segment_init(void);
    #if defined  _COMPILE_IAR
        #pragma segment=".data"
        #pragma segment=".bss"
        static unsigned char *ptrTopOfUsedMemory = 0;
        #define HEAP_START_ADDRESS ptrTopOfUsedMemory                    // IAR compiler - last location of static variables
        #define  __sleep_mode() __WFI()                                  // IAR intrinsic
    #elif defined _COMPILE_KEIL
        #define HEAP_START_ADDRESS    _keil_ram_size(0)
        #define __disable_interrupt() __disable_irq()                    // KEIL intrinsics
        #define __enable_interrupt()  __enable_irq()
        #define __sleep_mode()        __wfi()
    #else                                                                // disable interrupt in assembler code
        extern unsigned char __heap_end__;
        #define HEAP_START_ADDRESS &__heap_end__                         // GNU last location of static variables
        #if defined ROWLEY || defined _KDS
            #define asm(x) __asm__(x)
        #elif defined _COMPILE_COSMIC
            #define asm(x) _asm(x)
        #endif
        #if defined _COMPILE_GHS                                         // {110}
            #define __disable_interrupt() asm("cpsid i")                 // __DI() intrinsics are not used becasue they are asm("cpsid if") which doesn't allow correct low power mode operation
            #define __enable_interrupt()  asm("cpsie i")                 // __EI()
            #define __sleep_mode()        asm("wfi")
        #else
            #define __disable_interrupt() asm("cpsid   i")
            #define __enable_interrupt()  asm("cpsie   i")
            #define __sleep_mode()        asm("wfi")
        #endif
    #endif

    #if defined _COMPILE_KEIL

typedef struct stREGION_TABLE
{
    unsigned char *ptrConst;                                             // const variables belonging to block
    unsigned char *ptrDestination;                                       // destination in RAM
    unsigned long  ulLength;                                             // length of the block in SRAM
    unsigned long  ulUnknown;                                            // entry unknown
} REGION_TABLE;

// Calculate the end of used SRAM from the Keil linker information and optionally initialise variables
//
static unsigned char *_keil_ram_size(int iInit)
{
    extern REGION_TABLE Region$$Table$$Base;                            // table location supplied by linker
    extern REGION_TABLE Region$$Table$$Limit;                           // end of table list
    REGION_TABLE *ptrEntries = &Region$$Table$$Base;                    // first block
    unsigned char *ptrRam = ptrEntries->ptrDestination;                 // RAM address
    do {
        if (iInit != 0) {
            if (ptrEntries->ulUnknown == 0x60) {                        // not valid for uVision4
                uMemset(ptrRam, 0, ptrEntries->ulLength);               // zero data
            }
            else {
                uMemcpy(ptrRam, ptrEntries->ptrConst, ptrEntries->ulLength); // intialise data (uVision4 uses compressed data in Flash and so can't be used like this
            }
        }
        ptrRam += ptrEntries->ulLength;                                 // add length
        ptrEntries++;                                                   // move to next block
    } while (ptrEntries != &Region$$Table$$Limit);
    return ptrRam;
}
    #endif

    #if defined _COMPILE_IAR || defined _COMPILE_GHS                     // {110}
// This is the first function called so that it can immediately disable the watchdog so that it doesn't fire during variable initialisation
//
static void disable_watchdog(void)
{
    INIT_WATCHDOG_DISABLE();                                             // configure the input used to determine the watchdog configuration
    if (WATCHDOG_DISABLE() == 0) {                                       // if the watchdog disable input is not active
        ACTIVATE_WATCHDOG();                                             // allow user configuration of internal watch dog timer
    }
    else {
        #if defined KINETIS_KL && !defined KINETIS_KL82                  // {67}
        SIM_COPC = SIM_COPC_COPT_DISABLED;                               // disable the COP
        #else
        UNLOCK_WDOG();                                                   // open a windows to write to watchdog
            #if defined KINETIS_KE
        WDOG_CS2 = 0;                                                    // this is required to disable the watchdog
        WDOG_TOVAL = 0xffff;
        WDOG_WIN = 0;
        WDOG_CS1 = WDOG_CS1_UPDATE;                                      // disable watchdog but allow updates
            #else
        WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_CLKSRC); // disable watchdog
            #endif
        #endif
    }
        #if defined SUPPORT_LOW_POWER && (defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000))
        PMC_REGSC = PMC_REGSC_ACKISO;                                    // acknowledge the isolation mode to set certain peripherals and I/O pads back to normal run state
        #endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED    
        #if defined USER_STARTUP_CODE                                    // {40} allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
        #endif
        #if defined _COMPILE_GHS                                         // {110}
    _start_T();                                                          // now call the GHS initialisation code which initialises variables and then calls main() 
        #else
    __iar_program_start();                                               // now call the IAR initialisation code which initialises variables and then calls main() 
        #endif
}
    #endif


/* =================================================================== */
/*                                main()                               */
/* =================================================================== */

// Main entry for the target code
//
extern int main(void)
{
#if defined MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart;
#endif
#if defined (_COMPILE_IAR)
    if (__sfe(".bss") > __sfe(".data")) {                                // set last used SRAM address
        ptrTopOfUsedMemory = __sfe(".bss");
    }
    else {
        ptrTopOfUsedMemory = __sfe(".data");
    }
#endif
    fnInitHW();                                                          // perform hardware initialisation (note that we do not have heap yet)
#if defined RANDOM_NUMBER_GENERATOR && !defined RND_HW_SUPPORT
    ptrSeed = RANDOM_SEED_LOCATION;                                      // {23}
#endif
#if defined MULTISTART
    prtInfo = ptMultiStartTable;                                         // if the user has already set to alternative start configuration
    if (prtInfo == 0) {                                                  // no special start required
_abort_multi:
        fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                 // initialise heap
        uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system (and TICK interrupt)
        while ((prtInfo = (MULTISTART_TABLE *)uTaskerSchedule()) == 0) {}// schedule uTasker
    }

    do {
        pucHeapStart = HEAP_START_ADDRESS;
        if (prtInfo->new_hw_init) {                                      // info to next task configuration available
            pucHeapStart = prtInfo->new_hw_init(JumpTable);              // get heap details from next configuration
            if (!pucHeapStart) {
                goto _abort_multi;                                       // this can happen if the jump table version doesn't match - prefer to stay in boot mode than start an application which will crash
            }
        }

        fnInitialiseHeap(prtInfo->ptHeapNeed, pucHeapStart);             // initialise the new heap
                                                                         // start the operating system with next configuration
        uTaskerStart((UTASKTABLEINIT *)prtInfo->ptTaskTable, prtInfo->ptNodesTable, PHYSICAL_QUEUES);
        while ((prtInfo = (MULTISTART_TABLE *)uTaskerSchedule()) == 0) {}// schedule uTasker

    } while (1);
#else
    fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                     // initialise heap
    uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system (and TICK interrupt)
    while (1) {
        uTaskerSchedule();                                               // schedule uTasker
    }
#endif
    return 0;                                                            // we never return....
}
#endif                                                                   // end if not _WINDOWS

#if defined _COMPILE_KEIL
// Keil demands the use of a __main() call to correctly initialise variables - it then calls main()
//
extern void _init(void)
{
    _LowLevelInit();                                                     // configure watchdog and set the CPU to operating speed
    __main();                                                            // Keil initialises variables and then calls main()
}
#endif


#if defined RANDOM_NUMBER_GENERATOR
    #if defined RND_HW_SUPPORT
extern void fnInitialiseRND(unsigned short *usSeedValue)
{
    #if defined RANDOM_NUMBER_GENERATOR_B                                // {64}
    POWER_UP(3, SIM_SCGC3_RNGB);                                         // power up RNGB
    RNG_CR = (RNG_CR_FUFMODE_TE | RNG_CR_AR);                            // automatic reseed mode and generate a bus error on FIFO underrun
    RNG_CMD = (RNG_CMD_GS | RNG_CMD_CI | RNG_CMD_CE);                    // start the initial seed process
                                                                         // the initial seeding takes some time but we don't wait for it to complete here - if it hasn't completed when we first need a value we will wait for it then
    #else
    POWER_UP(3, SIM_SCGC3_RNGA);                                         // power up RNGA
    RNG_CR = (RNG_CR_INTM | RNG_CR_HA | RNG_CR_GO);                      // start first conversion
    #endif
}

// Get a random number from the RNG
//
extern unsigned short fnGetRndHW(void)
{
    unsigned long ulRandomNumber;
    #if defined RANDOM_NUMBER_GENERATOR_B                                // {64}
    while (RNG_SR & RNG_SR_BUSY) {}                                      // wait for the RNGB to become ready (it may be seeding)
    while ((RNG_SR & RNG_SR_FIFO_LVL_MASK) == 0) {                       // wait for at least one output word to become available
        #if defined _WINDOWS
        RNG_OUT = rand();
        RNG_SR |= 0x00000100;                                            // put one result in the FIFO
        #endif
    }
    ulRandomNumber = RNG_OUT;                                            // read from the FIFO
        #if defined _WINDOWS
    RNG_SR &= ~RNG_SR_FIFO_LVL_MASK;
        #endif
    #else                                                                // RNGA
    while (!(RNG_SR & RNG_SR_OREG_LVL)) {                                // wait for an output to become available
        #if defined _WINDOWS
        RNG_SR |= RNG_SR_OREG_LVL;
        RNG_OR = rand();
        #endif
    }
        #if defined _WINDOWS
    RNG_SR &= ~RNG_SR_OREG_LVL;
        #endif
    ulRandomNumber = RNG_OR;                                             // read output value that has been generated
    #endif
    return (unsigned short)(ulRandomNumber);                             // return 16 bits of output
}
    #else
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
#endif

#if defined DEVICE_WITH_SLCD
// The SLCD controller may retain segment settings across resets so this initialisation routine is used to ensure all are cleared when starting
//
extern void fnClearSLCD(void)
{
    unsigned long *ptrSegments = LCD_WF3TO0_ADDR;
    int i = 0;
    while (i++ < SEGMENT_REGISTER_COUNT) {
        *ptrSegments++ = 0;
    }
    #if defined _WINDOWS && defined SLCD_FILE
    fnSimulateSLCD();                                                    // allow any SLCD display updates to be made
    #endif
}
#endif

extern void fnDelayLoop(unsigned long ulDelay_us)
{
#if !defined TICK_USES_LPTMR && !defined TICK_USES_RTC                   // if the SYSTICK is operating we use it as a us timer for best independence of code execution speed and compiler (KL typically +15% longer then requested value between 100us and 10ms)
    #define CORE_US (CORE_CLOCK/1000000)                                 // the number of core clocks in a us
    #if !defined _WINDOWS
    register unsigned long ulPresentSystick;
    #endif
    register unsigned long ulMatch;
    register unsigned long _ulDelay_us = ulDelay_us;                     // ensure that the compiler puts the variable in a register rather than work with it on the stack
    if (_ulDelay_us == 0) {                                              // minimum delay is 1us
        _ulDelay_us = 1;
    }
    (void)SYSTICK_CSR;                                                   // clear the SysTick reload flag
    ulMatch = (SYSTICK_CURRENT - CORE_US);                               // next 1us match value (SysTick counts down)
    do {
    #if !defined _WINDOWS
        while ((ulPresentSystick = SYSTICK_CURRENT) > ulMatch) {         // wait until a us period has expired
            if (SYSTICK_CSR & SYSTICK_COUNTFLAG) {                       // if we missed a reload
                (void)SYSTICK_CSR;
                break;                                                   // assume a us period expired
            }
        }
        ulMatch = (ulPresentSystick - CORE_US);
    #endif
    } while (--_ulDelay_us);
#else
    register unsigned long _ulDelay_us = ulDelay_us;                     // ensure that the compiler puts the variable in a register rather than work with it on the stack
    register unsigned long ul_us;
    while (_ulDelay_us--) {                                              // for each us required        
        ul_us = (CORE_CLOCK/8000000);                                    // tuned but may be slightly compiler dependent - interrupt processing may increase delay
        while (ul_us--) {}                                               // simple loop tuned to perform us timing
    }
#endif
}

// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               // perform hardware initialisation
{
#if defined _WINDOWS
    unsigned long ulPortPullups[] = {
        PORT0_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_kinetis.h
    #if PORTS_AVAILABLE > 1
        PORT1_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 2
        PORT2_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 3
        PORT3_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 4
        PORT4_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 5
        PORT5_DEFAULT_INPUT,
    #endif
    #if defined SUPPORT_ADC                                              // {5}
        ((ADC0_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #if ADC_CONTROLLERS > 1
        ((ADC1_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #endif
        #if ADC_CONTROLLERS > 2
        ((ADC2_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #endif
        #if ADC_CONTROLLERS > 3
        ((ADC3_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #endif
    #endif
    };
    #if defined RANDOM_NUMBER_GENERATOR && !defined RND_HW_SUPPORT
    static unsigned short usRandomSeed = 0;
    ptrSeed = &usRandomSeed;
    #endif
    fnInitialiseDevice(ulPortPullups);
#endif
#if !defined _COMPILE_KEIL
    _LowLevelInit();                                                     // configure watchdog and set the CPU to operating speed (system variables will be initialised at latest here)
#endif
#if defined RUN_LOOPS_IN_RAM
    fnInitDriver();                                                      // initialise driver code in SRAM (must be first)
    #if defined USE_IP && ((!defined IP_RX_CHECKSUM_OFFLOAD && !defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined _WINDOWS)
    fnInitIP();                                                          // initialise IP routines to run from SRAM
    #endif
#endif
#if !defined DEVICE_WITHOUT_DMA && !defined KINETIS_KL
    #if defined DEVICE_WITH_TWO_DMA_GROUPS
    DMA_CR = (DMA_CR_GRP0PRI_0 | DMA_CR_GRP1PRI_1);                      // set the two DMA groups to non-conflicting priorities
    #else
    DMA_CR = 0;
    #endif
#endif
#if defined DMA_MEMCPY_SET && !defined DEVICE_WITHOUT_DMA                // set the eDMA registers to a known zero state
    {
    #if !defined KINETIS_KL && !defined KINETIS_KE                       // {80}
        unsigned long *ptr_eDMAdes = (unsigned long *)eDMA_DESCRIPTORS;
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS; // {9}
        ptrDMA_TCD += DMA_MEMCPY_CHANNEL;
        while (ptr_eDMAdes < eDMA_DESCRIPTORS_END) {
            *ptr_eDMAdes++ = 0;                                          // clear out DMA descriptors after reset
        }
        ptrDMA_TCD->DMA_TCD_SOFF = 4;                                    // {87} source increment one long word for uMemcpy()
        ptrDMA_TCD->DMA_TCD_DOFF = 4;                                    // destination increment one long word
        ptrDMA_TCD->DMA_TCD_BITER_ELINK = 1;
        #if defined DMA_CHANNEL_0_PRIORITY                               // user defined channel priorities
        _SET_DMA_CHANNEL_PRIORITY(0, DMA_CHANNEL_0_PRIORITY);            // DMA priority, whereby channel can suspend a lower priority channel
        _SET_DMA_CHANNEL_PRIORITY(1, DMA_CHANNEL_1_PRIORITY);            // all channel priorities are set before use since it can be dangerous to change them later when DMA operations could take place during the process
        _SET_DMA_CHANNEL_PRIORITY(2, DMA_CHANNEL_2_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(3, DMA_CHANNEL_3_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(4, DMA_CHANNEL_4_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(5, DMA_CHANNEL_5_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(6, DMA_CHANNEL_6_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(7, DMA_CHANNEL_7_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(8, DMA_CHANNEL_8_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(9, DMA_CHANNEL_9_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(10, DMA_CHANNEL_10_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(11, DMA_CHANNEL_11_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(12, DMA_CHANNEL_12_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(13, DMA_CHANNEL_13_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(14, DMA_CHANNEL_14_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(15, DMA_CHANNEL_15_PRIORITY);
        _SET_DMA_CHANNEL_CHARACTERISTIC(DMA_MEMCPY_CHANNEL, (DMA_DCHPRI_ECP | DMA_DCHPRI_DPA)); // can be pre-empted by higher priority channel - it is expected that this channel will normally have priority 0
        #else                                                            // leave default channel priority (equal to the corresponding channel number)
        _SET_DMA_CHANNEL_PRIORITY(DMA_MEMCPY_CHANNEL, (/*DMA_DCHPRI_ECP | */DMA_DCHPRI_DPA | 0)); // lowest DMA priority and can be pre-empted by higher priority channel
            #if DMA_MEMCPY_CHANNEL != 0
        _SET_DMA_CHANNEL_PRIORITY(0, (DMA_MEMCPY_CHANNEL));              // no two priorities may ever be the same when the controller is used - switch priorities to avoid
            #endif
        #endif
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // {87} default transfer sizes long words
    #endif
    }
#endif
#if !defined KINETIS_KL && !defined KINETIS_KE
    #if defined CONFIGURE_CROSSBAR_SWITCH
    CONFIGURE_CROSSBAR_SWITCH();                                         // {85}
    #endif
#endif
#if defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK
    uMemset(ulFlashRow, 0xff, FLASH_ROW_SIZE);                           // initialise intermediate phrase memory
#endif
#if !defined TICK_USES_LPTMR && !defined TICK_USES_RTC
    SYSTICK_RELOAD = SYSTICK_COUNT_MASK;                                 // temporarily set maximum reload value
    SYSTICK_CURRENT = SYSTICK_COUNT_MASK;                                // write to the current value to cause the counter value to be reset to 0 and the reload value be set
    (void)SYSTICK_CSR;                                                   // ensure that the SYSTICK_COUNTFLAG flag is cleared
    SYSTICK_CSR = (SYSTICK_CORE_CLOCK | SYSTICK_ENABLE);                 // allow SYSTICK to run so that loop delays can already use it
  //while ((SYSTICK_CSR & SYSTICK_COUNTFLAG) == 0) {                     // wait for the reload to take place (should be instantaneous)
  //#if defined _WINDOWS
  //    SYSTICK_CSR |= SYSTICK_COUNTFLAG;
  //#endif
  //}
#endif
    fnUserHWInit();                                                      // allow the user to initialise hardware specific things - note that heap can not be used by this routine
#if defined _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
#endif
#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    // Power up the SPI interface, configure the pins used and select the mode and speed
    //
    POWER_UP_SPI_FLASH_INTERFACE();
    CONFIGURE_SPI_FLASH_INTERFACE();                                     // configure SPI interface for maximum possible speed (after TICK has been configured due to poential use of delay routine)
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
        #include "spi_flash_kinetis_atmel.h"
        #include "spi_flash_kinetis_stmicro.h"
        #include "spi_flash_kinetis_sst25.h"
        #include "spi_flash_w25q.h"
        #include "spi_flash_kinetis_s25fl1-k.h"
    #undef _CHECK_SPI_CHIPS
#endif
}


/* =================================================================== */
/*                    General Interrupt Control                        */
/* =================================================================== */

// Routine to disable interrupts during critical region
//
extern void uDisable_Interrupt(void)
{
#if defined _WINDOWS
    kinetis.CORTEX_M4_REGS.ulPRIMASK = INTERRUPT_MASKED;                 // mark that interrupts are masked
#else
    __disable_interrupt();                                               // disable interrupts to core
#endif
    iInterruptLevel++;                                                   // monitor the level of disable nesting
}

// Routine to re-enable interrupts on leaving a critical region (IAR uses intrinsic function)
//
extern void uEnable_Interrupt(void)
{
#if defined _WINDOWS
    if (iInterruptLevel == 0) {
        // A routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
        //
        _EXCEPTION("Unsymmetrical use of interrupt disable/enable detected!!");
    }
#endif
    if ((--iInterruptLevel) == 0) {                                      // only when no more interrupt nesting,
#if defined _WINDOWS
        kinetis.CORTEX_M4_REGS.ulPRIMASK = 0;                            // mark that interrupts are not masked
#else
        __enable_interrupt();                                            // enable processor interrupts
#endif
    }
}

// Routine to change interrupt level mask
//
#if !defined _COMPILE_KEIL
extern void uMask_Interrupt(unsigned char ucMaskLevel)                    // {102}
{
    #if !defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_KV && !defined _WINDOWS
    asm("msr basepri, r0");                                               // modify the base priority to block interrupts with a lower priority than this level
    asm("bx lr");                                                         // return
    #endif
}
#endif

// Function used to enter processor interrupts
//
extern void fnEnterInterrupt(int iInterruptID, unsigned char ucPriority, void (*InterruptFunc)(void)) // {55}
{
    volatile unsigned long *ptrIntSet = IRQ0_31_SER_ADD;                 // {73}
    volatile unsigned char *ptrPriority = IRQ0_3_PRIORITY_REGISTER_ADD;  // {73}
#if !defined INTERRUPT_VECTORS_IN_FLASH                                  // {111}
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    void (**processor_ints)(void);
#endif
#if defined _WINDOWS                                                     // back up the present enabled interrupt registers
    unsigned long ulState0 = IRQ0_31_SER;
    unsigned long ulState1 = IRQ32_63_SER;
    unsigned long ulState2 = IRQ64_95_SER;
    IRQ0_31_SER = IRQ32_63_SER = IRQ64_95_SER = IRQ0_31_CER = IRQ32_63_CER = IRQ64_95_CER = 0; // reset registers
#endif
#if !defined INTERRUPT_VECTORS_IN_FLASH
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;    // first processor interrupt location in the vector table
    processor_ints += iInterruptID;                                      // move the pointer to the location used by this interrupt number
    *processor_ints = InterruptFunc;                                     // enter the interrupt handler into the vector tale
#endif
    ptrPriority += iInterruptID;                                         // move to the priority location used by this interrupt
    *ptrPriority = (ucPriority << __NVIC_PRIORITY_SHIFT);                // {48} define the interrupt's priority (16 levels for K and 4 levels for KE/KL)
    ptrIntSet += (iInterruptID/32);                                      // move to the interrupt enable register in which this interrupt is controlled
    *ptrIntSet = (0x01 << (iInterruptID%32));                            // enable the interrupt
#if defined _WINDOWS
    IRQ0_31_SER  |= ulState0;                                            // synchronise the interrupt masks
    IRQ32_63_SER |= ulState1;
    IRQ64_95_SER |= ulState2;
    IRQ0_31_CER   = IRQ0_31_SER;
    IRQ32_63_CER  = IRQ32_63_SER;
    IRQ64_95_CER  = IRQ64_95_SER;
#endif
}


/* =================================================================== */
/*                                 TICK                                */
/* =================================================================== */

static __interrupt void _RealTimeInterrupt(void)
{
#if defined TICK_USES_LPTMR                                              // {94} tick interrupt from low power timer
    LPTMR0_CSR = LPTMR0_CSR;                                             // clear pending interrupt
#elif defined TICK_USES_RTC                                              // {100} tick interrupt from RTC
    RTC_SC |= RTC_SC_RTIF;                                               // clear pending interrupt
#else                                                                    // tick interrupt from systick
    INT_CONT_STATE_REG = PENDSTCLR;                                      // reset interrupt
    #if defined _WINDOWS
    INT_CONT_STATE_REG &= ~(PENDSTSET | PENDSTCLR);
    #endif
#endif
    uDisable_Interrupt();                                                // ensure tick handler cannot be interrupted
        fnRtmkSystemTick();                                              // operating system tick
    uEnable_Interrupt();
}


// Routine to initialise the tick interrupt (uses Cortex M4/M0+ SysTick timer, RTC or low power timer)
//
extern void fnStartTick(void)
{
#if defined TICK_USES_LPTMR                                              // {94} use the low power timer to derive the tick interrupt from
    POWER_UP(5, SIM_SCGC5_LPTIMER);                                      // ensure that the timer can be accessed
    LPTMR0_CSR = LPTMR_CSR_TCF;                                          // reset the timer and ensure no pending interrupts
    #if defined LPTMR_CLOCK_LPO                                          // define the low power clock speed for calculations
    LPTMR0_PSR = (LPTMR_PSR_PCS_LPO | LPTMR_PSR_PBYP);
    #elif defined LPTMR_CLOCK_INTERNAL_30_40kHz
    MCG_C2 &= ~MCG_C2_IRCS;                                              // select slow internal reference clock
    LPTMR0_PSR = (LPTMR_PSR_PCS_MCGIRCLK; | LPTMR_PSR_PBYP);
    #elif defined LPTMR_CLOCK_INTERNAL_4MHz
    MCG_C2 |= MCG_C2_IRCS;                                               // select fast internal reference clock
    LPTMR0_PSR = (LPTMR_PSR_PCS_MCGIRCLK | LPTMR_PSR_PBYP);
    #elif defined LPTMR_CLOCK_EXTERNAL_32kHz
    LPTMR0_PSR = (LPTMR_PSR_PCS_ERCLK32K | LPTMR_PSR_PBYP);
    #else                                                                // LPTMR_CLOCK_OSCERCLK
    OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                       // enable the external reference clock and keep it enabled in stop mode
        #if defined LPTMR_PRESCALE
    LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | ((LPTMR_PRESCALE_VALUE) << LPTMR_PSR_PRESCALE_SHIFT)); // program prescaler
        #else
    LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | LPTMR_PSR_PBYP);
        #endif
    #endif
    fnEnterInterrupt(irq_LPT_ID, LPTMR0_INTERRUPT_PRIORITY, (void (*)(void))_RealTimeInterrupt); // enter interrupt handler
    LPTMR0_CSR |= LPTMR_CSR_TIE;                                         // enable timer interrupt
    LPTMR0_CMR = LPTMR_MS_DELAY(TICK_RESOLUTION);                        // TICK period
    #if defined _WINDOWS
    if (LPTMR0_CMR > 0xffff) {
        _EXCEPTION("LPTMR0_CMR value too large (16 bits)");
    }
    #endif
    LPTMR0_CSR |= LPTMR_CSR_TEN;                                         // enable the low power timer
#elif defined TICK_USES_RTC                                              // {100} use RTC to derive the tick interrupt from
    POWER_UP(6, SIM_SCGC6_RTC);                                          // ensure the RTC is powered
    fnEnterInterrupt(irq_RTC_OVERFLOW_ID, PRIORITY_RTC, (void (*)(void))_RealTimeInterrupt); // enter interrupt handler
    #if defined RTC_USES_EXT_CLK || defined RTC_USES_INT_REF
    RTC_MOD = ((((TICK_RESOLUTION * _EXTERNAL_CLOCK)/RTC_CLOCK_PRESCALER_1)/1000) - 1); // set the match value
    #else
    RTC_MOD = ((((TICK_RESOLUTION * _EXTERNAL_CLOCK)/RTC_CLOCK_PRESCALER_2)/1000) - 1); // set the match value
    #endif
    RTC_SC = (RTC_SC_RTIE | RTC_SC_RTIF | _RTC_CLOCK_SOURCE | _RTC_PRESCALER); // clock the RTC from the defined clock source/pre-scaler and enable interrupt
    #if defined _WINDOWS
    if (RTC_MOD > 0xffff) {
        _EXCEPTION("MOD value too large (16 bits)");
    }
    #endif
#else                                                                    // use systick to derive the tick interrupt from
    #define REQUIRED_US ((1000000/(TICK_RESOLUTION)))                    // the TICK frequency we require in MHz
    #define TICK_DIVIDE (((CORE_CLOCK + REQUIRED_US/2)/REQUIRED_US) - 1) // the divide ratio required (for systick)

    #if TICK_DIVIDE > 0x00ffffff
        #error "TICK value cannot be achieved with SYSTICK at this core frequency!!"
    #endif
    #if !defined INTERRUPT_VECTORS_IN_FLASH                              // {111}
    VECTOR_TABLE *ptrVect;
        #if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)&vector_ram;
        #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
        #endif
    ptrVect->ptrSysTick = _RealTimeInterrupt;                            // enter interrupt handler
    #endif
    SYSTICK_RELOAD = TICK_DIVIDE;                                        // set reload value to determine the period
    SYSTICK_CURRENT = TICK_DIVIDE;                                       // {119}
    SYSTEM_HANDLER_12_15_PRIORITY_REGISTER |= (unsigned long)(SYSTICK_PRIORITY << (24 + __NVIC_PRIORITY_SHIFT)); // {116} enter the SYSTICK priority
    SYSTICK_CSR = (SYSTICK_CORE_CLOCK | SYSTICK_ENABLE | SYSTICK_TICKINT); // enable timer and its interrupt
    #if defined _WINDOWS
    SYSTICK_RELOAD &= SYSTICK_COUNT_MASK;                                // mask any values which are out of range
    SYSTICK_CURRENT = SYSTICK_RELOAD;                                    // prime the reload count
    #endif
#endif
#if defined MONITOR_PERFORMANCE                                          // configure a timer that will be used to measure the duration of task operation
    INITIALISE_MONITOR_TIMER();
#endif
}

/* =================================================================== */
/*                             Watchdog                                */
/* =================================================================== */

// Support watchdog re-triggering of specific hardware
//
extern void fnRetriggerWatchdog(void)
{
#if defined KINETIS_KL && !defined KINETIS_KL82                          // {67}
    if (SIM_COPC & SIM_COPC_COPT_LONGEST) {                              // if the COP is enabled
        SIM_SRVCOP = SIM_SRVCOP_1;                                       // issue COP service sequency
        SIM_SRVCOP = SIM_SRVCOP_2;
    }
#elif defined KINETIS_KE
    if (WDOG_CS1 & WDOG_CS1_EN) {                                        // if watchdog is enabled
        uDisable_Interrupt();                                            // protect the refresh sequence from interrupts
            REFRESH_WDOG();
        uEnable_Interrupt();
    }
#else
    if ((WDOG_STCTRLH & WDOG_STCTRLH_WDOGEN) != 0) {                     // if watchdog is enabled
        uDisable_Interrupt();                                            // protect the refresh sequence from interrupts
            REFRESH_WDOG();
        uEnable_Interrupt();
    }
#endif
    TOGGLE_WATCHDOG_LED();                                               // optionally flash a watchdog (heart-beat) LED
}

#if !defined DEVICE_WITHOUT_DMA
/* =================================================================== */
/*                                 DMA                                 */
/* =================================================================== */
    #if defined SUPPORT_ADC || (defined SUPPORT_DAC && (DAC_CONTROLLERS > 0)) || (defined SUPPORT_TIMER && defined SUPPORT_PWM_MODULE && (FLEX_TIMERS_AVAILABLE > 0))
#define _DMA_SHARED_CODE
    #include "kinetis_DMA.h"                                             // include driver code for peripheral/buffer DMA
#undef _DMA_SHARED_CODE
    #endif
#define _DMA_MEM_TO_MEM
    #include "kinetis_DMA.h"                                             // include memory-memory transfer code 
#undef _DMA_MEM_TO_MEM
#endif

#if (defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET)
/* =================================================================== */
/*                          Ethernet Controller                        */
/* =================================================================== */
    #include "kinetis_ENET.h"                                            // include Ethernet controller hardware driver code
#endif


#if defined USB_INTERFACE
/* =================================================================== */
/*                                USB                                  */
/* =================================================================== */
    #if defined USB_HS_INTERFACE
        #include "kinetis_USB_HS_Device.h"                               // include USB controller hardware HS device driver code
    #else
        #include "kinetis_USB_OTG.h"                                     // {121} include USB controller hardware OTG driver code
    #endif
#endif

#if defined SERIAL_INTERFACE
/* =================================================================== */
/*                    Serial Interface - UART                          */
/* =================================================================== */
    #include "kinetis_UART.h"                                            // include UART/LPUART hardware driver code
#endif


#if defined CAN_INTERFACE
/* =================================================================== */
/*                            FlexCAN/MSCAN                            */
/* =================================================================== */
    #include "kinetis_CAN.h"                                             // include FlexCAN/MSCAN hardware driver code
#endif


#if defined I2C_INTERFACE
/* =================================================================== */
/*                                  I2C                                */
/* =================================================================== */
    #include "kinetis_I2C.h"                                             // include I2C hardware driver code
#endif


#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined SUPPORT_PROGRAM_ONCE
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */
    #include "kinetis_FLASH.h"                                           // include FLASH driver code
#endif


#if defined SDCARD_SUPPORT && defined SD_CONTROLLER_AVAILABLE
/* =================================================================== */
/*                                SDHC                                 */
/* =================================================================== */
    #include "kinetis_SDHC.h"                                            // include SDHC driver code
#endif

#if (defined SUPPORT_RTC && !defined KINETIS_WITHOUT_RTC) || defined SUPPORT_SW_RTC
/* =================================================================== */
/*                           Real Time Clock                           */
/* =================================================================== */
    #include "kinetis_RTC.h"                                             // include RTC driver code
#endif

#if (defined SUPPORT_PITS || defined USB_HOST_SUPPORT) && !defined KINETIS_WITHOUT_PIT
/* =================================================================== */
/*                     Periodic Interrupt Timer (PIT)                  */
/* =================================================================== */
#define _PIT_CODE
    #include "kinetis_PIT.h"                                             // include PIT driver code
#undef _PIT_CODE
#endif


#if defined SUPPORT_LPTMR && !defined TICK_USES_LPTMR && !defined KINETIS_KE // {86}
/* =================================================================== */
/*                       Low Power Timer (LPTMR)                       */
/* =================================================================== */
#define _LPTMR_CODE
    #include "kinetis_LPTMR.h"                                          // include LPTMR driver code
#undef _LPTMR_CODE
#endif


#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)                 // {72} basic timer support based on FlexTimer
/* =================================================================== */
/*                              FlexTimer                              */
/* =================================================================== */
#define _FLEXTIMER_CODE
    #include "kinetis_FLEXTIMER.h"                                       // include FlexTimer driver code
#undef _FLEXTIMER_CODE
#endif

#if defined SUPPORT_TIMER && defined SUPPORT_PWM_MODULE && (FLEX_TIMERS_AVAILABLE > 0)
/* =================================================================== */
/*                                 PWM                                 */
/* =================================================================== */
#define _PWM_CODE
    #include "kinetis_PWM.h"                                             // include PWM configuration code
#undef _PWM_CODE
#endif


#if defined SUPPORT_PDB && !defined KINETIS_KL && !defined KINETIS_KE    // {58}
/* =================================================================== */
/*                    Programmable Delay Block (PDB)                   */
/* =================================================================== */
#define _PDB_CODE
    #include "kinetis_PDB.h"                                             // include PDB driver code
#undef _PDB_CODE
#endif


#if defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 0)
/* =================================================================== */
/*                        Key Board Interrupt                          */
/* =================================================================== */
#define _KBI_INTERRUPT_CODE
    #include "kinetis_KBI.h"                                             // include KBI interrupt driver code
#undef _KBI_INTERRUPT_CODE
#endif


#if defined SUPPORT_LOW_POWER && defined LLWU_AVAILABLE && defined SUPPORT_LLWU // {112}
/* =================================================================== */
/*                       Low Leakage Wake Up (LLWU)                    */
/* =================================================================== */
#define _LLWU_INTERRUPT_CODE
    #include "kinetis_LLWU.h"                                            // include LLWU interrupt driver code
#undef _LLWU_INTERRUPT_CODE
#endif

#if defined SUPPORT_I2S_SAI && (I2S_AVAILABLE > 0)
/* =================================================================== */
/*                                I2S / SAI                            */
/* =================================================================== */
#define _I2S_CODE
    #include "kinetis_I2S.h"                                             // include I2S/SAI driver code
#undef _I2S_CODE
#endif


#if defined SUPPORT_PORT_INTERRUPTS
/* =================================================================== */
/*                           Port Interrupts                           */
/* =================================================================== */
#define _PORT_INTERRUPT_CODE
    #include "kinetis_PORTS.h"                                           // include port interrupt driver code
#undef _PORT_INTERRUPT_CODE
#endif

#define _PORT_MUX_CODE
    #include "kinetis_PORTS.h"                                           // include port pin multiplexing code
#undef _PORT_MUX_CODE


#if defined SUPPORT_ADC                                                  // {5}
/* =================================================================== */
/*                                 ADC                                 */
/* =================================================================== */
#define _ADC_INTERRUPT_CODE
    #include "kinetis_ADC.h"                                             // include driver code for ADC
#undef _ADC_INTERRUPT_CODE
#endif


/* =================================================================== */
/*                General Peripheral/Interrupt Interface               */
/* =================================================================== */

// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
#if defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 0)
    case KEYBOARD_INTERRUPT:
    #define _KBI_CONFIG_CODE
        #include "kinetis_KBI.h"                                         // include KBI configuration code
    #undef _KBI_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_PORT_INTERRUPTS
    case PORT_INTERRUPT:
    #define _PORT_INT_CONFIG_CODE
        #include "kinetis_PORTS.h"                                       // include port interrupt configuration code
    #undef _PORT_INT_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_LOW_POWER && defined LLWU_AVAILABLE && defined SUPPORT_LLWU // {112}
    case WAKEUP_INTERRUPT:
    #define _LLWU_CONFIG_CODE
        #include "kinetis_LLWU.h"                                        // include LLWU configuration code
    #undef _LLWU_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_I2S_SAI && (I2S_AVAILABLE > 0)
    case I2S_SAI_INTERRUPT:
    #define _I2S_SAI_CONFIG_CODE
        #include "kinetis_I2S.h"                                         // include I2S/SAI configuration code
    #undef _I2S_SAI_CONFIG_CODE
        break;
#endif
#if (defined SUPPORT_PITS || defined USB_HOST_SUPPORT) && !defined KINETIS_WITHOUT_PIT // up to 4 x 32bit PITs in most Kinetis devices
    case PIT_INTERRUPT:
    #define _PIT_CONFIG_CODE
        #include "kinetis_PIT.h"                                         // include PIT configuration code
    #undef _PIT_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_TIMER && defined SUPPORT_PWM_MODULE && (FLEX_TIMERS_AVAILABLE > 0)
    case PWM_INTERRUPT:
    #define _PWM_CONFIG_CODE
        #include "kinetis_PWM.h"                                         // include PWM configuration code
    #undef _PWM_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)                 // {72}
    case TIMER_INTERRUPT:                                                // FlexTimer used in periodic or monostable interrupt mode (bus clock source)
    #define _FLEXTIMER_CONFIG_CODE
        #include "kinetis_FLEXTIMER.h"                                   // include FlexTimer configuration code
    #undef _FLEXTIMER_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_PDB                                                  // {58}
    case PDB_INTERRUPT:
    #define _PDB_CONFIG_CODE
        #include "kinetis_PDB.h"                                         // include PDM configuration code
    #undef _PDB_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_LPTMR && !defined TICK_USES_LPTMR && !defined KINETIS_KE // {86} [prescale bypassed]
    case LPTMR_INTERRUPT:
    #define _LPTMR_CONFIG_CODE
        #include "kinetis_LPTMR.h"                                      // include LPTMR configuration code
    #undef _LPTMR_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_DAC && (DAC_CONTROLLERS > 0)                         // {39}
    case DAC_INTERRUPT:
    #define _DAC_CONFIG_CODE
        #include "kinetis_DAC.h"                                         // include DAC configuration code
    #undef _DAC_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_ADC                                                  // {5}
    case ADC_INTERRUPT:
    #define _ADC_CONFIG_CODE
        #include "kinetis_ADC.h"                                         // include ADC configuration code
    #undef _ADC_CONFIG_CODE
        break;
#endif
    default: 
        _EXCEPTION("Attempting configuration of interrupt interface without appropriate support enabled!!");
        break;
    }
}


/* =================================================================== */
/*                     Dynamic Low Power Interface                     */
/* =================================================================== */

#if defined SUPPORT_LOW_POWER
    #include "kinetis_low_power.h"                                       // include driver code for low power mode
#endif


/* =================================================================== */
/*                                 Reset                               */
/* =================================================================== */

// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);             // request Cortex core reset, which will cause the software reset bit to be set in the mode controller for recognition after restart
#if !defined _WINDOWS
    while (1) {}
#endif
}

#if defined CLKOUT_AVAILABLE && !defined KINETIS_WITH_PCC
extern int fnClkout(int iClockSource)                                    // {120}
{
    unsigned long ulSIM_SOPT2 = (SIM_SOPT2 & ~(SIM_SOPT2_CLKOUTSEL_MASK)); // original control register value with clock source masked
    switch (iClockSource) {                                              // set the required clock source to be output on CLKOUT
    case FLASH_CLOCK_OUT:
    case BUS_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_FLASH;
        break;
    case LOW_POWER_OSCILLATOR_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_LPO;
        break;
    case INTERNAL_LIRC_CLOCK_OUT:                                        // same as INTERNAL_MCGIRCLK_CLOCK_OUT
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_MCGIRCLK;
        break;
    case EXTERNAL_OSCILLATOR_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_OSCERCLK0;
        break;
    #if defined KINETIS_HAS_IRC48M
    case INTERNAL_IRC48M_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_IRC48M;
        break;
    #endif
    case RTC_CLOCK_OUT:
    #if defined KINETIS_KL03
        _CONFIG_PERIPHERAL(B, 13, (PB_13_RTC_CLKOUT | PORT_SRE_SLOW | PORT_DSE_LOW)); // configure the RTC_CLKOUT pin
    #elif defined KINETIS_K64 || defined KINETIS_K65 || defined KINETIS_K66
        #if defined RTC_CLKOUT_ON_PTE_LOW
            _CONFIG_PERIPHERAL(E, 0, (PE_0_RTC_CLKOUT | PORT_SRE_SLOW | PORT_DSE_LOW)); // configure the RTC_CLKOUT pin (alt. 7)
        #else
            _CONFIG_PERIPHERAL(E, 26, (PE_26_RTC_CLKOUT | PORT_SRE_SLOW | PORT_DSE_LOW)); // configure the RTC_CLKOUT pin (alt. 6)
        #endif
    #endif
        return 0;
    #if defined KINETIS_K64
    case FLEXBUS_CLOCK_OUT:
        ulSIM_SOPT2 &= ~(SIM_SOPT2_CLKOUTSEL_IRC48M);
    #endif
    default:
        return -1;                                                       // invalid clock source
    }
    SIM_SOPT2 = ulSIM_SOPT2;                                             // select the clock source to be driven to the CLKOUT pin
    #if defined KINETIS_KL03
    _CONFIG_PERIPHERAL(A, 12, (PA_12_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin (PA_4_CLKOUT would be an alternative possibility)
    #elif defined KINETIS_KL05
    _CONFIG_PERIPHERAL(A, 15, (PA_15_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin (PA_4_CLKOUT would be an alternative possibility)
    #elif defined KINETIS_K64 && (PIN_COUNT == PIN_COUNT_144_PIN)
    _CONFIG_PERIPHERAL(A, 6, (PA_6_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin
    #else
    _CONFIG_PERIPHERAL(C, 3, (PC_3_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin
    #endif
    return 0;                                                            // valid clock source has been selected
}
#endif

#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
// Routine to request local SPI FLASH type
//
extern unsigned char fnSPI_Flash_available(void)
{
    return ucSPI_FLASH_Type[0];
}

    #if defined SPI_FLASH_MULTIPLE_CHIPS
extern unsigned char fnSPI_FlashExt_available(int iExtension)
{
    if (iExtension > SPI_FLASH_DEVICE_COUNT) {
        return 0;
    }
    return ucSPI_FLASH_Type[iExtension];
}
    #endif
#endif


#if defined USE_SDRAM                                                    // {38}
static void fnConfigureSDRAM(void)
{
/*  The following SDRAM devices are supported
    MT47H64M16HR-25                                                         used on K70 tower kit board
    MT46H32M16LFBF-5                                                        default
    MT46H32M16LFBF-6                                                        (compatible)
*/
    POWER_UP(3, SIM_SCGC3_DDR);                                          // power up the DDR controller

    // The DDR controller requires MCGDDRCLK2, which is generated by PLL1 ({90} moved to _LowLevelInit() so that it can be shared by FS USB)
    //
    #define KINETIS_DDR_SPEED ((_EXTERNAL_CLOCK * CLOCK_MUL_1)/CLOCK_DIV_1/2) // speed of DDR clock assuming external oscillator used

    #if defined MT47H64M16HR                                             // synchronous DDR2
    SIM_MCR |= (SIM_MCR_DDRCFG_DDR2 | SIM_MCR_DDRPEN);                   // enable all DDR I/O pins at full-strength (slew-rate)
    DDR_RCR = (DDR_RCR_RST);                                             // command reset of the DDR PHY (the register always read 0x00000000)
    DDR_PAD_CTRL = (DDR_PAD_CTRL_FIXED | DDR_PAD_CTRL_SPARE_DLY_CTRL_10_BUFFERS | DDR_PAD_CTRL_PAD_ODT_CS0_75_OHMS | 0x00030000); // note that 0x00030000 is not documented but taken from freescale example
    DDR_CR00 = DDR_CR00_DDRCLS_DDR2;                                      // set the class type to DDR2
    DDR_CR02 = ((2 << DDR_CR02_INITAREF_SHIFT) | (49 << DDR_CR02_TINIT_SHIFT));
    DDR_CR03 = ((2 << DDR_CR03_TCCD_SHIFT) | (2 << DDR_CR03_WRLAT_SHIFT) | (5 << DDR_CR03_LATGATE_SHIFT) | (6 << DDR_CR03_LATLIN_SHIFT));
    DDR_CR04 = ((2 << DDR_CR04_TBINT_SHIFT) | (2 << DDR_CR04_TRRD_SHIFT) | (9 << DDR_CR04_TRC_SHIFT) | (6 << DDR_CR04_TRASMIN_SHIFT));
    DDR_CR05 = ((2 << DDR_CR05_TWTR_SHIFT) | (3 << DDR_CR05_TRP_SHIFT) | (2 << DDR_CR05_TRTP_SHIFT) | (2 << DDR_CR05_TMRD_SHIFT));
    DDR_CR06 = ((2 << DDR_CR06_TMOD_SHIFT) | (36928 << DDR_CR06_TRASMAX_SHIFT) | 0x02000000); // note that 0x02000000 is not documented but taken from freescale example
    DDR_CR07 = (DDR_CR07_CCAPEN | (3 << DDR_CR07_TCKESR_SHIFT) | (3 << DDR_CR07_CLKPW_SHIFT));
    DDR_CR08 = ((5 << DDR_CR08_TDAL_SHIFT) | (3 << DDR_CR08_TWR_SHIFT) | (2 << DDR_CR08_TRASDI_SHIFT) | DDR_CR08_TRAS);
    DDR_CR09 = ((2 << DDR_CR09_BSTLEN_SHIFT) | (200 << DDR_CR09_TDLL_SHIFT));
    DDR_CR10 = ((3 << DDR_CR10_TRPAB_SHIFT) | (50 << DDR_CR10_TCPD_SHIFT) | (7 << DDR_CR10_TFAW_SHIFT));
    DDR_CR11 = DDR_CR11_TREFEN;                                          // enable refresh commands
    DDR_CR12 = ((49 << DDR_CR12_TRFC_SHIFT) | (1170 << DDR_CR12_TREF_SHIFT));
    DDR_CR13 = (5 << DDR_CR13_TREFINT_SHIFT);
    DDR_CR14 = ((200 << DDR_CR14_TXSR_SHIFT) | (2 << DDR_CR14_TPDEX_SHIFT));
    DDR_CR15 = (50 << DDR_CR15_TXSNR_SHIFT);
    DDR_CR16 = DDR_CR16_QKREF;
    DDR_CR20 = ((3 << DDR_CR20_CKSRX_SHIFT) | (3 << DDR_CR20_CKSRE_SHIFT));
    DDR_CR21 = 0x00040232;
    DDR_CR22 = 0;
    DDR_CR23 = 0x00040302;
    DDR_CR25 = ((10 << DDR_CR25_APREBIT_SHIFT) | (1 << DDR_CR25_COLSIZ_SHIFT) | (2 << DDR_CR25_ADDPINS_SHIFT) | DDR_CR25_BNK8);
    DDR_CR26 = (DDR_CR26_ADDCOL | DDR_CR26_BNKSPT | (255 << DDR_CR26_CMDAGE_SHIFT) | (255 << DDR_CR26_AGECNT_SHIFT));
    DDR_CR27 = (DDR_CR27_PLEN | DDR_CR27_PRIEN | DDR_CR27_RWEN | DDR_CR27_SWPEN);
    DDR_CR28 = (DDR_CR28_CSMAP | 0x00000002);                            // note that 0x00000002 is not documented but taken from freescale example
    DDR_CR29 = 0;
    DDR_CR30 = 1;
    DDR_CR34 = 0x02020101;
    DDR_CR36 = 0x01010201;
    DDR_CR37 = ((0 << DDR_CR37_R2RSAME_SHIFT) | (2 << DDR_CR37_R2WSAME_SHIFT) | (0 << DDR_CR37_W2RSAME_SHIFT) | (0 << DDR_CR37_W2WSAME_SHIFT));
    DDR_CR38 = (32 << DDR_CR38_PWRCNT_SHIFT);
    DDR_CR39 = ((1 << DDR_CR39_WP0_SHIFT) | (1 << DDR_CR39_RP0_SHIFT) | (32 << DDR_CR39_P0RDCNT_SHIFT));
    DDR_CR40 = ((32 << DDR_CR40_P1WRCNT_SHIFT) | DDR_CR40_P0TYP_ASYNC);
    DDR_CR41 = ((1 << DDR_CR41_WP1_SHIFT) | (1 << DDR_CR41_RP1_SHIFT) | (32 << DDR_CR41_P1RDCNT_SHIFT));
    DDR_CR42 = ((32 << DDR_CR42_P2WRCNT_SHIFT) | DDR_CR42_P1TYP_ASYNC);
    DDR_CR43 = ((1 << DDR_CR43_WP2_SHIFT) | (1 << DDR_CR43_RP2_SHIFT) | (32 << DDR_CR43_P2RDCNT_SHIFT));
    DDR_CR44 = (DDR_CR44_P2TYP_ASYNC);
    DDR_CR45 = ((3 << DDR_CR45_P0PRI0_SHIFT) | (3 << DDR_CR45_P0PRI1_SHIFT) | (3 << DDR_CR45_P0PRI2_SHIFT) | (3 << DDR_CR45_P0PRI3_SHIFT));
    DDR_CR46 = ((2 << DDR_CR46_P1PRI0_SHIFT) | (100 << DDR_CR46_P0PRIRLX_SHIFT) | (1 << DDR_CR46_P0ORD_SHIFT));
    DDR_CR47 = ((2 << DDR_CR47_P1PRI1_SHIFT) | (2 << DDR_CR47_P1PRI2_SHIFT) | (2 << DDR_CR47_P1PRI3_SHIFT) | (1 << DDR_CR47_P1ORD_SHIFT));
    DDR_CR48 = ((1 << DDR_CR48_P2PRI0_SHIFT) | (100 << DDR_CR48_P1PRIRLX_SHIFT) | (1 << DDR_CR48_P2PRI1_SHIFT));
    DDR_CR49 = ((2 << DDR_CR49_P2ORD_SHIFT) | (1 << DDR_CR49_P2PRI2_SHIFT) | (1 << DDR_CR49_P2PRI3_SHIFT));
    DDR_CR50 = (100 << DDR_CR50_P2PRIRLX_SHIFT);
    DDR_CR52 = ((2 << DDR_CR52_RDDTENBAS_SHIFT) | (6 << DDR_CR52_PHYRDLAT_SHIFT) | (2 << DDR_CR52_PYWRLTBS_SHIFT));
    DDR_CR53 = (968 << DDR_CR53_CRTLUDMX_SHIFT);
    DDR_CR54 = ((968 << DDR_CR54_PHYUPDTY0_SHIFT) | (968 << DDR_CR54_PHYUPDTY1_SHIFT));
    DDR_CR55 = ((968 << DDR_CR55_PHYUPDTY2_SHIFT) | (968 << DDR_CR55_PHYUPDTY3_SHIFT));
    DDR_CR56 = ((2 << DDR_CR56_WRLATADJ_SHIFT) | (3 << DDR_CR56_RDLATADJ_SHIFT) | (968 << DDR_CR56_PHYUPDRESP_SHIFT));
    DDR_CR57 = ((1 << DDR_CR57_CLKENDLY_SHIFT) | (2 << DDR_CR57_CMDDLY_SHIFT) | DDR_CR57_ODTALTEN);
    #else                                                                // MT46H32M16LFBF-5/6 - low power DDR
    SIM_MCR |= (SIM_MCR_DDRCFG_LPDDR_2 | SIM_MCR_DDRPEN);                // enable all DDR I/O pins at low-power half-strength (slew-rate)
    DDR_RCR = (DDR_RCR_RST);                                             // command reset of the DDR PHY (the register always read 0x00000000)
    DDR_PAD_CTRL = (DDR_PAD_CTRL_FIXED | DDR_PAD_CTRL_SPARE_DLY_CTRL_10_BUFFERS | DDR_PAD_CTRL_PAD_ODT_CS0_ODT_DISABLED);
    DDR_CR00 = DDR_CR00_DDRCLS_LPDDR;                                    // set the class type to low power DDR
    DDR_CR02 = ((2 << DDR_CR02_INITAREF_SHIFT) | (50 << DDR_CR02_TINIT_SHIFT));
    DDR_CR03 = ((2 << DDR_CR03_TCCD_SHIFT) | (1 << DDR_CR03_WRLAT_SHIFT) | (6 << DDR_CR03_LATGATE_SHIFT) | (6 << DDR_CR03_LATLIN_SHIFT));
        #if KINETIS_DDR_SPEED >= 150000000                               // suitable settings for 150MHz operation
    DDR_CR04 = ((1 << DDR_CR04_TBINT_SHIFT) | (2 << DDR_CR04_TRRD_SHIFT) | (10 << DDR_CR04_TRC_SHIFT) | (7 << DDR_CR04_TRASMIN_SHIFT));
    DDR_CR06 = ((0 << DDR_CR06_TMOD_SHIFT) | (10500 << DDR_CR06_TRASMAX_SHIFT)); // 70us time row access maximum
    DDR_CR08 = ((6 << DDR_CR08_TDAL_SHIFT) | (3 << DDR_CR08_TWR_SHIFT) | (3 << DDR_CR08_TRASDI_SHIFT));
    DDR_CR10 = ((3 << DDR_CR10_TRPAB_SHIFT) | (30000 << DDR_CR10_TCPD_SHIFT) | (0 << DDR_CR10_TFAW_SHIFT));
    DDR_CR12 = ((15 << DDR_CR12_TRFC_SHIFT) | (1170 << DDR_CR12_TREF_SHIFT)); // 7.8us DRAM cycles between refresh commands
    DDR_CR14 = ((19 << DDR_CR14_TXSR_SHIFT) | (2 << DDR_CR14_TPDEX_SHIFT));
    DDR_CR15 = (19 << DDR_CR15_TXSNR_SHIFT);
        #elif (KINETIS_DDR_SPEED == 120000000) || (KINETIS_DDR_SPEED == 125000000) // suitable settings for 120MHz/125MHz operation
    DDR_CR04 = ((1 << DDR_CR04_TBINT_SHIFT) | (2 << DDR_CR04_TRRD_SHIFT) | (8 << DDR_CR04_TRC_SHIFT) | (6 << DDR_CR04_TRASMIN_SHIFT));
    DDR_CR06 = ((0 << DDR_CR06_TMOD_SHIFT) | (8400 << DDR_CR06_TRASMAX_SHIFT)); // 70us time row access maximum
    DDR_CR08 = ((4 << DDR_CR08_TDAL_SHIFT) | (2 << DDR_CR08_TWR_SHIFT) | (3 << DDR_CR08_TRASDI_SHIFT));
    DDR_CR10 = ((2 << DDR_CR10_TRPAB_SHIFT) | (24000 << DDR_CR10_TCPD_SHIFT) | (0 << DDR_CR10_TFAW_SHIFT));
    DDR_CR12 = ((12 << DDR_CR12_TRFC_SHIFT) | (936 << DDR_CR12_TREF_SHIFT)); // 7.8us DRAM cycles between refresh commands
    DDR_CR14 = ((15 << DDR_CR14_TXSR_SHIFT) | (2 << DDR_CR14_TPDEX_SHIFT));
    DDR_CR15 = (15 << DDR_CR15_TXSNR_SHIFT);
        #else
            #error "DDR speed should be 120MHz, 125MHz or 150MHz so that correct settings can be made "
        #endif
    DDR_CR05 = ((2 << DDR_CR05_TWTR_SHIFT) | (3 << DDR_CR05_TRP_SHIFT) | (2 << DDR_CR05_TRTP_SHIFT) | (2 << DDR_CR05_TMRD_SHIFT));
    DDR_CR07 = (DDR_CR07_CCAPEN | (1 << DDR_CR07_TCKESR_SHIFT) | (1 << DDR_CR07_CLKPW_SHIFT));
    DDR_CR09 = ((3 << DDR_CR09_BSTLEN_SHIFT) | (0 << DDR_CR09_TDLL_SHIFT));
    DDR_CR11 = DDR_CR11_TREFEN;                                          // enable refresh commands
    DDR_CR16 = DDR_CR16_QKREF;
    DDR_CR20 = ((3 << DDR_CR20_CKSRX_SHIFT) | (3 << DDR_CR20_CKSRE_SHIFT));

    // Memory chip mode register settings
    //
    #define LPDDR_MR_CL         3
    #define LPDDR_MR_BURST_TYPE 0
    #define LPDDR_MR_BL         3
    #define MT46H_CHIP_MODE_REGISTER ((LPDDR_MR_CL << 4) | (LPDDR_MR_BURST_TYPE << 3) | (LPDDR_MR_BL << 0))

    #define LPDDR_EMR_STRENGTH 1
    #define LPDDR_EMR_PASR     0
    #define MT46H_EXTENDED_MODE_REGISTER ((LPDDR_EMR_STRENGTH << 5) | (LPDDR_EMR_PASR << 0))

    DDR_CR21 = ((0 << DDR_CR21_MR1DATA0_SHIFT) | (MT46H_CHIP_MODE_REGISTER << DDR_CR21_MR0DATA0_SHIFT));
    DDR_CR22 = ((0 << DDR_CR22_MR3DATA0_SHIFT) | (MT46H_EXTENDED_MODE_REGISTER << DDR_CR22_MR2DATA0_SHIFT));
    DDR_CR25 = ((10 << DDR_CR25_APREBIT_SHIFT) | (1 << DDR_CR25_COLSIZ_SHIFT) | (3 << DDR_CR25_ADDPINS_SHIFT));
    DDR_CR26 = (DDR_CR26_ADDCOL | DDR_CR26_BNKSPT | (255 << DDR_CR26_CMDAGE_SHIFT) | (255 << DDR_CR26_AGECNT_SHIFT));
    DDR_CR27 = (DDR_CR27_PLEN | DDR_CR27_PRIEN | DDR_CR27_RWEN | DDR_CR27_SWPEN);
    DDR_CR28 = (DDR_CR28_CSMAP);
    DDR_CR29 = 0;
    DDR_CR30 = 0;
    DDR_CR34 = 0;
    DDR_CR37 = ((3 << DDR_CR37_R2RSAME_SHIFT) | (2 << DDR_CR37_R2WSAME_SHIFT) | (0 << DDR_CR37_W2RSAME_SHIFT) | (0 << DDR_CR37_W2WSAME_SHIFT));
    DDR_CR38 = (32 << DDR_CR38_PWRCNT_SHIFT);
    DDR_CR39 = ((1 << DDR_CR39_WP0_SHIFT) | (1 << DDR_CR39_RP0_SHIFT) | (32 << DDR_CR39_P0RDCNT_SHIFT));
    DDR_CR41 = ((1 << DDR_CR41_WP1_SHIFT) | (1 << DDR_CR41_RP1_SHIFT) | (32 << DDR_CR41_P1RDCNT_SHIFT));
    DDR_CR43 = ((1 << DDR_CR43_WP2_SHIFT) | (1 << DDR_CR43_RP2_SHIFT) | (32 << DDR_CR43_P2RDCNT_SHIFT));
        #if defined DDRPORT_SYNC
    DDR_CR40 = ((32 << DDR_CR40_P1WRCNT_SHIFT) | DDR_CR40_P0TYP_SYNC);
    DDR_CR42 = ((32 << DDR_CR42_P2WRCNT_SHIFT) | DDR_CR42_P1TYP_SYNC);
    DDR_CR44 = (DDR_CR44_P2TYP_SYNC);
        #else
    DDR_CR40 = ((32 << DDR_CR40_P1WRCNT_SHIFT) | DDR_CR40_P0TYP_ASYNC);
    DDR_CR42 = ((32 << DDR_CR42_P2WRCNT_SHIFT) | DDR_CR42_P1TYP_ASYNC);
    DDR_CR44 = (DDR_CR44_P2TYP_ASYNC);
        #endif
    DDR_CR45 = ((3 << DDR_CR45_P0PRI0_SHIFT) | (3 << DDR_CR45_P0PRI1_SHIFT) | (3 << DDR_CR45_P0PRI2_SHIFT) | (3 << DDR_CR45_P0PRI3_SHIFT));
    DDR_CR46 = ((2 << DDR_CR46_P1PRI0_SHIFT) | (100 << DDR_CR46_P0PRIRLX_SHIFT) | (1 << DDR_CR46_P0ORD_SHIFT));
    DDR_CR47 = ((2 << DDR_CR47_P1PRI1_SHIFT) | (2 << DDR_CR47_P1PRI2_SHIFT) | (2 << DDR_CR47_P1PRI3_SHIFT) | (1 << DDR_CR47_P1ORD_SHIFT));
    DDR_CR48 = ((1 << DDR_CR48_P2PRI0_SHIFT) | (100 << DDR_CR48_P1PRIRLX_SHIFT) | (1 << DDR_CR48_P2PRI1_SHIFT));
    DDR_CR49 = ((2 << DDR_CR49_P2ORD_SHIFT) | (1 << DDR_CR49_P2PRI2_SHIFT) | (1 << DDR_CR49_P2PRI3_SHIFT));
    DDR_CR50 = (100 << DDR_CR50_P2PRIRLX_SHIFT);
    DDR_CR52 = ((1 << DDR_CR52_RDDTENBAS_SHIFT) | (6 << DDR_CR52_PHYRDLAT_SHIFT) | (1 << DDR_CR52_PYWRLTBS_SHIFT));
    DDR_CR53 = (968 << DDR_CR53_CRTLUDMX_SHIFT);
    DDR_CR54 = ((968 << DDR_CR54_PHYUPDTY0_SHIFT) | (968 << DDR_CR54_PHYUPDTY1_SHIFT));
    DDR_CR55 = ((968 << DDR_CR55_PHYUPDTY2_SHIFT) | (968 << DDR_CR55_PHYUPDTY3_SHIFT));
    DDR_CR56 = ((2 << DDR_CR56_WRLATADJ_SHIFT) | (3 << DDR_CR56_RDLATADJ_SHIFT) | (968 << DDR_CR56_PHYUPDRESP_SHIFT));
    DDR_CR57 = ((1 << DDR_CR57_CLKENDLY_SHIFT) | (2 << DDR_CR57_CMDDLY_SHIFT));
    #endif

    fnDelayLoop(0);

    DDR_CR00 = (DDR_CR00_DDRCLS_DDR2 | DDR_CR00_START);                  // start command processing
    while ((DDR_CR30 & DDR_CR30_INT_DRAM_INIT) == 0) {                   // wait for completion
    #if defined _WINDOWS
        DDR_CR30 |= DDR_CR30_INT_DRAM_INIT;
    #endif
    }
    MCM_CR |= (MCM_CR_DDR_SIZE_128MB);                                   // enable DDR address size translation
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
    _EXCEPTION("undefined interrupt!!!");
}


#if defined (_GNU) || defined _CODE_WARRIOR                              // {110}
extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

// Variable initialisation
//
extern void __init_gnu_data(void)
{
    unsigned char *ptrData;
    unsigned long ulInitDataLength;
    #if !defined _RAM_DEBUG
    const unsigned char *ptrFlash = &__data_load_start__;
    ulInitDataLength = (&__data_end__ - &__data_start__);
    ptrData = &__data_start__;
    while (ulInitDataLength--) {                                         // initialise data
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__text_start__;
    ptrFlash = &__text_load_start__;
    if (ptrFlash != ptrData) {                                           // if a move is required
        ulInitDataLength = (&__text_end__ - &__text_start__);
        while (ulInitDataLength--) {                                     // initialise text
            *ptrData++ = *ptrFlash++;
        }
    }
    #endif
    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }
}
#endif


// Perform very low level initialisation - called by the start-up code
//
static void _LowLevelInit(void)
{
#if !defined INTERRUPT_VECTORS_IN_FLASH
    VECTOR_TABLE *ptrVect;
    #if !defined _MINIMUM_IRQ_INITIALISATION
    void (**processor_ints)(void);
    #endif
#endif
#if defined KINETIS_K64 && (defined RUN_FROM_HIRC_PLL || defined RUN_FROM_HIRC_FLL || defined RUN_FROM_HIRC) // older K64 devies require the IRC48M to be switched on by the USB module
    #define IRC48M_TIMEOUT 1000
    int iIRC48M_USB_control = 0;
#endif
#if !defined _COMPILE_IAR
    INIT_WATCHDOG_DISABLE();                                             // configure an input used to control watchdog operation
    if (WATCHDOG_DISABLE() == 0) {                                       // if the watchdog disable input is not active
        ACTIVATE_WATCHDOG();                                             // allow user configuration of internal watchdog timer
    }
    else {                                                               // disable the watchdog
    #if defined KINETIS_KL && !defined KINETIS_KL82                      // {67}
        SIM_COPC = SIM_COPC_COPT_DISABLED;                               // disable the COP
    #else
        UNLOCK_WDOG();                                                   // open a window to write to watchdog
        #if defined KINETIS_KE
        WDOG_CS2 = 0;                                                    // this is required to disable the watchdog
        WDOG_TOVAL = 0xffff;
        WDOG_WIN = 0;
        WDOG_CS1 = WDOG_CS1_UPDATE;                                      // disable watchdog but allow updates
        #else
        WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_CLKSRC); // disable watchdog
        #endif
    #endif
    }
    #if defined SUPPORT_LOW_POWER && (defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000))
    PMC_REGSC = PMC_REGSC_ACKISO;                                        // acknowledge the isolation mode to set certain peripherals and I/O pads back to normal run state
    #endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
#endif
#if defined KINETIS_KW2X
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(B, RST_B, 0, (PORT_SRE_FAST | PORT_DSE_LOW)); // drive the modem reset input low
#endif
#if defined USER_STARTUP_CODE                                            // {1} allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
#endif
#if !defined ERRATA_E2644_SOLVED && !defined KINETIS_FLEX
    FMC_PFB0CR &= ~(BANK_DPE | BANKIPE);                                 // disable speculation logic
    FMC_PFB1CR &= ~(BANK_DPE | BANKIPE);
#endif
#if !defined ERRATE_E2647_SOLVED && !defined KINETIS_FLEX && (SIZE_OF_FLASH > (256 * 1024))
    FMC_PFB0CR &= ~(BANKDCE | BANKICE | BANKSEBE);                       // disable cache
    FMC_PFB1CR &= ~(BANKDCE | BANKICE | BANKSEBE);
#endif
    // Configure clock generator
    // - initially the processor is in FEI (FLL Engaged Internal) - Kinetis K presently running from 20..25MHz internal clock (32.768kHz IRC x 640 FLL factor; 20.97MHz)
    //
#if defined KINETIS_KE                                                   // KE and KEA presently running at default IRC x FLL frequency
    SIM_SOPT0 = SIM_SOPT_KE_DEFAULT;                                     // set required default - some fields are "write-only" and so can only be set once
    #if !defined RUN_FROM_DEFAULT_CLOCK
    OSC0_CR = (OSC_CR_OSCEN | OSC_CR_OSCSTEN | OSC_CR_OSCOS_SOURCE | _OSC_RANGE); // low gain mode, select crystal range and enable oscillator
    while ((OSC0_CR & OSC_CR_OSCINIT) == 0) {                            // wait until the oscillator has been initialised
        #if defined _WINDOWS
        OSC0_CR |= OSC_CR_OSCINIT;
        #endif
    }
        #if defined RUN_FROM_EXTERNAL_CLOCK
    ICS_C1 = (ICS_C1_CLKS_EXTERNAL_REF | _FLL_VALUE);                    // divide value to obtain 31.25kHz..39.06525kHz range from input frequency and select external clock as clock source
    while (ICS_S & ICS_S_IREFST) {                                       // wait for the clock source to become external clock
            #if defined _WINDOWS
        ICS_S &= ~(ICS_S_IREFST);
            #endif
    }
        #else
    ICS_C1 = (ICS_C1_CLKS_FLL | _FLL_VALUE);                             // divide value to obtain 31.25kHz..39.06525kHz range from input frequency and select FLL as clock source
    while (ICS_S & ICS_S_IREFST) {                                       // wait for the clock source to become external clock
            #if defined _WINDOWS
        ICS_S &= ~(ICS_S_IREFST);
            #endif
    }
    while ((ICS_S & ICS_S_LOCK) == 0) {                                  // wait for the FLL to lock
            #if defined _WINDOWS
        ICS_S |= ICS_S_LOCK;
            #endif
    }
        #endif
    #endif
    #if (BUS_CLOCK_DIVIDE == 2)
        #if defined SIM_CLKDIV
    SIM_CLKDIV = (SIM_CLKDIV_OUTDIV2_2);                                 // bus clock half the system clock (ICSOUTCLK)
        #else
    SIM_BUSDIV = SIM_BUSDIVBUSDIV;                                       // bus clock half the system clock (ICSOUTCLK)
        #endif
    #else
        #if defined KINETIS_KE04 || defined KINETIS_KE06
    SIM_CLKDIV = 0;                                                      // bus clock is equal to the system clock (ICSOUTCLK)
        #else
    SIM_BUSDIV = 0;                                                      // bus clock is equal to the system clock (ICSOUTCLK)
        #endif
    #endif
    ICS_C2 = _SYSCLK__DIV;                                               // set system clock frequency (ICSOUTCLK) once the bus/flash divider has been configured
    #if !defined _WINDOWS
    ICS_S |= ICS_S_LOLS;                                                 // clear loss of lock status
    #endif
#elif defined RUN_FROM_DEFAULT_CLOCK                                     // no configuration performed - remain in default clocked mode
    #if defined KINETIS_KL
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare system and flash clock divides (valid also as bus clock divider)
        #if defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #endif
    #elif defined KINETIS_KV
        #if defined ADC_CLOCK_ENABLED 
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)  | ((ADC_CLOCK_DIVIDE - 1) << 12) | SIM_OUTDIV5EN); // prepare bus clock divides
        #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)  | SIM_CLKDIV5_ADC_8); // prepare bus clock divides
        #endif
        #if defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #endif
    #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
        #if defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #endif
    #endif
#elif defined MCG_WITHOUT_PLL && defined RUN_FROM_LIRC
    MCG_SC = MCG_SC_FCRDIV_1;                                            // no divide after fast clock (4MHz)
    MCG_C2 |= MCG_C2_IRCS;                                               // select fast internal reference clock (rather than slow one) for MCGIRCLK
    MCG_C1 = (MCG_C1_IREFSTEN | MCG_C1_IRCLKEN | MCG_C1_CLKS_INTERN_CLK);// enable and select 4MHz IRC clock source and allow it to continue operating in STOP mode
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_INTERN_CLK) {       // wait until the 4MHz IRC source is selected
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_INTERN_CLK;
    #endif
    }
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // set system and bus clock dividers
#elif defined RUN_FROM_HIRC || defined RUN_FROM_HIRC_FLL || defined RUN_FROM_HIRC_PLL // 48MHz
    #if !defined KINETIS_K64 && defined SUPPORT_RTC && !defined RTC_USES_RTC_CLKIN && !defined RTC_USES_LPO_1kHz
    MCG_C2 = MCG_C2_EREFS;                                               // request oscillator
    OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                       // enable the external reference clock and keep it enabled in stop mode
    #endif
  //MCG_MC = MCG_MC_HIRCEN;                                              // this is optional and would allow the HIRC to run even when the processor is not working in HIRC mode
    #if defined MCG_C1_CLKS_HIRC
    MCG_C1 = MCG_C1_CLKS_HIRC;                                           // select HIRC clock source
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_HICR) {             // wait until the source is selected
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_HICR;
        #endif
    }
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    #else
    MCG_C7 = MCG_C7_OSCSEL_IRC48MCLK;                                    // route the IRC48M clock to the external reference clock input (this enables IRC48M)
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    MCG_C1 = (MCG_C1_IREFS | MCG_C1_CLKS_EXTERN_CLK);                    // switch IRC48M reference to MCGOUTCLK
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // wait until the new source is valid (move to FBI using IRC48M external source is complete)
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
        #endif
        #if defined KINETIS_K64                                          // older K64 devices require the IRC48M to be switched on by the USB module
        if (++iIRC48M_USB_control >= IRC48M_TIMEOUT) {                   // if the switch-over is taking too long it means that the clock needs to be enabled in the USB controller
            POWER_UP(4, SIM_SCGC4_USBOTG);                               // power up the USB controller module
            USB_CLK_RECOVER_IRC_EN = (USB_CLK_RECOVER_IRC_EN_REG_EN | USB_CLK_RECOVER_IRC_EN_IRC_EN); // the IRC48M is only useable when enabled via the USB module
        }
        #endif
    }
        #if defined RUN_FROM_HIRC_FLL
    MCG_C2 = (MCG_C2_IRCS | MCG_C2_RANGE_8M_32M);                        // select a high frquency range values so that the FLL input divide range is increased
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1280);                  // switch FLL input to the external clock source with correct divide value, and select the FLL output for MCGOUTCLK
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_FLL) {              // wait for the output to be set
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_FLL;
        #endif
    }
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #elif defined RUN_FROM_HIRC_PLL                                  // we are presently running directly from the IRC48MCLK and have also determined whether a K64 is an older or newer device (with IRC48M independent from the USB module)
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1280);               // switch the external clock source also to the FLL to satisfy the PBE state requirement
    MCG_C5 = ((CLOCK_DIV - 1) | MCG_C5_PLLSTEN0);                        // PLL remains enabled in normal stop modes
    MCG_C6 = ((CLOCK_MUL - MCG_C6_VDIV0_LOWEST) | MCG_C6_PLLS);          // complete PLL configuration and move to PBE
    while ((MCG_S & MCG_S_PLLST) == 0) {                                 // loop until the PLLS clock source becomes valid
            #if defined _WINDOWS
        MCG_S |= MCG_S_PLLST;
            #endif
    }
    while ((MCG_S & MCG_S_LOCK) == 0) {                                  // loop until PLL locks
            #if defined _WINDOWS
        MCG_S |= MCG_S_LOCK;
            #endif
    }
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);                  // finally move from PBE to PEE mode - switch to PLL clock
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {              // loop until the PLL clock is selected
            #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_PLL;
            #endif
    }
        #else
    MCG_C2 |= MCG_C2_LP;                                                 // set bypass to disable FLL and complete move to BLPE (in which PLL is also always disabled)
        #endif
    #endif
#elif defined KINETIS_WITH_MCG_LITE && defined RUN_FROM_LIRC             // 8MHz default
    // The IRC8 defaults to 8MHz with no FCRDIV divide
    //
    #if defined RUN_FROM_LIRC_2M                                         // select 2MHz ICR rather than 8MHz
    MCG_C2 = 0;
    #endif
    #if defined SLOW_CLOCK_DIVIDE                                        // if a slow clock output divider is specified
    MCG_SC = SLOW_CLOCK_DIVIDE_VALUE;                                    // select the output divider ratio
    #endif
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // set bus clock divides
#else
    #if defined KINETIS_WITH_MCG_LITE                                    // EXT mode (run directly from the oscillator input)
    MCG_C2 = (MCG_C2_EREFS | MCG_C2_IRCS);                               // select oscillator as external clock source
    OSC0_CR = (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                        // enable the oscillator and allow it to continue oscillating in stop mode
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    while ((MCG_S & MCG_S_OSCINIT0) == 0) {                              // wait for the oscillator to start
        #if defined _WINDOWS
        MCG_S |= MCG_S_OSCINIT0;
        #endif
    }
    MCG_C1 = MCG_C1_CLKS_EXTERN_CLK;                                     // select external clock source
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXT) {              // wait until the source is selected
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXT;
    #endif
    }
    #elif defined EXTERNAL_CLOCK                                         // first move from state FEI to state FBE
        #if defined RUN_FROM_HIRC_PLL
    MCG_C7 = MCG_C7_OSCSEL_IRC48MCLK;                                    // route the IRC48M clock to the external reference clock input (this enables IRC48M)
        #endif
        #if defined RUN_FROM_RTC_FLL
    POWER_UP(6, SIM_SCGC6_RTC);                                          // enable access to the RTC
    MCG_C7 = MCG_C7_OSCSEL_32K;                                          // select the RTC clock as external clock input to the FLL
    RTC_CR = (RTC_CR_OSCE);                                              // enable RTC oscillator and output the 32.768kHz output clock so that it can be used by the MCG (the first time that it starts it can have a startup/stabilisation time but this is not critical for the FLL usage)
    MCG_C1 = ((MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_RANGE0_1) & ~MCG_C1_IREFS); // switch the FLL input to the undivided external clock source (RTC)
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    while ((MCG_S & MCG_S_IREFST) != 0) {                                // wait until the switch to the external clock source has completed
            #if defined _WINDOWS
        MCG_S &= ~(MCG_S_IREFST);
            #endif
    }
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #elif defined RUN_FROM_RTC_FLL
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #else                                                            // external oscillator
            #if defined KINETIS_KW2X && defined RUN_FROM_MODEM_CLK_OUT
    // Single crystal with CLK_OUT used by MCU
    //
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, GPIO5, 0, (PORT_SRE_FAST | PORT_DSE_LOW)); // set the output to select 4MHz CLK_OUT frequency
    _SETBITS(B, RST_B);                                                  // release the modem reset
    _CONFIG_PORT_INPUT_FAST_LOW(B, IRQ_B, (PORT_PS_UP_ENABLE));          // enable input to monitor the modem's interrupt line
                #if !defined _WINDOWS
    while (_READ_PORT_MASK(B, IRQ_B) != 0) {}                            // wait for modem start-up interrupt request (approx. 25ms)
                #endif
            #endif
            #if EXTERNAL_CLOCK >= 8000000
    MCG_C2 = (MCG_C2_RANGE_8M_32M | MCG_C2_LOCRE0);                      // select external clock source (with reset on clock loss)
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1024);               // switch to external input clock (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary since the FLL will not be used)
            #else
    MCG_C2 = (MCG_C2_RANGE_1M_8M | MCG_C2_LOCRE0);                       // select external clock source (with reset on clock loss)
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_128);                // switch to external input clock (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary since the FLL will not be used)
            #endif
        #endif
    #else                                                                // crystal clock
        #if defined OSC_LOW_GAIN_MODE                                    // {66} if using low frequency low power mode no external resistor or load capacitors are used
            #define MCG_C2_GAIN_MODE 0                                   // don't select high gain mode since the oscillator will not start
        #else
            #define MCG_C2_GAIN_MODE MCG_C2_HGO                          // select high gain mode
        #endif
        #if defined FLL_FACTOR || defined RUN_FROM_EXTERNAL_CLOCK
    OSC0_CR = (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                        // enable the oscillator and allow it to continue oscillating in stop mode
        #endif
        #if CRYSTAL_FREQUENCY > 8000000
    MCG_C2 = (MCG_C2_RANGE_8M_32M | MCG_C2_GAIN_MODE | MCG_C2_EREFS | MCG_C2_LOCRE0); // select crystal oscillator and select a suitable range
        #elif CRYSTAL_FREQUENCY >= 1000000
    MCG_C2 = (MCG_C2_RANGE_1M_8M | MCG_C2_GAIN_MODE | MCG_C2_EREFS | MCG_C2_LOCRE0); // select crystal oscillator and select a suitable range
        #else                                                            // assumed to be 32kHz crystal
    MCG_C2 = (MCG_C2_RANGE_32K_40K | MCG_C2_GAIN_MODE | MCG_C2_EREFS | MCG_C2_LOCRE0); // select crystal oscillator and select a suitable range
        #endif
        #if CRYSTAL_FREQUENCY == 8000000
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_256);                // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
        #elif CRYSTAL_FREQUENCY == 16000000
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_512);                // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
        #elif CRYSTAL_FREQUENCY == 24000000
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1024);               // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
        #elif CRYSTAL_FREQUENCY >= 10000000 && CRYSTAL_FREQUENCY <= 12000000
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_256);                // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
        #elif CRYSTAL_FREQUENCY == 4000000
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_128);                // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
        #elif CRYSTAL_FREQUENCY == 32768
            #if !defined FLL_FACTOR
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_RANGE0_1);           // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
            #endif
        #else
            #error crystal speed support needs to be added!
        #endif
    while ((MCG_S & MCG_S_OSCINIT) == 0) {                               // loop until the crystal source has been selected
        #if defined _WINDOWS
        MCG_S |= MCG_S_OSCINIT;                                          // set the flag indicating that the ocsillator initialisation has completed
        #endif
    }
        #if defined FLL_FACTOR
            #if defined KINETIS_KL
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
            #elif defined KINETIS_KV
                #if defined ADC_CLOCK_ENABLED 
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)  | ((ADC_CLOCK_DIVIDE - 1) << 12) | SIM_OUTDIV5EN); // prepare bus clock divides
                #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)  | ((SIM_CLKDIV5_ADC_8 - 1) << 12)); // prepare bus clock divides
                #endif
            #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
            #endif
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
    MCG_C1 &= ~(MCG_C1_CLKS_INTERN_CLK | MCG_C1_CLKS_EXTERN_CLK | MCG_C1_IREFS); // move to FEE (MCGOUTCLK is derived from the FLL clock), selecting the external clock as source (rather than slow internal clock)
    while ((MCG_S & (MCG_S_CLKST_EXTERN_CLK | MCG_S_CLKST_INTERN_CLK)) != MCG_S_CLKST_FLL) { // loop until the FLL clock source becomes valid
            #if defined _WINDOWS
        MCG_S &= ~(MCG_S_CLKST_EXTERN_CLK | MCG_S_CLKST_INTERN_CLK);
            #endif
    }
        #endif
    #endif                                                               // endif not EXTERNAL_CLOCK
    #if !defined KINETIS_WITH_MCG_LITE && !defined RUN_FROM_RTC_FLL
    while ((MCG_S & MCG_S_IREFST) != 0) {                                // loop until the FLL source is no longer the internal reference clock
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_IREFST;
        #endif
    }
        #if !defined FLL_FACTOR
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // loop until the external reference clock source is valid
            #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
            #endif
    }
            #if defined RUN_FROM_EXTERNAL_CLOCK                          // {101}
                #if defined KINETIS_KL
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
                #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
                #endif
            #endif
        #endif
        #if defined CLOCK_DIV_1                                          // {90} PLL1 is to be configured and enabled 
    MCG_C11 = (MCG_C11_PLLREFSEL1_OSC0 | MCG_C11_PLLCLKEN1 | (CLOCK_DIV_1 - 1)); // use OSCO clock source for PLL1 with input divide set
    MCG_C12 = (CLOCK_MUL_1 - MCG_C12_VDIV1_LOWEST);                      // set multiplier - we don't wait for PLL1 to lock until after PLL0 setup has completed (allow them to lock in parallel)
        #endif
        #if !defined FLL_FACTOR && !defined RUN_FROM_EXTERNAL_CLOCK      // {95}
    MCG_C5 = ((CLOCK_DIV - 1) | MCG_C5_PLLSTEN0);                        // now move from state FEE to state PBE (or FBE) PLL remains enabled in normal stop modes
    MCG_C6 = ((CLOCK_MUL - MCG_C6_VDIV0_LOWEST) | MCG_C6_PLLS);
    while ((MCG_S & MCG_S_PLLST) == 0) {                                 // loop until the PLLS clock source becomes valid
            #if defined _WINDOWS
        MCG_S |= MCG_S_PLLST;
            #endif
    }
    while ((MCG_S & MCG_S_LOCK) == 0) {                                  // loop until PLL locks
            #if defined _WINDOWS
        MCG_S |= MCG_S_LOCK;
            #endif
    }
            #if defined KINETIS_KL
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
            #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
            #endif
            #if defined HIGH_SPEED_RUN_MODE_AVAILABLE && defined HIGH_SPEED_RUN_MODE_REQUIRED
    SMC_PMCTRL = SMC_PMCTRL_RUNM_HSRUN;                                  // {118} set high speed run mode (restrictions apply) so that the clock speeds can be obtained  
            #endif
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);                  // finally move from PBE to PEE mode - switch to PLL clock
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {              // loop until the PLL clock is selected
            #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_PLL;
            #endif
    }
        #endif
        #if defined CLOCK_DIV_1                                          // {90} PLL1 used by FS USB or SDRAM
    while ((MCG_S2 & MCG_S2_LOCK1) == 0) {                               // loop until PLL1 locks
            #if defined _WINDOWS
        MCG_S2 |= MCG_S2_LOCK1;
            #endif
    }
        #endif
    #endif
#endif
#if defined CLKOUT_AVAILABLE && !defined KINETIS_WITH_PCC                // select the clock signal to be driven on CLKOUT pin
    #if defined KINETIS_K64
      //fnClkout(FLEXBUS_CLOCK_OUT);                                     // select the clock to monitor on CLKOUT
    #endif
    #if defined LOW_POWER_OSCILLATOR_CLOCK_OUT
      //fnClkout(LOW_POWER_OSCILLATOR_CLOCK_OUT);
    #endif
  //fnClkout(INTERNAL_IRC48M_CLOCK_OUT);
  //fnClkout(INTERNAL_LIRC_CLOCK_OUT);                                   // equivalent to INTERNAL_MCGIRCLK_CLOCK_OUT
  //fnClkout(EXTERNAL_OSCILLATOR_CLOCK_OUT);
    fnClkout(RTC_CLOCK_OUT);
#endif
#if defined INTERRUPT_VECTORS_IN_FLASH                                   // {111}
    VECTOR_TABLE_OFFSET_REG = ((unsigned long)&__vector_table);
#else
    #if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)&vector_ram;
    #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
    #endif
    VECTOR_TABLE_OFFSET_REG   = (unsigned long)ptrVect;                  // position the vector table at the bottom of RAM
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
    } while (1);
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
#if defined (_GNU) || defined _CODE_WARRIOR                              // {110}
    __init_gnu_data();                                                   // initialise variables
#endif
#if defined USE_SDRAM                                                    // {38}
    fnConfigureSDRAM();                                                  // configure SDRAM
#endif
#if defined _WINDOWS && !defined INTERRUPT_VECTORS_IN_FLASH              // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Check the vector table size setting!!");
    }
    #if defined USE_SECTION_PROGRAMMING                                  // {105}
    memset(ucFlexRam, 0xff, sizeof(ucFlexRam));                          // when used as data flash the flex ram is initialised to all 0xff
    #endif
#endif
#if defined KINETIS_K_FPU                                                // {17}
    CPACR |= (0xf << 20);                                                // enable access to FPU
#endif
#if defined SUPPORT_LOW_POWER
    #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
        #if !defined SMC_PMPROT_LOW_POWER_LEVEL
            #if defined HIGH_SPEED_RUN_MODE_AVAILABLE
                #define SMC_PMPROT_LOW_POWER_LEVEL (SMC_PMPROT_AVLLS | SMC_PMPROT_ALLS | SMC_PMPROT_AVLP | SMC_PMPROT_AHSRUN) // allow all low power modes if nothing else defined
            #else
                #define SMC_PMPROT_LOW_POWER_LEVEL (SMC_PMPROT_AVLLS | SMC_PMPROT_ALLS | SMC_PMPROT_AVLP) // allow all low power modes if nothing else defined
            #endif
        #endif
    SMC_PMPROT = SMC_PMPROT_LOW_POWER_LEVEL;                             // {117}
    #elif !defined KINETIS_KE && !defined KINETIS_KEA
        #if !defined MC_PMPROT_LOW_POWER_LEVEL
            #define MC_PMPROT_LOW_POWER_LEVEL (MC_PMPROT_AVLP | MC_PMPROT_ALLS | MC_PMPROT_AVLLS1 | MC_PMPROT_AVLLS2 | MC_PMPROT_AVLLS3) // allow all low power modes if nothing else defined
        #endif
    MC_PMPROT = MC_PMPROT_LOW_POWER_LEVEL;                               // {117}
    #endif
    #if defined ERRATA_ID_8068 && !defined SUPPORT_RTC                   // if low power mode is to be used but the RTC will not be initialsed clear the RTC invalid flag to avoid the low power mode being blocked when e8068 is present
    POWER_UP(6, SIM_SCGC6_RTC);                                          // temporarily enable the RTC module
    RTC_TSR = 0;                                                         // clear the RTC invalid flag with a write of any value to RTC_TSR
    POWER_DOWN(6, SIM_SCGC6_RTC);                                        // power down the RTC again
    #endif
#endif
#if defined _WINDOWS
    fnUpdateOperatingDetails();                                          // {91} update operating details to be displayed in the simulator
#endif
#if defined SET_POWER_MODE
    SET_POWER_MODE();                                                    // {93}
#endif
}


#if !defined _COMPILE_KEIL                                               // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #if defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV   // {67} cortex-M0+ assembler code
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


// The initial stack pointer and PC value - this is usually linked at address 0x00000000 (or to application start location when working with a boot loader)
//
#if defined _COMPILE_IAR
__root const _RESET_VECTOR __vector_table @ ".intvec"                     // __root forces the function to be linked in IAR project
#elif defined _GNU
const _RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _CODE_WARRIOR
#pragma define_section vectortable ".RESET" ".RESET" ".RESET" far_abs R
static __declspec(vectortable) _RESET_VECTOR __vect_table
#elif defined _COMPILE_KEIL
__attribute__((section("VECT"))) const _RESET_VECTOR reset_vect
#else
    #if defined _COMPILE_GHS                                             // {110}
        #pragma ghs section rodata=".vectors"
    #elif defined _COMPILE_COSMIC                                        // {113}
        #pragma section const {vector}
    #endif
const _RESET_VECTOR __vector_table
#endif
= {
#if defined INTERRUPT_VECTORS_IN_FLASH
    {
#endif
    (void *)(RAM_START_ADDRESS + (SIZE_OF_RAM - NON_INITIALISED_RAM_SIZE)), // stack pointer to top of RAM
    (void (*)(void))START_CODE,                                          // start address
#if defined INTERRUPT_VECTORS_IN_FLASH
    },
#endif
#if defined _APPLICATION_VALIDATION                                      // {32}
    {0x87654321, 0xffffffff},                                            // signal that this application supports validation
    {0xffffffff, 0xffffffff},                                            // overwrite first location with 0x55aa33cc to validate the application
    #if defined _GNU && !defined _BM_BUILD
   {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff},
    {
        KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY,
        KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION,
        KINETIS_FLASH_CONFIGURATION_SECURITY,
        KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION,
        KINETIS_FLASH_CONFIGURATION_EEPROM_PROT,
        KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT
    }
    #endif
#elif defined INTERRUPT_VECTORS_IN_FLASH                                 // {111} presently used only by the KE04, KEA8 and KL03 (vectors in flash to save space when little flash resources available)
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
    #if defined SUPPORT_LOW_POWER && defined LLWU_AVAILABLE && defined SUPPORT_LLWU
    _wakeup_isr,                                                         // 7
    #else
    irq_default,                                                         // 7
    #endif
    #if defined I2C_INTERFACE
    _I2C_Interrupt_0,                                                    // 8
    #else
    irq_default,                                                         // 8
    #endif
    irq_default,                                                         // 9
    irq_default,                                                         // 10
    irq_default,                                                         // 11
    #if defined SERIAL_INTERFACE
        #if defined KINETIS_KL03
    _LPSCI0_Interrupt,                                                   // 12 LPUART 0
        #else
    _SCI0_Interrupt,                                                     // 12 UART 0
        #endif
    #else
        irq_default,                                                     // 12
    #endif
    irq_default,                                                         // 13
    irq_default,                                                         // 14
    #if defined SUPPORT_ADC
        _ADC_Interrupt_0,                                                // 15 ADC0
    #else
        irq_default,                                                     // 15
    #endif
    irq_default,                                                         // 16
    #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
	_flexTimerInterrupt_0,                                               // 17
    #else
    irq_default,                                                         // 17
    #endif
    #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 1 && !defined NO_FLEX_TIMER_2)
	_flexTimerInterrupt_1,                                               // 18
    #else
    irq_default,                                                         // 18
    #endif
	#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 2)
	_flexTimerInterrupt_2,                                               // 19
    #else
    irq_default,                                                         // 19
    #endif
    #if defined SUPPORT_RTC && defined KINETIS_KE
    _rtc_handler,                                                        // 20
    irq_default,                                                         // 21
    #elif defined SUPPORT_RTC && !defined KINETIS_KL02
    _rtc_alarm_handler,                                                  // 20
    _rtc_handler,                                                        // 21
    #else
    irq_default,                                                         // 20
    irq_default,                                                         // 21
    #endif
    #if (defined SUPPORT_PITS || defined USB_HOST_SUPPORT) && !defined KINETIS_WITHOUT_PIT
        #if defined KINETIS_KL
	_PIT_Interrupt,                                                      // 22
	irq_default,                                                         // 23
        #else
	_PIT0_Interrupt,                                                     // 22
	_PIT1_Interrupt,                                                     // 23
        #endif
    #else
    irq_default,                                                         // 22
	irq_default,                                                         // 23
    #endif
    #if defined USB_INTERFACE
    _usb_otg_isr,                                                        // 24
    #else
        #if defined KINETIS_KE && defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 0)
    _KBI0_isr,                                                           // 24
        #else
    irq_default,                                                         // 24
        #endif
    #endif
        #if defined KINETIS_KE && defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 1)
    _KBI1_isr,                                                           // 25
        #else
    irq_default,                                                         // 25
        #endif
    irq_default,                                                         // 26
    irq_default,                                                         // 27
    #if defined TICK_USES_LPTMR
    _RealTimeInterrupt,                                                  // 28
    #elif defined SUPPORT_LPTMR && !defined TICK_USES_LPTMR && !defined KINETIS_KE
    _LPTMR_periodic,                                                     // 28 warning that only periodic interrupt is supported (not single-shot)
    #else
    irq_default,                                                         // 28
    #endif
    #if defined KINETIS_KL03
    irq_default,                                                         // 29
        #if defined SUPPORT_PORT_INTERRUPTS && !defined NO_PORT_INTERRUPTS_PORTA
    _port_A_isr,                                                         // 30
        #else
    irq_default,                                                         // 30
        #endif
        #if defined SUPPORT_PORT_INTERRUPTS && !defined NO_PORT_INTERRUPTS_PORTB
    _port_B_isr,                                                         // 30
        #else
    irq_default,                                                         // 30
        #endif
    #endif
    }

#endif
};

#if !defined _BM_BUILD && !(defined _APPLICATION_VALIDATION && defined _GNU)
// Flash configuration - this is linked at address 0x00000400 (when working with standalone-build)
//
#if defined _COMPILE_IAR
__root const KINETIS_FLASH_CONFIGURATION __flash_config @ ".f_config"    // __root forces the function to be linked in IAR project
#elif defined _GNU
const KINETIS_FLASH_CONFIGURATION __attribute__((section(".f_config"))) __flash_config
#elif defined _CODE_WARRIOR
#pragma define_section flash_cfg ".FCONFIG" ".FCONFIG" ".FCONFIG" far_abs R
static __declspec(flash_cfg) KINETIS_FLASH_CONFIGURATION flash_config
#elif defined _COMPILE_KEIL
__attribute__((section("F_INIT"))) const KINETIS_FLASH_CONFIGURATION __flash_config
#else
    #if defined _COMPILE_GHS                                             // {110}
        #pragma ghs section rodata=".f_config"
    #elif defined _COMPILE_COSMIC                                        // {113}
        #pragma section {}
        #pragma section const {config}
    #endif
const KINETIS_FLASH_CONFIGURATION __flash_config
#endif
= {
    KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY,
    KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION,
    KINETIS_FLASH_CONFIGURATION_SECURITY,
    KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION,
    KINETIS_FLASH_CONFIGURATION_EEPROM_PROT,
    KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT
};
#if defined _COMPILE_GHS                                                 // {110}
    #pragma ghs section rodata=default
#elif defined _COMPILE_COSMIC                                            // {113}
    #pragma section {}
#endif
#endif
#endif
