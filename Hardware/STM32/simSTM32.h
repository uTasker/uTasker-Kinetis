/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      simSTM32.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    08.09.2012 Adapt RTC for F2/F4 devices                               {1}
    11.09.2012 Add SYSTEM_HANDLER_x_y_PRIORITY_REGISTERs                 {2}
    12.09.2012 Add I2C channel 3                                         {3}
    04.06.2013 Add USB-OTG to F1                                         {4}
    15.06.2014 Add F2/F4 DMA registers                                   {5}
    31.08.2017 Add ADC block                                             {6}
    19.06.2018 Change _GPIO_x to _PORTx for global compatibiity          {7}

*/  



typedef struct stSTM32_ETH
{
unsigned long ETH_MACCR;
unsigned long ETH_MACFF;
unsigned long ETH_MACHTHR;
unsigned long ETH_MACHTLR;
unsigned long ETH_MACMIIAR;
unsigned long ETH_MACMIIDR;
unsigned long ETH_MACFCR;
unsigned long ETH_MACVLANTR;
unsigned long ulRes1[2];
unsigned long ETH_MACRWUFFR;
unsigned long ETH_MACPMTCSR;
unsigned long ulRes2[2];
unsigned long ETH_MACSR;
unsigned long ETH_MACIMR;
unsigned long ETH_MACA0HR;
unsigned long ETH_MACA0LR;
unsigned long ETH_MACA1HR;
unsigned long ETH_MACA1LR;
unsigned long ETH_MACA2HR;
unsigned long ETH_MACA2LR;
unsigned long ETH_MACA3HR;
unsigned long ETH_MACA3LR;
unsigned long ulRes3[40];
unsigned long ETH_MMCCR;
unsigned long ETH_MMCRIR;
unsigned long ETH_MMCTIR;
unsigned long ETH_MMCRIMR;
unsigned long ETH_MMCTIMR;
unsigned long ulRes4[14];
unsigned long ETH_MMCTGFSCCR;
unsigned long ETH_MMCTGFMSCCR;
unsigned long ulRes5[5];
unsigned long ETH_MMCTGFCR;
unsigned long ulRes6[10];
unsigned long ETH_MMCRFCECR;
unsigned long ETH_MMCRFAECR;
unsigned long ulRes7[10];
unsigned long ETH_MMCRGUFCR;
unsigned long ulRes8[334];
unsigned long ETH_PTPTSCR;
unsigned long ETH_PTPSSIR;
unsigned long ETH_PTPTSHR;
unsigned long ETH_PTPTSLR;
unsigned long ETH_PTPTSHUR;
unsigned long ETH_PTPTSLUR;
unsigned long ETH_PTPTSAR;
unsigned long ETH_PTPTTHR;
unsigned long ETH_PTPTTLR;
unsigned long ulRes9[567];
unsigned long ETH_DMABMR;
unsigned long ETH_DMATPDR;
unsigned long ETH_DMARPDR;
unsigned long ETH_DMARDLAR;
unsigned long ETH_DMATDLAR;
unsigned long ETH_DMASR;
unsigned long ETH_DMAOMR;
unsigned long ETH_DMAIER;
unsigned long ETH_DMAMFBOCR;
unsigned long ulRes10[9];
unsigned long ETH_DMACHTDR;
unsigned long ETH_DMACHRDR;
unsigned long ETH_DMACHTBAR;
unsigned long ETH_DMACHRBAR;
} STM32_ETH;



typedef struct stSTM32_RCC
{
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    unsigned long RCC_CR;
    unsigned long RCC_PLLCFG;
    unsigned long RCC_CFGR;
    unsigned long RCC_CIR;
    unsigned long RCC_AHB1RSTR;
    unsigned long RCC_AHB2RSTR;
    unsigned long RCC_AHB3RSTR;
    unsigned long ulRes0;
    unsigned long RCC_APB1RSTR;
    unsigned long RCC_APB2RSTR;
    unsigned long ulRes1[2];
    unsigned long RCC_AHB1ENR;
    unsigned long RCC_AHB2ENR;
    unsigned long RCC_AHB3ENR;
    unsigned long ulRes2;
    unsigned long RCC_APB1ENR;
    unsigned long RCC_APB2ENR;
    unsigned long ulRes3[2];
    unsigned long RCC_AHB1LPENT;
    unsigned long RCC_AHB2LPENT;
    unsigned long RCC_AHB3LPENT;
    unsigned long ulRes4;
    unsigned long RCC_APB1LPENT;
    unsigned long RCC_APB2LPENT;
    unsigned long ulRes5[2];
    unsigned long RCC_BDCR;
    unsigned long RCC_CSR;
    unsigned long ulRes6[2];
    unsigned long RCC_SSCGR;
    unsigned long RCC_PLLI2SCFGR;
#elif defined _STM32L432
    unsigned long RCC_CR;
    unsigned long RCC_ICSCR;
    unsigned long RCC_CFGR;
    unsigned long RCC_PLLCFGR;
    unsigned long RCC_PLLSAI1CFGR;
    unsigned long ulRes0;
    unsigned long RCC_CIER;
    unsigned long RCC_CIFR;
    unsigned long RCC_CICR;
    unsigned long ulRes1;
    unsigned long RCC_AHB1RSTR;
    unsigned long RCC_AHB2RSTR;
    unsigned long RCC_AHB3RSTR;
    unsigned long ulRes2;
    unsigned long RCC_APB1RSTR1;
    unsigned long RCC_APB1RSTR2;
    unsigned long RCC_APB2RSTR;
    unsigned long ulRes3;
    unsigned long RCC_AHB1ENR;
    unsigned long RCC_AHB2ENR;
    unsigned long RCC_AHB3ENR;
    unsigned long ulRes4;
    unsigned long RCC_APB1ENR1;
    unsigned long RCC_APB1ENR2;
    unsigned long RCC_APB2ENR;
    unsigned long ulRes5;
    unsigned long RCC_AHB1SMENR;
    unsigned long RCC_AHB2SMENR;
    unsigned long RCC_AHB3SMENR;
    unsigned long ulRes6;
    unsigned long RCC_APB1SMENR1;
    unsigned long RCC_APB1SMENR2;
    unsigned long RCC_APB2SMENR;
    unsigned long ulRes7;
    unsigned long RCC_CCIPR;
    unsigned long ulRes8;
    unsigned long RCC_BDCR;
    unsigned long RCC_CSR;
    unsigned long RCC_CRRCR;
    unsigned long RCC_CCCIPR2;
#elif defined _STM32L0x1
    unsigned long RCC_CR;
    unsigned long RCC_ICSCR;
    unsigned long ulRes0;
    unsigned long RCC_CFGR;
    unsigned long RCC_CIER;
    unsigned long RCC_CIFR;
    unsigned long RCC_CICR;
    unsigned long RCC_IOPRSTR;
    unsigned long RCC_AHBRSTR;
    unsigned long RCC_APB2RSTR;
    unsigned long RCC_APB1RSTR;
    unsigned long RCC_IOPENR;
    unsigned long RCC_AHBENR;
    unsigned long RCC_APB2ENR;
    unsigned long RCC_APB1ENR;
    unsigned long RCC_IOPSMEN;
    unsigned long RCC_AHBSMENR;
    unsigned long RCC_APB2SMENR;
    unsigned long RCC_APB1SMENR;
    unsigned long RCC_CCIPR;
    unsigned long RCC_CSR;
#else
    unsigned long RCC_CR;
    unsigned long RCC_CFGR;
    unsigned long RCC_CIR;
    unsigned long RCC_APB2RSTR;
    unsigned long RCC_APB1RSTR;
    unsigned long RCC_AHBENR;
    unsigned long RCC_APB2ENR;
    unsigned long RCC_APB1ENR;
    unsigned long RCC_BDCR;
    unsigned long RCC_CSR;
    unsigned long RCC_AHBRSTR;
    unsigned long RCC_CFGR2;
#endif
} STM32_RCC;

typedef struct stSTM32_DMA
{
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {5}
    unsigned long DMA_LISR;
    unsigned long DMA_HISR;
    unsigned long DMA_LIFCR;
    unsigned long DMA_HIFCR;
    unsigned long DMA_S0CR;
    unsigned long DMA_S0NDTR;
    unsigned long DMA_S0PAR;
    unsigned long DMA_S0M0AR;
    unsigned long DMA_S0M1AR;
    unsigned long DMA_S0FCR;
    unsigned long DMA_S1CR;
    unsigned long DMA_S1NDTR;
    unsigned long DMA_S1PAR;
    unsigned long DMA_S1M0AR;
    unsigned long DMA_S1M1AR;
    unsigned long DMA_S1FCR;
    unsigned long DMA_S2CR;
    unsigned long DMA_S2NDTR;
    unsigned long DMA_S2PAR;
    unsigned long DMA_S2M0AR;
    unsigned long DMA_S2M1AR;
    unsigned long DMA_S2FCR;
    unsigned long DMA_S3CR;
    unsigned long DMA_S3NDTR;
    unsigned long DMA_S3PAR;
    unsigned long DMA_S3M0AR;
    unsigned long DMA_S3M1AR;
    unsigned long DMA_S3FCR;
    unsigned long DMA_S4CR;
    unsigned long DMA_S4NDTR;
    unsigned long DMA_S4PAR;
    unsigned long DMA_S4M0AR;
    unsigned long DMA_S4M1AR;
    unsigned long DMA_S4FCR;
    unsigned long DMA_S5CR;
    unsigned long DMA_S5NDTR;
    unsigned long DMA_S5PAR;
    unsigned long DMA_S5M0AR;
    unsigned long DMA_S5M1AR;
    unsigned long DMA_S5FCR;
    unsigned long DMA_S6CR;
    unsigned long DMA_S6NDTR;
    unsigned long DMA_S6PAR;
    unsigned long DMA_S6M0AR;
    unsigned long DMA_S6M1AR;
    unsigned long DMA_S6FCR;
    unsigned long DMA_S7CR;
    unsigned long DMA_S7NDTR;
    unsigned long DMA_S7PAR;
    unsigned long DMA_S7M0AR;
    unsigned long DMA_S7M1AR;
    unsigned long DMA_S7FCR;
#else
    unsigned long DMA_ISR;
    unsigned long DMA_IFCR;
    unsigned long DMA_CCR1;
    unsigned long DMA_CNDTR1;
    unsigned long DMA_CPAR1;
    unsigned long DMA_CMAR1;
    unsigned long ulRes0;
    unsigned long DMA_CCR2;
    unsigned long DMA_CNDTR2;
    unsigned long DMA_CPAR2;
    unsigned long DMA_CMAR2;
    unsigned long ulRes1;
    unsigned long DMA_CCR3;
    unsigned long DMA_CNDTR3;
    unsigned long DMA_CPAR3;
    unsigned long DMA_CMAR3;
    unsigned long ulRes2;
    unsigned long DMA_CCR4;
    unsigned long DMA_CNDTR4;
    unsigned long DMA_CPAR4;
    unsigned long DMA_CMAR4;
    unsigned long ulRes3;
    unsigned long DMA_CCR5;
    unsigned long DMA_CNDTR5;
    unsigned long DMA_CPAR5;
    unsigned long DMA_CMAR5;
    unsigned long ulRes4;
    unsigned long DMA_CCR6;
    unsigned long DMA_CNDTR6;
    unsigned long DMA_CPAR6;
    unsigned long DMA_CMAR6;
    unsigned long ulRes5;
    unsigned long DMA_CCR7;
    unsigned long DMA_CNDTR7;
    unsigned long DMA_CPAR7;
    unsigned long DMA_CMAR7;
#endif
} STM32_DMA;


typedef struct stSTM32_ADC                                               // {6}
{
unsigned long ADC1_SR;
unsigned long ADC1_CR1;
unsigned long ADC1_CR2;
unsigned long ADC1_SMPR1;
unsigned long ADC1_SMPR2;
unsigned long ADC1_JOFR1;
unsigned long ADC1_JOFR2;
unsigned long ADC1_JOFR3;
unsigned long ADC1_JOFR4;
unsigned long ADC1_HTR;
unsigned long ADC1_LTR;
unsigned long ADC1_SQR1;
unsigned long ADC1_SQR2;
unsigned long ADC1_SQR3;
unsigned long ADC1_JSQR;
unsigned long ADC1_JDR1;
unsigned long ADC1_JDR2;
unsigned long ADC1_JDR3;
unsigned long ADC1_JDR4;
unsigned long ADC1_DR;
unsigned long ulRes0[0x2c];
unsigned long ADC2_SR;
unsigned long ADC2_CR1;
unsigned long ADC2_CR2;
unsigned long ADC2_SMPR1;
unsigned long ADC2_SMPR2;
unsigned long ADC2_JOFR1;
unsigned long ADC2_JOFR2;
unsigned long ADC2_JOFR3;
unsigned long ADC2_JOFR4;
unsigned long ADC2_HTR;
unsigned long ADC2_LTR;
unsigned long ADC2_SQR1;
unsigned long ADC2_SQR2;
unsigned long ADC2_SQR3;
unsigned long ADC2_JSQR;
unsigned long ADC2_JDR1;
unsigned long ADC2_JDR2;
unsigned long ADC2_JDR3;
unsigned long ADC2_JDR4;
unsigned long ADC2_DR;
unsigned long ulRes1[0x2c];
unsigned long ADC3_SR;
unsigned long ADC3_CR1;
unsigned long ADC3_CR2;
unsigned long ADC3_SMPR1;
unsigned long ADC3_SMPR2;
unsigned long ADC3_JOFR1;
unsigned long ADC3_JOFR2;
unsigned long ADC3_JOFR3;
unsigned long ADC3_JOFR4;
unsigned long ADC3_HTR;
unsigned long ADC3_LTR;
unsigned long ADC3_SQR1;
unsigned long ADC3_SQR2;
unsigned long ADC3_SQR3;
unsigned long ADC3_JSQR;
unsigned long ADC3_JDR1;
unsigned long ADC3_JDR2;
unsigned long ADC3_JDR3;
unsigned long ADC3_JDR4;
unsigned long ADC3_DR;
unsigned long ulRes2[0x2c];
unsigned long ADC_CSR;
unsigned long ADC_CCR;
unsigned long ADC_CDR;
} STM32_ADC;

typedef struct stSTM32_SDIO
{
unsigned long SDIO_POWER;
unsigned long SDIO_CLKCR;
unsigned long SDIO_ARG;
unsigned long SDIO_CMD;
unsigned long SDIO_RESPCMD;
unsigned long SDIO_RESP1;
unsigned long SDIO_RESP2;
unsigned long SDIO_RESP3;
unsigned long SDIO_RESP4;
unsigned long SDIO_DTIMER;
unsigned long SDIO_DLEN;
unsigned long SDIO_DCTRL;
unsigned long SDIO_DCOUNT;
unsigned long SDIO_STA;
unsigned long SDIO_ICR;
unsigned long SDIO_MASK;
unsigned long ulRes0[2];
unsigned long SDIO_FIFOCNT;
unsigned long ulRes1[13];
unsigned long SDIO_FIFO0;
unsigned long SDIO_FIFO1;
unsigned long SDIO_FIFO2;
unsigned long SDIO_FIFO3;
unsigned long SDIO_FIFO4;
unsigned long SDIO_FIFO5;
unsigned long SDIO_FIFO6;
unsigned long SDIO_FIFO7;
unsigned long SDIO_FIFO8;
unsigned long SDIO_FIFO9;
unsigned long SDIO_FIFO10;
unsigned long SDIO_FIFO11;
unsigned long SDIO_FIFO12;
unsigned long SDIO_FIFO13;
unsigned long SDIO_FIFO14;
unsigned long SDIO_FIFO15;
unsigned long SDIO_FIFO16;
unsigned long SDIO_FIFO17;
unsigned long SDIO_FIFO18;
unsigned long SDIO_FIFO19;
unsigned long SDIO_FIFO20;
unsigned long SDIO_FIFO21;
unsigned long SDIO_FIFO22;
unsigned long SDIO_FIFO23;
unsigned long SDIO_FIFO24;
unsigned long SDIO_FIFO25;
unsigned long SDIO_FIFO26;
unsigned long SDIO_FIFO27;
unsigned long SDIO_FIFO28;
unsigned long SDIO_FIFO29;
unsigned long SDIO_FIFO30;
unsigned long SDIO_FIFO31;
} STM32_SDIO;


typedef struct stSTM32_FMI
{
#if defined _STM32L0x1
    unsigned long FLASH_ACR;
    unsigned long FLASH_PECR;
    unsigned long FLASH_PDKEYR;
    unsigned long FLASH_PKEYR;
    unsigned long FLASH_PRGKEYR;
    unsigned long FLASH_OPTKEYR;
    unsigned long FLASH_SR;
    unsigned long FLASH_OPTR;
    unsigned long FLASH_WRPROT1;
    unsigned long ulRes0[17];
    unsigned long FLASH_WRPROT2;
#else
    unsigned long FLASH_ACR;
    unsigned long FLASH_KEYR;
    unsigned long FLASH_OPTKEYR;
    unsigned long FLASH_SR;
    unsigned long FLASH_CR;
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    unsigned long FLASH_OPTCR;
        #if defined _STM32F42X || defined _STM32F43X
    unsigned long FLASH_OPTC1;
        #endif
    #else
    unsigned long FLASH_AR;
    unsigned long ulRes0;
    unsigned long FLASH_OBR;
    unsigned long FLASH_WRPR;
        #if defined XL_DENSITY
        unsigned long FLASH_KEYR2;
        unsigned long FLASH_SR2;
        unsigned long FLASH_CR2;
        unsigned long FLASH_AR2;
        #endif
    #endif
#endif
} STM32_FMI;


typedef struct stSTM32_EXTI
{
unsigned long EXTI_IMR;
unsigned long EXTI_EMR;
unsigned long EXTI_RTSR;
unsigned long EXTI_FTSR;
unsigned long EXTI_SWIER;
unsigned long EXTI_PR;
} STM32_EXTI;


typedef struct stSTM32_GPIO
{
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    unsigned long GPIO_MODER;
    unsigned long GPIO_OTYPER;
    unsigned long GPIO_OSPEEDR;
    unsigned long GPIO_PUPDR;
    unsigned long GPIO_IDR;
    unsigned long GPIO_ODR;
    unsigned long GPIO_BSRR;
    unsigned long GPIO_LCKR;
    unsigned long GPIO_AFRL;
    unsigned long GPIO_AFRH;
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        unsigned long GPIO_BRR;
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        unsigned long  GPIO_ASCR;
    #endif
#else
    unsigned long GPIO_CRL;
    unsigned long GPIO_CRH;
    unsigned long GPIO_IDR;
    unsigned long GPIO_ODR;
    unsigned long GPIO_BSRR;
    unsigned long GPIO_BRR;
    unsigned long GPIO_LCKR;
#endif
} STM32_GPIO;


typedef struct stSTM32_AFIO
{
unsigned long AFIO_EVCR;
unsigned long AFIO_MAPR;
unsigned long AFIO_EXTICR1;
unsigned long AFIO_EXTICR2;
unsigned long AFIO_EXTICR3;
unsigned long AFIO_EXTICR4;
unsigned long AFIO_MAPR2;
} STM32_AFIO;


typedef struct stSTM32_USART
{
#if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031
    unsigned long UART_CR1;
    unsigned long UART_CR2;
    unsigned long UART_CR3;
    unsigned long UART_BRR;
    unsigned long UART_GTPR;
    unsigned long UART_RTOR;
    unsigned long UART_RQR;
    unsigned long UART_ISR;
    unsigned long UART_ICR;
    unsigned long UART_RDR;
    unsigned long UART_TDR;
#else
    unsigned long USART_SR;
    unsigned long USART_DR;
    unsigned long USART_BRR;
    unsigned long USART_CR1;
    unsigned long USART_CR2;
    unsigned long USART_CR3;
    unsigned long USART_GTPR;
#endif
} STM32_USART;


typedef struct stSTM32_UART
{
#if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    unsigned long UART_CR1;
    unsigned long UART_CR2;
    unsigned long UART_CR3;
    unsigned long UART_BRR;
    unsigned long UART_GTPR;
    unsigned long UART_RTOR;
    unsigned long UART_RQR;
    unsigned long UART_ISR;
    unsigned long UART_ICR;
    unsigned long UART_RDR;
    unsigned long UART_TDR;
#else
    unsigned long UART_SR;
    unsigned long UART_DR;
    unsigned long UART_BRR;
    unsigned long UART_CR1;
    unsigned long UART_CR2;
    unsigned long UART_CR3;
    unsigned long UART_GTPR;
#endif
} STM32_UART;

#if LPUARTS_AVAILABLE > 0
typedef struct stSTM32_LPUART
{
    unsigned long LPUART_CR1;
    unsigned long LPUART_CR2;
    unsigned long LPUART_CR3;
    unsigned long LPUART_BRR;
    unsigned long ulRes[2];
    unsigned long LPUART_RQR;
    unsigned long LPUART_ISR;
    unsigned long LPUART_ICR;
    unsigned long LPUART_RDR;
    unsigned long LPUART_TDR;
} STM32_LPUART;
#endif


typedef struct stSTM32_IWDG
{
unsigned long IWDG_KR;
unsigned long IWDG_PR;
unsigned long IWDG_RLR;
unsigned long IWDG_SR;
#if defined _STM32L432 || defined _STM32L0x1
    unsigned long  IWDG_WINR;
#endif
} STM32_IWDG;


typedef struct stSTM32_RTC
{
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {1}
    unsigned long RTC_TR;
    unsigned long RTC_DR;
    unsigned long RTC_CR;
    unsigned long RTC_ISR;
    unsigned long RTC_PRER;
    unsigned long RTC_WUTR;
    unsigned long RTC_CALIBR;
    unsigned long RTC_ALRMAR;
    unsigned long RTC_ALRMBR;
    unsigned long RTC_WPR;
    unsigned long RTC_SSR;
    unsigned long RTC_SHIFTR;
    unsigned long RTC_TSTR;
    unsigned long ulRes0;
    unsigned long RTC_TSSSR;
    unsigned long RTC_CALR;
    unsigned long RTC_TAFCR;
    unsigned long RTC_ALRMASSR;
    unsigned long RTC_ALRMBSSR;
    unsigned long ulRes1;
    unsigned long RTC_BK0R;
    unsigned long RTC_BK1R;
    unsigned long RTC_BK2R;
    unsigned long RTC_BK3R;
    unsigned long RTC_BK4R;
    unsigned long RTC_BK5R;
    unsigned long RTC_BK6R;
    unsigned long RTC_BK7R;
    unsigned long RTC_BK8R;
    unsigned long RTC_BK9R;
    unsigned long RTC_BK10R;
    unsigned long RTC_BK11R;
    unsigned long RTC_BK12R;
    unsigned long RTC_BK13R;
    unsigned long RTC_BK14R;
    unsigned long RTC_BK15R;
    unsigned long RTC_BK16R;
    unsigned long RTC_BK17R;
    unsigned long RTC_BK18R;
    unsigned long RTC_BK19R;
#else
    unsigned short RTC_CRH;
    unsigned short usRes0;
    unsigned short RTC_CRL;
    unsigned short usRes1;
    unsigned short RTC_PRLH;
    unsigned short usRes2;
    unsigned short RTC_PRLL;
    unsigned short usRes3;
    unsigned short RTC_DIVH;
    unsigned short usRes4;
    unsigned short RTC_DIVL;
    unsigned short usRes5;
    unsigned short RTC_CNTH;
    unsigned short usRes6;
    unsigned short RTC_CNTL;
    unsigned short usRes7;
    unsigned short RTC_ALRH;
    unsigned short usRes8;
    unsigned short RTC_ALRL;
#endif
} STM32_RTC;


typedef struct stSTM32_I2C
{
unsigned long I2C_CR1;
unsigned long I2C_CR2;
unsigned long I2C_OAR1;
unsigned long I2C_OAR2;
unsigned long I2C_DR;
unsigned long I2C_SR1;
unsigned long I2C_SR2;
unsigned long I2C_CCR;
unsigned long I2C_TRISE;
} STM32_I2C;


typedef struct stSTM32_TIM2_3_4_5
{
unsigned long TIM_CR1;
unsigned long TIM_CR2;
unsigned long TIM_SMCR;
unsigned long TIM_DIER;
unsigned long TIM_SR;
unsigned long TIM_EGR;
unsigned long TIM_CCMR1;
unsigned long TIM_CCMR2;
unsigned long TIM_CCER;
unsigned long TIM_CNT;
unsigned long TIM_PSC;
unsigned long TIM_ARR;
unsigned long ulRes0;
unsigned long TIM_CCR1;
unsigned long TIM_CCR2;
unsigned long TIM_CCR3;
unsigned long TIM_CCR4;
unsigned long ulRes2;
unsigned long TIM_DCR;
unsigned long TIM_DMAR;
} STM32_TIM2_3_4_5;

typedef struct stSTM32_TIM0_10_11_12_13_14
{
unsigned long TIM_CR1;
unsigned long ulRes0;
unsigned long TIM_SMCR;
unsigned long TIM_DIER;
unsigned long TIM_SR;
unsigned long TIM_EGR;
unsigned long TIM_CCMR1;
unsigned long ulRes1;
unsigned long TIM_CCER;
unsigned long TIM_CNT;
unsigned long TIM_PSC;
unsigned long TIM_ARR;
unsigned long ulRes2;
unsigned long TIM_CCR1;
unsigned long ulRes3[6];
unsigned long TIM11OR;                                                   // only timer 11
} STM32_TIM9_10_11_12_13_14;

typedef struct stSTM32_USBD
{
unsigned long USB_EP0R;
unsigned long USB_EP1R;
unsigned long USB_EP2R;
unsigned long USB_EP3R;
unsigned long USB_EP4R;
unsigned long USB_EP5R;
unsigned long USB_EP6R;
unsigned long USB_EP7R;
unsigned long res1[8];
unsigned long USB_CNTR;
unsigned long USB_ISTR;
unsigned long USB_FNR;
unsigned long USB_DADDR;
unsigned long USB_BTABLE;
} STM32_USBD;

typedef struct stSTM32_PWR
{
unsigned long PWR_CR;
} STM32_PWR;

typedef struct stSTM32_CAN
{
unsigned long CAN_MCR;
unsigned long CAN_MSR;
unsigned long CAN_TSR;
unsigned long CAN_RF0R;
unsigned long CAN_RF1R;
unsigned long CAN_IER;
unsigned long CAN_ESR;
unsigned long CAN_BTR;
unsigned long ulRes0[88];
unsigned long CAN_TI0R;
unsigned long CAN_TDT0R;
unsigned long CAN_TDL0R;
unsigned long CAN_TDH0R;
unsigned long CAN_TI1R;
unsigned long CAN_TDT1R;
unsigned long CAN_TDL1R;
unsigned long CAN_TDH1R;
unsigned long CAN_TI2R;
unsigned long CAN_TDT2R;
unsigned long CAN_TDL2R;
unsigned long CAN_TDH2R;
unsigned long CAN_RI0R;
unsigned long CAN_RDT0R;
unsigned long CAN_RDL0R;
unsigned long CAN_RDH0R;
unsigned long CAN_RI1R;
unsigned long CAN_RDT1R;
unsigned long CAN_RDL1R;
unsigned long CAN_RDH1R;
unsigned long ulRes1[12];
unsigned long CAN_FMR;
unsigned long CAN_FM1R;
unsigned long ulRes2;
unsigned long CAN_FS1R;
unsigned long ulRes3;
unsigned long CAN_FFA1R;
unsigned long ulRes4;
unsigned long CAN_FA1R;
unsigned long ulRes5[8];
unsigned long CAN_F0R1;
unsigned long CAN_F0R2;
unsigned long CAN_F1R1;
unsigned long CAN_F1R2;
unsigned long CAN_F2R1;
unsigned long CAN_F2R2;
unsigned long CAN_F3R1;
unsigned long CAN_F3R2;
unsigned long CAN_F4R1;
unsigned long CAN_F4R2;
unsigned long CAN_F5R1;
unsigned long CAN_F5R2;
unsigned long CAN_F6R1;
unsigned long CAN_F6R2;
unsigned long CAN_F7R1;
unsigned long CAN_F7R2;
unsigned long CAN_F8R1;
unsigned long CAN_F8R2;
unsigned long CAN_F9R1;
unsigned long CAN_F9R2;
unsigned long CAN_F10R1;
unsigned long CAN_F10R2;
unsigned long CAN_F11R1;
unsigned long CAN_F11R2;
unsigned long CAN_F12R1;
unsigned long CAN_F12R2;
unsigned long CAN_F13R1;
unsigned long CAN_F13R2;
unsigned long CAN_F14R1;
unsigned long CAN_F14R2;
unsigned long CAN_F15R1;
unsigned long CAN_F15R2;
unsigned long CAN_F16R1;
unsigned long CAN_F16R2;
unsigned long CAN_F17R1;
unsigned long CAN_F17R2;
unsigned long CAN_F18R1;
unsigned long CAN_F18R2;
unsigned long CAN_F19R1;
unsigned long CAN_F19R2;
unsigned long CAN_F20R1;
unsigned long CAN_F20R2;
unsigned long CAN_F21R1;
unsigned long CAN_F21R2;
unsigned long CAN_F22R1;
unsigned long CAN_F22R2;
unsigned long CAN_F23R1;
unsigned long CAN_F23R2;
unsigned long CAN_F24R1;
unsigned long CAN_F24R2;
unsigned long CAN_F25R1;
unsigned long CAN_F25R2;
unsigned long CAN_F26R1;
unsigned long CAN_F26R2;
unsigned long CAN_F27R1;
unsigned long CAN_F27R2;
} STM32_CAN;

typedef struct stSTM32_CAN_SLAVE
{
unsigned long CAN_MCR;
unsigned long CAN_MSR;
unsigned long CAN_TSR;
unsigned long CAN_RF0R;
unsigned long CAN_RF1R;
unsigned long CAN_IER;
unsigned long CAN_ESR;
unsigned long CAN_BTR;
unsigned long ulRes0[88];
unsigned long CAN_TI0R;
unsigned long CAN_TDT0R;
unsigned long CAN_TDL0R;
unsigned long CAN_TDH0R;
unsigned long CAN_TI1R;
unsigned long CAN_TDT1R;
unsigned long CAN_TDL1R;
unsigned long CAN_TDH1R;
unsigned long CAN_TI2R;
unsigned long CAN_TDT2R;
unsigned long CAN_TDL2R;
unsigned long CAN_TDH2R;
unsigned long CAN_RI0R;
unsigned long CAN_RDT0R;
unsigned long CAN_RDL0R;
unsigned long CAN_RDH0R;
unsigned long CAN_RI1R;
unsigned long CAN_RDT1R;
unsigned long CAN_RDL1R;
unsigned long CAN_RDH1R;
unsigned long ulRes1[12];
unsigned long CAN_FMR;
unsigned long CAN_FM1R;
unsigned long ulRes2;
unsigned long CAN_FS1R;
unsigned long ulRes3;
unsigned long CAN_FFA1R;
unsigned long ulRes4;
unsigned long CAN_FA1R;
} STM32_CAN_SLAVE;


typedef struct stSTM32_SPI
{
unsigned short SPI1_CR1;
unsigned short usRes1;
unsigned short SPI1_CR2;
unsigned short usRes2;
unsigned short SPI1_SR;
unsigned short usRes3;
unsigned short SPI1_DR;
unsigned short usRes4;
unsigned short SPI1_CRCPR;
unsigned short usRes5;
unsigned short SPI1_RXCRCR;
unsigned short usRes6;
unsigned short SPI1_TXCRCR; 
} STM32_SPI;


typedef struct stSTM32_SPI_I2S
{
unsigned short SPI2_CR1;
unsigned short usRes1;
unsigned short SPI2_CR2;
unsigned short usRes2;
unsigned short SPI2_SR;
unsigned short usRes3;
unsigned short SPI2_DR;
unsigned short usRes4;
unsigned short SPI2_CRCPR;
unsigned short usRes5;
unsigned short SPI2_RXCRCR;
unsigned short usRes6;
unsigned short SPI2_TXCRCR;
unsigned short usRes7;
unsigned short SPI2_I2SCFGR;
unsigned short usRes8;
unsigned short SPI2_I2SPR;
} STM32_SPI_I2S;


#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
typedef struct stSTM32_SYSCFG
{
unsigned long SYSCFG_MEMRM;
unsigned long SYSCFG_PMC;
unsigned long SYSCFG_EXTICR1;
unsigned long SYSCFG_EXTICR2;
unsigned long SYSCFG_EXTICR3;
unsigned long SYSCFG_EXTICR4;
unsigned long ulRes0[2];
unsigned long SYSCFG_CMPCR;
} STM32_SYSCFG;

typedef struct stSTM32_FSMC
{
unsigned long FSMC_BCR1;
unsigned long FSMC_BTR1;
unsigned long FSMC_BCR2;
unsigned long FSMC_BTR2;
unsigned long FSMC_BCR3;
unsigned long FSMC_BTR3;
unsigned long FSMC_BCR4;
unsigned long FSMC_BTR4;
unsigned long ulRes0[16];
unsigned long FSMC_PCR2;
unsigned long FSMC_PSR2;
unsigned long FSMC_PMEM2;
unsigned long FSMC_PATT2;
unsigned long ulRes1;
unsigned long FSMC_ECCR2;
unsigned long ulRes2[2];
unsigned long FSMC_PCR3;
unsigned long FSMC_PSR3;
unsigned long FSMC_PMEM3;
unsigned long FSMC_PATT3;
unsigned long ulRes3;
unsigned long FSMC_ECCR3;
unsigned long ulRes4[2];
unsigned long FSMC_PCR4;
unsigned long FSMC_PSR4;
unsigned long FSMC_PMEM4;
unsigned long FSMC_PATT4;
unsigned long FSMC_PIO4;
unsigned long ulRes5[20];
unsigned long FSMC_BWTR1;
unsigned long ulRes6;
unsigned long FSMC_BWTR2;
unsigned long ulRes7;
unsigned long FSMC_BWTR3;
unsigned long ulRes8;
unsigned long FSMC_BWTR4;
} STM32_FSMC;
#endif

#if defined _STM32F7XX
typedef struct stSTM32_QSPI
{
    unsigned long QUADSPI_CR;
    unsigned long QUADSPI_DCR;
    unsigned long QUADSPI_SR;
    unsigned long QUADSPI_FCR;
    unsigned long QUADSPI_DLR;
    unsigned long QUADSPI_CCR;
    unsigned long QUADSPI_AR;
    unsigned long QUADSPI_ABR;
    unsigned long QUADSPI_DR;
    unsigned long QUADSPI_PSMKR;
    unsigned long QUADSPI_PSMAR;
    unsigned long QUADSPI_PIR;
    unsigned long QUADSPI_LPTR;
} STM32_QSPI;
#endif

typedef struct stSTM32_USB_OTG_FS                                        // {4}
{
unsigned long OTG_FS_GOTGCTL;
unsigned long OTG_FS_GOTGINT;
unsigned long OTG_FS_GAHBCFG;
unsigned long OTG_FS_GUSBCFG;
unsigned long OTG_FS_GRSTCTL;
unsigned long OTG_FS_GGINTSTS;
unsigned long OTG_FS_GINTMSK;
unsigned long OTG_FS_GRXSTSR;
unsigned long OTG_FS_GRXSTSPR;
unsigned long OTG_FS_GRXFSIZ;
unsigned long OTG_FS_DIEPTXF0;
unsigned long OTG_FS_HNPTXSTS;
unsigned long ulRes0[2];
unsigned long OTG_FS_GCCFG;
unsigned long OTG_FS_CID;
unsigned long ulRes1[48];
unsigned long OTG_FS_HPTXFSIZ;
unsigned long OTG_FS_DIEPTXF1;
unsigned long OTG_FS_DIEPTXF2;
unsigned long OTG_FS_DIEPTXF3;
unsigned long ulRes2[188];
unsigned long OTG_FS_HCFG;
unsigned long OTG_FS_HFIR;
unsigned long OTG_FS_HFNUM;
unsigned long ulRes3;
unsigned long OTG_FS_HPTXSTS;
unsigned long OTG_FS_HAINT;
unsigned long OTG_FS_HAINTMSK;
unsigned long ulRes4[9];
unsigned long OTG_FS_HPRT;
unsigned long ulRes5[47];
unsigned long OTG_FS_HCCHAR0;
unsigned long ulRes6;
unsigned long OTG_FS_HCINT0;
unsigned long OTG_FS_HCINTMSK0;
unsigned long OTG_FS_HCTSIZ0;
unsigned long ulRes7[3];
unsigned long OTG_FS_HCCHAR1;
unsigned long ulRes8;
unsigned long OTG_FS_HCINT1;
unsigned long OTG_FS_HCINTMSK1;
unsigned long OTG_FS_HCTSIZ1;
unsigned long ulRes9[3];
unsigned long OTG_FS_HCCHAR2;
unsigned long ulRes10;
unsigned long OTG_FS_HCINT2;
unsigned long OTG_FS_HCINTMSK2;
unsigned long OTG_FS_HCTSIZ2;
unsigned long ulRes11[3];
unsigned long OTG_FS_HCCHAR3;
unsigned long ulRes12;
unsigned long OTG_FS_HCINT3;
unsigned long OTG_FS_HCINTMSK3;
unsigned long OTG_FS_HCTSIZ3;
unsigned long ulRes13[3];
unsigned long OTG_FS_HCCHAR4;
unsigned long ulRes14;
unsigned long OTG_FS_HCINT4;
unsigned long OTG_FS_HCINTMSK4;
unsigned long OTG_FS_HCTSIZ4;
unsigned long ulRes15[3];
unsigned long OTG_FS_HCCHAR5;
unsigned long ulRes16;
unsigned long OTG_FS_HCINT5;
unsigned long OTG_FS_HCINTMSK5;
unsigned long OTG_FS_HCTSIZ5;
unsigned long ulRes17[3];
unsigned long OTG_FS_HCCHAR6;
unsigned long ulRes18;
unsigned long OTG_FS_HCINT6;
unsigned long OTG_FS_HCINTMSK6;
unsigned long OTG_FS_HCTSIZ6;
unsigned long ulRes19[3];
unsigned long OTG_FS_HCCHAR7;
unsigned long ulRes20;
unsigned long OTG_FS_HCINT7;
unsigned long OTG_FS_HCINTMSK7;
unsigned long OTG_FS_HCTSIZ7;
unsigned long ulRes21[131];
unsigned long OTG_FS_DCFG;
unsigned long OTG_FS_DCTL;
unsigned long OTG_FS_DSTS;
unsigned long ulRes22;
unsigned long OTG_FS_DIEPMSK;
unsigned long OTG_FS_DOEPMSK;
unsigned long OTG_FS_DAINT;
unsigned long OTG_FS_DAINTMSK;
unsigned long ulRes23[2];
unsigned long OTG_FS_DVBUSDIS;
unsigned long OTG_FS_DVBUSPULSE;
unsigned long ulRes24;
unsigned long OTG_FS_DIEPEMPMSK;
unsigned long ulRes25[50];
unsigned long OTG_FS_DIEPCTL0;
unsigned long ulRes26;
unsigned long OTG_FS_DIEPINT0;
unsigned long ulRes27;
unsigned long OTG_FS_DIEPTSIZ0;
unsigned long ulRes28;
unsigned long OTG_FS_DTXFSTS0;
unsigned long ulRes29;
unsigned long OTG_FS_DIEPCTL1;
unsigned long ulRes30;
unsigned long OTG_FS_DIEPINT1;
unsigned long ulRes31;
unsigned long OTG_FS_DIEPTSIZ1;
unsigned long ulRes32;
unsigned long OTG_FS_DTXFSTS1;
unsigned long ulRes33;
unsigned long OTG_FS_DIEPCTL2;
unsigned long ulRes34;
unsigned long OTG_FS_DIEPINT2;
unsigned long ulRes35;
unsigned long OTG_FS_DIEPTSIZ2;
unsigned long ulRes36;
unsigned long OTG_FS_DTXFSTS2;
unsigned long ulRes37;
unsigned long OTG_FS_DIEPCTL3;
unsigned long ulRes38;
unsigned long OTG_FS_DIEPINT3;
unsigned long ulRes39;
unsigned long OTG_FS_DIEPTSIZ3;
unsigned long ulRes40;
unsigned long OTG_FS_DTXFSTS3;
unsigned long ulRes41[97];
unsigned long OTG_FS_DOEPCTL0;
unsigned long ulRes42;
unsigned long OTG_FS_DOEPINT0;
unsigned long ulRes43;
unsigned long OTG_FS_DOEPTSIZ0;
unsigned long ulRes44[3];
unsigned long OTG_FS_DOEPCTL1;
unsigned long ulRes45;
unsigned long OTG_FS_DOEPINT1;
unsigned long ulRes46;
unsigned long OTG_FS_DOEPTSIZ1;
unsigned long ulRes47[3];
unsigned long OTG_FS_DOEPCTL2;
unsigned long ulRes48;
unsigned long OTG_FS_DOEPINT2;
unsigned long ulRes49;
unsigned long OTG_FS_DOEPTSIZ2;
unsigned long ulRes50[3];
unsigned long OTG_FS_DOEPCTL3;
unsigned long ulRes51;
unsigned long OTG_FS_DOEPINT3;
unsigned long ulRes52;
unsigned long OTG_FS_DOEPTSIZ3;
unsigned long ulRes53[163];
unsigned long OTG_FS_PCGCCTL;
unsigned long ulRes54[127];
unsigned char OTG_FS_DFIFO0[0x1000];
unsigned char OTG_FS_DFIFO2[0x1000];
unsigned char OTG_FS_DFIFO3[0x1000];
unsigned char OTG_FS_DFIFO4[0x1000];
} STM32_USB_OTG_FS;


typedef struct stSTM32_DBG
{
unsigned long DBGMCU_IDCODE;
unsigned long DBGMCU_CR;
} STM32_DBG;


typedef struct stSTM32_CORTEX_M3
{
    unsigned long ulRes1;
    const unsigned long INT_CONT_TYPE;
    unsigned long ulRes2[2];
    unsigned long SYSTICK_CSR;
    unsigned long SYSTICK_RELOAD;
    unsigned long SYSTICK_CURRENT;
    const unsigned long SYSTICK_CALIB;
    unsigned long ulRes3[56];
    unsigned long IRQ0_31_SER;   
    unsigned long IRQ32_63_SER;  
    unsigned long IRQ64_95_SER;  
    unsigned long IRQ96_127_SER; 
    unsigned long IRQ128_159_SER;
    unsigned long IRQ160_191_SER;
    unsigned long IRQ192_223_SER;
    unsigned long IRQ224_239_SER;
    unsigned long ulRes4[24];
    unsigned long IRQ0_31_CER;
    unsigned long IRQ32_63_CER;  
    unsigned long IRQ64_95_CER;
    unsigned long IRQ96_127_CER;
    unsigned long IRQ128_159_CER;
    unsigned long IRQ160_191_CER;
    unsigned long IRQ192_223_CER;
    unsigned long IRQ224_239_CER;
    unsigned long ulRes5[24];
    unsigned long IRQ0_31_SPR;
    unsigned long IRQ32_63_SPR;
    unsigned long IRQ64_95_SPR;
    unsigned long IRQ96_127_SPR;
    unsigned long IRQ128_159_SPR;
    unsigned long IRQ160_191_SPR;
    unsigned long IRQ192_223_SPR;
    unsigned long IRQ224_239_SPR;
    unsigned long ulRes6[24];
    unsigned long IRQ0_31_CPR;
    unsigned long IRQ32_63_CPR;
    unsigned long IRQ64_95_CPR;
    unsigned long IRQ96_127_CPR;
    unsigned long IRQ128_159_CPR;
    unsigned long IRQ160_191_CPR;
    unsigned long IRQ192_223_CPR;
    unsigned long IRQ224_239_CPR;
    unsigned long ulRes7[24];
    unsigned long IRQ0_31_ABR;
    unsigned long IRQ32_63_ABR;
    unsigned long IRQ64_95_ABR;
    unsigned long IRQ96_127_ABR;
    unsigned long IRQ128_159_ABR;
    unsigned long IRQ160_191_ABR;
    unsigned long IRQ192_223_ABR;
    unsigned long IRQ224_239_ABR;
    unsigned long ulRes8[56];
    unsigned long IRQ0_3_PRIORITY_REGISTER;
    unsigned long IRQ4_7_PRIORITY_REGISTER;
    unsigned long IRQ8_11_PRIORITY_REGISTER;
    unsigned long IRQ12_15_PRIORITY_REGISTER;
    unsigned long IRQ16_19_PRIORITY_REGISTER;
    unsigned long IRQ20_23_PRIORITY_REGISTER;
    unsigned long IRQ24_27_PRIORITY_REGISTER;
    unsigned long IRQ28_31_PRIORITY_REGISTER;
    unsigned long IRQ32_35_PRIORITY_REGISTER;
    unsigned long IRQ36_39_PRIORITY_REGISTER;
    unsigned long IRQ40_43_PRIORITY_REGISTER;
    unsigned long IRQ44_47_PRIORITY_REGISTER;
    unsigned long IRQ48_51_PRIORITY_REGISTER;
    unsigned long IRQ52_55_PRIORITY_REGISTER;
    unsigned long IRQ56_59_PRIORITY_REGISTER;
    unsigned long IRQ60_63_PRIORITY_REGISTER;
    unsigned long IRQ64_67_PRIORITY_REGISTER;
    unsigned long IRQ68_71_PRIORITY_REGISTER;
    unsigned long IRQ72_75_PRIORITY_REGISTER;
    unsigned long IRQ76_79_PRIORITY_REGISTER;
    unsigned long IRQ80_83_PRIORITY_REGISTER;
    unsigned long IRQ84_87_PRIORITY_REGISTER;
    unsigned long IRQ88_91_PRIORITY_REGISTER;
    unsigned long IRQ92_95_PRIORITY_REGISTER;
    unsigned long IRQ96_99_PRIORITY_REGISTER;
    unsigned long IRQ100_103_PRIORITY_REGISTER;
    unsigned long IRQ104_107_PRIORITY_REGISTER;
    unsigned long IRQ108_111_PRIORITY_REGISTER;
    unsigned long IRQ112_115_PRIORITY_REGISTER;
    unsigned long IRQ116_119_PRIORITY_REGISTER;
    unsigned long IRQ120_123_PRIORITY_REGISTER;
    unsigned long IRQ124_127_PRIORITY_REGISTER;
    unsigned long IRQ128_131_PRIORITY_REGISTER;
    unsigned long IRQ132_135_PRIORITY_REGISTER;
    unsigned long IRQ136_139_PRIORITY_REGISTER;
    unsigned long IRQ140_143_PRIORITY_REGISTER;
    unsigned long IRQ144_147_PRIORITY_REGISTER;
    unsigned long IRQ148_151_PRIORITY_REGISTER;
    unsigned long IRQ152_155_PRIORITY_REGISTER;
    unsigned long IRQ156_159_PRIORITY_REGISTER;
    unsigned long IRQ160_163_PRIORITY_REGISTER;
    unsigned long IRQ164_167_PRIORITY_REGISTER;
    unsigned long IRQ168_171_PRIORITY_REGISTER;
    unsigned long IRQ172_175_PRIORITY_REGISTER;
    unsigned long IRQ176_179_PRIORITY_REGISTER;
    unsigned long IRQ180_183_PRIORITY_REGISTER;
    unsigned long IRQ184_187_PRIORITY_REGISTER;
    unsigned long IRQ188_191_PRIORITY_REGISTER;
    unsigned long IRQ192_195_PRIORITY_REGISTER;
    unsigned long IRQ196_199_PRIORITY_REGISTER;
    unsigned long IRQ200_203_PRIORITY_REGISTER;
    unsigned long IRQ204_207_PRIORITY_REGISTER;
    unsigned long IRQ208_211_PRIORITY_REGISTER;
    unsigned long IRQ212_215_PRIORITY_REGISTER;
    unsigned long IRQ216_219_PRIORITY_REGISTER;
    unsigned long IRQ220_223_PRIORITY_REGISTER;
    unsigned long IRQ224_227_PRIORITY_REGISTER;
    unsigned long IRQ228_231_PRIORITY_REGISTER;
    unsigned long IRQ232_235_PRIORITY_REGISTER;
    unsigned long IRQ236_239_PRIORITY_REGISTER;
    unsigned long ulRes9[516];
    unsigned long CPUID_BASE_REGISTER;
    unsigned long INT_CONT_STATE_REG;
    unsigned long VECTOR_TABLE_OFFSET_REG;
    unsigned long APPLICATION_INT_RESET_CTR_REG;
    unsigned long SYSTEM_CONTROL_REGISTER;
    unsigned long CONFIGURATION_CONTROL_REGISTER;
    unsigned long SYSTEM_HANDLER_4_7_PRIORITY_REGISTER;                  // {2}
    unsigned long SYSTEM_HANDLER_8_11_PRIORITY_REGISTER;
    unsigned long SYSTEM_HANDLER_12_15_PRIORITY_REGISTER;
    unsigned long ulRes10[25];
    unsigned long CPACR;
#if defined STM32_FPU
    unsigned long ulRes11[106];
    unsigned long FPCCR;
#endif
} STM32_CORTEX_M3;

#if defined ARM_MATH_CM4 || defined ARM_MATH_CM7                         // {40}
typedef struct stSTM32_CORTEX_M4_DEBUG
{
    unsigned long DHCSR;
    unsigned long DCRSR;
    unsigned long DCRDR;
    unsigned long DEMCR;
} STM32_CORTEX_M4_DEBUG;

typedef struct stSTM32_CORTEX_M4_TRACE
{
    unsigned long DWT_CTRL;
    unsigned long DWT_CYCCNT;
    unsigned long DWT_CPICNT;
    unsigned long DWT_EXCCNT;
    unsigned long DWT_SLEEPVNT;
    unsigned long DWT_LSUCNT;
    unsigned long DWT_FOLDCNT;
    unsigned long DWT_PCSR;
    unsigned long DWT_COMP0;
    unsigned long DWT_MASK0;
    unsigned long DWT_FUNCTION0;
    unsigned long ulRes0;
    unsigned long DWT_COMP1;
    unsigned long DWT_MASK1;
    unsigned long DWT_FUNCTION1;
    unsigned long ulRes1;
    unsigned long DWT_COMP2;
    unsigned long DWT_MASK2;
    unsigned long DWT_FUNCTION2;
    unsigned long ulRes2;
    unsigned long DWT_COMP3;
    unsigned long DWT_MASK3;
    unsigned long DWT_FUNCTION3;
    #if defined ARM_MATH_CM7
    unsigned long ulRes3[981];
    unsigned long DWT_LAR;
    unsigned long DWT_LSR;
    unsigned long ulRes4[6];
    #else
    unsigned long ulRes3[989];
    #endif
    unsigned long DWT_PID4;
    unsigned long DWT_PID5;
    unsigned long DWT_PID6;
    unsigned long DWT_PID7;
    unsigned long DWT_PID0;
    unsigned long DWT_PID1;
    unsigned long DWT_PID2;
    unsigned long DWT_PID3;
    unsigned long DWT_CID0;
    unsigned long DWT_CID1;
    unsigned long DWT_CID2;
    unsigned long DWT_CID3;
} STM32_CORTEX_M4_TRACE;
#endif

typedef struct stSTM32_CORTEX_M3_REGS
{
    unsigned long ulR0;                                                  // low registers
    unsigned long ulR1;
    unsigned long ulR2;
    unsigned long ulR3;
    unsigned long ulR4;
    unsigned long ulR5;
    unsigned long ulR6;
    unsigned long ulR7;
    unsigned long ulR8;                                                  // high registers
    unsigned long ulR9;
    unsigned long ulR10;
    unsigned long ulR11;
    unsigned long ulR12;
    unsigned long ulR13_SP;                                              // stack pointer
    unsigned long ulPSP;                                                 // banked versions of stack pointer
    unsigned long ulMSP;
    unsigned long ulR14_LR;                                              // link register
    unsigned long ulR15_PC;                                              // program counter
                                                                         // Special registers
                                                                         //
    unsigned long ulPSR;                                                 // program status register
    unsigned long ulPRIMASK;
    unsigned long ulFAULTMASK;
    unsigned long ulBASEPRI;
    unsigned long ulCONTROL;                                             // control
} STM32_CORTEX_M3_REGS;

#define INTERRUPT_MASKED 0x00000001

// A copy of the STM32 peripheral registers for simulation purposes
// (each peripheral block is identical to reality but the ordering and position of blocks are generally not important)
//
typedef struct stSTR32M_PERIPH
{
    STM32_ETH            ETH;
    STM32_RCC            RCC;
    STM32_DMA            DMAC[2];
    STM32_ADC            ADC;                                            // {6}
    STM32_SDIO           SDIO;
    STM32_FMI            FMI;
    STM32_EXTI           EXTI;
    STM32_GPIO           Ports[9];
#if !defined _STM32F2XX && !defined _STM32F4XX && !defined _STM32F7XX
    STM32_AFIO           AFIO;
#endif
    STM32_USART          USART[USARTS_AVAILABLE];
#if UARTS_AVAILABLE > 0
    STM32_UART           UART[UARTS_AVAILABLE];
#endif
#if LPUARTS_AVAILABLE > 0
    STM32_LPUART         LPUART[LPUARTS_AVAILABLE];
#endif
    STM32_IWDG           IWDG;
    STM32_RTC            RTC;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {3}
    STM32_I2C            I2C[3];
#else
    STM32_I2C            I2C[2];
#endif
    STM32_TIM2_3_4_5     TIM2_3_4_5[4];
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    STM32_TIM9_10_11_12_13_14 TIM9_10_11_12_13_14[6];
#endif
#if defined USB_DEVICE_AVAILABLE
    unsigned long        USB_CAN_SRAM[USB_CAN_SRAM_SIZE];
    STM32_USBD           USBD;
#endif
    STM32_PWR            PWR;
    STM32_CAN            CAN;
    STM32_CAN_SLAVE      CAN_SLAVE;

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    STM32_SPI_I2S        SPI_I2S[3];
    STM32_SYSCFG         SYSCFG;
    STM32_FSMC           FSMC;
#else
    STM32_SPI            SPI;
    STM32_SPI_I2S        SPI_I2S[2];
#endif
#if defined _STM32F7XX
    STM32_QSPI           QUADSPI;
#endif
#if defined USB_OTG_AVAILABLE
    STM32_USB_OTG_FS     USB_OTG_FS;                                     // {4}
#endif
    STM32_DBG            DBG;

    STM32_CORTEX_M3      CORTEX_M3;

#if defined ARM_MATH_CM4 || defined ARM_MATH_CM7                         // {40}
    STM32_CORTEX_M4_DEBUG  CORTEX_M4_DEBUG;
    STM32_CORTEX_M4_TRACE  CORTEX_M4_TRACE;
#endif

    STM32_CORTEX_M3_REGS CORTEX_M3_REGS;                                 // only for simulation use
} STM32M_PERIPH;


extern STM32M_PERIPH  STM32;

#define _PORTA                          0                                // references for display in simulator
#define _PORTB                          1
#define _PORTC                          2
#define _PORTD                          3
#define _PORTE                          4
#define _PORTF                          5
#define _PORTG                          6
#define _PORTH                          7
#define _PORTI                          8
#define _PORTJ                          9
#define _PORTK                          10

#define _ADDITIONAL_FUNCTION            16