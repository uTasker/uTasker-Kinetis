/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_USB_Device.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2015
    *********************************************************************
    05.10.2015 fnGetUSB_HW() modification for compatibility with host mode

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

/* =================================================================== */
/*                                USB                                  */
/* =================================================================== */


#define fnGetEndPointCtr(iEndPoint) (unsigned char *)(ENDPT0_ADD + ((iEndPoint) * sizeof(unsigned long)))

// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *usb_hardware, unsigned char ucFrameType, KINETIS_USB_BD *ptUSB_BD_rx, KINETIS_USB_ENDPOINT_BD *ptEndpointBD)
{
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting messages to queue

    usb_hardware->ptrEndpoint = &usb_endpoints[iEndpoint_ref];           // mark the present transmit endpoint information for possible subroutine or response use
    if ((usb_endpoints[iEndpoint_ref].ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) {  // set the next transmit pointer details
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
        usb_hardware->ptrEndpoint->ulEndpointSize ^= ALTERNATE_TX_BUFFER;
        ptUSB_BD_rx->ulUSB_BDControl |= (OWN);                           // re-enable reception for this endpoint buffer since it has been freed
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        uEnable_Interrupt();
        return MAINTAIN_OWNERSHIP;
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
        usb_hardware->ulRxControl |= (usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS)); // reception buffer size
        ptUSB_BD_rx->ulUSB_BDControl = (OWN | usb_hardware->ulRxControl); // re-enable reception on this endpoint
        usb_endpoints[iEndpoint_ref].ulNextRxData0 = usb_hardware->ulRxControl; // last buffer processed
        break;
    }
    uEnable_Interrupt();
    return 0;
}

// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
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
        if (ucUSB_Int_status & ~TOK_DNE) {
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting messages to queue
            if (ucUSB_Int_status & USB_RST) {                            // reset detected - D+ and D- in SE0 (single ended logic 0) state for > 2.5us
                INT_STAT = (USB_RST | SLEEP | RESUME_EN | USB_ERROR);    // reset flags
                CTL |= ODD_RST;                                          // reset all odd BDTs which specifies the even bank
                INT_ENB &= ~(RESUME_EN | SLEEP_EN);                      // disable resume and suspend interrupts
                usb_endpoints[0].ulEndpointSize &= ~ALTERNATE_TX_BUFFER;
                usb_endpoints[0].ulNextRxData0 = 0;                      // DATA 0 is always first reception (in SETUP)
                CTL &= ~ODD_RST;
                ADDR = 0;                                                // reset device address
                ENDPT0 = EP_HSHK;                                        // disable transmission and reception
                ENDPT1 = 0; ENDPT2 = 0; ENDPT2 = 0; ENDPT4 = 0; ENDPT5 = 0; ENDPT6 = 0; ENDPT7 = 0; ENDPT8 = 0; ENDPT9 = 0; ENDPT10 = 0;
                ENDPT11 = 0; ENDPT2 = 0; ENDPT13 = 0; ENDPT14 = 0; ENDPT15 = 0;// disable all other endpoints
                while (ENDPT0 & EP_TX_ENABLE) {}                         // wait for tx disable to complete
                                                                         // reconfigure all packet buffers of the control endpoint
                ptrBDT->usb_bd_rx_odd.ulUSB_BDControl  = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS | OWN);  // set control endpoint rx size and configure Data Toggle Synchronisation (receive DATA 0 frames)
                ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1); // set control endpoint rx size and configure Data Toggle Synchronisation (Receive DATA 1 frames)
                
                fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware); // generic handler routine
                usb_hardware.ucUSBAddress = 0;                           // reset the address to revert back to the default state
                ENDPT0 = (EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);        // enable control endpoint reception and transmission
            }
            if (ucUSB_Int_status & USB_ERROR) {                          // error detected
                if (ERR_STAT & CRC16) {                                  // {71} CRC-16 error
                    USB_errors.ulUSB_errors_CRC16++;            		 // count errors
            	}
                if (ERR_STAT & CRC5_EOF) {                               // CRC-5 error
                    USB_errors.ulUSB_errors_CRC5++;            		     // count errors
            	}
            	ERR_STAT = (CRC16 | CRC5_EOF);                           // clear error sources
                INT_STAT = USB_ERROR;                                    // reset flag
            }
            if (ucUSB_Int_status & SLEEP) {                              // suspend state detected - 3ms of idle USB bus detected (low speed D+ = 0, D- = 1 / high speed D+ = 1, D- = 0)
                INT_STAT = (SLEEP | RESUME);                             // reset flag
                INT_ENB |= RESUME_EN;                                    // enable resume interrupt
                fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);       // generic handler routine
            }
            if (ucUSB_Int_status & RESUME) {                             // resume detected - 10ms inverted idle USB bus state (low speed D+ = 1, D- = 0 / high speed D+ = 0, D- = 1)
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
        if (ucUSB_Int_status & TOK_DNE) {                                // current processed token complete
            KINETIS_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;              // start of BDT
            KINETIS_USB_BD *ptUSB_BD;                                    // specific BD
            int iEndpoint_ref = (STAT >> END_POINT_SHIFT);               // the endpoint belonging to this event
            ptEndpointBD += iEndpoint_ref;                               // set to endpoint bd block as indicated by the status register
            if (STAT & TX_TRANSACTION) {                                 // determine the specific BD for the transaction
                usb_hardware.ptrEndpoint = &usb_endpoints[iEndpoint_ref];            
                if ((usb_hardware.ptrEndpoint->ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) { // set a pointer to the buffer descriptor of the next transmission buffer to use
                    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl;
                    usb_hardware.ptrDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
                }
                else {
                    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl;
                    usb_hardware.ptrDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
                }
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when handling the next possible transmission
                fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpoint_ref, &usb_hardware); // handle ack event
                uEnable_Interrupt();
                ADDR = usb_hardware.ucUSBAddress;                        // program the address to be used 
            }
            else {                                                       // receive packet
                if (STAT & ODD_BANK) {                                   // check whether odd or even bank and update local flag
                    ptUSB_BD = &ptEndpointBD->usb_bd_rx_odd;             // received data in odd buffer
                    usb_hardware.ulRxControl = DATA_1;
                }
                else {
                    ptUSB_BD = &ptEndpointBD->usb_bd_rx_even;            // data in even buffer
                    usb_hardware.ulRxControl = 0;
                }
                usb_hardware.usLength = GET_FRAME_LENGTH();              // the length of the present frame
                switch (ptUSB_BD->ulUSB_BDControl & RX_PID_MASK) {
                case (OUT_PID << RX_PID_SHIFT):                          // OUT frame - for any endpoint
                    {
                        unsigned long ulDataToggle;
                        if (usb_endpoints[iEndpoint_ref].ulEndpointSize & DTS) { // data toggling active (not isochronous)
                            ulDataToggle = (usb_endpoints[iEndpoint_ref].ulNextRxData0 & RX_DATA_TOGGLE); // the expected DATA
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

                        if (*fnGetEndPointCtr(iEndpoint_ref) & EP_CTL_DIS) {
                             fnProcessInput(iEndpoint_ref, &usb_hardware, USB_OUT_FRAME, ptUSB_BD, ptEndpointBD); // non-control endpoint
                        }
                        else {                                           // control endpoint
                            fnProcessInput(iEndpoint_ref, &usb_hardware, USB_CONTROL_OUT_FRAME, ptUSB_BD, ptEndpointBD);
                        }
                        if (ulDataToggle) {                              // synchronise the data toggle to detect repeated data
                            usb_endpoints[iEndpoint_ref].ulNextRxData0 &= ~(RX_DATA_TOGGLE);
                        }
                        else {
                            usb_endpoints[iEndpoint_ref].ulNextRxData0 |= RX_DATA_TOGGLE; 
                        }
                    }
                    break;

                case (SETUP_PID << RX_PID_SHIFT):                        // set up PID - on control endpoint 
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

                default:                                                 // if anything else is received it indicates an error so stall
                    ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL); // stall the endpoint
                    ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = (OWN | BDT_STALL);
                    fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);
                    break;
                }
            }
            INT_STAT = TOK_DNE;                                          // clear the flag (don't clear until processing has completed to ensure that the STAT register stays valid)
    #if defined _WINDOWS
            INT_STAT &= ~(TOK_DNE);
    #endif
        }
    }
}

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW **ptr_usb_hardware)
{
#if 1
    KINETIS_USB_ENDPOINT_BD *ptEndpointBD;
    USB_END_POINT *ptrThisEndpoint;
    unsigned long *ptrThisControl;
    unsigned char **ptrThisBuffer;
    if ((*fnGetEndPointCtr(iEndpoint) & EP_TX_ENABLE) == 0) {            // if the endpoint is not enabled for transmission
        return ENDPOINT_NOT_ACTIVE;
    }
    ptEndpointBD = ptrBDT;                                               // start of BDT
    ptEndpointBD += iEndpoint;                                           // move to the buffer descriptor for this endpoint
    ptrThisEndpoint = &usb_endpoints[iEndpoint];
    (*ptr_usb_hardware)->ptrEndpoint = ptrThisEndpoint;                  // this is modified even if the endpoint is not free
    uDisable_Interrupt();                                                // protect from interrupts
        if ((ptrThisEndpoint->ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) { // the transmit buffer to be used
            ptrThisControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl;
            ptrThisBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
        }
        else {
            ptrThisControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl;
            ptrThisBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
        }
        if (*ptrThisControl & OWN) {                                     // check whether the USB controller owns the transmit buffer descriptor
            uEnable_Interrupt();
            return ENDPOINT_NOT_FREE;                                    // endpoint is presently busy so no data may be copied
        }    
        (*ptr_usb_hardware)->ptr_ulUSB_BDControl = ptrThisControl;       // update the hardware pointers (only when the endpoint is free)
        (*ptr_usb_hardware)->ptrDatBuffer = ptrThisBuffer;
    uEnable_Interrupt();
#else
    KINETIS_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;                      // start of BDT
    if ((*fnGetEndPointCtr(iEndpoint) & EP_TX_ENABLE) == 0) {            // if the endpoint is not enabled for transmission
        return ENDPOINT_NOT_ACTIVE;
    }
    ptr_usb_hardware->ptrEndpoint = &usb_endpoints[iEndpoint];
    ptEndpointBD += iEndpoint;
    uDisable_Interrupt();
    if ((ptr_usb_hardware->ptrEndpoint->ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) { // the transmit buffer to be used
        ptr_usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl;
        ptr_usb_hardware->ptrDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
    }
    else {
        ptr_usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl;
        ptr_usb_hardware->ptrDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
    }
    if (*ptr_usb_hardware->ptr_ulUSB_BDControl & OWN) {                  // check whether the USB controller owns the transmit buffer descriptor
        uEnable_Interrupt();
        return ENDPOINT_NOT_FREE;                                        // endpoint is presently busy so no data may be copied
    }
    uEnable_Interrupt();
#endif
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
    #if defined KINETIS_K64 && (defined RUN_FROM_HIRC_PLL || defined RUN_FROM_HIRC)
    int iIRC48M_workaround = 0;
    unsigned char original_MCG_C1 = MCG_C1;                              // note the original register settings for potential restoral later
    unsigned char original_MCG_C2 = MCG_C2;
    unsigned char original_MCG_SC = MCG_SC;
    #endif
    unsigned char ucEndpoints = (pars->ucEndPoints + 1);                 // endpoint count, including endpoint 0
    #if defined USE_USB_OTG_CHARGE_PUMP
    QUEUE_HANDLE iic_handle;
    #endif
    #if !defined KINETIS_KL
    FMC_PFAPR |= FMC_FPAPR_USB_FS;                                       // allow USB controller to read from Flash
    #endif
    SIM_SOPT1_SET(SIM_SOPT1_USBREGEN, SIM_SOPT1CFG_URWE);                // ensure USB regulator is enabled
    SIM_SOPT1_CLR(SIM_SOPT1_USBSTBY, SIM_SOPT1CFG_UVSWE);                // and not in standby
    switch (pars->ucClockSource) {                                       // set USB clock source
    case INTERNAL_USB_CLOCK:
    #if (defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000)) && !defined KINETIS_K21 && !defined KINETIS_K22 && !defined KINETIS_K24 && !defined KINETIS_K64 && !defined KINETIS_K65
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

    #if defined KINETIS_K64 && (defined RUN_FROM_HIRC_PLL || defined RUN_FROM_HIRC)
    // Early devices may disable the IRC48M when the reset command is executed so we need to switch to a temporary clock source and switch back again once completed
    //
    if (iIRC48M_workaround != 0) {                                       // oder K64 device which disables the IRC48M when the USB reset command is executed
        // Temporarily move to an alternative clock source
        //
        #if defined RUN_FROM_HIRC_PLL                                    // operating from PLL (PEE state)
        MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1280);           // move from PEE to PBE
        while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {   // loop until the external clock is selected
            #if defined _WINDOWS
            MCG_S &= ~MCG_S_CLKST_MASK;
            MCG_S |= MCG_S_CLKST_EXTERN_CLK;
            #endif
        }
        MCG_C6 = (CLOCK_MUL - MCG_C6_VDIV0_LOWEST);                      // move to FBE
        while ((MCG_S & MCG_S_PLLST) != 0) {                             // loop until the PLLS clock is no longer valid
                #if defined _WINDOWS
            MCG_S &= ~MCG_S_PLLST;
                #endif
        }
        #endif
        MCG_SC &= ~(MCG_SC_FCRDIV_128);                                  // remove any divide values to achieve 4MHz
        MCG_C2 |= MCG_C2_IRCS;                                           // ensure fast internal clock is selected for MCGIRCLK
        MCG_C1 = ((MCG_C1 & ~MCG_C1_CLKS_EXTERN_CLK) | MCG_C1_CLKS_INTERN_CLK | MCG_C1_IRCLKEN); // switch MCGOUTCLK to the fast internal clock
        while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_INTERN_CLK) {   // wait for the output to be set
            #if defined _WINDOWS
            MCG_S &= ~MCG_S_CLKST_MASK;
            MCG_S |= MCG_S_CLKST_INTERN_CLK;
            #endif
        }
        // We are now in FBI state wuth the sytem clock driven by the 4MHz IRC
        //
    }
    #endif
    USB_USBTRC0 |= USB_USBTRC0_USBRESET;                                 // reset USB controller
    while (USB_USBTRC0 & USB_USBTRC0_USBRESET) {                         // wait for the reset to complete
    #if defined _WINDOWS
        USB_USBTRC0 = 0;
    #endif
    }
    #if defined KINETIS_HAS_IRC48M && defined USB_CRYSTAL_LESS           // {104}
    USB_CLK_RECOVER_IRC_EN = USB_CLK_RECOVER_IRC_EN_IRC_EN;              // enable 48MHz IRC clock and clock recovery (this may have been disabled by the USB reset command)
    USB_CLK_RECOVER_CTRL = USB_CLK_RECOVER_CTRL_CLOCK_RECOVER_EN;
    #endif
    #if defined KINETIS_K64 && (defined RUN_FROM_HIRC_PLL || defined RUN_FROM_HIRC)
    if (iIRC48M_workaround != 0) {
        // Move back to original clock source
        //
        #if defined RUN_FROM_HIRC_PLL                                    // we are in FBI state and must move to FBE
        MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1280);           // switch the external clock source also to the FLL to satisfy the PBE state requirement
        MCG_C5 = ((CLOCK_DIV - 1) | MCG_C5_PLLSTEN0);                    // PLL remains enabled in normal stop modes
        MCG_C6 = ((CLOCK_MUL - MCG_C6_VDIV0_LOWEST) | MCG_C6_PLLS);      // complete PLL configuration and move to PBE
        while ((MCG_S & MCG_S_PLLST) == 0) {                             // loop until the PLLS clock source becomes valid
                #if defined _WINDOWS
            MCG_S |= MCG_S_PLLST;
                #endif
        }
        while ((MCG_S & MCG_S_LOCK) == 0) {                              // loop until PLL locks
                #if defined _WINDOWS
            MCG_S |= MCG_S_LOCK;
                #endif
        }
        MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);              // finally move from PBE to PEE mode - switch to PLL clock
        while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {          // loop until the PLL clock is selected
                #if defined _WINDOWS
            MCG_S &= ~MCG_S_CLKST_MASK;
            MCG_S |= MCG_S_CLKST_PLL;
                #endif
        }
        #else
        MCG_C1 = ((MCG_C1 & ~MCG_C1_CLKS_INTERN_CLK) | (original_MCG_C1 & MCG_S_CLKST_EXTERN_CLK)); // switch MCGOUTCLK back to the original clock source
        while ((MCG_S & MCG_S_CLKST_MASK) != (original_MCG_C1 & MCG_S_CLKST_EXTERN_CLK)) { // wait for the output to be set
            #if defined _WINDOWS
            MCG_S &= ~MCG_S_CLKST_MASK;
            MCG_S |= (original_MCG_C1 & MCG_S_CLKST_EXTERN_CLK);
            #endif
        }
        #endif
        MCG_C2 = original_MCG_C2;                                        // return original clock settings
        MCG_SC = original_MCG_SC;
        MCG_C1 = original_MCG_C1;
    }
    #endif

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
    ptrBDT->usb_bd_rx_even.ptrUSB_BD_Data  = (unsigned char *)uMalloc(ENDPOINT_0_SIZE); // alloc buffer memory for control endpoint
    ptrBDT->usb_bd_rx_odd.ptrUSB_BD_Data   = (unsigned char *)uMalloc(ENDPOINT_0_SIZE);
    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl  = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS | OWN); // set endpoint rx size and configure Data Toggle Synchronisation (receive DATA 0 frames)
    ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1); // set endpoint rx size and configure Data Toggle Synchronisation (receive DATA 1 frames)
    usb_endpoints[0].ulEndpointSize = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS);
    ENDPT0 = (EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);                    // enable control endpoint transmission and reception

    OTG_INT_STAT = (A_VBUS_CHG | BSESS_CHG | SESS_VLD_CHG | LINE_STAT_CHG | MSEC_1 | ID_CHG); // reset any pending OTG interrupts
    INT_STAT = (STALL | ATTACH | RESUME | SLEEP | TOK_DNE | SOF_TOK | USB_ERROR | USB_RST); // reset any pending USB interrupts
    ERR_STAT = (PID_ERR | CRC5_EOF | CRC16 | DFN8 | BTO_ERR | USB_DMA_ERR | BTS_ERR); // reset any pending USB error interrupts
    #if defined _WINDOWS
    INT_STAT = ERR_STAT = 0;
    #endif

    fnEnterInterrupt(irq_USB_OTG_ID, PRIORITY_USB_OTG, _usb_otg_isr);    // configure and enter the USB handling interrupt routine in the vector table
    USB_CTRL = 0;                                                        // take transceiver out of suspend mode (with no pull-downs enabled)
    INT_ENB = (STALL_EN | USB_ERROR_EN | TOK_DNE_EN | USB_RST_EN);       // enable USB interrupts
    ERR_ENB = (CRC5_EOF_EN | CRC16_EN);                                  // enable some error sources, for statistic use (CRC errors)
    CTL = USB_EN_SOF_EN;                                                 // enable USB module device operation
    if (pars->usConfig & USB_FULL_SPEED) {
        USB_USBTRC0 = USB_USBTRC0_RES;                                   // this bit is not defined but should be set for correct operation
        USB_OTG_CONTROL = USB_OTG_CONTROL_DPPULLUPNONOTG;                // enable pull up on D+ for full speed operation           
    }
    else {
        //OTG_CTRL = (DM_HIGH | OTG_EN);                                 // enable pull up on D- for low speed operation (this doesn't seem possible)
    }

    #if defined USE_USB_OTG_CHARGE_PUMP
    if (pars->OTG_I2C_Channel == 0) {                                    // if no I2C interface opened, open it now
	    IICTABLE tIICParameters;

	    tIICParameters.Channel = CHARGE_PUMP_IIC_CHANNEL;
	    tIICParameters.usSpeed = 100;                                    // 100k
        tIICParameters.Rx_tx_sizes.TxQueueSize = 10;
        tIICParameters.Rx_tx_sizes.RxQueueSize = 0;
	    tIICParameters.Task_to_wake = 0;                                 // no wake on transmission

        iic_handle = fnOpen(TYPE_IIC, FOR_I_O, &tIICParameters);         // open the channel with defined configurations
    }
    else {
        iic_handle = pars->OTG_I2C_Channel;                              // share already available IIC interface
    }
    if (iic_handle != 0) {
        #define CONTROL_REGISTER_1  0x10
        #define CONTROL_REGISTER_2  0x11
        #define  BDISC_ACONN        0x04
        #define  DP_PULLUP          0x10
        #define  DM_PULLUP          0x20
        const static unsigned char ucEnableDevice[] = {OTG_CHARGE_PUMP_IIC_ADD, CONTROL_REGISTER_2, 0x00}; // this assumes the MAX3353 
        unsigned char ucFS_Device[]    = {OTG_CHARGE_PUMP_IIC_ADD, CONTROL_REGISTER_1, (BDISC_ACONN | DP_PULLUP)};
        fnWrite(iic_handle, (unsigned char *)ucEnableDevice, sizeof(ucEnableDevice)); // turn on device
        if ((pars->usConfig & USB_FULL_SPEED) == 0) {
            ucFS_Device[2] = (BDISC_ACONN | DM_PULLUP);                  // low speed mode
        }
        fnWrite(iic_handle, ucFS_Device, sizeof(ucFS_Device));           // configure for device mode
    }
    #endif
}

