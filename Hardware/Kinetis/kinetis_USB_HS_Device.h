/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_USB_HS_Device.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    28.03.2013 Add USB HS support                                        {33}
    04.06.2013 Added USB_FS_MALLOC(), USB_FS_MALLOC_ALIGN(), USB_HS_MALLOC(), USB_HS_MALLOC_ALIGN() defaults {42}
    05.03.2014 Add USB error counters and clear errors in the USB interrupt routine {71}
    28.05.2014 Move re-enable of USB SIE token processing to after input handling to avoid sporadic SETUP frame loss {83}
    18.07.2015 Add USB_SIMPLEX_ENDPOINTS suport to HS device             {1}
    05.10.2015 fnGetUSB_HW() modification for compatibility with host mode
    23.12.2015 Add zero copy OUT endpoint buffer option                  {2}
    07.02.2016 Set length to 8 when receiving setup frames (to avoid old lengths from OUTs on the endpoint from being kept) {3}
    08.05.2017 Correct internal HS USB 12MHz oscillator setting          {4}

*/
#if defined USB_HS_INTERFACE
/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined USB_HS_MALLOC                                               // {42}
    #define USB_HS_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif
#if !defined USB_HS_MALLOC_ALIGN                                         // {42}
    #define USB_HS_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
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
//static KINETIS_USB_ENDPOINT_BD *ptrBDT = 0;
static USB_END_POINT *usb_endpoints = 0;
static USB_ERRORS USB_errors = {0};                                      // {71}

/* =================================================================== */
/*                                USB                                  */
/* =================================================================== */


#define fnGetEndPointCtr(iEndPoint) (unsigned char *)(ENDPT0_ADD + ((iEndPoint) * sizeof(unsigned long)))
/*
// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *usb_hardware, unsigned char ucFrameType, KINETIS_USB_BD *ptUSB_BD_rx, KINETIS_USB_ENDPOINT_BD *ptEndpointBD)
{
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting messages to queue

    usb_hardware->ptrEndpoint = &usb_endpoints[iEndpoint_ref];           // mark the present transmit endpoint information for possible subroutine or response use
    if ((usb_endpoints[iEndpoint_ref].ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) {  // set the next transmit pointer details
        usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl; // prepare hardware relevant data for the generic handler's use
        usb_hardware->ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
    }
    else {
        usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl; // prepare hardware relevant data for the generic handler's use
        usb_hardware->ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
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
            ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL); // force stall handshake on both control 0 buffers
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
*/
// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
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
}


#define fnGetEndPointCtrHS(iEndPoint) (USBHS_EPCR0_ADDR + iEndPoint)

static __interrupt void _usb_hs_otg_isr(void);

static unsigned char ucEndpointCount = 0;

// USBHS controller initialisation
//
static void fnUSBHS_init(unsigned char ucEndpoints)
{
    static KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrEndpointQueueHeader = 0;
    int i;

    USBHS_USBMODE = (USBHS_USBMODE_CM_DEVICE | USBHS_USBMODE_ES_LITTLE | USBHS_USBMODE_SLOM/* | USBHS_USBMODE_SDIS*/); // note that stream is disabled so that single rx buffering is possible (this may reduce OUT speed but is simpler, especially when using for software uploading where the speed is limited by flash programming anyway)
    USBHS_EPSETUPSR = 0xffffffff;                                        // clear endpoint setup register (write '1' to clear each flag)
    #if defined _WINDOWS
    USBHS_EPSETUPSR = 0;
    #endif
    // Create an endpoint queue - each header is 48 bytes in size but has to be aligned on 64 byte boundary - the queue itself must be 2k aligned
    //
    if (ptrEndpointQueueHeader == 0) {                                   // allocate endpoint header queue on first call
        unsigned char *ptrEP0_rx;
        ptrEndpointQueueHeader = uMallocAlign((MAX_MALLOC)(sizeof(KINETIS_USBHS_ENDPOINT_QUEUE_HEADER) * ucEndpoints * 2), (2 * 1024)); // get endpoint queue headers starting at a 2k boundary
        for (i = 0; i < (ucEndpoints * 2); i++) {                        // create single transfer block for each receive and transmit pipe and endpoint
            (ptrEndpointQueueHeader + i)->my_dTD = (USB_HS_TRANSFER_OVERLAY *)uMallocAlign((MAX_MALLOC)(32), (32)); // allocate and enter the location of the dedicated transfer block (32 byte size dTD aligned on 32 byte boundary)
            (ptrEndpointQueueHeader + i)->my_dTD->ulNextdTD_pointer = ENDPOINT_QUEUE_HEADER_NEXT_INVALID; // single transfer block
        }
        // Endpoint 0 requires a small buffer in case it needs to receive OUT data (it often receives zero data too so must be primed at least for this reception)
        //
        ptrEP0_rx = uMalloc(64);                                         // allocate 64 bytes for endpoint 0 reception
        ptrEndpointQueueHeader->dTD.ulBufferPointerPage[0] = (unsigned long)ptrEP0_rx;
        ptrEndpointQueueHeader->dTD.ulBufferPointerPage[1] = ((unsigned long)(ptrEP0_rx + (4 * 1024)) & ENDPOINT_QUEUE_HEADER_BUFFER_POINTER_MASK); // two pages defined to allow any page boundary for the first 4k of data
        ptrEndpointQueueHeader->ulBufferLength = ptrEndpointQueueHeader->ulEndpointLength = (64 << 16);
      //ptrEndpointQueueHeader->dTD.ulBufferPointerPage[2] = (ptrUSB_HW->ptrQueueHeader->dTD.ulBufferPointerPage[1] + (4 * 1024)); // further page buffers could be used in case of large reception content
      //ptrEndpointQueueHeader->dTD.ulBufferPointerPage[3] = (ptrUSB_HW->ptrQueueHeader->dTD.ulBufferPointerPage[2] + (4 * 1024));
      //ptrEndpointQueueHeader->dTD.ulBufferPointerPage[4] = (ptrUSB_HW->ptrQueueHeader->dTD.ulBufferPointerPage[3] + (4 * 1024));

        ptrEndpointQueueHeader->CurrentdTD_pointer = ptrEndpointQueueHeader->my_dTD;
        ptrEndpointQueueHeader->dTD.ul_dtToken = (ptrEndpointQueueHeader->ulBufferLength | ENDPOINT_QUEUE_HEADER_TOKEN_IOC | ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE);
        ptrEndpointQueueHeader->my_dTD->ptrBufferStart = ptrEP0_rx;      // insert software pointer reference to the start of the buffer (this is due to the fact that the first page buffer's offset increments with data reception)
        ucEndpointCount = ucEndpoints;
    }
    USBHS_EPLISTADDR = (unsigned long)ptrEndpointQueueHeader;            // set to the endpoint list
    for (i = 0; i < (ucEndpoints * 2); i++) {                            // initialise the device queue headers
        (ptrEndpointQueueHeader + i)->ulCapabilities = ((64 << 16) | ENDPOINT_QUEUE_HEADER_CTL_IOS); // default is maximum 64 byte endpoint size (maximum is 1024 for high-speed USB) - setup tokens generate an interrupt
        (ptrEndpointQueueHeader + i)->dTD.ulNextdTD_pointer = ENDPOINT_QUEUE_HEADER_NEXT_INVALID; // pointer is not yet valid
    }
    USBHS_EPCR0 = (USBHS_EPCR_RXE | USBHS_EPCR_RXR | USBHS_EPCR_TXE | USBHS_EPCR_TXR); // reset data toggle (synchronise) on endpoint 0
    fnEnterInterrupt(irq_USB_HS_ID, PRIORITY_USB_HS_OTG, _usb_hs_otg_isr); //configure and enter the USB handling interrupt routine in the vector table
    USBHS_USBINTR = (USBHS_USBINTR_UE | USBHS_USBINTR_UEE | USBHS_USBINTR_PCE | USBHS_USBINTR_URE); // enable interrupt sources
    USBHS_USBCMD = (USBHS_USBCMD_RS);                                    // set to run mode (in device mode this causes the controller to enable a pull-up on D+ and initiate an attach event
    USBHS_EPPRIME |= (USBHS_EPPRIME_PERB0);                              // prime the reception
    #if defined _WINDOWS
    uMemcpy((void *)ptrEndpointQueueHeader->CurrentdTD_pointer, &ptrEndpointQueueHeader->dTD, (sizeof(ptrEndpointQueueHeader->dTD) - sizeof(unsigned long))); // the USBHS controller automatically copied the dTD content to the transfer buffer
    #endif
}


// This routine checks through all endpoints and stops any that are presently active - all endpoints are disabled
// - it also needs to clean up queues
//
static void fnDeInitEndpoints(void)
{
    KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
    int i;
    volatile unsigned long *ptrEP_Ctrl = USBHS_EPCR0_ADDR;
    unsigned long ulRxEp = USBHS_EPFLUSH_FERB0;
    unsigned long ulTxEp = USBHS_EPFLUSH_FETB0;
    unsigned long ulFlushEndpoints = 0;
    if (ptrQueueHeader == 0) {
        return;                                                          // queue header not yet allocated so ignore
    }
    while (USBHS_EPPRIME != 0) {                                         // cancel all primed status by waiting until all bits in the EPPRIME are 0 and then writing to EPFLUSH
    #if defined _WINDOWS
        USBHS_EPPRIME = 0;
    #endif
    }
    for (i = 0; i < ucEndpointCount; i++) {
        *ptrEP_Ctrl &= ~(USBHS_EPCR_RXE | USBHS_EPCR_RXT_INT | USBHS_EPCR_TXE | USBHS_EPCR_TXT_INT); // disable endpoint rx and tx operation and reset endpoints types
        if (ptrQueueHeader->dTD.ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE) { // if the reception endpoint is presently active (as reset ocurred)
            ulFlushEndpoints |= ulRxEp;                                  // collect the enpoints to be flushed
        }
        ptrQueueHeader++;                                                // move to transmit queue header
        if (ptrQueueHeader->dTD.ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE) { // if the transmission endpoint is presently active (as reset ocurred)
            ulFlushEndpoints |= ulTxEp;                                  // collect the enpoints to be flushed
        }
        ptrQueueHeader++;
        ulRxEp <<= 1;
        ulTxEp <<= 1;
        ptrEP_Ctrl++;
    }

    do {
        USBHS_EPFLUSH = ulFlushEndpoints;                                // command a flush of the active endpoints
        while (USBHS_EPFLUSH != 0) {                                     // this operation may take a large amount of time depending on the USB bus activity and it is not desirable to have it in an interrupt routine
    #if defined _WINDOWS
            USBHS_EPFLUSH = 0;                                           // the flush bit is self-clearing
    #endif
        } 
    } while ((ulFlushEndpoints = USBHS_EPSR) != 0);                      // check whether the flush failed - this can happen when a packet to a particular endpoint was in progress
}


// It has been found that the USBHS controller can sent data from SRAM or FLASH source addresses therefore we don't copy the data into an intermediate buffer
// but instead send it from where it is (assuming that it remains stable)
// - two buffer pointers are prepared, whereby the second is set to a 4k page boundary so that at least 4k of linear input data can be sent even when its buffer crosses a 4k boundary
//
extern void fnTxUSBHS(unsigned char *pData, unsigned short usLen, int iEndpoint, USB_HW *ptrUSB_HW)
{
    KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrTxQueueHeader = (ptrUSB_HW->ptrQueueHeader + 1);

    ptrTxQueueHeader->dTD.ulBufferPointerPage[0] = (unsigned long)pData; // enter the buffer pointers on incremented 4k page boundaries
    ptrTxQueueHeader->dTD.ulBufferPointerPage[1] = ((unsigned long)(pData + (4 * 1024)) & ENDPOINT_QUEUE_HEADER_BUFFER_POINTER_MASK);
  //ptrTxQueueHeader->dTD.ulBufferPointerPage[2] = (ptrTxQueueHeader->dTD.ulBufferPointerPage[1] + (4 * 1024)); // further page buffers could be used for handling large transmissions of more than 4k
  //ptrTxQueueHeader->dTD.ulBufferPointerPage[3] = (ptrTxQueueHeader->dTD.ulBufferPointerPage[2] + (4 * 1024));
  //ptrTxQueueHeader->dTD.ulBufferPointerPage[4] = (ptrTxQueueHeader->dTD.ulBufferPointerPage[3] + (4 * 1024));

    // We don't use a linked list so the first entry is always set
    //
    ptrTxQueueHeader->CurrentdTD_pointer = ptrTxQueueHeader->my_dTD;
    ptrTxQueueHeader->dTD.ul_dtToken = ((usLen << ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT) | ENDPOINT_QUEUE_HEADER_TOKEN_IOC | ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE); // the length of data to be sent with interrupt on completion
    USBHS_EPPRIME |= ((USBHS_EPPRIME_PETB0) << iEndpoint);               // prime the transmission
    _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW);
}

// This routine prepares an endpoints reception buffer to receive data
//
static void fnPrimeReception(unsigned short usLastLength, KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader, int iEndpointNumber)
{
    if (usLastLength != 0) {                                   // if we haven't just received zero data
        ptrQueueHeader->dTD.ulBufferPointerPage[0] = (unsigned long)ptrQueueHeader->my_dTD->ptrBufferStart; // reset the first page buffer pointer
        ptrQueueHeader->dTD.ulBufferPointerPage[1] = ((unsigned long)(ptrQueueHeader->my_dTD->ptrBufferStart + (4 * 1024)) & ENDPOINT_QUEUE_HEADER_BUFFER_POINTER_MASK);
    }
    ptrQueueHeader->dTD.ul_dtToken = (ptrQueueHeader->ulBufferLength | ENDPOINT_QUEUE_HEADER_TOKEN_IOC | ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE);
    USBHS_EPPRIME |= ((USBHS_EPPRIME_PERB0) << iEndpointNumber); // prime the reception again on the endpoint's reception
    #if defined _WINDOWS
    uMemcpy((void *)ptrQueueHeader->CurrentdTD_pointer, &ptrQueueHeader->dTD, (sizeof(ptrQueueHeader->dTD) - sizeof(unsigned long))); // the USBHS controller automatically copied the dTD content to the transfer buffer
    #endif
}

// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessHSInput(int iEndpoint_ref, USB_HW *usb_hardware, unsigned char ucFrameType, unsigned char *ptrData)
{
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting messages to queue
    switch (fnUSB_handle_frame(ucFrameType, ptrData, iEndpoint_ref, usb_hardware)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        fnTxUSBHS(0, 0, iEndpoint_ref, usb_hardware);
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        uEnable_Interrupt();
        return MAINTAIN_OWNERSHIP;
    case STALL_ENDPOINT:                                                 // send stall
        if (iEndpoint_ref == 0) {                                        // check whether control 0 endpoint
            USBHS_EPCR0 |= (USBHS_EPCR_TXS);                             // the stall is cleared when a following SETUP packet is received
            fnSetUSBEndpointState(0, USB_ENDPOINT_STALLED);       
            _SIM_USB(USB_SIM_STALL, 0, usb_hardware);
        }
        else {                                                           // assume IN endpoint is always one higher than OUT endpoint
            int iIN_ref = fnGetPairedIN(iEndpoint_ref);                  // get the paired IN endpoint reference
            *(USBHS_EPCR0_ADDR + iIN_ref) |= (USBHS_EPCR_TXS);           // stall
            fnSetUSBEndpointState(iIN_ref, USB_ENDPOINT_STALLED);        // mark the stall at the IN endpoint
            _SIM_USB(USB_SIM_STALL, iIN_ref, usb_hardware);
        }
        break;
    }
    if (ucFrameType != USB_SETUP_FRAME) {                                // setup frames don't require reception to be re-enabled
        fnPrimeReception(usb_hardware->usLength, usb_hardware->ptrQueueHeader, iEndpoint_ref); // prepare for next reception
    }
    uEnable_Interrupt();
    return 0;
}

// High-speed USB interrupt handler
//
static __interrupt void _usb_hs_otg_isr(void)
{
    #define USBHS_STATE_RESETTING 0x01
    #define USBHS_STATE_SUSPENDED 0x02
    static unsigned char ucUSBHS_state = 0;
    unsigned char ucSetupBuffer[8];
    unsigned long ulInterrupts;

    while ((ulInterrupts = (USBHS_USBSTS & USBHS_USBINTR)) != 0) {       // while enabled interrupt pending
        USBHS_USBSTS = ulInterrupts;                                     // clear all interrupts that will be handled
    #if defined _WINDOWS
        USBHS_USBSTS = 0;
    #endif
        if ((ulInterrupts & USBHS_USBINTR_URE) != 0) {                   // handle USB reset interrupt
            fnDeInitEndpoints();                                         // abort any active enpoints and free transfer buffers
            USBHS_EPCR0 = (USBHS_EPCR_RXE | USBHS_EPCR_RXR | USBHS_EPCR_TXE | USBHS_EPCR_TXR); // reset data toggle (synchronise) on endpoint 0
            USBHS_EPSETUPSR = USBHS_EPSETUPSR;                           // clear all setup token semaphores by reading the EPSETUSR register and writing the same value back
            USBHS_EPCOMPLETE = USBHS_EPCOMPLETE;                         // clear all the endpoint complete status bits by reading the EPCOMPLETE register and writing the same value back
    #if defined _WINDOWS
            USBHS_EPSETUPSR = 0;
            USBHS_EPCOMPLETE = 0;
            USBHS_USBSTS = USBHS_USBINTR_PCE;                            // usually a port change interrupt follows a reset
    #endif
            if ((USBHS_PORTSC1 & USBHS_PORTSC1_PR) == 0) {               // if we are too slow responding the port reset will have completed
                USBHS_USBCMD &= ~(USBHS_USBCMD_RS);                      // ensure not in run mode
                USBHS_USBCMD = USBHS_USBCMD_RST;                         // command a hardware reset
                fnUSBHS_init(ucEndpointCount);                           // re-initialise the controller
                ucUSBHS_state = 0;
            }
            else {
                ucUSBHS_state = USBHS_STATE_RESETTING;                   // the ulpi is still detecting the reset state
            }

            USBHS_USBINTR &= ~(USBHS_USBINTR_SLE);                       // disable the suspend interrupt
            USBHS_DEVICEADDR = 0;                                        // reset device address
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting messages to queue
                fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware); // generic handler routine
                usb_hardware.ucUSBAddress = 0;                           // reset the address to revert back to the default state
            uEnable_Interrupt();                                         // re-enable interrupts
        }

        if ((ulInterrupts & USBHS_USBINTR_UE) != 0) {                    // handle transfer complete interrupt
            unsigned long ulBit = USBHS_EPSETUPSR_SETUP0;
            unsigned long ulEndpointBitReference;            
            int iEndpointNumber = 0;
                ulEndpointBitReference = USBHS_EPSETUPSR;                // the transfer endpoint (status is not cleared here since it is cleared when the setup data is read from the buffer)
                while (ulEndpointBitReference != 0) {                    // first handle all waiting setup tokens
                    if ((ulBit & ulEndpointBitReference) != 0) {
                        usb_hardware.ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR + (2 * iEndpointNumber);
                        USBHS_EPSETUPSR = ulBit;                         // clear the status bit
                        // Setup reception data is special in that it is stored directly in the 8 byte space in the queue header
                        // - we extract it here, with help from the tripwire semaphore
                        //
                        do {
                            USBHS_USBCMD |= (USBHS_USBCMD_SUTW);         // setup tripwire semaphore
                            uMemcpy(ucSetupBuffer, usb_hardware.ptrQueueHeader->ucSetupBuffer, 8); // copy the fixed length setup content locally
                        } while ((USBHS_USBCMD & USBHS_USBCMD_SUTW) == 0); // if the hardware has cleared the semaphone we need to repeat to be sure of correct data
                        USBHS_USBCMD &= ~USBHS_USBCMD_SUTW;              // remove tripwire semaphore
                        usb_hardware.usLength = 8;                       // {3}
                        while ((USBHS_EPSETUPSR & ulEndpointBitReference) != 0) {
    #if defined _WINDOWS
                            USBHS_EPSETUPSR &= ~ulEndpointBitReference;
    #endif
                        }
                        USBHS_EPPRIME |= (USBHS_EPPRIME_PERB0 << iEndpointNumber); // prime endpoint 0 reception for OUT data that will follow
                        fnProcessHSInput(iEndpointNumber, &usb_hardware, USB_SETUP_FRAME, ucSetupBuffer);
                        ulEndpointBitReference &= ~(ulBit);              // handled
                    }
                    iEndpointNumber++;
                    ulBit <<= 1;
                }
                ulEndpointBitReference = USBHS_EPCOMPLETE;
                USBHS_EPCOMPLETE = ulEndpointBitReference;               // clear the endpoints since we are going to handle them here
    #if defined _WINDOWS
                USBHS_EPCOMPLETE = 0;
    #endif
                ulBit = USBHS_EPCOMPLETE_ERCE0;
                iEndpointNumber = 0;
                while ((ulEndpointBitReference & 0x0000ffff) != 0) {     // handle all waiting receptions
                    if ((ulBit & ulEndpointBitReference) != 0) {
                        usb_hardware.ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR + (2 * iEndpointNumber);
                        usb_hardware.usLength = (unsigned short)((usb_hardware.ptrQueueHeader->ulBufferLength >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT) - (usb_hardware.ptrQueueHeader->my_dTD->ul_dtToken >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT)); // the length of the present frame
                        if ((*(USBHS_EPCR0_ADDR + iEndpointNumber) & USBHS_EPCR_RXT_INT) != 0) { // check whether non-control endpoint
                             fnProcessHSInput(iEndpointNumber, &usb_hardware, USB_OUT_FRAME, usb_hardware.ptrQueueHeader->my_dTD->ptrBufferStart); // non-control endpoint
                        }
                        else {
                             fnProcessHSInput(iEndpointNumber, &usb_hardware, USB_CONTROL_OUT_FRAME, usb_hardware.ptrQueueHeader->my_dTD->ptrBufferStart); // control endpoint
                        }
                        ulEndpointBitReference &= ~(ulBit);
                    }
                    iEndpointNumber++;
                    ulBit <<= 1;
                }
                ulBit = USBHS_EPCOMPLETE_ETCE0;
                iEndpointNumber = 0;
                while ((ulEndpointBitReference & 0xffff0000) != 0) {     // then handle all transmission complete interrupts
                    if ((ulBit & ulEndpointBitReference) != 0) {
                        usb_hardware.ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR + (2 * iEndpointNumber);
                        fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpointNumber, &usb_hardware); // handle ack event
                        USBHS_DEVICEADDR = (usb_hardware.ucUSBAddress << USBHS_DEVICEADDR_USBADR_SHIFT); // program the address to be used
                        ulEndpointBitReference &= ~(ulBit);
                    }
                    iEndpointNumber++;
                    ulBit <<= 1;
                }
        }
        if ((ulInterrupts & USBHS_USBINTR_PCE) != 0) {                   // handle port change interrupt
    #if defined _WINDOWS
            USBHS_PORTSC1 = 0;
    #endif
            ucUSBHS_state &= ~USBHS_STATE_RESETTING;                     // a reset must be complete when a port change interrupt is received
            if ((USBHS_PORTSC1 & USBHS_PORTSC1_SUSP) == 0) {             // not in the suspended state
                if ((ucUSBHS_state & USBHS_STATE_SUSPENDED) != 0) {      // resume detected - 10ms inverted idle USB bus state (low speed D+ = 1, D- = 0 / high speed D+ = 0, D- = 1)
                    ucUSBHS_state &= ~USBHS_STATE_SUSPENDED;             // no longer in the suspend state
                    uDisable_Interrupt();                                // ensure interrupts remain blocked when putting messages to queue
                        fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);// generic handler routine
                    uEnable_Interrupt();                                 // re-enable interrupts
                }
            }
        }
        if ((ulInterrupts & USBHS_USBINTR_UEE) != 0) {                   // handle USB error interrupt (these are not expected but such events could be debugged here)
        }
        if ((ulInterrupts & USBHS_USBINTR_SLE) != 0) {                   // suspend state detected - 3ms of idle USB bus detected (low speed D+ = 0, D- = 1 / high speed D+ = 1, D- = 0)
            ucUSBHS_state |= USBHS_STATE_SUSPENDED;
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting messages to queue
                fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);       // generic handler routine
            uEnable_Interrupt();                                         // re-enable interrupts
        }
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

/*
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
                    usb_hardware.ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
                }
                else {
                    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl;
                    usb_hardware.ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
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
}*/

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW **ptr_usb_hardware)
{
    if ((*(USBHS_EPCR0_ADDR + iEndpoint) & USBHS_EPCR_TXE) == 0) {       // if the particular endpoint is not enabled for transmission 
        return ENDPOINT_NOT_ACTIVE;
    }
    (*ptr_usb_hardware)->ucDeviceType = USB_DEVICE_HS;
    (*ptr_usb_hardware)->ptrEndpoint = &usb_endpoints[iEndpoint];
    (*ptr_usb_hardware)->ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
    (*ptr_usb_hardware)->ptrQueueHeader += (2 * iEndpoint);
    return ENDPOINT_FREE;
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength, USB_ENDPOINT *ptrEndpoint)
{
    KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader;
    volatile unsigned long *ptrEndPointCtr = fnGetEndPointCtrHS(ucEndpointRef);
    unsigned long  endpoint_config;
        #if defined USB_SIMPLEX_ENDPOINTS                                // {1}
    unsigned long original_endpoint_config;
        #else
    *ptrEndPointCtr &= ~(USBHS_EPCR_RXE | USBHS_EPCR_TXE);               // disable endpoint transmitter and receiver
        #endif
    usb_endpoints[ucEndpointRef].ulEndpointSize = 0;
    usb_endpoints[ucEndpointRef].ulNextRxData0 = 0;

    if (ENDPOINT_DISABLE == ucEndpointType) {
        #if defined USB_SIMPLEX_ENDPOINTS
        *ptrEndPointCtr = 0;                                             // disable endpoint transmitter and receiver
        #endif
        USBHS_USBINTR &= ~(USBHS_USBINTR_SLE);                           // disable suspend interrupt when not configured
        return;
    }
    USBHS_USBSTS = USBHS_USBINTR_SLE;                                    // clear possible pending suspend interrupt
        #if defined _WINDOWS
    USBHS_USBSTS &= ~(USBHS_USBINTR_SLE);
        #endif
    USBHS_USBINTR |= (USBHS_USBINTR_SLE);                                // since we have just been configured, enable suspend interrupt

    #if defined USB_SIMPLEX_ENDPOINTS
    original_endpoint_config = *ptrEndPointCtr;                          // {1} backup original endpoint configuration
    #endif
    switch (ucEndpointType & ~IN_ENDPOINT) {
    case ENDPOINT_ISOCHRONOUS:
        endpoint_config = (USBHS_EPCR_TXE | USBHS_EPCR_TXT_ISO | USBHS_EPCR_RXE | USBHS_EPCR_RXT_ISO); 
        break;
    case ENDPOINT_BULK:
        endpoint_config = (USBHS_EPCR_TXE | USBHS_EPCR_TXT_BULK | USBHS_EPCR_RXE | USBHS_EPCR_RXT_BULK);
        break;
    case ENDPOINT_INTERRUPT:
        endpoint_config = (USBHS_EPCR_TXE | USBHS_EPCR_TXT_INT | USBHS_EPCR_RXE | USBHS_EPCR_RXT_INT);
        break;
    case ENDPOINT_CONTROL:
    default:
        endpoint_config = (USBHS_EPCR_TXE | USBHS_EPCR_TXT_CON | USBHS_EPCR_RXE | USBHS_EPCR_RXT_CON);
        break;
    }
    ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR + (2 * ucEndpointRef); // reception queue header
    if ((ucEndpointType & IN_ENDPOINT) == 0) {                           // output type endpoint (or control)
        if ((ucEndpointType & ~IN_ENDPOINT) != ENDPOINT_CONTROL) {
            endpoint_config &= ~(USBHS_EPCR_TXE | USBHS_EPCR_TXT_INT);   // disable transmission
        }
        if (ptrQueueHeader->my_dTD->ptrBufferStart == 0) {               // if the endpoint is being used for the first time
            if (usMaxEndpointLength > usEndpointLength) {                // if no larger specified take the value
                usEndpointLength = usMaxEndpointLength;
            }
            ptrQueueHeader->ulBufferLength = usEndpointLength;
            if ((ptrEndpoint->usParameters & USB_OUT_ZERO_COPY) != 0) {  // {2} use zero copy type of endpoint buffer
                QUEQUE *ptrQueue = (QUEQUE *)(ptrEndpoint->ptrEndpointOutCtr);
                ptrQueue->new_chars = 0;
                ptrQueue->put = ptrQueue->QUEbuffer;
                ptrQueueHeader->my_dTD->ptrBufferStart = ptrQueue->QUEbuffer; // set descriptor data pointer to start of linear reception buffer
            }
            else {
                ptrQueueHeader->my_dTD->ptrBufferStart = (unsigned char *)uMalloc((MAX_MALLOC)(ptrQueueHeader->ulBufferLength));
            }
            ptrQueueHeader->ulBufferLength <<= 16;                       // move the length into position for later register use
        }
        ptrQueueHeader->ulCapabilities = ptrQueueHeader->ulEndpointLength = (usEndpointLength << 16);
        if ((ucEndpointType & ~IN_ENDPOINT) != ENDPOINT_CONTROL) {
            ptrQueueHeader->ulCapabilities |= ENDPOINT_QUEUE_HEADER_ZLT; // disable zero data frame on non-control endpoints
        }

        // Prepare for reception on this endpoint
        //
        ptrQueueHeader->my_dTD->ulNextdTD_pointer = ENDPOINT_QUEUE_HEADER_NEXT_INVALID; // single transfer block
        ptrQueueHeader->CurrentdTD_pointer = ptrQueueHeader->my_dTD;
        ptrQueueHeader->dTD.ulBufferPointerPage[0] = (unsigned long)(ptrQueueHeader->my_dTD->ptrBufferStart);
        ptrQueueHeader->dTD.ulBufferPointerPage[1] = ((unsigned long)(ptrQueueHeader->dTD.ulBufferPointerPage[0] + (4 * 1024)) & ENDPOINT_QUEUE_HEADER_BUFFER_POINTER_MASK);
        fnPrimeReception(0, ptrQueueHeader, ucEndpointRef);
    }
    if ((ucEndpointType & IN_ENDPOINT) || ((ucEndpointType & ~IN_ENDPOINT) == ENDPOINT_CONTROL)) { // input type endpoint (or control)
        ptrQueueHeader++;                                                // move to transmission queue header
        ptrQueueHeader->ulCapabilities = ptrQueueHeader->ulEndpointLength = (usEndpointLength << 16);
        if ((ucEndpointType & ~IN_ENDPOINT) != ENDPOINT_CONTROL) {
            ptrQueueHeader->ulCapabilities |= ENDPOINT_QUEUE_HEADER_ZLT; // disable zero data frame on non-control endpoints
            endpoint_config &= ~(USBHS_EPCR_RXE | USBHS_EPCR_RXT_INT);   // disable reception
        }
    }
    #if defined USB_SIMPLEX_ENDPOINTS
    endpoint_config |= original_endpoint_config;                         // {1} combine original with new endpoint configuration to allow shared IN/OUT operation
    #endif
    *ptrEndPointCtr = endpoint_config;                                   // set the endpoint mode
    _SIM_USB(USB_SIM_ENUMERATED,0,0);                                    // inform the simulator that USB enumeration has completed
}

// This routine is called with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    unsigned short usLength;
    KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR + (2 * ucEndpointRef);
    if (ptrQueueHeader->dTD.ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE) { // no data available
        return USB_BUFFER_NO_DATA;
    }
    usLength = (unsigned short)((ptrQueueHeader->ulBufferLength >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT) - (ptrQueueHeader->my_dTD->ul_dtToken >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT)); // the length of the present frame
    if (fnEndpointData(ucEndpointRef, ptrQueueHeader->my_dTD->ptrBufferStart, usLength, OUT_DATA_RECEPTION, 0) != MAINTAIN_OWNERSHIP) {
        fnPrimeReception(usLength, ptrQueueHeader, ucEndpointRef);       // re-enable reception on this endpoint
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
    usb_hardware.ucDeviceType = USB_DEVICE_HS;                           // mark that the HS controller is being used

    #if defined ENABLE_HSUSB_TRANSCEIVER
    ENABLE_HSUSB_TRANSCEIVER();
    #endif
    #if defined MPU_AVAILABLE
    MPU_CESR = 0;                                                        // allow concurrent access to MPU controller
    #endif
    FMC_PFAPR |= FMC_FPAPR_USB_HS;                                       // allow USBHS controller to read from Flash

    #if defined KINETIS_WITH_USBPHY                                      // device with integrated HS PHY
        POWER_UP(3, SIM_SCGC3_USBHS);                                    // power up the USB HS controller module
        // Requirements for operation are:
        // - VREGIN0 or VREGIN1 connected to 5V so that 3.3V USB is valid
        // - 32kHz slow clock is enabled
        // - external reference clock is enabled and is 12MHz, 16MHz or 24MHz
        // 
        MCG_C1 |= MCG_C1_IRCLKEN;                                        // 32kHz IRC enable
        OSC0_CR |= OSC_CR_ERCLKEN;                                       // external reference clock enable
        SIM_SOPT2 |= SIM_SOPT2_USBREGEN;                                 // enable USB PHY PLL regulator
        POWER_UP(3, SIM_SCGC3_USBHSPHY);                                 // enable clocks to PHY
        SIM_USBPHYCTL = (SIM_USBPHYCTL_USBVOUTTRG_3_310V | SIM_USBPHYCTL_USBVREGSEL); // 3.310V source VREG_IN1 (in case both are powered)
        USBPHY_TRIM_OVERRIDE_EN = 0x0000001f;                            // override IFR values
        USBPHY_CTRL = (USBPHY_CTRL_ENUTMILEVEL2 | USBPHY_CTRL_ENUTMILEVEL3); // release PHY from reset and enable its clock
        #if _EXTERNAL_CLOCK == 24000000
        USBPHY_PLL_SIC = (USBPHY_PLL_SIC_PLL_POWER | USBPHY_PLL_SIC_PLL_ENABLE | USBPHY_PLL_SIC_PLL_BYPASS | USBPHY_PLL_SIC_PLL_DIV_SEL_24MHz); // power up PLL to run at 480MHz from 24MHz clock input
        #elif _EXTERNAL_CLOCK == 16000000
        USBPHY_PLL_SIC = (USBPHY_PLL_SIC_PLL_POWER | USBPHY_PLL_SIC_PLL_ENABLE | USBPHY_PLL_SIC_PLL_BYPASS | USBPHY_PLL_SIC_PLL_DIV_SEL_16MHz); // power up PLL to run at 480MHz from 16MHz clock input
        #elif _EXTERNAL_CLOCK == 12000000
        USBPHY_PLL_SIC = (USBPHY_PLL_SIC_PLL_POWER | USBPHY_PLL_SIC_PLL_ENABLE | USBPHY_PLL_SIC_PLL_BYPASS | USBPHY_PLL_SIC_PLL_DIV_SEL_12MHz); // {4} power up PLL to run at 480MHz from 12MHz clock input
        #else
            #error "USB PLL requires an external reference of 12MHz, 16MHz or 24MHz!"
        #endif
        #if defined ERRATA_ID_9712 && defined EXTERNAL_CLOCK
        // Error 9712 workaround is being enabled - advise use of crystal rather than external clock!
        //
        MCG_C2 |= MCG_C2_EREFS;                                          // pretend that crystal is being used so that the PLL will lock
        #endif
        USBPHY_PLL_SIC &= ~(USBPHY_PLL_SIC_PLL_BYPASS);                  // cear the bypass
        USBPHY_PLL_SIC |= (USBPHY_PLL_SIC_PLL_EN_USB_CLKS);              // enable USB clock output from PHY PLL
        while ((USBPHY_PLL_SIC & USBPHY_PLL_SIC_PLL_LOCK) == 0) {        // wait for the PLL to lock
        #if defined _WINDOWS
            USBPHY_PLL_SIC |= USBPHY_PLL_SIC_PLL_LOCK;
        #endif
        }
        #if defined ERRATA_ID_9712 && defined EXTERNAL_CLOCK
        // Error 9712 workaround is being enabled - advise use of crystal rather than external clock!
        //
      //MCG_C2 &= ~MCG_C2_EREFS;                                         // remove the external reference from oscillator requested flag
        #endif
        USBPHY_PWD = 0;                                                  // for normal operation
        USBPHY_ANACTRL = ((24 << 4) | 4);                                // frac = 24 and  Clk /4
        while ((USBPHY_ANACTRL & 0x80000000) == 0) {
        #if defined _WINDOWS
            USBPHY_ANACTRL |= 0x80000000;
        #endif
        }
        USBPHY_TX |= (1 << 24);                                          // reserved??
    #else
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
    #endif

    if (ucEndpoints > NUMBER_OF_USBHS_ENDPOINTS) {                       // limit endpoint count
        ucEndpoints = NUMBER_OF_USBHS_ENDPOINTS;                         // limit to maximum available in device
    }

    usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * ucEndpoints)); // get endpoint control structures
    fnUSBHS_init(ucEndpoints);
}
#endif
