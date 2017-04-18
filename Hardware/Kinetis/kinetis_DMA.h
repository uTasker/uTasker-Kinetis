/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_DMA.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    17.11.2013 Add uReverseMemcpy() using DMA                            {59}
    06.05.2014 Add KL DMA based uMemcpy() and uMemset()                  {80}
    17.06.2014 Optimised DMA based uMemset()/uMemcpy()/uReverseMemcpy() to utilise widest transfer sizes possible {87}
    02.09.2014 Move remaining byte copy in DMA based uMemcpy() to after DMA termination {103}
    04.01.2016 Added automatic DMA half- and full-buffer handling for KL devices {1}
    05.02.2016 Protect KL memcpy DMA before clearing the DONE flag       {2}
    25.04.2016 Correct buffer wrap direction for K parts                 {3}
    02.03.2017 Set the DMA_TCD_CITER_ELINK value earlier to protect initial part of code from interrupts {4}
    02.03.2017 Add optional alternative DMA channel for use by interrupts when the main one is in use {5}

*/

#if defined _DMA_SHARED_CODE

#define DMA_TRANSFER_INPUT         0x00
#define DMA_TRANSFER_OUTPUT        0x01
#define DMA_TRANSFER_HALF_BUFFER   0x02
#define DMA_TRANSFER_FIRST_BUFFER  0x00
#define DMA_TRANSFER_SECOND_BUFFER 0x04


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _DMA_Interrupt_0(void);
static __interrupt void _DMA_Interrupt_1(void);
static __interrupt void _DMA_Interrupt_2(void);
static __interrupt void _DMA_Interrupt_3(void);
#if DMA_CHANNEL_COUNT > 4
    static __interrupt void _DMA_Interrupt_4(void);
#endif
#if DMA_CHANNEL_COUNT > 5
    static __interrupt void _DMA_Interrupt_5(void);
#endif
#if DMA_CHANNEL_COUNT > 6
    static __interrupt void _DMA_Interrupt_6(void);
#endif
#if DMA_CHANNEL_COUNT > 7
    static __interrupt void _DMA_Interrupt_7(void);
#endif
#if DMA_CHANNEL_COUNT > 8
    static __interrupt void _DMA_Interrupt_8(void);
#endif
#if DMA_CHANNEL_COUNT > 9
    static __interrupt void _DMA_Interrupt_9(void);
#endif
#if DMA_CHANNEL_COUNT > 10
    static __interrupt void _DMA_Interrupt_10(void);
#endif
#if DMA_CHANNEL_COUNT > 11
    static __interrupt void _DMA_Interrupt_11(void);
#endif
#if DMA_CHANNEL_COUNT > 12
    static __interrupt void _DMA_Interrupt_12(void);
#endif
#if DMA_CHANNEL_COUNT > 13
    static __interrupt void _DMA_Interrupt_13(void);
#endif
#if DMA_CHANNEL_COUNT > 14
    static __interrupt void _DMA_Interrupt_14(void);
#endif
#if DMA_CHANNEL_COUNT > 15
    static __interrupt void _DMA_Interrupt_15(void);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const unsigned char *_DMA_Interrupt[DMA_CHANNEL_COUNT] = {
    (unsigned char *)_DMA_Interrupt_0,
    (unsigned char *)_DMA_Interrupt_1,
    (unsigned char *)_DMA_Interrupt_2,
    (unsigned char *)_DMA_Interrupt_3,
#if DMA_CHANNEL_COUNT > 4
    (unsigned char *)_DMA_Interrupt_4,
#endif
#if DMA_CHANNEL_COUNT > 5
    (unsigned char *)_DMA_Interrupt_5,
#endif
#if DMA_CHANNEL_COUNT > 6
    (unsigned char *)_DMA_Interrupt_6,
#endif
#if DMA_CHANNEL_COUNT > 7
    (unsigned char *)_DMA_Interrupt_7,
#endif
#if DMA_CHANNEL_COUNT > 8
    (unsigned char *)_DMA_Interrupt_8,
#endif
#if DMA_CHANNEL_COUNT > 9
    (unsigned char *)_DMA_Interrupt_9,
#endif
#if DMA_CHANNEL_COUNT > 10
    (unsigned char *)_DMA_Interrupt_10,
#endif
#if DMA_CHANNEL_COUNT > 11
    (unsigned char *)_DMA_Interrupt_11,
#endif
#if DMA_CHANNEL_COUNT > 12
    (unsigned char *)_DMA_Interrupt_12,
#endif
#if DMA_CHANNEL_COUNT > 13
    (unsigned char *)_DMA_Interrupt_13,
#endif
#if DMA_CHANNEL_COUNT > 14
    (unsigned char *)_DMA_Interrupt_14,
#endif
#if DMA_CHANNEL_COUNT > 15
    (unsigned char *)_DMA_Interrupt_15,
#endif
};

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

// Interrupt handlers for DMA operation
// - either a complete or a half (not KL) buffer has been completed
//
static void (*_DMA_handler[DMA_CHANNEL_COUNT])(void) = {0};              // user DMA interrupt handlers
#if defined KINETIS_KL
    static unsigned long ulRepeatLength[DMA_CHANNEL_COUNT] = {0};        // {1}
    static unsigned char ucDirectionOutput[DMA_CHANNEL_COUNT] = {DMA_TRANSFER_INPUT};
    static unsigned char *ptrStart[DMA_CHANNEL_COUNT] = {0};
#endif


/* =================================================================== */
/*                       DMA Interrupt Handlers                        */
/* =================================================================== */

static void _DMA_Handler(int iChannel)
{
    #if defined KINETIS_KL
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += iChannel;                                                  // move to the use DMA channel
        #if defined _WINDOWS
    if ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_CE) != 0) {
        // Configuration error!!!!!!!!
        //
        ptrDMA->DMA_DCR &= ~(DMA_DCR_ERQ);                               // stop further transfers
        ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;
    }
        #endif

    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear DMA interrupt
        #if defined _WINDOWS
    ptrDMA->DMA_DSR_BCR = 0;
        #endif
    if (ulRepeatLength[iChannel] != 0) {                                 // {1} if in automatic buffer repeat mode
        int iDontResetPointer = 0;
        if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_HALF_BUFFER) != 0) { // if emulating half-buffer interrupt
            if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_SECOND_BUFFER) == 0) { // first half of the buffer has completed
                iDontResetPointer = 1;                                   // the second half of the buffer is now in progress so the source/desination pointer is not changed
            }
            ucDirectionOutput[iChannel] ^= (DMA_TRANSFER_SECOND_BUFFER); // toggle buffer
            ptrDMA->DMA_DSR_BCR = (ulRepeatLength[iChannel]/2);          // the half-buffer length
        }
        else {                                                           // full-buffer operation
            ptrDMA->DMA_DSR_BCR = ulRepeatLength[iChannel];              // the buffer length
        }
        if (iDontResetPointer == 0) {
            if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_OUTPUT) != 0) {
                ptrDMA->DMA_SAR = (unsigned long)ptrStart[iChannel];     // set the source pointer back to the start of the buffer
            }
            else {
                ptrDMA->DMA_DAR = (unsigned long)ptrStart[iChannel];     // set the destination pointer back to the start of the buffer
            }
        }
        ptrDMA->DMA_DCR |= (DMA_DCR_ERQ);                                // restart DMA
    }
    #else
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << iChannel);                              // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << iChannel);                                // clear the interrupt request
        #endif
    #endif
    if (_DMA_handler[iChannel] != 0) {                                   // if there is a user handler
        uDisable_Interrupt();
            _DMA_handler[iChannel]();                                    // call user handling function
        uEnable_Interrupt();
    }
}

static __interrupt void _DMA_Interrupt_0(void)
{
    _DMA_Handler(0);
}

static __interrupt void _DMA_Interrupt_1(void)
{
    _DMA_Handler(1);
}

static __interrupt void _DMA_Interrupt_2(void)
{
    _DMA_Handler(2);
}

static __interrupt void _DMA_Interrupt_3(void)
{
    _DMA_Handler(3);
}

#if DMA_CHANNEL_COUNT > 4
static __interrupt void _DMA_Interrupt_4(void)
{
    _DMA_Handler(4);
}
#endif

#if DMA_CHANNEL_COUNT > 5
static __interrupt void _DMA_Interrupt_5(void)
{
    _DMA_Handler(5);
}
#endif

#if DMA_CHANNEL_COUNT > 6
static __interrupt void _DMA_Interrupt_6(void)
{
    _DMA_Handler(6);
}
#endif

#if DMA_CHANNEL_COUNT > 7
static __interrupt void _DMA_Interrupt_7(void)
{
    _DMA_Handler(7);
}
#endif

#if DMA_CHANNEL_COUNT > 8
static __interrupt void _DMA_Interrupt_8(void)
{
    _DMA_Handler(8);
}
#endif

#if DMA_CHANNEL_COUNT > 9
static __interrupt void _DMA_Interrupt_9(void)
{
    _DMA_Handler(9);
}
#endif

#if DMA_CHANNEL_COUNT > 10
static __interrupt void _DMA_Interrupt_10(void)
{
    _DMA_Handler(10);
}
#endif

#if DMA_CHANNEL_COUNT > 11
static __interrupt void _DMA_Interrupt_11(void)
{
    _DMA_Handler(11);
}
#endif

#if DMA_CHANNEL_COUNT > 12
static __interrupt void _DMA_Interrupt_12(void)
{
    _DMA_Handler(12);
}
#endif

#if DMA_CHANNEL_COUNT > 13
static __interrupt void _DMA_Interrupt_13(void)
{
    _DMA_Handler(13);
}
#endif

#if DMA_CHANNEL_COUNT > 14
static __interrupt void _DMA_Interrupt_14(void)
{
    _DMA_Handler(14);
}
#endif

#if DMA_CHANNEL_COUNT > 15
static __interrupt void _DMA_Interrupt_15(void)
{
    _DMA_Handler(15);
}
#endif

extern void fnDMA_BufferReset(int iChannel, int iAction)
{
    #if defined KINETIS_KL
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += iChannel;
    if (iAction == DMA_BUFFER_START) {
        ptrDMA->DMA_DCR |= (DMA_DCR_ERQ);                                // just enable
        return;
    }
    ptrDMA->DMA_DCR &= ~(DMA_DCR_ERQ);                                   // disable operation
    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear the DONE flag and clear errors etc.
    if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_HALF_BUFFER) != 0) { // if emulating half-buffer interrupt
        if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_SECOND_BUFFER) == 0) { // first half of the buffer was in operation
            ucDirectionOutput[iChannel] &= ~(DMA_TRANSFER_SECOND_BUFFER);// start at first buffer half
        }
        ptrDMA->DMA_DSR_BCR = (ulRepeatLength[iChannel]/2);              // the half-buffer length
    }
    else {                                                               // full-buffer operation
        ptrDMA->DMA_DSR_BCR = ulRepeatLength[iChannel];                  // the buffer length
    }
    if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_OUTPUT) != 0) {
        ptrDMA->DMA_SAR = (unsigned long)ptrStart[iChannel];             // set the source pointer back to the start of the buffer
    }
    else {
        ptrDMA->DMA_DAR = (unsigned long)ptrStart[iChannel];             // set the destination pointer back to the start of the buffer
    }
    if (iAction != DMA_BUFFER_RESET) {                                   // if not a buffer reset without continued operation
        ptrDMA->DMA_DCR |= (DMA_DCR_ERQ);                                // restart DMA from the start of the buffer
    }
    #else
    switch (iAction) {
    case DMA_BUFFER_START:
        DMA_ERQ |= (DMA_ERQ_ERQ0 << iChannel);                           // just enable
        break;
    case DMA_BUFFER_RESET:                                               // reset the DMA back to the start of the present buffer
    case DMA_BUFFER_RESTART:                                             // reset and start again
        {
            int iSize = 1;                                               // default is single byte size
            unsigned long ulBufferLength;
            KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
            DMA_ERQ &= ~(DMA_ERQ_ERQ0 << iChannel);                      // disable DMA operation
            ptrDMA_TCD += iChannel;
            if (ptrDMA_TCD->DMA_TCD_DLASTSGA == 0) {                     // input buffer needs to be reset
                if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_SSIZE_16) != 0) {
                    iSize = 2;
                }
                else if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_SSIZE_32) != 0) {
                    iSize = 4;
                }
                ptrDMA_TCD->DMA_TCD_SADDR += (ptrDMA_TCD->DMA_TCD_CITER_ELINK * iSize); // project to the end of the transfer that is remaining
                ulBufferLength = -(signed long)(ptrDMA_TCD->DMA_TCD_SLAST);
                ptrDMA_TCD->DMA_TCD_SADDR -= ulBufferLength;             // set back to start of the input buffer
            }
            else {                                                       // output buffer needs to be reset
                if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_DSIZE_16) != 0) {
                    iSize = 2;
                }
                else if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_DSIZE_32) != 0) {
                    iSize = 4;
                }
                ptrDMA_TCD->DMA_TCD_DADDR += (ptrDMA_TCD->DMA_TCD_CITER_ELINK * iSize); // project to the end of the transfer that is remaining
                ulBufferLength = -(signed long)(ptrDMA_TCD->DMA_TCD_DLASTSGA);
                ptrDMA_TCD->DMA_TCD_DADDR -= ulBufferLength;             // set back to start of the output buffer
            }
            ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = (signed short)(ulBufferLength/ iSize); // set the cycle length
            if (iAction != DMA_BUFFER_RESET) {                           // if not a buffer reset without continued operation
                DMA_ERQ |= (DMA_ERQ_ERQ0 << iChannel);                   // restart DMA from the start of the buffer
            }
        }
        break;
    }
    #endif
}

/* =================================================================== */
/*                         DMA Configuration                           */
/* =================================================================== */


// Buffer source to fixed destination address or fixed source address to buffer
//
extern void fnConfigDMA_buffer(unsigned char ucDMA_channel, unsigned char ucDmaTriggerSource, unsigned long ulBufLength, void *ptrBufSource, void *ptrBufDest, unsigned long ulRules, void (*int_handler)(void), int int_priority)
{
    unsigned char ucSize = (unsigned char)(ulRules & 0x07);              // transfer size 1, 2 or 4 bytes

    #if defined KINETIS_KL
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
        #if defined _WINDOWS
    if (ucDMA_channel >= DMA_CHANNEL_COUNT) {
        _EXCEPTION("Error - peripheral DMA is specifying a non-existent channel!!");
    }
        #endif
    ptrDMA += ucDMA_channel;                                             // move to the DMA channel to be used
    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear the DONE flag and clear errors etc.
    switch (ucSize) {
    default:
    case 1:                                                              // byte transfers
        ucSize = 1;
        ptrDMA->DMA_DCR = (DMA_DCR_DSIZE_8 | DMA_DCR_SSIZE_8 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // transfer size bytes
        break;
    case 2:                                                              // half-word transfers
        ptrDMA->DMA_DCR = (DMA_DCR_DSIZE_16 | DMA_DCR_SSIZE_16 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // transfer size half-words
        break;
    case 4:
        ptrDMA->DMA_DCR = (DMA_DCR_DSIZE_32 | DMA_DCR_SSIZE_32 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // transfer sizes long words
        break;
    }
    if ((ulRules & DMA_DIRECTION_OUTPUT) != 0) {                         // buffer to fixed output
        ptrDMA->DMA_DCR |= (DMA_DCR_SINC);                               // transfers with increment only on source
        ucDirectionOutput[ucDMA_channel] = DMA_TRANSFER_OUTPUT;          // remember the output direction
        ptrStart[ucDMA_channel] = ptrBufSource;                          // remember the start of the source buffer
    }
    else {                                                               // fixed input to buffer
        ptrDMA->DMA_DCR |= (DMA_DCR_DINC);                               // transfers with increment only on destination
        ucDirectionOutput[ucDMA_channel] = DMA_TRANSFER_INPUT;           // remember the input direction
        ptrStart[ucDMA_channel] = ptrBufDest;                            // remember the start of the destination buffer
    }
    if ((DMA_FIXED_ADDRESSES & ulRules) != 0) {                          // if both source and destination addresses are fixed
        ptrDMA->DMA_DCR &= ~(DMA_DCR_DINC | DMA_DCR_SINC);               // disable source and destination increments
    }
    ptrDMA->DMA_SAR = (unsigned long)ptrBufSource;                       // set source buffer
    ptrDMA->DMA_DAR = (unsigned long)ptrBufDest;                         // set destination buffer
    if ((ulRules & DMA_AUTOREPEAT) != 0) {                               // {1} the DMA is to be automatically repeated after each transfer has completed
        ulRepeatLength[ucDMA_channel] = ulBufLength;                     // the full-buffer length to be repeated
        if ((ulRules & DMA_HALF_BUFFER_INTERRUPT) != 0) {
        #if defined _WINDOWS
            if ((ulBufLength%(ucSize * 2)) != 0) {
                _EXCEPTION("Buffer length is expected to be divisible by twice the transfer size");
            }
        #endif
            ulBufLength /= 2;                                            // half buffer mode length
            ucDirectionOutput[ucDMA_channel] |= DMA_TRANSFER_HALF_BUFFER;// emulate half buffer interrupt operation
        }
    }
    else {
        ulRepeatLength[ucDMA_channel] = 0;                               // no automatic repetition based on end of buffer interrupt
    }
    _DMA_handler[ucDMA_channel] = int_handler;                           // user interrupt callback
    if ((int_handler != 0) || ((ulRules & DMA_AUTOREPEAT) != 0)) {       // if there is a buffer interrupt handler at the end of DMA buffer operation (only full buffer interrupt is supported by the KL DMA controller)
        ptrDMA->DMA_DSR_BCR = ulBufLength;                               // set transfer count (don't set DMA_DSR_BCR_DONE at the same time otherwise BCR is reset)
        fnEnterInterrupt((irq_DMA0_ID + ucDMA_channel), int_priority, (void (*)(void))_DMA_Interrupt[ucDMA_channel]); // enter DMA interrupt handler on buffer completion
        ptrDMA->DMA_DCR |= (DMA_DCR_EINT | DMA_DCR_D_REQ);               // interrupt when the transmit buffer is empty and stop operation after full buffer has been transferred
    }
    else {                                                               // else free-running in circular buffer without any interrupt (source buffer must also be aligned to the correct buffer boundary!!)
        ptrDMA->DMA_DSR_BCR = 0xffff0;                                   // set maximum transfer count (this needs to be regularly retriggered for infinite operation)
        if ((DMA_NO_MODULO & ulRules) == 0) {                            // if modulo operation hasn't been disabled
            unsigned long ulMod = DMA_DCR_SMOD_256K;                     // configure circular buffer operation - the length and buffer alignment must be suitable
    #if defined _WINDOWS
            if ((ulBufLength != 16) && (ulBufLength != 32) && (ulBufLength != 64) && (ulBufLength != 128) && (ulBufLength != 256) && (ulBufLength != 512) && (ulBufLength != 1024) && (ulBufLength != (2 * 1024)) && (ulBufLength != (4 * 1024)) && (ulBufLength != (8 * 1024)) && (ulBufLength != (16 * 1024)) && (ulBufLength != (32 * 1024)) && (ulBufLength != (64 * 1024)) && (ulBufLength != (128 * 1024)) && (ulBufLength != (256 * 1024))) {
                _EXCEPTION("Invalid circular buffer size!!");
            }
            if ((unsigned long)ptrBufSource & (ulBufLength - 1)) {
                _EXCEPTION("Circular buffer not-aligned!!");
            }
    #endif
            while (ulBufLength < (256 * 1024)) {                         // calculate the modulo value required for the source
                ulBufLength *= 2;
                ulMod -= DMA_DCR_SMOD_16;
            }
            if ((ulRules & DMA_DIRECTION_OUTPUT) != 0) {                 // if the buffer is the destination
                ulMod >>= 4;                                             // move to destination MOD field
            }
            ptrDMA->DMA_DCR |= ulMod;                                    // the modulo setting
        }
    }
    POWER_UP(6, SIM_SCGC6_DMAMUX0);                                      // enable DMA multiplexer 0
    *(unsigned char *)(DMAMUX0_BLOCK + ucDMA_channel) = (ucDmaTriggerSource | DMAMUX_CHCFG_ENBL); // connect trigger to DMA channel
    ptrDMA->DMA_DCR |= (DMA_DCR_CS | DMA_DCR_EADREQ);                    // enable peripheral request - single cycle for each request (asynchronous requests enabled in stop mode)
    #else
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    ptrDMA_TCD += ucDMA_channel;
    if ((DMA_FIXED_ADDRESSES & ulRules) != 0) {                          // if both source and destination addresses are fixed
        ptrDMA_TCD->DMA_TCD_SOFF = 0;                                    // source not incremented
        ptrDMA_TCD->DMA_TCD_DOFF = 0;                                    // destination not incremented
        ptrDMA_TCD->DMA_TCD_SLAST = 0;                                   // no source displacement on transmit buffer completion
        ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;                                // no destination displacement on transmit buffer completion
    }
    else {
        if ((ulRules & DMA_DIRECTION_OUTPUT) != 0) {                     // buffer to fixed output
            ptrDMA_TCD->DMA_TCD_SOFF = ucSize;                           // source increment (buffer)
            ptrDMA_TCD->DMA_TCD_DOFF = 0;                                // destination not incremented
            ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;                            // {3} no destination displacement on transmit buffer completion
            ptrDMA_TCD->DMA_TCD_SLAST = (-(signed long)(ulBufLength));   // {3} when the buffer has been transmitted set the destination back to the start of it
        }
        else {                                                           // fixed input to buffer
            ptrDMA_TCD->DMA_TCD_SOFF = 0;                                // source not incremented
            ptrDMA_TCD->DMA_TCD_DOFF = ucSize;                           // destination increment one word (buffer)
            ptrDMA_TCD->DMA_TCD_DLASTSGA = (-(signed long)(ulBufLength));// {3} when the buffer has been filled set the destination back to the start of it
            ptrDMA_TCD->DMA_TCD_SLAST = 0;                               // {3} no source displacement on receive buffer completion
        }
    }
    switch (ucSize) {                                                    // the individual transfer size
    default:
    case 1:                                                              // single byte
        ucSize = 1;
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_8 | DMA_TCD_ATTR_SSIZE_8); // transfer sizes bytes
        break;
    case 2:                                                              // half-word
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_16 | DMA_TCD_ATTR_SSIZE_16); // transfer sizes words
        break;
    case 4:                                                              // word
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // transfer sizes long words
        break;
    }
    ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)ptrBufSource;             // source buffer
    ptrDMA_TCD->DMA_TCD_NBYTES_ML = ucSize;                              // each request starts a single transfer of this size
    _DMA_handler[ucDMA_channel] = int_handler;                           // user interrupt callback
    if (int_handler != 0) {                                              // if there is a buffer interrupt handler at the end of DMA buffer operation
        if ((ulRules & DMA_HALF_BUFFER_INTERRUPT) != 0) {
            ptrDMA_TCD->DMA_TCD_CSR = (DMA_TCD_CSR_INTMAJOR | DMA_TCD_CSR_INTHALF); // interrupt when the transmit buffer is half full (and when full)
        }
        else {
            ptrDMA_TCD->DMA_TCD_CSR = (DMA_TCD_CSR_INTMAJOR);            // interrupt when the transmit buffer is full
        }
        fnEnterInterrupt((irq_DMA0_ID + ucDMA_channel), int_priority, (void (*)(void))_DMA_Interrupt[ucDMA_channel]); // enter DMA interrupt handler on ful/half buffer completion
    }
    else {
        ptrDMA_TCD->DMA_TCD_CSR = 0;                                     // free-running mode without any interrupt
    }
    ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)ptrBufDest;               // destination
  //ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;                                    // {3} no destination displacement on transmit buffer completion
  //ptrDMA_TCD->DMA_TCD_SLAST = (-(signed long)(ulBufLength));           // {3} when the buffer has been transmitted set the destination back to the start of it
    ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = (signed short)(ulBufLength/ucSize); // the number of service requests to be performed each cycle
    POWER_UP(6, SIM_SCGC6_DMAMUX0);                                      // enable DMA multiplexer 0
    *(unsigned char *)(DMAMUX0_BLOCK + ucDMA_channel) = (ucDmaTriggerSource | DMAMUX_CHCFG_ENBL); // connect trigger source to DMA channel
    #endif
    #if defined _WINDOWS                                                 // simulator checks to help detect incorrect usage
    if (DMA_MEMCPY_CHANNEL == ucDMA_channel) {
        _EXCEPTION("Warning - peripheral DMA is using the channel reserved for DMA based uMemcpy()!!");
    }
        #if defined DMA_MEMCPY_CHANNEL_ALT                               // {5}
    if (DMA_MEMCPY_CHANNEL_ALT == ucDMA_channel) {
        _EXCEPTION("Warning - peripheral DMA is using the alternative channel reserved for DMA based uMemcpy()!!");
    }
        #endif
    if (DMAMUX0_DMA0_CHCFG_SOURCE_PIT0 == ucDmaTriggerSource) {
        if (ucDMA_channel != 0) {
            _EXCEPTION("PIT0 trigger only operates on DMA channel 0!!");
        }
        #if defined ERRATA_ID_5746
        if ((ptrDMA->DMA_DCR & DMA_DCR_CS) != 0) {
            _EXCEPTION("PIT0 trigger generates two data transfers when in cycle-steal mode!!");
        }
        #endif
    }
    else if (DMAMUX0_DMA0_CHCFG_SOURCE_PIT1 == ucDmaTriggerSource) {
        if (ucDMA_channel != 1) {
            _EXCEPTION("PIT1 trigger only operates on DMA channel 1!!");
        }
        #if defined ERRATA_ID_5746
        if ((ptrDMA->DMA_DCR & DMA_DCR_CS) != 0) {
            _EXCEPTION("PIT1 trigger generates two data transfers when in cycle-steal mode!!");
        }
        #endif
    }
    else if (DMAMUX0_DMA0_CHCFG_SOURCE_PIT2 == ucDmaTriggerSource) {
        if (ucDMA_channel != 2) {
            _EXCEPTION("PIT2 trigger only operates on DMA channel 2!!");
        }
    }
    else if (DMAMUX0_DMA0_CHCFG_SOURCE_PIT3 == ucDmaTriggerSource) {
        if (ucDMA_channel != 3) {
            _EXCEPTION("PIT3 trigger only operates on DMA channel 3!!");
        }
    }
    #endif
    // Note that the DMA channel has not been activated yet - to do this fnDMA_BufferReset(channel_number, DMA_BUFFER_START); is performed
    //
}
#endif

/* =================================================================== */
/*            DMA based memcpy(), memset() and reverse memcpy()        */
/* =================================================================== */

#if defined _DMA_MEM_TO_MEM
#if defined DMA_MEMCPY_SET && !defined DEVICE_WITHOUT_DMA
//#define AVOID_PARTITION_BURST
#define SMALLEST_DMA_COPY 15                                             // smaller copies have no DMA advantage
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)      // {9}
{
    register unsigned char *ptr = (unsigned char *)ptrTo;
    register unsigned char *buffer = (unsigned char *)ptrFrom;

    #if defined AVOID_PARTITION_BURST
        if (buffer < (unsigned char *)0x20000000) {
            if ((buffer + Size) >= (unsigned char *)0x20000000) {        // avoid the source from performing bursts from low to high memory area
                goto _do_simple_copy;
            }
        }
        if (ptr < (unsigned char *)0x20000000) {
            if ((ptr + Size) >= (unsigned char *)0x20000000) {           // avoid the destination from performing bursts from low to high memory area
                goto _do_simple_copy;
            }
        }
    #endif

    if (Size >= SMALLEST_DMA_COPY) {                                     // if large enough to be worthwhile
    #if defined KINETIS_KL                                               // {80}
        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
        ptrDMA += DMA_MEMCPY_CHANNEL;
        if (ptrDMA->DMA_DCR == 0) {                                      // if not already in use
            unsigned long ulTransfer;
            while (((unsigned long)buffer) & 0x3) {                      // {87} move to a long word boundary for source
                *ptr++ = *buffer++;
                Size--;
            }
          //ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                      // {2} clear the DONE flag and clear errors etc.
            if (((unsigned long)ptr & 0x3) != 0) {                       // if the destination is not also long word aligned
                if ((unsigned long)ptr & 0x1) {                          // not short word aligned
                    ulTransfer = Size;
                    ptrDMA->DMA_DCR = (DMA_DCR_DINC | DMA_DCR_SINC | DMA_DCR_D_REQ | DMA_DCR_DSIZE_8 | DMA_DCR_SSIZE_8 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // set mode and protect from interrupts that could use the function in the process
                }
                else {
                    ulTransfer = (Size & ~0x1);                          // ensure length is suitable for short words
                    ptrDMA->DMA_DCR = (DMA_DCR_DINC | DMA_DCR_SINC | DMA_DCR_D_REQ | DMA_DCR_DSIZE_16 | DMA_DCR_SSIZE_16 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // set mode and protect from interrupts that could use the function in the process
                }
            }
            else {
                ptrDMA->DMA_DCR = (DMA_DCR_DINC | DMA_DCR_SINC | DMA_DCR_D_REQ | DMA_DCR_DSIZE_32 | DMA_DCR_SSIZE_32 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // set mode and protect from interrupts that could use the function in the process
                ulTransfer = (Size & ~0x3);                              // ensure length is suitable for long words
            }
            ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                      // {2} clear the DONE flag and clear errors etc.
            ptrDMA->DMA_SAR = (unsigned long)buffer;                     // set address of sourse
            ptrDMA->DMA_DAR = (unsigned long)ptr;                        // set address of destination
            ptrDMA->DMA_DSR_BCR = (ulTransfer & DMA_DSR_BCR_BCR_MASK);   // set transfer count (don't set DMA_DSR_BCR_DONE at the same time otherwise BCR is reset)
            ptrDMA->DMA_DCR |= (DMA_DCR_START);                          // start DMA transfer
        #if defined _WINDOWS
            ptrDMA->DMA_DSR_BCR &= ~(DMA_DSR_BCR_DONE);
            ptrDMA->DMA_DSR_BCR |= DMA_DSR_BCR_BSY;
        #endif
            ptr += ulTransfer;                                           // move the destination pointer to beyond the transfer
            buffer += ulTransfer;                                        // move the source pointer to beyond the transfer
            Size -= ulTransfer;                                          // bytes remaining

            while ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_DONE) == 0) { fnSimulateDMA(DMA_MEMCPY_CHANNEL); } // wait until completed

            while (Size-- != 0) {                                        // {103}{87} complete any remaining bytes
                *ptr++ = *buffer++;
            }

            ptrDMA->DMA_DCR = 0;                                         // free the DMA channel for further use
            return ptrTo;                                                // return pointer to original buffer according to memcpy() declaration
        }
    #else
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += DMA_MEMCPY_CHANNEL;
        #if defined DMA_MEMCPY_CHANNEL_ALT                               // {5}
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK != 0) {                      // if the main channel is already in use
            ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
            ptrDMA_TCD += DMA_MEMCPY_CHANNEL_ALT;                        // move to the alternate channel (may be used by interrupts)
        }
        #endif
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                      // if not already in use
            unsigned long ulTransfer;
            while ((((unsigned long)buffer) & 0x3) != 0) {               // {87} move to a long word boundary for source
                *ptr++ = *buffer++;
                Size--;
            }
            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;                         // {4} one main loop iteration - this protects the DMA channel from interrupt routines that may also want to use the function
            if (((unsigned long)ptr & 0x3) != 0) {                       // if the destination is not also long word aligned
                if (((unsigned long)ptr & 0x1) != 0) {                   // not short word aligned
                    ulTransfer = Size;
                    ptrDMA_TCD->DMA_TCD_DOFF = 1;                        // destination has to be byte aligned
                    ptrDMA_TCD->DMA_TCD_SOFF = 1;                        // source has to be byte aligned
                    ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_8 | DMA_TCD_ATTR_SSIZE_8);
                }
                else {
                    ulTransfer = (Size & ~0x1);                          // ensure length is suitable for short words
                    ptrDMA_TCD->DMA_TCD_DOFF = 2;                        // destination has to be short word aligned
                    ptrDMA_TCD->DMA_TCD_SOFF = 2;                        // source has to be short word aligned
                    ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_16 | DMA_TCD_ATTR_SSIZE_16);
                }
            }
            else {
                ulTransfer = (Size & ~0x3);                              // ensure length is suitable for long words
            }
          //ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;                         // {4} one main loop iteration - this protects the DMA channel from interrupt routines that may also want to use the function
            ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)buffer;           // set source for copy
            ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)ptr;              // set destination for copy
            ptrDMA_TCD->DMA_TCD_NBYTES_ML = ulTransfer;                  // set number of bytes to be copied
            ptrDMA_TCD->DMA_TCD_CSR = DMA_TCD_CSR_START;                 // start DMA transfer

            ptr += ulTransfer;                                           // move the destination pointer to beyond the transfer
            buffer += ulTransfer;                                        // move the source pointer to beyond the transfer
            Size -= ulTransfer;                                          // bytes remaining

            while ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_DONE) == 0) { fnSimulateDMA(DMA_MEMCPY_CHANNEL); } // wait until completed

            while (Size-- != 0) {                                        // {103}{87} complete any remaining bytes
                *ptr++ = *buffer++;
            }

            ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // ensure standard settings are returned
            ptrDMA_TCD->DMA_TCD_DOFF = 4;                                // destination increment long word
            ptrDMA_TCD->DMA_TCD_SOFF = 4;                                // source increment long word

            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 0;                         // allow further use of the channel for DMA memory copy functions
            return ptrTo;                                                // return pointer to original buffer according to memset() declaration
        }
    #endif
    }
#if defined AVOID_PARTITION_BURST
_do_simple_copy:
#endif
    // Normal memcpy() solution
    //
    while (Size-- != 0) {
        *ptr++ = *buffer++;                                              // copy from input buffer to output buffer
    }
    return ptrTo;                                                        // return pointer to original buffer according to memcpy() declaration
}

    #if defined UREVERSEMEMCPY && !defined DMA_REVMEMCPY_NOT_POSSIBLE    // {59} KL part don't support this
// uMemcpy() executed in a reverse order to allow shifting buffers to the rights
//
extern void *uReverseMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    register unsigned char *ptr = (unsigned char *)ptrTo;
    register unsigned char *buffer = (unsigned char *)ptrFrom;

    if (Size >= SMALLEST_DMA_COPY) {                                     // if large enough to be worthwhile
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += DMA_MEMCPY_CHANNEL;
        #if defined DMA_MEMCPY_CHANNEL_ALT                               // {5}
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK != 0) {                      // if the main channel is already in use
            ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
            ptrDMA_TCD += DMA_MEMCPY_CHANNEL_ALT;                        // move to the alternate channel (may be used by interrupts)
        }
        #endif
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                      // if not already in use
            register unsigned char *ptrEndTo = ((unsigned char *)ptrTo + Size);
            register unsigned char *ptrEndFrom = ((unsigned char *)ptrFrom + Size);
            int iFillEnd = 0;
            int iOffset;
            unsigned long ulTransfer;
            ptr = (ptrEndTo - 4);                                        // move to final long word
            buffer = (ptrEndFrom - 4);                                   // move to final long word
            while ((((unsigned long)buffer) & 0x3) != 0) {               // {87} move to a long word boundary for source
                iFillEnd++;
                ptr--;
                buffer--;
                Size--;
            }
            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;                         // {4} one main loop iteration - this protects the DMA channel from interrupt routines that may also want to use the function
            if (((unsigned long)ptr & 0x3) != 0) {                       // if the destination is not also long word aligned
                if (((unsigned long)ptr & 0x1) != 0) {                   // not short word aligned
                    ptr += (iFillEnd + 3);
                    buffer += (iFillEnd + 3);
                    Size += iFillEnd;
                    iFillEnd = 0;                                        // perform complete process using byte DMA transfers
                    ulTransfer = Size;
                    ptrDMA_TCD->DMA_TCD_DOFF = -1;                       // destination has to be byte aligned
                    ptrDMA_TCD->DMA_TCD_SOFF = -1;                       // source has to be byte aligned
                    ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_8 | DMA_TCD_ATTR_SSIZE_8);
                }
                else {                                                   // short word aligned
                    ptr += 2;
                    buffer += 2;
                    ulTransfer = (Size & ~0x1);                          // ensure length is suitable for short words
                    ptrDMA_TCD->DMA_TCD_DOFF = -2;                       // destination has to be short word aligned
                    ptrDMA_TCD->DMA_TCD_SOFF = -2;                       // source has to be short word aligned
                    ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_16 | DMA_TCD_ATTR_SSIZE_16);
                }
                iOffset = 0;
            }
            else {
                switch (Size & 0x3) {
                case 0x00:
                    iOffset = 0;
                    break;
                case 0x01:
                    iOffset = 2;
                    break;
                case 0x02:
                    iOffset = 1;
                    break;
                case 0x03:
                    iOffset = 0;
                    break;
                }
                ulTransfer = (Size & ~0x3);                              // ensure length is suitable for long words
                ptrDMA_TCD->DMA_TCD_SOFF = -4;                           // source decrement
                ptrDMA_TCD->DMA_TCD_DOFF = -4;                           // destination decrement
            }
            while (iFillEnd != 0) {                                      // transfers at the end need be be performed before the DMA is started to ensure that the source remains stable
                *(--ptrEndTo) = *(--ptrEndFrom);
                iFillEnd--;
            }

          //ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;                         // {4} one main loop iteration - this protects the DMA channel from interrupt routines that may also want to use the function
            ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)buffer;           // set source for copy
            ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)ptr;              // set destination for copy
            ptrDMA_TCD->DMA_TCD_NBYTES_ML = ulTransfer;                  // set number of bytes to be copied
            ptrDMA_TCD->DMA_TCD_CSR = DMA_TCD_CSR_START;                 // start DMA transfer (backwards)

            Size -= ulTransfer;                                          // bytes remaining
            ulTransfer -= Size;
            ulTransfer -= iOffset;
            ptr -= ulTransfer;                                           // move the destination pointer to before the transfer
            buffer -= ulTransfer;

            while ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_DONE) == 0) { fnSimulateDMA(DMA_MEMCPY_CHANNEL); } // wait until completed

            while (Size-- != 0) {                                        // {87} complete any remaining bytes
                *ptr-- = *buffer--;
            }

            ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // ensure standard settings are returned
            ptrDMA_TCD->DMA_TCD_DOFF = 4;                                // destination increment long word
            ptrDMA_TCD->DMA_TCD_SOFF = 4;                                // source increment long word
            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 0;                         // allow further use of the channel for DMA memory copy functions
            return ptrTo;                                                // return pointer to original buffer according to memcpy() declaration
        }
    }
    // Normal software method
    //
    ptr += Size;                                                         // move to the end of the buffers
    buffer += Size;

    while (Size-- != 0) {
        *(--ptr) = *(--buffer);                                          // copy backwards
    }
    return ptrTo;                                                        // return pointer to original buffer according to memcpy() declaration
}
    #endif

// memset implementation
//
extern void *uMemset(void *ptrTo, unsigned char ucValue, size_t Size)    // {9}
{
    register unsigned char *ptr = (unsigned char *)ptrTo;

    if (Size >= SMALLEST_DMA_COPY) {                                     // if large enough to be worthwhile 
    #if defined KINETIS_KL                                               // {80}
        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
        ptrDMA += DMA_MEMCPY_CHANNEL;
        if (ptrDMA->DMA_DCR == 0) {                                      // if not already in use
            unsigned long ulTransfer;
            volatile unsigned long ulToCopy = (ucValue | (ucValue << 8) | (ucValue << 16) | (ucValue << 24));
            while (((unsigned long)ptr) & 0x3) {                         // {87} move to a long word boundary
                *ptr++ = ucValue;
                Size--;
            }
            ptrDMA->DMA_DCR = (DMA_DCR_DINC | DMA_DCR_D_REQ | DMA_DCR_DSIZE_32 | DMA_DCR_SSIZE_32 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // {2} set mode and protect from interrupts that could use the function in the process (protect from interrupt use from here on in)
            ulTransfer = (Size & (~0x3 & DMA_DSR_BCR_BCR_MASK));
            ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                      // clear the DONE flag and clear errors etc.
            ptrDMA->DMA_DSR_BCR = (ulTransfer | DMA_DSR_BCR_DONE);       // set transfer count and clear the DONE flag
            ptrDMA->DMA_SAR = (unsigned long)&ulToCopy;                  // set address of long word value to be set
            ptrDMA->DMA_DAR = (unsigned long)ptrTo;                      // set destination for copy
            ptrDMA->DMA_DSR_BCR = ulTransfer;                            // set transfer count (don't set DMA_DSR_BCR_DONE at the same time otherwise BCR is reset)
            ptrDMA->DMA_DCR = (DMA_DCR_START | DMA_DCR_DINC | DMA_DCR_D_REQ | DMA_DCR_DSIZE_32 | DMA_DCR_SSIZE_32 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // start DMA transfer
        #if defined _WINDOWS
            ptrDMA->DMA_DSR_BCR &= ~(DMA_DSR_BCR_DONE);
            ptrDMA->DMA_DSR_BCR |= DMA_DSR_BCR_BSY;
        #endif
            ptr += ulTransfer;                                           // move the destination pointer to beyond the transfer
            Size -= ulTransfer;                                          // bytes remaining
            while (Size--) {                                             // {87} complete any remaining bytes
                *ptr++ = ucValue;
            }
            while ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_DONE) == 0) { fnSimulateDMA(DMA_MEMCPY_CHANNEL); } // wait until completed

            ptrDMA->DMA_DCR = 0;                                         // free the DMA channel for further use
            return ptrTo;                                                // return pointer to original buffer according to memcpy() declaration
        }
    #else
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += DMA_MEMCPY_CHANNEL;
        #if defined DMA_MEMCPY_CHANNEL_ALT                               // {5}
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK != 0) {                      // if the main channel is already in use
            ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
            ptrDMA_TCD += DMA_MEMCPY_CHANNEL_ALT;                        // move to the alternate channel (may be used by interrupts)
        }
        #endif
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                      // if not already in use
            volatile unsigned long ulToCopy = (ucValue | (ucValue << 8) | (ucValue << 16) | (ucValue << 24));
            unsigned long ulTransfer;
            while ((((unsigned long)ptr) & 0x3) != 0) {                  // {87} move to a long word boundary
                *ptr++ = ucValue;
                Size--;
            }
            ulTransfer = (Size & ~0x3);
            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;                         // one main loop iteration - this protects the DMA channel from interrupt routines that may also want to use the function
            ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)&ulToCopy;        // set address of long word value to be set
            ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)ptr;              // set destination for copy
            ptrDMA_TCD->DMA_TCD_NBYTES_ML = ulTransfer;                  // set number of bytes to be copied
            ptrDMA_TCD->DMA_TCD_SOFF = 0;                                // no source increment
            ptrDMA_TCD->DMA_TCD_CSR = DMA_TCD_CSR_START;                 // start DMA transfer

            ptr += ulTransfer;                                           // move the destination pointer to beyond the transfer
            Size -= ulTransfer;                                          // bytes remaining
            while (Size-- != 0) {                                        // {87} complete any remaining bytes
                *ptr++ = ucValue;
            }

            while ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_DONE) == 0) { fnSimulateDMA(DMA_MEMCPY_CHANNEL); } // wait until completed
            ptrDMA_TCD->DMA_TCD_SOFF = 4;                                // set source increment for the uMemcpy() function
            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 0;                         // allow further use of the channel for DMA memory copy functions
            return ptrTo;                                                // return pointer to original buffer according to memcpy() declaration
        }
    #endif
    }

    // SW memset method
    //
    while (Size-- != 0) {
        *ptr++ = ucValue;                                                // set the value to each location
    }
    return ptrTo;                                                        // return pointer to original buffer according to memset() declaration
}
#endif

#endif
