/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_SDHC.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/


extern void fnSetSD_clock(unsigned long ulSpeed)
{
    SDHC_SYSCTL &= ~SDHC_SYSCTL_SDCLKEN;                                 // stop clock to set frequency
    SDHC_SYSCTL = (SDHC_SYSCTL_DTOCV_227 | ulSpeed);                     // set new speed
    while ((SDHC_PRSSTAT & SDHC_PRSSTAT_SDSTB) == 0) {                   // wait for new speed to stabilise
#if defined _WINDOWS
        SDHC_PRSSTAT |= SDHC_PRSSTAT_SDSTB;
#endif
    }
    fnDelayLoop(50);                                                     // the SDHC_PRSSTAT_SDSTB doesn't seem to behave correctly so ensure with a delay
    SDHC_SYSCTL = (SDHC_SYSCTL_SDCLKEN | SDHC_SYSCTL_DTOCV_227 | ulSpeed); // enable clock
}


// Initialise the SD controllers interface with low speed clock during initailiation sequence
//
extern void fnInitSDCardInterface(void)
{
    POWER_UP(3, SIM_SCGC3_SDHC);                                         // power up the SDHC controller

    // Configure the pins for the hardware
    //
    _CONFIG_PERIPHERAL(E, 3, (PE_3_SDHC0_CMD  | PORT_DSE_HIGH | PORT_PS_UP_ENABLE)); // SDHC_CMD on PE.3 (alt. function 4)
    _CONFIG_PERIPHERAL(E, 1, (PE_1_SDHC0_D0   | PORT_DSE_HIGH | PORT_PS_UP_ENABLE)); // SDHC_D0 on PE.1  (alt. function 4)
    _CONFIG_PERIPHERAL(E, 0, (PE_0_SDHC0_D1   | PORT_DSE_HIGH | PORT_PS_UP_ENABLE)); // SDHC_D1 on PE.0  (alt. function 4)
    _CONFIG_PERIPHERAL(E, 5, (PE_5_SDHC0_D2   | PORT_DSE_HIGH | PORT_PS_UP_ENABLE)); // SDHC_D2 on PE.5  (alt. function 4)
    _CONFIG_PERIPHERAL(E, 4, (PE_4_SDHC0_D3   | PORT_DSE_HIGH | PORT_PS_UP_ENABLE)); // SDHC_D3 on PE.4  (alt. function 4)
    _CONFIG_PERIPHERAL(E, 2, (PE_2_SDHC0_DCLK | PORT_DSE_HIGH));         // SDHC_DCLK on PE.2 (alt. function 4)

    SDHC_VENDOR = 0;                                                     // disable external DMA request
    SDHC_WML = (0x00010001);                                             // FIFO watermark set to 1 for read and write
    SDHC_PROCTL = (SDHC_PROCTL_EMODE_LITTLE);                            // set little endian mode
    fnSetSD_clock(SDHC_SYSCTL_SPEED_SLOW);                               // set initial speed to about 390kHz (between 100k and 400k)

    SDHC_IRQSTAT = 0xffffffff;                                           // {63} clear all (error) flags
}


// Send a command to the SD-card and return a result, plus optional returned arguments
//
extern int fnSendSD_command(const unsigned char ucCommand[6], unsigned char *ucResult, unsigned char *ptrReturnData)
{
    #define MAX_POLL_COUNT   10000
    static int iCommandYieldCount = 0;
    static int iCommandState = 0;
    int iRtn = 0;

    if (iCommandYieldCount > MAX_POLL_COUNT) {                           // the present access is taking too long - quit with SD card error
    #if !defined UTFAT_DISABLE_DEBUG_OUT
        fnDebugMsg("TIMEOUT!!!\r\n");
    #endif
        iRtn = ERROR_CARD_TIMEOUT;
    }
    else {
        switch (iCommandState) {
        case 0:
            while (SDHC_PRSSTAT & SDHC_PRSSTAT_CIHB) {                   // wait for idle command line
    #if defined _WINDOWS
                SDHC_PRSSTAT &= ~SDHC_PRSSTAT_CIHB;
    #endif
            }
            SDHC_CMDARG = ((ucCommand[1] << 24) | (ucCommand[2] << 16) | (ucCommand[3] << 8) | ucCommand[4]); // set the argument of the command
            switch (ucCommand[0]) {                                      // case on command type
            case GO_IDLE_STATE_CMD0:
                SDHC_XFERTYP  = ((ucCommand[0] << SDHC_XFERTYP_CMDINX_SHIFT) | SDHC_XFERTYP_DTDSEL_WRITE | SDHC_XFERTYP_RSPTYP_NONE | SDHC_XFERTYP_RSPTYP_48 | SDHC_XFERTYP_CMDTYP_NORM); // launch the command (no response)
    #if defined _WINDOWS
                SDHC_IRQSTAT = SDHC_IRQSTAT_CC;                          // since there is no response, just set the sent flag
    #endif
                break;
            case SEND_CSD_CMD9:
            case SEND_CID_CMD2:
                SDHC_XFERTYP  = ((ucCommand[0] << SDHC_XFERTYP_CMDINX_SHIFT) | SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_CICEN | SDHC_XFERTYP_DTDSEL_WRITE | SDHC_XFERTYP_RSPTYP_NONE | SDHC_XFERTYP_RSPTYP_126 | SDHC_XFERTYP_CMDTYP_NORM); // launch the command with long response
    #if defined _WINDOWS
                SDHC_IRQSTAT = SDHC_IRQSTAT_CC;                          // since there is no response, just set the sent flag
    #endif
                break;
    #if defined UTFAT_MULTIPLE_BLOCK_WRITE
            case WRITE_MULTIPLE_BLOCK_CMD25:                             // multiple block write
                SDHC_DSADDR = 0;
                SDHC_BLKATTR = ((*(unsigned long *)ptrReturnData << SDHC_BLKATTR_BLKCNT_SHIFT) | 512); // defined block count with 512 byte size
                SDHC_XFERTYP  = ((ucCommand[0] << SDHC_XFERTYP_CMDINX_SHIFT) | SDHC_XFERTYP_AC12EN | SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_CICEN | SDHC_XFERTYP_RSPTYP_NONE | SDHC_XFERTYP_CMDTYP_NORM | SDHC_XFERTYP_BCEN | SDHC_XFERTYP_DTDSEL_WRITE | SDHC_XFERTYP_MSBSEL | SDHC_XFERTYP_DPSEL | SDHC_XFERTYP_RSPTYP_48); // launch the command with a write
        #if defined _WINDOWS
                SDHC_IRQSTAT = (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_TC);
        #endif
                break;
    #endif
            case WRITE_BLOCK_CMD24:                                      // single sector write
                SDHC_DSADDR = 0;
                SDHC_BLKATTR = ((1 << SDHC_BLKATTR_BLKCNT_SHIFT) | 512); // 1 block with 512 byte size
                SDHC_XFERTYP  = ((ucCommand[0] << SDHC_XFERTYP_CMDINX_SHIFT) | SDHC_XFERTYP_AC12EN | SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_CICEN | SDHC_XFERTYP_RSPTYP_NONE | SDHC_XFERTYP_CMDTYP_NORM | SDHC_XFERTYP_BCEN | SDHC_XFERTYP_DTDSEL_WRITE | SDHC_XFERTYP_MSBSEL | SDHC_XFERTYP_DPSEL | SDHC_XFERTYP_RSPTYP_48); // launch the command with a write
    #if defined _WINDOWS
                SDHC_IRQSTAT = (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_TC);
    #endif
                break;
            case READ_SINGLE_BLOCK_CMD17:                                // single sector read
                {
                    SDHC_DSADDR = 0;
                    SDHC_BLKATTR = ((1 << SDHC_BLKATTR_BLKCNT_SHIFT) | 512); // 1 block with 512 byte size
                    SDHC_XFERTYP  = ((READ_SINGLE_BLOCK_CMD17 << SDHC_XFERTYP_CMDINX_SHIFT) | SDHC_XFERTYP_AC12EN | SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_CICEN | SDHC_XFERTYP_RSPTYP_NONE | SDHC_XFERTYP_RSPTYP_NONE | SDHC_XFERTYP_CMDTYP_NORM | SDHC_XFERTYP_BCEN | SDHC_XFERTYP_DTDSEL_READ | SDHC_XFERTYP_MSBSEL | SDHC_XFERTYP_DPSEL | SDHC_XFERTYP_RSPTYP_48); // launch the command with a read
    #if defined _WINDOWS
                    SDHC_IRQSTAT = (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_TC);
    #endif
                }
                break;
    #if defined UTFAT_MULTIPLE_BLOCK_WRITE
            case STOP_TRANSMISSION_CMD12:                                // terminate multiple block mode
              //SDHC_PROCTL |= (SDHC_PROCTL_SABGREQ);                    // request stopping block transmission (when not all previously specified blocks have been sent)
                while (!(SDHC_IRQSTAT & SDHC_IRQSTAT_TC)) {              // wait for any active transfer to complete
        #if defined _WINDOWS
                    SDHC_IRQSTAT |= SDHC_IRQSTAT_TC;
        #endif
                }
              //SDHC_PROCTL &= ~(SDHC_PROCTL_SABGREQ);
                SDHC_IRQSTAT = (SDHC_IRQSTAT_TC | SDHC_IRQSTAT_BWR | SDHC_IRQSTAT_AC12E); // reset flags

    #endif
          //case PRE_ERASE_BLOCKS_CMD23:                                 // prepare multiple block write
            default:
                SDHC_XFERTYP  = ((ucCommand[0] << SDHC_XFERTYP_CMDINX_SHIFT) | SDHC_XFERTYP_CCCEN | SDHC_XFERTYP_CICEN | SDHC_XFERTYP_DTDSEL_WRITE | SDHC_XFERTYP_RSPTYP_NONE | SDHC_XFERTYP_RSPTYP_48 | SDHC_XFERTYP_CMDTYP_NORM); // launch the command (short response)
    #if defined _WINDOWS
                if (SEND_OP_COND_ACMD_CMD41 == ucCommand[0]) {
                    SDHC_IRQSTAT = (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_CCE); // this response has no valid CRC
                }
                else {
                    SDHC_IRQSTAT = SDHC_IRQSTAT_CC;                      // set the successful response flag
                }
    #endif
                break;
            }
    #if defined _WINDOWS
            _fnSimSD_write((unsigned char)(ucCommand[0] | 0x40));_fnSimSD_write(ucCommand[1]);_fnSimSD_write(ucCommand[2]);_fnSimSD_write(ucCommand[3]);_fnSimSD_write(ucCommand[4]);_fnSimSD_write(0);_fnSimSD_write(0xff);
    #endif
        case 6:
            do {                                                         // loop until the command has been transferred
                if (SDHC_IRQSTAT & SDHC_IRQSTAT_CC) {                    // command has completed
                    if (GO_IDLE_STATE_CMD0 == ucCommand[0]) {            // this command receives no response
                        *ucResult = R1_IN_IDLE_STATE;                    // dummy response for compatibility
                    }
                    else if (/*(READ_SINGLE_BLOCK_CMD17 == ucCommand[0]) || */(WRITE_BLOCK_CMD24 == ucCommand[0])) {
                        *ucResult = 0;                                   // for compatibility
                    }
                    else {
                        if (SDHC_IRQSTAT & SDHC_IRQSTAT_CTOE) {          // timeout occurred while waiting for command to complete
                            SDHC_IRQSTAT = (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_CIE | SDHC_IRQSTAT_CEBE | SDHC_IRQSTAT_CTOE); // clear flags
                            iCommandYieldCount++;                        // monitor the maximum number of timeouts
                            iCommandState = 0;                           // allow resend of command
                            uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_GO); // switch to polling mode of operation
                            return CARD_BUSY_WAIT;
                        }
                        else if (SDHC_IRQSTAT & SDHC_IRQSTAT_CCE) {      // CRC error detected
                            if (ucCommand[0] == SEND_OP_COND_ACMD_CMD41) { // this responds with command value 0x3f and CRC-7 0xff
                                *ucResult = 0;                           // for compatibility
                            }
                            else {                                       // other CRC errors cause retry
                                *ucResult = R1_COMMAND_CRC_ERROR;
                            }
                            SDHC_IRQSTAT = (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_CIE | SDHC_IRQSTAT_CEBE | SDHC_IRQSTAT_CTOE | SDHC_IRQSTAT_CCE); // clear flags
                            break;
                        }

                        if ((SDHC_IRQSTAT & (SDHC_IRQSTAT_CIE | SDHC_IRQSTAT_CEBE)) == 0) { // if matching response and no error
                            switch (ucCommand[0]) {
                            case SEND_IF_COND_CMD8:
                                *ucResult = SDC_CARD_VERSION_2PLUS;      // for compatibility
                                break;
                            case APP_CMD_CMD55:
                                *ucResult = R1_IN_IDLE_STATE;            // for compatibility
                                break;
                            default:
                                *ucResult = 0;                           // for compatibility
                                break;
                            }
                        }
                    }
                    SDHC_IRQSTAT = (SDHC_IRQSTAT_CC | SDHC_IRQSTAT_CIE | SDHC_IRQSTAT_CEBE | SDHC_IRQSTAT_CTOE | SDHC_IRQSTAT_CCE); // clear flags
                    break;
                }
                else {                                                   // the command/response is still being executed
                    iCommandState = 6;
                    if (iCommandYieldCount == 0) {                       // just count once as timeout
                        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_GO); // switch to polling mode of operation                        
                    }
                    iCommandYieldCount++;;
                    return CARD_BUSY_WAIT;                               // poll up to 20 times before yielding
                }
            } while (*ucResult & SD_CARD_BUSY);                          // poll the card until it is no longer indicating busy and returns the value
            if (ptrReturnData != 0) {                                    // if the caller requests data, read it here
                if ((ucCommand[0] == SEND_CSD_CMD9) || (ucCommand[0] == SEND_CID_CMD2)) { // exception requiring 16 bytes
    #if defined _WINDOWS
                    _fnSimSD_write(0xff);_fnSimSD_write(0xff);           // for compatibility
                    SDHC_CMDRSP3  = (_fnSimSD_write(0xff) << 16);
                    SDHC_CMDRSP3 |= (_fnSimSD_write(0xff) << 8);
                    SDHC_CMDRSP3 |= _fnSimSD_write(0xff);
                    SDHC_CMDRSP2  = (_fnSimSD_write(0xff) << 24);
                    SDHC_CMDRSP2 |= (_fnSimSD_write(0xff) << 16);
                    SDHC_CMDRSP2 |= (_fnSimSD_write(0xff) << 8);
                    SDHC_CMDRSP2 |=  _fnSimSD_write(0xff);
                    SDHC_CMDRSP1  = (_fnSimSD_write(0xff) << 24);
                    SDHC_CMDRSP1 |= (_fnSimSD_write(0xff) << 16);
                    SDHC_CMDRSP1 |= (_fnSimSD_write(0xff) << 8);
                    SDHC_CMDRSP1 |=  _fnSimSD_write(0xff);
                    SDHC_CMDRSP0  = (_fnSimSD_write(0xff) << 24);
                    SDHC_CMDRSP0 |= (_fnSimSD_write(0xff) << 16);
                    SDHC_CMDRSP0 |= (_fnSimSD_write(0xff) << 8);
                    SDHC_CMDRSP0 |=  _fnSimSD_write(0xff);
                    _fnSimSD_write(0xff);                                // dummy for checksum
    #endif
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP3 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP3 >> 8);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP3);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP2 >> 24);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP2 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP2 >> 8);
                    *ptrReturnData++ = (unsigned char)SDHC_CMDRSP2;
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP1 >> 24);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP1 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP1 >> 8);
                    *ptrReturnData++ = (unsigned char)SDHC_CMDRSP1;
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP0 >> 24);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP0 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP0 >> 8);
                    *ptrReturnData++ = (unsigned char)SDHC_CMDRSP0;
                    *ptrReturnData   = 0;                                // checksum not received from controller
                }
                else if (ucCommand[0] != WRITE_MULTIPLE_BLOCK_CMD25) {   // WRITE_MULTIPLE_BLOCK_CMD25 passes value but doesn't want a response
    #if defined _WINDOWS
                    if (ucCommand[0] != SEND_OP_COND_ACMD_CMD41) {
                        _fnSimSD_write(0xff);_fnSimSD_write(0xff);       // for compatibility
                    }
                    SDHC_CMDRSP0  = (_fnSimSD_write(0xff) << 24);
                    SDHC_CMDRSP0 |= (_fnSimSD_write(0xff) << 16);
                    SDHC_CMDRSP0 |= (_fnSimSD_write(0xff) << 8);
                    SDHC_CMDRSP0 |=  _fnSimSD_write(0xff);
    #endif
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP0 >> 24);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP0 >> 16);
                    *ptrReturnData++ = (unsigned char)(SDHC_CMDRSP0 >> 8);
                    *ptrReturnData   = (unsigned char)SDHC_CMDRSP0;
                }
            }
            break;
        }
    }
    if (iCommandYieldCount != 0) {
        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_STOP);             // switch to event mode of operation since write has completed
        iCommandYieldCount = 0;
    }
    iCommandState = 0;
    return iRtn;
}

// Temporary debug counters (can be removed)
//
static unsigned long ulSectorReadCount = 0;
static unsigned long ulPartialSectorReadCount = 0;
static unsigned long ulSectorWriteCount = 0;

// Read a sector from SD card into the specified data buffer (the buffer should be long word aligned for maximum efficiency)
//
extern int fnGetSector(unsigned char *ptrBuf)
{
    unsigned long *ptrData = (unsigned long *)ptrBuf;                    // the Kinetis driver ensures that the buffer is long word aligned
    int i = (512/sizeof(unsigned long));                                 // long word in a 512 byte sector
    int iMonitorRead;                                                    // {62}
    #if defined _COMPILE_KEIL                                            // {47}
    unsigned long ulTempBuffer[512/sizeof(unsigned long)];               // long word aligned intermediate buffer
    int iIntermediateBuffer;
    if ((unsigned long)ptrData & 0x03) {                                 // check that the caller's buffer is long word aligned
        ptrData = ulTempBuffer;                                          // read to intermediate buffer to avoid access problems
        iIntermediateBuffer = 1;
    }
    else {
        iIntermediateBuffer = 0;
    }
    #endif
    #if defined _WINDOWS
    _fnSimSD_write(0xff);                                                // dummy for simulator compatibility
    #endif
    while (SDHC_PRSSTAT & SDHC_PRSSTAT_DLA) {                            // ensure that the previous command has completed
    #if defined _WINDOWS
        SDHC_PRSSTAT &= ~SDHC_PRSSTAT_DLA;
    #endif
    };
    ulSectorReadCount++;                                                 // counter to indicate how many times a sector read has been executed
    while (i--) {                                                        // for each long word to be read
        iMonitorRead = 0;                                                // {62}
        if (SDHC_IRQSTAT & (SDHC_IRQSTAT_DEBE | SDHC_IRQSTAT_DCE | SDHC_IRQSTAT_DTOE)) { // check for read errors
            SDHC_IRQSTAT = (SDHC_IRQSTAT_DEBE | SDHC_IRQSTAT_DCE | SDHC_IRQSTAT_DTOE | SDHC_IRQSTAT_BRR); // reset error flags
            return UTFAT_DISK_READ_ERROR;                                // return error
        }
        while ((SDHC_PRSSTAT & SDHC_PRSSTAT_BREN) == 0) {                // wait for the buffer read enable flag to become set (room for write in output buffer)
    #if defined _WINDOWS
            SDHC_PRSSTAT |= SDHC_PRSSTAT_BREN;
    #endif
            if (++iMonitorRead >= 1000) {                                // {62} if the buffer stalls (can happen when SD card removed during operation)
                return UTFAT_DISK_READ_ERROR;                            // return with error
            }
        }
    #if defined _WINDOWS
        SDHC_DATPORT =   _fnSimSD_write(0xff);
        SDHC_DATPORT |= (_fnSimSD_write(0xff) << 8);
        SDHC_DATPORT |= (_fnSimSD_write(0xff) << 16);
        SDHC_DATPORT |= (_fnSimSD_write(0xff) << 24);
    #endif
        *ptrData++ = SDHC_DATPORT;                                       // this access should be long word aligned for maximum efficiency
    }
    while ((SDHC_IRQSTAT & SDHC_IRQSTAT_TC) == 0) {                      // wait for complete transfer to complete
    #if defined _WINDOWS
        SDHC_IRQSTAT |= SDHC_IRQSTAT_TC;
    #endif
    }
    #if defined _WINDOWS
    _fnSimSD_write(0xff);_fnSimSD_write(0xff);                           // dummy for simulator compatibility
    #endif
    SDHC_IRQSTAT = (SDHC_IRQSTAT_TC | SDHC_IRQSTAT_BRR | SDHC_IRQSTAT_AC12E); // reset flags
    #if defined _COMPILE_KEIL                                            // {47}
    if (iIntermediateBuffer != 0) {                                      // read data is in intermediate buffer
        uMemcpy(ptrBuf, ((unsigned char *)ulTempBuffer), 512);           // copy from intermediate buffer to user's buffer which isn't aligned
    }
    #endif
    return UTFAT_SUCCESS;                                                // read successfully terminated
}

// Read a specified amount of data from present SD card sector into the specified data buffer (usStart and usStop are offset from start of sector and avoid other data outside of this range being overwritten)
//
extern int fnReadPartialSector(unsigned char *ptrBuf, unsigned short usStart, unsigned short usStop)
{
    int iReadState;
    unsigned long ulTempBuffer[512/sizeof(unsigned long)];               // since the SD controller will always read a complete sector we use an intermediate buffer (it is long word aligned)
ulPartialSectorReadCount++;
    if ((iReadState = fnGetSector((unsigned char *)ulTempBuffer)) == UTFAT_SUCCESS) { // fill the temporary buffer with the sector data
        uMemcpy(ptrBuf, (((unsigned char *)ulTempBuffer) + usStart), (usStop - usStart)); // just copy the part required
    }
    return iReadState;
}

// Write present sector with buffer data
//
extern int fnPutSector(unsigned char *ptrBuf, int iMultiBlock)
{
    unsigned long *ptrData = (unsigned long *)ptrBuf;
    int i = (512/sizeof(unsigned long));
//unsigned long ulStartTime = uTaskerSystemTick;
//static unsigned long ulMaxDelay = 0;
//_SETBITS(A, DEMO_LED_2);
ulSectorWriteCount++;
    #if defined _WINDOWS
    _fnSimSD_write(0xfe);                                                // dummy for simulator compatibility
    #endif
    while (i--) {                                                        // for each long word of the sector buffer to be written
        while (!(SDHC_PRSSTAT & SDHC_PRSSTAT_BWEN)) {                    // wait until there is buffer space when the tranmission buffer fills
    #if defined _WINDOWS
            SDHC_PRSSTAT |= SDHC_PRSSTAT_BWEN;
    #endif
        }
        SDHC_DATPORT = *ptrData++;                                       // copy the data to be sent to the SDHC buffer (it will be transferred by the controller as fast as possible to the card)
    #if defined _WINDOWS                                                 // simulate the write of 4 bytes from the long word buffer entry
        _fnSimSD_write((unsigned char)SDHC_DATPORT);
        _fnSimSD_write((unsigned char)(SDHC_DATPORT >> 8));
        _fnSimSD_write((unsigned char)(SDHC_DATPORT >> 16));
        _fnSimSD_write((unsigned char)(SDHC_DATPORT >> 24));
    #endif
    }
    #if defined _WINDOWS
    _fnSimSD_write(0xff);_fnSimSD_write(0xff);_fnSimSD_write(0xff);      // dummy for simulator compatibility after sector data transfer
    #endif
    if (iMultiBlock == 0) {                                              // when performing multiple block transfers the transfer complete is not waited for
      //int iMonitorWrite = 0;
        while (!(SDHC_IRQSTAT & SDHC_IRQSTAT_TC)) {                      // wait for transfer to complete
        #if defined _WINDOWS
            SDHC_IRQSTAT |= SDHC_IRQSTAT_TC;
        #endif
          //if (++iMonitorWrite > 100000) {                              // protect against blocking
          //    return UTFAT_DISK_WRITE_ERROR;
          //}
        }
        SDHC_IRQSTAT = (SDHC_IRQSTAT_TC | SDHC_IRQSTAT_BWR | SDHC_IRQSTAT_AC12E); // reset flags
     /* if (uTaskerSystemTick > (ulStartTime + 1) ) {                // monitor > 50ms delays
            if ((uTaskerSystemTick - ulStartTime) > ulMaxDelay) {
                ulMaxDelay = (uTaskerSystemTick - ulStartTime);
                fnDebugMsg("Record = ");
                fnDebugDec(ulMaxDelay, WITH_CR_LF);
            }
            else {
                fnDebugMsg("L ");
            }
        }*/
    }
//_CLEARBITS(A, DEMO_LED_2);
    return UTFAT_SUCCESS;
}
