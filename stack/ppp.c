/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    --------------------------------------------------------------------
    File:      PPP.c
    Project:   Single Chip Embedded Internet
    --------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    ********************************************************************/

/***********************************************************************/
/***********************************************************************/
/*******                                                        ********/
/*******     WARNING: This file is in development !!!!          ********/
/*******     It is included in the present service pack         ********/
/*******     so that you can use it for first tests or          ********/
/*******     you can further develop it for your own needs      ********/
/*******     or in collaboration with myself.                   ********/
/*******     To learn more about the state of development       ********/
/*******     or if you have any questions or suggestions        ********/
/*******     please email me at Mark@uTasker.com                ********/
/***********************************************************************/
/***********************************************************************/

#include "config.h"


#ifdef USE_PPP

#define OWN_TASK TASK_PPP

#define PPP_RX_BUFFER_SIZE   128                                         // space in driver rx buffer
#define PPP_TX_BUFFER_SIZE   2048                                        // space in driver tx buffer
#define PPP_RX_BUFFER_SPACE  2028                                        // local buffer space

#define PPP_STATE_INIT       0
#define PPP_STATE_IDLE       1
#define PPP_STATE_CLIENT     2
#define PPP_STATE_IN_FRAME   3

#define PPP_INIT_CRC         0xffff
#define PPP_GOOD_CRC         0xf0b8

static int fnHandlePPP_frame(unsigned char *ptrFrame, QUEUE_TRANSFER ppp_frame_length);
static unsigned short fnPPP_crc(unsigned short crc_value, unsigned char ucData);

static int iPPP_State = PPP_STATE_INIT;
static QUEUE_HANDLE PPP_PortID;
static unsigned long ulACCM_bits = 0xffffffff;                           // default: escape all characters between 0x00 and 0x1f
static unsigned char ucMagicNumer[4];

static const unsigned char cClient[] = {'C', 'L', 'I', 'E', 'N', 'T', 'S', 'E', 'R', 'V', 'E', 'R'};



// PPP task
//
extern void fnPPP(TTASKTABLE *ptrTaskTable)
{
    static unsigned char   ucInputMessage[PPP_RX_BUFFER_SPACE];          // reserve space for receiving messages
    static QUEUE_TRANSFER  ppp_frame_length = 0;                         // collected frame size
    static int             iRxEscape = 0;
    static unsigned short  crc_value;
  //QUEUE_HANDLE           PortIDInternal = ptrTaskTable->TaskID;        // queue ID for task input

    if (iPPP_State == PPP_STATE_INIT) {
        TTYTABLE tInterfaceParameters;                                   // table for passing information to driver

        tInterfaceParameters.Channel = PPP_UART;                         // set UART channel for serial use
        tInterfaceParameters.ucSpeed = SERIAL_BAUD_19200;                // baud rate
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = PPP_RX_BUFFER_SIZE; // input buffer size
        tInterfaceParameters.Rx_tx_sizes.TxQueueSize = PPP_TX_BUFFER_SIZE; // output buffer size
        tInterfaceParameters.Task_to_wake = OWN_TASK;                    // wake self when messages have been received
        #ifdef SUPPORT_FLOW_HIGH_LOW
        tInterfaceParameters.ucFlowHighWater = 80;                       // set the flow control high and low water levels in %
        tInterfaceParameters.ucFlowLowWater = 20;
        #endif
        tInterfaceParameters.Config = (CHAR_8 + NO_PARITY + ONE_STOP /*+ USE_XON_OFF*/ + CHAR_MODE);
        #ifdef SERIAL_SUPPORT_DMA
            tInterfaceParameters.ucDMAConfig = 0;
        #endif
        if ((PPP_PortID = fnOpen(TYPE_TTY, FOR_I_O, &tInterfaceParameters)) != NO_ID_ALLOCATED) { // open serial port with defined parameters
            fnDriver(PPP_PortID, (TX_ON | RX_ON), 0);                    // enable rx and tx
        }
        iPPP_State = PPP_STATE_IDLE;
    }

    while (fnRead(PPP_PortID, &ucInputMessage[ppp_frame_length], 1) != 0) { // while serial input waiting
        switch (iPPP_State) {
        case PPP_STATE_IDLE:                                             // waiting for dial in
            if (ucInputMessage[ppp_frame_length] == cClient[ppp_frame_length]) {
                if (++ppp_frame_length >= 6)  {
                    iPPP_State = PPP_STATE_CLIENT;
                    fnWrite(PPP_PortID, (unsigned char *)cClient, sizeof(cClient)); // answer with CLIENTSERVER
                }
                else {
                    break;
                }
            }
            ppp_frame_length = 0;
            break;

        case PPP_STATE_CLIENT:
            if (ucInputMessage[0] == 0x7e) {
                iPPP_State = PPP_STATE_IN_FRAME;
                crc_value = PPP_INIT_CRC;                                // prime crc value for frame checking
            }
            break;

        case PPP_STATE_IN_FRAME:
            if (ucInputMessage[ppp_frame_length] == 0x7e) {
                static int iFrameCnt = 0;
                if (++iFrameCnt == 0x0e) {
                    iFrameCnt = 0;
                }
                if (crc_value == PPP_GOOD_CRC) {                         // drop all frames with bad crc value
                    iPPP_State = fnHandlePPP_frame(ucInputMessage, ppp_frame_length);
                }
                ppp_frame_length = 0;
                break;
            }
            if (iRxEscape != 0) {                                        // escaping character
                ucInputMessage[ppp_frame_length] ^= 0x20;
                iRxEscape = 0;
            }
            else if (ucInputMessage[ppp_frame_length] == 0x7d) {
                iRxEscape = 1;
                break;
            }
            crc_value = fnPPP_crc(crc_value, ucInputMessage[ppp_frame_length]); // calculate CRC over each data byte
            if (++ppp_frame_length >= PPP_RX_BUFFER_SPACE) {             // check for over length frames
                iPPP_State = PPP_STATE_CLIENT;
                ppp_frame_length = 0;
            }
            break;

        }
    }
}

#define LCP_PROTOCOL       0xc021
#define PAP_PROTOCOL       0xc023
#define CHAP_PROTOCOL      0xc22305
#define MSCHAPV1_PROTOCOL  0xc22380
#define MSCHAPV2_PROTOCOL  0xc22381
#define EAP_PROTOCOL       0xc227
#define SPAP_PROTOCOL      0xc027
#define OLD_SPAP_PROTOCOL  0xc123
#define CCP_PROTOCOL       0x80fd
#define IPCP_PROTOCOL      0x8021
#define IPV4_PROTOCOL      0x0021


#define VENDOR_EXTENSION   0x00
#define CONFIG_REQUEST     0x01
#define CONFIG_ACK         0x02
#define CONFIG_NACK        0x03
#define CONFIG_REJECT      0x04
#define TERMINATE_REQUEST  0x05
#define TERMINATE_ACK      0x06
#define CODE_REJECT        0x07
#define PROTOCOL_REJECT    0x08
#define ECHO_REQUEST       0x09
#define ECHO_REPLY         0x0a
#define DISCARD_REQUEST    0x0b
#define IDENTIFICATION     0x0c
#define TIME_REMAINING     0x0d
#define RESET_REQUEST      0x0e
#define RESET_ACK          0x0f

// Options
#define ACCM               0x02                                          // Asynchronous Control Character Map
#define AUTH_PROT          0x03
#define QUALITY_PROT       0x04
#define MAGIC_NUMBER       0x05
#define PFC                0x07                                          // Protocol Field Compression
#define ACFC               0x08                                          // Address and Control Field Compression
#define CALLBACK           0x0d

#define IPCP               0x02                                          // IP compression protocol
#define IPIP               0x03                                          // IP address (0.0.0.0 if not known)
#define IPDNS              0x81                                          // Primary DNS address (0.0.0.0 if not known)
#define IPNBNS             0x82                                          // Primary NBNS address (0.0.0.0 if not known)
#define IPSDNS             0x83                                          // Secondary DNS address (0.0.0.0 if not known)
#define IPSNBNS            0x84                                          // Secondary NBNS address (0.0.0.0 if not known)

#define AUTHENTICATE_REQUEST 0x01
#define AUTHENTICATE_ACK     0x02
#define AUTHENTICATE_NAK     0x03

typedef struct st_ppp_info {
    unsigned char ucPPP_Code;
    unsigned char ucPPP_ID;
    unsigned char ucPPP_Length[2];
} PPP_INFO;

typedef struct st_ppp_option {
    unsigned char ucPPP_option;
    unsigned char ucPPP_option_length;
} PPP_OPTION;

static const unsigned char ucHDLC_Flag = 0x7e;
static const unsigned char ucHDLC_Add[] = {0xff, 0x03};

static unsigned short usTxCRC;

// Check whether this data byte has to be escaped
//
static int fnIsEscape(const unsigned char ucData)
{
    if ((ucData == 0x7e) || (ucData == 0x7d)) {
        return 1;
    }

    if (ucData <= 0x1f) {                                                // if the data byte is in the range 0..0x1f check whether it has to be escaped
        if ((ulACCM_bits >> ucData) & 0x01) {
            return 1;
        }
    }
    return 0;
}

// Write PPP data to the UART, calculating its check sum and escaping characters if necessary
//
static void fnWritePPP(const unsigned char *ptrData, QUEUE_TRANSFER length)
{
    const unsigned char *ptrTx = ptrData;
    QUEUE_TRANSFER txLength = 0;

    while (length != 0) {
        usTxCRC = fnPPP_crc(usTxCRC, *ptrData);                          // calculate the crc of the tx frame over each byte
        if (fnIsEscape(*ptrData)) {
            unsigned char ucStuffed[2];
            if (txLength != 0) {
                fnWrite(PPP_PortID, (unsigned char *)ptrTx, txLength);   // send waiting non-stuffed segments
            }
            ucStuffed[0] = 0x7d;
            ucStuffed[1] = (*ptrData ^ 0x20);
            fnWrite(PPP_PortID, ucStuffed, sizeof(ucStuffed));           // send stuffed character
            ptrData++;
            ptrTx = ptrData;
            txLength = 0;
        }
        else {
            ptrData++;
            txLength++;                                                  // collect the length without byte stuffing
        }
        length--;
    }
    if (txLength != 0) {
        fnWrite(PPP_PortID, (unsigned char *)ptrTx, txLength);           // send waiting non-stuffed segments
    }
}

static unsigned char ucID = 0x0;


static void fnReject(unsigned short usProtocol, unsigned char ucOption, unsigned char *option_data, unsigned char ucOptionLength)
{
    unsigned char ucProtocolCode[6 + 2]; // fixed test message
    unsigned char ucCRC_16[2];

    usTxCRC = PPP_INIT_CRC;                                              // prime tx crc value
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send start flag
    fnWritePPP(ucHDLC_Add, sizeof(ucHDLC_Add));                          // send address and control

    ucProtocolCode[0] = (unsigned char)(usProtocol>>8);
    ucProtocolCode[1] = (unsigned char)(usProtocol);
    ucProtocolCode[2] = CONFIG_REJECT;
    ucProtocolCode[3] = ucID++;
    ucProtocolCode[4] = 0;
    ucProtocolCode[5] = ucOptionLength + 4;

    ucProtocolCode[6] = ucOption;
    ucProtocolCode[7] = ucOptionLength;

    fnWritePPP(ucProtocolCode, sizeof(ucProtocolCode));                  // send protocol, code, ID and length
    ucOptionLength -= 2;
    if (ucOptionLength != 0) {
        fnWritePPP(option_data, ucOptionLength);
    }

    usTxCRC ^= 0xffff;
    ucCRC_16[0] = (unsigned char)(usTxCRC);
    ucCRC_16[1] = (unsigned char)(usTxCRC >> 8);
    fnWritePPP(ucCRC_16, sizeof(ucCRC_16));                              // add the check sum
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send end flag
}


// Test
//
static void fnNAK(unsigned short usProtocol, PPP_INFO *ppp_inf)
{
    unsigned short usOptionsLength = ppp_inf->ucPPP_Length[0];
    unsigned char ucCRC_16[2];
    unsigned char ucProtocolCode[2];

    ppp_inf->ucPPP_Code = CONFIG_NACK;                                    // convert into ack frame

    usTxCRC = PPP_INIT_CRC;                                              // prime tx crc value
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send start flag
    fnWritePPP(ucHDLC_Add, sizeof(ucHDLC_Add));                          // send address and control

    ucProtocolCode[0] = (unsigned char)(usProtocol>>8);
    ucProtocolCode[1] = (unsigned char)(usProtocol);
    fnWritePPP(ucProtocolCode, sizeof(ucProtocolCode));

    usOptionsLength <<= 8;
    usOptionsLength |= ppp_inf->ucPPP_Length[1];

    fnWritePPP((unsigned char *)ppp_inf, usOptionsLength);

    usTxCRC ^= 0xffff;
    ucCRC_16[0] = (unsigned char)(usTxCRC);
    ucCRC_16[1] = (unsigned char)(usTxCRC >> 8);
    fnWritePPP(ucCRC_16, sizeof(ucCRC_16));                              // add the check sum
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send end flag
}

// send back a configuration acknowledge since we accept the negotiation parameters
//
static void fnAccept(unsigned short usProtocol, PPP_INFO *ppp_inf)
{
    unsigned short usOptionsLength = ppp_inf->ucPPP_Length[0];
    unsigned char ucCRC_16[2];
    unsigned char ucProtocolCode[2];

    ppp_inf->ucPPP_Code = CONFIG_ACK;                                    // convert into ack frame

    usTxCRC = PPP_INIT_CRC;                                              // prime tx crc value
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send start flag
    fnWritePPP(ucHDLC_Add, sizeof(ucHDLC_Add));                          // send address and control

    ucProtocolCode[0] = (unsigned char)(usProtocol>>8);
    ucProtocolCode[1] = (unsigned char)(usProtocol);
    fnWritePPP(ucProtocolCode, sizeof(ucProtocolCode));

    usOptionsLength <<= 8;
    usOptionsLength |= ppp_inf->ucPPP_Length[1];

    fnWritePPP((unsigned char *)ppp_inf, usOptionsLength);

    usTxCRC ^= 0xffff;
    ucCRC_16[0] = (unsigned char)(usTxCRC);
    ucCRC_16[1] = (unsigned char)(usTxCRC >> 8);
    fnWritePPP(ucCRC_16, sizeof(ucCRC_16));                              // add the check sum
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send end flag
}

static void fnStartAuthentication(void)
{
    static const unsigned char ucPAP_authentication[] = {AUTH_PROT, 4, (unsigned char)(PAP_PROTOCOL>>8),(unsigned char)(PAP_PROTOCOL)};
    unsigned char ucProtocolCode[2];
    unsigned char ucCRC_16[2];
    PPP_INFO ppp_inf;

    usTxCRC = PPP_INIT_CRC;                                              // prime tx crc value
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send start flag
    fnWritePPP(ucHDLC_Add, sizeof(ucHDLC_Add));                          // send address and control

    ucProtocolCode[0] = (unsigned char)(LCP_PROTOCOL>>8);
    ucProtocolCode[1] = (unsigned char)(LCP_PROTOCOL);
    fnWritePPP(ucProtocolCode, sizeof(ucProtocolCode));

    ppp_inf.ucPPP_Code = CONFIG_REQUEST;
    ppp_inf.ucPPP_ID   = ucID++;
    ppp_inf.ucPPP_Length[0] = 0;
    ppp_inf.ucPPP_Length[1] = (sizeof(ppp_inf) + sizeof(ucPAP_authentication));
    fnWritePPP((unsigned char *)&ppp_inf, sizeof(ppp_inf));
    fnWritePPP((unsigned char *)&ucPAP_authentication, sizeof(ucPAP_authentication));

    usTxCRC ^= 0xffff;
    ucCRC_16[0] = (unsigned char)(usTxCRC);
    ucCRC_16[1] = (unsigned char)(usTxCRC >> 8);
    fnWritePPP(ucCRC_16, sizeof(ucCRC_16));                              // add the check sum
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send end flag
}

static void fnSendPAP_ok(void)
{
    unsigned char ucProtocolCode[2];
    unsigned char ucCRC_16[2];
    PPP_INFO ppp_inf;
    unsigned char no_data = 0;

    usTxCRC = PPP_INIT_CRC;                                              // prime tx crc value
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send start flag
    fnWritePPP(ucHDLC_Add, sizeof(ucHDLC_Add));                          // send address and control

    ucProtocolCode[0] = (unsigned char)(PAP_PROTOCOL>>8);
    ucProtocolCode[1] = (unsigned char)(PAP_PROTOCOL);
    fnWritePPP(ucProtocolCode, sizeof(ucProtocolCode));

    ppp_inf.ucPPP_Code = AUTHENTICATE_ACK;
    ppp_inf.ucPPP_ID   = ucID++;
    ppp_inf.ucPPP_Length[0] = 0;
    ppp_inf.ucPPP_Length[1] = (sizeof(ppp_inf) + 1);
    fnWritePPP((unsigned char *)&ppp_inf, sizeof(ppp_inf));
    fnWritePPP(&no_data, 1);

    usTxCRC ^= 0xffff;
    ucCRC_16[0] = (unsigned char)(usTxCRC);
    ucCRC_16[1] = (unsigned char)(usTxCRC >> 8);
    fnWritePPP(ucCRC_16, sizeof(ucCRC_16));                              // add the check sum
    fnWrite(PPP_PortID, (unsigned char *)&ucHDLC_Flag, sizeof(ucHDLC_Flag)); // send end flag
}

static unsigned char ucTestIP[] = {192,168,0,151};
static unsigned char ucTestDNS[] = {192,168,0,1};
static unsigned char ucTestN[] = {192,168,0,1};

static void fnNegotiateOptions(unsigned short usProtocol, PPP_INFO *ppp_inf, unsigned char *ptrFrame)
{
    int iFound = 0;
    unsigned short usOptionsLength = ppp_inf->ucPPP_Length[0];
    unsigned char ucSingleOptLength;
    PPP_OPTION *ppp_opt = (PPP_OPTION *)ptrFrame;
    usOptionsLength <<= 8;
    usOptionsLength |= ppp_inf->ucPPP_Length[1];
    usOptionsLength -= sizeof(PPP_INFO);

    while (usOptionsLength) {
        ptrFrame += sizeof(PPP_OPTION);
        if (LCP_PROTOCOL == usProtocol) {
            switch (ppp_opt->ucPPP_option) {
            case ACCM:
     //           ulACCM_bits = *ptrFrame;                               // set the requested character map
     //           ulACCM_bits <<= 8;            // validate only when moving to open state
     //           ulACCM_bits = *(ptrFrame+1);
     //           ulACCM_bits <<= 8;
     //           ulACCM_bits = *(ptrFrame+2);
     //           ulACCM_bits <<= 8;
     //           ulACCM_bits = *(ptrFrame+3);
                break;
            case MAGIC_NUMBER:
                uMemcpy(ucMagicNumer, ptrFrame, 4);
                break;
            case PFC:                                                    // Protocol field compression
            case ACFC:                                                   // address and control field compression
            case CALLBACK:
                fnReject(usProtocol, ppp_opt->ucPPP_option, ptrFrame, ppp_opt->ucPPP_option_length); // reject the option
                return;
            }
        }
        else if (CCP_PROTOCOL == usProtocol) {
            fnReject(usProtocol, ppp_opt->ucPPP_option, ptrFrame, ppp_opt->ucPPP_option_length);
            return;
        }
        else if (IPCP_PROTOCOL == usProtocol) {
            switch (ppp_opt->ucPPP_option) {
            case IPCP:                                                   // IP compression protocol
            //    fnReject(usProtocol, ppp_opt->ucPPP_option, ptrFrame, ppp_opt->ucPPP_option_length); // reject the option
            //    return;
                break;

            case IPIP:
                if (!(uMemcmp(ptrFrame, ucTestIP, IPV4_LENGTH))) {
                    iFound = 1;
                }
                else  {
                    uMemcpy(ptrFrame, ucTestIP, IPV4_LENGTH);
                }
                break;
            case IPDNS:
                if (!iFound) {
                    uMemcpy(ptrFrame, ucTestDNS, IPV4_LENGTH);
                }
                break;
            case IPNBNS:
                if (!iFound) {
                    uMemcpy(ptrFrame, ucTestN, IPV4_LENGTH);
                }
                break;
            case IPSDNS:
                if (!iFound) {
                    uMemcpy(ptrFrame, ucTestDNS, IPV4_LENGTH);
                }
                break;
            case IPSNBNS:
                if (!iFound) {
                    uMemcpy(ptrFrame, ucTestN, IPV4_LENGTH);
                }
                break;
            }
        }
        ucSingleOptLength = ppp_opt->ucPPP_option_length;
        if (ucSingleOptLength < sizeof(PPP_OPTION)) {
            ucSingleOptLength = sizeof(PPP_OPTION);                      // ensure length is not less that the opton field can really be
        }
        if (ppp_opt->ucPPP_option_length >= usOptionsLength) {           // protect against bad option length value
            usOptionsLength = 0;
        }
        else {
            usOptionsLength -= (ppp_opt->ucPPP_option_length);
        }
        ptrFrame += (ucSingleOptLength - sizeof(PPP_OPTION));
        ppp_opt = (PPP_OPTION *)ptrFrame;
    }
    if ((IPCP_PROTOCOL == usProtocol) && (!iFound)) {
        fnNAK(usProtocol, ppp_inf);
    }
    else {
        fnAccept(usProtocol, ppp_inf);                                   // if we haven't rejected a negotiation option accept now
    }
    if (LCP_PROTOCOL == usProtocol) {
        fnStartAuthentication();
    }
}

static int fnHandlePPP_frame(unsigned char *ptrFrame, QUEUE_TRANSFER ppp_frame_length)
{
    if (*ptrFrame++ == 0xff) {                                           // address alwas 0xff
        if (*ptrFrame++ == 0x03) {                                       // control always unnumbered info
            PPP_INFO *ppp_inf;
            unsigned short usProtocol = *ptrFrame++;
            usProtocol <<= 8;
            usProtocol |= *ptrFrame++;
            ppp_inf = (PPP_INFO *)ptrFrame;
            ptrFrame += sizeof(PPP_INFO);
            switch (usProtocol) {
            case LCP_PROTOCOL:
                switch (ppp_inf->ucPPP_Code) {                           // code
                case CONFIG_REQUEST:
                    fnNegotiateOptions(usProtocol, ppp_inf, ptrFrame);
                    break;
                case CONFIG_ACK:
                    break;                                               // peer has accepted out option request
                case CONFIG_NACK:
                    break;
                case CONFIG_REJECT:
                    break;
                case TERMINATE_REQUEST:                                  // terminate connection request
                    break;
                case IDENTIFICATION:                                     // peer moved to LCP opened state (often sent twice - no need to acknowledge it)
                    break;
                }
                break;

            case PAP_PROTOCOL:                                           // peer performing authentication
                switch (ppp_inf->ucPPP_Code) {
                case AUTHENTICATE_REQUEST:
                    fnSendPAP_ok();
                    break;
                }
                break;

            case CCP_PROTOCOL:                                           // compression control protocol
                switch (ppp_inf->ucPPP_Code) {                           // code
                case CONFIG_REQUEST:
                    fnNegotiateOptions(usProtocol, ppp_inf, ptrFrame);
                    break;
                }
                break;

            case IPCP_PROTOCOL:                                          // IP Control protocol
                switch (ppp_inf->ucPPP_Code) {                           // code
                case CONFIG_REQUEST:
                    fnNegotiateOptions(usProtocol, ppp_inf, ptrFrame);
                    break;
                }
                break;

            case IPV4_PROTOCOL:                                          // IPV4 data
                switch (ppp_inf->ucPPP_Code) {                           // code
                case CONFIG_REQUEST:
                    fnNegotiateOptions(usProtocol, ppp_inf, ptrFrame);
                    break;
                }
                break;
            }
        }
    }
    return PPP_STATE_CLIENT;
}

#ifdef _PPP_CRC_LOOKUP
// PPP CRC look up table as defined in RFC 1171
//
static const unsigned short fcstab[256] = {
      0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
      0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
      0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
      0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
      0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
      0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
      0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
      0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
      0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
      0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
      0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
      0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
      0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
      0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
      0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
      0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
      0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
      0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
      0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
      0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
      0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
      0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
      0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
      0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
      0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
      0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
      0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
      0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
      0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
      0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
      0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
      0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

static unsigned short fnPPP_crc(unsigned short usCRC, unsigned char ucData)
{
    usCRC = (usCRC >> 8) ^ fcstab[(usCRC ^ ucData) & 0xff];
    return usCRC;
}
#else
// Optimised code implementation of CCITT CRC-16
//
static unsigned short fnPPP_crc(unsigned short usCRC, unsigned char ucData)
{
    ucData ^= (unsigned char)usCRC;
    ucData ^= (ucData << 4);

    return ((((unsigned short)ucData << 8) | (usCRC >> 8)) ^ (ucData >> 4) ^ ((unsigned short)ucData << 3));
}
#endif



#endif
