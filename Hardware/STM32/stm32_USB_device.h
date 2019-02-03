/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      stm32_USB_device.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/

#if defined _USB_DEVICE_DRIVER_CODE && defined USB_DEVICE_AVAILABLE

static USB_HW usb_hardware = {{{0}}};
static unsigned long USB_errors = 0;

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


// This routine is called with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    return USB_BUFFER_BLOCKED;
}


// Return USB error counters
//
extern unsigned long fnUSB_error_counters(int iValue)                    // {24}
{
    // No error counters supported 
    //
    return USB_errors;
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
    volatile unsigned long *ptrEndpointControl = USB_EP0R_ADD;
    ptrEndpointControl += iEndpoint;
    switch (*ptrEndpointControl & USB_EPR_CTR_STAT_TX_MASK) {
    case USB_EPR_CTR_STAT_TX_NAK:
        return ENDPOINT_FREE;                                            // there is presently no data being transferred
    case USB_EPR_CTR_STAT_TX_DISABLED:
        return ENDPOINT_NOT_ACTIVE;                                      // endpoint is not enabled
  //case USB_EPR_CTR_STAT_TX_VALID:
  //case USB_EPR_CTR_STAT_TX_STALL:
    default:
        return ENDPOINT_NOT_FREE;                                        // endpoint is presently busy so no data may be copied
    }
}

// Function for setting reception buffer length to buffer descriptor
//
static unsigned short fnSetRxLength(unsigned short usEndpointLength)
{
    if (usEndpointLength <= 62) {
        return (unsigned short)(((((usEndpointLength + 1) / 2) << USB_COUNT_SHIFT) & USB_COUNT_RX_NUM_BLOCK_MASK)); // valid for 2, 3, 6, ..62 (<2 not allowed)
    }
    else {
        return (unsigned short)((((((usEndpointLength + 31) / 32) - 1) << USB_COUNT_SHIFT) & USB_COUNT_RX_NUM_BLOCK_MASK) | USB_COUNT_RX_BL_SIZE); // valid for 32, 64, 96, .. 1024 (rounded up to allowed sizes)
    }
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength, USB_ENDPOINT *ptrEndpoint)
{
    static unsigned short usNextBuffer = 0;
    unsigned long ulEndpoint_config;
    volatile unsigned long *ptr_ulEndpointCtrl;
    USB_BD_TABLE *ptrBD = (USB_BD_TABLE *)USB_CAN_SRAM_ADDR;             // start of dedicated SRAM area
    if (ENDPOINT_DISABLE == ucEndpointType) {                            // all endpoints are automatically deactivated when there is a USB reset
        usNextBuffer = (((NUMBER_OF_USB_ENDPOINTS * sizeof(USB_BD_TABLE) / sizeof(unsigned short))) + ENDPOINT_0_SIZE);
        return;
    }
    ptr_ulEndpointCtrl = USB_EP0R_ADD;
    ptr_ulEndpointCtrl += ucEndpointRef;

    switch ((ucEndpointType & ~(IN_ENDPOINT))) {
    case ENDPOINT_ISOCHRONOUS:
        ulEndpoint_config = (USB_EPR_CTR_EP_TYPE_ISO);
        break;
    case ENDPOINT_BULK:
        ulEndpoint_config = (USB_EPR_CTR_EP_TYPE_BULK);
        break;
    case ENDPOINT_INTERRUPT:
        ulEndpoint_config = (USB_EPR_CTR_EP_TYPE_INTERRUPT);
        break;
    case ENDPOINT_CONTROL:
    default:
        ulEndpoint_config = (USB_EPR_CTR_EP_TYPE_CONTROL);
        break;
    }

    if (usMaxEndpointLength > usEndpointLength) {                        // if no larger specified take the specified value
        usEndpointLength = usMaxEndpointLength;
    }
    ptrBD += ucEndpointRef;
    ptrBD->usUSB_COUNT_TX_0 = 0;
    if ((ucEndpointType & IN_ENDPOINT) != 0) {                           // IN type endpoint
        ulEndpoint_config |= USB_EPR_CTR_STAT_TX_NAK;                    // transmitter active but returns NAKs initially
        ptrBD->usUSB_ADDR_TX = usNextBuffer;                             // buffer location for transmission
    }
    else {                                                               // OUT type endpoint
        ulEndpoint_config |= USB_EPR_CTR_STAT_RX_VALID;                  // receiver can accept data
        ptrBD->usUSB_COUNT_RX_0 = fnSetRxLength(usEndpointLength);
        ptrBD->usUSB_ADDR_RX = usNextBuffer;                             // buffer location for reception
    }
    usNextBuffer += (usEndpointLength);
    ulEndpoint_config |= ucEndpointRef;
    #if defined _WINDOWS
    *ptr_ulEndpointCtrl |= ulEndpoint_config;                            // set endpoint configuration
    #else
    *ptr_ulEndpointCtrl = ulEndpoint_config;                             // set endpoint configuration
    #endif
    _SIM_USB(USB_SIM_ENUMERATED, 0, 0);                                  // inform the simulator that USB enumeration has completed
}


#if 0
HAL_StatusTypeDef USB_ActivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep)
{

    { // double buffered reference
        /*Set the endpoint as double buffered*/
        PCD_SET_EP_DBUF(USBx, ep->num);
        /*Set buffer address for double buffered mode*/
        PCD_SET_EP_DBUF_ADDR(USBx, ep->num, ep->pmaaddr0, ep->pmaaddr1);

        if (ep->is_in == 0)
        {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            PCD_CLEAR_RX_DTOG(USBx, ep->num);
            PCD_CLEAR_TX_DTOG(USBx, ep->num);

            /* Reset value of the data toggle bits for the endpoint out*/
            PCD_TX_DTOG(USBx, ep->num);

            PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_VALID);
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
        }
        else
        {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            PCD_CLEAR_RX_DTOG(USBx, ep->num);
            PCD_CLEAR_TX_DTOG(USBx, ep->num);
            PCD_RX_DTOG(USBx, ep->num);
            /* Configure DISABLE status for the Endpoint*/
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
            PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
        }
    }
}

HAL_StatusTypeDef USB_DeactivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep)
{

    /*Double Buffer*/

    {
        if (ep->is_in == 0)
        {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            PCD_CLEAR_RX_DTOG(USBx, ep->num);
            PCD_CLEAR_TX_DTOG(USBx, ep->num);

            /* Reset value of the data toggle bits for the endpoint out*/
            PCD_TX_DTOG(USBx, ep->num);

            PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
        }
        else
        {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            PCD_CLEAR_RX_DTOG(USBx, ep->num);
            PCD_CLEAR_TX_DTOG(USBx, ep->num);
            PCD_RX_DTOG(USBx, ep->num);
            /* Configure DISABLE status for the Endpoint*/
            PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
            PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
        }
    }
}
#endif

// Copy data to the TxFIFO and start transmission
//
extern void fnSendUSB_data(unsigned char *pData, unsigned short Len, int iEndpoint, USB_HW *ptrUSB_HW)
{
    register unsigned long ulNextEntry = 0;
    USB_BD_TABLE *ptrBD = (USB_BD_TABLE *)USB_CAN_SRAM_ADDR;             // the start of the buffer descriptor table (we dont use an offset)
    unsigned long *ptrEndpointControl = (unsigned long *)USB_EP0R_ADD;
    volatile unsigned long *ptrOutputBuffer = (USB_CAN_SRAM_ADDR + USB_BTABLE);
    ptrBD += iEndpoint;
    ptrEndpointControl += iEndpoint;
    ptrOutputBuffer += (ptrBD->usUSB_ADDR_TX/2);
    ptrBD->usUSB_COUNT_TX_0 = Len;
    while (Len != 0) {
        ulNextEntry = *pData++;
        if (Len >= 2) {
            ulNextEntry |= (*pData++ << 8);
            Len -= 2;
        }
        else {
            Len--;
        }
        *ptrOutputBuffer++ = ulNextEntry;
    }
    #if defined _WINDOWS
    *ptrEndpointControl = ((*ptrEndpointControl & ~(USB_EPR_CTR_CTR_TX | USB_EPR_CTR_STAT_TX_VALID)) | (USB_EPR_CTR_STAT_TX_VALID));
    #else
    *ptrEndpointControl = (((*ptrEndpointControl & ~(USB_EPR_CTR_DTOG_RX | USB_EPR_CTR_STAT_RX_MASK | USB_EPR_CTR_CTR_TX | USB_EPR_CTR_DTOG_TX)) ^ (USB_EPR_CTR_STAT_TX_VALID))/* | USB_EPR_CTR_DTOG_TX*/); // start transmission
    #endif
    _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW);
}

// Send a zero data frame
//
extern void fnSendZeroData(USB_HW *ptrUSB_HW, int iEndpoint)
{
    fnSendUSB_data(0, 0, iEndpoint, ptrUSB_HW);
}

// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
    volatile unsigned long *ptrEndpointControl = USB_EP0R_ADD;
    ptrEndpointControl += (ucEndpoint & ~IN_ENDPOINT);
    if ((ucEndpoint & IN_ENDPOINT) != 0) {
#if defined _WINDOWS
        *ptrEndpointControl &= ~(USB_EPR_CTR_STAT_TX_MASK);
        *ptrEndpointControl |= (USB_EPR_CTR_STAT_TX_NAK);                    // unstall IN endpoint
#else
        *ptrEndpointControl = ((*ptrEndpointControl & ~(USB_EPR_CTR_DTOG_RX | USB_EPR_CTR_STAT_RX_MASK | USB_EPR_CTR_DTOG_TX)) ^ USB_EPR_CTR_STAT_TX_VALID); // stall control endpoint (move from disabled to NAK)
#endif
    }
    else {
        _EXCEPTION("To do!!");
    }
}


static void fnPullUSB_data(unsigned char *ptrOutput, unsigned long *ptrInputBuffer, unsigned short usLength)
{
    unsigned long ulValue;
    while (usLength != 0) {
        ulValue = *ptrInputBuffer++;
        *ptrOutput++ = (unsigned char)ulValue;
        if (usLength > 1) {
            *ptrOutput++ = (unsigned char)(ulValue >> 8);
            usLength -= 2;
        }
        else {
            usLength--;
        }
    }
}

extern unsigned char fnGetUSB_data(unsigned char **ptrData, int iInc)
{
    register unsigned char *ptrBuffer = *ptrData;
    unsigned char ucValue;
    switch (((CAST_POINTER_ARITHMETIC)ptrBuffer) & 0x3) {
    case 0:
        ucValue = *ptrBuffer;
        if (iInc != 0) {
            *ptrData = (ptrBuffer + 1);
        }
        break;
    case 1:
        ucValue = *ptrBuffer;
        if (iInc != 0) {
            *ptrData = (ptrBuffer + 3);
        }
        break;
    case 2:
    case 3:
        _EXCEPTION("Unexpected pointer");
        return 0;
    }
    return ucValue;
}

// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, unsigned char ucFrameType)
{
    int iReturn;
    USB_BD_TABLE *ptUSB_BD = (USB_BD_TABLE *)USB_CAN_SRAM_ADDR;
    volatile unsigned long *ptrInputBuffer = (USB_CAN_SRAM_ADDR);        // the start of the buffer descriptor table (we dont use an offset)
    volatile unsigned long *ptrEndpointControl;
    unsigned short usEndpointLength;
    unsigned char ucInputBuffer[1024];                                   // temporary buffer for extracting the (maximum size of) USB reception data to
    ptUSB_BD += iEndpoint_ref;
    usEndpointLength = ptUSB_BD->usUSB_COUNT_RX_0;
    usb_hardware.usLength = (usEndpointLength & USB_COUNT_COUNT_MASK);
    ptrInputBuffer += (ptUSB_BD->usUSB_ADDR_RX/2);
    fnPullUSB_data(ucInputBuffer, (unsigned long *)ptrInputBuffer, usb_hardware.usLength);
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting messages to queue
    switch (iReturn = fnUSB_handle_frame(ucFrameType, ucInputBuffer, iEndpoint_ref, &usb_hardware)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        FNSEND_ZERO_DATA(&usb_hardware, iEndpoint_ref);
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later (frame data has not been consumed)
        uEnable_Interrupt();
        return iReturn;
    case STALL_ENDPOINT:                                                 // send stall
        ptrEndpointControl = USB_EP0R_ADD;
        if (iEndpoint_ref != 0) {
            iEndpoint_ref = fnGetPairedIN(iEndpoint_ref);                // get the paired IN endpoint reference
            ptrEndpointControl += iEndpoint_ref;
        }
#if defined _WINDOWS
        *ptrEndpointControl &= ~(USB_EPR_CTR_STAT_TX_MASK);
        *ptrEndpointControl |= (USB_EPR_CTR_STAT_TX_STALL);              // stall control endpoint
#else
        *ptrEndpointControl = (((*ptrEndpointControl & ~(USB_EPR_CTR_DTOG_RX | USB_EPR_CTR_STAT_RX_MASK | USB_EPR_CTR_DTOG_TX)) ^ USB_EPR_CTR_STAT_TX_STALL) | USB_EPR_CTR_STAT_TX_NAK); // stall control endpoint
#endif
        fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);
        _SIM_USB(USB_SIM_STALL, iEndpoint_ref, &usb_hardware);
        break;
    default:
        break;
    }
    uEnable_Interrupt();
    return 0;
}

volatile extern int iTrigger;
// USB device FS interrupt handler (high priority interrupts) - only on isochronous or double-buffered endpoints
//
static __interrupt void USB_Device_HP_Interrupt(void)
{
    register unsigned char ucFrameType;
    register unsigned long ulInterrupts = USB_ISTR;
    int iEndpoint_ref = (ulInterrupts & USB_ISTR_EP_ID_MASK);            // the end point interrupting
    volatile unsigned long *ptrEndpointControl = USB_EP0R_ADD;
    ptrEndpointControl += iEndpoint_ref;
    if ((ulInterrupts & USB_ISTR_DIR) != 0) {                            // OUT
        if ((*ptrEndpointControl & USB_EPR_CTR_SETUP) != 0) {            // SETUP
            ucFrameType = USB_SETUP_FRAME;
        }
        else {                                                           // OUT
            ucFrameType = USB_OUT_FRAME;
        }
        if (fnProcessInput(iEndpoint_ref, ucFrameType) != MAINTAIN_OWNERSHIP) {
#if defined _WINDOWS
            *ptrEndpointControl &= ~(USB_EPR_CTR_STAT_RX_MASK | USB_EPR_CTR_CTR_RX);
            *ptrEndpointControl |= (USB_EPR_CTR_STAT_RX_VALID);
            *ptrEndpointControl ^= (USB_EPR_CTR_DTOG_RX);                // toggle receive data only for simulation
#else
            *ptrEndpointControl = (((*ptrEndpointControl &  ~(USB_EPR_CTR_CTR_RX | USB_EPR_CTR_DTOG_RX | USB_EPR_CTR_DTOG_TX | USB_EPR_CTR_STAT_TX_MASK)) ^ USB_EPR_CTR_STAT_RX_VALID )); // free receiver by setting the ready state and resetting the receive flag
#endif
        }
    }
    else {                                                               // transmission acknowledged
        uDisable_Interrupt();                                            // ensure interrupts remain blocked when handling the next possible transmission
            fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpoint_ref, &usb_hardware); // handle tx ack event
        uEnable_Interrupt();                                             // allow higher priority interrupts again
        USB_DADDR = (USB_DADDR_EF | usb_hardware.ucUSBAddress);          // program the address to be used by device and keep function enabled
    #if defined _WINDOWS
        *ptrEndpointControl &= ~(USB_EPR_CTR_STAT_RX_MASK | USB_EPR_CTR_CTR_TX);
        *ptrEndpointControl |= (USB_EPR_CTR_STAT_RX_VALID);
        *ptrEndpointControl ^= (USB_EPR_CTR_DTOG_TX);                    // toggle transmit data for simulation
    #else
        *ptrEndpointControl = ((*ptrEndpointControl &  ~(USB_EPR_CTR_CTR_TX | USB_EPR_CTR_DTOG_RX | USB_EPR_CTR_STAT_RX_MASK | USB_EPR_CTR_DTOG_TX | USB_EPR_CTR_STAT_TX_MASK))); // reset the transmit flag
    #endif
    }
}

#if 0
/**
* @brief  USB_ActivateRemoteWakeup : active remote wakeup signalling
* @param  USBx : Selected device
* @retval HAL status
*/
HAL_StatusTypeDef USB_ActivateRemoteWakeup(USB_TypeDef *USBx)
{
    USBx->CNTR |= USB_CNTR_RESUME;

    return HAL_OK;
}

/**
* @brief  USB_DeActivateRemoteWakeup : de-active remote wakeup signalling
* @param  USBx : Selected device
* @retval HAL status
*/
HAL_StatusTypeDef USB_DeActivateRemoteWakeup(USB_TypeDef *USBx)
{
    USBx->CNTR &= ~(USB_CNTR_RESUME);
    return HAL_OK;
}

/**
* @brief  Copy a buffer from user memory area to packet memory area (PMA)
* @param  USBx : pointer to USB register.
* @param  pbUsrBuf : pointer to user memory area.
* @param  wPMABufAddr : address into PMA.
* @param  wNBytes : number of bytes to be copied.
* @retval None
*/
void USB_WritePMA(USB_TypeDef *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
    uint32_t nbytes = (wNBytes + 1) >> 1;   /* nbytes = (wNBytes + 1) / 2 */
    uint32_t index = 0, temp1 = 0, temp2 = 0;
    uint16_t *pdwVal = NULL;

    pdwVal = (uint16_t *)(wPMABufAddr * 2 + (uint32_t)USBx + 0x400);
    for (index = nbytes; index != 0; index--)
    {
        temp1 = (uint16_t)* pbUsrBuf;
        pbUsrBuf++;
        temp2 = temp1 | (uint16_t)* pbUsrBuf << 8;
        *pdwVal++ = temp2;
        pdwVal++;
        pbUsrBuf++;
    }
}

/**
* @brief  Copy a buffer from user memory area to packet memory area (PMA)
* @param  USBx : pointer to USB register.
* @param  pbUsrBuf : pointer to user memory area.
* @param  wPMABufAddr : address into PMA.
* @param  wNBytes : number of bytes to be copied.
* @retval None
*/
void USB_ReadPMA(USB_TypeDef *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
    uint32_t nbytes = (wNBytes + 1) >> 1;/* /2*/
    uint32_t index = 0;
    uint32_t *pdwVal = NULL;

    pdwVal = (uint32_t *)(wPMABufAddr * 2 + (uint32_t)USBx + 0x400);
    for (index = nbytes; index != 0; index--)
    {
        *(uint16_t*)pbUsrBuf++ = *pdwVal++;
        pbUsrBuf++;
    }
}


/**
* @brief  USB_EPStartXfer : setup and starts a transfer over an EP
* @param  USBx : Selected device
* @param  ep: pointer to endpoint structure
* @retval HAL status
*/
HAL_StatusTypeDef USB_EPStartXfer(USB_TypeDef *USBx, USB_EPTypeDef *ep)
{
    uint16_t pmabuffer = 0;
    uint32_t len = ep->xfer_len;

    /* IN endpoint */
    if (ep->is_in == 1)
    {
        /*Multi packet transfer*/
        if (ep->xfer_len > ep->maxpacket)
        {
            len = ep->maxpacket;
            ep->xfer_len -= len;
        }
        else
        {
            len = ep->xfer_len;
            ep->xfer_len = 0;
        }

        /* configure and validate Tx endpoint */
        if (ep->doublebuffer == 0)
        {
            USB_WritePMA(USBx, ep->xfer_buff, ep->pmaadress, len);
            PCD_SET_EP_TX_CNT(USBx, ep->num, len);
        }
        else
        {
            /* Write the data to the USB endpoint */
            if (PCD_GET_ENDPOINT(USBx, ep->num)& USB_EP_DTOG_TX)
            {
                /* Set the Double buffer counter for pmabuffer1 */
                PCD_SET_EP_DBUF1_CNT(USBx, ep->num, ep->is_in, len);
                pmabuffer = ep->pmaaddr1;
            }
            else
            {
                /* Set the Double buffer counter for pmabuffer0 */
                PCD_SET_EP_DBUF0_CNT(USBx, ep->num, ep->is_in, len);
                pmabuffer = ep->pmaaddr0;
            }
            USB_WritePMA(USBx, ep->xfer_buff, pmabuffer, len);
            PCD_FreeUserBuffer(USBx, ep->num, ep->is_in);
        }

        PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_VALID);
    }
    else /* OUT endpoint */
    {
        /* Multi packet transfer*/
        if (ep->xfer_len > ep->maxpacket)
        {
            len = ep->maxpacket;
            ep->xfer_len -= len;
        }
        else
        {
            len = ep->xfer_len;
            ep->xfer_len = 0;
        }

        /* configure and validate Rx endpoint */
        if (ep->doublebuffer == 0)
        {
            /*Set RX buffer count*/
            PCD_SET_EP_RX_CNT(USBx, ep->num, len);
        }
        else
        {
            /*Set the Double buffer counter*/
            PCD_SET_EP_DBUF_CNT(USBx, ep->num, ep->is_in, len);
        }

        PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_VALID);
    }

    return HAL_OK;
}



* @brief  This function handles PCD Endpoint interrupt request.
* @param  hpcd: PCD handle
* @retval HAL status
* /
static HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd)
{
    PCD_EPTypeDef *ep = NULL;
    uint16_t count = 0;
    uint8_t epindex = 0;
    __IO uint16_t wIstr = 0;
    __IO uint16_t wEPVal = 0;

    /* stay in loop while pending interrupts */
    while (((wIstr = hpcd->Instance->ISTR) & USB_ISTR_CTR) != 0)
    {
        /* extract highest priority endpoint number */
        epindex = (uint8_t)(wIstr & USB_ISTR_EP_ID);

        if (epindex == 0)
        {
            /* Decode and service control endpoint interrupt */

            /* DIR bit = origin of the interrupt */
            if ((wIstr & USB_ISTR_DIR) == 0)
            {
                /* DIR = 0 */

                /* DIR = 0      => IN  int */
                /* DIR = 0 implies that (EP_CTR_TX = 1) always  */
                PCD_CLEAR_TX_EP_CTR(hpcd->Instance, PCD_ENDP0);
                ep = &hpcd->IN_ep[0];

                ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
                ep->xfer_buff += ep->xfer_count;

                /* TX COMPLETE */
                HAL_PCD_DataInStageCallback(hpcd, 0U);


                if ((hpcd->USB_Address > 0U) && (ep->xfer_len == 0U))
                {
                    hpcd->Instance->DADDR = (hpcd->USB_Address | USB_DADDR_EF);
                    hpcd->USB_Address = 0U;
                }

            }
            else
            {
                /* DIR = 1 */

                /* DIR = 1 & CTR_RX       => SETUP or OUT int */
                /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */
                ep = &hpcd->OUT_ep[0U];
                wEPVal = PCD_GET_ENDPOINT(hpcd->Instance, PCD_ENDP0);

                if ((wEPVal & USB_EP_SETUP) != 0U)
                {
                    /* Get SETUP Packet*/
                    ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);
                    USB_ReadPMA(hpcd->Instance, (uint8_t*)hpcd->Setup, ep->pmaadress, ep->xfer_count);
                    /* SETUP bit kept frozen while CTR_RX = 1*/
                    PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);

                    /* Process SETUP Packet*/
                    HAL_PCD_SetupStageCallback(hpcd);
                }

                else if ((wEPVal & USB_EP_CTR_RX) != 0U)
                {
                    PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);
                    /* Get Control Data OUT Packet*/
                    ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

                    if (ep->xfer_count != 0U)
                    {
                        USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaadress, ep->xfer_count);
                        ep->xfer_buff += ep->xfer_count;
                    }

                    /* Process Control Data OUT Packet*/
                    HAL_PCD_DataOutStageCallback(hpcd, 0U);

                    PCD_SET_EP_RX_CNT(hpcd->Instance, PCD_ENDP0, ep->maxpacket);
                    PCD_SET_EP_RX_STATUS(hpcd->Instance, PCD_ENDP0, USB_EP_RX_VALID);
                }
            }
        }
        else
        {
            /* Decode and service non control endpoints interrupt  */

            /* process related endpoint register */
            wEPVal = PCD_GET_ENDPOINT(hpcd->Instance, epindex);
            if ((wEPVal & USB_EP_CTR_RX) != 0U)
            {
                /* clear int flag */
                PCD_CLEAR_RX_EP_CTR(hpcd->Instance, epindex);
                ep = &hpcd->OUT_ep[epindex];

                /* OUT double Buffering*/
                if (ep->doublebuffer == 0U)
                {
                    count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);
                    if (count != 0U)
                    {
                        USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaadress, count);
                    }
                }
                else
                {
                    if (PCD_GET_ENDPOINT(hpcd->Instance, ep->num) & USB_EP_DTOG_RX)
                    {
                        /*read from endpoint BUF0Addr buffer*/
                        count = PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);
                        if (count != 0U)
                        {
                            USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0, count);
                        }
                    }
                    else
                    {
                        /*read from endpoint BUF1Addr buffer*/
                        count = PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);
                        if (count != 0U)
                        {
                            USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1, count);
                        }
                    }
                    PCD_FreeUserBuffer(hpcd->Instance, ep->num, PCD_EP_DBUF_OUT);
                }
                /*multi-packet on the NON control OUT endpoint*/
                ep->xfer_count += count;
                ep->xfer_buff += count;

                if ((ep->xfer_len == 0U) || (count < ep->maxpacket))
                {
                    /* RX COMPLETE */
                    HAL_PCD_DataOutStageCallback(hpcd, ep->num);
                }
                else
                {
                    HAL_PCD_EP_Receive(hpcd, ep->num, ep->xfer_buff, ep->xfer_len);
                }

            } /* if((wEPVal & EP_CTR_RX) */

            if ((wEPVal & USB_EP_CTR_TX) != 0U)
            {
                ep = &hpcd->IN_ep[epindex];

                /* clear int flag */
                PCD_CLEAR_TX_EP_CTR(hpcd->Instance, epindex);

                /* IN double Buffering*/
                if (ep->doublebuffer == 0U)
                {
                    ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
                    if (ep->xfer_count != 0U)
                    {
                        USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaadress, ep->xfer_count);
                    }
                }
                else
                {
                    if (PCD_GET_ENDPOINT(hpcd->Instance, ep->num) & USB_EP_DTOG_TX)
                    {
                        /*read from endpoint BUF0Addr buffer*/
                        ep->xfer_count = PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);
                        if (ep->xfer_count != 0U)
                        {
                            USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0, ep->xfer_count);
                        }
                    }
                    else
                    {
                        /*read from endpoint BUF1Addr buffer*/
                        ep->xfer_count = PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);
                        if (ep->xfer_count != 0U)
                        {
                            USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1, ep->xfer_count);
                        }
                    }
                    PCD_FreeUserBuffer(hpcd->Instance, ep->num, PCD_EP_DBUF_IN);
                }
                /*multi-packet on the NON control IN endpoint*/
                ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
                ep->xfer_buff += ep->xfer_count;

                /* Zero Length Packet? */
                if (ep->xfer_len == 0U)
                {
                    /* TX COMPLETE */
                    HAL_PCD_DataInStageCallback(hpcd, ep->num);
                }
                else
                {
                    HAL_PCD_EP_Transmit(hpcd, ep->num, ep->xfer_buff, ep->xfer_len);
                }
            }
        }
    }
    return HAL_OK;
}
#endif

// When a reset is detected the endpoint registers are all cleared automatically
//
static void fnUSB_reset(int iError)
{
    USB_BD_TABLE *ptrBD = (USB_BD_TABLE *)USB_CAN_SRAM_ADDR;             // start of dedicated SRAM area
    fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware);         // generic handler routine
    usb_hardware.ucUSBAddress = 0;                                       // reset the address to revert back to the default state
    USB_BTABLE = 0;                                                      // no offset used in USB SRAM area
    ptrBD->usUSB_ADDR_TX = ptrBD->usUSB_ADDR_RX = (NUMBER_OF_USB_ENDPOINTS * sizeof(USB_BD_TABLE)/sizeof(unsigned short)); // buffer location of endpoint 0 data
    ptrBD->usUSB_COUNT_TX_0 = 0;
    ptrBD->usUSB_COUNT_RX_0 = fnSetRxLength(ENDPOINT_0_SIZE);
    USB_DADDR = (USB_DADDR_EF | 0);                                      // reset device address and enable function
    USB_EP0R = (USB_EPR_CTR_STAT_RX_VALID | USB_EPR_CTR_STAT_TX_NAK | USB_EPR_CTR_EP_TYPE_CONTROL); // enable control endpoint reception and transmission
}


// USB device FS interrupt handler (low priority interrupts)
//
static __interrupt void USB_Device_LP_Interrupt(void)
{
    register unsigned long ulInterrupts = USB_ISTR;                     // read the pending interrupts
    if ((ulInterrupts & (USB_ISTR_RESET | USB_ISTR_SUSP | USB_ISTR_WKUP | USB_ISTR_ERR)) != 0) { // state change or error interrupt
        if ((ulInterrupts & USB_ISTR_RESET) != 0) {                     // if USB reset detected - D+ and D- in SE0 (single ended logic 0) state for > 2.5us
            fnUSB_reset(0);
            USB_CNTR = (USB_CNTR_RESETM | USB_CNTR_ERRM | USB_CNTR_SUSPM | USB_CNTR_CTRM);
        }
        else if ((ulInterrupts & USB_ISTR_SUSP) != 0) {                  // USB suspend detected
            USB_CNTR = (USB_CNTR_RESETM | USB_CNTR_ERRM | USB_CNTR_WKUPM);
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting messages to queue
                fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);       // generic suspend handler routine
            uEnable_Interrupt();
        }
        else if ((ulInterrupts & USB_ISTR_WKUP) != 0) {                  // USB wakeup detected
            USB_CNTR = (USB_CNTR_RESETM | USB_CNTR_ERRM | USB_CNTR_SUSPM);
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting messages to queue
                fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);        // generic resume handler routine
            uEnable_Interrupt();
        }
        else if ((ulInterrupts & USB_ISTR_ERR) != 0) {
            // Error
            //
            USB_errors++;
        }
        USB_ISTR = 0;                                                    // clear interrupt(s)
    }
    else if ((USB_ISTR_CTR & ulInterrupts) != 0) {                       // correct transaction
        USB_ISTR &= ~(USB_ISTR_CTR);                                     // clear interrupt
        USB_Device_HP_Interrupt();
    }
}

// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
    // The USB clock of 48MHz is supplied by the PLL output and it can either be used directly or after being divided by 1.5
    // - this means that the PLL output must be either 48MHz or 72MHz in order for USB to be able to operate
    //
    #if PLL_OUTPUT_FREQ == 72000000                                      // if the PLL is set to 72MHz we divide it by 1.5 to get the USB clock
    RCC_CFGR &= ~RCC_CFGR_OTGFSPRE;
    #elif PLL_OUTPUT_FREQ == 48000000                                    // use the PLL directly as USB clock
    RCC_CFGR |= RCC_CFGR_OTGFSPRE;
    #else                                                                // invalid PLL frequency for USB use
    #error "PLL must be 48MHz or 72MHz for USB FS use!!"
    #endif
    POWER_UP(APB1, RCC_APB1ENR_USBEN);                                   // power up USB controller

    RCC_APB1RSTR |= RCC_APB1RSTR_USBRST;                                 // reset the module
    RCC_APB1RSTR &= ~(RCC_APB1RSTR_USBRST);
    USB_CNTR = USB_CNTR_FRES;                                            // leave powered down mode (connecting transceiver) but leave in reset until it is stable
    fnDelayLoop(USB_DEVICE_T_STARTUP);                                   // wait stabilisation delay
    USB_CNTR = 0;                                                        // exit forced reset state
    USB_ISTR = 0;                                                        // clear possible spurious interrupts that are pending

    fnEnterInterrupt(irq_USB_LP_CAN_RX0_ID, PRIORITY_DEVICE_LP_FS, USB_Device_LP_Interrupt); // enter USB low priority interrupt handler
    fnEnterInterrupt(irq_USB_HP_CAN_TX_ID, PRIORITY_DEVICE_HP_FS, USB_Device_HP_Interrupt); // enter USB high priority interrupt handler
    {
        int i = 0;
        volatile unsigned long *ptrSRAM = USB_CAN_SRAM_ADDR;
        while (i++ < USB_CAN_SRAM_SIZE) {
            *ptrSRAM++ = 0;
        }
    }
    USB_CNTR = (USB_CNTR_RESETM | USB_CNTR_ERRM);                        // enable USB reset and error interrupts
    #if  defined _STM32F103X
    _CONFIG_PERIPHERAL_INPUT(A, (0), (PORTA_BIT11 | PORTA_BIT12), (ALTERNATIVE_FUNCTION));
    #endif
}
#endif