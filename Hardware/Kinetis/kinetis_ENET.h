/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_ENET.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    11.06.2015 Modify the handling of link-down with _KSZ8081RNA         {1}
    17.01.2016 Add fnResetENET() when restarting Ethernet                {2}
    27.01.2016 Add _KSZ8051RNL                                           {3}

*/

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined ETH_BD_MALLOC_ALIGN
    #define ETH_BD_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
#endif
#if !defined ETH_RX_BUF_MALLOC_ALIGN
    #define ETH_RX_BUF_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
#endif
#if !defined ETH_TX_BUF_MALLOC_ALIGN
    #define ETH_TX_BUF_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
#endif

#if defined _WINDOWS || defined ETHER_DBSWP                              // {21} device with ETHER_DBSWP control in the ECR register can configure the EMAC buffer decriptors to use little-endian mode
    #define fnLE_ENET_add(x)  x
    #define fnLE_ENET_word(x) x
#else                                                                    // when EMAC operates with buffer descripts in big-endian mode addresses and lengths are converted to little-endian using these macros
    // Convert between little and big-endian address
    //
    #define fnLE_ENET_add(x) (unsigned char *)(((unsigned long)(x) >> 24) | (((unsigned long)(x) >> 8) & 0x0000ff00) | (((unsigned long)(x) << 8) & 0x00ff0000) | (((unsigned long)(x) << 24) & 0xff000000))

    // Convert between little and big-endian short word
    //
    #define fnLE_ENET_word(x) (((unsigned short)(x) >> 8) | ((unsigned short)(x) << 8))
#endif

//#define TEST_CRC_32_IEEE                                               // use Ethernet receptions to verify CRC module CRC-32 IEEE settings (test only)

#if defined EMAC_ENHANCED
    #if defined ETHER_DBSWP                                              // {21} if the EMAC supports buffer descriptors in little-endian format enable this mode
        #define ENABLE_ETHERNET_OPERATION (ETHER_EN | ETHER_EN1588 | ETHER_DBSWP)
    #else
        #define ENABLE_ETHERNET_OPERATION (ETHER_EN | ETHER_EN1588)
    #endif
#else
    #if defined ETHER_DBSWP                                              // {21} if the EMAC supports buffer descriptors in little-endian format enable this mode
        #define ENABLE_ETHERNET_OPERATION (ETHER_EN | ETHER_DBSWP)
    #else
        #define ENABLE_ETHERNET_OPERATION (ETHER_EN)
    #endif
#endif

#if defined ETHERNET_RMII && defined ETHERNET_RMII_CLOCK_INPUT           // RMII mode with external clock inut
    #define ETHERNET_CONTROLLER_CLOCK   50000000                         // 50MHz external clock supplied as Ethernet controller clock
#else
    #define ETHERNET_CONTROLLER_CLOCK   SYSTEM_CLOCK                     // Ethernet controller clocked by the system clock
#endif
/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if (defined ETH_INTERFACE && !defined NO_INTERNAL_ETHERNET) && !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME
    static const unsigned char EMAC_RXA_int_message[HEADER_LENGTH] = {0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RX_INTERRUPT}; // define fixed interrupt event
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if (defined ETH_INTERFACE && !defined NO_INTERNAL_ETHERNET)
    #if defined USE_MULTIPLE_BUFFERS
        static unsigned char ucMultiBuf = 0;
    #elif defined _MAGIC_RESET_FRAME
        static volatile KINETIS_FEC_BD *ptrMagicRxBd = 0;                // initially disabled
    #endif
    static KINETIS_FEC_BD *ptrRxBd_start, *ptrTxBd_start;
    static volatile KINETIS_FEC_BD *ptrRxBd, *ptrTxBd;
    static unsigned short usEthernetLength = 0;
    #if defined PHY_TAIL_TAGGING && !defined NO_INTERNAL_ETHERNET        // {44}
        static int iTailTagging = 0;                                     // tail tagging is disabled by default
        static unsigned char ucTailTagPort = ETH_TX_PORT_SWITCH;         // port for transmission - default is for switch operation
    #endif
    #if defined EMAC_ENHANCED && defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD && defined IPV4_SUPPORT_TX_FRAGMENTATION
        static int iFragmentTx = 0;
    #endif
    #if defined USE_IGMP && !defined NO_INTERNAL_ETHERNET                // {74}
        #if defined USE_IPV6
            #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 2)
        #else
            #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 1)
        #endif
        static unsigned char ucGroupHashes[MAX_MULTICAST_FILTERS] = {0};
    #endif
    #if defined _WINDOWS
        static unsigned short usPhyMode = 0;
    #endif
#endif


/* =================================================================== */
/*                          Ethernet Controller                        */
/* =================================================================== */



#if defined PHY_TAIL_TAGGING                                             // {44}
extern void fnSetTailTagMode(int iPort)
{
    if (iPort != iTailTagging) {                                         // only react to changes
        iTailTagging = iPort;                                            // enable/disable tail tagging (the enabled value controls frame reception when simulating)
    #if defined USE_IP
        fnDeleteArp();                                                   // delete ARP table when changing mode
    #endif
    #if defined USE_IPV6
        fnDeleteNeighbors();                                             // delete the IPV6 neighbor cache when changing mode
    #endif
        if (iTailTagging != 0) {                                         // if enabled
            _fnMIIwrite(0, 3, 0x74);                                     // enable tail-tagging operation at Micrel switch
            _fnMIIwrite(0, 0x11, 0x47);                                  // set both ports to pass all reception to sniffer port
            _fnMIIwrite(0, 0x21, 0x47);
            _fnMIIwrite(0, 0x31, 0x87);                                  // define the processor as sniffer port (promiscuous)
        }
        else {
            _fnMIIwrite(0, 3, 0x34);                                     // disable operation at Micrel switch
            _fnMIIwrite(0, 0x11, 0x07);                                  // don't pass reception to sniffer port
            _fnMIIwrite(0, 0x21, 0x07);
            _fnMIIwrite(0, 0x31, 0x07);                                  // disable the processor as sniffer port (non-promiscuous) since we return to normal switch mode
        }
    }
}

extern int fnGetTailTagMode(void)
{
    return (iTailTagging);                                               // return present mode
}

extern void fnSetTailTagPort(unsigned char ucPort)
{
    ucTailTagPort = ucPort;                                              // set the port for frame transmission
}

extern unsigned char fnGetTailTagPort(void)
{
    return (ucTailTagPort);                                              // return present transmission port
}
#endif

#if defined EMAC_ENHANCED && defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD && defined IPV4_SUPPORT_TX_FRAGMENTATION
extern void fnSetFragmentMode(int iMode)
{
    iFragmentTx = iMode;
}
#endif


// This is called when an Ethernet event has been received. Generally it is due to a reception and returns channel and message details.
//
extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame)
{
#if defined LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_int_message[HEADER_LENGTH] = {0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_RX_INTERRUPT};
#endif
    if (EMAC_RX_INTERRUPT == *ucEvent) {
#if defined USE_MULTIPLE_BUFFERS
        if ((rx_frame->frame_size = fnExtractNextRx()) == 0) {           // get frame details from present buffer descriptor
            return -1;                                                   // nothing (else) waiting
        }
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ucEthernetInput;     // return pointer to the fixed linear input buffer
#else
        if ((ptrRxBd->usBDControl & EMPTY_BUFFER) != 0) {
            return -1;                                                   // nothing else waiting
        }
        else if (ptrRxBd->usBDLength == 0) {                             // zero length is invalid
            return -1;
        }
        if ((ptrRxBd->usBDControl & (TRUNCATED_FRAME | OVERRUN_FRAME)) != 0) { // corrupted reception
            if ((ptrRxBd->usBDControl & TRUNCATED_FRAME) == 0) {
    #if defined USE_IP_STATS
                fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES, DEFAULT_NETWORK); // we lost a frame due to RX overrun so count the event
    #endif
            }
            rx_frame->frame_size = 0;                                    // reject further frame processing
        }
        else {                                                           // reception without errors
            rx_frame->frame_size = fnLE_ENET_word(ptrRxBd->usBDLength);  // length in the buffer
            if ((RCR & CRCFWD) == 0) {                                   // if the CRC-32 is not being removed reduce the reception length (the user knows that it will be in the buffer after the data)
                rx_frame->frame_size -= 4;
            }
    #if defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD
            rx_frame->usFragment_CS = fnLE_ENET_word(ptrRxBd->usPayloadCS); // save the payload checksum of the possible fragment
            rx_frame->ucSpecialHandling = 0;
    #endif
    #if defined PHY_TAIL_TAGGING                                         // {44}
            if (iTailTagging != 0) {                                     // if tail tagging is enabled
                rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)fnLE_ENET_add(ptrRxBd->ptrBD_Data);
        #if defined _WINDOWS
                rx_frame->ucRxPort = (unsigned char)iTailTagging;        // simulated frames arrive over this port
        #else                                                            // in tail tagging mode the source port is read from the received trame
                rx_frame->frame_size--;                                  // remove tail from the length
                if (rx_frame->ptEth->ucData[rx_frame->frame_size - 14] & 0x01) {
                    rx_frame->ucRxPort = 2;                              // this Ethernet frame arrived over port 2
                }
                else {
                    rx_frame->ucRxPort = 1;                              // this Ethernet frame arrived over port 1
                }
        #endif
            }
            else {
                rx_frame->ucRxPort = 0;                                  // don't care when tail tagging is not enabled
            }
    #endif
        }
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)fnLE_ENET_add(ptrRxBd->ptrBD_Data);
    #if defined TEST_CRC_32_IEEE                                         // test option checking
        if (rx_frame->frame_size != 0) {
            unsigned long ulLength = rx_frame->frame_size;
            unsigned long *ptr = (unsigned long *)rx_frame->ptEth;
            POWER_UP(6, SIM_SCGC6_CRC);                                  // power up the CRC module
            CRC_CTRL = (CRC_CTRL_TCRC_32 | CRC_CTRL_TOTR_BITS_BYTES | CRC_CTRL_TOT_BITS_BYTES | CRC_CTRL_FXOR); // mode required for CRC-32 IEEE compatibility
            CRC_GPOLY = 0x04c11db7;                                      // set the polynomial
            CRC_CTRL = (CRC_CTRL_TCRC_32 | CRC_CTRL_WAS | CRC_CTRL_TOTR_BITS_BYTES | CRC_CTRL_TOT_BITS_BYTES | CRC_CTRL_FXOR); // enable write of seed
            CRC_CRC = 0xffffffff;                                        // seed is 0xffffffff
            CRC_CTRL = (CRC_CTRL_TCRC_32 | CRC_CTRL_TOTR_BITS_BYTES | CRC_CTRL_TOT_BITS_BYTES | CRC_CTRL_FXOR); // switch back to data input mode
            while (ulLength >= 4) {                                      // main part using long words
                CRC_CRC = *ptr++;                                        // process long world
                ulLength -= 4;                                           // by bytes at a time - the CRC calculation requires 2 clock cycles
            }
        #if defined ERRATE_E2776_SOLVED || defined _WINDOWS
            if (ulLength != 0) {                                         // remaining bytes that didn't fit into a long word
                unsigned char *ptrByte = (unsigned char *)ptr;
                CRC_CRC_HU = *ptrByte++;                                 // write bytes starting with MSB
                ulLength--;
                if (ulLength != 0) {
                    CRC_CRC_HL = *ptrByte++;
                    ulLength--;
                    if (ulLength != 0) {
                        CRC_CRC_LU = *ptrByte++;
                        ulLength--;
                    }
                }
                ptr = (unsigned long *)ptrByte;
            }

            if (CRC_CRC == *ptr) {                                       // check the CRC that was calculated with that received at the end of the Ethernet frame
                // CRC-32 matches
            }
            else {
                // CRC-32 mismatch
            }
        #else                                                            // errata 2776 workaround
            if (ulLength != 0) {                                         // remaining bytes that didn't fit into a long word
            #if !defined _COMPILE_KEIL
                register unsigned long ull = 0;
                register int i;
            #else
                register unsigned long ull;
            #endif
                CRC_CTRL = (CRC_CTRL_TCRC_32 | CRC_CTRL_TOTR_BITS_BYTES | CRC_CTRL_FXOR); // switch to mode without swapping
            #if defined _COMPILE_KEIL
                ull = __rbit(*ptr);                                      // swap all bits of long word
            #else
                // To do - insert asm code for other compilers if required
                //
                for (i = 0; i < 32; i++) {
                    if (*ptr & (0x80000000 >> i)) {
                        ull |= (0x00000001 << i);
                    }
                }
            #endif
                CRC_CRC_LL = (unsigned char)(ull >> 24);                 // swap byte
                ulLength--;
                if (ulLength != 0) {
                    CRC_CRC_LU = (unsigned char)(ull >> 16)              // swap byte
                    ulLength--;
                    if (ulLength != 0) {
                        CRC_CRC_HL = (unsigned char)(ull >> 8);          // swap byte
                        ulLength--;
                    }
                }
            }

            if (CRC_CRC == *ptr) {                                       // check the CRC that was calculated with that received at the end of the Ethernet frame
                // CRC-32 matches
            }
            else {
                // CRC-32 mismatch
            }
        #endif
        }
    #endif
#endif
#if IP_INTERFACE_COUNT > 1
        rx_frame->ucInterface = (ETHERNET_INTERFACE >> INTERFACE_SHIFT); // {30}
        rx_frame->ucInterfaceHandling = DEFAULT_INTERFACE_CHARACTERISTICS; // handling that this interface needs
#endif
#if defined LAN_REPORT_ACTIVITY
        fnWrite(INTERNAL_ROUTE, (unsigned char *)EMAC_int_message, HEADER_LENGTH); // inform the task of event
#endif
        return 0;                                                        // channel received from 0 (fixed value for compatibility with other devices)
    }
#if defined USE_IP_STATS && !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME
    else {
        fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);                    // count other unexpected events
    }
#endif
    return -1;                                                           // channel invalid
}

#if defined LAN_REPORT_ACTIVITY || defined LAN_TX_FPU_WORKAROUND         // {12}
// Ethernet transmission interrupt
//
static __interrupt void _fec_txf_isr(void)
{
    #if defined LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_int_message[HEADER_LENGTH] = {0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT};
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char *)EMAC_int_message, HEADER_LENGTH); // inform the task of event
    uEnable_Interrupt();                                                 // release     
    #endif
    EIR = TXF;                                                           // reset interrupt
    #if defined _WINDOWS
    EIR &= ~TXF;
    #endif
    #if defined LAN_TX_FPU_WORKAROUND
    TDAR = TDAR_TDAR;                                                    // {12} retrigger buffer polling to ensure that no waiting buffer is missed (using when the errata has been solved can have detremental effects)
    #endif
}
#endif

#if defined _MAGIC_RESET_FRAME
extern void fnEnableMagicFrames(int iOnOff)
{
    uDisable_Interrupt();                                                // protect from ethernet reception interrupts
        if (iOnOff == 0) {
            ptrMagicRxBd = 0;                                            // disable
        }
        else {
            ptrMagicRxBd = ptrRxBd;                                      // enabled by synchronising the magic frame buffer with the receiver
        }
    uEnable_Interrupt();                                                 // allow further interrupt processing
}
#endif

// Ethernet reception interrupt
//
static __interrupt void _fec_rx_frame_isr(void)
{
#if defined _MAGIC_RESET_FRAME  && !defined USE_MULTIPLE_BUFFERS         // {22}
    if (ptrMagicRxBd != 0) {
        if ((ptrMagicRxBd->usBDControl & EMPTY_BUFFER) == 0) {           // ignore if buffer is empty
            if ((ptrMagicRxBd->usBDControl & (TRUNCATED_FRAME | OVERRUN_FRAME)) == 0) { // only handle valid frames
                unsigned short usLength = fnLE_ENET_word(ptrMagicRxBd->usBDLength); // length in the buffer
                if ((RCR & CRCFWD) == 0) {                               // if the CRC-32 is not being removed reduce the reception length (the user knows that it will be in the buffer after the data)
                    usLength -= 4;
                }
                if (usLength == 93) {                                    // payload length must be 93 bytes in length
                    unsigned char *ptrData = &((ETHERNET_FRAME_CONTENT *)fnLE_ENET_add(ptrMagicRxBd->ptrBD_Data))->ethernet_frame_type[0];
                    int i;
                    for (i = 0; i < 24; i++) {                           // remaining header must be (unsigned short)_MAGIC_RESET_FRAME repeated 24 times
                        if (*ptrData++ != (unsigned char)(_MAGIC_RESET_FRAME >> 8)) {
                            goto _not_magic_frame;
                        }
                        if (*ptrData++ != (unsigned char)(_MAGIC_RESET_FRAME)) {
                            goto _not_magic_frame;
                        }
                    }
                    // A magic frame has been received - the 61st byte is used to signal the type and 32 bytes of optional data are passed
                    //
                    fnMagicFrame(*ptrData, (ptrData + 1));               // allow the user to handle the magic frame
                }
            }
_not_magic_frame:
            if ((ptrMagicRxBd->usBDControl & WRAP_BIT_RX) != 0) {
                ptrMagicRxBd = (KINETIS_FEC_BD *)ERDSR;                  // set to first BD
            }
            else {
                ptrMagicRxBd++;                                          // increment to next buffer
            }
        }
    }
#endif
#if defined ETHERNET_RELEASE_AFTER_EVERY_FRAME
    uTaskerStateChange(TASK_ETHERNET, UTASKER_ACTIVATE);                 // schedule the Ethernet task
#else
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char *)EMAC_RXA_int_message, HEADER_LENGTH); // inform the Ethernet task that a new frame is waiting
    uEnable_Interrupt();                                                 // release  
#endif
    EIR = RXF;                                                           // reset interrupt flag
#if defined _WINDOWS
    EIR &= ~RXF; 
#endif  
}

// Re-install flushed buffer descriptors and restart operation
//
static void fnRestartEthernet(void)
{
#if defined _WINDOWS
    extern void fnResetENET(void);
#endif
    int i;
    KINETIS_FEC_BD *ptrBD = ptrRxBd_start;
#if defined _WINDOWS
    fnResetENET();                                                       // {2} reset simulation
#endif
    ptrRxBd = (volatile KINETIS_FEC_BD *)ptrRxBd_start;
#if defined _MAGIC_RESET_FRAME && !defined USE_MULTIPLE_BUFFERS          // {22}
    if (ptrMagicRxBd != 0) {
        ptrMagicRxBd = ptrRxBd;
    }
#endif
    ERDSR = (unsigned long)ptrRxBd_start;
    EMRBR = usEthernetLength;
#if defined USE_MULTIPLE_BUFFERS
    for (i = 0; i < (NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * ucMultiBuf); i++) { // initialise each Rx buffer descriptor
        if (i == ((NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * ucMultiBuf) - 1)) {
            ptrBD->usBDControl = (EMPTY_BUFFER | WRAP_BIT_RX);
        }
        else {
            ptrBD->usBDControl = EMPTY_BUFFER;
        }
        ptrBD++;
    }
#else
    for (i = 0; i < NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE; i++) {      // initialise each Rx buffer descriptor
        if (i == (NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE - 1)) {
            ptrBD->usBDControl = (EMPTY_BUFFER | WRAP_BIT_RX);
        }
        else {
            ptrBD->usBDControl = EMPTY_BUFFER;
        }
        ptrBD++;
    }
#endif
    ptrBD = ptrTxBd_start;
    ptrTxBd = ptrTxBd_start;
    ETDSR = (unsigned long)ptrTxBd_start;
    for (i = 0; i < NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE; i++) {
        if (i == (NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE - 1)) {
            ptrBD->usBDControl = (WRAP_BIT_TX);
        }
        else {
            ptrBD->usBDControl = 0;
        }
        ptrBD++;
    }
    ECR = ENABLE_ETHERNET_OPERATION;                                     // re-enable FEC
    RDAR = 0;                                                            // enable RX BD polling
}

// Ethernet error and miscellaneous interrupt
//
static __interrupt void _fec_misc(void)
{
    unsigned long ulSources = EIR;                                       // check what interrupt occurred
    EIR = (BABR | BABT | GRA | EBERR | LC | RL | UN);                    // reset the interrupt event (put break point here to analyse what happened)
#if defined _WINDOWS
    EIR &= ~(BABR | BABT | GRA | EBERR | LC | RL | UN);
#endif
    if (ulSources & EBERR) {                                             // the operation will be stopped if there is a DMA error
        fnRestartEthernet();        
    }
}

#if defined _DP83849I                                                    // {52} adjust registers and flags to suit the PHY
    #define PHY_INTERRUPT_REGISTER 0x12
    #define PHY_LINK_STATE_CHANGE  0x2000
    #define PHY_LINK_STATUS_REG    0x10
    #define PHY_LINK_MASK          0x0007
    #define PHY_LINK_10BASE_T_HALF_DUPLEX   0x0003
    #define PHY_LINK_100BASE_TX_HALF_DUPLEX 0x0001
    #define PHY_LINK_10BASE_T_FULL_DUPLEX   0x0007
    #define PHY_LINK_100BASE_TX_FULL_DUPLEX 0x0005
#elif defined _KSZ8081RNA || defined _KSZ8051RNL                         // {84}{3}
    #define PHY_INTERRUPT_REGISTER KS8041_INTERRUPT_CONTROL_STATUS_REGISTER // 0x1b
    #define PHY_LINK_STATE_CHANGE  (PHY_LINK_UP_INT)                     // {1}
    #define PHY_LINK_STATUS_REG    0x1e                                  // PHY control 1
    #define PHY_LINK_MASK          0x0007
    #define PHY_LINK_10BASE_T_FULL_DUPLEX   0x0005
    #define PHY_LINK_100BASE_TX_FULL_DUPLEX 0x0006
    #define PHY_LINK_100BASE_TX_HALF_DUPLEX 0x0002
    #define PHY_LINK_10BASE_T_HALF_DUPLEX   0x0001
#else
    #define PHY_INTERRUPT_REGISTER KS8041_INTERRUPT_CONTROL_STATUS_REGISTER
    #define PHY_LINK_STATE_CHANGE  PHY_LINK_UP_INT
    #define PHY_LINK_STATUS_REG    KS8041_100BASETX_PHY_CONTROL_REGISTER
    #define PHY_LINK_MASK          PHY_MODE_MASK
    #define PHY_LINK_10BASE_T_HALF_DUPLEX   PHY_MODE_10BASE_T_HALF_DUPLEX
    #define PHY_LINK_100BASE_TX_HALF_DUPLEX PHY_MODE_100BASE_TX_HALF_DUPLEX
    #define PHY_LINK_10BASE_T_FULL_DUPLEX   PHY_MODE_10BASE_T_FULL_DUPLEX
    #define PHY_LINK_100BASE_TX_FULL_DUPLEX PHY_MODE_100BASE_TX_FULL_DUPLEX
#endif

// Function to read from the MII interface
//
static unsigned short fnMIIread(unsigned char _mpadr, unsigned char _mradr)
{
#if defined PHY_MICREL_SMI
    if (_mpadr == 0) {                                                   // {19}
    	int i;
        #define MDIO_LINE      PORTB_BIT0
        #define MDC_LINE       PORTB_BIT1
        #define SET_MDC_HIGH()  _SETBITS(B, MDC_LINE);_SETBITS(B, MDC_LINE);_SETBITS(B, MDC_LINE);_SETBITS(B, MDC_LINE)
        #define SET_MDC_LOW()   _CLEARBITS(B, MDC_LINE);_CLEARBITS(B, MDC_LINE);_CLEARBITS(B, MDC_LINE);_CLEARBITS(B, MDC_LINE)
        // Bit bang to achieve the operation since the read doesn't work with the Kinetis in SMI mode
        //
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (MDIO_LINE | MDC_LINE), (MDIO_LINE), (PORT_SRE_FAST)); // GPIO driving with initial value
    	for (i = 0; i < 32; i++) {
            SET_MDC_HIGH();                                              // preamble with 32 clocks
            SET_MDC_LOW();
        }
        _CLEARBITS(B, MDIO_LINE);                                        // '0'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        _SETBITS(B, MDIO_LINE);                                          // '1'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        _CLEARBITS(B, MDIO_LINE);                                        // '0'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        _SETBITS(B, MDIO_LINE);                                          // '1'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
    	for (i = 0; i < 8; i++) {                                        // register value
        	if (_mradr & 0x80) {
                _SETBITS(B, MDIO_LINE);                                  // '1'
            }
        	else {
                _CLEARBITS(B, MDIO_LINE);                                // '0'
            }
            SET_MDC_HIGH();                                              // clock
        	_mradr <<= 1;
            SET_MDC_LOW();
        }
        SET_MDC_HIGH();                                                  // clock
        _FLOAT_PORT(B, MDIO_LINE);                                       // set data line as input
        SET_MDC_LOW();

    	for (i = 0; i < 9; i++) {                                        // 9 clocks reading in '0'
            SET_MDC_HIGH();                                              // clock
            SET_MDC_LOW();
        }
    	_mradr = 0;
    	for (i = 0; i < 8; i++) {                                        // 8 clocks reading in data
            SET_MDC_HIGH();                                              // clock
        	_mradr <<= 1;
            if (_READ_PORT_MASK(B, MDIO_LINE)) {                         // if '1'
            	_mradr |= 1;
            }
            SET_MDC_LOW();
        }
        // Return pin mode when completed
        //
        _CONFIG_PERIPHERAL(B, 0, PB_0_MII0_MDIO);                        // MII0_MDIO on PB.0 (alt. function 4)
        _CONFIG_PERIPHERAL(B, 1, PB_1_MII0_MDC);                         // MII0_MDC on PB.1 (alt. function 4)
    	return _mradr;
    }
	else {
        MMFR = (MII_READ | (_mpadr << 23) | (_mradr << 18));             // command read from given address
    }
#else
    MMFR = (MII_READ | (_mpadr << 23) | (_mradr << 18));
#endif

    while ((EIR & MII) == 0) {                                           // wait until the read has completed
#if defined _WINDOWS
        EIR |= MII;
#endif
    };
    EIR = MII;                                                           // reset the interrupt event
#if defined _WINDOWS
    EIR &= ~MII;
    if (PHY_INTERRUPT_REGISTER == _mradr) {      
        MMFR = PHY_LINK_STATE_CHANGE;                                    // link state change
    }
    else if (PHY_REG_CR == _mradr) {
        MMFR = (PHY_R0_DR | PHY_R0_ANE);                                 // default is typically auto-negotiation with 100M speed set
    }
    else if (PHY_LINK_STATUS_REG == _mradr) {
        MMFR = PHY_LINK_10BASE_T_FULL_DUPLEX;                            // state link up 100M
    }
    else if (PHY_REG_ANAR == _mradr) {
        MMFR = (PHY_R4_NP | PHY_R4_100F | PHY_R4_100H | PHY_R4_10F | PHY_R4_10H | 1); // capabilities
    }
    #if defined PHY_IDENTIFIER
    else if (PHY_REG_ID1 == _mradr) {
        MMFR = (unsigned short)(PHY_IDENTIFIER >> 16);                   // ID 1
    }
    else if (PHY_REG_ID2 == _mradr) {
        MMFR = (unsigned short)(PHY_IDENTIFIER);                         // ID 2
    }
    #endif
    #if defined PHY_INTERRUPT
    else if (KS8041_INTERRUPT_CONTROL_STATUS_REGISTER == _mradr) {
        unsigned long ulBit = PHY_INTERRUPT;
        unsigned char ucPortBit = 0;
        while ((ulBit & 0x80000000) == 0) {
            ucPortBit++;
            ulBit <<= 1;
        }
        MMFR = PHY_LINK_UP_INT;
        fnSimulateInputChange(PHY_INTERRUPT_PORT, ucPortBit, SET_INPUT); // clear level sensitive interrupt input
    }
    else if (KS8041_BASIC_STATUS_REGISTER == _mradr) {
        MMFR = PHY_100MEG_HALF_DUPLEX;
    }
    #endif
#endif
    return (unsigned short)MMFR;                                         // return the data read
}


#if defined PHY_POLL_LINK || defined PHY_INTERRUPT                       // {37}
extern void fnCheckEthLinkState(void)
{
    volatile unsigned short usInterrupt;
    #if defined INTERRUPT_TASK_PHY
    unsigned char int_phy_message[HEADER_LENGTH];
    #endif
    #if defined STOP_MII_CLOCK
    MSCR = (((ETHERNET_CONTROLLER_CLOCK/(2 * MII_MANAGEMENT_CLOCK_SPEED)) + 1) << 1); // enable PHY clock for reads
    #endif
    usInterrupt = fnMIIread(PHY_ADDRESS, PHY_INTERRUPT_REGISTER);        // read the cause(s) of the interrupt, which resets the bits
    if (PHY_LINK_STATE_CHANGE & usInterrupt) {                           // a link state change has taken place
        int iFullDuplex = 0;
        usInterrupt = fnMIIread(PHY_ADDRESS, PHY_LINK_STATUS_REG);       // check the  details of link
        switch (usInterrupt & PHY_LINK_MASK) {        
        case PHY_LINK_100BASE_TX_FULL_DUPLEX:
            iFullDuplex = 1;
        case PHY_LINK_100BASE_TX_HALF_DUPLEX:
    #if defined INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = (unsigned char)(LAN_LINK_UP_100 + iFullDuplex); // {45}
    #endif
    #if defined ETHERNET_RMII
            RCR &= ~RMII_10T;                                            // ensure that RMII is operating in 100M mode
    #endif
            break;
        case PHY_LINK_10BASE_T_FULL_DUPLEX:
            iFullDuplex = 1;
        case PHY_LINK_10BASE_T_HALF_DUPLEX:
    #if defined INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = (unsigned char)(LAN_LINK_UP_10 + iFullDuplex); // {45}
    #endif
    #if defined ETHERNET_RMII
            RCR |= RMII_10T;                                             // ensure that RMII is operating in 10M mode
    #endif
            break;
    #if (defined _DP83849I/* || defined _KSZ8081RNA*/) && defined INTERRUPT_TASK_PHY // {52}{84}{1}
        default:                                                         // link has gone down
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
            break;
    #endif
        }
        if (((iFullDuplex != 0) && (RCR & DRT)) || ((iFullDuplex == 0) && ((TCR & FDEN) != 0))) { // only restart if duplex mode has changed
            ECR = 0;                                                     // disable FEC in order to modify duplex mode
            if (iFullDuplex != 0) {                                      // when using RMII it is important to synchronise the mode
                TCR |= FDEN;
                RCR &= ~DRT;                                             // full duplex
            }
            else {
                TCR &= ~FDEN;
                RCR |= DRT;                                              // half duplex
            }
            fnRestartEthernet();                                         // a change in mode require a restart of the Ethernet interface
        }
    }
    #if defined INTERRUPT_TASK_PHY
        #if defined _DP83849I                                            // {52}
    else {
            #if defined STOP_MII_CLOCK                                   // moved down to allow else before the LINK DOWN check
        MSCR = 0;                                                        // disable clock
            #endif
        return;                                                          // link state has not changed so ignore
    }
        #else
    else if (PHY_LINK_DOWN_INT & usInterrupt) {                          // {27} add else so that the LINK UP event is not overwritten to UNKNOWN
        int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
    }
    else {
        int_phy_message[MSG_INTERRUPT_EVENT] = UNKNOWN_INTERRUPT;
    }
        #endif    
    int_phy_message[MSG_DESTINATION_NODE] = int_phy_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    int_phy_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_PHY;
    int_phy_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
    uDisable_Interrupt();                                                // {49} ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char *)int_phy_message, HEADER_LENGTH); // inform the corresponding task
    uEnable_Interrupt();
    #endif
    #if defined STOP_MII_CLOCK                                           // {27} moved down to allow else before the LINK DOWN check
    MSCR = 0;                                                            // disable clock
    #endif
}
#endif

#if defined PHY_INTERRUPT
static void fnPhyInterrupt(void)                                         // port interrupt call-back
{
    uEnable_Interrupt();                                                 // {49} don't block higher priority interrupts while handling
        fnCheckEthLinkState();                                           // {37}
    uDisable_Interrupt();                                                // block interrupts again for compatibility
}
#endif

// Function to write to the MII interface
//
static void fnMIIwrite(unsigned char _mpadr, unsigned char _mradr, unsigned short _mwdata)
{
#if defined PHY_MICREL_SMI
    if (_mpadr == 0) {                                                   // {19}
        MMFR = ((MII_TA | MII_ST) | (_mradr << 18) | _mwdata);           // command SMI write to given SMI register
	}
	else {
        MMFR = (MII_WRITE | (_mpadr << 23) | (_mradr << 18) | _mwdata);  // command write to given address
	}
#else
    MMFR = (MII_WRITE | (_mpadr << 23) | (_mradr << 18) | _mwdata);
#endif
    while ((EIR & MII) == 0) {                                           // wait until the write has completed
#if defined _WINDOWS
        EIR |= MII;
#endif
    };
    EIR = MII;                                                           // reset the interrupt event
#if defined _WINDOWS
    EIR &= ~MII;
#endif
}

#if defined PHY_MULTI_PORT                                               // {18}
extern unsigned short _fnMIIread(unsigned char ucPort, unsigned char _mradr)
{
    return (fnMIIread(ucPort, _mradr));
}

extern void _fnMIIwrite(unsigned char ucPort, unsigned char _mradr, unsigned short _mwdata)
{
    fnMIIwrite(ucPort, _mradr, _mwdata);
}
#else
extern unsigned short _fnMIIread(unsigned char _mradr)
{
    return (fnMIIread(PHY_ADDRESS, _mradr));
}

extern void _fnMIIwrite(unsigned char _mradr, unsigned short _mwdata)
{
    fnMIIwrite(PHY_ADDRESS, _mradr, _mwdata);
}
#endif

#if defined USE_IGMP || defined USE_IPV6
static unsigned char fnCalculateHash(unsigned char *ptrMac)
{
    #define CRC_POLY 0xedb88320                                          // CRC-32 polynomial in reverse direction
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
    ulCRC32 >>= 26;                                                      // the upper 6 bits of the CRC32 determine the hash entry location
    return (unsigned char)ulCRC32;
}

static void fnEnableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        GALR |= (0x00000001 << ucHashRef);                               // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        GAUR |= (0x00000001 << ucHashRef);
    }
}
#endif

#if defined USE_IPV6
// Configure a multicast address for IPV6 use
//
static void fnConfigureMulticastIPV6(void)
{
    unsigned char ucHashRef;
    unsigned char ucMac[MAC_LENGTH];                                     // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    ucMac[0] = 0x33;
    ucMac[1] = 0x33;
    ucMac[2] = 0xff;
    ucMac[3] = ucLinkLocalIPv6Address[DEFAULT_NETWORK][3 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[4] = ucLinkLocalIPv6Address[DEFAULT_NETWORK][4 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[5] = ucLinkLocalIPv6Address[DEFAULT_NETWORK][5 + (IPV6_LENGTH - MAC_LENGTH)];
    ucHashRef = fnCalculateHash(ucMac);
    fnEnableMulticastHash(ucHashRef);
    #if defined USE_IGMP
    ucGroupHashes[0] = ucHashRef;                                        // mark that this hash is required by IPv6
    #endif
}
#endif

#if defined USE_IGMP                                                     // {74}
static void fnDisableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        GALR &= ~(0x00000001 << ucHashRef);                              // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        GAUR &= ~(0x00000001 << ucHashRef);
    }
}

// IGMP adds and removes multicast host groups and requires the receiver's multicast filtering to be modified in the process
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
    unsigned long ulPhyIdentifier;
    unsigned short usRxSize = pars->usSizeRx;
    #if defined PHY_INTERRUPT
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    #endif
    unsigned short usMIIData;
    unsigned char *ptrBuffer;
    KINETIS_FEC_BD *ptrBD;
    int i;
    #if defined ETHERNET_RMII && !defined ETHERNET_RMII_CLOCK_INPUT      // {14} don't do this check if configured to use external reference clock on dedicated input
        #if OSCERCLK != 50000000
            #error Ethernet RMII operation requires a 50MHz external clock signal!!
        #endif
    #endif
    POWER_UP(2, SIM_SCGC2_ENET);                                         // power up the Ethernet controller
    #if defined MPU_AVAILABLE
    MPU_CESR = 0;                                                        // allow concurrent access to MPU controller
    #endif
    #if defined FORCE_PHY_CONFIG
    FNFORCE_PHY_CONFIG();                                                // drive configuration lines and reset
    #endif
    #if defined ETHERNET_MDIO_WITH_PULLUPS
    _CONFIG_PERIPHERAL(B, 0, (PB_0_MII0_MDIO | PORT_PS_UP_ENABLE));      // MII0_MDIO on PB.0 (alt. function 4) with pullup enabled
    #else
    _CONFIG_PERIPHERAL(B, 0, PB_0_MII0_MDIO);                            // MII0_MDIO on PB.0 (alt. function 4)
    #endif
    _CONFIG_PERIPHERAL(B, 1, PB_1_MII0_MDC);                             // MII0_MDC on PB.1 (alt. function 4)

    #if defined JTAG_DEBUG_IN_USE_ERRATA_2541
    _CONFIG_PERIPHERAL(A, 5, (PORT_PS_DOWN_ENABLE));                     // pull the optional line down to 0V to avoid disturbing JTAG_TRST - not needed when using SWD for debugging 
    #else
    _CONFIG_PERIPHERAL(A, 5, PA_5_MII0_RXER);                            // MII0_RXER on PA.5 (alt. function 4)
    #endif
    _CONFIG_PERIPHERAL(A, 12, PA_12_MII0_RXD1);                          // MII0_RXD1 on PA.12 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 13, PA_13_MII0_RXD0);                          // MII0_RXD0 on PA.13 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 14, PA_14_RMII0_CRS_DV);                       // RMII0_CRS_DV/ MII0_RXDV on PA.14 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 15, PA_15_MII0_TXEN);                          // MII0_TXEN on PA.15 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 16, PA_16_MII0_TXD0);                          // MII0_TXD0 on PA.16 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 17, PA_17_MII0_TXD1);                          // MII0_TXD1 on PA.17 (alt. function 4)
    #if defined ETHERNET_RMII && defined ETHERNET_RMII_CLOCK_INPUT       // {14}
    // Add pin connection if it becomes available
    //
        #if defined KINETIS_K65 || defined KINETIS_K66
    _CONFIG_PERIPHERAL(E, 26,  PE_26_ENET_1588_CLKIN);
        #endif
    #endif
    #if !defined ETHERNET_RMII                                           // additional signals used in MII mode
    _CONFIG_PERIPHERAL(A, 9,  PA_9_MII0_RXD3);                           // MII0_RXD3 on PA.9  (alt. function 4)
    _CONFIG_PERIPHERAL(A, 10, PA_10_MII0_RXD2);                          // MII0_RXD2 on PA.10 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 11, PA_11_MII0_RXCLK);                         // MII0_RXCLK on PA.11 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 24, PA_24_MII0_TXD2);                          // MII0_TXD2 on PA.24 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 25, PA_25_MII0_TXCLK);                         // MII0_TXCLK on PA.25 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 26, PA_26_MII0_TXD3);                          // MII0_TXD3 on PA.26 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 27, PA_27_MII0_CRS);                           // MII0_CRS  on PA.27 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 28, PA_28_MII0_TXER);                          // MII0_TXER on PA.28 (alt. function 4)
    _CONFIG_PERIPHERAL(A, 29, PA_29_MII0_COL);                           // MII0_COL  on PA.29 (alt. function 4)
    #endif

    FNRESETPHY();                                                        // control the reset line to the PHY to respect any power up delays if required
                                                                         // EMAC initialisation
    ECR = RESET_FEC;                                                     // EMAC software reset
    #if !defined _WINDOWS
    while (ECR & RESET_FEC) {}                                           // short wait until reset is complete
    #endif
    #if defined LAN_REPORT_ACTIVITY || defined LAN_TX_FPU_WORKAROUND     // {12}
    EIMR = (BABR | BABT | GRA | TXF | RXF | EBERR | LC | RL | UN);       // interrupt sources
    #else
    EIMR = (BABR | BABT | GRA | RXF | EBERR | LC | RL | UN);             // interrupt sources
    #endif
    EIR = 0xffffffff;                                                    // reset all pending interrupts
    #if (((ETHERNET_CONTROLLER_CLOCK/(2 * MII_MANAGEMENT_CLOCK_SPEED)) + 1) > 0x3f) // ensure that there is no overflow when a low speed can't be obtained
    MSCR = 0x7e;                                                         // maximum vaue
    #else
    MSCR = (((ETHERNET_CONTROLLER_CLOCK/(2 * MII_MANAGEMENT_CLOCK_SPEED)) + 1) << 1); // generate the communication channel clock
    #endif
    GAUR = 0;                                                            // reset group hash entries
    GALR = 0;
    IALR = 0;                                                            // reset individual hash entries
    IAUR = 0;
    #if defined USE_IPV6
    PALR = ((network[DEFAULT_NETWORK].ucOurMAC[0] << 24) | (network[DEFAULT_NETWORK].ucOurMAC[1] << 16) | (network[DEFAULT_NETWORK].ucOurMAC[2] << 8) | (network[DEFAULT_NETWORK].ucOurMAC[3])); // set the MAC address
    PAUR = ((network[DEFAULT_NETWORK].ucOurMAC[4] << 24) | (network[DEFAULT_NETWORK].ucOurMAC[5] << 16));
    #else
    PALR = ((pars->ucMAC[0] << 24) | (pars->ucMAC[1] << 16) | (pars->ucMAC[2] << 8) | (pars->ucMAC[3])); // set the MAC address
    PAUR = ((pars->ucMAC[4] << 24) | (pars->ucMAC[5] << 16));
    #endif
    #if defined _WINDOWS
    EIR = 0;
    if ((PALR & 0x01000000) != 0) {                                      // {29}
        _EXCEPTION("Check that the own MAC address doesn't set the multicast bit!!");
    }
    #endif
    OPD = PAUSE_DURATION;
    if (usRxSize > 1514) {
        usRxSize = 1514;                                                 // maximum frame size supported (without CRC-32)
    }
    #if defined SUPPORT_VLAN
    usRxSize += 4;                                                       // increase by 4 to allow for reception of a VLAN tag
    #endif
    #if defined ETHERNET_RMII    
    RCR = ((usRxSize << 16) | FEC_MII_MODE | CRCFWD | RMII_MODE);        // set max frame size to be received and RMII mode - don't pass received CRC to use
    #else
    RCR = ((usRxSize << 16) | FEC_MII_MODE | CRCFWD);                    // set max frame size to be received and MII mode - don't pass received CRC to use
    #endif
    #if defined TEST_CRC_32_IEEE                                         // test requires CRC_32 to be copied to the input buffer
    RCR &= ~(CRCFWD);
    usRxSize += 4;                                                       // ensure space for the CRC-32 in the buffer
    #else
    if ((pars->usMode & PASS_CRC32_RX) != 0) {                           // {13}
        RCR &= ~(CRCFWD);                                                // enable CRC-32 value to be stored in the reception buffer
        usRxSize += 4;                                                   // ensure space for the CRC-32 in the buffer
    }
    #endif

    if ((pars->usMode & PROMISCUOUS) != 0) {
        RCR |= PROM;                                                     // no destination address checking
    }
    else {
    #if defined USE_IPV6
        if (pars->usMode & CON_MULTICAST) {                              // if multicast reception is to be configured
            fnConfigureMulticastIPV6();                                  // set IPV6 multicast address
        }
    #endif
        if ((pars->usMode & NO_BROADCAST) != 0) {
            RCR |= BC_REJ;                                               // reject broadcast frames
        }
    }

    if ((pars->usMode & FULL_DUPLEX) != 0) {                             // define network control
        TCR = FDEN;
        if ((pars->usMode & RX_FLOW_CONTROL) != 0) {
            RCR |= FCE;                                                  // uses pause flow control
        }
    }
    else {
        TCR = 0;
        RCR |= DRT;                                                      // half duplex
    }

    if ((pars->usMode & LOOPBACK) != 0) {                                // optional Loopback for testing
        RCR |= LOOP;
    }

    MIBC = (MIB_DISABLE | MIB_CLEAR);                                    // ensure MIB is disabled while resetting and command clear to reset all counters in the maintenance block RAM                               // 
    MIBC = 0;

    #if defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
    ENET_TACC = (TACC_IPCHK | TACC_PROCHK);
    ENET_TFWR = (TFWR_STRFWD);                                           // enable store and forward so that the check sum accelerator can be used on transmission
    #elif defined IP_TX_CHECKSUM_OFFLOAD
    ENET_TACC = (TACC_IPCHK);
    ENET_TFWR = (TFWR_STRFWD);                                           // enable store and forward so that the check sum accelerator can be used on transmission
    #elif defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
    ENET_TACC = (TACC_PROCHK);
    ENET_TFWR = TFWR_STRFWD;                                             // enable store and forward so that the check sum accelerator can be used on transmission
    #else
    ENET_TFWR = (TFWR_TFWR_4032);                                        // don't start sending Ethernet frame until FIFO has been filled to ensure no Tx under-run is possible
    #endif
    #if defined IP_RX_CHECKSUM_OFFLOAD
    ENET_RACC = (RACC_IPDIS | RACC_PRODIS/* | RACC_LINEDIS*/);           // automatically discard frames with MAC, IP header or IP protocol errors
    #else
  //ENET_RACC = (RACC_LINEDIS);                                          // automatically discard reception frames with MAC errors
    #endif

    #if defined SCAN_PHY_ADD || defined POLL_PHY
    i = 0;
    do {                                                                 // check that the external PHY is responding correctly
    #endif
        ulPhyIdentifier = fnMIIread(PHY_ADDRESS, PHY_REG_ID1);           // check that the PHY is working correctly by reading its identifier - part 1
        ulPhyIdentifier <<= 16;
        ulPhyIdentifier |= fnMIIread(PHY_ADDRESS, PHY_REG_ID2);          // check that the PHY is working correctly by reading its identifier - part 2

        if ((ulPhyIdentifier & PHY_MASK) != (PHY_IDENTIFIER & PHY_MASK)) {
    #if defined SCAN_PHY_ADD || defined POLL_PHY
        #if defined SCAN_PHY_ADD
            PHY_ADDRESS++;
            if (PHY_ADDRESS > 31)                                        // all possible PHY addresses have been checked but no match found
        #else
            if (++i > POLL_PHY)
        #endif
            {
    #endif
    #if defined ALLOW_PHY_ERROR
        #if defined SCAN_PHY_ADD || defined POLL_PHY
                break;
        #endif
    #else
                MSCR = 0;                                                // disable clock
                POWER_DOWN(2, SIM_SCGC2_ENET);
                return -1;                                               // if the identifier is incorrect the phy isn't responding correctly - return error
    #endif
    #if defined SCAN_PHY_ADD || defined POLL_PHY
            }
    #endif
        }
    #if defined SCAN_PHY_ADD || defined POLL_PHY
        else {
            break;                                                       // the present PHY address has resulted in a match so use this address
        }
    } while (1);
    #endif

    if ((usRxSize & 0x0f) != 0) {                                        // ensure size is suitable (divisible by 16)
        usRxSize &= ~0x0f;
        usRxSize += 0x10;
    }
    #if defined USE_MULTIPLE_BUFFERS
    EMRBR = usEthernetLength = 256;                                      // use small multiple buffers for reception
    ucMultiBuf = ((usRxSize + 128)/256);
    ptrRxBd = ptrRxBd_start = ptrBD = (KINETIS_FEC_BD *)ETH_BD_MALLOC_ALIGN((sizeof(KINETIS_FEC_BD) * NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * ucMultiBuf), 16); // define memory for rx buffer descriptors (aligned on 16 byte boundary)
    #else
    EMRBR = usEthernetLength = usRxSize;                                 // set receive buffer size
    ptrRxBd = ptrRxBd_start = ptrBD = (KINETIS_FEC_BD *)ETH_BD_MALLOC_ALIGN((sizeof(KINETIS_FEC_BD) * NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 16); // define memory for rx buffer descriptors (aligned on 16 byte boundary)
    #endif
    ERDSR = (unsigned long)ptrBD;                                        // set pointer to receive descriptor ring
    #if defined USE_MULTIPLE_BUFFERS
    ptrBuffer = ETH_RX_BUF_MALLOC_ALIGN((ucMultiBuf * 256 * NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 16); // get rx buffer memory
    for (i = 0; i < (NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * ucMultiBuf); i++) { // initialise each Rx buffer descriptor
        if (i == ((NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * ucMultiBuf) - 1)) {
            ptrBD->usBDControl = (EMPTY_BUFFER | WRAP_BIT_RX);
        }
        else {
            ptrBD->usBDControl = EMPTY_BUFFER;
        }
        #if defined EMAC_ENHANCED
        ptrBD->ulBDControlEnhanced = RX_GEN_INTERRUPT;
        #endif
        ptrBD->ptrBD_Data = fnLE_ENET_add(ptrBuffer);
        ptrBuffer += 256;
        ptrBD++;
    }
    #else
    ptrBuffer = (unsigned char *)ETH_RX_BUF_MALLOC_ALIGN((usRxSize * NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 16); // get rx buffer memory
    for (i = 0; i < NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE; i++) {      // initialise each Rx buffer descriptor
        if (i == (NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE - 1)) {
            ptrBD->usBDControl = (EMPTY_BUFFER | WRAP_BIT_RX);
        }
        else {
            ptrBD->usBDControl = EMPTY_BUFFER;
        }
        #if defined EMAC_ENHANCED
        ptrBD->ulBDControlEnhanced = RX_GEN_INTERRUPT;
        #endif
        ptrBD->ptrBD_Data = fnLE_ENET_add(ptrBuffer);
        ptrBuffer += usRxSize;
        ptrBD++;
    }
    #endif

    if ((pars->usSizeTx & 0x07) != 0) {                                  // ensure size is suitable (divisible by 8)
        pars->usSizeTx &= ~0x07;
        pars->usSizeTx += 0x08;                                          // when not, round-up to next length divisible by 8
    }
    ptrTxBd = ptrTxBd_start = ptrBD = (KINETIS_FEC_BD *)ETH_BD_MALLOC_ALIGN((sizeof(KINETIS_FEC_BD) * NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE), 16);
    ETDSR = (unsigned long)ptrBD;
    ptrBuffer = (unsigned char *)ETH_TX_BUF_MALLOC_ALIGN((pars->usSizeTx * NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE), 8); // get tx buffer memory
    for (i = 0; i < NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE; i++) {
        if (i == (NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE - 1)) {
            ptrBD->usBDControl = (WRAP_BIT_TX);
        }
        else {
            ptrBD->usBDControl = 0;
        }
    #if defined EMAC_ENHANCED
        #if defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
        ptrBD->ulBDControlEnhanced = (TX_GENERATE_INT | TX_INSERT_PROT_CS | TX_INSERT_IP_CS);
        #elif defined IP_TX_CHECKSUM_OFFLOAD
        ptrBD->ulBDControlEnhanced = (TX_GENERATE_INT | TX_INSERT_IP_CS);
        #elif defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
        ptrBD->ulBDControlEnhanced = (TX_GENERATE_INT | TX_INSERT_PROT_CS);
        #else
        ptrBD->ulBDControlEnhanced = (TX_GENERATE_INT);
        #endif
        #if defined EMAC_IEEE1588
        ptrBD->ulBDControlEnhanced |= TX_ADD_TIMESTAMP;                  // enable time stamping of transmitted frames
        #endif
    #endif
        ptrBD->ptrBD_Data = fnLE_ENET_add(ptrBuffer);
        ptrBuffer += pars->usSizeTx;
        ptrBD++;
    }
    fnEnterInterrupt(irq_ETH_RX_ID, PRIORITY_EMAC, _fec_rx_frame_isr);   // configure and enter the Ethernet rx handling interrupt routine in the vector table
    #if defined LAN_REPORT_ACTIVITY || defined LAN_TX_FPU_WORKAROUND     // {12}
    fnEnterInterrupt(irq_ETH_TX_ID, PRIORITY_EMAC, _fec_txf_isr);        // configure and enter the Ethernet tx handling interrupt routine in the vector table
    #endif
    fnEnterInterrupt(irq_ETH_ERR_MISC_ID, PRIORITY_EMAC, _fec_misc);     // configure and enter the Ethernet error and miscellaneous handling interrupt routine in the vector table

    ECR = ENABLE_ETHERNET_OPERATION;                                     // enable FEC for operation
    #if defined _KSZ8051RNL && defined ETHERNET_RMII                     // {3}
    usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_CTRL2);                   // get KSZ8051RNL control 2 register value
    usMIIData |= KSZ8051_CTRL2_RMII_50MHZ_CLK;                           // select 50MHz output clock mode (rather than the default 25MHz)
    fnMIIwrite(PHY_ADDRESS, PHY_REG_CTRL2, usMIIData);                   // write back new value
    #endif
    #if !defined _PHY_KSZ8863                                            // {43}
    if ((pars->usMode & (LAN_10M | LAN_100M)) == 0) {                    // when in auto-negotiation mode
        usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_ANAR);                // get auto neg. mode supported by PHY
        if ((pars->usMode & RX_FLOW_CONTROL) != 0) {
            usMIIData |= PHY_R4_FC;                                      // set flow control
        }
        fnMIIwrite(PHY_ADDRESS, PHY_REG_ANAR, usMIIData);                // write PHY_REG_ANAR to configure PHY Link abilities advertisement
    }
    #endif
    #if defined PHY_INTERRUPT
    fnMIIwrite(PHY_ADDRESS, KS8041_INTERRUPT_CONTROL_STATUS_REGISTER,    // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
               );
    #endif
    usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_CR);                      // read PHY Control Register 0

    if ((pars->usMode & (LAN_10M | LAN_100M)) == 0) {
        usMIIData |= (PHY_R0_ANE | PHY_R0_RAN);                          // configure and start auto-negotiation
    }
    else {
        usMIIData &= ~(PHY_R0_ANE | PHY_R0_RAN);                         // ensure that the PHY doesn't perform auto-negotiation
        if ((pars->usMode & FULL_DUPLEX) != 0) {
            usMIIData |= PHY_R0_DPLX;                                    // set full duplex operation
        }
        else {
            usMIIData &= ~PHY_R0_DPLX;                                   // set half-duplex operation
        }

        if ((pars->usMode & LAN_100M) != 0) {
            usMIIData |= PHY_R0_DR;                                      // set manual 100Mb
        }
        else {
            usMIIData &= ~PHY_R0_DR;                                     // set manual 10Mb
    #if defined ETHERNET_RMII
            RCR |= RMII_10T;
    #endif
        }
    }
    fnMIIwrite(PHY_ADDRESS, PHY_REG_CR, usMIIData);                      // command initial operating mode of PHY
    #if defined _PHY_KSZ8863                                             // {43}
    fnMIIwrite(PHY_ADDRESS_2, PHY_REG_CR, usMIIData);                    // command initial operating mode of PHY (second port)
    #endif
    #if defined PHY_INTERRUPT                                            // enable PHY interrupt
    fnMIIwrite(PHY_ADDRESS, KS8041_INTERRUPT_CONTROL_STATUS_REGISTER,    // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
               );
    interrupt_setup.int_type       = PORT_INTERRUPT;                     // identifier to configure port interrupt
    interrupt_setup.int_handler    = fnPhyInterrupt;                     // handling function
    interrupt_setup.int_priority   = PRIORITY_PHY_PORT_INT;              // interrupt priority level
    interrupt_setup.int_port       = PHY_INTERRUPT_PORT;                 // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = PHY_INTERRUPT;                      // the IRQ input connected
    interrupt_setup.int_port_sense = (IRQ_LOW_LEVEL | PULLUP_ON);        // interrupt is to be low level sensitive
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt
    #endif
    RDAR = 0;                                                            // enable RX BD polling
    #if defined STOP_MII_CLOCK
    MSCR = 0;                                                            // disable clock
    #endif
    return 0;                                                            // configuration successful
}


#if defined _WINDOWS
  //#if !defined USE_IP
  //const unsigned char cucBroadcast[MAC_LENGTH] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; // used also for broadcast IP
  //#endif

    #if defined USE_IPV6 || defined USE_IGMP
static int fnIsHashMulticast(unsigned char *ucData)
{
    unsigned long ulCRC32 = 0xffffffff;
    int i, bit;
    unsigned char ucByte;
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
    ulCRC32 >>= 26;                                                      // the upper 6 bits of the CRC32 detemine the hash entry location

    if (ulCRC32 < 32) {
        if (GALR & (0x00000001 << ulCRC32)) {
            return 1;                                                    // multicast hash match
        }
    }
    else {
        ulCRC32 -= 32;
        if (GAUR & (0x00000001 << ulCRC32)) {
            return 1;                                                    // multicast hash match
        }
    }
    return 0;                                                            // not a match
}
    #endif

    // Check whether a unicast address matches
    //
    static int fnIsOurMAC(unsigned char *ucData)
    {
        if ((*ucData == (unsigned char)(PALR >> 24)) && (*(ucData + 1) == (unsigned char)(PALR >> 16)) && (*(ucData + 2) == (unsigned char)(PALR >> 8)) && (*(ucData + 3) == (unsigned char)(PALR)) &&
            (*(ucData + 4) == (unsigned char)(PAUR >> 24)) && (*(ucData + 5) == (unsigned char)(PAUR >> 16))) {
            return 1;
        }
        return 0;
    }

    // This routine is supplied to aid the simulator. It checks whether Ethereal frames should be accepted based
    // on the programmed MAC address and the Ethernet operating mode
    //
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen)
    {
        // This function checks to see whether we can receive the simulated frame and can block it if wanted
        //
        if (usLen < ((2 * MAC_LENGTH) + 2)) {
            return 0;                                                    // too short to be a valid Ethernet frame
        }
    #if !defined _DEVELOP_SIM
        if (fnIsOurMAC(&ucData[MAC_LENGTH]) != 0) {
            return 0;                                                    // we never accept echos of our own transmissions - check before broadcast
        }
    #endif
        if (uMemcmp(ucData, cucBroadcast, MAC_LENGTH) == 0) {
            return 1;                                                    // broadcast
        }
        if ((RCR & PROM) != 0) {
            return 1;                                                    // in promiscuous mode we accept everything
        }
        if (fnIsOurMAC(&ucData[0]) != 0) {
            return 1;                                                    // we accept our mac address
        }
    #if defined USE_IPV6 || defined USE_IGMP
        if (fnIsHashMulticast(&ucData[0]) != 0) {
            return 1;                                                    // multicast hash match
        }
    #endif
        return 0;                                                        // otherwise ignore
    }
#endif

// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
extern int fnWaitTxFree(void)
{
    volatile int iWait = 5000;                                           // wait until the transmitter is free
    while ((ptrTxBd->usBDControl & READY_TX) != 0) {
        if (--iWait == 0) {
            return 1;                                                    // we have waited long enough - give up...
        }
    }
    return 0;
}

extern void fnPutInBuffer(unsigned char *ptrOut, unsigned char *ptrIn, QUEUE_TRANSFER nr_of_bytes)
{
    uMemcpy(ptrOut, ptrIn, nr_of_bytes);                                 // the Ethernet output buffer is linear so can be copied to using uMemcpy()
}

// This can be called by the ethernet driver to find out how many receive buffers the Ethernet Controller has
//
extern int fnGetQuantityRxBuf(void)
{
    return 1;
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return 0;
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    return fnLE_ENET_add(ptrTxBd->ptrBD_Data);
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been used
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
    #if defined USE_MULTIPLE_BUFFERS
    if (iBufNr >= 0) {
        return;                                                          // when using multi-small buffers all processing is internal
    }
    #endif
    ptrRxBd->usBDLength = 0;                                             // the buffer has been completely used so free it for use by the Ethernet controller
    ptrRxBd->usBDControl |= EMPTY_BUFFER;                                // free the buffer
    #if defined _WINDOWS
    ptrRxBd->usBDControl &= ~(LAST_IN_FRAME_RX | TRUNCATED_FRAME | OVERRUN_FRAME);
    #endif
    if ((ptrRxBd->usBDControl & WRAP_BIT_RX) != 0) {                     // handle wrap
        ptrRxBd = (KINETIS_FEC_BD *)ERDSR;                               // set to first BD
    }
    else {
        ptrRxBd++;                                                       // increment to next buffer
    }
    RDAR = 0;                                                            // re-enable buffer operation
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
    #if defined PHY_TAIL_TAGGING                                         // {44}
    if (iTailTagging != 0) {
        while (DataLen < 60) {                                           // when tail tagging we need to fill out short frames
            *ptr_put++ = 0x00;                                           // pad with zeros if smaller than 60 [chip must send at least 60]
            DataLen++;
        }
        #if !defined _WINDOWS
        ptrTxBd->usBDLength = fnLE_ENET_word(DataLen + 1);               // add tag to tail to specify which port to transmit over
        #endif
        *ptr_put = ucTailTagPort;                                        // specify port for the frame to be output to
    }
    else {
        ptrTxBd->usBDLength = fnLE_ENET_word(DataLen);                   // mark length of data to send (Kinetis automatically zero-pads transmission to minimum length)
    }
    #else
    ptrTxBd->usBDLength = fnLE_ENET_word(DataLen);                       // mark length of data to send (Kinetis automatically zero-pads transmission to minimum length)
    #endif
    #if defined EMAC_ENHANCED && defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD && defined IPV4_SUPPORT_TX_FRAGMENTATION
    if (iFragmentTx == 0) {
        ptrTxBd->ulBDControlEnhanced |= (TX_INSERT_PROT_CS);             // ensure that payload checkum offloading is enabled for this frame
    }
    else {
        ptrTxBd->ulBDControlEnhanced &= ~(TX_INSERT_PROT_CS);            // disable payload checkum offloading for this frame
    }
    #endif
    ptrTxBd->usBDControl |= (READY_TX | LAST_IN_FRAME_TX | TX_CRC);      // start transmission
    #if defined EMAC_ENHANCED
        #if defined LAN_REPORT_ACTIVITY || defined LAN_TX_FPU_WORKAROUND // {12}
    ptrTxBd->ulBDU = TX_GENERATE_INT;
        #else
    ptrTxBd->ulBDU = 0;
        #endif
    #endif
    TDAR = TDAR_TDAR;                                                    // enable TX BD operation to send the new content

    #if defined _WINDOWS
    fnSimulateEthTx(DataLen, fnGetSimTxBufferAdd());
        #if defined PSEUDO_LOOPBACK                                      // if we detect an IP frame being sent to our own address we loop it back to the input
    if (((*(ptr_put - DataLen + 12)) == 0x08) && (!(uMemcmp(&network[DEFAULT_NETWORK].ucOurIP[0], (ptr_put - DataLen + 26), IPV4_LENGTH)))) {
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
        #endif
    fec_txf_isr();                                                       // simulate end of transmission - tx interrupt
    ptrTxBd->usBDControl &= ~READY_TX;                                   // free buffer for next transmission
    #endif

    if ((ptrTxBd->usBDControl & WRAP_BIT_TX) != 0) {                     // prepare local pointer for next write
        ptrTxBd = (KINETIS_FEC_BD *)ETDSR;
    }
    else {
        ptrTxBd++;
    }
    return (DataLen);
}
