/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_ENET.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                          // {7}
    static const unsigned char EMAC_RX_int_message[HEADER_LENGTH] = {0, 0, TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RX_INTERRUPT}; // define fixed interrupt event
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static STM32_BD *ptrRxBd = 0;
static STM32_BD *ptrTxBd = 0;

#if defined USE_IGMP                                                     // {23}
    #if defined USE_IPV6
        #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 2)
    #else
        #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 1)
    #endif
    static unsigned char ucGroupHashes[MAX_MULTICAST_FILTERS] = {0};
#endif


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */
extern ETHERNETQue *eth_tx_control;                                      // Ethernet structure pointers for Ethernet Interrupt handling
extern ETHERNETQue *eth_rx_control;

/* =================================================================== */
/*                          Ethernet Interface                         */
/* =================================================================== */


// This is called by the Ethernet task to handle an Ethernet event, which is usually a reception frame
//
extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame)
{
#if defined LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_rx_int_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_RX_INTERRUPT };
#endif
    if (EMAC_RX_INTERRUPT == *ucEvent) {
        if (ptrRxBd->bd_word0 & RX_BD_WORD0_OWN) {                       // check the ownership
            return -1;                                                   // belongs to DMA so not ready
        }
#if IP_INTERFACE_COUNT > 1
        rx_frame->ucInterface = ETHERNET_INTERFACE;                      // {20}
#endif
#if defined LAN_REPORT_ACTIVITY
        fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_rx_int_message, HEADER_LENGTH);
#endif
#if defined IP_RX_CHECKSUM_OFFLOAD                                       // the device is performing IPv4/v6 reception checksum verifications
        if (ptrRxBd->bd_word0 & RX_BD_WORD0_FT) {                        // the reception frame is IPv4/v6
            if (ptrRxBd->bd_word0 & (RX_BD_WORD0_PCE | RX_BD_WORD0_IPHCE)) { // the device has detected either a payload or an IPv4/v6 header checksum error
                rx_frame->frame_size = 0;                                // set invalid length so that the frame is unconditionally discarded
                return 0;
            }
        }
#endif
        rx_frame->frame_size = (unsigned short)(((ptrRxBd->bd_word0 & RX_BD_WORD0_FL_MASK) >> RX_BD_WORD0_FL_SHIFT) - 4); // frame length minus CRC
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ptrRxBd->bd_dma_buf1_address; // pointer to frame in memory
        return 0;                                                        // channel received from 0 (fixed value for compatibility with other devices)
    }
#if !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME && defined USE_IP_STATS  // {7}
    else if (ETHERNET_RX_OVERRUN == *ucEvent) {
        fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES);                  // we lost a frame due to RX overrun so count the event
    }
    else {
        fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);                    // count other unexpected events
    }
#endif
    return -1;                                                           // invalid channel
}

#if defined _DP83848
    #define READ_INTERRUPT_FLAGS   DP83848_MII_INTERRUPT_STATUS_REGISTER
    #define LINK_CHANGE_INTERRUPTS (DP83848_LINK_INT | DP83848_ANC_INT)
    #define READ_STATE             DP83848_PHY_STATUS_REGISTER
    #define SPEED_100M             SPEED_STATUS_10M
    #define FULL_DUPLEX_FLAG       DUPLEX_STATUS_FULL_DUPLEX
#elif defined _LAN8720 || defined _LAN8740 || defined _LAN8742           // {12}
    #define READ_INTERRUPT_FLAGS   (0x1d << ETH_MACMIIAR_MR_SHIFT)
    #define LINK_CHANGE_INTERRUPTS (0x0050)                              // auto-negotiation complete or link-down
    #define READ_STATE             (0x1f << ETH_MACMIIAR_MR_SHIFT)
    #define SPEED_100M             0x0004                                // bit indicating 10M speed (used inverted for compatibility)
    #define FULL_DUPLEX_FLAG       0x0010                                // flag indicating full duplex link
#elif defined _KS8721                                                    // {32}
    #define READ_INTERRUPT_FLAGS   KS8721_INTERRUPT_CONTROL_STATUS_REGISTER
    #define LINK_CHANGE_INTERRUPTS (PHY_LINK_UP_INT | PHY_LINK_DOWN_INT)
    #define READ_STATE             KS8721_100BASETX_PHY_CONTROL_REGISTER
    #define SPEED_100M             PHY_OP_MODE_100_BASE_T_HALF_DUPLEX
    #define FULL_DUPLEX_FLAG       PHY_OP_MODE_FULL_DUPLEX
#elif defined _ST802RT1B
    #define READ_INTERRUPT_FLAGS   ST802RT1_PHY_STATUS_REGISTER
    #define LINK_CHANGE_INTERRUPTS (ST802RT1_LINK_DOWN_INTERRUPT | ST802RT1_AUTO_NEG_COMPLETE)
    #define READ_STATE             ST802RT1_PHY_STATUS_REGISTER
    #define SPEED_100M             ST802RT1_SPEED_100M
    #define FULL_DUPLEX_FLAG       ST802RT1_FULL_DUPLEX
#endif

// PHY Read
//
static unsigned short fnReadMII_PHY(unsigned long ulRegAdr)
{
    ETH_MACMIIAR = (READ_FROM_PHY | ulRegAdr);                           // command read from specified register address

    #if defined _WINDOWS
    ETH_MACMIIAR &= ~ETH_MACMIIAR_MB;
    if (GENERIC_BASIC_CONTROL_REGISTER == ulRegAdr) {
        ETH_MACMIIDR = 0;
    }
    else if (GENERIC_PHY_IDENTIFIER_1 == ulRegAdr) {                     // simulate reads from identifier register
        ETH_MACMIIDR = (PHY_IDENTIFIER >> 16);
    }
    else if (GENERIC_PHY_IDENTIFIER_2 == ulRegAdr) {
        ETH_MACMIIDR = (PHY_IDENTIFIER & 0x0000ffff);
    }
    else if (GENERIC_BASIC_STATUS_REGISTER == ulRegAdr) {
        ETH_MACMIIDR = (PHY_LINK_IS_UP | PHY_AUTO_NEGOTIATION_COMPLETE);
    }
    else if (READ_INTERRUPT_FLAGS == ulRegAdr) {
        #if defined PHY_INTERRUPT
        unsigned short usBit = PHY_INTERRUPT;
        unsigned char ucPortBit = 0;
        while ((usBit & 0x8000) == 0) {
            ucPortBit++;
            usBit <<= 1;
        }
        fnSimulateInputChange(PHY_INT_PORT, ucPortBit, SET_INPUT);       // remove IRQ when this register is read
        #endif
        ETH_MACMIIDR = LINK_CHANGE_INTERRUPTS;                           // link up and auto-negotiation complete
    }
    else if (READ_STATE == ulRegAdr) {
        ETH_MACMIIDR = FULL_DUPLEX_FLAG;                                 // 100M full-duplex
    }
    #endif

    while (ETH_MACMIIAR & ETH_MACMIIAR_MB) {}                            // wait until the command has terminated

    return (unsigned short)ETH_MACMIIDR;                                 // return the value read
}


// PHY write
//
static void fnWriteMII_PHY(unsigned long ulRegAdr, unsigned short usData)
{
    ETH_MACMIIDR = usData;                                               // prepare the value to be written to the register
    ETH_MACMIIAR = (WRITE_TO_PHY | ulRegAdr);                            // command write to specified register address

#if defined _WINDOWS
    if (GENERIC_BASIC_CONTROL_REGISTER == ulRegAdr) {
        usPhyMode = (usData & ~0x0200);                                  // save mode, removing the self-clearing auto-negotiation start bit
    }
    ETH_MACMIIAR &= ~ETH_MACMIIAR_MB;
#endif

    while ((ETH_MACMIIAR & ETH_MACMIIAR_MB) != 0) {}                     // wait until the command has terminated
}

extern unsigned short _fnMIIread(unsigned char _mradr)
{
    return (fnReadMII_PHY((_mradr << ETH_MACMIIAR_MR_SHIFT)));
}

extern void _fnMIIwrite(unsigned char _mradr, unsigned short _mwdata)
{
    fnWriteMII_PHY((_mradr << ETH_MACMIIAR_MR_SHIFT), _mwdata);
}


#if defined PHY_POLL_LINK || defined PHY_INTERRUPT                       // {11}
extern void fnCheckEthLinkState(void)
{
    unsigned short usInterrupt = fnReadMII_PHY(READ_INTERRUPT_FLAGS);    // read the cause(s) of the interrupt, which resets the flags in the register read
    #if defined INTERRUPT_TASK_PHY
    unsigned char int_phy_message[HEADER_LENGTH]; // = {INTERNAL_ROUTE, INTERNAL_ROUTE, INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT}; // define standard interrupt event
    int_phy_message[MSG_DESTINATION_NODE] = int_phy_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    int_phy_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_PHY;
    int_phy_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
    int_phy_message[MSG_INTERRUPT_EVENT] = UNKNOWN_INTERRUPT;
    #endif
    if (usInterrupt & LINK_CHANGE_INTERRUPTS) {                          // change of link state detected or auto-negotiation complete
        usInterrupt = fnReadMII_PHY(GENERIC_BASIC_STATUS_REGISTER);      // read whether the link is up or not
    }
    else {
        return;                                                          // ignore if no change
    }
    if (usInterrupt & (PHY_AUTO_NEGOTIATION_COMPLETE | PHY_LINK_IS_UP)) {// since auto-negotiation has just completed ensure that MAC settings are synchronised
        usInterrupt = fnReadMII_PHY(READ_STATE);                         // check the  details of link
    #if !defined _KS8721 && !defined _ST802RT1B
        usInterrupt ^= SPEED_100M;                                       // invert the flag for compatibility
    #endif
        if (usInterrupt & SPEED_100M) {                                  // if at least 100 half duplex it indicates a 100MHz link
           ETH_MACCR |= ETH_MACCR_FES;                                   // ensure EMAC is set to 100MHz mode
    #if defined INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
    #endif
        }
        else {
            ETH_MACCR &= ~ETH_MACCR_FES;                                 // ensure EMAC is set to 10MHz mode
    #if defined INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
    #endif
        }
        if (usInterrupt & FULL_DUPLEX_FLAG) {
            ETH_MACCR &= ~(ETH_MACCR_IFG_64);
            ETH_MACCR |= (ETH_MACCR_IFG_96 | ETH_MACCR_DM);
    #if defined INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT]++;                      // change the event to full-duplex value
    #endif
        }
        else {                                                           // half-duplex
            ETH_MACCR &= ~(ETH_MACCR_IFG_96 | ETH_MACCR_DM);
            ETH_MACCR |= (ETH_MACCR_IFG_64);
        }
    }
    #if defined INTERRUPT_TASK_PHY
    else {                                                               // link down    
        int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
    }
    fnWrite(INTERNAL_ROUTE, (unsigned char *)int_phy_message, HEADER_LENGTH); // inform the corresponding task of new link state
    #endif
}
#endif

// PHY interrupt
//
#if defined PHY_INTERRUPT
static void fnPhyInterrupt(void)                                         // PHY interrupt input callback
{
    while (PHY_INT_PIN_STATE() == 0) {                                   // while the interrupt line remains low
        fnCheckEthLinkState();                                           // {11}
    }
}
#endif

#if defined _MAGIC_RESET_FRAME                                           // {21}
static STM32_BD *ptrMagicRxBd = 0;                                       // initially disabled

// User interface to enable and disable magic frame detection
//
extern void fnEnableMagicFrames(int iOnOff)
{
    uDisable_Interrupt();                                                // protect from ethernet reception interrupts
    if (iOnOff == 0) {
        ptrMagicRxBd = 0;                                                // disable
    }
    else {
        ptrMagicRxBd = ptrRxBd;                                          // enabled by synchronising the magic frame buffer with the receiver
    }
    uEnable_Interrupt();                                                 // allow further interrupt processing
}
#endif

// EMAC interrupt handler
//
__interrupt void EMAC_Interrupt(void)
{
    while ((ETH_DMAIER & ETH_DMASR) & (ETH_DMAIER_TIE | ETH_DMAIER_TPSIE | ETH_DMAIER_TBUIE | ETH_DMAIER_TJTIE | 
                                       ETH_DMAIER_ROTIE | ETH_DMAIER_TUIE | ETH_DMAIER_RIE | ETH_DMAIER_RBUIE | 
                                       ETH_DMAIER_RPSIE | ETH_DMAIER_RWTIE | ETH_DMAIER_ETIE | ETH_DMAIER_FBEIE | 
                                       ETH_DMAIER_ERIE | ETH_DMAIER_AISE | ETH_DMAIER_NISE)) { // while enabled EMAC interrupt pending
        if (ETH_DMASR & ETH_DMAIER_RIE) {                                // reception interrupt
    #if defined _MAGIC_RESET_FRAME  && !defined USE_MULTIPLE_BUFFERS     // {21}
            if (ptrMagicRxBd != 0) {                                     // if enabled
                if ((ptrMagicRxBd->bd_word0 & RX_BD_WORD0_OWN) == 0) {   // ignore if buffer is not ready
                    if ((ptrMagicRxBd->bd_word0 & (RX_BD_WORD0_CE | RX_BD_WORD0_RE)) == 0) { // only handle valid frames
                        unsigned short usLength = (unsigned short)(((ptrMagicRxBd->bd_word0 & RX_BD_WORD0_FL_MASK) >> RX_BD_WORD0_FL_SHIFT) - 4); // frame length minus CRC
                        if (usLength == 93) {                            // payload length must be 93 bytes in length
                            unsigned char *ptrData = &((ETHERNET_FRAME_CONTENT *)ptrMagicRxBd->bd_dma_buf1_address)->ethernet_frame_type[0];
                            int i;
                            for (i = 0; i < 24; i++) {                   // remaining header must be (unsigned short)_MAGIC_RESET_FRAME repeated 24 times
                                if (*ptrData++ != (unsigned char)(_MAGIC_RESET_FRAME >> 8)) {
                                    goto _not_magic_frame;
                                }
                                if (*ptrData++ != (unsigned char)(_MAGIC_RESET_FRAME)) {
                                    goto _not_magic_frame;
                                }
                            }
                            // A magic frame has been received - the 61st byte is used to signal the type and 32 bytes of optional data are passed
                            //
                            fnMagicFrame(*ptrData, (ptrData + 1));       // allow the user to handle the magic frame
                        }
                    }
_not_magic_frame:
                    ptrMagicRxBd = (STM32_BD *)ptrMagicRxBd->bd_dma_buf2_address; // go to next buffer descriptor
                }
            }
    #endif
            ETH_DMASR = (ETH_DMAIER_RIE | ETH_DMAIER_NISE);              // clear interrupt
    #if defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                       // {7}
            uTaskerStateChange(TASK_ETHERNET, UTASKER_ACTIVATE);         // schedule the Ethernet task
    #else
            uDisable_Interrupt();                                        // ensure message can not be interrupted
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_RX_int_message, HEADER_LENGTH); // inform the Ethernet task
            uEnable_Interrupt();                                         // release
    #endif
        }
    #if defined LAN_REPORT_ACTIVITY
        if (ETH_DMASR & ETH_DMAIER_TIE) {                                // transmission interrupt
            static const unsigned char EMAC_int_message[HEADER_LENGTH] = {0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT};
            ETH_DMASR = (ETH_DMAIER_NISE | ETH_DMAIER_TIE);              // clear interrupt
            uDisable_Interrupt();                                        // ensure message can not be interrupted
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
            uEnable_Interrupt();                                         // release
        }
    #endif
    #if defined _WINDOWS
        ETH_DMASR = 0;
    #endif
    }
}

#if defined USE_IGMP || defined USE_IPV6
static unsigned char fnCalculateHash(unsigned char *ptrMac)
{
    #define CRC_POLY 0xedb88320                                          // CRC-32 polynomial in reverse direction
    unsigned char ucHash = 0;
    unsigned long ulCRC32 = 0xffffffff;
    int i, iBit;
    unsigned char ucByte;
	for (i = 0; i < MAC_LENGTH; i++) {
        ucByte = *ptrMac++;
        for (iBit = 0; iBit < 8; iBit++) {
            if ((ulCRC32 ^ ucByte) & 1) {
                ulCRC32 = (ulCRC32 >> 1) ^ CRC_POLY;
            }
            else {
                ulCRC32 >>= 1;
            }
            ucByte >>= 1;
        }
    }
    for (i = 0; i < 6; i++) {                                            // the hash is the negated first 6 bits of the CRC32 in bit swapped order
        if ((ulCRC32 & (1 << i)) == 0) {
            ucHash |= (1 << (5 - i));
        }
    }
    return ucHash;
}

static void fnEnableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        ETH_MACHTLR |= (0x00000001 << ucHashRef);                        // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        ETH_MACHTHR |= (0x00000001 << ucHashRef);
    }
    ETH_MACFFR |= (ETH_MACFFR_HM);                                       // {4} enable hash filter
}
#endif

#if defined USE_IPV6
// Configure a multicast address for IPV6 use
//
extern void fnConfigureMulticastIPV6(void)
{
    unsigned char ucMac[MAC_LENGTH];                                     // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    ucMac[0] = 0x33;
    ucMac[1] = 0x33;
    ucMac[2] = 0xff;
    ucMac[3] = ucLinkLocalIPv6Address[3 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[4] = ucLinkLocalIPv6Address[4 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[5] = ucLinkLocalIPv6Address[5 + (IPV6_LENGTH - MAC_LENGTH)];
    ucHashRef = fnCalculateHash(ucMac);
    fnEnableMulticastHash(ucHashRef);
    #if defined USE_IGMP
    ucGroupHashes[0] = ucHashRef;                                        // mark that this hash is required by IPv6
    #endif
}
#endif

#if defined USE_IGMP                                                     // {23}
static void fnDisableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        ETH_MACHTLR &= ~(0x00000001 << ucHashRef);                       // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        ETH_MACHTHR &= ~(0x00000001 << ucHashRef);
    }
}


// IGMP adds and removed multicast host groups and requires the receiver's multicast filtering to be modified in the process
//
extern void fnModifyMulticastFilter(QUEUE_TRANSFER action, unsigned char *ptrIP)
{
    int iFree = -1;
    int iMatch = -1;
    int i;
    int iHashCount = 0;
    unsigned char ucMac[MAC_LENGTH];
    unsigned char ucHashRef;
    switch (action) {
    case REMOVE_IPV4_FILTER:
    case ADD_IPV4_FILTER:                                                // a filter has to be enabled for this IPv4 address
        ucMac[0] = 0x01;                                                 // IANA owned multicast ethernet address block
        ucMac[1] = 0x00;
        ucMac[2] = 0x5e;                                                 // enter the multicast MAC address belonging to IANA for this purpose
        ucMac[3] = (*(ptrIP + 1) & 0x7f);                                // add 23 bits of the destination IP address
        ucMac[4] = *(ptrIP + 2);
        ucMac[5] = *(ptrIP + 3);
        ucHashRef = fnCalculateHash(ucMac);
        break;
    default:                                                             // ignore any other calls
        return;
    }
    for (i = 0; i < MAX_MULTICAST_FILTERS; i++) {                        // check whether the entry exists
        if (ucGroupHashes[i] == ucHashRef) {                             // existing hash entry found
            iMatch = i;
            iHashCount++;                                                // count the number of groups requiring this hash
        }
        else if (ucGroupHashes[i] == 0) {                                // free location
            iFree = i;                                                   // remember the free entry
        }
    }
    if (action == ADD_IPV4_FILTER) {                                     // the entry doesn't exist so we add it
        if (iFree < 0) {
            _EXCEPTION("No more hash entry space");
        }
        else {
            ucGroupHashes[iFree] = ucHashRef;                            // note that this hash is required
            fnEnableMulticastHash(ucHashRef);                            // enable the hash entry
        }
    }
    else if (action == REMOVE_IPV4_FILTER) {
        if (iMatch >= 0) {                                               // remove an entry for this hash
            ucGroupHashes[iMatch] = 0;                                   // entry free
            if (iHashCount == 1) {                                       // single entry so it can be disabled
                fnDisableMulticastHash(ucHashRef);
            }
        }
    }
}
#endif


// Ethernet configuration routine
//
extern int fnConfigEthernet(ETHTABLE *pars)
{
#if defined PHY_INTERRUPT                                                // if PHY interrupt line is connected use it to detect link state changes and control auto-negotiated speed
    INTERRUPT_SETUP interrupt_setup;
#endif
    volatile int i;
    unsigned long ulPhyIdentifier, ulRegister;
    unsigned short usMIIData;
    STM32_BD *ptrBd, *ptrFirstTxBd;
    unsigned char *ptrBuffer;
#if defined ETHERNET_RMII && (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX) // {6}
    POWER_UP(APB2, RCC_APB2ENR_SYSCFGEN);                                // power up the system configuration controller
    SYSCFG_PMC = SYSCFG_PMC_MII_RMII_SEL;                                // enable RMII mode (performed while the MAC is in reset and before MAC clocks have been enabled)
#endif
#if defined ETHERNET_RMII && (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX) // {6}
    POWER_UP(AHB1, (RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN)); // {14} enable clocks to Ethernet controller modules
    // RMII mode
    //
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_ETH), (ETH_MDIO_A_2), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_ETH), (ETH_MDC_C_1), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    #if defined ETH_TXD_TX_EN_G                                          // locate TXD0 and TXD1 and TX_EN on port G rather than port B
    _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_ETH), (ETH_TXEN_G_11 | ETH_TXD0_G_13 | ETH_TXD1_G_14), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    #elif defined ETH_TXD1_B                                             // locate TXD0 and TX_EN on port G rather, with TXD1 on port B
    _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_ETH), (ETH_TXEN_G_11 | ETH_TXD0_G_13), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_ETH), (ETH_TXD1_B_13), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    #elif defined ETH_TXD_G                                              // locate TXD0 and TXD1 on port G rather than port B
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_ETH), (ETH_TXEN_B_11), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_ETH), (ETH_TXD0_G_13 | ETH_TXD1_G_14), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    #elif defined ETH_TXD1_B
    #else
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_ETH), (ETH_TXD0_B_12 | ETH_TXD1_B_13 | ETH_TXEN_B_11), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    #endif
    _CONFIG_PERIPHERAL_INPUT(A,  (PERIPHERAL_ETH), (ETH_RXCLK_A_1 | ETH_RX_DV_A_7), FLOATING_INPUT);    
    _CONFIG_PERIPHERAL_INPUT(C,  (PERIPHERAL_ETH), (ETH_RXD0_C_4 | ETH_RXD1_C_5), FLOATING_INPUT);
#else
    #if defined ETHERNET_RMII                                            // {14} STM32F1xx RMII
        #if defined ETHERNET_DRIVE_PHY_25MHZ                             // drive 50MHz PHY clock
            #if !defined USE_PLL2_CLOCK
                #error "USE_PLL2_CLOCK is expected in order to use RMII Ethernet mode"
            #endif
            #define PLL3_MUL_FACTOR (50000000/(CRYSTAL_FREQ/PLL2_INPUT_DIV)) // PLL3 multiplication factor that is required
            #if PLL3_MUL_FACTOR >= 8 && PLL3_MUL_FACTOR <= 14
                #define PLL3_MUL_VALUE ((PLL3_MUL_FACTOR - 2) << 12)
            #elif PLL3_MUL_FACTOR == 16
                #define PLL3_MUL_VALUE (0xe << 12)
            #elif PLL3_MUL_FACTOR == 20
                #define PLL3_MUL_VALUE (0xf << 12)
            #else
                #error "Error - PLL3 multiplication not possible for RMII Ethernet"
            #endif
    RCC_CFGR2 = ((RCC_CFGR2 & ~RCC_CFGR2_PLL3MUL_MASK) | PLL3_MUL_VALUE);// set PLL3 to generate 50MHz for RMII use
    RCC_CFGR = ((RCC_CFGR& ~RCC_CFGR_MCO1_MASK) | RCC_CFGR_MCO1_PLL3);   // select MCI output from PLL3 for ethernet
    RCC_CR |= RCC_CR_PLL3ON;                                             // enable PLL3
    while ((RCC_CR & RCC_CR_PLL3RDY) == 0) {                             // wait until the PLL3 is locked
            #if defined _WINDOWS
        RCC_CR |= RCC_CR_PLL3RDY;
            #endif
    }
        #endif
    _PERIPHERAL_REMAP(MII_RMII_SEL);                                     // configure for connectivity with an RMII PHY (configured before enabling clocks)
    _PERIPHERAL_REMAP(ETH_REMAP);                                        // remap certain MII pins (PD8:PD9:PD10)
    POWER_UP(AHB1, (RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN)); // enable clocks to Ethernet controller modules
        #if defined ETHERNET_DRIVE_PHY_25MHZ                             // configure the MCO output to drive PHY clock
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_SYS), (MCO_A_8), (OUTPUT_FAST | OUTPUT_PUSH_PULL)); // drive 50MHz clock to the PHY
        #endif
    // Configure RMII connections
    //
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_ETH), (ETH_TXEN_B_11 | ETH_TXD0_B_12 | ETH_TXD1_B_13), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_ETH), (ETH_MDIO_A_2), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_ETH), (ETH_MDC_C_1), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_INPUT(A,  (PERIPHERAL_ETH), (ETH_RXCLK_A_1), FLOATING_INPUT);
    _CONFIG_PERIPHERAL_INPUT(D,  (PERIPHERAL_ETH), (ETH_RX_DV_D_8 | ETH_RXD0_D_10 | ETH_RXD1_D_9), FLOATING_INPUT);
    #else
    POWER_UP(AHB1, (RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN)); // {14} enable clocks to Ethernet controller modules
    // MII mode
    //
        #if !defined _STM32F2XX && !defined _STM32F4XX && !defined _STM32F7XX
    _PERIPHERAL_REMOVE_REMAP(MII_RMII_SEL);                              // select MII interface mode rather than RMII
    _PERIPHERAL_REMAP(ETH_REMAP);                                        // remap certain MII pins (PD8:PD9:PD10:PD11:PD12)
        #endif
        #if defined ETHERNET_DRIVE_PHY_25MHZ                             // configure the MCO output to drive 25MHz PHY clock
    RCC_CFGR |= (RCC_CFGR_MCO1_XT1);                                     // select MCO to be equal to the 25MHz external crystal (note that it is recommended to set this after reset, before configuring the clocks to avoid glitches - since the clock is not yet driven to the PHY this is not respected)
            #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_SYS), (MCO1_A_8), (OUTPUT_FAST | OUTPUT_PUSH_PULL)); // drive 25MHz clock to the PHY
            #else
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_SYS), (MCO_A_8), (OUTPUT_FAST | OUTPUT_PUSH_PULL)); // drive 25MHz clock to the PHY
            #endif
        #endif
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_ETH), (ETH_MDIO_A_2), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
        #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_ETH), (ETH_TXEN_G_11 | ETH_TXD0_G_13 | ETH_TXD1_G_14), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_ETH), (ETH_TXD3_B_8), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_ETH), (ETH_MDC_C_1 | ETH_TXD2_C_2), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_INPUT(H,  (PERIPHERAL_ETH), (ETH_CRS_H_2 | ETH_COL_H_3 | ETH_RXD2_H_6 | ETH_RXD3_H_7), FLOATING_INPUT);
    _CONFIG_PERIPHERAL_INPUT(A,  (PERIPHERAL_ETH), (ETH_RXCLK_A_1 | ETH_RX_DV_A_7), FLOATING_INPUT);    
    _CONFIG_PERIPHERAL_INPUT(I,  (PERIPHERAL_ETH), (ETH_RX_ER_I_10), FLOATING_INPUT);
    _CONFIG_PERIPHERAL_INPUT(C,  (PERIPHERAL_ETH), (ETH_TXCLK_C_3 | ETH_RXD0_C_4 | ETH_RXD1_C_5), FLOATING_INPUT);
        #else
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_ETH), (ETH_TXD3_B_8 | ETH_TXEN_B_11 | ETH_TXD0_B_12 | ETH_TXD1_B_13), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_ETH), (ETH_MDC_C_1 | ETH_TXD2_C_2), (OUTPUT_FAST | OUTPUT_PUSH_PULL));
    _CONFIG_PERIPHERAL_INPUT(A,  (PERIPHERAL_ETH), (ETH_CRS_A_0 | ETH_RXCLK_A_1 | ETH_COL_A_3), FLOATING_INPUT);
    _CONFIG_PERIPHERAL_INPUT(B,  (PERIPHERAL_ETH), (ETH_RX_ER_B_10), FLOATING_INPUT);
    _CONFIG_PERIPHERAL_INPUT(C,  (PERIPHERAL_ETH), (ETH_TXCLK_C_3), FLOATING_INPUT);
    _CONFIG_PERIPHERAL_INPUT(D,  (PERIPHERAL_ETH), (ETH_RX_DV_D_8 | ETH_RXD0_D_10 | ETH_RXD1_D_9 | ETH_RXD2_D_11 | ETH_RXD3_D_12), FLOATING_INPUT);
        #endif
    #endif
#endif
    ETH_DMABMR = (ETH_DMABMR_PBL_1_BEAT | ETH_DMABMR_RDP_1_BEAT | ETH_DMABMR_SR);
    while ((ETH_DMABMR & ETH_DMABMR_SR) != 0) {                          // command a reset of the ethernet controller and wait for the reset to complete
#if defined _WINDOWS
        ETH_DMABMR &= ~ETH_DMABMR_SR;
#endif
    }
#if defined RESET_PHY                                                    // {10}
    NEGATE_PHY_RST();                                                    // take phy out of reset state (this is performed after the Ethernet controller reset otherwise it can disturb the Ethernet controller reset)
    fnDelayLoop(20);                                                     // short wait until the device becomes ready
#else
    fnWriteMII_PHY(GENERIC_BASIC_CONTROL_REGISTER, PHY_SOFTWARE_RESET);  // command a software reset of PHY
    i = 1000000;                                                         // limit the time we would wait in case the PHY wasn't responding
    while ((fnReadMII_PHY(GENERIC_BASIC_CONTROL_REGISTER) & PHY_SOFTWARE_RESET) != 0) { // wait until reset completed
        if (--i == 0) {                                                  // quit if no response
            return -1;                                                   // the phy isn't responding correctly - return error
        }
    }
#endif
    ulPhyIdentifier = fnReadMII_PHY(GENERIC_PHY_IDENTIFIER_1);           // check that the PHY is working correctly by reading its identifier
    ulPhyIdentifier <<= 16;
    ulPhyIdentifier |= fnReadMII_PHY(GENERIC_PHY_IDENTIFIER_2);          // check that the PHY is working correctly by reading its identifier
    if ((ulPhyIdentifier & PHY_MASK) != (PHY_IDENTIFIER & PHY_MASK)) {
        return -1;                                                       // if the identifier is incorrect the phy isn't responding correctly - return error
    }

    usMIIData = 0;                                                       // configure the mode which we would like to use
#if defined IP_RX_CHECKSUM_OFFLOAD
    ETH_MACCR = ETH_MACCR_IPCO;                                          // enable IPv4 rx checksum offloading on UDP, TCP and ICMP frame reception
#else
    ETH_MACCR = 0;
#endif
    if ((pars->usMode & (LAN_10M | LAN_100M)) != 0) {                    // fixed speed setting
        if ((pars->usMode & LAN_100M) != 0)  {
            usMIIData |= PHY_SELECT_100M_SPEED;
            ETH_MACCR |= ETH_MACCR_FES;
        }
        if ((pars->usMode & FULL_DUPLEX) != 0) {
            usMIIData |= PHY_FULL_DUPLEX_MODE;
            ETH_MACCR |= (ETH_MACCR_IFG_96 | ETH_MACCR_DM);
        }
        else {
            ETH_MACCR |= (ETH_MACCR_IFG_64);
        }
    }
    else {
        usMIIData |= (PHY_ENABLE_AUTO_NEGOTIATION | PHY_RESTART_AUTO_NEGOTIATION);
    }

    if ((pars->usMode & LOOPBACK) != 0) {
        usMIIData |= PHY_LOOP_BACK;                                      // test loop back at PHY
      //ETH_MACCR |= (ETH_MACCR_LM);                                     // test loop back at MII
    }

    fnWriteMII_PHY(GENERIC_BASIC_CONTROL_REGISTER, usMIIData);           // write the PHY operating mode

#if defined USE_IPV6
    ETH_MACA0HR = (0x80000000 | ((network[DEFAULT_NETWORK].ucOurMAC[5] << 8)  | (network[DEFAULT_NETWORK].ucOurMAC[4]))); // set the MAC address
    ETH_MACA0LR = ((network[DEFAULT_NETWORK].ucOurMAC[3] << 24) | (network[DEFAULT_NETWORK].ucOurMAC[2] << 16) | (network[DEFAULT_NETWORK].ucOurMAC[1] << 8) | network[DEFAULT_NETWORK].ucOurMAC[0]);
#else
    ETH_MACA0HR = (0x80000000 | ((pars->ucMAC[5] << 8)  | (pars->ucMAC[4]))); // set the MAC address
    ETH_MACA0LR = ((pars->ucMAC[3] << 24) | (pars->ucMAC[2] << 16) | (pars->ucMAC[1] << 8) | pars->ucMAC[0]);
#endif

    if ((pars->usMode & PROMISCUOUS) != 0) {
        ETH_MACFFR = ETH_MACFFR_PM;
    }
#if defined USE_IPV6
    else if (pars->usMode & CON_MULTICAST) {                             // if multicast reception is to be configured
        fnConfigureMulticastIPV6();                                      // set IPV6 multicast address
    }
#endif
    else {
        ETH_MACFFR = ETH_MACFFR_HPF;                                     // unicast
    }
    ETH_MACFCR = ETH_MACFCR_PLT_4;
    ETH_MACVLANTR = 0;
    ETH_DMABMR = (ETH_DMABMR_AAB | ETH_DMABMR_RDP_1_BEAT | ETH_DMABMR_PBL_1_BEAT | ETH_DMABMR_USP);

    ulRegister = pars->usSizeRx;
    i = NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE;
    ptrBuffer = uMallocAlign((MAX_MALLOC)(ulRegister * NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 4);// create receiver buffer memory pool
    ptrBd = uMallocAlign((MAX_MALLOC)(sizeof(STM32_BD)  *NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 4); // create a smaller pool of buffer descriptors
    ETH_DMARDLAR = (unsigned long)ptrBd;                                 // inform the MAC where the first buffer descriptor is located
#if defined _WINDOWS
    ETH_DMACHRDR = ETH_DMARDLAR;
#endif
    ptrRxBd = ptrBd;                                                     // remember where it starts
    while (i-- != 0) {                                                   // initialise the buffer descriptors
        ptrBd->bd_dma_buf1_address = ptrBuffer;                          // buffer location
        ptrBd->bd_word0 = RX_BD_WORD0_OWN;
        ptrBd->bd_word1 = (ulRegister | RX_BD_WORD1_RCH);                // size of frame which can be accepted in the buffer
        if (i == 0) {
            ptrBd->bd_dma_buf2_address = ptrRxBd;                        // set final            
        }
        else {
            ptrBd->bd_dma_buf2_address = (ptrBd + 1);
            ptrBuffer += ulRegister;
            ptrBd++;
        }
    }

    ulRegister = pars->usSizeTx;

    i = NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE;

    ptrBuffer = uMallocAlign((MAX_MALLOC)(ulRegister * NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE), 4);// create transmitter buffer memory pool
    ptrBd = uMallocAlign((MAX_MALLOC)(sizeof(STM32_BD) * NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE), 4); // create a smaller pool of buffer descriptors

    ETH_DMATDLAR = (unsigned long)ptrBd;                                 // inform the MAC where the first buffer descriptor is located
    ptrFirstTxBd = ptrTxBd = ptrBd;                                      // remember where it starts
    while (i-- != 0) {                                                   // initialise the buffer descriptors
        ptrBd->bd_dma_buf1_address = ptrBuffer;                          // set buffer address
        ptrBd->bd_word0 = TX_BD_WORD0_TCH;
        if (i == 0) {
            ptrBd->bd_dma_buf2_address = ptrFirstTxBd;
        }
        else {
            ptrBd->bd_dma_buf2_address = (ptrBd + 1);
            ptrBuffer += ulRegister;
            ptrBd++;
        }
    }

    fnEnterInterrupt(irq_ETH_ID, PRIORITY_EMAC, EMAC_Interrupt);         // enter EMAC interrupt handler

#if defined LAN_REPORT_ACTIVITY
    ETH_DMAIER = (ETH_DMAIER_TIE | ETH_DMAIER_RIE | ETH_DMAIER_NISE);    // enable rx and tx interrupts
#else
    ETH_DMAIER = (ETH_DMAIER_RIE | ETH_DMAIER_NISE);                     // enable rx interrupt
#endif

#if defined PHY_INTERRUPT                                                // if PHY interrupt line is connected use it to detect link state changes and control auto-negotiated speed
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = fnPhyInterrupt;                        // handling function
    interrupt_setup.int_port = PHY_INT_PORT;                             // the port used
    interrupt_setup.int_priority = PHY_INT_PRIORITY;                     // port interrupt priority
    interrupt_setup.int_port_bit = PHY_INTERRUPT;                        // the input connected
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE);                 // interrupt on this edge
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt
#endif

#if defined _DP83848
    fnWriteMII_PHY(DP83848_MII_INTERRUPT_STATUS_REGISTER,  (DP83848_LINK_INT_EN | DP83848_ANC_INT_EN)); // enable link status change and auto-negotiation complete interrupts
    fnWriteMII_PHY(DP83848_MII_INTERRUPT_CONTROL_REGISTER, (DP83848_INT_OE | DP83848_INT_EN)); // enable interrupt output to trigger processor interrupt input
#elif defined _KS8721                                                    // {32}
    fnWriteMII_PHY(KS8721_INTERRUPT_CONTROL_STATUS_REGISTER,             // enable various PHY interrupts
//         PHY_REMOTE_FAULT_INT_ENABLE
//         PHY_LINK_PARTNER_ACK_INT_ENABLE
//         PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//         PHY_PAGE_RECEIVED_INT_ENABLE
//         PHY_RECEIVE_ERROR_INT_ENABLE
//         PHY_JABBER_INT_ENABLE
          (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
    );
#elif defined _ST802RT1B
    fnWriteMII_PHY(ST802RT1_INTERRUPT_CONTROL_REGISTER, (ST802RT1_LK_DWN_EN | ST802RT1_AN_CMPL_EN | ST802RT1_INT_EN)); // enable PHY interrupts on link state changes
    fnReadMII_PHY(ST802RT1_PHY_STATUS_REGISTER);                         // read the status in order to allow interrupts to start operating
#endif
    ETH_MACCR |= (ETH_MACCR_RE | ETH_MACCR_TE);                          // enable transmission and reception
    ETH_DMAOMR |= (ETH_DMAOMR_RSF | ETH_DMAOMR_TSF | ETH_DMAOMR_FTF | ETH_DMAOMR_ST | ETH_DMAOMR_SR); // flush transmit FIFO and start monitoring transmission/reception buffer descriptors
    ETH_DMARPDR = 0;
#if defined _WINDOWS
    fnOpenDefaultHostAdapter();                                          // configure PC NIC for Ethernet operation
#endif
    return 0;                                                            // ethernet interface was successfully configured
}


#if defined _WINDOWS
    #if defined USE_IPV6 || defined USE_IGMP
static int fnIsHashMulticast(unsigned char *ucData)
{
    unsigned long ulCRC32 = 0xffffffff;                                  // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    int i, bit;
    unsigned char ucByte;
    unsigned char ucHash = 0;
    if ((ETH_MACFFR & ETH_MACFFR_PAM) && (*ucData & 0x01)) {             // all multicast accept and multicast bit set
        return 1;
    }
    if (!(ETH_MACFFR & ETH_MACFFR_HM)) {                                 // multicast hash not enabled
        return 0;
    }
    if (!(*ucData & 0x01)) {                                             // if not multicast, don't check
        return 0;
    }
	for (i = 0; i < MAC_LENGTH; i++) {
        ucByte = *ucData++;;
        for (bit = 0; bit < 8; bit++) {
            if ((ulCRC32 ^ ucByte) & 1) {
                ulCRC32 = (ulCRC32 >> 1) ^ CRC_POLY;
            }
            else {
                ulCRC32 >>= 1;
            }
            ucByte >>= 1;
        }
	}
    for (i = 0; i < 6; i++) {                                            // the hash is the negated first 6 bits of the CRC32 in bit swapped order
        if ((ulCRC32 & (1 << i)) == 0) {
            ucHash |= (1 << (5 - i));
        }
    }

    if (ucHash < 32) {
        if (ETH_MACHTLR & (0x00000001 << ucHash)) {
            return 1;                                                    // multicast hash match
        }
    }
    else {
        ucHash -= 32;
        if (ETH_MACHTHR & (0x00000001 << ucHash)) {
            return 1;                                                    // multicast hash match
        }
    }
    return 0;                                                            // not a match
}
    #endif

    // This routine is supplied to aid the simulator. It checks whether Ethereal frames should be accepted based
    // on the programmed MAC address and the Ethernet operating mode
    //
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen)
    {
        unsigned char ucProgrammedMAC[MAC_LENGTH];

        if (!(ETH_MACCR & ETH_MACCR_RE)) {
            return 0;                                                    // Ethernet interface not receiving
        }

        if (usLen < ((2 * MAC_LENGTH) + 2)) {
            return 0;                                                    // too short to be a valid Ethernet frame
        }

        ucProgrammedMAC[5] = (unsigned char)(ETH_MACA0HR >> 8);
        ucProgrammedMAC[4] = (unsigned char)(ETH_MACA0HR);
        ucProgrammedMAC[3] = (unsigned char)(ETH_MACA0LR >> 24);
        ucProgrammedMAC[2] = (unsigned char)(ETH_MACA0LR >> 16);
        ucProgrammedMAC[1] = (unsigned char)(ETH_MACA0LR >> 8);
        ucProgrammedMAC[0] = (unsigned char)(ETH_MACA0LR);

    #if !defined _DEVELOP_SIM
        if (uMemcmp(&ucData[MAC_LENGTH], ucProgrammedMAC, MAC_LENGTH) == 0) { // we never accept echos of our own transmissions - check before broadcast
            return 0;
        }
    #endif

        if (!(uMemcmp(ucData, cucBroadcast, MAC_LENGTH))) {
            if (!(ETH_MACFFR & ETH_MACFFR_BFD)) {
                return 1;                                                // broadcast accepted
            }
            else {
                return 0;                                                // broadcast but broadcasts not accepted
            }
        }


        if ((ETH_MACFFR & ETH_MACFFR_HPF) && (!(uMemcmp(ucData, ucProgrammedMAC, MAC_LENGTH)))) {
            return 2;                                                    // our MAC
        }

        if (!uMemcmp(&ucData[MAC_LENGTH], ucProgrammedMAC, MAC_LENGTH)) {
            return 0;                                                    // we never accept echos of our own transmissions
        }

        if (ETH_MACFFR & ETH_MACFFR_PM) {
            return 3;                                                    // in promiscuous mode we accept everything
        }
    #if defined USE_IPV6 || defined USE_IGMP                             // {26}
        if (fnIsHashMulticast(&ucData[0])) {
            return 4;                                                    // multicast hash match
        }
    #endif
        return 0;                                                        // no match so reject
    }
#endif

// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
extern int fnWaitTxFree(void)
{
    return 0;                                                            // no wait foreseen
}

// This can be called by the ethernet driver to find out how many receive buffers the Ethernet Controller has
//
extern int fnGetQuantityRxBuf(void)
{
    return NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE;
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return 0;                                                            // for compatibility - the STM32 uses buffer descriptors for tracking buffers
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    return (unsigned char *)ptrTxBd->bd_dma_buf1_address;
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been processed
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
    ptrRxBd->bd_word0 = RX_BD_WORD0_OWN;                                 // allow the old buffer to be used again
    ptrRxBd = (STM32_BD *)ptrRxBd->bd_dma_buf2_address;                  // go to next buffer descriptor
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
    ptrTxBd->bd_word1 = DataLen;                                         // set new frame length (frames smaller than 60 bytes are automatically padded by the Ethernet transmitter)
    #if defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD && !defined _WINDOWS
    ptrTxBd->bd_word0 = (TX_BD_WORD0_CIC_IP_ALL | TX_BD_WORD0_IC | TX_BD_WORD0_FS | TX_BD_WORD0_LS | TX_BD_WORD0_TCH | TX_BD_WORD0_OWN); // frame ready - IP payload checksum is automatically inserted
    #elif defined IP_TX_CHECKSUM_OFFLOAD && !defined _WINDOWS
    ptrTxBd->bd_word0 = (TX_BD_WORD0_CIC_IP_HEAD | TX_BD_WORD0_IC | TX_BD_WORD0_FS | TX_BD_WORD0_LS | TX_BD_WORD0_TCH | TX_BD_WORD0_OWN); // frame ready - IP payload checksum is automatically inserted
    #else
    ptrTxBd->bd_word0 = (TX_BD_WORD0_IC | TX_BD_WORD0_FS | TX_BD_WORD0_LS | TX_BD_WORD0_TCH | TX_BD_WORD0_OWN); // frame ready
    #endif
  //ETH_DMASR = ETH_DMAIER_TPSIE;                                        // reset possible under-run flag
    ETH_DMATPDR = 0;                                                     // issue a transmit poll demand

    #if defined _WINDOWS
    ETH_DMASR = 0;
    fnSimulateEthTx(ptrTxBd->bd_word1, (ptr_put - DataLen));
    if ((usPhyMode & PHY_LOOP_BACK) || (ETH_MACCR & ETH_MACCR_LM)) {     // if the PHY or EMAC is in loop back mode, simulate reception of sent frame
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
    if (ptrTxBd->bd_word0 & TX_BD_WORD0_IC) {
        ETH_DMASR = ETH_DMAIER_TIE;
    }
    ptrTxBd->bd_word0 = 0;                                               // simulate the ownership changing
    if ((ETH_DMASR & ETH_DMAIER_TIE) & ETH_DMAIER) {                     // if tx interrupt enabled
        EMAC_Interrupt();                                                // simulate end of transmission - tx interrupt
    }
    #endif
    ptrTxBd = (STM32_BD *)ptrTxBd->bd_dma_buf2_address;                  // set next buffer descriptor to be used
    return (DataLen);
}
