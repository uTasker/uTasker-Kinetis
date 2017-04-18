/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetisSim.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    04.03.2012 Add NAND Flash controller                                 {1}
    18.03.2012 Add ADC                                                   {2}
    07.04.2012 Extend DMA simulation to multiple channels and include interrupt {3}
    08.04.2012 UART TX/RX DMA simulation                                 {4}
    17.04.2012 Add touch port references                                 {5}
    18.06.2012 Add CRC initialisation                                    {6}
    13.08.2012 Add 64 bytes of program once memory                       {7}
    09.10.2012 Add external ports                                        {8}
    27.10.2012 Change ADC port reference when varying value              {9}
    29.11.2012 Synchronise GPIOx_PDIR state with output values           {10}
    14.01.2013 Correct present leap year days                            {11}
    04.02.2013 Add USBHS                                                 {12}
    07.02.2013 Ignore USB simulation input when the USB controller is not enabled {13}
    24.05.2013 Add DDR1/2/LP SDRAM controller                            {14}
    29.05.2013 Add MCM                                                   {15}
    25.07.2013 Add PDB                                                   {16}
    06.08.2013 Add IMMEDIATE_MEMORY_ALLOCATION compatibility of fnStartTelnet() use {17}
    06.08.2013 Add free running DMA Rx UART support                      {18}
    04.10.2013 Allow ports to be read and port interrupts to operate on them when not configured as GPIO {19}
    27.10.2013 Add ADC triggering by PDB                                 {20}
    26.11.2013 Correct flash address checking                            {21}
    01.12.2013 Drop Ethernet reception frame if all buffer descriptors are in use {22}
    26.12.2013 Add RANDOM_NUMBER_GENERATOR_B for devices without RNGA    {23}
    25.01.2014 Add Kinetis KL support                                    {24}
    02.02.2014 Add USB host mode                                         {25}
    15.02.2014 Add mask revision                                         {26}
    18.02.2014 Correct RTC date/time priming                             {27}
    31.03.2014 Add watchdog clock prescaler                              {28}
    05.04.2014 Add FlexTimer interrupts                                  {29}
    17.04.2014 Add RTC seconds interrupt support                         {30}
    20.04.2014 Define KL25 disabled ports                                {31}
    06.05.2014 Add KL DMA based DMA simulation                           {32}
    11.05.2014 Add FlexNVM data flash support                            {33}
    08.06.2014 Add Crossbar Switch                                       {34}
    15.06.2014 Add LPTMR                                                 {35}
    17.06.2014 Extend K part DMA to respect short word and long word transfer sizes {36}
    25.06.2014 Add hardware operating details to simulator status bar    {37}
    22.07.2014 Respect TPM clock source selections                       {38}
    19.08.2014 Respect clock divider registers when displaying clock rates {39}
    14.05.2015 Add ADC compare function                                  {40}
    17.01.2016 Add fnResetENET()                                         {41}
    02.02.2017 Adapt for us tick resolution
    24.03.2017 Reset all host endpoints data frame types                 {42}

*/  
                          

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined _KINETIS

#if defined CAN_INTERFACE && defined SIM_KOMODO
    #include "..\..\WinSim\Komodo\komodo.h" 
#endif

#define PROGRAM_ONCE_AREA_SIZE   64                                      // 64 bytes in 8 blocks of 8 bytes

KINETIS_PERIPH  kinetis = {0};

unsigned char uninitialisedRAM[16];

unsigned long vector_ram[(sizeof(VECTOR_TABLE))/sizeof(unsigned long)]; // long word aligned

#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    extern unsigned long fnGetExtPortFunction(int iExtPortReference);
    extern unsigned long fnGetExtPortState(int iExtPortReference);
#endif

static unsigned long ulPort_in_A, ulPort_in_B, ulPort_in_C, ulPort_in_D, ulPort_in_E;
#if PORTS_AVAILABLE > 5
    static unsigned long ulPort_in_F;
#endif

#if defined KINETIS_KE
    static unsigned char ucPortFunctions[PORTS_AVAILABLE_8_BIT][PORT_WIDTH] = {0};
#else
    static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = {0};
#endif
static unsigned long ulPeripherals[PORTS_AVAILABLE] = {0};
#if defined SUPPORT_ADC
    static unsigned short usADC_values[ADC_CHANNELS * ADC_CONTROLLERS];
    static void *ptrPorts[2] = {(void *)ucPortFunctions, (void *)usADC_values}; // {2}
#endif

static const unsigned long ulDisabled[PORTS_AVAILABLE] = {
#if defined KINETIS_K00
    0x00003020,                                                          // port A disabled default pins
    0x000f0000,                                                          // port B disabled default pins
    0x00000c30,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x00000003                                                           // port E disabled default pins
#elif defined KINETIS_K10
    0x3f01ce60,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K20
    #if defined KINETIS_K22
    0x3f01ce60,                                                          // port A disabled default pins
    0x00ff8300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
    #elif PIN_COUNT == PIN_COUNT_64_PIN
    0x00003020,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x00000830,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0xffffffff                                                           // port E disabled default pins
    #else
    0x3f01ce60,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
    #endif
#elif defined KINETIS_K30
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K40
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K50
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K51
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K52
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K53
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K64
    0x3f01ce60,                                                          // port A disabled default pins
    0x00ff3300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x00007f9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K60
    0x3f01ce40,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K61
    0x00000020,                                                          // port A disabled default pins
    0x00c00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x000004f0,                                                          // port E disabled default pins
    0x001ff900                                                           // port F disabled default pins
#elif defined KINETIS_K64
    0x3f01ce40,                                                          // port A disabled default pins
    0x00ff0300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K70
    0x00000020,                                                          // port A disabled default pins
    0x00c00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x000004f0,                                                          // port E disabled default pins
    0x001ff900                                                           // port F disabled default pins
#elif defined KINETIS_K80
    0x3f01ce40,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_KE                                                 // not used by KE
    0x00000000,
    #if PORTS_AVAILABLE > 1
    0x00000000
    #endif
#elif defined KINETIS_KL                                                 // {24}
    #if defined KINETIS_KL02
    0x00002c60,                                                          // port A disabled default pins
    0x000030d8,                                                          // port B disabled default pins
    #elif defined KINETIS_KL03
    0x000000e0,                                                          // port A disabled default pins
    0x00002cd8,                                                          // port B disabled default pins
    #elif defined KINETIS_KL05
    0x000fcc60,                                                          // port A disabled default pins
    0x001f80d8,                                                          // port B disabled default pins
    #elif defined KINETIS_KL25                                           // {31}
    0x0003f026,                                                          // port A disabled default pins
    0x00000f00,                                                          // port B disabled default pins
    0x00033c38,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x8300003f                                                           // port E disabled default pins
    #elif defined KINETIS_KL26
    0x0003f0e6,                                                          // port A disabled default pins
    0x00f01f00,                                                          // port B disabled default pins
    0x00f73c38,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x8700003f                                                           // port E disabled default pins
    #elif defined KINETIS_KL27
    0x00003026,                                                          // port A disabled default pins
    0x000f0000,                                                          // port B disabled default pins
    0x00000c38,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x83000003                                                           // port E disabled default pins
    #else                                                                // KL20/KL40
    0x0003f0e6,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
        #if PORTS_AVAILABLE > 2
    0x00000000,                                                          // port C disabled default pins
    0x00000000,                                                          // port D disabled default pins
    0x8700007f                                                           // port E disabled default pins
        #endif
    #endif
#elif defined KINETIS_KV10
    0x00000006,                                                          // port A disabled default pins
    0x00030000,                                                          // port B disabled default pins
    0x00000030,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x03000000                                                           // port E disabled default pins
#elif defined KINETIS_KV30 || defined KINETIS_KV50
    0x0001f020,                                                          // port A disabled default pins
    0x00ff0200,                                                          // port B disabled default pins
    0x0007f030,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x04000070                                                           // port E disabled default pins
#elif defined KINETIS_KW2X
    0x00000000,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000030,                                                          // port C disabled default pins
    0x0000000d,                                                          // port D disabled default pins
    0x00000010                                                           // port E disabled default pins
#endif
};

static void fnPortInterrupt(int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig);

static int iFlagRefresh = 0;

#if defined USB_INTERFACE
    #define _fnLE_add(x) (void *)x

    #if defined USB_HS_INTERFACE                                         // {12}
        static unsigned char ucHSTxBuffer[NUMBER_OF_USBHS_ENDPOINTS] = {0}; // monitor the high speed controller's transmission buffer use
        static unsigned long ulHSEndpointInt = 0;
    #endif
    static unsigned char ucTxBuffer[NUMBER_OF_USB_ENDPOINTS] = {0};      // monitor the controller's transmission buffer use
    static unsigned char ucRxBank[NUMBER_OF_USB_ENDPOINTS];              // monitor the buffer to inject to
    static unsigned long ulEndpointInt = 0;
    #if defined USB_HOST_SUPPORT
    static int iData1Frame[NUMBER_OF_USB_ENDPOINTS] = {0};
    #endif
#endif
#if defined KINETIS_KL                                                   // {24}
    static unsigned long ulCOPcounter = 0;
#endif

// Initialisation of all non-zero registers in the device
//
static void fnSetDevice(unsigned long *port_inits)
{
    extern void fnEnterHW_table(void *hw_table);

#if defined SUPPORT_ADC
    int i;
#endif

#if !defined KINETIS_KL
    FMC_PFAPR  = 0x00f8003f;                                             // flash memory controller
    FMC_PFB0CR = 0x3002001f;
    FMC_PFB1CR = 0x3002001f;
#endif

#if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000) // {26}
    #if defined KINETIS_KL && defined RTC_USES_LPO_1kHz
    RCM_SRS0 = RCM_SRS0_PIN;                                             // simulate external reset
    #else
    RCM_SRS0 = (RCM_SRS0_POR | RCM_SRS0_LVD);                            // reset control module - reset status due to power on reset
    #endif
    #if defined KINETIS_KL || defined KINETIS_K22
    SMC_STOPCTRL = SMC_STOPCTRL_VLLSM_VLLS3;
    #else
    SMC_VLLSCTRL = SMC_VLLSCTRL_VLLSM_VLLS3;
    #endif
    SMC_PMSTAT = SMC_PMSTAT_RUN;
#elif !defined KINETIS_KE && !defined KINETIS_KEA
    MC_SRSL = (MC_SRSL_POR | MC_SRSL_LVD);                               // mode control - reset status due to power on reset
#endif
#if defined KINETIS_KL && !defined KINETIS_KL82
    SIM_COPC = SIM_COPC_COPT_LONGEST;                                    // COP (computer operating properly) rather than watchdog
#elif defined KINETIS_KE
    WDOG_CS1 = WDOG_CS1_EN;
    WDOG_CS2 = WDOG_CS2_CLK_1kHz;
    WDOG_TOVALL = 0x04;
#else
    WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_CLKSRC | WDOG_STCTRLH_WDOGEN); // watchdog
    WDOG_STCTRLL = 0x0001;
    WDOG_TOVALH = 0x004c;
    WDOG_TOVALL = 0x4b4c;
    WDOG_WINL = 0x0010;
    WDOG_REFRESH = 0xb480;
    WDOG_UNLOCK = 0xd928;
    WDOG_PRESC = 0x0400;
#endif
#if defined KINETIS_KL                                                   // {24}
    #if defined KINETIS_WITH_SCG
    SCG_VERID = 0x00000040;
    SCG_CSR = 0x02000001;
    SCG_RCCR = 0x02000001;
    SCG_VCCR = 0x02000001;
    SCG_HCCR = 0x02000001;
    SCG_CLKOUTCNFG = 0x02000000;
    SCG_SOSCCFG = 0x00000010;
    SCG_SIRCCSR = 0x03000005;
    SCG_SIRCCFG = 0x00000001;
    #elif defined KINETIS_KL03 || defined KINETIS_KL43 || defined KINETIS_KL27
    MCG_C1 = MCG_C1_CLKS_LIRC;
    MCG_C2 = MCG_C2_IRCS;
    MCG_S  = MCG_S_CLKST_LICR;
    #else
    MCG_C1 = MCG_C1_IREFS;
    #endif
#elif defined KINETIS_KE
    SIM_SRSID = (SIM_SRSID_LVD | SIM_SRSID_POR);
    ICS_C1 = ICS_C1_IREFS;
    ICS_C2 = ICS_C2_BDIV_2;
    ICS_S = ICS_S_IREFST;
#else
    EWM_CMPH = 0xff;                                                     // external watchdog monitor

    MCG_C1 = MCG_C1_IREFS;                                               // multi-purpose clock generator
    MCG_C2 = MCG_C2_LOCRE0;
    MCG_S  = MCG_S_IREFST;
    #if defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000)
    MCG_SC = 0x02;
    MCG_C10 = 0x80;
    #endif
    SIM_SDID = 0x014a;                                                   // K60 ID
#endif
    PMC_LVDSC1 = PMC_LVDSC1_LVDRE;                                       // low voltage detect reset enabled by default
    PMC_REGSC = PMC_REGSC_REGONS;                                        // regulator is in run regulation
#if defined MPU_AVAILABLE
    MPU_CESR  = (0x0081820 | MPU_CESR_VLD);                              // memory protection unit
    MPU_RGD0_WORD1  = 0x0000001f;
    MPU_RGD1_WORD1  = 0x0000001f;
    MPU_RGD2_WORD1  = 0x0000001f;
    MPU_RGD3_WORD1  = 0x0000001f;
    MPU_RGD4_WORD1  = 0x0000001f;
    MPU_RGD5_WORD1  = 0x0000001f;
    MPU_RGD6_WORD1  = 0x0000001f;
    MPU_RGD7_WORD1  = 0x0000001f;
    MPU_RGD8_WORD1  = 0x0000001f;
    MPU_RGD9_WORD1  = 0x0000001f;
    MPU_RGD10_WORD1 = 0x0000001f;
    MPU_RGD11_WORD1 = 0x0000001f;
    MPU_RGD12_WORD1 = 0x0000001f;
    MPU_RGD13_WORD1 = 0x0000001f;
    MPU_RGD14_WORD1 = 0x0000001f;
    MPU_RGD15_WORD1 = 0x0000001f;
#endif
#if !defined KINETIS_WITHOUT_PIT
    PIT_MCR = PIT_MCR_MDIS;                                              // PITs disabled
#endif
#if defined KINETIS_KE
    SIM_SCGC = (SIM_SCGC_FLASH | SIM_SCGC_SWD);
    SIM_SOPT0 = (SIM_SOPT_NMIE | SIM_SOPT_RSTPE | SIM_SOPT_SWDE);        // PTB4 functions as NMI, PTA5 pin functions as RESET, PTA4 and PTC4 function as single wire debug
#elif defined KINETIS_KV
    SIM_SCGC4 = 0xf0000030;
    SIM_SCGC5 = 0x00040180;
    SIM_SCGC6 = SIM_SCGC6_FTFL;
    SIM_SCGC7 = 0x00000100;
    SIM_CLKDIV1 = (SIM_CLKDIV1_BUS_2 | SIM_CLKDIV5_ADC_2);
#else
    SIM_SCGC6 = (0x40000000 | SIM_SCGC6_FTFL);
    #if defined KINETIS_HAS_IRC48M && !defined KINETIS_KL
    SIM_SOPT2 = (SIM_SOPT2_TRACECLKSEL);
    #elif defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000)
    SIM_SOPT2 = (SIM_SOPT2_TRACECLKSEL | 0x04000000 | SIM_SOPT2_NFCSRC_MCGPLL0CLK);
    SIM_CLKDIV4 = 0x00000002;
    #elif !defined KINETIS_KL
    SIM_SOPT2 = SIM_SOPT2_TRACECLKSEL;
    #endif
#endif
#if defined KINETIS_KL
    #if defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL05
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT3);              // default SWD_CLK
    PORTA_PCR1 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT3);    // default reset input
    PORTA_PCR2 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT3);              // default SWD_DIO
    PORTA_PCR3 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT0);              // default EXTAL0
    PORTA_PCR4 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT0);              // default XTAL0
        #if defined KINETIS_KL03
    PORTB_PCR5 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT3);    // default NMI input
        #endif
    #else
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT7);              // port configuration defaults (JTAG/SWD)
    PORTA_PCR20 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);   // default reset input
    PORTA_PCR3 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    #endif
#elif defined KINETIS_KE
    GPIOA_PIDR = 0xffffffff;                                             // port input disable registers
    #if PORTS_AVAILABLE > 1
    GPIOB_PIDR = 0xffffffff;
    #endif
    #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
    PORT_IOFLT0 = 0x00c00000;
    PORT_PUE0   = 0x00100000;
    #else
    PORT_IOFLT = 0x00c00000;
        #if (defined KINETIS_KE04 && (SIZE_OF_FLASH <= (8 * 1024)))
    PORT_PUEL  = 0x00000001;
        #else
    PORT_PUEL  = 0x00100000;
        #endif
    #endif
#elif defined KINETIS_KV10
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT7);              // port configuration defaults (JTAG/SWD)
    PORTA_PCR3 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR20 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
#else
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT7);              // port configuration defaults (JTAG/SWD)
    PORTA_PCR1 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR2 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR3 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    if ((ulDisabled[0] & 0x20) == 0) {
        PORTA_PCR5 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    }
#endif
#if defined USB_INTERFACE
    PER_ID = 0x04;                                                       // USB-OTG
    ID_COMP = 0xfb;
    REV = 0x33;
    ADD_INFO = IEHOST;
    OTG_INT_STAT = MSEC_1;
#endif
#if LPUARTS_AVAILABLE > 0
    LPUART0_BAUD = (LPUART_BAUD_OSR_16 | 0x00000004);
    LPUART0_STAT = (LPUART_STAT_TDRE | LPUART_STAT_TC);
    LPUART0_DATA = (LPUART_DATA_RXEMPT);
#elif UARTS_AVAILABLE > 0
    UART0_BDL    = 0x04;                                                 // UARTs
    UART0_S1     = (UART_S1_TDRE | UART_S1_TC);
    #if !defined KINETIS_KL && !defined KINETIS_KE
    UART0_SFIFO  = 0xc0;
    UART0_RWFIFO = 0x01;
    UART0_WP7816T0 = 0x0a;
    UART0_WF7816 = 0x01;
    #endif
#endif
#if LPUARTS_AVAILABLE > 1
    LPUART1_BAUD = (LPUART_BAUD_OSR_16 | 0x00000004);
    LPUART1_STAT = (LPUART_STAT_TDRE | LPUART_STAT_TC);
    LPUART1_DATA = (LPUART_DATA_RXEMPT);
#elif UARTS_AVAILABLE > 1
    UART1_BDL    = 0x04;   
    UART1_S1     = 0xc0;
        #if !defined KINETIS_KL && !defined KINETIS_KE
    UART1_SFIFO  = 0xc0;
    UART1_RWFIFO = 0x01;
    UART1_WP7816T0 = 0x0a;
    UART1_WF7816 = 0x01;
        #endif
#endif
#if UARTS_AVAILABLE > 2 || (LPUARTS_AVAILABLE == 2 && UARTS_AVAILABLE == 1)
    UART2_BDL    = 0x04;   
    UART2_S1     = 0xc0;
     #if !defined KINETIS_KL && !defined KINETIS_KE
    UART2_SFIFO  = 0xc0;
    UART2_RWFIFO = 0x01;
    UART2_WP7816T0 = 0x0a;
    UART2_WF7816 = 0x01;
    #endif
#endif
#if UARTS_AVAILABLE > 3
    UART3_BDL    = 0x04;   
    UART3_S1     = 0xc0;
    UART3_SFIFO  = 0xc0;
    UART3_RWFIFO = 0x01;
    UART3_WP7816T0 = 0x0a;
    UART3_WF7816 = 0x01;
#endif
#if UARTS_AVAILABLE > 4
    UART4_BDL    = 0x04;   
    UART4_S1     = 0xc0;
    UART4_SFIFO  = 0xc0;
    UART4_RWFIFO = 0x01;
    UART4_WP7816T0 = 0x0a;
    UART4_WF7816 = 0x01;
#endif
#if UARTS_AVAILABLE > 5
    UART5_BDL    = 0x04;   
    UART5_S1     = 0xc0;
    UART5_SFIFO  = 0xc0;
    UART5_RWFIFO = 0x01;
    UART5_WP7816T0 = 0x0a;
    UART5_WF7816 = 0x01;
#endif
#if defined KINETIS_KE
    FTMRH_FSTAT = FTMRH_STAT_CCIF;
    FTMRH_FSEC  = KINETIS_FLASH_CONFIGURATION_SECURITY;
    FTMRH_FPROT = KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT;
    FTMRH_FOPT  = KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION;
#else
    FTFL_FSTAT  = FTFL_STAT_CCIF;                                        // flash controller
    FTFL_FSEC   = KINETIS_FLASH_CONFIGURATION_SECURITY;
    FTFL_FOPT   = KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION;
    FTFL_FEPROT = KINETIS_FLASH_CONFIGURATION_EEPROM_PROT;
    FTFL_FDPROT = KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT;
    FTFL_FPROT0 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION >> 24);
    FTFL_FPROT1 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION >> 16);
    FTFL_FPROT2 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION >> 8);
    FTFL_FPROT3 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION);
#endif
#if defined KINETIS_KL || defined KINETIS_KE
    SPI0_C1     = SPI_C1_CPHA;
    SPI0_S      = SPI_S_SPTEF;
    #if !defined KINETIS_KL03
    SPI1_C1     = SPI_C1_CPHA;
    SPI1_S      = SPI_S_SPTEF;
    #endif
    #if defined MSCAN_CAN_INTERFACE
    MSCAN_CANCTL0 = MSCAN_CANCTL0_INITRQ;
    MSCAN_CANCTL1 = (MSCAN_CANCTL1_INITAK | MSCAN_CANCTL1_LISTEN);
    MSCAN_CANTFLG = (MSCAN_CANTFLG_TXE0 | MSCAN_CANTFLG_TXE1 | MSCAN_CANTFLG_TXE2);
    #endif
#else
    SPI0_MCR    = (SPI_MCR_DOZE | SPI_MCR_HALT);                         // DSPI
    SPI0_CTAR0  = 0x78000000;
    SPI0_CTAR1  = 0x78000000;
    SPI0_SR     = 0x02000000;
    #if SPI_AVAILABLE > 1
    SPI1_MCR    = (SPI_MCR_DOZE | SPI_MCR_HALT);
    SPI1_CTAR0  = 0x78000000;
    SPI1_CTAR1  = 0x78000000;
    SPI1_SR     = 0x02000000;
    #endif
    #if SPI_AVAILABLE > 2
    SPI2_MCR    = (SPI_MCR_DOZE | SPI_MCR_HALT);
    SPI2_CTAR0  = 0x78000000;
    SPI2_CTAR1  = 0x78000000;
    SPI2_SR     = 0x02000000;
    #endif
    SDHC_PROCTL    = SDHC_PROCTL_EMODE_LITTLE;                           // SDHC
    SDHC_SYSCTL    = (SDHC_SYSCTL_SDCLKFS_256 | SDHC_SYSCTL_SDCLKEN);
    SDHC_IRQSTATEN = 0x117f013f;
    SDHC_WML       = 0x00100010;
    SDHC_VENDOR    = SDHC_VENDOR_EXTDMAEN;
    SDHC_HOSTVER   = 0x00001201;

    CAN0_MCR       = 0xd890000f;                                         // FlexCAN
    CAN0_RXGMASK   = 0xffffffff;
    CAN0_RX14MASK  = 0xffffffff;
    CAN0_RX15MASK  = 0xffffffff;
    CAN0_CTRL2     = 0x00c00000;
    CAN0_RXFGMASK  = 0xffffffff;
    #if NUMBER_OF_CAN_INTERFACES > 1
    CAN1_MCR       = 0xd890000f;
    CAN1_RXGMASK   = 0xffffffff;
    CAN1_RX14MASK  = 0xffffffff;
    CAN1_RX15MASK  = 0xffffffff;
    CAN1_CTRL2     = 0x00c00000;
    CAN1_RXFGMASK  = 0xffffffff;
    #endif
#endif
#if defined KINETIS_K80                                                  // QSPI
    QuadSPI0_FLSHCR = 0x00000303;
    QuadSPI0_DLPR   = 0xaa553443;
    QuadSPI0_LUTKEY = 0x5af05af0;
    QuadSPI0_LCKCR  = 0x00000002;
#endif
#if !defined KINETIS_KL02
    #if !defined KINETIS_KE
        #if defined SUPPORT_RTC                                          // RTC
    RTC_SR      = 0;                                                     // assume running
        #else
    RTC_SR      = RTC_SR_TIF;
        #endif
    RTC_LR      = (RTC_LR_TCL | RTC_LR_CRL | RTC_LR_SRL | RTC_LR_LRL);
    RTC_IER     = (RTC_IER_TIIE | RTC_IER_TOIE | RTC_IER_TAIE);
    #endif
    #if defined KINETIS_KL
    SIM_SOPT1   = SIM_SOPT1_OSC32KSEL_LPO_1kHz;                          // assume retained over reset
    #elif !defined KINETIS_KE
    RTC_RAR     = (RTC_RAR_TSRW | RTC_RAR_TPRW | RTC_RAR_TARW| RTC_RAR_TCRW | RTC_RAR_CRW | RTC_RAR_SRW | RTC_RAR_LRW | RTC_RAR_IERW);
    #endif
    #if !defined KINETIS_KL && !defined KINETIS_KE && !defined CROSSBAR_SWITCH_LITE
    AXBS_CRS0 = 0x76543210;                                              // {34} default crossbar switch settings
    AXBS_CRS1 = 0x76543210;
    AXBS_CRS2 = 0x76543210;
    AXBS_CRS3 = 0x76543210;
    AXBS_CRS4 = 0x76543210;
    AXBS_CRS5 = 0x76543210;
    AXBS_CRS6 = 0x76543210;

    DMA_DCHPRI0    = 0;                                                  // default DMA channel priorities
    DMA_DCHPRI1    = 1;
    DMA_DCHPRI2    = 2;
    DMA_DCHPRI3    = 3;
    DMA_DCHPRI4    = 4;
    DMA_DCHPRI5    = 5;
    DMA_DCHPRI6    = 6;
    DMA_DCHPRI7    = 7;
    DMA_DCHPRI8    = 8;
    DMA_DCHPRI9    = 9;
    DMA_DCHPRI10   = 10;
    DMA_DCHPRI11   = 11;
    DMA_DCHPRI12   = 12;
    DMA_DCHPRI13   = 13;
    DMA_DCHPRI14   = 14;
    DMA_DCHPRI15   = 15;
    #endif
#endif
#if defined KINETIS_KE
    ADC0_SC1 = ADC_SC1A_ADCH_OFF;
    ADC0_SC2 = ADC_SC2_FEMPTY;
#else
    ADC0_SC1A   = ADC_SC1A_ADCH_OFF;                                     // ADC
    ADC0_SC1B   = ADC_SC1A_ADCH_OFF;
    ADC0_OFS    = 0x00000004;
    ADC0_PG     = 0x00008200;
    ADC0_MG     = 0x00008200;
    ADC0_CLPD   = 0x0000000a;
    ADC0_CLPS   = 0x00000020;
    ADC0_CLP4   = 0x00000200;
    ADC0_CLP3   = 0x00000100;
    ADC0_CLP2   = 0x00000080;
    ADC0_CLP1   = 0x00000040;
    ADC0_CLP0   = 0x00000020;
    ADC0_CLMD   = 0x0000000a;
    ADC0_CLMS   = 0x00000020;
    ADC0_CLM4   = 0x00000200;
    ADC0_CLM3   = 0x00000100;
    ADC0_CLM2   = 0x00000080;
    ADC0_CLM1   = 0x00000040;
    ADC0_CLM0   = 0x00000020;
#endif
#if ADC_CONTROLLERS > 1
    ADC1_SC1A   = ADC_SC1A_ADCH_OFF;
    ADC1_SC1B   = ADC_SC1A_ADCH_OFF;
    ADC1_OFS    = 0x00000004;
    ADC1_PG     = 0x00008200;
    ADC1_MG     = 0x00008200;
    ADC1_CLPD   = 0x0000000a;
    ADC1_CLPS   = 0x00000020;
    ADC1_CLP4   = 0x00000200;
    ADC1_CLP3   = 0x00000100;
    ADC1_CLP2   = 0x00000080;
    ADC1_CLP1   = 0x00000040;
    ADC1_CLP0   = 0x00000020;
    ADC1_CLMD   = 0x0000000a;
    ADC1_CLMS   = 0x00000020;
    ADC1_CLM4   = 0x00000200;
    ADC1_CLM3   = 0x00000100;
    ADC1_CLM2   = 0x00000080;
    ADC1_CLM1   = 0x00000040;
    ADC1_CLM0   = 0x00000020;
#endif
#if DAC_CONTROLLERS > 1
    DAC0_SR     = DAC_SR_DACBFRPTF;                                      // DAC
    DAC0_C2     = 0x0f;
    #if DAC_CONTROLLERS > 1
    DAC1_SR     = DAC_SR_DACBFRPTF;
    DAC1_C2     = 0x0f;
    #endif
#endif
#if defined RNG_AVAILABLE                                                // random number generator
    #if defined RANDOM_NUMBER_GENERATOR_B                                // {64}
    RNG_VER     = (RNG_VER_RNGB | 0x00000280);
    RNG_SR      = (RNG_SR_FIFO_SIZE_5 | RNG_SR_RS | RNG_SR_SLP | 0x00000001);
    #else
    RNG_SR      = (RNG_SR_OREG_SIZE);
    #endif
#endif
    MCM_PLASC  = 0x001f;                                                  // {15}
    MCM_PLAMC  = 0x003f;
#if defined ETHERNET_AVAILABLE
    ECR        = 0xf0000000;                                             // ENET
    MIBC       = 0xc0000000;
    RCR        = 0x05ee0001;
    PAUR       = 0x00008808;
    OPD        = 0x00010000;
    ENET_RAEM  = 0x00000004;
    ENET_RAFL  = 0x00000004;
    ENET_TAEM  = 0x00000004;
    ENET_TAFL  = 0x00000008;
    ENET_TIPG  = 0x0000000c;
    ENET_FTRL  = 0x000007ff;
    ENET_ATPER = 0x3b9aca00;
#endif
#if !defined KINETIS_KL && !defined KINETIS_KE                           // {24}
    PDB0_MOD   = 0x0000ffff;                                             // PDB {16}
    PDB0_IDLY  = 0x0000ffff;
#endif
    I2C0_S      = I2C_TCF;                                               // I2C
    I2C0_A2     = 0xc2;
#if I2C_AVAILABLE > 1
    I2C1_S      = 0x80;
    I2C1_A2     = 0xc2;
#endif
#if I2C_AVAILABLE > 2
    I2C2_S      = 0x80;
    I2C2_A2     = 0xc2;
#endif
#if defined DEVICE_WITH_SLCD
    LCD_GCR     = 0x08350003;                                            // SLCD
#endif
    GPIOA_PDIR  = ulPort_in_A = *port_inits++;                           // set port inputs to default states
#if PORTS_AVAILABLE > 1
    GPIOB_PDIR  = ulPort_in_B = *port_inits++;
#endif
#if PORTS_AVAILABLE > 2
    GPIOC_PDIR  = ulPort_in_C = *port_inits++;
#endif
#if PORTS_AVAILABLE > 3
    GPIOD_PDIR  = ulPort_in_D = *port_inits++;
#endif
#if PORTS_AVAILABLE > 4
    GPIOE_PDIR  = ulPort_in_E = *port_inits++;
#endif
#if PORTS_AVAILABLE > 5
    GPIOE_PDIR  = ulPort_in_F = *port_inits++;
#endif
#if !defined KINETIS_KL && !defined KINETIS_KE
    FTM0_MODE = FTM_MODE_WPDIS;                                          // FlexTimer
    FTM1_MODE = FTM_MODE_WPDIS;
    FTM2_MODE = FTM_MODE_WPDIS;
#endif
#if defined SDRAM_CONTROLLER_AVAILABLE
    NFC_CMD1  = 0x30ff0000;                                              // {1} NAND flash controller
    NFC_CMD2  = 0x007ee000;
    NFC_RAR   = 0x11000000;
    NFC_SWAP  = 0x0ffe0ffe;

    DDR_CR00 = 0x20400000;                                               // {14} DDR1/2/LP SDRAM controller
    DDR_CR01 = 0x00020b10;
    DDR_CR20 = 0x0c000000;
    DDR_CR21 = 0x00000400;
    DDR_CR22 = 0x00000400;
    DDR_CR51 = 0x00000400;
    DDR_PAD_CTRL = 0x00000200;
#endif
#if defined KINETIS_K70                                                  // LCD
    LCDC_LCWHB = 0x10100ff;
    LCDC_LHCR = 0x04000000;
    LCDC_LVCR = 0x04000000;
    LCDC_LDCR = 0x80040060;
    LCDC_LGWDCR = 0x80040060;
#endif
#if defined HS_USB_AVAILABLE                                             // {12}
    USBHS_ID = 0xe461fa05;
    USBHS_HWGENERAL = 0x00000085;
    USBHS_HWHOST = 0x10020001;
    USBHS_HWDEVICE = 0x00000009;
    USBHS_HWTXBUF = 0x80070908;
    USBHS_HWRXBUF = 0x00000808;
    USBHS_HCIVERSION = 0x01000040;
    USBHS_HCSPARAMS = 0x00010011;
    USBHS_HCCPARAMS = 0x00000006;
    USBHS_DCIVERSION = 0x0001;
    USBHS_USBCMD = 0x00080000;
    USBHS_BURSTSIZE = 0x00008080;
    USBHS_PORTSC1 = 0x80000000;
    USBHS_OTGSC = 0x00001020;
    USBHS_USBMODE = 0x00005000;
    USBHS_EPCR0 = 0x00800080;
    #if defined KINETIS_WITH_USBPHY
    USBPHY_PWD = 0x001e1c00;
    USBPHY_TX = 0x10060607;
    USBPHY_CTRL = 0xc0000000;
    USBPHY_DEBUG = 0x7f180000;
    USBPHY_DEBUG1 = 0x00001000;
    USBPHY_VERSION = 0x04030000;
    USBPHY_PLL_SIC = 0x00012000;
    USBPHY_USB1_VBUS_DETECT = 0x00700004;
    USBPHY_ANACTRL = 0x00000402;
    USBPHY_USB1_LOOPBACK = 0x00550000;
    USBPHY_USB1_LOOPBACK_HSFSCNT = 0x00040010;
    #endif
#endif
#if defined CHIP_HAS_FLEXIO
    FLEXIO_VERID = 0x01000000;
    FLEXIO_PARAM = 0x10080404;
#endif
#if !defined KINETIS_KL && !defined KINETIS_KE
    CRC_CRC   = 0xffffffff;                                              // {6} CRC
    CRC_GPOLY = 0x00001021;                                              // default CRC-16 polynomial
#endif
#if defined LLWU_AVAILABLE && !defined KINETIS_KL03 && !defined KINETIS_KL05
    LLWU_RST = LLWU_RST_LLRSTE;
#endif
    fnSimPers();                                                         // synchronise peripheral states with default settings
#if defined SUPPORT_ADC
    for (i = 0; i < (ADC_CHANNELS * ADC_CONTROLLERS); i++) {
        usADC_values[i] = (unsigned short)*port_inits++;                 // {2} prime initial ADC values
    }
    fnEnterHW_table(ptrPorts);                                           
#else
    fnEnterHW_table(ucPortFunctions);
#endif
}

unsigned char ucFLASH[SIZE_OF_FLASH + PROGRAM_ONCE_AREA_SIZE];           // {7} internal memory plus program-once flash content

extern void fnInitialiseDevice(void *port_inits)
{
    memset(ucFLASH, 0xff, sizeof(ucFLASH));                              // we start with deleted FLASH memory contents (use memset() rather than uMemset() since the DMA controller may not be initialised yet)
    fnPrimeFileSystem();                                                 // the project can then set parameters or files as required
    fnSetDevice((unsigned long *)port_inits);                            // set device registers to startup condition (if not zerod)
}

extern void fnDeleteFlashSector(unsigned char *ptrSector)
{
#if (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0))
    if (ptrSector >= &ucFLASH[SIZE_OF_FLASH - SIZE_OF_EEPROM] ) {
        uMemset(ptrSector, 0xff, KE_EEPROM_GRANULARITY);
        return;
    }
#endif
    uMemset(ptrSector, 0xff, FLASH_GRANULARITY);
}


extern unsigned char *fnGetEEPROM(unsigned short usOffset);

extern unsigned char *fnGetFileSystemStart(int iMemory)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    return (fnGetEEPROM(uFILE_START));
#elif defined (SAVE_COMPLETE_FLASH)
    return (&ucFLASH[0]);
#else
    return (&ucFLASH[uFILE_START - FLASH_START_ADDRESS]);
#endif
}


extern unsigned long fnGetFlashSize(int iMemory)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    return (fnGetEEPROMSize());
#elif defined (SAVE_COMPLETE_FLASH)
    return (sizeof(ucFLASH));
#else
    return (FILE_SYSTEM_SIZE);
#endif
}



// Transform physical access address to address in simulated FLASH
//
extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd)               
{
    unsigned char *ucSimulatedFlash;
#if defined FLEXFLASH_DATA                                               // {33}
    if (ucAdd >= (unsigned char *)FLEXNVM_START_ADDRESS) {               // access may be in FlexNMV area
        ucAdd -= (FLEXNVM_START_ADDRESS - (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)); // move to contiguous simulation flash area
    }
#elif defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)
    if (ucAdd >= (unsigned char *)KE_EEPROM_START_ADDRESS) {             // access may be in EEPROM area
        ucAdd -= (KE_EEPROM_START_ADDRESS - (SIZE_OF_FLASH - SIZE_OF_EEPROM)); // move to contiguous simulation flash area
    }
#endif
    ucSimulatedFlash = ucAdd + (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    if (ucSimulatedFlash >= &ucFLASH[SIZE_OF_FLASH/* - PROGRAM_ONCE_AREA_SIZE*/]) { // {21} check flash bounds
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

extern unsigned char fnMapPortBit(unsigned long ulRealBit)
{
    unsigned long ulBit = 0x80000000;
    unsigned char ucRef = 0;

    while (ulBit) {
        if (ulRealBit & ulBit) {
            break;
        }
        ulBit >>= 1;
        ucRef++;
    }
    return ucRef;
}

// Periodic tick (on timer 1) - dummy since timer simulation used now
//
extern void RealTimeInterrupt(void)
{
}

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

// Get the present state of a port
//
extern unsigned long fnGetPresentPortState(int portNr)
{
#if defined KINETIS_KE
    int iShift;
#else
    #define iShift 0
#endif
    portNr--;
#if defined KINETIS_KE
    iShift = ((portNr % 4) * 8);
    portNr /= 4;                                                         // convert 8 bit port to 32 bit port
#endif
    switch (portNr) {
    case _PORTA:
        return (((GPIOA_PDDR & GPIOA_PDOR) | (~GPIOA_PDDR & ulPort_in_A)) >> iShift);
#if PORTS_AVAILABLE > 1
    case _PORTB:
        return (((GPIOB_PDDR & GPIOB_PDOR) | (~GPIOB_PDDR & ulPort_in_B)) >> iShift);
#endif
#if PORTS_AVAILABLE > 2
    case _PORTC:
        return ((GPIOC_PDDR & GPIOC_PDOR) | (~GPIOC_PDDR & ulPort_in_C));  
#endif
#if PORTS_AVAILABLE > 3
    case _PORTD:
        return ((GPIOD_PDDR & GPIOD_PDOR) | (~GPIOD_PDDR & ulPort_in_D));
#endif
#if PORTS_AVAILABLE > 4
    case _PORTE:
        return ((GPIOE_PDDR & GPIOE_PDOR) | (~GPIOE_PDDR & ulPort_in_E));
#endif
#if PORTS_AVAILABLE > 5
    case _PORTF:
        return ((GPIOF_PDDR & GPIOF_PDOR) | (~GPIOF_PDDR & ulPort_in_F));
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    case _PORT_EXP_0:                                                    // external ports extensions
    case _PORT_EXP_1:
    case _PORT_EXP_2:
    case _PORT_EXP_3:
    case _PORT_EXP_4:
    case _PORT_EXP_5:
    case _PORT_EXP_6:
    case _PORT_EXP_7:
        return (fnGetExtPortState(portNr));                              // {8} pin states of external port
#endif
    default:
        return 0;
    }
}

// Get the present state of a port direction
//
extern unsigned long fnGetPresentPortDir(int portNr)
{
    unsigned long ulConnectedGPIO;
    unsigned long ulCheckedPin;
    unsigned long ulBit = 0x00000001;
#if defined KINETIS_KE
    int iShift;
#else
    unsigned long *ptrPCR = (unsigned long *)PORT0_BLOCK;
#endif
    portNr--;
#if defined KINETIS_KE
    iShift = ((portNr % 4) * 8);
    portNr /= 4;                                                         // convert 8 bit port to 32 bit port
#else
    ptrPCR += (portNr * (sizeof(KINETIS_PORT)/sizeof(unsigned long)));
#endif
    switch (portNr) {
    case _PORTA:
        ulCheckedPin = ulConnectedGPIO = (GPIOA_PDDR >> iShift);
        break;
#if PORTS_AVAILABLE > 1
    case _PORTB:
        ulCheckedPin = ulConnectedGPIO = (GPIOB_PDDR >> iShift);
        break;
#endif
#if PORTS_AVAILABLE > 2
    case _PORTC:
        ulCheckedPin = ulConnectedGPIO = GPIOC_PDDR;
        break;
#endif
#if PORTS_AVAILABLE > 3
    case _PORTD:
        ulCheckedPin = ulConnectedGPIO = GPIOD_PDDR;
        break;
#endif
#if PORTS_AVAILABLE > 4
    case _PORTE:
        ulCheckedPin = ulConnectedGPIO = GPIOE_PDDR;
        break;
#endif
#if PORTS_AVAILABLE > 5
    case _PORTF:
        ulCheckedPin = ulConnectedGPIO = GPIOF_PDDR;
        break;
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    case _PORT_EXP_0:                                                    // {8} external ports extensions
    case _PORT_EXP_1:
    case _PORT_EXP_2:
    case _PORT_EXP_3:
    case _PORT_EXP_4:
    case _PORT_EXP_5:
    case _PORT_EXP_6:
    case _PORT_EXP_7:
        return (fnGetExtPortDirection(portNr));
#endif
    default:
        return 0;
    }
    while (ulCheckedPin != 0) {
#if !defined KINETIS_KE
        if ((*ptrPCR & PORT_MUX_MASK) != PORT_MUX_GPIO) {                // only connected port bits are considered as outputs
            ulConnectedGPIO &= ~ulBit;
        }
        ptrPCR++;
#endif
        ulCheckedPin &= ~ulBit;
        ulBit <<= 1;
    }
    return (ulConnectedGPIO);                                            // connected ports configured as GPIO outputs
}


extern unsigned long fnGetPresentPortPeriph(int portNr)
{
#if defined KINETIS_KE
    int iShift;
#endif
    portNr--;
#if defined KINETIS_KE
    iShift = ((portNr % 4) * 8);
    portNr /= 4;                                                         // convert 8 bit port to 32 bit port
#endif
    switch (portNr) {
    case _PORTA:
        return (ulPeripherals[0] >> iShift);
    case _PORTB:
        return (ulPeripherals[1] >> iShift);
    case _PORTC:
        return (ulPeripherals[2]);
    case _PORTD:
        return (ulPeripherals[3]);
    case _PORTE:
        return (ulPeripherals[4]);
#if PORTS_AVAILABLE > 5
    case _PORTF:
        return (ulPeripherals[5]);
#endif
    default:
        return 0;
    }
}


// See whether there has been a port change which should be displayed
//
extern int fnPortChanges(int iForce)
{
    int iRtn = iFlagRefresh;
    static unsigned long ulPortDir0 = 0, ulPortDir1 = 0, ulPortDir2 = 0, ulPortDir3 = 0, ulPortDir4 = 0;
    static unsigned long ulPortVal0 = 0, ulPortVal1 = 0, ulPortVal2 = 0, ulPortVal3 = 0, ulPortVal4 = 0;
    static unsigned long ulPortFunction0 = 0, ulPortFunction1 = 0, ulPortFunction2 = 0, ulPortFunction3 = 0, ulPortFunction4 = 0;
#if PORTS_AVAILABLE > 5
    static unsigned long ulPortFunction5 = 0; 
    static unsigned long ulPortDir5 = 0;
    static unsigned long ulPortVal5 = 0;
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    static unsigned long ulPortExtValue[_EXTERNAL_PORT_COUNT] = {0};
    int iExPort = 0;
#endif
    unsigned long ulNewValue;
    unsigned long ulNewPortPer;
    iFlagRefresh = 0;

    ulNewValue = fnGetPresentPortState(_PORTA + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTA + 1);
    if ((ulNewValue != ulPortVal0) || (ulNewPortPer != ulPortFunction0)) {
        ulPortVal0 = ulNewValue;
        ulPortFunction0 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
    ulNewValue = fnGetPresentPortState(_PORTB + 1);
    ulNewPortPer = fnGetPresentPortPeriph(PORTB + 1);
    if ((ulNewValue != ulPortVal1) || (ulNewPortPer != ulPortFunction1)) {
        ulPortVal1 = ulNewValue;
        ulPortFunction1 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORTC + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTC + 1);
    if ((ulNewValue != ulPortVal2) || (ulNewPortPer != ulPortFunction2)) {
        ulPortVal2 = ulNewValue;
        ulPortFunction2 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORTD + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTD + 1);
    if ((ulNewValue != ulPortVal3) || (ulNewPortPer != ulPortFunction3)) {
        ulPortVal3 = ulNewValue;
        ulPortFunction3 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORTE + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTE + 1);
    if ((ulNewValue != ulPortVal4) || (ulNewPortPer != ulPortFunction4)) {
        ulPortVal4 = ulNewValue;
        ulPortFunction4 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
#if PORTS_AVAILABLE > 5
    ulNewValue = fnGetPresentPortState(_PORTF + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTF + 1);
    if ((ulNewValue != ulPortVal5) || (ulNewPortPer != ulPortFunction5)) {
        ulPortVal5 = ulNewValue;
        ulPortFunction5 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    while (iExPort < _EXTERNAL_PORT_COUNT) {                             // external ports extensions
        ulNewValue = fnGetPresentPortState(iExPort + (_PORT_EXP_0 + 1));
        if (ulNewValue != ulPortExtValue[iExPort]) {
            ulPortExtValue[iExPort] = ulNewValue;
            iRtn |= PORT_CHANGE;
        }
        iExPort++;
    }
#endif
    return iRtn;
}

#if defined KINETIS_KE
static void fnHandleIRQ(int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig)
{
    switch (iPort) {                                                     // check ports that have potential interrupt functions
    case KE_PORTA:
        if (ulChangedBit & KE_PORTA_BIT5) {
            if ((SIM_SOPT0 & SIM_SOPT_RSTPE) == 0) {                     // PTA5 not programmed as reset pin
    #if defined SIM_PINSEL_IRQPS_PTI6
                if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) != SIM_PINSEL_IRQPS_PTA5) {
                    return;
                }
    #endif
                break;                                                   // valid IRQ input so we check the input settings
            }
        }
        return;
    #if defined SIM_PINSEL_IRQPS_PTI6
    case KE_PORTI:
        switch (SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) {
        case SIM_PINSEL_IRQPS_PTI0:
            if (ulChangedBit != KE_PORTI_BIT0) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI1:
            if (ulChangedBit != KE_PORTI_BIT1) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI2:
            if (ulChangedBit != KE_PORTI_BIT2) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI3:
            if (ulChangedBit != KE_PORTI_BIT3) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI4:
            if (ulChangedBit != KE_PORTI_BIT4) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI5:
            if (ulChangedBit != KE_PORTI_BIT5) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI6:
            if (ulChangedBit != KE_PORTI_BIT6) {
                return;
            }
            break;
        default:
            return;
        }
        break;
    #endif
    default:
        return;
    }
    // IRQ input has changed so we check to see whether the change/state matches with the programmed one
    //
    if (IRQ_SC & IRQ_SC_IRQIE) {                                         // interrupt is enabled
        if (IRQ_SC & IRQ_SC_IRQEDG) {                                    // high or rising edge sensitive
            if ((ulNewState & ulChangedBit) == 0) {                      // input has changed to '0'
                return;
            }
        }
        else {                                                           // low or falling edge sensitive
            if ((ulNewState & ulChangedBit) != 0) {                      // input has changed to '1'
                return;
            }
        }
        IRQ_SC |= IRQ_SC_IRQF;                                           // set the interrupt flag
        if (fnGenInt(irq_IRQ_ID) != 0) {                                 // if the interrupt is enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_IRQ();                     // call port interrupt handler
        }
    }
}

static void fnHandleKBI(int iController, int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig)
{
    #if KBI_WIDTH == 32
    unsigned long KBI_input = 0;
    #else
    unsigned char KBI_input = 0;
    #endif
    switch (iPort) {                                                     // check ports that have potential interrupt functions
    case KE_PORTA:
        if (iController == 0) {
            if (ulChangedBit & KE_PORTA_BIT0) {
                if (ucPortFunctions[iPort][0] == PA_0_KBI0_P0) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x01;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTA_BIT1) {
                if (ucPortFunctions[iPort][1] == PA_1_KBI0_P1) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x02;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTA_BIT2) {
                if (ucPortFunctions[iPort][2] == PA_2_KBI0_P2) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x04;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTA_BIT3) {
                if (ucPortFunctions[iPort][3] == PA_3_KBI0_P3) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x08;
                    break;
                }
            }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
            else if (ulChangedBit & KE_PORTA_BIT4) {
                if (ucPortFunctions[iPort][4] == PA_4_KBI0_P4) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x10;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTA_BIT5) {
                if (ucPortFunctions[iPort][5] == PA_5_KBI0_P5) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x20;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTA_BIT6) {
                if (ucPortFunctions[iPort][6] == PA_6_KBI0_P6) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x40;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTA_BIT7) {
                if (ucPortFunctions[iPort][7] == PA_7_KBI0_P7) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x80;
                    break;
                }
            }
    #endif
            else if (ulChangedBit & KE_PORTB_BIT0) {
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort][8] == PB_0_KBI0_P8) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x100;
                    break;
                }
    #else
                if (ucPortFunctions[iPort][8] == PB_0_KBI0_P4) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x10;
                    break;
                }
    #endif
            }
            else if (ulChangedBit & KE_PORTB_BIT1) {
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort][9] == PB_1_KBI0_P9) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x200;
                    break;
                }
    #else
                if (ucPortFunctions[iPort][9] == PB_1_KBI0_P5) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x20;
                    break;
                }
    #endif
            }
            else if (ulChangedBit & KE_PORTB_BIT2) {
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort][10] == PB_2_KBI0_P10) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x400;
                    break;
                }
    #else
                if (ucPortFunctions[iPort][10] == PB_2_KBI0_P6) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x40;
                    break;
                }
    #endif
            }
            else if (ulChangedBit & KE_PORTB_BIT3) {
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort][11] == PB_3_KBI0_P11) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x800;
                    break;
                }
    #else
                if (ucPortFunctions[iPort][11] == PB_3_KBI0_P7) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x80;
                    break;
                }
    #endif
            }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
            else if (ulChangedBit & KE_PORTB_BIT4) {
                if (ucPortFunctions[iPort][12] == PB_4_KBI0_P12) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x1000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTB_BIT5) {
                if (ucPortFunctions[iPort][13] == PB_5_KBI0_P13) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x2000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTB_BIT6) {
                if (ucPortFunctions[iPort][14] == PB_6_KBI0_P14) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x4000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTB_BIT7) {
                if (ucPortFunctions[iPort][15] == PB_7_KBI0_P15) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x8000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT0) {
                if (ucPortFunctions[iPort][16] == PC_0_KBI0_P16) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x10000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT1) {
                if (ucPortFunctions[iPort][17] == PC_1_KBI0_P17) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x20000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT2) {
                if (ucPortFunctions[iPort][18] == PC_2_KBI0_P18) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x40000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT3) {
                if (ucPortFunctions[iPort][19] == PC_3_KBI0_P19) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x80000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT4) {
                if (ucPortFunctions[iPort][20] == PC_4_KBI0_P20) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x100000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT5) {
                if (ucPortFunctions[iPort][21] == PC_5_KBI0_P21) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x200000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT6) {
                if (ucPortFunctions[iPort][22] == PC_6_KBI0_P22) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x400000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTC_BIT7) {
                if (ucPortFunctions[iPort][23] == PC_7_KBI0_P23) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x800000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT0) {
                if (ucPortFunctions[iPort][24] == PD_0_KBI0_P24) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x1000000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT1) {
                if (ucPortFunctions[iPort][25] == PD_1_KBI0_P25) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x2000000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT2) {
                if (ucPortFunctions[iPort][26] == PD_2_KBI0_P26) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x4000000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT3) {
                if (ucPortFunctions[iPort][27] == PD_3_KBI0_P27) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x8000000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT4) {
                if (ucPortFunctions[iPort][28] == PD_4_KBI0_P28) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x10000000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT5) {
                if (ucPortFunctions[iPort][29] == PD_5_KBI0_P29) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x20000000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT6) {
                if (ucPortFunctions[iPort][30] == PD_6_KBI0_P30) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x40000000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT7) {
                if (ucPortFunctions[iPort][31] == PD_7_KBI0_P31) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x80000000;
                    break;
                }
            }
   #endif
        }
    #if KBIS_AVAILABLE > 1 &&  !defined KINETIS_KE04 && !defined KINETIS_KE06 && !defined KINETIS_KEA64 && !defined KINETIS_KEA128
        else {
            if (ulChangedBit & KE_PORTD_BIT0) {
                if (ucPortFunctions[iPort][24] == PD_0_KBI1_P0) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x01;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT1) {
                if (ucPortFunctions[iPort][25] == PD_1_KBI1_P1) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x02;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT2) {
                if (ucPortFunctions[iPort][26] == PD_1_KBI1_P1) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x04;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT3) {
                if (ucPortFunctions[iPort][27] == PD_3_KBI1_P3) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x08;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT4) {
                if (ucPortFunctions[iPort][28] == PD_4_KBI1_P4) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x10;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT5) {
                if (ucPortFunctions[iPort][29] == PD_5_KBI1_P5) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x20;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT6) {
                if (ucPortFunctions[iPort][30] == PD_6_KBI1_P6) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x40;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT7) {
                if (ucPortFunctions[iPort][31] == PD_7_KBI1_P7) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x80;
                    break;
                }
            }
        }
    #endif
        return;
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
    case KE_PORTE:
        if (ulChangedBit & KE_PORTE_BIT0) {
            if (ucPortFunctions[iPort][0] == PE_0_KBI1_P0) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000001;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT1) {
            if (ucPortFunctions[iPort][1] == PE_1_KBI1_P1) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000002;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT2) {
            if (ucPortFunctions[iPort][2] == PE_2_KBI1_P2) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000004;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT3) {
            if (ucPortFunctions[iPort][3] == PE_3_KBI1_P3) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000008;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT4) {
            if (ucPortFunctions[iPort][4] == PE_4_KBI1_P4) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000010;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT5) {
            if (ucPortFunctions[iPort][5] == PE_5_KBI1_P5) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000020;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT6) {
            if (ucPortFunctions[iPort][6] == PE_6_KBI1_P6) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000040;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT7) {
            if (ucPortFunctions[iPort][7] == PE_7_KBI1_P7) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000080;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT0) {
            if (ucPortFunctions[iPort][8] == PF_0_KBI1_P8) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000100;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT1) {
            if (ucPortFunctions[iPort][9] == PF_1_KBI1_P9) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000200;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT2) {
            if (ucPortFunctions[iPort][10] == PF_2_KBI1_P10) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000400;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT3) {
            if (ucPortFunctions[iPort][11] == PF_3_KBI1_P11) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000800;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT4) {
            if (ucPortFunctions[iPort][12] == PF_4_KBI1_P12) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00001000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT5) {
            if (ucPortFunctions[iPort][13] == PF_5_KBI1_P13) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00002000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT6) {
            if (ucPortFunctions[iPort][14] == PF_6_KBI1_P14) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00004000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT7) {
            if (ucPortFunctions[iPort][15] == PF_7_KBI1_P15) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00008000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT0) {
            if (ucPortFunctions[iPort][16] == PG_0_KBI1_P16) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00010000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT1) {
            if (ucPortFunctions[iPort][17] == PG_1_KBI1_P17) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00020000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT2) {
            if (ucPortFunctions[iPort][18] == PG_2_KBI1_P18) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00040000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT3) {
            if (ucPortFunctions[iPort][19] == PG_3_KBI1_P19) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00080000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT4) {
            if (ucPortFunctions[iPort][20] == PG_4_KBI1_P20) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00100000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT5) {
            if (ucPortFunctions[iPort][21] == PG_5_KBI1_P21) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00200000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT6) {
            if (ucPortFunctions[iPort][22] == PG_6_KBI1_P22) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00400000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT7) {
            if (ucPortFunctions[iPort][23] == PG_7_KBI1_P23) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00800000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT0) {
            if (ucPortFunctions[iPort][24] == PH_0_KBI1_P24) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x01000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT1) {
            if (ucPortFunctions[iPort][25] == PH_1_KBI1_P25) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x02000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT2) {
            if (ucPortFunctions[iPort][26] == PH_2_KBI1_P26) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x04000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT3) {
            if (ucPortFunctions[iPort][27] == PH_3_KBI1_P27) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x08000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT4) {
            if (ucPortFunctions[iPort][28] == PH_4_KBI1_P28) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x10000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT5) {
            if (ucPortFunctions[iPort][29] == PH_5_KBI1_P29) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x20000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT6) {
            if (ucPortFunctions[iPort][30] == PH_6_KBI1_P30) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x40000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT7) {
            if (ucPortFunctions[iPort][31] == PH_7_KBI1_P31) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x80000000;
                break;
            }
        }
        return;
#endif
    default:
        return;
    }
    // A valid keyboard interrupt input has changed so we check to see whether the change/state matches with the programmed one
    //
    if (iController == 0) {
        if (KBI0_SC & KBI_SC_KBIE) {                                     // main KBI interrupt enabled
            if (KBI0_ES & KBI_input) {                                   // high or rising edge sensitive
                if ((ulNewState & ulChangedBit) == 0) {                  // input has changed to '0'
                    return;
                }
            }
            else {                                                       // low or falling edge sensitive
                if ((ulNewState & ulChangedBit) != 0) {                  // input has changed to '1'
                    return;
                }
            }
        }
        KBI0_SC |= KBI_SC_KBF;                                           // interrupt request detected
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        KBI0_SP |= ulChangedBit;                                         // flag the source that caused the interrupt
    #endif
        if (fnGenInt(irq_KBI0_ID) != 0) {                                // if the interrupt is enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_KBI0();                    // call port interrupt handler
        }
    }
    #if KBIS_AVAILABLE > 1
    else {
        if (KBI1_SC & KBI_SC_KBIE) {                                     // main KBI interrupt enabled
            if (KBI1_ES & KBI_input) {                                   // high or rising edge sensitive
                if ((ulNewState & ulChangedBit) == 0) {                  // input has changed to '0'
                    return;
                }
            }
            else {                                                       // low or falling edge sensitive
                if ((ulNewState & ulChangedBit) != 0) {                  // input has changed to '1'
                    return;
                }
            }
        }
        KBI1_SC |= KBI_SC_KBF;                                           // interrupt request detected
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        KBI1_SP |= ulChangedBit;                                         // flag the source that caused the interrupt
    #endif
        if (fnGenInt(irq_KBI1_ID) != 0) {                                // if the interrupt is enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_KBI1();                    // call port interrupt handler
        }
    }
    #endif
}
#endif

#if defined SUPPORT_ADC                                                  // {2}
static unsigned short fnConvertSimADCvalue(KINETIS_ADC_REGS *ptrADC, unsigned short usStandardValue)
{
    #if defined KINETIS_KE
    switch (ptrADC->ADC_SC3 & (ADC_CFG1_MODE_MASK)) {
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit
        usStandardValue >>= 4;
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit
        usStandardValue >>= 6;
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit
        usStandardValue >>= 8;
        break;
    }
    #else
    switch (ptrADC->ADC_CFG1 & ADC_CFG1_MODE_16) {
    case ADC_CFG1_MODE_16:                                               // conversion mode - single-ended 16 bit or differential 16 bit
        break;
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit or differential 13 bit
        usStandardValue >>= 4;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0800) {
                usStandardValue |= 0xf000;                               // sign extend
            }
        }
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit or differential 11 bit
        usStandardValue >>= 6;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0200) {
                usStandardValue |= 0xfc00;                               // sign extend
            }
        }
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit or differential 9 bit
        usStandardValue >>= 8;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0080) {
                usStandardValue |= 0xff00;                               // sign extend
            }
        }
        break;
    }
    #endif
    return usStandardValue;
}

static void fnSimADC(int iChannel)
{
    KINETIS_ADC_REGS *ptrADC;
    int iValue = 0;
    unsigned short usADCvalue;
    if (iChannel == 0) {
        ptrADC = (KINETIS_ADC_REGS *)ADC0_BLOCK;
    }
    #if ADC_CONTROLLERS > 1
    else if (iChannel == 1) {
        ptrADC = (KINETIS_ADC_REGS *)ADC1_BLOCK;
        iValue = ADC_CHANNELS;
    }
    #endif
    #if ADC_CONTROLLERS > 2
    else if (iChannel == 2) {
        ptrADC = (KINETIS_ADC_REGS *)ADC2_BLOCK;
        iValue = (ADC_CHANNELS * 2);
    }
    #endif
    #if ADC_CONTROLLERS > 3
    else if (iChannel == 3) {
        ptrADC = (KINETIS_ADC_REGS *)ADC3_BLOCK;
        iValue = (ADC_CHANNELS * 3);
    }
    #endif
    else {
        return;
    }
    iValue += (ptrADC->ADC_SC1A & ADC_SC1A_ADCH_OFF);                    // the input being converted
    usADCvalue = fnConvertSimADCvalue(ptrADC, usADC_values[iValue]);     // convert the standard value to the format used by the present mode
    if ((ptrADC->ADC_SC2 & ADC_SC2_ACFE) != 0) {                         // {40} if the compare function is enabled
    #if !defined KINETIS_KE
        if ((ptrADC->ADC_SC2 & ADC_SC2_ACREN) != 0) {                    // range enabled (uses CV1 and CV2)
            if ((ptrADC->ADC_SC2 & ADC_SC2_ACFGT) != 0) {                // greater or equal
                if (ptrADC->ADC_CV1 <= ptrADC->ADC_CV2) {                // inside range inclusive
                    if ((usADCvalue < ptrADC->ADC_CV1) || (usADCvalue > ptrADC->ADC_CV2)) {
                        return;
                    }
                }
                else {                                                   // outside range inclusive
                    if ((usADCvalue < ptrADC->ADC_CV1) && (usADCvalue > ptrADC->ADC_CV2)) {
                        return;
                    }
                }
            }
            else {
                if (ptrADC->ADC_CV1 <= ptrADC->ADC_CV2) {                // outside range not inclusive
                    if ((usADCvalue >= ptrADC->ADC_CV1) && (usADCvalue <= ptrADC->ADC_CV2)) {
                        return;
                    }
                }
                else {                                                   // inside range not inclusive
                    if ((usADCvalue >= ptrADC->ADC_CV1) || (usADCvalue <= ptrADC->ADC_CV2)) {
                        return;
                    }
                }
            }
        }
        else {                                                           // uses only CV1
    #endif
            if (ptrADC->ADC_SC2 & ADC_SC2_ACFGT) {                       // greater or equal
                if (usADCvalue < ptrADC->ADC_CV1) {                      // if the range is not true the result is not copied to the data register and the conversion is not complete
                    return;
                }
            }
            else {                                                       // less
                if (usADCvalue >= ptrADC->ADC_CV1) {                     // if the range is not true the result is not copied to the data register and the conversion is not complete
                    return;
                }
            }
        }
    #if !defined KINETIS_KE
    }
    #endif
    ptrADC->ADC_RA = usADCvalue;                                         // set the latest voltage to the result register
    ptrADC->ADC_SC1A |= ADC_SC1A_COCO;                                   // set end of conversion flag
}
#endif

#if defined SUPPORT_LLWU && defined LLWU_AVAILABLE
static const unsigned char cWakeupPorts[PORTS_AVAILABLE][PORT_WIDTH] = {
    #if defined KINETIS_KL03
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    #elif defined KINETIS_KL05
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P0, LLWU_P1, LLWU_P2, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, LLWU_P5, NO_WAKEUP, LLWU_P6, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },

    #elif defined KINETIS_KL46
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P0, LLWU_P1, LLWU_P2, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, LLWU_P5, NO_WAKEUP, LLWU_P6, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #elif defined KINETIS_KL
    // PTA0..PTA31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P5, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, LLWU_P6, NO_WAKEUP, LLWU_P7, LLWU_P8, LLWU_P9, LLWU_P10, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P14, NO_WAKEUP, LLWU_P15, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #else
    // PTA0..PTA31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P5, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, LLWU_P6, NO_WAKEUP, LLWU_P7, LLWU_P8, LLWU_P9, LLWU_P10, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P11, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { LLWU_P12, NO_WAKEUP, LLWU_P13, NO_WAKEUP, LLWU_P14, NO_WAKEUP, LLWU_P15, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, LLWU_P0, LLWU_P1, NO_WAKEUP, LLWU_P2, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #endif
};
static void fnWakeupInterrupt(int iPortReference, unsigned long ulPortState, unsigned long ulBit, unsigned char ucPortBit)
{
    ucPortBit = ((PORT_WIDTH - 1) - ucPortBit);
    if (cWakeupPorts[iPortReference][ucPortBit] == NO_WAKEUP) {
        return;                                                          // this pin doesn't support low-leakage wakup
    }
    else {
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        volatile unsigned char *ptrFlagRegister = LLWU_FLAG_ADDRESS + (cWakeupPorts[iPortReference][ucPortBit]/8);
        unsigned char *ptrWakeupEnable = (unsigned char *)LLWU_BLOCK + (cWakeupPorts[iPortReference][ucPortBit]/4);
        unsigned int iPositionShift = ((cWakeupPorts[iPortReference][ucPortBit]%4) * LLWU_PE_WUPE_SHIFT); // shift in the register required to extract the input configuration
        switch ((*ptrWakeupEnable >> iPositionShift) & LLWU_PE_WUPE_MASK) {
        case LLWU_PE_WUPE_FALLING:
            if ((ulPortState & ulBit) != 0) {
                return;                                                  // a rising edge doesn't match
            }
            break;
        case LLWU_PE_WUPE_RISING:
            if ((ulPortState & ulBit) == 0) {
                return;                                                  // a falling edge doesn't match
            }
            break;
        case LLWU_PE_WUPE_CHANGE:                                        // a change always matches
            break;
        case LLWU_PE_WUPE_OFF:
            return;                                                      // function is disable on this pin
        }
        // The input change matches
        //
        *ptrFlagRegister = (LLWU_F_WUF0 << (cWakeupPorts[iPortReference][ucPortBit]%8)); // set source flag
        ptrVect->processor_interrupts.irq_LL_wakeup();                   // call wakeup interrupt handler
    }
}
#endif

#if defined SUPPORT_ADC
extern int fnGetADC_sim_channel(int iPort, int iBit);
static int fnHandleADCchange(int iChange, int iPort, unsigned char ucPortBit)
{
    if ((iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG | TOGGLE_INPUT_POS)) != 0) {
        unsigned short usStepSize;
        signed int iAdc = fnGetADC_sim_channel(iPort, (/*31 - */ucPortBit)); // {9}
        if (iAdc < 0) {                                                  // {9} ignore if not valid ADC port
            return -1;                                                   // not analoge input so ignore
        }
        if ((TOGGLE_INPUT_ANALOG & iChange) != 0) {
            usStepSize = (0xffff/3);
        }
        else {
            usStepSize = ((ADC_SIM_STEP_SIZE * 0xffff) / ADC_REFERENCE_VOLTAGE);
        }
        if ((TOGGLE_INPUT_NEG & iChange) != 0) {                         // force a smaller voltage
            if (usADC_values[iAdc] >= usStepSize) {
                usADC_values[iAdc] -= usStepSize;                        // decrease the voltage on the pin
            }
        }
        else {
            if ((usADC_values[iAdc] + usStepSize) <= 0xffff) {
                usADC_values[iAdc] += usStepSize;                        // increase the voltage on the pin
            }
        }
    }
    return 0;
}
#endif

// Simulate setting, clearing or toggling of an input pin
//
extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
#if defined KINETIS_KL
    static unsigned long ulTSI[61] = {0};
#endif
#if defined KINETIS_KE
    unsigned long ulBit;
#else
    unsigned long ulBit = (0x80000000 >> ucPortBit);
    unsigned long *ptrPCR = (unsigned long *)PORT0_BLOCK;
    ptrPCR += (ucPort * sizeof(KINETIS_PORT)/sizeof(unsigned long));
#endif
    iFlagRefresh = PORT_CHANGE;
#if defined SUPPORT_ADC
    if (fnHandleADCchange(iChange, ucPort, ucPortBit) == 0) {            // handle possible ADC function on pin
        return;                                                          // if ADC we do not handle digital functions
    }
#endif
#if defined KINETIS_KE                                                   // KE uses byte terminology but physically hve long word ports
    ulBit = (0x80000000 >> (ucPortBit + ((3 - (ucPort % 4)) * 8))); // convert to long word port representation
    ucPort /= 4;
#endif
    switch (ucPort) {
    case _PORTA:
        if ((~GPIOA_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_A;
#if !defined KINETIS_KE
            if ((SIM_SCGC5 & SIM_SCGC5_PORTA) == 0) {                    // ignore if port is not clocked
                return;
            }
#endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_A ^= ulBit;                                    // set new pin state
#if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
#endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOA_PDIR &= ~ulBit;
                GPIOA_PDIR |= (ulPort_in_A & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_A |= ulBit;
#if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
#endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOA_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_A &= ~ulBit;
#if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
#endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOA_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_A != ulOriginal_port_state) {                  // if a change took place
#if defined SUPPORT_LLWU && defined LLWU_AVAILABLE
                fnWakeupInterrupt(_PORTA, ulPort_in_A, ulBit, ucPortBit);// handle wakeup events on the pin
#endif
#if defined KINETIS_KE
                fnPortInterrupt(_PORTA, ulPort_in_A, ulBit, 0);          // handle interrupts on the pin
#else
                fnPortInterrupt(_PORTA, ulPort_in_A, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
#endif
            }
        }
        break;
#if PORTS_AVAILABLE > 1
    case _PORTB:
        if ((~GPIOB_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_B;
    #if !defined KINETIS_KE
            if ((SIM_SCGC5 & SIM_SCGC5_PORTB) == 0) {                    // ignore if port is not clocked
                return;
            }
    #endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_B ^= ulBit;                                    // set new pin state
    #if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOB_PDIR &= ~ulBit;
                GPIOB_PDIR |= (ulPort_in_B & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_B |= ulBit;
    #if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOB_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_B &= ~ulBit;
    #if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOB_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_B != ulOriginal_port_state) {                  // if a change took place
    #if defined SUPPORT_LLWU && defined LLWU_AVAILABLE
                fnWakeupInterrupt(_PORTB, ulPort_in_B, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
    #if defined KINETIS_KE
                fnPortInterrupt(_PORTB, ulPort_in_B, ulBit, 0);          // handle interrupts on the pin
    #else
                fnPortInterrupt(_PORTB, ulPort_in_B, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
    #endif
            }
        }
        break;
#endif
#if PORTS_AVAILABLE > 2
    case _PORTC:
        if (~GPIOC_PDDR & ulBit) {                                       // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_C;
    #if !defined KINETIS_KE
            if ((SIM_SCGC5 & SIM_SCGC5_PORTC) == 0) {                    // ignore if port is not clocked
                return;
            }
    #endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_C ^= ulBit;                                    // set new pin state
    #if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOC_PDIR &= ~ulBit;
                GPIOC_PDIR |= (ulPort_in_C & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_C |= ulBit;
    #if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOC_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_C &= ~ulBit;
    #if !defined KINETIS_KE
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOC_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_C != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTC, ulPort_in_C, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
    #if defined KINETIS_KE
                fnPortInterrupt(_PORTC, ulPort_in_C, ulBit, 0);          // handle interrupts on the pin
    #else
                fnPortInterrupt(_PORTC, ulPort_in_C, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
    #endif
            }
        }
        break;
#endif
#if PORTS_AVAILABLE > 3
    case _PORTD:
        if (~GPIOD_PDDR & ulBit) {                                       // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_D;
            if (!(SIM_SCGC5 & SIM_SCGC5_PORTD)) {                        // ignore if port is not clocked
                return;
            }
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_D ^= ulBit;                                    // set new pin state
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOD_PDIR &= ~ulBit;
                GPIOD_PDIR |= (ulPort_in_D & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_D |= ulBit;
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOD_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_D &= ~ulBit;
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOD_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_D != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTD, ulPort_in_D, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
                fnPortInterrupt(_PORTD, ulPort_in_D, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
            }
        }
        break;
    case _PORTE:
        if ((~GPIOE_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_E;
            if ((SIM_SCGC5 & SIM_SCGC5_PORTE) == 0) {                    // ignore if port is not clocked
                return;
            }
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_E ^= ulBit;                                    // set new pin state
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOE_PDIR &= ~ulBit;
                GPIOE_PDIR |= (ulPort_in_E & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_E |= ulBit;
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOE_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_E &= ~ulBit;
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOE_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_E != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTE, ulPort_in_E, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
                fnPortInterrupt(_PORTE, ulPort_in_E, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
            }
        }
        break;
#endif
#if PORTS_AVAILABLE > 5
    case _PORTF:
        if ((~GPIOF_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_F;
            if ((SIM_SCGC5 & SIM_SCGC5_PORTF) == 0) {                    // ignore if port is not clocked
                return;
            }
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_F ^= ulBit;                                    // set new pin state
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOF_PDIR &= ~ulBit;
                GPIOF_PDIR |= (ulPort_in_F & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_F |= ulBit;
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOF_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_F &= ~ulBit;
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOF_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_F != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTF, ulPort_in_F, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
                fnPortInterrupt(_PORTF, ulPort_in_F, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
            }
        }
        break;
#endif
#if defined KINETIS_KL
    case _TOUCH_PORTB:
        if (ulBit == PORTB_BIT16) {                                       // channel 9 on PTB16
            if (iChange == TOGGLE_INPUT) {
                ulTSI[9] = ~ulTSI[9];
            }
            else if (iChange == SET_INPUT) {
                ulTSI[9] = 0;
            }
            else {
                ulTSI[9] = 1;
            }
        }
        else if (ulBit == PORTB_BIT17) {                                 // channel 10 on PTB17
            if (iChange == TOGGLE_INPUT) {
                ulTSI[10] = ~ulTSI[10];
            }
            else if (iChange == SET_INPUT) {
                ulTSI[10] = 0;
            }
            else {
                ulTSI[10] = 1;
            }
        }
        break;
#else
    case _TOUCH_PORTA:                                                   // {5} touch sensor
        if (ulBit == PORTA_BIT0) {                                       // channel 1 on PTA0
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR1 = ((~TSI0_CNTR1 & 0xffff0000) | (TSI0_CNTR1 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR1 = (TSI0_CNTR1 & 0x0000ffff);
            }
            else {
                TSI0_CNTR1 = (TSI0_CNTR1 | 0xffff0000);
            }
        }
        else if (ulBit == PORTA_BIT1) {                                  // channel 2 on PTA1
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR3 = ((~TSI0_CNTR3 & 0x0000ffff) | (TSI0_CNTR3 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR3 = (TSI0_CNTR3 & 0xffff0000);
            }
            else {
                TSI0_CNTR3 = (TSI0_CNTR3 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTA_BIT2) {                                  // channel 3 on PTA2
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR3 = ((~TSI0_CNTR3 & 0xffff0000) | (TSI0_CNTR3 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR3 = (TSI0_CNTR3 & 0x0000ffff);
            }
            else {
                TSI0_CNTR3 = (TSI0_CNTR3 | 0xffff0000);
            }
        }
        else if (ulBit == PORTA_BIT3) {                                  // channel 4 on PTA3
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR5 = ((~TSI0_CNTR5 & 0x0000ffff) | (TSI0_CNTR5 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR5 = (TSI0_CNTR5 & 0xffff0000);
            }
            else {
                TSI0_CNTR5 = (TSI0_CNTR5 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTA_BIT4) {                                  // channel 5 on PTA4
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR5 = ((~TSI0_CNTR5 & 0xffff0000) | (TSI0_CNTR5 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR5 = (TSI0_CNTR5 & 0x0000ffff);
            }
            else {
                TSI0_CNTR5 = (TSI0_CNTR5 | 0xffff0000);
            }
        }
        if (iChange == TOGGLE_INPUT) {
            ulPort_in_A ^= ulBit;
        }
        else if (iChange == SET_INPUT) {
            ulPort_in_A |= ulBit;
        }
        else {
            ulPort_in_A &= ~ulBit;
        }
        break;
    case _TOUCH_PORTB:
        if (ulBit == PORTB_BIT0) {                                       // channel 0 on PTB0
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR1 = ((~TSI0_CNTR1 & 0x0000ffff) | (TSI0_CNTR1 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR1 = (TSI0_CNTR1 & 0xffff0000);
            }
            else {
                TSI0_CNTR1 = (TSI0_CNTR1 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT1) {                                  // channel 6 on PTB1
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR7 = ((~TSI0_CNTR7 & 0x0000ffff) | (TSI0_CNTR7 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR7 = (TSI0_CNTR7 & 0xffff0000);
            }
            else {
                TSI0_CNTR7 = (TSI0_CNTR7 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT2) {                                  // channel 7 on PTB2
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR7 = ((~TSI0_CNTR7 & 0xffff0000) | (TSI0_CNTR7 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR7 = (TSI0_CNTR7 & 0x0000ffff);
            }
            else {
                TSI0_CNTR7 = (TSI0_CNTR7 | 0xffff0000);
            }
        }
        else if (ulBit == PORTB_BIT3) {                                  // channel 8 on PTB3
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR9 = ((~TSI0_CNTR9 & 0x0000ffff) | (TSI0_CNTR9 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR9 = (TSI0_CNTR9 & 0xffff0000);
            }
            else {
                TSI0_CNTR9 = (TSI0_CNTR9 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT16) {                                 // channel 9 on PTB16
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR9 = ((~TSI0_CNTR9 & 0xffff0000) | (TSI0_CNTR9 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR9 = (TSI0_CNTR9 & 0x0000ffff);
            }
            else {
                TSI0_CNTR9 = (TSI0_CNTR9 | 0xffff0000);
            }
        }
        else if (ulBit == PORTB_BIT17) {                                 // channel 10 on PTB17
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR11 = ((~TSI0_CNTR11 & 0x0000ffff) | (TSI0_CNTR11 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR11 = (TSI0_CNTR11 & 0xffff0000);
            }
            else {
                TSI0_CNTR11 = (TSI0_CNTR11 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT18) {                                 // channel 11 on PTB18
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR11 = ((~TSI0_CNTR9 & 0xffff0000) | (TSI0_CNTR11 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR11 = (TSI0_CNTR11 & 0x0000ffff);
            }
            else {
                TSI0_CNTR11 = (TSI0_CNTR11 | 0xffff0000);
            }
        }
        else if (ulBit == PORTB_BIT19) {                                 // channel 12 on PTB19
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR13 = ((~TSI0_CNTR13 & 0x0000ffff) | (TSI0_CNTR13 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR13 = (TSI0_CNTR13 & 0xffff0000);
            }
            else {
                TSI0_CNTR13 = (TSI0_CNTR13 | 0x0000ffff);
            }
        }
        if (iChange == TOGGLE_INPUT) {
            ulPort_in_B ^= ulBit;
        }
        else if (iChange == SET_INPUT) {
            ulPort_in_B |= ulBit;
        }
        else {
            ulPort_in_B &= ~ulBit;
        }
        break;
#if PORTS_AVAILABLE > 2
    case _TOUCH_PORTC:
        if (ulBit == PORTC_BIT0) {                                       // channel 13 on PTC0
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR13 = ((~TSI0_CNTR13 & 0xffff0000) | (TSI0_CNTR13 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR13 = (TSI0_CNTR13 & 0x0000ffff);
            }
            else {
                TSI0_CNTR13 = (TSI0_CNTR13 | 0xffff0000);
            }
        }
        else if (ulBit == PORTC_BIT1) {                                  // channel 14 on PTC1
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR15 = ((~TSI0_CNTR15 & 0x0000ffff) | (TSI0_CNTR15 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR15 = (TSI0_CNTR15 & 0xffff0000);
            }
            else {
                TSI0_CNTR15 = (TSI0_CNTR15 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTC_BIT2) {                                  // channel 15 on PTC2
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR15 = ((~TSI0_CNTR15 & 0xffff0000) | (TSI0_CNTR15 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR15 = (TSI0_CNTR15 & 0x0000ffff);
            }
            else {
                TSI0_CNTR15 = (TSI0_CNTR15 | 0xffff0000);
            }
        }
        if (iChange == TOGGLE_INPUT) {
            ulPort_in_C ^= ulBit;
        }
        else if (iChange == SET_INPUT) {
            ulPort_in_C |= ulBit;
        }
        else {
            ulPort_in_C &= ~ulBit;
        }
        break;
#endif
#endif
    case _PORT_EXP_0:                                                    // {8} external ports extensions
    case _PORT_EXP_1:
    case _PORT_EXP_2:
    case _PORT_EXP_3:
    case _PORT_EXP_4:
    case _PORT_EXP_5:
    case _PORT_EXP_6:
    case _PORT_EXP_7:
#if defined HANDLE_EXT_PORT
        HANDLE_EXT_PORT(ucPort, iChange, ulBit);
#endif
        break;
    }
}

// if either PORTx_GPCLR of PORTx_GPCHR have been written to, set the PCD registers accordingly
//
static void fnSetPinCharacteristics(int iPortRef, unsigned long ulHigh, unsigned long ulLow)
{
    unsigned long ulBit = 0x00010000;
#if !defined KINETIS_KE
    unsigned long *ptrPCR = (unsigned long *)(PORT0_BLOCK + (iPortRef * 0x1000));
#endif
    while (ulBit != 0) {
        if ((ulLow & ulBit) != 0) {
#if !defined KINETIS_KE
            *ptrPCR = (ulLow & 0x0000ffff);
#endif
        }
#if !defined KINETIS_KE
        ptrPCR++;
#endif
        ulBit <<= 1;
    }
    ulBit = 0x00010000;
    while (ulBit != 0) {
        if ((ulHigh & ulBit) != 0) {
#if !defined KINETIS_KE
            *ptrPCR = (ulHigh & 0x0000ffff);
#endif
        }
#if !defined KINETIS_KE
        ptrPCR++;
#endif
        ulBit <<= 1;
    }
#if !defined KINETIS_KE
    *ptrPCR++ = 0;                                                       // clear the PORTx_GPCLR and PORTx_GPCHR registers, which read always 0
    *ptrPCR = 0;
#endif
}

// Update ports based on present register settings
//
extern void fnSimPorts(void)
{
    unsigned long ulNewState;
#if !defined KINETIS_KE
    if ((SIM_SCGC5 & SIM_SCGC5_PORTA) != 0) {                            // if port is clocked
#endif
        ulNewState = (GPIOA_PDOR | GPIOA_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOA_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOA_PTOR;                                        // toggle bits from toggle register
        GPIOA_PDOR = ulNewState;
        GPIOA_PDIR = ((ulPort_in_A & ~GPIOA_PDDR) | (GPIOA_PDOR & GPIOA_PDDR)); // input state {10}
#if !defined KINETIS_KE
        if ((PORTA_GPCLR != 0) || (PORTA_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTA, PORTA_GPCHR, PORTA_GPCLR);
        }
#endif
#if !defined KINETIS_KE
    }
#endif
    GPIOA_PTOR = GPIOA_PSOR = GPIOA_PCOR = 0;                            // registers always read 0

#if PORTS_AVAILABLE > 1
    #if !defined KINETIS_KE
    if ((SIM_SCGC5 & SIM_SCGC5_PORTB) != 0) {                            // if port is clocked
    #endif
        ulNewState = (GPIOB_PDOR | GPIOB_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOB_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOB_PTOR;                                        // toggle bits from toggle register
        GPIOB_PDOR = ulNewState;
        GPIOB_PDIR = ((ulPort_in_B & ~GPIOB_PDDR) | (GPIOB_PDOR & GPIOB_PDDR)); // input state {10}
    #if !defined KINETIS_KE
        if ((PORTB_GPCLR != 0) || (PORTB_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTB, PORTB_GPCHR, PORTB_GPCLR);
        }
    #endif
    #if !defined KINETIS_KE
    }
    #endif
    GPIOB_PTOR = GPIOB_PSOR = GPIOB_PCOR = 0;                            // registers always read 0
#endif
#if PORTS_AVAILABLE > 2
    #if !defined KINETIS_KE
    if ((SIM_SCGC5 & SIM_SCGC5_PORTC) != 0) {                            // if port is clocked
    #endif
        ulNewState = (GPIOC_PDOR | GPIOC_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOC_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOC_PTOR;                                        // toggle bits from toggle register
        GPIOC_PDOR = ulNewState;
        GPIOC_PDIR = ((ulPort_in_C & ~GPIOC_PDDR) | (GPIOC_PDOR & GPIOC_PDDR)); // input state {10}
    #if !defined KINETIS_KE
        if ((PORTC_GPCLR != 0) || (PORTC_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTC, PORTC_GPCHR, PORTC_GPCLR);
        }
    #endif
    #if !defined KINETIS_KE
    }
    #endif
    GPIOC_PTOR = GPIOC_PSOR = GPIOC_PCOR = 0;                            // registers always read 0
#endif
#if PORTS_AVAILABLE > 3
    if ((SIM_SCGC5 & SIM_SCGC5_PORTD) != 0) {                            // if port is clocked
        ulNewState = (GPIOD_PDOR | GPIOD_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOD_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOD_PTOR;                                        // toggle bits from toggle register
        GPIOD_PDOR = ulNewState;
        GPIOD_PDIR = ((ulPort_in_D & ~GPIOD_PDDR) | (GPIOD_PDOR & GPIOD_PDDR)); // input state {10}
        if ((PORTD_GPCLR != 0) || (PORTD_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTD, PORTD_GPCHR, PORTD_GPCLR);
        }
    }
    GPIOD_PTOR = GPIOD_PSOR = GPIOD_PCOR = 0;                            // registers always read 0

    if ((SIM_SCGC5 & SIM_SCGC5_PORTE) != 0) {                            // if port is clocked
        ulNewState = (GPIOE_PDOR | GPIOE_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOE_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOE_PTOR;                                        // toggle bits from toggle register
        GPIOE_PDOR = ulNewState;
        GPIOE_PDIR = ((ulPort_in_E & ~GPIOE_PDDR) | (GPIOE_PDOR & GPIOE_PDDR)); // input state {10}
        if ((PORTE_GPCLR != 0) || (PORTE_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTE, PORTE_GPCHR, PORTE_GPCLR);
        }
    }
    GPIOE_PTOR = GPIOE_PSOR = GPIOE_PCOR = 0;                            // registers always read 0
#endif
#if PORTS_AVAILABLE > 5
    if ((SIM_SCGC5 & SIM_SCGC5_PORTF) != 0) {                            // if port is clocked
        ulNewState = (GPIOF_PDOR | GPIOF_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOF_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOF_PTOR;                                        // toggle bits from toggle register
        GPIOF_PDOR = ulNewState;
        GPIOF_PDIR = ((ulPort_in_F & ~GPIOF_PDDR) | (GPIOF_PDOR & GPIOF_PDDR)); // input state {10}
        if ((PORTF_GPCLR != 0) || (PORTF_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTF, PORTF_GPCHR, PORTF_GPCLR);
        }
    }
    GPIOF_PTOR = GPIOF_PSOR = GPIOF_PCOR = 0;                            // registers always read 0
#endif
}


// Update peripherals based on present port register settings - this is only called when a peripheral setting has been changed
//
extern void fnSimPers(void)
{
    int iPort = 0;
    int iPin = 0;
#if !defined KINETIS_KE
    unsigned long *ptrPortPin;
#endif
    unsigned long ulBit;
    for (iPort = 0; iPort < PORTS_AVAILABLE; iPort++) {
#if !defined KINETIS_KE
        ptrPortPin = (unsigned long *)(PORT0_BLOCK + (iPort * sizeof(KINETIS_PORT)));
#endif
        ulPeripherals[iPort] = 0;
        ulBit = 0x00000001;
        for (iPin = 0; iPin < 32; iPin++) {
#if defined KINETIS_KE
            // KE devices' peripheral functions are selected by the various peripherals that can use the pins. Each peripheral pin function has a priority and so the the one with highest is valid in case of multiple enabled peripherals on a pin
            // Peripheral can also have pin multiplexing options
            //
            if (iPort == 0) {
                switch (ulBit) {
                case KE_PORTA_BIT0:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD0) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_ADC0_SE0;
                        break;
                    }
    #endif
    #if (SIZE_OF_FLASH <= (8 * 1024))
                    if ((SIM_SOPT0 & SIM_SOPT_SWDE) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_SWD_CLK;
                    }
                    else
    #endif
                    if ((FTM0_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && ((SIM_PINSEL0 & SIM_PINSEL_FTM0PS0) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_FTM0_CH0;
                    }
                    else if (KBI0_PE & 0x01) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_KBI0_P0;
                    }
                    break;
                case KE_PORTA_BIT1:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD1) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_ADC0_SE1;
                        break;
                    }
    #endif
                    if ((FTM0_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && ((SIM_PINSEL0 & SIM_PINSEL_FTM0PS1) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_FTM0_CH1;                    
                    }
                    else if (KBI0_PE & 0x02) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_KBI0_P1;
                    }
    #if defined PA_1_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTA_BIT2:
                    if ((UART0_C2 & UART_C2_RE) && (SIM_PINSEL0 & SIM_PINSEL_UART0PS) && ((UART0_C1 & UART_C1_LOOPS) == 0)) { // UART0 rx enabled and mapped to PTA2 and PTA3 rather than PTB0 and PTB1 (and not in sigle-wire/loop back mode)
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_2_UART0_RX;
                    }
                    else if ((I2C0_C1 & I2C_IEN) && ((SIM_PINSEL0 & SIM_PINSEL_I2C0PS) == 0)) { // if I2C is enabled and I2C0 not mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_2_I2C0_SDA;
                    }
                    else if (KBI0_PE & 0x04) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_2_KBI0_P2;
                    }
                    break;
                case KE_PORTA_BIT3:
                    if ((UART0_C2 & UART_C2_TE) && (SIM_PINSEL0 & SIM_PINSEL_UART0PS)) { // UART0 tx enabled and mapped to PTA2 and PTA3 rather than PTB0 and PTB1
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_3_UART0_TX;
                    }
                    else if ((I2C0_C1 & I2C_IEN) && (!(SIM_PINSEL0 & SIM_PINSEL_I2C0PS))) { // if I2C is enabled and I2C0 not mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_3_I2C0_SCL;
                    }
                    else if (KBI0_PE & 0x08) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_3_KBI0_P3;
                    }
                    break;
                case KE_PORTA_BIT4:
                    if (SIM_SOPT0 & SIM_SOPT_SWDE) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_4_SWD_DIO;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x10) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_4_KBI0_P4;
                    }
    #endif
                    break;
                case KE_PORTA_BIT5:
                    if ((SIM_SOPT0 & SIM_SOPT_RSTPE) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_5_RESET;
                    }
                    else if ((IRQ_SC & IRQ_SC_IRQPE) != 0) {             // IRQ pin function enabled
    #if defined SIM_PINSEL_IRQPS_PTI6
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTA5) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTA][iPin] = PA_5_IRQ;
                        }
    #else
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[iPort][iPin] = PA_5_IRQ;
    #endif
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x20) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_5_KBI0_P5;
                    }
    #endif
                    break;
                case KE_PORTA_BIT6:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD6) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_6_ADC0_SE2;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI0_PE & 0x40) != 0) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_6_KBI0_P6;
                    }
    #endif
    #if defined PA_6_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_6_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTA_BIT7:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD7) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_7_ADC0_SE3;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI0_PE & 0x80) != 0) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_7_KBI0_P7;
                    }
    #endif
    #if defined PA_7_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_7_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT0:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD4) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_ADC0_SE4;
                        break;
                    }
    #endif
                    if ((UART0_C2 & UART_C2_RE) && ((SIM_PINSEL0 & SIM_PINSEL_UART0PS) == 0) && ((UART0_C1 & UART_C1_LOOPS) == 0)) { // UART0 rx enabled and not mapped to PTA2 and PTA3 rather than PTB0 and PTB1 (and not in sigle-wire/loop back mode)
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_UART0_RX;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x100) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_KBI0_P8;
                    }
    #else
                    else if (KBI0_PE & 0x10) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_KBI0_P4;
                    }
    #endif
                    break;
                case KE_PORTB_BIT1:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD5) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_ADC0_SE5;
                        break;
                    }
    #endif
                    if ((UART0_C2 & UART_C2_TE) && (!(SIM_PINSEL0 & SIM_PINSEL_UART0PS) == 0)) { // UART0 tx enabled and not mapped to PTA2 and PTA3 rather than PTB0 and PTB1
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_UART0_TX;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x200) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_KBI0_P9;
                    }
    #else
                    else if (KBI0_PE & 0x20) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_KBI0_P5;
                    }
    #endif
                    break;
                case KE_PORTB_BIT2:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD6) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_ADC0_SE6;
                        break;
                    }
    #endif
                    if ((FTM0_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM0PS0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_FTM0_CH0;                    
                    }
                    else if ((SPI0_C1 & SPI_C1_MSTR) && (!(SIM_PINSEL0 & SIM_PINSEL_SPI0PS))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_SPI0_SCK;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x400) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_KBI0_P10;
                    }
    #else
                    else if (KBI0_PE & 0x40) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_KBI0_P6;
                    }
    #endif
    #if defined PB_2_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT3:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD7) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_ADC0_SE7;
                        break;
                    }
    #endif
                    if ((FTM0_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM0PS1)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_FTM0_CH1;                    
                    }
                    else if ((SPI0_C1 & SPI_C1_MSTR) && (!(SIM_PINSEL0 & SIM_PINSEL_SPI0PS))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_SPI0_MOSI;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x800) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_KBI0_P11;
                    }
    #else
                    else if (KBI0_PE & 0x80) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_KBI0_P7;
                    }
    #endif
    #if defined PB_3_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT4:
                    if (SIM_SOPT0 & SIM_SOPT_NMIE) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_NMI;
                    }
                    else if ((SPI0_C1 & SPI_C1_MSTR) && ((SIM_PINSEL0 & SIM_PINSEL_SPI0PS) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_SPI0_MISO;
                    }
                    else if ((FTM2_C4SC & (FTM_CSC_ELSA | FTM_CSC_ELSB))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_FTM2_CH4;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x1000) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_KBI0_P12;
                    }
    #endif
    #if defined PB_4_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT5:
                    if ((SPI0_C1 & SPI_C1_MSTR) && (!(SIM_PINSEL0 & SIM_PINSEL_SPI0PS)) &&(SPI0_C1 & SPI_C1_SSOE)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_SPI0_PCS0;
                    }
                    else if ((FTM2_C5SC & (FTM_CSC_ELSA | FTM_CSC_ELSB))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_FTM2_CH5;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x2000) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_KBI0_P13;
                    }
    #endif
    #if defined PB_5_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT6:
                    if ((I2C0_C1 & I2C_IEN) && (SIM_PINSEL0 & SIM_PINSEL_I2C0PS)) { // if I2C is enabled and I2C0 is mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_6_I2C0_SDA;
                    }
                    else if (OSC0_CR & OSC_CR_OSCEN) {                   // crystal oscillator enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_6_XTAL;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x4000) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_6_KBI0_P14;
                    }
    #endif
                    break;
                case KE_PORTB_BIT7:
                    if ((I2C0_C1 & I2C_IEN) && (SIM_PINSEL0 & SIM_PINSEL_I2C0PS)) { // if I2C is enabled and I2C0 is mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_7_I2C0_SCL;
                    }
                    else if (OSC0_CR & OSC_CR_OSCEN) {                   // crystal oscillator enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_7_EXTAL;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x8000) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_7_KBI0_P15;
                    }
    #endif
                    break;
                case KE_PORTC_BIT0:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD8) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_0_ADC0_SE8;
                        break;
                    }
    #endif
                    if ((FTM2_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && ((SIM_PINSEL0 & SIM_PINSEL_FTM1PS1) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_0_FTM2_CH0;                    
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x10000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_0_KBI0_P16;
                    }
    #endif
    #if defined PC_0_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_0_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT1:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD9) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_1_ADC0_SE9;
                        break;
                    }
    #endif
                    if ((FTM2_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL0 & SIM_PINSEL_FTM1PS1))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_1_FTM2_CH1;                    
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x20000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_1_KBI0_P17;
                    }
    #endif
    #if defined PC_1_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_1_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT2:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD10) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_2_ADC0_SE10;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 ||  defined KINETIS_KEA128
                    if ((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL1 & (SIM_PINSEL1_FTM2PS2_PTD0 | SIM_PINSEL1_FTM2PS2_PTG4)))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_2_FTM2_CH2;                    
                    }
    #else
                    if ((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL0 & SIM_PINSEL_FTM2PS2))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[iPort + 2][iPin - 16] = PC_2_FTM2_CH2;                    
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x40000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_2_KBI0_P18;
                    }
    #endif
    #if defined PC_2_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_2_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT3:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD11) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_3_ADC0_SE11;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL1 & (SIM_PINSEL1_FTM2PS3_PTD1 | SIM_PINSEL1_FTM2PS3_PTG5)))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_FTM2_CH3;                    
                    }
    #else
                    if ((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL0 & SIM_PINSEL_FTM2PS3))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_FTM2_CH3;                    
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x80000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_KBI0_P19;
                    }
    #endif
    #if defined PC_3_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT4:
                    if ((FTM1_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL0 & SIM_PINSEL_FTM1PS0))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_4_FTM1_CH0;                    
                    }
    #if (SIZE_OF_FLASH > (8 * 1024))
                    else if (SIM_SOPT0 & SIM_SOPT_SWDE) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_4_SWD_CLK;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x100000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_4_KBI0_P20;
                    }
    #endif
                    break;
                case KE_PORTC_BIT5:
                    if ((FTM1_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL0 & SIM_PINSEL_FTM1PS1))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_5_FTM1_CH1;                    
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x200000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_5_KBI0_P21;
                    }
    #endif
                    break;
                case KE_PORTC_BIT6:
    #if defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) && ((SIM_PINSEL1 & SIM_PINSEL1_MSCANPS) == 0)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_6_CAN0_RX;   
                    }
                    else
    #endif
    #if UARTS_AVAILABLE > 1
                    if ((UART1_C2 & UART_C2_RE) && ((UART1_C1 & UART_C1_LOOPS) == 0)) { // UART1 rx enabled and not in single-wire/loop back mode
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_6_UART1_RX; 
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x400000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_6_KBI0_P22;
                    }
    #endif
                    break;
                case KE_PORTC_BIT7:
    #if defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) && ((SIM_PINSEL1 & SIM_PINSEL1_MSCANPS) == 0)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_7_CAN0_TX;   
                    }
                    else
    #endif
    #if UARTS_AVAILABLE > 1
                    if (UART1_C2 & UART_C2_TE) {                         // UART1 tx enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_7_UART1_TX; 
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x800000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_7_KBI0_P23;
                    }
    #endif
                    break;
                case KE_PORTD_BIT0:
                    if (SPI1_C1 & SPI_C1_MSTR) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_SPI1_SCK; 
                    }
    #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && ((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS2_PTD0) && !(SIM_PINSEL1 & SIM_PINSEL1_FTM2PS2_PTG4))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_FTM2_CH2;                    
                    }
    #else
                    else if ((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM2PS2)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_FTM2_CH2;                    
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x1000000) {                      // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_KBI0_P24;
                    }
    #else
                    else if (KBI1_PE & 0x01) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_KBI1_P0;
                    }
    #endif
                    break;
                case KE_PORTD_BIT1:
                    if (SPI1_C1 & SPI_C1_MSTR) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_SPI1_MOSI; 
                    }
    #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 ||  defined KINETIS_KEA128
                    else if ((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && ((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS3_PTD1) && !(SIM_PINSEL1 & SIM_PINSEL1_FTM2PS3_PTG5))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_FTM2_CH3;                    
                    }
    #else
                    else if ((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM2PS3)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_FTM2_CH3;                    
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x2000000) {                      // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_KBI0_P25;
                    }
    #else
                    else if (KBI1_PE & 0x02) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_KBI1_P1;
                    }
    #endif
                    break;
                case KE_PORTD_BIT2:
                    if (SPI1_C1 & SPI_C1_MSTR) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_2_SPI1_MISO;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x4000000) {                      // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_2_KBI0_P26;
                    }
    #else
                    else if (KBI1_PE & 0x04) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_2_KBI1_P2;
                    }
    #endif
                    break;
                case KE_PORTD_BIT3:
                    if (SPI1_C1 & SPI_C1_MSTR) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_3_SPI1_PCS0;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x8000000) {                      // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_3_KBI0_P27;
                    }
    #else
                    else if (KBI1_PE & 0x08) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_3_KBI1_P3;
                    }
    #endif
                    break;
                case KE_PORTD_BIT4:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI0_PE & 0x10000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_4_KBI0_P28;
                    }
    #else
                    if (KBI1_PE & 0x10) {                                // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_4_KBI1_P4;
                    }
    #endif
                    break;
                case KE_PORTD_BIT5:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI0_PE & 0x20000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_5_KBI0_P29;
                    }
    #else
                    if (KBI1_PE & 0x20) {                                // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_5_KBI1_P5;
                    }
    #endif
                    break;
                case KE_PORTD_BIT6:
    #if UARTS_AVAILABLE > 2
                    if ((UART2_C2 & UART_C2_RE) && (!(UART2_C1 & UART_C1_LOOPS))) { // UART2 rx enabled and not in sigle-wire/loop back mode
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_6_UART2_RX;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI0_PE & 0x40000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_6_KBI0_P30;
                        break;
                    }
    #else
                    if (KBI1_PE & 0x40) {                                // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_6_KBI1_P6;
                    }
    #endif
                    break;
                case KE_PORTD_BIT7:
    #if UARTS_AVAILABLE > 2
                    if (UART2_C2 & UART_C2_TE) {                         // UART2 tx enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_7_UART2_TX;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI0_PE & 0x80000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_7_KBI0_P31;
                        break;
                    }
    #else
                    if (KBI1_PE & 0x80) {                                // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_7_KBI1_P7;
                    }
    #endif
                    break;
                }
            }
            else if (iPort == 1) {
                switch (ulBit) {
                case KE_PORTE_BIT0:
                    if ((SPI0_C1 & SPI_C1_MSTR) && (SIM_PINSEL0 & SIM_PINSEL_SPI0PS)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_0_SPI0_SCK;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000001) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_0_KBI1_P0;
                    }
    #endif
                    break;
                case KE_PORTE_BIT1:
                    if ((SPI0_C1 & SPI_C1_MSTR) && (SIM_PINSEL0 & SIM_PINSEL_SPI0PS)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_1_SPI0_MOSI;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000002) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_1_KBI1_P1;
                    }
    #endif
                    break;
                case KE_PORTE_BIT2:
                    if (((SPI0_C1 & SPI_C1_MSTR) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_SPI0PS) != 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_2_SPI0_MISO;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000004) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_2_KBI1_P2;
                    }
    #endif
                    break;
                case KE_PORTE_BIT3:
                    if ((SPI0_C1 & SPI_C1_MSTR) && (SPI0_C1 & SPI_C1_SSOE) && (SIM_PINSEL0 & SIM_PINSEL_SPI0PS)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_3_SPI0_PCS0;
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000008) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_3_KBI1_P3;
                    }
    #endif
                    break;
                case KE_PORTE_BIT4:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000010) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_4_KBI1_P4;
                    }
    #endif
                    break;
                case KE_PORTE_BIT5:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000020) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_5_KBI1_P5;
                    }
    #endif
                    break;
                case KE_PORTE_BIT6:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000040) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_6_KBI1_P6;
                    }
    #endif
                    break;
                case KE_PORTE_BIT7:
    #if defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) && (SIM_PINSEL1 & SIM_PINSEL1_MSCANPS)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_7_CAN0_TX;   
                    }
                    else
    #endif
                    if ((FTM1_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM1PS1)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_7_FTM1_CH1;                    
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000080) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_7_KBI1_P7;
                    }
    #endif
                    break;
                case KE_PORTF_BIT0:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000100) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_0_KBI1_P8;
                    }
    #endif
                    break;
                case KE_PORTF_BIT1:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000200) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_1_KBI1_P9;
                    }
    #endif
                    break;
                case KE_PORTF_BIT2:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000400) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_2_KBI1_P10;
                    }
    #endif
                    break;
                case KE_PORTF_BIT3:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI1_PE & 0x00000800) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_3_KBI1_P11;
                    }
    #endif
                    break;
                case KE_PORTF_BIT4:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD12) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_4_ADC0_SE12;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI1_PE & 0x00001000) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_4_KBI1_P12;
                    }
    #endif
                    break;
                case KE_PORTF_BIT5:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD13) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_5_ADC0_SE13;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00002000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_5_KBI1_P13;
                    }
    #endif
                    break;
                case KE_PORTF_BIT6:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD14) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_6_ADC0_SE14;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00004000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_6_KBI1_P14;
                    }
    #endif
                    break;
                case KE_PORTF_BIT7:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD15) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_7_ADC0_SE15;
                        break;
                    }
    #endif
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00008000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_7_KBI1_P15;
                    }
    #endif
                    break;
                case KE_PORTG_BIT0:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00010000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_0_KBI1_P16;
                    }
    #endif
                    break;
                case KE_PORTG_BIT1:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00020000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_1_KBI1_P17;
                    }
    #endif
                    break;
                case KE_PORTG_BIT2:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00040000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_2_KBI1_P18;
                    }
    #endif
                    break;
                case KE_PORTG_BIT3:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00080000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_3_KBI1_P19;
                    }
    #endif
                    break;
                case KE_PORTG_BIT4:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00100000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_4_KBI1_P20;
                    }
    #endif
                    break;
                case KE_PORTG_BIT5:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00200000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_5_KBI1_P21;
                    }
    #endif
                    break;
                case KE_PORTG_BIT6:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00400000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_6_KBI1_P22;
                    }
    #endif
                    break;
                case KE_PORTG_BIT7:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00800000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_7_KBI1_P23;
                    }
    #endif
                    break;
                case KE_PORTH_BIT0:
                    if ((FTM2_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM1PS1)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_0_FTM2_CH0;                    
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x01000000) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_0_KBI1_P24;
                    }
    #endif
                    break;
                case KE_PORTH_BIT1:
                    if ((FTM2_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM1PS1)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_1_FTM2_CH1;                    
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x02000000) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_1_KBI1_P25;
                    }
    #endif
                    break;
                case KE_PORTH_BIT2:
    #if defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) && (SIM_PINSEL1 & SIM_PINSEL1_MSCANPS)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_2_CAN0_RX;   
                    }
                    else
    #endif
                    if ((FTM1_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM1PS0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_2_FTM1_CH0;                    
                    }
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x04000000) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_2_KBI1_P26;
                    }
    #endif
                    break;
                case KE_PORTH_BIT3:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x08000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_3_KBI1_P27;
                    }
    #endif
                    break;
                case KE_PORTH_BIT4:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x10000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_4_KBI1_P28;
                    }
    #endif
                    break;
                case KE_PORTH_BIT5:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x20000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_5_KBI1_P29;
                    }
    #endif
                    break;
                case KE_PORTH_BIT6:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x40000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_6_KBI1_P30;
                    }
    #endif
                    break;
                case KE_PORTH_BIT7:
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x80000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_7_KBI1_P31;
                    }
    #endif
                    break;
                }
            }
    #if defined SIM_PINSEL_IRQPS_PTI6
            else {
                switch (ulBit) {
                case KE_PORTI_BIT0:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI0) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_0_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT1:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI1) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_1_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT2:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI2) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_2_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT3:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI3) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_3_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT4:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI4) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_4_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT5:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI5) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_5_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT6:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI6) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_6_IRQ;
                        }
                    }
                    break;
                }
            }
    #endif
#else
            ucPortFunctions[iPort][iPin] = (unsigned char)((*ptrPortPin++ & PORT_MUX_MASK) >> 8);
            if (ucPortFunctions[iPort][iPin] != (PORT_MUX_GPIO >> 8)) {
                if ((ucPortFunctions[iPort][iPin] != 0) || ((ulDisabled[iPort] & ulBit) == 0)) { // not ALT 0 function (usually default) or default peripheral is not disabled
                    ulPeripherals[iPort] |= ulBit;
                }
            }
    #if defined ERRATA_ID_3402
            if ((iPort == XTAL0_PORT) && (iPin == XTAL0_PIN)) {
                if (OSC0_CR & OSC_CR_ERCLKEN) {                          // if OSC is enabled the XTAL pin is overridden by the oscillator functions
                    ucPortFunctions[iPort][iPin] = 0;
                    ulPeripherals[iPort] |= ulBit;
                }
            }
    #endif
#endif
            ulBit <<= 1;
        }
    }
    iFlagRefresh = PORT_CHANGE;
}

extern int fnSimulateDMA(int channel)                                    // {3}
{
#if !defined DEVICE_WITHOUT_DMA && !defined KINETIS_KL82
#if defined KINETIS_KL                                                   // {32}
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += channel;
    if (((ptrDMA->DMA_DCR & (DMA_DCR_START | DMA_DCR_ERQ)) != 0) && ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) != 0)) { // sw commanded start or source request (ignore is no count value remaining)
        ptrDMA->DMA_DSR_BCR |= DMA_DSR_BCR_BSY;
        while ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) != 0) {      // while bytes to be transferred
            if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_8) != 0) {              // 8 bit transfers
                *(unsigned char *)ptrDMA->DMA_DAR = *(unsigned char *)ptrDMA->DMA_SAR; // byte transfer
                ptrDMA->DMA_DSR_BCR = ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) - 1);
            }
            else if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_16) != 0) {        // 16 bit transfers
                *(unsigned short *)ptrDMA->DMA_DAR = *(unsigned short *)ptrDMA->DMA_SAR; // short word transfer
                ptrDMA->DMA_DSR_BCR = ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) - 2);
            }
            else {                                                       // 32 bit transfers
                *(unsigned long *)ptrDMA->DMA_DAR = *(unsigned long *)ptrDMA->DMA_SAR; // long word transfer
                ptrDMA->DMA_DSR_BCR = ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) - 4);
            }
            if ((ptrDMA->DMA_DCR & DMA_DCR_DINC) != 0) {                 // destination increment enabled
                if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_8) != 0) {
                    ptrDMA->DMA_DAR = (ptrDMA->DMA_DAR + 1);
                }
                else if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_16) != 0) {
                    ptrDMA->DMA_DAR = (ptrDMA->DMA_DAR + 2);
                }
                else {
                    ptrDMA->DMA_DAR = (ptrDMA->DMA_DAR + 4);
                }
            }
            if ((ptrDMA->DMA_DCR & DMA_DCR_SINC) != 0) {                 // source increment enabled
                if ((ptrDMA->DMA_DCR & DMA_DCR_SSIZE_8) != 0) {
                    ptrDMA->DMA_SAR  = (ptrDMA->DMA_SAR + 1);
                }
                else if ((ptrDMA->DMA_DCR & DMA_DCR_SSIZE_16) != 0) {
                    ptrDMA->DMA_SAR  = (ptrDMA->DMA_SAR + 2);
                }
                else {
                    ptrDMA->DMA_SAR  = (ptrDMA->DMA_SAR + 4);
                }
            }
            if ((ptrDMA->DMA_DCR & DMA_DCR_CS) != 0) {                   // if in cycle-steal mode only one transfer is performed at a time
                if (ptrDMA->DMA_DSR_BCR != 0) {
                    return 1;                                            // still active
                }
                else {
                    unsigned long ulLength = 0;
                    switch (ptrDMA->DMA_DCR & DMA_DCR_SMOD_256K) {       // hande automatic source modulo buffer operation
                    case DMA_DCR_SMOD_16:
                        ulLength = 16;
                        break;
                    case DMA_DCR_SMOD_32:
                        ulLength = 32;
                        break;
                    case DMA_DCR_SMOD_64:
                        ulLength = 64;
                        break;
                    case DMA_DCR_SMOD_128:
                        ulLength = 128;
                        break;
                    case DMA_DCR_SMOD_256:
                        ulLength = 256;
                        break;
                    case DMA_DCR_SMOD_512:
                        ulLength = 512;
                        break;
                    case DMA_DCR_SMOD_1K:
                        ulLength = 1024;
                        break;
                    case DMA_DCR_SMOD_2K:
                        ulLength = (2 * 1024);
                        break;
                    case DMA_DCR_SMOD_4K:
                        ulLength = (4 * 1024);
                        break;
                    case DMA_DCR_SMOD_8K:
                        ulLength = (8 * 1024);
                        break;
                    case DMA_DCR_SMOD_16K:
                        ulLength = (16 * 1024);
                        break;
                    case DMA_DCR_SMOD_32K:
                        ulLength = (32 * 1024);
                        break;
                    case DMA_DCR_SMOD_64K:
                        ulLength = (64 * 1024);
                        break;
                    case DMA_DCR_SMOD_128K:
                        ulLength = (128 * 1024);
                        break;
                    case DMA_DCR_SMOD_256K:
                        ulLength = (256 * 1024);
                        break;
                    }
                    ptrDMA->DMA_DSR_BCR = ulLength;
                    ptrDMA->DMA_SAR -= ulLength;
                    switch (ptrDMA->DMA_DCR & DMA_DCR_DMOD_256K) {       // hande automatic destination modulo buffer operation
                    case DMA_DCR_DMOD_16:
                        ulLength = 16;
                        break;
                    case DMA_DCR_DMOD_32:
                        ulLength = 32;
                        break;
                    case DMA_DCR_DMOD_64:
                        ulLength = 64;
                        break;
                    case DMA_DCR_DMOD_128:
                        ulLength = 128;
                        break;
                    case DMA_DCR_DMOD_256:
                        ulLength = 256;
                        break;
                    case DMA_DCR_DMOD_512:
                        ulLength = 512;
                        break;
                    case DMA_DCR_DMOD_1K:
                        ulLength = 1024;
                        break;
                    case DMA_DCR_DMOD_2K:
                        ulLength = (2 * 1024);
                        break;
                    case DMA_DCR_DMOD_4K:
                        ulLength = (4 * 1024);
                        break;
                    case DMA_DCR_DMOD_8K:
                        ulLength = (8 * 1024);
                        break;
                    case DMA_DCR_DMOD_16K:
                        ulLength = (16 * 1024);
                        break;
                    case DMA_DCR_DMOD_32K:
                        ulLength = (32 * 1024);
                        break;
                    case DMA_DCR_DMOD_64K:
                        ulLength = (64 * 1024);
                        break;
                    case DMA_DCR_DMOD_128K:
                        ulLength = (128 * 1024);
                        break;
                    case DMA_DCR_DMOD_256K:
                        ulLength = (256 * 1024);
                        break;
                    default:
                        ulLength = 0;
                        break;
                    }
                    ptrDMA->DMA_DAR -= ulLength;
                    ptrDMA->DMA_DSR_BCR |= ulLength;
                    break;
                }
            }
        }
        ptrDMA->DMA_DSR_BCR |= DMA_DSR_BCR_DONE;
        if ((ptrDMA->DMA_DCR & DMA_DCR_EINT) != 0) {                     // if interrupt is enabled
            if (fnGenInt(irq_DMA0_ID + channel) != 0) {                  // if DMA channel interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                switch (channel) {
                case 0:
                    ptrVect->processor_interrupts.irq_DMA0();            // call the interrupt handler for DMA channel 0
                    break;
                case 1:
                    ptrVect->processor_interrupts.irq_DMA1();            // call the interrupt handler for DMA channel 1
                    break;
                case 2:
                    ptrVect->processor_interrupts.irq_DMA2();            // call the interrupt handler for DMA channel 2
                    break;
                case 3:
                    ptrVect->processor_interrupts.irq_DMA3();            // call the interrupt handler for DMA channel 3
                    break;
                }
            }
        }
        return 0;
    }
#elif !defined KINETIS_KE
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    ptrDMA_TCD += channel;

    #if defined irq_DMA4_ID
    if (channel > 15) {
        _EXCEPTION("Warning - the simulator doesn't simulate DMA channels above 15 at the moment!!");
    }
    #else
    if (channel > 3) {
        _EXCEPTION("Warning - invalid DMA channel being used!!");
    }
    #endif

    if ((ptrDMA_TCD->DMA_TCD_CSR & (DMA_TCD_CSR_START | DMA_TCD_CSR_ACTIVE)) != 0) { // sw commanded start or active
        int interrupt = 0;
        ptrDMA_TCD->DMA_TCD_CSR |= DMA_TCD_CSR_ACTIVE;                   // mark active 
        ptrDMA_TCD->DMA_TCD_CSR &= ~DMA_TCD_CSR_START;                   // clear the start bit
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK != 0) {                      // main loop iterations
            unsigned long ulMinorLoop = ptrDMA_TCD->DMA_TCD_NBYTES_ML;   // the number of bytes to transfer
            if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_DSIZE_32) != 0) { // {36} handle long word transfers
                if ((ptrDMA_TCD->DMA_TCD_DOFF & 0x3) != 0) {
                    _EXCEPTION("DMA destination offset error!!");
                }
                if ((ptrDMA_TCD->DMA_TCD_SOFF & 0x3) != 0) {
                    _EXCEPTION("DMA source offset error!!");
                }
                if ((ulMinorLoop & 0x3) != 0) {
                    _EXCEPTION("DMA copy size error!!");
                }
                while (ulMinorLoop != 0) {
                    *(unsigned long *)ptrDMA_TCD->DMA_TCD_DADDR = *(unsigned long *)ptrDMA_TCD->DMA_TCD_SADDR; // long word transfer
                    ptrDMA_TCD->DMA_TCD_DADDR = ptrDMA_TCD->DMA_TCD_DADDR + ptrDMA_TCD->DMA_TCD_DOFF;
                    ptrDMA_TCD->DMA_TCD_SADDR = ptrDMA_TCD->DMA_TCD_SADDR + ptrDMA_TCD->DMA_TCD_SOFF;
                    ulMinorLoop -= sizeof(unsigned long);
                }
            }
            else if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_DSIZE_16) != 0) { // {36} handle short word transfers
                if ((ptrDMA_TCD->DMA_TCD_DOFF & 0x1) != 0) {
                    _EXCEPTION("DMA destination offset error!!");
                }
                if ((ptrDMA_TCD->DMA_TCD_SOFF & 0x1) != 0) {
                    _EXCEPTION("DMA source offset error!!");
                }
                if ((ulMinorLoop & 0x1) != 0) {
                    _EXCEPTION("DMA copy size error!!");
                }
                while (ulMinorLoop != 0) {
                    *(unsigned short *)ptrDMA_TCD->DMA_TCD_DADDR = *(unsigned short *)ptrDMA_TCD->DMA_TCD_SADDR; // short word transfer
                    ptrDMA_TCD->DMA_TCD_DADDR = (ptrDMA_TCD->DMA_TCD_DADDR + ptrDMA_TCD->DMA_TCD_DOFF);
                    ptrDMA_TCD->DMA_TCD_SADDR = (ptrDMA_TCD->DMA_TCD_SADDR + ptrDMA_TCD->DMA_TCD_SOFF);
                    if (ulMinorLoop <= sizeof(unsigned short)) {
                        ulMinorLoop = 0;
                    }
                    else {
                        ulMinorLoop -= sizeof(unsigned short);
                    }
                }
            }
            else {
                while ((ulMinorLoop--) != 0) {
                    *(unsigned char *)ptrDMA_TCD->DMA_TCD_DADDR = *(unsigned char *)ptrDMA_TCD->DMA_TCD_SADDR; // byte transfer
                    ptrDMA_TCD->DMA_TCD_DADDR = ptrDMA_TCD->DMA_TCD_DADDR + ptrDMA_TCD->DMA_TCD_DOFF;
                    ptrDMA_TCD->DMA_TCD_SADDR = ptrDMA_TCD->DMA_TCD_SADDR + ptrDMA_TCD->DMA_TCD_SOFF;
                }
            }
            (ptrDMA_TCD->DMA_TCD_CITER_ELINK)--;
            if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                  // major loop completed
                if ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_INTMAJOR) != 0) { // {18}
                    interrupt = 1;                                       // possible interrupt
                }
                if ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_DREQ) != 0) { // disable on completion of major loop
                    DMA_ERQ &= ~(DMA_ERQ_ERQ0 << channel);
                    ptrDMA_TCD->DMA_TCD_CSR &= ~DMA_TCD_CSR_ACTIVE;      // completed
                }
                else {
                    ptrDMA_TCD->DMA_TCD_CITER_ELINK = ptrDMA_TCD->DMA_TCD_BITER_ELINK;
                }
                ptrDMA_TCD->DMA_TCD_CSR |= DMA_TCD_CSR_DONE;
                ptrDMA_TCD->DMA_TCD_DADDR += ptrDMA_TCD->DMA_TCD_DLASTSGA;
                ptrDMA_TCD->DMA_TCD_SADDR += ptrDMA_TCD->DMA_TCD_SLAST; // ???
            }
            else if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == (ptrDMA_TCD->DMA_TCD_BITER_ELINK/2)) { // half complete
                if ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_INTHALF) != 0) { // check whether half-buffer interrupt has been configured
                    interrupt = 1;
                }
            }

            if (interrupt != 0) {                                        // if possible interrupt to generate
                DMA_INT |= (DMA_INT_INT0 << channel);
                if (fnGenInt(irq_DMA0_ID + channel) != 0) {              // if DMA channel interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    switch (channel) {
                    case 0:
                        ptrVect->processor_interrupts.irq_DMA0();        // call the interrupt handler for DMA channel 0
                        break;
                    case 1:
                        ptrVect->processor_interrupts.irq_DMA1();        // call the interrupt handler for DMA channel 1
                        break;
                    case 2:
                        ptrVect->processor_interrupts.irq_DMA2();        // call the interrupt handler for DMA channel 2
                        break;
                    case 3:
                        ptrVect->processor_interrupts.irq_DMA3();        // call the interrupt handler for DMA channel 3
                        break;
    #if defined irq_DMA4_ID
                    case 4:
                        ptrVect->processor_interrupts.irq_DMA4();        // call the interrupt handler for DMA channel 4
                        break;
                    case 5:
                        ptrVect->processor_interrupts.irq_DMA5();        // call the interrupt handler for DMA channel 5
                        break;
                    case 6:
                        ptrVect->processor_interrupts.irq_DMA6();        // call the interrupt handler for DMA channel 6
                        break;
                    case 7:
                        ptrVect->processor_interrupts.irq_DMA7();        // call the interrupt handler for DMA channel 7
                        break;
                    case 8:
                        ptrVect->processor_interrupts.irq_DMA8();        // call the interrupt handler for DMA channel 8
                        break;
                    case 9:
                        ptrVect->processor_interrupts.irq_DMA9();        // call the interrupt handler for DMA channel 9
                        break;
                    case 10:
                        ptrVect->processor_interrupts.irq_DMA10();       // call the interrupt handler for DMA channel 10
                        break;
                    case 11:
                        ptrVect->processor_interrupts.irq_DMA11();       // call the interrupt handler for DMA channel 11
                        break;
                    case 12:
                        ptrVect->processor_interrupts.irq_DMA12();       // call the interrupt handler for DMA channel 12
                        break;
                    case 13:
                        ptrVect->processor_interrupts.irq_DMA13();       // call the interrupt handler for DMA channel 13
                        break;
                    case 14:
                        ptrVect->processor_interrupts.irq_DMA14();       // call the interrupt handler for DMA channel 14
                        break;
                    case 15:
                        ptrVect->processor_interrupts.irq_DMA15();       // call the interrupt handler for DMA channel 15
                        break;
    #endif
                    }
                }
            }
            if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {
                return 0;                                                // completed
            }
        }
        return 1;                                                        // not completed
    }
#endif
#endif
    return -1;                                                           // no operation
}

static void fnHandleDMA_triggers(int iTriggerSource, int iDMAmux)
{
#if !defined DEVICE_WITHOUT_DMA
    #if defined KINETIS_KL
    #define MAX_DMA_MUX_CHANNELS 4
    #else
    #define MAX_DMA_MUX_CHANNELS 16
    #endif
    unsigned char *ptrMux = DMAMUX0_CHCFG_ADD;                       // check the channel multiplexers to see whether it connects to the port
    int iChannel = 0;
    iTriggerSource &= ~(DMAMUX_CHCFG_TRIG);
    if (iDMAmux == 1) {
        #if defined DMAMUX1_CHCFG_ADD
        ptrMux = DMAMUX0_CHCFG_ADD;
        #else
        return;
        #endif
    }
    while (iChannel < MAX_DMA_MUX_CHANNELS) {
        if ((*ptrMux++ & ~(DMAMUX_CHCFG_TRIG)) == (DMAMUX_CHCFG_ENBL | (unsigned char)iTriggerSource)) { // matching enabled trigger
                #if defined _WINDOWS
            if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT0 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 0) {
                    _EXCEPTION("PIT0 trigger only operates on DMA channel 0!!");
                }
            }
            else if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT1 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 1) {
                    _EXCEPTION("PIT1 trigger only operates on DMA channel 1!!");
                }
            }
            else if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT2 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 2) {
                    _EXCEPTION("PIT2 trigger only operates on DMA channel 2!!");
                }
            }
            else if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT3 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 3) {
                    _EXCEPTION("PIT3 trigger only operates on DMA channel 3!!");
                }
            }
    #endif
    #if !defined KINETIS_KL
            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << iChannel)) != 0) {           // if the DMA channel is enabled
                KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                ptrDMA_TCD += iChannel;
                ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE);             // trigger
                fnSimulateDMA(iChannel);
            }
    #else
            fnSimulateDMA(iChannel);
    #endif
        }
        iChannel++;
    }
#endif
}

// Handle port interrupts on input changes
//
static void fnPortInterrupt(int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig)
{
#if defined KINETIS_KE
    if ((SIM_SCGC_IRQ & SIM_SCGC) != 0) {                                // if the IRQ module is enabled
        fnHandleIRQ(iPort, ulNewState, ulChangedBit, ptrPortConfig);
    }
    #if KBIS_AVAILABLE > 0
    if ((SIM_SCGC_KBI0 & SIM_SCGC) != 0) {                               // if the KBI0 module is enabled
        fnHandleKBI(0, iPort, ulNewState, ulChangedBit, ptrPortConfig);
    }
    #endif
    #if KBIS_AVAILABLE > 1
    if ((SIM_SCGC_KBI1 & SIM_SCGC) != 0) {                               // if the KBI1 module is enabled
        fnHandleKBI(1, iPort, ulNewState, ulChangedBit, ptrPortConfig);
    }
    #endif
#else
    int iPortInterruptSource;
    if ((*ptrPortConfig & PORT_IRQC_INT_MASK) == 0) {                    // no interrupt/DMA function programmed
        return;
    }
    switch (*ptrPortConfig & PORT_IRQC_INT_MASK)  {                      // interrupt/DMA property of the pin
    case PORT_IRQC_BOTH:
    case PORT_IRQC_DMA_BOTH:
        break;
    case PORT_IRQC_LOW_LEVEL:
    case PORT_IRQC_FALLING:
    case PORT_IRQC_DMA_FALLING:
        if ((ulNewState & ulChangedBit) != 0) {
            return;                                                      // not correct level
        }
        break;
    case PORT_IRQC_HIGH_LEVEL:
    case PORT_IRQC_RISING:
    case PORT_IRQC_DMA_RISING:
        if ((ulNewState & ulChangedBit) == 0) {
            return;                                                      // not correct level
        }
        break;
    }
    if ((*ptrPortConfig & PORT_IRQC_LOW_LEVEL) == 0) {                   // DMA trigger and not interrupt
    #if defined DMAMUX1_CHCFG_SOURCE_PORTF
        if (iPort == _PORTF) {
            fnHandleDMA_triggers(DMAMUX1_CHCFG_SOURCE_PORTF, 1);         // port F is on second DMAMUX
        }
        else {
    #endif
            fnHandleDMA_triggers((DMAMUX0_CHCFG_SOURCE_PORTA + iPort), 0);
    #if defined DMAMUX1_CHCFG_SOURCE_PORTF
        }
    #endif
        return;
    }
    *ptrPortConfig |= PORT_ISF;
    iPortInterruptSource = (irq_PORTA_ID + iPort);
    switch (iPort) {
    case _PORTA:
        PORTA_ISFR |= ulChangedBit;         
        break;
    #if PORTS_AVAILABLE > 1
    case _PORTB:
        PORTB_ISFR |= ulChangedBit;
        #if defined irq_PORTBCD_E_ID                                     // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 2
    case _PORTC:
        PORTC_ISFR |= ulChangedBit;
        #if defined irq_PORTC_D_ID                                       // shared port C/D interrupt vector
        iPortInterruptSource = irq_PORTC_D_ID;
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 3
    case _PORTD:
        PORTD_ISFR |= ulChangedBit;
        #if defined irq_PORTC_D_ID                                       // shared port C/D interrupt vector
        iPortInterruptSource = irq_PORTC_D_ID;
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 4
    case _PORTE:
        PORTE_ISFR |= ulChangedBit;
        #if defined irq_PORTBCD_E_ID                                     // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 5
    case _PORTF:
        PORTF_ISFR |= ulChangedBit;
        break;
    #endif
    }
    if (fnGenInt(iPortInterruptSource) != 0) {                           // if port interrupt is not disabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        switch (iPort) {
        case _PORTA:
            ptrVect->processor_interrupts.irq_PORTA();                   // call port interrupt handler
            break;
        case _PORTB:
    #if defined irq_PORTBCD_E_ID                                         // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
    #elif defined irq_PORTB_ID
            ptrVect->processor_interrupts.irq_PORTB();                   // call port interrupt handler
    #endif
            break;
    #if PORTS_AVAILABLE > 2
        case _PORTC:
        #if defined irq_PORTC_ID
            ptrVect->processor_interrupts.irq_PORTC();                   // call port interrupt handler
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
        #elif defined irq_PORTC_D_ID                                     // shared port C/D interrupt vector
            ptrVect->processor_interrupts.irq_PORTC_D();                 // call port interrupt handler
        #endif
            break;
    #endif
    #if PORTS_AVAILABLE > 3
        case _PORTD:
        #if defined irq_PORTC_ID
            ptrVect->processor_interrupts.irq_PORTD();                   // call port interrupt handler
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
        #elif defined irq_PORTC_D_ID                                     // shared port C/D interrupt vector
            ptrVect->processor_interrupts.irq_PORTC_D();                 // call port interrupt handler
        #endif
            break;
    #endif
    #if PORTS_AVAILABLE > 4
        case _PORTE:
        #if defined irq_PORTE_ID
            ptrVect->processor_interrupts.irq_PORTE();                   // call port interrupt handler
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
        #endif
            break;
    #endif
    #if PORTS_AVAILABLE > 5
        case _PORTF:
    #if defined irq_PORTF_ID
            ptrVect->processor_interrupts.irq_PORTF();                   // call port interrupt handler
    #endif
            break;
    #endif
        }
    } 
#endif
}

#if defined I2C_INTERFACE

static void fnInterruptI2C(int irq_I2C_ID)
{
    if (fnGenInt(irq_I2C_ID) != 0) {                                     // if I2C interrupt is not disabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        switch (irq_I2C_ID) {
        case irq_I2C0_ID:
            ptrVect->processor_interrupts.irq_I2C0();                    // call the interrupt handler
            break;
    #if I2C_AVAILABLE > 1
        case irq_I2C1_ID:
            ptrVect->processor_interrupts.irq_I2C1();                    // call the interrupt handler
            break;
    #endif
    #if I2C_AVAILABLE > 2
        case irq_I2C2_ID:
            ptrVect->processor_interrupts.irq_I2C2();                    // call the interrupt handler
            break;
    #endif
    #if I2C_AVAILABLE > 3
        case irq_I2C3_ID:
            ptrVect->processor_interrupts.irq_I2C3();                    // call the interrupt handler
            break;
    #endif
        }
    }
}

extern void fnSimulateI2C(int iPort, unsigned char *ptrDebugIn, unsigned short usLen, int iRepeatedStart)
{
    KINETIS_I2C_CONTROL *ptrI2C = 0;
    int iI2C_irq = 0;
    switch (iPort) {
    case 0:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C0_BLOCK;
        iI2C_irq = irq_I2C0_ID;
        break;
    #if I2C_AVAILABLE > 1
    case 1:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C1_BLOCK;
        iI2C_irq = irq_I2C1_ID;
        break;
    #endif
    #if I2C_AVAILABLE > 2
    case 2:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C2_BLOCK;
        iI2C_irq = irq_I2C2_ID;
        break;
    #endif
    #if I2C_AVAILABLE > 3
    case 3:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C3_BLOCK;
        iI2C_irq = irq_I2C3_ID;
        break;
    #endif
    default:
        _EXCEPTION("Injecting to invalid I2C channel!");
        break;
    }
    ptrI2C->I2C_S = (I2C_IBB);                                           // bus is busy
    #if defined I2C_START_CONDITION_INTERRUPT || defined DOUBLE_BUFFERED_I2C
    ptrI2C->I2C_FLT |= (I2C_FLT_FLT_STARTF);                             // start condition detect flag
    if ((ptrI2C->I2C_FLT & I2C_FLT_FLT_SSIE) != 0) {                     // if the start/stop condition interrupt is enabled
        ptrI2C->I2C_S |= I2C_IIF;                                        // interrupt flag set
        fnInterruptI2C(iI2C_irq);                                        // generate the I2C interrupt
    }
    #endif
    if ((*ptrDebugIn & 0xfe) == (ptrI2C->I2C_A1 & 0xfe)) {               // if the address matches the slave address
        ptrI2C->I2C_S = (I2C_IBB | I2C_IAAS | I2C_TCF);                  // bus is busy, addressed as slave and transfer is complete
        ptrI2C->I2C_D = *ptrDebugIn++;                                   // the address is put to the data register
        if ((ptrI2C->I2C_D & 0x01) != 0) {                               // addressed for read
            ptrI2C->I2C_S |= I2C_SRW;
            fnInterruptI2C(iI2C_irq);                                    // generate the I2C interrupt
            usLen = *ptrDebugIn;                                         // the number of bytes to be read
            while (usLen-- > 0) {
                if (usLen == 0) {
                    ptrI2C->I2C_S |= I2C_RXACK;                          // the master doesn't acknowledge the final byte
                }
                else {
                    ptrI2C->I2C_S &= ~I2C_RXACK;
                }
                fnInterruptI2C(iI2C_irq);                                // generate the I2C interrupt
            }
            ptrI2C->I2C_S = I2C_SRW;                                     // slave transmit
        }
        else {
            fnInterruptI2C(iI2C_irq);                                    // generate the I2C interrupt
            while (usLen-- > 1) {
                ptrI2C->I2C_D = *ptrDebugIn++;                           // next byte is put to the data register
                fnInterruptI2C(iI2C_irq);                                // generate the I2C interrupt
            }
        }
    }
    else {
        ptrI2C->I2C_S = (I2C_IBB | I2C_TCF);                             // bus is busy and transfer is complete
    }
    ptrI2C->I2C_S = 0;
    #if defined DOUBLE_BUFFERED_I2C
    ptrI2C->I2C_S = (I2C_TCF);                                           // transfer completed flag is set automatically in double-buffered mode
    #endif
    if (iRepeatedStart == 0) {
        ptrI2C->I2C_FLT |= (I2C_FLT_FLT_STOPF);                          // stop condition detect flag
        if ((ptrI2C->I2C_FLT & I2C_FLT_FLT_INT) != 0) {                  // if the start/stop condition interrupt is enabled
            ptrI2C->I2C_S |= I2C_IIF;                                    // interrupt flag set
            fnInterruptI2C(iI2C_irq);                                    // generate the I2C interrupt
        }
    }
}
#endif

#if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0                         // device contains both UART and LPUART
#define UART_TYPE_LPUART 0
#define UART_TYPE_UART   1
static const unsigned char uart_type[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = {
        #if defined LPUARTS_PARALLEL                                     // K22
    UART_TYPE_UART,                                                      // UART0
    UART_TYPE_UART,                                                      // UART1
    UART_TYPE_UART,                                                      // UART2
    UART_TYPE_LPUART,                                                    // LPUART0 (numbered 3)
        #else                                                            // KL43
    UART_TYPE_LPUART,                                                    // LPUART0
    UART_TYPE_LPUART,                                                    // LPUART1
    UART_TYPE_UART,                                                      // UART2
        #endif
};
#endif

// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
#if defined SERIAL_INTERFACE
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (iPort >= NUMBER_SERIAL) {
        extern int fnRxExtSCI(int iChannel, unsigned char *ptrData, unsigned short usLength);
        int iHandled;
        while ((iHandled = fnRxExtSCI((iPort - NUMBER_SERIAL), ptrDebugIn, usLen)) > 0) { // handle reception of each byte
            switch (iPort - NUMBER_SERIAL) {                             // generate an interrupt on the corresponding port line (assumed negative)
            case 0:
            case 1:
                fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                break;
        #if NUMBER_EXTERNAL_SERIAL > 2
            case 2:
            case 3:
                fnSimulateInputChange(EXT_UART_2_3_INT_PORT, fnMapPortBit(EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters              
                break;
        #endif
            }
            ptrDebugIn += iHandled;
            usLen -= iHandled;
        }
        return;
    }
    #endif

    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[iPort] == UART_TYPE_LPUART) {
        #endif
        switch (iPort) {
        #if defined LPUARTS_PARALLEL
        case UARTS_AVAILABLE:
        #else
        case 0:
        #endif
            if ((LPUART0_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                while ((usLen--) != 0) {                                 // for each reception character
                    LPUART0_DATA = *ptrDebugIn++;
                    LPUART0_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART0_CTRL & LPUART_CTRL_RIE) != 0) {
                                                                         // if reception interrupt is enabled
        #if !defined KINETIS_KE && !defined KINETIS_KL03 && !defined KINETIS_KL43 // these don't support DMA
                        if ((LPUART0_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the LPUART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_LPUART0_RX_CHANNEL
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_LPUART0_RX_CHANNEL)) != 0) { // if source enabled
                                KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                                ptrDMA_TCD += DMA_LPUART0_RX_CHANNEL;
                                ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                                fnSimulateDMA(DMA_LPUART0_RX_CHANNEL);   // trigger DMA transfer on the UART's channel
                                LPUART0_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
            #endif
                        }
                        else {
        #endif
                            if (fnGenInt(irq_LPUART0_ID) != 0) {         // if LPUART0 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART0(); // call the interrupt handler
                            }
        #if !defined KINETIS_KE && !defined KINETIS_KL03 & !defined KINETIS_KL43
                        }
        #endif
                    }
                }
            }
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):
            #else
        case 1:
            #endif
            if ((LPUART1_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                while ((usLen--) != 0) {                                 // for each reception character
                    LPUART1_DATA = *ptrDebugIn++;
                    LPUART1_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART1_CTRL & LPUART_CTRL_RIE) != 0) {
                                                                         // if reception interrupt is enabled
            #if !defined KINETIS_KE && !defined KINETIS_KL03 && !defined KINETIS_KL43 // these don't support DMA
                        if ((LPUART1_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the UART is operating in DMA reception mode
                #if defined SERIAL_SUPPORT_DMA && defined DMA_LPUART1_RX_CHANNEL
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_LPUART1_RX_CHANNEL)) != 0) { // if source enabled
                                KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                                ptrDMA_TCD += DMA_LPUART1_RX_CHANNEL;
                                ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                                fnSimulateDMA(DMA_LPUART1_RX_CHANNEL);   // trigger DMA transfer on the UART's channel
                                LPUART1_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #endif
                        }
                        else {
            #endif
                            if (fnGenInt(irq_LPUART1_ID) != 0) {             // if LPUART1 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART1(); // call the interrupt handler
                            }
                #if !defined KINETIS_KE && !defined KINETIS_KL03 & !defined KINETIS_KL43
                        }
                #endif
                    }
                }
            }
            break;
        #endif
        #if LPUARTS_AVAILABLE > 2
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 2):
            #else
        case 2:
            #endif
            if ((LPUART2_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                while ((usLen--) != 0) {                                 // for each reception character
                    LPUART2_DATA = *ptrDebugIn++;
                    LPUART2_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART2_CTRL & LPUART_CTRL_RIE) != 0) {
                                                                         // if reception interrupt is enabled
                        if ((LPUART2_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_LPUART2_RX_CHANNEL
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_LPUART2_RX_CHANNEL)) != 0) { // if source enabled
                                KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                                ptrDMA_TCD += DMA_LPUART2_RX_CHANNEL;
                                ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                                fnSimulateDMA(DMA_LPUART2_RX_CHANNEL);   // trigger DMA transfer on the UART's channel
                                LPUART2_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
            #endif
                        }
                        else {
                            if (fnGenInt(irq_LPUART2_ID) != 0) {             // if LPUART2 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART2(); // call the interrupt handler
                            }
                        }
                    }
                }
            }
            break;
        #endif
        #if LPUARTS_AVAILABLE > 3
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 3):
            #else
        case 3:
            #endif
            if ((LPUART3_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                while ((usLen--) != 0) {                                 // for each reception character
                    LPUART3_DATA = *ptrDebugIn++;
                    LPUART3_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART3_CTRL & LPUART_CTRL_RIE) != 0) {
                                                                         // if reception interrupt is enabled
                        if ((LPUART3_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_LPUART3_RX_CHANNEL
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_LPUART3_RX_CHANNEL)) != 0) { // if source enabled
                                KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                                ptrDMA_TCD += DMA_LPUART3_RX_CHANNEL;
                                ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                                fnSimulateDMA(DMA_LPUART3_RX_CHANNEL);   // trigger DMA transfer on the UART's channel
                                LPUART3_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
            #endif
                        }
                        else {
                            if (fnGenInt(irq_LPUART3_ID) != 0) {             // if LPUART3 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART3(); // call the interrupt handler
                            }
                        }
                    }
                }
            }
            break;
        #endif
        #if LPUARTS_AVAILABLE > 4
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 4):
            #else
        case 4:
            #endif
            if ((LPUART4_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                while ((usLen--) != 0) {                                 // for each reception character
                    LPUART4_DATA = *ptrDebugIn++;
                    LPUART4_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART4_CTRL & LPUART_CTRL_RIE) != 0) {
                                                                         // if reception interrupt is enabled
                        if ((LPUART4_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_LPUART3_RX_CHANNEL
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_LPUART4_RX_CHANNEL)) != 0) { // if source enabled
                                KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                                ptrDMA_TCD += DMA_LPUART4_RX_CHANNEL;
                                ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                                fnSimulateDMA(DMA_LPUART4_RX_CHANNEL);   // trigger DMA transfer on the UART's channel
                                LPUART4_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
            #endif
                        }
                        else {
                            if (fnGenInt(irq_LPUART4_ID) != 0) {             // if LPUART4 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART4(); // call the interrupt handler
                            }
                        }
                    }
                }
            }
            break;
        #endif
        }
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif

    #if UARTS_AVAILABLE > 0
    switch (iPort) {
        #if LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL
    case 0:
        if ((UART0_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while ((usLen--) != 0) {                                     // for each reception character
                UART0_D = *ptrDebugIn++;
                UART0_S1 |= UART_S1_RDRF;                                // set interrupt cause
                if ((UART0_C2 & UART_C2_RIE) != 0) {                     // if reception interrupt is enabled
            #if !defined KINETIS_KE && !defined KINETIS_KL03             // these don't support DMA
                    if ((UART0_C5 & UART_C5_RDMAS) != 0) {               // {4} if the UART is operating in DMA reception mode
                #if defined SERIAL_SUPPORT_DMA && defined DMA_UART0_RX_CHANNEL
                        if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART0_RX_CHANNEL)) != 0) { // if source enabled
                            KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                            ptrDMA_TCD += DMA_UART0_RX_CHANNEL;
                            ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                            fnSimulateDMA(DMA_UART0_RX_CHANNEL);         // trigger DMA transfer on the UART's channel
                            UART0_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
                #endif
                    }
                    else {
            #endif
                        if (fnGenInt(irq_UART0_ID) != 0) {               // if UART0 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART0();   // call the interrupt handler
                        }
            #if !defined KINETIS_KE && !defined KINETIS_KL03 & !defined KINETIS_KL43
                    }
            #endif
                }
            }
        }
        break;
        #endif
    #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
    case 1:
        if ((UART1_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while ((usLen--) != 0) {                                     // for each reception character
                UART1_D = *ptrDebugIn++;                                 // save the received byte to the UART data register
                UART1_S1 |= UART_S1_RDRF;                                // set interrupt cause
                if ((UART1_C2 & UART_C2_RIE) != 0) {                     // if reception interrupt (or DMA) is enabled
        #if !defined KINETIS_KE
            #if defined KINETIS_KL
                    if ((UART1_C4 & UART_C4_RDMAS) != 0)                 // DMA mode
            #else
                    if ((UART1_C5 & UART_C5_RDMAS) != 0)                 // DMA mode
            #endif
                    {                                                    // {4} if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_UART1_RX_CHANNEL
                        if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART1_RX_CHANNEL)) != 0) { // if source enabled
                            KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                            ptrDMA_TCD += DMA_UART1_RX_CHANNEL;
                            ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                            fnSimulateDMA(DMA_UART1_RX_CHANNEL);         // trigger DMA transfer on the UART's channel
                            UART1_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
            #endif
                    }
                    else {
        #endif
                        if (fnGenInt(irq_UART1_ID) != 0) {               // if UART1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART1();   // call the interrupt handler
                        }
        #if !defined KINETIS_KE
                    }
        #endif
                }
            }
        }
        break;
    #endif
    #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || ((UARTS_AVAILABLE == 1) && (LPUARTS_AVAILABLE == 2))
    case 2:
        if ((UART2_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen--) {                                            // for each reception character
                UART2_D = *ptrDebugIn++;
                UART2_S1 |= UART_S1_RDRF;                                // set interrupt cause
                if (UART2_C2 & UART_C2_RIE) {                            // if reception interrupt is enabled
        #if !defined KINETIS_KE
            #if defined KINETIS_KL
                    if (UART2_C4 & UART_C4_TDMAS)
            #else
                    if (UART2_C5 & UART_C5_RDMAS)
            #endif
                    {                                                    // {4} if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_UART2_RX_CHANNEL
                        if (DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART2_RX_CHANNEL)) { // if source enabled
                            KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                            ptrDMA_TCD += DMA_UART2_RX_CHANNEL;
                            ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                            fnSimulateDMA(DMA_UART2_RX_CHANNEL);         // trigger DMA transfer on the UART's channel
                            UART2_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
            #endif
                    }
                    else {
        #endif
                        if (fnGenInt(irq_UART2_ID) != 0) {               // if UART2 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART2();   // call the interrupt handler
                        }
        #if !defined KINETIS_KE
                    }
        #endif
                }
            }
        }
        break;
    #endif
    #if UARTS_AVAILABLE > 3
    case 3:
        if ((UART3_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen--) {                                            // for each reception character
                UART3_D = *ptrDebugIn++;
                UART3_S1 |= UART_S1_RDRF;                                // set interrupt cause
                if (UART3_C2 & UART_C2_RIE) {                            // if reception interrupt is enabled
                    if (UART3_C5 & UART_C5_RDMAS) {                      // {4} if the UART is operating in DMA reception mode
        #if defined SERIAL_SUPPORT_DMA
                        if (DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART3_RX_CHANNEL)) { // if source enabled
                            KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                            ptrDMA_TCD += DMA_UART3_RX_CHANNEL;
                            ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                            fnSimulateDMA(DMA_UART3_RX_CHANNEL);         // trigger DMA transfer on the UART's channel
                            UART3_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
        #endif
                    }
                    else {
                        if (fnGenInt(irq_UART3_ID) != 0) {               // if UART3 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART3();   // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    #if UARTS_AVAILABLE > 4
    case 4:
        if ((UART4_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen--) {                                            // for each reception character
                UART4_D = *ptrDebugIn++;
                UART4_S1 |= UART_S1_RDRF;                                // set interrupt cause
                if (UART4_C2 & UART_C2_RIE) {                            // if reception interrupt is enabled
                    if (UART4_C5 & UART_C5_RDMAS) {                      // {4} if the UART is operating in DMA reception mode
        #if defined SERIAL_SUPPORT_DMA
                        if (DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART4_RX_CHANNEL)) { // if source enabled
                            KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                            ptrDMA_TCD += DMA_UART4_RX_CHANNEL;
                            ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                            fnSimulateDMA(DMA_UART4_RX_CHANNEL);         // trigger DMA transfer on the UART's channel
                            UART4_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
        #endif
                    }
                    else {
                        if (fnGenInt(irq_UART4_ID) != 0) {               // if UART4 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART4();   // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    #if UARTS_AVAILABLE > 5
    case 5:
        if ((UART5_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen--) {                                            // for each reception character
                UART5_D = *ptrDebugIn++;
                UART5_S1 |= UART_S1_RDRF;                                // set interrupt cause
                if (UART5_C2 & UART_C2_RIE) {                            // if reception interrupt is enabled
                    if (UART5_C5 & UART_C5_RDMAS) {                      // {4} if the UART is operating in DMA reception mode
        #if defined SERIAL_SUPPORT_DMA
                        if (DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART5_RX_CHANNEL)) { // if source enabled
                            KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                            ptrDMA_TCD += DMA_UART5_RX_CHANNEL;
                            ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE); // trigger
                            fnSimulateDMA(DMA_UART5_RX_CHANNEL);         // trigger DMA transfer on the UART's channel
                            UART5_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
        #endif
                    }
                    else {
                        if (fnGenInt(irq_UART5_ID) != 0) {               // if UART5 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART5();   // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    }
    #endif
#endif
}


// Handle UART transmission interrupts
//
static void fnUART_Tx_int(int iChannel)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[iChannel] == UART_TYPE_LPUART) {
        #endif
        switch (iChannel) {
        #if defined LPUARTS_PARALLEL
        case UARTS_AVAILABLE:
        #else
        case 0:                                                              // LPUART0
        #endif
            if ((LPUART0_CTRL & LPUART_CTRL_TE) != 0) {                      // if transmitter enabled
                LPUART0_STAT |= (LPUART_STAT_TDRE | LPUART_STAT_TC);         // set interrupt cause
                if ((LPUART0_CTRL & LPUART0_STAT) != 0) {                    // if transmit interrupt type enabled
                    if (fnGenInt(irq_LPUART0_ID) != 0) {                     // if LPUART0 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART0();         // call the interrupt handler
                    }
                }
            }
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):
            #else
        case 1:
            #endif
            if (LPUART1_CTRL & LPUART_CTRL_TE) {                             // if transmitter enabled
                LPUART1_STAT |= (LPUART_STAT_TDRE | LPUART_STAT_TC);         // set interrupt cause
                if (LPUART1_CTRL & LPUART1_STAT) {                           // if transmit interrupt type enabled
                    if (fnGenInt(irq_LPUART1_ID) != 0) {                     // if LPUART1 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART1();         // call the interrupt handler
                    }
                }
            }
            break;
        #endif
        }
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    switch (iChannel) {
        #if LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL
    case 0:
        if (UART0_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART0_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if ((UART0_C2 & UART0_S1) != 0) {                            // if transmit interrupt type enabled
                if (fnGenInt(irq_UART0_ID) != 0) {
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART0();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
    case 1:
        if (UART1_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART1_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART1_C2 & UART1_S1) {                                   // if transmit interrupt type enabled
                if (fnGenInt(irq_UART1_ID) != 0) {
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART1();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
        #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || (LPUARTS_AVAILABLE == 2 && UARTS_AVAILABLE == 1)
    case 2:
        if (UART2_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART2_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART2_C2 & UART2_S1) {                                   // if transmit interrupt type enabled
                if (fnGenInt(irq_UART2_ID) != 0) {                       // if UART2 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART2();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 3
    case 3:
        if (UART3_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART3_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART3_C2 & UART3_S1) {                                   // if transmit interrupt type enabled
                if (fnGenInt(irq_UART3_ID) != 0) {                       // if UART3 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART3();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 4
    case 4:
        if (UART4_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART4_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART4_C2 & UART4_S1) {                                   // if transmit interrupt type enabled
                if (fnGenInt(irq_UART4_ID) != 0) {                       // if UART4 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART4();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 5
    case 5:
        if (UART5_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART5_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART5_C2 & UART5_S1) {                                   // if transmit interrupt type enabled
                if (fnGenInt(irq_UART5_ID) != 0) {                       // if UART5 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART5();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
    }
    #endif
}



// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{
#if defined SERIAL_INTERFACE
    extern unsigned char ucTxLast[NUMBER_SERIAL];
#endif
    unsigned long ulNewActions = 0;
    int *ptrCnt;

    if (((iInts & CHANNEL_0_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART0];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_0_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx0(ucTxLast[0]);
                ulNewActions |= SEND_COM_0;
                fnUART_Tx_int(0);
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
		        iInts &= ~CHANNEL_1_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx1(ucTxLast[1]);
                ulNewActions |= SEND_COM_1;
                fnUART_Tx_int(1);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_2_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART2];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_2_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx2(ucTxLast[2]);
                ulNewActions |= SEND_COM_2;
                fnUART_Tx_int(2);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_3_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART3];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_3_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_3_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx3(ucTxLast[3]);
                ulNewActions |= SEND_COM_3;
                fnUART_Tx_int(3);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_4_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART4];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_4_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_4_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx4(ucTxLast[4]);
                ulNewActions |= SEND_COM_4;
                fnUART_Tx_int(4);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_5_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART5];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_5_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_5_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx5(ucTxLast[5]);
                ulNewActions |= SEND_COM_5;
                fnUART_Tx_int(5);
#endif
            }
        }
	}

#if NUMBER_EXTERNAL_SERIAL > 0
	if (((iInts & CHANNEL_0_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART0];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_0_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx0() != 0) {
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_0;
            }
        }
	}
	if (((iInts & CHANNEL_1_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART1];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_1_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_1_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx1() != 0) {
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_1;
            }
        }
	}
    #if NUMBER_EXTERNAL_SERIAL > 2
	if (((iInts & CHANNEL_2_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART2];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_2_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx2() != 0) {
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, fnMapPortBit(EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_2;
            }
        }
	}
	if (((iInts & CHANNEL_3_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART3];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_3_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_3_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx3() != 0) {
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, fnMapPortBit(EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_3;
            }
        }
	}
    #endif
#endif

#if defined I2C_INTERFACE
    if (((iInts & I2C_INT0) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT0;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT0;                                      // interrupt has been handled
		        iInts &= ~I2C_INT0;
                if ((I2C0_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
                    if (fnGenInt(irq_I2C0_ID) != 0) {                    // if I2C interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_I2C0();        // call the interrupt handler
                    }
                }
            }
        }
	}
#endif

#if NUMBER_I2C > 1
    if (((iInts & I2C_INT1) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C1];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT1;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT1;                                      // interrupt has been handled
                if ((I2C1_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
                    if (fnGenInt(irq_I2C1_ID) != 0) {                    // if I2C interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_I2C1();       // call the interrupt handler
                    }
                }
            }
        }
	}
#endif

#if NUMBER_I2C > 2
    if (((iInts & I2C_INT2) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C2];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT2;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT2;                                      // interrupt has been handled
                if ((I2C2_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
                    if (fnGenInt(irq_I2C2_ID) != 0) {                    // if I2C interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_I2C2();       // call the interrupt handler
                    }
                }
            }
        }
	}
#endif

#if NUMBER_I2C > 3
    if (((iInts & I2C_INT3) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C3];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT3;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT3;                                      // interrupt has been handled
                if ((I2C3_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
                    if (fnGenInt(irq_I2C3_ID) != 0) {                    // if I2C interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_I2C3();       // call the interrupt handler
                    }
                }
            }
        }
	}
#endif
#if defined USB_INTERFACE
    #if defined USB_HS_INTERFACE                                         // {12}
    if ((iInts & USBHS_INT) != 0) {
        int iEndpoint = 0;
        iInts &= ~USBHS_INT;
        while ((iEndpoint < NUMBER_OF_USBHS_ENDPOINTS) && (ulHSEndpointInt != 0)) {
            if (ulHSEndpointInt & (1 << iEndpoint)) {
                ulHSEndpointInt &= ~(1 << iEndpoint);
                fnCheckUSBOut(0, iEndpoint);
            }
            iEndpoint++;
        }
    }
    #endif
    if (((iInts & USB_INT) & ~iMasks) != 0) {                            // full-speed USB
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

#if defined USB_INTERFACE

#define _fnLE_add(x) (void *)x

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
#if defined USB_HS_INTERFACE                                             // {12}
    static unsigned char ucHSRxBank[NUMBER_OF_USB_ENDPOINTS];            // monitor the buffer to inject to
#endif
    unsigned char *ptrData;
#if defined USB_HOST_SUPPORT
    int iRealEndpoint = iEndPoint;
#endif
    KINETIS_USB_ENDPOINT_BD *ptrBDT = (KINETIS_USB_ENDPOINT_BD *)((BDT_PAGE_01 << 8) + (BDT_PAGE_02 << 16) + (BDT_PAGE_03 << 24)); // address of buffer descriptors in RAM
#if defined USB_HS_INTERFACE                                             // {12}
    if (USBHS_USBCMD & USBHS_USBCMD_RS) {                                // if the high speed USB controller is in the running state
        if (ptrDebugIn == 0) {                                           // bus state changes
            USBHS_USBSTS = 0;
            if (usLenEvent & USB_RESET_CMD) {
                USBHS_USBSTS |= (USBHS_USBINTR_URE);                     // set USB reset interrupt flag
                USBHS_PORTSC1 = USBHS_PORTSC1_PR;
                memset(ucHSRxBank,   0, sizeof(ucHSRxBank));             // default is even bank
                memset(ucHSTxBuffer, 0, sizeof(ucHSTxBuffer));           // default is even buffer
            }
            if (usLenEvent & USB_SLEEP_CMD) {
                USBHS_PORTSC1 |= USBHS_PORTSC1_SUSP;
                USBHS_USBSTS |= USBHS_USBINTR_SLE;
            }
            if (usLenEvent & USB_RESUME_CMD) {
                USBHS_PORTSC1 &= ~USBHS_PORTSC1_SUSP;
                USBHS_USBSTS |= USBHS_USBINTR_PCE;
            }
            if (usLenEvent & USB_IN_SUCCESS) {
                USBHS_EPCOMPLETE |= (USBHS_EPCOMPLETE_ETCE0 << iEndPoint); // transmission complete
                USBHS_USBSTS |= USBHS_USBINTR_UE;                        // transfer complete status
            }
        }
        else {
            KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR + (2 * iEndPoint); // endpoint's reception queue header
            if ((unsigned char)(USBHS_DEVICEADDR >> USBHS_DEVICEADDR_USBADR_SHIFT) != (unsigned char)iDevice) { // not our device address so ignore
                if (iDevice != 0xff) {                                   // special broadcast for simulator use so that it doesn't have to know the USB address
                    return 1;
                }
            }
            USBHS_USBSTS |= USBHS_USBINTR_UE;                            // transfer complete status

            switch (ucPID) {
            case SETUP_PID:
                uMemcpy(ptrQueueHeader->ucSetupBuffer, ptrDebugIn, usLenEvent); // setup data content (always 8 bytes in length) is stored directly in the queue header
                USBHS_EPSETUPSR |= (USBHS_EPSETUPSR_SETUP0 << iEndPoint); // setup complete
                break;
            case OUT_PID:                                                // this presently generates an end of reception on each packet received - the USBHS controller does it only on complete data reception
                if (ptrQueueHeader->CurrentdTD_pointer != 0) {
                    if (ptrQueueHeader->CurrentdTD_pointer->ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE) { // transfer buffer enabled for reception
                        if (ptrQueueHeader->CurrentdTD_pointer->ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_IOC) { // if an interrupt is to be generated on completion
                            USBHS_EPCOMPLETE |= (USBHS_EPCOMPLETE_ERCE0 << iEndPoint); // reception complete
                        }
                        uMemcpy((void *)ptrQueueHeader->CurrentdTD_pointer->ulBufferPointerPage[0], ptrDebugIn, usLenEvent); // copy data
                        ptrQueueHeader->CurrentdTD_pointer->ul_dtToken -= (usLenEvent << ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT);
                        ptrQueueHeader->CurrentdTD_pointer->ul_dtToken &= ~(ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_MASK);
                    }
                }
                break;
            }
            ucRxBank[iEndPoint] ^= ODD_BANK;                             // the next one to be used - toggle mechanism
        }
        if (fnGenInt(irq_USB_HS_ID) != 0) {                              // if USB HS interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_USB_HS();                  // call the interrupt handler
        } 
    }
#endif
#if defined USB_HOST_SUPPORT                                             // {25}
    if ((CTL & (USB_EN_SOF_EN | HOST_MODE_EN)) == 0) {                   // if the USB controller is not enabled in host or device mode ignore inputs
        return 0;                                                        // ignore
    }
    if ((CTL & HOST_MODE_EN) != 0) {                                     // if host mode of operation
        iEndPoint = 0;                                                   // in host mode all reception is on endpoint 0
    }
#else
    if ((CTL & USB_EN_SOF_EN) == 0) {                                    // {13} if the USB controller is not enabled ignore inputs
        return 0;
    }
#endif

    STAT = ((unsigned char)iEndPoint << END_POINT_SHIFT);                // set the endpoint to the STAT register
    if (ptrDebugIn == 0) {                                               // bus state changes
        INT_STAT = 0;
        if ((usLenEvent & USB_RESET_CMD) != 0) {
            INT_STAT |= USB_RST;
            memset(ucRxBank,   0, sizeof(ucRxBank));                     // default is even bank
            memset(ucTxBuffer, 0, sizeof(ucTxBuffer));                   // default is even buffer
        }
        if ((usLenEvent & USB_SLEEP_CMD) != 0) {
            INT_STAT |= SLEEP;
        }
        if ((usLenEvent & USB_RESUME_CMD) != 0) {
            INT_STAT |= RESUME;
        }
        if ((usLenEvent & USB_IN_SUCCESS) != 0) {
            INT_STAT |= (TOK_DNE);                                       // interrupt status
            STAT |= TX_TRANSACTION;                                      // set status bit to indicate that a transmission was successful
            STAT |= (ucTxBuffer[iEndPoint] ^ ODD_BANK);                  // signal which buffer the frame is in
        }
    #if defined USB_HOST_SUPPORT                                         // {25}
        if ((USB_FULLSPEED_ATTACH_CMD & usLenEvent) != 0) {
            INT_STAT |= ATTACH;                                          // attach interrupt
            CTL |= JSTATE;                                               // full speed state (D+ is pulled up by device)
        }
        else if ((USB_LOWSPEED_ATTACH_CMD & usLenEvent) != 0) {
            INT_STAT |= ATTACH;                                          // attach interrupt
            CTL &= ~JSTATE;                                              // low speed state (D- is pulled up by device and D+ is low)
        }
        else if ((USB_SOF_EVENT & usLenEvent) != 0) {                    // SOF interrupt
            INT_STAT |= SOF_TOK;
        }
    #endif
    }
    else {                                                               // data being injected
        unsigned short usLength;
        if ((ADDR & ~LS_EN) != iDevice) {                                // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        INT_STAT |= TOK_DNE;                                             // interrupt status                                           
        STAT |= ucRxBank[iEndPoint];                                     // the buffer used for this reception

        ptrBDT += iEndPoint;
        if ((ucRxBank[iEndPoint] & ODD_BANK) == 0) {                     // even buffer
            if ((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & OWN) == 0) {   // check whether it is owned by the USB controller
                _EXCEPTION("Rx buffer not ready!!");
                return 1;                                                // no controller ownership so ignore
            }
            if ((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & KEEP_OWNERSHIP) == 0) {
                ptrBDT->usb_bd_rx_even.ulUSB_BDControl &= ~OWN;          // mark that the buffer is no longer owned by the USB controller
            }
            usLength = (unsigned short)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // the size that this endpoint can receive
            if (usLength < usLenEvent) {                                 // limit the injected length to the endpoint limit
                usLenEvent = usLength;
            }
    #if defined USB_HOST_SUPPORT
            if ((CTL & HOST_MODE_EN) != 0) {
                if (ptrBDT->usb_bd_rx_even.ulUSB_BDControl & DTS) {      // if data toggle synchronisation is being used
                    if (((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & DATA_1) == 0) == iData1Frame[iRealEndpoint]) { // check that the receive buffer accepts this data token
                        _EXCEPTION("Wrong Rx buffer!!");
                    }
                }
            }
    #endif
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl &= ~(RX_PID_MASK | USB_BYTE_CNT_MASK | DATA_1);
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= (ucPID << RX_PID_SHIFT);
            ptrData = ptrBDT->usb_bd_rx_even.ptrUSB_BD_Data;
            ptrData = _fnLE_add((unsigned long)ptrData);
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= SET_FRAME_LENGTH(usLenEvent); // add length
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {                             // in host mode
                if (STALL_PID == ucPID) {                                // if stall being injected on this endpoint
                    ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= (STALL_PID << RX_PID_SHIFT);
                }
                else if (iData1Frame[iRealEndpoint] != 0) {              // if the endpoint is expecting a DATA 1 frame
                    ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= ((DATA1_PID << RX_PID_SHIFT) | DATA_1); // DATA1 frame
                }
                else {
                    ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= (DATA0_PID << RX_PID_SHIFT); // DATA0 frame
                }
                iData1Frame[iRealEndpoint] ^= 1;                         // toggle the DATA type
                CTL &= ~(TXSUSPEND_TOKENBUSY);                           // token no longer in progress
            }
    #endif
        }
        else {                                                           // odd buffer
            if ((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & OWN) == 0) {
                _EXCEPTION("Rx buffer not ready!!");
                return 1;                                                // no controller ownership so ignore
            }
            if ((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & KEEP_OWNERSHIP) == 0) {
                ptrBDT->usb_bd_rx_odd.ulUSB_BDControl &= ~OWN;           // the buffer descriptor is now owned by the controller
            }
            usLength = (unsigned short)((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // the size that this endpoint can receive
            if (usLength < usLenEvent) {                                 // limit the injected length to the endpoint limit
                usLenEvent = usLength;
            }
    #if defined USB_HOST_SUPPORT
            if ((CTL & HOST_MODE_EN) != 0) {
                if ((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & DTS) != 0) {// if data toggle synchronisation is being used
                    if (((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & DATA_1) == 0) == iData1Frame[iRealEndpoint]) { // check that the receive buffer accepts this data token
                        _EXCEPTION("Wrong Rx buffer!!");
                    }
                }
            }
    #endif
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl &= ~(RX_PID_MASK | USB_BYTE_CNT_MASK);
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= (ucPID << RX_PID_SHIFT);
            ptrData = ptrBDT->usb_bd_rx_odd.ptrUSB_BD_Data;
            ptrData = _fnLE_add((unsigned long)ptrData);
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= SET_FRAME_LENGTH(usLenEvent); // add length
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {
                if (STALL_PID == ucPID) {
                    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= (STALL_PID << RX_PID_SHIFT);
                }
                else if (iData1Frame[iRealEndpoint] != 0) {
                    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= ((DATA1_PID << RX_PID_SHIFT) | DATA_1); // DATA1 frame
                }
                else {
                    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= (DATA0_PID << RX_PID_SHIFT); // DATA0 frame
                }
                iData1Frame[iRealEndpoint] ^= 1;
                CTL &= ~(TXSUSPEND_TOKENBUSY);                           // token no longer in progress
            }
    #endif
        }
        memcpy(ptrData, ptrDebugIn, usLenEvent);
        ucRxBank[iEndPoint] ^= ODD_BANK;                                 // the next rx buffer to be used - toggle mechanism
    }
    if (fnGenInt(irq_USB_OTG_ID) != 0) {                                 // if USB interrupt is not disabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        ptrVect->processor_interrupts.irq_USB_OTG();                     // call the interrupt handler
    } 
    return 0;
}

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
    case USB_SIM_SOF:
        if ((CTL & HOST_MODE_EN) && (INT_ENB & SOF_TOK_EN)) {            // if host mode and SOF enabled
            iInts |= USB_INT;;
            ulEndpointInt = 1;                                           // flag endpoint - always endpoint 0 used
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
#if defined USB_HS_INTERFACE                                             // {12}
        if (ptrUSB_HW->ucDeviceType == USB_DEVICE_HS) {
            KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
            ptrQueueHeader += ((2 * iEndpoint) + 1);                     // move to the transmitter queue header
            if (ptrQueueHeader->CurrentdTD_pointer != 0) {
                uMemcpy((void *)ptrQueueHeader->CurrentdTD_pointer, &ptrQueueHeader->dTD, sizeof(USB_HS_TRANSFER_OVERLAY)); // transfer the transfer block content to the overlay block
                if (ptrQueueHeader->CurrentdTD_pointer->ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_IOC) { // if transfer termination should generate an interrupt
                    iInts |= USBHS_INT;                                  // flag that the interrupt should be handled
                    ulHSEndpointInt |= (1 << iEndpoint);                 // flag endpoint
                }
            }
            break;
        }
#endif
        if ((OWN & *ptrUSB_HW->ptr_ulUSB_BDControl) == 0) {              // if the ownership has not been passed to the USB controller ignore it
            _EXCEPTION("Ignored frame due to incorrect buffer ownership");
            return;
        }
        iInts |= USB_INT;                                                // flag that the interrupt should be handled
        ulEndpointInt |= (1 << iEndpoint);                               // flag endpoint
        break;
    case USB_SIM_ENUMERATED:                                             // flag that we have completed enumeration
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


#if defined USB_HOST_SUPPORT                                             // {25}

#define MAX_TOKENS 10
static unsigned char ucTokenQueue[MAX_TOKENS] = {0};
static int iTokenIn = 0;
static int iTokenOut = 0;

extern void fnAddToken(unsigned char ucToken)
{
    ucTokenQueue[iTokenIn++] = ucToken;
    if (iTokenIn >= MAX_TOKENS) {
        iTokenIn = 0;
    }
    iInts |= USB_INT;                                                    // there will generally be an interrupt after this so set the interrupt flag
    ulEndpointInt |= (0x1 << (ucToken & 0x0f));
}

static unsigned char ucGetToken(int iNoIN)
{
    unsigned char ucToken;
    do {
        ucToken = ucTokenQueue[iTokenOut];
        if (ucToken != 0) {
            ucTokenQueue[iTokenOut] = 0;
            ++iTokenOut;
            if (iTokenOut >= MAX_TOKENS) {
                iTokenOut = 0;
            }
        }
    } while ((iNoIN != 0) && ((ucToken >> 4) == IN_PID));                // skip IN tokens if so requested
    return ucToken;
}

#if defined USB_MSD_HOST
#define MEMORY_STICK_USES_SHARED_ENDPOINTS
#define DEVICE_ENPOINT0_LENGTH   64
static const unsigned char ucDeviceDescriptor[] = {                      // constant device descriptor (example of memory stick)
    0x12,
    0x01,
    0x00, 0x02,
    0x00, 0x00, 0x00,
    DEVICE_ENPOINT0_LENGTH,
    0x0d, 0x1d,
    0x13, 0x02,
    0x10, 0x01,
    0x01, 0x02, 0x03,
    0x01
};

static const unsigned char ucConfigDescriptor[] = {                      // constant configuration descriptor (example of memory stick)
    0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x64,                // configuration descriptor
    0x09, 0x04, 0x00, 0x00, 0x03, 0x08, 0x06, 0x50, 0x00,                // interface descriptor
    #if defined MEMORY_STICK_USES_SHARED_ENDPOINTS
    0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,                            // bulk IN endpoint - 64 bytes on endpoint 2
    #else
    0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00,                            // bulk IN endpoint - 64 bytes on endpoint 1
    #endif
    0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,                            // bulk OUT endpoint - 64 bytes on endpoint 2
    0x07, 0x05, 0x83, 0x03, 0x02, 0x00, 0x01                             // interrupt IN endpoint - 2 bytes - 1ms polling rate
};


static unsigned char ucDataTransport[512] = {0};
static unsigned short usDataTransportLength = 0;
static int iStall = 0;
static unsigned long ulFrameCount = 0;
static unsigned char ucLastOpCode = 0;
static unsigned char ucPresentLUN = 0;
static unsigned long ulLBA = 0;
static unsigned char ucMemoryStickSector[512] = {0};
static int iSectorOffset = 0;

static unsigned char ucStatusTransport[] = {
    0x55, 0x53, 0x42, 0x53,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00
};

static const unsigned char ucMaxLum[1] = {0};

static int fnMsdModel(unsigned char *ptrUSBData, unsigned short usUSBLength)
{
    USB_MASS_STORAGE_CBW_LW *ptrBlockWrapper = (USB_MASS_STORAGE_CBW_LW *)ptrUSBData;
    if (ucLastOpCode == UFI_WRITE_10) {                                  // OUT data that belongs to a write is not a block wrapper
        if (usUSBLength != 64) {
            _EXCEPTION("Endpoint size of 64 always expected!!");
        }
        memcpy(&ucMemoryStickSector[iSectorOffset], ptrUSBData, usUSBLength); // collect the data into the temporary buffer
        iSectorOffset += usUSBLength;                                    // the new length of data collected
        if (--ulFrameCount == 0) {
            ucLastOpCode = 0;                                            // complete write has been received so reset the op code
        }
        if (iSectorOffset >= 512) {                                      // if a complete sector is available
            fnPutSimDiskData(ucMemoryStickSector, ucPresentLUN, ulLBA++);// save it to the memory stick
            iSectorOffset = 0;
        }
        return 1;                                                        // OUT belonging to a write has been handled
    }
    if (uMemcmp(ptrUSBData, "USBC", 4) != 0) {
        _EXCEPTION("USBC expected!");
    }
    ptrUSBData += 4;
    uMemcpy(&ucStatusTransport[4], ptrUSBData, 4);                       // tag ID for status stage
    ulFrameCount = 1;                                                    // default is that the content fits in a single frame
    ucLastOpCode = ptrBlockWrapper->CBWCB[0];
    switch (ucLastOpCode) {                                              // op code
    case UFI_INQUIRY:
        {
            static const unsigned char ucUFI_INQUIRY[] = {               // reference response to UFI_INQUIRY
                0x00, 0x80, 0x02, 0x02,                                  // removable SCSI-2
                0x1f, 0x00, 0x00, 0x00,                                  // length 31
                0x53, 0x57, 0x49, 0x53,                                  // SWISSBIT
                0x53, 0x42, 0x49, 0x54,
                0x56, 0x69, 0x63, 0x74,                                  // Victorinox 2.0
                0x6f, 0x72, 0x69, 0x6e,
                0x6f, 0x78, 0x20, 0x32,
                0x2e, 0x30, 0x20, 0x20,
                0x32, 0x2e, 0x30, 0x30
            };
            memcpy(ucDataTransport, ucUFI_INQUIRY, sizeof(ucUFI_INQUIRY));
            usDataTransportLength = sizeof(ucUFI_INQUIRY);
//iStall = 1; // test stalling
        }
        break;
    case UFI_REQUEST_SENSE:
        {
            static const unsigned char ucUFI_REQUEST_SENSE[] = {         // reference response to UFI_REQUEST_SENSE
                0x70, 0x00, 0x06, 0x00,                                  // unit attention
                0x00, 0x00, 0x00, 0x0a,
                0x00, 0x00, 0x00, 0x00,
                0x28, 0x00, 0x00, 0x00,
                0x00, 0x00
            };
            memcpy(ucDataTransport, ucUFI_REQUEST_SENSE, sizeof(ucUFI_REQUEST_SENSE));
            usDataTransportLength = sizeof(ucUFI_REQUEST_SENSE);
        }
        break;
    case UFI_READ_FORMAT_CAPACITY:
        {
            static const unsigned char ucUFI_READ_FORMAT_CAPACITY[] = {  // reference response to UFI_READ_FORMAT_CAPACITY
                0x00, 0x00, 0x00, 0x08,                                  // capacity list length 8
                0x01, 0x00, 0x00, 0x00,                                  // number of blocks 16777216
                0x03, 0x00, 0x02, 0x00                                   // block size 512
            };
            memcpy(ucDataTransport, ucUFI_READ_FORMAT_CAPACITY, sizeof(ucUFI_READ_FORMAT_CAPACITY));
            usDataTransportLength = sizeof(ucUFI_READ_FORMAT_CAPACITY);
        }
        break;
    case UFI_READ_CAPACITY:
        {
            static const unsigned char ucUFI_READ_CAPACITY[] = {         // reference response to UFI_READ_CAPACITY
                0x00, 0x0f, 0x82, 0xff,                                  // logical bock address 1016575
                0x01, 0x00, 0x20, 0x00                                   // logical block length 512 (496.392 MB capacity)
            };
            memcpy(ucDataTransport, ucUFI_READ_CAPACITY, sizeof(ucUFI_READ_CAPACITY));
            usDataTransportLength = sizeof(ucUFI_READ_CAPACITY);
        }
        break;
    case UFI_WRITE_10:
    case UFI_READ_10:
        {
            if (ptrBlockWrapper->dCBWCBLength != 10) {
                _EXCEPTION("Incorrect read length!!");
            }
            ulLBA = ((ptrBlockWrapper->CBWCB[2] << 24) | (ptrBlockWrapper->CBWCB[3] << 16) | (ptrBlockWrapper->CBWCB[4] << 8) | (ptrBlockWrapper->CBWCB[5])); // the block number where the read starts
            ulFrameCount = ((ptrBlockWrapper->CBWCB[6] << 16) | (ptrBlockWrapper->CBWCB[7] << 8) | (ptrBlockWrapper->CBWCB[8])); // the number of bocks to be read
            ulFrameCount *= (512/64);                                    // the total amount of USB frames that will be returned
            ucPresentLUN = ptrBlockWrapper->dCBWLUN;
            if (UFI_READ_10 == ucLastOpCode) {
                fnGetSimDiskData(ucMemoryStickSector, ucPresentLUN, ulLBA); // read the sector's content from the simulated memory stick
                usDataTransportLength = 64;                              // endpoint assumed to be 64 bytes in length
                memcpy(ucDataTransport, ucMemoryStickSector, usDataTransportLength); // prepare the frame to be returned
                iSectorOffset = 64;
            }
            else {
                usDataTransportLength = 0;
                iSectorOffset = 0;
                return 1;                                                // write
            }
        }
        break;
    default:
        _EXCEPTION("Unexpected Op Code!");
        break;
    }
    return 0;
}
#elif defined USB_CDC_HOST
#define DEVICE_ENPOINT0_LENGTH   8
static const unsigned char ucDeviceDescriptor[] = {                      // constant device descriptor (example of CDC)
    0x12, 0x01, 0x00, 0x02, 0xef, 0x02, 0x01, DEVICE_ENPOINT0_LENGTH, 0xA2, 0x15, 0x44, 0x00, 0x00, 0x01,
    0x01, 0x02, 0x03, 0x01 
};

static const unsigned char ucConfigDescriptor[] = {                      // constant configuration descriptor (example of CDC)
    0x09, 0x02, 0x4b, 0x00, 0x02, 0x01, 0x04, 0xc0, 0x00,                // configuration descriptor
    0x08, 0x0b, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00,                      // interface association descriptor
    0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x05,                // interface descriptor
    0x05, 0x24, 0x00, 0x00, 0x02,                                        // class descriptor
    0x05, 0x24, 0x01, 0x01, 0x00,                                        // class descriptor
    0x04, 0x24, 0x02, 0x02,                                              // class descriptor
    0x05, 0x24, 0x06, 0x00, 0x01,                                        // class descriptor
    0x07, 0x05, 0x81, 0x03, 0x20, 0x00, 0x0a,                            // interrupt IN endpoint - 32 bytes on endpoint 1
    0x09, 0x04, 0x01, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x05,                // interface descriptor
    0x07, 0x05, 0x02, 0x02, 0x20, 0x00, 0x00,                            // bulk OUT endpoint - 32 bytes on endpoint 2
    0x07, 0x05, 0x82, 0x02, 0x20, 0x00, 0x00,                            // bulk OUT endpoint - 32 bytes on endpoint 2
};
#endif


static unsigned char ucStringDescriptor[] = {                            // string descriptor
    0x00,                                                                // length
    0x03,                                                                // type
    0x00, 0x00,                                                          // variable length content
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
};

#define QUEUE_DEVICE_DESCRIPTOR 1
#define QUEUE_ZERO_DATA         2
#define QUEUE_CONFIG_DESCRIPTOR 3
#define QUEUE_STRING_DESCRIPTOR 4
#define QUEUE_MAX_LUN           5

static void fnSimulateUSB_ep0(int iAdd, unsigned char *ptrData, unsigned short usSize)
{
    unsigned short usSent = 0;
    unsigned short usRemaining = usSize;
    while (usRemaining != 0) {
        if (usRemaining > DEVICE_ENPOINT0_LENGTH) {
            fnSimulateUSB(iAdd, 0, 0, ptrData, DEVICE_ENPOINT0_LENGTH);
            usRemaining -= DEVICE_ENPOINT0_LENGTH;
            ptrData += DEVICE_ENPOINT0_LENGTH;
        }
        else {
            fnSimulateUSB(iAdd, 0, 0, ptrData, usRemaining);
            if (usRemaining == DEVICE_ENPOINT0_LENGTH) {
                fnSimulateUSB(iAdd, 0, 0, ptrData, 0);                   // terminate with zero length frame
            }
            break;
        }
    }
}

// This routine reacts to embedded host mode transfers as a device would do
//
static void fnUSBHostModel(int iEndpoint, unsigned char ucPID, unsigned short usUSBLength, unsigned char *ptrUSBData)
{
    static unsigned char ucStringRef = 0;
    static int iHostQueue = 0;

    switch (ucPID) {
    case 0:                                                              // reset
        iHostQueue = 0;
        break;
    case SETUP_PID:                                                      // the host is sending setup data
        {
            USB_HOST_DESCRIPTOR *ptrDescriptor = (USB_HOST_DESCRIPTOR *)ptrUSBData;
            if (usUSBLength != 8) {
                _EXCEPTION("Bad descriptor length detected!");
            }
            if (ptrDescriptor->bmRecipient_device_direction == (STANDARD_DEVICE_TO_HOST | REQUEST_INTERFACE_CLASS)) { // class spefific interface request
    #if defined USB_MSD_HOST
                if (GET_MAX_LUN == ptrDescriptor->bRequest) {            // requesting maximum LUN
                    iHostQueue = QUEUE_MAX_LUN;                          // send the data
                }
    #endif
                break;
            }
            if (USB_REQUEST_GET_DESCRIPTOR == ptrDescriptor->bRequest) { // host is requesting a descriptor 
                if (ptrDescriptor->wValue[1] == (STANDARD_DEVICE_DESCRIPTOR >> 8)) { // requesting the standard device descriptor
                    iHostQueue = QUEUE_DEVICE_DESCRIPTOR;                // send the device descriptor on next IN
                }
                else if (ptrDescriptor->wValue[1] == (STANDARD_CONFIG_DESCRIPTOR >> 8)) { // requesting the standard configuration descriptor
                    iHostQueue = QUEUE_CONFIG_DESCRIPTOR;                // send the configuration descriptor on next IN
                }
                else if (ptrDescriptor->wValue[1] == (STANDARD_STRING_DESCRIPTOR >> 8)) { // host requesting a string
                    iHostQueue = QUEUE_STRING_DESCRIPTOR;                // send the configuration descriptor on next IN
                    switch (ptrDescriptor->wValue[0]) {                  // the string referenced
                    case 0:                                              // string language ID
                        ucStringDescriptor[0] = 4;                       // length
                        ucStringDescriptor[2] = 0x09;                    // English (US)
                        ucStringDescriptor[3] = 0x04;
                        break;
                    case 1:                                              // manufacturer
                        ucStringDescriptor[0] = 0x12;                    // length
                        ucStringDescriptor[2] = 0x54;
                        ucStringDescriptor[3] = 0x00;
                        ucStringDescriptor[4] = 0x44;
                        ucStringDescriptor[5] = 0x00;
                        ucStringDescriptor[6] = 0x4b;
                        ucStringDescriptor[7] = 0x00;
                        ucStringDescriptor[8] = 0x4d;
                        ucStringDescriptor[9] = 0x00;
                        ucStringDescriptor[10] = 0x65;
                        ucStringDescriptor[11] = 0x00;
                        ucStringDescriptor[12] = 0x64;
                        ucStringDescriptor[13] = 0x00;
                        ucStringDescriptor[14] = 0x69;
                        ucStringDescriptor[15] = 0x00;
                        ucStringDescriptor[16] = 0x61;
                        ucStringDescriptor[17] = 0x00;
                        break;
                    case 2:                                              // product
                        ucStringDescriptor[0] = 0x10;                    // length
                        ucStringDescriptor[2] = 'C';
                        ucStringDescriptor[3] = 0x00;
                        ucStringDescriptor[4] = 'D';
                        ucStringDescriptor[5] = 0x00;
                        ucStringDescriptor[6] = 'C';
                        ucStringDescriptor[7] = 0x00;
                        ucStringDescriptor[8] = ' ';
                        ucStringDescriptor[9] = 0x00;
                        ucStringDescriptor[10] = ' ';
                        ucStringDescriptor[11] = 0x00;
                        ucStringDescriptor[12] = ' ';
                        ucStringDescriptor[13] = 0x00;
                        ucStringDescriptor[14] = ' ';
                        ucStringDescriptor[15] = 0x00;
                        break;
                    case 3:                                              // serial number
                        ucStringDescriptor[0] = 0x0c;                    // length
                        ucStringDescriptor[2] = 0x31;
                        ucStringDescriptor[3] = 0x00;
                        ucStringDescriptor[4] = 0x32;
                        ucStringDescriptor[5] = 0x00;
                        ucStringDescriptor[6] = 0x33;
                        ucStringDescriptor[7] = 0x00;
                        ucStringDescriptor[8] = 0x34;
                        ucStringDescriptor[9] = 0x00;
                        ucStringDescriptor[10] = 0x35;
                        ucStringDescriptor[11] = 0x00;
                        break;
                    }
                }
            }
            else if (USB_REQUEST_SET_ADDRESS == ptrDescriptor->bRequest) { // address being set
                static unsigned char ucDeviceAddress;
                ucDeviceAddress = ptrDescriptor->wValue[0];
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else if (USB_REQUEST_SET_CONFIGURATION == ptrDescriptor->bRequest) { // configuration being set
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else if (USB_REQUEST_CLEAR_FEATURE == ptrDescriptor->bRequest) { // clear feature
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else if (USB_REQUEST_SET_INTERFACE == ptrDescriptor->bRequest) { // set interface
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else {
                _EXCEPTION("Unknown request");
            }
        }
        break;
    case IN_PID:                                                         // the device can insert data since the host is sending IN PID
        switch (iHostQueue) {
        case QUEUE_DEVICE_DESCRIPTOR:                                    // inject the device descriptor
            fnSimulateUSB_ep0((ADDR & ~LS_EN),(unsigned char *)&ucDeviceDescriptor, sizeof(ucDeviceDescriptor));
            break;
        case QUEUE_CONFIG_DESCRIPTOR:
            fnSimulateUSB_ep0((ADDR & ~LS_EN),(unsigned char *)&ucConfigDescriptor, sizeof(ucConfigDescriptor));
            break;
        case QUEUE_STRING_DESCRIPTOR:
            fnSimulateUSB_ep0((ADDR & ~LS_EN), (unsigned char *)&ucStringDescriptor, ucStringDescriptor[0]);
            break;
    #if defined USB_MSD_HOST
        case QUEUE_MAX_LUN:
            fnSimulateUSB((ADDR & ~LS_EN), 0, 0, (unsigned char *)&ucMaxLum, sizeof(ucMaxLum));
            break;
    #endif
        case QUEUE_ZERO_DATA:
            fnSimulateUSB((ADDR & ~LS_EN), 0, 0, (unsigned char *)1, 0); // inject a zero data frame
            break;
        default:
    #if defined USB_MSD_HOST
        #if defined MEMORY_STICK_USES_SHARED_ENDPOINTS
            if (iEndpoint == 2)                                          // IN is on endpoint 2
        #else
            if (iEndpoint == 1)                                          // IN is on endpoint 1
        #endif
            {
                if (iStall != 0) {
                    // Stall the endoint
                    //
                    iStall = 0;
                    fnSimulateUSB((ADDR & ~LS_EN), iEndpoint, STALL_PID, (unsigned char *)1, 0); // inject a stall token on this IN endpoint
                    break;
                }
                if (ulFrameCount != 0) {
                    fnSimulateUSB((ADDR & ~LS_EN), iEndpoint, 0, (unsigned char *)&ucDataTransport, usDataTransportLength);
                    ulFrameCount--;
                    if (ulFrameCount != 0) {                             // if the data occupies multiple frames
                        if (ucLastOpCode == UFI_READ_10) {               // we are presently reading sector content
                            if (iSectorOffset >= 512) {                  // if a complete sector has been read
                                iSectorOffset = 0;
                                ulLBA++;                                 // move to the next sector
                                fnGetSimDiskData(ucMemoryStickSector, ucPresentLUN, ulLBA); // read the sector's content from the simulated memory stick
                            }
                            usDataTransportLength = 64;                  // endpoint assumed to be 64 bytes in length
                            memcpy(ucDataTransport, (ucMemoryStickSector + iSectorOffset), usDataTransportLength); // prepare the frame to be returned
                            iSectorOffset += 64;
                        }
                        else {
                            _EXCEPTION("Add further op codes!!");
                        }

                    }
                }
                else {
                    fnSimulateUSB((ADDR & ~LS_EN), iEndpoint, 0, (unsigned char *)&ucStatusTransport, sizeof(ucStatusTransport));
                }
            }
    #endif
            break;
        }
        iHostQueue = 0;
        break;
    case OUT_PID:
    #if defined USB_MSD_HOST
        if (2 == iEndpoint) {                                            // we expect bulk out on this endpoint only
            fnMsdModel(ptrUSBData, usUSBLength);                         // allow MSD model to handle the data
        }
        else if ((0 == iEndpoint) && (usUSBLength == 0)) {               // expect only zero terminations on endpoint 0
        }
        else {
            _EXCEPTION("Unexpected");
        }
    #endif
        break;
    }
}

// Synchronise to correct data frame for all endpoints
//
extern void fnResetUSB_buffers(void)
{
    int i = 0;
    iData1Frame[i++] = 1;                                                // reset endpoint 0 to DATA1
    while (i < NUMBER_OF_USB_ENDPOINTS) {                                // {42}
        iData1Frame[i++] = 0;                                            // reset additional endpoint data frames to DATA0
    }
}
#endif

// Check whether data has been prepared for transmission
//
extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
    KINETIS_USB_BD *bufferDescriptor;
    unsigned long ulAddress = ((BDT_PAGE_01 << 8) | (BDT_PAGE_02 << 16) | (BDT_PAGE_03 << 24));
    KINETIS_USB_ENDPOINT_BD *bdt = (KINETIS_USB_ENDPOINT_BD *)ulAddress;
    #if TICK_RESOLUTION >= 1000
    int iMaxUSB_ints = (TICK_RESOLUTION/1000);
    #else
    int iMaxUSB_ints = 1;
    #endif
    #if defined USB_HOST_SUPPORT
    int iRealEndpoint = iEndpoint;
    #endif
    #if defined USB_HS_INTERFACE                                         // {12}
    if ((USBHS_USBCMD & USBHS_USBCMD_RS) != 0) {                         // if USB HS controller operating
        KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
        ptrQueueHeader += ((2 * iEndpoint) + 1);                         // move to the transmitter queue header
        if (ptrQueueHeader->CurrentdTD_pointer != 0) {
            unsigned short usDataLength = (unsigned short)(ptrQueueHeader->CurrentdTD_pointer->ul_dtToken >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT);
            unsigned short usMaxPacket = (unsigned short)(ptrQueueHeader->ulCapabilities >> 16);
            unsigned char *ptrUSBData = (unsigned char *)ptrQueueHeader->CurrentdTD_pointer->ulBufferPointerPage[0];
            do {                                                         // the USB HS controller automatically sends complete buffers as single frames and there is only a single interrupt on completion
                if (usDataLength < usMaxPacket) {
                    usMaxPacket = usDataLength;
                }
                fnLogUSB(iEndpoint, 0, usMaxPacket, ptrUSBData, ((ucHSTxBuffer[iEndpoint] & ODD_BANK) != 0));
                ucHSTxBuffer[iEndpoint] ^= ODD_BANK;                     // toggle buffer
                usDataLength -= usMaxPacket;
                ptrUSBData += usMaxPacket;
            } while (usDataLength != 0);
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
            ptrQueueHeader->CurrentdTD_pointer->ulBufferPointerPage[0] = (unsigned long)ptrUSBData;
        }
    }
    #endif
    if ((CTL & USB_EN_SOF_EN) == 0) {                                    // {13} if the FS USB controller is not enabled ignore inputs
        return;
    }
    bdt += iEndpoint;

    do {
    #if defined USB_HOST_SUPPORT                                         // {25}
        if ((CTL & HOST_MODE_EN) != 0) {
            iEndpoint = 0;
            bdt = (KINETIS_USB_ENDPOINT_BD *)ulAddress;                  // in host mode only endpoint 0 buffers are used
            if ((INT_ENB & SOF_TOK_EN) != 0) {                           // if host mode and SOF enabled
                fnSimulateUSB(iDevice, 0, 0, 0, USB_SOF_EVENT);          // generate SOF interrupt
            }
        }
    #endif
        if (ucTxBuffer[iEndpoint] != 0) {                                // decide which is the active output buffer descriptor
            bufferDescriptor = &bdt->usb_bd_tx_odd;                      // the odd one is to be used
        }
        else {
            bufferDescriptor = &bdt->usb_bd_tx_even;                     // the even one is to be used
        }
        if ((bufferDescriptor->ulUSB_BDControl & OWN) != 0) {            // owned by USB controller so interpret it
            unsigned short usUSBLength;
            unsigned char *ptrUSBData = 0;
            usUSBLength  = (unsigned char)((bufferDescriptor->ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // get the length from the control register
            if (usUSBLength != 0) {
                ptrUSBData = _fnLE_add((CAST_POINTER_ARITHMETIC)bufferDescriptor->ptrUSB_BD_Data); // the data to be sent
            }
            if ((bufferDescriptor->ulUSB_BDControl & KEEP_OWNERSHIP) == 0) { // if the KEEP bit is not set
                bufferDescriptor->ulUSB_BDControl &= ~OWN;               // remove SIE ownership
            }
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {                             // if in host mode
                unsigned char ucToken = ucGetToken(1);                   // the token that was sent (skip INs)
                if ((ucToken >> 4) == SETUP_PID) {                       // a SETUP token was sent (will always be on control endpoint 0
                    fnLogUSB(iRealEndpoint, SETUP_PID, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0)); // log the transmitted data
                    fnUSBHostModel((ucToken & 0x0f), SETUP_PID, usUSBLength, ptrUSBData); // let the host model handle the data
                }
                else if ((ucToken >> 4) == OUT_PID) {                    // an OUT token was sent
                    fnLogUSB(iRealEndpoint, 0, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0));
                    fnUSBHostModel((ucToken & 0x0f), OUT_PID, usUSBLength, ptrUSBData); // let the host model handle the data
                }
                else {
                    _EXCEPTION("Unexpected token queued");               // IN tokens are not expected to be returned since they should be skipped
                    return;
                }
                bufferDescriptor->ulUSB_BDControl &= ~(RX_PID_MASK);
                bufferDescriptor->ulUSB_BDControl |= (ACK_PID << RX_PID_SHIFT); // insert ACK
                ucTxBuffer[0] ^= ODD_BANK;                               // toggle buffer (always on endpoint 0)
                CTL &= ~(TXSUSPEND_TOKENBUSY);                           // token no longer in progress
                fnSimulateUSB(iDevice, 0, 0, 0, USB_IN_SUCCESS);         // generate tx interrupt (always on endpoint 0)
            }
            else {                                                       // device mode
                fnLogUSB(iRealEndpoint, 0, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0));
                ucTxBuffer[iRealEndpoint] ^= ODD_BANK;                   // toggle buffer
                fnSimulateUSB(iDevice, iRealEndpoint, 0, 0, USB_IN_SUCCESS); // generate tx interrupt
            }
    #else
            fnLogUSB(iEndpoint, 0, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0));
            ucTxBuffer[iEndpoint] ^= ODD_BANK;                           // toggle buffer
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
    #endif
        }                                                                // handle further buffer if available
        else {                                                           // no data to be sent - the buffer descrptior is not owned
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {                             // if in host mode
                unsigned char ucToken = ucGetToken(0);                   // we expect an IN
                if ((ucToken >> 4) == IN_PID) {                          // an IN token is to be sent so that the device can return data
                    fnUSBHostModel((ucToken & 0x0f), IN_PID, 0, 0);      // let the host model prepare the data
                }
                else if (ucToken != 0) {                                 // an OUT_PID is never expected here since the buffer descrptior would be owned if it were set as token
                    _EXCEPTION("Unexpected token");
                }
            }
    #endif
            break;
        }
        if (--iMaxUSB_ints == 0) {                                       // limit the number of interrupts handled in a single call (eg. to avoid isochronous INs causing an infinite loop)
            iMasks |= USB_INT;
            return;
        }
    } while (1);
}

// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    #if defined USB_HS_INTERFACE                                         // {12}
    KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
    ptrQueueHeader += (2 * iEndpoint);                                   // move to the receiver queue header
    return (unsigned short)(ptrQueueHeader->ulCapabilities >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT); // the endpoint reception buffer size
    #else
    KINETIS_USB_ENDPOINT_BD *ptrBDT = (KINETIS_USB_ENDPOINT_BD *)((BDT_PAGE_01 << 8) + (BDT_PAGE_02 << 16) + (BDT_PAGE_03 << 24));
    unsigned short usBufferSize;
    ptrBDT += iEndpoint;
    usBufferSize =  (unsigned short)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // get the length from the control register
    return usBufferSize;
    #endif
}
#endif

extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState)
{
// Note that the modem status bits are according to the MS specifications as passed by GetCommModemStatus().
// To avoid including MS headers, the bits are defined here - it is not expected that they will ever change...
//
#define MS_CTS_ON  0x0010
#define MS_DSR_ON  0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080                                                // carrier detect
#if defined SUPPORT_HW_FLOW
    unsigned long ulChange = (ulNewState ^ ulOldState);
#endif
}

// UART Break detection simulation
//
extern void fnSimulateBreak(int iPort)
{
}


// Process simulated DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
    unsigned long ulNewActions = 0;
    unsigned long ulChannel = 0x00000001;
    unsigned long iChannel = 0;
    int _iDMA = iDMA;
    #if defined SERIAL_INTERFACE && defined SERIAL_SUPPORT_DMA
    int *ptrCnt;
    #endif

    while (_iDMA != 0) {                                                 // while DMA operations to be performed
        if (_iDMA & ulChannel) {                                         // DMA request on this channel
            _iDMA &= ~ulChannel;
            switch (iChannel) {
    #if defined SERIAL_INTERFACE && defined SERIAL_SUPPORT_DMA           // {4}
            case DMA_UART0_TX_CHANNEL:                                   // handle UART DMA transmission on UART 0
        #if LPUARTS_AVAILABLE > 0 && !defined LPUARTS_PARALLEL
                if ((LPUART0_BAUD & LPUART_BAUD_TDMAE) != 0)             // if DMA operation is enabled
        #else
                if ((UART0_C5 & UART_C5_TDMAS) != 0)                     // if DMA operation is enabled
        #endif
                {
                    ptrCnt = (int *)argv[THROUGHPUT_UART0];              // the number of characters in each tick period
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            if (fnSimulateDMA(iChannel) > 0) {           // process the trigger
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(0);                        // handle possible pending interrupt after DMA completion
                            }
        #if LPUARTS_AVAILABLE > 0 && !defined LPUARTS_PARALLEL
	                        fnLogTx0((unsigned char)LPUART0_DATA);
        #else
	                        fnLogTx0(UART0_D);
        #endif
                            ulNewActions |= SEND_COM_0;
                        }
                    }
                }
                break;
        #if ((UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1)
            case DMA_UART1_TX_CHANNEL:                                   // handle UART DMA transmission on UART 1
            #if LPUARTS_AVAILABLE > 1 && !defined LPUARTS_PARALLEL
                if (LPUART1_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #elif defined KINETIS_KL
                if (UART1_C4 & UART_C4_TDMAS)
            #else
                if (UART1_C5 & UART_C5_TDMAS)
            #endif
                {                                                        // if DMA operation is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_UART1];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            if (fnSimulateDMA(iChannel) > 0) {           // process the trigger
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(1);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 1 && !defined LPUARTS_PARALLEL
	                        fnLogTx1((unsigned char)LPUART1_DATA);
            #else
	                        fnLogTx1(UART1_D);
            #endif
                            ulNewActions |= SEND_COM_1;
                        }
                    }
                }
                break;
        #endif
        #if ((UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2)
            case DMA_UART2_TX_CHANNEL:                                   // handle UART DMA transmission on UART 2
            #if defined KINETIS_KL && !defined KINETIS_KL43
                if (UART2_C4 & UART_C4_TDMAS)
            #elif LPUARTS_AVAILABLE > 2 && !defined LPUARTS_PARALLEL
                if (LPUART2_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #else
                if (UART2_C5 & UART_C5_TDMAS)
            #endif
                {                                                        // if DMA operation is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_UART2];
                    if (*ptrCnt) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            if (fnSimulateDMA(iChannel) > 0) {           // process the trigger
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(2);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 2 && !defined LPUARTS_PARALLEL
	                        fnLogTx1((unsigned char)LPUART2_DATA);
            #else
	                        fnLogTx2(UART2_D);
            #endif
                            ulNewActions |= SEND_COM_2;
                        }
                    }
                }
                break;
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case DMA_UART3_TX_CHANNEL:                                   // handle UART DMA transmission on UART 3
            #if LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL
                if (LPUART3_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #elif UARTS_AVAILABLE == 3 && LPUARTS_AVAILABLE == 1 && defined LPUARTS_PARALLEL
                if (LPUART0_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #else
                if (UART3_C5 & UART_C5_TDMAS)                            // if DMA operation is enabled
            #endif
                {
                    ptrCnt = (int *)argv[THROUGHPUT_UART3];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            if (fnSimulateDMA(iChannel) > 0) {           // process the trigger
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(3);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL
                            fnLogTx3((unsigned char)LPUART3_DATA);
            #elif UARTS_AVAILABLE == 3 && LPUARTS_AVAILABLE == 1 && defined LPUARTS_PARALLEL
	                        fnLogTx3((unsigned char)LPUART0_DATA);
            #else
	                        fnLogTx3(UART3_D);
            #endif
                            ulNewActions |= SEND_COM_3;
                        }
                    }
                }
                break;
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case DMA_UART4_TX_CHANNEL:                                   // handle UART DMA transmission on UART 4
            #if LPUARTS_AVAILABLE > 4 && !defined LPUARTS_PARALLEL
                if (LPUART4_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #else
                if (UART4_C5 & UART_C5_TDMAS)                            // if DMA operation is enabled
            #endif
                {
                    ptrCnt = (int *)argv[THROUGHPUT_UART4];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            if (fnSimulateDMA(iChannel) > 0) {           // process the trigger
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(4);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 4 && !defined LPUARTS_PARALLEL
                            fnLogTx4((unsigned char)LPUART4_DATA);
            #else
	                        fnLogTx4(UART4_D);
            #endif
                            ulNewActions |= SEND_COM_4;
                        }
                    }
                }
                break;
        #endif
        #if UARTS_AVAILABLE > 5
            case DMA_UART5_TX_CHANNEL:                                   // handle UART DMA transmission on UART 5
                if (UART5_C5 & UART_C5_TDMAS) {                          // if DMA operation is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_UART5];
                    if (*ptrCnt) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            if (fnSimulateDMA(iChannel) > 0) {           // process the trigger
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(5);                        // handle possible pending interrupt after DMA completion
                            }
	                        fnLogTx5(UART5_D);
                            ulNewActions |= SEND_COM_5;
                        }
                    }
                }
                break;
        #endif
    #endif
            default:
                iDMA &= ~ulChannel;
    #if !defined KINETIS_KL
                if (fnSimulateDMA(iChannel) > 1) {                      // process the trigger
                    iDMA |= ulChannel;                                  // further DMA triggers
                }
    #endif
                break;
            }
        }
        ulChannel <<= 1;
        iChannel++;
    }
    return ulNewActions;
}



extern void fnSimulateLinkUp(void)
{
#if defined ETH_INTERFACE
    #if defined PHY_INTERRUPT
    unsigned long ulBit = PHY_INTERRUPT;
    unsigned char ucPortBit = 0;
    while ((ulBit & 0x80000000) == 0) {
        ucPortBit++;
        ulBit <<= 1;
    }
    MMFR = PHY_LINK_UP_INT;
    fnSimulateInputChange(PHY_INTERRUPT_PORT, ucPortBit, CLEAR_INPUT);   // clear level sensitive interrupt input
    #endif
    fnUpdateIPConfig();                                                  // update display in simulator
#elif defined USB_CDC_RNDIS
    fnUpdateIPConfig();                                                  // update display in simulator
#endif
}

#if (defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET)
extern void fec_txf_isr(void)
{
    EIR |= (TXF | TXB);                                                  // set frame and buffer interrupt events
    if (EIMR & TXF) {                                                    // if interrupt is enabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        ptrVect->processor_interrupts.irq_ETH_TX();                      // call the interrupt handler
	}
}
#endif

#if (defined SUPPORT_RTC && !defined KINETIS_WITHOUT_RTC) || defined SUPPORT_SW_RTC

#define NTP_TO_1970_TIME 2208988800u
#define LEAP_YEAR(year) ((year%4)==0)                                    // valid from 1970 to 2038
static const unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
// Synchronise the internal RTC to the PC time when simulating
//
extern void fnInitInternalRTC(char *argv[])
{
    unsigned short *ptrShort = (unsigned short *)*argv;
    unsigned short RTC_YEAR, RTC_MONTH, RTC_DOW, RTC_DOM, RTC_HOUR, RTC_MIN, RTC_SEC;
    unsigned long ulKinetisTime;
    unsigned long ulLeapYears = 1970;
    RTC_YEAR = *ptrShort++;
    RTC_MONTH = *ptrShort++;
    RTC_DOW = *ptrShort++;
    RTC_DOM = *ptrShort++;
    RTC_HOUR = *ptrShort++;
    RTC_MIN = *ptrShort++;
    RTC_SEC = *ptrShort++;

    ulKinetisTime = ((RTC_YEAR - 1970) * 365);                           // years since reference time, represented in days without respecting leap years
    while (ulLeapYears <= RTC_YEAR) {                                    // {11}
        if (LEAP_YEAR(ulLeapYears)) {                                    // count leap years
            if (ulLeapYears == RTC_YEAR) {                               // presently in a leap year
                if ((RTC_MONTH > 2) && (RTC_DOM > 28)) {                 // {27} past February 28 so count extra leap day in this year
                    ulKinetisTime++;
                }
            }
            else {
                ulKinetisTime++;                                         // count extra days in passed leap years
            }
        }
        ulLeapYears++;
    }
    while (--RTC_MONTH != 0) {
        ulKinetisTime += monthDays[RTC_MONTH - 1];                       // {27} add past days of previous months of this year
    }
    ulKinetisTime += (RTC_DOM - 1);                                      // add past number of days in present month
    ulKinetisTime *= 24;                                                 // convert days to hours
    ulKinetisTime += RTC_HOUR;                                           // add hours passed in present day
    ulKinetisTime *= 60;                                                 // convert hours to minutes
    ulKinetisTime += RTC_MIN;                                            // add minutes in present hour
    ulKinetisTime *= 60;                                                 // convert minutes to seconds
    ulKinetisTime += RTC_SEC;                                            // add seconds in present minute
    #if !defined KINETIS_WITHOUT_RTC && !defined KINETIS_KE
        #if defined KINETIS_KL && defined RTC_USES_LPO_1kHz
    RTC_TSR = ulKinetisTime;                                             // set the initial seconds count value (since 1970)
    *RTC_SECONDS_LOCATION = ulKinetisTime;                               // set time information to non-initialised ram
    *RTC_PRESCALER_LOCATION = 0;
    *RTC_ALARM_LOCATION = 0;
    *RTC_VALID_LOCATION = RTC_VALID_PATTERN;                             // pass the time using variables
        #else
    RTC_TSR = ulKinetisTime;                                             // set the initial seconds count value (since 1970)
    RTC_SR = 0;
        #endif
    #else
    *RTC_SECONDS_LOCATION = ulKinetisTime;                               // set time information to non-initialised ram
    *RTC_PRESCALER_LOCATION = 0;
    *RTC_ALARM_LOCATION = 0;
    *RTC_VALID_LOCATION = RTC_VALID_PATTERN;
    #endif
}
#endif

#if defined SUPPORT_ADC                                                  // {2}
static void fnTriggerADC(int iADC, int iHW_trigger)
{
    switch (iADC) {
    case 0:                                                              // ADC0
        if (IS_POWERED_UP(6, SIM_SCGC6_ADC0) && ((ADC0_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF)) { // ADC0 powered up and operating
            if ((iHW_trigger != 0) || ((ADC0_SC2 & ADC_SC2_ADTRG_HW) == 0)) { // hardware or software trigger
                fnSimADC(0);                                             // perform ADC conversion
                if ((ADC0_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
                    fnHandleDMA_triggers(DMAMUX_CHCFG_SOURCE_ADC0, 0);   // handle DMA triggered on ADC0 conversion
                    if ((ADC0_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
                        if (fnGenInt(irq_ADC0_ID) != 0) {                // if ADC0 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC0();    // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #if ADC_CONTROLLERS > 1
    case 1:                                                              // ADC1
        if (IS_POWERED_UP(3, SIM_SCGC3_ADC1) && ((ADC1_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF)) { // ADC1 powered up and operating
            if ((ADC1_SC2 & ADC_SC2_ADTRG_HW) == 0) {                    // software trigger mode
                fnSimADC(1);                                             // perform ADC conversion
                if ((ADC1_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
                    fnHandleDMA_triggers(DMAMUX_CHCFG_SOURCE_ADC1, 0);   // handle DMA triggered on ADC1 conversion
                    if ((ADC1_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
                        if (fnGenInt(irq_ADC1_ID) != 0) {                // if ADC1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC1();    // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    #if ADC_CONTROLLERS > 2
    case 2:                                                              // ADC2
        if ((SIM_SCGC6 & SIM_SCGC6_ADC2) && ((ADC2_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF)) { // ADC2 powered up and operating
            if ((ADC2_SC2 & ADC_SC2_ADTRG_HW) == 0) {                    // software trigger mode
                fnSimADC(2);                                             // perform ADC conversion
                if ((ADC2_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
                    fnHandleDMA_triggers(DMAMUX1_CHCFG_SOURCE_ADC2, 1);  // handle DMA triggered on ADC2 conversion
                    if ((ADC2_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
                        if (fnGenInt(irq_ADC2_ID) != 0) {                // if ADC2 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC2();    // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    #if ADC_CONTROLLERS > 3
    case 3:                                                              // ADC3
        if ((SIM_SCGC3 & SIM_SCGC3_ADC3) && ((ADC3_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF)) { // ADC3 powered up and operating
            if ((ADC3_SC2 & ADC_SC2_ADTRG_HW) == 0) {                    // software trigger mode
                fnSimADC(3);                                             // perform ADC conversion
                if ((ADC3_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
                    fnHandleDMA_triggers(DMAMUX1_CHCFG_SOURCE_ADC3, 1);  // handle DMA triggered on ADC3 conversion
                    if ((ADC3_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
                        if (fnGenInt(irq_ADC3_ID) != 0) {                // if ADC3 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC3();    // call the interrupt handler
                        }
                    }
                }
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
#if !defined KINETIS_KL                                                  // {24}
    static int iPDB = 0;
    static int iPDB_interrupt_triggered = 0;
    static int iPDB_ch0_0_triggered = 0;
    static int iPDB_ch0_1_triggered = 0;
    static int iPDB_ch1_0_triggered = 0;
    static int iPDB_ch1_1_triggered = 0;
#endif
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;

#if defined CAN_INTERFACE && defined SIM_KOMODO
    fnSimCAN(0, 0, CAN_SIM_CHECK_RX);                                    // poll the CAN interface at the tick rate
#endif

    if (APPLICATION_INT_RESET_CTR_REG & SYSRESETREQ) {
        return RESET_SIM_CARD;                                           // commanded reset
    }
    // Watchdog
    //
#if defined KINETIS_KL && !defined KINETIS_KL82                          // {24}
    if ((SIM_COPC & SIM_COPC_COPT_LONGEST) != SIM_COPC_COPT_DISABLED) {  // check only when COP is enabled 
        if (SIM_SRVCOP == SIM_SRVCOP_2) {                                // assume retriggered
            ulCOPcounter = 0;
        }
        else {
            unsigned long ulCOP_trigger = 0;
            if ((SIM_COPC & SIM_COPC_COPCLKS_BUS) != 0) {                // COP clocked from bus clock
                ulCOPcounter += (unsigned long)(((unsigned long long)BUS_CLOCK * (unsigned long long)TICK_RESOLUTION)/1000000); // clocks in a tick period
            }
            else {                                                       // COP clocked from 1kHz clock
    #if TICK_RESOLUTION >= 1000
                ulCOPcounter += (TICK_RESOLUTION/1000);
    #else
                ulCOPcounter++;
    #endif
            }
            switch (SIM_COPC & SIM_COPC_COPT_LONGEST) {                  // COP mode
            case SIM_COPC_COPT_SHORTEST:
                if (SIM_COPC & SIM_COPC_COPCLKS_BUS) {                   // COP clocked from bus clock
                    ulCOP_trigger = (2 << (13 - 1));
                }
                else {                                                   // COP clocked from 1kHz clock
                    ulCOP_trigger = (2 << (5 - 1));
                }
                break;
            case SIM_COPC_COPT_MEDIUM:
                if (SIM_COPC & SIM_COPC_COPCLKS_BUS) {                   // COP clocked from bus clock
                    ulCOP_trigger = (2 << (16 - 1));
                }
                else {                                                   // COP clocked from 1kHz clock
                    ulCOP_trigger = (2 << (8 - 1));
                }
                break;
            case SIM_COPC_COPT_LONGEST:
                if (SIM_COPC & SIM_COPC_COPCLKS_BUS) {                   // COP clocked from bus clock
                    ulCOP_trigger = (2 << (18 - 1));
                }
                else {                                                   // COP clocked from 1kHz clock
                    ulCOP_trigger = (2 << (10 - 1));
                }
                break;
            }
            if (ulCOPcounter >= ulCOP_trigger) {
                return RESET_CARD_WATCHDOG;                              // watchdog reset
            }
        }
        SIM_SRVCOP = SIM_SRVCOP_1;                                       // reset to detect next retrigger
    }
#elif defined KINETIS_KE
    if ((WDOG_CS1 & WDOG_CS1_EN) != 0) {
    #if TICK_RESOLUTION >= 1000
        unsigned long ulCounter = (TICK_RESOLUTION/1000);                // assume 1000Hz LPO clock
    #else
        unsigned long ulCounter = 1;                                     // assume 1000Hz LPO clock
    #endif
        unsigned long ulWdogCnt = ((WDOG_CNTH << 8) | WDOG_CNTL);        // present watchdog count value
        unsigned long ulWdogTimeout = ((WDOG_TOVALH << 8) | WDOG_TOVALL);// timeout value
        ulWdogCnt += ulCounter;                                          // next value
        if (ulWdogCnt >= ulWdogTimeout) {
          //return RESET_CARD_WATCHDOG;                                  // watchdog reset
        }
        WDOG_CNTH = (unsigned char)(ulWdogCnt >> 8);                     // new watchdog count value
        WDOG_CNTL = (unsigned char)(ulWdogCnt);
    }
#else
    if (WDOG_STCTRLH & WDOG_STCTRLH_WDOGEN) {                            // watchdog enabled
    #if TICK_RESOLUTION >= 1000
        unsigned long ulCounter = (TICK_RESOLUTION/1000);                // {28} assume 1000Hz LPO clock
    #else
        unsigned long ulCounter = 1;                                     // assume 1000Hz LPO clock
    #endif
        unsigned long ulWatchdogCount = ((WDOG_TMROUTH << 16) | (WDOG_TMROUTL)); // present watchdog count
        unsigned long ulWatchdogTimeout = ((WDOG_TOVALH << 16) | (WDOG_TOVALL)); // watchdog timeout value        
        if (WDOG_STCTRLH & WDOG_STCTRLH_CLKSRC) {                        // not sure which source is which at the moment
        }
        else {
            ulCounter /= (((WDOG_PRESC >> 8) & 0x7) + 1);                // {28} respect LPO clock presecaler
        }        
        if ((ulWatchdogCount + ulCounter) >= ulWatchdogTimeout) {
            return RESET_CARD_WATCHDOG;                                  // watchdog reset
        }
        else {
            ulWatchdogCount += ulCounter;
            WDOG_TMROUTH = (unsigned short)(ulWatchdogCount >> 16);
            WDOG_TMROUTL = (unsigned short)(ulWatchdogCount);
        }
    }
#endif

    if ((SYSTICK_CSR & SYSTICK_ENABLE) != 0) {                           // SysTick is enabled
        unsigned long ulTickCount = 0;
        if ((SYSTICK_CSR & SYSTICK_CORE_CLOCK) != 0) {
            ulTickCount = (unsigned long)((unsigned long long)((unsigned long long)TICK_RESOLUTION * (unsigned long long)SYSTEM_CLOCK)/1000000); // count per tick period from internal clock
        }
        if ((SYSTICK_CURRENT + 1) > ulTickCount) {
            SYSTICK_CURRENT -= ulTickCount;
        }
        else {
            SYSTICK_CURRENT = SYSTICK_RELOAD;
            if ((SYSTICK_CSR & SYSTICK_TICKINT) != 0) {                  // if interrupt enabled
                INT_CONT_STATE_REG |= PENDSTSET;
                if ((kinetis.CORTEX_M4_REGS.ulPRIMASK & INTERRUPT_MASKED) == 0) { // if interrupt have been enabled, call interrupt handler
                    ptrVect->ptrSysTick();
                }
            }
        }
    }

#if !defined KINETIS_WITHOUT_PIT
    if ((PIT_MCR & PIT_MCR_MDIS) == 0) {                                 // if PIT module is not disabled
        if ((PIT_TCTRL0 & PIT_TCTRL_TEN) != 0) {                         // if PIT0 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL0 <= ulCount) {
                ulCount -= PIT_CVAL0;
                if (ulCount >= PIT_LDVAL0) {
                    ulCount = 0;
                }
                PIT_CVAL0 = PIT_LDVAL0;                                  // reload
                PIT_CVAL0 -= ulCount;
                PIT_TFLG0 = PIT_TFLG_TIF;                                // flag that a reload occurred
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT0, 0); // handle DMA triggered on PIT0
    #if defined KINETIS_KE
                if ((SIM_SOPT0 & SIM_SOPT_ADHWT_PIT0) != 0) {            // if PIT0 overflow is programmed to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                }
    #else
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT0:                          // if PIT0 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
    #endif
                if ((PIT_TCTRL0 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
    #if defined KINETIS_KL && !defined KINETIS_KE                        // {24}
                    if (fnGenInt(irq_PIT_ID) != 0) {                     // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT();         // call the shared interrupt handler
                    }
    #else
                    if (fnGenInt(irq_PIT0_ID) != 0) {                    // if PIT0 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT0();        // call the interrupt handler
                    }
    #endif
                }
            }
            else {
                PIT_CVAL0 -= ulCount;
            }
        }
        if ((PIT_TCTRL1 & PIT_TCTRL_TEN) != 0) {                         // if PIT1 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL1 <= ulCount) {
                ulCount -= PIT_CVAL1;
                if (ulCount >= PIT_LDVAL1) {
                    ulCount = 0;
                }
                PIT_CVAL1 = PIT_LDVAL1;                                  // reload
                PIT_CVAL1 -= ulCount;
                PIT_TFLG1 = PIT_TFLG_TIF;                                // flag that a reload occurred
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT1, 0); // handle DMA triggered on PIT1
    #if !defined KINETIS_KE
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT1:                          // if PIT1 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
    #endif
                if ((PIT_TCTRL1 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
    #if defined KINETIS_KL && !defined KINETIS_KE                        // {24}
                    if (fnGenInt(irq_PIT_ID) != 0) {                     // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT();         // call the shared interrupt handler
                    }
    #else
                    if (fnGenInt(irq_PIT1_ID) != 0) {                    // if PIT1 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT1();        // call the interrupt handler
                    }
    #endif
                }
            }
            else {
                PIT_CVAL1 -= ulCount;
            }
        }
    #if !defined KINETIS_KL && !defined KINETIS_KE                       // {24}
        if ((PIT_TCTRL2 & PIT_TCTRL_TEN) != 0) {                         // if PIT2 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL2 <= ulCount) {
                ulCount -= PIT_CVAL2;
                if (ulCount >= PIT_LDVAL2) {
                    ulCount = 0;
                }
                PIT_CVAL2 = PIT_LDVAL2;                                  // reload
                PIT_CVAL2 -= ulCount;
                PIT_TFLG2 = PIT_TFLG_TIF;                                // flag that a reload occurred
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT2, 0); // handle DMA triggered on PIT2
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT2:                          // if PIT2 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
                if ((PIT_TCTRL2 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
                    if (fnGenInt(irq_PIT2_ID) != 0) {                    // if PIT2 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT2();        // call the interrupt handler
                    }
                }
            }
            else {
                PIT_CVAL2 -= ulCount;
            }
        }
        if ((PIT_TCTRL3 & PIT_TCTRL_TEN) != 0) {                         // if PIT3 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL3 <= ulCount) {
                ulCount -= PIT_CVAL3;
                if (ulCount >= PIT_LDVAL3) {
                    ulCount = 0;
                }
                PIT_CVAL3 = PIT_LDVAL3;                                  // reload
                PIT_CVAL3 -= ulCount;
                PIT_TFLG3 = PIT_TFLG_TIF;                                // flag that a reload occurred
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT3, 0); // handle DMA triggered on PIT3
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT3:                          // if PIT3 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
                if ((PIT_TCTRL3 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
                    if (fnGenInt(irq_PIT3_ID) != 0) {                    // if PIT3 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT3();        // call the interrupt handler
                    }
                }
            }
            else {
                PIT_CVAL3 -= ulCount;
            }
        }
    #endif
    }
#endif
#if !defined KINETIS_WITHOUT_RTC && !defined KINETIS_KE
    if ((RTC_SR & RTC_SR_TCE) != 0) {                                    // RTC is enabled
        if ((RTC_SR & RTC_SR_TIF) == 0) {                                // if invalid flag not set
            unsigned long ulCounter;
            switch (SIM_SOPT1 & SIM_SOPT1_OSC32KSEL_MASK) {
            case SIM_SOPT1_OSC32KSEL_SYS_OSC:
                ulCounter = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)32000)/1000000); // approximately 32kHz clock pulses in a TICK period
                break;
            case SIM_SOPT1_OSC32KSEL_LPO_1kHz:
    #if TICK_RESOLUTION >= 1000
                ulCounter = ((TICK_RESOLUTION/1000));                    // approximately 1kHz clock pulses in a TICK period
    #else
                ulCounter = 1;
    #endif
                break;
    #if defined KINETIS_KL
            case SIM_SOPT1_OSC32KSEL_RTC_CLKIN:                          // 32kHz clock input assumed
    #else
            case SIM_SOPT1_OSC32KSEL_32k:
    #endif
                ulCounter = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)32768)/1000000); // 32kHz clock pulses in a TICK period
                break;
            }
            RTC_TPR += ulCounter;
            if (RTC_TPR >= 32768) {
                RTC_TPR = (RTC_TPR - 32768);                             // handle second overflow
    #if defined irq_RTC_SECONDS_ID                                       // {30} if the RTC in the device has seconds interrupt capability
                if ((RTC_IER & RTC_IER_TSIE) != 0) {                     // if seconds interrupt is enabled
                    if (fnGenInt(irq_RTC_SECONDS_ID) != 0) {             // if RTC seconds interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_RTC_SECONDS(); // call the interrupt handler
                    }
                }
    #endif
                if (RTC_TAR == RTC_TSR) {                                // alarm match
                    RTC_TSR = (RTC_TSR + 1);
                    RTC_SR |= RTC_SR_TAF;
                    if (RTC_IER & RTC_IER_TAIE) {                        // interrupt on alarm enabled
                        if (fnGenInt(irq_RTC_ALARM_ID) != 0) {           // if RTC interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_RTC_ALARM(); // call the interrupt handler
                        }
                    }
                }
                else {
                    RTC_TSR = (RTC_TSR + 1);
                }
            }
        }
    }
#endif
#if defined KINETIS_KE
    if ((SIM_SCGC & SIM_SCGC_RTC) != 0) {
        unsigned long ulCount = 0;
        switch (RTC_SC & RTC_SC_RTCLKS_BUS) {                            // RTC clock source
        case RTC_SC_RTCLKS_EXT:                                          // external clock
    #if !defined _EXTERNAL_CLOCK                                         // if no external clock is available the OSCERCLK is not valid
        #define _EXTERNAL_CLOCK 0
    #endif
            ulCount = (unsigned long)((((unsigned long long)TICK_RESOLUTION) * (unsigned long long)_EXTERNAL_CLOCK)/1000000); // prescaler count in tick interval
            switch (RTC_SC & RTC_SC_RTCPS_1000) {
            case RTC_SC_RTCPS_1:
                break;
            case RTC_SC_RTCPS_2:
                ulCount /= 2;
                break;
            case RTC_SC_RTCPS_4:
                ulCount /= 4;
                break;
            case RTC_SC_RTCPS_8:
                ulCount /= 8;
                break;
            case RTC_SC_RTCPS_16:
                ulCount /= 16;
                break;
            case RTC_SC_RTCPS_32:
                ulCount /= 32;
                break;
            case RTC_SC_RTCPS_64:
                ulCount /= 64;
                break;
            }
            break;
        case RTC_SC_RTCLKS_INT:                                          // internal 32kHz clock
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)ICSIRCLK)/1000000); // count value in a tick period
            switch (RTC_SC & RTC_SC_RTCPS_1000) {                        // apply prescaler
            case RTC_SC_RTCPS_1:
                break;
            case RTC_SC_RTCPS_2:
                ulCount /= 2;
                break;
            case RTC_SC_RTCPS_4:
                ulCount /= 4;
                break;
            case RTC_SC_RTCPS_8:
                ulCount /= 8;
                break;
            case RTC_SC_RTCPS_16:
                ulCount /= 16;
                break;
            case RTC_SC_RTCPS_32:
                ulCount /= 32;
                break;
            case RTC_SC_RTCPS_64:
                ulCount /= 64;
                break;
            }
            break;
        case RTC_SC_RTCLKS_1K:                                           // internal 1kHz clock
    #if TICK_RESOLUTION >= 1000
            ulCount = (TICK_RESOLUTION/1000);
    #else
            ulCount = 1;
    #endif
            switch (RTC_SC & RTC_SC_RTCPS_1000) {
            case RTC_SC_RTCPS_128:
                ulCount /= 128;
                break;
            case RTC_SC_RTCPS_256:
                ulCount /= 256;
                break;
            case RTC_SC_RTCPS_512:
                ulCount /= 512;
                break;
            case RTC_SC_RTCPS_1024:
                ulCount /= 1024;
                break;
            case RTC_SC_RTCPS_2048:
                ulCount /= 2048;
                break;
            case RTC_SC_RTCPS_100:
                ulCount /= 100;
                break;
            case RTC_SC_RTCPS_1000:
                ulCount /= 1000;
                break;
            }
            break;
        case RTC_SC_RTCLKS_BUS:                                          // bus clock
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000);
            switch (RTC_SC & RTC_SC_RTCPS_1000) {
            case RTC_SC_RTCPS_128:
                ulCount /= 128;
                break;
            case RTC_SC_RTCPS_256:
                ulCount /= 256;
                break;
            case RTC_SC_RTCPS_512:
                ulCount /= 512;
                break;
            case RTC_SC_RTCPS_1024:
                ulCount /= 1024;
                break;
            case RTC_SC_RTCPS_2048:
                ulCount /= 2048;
                break;
            case RTC_SC_RTCPS_100:
                ulCount /= 100;
                break;
            case RTC_SC_RTCPS_1000:
                ulCount /= 1000;
                break;
            }
            break;
        }
        ulCount += RTC_CNT;
        if (ulCount > RTC_MOD) {
            ulCount -= RTC_MOD;
            RTC_CNT = ulCount;
            if ((RTC_SC & RTC_SC_RTIE) != 0) {                           // if interrupt enabled
                if (fnGenInt(irq_RTC_OVERFLOW_ID) != 0) {                // if core interrupt interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_RTC_OVERFLOW();    // call the interrupt handler
                }
            }
        }
        else {
            RTC_CNT = ulCount;
        }
    }
#elif defined SUPPORT_LPTMR                                              // {35}
    if (((SIM_SCGC5 & SIM_SCGC5_LPTIMER) != 0) && ((LPTMR0_CSR & LPTMR_CSR_TEN) != 0)) { // if the low power timer is enabled and running
        unsigned long ulCount = 0;                                       // count in a tick period
        switch (LPTMR0_PSR & LPTMR_PSR_PCS_OSC0ERCLK) {
        case LPTMR_PSR_PCS_LPO:
    #if TICK_RESOLUTION >= 1000
            ulCount = (TICK_RESOLUTION/1000);
    #else
            ulCount = 1;
    #endif
            break;
        case LPTMR_PSR_PCS_MCGIRCLK:
            if ((MCG_C2 & MCG_C2_IRCS) != 0) {
                ulCount = (TICK_RESOLUTION * (4000000/1000000));
            }
            else {
                ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)35000)/1000000);
            }
            break;
        case LPTMR_PSR_PCS_ERCLK32K:
            ulCount = (TICK_RESOLUTION * 32768);
            break;
        case LPTMR_PSR_PCS_OSC0ERCLK:
    #if defined _EXTERNAL_CLOCK
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)_EXTERNAL_CLOCK)/1000000); // external clocks in a tick period (assuming no pre-scaler)
    #else
            _EXCEPTION("no external clock defined so this selection should not be used");
    #endif
            break;
        }
        if ((LPTMR0_PSR & LPTMR_PSR_PBYP) == 0) {                        // if the prescaler bypass hasn't been disabled
            ulCount >>= (((LPTMR0_PSR & LPTMR_PSR_PRESCALE_MASK) >> LPTMR_PSR_PRESCALE_SHIFT) + 1);
        }
        if (LPTMR0_CNR <= LPTMR0_CMR) {                                  // timer count has not yet reached the match value
            ulCount = (LPTMR0_CNR + ulCount);                            // the next count value
            if (ulCount > LPTMR0_CMR) {
                if ((LPTMR0_CSR & LPTMR_CSR_TFC_FREERUN) == 0) {
                    ulCount = (ulCount - LPTMR0_CMR);
                    if (ulCount > LPTMR0_CMR) {
                        ulCount = LPTMR0_CMR;
                    }
                }
                if ((LPTMR0_CSR & LPTMR_CSR_TIE) != 0) {                 // if LPTMR interrupt is enabled
                    if (fnGenInt(irq_LPT_ID) != 0) {                     // if LPTMR interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPT();         // call the interrupt handler
                    }
                }
            }
        }
        else {
            ulCount = (LPTMR0_CNR + ulCount);
        }
        if (ulCount > 0xffff) {
            ulCount = (ulCount - 0xffff);
        }
        LPTMR0_CNR = ulCount;
    }
#endif
#if !defined KINETIS_KL && !defined KINETIS_KE                           // {24}
    if (((SIM_SCGC6 & SIM_SCGC6_PDB) != 0) && ((PDB0_SC & PDB_SC_PDBEN) != 0)) { // {16} PDB powered and enabled
        if ((PDB0_SC & PDB_SC_TRGSEL_SW) == PDB_SC_TRGSEL_SW) {          // software triggered
            if ((PDB0_SC & PDB_SC_SWTRIG) != 0) {
                PDB0_SC &= ~(PDB_SC_SWTRIG);                             // clear the software trigger
                iPDB = 1;                                                // triggered and running
                PDB0_CNT = 0;
            }
        }
        if (iPDB != 0) {                                                 // if running
            unsigned long ulPDB_count = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000);
            switch (PDB0_SC & PDB_SC_MULT_40) {
            case PDB_SC_MULT_1:
                break;
            case PDB_SC_MULT_10:
                ulPDB_count /= 10;
                break;
            case PDB_SC_MULT_20:
                ulPDB_count /= 20;
                break;
            case PDB_SC_MULT_40:
                ulPDB_count /= 40;
                break;
            }
            switch (PDB0_SC & PDB_SC_PRESCALER_128) {
            case PDB_SC_PRESCALER_1:
                break;
            case PDB_SC_PRESCALER_2:
                ulPDB_count /= 2;
                break;
            case PDB_SC_PRESCALER_4:
                ulPDB_count /= 4;
                break;
            case PDB_SC_PRESCALER_8:
                ulPDB_count /= 8;
                break;
            case PDB_SC_PRESCALER_16:
                ulPDB_count /= 16;
                break;
            case PDB_SC_PRESCALER_32:
                ulPDB_count /= 32;
                break;
            case PDB_SC_PRESCALER_64:
                ulPDB_count /= 64;
                break;
            case PDB_SC_PRESCALER_128:
                ulPDB_count /= 128;
                break;
            }
            ulPDB_count += PDB0_CNT;                                     // new count value
            if ((iPDB_interrupt_triggered == 0) && (ulPDB_count >= (PDB0_IDLY & 0xffff))) { // interrupt trigger reached
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                iPDB_interrupt_triggered = 1;
                if ((PDB0_SC & PDB_SC_PDBIE) != 0) {                     // if interrupt is enabled
                    PDB0_SC |= PDB_SC_PDBIF;                             // set the interrupt flag
                    if (fnGenInt(irq_PDB_ID) != 0) {
                        ptrVect->processor_interrupts.irq_PDB();         // call the interrupt handler
                    }
                }
            }
            if ((iPDB_ch0_0_triggered == 0) && (ulPDB_count >= (PDB0_CH0DLY0 & 0xffff))) { // channel 0 delay 0 trigger reached
                iPDB_ch0_0_triggered = 1;
                if ((PDB0_CH0C1 & PDB_C1_EN_0) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH0C1 & PDB_C1_TOS_0) != 0) {              // if ADC0 trigger is enabled
                    }
                }
            }
            if ((iPDB_ch0_1_triggered == 0) && (ulPDB_count >= (PDB0_CH0DLY1 & 0xffff))) { // channel 0 delay 1 trigger reached
                iPDB_ch0_1_triggered = 1;
                if ((PDB0_CH0C1 & PDB_C1_EN_1) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH0C1 & PDB_C1_TOS_1) != 0) {              // if ADC1 trigger is enabled
                    }
                }
            }
    #if ADC_CONTROLLERS > 1
            if ((iPDB_ch1_0_triggered == 0) && (ulPDB_count >= (PDB0_CH1DLY0 & 0xffff))) { // channel 1 delay 0 trigger reached
                iPDB_ch1_0_triggered = 1;
                if ((PDB0_CH1C1 & PDB_C1_EN_0) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH1C1 & PDB_C1_TOS_1) != 0) {              // if ADC1 trigger is enabled
                    }
                }
            }
            if ((iPDB_ch1_1_triggered == 0) && (ulPDB_count >= (PDB0_CH1DLY1 & 0xffff))) { // channel 1 delay 1 trigger reached
                iPDB_ch1_1_triggered = 1;
                if ((PDB0_CH1C1 & PDB_C1_EN_1) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH1C1 & PDB_C1_TOS_1) != 0) {              // if ADC1 trigger is enabled
                    }
                }
            }
    #endif
            
            if (ulPDB_count >= (PDB0_MOD & 0xffff)) {                    // cycle complete
                iPDB_interrupt_triggered = 0;
                iPDB_ch0_0_triggered = 0;
                iPDB_ch0_1_triggered = 0;
                iPDB_ch1_0_triggered = 0;
                iPDB_ch1_1_triggered = 0;
                PDB0_CNT = 0;
                if ((PDB0_SC & PDB_SC_CONT) == 0) {                      // single shot
                    iPDB = 0;
                }
            }
            else {
                PDB0_CNT = ulPDB_count;
            }
        }
    }
    else {
        iPDB = 0;
    }
#endif

#if defined SUPPORT_ADC                                                  // {2}
    fnTriggerADC(0, 0);                                                  // handle software triggered ADC0
    #if ADC_CONTROLLERS > 1
    fnTriggerADC(1, 0);                                                  // handle software triggered ADC1
    #endif
    #if ADC_CONTROLLERS > 2
    fnTriggerADC(2, 0);                                                  // handle software triggered ADC2
    #endif
    #if ADC_CONTROLLERS > 3
    fnTriggerADC(3, 0);                                                  // handle software triggered ADC3
    #endif
#endif
#if defined SUPPORT_TIMER                                                // {29}
    if (IS_POWERED_UP(6, SIM_SCGC6_FTM0) &&((FTM0_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the FlexTimer is powered and clocked
    #if defined KINETIS_KL
        unsigned long ulCountIncrease;
        switch (SIM_SOPT2 & SIM_SOPT2_TPMSRC_MCGIRCLK) {                 // {38}
        case SIM_SOPT2_TPMSRC_MCGIRCLK:
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGIRCLK)/1000000); // bus clocks in a period
            if ((MCG_C2 & MCG_C2_IRCS) != 0) {                           // if fast clock
                int prescaler = ((MCG_SC >> 1) & 0x7);                   // FCRDIV value
                while (prescaler-- != 0) {
                    ulCountIncrease /= 2;                                // FCRDIV prescale
                }
            }
            break;
        case SIM_SOPT2_TPMSRC_OSCERCLK:
        #if defined OSCERCLK
            ulCountIncrease= (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)OSCERCLK)/1000000); // bus clocks in a period
        #else
            _EXCEPTION("No OSCERCLK available");
        #endif
            break;
        case SIM_SOPT2_TPMSRC_MCG:
        #if defined FLL_FACTOR
            ulCountIncrease= (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGFLLCLK)/1000000); // bus clocks in a period
        #else
            ulCountIncrease= (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)(MCGPLLCLK/2))/1000000); // bus clocks in a period
        #endif
            break;
        }
    #elif defined KINETIS_KE
        unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
    #else
        unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
    #endif
        ulCountIncrease /= (1 << (FTM0_SC & FTM_SC_PS_128));             // apply pre-scaler
        ulCountIncrease += FTM0_CNT;                                     // new counter value (assume up counting)
        if (ulCountIncrease >= FTM0_MOD) {                               // match/overflow
    #if defined KINETIS_KL || defined KINETIS_KE
            FTM0_SC |= FTM_SC_TOF;                                       // set overflow flag
            ulCountIncrease -= FTM0_MOD;
    #else
            FTM0_CNT = FTM0_CNTIN;
            FTM0_SC |= FTM_SC_TOF;                                       // set overflow flag
            if (ulCountIncrease > (0xffff + (FTM0_MOD - FTM0_CNTIN))) {
                ulCountIncrease = (0xffff + (FTM0_MOD - FTM0_CNTIN));
            }
            while (ulCountIncrease >= FTM0_MOD) {
                ulCountIncrease -= (FTM0_MOD - FTM0_CNTIN);
            }
    #endif
        }
        FTM0_CNT = ulCountIncrease;                                      // new counter value
        if (((FTM0_SC & FTM_SC_TOIE) != 0) && ((FTM0_SC & FTM_SC_TOF) != 0)) { // if overflow occurred and interrupt enabled
    #if defined KINETIS_KL
            if (fnGenInt(irq_TPM0_ID) != 0) {                            // if timer/PWM module 0 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_TPM0();                // call the interrupt handler
            }
    #else
            if (fnGenInt(irq_FTM0_ID) != 0) {                            // if FlexTimer 0 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_FTM0();                // call the interrupt handler
            }
    #endif
        }
    }
    #if FLEX_TIMERS_AVAILABLE > 1
    if (IS_POWERED_UP(6, SIM_SCGC6_FTM1) && ((FTM1_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the FlexTimer is powered and clocked
        #if defined KINETIS_KL
        unsigned long ulCountIncrease;
        switch (SIM_SOPT2 & SIM_SOPT2_TPMSRC_MCGIRCLK) {                 // {38}
        case SIM_SOPT2_TPMSRC_MCGIRCLK:
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGIRCLK)/1000000); // bus clocks in a period
            if (MCG_C2 & MCG_C2_IRCS) {                                  // if fast clock
                int prescaler = ((MCG_SC >> 1) & 0x7);                   // FCRDIV value
                while (prescaler--) {
                    ulCountIncrease /= 2;                                // FCRDIV prescale
                }
            }
            break;
        case SIM_SOPT2_TPMSRC_OSCERCLK:
        #if defined OSCERCLK
            ulCountIncrease = (unsigned long)((unsigned long long)TICK_RESOLUTION * (unsigned long long)OSCERCLK)/1000000; // bus clocks in a period
        #else
            _EXCEPTION("No OSCERCLK available");
        #endif
            break;
        case SIM_SOPT2_TPMSRC_MCG:
        #if defined FLL_FACTOR
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGFLLCLK)/1000000); // bus clocks in a period
        #else
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)(MCGPLLCLK/2))/1000000); // bus clocks in a period
        #endif
            break;
        }
        #elif defined KINETIS_KE
        unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
        #else
        unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
        #endif
        ulCountIncrease /= (1 << (FTM1_SC & FTM_SC_PS_128));             // apply pre-scaler
        ulCountIncrease += FTM1_CNT;                                     // new counter value (assume up counting)
        if (ulCountIncrease >= FTM1_MOD) {                               // match
        #if defined KINETIS_KL || defined KINETIS_KE
            FTM1_SC |= FTM_SC_TOF;                                       // set overflow flag
            ulCountIncrease -= FTM1_MOD;
        #else
            FTM1_CNT = FTM1_CNTIN;
            FTM1_SC |= FTM_SC_TOF;                                       // set overflow flag
            if (ulCountIncrease > (0xffff + (FTM1_MOD - FTM1_CNTIN))) {
                ulCountIncrease = (0xffff + (FTM1_MOD - FTM1_CNTIN));
            }
            while (ulCountIncrease >= FTM1_MOD) {
                ulCountIncrease -= (FTM1_MOD - FTM1_CNTIN);
            }
        #endif
        }
        FTM1_CNT = ulCountIncrease;                                      // new counter value
        if (((FTM1_SC & FTM_SC_TOIE) != 0) && ((FTM1_SC & FTM_SC_TOF) != 0)) { // if overflow occurred and interrupt enabled
        #if defined KINETIS_KL
            if (fnGenInt(irq_TPM1_ID) != 0) {                            // if timer/PWM module 1 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_TPM1();                // call the interrupt handler
            }
        #else
            if (fnGenInt(irq_FTM1_ID) != 0) {                            // if FlexTimer 1 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_FTM1();                // call the interrupt handler
            }
        #endif
        }
        #if defined KINETIS_KL
        // Check for ADC triggers
        //
        if ((SIM_SOPT7 & SIM_SOPT7_ADC0ALTTRGEN) == 0) {                 // if the default hardware trigger source is used
            if (FTM1_CNT >= FTM0_C0V) {                                  // TPM1 channel 0 can trigger ADC0 input A
                fnTriggerADC(0, 1);
            }
            if (FTM1_CNT >= FTM0_C1V) {                                  // TPM1 channel 1 can trigger ADC0 input B
                fnTriggerADC(0, 1);
            }
        }
        #endif
    }
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
        #if defined KINETIS_KL
    if ((SIM_SCGC6 & SIM_SCGC6_FTM2) &&((FTM2_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0))  // if the timer/PWM module is powered and clocked
        #else
    if (IS_POWERED_UP(3, SIM_SCGC3_FTM2) &&((FTM2_SC & FTM_SC_CLKS_EXT) != 0))  // if the FlexTimer is powered and clocked
        #endif
    {
        #if defined KINETIS_KL
        unsigned long ulCountIncrease;
        switch (SIM_SOPT2 & SIM_SOPT2_TPMSRC_MCGIRCLK) {                 // {38}
        case SIM_SOPT2_TPMSRC_MCGIRCLK:
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGIRCLK)/1000000); // bus clocks in a period
            if ((MCG_C2 & MCG_C2_IRCS) != 0) {                           // if fast clock
                int prescaler = ((MCG_SC >> 1) & 0x7);                   // FCRDIV value
                while (prescaler-- != 0) {
                    ulCountIncrease /= 2;                                // FCRDIV prescale
                }
            }
            break;
        case SIM_SOPT2_TPMSRC_OSCERCLK:
            #if defined OSCERCLK
            ulCountIncrease= (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)OSCERCLK)/1000000); // bus clocks in a period
            #else
            _EXCEPTION("No OSCERCLK available!!");
            #endif
            break;
        case SIM_SOPT2_TPMSRC_MCG:
        #if defined FLL_FACTOR
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGFLLCLK)/1000000); // bus clocks in a period
        #else
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)(MCGPLLCLK/2))/1000000); // bus clocks in a period
        #endif
            break;
        }
        #elif defined KINETIS_KE
        unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
        #else
        unsigned long ulCountIncrease = (unsigned long long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
        #endif
        ulCountIncrease /= (1 << (FTM2_SC & FTM_SC_PS_128));             // apply pre-scaler
        ulCountIncrease += FTM2_CNT;                                     // new counter value (assume up counting)
        if (ulCountIncrease >= FTM2_MOD) {                               // match
        #if defined KINETIS_KL || defined KINETIS_KE
            FTM2_SC |= FTM_SC_TOF;                                       // set overflow flag
            ulCountIncrease -= FTM2_MOD;
        #else
            FTM2_CNT = FTM2_CNTIN;
            FTM2_SC |= FTM_SC_TOF;                                       // set overflow flag
            if (ulCountIncrease > (0xffff + (FTM2_MOD - FTM2_CNTIN))) {
                ulCountIncrease = (0xffff + (FTM2_MOD - FTM2_CNTIN));
            }
            while (ulCountIncrease >= FTM2_MOD) {
                ulCountIncrease -= (FTM2_MOD - FTM2_CNTIN);
            }
        #endif
        }
        FTM2_CNT = ulCountIncrease;                                      // new counter value
        if ((FTM2_SC & FTM_SC_TOIE) && (FTM2_SC & FTM_SC_TOF)) {         // if overflow occurred and interrupt enabled
        #if defined KINETIS_KL
            if (fnGenInt(irq_TPM2_ID) != 0) {                            // if timer/PWM module 2 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_TPM2();                // call the interrupt handler
            }
        #else
            if (fnGenInt(irq_FTM2_ID) != 0) {                            // if FlexTimer 2 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_FTM2();                // call the interrupt handler
            }
        #endif
        }
    }
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    if (((SIM_SCGC3 & SIM_SCGC3_FTM3) != 0) && ((FTM3_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the FlexTimer is powered and clocked
        #if defined KINETIS_KL
        unsigned long ulCountIncrease;
        switch ((SIM_SOPT2 & SIM_SOPT2_TPMSRC_MCGIRCLK) != 0) {          // {38}
        case SIM_SOPT2_TPMSRC_MCGIRCLK:
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGIRCLK)/1000000); // bus clocks in a period
            if (MCG_C2 & MCG_C2_IRCS) {                                  // if fast clock
                int prescaler = ((MCG_SC >> 1) & 0x7);                   // FCRDIV value
                while (prescaler--) {
                    ulCountIncrease /= 2;                                // FCRDIV prescale
                }
            }
            break;
        case SIM_SOPT2_TPMSRC_OSCERCLK:
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)OSCERCLK)/1000000); // bus clocks in a period
            break;
        case SIM_SOPT2_TPMSRC_MCG:
        #if defined FLL_FACTOR
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGFLLCLK)/1000000); // bus clocks in a period
        #else
            ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)(MCGPLLCLK/2))/1000000); // bus clocks in a period
        #endif
            break;
        }
        #elif defined KINETIS_KE
        unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
        #else
        unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // bus clocks in a period (assume clocked from bus clock)
        #endif
        ulCountIncrease /= (1 << (FTM3_SC & FTM_SC_PS_128));             // apply pre-scaler
        ulCountIncrease += FTM3_CNT;                                     // new counter value (assume up counting)
        if (ulCountIncrease >= FTM3_MOD) {                               // match
        #if defined KINETIS_KL || defined KINETIS_KE
            FTM3_SC |= FTM_SC_TOF;                                       // set overflow flag
            ulCountIncrease -= FTM3_MOD;
        #else
            FTM3_CNT = FTM3_CNTIN;
            FTM3_SC |= FTM_SC_TOF;                                       // set overflow flag
            if (ulCountIncrease > (0xffff + (FTM3_MOD - FTM3_CNTIN))) {
                ulCountIncrease = (0xffff + (FTM3_MOD - FTM3_CNTIN));
            }
            while (ulCountIncrease >= FTM3_MOD) {
                ulCountIncrease -= (FTM3_MOD - FTM3_CNTIN);
            }
        #endif
        }
        FTM3_CNT = ulCountIncrease;                                      // new counter value
        if (((FTM3_SC & FTM_SC_TOIE) != 0) && ((FTM3_SC & FTM_SC_TOF) != 0)) { // if overflow occurred and interrupt enabled
            if (fnGenInt(irq_FTM3_ID) != 0) {                            // if FlexTimer 2 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_FTM3();                // call the interrupt handler
            }
        }
    }
    #endif
#endif
    return 0;
}


extern unsigned char *fnGetSimTxBufferAdd(void)
{
#if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET
    return (fnGetTxBufferAdd(0));
#else
    return 0;
#endif
}


#if defined ETH_INTERFACE
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
#endif

#if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET // we feed frames in promiscuous mode and filter them according to their details
static KINETIS_FEC_BD *ptrSimRxBD = 0;
static int iFirstRec = 0;

extern void fnResetENET(void)                                            // {41}
{
    iFirstRec = 0;
}

static int fnCheckEthernet(unsigned char *ucData, unsigned short usLen, int iForce)
{
	unsigned char *ptrInput;
    unsigned short usCopyLen;
    unsigned short usFullLength;
    #if defined USE_MULTIPLE_BUFFERS
    unsigned short ucMaxRxBufLen = 256;
    #else
    unsigned short ucMaxRxBufLen = 0x600;
    #endif

    if ((iForce == 0) && (fnCheckEthernetMode(ucData, usLen) == 0)) {
        return 0;                                                        // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
    }

    if (iFirstRec == 0) {
        if ((ECR & ETHER_EN) == 0) {
            return 0;                                                    // ignore if the FEC has not yet been programmed
        }
        iFirstRec = 1;                                                   // we do this only once
        ptrSimRxBD = (KINETIS_FEC_BD *)ERDSR;                            // set to first BD
    }

    if ((ptrSimRxBD->usBDControl & EMPTY_BUFFER) == 0) {                 // {22} drop reception frame if there are no free buffer descriptors
        return 0;
    }

    usFullLength = usLen;

    while (usLen != 0) {
        ptrInput = ptrSimRxBD->ptrBD_Data;
        if (usLen > ucMaxRxBufLen) {
            ptrSimRxBD->usBDLength = ucMaxRxBufLen;
            usCopyLen = ucMaxRxBufLen;
        }
        else {
            usCopyLen = ptrSimRxBD->usBDLength = usLen;
            ptrSimRxBD->usBDLength = usFullLength;
            ptrSimRxBD->usBDControl |= LAST_IN_FRAME_RX;
        }
        usLen -= usCopyLen;
	    while (usCopyLen-- != 0) {
	        *ptrInput++ = *ucData++;                                     // put bytes in input buffer
	    }
        if (usLen == 0) {                                                // last buffer
            if ((RCR & CRCFWD) == 0) {                                   // if CRC stripping is not disabled
                ptrSimRxBD->usBDLength += 4;                             // add dummy CRC32 - this corrects the receive count and also tests that there is adequate buffer space
            }
            *ptrInput++ = 0x33;
            *ptrInput++ = 0x66;
            *ptrInput++ = 0xaa;
            *ptrInput   = 0x55;
        }
        ptrSimRxBD->usBDControl &= ~EMPTY_BUFFER;                        // mark that the buffer contains
        EIR |= RXF;                                                      // set receive frame interrupt event
        if (((EIMR & RXF) != 0) && (ptrSimRxBD->usBDControl & LAST_IN_FRAME_RX)) { // if interrupts are enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_ETH_RX();                  // call the interrupt handler
	    }
        if ((ptrSimRxBD->usBDControl & WRAP_BIT_RX) != 0) {
            ptrSimRxBD = (KINETIS_FEC_BD *)ERDSR;                        // set to first BD
        }
        else {
            ptrSimRxBD++;                                                // set to next RX BD
        }
    }
    return 1;
}
#endif




extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce)
{
    int iReturn = 1;
#if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET // we feed frames in promiscuous mode and filter them according to their details
    iReturn = fnCheckEthernet(ucData, usLen, iForce);
#endif
#if defined ETH_INTERFACE && defined ENC424J600_INTERFACE
    iReturn &= ~(fnCheckENC424J600(ucData, usLen, iForce));
#endif
    return iReturn;
}


#if defined SUPPORT_KEY_SCAN

#if KEY_COLUMNS == 0
    #define _NON_MATRIX
    #undef KEY_COLUMNS
    #define KEY_COLUMNS  VIRTUAL_KEY_COLUMNS
    #undef KEY_ROWS
    #define KEY_ROWS VIRTUAL_KEY_ROWS
#endif


int iKeyPadInputs[KEY_COLUMNS][KEY_ROWS] = {0};

extern void fnSimulateKeyChange(int *intTable)
{
#if defined _NON_MATRIX
    int iCol, iRow;
    int iChange;
    for (iCol = 0; iCol < KEY_COLUMNS; iCol++) {
        for (iRow = 0; iRow < KEY_ROWS; iRow++) {
            iChange = iKeyPadInputs[iCol][iRow];                         // original value
            iKeyPadInputs[iCol][iRow] = *intTable++;                     // new value
            if (iChange != iKeyPadInputs[iCol][iRow]) {
    #if defined KEY_POLARITY_POSITIVE
                if (iChange)
    #else
                if (iChange != 0)                                        // generally a key press is a '0' 
    #endif
                {
                    iChange = SET_INPUT;
                }
                else {
                    iChange = CLEAR_INPUT;
                }
                switch ((iCol * KEY_ROWS) + iRow) {                      // the specific input
    #if defined KEY_1_PORT_REF
                case 0:                                                  // first key
                    fnSimulateInputChange(KEY_1_PORT_REF, fnMapPortBit(KEY_1_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_2_PORT_REF
                case 1:                                                  // second key
                    fnSimulateInputChange(KEY_2_PORT_REF, fnMapPortBit(KEY_2_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_3_PORT_REF
                case 2:                                                  // third key
                    fnSimulateInputChange(KEY_3_PORT_REF, fnMapPortBit(KEY_3_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_4_PORT_REF
                case 3:                                                  // fourth key
                    fnSimulateInputChange(KEY_4_PORT_REF, fnMapPortBit(KEY_4_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_5_PORT_REF
                case 4:                                                  // fifth key
                    fnSimulateInputChange(KEY_5_PORT_REF, fnMapPortBit(KEY_5_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_6_PORT_REF
                case 5:                                                  // sixth key
                    fnSimulateInputChange(KEY_6_PORT_REF, fnMapPortBit(KEY_6_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_7_PORT_REF
                case 6:                                                  // seventh key
                    fnSimulateInputChange(KEY_7_PORT_REF, fnMapPortBit(KEY_7_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_8_PORT_REF
                case 7:                                                  // eighth key
                    fnSimulateInputChange(KEY_8_PORT_REF, fnMapPortBit(KEY_8_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_9_PORT_REF
                case 8:                                                  // ninth key
                    fnSimulateInputChange(KEY_9_PORT_REF, fnMapPortBit(KEY_9_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_10_PORT_REF
                case 9:                                                  // tenth key
                    fnSimulateInputChange(KEY_10_PORT_REF, fnMapPortBit(KEY_10_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_11_PORT_REF
                case 10:                                                 // eleventh key
                    fnSimulateInputChange(KEY_11_PORT_REF, fnMapPortBit(KEY_11_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_12_PORT_REF
                case 11:                                                 // twelf key
                    fnSimulateInputChange(KEY_12_PORT_REF, fnMapPortBit(KEY_12_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_13_PORT_REF
                case 12:                                                 // thirteenth key
                    fnSimulateInputChange(KEY_13_PORT_REF, fnMapPortBit(KEY_13_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_14_PORT_REF
                case 13:                                                 // fourteenth key
                    fnSimulateInputChange(KEY_14_PORT_REF, fnMapPortBit(KEY_14_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_15_PORT_REF
                case 14:                                                 // fifteenth key
                    fnSimulateInputChange(KEY_15_PORT_REF, fnMapPortBit(KEY_15_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_16_PORT_REF
                case 15:                                                 // sixteenth key
                    fnSimulateInputChange(KEY_16_PORT_REF, fnMapPortBit(KEY_16_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_17_PORT_REF
                case 16:                                                 // seventeenth key
                    fnSimulateInputChange(KEY_17_PORT_REF, fnMapPortBit(KEY_17_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_18_PORT_REF
                case 17:                                                 // eighteenth key
                    fnSimulateInputChange(KEY_18_PORT_REF, fnMapPortBit(KEY_18_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_19_PORT_REF
                case 18:                                                 // nineteenth key
                    fnSimulateInputChange(KEY_19_PORT_REF, fnMapPortBit(KEY_19_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_20_PORT_REF
                case 19:                                                 // twentieth key
                    fnSimulateInputChange(KEY_20_PORT_REF, fnMapPortBit(KEY_20_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_21_PORT_REF
                case 20:                                                 // twenty first key
                    fnSimulateInputChange(KEY_21_PORT_REF, fnMapPortBit(KEY_21_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_22_PORT_REF
                case 21:                                                 // twenty second key
                    fnSimulateInputChange(KEY_22_PORT_REF, fnMapPortBit(KEY_22_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_23_PORT_REF
                case 22:                                                 // twenty third key
                    fnSimulateInputChange(KEY_23_PORT_REF, fnMapPortBit(KEY_23_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_24_PORT_REF
                case 23:                                                 // twenty fourth key
                    fnSimulateInputChange(KEY_24_PORT_REF, fnMapPortBit(KEY_24_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_25_PORT_REF
                case 24:                                                 // twenty fifth key
                    fnSimulateInputChange(KEY_25_PORT_REF, fnMapPortBit(KEY_25_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_26_PORT_REF
                case 25:                                                 // twenty sixth key
                    fnSimulateInputChange(KEY_26_PORT_REF, fnMapPortBit(KEY_26_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_27_PORT_REF
                case 26:                                                  // twenty seventh key
                    fnSimulateInputChange(KEY_27_PORT_REF, fnMapPortBit(KEY_27_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_28_PORT_REF
                case 27:                                                  // twenty eighth key
                    fnSimulateInputChange(KEY_28_PORT_REF, fnMapPortBit(KEY_28_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_29_PORT_REF
                case 28:                                                  // twenty ninth key
                    fnSimulateInputChange(KEY_29_PORT_REF, fnMapPortBit(KEY_29_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_30_PORT_REF
                case 29:                                                  // thirtieth key
                    fnSimulateInputChange(KEY_30_PORT_REF, fnMapPortBit(KEY_30_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_31_PORT_REF
                case 30:                                                  // thirty first key
                    fnSimulateInputChange(KEY_31_PORT_REF, fnMapPortBit(KEY_31_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_32_PORT_REF
                case 31:                                                  // thirty second key
                    fnSimulateInputChange(KEY_32_PORT_REF, fnMapPortBit(KEY_32_PORT_PIN), iChange);
                    break;
    #endif
                }
            }
        }
    }
#else
    memcpy(iKeyPadInputs, intTable, sizeof(iKeyPadInputs));              // copy key pad state to local set
#endif
}

static int fnColumnLow(int iColumnOutput)
{
#if defined KEY_COLUMNS && !defined _NON_MATRIX
    switch (iColumnOutput) {
    case 0:
        return (KEY_COL_OUT_1 & ~KEY_COL_OUT_PORT_1 & KEY_COL_OUT_DDR_1);// if column 1 is being driven low
    #if KEY_COLUMNS > 1
    case 1:
        return (KEY_COL_OUT_2 & ~KEY_COL_OUT_PORT_2 & KEY_COL_OUT_DDR_2);// if column 2 is being driven low
    #endif
    #if KEY_COLUMNS > 2
    case 2:
        return (KEY_COL_OUT_3 & ~KEY_COL_OUT_PORT_3 & KEY_COL_OUT_DDR_3);// if column 3 is being driven low
    #endif
    #if KEY_COLUMNS > 3
    case 3:
        return (KEY_COL_OUT_4 & ~KEY_COL_OUT_PORT_4 & KEY_COL_OUT_DDR_4);// if column 4 is being driven low
    #endif
    #if KEY_COLUMNS > 4
    case 4:
        return (KEY_COL_OUT_5 & ~KEY_COL_OUT_PORT_5 & KEY_COL_OUT_DDR_5);// if column 5 is being driven low
    #endif
    #if KEY_COLUMNS > 5
    case 5:
        return (KEY_COL_OUT_6 & ~KEY_COL_OUT_PORT_6 & KEY_COL_OUT_DDR_6);// if column 6 is being driven low
    #endif
    #if KEY_COLUMNS > 6
    case 6:
        return (KEY_COL_OUT_7 & ~KEY_COL_OUT_PORT_7 & KEY_COL_OUT_DDR_7);// if column 7 is being driven low
    #endif
    #if KEY_COLUMNS > 7
    case 7:
        return (KEY_COL_OUT_8 & ~KEY_COL_OUT_PORT_8 & KEY_COL_OUT_DDR_8);// if column 8 is being driven low
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

#if !defined _NON_MATRIX
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
#endif
}

// This routine updates the ports to respect the present setting of a connected matrix key pad
//
extern void fnSimMatrixKB(void)
{
    int i, j;
    iFlagRefresh = fnPortChanges(1);                                     // synchronise with present settings                                                                         
    for (i = 0; i < KEY_COLUMNS; i++) {                                  // check whether a column control is being driven low. If this is the case, any pressed ones in the column are seen at the row input
        if (fnColumnLow(i)) {
            for (j = 0; j < KEY_ROWS; j++) {
                fnSetRowInput(j, iKeyPadInputs[i][j]);
            }
        }
    }
}
#endif

#if defined DEVICE_WITH_SLCD
static unsigned long lcd_wf3to0 = 0;
static unsigned long lcd_wf7to4 = 0;
static unsigned long lcd_wf11to8 = 0;
static unsigned long lcd_wf15to12 = 0;
static unsigned long lcd_wf19to16 = 0;
static unsigned long lcd_wf23to20 = 0;
static unsigned long lcd_wf27to24 = 0;
static unsigned long lcd_wf31to28 = 0;
static unsigned long lcd_wf35to32 = 0;
static unsigned long lcd_wf39to36 = 0;
static unsigned long lcd_wf43to40 = 0;
static unsigned long lcd_wf47to44 = 0;
static unsigned long lcd_wf51to48 = 0;
static unsigned long lcd_wf55to52 = 0;
static unsigned long lcd_wf59to56 = 0;
static unsigned long lcd_wf63to60 = 0;

// check for changes in SLCD segment registers and call display updates when necessary
//
extern void fnSimulateSLCD(void)
{
    #if defined SLCD_FILE
        #if defined KINETIS_KL || defined KINETIS_KL43
    if ((!(SIM_SCGC5 & SIM_SCGC5_SLCD)) || (!(LCD_GCR & LCD_GCR_LCDEN))) { // if SLCD controller not enabled
        return;
    }
        #else
    if ((!(SIM_SCGC3 & SIM_SCGC3_SLCD)) || (!(LCD_GCR & LCD_GCR_LCDEN))) { // if SLCD controller not enabled
        return;
    }
        #endif
    if (LCD_WF3TO0 != lcd_wf3to0) {
        CollectCommand(0, LCD_WF3TO0);                                   // inform of the new value (register 0)
        lcd_wf3to0 = LCD_WF3TO0;
    }
    if (LCD_WF7TO4 != lcd_wf7to4) {
        CollectCommand(1, LCD_WF7TO4);                                   // inform of the new value (register 1)
        lcd_wf7to4 = LCD_WF7TO4;
    }
    if (LCD_WF11TO8 != lcd_wf11to8) {
        CollectCommand(2, LCD_WF11TO8);                                  // inform of the new value (register 2)
        lcd_wf11to8 = LCD_WF11TO8;
    }
    if (LCD_WF15TO12 != lcd_wf15to12) {
        CollectCommand(3, LCD_WF15TO12);                                 // inform of the new value (register 3)
        lcd_wf15to12 = LCD_WF15TO12;
    }
    if (LCD_WF19TO16 != lcd_wf19to16) {
        CollectCommand(4, LCD_WF19TO16);                                 // inform of the new value (register 4)
        lcd_wf19to16 = LCD_WF19TO16;
    }
    if (LCD_WF23TO20 != lcd_wf23to20) {
        CollectCommand(5, LCD_WF23TO20);                                 // inform of the new value (register 5)
        lcd_wf23to20 = LCD_WF23TO20;
    }
    if (LCD_WF27TO24 != lcd_wf27to24) {
        CollectCommand(6, LCD_WF27TO24);                                 // inform of the new value (register 6)
        lcd_wf27to24 = LCD_WF27TO24;
    }
    if (LCD_WF31TO28 != lcd_wf31to28) {
        CollectCommand(7, LCD_WF31TO28);                                 // inform of the new value (register 7)
        lcd_wf31to28 = LCD_WF31TO28;
    }
    if (LCD_WF35TO32 != lcd_wf35to32) {
        CollectCommand(8, LCD_WF35TO32);                                 // inform of the new value (register 8)
        lcd_wf35to32 = LCD_WF35TO32;
    }
    if (LCD_WF39TO36 != lcd_wf39to36) {
        CollectCommand(9, LCD_WF39TO36);                                 // inform of the new value (register 9)
        lcd_wf39to36 = LCD_WF39TO36;
    }
    if (LCD_WF43TO40 != lcd_wf43to40) {
        CollectCommand(10, LCD_WF43TO40);                                // inform of the new value (register 10)
        lcd_wf43to40 = LCD_WF43TO40;
    }
    if (LCD_WF47TO44 != lcd_wf47to44) {
        CollectCommand(11, LCD_WF47TO44);                                // inform of the new value (register 11)
        lcd_wf47to44 = LCD_WF47TO44;
    }
    if (LCD_WF51TO48 != lcd_wf51to48) {
        CollectCommand(12, LCD_WF51TO48);                                // inform of the new value (register 12)
        lcd_wf51to48 = LCD_WF51TO48;
    }
    if (LCD_WF55TO52 != lcd_wf55to52) {
        CollectCommand(13, LCD_WF55TO52);                                // inform of the new value (register 13)
        lcd_wf55to52 = LCD_WF55TO52;
    }
    if (LCD_WF59TO56 != lcd_wf59to56) {
        CollectCommand(14, LCD_WF59TO56);                                 // inform of the new value (register 14)
        lcd_wf59to56 = LCD_WF59TO56;
    }
    if (LCD_WF63TO60 != lcd_wf63to60) {
        CollectCommand(15, LCD_WF63TO60);                                 // inform of the new value (register 15)
        lcd_wf63to60 = LCD_WF63TO60;
    }
    #endif
}
#endif


#if defined BATTERY_BACKED_RAM
// Return all RTC content which is battery backed up
//
extern int fnGetBatteryRAMContent(unsigned char *ucData, unsigned long ulReference)
{
    return 0;                                                            // all data saved
}
extern int fnPutBatteryRAMContent(unsigned char ucData, unsigned long ulReference)
{
    return 0;                                                            // no more data accepted
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


static void fnBufferSent(int iBuffer, int iRemote)
{
#if defined MSCAN_CAN_INTERFACE
#else
    int iChannel = (iBuffer >> 24);                                      // extract the channel number
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_CONTROL *ptrCAN_control;

    iBuffer &= 0x00ffffff;                                               // remove channel number
    #if NUMBER_OF_CAN_INTERFACES > 1
    if (iChannel != 0) {
        ptrMessageBuffer = MBUFF0_ADD_1;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
    }
    else {
        ptrMessageBuffer = MBUFF0_ADD_0;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    }
    #else
    ptrMessageBuffer = MBUFF0_ADD_0;
    ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    #endif
    ptrMessageBuffer += iBuffer;

    switch (iRemote) {        
    case SIM_CAN_TX_OK:
        if (((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ONCE) && (ptrMessageBuffer->ulCode_Len_TimeStamp & RTR)) { // sending remote frame
            ptrMessageBuffer->ulCode_Len_TimeStamp = (((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_RX_EMPTY) | (ptrCAN_control->CAN_TIMER & 0x0000ffff)); // convert to temporary rx message buffer
        }
        else {
            ptrMessageBuffer->ulCode_Len_TimeStamp = (((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE) | (ptrCAN_control->CAN_TIMER & 0x0000ffff));
        }
        break;
    case SIM_CAN_TX_FAIL:
        {
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                ptrCAN_control->CAN_ESR1 |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (ptrCAN_control->CAN_CTRL1 & ERRMSK) {                // only generate interrupt when interrupt is enabled
                    if (iChannel != 0) {
    #if NUMBER_OF_CAN_INTERFACES > 1
                        if (fnGenInt(irq_CAN1_ERROR_ID) != 0) {          // if CAN1 error interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_CAN1_ERROR(); // call the interrupt handler
                        }
    #endif
                    }
                    else {
                        if (fnGenInt(irq_CAN0_ERROR_ID) != 0) {          // if CAN0 error interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_CAN0_ERROR(); // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    case SIM_CAN_TX_REMOTE_OK:                                           // don't clear the message buffer on remote transmissions
    case SIM_CAN_RX_READY:
        break;
    }
    switch (iBuffer) {                                                   // call transmission complete interrupt
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        ptrCAN_control->CAN_IFLAG1 |= (1 << iBuffer);                    // set interrupt flag for the buffer
        if (ptrCAN_control->CAN_IMASK1 & (1 << iBuffer)) {               // check whether the interrupt is enabled on this buffer
            if (iChannel != 0) {
    #if NUMBER_OF_CAN_INTERFACES > 1
                if (fnGenInt(irq_CAN1_MESSAGE_ID) != 0) {                // if CAN1 message interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_CAN1_MESSAGE();    // call the interrupt handler
                }
    #endif
            }
            else {
                if (fnGenInt(irq_CAN0_MESSAGE_ID) != 0) {                // if CAN0 message interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_CAN0_MESSAGE();    // call the interrupt handler
                }
            }
        }
        break;
    default:                                                             // this happens when something is received
        break;
    }
#endif
}

    #if !defined _LOCAL_SIMULATION && defined SIM_KOMODO
// Komodo has received something on the CAN bus
//
static void fnCAN_reception(int iChannel, unsigned char ucDataLength, unsigned char *ptrData, unsigned long ulId, int iExtendedAddress, int iRemodeRequest, unsigned short usTimeStamp, Komodo km)
{
#if defined MSCAN_CAN_INTERFACE
#else
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_CONTROL *ptrCAN_control;
    int i = 0;
    int iRxAvailable = 0;
    int iOverrun = 0;
    unsigned char ucBuffer;
    if ((ucDataLength == 0) && (iRemodeRequest == 0)) {
        return;                                                          // ignore when reception without data
    }
    #if NUMBER_OF_CAN_INTERFACES > 1
    if (iChannel != 0) {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_1;                                 // the first of 16 message buffers in the FlexCan module
    }
    else {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_0;                                 // the first of 16 message buffers in the FlexCan module
    }
    #else
    ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    ptrMessageBuffer = MBUFF0_ADD_0;                                 // the first of 16 message buffers in the FlexCan module
    #endif

    if (iExtendedAddress == 0) {
        ulId <<= CAN_STANDARD_SHIFT;
        ulId &= CAN_STANDARD_BITMASK;
    }

    while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
        if (ptrMessageBuffer->ulID == ulId) {
            iRxAvailable++;
            if (iRemodeRequest != 0) {                                   // remote request being received
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ON_REQ) { // remote message waiting to be sent
                    int iResult;
                    km_can_packet_t pkt;
                    unsigned long arb_count = 0;
                    unsigned char ucTxDataLength;
                    unsigned char ucData[8];
                    pkt.id = ptrMessageBuffer->ulID;
                    // Send the CAN frame via remote simulator
                    // 
                    pkt.remote_req = 0;          
                    if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                        pkt.id |= CAN_EXTENDED_ID;                       // the address is to be handled as extended
                        pkt.extend_addr = 1;
                    }   
                    else {
                        pkt.id >>= CAN_STANDARD_SHIFT;                   // the address if a standard address
                        pkt.extend_addr = 0;
                    }
                    ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                    pkt.dlc = ucTxDataLength;
                    // Convert from long word, big-endian format
                    //
                    if (ucTxDataLength != 0) {
                        ucData[0] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 24);
                        if (ucTxDataLength > 1) {
                            ucData[1] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 16);
                            if (ucTxDataLength > 2) {
                                ucData[2] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 8);
                                if (ucTxDataLength > 3) {
                                    ucData[3] = (unsigned char)(ptrMessageBuffer->ulData[0]);
                                    if (ucTxDataLength > 4) {
                                        ucData[4] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 24);
                                        if (ucTxDataLength > 5) {
                                            ucData[5] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 16);
                                            if (ucTxDataLength > 6) {
                                                ucData[6] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 8);
                                                if (ucTxDataLength > 7) {
                                                    ucData[7] = (unsigned char)(ptrMessageBuffer->ulData[1]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, ucTxDataLength, (const unsigned char *)ucData); // send
                    iResult = km_can_async_collect(km, 10, &arb_count);  // collect the result of the last transmission
                    switch (iResult) {
                    case KM_OK:
                        fnBufferSent(((iChannel << 24) | i), SIM_CAN_TX_REMOTE_OK);
                        break;
                    default:
                        fnBufferSent(((iChannel << 24) | i), SIM_CAN_TX_FAIL);
                        break;
                    }
                    return;
                }
            }
            else if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_RX_EMPTY) {
                ucBuffer = (unsigned char)i;                             // we use this buffer for reception since it suits
                break;
            }
        }
        ptrMessageBuffer++;
        i++;
    }

    #if NUMBER_OF_CAN_INTERFACES > 1
    if (iChannel != 0) {
        ptrMessageBuffer = MBUFF0_ADD_1;
    }
    else {
        ptrMessageBuffer = MBUFF0_ADD_0;
    }
    #else
    ptrMessageBuffer = MBUFF0_ADD_0;
    #endif

    if (iRxAvailable != 0) {
        i = 0;
        while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
            if (ptrMessageBuffer->ulID == ulId) {
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == (MB_RX_FULL)) {
                    ucBuffer = (unsigned char)i;                         // we use this buffer for reception - it will set overrun...
                    iOverrun = 1;
                    break;
                }
            }
            ptrMessageBuffer++;
            i++;
        }
    }
    else {
        return;                                                          // no reception buffer found so ignore
    }

    #if NUMBER_OF_CAN_INTERFACES > 1
    if (iChannel != 0) {
        ptrMessageBuffer = MBUFF0_ADD_1;
    }
    else {
        ptrMessageBuffer = MBUFF0_ADD_0;
    }
    #else
    ptrMessageBuffer = MBUFF0_ADD_0;
    #endif
    ptrMessageBuffer += ucBuffer;                                        // set the local simulate buffer correspondingly

    if (iOverrun != 0) {
        ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_OVERRUN | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME)));
    }
    else {
        ptrMessageBuffer->ulCode_Len_TimeStamp = ((MB_RX_FULL) | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME))); // clear out code, length and time stamp fields
    }

    ptrMessageBuffer->ulCode_Len_TimeStamp |= usTimeStamp;
    ptrMessageBuffer->ulCode_Len_TimeStamp |= (ucDataLength << 16);
    ptrMessageBuffer->ulID = ulId;

    i = 0;
    // Save in long word, big-endian format
    //
    ptrMessageBuffer->ulData[0] = ((*(ptrData) << 24) | (*(ptrData + 1) << 16) | (*(ptrData + 2) << 8) | (*(ptrData + 3)));
    ptrMessageBuffer->ulData[1] = ((*(ptrData + 4) << 24) | (*(ptrData + 5) << 16) | (*(ptrData + 6) << 8) | (*(ptrData + 7)));
    fnBufferSent((ucBuffer | (iChannel << 24)), SIM_CAN_RX_READY);       // generate interrupt
#endif
}
    #endif

extern void fnSimCAN(int iChannel, int iBufferNumber, int iSpecial)
{
#if defined MSCAN_CAN_INTERFACE
#else
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_CONTROL *ptrCAN_control;
    #if !defined _LOCAL_SIMULATION
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
        (CAN0_TIMER)++;                                                  // normally these only count when CAN controller is enabled and they would count at the CAN bit rate
        #if NUMBER_OF_CAN_INTERFACES > 1
        (CAN1_TIMER)++;
        #endif
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
    #if NUMBER_OF_CAN_INTERFACES > 1
    if (iChannel != 0) {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_1;                                 // the first of 16 message buffers in the FlexCan module
    }
    else {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_0;                                 // the first of 16 message buffers in the FlexCan module
    }
    #else
    ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    ptrMessageBuffer = MBUFF0_ADD_0;                                     // the first of 16 message buffers in the FlexCan module
    #endif

    // Configuration changes
    //
    if ((ptrCAN_control->CAN_MCR & (CAN_FRZ | CAN_HALT)) != 0x00) {
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
            int iCAN_speed = 0;
            if ((CLK_SRC_PERIPH_CLK & ptrCAN_control->CAN_CTRL1) != 0) { // CAN clock derived from external clock/crystal (lowest jitter)
                iCAN_speed = ((_EXTERNAL_CLOCK/25)/(ptrCAN_control->CAN_CTRL1 >> 24)); // assume that time quanta is 25
            }
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

    ptrMessageBuffer += iBufferNumber;                                   // move to the buffer

    // Transmission
    //
    switch (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) {
    case MB_TX_INACTIVE:
        // Assume that we were previously active - for example sending remote frames - disable the HW simulator
        //
    #if !defined _LOCAL_SIMULATION
        #if defined USE_TELNET && defined UTASKER_SIM
        if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {
            if (iSpecial == CAN_SIM_FREE_BUFFER) {
                tcp_message.ucTCP_Message[0] = 'd';
            }
            else  {
                tcp_message.ucTCP_Message[0] = 'D';
            }
            tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number
            fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, 2, TCP_BUF_SEND); // send data to HW simulator
        }
        #elif defined SIM_KOMODO
        if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {
            if (iSpecial == CAN_SIM_FREE_BUFFER) {
                // Nothing to do in this case
                //
            }
            else  {
                unsigned char data[3] = {0x01, 0x02, 0x03};
                km_can_packet_t pkt;
                pkt.remote_req   = 0;
                pkt .extend_addr = 1;
                pkt.dlc          = 3;
                pkt.id           = 0x105;
                km_can_async_submit(km, iChannel, 0, &pkt, pkt.dlc, data);
            }
        }
        #endif
    #endif
        break;

    case MB_TX_SEND_ON_REQ:                                              // this buffer containes a queued message to be sent on a Remote Frame
    #if defined _LOCAL_SIMULATION
        // fall through
    #endif
    case MB_TX_SEND_ONCE:                                           
        if (ptrMessageBuffer->ulCode_Len_TimeStamp & RTR) {              // remote frame is to be transmitted
    #if defined _LOCAL_SIMULATION
        #if defined _TX_OK
            // The buffer converts automatically to a receive buffer
            //
            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_FULL | RTR | (0x03<<16));
            ptrMessageBuffer->ucData[0] = 0x01;                          // receive some dummy data
            ptrMessageBuffer->ucData[1] = 0x02;
            ptrMessageBuffer->ucData[2] = 0x03;
            goto _rx_int;
        #else
            // The remote transmission failed
            //
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                ptrCAN_control->CAN_ESR1 |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (ptrCAN_control->CAN_CTRL1 & ERRMSK) {                // only generate interrupt when interrupt is enabled
                    CAN_error_Interrupt();                               // simulate a number of errors
                }
            }
        #endif
    #else
            if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {// send a remote frame
        #if defined USE_TELNET && defined UTASKER_SIM
                unsigned long ulID;
                tcp_message.ucTCP_Message[0] = 'r';
                tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number

                ulID = ptrMessageBuffer->ulID;
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    ulID |= CAN_EXTENDED_ID;                             // the address is to be handled as extended
                }   
                else {
                    ulID >>= CAN_STANDARD_SHIFT;                         // the address if a standard address
                }
                tcp_message.ucTCP_Message[2] = (unsigned char)(ulID >> 24);
                tcp_message.ucTCP_Message[3] = (unsigned char)(ulID >> 16);
                tcp_message.ucTCP_Message[4] = (unsigned char)(ulID >> 8);
                tcp_message.ucTCP_Message[5] = (unsigned char)(ulID);

                fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, 6, TCP_BUF_SEND); // send data to HW simulator
        #elif defined SIM_KOMODO
                int iResult;
                km_can_packet_t pkt;
                unsigned long arb_count = 0;
                pkt.id = ptrMessageBuffer->ulID;
                // Send the CAN frame via remote simulator
                // 
                pkt.remote_req = 1;          
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    pkt.id |= CAN_EXTENDED_ID;                           // the address is to be handled as extended
                    pkt.extend_addr = 1;
                }   
                else {
                    pkt.id >>= CAN_STANDARD_SHIFT;                       // the address if a standard address
                    pkt.extend_addr = 0;
                }
                iLastTxBuffer = iBufferNumber;
                iBufferNumber = 0;
                ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                pkt.dlc = ucTxDataLength;
                km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, 0, 0); // send
                iResult = km_can_async_collect(km, 10, &arb_count);      // collect the result of the last transmission
                switch (iResult) {
                case KM_OK:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_OK);
                    break;
                default:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_FAIL);
                    break;
                }
        #endif
            }
            else {
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_FULL | RTR | (0x03 << 16)); // the buffer converts automatically to a receive buffer
                ptrMessageBuffer->ulData[0] = 0x01020300;                // receive some dummy data

                fnBufferSent(iBufferNumber, SIM_CAN_TX_REMOTE_OK);
            }
    #endif
        }
        else {                                                           // this buffer contains a message to be transmitted once
    #if defined _LOCAL_SIMULATION                                        // simple simulation for simple testing of driver
        #if defined _TX_OK
            ptrMessageBuffer->ulCode_Len_TimeStamp = ((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE);
_rx_int:
            CAN_IFLAG1 |= (0x00000001 << iBufferNumber);                 // set interrupt flag
            if (CAN_IMASK1 & (0x00000001 << iBufferNumber)) {
                CAN_buf_Interrupt(iBufferNumber);                        // call interrupt routine
            }
        #else
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                ptrCAN_control->CAN_ESR1 |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (ptrCAN_control->CAN_CTRL1 & ERRMSK) {                // only generate interrupt when interrupt is enabled
                    CAN_error_Interrupt();                               // simulate a number of errors
                }
            }
        #endif
    #else
            if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {
        #if defined USE_TELNET && defined UTASKER_SIM
                unsigned long ulID;
                // Send the CAN frame to remote simulator
                // [['T'] - BUFFER NUMBER - LENGTH - ID - DATA[...]]
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) ==  MB_TX_SEND_ON_REQ) {
                    tcp_message.ucTCP_Message[0] = 'R';                  // set remote transmission
                }
                else {
                    tcp_message.ucTCP_Message[0] = 'T';
                }
                tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number
                tcp_message.ucTCP_Message[2] = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                ulID = ptrMessageBuffer->ulID;
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    ulID |= CAN_EXTENDED_ID;                             // the address is to be handled as extended
                }   
                else {
                    ulID >>= CAN_STANDARD_SHIFT;                         // the address if a standard address
                }
                tcp_message.ucTCP_Message[3] = (unsigned char)(ulID >> 24);
                tcp_message.ucTCP_Message[4] = (unsigned char)(ulID >> 16);
                tcp_message.ucTCP_Message[5] = (unsigned char)(ulID >> 8);
                tcp_message.ucTCP_Message[6] = (unsigned char)(ulID);

                iLastTxBuffer = iBufferNumber;

                iBufferNumber = 0;
                usTxDataLength = tcp_message.ucTCP_Message[2] + 7;
                while (iBufferNumber++ < tcp_message.ucTCP_Message[2]) {
                    tcp_message.ucTCP_Message[iBufferNumber+6] = ptrMessageBuffer->ucData[iBufferNumber-1];
                }
                fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, usTxDataLength, TCP_BUF_SEND); // send data to HW simulator
        #elif defined SIM_KOMODO
                int iResult;
                km_can_packet_t pkt;
                unsigned long arb_count = 0;
                unsigned char ucData[8];
                pkt.id = ptrMessageBuffer->ulID;
                // Send the CAN frame via remote simulator
                // 
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ON_REQ) {
                    return;                                              // the message is not sent in this case but will be sent on any remote frame request receptions
                }
                else {
                    pkt.remote_req = 0;
                }             
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    pkt.id |= CAN_EXTENDED_ID;                           // the address is to be handled as extended
                    pkt.extend_addr = 1;
                }   
                else {
                    pkt.id >>= CAN_STANDARD_SHIFT;                       // the address if a standard address
                    pkt.extend_addr = 0;
                }

                iLastTxBuffer = iBufferNumber;
                iBufferNumber = 0;
                ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                pkt.dlc = ucTxDataLength;
                // Convert from long word, big-endian format
                //
                if (ucTxDataLength != 0) {
                    ucData[0] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 24);
                    if (ucTxDataLength > 1) {
                        ucData[1] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 16);
                        if (ucTxDataLength > 2) {
                            ucData[2] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 8);
                            if (ucTxDataLength > 3) {
                                ucData[3] = (unsigned char)(ptrMessageBuffer->ulData[0]);
                                if (ucTxDataLength > 4) {
                                    ucData[4] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 24);
                                    if (ucTxDataLength > 5) {
                                        ucData[5] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 16);
                                        if (ucTxDataLength > 6) {
                                            ucData[6] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 8);
                                            if (ucTxDataLength > 7) {
                                                ucData[7] = (unsigned char)(ptrMessageBuffer->ulData[1]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, ucTxDataLength, (const unsigned char *)ucData); // send
                iResult = km_can_async_collect(km, 10, &arb_count);      // collect the result of the last transmission
                switch (iResult) {
                case KM_OK:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_OK);
                    break;
                default:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_FAIL);
                    break;
                }
        #endif
            }
            else {
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) ==  MB_TX_SEND_ON_REQ) {
                    fnBufferSent(((iChannel << 24) | iBufferNumber), SIM_CAN_TX_REMOTE_OK);
                }
                else {
                    fnBufferSent(((iChannel << 24) | iBufferNumber), SIM_CAN_TX_OK);
                }
                return;
            }
    #endif
        }
        break;
    default:
        break;
    }
#endif
}
#endif

#if defined SUPPORT_TOUCH_SCREEN
static int iPenLocationX = 0;                                            // last sample ADC input value
static int iPenLocationY = 0;
static int iPenDown = 0;

    #if defined K70F150M_12M
        #define MIN_X_TOUCH          0x0230                              // tuned values - for calibration these should be taken from parameters
        #define MAX_X_TOUCH          0x0e90
        #define MIN_Y_TOUCH          0x03c5
        #define MAX_Y_TOUCH          0x0d30
    #elif defined TOUCH_FT6206
        #define MIN_X_TOUCH          0x0230                              // tuned values - for calibration these should be taken from parameters
        #define MAX_X_TOUCH          0x0e90
        #define MIN_Y_TOUCH          0x03c5
        #define MAX_Y_TOUCH          0x0d30
    #endif

extern void fnPenPressed(int iX, int iY)
{
    iPenLocationX = (iX - 6);
    iPenLocationY = (iY - 3);
    iPenDown = 1;
}

extern void fnPenMoved(int iX, int iY)
{
    iPenLocationX = (iX - 6);
    iPenLocationY = (iY - 3);
}

extern void fnPenLifted(void)
{
    iPenLocationX = 0;
    iPenLocationY = 0;
    iPenDown = 0;
}

extern void fnGetPenSamples(unsigned short *ptrX, unsigned short *ptrY)
{
    if (iPenLocationX != 0) {
        iPenLocationX = iPenLocationX;
    }
    if (iPenDown == 0) {                                                 // if the pen is not applied a high voltage is expected - higher than the normal maximum pressed value
        *ptrX = (MAX_X_TOUCH + 1);
        *ptrY = (MAX_Y_TOUCH + 1);
    }
    else {
        *ptrX = (MIN_X_TOUCH + ((iPenLocationX * ((MAX_X_TOUCH - MIN_X_TOUCH)))/GLCD_X));
        *ptrY = (MIN_Y_TOUCH + ((iPenLocationY * ((MAX_Y_TOUCH - MIN_Y_TOUCH)))/GLCD_Y));
    }
}
#endif

// Prepare a string to be displayed in the simulator status bar          // {37}
//
extern void fnUpdateOperatingDetails(void)
{
#if !defined NO_STATUS_BAR
    extern void fnPostOperatingDetails(char *ptrDetails);
    unsigned long ulBusClockSpeed;
    #if !defined BUS_FLASH_CLOCK_SHARED
    unsigned long ulFlashClockSpeed;
    #endif
    CHAR cOperatingDetails[256];
    CHAR *ptrBuffer = cOperatingDetails;
    ptrBuffer = uStrcpy(ptrBuffer, "FLASH = ");
    ptrBuffer = fnBufferDec((SIZE_OF_FLASH/1024), 0, ptrBuffer);
    #if defined HIGH_SPEED_RUN_MODE_AVAILABLE && defined HIGH_SPEED_RUN_MODE_REQUIRED
    if ((SMC_PMCTRL & SMC_PMCTRL_RUNM_HSRUN) == SMC_PMCTRL_RUNM_HSRUN) {
        ptrBuffer = uStrcpy(ptrBuffer, "k (read-only), SRAM");
    }
    else {
        ptrBuffer = uStrcpy(ptrBuffer, "k, SRAM = ");
    }
    #else
    ptrBuffer = uStrcpy(ptrBuffer, "k, SRAM = ");
    #endif
    ptrBuffer = fnBufferDec((SIZE_OF_RAM/1024), 0, ptrBuffer);
    #if defined BUS_FLASH_CLOCK_SHARED
    ptrBuffer = uStrcpy(ptrBuffer, "k, BUS/FLASH CLOCK = ");
    #else
    ptrBuffer = uStrcpy(ptrBuffer, "k, BUS CLOCK = ");
    #endif
    #if defined KINETIS_KL
        #if defined BUS_FLASH_CLOCK_SHARED
    ulBusClockSpeed = (SYSTEM_CLOCK/(((SIM_CLKDIV1 >> 16) & 0xf) + 1));
        #else
    ulBusClockSpeed = (MCGOUTCLK / (((SIM_CLKDIV1 >> 24) & 0xf) + 1));
        #endif
    #elif defined KINETIS_KV10
    ulBusClockSpeed = (SYSTEM_CLOCK/(((SIM_CLKDIV1 >> 16) & 0x7) + 1));
    #elif defined KINETIS_KE
        #if defined KINETIS_KE04 || defined KINETIS_KEA8 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
	ulBusClockSpeed = ICSOUT_CLOCK;
	switch (SIM_CLKDIV & SIM_CLKDIV_OUTDIV1_4) {
	case SIM_CLKDIV_OUTDIV1_1:
		break;
	case SIM_CLKDIV_OUTDIV1_2:
		ulBusClockSpeed /= 2;
		break;
	case SIM_CLKDIV_OUTDIV1_3:
		ulBusClockSpeed /= 3;
		break;
	case SIM_CLKDIV_OUTDIV1_4:
		ulBusClockSpeed /= 4;
		break;
	}
    if ((SIM_CLKDIV & SIM_CLKDIV_OUTDIV2_2) != 0) {
        ulBusClockSpeed /= 2;
    }
        #else
    if (SIM_BUSDIV & SIM_BUSDIVBUSDIV) {
        ulBusClockSpeed = (SYSTEM_CLOCK/2);
    }
    else {
        ulBusClockSpeed = SYSTEM_CLOCK;
    }
        #endif
    #else
    ulBusClockSpeed = (MCGOUTCLK/(((SIM_CLKDIV1 >> 24) & 0xf) + 1));
    #endif
    ptrBuffer = fnBufferDec(ulBusClockSpeed, 0, ptrBuffer);
    #if defined HIGH_SPEED_RUN_MODE_AVAILABLE && defined HIGH_SPEED_RUN_MODE_REQUIRED
    if ((SMC_PMCTRL & SMC_PMCTRL_RUNM_HSRUN) == SMC_PMCTRL_RUNM_HSRUN) {
        ptrBuffer = uStrcpy(ptrBuffer, " [HS-RUN]");
    }
    #endif
    #if !defined BUS_FLASH_CLOCK_SHARED                                  // in these devices the bus and flash clock are the same
    ptrBuffer = uStrcpy(ptrBuffer, ", FLASH CLOCK = ");
    ulFlashClockSpeed = (MCGOUTCLK/(((SIM_CLKDIV1 >> 16) & 0xf) + 1));
    ptrBuffer = fnBufferDec(ulFlashClockSpeed, 0, ptrBuffer);
    #endif
    fnPostOperatingDetails(cOperatingDetails);
#endif
}
#endif
