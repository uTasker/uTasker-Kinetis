/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32Sim.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    08.09.2012 Adapt RTC for F2/F4 devices                               {1}
    09.09.2012 Handle additional functions                               {2}
    12.09.2012 Add I2C 2 and 3 interrupts                                {3}
    11.06.2013 Add STM32F1xx USB ports                                   {4}
    26.09.2013 Add STM32F1xx RMII mode                                   {5}
    02.03.2014 Add CAN interface simulation                              {6}
    15.06.2014 Add F2/F4 memory to memory DMA                            {7}
    26.05.2015 Add hardware operating details to simulator status bar    {8}
    28.02.2017 Add UARTs 7 and 8                                         {9}
    02.02.2017 Adapt for us tick resolution
    06.09.2017 Add ADC simulation                                        {10}
    26.09.2018 Correct fnMapPortBit()                                    {11}

*/  


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */
#if defined _STM32

#include "config.h"

#if defined CAN_INTERFACE && defined SIM_KOMODO
    #include "..\..\WinSim\Komodo\komodo.h" 
#endif


/**************************************************************************************************/
/*********************************  ST STM32  *****************************************************/
/**************************************************************************************************/


#define GET_PERIPHERALS 0
#define GET_OUTPUTS     1
#define GET_PER_TYPE    2
#define GET_OUTPUT_MASK 3
static unsigned short fnGetPortType(int portNr, int iRequest, int iStart);

static int iFlagRefresh = 0;

static unsigned long ulGPIOIN[11] = {0};                                 // the size is forced to largest port range
static unsigned long ulGPIOPER[PORTS_AVAILABLE] = {0};
static unsigned long ulGPIODDR[PORTS_AVAILABLE] = {0};
static unsigned long ulGPIODRIVE_VALUE[PORTS_AVAILABLE] = {0};

#if defined SUPPORT_ADC                                                  // {10}
    static unsigned short usADC_values[ADC_CHANNELS];
#endif

#if defined USB_INTERFACE
    static unsigned long ulEndpointInt = 0;
#endif
#if defined CAN_INTERFACE
    static unsigned short usCAN_time = 0;                                // CAN timestamp
#endif

unsigned long vector_ram[(sizeof(VECTOR_TABLE))/sizeof(unsigned long)];  // long word aligned

static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = {{0}};

// Initialisation for all non-zero registers in the device
//
static void fnSetDevice(unsigned short *port_inits)
{
    extern void fnEnterHW_table(void *hw_table);
#if defined SUPPORT_ADC
    int i;
#endif
    CPUID_BASE_REGISTER = 0x411fc231;
#if defined _STM32L0x1
    RCC_CR = (RCC_CR_MSIRDY | RCC_CR_MSION);                             // reset and clock control
    RCC_ICSCR = (RCC_ICSCR_MSIRANGE_2_097M);
#elif defined _STM32L432
    RCC_CR = (RCC_CR_MSIRANGE_4M | RCC_CR_MSIRDY | RCC_CR_MSION);        // reset and clock control
    RCC_CSR = (RCC_CSR_PINRSTF | RCC_CSR_PORRSTF | RCC_CSR_MSIRANGE_4M);
    RCC_APB1ENR1 = RCC_APB1ENR1_RTCAPBEN;
    RCC_APB1ENR1 = 0x00000400;
    RCC_AHB1SMENR = 0x00011303;
    RCC_AHB2SMENR = 0x0005229f;
    RCC_AHB3SMENR = 0x00000100;
    RCC_APB1SMENR1 = 0xf7e6ce31;
    RCC_APB1SMENR2 = 0x00000025;
    RCC_APB2SMENR = 0x00235c01;
    RCC_ICSCR = 0x106f0089;
    RCC_PLLCFGR = 0x00001000;
    RCC_PLLSAI1CFGR = 0x00001000;
#else
    RCC_CR = (0x00000080 | RCC_CR_HSIRDY | RCC_CR_HSION);                // reset and clock control
    RCC_CSR = (RCC_CSR_PINRSTF | RCC_CSR_PORRSTF);
#endif
    GPIOA_IDR = ulGPIOIN[0] = *port_inits++;                             // set port inputs to default states
    GPIOB_IDR = ulGPIOIN[1] = *port_inits++;
    GPIOC_IDR = ulGPIOIN[2] = *port_inits++;
    GPIOD_IDR = ulGPIOIN[3] = *port_inits++;
    GPIOE_IDR = ulGPIOIN[4] = *port_inits++;
    GPIOF_IDR = ulGPIOIN[5] = *port_inits++;
    GPIOG_IDR = ulGPIOIN[6] = *port_inits++;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
    GPIOH_IDR = ulGPIOIN[7] = *port_inits++;
    GPIOI_IDR = ulGPIOIN[8] = *port_inits++;
    #if defined _STM32F7XX
    GPIOJ_IDR = ulGPIOIN[9]  = *port_inits++;
    GPIOK_IDR = ulGPIOIN[10] = *port_inits++;
    #endif

    #if defined _STM32F7XX
    PWR_CR = PWR_CR_VOS_1;
    #else
    PWR_CR = PWR_CR_VOS;
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
    RCC_AHB1ENR = RCC_AHB1ENR_FLASHEN;
    RCC_PLLCFGR = 0x00001000;
    #elif !defined _STM32L432
    RCC_AHB1ENR = RCC_AHB1ENR_CCMDATARAMEN;
    RCC_PLLCFGR = 0x24003010;
    #endif
#elif defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
    RCC_AHB1ENR = (RCC_AHB1ENR_FLASHEN);
#elif defined _STM32L0x1
    RCC_AHBENR = RCC_AHBENR_MIFEN;
    RCC_IOPSMEN = 0x0000009f;
    RCC_AHBSMENR = 0x01001301;
    RCC_APB2SMENR = 0x00405225;
    RCC_APB1SMENR = 0xd07e4833;
    RCC_CSR = 0x0c000000;
#else
    RCC_AHB1ENR = (RCC_AHB1ENR_FLITFEN | RCC_AHB1ENR_SRAMEN);
#endif
#if defined _STM32L0x1
    FLASH_PECR = 0x0000007;
    FLASH_SR = 0x0000000c;
    FLASH_OPTR = 0x807000aa;
#else
    FLASH_CR = FLASH_CR_LOCK;                                            // FLASH controller
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    FLASH_OPTCR = DEFAULT_FLASH_OPTION_SETTING;
        #if defined FLASH_OPTCR1
    FLASH_OPTCR1 = DEFAULT_FLASH_OPTION_SETTING_1;
        #endif
    #else
    FLASH_WRPR = 0xffffffff;
    FLASH_OBR = 0x03fffffc;
    #endif
#endif
#if defined _STM32L0x1
    GPIOA_MODER = 0xebfffcff;
    GPIOA_PUPDR = 0x24000000;
    GPIOB_MODER = 0xffffffff;
    GPIOC_MODER = 0xffffffff;
    GPIOD_MODER = 0xffffffff;
    GPIOE_MODER = 0xffffffff;
    GPIOH_MODER = 0xffffffff;
#elif defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    GPIOA_MODER = 0xa8000000;
    GPIOB_MODER = 0x00000280;
    GPIOB_OSPEEDR = 0x000000c0;
    GPIOA_PUPDR = 0x64000000;
    GPIOB_PUPDR = 0x00000100;
#else
    GPIOA_CRL = 0x44444444;                                              // port input states
    GPIOA_CRH = 0x44444444;
    GPIOB_CRL = 0x44444444;
    GPIOB_CRH = 0x44444444;
    GPIOC_CRL = 0x44444444;
    GPIOC_CRH = 0x44444444;
    GPIOD_CRL = 0x44444444;
    GPIOD_CRH = 0x44444444;
    GPIOE_CRL = 0x44444444;
    GPIOE_CRH = 0x44444444;
    GPIOF_CRL = 0x44444444;
    GPIOF_CRH = 0x44444444;
    GPIOG_CRL = 0x44444444;
    GPIOG_CRH = 0x44444444;
#endif

#if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    USART1_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    USART2_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    #if USARTS_AVAILABLE > 2
    USART3_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    #endif
    #if UARTS_AVAILABLE > 0
    UART4_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    #endif
    #if UARTS_AVAILABLE > 1
    UART5_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    #endif
    #if USARTS_AVAILABLE > 3
    USART6_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    #endif
    #if UARTS_AVAILABLE > 2
    UART7_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    #endif
    #if UARTS_AVAILABLE > 3
    UART8_ISR = (0x02000000 | USART_ISR_TXE | USART_ISR_TC);
    #endif
    #if LPUARTS_AVAILABLE > 0
    LPUART1_ISR = (USART_ISR_TXE | USART_ISR_TC);
    #endif
#else
    USART1_SR = 0x00c0;                                                  // USARTs and UARTs
    USART2_SR = 0x00c0;
    #if USARTS_AVAILABLE > 2 && !defined USART3_NOT_PRESENT
    USART3_SR = 0x00c0;
    #endif
    #if UARTS_AVAILABLE > 0
    UART4_SR  = 0x00c0;
    #endif
    #if UARTS_AVAILABLE > 1
    UART5_SR  = 0x00c0;
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX
    USART6_SR = 0x00c0;
    #endif
    #if CHIP_HAS_UARTS > 6
    UART7_SR = (USART_SR_TC | USART_SR_TXE);
    #endif
    #if CHIP_HAS_UARTS > 7
    UART8_SR = (USART_SR_TC | USART_SR_TXE);
    #endif
#endif

    SPI1_CRCPR = 7;                                                      // SPI (I2S)
    SPI2_I2SPR = 0x00000002;
    SPI2_CRCPR = 7;
    SPI3_I2SPR = 0x00000002;
    SPI3_CRCPR = 7;

    ETH_MACA0HR = 0x8000ffff;                                            // Ethernet
    ETH_MACA0LR = 0xffffffff;
    ETH_MACA1HR = 0x0000ffff;
    ETH_MACA1LR = 0xffffffff;
    ETH_MACA2HR = 0x0000ffff;
    ETH_MACA2LR = 0xffffffff;
    ETH_MACA3HR = 0x0000ffff;
    ETH_MACA3LR = 0xffffffff;
    ETH_DMABMR  = 0x00002101;
    ETH_MACCR   = 0x00008000;

    IWDG_RLR = 0xfff;                                                    // independent watchdog
#if defined _STM32L432 || defined _STM32L0x1
    IWDG_WINR = 0xfff;
#endif
#if defined USB_DEVICE_AVAILABLE                                         // USB FS device
    USB_CNTR = (USB_CNTR_PDWN | USB_CNTR_FRES);
#endif
#if defined USB_OTG_AVAILABLE                                            // USB OTG FS
    OTG_FS_GOTGCTL  = OTG_FS_GOTGCTL_DHNPEN;
    OTG_FS_GUSBCFG  = 0x00000a00;
    OTG_FS_GRSTCTL  = 0x20000000;
    OTG_FS_DCFG     = 0x02200000;
    OTG_FS_GRXFSIZ  = 0x00000200;
    OTG_FS_DIEPTXF0 = 0x00000200;
    OTG_FS_DIEPTXF1 = OTG_FS_DIEPTXF2 = OTG_FS_DIEPTXF3 = 0x02000400;
    OTG_FS_DOEPINT0 = OTG_FS_DOEPINT1 = OTG_FS_DOEPINT2 = OTG_FS_DOEPINT3 = 0x00000080;
    OTG_FS_DIEPINT0 = OTG_FS_DIEPINT1 = OTG_FS_DIEPINT2 = OTG_FS_DIEPINT3 = OTG_FS_DIEPINT_TXFE;
    OTG_FS_DSTS     = 0x00000010;
    OTG_FS_DOEPCTL0 = OTG_FS_DOEPCTL_USBAEP;

    FSMC_BCR1  = (FSMC_BCR_WAITEN | FSMC_BCR_WREN | FSMC_BCR_FACCEN | FSMC_BCR_MWD_16 | FSMC_BCR_MTYP_NOR_FLASH); // FSMC
    FSMC_BCR2  = (FSMC_BCR_WAITEN | FSMC_BCR_WREN | FSMC_BCR_FACCEN | FSMC_BCR_MWD_16 | FSMC_BCR_MTYP_SRAM_ROM);
    FSMC_BCR3  = (FSMC_BCR_WAITEN | FSMC_BCR_WREN | FSMC_BCR_FACCEN | FSMC_BCR_MWD_16 | FSMC_BCR_MTYP_SRAM_ROM);
    FSMC_BCR4  = (FSMC_BCR_WAITEN | FSMC_BCR_WREN | FSMC_BCR_FACCEN | FSMC_BCR_MWD_16 | FSMC_BCR_MTYP_SRAM_ROM);

    FSMC_BTR1  = 0x0fffffff;
    FSMC_BTR2  = 0x0fffffff;
    FSMC_BTR3  = 0x0fffffff;
    FSMC_BTR4  = 0x0fffffff;
    FSMC_BWTR1 = 0x0fffffff;
    FSMC_BWTR2 = 0x0fffffff;
    FSMC_BWTR3 = 0x0fffffff;
    FSMC_BWTR4 = 0x0fffffff;
#endif


    ADC1_HTR = 0x00000fff;                                               // ADC
    ADC2_HTR = 0x00000fff;
    ADC3_HTR = 0x00000fff;

    CAN1_MCR   = (CAN_MCR_DBF | CAN_MCR_SLEEP);                          // bxCAN
    CAN1_MSR   = (CAN_MSR_RX | CAN_MSR_SAMP | CAN_MSR_SLAK);
    CAN1_TSR   = (0x1c000000);
    CAN1_BTR   = (0x00230000);
    CAN1_FMR   = (CAN_FMR_FINIT | 0x2a1ce00);

    CAN2_MCR   = (CAN_MCR_DBF | CAN_MCR_SLEEP);
    CAN2_MSR   = (0x00000c02);
    CAN2_TSR   = (0x1c000000);
    CAN2_BTR   = (0x00230000);
    CAN2_FMR   = (CAN_FMR_FINIT | 0x2a1ce00);

#if defined SUPPORT_ADC                                                  // {10}
    for (i = 0; i < (ADC_CHANNELS); i++) {
        usADC_values[i] = (unsigned short)*port_inits++;                 // {2} prime initial ADC values
    }
#endif
    fnEnterHW_table(ucPortFunctions);
}


static unsigned char ucFLASH[SIZE_OF_FLASH];                             // copy of FLASH memory


extern void fnInitialiseDevice(void *port_inits)
{
    uMemset(ucFLASH, 0xff, sizeof(ucFLASH));                             // we start with deleted FLASH memory contents
    fnPrimeFileSystem();                                                 // the project can then set parameters or files as required
    fnSetDevice((unsigned short *)port_inits);                           // set device registers to startup condition (if not zerod)
}

extern void fnDeleteFlashSector(unsigned char *ptrSector)
{
	int i = FLASH_GRANULARITY;

	while (i-- != 0) {
		*ptrSector++ = 0xff;
	}
}



// Transform physical access address to address in simulated FLASH
//
extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd)               
{
    unsigned char *ucSimulatedFlash = ucAdd + (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    if (ucSimulatedFlash >= &ucFLASH[SIZE_OF_FLASH]) {                   // check flash bounds
        _EXCEPTION("Attempted access outside of internal Flash bounds!!!");
    }
    return (ucSimulatedFlash);                                           // location in simulated internal FLASH memory
}

// Transform simulated address in simulated FLASH back to physical address in real FLASH
//
extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd) 
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    return (ucAdd - sim_add);
}



extern unsigned char *fnGetFileSystemStart(int iMemory)
{
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    return (fnGetEEPROM(uFILE_START));                                   // EEPROM
    #else
    return (ucFLASH);                                                    // internal FLASH
    #endif
}

extern unsigned long fnGetFlashSize(int iMemory)
{
    return SIZE_OF_FLASH;                                                // size fo internal flash
}


/******************************************************************************************************/



// A copy of the STM32 peripheral registers for simulation purposes
//
extern STM32M_PERIPH  STM32 = {0};


// Check whether a particular interrupt is enabled in the NVIC
//
static int fnGenInt(int iIrqID)
{
    if (iIrqID < 32) {
        IRQ0_31_SPR |= (1 << iIrqID);
        IRQ0_31_CPR = IRQ0_31_SPR;
        if ((IRQ0_31_SER & (1 << iIrqID)) != 0) {                        // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 64) {
        IRQ32_63_SPR |= (1 << (iIrqID - 32));
        IRQ32_63_CPR = IRQ32_63_SPR;
        if ((IRQ32_63_SER & (1 << (iIrqID - 32))) != 0) {                // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 96) {
        IRQ64_95_SPR |= (1 << (iIrqID - 64));
        IRQ64_95_CPR = IRQ64_95_SPR;
        if ((IRQ64_95_SER & (1 << (iIrqID - 64))) != 0) {                // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 128) {
        IRQ96_127_SPR |= (1 << (iIrqID - 96));
        IRQ96_127_CPR = IRQ96_127_SPR;
        if ((IRQ96_127_SER & (1 << (iIrqID - 96))) != 0) {               // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 160) {
        IRQ128_159_SPR |= (1 << (iIrqID - 128));
        IRQ128_159_CPR = IRQ128_159_SPR;
        if ((IRQ128_159_SER & (1 << (iIrqID - 128))) != 0) {             // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 192) {
        IRQ160_191_SPR |= (1 << (iIrqID - 164));
        IRQ160_191_CPR = IRQ160_191_SPR;
        if ((IRQ160_191_SER & (1 << (iIrqID - 164))) != 0) {             // if interrupt is not disabled
            return 1;
        }
    }
    return 0;
}


// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{
    unsigned long ulNewActions = 0;
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    int *ptrCnt;

	if (((iInts & CHANNEL_0_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART0];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
#if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_0_SERIAL_INT;                          // interrupt has been handled
                fnLogTx0((unsigned char)USART1_TDR);
                ulNewActions |= SEND_COM_0;
    #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
                USART1_ISR |= USART_ISR_TXE;
    #else
                USART1_SR |= USART_SR_TXE;
    #endif
                if ((USART1_CR1 & USART_CR1_TXEIE) != 0) {               // if tx interrupts enabled
                    if (fnGenInt(irq_USART1_ID) != 0) {                  // if USART 1 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_USART1();      // call the interrupt handler
                    }
                }
#endif
            }
        }
	}

	if (((iInts & CHANNEL_1_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART1];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_1_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
#if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_1_SERIAL_INT;                          // interrupt has been handled
                fnLogTx1((unsigned char)USART2_TDR);
                ulNewActions |= SEND_COM_1;
    #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
                USART2_ISR |= USART_ISR_TXE;
    #else
                USART2_SR |= USART_SR_TXE;
    #endif
                if ((USART2_CR1 & USART_CR1_TXEIE) != 0) {               // if tx interrupts enabled
                    if (fnGenInt(irq_USART2_ID) != 0) {                  // if USART 2 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_USART2();      // call the interrupt handler
                    }
                }
#endif
            }
        }
	}
#if USARTS_AVAILABLE > 2 && !defined USART3_NOT_PRESENT
	if (((iInts & CHANNEL_2_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART2];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
    #if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_2_SERIAL_INT;                          // interrupt has been handled
                fnLogTx2((unsigned char)USART3_TDR);
                ulNewActions |= SEND_COM_2;
        #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32L4X5 || defined _STM32L4X6
                USART3_ISR |= USART_ISR_TXE;
        #else
                USART3_SR |= USART_SR_TXE;
        #endif
                if ((USART3_CR1 & USART_CR1_TXEIE) != 0) {               // if tx interrupts enabled
                    if ((IRQ32_63_SER & (1 << (irq_USART3_ID - 32))) != 0) { // if USART 3 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_USART3();      // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
#endif
#if UARTS_AVAILABLE > 0
	if (((iInts & CHANNEL_3_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART3];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_3_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
    #if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_3_SERIAL_INT;                          // interrupt has been handled
                fnLogTx3((unsigned char)UART4_TDR);
                ulNewActions |= SEND_COM_3;
        #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32L4X5 || defined _STM32L4X6
                UART4_ISR |= USART_ISR_TXE;
        #else
                UART4_SR |= USART_SR_TXE;
        #endif
                if ((UART4_CR1 & USART_CR1_TXEIE) != 0) {                // if tx interrupts enabled
                    if ((IRQ32_63_SER & (1 << (irq_UART4_ID - 32))) != 0) { // if UART 4 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_UART4();       // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
#endif
#if UARTS_AVAILABLE > 1
	if (((iInts & CHANNEL_4_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART4];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_4_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
    #if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_4_SERIAL_INT;                          // interrupt has been handled
                fnLogTx4((unsigned char)UART5_TDR);
                ulNewActions |= SEND_COM_4;
        #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32L4X5 || defined _STM32L4X6
                UART5_ISR |= USART_ISR_TXE;
        #else
                UART5_SR |= USART_SR_TXE;
        #endif
                if ((UART5_CR1 & USART_CR1_TXEIE) != 0) {                // if tx interrupts enabled
                    if ((IRQ32_63_SER & (1 << (irq_UART5_ID - 32))) != 0) { // if UART 5 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_UART5();       // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L4X5 || defined _STM32L4X6
	if ((iInts & CHANNEL_5_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART5];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_5_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
    #if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_5_SERIAL_INT;                          // interrupt has been handled
        #if defined _STM32L4X5 || defined _STM32L4X6
                fnLogTx5((unsigned char)LPUART1_TDR);
        #else
                fnLogTx5((unsigned char)USART6_TDR);
        #endif
                ulNewActions |= SEND_COM_5;
        #if defined _STM32F7XX
                USART6_ISR |= USART_ISR_TXE;
        #elif defined _STM32L4X5 || defined _STM32L4X6
                LPUART1_ISR |= USART_ISR_TXE;
        #else
                USART6_SR |= USART_SR_TXE;
        #endif
        #if defined _STM32L4X5 || defined _STM32L4X6
                if (LPUART1_CR1 & USART_CR1_TXEIE) {                     // if tx interrupts enabled
                    if (IRQ64_95_SER & (1 << (irq_LPUART1_ID - 64))) {   // if LPUART1 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_LPUART1();     // call the interrupt handler
                    }
                }
        #else
                if (USART6_CR1 & USART_CR1_TXEIE) {                      // if tx interrupts enabled
                    if (IRQ64_95_SER & (1 << (irq_USART6_ID - 64))) {    // if USART 6 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_USART6();      // call the interrupt handler
                    }
                }
        #endif
    #endif
            }
        }
	}
#endif

#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {9}
	if (((iInts & CHANNEL_6_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART6];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_6_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
    #if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_6_SERIAL_INT;                          // interrupt has been handled
                fnLogTx6((unsigned char)UART7_TDR);
                ulNewActions |= SEND_COM_6;
        #if defined _STM32F7XX
                UART7_ISR |= USART_ISR_TXE;
        #else
                UART7_SR |= USART_SR_TXE;
        #endif
                if ((UART7_CR1 & USART_CR1_TXEIE) != 0) {                // if tx interrupts enabled
                    if (IRQ64_95_SER & (1 << (irq_UART7_ID - 64))) {     // if UART 7 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_UART7();       // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
	if (((iInts & CHANNEL_7_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART7];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_7_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
    #if defined SERIAL_INTERFACE
		        iInts &= ~CHANNEL_7_SERIAL_INT;                          // interrupt has been handled
                fnLogTx7((unsigned char)UART8_TDR);
                ulNewActions |= SEND_COM_7;
        #if defined _STM32F7XX
                UART8_ISR |= USART_ISR_TXE;
        #else
                UART8_SR |= USART_SR_TXE;
        #endif
                if ((UART8_CR1 & USART_CR1_TXEIE) != 0) {                // if tx interrupts enabled
                    if (IRQ64_95_SER & (1 << (irq_UART8_ID - 64))) {     // if UART 8 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_UART8();       // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
#endif

    if ((iInts & I2C_INT0) != 0) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT0;                                      // enough I2C interupts handled in this tick period
            }
            else {
#if defined I2C_INTERFACE
		        iInts &= ~I2C_INT0;
                if ((I2C1_CR2 & (I2C_CR2_ITBUFEN)) != 0) {               // if tx interrupts enabled
                    if (IRQ0_31_SER & (1 << irq_I2C1_EV_ID)) {           // if I2C1 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_I2C1_EV();     // call the interrupt handler
                    }
                }
#endif
            }
        }
    }
    if (iInts & I2C_INT1) {                                              // {3}
        ptrCnt = (int *)argv[THROUGHPUT_I2C1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT1;                                      // enough I2C interupts handled in this tick period
            }
            else {
#if defined I2C_INTERFACE
		        iInts &= ~I2C_INT1;
                if ((I2C2_CR2 & (I2C_CR2_ITBUFEN)) != 0) {               // if tx interrupts enabled
                    if (IRQ32_63_SER & (1 << (irq_I2C2_EV_ID - 32))) {   // if I2C2 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_I2C2_EV();     // call the interrupt handler
                    }
                }
#endif
            }
        }
    }
#if CHIP_HAS_I2C > 2                                                     // {3}
    if (iInts & I2C_INT2) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C2];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT2;                                      // enough I2C interupts handled in this tick period
            }
            else {
#if defined I2C_INTERFACE
		        iInts &= ~I2C_INT2;
                I2C3_SR1 |= I2C_SR1_TxE;                                 // transmitter buffer empty
                I2C3_SR1 &= ~(I2C_SR1_BTF);                              // transfer not yet complete
                if ((I2C3_CR2 & (I2C_CR2_ITBUFEN)) != 0) {               // if tx interrupts enabled
                    if (IRQ64_95_SER & (1 << (irq_I2C3_EV_ID - 64))) {   // if I2C3 interrupt is not disabled
                        ptrVect->processor_interrupts.irq_I2C3_EV();     // call the interrupt handler
                    }
                }
                if ((iInts & I2C_INT2) == 0) {
                    I2C3_SR1 |= (I2C_SR1_BTF | I2C_SR1_SB);              // transfer complete or start complete
                    if ((I2C3_CR2 & (I2C_CR2_ITEVTEN)) != 0) {           // if event interrupts enabled
                        if (IRQ64_95_SER & (1 << (irq_I2C3_EV_ID - 64))) { // if I2C3 interrupt is not disabled
                            ptrVect->processor_interrupts.irq_I2C3_EV(); // call the interrupt handler
                        }
                    }
                }
#endif
            }
        }
    }
#endif
#if defined USB_INTERFACE && defined USB_DEVICE_AVAILABLE
    if (iInts & USB_INT) {
        int iEndpoint = 0;
        iInts &= ~USB_INT;
        while ((iEndpoint < NUMBER_OF_USB_ENDPOINTS) && (ulEndpointInt != 0)) {
            if (ulEndpointInt & (1 << iEndpoint)) {
                ulEndpointInt &= ~(1 << iEndpoint);
                fnCheckUSBOut(0, iEndpoint);
            }
            iEndpoint++;
        }
    }
#endif
    return ulNewActions;
}

// Process simulated DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
#if defined DMA_MEMCPY_SET
    if (argv == 0) {                                                     // memory to memory transfer
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX) // {7} memory to memory transfers are only suppoirted on DMA2
        if ((DMA2_S1CR & DMA_SxCR_EN) != 0) {                            // if enabled
            unsigned short usLength = (unsigned short)DMA2_S1NDTR;       // the transfer length
            unsigned char *ptrDestination = (unsigned char *)DMA2_S1M0AR;
            unsigned char *ptrSource = (unsigned char *)DMA2_S1PAR;
            if ((DMA2_S1CR & (DMA_SxCR_DIR_M2M | DMA_SxCR_DIR_M2P)) != (DMA_SxCR_DIR_M2M)) {
                _EXCEPTION("DMA configuration doesn't seem to be correct!!");
            }
            switch (DMA2_S1CR & (DMA_SxCR_PSIZE_32 | DMA_SxCR_PSIZE_16)) {
            case DMA_SxCR_PSIZE_8:
                if ((DMA2_S1CR & (DMA_SxCR_MSIZE_32 | DMA_SxCR_MSIZE_16)) != DMA_SxCR_MSIZE_8) {
                    _EXCEPTION("Source and destination sizes are not equal!!");
                }
                while (usLength-- != 0) {
                    *ptrDestination = *ptrSource;
                    if ((DMA2_S1CR & DMA_SxCR_MINC) != 0) {
                        ptrDestination++;
                    }
                    if ((DMA2_S1CR & DMA_SxCR_PINC) != 0) {
                        ptrSource++;
                    }
                }
                break;
            case DMA_SxCR_PSIZE_16:
                if ((DMA2_S1CR & (DMA_SxCR_MSIZE_32 | DMA_SxCR_MSIZE_16)) != DMA_SxCR_MSIZE_16) {
                    _EXCEPTION("Source and destination sizes are not equal!!");
                }
                while (usLength-- != 0) {
                    *(unsigned short *)ptrDestination = *(unsigned short *)ptrSource;
                    if ((DMA2_S1CR & DMA_SxCR_MINC) != 0) {
                        ptrDestination += sizeof(unsigned short);
                    }
                    if ((DMA2_S1CR & DMA_SxCR_PINC) != 0) {
                        ptrSource += sizeof(unsigned short);
                    }
                }
                break;
            case DMA_SxCR_PSIZE_32:
                if ((DMA2_S1CR & (DMA_SxCR_MSIZE_32 | DMA_SxCR_MSIZE_16)) != DMA_SxCR_MSIZE_32) {
                    _EXCEPTION("Source and destination sizes are not equal!!");
                }
                while (usLength-- != 0) {
                    *(unsigned long *)ptrDestination = *(unsigned long *)ptrSource;
                    if ((DMA2_S1CR & DMA_SxCR_MINC) != 0) {
                        ptrDestination += sizeof(unsigned long);
                    }
                    if ((DMA2_S1CR & DMA_SxCR_PINC) != 0) {
                        ptrSource += sizeof(unsigned long);
                    }
                }
                break;
            }
            DMA2_S1NDTR = 0;
            DMA2_LISR = (DMA_LISR_TCIF1 | DMA_LISR_HTIF1);
            DMA2_S1CR &= ~(DMA_SxCR_EN);
        }
    #else        
        if (DMA_CCR_MEMCPY & DMA1_CCR1_EN) {                             // if enabled
            unsigned short usLength = (unsigned short)DMA_CNDTR_MEMCPY;  // the transfer length
            unsigned char *ptrSource = (unsigned char *)DMA_CMAR_MEMCPY;
            unsigned char *ptrDestination = (unsigned char *)DMA_CPAR_MEMCPY;
            if ((DMA_CCR_MEMCPY & (DMA1_CCR1_MEM2MEM | DMA1_CCR1_DIR)) != (DMA1_CCR1_MEM2MEM | DMA1_CCR1_DIR)) {
                _EXCEPTION("DMA configuration doesn't seem to be correct!!");
            }

            while (usLength-- != 0) {
                *ptrDestination = *ptrSource;
                if (DMA_CCR_MEMCPY & DMA1_CCR1_PINC) {
                    ptrDestination++;
                }
                if (DMA_CCR_MEMCPY & DMA1_CCR1_MINC) {
                    ptrSource++;
                }
            }
            DMA_CNDTR_MEMCPY = 0;
        #if MEMCPY_CHANNEL > 7
            DMA2_ISR |= (DMA1_ISR_TCIF1 << ((MEMCPY_CHANNEL - 8) * 4));
        #else
            DMA1_ISR |= (DMA1_ISR_TCIF1 << ((MEMCPY_CHANNEL - 1) * 4));
        #endif
        }
    #endif
        return 0;
    }
#endif
    return 0;
}

// Periodic tick - dummy since the timer is now handled by timer simulator
//
extern void RealTimeInterrupt(void)
{
}

#if defined I2C_INTERFACE
extern void fnSimulateI2C(int iPort, unsigned char *ptrDebugIn, unsigned short usLen, int iRepeatedStart)
{
    _EXCEPTION("To do!!!");
}
#endif

// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
#if defined SERIAL_INTERFACE
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    switch (iPort) {
    case 0:                                                              // USART 1
	    while (usLen-- != 0) {
    #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
		    USART1_RDR = *ptrDebugIn++;                                  // put received byte to input buffer
            USART1_ISR |= USART_ISR_RXNE;
    #else
		    USART1_DR = *ptrDebugIn++;                                   // put received byte to input buffer
            USART1_SR |= USART_SR_RXNE;
    #endif
            if ((USART1_CR1 & USART_CR1_RXNEIE) != 0) {                  // if rx interrupts enabled
                if (fnGenInt(irq_USART1_ID) != 0) {                      // if USART 1 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_USART1();          // call the interrupt handler (note that UART0 is refered to as USART1 at the STM32)
                }
            }
	    }
        break;
    #if USARTS_AVAILABLE > 2
    case 1:                                                              // USART 2
	    while (usLen-- != 0) {
        #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
		    USART2_RDR = *ptrDebugIn++;                                  // put received byte to input buffer
            USART2_ISR |= USART_ISR_RXNE;
        #else
		    USART2_DR = *ptrDebugIn++;                                   // put received byte to input buffer
            USART2_SR |= USART_SR_RXNE;
        #endif
            if ((USART2_CR1 & USART_CR1_RXNEIE) != 0) {                  // if rx interrupts enabled
                if (fnGenInt(irq_USART2_ID) != 0) {                      // if USART 2 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_USART2();          // call the interrupt handler
                }
            }
	    }
        break;
    #endif
    #if USARTS_AVAILABLE > 2 && !defined USART3_NOT_PRESENT
    case 2:                                                              // USART 3
	    while (usLen-- != 0) {
        #if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32L4X5 || defined _STM32L4X6
		    USART3_RDR = *ptrDebugIn++;                                  // put received byte to input buffer
            USART3_ISR |= USART_ISR_RXNE;
        #else
		    USART3_DR = *ptrDebugIn++;                                   // put received byte to input buffer
            USART3_SR |= USART_SR_RXNE;
        #endif
            if ((USART3_CR1 & USART_CR1_RXNEIE) != 0) {                  // if rx interrupts enabled
                if ((IRQ32_63_SER & (1 << (irq_USART3_ID - 32))) != 0) { // if USART 3 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_USART3();          // call the interrupt handler
                }
            }
	    }
        break;
    #endif
    #if UARTS_AVAILABLE > 0
    case 3:                                                              // UART 4
	    while (usLen-- != 0) {
        #if defined _STM32F7XX || defined _STM32L4X5 || defined _STM32L4X6
		    UART4_RDR = *ptrDebugIn++;                                   // put received byte to input buffer
            UART4_ISR |= USART_ISR_RXNE;
        #else
		    UART4_DR = *ptrDebugIn++;                                    // put received byte to input buffer
            UART4_SR |= USART_SR_RXNE;
        #endif
            if ((UART4_CR1 & USART_CR1_RXNEIE) != 0) {                   // if rx interrupts enabled
                if ((IRQ32_63_SER & (1 << (irq_UART4_ID - 32))) != 0) {  // if UART 4 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_UART4();           // call the interrupt handler
                }
            }
	    }
        break;
    #endif
    #if UARTS_AVAILABLE > 1
    case 4:                                                              // UART 5
	    while (usLen-- != 0) {
        #if defined _STM32F7XX || defined _STM32L4X5 || defined _STM32L4X6
		    UART5_RDR = *ptrDebugIn++;                                   // put received byte to input buffer
            UART5_ISR |= USART_ISR_RXNE;
        #else
		    UART5_DR = *ptrDebugIn++;                                    // put received byte to input buffer
            UART5_SR |= USART_SR_RXNE;
        #endif
            if ((UART5_CR1 & USART_CR1_RXNEIE) != 0) {                   // if rx interrupts enabled
                if ((IRQ32_63_SER & (1 << (irq_UART5_ID - 32))) != 0) {  // if UART 5 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_UART5();           // call the interrupt handler
                }
            }
	    }
        break;
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:                                                              // USART 6
	    while (usLen-- != 0) {
        #if defined _STM32F7XX
		    USART6_RDR = *ptrDebugIn++;                                  // put received byte to input buffer
            USART6_ISR |= USART_ISR_RXNE;
        #else
		    USART6_DR = *ptrDebugIn++;                                   // put received byte to input buffer
            USART6_SR |= USART_SR_RXNE;
        #endif
            if (USART6_CR1 & USART_CR1_RXNEIE) {                         // if rx interrupts enabled
                if (IRQ64_95_SER & (1 << (irq_USART6_ID - 64))) {        // if USART 6 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_USART6();          // call the interrupt handler
                }
            }
	    }
        break;
    #elif defined _STM32L4X5 || defined _STM32L4X6
    case 5:                                                              // LPUART 1
	    while (usLen-- != 0) {
		    LPUART1_RDR = *ptrDebugIn++;                                 // put received byte to input buffer
            LPUART1_ISR |= USART_ISR_RXNE;
            if (LPUART1_CR1 & USART_CR1_RXNEIE) {                        // if rx interrupts enabled
                if (IRQ64_95_SER & (1 << (irq_LPUART1_ID - 64))) {       // if LPUART 1 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_LPUART1();         // call the interrupt handler
                }
            }
	    }
        break;
    #endif
    #if CHIP_HAS_UARTS > 6
    case 6:                                                              // USART 7
	    while (usLen-- != 0) {
        #if defined _STM32F7XX
		    UART7_RDR = *ptrDebugIn++;                                   // put received byte to input buffer
            UART7_ISR |= USART_ISR_RXNE;
        #else
		    UART7_DR = *ptrDebugIn++;                                    // put received byte to input buffer
            UART7_SR |= USART_SR_RXNE;
        #endif
            if (UART7_CR1 & USART_CR1_RXNEIE) {                          // if rx interrupts enabled
                if (IRQ64_95_SER & (1 << (irq_UART7_ID - 64))) {         // if USART 7 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_UART7();           // call the interrupt handler
                }
            }
	    }
        break;
    #endif
    #if CHIP_HAS_UARTS > 7
    case 7:                                                              // USART 8
	    while (usLen-- != 0) {
        #if defined _STM32F7XX
		    UART8_RDR = *ptrDebugIn++;                                   // put received byte to input buffer
            UART8_ISR |= USART_ISR_RXNE;
        #else
		    UART8_DR = *ptrDebugIn++;                                    // put received byte to input buffer
            UART8_SR |= USART_SR_RXNE;
        #endif
            if ((UART8_CR1 & USART_CR1_RXNEIE) != 0) {                   // if rx interrupts enabled
                if (IRQ64_95_SER & (1 << (irq_UART8_ID - 64))) {         // if USART 8 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_UART8();           // call the interrupt handler
                }
            }
	    }
        break;
    #endif
    default:
        break;
    }
#endif
}

extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState)
{
#if defined SUPPORT_HW_FLOW
    switch (iPort) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:
        break;
    #endif
    }
#endif
}

// UART Break detection simulation
//
extern void fnSimulateBreak(int iPort)
{
}

#if defined SUPPORT_KEY_SCAN
static unsigned char fnMapPortBit(unsigned short usRealBit)              // {11}
{
    unsigned char ucBit = 0;

    while (ucBit < 16) {
        ucBit++;
        if ((usRealBit & (1 << ucBit)) != 0) {
            return (15 - ucBit);
        }
    }
    return 0;
}


int iKeyPadInputs[KEY_COLUMNS][KEY_ROWS] = {0};

extern void fnSimulateKeyChange(int *intTable)
{
    memcpy(iKeyPadInputs, intTable, sizeof(iKeyPadInputs));              // copy key pad state to local set
}

static int fnColumnLow(int iColumnOutput)
{
    #if defined KEY_COLUMNS
    switch (iColumnOutput) {
    case 0:
        return (KEY_COL_OUT_1 & ~KEY_COL_OUT_PORT_1 & KEY_COL_OUT_DDR_1 && KEY_COL_OUTFUNC_PORT_1());
        #if KEY_COLUMNS > 1
    case 1:
        return (KEY_COL_OUT_2 & ~KEY_COL_OUT_PORT_2 & KEY_COL_OUT_DDR_2 && KEY_COL_OUTFUNC_PORT_2());
        #endif
        #if KEY_COLUMNS > 2
    case 2:
        return (KEY_COL_OUT_3 & ~KEY_COL_OUT_PORT_3 & KEY_COL_OUT_DDR_3 && KEY_COL_OUTFUNC_PORT_3());
        #endif
        #if KEY_COLUMNS > 3
    case 3:
        return (KEY_COL_OUT_4 & ~KEY_COL_OUT_PORT_4 & KEY_COL_OUT_DDR_4 && KEY_COL_OUTFUNC_PORT_4());
        #endif
        #if KEY_COLUMNS > 4
    case 4:
        return (KEY_COL_OUT_5 & ~KEY_COL_OUT_PORT_5 & KEY_COL_OUT_DDR_5 && KEY_COL_OUTFUNC_PORT_5());
        #endif
        #if KEY_COLUMNS > 5
    case 5:
        return (KEY_COL_OUT_6 & ~KEY_COL_OUT_PORT_6 & KEY_COL_OUT_DDR_6 && KEY_COL_OUTFUNC_PORT_6());
        #endif
        #if KEY_COLUMNS > 6
    case 6:
        return (KEY_COL_OUT_7 & ~KEY_COL_OUT_PORT_7 & KEY_COL_OUT_DDR_7 && KEY_COL_OUTFUNC_PORT_7());
        #endif
        #if KEY_COLUMNS > 7
    case 7:
        return (KEY_COL_OUT_8 & ~KEY_COL_OUT_PORT_8 & KEY_COL_OUT_DDR_8 && KEY_COL_OUTFUNC_PORT_8());
        #endif
    }
    #endif
    return 0;
}

static void fnSetRowInput(int iRowInput, int iState)
{
    int iChange;

    if (iState) {
        iChange = CLEAR_INPUT;
    }
    else {
        iChange = SET_INPUT;
    }

    switch (iRowInput) {
    case 0:
    #if defined KEY_ROWS
        fnSimulateInputChange(KEY_ROW_IN_PORT_1_REF, fnMapPortBit(KEY_ROW_IN_1), iChange);
    #endif
        break;
    case 1:
    #if KEY_ROWS > 1
        fnSimulateInputChange(KEY_ROW_IN_PORT_2_REF, fnMapPortBit(KEY_ROW_IN_2), iChange);
    #endif
        break;
    case 2:
    #if KEY_ROWS > 2
        fnSimulateInputChange(KEY_ROW_IN_PORT_3_REF, fnMapPortBit(KEY_ROW_IN_3), iChange);
    #endif
        break;
    case 3:
    #if KEY_ROWS > 3
        fnSimulateInputChange(KEY_ROW_IN_PORT_4_REF, fnMapPortBit(KEY_ROW_IN_4), iChange);
    #endif
        break;
    case 4:
    #if KEY_ROWS > 4
        fnSimulateInputChange(KEY_ROW_IN_PORT_5_REF, fnMapPortBit(KEY_ROW_IN_5), iChange);
    #endif
        break;
    case 5:
    #if KEY_ROWS > 5
        fnSimulateInputChange(KEY_ROW_IN_PORT_6_REF, fnMapPortBit(KEY_ROW_IN_6), iChange);
    #endif
        break;
    case 6:
    #if KEY_ROWS > 6
        fnSimulateInputChange(KEY_ROW_IN_PORT_7_REF, fnMapPortBit(KEY_ROW_IN_7), iChange);
    #endif
        break;
    case 7:
    #if KEY_ROWS > 7
        fnSimulateInputChange(KEY_ROW_IN_PORT_8_REF, fnMapPortBit(KEY_ROW_IN_8), iChange);
    #endif
        break;
    }
}

// This routine updates the ports to respect the present setting of a connected matrix key pad
//
extern void fnSimMatrixKB(void)
{
    int i, j;

    iFlagRefresh = fnPortChanges(1);                                     // synchronise with present settings
    // Check whether a column control is being driven low. If this is the case, any pressed ones in the column are seen at the row input
    //
    for (i = 0; i < KEY_COLUMNS; i++) {
        if (fnColumnLow(i)) {
            for (j = 0; j < KEY_ROWS; j++) {
                fnSetRowInput(j, iKeyPadInputs[i][j]);
            }
        }
    }
}
#endif

static void fnHandleExti(unsigned short usOriginal, unsigned short usNew, unsigned char ucPort)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    int iInputCount = 0;
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    unsigned long *ptrMux = SYSCFG_EXTICR1_ADDR;
    #else
    unsigned long *ptrMux = AFIO_EXTICR1_ADD;
    #endif
    unsigned short usChange  = (usOriginal ^ usNew);                     // changed input(s)
    unsigned short usRising  = (usChange & usNew);                       // positive changes
    unsigned short usFalling = (usChange & ~usNew);                      // negative changes
    unsigned short usBit = 0x0001;
    while (usChange != 0) {                                              // while all changes have not been checked
        if ((usChange & usBit) != 0) {                                   // change on this input
            if (((*ptrMux >> (4 * iInputCount)) & 0xf) == ucPort) {      // if this input is connected to this port
                if (((usBit & usRising & EXTI_RTSR) != 0) || ((usBit & usFalling & EXTI_FTSR) != 0)) {  // if the particular edge is enabled
                    EXTI_PR |= usBit;                                    // set the interrupt pending bit
                    if ((usBit & EXTI_IMR) != 0) {                       // interrupt not masked
    #if defined irq_EXTI0_ID
                        if ((usBit & 0x0001) != 0) {
                            if (IRQ0_31_SER & (1 << irq_EXTI0_ID)) {    // if EXTI0 interrupt is not disabled
                                ptrVect->processor_interrupts.irq_EXTI0(); // call the interrupt handler
                            }
                        }
                        else if ((usBit & 0x0002) != 0) {
                            if (IRQ0_31_SER & (1 << irq_EXTI1_ID)) {    // if EXTI1 interrupt is not disabled
                                ptrVect->processor_interrupts.irq_EXTI1(); // call the interrupt handler
                            }
                        }
                        else if ((usBit & 0x0004) != 0) {
                            if (IRQ0_31_SER & (1 << irq_EXTI2_ID)) {    // if EXTI2 interrupt is not disabled
                                ptrVect->processor_interrupts.irq_EXTI2(); // call the interrupt handler
                            }
                        }
                        else if ((usBit & 0x0008) != 0) {
                            if (IRQ0_31_SER & (1 << irq_EXTI3_ID)) {    // if EXTI3 interrupt is not disabled
                                ptrVect->processor_interrupts.irq_EXTI3(); // call the interrupt handler
                            }
                        }
                        else if ((usBit & 0x0010) != 0) {
                            if ((IRQ0_31_SER & (1 << irq_EXTI4_ID)) != 0) {    // if EXTI3 interrupt is not disabled
                                ptrVect->processor_interrupts.irq_EXTI4(); // call the interrupt handler
                            }
                        }
                        else if ((usBit & 0x03e0) != 0) {
                            if (IRQ0_31_SER & (1 << irq_EXTI9_5_ID)) {   // if EXTI5..9 interrupt is not disabled
                                ptrVect->processor_interrupts.irq_EXTI9_5(); // call the interrupt handler
                            }
                        }
                        else if ((usBit & 0xfc00) != 0) {
                            if (IRQ32_63_SER & (1 << (irq_EXTI15_10_ID - 32))) { // if EXTI10..15 interrupt is not disabled
                                ptrVect->processor_interrupts.irq_EXTI15_10(); // call the interrupt handler
                            }
                        }
    #endif
                    }
                }
            }
            usChange &= ~usBit;
        }
        usBit <<= 1;
        iInputCount++;
        if (iInputCount == 4) {
            iInputCount = 0;
            ptrMux++;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure that ports are updated when a peripheral function changes
    }
}

extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
    unsigned short usBit;
    unsigned short usOriginal;
    ucPortBit = (15 - ucPortBit);
    usBit = (0x01 << ucPortBit);
    if ((ulGPIODDR[ucPort] & usBit) != 0) {                              // check whether the pin is programmed as an output
        return;                                                          // pin is not programmed as an input
    }
    if (__GPIO_IS_POWERED(ucPort) == 0) {
        return;                                                          // not clocked so ignore
    }
    if (__GPIO_IS_IN_RESET(ucPort) != 0) {
        return;                                                          // in reset so ignore
    }
    usOriginal = (unsigned short)ulGPIOIN[ucPort];
    if (iChange == TOGGLE_INPUT) {
        ulGPIOIN[ucPort] ^= usBit;                                       // toggle the input state
    }
    else if (iChange == SET_INPUT) {
        ulGPIOIN[ucPort] |= usBit;                                       // set the input high
    }
    else {
        ulGPIOIN[ucPort] &= ~usBit;                                      // set the input low
    }
    switch (ucPort) {
    case _PORTA:
        GPIOA_IDR = ((ulGPIODDR[ucPort] & GPIOA_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
    case _PORTB:
        GPIOB_IDR = ((ulGPIODDR[ucPort] & GPIOB_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
    case _PORTC:
        GPIOC_IDR = ((ulGPIODDR[ucPort] & GPIOC_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
    case _PORTD:
        GPIOD_IDR = ((ulGPIODDR[ucPort] & GPIOD_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
    case _PORTE:
        GPIOE_IDR = ((ulGPIODDR[ucPort] & GPIOE_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
    case _PORTF:
        GPIOF_IDR = ((ulGPIODDR[ucPort] & GPIOF_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
    case _PORTG:
        GPIOG_IDR = ((ulGPIODDR[ucPort] & GPIOG_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case _PORTH:
        GPIOH_IDR = ((ulGPIODDR[ucPort] & GPIOH_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
    case _PORTI:
        GPIOI_IDR = ((ulGPIODDR[ucPort] & GPIOI_ODR) | (~ulGPIODDR[ucPort] & ulGPIOIN[ucPort]));
        break;
#endif
    }
    fnHandleExti(usOriginal, (unsigned short)ulGPIOIN[ucPort], ucPort);  // handle possible port change interrupts
}


// Simulate buffers for Ethernet use
//
unsigned char *fnGetSimTxBufferAdd(void)
{
#if defined ETH_INTERFACE
    return (fnGetTxBufferAdd(0));
#else
    return 0;
#endif
}



#if defined ETH_INTERFACE
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
#endif

extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce)
{
#if defined ETH_INTERFACE                                                // we feed frames in promiscuous mode and filter them according to their details
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    int iType;
    STM32_BD *ptrRxBd;

    if ((!iForce) && (!(iType = fnCheckEthernetMode(ucData, usLen)))) {  // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
        return 0;
    }
    ptrRxBd = (STM32_BD *)ETH_DMACHRDR;                                  // present receive buffer description
    if ((ptrRxBd->bd_word1 & RX_BD_WORD1_RBS1_MASK) < usLen) {           // check that the buffer has enough space
        usLen = (unsigned short)(ptrRxBd->bd_word1 & RX_BD_WORD1_RBS1_MASK);
    }
    ptrRxBd->bd_word0 &= ~(RX_BD_WORD0_FL_MASK | RX_BD_WORD0_OWN | RX_BD_WORD0_FT | RX_BD_WORD0_PCE | RX_BD_WORD0_IPHCE);
    ptrRxBd->bd_word0 |= ((usLen + 4) << RX_BD_WORD0_FL_SHIFT);          // size including CRC
    uMemcpy((unsigned char *)ptrRxBd->bd_dma_buf1_address, ucData, usLen); // copy frame to input buffer
    if (((ucData[12] == 0x08) && (ucData[13] == 0x00)) || (((ucData[12] == 0x86) && (ucData[13] == 0xdd)))) { // if IPv4 or IPv6 (no VLAN expected)
        ptrRxBd->bd_word0 |= RX_BD_WORD0_FT;                             // IPv4/v6 type frame
        if (ETH_MACCR & ETH_MACCR_IPCO) {                                // if reception checksum enabled
          //ptrRxBd->bd_word0 |= (RX_BD_WORD0_PCE | RX_BD_WORD0_IPHCE);  // error could be signalled here
        }
    }
    ETH_DMACHRDR = (unsigned long)ptrRxBd->bd_dma_buf2_address;          // next receive buffer descriptor
    ETH_DMASR |= ETH_DMAIER_RIE;                                         // set the status flag
    if (ETH_DMAIER & ETH_DMAIER_RIE) {                                   // if receive interrupt is enabled
        IRQ32_63_SPR |= (1 << (irq_ETH_ID - 32));
        IRQ32_63_CPR = IRQ32_63_SPR;
        if (IRQ32_63_SER & (1 << (irq_ETH_ID - 32))) {                   // if Ethernet interrupt is not disabled
            ptrVect->processor_interrupts.irq_ETH();                     // call the interrupt handler
        }
    }
    return 1;
#else
    return 0;
#endif
}

static void fnUpdatePeripheral(int iPort, unsigned long ulPeriph)
{
    int i = 0;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    int iHigh = 0;
    int iFunctionShift = 0;
    unsigned long ulMaskMode = 0x00000003;
    STM32_GPIO *ptrPort = &STM32.Ports[iPort];
    while (i < PORT_WIDTH) {
        if ((iPort == _PORTH) && (i <= 1)) {                            // {2}
            if ((RCC_CR & RCC_CR_HSEON) != 0) {                          // if external oscillator is enabled
                ucPortFunctions[iPort][i] = _ADDITIONAL_FUNCTION;        // OSC_IN and OSC_OUT enabled on PH0 and PH1
            }
        }
        else {
            if (((ptrPort->GPIO_MODER & ulMaskMode) >> (i * 2)) == GPIO_MODER_ALTERNATE_FUNC) { // if port bit set as alternative function
                if (iHigh == 0) {
                    ucPortFunctions[iPort][i] = (unsigned char)((ptrPort->GPIO_AFRL >> iFunctionShift) & 0x0000000f);
                }
                else {
                    ucPortFunctions[iPort][i] = (unsigned char)((ptrPort->GPIO_AFRH >> iFunctionShift) & 0x0000000f);
                }
            }
            else if (((ptrPort->GPIO_MODER & ulMaskMode) >> (i * 2)) == GPIO_MODER_ANALOG) { // if port bit set as analog function
                ucPortFunctions[iPort][i] = 16;                          // additional
            }
            else {
                ucPortFunctions[iPort][i] = 0;
            }
        }
        ulMaskMode <<= 2;
        iFunctionShift += 4;
        if (iFunctionShift >= 32) {
            iFunctionShift = 0;
            iHigh = 1;
        }
        i++;
    }
#else
    unsigned long ulBit = 0x00000001;
    switch (iPort) {
    case _PORTA:
        while (i < PORT_WIDTH) {
            if (ulBit & ulPeriph) {                                      // for each port bit that has a peripheral function
                switch (i) {
                case 0:
                case 1:
    #if defined ETH_INTERFACE
                    if ((AFIO_MAPR & MII_RMII_SEL) == 0) {               // MII
                        ucPortFunctions[iPort][i] = 0;                   // this matches with the value in STM32_ports.c
                    }
                    else {                                               // RMII
                    }
    #endif
                    break;
                case 2:
                    if (((AFIO_MAPR & USART2_REMAPPED) == 0) && ((USART2_CR1 & USART_CR1_RE) != 0)) {
                        ucPortFunctions[iPort][i] = 3;                   // USART2_TX
                    }
                    break;
                case 3:
                    if ((!(AFIO_MAPR & USART2_REMAPPED)) && (USART2_CR1 & USART_CR1_RE)) {
                        ucPortFunctions[iPort][i] = 3;                   // USART2_RX
                    }
                    else {
    #if defined ETH_INTERFACE
                        if (!(AFIO_MAPR & MII_RMII_SEL)) {               // MII
                            ucPortFunctions[iPort][i] = 0;               // ETH_MII_MDIO
                        }
                        else {                                           // RMII
                        }
    #endif
                    }
                    break;
                case 6:
                    if ((AFIO_MAPR & TIM3_FULL_REMAP) == 0) {
                        ucPortFunctions[iPort][i] = 1;                   // TIM3_CH1 this matches with the value in STM32_ports.c
                    }                    
                    break;
                case 7:
    #if defined _CONNECTIVITY_LINE
                    if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) {     // {5}
                        ucPortFunctions[iPort][i] = 3;
                        break;
                    }
    #endif
                    if ((AFIO_MAPR & TIM3_FULL_REMAP) == 0) {
                        ucPortFunctions[iPort][i] = 1;                   // TIM3_CH2 this matches with the value in STM32_ports.c
                    }                    
                    break;
                case 8:
                    ucPortFunctions[iPort][i] = 0;                       // this matches with the value in STM32_ports.c
                    break;
                case 9:
    #if defined _CONNECTIVITY_LINE                                       // {4}
                    if ((OTG_FS_GCCFG & OTG_FS_GCCFG_PWRDWN) != 0) {     // if USB PHY is enabled
                        if (OTG_FS_GCCFG & (OTG_FS_GCCFG_VBUSASEN | OTG_FS_GCCFG_VBUSBSEN)) { // bus monitoring enabled
                            ucPortFunctions[iPort][i] = 3;               // peripheral USB_VBUS
                            break;
                        }
                    }
    #endif
                case 10:
                    ucPortFunctions[iPort][i] = 1;                       // UART1
                    break;
                case 11:
                case 12:
    #if defined _CONNECTIVITY_LINE && defined USB_OTG_AVAILABLE          // {4}
                    if ((OTG_FS_GCCFG & OTG_FS_GCCFG_PWRDWN) != 0) {     // if USB PHY is enabled
                        ucPortFunctions[iPort][i] = 3;                   // peripheral USB_VBUS
                    }
                    break;
    #elif defined _PERFORMANCE_LINE && defined USB_DEVICE_AVAILABLE
                    if ((USB_CNTR & USB_CNTR_PDWN) == 0) {               // if USB PHY is connected
                        ucPortFunctions[iPort][i] = 2;                   // USBDP/USBDM
                    }
    #endif
                    break;
                }
            }
            i++;
            ulBit <<= 1;
        }
        break;
    case _PORTB:
        while (i < PORT_WIDTH) {
            if (ulBit & ulPeriph) {                                      // for each port bit that has a peripheral function enabled
                switch (i) {
                case 0:
                case 1:
                    if (((AFIO_MAPR & TIM3_FULL_REMAP) == TIM3_PARTIAL_REMAP) || ((AFIO_MAPR & TIM3_FULL_REMAP) == 0)) {
                        ucPortFunctions[iPort][i] = 1;                   // TIM3_CH3 / TIM3_CH4 this matches with the value in STM32_ports.c
                    }
                    break;
                case 4:
                    if ((AFIO_MAPR & TIM3_FULL_REMAP) == TIM3_PARTIAL_REMAP) {
                        ucPortFunctions[iPort][i] = 0;                   // TIM3_CH1 this matches with the value in STM32_ports.c
                    }                    
                    break;
                case 5:
                    if ((AFIO_MAPR & TIM3_FULL_REMAP) == TIM3_PARTIAL_REMAP) {
                        ucPortFunctions[iPort][i] = 1;                   // TIM3_CH2 this matches with the value in STM32_ports.c
                    }
                    else {
                        ucPortFunctions[iPort][i] = 0;
                    }
                    break;
                case 6:
                    if ((AFIO_MAPR & USART1_REMAPPED) && (USART1_CR1 & USART_CR1_TE)) { // if USART1 tx is enabled
                        ucPortFunctions[iPort][i] = 1;                   // USART1_TX
                    }
                    else {
                        ucPortFunctions[iPort][i] = 0;                   // I2C1_SDA
                    }
                    break;
                case 7:
                    if ((AFIO_MAPR & USART1_REMAPPED) && (USART1_CR1 & USART_CR1_RE)) { // if USART1 rx is enabled
                        ucPortFunctions[iPort][i] = 1;                   // USART1_RX
                    }
                    else {
                        ucPortFunctions[iPort][i] = 0;                   // I2C1_SDA
                    }
                    break;
                case 8:
                    ucPortFunctions[iPort][i] = 0;                       // this matches with the value in STM32_ports.c
                    break;
                case 10:
    #if defined _CONNECTIVITY_LINE
                    if ((ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) != 0) { // EMAC is enabled so assume this use
                        if ((AFIO_MAPR & MII_RMII_SEL) == 0) {           // {5} only in MII mode
                            ucPortFunctions[iPort][i] = 0;               // ETH_MII_RX_ER
                        }
                        break;
                    }
    #endif
    #if USARTS_AVAILABLE > 2
                    if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == 0) && ((USART3_CR1 & USART_CR1_RE) != 0)) {
                        ucPortFunctions[iPort][i] = 0;                   // USARTRX3
                    }
                    else {
                        ucPortFunctions[iPort][i] = 3;                   // I2C2_SCL
                    }
    #endif
                    break;
                case 11:
                    if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) {     // EMAC is enabled so assume this use
                        ucPortFunctions[iPort][i] = 0;                   // ETH_MII_TX_EN
                    }
    #if USARTS_AVAILABLE > 2
                    else if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == 0) && ((USART3_CR1 & USART_CR1_RE) != 0)) {
                        ucPortFunctions[iPort][i] = 0;                   // USART3_RX
                    }
    #endif
                    else {
                        ucPortFunctions[iPort][i] = 3;                   // I2C2_SDA
                    }
                    break;
                case 12:
                case 13:
                    ucPortFunctions[iPort][i] = 0;                       // this matches with the value in STM32_ports.c
                    break;
                }
            }
            i++;
            ulBit <<= 1;
        }
        break;
    case _PORTC:
        while (i < PORT_WIDTH) {
            if (ulBit & ulPeriph) {                                      // for each port bit that has a peripheral function
                switch (i) {
                case 1:
                case 2:
                case 3:
                    ucPortFunctions[iPort][i] = 0;                       // this matches with the value in STM32_ports.c
                    break;
                case 6:
                case 7:
                case 8:
                case 9:
                    if ((AFIO_MAPR & TIM3_FULL_REMAP) == TIM3_FULL_REMAP) {
                        ucPortFunctions[iPort][i] = 0;                   // TIM3_CHx this matches with the value in STM32_ports.c
                    }
                    break;
                case 10:
    #if USARTS_AVAILABLE > 2
                    if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == USART3_PARTIALLY_REMAPPED) && ((USART3_CR1 & USART_CR1_TE) != 0)) {
                        ucPortFunctions[iPort][i] = 1;                   // USART3_TX
                    }
        #if defined _CONNECTIVITY_LINE
                    else if (AFIO_MAPR & SPI3_REMAP) {                   // alternative mapping (SPI3 PA4:PC10:PC11:PC12)
                        ucPortFunctions[iPort][i] = 0;                   // SPI3 this matches with the value in STM32_ports.c
                    }
        #endif
    #endif
                    break;
                case 11:
    #if USARTS_AVAILABLE > 2
                    if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == USART3_PARTIALLY_REMAPPED) && ((USART3_CR1 & USART_CR1_RE) != 0)) {
                        ucPortFunctions[iPort][i] = 1;                   // USART3_RX
                    }
        #if defined _CONNECTIVITY_LINE
                    else if (AFIO_MAPR & SPI3_REMAP) {                   // alternative mapping (SPI3 PA4:PC10:PC11:PC12)
                        ucPortFunctions[iPort][i] = 0;                   // SPI3 this matches with the value in STM32_ports.c
                    }
        #endif
    #endif
                    break;
                case 12:
    #if defined _CONNECTIVITY_LINE
                    if (AFIO_MAPR & SPI3_REMAP) {                        // alternative mapping (SPI3 PA4:PC10:PC11:PC12)
                        ucPortFunctions[iPort][i] = 0;                   // SPI3 this matches with the value in STM32_ports.c
                    }
    #endif
                    break;
                }
            }
            i++;
            ulBit <<= 1;
        }
        break;
    case _PORTD:
        while (i < PORT_WIDTH) {
            if (ulBit & ulPeriph) {                                      // for each port bit that has a peripheral function
                switch (i) {
                case 8:
    #if USARTS_AVAILABLE > 2
                    if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == USART3_FULLY_REMAPPED) && ((USART3_CR1 & USART_CR1_TE) != 0)) {
                        ucPortFunctions[iPort][i] = 2;                   // USART3_TX
                    }
        #if defined _CONNECTIVITY_LINE
                    else if ((AFIO_MAPR & ETH_REMAP) != 0) {             // alternative mapping (EMAC PD8:PD9:PD10:PD11:PD12)
                        if ((AFIO_MAPR & MII_RMII_SEL) == 0) {           // MII
                            ucPortFunctions[iPort][i] = 0;               // this matches with the value in STM32_ports.c
                        }
                        else {                                           // RMII
                        }
                    }
        #endif
    #endif
                    break;
                case 9:
    #if USARTS_AVAILABLE > 2
                    if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == USART3_FULLY_REMAPPED) && ((USART3_CR1 & USART_CR1_RE) != 0)) {
                        ucPortFunctions[iPort][i] = 2;                   // USART3_RX
                    }
        #if defined _CONNECTIVITY_LINE
                    else if (AFIO_MAPR & ETH_REMAP) {                    // alternative mapping (EMAC PD8:PD9:PD10:PD11:PD12)
                        if (!(AFIO_MAPR & MII_RMII_SEL)) {               // MII
                            ucPortFunctions[iPort][i] = 0;               // this matches with the value in STM32_ports.c
                        }
                        else {                                           // RMII
                        }
                    }
        #endif
    #endif
                    break;
                case 10:
                case 11:
                case 12:
    #if defined _CONNECTIVITY_LINE
                    if (AFIO_MAPR & ETH_REMAP) {                         // alternative mapping (EMAC PD8:PD9:PD10:PD11:PD12)
                        if (!(AFIO_MAPR & MII_RMII_SEL)) {               // MII
                            ucPortFunctions[iPort][i] = 0;               // this matches with the value in STM32_ports.c
                        }
                        else {                                           // RMII
                        }
                    }
    #endif
                    break;
                case 5:
                case 6:
                    if (AFIO_MAPR & USART2_REMAPPED) {
                        ucPortFunctions[iPort][i] = 1;                   // this matches with the value in STM32_ports.c
                    }
                    break;
                }
            }
            i++;
            ulBit <<= 1;
        }
        break;
    case _PORTE:
        while (i < PORT_WIDTH) {
            if (ulBit & ulPeriph) {                                      // for each port bit that has a peripheral function
                switch (i) {
                default:
                    break;
                }
            }
            i++;
            ulBit <<= 1;
        }
        break;
    case _PORTF:
        while (i < PORT_WIDTH) {
            if (ulBit & ulPeriph) {                                      // for each port bit that has a peripheral function
                switch (i) {
                default:
                    break;
                }
            }
            i++;
            ulBit <<= 1;
        }
        break;
    case _PORTG:
        while (i < PORT_WIDTH) {
            if (ulBit & ulPeriph) {                                      // for each port bit that has a peripheral function
                switch (i) {
                default:
                    break;
                }
            }
            i++;
            ulBit <<= 1;
        }
        break;
    }
#endif
}

// We can update port state displays if we want
//
extern void fnSimPorts(void)
{
    unsigned long ulNewPeriph;
    int iPort = 0;

    while (iPort < PORTS_AVAILABLE) {
        ulNewPeriph = fnGetPresentPortPeriph(iPort + 1);
        if (ulGPIOPER[iPort] != ulNewPeriph) {
            ulGPIOPER[iPort] = ulNewPeriph;
            fnUpdatePeripheral(iPort, ulNewPeriph);                      // update peripheral details
            iFlagRefresh = PORT_CHANGE;                                  // ensure that ports are updated when a peripheral function changes
        }
        ulNewPeriph = fnGetPortType(iPort, GET_OUTPUTS, 0);
        if (ulGPIODDR[iPort] != ulNewPeriph) {
            ulGPIODDR[iPort] = ulNewPeriph;
            iFlagRefresh = PORT_CHANGE;                                  // ensure that ports are updated when a direction changes
        }
        switch (iPort) {
        case _PORTA:
            if ((GPIOA_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOA_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOA_IDR = ((ulGPIODDR[iPort] & GPIOA_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
        case _PORTB:
            if ((GPIOB_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOB_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOB_IDR = ((ulGPIODDR[iPort] & GPIOB_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
        case _PORTC:
            if ((GPIOC_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOC_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOC_IDR = ((ulGPIODDR[iPort] & GPIOC_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
        case _PORTD:
            if ((GPIOD_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOD_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOD_IDR = ((ulGPIODDR[iPort] & GPIOD_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
        case _PORTE:
            if ((GPIOE_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOE_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOE_IDR = ((ulGPIODDR[iPort] & GPIOE_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
        case _PORTF:
            if ((GPIOF_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOF_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOF_IDR = ((ulGPIODDR[iPort] & GPIOF_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
        case _PORTG:
            if ((GPIOG_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOG_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOG_IDR = ((ulGPIODDR[iPort] & GPIOG_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        case _PORTH:
            if ((GPIOH_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOH_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOH_IDR = ((ulGPIODDR[iPort] & GPIOH_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
        case _PORTI:
            if ((GPIOI_ODR & ulGPIODDR[iPort]) != ulGPIODRIVE_VALUE[iPort]) {
                ulGPIODRIVE_VALUE[iPort] = GPIOI_ODR & ulGPIODDR[iPort]; // new output driven state
                GPIOI_IDR = ((ulGPIODDR[iPort] & GPIOI_ODR) | (~ulGPIODDR[iPort] & ulGPIOIN[iPort]));
                iFlagRefresh = PORT_CHANGE;                              // ensure that ports are updated when an output state changes
            }
            break;
#endif
        }
        iPort++;
    }
}


extern int fnPortChanges(int iForce)
{
    int iRtn = iFlagRefresh;
    iFlagRefresh = 0;
    return iRtn;
}

extern unsigned long fnGetPresentPortState(int portNr)
{
    portNr -= 1;
    switch (portNr) {
    case _PORTA:
        return ((ulGPIODDR[portNr] & GPIOA_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
    case _PORTB:
        return ((ulGPIODDR[portNr] & GPIOB_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
    case _PORTC:
        return ((ulGPIODDR[portNr] & GPIOC_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
    case _PORTD:
        return ((ulGPIODDR[portNr] & GPIOD_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
    case _PORTE:
        return ((ulGPIODDR[portNr] & GPIOE_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
    case _PORTF:
        return ((ulGPIODDR[portNr] & GPIOF_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
    case _PORTG:
        return ((ulGPIODDR[portNr] & GPIOG_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case _PORTH:
        return ((ulGPIODDR[portNr] & GPIOH_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
    case _PORTI:
        return ((ulGPIODDR[portNr] & GPIOI_ODR) | (~ulGPIODDR[portNr] & ulGPIOIN[portNr]));
#endif
    default:
        return 0;
    }
}


static unsigned short fnGetPortType(int portNr, int iRequest, int i)
{
    unsigned short usPeripherals = 0;
    unsigned short usBit = 0x0001;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    unsigned long ulMask = 0x00000003;
    STM32_GPIO *ptrPort = &STM32.Ports[portNr];
    if (GET_OUTPUTS == iRequest) {
        while (i < PORT_WIDTH) {
            if (((ptrPort->GPIO_MODER & ulMask) >> (i * 2)) == GPIO_MODER_OUTPUT) { // if port bit set as output
                usPeripherals |= usBit;
            }
            usBit <<= 1;
            ulMask <<= 2;
            i++;
        }
    }
    else if (GET_PERIPHERALS == iRequest) {
        while (i < PORT_WIDTH) {
            if (((ptrPort->GPIO_MODER & ulMask) >> (i * 2)) == GPIO_MODER_ALTERNATE_FUNC) { // if port bit set as alternative function
                usPeripherals |= usBit;
            }
            else if (((ptrPort->GPIO_MODER & ulMask) >> (i * 2)) == GPIO_MODER_ANALOG) { // if port bit set as analog function
    #if !defined _STM32L0x1
                usPeripherals |= usBit;
    #endif
            }
            usBit <<= 1;
            ulMask <<= 2;
            i++;
        }
    #if !defined _STM32L0x1
        if (portNr == _PORTH) {                                         // {2}
            if ((RCC_CR & RCC_CR_HSEON) != 0) {                          // if external oscillator is enabled
                usPeripherals |= (PORTH_BIT0 | PORTH_BIT1);              // OSC_IN and OSC_OUT enabled on PH0 and PH1
            }
        }
    #endif
    }
#else
    unsigned long  ulReg;

    switch (portNr) {
    case _PORTA:
        while (i < PORT_WIDTH) {
            if (i < 8) {
                ulReg = GPIOA_CRL;
                ulReg >>= (i * 4);
            }
            else {
                ulReg = GPIOA_CRH;
                ulReg >>= ((i - 8) * 4);
            }
            switch (iRequest) {
            case GET_OUTPUTS:
                if (((ulReg & ALTERNATIVE_FUNCTION) == 0) && ((ulReg & OUTPUT_FAST) != 0)) { // mode set to output but not alternative peripheral output
                    usPeripherals |= usBit;
                }
                break;
            case GET_PERIPHERALS:                
                if (((ulReg & ALTERNATIVE_FUNCTION) != 0) && ((ulReg & OUTPUT_FAST) != 0)) { // mode set to peripheral output function
                    usPeripherals |= usBit;                              // peripheral output
                }
                else if ((ulReg & OUTPUT_FAST) == 0) {                   // input but could be attached to peripheral
                    switch (i) {
                    case 0:
                    case 1:
    #if defined _CONNECTIVITY_LINE
                        if (!(AFIO_MAPR & MII_RMII_SEL)) {               // MII
                            if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) { // EMAC is enabled so assume peripheral input
                                usPeripherals |= usBit;                  // peripheral input
                            }
                        }
                        else {                                           // RMII
                            if ((i == 1) && (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE))) { // {5} EMAC is enabled so assume peripheral input
                                usPeripherals |= usBit;                  // peripheral input
                            }
                        }
    #endif
                        break;
                    case 3:
                        if (((AFIO_MAPR & USART2_REMAPPED) == 0) && ((USART2_CR1 & USART_CR1_RE) != 0)) {
                            usPeripherals |= usBit;                      // peripheral input
                        }
                        else {
    #if defined _CONNECTIVITY_LINE
                            if ((AFIO_MAPR & MII_RMII_SEL) == 0) {       // MII
                                if ((ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) != 0) { // EMAC is enabled so assume peripheral input
                                    usPeripherals |= usBit;              // peripheral input
                                }
                            }
                            else {                                       // RMII
                            }
    #endif
                        }
                        break;
    #if defined _CONNECTIVITY_LINE                                       // {4}
                    case 7:
                        if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) { // {5} EMAC is enabled so assume peripheral input
                            usPeripherals |= usBit;                      // peripheral input
                        }
                        break;
                    case 9:
                        if (OTG_FS_GCCFG & OTG_FS_GCCFG_PWRDWN) {        // if USB PHY is enabled
                            if (OTG_FS_GCCFG & (OTG_FS_GCCFG_VBUSASEN | OTG_FS_GCCFG_VBUSBSEN)) { // bus monitoring enabled
                                usPeripherals |= usBit;                  // peripheral USB_VBUS
                            }
                        }
                        break;
    #endif
                    case 10:
                        if (((AFIO_MAPR & USART1_REMAPPED) == 0) && (USART1_CR1 & USART_CR1_RE)) { // if USART1 rx is enabled
                            usPeripherals |= usBit;                      // peripheral input
                        }
                        break;
    #if defined _CONNECTIVITY_LINE && defined USB_OTG_AVAILABLE          // {4}
                    case 11:
                    case 12:
                        if ((OTG_FS_GCCFG & OTG_FS_GCCFG_PWRDWN) != 0) { // if USB PHY is enabled
                            usPeripherals |= usBit;                      // peripheral USB_DM/USB_DP
                        }
                        break;
    #elif defined _PERFORMANCE_LINE && defined USB_DEVICE_AVAILABLE
                    case 11:
                    case 12:
                        if ((USB_CNTR & USB_CNTR_PDWN) == 0) {           // if USB PHY is connected
                            usPeripherals |= usBit;                      // peripheral USB_DM/USB_DP
                        }
                        break;
    #endif
                    }
                }
                break;
            }
            usBit <<= 1;
            i++;
        }
        break;
    case _PORTB:
        while (i < PORT_WIDTH) {
            if (i < 8) {
                ulReg = GPIOB_CRL;
                ulReg >>= (i * 4);
            }
            else {
                ulReg = GPIOB_CRH;
                ulReg >>= ((i - 8) * 4);
            }
            switch (iRequest) {
            case GET_OUTPUTS:
                if ((!(ulReg & ALTERNATIVE_FUNCTION)) && (ulReg & OUTPUT_FAST)) { // mode set to output but not alternative peripheral output
                    usPeripherals |= usBit;
                }
                break;
            case GET_PERIPHERALS:                
                if ((ulReg & ALTERNATIVE_FUNCTION) && (ulReg & OUTPUT_FAST)) { // mode set to peripheral output function
                    usPeripherals |= usBit;                              // peripheral output
                }
                else if (!(ulReg & OUTPUT_FAST)) {                       // input but could be attached to peripheral
                    switch (i) {
                    case 7:
                        if ((AFIO_MAPR & USART1_REMAPPED) && (USART1_CR1 & USART_CR1_RE)) { // if USART1 rx is enabled
                            usPeripherals |= usBit;                      // peripheral input
                        }
                        break;
    #if defined _CONNECTIVITY_LINE
                    case 10:
                        if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) { // EMAC is enabled so assume peripheral input
                            if (!(AFIO_MAPR & MII_RMII_SEL)) {           // {5} only in MII mode
                                usPeripherals |= usBit;                  // peripheral input
                            }
                        }
                        break;
    #endif
                    case 11:
    #if USARTS_AVAILABLE > 2
                        if ((!(AFIO_MAPR & USART3_FULLY_REMAPPED)) && ((USART3_CR1 & USART_CR1_RE) != 0)) { // if USART3 rx is enabled
                            usPeripherals |= usBit;                      // peripheral input
                        }
    #endif
                        break;
                    }
                }
                break;
            }
            usBit <<= 1;
            i++;
        }
        break;
    case _PORTC:
        while (i < PORT_WIDTH) {
            if (i < 8) {
                ulReg = GPIOC_CRL;
                ulReg >>= (i * 4);
            }
            else {
                ulReg = GPIOC_CRH;
                ulReg >>= ((i - 8) * 4);
            }
            switch (iRequest) {
            case GET_OUTPUTS:
                if ((!(ulReg & ALTERNATIVE_FUNCTION)) && (ulReg & OUTPUT_FAST)) { // mode set to output but not alternative peripheral output
                    usPeripherals |= usBit;
                }
                break;
            case GET_PERIPHERALS:                
                if ((ulReg & ALTERNATIVE_FUNCTION) && (ulReg & OUTPUT_FAST)) { // mode set to peripheral output function
                    usPeripherals |= usBit;                              // peripheral output
                }
                else if (!(ulReg & OUTPUT_FAST)) {                       // input but could be attached to peripheral
                    switch (i) {
     #if defined _CONNECTIVITY_LINE
                    case 4:                                              // {5}
                    case 5:                                              // {5}
                    case 3:
                        if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) { // EMAC is enabled so assume peripheral input
                            if ((!(AFIO_MAPR & MII_RMII_SEL)) || (i == 4) || (i == 5)) { // {5} ignore when RMII is used
                                usPeripherals |= usBit;                  // peripheral input
                            }
                        }
                        break;
    #endif
                    case 11:
    #if USARTS_AVAILABLE > 2
                        if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == USART3_PARTIALLY_REMAPPED) && ((USART3_CR1 & USART_CR1_RE) != 0)) {
                            usPeripherals |= usBit;                      // USART3_RX
                        }
                        else if ((SPI3_CR1 & SPICR1_SPE) != 0) {         // SPI3 is enabled so assume peripheral input
                            usPeripherals |= usBit;                      // peripheral input
                        }
    #endif
                        break;
                    }
                }
                break;
            }
            usBit <<= 1;
            i++;
        }
        break;
    case _PORTD:
        while (i < PORT_WIDTH) {
            if (i < 8) {
                ulReg = GPIOD_CRL;
                ulReg >>= (i * 4);
            }
            else {
                ulReg = GPIOD_CRH;
                ulReg >>= ((i - 8) * 4);
            }
            switch (iRequest) {
            case GET_OUTPUTS:
                if ((!(ulReg & ALTERNATIVE_FUNCTION)) && (ulReg & OUTPUT_FAST)) { // mode set to output but not alternative peripheral output
                    usPeripherals |= usBit;
                }
                break;
            case GET_PERIPHERALS:                
                if ((ulReg & ALTERNATIVE_FUNCTION) && (ulReg & OUTPUT_FAST)) { // mode set to peripheral output function
                    usPeripherals |= usBit;                              // peripheral output
                }
                else if ((ulReg & OUTPUT_FAST) == 0) {                   // input but could be attached to peripheral
                    switch (i) {
                    case 9:
    #if USARTS_AVAILABLE > 2
                        if (((AFIO_MAPR & USART3_FULLY_REMAPPED) == USART3_FULLY_REMAPPED) && ((USART3_CR1 & USART_CR1_RE) != 0)) {
                            usPeripherals |= usBit;                      // USART3_RX
                        }
        #if defined _CONNECTIVITY_LINE
                        else if (AFIO_MAPR & ETH_REMAP) {                // alternative mapping (EMAC PD8:PD9:PD10:PD11:PD12)
                            if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) { // EMAC is enabled so assume peripheral input
                                usPeripherals |= usBit;                  // peripheral input
                            }
                        }
        #endif
    #endif
                        break;
                    case 8:
                    case 10:
                    case 11:
                    case 12:
    #if defined _CONNECTIVITY_LINE
                        if (AFIO_MAPR & ETH_REMAP) {                     // alternative mapping (EMAC PD8:PD9:PD10:PD11:PD12)
                            if (ETH_MACCR & (ETH_MACCR_RE | ETH_MACCR_TE)) { // EMAC is enabled so assume peripheral input
                                if ((AFIO_MAPR & MII_RMII_SEL) && ((i == 11) || (i == 12))) { // {5} ignore in RMII mode
                                    break;
                                }
                                usPeripherals |= usBit;                  // peripheral input
                            }
                        }
    #endif
                        break;
                    case 6:
                        if (AFIO_MAPR & USART2_REMAPPED) {
                            if (USART2_CR1 & (USART_CR1_RE)) {               // if USART2 rx is enabled
                                usPeripherals |= usBit;                      // peripheral input
                            }
                        }
                        break;
                    }
                }
                break;
            }
            usBit <<= 1;
            i++;
        }
        break;
    case _PORTE:
        while (i < PORT_WIDTH) {
            if (i < 8) {
                ulReg = GPIOE_CRL;
                ulReg >>= (i * 4);
            }
            else {
                ulReg = GPIOE_CRH;
                ulReg >>= ((i - 8) * 4);
            }
            switch (iRequest) {
            case GET_OUTPUTS:
                if ((!(ulReg & ALTERNATIVE_FUNCTION)) && (ulReg & OUTPUT_FAST)) { // mode set to output but not alternative peripheral output
                    usPeripherals |= usBit;
                }
                break;
            case GET_PERIPHERALS:                
                if ((ulReg & ALTERNATIVE_FUNCTION) && (ulReg & OUTPUT_FAST)) { // mode set to peripheral output function
                    usPeripherals |= usBit;                              // peripheral output
                }
                else if (!(ulReg & OUTPUT_FAST)) {                       // input but could be attached to peripheral
                    switch (i) {
                    default:
                        break;
                    }
                }
                break;
            }
            usBit <<= 1;
            i++;
        }
        break;
    case _PORTF:
        while (i < PORT_WIDTH) {
            if (i < 8) {
                ulReg = GPIOF_CRL;
                ulReg >>= (i * 4);
            }
            else {
                ulReg = GPIOF_CRH;
                ulReg >>= ((i - 8) * 4);
            }
            switch (iRequest) {
            case GET_OUTPUTS:
                if ((!(ulReg & ALTERNATIVE_FUNCTION)) && (ulReg & OUTPUT_FAST)) { // mode set to output but not alternative peripheral output
                    usPeripherals |= usBit;
                }
                break;
            case GET_PERIPHERALS:                
                if ((ulReg & ALTERNATIVE_FUNCTION) && (ulReg & OUTPUT_FAST)) { // mode set to peripheral output function
                    usPeripherals |= usBit;                              // peripheral output
                }
                else if (!(ulReg & OUTPUT_FAST)) {                       // input but could be attached to peripheral
                    switch (i) {
                    default:
                        break;
                    }
                }
                break;
            }
            usBit <<= 1;
            i++;
        }
        break;
    case _PORTG:
        while (i < PORT_WIDTH) {
            if (i < 8) {
                ulReg = GPIOG_CRL;
                ulReg >>= (i * 4);
            }
            else {
                ulReg = GPIOG_CRH;
                ulReg >>= ((i - 8) * 4);
            }
            switch (iRequest) {
            case GET_OUTPUTS:
                if ((!(ulReg & ALTERNATIVE_FUNCTION)) && (ulReg & OUTPUT_FAST)) { // mode set to output but not alternative peripheral output
                    usPeripherals |= usBit;
                }
                break;
            case GET_PERIPHERALS:                
                if ((ulReg & ALTERNATIVE_FUNCTION) && (ulReg & OUTPUT_FAST)) { // mode set to peripheral output function
                    usPeripherals |= usBit;                              // peripheral output
                }
                else if (!(ulReg & OUTPUT_FAST)) {                       // input but could be attached to peripheral
                    switch (i) {
                    default:
                        break;
                    }
                }
                break;
            }
            usBit <<= 1;
            i++;
        }
        break;
    }
#endif
    return usPeripherals;
}


extern unsigned long fnGetPresentPortPeriph(int portNr)
{
    return (fnGetPortType((portNr - 1), GET_PERIPHERALS, 0));
}

extern unsigned long fnGetPresentPortDir(int portNr)
{
    return (fnGetPortType((portNr - 1), GET_OUTPUTS, 0));
}


extern void fnSimulateLinkUp(void)
{
#if defined ETH_INTERFACE
    #if defined PHY_INTERRUPT
    unsigned long usBit = PHY_INTERRUPT;
    unsigned char ucPortBit = 0;
    while ((usBit & 0x8000) == 0) {
        ucPortBit++;
        usBit <<= 1;
    }
    fnSimulateInputChange(PHY_INT_PORT, ucPortBit, CLEAR_INPUT);         // simulate a falling edge on the PHY interrupt line
    #endif
    fnUpdateIPConfig();                                                  // update display in simulator
#endif
}


#if defined USB_INTERFACE
static unsigned char ucTxBuffer[NUMBER_OF_USB_ENDPOINTS] = {0};          // monitor the controller's transmission buffer use
static unsigned char ucRxBank[NUMBER_OF_USB_ENDPOINTS] = {0};            // monitor the buffer to inject to

// Handle data sent by USB
//
extern void fnSimUSB(int iType, int iEndpoint, USB_HW *ptrUSB_HW)
{
    extern void fnChangeUSBState(int iNewState);
    switch (iType) {
    case USB_SIM_RESET:
        {
            int x;
            fnChangeUSBState(0);
            for (x = 0; x < NUMBER_OF_USB_ENDPOINTS; x++) {
                fnLogUSB(x, 0, 0, (unsigned char *)0xffffffff, 0);       // log reset on each endpoint
            }
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
#if defined USB_DEVICE_AVAILABLE
        if (iEndpoint != 0) {
            iInts |= USB_INT;                                            // flag that the interrupt should be handled
            ulEndpointInt |= (1 << iEndpoint);                           // flag endpoint
        }
#endif
        break;
    case USB_SIM_ENUMERATED:                                             // flag that we have completed emumeration
        fnChangeUSBState(1);
        break;
    case USB_SIM_STALL:
        fnLogUSB(iEndpoint, 0, 1, (unsigned char *)0xffffffff, 0);       // log stall
        break;
    case USB_SIM_SUSPEND:
        fnChangeUSBState(0);
        break;
    }
}

#if defined USB_DEVICE_AVAILABLE
// The SRAM is organised as long words but only half-words are actually used and the unused half-words are skipped
//
static void fnWriteUSB_data(unsigned long *ptrInputBuffer, unsigned char *ptrDebugIn, unsigned short usLength)
{
    unsigned long ulNextEntry = 0;
    while (usLength != 0) {
        ulNextEntry = *ptrDebugIn++;
        if (usLength >= 2) {
            ulNextEntry |= (*ptrDebugIn++ << 8);
            usLength -= 2;
        }
        else {
            usLength--;
        }
        *ptrInputBuffer++ = ulNextEntry;
    }
}

static void fnReadUSB_data(unsigned long *ptrUSB_in, unsigned char *ptrOutputBuffer, unsigned short usLength)
{
    unsigned long ulNextEntry = 0;
    while (usLength != 0) {
        ulNextEntry = *ptrUSB_in++;
        *ptrOutputBuffer++ = (unsigned char)ulNextEntry;
        if (usLength >= 2) {
            *ptrOutputBuffer++ = (unsigned char)(ulNextEntry >> 8);
            usLength -= 2;
        }
        else {
            usLength--;
        }
    }
}

static void fnUSB_device_transfer_interrupt(int iEndpoint)
{
    USB_ISTR &= ~(USB_ISTR_EP_ID_MASK);
    USB_ISTR |= (iEndpoint);
    if (((USB_CNTR & USB_ISTR) & (USB_CNTR_CTRM)) != 0) {                // if correct transfer interrupt enabled and interrupt source pending
        if (0 == 1) {                                                    // isochronous or double-buffered endpoint
            if (fnGenInt(irq_USB_HP_CAN_TX_ID) != 0) {                   // if high priority USB interrupt is not disabled in core
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_USB_HP_CAN1_TX();      // call the high priority interrupt handler
            }
        }
        else {
            if (fnGenInt(irq_USB_LP_CAN_RX0_ID) != 0) {                  // if low priority USB interrupt is not disabled in core
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_USB_LP_CAN1_RX0();     // call the low priority interrupt handler
            }
        }
    }
}

static unsigned short fnExtractEndpointLength(unsigned short usUSB_COUNT_RX)
{
    unsigned short usMaxRx = ((usUSB_COUNT_RX & USB_COUNT_RX_NUM_BLOCK_MASK) >> USB_COUNT_SHIFT);
    if ((usUSB_COUNT_RX & USB_COUNT_RX_BL_SIZE) != 0) {
        usMaxRx *= 32;
        usMaxRx += 32;
    }
    else {
        usMaxRx *= 2;
    }
    return (usMaxRx);
}
#endif

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
    int iReset = 0;
#if defined USB_DEVICE_AVAILABLE
    if (ptrDebugIn == 0) {
        if ((usLenEvent & USB_RESET_CMD) != 0) {                         // usb reset
            memset(ucRxBank, 0, sizeof(ucRxBank));                       // default is even bank
            memset(ucTxBuffer, 0, sizeof(ucTxBuffer));                   // default is even buffer
            USB_ISTR = USB_ISTR_RESET;                                   // set reset detected interrupt flag
            USB_EP0R = USB_EP1R = USB_EP2R = USB_EP3R = USB_EP4R = USB_EP5R = USB_EP6R = USB_EP7R = 0;
            iReset = 1;
        }
        if ((usLenEvent & USB_SLEEP_CMD) != 0) {                         // usb suspend
            USB_ISTR |= USB_ISTR_SUSP;                                   // set suspend detected interrupt flag
        }
        if ((usLenEvent & USB_RESUME_CMD) != 0) {
            USB_ISTR |= USB_ISTR_WKUP;                                   // set resume detected interrupt flag
        }
        if ((usLenEvent & USB_IN_SUCCESS) != 0) {
        }
        if (((USB_CNTR & USB_ISTR) & (USB_ISTR_RESET | USB_ISTR_SUSP | USB_ISTR_WKUP)) != 0) { // if low priority interrupt enabled and interrupt source pending
            if (fnGenInt(irq_USB_LP_CAN_RX0_ID) != 0) {                      // if low priority USB interrupt is not disabled in core
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_USB_LP_CAN1_RX0();         // call the low priority interrupt handler
                if (iReset != 0) {
                }
            }
        }
    }
    else {
        volatile unsigned long *ptrEndpointControl = USB_EP0R_ADD;
        USB_BD_TABLE *ptrBD = (USB_BD_TABLE *)(USB_CAN_SRAM_ADDR + USB_BTABLE); // the start of the buffer descripter table
        if ((USB_DADDR & USB_DADDR_EF) == 0) {                           // if the function is disabled
            return 1;
        }
        if ((USB_DADDR & USB_DADDR_ADD_MASK) != (unsigned long)iDevice) { // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        ptrEndpointControl += iEndPoint;
        ptrBD += iEndPoint;
        switch (ucPID) {
        case OUT_PID:
        case SETUP_PID:
            {
                unsigned short usMaxRx = fnExtractEndpointLength(ptrBD->usUSB_COUNT_RX_0);
                volatile unsigned long *ptrInputBuffer = (USB_CAN_SRAM_ADDR + USB_BTABLE);
                ptrInputBuffer += (ptrBD->usUSB_ADDR_RX/2);
                if (usLenEvent > usMaxRx) {
                    usLenEvent = usMaxRx;
                }
                fnWriteUSB_data((unsigned long *)ptrInputBuffer, ptrDebugIn, usLenEvent);
                ptrBD->usUSB_COUNT_RX_0 &= ~(USB_COUNT_COUNT_MASK);
                ptrBD->usUSB_COUNT_RX_0 |= (usLenEvent & USB_COUNT_COUNT_MASK);
                ucTxBuffer[iEndPoint] = 0;
                if ((*ptrEndpointControl & USB_EPR_CTR_STAT_RX_MASK) == USB_EPR_CTR_STAT_RX_VALID) {
                    if (ucPID == SETUP_PID) {
                        *ptrEndpointControl |= (USB_EPR_CTR_CTR_RX | USB_EPR_CTR_SETUP | USB_EPR_CTR_DTOG_TX);
                    }
                    else {
                        *ptrEndpointControl &= ~(USB_EPR_CTR_SETUP);
                        *ptrEndpointControl |= (USB_EPR_CTR_CTR_RX);
                    }
                    USB_ISTR |= (USB_CNTR_CTRM | USB_ISTR_DIR);
                }
            }
            break;
        default:
            _EXCEPTION("Unknown PID!");
            return 0;
        }
        fnUSB_device_transfer_interrupt(iEndPoint);
    }
#elif defined USB_OTG_AVAILABLE
    if (ptrDebugIn == 0) {
        if ((usLenEvent & USB_RESET_CMD) != 0) {                         // usb reset
            memset(ucRxBank,   0, sizeof(ucRxBank));                     // default is even bank
            memset(ucTxBuffer, 0, sizeof(ucTxBuffer));                   // default is even buffer
            OTG_FS_GINTSTS = (OTG_FS_GINTSTS_PTXFE | 0x00800000 | OTG_FS_GINTSTS_USBRST | OTG_FS_GINTSTS_USBSUSP | OTG_FS_GINTSTS_ESUSP | OTG_FS_GINTSTS_NPTXFE); // set reset detected interrupt flag
            iReset = 1;
        }
        if ((usLenEvent & USB_SLEEP_CMD) != 0) {                         // usb suspend
            OTG_FS_GINTSTS |= OTG_FS_GINTSTS_USBSUSP;                    // set suspend detected interrupt flag
        }
        if ((usLenEvent & USB_RESUME_CMD) != 0) {
            OTG_FS_GINTSTS |= OTG_FS_GINTSTS_WKUINT;                     // set resume detected interrupt flag
        }
        if ((usLenEvent & USB_IN_SUCCESS) != 0) {
        }
    }
    else {
        if (((OTG_FS_DCFG & OTG_FS_DCFG_DAD_MASK) >> 4) != (unsigned long)iDevice) { // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        OTG_FS_GRXSTSR = ((iEndPoint & OTG_FS_GRXSTSR_EPNUM) | (usLenEvent << OTG_FS_GRXSTSR_BCNT_SHIFT)); // endpoint and length
        switch (ucPID) {
        case SETUP_PID:
            {
                volatile unsigned long *ptrFIFO = OTG_FS_DFIFO0_ADDR;
                OTG_FS_GRXSTSR |= (OTG_FS_GRXSTSR_PKTSTS_SETUP_RX | OTG_FS_GRXSTSR_DPID_DATA0); // add PID type
                ucTxBuffer[iEndPoint] = 0;
                OTG_FS_GINTSTS |= OTG_FS_GINTSTS_RXFLVL;                 // set interrupt flag
                while (usLenEvent) {
                    *ptrFIFO++ = (*ptrDebugIn | (*(ptrDebugIn + 1) << 8) | (*(ptrDebugIn + 2) << 16) | (*(ptrDebugIn + 3) << 24));
                    if (usLenEvent <= 4) {
                        break;
                    }
                    ptrDebugIn += 4;                                     // {}
                    usLenEvent -= 4;
                }
            }
            break;
        case OUT_PID:
            OTG_FS_GRXSTSR |= (OTG_FS_GRXSTSR_PKTSTS_OUT_RX);            // add PID type
            ucTxBuffer[iEndPoint] ^= 1;                                  // data toggle
            if (ucTxBuffer[iEndPoint] != 0) {
                OTG_FS_GRXSTSR |= OTG_FS_GRXSTSR_DPID_DATA1;
            }
            OTG_FS_GINTSTS |= OTG_FS_GINTSTS_RXFLVL;                     // set interrupt flag
            if (usLenEvent != 0) {
                volatile unsigned long *ptrFIFO = OTG_FS_DFIFO0_ADDR;
                while (usLenEvent) {
                    *ptrFIFO++ = (*ptrDebugIn | (*(ptrDebugIn + 1) << 8) | (*(ptrDebugIn + 2) << 16) | (*(ptrDebugIn + 3) << 24));
                    if (usLenEvent <= 4) {
                        break;
                    }
                    ptrDebugIn += 4;                                     // {}
                    usLenEvent -= 4;
                }
            }
            break;
        default:
            _EXCEPTION("Unknown PID!");
            return 0;
        }
        OTG_FS_GRXSTSP = OTG_FS_GRXSTSR;
    }
    if (OTG_FS_GINTSTS & OTG_FS_GINTMSK) {                               // if interrupt enabled
        if (OTG_FS_GAHBCFG & OTG_FS_GAHBCFG_GINTMSK) {                   // if global USB interrupts enabled
            if (IRQ64_95_SER & (1 << (irq_OTG_FS_ID - 64))) {            // if USB interrupt is not disabled in core
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_OTG_FS();              // call the interrupt handler
                if (iReset != 0) {                                       // reset is followed by ENUM if interrupt is enabled
                    OTG_FS_GINTSTS |= OTG_FS_GINTSTS_ENUMDNE;
                    if (OTG_FS_GINTMSK & OTG_FS_GINTMSK_ENUMDNEM) {
                        ptrVect->processor_interrupts.irq_OTG_FS();      // call the interrupt handler
                    }
                }
                else if (ucPID == SETUP_PID) {                           // after passing the SETUP data there is a following interrupt
                    OTG_FS_GRXSTSR &= ~(OTG_FS_GRXSTSR_BCNT_MASK | OTG_FS_GRXSTSR_PKTSTS_MASK);
                    OTG_FS_GRXSTSR |= OTG_FS_GRXSTSR_PKTSTS_SETUP_OK;    // transaction completed
                    OTG_FS_GRXSTSP  = OTG_FS_GRXSTSR;
                    OTG_FS_GINTSTS |= OTG_FS_GINTSTS_RXFLVL;
                    if (OTG_FS_GINTMSK & OTG_FS_GINTMSK_RXFLVLM) {
                        ptrVect->processor_interrupts.irq_OTG_FS();      // call the interrupt handler
                        ptrVect->processor_interrupts.irq_OTG_FS();      // the USB controller tends to generate a spurious interrupt here
                        OTG_FS_GINTSTS |= OTG_FS_GINTSTS_IEPINT;
                        OTG_FS_DIEPINT0 |= OTG_FS_DIEPINT_XFRC;
                        if (OTG_FS_GINTMSK & OTG_FS_GINTMSK_IEPINT) {
                            ptrVect->processor_interrupts.irq_OTG_FS();  // IN transfer complete
                        }
                    }
                }
                else if (ucPID == OUT_PID) {
                    ptrVect->processor_interrupts.irq_OTG_FS();          // the USB controller tends to generate a spurious interrupt here
                }
            }
        }
    }
#endif
    return 0;
}

// Check whether data has been prepared for transmission
//
extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
#if defined USB_DEVICE_AVAILABLE
    volatile unsigned long *ptrEndpointControl = USB_EP0R_ADD;
    unsigned long *ptrUSB_data;
    USB_BD_TABLE *ptrBD = (USB_BD_TABLE *)(USB_CAN_SRAM_ADDR + USB_BTABLE); // the start of the buffer descripter table
    unsigned char ucTxBufferData[1024];
    ptrEndpointControl += iEndpoint;
    ptrBD += iEndpoint;
    FOREVER_LOOP() {                                                     // allow handling multiple transmissions
        switch (*ptrEndpointControl & USB_EPR_CTR_STAT_TX_MASK) {
        case USB_EPR_CTR_STAT_TX_VALID:                                  // transmission is set up
            ptrUSB_data = (unsigned long *)(USB_CAN_SRAM_ADDR + USB_BTABLE);
            ptrUSB_data += (ptrBD->usUSB_ADDR_TX / 2);
            fnReadUSB_data(ptrUSB_data, ucTxBufferData, ptrBD->usUSB_COUNT_TX_0);
            fnLogUSB(iEndpoint, 0, ptrBD->usUSB_COUNT_TX_0, ucTxBufferData, 1 /* data toggle needs to be monitored */);
            *ptrEndpointControl &= ~(USB_EPR_CTR_STAT_TX_MASK);          // disable transmitter state
            *ptrEndpointControl |= (USB_EPR_CTR_CTR_TX | USB_EPR_CTR_STAT_TX_NAK);
            USB_ISTR &= ~(USB_ISTR_DIR);
            USB_ISTR |= (USB_CNTR_CTRM);
            fnUSB_device_transfer_interrupt(iEndpoint);
            break;
        case USB_EPR_CTR_STAT_TX_STALL:
        default:
            return;
        }
    }
#elif defined USB_OTG_AVAILABLE
    unsigned short usUSBLength;
    switch (iEndpoint) {
    case 0:
        if ((OTG_FS_DIEPCTL0 & OTG_FS_DIEPCTL_EPENA) != 0) {
            usUSBLength = (unsigned short)(OTG_FS_DIEPTSIZ0 & OTG_FS_DIEPTSIZ_XFRSIZ_MASK); // length of data in FIFO
            fnLogUSB(iEndpoint, 0, usUSBLength, (unsigned char *)OTG_FS_DFIFO0_ADDR, 1 /* data toggle needs to be monitored */);
            OTG_FS_DIEPCTL0 &= ~(OTG_FS_DIEPCTL_EPENA);                  // disable the endpoint after transmission
        }
        break;
    case 1:
        if ((OTG_FS_DIEPCTL1 & OTG_FS_DIEPCTL_EPENA) != 0) {
            usUSBLength = (unsigned short)(OTG_FS_DIEPTSIZ1 & OTG_FS_DIEPTSIZ_XFRSIZ_MASK); // length of data in FIFO
            fnLogUSB(iEndpoint, 0, usUSBLength, (unsigned char *)OTG_FS_DFIFO1_ADDR, 1 /* data toggle needs to be monitored */);
            OTG_FS_DIEPCTL1 &= ~(OTG_FS_DIEPCTL_EPENA);                  // disable the endpoint after transmission
        }
        break;
    case 2:
        if ((OTG_FS_DIEPCTL2 & OTG_FS_DIEPCTL_EPENA) != 0) {
            usUSBLength = (unsigned short)(OTG_FS_DIEPTSIZ2 & OTG_FS_DIEPTSIZ_XFRSIZ_MASK); // length of data in FIFO
            fnLogUSB(iEndpoint, 0, usUSBLength, (unsigned char *)OTG_FS_DFIFO2_ADDR, 1 /* data toggle needs to be monitored */);
            OTG_FS_DIEPCTL2 &= ~(OTG_FS_DIEPCTL_EPENA);                  // disable the endpoint after transmission
        }
        break;
    case 3:
        if ((OTG_FS_DIEPCTL3 & OTG_FS_DIEPCTL_EPENA) != 0) {
            usUSBLength = (unsigned short)(OTG_FS_DIEPTSIZ3 & OTG_FS_DIEPTSIZ_XFRSIZ_MASK); // length of data in FIFO
            fnLogUSB(iEndpoint, 0, usUSBLength, (unsigned char *)OTG_FS_DFIFO3_ADDR, 1 /* data toggle needs to be monitored */);
            OTG_FS_DIEPCTL3 &= ~(OTG_FS_DIEPCTL_EPENA);                  // disable the endpoint after transmission
        }
        break;
    }
#endif
}

// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    unsigned short usLength = 0;
    #if defined USB_DEVICE_AVAILABLE
    volatile unsigned long *ptrEndpointControl = USB_EP0R_ADD;
    USB_BD_TABLE *ptrBD = (USB_BD_TABLE *)(USB_CAN_SRAM_ADDR + USB_BTABLE); // the start of the buffer descripter table
    ptrEndpointControl += iEndpoint;
    ptrBD += iEndpoint;
    usLength = fnExtractEndpointLength(ptrBD->usUSB_COUNT_RX_0);
    #elif defined USB_OTG_AVAILABLE
    switch (iEndpoint) {
    case 0:
        usLength = (unsigned short)(OTG_FS_DIEPTXF0 >> 16);              // length in words
        break;
    case 1:
        usLength = (unsigned short)(OTG_FS_DIEPTXF1 >> 16);              // length in words
        break;
    case 2:
        usLength = (unsigned short)(OTG_FS_DIEPTXF2 >> 16);              // length in words
        break;
    case 3:
        usLength = (unsigned short)(OTG_FS_DIEPTXF3 >> 16);              // length in words
        break;
    }
    usLength *= 4;                                                       // length in bytes
    #endif
    return usLength;
}
#endif

#if !defined DEVICE_WITHOUT_DMA
static int fnSimulateDMA(unsigned char ucChannel, int iDMA)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    switch (iDMA) {
    case 1:
        break;
    #if defined DMA2_S0CR
    case 2:
        // Only channel 0 support at present!!!
        //
        if (((DMA2_S0CR >> 25) & 0x7) == ucChannel) {                    // if the channel matches the DMA setting
            DMA2_S0NDTR &= 0x0000ffff;
            if (((DMA2_S0CR & DMA_SxCR_EN) != 0) && (DMA2_S0NDTR !=  0)) { // if the channel is enabled and has a count value
                unsigned long ulData;
                switch (DMA2_S0CR & (DMA_SxCR_DIR_M2P | DMA_SxCR_DIR_M2M)) {
                case 0:                                                  // peripheral to memory
                    switch (DMA2_S0CR & (DMA_SxCR_PSIZE_32 | DMA_SxCR_PSIZE_16)) {
                    case 0:
                        ulData = *(unsigned char *)DMA2_S0PAR;           // read the byte peripheral data
                        if ((DMA2_S0CR & DMA_SxCR_PINC) != 0) {
                            DMA2_S0PAR = (DMA2_S0PAR + 1);
                        }
                        break;
                    case DMA_SxCR_PSIZE_16:
                        ulData = *(unsigned short *)DMA2_S0PAR;          // read the short word peripheral data
                        if ((DMA2_S0CR & DMA_SxCR_PINC) != 0) {
                            DMA2_S0PAR = (DMA2_S0PAR + 2);
                        }
                        break;
                    case DMA_SxCR_PSIZE_32:
                        ulData = *(unsigned long *)DMA2_S0PAR;           // read the long word peripheral data
                        if ((DMA2_S0CR & DMA_SxCR_PINC) != 0) {
                            DMA2_S0PAR = (DMA2_S0PAR + 4);
                        }
                        break;
                    }
                    switch (DMA2_S0CR & (DMA_SxCR_MSIZE_32 | DMA_SxCR_MSIZE_16)) {
                    case 0:
                        *(unsigned char *)DMA2_S0M0AR = (unsigned char)ulData; // write the byte to memory
                        if ((DMA2_S0CR & DMA_SxCR_MINC) != 0) {
                            DMA2_S0M0AR = (DMA2_S0M0AR + 1);
                        }
                        break;
                    case DMA_SxCR_MSIZE_16:
                        *(unsigned short *)DMA2_S0M0AR = (unsigned short)ulData; // write the short word to memory
                        if ((DMA2_S0CR & DMA_SxCR_MINC) != 0) {
                            DMA2_S0M0AR = (DMA2_S0M0AR + 2);
                        }
                        break;
                    case DMA_SxCR_MSIZE_32:
                        *(unsigned long *)DMA2_S0M0AR = ulData;          // write the long word to memory
                        if ((DMA2_S0CR & DMA_SxCR_MINC) != 0) {
                            DMA2_S0M0AR = (DMA2_S0M0AR + 4);
                        }
                        break;
                    }
                    break;
                case DMA_SxCR_DIR_M2P:                                   // memory to peripheral transfer
                case DMA_SxCR_DIR_M2M:                                   // memory to memory transfer
                    _EXCEPTION("To do");
                    break;
                }
                DMA2_S0NDTR = (DMA2_S0NDTR - 1);
                if (DMA2_S0NDTR == 0) {                                  // completed
                    DMA1_LISR |= (DMA_LISR_HTIF0 | DMA_LISR_TCIF0);      // set flags to indicate half and full transfer completed
                    if ((DMA2_S0CR & DMA_SxCR_TCIE) != 0) {              // if interrupt on full-buffer is enabled
                        if ((IRQ32_63_SER & (1 << (irq_DMA2_Channel1_ID - 32))) != 0) { // if DMA stream interrupt is not disabled
                            ptrVect->processor_interrupts.irq_DMA2_Stream0(); // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    }
    return -1;                                                           // no operation
}

static void fnHandleDMA_triggers(unsigned char ucChannel, int iDMA)
{
    switch (iDMA) {
    case 1:
        break;
    #if defined DMA2_S0CR
    case 2:
        if (((DMA2_S0CR >> 25) & 0x7) == ucChannel) {                    // if the channel matches the DMA setting
            fnSimulateDMA(ucChannel, iDMA);
        }
        break;
    #endif
    }
}
#endif

#if defined SUPPORT_ADC                                                  // {10}
static unsigned short fnConvertSimADCvalue(STM32_ADC_REGS *ptrADC, unsigned short usStandardValue)
{
    switch (ptrADC->ADC_CR1 & ADC_CR1_RES_6_BIT) {
    case ADC_CR1_RES_12_BIT:                                             // conversion mode - single-ended 12 bit
        if ((ptrADC->ADC_CR2 & ADC_CR2_ALIGN_LEFT) == 0) {
            usStandardValue >>= 4;
        }
        else {
            usStandardValue &= 0xfff0;
        }
        break;
    case ADC_CR1_RES_10_BIT:                                             // conversion mode - single-ended 10 bit
        if ((ptrADC->ADC_CR2 & ADC_CR2_ALIGN_LEFT) == 0) {
            usStandardValue >>= 6;
        }
        else {
            usStandardValue &= 0xffc0;
        }
        break;
    case ADC_CR1_RES_8_BIT:                                              // conversion mode - single-ended 8 bit
        if ((ptrADC->ADC_CR2 & ADC_CR2_ALIGN_LEFT) == 0) {
            usStandardValue >>= 8;
        }
        else {
            usStandardValue &= 0xff00;
        }
        break;
    case ADC_CR1_RES_6_BIT:                                              // conversion mode - single-ended 6 bit
        if ((ptrADC->ADC_CR2 & ADC_CR2_ALIGN_LEFT) == 0) {
            usStandardValue >>= 10;
        }
        else {
            usStandardValue &= 0xfc00;
        }
        break;
    }
    return usStandardValue;
}

static void fnSimADC(int iChannel)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    unsigned long ulSample = 0;
    STM32_ADC_REGS *ptrADC;
    unsigned long ulValue = 0;
    unsigned long ulChannel;
    unsigned short usADCvalue;
    if (iChannel == 1) {
        ptrADC = (STM32_ADC_REGS *)ADC_BLOCK;
    }
    #if ADC_CONTROLLERS > 1
    else if (iChannel == 2) {
        ptrADC = (STM32_ADC_REGS *)(ADC_BLOCK + 0x100);
    }
    #endif
    #if ADC_CONTROLLERS > 2
    else if (iChannel == 3) {
        ptrADC = (STM32_ADC_REGS *)(ADC_BLOCK + 0x200);
    }
    #endif
    else {
        return;
    }
    if ((ptrADC->ADC_CR1 & ADC_CR1_SCAN) != 0) {                         // if scan mode is enabled
        ulValue = ((ptrADC->ADC_SQR1 & ADC_SQR1_L_MASK) >> ADC_SQR1_L_SHIFT); // the sequence length
        while (ulSample <= ulValue) {
            if (ulSample >= 12) {
                ulChannel = (ptrADC->ADC_SQR1 >> ((ulSample - 12) * 5));
            }
            else if (ulSample >= 6) {
                ulChannel = (ptrADC->ADC_SQR2 >> ((ulSample - 6) * 5));
            }
            else {
                ulChannel = (ptrADC->ADC_SQR3 >> (ulSample * 5));
            }
            ulChannel &= 0x1f;                                           // the channel being sampled
            ulSample++;
            if ((ptrADC->ADC_CR1 & ADC_CR1_AWDEN) != 0) {                // analog watchog mode enabled
                if (((ptrADC->ADC_CR1 & ADC_CR1_AWDSGL_SINGLE) == 0) || ((ptrADC->ADC_CR1 & ADC_CR1_AWDCH_MASK) == ulChannel)) { // if on all channels or this particular one only
                    if ((usADC_values[ulChannel] > ptrADC->ADC_HTR) || (usADC_values[ulChannel] < ptrADC->ADC_LTR)) { // check whether the sample value triggers the analog watchog
                        ptrADC->ADC_SR |= ADC_SR_AWD;
                        ADC_CSR |= (ADC_CSR_AWD1 << ((iChannel - 1) * 8));
                        if ((ptrADC->ADC_CR1 & ADC_CR1_AWDIE) != 0) {    // if the analog watchog interrupt is enabled
                            if ((IRQ0_31_SER & (1 << irq_ADC_ID)) != 0) {// if ADC interrupt is not disabled
                                ptrVect->processor_interrupts.irq_ADC1_2_3(); // call the interrupt handler
                            }
                        }
                    }
                }
            }
            usADCvalue = fnConvertSimADCvalue(ptrADC, usADC_values[ulChannel]); // convert the standard value to the format used by the present mode
            ptrADC->ADC_DR = usADCvalue;                                 // put the result into the regular data register
            if ((ptrADC->ADC_CR2 & ADC_CR2_DMA) != 0) {                  // if in DMA mode
    #if !defined DEVICE_WITHOUT_DMA
                fnHandleDMA_triggers(0, 2);                              // process the trigger
    #endif
            }
            else if ((ptrADC->ADC_CR2 & ADC_CR2_EOCS_CONVERSION) != 0) { // if the EOC is set after each individual conversion
                ptrADC->ADC_SR |= ADC_SR_EOC;
                ADC_CSR |= (ADC_CSR_EOC1 << ((iChannel - 1) * 8));
                if ((ptrADC->ADC_CR1 & ADC_CR1_EOCIE) != 0) {            // if the end of conversion interrupt is enabled
                    if ((IRQ0_31_SER & (1 << irq_ADC_ID)) != 0) {        // if ADC interrupt is not disabled
                        ptrVect->processor_interrupts.irq_ADC1_2_3();    // call the interrupt handler
                    }
                }
            }
        }
    }
    else {
        _EXCEPTION("To do");
    }
    if (((ptrADC->ADC_CR2 & ADC_CR2_EOCS_CONVERSION) == 0) && ((ptrADC->ADC_CR2 & ADC_CR2_DMA) == 0)) { // if the EOC is set after sequence conversion (not DMA mode)
        ptrADC->ADC_SR |= ADC_SR_EOC;
        ADC_CSR |= (ADC_CSR_EOC1 << ((iChannel - 1) * 8));
        if ((ptrADC->ADC_CR1 & ADC_CR1_EOCIE) != 0) {                    // if the end of conversion interrupt is enabled
            if ((IRQ0_31_SER & (1 << irq_ADC_ID)) != 0) {                // if ADC interrupt is not disabled
                ptrVect->processor_interrupts.irq_ADC1_2_3();            // call the interrupt handler
            }
        }
    }
    if ((ptrADC->ADC_CR2 & ADC_CR2_CONT) != 0) {
        ptrADC->ADC_CR2 |= ADC_CR2_SWSTART;                              // enable next conversion (this may not be accurate simulation but will keep it running)
    }
}

static void fnTriggerADC(int iADC, int iHW_trigger)
{
    switch (iADC) {
    case 1:                                                              // ADC1
        if ((IS_POWERED_UP(APB2, RCC_APB2ENR_ADC1EN)) && ((ADC1_CR2 & ADC_CR2_ADON) != 0)) { // ADC1 powered up and operating
            if ((ADC1_CR2 & ADC_CR2_SWSTART) != 0) {                     // software trigger 
                ADC1_CR2 &= ~(ADC_CR2_SWSTART);                          // clear the start flag
                fnSimADC(1);                                             // perform ADC conversion
            }
        }
        break;
    #if ADC_CONTROLLERS > 1
    case 2:                                                              // ADC2
        if ((IS_POWERED_UP(APB2, RCC_APB2ENR_ADC2EN)) && ((ADC2_CR2 & ADC_CR2_ADON) != 0)) { // ADC2 powered up and operating
            if ((ADC2_CR2 & ADC_CR2_SWSTART) != 0) {                     // software trigger
                ADC2_CR2 &= ~(ADC_CR2_SWSTART);                          // clear the start flag
                fnSimADC(2);                                             // perform ADC conversion
            }
        }
        break;
    #endif
    #if ADC_CONTROLLERS > 2
    case 3:                                                              // ADC3
        if ((IS_POWERED_UP(APB2, RCC_APB2ENR_ADC3EN)) && ((ADC3_CR2 & ADC_CR2_ADON) != 0)) { // ADC3 powered up and operating
            if ((ADC3_CR2 & ADC_CR2_SWSTART) != 0) {                     // software trigger
                ADC3_CR2 &= ~(ADC_CR2_SWSTART);                          // clear the start flag
                fnSimADC(3);                                             // perform ADC conversion
            }
        }
        break;
    #endif
    }
}
#endif



// We can simulate timers during a tick here if required
//
extern int fnSimTimers(void)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    if (APPLICATION_INT_RESET_CTR_REG == (VECTKEY | SYSRESETREQ)) {      // processor reset was commanded
        return RESET_SIM_CARD;
    }
#if defined CAN_INTERFACE
    usCAN_time++;                                                        // increment CAN timestamp
    #if defined SIM_KOMODO
    fnSimCAN(0, 0, CAN_SIM_CHECK_RX);                                    // poll the CAN interface at the tick rate
    #endif
#endif
    if ((SYSTICK_CSR & SYSTICK_ENABLE) != 0) {                           // SysTick is enabled
        unsigned long ulTickCount = 0;
        if ((SYSTICK_CSR & SYSTICK_CORE_CLOCK) != 0) {
            ulTickCount = (unsigned long)((unsigned long long)((unsigned long long)TICK_RESOLUTION * (unsigned long long)HCLK)/1000000); // count per tick period from internal clock
        }
        else {
            ulTickCount = (unsigned long)((unsigned long long)((unsigned long long)TICK_RESOLUTION * (unsigned long long)HCLK)/8000000); // count per tick period from internal clock divided by 8
        }
        if (ulTickCount < SYSTICK_CURRENT) {
            SYSTICK_CURRENT -= ulTickCount;
        }
        else {
            SYSTICK_CURRENT = SYSTICK_RELOAD;
            if ((SYSTICK_CSR & SYSTICK_TICKINT) != 0) {                  // if interrupt enabled
                INT_CONT_STATE_REG |= PENDSTSET;
                if ((STM32.CORTEX_M3_REGS.ulPRIMASK & INTERRUPT_MASKED) == 0) { // if interrupt has been enabled, call interrupt handler
                    ptrVect->ptrSysTick();
                }
            }
        }
    }

    if (IWDG_KR != 0) {                                                  // watchdog is possibly operating
        static unsigned short usWatchdog = 0xfff;
        static int iWatchdogActive = 0;
        if (IWDG_KR == IWDG_KR_RETRIGGER) {                              // watchdog has been retriggered
            if (iWatchdogActive != 0) {
                IWDG_KR = 0x1234;                                        // mark as operational but not retriggered
                usWatchdog = (unsigned short)IWDG_RLR;                   // reload the count value            
            }
        }
        else {
            unsigned long ulWD_count;                                    // count per tick interval
            if (IWDG_KR == IWDG_KR_START) {
                usWatchdog = (unsigned short)IWDG_RLR;
                iWatchdogActive = 1;
            }
            switch (IWDG_PR) {
            case IWDG_PR_PRESCALE_4:
                ulWD_count = ((40 * TICK_RESOLUTION)/4000);
                break;
            case IWDG_PR_PRESCALE_8:
                ulWD_count = ((40000000/8)/TICK_RESOLUTION);
                break;
            case IWDG_PR_PRESCALE_16:
                ulWD_count = ((40000000/16)/TICK_RESOLUTION);
                break;
            case IWDG_PR_PRESCALE_32:
                ulWD_count = ((40 * TICK_RESOLUTION)/32000);
                break;
            case IWDG_PR_PRESCALE_64:
                ulWD_count = ((40000000/64)/TICK_RESOLUTION);
                break;
            case IWDG_PR_PRESCALE_128:
                ulWD_count = ((40000000/128)/TICK_RESOLUTION);
                break;
            case IWDG_PR_PRESCALE_256:
            default:
                ulWD_count = ((40000000/256)/TICK_RESOLUTION);
                break;
            }
            if (ulWD_count >= usWatchdog) {
                return RESET_CARD_WATCHDOG;
            }
            else {
                usWatchdog -= (unsigned short)ulWD_count;                // counting down
            }
        }
    }
#if defined SUPPORT_RTC                                                  // {1}
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    if (RCC_BDCR & RCC_BDCR_RTCEN) {                                     // if RTC is enabled
        if (!(RTC_ISR & (RTC_ISR_INIT))) {                               // if not in initialisation mode
            static unsigned long _RTC_SSR = 0;
            unsigned long ulSubSecondCnt = (((32768/(unsigned short)((RTC_PRER & 0x7fff) + 1)) * (TICK_RESOLUTION/1000))/10); // sub-second increments in the tick period
            int iSecondCount = 0;
            if (_RTC_SSR >= ulSubSecondCnt) {
                _RTC_SSR -= ulSubSecondCnt;                              // the sub-second value decrements
                if (_RTC_SSR == 0) {
                    iSecondCount = 1;
                }
            }
            else {
                if (_RTC_SSR != 0) {
                    iSecondCount = 1;
                }
                _RTC_SSR = ((_RTC_SSR + ((32768/(unsigned short)((RTC_PRER & 0x7fff) + 1)) * 100)) - ulSubSecondCnt);
            }
            RTC_SSR = (_RTC_SSR / 100);
            if (iSecondCount != 0) {                                     // increment the RTC time
                if ((RTC_TR & RTC_TR_SU) == 9) {
                    unsigned long ulIncrement;
                    RTC_TR &= ~(RTC_TR_SU);                              // overflow seconds
                    ulIncrement = ((RTC_TR & RTC_TR_ST) >> 4);           // tens of seconds
                    RTC_TR &= ~(RTC_TR_ST);                              // clear tens of seconds
                    if (ulIncrement == 5) {                              // tens of seconds overflow
                        ulIncrement = ((RTC_TR & RTC_TR_MNU) >> 8);      // minutes
                        RTC_TR &= ~(RTC_TR_MNU);                         // clear minutes
                        if (ulIncrement == 9) {                          // overflow minutes
                            ulIncrement = ((RTC_TR & RTC_TR_MNT) >> 12); // tens of minutes
                            RTC_TR &= ~(RTC_TR_MNT);                     // clear tens of minutes
                            if (ulIncrement == 5) {                      // overflow tens of minutes
                                ulIncrement = ((RTC_TR & RTC_TR_HT) >> 20); // tens of hours
                                ulIncrement *= 10;
                                ulIncrement |= ((RTC_TR & RTC_TR_HU) >> 16); // hours
                                RTC_TR &= ~(RTC_TR_HT | RTC_TR_HU);      // clear hours
                                ulIncrement++;
                                if (ulIncrement == 24) {                 // overflow day
                                    // Simulation doesn't handle beyond days
                                    //
                                }
                                else {
                                    if (ulIncrement > 10) {
                                        if (ulIncrement > 20) {
                                            ulIncrement -= 20;
                                            ulIncrement |= (2 << 4);
                                        }
                                        else {
                                            ulIncrement -= 10;
                                            ulIncrement |= (1 << 4);
                                        }
                                    }
                                    ulIncrement <<= 16;
                                    RTC_TR |= ulIncrement;
                                }
                            }
                            else {
                                ulIncrement++;
                                ulIncrement <<= 12;
                                RTC_TR |= ulIncrement;
                            }
                        }
                        else {
                            ulIncrement++;
                            ulIncrement <<= 8;
                            RTC_TR |= ulIncrement;
                        }
                    }
                    else {
                        ulIncrement++;
                        ulIncrement <<= 4;
                        RTC_TR |= ulIncrement;
                    }
                }
                else {
                    RTC_TR = (RTC_TR + 1);
                }
            }
        }
    }
    #endif
#endif
#if defined SUPPORT_TIMER 
    if (((RCC_APB1ENR & RCC_APB1ENR_TIM2EN) != 0) && ((TIM2_CR1 & TIM_CR1_CEN) != 0)) { // timer 2 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK1 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM2_PSC + 1);                                   // respect the prescaler
        TIM2_CNT += ulTickCount;
        if (TIM2_CNT >= TIM2_ARR) {
            TIM2_CNT = 0;
            TIM2_SR |= TIM_SR_UIF;
            if (TIM2_CR1 & TIM_CR1_OPM) {                                // single-shot mode
                TIM2_CR1 = 0;                                            // automatically disable further operation
            }
            if ((TIM2_DIER & TIM_DIER_UIE) != 0) {                       // overflow interrupt enabled
                if ((IRQ0_31_SER & (1 << irq_TIM2_ID)) != 0) {           // if TIM2 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM2();            // call the interrupt handler
                }
            }
        }
        TIM2_CNT &= 0xffff;
    }

    if (((RCC_APB1ENR & RCC_APB1ENR_TIM3EN) != 0) && ((TIM3_CR1 & TIM_CR1_CEN) != 0)) { // timer 3 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK1 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM3_PSC + 1);                                   // respect the prescaler
        TIM3_CNT += ulTickCount;
        if (TIM3_CNT >= TIM3_ARR) {
            TIM3_CNT = 0;
            TIM3_SR |= TIM_SR_UIF;
            if ((TIM3_CR1 & TIM_CR1_OPM) != 0) {                         // single-shot mode
                TIM3_CR1 = 0;                                            // automatically disable further operation
            }
            if ((TIM3_DIER & TIM_DIER_UIE) != 0) {                       // overflow interrupt enabled
                if ((IRQ0_31_SER & (1 << irq_TIM3_ID)) != 0) {           // if TIM3 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM3();            // call the interrupt handler
                }
            }
        }
        TIM3_CNT &= 0xffff;
    }

    if (((RCC_APB1ENR & RCC_APB1ENR_TIM4EN) != 0) && ((TIM4_CR1 & TIM_CR1_CEN) != 0)) { // timer 4 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK1 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM4_PSC + 1);                                   // respect the prescaler
        TIM4_CNT += ulTickCount;
        if (TIM4_CNT >= TIM4_ARR) {
            TIM4_CNT = 0;
            TIM4_SR |= TIM_SR_UIF;
            if (TIM4_CR1 & TIM_CR1_OPM) {                                // single-shot mode
                TIM4_CR1 = 0;                                            // automatically disable further operation
            }
            if (TIM4_DIER & TIM_DIER_UIE) {                              // overflow interrupt enabled
                if (IRQ0_31_SER & (1 << irq_TIM4_ID)) {                  // if TIM4 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM4();            // call the interrupt handler
                }
            }
        }
        TIM4_CNT &= 0xffff;
    }

    if (((RCC_APB1ENR & RCC_APB1ENR_TIM5EN) != 0) && ((TIM5_CR1 & TIM_CR1_CEN) != 0)) { // timer 5 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK1 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM5_PSC + 1);                                   // respect the prescaler
        TIM5_CNT += ulTickCount;
        if (TIM5_CNT >= TIM5_ARR) {
            TIM5_CNT = 0;
            TIM5_SR |= TIM_SR_UIF;
            if (TIM5_CR1 & TIM_CR1_OPM) {                                // single-shot mode
                TIM5_CR1 = 0;                                            // automatically disable further operation
            }
            if (TIM5_DIER & TIM_DIER_UIE) {                              // overflow interrupt enabled
                if (IRQ32_63_SER & (1 << (irq_TIM5_ID - 32))) {          // if TIM5 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM5();            // call the interrupt handler
                }
            }
        }
        TIM5_CNT &= 0xffff;
    }

    #if defined TIMER_9_AVAILABLE
    if (((RCC_APB2ENR & RCC_APB2ENR_TIM9EN) != 0) && ((TIM9_CR1 & TIM_CR1_CEN) != 0)) { // timer 10 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK2 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM9_PSC + 1);                                   // respect the prescaler
        TIM9_CNT += ulTickCount;
        if (TIM9_CNT >= TIM10_ARR) {
            TIM9_CNT = 0;
            TIM9_SR |= TIM_SR_UIF;
            if (TIM9_CR1 & TIM_CR1_OPM) {                                // single-shot mode
                TIM9_CR1 = 0;                                            // automatically disable further operation
            }
            if (TIM9_DIER & TIM_DIER_UIE) {                              // overflow interrupt enabled
                if (IRQ0_31_SER & (1 << (irq_TIM1_BRK_TIM9_ID))) {       // if TIM9 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM1_BRK_TIM9();   // call the interrupt handler
                }
            }
        }
        TIM9_CNT &= 0xffff;
    }
    #endif
    #if defined TIMER_10_AVAILABLE
    if (((RCC_APB2ENR & RCC_APB2ENR_TIM10EN) != 0) && ((TIM10_CR1 & TIM_CR1_CEN) != 0)) { // timer 10 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK2 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM10_PSC + 1);                                  // respect the prescaler
        TIM10_CNT += ulTickCount;
        if (TIM10_CNT >= TIM10_ARR) {
            TIM10_CNT = 0;
            TIM10_SR |= TIM_SR_UIF;
            if (TIM10_CR1 & TIM_CR1_OPM) {                               // single-shot mode
                TIM10_CR1 = 0;                                           // automatically disable further operation
            }
            if (TIM10_DIER & TIM_DIER_UIE) {                             // overflow interrupt enabled
                if (IRQ0_31_SER & (1 << (irq_TIM1_UP_TIM10_ID))) {       // if TIM10 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM1_UP_TIM10();   // call the interrupt handler
                }
            }
        }
        TIM10_CNT &= 0xffff;
    }
    #endif
    #if defined TIMER_11_AVAILABLE
    if ((RCC_APB2ENR & RCC_APB2ENR_TIM11EN) && (TIM11_CR1 & TIM_CR1_CEN)) { // timer 11 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK2 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM11_PSC + 1);                                  // respect the prescaler
        TIM11_CNT += ulTickCount;
        if (TIM11_CNT >= TIM10_ARR) {
            TIM11_CNT = 0;
            TIM11_SR |= TIM_SR_UIF;
            if (TIM11_CR1 & TIM_CR1_OPM) {                               // single-shot mode
                TIM11_CR1 = 0;                                           // automatically disable further operation
            }
            if (TIM11_DIER & TIM_DIER_UIE) {                             // overflow interrupt enabled
                if (IRQ0_31_SER & (1 << (irq_TIM1_TRG_COM_TIM11_ID))) {  // if TIM10 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM1_TRG_COM_TIM11(); // call the interrupt handler
                }
            }
        }
        TIM11_CNT &= 0xffff;
    }
    #endif
    #if defined TIMER_12_AVAILABLE
    if ((RCC_APB1ENR & RCC_APB1ENR_TIM12EN) && (TIM12_CR1 & TIM_CR1_CEN)) { // timer 13 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK1 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM12_PSC + 1);                                  // respect the prescaler
        TIM12_CNT += ulTickCount;
        if (TIM12_CNT >= TIM10_ARR) {
            TIM12_CNT = 0;
            TIM12_SR |= TIM_SR_UIF;
            if (TIM12_CR1 & TIM_CR1_OPM) {                               // single-shot mode
                TIM12_CR1 = 0;                                           // automatically disable further operation
            }
            if (TIM12_DIER & TIM_DIER_UIE) {                             // overflow interrupt enabled
                if (IRQ32_63_SER & (1 << (irq_TIM8_BRK_TIM12_ID - 32))) {// if TIM12 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM8_BRK_TIM12();  // call the interrupt handler
                }
            }
        }
        TIM12_CNT &= 0xffff;
    }
    #endif
    #if defined TIMER_13_AVAILABLE
    if ((RCC_APB1ENR & RCC_APB1ENR_TIM13EN) && (TIM13_CR1 & TIM_CR1_CEN)) { // timer 13 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK1 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM13_PSC + 1);                                  // respect the prescaler
        TIM13_CNT += ulTickCount;
        if (TIM13_CNT >= TIM10_ARR) {
            TIM13_CNT = 0;
            TIM13_SR |= TIM_SR_UIF;
            if (TIM13_CR1 & TIM_CR1_OPM) {                               // single-shot mode
                TIM13_CR1 = 0;                                           // automatically disable further operation
            }
            if (TIM13_DIER & TIM_DIER_UIE) {                             // overflow interrupt enabled
                if (IRQ32_63_SER & (1 << (irq_TIM8_UP_TIM13_ID - 32))) { // if TIM10 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM8_UP_TIM13(); // call the interrupt handler
                }
            }
        }
        TIM13_CNT &= 0xffff;
    }
    #endif
    #if defined TIMER_14_AVAILABLE
    if ((RCC_APB1ENR & RCC_APB1ENR_TIM14EN) && (TIM14_CR1 & TIM_CR1_CEN)) { // timer 14 is enabled
        unsigned long ulTickCount = ((TICK_RESOLUTION/1000) * ((PCLK1 * 2)/1000)); // count per tick period with no prescaler
        ulTickCount /= (TIM14_PSC + 1);                                  // respect the prescaler
        TIM14_CNT += ulTickCount;
        if (TIM14_CNT >= TIM10_ARR) {
            TIM14_CNT = 0;
            TIM14_SR |= TIM_SR_UIF;
            if (TIM14_CR1 & TIM_CR1_OPM) {                               // single-shot mode
                TIM14_CR1 = 0;                                           // automatically disable further operation
            }
            if (TIM14_DIER & TIM_DIER_UIE) {                             // overflow interrupt enabled
                if (IRQ32_63_SER & (1 << (irq_TIM8_TRG_COM_TIM14_ID - 32))) { // if TIM10 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_TIM8_TRG_COM_TIM14(); // call the interrupt handler
                }
            }
        }
        TIM14_CNT &= 0xffff;
    }
    #endif
#endif
#if defined SUPPORT_ADC                                                  // {10}
    fnTriggerADC(1, 0);                                                  // handle software triggered ADC1
    #if ADC_CONTROLLERS > 1
    fnTriggerADC(2, 0);                                                  // handle software triggered ADC2
    #endif
    #if ADC_CONTROLLERS > 2
    fnTriggerADC(3, 0);                                                  // handle software triggered ADC3
    #endif
#endif
    return 0;
}

#if defined SUPPORT_RTC

#define NTP_TO_1970_TIME 2208988800u
#define LEAP_YEAR(year) ((year%4)==0)                                    // valid from 1970 to 2038
static const unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
// Synchronise the internal RTC to the PC time when simulating
//
extern void fnInitInternalRTC(char *argv[])
{
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    unsigned long ulRegister;
#endif
    unsigned short *ptrShort = (unsigned short *)*argv;
    unsigned short RTC_YEAR, RTC_MONTH, RTC_DOW, RTC_DOM, RTC_HOUR, RTC_MIN, RTC_SEC;
    unsigned short usMonth;
    unsigned long ulStartTime;
    unsigned long ulLeapYears = 1970;

    RTC_YEAR = *ptrShort++;
    RTC_MONTH = *ptrShort++;
    RTC_DOW = *ptrShort++;
    RTC_DOM = *ptrShort++;
    RTC_HOUR = *ptrShort++;
    RTC_MIN = *ptrShort++;
    RTC_SEC = *ptrShort++;

    usMonth = RTC_MONTH;

    ulStartTime = ((RTC_YEAR - 1970) * 365);                             // years since reference time, represented in days without respecting leap years
    while (ulLeapYears < RTC_YEAR) {
        if (LEAP_YEAR(ulLeapYears)) {                                    // count leap years
            if (ulLeapYears == RTC_YEAR) {                               // presently in a leap year
                if ((usMonth > 2) || (RTC_DOM > 28)) {                   // past February 28 so count extra leap day in this year
                    ulStartTime++;
                }
            }
            else {
                ulStartTime++;                                           // count extra days in passed leap years
            }
        }
        ulLeapYears++;
    }
    while (--usMonth != 0) {
        ulStartTime += monthDays[RTC_MONTH];                             // add past days of previous months of this year
    }
    ulStartTime += (RTC_DOM - 1);                                        // add past number of days in present month
    ulStartTime *= 24;                                                   // convert days to hours
    ulStartTime += RTC_HOUR;                                             // add hours passed in present day
    ulStartTime *= 60;                                                   // convert hours to minutes
    ulStartTime += RTC_MIN;                                              // add minutes in present hour
    ulStartTime *= 60;                                                   // convert minutes to seconds
    ulStartTime += RTC_SEC;                                              // add seconds in present minute

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {1}
    ulRegister  = (RTC_SEC%10);
    ulRegister |= ((RTC_SEC/10) << 4);
    ulRegister |= ((RTC_MIN%10) << 8);
    ulRegister |= ((RTC_MIN/10) << 12);
    ulRegister |= ((RTC_HOUR%10) << 16);
    ulRegister |= ((RTC_HOUR/10) << 20);
    RTC_TR = ulRegister;

    ulRegister  = (RTC_DOM%10);
    ulRegister |= ((RTC_DOM/10) << 4);
    ulRegister |= ((RTC_MONTH%10) << 8);
    ulRegister |= ((RTC_MONTH/10) << 12);
    ulRegister |= (RTC_DOW << 13);
    ulRegister |= (((RTC_YEAR - 2000)%10) << 16);
    ulRegister |= (((RTC_YEAR - 2000)/10) << 20);
    RTC_DR = ulRegister;

    RTC_ISR = RTC_ISR_INITS;                                             // initialised
    RTC_PRER = ((((32768/512) - 1) << RTC_PRER_ASYN_SHIFT) | ((512 - 1))); // assume programmed for 32kHz external oscillator
    RCC_BDCR = RCC_BDCR_RTCEN;
#else
    RTC_CNTL = (unsigned short)ulStartTime;                              // set the initial seconds count value (since 1970)
    RTC_CNTH = (unsigned short)(ulStartTime >> 16);
#endif
}
#endif

#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE
extern void fnSTMPE811(int iX, int iY);

extern void fnPenPressed(int iX, int iY)
{
    fnSTMPE811(iX, iY);
    fnSimulateInputChange(PORT_EXT_TOUCH_IRQ_PORT, (15 - PORT_EXT_TOUCH_IRQ_PORT_BIT), CLEAR_INPUT);
}

extern void fnPenMoved(int iX, int iY)
{
    fnSTMPE811(iX, iY);
}

extern void fnPenLifted(void)
{
    fnSTMPE811(-1, -1);
    fnSimulateInputChange(PORT_EXT_TOUCH_IRQ_PORT, (15 - PORT_EXT_TOUCH_IRQ_PORT_BIT), CLEAR_INPUT);
}
#endif


#if defined CAN_INTERFACE
static int iLastTxBuffer;

typedef struct stTCP_MESSAGE                                             // definition of a data frame structure
{
  //TCP_HEADER     tTCP_Header;                                          // reserve header space
    unsigned char  ucTCP_Message[50];                                    // space for content
} TCP_MESSAGE;

#define SIM_CAN_TX_OK           0
#define SIM_CAN_TX_REMOTE_OK    1
#define SIM_CAN_TX_FAIL         2
#define SIM_CAN_TX_REMOTE_FAIL  3
#define SIM_CAN_RX_READY        4



    #if !defined _LOCAL_SIMULATION && defined SIM_KOMODO
// Komodo has received something on the CAN bus
//
static void fnCAN_reception(int iChannel, unsigned char ucDataLength, unsigned char *ptrData, unsigned long ulId, int iExtendedAddress, int iRemodeRequest, unsigned short usTimeStamp, Komodo km)
{
    static CAN_MAILBOX RxMailboxFIFO[NUMBER_OF_CAN_INTERFACES][2][2] = {{{0}}}; // additional message FIFO space

    int i;
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    STM_CAN_CONTROL *ptrCAN_control;
    CAN_MAILBOX *ptrRxMailbox;
    unsigned long *ptrFilters = CAN1_F0R1_ADDR;
    unsigned long *ptrFilterEnd = CAN1_F0R1_ADDR;
    if ((ucDataLength == 0) && (iRemodeRequest == 0)) {
        return;                                                          // ignore when reception without data
    }
    if (iChannel == 0) {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
        ptrRxMailbox = ADDR_CAN1_MAILBOX_0_RX;                           // the first of 2 receive mailboxes
        ptrFilterEnd += (((CAN1_FMR & CAN_FMR_CAN2SB_MASK) >> CAN_FMR_CAN2SB_SHIFT) * 2);
        i = 0;                                                           // start in first half of filter table
    }
    else {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
        ptrRxMailbox = ADDR_CAN2_MAILBOX_0_RX;                           // the first of 2 receive mailboxes
        ptrFilterEnd += (NUMBER_CAN_RX_FILTERS * 2);
        i = ((CAN1_FMR & CAN_FMR_CAN2SB_MASK) >> CAN_FMR_CAN2SB_SHIFT);  // start in the second half fo the filter table
        ptrFilters += (((CAN1_FMR & CAN_FMR_CAN2SB_MASK) >> CAN_FMR_CAN2SB_SHIFT) * 2);
    }

    if (iExtendedAddress == 0) {
        ulId <<= CAN_RIR_STID_SHIFT;
        ulId &= CAN_RIR_STID_MASK;
    }
    else {
        ulId <<= CAN_RIR_EXID_SHIFT;
        ulId &= CAN_RIR_EXID_MASK;
        ulId |= CAN_RIR_IDE;
    }
    if (iRemodeRequest != 0) {
        ulId |= CAN_RIR_RTR;
    }
    while (ptrFilters < ptrFilterEnd) {                                  // search for a filter matching this ID
        if (CAN1_FA1R & (CAN_FA1R_FACT0ON << i)) {                       // if the filter is active
            if (CAN1_FM1R & (CAN_FM1R_FBM0_LIST_MODE << i)) {            // list mode (not supported
            }
            else {                                                       // mask mode
                if ((ulId & *(ptrFilters + 1)) == (*ptrFilters & *(ptrFilters + 1))) { // check whether the filter matches
                    break;                                               // match found
                }
            }
        }
        i++;
        ptrFilters += 2;
    }
    if (ptrFilters < ptrFilterEnd) {                                     // match found
        if (CAN1_FFA1R & (CAN_FFA1R_FFA0_FIFO1 << i)) {                  // if the filter uses FIFO 1
            if ((ptrCAN_control->CAN_RF1R & CAN_RFR_FMP_MASK) < 3) {     // room in the FIFO
                if ((ptrCAN_control->CAN_RF1R & CAN_RFR_FMP_MASK) == 0) {// first message in the FIFO
                    ptrRxMailbox++;
                }
                else {                                                   // else put in FIFO queue
                    ptrRxMailbox = &RxMailboxFIFO[iChannel][1][(ptrCAN_control->CAN_RF1R & CAN_RFR_FMP_MASK) - 1];
                }
                ptrRxMailbox->CAN_TIR = ulId;
                ptrRxMailbox->CAN_TDTR = ((((i - 1)/2) << 8) & CAN_RDTR_FMI_MASK) + (ucDataLength & CAN_RDTR_DLC_MASK); // the filter number assumes that all are used in 32 bit mask mode and the FIFO use alternates 0,1,0,1,..
                if (ptrCAN_control->CAN_MCR & CAN_MCR_TTCM) {            // if time stamp enabled
                    ptrRxMailbox->CAN_TDTR |= (usCAN_time << 16);
                }
                if (ptrData != 0) {
                    ptrRxMailbox->CAN_TDLR = *ptrData++;
                    ptrRxMailbox->CAN_TDLR |= (unsigned char)(*ptrData++ << 8);
                    ptrRxMailbox->CAN_TDLR |= (unsigned char)(*ptrData++ << 16);
                    ptrRxMailbox->CAN_TDLR |= (unsigned char)(*ptrData++ << 24);
                    ptrRxMailbox->CAN_TDHR = *ptrData++;
                    ptrRxMailbox->CAN_TDHR |= (unsigned char)(*ptrData++ << 8);
                    ptrRxMailbox->CAN_TDHR |= (unsigned char)(*ptrData++ << 16);
                    ptrRxMailbox->CAN_TDHR |= (unsigned char)(*ptrData++ << 24);
                }
                ptrCAN_control->CAN_RF1R++;                              // new FIFO depth
                if (ptrCAN_control->CAN_RF1R == 3) {
                    ptrCAN_control->CAN_RF1R |= CAN_RFR_FULL;            // FIFO is full
                }
                if (ptrCAN_control->CAN_IER & CAN_IER_FMPIE1) {          // if FIFO 1 interrupt is enabled
                    if (iChannel != 0) {
                        if (IRQ64_95_SER & (1 << (irq_CAN2_RX1_ID - 65))) { // if CAN 2 Rx FIFO 1 interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN2_RX1();// call the interrupt handler
                        }
                    }
                    else {
                        if (IRQ0_31_SER & (1 << irq_CAN1_RX1_ID)) {      // if CAN 1  Rx FIFO 1 interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN1_RX1();// call the interrupt handler
                        }
                    }
                    if (ptrCAN_control->CAN_RF1R & CAN_RFR_RFOM) {       // the user has released the FIFO
                        ptrCAN_control->CAN_RF1R &= ~(CAN_RFR_RFOM);
                        ptrCAN_control->CAN_RF1R--;
                    }
                }
            }
            else {
                ptrCAN_control->CAN_RF1R |= CAN_RFR_FOVR;                // FIFO overrun
            }
        }
        else {                                                           // the filter uses FIFO 0
            if ((ptrCAN_control->CAN_RF0R & CAN_RFR_FMP_MASK) < 3) {     // room in the FIFO
                if ((ptrCAN_control->CAN_RF0R & CAN_RFR_FMP_MASK) != 0) {// not first message in the FIFO                                                 
                    ptrRxMailbox = &RxMailboxFIFO[iChannel][0][(ptrCAN_control->CAN_RF0R & CAN_RFR_FMP_MASK) - 1]; // so put in FIFO queue
                }
                ptrRxMailbox->CAN_TIR = ulId;
                ptrRxMailbox->CAN_TDTR = ((((i - 1)/2) << 8) & CAN_RDTR_FMI_MASK) + (ucDataLength & CAN_RDTR_DLC_MASK); // the filter number assumes that all are used in 32 bit mask mode and the FIFO use alternates 0,1,0,1,..
                if (ptrCAN_control->CAN_MCR & CAN_MCR_TTCM) {            // if time stamp enabled
                    ptrRxMailbox->CAN_TDTR |= (usCAN_time << 16);
                }
                if (ptrData != 0) {
                    ptrRxMailbox->CAN_TDLR = *ptrData++;
                    ptrRxMailbox->CAN_TDLR |= (*ptrData++ << 8);
                    ptrRxMailbox->CAN_TDLR |= (*ptrData++ << 16);
                    ptrRxMailbox->CAN_TDLR |= (*ptrData++ << 24);
                    ptrRxMailbox->CAN_TDHR = *ptrData++;
                    ptrRxMailbox->CAN_TDHR |= (*ptrData++ << 8);
                    ptrRxMailbox->CAN_TDHR |= (*ptrData++ << 16);
                    ptrRxMailbox->CAN_TDHR |= (*ptrData++ << 24);
                }
                ptrCAN_control->CAN_RF0R++;                              // new FIFO depth
                if (ptrCAN_control->CAN_RF0R == 3) {
                    ptrCAN_control->CAN_RF0R |= CAN_RFR_FULL;            // FIFO is full
                }
                if (ptrCAN_control->CAN_IER & CAN_IER_FMPIE0) {          // if FIFO 0 interrupt is enabled
                    if (iChannel != 0) {
                        if (IRQ64_95_SER & (1 << (irq_CAN2_RX0_ID - 64))) { // if CAN 2  Rx FIFO 0 interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN2_RX0();// call the interrupt handler
                        }
                    }
                    else {
                        if (IRQ0_31_SER & (1 << irq_CAN1_RX0_ID)) {      // if CAN 1  Rx FIFO 0 interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN1_RX0();// call the interrupt handler
                        }
                    }
                    if (ptrCAN_control->CAN_RF0R & CAN_RFR_RFOM) {       // the user has released the FIFO
                        ptrCAN_control->CAN_RF0R &= ~(CAN_RFR_RFOM);
                        ptrCAN_control->CAN_RF0R--;
                    }
                }
            }
            else {
                ptrCAN_control->CAN_RF0R |= CAN_RFR_FOVR;                // FIFO overrun
            }

        }
    }
}
    #endif

extern void fnSimCAN(int iChannel, int iBufferNumber, int iSpecial)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    CAN_MAILBOX *ptrMailbox;
    STM_CAN_CONTROL *ptrCAN_control;
    #ifndef _LOCAL_SIMULATION
        #if defined USE_TELNET && defined UTASKER_SIM
    unsigned short usTxDataLength;
    TCP_MESSAGE tcp_message;
        #elif defined SIM_KOMODO
    unsigned char ucTxDataLength;
    static Komodo km = -1;
        #endif
    #endif

    #if defined SIM_KOMODO
    if (CAN_SIM_CHECK_RX == iSpecial) {
        u08 data_in[8];
        km_can_packet_t pkt;
        km_can_info_t   info;
        pkt.remote_req  = 0;
        pkt.extend_addr = 0;
        pkt.dlc         = 8;
        pkt.id          = 0xff;                                          // promiscuous

        if (km < 0) {
            return;
        }
        while ((km_can_read(km, &info, &pkt, 8, data_in)) >= 0) {        // has something been received?
            fnCAN_reception(info.channel, pkt.dlc, data_in, pkt.id, pkt.extend_addr, pkt.remote_req, (unsigned short)(info.timestamp), km);
        }
        return;
    }
    else if (CAN_SIM_TERMINATE == iSpecial) {
        if (km < 0) {
            return;
        }
        km_disable(km);
        km_close(KOMODO_USB_PORT);
        return;
    }
    #endif
    if (iChannel == 0) {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN1_BLOCK;
        ptrMailbox = ADDR_CAN1_MAILBOX_TX;                               // the first of 3 transmit mailboxes
    }
    else {
        ptrCAN_control = (STM_CAN_CONTROL *)CAN2_BLOCK;
        ptrMailbox = ADDR_CAN2_MAILBOX_TX;                               // the first of 3 transmit mailboxes
    }

    // Configuration changes
    //
    if (ptrCAN_control->CAN_MSR & (CAN_MSR_INAK | CAN_MSR_SLAK)) {       // in sleep or initialisation modes
        return;                                                          // off so don't do anything
    }

    #if defined USE_TELNET && defined UTASKER_SIM
    if (SimSocket == -1) {
        #if defined IMMEDIATE_MEMORY_ALLOCATION                          // {17} extra parameter for tx buffer size to immediately allocate (uses default size)
        SimSocket = fnStartTelnet(usSimPort, 0xffff, 0, 0, 0, fnCANListener);
        #else
        SimSocket = fnStartTelnet(usSimPort, 0xffff, 0, 0, fnCANListener);
        #endif
        fnTelnet(SimSocket, TELNET_RAW_MODE);                            // set to RAW operating mode
        fnTCP_Connect(SimSocket, ucSimIP, usSimPort, 0, 0);              // attempt to establish a connection to remote  server
        return;
    }
    #elif defined SIM_KOMODO
    if (CAN_SIM_INITIALISE == iSpecial) {
        if (km < 0) {
            km = km_open(KOMODO_USB_PORT);                               // open a link to the Komodo via USB (channel 0 uses CAN A, channel 1 uses CAN B)
        }
        if (km >= 0) {
            int iCAN_speed = ((PCLK1/25)/((ptrCAN_control->CAN_BTR & CAN_BTR_BRP_MASK) + 1)); // assume that time quanta is 25
            if (iChannel != 0) {                                         // CAN B
                km_disable(km);
                km_acquire(km, (KM_FEATURE_CAN_B_CONFIG | KM_FEATURE_CAN_B_CONTROL | KM_FEATURE_CAN_B_LISTEN)); // acquire features
                km_timeout(km, KM_TIMEOUT_IMMEDIATE);
                km_can_bitrate(km, KM_CAN_CH_B, iCAN_speed);             // set bitrate
              //km_can_target_power(km, KM_CAN_CH_B, 1);                 // set target power
                km_enable(km);
              //km_can_target_power(km, KM_CAN_CH_B, 0);                 // disable target power
            }
            else {                                                       // CAN A
                km_disable(km);
                km_acquire(km, (KM_FEATURE_CAN_A_CONFIG | KM_FEATURE_CAN_A_CONTROL | KM_FEATURE_CAN_A_LISTEN)); // acquire features
                km_timeout(km, KM_TIMEOUT_IMMEDIATE);
                km_can_bitrate(km, KM_CAN_CH_A, iCAN_speed);             // set bitrate
              //km_can_target_power(km, KM_CAN_CH_A, 1);                 // set target power
                km_enable(km);
              //km_can_target_power(km, KM_CAN_CH_A, 0);                 // disable target power
            }
        }
    }
    #endif

    ptrMailbox += iBufferNumber;                                         // move to the mailbox to be used
    if (!(ptrMailbox->CAN_TIR & CAN_TIR_TXRQ)) {                         // if the tranmit request is not set we ignore it
        return;
    }

    // Transmission
    //
    if (ptrMailbox->CAN_TIR & CAN_TIR_TXRQ) {                            // transmission is requested
    #if defined SIM_KOMODO
        unsigned char ucData[8];
        int iResult;
        km_can_packet_t pkt;
        unsigned long arb_count = 0;
        // Send the CAN frame via remote simulator
        // 
        if (ptrMailbox->CAN_TIR & CAN_IR_IDE) {                          // extended identifier
            pkt.id = (ptrMailbox->CAN_TIR >> CAN_IR_EXTENDED_ID_SHIFT);
            pkt.id |= CAN_EXTENDED_ID;                                   // the address is to be handled as extended
            pkt.extend_addr = 1;
        }   
        else {
            pkt.id = (ptrMailbox->CAN_TIR >> CAN_IR_STANDARD_ID_SHIFT); // standard identifier
            pkt.extend_addr = 0;
        }
        iLastTxBuffer = iBufferNumber;
        ucTxDataLength = (unsigned char)(ptrMailbox->CAN_TDTR & CAN_TDTR_DLC_MASK);
        pkt.dlc = ucTxDataLength;
        ucData[0] = (unsigned char)ptrMailbox->CAN_TDLR;
        ucData[1] = (unsigned char)(ptrMailbox->CAN_TDLR >> 8);
        ucData[2] = (unsigned char)(ptrMailbox->CAN_TDLR >> 16);
        ucData[3] = (unsigned char)(ptrMailbox->CAN_TDLR >> 24);
        ucData[4] = (unsigned char)ptrMailbox->CAN_TDHR;
        ucData[5] = (unsigned char)(ptrMailbox->CAN_TDHR >> 8);
        ucData[6] = (unsigned char)(ptrMailbox->CAN_TDHR >> 16);
        ucData[7] = (unsigned char)(ptrMailbox->CAN_TDHR >> 24);
        if (ptrMailbox->CAN_TDTR & CAN_TDTR_TGT) {                       // if transmit global time is enabled the CAN time is inserted into the last two bytes of the frame
            ucData[6] = (unsigned char)(usCAN_time >> 8);
            ucData[7] = (unsigned char)(usCAN_time);
        }
        if (ptrMailbox->CAN_TIR & CAN_IR_RTR) {
            pkt.remote_req = 1;
            km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, 0, 0);  // send
        }
        else {
            pkt.remote_req = 0;
            km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, ucTxDataLength, (const unsigned char *)ucData);  // send
        }
        iResult = km_can_async_collect(km, 10, &arb_count);              // collect the result of the last transmission
        switch (iResult) {
        case KM_OK:
            switch (iBufferNumber) {
            case 0:
                ptrCAN_control->CAN_TSR |= (CAN_TSR_RQCP0 | CAN_TSR_TXOK0);
                break;
            case 1:
                ptrCAN_control->CAN_TSR |= (CAN_TSR_RQCP1 | CAN_TSR_TXOK1);
                break;
            case 2:
                ptrCAN_control->CAN_TSR |= (CAN_TSR_RQCP2 | CAN_TSR_TXOK2);
                break;
            }
            ptrMailbox->CAN_TIR &= ~CAN_TIR_TXRQ;
            if (ptrCAN_control->CAN_TSR & (CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2)) { // sucessful transmission
                ptrMailbox->CAN_TDTR = ((ptrMailbox->CAN_TDTR & ~CAN_TDTR_TIME_MASK) | (usCAN_time << 16)); // add a time stamp
                if (ptrCAN_control->CAN_IER & CAN_IER_TMEIE) {           // if tx interrupt is enabled
                    if (iChannel != 0) {
                        if (IRQ32_63_SER & (1 << (irq_CAN2_TX_ID - 32))) { // if CAN 2 Tx interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN2_TX(); // call the interrupt handler
                        }
                    }
                    else {
                        if (IRQ0_31_SER & (1 << irq_CAN1_TX_ID)) {       // if CAN 1 Tx interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN1_TX(); // call the interrupt handler
                        }
                    }
                }
            }
            break;
        default:                                                         // transmission error
            switch (iBufferNumber) {
            case 0:
                ptrCAN_control->CAN_TSR |= (CAN_TSR_TERR0);
                break;
            case 1:
                ptrCAN_control->CAN_TSR |= (CAN_TSR_TERR1);
                break;
            case 2:
                ptrCAN_control->CAN_TSR |= (CAN_TSR_TERR2);
                break;
            }
            if (ptrCAN_control->CAN_TSR & (CAN_TSR_TERR0 | CAN_TSR_TERR1 | CAN_TSR_TERR2)) { // failed transmission
                if ((ptrCAN_control->CAN_IER & CAN_IER_EPVIE) &&  (ptrCAN_control->CAN_IER & CAN_IER_ERRIE)) { // if passive error interrupt enabled
                    if (iChannel != 0) {
                        if (IRQ64_95_SER & (1 << (irq_CAN2_SCE_ID - 64))) { // if CAN 2 Tx interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN2_SCE();// call the interrupt handler
                        }
                    }
                    else {
                        if (IRQ0_31_SER & (1 << irq_CAN1_SCE_ID)) {      // if CAN 1 Tx interrupt is not disabled
                            ptrVect->processor_interrupts.irq_CAN1_SCE();// call the interrupt handler
                        }
                    }
                }
            }
            break;
        }
    #endif
    }
}
#endif

#if 1 //defined RUN_IN_FREE_RTOS
extern unsigned long *fnGetRegisterAddress(unsigned long ulAddress)
{
    ulAddress -= 0xe000e000;
    ulAddress += (unsigned long)CORTEX_M3_BLOCK;
    return (unsigned long *)ulAddress;
}

extern void fnSetReg(int iRef, unsigned long ulValue)
{
    switch (iRef) {
    case 0:
        STM32.CORTEX_M3_REGS.ulR0 = ulValue;
        break;
    case 14:
        STM32.CORTEX_M3_REGS.ulPSP = ulValue;
        break;
    case 15:
        STM32.CORTEX_M3_REGS.ulMSP = ulValue;
        break;
    case 19:
        STM32.CORTEX_M3_REGS.ulPRIMASK = ulValue;
        break;
    case 20:
        STM32.CORTEX_M3_REGS.ulFAULTMASK = ulValue;
        break;
    case 22:
        STM32.CORTEX_M3_REGS.ulCONTROL = ulValue;
        break;
    }
}
#endif

// Prepare a string to be displayed in the simulator status bar          // {8}
//
extern void fnUpdateOperatingDetails(void)
{
    #if !defined NO_STATUS_BAR
    extern void fnPostOperatingDetails(char *ptrDetails);
    unsigned long ulHCLK;
    unsigned long ulAPB1;
    #if defined RCC_CFGR_PPRE2_HCLK_DIV2
    unsigned long ulAPB2;
    #endif
    CHAR cOperatingDetails[256];
    CHAR *ptrBuffer = cOperatingDetails;
    ptrBuffer = uStrcpy(ptrBuffer, "FLASH = ");
    ptrBuffer = fnBufferDec((SIZE_OF_FLASH/1024), 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, "k, SRAM = ");
    ptrBuffer = fnBufferDec((SIZE_OF_RAM/1024), 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, "k, HCLK = ");
    ulHCLK = (PLL_OUTPUT_FREQ >> ((RCC_CFGR & RCC_CFGR_HPRE_SYSCLK_DIV512) >> 4)); // HCLK according to register setting
    if ((RCC_CFGR & RCC_CFGR_PPRE1_HCLK_DIV2) != 0) {                    // if divide enabled
        #if defined _STM32L432 || defined _STM32L0x1
        ulAPB1 = (ulHCLK >> (((RCC_CFGR >> 8) & 0x03) + 1));             // APB1 clock according to register settings
        #else
        ulAPB1 = (ulHCLK >> (((RCC_CFGR >> 10) & 0x03) + 1));            // APB1 clock according to register settings
        #endif
    }
    else {
        ulAPB1 = ulHCLK;
    }
    #if defined RCC_CFGR_PPRE2_HCLK_DIV2
    if ((RCC_CFGR & RCC_CFGR_PPRE2_HCLK_DIV2) != 0) {                    // if divide enabled
        #if defined _STM32L432 || defined _STM32L0x1
        ulAPB2 = (ulHCLK >> (((RCC_CFGR >> 11) & 0x03) + 1));            // APB2 clock according to register settings
        #else
        ulAPB2 = (ulHCLK >> (((RCC_CFGR >> 13) & 0x03) + 1));            // APB2 clock according to register settings
        #endif
    }
    else {
        ulAPB2 = ulHCLK;
    }
    #endif
    ptrBuffer = fnBufferDec(ulHCLK, 0, ptrBuffer);
    #if defined RCC_CFGR_PPRE2_HCLK_DIV2
    ptrBuffer = uStrcpy(ptrBuffer, ", APB1 = ");
    ptrBuffer = fnBufferDec(ulAPB1, 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, ", APB2 = ");
    ptrBuffer = fnBufferDec(ulAPB2, 0, ptrBuffer);
    #else
    ptrBuffer = uStrcpy(ptrBuffer, ", APB = ");
    ptrBuffer = fnBufferDec(ulAPB1, 0, ptrBuffer);
    #endif
    fnPostOperatingDetails(cOperatingDetails);
    #endif
}
#endif



