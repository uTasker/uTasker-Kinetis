/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      NetworkIndicator.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    20.01.2009 Add debug message for link up/down                        {1}
    16.12.2012 Add PHY_POLL_LINK support (this version doesn't operate together with LAN_REPORT_ACTIVITY) {2}
    16.12.2012 Add duplex information                                    {3}
    08.07.2013 Add ELZET80 device discovery support                      {4}
    02.06.2014 Allow task to be used with define INTERRUPT_TASK_PHY      {5}
    06.12.2015 Report link state changes using fnEthernetStateChange()   {6}
    06.12.2015 Report link up and down to USB task when RNDIS is used    {7}

*/ 

#include "config.h"


/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if defined LAN_REPORT_ACTIVITY || defined PHY_POLL_LINK || defined INTERRUPT_TASK_PHY // {2}{5}

#define OWN_TASK               TASK_NETWORK_INDICATOR

#define COLLISION_TIME         (2 * SEC)
#define TX_TIME                (0.05 * SEC)
#define RX_TIME                (0.1 * SEC)
#define PHY_POLL_TIME          (2 * SEC)                                 // {2}

#define E_TIMEOUT              1

#define LINK_LED_ON            0x01
#define COL_LED_ON             0x02
#define TX_LED_OFF             0x04
#define RX_LED_OFF             0x08


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */


extern void fnNetworkIndicator(TTASKTABLE *ptrTaskTable)                 // task
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[HEADER_LENGTH];                         // reserve space for receiving messages
    static unsigned char ucState = 0;                                    // state of the Ethernet connection leds

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
            if (E_TIMEOUT == ucInputMessage[MSG_TIMER_EVENT]) {          // LED blink period expired
    #if defined LAN_REPORT_ACTIVITY
                ACTIVITY_LED_OFF();                                      // turn off the activity LED
                ucState &= ~(TX_LED_OFF | RX_LED_OFF | COL_LED_ON);
    #endif
    #if defined PHY_POLL_LINK && !defined NO_INTERNAL_ETHERNET
                fnCheckEthLinkState();                                   // check the PHY link state (synchronising the EMAC if required)
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(PHY_POLL_TIME), E_TIMEOUT); // start timer for next link state poll
    #endif
            }
            break;

        case INTERRUPT_EVENT:
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {
    #if defined LAN_REPORT_ACTIVITY
            case EMAC_RX_INTERRUPT:
            case EMAC_TX_INTERRUPT:
                if (ucState & (TX_LED_OFF | RX_LED_OFF)) {
                    break;                                               // don't disturb if already blinking
                }
                ACTIVITY_LED_ON();                                       // turn on activity LED (for a short time)

                if (ucInputMessage[MSG_INTERRUPT_EVENT] == EMAC_RX_INTERRUPT) {
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(RX_TIME), E_TIMEOUT); // blink activity LED
                    ucState |= RX_LED_OFF;
                }
                else {
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(TX_TIME), E_TIMEOUT); // blink activity LED
                    ucState |= TX_LED_OFF;
                }
                break;
    #endif
    #if defined INTERRUPT_TASK_PHY
            case LAN_LINK_UP_100_FD:                                     // {3} link has gone up
            case LAN_LINK_UP_100:
        #if defined LAN_REPORT_ACTIVITY
                SPEED_LED_ON();                                          // show link up with 100M
        #endif
            case LAN_LINK_UP_10_FD:
            case LAN_LINK_UP_10:
        #if defined LAN_REPORT_ACTIVITY
                if ((ucInputMessage[MSG_INTERRUPT_EVENT] == LAN_LINK_UP_10_FD) || ((ucInputMessage[MSG_INTERRUPT_EVENT] == LAN_LINK_UP_10_FD))) {
                    SPEED_LED_OFF();                                     // show link up with 10M
                }
        #endif
                if ((ucState & LINK_LED_ON) == 0) {                      // if the previous state was link down
                    ucState = LINK_LED_ON;                               // new state is link up
                    fnEthernetStateChange(DEFAULT_IP_INTERFACE, ucInputMessage[MSG_INTERRUPT_EVENT]); // {6}
        #if defined LAN_REPORT_ACTIVITY
                    TURN_LINK_LED_ON();
        #endif
        #if defined USB_CDC_RNDIS
                    fnInterruptMessage(TASK_USB, LAN_LINK_UP_100_FD);    // {7} report link-up type and the RNDIS task can consult the Ethernet task in case it needs to know exact details
        #endif
                }
                break;
    #endif

            case LAN_LINK_DOWN:                                          // link has gone down
    #if defined LAN_REPORT_ACTIVITY
                LINK_DOWN_LEDS();
    #endif
    #if defined INTERRUPT_TASK_PHY
                if ((ucState & LINK_LED_ON) != 0) {                      // if the previous state was link up
                    ucState &= ~LINK_LED_ON;                             // new state is link down
                    fnEthernetStateChange(DEFAULT_IP_INTERFACE, ucInputMessage[MSG_INTERRUPT_EVENT]); // {6}
        #if defined USB_CDC_RNDIS
                    fnInterruptMessage(TASK_USB, LAN_LINK_DOWN);         // {7}
        #endif
                }
    #endif
    #if defined PHY_POLL_LINK
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(PHY_POLL_TIME), E_TIMEOUT); // start polling the state of the link               
    #endif
                break;

            case ETHERNET_COLLISION:
                break;
            }
            break;

        default:
    #ifdef SUPPORT_FLUSH
            fnFlush(PortIDInternal, FLUSH_RX);                           // flush unexpected messages
    #endif
            break;
        }
    }
}
#endif

#if defined ELZET80_DEVICE_DISCOVERY                                     //  {4}

#define DISCOVERY_REV_MAIN      1                                        // version number of discovery code
#define DISCOVERY_REV_SUB       7


#if defined BOOTLOADER
    #define DISCOVERY_IDENTIFICATION 'B'                                 // A for Application, B for Bootloader, C for Configbox
#else
    #define DISCOVERY_IDENTIFICATION 'A'                                 // A for Application, B for Bootloader, C for Configbox
#endif

#define INFOREQCMD             'I'
#define PARSETCMD              'S'

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
typedef struct _PACK stINFO_DATA {
    CHAR elzet[5];                                                       // discovery recognition string
    unsigned char id;                                                    // identity
    unsigned char ip[IPV4_LENGTH];                                       // IPv4 address
    unsigned char sub[IPV4_LENGTH];                                      // IPv4 subnet mask
    unsigned char dhcp;
    unsigned char mac[MAC_LENGTH];                                       // mac address
    CHAR ser[9];
    CHAR ver[28];
    unsigned char bcc;
} INFO_DATA;

typedef struct _PACK stSETTINGS_DATA {
    CHAR elzet[5];                                                       // discovery recognition string
    unsigned char id;                                                    // identity
    unsigned char ip[IPV4_LENGTH];                                       // IPv4 address
    unsigned char sub[IPV4_LENGTH];                                      // IPv4 subnet mask
    unsigned char dhcp;
    unsigned char bcc;
} SETTINGS_DATA;
__PACK_OFF

typedef struct stUDP_INFO_MESSAGE {
    UDP_HEADER tUDP_Header;                                              // reserve header space
    INFO_DATA tUDP_Data;
} UDP_INFO_MESSAGE;

static USOCKET MyUDP_Socket = -1;                                        // socket not yet allocated


// Parse a command containing configuration information
//
static int fnParseSettings(unsigned char *ucSettings)
{
    int bcc = 0;
    int j;
    SETTINGS_DATA settings;

    uMemcpy(&settings, ucSettings, 16);

    // Checksum
    //
    bcc = ucSettings[0];

    for (j = 1; j < 15; j++) {
        bcc = (bcc ^ ucSettings[j]);                                     // calculate BCC 
    }

    if (bcc == settings.bcc) {                                           // as long as the checksum is correct
        uMemcpy(network[DEFAULT_NETWORK].ucOurIP, settings.ip, IPV4_LENGTH); // set the IPv4 address and sub-net mask
        uMemcpy(network[DEFAULT_NETWORK].ucNetMask, settings.sub, IPV4_LENGTH);

    #if !defined BOOTLOADER && defined USE_MAINTENANCE
        if (settings.dhcp != 0) {
            temp_pars->temp_parameters.usServers |= ACTIVE_DHCP;         // enable DHCP
            fnStartDHCP((UTASK_TASK)(FORCE_INIT | TASK_APPLICATION), 0);
        }
        else {
            temp_pars->temp_parameters.usServers &= ~ACTIVE_DHCP;        // disable DHCP
            fnDeleteArp();                                               // ensure our ARP table is empty
            fnGetIP_ARP(network[DEFAULT_NETWORK].ucOurIP, TASK_APPLICATION, -1); // cause a ping of our own address and wait for the result
        }
        #if defined USE_PARAMETER_BLOCK
            #if defined USE_PARAMETER_BLOCK
        fnDelPar(SWAP_PARAMETER_BLOCK);
            #endif
        fnSaveNewPars(0);
        #endif
    #endif
        return 1;                                                        // settings accepted
    }
    return 0;                                                            // not accepted
}

// Return discovery details
//
static int fnSendInfo(void)
{
    UDP_INFO_MESSAGE tUDP_Message;                                       // temporary message space
    uMemset(&tUDP_Message.tUDP_Data, 0x00, sizeof(tUDP_Message.tUDP_Data)); // ensure data is zeroed
    uMemcpy(&tUDP_Message.tUDP_Data.elzet[0], "ELZET", sizeof(tUDP_Message.tUDP_Data.elzet)); // add recognition string
    tUDP_Message.tUDP_Data.id = DISCOVERY_IDENTIFICATION;

    uMemcpy(tUDP_Message.tUDP_Data.ip, network[DEFAULT_NETWORK].ucOurIP, IPV4_LENGTH); // add our IPv4 address
    uMemcpy(tUDP_Message.tUDP_Data.sub, network[DEFAULT_NETWORK].ucNetMask, IPV4_LENGTH); // add our IPv4 subnet mask

    #if defined BOOTLOADER 
    tUDP_Message.tUDP_Data.dhcp = 1;                                     // boot loader always uses DHCP
    #else		
    if (temp_pars->temp_parameters.usServers & ACTIVE_DHCP) {
        tUDP_Message.tUDP_Data.dhcp = 1;                                 // DHCP is in use
    }
    else {
        tUDP_Message.tUDP_Data.dhcp = 0;                                 // DHCP is not in use
    }
    #endif

    uMemcpy(tUDP_Message.tUDP_Data.mac, network[DEFAULT_NETWORK].ucOurMAC, sizeof(tUDP_Message.tUDP_Data.mac)); // add our MAC address

    fnAddDiscoverySerialNumber(&tUDP_Message.tUDP_Data.ser[0], sizeof(tUDP_Message.tUDP_Data.ser)); // add a serial number to the discovery response

    #if defined BOOTLOADER                                               // add version number
    uMemcpy(tUDP_Message.tUDP_Data.ver, &SOFTWARE_VERSION, sizeof(SOFTWARE_VERSION));
    #else	
    tUDP_Message.tUDP_Data.ver[0] = 'V';
    tUDP_Message.tUDP_Data.ver[1] = DISCOVERY_REV_MAIN + '0';
    tUDP_Message.tUDP_Data.ver[2] = '.';
    tUDP_Message.tUDP_Data.ver[3] = DISCOVERY_REV_SUB + '0';
    tUDP_Message.tUDP_Data.ver[4] = 0;
    #endif

    fnSendUDP(MyUDP_Socket, (unsigned char *) cucBroadcast, ELZET80_DISCOVERY_UDP_PORT, (unsigned char *)&tUDP_Message.tUDP_Header, sizeof(tUDP_Message.tUDP_Data), 0);
    return 1;
}

// UDP listener - handles reception frames
//
static int fnReceiveUDP(USOCKET c, unsigned char uc, unsigned char *ucIP, unsigned short us, unsigned char *data, unsigned short us2)
{
    if (data[0] == 'E' && data[1] == 'L' && data[2] == 'Z' && data[3] == 'E' && data[4] == 'T') { // check that the frame's recognition string is correct
        switch(data[5]){                                                 // check the ID
        case INFOREQCMD:
            return fnSendInfo();                                         // send back a frame containing discovery information

        case PARSETCMD:
            return fnParseSettings(data);                                // parse received configuration command
        }
    }
    return 0;                                                            // frame content was not recognised
}

// Listen on UDP port for discovery requests
//
extern void fnSetupUDPSocket(void)
{
    if (MyUDP_Socket < 0) {                                              // socket not yet allocated
        MyUDP_Socket = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnReceiveUDP, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS));
        fnBindSocket(MyUDP_Socket, ELZET80_DISCOVERY_UDP_PORT);
    }
}
#endif
