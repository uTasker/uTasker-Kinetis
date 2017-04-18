/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      debug_hal.h
    Project:   uTasker project
               - hardware application layer for Kinetis
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file includes a number of hardware specific routines that are
    included in debug.c. This include file allows the main content of debug.c
    to be hardware independent and thus shared by all processor projects. 
    In addition it makes adding specific setups for different processor boards
    possible by exchanging this file or by including multiple sets of the
    routines in it, controlled by defines of each target type

    25.01.2013 Modify fnPortState() to return correct value of inputs (as well as outputs) {1}
    16.05.2013 Add ezport hardware interface                             {2}
    16.01.2014 Add FRDM_K20D50M                                          {3}
    26.01.2014 Add FRDM_KL46Z                                            {4}
    28.01.2014 Add TWR_KL46Z48M                                          {5}
    30.01.2014 Add FRDM_KL25Z, FRDM_KL26Z and TWR_KL25Z48M               {6}
    15.02.2014 Select reset cause based on revison                       {7}
    16.02.2014 Adjust fnAddResetCause() to allow output to debug interface {8}
    22.02.2014 Add FRDM_KL02Z, FRDM_KL05Z and FRDM_KE02Z                 {9}
    08.04.2014 Add FRDM_K64F                                             {10}
    06.07.2014 Add Kinetis KE reset cause                                {11}
    15.07.2014 Add fnShowLowPowerMode() and fnSetLowPowerMode)           {12}
    09.11.2014 Add FRDM_KE04Z, defined FRDM_KL03Z, FRDM_K22F and TWR_K22F120M
    04.12.2014 Add FRDM_KL43Z
    16.02.2015 Add TWR_KL43Z48M
    05.03.2015 Add TWR_KV10Z32
    12.03.2015 Add TWR_KV31F120M
    17.03.2015 Add FRDM_KL27Z
    06.01.2016 Add FRDM_K82F
    24.01.2016 Add BLAZE_K22
    22.11.2016 Add TEENSY_3_5 and TEENSY_3_6
    02.02.2017 Add fnVirtualWakeupInterruptHandler()                     {13}
    12.02.2017 Add FRDM_KL82Z

*/


#if defined USE_MAINTENANCE

// Check the present state of a particular port
//
extern int fnPortState(CHAR cPortBit)
{
    switch (cPortBit) {
#if defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M || defined FRDM_KE04Z || defined TRK_KEA128 || defined TRK_KEA64 || defined TRK_KEA8 || defined K70F150M_12M || defined K20FX512_120
    case '1':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_1) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_1)) != 0); // {1}
    case '2':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_2) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_3) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_3)) != 0);
    #if !defined FRDM_KE04Z && !defined K70F150M_12M
    case '4':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_4) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_4)) != 0);
    #endif
#elif defined FRDM_K64F || defined FreeLON                               // {10}
    case '1':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_1) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_2) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_3) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_4) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_4)) != 0);
#elif defined TWR_K21D50M || defined tinyK20 || defined TWR_K21F120M || defined TWR_K22F120M || defined TWR_K24F120M || defined TWR_KW21D256 || defined TWR_KW24D512 || defined rcARM_KL26
    case '1':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_1) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_2) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_3) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_4) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_4)) != 0);
#elif defined TWR_K20D50M || defined TWR_K20D72M || defined TEENSY_3_1 || defined TEENSY_LC || defined FRDM_K82F || defined TEENSY_3_5 ||defined TEENSY_3_6 || defined FRDM_KL82Z
    case '1':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_1) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_2) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_3) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_4) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_4)) != 0);
#elif defined TWR_KV10Z32
    case '1':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_1) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_2) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_3) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_4) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_4)) != 0);
#elif defined FRDM_K20D50M                                               // {3}
    case '1':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_1) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_2) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_3) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_3)) != 0);
#elif defined FRDM_KL25Z                                                 // {6}
    case '1':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_1) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_2) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_3) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_3)) != 0);
#elif defined FRDM_KL26Z || defined CAPUCCINO_KL27                       // {6}
    case '1':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_1) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_2) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_3) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_3)) != 0);
#elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE06Z || defined FRDM_KE02Z40M || defined K60F150M_50M || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80 || defined BLAZE_K22 // {9}
    case '1':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_1) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_2) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_3) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_3)) != 0);
    #if defined BLAZE_K22
    case '4':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_4) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_4)) != 0);
    #endif
#elif defined FRDM_KL27Z
    case '1':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_1) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_2) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_3) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_3)) != 0);
#elif defined TWR_KL25Z48M                                               // {6}
    case '1':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_1) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_2) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_3) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_4) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_4)) != 0);
#elif defined FRDM_KL43Z
    case '1':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_1) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_2) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_3) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_3) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_3)) != 0);
#elif defined FRDM_KL46Z                                                 // {4}
    case '1':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_1) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_2) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_3) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_3) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_3)) != 0);
#elif defined TWR_KL46Z48M                                               // {5}
    case '1':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_1) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_2) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_3) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_3) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_3)) != 0);
#elif defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_K40D100M
    case '1':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_1) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_2) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_3) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_4) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_4)) != 0);
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TWR_K64F120M
    case '1':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_1) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_2) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_3) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_4) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_4)) != 0);
#elif defined FRDM_K22F || defined TRINAMIC_LANDUNGSBRUECKE
    case '1':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_1) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_2) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_3) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_4) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_4)) != 0);
#elif defined TWR_KL43Z48M || defined TWR_K65F180M
    case '1':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_1) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_2) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_3) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_4) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_4)) != 0);
#elif defined TWR_KV31F120M
    case '1':
        return (((GPIOD_PDDR & GPIOD_PDOR & DEMO_LED_1) | (~GPIOD_PDDR & GPIOD_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOB_PDDR & GPIOB_PDOR & DEMO_LED_2) | (~GPIOB_PDDR & GPIOB_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_3) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_4) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_4)) != 0);
#elif defined FRDM_K66F
    case '1':
        return (((GPIOE_PDDR & GPIOE_PDOR & DEMO_LED_1) | (~GPIOE_PDDR & GPIOE_PDIR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIOC_PDDR & GPIOC_PDOR & DEMO_LED_2) | (~GPIOC_PDDR & GPIOC_PDIR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_3) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIOA_PDDR & GPIOA_PDOR & DEMO_LED_4) | (~GPIOA_PDDR & GPIOA_PDIR & DEMO_LED_4)) != 0);
#endif
    default:
        return 0;
    }
}


// Check whether the port is configured as an input or as output
//
extern int fnPortInputConfig(CHAR cPortBit)
{
    switch (cPortBit) {
#if defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M || defined FRDM_KE04Z || defined TRK_KEA128 || defined TRK_KEA64 || defined TRK_KEA8 || defined K70F150M_12M || defined K20FX512_120
    case '1':
        return ((GPIOA_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOA_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOA_PDDR & DEMO_LED_3) == 0);
    #if !defined FRDM_KE04Z && !defined K70F150M_12M
    case '4':
        return ((GPIOA_PDDR & DEMO_LED_4) == 0);
    #endif
#elif defined FRDM_K64F || defined FreeLON                               // {10}
    case '1':
        return ((GPIOE_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOC_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOB_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOB_PDDR & DEMO_LED_4) == 0);
#elif defined TWR_K21D50M || defined tinyK20 || defined TWR_K21F120M || defined TWR_K22F120M || defined TWR_K24F120M || defined TWR_KW21D256 || defined TWR_KW24D512 || defined rcARM_KL26
    case '1':
        return ((GPIOD_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOD_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOD_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOD_PDDR & DEMO_LED_4) == 0);
#elif defined TWR_K20D50M || defined TWR_K20D72M || defined TEENSY_3_1 || defined TEENSY_LC || defined FRDM_K82F || defined TEENSY_3_5 || defined TEENSY_3_6 || defined FRDM_KL82Z
    case '1':
        return ((GPIOC_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOC_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOC_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOC_PDDR & DEMO_LED_4) == 0);
#elif defined TWR_KV10Z32
    case '1':
        return ((GPIOC_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOE_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOC_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOC_PDDR & DEMO_LED_4) == 0);
#elif defined FRDM_K20D50M                                               // {3}
    case '1':
        return ((GPIOD_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOC_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOA_PDDR & DEMO_LED_3) == 0);
#elif defined FRDM_KL25Z                                                 // {6}
    case '1':
        return ((GPIOB_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOD_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOB_PDDR & DEMO_LED_3) == 0);
#elif defined FRDM_KL26Z || defined CAPUCCINO_KL27                       // {6}
    case '1':
        return ((GPIOE_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOE_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOD_PDDR & DEMO_LED_3) == 0);
#elif defined FRDM_KL27Z
    case '1':
        return ((GPIOB_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOB_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOA_PDDR & DEMO_LED_3) == 0);
#elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE06Z || defined FRDM_KE02Z40M || defined K60F150M_50M || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80 || defined BLAZE_K22 // {9}
    case '1':
        return ((GPIOB_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOB_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOB_PDDR & DEMO_LED_3) == 0);
    #if defined BLAZE_K22
    case '4':
        return ((GPIOC_PDDR & DEMO_LED_4) == 0);
    #endif
#elif defined TWR_KL25Z48M                                               // {6}
    case '1':
        return ((GPIOA_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOB_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOA_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOA_PDDR & DEMO_LED_4) == 0);
#elif defined FRDM_KL43Z
    case '1':
        return ((GPIOE_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOD_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOB_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOB_PDDR & DEMO_LED_4) == 0);

#elif defined FRDM_KL46Z                                                 // {4}
    case '1':
        return ((GPIOD_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOE_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOC_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOC_PDDR & DEMO_LED_4) == 0);
#elif defined TWR_KL46Z48M                                               // {5}
    case '1':
        return ((GPIOA_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOB_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOE_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOA_PDDR & DEMO_LED_4) == 0);
#elif defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_K40D100M
    case '1':
        return ((GPIOC_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOC_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOB_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOC_PDDR & DEMO_LED_4) == 0);
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TWR_K64F120M
    case '1':
        return ((GPIOE_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOE_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOE_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOE_PDDR & DEMO_LED_4) == 0);
#elif defined FRDM_K22F || defined TRINAMIC_LANDUNGSBRUECKE
    case '1':
        return ((GPIOA_PDDR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIOA_PDDR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIOD_PDDR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIOD_PDDR & DEMO_LED_4) == 0);
#endif
    default:
        return 0;
    }
}


// Configure a processor port pin as input/output or analogue
//
extern int fnConfigPort(CHAR cPortBit, CHAR cType)
{
    unsigned long ulPortBit;
    switch (cPortBit) {
    case '0':
        ulPortBit = DEMO_LED_1;
        break;
    case '1':
        ulPortBit = DEMO_LED_2;
        break;
    case '2':
        ulPortBit = DEMO_LED_3;
        break;
    case '3':
        ulPortBit = DEMO_LED_4;
        break;
    default:
        return 0;
    }
    cPortBit -= '0';                                                     // select bit offset
    switch (cType) {
    case 'd':                                                            // default use for 0..4
    case 'i':                                                            // port to be input
#if defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M || defined FRDM_KE04Z || defined TRK_KEA128 || defined TRK_KEA64 || defined TRK_KEA8 || defined K70F150M_12M || defined K20FX512_120
        _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);             // set port bit to input (on port A)
#elif defined FRDM_K64F || defined FreeLON                               // {10}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port E)
            break;
        case 1:
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        }
#elif defined FRDM_K22F || defined TRINAMIC_LANDUNGSBRUECKE
        switch (cPortBit) {
        case 0:
        case 1:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port D)
            break;
        }
#elif defined TWR_KL43Z48M || defined TWR_K65F180M
        switch (cPortBit) {
        case 0:
        case 1:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        }
#elif defined TWR_KV31F120M
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port D)
            break;
        case 1:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port E)
            break;
        }
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TWR_K64F120M
        _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);             // set port bit to input (on port E)
#elif defined TWR_K21D50M || defined tinyK20 || defined TWR_K21F120M || defined TWR_K22F120M || defined TWR_K24F120M || defined TWR_KW21D256 || defined TWR_KW24D512 || defined rcARM_KL26
        _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);             // set port bit to input (on port D)
#elif defined TWR_K20D50M || defined TWR_K20D72M || defined TEENSY_3_1 || defined TEENSY_LC || defined FRDM_K82F || defined TEENSY_3_5 || defined TEENSY_3_6 || defined FRDM_KL82Z
    #if ((defined TEENSY_3_1 || defined TEENSY_LC) && defined ENC424J600_INTERFACE)
        if (ulPortBit & (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3)) {        // don't disturb these pins which are used by the Ethernet controller
            return 0;
        }
    #endif
    #if !(defined TEENSY_LC && defined ENC424J600_INTERFACE)
        _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);             // set port bit to input (on port C)
    #endif
#elif defined TWR_KV10Z32
        if (cPortBit == 1) {
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port E)
        }
        else {
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
        }
#elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE06Z || defined FRDM_KE02Z40M || defined K60F150M_50M || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80 || defined BLAZE_K22 // {9}
        #if defined BLAZE_K22
        if (cPortBit == 4) {
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        }
        #endif
        _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);             // set port bit to input (on port B)
#elif defined FRDM_K20D50M                                               // {3}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port D)
            break;
        case 1:
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        case 2:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        }
#elif defined FRDM_KL25Z                                                 // {6}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port D)
            break;
        case 1:
            _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        case 2:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        }
#elif defined FRDM_KL26Z || defined CAPUCCINO_KL27                       // {6}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port D)
            break;
        case 1:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        case 2:
            _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        }
#elif defined FRDM_KL27Z
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        case 1:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        case 2:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        }
#elif defined TWR_KL25Z48M                                               // {6}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        case 1:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        case 2:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        case 3:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        }
#elif defined FRDM_KL43Z
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port D)
            break;
        case 1:
            _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port E)
            break;
        case 2:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        case 3:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        }
#elif defined FRDM_KL46Z                                                 // {4}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(D, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port D)
            break;
        case 1:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port E)
            break;
        case 2:
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        case 3:
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        }
#elif defined TWR_KL46Z48M                                               // {5}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        case 1:
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
            break;
        case 2:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port E)
            break;
        case 3:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        }
#elif defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_K40D100M
        if (cPortBit == 3) {
            _CONFIG_PORT_INPUT(B, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port B)
        }
        else {
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
        }
#elif defined FRDM_K66F
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(E, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port E)
            break;
        case 1:
            _CONFIG_PORT_INPUT(C, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port C)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_INPUT(A, ulPortBit, PORT_PS_UP_ENABLE);         // set port bit to input (on port A)
            break;
        }
#endif
        temp_pars->temp_parameters.ucUserOutputs &= ~(1 << cPortBit);    // save present DDR setting
        break;

    case 'o':                                                            // port to be output
#if defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M || defined FRDM_KE04Z || defined TRK_KEA128 || defined TRK_KEA64 || defined TRK_KEA8 || defined K70F150M_12M || defined K20FX512_120
        _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);                // set port bit to output (on port A)
#elif defined FRDM_K64F || defined FreeLON                               // {10}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port E)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        }
#elif defined FRDM_K22F || defined TRINAMIC_LANDUNGSBRUECKE
        switch (cPortBit) {
        case 0:
        case 1:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_FAST | PORT_DSE_HIGH); // set port bit to output (on port A)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_FAST | PORT_DSE_HIGH); // set port bit to output (on port D)
            break;
        }
#elif defined TWR_KL43Z48M || defined TWR_K65F180M 
        switch (cPortBit) {
        case 0:
        case 1:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        }
#elif defined TWR_KV31F120M
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port D)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port E)
            break;
        }
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TWR_K64F120M
        _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);                // set port bit to output (on port E)
#elif defined TWR_K21D50M || defined tinyK20 || defined TWR_K21F120M || defined TWR_K22F120M || defined TWR_K24F120M || defined TWR_KW21D256 || defined TWR_KW24D512 || defined rcARM_KL26
        _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_SLOW);                // set port bit to output (on port D)
#elif defined TWR_K20D50M || defined TWR_K20D72M || defined TEENSY_3_1 || defined TEENSY_LC || defined FRDM_K82F || defined TEENSY_3_5 || defined TEENSY_3_6 || defined FRDM_KL82Z
    #if ((defined TEENSY_3_1 || defined TEENSY_LC) && defined ENC424J600_INTERFACE)
        if (ulPortBit & (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3)) {        // don't disturb these pins which are used by the Ethernet controller
            return 0;
        }
    #endif
    #if !(defined TEENSY_LC && defined ENC424J600_INTERFACE)
        _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);                // set port bit to output (on port C)
    #endif
#elif defined TWR_KV10Z32
        if (cPortBit == 1) {
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port E)
        }
        else {
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
        }
#elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE06Z || defined FRDM_KE02Z40M || defined K60F150M_50M || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80 || defined BLAZE_K22 // {9}
    #if defined BLAZE_K22
        if (cPortBit == 4) {
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        }
    #endif
        _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);                // set port bit to output (on port B)
#elif defined FRDM_K20D50M                                               // {3}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port D)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        }
#elif defined FRDM_KL25Z                                                 // {6}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port D)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        }
#elif defined FRDM_KL26Z || defined CAPUCCINO_KL27                       // {6}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port D)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        }
#elif defined FRDM_KL27Z
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        }
#elif defined TWR_KL25Z48M                                               // {6}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        }
#elif defined FRDM_KL43Z
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port D)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port E)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        }
#elif defined FRDM_KL46Z                                                 // {4}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(D, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port D)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port E)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        }
#elif defined TWR_KL46Z48M                                               // {5}
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port E)
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        }
#elif defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_K40D100M
        if (cPortBit == 3) {
            _CONFIG_PORT_OUTPUT(B, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port B)
        }
        else {
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
        }
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TWR_K64F120M
        _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);                // set port bit to output (on port A)
#elif defined FRDM_K66F
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(E, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port E)
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(C, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port C)
            break;
        case 2:
        case 3:
            _CONFIG_PORT_OUTPUT(A, ulPortBit, PORT_SRE_SLOW);            // set port bit to output (on port A)
            break;
        }
#endif
        temp_pars->temp_parameters.ucUserOutputs |= (1 << cPortBit);     // save present DDR setting
        break;

    case 'a':                                                            // port to be analog
        break;

    default:
        return -1;
    }
    return 0;
}


// Set a single output to logical '1' or '0' state
//
static void fnSetPortBit(unsigned short usBit, int iSetClr)
{
#if !defined FRDM_KL02Z && !defined tinyK20 && !defined FRDM_KL03Z && !defined FRDM_KL05Z && !defined FRDM_KE02Z && !defined FRDM_KE06Z && !defined FRDM_KE02Z40M && !defined TRK_KEA128 && !defined TRK_KEA64 && !defined TRK_KEA8 && !defined FRDM_KE04Z && !defined FRDM_K22F && !defined K70F150M_12M && !defined TWR_KL43Z48M && !defined TWR_KV31F120M && !defined TWR_KW21D256 && !defined TWR_KW24D512 && !defined K20FX512_120 && !defined K60F150M_50M && !defined TWR_K65F180M && !defined FRDM_K66F && !defined FRDM_K82F && !defined BLAZE_K22 && !defined FRDM_KEAZN32Q64 && !defined FRDM_KEAZ64Q64 && !defined FRDM_KEAZ128Q80 && !defined TEENSY_3_5 && !defined TEENSY_3_6 && !defined FRDM_KL82Z && !defined TRINAMIC_LANDUNGSBRUECKE // {9}
    if (iSetClr != 0) {
        switch (usBit) {
        case 0x0001:
            USER_SET_PORT_1 = USER_PORT_1_BIT;
            break;
        case 0x0002:
            USER_SET_PORT_2 = USER_PORT_2_BIT;
            break;
        case 0x0004:
            USER_SET_PORT_3 = USER_PORT_3_BIT;
            break;
        case 0x0008:
            USER_SET_PORT_4 = USER_PORT_4_BIT;
            break;
        case 0x0010:
            USER_SET_PORT_5 = USER_PORT_5_BIT;
            break;
        case 0x0020:
            USER_SET_PORT_6 = USER_PORT_6_BIT;
            break;
        case 0x0040:
            USER_SET_PORT_7 = USER_PORT_7_BIT;
            break;
        case 0x0080:
            USER_SET_PORT_8 = USER_PORT_8_BIT;
            break;
        case 0x0100:
            USER_SET_PORT_9 = USER_PORT_9_BIT;
            break;
        case 0x0200:
            USER_SET_PORT_10 = USER_PORT_10_BIT;
            break;
        case 0x0400:
            USER_SET_PORT_11 = USER_PORT_11_BIT;
            break;
        case 0x0800:
            USER_SET_PORT_12 = USER_PORT_12_BIT;
            break;
        case 0x1000:
            USER_SET_PORT_13 = USER_PORT_13_BIT;
            break;
        case 0x2000:
            USER_SET_PORT_14 = USER_PORT_14_BIT;
            break;
        case 0x4000:
            USER_SET_PORT_15 = USER_PORT_15_BIT;
            break;
        case 0x8000:
            USER_SET_PORT_16 = USER_PORT_16_BIT;
            break;
        }
    }
    else {
        switch (usBit) {
        case 0x0001:
            USER_CLEAR_PORT_1 = USER_PORT_1_BIT;
            break;
        case 0x0002:
            USER_CLEAR_PORT_2 = USER_PORT_2_BIT;
            break;
        case 0x0004:
            USER_CLEAR_PORT_3 = USER_PORT_3_BIT;
            break;
        case 0x0008:
            USER_CLEAR_PORT_4 = USER_PORT_4_BIT;
            break;
        case 0x0010:
            USER_CLEAR_PORT_5 = USER_PORT_5_BIT;
            break;
        case 0x0020:
            USER_CLEAR_PORT_6 = USER_PORT_6_BIT;
            break;
        case 0x0040:
            USER_CLEAR_PORT_7 = USER_PORT_7_BIT;
            break;
        case 0x0080:
            USER_CLEAR_PORT_8 = USER_PORT_8_BIT;
            break;
        case 0x0100:
            USER_CLEAR_PORT_9 = USER_PORT_9_BIT;
            break;
        case 0x0200:
            USER_CLEAR_PORT_10 = USER_PORT_10_BIT;
            break;
        case 0x0400:
            USER_CLEAR_PORT_11 = USER_PORT_11_BIT;
            break;
        case 0x0800:
            USER_CLEAR_PORT_12 = USER_PORT_12_BIT;
            break;
        case 0x1000:
            USER_CLEAR_PORT_13 = USER_PORT_13_BIT;
            break;
        case 0x2000:
            USER_CLEAR_PORT_14 = USER_PORT_14_BIT;
            break;
        case 0x4000:
            USER_CLEAR_PORT_15 = USER_PORT_15_BIT;
            break;
        case 0x8000:
            USER_CLEAR_PORT_16 = USER_PORT_16_BIT;
            break;
        }
    }                                                                    _SIM_PORTS; // ensure the simulator updates its port states
#endif
}


// Request present logic level of an output
//
extern int fnUserPortState(CHAR cPortBit)
{
#if !defined FRDM_KL02Z && !defined tinyK20 && !defined FRDM_KL03Z && !defined FRDM_KL05Z && !defined FRDM_KE02Z && !defined FRDM_KE04Z && !defined FRDM_KE06Z && !defined FRDM_KE02Z40M&& !defined TRK_KEA128 && !defined TRK_KEA64 && !defined TRK_KEA8 && !defined FRDM_K22F && !defined K70F150M_12M && !defined TWR_KL43Z48M && !defined TWR_KV31F120M && !defined TWR_KW21D256 && !defined TWR_KW24D512 && !defined K20FX512_120 && !defined K60F150M_50M && !defined TWR_K65F180M && !defined FRDM_K66F && !defined FRDM_K82F && !defined BLAZE_K22 && !defined FRDM_KEAZN32Q64 && !defined FRDM_KEAZ64Q64 && !defined FRDM_KEAZ128Q80 && !defined TEENSY_3_5 && !defined TEENSY_3_6 && !defined FRDM_KL82Z && !defined TRINAMIC_LANDUNGSBRUECKE // {9}
    switch (cPortBit) {
    case 'a':
        return ((USER_PORT_1 & USER_PORT_1_BIT) != 0);
    case 'b':
        return ((USER_PORT_2 & USER_PORT_2_BIT) != 0);
    case 'c':
        return ((USER_PORT_3 & USER_PORT_3_BIT) != 0);
    case 'd':
        return ((USER_PORT_4 & USER_PORT_4_BIT) != 0);
    case 'e':
        return ((USER_PORT_5 & USER_PORT_5_BIT) != 0);
    case 'f':
        return ((USER_PORT_6 & USER_PORT_6_BIT) != 0);
    case 'g':
        return ((USER_PORT_7 & USER_PORT_7_BIT) != 0);
    case 'h':
        return ((USER_PORT_8 & USER_PORT_8_BIT) != 0);
    case 'i':
        return ((USER_PORT_9 & USER_PORT_9_BIT) != 0);
    case 'j':
        return ((USER_PORT_10 & USER_PORT_10_BIT) != 0);
    case 'k':
        return ((USER_PORT_11 & USER_PORT_11_BIT) != 0);
    case 'l':
        return ((USER_PORT_12 & USER_PORT_12_BIT) != 0);
    case 'm':
        return ((USER_PORT_13 & USER_PORT_13_BIT) != 0);
    case 'n':
        return ((USER_PORT_14 & USER_PORT_14_BIT) != 0);
    case 'o':
        return ((USER_PORT_15 & USER_PORT_15_BIT) != 0);
    case 'p':
        return ((USER_PORT_16 & USER_PORT_16_BIT) != 0);
    default:
        break;
    }
#endif
    return 0;
}

// Configure port bit as output (a..p are port bit references)
//
static int fnConfigOutputPort(CHAR cPortBit)
{
#if !defined FRDM_KL02Z && !defined tinyK20 && !defined FRDM_KL03Z && !defined FRDM_KL05Z && !defined FRDM_KE02Z && !defined FRDM_KE04Z && !defined FRDM_KE06Z && !defined FRDM_KE02Z40M && !defined TRK_KEA128 && !defined TRK_KEA64 && !defined TRK_KEA8 && !defined FRDM_K22F && !defined K70F150M_12M && !defined TWR_KL43Z48M && !defined TWR_KV31F120M && !defined TWR_KW21D256 && !defined TWR_KW24D512 && !defined K20FX512_120 && !defined K60F150M_50M && !defined TWR_K65F180M && !defined FRDM_K66F && !defined FRDM_K82F && !defined BLAZE_K22 && !defined FRDM_KEAZN32Q64 && !defined FRDM_KEAZ64Q64 && !defined FRDM_KEAZ128Q80 && !defined TEENSY_3_5 && !defined TEENSY_3_6 && ! defined FRDM_KL82Z && !defined TRINAMIC_LANDUNGSBRUECKE // {9}
    switch (cPortBit) {
    case 'a':
        _CONFIG_OUTPUT_PORT_1();        
        break;
    case 'b':
        _CONFIG_OUTPUT_PORT_2(); 
        break;
    case 'c':
        _CONFIG_OUTPUT_PORT_3(); 
        break;
    case 'd':
        _CONFIG_OUTPUT_PORT_4(); 
        break;
    case 'e':
        _CONFIG_OUTPUT_PORT_5(); 
        break;
    case 'f':
        _CONFIG_OUTPUT_PORT_6(); 
        break;
    case 'g':
        _CONFIG_OUTPUT_PORT_7(); 
        break;
    case 'h':
        _CONFIG_OUTPUT_PORT_8(); 
        break;
    case 'i':
        _CONFIG_OUTPUT_PORT_9(); 
        break;
    case 'j':
        _CONFIG_OUTPUT_PORT_10(); 
        break;
    case 'k':
        _CONFIG_OUTPUT_PORT_11(); 
        break;
    case 'l':
        _CONFIG_OUTPUT_PORT_12(); 
        break;
    case 'm':
        _CONFIG_OUTPUT_PORT_13(); 
        break;
    case 'n':
        _CONFIG_OUTPUT_PORT_14(); 
        break;
    case 'o':
        _CONFIG_OUTPUT_PORT_15(); 
        break;
    case 'p':
        _CONFIG_OUTPUT_PORT_16(); 
        break;
    default:
        return 1;
    }
#endif
    return 0;
}


// Toggle the state of an output port
//
extern int fnTogglePortOut(CHAR cPortBit)
{
#if !defined FRDM_KL02Z && !defined tinyK20 && !defined FRDM_KL03Z && !defined FRDM_KL05Z && !defined FRDM_KE02Z && !defined FRDM_KE04Z && !defined FRDM_KE06Z && !defined FRDM_KE02Z40M && !defined TRK_KEA128 && !defined TRK_KEA64 && !defined TRK_KEA8 && !defined FRDM_K22F && !defined K70F150M_12M && !defined TWR_KL43Z48M && !defined TWR_KV31F120M && !defined TWR_KW21D256 && !defined TWR_KW24D512 && !defined K20FX512_120 && !defined K60F150M_50M && !defined FRDM_K82F && !defined BLAZE_K22 && !defined FRDM_KEAZN32Q64 && !defined FRDM_KEAZ64Q64 && !defined FRDM_KEAZ128Q80 && !defined FRDM_K66F && !defined TRINAMIC_LANDUNGSBRUECKE // {9}
    switch (cPortBit) {
    case 'a':
        USER_TOGGLE_PORT_1 = USER_PORT_1_BIT;
        break;
    case 'b':
        USER_TOGGLE_PORT_2 = USER_PORT_2_BIT;
        break;
    case 'c':
        USER_TOGGLE_PORT_3 = USER_PORT_3_BIT;
        break;
    case 'd':
        USER_TOGGLE_PORT_4 = USER_PORT_4_BIT;
        break;
    case 'e':
        USER_TOGGLE_PORT_5 = USER_PORT_5_BIT;
        break;
    case 'f':
        USER_TOGGLE_PORT_6 = USER_PORT_6_BIT;
        break;
    case 'g':
        USER_TOGGLE_PORT_7 = USER_PORT_7_BIT;
        break;
    case 'h':
        USER_TOGGLE_PORT_8 = USER_PORT_8_BIT;
        break;
    case 'i':
        USER_TOGGLE_PORT_9 = USER_PORT_9_BIT;
        break;
    case 'j':
        USER_TOGGLE_PORT_10 = USER_PORT_10_BIT;
        break;
    case 'k':
        USER_TOGGLE_PORT_11 = USER_PORT_11_BIT;
        break;
    case 'l':
        USER_TOGGLE_PORT_12 = USER_PORT_12_BIT;
        break;
    case 'm':
        USER_TOGGLE_PORT_13 = USER_PORT_13_BIT;
        break;
    case 'n':
        USER_TOGGLE_PORT_14 = USER_PORT_14_BIT;
        break;
    case 'o':
        USER_TOGGLE_PORT_15 = USER_PORT_15_BIT;
        break;
    case 'p':
        USER_TOGGLE_PORT_16 = USER_PORT_16_BIT;
        break;
    default:
        break;
    }
    temp_pars->temp_parameters.usUserDefinedOutputs ^= (0x0001 << (cPortBit - 'a'));
    _SIM_PORTS;                                                          // ensure the simulator updates its port states
#endif
    return 0;
}

// Write a value to a group of output bits grouped as a byte
//
extern void fnSetPortOut(unsigned char ucPortOutputs, int iInitialisation)
{
#if defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M || defined FRDM_KE04Z || defined TRK_KEA128 || defined TRK_KEA128 || defined K70F150M_12M || defined K20FX512_120
    unsigned long ulPortOutputSetting = 0;
    #if !defined FRDM_KE04Z && !defined TRK_KEA128
    if (iInitialisation != 0) {
        POWER_UP(5, SIM_SCGC5_PORTA);                                    // ensure port is powered before writing initial values
    }
    #endif
    if ((ucPortOutputs & MAPPED_DEMO_LED_1) != 0) {
        ulPortOutputSetting |= DEMO_LED_1;
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_2) != 0) {
        ulPortOutputSetting |= DEMO_LED_2;
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_3) != 0) {
        ulPortOutputSetting |= DEMO_LED_3;
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_4) != 0) {
        ulPortOutputSetting |= DEMO_LED_4;
    }
    _WRITE_PORT_MASK(A, ulPortOutputSetting, (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4));
#elif defined FRDM_K64F || defined FreeLON                               // {10}
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTE | SIM_SCGC5_PORTB | SIM_SCGC5_PORTC)); // ensure ports are powered before writing initial values
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_1) != 0) {
        _WRITE_PORT_MASK(E, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_1));
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_2) != 0) {
        _WRITE_PORT_MASK(C, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_2));
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_3) != 0) {
        _WRITE_PORT_MASK(B, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_3));
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_4) != 0) {
        _WRITE_PORT_MASK(B, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_4));
    }
#elif defined FRDM_K22F || defined TRINAMIC_LANDUNGSBRUECKE
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTA | SIM_SCGC5_PORTD));                // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(A, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(A, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(D, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(D, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_4));
    }
#elif defined TWR_KL43Z48M || defined TWR_K65F180M 
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTA | SIM_SCGC5_PORTB));                // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(A, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(A, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(B, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(B, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_4));
    }
#elif defined TWR_KV31F120M
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTD | SIM_SCGC5_PORTB | SIM_SCGC5_PORTE)); // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(D, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(B, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(E, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(E, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_4));
    }
#elif defined TWR_K21D50M || defined tinyK20 || defined TWR_K21F120M || defined TWR_K22F120M || defined TWR_K24F120M || defined TWR_KW21D256 || defined TWR_KW24D512
    unsigned long ulPortOutputSetting = 0;
    if (iInitialisation != 0) {
        POWER_UP(5, SIM_SCGC5_PORTD);                                    // ensure port is powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        ulPortOutputSetting |= DEMO_LED_1;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        ulPortOutputSetting |= DEMO_LED_2;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        ulPortOutputSetting |= DEMO_LED_3;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        ulPortOutputSetting |= DEMO_LED_4;
    }
    _WRITE_PORT_MASK(D, ulPortOutputSetting, (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4));
#elif defined TWR_K20D50M || defined TWR_K20D72M || defined TEENSY_3_1 || defined TEENSY_LC || defined FRDM_K82F || defined TEENSY_3_5 || defined TEENSY_3_6 || defined FRDM_KL82Z
    unsigned long ulPortOutputSetting = 0;
    if (iInitialisation != 0) {
        POWER_UP(5, SIM_SCGC5_PORTC);                                    // ensure port is powered before writing initial values
    }
    #if !((defined TEENSY_3_1 || defined TEENSY_LC) && defined ENC424J600_INTERFACE)
    if ((ucPortOutputs & MAPPED_DEMO_LED_1) != 0) {
        ulPortOutputSetting |= DEMO_LED_1;
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_2) != 0) {
        ulPortOutputSetting |= DEMO_LED_2;
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_3) != 0) {
        ulPortOutputSetting |= DEMO_LED_3;
    }
    #endif
    #if !(defined TEENSY_LC && defined ENC424J600_INTERFACE)
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        ulPortOutputSetting |= DEMO_LED_4;
    }
    _WRITE_PORT_MASK(C, ulPortOutputSetting, (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4));
    #endif
#elif defined TWR_KV10Z32
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTC | SIM_SCGC5_PORTE));                // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(C, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(E, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(C, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(C, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_4));
    }
#elif defined FRDM_K20D50M                                               // {3}
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTA | SIM_SCGC5_PORTC | SIM_SCGC5_PORTD)); // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(D, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(C, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(A, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_3));
    }
#elif defined FRDM_KL25Z                                                 // {6}
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTD | SIM_SCGC5_PORTB));                // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(B, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(D, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(B, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_3));
    }
#elif defined FRDM_KL26Z || defined CAPUCCINO_KL27                       // {6}
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTE | SIM_SCGC5_PORTD));                // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(E, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(E, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(D, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_3));
    }
#elif defined FRDM_KL27Z
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTB | SIM_SCGC5_PORTA));                // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(B, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(B, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(A, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_3));
    }
#elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE06Z || defined FRDM_KE02Z40M || defined K60F150M_50M || defined TWR_K65F180M || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80 || defined BLAZE_K22 // {9}
    #if !defined KINETIS_KE
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTB));                                  // ensure port is powered before writing initial values
        #if defined BLAZE_K22
        POWER_UP(5, (SIM_SCGC5_PORTC));
        #endif
    }
    #endif
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(B, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(B, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(B, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_3));
    }
    #if defined BLAZE_K22
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(C, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_4));
    }

    #endif
#elif defined TWR_KL25Z48M                                               // {6}
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTA | SIM_SCGC5_PORTB));                // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(A, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(B, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(A, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(A, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_4));
    }
#elif defined FRDM_KL43Z
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTD | SIM_SCGC5_PORTE | SIM_SCGC5_PORTB)); // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(E, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(D, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(B, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(B, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_4));
    }
#elif defined FRDM_KL46Z                                                 // {4}
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTD | SIM_SCGC5_PORTE | SIM_SCGC5_PORTC)); // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(D, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(D, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(E, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(C, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(C, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_4));
    }
#elif defined TWR_KL46Z48M                                               // {5}
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTA | SIM_SCGC5_PORTB | SIM_SCGC5_PORTE)); // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(A, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(B, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(B, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(E, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(A, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_4));
    }
#elif defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_K40D100M
    unsigned long ulPortOutputSetting = 0;
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTB | SIM_SCGC5_PORTC));                // ensure port is powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        ulPortOutputSetting |= DEMO_LED_1;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        ulPortOutputSetting |= DEMO_LED_2;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        ulPortOutputSetting |= DEMO_LED_3;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        ulPortOutputSetting |= DEMO_LED_4;
    }
    _WRITE_PORT_MASK(C, ulPortOutputSetting, (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3));
    _WRITE_PORT_MASK(B, ulPortOutputSetting, (DEMO_LED_4));
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TWR_K64F120M
    unsigned long ulPortOutputSetting = 0;
    if (iInitialisation != 0) {
        POWER_UP(5, SIM_SCGC5_PORTE);                                    // ensure port is powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        ulPortOutputSetting |= DEMO_LED_1;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        ulPortOutputSetting |= DEMO_LED_2;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        ulPortOutputSetting |= DEMO_LED_3;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        ulPortOutputSetting |= DEMO_LED_4;
    }
    _WRITE_PORT_MASK(E, ulPortOutputSetting, (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4));
#elif defined FRDM_K66F
    if (iInitialisation != 0) {
        POWER_UP(5, (SIM_SCGC5_PORTA | SIM_SCGC5_PORTC | SIM_SCGC5_PORTE)); // ensure ports are powered before writing initial values
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        _WRITE_PORT_MASK(E, (DEMO_LED_1), (DEMO_LED_1));
    }
    else {
        _WRITE_PORT_MASK(E, (0), (DEMO_LED_1));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        _WRITE_PORT_MASK(C, (DEMO_LED_2), (DEMO_LED_2));
    }
    else {
        _WRITE_PORT_MASK(C, (0), (DEMO_LED_2));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        _WRITE_PORT_MASK(A, (DEMO_LED_3), (DEMO_LED_3));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_3));
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        _WRITE_PORT_MASK(A, (DEMO_LED_4), (DEMO_LED_4));
    }
    else {
        _WRITE_PORT_MASK(A, (0), (DEMO_LED_4));
    }
#endif
}
#endif                                                                   // end USE_MAINTENANCE

#if defined EZPORT_CLONER && defined USE_MAINTENANCE && defined KEEP_DEBUG // {2}
#if defined FRDM_K64F
    #define EZRESETOUT          PORTE_BIT24                              // port output for reset signal
    #define EZCSOUT             PORTC_BIT4                               // port output for chip select control
    #define EZDIN               PORTD_BIT3                               // port input for data
    #define EZDOUT              PORTD_BIT2                               // port output for data
    #define EZCLKOUT            PORTD_BIT1                               // port output for clock control

    #define _ASSERT_EZRESETOUT() _CLEARBITS(E, EZRESETOUT)               // assert the reset line
    #define _NEGATE_EZRESETOUT() _SETBITS(E, EZRESETOUT)                 // negate the reset line
    #define _ASSERT_EZCSOUT()   _CLEARBITS(C, EZCSOUT)                   // assert the chip select line
    #define _NEGATE_EZCSOUT()   _SETBITS(C, EZCSOUT)                     // negate the chip select line
    #define _SET_EZCLKOUT_HIGH() _SETBITS(D, EZCLKOUT)                   // generate a rising clock edge
    #define _SET_EZCLKOUT_LOW()  _CLEARBITS(D, EZCLKOUT)                 // generate a falling clock edge
    #define _SET_EZDOUT_HIGH()  _SETBITS(D, EZDOUT)                      // set EZDOUT to '1'
    #define _SET_EZDOUT_LOW()   _CLEARBITS(D, EZDOUT)                    // set EZDOUT to '0'
    #define _GET_EZDIN_STATE()  _READ_PORT_MASK(D, EZDIN)                // read the state of the EZDIN pin
#else
    #define EZRESETOUT          PORTD_BIT15                              // port output for reset signal
    #define EZCSOUT             PORTD_BIT11                              // SPI2_SC0
    #define EZDIN               PORTD_BIT13                              // SPI2_MOSI
    #define EZDOUT              PORTD_BIT13                              // SPI2_MISO
    #define EZCLKOUT            PORTD_BIT12                              // SPI2_CLK

    #define _ASSERT_EZRESETOUT() _SETBITS(D, EZRESETOUT);                // assert the reset line
    #define _NEGATE_EZRESETOUT() _CLEARBITS(D, EZRESETOUT);              // negate the reset line
    #define _ASSERT_EZCSOUT()   _CLEARBITS(D, EZCSOUT)                   // assert the chip select line
    #define _NEGATE_EZCSOUT()   _SETBITS(D, EZCSOUT)                     // negate the chip select line
    #define _SET_EZCLKOUT_HIGH() _SETBITS(D, EZCLKOUT)                   // generate a rising clock edge
    #define _SET_EZCLKOUT_LOW()  _CLEARBITS(D, EZCLKOUT)                 // generate a falling clock edge
    #define _SET_EZDOUT_HIGH()  _SETBITS(D, EZDOUT)                      // set EZDOUT to '1'
    #define _SET_EZDOUT_LOW()   _CLEARBITS(D, EZDOUT)                    // set EZDOUT to '0'
    #define _GET_EZDIN_STATE()  _READ_PORT_MASK(D, EZDIN)                // read the state of the EZDIN pin
#endif

// Configure the EzPort signals to their default state
//
static void fnConfigEz(void)
{
    #if defined FRDM_K64F
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, (EZRESETOUT), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); // configure reset output driven low
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (EZCSOUT), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); // configure the chip select output driven low
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (EZDOUT | EZCLKOUT), (EZDOUT | EZCLKOUT), (PORT_SRE_SLOW | PORT_DSE_HIGH)); // configure the data and clock outputs driven high
    _CONFIG_PORT_INPUT_FAST_LOW(D, (EZDIN), (PORT_PS_UP_ENABLE));        // configure the data input
    #else
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (EZRESETOUT | EZCSOUT | EZDOUT | EZCLKOUT), (EZDOUT | EZCLKOUT), (PORT_SRE_SLOW | PORT_DSE_HIGH));
    _CONFIG_PORT_INPUT(D, (EZDIN), (PORT_PS_UP_ENABLE));
    #endif
}

// Send and receive a single byte
// - clock is '1' on entry (this can be used to send a command or data)
// - the chip select is asserted
//
static unsigned char fnSendEZPORT(unsigned char ucDataOut)
{
    unsigned char ucBit = 0x80;                                          // data sent and receive most significant bit first
    unsigned char ucReadByte = 0;
    do {
        if ((ucDataOut & ucBit) != 0) {                                  // set data output state
            _SET_EZDOUT_HIGH();                                          // set EZDOUT to '1'
        }
        else {
            _SET_EZDOUT_LOW();                                           // set EZDOUT to '0'
        }
        fnDelayLoop(1);
        _SET_EZCLKOUT_LOW();                                             // generate falling clock edge
        fnDelayLoop(1);                   
        _SET_EZCLKOUT_HIGH();                                            // rising clock edge
        if (_GET_EZDIN_STATE() != 0) {                                   // read data in
            ucReadByte |= ucBit;
        }
        ucBit >>= 1;
    } while (ucBit != 0);
    return ucReadByte;                                                   // returns with clock set to '1'
}

// Take the slave out of reset, latching its EzPort mode
//
static void fnNegateResetEz(int iEzMode)                                 // remove reset with ezport chip select asserted
{
    if (iEzMode != 0) {
        fnDelayLoop(1);
        _ASSERT_EZCSOUT();                                               // assert ezport chip select
        _NEGATE_EZRESETOUT();                                            // remove the reset state
        fnDelayLoop(EZ_RESET_CS_DELAY_US);                               // delay for the chip select to be latched by the slave
        _NEGATE_EZCSOUT();                                               // negate ezport chip select
    }
    else {
        _ASSERT_EZRESETOUT();                                            // assert the reset state
        _NEGATE_EZCSOUT();                                               // negate ezport chip select
        fnDelayLoop(10);
        _NEGATE_EZRESETOUT();                                            // remove the reset state so that the device can start up normally
    }
    fnDelayLoop(10);
}

// Read the slave's EzPort status register
//
static unsigned char fnGetStatusEz(int iDisplay)
{
    unsigned char ucStatus;
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_RDSR);                                        // command status read
    ucStatus = fnSendEZPORT(0xff);                                       // read the returned status value
    #if defined _WINDOWS
    ucStatus = 0;
    #endif
    if (iDisplay != 0) {                                                 // if the value is to be displayed
        fnDebugMsg("EZPORT status = ");
        fnDebugHex(ucStatus, (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF));
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
    return ucStatus;                                                     // return the read status value
}

#if defined _M5223X
#define RESET_CLOCK_SPEED       20000000                                 // default clock speed out of reset
static void fnPrepareEz(CHAR *ptrInput)
{
    unsigned long ulClockSpeed = RESET_CLOCK_SPEED;                      // default slave processor clock speed if nothing else entered
    unsigned char ucConfigSetting;
    if (*ptrInput != 0) {                                                // if a clock speed it passed, we calculate the correct setting for it
        ulClockSpeed = fnDecStrHex(ptrInput);                            // extract the speed in Hz
    }
    ucConfigSetting = (unsigned char)((ulClockSpeed/2)/185000);          // the divisor needed to get around 200k (not less that 150k and not more than 200k)
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_WRCR);                                        // start commanding configuration value
    fnDebugMsg("EZPORT configured with ");
    fnSendEZPORT(ucConfigSetting);                                       // send configuration value
    fnDebugHex(ucConfigSetting, (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF));
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
    fnDoHardware(DO_EZSTATUS , 0);                                       // show new status
}
#endif

// The write enable must have been set before these commands are acceted
//
static void fnEraseEz(CHAR *ptrInput)
{
    _ASSERT_EZCSOUT();
    fnSendEZPORT(EZCOMMAND_WREN);
    _NEGATE_EZCSOUT();
    fnDelayLoop(1);
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    if (ptrInput != 0) {
        unsigned long ulSectorAddress = fnHexStrHex(ptrInput);
        ulSectorAddress &= ~(0xff000000f);                               // address must be 128 bit aligned
        fnSendEZPORT(EZCOMMAND_SE);                                      // sector erase command
        fnSendEZPORT((unsigned char)(ulSectorAddress >> 16));            // send 24 bit address
        fnSendEZPORT((unsigned char)(ulSectorAddress >> 8));
        fnSendEZPORT((unsigned char)(ulSectorAddress));
        fnDebugMsg("Sector [");
        fnDebugHex(ulSectorAddress, (sizeof(ulSectorAddress) | WITH_LEADIN));
        fnDebugMsg("]");
    }
    else {
        fnSendEZPORT(EZCOMMAND_BE);
        fnDebugMsg("Bulk");
    }
    fnDebugMsg(" erase issued\r\n");
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

// Send a command and display the resulting status register value
//
static void fnCommandEz(CHAR *ptrInput)
{
    if (*ptrInput != 0) {
        unsigned char ucValue;
        _ASSERT_EZCSOUT();                                               // assert ezport chip select - clock always starts at '1'
        ucValue = (unsigned char)fnHexStrHex(ptrInput);
        fnSendEZPORT(ucValue);
        fnDebugHex(ucValue, (sizeof(unsigned char) | WITH_LEADIN));
        fnDebugMsg(" written\r\n");
        _NEGATE_EZCSOUT();                                               // terminate sequence by negating the chip select line
        fnDoHardware(DO_EZSTATUS , 0);                                   // show new status
    }
    else {
        fnDebugMsg("Command missing!\r\n");
    }
}

// The start address must be long word aligned, the clock must be <= 1/8 the system clock
//  the command is not accepted if WEF, WIP or FS status is true
//
static void fnReadEz(CHAR *ptrInput)
{
    static unsigned long ulLastReadAddress = 0;
    static unsigned short usLastReadLength = 16;
    int iRead = 0;
    unsigned short usLength = usLastReadLength;                          // default length if nothing else entered
    if (*ptrInput != 0) {
        ulLastReadAddress = fnHexStrHex(ptrInput);                       // the address to be read from
        if (fnJumpWhiteSpace(&ptrInput) == 0) {                          // optional length
            usLength = (unsigned short)fnDecStrHex(ptrInput);
            if (usLength == 0) {
                usLength = 1;
            }
        }
    }
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulLastReadAddress &= ~(0xff0000003);                                 // address must be long word aligned
    fnSendEZPORT(EZCOMMAND_READ);                                        // send read command
    fnSendEZPORT((unsigned char)(ulLastReadAddress >> 16));              // send 24 bit address
    fnSendEZPORT((unsigned char)(ulLastReadAddress >> 8));
    fnSendEZPORT((unsigned char)(ulLastReadAddress));
    fnDebugMsg("\r\nRead from address ");
    fnDebugHex(ulLastReadAddress, (sizeof(ulLastReadAddress) | WITH_LEADIN | WITH_CR_LF));
    usLastReadLength = usLength;
    while (usLength-- != 0) {
        fnDebugHex(fnSendEZPORT(0xff), (sizeof(unsigned char) | WITH_LEADIN | WITH_SPACE)); // read bytes
        ulLastReadAddress++;
        if (++iRead >= 16) {
            fnDebugMsg("\r\n");
            iRead = 0;
        }
    }
    if (iRead != 0) {
        fnDebugMsg("\r\n");
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

static void fnGetEz(unsigned char *ptrBuffer, unsigned long ulReadAddress, int iLength)
{
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulReadAddress &= ~(0xff0000003);                                     // address must be long word aligned
    fnSendEZPORT(EZCOMMAND_READ);                                        // send read command
    fnSendEZPORT((unsigned char)(ulReadAddress >> 16));                  // send 24 bit address
    fnSendEZPORT((unsigned char)(ulReadAddress >> 8));
    fnSendEZPORT((unsigned char)(ulReadAddress));
    while (iLength-- != 0) {
        *ptrBuffer++ = fnSendEZPORT(0xff);
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

static void fnProgEz(CHAR *ptrInput)
{
    static unsigned long ulLastWriteAddress = 0;
    int iWriteLength = 16;                                               // writes must be multiples of 16 bytes
    unsigned char ucData[16];
    unsigned char *ptrData = ucData;
    _ASSERT_EZCSOUT();
    fnSendEZPORT(EZCOMMAND_WREN);
    _NEGATE_EZCSOUT();
    uMemset(ucData, 0x55, sizeof(ucData));                               // default pattern if nothing else set
    if (*ptrInput != 0) {
        if (*ptrInput == 'U') {                                          // unsecure command
            ulLastWriteAddress = 0x400;                                  // flash configuration
            uMemset(ucData, 0xff, sizeof(ucData));
            ucData[12] = 0xfe;                                           // clear unsecure bit
    #if !defined _WINDOWS
            while ((fnGetStatusEz(0) & EZCOMMAND_SR_WIP) != 0) {}        // wait for a possible bulk-erase to complete
    #endif
        }
        else {
            ulLastWriteAddress = fnHexStrHex(ptrInput);                  // the address to be written to
            if (fnJumpWhiteSpace(&ptrInput) == 0) {                      // optional value
                unsigned char ucPattern = (unsigned char)fnDecStrHex(ptrInput);
                int i = 0;
                while (i < sizeof(ucData)) {
                    ucData[i++] = ucPattern++;
                }
            }
        }
    }
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulLastWriteAddress &= ~(0xff000000f);                                // address must be 128 bit aligned
    fnSendEZPORT(EZCOMMAND_PP);                                          // send program command
    fnSendEZPORT((unsigned char)(ulLastWriteAddress >> 16));             // send 24 bit address
    fnSendEZPORT((unsigned char)(ulLastWriteAddress >> 8));
    fnSendEZPORT((unsigned char)(ulLastWriteAddress));
    fnDebugMsg("\r\nWriting to address ");
    fnDebugHex(ulLastWriteAddress, (sizeof(ulLastWriteAddress) | WITH_LEADIN | WITH_CR_LF));
    while (iWriteLength-- != 0) {
        fnSendEZPORT(*ptrData++);                                        // send bytes
        ulLastWriteAddress++;
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

// This routine enables a write and copies/programs up to a sector of data content
//
static void fnCopyEz(unsigned long ulWriteAddress, unsigned char *ptrSource, int iLength)
{
    _ASSERT_EZCSOUT();
    fnSendEZPORT(EZCOMMAND_WREN);                                        // enable writes
    _NEGATE_EZCSOUT();
    fnDelayLoop(1);
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulWriteAddress &= ~(0xff000000f);                                    // address must be 128 bit aligned
    fnSendEZPORT(EZCOMMAND_PP);                                          // send program command
    fnSendEZPORT((unsigned char)(ulWriteAddress >> 16));                 // send 24 bit address
    fnSendEZPORT((unsigned char)(ulWriteAddress >> 8));
    fnSendEZPORT((unsigned char)(ulWriteAddress));
    while (iLength-- != 0) {
        fnSendEZPORT(*ptrSource++);                                      // send bytes
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}
#endif

// Determine the cause of the last reset
//
extern unsigned char fnAddResetCause(CHAR *ptrBuffer)
{
    const CHAR *ptrStr;
#if !defined KINETIS_KE                                                  // {11}
    static const CHAR cJtag[]          = "JTAG";
    static const CHAR cWakeupOther[]   = "Wakeup";
    static const CHAR cWakeup[]        = "Wakeup reset";
#endif
    static const CHAR cLockup[]        = "Lockup";
    static const CHAR cSoftwareReset[] = "Software";
    static const CHAR cPowerOn[]       = "Power-on";
    static const CHAR cUndervoltage[]  = "Undervoltage";
    static const CHAR cResetInput[]    = "External";
    static const CHAR cWatchdog[]      = "WDOG";
    static const CHAR cClockLoss[]     = "Clock loss";
    static const CHAR cUnknown[]       = "???";
#if defined KINETIS_KE                                                   // {11}
    static const CHAR cHostDebug[]     = "Host debugger";
    static const CHAR cPerFailure[]    = "peripheral failure";
    if ((SIM_SRSID & SIM_SRSID_POR) != 0) {                              // power on reset
        ptrStr = cPowerOn;
    }
    else if ((SIM_SRSID & SIM_SRSID_LVD) != 0) {                         // low voltage detector
        ptrStr = cUndervoltage;
    }
    else if ((SIM_SRSID & SIM_SRSID_LOC) != 0) {                         // loss of clock
        ptrStr = cClockLoss;
    }
    else if ((SIM_SRSID & SIM_SRSID_WDOG) != 0) {                        // watchdog
        ptrStr = cWatchdog;
    }
    else if ((SIM_SRSID & SIM_SRSID_PIN) != 0) {                         // reset pin
        ptrStr = cResetInput;
    }
    else if ((SIM_SRSID & SIM_SRSID_LOCKUP) != 0) {                      // core lockup
        ptrStr = cLockup;
    }
    else if ((SIM_SRSID & SIM_SRSID_SW) != 0) {                          // software reset
        ptrStr = cSoftwareReset;
    }
    else if ((SIM_SRSID & SIM_SRSID_MDMAP) != 0) {                       // host debugger
        ptrStr = cHostDebug;
    }
    else if ((SIM_SRSID & SIM_SRSID_SACKERR) != 0) {                     // peripheral failed to acknowledge attempt to enter stop mode
        ptrStr = cPerFailure;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
#elif defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000) // {7}
    static const CHAR cHostDebug[]     = "Host debugger";
    static const CHAR cEZPORT[]        = "EZPORT";
    static const CHAR cPerFailure[]    = "peripheral failure";
    static const CHAR cTamper[]        = "tamper";

    if ((RCM_SRS0 & RCM_SRS0_POR) != 0) {                                // power on reset
        ptrStr = cPowerOn;
    }
    else if ((RCM_SRS0 & RCM_SRS0_LVD) != 0) {                           // low voltage detector
        ptrStr = cUndervoltage;
    }
    else if ((RCM_SRS0 & RCM_SRS0_LOC) != 0) {                           // loss of external clock
        ptrStr = cClockLoss;
    }
    else if ((RCM_SRS0 & RCM_SRS0_WDOG) != 0) {                          // watchdog
        ptrStr = cWatchdog;
    }
    else if ((RCM_SRS0 & RCM_SRS0_WAKEUP) != 0) {
        if ((RCM_SRS0 & RCM_SRS0_PIN) != 0) {
            ptrStr = cWakeup;                                            // wakeup via reset pin
        }
        else {
            ptrStr = cWakeupOther;                                       // wakeup from other source
        }
    }
    else if ((RCM_SRS0 & RCM_SRS0_PIN) != 0) {                           // reset pin
        ptrStr = cResetInput;
    }
    else if ((RCM_SRS1 & RCM_SRS1_JTAG) != 0) {                          // jtag
        ptrStr = cJtag;
    }
    else if ((RCM_SRS1 & RCM_SRS1_LOCKUP) != 0) {                        // core lockup
        ptrStr = cLockup;
    }
    else if ((RCM_SRS1 & RCM_SRS1_SW) != 0) {                            // software reset
        ptrStr = cSoftwareReset;
    }
    else if ((RCM_SRS1 & RCM_SRS1_MDM_AP) != 0) {                        // host debugger
        ptrStr = cHostDebug;
    }
    else if ((RCM_SRS1 & RCM_SRS1_EZPT) != 0) {                          // EZPORT reset
        ptrStr = cEZPORT;
    }
    else if ((RCM_SRS1 & RCM_SRS1_SACKERR) != 0) {                       // peripheral failed to acknowledge attempt to enter stop mode
        ptrStr = cPerFailure;
    }
    else if ((RCM_SRS1 & RCM_SRS1_TAMPER) != 0) {                        // tamper detect
        ptrStr = cTamper;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
#else
    if ((MC_SRSH & MC_SRSH_JTAG) != 0) {                                 // jtag reset
        ptrStr = cJtag;
    }
    else if ((MC_SRSH & MC_SRSH_LOCKUP) != 0) {                          // lockup reset
        ptrStr = cLockup;
    }
    else if ((MC_SRSH & MC_SRSH_SW) != 0) {                              // software reset
        ptrStr = cSoftwareReset;
    }
    else if (MC_SRSL == (MC_SRSL_POR | MC_SRSL_LVD)) {                   // power on reset
        ptrStr = cPowerOn;
    }
    else if (MC_SRSL == MC_SRSL_LVD) {                                   // low voltage detect reset
        ptrStr = cUndervoltage;
    }
    else if (MC_SRSL == (MC_SRSL_PIN | MC_SRSL_WAKEUP)) {                // low leakage wakeup reset due to reset pin
        ptrStr = cWakeup;
    }
    else if (MC_SRSL == MC_SRSL_WAKEUP) {                                // low voltage wakeup due to other source
        ptrStr = cWakeupOther;
    }
    else if ((MC_SRSL & MC_SRSL_PIN) != 0) {                             // reset input
        ptrStr = cResetInput;
    }
    else if ((MC_SRSL & MC_SRSL_COP) != 0) {                             // watchdog reset
        ptrStr = cWatchdog;
    }
    else if ((MC_SRSL & MC_SRSL_LOC) != 0) {                             // clock-loss
        ptrStr = cClockLoss;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
#endif
    if (ptrBuffer == 0) {                                                // {8}
        fnDebugMsg((CHAR *)ptrStr);
        return 0;
    }
    return (uStrcpy(ptrBuffer, ptrStr) - ptrBuffer);                     // return the length of the string
}


#if defined SUPPORT_LOW_POWER                                            // {12}
// Display the low power modes that the processor offers and show presently active one
//
extern void fnShowLowPowerMode(void)
{
    int iMode;
    int iPresentMode;
    // Determine the present low power mode of operation from the SMC settings
    //
    iPresentMode = fnGetLowPowerMode();                                  // get the present mode from the device

    for (iMode = 0; iMode <= MAX_LP_MODES; iMode++) {                    // display the possible low power modes
        switch (iMode) {
        case RUN_MODE:
            fnDebugMsg("0 = RUN");                                       // no low power mode used
            break;
        case WAIT_MODE:
            fnDebugMsg("1 = WAIT");
            break;
        case STOP_MODE:
            fnDebugMsg("2 = STOP");
            break;
    #if defined KINETIS_K22
        case VLPR_MODE:
            fnDebugMsg("3 = VLPR");
            break;
        case VLPW_MODE:
            fnDebugMsg("4 = VLPW");
            break;
        case VLPS_MODE:
            fnDebugMsg("5 = VLPS");
            break;
        case LLS2_MODE:
            fnDebugMsg("6 = LLS2");
            break;
        case LLS3_MODE:
            fnDebugMsg("7 = LLS3");
            break;
        case VLLS0_MODE:
            fnDebugMsg("8 = VLLS0");
            break;
        case VLLS1_MODE:
            fnDebugMsg("9 = VLLS1");
            break;
        case VLLS2_MODE:
            fnDebugMsg("10 = VLLS1");
            break;
        case VLLS3_MODE:
            fnDebugMsg("11 = VLLS3");
            break;
    #elif defined KINETIS_KL27
        case VLPR_MODE:
            fnDebugMsg("3 = VLPR");
            break;
        case VLPW_MODE:
            fnDebugMsg("4 = VLPW");
            break;
        case VLPS_MODE:
            fnDebugMsg("5 = VLPS");
            break;
        case LLS_MODE:
            fnDebugMsg("6 = LLS");
            break;
        case VLLS0_MODE:
            fnDebugMsg("7 = VLLS0");
            break;
        case VLLS1_MODE:
            fnDebugMsg("8 = VLLS1");
            break;
        case VLLS3_MODE:
            fnDebugMsg("9 = VLLS3");
            break;
    #elif defined KINETIS_KL03
        case PSTOP1_MODE:
            fnDebugMsg("3 = PSTOP1");
            break;
        case PSTOP2_MODE:
            fnDebugMsg("4 = PSTOP2");
            break;
        case VLPR_MODE:
            fnDebugMsg("5 = VLPR");
            break;
        case VLPW_MODE:
            fnDebugMsg("6 = VLPW");
            break;
        case VLPS_MODE:
            fnDebugMsg("7 = VLPS");
            break;
        case VLLS0_MODE:
            fnDebugMsg("8 = VLLS0");
            break;
        case VLLS1_MODE:
            fnDebugMsg("9 = VLLS1");
            break;
        case VLLS3_MODE:
            fnDebugMsg("10 = VLLS3");
            break;
    #elif !defined KINETIS_KE
        case VLPR_MODE:
            fnDebugMsg("3 = VLPR");
            break;
        case VLPS_MODE:
            fnDebugMsg("4 = VLPS");
            break;
        case LLS_MODE:
            fnDebugMsg("5 = LLS");
            break;
        case VLLS0_MODE:
            fnDebugMsg("6 = VLLS0");
            break;
        case VLLS1_MODE:
            fnDebugMsg("7 = VLLS1");
            break;
        #if defined KINETIS_KL
        case VLLS3_MODE:
            fnDebugMsg("8 = VLLS3");
            break;
        #else
        case VLLS2_MODE:
            fnDebugMsg("8 = VLLS2");
            break;
        case VLLS3_MODE:
            fnDebugMsg("9 = VLLS3");
            break;
        #endif
    #endif
        }
        if (iPresentMode == iMode) {
            fnDebugMsg(" [active]");                                     // this mode is presently active
        }
        fnDebugMsg("\r\n");
    }
}


    #if defined LOW_POWER_CYCLING_MODE
extern int fnVirtualWakeupInterruptHandler(int iDeepSleep)               // {13}
{
    if (iDeepSleep == 0) {
        return 0;                                                        // only loop in deep sleep modes (not wait based)
    }
    #if defined FRDM_K22F
    if (fnIsPending(irq_UART1_ID) != 0) {                                // if there is a pending interrupt from the UART
        iLowPowerLoopMode = LOW_POWER_CYCLING_PAUSED;
        return 0;
    }
    if (fnIsPending(irq_RTC_SECONDS_ID) != 0) {                          // if there is a pending interrupt from the RTC
        iLowPowerLoopMode = LOW_POWER_CYCLING_PAUSED;
        return 0;
    }
    if (fnIsPending(irq_DMA4_ID) != 0) {                                 // if there is a pending interrupt from the UART tx DMA
        iLowPowerLoopMode = LOW_POWER_CYCLING_PAUSED;
        return 0;
    }
    #endif
    #if defined TICK_USES_LPTMR
    TOGGLE_TEST_OUTPUT();
    LPTMR0_CSR = LPTMR0_CSR;                                             // clear pending interrupt at LPTMR (wakeup source)
    WRITE_ONE_TO_CLEAR(*(volatile unsigned char *)(LLWU_FLAG_ADDRESS + 2), MODULE_LPTMR0); // reset the wakeup flag (write '1' to clear)
    fnClearPending(irq_LL_wakeup_ID);
    fnClearPending(irq_LPT_ID);
    if (++iLowPowerLoopMode > (DELAY_LIMIT)(0.2 * SEC)) {                // take over basic tick operation to retrigger the watchog every 200ms
        fnRetriggerWatchdog();
        iLowPowerLoopMode = LOW_POWER_CYCLING_ENABLED;
    }
    TOGGLE_TEST_OUTPUT();
    #endif
    return 1;                                                            // stay in low power cycling mode
}
    #endif
#endif
