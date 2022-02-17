/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      stm32_ADC.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/

#if defined _ADC_INTERRUPT_CODE

extern void fnDMA_BufferReset(int iChannel, int iAction);
    #define DMA_BUFFER_RESET     0                                           // disable DMA and reset the buffer (but don't re-enable yet)
    #define DMA_BUFFER_START     1                                           // start DMA operation (must have been configured previously)
    #define DMA_BUFFER_RESTART   2                                           // reset buffer and restart immediately

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _ADC_Interrupt(void);


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*_ADC_Interrupt_handler[ADC_CONTROLLERS])(ADC_INTERRUPT_RESULT *) = {0}; // user ADC interrupt handlers
static ADC_INTERRUPT_RESULT adc_result[ADC_CONTROLLERS] = {{0}};

/* =================================================================== */
/*                        ADC Interrupt Handlers                       */
/* =================================================================== */

// ADC conversion interrupt
//
static void fnEndOfConversion(int iADC, unsigned long ulValue)
{
    register int iSampleNumber = adc_result[iADC].ucSample;
    adc_result[iADC].usADC_value[iSampleNumber] = (unsigned short)ulValue;
    adc_result[iADC].ucADC_status[iSampleNumber] = (unsigned char)ADC_RESULT_VALID; // assumed that the result is valid
    if (++(adc_result[iADC].ucSample) > adc_result[iADC].ucSequenceLength) { // complete sequence ready
        adc_result[iADC].ucSample = 0;
        if (_ADC_Interrupt_handler[iADC] != 0) {                         // if there is a user handler
            uDisable_Interrupt();
                _ADC_Interrupt_handler[iADC](&adc_result[iADC]);         // call handling function to inform that the result is ready
            uEnable_Interrupt();
        }
    }
}

#if !defined DEVICE_WITHOUT_DMA
static void fnEndOfConversionDMA1(void)
{
    if ((ADC1_CR1 & ADC_CR1_AWDEN) != 0) {                               // analog watchdog mode in operation
        ADC1_CR2 &= ~(ADC_CR2_DMA);                                      // toggle the DMA mode control bit so that further DMA operations are executed
        adc_result[0].ucADC_status[(ADC1_CR1 & ADC_CR1_AWDCH_MASK)] = ADC_RESULT_VALID;
    #if defined _WINDOWS
        DMA2_S0NDTR = (unsigned long)(16);                               // the number of transfers (decremented after each trigger) - it seems like the HW puts this orignal value back when the DMA is re-enabled (?)
        DMA2_S0M0AR = (DMA2_S0M0AR - (16 * sizeof(unsigned short)));     // the original memory pointer - it seems like the HW sort of does this automatically(?)
    #endif
        ADC1_CR2 |= (ADC_CR2_DMA);
        fnDMA_BufferReset(2, DMA_BUFFER_START);                          // enable again
      //ADC1_CR2 |= ADC_CR2_SWSTART;                                     // start next conversion (not needed in loop mode)
    }
    else {
        register int iSamples = adc_result[0].ucSequenceLength;          // the sequence length that has just completed
        do {
            adc_result[0].ucADC_status[iSamples--] = (unsigned char)ADC_RESULT_VALID; // mark each entry as valid
        } while (iSamples >= 0);
        if (_ADC_Interrupt_handler[0] != 0) {                            // if there is a user handler
            uDisable_Interrupt();
            _ADC_Interrupt_handler[0](&adc_result[0]);                   // call handling function to inform that the result is ready
            uEnable_Interrupt();
        }
    }
}

static __interrupt void _DMA_Interrupt_0(void);
static __interrupt void _DMA_Interrupt_1(void);
static __interrupt void _DMA_Interrupt_2(void);
static __interrupt void _DMA_Interrupt_3(void);
static __interrupt void _DMA_Interrupt_4(void);
static __interrupt void _DMA_Interrupt_5(void);
static __interrupt void _DMA_Interrupt_6(void);
static __interrupt void _DMA_Interrupt_7(void);

static const unsigned char *_DMA_Interrupt[8] = {
    (unsigned char *)_DMA_Interrupt_0,
    (unsigned char *)_DMA_Interrupt_1,
    (unsigned char *)_DMA_Interrupt_2,
    (unsigned char *)_DMA_Interrupt_3,
    (unsigned char *)_DMA_Interrupt_4,
    (unsigned char *)_DMA_Interrupt_5,
    (unsigned char *)_DMA_Interrupt_6,
    (unsigned char *)_DMA_Interrupt_7,
};

static void(*_DMA_handler[8])(void) = {0};              // user DMA interrupt handlers

/* =================================================================== */
/*                       DMA Interrupt Handlers                        */
/* =================================================================== */

static void _DMA_Handler(int iChannel)
{
    if (_DMA_handler[iChannel] != 0) {                                   // if there is a user handler
        uDisable_Interrupt();
            _DMA_handler[iChannel]();                                    // call user handling function
        uEnable_Interrupt();
    }
}

static void _DMA_Interrupt_0(void)
{
    DMA2_LIFCR = (DMA_LIFCR_TCIF0 | DMA_LIFCR_HTIF0);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_LIFCR &= ~(DMA_LIFCR_TCIF0 | DMA_LIFCR_HTIF0);
#endif
    _DMA_Handler(0);
}

static void _DMA_Interrupt_1(void)
{
    DMA2_LIFCR = (DMA_LIFCR_TCIF1 | DMA_LIFCR_HTIF1);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_LIFCR &= ~(DMA_LIFCR_TCIF1 | DMA_LIFCR_HTIF1);
#endif
    _DMA_Handler(1);
}

static void _DMA_Interrupt_2(void)
{
    DMA2_LIFCR = (DMA_LIFCR_TCIF2 | DMA_LIFCR_HTIF2);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_LIFCR &= ~(DMA_LIFCR_TCIF2 | DMA_LIFCR_HTIF2);
#endif
    _DMA_Handler(2);
}

static void _DMA_Interrupt_3(void)
{
    DMA2_LIFCR = (DMA_LIFCR_TCIF3 | DMA_LIFCR_HTIF3);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_LIFCR &= ~(DMA_LIFCR_TCIF3 | DMA_LIFCR_HTIF3);
#endif
    _DMA_Handler(3);
}

static void _DMA_Interrupt_4(void)
{
    DMA2_HIFCR = (DMA_HIFCR_TCIF4 | DMA_HIFCR_HTIF4);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_HIFCR &= ~(DMA_HIFCR_TCIF4 | DMA_HIFCR_HTIF4);
#endif
    _DMA_Handler(4);
}

static void _DMA_Interrupt_5(void)
{
    DMA2_HIFCR = (DMA_HIFCR_TCIF5 | DMA_HIFCR_HTIF5);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_HIFCR &= ~(DMA_HIFCR_TCIF5 | DMA_HIFCR_HTIF5);
#endif
    _DMA_Handler(5);
}

static void _DMA_Interrupt_6(void)
{
    DMA2_HIFCR = (DMA_HIFCR_TCIF6 | DMA_HIFCR_HTIF6);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_HIFCR &= ~(DMA_HIFCR_TCIF6 | DMA_HIFCR_HTIF6);
#endif
    _DMA_Handler(6);
}

static void _DMA_Interrupt_7(void)
{
    DMA2_HIFCR = (DMA_HIFCR_TCIF7 | DMA_HIFCR_HTIF7);                    // clear the interrupt flag (half- or full transfer complete)
#if defined _WINDOWS
    DMA2_HIFCR &= ~(DMA_HIFCR_TCIF7 | DMA_HIFCR_HTIF7);
#endif
    _DMA_Handler(7);
}
#endif

// ADC global interrupt
//
static __interrupt void _ADC_Interrupt(void)
{
    register unsigned long ulStatus = ADC_CSR;                           // read the ADC global status register   
    if ((ulStatus & ADC_CSR_EOC1) != 0) {                                // end of conversion interrupt on ADC 1
        ADC1_SR &= ~(ADC_SR_EOC | ADC_SR_STRT);                          // clear the interrupt flag
        if ((ADC1_CR1 & ADC_CR1_EOCIE) != 0) {                           // if this interrupt is of interest
            fnEndOfConversion(0, ADC1_DR);                               // handle the interrupt on ADC1
        }
    }
    if ((ulStatus & ADC_CSR_OV1) != 0) {
        ADC1_SR &= ~(ADC_SR_OVR | ADC_SR_STRT);                          // clear the overrun interrupt flag
        ADC1_CR2 |= ADC_CR2_SWSTART;                                     // re-start conversion
    }
    if ((ulStatus & ADC_CSR_AWD1) != 0) {                                // analog watchog interrupt fired
        if ((ADC1_CR1 & ADC_CR1_AWDIE) != 0) {                           // if this interrupt is of interest
            ADC1_SR &= ~(ADC_SR_AWD | ADC_SR_STRT);                      // clear the analog watchdog interrupt flag
            uDisable_Interrupt();
                _ADC_Interrupt_handler[0](&adc_result[0]);               // call handling function to inform that the analog watchdog  event took place
            uEnable_Interrupt();
        }
    }
#if ADC_CONTROLLERS > 1
    if ((ulStatus & ADC_CSR_EOC2) != 0) {                                // end of conversion interrupt on ADC 2
        ADC2_SR &= ~(ADC_SR_EOC | ADC_SR_STRT);                          // clear the interrupt flag
        if ((ADC2_CR1 & ADC_CR1_EOCIE) != 0) {                           // if this interrupt is of interest
            fnEndOfConversion(1, ADC2_DR);                               // handle the interrupt on ADC2
        }
    }
    if ((ulStatus & ADC_CSR_OV2) != 0) {
        ADC2_SR &= ~(ADC_SR_OVR | ADC_SR_STRT);                          // clear the overrun interrupt flag
        ADC2_CR2 |= ADC_CR2_SWSTART;                                     // re-start conversion
    }
#endif
#if ADC_CONTROLLERS > 2
    if ((ulStatus & ADC_CSR_EOC3) != 0) {                                // end of conversion interrupt on ADC 3
        ADC3_SR &= ~(ADC_SR_EOC | ADC_SR_STRT);                          // clear the interrupt flag
        if ((ADC3_CR1 & ADC_CR1_EOCIE) != 0) {                           // if this interrupt is of interest
            fnEndOfConversion(2, ADC2_DR);                               // handle the interrupt on ADC3
        }
    }
    if ((ulStatus & ADC_CSR_OV3) != 0) {
        ADC3_SR &= ~(ADC_SR_OVR | ADC_SR_STRT);                          // clear the overrun interrupt flag
        ADC3_CR2 |= ADC_CR2_SWSTART;                                     // re-start conversion
    }
#endif
}

// Temporarily here for development purposes - to be removed to own file
//
extern void fnConfigDMA_buffer(unsigned char ucDMA_channel, unsigned char ucDmaTriggerSource, unsigned long ulBufLength, void *ptrBufSource, void *ptrBufDest, unsigned long ulRules, void(*int_handler)(void), int int_priority)
{
    // Only peripheral -> memory implemented
    //
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    // DMA2 is used (channel 0)
    //
    DMA2_S0CR &= ~DMA_SxCR_EN;                                           // initially disabled
    while ((DMA2_S0CR & DMA_SxCR_EN) != 0) {                             // wait until the channel completes disabling
    }
    DMA2_S0CR = 0;                                                       // clear register once any previous activity has stopped
    DMA2_S0PAR = (unsigned long)ptrBufSource;                            // set the peripheral port that will be read from (the controller needs to be disabled for this register to be written to)
    DMA2_S0M0AR = (unsigned long)ptrBufDest;                             // set the memory address which will be written to
    DMA2_S0NDTR = ulBufLength;                                           // the number of transfers (decremented after each trigger)
    DMA2_S0CR |= ((ucDmaTriggerSource << 25) | /*DMA_SxCR_PFCTRL |*/ (DMA_SxCR_PL_VERY_HIGH)); // set the channel and DMA priority (make priority configurable?)(not that periphera flow control is only suppoted by the SDIO)
    // no FIFO configured (?)
    DMA2_S0FCR |= DMA_SxFCR_DMDIS;                                       // enable direct mode when source and destination sizes differ
    DMA2_S0CR |= (DMA_SxCR_DIR_P2M | DMA_SxCR_MINC | DMA_SxCR_PSIZE_32 | DMA_SxCR_MSIZE_16 | DMA_SxCR_PBURST_SINGLE | DMA_SxCR_MBURST_SINGLE); // peripheral to memory transfer with memory incrementing - 32 bits read and 16 saved
    if (int_handler != 0) {                                              // if there is a buffer interrupt handler
        _DMA_handler[0] = int_handler;                                   // enter the user's handler
        fnEnterInterrupt((irq_DMA2_Channel1_ID + 0), int_priority, (void(*)(void))_DMA_Interrupt[0]); // enter DMA interrupt handler on buffer completion
        DMA2_S0CR |= DMA_SxCR_TCIE;                                      // enable an interrupt on transfer completion
    }
    #else
    _EXCEPTION("Not supported!");
    #endif
}

extern void fnDMA_BufferReset(int iChannel, int iAction)
{
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    // DMA2 is used (channel 0)
    //
    #if defined _WINDOWS
    static unsigned short usLastTransferCount = 0;
    static unsigned long ulLastPeripheral = 0;
    static unsigned long ulLastMemory = 0;
    #endif
    switch (iAction) {
    case DMA_BUFFER_START:
        DMA2_S0CR |= DMA_SxCR_EN;                                        // enable the stream
    #if defined _WINDOWS
        if (DMA2_S0NDTR == 0) {                                          // if the last transfer completed
            DMA2_S0NDTR = usLastTransferCount;                           // reload its value to mimic the behavior of the DMA controller when re-starting
            DMA2_S0PAR = ulLastPeripheral;
            DMA2_S0M0AR = ulLastMemory;
        }
        else {
            usLastTransferCount = (unsigned short)DMA2_S0NDTR;           // backup the transfer count in case it needs to be re-loaded
            ulLastPeripheral = DMA2_S0PAR;
            ulLastMemory = DMA2_S0M0AR;
        }
    #endif
        break;
    case DMA_BUFFER_RESET:                                               // reset the DMA back to the start of the present buffer
    case DMA_BUFFER_RESTART:                                             // reset and start again
        _EXCEPTION("To do!");
        break;
    }
    #else
    _EXCEPTION("Not supported!");
    #endif
}
#endif

/* =================================================================== */
/*                         ADC Configuration                           */
/* =================================================================== */

#if defined _ADC_CONFIG_CODE
        {
            ADC_SETUP *ptrADC_settings = (ADC_SETUP *)ptrSettings; 
            STM32_ADC_REGS *ptrADC;
            register int iADC_controller = ptrADC_settings->int_adc_controller;
            register int iADC_index = (iADC_controller - 1);
            unsigned char ucADC_channel = ptrADC_settings->int_adc_bit;  // the channel to be configured
            if ((ADC_DISABLE_ADC & ptrADC_settings->int_adc_mode) != 0) {// disable the ADC
                if (iADC_controller == 1) {
                    ADC1_CR2 = 0;                                        // turn off the ADC
                    POWER_DOWN(APB2, RCC_APB2ENR_ADC1EN);                // disable clocks to ADC 1
                }
    #if ADC_CONTROLLERS > 1
                else if (iADC_controller == 2) {
                    ADC2_CR2 = 0;                                        // turn off the ADC
                    POWER_DOWN(APB2, RCC_APB2ENR_ADC2EN);                // disable clocks to ADC 2
                }
        #if ADC_CONTROLLERS > 2
                else if (iADC_controller == 3) {
                    ADC3_CR2 = 0;                                        // turn off the ADC
                    POWER_DOWN(APB2, RCC_APB2ENR_ADC3EN);                // disable clocks to ADC 3
                }
        #endif
    #endif
    #if defined _WINDOWS
                else {
                    _EXCEPTION("ADC controller doesn't exist!!");
                }
    #endif
                return;
            }
            if (iADC_controller == 1) {                                  // the ADC controller
                ptrADC = (STM32_ADC_REGS *)ADC_BLOCK;                    // ADC 1
            }
    #if ADC_CONTROLLERS > 1
            else if (iADC_controller == 2) {
                ptrADC = (STM32_ADC_REGS *)(ADC_BLOCK + 0x100);          // ADC 2
            }
    #endif
    #if ADC_CONTROLLERS > 2
            else if (iADC_controller == 3) {
                ptrADC = (STM32_ADC_REGS *)(ADC_BLOCK + 0x200);          // ADC 3
            }
    #endif
            else {
    #if defined _WINDOWS
                _EXCEPTION("ADC controller doesn't exist!!");
    #endif
                return;
            }
            if ((ptrADC_settings->int_adc_mode & ADC_READ_ONLY) == 0) {  // if not being called only for read
                if ((ptrADC_settings->int_adc_mode & ADC_CONFIGURE_ADC) != 0) { // main configuration is to be performed
                    if (iADC_controller == 1) {                          // ADC1
                        POWER_UP(APB2, RCC_APB2ENR_ADC1EN);              // enable clocks to ADC 1
                        adc_result[0].ucADC_channel = 1;
                    }
    #if ADC_CONTROLLERS > 1
                    else if (iADC_controller == 2) {
                        POWER_UP(APB2, RCC_APB2ENR_ADC2EN);              // enable clocks to ADC 2
                        adc_result[1].ucADC_channel = 2;
                    }
        #if ADC_CONTROLLERS > 2
                    else if (iADC_controller == 3) {
                        POWER_UP(APB2, RCC_APB2ENR_ADC3EN);              // enable clocks to ADC 3
                        adc_result[2].ucADC_channel = 3;
                    }
        #endif
    #endif
                    ptrADC->ADC_CR1 = (ptrADC_settings->int_adc_mode & ADC_CR1_RES_6_BIT); // set the resolution
                    ADC_CCR = (ptrADC_settings->int_adc_mode & ADC_CLOCK_PCLK_DIV_8); // set ADC clock speed (valid for all ADC controllers)
                    ptrADC->ADC_CR2 = (ADC_CR2_ADON | ADC_CR2_EOCS_CONVERSION | ((ADC_CR2_ALIGN_LEFT | ADC_FULL_BUFFER_DMA) & ptrADC_settings->int_adc_mode)); // turn ADC on with the end of conversion signal at each conversion and set alignment and DMA mode (when set for the first time this wakes the ADC from its power-down mode)
                    if ((ptrADC_settings->int_adc_mode & ADC_LOOP_MODE) != 0) { // if continuous conversion is required
                        ptrADC->ADC_CR2 |= ADC_CR2_CONT;                 // continuous conversion
                    }
                    else {
                      //ptrADC->ADC_CR2 &= ~ADC_CR2_CONT;                // single conversion
                    }
                }
                if ((ptrADC_settings->int_adc_mode & ADC_CONFIGURE_CHANNEL) != 0) { // if a channel is to be configured
                    // Configure the input analog pin
                    //
                    if (ucADC_channel > 15) {                            // internal sources
                        if (iADC_controller != 1) {                      // only available on ADC1
                            _EXCEPTION("Internal ADC sources are only available on ADC1!");
                        }
                        switch (ucADC_channel) {
                        case ADC1_TEMPERATURE_SENSOR:
    #if defined _STM32F42X || defined _STM32F43X
                            ucADC_channel = ADC1_TEMPERATURE_SENSOR_;    // map the channel number to the one used by this processor type
    #endif
                            // Fall through intentionally
                            //
                        case ADC1_VREFINT:
                            ADC_CCR |= ADC_CSR_CCR_TSVREFE;      // power up the temperature sensor/VREFINT
                            break;
                        case ADC1_VBAT:
                            ADC_CCR |= ADC_CSR_CCR_VBATE;        // select the battery voltage (VBAT/2 or VBAT/4, depending on chip type)
                            break;
                        }
    #if defined _STM32F42X || defined _STM32F43X
                        if ((ADC_CCR & (ADC_CSR_CCR_VBATE | ADC_CSR_CCR_TSVREFE)) == (ADC_CSR_CCR_VBATE | ADC_CSR_CCR_TSVREFE)) {
                            _EXCEPTION("Attempting to use VBAT and temperate sensor t the same time - only VBAT will be performed!");
                        }
    #endif
                    }
                    else {                                               // external pins
                        int iShared = 0;
                        switch (iADC_controller) {                       // configure the referenced external input pin to its analog mode
                        case 3:
                            switch (ucADC_channel) {
                            case 4:                                      // ADC3_IN4
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT6);
                                break;
                            case 5:                                      // ADC3_IN5
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT7);
                                break;
                            case 6:                                      // ADC3_IN6
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT8);
                                break;
                            case 7:                                      // ADC3_IN7
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT9);
                                break;
                            case 8:                                      // ADC3_IN8
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT10);
                                break;
                            case 9:                                      // ADC3_IN9
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT3);
                                break;
                            case 14:                                     // ADC3_IN14
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT4);
                                break;
                            case 15:                                     // ADC3_IN15
                                _CONFIG_ANALOG_INPUT(F, PORTF_BIT5);
                                break;
                            default:
                                iShared = 1;                             // fall-through to share inputs 0,1,2,3,10,11,12 and 13 with ADC1/ADC2
                                break;
                            }
                            if (iShared == 0) {
                                break;
                            }
                        case 1:                                          // ADC 1
                        case 2:                                          // ADC 2
                            switch (ucADC_channel) {
                            case 0:                                      // ADC123_IN0
                                _CONFIG_ANALOG_INPUT(A, PORTA_BIT0);
                                break;
                            case 1:                                      // ADC123_IN1
                                _CONFIG_ANALOG_INPUT(A, PORTA_BIT1);
                                break;
                            case 2:                                      // ADC123_IN2
_CONFIG_ANALOG_INPUT(A, PORTA_BIT2);
break;
                            case 3:                                      // ADC123_IN3
                                _CONFIG_ANALOG_INPUT(A, PORTA_BIT3);
                                break;
                            case 4:                                      // ADC12_IN4
                                _CONFIG_ANALOG_INPUT(A, PORTA_BIT4);
                                break;
                            case 5:                                      // ADC12_IN5
                                _CONFIG_ANALOG_INPUT(A, PORTA_BIT5);
                                break;
                            case 6:                                      // ADC12_IN6
                                _CONFIG_ANALOG_INPUT(A, PORTA_BIT6);
                                break;
                            case 7:                                      // ADC12_IN7
                                _CONFIG_ANALOG_INPUT(A, PORTA_BIT7);
                                break;
                            case 8:                                      // ADC12_IN8
                                _CONFIG_ANALOG_INPUT(B, PORTB_BIT0);
                                break;
                            case 9:                                      // ADC12_IN9
                                _CONFIG_ANALOG_INPUT(B, PORTB_BIT1);
                                break;
                            case 10:                                     // ADC123_IN10
                                _CONFIG_ANALOG_INPUT(C, PORTC_BIT0);
                                break;
                            case 11:                                     // ADC123_IN11
                                _CONFIG_ANALOG_INPUT(C, PORTC_BIT1);
                                break;
                            case 12:                                     // ADC123_IN12
                                _CONFIG_ANALOG_INPUT(C, PORTC_BIT2);
                                break;
                            case 13:                                     // ADC123_IN13
                                _CONFIG_ANALOG_INPUT(C, PORTC_BIT3);
                                break;
                            case 14:                                     // ADC12_IN14
                                _CONFIG_ANALOG_INPUT(C, PORTC_BIT4);
                                break;
                            case 15:                                     // ADC12_IN15
                                _CONFIG_ANALOG_INPUT(C, PORTC_BIT5);
                                break;
                            default:
                                _EXCEPTION("Invalid ADC channel!");
                                break;
                            }
                            break;
                        }
                    }
                    if ((ptrADC_settings->int_adc_mode & ADC_SEQUENTIAL_MODE) != 0) { // add the channel to a sequence
                        unsigned long ulSequenceNumber = ((ptrADC->ADC_SQR1 & ADC_SQR1_L_MASK) >> ADC_SQR1_L_SHIFT); // the present sequence number
                        ++ulSequenceNumber;                              // increment the sequence number
#if defined _WINDOWS
                        if (ulSequenceNumber > 15) {
                            _EXCEPTION("Sequence length error");
                        }
#endif
                        ptrADC->ADC_SQR1 &= ~(ADC_SQR1_L_MASK);          // mask out the length
                        if (ulSequenceNumber >= 12) {
                            unsigned long ulNextSequenceNumber = (ulSequenceNumber << ADC_SQR1_L_SHIFT);
                            ptrADC->ADC_SQR1 &= ~(0x1f << ((ulSequenceNumber - 12) * 5)); // clear the channel location
                            ulSequenceNumber = (ptrADC->ADC_SQR1 | (ucADC_channel << ((ulSequenceNumber - 12) * 5))); // insert channel reference
                            ulSequenceNumber |= ulNextSequenceNumber;
                        }
                        else if (ulSequenceNumber >= 6) {
                            ptrADC->ADC_SQR2 &= ~(0x1f << ((ulSequenceNumber - 6) * 5)); // clear the channel location
                            ptrADC->ADC_SQR2 |= (ucADC_channel << ((ulSequenceNumber - 6) * 5)); // insert channel reference
                            ulSequenceNumber <<= ADC_SQR1_L_SHIFT;
                        }
                        else {
                            ptrADC->ADC_SQR3 &= ~(0x1f << (ulSequenceNumber * 5)); // clear the channel location
                            ptrADC->ADC_SQR3 |= (ucADC_channel << (ulSequenceNumber * 5)); // insert channel reference
                            ulSequenceNumber <<= ADC_SQR1_L_SHIFT;
                            ptrADC->ADC_CR1 |= (ADC_CR1_SCAN);           // enable scan mode
                        }
                        ptrADC->ADC_SQR1 |= ulSequenceNumber;            // insert the new sequence length
                    }
                    else {
                        ptrADC->ADC_CR1 &= ~(ADC_CR1_SCAN);              // not scan mode when only one input
                        ptrADC->ADC_SQR1 = 0;                            // single input (reset previous sequence)
                        ptrADC->ADC_SQR3 = ucADC_channel;                // set the channel for the single conversion
                    }
                    // Set the sample time for the channel
                    //
                    if (ucADC_channel >= 10) {
                        ptrADC->ADC_SMPR1 &= ~(ADC_SMPR1_SMP10_MASK << (3 * (ucADC_channel - 10)));
                        ptrADC->ADC_SMPR1 |= ((ptrADC_settings->int_adc_mode & ADC_SMPR1_SMP10_MASK) << (3 * (ucADC_channel - 10)));
                    }
                    else {
                        ptrADC->ADC_SMPR2 &= ~(ADC_SMPR2_SMP0_MASK << (3 * ucADC_channel));
                        ptrADC->ADC_SMPR2 |= ((ptrADC_settings->int_adc_mode & ADC_SMPR2_SMP0_MASK) << (3 * ucADC_channel));
                    }
                    if (ptrADC_settings->int_handler != 0) {             // if user call-back handler entered
                        _ADC_Interrupt_handler[iADC_index] = ptrADC_settings->int_handler; // enter the user interrupt handler function
                        fnEnterInterrupt(irq_ADC_ID, ptrADC_settings->int_priority, (void(*)(void))_ADC_Interrupt); // enter the global ADC interrupt handler
                        ptrADC->ADC_CR1 |= (ADC_CR1_EOCIE | ADC_CR1_OVRIE); // enable interrupt on end of each conversion (and overrun)
                        if ((ptrADC_settings->int_adc_int_type & (ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT)) != 0) { // if an analog watchog function is defined
                            if ((ptrADC_settings->int_adc_int_type & (ADC_LOW_LIMIT_INT)) != 0) {
                                ptrADC->ADC_LTR = ptrADC_settings->int_low_level_trigger; // set the lower threshold (12 bit right aligned)
                            }
                            else {
                                ptrADC->ADC_LTR = 0;
                            }
                            if ((ptrADC_settings->int_adc_int_type & (ADC_HIGH_LIMIT_INT)) != 0) {
                                ptrADC->ADC_HTR = ptrADC_settings->int_high_level_trigger; // set the upper threshold (12 bit right aligned)
                            }
                            else {
                                ptrADC->ADC_HTR = 0;
                            }
                            ptrADC->ADC_CR1 &= ~(ADC_CR1_AWDCH_MASK | ADC_CR1_AWDSGL_SINGLE);
                            if (ptrADC_settings->int_watchdog_bit != ADC_ANALOG_WATCHDOG_ALL) {
                                ptrADC->ADC_CR1 |= ((ptrADC_settings->int_watchdog_bit) | ADC_CR1_AWDSGL_SINGLE); // monitor single regular channel
                            }
                            ptrADC->ADC_CR1 |= (ADC_CR1_AWDEN | ADC_CR1_AWDIE); // enable the analog watchdog function and interrupt
                        }
                    }
                    if ((ptrADC->ADC_CR2 & ADC_CR2_DMA) != 0) {          // DMA mode being used
                        ptrADC->ADC_CR2 &= ~(ADC_CR2_EOCS_CONVERSION);   // interrupt at end of complete sequence only
                        fnConfigDMA_buffer(2, 0, (((ptrADC->ADC_SQR1 >> ADC_SQR1_L_SHIFT) & 0x01f) + 1), (void *)(ADC1_DR_ADD + (0x100 * (iADC_index))), adc_result[iADC_index].usADC_value, 0, fnEndOfConversionDMA1, 0);
                    }
                }
                if ((ADC_START_OPERATION & ptrADC_settings->int_adc_mode) != 0) {
                    adc_result[iADC_index].ucSample = 0;
                    adc_result[iADC_index].ucSequenceLength = ((ptrADC->ADC_SQR1 >> ADC_SQR1_L_SHIFT) & 0x01f); // the sequence length
                    if ((ptrADC->ADC_CR2 & ADC_CR2_DMA) != 0) {          // DMA mode being used
                        ptrADC->ADC_CR2 &= ~(ADC_CR2_DMA);               // toggle the DMA mode control bit so that further DMA operations are executed
                        ptrADC->ADC_CR2 |= (ADC_CR2_DMA);
                        fnDMA_BufferReset(2, DMA_BUFFER_START);
                    }
                    ptrADC->ADC_CR2 |= ADC_CR2_SWSTART;                  // start conversion
                }
            }
            if (((ptrADC_settings->int_adc_mode & (ADC_GET_RESULT)) != 0) && (ptrADC_settings->int_adc_result != 0)) { // if there is a result structure
                int i;
                for (i = 0; i < ADC_CHANNELS; i++) {
                    ptrADC_settings->int_adc_result->usADC_value[i] = (unsigned short)adc_result[iADC_index].usADC_value[i];
                }
              //uMemcpy(ptrADC_settings->int_adc_result->usADC_value, adc_result[iADC_index].usADC_value, sizeof(ptrADC_settings->int_adc_result->usADC_value)); // temporarily removed until buffers can be made of same types
                uMemcpy(ptrADC_settings->int_adc_result->ucADC_status, adc_result[iADC_index].ucADC_status, sizeof(ptrADC_settings->int_adc_result->ucADC_status));
            }
        }
#endif
