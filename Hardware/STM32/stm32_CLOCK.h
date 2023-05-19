/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_CLOCK.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    17.10.2017 Add timers 9 and 12                                       {1}
    17.10.2017 Correct prescaler adjustment for PCLK2                    {2}

*/

#if defined _STM32L0x1
    RCC_CR = (RCC_CR_MSIRDY | RCC_CR_MSION);                             // set reset state - default is MSI at around 2.097MHz
    RCC_ICSCR = (RCC_ICSCR_MSIRANGE_2_097M);
#elif defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
    RCC_CR = (RCC_CR_MSIRANGE_4M | RCC_CR_MSIRDY | RCC_CR_MSION);        // set reset state - default is MSI at around 4MHz
#else
    RCC_CR = (0x00000080 | RCC_CR_HSIRDY | RCC_CR_HSION);                // set reset state - default is high-speed internal clock
#endif
    RCC_CFGR = 0;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    RCC_PLLCFGR = RCC_PLLCFGR_RESET_VALUE;                               // set the PLL configuration register to default
#endif
#if !defined USE_HSI_CLOCK && !defined _STM32L432 && !defined _STM32L0x1 && !defined _STM32L4X5 && !defined _STM32L4X6
    #if defined _EXTERNAL_CLOCK                                          // use external clock input rather than crystal oscillator
    RCC_CR = (0x00000080 | RCC_CR_HSIRDY | RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_HSEBYP); // enable the high-speed external clock
    #else
    RCC_CR = (0x00000080 | RCC_CR_HSIRDY | RCC_CR_HSION | RCC_CR_HSEON); // enable the high-speed external clock
    #endif
#endif
#if defined _STM32F7XX
    FLASH_ACR = (FLASH_ACR_ARTRS);                                       // reset ART accelerator
    FLASH_ACR = (FLASH_ACR_PRFTEN | FLASH_ACR_ARTEN | FLASH_WAIT_STATES); // set flash wait states appropriately and enable pre-fetch buffer and ACT
    RCC_CFGR = (_RCC_CFGR_HPRE_SYSCLK | _RCC_CFGR_PPRE1_HCLK | _RCC_CFGR_PPRE2_HCLK); // set HCLK (AHB), PCLK1 and PCLK2 speeds
#elif defined _STM32F2XX || defined _STM32F4XX
    FLASH_ACR = (FLASH_ACR_ICRST | FLASH_ACR_DCRST);                     // flush data and instruction cache
    FLASH_ACR = (/*FLASH_ACR_PRFTEN | */FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_WAIT_STATES); // set flash wait states appropriately and enable pre-fetch buffer and cache
    RCC_CFGR = (_RCC_CFGR_HPRE_SYSCLK | _RCC_CFGR_PPRE1_HCLK | _RCC_CFGR_PPRE2_HCLK); // set HCLK (AHB), PCLK1 and PCLK2 speeds
#elif defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
    FLASH_ACR = (FLASH_ACR_ICRST | FLASH_ACR_DCRST);                     // flush data and instruction cache
    FLASH_ACR = (FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_WAIT_STATES);   // set flash wait states appropriately and enable pre-fetch buffer and cache
    RCC_CFGR = (_RCC_CFGR_HPRE_SYSCLK | _RCC_CFGR_PPRE1_HCLK | _RCC_CFGR_PPRE2_HCLK); // prepare HCLK (AHB), PCLK1 and PCLK2 speeds
#elif defined _STM32L0x1
    FLASH_ACR = (FLASH_ACR_PRE_READ | FLASH_ACR_PRFTEN | FLASH_WAIT_STATES); // set flash wait states appropriately and enable pre-fetch buffer and cache
    RCC_CFGR = (_RCC_CFGR_HPRE_SYSCLK | _RCC_CFGR_PPRE1_HCLK | _RCC_CFGR_PPRE2_HCLK); // prepare HCLK (AHB), PCLK1 and PCLK2 speeds
#elif defined _CONNECTIVITY_LINE || defined _PERFORMANCE_LINE || defined _STM32F031
    FLASH_ACR = (FLASH_ACR_PRFTBE | FLASH_WAIT_STATES);                  // set flash wait states appropriately and enable pre-fetch buffer
    RCC_CFGR = (RCC_CFGR_HPRE_SYSCLK | RCC_CFGR_PPRE1_HCLK_DIV2 | RCC_CFGR_PPRE2_HCLK); // set HCLK to SYSCLK, PCLK2 to HCLK and PCLK1 to HCLK/2 - PCLK1 must not be greater than SYSCLK/2
#else
    FLASH_ACR = 0; //FLASH_ACR_HLFCYA;                                   // enable half-cycle access - to do???
    RCC_CFGR = (RCC_CFGR_HPRE_SYSCLK | RCC_CFGR_PPRE1_HCLK_DIV2 | RCC_CFGR_PPRE2_HCLK); // set HCLK to SYSCLK, PCLK2 to HCLK and PCLK1 to HCLK/2 - PCLK1 must not be greater than SYSCLK/2
#endif
#if !defined USE_HSI_CLOCK && !defined USE_MSI_CLOCK
    while ((RCC_CR & RCC_CR_HSERDY) == 0) {                              // wait until the oscillator is ready
    #if defined _WINDOWS
        RCC_CR |= RCC_CR_HSERDY;
    #endif
    }
#endif
#if defined DISABLE_PLL
    #if !defined USE_HSI_CLOCK && !defined USE_MSI_CLOCK
    RCC_CFGR |= RCC_CFGR_HSE_SELECT;                                     // set oscillator as direct source
    #elif defined _STM32L0x1 && defined USE_MSI_CLOCK                    // set MSI frequency
        #if MSI_CLOCK != 2097000
    RCC_ICSCR = (RCC_ICSCR_MISRANGE_SETTING | RCC_CR_MSIRDY | RCC_CR_MSION); // set the MSI range value and use this register's value to control it
        #endif
    #elif (defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6) && defined USE_MSI_CLOCK // set MSI frequency
        #if MSI_CLOCK != 4000000
    RCC_CR = (RCC_CR_MISRANGE_SETTING | RCC_CR_MSIRGSEL | RCC_CR_MSIRDY | RCC_CR_MSION); // set the MSI range value and use this register's value to control it
        #endif
    #elif (defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6) && defined USE_HSI_CLOCK
    RCC_CR |= (RCC_CR_HSION);                                            // turn on the 16MHz HSI oscillator
    RCC_CFGR |= (RCC_CFGR_HSI16_SELECT);                                 // switch from 4MHz MSI to HSE16
    #endif
#else
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #if SYSCLK > 144000000
    POWER_UP(APB1, (RCC_APB1ENR_PWREN));
            #if defined _STM32F7XX
    PWR_CR = PWR_CR_VOS_1;                                               // enable high performance mode when the speed is greater than 144MHz
            #else
    PWR_CR = PWR_CR_VOS;                                                 // enable high performance mode when the speed is greater than 144MHz
            #endif
        #endif
        #if defined USE_HSI_CLOCK    
    RCC_PLLCFGR = ((PLL_Q_VALUE << 24) | (PLL_P_VALUE << 16) | PLL_INPUT_DIV | (_PLL_VCO_MUL << 6) | RCC_PLLCFGR_PLLSRC_HSI);
        #else
    RCC_PLLCFGR = ((PLL_Q_VALUE << 24) | (PLL_P_VALUE << 16) | PLL_INPUT_DIV | (_PLL_VCO_MUL << 6) | RCC_PLLCFGR_PLLSRC_HSE);
        #endif
    #else
        #if defined USE_HSI_CLOCK    
    RCC_CFGR |= (((_PLL_VCO_MUL - 2) << 18));                            // set PLL multiplication factor from HSI input (divided by 2)
        #else
            #if defined _CONNECTIVITY_LINE && defined USE_PLL2_CLOCK
    // Generate an intermediate frequency on PLL2 and then use this as input to the main PLL
    //
    RCC_CFGR2 = (((PLL2_INPUT_DIV - 1) << 4) | ((_PLL2_VCO_MUL - 2) << 8) | RCC_CFGR2_PREDIV1SRC | (PLL_INPUT_DIV - 1));
    RCC_CR |= RCC_CR_PLL2ON;                                             // enable PLL2 and wait for it to become ready
    while ((RCC_CR & RCC_CR_PLL2RDY) == 0) {
                #if defined _WINDOWS
        RCC_CR |= RCC_CR_PLL2RDY;
                #endif
    }
            #else
    RCC_CFGR2 = (PLL_INPUT_DIV - 1);                                     // set PLL input pre-divide
            #endif
    RCC_CFGR |= (((_PLL_VCO_MUL - 2) << 18) | RCC_CFGR_PLL_SRC);         // set PLL multiplication factor and select the pre-divide clock source
        #endif
    #endif
    RCC_CR |= RCC_CR_PLLON;                                              // enable PLL and wait for it to become ready
    while ((RCC_CR & RCC_CR_PLLRDY) == 0) {
    #if defined _WINDOWS
        RCC_CR |= RCC_CR_PLLRDY;
    #endif
    }   
    RCC_CFGR |= RCC_CFGR_PLL_SELECT;                                     // select PLL as system clock source
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_PLL_USED) {        // wait until PLL used as system clock
    #if defined _WINDOWS
        RCC_CFGR &= ~RCC_CFGR_SWS_MASK; RCC_CFGR |= RCC_CFGR_PLL_USED;
    #endif
    }
#endif

