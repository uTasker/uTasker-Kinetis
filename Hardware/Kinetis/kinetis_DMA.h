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
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    04.01.2016 Added automatic DMA half- and full-buffer handling for KL devices {1}
    05.02.2016 Protect KL memcpy DMA before clearing the DONE flag               {2}

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
#endif


/* =================================================================== */
/*                       DMA Interrupt Handlers                        */
/* =================================================================== */

static void _DMA_Handler(int iChannel)
{
    #if defined KINETIS_KL
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += iChannel;
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
                ptrDMA->DMA_SAR -= (ulRepeatLength[iChannel]);           // set the source pointer back to the start of the buffer
            }
            else {
                ptrDMA->DMA_DAR -= (ulRepeatLength[iChannel]);           // set the destination pointer back to the start of the buffer
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


extern void fnDMA_BufferReset(int iChannel)
{
    #if defined KINETIS_KL
    unsigned long ulRewind;
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += iChannel;
    ptrDMA->DMA_DCR &= ~(DMA_DCR_ERQ);                                   // disable operation
    ulRewind = (ulRepeatLength[iChannel] - (ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK)); // the buffer length minus the transfers that haven't taken place in the present buffer
    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear the DONE flag and clear errors etc.
    if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_HALF_BUFFER) != 0) { // if emulating half-buffer interrupt
        if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_SECOND_BUFFER) == 0) { // first half of the buffer was in operation
            ucDirectionOutput[iChannel] &= ~(DMA_TRANSFER_SECOND_BUFFER);// start at first buffer half
            ulRewind -= (ulRepeatLength[iChannel]/2);                    // correct the size to rewind to respect that only half the buffer was programmed
        }
        ptrDMA->DMA_DSR_BCR = (ulRepeatLength[iChannel]/2);              // the half-buffer length
    }
    else {                                                               // full-buffer operation
        ptrDMA->DMA_DSR_BCR = ulRepeatLength[iChannel];                  // the buffer length
    }
    if ((ucDirectionOutput[iChannel] & DMA_TRANSFER_OUTPUT) != 0) {
        ptrDMA->DMA_SAR -= ulRewind;                                     // set the source pointer back to the start of the buffer
    }
    else {
        ptrDMA->DMA_DAR -= ulRewind;                                     // set the destination pointer back to the start of the buffer
    }
    ptrDMA->DMA_DCR |= (DMA_DCR_ERQ);                                    // restart DMA from the start of the buffer
    #endif
}

/* =================================================================== */
/*                         DMA Configuration                           */
/* =================================================================== */

// Buffer source to fixed short word address or fixed short word address to buffer
//
static void fnConfigDMA_buffer(unsigned char ucDMA_channel, unsigned char ucDmaTriggerSource, unsigned long ulBufLength, void *ptrBufSource, void *ptrBufDest, int iAutoRepeat, int iHalfBufferInterrupt, void (*int_handler)(void), int int_priority, int iOutput)
{
    #if defined KINETIS_KL
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
        #if defined _WINDOWS
    if (ucDMA_channel >= DMA_CHANNEL_COUNT) {
        _EXCEPTION("Error - peripheral DMA is specifying a non-existent channel!!");
    }
            #if defined DMA_MEMCPY_SET
    if (DMA_MEMCPY_CHANNEL == ucDMA_channel) {
        _EXCEPTION("Warning - peripheral DMA is using the channel reserved for DMA based uMemcpy()!!");
    }
            #endif
        #endif
    ptrDMA += ucDMA_channel;
    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear the DONE flag and clear errors etc.
    if (iOutput != 0) {                                                  // buffer to fixed output
        ptrDMA->DMA_DCR = (DMA_DCR_SINC | DMA_DCR_DSIZE_16 | DMA_DCR_SSIZE_16 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // 16 bit transfers with increment only on source
        ucDirectionOutput[ucDMA_channel] = DMA_TRANSFER_OUTPUT;
    }
    else {                                                               // fixed input to buffer
        ptrDMA->DMA_DCR = (DMA_DCR_DINC | DMA_DCR_DSIZE_16 | DMA_DCR_SSIZE_16 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF); // 16 bit transfers with increment only on destination
        ucDirectionOutput[ucDMA_channel] = DMA_TRANSFER_INPUT;
    }
    ptrDMA->DMA_SAR = (unsigned long)ptrBufSource;                       // source buffer
    ptrDMA->DMA_DAR = (unsigned long)ptrBufDest;                         // destination buffer
    if (iAutoRepeat != 0) {                                              // {1} the DMA is to be automatically repeated after each transfer has completed
        ulRepeatLength[ucDMA_channel] = ulBufLength;                     // the full-buffer length to be repeated
        if (iHalfBufferInterrupt != 0) {
        #if defined _WINDOWS
            if ((ulBufLength%4) != 0) {
                _EXCEPTION("Buffer length is expected to be divisible by 4 - half-buffer of half-words");
            }
        #endif
            ulBufLength /= 2;                                            // half buffer mode length
            ucDirectionOutput[ucDMA_channel] |= DMA_TRANSFER_HALF_BUFFER;// emulate half buffer interrupt operation
        }
    }
    else {
        ulRepeatLength[ucDMA_channel] = 0;                               // no automatic repetition based on end of buffer interrupt
    }
    ptrDMA->DMA_DSR_BCR = ulBufLength;                                   // set transfer count (don't set DMA_DSR_BCR_DONE at the same time otherwise BCR is reset)
    _DMA_handler[ucDMA_channel] = int_handler;                           // user interrupt callback
    if ((int_handler != 0) || (iAutoRepeat != 0)) {                      // if there is a buffer interrupt handler at the end of DMA buffer operation (only full buffer interrupt is supported by the KL DMA controller)
        fnEnterInterrupt((irq_DMA0_ID + ucDMA_channel), int_priority, (void (*)(void))_DMA_Interrupt[ucDMA_channel]); // enter DMA interrupt handler on buffer completion
        ptrDMA->DMA_DCR |= (DMA_DCR_EINT | DMA_DCR_D_REQ);               // interrupt when the transmit buffer is empty and stop operation after full buffer has been transferred
    }
    else {                                                               // else free-running in circular buffer without any interrupt (source buffer must also be aligned to the correct buffer boundary!!)
        unsigned long ulMod = DMA_DCR_SMOD_256K;                         // configure circular buffer operation - the length and buffer alignment must be suitable
        ptrDMA->DMA_DSR_BCR = 0xffff0;                                   // set maximum transfer count (this needs to be regularly retriggered for infinite operation)
        #if defined _WINDOWS
        if ((ulBufLength != 16) && (ulBufLength != 32) && (ulBufLength != 64) && (ulBufLength != 128) && (ulBufLength != 256) && (ulBufLength != 512) && (ulBufLength != 1024) && (ulBufLength != (2 * 1024)) && (ulBufLength != (4 * 1024)) && (ulBufLength != (8 * 1024)) && (ulBufLength != (16 * 1024)) && (ulBufLength != (32 * 1024)) && (ulBufLength != (64 * 1024)) && (ulBufLength != (128 * 1024)) && (ulBufLength != (256 * 1024))) {
            _EXCEPTION("Invalid circular buffer size!!");
        }
        if ((unsigned long)ptrBufSource & (ulBufLength - 1)) {
            _EXCEPTION("Circular buffer not-aligned!!");
        }
        #endif
        while (ulBufLength < (256 * 1024)) {                             // calculate the modulo value required for the source
            ulBufLength *= 2;
            ulMod -= DMA_DCR_SMOD_16;
        }
        if (iOutput == 0) {                                              // if the buffer is the destination
            ulMod >>= 4;                                                 // move to destination MOD field
        }
        ptrDMA->DMA_DCR |= ulMod;
    }
    POWER_UP(6, SIM_SCGC6_DMAMUX0);                                      // enable DMA multiplexer 0
    *(unsigned char *)(DMAMUX0_BLOCK + ucDMA_channel) = (ucDmaTriggerSource | DMAMUX_CHCFG_ENBL); // connect trigger to DMA channel
    ptrDMA->DMA_DCR |= (DMA_DCR_ERQ | DMA_DCR_CS | DMA_DCR_EADREQ);      // enable peripheral request - single cycle for each request (asynchronous requests enabled in stop mode)
    #else
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    ptrDMA_TCD += ucDMA_channel;
    if (iOutput != 0) {                                                  // buffer to fixed output
        ptrDMA_TCD->DMA_TCD_SOFF = 2;                                    // source increment one word (buffer)
        ptrDMA_TCD->DMA_TCD_DOFF = 0;                                    // destination not incremented
    }
    else {                                                               // fixed input to buffer
        ptrDMA_TCD->DMA_TCD_SOFF = 0;                                    // source not incremented
        ptrDMA_TCD->DMA_TCD_DOFF = 2;                                    // destination increment one word (buffer)
    }
    ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_16 | DMA_TCD_ATTR_SSIZE_16); // transfer sizes always words
    ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)ptrBufSource;             // source buffer
    ptrDMA_TCD->DMA_TCD_NBYTES_ML = 2;                                   // each request starts a single transfer of a word
    _DMA_handler[ucDMA_channel] = int_handler;                           // user interrupt callback
    if (int_handler != 0) {                                              // if there is a buffer interrupt handler at the end of DMA buffer operation
        if (iHalfBufferInterrupt != 0) {
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
    ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;                                    // no destination displacement on transmit buffer completion
    ptrDMA_TCD->DMA_TCD_SLAST = (-(signed long)(ulBufLength));           // when the buffer has been transmitted set the destination back to the start of it
    ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = (signed short)(ulBufLength/sizeof(signed short)); // the number of service requests
    POWER_UP(6, SIM_SCGC6_DMAMUX0);                                      // enable DMA multiplexer 0
    *(unsigned char *)(DMAMUX0_BLOCK + ucDMA_channel) = (ucDmaTriggerSource | DMAMUX_CHCFG_ENBL); // connect trigger source to DMA channel
    DMA_ERQ |= (DMA_ERQ_ERQ0 << ucDMA_channel);                          // enable request source
    #endif
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
CLEAR_TEST_OUTPUT();
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

            while (Size--) {                                             // {103}{87} complete any remaining bytes
                *ptr++ = *buffer++;
            }

            ptrDMA->DMA_DCR = 0;                                         // free the DMA channel for further use
SET_TEST_OUTPUT();
            return ptrTo;                                                // return pointer to original buffer according to memcpy() declaration
        }
    #else
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += DMA_MEMCPY_CHANNEL;
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                      // if not already in use
            unsigned long ulTransfer;
            while (((unsigned long)buffer) & 0x3) {                      // {87} move to a long word boundary for source
                *ptr++ = *buffer++;
                Size--;
            }
            if (((unsigned long)ptr & 0x3) != 0) {                       // if the destination is not also long word aligned
                if ((unsigned long)ptr & 0x1) {                          // not short word aligned
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
            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;                         // one main loop iteration - this protects the DMA channel from interrupt routines that may also want to use the function
            ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)buffer;           // set source for copy
            ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)ptr;              // set destination for copy
            ptrDMA_TCD->DMA_TCD_NBYTES_ML = ulTransfer;                  // set number of bytes to be copied
            ptrDMA_TCD->DMA_TCD_CSR = DMA_TCD_CSR_START;                 // start DMA transfer

            ptr += ulTransfer;                                           // move the destination pointer to beyond the transfer
            buffer += ulTransfer;                                        // move the source pointer to beyond the transfer
            Size -= ulTransfer;                                          // bytes remaining

            while ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_DONE) == 0) { fnSimulateDMA(DMA_MEMCPY_CHANNEL); } // wait until completed

            while (Size--) {                                             // {103}{87} complete any remaining bytes
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
    while (Size--) {
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
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                      // if not already in use
            register unsigned char *ptrEndTo = ((unsigned char *)ptrTo + Size);
            register unsigned char *ptrEndFrom = ((unsigned char *)ptrFrom + Size);
            int iFillEnd = 0;
            int iOffset;
            unsigned long ulTransfer;
            ptr = (ptrEndTo - 4);                                        // move to final long word
            buffer = (ptrEndFrom - 4);                                   // move to final long word
            while (((unsigned long)buffer) & 0x3) {                      // {87} move to a long word boundary for source
                iFillEnd++;
                ptr--;
                buffer--;
                Size--;
            }
            if (((unsigned long)ptr & 0x3) != 0) {                       // if the destination is not also long word aligned
                if ((unsigned long)ptr & 0x1) {                          // not short word aligned
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
            while (iFillEnd) {                                           // transferes at the end need be be performed before the DMA is started to ensure that the source remains stable
                *(--ptrEndTo) = *(--ptrEndFrom);
                iFillEnd--;
            }

            ptrDMA_TCD->DMA_TCD_CITER_ELINK = 1;                         // one main loop iteration - this protects the DMA channel from interrupt routines that may also want to use the function
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

            while (Size--) {                                             // {87} complete any remaining bytes
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

    while (Size--) {
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
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                      // if not already in use
            volatile unsigned long ulToCopy = (ucValue | (ucValue << 8) | (ucValue << 16) | (ucValue << 24));
            unsigned long ulTransfer;
            while (((unsigned long)ptr) & 0x3) {                         // {87} move to a long word boundary
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
            while (Size--) {                                             // {87} complete any remaining bytes
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
    while (Size--) {
        *ptr++ = ucValue;                                                // set the value to each location
    }
    return ptrTo;                                                        // return pointer to original buffer according to memset() declaration
}
#endif

#endif
