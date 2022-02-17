/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      smtp.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    16.02.2007 Add SMTP LOGIN support (#if defined USE_SMTP_AUTHENTICATION) and improve TCP interraction to avoid unnecessary ACKs
    18.05.2007 Adjust fnEncode64() use                                   {1}

*/

#include "config.h"


#if defined USE_SMTP 


#define OWN_TASK                    TASK_SMTP


#define SMTP_STATE_INIT              0                                   // SMTP states
#define SMTP_STATE_CLOSED            1
#define SMTP_OPEN_REQUESTED          2
#define SMTP_STATE_OPEN_SENT         3
#define SMTP_STATE_CONNECTION_OPENED 4
#define SMTP_STATE_SERVER_READY      5
#define SMTP_STATE_LOGIN_1           6
#define SMTP_STATE_LOGIN_2           7
#define SMTP_STATE_LOGIN_3           8
#define SMTP_STATE_MAIL_TO_SEND      9
#define SMTP_RCPTTO_SENT             10
#define SMTP_RCPTTO_ACKED            11
#define SMTP_DATAREQ_ACKED           12
#define SMTP_SENDING_DATA            13
#define SMTP_END                     14
#define SMTP_QUITTING                15


#define SMTP_CMD_SERVER_READY        0x220                               // server sends this when connected
#define SMTP_CMD_OK                  0x250                               // server informs that command successfully executed
#define SMTP_CMD_DATAOK              0x354                               // server informs OK to send
#define SMTP_CMD_QUITOK              0x221                               // server informs OK to quit
#define SMTP_CMD_LOGIN_USER          0x334                               // server informs to login with user name or password
#define SMTP_CMD_LOGIN_OK            0x235                               // server informs of successful login
#define SMTP_CMD_LOGIN_BAD           0x535                               // server informs of failed login
#define SMTP_CMD_LOGIN_UNAVAILABLE   0x501                               // server informs that LOGIN command is unavailable
#define SMTP_CMD_REJECTED            0x550                               // server informs that EMAIL rejected due to Policy Rejection (possibly login required)



#define SMTP_SIMPLE_TCP_ACK   0                                          // pseudo ack


#define SMTP_CLIENT_TIMEOUT (DELAY_LIMIT)(30*SEC)                        
#define E_SMTP_TIMEOUT 1


static int  fnSetNextSMTP_state(unsigned char ucNextState);
static int  fnSMTPListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static int  fnSMTP_error(unsigned char ucError);
static int  fnRegenerate(void);
static int  fnHandleData(unsigned char *ptrData);

static unsigned char  ucSMTP_state = SMTP_STATE_INIT;
static USOCKET        SMTP_TCP_socket = -1;
static unsigned char  ipSMTP[IPV4_LENGTH];
static unsigned short usEmailTextPosition;
static const CHAR  *(*fnUserCallback)(unsigned char, unsigned short *) = 0;
#if defined USE_SMTP_AUTHENTICATION
    static unsigned char ucSMTP_mode = 0;
#endif

extern void fnSmtp(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (ucSMTP_state != SMTP_STATE_INIT) {
        if ( fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {    // check input queue
            if (ucInputMessage[MSG_SOURCE_TASK] == TIMER_EVENT) {
                fnSMTP_error(ERROR_SMTP_TIMEOUT);                        // assume E_SMTP_TIMEOUT
            }
        }
    }
}

// The user calls this to initiate a connection to the SMTP server
//
extern int fnConnectSMTP(unsigned char *ucIP, unsigned char ucMode, const CHAR *(*fnCallback)(unsigned char, unsigned short *))
{
    if (SMTP_TCP_socket < 0) {                                           // we have no socket - or called before initialisation complete
        if ((SMTP_TCP_socket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, TCP_DEFAULT_TIMEOUT, fnSMTPListener)) < 0) {
            return ERROR_SMTP_NOT_READY;
        }
    }

    if (ucSMTP_state > SMTP_STATE_CLOSED) {
        return ERROR_SMTP_IN_USE;                                        // called while already active
    }

    uMemcpy(ipSMTP, ucIP, IPV4_LENGTH);                                  // save the address of the SMTP server we want to connect to
    fnUserCallback = fnCallback;
#if defined USE_SMTP_AUTHENTICATION
    ucSMTP_mode = ucMode;                                                // mode of operation for this transmission
#endif

    fnSetNextSMTP_state(SMTP_OPEN_REQUESTED);
    return 0;                                                            // OK
}

static int fnSetNextSMTP_state(unsigned char ucNextState)
{
    int iRtn = APP_ACCEPT;

    switch (ucSMTP_state = ucNextState) {
    case SMTP_OPEN_REQUESTED:
        fnTCP_close(SMTP_TCP_socket);                                    // release existing connection
        if (fnTCP_Connect(SMTP_TCP_socket, ipSMTP, SMTP_PORT, 0, 0) >= 0) {
            ucSMTP_state = SMTP_STATE_OPEN_SENT;
            uTaskerMonoTimer( OWN_TASK, SMTP_CLIENT_TIMEOUT, E_SMTP_TIMEOUT );
        }
        break;

    case SMTP_STATE_CLOSED:                                              // the SMTP server closes the TCP/IP connection
        uTaskerStopTimer(OWN_TASK);                                      // stop monitoring the connection
        fnUserCallback(SMTP_MAIL_SUCCESSFULLY_SENT, 0);                  // inform the application that the Email has been successfully sent
        return iRtn;

    case SMTP_STATE_CONNECTION_OPENED:
        break;

  //case SMTP_QUITTING:                                                  // all other states
  //case SMTP_END:
  //case SMTP_SENDING_DATA:
  //case SMTP_DATAREQ_ACKED:
  //case SMTP_RCPTTO_ACKED:
  //case SMTP_RCPTTO_SENT:
  //case SMTP_STATE_MAIL_TO_SEND:
  //case SMTP_STATE_SERVER_READY:                                        // etc.
    default:
        iRtn = fnRegenerate();                                           // send user name / password etc.
        break;
   }
   uTaskerMonoTimer( OWN_TASK, SMTP_CLIENT_TIMEOUT, E_SMTP_TIMEOUT );    // monitor connection
   return iRtn;
}

// Local listener on SMTP port
//
static int fnSMTPListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    if (Socket != SMTP_TCP_socket) {
        return APP_REJECT;                                               // ignore if not our socket
    }

    switch (ucEvent) {
    case TCP_EVENT_ARP_RESOLUTION_FAILED:
        return (fnSMTP_error(ERROR_SMTP_ARP_FAIL));                      // inform client of failure - couldn't resolve the IP address..

    case TCP_EVENT_CONNECTED:
        if (ucSMTP_state == SMTP_STATE_OPEN_SENT) {
            return (fnSetNextSMTP_state(SMTP_STATE_CONNECTION_OPENED));
        }
        break;

    case TCP_EVENT_ACK:
        if (ucSMTP_state >= SMTP_DATAREQ_ACKED) {
            fnHandleData(0);
        }
        break;

    case TCP_EVENT_CLOSE:
        break;

    case TCP_EVENT_ABORT:
        if (ucSMTP_state > SMTP_STATE_CLOSED) {
            return (fnSMTP_error(ERROR_SMTP_HOST_CLOSED));
        }
        // Fall through intentional
        //
    case TCP_EVENT_CLOSED:
        return (fnSetNextSMTP_state(SMTP_STATE_CLOSED));

    case TCP_EVENT_REGENERATE:                                           // we must repeat
        return (fnRegenerate());                      

    case TCP_EVENT_DATA:                                                 // we have new receive data
        if (usPortLen < 3) {
            return -1;                                                   // if the length is too short ignore it
        }

        return (fnHandleData(ucIp_Data));                                // interpret the data

    case TCP_EVENT_CONREQ:                                               // we do not accept connection requests
    default:
        return APP_REJECT;
    }
    return APP_ACCEPT;
}

// Send or resent a frame
//
static int fnRegenerate(void)
{
    CHAR cSMTPData[MIN_TCP_HLEN + SMTP_MESSAGE_LEN];
    unsigned short usDataLen = 5;                                        // general length of strings
    CHAR *ptrStr = &cSMTPData[MIN_TCP_HLEN];
    const CHAR *ptrUserInfo;
    unsigned char ucPush = TCP_FLAG_PUSH;

    switch (ucSMTP_state) {                                              // resend last packet
    case SMTP_STATE_SERVER_READY:                                        // send Hello
        uStrcpy(ptrStr, "EHLO ");                                        // callback to get user name
        ptrUserInfo = fnUserCallback(SMTP_GET_DOMAIN, 0);
        uStrcpy((ptrStr + usDataLen), ptrUserInfo);
        usDataLen = uStrlen(ptrStr);
        break;

#if defined USE_SMTP_AUTHENTICATION
    case SMTP_STATE_LOGIN_1:
        uStrcpy(ptrStr, "AUTH LOGIN");                                   // request login
        usDataLen = 10;
        break;                                                           // from this point until login has completed, all text is base 64 coded

    case SMTP_STATE_LOGIN_2:
        ptrUserInfo = fnUserCallback(SMTP_USER_NAME, 0);                 // callback to get user name
        usDataLen = (unsigned short)fnEncode64((unsigned char *)ptrUserInfo, ptrStr, 0); // {1}
        break;

    case SMTP_STATE_LOGIN_3:
        ptrUserInfo = fnUserCallback(SMTP_USER_PASS, 0);                 // callback to get user password
        usDataLen = (unsigned short)fnEncode64((unsigned char *)ptrUserInfo, ptrStr, 0); // {1}
        break;
#endif

    case SMTP_STATE_MAIL_TO_SEND:                                        // send Mail from
        uStrcpy(ptrStr, "MAIL FROM: <");                                 // callback to get sender
        usDataLen = 12;
        ptrUserInfo = fnUserCallback(SMTP_GET_SENDER, 0);
        uStrcpy((ptrStr + usDataLen), ptrUserInfo);
        usDataLen = uStrlen(ptrStr);
        *(ptrStr + usDataLen) = '>';
        usDataLen++;
        break;

    case SMTP_RCPTTO_SENT:                                               // send destination
        uStrcpy(ptrStr, "RCPT TO: <");                                   // callback to get destination
        usDataLen = 10;
        ptrUserInfo = fnUserCallback(SMTP_GET_DESTINATION, 0);
        uStrcpy((ptrStr + usDataLen), ptrUserInfo);
        usDataLen = uStrlen(ptrStr);
        *(ptrStr + usDataLen) = '>';
        usDataLen++;
        break;

    case SMTP_RCPTTO_ACKED:                                              // send DATA request
        uStrcpy(ptrStr, "DATA");
        usDataLen = 4;
        break;

    case SMTP_DATAREQ_ACKED:                                             // send email body
        uStrcpy(ptrStr, "To: ");                                         // callback to get destination and subject and sender
        usDataLen = 4;
        ptrUserInfo = fnUserCallback(SMTP_GET_DESTINATION, 0);
        uStrcpy((ptrStr + usDataLen), ptrUserInfo);
        usDataLen += uStrlen((CHAR *)ptrUserInfo);
        uStrcpy(ptrStr + usDataLen, "\r\nSubject: ");
        usDataLen += 11;
        ptrUserInfo = fnUserCallback(SMTP_GET_SUBJECT, 0);
        uStrcpy((ptrStr + usDataLen), ptrUserInfo);
        usDataLen += uStrlen((CHAR *)ptrUserInfo);
        uStrcpy(ptrStr + usDataLen, "\r\nFrom: ");
        usDataLen += 8;
        ptrUserInfo = fnUserCallback(SMTP_GET_SENDER, 0);
        uStrcpy((ptrStr + usDataLen), ptrUserInfo);
        usDataLen += uStrlen((CHAR *)ptrUserInfo);
        uStrcpy(ptrStr + usDataLen, "\r\n");                             // insert empty row
        usDataLen += 2;
        usEmailTextPosition = 0;                                         // prepare for content transmission
        break;

    case SMTP_END:
    case SMTP_SENDING_DATA:
        {
            unsigned short usEmailData = usEmailTextPosition;
            ptrUserInfo = fnUserCallback(SMTP_SEND_MESSAGE, &usEmailData);
            if (!usEmailData) {
                uStrcpy(ptrStr, "\r\n.");                                // application has no more data in this email - so send data end
                usDataLen = 3;
                ucSMTP_state = SMTP_END;
            }
            else {
                ucPush = 0;
                if ((usDataLen = usEmailData) > SMTP_MESSAGE_LEN) {
                    usDataLen = SMTP_MESSAGE_LEN;                        // don't let the application overload the buffer
                }
                uMemcpy(ptrStr, ptrUserInfo, usDataLen);                 // copy the data locally
            }
        }
        break;

    case SMTP_QUITTING:
        uStrcpy(ptrStr, "QUIT");
        usDataLen = 4;
        break;

    default:
        return APP_ACCEPT;                                               // no TCP data sent
    }
    if (ucSMTP_state != SMTP_SENDING_DATA) {
        ptrStr[usDataLen++] = '\r';                                      // terminate
        ptrStr[usDataLen++] = '\n';
    }

    if (fnSendTCP(SMTP_TCP_socket, (unsigned char *)cSMTPData, usDataLen, ucPush) <= 0) { // send data
        return APP_ACCEPT;                                               // no TCP data sent
    }

    return (APP_SENT_DATA);                                              // we have sent TCP data
}


static int fnHandleData(unsigned char *ptrData)
{
    unsigned short usCommand;

    if (!ptrData) {
        usCommand = SMTP_SIMPLE_TCP_ACK;
    }
    else {
        usCommand  = *ptrData++ - '0';
        usCommand  <<= 4;
        usCommand += (*ptrData++ - '0');
        usCommand  <<= 4;
        usCommand += (*ptrData++ - '0');
    }

    switch (ucSMTP_state) {
    case SMTP_STATE_CONNECTION_OPENED:                                   // we have a fresh TCP connection
        if (usCommand == SMTP_CMD_SERVER_READY) {
            return (fnSetNextSMTP_state(SMTP_STATE_SERVER_READY));
        }
        break;

    case SMTP_STATE_SERVER_READY:                                        // we have sent EHLO and expect an ACK
        if (SMTP_CMD_OK == usCommand) {
#if defined USE_SMTP_AUTHENTICATION
            if (ucSMTP_mode & SMTP_LOGIN) {                              // in authentication mode we request login
                return (fnSetNextSMTP_state(SMTP_STATE_LOGIN_1));
            }
            else {
                return (fnSetNextSMTP_state(SMTP_STATE_MAIL_TO_SEND));   // no login
            }
#else
            return (fnSetNextSMTP_state(SMTP_STATE_MAIL_TO_SEND));
#endif
        }
        break;

#if defined USE_SMTP_AUTHENTICATION
    case SMTP_STATE_LOGIN_1:
        if ((SMTP_CMD_LOGIN_USER == usCommand) && (*(ptrData + 1) == 'V')) { // check that the first byte of base 64 coded USERNAME is correct
            return (fnSetNextSMTP_state(SMTP_STATE_LOGIN_2));
        }
        else if (SMTP_CMD_LOGIN_UNAVAILABLE == usCommand) {
            return (fnSMTP_error(ERROR_SMTP_LOGIN_NOT_SUPPORTED));
        }
        break;

    case SMTP_STATE_LOGIN_2:
        if ((SMTP_CMD_LOGIN_USER == usCommand) && (*(ptrData+1) == 'U')) { // check that the first byte of base 64 coded PASS is correct
            return (fnSetNextSMTP_state(SMTP_STATE_LOGIN_3));
        }
        break;

    case SMTP_STATE_LOGIN_3:
        if (SMTP_CMD_LOGIN_OK == usCommand) {
            return (fnSetNextSMTP_state(SMTP_STATE_MAIL_TO_SEND));       // continue with email transmission
        }
        else if (SMTP_CMD_LOGIN_BAD == usCommand) {
            return (fnSMTP_error(ERROR_SMTP_LOGIN_FAILED));
        }
        break;
#endif

    case SMTP_STATE_MAIL_TO_SEND:                                        // we have sent Mail from and expect an ACK
        if (SMTP_CMD_OK == usCommand) {
            return (fnSetNextSMTP_state(SMTP_RCPTTO_SENT));
        }
        break;

    case SMTP_RCPTTO_SENT:                                               // we have sent Destination and expect an ACK
        if (SMTP_CMD_OK == usCommand) {
            return (fnSetNextSMTP_state(SMTP_RCPTTO_ACKED));
        }
        else if (SMTP_CMD_REJECTED == usCommand) {                       // rejected - possibly needs LOGIN
            return (fnSMTP_error(ERROR_SMTP_POLICY_REJECT));
        }
        break;

    case SMTP_RCPTTO_ACKED:
        if (SMTP_CMD_DATAOK == usCommand) {                              // we have sent data request and expect a Data Ack
            return (fnSetNextSMTP_state(SMTP_DATAREQ_ACKED));
        }
        break;

    case SMTP_DATAREQ_ACKED:                                             // We have sent the Email body and continue with data
        if (SMTP_SIMPLE_TCP_ACK == usCommand) {
            return (fnSetNextSMTP_state(SMTP_SENDING_DATA));
        }
        break;

    case SMTP_SENDING_DATA:
        if (SMTP_SIMPLE_TCP_ACK == usCommand) {
            usEmailTextPosition += SMTP_MESSAGE_LEN;                     // we send always blocks of this length
            return (fnSetNextSMTP_state(SMTP_SENDING_DATA));
        }
        break;

    case SMTP_END:
        if (SMTP_CMD_OK == usCommand) {
            return (fnSetNextSMTP_state(SMTP_QUITTING));
        }
        break;


    case SMTP_QUITTING:
        if (SMTP_CMD_QUITOK == usCommand) {
            return (fnSetNextSMTP_state(SMTP_STATE_CLOSED));
        }
        break;
    }
    return APP_ACCEPT;
}

// General error reporting routine
//
static int fnSMTP_error(unsigned char ucError)
{
    fnTCP_close(SMTP_TCP_socket);                                        // close connection on error
    ucSMTP_state = SMTP_STATE_CLOSED;
    fnUserCallback(ucError, 0);
    return APP_REQUEST_CLOSE;
}
#endif

