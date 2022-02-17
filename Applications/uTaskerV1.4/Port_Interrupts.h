/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      Port_Interrupts.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    28.10.2009 Modify SAM7X initialisation to include glitch filer configuration {1}
    30.12.2009 Modify M522xx use of IRQ7 to NMI technique                 {2}
    10.02.2010 NMI port check only with _M5223X                           {3}
    21.04.2011 Update calls from sendCAN() to fnSendCAN()                 {4}
    04.12.2011 Add Kinetis test support                                   {5}
    11.01.2015 Add wake-up test support                                   {6}
    11.12.2015 Add DMA port mirroring reference                           {7}
    Note that the external interrupt tests are not suitable for LPC210x as in this file

The file is otherwise not specifically linked in to the project since it
is included by application.c when needed.

*/

#if !defined _PORT_INTS_CONFIG
    #define _PORT_INTS_CONFIG

    #if !defined K70F150M_12M && !defined TWR_K53N512 && !defined TWR_K40X256 && !defined TWR_K40D100M && !defined KWIKSTIK
      //#define IRQ_TEST                                                 // test IRQ port interrupts
      //#define DMA_PORT_MIRRORING                                       // demonstrate using DMA to control one or more output ports to follow an input port
        #if defined SUPPORT_LOW_POWER
          //#define WAKEUP_TEST                                          // test wake-up port interrupts
        #endif
    #endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */
    #if defined IRQ_TEST || defined WAKEUP_TEST
        static void fnInitIRQ(void);

        #if defined _M5223X                                              // {2}
            static unsigned long ulNMI_event_count = 0;
            static unsigned long ulNMI_processed_count = 0;
        #endif
    #endif
#endif

#if defined _M5223X && defined _PORT_NMI_CHECK && defined IRQ_TEST       // {2}{3} check for NMI interrupt each time the application is scheduled
        while (ulNMI_event_count != ulNMI_processed_count) {             // if there are open events
            ulNMI_processed_count++;                                     // this one processed - note that ulNMI_event_count is not written since it may be modified by the NMI during such an access
            fnDebugMsg("NMI_int\r\n");
        }
#endif

#if defined _PORT_INTS_EVENTS && defined IRQ_TEST                        // monostable timer event handling
                if ((IRQ1_EVENT <= ucInputMessage[MSG_INTERRUPT_EVENT]) && (IRQ11_EVENT >= ucInputMessage[MSG_INTERRUPT_EVENT])) {
    #if defined WAKEUP_TEST
                    fnDebugMsg("WOKEN - ");
                    switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {
                    case IRQ4_EVENT:
                        fnDebugMsg("going to LSS again\r\n");
                        break;
                    case IRQ5_EVENT:
                        fnSetLowPowerMode(WAIT_MODE);
                        fnDebugMsg("restoring WAIT mode\r\n");
                        break;
                    }
    #else 
                    fnDebugMsg("IRQ_");
                    switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {
                    case IRQ1_EVENT:
                        fnDebugMsg("1");
                        break;
                    case IRQ4_EVENT:
                        fnDebugMsg("4");
        #if defined CAN_INTERFACE && defined TEST_CAN
                        fnSendCAN(1);                                    // {4}
        #endif
                        break;
                    case IRQ5_EVENT:
                        fnDebugMsg("5");
        #if defined CAN_INTERFACE && defined TEST_CAN
                        fnSendCAN(7);                                    // {4}
        #endif
                        break;
                    case IRQ7_EVENT:
                        fnDebugMsg("7");
                        break;
                    case IRQ11_EVENT:
                        fnDebugMsg("11");
                        break;
                    default:
                        break;
                    }
                    fnDebugMsg("\r\n");
                    break;
    #endif
                }
#endif






#if defined _PORT_INT_CODE && defined IRQ_TEST
// Test routines to handle the IRQ test inputs
//
    #if !defined TEST_DS1307 && !(defined _M5225X && !defined INTERRUPT_TASK_PHY)  // use this input for RTC
static void test_irq_1(void)
{
    fnInterruptMessage(OWN_TASK, IRQ1_EVENT);
}
    #endif


static void test_irq_4(void)
{
    fnInterruptMessage(OWN_TASK, IRQ4_EVENT);
    #if defined _KINETIS && !defined KINETIS_KE
  //_DIS_ARM_PORT_INTERRUPT(A, 19);                                      // example of disabling the interrupt
  //_RE_ARM_PORT_INTERRUPT(A, 19, PORT_IRQC_RISING);                     // example of re-enabling or changing its sensitivity
    #endif
}
    #if !defined M52259DEMO && !defined _LPC23XX && !defined _LPC17XX
static void test_irq_5(void)
{
    fnInterruptMessage(OWN_TASK, IRQ5_EVENT);
}
    #endif
    #if defined _M5223X                                                  // {2}
static void test_nmi_7(void)
{
    // The M522XX irq7 has NMI characteristics and so should avoid operating system calls involving queues
    // - this technique shows a safe method of achieving the same effect
    //
    ulNMI_event_count++;                                                 // mark that new event has occurred
    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);                      // safely schedule the task to handle the event
}
    #elif !defined _LPC23XX && !defined _LPC17XX

static void test_irq_7(void)
{
    fnInterruptMessage(OWN_TASK, IRQ7_EVENT);
}
    #endif
    #if !defined _M521X
static void test_irq_11(void)
{
    fnInterruptMessage(OWN_TASK, IRQ11_EVENT);
}
    #endif

// Configure several IRQ inputs to demonstrate port change/wakeup interrupts
//
static void fnInitIRQ(void)
{
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    #if defined _KINETIS                                                 // {5}
    interrupt_setup.int_type       = PORT_INTERRUPT;                     // identifier to configure port interrupt
    interrupt_setup.int_handler    = test_irq_4;                         // handling function
        #if defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_KL43Z48M
    interrupt_setup.int_priority   = PRIORITY_PORT_C_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTC;                              // the port that the interrupt input is on
            #if defined FRDM_KL43Z || defined TWR_KL43Z48M
                #if defined WAKEUP_TEST
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
    interrupt_setup.int_handler    = test_irq_5;
                #endif
    interrupt_setup.int_port_bits  = PORTC_BIT3;                         // the IRQ input connected (SWITCH_3 on FRDM-KL43Z) - LLWU_P7
            #else
    interrupt_setup.int_port_bits  = PORTC_BIT12;                        // the IRQ input connected (SWITCH_3 on FRDM-KL46Z)
            #endif
        #elif defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined TRK_KEA64 || defined TRK_KEA128
    // Keyboard
    //
    interrupt_setup.int_type       = KEYBOARD_INTERRUPT;                 // define keyboard interrupt rather than IRQ
    interrupt_setup.int_priority   = PRIORITY_KEYBOARD_INT;              // interrupt priority level
    interrupt_setup.int_port       = KE_PORTD;                           // the port that the interrupt input is on (KE_PORTA, KE_PORTB, KE_PORTC and KE_PORTD are the same)
    interrupt_setup.int_port_bits  = (KE_PORTD_BIT5 | KE_PORTB_BIT3);    // the IRQs input connected
    // IRQ
    //
  //interrupt_setup.int_priority   = PRIORITY_PORT_IRQ_INT;              // interrupt priority level
  //interrupt_setup.int_port       = KE_PORTA;                           // the port that the interrupt input is on (when using PTA5 as IRQ SIM_SOPT_KE_DEFAULT must be configured to disable the reset function on the pin)
  //interrupt_setup.int_port_bits  = KE_PORTA_BIT5;                      // the IRQ input connected
        #elif defined FRDM_KE04Z || defined FRDM_KE06Z
    // Keyboard
    //
    interrupt_setup.int_type       = KEYBOARD_INTERRUPT;                 // define keyboard interrupt rather than IRQ
    interrupt_setup.int_priority   = PRIORITY_KEYBOARD_INT;              // interrupt priority level
  //interrupt_setup.int_port       = KE_PORTD;                           // the port that the interrupt input is on (KE_PORTA, KE_PORTB, KE_PORTC and KE_PORTD are the same)
    interrupt_setup.int_port       = KE_PORTH;                           // the port that the interrupt input is on (KE_PORTE, KE_PORTF, KE_PORTG and KE_PORTH are the same)
    interrupt_setup.int_port_bits  = (KE_PORTH_BIT6 | KE_PORTH_BIT7);    // the IRQs input connected
    // IRQ
    //
  //interrupt_setup.int_priority   = PRIORITY_PORT_IRQ_INT;              // interrupt priority level
  //interrupt_setup.int_port       = KE_PORTA;                           // the port that the interrupt input is on (when using PTA5 as IRQ SIM_SOPT_KE_DEFAULT must be configured to disable the reset function on the pin)
  //interrupt_setup.int_port_bits  = KE_PORTA_BIT5;                      // the IRQ input connected
    //
  //interrupt_setup.int_port       = KE_PORTI;                           // the port that the interrupt input is on
  //interrupt_setup.int_port_bits  = KE_PORTI_BIT6;                      // the IRQ input connected
        #elif defined TWR_K24F120M || defined TWR_K64F120M || defined FRDM_K64F || defined TWR_K21F120M || defined TWR_K22F120M || defined TEENSY_3_1 || defined TWR_K21D50M
            #if defined WAKEUP_TEST
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
            #endif
    interrupt_setup.int_priority   = PRIORITY_PORT_C_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTC;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTC_BIT6;                         // the IRQ input connected (SWITCH_2 on TWR_K24F120M/SWITCH_1 on TWR_K24F120M/SW1 on TWR_K22F120M/Pin 11 on TEENSY3.1/SW3 on TWR_K21D50M) LLWU_P10
            #if defined TWR_K21F120M || defined TWR_K22F120M || defined TWR_K21D50M
    interrupt_setup.int_handler    = test_irq_5;
            #endif
        #elif defined FRDM_K20D50M
            #if defined WAKEUP_TEST
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
    interrupt_setup.int_handler    = test_irq_5;
    interrupt_setup.int_priority   = PRIORITY_PORT_D_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTD;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTD_BIT0;                         // the IRQ input connected - LLWU_P12
            #endif
        #elif defined TWR_K20D50M
    interrupt_setup.int_priority   = PRIORITY_PORT_C_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTC;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTC_BIT2;                         // the IRQ input connected (SWITCH_3 on TWR-K20D50M)
        #elif defined TWR_K20D72M || defined TWR_KL46Z48M
            #if defined WAKEUP_TEST
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
    interrupt_setup.int_handler    = test_irq_5;
            #endif
    interrupt_setup.int_priority   = PRIORITY_PORT_C_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTC;                              // the port that the interrupt input is on
            #if defined TWR_KL46Z48M
    interrupt_setup.int_port_bits  = PORTC_BIT3;                         // (SW2 on TWR-KL46Z48M) LLWU_P7
            #else
    interrupt_setup.int_port_bits  = PORTC_BIT1;                         // (SW1 on TWR-K20D72M) LLWU_P6
            #endif
        #elif defined FRDM_KL26Z || defined FRDM_KL27Z || defined CAPUCCINO_KL27 || defined FRDM_K22F || defined K22F128_100M || defined TWR_K53N512 || defined TWR_K40D100M || defined FRDM_KL25Z
            #if defined WAKEUP_TEST
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
    interrupt_setup.int_handler    = test_irq_5;
            #endif
    interrupt_setup.int_priority   = PRIORITY_PORT_C_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTC;                              // the port that the interrupt input is on
            #if defined FRDM_K22F || defined K22F128_100M || defined FRDM_KL27Z
    interrupt_setup.int_port_bits  = PORTC_BIT1;                         // the IRQ input connected (SW2 on FRDM-K22F) LLWU_P6 (SW3 on FRDM-KL27Z)
            #elif defined TWR_K53N512 || defined TWR_K40D100M || defined FRDM_KL25Z
    interrupt_setup.int_port_bits  = PORTC_BIT5;                         // the IRQ input connected (SW1 on TWR-K53N512 and TWR-K40D100M) LLWU_P9
            #else
    interrupt_setup.int_port_bits  = PORTC_BIT3;                         // the IRQ input connected (SW1 on FRDM-KL26Z) LLWU_P7
            #endif
        #elif defined FRDM_KL03Z
            #if defined WAKEUP_TEST
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
    interrupt_setup.int_port_bits  = SWITCH_2;                           // PTB0
            #else
    interrupt_setup.int_port_bits  = PORTB_BIT7;                         // J1-6 on FRDM-KL03Z
            #endif
    interrupt_setup.int_priority   = PRIORITY_PORT_B_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTB;                              // the port that the interrupt input is on
        #else
    interrupt_setup.int_priority   = PRIORITY_PORT_A_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTA;                              // the port that the interrupt input is on
            #if defined FRDM_KL05Z || defined TEENSY_LC
                #if defined WAKEUP_TEST
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
    interrupt_setup.int_port       = PORTB;                              // the port that the interrupt input is on
        #if defined TEENSY_LC
    interrupt_setup.int_port_bits  = PORTB_BIT0;
        #else
    interrupt_setup.int_handler    = test_irq_5;
    interrupt_setup.int_port_bits  = PORTB_BIT2;
        #endif
                #else
    interrupt_setup.int_port_bits  = PORTA_BIT5;                         // the IRQ input connected (LLWU_P1)
                #endif
            #elif defined TWR_KV31F120M
    interrupt_setup.int_port_bits  = PORTA_BIT4;                         // the IRQ input connected (SWITCH_3 on TWR_KV31F120M)
            #else
    interrupt_setup.int_port_bits  = PORTA_BIT19;                        // the IRQ input connected (SWITCH_1 on TWR_K60N512)
            #endif
        #endif
        #if defined DMA_PORT_MIRRORING && defined FRDM_K64F              // {7}
    interrupt_setup.int_port       = PORTB;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTB_BIT16;                        // UART input pin on FRDM-K64F
    interrupt_setup.int_port_sense = (IRQ_BOTH_EDGES | PULLUP_ON | PORT_DMA_MODE | PORT_KEEP_PERIPHERAL); // DMA on both edges and keep peripheral function
    interrupt_setup.int_handler = 0;                                     // no interrupt handler when using DMA
    {
        // Configure the DMA trigger form the UART input pin change to toggle an alternative port so that the input ignal is mirrored to that output without CPU intervention
        //
        static const unsigned long ulOutput = PORTC_BIT16;               // the output to be mirrored to
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += 9;                                                 // user DMA channel 9
        ptrDMA_TCD->DMA_TCD_SOFF = 0;                                    // source not incremented
        ptrDMA_TCD->DMA_TCD_DOFF = 0;                                    // destination not incremented
        ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // transfer sizes always single bytes
        ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)&ulOutput;            // source is the location of a fixed pattern to be written
        ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)(&((GPIO_REGS *)GPIOC_ADD)->PTOR); // destination is the port toggle register
        ptrDMA_TCD->DMA_TCD_SLAST = ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;    // no change to address when the buffer has filled
        ptrDMA_TCD->DMA_TCD_NBYTES_ML = sizeof(unsigned long);           // each request starts a single long word transfer
        ptrDMA_TCD->DMA_TCD_CSR = 0;                                     // free-running
        POWER_UP(6, SIM_SCGC6_DMAMUX0);                                  // enable DMA multiplexer 0
        DMAMUX0_CHCFG9 = (DMAMUX0_CHCFG_SOURCE_PORTB | DMAMUX_CHCFG_ENBL); // trigger DMA channel 9 on port B changes
        DMA_ERQ |= (DMA_ERQ_ERQ9);                                       // enable request source
    }
        #else
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON);     // interrupt is to be falling edge sensitive
  //interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON | ENABLE_PORT_MODE); // set the pin to port mode - this is needed if the pin is disabled by default otherwise the pull-up/LLWU functions won't work
        #endif
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt
        #if defined DMA_PORT_MIRRORING && defined FRDM_K64F              // {7}
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON);     // interrupt is to be falling edge sensitive
        #endif
        #if (PORTS_AVAILABLE > 4) && (!defined KINETIS_KL || defined TEENSY_LC) && !defined TWR_K22F120M && !defined TWR_K20D50M && !defined TWR_K20D72M && !defined TWR_K53N512 && !defined TWR_K40D100M && !defined TWR_K21D50M && !defined TWR_K21F120M
    interrupt_setup.int_handler    = test_irq_5;                         // handling function
            #if defined TWR_K24F120M
    interrupt_setup.int_port       = PORTA;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTA_BIT13;                        // LLWU_P4
            #elif defined TWR_K64F120M || defined FRDM_K64F
    interrupt_setup.int_port       = PORTA;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTA_BIT4;                         // (SWITCH_3 on TWR_K24F120M) LLWU_P3
            #elif defined TEENSY_3_1
    interrupt_setup.int_port       = PORTD;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTD_BIT4;                         // (pin 6) LLWU_P14
            #elif defined TEENSY_LC
    interrupt_setup.int_port       = PORTC;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTC_BIT6;                         // (pin 16) LLWU_P5
            #else
    interrupt_setup.int_type       = PORT_INTERRUPT;
    interrupt_setup.int_priority   = PRIORITY_PORT_E_INT;                // interrupt priority level
    interrupt_setup.int_port       = PORTE;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTE_BIT26;                        // the IRQ input connected (SWITCH_2 on TWR_K60N512)
    interrupt_setup.int_port_sense = (IRQ_RISING_EDGE | PULLUP_ON);      // interrupt is to be rising edge sensitive
            #endif
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt
        #endif
    #elif defined _HW_SAM7X
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = test_irq_4;                            // handling function
    interrupt_setup.int_priority = PRIORITY_PIOA;                        // port interrupt priority
    interrupt_setup.int_port = PORT_A;                                   // the port used
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | IRQ_GLITCH_ENABLE); // {1} interrupt on this edge
    interrupt_setup.int_port_bits = PA29;                                // the input connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_port = PORTA_IRQ0;                               // the port used (fixed interrupt)
    interrupt_setup.int_handler = test_irq_5;                            // handling function
    interrupt_setup.int_port_sense = IRQ_FALLING_EDGE;                   // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_handler = test_irq_7;                            // handling function
    interrupt_setup.int_priority = PRIORITY_PIOB;                        // port interrupt priority
    interrupt_setup.int_port = PORT_B;                                   // the port used
    interrupt_setup.int_port_sense = (IRQ_RISING_EDGE | IRQ_GLITCH_ENABLE); // {1} interrupt on rising edges
    interrupt_setup.int_port_bits = (PB25);                              // the inputs connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_handler = test_irq_11;                           // handling function
    interrupt_setup.int_port_sense = (IRQ_RISING_EDGE | IRQ_FALLING_EDGE | IRQ_GLITCH_ENABLE); // {1} interrupt on both edges
    interrupt_setup.int_port_bits = (PB27 | PB24);                       // the inputs connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
    #elif defined _HW_AVR32
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = test_irq_4;                            // handling function
    interrupt_setup.int_priority = PRIORITY_GPIO;                        // port interrupt priority
    interrupt_setup.int_port = PORT_0;                                   // the port used
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | IRQ_ENABLE_GLITCH_FILER); // interrupt on this edge with active glitch filter
    interrupt_setup.int_port_bits = PA22;                                // the input connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_port = PORT_1;                                   // the port used
    interrupt_setup.int_handler = test_irq_5;                            // handling function
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | IRQ_ENABLE_GLITCH_FILER); // interrupt on this edge with active glitch filter
    interrupt_setup.int_port_bits = (PB22 | PB23);                       // the inputs connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_handler = test_irq_7;                            // handling function
    interrupt_setup.int_port_sense = (IRQ_RISING_EDGE);                  // interrupt on rising edges
    interrupt_setup.int_port_bits = (PB24);                              // the inputs connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_handler = test_irq_1;                            // handling function
    interrupt_setup.int_port_sense = (IRQ_RISING_EDGE);                  // interrupt on rising edges
    interrupt_setup.int_port_bits = (PB31);                              // the inputs connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_port = EXT_INT_CONTROLLER;                       // external interrupt controller
    interrupt_setup.int_handler = test_irq_11;                           // handling function
    interrupt_setup.int_port_sense = (IRQ_RISING_EDGE | IRQ_ENABLE_GLITCH_FILER); // interrupt on rising edge with active filter
    interrupt_setup.int_port_bits = (EXT_INT_0 | EXT_INT_3);             // the inputs connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
    #elif _STR91XF
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = test_irq_4;                            // handling function
    interrupt_setup.int_priority = (0);                                  // port interrupt priority
    interrupt_setup.int_port_bit = EXINT_2;                              // the input connected
    interrupt_setup.int_port_sense = WUI_RISING_EDGE;                    // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_handler = test_irq_5;                            // handling function
    interrupt_setup.int_port_bit = EXINT_15;                             // the input connected
    interrupt_setup.int_port_sense = WUI_FALLING_EDGE;                   // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_handler = test_irq_7;                            // handling function
    interrupt_setup.int_port_bit = EXINT_21;                             // the input connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_handler = test_irq_1;                            // handling function
    interrupt_setup.int_port_bit = EXINT_22;                             // the input connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    interrupt_setup.int_type = PORT_CHANNEL_INTERRUPT;
    interrupt_setup.int_handler = test_irq_11;                           // handling function
    interrupt_setup.int_port_bit = EXINT_31;                             // the input connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
    #elif defined _LM3SXXXX
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = test_irq_4;                            // handling function
    interrupt_setup.int_port = PORT_C;                                   // the port used
    interrupt_setup.int_priority = 3;                                    // port interrupt priority
    interrupt_setup.int_port_bit = 7;                                    // the input connected
    interrupt_setup.int_port_characteristic = PULLUP_ON;                 // enable pull-up resistor at input
    interrupt_setup.int_port_sense = IRQ_RISING_EDGE;                    // interrupt on this edge
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    interrupt_setup.int_port = PORT_A;                                   // the port used
    interrupt_setup.int_port_bit = 7;                                    // the input connected
    interrupt_setup.int_handler = test_irq_1;                            // handling function
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    interrupt_setup.int_port_bit = 6;                                    // the input connected
    interrupt_setup.int_handler = test_irq_5;                            // handling function
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    interrupt_setup.int_port_bit = 5;                                    // the input connected
    interrupt_setup.int_port_sense = IRQ_FALLING_EDGE;                   // interrupt on this edge
    interrupt_setup.int_handler = test_irq_7;                            // handling function
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    interrupt_setup.int_port_bit = 4;                                    // the input connected
    interrupt_setup.int_handler = test_irq_11;                           // handling function
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    #elif defined _LPC23XX || defined _LPC17XX
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = test_irq_4;                            // handling function
    interrupt_setup.int_port = PORT_0;                                   // the port used
    interrupt_setup.int_priority = 3;                                    // port interrupt priority
    interrupt_setup.int_port_bits = PORT0_BIT0;                          // the input connected
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_DOWN_OFF); // interrupt on this edge
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    interrupt_setup.int_handler = test_irq_1;                            // handling function
    interrupt_setup.int_port_bits = PORT0_BIT4;                          // the input connected
    interrupt_setup.int_port_sense = (IRQ_RISING_EDGE | PULLUP_DOWN_OFF); // interrupt on this edge
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    interrupt_setup.int_handler = test_irq_11;                           // handling function
    interrupt_setup.int_port = EXTERNALINT;                              // an external interrupt rather than a port interrupt
    interrupt_setup.int_port_bits = EINT3;                               // the input connected is EINT1 (0 to 3 is possible whereby EINT3 is shared with GPIO interrupts)
    interrupt_setup.int_priority = 12;                                   // external interrupt priority
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
    #else                                                                // M5223X
    interrupt_setup.int_type     = PORT_INTERRUPT;                       // identifier when configuring
        #if !defined TEST_DS1307 && !(defined _M5225X && !defined INTERRUPT_TASK_PHY) // uses this input for RTC or PHY
    interrupt_setup.int_handler  = test_irq_1;                           // handling function
    interrupt_setup.int_priority = (INTERRUPT_LEVEL_1);                  // interrupt priority level (this can not be modified for IRQ1..IRQ7 so the value is not really relevant)
    interrupt_setup.int_port_bit = 1;                                    // the IRQ input connected
    interrupt_setup.int_port_sense = IRQ_BOTH_EDGES;                     // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
        #endif
    interrupt_setup.int_priority = (INTERRUPT_LEVEL_4);                  // interrupt priority level (this can not be modified for IRQ1..IRQ7 so the value is not really relevant)
    interrupt_setup.int_handler  = test_irq_4;                           // handling function
    interrupt_setup.int_port_bit = 4;                                    // the IRQ input connected
    interrupt_setup.int_port_sense = IRQ_RISING_EDGE;                    // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
        #if !defined M52259DEMO                                          // this board uses the pin for PHY communication
    interrupt_setup.int_priority = (INTERRUPT_LEVEL_5);                  // interrupt priority level (this can not be modified for IRQ1..IRQ7 so the value is not really relevant)
    interrupt_setup.int_handler  = test_irq_5;                           // handling function
    interrupt_setup.int_port_bit = 5;                                    // the IRQ input connected
    interrupt_setup.int_port_sense = IRQ_RISING_EDGE;                    // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
        #endif
    interrupt_setup.int_priority = (INTERRUPT_LEVEL_7);                  // interrupt priority level (this can not be modified for IRQ1..IRQ7 so the value is not really relevant)
    interrupt_setup.int_handler  = test_nmi_7;                           // {2} handling function
    interrupt_setup.int_port_bit = 7;                                    // the NMI input connected
    interrupt_setup.int_port_sense = IRQ_FALLING_EDGE;                   // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt  (test not available on 80 pin devices)
        #ifndef _M521X
    interrupt_setup.int_priority = (IRQ11_INTERRUPT_PRIORITY);           // set level and priority
    interrupt_setup.int_handler  = test_irq_11;                          // handling function
    interrupt_setup.int_port_bit = 11;                                   // the IRQ input connected (on all devices)
    interrupt_setup.int_port_sense = IRQ_RISING_EDGE;                    // interrupt on this edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
        #endif
    #endif
    #if defined FRDM_K64F_ && defined WAKEUP_TEST                        // configure all K64 LLWU pins
    interrupt_setup.int_type       = PORT_INTERRUPT;                     // identifier to configure port interrupt
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON/* | ENABLE_PORT_MODE*/);
    interrupt_setup.int_type       = WAKEUP_INTERRUPT;                   // configure as wake-up interrupt
    interrupt_setup.int_handler    = test_irq_5;
    interrupt_setup.int_port       = PORTA;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = (PORTA_BIT4 | PORTA_BIT13);
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure wakeup pins on this port
    interrupt_setup.int_port       = PORTB;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PORTB_BIT0;
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure wakeup pins on this port
    interrupt_setup.int_port       = PORTC;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = (PORTC_BIT1 | PORTC_BIT3 | PORTC_BIT4 | PORTC_BIT5 | PORTC_BIT6 | PORTC_BIT11);
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure wakeup pins on this port
    interrupt_setup.int_port       = PORTD;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = (PORTD_BIT0 | PORTD_BIT2 | PORTD_BIT4 | PORTD_BIT6);
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure wakeup pins on this port
    interrupt_setup.int_port       = PORTE;                              // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = (PORTE_BIT1 | PORTE_BIT2 | PORTE_BIT4);
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure wakeup pins on this port
    #endif
#if (defined FRDM_K64F || defined FRDM_KL25Z || defined FRDM_KL27Z || defined CAPUCCINO_KL27 || defined FRDM_KL26Z || defined FRDM_KL43Z || defined TWR_KL43Z48M || defined FRDM_KL05Z) && defined WAKEUP_TEST
    interrupt_setup.int_port = PORT_MODULE;                              // define a wakeup interrupt on a module
    interrupt_setup.int_port_bits = (MODULE_RTC_ALARM);                  // wakeup on RTC alarm interrupts
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
    #endif
}
#endif



