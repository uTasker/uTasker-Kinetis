/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      simkinetis.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    04.03.2012 Add NAND Flash controller                                 {1}
    17.03.2012 Add ADC                                                   {2}
    17.04.2012 Add touch port references                                 {3}
    18.04.2012 Add DMAMUX1                                               {4}
    27.04.2012 Add Reset Control Module                                  {5}
    17.06.2012 Add CRC module                                            {6}
    09.10.2012 Renumber and external ports to 8                          {7}
    04.02.2013 Add USBHS                                                 {8}
    21.03.2013 Add NAND flash controller buffers                         {9}
    24.05.2013 Add DDR1/2/LP SDRAM controller                            {10}
    29.05.2013 Add MCM                                                   {11}
    25.07.2013 Add PDB                                                   {12}
    26.12.2013 Add RANDOM_NUMBER_GENERATOR_B for devices without RNGA    {13}
    17.01.2014 Add CPACR                                                 {14}
    25.01.2014 Add Kinetis KL support                                    {15}
    15.02.2014 Add mask revision                                         {16}
    21.02.2014 Add MMCAU                                                 {17}
    06.05.2014 Add KL DMA                                                {18}
    08.06.2014 Add Crossbar Switch                                       {19}
    15.06.2014 Add LPTMR                                                 {20}
    15.07.2014 Add SMC                                                   {21}
    07.08.2014 Add KBI                                                   {22}
    08.12.2014 Add FlexIO                                                {23}
    14.12.2014 Add IRQ                                                   {24}
    17.12.2014 Add LCD controller                                        {25}
    11.01.2015 Add low-leakage wake-up unit (LLWU)                       {26}
    22.03.2015 Add MSCAN                                                 {27}
    14.12.2015 Add I2S                                                   {28}
    09.01.2016 Add QSPI and USB PHY                                      {29}
    02.06.2016 Add ACMP and CMP                                          {30}
    09.12.2016 Add PWT                                                   {31}
    11.02.2017 Add system clock generator                                {32}
    14.02.2017 Add LTC                                                   {33}

*/  

extern void fnSimulateSLCD(void);

typedef struct stKINETIS_CORTEX_M4
{
unsigned long ulRes1;
const unsigned long INT_CONT_TYPE;
unsigned long ulRes2[2];
unsigned long SYSTICK_CSR;
unsigned long SYSTICK_RELOAD;
volatile unsigned long SYSTICK_CURRENT;
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
unsigned long SYSTEM_HANDLER_4_7_PRIORITY_REGISTER;                      // {14}
unsigned long SYSTEM_HANDLER_8_11_PRIORITY_REGISTER;
unsigned long SYSTEM_HANDLER_12_15_PRIORITY_REGISTER;
unsigned long ulRes10[25];
unsigned long CPACR;
} KINETIS_CORTEX_M4;

typedef struct stKINETIS_CORTEX_M4_REGS
{
unsigned long ulPRIMASK;
} KINETIS_CORTEX_M4_REGS;

#define INTERRUPT_MASKED 0x00000001

#if defined KINETIS_KL                                                   // {18}
    #if !defined DEVICE_WITHOUT_DMA
    typedef struct stKINETIS_KL_DMA
    {
    unsigned long DMA_SAR0;
    unsigned long DMA_DAR0;
    unsigned long DMA_DSR_BCR0;
    unsigned long DMA_DCR0;
    unsigned long DMA_SAR1;
    unsigned long DMA_DAR1;
    unsigned long DMA_DSR_BCR1;
    unsigned long DMA_DCR1;
    unsigned long DMA_SAR2;
    unsigned long DMA_DAR2;
    unsigned long DMA_DSR_BCR;
    unsigned long DMA_DCR2;
    unsigned long DMA_SAR3;
    unsigned long DMA_DAR3;
    unsigned long DMA_DSR_BCR3;
    unsigned long DMA_DCR3;
    } KINETIS_KL_DMA;
    #endif
#else
    typedef struct stKINETIS_eDMA
    {
    unsigned long DMA_CR;
    unsigned long DMA_ES;
    unsigned long ulRes0;
    unsigned long DMA_ERQ;
    unsigned long ulRes1;
    unsigned long DMA_EEI;
    unsigned char DMA_CEEI;
    unsigned char DMA_SEEI;
    unsigned char DMA_CERQ;
    unsigned char DMA_SERQ;
    unsigned char DMA_CDNE;
    unsigned char DMA_SSRT;
    unsigned char DMA_CERR;
    unsigned char DMA_CINT;
    unsigned long ulRes2;
    unsigned long DMA_INT;
    unsigned long ulRes3;
    unsigned long DMA_ERR;
    unsigned long ulRes4;
    unsigned long DMA_HRS;
    unsigned long ulRes5[50];
    unsigned char DMA_DCHPRI3;
    unsigned char DMA_DCHPRI2;
    unsigned char DMA_DCHPRI1;
    unsigned char DMA_DCHPRI0;
    unsigned char DMA_DCHPRI7;
    unsigned char DMA_DCHPRI6;
    unsigned char DMA_DCHPRI5;
    unsigned char DMA_DCHPRI4;
    unsigned char DMA_DCHPRI11;
    unsigned char DMA_DCHPRI10;
    unsigned char DMA_DCHPRI9;
    unsigned char DMA_DCHPRI8;
    unsigned char DMA_DCHPRI15;
    unsigned char DMA_DCHPRI14;
    unsigned char DMA_DCHPRI13;
    unsigned char DMA_DCHPRI12;
    #if defined KINETIS_K_FPU && !defined KINETIS_K21 && !defined KINETIS_K22 && !defined KINETIS_K64
        unsigned char DMA_DCHPRI19;
        unsigned char DMA_DCHPRI18;
        unsigned char DMA_DCHPRI17;
        unsigned char DMA_DCHPRI16;
        unsigned char DMA_DCHPRI23;
        unsigned char DMA_DCHPRI22;
        unsigned char DMA_DCHPRI21;
        unsigned char DMA_DCHPRI20;
        unsigned char DMA_DCHPRI27;
        unsigned char DMA_DCHPRI26;
        unsigned char DMA_DCHPRI25;
        unsigned char DMA_DCHPRI24;
        unsigned char DMA_DCHPRI31;
        unsigned char DMA_DCHPRI30;
        unsigned char DMA_DCHPRI29;
        unsigned char DMA_DCHPRI28;
    #endif
    } KINETIS_eDMA;
#endif

typedef struct stKINETIS_eDMADES
{
unsigned long  DMA_TCD0_SADDR;
  signed short DMA_TCD0_SOFF;
unsigned short DMA_TCD0_ATTR;
unsigned long  DMA_TCD0_NBYTES_MLNO;
//unsigned long  DMA_TCD0_NBYTES_MLOFFNO;
//unsigned long  DMA_TCD0_NBYTES_MLOFFYES;
unsigned long  DMA_TCD0_SLAST;
unsigned long  DMA_TCD0_DADDR;
  signed short DMA_TCD0_DOFF;
unsigned short DMA_TCD0_CITER_ELINKYES;
//unsigned short DMA_TCD0_CITER_ELINKNO;
unsigned long  DMA_TCD0_DLASTSGA;
unsigned short DMA_TCD0_CSR;
unsigned short DMA_TCD0_BITER_ELINKYES;
//unsigned short DMA_TCD0_BITER_ELINKNO;
unsigned long  DMA_TCD1_SADDR;
  signed short DMA_TCD1_SOFF;
unsigned short DMA_TCD1_ATTR;
unsigned long  DMA_TCD1_NBYTES_MLNO;
//unsigned long  DMA_TCD1_NBYTES_MLOFFNO;
//unsigned long  DMA_TCD1_NBYTES_MLOFFYES;
unsigned long  DMA_TCD1_SLAST;
unsigned long  DMA_TCD1_DADDR;
  signed short DMA_TCD1_DOFF;
unsigned short DMA_TCD1_CITER_ELINKYES;
//unsigned short DMA_TCD1_CITER_ELINKNO;
unsigned long  DMA_TCD1_DLASTSGA;
unsigned short DMA_TCD1_CSR;
unsigned short DMA_TCD1_BITER_ELINKYES;
//unsigned short DMA_TCD1_BITER_ELINKNO;
unsigned long  DMA_TCD2_SADDR;
  signed short DMA_TCD2_SOFF;
unsigned short DMA_TCD2_ATTR;
unsigned long  DMA_TCD2_NBYTES_MLNO;
//unsigned long  DMA_TCD2_NBYTES_MLOFFNO;
//unsigned long  DMA_TCD2_NBYTES_MLOFFYES;
unsigned long  DMA_TCD2_SLAST;
unsigned long  DMA_TCD2_DADDR;
  signed short DMA_TCD2_DOFF;
unsigned short DMA_TCD2_CITER_ELINKYES;
//unsigned short DMA_TCD2_CITER_ELINKNO;
unsigned long  DMA_TCD2_DLASTSGA;
unsigned short DMA_TCD2_CSR;
unsigned short DMA_TCD2_BITER_ELINKYES;
//unsigned short DMA_TCD2_BITER_ELINKNO;
unsigned long  DMA_TCD3_SADDR;
  signed short DMA_TCD3_SOFF;
unsigned short DMA_TCD3_ATTR;
unsigned long  DMA_TCD3_NBYTES_MLNO;
//unsigned long  DMA_TCD3_NBYTES_MLOFFNO;
//unsigned long  DMA_TCD3_NBYTES_MLOFFYES;
unsigned long  DMA_TCD3_SLAST;
unsigned long  DMA_TCD3_DADDR;
  signed short DMA_TCD3_DOFF;
unsigned short DMA_TCD3_CITER_ELINKYES;
//unsigned short DMA_TCD3_CITER_ELINKNO;
unsigned long  DMA_TCD3_DLASTSGA;
unsigned short DMA_TCD3_CSR;
unsigned short DMA_TCD3_BITER_ELINKYES;
//unsigned short DMA_TCD3_BITER_ELINKNO;
#if defined irq_DMA4_ID
    unsigned long  DMA_TCD4_SADDR;
      signed short DMA_TCD4_SOFF;
    unsigned short DMA_TCD4_ATTR;
    unsigned long  DMA_TCD4_NBYTES_MLNO;
    //unsigned long  DMA_TCD4_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD4_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD4_SLAST;
    unsigned long  DMA_TCD4_DADDR;
      signed short DMA_TCD4_DOFF;
    unsigned short DMA_TCD4_CITER_ELINKYES;
    //unsigned short DMA_TCD4_CITER_ELINKNO;
    unsigned long  DMA_TCD4_DLASTSGA;
    unsigned short DMA_TCD4_CSR;
    unsigned short DMA_TCD4_BITER_ELINKYES;
    //unsigned short DMA_TCD4_BITER_ELINKNO;
    unsigned long  DMA_TCD5_SADDR;
      signed short DMA_TCD5_SOFF;
    unsigned short DMA_TCD5_ATTR;
    unsigned long  DMA_TCD5_NBYTES_MLNO;
    //unsigned long  DMA_TCD5_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD5_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD5_SLAST;
    unsigned long  DMA_TCD5_DADDR;
      signed short DMA_TCD5_DOFF;
    unsigned short DMA_TCD5_CITER_ELINKYES;
    //unsigned short DMA_TCD5_CITER_ELINKNO;
    unsigned long  DMA_TCD5_DLASTSGA;
    unsigned short DMA_TCD5_CSR;
    unsigned short DMA_TCD5_BITER_ELINKYES;
    //unsigned short DMA_TCD5_BITER_ELINKNO;
    unsigned long  DMA_TCD6_SADDR;
      signed short DMA_TCD6_SOFF;
    unsigned short DMA_TCD6_ATTR;
    unsigned long  DMA_TCD6_NBYTES_MLNO;
    //unsigned long  DMA_TCD6_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD6_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD6_SLAST;
    unsigned long  DMA_TCD6_DADDR;
      signed short DMA_TCD6_DOFF;
    unsigned short DMA_TCD6_CITER_ELINKYES;
    //unsigned short DMA_TCD6_CITER_ELINKNO;
    unsigned long  DMA_TCD6_DLASTSGA;
    unsigned short DMA_TCD6_CSR;
    unsigned short DMA_TCD6_BITER_ELINKYES;
    //unsigned short DMA_TCD6_BITER_ELINKNO;
    unsigned long  DMA_TCD7_SADDR;
      signed short DMA_TCD7_SOFF;
    unsigned short DMA_TCD7_ATTR;
    unsigned long  DMA_TCD7_NBYTES_MLNO;
    //unsigned long  DMA_TCD7_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD7_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD7_SLAST;
    unsigned long  DMA_TCD7_DADDR;
      signed short DMA_TCD7_DOFF;
    unsigned short DMA_TCD7_CITER_ELINKYES;
    //unsigned short DMA_TCD7_CITER_ELINKNO;
    unsigned long  DMA_TCD7_DLASTSGA;
    unsigned short DMA_TCD7_CSR;
    unsigned short DMA_TCD7_BITER_ELINKYES;
    //unsigned short DMA_TCD7_BITER_ELINKNO;
    unsigned long  DMA_TCD8_SADDR;
      signed short DMA_TCD8_SOFF;
    unsigned short DMA_TCD8_ATTR;
    unsigned long  DMA_TCD8_NBYTES_MLNO;
    //unsigned long  DMA_TCD8_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD8_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD8_SLAST;
    unsigned long  DMA_TCD8_DADDR;
      signed short DMA_TCD8_DOFF;
    unsigned short DMA_TCD8_CITER_ELINKYES;
    //unsigned short DMA_TCD8_CITER_ELINKNO;
    unsigned long  DMA_TCD8_DLASTSGA;
    unsigned short DMA_TCD8_CSR;
    unsigned short DMA_TCD8_BITER_ELINKYES;
    //unsigned short DMA_TCD8_BITER_ELINKNO;
    unsigned long  DMA_TCD9_SADDR;
      signed short DMA_TCD9_SOFF;
    unsigned short DMA_TCD9_ATTR;
    unsigned long  DMA_TCD9_NBYTES_MLNO;
    //unsigned long  DMA_TCD9_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD9_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD9_SLAST;
    unsigned long  DMA_TCD9_DADDR;
      signed short DMA_TCD9_DOFF;
    unsigned short DMA_TCD9_CITER_ELINKYES;
    //unsigned short DMA_TCD9_CITER_ELINKNO;
    unsigned long  DMA_TCD9_DLASTSGA;
    unsigned short DMA_TCD9_CSR;
    unsigned short DMA_TCD9_BITER_ELINKYES;
    //unsigned short DMA_TCD9_BITER_ELINKNO;
    unsigned long  DMA_TCD10_SADDR;
      signed short DMA_TCD10_SOFF;
    unsigned short DMA_TCD10_ATTR;
    unsigned long  DMA_TCD10_NBYTES_MLNO;
    //unsigned long  DMA_TCD10_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD10_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD10_SLAST;
    unsigned long  DMA_TCD10_DADDR;
      signed short DMA_TCD10_DOFF;
    unsigned short DMA_TCD10_CITER_ELINKYES;
    //unsigned short DMA_TCD10_CITER_ELINKNO;
    unsigned long  DMA_TCD10_DLASTSGA;
    unsigned short DMA_TCD10_CSR;
    unsigned short DMA_TCD10_BITER_ELINKYES;
    //unsigned short DMA_TCD10_BITER_ELINKNO;
    unsigned long  DMA_TCD11_SADDR;
      signed short DMA_TCD11_SOFF;
    unsigned short DMA_TCD11_ATTR;
    unsigned long  DMA_TCD11_NBYTES_MLNO;
    //unsigned long  DMA_TCD11_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD11_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD11_SLAST;
    unsigned long  DMA_TCD11_DADDR;
      signed short DMA_TCD11_DOFF;
    unsigned short DMA_TCD11_CITER_ELINKYES;
    //unsigned short DMA_TCD11_CITER_ELINKNO;
    unsigned long  DMA_TCD11_DLASTSGA;
    unsigned short DMA_TCD11_CSR;
    unsigned short DMA_TCD11_BITER_ELINKYES;
    //unsigned short DMA_TCD11_BITER_ELINKNO;
    unsigned long  DMA_TCD12_SADDR;
      signed short DMA_TCD12_SOFF;
    unsigned short DMA_TCD12_ATTR;
    unsigned long  DMA_TCD12_NBYTES_MLNO;
    //unsigned long  DMA_TCD12_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD12_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD12_SLAST;
    unsigned long  DMA_TCD12_DADDR;
      signed short DMA_TCD12_DOFF;
    unsigned short DMA_TCD12_CITER_ELINKYES;
    //unsigned short DMA_TCD12_CITER_ELINKNO;
    unsigned long  DMA_TCD12_DLASTSGA;
    unsigned short DMA_TCD12_CSR;
    unsigned short DMA_TCD12_BITER_ELINKYES;
    //unsigned short DMA_TCD12_BITER_ELINKNO;
    unsigned long  DMA_TCD13_SADDR;
      signed short DMA_TCD13_SOFF;
    unsigned short DMA_TCD13_ATTR;
    unsigned long  DMA_TCD13_NBYTES_MLNO;
    //unsigned long  DMA_TCD13_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD13_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD13_SLAST;
    unsigned long  DMA_TCD13_DADDR;
      signed short DMA_TCD13_DOFF;
    unsigned short DMA_TCD13_CITER_ELINKYES;
    //unsigned short DMA_TCD13_CITER_ELINKNO;
    unsigned long  DMA_TCD13_DLASTSGA;
    unsigned short DMA_TCD13_CSR;
    unsigned short DMA_TCD13_BITER_ELINKYES;
    //unsigned short DMA_TCD13_BITER_ELINKNO;
    unsigned long  DMA_TCD14_SADDR;
      signed short DMA_TCD14_SOFF;
    unsigned short DMA_TCD14_ATTR;
    unsigned long  DMA_TCD14_NBYTES_MLNO;
    //unsigned long  DMA_TCD14_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD14_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD14_SLAST;
    unsigned long  DMA_TCD14_DADDR;
      signed short DMA_TCD14_DOFF;
    unsigned short DMA_TCD14_CITER_ELINKYES;
    //unsigned short DMA_TCD14_CITER_ELINKNO;
    unsigned long  DMA_TCD14_DLASTSGA;
    unsigned short DMA_TCD14_CSR;
    unsigned short DMA_TCD14_BITER_ELINKYES;
    //unsigned short DMA_TCD14_BITER_ELINKNO;
    unsigned long  DMA_TCD15_SADDR;
      signed short DMA_TCD15_SOFF;
    unsigned short DMA_TCD15_ATTR;
    unsigned long  DMA_TCD15_NBYTES_MLNO;
    //unsigned long  DMA_TCD15_NBYTES_MLOFFNO;
    //unsigned long  DMA_TCD15_NBYTES_MLOFFYES;
    unsigned long  DMA_TCD15_SLAST;
    unsigned long  DMA_TCD15_DADDR;
      signed short DMA_TCD15_DOFF;
    unsigned short DMA_TCD15_CITER_ELINKYES;
    //unsigned short DMA_TCD15_CITER_ELINKNO;
    unsigned long  DMA_TCD15_DLASTSGA;
    unsigned short DMA_TCD15_CSR;
    unsigned short DMA_TCD15_BITER_ELINKYES;
    //unsigned short DMA_TCD15_BITER_ELINKNO;
    #if defined KINETIS_K_FPU && !defined KINETIS_K21 && !defined KINETIS_K22 && !defined KINETIS_K64
        unsigned long  DMA_TCD16_SADDR;
          signed short DMA_TCD16_SOFF;
        unsigned short DMA_TCD16_ATTR;
        unsigned long  DMA_TCD16_NBYTES_MLNO;
        //unsigned long  DMA_TCD16_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD16_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD16_SLAST;
        unsigned long  DMA_TCD16_DADDR;
          signed short DMA_TCD16_DOFF;
        unsigned short DMA_TCD16_CITER_ELINKYES;
        //unsigned short DMA_TCD0_CITER_ELINKNO;
        unsigned long  DMA_TCD16_DLASTSGA;
        unsigned short DMA_TCD16_CSR;
        unsigned short DMA_TCD16_BITER_ELINKYES;
        //unsigned short DMA_TCD16_BITER_ELINKNO;
        unsigned long  DMA_TCD17_SADDR;
          signed short DMA_TCD17_SOFF;
        unsigned short DMA_TCD17_ATTR;
        unsigned long  DMA_TCD17_NBYTES_MLNO;
        //unsigned long  DMA_TCD17_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD17_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD17_SLAST;
        unsigned long  DMA_TCD17_DADDR;
          signed short DMA_TCD17_DOFF;
        unsigned short DMA_TCD17_CITER_ELINKYES;
        //unsigned short DMA_TCD17_CITER_ELINKNO;
        unsigned long  DMA_TCD17_DLASTSGA;
        unsigned short DMA_TCD17_CSR;
        unsigned short DMA_TCD17_BITER_ELINKYES;
        //unsigned short DMA_TCD17_BITER_ELINKNO;
        unsigned long  DMA_TCD18_SADDR;
          signed short DMA_TCD18_SOFF;
        unsigned short DMA_TCD18_ATTR;
        unsigned long  DMA_TCD18_NBYTES_MLNO;
        //unsigned long  DMA_TCD18_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD18_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD18_SLAST;
        unsigned long  DMA_TCD18_DADDR;
          signed short DMA_TCD18_DOFF;
        unsigned short DMA_TCD18_CITER_ELINKYES;
        //unsigned short DMA_TCD18_CITER_ELINKNO;
        unsigned long  DMA_TCD18_DLASTSGA;
        unsigned short DMA_TCD18_CSR;
        unsigned short DMA_TCD18_BITER_ELINKYES;
        //unsigned short DMA_TCD18_BITER_ELINKNO;
        unsigned long  DMA_TCD19_SADDR;
          signed short DMA_TCD19_SOFF;
        unsigned short DMA_TCD19_ATTR;
        unsigned long  DMA_TCD19_NBYTES_MLNO;
        //unsigned long  DMA_TCD19_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD19_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD19_SLAST;
        unsigned long  DMA_TCD19_DADDR;
          signed short DMA_TCD19_DOFF;
        unsigned short DMA_TCD19_CITER_ELINKYES;
        //unsigned short DMA_TCD19_CITER_ELINKNO;
        unsigned long  DMA_TCD19_DLASTSGA;
        unsigned short DMA_TCD19_CSR;
        unsigned short DMA_TCD19_BITER_ELINKYES;
        //unsigned short DMA_TCD19_BITER_ELINKNO;
        unsigned long  DMA_TCD20_SADDR;
          signed short DMA_TCD20_SOFF;
        unsigned short DMA_TCD20_ATTR;
        unsigned long  DMA_TCD20_NBYTES_MLNO;
        //unsigned long  DMA_TCD20_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD20_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD20_SLAST;
        unsigned long  DMA_TCD20_DADDR;
          signed short DMA_TCD20_DOFF;
        unsigned short DMA_TCD20_CITER_ELINKYES;
        //unsigned short DMA_TCD20_CITER_ELINKNO;
        unsigned long  DMA_TCD20_DLASTSGA;
        unsigned short DMA_TCD20_CSR;
        unsigned short DMA_TCD20_BITER_ELINKYES;
        //unsigned short DMA_TCD20_BITER_ELINKNO;
        unsigned long  DMA_TCD21_SADDR;
          signed short DMA_TCD21_SOFF;
        unsigned short DMA_TCD21_ATTR;
        unsigned long  DMA_TCD21_NBYTES_MLNO;
        //unsigned long  DMA_TCD21_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD21_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD21_SLAST;
        unsigned long  DMA_TCD21_DADDR;
          signed short DMA_TCD21_DOFF;
        unsigned short DMA_TCD21_CITER_ELINKYES;
        //unsigned short DMA_TCD21_CITER_ELINKNO;
        unsigned long  DMA_TCD21_DLASTSGA;
        unsigned short DMA_TCD21_CSR;
        unsigned short DMA_TCD21_BITER_ELINKYES;
        //unsigned short DMA_TCD21_BITER_ELINKNO;
        unsigned long  DMA_TCD22_SADDR;
          signed short DMA_TCD22_SOFF;
        unsigned short DMA_TCD22_ATTR;
        unsigned long  DMA_TCD22_NBYTES_MLNO;
        //unsigned long  DMA_TCD22_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD22_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD22_SLAST;
        unsigned long  DMA_TCD22_DADDR;
          signed short DMA_TCD22_DOFF;
        unsigned short DMA_TCD22_CITER_ELINKYES;
        //unsigned short DMA_TCD22_CITER_ELINKNO;
        unsigned long  DMA_TCD22_DLASTSGA;
        unsigned short DMA_TCD22_CSR;
        unsigned short DMA_TCD22_BITER_ELINKYES;
        //unsigned short DMA_TCD22_BITER_ELINKNO;
        unsigned long  DMA_TCD23_SADDR;
          signed short DMA_TCD23_SOFF;
        unsigned short DMA_TCD23_ATTR;
        unsigned long  DMA_TCD23_NBYTES_MLNO;
        //unsigned long  DMA_TCD23_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD23_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD23_SLAST;
        unsigned long  DMA_TCD23_DADDR;
          signed short DMA_TCD23_DOFF;
        unsigned short DMA_TCD23_CITER_ELINKYES;
        //unsigned short DMA_TCD23_CITER_ELINKNO;
        unsigned long  DMA_TCD23_DLASTSGA;
        unsigned short DMA_TCD23_CSR;
        unsigned short DMA_TCD23_BITER_ELINKYES;
        //unsigned short DMA_TCD23_BITER_ELINKNO;
        unsigned long  DMA_TCD24_SADDR;
          signed short DMA_TCD24_SOFF;
        unsigned short DMA_TCD24_ATTR;
        unsigned long  DMA_TCD24_NBYTES_MLNO;
        //unsigned long  DMA_TCD24_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD24_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD24_SLAST;
        unsigned long  DMA_TCD24_DADDR;
          signed short DMA_TCD24_DOFF;
        unsigned short DMA_TCD24_CITER_ELINKYES;
        //unsigned short DMA_TCD24_CITER_ELINKNO;
        unsigned long  DMA_TCD24_DLASTSGA;
        unsigned short DMA_TCD24_CSR;
        unsigned short DMA_TCD24_BITER_ELINKYES;
        //unsigned short DMA_TCD24_BITER_ELINKNO;
        unsigned long  DMA_TCD25_SADDR;
          signed short DMA_TCD25_SOFF;
        unsigned short DMA_TCD25_ATTR;
        unsigned long  DMA_TCD25_NBYTES_MLNO;
        //unsigned long  DMA_TCD25_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD25_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD25_SLAST;
        unsigned long  DMA_TCD25_DADDR;
          signed short DMA_TCD25_DOFF;
        unsigned short DMA_TCD25_CITER_ELINKYES;
        //unsigned short DMA_TCD25_CITER_ELINKNO;
        unsigned long  DMA_TCD25_DLASTSGA;
        unsigned short DMA_TCD25_CSR;
        unsigned short DMA_TCD25_BITER_ELINKYES;
        //unsigned short DMA_TCD25_BITER_ELINKNO;
        unsigned long  DMA_TCD26_SADDR;
          signed short DMA_TCD26_SOFF;
        unsigned short DMA_TCD26_ATTR;
        unsigned long  DMA_TCD26_NBYTES_MLNO;
        //unsigned long  DMA_TCD26_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD26_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD26_SLAST;
        unsigned long  DMA_TCD26_DADDR;
          signed short DMA_TCD26_DOFF;
        unsigned short DMA_TCD26_CITER_ELINKYES;
        //unsigned short DMA_TCD26_CITER_ELINKNO;
        unsigned long  DMA_TCD26_DLASTSGA;
        unsigned short DMA_TCD26_CSR;
        unsigned short DMA_TCD26_BITER_ELINKYES;
        //unsigned short DMA_TCD26_BITER_ELINKNO;
        unsigned long  DMA_TCD27_SADDR;
          signed short DMA_TCD27_SOFF;
        unsigned short DMA_TCD27_ATTR;
        unsigned long  DMA_TCD27_NBYTES_MLNO;
        //unsigned long  DMA_TCD27_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD27_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD27_SLAST;
        unsigned long  DMA_TCD27_DADDR;
          signed short DMA_TCD27_DOFF;
        unsigned short DMA_TCD27_CITER_ELINKYES;
        //unsigned short DMA_TCD27_CITER_ELINKNO;
        unsigned long  DMA_TCD27_DLASTSGA;
        unsigned short DMA_TCD27_CSR;
        unsigned short DMA_TCD27_BITER_ELINKYES;
        //unsigned short DMA_TCD27_BITER_ELINKNO;
        unsigned long  DMA_TCD28_SADDR;
          signed short DMA_TCD28_SOFF;
        unsigned short DMA_TCD28_ATTR;
        unsigned long  DMA_TCD28_NBYTES_MLNO;
        //unsigned long  DMA_TCD28_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD28_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD28_SLAST;
        unsigned long  DMA_TCD28_DADDR;
          signed short DMA_TCD28_DOFF;
        unsigned short DMA_TCD28_CITER_ELINKYES;
        //unsigned short DMA_TCD28_CITER_ELINKNO;
        unsigned long  DMA_TCD28_DLASTSGA;
        unsigned short DMA_TCD28_CSR;
        unsigned short DMA_TCD28_BITER_ELINKYES;
        //unsigned short DMA_TCD28_BITER_ELINKNO;
        unsigned long  DMA_TCD29_SADDR;
          signed short DMA_TCD29_SOFF;
        unsigned short DMA_TCD29_ATTR;
        unsigned long  DMA_TCD29_NBYTES_MLNO;
        //unsigned long  DMA_TCD29_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD29_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD29_SLAST;
        unsigned long  DMA_TCD29_DADDR;
          signed short DMA_TCD29_DOFF;
        unsigned short DMA_TCD29_CITER_ELINKYES;
        //unsigned short DMA_TCD29_CITER_ELINKNO;
        unsigned long  DMA_TCD29_DLASTSGA;
        unsigned short DMA_TCD29_CSR;
        unsigned short DMA_TCD29_BITER_ELINKYES;
        //unsigned short DMA_TCD29_BITER_ELINKNO;
        unsigned long  DMA_TCD30_SADDR;
          signed short DMA_TCD30_SOFF;
        unsigned short DMA_TCD30_ATTR;
        unsigned long  DMA_TCD30_NBYTES_MLNO;
        //unsigned long  DMA_TCD30_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD30_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD30_SLAST;
        unsigned long  DMA_TCD30_DADDR;
          signed short DMA_TCD30_DOFF;
        unsigned short DMA_TCD30_CITER_ELINKYES;
        //unsigned short DMA_TCD30_CITER_ELINKNO;
        unsigned long  DMA_TCD30_DLASTSGA;
        unsigned short DMA_TCD30_CSR;
        unsigned short DMA_TCD30_BITER_ELINKYES;
        //unsigned short DMA_TCD30_BITER_ELINKNO;
        unsigned long  DMA_TCD31_SADDR;
          signed short DMA_TCD31_SOFF;
        unsigned short DMA_TCD31_ATTR;
        unsigned long  DMA_TCD31_NBYTES_MLNO;
        //unsigned long  DMA_TCD31_NBYTES_MLOFFNO;
        //unsigned long  DMA_TCD31_NBYTES_MLOFFYES;
        unsigned long  DMA_TCD31_SLAST;
        unsigned long  DMA_TCD31_DADDR;
          signed short DMA_TCD31_DOFF;
        unsigned short DMA_TCD31_CITER_ELINKYES;
        //unsigned short DMA_TCD31_CITER_ELINKNO;
        unsigned long  DMA_TCD31_DLASTSGA;
        unsigned short DMA_TCD31_CSR;
        unsigned short DMA_TCD31_BITER_ELINKYES;
        //unsigned short DMA_TCD31_BITER_ELINKNO;
    #endif
#endif
} KINETIS_eDMADES;

typedef struct stKINETIS_FB
{
unsigned long CSAR0;
unsigned long CSMR0;
unsigned long CSCR0;
unsigned long CSAR1;
unsigned long CSMR1;
unsigned long CSCR1;
unsigned long CSAR2;
unsigned long CSMR2;
unsigned long CSCR2;
unsigned long CSAR3;
unsigned long CSMR3;
unsigned long CSCR3;
unsigned long CSAR4;
unsigned long CSMR4;
unsigned long CSCR4;
unsigned long CSAR5;
unsigned long CSMR5;
unsigned long CSCR5;
unsigned long ulRes0[6];
unsigned long CSPMCR;
} KINETIS_FB;

#if defined MPU_AVAILABLE
typedef struct stKINETIS_MPU
{
unsigned long MPU_CESR;
unsigned long ulRes0[3];
unsigned long MPU_EAR0;
unsigned long MPU_EDR0;
unsigned long MPU_EAR1;
unsigned long MPU_EDR1;
unsigned long MPU_EAR2;
unsigned long MPU_EDR2;
unsigned long MPU_EAR3;
unsigned long MPU_EDR3;
unsigned long MPU_EAR4;
unsigned long MPU_EDR4;
unsigned long MPU_EAR5;
unsigned long MPU_EDR5;
unsigned long MPU_EAR6;
unsigned long MPU_EDR6;
unsigned long MPU_EAR7;
unsigned long MPU_EDR7;
unsigned long ulRes1[236];
unsigned long MPU_RGD0_WORD0;
unsigned long MPU_RGD0_WORD1;
unsigned long MPU_RGD0_WORD2;
unsigned long MPU_RGD0_WORD3;
unsigned long MPU_RGD1_WORD0;
unsigned long MPU_RGD1_WORD1;
unsigned long MPU_RGD1_WORD2;
unsigned long MPU_RGD1_WORD3;
unsigned long MPU_RGD2_WORD0;
unsigned long MPU_RGD2_WORD1;
unsigned long MPU_RGD2_WORD2;
unsigned long MPU_RGD2_WORD3;
unsigned long MPU_RGD3_WORD0;
unsigned long MPU_RGD3_WORD1;
unsigned long MPU_RGD3_WORD2;
unsigned long MPU_RGD3_WORD3;
unsigned long MPU_RGD4_WORD0;
unsigned long MPU_RGD4_WORD1;
unsigned long MPU_RGD4_WORD2;
unsigned long MPU_RGD4_WORD3;
unsigned long MPU_RGD5_WORD0;
unsigned long MPU_RGD5_WORD1;
unsigned long MPU_RGD5_WORD2;
unsigned long MPU_RGD5_WORD3;
unsigned long MPU_RGD6_WORD0;
unsigned long MPU_RGD6_WORD1;
unsigned long MPU_RGD6_WORD2;
unsigned long MPU_RGD6_WORD3;
unsigned long MPU_RGD7_WORD0;
unsigned long MPU_RGD7_WORD1;
unsigned long MPU_RGD7_WORD2;
unsigned long MPU_RGD7_WORD3;
unsigned long MPU_RGD8_WORD0;
unsigned long MPU_RGD8_WORD1;
unsigned long MPU_RGD8_WORD2;
unsigned long MPU_RGD8_WORD3;
unsigned long MPU_RGD9_WORD0;
unsigned long MPU_RGD9_WORD1;
unsigned long MPU_RGD9_WORD2;
unsigned long MPU_RGD9_WORD3;
unsigned long MPU_RGD10_WORD0;
unsigned long MPU_RGD10_WORD1;
unsigned long MPU_RGD10_WORD2;
unsigned long MPU_RGD10_WORD3;
unsigned long MPU_RGD11_WORD0;
unsigned long MPU_RGD11_WORD1;
unsigned long MPU_RGD11_WORD2;
unsigned long MPU_RGD11_WORD3;
unsigned long MPU_RGD12_WORD0;
unsigned long MPU_RGD12_WORD1;
unsigned long MPU_RGD12_WORD2;
unsigned long MPU_RGD12_WORD3;
unsigned long MPU_RGD13_WORD0;
unsigned long MPU_RGD13_WORD1;
unsigned long MPU_RGD13_WORD2;
unsigned long MPU_RGD13_WORD3;
unsigned long MPU_RGD14_WORD0;
unsigned long MPU_RGD14_WORD1;
unsigned long MPU_RGD14_WORD2;
unsigned long MPU_RGD14_WORD3;
unsigned long MPU_RGD15_WORD0;
unsigned long MPU_RGD15_WORD1;
unsigned long MPU_RGD15_WORD2;
unsigned long MPU_RGD15_WORD3;
unsigned long ulRes2[448];
unsigned long MPU_RGDAAC0;
unsigned long MPU_RGDAAC1;
unsigned long MPU_RGDAAC2;
unsigned long MPU_RGDAAC3;
unsigned long MPU_RGDAAC4;
unsigned long MPU_RGDAAC5;
unsigned long MPU_RGDAAC6;
unsigned long MPU_RGDAAC7;
unsigned long MPU_RGDAAC8;
unsigned long MPU_RGDAAC9;
unsigned long MPU_RGDAAC10;
unsigned long MPU_RGDAAC11;
unsigned long MPU_RGDAAC12;
unsigned long MPU_RGDAAC13;
unsigned long MPU_RGDAAC14;
unsigned long MPU_RGDAAC15;
} KINETIS_MPU;
#endif

typedef struct stKINETIS_FMC
{
unsigned long FMC_PFAPR;
unsigned long FMC_PFB0CR;
unsigned long FMC_PFB1CR;
unsigned long ulRes0[61];
unsigned long FMC_TAGVDW0S0;
unsigned long FMC_TAGVDW0S1;
unsigned long FMC_TAGVDW0S2;
unsigned long FMC_TAGVDW0S3;
unsigned long FMC_TAGVDW0S4;
unsigned long FMC_TAGVDW0S5;
unsigned long FMC_TAGVDW0S6;
unsigned long FMC_TAGVDW0S7;
unsigned long FMC_TAGVDW1S0;
unsigned long FMC_TAGVDW1S1;
unsigned long FMC_TAGVDW1S2;
unsigned long FMC_TAGVDW1S3;
unsigned long FMC_TAGVDW1S4;
unsigned long FMC_TAGVDW1S5;
unsigned long FMC_TAGVDW1S6;
unsigned long FMC_TAGVDW1S7;
unsigned long FMC_TAGVDW2S0;
unsigned long FMC_TAGVDW2S1;
unsigned long FMC_TAGVDW2S2;
unsigned long FMC_TAGVDW2S3;
unsigned long FMC_TAGVDW2S4;
unsigned long FMC_TAGVDW2S5;
unsigned long FMC_TAGVDW2S6;
unsigned long FMC_TAGVDW2S7;
unsigned long FMC_TAGVDW3S0;
unsigned long FMC_TAGVDW3S1;
unsigned long FMC_TAGVDW3S2;
unsigned long FMC_TAGVDW3S3;
unsigned long FMC_TAGVDW3S4;
unsigned long FMC_TAGVDW3S5;
unsigned long FMC_TAGVDW3S6;
unsigned long FMC_TAGVDW3S7;
unsigned long ulRes1[32];
unsigned long FMC_DATAW0S0U;
unsigned long FMC_DATAW0S0L;
unsigned long FMC_DATAW0S1U;
unsigned long FMC_DATAW0S1L;
unsigned long FMC_DATAW0S2U;
unsigned long FMC_DATAW0S2L;
unsigned long FMC_DATAW0S3U;
unsigned long FMC_DATAW0S3L;
unsigned long FMC_DATAW0S4U;
unsigned long FMC_DATAW0S4L;
unsigned long FMC_DATAW0S5U;
unsigned long FMC_DATAW0S5L;
unsigned long FMC_DATAW0S6U;
unsigned long FMC_DATAW0S6L;
unsigned long FMC_DATAW0S7U;
unsigned long FMC_DATAW0S7L;
unsigned long FMC_DATAW1S0U;
unsigned long FMC_DATAW1S0L;
unsigned long FMC_DATAW1S1U;
unsigned long FMC_DATAW1S1L;
unsigned long FMC_DATAW1S2U;
unsigned long FMC_DATAW1S2L;
unsigned long FMC_DATAW1S3U;
unsigned long FMC_DATAW1S3L;
unsigned long FMC_DATAW1S4U;
unsigned long FMC_DATAW1S4L;
unsigned long FMC_DATAW1S5U;
unsigned long FMC_DATAW1S5L;
unsigned long FMC_DATAW1S6U;
unsigned long FMC_DATAW1S6L;
unsigned long FMC_DATAW1S7U;
unsigned long FMC_DATAW1S7L;
unsigned long FMC_DATAW2S0U;
unsigned long FMC_DATAW2S0L;
unsigned long FMC_DATAW2S1U;
unsigned long FMC_DATAW2S1L;
unsigned long FMC_DATAW2S2U;
unsigned long FMC_DATAW2S2L;
unsigned long FMC_DATAW2S3U;
unsigned long FMC_DATAW2S3L;
unsigned long FMC_DATAW2S4U;
unsigned long FMC_DATAW2S4L;
unsigned long FMC_DATAW2S5U;
unsigned long FMC_DATAW2S5L;
unsigned long FMC_DATAW2S6U;
unsigned long FMC_DATAW2S6L;
unsigned long FMC_DATAW2S7U;
unsigned long FMC_DATAW2S7L;
unsigned long FMC_DATAW3S0U;
unsigned long FMC_DATAW3S0L;
unsigned long FMC_DATAW3S1U;
unsigned long FMC_DATAW3S1L;
unsigned long FMC_DATAW3S2U;
unsigned long FMC_DATAW3S2L;
unsigned long FMC_DATAW3S3U;
unsigned long FMC_DATAW3S3L;
unsigned long FMC_DATAW3S4U;
unsigned long FMC_DATAW3S4L;
unsigned long FMC_DATAW3S5U;
unsigned long FMC_DATAW3S5L;
unsigned long FMC_DATAW3S6U;
unsigned long FMC_DATAW3S6L;
unsigned long FMC_DATAW3S7U;
unsigned long FMC_DATAW3S7L;
} KINETIS_FMC;

#if defined KINETIS_KE
typedef struct stKINETIS_IRQ                                             // {24}
{
    volatile unsigned char IRQ_SC;
} KINETIS_IRQ;
#endif

#if defined KINETIS_KL || defined KINETIS_KE
typedef struct stKINETIS_SPI
{
    #if defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43 || defined KINETIS_KL46 // supporting 16 bit words
        unsigned char SPI_S;
        unsigned char SPI_BR;
        unsigned char SPI_C2;
        unsigned char SPI_C1;
        unsigned char SPI_ML;
        unsigned char SPI_MH;
        unsigned char SPI_DL;
        unsigned char SPI_DH;
        unsigned char ucRes0;
        unsigned char SPI_CI;
        unsigned char SPI_C3;
    #elif defined KINETIS_KL03
        unsigned char SPI_S;
        unsigned char SPI_BR;
        unsigned char SPI_C2;
        unsigned char SPI_C1;
        unsigned char SPI_M;
        unsigned char ucRes0;
        unsigned char SPI_D;
    #else
        unsigned char SPI_C1;
        unsigned char SPI_C2;
        unsigned char SPI_BR;
        unsigned char SPI_S;
        unsigned char ucRes0;
        unsigned char SPI_D;
        unsigned char ucRes1;
        unsigned char SPI_M;
    #endif
} KINETIS_SPI;
#else
typedef struct stKINETIS_DSPI
{
unsigned long SPI_MCR;
unsigned long ulRes0;
unsigned long SPI_TCR;
unsigned long SPI_CTAR0;
unsigned long SPI_CTAR1;
unsigned long ulRes1[6];
unsigned long SPI_SR;
unsigned long SPI_RSER;
unsigned long SPI_PUSHR;
unsigned long SPI_POPR;
unsigned long SPI_TXFR0;
unsigned long SPI_TXFR1;
unsigned long SPI_TXFR2;
unsigned long SPI_TXFR3;
unsigned long ulRes2[12];
unsigned long SPI_RXFR0;
unsigned long SPI_RXFR1;
unsigned long SPI_RXFR2;
unsigned long SPI_RXFR3;
} KINETIS_DSPI;
#endif

#if defined KINETIS_KE
typedef struct stKINETIS_KBI                                             // {22}
{
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
    unsigned long KBI_PE;
    unsigned long KBI_ES;
    unsigned long KBI_SC;
    unsigned long KBI_SP;
    #else
    unsigned char KBI_SC;
    unsigned char KBI_PE;
    unsigned char KBI_ES;
    #endif
} KINETIS_KBI;

    #if defined MSCAN_CAN_INTERFACE
typedef struct stKINETIS_MSCAN                                           // {27}
{
    unsigned char MSCAN_CANCTL0;
    unsigned char MSCAN_CANCTL1;
    unsigned char MSCAN_CANBTR0;
    unsigned char MSCAN_CANBTR1;
    unsigned char MSCAN_CANRFLG;
    unsigned char MSCAN_CANRIER;
    unsigned char MSCAN_CANTFLG;
    unsigned char MSCAN_CANTIER;
    unsigned char MSCAN_CANTARQ;
    unsigned char MSCAN_CANTAAK;
    unsigned char MSCAN_CANTBSE;
    unsigned char MSCAN_CANIDAC;
    unsigned char MSCAN_CANMISC;
    unsigned char MSCAN_CANRXERR;
    unsigned char ucRes0;
    unsigned char MSCAN_CANTXERR;
    unsigned char MSCAN_CANIDAR0;
    unsigned char MSCAN_CANIDAR1;
    unsigned char MSCAN_CANIDAR2;
    unsigned char MSCAN_CANIDAR3;
    unsigned char MSCAN_CANIDMR0;
    unsigned char MSCAN_CANIDMR1;
    unsigned char MSCAN_CANIDMR2;
    unsigned char MSCAN_CANIDMR3;
    unsigned char MSCAN_CANIDAR4;
    unsigned char MSCAN_CANIDAR5;
    unsigned char MSCAN_CANIDAR6;
    unsigned char MSCAN_CANIDAR7;
    unsigned char MSCAN_CANIDMR4;
    unsigned char MSCAN_CANIDMR5;
    unsigned char MSCAN_CANIDMR6;
    unsigned char MSCAN_CANIDMR7;
    unsigned char MSCAN_REIDR0;
    unsigned char MSCAN_RSIDR0;
    unsigned char MSCAN_REIDR1;
    unsigned char MSCAN_RSIDR1;
    unsigned char MSCAN_REIDR2;
    unsigned char MSCAN_REIDR3;
    unsigned char MSCAN_REDSR0;
    unsigned char MSCAN_REDSR1;
    unsigned char MSCAN_REDSR2;
    unsigned char MSCAN_REDSR3;
    unsigned char MSCAN_REDSR4;
    unsigned char MSCAN_REDSR5;
    unsigned char MSCAN_REDSR6;
    unsigned char MSCAN_REDSR7;
    unsigned char MSCAN_RDLR;
    unsigned char ucRes1;
    unsigned char MSCAN_RTSRH;
    unsigned char MSCAN_RTSRL;
    unsigned char MSCAN_TEIDR0;
    unsigned char MSCAN_TSIDR0;
    unsigned char MSCAN_TEIDR1;
    unsigned char MSCAN_TSIDR1;
    unsigned char MSCAN_TEIDR2;
    unsigned char MSCAN_TEIDR3;
    unsigned char MSCAN_TEDSR0;
    unsigned char MSCAN_TEDSR1;
    unsigned char MSCAN_TEDSR2;
    unsigned char MSCAN_TEDSR3;
    unsigned char MSCAN_TEDSR4;
    unsigned char MSCAN_TEDSR5;
    unsigned char MSCAN_TEDSR6;
    unsigned char MSCAN_TEDSR7;
    unsigned char MSCAN_TDLR;
    unsigned char MSCAN_TBPR;
    unsigned char MSCAN_TTSRH;
    unsigned char MSCAN_TTSRL;
} KINETIS_MSCAN;
    #endif
#endif

typedef struct stKINETIC_CRC                                             // {6}
{
unsigned long CRC_CRC;
unsigned long CRC_GPOLY;
unsigned long CRC_CTRL;
} KINETIC_CRC;

typedef struct stKINETIS_LLWU                                            // {6}
{
#if defined KINETIS_KL03
    unsigned char LLWU_PE1;
    unsigned char LLWU_PE2;
    unsigned char LLWU_ME;
    unsigned char LLWU_F1;
    unsigned char LLWU_F3;
    unsigned char LLWU_FILT1;
    unsigned char LLWU_FILT2;
#else
    unsigned char LLWU_PE1;
    unsigned char LLWU_PE2;
    unsigned char LLWU_PE3;
    unsigned char LLWU_PE4;
    unsigned char LLWU_ME;
    unsigned char LLWU_F1;
    unsigned char LLWU_F2;
    unsigned char LLWU_F3;
    unsigned char LLWU_FILT1;
    unsigned char LLWU_FILT2;
    unsigned char LLWU_RST;
#endif
} KINETIS_LLWU;

typedef struct stKINETIS_PMC
{
unsigned char PMC_LVDSC1;
unsigned char PMC_LVDSC2;
unsigned char PMC_REGSC;
} 
KINETIS_PMC;

#if defined HS_USB_AVAILABLE
typedef struct stKINETIC_USBHS                                           // {8}
{
unsigned long USBHS_ID;
unsigned long USBHS_HWGENERAL;
unsigned long USBHS_HWHOST;
unsigned long USBHS_HWDEVICE;
unsigned long USBHS_HWTXBUF;
unsigned long USBHS_HWRXBUF;
unsigned long ulRes1[0x1a];
unsigned long USBHS_GPTIMER0LD;
unsigned long USBHS_GPTIMER0CTL;
unsigned long USBHS_GPTIMER1LD;
unsigned long USBHS_GPTIMER1CTL;
unsigned long USBHS_USB_SBUSCFG;
unsigned long ulRes2[0x1b];
unsigned long USBHS_HCIVERSION;
unsigned long USBHS_HCSPARAMS;
unsigned long USBHS_HCCPARAMS;
unsigned long ulRes3[0x5];
unsigned short usRes4;
unsigned short USBHS_DCIVERSION;
unsigned long USBHS_DCCPARAMS;
unsigned long ulRes5[0x6];
unsigned long USBHS_USBCMD;
unsigned long USBHS_USBSTS;
unsigned long USBHS_USBINTR;
unsigned long USBHS_FRINDEX;
unsigned long ulRes6;
unsigned long USBHS_PERIODICLISTBASE_DEVICEADDR;
unsigned long USBHS_ASYNCLISTADDR_EPLISTADDR;
unsigned long USBHS_TTCTRL;
unsigned long USBHS_BURSTSIZE;
unsigned long USBHS_TXFILLTUNING;
unsigned long ulRes7[0x2];
unsigned long USBHS_ULPI_VIEWPORT;
unsigned long ulRes8;
unsigned long USBHS_ENDPTNAK;
unsigned long USBHS_ENDPTNAKEN;
unsigned long USBHS_CONFIGFLAG;
unsigned long USBHS_PORTSC1;
unsigned long ulRes9[0x7];
unsigned long USBHS_OTGSC;
unsigned long USBHS_USBMODE;
unsigned long USBHS_EPSETUPSR;
unsigned long USBHS_EPPRIME;
unsigned long USBHS_EPFLUSH;
unsigned long USBHS_EPSR;
unsigned long USBHS_EPCOMPLETE;
unsigned long USBHS_EPCR0;
unsigned long USBHS_EPCR1;
unsigned long USBHS_EPCR2;
unsigned long USBHS_EPCR3;
unsigned long USBHS_EPCR4;
unsigned long USBHS_EPCR5;
unsigned long USBHS_EPCR6;
unsigned long USBHS_EPCR7;
unsigned long USBHS_EPCR8;
unsigned long USBHS_EPCR9;
unsigned long USBHS_EPCR10;
unsigned long USBHS_EPCR11;
unsigned long USBHS_EPCR12;
unsigned long USBHS_EPCR13;
unsigned long USBHS_EPCR14;
unsigned long USBHS_EPCR15;
unsigned long USBHS_USBGENCTRL;
} KINETIC_USBHS;

    #if defined KINETIS_WITH_USBPHY
typedef struct stKINETIC_USBPHY                                          // {29}
{
unsigned long USBPHY_PWD;
unsigned long USBPHY_PWD_SET;
unsigned long USBPHY_PWD_CLR;
unsigned long USBPHY_PWD_TOG;
unsigned long USBPHY_TX;
unsigned long USBPHY_TX_SET;
unsigned long USBPHY_TX_CLR;
unsigned long USBPHY_TX_TOG;
unsigned long USBPHY_RX;
unsigned long USBPHY_RX_SET;
unsigned long USBPHY_RX_CLR;
unsigned long USBPHY_RX_TOG;
unsigned long USBPHY_CTRL;
unsigned long USBPHY_CTRL_SET;
unsigned long USBPHY_CTRL_CLR;
unsigned long USBPHY_CTRL_TOG;
unsigned long USBPHY_STATUS;
unsigned long ulRes0[3];
unsigned long USBPHY_DEBUG;
unsigned long USBPHY_DEBUG_SET;
unsigned long USBPHY_DEBUG_CLR;
unsigned long USBPHY_DEBUG_TOG;
unsigned long USBPHY_DEBUG0_STATUS;
unsigned long ulRes1[3];
unsigned long USBPHY_DEBUG1;
unsigned long USBPHY_DEBUG1_SET;
unsigned long USBPHY_DEBUG1_CLR;
unsigned long USBPHY_DEBUG1_TOG;
unsigned long USBPHY_VERSION;
unsigned long ulRes2[7];
unsigned long USBPHY_PLL_SIC;
unsigned long USBPHY_PLL_SIC_SET;
unsigned long USBPHY_PLL_SIC_CLR;
unsigned long USBPHY_PLL_SIC_TOG;
unsigned long ulRes3[4];
unsigned long USBPHY_USB1_VBUS_DETECT;
unsigned long USBPHY_USB1_VBUS_DETECT_SET;
unsigned long USBPHY_USB1_VBUS_DETECT_CLR;
unsigned long USBPHY_USB1_VBUS_DETECT_TOG;
unsigned long USBPHY_USB1_VBUS_DET_STAT;
unsigned long ulRes4[7];
unsigned long USBPHY_USB1_VBUS_CHRG_DET_STAT;
unsigned long ulRes5[3];
unsigned long USBPHY_ANACTRL;
unsigned long USBPHY_ANACTRL_SET;
unsigned long USBPHY_ANACTRL_CLR;
unsigned long USBPHY_ANACTRL_TOG;
unsigned long USBPHY_USB1_LOOPBACK;
unsigned long USBPHY_USB1_LOOPBACK_SET;
unsigned long USBPHY_USB1_LOOPBACK_CLR;
unsigned long USBPHY_USB1_LOOPBACK_TOG;
unsigned long USBPHY_USB1_LOOPBACK_HSFSCNT;
unsigned long USBPHY_USB1_LOOPBACK_HSFSCNT_SET;
unsigned long USBPHY_USB1_LOOPBACK_HSFSCNT_CLR;
unsigned long USBPHY_USB1_LOOPBACK_HSFSCNT_TOG;
unsigned long USBPHY_TRIM_OVERRIDE_EN;
unsigned long USBPHY_TRIM_OVERRIDE_EN_SET;
unsigned long USBPHY_TRIM_OVERRIDE_EN_CLR;
unsigned long USBPHY_TRIM_OVERRIDE_EN_TOG;
} KINETIC_USBPHY;
    #endif
#endif

typedef struct stKINETIS_FTFL
{
#if defined KINETIS_KE
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        unsigned char ucRes0;
        unsigned char FTMRH_FCCOBIX;
        unsigned char FTMRH_FSEC;
        unsigned char FTMRH_FCLKDIV;
        unsigned char ucRes1;
        unsigned char FTMRH_FSTAT;
        unsigned char ucRes2;
        unsigned char FTMRH_FCNFG;
        unsigned char FTMRH_FCCOBLO;
        unsigned char FTMRH_FCCOBHI;
        unsigned char ucRes3;
        unsigned char FTMRH_FPROT;
        unsigned char ucRes4[3];
        unsigned char FTMRH_FOPT;
    #else
        unsigned char FTMRH_FCLKDIV;
        unsigned char FTMRH_FSEC;
        unsigned char FTMRH_FCCOBIX;
        unsigned char ucRes0;
        unsigned char FTMRH_FCNFG;
        unsigned char FTMRH_FERCNFG;
        unsigned char FTMRH_FSTAT;
        unsigned char FTMRH_FERSTAT;
        unsigned char FTMRH_FPROT;
        unsigned char FTMRH_EEPROT;
        unsigned char FTMRH_FCCOBHI;
        unsigned char FTMRH_FCCOBLO;
        unsigned char FTMRH_FOPT;
    #endif
#else
    unsigned char FTFL_FSTAT;
    unsigned char FTFL_FCNFG;
    unsigned char FTFL_FSEC;
    unsigned char FTFL_FOPT;
    unsigned char FTFL_FCCOB3;
    unsigned char FTFL_FCCOB2;
    unsigned char FTFL_FCCOB1;
    unsigned char FTFL_FCCOB0;
    unsigned char FTFL_FCCOB7;
    unsigned char FTFL_FCCOB6;
    unsigned char FTFL_FCCOB5;
    unsigned char FTFL_FCCOB4;
    unsigned char FTFL_FCCOBB;
    unsigned char FTFL_FCCOBA;
    unsigned char FTFL_FCCOB9;
    unsigned char FTFL_FCCOB8;
    unsigned char FTFL_FPROT3;
    unsigned char FTFL_FPROT2;
    unsigned char FTFL_FPROT1;
    unsigned char FTFL_FPROT0;
    unsigned char ucRes0[2];
    unsigned char FTFL_FEPROT;
    unsigned char FTFL_FDPROT;
#endif
} KINETIS_FTFL;


typedef struct stKINETIS_DMAMUX
{
unsigned char DMAMUX_CHCFG0;
unsigned char DMAMUX_CHCFG1;
unsigned char DMAMUX_CHCFG2;
unsigned char DMAMUX_CHCFG3;
#if !defined KINETIS_KL
    unsigned char DMAMUX_CHCFG4;
    unsigned char DMAMUX_CHCFG5;
    unsigned char DMAMUX_CHCFG6;
    unsigned char DMAMUX_CHCFG7;
    unsigned char DMAMUX_CHCFG8;
    unsigned char DMAMUX_CHCFG9;
    unsigned char DMAMUX_CHCFG10;
    unsigned char DMAMUX_CHCFG11;
    unsigned char DMAMUX_CHCFG12;
    unsigned char DMAMUX_CHCFG13;
    unsigned char DMAMUX_CHCFG14;
    unsigned char DMAMUX_CHCFG15;
#endif
} KINETIS_DMAMUX;

#if I2S_AVAILABLE > 0
typedef struct st_KINETIS_I2S                                            // {28}
{
unsigned long I2S_TCSR;
unsigned long ulRes0;
unsigned long I2S_TCR2;
unsigned long I2S_TCR3;
unsigned long I2S_TCR4;
unsigned long I2S_TCR5;
unsigned long ulRes1[2];
volatile unsigned long I2S_TDR0;
unsigned long ulRes2[15];
unsigned long I2S_TMR;
unsigned long ulRes3[7];
unsigned long I2S_RCSR;
unsigned long ulRes4;
unsigned long I2S_RCR2;
unsigned long I2S_RCR3;
unsigned long I2S_RCR4;
unsigned long I2S_RCR5;
unsigned long ulRes5[2];
volatile unsigned long I2S_RDR0;
unsigned long ulRes6[15];
unsigned long I2S_RMR;
unsigned long ulRes7[7];
unsigned long I2S_MCR;
} KINETIS_I2S;
#endif

#if defined PWT_AVAILABLE
typedef struct st_KINETIS_PWT                                            // {31}
{
    unsigned long PWT_R1;
    unsigned long PWT_R2;
} KINETIS_PWT;
#endif

typedef struct stKINETIS_PDB                                             // {12}
{
unsigned long PDB_SC;
unsigned long PDB_MOD;
unsigned long PDB_CNT;
unsigned long PDB_IDLY;
unsigned long PDB_CH0C1;
unsigned long PDB_CH0S;
unsigned long PDB_CH0DLY0;
unsigned long PDB_CH0DLY1;
unsigned long ulRes0[0x6];
unsigned long PDB_CH1C1;
unsigned long PDB_CH1S;
unsigned long PDB_CH1DLY0;
unsigned long PDB_CH1DLY1;
#if defined ADC_CONTROLLERS > 2
    unsigned long ulRes0a[6];
    unsigned long PDB_CH2C1;
    unsigned long PDB_CH2S;
    unsigned long PDB_CH2DLY0;
    unsigned long PDB_CH2DLY1;
    unsigned long ulRes0b[6];
    unsigned long PDB_CH3C1;
    unsigned long PDB_CH3S;
    unsigned long PDB_CH3DLY0;
    unsigned long PDB_CH3DLY1;
    unsigned long ulRes1[0x2e];
#else
    unsigned long ulRes1[0x42];
#endif
unsigned long PDB_DACINTC0;
unsigned long PDB_DACINT0;
unsigned long PDB_DACINTC1;
unsigned long PDB_DACINT1;
unsigned long ulRes2[0xc];
unsigned long PDB_POEN;
unsigned long PDB_PO0DLY;
unsigned long PDB_PO1DLY;
unsigned long PDB_PO2DLY;
} KINETIS_PDB;


typedef struct stKINETIS_PIT
{
unsigned long PIT_MCR;
#if defined KINETIS_KL
    unsigned long ulRes0[55];
    unsigned long PIT_LTMR64H;
    unsigned long PIT_LTMR64L;
    unsigned long ulRes1[6];
#else
    unsigned long ulRes0[63];
#endif
unsigned long PIT_LDVAL0;
unsigned long PIT_CVAL0;
unsigned long PIT_TCTRL0;
unsigned long PIT_TFLG0;
unsigned long PIT_LDVAL1;
unsigned long PIT_CVAL1;
unsigned long PIT_TCTRL1;
unsigned long PIT_TFLG1;
#if !defined KINETIS_KL                                                  // {15}
    unsigned long PIT_LDVAL2;
    unsigned long PIT_CVAL2;
    unsigned long PIT_TCTRL2;
    unsigned long PIT_TFLG2;
    unsigned long PIT_LDVAL3;
    unsigned long PIT_CVAL3;
    unsigned long PIT_TCTRL3;
    unsigned long PIT_TFLG3;
#endif
} KINETIS_PIT;


typedef struct stKINETIS_FTM
{
unsigned long FTM_SC;
unsigned long FTM_CNT;
unsigned long FTM_MOD;
unsigned long FTM_C0SC;
unsigned long FTM_C0V;
unsigned long FTM_C1SC;
unsigned long FTM_C1V;
unsigned long FTM_C2SC;
unsigned long FTM_C2V;
unsigned long FTM_C3SC;
unsigned long FTM_C3V;
unsigned long FTM_C4SC;
unsigned long FTM_C4V;
unsigned long FTM_C5SC;
unsigned long FTM_C5V;
#if defined KINETIS_KL || defined KINETIS_KE
    unsigned long ulRes0[6];
    unsigned long FTM_STATUS;
    unsigned long ulRes1[12];
    unsigned long FTM_CONF;
#else
    unsigned long FTM_C6SC;
    unsigned long FTM_C6V;
    unsigned long FTM_C7SC;
    unsigned long FTM_C7V;
    unsigned long FTM_CNTIN;
    unsigned long FTM_STATUS;
    unsigned long FTM_MODE;
    unsigned long FTM_SYNC;
    unsigned long FTM_OUTINIT;
    unsigned long FTM_OUTMASK;
    unsigned long FTM_COMBINE;
    unsigned long FTM_DEADTIME;
    unsigned long FTM_EXTTRIG;
    unsigned long FTM_POL;
    unsigned long FTM_FMS;
    unsigned long FTM_FILTER;
    unsigned long FTM_FLTCTRL;
    unsigned long FTM_QDCTRL;
    unsigned long FTM_CONF;
    unsigned long FTM_FLTPOL;
    unsigned long FTM_SYNCONF;
    unsigned long FTM_INVCTRL;
    unsigned long FTM_SWOCTRL;
    unsigned long FTM_PWMLOAD;
#endif
} KINETIS_FTM;


typedef struct stKINETIS_ADC
{
#if defined KINETIS_KE
    unsigned long ADC_SC1;
    unsigned long ADC_SC2;
    unsigned long ADC_SC3;
    unsigned long ADC_SC4;
    unsigned long ADC_R;
    unsigned long ADC_CV;
    unsigned long ADC_APCTL1;
#else
    unsigned long ADC_SC1A;
    unsigned long ADC_SC1B;
    unsigned long ADC_CFG1;
    unsigned long ADC_CFG2;
    unsigned long ADC_RA;
    unsigned long ADC_RB;
    unsigned long ADC_CV1;
    unsigned long ADC_CV2;
    unsigned long ADC_SC2;
    unsigned long ADC_SC3;
    unsigned long ADC_OFS;
    unsigned long ADC_PG;
    unsigned long ADC_MG;
    unsigned long ADC_CLPD;
    unsigned long ADC_CLPS;
    unsigned long ADC_CLP4;
    unsigned long ADC_CLP3;
    unsigned long ADC_CLP2;
    unsigned long ADC_CLP1;
    unsigned long ADC_CLP0;
    unsigned long ADC_PGA;
    unsigned long ADC_CLMD;
    unsigned long ADC_CLMS;
    unsigned long ADC_CLM4;
    unsigned long ADC_CLM3;
    unsigned long ADC_CLM2;
    unsigned long ADC_CLM1;
    unsigned long ADC_CLM0;
#endif
} KINETIS_ADC;

typedef struct stKINETIS_RTC
{
#if defined KINETIS_KE
    unsigned long RTC_SC;
    unsigned long RTC_MOD;
    unsigned long RTC_CNT;
#else
    unsigned long RTC_TSR;
    unsigned long RTC_TPR;
    unsigned long RTC_TAR;
    unsigned long RTC_TCR;
    unsigned long RTC_CR;
    unsigned long RTC_SR;
    unsigned long RTC_LR;
    unsigned long RTC_IER;
    #if !defined KINETIS_KL
        unsigned long ulRes[504];
        unsigned long RTC_WAR;
        unsigned long RTC_RAR;
    #endif
#endif
} KINETIS_RTC;

 #if !defined KINETIS_KE
typedef struct stKINETIS_LPTMR                                           // {20}
{
unsigned long LPTMR_CSR;
unsigned long LPTMR_PSR;
unsigned long LPTMR_CMR;
unsigned long LPTMR_CNR;
} KINETIS_LPTMR;
#endif

#if !defined CROSSBAR_SWITCH_LITE
typedef struct stKINETIS_AXBS                                            // {18}
{
unsigned long AXBS_PRS0;
unsigned long ulRes0a[3];
unsigned long AXBS_CRS0;
unsigned long ulRes0b[59];
unsigned long AXBS_PRS1;
unsigned long ulRes1a[3];
unsigned long AXBS_CRS1;
unsigned long ulRes1b[59];
unsigned long AXBS_PRS2;
unsigned long ulRes2a[3];
unsigned long AXBS_CRS2;
unsigned long ulRes2b[59];
unsigned long AXBS_PRS3;
unsigned long ulRes3a[3];
unsigned long AXBS_CRS3;
unsigned long ulRes03b[59];
unsigned long AXBS_PRS4;
unsigned long ulRes4a[3];
unsigned long AXBS_CRS4;
unsigned long ulRes4b[59];
unsigned long AXBS_PRS5;
unsigned long ulRes5a[3];
unsigned long AXBS_CRS5;
unsigned long ulRes5b[59];
unsigned long AXBS_PRS6;
unsigned long ulRes6a[3];
unsigned long AXBS_CRS6;
unsigned long ulRes6b[59];
unsigned long AXBS_PRS7;
unsigned long ulRes7a[3];
unsigned long AXBS_CRS7;
unsigned long ulRes7b[59];
unsigned long AXBS_MGPCR0;
unsigned long ulResm0[63];
unsigned long AXBS_MGPCR1;
unsigned long ulResm1[63];
unsigned long AXBS_MGPCR2;
unsigned long ulResm2[63];
unsigned long AXBS_MGPCR3;
unsigned long ulResm3[63];
unsigned long AXBS_MGPCR4;
unsigned long ulResm4[63];
unsigned long AXBS_MGPCR5;
unsigned long ulResm5[63];
unsigned long AXBS_MGPCR6;
unsigned long ulResm6[63];
unsigned long AXBS_MGPCR7;
} KINETIS_AXBS;
#endif

typedef struct stKINETIS_TSI
{
#if defined KINETIS_KL
    unsigned long TSI_GENCS;
    unsigned long TSI_DATA;
    unsigned long TSI_TSHD;
#else
    unsigned long TSI_GENCS;
    unsigned long TSI_SCANC;
    unsigned long TSI_PEN;
    unsigned long TSI_STATUS;
    unsigned long ulRes0[60];
    unsigned long TSI_CNTR1;
    unsigned long TSI_CNTR3;
    unsigned long TSI_CNTR5;
    unsigned long TSI_CNTR7;
    unsigned long TSI_CNTR9;
    unsigned long TSI_CNTR11;
    unsigned long TSI_CNTR13;
    unsigned long TSI_CNTR15;
    unsigned long TSI_THRESHLD0;
    #if KINETIS_MAX_SPEED >= 100000000
        unsigned long TSI_THRESHLD1;
        unsigned long TSI_THRESHLD2;
        unsigned long TSI_THRESHLD3;
        unsigned long TSI_THRESHLD4;
        unsigned long TSI_THRESHLD5;
        unsigned long TSI_THRESHLD6;
        unsigned long TSI_THRESHLD7;
        unsigned long TSI_THRESHLD8;
        unsigned long TSI_THRESHLD9;
        unsigned long TSI_THRESHLD10;
        unsigned long TSI_THRESHLD11;
        unsigned long TSI_THRESHLD12;
        unsigned long TSI_THRESHLD13;
        unsigned long TSI_THRESHLD14;
        unsigned long TSI_THRESHLD15;
    #endif
#endif
} KINETIS_TSI;

typedef struct stKINETIS_SIM
{
#if defined KINETIS_KE
    unsigned long SIM_SRSID;
    unsigned long SIM_SOPT0;
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        unsigned long SIM_SOPT1;
    #endif
    unsigned long SIM_PINSEL0;
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        unsigned long SIM_PINSEL1;
    #endif
    unsigned long SIM_SCGC;
    unsigned long SIM_UUIDL;
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        unsigned long SIM_UUIDML;
        unsigned long SIM_UUIDMH;
        unsigned long SIM_CLKDIV;
    #else
        unsigned long SIM_UUIDH;
        unsigned long SIM_BUSDIV;
    #endif
#else
    #if !defined KINETIS_KL02
        unsigned long SIM_SOPT1;
    #endif
    #if defined KINETIS_K_FPU || defined KINETIS_KL                      // {15}
        unsigned long SIM_SOPT1CGF;
        unsigned char ucRes0[0x1000 - 4];
    #else
        unsigned char ucRes0[0x1000];
    #endif
    unsigned long SIM_SOPT2;
    unsigned long ulRes1;
    unsigned long SIM_SOPT4;
    unsigned long SIM_SOPT5;
    #if defined KINETIS_KL
        unsigned long ulRes1a;
    #else
        unsigned long SIM_SOPT6;
    #endif
    unsigned long SIM_SOPT7;
    unsigned long ulRes2[2];
    unsigned long SIM_SDID;
    #if defined KINETIS_KL
        unsigned long ulRes2a[3];
    #else
        unsigned long SIM_SCGC1;
        unsigned long SIM_SCGC2;
        unsigned long SIM_SCGC3;
    #endif
    unsigned long SIM_SCGC4;
    unsigned long SIM_SCGC5;
    unsigned long SIM_SCGC6;
    unsigned long SIM_SCGC7;
    unsigned long SIM_CLKDIV1;
    #if defined KINETIS_KL && !defined KINETIS_KL82
        unsigned long ulRes2b;
    #else
        unsigned long SIM_CLKDIV2;
    #endif
    unsigned long SIM_FCFG1;
    unsigned long SIM_FCFG2;
    #if defined KINETIS_KL
        unsigned long ulRes2c;
    #else
        unsigned long SIM_UIDH;
    #endif
    unsigned long SIM_UIDMH;
    unsigned long SIM_UIDML;
    unsigned long SIM_UIDL;
    #if defined KINETIS_KL && !defined KINETIS_KL82                      // {15}
        unsigned long ulRes3[39];
        unsigned long SIM_COPC;
        unsigned long SIM_SRVCOPC;
    #elif defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000)
        unsigned long SIM_CLKDIV3;
        unsigned long SIM_CLKDIV4;
        unsigned long SIM_MCR;
    #endif
#endif
} KINETIS_SIM;

#if defined KINETIS_KE
    typedef struct stKINETIS_KE_PORT
    {
    #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA
        unsigned long PORT_IOFLT0;
        unsigned long PORT_IOFLT1;
        unsigned long PORT_PUE0;
        unsigned long PORT_PUE1;
        unsigned long PORT_PUE2;
        unsigned long PORT_HDRVE;
    #else
        unsigned long PORT_IOFLT;
        unsigned long PORT_PUEL;
        #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024)))
        unsigned long PORT_PUEH;
        #else
        unsigned long ulRes0;
        #endif
        unsigned long PORT_HDRVE;
    #endif
    } KINETIS_KE_PORT;
#else
    typedef struct stKINETIS_PORT
    {
    unsigned long PORT_PCR0;
    unsigned long PORT_PCR1;
    unsigned long PORT_PCR2;
    unsigned long PORT_PCR3;
    unsigned long PORT_PCR4;
    unsigned long PORT_PCR5;
    unsigned long PORT_PCR6;
    unsigned long PORT_PCR7;
    unsigned long PORT_PCR8;
    unsigned long PORT_PCR9;
    unsigned long PORT_PCR10;
    unsigned long PORT_PCR11;
    unsigned long PORT_PCR12;
    unsigned long PORT_PCR13;
    unsigned long PORT_PCR14;
    unsigned long PORT_PCR15;
    unsigned long PORT_PCR16;
    unsigned long PORT_PCR17;
    unsigned long PORT_PCR18;
    unsigned long PORT_PCR19;
    unsigned long PORT_PCR20;
    unsigned long PORT_PCR21;
    unsigned long PORT_PCR22;
    unsigned long PORT_PCR23;
    unsigned long PORT_PCR24;
    unsigned long PORT_PCR25;
    unsigned long PORT_PCR26;
    unsigned long PORT_PCR27;
    unsigned long PORT_PCR28;
    unsigned long PORT_PCR29;
    unsigned long PORT_PCR30;
    unsigned long PORT_PCR31;
    unsigned long PORT_GPCLR;
    unsigned long PORT_GPCHR;
    unsigned long ulRes0[6];
    unsigned long PORT_ISFR;
    unsigned long ulRes1[7];
    unsigned long PORT_DFER;
    unsigned long PORT_DFCR;
    unsigned long PORT_DFWR;
    unsigned long ulRef2[1024 - 51];
    } KINETIS_PORT;
#endif

#if !defined KINETIS_KL || defined KINETIS_KL82
    typedef struct stKINETIS_WDOG
    {
    #if defined KINETIS_KE
    unsigned char WDOG_CS1;
    unsigned char WDOG_CS2;
    unsigned char WDOG_CNTH;
    unsigned char WDOG_CNTL;
    unsigned char WDOG_TOVALH;
    unsigned char WDOG_TOVALL;
    unsigned char WDOG_WINH;
    unsigned char WDOG_WINL;
    #else
    unsigned short WDOG_STCTRLH;
    unsigned short WDOG_STCTRLL;
    unsigned short WDOG_TOVALH;
    unsigned short WDOG_TOVALL;
    unsigned short WDOG_WINH;
    unsigned short WDOG_WINL;
    unsigned short WDOG_REFRESH;
    unsigned short WDOG_UNLOCK;
    unsigned short WDOG_TMROUTH;
    unsigned short WDOG_TMROUTL;
    unsigned short WDOG_RSTCNT;
    unsigned short WDOG_PRESC;
    #endif
    } KINETIS_WDOG;
#endif

#if !defined KINETIS_KL && !defined KINETIS_KE
typedef struct stKINETIS_EWM
{
unsigned char EWM_CTRL;
unsigned char EWM_SERV;
unsigned char EWM_CMPL;
unsigned char EWM_CMPH;
} KINETIS_EWM;
#endif

#if defined KINETIS_KE
    typedef struct stKINETIS_ICS
    {
    unsigned char ICS_C1;
    unsigned char ICS_C2;
    unsigned char ICS_C3;
    unsigned char ICS_C4;
    unsigned char ICS_S;
    } KINETIS_ICS;
#elif defined KINETIS_WITH_SCG                                           // {32}
    typedef struct stKINETIS_SCG
    {
    unsigned long SCG_VERID;
    unsigned long SCG_PARAM;
    unsigned long ulRes0;
    unsigned long SCG_CSR;
    unsigned long SCG_RCCR;
    unsigned long SCG_VCCR;
    unsigned long SCG_HCCR;
    unsigned long SCG_CLKOUTCNFG;
    unsigned long ulRes1[56];
    unsigned long SCG_SOSCCSR;
    unsigned long SCG_SOSCDIV;
    unsigned long SCG_SOSCCFG;
    unsigned long ulRes2[62];
    unsigned long SCG_SIRCCSR;
    unsigned long SCG_SIRCDIV;
    unsigned long SCG_SIRCCFG;
    unsigned long ulRes3[62];
    unsigned long SCG_FIRCCSR;
    unsigned long SCG_FIRCDIV;
    unsigned long SCG_FIRCCFG;
    unsigned long ulRes4;
    unsigned long SCG_FIRCTCFG;
    unsigned long SCG_FIRCSTAT;
    unsigned long ulRes5[186];
    unsigned long SCG_SPPLCCSR;
    unsigned long SCG_SPPLCDIV;
    unsigned long SCG_SPPLCFG;
    } KINETIS_SCG;
#else
    typedef struct stKINETIS_MCG
    {
    unsigned char MCG_C1;
    unsigned char MCG_C2;
    #if defined KINETIS_WITH_MCG_LITE
    unsigned char ucRes0[4];
    #else
    unsigned char MCG_C3;
    unsigned char MCG_C4;
    unsigned char MCG_C5;
    unsigned char MCG_C6;
    #endif
    unsigned char MCG_S;
    unsigned char ucRes1;
    #if defined KINETIS_WITH_MCG_LITE
        unsigned char MCG_SC;
        unsigned char ucRes2[0x10];
        unsigned char MCG_MC;
    #else
        #if defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000)
        unsigned char MCG_SC;
        unsigned char ucRes2;
        unsigned char MCG_ATCVH;
        unsigned char MCG_ATCVL;
        unsigned char MCG_C7;
        unsigned char MCG_C8;
        unsigned char ucRes3;
        unsigned char MCG_C10;
        unsigned char MCG_C11;
        unsigned char MCG_C12;
        unsigned char MCG_S2;
        #else
        unsigned char MCG_ATCM;
        unsigned char ucRes2;
        unsigned char MCG_ATCVH;
        unsigned char MCG_ATCVL;
        #endif
    #endif
    } KINETIS_MCG;
#endif

#if defined KINETIS_K70 || (defined KINETIS_K60 && defined KINETIS_K_FPU)
typedef struct stKINETIS_NFC                                             // {1}
{
unsigned long NFC_CMD1;
unsigned long NFC_CMD2;
unsigned long NFC_CAR;
unsigned long NFC_RAR;
unsigned long NFC_RPT;
unsigned long NFC_RAI;
unsigned long NFC_SR1;
unsigned long NFC_SR2;
unsigned long NFC_DMA1;
unsigned long NFC_DMACFG;
unsigned long NFC_SWAP;
unsigned long NFC_SECSZ;
unsigned long NFC_CFG;
unsigned long NFC_DMA2;
unsigned long NFC_ISR;
} KINETIS_NFC;


typedef struct stKINETIS_DDR                                             // {10}
{
unsigned long DDR_CR00;
unsigned long DDR_CR01;
unsigned long DDR_CR02;
unsigned long DDR_CR03;
unsigned long DDR_CR04;
unsigned long DDR_CR05;
unsigned long DDR_CR06;
unsigned long DDR_CR07;
unsigned long DDR_CR08;
unsigned long DDR_CR09;
unsigned long DDR_CR10;
unsigned long DDR_CR11;
unsigned long DDR_CR12;
unsigned long DDR_CR13;
unsigned long DDR_CR14;
unsigned long DDR_CR15;
unsigned long DDR_CR16;
unsigned long DDR_CR17;
unsigned long DDR_CR18;
unsigned long DDR_CR19;
unsigned long DDR_CR20;
unsigned long DDR_CR21;
unsigned long DDR_CR22;
unsigned long DDR_CR23;
unsigned long DDR_CR24;
unsigned long DDR_CR25;
unsigned long DDR_CR26;
unsigned long DDR_CR27;
unsigned long DDR_CR28;
unsigned long DDR_CR29;
unsigned long DDR_CR30;
unsigned long DDR_CR31;
unsigned long DDR_CR32;
unsigned long DDR_CR33;
unsigned long DDR_CR34;
unsigned long DDR_CR35;
unsigned long DDR_CR36;
unsigned long DDR_CR37;
unsigned long DDR_CR38;
unsigned long DDR_CR39;
unsigned long DDR_CR40;
unsigned long DDR_CR41;
unsigned long DDR_CR42;
unsigned long DDR_CR43;
unsigned long DDR_CR44;
unsigned long DDR_CR45;
unsigned long DDR_CR46;
unsigned long DDR_CR47;
unsigned long DDR_CR48;
unsigned long DDR_CR49;
unsigned long DDR_CR50;
unsigned long DDR_CR51;
unsigned long DDR_CR52;
unsigned long DDR_CR53;
unsigned long DDR_CR54;
unsigned long DDR_CR55;
unsigned long DDR_CR56;
unsigned long DDR_CR57;
unsigned long DDR_CR58;
unsigned long DDR_CR59;
unsigned long DDR_CR60;
unsigned long DDR_CR61;
unsigned long DDR_CR62;
unsigned long DDR_CR63;
unsigned long ulRes1[32];
unsigned long DDR_RCR;
unsigned long ulRes2[10];
unsigned long DDR_PAD_CTRL;
} KINETIS_DDR;
#endif

#if defined KINETIS_K70
typedef struct stKINETIS_LCD                                             // {25}
{
unsigned long LCDC_LSSAR;
unsigned long LCDC_LSR;
unsigned long LCDC_LVPWR;
unsigned long LCDC_LCPR;
unsigned long LCDC_LCWHB;
unsigned long LCDC_LCCMR;
unsigned long LCDC_LPCR;
unsigned long LCDC_LHCR;
unsigned long LCDC_LVCR;
unsigned long LCDC_LPOR;
unsigned long ulRes0;
unsigned long LCDC_LPCCR;
unsigned long LCDC_LDCR;
unsigned long LCDC_LRMCR;
unsigned long LCDC_LICR;
unsigned long LCDC_LIER;
unsigned long LCDC_LISR;
unsigned long ulRes1[3];
unsigned long LCDC_LGWSAR;
unsigned long LCDC_LGWSR;
unsigned long LCDC_LGWVPWR;
unsigned long LCDC_LGWPOR;
unsigned long LCDC_LGWPR;
unsigned long LCDC_LGWCR;
unsigned long LCDC_LGWDCR;
unsigned long ulRes2[5];
unsigned long LCDC_LAUSCR;
unsigned long LCDC_LAUSCCR;
} KINETIS_LCD;
#endif

typedef struct stKINETIS_SDHC
{
unsigned long SDHC_DSADDR;
unsigned long SDHC_BLKATTR;
unsigned long SDHC_CMDARG;
unsigned long SDHC_XFERTYP;
unsigned long SDHC_CMDRSP0;
unsigned long SDHC_CMDRSP1;
unsigned long SDHC_CMDRSP2;
unsigned long SDHC_CMDRSP3;
unsigned long SDHC_DATPORT;
unsigned long SDHC_PRSSTAT;
unsigned long SDHC_PROCTL;
unsigned long SDHC_SYSCTL;
unsigned long SDHC_IRQSTAT;
unsigned long SDHC_IRQSTATEN;
unsigned long SDHC_IRQSIGEN;
unsigned long SDHC_AC12ERR;
unsigned long SDHC_HTCAPBLT;
unsigned long SDHC_WML;
unsigned long ulRes0[2];
unsigned long SDHC_FEVT;
unsigned long SDHC_ADMAES;
unsigned long SDHC_ADSADDR;
unsigned long ulRes1[25];
unsigned long SDHC_VENDOR;
unsigned long SDHC_MMCBOOT;
unsigned long ulRes2[13];
unsigned long SDHC_HOSTVER;
} KINETIS_SDHC;

typedef struct stKINETIS_OSC
{
unsigned char OSC_CR;
} KINETIS_OSC;

typedef struct stKINETIS_I2C
{
unsigned char I2C_A1;
unsigned char I2C_F;
unsigned char I2C_C1;
unsigned char I2C_S;
unsigned char I2C_D;
unsigned char I2C_C2;
unsigned char I2C_FLT;
unsigned char I2C_RA;
unsigned char I2C_SMB;
unsigned char I2C_A2;
unsigned char I2C_SLTH;
unsigned char I2C_SLTL;
} KINETIS_I2C;

typedef struct stKINETIS_UART
{
#if defined KINETIS_KL03
    unsigned long LPUART_BAUD;
    volatile unsigned long LPUART_STAT;
    volatile unsigned long LPUART_CTRL;
    volatile unsigned long LPUART_DATA;
    unsigned long LPUART_MATCH;
#else
    unsigned char UART_BDH;
    unsigned char UART_BDL;
    unsigned char UART_C1;
    unsigned char UART_C2;
    unsigned char UART_S1;
    unsigned char UART_S2;
    unsigned char UART_C3;
    unsigned char UART_D;
    #if defined KINETIS_KL
        unsigned char UART_MA1_C4;                                       // UART0 has full set but UARTs 1, 2 etc. have UART_C4 at this location and no futher registers (exception KL43)
        unsigned char UART_MA2;
        unsigned char UART_C4;
        unsigned char UART_C5;
    #elif !defined KINETIS_KE
        unsigned char UART_MA1;
        unsigned char UART_MA2;
        unsigned char UART_C4;
        unsigned char UART_C5;
    #endif
    #if !defined KINETIS_KL && !defined KINETIS_KE
        unsigned char UART_ED;
        unsigned char UART_MODEM;
        unsigned char UART_IR;
        unsigned char ucRes0;
        unsigned char UART_PFIFO;
        unsigned char UART_CFIFO;
        unsigned char UART_SFIFO;
        unsigned char UART_TWFIFO;
        unsigned char UART_TCFIFI;
        unsigned char UART_RWFIFO;
        unsigned char UART_RCFIFI;
        unsigned char ucRes1;
    #elif defined KINETIS_KL43
        unsigned char ucRes1[12];
    #endif
    #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined KINETIS_KL43
        unsigned char UART_C7816;
        unsigned char UART_IE7816;
        unsigned char UART_IS7816;
        unsigned char UART_WP7816T0;
        unsigned char UART_WP7816T1;
        unsigned char UART_WN7816;
        unsigned char UART_WF7816;
        unsigned char UART_ET7816;
        unsigned char UART_TL7816;
    #endif
#endif
} KINETIS_UART;

typedef struct stKINETIS_LPUART
{
    unsigned long LPUART_BAUD;
    volatile unsigned long LPUART_STAT;
    volatile unsigned long LPUART_CTRL;
    volatile unsigned long LPUART_DATA;
    unsigned long LPUART_MATCH;
} KINETIS_LPUART;

typedef struct stKINETIS_USB
{
unsigned char PER_ID;
unsigned char ucRes1[3];
unsigned char ID_COMP;
unsigned char ucRes2[3];
unsigned char REV;
unsigned char ucRes3[3];
unsigned char ADD_INFO;
unsigned char ucRes4[3];
unsigned char OTG_INT_STAT;
unsigned char ucRes5[3];
unsigned char OTG_INT_EN;
unsigned char ucRes6[3];
unsigned char OTG_STATUS;
unsigned char ucRes7[3];
unsigned char OTG_CTRL;
unsigned char ucRes8[0x63];
unsigned char INT_STAT;
unsigned char ucRes9[3];
unsigned char INT_ENB;
unsigned char ucRes10[3];
unsigned char ERR_STAT;
unsigned char ucRes11[3];
unsigned char ERR_ENG;
unsigned char ucRes12[3];
unsigned char STAT;
unsigned char ucRes13[3];
unsigned char CTL;
unsigned char ucRes14[3];
unsigned char ADDR;
unsigned char ucRes15[3];
unsigned char BDT_PAGE_01;
unsigned char ucRes16[3];
unsigned char FRM_NUML;
unsigned char ucRes17[3];
unsigned char FRM_NUMH;
unsigned char ucRes18[3];
unsigned char TOKEN;
unsigned char ucRes19[3];
unsigned char SOF_THLD;
unsigned char ucRes20[3];
unsigned char BDT_PAGE_02;
unsigned char ucRes21[3];
unsigned char BDT_PAGE_03;
unsigned char ucRes22[11];
unsigned char ENDPT0;
unsigned char ucRes23[3];
unsigned char ENDPT1;
unsigned char ucRes24[3];
unsigned char ENDPT2;
unsigned char ucRes25[3];
unsigned char ENDPT3;
unsigned char ucRes26[3];
unsigned char ENDPT4;
unsigned char ucRes27[3];
unsigned char ENDPT5;
unsigned char ucRes28[3];
unsigned char ENDPT6;
unsigned char ucRes29[3];
unsigned char ENDPT7;
unsigned char ucRes30[3];
unsigned char ENDPT8;
unsigned char ucRes31[3];
unsigned char ENDPT9;
unsigned char ucRes32[3];
unsigned char ENDPT10;
unsigned char ucRes33[3];
unsigned char ENDPT11;
unsigned char ucRes34[3];
unsigned char ENDPT12;
unsigned char ucRes35[3];
unsigned char ENDPT13;
unsigned char ucRes36[3];
unsigned char ENDPT14;
unsigned char ucRes37[3];
unsigned char ENDPT15;
unsigned char ucRes38[3];
unsigned char USB_CTRL;
unsigned char ucRes39[3];
unsigned char USB_OTG_OBSERVE;
unsigned char ucRes40[3];
unsigned char USB_OTG_CONTROL;
unsigned char ucRes41[3];
unsigned char USB_USBTRC0;
#if defined KINETIS_HAS_IRC48M
    unsigned char ucRes42[7];
    unsigned char USB_USBFRMADJUST;
    unsigned char ucRes43[43];
    unsigned char USB_CLK_RECOVER_CTRL;
    unsigned char ucRes44[3];
    unsigned char USB_CLK_RECOVER_IRC_EN;
    unsigned char ucRes45[23];
    unsigned char USB_CLK_RECOVER_INT_STATUS;
#endif
} KINETIS_USB;


typedef struct stKINETIS_MC
{
unsigned char MC_SRSH;      
unsigned char MC_SRSL;
unsigned char MC_PMPROT;  
unsigned char MC_PMCTRL;
} KINETIS_MC;

#if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000) // {16}

    typedef struct stKINETIS_SMC                                         // {21}
    {
    unsigned char SMC_PMPROT;
    unsigned char SMC_PMCTRL;
    #if defined KINETIS_KL || defined KINETIS_K22
        unsigned char SMC_STOPCTRL;
    #else
        unsigned char SMC_VLLSCTRL;
    #endif
    unsigned char SMC_PMSTAT;
    } KINETIS_SMC;

    typedef struct stKINETIS_RCM                                         // {5}
    {
    unsigned char RCM_SRS0;
    unsigned char RCM_SRS1;
    unsigned char ucRes0[2];
    unsigned char RCM_RPFC;
    unsigned char RCM_RPFW;
    #if defined ROM_BOOTLOADER
    unsigned char RCM_FM;
    #else
    unsigned char ucRes1;
    #endif
    unsigned char RCM_MR;
    } KINETIS_RCM;
#endif

#if defined KINETIS_KE                                                   // {30}
    typedef struct stKINETIS_ACMP                                        // analogue comparator
    {
    unsigned char ACMP_CS;
    unsigned char ACMP_C0;
    unsigned char ACMP_C1;
    unsigned char ACMP_C2;
    } KINETIS_ACMP;
#else
    typedef struct stKINETIS_CMP                                         // comparator
    {
    unsigned char CMP_CR0;
    unsigned char CMP_CR1;
    unsigned char CMP_FPR;
    unsigned char CMP_SCR;
    unsigned char CMP_DACCR;
    unsigned char CMP_MUXCR;
    } KINETIS_CMP;
#endif

#if !defined KINETIS_KL
    typedef struct stKINETIS_VREF                                        // VREF
    {
    unsigned char VREF_TRM;
    unsigned char VREF_SC;
    } KINETIS_VREF;
#endif

#if defined RNG_AVAILABLE
    #if defined RANDOM_NUMBER_GENERATOR_B                                // {13}
    typedef struct stKINETIS_RNGB
    {
    unsigned long RNG_VER;
    unsigned long RNG_CMD;
    unsigned long RNG_CR;
    unsigned long RNG_SR;
    unsigned long RNG_ESR;
    unsigned long RNG_OUT;
    } KINETIS_RNGB;
    #else
    typedef struct stKINETIS_RNGA
    {
    unsigned long RNG_CR;
    unsigned long RNG_SR;
    unsigned long RNG_ER;
    unsigned long RNG_OR;
    } KINETIS_RNGA;
    #endif
#endif

#if defined DEVICE_WITH_SLCD
typedef struct stKINETIS_SLCD
{
unsigned long LCD_GCR;
unsigned long LCD_AR;
unsigned long LCD_FDCR;
unsigned long LCD_FDSR;
unsigned long LCD_PENL;
unsigned long LCD_PENH;
unsigned long LCD_BPENL;
unsigned long LCD_BPENH;
unsigned long LCD_WF3TO0;
unsigned long LCD_WF7TO4;
unsigned long LCD_WF11TO8;
unsigned long LCD_WF15TO12;
unsigned long LCD_WF19TO16;
unsigned long LCD_WF23TO20;
unsigned long LCD_WF27TO24;
unsigned long LCD_WF31TO28;
unsigned long LCD_WF35TO32;
unsigned long LCD_WF39TO36;
unsigned long LCD_WF43TO40;
unsigned long LCD_WF47TO44;
unsigned long LCD_WF51TO48;
unsigned long LCD_WF55TO52;
unsigned long LCD_WF59TO56;
unsigned long LCD_WF63TO60;
} KINETIS_SLCD;
#endif


#if defined KINETIS_K52 || defined KINETIS_K53 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K70
typedef struct stKINETIS_EMAC
{
unsigned long ulRes;
unsigned long EIR;
unsigned long EIMR;
unsigned long ulRes0;
unsigned long RDAR;
unsigned long TDAR;
unsigned long ulRes1[3];
unsigned long ECR;
unsigned long ulRes2[6];
unsigned long MMFR;
unsigned long MSCR;
unsigned long ulRes3[7];
unsigned long MIBC;
unsigned long ulRes4[7];
unsigned long RCR;
unsigned long ulRes5[15];
unsigned long TCR;
unsigned long ulRes6[7];
unsigned long PALR;
unsigned long PAUR;
unsigned long OPD;
unsigned long ulRes7[10];
unsigned long IAUR;
unsigned long IALR;
unsigned long GAUR;
unsigned long GALR;
unsigned long ulRes8[7];
unsigned long ENET_TFWR;
unsigned long ulRes9[14];
unsigned long ERDSR;
unsigned long ETDSR;
unsigned long EMRBR;
unsigned long ulRes10;
unsigned long ENET_RSFL;
unsigned long ENET_RSEM;
unsigned long ENET_RAEM;
unsigned long ENET_RAFL;
unsigned long ENET_TSEM;
unsigned long ENET_TAEM;
unsigned long ENET_TAFL;
unsigned long ENET_TIPG;
unsigned long ENET_FTRL;
unsigned long ulRes11[3];
unsigned long ENET_TACC;
unsigned long ENET_RACC;
unsigned long ulRes12[14];
unsigned long ulMIB[512/sizeof (unsigned long)];                         // counters
unsigned long ENET_ATCR;
unsigned long ENET_ATVR;
unsigned long ENET_ATOFF;
unsigned long ENET_ATPER;
unsigned long ENET_ATCOR;
unsigned long ENET_ATINC;
unsigned long ENET_ATSTMP;
unsigned long ulRes13[122];
unsigned long ENET_TGSR;
unsigned long ENET_TCSR0;
unsigned long ENET_TCCR0;
unsigned long ENET_TCSR1;
unsigned long ENET_TCCR1;
unsigned long ENET_TCSR2;
unsigned long ENET_TCCR2;
unsigned long ENET_TCSR3;
unsigned long ENET_TCCR3;
} KINETIS_EMAC;
#endif

typedef struct stKINETIS_DAC
{
unsigned char DAC_DAT0L;
unsigned char DAC_DAT0H;
unsigned char DAC_DAT1L;
unsigned char DAC_DAT1H;
#if defined KINETIS_KL
    unsigned char ucRes0[28];
#else
    unsigned char DAC_DAT2L;
    unsigned char DAC_DAT2H;
    unsigned char DAC_DAT3L;
    unsigned char DAC_DAT3H;
    unsigned char DAC_DAT4L;
    unsigned char DAC_DAT4H;
    unsigned char DAC_DAT5L;
    unsigned char DAC_DAT5H;
    unsigned char DAC_DAT6L;
    unsigned char DAC_DAT6H;
    unsigned char DAC_DAT7L;
    unsigned char DAC_DAT7H;
    unsigned char DAC_DAT8L;
    unsigned char DAC_DAT8H;
    unsigned char DAC_DAT9L;
    unsigned char DAC_DAT9H;
    unsigned char DAC_DAT10L;
    unsigned char DAC_DAT10H;
    unsigned char DAC_DAT11L;
    unsigned char DAC_DAT11H;
    unsigned char DAC_DAT12L;
    unsigned char DAC_DAT12H;
    unsigned char DAC_DAT13L;
    unsigned char DAC_DAT13H;
    unsigned char DAC_DAT14L;
    unsigned char DAC_DAT14H;
    unsigned char DAC_DAT15L;
    unsigned char DAC_DAT15H;
#endif
unsigned char DAC_SR;
unsigned char DAC_C0;
unsigned char DAC_C1;
unsigned char DAC_C2;
} KINETIS_DAC;


typedef struct stKINETIS_GPIO
{
unsigned long GPIO_PDOR;
unsigned long GPIO_PSOR;
unsigned long GPIO_PCOR;
unsigned long GPIO_PTOR;
unsigned long GPIO_PDIR;
unsigned long GPIO_PDDR;
#if defined KINETIS_KE
    unsigned long GPIO_PIDR;
    unsigned long ulRes[9];
#else
    unsigned long ulRes[10];
#endif
} KINETIS_GPIO;

typedef struct stKINETIS_MCM                                             // {11}
{
unsigned long ulRes0[2];
unsigned short MCM_PLASC;
unsigned short MCM_PLAMC;
#if defined KINETIS_K02
    unsigned long MCM_PLACR;
    unsigned long MCM_ISCR;
    unsigned long ulRes1[12];
    unsigned long MCM_CPO;
#else
    unsigned long MCM_CR;
    unsigned long MCM_ISR;
    unsigned long MCM_ETBCC;
    unsigned long MCM_ETBRL;
    unsigned long MCM_ETBCNT;
    unsigned long ulRes1[4];
    unsigned long MCM_PID;
#endif
} KINETIS_MCM;

#if defined CAU_V1_AVAILABLE || defined CAU_V2_AVAILABLE
typedef struct stKINETIS_MMCAU                                           // {17}
{
unsigned long CAU_CASR;
unsigned long CAU_CAA;
unsigned long CAU_CA0;
unsigned long CAU_CA1;
unsigned long CAU_CA2;
unsigned long CAU_CA3;
unsigned long CAU_CA4;
unsigned long CAU_CA5;
    #if defined CAU_V2_AVAILABLE
    unsigned long CAU_CA6;
    unsigned long CAU_CA7;
    unsigned long CAU_CA8;
    #endif
} KINETIS_MMCAU;
#endif

typedef struct stKINETIS_CAN
{
unsigned long  CAN_MCR;
unsigned long  CAN_CTRL;
unsigned long  CAN_TIMER;
unsigned long  ulRes1;
unsigned long  RXGMASK;
unsigned long  RX14MASK;
unsigned long  RX15MASK;
unsigned long  CAN_ECR;
unsigned long  CAN_ESR1;
unsigned long  CAN_IMASK2;
unsigned long  CAN_IMASK1;
unsigned long  CAN_IFLAG2;
unsigned long  CAN_IFLAG1;
unsigned long  CAN_CTRL2;
unsigned long  CAN_ESR2;
unsigned long  ulRes2[2];
unsigned long  CAN_CRCR;
unsigned long  CAN_RXFGMASK;
unsigned long  CAN_RXFIR;
unsigned long  ulRes3[12];
KINETIS_CAN_BUF  MBUFF0;
KINETIS_CAN_BUF  MBUFF1;
KINETIS_CAN_BUF  MBUFF2;
KINETIS_CAN_BUF  MBUFF3;
KINETIS_CAN_BUF  MBUFF4;
KINETIS_CAN_BUF  MBUFF5;
KINETIS_CAN_BUF  MBUFF6;
KINETIS_CAN_BUF  MBUFF7;
KINETIS_CAN_BUF  MBUFF8;
KINETIS_CAN_BUF  MBUFF9;
KINETIS_CAN_BUF  MBUFF10;
KINETIS_CAN_BUF  MBUFF11;
KINETIS_CAN_BUF  MBUFF12;
KINETIS_CAN_BUF  MBUFF13;
KINETIS_CAN_BUF  MBUFF14;
KINETIS_CAN_BUF  MBUFF15;
unsigned long  ulRes4[448];
unsigned long  CAN_RXIMR0;
unsigned long  CAN_RXIMR1;
unsigned long  CAN_RXIMR2;
unsigned long  CAN_RXIMR3;
unsigned long  CAN_RXIMR4;
unsigned long  CAN_RXIMR5;
unsigned long  CAN_RXIMR6;
unsigned long  CAN_RXIMR7;
unsigned long  CAN_RXIMR8;
unsigned long  CAN_RXIMR9;
unsigned long  CAN_RXIMR10;
unsigned long  CAN_RXIMR11;
unsigned long  CAN_RXIMR12;
unsigned long  CAN_RXIMR13;
unsigned long  CAN_RXIMR14;
unsigned long  CAN_RXIMR15;
} KINETIS_CAN;


#if defined CHIP_HAS_FLEXIO                                              // {23}
typedef struct stKINETIS_FLEXIO
{
unsigned long FLEXIO_VERID;
unsigned long FLEXIO_PARAM;
unsigned long FLEXIO_CTRL;
unsigned long ulRes0;
unsigned long FLEXIO_SHIFTSTAT;
unsigned long FLEXIO_SHIFTERR;
unsigned long FLEXIO_TIMSTAT;
unsigned long ulRes1;
unsigned long FLEXIO_SHIFTSIEN;
unsigned long FLEXIO_SHIFTEIEN;
unsigned long FLEXIO_TIMIEN;
unsigned long ulRes2;
unsigned long FLEXIO_SHIFTSDEN;
unsigned long ulRes3[19];
unsigned long FLEXIO_SHIFTCTL0;
unsigned long FLEXIO_SHIFTCTL1;
unsigned long FLEXIO_SHIFTCTL2;
unsigned long FLEXIO_SHIFTCTL3;
unsigned long ulRes4[28];
unsigned long FLEXIO_SHIFTCFG0;
unsigned long FLEXIO_SHIFTCFG1;
unsigned long FLEXIO_SHIFTCFG2;
unsigned long FLEXIO_SHIFTCFG3;
unsigned long ulRes5[60];
unsigned long FLEXIO_SHIFTBUF0;
unsigned long FLEXIO_SHIFTBUF1;
unsigned long FLEXIO_SHIFTBUF2;
unsigned long FLEXIO_SHIFTBUF3;
unsigned long ulRes6[28];
unsigned long FLEXIO_SHIFTBUFBIS0;
unsigned long FLEXIO_SHIFTBUFBIS1;
unsigned long FLEXIO_SHIFTBUFBIS2;
unsigned long FLEXIO_SHIFTBUFBIS3;
unsigned long ulRes7[28];
unsigned long FLEXIO_SHIFTBUFBYS0;
unsigned long FLEXIO_SHIFTBUFBYS1;
unsigned long FLEXIO_SHIFTBUFBYS2;
unsigned long FLEXIO_SHIFTBUFBYS3;
unsigned long ulRes8[28];
unsigned long FLEXIO_SHIFTBUFBBS0;
unsigned long FLEXIO_SHIFTBUFBBS1;
unsigned long FLEXIO_SHIFTBUFBBS2;
unsigned long FLEXIO_SHIFTBUFBBS3;
unsigned long ulRes9[28];
unsigned long FLEXIO_TIMCTL0;
unsigned long FLEXIO_TIMCTL1;
unsigned long FLEXIO_TIMCTL2;
unsigned long FLEXIO_TIMCTL3;
unsigned long ulRes10[28];
unsigned long FLEXIO_TIMCFG0;
unsigned long FLEXIO_TIMCFG1;
unsigned long FLEXIO_TIMCFG2;
unsigned long FLEXIO_TIMCFG3;
unsigned long ulRes11[28];
unsigned long FLEXIO_TIMCMP0;
unsigned long FLEXIO_TIMCMP1;
unsigned long FLEXIO_TIMCMP2;
unsigned long FLEXIO_TIMCMP3;
} KINETIS_FLEXIO;
#endif

#if defined LTC_AVAILABLE                                                // {33}
typedef struct stKINETIS_LTC
{
unsigned long LTC0_MD_MDPK;
unsigned long ulRes0;
unsigned long LTC0_KS;
unsigned long LTC0_DS;
unsigned long ulRes1;
unsigned long LTC0_ICVS;
unsigned long ulRes2[5];
unsigned long LTC0_COM;
unsigned long LTC0_CTL;
unsigned long ulRes3[2];
unsigned long LTC0_CW;
unsigned long ulRes4;
unsigned long LTC0_STA;
unsigned long LTC0_ESTA;
unsigned long ulRes5[2];
unsigned long LTC0_AADSZ;
unsigned long ulRes6;
unsigned long LTC0_IVS;
unsigned long ulRes7;
unsigned long LTC0_DPAMS;
unsigned long ulRes8[5];
unsigned long LTC0_PKASZ;
unsigned long ulRes9;
unsigned long LTC0_PKBSZ;
unsigned long ulRes10;
unsigned long LTC0_PKNSZ;
unsigned long ulRes11;
unsigned long LTC0_PKESZ;
unsigned long ulRes12[26];
unsigned long LTC0_CTX_0;
unsigned long LTC0_CTX_1;
unsigned long LTC0_CTX_2;
unsigned long LTC0_CTX_3;
unsigned long LTC0_CTX_4;
unsigned long LTC0_CTX_5;
unsigned long LTC0_CTX_6;
unsigned long LTC0_CTX_7;
unsigned long LTC0_CTX_8;
unsigned long LTC0_CTX_9;
unsigned long LTC0_CTX_10;
unsigned long LTC0_CTX_11;
unsigned long LTC0_CTX_12;
unsigned long LTC0_CTX_13;
unsigned long LTC0_CTX_14;
unsigned long LTC0_CTX_15;
unsigned long ulRes13[49];
unsigned long LTC0_KEY_0;
unsigned long LTC0_KEY_1;
unsigned long LTC0_KEY_2;
unsigned long LTC0_KEY_3;
unsigned long LTC0_KEY_4;
unsigned long LTC0_KEY_5;
unsigned long LTC0_KEY_6;
unsigned long LTC0_KEY_7;
unsigned long ulRes14[361];
unsigned long LTC0_FIFOSTA;
unsigned long ulRes15[8];
unsigned long LTC0_IFIFO;
unsigned long ulRes16[4];
unsigned long LTC0_OFIFO;
} KINETIS_LTC;
#endif

#if defined KINETIS_K80
typedef struct stKINETIS_QSPI                                            // {29}
{
    unsigned long QuadSPI_MCR;
    unsigned long ulRes0;
    unsigned long QuadSPI_IPCR;
    unsigned long QuadSPI_FLSHCR;
    unsigned long QuadSPI_BUF0CR;
    unsigned long QuadSPI_BUF1CR;
    unsigned long QuadSPI_BUF2CR;
    unsigned long QuadSPI_BUF3CR;
    unsigned long QuadSPI_BFGENCR;
    unsigned long QuadSPI_SOCCR;
    unsigned long ulRes1[2];
    unsigned long QuadSPI_BUF0IND;
    unsigned long QuadSPI_BUF1IND;
    unsigned long QuadSPI_BUF2IND;
    unsigned long ulRes3[49];
    unsigned long QuadSPI_SFAR;
    unsigned long QuadSPI_SFACR;
    unsigned long QuadSPI_SMPR;
    unsigned long QuadSPI_RBSR;
    unsigned long QuadSPI_RBCT;
    unsigned long ulRes4[15];
    unsigned long QuadSPI_TBSR;
    unsigned long QuadSPI_TBDR;
    unsigned long QuadSPI_TBCT;
    unsigned long QuadSPI_SR;
    unsigned long QuadSPI_FR;
    unsigned long QuadSPI_RSER;
    unsigned long QuadSPI_SPNDST;
    unsigned long QuadSPI_SPTRCLR;
    unsigned long ulRes5[4];
    unsigned long QuadSPI_SFA1AD;
    unsigned long QuadSPI_SFA2AD;
    unsigned long QuadSPI_SFB1AD;
    unsigned long QuadSPI_SFB2AD;
    unsigned long QuadSPI_DLPR;
    unsigned long ulRes6[27];
    unsigned long QuadSPI_RBDR0;
    unsigned long QuadSPI_RBDR1;
    unsigned long QuadSPI_RBDR2;
    unsigned long QuadSPI_RBDR3;
    unsigned long QuadSPI_RBDR4;
    unsigned long QuadSPI_RBDR5;
    unsigned long QuadSPI_RBDR6;
    unsigned long QuadSPI_RBDR7;
    unsigned long QuadSPI_RBDR8;
    unsigned long QuadSPI_RBDR9;
    unsigned long QuadSPI_RBDR10;
    unsigned long QuadSPI_RBDR11;
    unsigned long QuadSPI_RBDR12;
    unsigned long QuadSPI_RBDR13;
    unsigned long QuadSPI_RBDR14;
    unsigned long QuadSPI_RBDR15;
    unsigned long ulRes7[48];
    unsigned long QuadSPI_LUTKEY;
    unsigned long QuadSPI_LCKCR;
    unsigned long ulRes8;
    unsigned long QuadSPI_LUT0;
    unsigned long QuadSPI_LUT1;
    unsigned long QuadSPI_LUT2;
    unsigned long QuadSPI_LUT3;
    unsigned long QuadSPI_LUT4;
    unsigned long QuadSPI_LUT5;
    unsigned long QuadSPI_LUT6;
    unsigned long QuadSPI_LUT7;
    unsigned long QuadSPI_LUT8;
    unsigned long QuadSPI_LUT9;
    unsigned long QuadSPI_LUT10;
    unsigned long QuadSPI_LUT11;
    unsigned long QuadSPI_LUT12;
    unsigned long QuadSPI_LUT13;
    unsigned long QuadSPI_LUT14;
    unsigned long QuadSPI_LUT15;
    unsigned long QuadSPI_LUT16;
    unsigned long QuadSPI_LUT17;
    unsigned long QuadSPI_LUT18;
    unsigned long QuadSPI_LUT19;
    unsigned long QuadSPI_LUT20;
    unsigned long QuadSPI_LUT21;
    unsigned long QuadSPI_LUT22;
    unsigned long QuadSPI_LUT23;
    unsigned long QuadSPI_LUT24;
    unsigned long QuadSPI_LUT25;
    unsigned long QuadSPI_LUT26;
    unsigned long QuadSPI_LUT27;
    unsigned long QuadSPI_LUT28;
    unsigned long QuadSPI_LUT29;
    unsigned long QuadSPI_LUT30;
    unsigned long QuadSPI_LUT31;
    unsigned long QuadSPI_LUT32;
    unsigned long QuadSPI_LUT33;
    unsigned long QuadSPI_LUT34;
    unsigned long QuadSPI_LUT35;
    unsigned long QuadSPI_LUT36;
    unsigned long QuadSPI_LUT37;
    unsigned long QuadSPI_LUT38;
    unsigned long QuadSPI_LUT39;
    unsigned long QuadSPI_LUT40;
    unsigned long QuadSPI_LUT41;
    unsigned long QuadSPI_LUT42;
    unsigned long QuadSPI_LUT43;
    unsigned long QuadSPI_LUT44;
    unsigned long QuadSPI_LUT45;
    unsigned long QuadSPI_LUT46;
    unsigned long QuadSPI_LUT47;
    unsigned long QuadSPI_LUT48;
    unsigned long QuadSPI_LUT49;
    unsigned long QuadSPI_LUT50;
    unsigned long QuadSPI_LUT51;
    unsigned long QuadSPI_LUT52;
    unsigned long QuadSPI_LUT53;
    unsigned long QuadSPI_LUT54;
    unsigned long QuadSPI_LUT55;
    unsigned long QuadSPI_LUT56;
    unsigned long QuadSPI_LUT57;
    unsigned long QuadSPI_LUT58;
    unsigned long QuadSPI_LUT59;
    unsigned long QuadSPI_LUT60;
    unsigned long QuadSPI_LUT61;
    unsigned long QuadSPI_LUT62;
    unsigned long QuadSPI_LUT63;
} KINETIS_QSPI;
#endif

typedef struct stKINETIS_PERIPH
{
#if defined KINETIS_KL
    #if !defined DEVICE_WITHOUT_DMA
    KINETIS_KL_DMA     DMA;                                              // {18}
    #endif
#else
    KINETIS_eDMA       eDMA;
    KINETIS_eDMADES    eDMADES;
    KINETIS_FB         FB;
#endif
#if defined MPU_AVAILABLE
    KINETIS_MPU        MPU;
#endif
#if !defined KINETIS_KL
    KINETIS_FMC        FMC;
#endif
#if defined KINETIS_KE
    KINETIS_IRQ        IRQ;                                              // {24}
#endif
#if defined KINETIS_KL || defined KINETIS_KE
    KINETIS_SPI        SPI[2];
  #if defined KINETIS_KE
    KINETIS_KBI        KBI[2];                                           // {22}
    #if defined MSCAN_CAN_INTERFACE
    KINETIS_MSCAN      MSCAN;                                            // {27}
    #endif
  #endif
#else
    #if defined KINETIS_K20 && (KINETIS_MAX_SPEED == 72000000)
    KINETIS_CAN        CAN[1];
    #else
    KINETIS_CAN        CAN[2];
    #endif
    KINETIS_DSPI       DSPI[3];
    KINETIC_CRC        CRC;                                              // {6}
#endif
#if defined LLWU_AVAILABLE
    KINETIS_LLWU       LLWU;                                             // {26}
#endif
    KINETIS_PMC        PMC;
#if defined HS_USB_AVAILABLE
    KINETIC_USBHS      USBHS;                                            // {8}
    #if defined KINETIS_WITH_USBPHY
    KINETIC_USBPHY     USBHSPHY;                                         // {29}
    #endif
#endif
    KINETIS_FTFL       FTFL;
#if defined KINETIS_K70 || (defined KINETIS_K60 && defined KINETIS_K_FPU)// {4}
    KINETIS_DMAMUX     DMAMUX[2];
#else
    KINETIS_DMAMUX     DMAMUX[1];
#endif
#if I2S_AVAILABLE > 0
    KINETIS_I2S        I2S_SAI[I2S_AVAILABLE];                           // {28}
#endif
#if defined PWT_AVAILABLE
    KINETIS_PWT        PWT;                                              // {31}
#endif
#if !defined KINETIS_KL                                                  // {15}
    KINETIS_PDB        PDB;                                              // {12}
#endif
#if !defined KINETIS_WITHOUT_PIT
    KINETIS_PIT        PIT;
#endif
    KINETIS_FTM        FTM[FLEX_TIMERS_AVAILABLE];
    KINETIS_ADC        ADC0;
#if ADC_CONTROLLERS > 2                                                  // {2}
    KINETIS_ADC        ADC2;
#endif
#if !defined KINETIS_KL02
    KINETIS_RTC        RTC;
#endif
#if !defined KINETIS_KE
    KINETIS_LPTMR      LPTMR;                                            // {20}
#endif
#if !defined KINETIS_KE && !defined KINETIS_KL && !defined CROSSBAR_SWITCH_LITE
    KINETIS_AXBS       AXBS;                                             // {19}
#endif
    KINETIS_TSI        TSI;
    KINETIS_SIM        SIM;
#if defined KINETIS_KE
    KINETIS_KE_PORT    PORT;
#else
    KINETIS_PORT       PORT[PORTS_AVAILABLE];
#endif
#if !defined KINETIS_KL || defined KINETIS_KL82
    KINETIS_WDOG       WDOG;
#endif
#if defined CHIP_HAS_FLEXIO                                              // {23}
    KINETIS_FLEXIO     FLEXIO;
#endif
#if defined LTC_AVAILABLE
    KINETIS_LTC        LTC;                                              // {33}
#endif
#if defined KINETIS_K80
    KINETIS_QSPI       QSPI;                                             // {29}
#endif
#if !defined KINETIS_KL && !defined KINETIS_KE
    KINETIS_EWM        EWM;
#endif
#if defined KINETIS_KE
    KINETIS_ICS        ICS;
#elif defined KINETIS_WITH_SCG                                           // {32}
    KINETIS_SCG        SCG;
#else
    KINETIS_MCG        MCG;
#endif
    KINETIS_OSC        OSC[2];
    KINETIS_I2C        I2C[I2C_AVAILABLE];
#if UARTS_AVAILABLE > 0
    KINETIS_UART       UART[UARTS_AVAILABLE];
#endif
#if LPUARTS_AVAILABLE > 0
    KINETIS_LPUART     LPUART[LPUARTS_AVAILABLE];
#endif
    KINETIS_USB        USB;
#if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000) // {16}
    KINETIS_SMC        SMC;                                              // {21}
    KINETIS_RCM        RCM;                                              // {5}
#elif !defined KINETIS_KE && !defined KINETIS_KEA
    KINETIS_MC         MC;
#endif
#if defined KINETIS_KE                                                   // {30}
    KINETIS_ACMP       ACMP[2];
#else
    KINETIS_CMP        CMP[4];
#endif
#if !defined KINETIS_KL
    KINETIS_VREF       VREF;
#endif
#if defined RNG_AVAILABLE
    #if !defined RANDOM_NUMBER_GENERATOR_B                               // {13}
    KINETIS_RNGA       RNGA;
    #else
    KINETIS_RNGB       RNGB;
    #endif
#endif
#if defined SDRAM_CONTROLLER_AVAILABLE
    unsigned char      NFC_buf[4][0x900];                                // {9}
    KINETIS_NFC        NFC;                                              // {1}
    KINETIS_DDR        DDR;                                              // {10}
#endif
#if defined KINETIS_K70
    KINETIS_LCD        LCD;                                              // {25}
#endif
#if !defined KINETIS_KL
    KINETIS_SDHC       SDHC;
    KINETIS_ADC        ADC1;
#endif
#if ADC_CONTROLLERS > 3                                                  // {2}
    KINETIS_ADC        ADC3;
#endif
#if defined DEVICE_WITH_SLCD
    KINETIS_SLCD       SLCD;
#endif
#if defined KINETIS_K52 || defined KINETIS_K53 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K70
    KINETIS_EMAC       EMAC;
#endif
#if DAC_CONTROLLERS > 0
    KINETIS_DAC        DAC[DAC_CONTROLLERS];
#endif
    KINETIS_GPIO       GPIO[PORTS_AVAILABLE];
    KINETIS_MCM        MCM;                                              // {11}
#if defined CAU_V1_AVAILABLE || defined CAU_V2_AVAILABLE
    KINETIS_MMCAU      MMCAU;                                            // {17}
#endif

    KINETIS_CORTEX_M4  CORTEX_M4;

    KINETIS_CORTEX_M4_REGS CORTEX_M4_REGS;                               // only for simulation use
} KINETIS_PERIPH;



extern KINETIS_PERIPH kinetis;

#if defined KINETIS_KE
    #define _PORTS_AVAILABLE_   PORTS_AVAILABLE_8_BIT
#else
    #define _PORTS_AVAILABLE_   PORTS_AVAILABLE
#endif

#define _PORTA                  0
#define _PORTB                  1
#define _PORTC                  2
#define _PORTD                  3
#define _PORTE                  4
#define _PORTF                  5
#define _PORTG                  6
#define _PORTH                  7
#define _PORTI                  8

#define _GPIO_ADC               (_PORTS_AVAILABLE_)                      // dedicated ADC pins

#define _PORT_EXP_0             (_PORTS_AVAILABLE_ + 1)                  // {7}
#define _PORT_EXP_1             (_PORTS_AVAILABLE_ + 2)                  // these port numbers should be in order following the internal ports and a fictive ADC port
#define _PORT_EXP_2             (_PORTS_AVAILABLE_ + 3)
#define _PORT_EXP_3             (_PORTS_AVAILABLE_ + 4)
#define _PORT_EXP_4             (_PORTS_AVAILABLE_ + 5)
#define _PORT_EXP_5             (_PORTS_AVAILABLE_ + 6)
#define _PORT_EXP_6             (_PORTS_AVAILABLE_ + 7)
#define _PORT_EXP_7             (_PORTS_AVAILABLE_ + 8)


#define _TOUCH_PORTA            30                                       // {3} touch senor input references on ports
#define _TOUCH_PORTB            31
#define _TOUCH_PORTC            32
#define _TOUCH_PORTD            33
#define _TOUCH_PORTE            34
#define _TOUCH_PORTF            35

#define PIN_COUNT_24_PIN        1
#define PIN_COUNT_32_PIN        2
#define PIN_COUNT_48_PIN        3
#define PIN_COUNT_64_PIN        4
#define PIN_COUNT_80_PIN        5
#define PIN_COUNT_81_PIN        6
#define PIN_COUNT_100_PIN       7
#define PIN_COUNT_121_PIN       8
#define PIN_COUNT_144_PIN       9
#define PIN_COUNT_196_PIN       10
#define PIN_COUNT_256_PIN       11

#define PACKAGE_LQFP            1
#define PACKAGE_MAPBGA          2
#define PACKAGE_BGA             PACKAGE_MAPBGA
#define PACKAGE_QFN             3
#define PACKAGE_WLCSP           4
