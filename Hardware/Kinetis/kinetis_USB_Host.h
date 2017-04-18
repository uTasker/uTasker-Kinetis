/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_USB_Host.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2015
    *********************************************************************

*/

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined USB_FS_MALLOC                                               // {42}
    #define USB_FS_MALLOC(x) uMalloc((MAX_MALLOC)(x)) 
#endif
#if !defined USB_FS_MALLOC_ALIGN                                         // {42}
    #define USB_FS_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
#endif
#if !defined USB_HS_MALLOC                                               // {42}
    #define USB_HS_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif
#if !defined USB_HS_MALLOC_ALIGN                                         // {42}
    #define USB_HS_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
#endif


#if defined ERRATA_ID_7166 && defined SUPPORT_LOW_POWER
    #error "errate e7166 - SOC: SDHC, NFC, USBOTG, and cache modules are not clocked correctly in low-power modes - do not enable low power operation when USB is used!"
#endif

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

typedef struct stUSB_ERRORS                                              // {71}
{
    unsigned long ulUSB_errors_CRC5;
    unsigned long ulUSB_errors_CRC16;
} USB_ERRORS;

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static USB_HW usb_hardware = {0};                                        // {33}
static KINETIS_USB_ENDPOINT_BD *ptrBDT = 0;
static USB_END_POINT *usb_endpoints = 0;
static USB_ERRORS USB_errors = {0};                                      // {71}

static void fnUSB_reset(int iError);
static void fnUSB_delay(void (*timeout_handler)(void), unsigned long ulDelay);

/* =================================================================== */
/*                                USB                                  */
/* =================================================================== */


#define fnGetEndPointCtr(iEndPoint) (unsigned char *)(ENDPT0_ADD + ((iEndPoint) * sizeof(unsigned long)))

    #if defined USB_HOST_SUPPORT

static int fnSendToken(unsigned char ucPID)
{
    while ((CTL & TXSUSPEND_TOKENBUSY) != 0) {                           // wait if there is already a token in process
        if ((INT_STAT & USB_RST) != 0) {                                 // reset state detected
            CTL &= ~TXSUSPEND_TOKENBUSY;                                 // abort present token transaction
            return -1;                                                   // quit
        }
    }
    if ((ucPID >> 4) == IN_PID) {                                        // if an IN PID is to be sent
        unsigned char ucEndpoint = (ucPID & 0x0f);                       // the endpoint
        if (ucEndpoint != usb_hardware.ucHostEndpointActive) {           // change of endpoint detected
            usb_hardware.ptrEndpoint = &usb_endpoints[ucEndpoint];       // the endpoint's characteristics
            usb_hardware.ucHostEndpointActive = ucEndpoint;              // the newly active endpoint
            if (usb_endpoints[ucEndpoint].ulNextRxData0 & DATA_1) {      // endure that the receive buffer has the endpoints data flags set correctly
                *(usb_hardware.ptr_ulUSB_Rx_BDControl) |= DATA_1;
                *(usb_hardware.ptr_ulUSB_Alt_Rx_BDControl) &= ~(DATA_1);
            }
            else {
                *(usb_hardware.ptr_ulUSB_Rx_BDControl) &= ~(DATA_1);
                *(usb_hardware.ptr_ulUSB_Alt_Rx_BDControl) |= DATA_1;
            }
        }
    }
    TOKEN = ucPID;                                                       // start transmission of token to endpoint
#if defined _WINDOWS
    fnAddToken(TOKEN);                                                   // inform simulator which token was last sent
#endif
    INT_ENB = (TOK_DNE_EN | STALL_EN | USB_ERROR_EN | USB_RST);          // interrupt when complete
    return 0;                                                            // success
}

static void usb_start(void)
{
    fnUSB_handle_frame(USB_DEVICE_DETECTED, 0, 0, &usb_hardware);        // generic handler routine which will always request a setup frame to be sent
}

// USB host reset pulse complete
//
static void usb_reset_timeout(void)
{
    KINETIS_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;                      // start of BDT
    ERR_ENB = (CRC5_EOF_EN | CRC16_EN/* | BTO_ERR_EN*/);                 // enable some error sources
    CTL &= ~USB_RESET;                                                   // remove USB bus reset
    CTL |= USB_EN_SOF_EN;                                                // start sending SOF packets
    usb_hardware.ptrEndpoint = &usb_endpoints[0];
    usb_hardware.ucHostEndpointActive = 0;
    usb_hardware.ucBufferUsage = 0;                                      // reset to even reception and even transmit buffers
    usb_hardware.ptr_ulUSB_Rx_BDControl = &ptEndpointBD->usb_bd_rx_even.ulUSB_BDControl; // first reception is to the even buffer
    usb_hardware.ptr_ulUSB_Alt_Rx_BDControl = &ptEndpointBD->usb_bd_rx_odd.ulUSB_BDControl; // the alternate buffer is the odd buffer
    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl; // first transmission is from even buffer (DATA0)
    usb_hardware.ptrDatBuffer = usb_hardware.ptrUSB_BD_EvenData = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
    if (ADDR & LS_EN) {
        usb_hardware.ucDeviceSpeed = USB_DEVICE_SPEED_LOW;
    }
    else {
        usb_hardware.ucDeviceSpeed = USB_DEVICE_SPEED_FULL;
    }
    fnUSB_delay(usb_start, PIT_MS_DELAY(40));     // try waiting to allow slow sticks to be ready
  //fnUSB_handle_frame(USB_DEVICE_DETECTED, 0, 0, &usb_hardware);        // generic handler routine which will always request a setup frame to be sent
}

// SETUP was sent but not acknowledged within timeout period
//
static void usb_timeout(void)
{
    CTL |= USB_RESET;                                                    // send reset to USB bus for a minimum of 10ms
    fnUSB_delay(usb_reset_timeout, PIT_MS_DELAY(15));
    fnUSB_reset(1);                                                      // reset to try again
    fnUSB_handle_frame(USB_DEVICE_TIMEOUT, 0, 0, &usb_hardware); 
}

extern int fnHostEndpoint(unsigned char ucEndpoint, int iCommand, int iEvent)
{
    switch (iCommand) {
    case IN_POLLING:
        if (iEvent != 0) {
            fnSendToken((unsigned char)((IN_PID << 4) | ucEndpoint));    // start IN polling on this endpoint (the new endpoint is set as active)
        }
        else {
            // To do - disable IN polling on the endpoint
        }
    }
    return 0;
}

extern void fnResetDataToggle(int iEndpoint, USB_HW *ptrUSB_HW)
{
    usb_endpoints[iEndpoint].ulNextRxData0 &= ~(DATA_1);                 // next IN reception from the endpoint will be changed to DATA 0
}
    #endif

// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *usb_hardware, unsigned char ucFrameType, KINETIS_USB_BD *ptUSB_BD_rx, KINETIS_USB_ENDPOINT_BD *ptEndpointBD)
{
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting messages to queue

    usb_hardware->ptrEndpoint = &usb_endpoints[iEndpoint_ref];           // mark the present transmit endpoint information for possible subroutine or response use
    #if defined USB_HOST_SUPPORT
    if ((usb_hardware->ucBufferUsage & TX_TRANSACTION) == 0)             // host uses only endpoint 0 for transmission
    #else
    if ((usb_endpoints[iEndpoint_ref].ulEndpointSize & ALTERNATE_TX_BUFFER) == 0)
    #endif
    {                                                                    // set the next transmit pointer details
        usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl; // prepare hardware relevant data for the generic handler's use
        usb_hardware->ptrDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
    }
    else {
        usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl; // prepare hardware relevant data for the generic handler's use
        usb_hardware->ptrDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
    }

    switch (fnUSB_handle_frame(ucFrameType, (unsigned char *)ptUSB_BD_rx->ptrUSB_BD_Data, iEndpoint_ref, usb_hardware)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        *usb_hardware->ptr_ulUSB_BDControl = (OWN | usb_hardware->ptrEndpoint->ulNextTxData0); // transmit a zero data packet on control endpoint
        _SIM_USB(USB_SIM_TX, iEndpoint_ref, usb_hardware);
        usb_hardware->ptrEndpoint->ulNextTxData0 ^= DATA_1;              // toggle the data packet
    #if defined USB_HOST_SUPPORT
        if (usb_hardware->ucModeType & USB_HOST_MODE) {
            usb_hardware->ucBufferUsage ^= TX_TRANSACTION;
            fnSendToken((unsigned char)((OUT_PID << 4) | iEndpoint_ref));// start zero data transfer
            goto _free_rx;
        }
    #endif
        usb_hardware->ptrEndpoint->ulEndpointSize ^= ALTERNATE_TX_BUFFER;
        ptUSB_BD_rx->ulUSB_BDControl |= (OWN);                           // re-enable reception for this endpoint buffer since it has been freed
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        uEnable_Interrupt();
        return MAINTAIN_OWNERSHIP;
    #if defined USB_HOST_SUPPORT
    case INITIATE_IN_TOKEN:
        fnSendToken((unsigned char)(IN_PID << 4) | iEndpoint_ref);       // start transmission of next IN token
        goto _free_rx;
 // case SEND_SETUP:
 //     fnSendToken(SETUP_PID << 4);                                     // start transmission of next SETUP token
 //     goto _free_rx;        
    #endif
    case STALL_ENDPOINT:                                                 // send stall
        if (iEndpoint_ref == 0) {                                        // check whether control 0 endpoint
            ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL);// force stall handshake on both control 0 buffers
            ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = (OWN | BDT_STALL);
            fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);       
            _SIM_USB(USB_SIM_STALL, iEndpoint_ref, usb_hardware);
        }
        else {                                                           // assume IN endpoint is always one higher than OUT endpoint
            KINETIS_USB_ENDPOINT_BD *ptOUT_BD = ptEndpointBD;
            int iIN_ref = fnGetPairedIN(iEndpoint_ref);                  // get the paired IN endpoint reference
            ptEndpointBD = ptrBDT;
            ptEndpointBD += iIN_ref;
            ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL); // stall/halt the corresponding IN endpoint
            ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = (OWN | BDT_STALL);
            ptEndpointBD = ptOUT_BD;
            fnSetUSBEndpointState(iIN_ref, USB_ENDPOINT_STALLED);        // mark the stall at the IN endpoint
            _SIM_USB(USB_SIM_STALL, iIN_ref, usb_hardware);
        }
        // Fall through to free buffer
        //
    default:
#if defined USB_HOST_SUPPORT
_free_rx:
#endif
#if defined USB_HOST_SUPPORT                                             // not using usb_hardware->ulRxControl
        ptUSB_BD_rx->ulUSB_BDControl &= DATA_1;                          // retain the DATA1 state of the receive buffer
        ptUSB_BD_rx->ulUSB_BDControl |= (OWN | (usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS))); // re-enable reception on this endpoint
        usb_endpoints[iEndpoint_ref].ulNextRxData0 = ptUSB_BD_rx->ulUSB_BDControl; // last buffer processed
#else
        usb_hardware->ulRxControl |= (usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS)); // reception buffer size
        ptUSB_BD_rx->ulUSB_BDControl = (OWN | usb_hardware->ulRxControl); // re-enable reception on this endpoint
        usb_endpoints[iEndpoint_ref].ulNextRxData0 = usb_hardware->ulRxControl; // last buffer processed
#endif
        break;
    }
    uEnable_Interrupt();
    return 0;
}

// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
    #if defined USB_HS_INTERFACE                                         // {33}
    volatile unsigned long *ptrControl = USBHS_EPCR0_ADDR;
    ptrControl += (ucEndpoint & ~IN_ENDPOINT);
    if (ucEndpoint & IN_ENDPOINT) {
        *ptrControl |= USBHS_EPCR_TXR;                                   // resynchronise the data toggle
        *ptrControl &= ~(USBHS_EPCR_TXS);                                // remove stall on IN
    }
    else {
        *ptrControl |= USBHS_EPCR_RXR;                                   // resynchronise the data toggle        
        *ptrControl &= ~(USBHS_EPCR_RXS);                                // remove stall on OUT
    }
    #else
    KINETIS_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;
    unsigned char *ptrEndPointCtr = fnGetEndPointCtr(ucEndpoint & ~IN_ENDPOINT);
    ptEndpointBD += (ucEndpoint & ~IN_ENDPOINT);
    if (ucEndpoint & IN_ENDPOINT) {
        usb_endpoints[ucEndpoint & ~IN_ENDPOINT].ulNextTxData0 &= ~DATA_1;
        ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = 0;
        ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = 0;
        *ptrEndPointCtr &= ~EP_STALL;
    }
    else {
        ptEndpointBD->usb_bd_rx_even.ulUSB_BDControl &= ~BDT_STALL;
        ptEndpointBD->usb_bd_rx_odd.ulUSB_BDControl  &= ~BDT_STALL;
    }
    #endif
}

    #if defined USB_HOST_SUPPORT
        #if !defined USB_HOST_PIT_CHANNEL
            #define USB_HOST_PIT_CHANNEL    0                            // user PIT0 if no other is defined
        #endif
// A single-shot HW timer reserved for USB host timing use. It uses a user definable PIT channel
//
static void fnUSB_delay(void (*timeout_handler)(void), unsigned long ulDelay)
{
    PIT_SETUP pit_setup;                                                 // interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.int_handler = timeout_handler;
    pit_setup.int_priority = USB_PIT_INTERRUPT_PRIORITY;
    pit_setup.count_delay = ulDelay;                                     // delay
    pit_setup.mode = PIT_SINGLE_SHOT;                                    // one-shot interrupt
    pit_setup.ucPIT = USB_HOST_PIT_CHANNEL;
    fnConfigureInterrupt((void *)&pit_setup);                            // enter interrupt
}


extern void fnUSB_ResetCycle(void)
{
    if ((INT_STAT & ATTACH) != 0) {                                      // a device has really been attached
        if ((CTL & JSTATE) == 0) {                                       // low speed device connected
            usb_hardware.ucUSBAddress = LS_EN;
            ADDR = LS_EN;                                                // select low speed mode
        }
        ENDPT0 = (HOST_WO_HUB | EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);  // enable control 0 endpoint transmission and reception for host usage
        INT_STAT = (USB_RST | SLEEP | RESUME_EN | USB_ERROR);            // reset flags that may be present after some signal bouncing at connection
        CTL |= USB_RESET;                                                // send reset to USB bus for a minimum of 10ms
        fnUSB_delay(usb_reset_timeout, PIT_MS_DELAY(12));
    }
    else {                                                               // spurious attach - activate the interrupt again
        INT_ENB = ATTACH_EN;                                             // enable an interrupt on the attach of a device
    }
}

// After a short delay after a possible attach, check the state and decide whether we need to continue with the attached device
//
static void usb_possible_attach(void)
{
    fnUSB_ResetCycle();
}

//unsigned long ulHostEndpoint = 0;

// Enable a single-shot SOF interrupt to handle the next host event
//
/*
extern void fnUSB_host_sync(QUEUE_HANDLE channel)
{
    INT_STAT = SOF_TOK;                                                  // reset pending SOF flag
    ulHostEndpoint |= (1 << channel);                                    // flag which endpoint is to be processed
    INT_ENB |= SOF_TOK_EN;                                               // SOF interrupt enabled
#if defined _WINDOWS
    fnSimUSB(USB_SIM_SOF, 0, &usb_hardware);
#endif
}*/

extern void fnHostReleaseBuffer(int iEndpoint, unsigned char ucTransferType, USB_HW *ptrUSB_HW)
{
    if (ucTransferType == SETUP_PID) {
        fnSendToken((unsigned char)((ucTransferType << 4) | iEndpoint)); // release the setup frame on the corresponding endpoint
      //fnUSB_delay(usb_timeout, PIT_MS_DELAY(10));                      // monitor the successful transmission
    }
    else if (ucTransferType == OUT_PID) {                                // OUT
        fnSendToken((unsigned char)((ucTransferType << 4) | iEndpoint)); // release the data frame on the corresponding endpoint
    }
    else {
        _EXCEPTION("Token to be added!!");
    }
    ptrUSB_HW->ucHostEndpointActive = (unsigned char)iEndpoint;          // remember the endpoint that the host is transmitting data on
}
    #endif


static void fnUSB_reset(int iError)
{
    INT_STAT = (USB_RST | SLEEP | RESUME_EN | USB_ERROR);                // reset flags
    CTL |= ODD_RST;                                                      // reset all odd BDTs which specifies the even bank
    INT_ENB &= ~(RESUME_EN | SLEEP_EN);                                  // disable resume and suspend interrupts
    usb_endpoints[0].ulEndpointSize &= ~ALTERNATE_TX_BUFFER;
    usb_endpoints[0].ulNextRxData0 = RX_DATA_TOGGLE;                     // DATA 1 is always first reception (as response to SETUP)
    CTL &= ~ODD_RST;
    ADDR = 0;                                                            // reset device address
    ENDPT0 = EP_HSHK;                                                    // disable transmission and reception
    ENDPT1 = 0; ENDPT2 = 0; ENDPT2 = 0; ENDPT4 = 0; ENDPT5 = 0; ENDPT6 = 0; ENDPT7 = 0; ENDPT8 = 0; ENDPT9 = 0; ENDPT10 = 0;
    ENDPT11 = 0; ENDPT2 = 0; ENDPT13 = 0; ENDPT14 = 0; ENDPT15 = 0;      // disable all other endpoints
    while (ENDPT0 & EP_TX_ENABLE) {}                                     // wait for tx disable to complete
                                                                         // reconfigure all packet buffers of the control endpoint
    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl  = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS | OWN); // set control endpoint rx size and configure Data Toggle Synchronisation (receive DATA 0 frames)
    ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1); // set control endpoint rx size and configure Data Toggle Synchronisation (Receive DATA 1 frames)
                
    #if defined USB_HOST_SUPPORT
    if (CTL & HOST_MODE_EN) {                                            // if a reset was detected in host mode it means that the device was removed
        fnUSB_handle_frame(USB_DEVICE_REMOVED, 0, 0, &usb_hardware);     // generic handler routine
        INT_ENB = ATTACH_EN;                                             // enable an interrupt on the attach of a device
      //uEnable_Interrupt();
        return;
    }
    fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware);         // generic handler routine
    #else
    fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware);         // generic handler routine
    #endif
    usb_hardware.ucUSBAddress = 0;                                       // reset the address to revert back to the default state
    ENDPT0 = (EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);                    // enable control endpoint reception and transmission
}

extern unsigned long fnUSB_error_counters(int iValue)                    // {71}
{
    switch (iValue) {
    case USB_CRC_5_ERROR_COUNTER:
        return USB_errors.ulUSB_errors_CRC5;
    case USB_CRC_16_ERROR_COUNTER:
        return USB_errors.ulUSB_errors_CRC16;
    case USB_ERRORS_RESET:
        uMemset(&USB_errors, 0, sizeof(USB_errors));
        break;
    }
    return 0;
}

// USB OTG Interrupt handler
//
static __interrupt void _usb_otg_isr(void)
{
    unsigned char ucUSB_Int_status; 
    
    while ((ucUSB_Int_status = (unsigned char)(INT_STAT & INT_ENB)) != 0) { // read present status
        if ((ucUSB_Int_status & ~TOK_DNE) != 0) {                        // check first for bus state changes
    #if defined USB_HOST_SUPPORT
            if ((ATTACH & ucUSB_Int_status) != 0) {                      // device has been attached
                INT_STAT = ATTACH;                                       // reset flag
                INT_ENB  = 0;                                            // disable further interrupts
                fnUSB_delay(usb_possible_attach, PIT_MS_DELAY(50));      // wait a short time before checking whether this was a real attach
            }
    #endif
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting messages to queue
            if ((ucUSB_Int_status & USB_RST) != 0) {                     // reset detected - D+ and D- in SE0 (single ended logic 0) state for > 2.5us
                fnUSB_reset(0);
            }
            if ((ucUSB_Int_status & USB_ERROR) != 0) {                   // error detected
              //if (ERR_STAT & BTO_ERR) {                                // bus turnaround timeout error
              //}
                if ((ERR_STAT & CRC16) != 0) {                           // CRC-16 error
                    USB_errors.ulUSB_errors_CRC16++;                     // count errors
            	}
                if ((ERR_STAT & CRC5_EOF) != 0) {                        // CRC-5 error
                    USB_errors.ulUSB_errors_CRC5++;                      // count errors
            	}
            	ERR_STAT = (CRC16 | CRC5_EOF | BTO_ERR);                 // clear error sources
                INT_STAT = USB_ERROR;                                    // reset flag
            }
            if ((ucUSB_Int_status & SLEEP) != 0) {                       // suspend state detected - 3ms of idle USB bus detected (low speed D+ = 0, D- = 1 / high speed D+ = 1, D- = 0)
                INT_STAT = (SLEEP | RESUME);                             // reset flag
                INT_ENB |= RESUME_EN;                                    // enable resume interrupt
                fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);       // generic handler routine
            }
            if ((ucUSB_Int_status & RESUME) != 0) {                      // resume detected - 10ms inverted idle USB bus state (low speed D+ = 1, D- = 0 / high speed D+ = 0, D- = 1)
                INT_STAT = (SLEEP | RESUME);                             // reset flag
                INT_ENB &= ~RESUME_EN;                                   // disable resume interrupt
                fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);        // generic handler routine
            }
            uEnable_Interrupt();
            INT_STAT = (unsigned char)(ucUSB_Int_status & ~TOK_DNE);     // reset all other flags
    #if defined _WINDOWS
            INT_STAT &= (~(ucUSB_Int_status & ~TOK_DNE) | ATTACH);       // leave ATTACH status in the register
    #endif
        }
        if ((ucUSB_Int_status & TOK_DNE) != 0) {                         // current processed token complete
            KINETIS_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;              // start of BDT
            KINETIS_USB_BD *ptUSB_BD;                                    // specific BD
            int iEndpoint_ref = (STAT >> END_POINT_SHIFT);               // the endpoint belonging to this event (this is always 0 in host mode)
            ptEndpointBD += iEndpoint_ref;                               // set to endpoint bd block as indicated by the status register
            if ((STAT & TX_TRANSACTION) != 0) {                          // determine the specific BD for the transaction
    #if defined USB_HOST_SUPPORT
                int iHostDo;
                usb_hardware.ptrEndpoint = &usb_endpoints[0];            // host uses only endpoint 0 for transmission
                if ((usb_hardware.ucBufferUsage & TX_TRANSACTION) == 0)  // if the last transmission on the fixed endpoint 0 was in the even buffer
    #else
                usb_hardware.ptrEndpoint = &usb_endpoints[iEndpoint_ref];         
                if ((usb_hardware.ptrEndpoint->ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) // if the last transmission on this endpoint was in the even buffer
    #endif   
                {                                                        // set a pointer to the buffer descriptor of the next transmission buffer to use
                    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl; // use even buffer for next transmission
                    usb_hardware.ptrDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data; // the buffer space associated with the even bufer
                }
                else {
                    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl; // use odd buffer for next transmission
                    usb_hardware.ptrDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data; // the buffer space associated with the odd bufer
                }
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when handling the next possible transmission
    #if defined USB_HOST_SUPPORT
                    iHostDo = fnUSB_handle_frame(USB_TX_ACKED, 0, usb_hardware.ucHostEndpointActive, &usb_hardware); // handle tx ack event
    //usb_endpoints[ucHostEndpointActive].ulNextTxData0 ^= DATA_1;      WHERE?          // toggle the data token for this endpoint ready for next use
    #else
                    fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpoint_ref, &usb_hardware); // handle ack event
    #endif
                uEnable_Interrupt();                                     // allow higher priority interrupts again
    #if defined USB_HOST_SUPPORT
                if ((CTL & HOST_MODE_EN) == 0) {                         // if in device mode
                    ADDR = usb_hardware.ucUSBAddress;                    // program the address to be used by device
                }
    #else
                ADDR = usb_hardware.ucUSBAddress;                        // program the address to be used 
    #endif
    #if defined USB_HOST_SUPPORT
                if (INITIATE_IN_TOKEN == iHostDo) {                      // if IN token polling is to be initiated
                    fnSendToken((unsigned char)((IN_PID << 4) | usb_hardware.ucHostEndpointActive)); // start transmission of IN token on the appropriate endpoint
                  //fnUSB_delay(usb_timeout, PIT_MS_DELAY(10));          // monitor the response reception
                }
    #endif
            }
            else {                                                       // receive packet
                if (STAT & ODD_BANK) {                                   // check whether odd or even bank and update local flag
                    ptUSB_BD = &ptEndpointBD->usb_bd_rx_odd;             // received data in odd buffer
    #if defined USB_HOST_SUPPORT                                         // host always receives on endpoint 0
                    usb_hardware.ptr_ulUSB_Rx_BDControl = &ptEndpointBD->usb_bd_rx_even.ulUSB_BDControl; // next reception will be in the even buffer
                    usb_hardware.ptr_ulUSB_Alt_Rx_BDControl = &ptEndpointBD->usb_bd_rx_odd.ulUSB_BDControl;
                    usb_hardware.ucBufferUsage &= ~(ODD_BANK);           // next reception will be in the even buffer
    #else
                    usb_hardware.ulRxControl = DATA_1;
    #endif
                }
                else {
                    ptUSB_BD = &ptEndpointBD->usb_bd_rx_even;            // data in even buffer
    #if defined USB_HOST_SUPPORT                                         // host always receives on endpoint 0
                    usb_hardware.ptr_ulUSB_Rx_BDControl = &ptEndpointBD->usb_bd_rx_odd.ulUSB_BDControl; // next reception will be in the odd buffer
                    usb_hardware.ptr_ulUSB_Alt_Rx_BDControl = &ptEndpointBD->usb_bd_rx_even.ulUSB_BDControl;
                    usb_hardware.ucBufferUsage |= ODD_BANK;              // next reception will be in the odd buffer
    #else
                    usb_hardware.ulRxControl = 0;
    #endif
                }
                usb_hardware.usLength = GET_FRAME_LENGTH();              // the length of the present frame
                switch (ptUSB_BD->ulUSB_BDControl & RX_PID_MASK) {
    #if defined USB_HOST_SUPPORT
                case (DATA1_PID << RX_PID_SHIFT):                        // host IN reception (DATA1) [ptUSB_BD->ulUSB_BDControl has DATA_1 set]
                case (DATA0_PID << RX_PID_SHIFT):                        // host IN reception (DATA0) [ptUSB_BD->ulUSB_BDControl has DATA_1 clear]
    #endif
                case (OUT_PID << RX_PID_SHIFT):                          // OUT frame - for any endpoint
                    {
                        unsigned long ulDataToggle;
                        if (usb_endpoints[usb_hardware.ucHostEndpointActive].ulEndpointSize & DTS) { // data toggling active on this endpoint (not isochronous)
                            ulDataToggle = (usb_endpoints[usb_hardware.ucHostEndpointActive].ulNextRxData0 & RX_DATA_TOGGLE); // the expected DATA
                            if ((ulDataToggle != 0) != ((ptUSB_BD->ulUSB_BDControl & DATA_1) != 0)) { // if the received is not synchronous to the data toggling it represents repeated data since an ACK from us has been lost
    #if defined _SUPRESS_REPEAT_DATA
                                unsigned long ulNextRxData0_required = usb_endpoints[iEndpoint_ref].ulNextRxData0;
                                fnProcessInput(iEndpoint_ref, &usb_hardware, USB_DATA_REPEAT, ptUSB_BD, ptEndpointBD);
                                usb_endpoints[iEndpoint_ref].ulNextRxData0 = ulNextRxData0_required; // ensure we remain synchron with read stream
                                break;
    #endif
                            }
                        }
                        else {
                            ulDataToggle = RX_DATA_TOGGLE;
                        }
    #if defined USB_HOST_SUPPORT
                        if (*fnGetEndPointCtr(usb_hardware.ucHostEndpointActive) & EP_CTL_DIS)
    #else
                        if (*fnGetEndPointCtr(iEndpoint_ref) & EP_CTL_DIS)
    #endif
                        { // check whether this endpoint is a non-control type
    #if defined USB_HOST_SUPPORT
                            if (CTL & HOST_MODE_EN) {
                                fnProcessInput(usb_hardware.ucHostEndpointActive, &usb_hardware, USB_OUT_FRAME, ptUSB_BD, ptEndpointBD); // non-control endpoint
                            }
                            else {
                                fnProcessInput(iEndpoint_ref, &usb_hardware, USB_OUT_FRAME, ptUSB_BD, ptEndpointBD); // non-control endpoint
                            }
    #else
                             fnProcessInput(iEndpoint_ref, &usb_hardware, USB_OUT_FRAME, ptUSB_BD, ptEndpointBD); // non-control endpoint
    #endif
                        }
                        else {                                           // control endpoint
    #if defined USB_HOST_SUPPORT
                            if (CTL & HOST_MODE_EN) {
                                fnProcessInput(usb_hardware.ucHostEndpointActive, &usb_hardware, USB_CONTROL_OUT_FRAME, ptUSB_BD, ptEndpointBD);
                            }
                            else {
                                fnProcessInput(iEndpoint_ref, &usb_hardware, USB_CONTROL_OUT_FRAME, ptUSB_BD, ptEndpointBD);
                            }
    #else
                            fnProcessInput(iEndpoint_ref, &usb_hardware, USB_CONTROL_OUT_FRAME, ptUSB_BD, ptEndpointBD);
    #endif
                        }
    #if defined USB_HOST_SUPPORT
                        usb_endpoints[usb_hardware.ucHostEndpointActive].ulNextRxData0 ^= (DATA_1); // invert the data token for next frame
    #else
                        if (ulDataToggle) {                              // synchronise the data toggle to detect repeated data
                            usb_endpoints[iEndpoint_ref].ulNextRxData0 &= ~(RX_DATA_TOGGLE);
                        }
                        else {
                            usb_endpoints[iEndpoint_ref].ulNextRxData0 |= RX_DATA_TOGGLE; 
                        }
    #endif
                    }
                    break;

                case (SETUP_PID << RX_PID_SHIFT):                        // set up PID - on control endpoint (only received at device)
                    ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = 0;    // disable all packet buffers
                    ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = 0;
                    if ((usb_hardware.ulRxControl & DATA_1) == 0) {      // synchronise the reception data buffers
                        ptrBDT->usb_bd_rx_even.ulUSB_BDControl = ((usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS))); // prepare buffer for next reception but don't free until consumed
                        ptrBDT->usb_bd_rx_odd.ulUSB_BDControl = (ptrBDT->usb_bd_rx_even.ulUSB_BDControl | DATA_1 | OWN); // free alternate buffer ready for next reception
                    }
                    else {
                        ptrBDT->usb_bd_rx_odd.ulUSB_BDControl = ((usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS))); // prepare buffer for next reception but don't free until consumed
                        ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1 | OWN); // free alternate buffer ready for next reception
                    }
                    usb_endpoints[iEndpoint_ref].ulNextTxData0 = (DATA_1 | DTS); // always a data 1 packet following a setup packet
                    
                  //CTL &= ~TXSUSPEND_TOKENBUSY;                         // allow SIE to continue token processing

                    if (fnProcessInput(iEndpoint_ref, &usb_hardware, USB_SETUP_FRAME, ptUSB_BD, ptEndpointBD) != MAINTAIN_OWNERSHIP) {
                        usb_endpoints[iEndpoint_ref].ulNextRxData0 |= (RX_DATA_TOGGLE);
                    }
                    CTL &= ~TXSUSPEND_TOKENBUSY;                         // {83} allow SIE to continue token processing
                    break;

    #if defined USB_HOST_SUPPORT
                case (STALL_PID << RX_PID_SHIFT):                        // the host has returned a stall
                    // Free the stalled reception buffer
                    //
                    if (*(usb_hardware.ptr_ulUSB_Rx_BDControl) & DATA_1) { // restore the buffer with correct DATA1 flag
                        ptUSB_BD->ulUSB_BDControl = 0;
                    }
                    else {
                        ptUSB_BD->ulUSB_BDControl = DATA_1;
                    }
                    ptUSB_BD->ulUSB_BDControl |= (OWN | (usb_endpoints[usb_hardware.ucHostEndpointActive].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS))); // re-enable reception on this endpoint
                    usb_endpoints[usb_hardware.ucHostEndpointActive].ulNextRxData0 = ptUSB_BD->ulUSB_BDControl; // last buffer processed
                    fnUSB_handle_frame(USB_HOST_STALL_DETECTED, 0, usb_hardware.ucHostEndpointActive, &usb_hardware); // inform about the stall on this endpoint
                    CTL &= ~TXSUSPEND_TOKENBUSY;                         // allow SIE to continue token processing
                    break;

                case (ACK_PID << RX_PID_SHIFT):
                    fnUSB_handle_frame(USB_HOST_ACK_PID_DETECTED, 0, usb_hardware.ucHostEndpointActive, &usb_hardware); // error code 0
                    break;
                case (NAK_PID << RX_PID_SHIFT):
                    fnUSB_handle_frame(USB_HOST_NACK_PID_DETECTED, 0, usb_hardware.ucHostEndpointActive, &usb_hardware); // error code 1
                    break;
                case (0x00 << RX_PID_SHIFT):                             // bus timeout
                    fnUSB_handle_frame(USB_HOST_BUS_TIMEOUT_DETECTED, 0, usb_hardware.ucHostEndpointActive, &usb_hardware); // error code 2
                    break;
                case (0x0f << RX_PID_SHIFT):                             // data error (data token received in the present buffer doesn't match that expected)
                    fnUSB_handle_frame(USB_HOST_DATA_ERROR_DETECTED, 0, usb_hardware.ucHostEndpointActive, &usb_hardware); // error code 3
                    break;
    #endif

                default:                                                 // if anything else is received it indicates an error so stall
                    ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL); // stall the endpoint
                    ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = (OWN | BDT_STALL);
                    fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);
                    break;
                }
            }
            INT_STAT = TOK_DNE;                                          // clear the flag (don't clear until processing has completed to ensure that the STAT register stays valid)
    #if defined _WINDOWS
            INT_STAT = 0;                                                // simulate fag being cleared
    #endif
        }
    }
}

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW **ptr_usb_hardware)
{
    KINETIS_USB_ENDPOINT_BD *ptEndpointBD;
    if ((*fnGetEndPointCtr(iEndpoint) & EP_TX_ENABLE) == 0) {            // if the endpoint is not enabled for transmission
        return ENDPOINT_NOT_ACTIVE;
    }
    *ptr_usb_hardware = &usb_hardware;
    ptEndpointBD = ptrBDT;                                               // start of BDT
        #if defined USB_HOST_SUPPORT
    if (usb_hardware.ucModeType & USB_HOST_MODE) {                       // in host mode
        if ((*(usb_hardware.ptr_ulUSB_BDControl) & OWN) != 0) {          // check whether the USB controller owns the transmit buffer descriptor
            return ENDPOINT_NOT_FREE;                                    // endpoint is presently busy so no data may be copied
        }
        if (iEndpoint != usb_hardware.ucHostEndpointActive) {            // change of endpoint
            ptEndpointBD += iEndpoint;
            usb_hardware.ptrEndpoint = &usb_endpoints[iEndpoint];        // the endpoint's characteristics
            usb_hardware.ucHostEndpointActive = iEndpoint;               // the newly active endpoint
            if (usb_endpoints[iEndpoint].ulNextRxData0 & DATA_1) {
                *(usb_hardware.ptr_ulUSB_Rx_BDControl) |= DATA_1;
                *(usb_hardware.ptr_ulUSB_Alt_Rx_BDControl) &= ~(DATA_1);
            }
            else {
                *(usb_hardware.ptr_ulUSB_Rx_BDControl) &= ~(DATA_1);
                *(usb_hardware.ptr_ulUSB_Alt_Rx_BDControl) |= DATA_1;
            }
        }
        return ENDPOINT_FREE;                                            // endpoint transmit buffer is free
    }
    else {
        usb_hardware.ptrEndpoint = &usb_endpoints[iEndpoint];
        ptEndpointBD += iEndpoint;                                       // device mode
    }
        #else
    ptEndpointBD += iEndpoint;
        #endif
    /*
    uDisable_Interrupt();
    if ((usb_hardware.ptrEndpoint->ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) { // the transmit buffer to be used
        usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl;
        usb_hardware.ptrDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
    }
    else {
        usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl;
        usb_hardware.ptrDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
    }
    if (*(usb_hardware.ptr_ulUSB_BDControl) & OWN) {                     // check whether the USB controller owns the transmit buffer descriptor
        uEnable_Interrupt();
        return ENDPOINT_NOT_FREE;                                        // endpoint is presently busy so no data may be copied
    }
    uEnable_Interrupt();*/
    _EXCEPTION("To combine with device mode operation");
    return ENDPOINT_FREE;                                                // endpoint transmit buffer is free
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength)
{
    KINETIS_USB_ENDPOINT_BD *ptrEndpointBDT = ptrBDT;
    unsigned long  ulControlContent = (DTS | DATA_1);                    // prepare Data Toggle Synchronisation as default
    unsigned char *ptrEndPointCtr = fnGetEndPointCtr(ucEndpointRef);     // get a pointer to the endpoint control register
    unsigned char  endpoint_config;
    ptrEndpointBDT += ucEndpointRef;                                     // set pointer to the endpoint's buffer descriptor
        #if defined USB_SIMPLEX_ENDPOINTS
    if (ENDPOINT_DISABLE == ucEndpointType) {                            // if this endpoint is to be disabled
        *ptrEndPointCtr = 0;                                             // disable endpoint transmitter and receiver
        while (*ptrEndPointCtr & EP_TX_ENABLE) {}                        // wait for tx disable to complete
        ptrEndpointBDT->usb_bd_tx_odd.ulUSB_BDControl = 0;               // reset endpoint buffer descriptor control entries
        ptrEndpointBDT->usb_bd_tx_even.ulUSB_BDControl = 0;
        ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl = 0;
        ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = 0;
        usb_endpoints[ucEndpointRef].ulEndpointSize = 0;
        usb_endpoints[ucEndpointRef].ulNextRxData0 = 0;
        INT_ENB &= ~(SLEEP_EN | RESUME_EN);                              // disable suspend and resume interrupts when not configured
        *ptrEndPointCtr = 0;
        return;                                                          // disabled so return
    }
    INT_ENB |= SLEEP_EN;                                                 // since we have just been configured, enable suspend interrupt
    endpoint_config = *ptrEndPointCtr;                                   // get the original configuration setting

    if ((ucEndpointType & ~IN_ENDPOINT) == ENDPOINT_ISOCHRONOUS) {       // is this an isochronous endpoint?
        endpoint_config = (RETRY_DIS | EP_TX_ENABLE);                    // no retries on isochronous endpoint
        ulControlContent = 0;                                            // disable data toggle synchronisation on receiver
        usb_endpoints[ucEndpointRef].ulNextTxData0 = 0;                  // disable data toggle synchronisation on transmitter
    }
    else {
        usb_endpoints[ucEndpointRef].ulNextTxData0 = DTS;                // set data toggle synchronisation on transmission
        endpoint_config |= (EP_HSHK);                                    // bulk/control/interrupt endpoint - handshake enabled
    }
        #if defined USB_HOST_SUPPORT
    if (usb_hardware.ucModeType & USB_HOST_MODE) {
        ucEndpointType ^= IN_ENDPOINT;                                   // in host mode reverse the direction of the trasnmission/reception
    }
        #endif
    if ((ucEndpointType & IN_ENDPOINT) == 0) {                           // OUT type endpoint
        if (ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data == 0) {        // if the endpoint is being used for the first time
            if (usMaxEndpointLength > usEndpointLength) {                // if no larger specified take the specified value
                usEndpointLength = usMaxEndpointLength;
            }
            ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data = (unsigned char *)USB_FS_MALLOC(usEndpointLength); // alloc buffer memory for control endpoint
            ptrEndpointBDT->usb_bd_rx_odd.ptrUSB_BD_Data  = (unsigned char *)USB_FS_MALLOC(usEndpointLength);
        }
        endpoint_config |= EP_RX_ENABLE;                                 // enable reception on this endpoint
        ulControlContent |= SET_FRAME_LENGTH(usEndpointLength);          // set endpoint rx size                                     
        ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl = (ulControlContent | OWN); // odd buffer for DATA1 frames
        ulControlContent &= ~DATA_1;                                     // even buffer for DATA 0 frames
        ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = (ulControlContent | OWN); // allow endpoints to receive
        usb_endpoints[ucEndpointRef].ulEndpointSize = ulControlContent;
    }
    else {
        endpoint_config |= EP_TX_ENABLE;                                 // enable transmission on IN endpoint
    }
    if ((ucEndpointType & ~(IN_ENDPOINT)) != ENDPOINT_CONTROL) {         // non-control endpoint
        endpoint_config |= EP_CTL_DIS;                                   // not control endpoint
    }
        #else
    *ptrEndPointCtr &= ~(EP_TX_ENABLE | EP_RX_ENABLE);                   // disable endpoint transmitter and receiver
    while (*ptrEndPointCtr & EP_TX_ENABLE) {}                            // wait for tx disable to complete
    ptrEndpointBDT->usb_bd_tx_odd.ulUSB_BDControl = 0;                   // reset endpoint buffer descriptor control entries
    ptrEndpointBDT->usb_bd_tx_even.ulUSB_BDControl = 0;
    ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl = 0;
    ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = 0;
    usb_endpoints[ucEndpointRef].ulEndpointSize = 0;
    usb_endpoints[ucEndpointRef].ulNextRxData0 = 0;

    if (ENDPOINT_DISABLE == ucEndpointType) {                            // if this endpoint is to be disabled
        INT_ENB &= ~(SLEEP_EN | RESUME_EN);                              // disable suspend and resume interrupts when not configured
        *ptrEndPointCtr = 0;
        return;                                                          // disabled so return
    }
    INT_ENB |= SLEEP_EN;                                                 // since we have just been configured, enable suspend interrupt

    if ((ucEndpointType & ~IN_ENDPOINT) == ENDPOINT_ISOCHRONOUS) {       // is this an isochronous endpoint?
        endpoint_config = (RETRY_DIS | EP_TX_ENABLE);                    // no retries on isochronous endpoint
        ulControlContent = 0;                                            // disable data toggle synchronisation on receiver
        usb_endpoints[ucEndpointRef].ulNextTxData0 = 0;                  // disable data toggle synchronisation on transmitter
    }
    else {
        usb_endpoints[ucEndpointRef].ulNextTxData0 = DTS;                // set data toggle synchronisation on transmission
        endpoint_config = (EP_HSHK | EP_TX_ENABLE);                      // bulk/control/interrupt endpoint - handshake enabled
    }
        #if defined USB_HOST_SUPPORT
    if (usb_hardware.ucModeType & USB_HOST_MODE) {
        ucEndpointType ^= IN_ENDPOINT;                                   // in host mode reverse the direction of the transmission/reception
    }
        #endif
    if ((ucEndpointType & IN_ENDPOINT) == 0) {                           // OUT type endpoint
        if (ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data == 0) {        // if the endpoint is being used for the first time
            if (usMaxEndpointLength > usEndpointLength) {                // if no larger specified take the specified value
                usEndpointLength = usMaxEndpointLength;
            }
        #if defined USB_HOST_SUPPORT
            if ((usb_hardware.ucModeType & USB_HOST_MODE) == 0) {
                ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data = (unsigned char *)USB_FS_MALLOC(usEndpointLength); // alloc buffer memory for control endpoint
                ptrEndpointBDT->usb_bd_rx_odd.ptrUSB_BD_Data  = (unsigned char *)USB_FS_MALLOC(usEndpointLength);
            }
        #else
            ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data = (unsigned char *)USB_FS_MALLOC(usEndpointLength); // alloc buffer memory for control endpoint
            ptrEndpointBDT->usb_bd_rx_odd.ptrUSB_BD_Data  = (unsigned char *)USB_FS_MALLOC(usEndpointLength);
        #endif
        }
        endpoint_config |= EP_RX_ENABLE;                                 // enable reception on this endpoint
        ulControlContent |= SET_FRAME_LENGTH(usEndpointLength);          // set endpoint rx size                                     
        ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl = (ulControlContent | OWN); // odd buffer for DATA1 frames
        ulControlContent &= ~DATA_1;                                     // even buffer for DATA 0 frames
        ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = (ulControlContent | OWN); // allow endpoints to receive
        usb_endpoints[ucEndpointRef].ulEndpointSize = ulControlContent;
    }
    if ((ucEndpointType & ~IN_ENDPOINT) != ENDPOINT_CONTROL) {           // non-control endpoint
        endpoint_config |= EP_CTL_DIS;                                   // not control endpoint
        if ((ucEndpointType & IN_ENDPOINT) == 0) {                       // out type
            endpoint_config &= ~EP_TX_ENABLE;                            // disable transmission
        }
    }
        #endif
    *ptrEndPointCtr = endpoint_config;                                   // set the endpoint mode
    _SIM_USB(USB_SIM_ENUMERATED,0,0);                                    // inform the simulator that USB enumeration has completed
}

// This routine is called with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    unsigned short usLength;
    unsigned long ulBuffer;
    KINETIS_USB_BD *ptUSB_BD;                                             // specific BD
    KINETIS_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;                       // start of BDT
    ptEndpointBD += ucEndpointRef;
    if (usb_endpoints[ucEndpointRef].ulNextRxData0 & DATA_1) {
        ptUSB_BD = &ptEndpointBD->usb_bd_rx_even;
        ulBuffer = 0;
    }
    else {
        ptUSB_BD = &ptEndpointBD->usb_bd_rx_odd;
        ulBuffer = DATA_1;
    }
    if (ptUSB_BD->ulUSB_BDControl & OWN) {                               // no data available
        return USB_BUFFER_NO_DATA;
    }
    usLength = GET_FRAME_LENGTH();                                       // the number of bytes waiting in the buffer
    if (fnEndpointData(ucEndpointRef, ptUSB_BD->ptrUSB_BD_Data, usLength, OUT_DATA_RECEPTION) != MAINTAIN_OWNERSHIP) {
        ulBuffer |= (usb_endpoints[ucEndpointRef].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS));// reception buffer size
        ptUSB_BD->ulUSB_BDControl = (ulBuffer | OWN);                    // re-enable reception on this endpoint
        usb_endpoints[ucEndpointRef].ulNextRxData0 &= (RX_DATA_TOGGLE);  // don't disturb this flag
        usb_endpoints[ucEndpointRef].ulNextRxData0 |= ulBuffer;          // last buffer processed
        return USB_BUFFER_FREED;                                         // buffer consumed and freed
    }
    return USB_BUFFER_BLOCKED;
}

// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
    #if defined USB_BDT_FIXED
        #if defined _WINDOWS
    static KINETIS_USB_ENDPOINT_BD USB_BDT_RAM[NUMBER_OF_USB_ENDPOINTS * 2];
    static KINETIS_USB_ENDPOINT_BD *__USB_BDT_RAM = USB_BDT_RAM;
        #else
    extern KINETIS_USB_ENDPOINT_BD __USB_BDT_RAM[];
        #endif
    #endif
    unsigned char ucEndpoints = (pars->ucEndPoints + 1);                 // endpoint count, including endpoint 0
    #if defined USE_USB_OTG_CHARGE_PUMP
    QUEUE_HANDLE iic_handle;
    #endif
    #if defined USB_HS_INTERFACE                                         // {33}
    if (pars->usConfig & USB_HIGH_SPEED) {                               // use high speed controller rather than the full-speed controller
        usb_hardware.ucDeviceType = USB_DEVICE_HS;

        MPU_CESR = 0;                                                    // allow concurrent access to MPU controller
        FMC_PFAPR |= FMC_FPAPR_USB_HS;                                   // allow USBHS controller to read from Flash

        POWER_UP(6, SIM_SCGC6_USBHS);                                    // power up the USB HS controller module

        _CONFIG_PERIPHERAL(A, 7,  PA_7_ULPI_DIR);                        // ULPI_DIR on PA.7    (alt. function 2)
        _CONFIG_PERIPHERAL(A, 8,  PA_8_ULPI_NXT);                        // ULPI_NXT on PA.8    (alt. function 2)
        _CONFIG_PERIPHERAL(A, 10, PA_10_ULPI_DATA0);                     // ULPI_DATA0 on PA.10 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 11, PA_11_ULPI_DATA1);                     // ULPI_DATA1 on PA.11 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 24, PA_24_ULPI_DATA2);                     // ULPI_DATA2 on PA.24 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 25, PA_25_ULPI_DATA3);                     // ULPI_DATA3 on PA.25 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 26, PA_26_ULPI_DATA4);                     // ULPI_DATA4 on PA.26 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 27, PA_27_ULPI_DATA5);                     // ULPI_DATA5 on PA.27 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 28, PA_28_ULPI_DATA6);                     // ULPI_DATA6 on PA.28 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 29, PA_29_ULPI_DATA7);                     // ULPI_DATA7 on PA.29 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 6,  PA_6_ULPI_CLK);                        // ULPI_CLK on PA.6    (alt. function 2)
        _CONFIG_PERIPHERAL(A, 9,  PA_9_ULPI_STP);                        // ULPI_STP on PA.9    (alt. function 2)

        if (ucEndpoints > NUMBER_OF_USBHS_ENDPOINTS) {                   // limit endpoint count
            ucEndpoints = NUMBER_OF_USBHS_ENDPOINTS;                     // limit to maximum available in device
        }

        usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * ucEndpoints)); // get endpoint control structures
        fnUSBHS_init(ucEndpoints);
        return;
    }
    #endif
    #if !defined KINETIS_KL
    FMC_PFAPR |= FMC_FPAPR_USB_FS;                                       // allow USB controller to read from Flash
    #endif
    SIM_SOPT1_SET(SIM_SOPT1_USBREGEN, SIM_SOPT1CFG_URWE);                // ensure USB regulator is enabled
    SIM_SOPT1_CLR(SIM_SOPT1_USBSTBY, SIM_SOPT1CFG_UVSWE);                // and not in standby
    switch (pars->ucClockSource) {                                       // set USB clock source
    case INTERNAL_USB_CLOCK:
    #if (defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000)) && !defined KINETIS_K21 && !defined KINETIS_K22 && !defined KINETIS_K24 && !defined KINETIS_K64
        #if defined USB_CLOCK_SOURCE_MCGPLL0CLK
            #define USB_CLOCK_SOURCE   MCGPLLCLK
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC | SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_USBFSRC_MCGPLL0); // set the source to MCGPLL0CLK
        #elif defined USB_CLOCK_SOURCE_MCGPLL1CLK                        // {90} use PLL1, dedicated for peripheral use
            #define USB_CLOCK_SOURCE   MCGPLL1CLK                        // use PLL1
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC | SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_USBFSRC_MCGPLL1); // set the source to MCGPLL1CLK
        #else
            #define USB_CLOCK_SOURCE   MCGPLLCLK
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC | SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_USBFSRC_MCGPLLCLK); // set the source to MCGPLLCLK
        #endif
    #elif defined KINETIS_HAS_IRC48M && defined USB_CRYSTAL_LESS         // {104}
        #define USB_CLOCK_SOURCE   48000000
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC | SIM_SOPT2_PLLFLLSEL_IRC48M);    // set the source to IRC48M
    #else
        #define USB_CLOCK_SOURCE   MCGPLLCLK                             // fixed clock source for USB
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC | SIM_SOPT2_PLLFLLSEL);           // set the source to MCGPLLCLK
    #endif
    #if defined KINETIS_KL                                               // {67}
        #if defined KINETIS_HAS_IRC48M && defined USB_CRYSTAL_LESS       // {104}
        #elif defined _WINDOWS
            #if (MCGPLLCLK/2) != 48000000                                // check that the clock has the correct frequency for USB operation
                #error "USB clock not 48MHz!!"
            #endif
        #endif
    #else
        #if defined KINETIS_HAS_IRC48M && defined USB_CRYSTAL_LESS       // {104}
        SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV_1;                              // no divide when IRC48M is used
        #elif (USB_CLOCK_SOURCE/48000000) > 4
        SIM_CLKDIV2 |= (((USB_CLOCK_SOURCE/48000000) - 1) << 1);         // the value of the clock source must be chosen to allow 48MHz to be achieved - possible without fraction
        #else
        SIM_CLKDIV2 |= (((((USB_CLOCK_SOURCE * 2)/48000000) - 1) << 1) | SIM_CLKDIV2_USBFRAC); // the value of the clock source must be chosen to allow 48MHz to be achieved!
        #endif
        #if defined _WINDOWS                                             // check that the clock rate programmed is accurate for USB
        if (SIM_CLKDIV2 & SIM_CLKDIV2_USBFRAC) {
            if (((USB_CLOCK_SOURCE/(((SIM_CLKDIV2 >> 1) & 0x07) + 1)) * 2) != 48000000) {
                unsigned long _USB_CLOCK_SOURCE = USB_CLOCK_SOURCE;
                unsigned long _SIM_CLKDIV2 = SIM_CLKDIV2;
                _EXCEPTION("USB Clock not 48MHz!!!!!!");
            }
        }
        else {
            if ((USB_CLOCK_SOURCE/(((SIM_CLKDIV2 >> 1) & 0x07) + 1)) != 48000000) {
                unsigned long _USB_CLOCK_SOURCE = USB_CLOCK_SOURCE;
                unsigned long _SIM_CLKDIV2 = SIM_CLKDIV2;
                _EXCEPTION("USB Clock not 48MHz!!!!!!");
            }
        }
        #endif
    #endif
        break;
  //case EXTERNAL_USB_CLOCK:
  //case SPECIAL_USB_CLOCK:                                              // use alternative clock input pin
    default:
        _CONFIG_PERIPHERAL(E, 26, PE_26_USB_CLKIN);                      // USB_CLKIN on PE.26 (alt. function 7)
        break;
    }
    #if KINETIS_MAX_SPEED >= 100000000                                   // devices with less that 100MHz speed don't have memory protection unit
    MPU_CESR = 0;                                                        // allow concurrent access to MPU controller
    #endif
    #if defined KINETIS_K64 && (defined RUN_FROM_HIRC_PLL || defined RUN_FROM_HIRC)
    if (IS_POWERED_UP(4, SIM_SCGC4_USBOTG)) {                            // if the USB module is already powered up it means that it has to be enabled to turn on the IRC48M
        iIRC48M_workaround = 1;                                          // mark that we need to temporarity switch system clock source during the USB reset command
    }
    #endif
    POWER_UP(4, SIM_SCGC4_USBOTG);                                       // power up the USB controller module

    if (ucEndpoints > NUMBER_OF_USB_ENDPOINTS) {                         // limit endpoint count
        ucEndpoints = NUMBER_OF_USB_ENDPOINTS;                           // limit to maximum available in device
    }

    usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * ucEndpoints)); // get endpoint control structures
    USB_USBTRC0 |= USB_USBTRC0_USBRESET;                                 // reset USB controller
    while (USB_USBTRC0 & USB_USBTRC0_USBRESET) {                         // wait for the reset to complete
    #if defined _WINDOWS
        USB_USBTRC0 = 0;
    #endif
    }
    CTL = ODD_RST;                                                       // reset all odd BDTs which specifies the even bank
  //USB_CTRL = PDE;                                                      // take transceiver out of suspend mode (monitor 2.2.2014)

    #if defined USB_BDT_FIXED
        #if defined _WINDOWS                                             // set buffer to 512 byte aligned boundary
    __USB_BDT_RAM = (KINETIS_USB_ENDPOINT_BD *)(((CAST_POINTER_ARITHMETIC)__USB_BDT_RAM) & 0xffffff00);
    if (__USB_BDT_RAM < USB_BDT_RAM)  {
        __USB_BDT_RAM = (KINETIS_USB_ENDPOINT_BD *)(((CAST_POINTER_ARITHMETIC)(__USB_BDT_RAM + NUMBER_OF_USB_ENDPOINTS)) & 0xffffff00);
    }
        #endif
    ptrBDT = __USB_BDT_RAM;
    uMemset(ptrBDT, 0x00, (ucEndpoints * ENDPOINT_BD_SIZE));
    #else
    ptrBDT = uMallocAlign((MAX_MALLOC)(ucEndpoints * ENDPOINT_BD_SIZE), 512); // get BDT - buffer descriptor table memory (zeroed) - aligned to 512 byte boundars
    #endif

    BDT_PAGE_01 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrBDT >> 8); // enter the BDT location
    BDT_PAGE_02 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrBDT >> 16);
    BDT_PAGE_03 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrBDT >> 24);

    ADDR = 0;                                                            // set default address
    CTL = 0;                                                             // reset complete
                                                                         
    ENDPT0 = 0;                                                          // configure endpoint 0 - the only one used during enumeration: USB then in default state
    while (ENDPT0 & EP_TX_ENABLE) {}                                     // wait for tx disable to complete
                                                                         // configure rx side of endpoint
    ptrBDT->usb_bd_rx_even.ptrUSB_BD_Data  = (unsigned char *)uMalloc(64); // alloc buffer memory for control endpoint (the size must be full-sized since this is used for all endpoints in host mode)
    ptrBDT->usb_bd_rx_odd.ptrUSB_BD_Data   = (unsigned char *)uMalloc(64);
    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl  = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS | OWN); // set endpoint rx size and configure Data Toggle Synchronisation (receive DATA 0 frames)
    ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1); // set endpoint rx size and configure Data Toggle Synchronisation (receive DATA 1 frames)
    usb_endpoints[0].ulEndpointSize = (SET_FRAME_LENGTH(64) | DTS);
    usb_endpoints[0].ulNextRxData0 = RX_DATA_TOGGLE;                     // DATA 1 is always first reception (as response to SETUP)
    ENDPT0 = (EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);                    // enable control endpoint transmission and reception

    OTG_INT_STAT = (A_VBUS_CHG | BSESS_CHG | SESS_VLD_CHG | LINE_STAT_CHG | MSEC_1 | ID_CHG); // reset any pending OTG interrupts
    INT_STAT = (STALL | ATTACH | RESUME | SLEEP | TOK_DNE | SOF_TOK | USB_ERROR | USB_RST); // reset any pending USB interrupts
    ERR_STAT = (PID_ERR | CRC5_EOF | CRC16 | DFN8 | BTO_ERR | USB_DMA_ERR | BTS_ERR); // reset any pending USB error interrupts
    #if defined _WINDOWS
    INT_STAT = ERR_STAT = 0;
    #endif

    fnEnterInterrupt(irq_USB_OTG_ID, PRIORITY_USB_OTG, _usb_otg_isr);    // configure and enter the USB handling interrupt routine in the vector table

    CTL = HOST_MODE_EN;                                                  // enable host mode operation (pull-down resistors enabled) but don't generate SOF (SOF counter loaded with 12000)
    USB_CTRL = PDE;                                                      // take transceiver out of suspend mode with pull-down resistors enabled
    OTG_CTRL = (DP_LOW_DM_LOW | OTG_EN);                                 // pull down D+ and D- with weak pulldowns
    SOF_THLD = 74;                                                       // temp worst case threshold value for 64 bytes
    INT_ENB = ATTACH_EN;                                                 // enable an interrupt on the attach of a device
    USB_HOST_POWER_ON();                                                 // enable host power (5V)
    usb_hardware.ucModeType = USB_HOST_MODE;                             // mark that we are in host mode
}
