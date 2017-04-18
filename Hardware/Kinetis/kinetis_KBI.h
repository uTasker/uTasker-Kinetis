/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_KBI.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/

#if defined _KBI_INTERRUPT_CODE

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

    #define _KBI_0               0x40
    #define _KBI_1               0x80
    #define _KBI_CONTROLLER_MASK 0xc0
    #define _KBI_PIN_REF_MASK    0x3f

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA128
static const unsigned char _KBI[PORTS_AVAILABLE][32] = {
    {(_KBI_0 | 0),  (_KBI_0 | 1),  (_KBI_0 | 2),  (_KBI_0 | 3),  (_KBI_0 | 4),  (_KBI_0 | 5),  (_KBI_0 | 6),  (_KBI_0 | 7), // PTA0..PTA7
     (_KBI_0 | 8),  (_KBI_0 | 9),  (_KBI_0 | 10), (_KBI_0 | 11), (_KBI_0 | 12), (_KBI_0 | 29), (_KBI_0 | 30), (_KBI_0 | 15), // PTB0..PTB7
     (_KBI_0 | 16), (_KBI_0 | 17), (_KBI_0 | 18), (_KBI_0 | 19), (_KBI_0 | 20), (_KBI_0 | 21), (_KBI_0 | 22), (_KBI_0 | 23), // PTC0..PTC7
     (_KBI_1 | 24), (_KBI_1 | 25), (_KBI_0 | 26), (_KBI_0 | 27), (_KBI_0 | 28), (_KBI_0 | 29), (_KBI_0 | 30), (_KBI_0 | 31)}, // PTD0..PTD7
    #if PORTS_AVAILABLE > 1
    {(_KBI_1 | 0),  (_KBI_1 | 1),  (_KBI_1 | 2),  (_KBI_1 | 3),  (_KBI_1 | 4),  (_KBI_1 | 5),  (_KBI_1 | 6),  (_KBI_1 | 7), // PTE0..PTE7
     (_KBI_1 | 8),  (_KBI_1 | 9),  (_KBI_1 | 10), (_KBI_1 | 11), (_KBI_1 | 12), (_KBI_1 | 13), (_KBI_1 | 14), (_KBI_1 | 15), // PTF0..PTF7
     (_KBI_1 | 16), (_KBI_1 | 17), (_KBI_1 | 18), (_KBI_1 | 19), (_KBI_1 | 20), (_KBI_1 | 21), (_KBI_1 | 22), (_KBI_1 | 23), // PTG0..PTG7
     (_KBI_1 | 24), (_KBI_1 | 25), (_KBI_1 | 26), (_KBI_1 | 27), (_KBI_1 | 28), (_KBI_1 | 29), (_KBI_1 | 30), (_KBI_1 | 31)}, // PTH0..PTH7
    #endif
};

static void (*KBI_handlers[KBIS_AVAILABLE][KBI_WIDTH])(void) = {{0}};    // a handler for each possible KBI pin
    #elif defined KINETIS_KEA8
static const unsigned char _KBI[PORTS_AVAILABLE][32] = {
    {(_KBI_0 | 0),  (_KBI_0 | 1),  (_KBI_0 | 2),  (_KBI_0 | 3),  0,             0,             0,             0, // PTA0..PTA7
     (_KBI_0 | 4),  (_KBI_0 | 5),  (_KBI_0 | 6),  (_KBI_0 | 7),  (_KBI_1 | 6),  (_KBI_1 | 7),  0,             0, // PTB0..PTB7
     (_KBI_1 | 2),  (_KBI_1 | 3),  (_KBI_1 | 4),  (_KBI_1 | 5),  (_KBI_1 | 0),  (_KBI_1 | 1),  0,             0,} // PTC0..PTC7
};

static void (*KBI_handlers[KBIS_AVAILABLE])(void) = {0};                 // a single interrupt for each keyboard controller
    #else
static const unsigned char _KBI[PORTS_AVAILABLE][32] = {
    {(_KBI_0 | 0), (_KBI_0 | 1), (_KBI_0 | 2), (_KBI_0 | 3), 0, 0, 0, 0, // PTA0..PTA7
     (_KBI_0 | 4), (_KBI_0 | 5), (_KBI_0 | 6), (_KBI_0 | 7), 0, 0, 0, 0, // PTB0..PTB7
     0,0,0,0,0,0,0,0,                                                    // PTC0..PTC7
     (_KBI_1 | 0), (_KBI_1 | 1), (_KBI_1 | 2), (_KBI_1 | 3), (_KBI_1 | 4), (_KBI_1 | 5), (_KBI_1 | 6), (_KBI_1 | 7)}, // PTD0..PTD7
    {0,0,0,0,0,0,0,0,                                                    // PTE0..PTE7
     0,0,0,0,0,0,0,0,                                                    // PTF0..PTF7
     0,0,0,0,0,0,0,0,                                                    // PTG0..PTG7
     0,0,0,0,0,0,0,0},                                                   // PTH0..PTH7
};

static void (*KBI_handlers[KBIS_AVAILABLE])(void) = {0};                 // a single interrupt for each keyboard controller
    #endif


/* =================================================================== */
/*                       KBI Interrupt Handlers                        */
/* =================================================================== */

static void _KBI0_isr(void)                                              // KE keyboard interrupt 0
{
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA128
    int iPin = 0;
    unsigned long ulPin = 0x00000001;
    unsigned long ulFlags = KBI0_SP;                                     // read enabled interrupt flags
    KBI0_SC |= (KBI_SC_RSTKBSP | KBI_SC_KBACK);                          // clear flags and pending interrupt (note that subsequent edge sensitive interrupts are only accepted by the KBI when all other inputs have retunred to their original state)
    #if defined _WINDOWS
    KBI0_SC &= ~(KBI_SC_RSTKBSP | KBI_SC_KBACK);
    KBI0_SP = 0;
    #endif
    while (ulFlags != 0) {                                               // for each keyboard interrupt that has signalled a trigger
        if (ulPin & ulFlags) {
            ulFlags &= ~ulPin;
            if (KBI_handlers[0][iPin] != 0) {
                uDisable_Interrupt();
                    KBI_handlers[0][iPin]();                             // call user handler
                uEnable_Interrupt();
            }
        }
        iPin++;
        ulPin <<= 1;
    }
    #else
    KBI0_SC |= KBI_SC_KBACK;                                            // clear pending interrupt
        #if defined _WINDOWS
    KBI0_SC &= ~(KBI_SC_KBACK | KBI_SC_KBF);
        #endif
    if (KBI_handlers[0] != 0) {
        uDisable_Interrupt();
            KBI_handlers[0]();                                          // call user handler
        uEnable_Interrupt();
    }
    #endif
}
    #if KBIS_AVAILABLE > 1
static void _KBI1_isr(void)                                              // KE keyboard interrupt 1
{
        #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA128
    int iPin = 0;
    unsigned long ulPin = 0x00000001;
    unsigned long ulFlags = KBI1_SP;                                     // read enabled interrupt flags
    KBI1_SC |= (KBI_SC_RSTKBSP | KBI_SC_KBACK);                          // clear flags and pending interrupt (note that subsequent edge sensitive interrupts are only accepted by the KBI when all other inputs have retunred to their original state)
    #if defined _WINDOWS
    KBI1_SC &= ~(KBI_SC_RSTKBSP | KBI_SC_KBACK);
    KBI1_SP = 0;
    #endif
    while (ulFlags != 0) {                                               // for each keyboard interrupt that has signalled a trigger
        if (ulPin & ulFlags) {
            ulFlags &= ~ulPin;
            if (KBI_handlers[1][iPin] != 0) {
                uDisable_Interrupt();
                    KBI_handlers[1][iPin]();                             // call user handler
                uEnable_Interrupt();
            }
        }
        iPin++;
        ulPin <<= 1;
    }
        #else
    KBI1_SC |= KBI_SC_KBACK;                                             // clear pending interrupt
        #if defined _WINDOWS
    KBI1_SC &= ~(KBI_SC_KBACK | KBI_SC_KBF);
        #endif
    if (KBI_handlers[1] != 0) {
        uDisable_Interrupt();
            KBI_handlers[1]();                                          // call user handler
        uEnable_Interrupt();
    }
        #endif
}
    #endif
#endif

/* =================================================================== */
/*                         KBI Configuration                           */
/* =================================================================== */

#if defined _KBI_CONFIG_CODE
        {
            INTERRUPT_SETUP *port_interrupt = (INTERRUPT_SETUP *)ptrSettings;
            _KINETIS_KBI *ptrKBI;
            unsigned long *ptrPullUps = (PORT_PUEL_ADD + port_interrupt->int_port);
    #if KBI_WIDTH == 32
            unsigned long KBI_enables[KBIS_AVAILABLE] = {0};
    #else
            unsigned char KBI_enables[KBIS_AVAILABLE] = {0};
    #endif
            unsigned long ulPins = port_interrupt->int_port_bits;
            unsigned long ulBit = 0x00000001;
            int iBitNumber = 0;
            while (ulPins != 0) {                                        // allow multiple inputs to be configured and assigned to a single interrupt
                if ((ulBit & ulPins) != 0) {
                    ulPins &= ~(ulBit);
                    switch (_KBI[port_interrupt->int_port][iBitNumber] & _KBI_CONTROLLER_MASK) { // get the KBI controller that the pin is assigned to
                    case _KBI_0:
                        POWER_UP(0, SIM_SCGC_KBI0);                      // ensure the module is powered
                        KBI_enables[0] |= (1 << (_KBI[port_interrupt->int_port][iBitNumber] & _KBI_PIN_REF_MASK));
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA128
                        KBI_handlers[0][iBitNumber] = port_interrupt->int_handler; // enter the user handler for the individual pin
    #else
                        KBI_handlers[0] = port_interrupt->int_handler;   // enter the user handler for the keyboard interrupt controller
    #endif
                        fnEnterInterrupt(irq_KBI0_ID, port_interrupt->int_priority, _KBI0_isr); // ensure that the handler for this KBI is entered
                        break;
    #if KBIS_AVAILABLE > 1
                    case _KBI_1:
                        POWER_UP(0, SIM_SCGC_KBI1);                      // ensure the module is powered
                        KBI_enables[1] |= (1 << (_KBI[port_interrupt->int_port][iBitNumber] & _KBI_PIN_REF_MASK));
        #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA128
                        KBI_handlers[1][iBitNumber] = port_interrupt->int_handler; // enter the user handler for the individual pin
        #else
                        KBI_handlers[1] = port_interrupt->int_handler;   // enter the user handler for the keyboard interrupt controller
        #endif
                        fnEnterInterrupt(irq_KBI1_ID, port_interrupt->int_priority, _KBI1_isr); // ensure that the handler for this KBI is entered
                        break;
    #endif
                    default:
                        _EXCEPTION("Invalid KBI input");
                        break;
                    }
                }
                iBitNumber++;
                ulBit <<= 1;
            }
            if (port_interrupt->int_port_sense & PULLUP_ON) {            // pull up required on the input
                *ptrPullUps |= port_interrupt->int_port_bits;            // enabled pull-ups on the specified KBI inputs
            }
            else {
                *ptrPullUps &= ~port_interrupt->int_port_bits;           // disabled pull-ups on the specified KBI inputs
            }
            iBitNumber = 0;
            do {
                if (KBI_enables[iBitNumber] != 0) {
                    switch (iBitNumber) {
                    case 0:
                        ptrKBI = (_KINETIS_KBI *)KBI0_BLOCK;
                        break;
    #if KBIS_AVAILABLE > 1
                    case 1:
                        ptrKBI = (_KINETIS_KBI *)KBI1_BLOCK;
                        break;
    #endif
                    }
                    ptrKBI->KBI_SC &= ~KBI_SC_KBIE;                      // mask main KBI interrupt
                    if (port_interrupt->int_port_sense & (IRQ_LOW_LEVEL | IRQ_HIGH_LEVEL)) { // level sensitive input
                        ptrKBI->KBI_SC |= KBI_SC_KBMOD;                  // detect levels as well as edges
                    }
                    else {
                        ptrKBI->KBI_SC &= ~KBI_SC_KBMOD;                 // detect only edges
                    }
                    if (port_interrupt->int_port_sense & IRQ_RISING_EDGE) {
                        ptrKBI->KBI_ES |= KBI_enables[iBitNumber];       // select rising edge interrupt
                    }
                    else {
                        ptrKBI->KBI_ES &= ~KBI_enables[iBitNumber];      // select falling edge interrupt
                    }
                    ptrKBI->KBI_PE |= KBI_enables[iBitNumber];           // enable the pin as KBI interrupt
    #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA128
                    ptrKBI->KBI_SC |= (KBI_SC_RSTKBSP | KBI_SC_KBACK | KBI_SC_KBSPEN); // clear any false interrupts and clear flagged interrupts - real KBI_SP register enable
    #else
                    ptrKBI->KBI_SC |= KBI_SC_KBACK;                      // clear any false interrupts
    #endif
                    ptrKBI->KBI_SC |= KBI_SC_KBIE;                       // enable main KBI interrupt
                }
            } while (++iBitNumber < KBIS_AVAILABLE);
        }
    #if defined _WINDOWS
        _SIM_PER_CHANGE;
    #endif
#endif