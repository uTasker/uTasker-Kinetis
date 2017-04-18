/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_ADC.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    03.06.2013 Add ADC result to interrupt call-back                     {41}
    30.09.2013 Add ADC A/B input multiplexer control                     {54}
    27.10.2013 Add ADC DMA configuration                                 {57}
    13.05.2015 Add low/high threshold single shot interrupt mode         {1}
    04.12.2015 Add Kinetis KE ADC mode                                   {2}
    23.12.2015 Add automatic ADC DMA buffer repetition                   {3}
    04.01.2016 Allow free-run ADC with DMA                               {4}
    06.03.2017 Allow alternative DMA trigger sources                     {5}

*/

#if defined _ADC_INTERRUPT_CODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _ADC_Interrupt_0(void);
    #if ADC_CONTROLLERS > 1
static __interrupt void _ADC_Interrupt_1(void);
    #endif
    #if ADC_CONTROLLERS > 2
static __interrupt void _ADC_Interrupt_2(void);
    #endif
    #if ADC_CONTROLLERS > 3
static __interrupt void _ADC_Interrupt_3(void);
    #endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const unsigned char *_ADC_Interrupt[ADC_CONTROLLERS] = {
    (unsigned char *)_ADC_Interrupt_0,
    #if ADC_CONTROLLERS > 1
    (unsigned char *)_ADC_Interrupt_1,
    #endif
    #if ADC_CONTROLLERS > 2
    (unsigned char *)_ADC_Interrupt_2,
    #endif
    #if ADC_CONTROLLERS > 3
    (unsigned char *)_ADC_Interrupt_3
    #endif
};


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*_ADC_Interrupt_handler[ADC_CONTROLLERS])(ADC_INTERRUPT_RESULT *) = {0}; // user ADC interrupt handlers


/* =================================================================== */
/*                        ADC Interrupt Handlers                       */
/* =================================================================== */

// ADC 0 interrupt
//
static __interrupt void _ADC_Interrupt_0(void)
{
    ADC0_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[0] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC0_RA;
        adc_result.ucADC_channel = 0;                                    // channel is not added
        adc_result.ucADC_flags = ADC_RESULT_VALID;                       // assumed that the result is valid
        uDisable_Interrupt();
            _ADC_Interrupt_handler[0](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}

    #if ADC_CONTROLLERS > 1
// ADC 1 interrupt
//
static __interrupt void _ADC_Interrupt_1(void)
{
    ADC1_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[1] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC1_RA;
        adc_result.ucADC_channel = 0;
        adc_result.ucADC_flags = ADC_RESULT_VALID;
        uDisable_Interrupt();
            _ADC_Interrupt_handler[1](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}
    #endif
    #if ADC_CONTROLLERS > 2
// ADC 2 interrupt
//
static __interrupt void _ADC_Interrupt_2(void)
{
    ADC2_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[2] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC2_RA;
        adc_result.ucADC_channel = 0;
        adc_result.ucADC_flags = ADC_RESULT_VALID;
        uDisable_Interrupt();
            _ADC_Interrupt_handler[2](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}
    #endif
    #if ADC_CONTROLLERS > 3
// ADC 3 interrupt
//
static __interrupt void _ADC_Interrupt_3(void)
{
    ADC3_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[3] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC3_RA;
        adc_result.ucADC_channel = 0;
        adc_result.ucADC_flags = ADC_RESULT_VALID;
        uDisable_Interrupt();
            _ADC_Interrupt_handler[3](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}
    #endif

    #if !defined KINETIS_KE
static unsigned char fnSetADC_channel(unsigned char ucADC_channel, int iDiffMode)
{
        #if defined _WINDOWS
    if ((ucADC_channel == 0x1c) || (ucADC_channel > ADC_SC1A_ADCH_OFF) || ((ucADC_channel > ADC_SC1A_ADCH_23) && (ucADC_channel < ADC_SC1A_ADCH_TEMP_SENS))) {
        _EXCEPTION("Selecting invalid ADC channel!");                    // invalid channels                                     
    }
        #endif
    if ((iDiffMode != 0) && ((ucADC_channel <= ADC_SC1A_ADCH_D3) || ((ucADC_channel >= ADC_SC1A_ADCH_TEMP_SENS) && (ucADC_channel <= ADC_SC1A_ADCH_VREFSH)))) { // set up channel in differential mode (channels 0..3, temperature, bandgap and VREFSH)
        return (ucADC_channel | ADC_SC1A_DIFF);
    }
    else {                                                               // single ended (channels 0..23, temperature, bandgap and VREFSH and VREFSL)
        switch (ucADC_channel) {
        case ADC_SE23_SINGLE:
        #if defined KINETIS_KL43
            _CONFIG_PERIPHERAL(E, 30, PE_30_ADC0_SE23);                  // ensure that the ADC pin is configured
        #endif
            break;
        case ADC_SE4_SINGLE:
        #if defined KINETIS_KL43
            _CONFIG_PERIPHERAL(E, 21, PE_21_ADC0_SE4);                   // ensure that the ADC pin is configured
        #endif
            break;
        #if defined KINETIS_KL27
        case ADC_SE8_SINGLE:
            _CONFIG_PERIPHERAL(B, 0, PB_0_ADC0_SE8);                     // ensure that the ADC pin is configured
            break;
        #endif
        }
        return (ucADC_channel);
    }
}
    #endif

// Convert a standard 16 bit value to the present ADC mode format
//
static unsigned short fnConvertADCvalue(KINETIS_ADC_REGS *ptrADC, unsigned short usStandardValue, int iPlusOne) // {1}
{
    #if defined KINETIS_KE                                               // {2}
    switch (ptrADC->ADC_SC3 & (ADC_CFG1_MODE_12 | ADC_CFG1_MODE_10)) {
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x0fff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x03ff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x00ff) {
                usStandardValue++;
            }
        }
        break;
    }
    #else
    switch (ptrADC->ADC_CFG1 & ADC_CFG1_MODE_16) {
    case ADC_CFG1_MODE_16:                                               // conversion mode - single-ended 16 bit or differential 16 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0xffff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit or differential 13 bit
        usStandardValue >>= 4;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0800) {
                usStandardValue |= 0xf000;                               // sign extend
            }
        }
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x0fff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit or differential 11 bit
        usStandardValue >>= 6;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0200) {
                usStandardValue |= 0xfc00;                               // sign extend
            }
        }
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x03ff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit or differential 9 bit
        usStandardValue >>= 8;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0080) {
                usStandardValue |= 0xff00;                               // sign extend
            }
        }
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x00ff) {
                usStandardValue++;
            }
        }
        break;
    }
    #endif
    return usStandardValue;
}
#endif

/* =================================================================== */
/*                         ADC Configuration                           */
/* =================================================================== */

#if defined _ADC_CONFIG_CODE
        {
            ADC_SETUP *ptrADC_settings = (ADC_SETUP *)ptrSettings; 
            KINETIS_ADC_REGS *ptrADC;
            int irq_ADC_ID;
    #if !defined KINETIS_KE
            unsigned char ucADC_channel = ptrADC_settings->int_adc_bit;  // the channel to be configured
    #endif
            if ((ADC_DISABLE_ADC & ptrADC_settings->int_adc_mode) != 0) {
                if (ptrADC_settings->int_adc_controller == 0) {
                    POWER_DOWN(6, SIM_SCGC6_ADC0);                       // disable clocks to module
                }
    #if ADC_CONTROLLERS > 1
                else if (ptrADC_settings->int_adc_controller == 1) {
                    POWER_DOWN(3, SIM_SCGC3_ADC1);
                }
        #if ADC_CONTROLLERS > 2
                else if (ptrADC_settings->int_adc_controller == 2) {
                    POWER_DOWN(6, SIM_SCGC6_ADC2);
                }
        #endif
        #if ADC_CONTROLLERS > 3
                else if (ptrADC_settings->int_adc_controller == 3) {
                    POWER_DOWN(3, SIM_SCGC3_ADC3);
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
            if (ptrADC_settings->int_adc_controller == 0) {
                irq_ADC_ID = irq_ADC0_ID;
                ptrADC = (KINETIS_ADC_REGS *)ADC0_BLOCK;
            }
    #if ADC_CONTROLLERS > 1
            else if (ptrADC_settings->int_adc_controller == 1) {
                irq_ADC_ID = irq_ADC1_ID;
                ptrADC = (KINETIS_ADC_REGS *)ADC1_BLOCK;
            }
    #endif
    #if ADC_CONTROLLERS > 2
            else if (ptrADC_settings->int_adc_controller == 2) {
                irq_ADC_ID = irq_ADC2_ID;
                ptrADC = (KINETIS_ADC_REGS *)ADC2_BLOCK;
            }
    #endif
    #if ADC_CONTROLLERS > 3
            else if (ptrADC_settings->int_adc_controller == 3) {
                irq_ADC_ID = irq_ADC3_ID;
                ptrADC = (KINETIS_ADC_REGS *)ADC3_BLOCK;
            }
    #endif
            else {
    #if defined _WINDOWS
                _EXCEPTION("ADC controller doesn't exist!!");
    #endif
                return;
            }
            if ((ptrADC_settings->int_adc_mode & ADC_READ_ONLY) == 0) {  // if not being called only for read
                unsigned char ucChannelConfig = 0;
                if ((ptrADC_settings->int_adc_mode & ADC_CONFIGURE_ADC) != 0) { // main configuration is to be performed
                    if (ptrADC_settings->int_adc_controller == 0) {      // ADC0
                        POWER_UP(6, SIM_SCGC6_ADC0);                     // enable clocks to module
                    }
    #if ADC_CONTROLLERS > 1
                    else if (ptrADC_settings->int_adc_controller == 1) {
                        POWER_UP(3, SIM_SCGC3_ADC1);                     // enable clocks to module
                    }
        #if ADC_CONTROLLERS > 2
                    else if (ptrADC_settings->int_adc_controller == 2) {
                        POWER_UP(6, SIM_SCGC6_ADC2);                     // enable clocks to module
                    }
            #if ADC_CONTROLLERS > 3
                    else if (ptrADC_settings->int_adc_controller == 3) {
                        POWER_UP(3, SIM_SCGC3_ADC3);                     // enable clocks to module
                    }
            #endif
        #endif
    #endif
                    ptrADC->ADC_CFG1 = (0xff & ptrADC_settings->int_adc_mode);
    #if defined _WINDOWS
                    // Check that the ADC frequency is set to a valid range
                    //
                    if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADICLK_ASY) <= ADC_CFG1_ADICLK_BUS2) { // clock derived from the bus clock
                        unsigned long ulADC_clock = BUS_CLOCK;
                        if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADICLK_BUS2) != 0) {
                            ulADC_clock /= 2;
                        }
                        ulADC_clock >>= ((ADC_CFG1_ADIV_8 & ptrADC->ADC_CFG1) >> 5);
                        switch (ptrADC->ADC_CFG1 & ADC_CFG1_MODE_MASK) {
                        case ADC_CFG1_MODE_8:
                        case ADC_CFG1_MODE_10:
                        case ADC_CFG1_MODE_12:
        #if defined KINETIS_KE
                            if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADLPC) != 0) { // low power mode used when high sampling speeds are not required
                                if ((ulADC_clock < 400000) || (ulADC_clock > 4000000)) { // check valid ADC clock rate in low power mode
                                    _EXCEPTION("ADC clock rate outside valid range 400kHz..4MHz");
                                }
                            }
                            else {
                                if ((ulADC_clock < 400000) || (ulADC_clock > 8000000)) { // check valid ADC clock rate in high speed mode
                                    _EXCEPTION("ADC clock rate outside valid range 400kHz..8MHz");
                                }
                            }
        #else
                            if ((ulADC_clock < 1000000) || (ulADC_clock > 18000000)) { // check valid ADC clock rate
                                _EXCEPTION("ADC clock rate outside valid range 1MHz..18MHz for modes <= 13 bits");
                            }
        #endif
                            break;
        #if defined KINETIS_KE
                        default:
                            _EXCEPTION("Invalid ADC resolution!!");
                            break;
        #else
                        case ADC_CFG1_MODE_16:
                            if ((ulADC_clock < 2000000) || (ulADC_clock > 12000000)) { // check valid ADC clock rate
                                _EXCEPTION("ADC clock rate outside valid range 2MHz..12MHz for 16 bit mode");
                            }
                            break;
        #endif
                        }
                    }
    #endif
    #if !defined KINETIS_KE
                    if ((ptrADC_settings->int_adc_mode & ADC_SELECT_INPUTS_B) != 0) { // {54}
                        ptrADC->ADC_CFG2 = (ADC_CFG2_MUXSEL_B | (ptrADC_settings->int_adc_sample & 0x7)); // select mux B inputs
                    }
                    else {
                        ptrADC->ADC_CFG2 = (ADC_CFG2_MUXSEL_A | (ptrADC_settings->int_adc_sample & 0x7)); // select mux A inputs
                    }
                    ptrADC->ADC_SC2 = ((ptrADC_settings->int_adc_mode >> 8) & (ADC_SC2_REFSEL_ALT)); // configure the reference voltage used
                    if ((ADC_CALIBRATE & ptrADC_settings->int_adc_mode) != 0) { // calibration which should be performed once after a reset to achieve optimal accuracy
                        ptrADC->ADC_SC3 = (ADC_SC3_AVGS_32 | ADC_SC3_AVGE | ADC_SC3_CAL); // continuous conversion mode with hardware averaging during calibration
                        while ((ptrADC->ADC_SC1A & ADC_SC1A_COCO) == 0) {// wait for calibration to complete
        #if defined _WINDOWS
                            ptrADC->ADC_SC1A |= ADC_SC1A_COCO;
        #endif
                        }                                                // failure flag not checked since this should never fail
                        // Perform plus side calibration
                        //
                        ptrADC->ADC_PG = (((ptrADC->ADC_CLP0 + ptrADC->ADC_CLP1 + ptrADC->ADC_CLP2 + ptrADC->ADC_CLP3 + ptrADC->ADC_CLP4 + ptrADC->ADC_CLPS)/2) | 0x8000);

                        // Perform minus side calibration
                        //
                        ptrADC->ADC_MG = (((ptrADC->ADC_CLM0 + ptrADC->ADC_CLM1 + ptrADC->ADC_CLM2 + ptrADC->ADC_CLM3 + ptrADC->ADC_CLM4 + ptrADC->ADC_CLMS)/2) | 0x8000);

                        ptrADC->ADC_SC3 &= ~(ADC_SC3_CAL);               // remove calibration enable when calibration has completed
                        ptrADC->ADC_SC1A = (ptrADC->ADC_SC1A & ~(ADC_SC1A_COCO)); // write to SC1A clears the conversion complete flag from calibration
                    }
    #endif
                    ptrADC->ADC_SC2 = ((ptrADC_settings->int_adc_mode >> 8) & (ADC_SC2_REFSEL_ALT | ADC_SC2_ADTRG_HW)); // configure the reference voltage to be used and the trigger mode (hardware or software)
    #if !defined KINETIS_KE
                    ptrADC->ADC_SC3 = ((ptrADC_settings->int_adc_sample >> 4) & 0x07); // configure hardware averaging
    #endif
                    if ((ptrADC_settings->int_adc_mode & ADC_LOOP_MODE) != 0) { // if continuous conversion is required
    #if defined KINETIS_KE
                        ucChannelConfig = ADC_SC1A_ADCO;                 // enable continuous conversion
    #else
                        ptrADC->ADC_SC3 |= ADC_SC3_ADCO;                 // enable continuous conversion
    #endif
                    }
    #if !defined KINETIS_KE
                    else {
                        ptrADC->ADC_SC3 &= ~(ADC_SC3_ADCO);              // disable continuous conversion
                    }
    #endif
                }
                if ((ptrADC_settings->int_adc_mode & ADC_CONFIGURE_CHANNEL) != 0) { // if a channel is to be configured
    #if !defined KINETIS_KE
                    ucChannelConfig = fnSetADC_channel(ucADC_channel, ((ptrADC_settings->int_adc_mode & ADC_DIFFERENTIAL_INPUT) != 0)); // check that the ADC channel is valid and prepare configuration value
                    if ((ptrADC_settings->int_adc_mode & ADC_HW_TRIGGERED) != 0) { // channel B is only valid in hardware triggered mode
                        ptrADC->ADC_SC1B = fnSetADC_channel(ptrADC_settings->int_adc_bit_b, (ptrADC_settings->int_adc_mode & ADC_DIFFERENTIAL_B));
                    }
    #endif
                  //if ((ptrADC_settings->int_adc_mode & ADC_LOOP_MODE) == 0) { // single shot mode {4} removed to allow free-run ADC with DMA
    #if !defined DEVICE_WITHOUT_DMA
                        if ((ptrADC_settings->int_adc_mode & (ADC_FULL_BUFFER_DMA | ADC_HALF_BUFFER_DMA)) != 0) { // {57} if DMA operation is being specified
                            unsigned long *ptrADC_result = (unsigned long *)((unsigned long)ptrADC + 0x010); // ADC channel as result register
                            unsigned long ulDMA_rules = (DMA_DIRECTION_INPUT | DMA_HALF_WORDS);
                            unsigned char ucTriggerSource = ptrADC_settings->ucDmaTriggerSource; // {5}
                            ptrADC->ADC_SC2 |= ADC_SC2_DMAEN;            // enable DMA trigger on ADC conversion end
                            if ((ptrADC_settings->int_adc_mode & ADC_FULL_BUFFER_DMA_AUTO_REPEAT) != 0) {
                                ulDMA_rules |= DMA_AUTOREPEAT;
                            }
                            if ((ptrADC_settings->int_adc_mode & ADC_HALF_BUFFER_DMA) != 0) {
                                ulDMA_rules |= DMA_HALF_BUFFER_INTERRUPT;
                            }
                            if (ucTriggerSource == 0) {                  // {5} if the default is defined
                                ucTriggerSource = (DMAMUX_CHCFG_SOURCE_ADC0 + ptrADC_settings->int_adc_controller);
                            }
                            fnConfigDMA_buffer(ptrADC_settings->ucDmaChannel, ucTriggerSource, ptrADC_settings->ulADC_buffer_length, ptrADC_result, ptrADC_settings->ptrADC_Buffer, ulDMA_rules, ptrADC_settings->dma_int_handler, ptrADC_settings->dma_int_priority); // source is the ADC result register and destination is the ADC buffer
                            fnDMA_BufferReset(ptrADC_settings->ucDmaChannel, DMA_BUFFER_START);
                        }
                        else if ((ptrADC_settings->int_adc_mode & ADC_LOOP_MODE) == 0) { // single shot mode {4}
    #endif
                            if (ptrADC_settings->int_handler != 0) {     // if single-shot conversion handler entered
                                _ADC_Interrupt_handler[ptrADC_settings->int_adc_controller] = ptrADC_settings->int_handler; // enter the interrupt hanlder function
                                fnEnterInterrupt(irq_ADC_ID, ptrADC_settings->int_priority, (void (*)(void))_ADC_Interrupt[ptrADC_settings->int_adc_controller]);
                                ucChannelConfig |= ADC_SC1A_AIEN;        // enable interrupt on end of conversion
                                if (ptrADC_settings->int_adc_int_type & (ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT)) { // {1} if a level is defined
    #if defined KINETIS_KE
                                    ucChannelConfig = ADC_SC1A_ADCO;     // enable continuous conversion
    #else
                                    ptrADC->ADC_SC3 |= ADC_SC3_ADCO;     // enable continuous conversion
    #endif
                                    switch (ptrADC_settings->int_adc_int_type & (ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT)) {
                                    case ADC_LOW_LIMIT_INT:
                                        ptrADC->ADC_CV1 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_low_level_trigger, 0); // set low level threshold according to mode
                                        ptrADC->ADC_SC2 &= ~(ADC_SC2_ACFGT);
                                        ptrADC->ADC_SC2 |= (ADC_SC2_ACFE); // enable compare function for lower than threshold
                                        break;
                                    case ADC_HIGH_LIMIT_INT:
                                        ptrADC->ADC_CV1 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_high_level_trigger, 1); // set high level threshold according to mode
                                        ptrADC->ADC_SC2 |= (ADC_SC2_ACFGT | ADC_SC2_ACFE); // enable compare function for greater or equal to threshold
                                        break;
    #if !defined KINETIS_KE
                                    case (ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT): // if the low value is less that the high value it results in a trigger when the value is higher or lower than the thresholds - if the low value is higher than th high value it rsults in a trigger if the value is between the thresholds
                                        ptrADC->ADC_CV1 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_low_level_trigger, 0); // set low range threshold
                                        ptrADC->ADC_CV2 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_high_level_trigger, 1); // set high range threshold
                                        ptrADC->ADC_SC2 &= ~ADC_SC2_ACFGT;
                                        ptrADC->ADC_SC2 |= (ADC_SC2_ACFE | ADC_SC2_ACREN); // enable compare function on range
                                        break;
    #endif
                                    }
                                }
                            }
    #if !defined DEVICE_WITHOUT_DMA
                        }
    #endif
                  //}
    #if defined KINETIS_KE
                    ucChannelConfig |= (unsigned char)(ptrADC_settings->int_adc_bit & ADC_SC1A_ADCH_OFF); // the channel being configured
                    if (ptrADC_settings->int_adc_bit < ADC_SC1A_ADCH_VSS) { // if configuring a channel that uses a pin
                        ptrADC->ADC_APCTL1 |= (ADC_APCTL1_AD0 << ptrADC_settings->int_adc_bit); // enable the pin's ADC function
                    }
    #else
                    ptrADC->ADC_SC1A = ucChannelConfig;                  // start conversion if software mode or enable triggered conversion
    #endif
                }
    #if defined KINETIS_KE                                               // {2}
                else {
                    ucChannelConfig |= (ptrADC->ADC_SC1A & ~(ADC_SC1A_ADCO | ADC_SC1A_ADCO));
                }
                ptrADC->ADC_SC1A = ucChannelConfig;                      // start conversion if software mode or enable triggered conversion
    #endif
            }
            if (((ptrADC_settings->int_adc_mode & (ADC_GET_RESULT)) != 0) && (ptrADC_settings->int_adc_result != 0)) { // if there is a result structure
    #if defined _WINDOWS
                switch (ptrADC_settings->int_adc_controller) {
                case 0:
                    if (IS_POWERED_UP(6, SIM_SCGC6_ADC0) == 0) {
                        _EXCEPTION("Trying to read from ADC0 that is not powered up!");
                    }
                    break;
        #if ADC_CONTROLLERS > 1
                case 1:
                    if (IS_POWERED_UP(3, SIM_SCGC3_ADC1) == 0) {
                        _EXCEPTION("Trying to read from ADC1 that is not powered up!");
                    }
                    break;
        #endif
        #if ADC_CONTROLLERS > 2
                case 2:
                    if (IS_POWERED_UP(6, SIM_SCGC6_ADC2) == 0) {
                        _EXCEPTION("Trying to read from ADC2 that is not powered up!");
                    }
                    break;
        #endif
        #if ADC_CONTROLLERS > 3
                case 3:
                    if (IS_POWERED_UP(3, SIM_SCGC3_ADC3) == 0) {
                        _EXCEPTION("Trying to read from ADC3 that is not powered up!");
                    }
                    break;
        #endif
                }
    #endif
                if (((ptrADC_settings->int_adc_mode & ADC_READ_ONLY) == 0) && ((ptrADC->ADC_SC1A & ADC_SC1A_AIEN) == 0)) { // no interrupt and not simple read
                    while ((ptrADC->ADC_SC1A & ADC_SC1A_COCO) == 0) {    // wait for conversion to complete
    #if defined _WINDOWS
                        ptrADC->ADC_SC1A |= ADC_SC1A_COCO;               // set conversion complete flag
    #endif
                    }
                }
                ptrADC_settings->int_adc_result->sADC_value[0] = (signed short)ptrADC->ADC_RA; // return the read value
            }
        }
#endif
