/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_I2S.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/

#if defined _I2S_CODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned long I2S_Tx_Errors[I2S_AVAILABLE] = {0};
static unsigned long I2S_Rx_Errors[I2S_AVAILABLE] = {0};

/* =================================================================== */
/*                      I2S/SAI Interrupt Handlers                     */
/* =================================================================== */

static void irq_i2S0_error(void)
{
    if ((I2S0_TCSR & (I2S_TCSR_FEF | I2S_TCSR_SEF)) != 0) {
        I2S0_TCSR |= (I2S_TCSR_FEF | I2S_TCSR_SEF);                      // reset tx error flags
        I2S_Tx_Errors[0]++;
    }
    if ((I2S0_RCSR & (I2S_RCSR_FEF | I2S_RCSR_SEF)) != 0) {
        I2S0_RCSR |= (I2S_RCSR_FEF | I2S_RCSR_SEF);                      // reset rx error flags
        I2S_Rx_Errors[0]++;
    }
}
#endif


/* =================================================================== */
/*                      I2S/SAI Configuration                          */
/* =================================================================== */


#if defined _I2S_SAI_CONFIG_CODE
        {
            I2S_SAI_SETUP *ptrI2S_SAIsetup = (I2S_SAI_SETUP *)ptrSettings;
            _KINETIS_I2S_SAI *ptrSAI = (_KINETIS_I2S_SAI *)I2S0_BLOCK;

            // SAI (I2S) initialisation -- I2S type protocol, 16 bit, mono
            //
            if ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_CONFIG_TX) != 0) {
                POWER_UP(6, SIM_SCGC6_SAI0);                             // ensure that the clock is enabled to the module
                // Transmitter
                //
                _CONFIG_PERIPHERAL(B, 18,  (PB_18_I2S0_TX_BCLK | PORT_DSE_HIGH | PORT_SRE_FAST)); // configure the used I2S pins
                _CONFIG_PERIPHERAL(C, 1,   (PC_1_I2S0_TXD0 | PORT_DSE_HIGH | PORT_SRE_FAST));
                _CONFIG_PERIPHERAL(C, 2,   (PC_2_I2S0_TX_FS | PORT_DSE_HIGH | PORT_SRE_FAST));

                ptrSAI->I2S_TCSR = (I2S_TCSR_SR | I2S_TCSR_FR);          // software and FIFO reset

              //_CONFIG_PERIPHERAL(C, 4,   (PC_4_I2S0_MCLK | PORT_DSE_HIGH | PORT_SRE_FAST)); // I2S0_MCLK is not used - CLKOUT at 24MHz is instead supplied to the DSP
                SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_CLKOUTSEL_MASK)) | SIM_SOPT2_CLKOUTSEL_BUS); // supply 24MHz (IRC48M/2) bus clock to DSP
                #if BUS_CLOCK != 24000000
                    #error "Bus clock is expected to be 24MHz!!"
                #endif
                _CONFIG_PERIPHERAL(C, 3,   (PC_3_CLKOUT | PORT_DSE_HIGH | PORT_SRE_FAST));
                fnDebugMsg("supplying 24MHz clock to DSP\r\n");

                ptrSAI->I2S_TCSR = (0);                                  // release software reset
                ptrSAI->I2S_TCR2 = (I2S_TCR2_SYNC_ASYN | I2S_TCR2_BCD_EXT_SLAVE | I2S_TCR2_BCP_ACT_LOW); // asynchronous mode with bit clock generated externally (slave)
                ptrSAI->I2S_TCR4 = (I2S_TCR4_FSD_EXT_SLAVE | I2S_TCR4_MF_MSB | I2S_TCR4_FSE | I2S_TCR4_FSP_HIGH | I2S_TCR4_FRSZ_2 | ((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_TCR4_SYWD_SHIFT)); // frame sync generated externally (slave), MSB first, frame sync one bit early, two words in a frame
                ptrSAI->I2S_TCR3 = (I2S_TCR3_WDFL_FIRST);                // start flag set on first work in frame
                ptrSAI->I2S_TCR5 = (((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_TCR5_FBT_SHIFT) | ((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_TCR5_W0W_SHIFT) | ((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_TCR5_WNW_SHIFT));
                ptrSAI->I2S_TMR  = I2S_TMR_CH1_MASK;                     // mono, so mask word 1 so that it is not copied to the input FIFO

                if ((ptrI2S_SAIsetup->I2S_SAI_mode & (I2S_SAI_FULL_BUFFER_DMA | I2S_SAI_HALF_BUFFER_DMA)) != 0) { // if DMA is being specified
                    fnConfigDMA_buffer(ptrI2S_SAIsetup->ucDmaChannel, ptrI2S_SAIsetup->ucDmaTriggerSource, ptrI2S_SAIsetup->ulI2S_SAI_buffer_length, ptrI2S_SAIsetup->ptrI2S_SAI_Buffer, (void *)&ptrSAI->I2S_TDR0, ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_FULL_BUFFER_DMA_AUTO_REPEAT) != 0), ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_HALF_BUFFER_DMA) != 0), ptrI2S_SAIsetup->int_handler, ptrI2S_SAIsetup->int_priority, 1); // source is the I2S buffer and destination is the I2S tx data register
                    ptrSAI->I2S_TCSR = I2S_TCSR_FWDE;                    // enable DMA on FIFO warning
                }
            }
            else if ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_CONFIG_RX) != 0) {
                POWER_UP(6, SIM_SCGC6_SAI0);                             // ensure that the clock is enabled to the module
                ptrSAI->I2S_RCSR = (I2S_RCSR_SR | I2S_RCSR_FR);          // software and FIFO reset
                // Receiver
                //
                _CONFIG_PERIPHERAL(C, 5,   (PC_5_I2S0_RXD0 | PORT_DSE_HIGH | PORT_SRE_FAST));

                ptrSAI->I2S_RCSR = (0);                                 // remove software reset
                ptrSAI->I2S_RCR2 = (I2S_RCR2_SYNC_SYN_TX | I2S_RCR2_BCD_EXT_SLAVE | I2S_RCR2_BCP_ACT_LOW); // synchronous with transmitter
                ptrSAI->I2S_RCR4 = (I2S_RCR4_FSD_EXT_SLAVE | I2S_RCR4_MF_MSB | I2S_RCR4_FSE | I2S_RCR4_FSP_HIGH | I2S_RCR4_FRSZ_2 | ((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_RCR4_SYWD_SHIFT)); // frame sync generated externally (slave), MSB first, frame sync one bit early, two words in a frame
                ptrSAI->I2S_RCR3 = (I2S_RCR3_WDFL_FIRST);                // start flag set on first work in frame
                ptrSAI->I2S_RCR5 = (((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_RCR5_FBT_SHIFT) | ((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_RCR5_W0W_SHIFT) | ((ptrI2S_SAIsetup->ucSynchBits - 1) << I2S_RCR5_WNW_SHIFT));
                ptrSAI->I2S_RMR  = I2S_RMR_CH1_MASK;                     // mono, so mask word 1 so that it is not copied to the input FIFO

                ptrSAI->I2S_MCR = I2S_MCR_MOE_IN;                        // master clock is external (input for slave)

                if ((ptrI2S_SAIsetup->I2S_SAI_mode & (I2S_SAI_FULL_BUFFER_DMA | I2S_SAI_HALF_BUFFER_DMA)) != 0) { // if DMA is being specified
                    fnConfigDMA_buffer(ptrI2S_SAIsetup->ucDmaChannel, ptrI2S_SAIsetup->ucDmaTriggerSource, ptrI2S_SAIsetup->ulI2S_SAI_buffer_length, (void *)&ptrSAI->I2S_RDR0, ptrI2S_SAIsetup->ptrI2S_SAI_Buffer, ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_FULL_BUFFER_DMA_AUTO_REPEAT) != 0), ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_HALF_BUFFER_DMA) != 0), ptrI2S_SAIsetup->int_handler, ptrI2S_SAIsetup->int_priority, 0); // source is the I2S rx data register and destination is the I2S buffer
                    ptrSAI->I2S_RCSR = I2S_RCSR_FWDE;                    // enable DMA on FIFO warning
                }
            }
            if ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_ENABLE_TX) != 0) {
                fnEnterInterrupt(irq_I2S0_ID, 0, (void (*)(void))irq_i2S0_error); // enter error interrupt handler
                ptrSAI->I2S_TCSR |= (I2S_TCSR_TE | I2S_TCSR_FEIE | I2S_TCSR_FEF | I2S_TCSR_SEF); // enable transmitter with error interrupt (clear error flags)
              //ptrSAI->I2S_TCR3 |= (I2S_TCR3_TCE);                      // enable transmit data channel
            }
            if ((ptrI2S_SAIsetup->I2S_SAI_mode & I2S_SAI_ENABLE_RX) != 0) {
                fnEnterInterrupt(irq_I2S0_ID, 0, (void (*)(void))irq_i2S0_error); // enter error interrupt handler
                ptrSAI->I2S_RCSR |= (I2S_RCSR_RE | I2S_RCSR_FEIE | I2S_RCSR_FEF | I2S_RCSR_SEF); // enable receiver with error interrupt (clear error flags)
              //ptrSAI->I2S_RCR3 |= (I2S_RCR3_RCE);                      // enable receive data channel
            }
        }
#endif

