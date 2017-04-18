/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      can_tests.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    The file is otherwise not specifically linked in to the project since it
    is included by application.c when needed.

    03.02.2011 Renamed fnInitCAN() to fnInitCANInterface()               {1}
    14.12.2011 Construct CAN test messages to avoid compiler using memcpy() {2}
    23.12.2011 Add two port CAN test when two CAN controllers are present {3}
    27.12.2011 Add fnSendCAN_message() for general CAN message transmission {4}

*/

#if defined CAN_INTERFACE && !defined _CAN_CONFIG
    #define _CAN_CONFIG

  //#define TEST_CAN                                                     // test CAN operation
      //#define PARTNER_CAN                                              // quick set of CAN partner addresses (use this when testing between two boards running the same test - one with and one without this)
      //#define TEST_FIRST_CAN_ONLY

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

    #if defined CAN_INTERFACE && defined TEST_CAN
        static void fnInitCANInterface(void);
        static void fnSendCAN(int key);
    #endif


/* =================================================================== */
/*                     local variable definitions                      */
/* =================================================================== */

    #if defined CAN_INTERFACE && defined TEST_CAN
        static QUEUE_HANDLE CAN_interface_ID0;
        #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1) // {3}
            static QUEUE_HANDLE CAN_interface_ID1;
        #endif
    #endif

#endif


// CAN interrupt event handling
//
#if defined _CAN_INT_EVENTS && defined CAN_INTERFACE && defined TEST_CAN
            case CAN_TX_REMOTE_ERROR:
                fnDebugMsg("CAN TX REMOTE ERROR: ");
                Length = fnRead(CAN_interface_ID0, ucInputMessage, GET_CAN_TX_REMOTE_ERROR); // read error
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)   // {3}
                if (Length == 0) {
                    fnDebugMsg("[1]");
                    Length = fnRead(CAN_interface_ID1, ucInputMessage, GET_CAN_TX_REMOTE_ERROR);
                }
    #endif
                if (Length != 0) {
                    int i = 0;
                    while (Length--) {                                   // display received message
                        fnDebugHex(ucInputMessage[i++], (WITH_LEADIN | WITH_TERMINATOR | WITH_SPACE | 1));
                    }
                }
                fnDebugMsg("\r\n");
                break;

            case CAN_TX_ERROR:                                           // no response to a message we sent
                fnDebugMsg("CAN TX ERROR: ");
                Length = fnRead(CAN_interface_ID0, ucInputMessage, GET_CAN_TX_ERROR); // read error
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)   // {3}
                if (Length == 0) {
                    fnDebugMsg("[1]");
                    Length = fnRead(CAN_interface_ID1, ucInputMessage, GET_CAN_TX_ERROR);
                }
    #endif
                if (Length != 0) {
                    int i = 0;
                    while (Length--) {                                   // display received message
                        fnDebugHex(ucInputMessage[i++], (WITH_LEADIN | WITH_TERMINATOR | WITH_SPACE | 1));
                    }
                }
                fnDebugMsg("\r\n");
                break;

            case CAN_OTHER_ERROR:                                        // other non-categorised error
                fnDebugMsg("CAN 2\r\n");
                break;

            case CAN_TX_ACK:                                             // a CAN message was successfully sent
                fnDebugMsg("CAN TX OK\r\n");
                break;

            case CAN_TX_REMOTE_ACK:                                      // a remote CAN message was successfully sent
                fnDebugMsg("CAN remote TX OK\r\n");
                break;

            case CAN_RX_REMOTE_MSG:
                Length = fnRead(CAN_interface_ID0, ucInputMessage, (GET_CAN_RX_REMOTE | GET_CAN_RX_TIME_STAMP | GET_CAN_RX_ID)); // read received CAN message
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)   // {3}
                if (Length == 0) {
                    fnDebugMsg("[1] ");
                    Length = fnRead(CAN_interface_ID1, ucInputMessage, (GET_CAN_RX_REMOTE | GET_CAN_RX_TIME_STAMP | GET_CAN_RX_ID));
                }
                else {
                    fnDebugMsg("[0] ");
                }
    #endif
                // Fall through intentional
                //
            case CAN_RX_MSG:                                             // a CAN message is waiting
                if (ucInputMessage[MSG_INTERRUPT_EVENT] == CAN_RX_MSG) {
                    Length = fnRead(CAN_interface_ID0, ucInputMessage, (GET_CAN_RX_TIME_STAMP | GET_CAN_RX_ID)); // read received CAN message
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)   // {3}
                    if (Length == 0) {
                        fnDebugMsg("[1] ");
                        Length = fnRead(CAN_interface_ID1, ucInputMessage, (GET_CAN_RX_TIME_STAMP | GET_CAN_RX_ID));
                    }
                    else {
                        fnDebugMsg("[0] ");
                    }
    #endif
                }
                if (Length != 0) {
                    int i = 0;
                    unsigned long ulID;
                    unsigned short usTimeStamp;

                    if (ucInputMessage[i] & CAN_MSG_RX) {
                        fnDebugMsg("CAN RX");
                    }
                    else {
                        fnDebugMsg("CAN REMOTE RX");
                    }

                    if (ucInputMessage[i++] & CAN_RX_OVERRUN) {
                        fnDebugMsg(" [OVERRUN!!]");
                    }

                    fnDebugMsg(": TimeStamp = ");
                    usTimeStamp = ucInputMessage[i++];
                    usTimeStamp <<= 8;
                    usTimeStamp |= ucInputMessage[i++];
                    fnDebugHex(usTimeStamp, (WITH_LEADIN | WITH_TERMINATOR | 2));

                    fnDebugMsg(" ID = ");

                    ulID = ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];

                    if (ulID & CAN_EXTENDED_ID) {
                        fnDebugHex((ulID & ~CAN_EXTENDED_ID), (WITH_LEADIN | WITH_TERMINATOR | 4));
                    }
                    else {
                        fnDebugHex(ulID, (WITH_LEADIN | WITH_TERMINATOR | 2));
                    }

                    if (Length > 7) {
                        Length -= 7;                                     // remove info to leave data length
                        fnDebugMsg(" Data =");

                        while (Length--) {                               // display received message
                            fnDebugHex(ucInputMessage[i++], (WITH_LEADIN | WITH_TERMINATOR | WITH_SPACE | 1));
                        }
                    }
                    else {
                        fnDebugMsg(" No Data");
                    }
                }
                fnDebugMsg("\r\n");
                break;
#endif



#if defined _CAN_INIT_CODE && defined CAN_INTERFACE && defined TEST_CAN
static void fnInitCANInterface(void)                                     // {1}
{
    CANTABLE tCANParameters;                                             // table for passing information to driver

    tCANParameters.Task_to_wake = OWN_TASK;                              // wake us on buffer events
    tCANParameters.Channel = 0;                                          // CAN0 interface
    tCANParameters.ulSpeed = 1000000;                                    // 1 Meg speed
  //tCANParameters.ulSpeed = 40000;                                      // slow speed for test
    #ifdef PARTNER_CAN
    tCANParameters.ulTxID = (CAN_EXTENDED_ID | 0x00000105);              // our partner's ID on the bus (extended)
    tCANParameters.ulRxID = 0x102;                                       // our standard rx ID on the bus (not extended)
    tCANParameters.ulRxIDMask = CAN_STANDARD_MASK;                       // use all standard bits for compare
    #else
    tCANParameters.ulTxID = 0x102;                                       // default ID of destination (not extended)
    tCANParameters.ulRxID = (CAN_EXTENDED_ID | 0x00000105);              // our ID (extended)
    tCANParameters.ulRxIDMask = CAN_EXTENDED_MASK;
    #endif
                                                                         // use all bits for compare
    tCANParameters.usMode = 0;                                           // use normal mode
    tCANParameters.ucTxBuffers = 2;                                      // assign two tx buffers for use
    tCANParameters.ucRxBuffers = 3;                                      // assign three rx buffers for use
    CAN_interface_ID0 = fnOpen(TYPE_CAN, FOR_I_O, &tCANParameters);      // open interface

    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)   // {3}
    tCANParameters.Channel = 1;                                          // CAN1 interface
        #ifdef PARTNER_CAN
    tCANParameters.ulTxID = 0x102;                                       // default ID of destination (not extended)
    tCANParameters.ulRxID = (CAN_EXTENDED_ID | 0x00000105);              // our ID (extended)
    tCANParameters.ulRxIDMask = CAN_EXTENDED_MASK;
        #else
    tCANParameters.ulTxID = (CAN_EXTENDED_ID | 0x00000105);              // our partner's ID on the bus (extended)
    tCANParameters.ulRxID = 0x102;                                       // our standard rx ID on the bus (not extended)
    tCANParameters.ulRxIDMask = CAN_STANDARD_MASK;                       // use all standard bits for compare
        #endif
    CAN_interface_ID1 = fnOpen(TYPE_CAN, FOR_I_O, &tCANParameters);      // open interface
    #endif
}

// Test interface for sending CAN messages to the specified channel
//
extern void fnSendCAN_message(int iChannel, unsigned char ucType, unsigned char *ptrData, unsigned char ucMessageLength) // {4}
{
    QUEUE_HANDLE CAN_interface = CAN_interface_ID0;
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)
    if (iChannel == 1) {
        CAN_interface = CAN_interface_ID1;
    }
    #endif

    if (TX_REMOTE_STOP & ucType) {
        if (fnWrite(CAN_interface, ptrData, (QUEUE_TRANSFER)(ucMessageLength | ucType)) == 0) {
            fnDebugMsg("No remote message found\r\n");
        }
        else {
            fnDebugMsg("Remote message cancelled\r\n");
        }
        return;
    }
    else if (FREE_CAN_RX_REMOTE & ucType) {
        if (fnRead(CAN_interface, 0, (QUEUE_TRANSFER)(ucType)) == 0) {
            fnDebugMsg("No Remote buffer found\r\n");
        }
        else {
            fnDebugMsg("Remote buffer freed\r\n");
        }
        return;
    }
    
    if (fnWrite(CAN_interface, ptrData, (QUEUE_TRANSFER)(ucMessageLength | ucType)) == ucMessageLength) {
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)
        fnDebugMsg("\nCAN ");
        fnDebugDec(iChannel, 0);
        fnDebugMsg(" sent ");
    #else
        fnDebugMsg("\nCAN sent ");
    #endif
        if (ucType & TX_REMOTE_FRAME) {
            fnDebugMsg("to remote queue ");
        }
        else {
            if (ucType & SPECIFIED_ID) {
                unsigned long ulID = *ptrData++;
                ulID <<= 8;
                ulID |= *ptrData++;
                ulID <<= 8;
                ulID |= *ptrData++;
                ulID <<= 8;
                ulID |= *ptrData++;
                if (ulID & CAN_EXTENDED_ID) {
                    ulID &= ~CAN_EXTENDED_ID;
                    fnDebugMsg("to extended ID");
                    fnDebugHex(ulID, (sizeof(ulID) |  WITH_LEADIN | WITH_SPACE));
                }
                else {
                    fnDebugMsg("to ID");
                    fnDebugHex(ulID, (2 |  WITH_LEADIN | WITH_SPACE));
                }
                ucMessageLength -= 4;
            }
            else {
                fnDebugMsg("to default ID");
            }
        }
        fnDebugMsg(":");
        while (ucMessageLength--) {
            fnDebugHex(*ptrData++, (sizeof(*ptrData) |  WITH_LEADIN | WITH_SPACE));
        }
        fnDebugMsg("\r\n");
    }
    else {
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)
        fnDebugMsg("\nCAN ");
        fnDebugDec(iChannel, 0);
        fnDebugMsg(" not sent\r\n");
    #else
        fnDebugMsg("\nCAN not sent\r\n");
    #endif
    }
}

static void fnSendCAN(int key)
{
    unsigned char ucTestMessage[8];                                      // {2}
    int i = 0;
    if (key == 1) {
        while (i < 8) {
            ucTestMessage[i] = (i + 1);                                  // test message 1,2,3,4,5,6,7,8
            i++;
        }
        fnSendCAN_message(0, CAN_TX_ACK_ON, ucTestMessage, (unsigned char)sizeof(ucTestMessage)); // send data to the default ID with acknowledge of successful delievery
    }
    else if (key == 7) {
        while (i < 8) {
            ucTestMessage[i] = (8 - i);                                  // test message 8,7,6,5,4,3,2,1
            i++;
        }
    #if !defined TEST_FIRST_CAN_ONLY && (NUMBER_OF_CAN_INTERFACES > 1)   // {3}
        fnSendCAN_message(1, CAN_TX_ACK_ON, ucTestMessage, (unsigned char)sizeof(ucTestMessage)); // send data to the default ID with acknowledge of successful delievery
    #else
        fnSendCAN_message(0, CAN_TX_ACK_ON, ucTestMessage, (unsigned char)sizeof(ucTestMessage));
    #endif
    }
}
#endif

