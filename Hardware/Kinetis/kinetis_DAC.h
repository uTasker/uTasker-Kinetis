/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_DAC.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    19.11.2013 Add DAC DMA configuration                                 {60}
    23.12.2015 Add automatic DAC DMA buffer repetition                   {1}
 
*/

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                         DAC Configuration                           */
/* =================================================================== */

#if defined _DAC_CONFIG_CODE
        {
            DAC_SETUP *ptrDAC_settings = (DAC_SETUP *)ptrSettings;
            DAC_REGS *ptrDAC_regs;
            unsigned char ucConfig = (DAC_C0_DACRFS_VREF | DAC_C0_DACTRGSET_HW | DAC_C0_LPEN_HIGH_POWER);
            if ((ptrDAC_settings->dac_mode & DAC_DISABLE) != 0) {
                switch (ptrDAC_settings->int_dac_controller) {
                case 0:
    #if defined KINETIS_KL
                    POWER_DOWN(6, SIM_SCGC6_DAC0);                       // powered down DAC 0
    #else
                    POWER_DOWN(2, SIM_SCGC2_DAC0);                       // powered down DAC 0
    #endif
                    break;
    #if DAC_CONTROLLERS > 1
                case 1:
                    POWER_DOWN(2, SIM_SCGC2_DAC1);                       // powered down DAC 1
                    break;
    #endif
                default:
                    return;
                }
            }
            switch (ptrDAC_settings->int_dac_controller) {
            case 0:
    #if defined KINETIS_KL
                POWER_UP(6, SIM_SCGC6_DAC0);                             // ensure the DAC 0 is powered up
        #if defined KINETIS_KL05
                _CONFIG_PERIPHERAL(B, 1, PB_1_DAC0_OUT);                 // ensure that the DAC output pin is configured
        #else
                _CONFIG_PERIPHERAL(E, 30, PE_30_DAC0_OUT);               // ensure that the DAC output pin is configured
        #endif
    #else
                POWER_UP(2, SIM_SCGC2_DAC0);                             // ensure the DAC 0 is powered up
    #endif
                ptrDAC_regs = (DAC_REGS *)DAC0_BASE_ADD;
                break;
    #if DAC_CONTROLLERS > 1
            case 1:
                POWER_UP(2, SIM_SCGC2_DAC1);                             // ensure the DAC 1 is powered up
                ptrDAC_regs = (DAC_REGS *)DAC1_BASE_ADD;
                break;
    #endif
            default:
                return;
            }
            if ((ptrDAC_settings->dac_mode & DAC_CONFIGURE) != 0) {
    #if defined KINETIS_KL
                if ((ptrDAC_settings->dac_mode & DAC_REF_VREF2) != 0) {  // use VREF2 rather than VREF1
                    ucConfig = DAC_C0_DACRFS_VREF2;
                }
    #else
                if ((ptrDAC_settings->dac_mode & DAC_REF_VDDA) != 0) {   // use VDAA
                    ucConfig = DAC_C0_DACRFS_VDDA;
                }
    #endif
                else {                                                   // else use VREF
                    // To do - enable VREF for internal use
                    //
                }
                if ((ptrDAC_settings->dac_mode & DAC_BUFFERED_MODE) != 0) {
                    // To do - add buffered mode
                    //
                }
                else {                                                   // non-buffered mode - write to data[0] triggers conversion
                    if ((ptrDAC_settings->dac_mode & DAC_HW_TRIGGER_MODE) == 0) { // if HW triggered mode is defined a DMA trigger source triggers the conversion
                        ucConfig |= DAC_C0_DACTRGSET_SW;                 // software writes start conversion
                    }
                }
            }
            if ((ptrDAC_settings->dac_mode & DAC_OUTPUT_VALUE) != 0) {   // in non-buffered mode set the analogue output value
                ptrDAC_regs->DAC_DAT[0] = ptrDAC_settings->usOutputValue;// prepare the first output value
            }
    #if !defined DEVICE_WITHOUT_DMA
            if ((ptrDAC_settings->dac_mode & (DAC_FULL_BUFFER_DMA | DAC_HALF_BUFFER_DMA)) != 0) { // {60} if DMA is being specified
                unsigned long ulDMA_rules = (DMA_DIRECTION_OUTPUT | DMA_HALF_WORDS); // DMA transfer is from a buffer to a fixed address and each transfer is a half-word in size
        #if defined _WINDOWS
                if ((ptrDAC_settings->int_dac_controller == 0) && (DMAMUX0_DMA0_CHCFG_SOURCE_PIT1 == ptrDAC_settings->ucDmaTriggerSource)) {
                    _EXCEPTION("DAC0 cannot be triggered from PIT channel 1!!");
                }
        #endif
                ptrDAC_regs->DAC_C1 = 0;
                if ((ptrDAC_settings->dac_mode & DAC_FULL_BUFFER_DMA_AUTO_REPEAT) != 0) {
                    ulDMA_rules |= DMA_AUTOREPEAT;
                }
                if ((ptrDAC_settings->dac_mode & DAC_HALF_BUFFER_DMA) != 0) {
                    ulDMA_rules |= DMA_HALF_BUFFER_INTERRUPT;
                }
                fnConfigDMA_buffer(ptrDAC_settings->ucDmaChannel, ptrDAC_settings->ucDmaTriggerSource, ptrDAC_settings->ulDAC_buffer_length, ptrDAC_settings->ptrDAC_Buffer, ptrDAC_regs, ulDMA_rules, ptrDAC_settings->int_handler, ptrDAC_settings->int_priority); // source is the DAC buffer and destination is the DAC data[0] register
                if ((ptrDAC_settings->dac_mode & DAC_BUFFER_DMA_START) != 0) {
                    fnDMA_BufferReset(ptrDAC_settings->ucDmaChannel, DMA_BUFFER_START); // start DMA operation
                }
            }
    #endif
            if ((ptrDAC_settings->dac_mode & DAC_ENABLE) != 0) {
                ucConfig |= DAC_C0_DACEN;                                // DAC operation is to be enabled
            }
            ptrDAC_regs->DAC_C0 = ucConfig;                              // set configuration and enable, if requested
        }
#endif

