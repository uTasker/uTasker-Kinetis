/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      tftp.c [TFTP server]
    Project:   uTasker - Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    22.05.2007 Sub-file write support added for devices with large flash granularity
    16.02.2008 Correct TFTP mode check                                   {1}
    16.02.2008 Correct broadcast address check                           {2}
    01.07.2009 Adapt for compatibility with STRING_OPTIMISATION          {3}
    03.11.2010 Add sub-file support                                      {4}
    15.02.2011 Additional location with sub-file support                 {5}

*/

#include "config.h"


#ifdef USE_TFTP


#define OWN_TASK          TASK_TFTP

#define TFTP_STATE_IDLE         0x00
#define TFTP_STATE_READING      0x01
#define TFTP_STATE_WRITING      0x02
#define TFTP_NET_ASCII_MODE     0x04
#define TFTP_OCTET_MODE         0x08
#define TFTP_STATE_TERMINATING  0x10
#define TFTP_STATE_CHECKING     0x20
#define TFTP_STATE_CLIENT       0x40
#define TFTP_STATE_CLIENT_START 0x80

#define FULL_TFTP_FRAME      512
#define TFTP_BUFFER_SIZE     (2 + 2 + 512)


#define TFTP_TIMEOUT          1
#define TFTP_ACK_TIMEOUT      2

#define TFTP_TIMEOUT_PERIOD   (6*SEC)
#define TFTP_ACK_PERIOD       (1*SEC)


typedef struct stUDP_TFTP_MESSAGE
{     
    UDP_HEADER     tUDP_Header;                                          // reserve header space
    unsigned char  ucUDP_Message[TFTP_BUFFER_SIZE];
} UDP_TFTP_MESSAGE;

typedef struct stUDP_TFTP_ACK
{     
    UDP_HEADER     tUDP_Header;                                          // reserve header space
    unsigned char  ucUDP_Message[4];
} UDP_TFTP_ACK;

static const CHAR tftp_error_message[] = TFTP_ERROR_MESSAGE;
typedef struct stUDP_TFTP_ERR
{     
    UDP_HEADER     tUDP_Header;                                          // reserve header space
    unsigned char  ucUDP_Message[4];
    CHAR           cErrorStr[sizeof(tftp_error_message)];
} UDP_TFTP_ERR;

#ifdef SUB_FILE_SIZE                                                     // {4}
    #define SUBFILE_WRITE  ,ucSubFileInProgress
    #define SUB_FILE_ON    ,SUB_FILE_TYPE
#else
    #define SUBFILE_WRITE
    #define SUB_FILE_ON
#endif


static unsigned char   ucTFTP_state = TFTP_STATE_IDLE;
static USOCKET         TFTPSocket = -1;                                  // UDP socket number
static void          (*fnApplicationCallback)(unsigned short usError, CHAR *error_message);
static unsigned short  tftp_client_port;
static unsigned char   tftp_client_ip[IPV4_LENGTH];
static unsigned char  *tftp_file_pointer;
static unsigned char  *tftp_next_file;
static unsigned short  tftp_block_number;
static MAX_FILE_LENGTH tftp_transfer_length;
static MAX_FILE_LENGTH tfpt_write_length;
static unsigned char   ucBlockRepeats;
#ifdef SUPPORT_MIME_IDENTIFIER
   static unsigned char ucMimeType;
#endif
#ifdef SUB_FILE_SIZE                                                     // {4}
    static unsigned char ucSubFileInProgress = 0;
#endif

static int  fnTFTPListner(USOCKET tftp_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen);
static void fnTFTP_error(unsigned short usError, CHAR *error_message);
static void fnSendTFTP_read_write(unsigned char ucReadWrite, CHAR *cFile);
static void fnSendTFTP_error(unsigned short usTFTP_error);
static void fnSendTFTP_block(int iRepeat);
  #define REPEAT_BLOCK   0
  #define FIRST_BLOCK_TX 1
  #define SUCCESSFUL_ACK 2


static const CHAR cOctet[]    = "octet";
#ifdef SUPPORT_NET_ASCII
    static const CHAR cNetAscii[] = "netascii";
#endif

// TFTP task - handles timeouts
//
extern void fnTftp(TTASKTABLE *ptrTaskTable)
{    
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if ( fnRead( PortIDInternal, ucInputMessage, HEADER_LENGTH )) {      // check input queue
        switch (ucInputMessage[ MSG_SOURCE_TASK ]) {
        case TIMER_EVENT:
            if (TFTP_TIMEOUT == ucInputMessage[MSG_TIMER_EVENT]) {
                if (ucTFTP_state & TFTP_STATE_TERMINATING) {                 
                    fnTFTP_error(TFTP_TRANSFER_WRITE_COMPLETE, 0);
                }
                else {
                    fnTFTP_error(TFTP_TRANSFER_DID_NOT_START, 0);        // no activity so close 'virtual' connection
                }
            }
            else if (TFTP_ACK_TIMEOUT == ucInputMessage[MSG_TIMER_EVENT]) {
                fnSendTFTP_block(REPEAT_BLOCK);                          // repeat last block
            }
            break;

        case  TASK_ARP:
            fnRead( PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the contents
            fnStopTFTP_server();
            switch (ucInputMessage[ 0 ]) {                               // ARP sends us either ARP resolution success or failed
            case ARP_RESOLUTION_SUCCESS:                                 // IP address has been resolved (repeat UDP frame).
                fnTFTP_error(TFTP_ARP_RESOLVED,0);                       // the user should retry since ARP has resolved the target IP address
                break;

            case ARP_RESOLUTION_FAILED:                                  // IP address could not be resolved...
                fnTFTP_error(TFTP_ARP_RESOLUTION_FAILED,0);              // the user should quit since ARP can not resolve the target address
                break;
            }
            
            break;
        }
    }
}


// Start TFTP server
//
extern int fnStartTFTP_server(void (*Callback)(unsigned short, CHAR *))
{
    if (TFTPSocket < 0) {
        if (((TFTPSocket = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnTFTPListner, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS))) >= 0)) {
            fnBindSocket(TFTPSocket, TFTP_SERVER_PORT);
            fnApplicationCallback = Callback;
            return 0;
        } 
    }
    return 1;                                                            // server could not be started, or already running
}

// Stop TFTP server
//
extern void fnStopTFTP_server(void)
{
    fnReleaseUDP_socket(TFTPSocket);
    TFTPSocket = -1;
    fnTFTP_error(0,0);                                                   // reset for next time
}

// Initiate TFTP client tranfer
//
extern int fnStartTFTP_client(void (*Callback)(unsigned short, CHAR *), unsigned char *ucIP, unsigned char ucReadWrite, CHAR *cFile, CHAR uFile)
{
    CHAR _uFile = uFile;                                                 // ensure parameter has an address
    if (fnStartTFTP_server(Callback)) {
        return -1;                                                       // socket already in use or couldn't be bound
    }
    uMemcpy(tftp_client_ip, ucIP, IPV4_LENGTH);                          // fill out the IP address of the TFTP server to be contacted
    tftp_client_port = TFTP_SERVER_PORT;                                 // start tranfer on well known port
    tftp_block_number = 0;
    tftp_next_file = tftp_file_pointer = uOpenFile(&_uFile);             // open file to be sent or for reception
    if (ucReadWrite == TFTP_PUT) {
        tftp_transfer_length = uGetFileLength(tftp_file_pointer);
        if (!tftp_transfer_length) {
            return -1;                                                   // local file doesn't exist
        }
        tftp_file_pointer += FILE_HEADER;                                // set to first data location
    }
    else {
#ifdef SUPPORT_MIME_IDENTIFIER
        ucMimeType = fnGetMimeType(cFile);                               // get the type of file being transfered
#endif
#ifdef SUB_FILE_SIZE                                                     // {4}
        ucSubFileInProgress = fnGetFileType(cFile);                      // get file characteristics so that it is later handled correctly
#endif
    }
    tfpt_write_length = 0;
    fnSendTFTP_read_write(ucReadWrite, cFile);                           // initiate the transfer
    return 0;                                                            
}


// extract the file name and mode 
//
static unsigned char fnGetFileMode(CHAR *Data, unsigned short usDataLength)
{
    while (usDataLength--) {
        if (*Data++ == 0) {                                              // end of file name reached
            if (uStrEquiv(cOctet, Data) > 0) {                           // {1}
                return (TFTP_OCTET_MODE);
            }
#ifdef SUPPORT_NET_ASCII
            else if (uStrEquiv(cNetAscii, Data) > 0) {                   // {1}
                return (TFTP_NET_ASCII_MODE);
            }
#endif
            break;
        }
    }  
    return TFTP_STATE_IDLE;                                              // invalid
}

static void fnSendTFTP_ack(void)
{
    UDP_TFTP_ACK tftp_frame;                                             // space for TFTP UDP frame

    tftp_frame.ucUDP_Message[0] = (unsigned char)(TFTP_OPCODE_ACK >> 8); // add op code and block count
    tftp_frame.ucUDP_Message[1] = (unsigned char)(TFTP_OPCODE_ACK);
    tftp_frame.ucUDP_Message[2] = (unsigned char)(tftp_block_number >> 8);
    tftp_frame.ucUDP_Message[3] = (unsigned char)(tftp_block_number);
    fnSendUDP(TFTPSocket, tftp_client_ip, tftp_client_port, (unsigned char *)&tftp_frame.tUDP_Header, sizeof(tftp_frame.ucUDP_Message), 0);
    uTaskerMonoTimer( OWN_TASK, TFTP_TIMEOUT_PERIOD, TFTP_TIMEOUT );     // monitor arrival of next block
}

static void fnSendTFTP_error(unsigned short usTFTP_error)
{
    UDP_TFTP_ERR tftp_frame;                                             // space for TFTP UDP frame

    tftp_frame.ucUDP_Message[0] = (unsigned char)(TFTP_OPCODE_ERROR >> 8); // add op code and error number
    tftp_frame.ucUDP_Message[1] = (unsigned char)(TFTP_OPCODE_ERROR);
    tftp_frame.ucUDP_Message[2] = (unsigned char)(usTFTP_error >> 8);
    tftp_frame.ucUDP_Message[3] = (unsigned char)(usTFTP_error);
    uStrcpy(tftp_frame.cErrorStr, tftp_error_message);                   // add fixed error text
    fnSendUDP(TFTPSocket, tftp_client_ip, tftp_client_port, (unsigned char *)&tftp_frame.tUDP_Header, (sizeof(tftp_frame.ucUDP_Message) + sizeof(tftp_frame.cErrorStr)), 0);
}

static void fnSendTFTP_read_write(unsigned char ucReadWrite, CHAR *cFile)
{
    unsigned short usOpCode;
    CHAR *ptrMessage;
    UDP_TFTP_MESSAGE tftp_frame;                                         // space for TFTP UDP frame

    if (ucReadWrite == TFTP_PUT) {                                       // put
        usOpCode = TFTP_OPCODE_WRQ;                                      // set write request
        ucTFTP_state = (TFTP_STATE_CLIENT | TFTP_STATE_CLIENT_START | TFTP_STATE_READING);
    }
    else {                                                               // get 
        usOpCode = TFTP_OPCODE_RRQ;                                      // set read request
        if (TFTP_GET_COMPARE == ucReadWrite) {                           // if we are not to save, only to compare
            ucTFTP_state = (TFTP_STATE_CLIENT | TFTP_STATE_CLIENT_START | TFTP_STATE_WRITING | TFTP_STATE_CHECKING);
        }
        else {
            ucTFTP_state = (TFTP_STATE_CLIENT | TFTP_STATE_CLIENT_START | TFTP_STATE_WRITING);
        }
    }

    tftp_frame.ucUDP_Message[0] = (unsigned char)(usOpCode >> 8);        // add op code    
    tftp_frame.ucUDP_Message[1] = (unsigned char)(usOpCode);
    ptrMessage = uStrcpy((CHAR *)&tftp_frame.ucUDP_Message[2], cFile);   // add the file name
#ifdef STRING_OPTIMISATION                                               // {3}
    ptrMessage++;
#endif
    ptrMessage = uStrcpy(ptrMessage, cOctet);                            // add the mode (only octet mode supported)
#ifdef STRING_OPTIMISATION                                               // {3}
    ptrMessage++;
#endif
    fnSendUDP(TFTPSocket, tftp_client_ip, tftp_client_port, (unsigned char *)&tftp_frame.tUDP_Header, (unsigned short)(ptrMessage - (CHAR *)tftp_frame.ucUDP_Message), OWN_TASK);
}

// Send a block of data
// 
static void fnSendTFTP_block(int iRepeat)
{
    MAX_FILE_LENGTH ThisBlockLength = tftp_transfer_length;
    UDP_TFTP_MESSAGE tftp_frame;                                         // space for TFTP UDP frame

    if (tftp_transfer_length > FULL_TFTP_FRAME) {
        ThisBlockLength = FULL_TFTP_FRAME;                               // maximum TFTP frame size
    }

    fnGetParsFile(tftp_file_pointer, &tftp_frame.ucUDP_Message[4], ThisBlockLength);

    if (iRepeat != REPEAT_BLOCK) {
        tftp_block_number++;
        ucBlockRepeats = 0;
        if (iRepeat == SUCCESSFUL_ACK) {                                 // on acks advance to next block
            if (!tftp_transfer_length) {                                 // if ack to final block
                fnTFTP_error(TFTP_TRANSFER_READ_COMPLETE, 0);            // reset and inform application of success
                return;
            }
            tftp_file_pointer += ThisBlockLength;                        // prepare next block
            tftp_transfer_length -= ThisBlockLength;
        }
    }
    tftp_frame.ucUDP_Message[0] = (unsigned char)(TFTP_OPCODE_DATA >> 8);// add op code and block count
    tftp_frame.ucUDP_Message[1] = (unsigned char)(TFTP_OPCODE_DATA);
    tftp_frame.ucUDP_Message[2] = (unsigned char)(tftp_block_number >> 8);
    tftp_frame.ucUDP_Message[3] = (unsigned char)(tftp_block_number);
    fnSendUDP(TFTPSocket, tftp_client_ip, tftp_client_port, (unsigned char *)&tftp_frame.tUDP_Header, (unsigned short)(ThisBlockLength + 4), 0);

    uTaskerMonoTimer( OWN_TASK, TFTP_ACK_PERIOD, TFTP_ACK_TIMEOUT );     // monitor the block's ack
}


// The local TFTP listener function - this will call the client listener if needed
//
static int fnTFTPListner(USOCKET tftp_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen)
{
    unsigned short usOpCode;
  
    if (usDataLen < 2) {
        if (UDP_EVENT_PORT_UNREACHABLE == ucEvent) {
            fnTFTP_error(TFTP_DESTINATION_UNREACHABLE, 0);               // no UDP port listening on server
        }
        return (-1);                                                     // invalid length
    }

    if (ucTFTP_state != TFTP_STATE_IDLE) {                               // if communicating ensure that only the correct client is handled
        if ((!(ucTFTP_state & TFTP_STATE_CLIENT_START) && (usPort != tftp_client_port)) || (uMemcmp(ucIP, tftp_client_ip, IPV4_LENGTH))) {
            return -1;                                                   // ignore
        }
        tftp_client_port = usPort;                                       // update to client port number on first message
        ucTFTP_state &= ~TFTP_STATE_CLIENT_START;
    }
    else if (!(ucTFTP_state & TFTP_STATE_CLIENT) && (!uMemcmp(ucIP, cucBroadcast, IPV4_LENGTH))) { // {2} - uMemcpy changed to uMemcmp
        return (-1);                                                     // server not allowed to accept a broadcast address
    }

    usOpCode = *ucData++;                                                // extract the op code
    usOpCode <<= 8;
    usOpCode |= *ucData++;
    usDataLen -= 2;

    switch (usOpCode) {
    case TFTP_OPCODE_RRQ:                                                // request to read data
        // fall through intentionally
    case TFTP_OPCODE_WRQ:
        if (ucTFTP_state & TFTP_STATE_CLIENT) {                          // in client mode ignore these op codes
            return -1;
        }
        ucTFTP_state = fnGetFileMode((CHAR *)ucData, usDataLen);         // extract the file name and mode 
        if (ucTFTP_state == TFTP_STATE_IDLE) {                           // invalid read or write
            return -1;
        }
        tftp_client_port = usPort;                                       // remember communication partner details
        uMemcpy(tftp_client_ip, ucIP, IPV4_LENGTH);
        tftp_block_number = 0;
        tftp_next_file = tftp_file_pointer = uOpenFile((CHAR *)ucData);
        tfpt_write_length = 0;
#ifdef SUPPORT_MIME_IDENTIFIER
        ucMimeType = fnGetMimeType((CHAR *)ucData);                      // get the type of file being saved
#endif
#ifdef SUB_FILE_SIZE                                                     // {4}
        ucSubFileInProgress = fnGetFileType((CHAR *)ucData);             // get file characteristics so that it is later handled correctly
#endif
        if (usOpCode == TFTP_OPCODE_WRQ) {
            ucTFTP_state |= TFTP_STATE_WRITING;
            fnSendTFTP_ack();                                            // answer with an ack
        }
        else {
            tftp_transfer_length = uGetFileLength(tftp_file_pointer);    // the length to be transfered
            if (!tftp_transfer_length) {
                fnSendTFTP_error(TFTP_FILE_NOT_FOUND);                   // the requested file doesn't exist
                fnTFTP_error((TFTP_CLIENT_ERROR | TFTP_FILE_NOT_FOUND), (CHAR *)ucData);
                break;
            }
            ucTFTP_state |= TFTP_STATE_READING;
            fnSendTFTP_block(FIRST_BLOCK_TX);                            // first block transmission
        }
        break;

    case TFTP_OPCODE_DATA:
        if (ucTFTP_state & TFTP_STATE_WRITING) {
            usOpCode = *ucData++;                                        // extract the block number
            usOpCode <<= 8;
            usOpCode |= *ucData++;            
            usDataLen -= 2;                                              // the length of data
            if (usOpCode == tftp_block_number) {                         // repeated block - acknowledge it but do nothing with it
                fnSendTFTP_ack();                                        // acknowledge that data has been received and saved
                break;
            }
            if (usOpCode == (tftp_block_number + 1)) {                   // check that block is the next
                if ((tftp_next_file) && ((tftp_file_pointer + usDataLen) >= tftp_next_file)) { // we need to ensure any files in the write space have been deleted
#ifdef SUPPORT_MIME_IDENTIFIER
                    unsigned char ucMimeType;
#endif
                    tftp_transfer_length = uGetFileLength(tftp_next_file); // the length of existing file
                    if (tftp_transfer_length != 0) {
                        if (tftp_transfer_length < FULL_TFTP_FRAME) {
                            tftp_transfer_length = FULL_TFTP_FRAME;      // ensure we make enough space
                        }
                        if (!(TFTP_STATE_CHECKING & ucTFTP_state)) {     // if not comparing file content
                            uFileErase(tftp_next_file, tftp_transfer_length);// clear the next file in the way                                                                         
                        }
                    }
#ifdef SUPPORT_MIME_IDENTIFIER
                    tftp_next_file = uOpenNextMimeFile(tftp_next_file, &tftp_transfer_length, &ucMimeType SUBFILE_WRITE); // {5} since we don't know the length of data which we are to receive we have to monitor the files which may be in the way and also the maximum length possible
#else
                    tftp_next_file = uOpenNextFile(tftp_next_file, &tftp_transfer_length SUBFILE_WRITE); // since we don't know the length of data which we are to receive we have to monitor the files which may be in the way and also the maximum length possible
#endif
                }

                if (((tfpt_write_length + usDataLen + FILE_HEADER) < tfpt_write_length) || (tftp_file_pointer >= ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER)) - usDataLen - tfpt_write_length)) {
                    // If there is no more space in FLASH we close the file, delete its contents and inform of an error
                    // This can be because the write is reaching the end of the physical memory or because the file size is increasing to beyond the maximum supported size
#ifdef SUPPORT_MIME_IDENTIFIER
                    tftp_transfer_length = uFileCloseMime(tftp_file_pointer, &ucMimeType); // close complete received file
#else
                    tftp_transfer_length = uFileClose(tftp_file_pointer); // close complete received file
#endif
                    if (!(TFTP_STATE_CHECKING & ucTFTP_state)) {         // if not comparing file content
                        uFileErase(tftp_file_pointer, tftp_transfer_length); // delete the corrupted file 
                    }
                    fnSendTFTP_error(TFTP_DISK_FULL);                    // the file won't fit
                    fnTFTP_error((TFTP_CLIENT_ERROR | TFTP_DISK_FULL), 0);
                    break;
                }
                if (TFTP_STATE_CHECKING & ucTFTP_state) {                // if comparing file content
#ifdef SUPPORT_FILE_COMPARE
                    if (uCompareFile((tftp_file_pointer + tfpt_write_length + FILE_HEADER), ucData, usDataLen)) {
                        fnSendTFTP_error(TFTP_DISK_FULL);                // stop present transfer (with error message)
                        fnTFTP_error(TFTP_FILE_NOT_EQUAL, 0);            // inform the application that the file content is different
                        break;
                    }
#endif
                }
                else {
                    uFileWrite(tftp_file_pointer, ucData, usDataLen SUBFILE_WRITE); // save the received data to file. Existing files will automatically be deleted
                }
                tfpt_write_length += usDataLen;                          // length of data written
                if (usDataLen < FULL_TFTP_FRAME) {                       // have we just received the last frame?
#ifdef SUPPORT_MIME_IDENTIFIER
                    uFileCloseMime(tftp_file_pointer, &ucMimeType);      // close complete received file
#else
                    uFileClose(tftp_file_pointer);                       // close complete received file
#endif
                    ucTFTP_state |= TFTP_STATE_TERMINATING;              // mark that we are in the termination phase
                    // Note that we don't close the virtual connection since if our last ack were to get lost the client would repeat and expect a response
                    // Instead we have a short monitor timer still running from the ack transmission. Once this times out we terminate completely
                    if (TFTP_STATE_CHECKING & ucTFTP_state) {
                        if (uGetFileLength(tftp_file_pointer) == tfpt_write_length) { // last check on file length
                            fnTFTP_error(TFTP_FILE_EQUALITY, 0);         // immediately inform that the file is identical
                        }
                        else {
                            fnTFTP_error(TFTP_FILE_NOT_EQUAL, 0);        // inform the application that the file content is different
                        }
                    }
                }
                tftp_block_number++;
                fnSendTFTP_ack();                                        // acknowledge that data has been received and saved
            }
        }
        break;

    case TFTP_OPCODE_ACK:
        if (ucTFTP_state & TFTP_STATE_READING) {                         // last block was successfully received
            usOpCode = *ucData++;                                        // extract the block number
            usOpCode <<= 8;
            usOpCode |= *ucData;
            if (usOpCode == tftp_block_number) {                         // only accept ACKs to the expected block number
                fnSendTFTP_block(SUCCESSFUL_ACK);                        // block transmission
            }
        }
        break;

    case TFTP_OPCODE_ERROR:
        usOpCode = *ucData++;                                            // extract the error code
        usOpCode <<= 8;
        usOpCode |= *ucData++;
        fnTFTP_error(usOpCode, (CHAR *)ucData);
        break;
    }

    return 0;
}


static void fnTFTP_error(unsigned short usError, CHAR *error_message)
{
    uTaskerStopTimer(OWN_TASK);                                          // kill all monitoring
    ucTFTP_state = TFTP_STATE_IDLE;                                      // go back to idle state
    if (fnApplicationCallback) {
        fnApplicationCallback(usError, error_message);
    }
}

#endif
