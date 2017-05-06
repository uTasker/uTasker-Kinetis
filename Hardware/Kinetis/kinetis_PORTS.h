/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_PORTS.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    05.10.2015 Add port interrupt support on ports B, C, D and E for devices that share a single interrupt {1}
    11.12.2015 Add port F                                                {2}
    11.12.2015 Add DMA trigger option                                    {3}
    11.12.2015 Add keep peripheral setting option                        {4}

*/

#if defined _PORT_MUX_CODE
// This routine is used to connect one or more pins to their GPIO function with defined characteristics
//
extern void fnConnectGPIO(int iPortRef, unsigned long ulPortBits, unsigned long ulCharacteristics)
{
    #if defined KINETIS_KE
    unsigned long ulHighDrive = 0;
    unsigned long *ptrPullup = (PORT_PUEL_ADD + iPortRef);
    if (ulCharacteristics & PORT_PS_UP_ENABLE) {
        *ptrPullup |= ulPortBits;
    }
    else {
        *ptrPullup &= ~ulPortBits;
    }
    if (iPortRef == 0) {                                                 // KE ports A, B, C and D
        if (ulPortBits & KE_PORTB_BIT4) {
            ulHighDrive |= 0x01;
        }
        if (ulPortBits & KE_PORTB_BIT5) {
            ulHighDrive |= 0x02;
        }
        if (ulPortBits & KE_PORTD_BIT0) {
            ulHighDrive |= 0x04;
        }
        if (ulPortBits & KE_PORTD_BIT1) {
            ulHighDrive |= 0x08;
        }
    }
    else {                                                               // KE ports E, F, G and H
        if (ulPortBits & KE_PORTE_BIT0) {
            ulHighDrive |= 0x10;
        }
        if (ulPortBits & KE_PORTE_BIT1) {
            ulHighDrive |= 0x20;
        }
        if (ulPortBits & KE_PORTH_BIT0) {
            ulHighDrive |= 0x40;
        }
        if (ulPortBits & KE_PORTH_BIT1) {
            ulHighDrive |= 0x80;
        }
    }
    if (ulCharacteristics & PORT_DSE_HIGH) {                             // if high drive strength is requested filter the pins that support it and set the approriate control bit(s)
        PORT_HDRVE |= ulHighDrive;
    }
    else {
        PORT_HDRVE &= ulHighDrive;
    }
    #else
    register unsigned long ulMask;
    unsigned long ulBit = 0x00000001;
    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT0_BLOCK + (iPortRef * 0x1000));
        #if defined _WINDOWS
    if (iPortRef >= PORTS_AVAILABLE) {                                   // trying to configure a port that is not available
        _EXCEPTION("The port that is being accessed is not available on this processor!!");
    }
        #endif
    if ((PORT_PSEUDO_FLAG_SET_ONLY_PULLS & ulCharacteristics) != 0) {    // {115}
        ulCharacteristics &= ~(PORT_PSEUDO_FLAG_SET_ONLY_PULLS | PORT_MUX_MASK | PORT_DSE_HIGH | PORT_ODE | PORT_PFE | PORT_SRE_SLOW);
        ulMask = (PORT_PS_UP_ENABLE);                                    // modify only pull-up/down setting
    }
    else {
        ulMask = (PORT_MUX_MASK | PORT_DSE_HIGH | PORT_ODE | PORT_PFE | PORT_SRE_SLOW | PORT_PS_UP_ENABLE);
    }
    while (ulPortBits != 0) {                                            // for each specified pin
        if (ulPortBits & ulBit) {
            *ptrPCR = ((*ptrPCR & ~ulMask) | ulCharacteristics);         // {115} set the GPIO characteristics for each port pin
            ulPortBits &= ~(ulBit);                                      // pin has been set
        }
        ptrPCR++;
        ulBit <<= 1;
    }
    #endif
    _SIM_PER_CHANGE;
}
#endif

#if defined _PORT_INTERRUPT_CODE
    #if defined KINETIS_KE                                               // KE uses external interrupt
static void (*IRQ_handler)(void) = 0;                                    // handler for IRQ

static __interrupt void _IRQ_isr(void)
{
    IRQ_SC |= (IRQ_SC_IRQACK);                                           // reset the interrupt flag
        #if defined _WINDOWS
    IRQ_SC &= ~(IRQ_SC_IRQACK | IRQ_SC_IRQF);
        #endif
    if (IRQ_handler != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked when user callback operates
            IRQ_handler();                                               // call the interrupt handler
        uEnable_Interrupt();
    }
}
    #else
    #if !defined NO_PORT_INTERRUPTS_PORTA                                // if port A support has not been removed
static void (*gpio_handlers_A[PORT_WIDTH])(void) = {0};                  // a handler for each possible port A pin
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORTB && (defined irq_PORTB_ID || defined irq_PORTBCD_E_ID) // {1} if port B support has not been removed
static void (*gpio_handlers_B[PORT_WIDTH])(void) = {0};                  // a handler for each possible port B pin
    #endif
    #if (PORTS_AVAILABLE > 2) && !defined NO_PORT_INTERRUPTS_PORTC       // if port C support has not been removed
static void (*gpio_handlers_C[PORT_WIDTH])(void) = {0};                  // a handler for each possible port C pin
    #endif
    #if (PORTS_AVAILABLE > 3) && !defined NO_PORT_INTERRUPTS_PORTD       // if port D support has not been removed
static void (*gpio_handlers_D[PORT_WIDTH])(void) = {0};                  // a handler for each possible port D pin
    #endif
    #if (PORTS_AVAILABLE > 4) && !defined NO_PORT_INTERRUPTS_PORTE && (defined irq_PORTE_ID || defined irq_PORTBCD_E_ID) // {1} if port E support has not been removed
static void (*gpio_handlers_E[PORT_WIDTH])(void) = {0};                  // a handler for each possible port E pin
    #endif
    #if (PORTS_AVAILABLE > 5) && !defined NO_PORT_INTERRUPTS_PORTE && defined irq_PORTF_ID// {2} if port F support has not been removed
static void (*gpio_handlers_F[PORT_WIDTH])(void) = {0};                  // a handler for each possible port F pin
    #endif

    #if !defined NO_PORT_INTERRUPTS_PORTA                                // if port A support has not been removed
// Interrupt routine called to handle port A input interrupts
//
static __interrupt void _port_A_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = PORTA_ISFR) != 0) {                              // read which pins are generating interrupts
        PORTA_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT0_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_A[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTA_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTA_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif
    #if defined irq_PORTBCD_E_ID
// Interrupt routine called to handle port B, C, D or E shared interrupt interrupts
//
static __interrupt void _portBCD_E_isr(void)                             // {1}
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    #if !defined NO_PORT_INTERRUPTS_PORTB
    while ((ulSources = PORTB_ISFR) != 0) {                              // read which pins are generating interrupts on port B
        PORTB_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT1_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_B[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTB_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTB_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORTC
    while ((ulSources = PORTC_ISFR) != 0) {                              // read which pins are generating interrupts on port C
        PORTC_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT2_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_C[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTC_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTC_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORTD
    while ((ulSources = PORTD_ISFR) != 0) {                              // read which pins are generating interrupts on port D
        PORTD_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT3_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_D[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTD_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTD_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORTE
    while ((ulSources = PORTE_ISFR) != 0) {                              // read which pins are generating interrupts on port E
        PORTE_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT4_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_E[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTE_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTE_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
    #endif
}
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORTB && defined irq_PORTB_ID        // if port B support has not been removed
// Interrupt routine called to handle port B input interrupts
//
static __interrupt void _port_B_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = PORTB_ISFR) != 0) {                              // read which pins are generating interrupts
        PORTB_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT1_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_B[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTB_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTB_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif
    #if (PORTS_AVAILABLE > 2) && !defined NO_PORT_INTERRUPTS_PORTC  // if port C support has not been removed
        #if defined irq_PORTC_ID
// Interrupt routine called to handle port C input interrupts
//
static __interrupt void _port_C_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = PORTC_ISFR) != 0) {                              // read which pins are generating interrupts
        PORTC_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT2_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_C[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTC_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
            #if defined _WINDOWS
                    PORTC_ISFR &= ~ulPortBit;
            #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
        #elif defined irq_PORTC_D_ID
// Interrupt routine called to handle shared port C and D input interrupts
//
static __interrupt void _portC_D_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = PORTC_ISFR) != 0) {                              // read which pins are generating interrupts on port C
        PORTC_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT2_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_C[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTC_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
    #if defined _WINDOWS
                    PORTC_ISFR &= ~ulPortBit;
    #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
    #if !defined NO_PORT_INTERRUPTS_PORTD
    while ((ulSources = PORTD_ISFR) != 0) {                              // read which pins are generating interrupts on port D
        PORTD_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT3_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_D[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTD_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTD_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
    #endif
}
        #endif
    #endif
    #if (PORTS_AVAILABLE > 3) && !defined NO_PORT_INTERRUPTS_PORTD && defined irq_PORTD_ID // if port D support has not been removed
// Interrupt routine called to handle port D input interrupts
//
static __interrupt void _port_D_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = PORTD_ISFR) != 0) {                              // read which pins are generating interrupts
        PORTD_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT3_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_D[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTD_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTD_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif
    #if (PORTS_AVAILABLE > 4) && !defined NO_PORT_INTERRUPTS_PORTE && defined irq_PORTE_ID // if port E support has not been removed
// Interrupt routine called to handle port E input interrupts
//
static __interrupt void _port_E_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = PORTE_ISFR) != 0) {                              // read which pins are generating interrupts
        PORTE_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if (ulSources & ulPortBit) {                                 // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT4_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_E[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTE_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTE_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif

    #if (PORTS_AVAILABLE > 5) && !defined NO_PORT_INTERRUPTS_PORTF && defined irq_PORTF_ID // {2} if port F support has not been removed
// Interrupt routine called to handle port F input interrupts
//
static __interrupt void _port_F_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = PORTF_ISFR) != 0) {                              // read which pins are generating interrupts
        PORTF_ISFR = ulSources;                                          // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                          // pending interrupt detected on this input
                register unsigned long ulInterruptType = ((*(unsigned long *)(PORT5_BLOCK + (iInterrupt * sizeof(unsigned long)))) & PORT_IRQC_INT_MASK);
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    gpio_handlers_F[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
                    if ((ulInterruptType == PORT_IRQC_HIGH_LEVEL) || (ulInterruptType == PORT_IRQC_LOW_LEVEL)) { // if level sensitive type
                        PORTF_ISFR = ulPortBit;                          // attempt to clear the level sensitive interrupt after the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    PORTF_ISFR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif

// This routine enters the user handler for a port interrupt. The handler can be assigned to multiple inputs
//
static void fnEnterPortInterruptHandler(INTERRUPT_SETUP *port_interrupt, unsigned long ulChars)
{
    register unsigned long ulPortBits = port_interrupt->int_port_bits;
    register int ucPortRef = (int)(port_interrupt->int_port);
    unsigned long ulBit = 0x00000001;
    int port_bit = 0;
    while (ulPortBits != 0) {
        if ((ulPortBits & ulBit) != 0) {
            switch (ucPortRef) {
    #if !defined NO_PORT_INTERRUPTS_PORTA                                // if port A support has not been removed
            case PORTA:
                gpio_handlers_A[port_bit] = port_interrupt->int_handler; // {26} enter the application handler
                if (gpio_handlers_A[port_bit] != 0) {
                    fnEnterInterrupt(irq_PORTA_ID, port_interrupt->int_priority, _port_A_isr); // ensure that the handler for this port is entered
                }
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) != 0) { // {4} if the interrupt/DMA is being added to a peripheral function
                    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT0_BLOCK);
                    ptrPCR += port_bit;
                    *ptrPCR = ((*ptrPCR & ~PORT_IRQC_INT_MASK) | ulChars); // modify only the interrupt property (it is assumed that the peripheral has been configured and the port is clocked)
                }
                else {
                    _CONFIG_PORT_INPUT(A, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORTB && (defined irq_PORTB_ID || defined irq_PORTBCD_E_ID) // {1} if port B support has not been removed
            case PORTB:
                gpio_handlers_B[port_bit] = port_interrupt->int_handler; // {26} enter the application handler
                if (gpio_handlers_B[port_bit] != 0) {
        #if defined irq_PORTBCD_E_ID                                     // {1} devices whith shared B, C, D and E port interrupts
                    fnEnterInterrupt(irq_PORTBCD_E_ID, port_interrupt->int_priority, _portBCD_E_isr); // ensure that the handler for this port is entered
        #else
                    fnEnterInterrupt(irq_PORTB_ID, port_interrupt->int_priority, _port_B_isr); // ensure that the handler for this port is entered
        #endif
                }
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) != 0) { // {4} if the interrupt/DMA is being added to a peripheral function
                    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT1_BLOCK);
                    ptrPCR += port_bit;
                    *ptrPCR = ((*ptrPCR & ~PORT_IRQC_INT_MASK) | ulChars); // modify only the interrupt property (it is assumed that the peripheral has been configured and the port is clocked)
                }
                else {
                    _CONFIG_PORT_INPUT(B, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if (PORTS_AVAILABLE > 2) && !defined NO_PORT_INTERRUPTS_PORTC       // if port C support has not been removed
            case PORTC:
                gpio_handlers_C[port_bit] = port_interrupt->int_handler; // {26} enter the application handler
                if (gpio_handlers_C[port_bit] != 0) {
        #if defined irq_PORTC_ID
                    fnEnterInterrupt(irq_PORTC_ID, port_interrupt->int_priority, _port_C_isr); // ensure that the handler for this port is entered
        #elif defined irq_PORTC_D_ID                                     // devices with shared C and D port interrupts
                    fnEnterInterrupt(irq_PORTC_D_ID, port_interrupt->int_priority, _portC_D_isr); // ensure that the handler for this port is entered
        #elif defined irq_PORTBCD_E_ID                                   // {1} devices with shared B, C, D and E port interrupts
                    fnEnterInterrupt(irq_PORTBCD_E_ID, port_interrupt->int_priority, _portBCD_E_isr); // ensure that the handler for this port is entered
        #endif
                }
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) != 0) { // {4} if the interrupt/DMA is being added to a peripheral function
                    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT2_BLOCK);
                    ptrPCR += port_bit;
                    *ptrPCR = ((*ptrPCR & ~PORT_IRQC_INT_MASK) | ulChars); // modify only the interrupt property (it is assumed that the peripheral has been configured and the port is clocked)
                }
                else {
                    _CONFIG_PORT_INPUT(C, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if (PORTS_AVAILABLE > 3) && !defined NO_PORT_INTERRUPTS_PORTD       // if port D support has not been removed
            case PORTD:
                gpio_handlers_D[port_bit] = port_interrupt->int_handler; // {26} enter the application handler
                if (gpio_handlers_D[port_bit] != 0) {
        #if defined irq_PORTD_ID
                    fnEnterInterrupt(irq_PORTD_ID, port_interrupt->int_priority, _port_D_isr); // ensure that the handler for this port is entered
        #elif defined irq_PORTC_D_ID                                     // device sharing port C and port D interrupts
                    fnEnterInterrupt(irq_PORTC_D_ID, port_interrupt->int_priority, _portC_D_isr); // ensure that the handler for this port is entered
        #elif defined irq_PORTBCD_E_ID                                   // {1} devices with shared B, C, D and E port interrupts
                    fnEnterInterrupt(irq_PORTBCD_E_ID, port_interrupt->int_priority, _portBCD_E_isr); // ensure that the handler for this port is entered
        #endif
                }
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) != 0) { // {4} if the interrupt/DMA is being added to a peripheral function
                    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT3_BLOCK);
                    ptrPCR += port_bit;
                    *ptrPCR = ((*ptrPCR & ~PORT_IRQC_INT_MASK) | ulChars); // modify only the interrupt property (it is assumed that the peripheral has been configured and the port is clocked)
                }
                else {
                    _CONFIG_PORT_INPUT(D, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if (PORTS_AVAILABLE > 4) && !defined NO_PORT_INTERRUPTS_PORTE && (defined irq_PORTE_ID || defined irq_PORTBCD_E_ID) // {1} if port E support has not been removed
            case PORTE:
                gpio_handlers_E[port_bit] = port_interrupt->int_handler; // {26} enter the application handler
                if (gpio_handlers_E[port_bit] != 0) {
        #if defined irq_PORTBCD_E_ID                                     // {1} devices with shared B, C, D and E port interrupts
                    fnEnterInterrupt(irq_PORTBCD_E_ID, port_interrupt->int_priority, _portBCD_E_isr); // ensure that the handler for this port is entered
        #else
                    fnEnterInterrupt(irq_PORTE_ID, port_interrupt->int_priority, _port_E_isr); // ensure that the handler for this port is entered
        #endif
                }
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) != 0) { // {4} if the interrupt/DMA is being added to a peripheral function
                    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT4_BLOCK);
                    ptrPCR += port_bit;
                    *ptrPCR = ((*ptrPCR & ~PORT_IRQC_INT_MASK) | ulChars); // modify only the interrupt property (it is assumed that the peripheral has been configured and the port is clocked)
                }
                else {
                    _CONFIG_PORT_INPUT(E, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if (PORTS_AVAILABLE > 5)  && !defined NO_PORT_INTERRUPTS_PORTE && defined irq_PORTF_ID // {2} if port F support has not been removed
            case PORTF:
                gpio_handlers_F[port_bit] = port_interrupt->int_handler; // enter the application handler
                if (gpio_handlers_F[port_bit] != 0) {
                    fnEnterInterrupt(irq_PORTF_ID, port_interrupt->int_priority, _port_F_isr); // ensure that the handler for this port is entered
                }
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) != 0) { // {4} if the interrupt/DMA is being added to a peripheral function
                    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT5_BLOCK);
                    ptrPCR += port_bit;
                    *ptrPCR = ((*ptrPCR & ~PORT_IRQC_INT_MASK) | ulChars); // modify only the interrupt property (it is assumed that the peripheral has been configured and the port is clocked)
                }
                else {
                    _CONFIG_PORT_INPUT(F, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
            default:
                _EXCEPTION("Warning - port for interrupt not enabled!!");
                return;
            }
            ulPortBits &= ~ulBit;
        }
        ulBit <<= 1;
        port_bit++;
    }
}
    #endif
#endif




#if defined _PORT_INT_CONFIG_CODE
        {
            INTERRUPT_SETUP *port_interrupt = (INTERRUPT_SETUP *)ptrSettings;
    #if defined KINETIS_KE                                               // KE uses external interrupt
        #if ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06)
            unsigned char ucPortPin = SIM_PINSEL_IRQPS_PTA5;             // default IRQ input
        #endif
            POWER_UP(0, SIM_SCGC_IRQ);                                   // enable clocks to the external interrupt module
        #if ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06)
            if (port_interrupt->int_port == KE_PORTA) {
              //SIM_SOPT0 &= ~(SIM_SOPT_RSTPE);                          // remove reset function so that IRQ function can be selected (PTA5) - this is a "write-once" field so will not actually work as such; the value must be configured in SIM_SOPT_KE_DEFAULT if this input is to be used
            }
            else {                                                       // assume PTI
                switch (port_interrupt->int_port_bits) {
                case KE_PORTI_BIT0:
                    ucPortPin = SIM_PINSEL_IRQPS_PTI0;
                    break;
                case KE_PORTI_BIT1:
                    ucPortPin = SIM_PINSEL_IRQPS_PTI1;
                    break;
                case KE_PORTI_BIT2:
                    ucPortPin = SIM_PINSEL_IRQPS_PTI2;
                    break;
                case KE_PORTI_BIT3:
                    ucPortPin = SIM_PINSEL_IRQPS_PTI3;
                    break;
                case KE_PORTI_BIT4:
                    ucPortPin = SIM_PINSEL_IRQPS_PTI4;
                    break;
                case KE_PORTI_BIT5:
                    ucPortPin = SIM_PINSEL_IRQPS_PTI5;
                    break;
                case KE_PORTI_BIT6:
                    ucPortPin = SIM_PINSEL_IRQPS_PTI6;
                    break;
                }
            }
            SIM_PINSEL0 = ((SIM_PINSEL0 & ~(SIM_PINSEL_IRQPS_PTI6)) | ucPortPin); // select the IRQ pin to use
        #else                                                            // KE02 has only one pin choice
          //SIM_SOPT0 &= ~(SIM_SOPT_RSTPE);                              // remove reset function so that IRQ function can be selected (PTA5) - this is a "write-once" field so will not actually work as such; the value must be configured in SIM_SOPT_KE_DEFAULT if this input is to be used
        #endif
            if ((port_interrupt->int_port_sense & PULLUP_ON) != 0) {
                IRQ_SC = IRQ_SC_IRQPE;                                   // enable IRQ pin function with pull-up
            }
            else {
                IRQ_SC = (IRQ_SC_IRQPE | IRQ_SC_IRQPDD);                 // enable IRQ pin function without pull-up
            }
            if ((port_interrupt->int_port_sense & (IRQ_RISING_EDGE | IRQ_HIGH_LEVEL)) != 0) { // rising edge or high sensitivity
                IRQ_SC |= (IRQ_SC_IRQEDG);                               // select rising edge or high sensitive
            }
            if ((port_interrupt->int_port_sense & (IRQ_HIGH_LEVEL | IRQ_LOW_LEVEL)) != 0) { // if level sensitive required
                IRQ_SC |= (IRQ_SC_IRQMOD);                               // select level sensitive
            }
            IRQ_handler = port_interrupt->int_handler;                   // enter the application handler
            fnEnterInterrupt(irq_IRQ_ID, port_interrupt->int_priority, _IRQ_isr); // ensure that the handler is entered
            IRQ_SC |= (IRQ_SC_IRQIE | IRQ_SC_IRQACK);                    // enable interrupt and reset pending interrupt
        #if defined _WINDOWS
            IRQ_SC &= ~(IRQ_SC_IRQACK | IRQ_SC_IRQF);
            _SIM_PER_CHANGE;
        #endif
    #else                                                                // else use port control interrupt module
            unsigned long ulCharacteristics = 0;                         // default is to disable interrupts
            if ((port_interrupt->int_port_sense & IRQ_LOW_LEVEL) != 0) {
                ulCharacteristics = PORT_IRQC_LOW_LEVEL;
            }
            else if ((port_interrupt->int_port_sense & IRQ_HIGH_LEVEL) != 0) {
                ulCharacteristics = PORT_IRQC_HIGH_LEVEL;
            }
            else if ((port_interrupt->int_port_sense & IRQ_RISING_EDGE) != 0) {
                if ((port_interrupt->int_port_sense & IRQ_FALLING_EDGE) != 0) {
                    ulCharacteristics = PORT_IRQC_BOTH;
                }
                else {
                    ulCharacteristics = PORT_IRQC_RISING;
                }
            }
            else if ((port_interrupt->int_port_sense & IRQ_FALLING_EDGE) != 0) {
                ulCharacteristics = PORT_IRQC_FALLING;
            }
            if ((port_interrupt->int_port_sense & PULLUP_ON) != 0) {
                ulCharacteristics |= (PORT_PS_UP_ENABLE);
            }
            else if ((port_interrupt->int_port_sense & PULLDOWN_ON) != 0) {
                ulCharacteristics |= (PORT_PS_DOWN_ENABLE);
            }
            if ((port_interrupt->int_port_sense & PORT_DMA_MODE) != 0) { // {3} if DMA is required rather than interrupt
                ulCharacteristics &= ~(PORT_IRQC_LOW_LEVEL);             // DMA transfer rather that an interrupt
            }
            fnEnterPortInterruptHandler(port_interrupt, ulCharacteristics); // configure the interrupt and port bits according to the interrupt requirements
    #endif
        }
#endif

