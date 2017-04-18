/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      POP3.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    
*/        

#include "config.h"


#ifdef USE_POP3


#define OWN_TASK     TASK_POP3


static unsigned char ucPOP3_ip_address[IPV4_LENGTH] = {0};

#define POP_STATE_INIT              0
#define POP_STATE_CLOSED            1
#define POP_STATE_OPEN_REQUESTED    2
#define POP_STATE_OPEN_SENT         3
#define POP_STATE_CONNECTION_OPENED 4
#define POP_STATE_SERVER_READY      5        
#define POP_STATE_PASSWORD_SENT     6    
#define POP_STATE_STAT_SENT         7
#define POP_STATE_LIST              8
#define POP_STATE_TOP0_SENT         9
#define POP_STATE_RETR_SENT        10
#define POP_STATE_DELETE_SENT      11
#define POP_STATE_QUIT_SENT        12
#define POP_STATE_UIDL             13
#define POP_STATE_RETRIEVE         14
#define POP_STATE_RECEIVING_MAIL   15
#define POP_STATE_DELETE_MSG       16



#define POP_CLIENT_TIMEOUT         (DELAY_LIMIT)(20*SEC)
#define E_POP_TIMEOUT              1
#define E_POP_POLL_TIME            2


static int  fnSetNextPOP_state(unsigned char ucNextState);
static int  fnPOP3Listener(signed char cSocket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static CHAR *(*fnUserCallback)(unsigned char, unsigned char*) = 0;
static void fnPOP_error(unsigned char ucError);
static unsigned short fnRegenerate(void);
static int  fnHandleData(unsigned char *ptrData, unsigned short usDataLength);


static unsigned char  ucPOP_state = POP_STATE_INIT;
static USOCKET        POP_TCP_socket = -1;
static unsigned char  ipPOP3[IPV4_LENGTH];
static unsigned char  ucUnacked = 0;
static unsigned short usMsgCnt;


extern void fnPOP3(TTASKTABLE *ptrTaskTable)
{    
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (!ucPOP_state) {
        ucPOP_state = POP_STATE_CLOSED;
    }
    else {
        if ( fnRead( PortIDInternal, ucInputMessage, HEADER_LENGTH )) {  // check input queue
            if (ucInputMessage[ MSG_SOURCE_TASK ] == TIMER_EVENT) {
                if (ucInputMessage[ MSG_TIMER_EVENT ] == E_POP_TIMEOUT) {
                    fnPOP_error(ERROR_POP3_TIMEOUT);
                }
                else {                                                   // assume E_POP_POLL_TIME
                    if (uMemcmp(ucPOP3_ip_address, cucNullMACIP, IPV4_LENGTH)) {
                        fnConnectPOP3(ucPOP3_ip_address);                // check our mailbox to see whether we have post
                    }
                }
            }
        }
    }
}

// The user calls this to initiate a connection to the POP3 server
//
extern int fnConnectPOP3(unsigned char *ucIP)
{
    if (POP_TCP_socket < 0) {                                            // we have no socket - or called before initialisation complete    
        if ((POP_TCP_socket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, TCP_DEFAULT_TIMEOUT, fnPOP3Listener)) < 0) {
            return ERROR_POP3_NOT_READY;
        }
    }

    if (ucPOP_state != POP_STATE_CLOSED) return ERROR_POP3_IN_USE;       // called while already active

    uMemcpy(ipPOP3, ucIP, IPV4_LENGTH);                                  // save the address of the POP3 server we want to connect to
    fnSetNextPOP_state(POP_STATE_OPEN_REQUESTED);
    return 0;                                                            // OK    
}

extern void fnStartPopPolling(DELAY_LIMIT PollTime, CHAR *(*fnCallback)(unsigned char, unsigned char *))
{
    fnUserCallback = fnCallback;

    uTaskerMonoTimer( OWN_TASK, PollTime, E_POP_POLL_TIME );           // our emails will be requested after this delay
}

static int fnSetNextPOP_state(unsigned char ucNextState)
{
    unsigned short ucSent = 0;

    switch (ucPOP_state = ucNextState) {
      case POP_STATE_OPEN_REQUESTED:
          fnTCP_close(POP_TCP_socket);                                   // release existing connection
          if (fnTCP_Connect(POP_TCP_socket, ipPOP3, POP_PORT, 0, 0) >= 0) {
              ucPOP_state = POP_STATE_OPEN_SENT;
              ucUnacked = 0;
          }
          break;

      case POP_STATE_CLOSED:
          fnTCP_close(POP_TCP_socket);
          uTaskerStopTimer(OWN_TASK);
          ucUnacked = 0;
          fnUserCallback(POP3_CONNECTION_CLOSED, 0);
          return APP_REQUEST_CLOSE;

      case POP_STATE_DELETE_MSG:
      case POP_STATE_UIDL:
      case POP_STATE_RETRIEVE:
      case POP_STATE_QUIT_SENT:
      case POP_STATE_LIST:
      case POP_STATE_STAT_SENT:
      case POP_STATE_PASSWORD_SENT:
      case POP_STATE_SERVER_READY:
          ucSent = fnRegenerate();                                       // send user name / password etc.
          break;

    //case POP_STATE_CONNECTION_OPENED:
    //case POP_STATE_RECEIVING_MAIL:
      default:
          break;
    }
    uTaskerMonoTimer( OWN_TASK, POP_CLIENT_TIMEOUT, E_POP_TIMEOUT );     // monitor connection
    return (ucSent > 0);
}

// local listener to TCP POP3 port
//
static int fnPOP3Listener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    if (Socket != POP_TCP_socket) return APP_REJECT;                     // ignore if not our socket 

    switch (ucEvent) {
    case TCP_EVENT_ARP_RESOLUTION_FAILED:
        fnPOP_error(ERROR_POP3_ARP_FAIL);                                // inform client of failure - couldn't resolve the address..
        break;

    case TCP_EVENT_CONNECTED:
        if (ucPOP_state == POP_STATE_OPEN_SENT) {
            return (fnSetNextPOP_state(POP_STATE_CONNECTION_OPENED));
        }
        break;

    case TCP_EVENT_ACK:
        ucUnacked = 0;
        break;

    case TCP_EVENT_ABORT:
        if (ucPOP_state > POP_STATE_CLOSED) {
            fnPOP_error(ERROR_POP3_HOST_CLOSED);
            return APP_REQUEST_CLOSE;
        }
        // fall through intentional
    case TCP_EVENT_CLOSED:
    case TCP_EVENT_CLOSE:
        return (fnSetNextPOP_state(POP_STATE_CLOSED));

    case TCP_EVENT_REGENERATE:                                           // we must repeat
        return (fnRegenerate() > 0);

    case TCP_EVENT_DATA:                                                 // we have new receive data
        if (ucUnacked) return -1;                                        // ignore if we have unacked data
        if (usPortLen < 3) return -1;                                    // If the length is too short ignore it

        return (fnHandleData(ucIp_Data, usPortLen));                     // interpret the data

    case TCP_EVENT_CONREQ:                                               // we do not accept connection requests
    default:
        return -1;
    }

    return APP_ACCEPT;
}

#define POP3_MESSAGE_LEN 40

static unsigned short fnRegenerate(void)
{
    unsigned char ucPOP3Data[MIN_TCP_HLEN + POP3_MESSAGE_LEN];
    unsigned short usDataLen = 5;                                        // general length of strings
    CHAR *ptrStr = (CHAR *)&ucPOP3Data[MIN_TCP_HLEN];
    CHAR *ptrUserInfo;

    switch (ucPOP_state) {                                               // resent last packet
        case POP_STATE_SERVER_READY:                                     // send user name
            uStrcpy(ptrStr, "USER ");                                    
            ptrUserInfo = fnUserCallback(POP3_USER_NAME, 0);             // callback to get user name
            uStrcpy((ptrStr + usDataLen), ptrUserInfo);
            usDataLen += uStrlen(ptrUserInfo);
            break;
        
        case POP_STATE_PASSWORD_SENT:                                    // send user password
            uStrcpy(ptrStr, "PASS ");
            ptrUserInfo = fnUserCallback(POP3_USER_PASS, 0);             // callback to get user password
            uStrcpy((ptrStr + usDataLen), ptrUserInfo);
            usDataLen += uStrlen(ptrUserInfo);
            break;
            
        case POP_STATE_STAT_SENT:                                        // send stat
            uStrcpy(ptrStr, "STAT");
            usDataLen = 4;
            break;
        
        case POP_STATE_LIST:
            uStrcpy(ptrStr, "LIST ");                                    // add the message number
            break;
        
        case POP_STATE_TOP0_SENT:
            uStrcpy(ptrStr, "TOP 0");
            break;    
        
        case POP_STATE_RETR_SENT:
            uStrcpy(ptrStr, "RETR ");                                    // add the message number
            break;    
        
        case POP_STATE_DELETE_SENT:
            uStrcpy(ptrStr, "DELE ");                                    // add the message number
            break;
        
        case POP_STATE_QUIT_SENT:
            uStrcpy(ptrStr, "QUIT");
            usDataLen = 4;
            break;

        case POP_STATE_UIDL:
            uStrcpy(ptrStr, "UIDL");                                     // we do the same as Outlook and send first UIDL for the UID listing of the mailbox
            usDataLen = 4;
            break;

        case POP_STATE_RETRIEVE:
            uStrcpy(ptrStr, "RETR 1");
            usDataLen = 6;
            break;

        case POP_STATE_DELETE_MSG:
            uStrcpy(ptrStr, "DELE 1");
            usDataLen = 6;
            break;

        default:
            return 0;
    }
    ptrStr[usDataLen++] = '\r';                                          // terminate
    ptrStr[usDataLen++] = '\n';

    return (fnSendTCP(POP_TCP_socket, ucPOP3Data, usDataLen, TCP_FLAG_PUSH) > 0); // send data
}

// The POP3 server has responded with a positive status responds which indicates the mumber of waiting messages
// The expected format is +OK N YYYY - N is number of messages, YYYY is size terminated with CRLF
//
static int fnRetrieveStat(unsigned char *ptrData)
{
    int iLen = 0;
    usMsgCnt = 0;

    ptrData += 4;
    while (*ptrData != ' ') {
        if (iLen++ >= 5) {
            return 1;
        }
        usMsgCnt *= 10;
        usMsgCnt += (*ptrData++ - '0');
    }
    return 0;
}

static CHAR *fnSearch(CHAR *ptrData, const CHAR *ptrSearched, unsigned short *usDataLeft)
{
    unsigned short usMatch = 0;
    unsigned short usLength = *usDataLeft;
    const CHAR *ptrSearching = ptrSearched;

    while (usLength--) {
        if (*ptrData++ != *ptrSearching) {
            usMatch = 0;
            ptrSearching = ptrSearched;
        }
        else {
            if (!(*(++ptrSearching)) && usMatch) {
                *usDataLeft = usLength;
                return (ptrData);                                        // return position of following charcaters
            }
            usMatch++;
        }
    }
    return 0;                                                            // not found
}


static int fnHandleData(unsigned char *ptrData, unsigned short usDataLength)
{
    if (*ptrData == '+') {                                               // POP OK
        switch (ucPOP_state) {
        case POP_STATE_CONNECTION_OPENED:
            return (fnSetNextPOP_state(POP_STATE_SERVER_READY));

        case POP_STATE_SERVER_READY:                                     // our user name has been acked
            return (fnSetNextPOP_state(POP_STATE_PASSWORD_SENT));

        case POP_STATE_PASSWORD_SENT:                                    // our Password has been acked
            return (fnSetNextPOP_state(POP_STATE_STAT_SENT));

        case POP_STATE_STAT_SENT:                                        // STAT results have been received
            if (fnRetrieveStat(ptrData)) {                               // if we have an error parsing the STAT response inform the user and close link
                fnPOP_error(ERROR_POP3_DIDNT_UNDERSTAND);
                return -1;
            }
            if ((fnUserCallback(POP3_WAITING_MESSAGES, (unsigned char *)(CAST_POINTER_ARITHMETIC)usMsgCnt)) != 0) {
              return (fnSetNextPOP_state(POP_STATE_UIDL));
            }
            else {
              return (fnSetNextPOP_state(POP_STATE_QUIT_SENT));
            }
            
        case POP_STATE_QUIT_SENT:                                        // quit was successful - we try to close the TCP connection
            return (fnSetNextPOP_state(POP_STATE_CLOSED));

        case POP_STATE_UIDL:                                             // we have received an OK from POP3 server to the UIDL request
            return (fnSetNextPOP_state(POP_STATE_LIST));

        case POP_STATE_LIST:                                             // we have received an OK from POP3 server to the LIST request
            return (fnSetNextPOP_state(POP_STATE_RETRIEVE));

        case POP_STATE_RETRIEVE:                                         // we have received an OK from POP3 server to the RETR request
            return (fnSetNextPOP_state(POP_STATE_RECEIVING_MAIL));

        case POP_STATE_DELETE_MSG:
            return(fnSetNextPOP_state(POP_STATE_QUIT_SENT));
        }
    }
    else {
        if (ucPOP_state == POP_STATE_RECEIVING_MAIL) {                   // we are receiving data
            CHAR *ptr;
            CHAR *ptrEnd = (CHAR *)ptrData;
            unsigned short usRemaining = usDataLength;
            unsigned char ucEvent = POP3_RX_MESSAGE;
            if (fnSearch((CHAR *)ptrData, "\r\n.\r\n", &usRemaining)) {  // see whether we can find the end of a complete message
                usDataLength -= (usRemaining + 3);
                ucEvent = POP3_RX_MESSAGE_END;
            }

            *(ptrData + usDataLength) = 0;                               // string terminate the frame

            if ((ptr = fnUserCallback(ucEvent, ptrData)) != 0) {
                if (POP3_SEARCH_SUBJECT == *ptr) {                       // the user wants to be informed of subject so we scan it here
                    if ((ptr = fnSearch((CHAR *)ptrData, "Subject: ", &usDataLength)) != 0) {
                        // convert the subject in the buffer to a null terinated string
                        // WARNING: we presently do not handle message over multiple TCP/IP buffer and so this can fail
                        ptrEnd = ptr;
                        while (usDataLength--) {
                            if (*ptrEnd == 0x0d) {
                                *ptrEnd = 0;
                                break;
                            }
                            ptrEnd++;
                        }
                        if (!(ptr = fnUserCallback(POP3_RX_SUBJECT, (unsigned char *)ptr))) {
                            return (fnSetNextPOP_state(POP_STATE_QUIT_SENT)); // application doesn't accept the subject so quite
                        }
                    }
                    if (!ptr) {
                        return 0;
                    }
                }

                if (POP3_GOOD_SUBJECT == *ptr) {                         // we search for the message content
                    if ((ptrEnd = fnSearch(ptrEnd, "\r\n\r\n", &usDataLength)) != 0) { // pass the remaining contents to the user
                        if (POP3_RX_MESSAGE_END == ucEvent) {
                            ucEvent = POP3_RX_TINY_MESSAGE;              // start and end found - must be a very short message
                        }
                        else {
                            ucEvent = POP3_RX_MESSAGE_START;
                        }
                        ptr = fnUserCallback(ucEvent, (unsigned char *)ptrEnd);
                    }
                }

                if (POP3_DELETE_MESSAGE == *ptr) {
                    return (fnSetNextPOP_state(POP_STATE_DELETE_MSG));    // the application has received a complete message and wants it to be deleted
                }

                if (ucEvent == POP3_RX_MESSAGE_END) {
                    return (fnSetNextPOP_state(POP_STATE_QUIT_SENT));    // Email has been completely received so we can close the connection
                }
            }
        }                                                                // else we assume '-' negative response                                                                         
    }
    return 0;
}


static void fnPOP_error(unsigned char ucError)
{
    fnSetNextPOP_state(POP_STATE_CLOSED);
    fnUserCallback(ucError, 0);
}


#endif

