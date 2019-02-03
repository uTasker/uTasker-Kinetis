/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      stm32_USB_OTG.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/

#if defined _USB_OTG_DRIVER_CODE && defined USB_OTG_AVAILABLE

static USB_HW usb_hardware = {{{0}}};
static USB_END_POINT *usb_endpoints;

#define ENDPOINT_RX_FIFO_SIZE      64                                    // (number of words) the RxFIFO and 4 TxFIFOs are divided into equal areas of 256 bytes each (total 1.25k available)
#define ENDPOINT_0_TX_FIFO_SIZE    64                                    // (number of words) the RxFIFO and 4 TxFIFOs are divided into equal areas of 256 bytes each (total 1.25k available)
#define ENDPOINT_1_TX_FIFO_SIZE    64                                    // (number of words) the RxFIFO and 4 TxFIFOs are divided into equal areas of 256 bytes each (total 1.25k available)
#define ENDPOINT_2_TX_FIFO_SIZE    64                                    // (number of words) the RxFIFO and 4 TxFIFOs are divided into equal areas of 256 bytes each (total 1.25k available)
#define ENDPOINT_3_TX_FIFO_SIZE    64                                    // (number of words) the RxFIFO and 4 TxFIFOs are divided into equal areas of 256 bytes each (total 1.25k available)


#if defined _WINDOWS
    #define _CLEAR_USB_INT(flag) OTG_FS_GINTSTS &= ~(flag)               // clear the global interrupt
    #define _CLEAR_EP_INT(dir, ep, flag)  OTG_FS_D##dir##EPINT##ep &= ~(flag) // clear endpoint interrupt
#else
    #define _CLEAR_USB_INT(flag) OTG_FS_GINTSTS = (flag)                 // clear the interrupt
    #define _CLEAR_EP_INT(dir, ep, flag)  OTG_FS_D##dir##EPINT##ep = (flag) // clear interrupt
#endif



/***** Debugging ****************************/
#define MAX_USB_EVENTS 2048
unsigned long ulUSBEvents[MAX_USB_EVENTS] = {0};
int iUSB_event_Number = 0;
int iLogOn = 0;

extern void fnAddUSB_event(unsigned long ulCause, unsigned long Value)
{/*
    if ((ulCause >= 0x100031) && (ulCause <= 0x100035)) {                                           // trigger from this cause
        iLogOn = 1;
    }*/
    if (iLogOn == 0) {
        return;
    }
    if (iUSB_event_Number >= (MAX_USB_EVENTS - 2)) {
        return;
       // iUSB_event_Number = 0;
    }
    ulUSBEvents[iUSB_event_Number++] = ulCause;
    ulUSBEvents[iUSB_event_Number++] = Value;
    ulUSBEvents[iUSB_event_Number++] = (OTG_FS_DSTS >> 8);               // SOF
}

/*********************************************/

// This routine is called with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    unsigned long *ulPtrCtrl = OTG_FS_DOEPCTL0_ADD;                      // {1}
    ulPtrCtrl += ((0x20/sizeof(unsigned long)) * ucEndpointRef);
    if (usb_hardware.usStoredLength[ucEndpointRef] == 0) {               // if no reception data stored
        *ulPtrCtrl |= OTG_FS_DOEPCTL_CNAK;                               // allow further reception on the endpoint
        return USB_BUFFER_NO_DATA;                                       // no waiting data
    }                                                                    // else handle stored data
    if (fnEndpointData(ucEndpointRef, (unsigned char *)usb_hardware.ulUSB_buffer[ucEndpointRef], usb_hardware.usStoredLength[ucEndpointRef], OUT_DATA_RECEPTION, 0) != MAINTAIN_OWNERSHIP) {
        usb_hardware.usStoredLength[ucEndpointRef] = 0;                  // the input is no longer valid
        *ulPtrCtrl |= OTG_FS_DOEPCTL_CNAK;                               // allow further reception on the endpoint
        return USB_BUFFER_FREED;                                         // buffer consumed and freed
    }
    return USB_BUFFER_BLOCKED;
}

unsigned long ulTemp[5];
// Extract the defined number of bytes from the RxFIFO
//
static void fnExtractFIFO(volatile unsigned long *ptrRxFIFO, USB_HW *ptr_usb_hardware, int iEndpoint_ref)
{
    int iLength = ptr_usb_hardware->usLength;
    unsigned long *ptrBuffer = ptr_usb_hardware->ulUSB_buffer[iEndpoint_ref];


if (iLogOn == 10)
{
ulTemp[0] = *ptrRxFIFO;
ulTemp[1] = *ptrRxFIFO;
ulTemp[2] = *ptrRxFIFO;
ulTemp[3] = *ptrRxFIFO;
ulTemp[4] = *ptrRxFIFO;
}
    while (iLength > 0) {
    #if defined _WINDOWS
        *ptrBuffer++ = *ptrRxFIFO++;
        _CLEAR_USB_INT(OTG_FS_GINTSTS_RXFLVL);                           // reading all rx FIFO content resets the interrupt
    #else
        *ptrBuffer++ = *ptrRxFIFO;
    #endif
        iLength -= 4;
    }
}


// Return USB error counters
//
extern unsigned long fnUSB_error_counters(int iValue)                    // {24}
{
    // No error counters supported 
    //
    return 0;
}

// Copy a buffer to the TxFIFO
//
extern void fnPutToFIFO(int iLength, volatile unsigned long *ptrTxFIFO, unsigned char *ptrBuffer)
{
    unsigned long ulEntry;
    do {
        ulEntry = *ptrBuffer++;
        ulEntry |= (*ptrBuffer++ << 8);
        ulEntry |= (*ptrBuffer++ << 16);
        ulEntry |= (*ptrBuffer++ << 24);
    #if defined _WINDOWS
        *ptrTxFIFO++ = ulEntry;
    #else
        *ptrTxFIFO = ulEntry;
    #endif
        iLength -= 4;
    } while (iLength > 0);
    _SIM_USB(USB_SIM_TX, 0, 0);
}


// Check whether the transmit FIFO is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW **ptr_usb_hardware)
{
    switch (iEndpoint) {
    case 0:
        if (OTG_FS_DTXFSTS0 != ENDPOINT_0_TX_FIFO_SIZE) {
            return ENDPOINT_NOT_FREE;                                    // data already queued
        }
        break;
    case 1:
        if (OTG_FS_DTXFSTS1 != ENDPOINT_1_TX_FIFO_SIZE) {
            return ENDPOINT_NOT_FREE;                                    // data already queued
        }
        break;
    case 2:
        if (OTG_FS_DTXFSTS2 != ENDPOINT_2_TX_FIFO_SIZE) {
            return ENDPOINT_NOT_FREE;                                    // data already queued
        }
        break;
    case 3:
        if (OTG_FS_DTXFSTS3 != ENDPOINT_3_TX_FIFO_SIZE) {
            return ENDPOINT_NOT_FREE;                                    // data already queued
        }
        break;
    }
    return ENDPOINT_FREE;                                                // there is presently no data in the TxFIFO
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength, USB_ENDPOINT *ptrEndpoint)
{
    unsigned long  ulEndpoint_config = OTG_FS_DIEPCTL_EPTYP_CONTROL;
    unsigned long  ulEP_interrupt;
    unsigned long *ulEndpointCtrl = OTG_FS_DIEPCTL0_ADD;

    if (ENDPOINT_DISABLE == ucEndpointType) {
        OTG_FS_DAINTMSK &= ~((OTG_FS_DAINTMSK_IEPM0 | OTG_FS_DAINTMSK_IEPM0) << ucEndpointRef); // disable endpoint interrupts
        ulEndpointCtrl += ((0x20/sizeof(unsigned long)) * ucEndpointRef);
        *ulEndpointCtrl = 0;                                             // deactivate OUT/IN control
        ulEndpointCtrl = OTG_FS_DOEPCTL0_ADD;
        ulEndpointCtrl += ((0x20/sizeof(unsigned long)) * ucEndpointRef);
        *ulEndpointCtrl = 0;                                             // deactivate OUT/IN control
        return;
    }

    switch ((ucEndpointType & ~(IN_ENDPOINT))) {
    case ENDPOINT_ISOCHRONOUS:
        ulEndpoint_config = (OTG_FS_DIEPCTL_EPTYP_ISO | OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_USBAEP_ACTIVE);
        break;
    case ENDPOINT_BULK:
        ulEndpoint_config = (OTG_FS_DIEPCTL_EPTYP_BULK | OTG_FS_DIEPCTLX_SD0PID_SEVNFRM | OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_USBAEP_ACTIVE);
        break;
    case ENDPOINT_INTERRUPT:
        ulEndpoint_config = (OTG_FS_DIEPCTL_EPTYP_INTERRUPT | OTG_FS_DIEPCTLX_SD0PID_SEVNFRM | OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_USBAEP_ACTIVE);
        break;
    case ENDPOINT_CONTROL:
        ulEndpoint_config = (OTG_FS_DIEPCTL_EPTYP_CONTROL | OTG_FS_DIEPCTLX_SD0PID_SEVNFRM | OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_USBAEP_ACTIVE);
        break;
    }

    if (usMaxEndpointLength > usEndpointLength) {                        // if no larger specified take the specified value
        usEndpointLength = usMaxEndpointLength;
    }
    usb_endpoints[ucEndpointRef].ulEndpointSize = usEndpointLength;
    ulEndpoint_config |= usEndpointLength;

    if ((ucEndpointType & IN_ENDPOINT) != 0) {                           // IN type endpoint
        ulEndpoint_config |= (ucEndpointRef << 22);
        ulEP_interrupt = (OTG_FS_DAINTMSK_IEPM0 << ucEndpointRef);
    }
    else {                                                               // OUT type endpoint
        ulEndpointCtrl = OTG_FS_DOEPCTL0_ADD;
        ulEP_interrupt = (OTG_FS_DAINTMSK_OEPM0 << ucEndpointRef);
    }
    ulEndpointCtrl += ((0x20/sizeof(unsigned long)) * ucEndpointRef);
    *ulEndpointCtrl = ulEndpoint_config;                                 // set endpoint configuration
    OTG_FS_DAINTMSK |= ulEP_interrupt;                                   // enable endpoint interrupts
    _SIM_USB(USB_SIM_ENUMERATED,0,0);                                    // inform the simulator that USB enumeration has completed
}

// Copy data to the TxFIFO and start transmission
//
extern void fnSendUSB_data(unsigned char *pData, unsigned short Len, int iEndpoint, USB_HW *ptrUSB_HW)
{
    unsigned long ulPacketLength;
    if (Len > usb_endpoints[iEndpoint].ulEndpointSize) {                 // decide how many packets are to be prepared
        if (Len > (usb_endpoints[iEndpoint].ulEndpointSize * 2)) {
            ulPacketLength = (OTG_FS_DIEPTSIZ_PKTCNT_3 | Len);           // set the length to be sent in three packets
        }
        else {
            ulPacketLength = (OTG_FS_DIEPTSIZ_PKTCNT_2 | Len);           // set the length to be sent in two packets
        }
    }
    else {
        ulPacketLength = (OTG_FS_DIEPTSIZ_PKTCNT_1 | Len);               // set the length to be sent in a single packet
    }
    switch (iEndpoint) {
    case 0:
        OTG_FS_DIEPTSIZ0 = ulPacketLength;                               // set the packet details
        OTG_FS_DIEPCTL0 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA); // enable operation and block further reception for the moment 
        fnPutToFIFO(Len, (OTG_FS_DFIFO0_ADDR), pData);                   // copy to FIFO where it will be sent
        OTG_FS_DOEPCTL0 |= (OTG_FS_DOEPCTL_CNAK);                        // allow reception on OUT endpoint
        break;
    case 1:
        OTG_FS_DIEPTSIZ1 = ulPacketLength;                               // set the packet details
        OTG_FS_DIEPCTL1 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA);
        fnPutToFIFO(Len, OTG_FS_DFIFO1_ADDR, pData);                     // copy the data to the FIFO
        break;
    case 2:
        OTG_FS_DIEPTSIZ2 = ulPacketLength;                               // set the packet details
        OTG_FS_DIEPCTL2 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA); // enable operation
        fnPutToFIFO(Len, OTG_FS_DFIFO2_ADDR, pData);                     // copy the data to the FIFO
        break;
    case 3:
        OTG_FS_DIEPTSIZ3 = ulPacketLength;                               // set the packet details
        OTG_FS_DIEPCTL3 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA);
        fnPutToFIFO(Len, OTG_FS_DFIFO3_ADDR, pData);
        break;
    }
    _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW);
}

// Send a zero data frame
//
extern void fnSendZeroData(USB_HW *ptrUSB_HW, int iEndpoint)
{
    switch (iEndpoint) {
    case 0:
        OTG_FS_DIEPTSIZ0 = (OTG_FS_DIEPTSIZ_PKTCNT_1);
        OTG_FS_DIEPCTL0 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA);
        break;
    case 1:
        OTG_FS_DIEPTSIZ1 = (OTG_FS_DIEPTSIZ_PKTCNT_1);
        OTG_FS_DIEPCTL1 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA);
        break;
    case 2:
        OTG_FS_DIEPTSIZ2 = (OTG_FS_DIEPTSIZ_PKTCNT_1);
        OTG_FS_DIEPCTL2 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA);
        break;
    case 3:
        OTG_FS_DIEPTSIZ3 = (OTG_FS_DIEPTSIZ_PKTCNT_1);
        OTG_FS_DIEPCTL3 |= (OTG_FS_DIEPCTL_CNAK | OTG_FS_DIEPCTL_EPENA);
        break;
    }
    _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW);
}


// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
    switch (ucEndpoint & ~IN_ENDPOINT) {
    case 0:
        OTG_FS_DIEPCTL0 &= ~OTG_FS_DIEPCTL_STALL;                        // clear stall on endpoint 0
        break;
    case 1:
        OTG_FS_DIEPCTL1 = ((OTG_FS_DIEPCTL1 & ~OTG_FS_DIEPCTL_STALL) | OTG_FS_DIEPCTLX_SD0PID_SEVNFRM); // clear stall on endpoint 1
        break;
    case 2:
        OTG_FS_DIEPCTL2 = ((OTG_FS_DIEPCTL2 & ~OTG_FS_DIEPCTL_STALL) | OTG_FS_DIEPCTLX_SD0PID_SEVNFRM); // clear stall on endpoint 2
        break;
    case 3:
        OTG_FS_DIEPCTL3 = ((OTG_FS_DIEPCTL3 & ~OTG_FS_DIEPCTL_STALL) | OTG_FS_DIEPCTLX_SD0PID_SEVNFRM); // clear stall on endpoint 1
        break;
    }
}


// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, unsigned char ucFrameType)
{
    int iReturn;
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting messages to queue

    usb_hardware.ptrEndpoint = &usb_endpoints[iEndpoint_ref];            // mark the present transmit endpoint information for possible subroutine or response use

    switch (iReturn = fnUSB_handle_frame(ucFrameType, (unsigned char *)usb_hardware.ulUSB_buffer[iEndpoint_ref], iEndpoint_ref, &usb_hardware)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        OTG_FS_DCFG = ((OTG_FS_DCFG_PFIVL_80_PER | OTG_FS_DCFG_DSPD_FULL_SPEED) | (usb_hardware.ucUSBAddress << 4)); // set the address (this needs to be set before the zero frame has actually been sent for the USB controller to accept it)
        FNSEND_ZERO_DATA(&usb_hardware, iEndpoint_ref);
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later (frame data has not been consumed)
        uEnable_Interrupt();
        return iReturn;
    case STALL_ENDPOINT:                                                 // send stall
        if (iEndpoint_ref == 0) {
            OTG_FS_DIEPCTL0 |= OTG_FS_DIEPCTL_STALL;                     // stall control endpoint
            fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);       
            _SIM_USB(USB_SIM_STALL, iEndpoint_ref, &usb_hardware);
        }
        else {
            int iIN_ref = fnGetPairedIN(iEndpoint_ref);                  // get the paired IN endpoint reference
            switch (iIN_ref) {
            case 1:
                OTG_FS_DIEPCTL1 |= OTG_FS_DIEPCTL_STALL;                 // stall on IN endpoint 1
                break;
            case 2:
                OTG_FS_DIEPCTL2 |= OTG_FS_DIEPCTL_STALL;                 // stall on IN endpoint 2
                break;
            case 3:
                OTG_FS_DIEPCTL3 |= OTG_FS_DIEPCTL_STALL;                 // stall on IN endpoint 3
                break;
            }
            fnSetUSBEndpointState(iIN_ref, USB_ENDPOINT_STALLED);        // mark the stall at the IN endpoint
            _SIM_USB(USB_SIM_STALL, iIN_ref, &usb_hardware);
        }
        break;
    default:
        break;
    }
    uEnable_Interrupt();
    return 0;
}


// Handle a RxFIFO event
//
static void fnGetRx(USB_HW *ptr_usb_hardware)
{
    register unsigned long ulReception;
    int iEndpoint_ref;
OTG_FS_GINTMSK &= ~(OTG_FS_GINTMSK_RXFLVLM); // block until read from FIFO
    ulReception = OTG_FS_GRXSTSP;                                        // read status of next entry in RxFIFO (pop the status value)
    iEndpoint_ref = (ulReception & OTG_FS_GRXSTSR_EPNUM);                // the endpoint to which the reception belongs

    if (iEndpoint_ref == 0) {                                            // control endpoint 0
        switch (ulReception & OTG_FS_GRXSTSR_PKTSTS_MASK) {              // switch depending on RxFIFO cause
        case OTG_FS_GRXSTSR_PKTSTS_OUT_OK:
fnAddUSB_event(0x99990001, ulReception);
    #if defined _WINDOWS
            _CLEAR_USB_INT(OTG_FS_GINTSTS_RXFLVL);                       // reading rx FIFO status resets the interrupt
            OTG_FS_GINTSTS |= OTG_FS_GINTSTS_OEPINT;                     // this is followed by an immediate OUT interrupt on endpoint 0
            OTG_FS_DAINT |= OTG_FS_DAINT_OEPINT0;
            OTG_FS_DOEPINT0 |= OTG_FS_DOEPINT_XFRC;                      // transfer complete
    #endif
            break;
        case OTG_FS_GRXSTSR_PKTSTS_SETUP_OK:                             // host is acknowledging the completion of a setup phase
fnAddUSB_event(0x99990002, ulReception);
OTG_FS_DOEPTSIZ0 = (OTG_FS_DOEPTSIZ_STUPCNT_3 | OTG_FS_DOEPTSIZ_PKTCNT | OTG_FS_DOEPTSIZ_PKTCNT | (64)); // try re-write
OTG_FS_DOEPCTL0 = (OTG_FS_DOEPCTL_EPENA | OTG_FS_DOEPCTL_CNAK);
    #if defined _WINDOWS
            _CLEAR_USB_INT(OTG_FS_GINTSTS_RXFLVL);                       // reading rx FIFO status resets the interrupt
            OTG_FS_GINTSTS |= OTG_FS_GINTSTS_OEPINT;                     // this is followed by an immediate OUT interrupt on endpoint 0
            OTG_FS_DAINT |= OTG_FS_DAINT_OEPINT0;
            OTG_FS_DOEPINT0 |= OTG_FS_DOEPINT_STUP;                      // setup complete
    #endif
            break;
        case OTG_FS_GRXSTSR_PKTSTS_OUT_RX:                               // OUT data
fnAddUSB_event(0x99990003, ulReception);
        case OTG_FS_GRXSTSR_PKTSTS_SETUP_RX:                             // setup PID received on control endpoint 0
            {
                unsigned char ucFrameType;
fnAddUSB_event(0x99990004, ulReception);
                ptr_usb_hardware->usLength = (unsigned short)((ulReception & OTG_FS_GRXSTSR_BCNT_MASK) >> OTG_FS_GRXSTSR_BCNT_SHIFT); // data length
                if ((ulReception & OTG_FS_GRXSTSR_PKTSTS_MASK) == OTG_FS_GRXSTSR_PKTSTS_SETUP_RX) { // {1}
                    ucFrameType = USB_SETUP_FRAME;
fnAddUSB_event(0x11111111, ulReception);
                }
                else {
                    ucFrameType = USB_OUT_FRAME;
if (ptr_usb_hardware->usLength != 0) {
fnAddUSB_event(0x22222222, ulReception);
//iLogOn++;
if (iLogOn == 2) { 
    iLogOn = 10;
}
}
    #if defined _WINDOWS
                    OTG_FS_GRXSTSR &= ~(OTG_FS_GRXSTSR_BCNT_MASK | OTG_FS_GRXSTSR_PKTSTS_MASK); // OUT OK follows
                    OTG_FS_GRXSTSR |= OTG_FS_GRXSTSR_PKTSTS_OUT_OK;
                    OTG_FS_GRXSTSP = OTG_FS_GRXSTSR;
    #endif
                }
                if (ptr_usb_hardware->usLength != 0) {
const unsigned char line_coding[] = {0x21, 0x20, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
                  //if (ucFrameType == USB_OUT_FRAME) {
                  //    fnDelayLoop(1000);                                          // workaround to stop losing first 4 bytes in RxFIFO
                  //}
                    fnExtractFIFO(OTG_FS_DFIFO0_ADDR, ptr_usb_hardware, 0); // read from FIFO to local buffer, clearing RxFIFO interrupt

if (uMemcmp(ptr_usb_hardware->ulUSB_buffer, line_coding, 8) == 0) {
iLogOn = 3; // start recording
}
if (ucFrameType == USB_SETUP_FRAME) {
fnAddUSB_event(0x77777777, *OTG_FS_DFIFO0_ADDR); // extract and discard the setup state done word
}
                }
//OTG_FS_DOEPTSIZ0 = (OTG_FS_DOEPTSIZ_STUPCNT_3 | OTG_FS_DOEPTSIZ_PKTCNT | OTG_FS_DOEPTSIZ_PKTCNT | (3 * 8));
                if (fnProcessInput(0, ucFrameType) == MAINTAIN_OWNERSHIP) { // the content of the input buffer has already been extracted
                    OTG_FS_DOEPCTL0 |= OTG_FS_DOEPCTL_SNAK;              // don't allow further receptions until cleared
                    ptr_usb_hardware->usStoredLength[0] = ptr_usb_hardware->usLength; // mark the amount of input data that is being stored in the input buffer
                }
                else {
                    OTG_FS_DOEPCTL0 |= OTG_FS_DOEPCTL_CNAK;              // allow further receptions
                }
            }
            break;
        default:
fnAddUSB_event(0x55555555, ulReception);
//fnAddUSB_event(0x66666666, *OTG_FS_DFIFO0_ADDR);
            break;
        }
    }
    else {                                                               // endpoints 1, 2 and 3
        volatile unsigned long *ptrTxFIFO = OTG_FS_DFIFO0_ADDR;
        ptrTxFIFO += (iEndpoint_ref + (0x1000/sizeof(unsigned long)));   // set the TxFIFO corresponding to the particular endpoint
        switch (ulReception & OTG_FS_GRXSTSR_PKTSTS_MASK) {              // switch depending on PID type
        case OTG_FS_GRXSTSR_PKTSTS_OUT_RX:                               // OUT data
            {
                unsigned long *ulPtrCtrl = OTG_FS_DOEPCTL0_ADD;
                ulPtrCtrl += ((0x20/sizeof(unsigned long)) * iEndpoint_ref);
                ptr_usb_hardware->usLength = (unsigned short)((ulReception & OTG_FS_GRXSTSR_BCNT_MASK) >> OTG_FS_GRXSTSR_BCNT_SHIFT); // data length
                fnExtractFIFO(OTG_FS_DFIFO0_ADDR, ptr_usb_hardware, iEndpoint_ref); // read from FIFO to local buffer, clearing RxFIFO interrupt
                if (fnProcessInput(iEndpoint_ref, USB_OUT_FRAME) == 0) { // {1} handle the content of the input buffer
                    *ulPtrCtrl |= OTG_FS_DOEPCTL_CNAK;                   // allow further reception on the endpoint
                }
                else {                                                   // don't allow further reception
                    *ulPtrCtrl |= OTG_FS_DOEPCTL_SNAK;                   // always respond with NAK
                    ptr_usb_hardware->usStoredLength[iEndpoint_ref] = ptr_usb_hardware->usLength; // mark the amount of input data that is being stored in the input buffer
                }
    #if defined _WINDOWS
                OTG_FS_GRXSTSR &= ~(OTG_FS_GRXSTSR_BCNT_MASK | OTG_FS_GRXSTSR_PKTSTS_MASK); // OUT OK follows
                OTG_FS_GRXSTSR |= OTG_FS_GRXSTSR_PKTSTS_OUT_OK;
                OTG_FS_GRXSTSP = OTG_FS_GRXSTSR;
                OTG_FS_GINTSTS |= OTG_FS_GINTSTS_RXFLVL;
    #endif
            }
            break;
        case OTG_FS_GRXSTSR_PKTSTS_OUT_OK:
    #if defined _WINDOWS
            _CLEAR_USB_INT(OTG_FS_GINTSTS_RXFLVL);                       // reading rx FIFO status resets the interrupt
            OTG_FS_GINTSTS |= OTG_FS_GINTSTS_OEPINT;                     // this is followed by an immediate OUT interrupt on endpoint 1
            OTG_FS_DAINT = (OTG_FS_DAINT_OEPINT0 << iEndpoint_ref);
            OTG_FS_DOEPINT1 |= OTG_FS_DOEPINT_XFRC;                      // transfer complete
    #endif
            break;
        case OTG_FS_GRXSTSR_PKTSTS_SETUP_OK:                             // host is acknowledging the completion of a setup phase
    #if defined _WINDOWS
            _CLEAR_USB_INT(OTG_FS_GINTSTS_RXFLVL);                       // reading rx FIFO status resets the interrupt
            OTG_FS_GINTSTS |= OTG_FS_GINTSTS_OEPINT;                     // this is followed by an immediate OUT interrupt on endpoint 1
            OTG_FS_DAINT |= (OTG_FS_DAINT_OEPINT0 << iEndpoint_ref);
            OTG_FS_DOEPINT1 |= OTG_FS_DOEPINT_STUP;                      // setup complete
    #endif
            break;
        case OTG_FS_GRXSTSR_PKTSTS_SETUP_RX:                             // setup PID received on control endpoint 1                   
            break;
        default:
            break;
        }
    }
    OTG_FS_GINTMSK |= (OTG_FS_GINTMSK_RXFLVLM);
}


// USB OTG FS interrupt handler
//
static __interrupt void USB_OTG_FS_Interrupt(void)
{
    unsigned long ulInterrupts;

fnAddUSB_event(0x55550001, 0);

    while ((ulInterrupts = (OTG_FS_GINTSTS & OTG_FS_GINTMSK)) != 0) {    // while enabled USB interrupts are pending
fnAddUSB_event(0x55550002, ulInterrupts);
        if (ulInterrupts & (OTG_FS_GINTSTS_USBRST | OTG_FS_GINTSTS_USBSUSP | OTG_FS_GINTSTS_WKUINT | OTG_FS_GINTSTS_ENUMDNE)) { // line state change
            if ((ulInterrupts & OTG_FS_GINTSTS_USBRST) != 0) {           // USB reset detected
              //OTG_FS_DCTL = OTG_FS_DCTL_RWUSIG;                        // clear remote wakeup signalling
                OTG_FS_GRSTCTL = (OTG_FS_GRSTCTL_TXFFLSH | OTG_FS_GRSTCTL_TXFNUM_ALL); // flush tx FIFOs
                while (OTG_FS_GRSTCTL & OTG_FS_GRSTCTL_TXFFLSH) {        // wait until the flush has competed
                #if defined _WINDOWS
                    OTG_FS_GRSTCTL &= ~OTG_FS_GRSTCTL_TXFFLSH;
                #endif
                }
                uMemset(usb_hardware.usStoredLength, 0, sizeof(usb_hardware.usStoredLength));
                fnDelayLoop(3);                                          // 3us delay to allow 3 PHY clocks to occur
                _CLEAR_EP_INT(I, 0, (OTG_FS_DIEPINT_XFRC | OTG_FS_DIEPINT_EPDISD | OTG_FS_DIEPINT_TOC | OTG_FS_DIEPINT_ITTXFE | OTG_FS_DIEPINT_INEPNE | OTG_FS_DIEPINT_TXFE)); // clear endpoint flags
                _CLEAR_EP_INT(I, 1, (OTG_FS_DIEPINT_XFRC | OTG_FS_DIEPINT_EPDISD | OTG_FS_DIEPINT_TOC | OTG_FS_DIEPINT_ITTXFE | OTG_FS_DIEPINT_INEPNE | OTG_FS_DIEPINT_TXFE));
                _CLEAR_EP_INT(I, 2, (OTG_FS_DIEPINT_XFRC | OTG_FS_DIEPINT_EPDISD | OTG_FS_DIEPINT_TOC | OTG_FS_DIEPINT_ITTXFE | OTG_FS_DIEPINT_INEPNE | OTG_FS_DIEPINT_TXFE));
                _CLEAR_EP_INT(I, 3, (OTG_FS_DIEPINT_XFRC | OTG_FS_DIEPINT_EPDISD | OTG_FS_DIEPINT_TOC | OTG_FS_DIEPINT_ITTXFE | OTG_FS_DIEPINT_INEPNE | OTG_FS_DIEPINT_TXFE));

                _CLEAR_EP_INT(O, 0, (OTG_FS_DOEPINT_XFRC | OTG_FS_DOEPINT_EPDISD | OTG_FS_DOEPINT_STUP | OTG_FS_DOEPINT_OTEPDIS | OTG_FS_DOEPINT_B2BSTUP)); // clear endpoint flags
                _CLEAR_EP_INT(O, 1, (OTG_FS_DOEPINT_XFRC | OTG_FS_DOEPINT_EPDISD | OTG_FS_DOEPINT_STUP | OTG_FS_DOEPINT_OTEPDIS | OTG_FS_DOEPINT_B2BSTUP));
                _CLEAR_EP_INT(O, 2, (OTG_FS_DOEPINT_XFRC | OTG_FS_DOEPINT_EPDISD | OTG_FS_DOEPINT_STUP | OTG_FS_DOEPINT_OTEPDIS | OTG_FS_DOEPINT_B2BSTUP));
                _CLEAR_EP_INT(O, 3, (OTG_FS_DOEPINT_XFRC | OTG_FS_DOEPINT_EPDISD | OTG_FS_DOEPINT_STUP | OTG_FS_DOEPINT_OTEPDIS | OTG_FS_DOEPINT_B2BSTUP));

                OTG_FS_DAINT = (OTG_FS_DAINT_IEPINT0 | OTG_FS_DAINT_IEPINT1 | OTG_FS_DAINT_IEPINT2 | OTG_FS_DAINT_IEPINT3 | OTG_FS_DAINT_OEPINT0 | OTG_FS_DAINT_OEPINT1 | OTG_FS_DAINT_OEPINT2 | OTG_FS_DAINT_OEPINT3); // clear all pending endpoint interrupts
                OTG_FS_DAINTMSK = (OTG_FS_DAINTMSK_IEPM0 | OTG_FS_DAINTMSK_OEPM0); // enable endpoint 0 interrupts
                OTG_FS_DOEPMSK = (OTG_FS_DOEPMSK_STUPM | OTG_FS_DOEPMSK_XFRCM); // enable interrupts on SETUP tokens and OUT transfer complete
                OTG_FS_DIEPMSK = (OTG_FS_DIEPMSK_XFRCM);                 // enable interrupts on IN transfer complete
                OTG_FS_DCFG = (OTG_FS_DCFG_DSPD_FULL_SPEED | OTG_FS_DCFG_PFIVL_80_PER); // reset device address and set full speed operation
              //OTG_FS_DOEPTSIZ0 = (OTG_FS_DOEPTSIZ_STUPCNT_3 | OTG_FS_DOEPTSIZ_PKTCNT | OTG_FS_DOEPTSIZ_PKTCNT | (3 * 8)); // set up endpoint 0 to receive up to 3 back-to-back SETUP packets
                OTG_FS_DOEPTSIZ0 = (OTG_FS_DOEPTSIZ_STUPCNT_3 | OTG_FS_DOEPTSIZ_PKTCNT | OTG_FS_DOEPTSIZ_PKTCNT | (64)); // try 64 byte length
                _CLEAR_USB_INT(OTG_FS_GINTSTS_USBRST | OTG_FS_GINTSTS_USBSUSP); // clear the interrupt
                OTG_FS_GINTMSK |= OTG_FS_GINTMSK_USBSUSPM;               // enable suspend interrupt
                usb_hardware.ucUSBAddress = 0;                           // reset the device's address
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when putting messages to queue
                fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware); // generic handler routine
                uEnable_Interrupt();
            }
            else if ((ulInterrupts & OTG_FS_GINTSTS_USBSUSP) != 0) {     // USB suspend detected
                OTG_FS_GINTMSK &= ~OTG_FS_GINTMSK_USBSUSPM;              // disable suspend interrupt
                _CLEAR_USB_INT(OTG_FS_GINTSTS_USBSUSP);                  // clear the interrupt
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when putting messages to queue
                fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);       // generic suspend handler routine
                uEnable_Interrupt();
            }
/*          else if (ulInterrupts & OTG_FS_GINTSTS_ENUMDNE) {            // the speed enumeration has completed (this interrupt is not of much use for the FS OTG since only full speed is possible in device mode)
              //OTG_FS_DSTS;                                             // normally this register is read to set the speed appropriately, but the OTG_FS model supports only full speed in device mode so leave it as it is
                OTG_FS_DCTL |= OTG_FS_DCTL_CGINAK;                       // activate endpoint 0
                OTG_FS_GUSBCFG = ((0x05 << 10) | OTG_FS_GUSBCFG_PHYSEL | OTG_FS_GUSBCFG_FDMOD); // this just reinforces the mode and is not important
                _CLEAR_USB_INT(OTG_FS_GINTSTS_ENUMDNE);                  // clear the interrupt
            }*/
            else if (ulInterrupts & OTG_FS_GINTSTS_WKUINT) {             // USB resume detected
              //OTG_FS_DCTL = OTG_FS_DCTL_RWUSIG;                        // clear remote wakeup signalling
                _CLEAR_USB_INT(OTG_FS_GINTSTS_WKUINT);                   // clear the interrupt
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when putting messages to queue
                fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);        // generic handler routine
                uEnable_Interrupt();
            }
        }
        else {
            if ((ulInterrupts & OTG_FS_GINTSTS_RXFLVL) != 0) {           // there is a reception in the RxFIFO
fnAddUSB_event(0x55550003, ulInterrupts);
                fnGetRx(&usb_hardware);                                  // check the RxFIFO event and read the content to local linear buffer if there is setup or endpoint out data
            }
            else if ((ulInterrupts & OTG_FS_GINTSTS_IEPINT) != 0) {      // IN interrupt
                unsigned long ulEp_int = (OTG_FS_DAINT & OTG_FS_DAINTMSK); // unmasked interrupt
fnAddUSB_event(0x55550004, ulInterrupts);
                _CLEAR_USB_INT(OTG_FS_GINTSTS_IEPINT);                   // clear the interrupt
                if ((ulEp_int & OTG_FS_DAINT_IEPINT0) != 0) {            // if IN interrupt on endpoint 0
                    if ((OTG_FS_DIEPINT0 & OTG_FS_DIEPINT_XFRC) != 0) {  // transfer complete interrupt
                        fnUSB_handle_frame(USB_TX_ACKED, 0, 0, &usb_hardware); // handle ack event
                    //   OTG_FS_DOEPTSIZ0 = (OTG_FS_DOEPTSIZ_STUPCNT_3 | OTG_FS_DOEPTSIZ_PKTCNT | (3 * 8));
                    //  OTG_FS_DOEPCTL0 = (OTG_FS_DOEPCTL_EPENA | OTG_FS_DOEPCTL_CNAK); // allow ACK of zero data frame
                        _CLEAR_EP_INT(I, 0, OTG_FS_DIEPINT_XFRC);        // clear interrupt
                    }
                }
                else if ((ulEp_int & OTG_FS_DAINT_IEPINT1) != 0) {       // if IN interrupt on endpoint 1
                    if ((OTG_FS_DIEPINT1 & OTG_FS_DIEPINT_ITTXFE) != 0) {// IN token received when Tx FIFO empty
                        _CLEAR_EP_INT(I, 1, OTG_FS_DIEPINT_ITTXFE);
                    }
                    if ((OTG_FS_DIEPINT1 & OTG_FS_DIEPINT_XFRC) != 0) {  // transfer complete
                        fnUSB_handle_frame(USB_TX_ACKED, 0, 1, &usb_hardware); // handle ack event
                        _CLEAR_EP_INT(I, 1, OTG_FS_DIEPINT_XFRC);
                    }
                    OTG_FS_DIEPEMPMSK &= ~OTG_FS_DIEPEMPMSK_INEPTXFEM1;  // disable further interrupts due to tx fifo empty
                }
                else if (ulEp_int & OTG_FS_DAINT_IEPINT2) {              // if IN interrupt on endpoint 2
                    if (OTG_FS_DIEPINT2 & OTG_FS_DIEPINT_ITTXFE) {       // IN token received when Tx FIFO empty
                        _CLEAR_EP_INT(I, 2, OTG_FS_DIEPINT_ITTXFE);
                    }
                    if (OTG_FS_DIEPINT2 & OTG_FS_DIEPINT_XFRC) {         // transfer complete
                        fnUSB_handle_frame(USB_TX_ACKED, 0, 2, &usb_hardware); // handle ack event
                        _CLEAR_EP_INT(I, 2, OTG_FS_DIEPINT_XFRC);
                    }
                }
                else if (ulEp_int & OTG_FS_DAINT_IEPINT3) {              // if IN interrupt on endpoint 3
                    if (OTG_FS_DIEPINT3 & OTG_FS_DIEPINT_ITTXFE) {       // IN token received when Tx FIFO empty
                        _CLEAR_EP_INT(I, 3, OTG_FS_DIEPINT_ITTXFE);
                    }
                    if (OTG_FS_DIEPINT3 & OTG_FS_DIEPINT_XFRC) {         // transfer complete
                        fnUSB_handle_frame(USB_TX_ACKED, 0, 3, &usb_hardware); // handle ack event
                        _CLEAR_EP_INT(I, 3, OTG_FS_DIEPINT_XFRC);
                    }
                    OTG_FS_DIEPEMPMSK &= ~OTG_FS_DIEPEMPMSK_INEPTXFEM3;  // disable further interrupts due to tx fifo empty
                }
            }
            else if (ulInterrupts & OTG_FS_GINTSTS_OEPINT) {             // OUT interrupt
                unsigned long ulEp_int = (OTG_FS_DAINT & OTG_FS_DAINTMSK); // unmasked interrupt
fnAddUSB_event(0x55550004, ulInterrupts);
                _CLEAR_USB_INT(OTG_FS_GINTSTS_OEPINT);                   // clear the interrupt
                if (ulEp_int & OTG_FS_DAINT_OEPINT0) {                   // if OUT interrupt on endpoint 0
                    if ((OTG_FS_DOEPINT0 & OTG_FS_DOEPMSK) & OTG_FS_DOEPINT_STUP) { // set up frame complete
                        _CLEAR_EP_INT(O, 0, OTG_FS_DOEPINT_STUP);        // clear endpoint interrupt
                    }
                    else if ((OTG_FS_DOEPINT0 & OTG_FS_DOEPMSK) & OTG_FS_DOEPINT_XFRC) {
                        _CLEAR_EP_INT(O, 0, OTG_FS_DOEPINT_XFRC);        // clear endpoint interrupt 
                    }
                }
                else if (ulEp_int & OTG_FS_DAINT_OEPINT1) {              // if OUT interrupt on endpoint 1
                    if (OTG_FS_DOEPINT1 & OTG_FS_DOEPINT_XFRC) {         // transfer complete
                        _CLEAR_EP_INT(O, 1, OTG_FS_DOEPINT_XFRC);        // clear endpoint interrupt
                    }
                }
                else if (ulEp_int & OTG_FS_DAINT_OEPINT2) {              // if OUT interrupt on endpoint 2
                    if (OTG_FS_DOEPINT2 & OTG_FS_DOEPINT_XFRC) {         // transfer complete
                        _CLEAR_EP_INT(O, 2, OTG_FS_DOEPINT_XFRC);        // clear endpoint interrupt
                    }
                }
                else if (ulEp_int & OTG_FS_DAINT_OEPINT3) {              // if OUT interrupt on endpoint 3
                    if (OTG_FS_DOEPINT3 & OTG_FS_DOEPINT_XFRC) {         // transfer complete
                        _CLEAR_EP_INT(O, 3, OTG_FS_DOEPINT_XFRC);        // clear endpoint interrupt
                    }
                }
            }
        }
    }
}


// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
    unsigned char ucEndpoints = (pars->ucEndPoints + 1);                 // endpoint count, including endpoint 0
    #if ENDPOINT_0_SIZE == 8
    unsigned long size_field = OTG_FS_DIEPCTL_MPSIZ_8_BYTES;
    #elif ENDPOINT_0_SIZE == 16
    unsigned long size_field = OTG_FS_DIEPCTL_MPSIZ_16_BYTES;
    #elif ENDPOINT_0_SIZE == 32
    unsigned long size_field = OTG_FS_DIEPCTL_MPSIZ_32_BYTES;
    #else
    unsigned long size_field = OTG_FS_DIEPCTL_MPSIZ_64_BYTES;
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX   // {13}
    _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_USB), (PORTA_BIT11 | PORTA_BIT12), (OUTPUT_PUSH_PULL | OUTPUT_ULTRA_FAST)); // configure the pins for use as USB
    // Note that the necessary 48MHz clock is supplied by the ring-clock, which is a second output from the main PLL
    // - this was configured during initialisation when the USB operation is enabled in the project
    //
    POWER_UP(APB2, RCC_APB2ENR_SYSCFGEN);                                // power up the system configuration controller
    POWER_UP(AHB2, RCC_AHB2ENR_OTGFSEN);                                 // power up USB controller
    #else
    // The USB clock of 48MHz is supplied by the PLL output and it can either be used directly or after being divided by 1.5
    // - this means that the PLL output must be either 48MHz or 72MHz in order for USB to be able to operate
    //
        #if PLL_OUTPUT_FREQ == 72000000                                  // if the PLL is set to 72MHz we divide it by 1.5 to get the USB clock
    RCC_CFGR &= ~RCC_CFGR_OTGFSPRE;
        #elif PLL_OUTPUT_FREQ == 48000000                                // use the PLL directly as USB clock
    RCC_CFGR |= RCC_CFGR_OTGFSPRE;
        #else                                                            // invalid PLL frequency for USB use
            #error "PLL must be 48MHz or 72MHz for USB FS use!!"
        #endif
    POWER_UP(AHB1, RCC_AHB1ENR_OTGFSEN);                                 // power up USB controller (only perform after setting the RCC_CFGR_OTGFSPRE accordingly)
    #endif
    RCC_APB1RSTR |= RCC_APB1RSTR_PWRRST;                                 // reset power (this is taken from an ST example but there are no details as to what for)
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX   // {13}
    OTG_FS_GCCFG = (OTG_FS_GCCFG_PWRDWN | OTG_FS_GCCFG_NOVBUSSENS);      // enable USB transceiver with Vbus sensing option disabled
    #else
    OTG_FS_GCCFG = (OTG_FS_GCCFG_PWRDWN | OTG_FS_GCCFG_VBUSASEN | OTG_FS_GCCFG_VBUSBSEN); // enable USB transceiver with Vbus sensing enabled (without sensing the device will not set its pull-ups)
    #endif

    fnDelayLoop(20000);                                                  // 20ms delay

    // The value controlling the turnaround time is based on the PHY clock (always 48MHz) and HCLK. If HCLK is equal to 48MHz the value is 5, given by (1 + (4 x HCLK/48MHz)). If HCLK is 96MHz it is 9
    //
    #define OTG_FS_GUSBCFG_TRDT_VALUE  (1 + (4 * (HCLK + (48000000/2)/48000000)))
    #if OTG_FS_GUSBCFG_TRDT_VALUE > OTG_FS_GUSBCFG_TRDT_MAX
        #define OTG_FS_GUSBCFG_TRDT_SETTING  OTG_FS_GUSBCFG_TRDT_MASK    // set the maximum value
    #else
        #define OTG_FS_GUSBCFG_TRDT_SETTING (OTG_FS_GUSBCFG_TRDT_VALUE << OTG_FS_GUSBCFG_TRDT_SHIFT)
    #endif
    OTG_FS_GUSBCFG = (OTG_FS_GUSBCFG_TRDT_SETTING | OTG_FS_GUSBCFG_PHYSEL | OTG_FS_GUSBCFG_FDMOD); // force device mode - this takes 25ms to become effective and pulls the D+ line high with pull-up

    fnDelayLoop(30000);                                                  // 30ms delay

    OTG_FS_PCGCCTL = 0;                                                  // restart PHY clock

    OTG_FS_DCFG = (OTG_FS_DCFG_PFIVL_80_PER | OTG_FS_DCFG_DSPD_FULL_SPEED); // set OTG_FS_DCFG_PFIVL_80_PER and full speed mode

    if (ucEndpoints > NUMBER_OF_USB_ENDPOINTS) {                         // limit endpoint count
        ucEndpoints = NUMBER_OF_USB_ENDPOINTS;                           // limit to maximum available in device
    }
    usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * ucEndpoints)); // get endpoint control structures
    usb_endpoints[0].ulEndpointSize = ENDPOINT_0_SIZE;
    OTG_FS_GRXFSIZ = (ENDPOINT_RX_FIFO_SIZE);                            // set Rx FIFO size for use by the control endpoint
    OTG_FS_DIEPTXF0 = (((unsigned long)(ENDPOINT_0_TX_FIFO_SIZE) << 16) | (ENDPOINT_RX_FIFO_SIZE)); // endpoint 0 tx FIFO following the rx FIFO (minimum depth 16 words)
    OTG_FS_DIEPTXF1 = (((unsigned long)(ENDPOINT_1_TX_FIFO_SIZE) << 16) | ((ENDPOINT_RX_FIFO_SIZE) + ENDPOINT_0_TX_FIFO_SIZE));
    OTG_FS_DIEPTXF2 = (((unsigned long)(ENDPOINT_2_TX_FIFO_SIZE) << 16) | ((ENDPOINT_RX_FIFO_SIZE) + ENDPOINT_0_TX_FIFO_SIZE + ENDPOINT_1_TX_FIFO_SIZE));
    OTG_FS_DIEPTXF3 = (((unsigned long)(ENDPOINT_3_TX_FIFO_SIZE) << 16) | ((ENDPOINT_RX_FIFO_SIZE) + ENDPOINT_0_TX_FIFO_SIZE + ENDPOINT_1_TX_FIFO_SIZE + ENDPOINT_2_TX_FIFO_SIZE));
    #if defined _WINDOWS
    OTG_FS_DTXFSTS0 = ENDPOINT_0_TX_FIFO_SIZE;
    OTG_FS_DTXFSTS1 = ENDPOINT_1_TX_FIFO_SIZE;
    OTG_FS_DTXFSTS2 = ENDPOINT_2_TX_FIFO_SIZE;
    OTG_FS_DTXFSTS3 = ENDPOINT_3_TX_FIFO_SIZE;
    #endif
    OTG_FS_GRSTCTL = (OTG_FS_GRSTCTL_TXFFLSH | OTG_FS_GRSTCTL_TXFNUM_ALL); // flush tx FIFOs
    while (OTG_FS_GRSTCTL & OTG_FS_GRSTCTL_TXFFLSH) {                    // wait until the flush has competed
    #if defined _WINDOWS
        OTG_FS_GRSTCTL &= ~OTG_FS_GRSTCTL_TXFFLSH;
    #endif
    }
    fnDelayLoop(3);                                                      // 3us delay to allow 3 PHY clocks to occur
    OTG_FS_GRSTCTL = OTG_FS_GRSTCTL_RXFFLSH;                             // flush all rx FIFOs
    while (OTG_FS_GRSTCTL & OTG_FS_GRSTCTL_RXFFLSH) {                    // wait until the flush has competed
    #if defined _WINDOWS
        OTG_FS_GRSTCTL &= ~OTG_FS_GRSTCTL_RXFFLSH;
    #endif
    }
    fnDelayLoop(3);                                                      // 3us delay to allow 3 PHY clocks to occur
    OTG_FS_DIEPMSK = 0;                                                  // disable IN interrupts
    OTG_FS_DOEPMSK = 0;                                                  // disable OUT interrupts
    OTG_FS_DAINTMSK = 0;                                                 // disable all endpoint interrupts
    OTG_FS_DAINT = (OTG_FS_DAINT_IEPINT0 | OTG_FS_DAINT_IEPINT1 | OTG_FS_DAINT_IEPINT2 | OTG_FS_DAINT_IEPINT3 | OTG_FS_DAINT_OEPINT0 | OTG_FS_DAINT_OEPINT1 | OTG_FS_DAINT_OEPINT2 | OTG_FS_DAINT_OEPINT3); // clear all pending endpoint interrupts

    if (OTG_FS_DIEPCTL0 & OTG_FS_DIEPCTL_EPENA) {                        // if the IN endpoint is enabled
        size_field |= (OTG_FS_DIEPCTL_EPDIS | OTG_FS_DIEPCTL_SNAK);      // disable endpoint
    }
    OTG_FS_DIEPCTL0 = size_field;                                        // set the endpoint size field
    OTG_FS_DIEPTSIZ0 = 0;
    _CLEAR_EP_INT(I, 0, (OTG_FS_DIEPINT_XFRC | OTG_FS_DIEPINT_EPDISD | OTG_FS_DIEPINT_TOC | OTG_FS_DIEPINT_ITTXFE | OTG_FS_DIEPINT_INEPNE | OTG_FS_DIEPINT_TXFE)); // reset flags 

    if (OTG_FS_DOEPCTL0 & OTG_FS_DIEPCTL_EPENA) {                        // if the OUT endpoint is enabled
        OTG_FS_DOEPCTL0 = (OTG_FS_DIEPCTL_EPDIS | OTG_FS_DIEPCTL_SNAK);  // disable endpoint
    }
    else {
        OTG_FS_DOEPCTL0 = 0;
    }
    OTG_FS_DOEPTSIZ0 = 0;

    // Clear pending interrupts
    //
    _CLEAR_EP_INT(O, 0, (OTG_FS_DOEPINT_XFRC | OTG_FS_DOEPINT_EPDISD | OTG_FS_DOEPINT_STUP | OTG_FS_DOEPINT_OTEPDIS | OTG_FS_DOEPINT_B2BSTUP));
    _CLEAR_USB_INT(OTG_FS_GINTSTS_MMIS | OTG_FS_GINTSTS_OTGINT | OTG_FS_GINTSTS_SOF | OTG_FS_GINTSTS_RXFLVL | \
                   OTG_FS_GINTSTS_NPTXFE | OTG_FS_GINTSTS_GINAKEFF | OTG_FS_GINTSTS_GONAKEFF | OTG_FS_GINTSTS_ESUSP | OTG_FS_GINTSTS_USBSUSP | \
                   OTG_FS_GINTSTS_USBRST | OTG_FS_GINTSTS_ENUMDNE | OTG_FS_GINTSTS_ISOODRP | OTG_FS_GINTSTS_EOPF | OTG_FS_GINTSTS_IEPINT | \
                   OTG_FS_GINTSTS_OEPINT | OTG_FS_GINTSTS_IISOIXFR | OTG_FS_GINTSTS_IPXFR | OTG_FS_GINTSTS_HPRTINT | OTG_FS_GINTSTS_HCINT | \
                   OTG_FS_GINTSTS_PTXFE | OTG_FS_GINTSTS_CIDSCHG | OTG_FS_GINTSTS_DISCINT | OTG_FS_GINTSTS_SRQINT | OTG_FS_GINTSTS_WKUINT);
    OTG_FS_GINTMSK = (OTG_FS_GINTMSK_RXFLVLM | OTG_FS_GINTMSK_USBRST /*| OTG_FS_GINTMSK_ENUMDNEM*/ | OTG_FS_GINTMSK_OEPINT | OTG_FS_GINTMSK_IEPINT | OTG_FS_GINTMSK_WUIM); // enable USB device interrupts
    fnEnterInterrupt(irq_OTG_FS_ID, PRIORITY_OTG_FS, USB_OTG_FS_Interrupt); // enter USB interrupt handler
    OTG_FS_GAHBCFG = OTG_FS_GAHBCFG_GINTMSK;                             // enable global interrupts
}
#endif