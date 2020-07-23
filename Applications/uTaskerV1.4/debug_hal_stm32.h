/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      debug_hal_stm32.h
    Project:   uTasker Demonstration project
               - hardware application layer for STM32
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    This file includes a number of hardware specific routines that are
    included in debug.c. This include file allows the main content of debug.c
    to be hardware independent and thus shared by all processor projects. 
    In addition it makes adding specific setups for different processor boards
    possible by exchanging this file or by including multiple sets of the
    routines in it, controlled by defines of each target type

*/


#if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS

// Check the present state of a particular port
//
extern int fnPortState(CHAR cPortBit)
{
    #if defined STM3241G_EVAL
    switch (cPortBit) {
    case '1':
        return (_READ_PORT_MASK(G, LED1) != 0);
    case '2':
        return (_READ_PORT_MASK(G, LED2) != 0);
    case '3':
        return (_READ_PORT_MASK(I, LED3) != 0);
    case '4':
        return (_READ_PORT_MASK(C, LED4) != 0);
    }
    #elif defined STM32F746G_DISCO
    switch (cPortBit) {
    case '1':
        return (_READ_PORT_MASK(I, LED1) != 0);
    case '2':
        return (_READ_PORT_MASK(I, LED2) != 0);
    case '3':
        return (_READ_PORT_MASK(I, LED3) != 0);
    case '4':
        return (_READ_PORT_MASK(I, LED4) != 0);
    }
    #elif defined WISDOM_STM32F407
    switch (cPortBit) {
    case '1':
        return (_READ_PORT_MASK(E, LED1) != 0);
    case '2':
        return (_READ_PORT_MASK(E, LED2) != 0);
    case '3':
        return (_READ_PORT_MASK(E, LED3) != 0);
    case '4':
        return (_READ_PORT_MASK(E, LED4) != 0);
    }
    #elif defined NUCLEO_F401RE
    switch (cPortBit) {
    case '1':
        return (_READ_PORT_MASK(A, LED1) != 0);
    case '2':
        return (_READ_PORT_MASK(A, LED2) != 0);
    case '3':
        return (_READ_PORT_MASK(A, LED3) != 0);
    case '4':
        return (_READ_PORT_MASK(A, LED4) != 0);
    }
    #elif defined STM3210C_EVAL
    switch (cPortBit) {
    case '1':
        return (_READ_PORT_MASK(D, LED4) != 0);
    case '2':
        return (_READ_PORT_MASK(D, LED3) != 0);
    case '3':
        return (_READ_PORT_MASK(D, LED1) != 0);
    case '4':
        return (_READ_PORT_MASK(D, LED2) != 0);
    }
    #elif defined STM32_P207 || defined STM32F407ZG_SK
    switch (cPortBit) {
    case '1':
        return (_READ_PORT_MASK(F, LED1) != 0);
    case '2':
        return (_READ_PORT_MASK(F, LED2) != 0);
    case '3':
        return (_READ_PORT_MASK(F, LED3) != 0);
    case '4':
        return (_READ_PORT_MASK(F, LED4) != 0);
    }
    #else                                                                // used when LEDs are all on the same port
    switch (cPortBit) {
        #if defined LED1
    case '1':
        if ((DEMO_INPUT_PORT & LED1) != 0) {
            return 1;
        }
        break;
        #endif
        #if defined LED2
    case '2':
        if ((DEMO_INPUT_PORT & LED2) != 0) {
            return 1;
        }
        break;
        #endif
        #if defined LED3
    case '3':
        if ((DEMO_INPUT_PORT & LED3) != 0) {
            return 1;
        }
        break;
        #endif
        #if defined LED4
    case '4':
        if ((DEMO_INPUT_PORT & LED4) != 0) {
            return 1;
        }
        break;
        #endif
    default:
        break;
    }
    #endif
    return 0;
}


// Check whether the port is configured as an input or as output
//
extern int fnPortInputConfig(CHAR cPortBit)
{
#if defined STM3210C_EVAL                                                // this device doesn't have a conventional DDR register
    switch (cPortBit) {
    case '1':
        if (GPIOD_CRL & 0x00030000) {                                    // PORTD_BIT4 [(1 << (4 * 4)) | (1 << ((4 * 4) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '2':
        if (GPIOD_CRL & 0x00003000) {                                    // PORTD_BIT3 [(1 << (3 * 4)) | (1 << ((3 * 4) + 1))]
            return 0;
        }
        break;
    case '3':
        if (GPIOD_CRL & 0x30000000) {                                    // PORTD_BIT7 [(1 << (7 * 4)) | (1 << ((7 * 4) + 1))]
            return 0;
        }
        break;
    case '4':
        if (GPIOD_CRH & 0x00300000) {                                    // PORTD_BIT13 [(1 << (5 * 4)) | (1 << ((5 * 4) + 1))]
            return 0;
        }
        break;
    default:
        break;
    }
#elif defined WISDOM_STM32F407
    switch (cPortBit) {
    case '1':
        if ((GPIOE_MODER & 0x00030000) == 0x00010000) {                  // PORTE_BIT8 [(1 << (8 * 4)) | (1 << ((8 * 4) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '2':
        if ((GPIOE_MODER & 0x000c0000) == 0x00040000) {                  // PORTE_BIT9 [(1 << (9 * 4)) | (1 << ((9 * 4) + 1))]
            return 0;
        }
        break;
    case '3':
        if ((GPIOE_MODER & 0x00300000) == 0x00100000) {                  // PORTE_BIT10 [(1 << (10 * 4)) | (1 << ((10 * 4) + 1))]
            return 0;
        }
        break;
    case '4':
        if ((GPIOE_MODER & 0x00c00000) == 0x00400000) {                  // PORTE_BIT11 [(1 << (11 * 4)) | (1 << ((11 * 4) + 1))]
            return 0;
        }
        break;
    default:
        break;
    }
#elif defined NUCLEO_F401RE
    switch (cPortBit) {
    case '1':
        if ((GPIOA_MODER & 0x00000c00) == 0x00000400) {                  // PORTA_BIT5 [(1 << (5 * 4)) | (1 << ((5 * 4) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '2':
        if ((GPIOA_MODER & 0x00003000) == 0x00001000) {                  // PORTA_BIT6 [(1 << (6 * 4)) | (1 << ((6 * 4) + 1))]
            return 0;
        }
        break;
    case '3':
        if ((GPIOA_MODER & 0x0000c000) == 0x00004000) {                  // PORTA_BIT7 [(1 << (7 * 4)) | (1 << ((7 * 4) + 1))]
            return 0;
        }
        break;
    case '4':
        if ((GPIOA_MODER & 0x00030000) == 0x00010000) {                  // PORTA_BIT8 [(1 << (8 * 4)) | (1 << ((8 * 4) + 1))]
            return 0;
        }
        break;
    default:
        break;
    }
#elif defined STM3241G_EVAL
    switch (cPortBit) {
    case '1':
        if ((GPIOG_MODER & 0x00003000) == 0x00001000) {                  // PORTG_BIT6 [(1 << (6 * 2)) | (1 << ((6 * 2) + 1))]
            return 0;
        }
        break;
    case '2':
        if ((GPIOG_MODER & 0x00030000) == 0x00010000) {                  // PORTG_BIT8 [(1 << (8 * 2)) | (1 << ((8 * 2) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '3':
        if ((GPIOI_MODER & 0x000c0000) == 0x00040000) {                  // PORTI_BIT9 [(1 << (9 * 2)) | (1 << ((9 * 2) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '4':
        if ((GPIOC_MODER & 0x0000c000) == 0x00004000) {                  // PORTC_BIT7 [(1 << (7 * 2)) | (1 << ((7 * 2) + 1))]
            return 0;
        }
        break;
    }
#elif defined STM32F746G_DISCO
    switch (cPortBit) {
    case '1':
        if ((GPIOI_MODER & 0x0000000c) == 0x00000004) {                  // PORTI_BIT1 [(1 << (1 * 2)) | (1 << ((1 * 2) + 1))]
            return 0;
        }
        break;
    case '2':
        if ((GPIOI_MODER & 0x00000030) == 0x00000010) {                  // PORTI_BIT2 [(1 << (2 * 2)) | (1 << ((2 * 2) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '3':
        if ((GPIOI_MODER & 0x000000c0) == 0x00000040) {                  // PORTI_BIT3 [(1 << (3 * 2)) | (1 << ((3 * 2) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '4':
        if ((GPIOI_MODER & 0x00000300) == 0x00000100) {                  // PORTI_BIT4 [(1 << (4 * 2)) | (1 << ((4 * 2) + 1))]
            return 0;
        }
        break;
    }
#elif defined NUCLEO_F429ZI || defined NUCLEO_F746ZG
    switch (cPortBit) {
    case '1':
    #define _PORTBIT_IS_INPUT(ref, bit_number)  ((GPIO##ref##_MODER & ((1 << (bit_number * 2)) | ((1 << ((bit_number * 2) + 1))))) == (1 << (bit_number * 2)))
        if (_PORTBIT_IS_INPUT(B, 0) != 0) {                              // PORTB_BIT0
            return 0;                                                    // configured as output
        }
        break;
    case '2':
        if (_PORTBIT_IS_INPUT(B, 7) != 0) {                              // PORTB_BIT7
            return 0;                                                    // configured as output
        }
        break;
    case '3':
        if (_PORTBIT_IS_INPUT(B, 14) != 0) {                             // PORTB_BIT14
            return 0;                                                    // configured as output
        }
        break;
    default:
        break;
    }
#elif defined STM32_P207 || defined STM32F407ZG_SK
    switch (cPortBit) {
    case '1':
        if ((GPIOF_MODER & 0x00003000) == 0x00001000) {                  // PORTF_BIT6 [(1 << (6 * 2)) | (1 << ((6 * 2) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '2':
        if ((GPIOF_MODER & 0x0000c000) == 0x00004000) {                  // PORTF_BIT7
            return 0;
        }
        break;
    case '3':
        if ((GPIOF_MODER & 0x00030000) == 0x00010000) {                  // PORTF_BIT8
            return 0;
        }
        break;
    case '4':
        if ((GPIOF_MODER & 0x000c0000) == 0x00040000) {                  // PORTF_BIT9
            return 0;
        }
        break;
    default:
        break;
    }
#elif defined NUCLEO_F429ZI || defined NUCLEO_L432KC || defined NUCLEO_L031K6 || defined NUCLEO_L011K4 || defined NUCLEO_F031K6 || defined NUCLEO_L496RG || defined NUCLEO_F746ZG
    switch (cPortBit) {
    case '1':
        if ((GPIOB_MODER & 0x00000030) == 0x00000010) {                  // PORTB_BIT3 [(1 << (3 * 2)) | (1 << ((3 * 2) + 1))]
            return 0;                                                    // configured as output
        }
        break;
    case '2':
        if ((GPIOB_MODER & 0x000000c0) == 0x00000040) {                  // PORTB_BIT4
            return 0;
        }
        break;
    case '3':
        if ((GPIOB_MODER & 0x00000300) == 0x00000100) {                  // PORTB_BIT5
            return 0;
        }
        break;
    default:
        break;
    }
#endif
    return 1;                                                            // configured as an input
}



// Configure a processor port pin as input/output or analogue
//
extern int fnConfigPort(CHAR cPortBit, CHAR cType)
{
    cPortBit -= '0';                                                     // select bit offset

    switch (cType) {
    case 'd':                                                            // default use for 0..4
    case 'i':                                                            // port to be input
        switch (cPortBit) {
#if defined STM3241G_EVAL
        case 0:
            _CONFIG_PORT_INPUT(G, (LED1), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(G, (LED2), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;     // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(I, (LED3), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_3;     // set present bit as input
            break;
        case 3:
            _CONFIG_PORT_INPUT(C, (LED4), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_4;     // set present bit as input
            break;
#elif defined STM32F746G_DISCO
        case 0:
            _CONFIG_PORT_INPUT(I, (LED1), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(I, (LED2), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;     // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(I, (LED3), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_3;     // set present bit as input
            break;
        case 3:
            _CONFIG_PORT_INPUT(I, (LED4), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_4;     // set present bit as input
            break;
#elif defined WISDOM_STM32F407
        case 0:
            _CONFIG_PORT_INPUT(E, (LED1), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(E, (LED2), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;     // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(E, (LED3), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_3;     // set present bit as input
            break;
        case 3:
            _CONFIG_PORT_INPUT(E, (LED4), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_4;     // set present bit as input
            break;
#elif defined NUCLEO_F401RE
        case 0:
            _CONFIG_PORT_INPUT(A, (LED1), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(A, (LED2), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;     // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(A, (LED3), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_3;     // set present bit as input
            break;
        case 3:
            _CONFIG_PORT_INPUT(A, (LED4), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_4;     // set present bit as input
            break;
#elif defined ST_MB913C_DISCOVERY || defined ARDUINO_BLUE_PILL
        case 0:
            _CONFIG_PORT_INPUT(C, (DEMO_LED_2 << PORT_SHIFT), (INPUT_PULL_UP)); // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(C, (DEMO_LED_1 << PORT_SHIFT), (INPUT_PULL_UP)); // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
#elif defined NUCLEO_F429ZI || defined NUCLEO_L432KC || defined NUCLEO_L031K6 || defined NUCLEO_L011K4 || defined NUCLEO_F031K6 || defined NUCLEO_L496RG || defined NUCLEO_F746ZG
        case 0:
            _CONFIG_PORT_INPUT(B, (LED1), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~0x01;           // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(B, (LED2), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~0x02;           // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(B, (LED3), (INPUT_PULL_UP));              // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~0x04;           // set present bit as input
            break;
#elif defined ST_MB997A_DISCOVERY
        case 0:
            _CONFIG_PORT_INPUT(D, (DEMO_LED_1 << PORT_SHIFT), (INPUT_PULL_UP));// configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(D, (DEMO_LED_2 << PORT_SHIFT), (INPUT_PULL_UP));// configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;      // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(D, (DEMO_LED_3 << PORT_SHIFT), (INPUT_PULL_UP));// configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_3;     // set present bit as input
            break;
        case 3:
            _CONFIG_PORT_INPUT(D, (DEMO_LED_4 << PORT_SHIFT), (INPUT_PULL_UP));// configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_4;     // set present bit as input
            break;
#elif defined STM32_P207 || defined STM32F407ZG_SK
        case 0:
            _CONFIG_PORT_INPUT(F, LED1, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(F, LED2, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;     // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(F, LED3, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_3;     // set present bit as input
            break;
        case 3:
            _CONFIG_PORT_INPUT(F, LED4, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_4;     // set present bit as input
            break;
#else                                                                    // STM3210C_EVAL
        case 0:
            _CONFIG_PORT_INPUT(D, LED4, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_1;     // set present bit as input
            break;
        case 1:
            _CONFIG_PORT_INPUT(D, LED3, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_2;     // set present bit as input
            break;
        case 2:
            _CONFIG_PORT_INPUT(D, LED1, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_4;     // set present bit as input
            break;
        case 3:
            _CONFIG_PORT_INPUT(D, LED2, (INPUT_PULL_UP));                // configure as input with pull-up
            temp_pars->temp_parameters.ucUserOutputs &= ~DEMO_LED_3;     // set present bit as input
            break;
#endif
        default:
            break;
        }
        break;

    case 'o':                                                            // port to be output
        switch (cPortBit) {
#if defined STM3241G_EVAL
        case 0:
            _CONFIG_PORT_OUTPUT(G, (LED1), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(G, (LED2), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;     // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(I, (LED3), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_3;     // set present bit as output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(C, (LED4), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_4;     // set present bit as output
            break;
#elif defined STM32F746G_DISCO
        case 0:
            _CONFIG_PORT_OUTPUT(I, (LED1), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(I, (LED2), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;     // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(I, (LED3), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_3;     // set present bit as output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(I, (LED4), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_4;     // set present bit as output
            break;
#elif defined WISDOM_STM32F407
        case 0:
            _CONFIG_PORT_OUTPUT(E, (LED1), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(E, (LED2), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;     // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(E, (LED3), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_3;     // set present bit as output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(E, (LED4), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_4;     // set present bit as output
            break;
#elif defined NUCLEO_F401RE
        case 0:
            _CONFIG_PORT_OUTPUT(A, (LED1), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(A, (LED2), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;     // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(A, (LED3), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_3;     // set present bit as output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(A, (LED4), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_4;     // set present bit as output
            break;

#elif defined ST_MB913C_DISCOVERY || defined ARDUINO_BLUE_PILL
        case 0:
            _CONFIG_PORT_OUTPUT(C, (DEMO_LED_2 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(C, (DEMO_LED_1 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
#elif defined NUCLEO_F429ZI || defined NUCLEO_L432KC || defined NUCLEO_L031K6 || defined NUCLEO_L011K4 || defined NUCLEO_F031K6 || defined NUCLEO_L496RG || defined NUCLEO_F746ZG
        case 0:
            _CONFIG_PORT_OUTPUT(B, (LED1), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= 0x01;            // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(B, (LED2), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= 0x02;            // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(B, (LED3), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= 0x04;            // set present bit as output
            break;
#elif defined ST_MB997A_DISCOVERY
        case 0:
            _CONFIG_PORT_OUTPUT(D, (DEMO_LED_1 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(D, (DEMO_LED_2 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;      // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(D, (DEMO_LED_3 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_3;      // set present bit as output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(D, (DEMO_LED_4 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)) // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_4;      // set present bit as output
            break;
#elif defined STM32_P207 || defined STM32F407ZG_SK
        case 0:
            _CONFIG_PORT_OUTPUT(F, (DEMO_LED_1 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(F, (DEMO_LED_2 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;      // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(F, (DEMO_LED_3 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_3;      // set present bit as output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(F, (DEMO_LED_4 << PORT_SHIFT), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_4;      // set present bit as output
            break;
#else                                                                    // STM3210C_EVAL
        case 0:
            _CONFIG_PORT_OUTPUT(D, LED4, (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_1;      // set present bit as output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(D, LED3, (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_2;      // set present bit as output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(D, LED1, (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_4;      // set present bit as output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(D, LED2, (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); // configure as medium speed output with push-pull output
            temp_pars->temp_parameters.ucUserOutputs |= DEMO_LED_3;      // set present bit as output
            break;
#endif
        default:
            break;
        }
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
    #if !defined STM32_P207 && !defined STM32F407ZG_SK && !defined NUCLEO_F429ZI && !defined NUCLEO_L432KC && !defined NUCLEO_L031K6 && !defined NUCLEO_L011K4 && !defined NUCLEO_F031K6 && !defined NUCLEO_L496RG && !defined NUCLEO_F746ZG
        #if defined STM3241G_EVAL || defined WISDOM_STM32F407 || defined NUCLEO_F401RE || defined STM32F746G_DISCO
    POWER_UP_USER_PORTS();                                               // ensure that the used ports are powered up before used
        #endif
    if (iSetClr != 0) {
        switch (usBit) {
        case 0x0001:
            USER_PORT_1 |= USER_PORT_1_BIT;
            break;
        case 0x0002:
            USER_PORT_2 |= USER_PORT_2_BIT;
            break;
        case 0x0004:
            USER_PORT_3 |= USER_PORT_3_BIT;
            break;
        case 0x0008:
            USER_PORT_4 |= USER_PORT_4_BIT;
            break;
        case 0x0010:
            USER_PORT_5 |= USER_PORT_5_BIT;
            break;
        case 0x0020:
            USER_PORT_6 |= USER_PORT_6_BIT;
            break;
        case 0x0040:
            USER_PORT_7 |= USER_PORT_7_BIT;
            break;
        case 0x0080:
            USER_PORT_8 |= USER_PORT_8_BIT;
            break;
        case 0x0100:
            USER_PORT_9 |= USER_PORT_9_BIT;
            break;
        case 0x0200:
            USER_PORT_10 |= USER_PORT_10_BIT;
            break;
        case 0x0400:
            USER_PORT_11 |= USER_PORT_11_BIT;
            break;
        case 0x0800:
            USER_PORT_12 |= USER_PORT_12_BIT;
            break;
        case 0x1000:
            USER_PORT_13 |= USER_PORT_13_BIT;
            break;
        case 0x2000:
            USER_PORT_14 |= USER_PORT_14_BIT;
            break;
        case 0x4000:
            USER_PORT_15 |= USER_PORT_15_BIT;
            break;
        case 0x8000:
            USER_PORT_16 |= USER_PORT_16_BIT;
            break;
        }
    }
    else {
        switch (usBit) {
        case 0x0001:
            USER_PORT_1 &= ~USER_PORT_1_BIT;
            break;
        case 0x0002:
            USER_PORT_2 &= ~USER_PORT_2_BIT;
            break;
        case 0x0004:
            USER_PORT_3 &= ~USER_PORT_3_BIT;
            break;
        case 0x0008:
            USER_PORT_4 &= ~USER_PORT_4_BIT;
            break;
        case 0x0010:
            USER_PORT_5 &= ~USER_PORT_5_BIT;
            break;
        case 0x0020:
            USER_PORT_6 &= ~USER_PORT_6_BIT;
            break;
        case 0x0040:
            USER_PORT_7 &= ~USER_PORT_7_BIT;
            break;
        case 0x0080:
            USER_PORT_8 &= ~USER_PORT_8_BIT;
            break;
        case 0x0100:
            USER_PORT_9 &= ~USER_PORT_9_BIT;
            break;
        case 0x0200:
            USER_PORT_10 &= ~USER_PORT_10_BIT;
            break;
        case 0x0400:
            USER_PORT_11 &= ~USER_PORT_11_BIT;
            break;
        case 0x0800:
            USER_PORT_12 &= ~USER_PORT_12_BIT;
            break;
        case 0x1000:
            USER_PORT_13 &= ~USER_PORT_13_BIT;
            break;
        case 0x2000:
            USER_PORT_14 &= ~USER_PORT_14_BIT;
            break;
        case 0x4000:
            USER_PORT_15 &= ~USER_PORT_15_BIT;
            break;
        case 0x8000:
            USER_PORT_16 &= ~USER_PORT_16_BIT;
            break;
        }
    }                                                                    _SIM_PORTS; // ensure the simulator updates its port states
    #endif
}


// Request present logic level of an output
//
extern int fnUserPortState(CHAR cPortBit)
{
    #if !defined STM32_P207 && !defined STM32F407ZG_SK && !defined NUCLEO_F429ZI && !defined NUCLEO_L432KC && !defined NUCLEO_L031K6 && !defined NUCLEO_L011K4 && !defined NUCLEO_F031K6 && !defined NUCLEO_L496RG && !defined NUCLEO_F746ZG
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
    #if !defined STM32_P207 && !defined STM32F407ZG_SK && !defined NUCLEO_F429ZI && !defined NUCLEO_L432KC && !defined NUCLEO_L031K6 && !defined NUCLEO_L011K4 && !defined NUCLEO_F031K6 && !defined NUCLEO_L496RG && !defined NUCLEO_F746ZG
    switch (cPortBit) {
    case 'a':
        CONFIG_USER_PORT_1();
        break;
    case 'b':
        CONFIG_USER_PORT_2();
        break;
    case 'c':
        CONFIG_USER_PORT_3();
        break;
    case 'd':
        CONFIG_USER_PORT_4();
        break;
    case 'e':
        CONFIG_USER_PORT_5();
        break;
    case 'f':
        CONFIG_USER_PORT_6();
        break;
    case 'g':
        CONFIG_USER_PORT_7();
        break;
    case 'h':
        CONFIG_USER_PORT_8();
        break;
    case 'i':
        CONFIG_USER_PORT_9();
        break;
    case 'j':
        CONFIG_USER_PORT_10();
        break;
    case 'k':
        CONFIG_USER_PORT_11();
        break;
    case 'l':
        CONFIG_USER_PORT_12();
        break;
    case 'm':
        CONFIG_USER_PORT_13();
        break;
    case 'n':
        CONFIG_USER_PORT_14();
        break;
    case 'o':
        CONFIG_USER_PORT_15();
        break;
    case 'p':
        CONFIG_USER_PORT_16();
        break;
    default:
        return 1;
    }                                                                    _SIM_PORTS; // ensure the simulator updates its port states
    #endif
    return 0;
}


// Toggle the state of an output port
//
extern int fnTogglePortOut(CHAR cPortBit)
{
    #if !defined STM32_P207 && !defined STM32F407ZG_SK && !defined NUCLEO_F429ZI && !defined NUCLEO_L432KC && !defined NUCLEO_L031K6 && !defined NUCLEO_L011K4 && !defined NUCLEO_F031K6 && !defined NUCLEO_L496RG && !defined NUCLEO_F746ZG
    switch (cPortBit) {
    case 'a':
        USER_PORT_1 ^= USER_PORT_1_BIT;
        break;
    case 'b':
        USER_PORT_2 ^= USER_PORT_2_BIT;
        break;
    case 'c':
        USER_PORT_3 ^= USER_PORT_3_BIT;
        break;
    case 'd':
        USER_PORT_4 ^= USER_PORT_4_BIT;
        break;
    case 'e':
        USER_PORT_5 ^= USER_PORT_5_BIT;
        break;
    case 'f':
        USER_PORT_6 ^= USER_PORT_6_BIT;
        break;
    case 'g':
        USER_PORT_7 ^= USER_PORT_7_BIT;
        break;
    case 'h':
        USER_PORT_8 ^= USER_PORT_8_BIT;
        break;
    case 'i':
        USER_PORT_9 ^= USER_PORT_9_BIT;
        break;
    case 'j':
        USER_PORT_10 ^= USER_PORT_10_BIT;
        break;
    case 'k':
        USER_PORT_11 ^= USER_PORT_11_BIT;
        break;
    case 'l':
        USER_PORT_12 ^= USER_PORT_12_BIT;
        break;
    case 'm':
        USER_PORT_13 ^= USER_PORT_13_BIT;
        break;
    case 'n':
        USER_PORT_14 ^= USER_PORT_14_BIT;
        break;
    case 'o':
        USER_PORT_15 ^= USER_PORT_15_BIT;
        break;
    case 'p':
        USER_PORT_16 ^= USER_PORT_16_BIT;
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
    #if defined STM3241G_EVAL || defined WISDOM_STM32F407 || defined NUCLEO_F401RE || defined STM32F746G_DISCO
    unsigned char ucBit = 0x01;
    while (ucBit != 0) {                                                 // for each possible output
        if (DEMO_USER_PORTS & ucBit) {                                   // if the port bit is to be an output
            switch (ucBit) {
            case DEMO_LED_1:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOGEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_1) {
                    _SETBITS(G, LED1);
                }
                else {
                    _CLEARBITS(G, LED1);
                }
                break;
            case DEMO_LED_2:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOGEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_2) {
                    _SETBITS(G, LED2);
                }
                else {
                    _CLEARBITS(G, LED2);
                }
                break;
            case DEMO_LED_3:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOIEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_3) {
                    _SETBITS(I, LED3);
                }
                else {
                    _CLEARBITS(I, LED3);
                }
                break;
            case DEMO_LED_4:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOCEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_4) {
                    _SETBITS(C, LED4);
                }
                else {
                    _CLEARBITS(C, LED4);
                }
                break;
            }
        }
        ucBit <<= 1;
    }
    #elif defined NUCLEO_F429ZI || defined NUCLEO_L432KC || defined NUCLEO_L031K6 || defined NUCLEO_L011K4 || defined NUCLEO_F031K6 || defined NUCLEO_L496RG || defined NUCLEO_F746ZG
    unsigned char ucBit = 0x01;
    while (ucBit != 0) {                                                 // for each possible output
        if ((0x07 & ucBit) != 0) {                                       // if the port bit is to be an output
            switch (ucBit) {
            case 0x01:
                if (iInitialisation != 0) {
                    __POWER_UP_GPIO(B);                                  // ensure port is powered up
                }
                if ((ucPortOutputs & 0x01) != 0) {
                    _SETBITS(B, LED1);
                }
                else {
                    _CLEARBITS(B, LED1);
                }
                break;
            case 0x02:
                if (iInitialisation != 0) {
                    __POWER_UP_GPIO(B);                                  // ensure port is powered up
                }
                if ((ucPortOutputs & 0x02) != 0) {
                    _SETBITS(B, LED2);
                }
                else {
                    _CLEARBITS(B, LED2);
                }
                break;
            case 0x04:
                if (iInitialisation != 0) {
                    __POWER_UP_GPIO(B);                                  // ensure port is powered up
                }
                if ((ucPortOutputs & 0x04) != 0) {
                    _SETBITS(B, LED3);
                }
                else {
                    _CLEARBITS(B, LED3);
                }
                break;
            }
        }
        ucBit <<= 1;
    }
    #elif defined STM3210C_EVAL
    unsigned char ucBit = 0x01;
    while (ucBit != 0) {                                                 // for each possible output
        if (DEMO_USER_PORTS & ucBit) {                                   // if the port bit is to be an output
            switch (ucBit) {
            case DEMO_LED_1:
                if (iInitialisation != 0) {
                    POWER_UP(APB2, RCC_APB2ENR_IOPDEN);                  // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_1) {
                    _SETBITS(D, LED4);
                }
                else {
                    _CLEARBITS(D, LED4);
                }
                break;
            case DEMO_LED_2:
                if (iInitialisation != 0) {
                    POWER_UP(APB2, RCC_APB2ENR_IOPDEN);                  // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_2) {
                    _SETBITS(D, LED3);
                }
                else {
                    _CLEARBITS(D, LED3);
                }
                break;
            case DEMO_LED_3:
                if (iInitialisation != 0) {
                    POWER_UP(APB2, RCC_APB2ENR_IOPDEN);                  // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_3) {
                    _SETBITS(D, LED1);
                }
                else {
                    _CLEARBITS(D, LED1);
                }
                break;
            case DEMO_LED_4:
                if (iInitialisation != 0) {
                    POWER_UP(APB2, RCC_APB2ENR_IOPDEN);                  // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_4) {
                    _SETBITS(D, LED2);
                }
                else {
                    _CLEARBITS(D, LED2);
                }
                break;
            }
        }
        ucBit <<= 1;
    }
    #elif defined STM32_P207 || defined STM32F407ZG_SK
    unsigned char ucBit = 0x01;
    while (ucBit != 0) {                                                 // for each possible output
        if (DEMO_USER_PORTS & ucBit) {                                   // if the port bit is to be an output
            switch (ucBit) {
            case DEMO_LED_1:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOFEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_1) {
                    _SETBITS(F, LED1);
                }
                else {
                    _CLEARBITS(F, LED1);
                }
                break;
            case DEMO_LED_2:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOFEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_2) {
                    _SETBITS(F, LED2);
                }
                else {
                    _CLEARBITS(F, LED2);
                }
                break;
            case DEMO_LED_3:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOFEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_3) {
                    _SETBITS(F, LED3);
                }
                else {
                    _CLEARBITS(F, LED3);
                }
                break;
            case DEMO_LED_4:
                if (iInitialisation != 0) {
                    POWER_UP(AHB1, RCC_AHB1ENR_GPIOFEN);                 // ensure port is powered up
                }
                if (ucPortOutputs & DEMO_LED_4) {
                    _SETBITS(F, LED4);
                }
                else {
                    _CLEARBITS(F, LED4);
                }
                break;
            }
        }
        ucBit <<= 1;
    }
    #else
    ENABLE_LED_PORT();                                                   // ensure port is clocked and not in reset
    DEMO_LED_PORT &= ~DEMO_USER_PORTS;                                   // mask port bits
    DEMO_LED_PORT |= (ucPortOutputs & DEMO_USER_PORTS);                  // set initial output state
    #endif
}
#endif                                                                   // end USE_MAINTENANCE

#if defined EZPORT_CLONER

#define EZRESETOUT PORTA_BIT0
#define EZCSOUT    PORTA_BIT1
#define EZDIN      PORTA_BIT2
#define EZDOUT     PORTA_BIT3
#define EZCLKOUT   PORTA_BIT4

// Send and receive a single byte
//
static unsigned char fnSendEZPORT(unsigned char ucDataOut)
{
    unsigned char ucBit = 0x80;                                          // data sent and receive most significant bit first
    unsigned char ucReadByte = 0;
    do {
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        if (ucDataOut & ucBit) {                                         // set new data output state
             _SETBITS(A, EZDOUT);
        }
        else {
             _CLEARBITS(A, EZDOUT);
        }
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _CLEARBITS(A, EZCLKOUT);                                         // falling clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        _SETBITS(A, EZCLKOUT);                                           // rising clock edge
        if (_READ_PORT_MASK(A, EZDIN)) {                                 // read data in
            ucReadByte |= ucBit;
        }
        ucBit >>= 1;
    } while (ucBit != 0);
    return ucReadByte;
}

static void fnConfigEz(void)
{
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (EZRESETOUT | EZCSOUT | EZDOUT | EZCLKOUT), (OUTPUT_SLOW | OUTPUT_PUSH_PULL),  (EZDOUT | EZCLKOUT));
    _CONFIG_PORT_INPUT(A, (EZDIN), (INPUT_PULL_UP));
}

static void fnNegateResetEz(void)                                        // remove reset with ezport chip select asserted
{
    _SETBITS(A, EZRESETOUT);                                             // negate the reset line
    fnDelayLoop(1);                                                      // delay for the chip select to be latched
    _SETBITS(A, EZCSOUT);                                                // negate ezport chip select
}

static void fnGetStatusEz(void)
{
    _CLEARBITS(A, EZCSOUT);                                              // assert ezport chip select - clock always starts at '1'
    fnDebugMsg("EZPORT status = ");
    fnSendEZPORT(EZCOMMAND_RDSR);                                        // command status read
    fnDebugHex(fnSendEZPORT(0xff), (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF)); // clock in status read and display value
    _SETBITS(A, EZCSOUT);                                                // terminate sequence
}

static void fnPrepareEz(CHAR *ptrInput)
{
    unsigned long ulClockSpeed = 25000000;                               // default clock speed 25MHz if nothing else entered
    unsigned char ucConfigSetting;
    if (*ptrInput != 0) {                                                // if a clock speed it passed, we calculate the correct setting for it
        ulClockSpeed = fnDecStrHex(ptrInput);                            // extract the speed in MHz
        ulClockSpeed *= 1000000;
    }
    ucConfigSetting = (unsigned char)((ulClockSpeed/2)/185000);          // the divisor needed to get around 200k (not less that 150k and not more than 200k)
    _CLEARBITS(D, EZCSOUT);                                              // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_WRCR);                                        // start commanding configuration value
    fnDebugMsg("EZPORT configured with ");
    fnSendEZPORT(ucConfigSetting);                                       // send configuration value
    fnDebugHex(ucConfigSetting, (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF));
    _SETBITS(D, EZCSOUT);                                                // terminate sequence
    fnDoHardware(DO_EZSTATUS , 0);                                       // show new status
}

static void fnBulkEraseEz(void)
{
    _CLEARBITS(D, EZCSOUT);                                              // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_BE);
    _SETBITS(D, EZCSOUT);                                                // terminate sequence
    fnDebugMsg("EZPORT bulk erase issued\r\n");
}

static void fnWriteEz(CHAR *ptrInput)
{
    if (*ptrInput != 0) {                                                // if a clock speed it passed, we calculate the correct setting for it
        unsigned char ucValue;
        _CLEARBITS(D, EZCSOUT);                                          // assert ezport chip select - clock always starts at '1'
        ucValue = (unsigned char)fnHexStrHex(ptrInput);
        fnSendEZPORT(ucValue);
        fnDebugHex(ucValue, (sizeof(unsigned char) | WITH_LEADIN));
        fnDebugMsg(" written\r\n");
        _SETBITS(D, EZCSOUT);                                            // terminate sequence
        fnDoHardware(DO_EZSTATUS , 0);                                   // show new status
    }
    else {
        fnDebugMsg("Value missing!\r\n");
    }
}
#endif

// Determine the cause of the last reset
//
extern unsigned char fnAddResetCause(CHAR *ptrBuffer)
{
    static unsigned long ulRCC_CSR     = 0xffffffff;
    static const CHAR cIndependentWD[] = "WDOG";
    static const CHAR cWindowWD[]      = "W-WDOG";
    static const CHAR cPowerOn[]       = "Power-on";
    static const CHAR cSoftware[]      = "Software";
    static const CHAR cLowPower[]      = "Low power";
    static const CHAR cPinReset[]      = "Reset pin";
    #if defined RCC_CSR_FWRSTF
    static const CHAR cFirwallReset[]  = "Firewall";
    #endif
    #if defined RCC_CSR_OBLRSTF
    static const CHAR cOBLReset[]      = "OBL";
    #endif
    #if defined RCC_CSR_V18PWRRSTF
    static const CHAR cV18PwrReset[]   = "V1.8";
    #endif
    static const CHAR cUnknown[]       = "???";
    const CHAR *ptrStr;
    if (ulRCC_CSR == 0xffffffff) {
        ulRCC_CSR = RCC_CSR;                                             // store the value for following requests
        RCC_CSR |= (RCC_CSR_RMVF);                                       // clear flags ready for next reset
    #if defined _WINDOWS
        RCC_CSR &= ~(RESET_CAUSE_FLAGS);
    #endif
    }

    if ((ulRCC_CSR & RCC_CSR_PORRSTF) != 0) {                            // power-on reset
        ptrStr = cPowerOn;
    }
    else if ((ulRCC_CSR & RCC_CSR_PINRSTF) != 0) {                       // reset pin
        ptrStr = cPinReset;
    }
    #if defined RCC_CSR_FWRSTF
    else if ((ulRCC_CSR & RCC_CSR_FWRSTF) != 0) {                        // firewall reset
        ptrStr = cFirwallReset;
    }
    #endif
    #if defined RCC_CSR_OBLRSTF
    else if ((ulRCC_CSR & RCC_CSR_OBLRSTF) != 0) {                       // options byte loading reset
        ptrStr = cOBLReset;
    }
    #endif
    #if defined RCC_CSR_V18PWRRSTF
    else if ((ulRCC_CSR & RCC_CSR_V18PWRRSTF) != 0) {                    // 1.8V domain reset
        ptrStr = cV18PwrReset;
    }
    #endif
    else if ((ulRCC_CSR & RCC_CSR_SFTRSTF) != 0) {                       // software commanded reset
        ptrStr = cSoftware;
    }
    else if ((ulRCC_CSR & RCC_CSR_IWDGRSTF) != 0) {                      // independent watchdog
        ptrStr = cIndependentWD;
    }
    else if ((ulRCC_CSR & RCC_CSR_WWDGRSTF) != 0) {                      // window watchdog
        ptrStr = cWindowWD;
    }
    else if ((ulRCC_CSR & RCC_CSR_LPWRRSTF) != 0) {                      // low-power reset
        ptrStr = cLowPower;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
    if (ptrBuffer == 0) {
        fnDebugMsg((CHAR *)ptrStr);
        return 0;
    }
    return (uStrcpy(ptrBuffer, ptrStr) - ptrBuffer);                     // return the length of the string
}

#if defined SUPPORT_LOW_POWER
// Display the low power modes that the processor offers and show presently active one
//
extern void fnShowLowPowerMode(void)
{
}
#endif
