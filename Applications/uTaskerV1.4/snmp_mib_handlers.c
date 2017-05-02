/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        snmp_mib_handlers.c
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file contains standard SNMP MIB-II handlers for get and set requests
    - each function handler is preceded by its description from rfc 1213
    - since this file is included from snmp.c it should be excluded from the build
    - the MIB table mib_table[] is declared at the bottom of the file

    16.04.2014 Modify SNMP_GET_VARIABLE_BINDING handling                 {1}
    31.01.2015 Updated callback to include community read and write checks as well as revised trap usage
    02.02.2017 Adapt for us tick resolution                              {2}

*/

#include "config.h"

#if defined USE_SNMP

static SNMP_MANAGER SNMP_manager_details[SNMP_MANAGER_COUNT] = {{{0}}};
static const CHAR cSNMP_communityRead[] = "public";
static const CHAR cSNMP_communityWrite[] = "private";
static const CHAR cSNMP_enterprise[] = {6,1,4,1,5};                      // 1.3. (iso.org.) is assumed so we add only the rest
static int fnSNMP_callback(unsigned char ucEvent, unsigned char *data, unsigned short usLength, int iManagerReference);


static int fnSNMP_callback(unsigned char ucEvent, unsigned char *data, unsigned short usLengthReference, int iManagerReference)
{
    switch (ucEvent) {
    case SNMP_COMMUNITY_CHECK_WRITE:
        if (usLengthReference != uStrlen(cSNMP_communityWrite)) {
            return SNMP_COMMUNITY_MISMATCH;                              // length mismatch
        }
        if (uMemcmp(cSNMP_communityWrite, data, usLengthReference) != 0) { // check that the SNMP request belongs to our community
            return SNMP_COMMUNITY_MISMATCH;                              // not recognised as community member
        }
        return SNMP_COMMUNITY_READ_WRITE;                                // allow read write access in this community
    case SNMP_COMMUNITY_CHECK_READ:                                      // check that a received community name matches the one expected
        if (usLengthReference != uStrlen(cSNMP_communityRead)) {
            return SNMP_COMMUNITY_MISMATCH;                              // length mismatch
        }
        if (uMemcmp(cSNMP_communityRead, data, usLengthReference) != 0) { // check that the SNMP request belongs to our community
            return SNMP_COMMUNITY_MISMATCH;                              // not recognised as community member
        }
        return SNMP_COMMUNITY_READ_ONLY;                                 // allow read only access in this community
    case SNMP_GET_COMMUNITY:                                             // return our community name string
        {
            CHAR *ptrBuf = uStrcpy((CHAR *)data, cSNMP_communityRead);   // add community details to SNMP trap
            return (ptrBuf - (CHAR *)data);
        }

    case SNMP_GET_ENTERPRISE:                                            // add enterprise details to SNMP trap
        uMemcpy(data, cSNMP_enterprise, (sizeof(cSNMP_enterprise)));
        return (sizeof(cSNMP_enterprise));

    case SNMP_GET_ERROR_STATUS:                                          // return single byte for error status
    case SNMP_GET_ERROR_INDEX:                                           // return single byte for error index
        return 0;

    case SNMP_GET_VARIABLE_BINDING:                                      // the user can add variable binding here based on the reference requested
        {                                                                // sysUpTime and snmpTrapOID are automatically added to SNMPv2-Trap-PDUs
            // User defined additional variable bindings (ucVariableBindingNumber counts 0, 1, 2 etc. to add multiple variable bindings)
            //
            switch (usLengthReference & 0xff) {                          // trap type
            case SNMP_ENTERPRISE_SPECIFIC:                               // handle alarms
                break;
            }
        }
        break;
    }
    return 0;
}



/*
    sysDescr OBJECT-TYPE
        SYNTAX  DisplayString (SIZE (0..255))
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "A textual description of the entity.  This value
                should include the full name and version
                identification of the system's hardware type,
                software operating-system, and networking
                software.  It is mandatory that this only contain
                printable ASCII characters."
*/
static int fn_sysDescr(MIB_CONTROL *ptrMIB_control)
{
    ptrMIB_control->ucObjectType = ASN1_OCTET_STRING_CODE;               // string type
    ptrMIB_control->ptrString = "uTasker system";                        // fixed string to be returned (null terminated)
    return 0;                                                            // could be handled
}

/*
    sysObjectID OBJECT-TYPE
        SYNTAX  OBJECT IDENTIFIER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The vendor's authoritative identification of the
                network management subsystem contained in the
                entity.  This value is allocated within the SMI
                enterprises subtree (1.3.6.1.4.1) and provides an
                easy and unambiguous means for determining `what
                kind of box' is being managed.  For example, if
                vendor `Flintstones, Inc.' was assigned the
                subtree 1.3.6.1.4.1.4242, it could assign the
                identifier 1.3.6.1.4.1.4242.1.1 to its `Fred
                Router'."
*/
static int fn_sysObjectID(MIB_CONTROL *ptrMIB_control)
{
    ptrMIB_control->ucObjectType = ASN1_OBJECT_IDENT_CODE;
    ptrMIB_control->ptrString = /*1.3.*/"6.1.4.1.1602.4.7";              // 1.3. (iso.org) is assumed and should not be added
    return 0;                                                            // could be handled
}

/*
    sysUpTime OBJECT-TYPE
        SYNTAX  TimeTicks
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The time (in hundredths of a second) since the
                network management portion of the system was last
                re-initialized."
*/
static int fn_sysUpTime(MIB_CONTROL *ptrMIB_control)
{
    unsigned long ulHundredths = uTaskerSystemTick;
    #if TICK_RESOLUTION >= 1000                                          // {2}
    ulHundredths *= (TICK_RESOLUTION/1000);
    #else
    ulHundredths /= (1000/TICK_RESOLUTION);
    #endif
    ulHundredths /= 100;                                                 // up time in 100th of second
    ptrMIB_control->ucObjectType = ASN1_TIME_STAMP_CODE;                 // time ticks type
    ptrMIB_control->ulInteger = ulHundredths;
    return 0;                                                            // could be handled
}

/*
    sysContact OBJECT-TYPE
        SYNTAX  DisplayString (SIZE (0..255))
        ACCESS  read-write
        STATUS  mandatory
        DESCRIPTION
                "The textual identification of the contact person
                for this managed node, together with information
                on how to contact this person."
*/
static int fn_sysContact(MIB_CONTROL *ptrMIB_control)
{
    ptrMIB_control->ucObjectType = ASN1_OCTET_STRING_CODE;               // string type
    ptrMIB_control->ptrString = 0;                                       // if we set a null string pointer it means that a string of length zero is returned as response
    return 0;                                                            // could be handled
}

/*
    sysName OBJECT-TYPE
        SYNTAX  DisplayString (SIZE (0..255))
        ACCESS  read-write
        STATUS  mandatory
        DESCRIPTION
                "An administratively-assigned name for this
                managed node.  By convention, this is the node's
                fully-qualified domain name."
*/
static int fn_sysName(MIB_CONTROL *ptrMIB_control)
{
    unsigned char ucLength;
    ptrMIB_control->ucObjectType = ASN1_OCTET_STRING_CODE;               // string type
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_SET_REQUEST) {        // setting a new name
        fnCopyASN1_string(temp_pars->temp_parameters.cDeviceIDName, ptrMIB_control->ptrContent, sizeof(temp_pars->temp_parameters.cDeviceIDName)); // copy the string received
    }
    ptrMIB_control->ptrString = fnGetDHCP_host_name(&ucLength, 0);       // fixed string to be returned (null terminated)
    return 0;                                                            // could be handled
}
/*
    sysLocation OBJECT-TYPE
        SYNTAX  DisplayString (SIZE (0..255))
        ACCESS  read-write
        STATUS  mandatory
        DESCRIPTION
                "The physical location of this node (e.g.,
                `telephone closet, 3rd floor')."
*/
static int fn_sysLocation(MIB_CONTROL *ptrMIB_control)
{
    ptrMIB_control->ucObjectType = ASN1_OCTET_STRING_CODE;               // string type
    ptrMIB_control->ptrString = 0;                                       // if we set a null string pointer it means that a string of length zero is returned as response
    return 0;                                                            // could be handled
}

/*
    sysServices OBJECT-TYPE
        SYNTAX  INTEGER (0..127)
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "A value which indicates the set of services that
                this entity primarily offers.

                The value is a sum.  This sum initially takes the
                value zero, Then, for each layer, L, in the range
                1 through 7, that this node performs transactions
                for, 2 raised to (L - 1) is added to the sum.  For
                example, a node which performs primarily routing
                functions would have a value of 4 (2^(3-1)).  In
                contrast, a node which is a host offering
                application services would have a value of 72
                (2^(4-1) + 2^(7-1)).  Note that in the context of
                the Internet suite of protocols, values should be
                calculated accordingly:

                     layer  functionality
                         1  physical (e.g., repeaters)
                         2  datalink/subnetwork (e.g., bridges)
                         3  internet (e.g., IP gateways)
                         4  end-to-end  (e.g., IP hosts)
                         7  applications (e.g., mail relays)

                For systems including OSI protocols, layers 5 and
                6 may also be counted."
*//*
static int fn_sysServices(MIB_CONTROL *ptrMIB_control)
{
    #define FUNCTIONALITY_PHYSICAL              0x01
    #define FUNCTIONALITY_DATALINK_SUBNETWORKS  0x02
    #define FUNCTIONALITY_INTERNET              0x04
    #define FUNCTIONALITY_END_TO_END            0x08
    #define FUNCTIONALITY_APPLICATIONS          0x40

    ptrMIB_control->ucObjectType = ASN1_INTEGER;
    ptrMIB_control->ulInteger = (FUNCTIONALITY_DATALINK_SUBNETWORKS | FUNCTIONALITY_INTERNET);
    return 0;                                                            // could be handled
}*/


/*
    ifNumber OBJECT-TYPE
        SYNTAX  INTEGER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of network interfaces (regardless of
                their current state) present on this system."
*/
static int fn_ifNumber(MIB_CONTROL *ptrMIB_control)
{
    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = IP_INTERFACE_COUNT;
    return 0;                                                            // could be handled
}


/*
    ifIndex OBJECT-TYPE
        SYNTAX  INTEGER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "A unique value for each interface.  Its value
                ranges between 1 and the value of ifNumber.  The
                value for each interface must remain constant at
                least from one re-initialization of the entity's
                network management system to the next re-
                initialization."

*/
static int fn_ifIndex(MIB_CONTROL *ptrMIB_control)                       // table type
{
    unsigned char ucInterfaceReference = ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength];
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    else {
        if (ucInterfaceReference == 0) {                                  // handle 0 and 1 as the same - interface reference starts at 1
            ucInterfaceReference = 1;
        }
    }
    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = ucInterfaceReference;                    // interface number
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index
    return 0;                                                            // could be handled
}

/*
    ifDescr OBJECT-TYPE
        SYNTAX  DisplayString (SIZE (0..255))
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "A textual string containing information about the
                interface.  This string should include the name of
                the manufacturer, the product name and the version
                of the hardware interface."
*/
static int fn_ifDescr(MIB_CONTROL *ptrMIB_control)                       // table type
{
    CHAR *ptrInterfaceDescriptions[IP_INTERFACE_COUNT] = {"Eth. 1"};
    unsigned char ucInterfaceReference = ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength]; // request for the next table entry
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {          
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    else {
        if (ucInterfaceReference == 0) {                                 // handle 0 and 1 as the same - interface reference starts at 1
            ucInterfaceReference = 1;
        }
    }
    ptrMIB_control->ucObjectType = ASN1_OCTET_STRING_CODE;               // string type
    ptrMIB_control->ptrString = ptrInterfaceDescriptions[ucInterfaceReference - 1]; // if we set a null string pointer it means that a string of length zero is returned as response
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index
    return 0;                                                            // could be handled
}

/*
    ifType OBJECT-TYPE
        SYNTAX  INTEGER {
                    other(1),          -- none of the following
                    regular1822(2),
                    hdh1822(3),
                    ddn-x25(4),
                    rfc877-x25(5),
                    ethernet-csmacd(6),
                    iso88023-csmacd(7),
                    iso88024-tokenBus(8),
                    iso88025-tokenRing(9),
                    iso88026-man(10),
                    starLan(11),
                    proteon-10Mbit(12),
                    proteon-80Mbit(13),
                    hyperchannel(14),
                    fddi(15),
                    lapb(16),
                    sdlc(17),
                    ds1(18),           -- T-1
                    e1(19),            -- european equiv. of T-1
                    basicISDN(20),
                    primaryISDN(21),   -- proprietary serial
                    propPointToPointSerial(22),
                    ppp(23),
                    softwareLoopback(24),
                    eon(25),            -- CLNP over IP [11]
                    ethernet-3Mbit(26),
                    nsip(27),           -- XNS over IP
                    slip(28),           -- generic SLIP
                    ultra(29),          -- ULTRA technologies
                    ds3(30),            -- T-3
                    sip(31),            -- SMDS
                    frame-relay(32)
                }
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The type of interface, distinguished according to
                the physical/link protocol(s) immediately `below'
                the network layer in the protocol stack."
*/
static int fn_ifType(MIB_CONTROL *ptrMIB_control)                        // table type
{
    unsigned char ucInterfaceReference = ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength];
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    else {
        if (ucInterfaceReference == 0) {                                  // handle 0 and 1 as the same - interface reference starts at 1
            ucInterfaceReference = 1;
        }
    }
    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = 6;                                        // interface number
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index
    return 0;                                                            // could be handled
}

/*
    ifMtu OBJECT-TYPE
        SYNTAX  INTEGER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The size of the largest datagram which can be
                sent/received on the interface, specified in
                octets.  For interfaces that are used for
                transmitting network datagrams, this is the size
                of the largest network datagram that can be sent
                on the interface."
*/
static int fn_ifMtu(MIB_CONTROL *ptrMIB_control)                         // table type
{
    unsigned char ucInterfaceReference = ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength];
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    else {
        if (ucInterfaceReference == 0) {                                  // handle 0 and 1 as the same - interface reference starts at 1
            ucInterfaceReference = 1;
        }
    }
    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = ETH_MTU;                                 // interface MTU
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index
    return 0;                                                            // could be handled
}

/*
    ifSpeed OBJECT-TYPE
        SYNTAX  Gauge
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "An estimate of the interface's current bandwidth
                in bits per second.  For interfaces which do not
                vary in bandwidth or for those where no accurate
                estimation can be made, this object should contain
                the nominal bandwidth."
*/
static int fn_ifSpeed(MIB_CONTROL *ptrMIB_control)                       // table type
{
    unsigned char ucInterfaceReference = ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength];
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    else {
        if (ucInterfaceReference == 0) {                                  // handle 0 and 1 as the same - interface reference starts at 1
            ucInterfaceReference = 1;
        }
    }
    ptrMIB_control->ucObjectType = ASN1_GAUGE_CODE;
    ptrMIB_control->ulInteger = 100000000;                               // 100M
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index
    return 0;                                                            // could be handled
}

/*
    ifPhysAddress OBJECT-TYPE
        SYNTAX  PhysAddress
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The interface's address at the protocol layer
                immediately `below' the network layer in the
                protocol stack.  For interfaces which do not have
                such an address (e.g., a serial line), this object
                should contain an octet string of zero length."

        1.3.6.1.2.1.2.2.1.6  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifPhyAddress]
*/
static int fn_ifPhysAddress(MIB_CONTROL *ptrMIB_control)                 // table type
{
    unsigned char ucInterfaceReference = ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength];
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    else {
        if (ucInterfaceReference == 0) {                                  // handle 0 and 1 as the same - interface reference starts at 1
            ucInterfaceReference = 1;
        }
    }
    ptrMIB_control->ucObjectType = ASN1_PHY_ADDRESS_CODE;                // pseudo type (this uses 
    ptrMIB_control->ptrString = (CHAR *)network[DEFAULT_NETWORK].ucOurMAC; // interface's MAC address
    ptrMIB_control->ucObjectLength = sizeof(network[DEFAULT_NETWORK].ucOurMAC);
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index
    return 0;                                                            // could be handled
}

/*
    ifAdminStatus OBJECT-TYPE
        SYNTAX  INTEGER {
                    up(1),       -- ready to pass packets
                    down(2),
                    testing(3)   -- in some test mode
                }
        ACCESS  read-write
        STATUS  mandatory
        DESCRIPTION
                "The desired state of the interface.  The
                testing(3) state indicates that no operational
                packets can be passed."
        1.3.6.1.2.1.2.2.1.7  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifAdminStatus]
*/
static int fn_ifAdminStatus(MIB_CONTROL *ptrMIB_control)                 // table type
{
    unsigned char ucInterfaceReference = ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength];
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    else {
        if (ucInterfaceReference == 0) {                                  // handle 0 and 1 as the same - interface reference starts at 1
            ucInterfaceReference = 1;
        }
    }
    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = 1;
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index
    return 0;                                                            // could be handled
}


/*
    ifOperStatus OBJECT-TYPE
        SYNTAX  INTEGER {
                    up(1),       -- ready to pass packets
                    down(2),
                    testing(3)   -- in some test mode
                }
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The current operational state of the interface.
                The testing(3) state indicates that no operational
                packets can be passed."
*//*
static int fn_ifOperStatus(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifLastChange OBJECT-TYPE
        SYNTAX  TimeTicks
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The value of sysUpTime at the time the interface
                entered its current operational state.  If the
                current state was entered prior to the last re-
                initialization of the local network management
                subsystem, then this object contains a zero
                value."
*//*
static int fn_ifLastChange(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifInOctets OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The total number of octets received on the
                interface, including framing characters."
*//*
static int fn_ifInOctets(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifInUcastPkts OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of subnetwork-unicast packets
                delivered to a higher-layer protocol."
*//*
static int fn_ifInUcastPkts(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifInNUcastPkts OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of non-unicast (i.e., subnetwork-
                broadcast or subnetwork-multicast) packets
                delivered to a higher-layer protocol."
*//*
static int fn_ifInNUcastPkts(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifInDiscards OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of inbound packets which were chosen
                to be discarded even though no errors had been
                detected to prevent their being deliverable to a
                higher-layer protocol.  One possible reason for
                discarding such a packet could be to free up
                buffer space."
*//*
static int fn_ifInDiscards(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifInErrors OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of inbound packets that contained
                errors preventing them from being deliverable to a
                higher-layer protocol."
*//*
static int fn_ifInErrors(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifInUnknownProtos OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of packets received via the interface
                which were discarded because of an unknown or
                unsupported protocol."
*//*
static int fn_ifInUnknownProtos(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifOutOctets OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The total number of octets transmitted out of the
                interface, including framing characters."
*//*
static int fn_ifOutOctets(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifOutUcastPkts OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The total number of packets that higher-level
                protocols requested be transmitted to a
                subnetwork-unicast address, including those that
                were discarded or not sent."
*//*
static int fn_ifOutUcastPkts(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifOutNUcastPkts OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The total number of packets that higher-level
                protocols requested be transmitted to a non-
                unicast (i.e., a subnetwork-broadcast or
                subnetwork-multicast) address, including those
                that were discarded or not sent."
*//*
static int fn_ifOutNUcastPkts(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifOutDiscards OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of outbound packets which were chosen
                to be discarded even though no errors had been
                detected to prevent their being transmitted.  One
                possible reason for discarding such a packet could
                be to free up buffer space."
*//*
static int fn_ifOutDiscards(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifOutErrors OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of outbound packets that could not be
                transmitted because of errors."
*//*
static int fn_ifOutErrors(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifOutQLen OBJECT-TYPE
        SYNTAX  Gauge
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The length of the output packet queue (in
                packets)."
*//*
static int fn_ifOutQLen(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/

/*
    ifSpecific OBJECT-TYPE
        SYNTAX  OBJECT IDENTIFIER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "A reference to MIB definitions specific to the
                particular media being used to realize the
                interface.  For example, if the interface is
                realized by an ethernet, then the value of this
                object refers to a document defining objects
                specific to ethernet.  If this information is not
                present, its value should be set to the OBJECT
                IDENTIFIER { 0 0 }, which is a syntatically valid
                object identifier, and any conformant
                implementation of ASN.1 and BER must be able to
                generate and recognize this value."
*//*
static int fn_ifSpecific(MIB_CONTROL *ptrMIB_control)
{
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {
        return -1;                                                       // no next in this handler
    }
    return 0;                                                            // could be handled
}*/


/*
    ipForwarding OBJECT-TYPE
        SYNTAX  INTEGER {
                    forwarding(1),    -- acting as a gateway
                    not-forwarding(2) -- NOT acting as a gateway
                }
        ACCESS  read-write
        STATUS  mandatory
        DESCRIPTION
                "The indication of whether this entity is acting
                as an IP gateway in respect to the forwarding of
                datagrams received by, but not addressed to, this
                entity.  IP gateways forward datagrams.  IP hosts
                do not (except those source-routed via the host).

                Note that for some managed nodes, this object may
                take on only a subset of the values possible.
                Accordingly, it is appropriate for an agent to
                return a `badValue' response if a management
                station attempts to change this object to an
                inappropriate value."
*//*
static int fn_ipForwarding(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipDefaultTTL OBJECT-TYPE
        SYNTAX  INTEGER
        ACCESS  read-write
        STATUS  mandatory
        DESCRIPTION
                "The default value inserted into the Time-To-Live
                field of the IP header of datagrams originated at
                this entity, whenever a TTL value is not supplied
                by the transport layer protocol."
*//*
static int fn_ipDefaultTTL(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipInReceives OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The total number of input datagrams received from
                interfaces, including those received in error."
*//*
static int fn_ipInReceives(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipInHdrErrors OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of input datagrams discarded due to
                errors in their IP headers, including bad
                checksums, version number mismatch, other format
                errors, time-to-live exceeded, errors discovered
                in processing their IP options, etc."
*//*
static int fn_ipInHdrErrors(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipInAddrErrors OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of input datagrams discarded because
                the IP address in their IP header's destination
                field was not a valid address to be received at
                this entity.  This count includes invalid
                addresses (e.g., 0.0.0.0) and addresses of
                unsupported Classes (e.g., Class E).  For entities
                which are not IP Gateways and therefore do not
                forward datagrams, this counter includes datagrams
                discarded because the destination address was not
                a local address."
*//*
static int fn_ipInAddrErrors(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipForwDatagrams OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of input datagrams for which this
                entity was not their final IP destination, as a
                result of which an attempt was made to find a
                route to forward them to that final destination.
                In entities which do not act as IP Gateways, this
                counter will include only those packets which were
                Source-Routed via this entity, and the Source-
                Route option processing was successful."
*//*
static int fn_ipForwDatagrams(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipInUnknownProtos OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of locally-addressed datagrams
                received successfully but discarded because of an
                unknown or unsupported protocol."
*//*
static int fn_ipInUnknownProtos(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
ipInDiscards OBJECT-TYPE
    SYNTAX  Counter
    ACCESS  read-only
    STATUS  mandatory
    DESCRIPTION
            "The number of input IP datagrams for which no
            problems were encountered to prevent their
            continued processing, but which were discarded
            (e.g., for lack of buffer space).  Note that this
            counter does not include any datagrams discarded
            while awaiting re-assembly."
*//*
static int fn_ipInDiscards(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipInDelivers OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The total number of input datagrams successfully
                delivered to IP user-protocols (including ICMP)."
*//*
static int fn_ipInDelivers(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipOutRequests OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The total number of IP datagrams which local IP
                user-protocols (including ICMP) supplied to IP in
                requests for transmission.  Note that this counter
                does not include any datagrams counted in
                ipForwDatagrams."
*//*
static int fn_ipOutRequests(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipOutDiscards OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of output IP datagrams for which no
                problem was encountered to prevent their
                transmission to their destination, but which were
                discarded (e.g., for lack of buffer space).  Note
                that this counter would include datagrams counted
                in ipForwDatagrams if any such packets met this
                (discretionary) discard criterion."
*//*
static int fn_ipOutDiscards(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipOutNoRoutes OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of IP datagrams discarded because no
                route could be found to transmit them to their
                destination.  Note that this counter includes any
                packets counted in ipForwDatagrams which meet this
                `no-route' criterion.  Note that this includes any
                datagarms which a host cannot route because all of
                its default gateways are down."
*//*
static int fn_ipOutNoRoutes(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipReasmTimeout OBJECT-TYPE
        SYNTAX  INTEGER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The maximum number of seconds which received
                fragments are held while they are awaiting
                reassembly at this entity."
*//*
static int fn_ipReasmTimeout(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipReasmReqds OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of IP fragments received which needed
                to be reassembled at this entity."
*//*
static int fn_ipReasmReqds(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipReasmOKs OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of IP datagrams successfully re-
                assembled."
*//*
static int fn_ipReasmOKs(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipReasmFails OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of failures detected by the IP re-
                assembly algorithm (for whatever reason: timed
                out, errors, etc).  Note that this is not
                necessarily a count of discarded IP fragments
                since some algorithms (notably the algorithm in
                RFC 815) can lose track of the number of fragments
                by combining them as they are received."
*//*
static int fn_ipReasmFails(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipFragOKs OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of IP datagrams that have been
                successfully fragmented at this entity."
*//*
static int fn_ipFragOKs(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipFragFails OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of IP datagrams that have been
                discarded because they needed to be fragmented at
                this entity but could not be, e.g., because their
                Don't Fragment flag was set."
*//*
static int fn_ipFragFails(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipFragCreates OBJECT-TYPE
        SYNTAX  Counter
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The number of IP datagram fragments that have
                been generated as a result of fragmentation at
                this entity."
*//*
static int fn_ipFragCreates(MIB_CONTROL *ptrMIB_control)
{
    return 0;                                                            // could be handled
}*/

/*
    ipAdEntAddr OBJECT-TYPE
        SYNTAX  IpAddress
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The IP address to which this entry's addressing
                information pertains."
        1.3.6.1.2.1.4.20.1.1 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntAddr]
*/
static int fn_ipAdEntAddr(MIB_CONTROL *ptrMIB_control)                   // table type
{
    int i = 0;
    unsigned char ucInterfaceReference = 0;
    unsigned char ucIPaddress[IPV4_LENGTH];
    if (ptrMIB_control->ucVariableBindingLength != (ptrMIB_control->ucLevel + 1)) { // input contains the IP address being referenced
      //int iLength = (ptrMIB_control->ucVariableBindingLength - ptrMIB_control->ucLevel);
        int iInput = (ptrMIB_control->ucLevel + 1);
        do {
            if (ptrMIB_control->ucVariableBinding[iInput] & 0x80) {
                ucIPaddress[i] = 0x80;
                ucIPaddress[i] |= ptrMIB_control->ucVariableBinding[++iInput];
            }
            else {
                ucIPaddress[i] = ptrMIB_control->ucVariableBinding[iInput];
            }
            iInput++;
        } while (++i < IPV4_LENGTH);

        // Match the received IP address reference to the interface entry
        //
        do {
            if (uMemcmp(network[ucInterfaceReference].ucOurIP, ucIPaddress, IPV4_LENGTH) == 0) {
                break;
            }
        } while (++ucInterfaceReference < IP_INTERFACE_COUNT);
        if (ucInterfaceReference >= IP_INTERFACE_COUNT) {                // this is not expected to happen
            ucInterfaceReference = 0;
        }
    }
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    ptrMIB_control->ucObjectType = ASN1_IP_ADDRESS_CODE;
    ptrMIB_control->ptrString = (CHAR *)network[ucInterfaceReference].ucOurIP; // interface's IP address
    ptrMIB_control->ucObjectLength = sizeof(network[ucInterfaceReference].ucOurIP);

    if (i == 0) {
        // Here we need to insert our IP address at the end of the object identifier (192.168.2.2 would be 1.3.6.1.2.1.4.20.1.1.192.168.2.2 [192 = 81 + 40. 168 = 81 + 28]
        //
        for (; i < IPV4_LENGTH; i++) {
            if (network[ucInterfaceReference].ucOurIP[i] < 0x80) {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = network[ucInterfaceReference].ucOurIP[i];
            }
            else {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength++] = 0x81;
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = (network[ucInterfaceReference].ucOurIP[i] & 0x7f);
            }
            if (i < (IPV4_LENGTH - 1)) {
                ptrMIB_control->ucVariableBindingLength++;
            }
        }
    }
    return 0;                                                            // could be handled
}

/*
    ipAdEntIfIndex OBJECT-TYPE
        SYNTAX  INTEGER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The index value which uniquely identifies the
                interface to which this entry is applicable.  The
                interface identified by a particular value of this
                index is the same interface as identified by the
                same value of ifIndex."
        1.3.6.1.2.1.4.20.1.2 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntIfIndex]
*/
static int fn_ipAdEntIfIndex(MIB_CONTROL *ptrMIB_control)                // table type
{
    int i = 0;
    unsigned char ucInterfaceReference = 0;
    unsigned char ucIPaddress[IPV4_LENGTH];
    if (ptrMIB_control->ucVariableBindingLength != (ptrMIB_control->ucLevel + 1)) { // input contains the IP address being referenced
      //int iLength = (ptrMIB_control->ucVariableBindingLength - ptrMIB_control->ucLevel);
        int iInput = (ptrMIB_control->ucLevel + 1);
        do {
            if (ptrMIB_control->ucVariableBinding[iInput] & 0x80) {
                ucIPaddress[i] = 0x80;
                ucIPaddress[i] |= ptrMIB_control->ucVariableBinding[++iInput];
            }
            else {
                ucIPaddress[i] = ptrMIB_control->ucVariableBinding[iInput];
            }
            iInput++;
        } while (++i < IPV4_LENGTH);

        // Match the received IP address reference to the interface entry
        //
        do {
            if (uMemcmp(network[ucInterfaceReference].ucOurIP, ucIPaddress, IPV4_LENGTH) == 0) {
                break;
            }
        } while (++ucInterfaceReference < IP_INTERFACE_COUNT);
        if (ucInterfaceReference >= IP_INTERFACE_COUNT) {                // this is not expected to happen
            ucInterfaceReference = 0;
        }
    }
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }

    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = 1;
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index

    if (i == 0) {
        // Here we need to insert our IP address at the end of the object identifier (192.168.2.2 would be 1.3.6.1.2.1.4.20.1.1.192.168.2.2 [192 = 81 + 40. 168 = 81 + 28]
        //
        for (; i < IPV4_LENGTH; i++) {
            if (network[ucInterfaceReference].ucOurIP[i] < 0x80) {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = network[ucInterfaceReference].ucOurIP[i];
            }
            else {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength++] = 0x81;
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = (network[ucInterfaceReference].ucOurIP[i] & 0x7f);
            }
            if (i < (IPV4_LENGTH - 1)) {
                ptrMIB_control->ucVariableBindingLength++;
            }
        }
    }
    return 0;                                                            // could be handled
}

/*
    ipAdEntNetMask OBJECT-TYPE
        SYNTAX  IpAddress
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The subnet mask associated with the IP address of
                this entry.  The value of the mask is an IP
                address with all the network bits set to 1 and all
                the hosts bits set to 0."
        1.3.6.1.2.1.4.20.1.3 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntNetMask]
*/
static int fn_ipAdEntNetMask(MIB_CONTROL *ptrMIB_control)                   // table type
{
    int i = 0;
    unsigned char ucInterfaceReference = 0;
    unsigned char ucIPaddress[IPV4_LENGTH];
    if (ptrMIB_control->ucVariableBindingLength != (ptrMIB_control->ucLevel + 1)) { // input contains the IP address being referenced
      //int iLength = (ptrMIB_control->ucVariableBindingLength - ptrMIB_control->ucLevel);
        int iInput = (ptrMIB_control->ucLevel + 1);
        do {
            if (ptrMIB_control->ucVariableBinding[iInput] & 0x80) {
                ucIPaddress[i] = 0x80;
                ucIPaddress[i] |= ptrMIB_control->ucVariableBinding[++iInput];
            }
            else {
                ucIPaddress[i] = ptrMIB_control->ucVariableBinding[iInput];
            }
            iInput++;
        } while (++i < IPV4_LENGTH);

        // Match the received IP address reference to the interface entry
        //
        do {
            if (uMemcmp(network[ucInterfaceReference].ucOurIP, ucIPaddress, IPV4_LENGTH) == 0) {
                break;
            }
        } while (++ucInterfaceReference < IP_INTERFACE_COUNT);
        if (ucInterfaceReference >= IP_INTERFACE_COUNT) {                // this is not expected to happen
            ucInterfaceReference = 0;
        }
    }
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }
    ptrMIB_control->ucObjectType = ASN1_IP_ADDRESS_CODE;
    ptrMIB_control->ptrString = (CHAR *)network[ucInterfaceReference].ucNetMask; // interface's IP address
    ptrMIB_control->ucObjectLength = sizeof(network[ucInterfaceReference].ucNetMask);

    if (i == 0) {
        // Here we need to insert our IP address at the end of the object identifier (192.168.2.2 would be 1.3.6.1.2.1.4.20.1.1.192.168.2.2 [192 = 81 + 40. 168 = 81 + 28]
        //
        for (; i < IPV4_LENGTH; i++) {
            if (network[ucInterfaceReference].ucOurIP[i] < 0x80) {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = network[ucInterfaceReference].ucOurIP[i];
            }
            else {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength++] = 0x81;
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = (network[ucInterfaceReference].ucOurIP[i] & 0x7f);
            }
            if (i < (IPV4_LENGTH - 1)) {
                ptrMIB_control->ucVariableBindingLength++;
            }
        }
    }
    return 0;                                                            // could be handled
}

/*
    ipAdEntBcastAddr OBJECT-TYPE
        SYNTAX  INTEGER
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The value of the least-significant bit in the IP
                broadcast address used for sending datagrams on
                the (logical) interface associated with the IP
                address of this entry.  For example, when the
                Internet standard all-ones broadcast address is
                used, the value will be 1.  This value applies to
                both the subnet and network broadcasts addresses
                used by the entity on this (logical) interface."
        1.3.6.1.2.1.4.20.1.4 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntBcastAddr]
*/
static int fn_ipAdEntBcastAddr(MIB_CONTROL *ptrMIB_control)                // table type
{
    int i = 0;
    unsigned char ucInterfaceReference = 0;
    unsigned char ucIPaddress[IPV4_LENGTH];
    if (ptrMIB_control->ucVariableBindingLength != (ptrMIB_control->ucLevel + 1)) { // input contains the IP address being referenced
      //int iLength = (ptrMIB_control->ucVariableBindingLength - ptrMIB_control->ucLevel);
        int iInput = (ptrMIB_control->ucLevel + 1);
        do {
            if (ptrMIB_control->ucVariableBinding[iInput] & 0x80) {
                ucIPaddress[i] = 0x80;
                ucIPaddress[i] |= ptrMIB_control->ucVariableBinding[++iInput];
            }
            else {
                ucIPaddress[i] = ptrMIB_control->ucVariableBinding[iInput];
            }
            iInput++;
        } while (++i < IPV4_LENGTH);

        // Match the received IP address reference to the interface entry
        //
        do {
            if (uMemcmp(network[ucInterfaceReference].ucOurIP, ucIPaddress, IPV4_LENGTH) == 0) {
                break;
            }
        } while (++ucInterfaceReference < IP_INTERFACE_COUNT);
        if (ucInterfaceReference >= IP_INTERFACE_COUNT) {                // this is not expected to happen
            ucInterfaceReference = 0;
        }
    }
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }

    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = 1;
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index

    if (i == 0) {
        // Here we need to insert our IP address at the end of the object identifier (192.168.2.2 would be 1.3.6.1.2.1.4.20.1.1.192.168.2.2 [192 = 81 + 40. 168 = 81 + 28]
        //
        for (; i < IPV4_LENGTH; i++) {
            if (network[ucInterfaceReference].ucOurIP[i] < 0x80) {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = network[ucInterfaceReference].ucOurIP[i];
            }
            else {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength++] = 0x81;
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = (network[ucInterfaceReference].ucOurIP[i] & 0x7f);
            }
            if (i < (IPV4_LENGTH - 1)) {
                ptrMIB_control->ucVariableBindingLength++;
            }
        }
    }
    return 0;                                                            // could be handled
}

/*
    ipAdEntReasmMaxSize OBJECT-TYPE
        SYNTAX  INTEGER (0..65535)
        ACCESS  read-only
        STATUS  mandatory
        DESCRIPTION
                "The size of the largest IP datagram which this
                entity can re-assemble from incoming IP fragmented
                datagrams received on this interface."
        1.3.6.1.2.1.4.20.1.5 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntReasmMaxSize]
*/
static int fn_ipAdEntReasmMaxSize(MIB_CONTROL *ptrMIB_control)           // table type
{
    int i = 0;
    unsigned char ucInterfaceReference = 0;
    unsigned char ucIPaddress[IPV4_LENGTH];
    if (ptrMIB_control->ucVariableBindingLength != (ptrMIB_control->ucLevel + 1)) { // input contains the IP address being referenced
      //int iLength = (ptrMIB_control->ucVariableBindingLength - ptrMIB_control->ucLevel);
        int iInput = (ptrMIB_control->ucLevel + 1);
        do {
            if (ptrMIB_control->ucVariableBinding[iInput] & 0x80) {
                ucIPaddress[i] = 0x80;
                ucIPaddress[i] |= ptrMIB_control->ucVariableBinding[++iInput];
            }
            else {
                ucIPaddress[i] = ptrMIB_control->ucVariableBinding[iInput];
            }
            iInput++;
        } while (++i < IPV4_LENGTH);

        // Match the received IP address reference to the interface entry
        //
        do {
            if (uMemcmp(network[ucInterfaceReference].ucOurIP, ucIPaddress, IPV4_LENGTH) == 0) {
                break;
            }
        } while (++ucInterfaceReference < IP_INTERFACE_COUNT);
        if (ucInterfaceReference >= IP_INTERFACE_COUNT) {                // this is not expected to happen
            ucInterfaceReference = 0;
        }
    }
    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {   // request for the next table entry
        if (ucInterfaceReference >= 0/*IP_INTERFACE_COUNT*/) {
            return -1;                                                   // end of table reached
        }
    }

    ptrMIB_control->ucObjectType = ASN1_INTEGER_CODE;
    ptrMIB_control->ulInteger = 0xbb80;        // test value
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = ucInterfaceReference; // modify the object identifier to match table index

    if (i == 0) {
        // Here we need to insert our IP address at the end of the object identifier (192.168.2.2 would be 1.3.6.1.2.1.4.20.1.1.192.168.2.2 [192 = 81 + 40. 168 = 81 + 28]
        //
        for (; i < IPV4_LENGTH; i++) {
            if (network[ucInterfaceReference].ucOurIP[i] < 0x80) {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = network[ucInterfaceReference].ucOurIP[i];
            }
            else {
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength++] = 0x81;
                ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = (network[ucInterfaceReference].ucOurIP[i] & 0x7f);
            }
            if (i < (IPV4_LENGTH - 1)) {
                ptrMIB_control->ucVariableBindingLength++;
            }
        }
    }
    return 0;                                                            // could be handled
}





static const MIB_TABLE mib_table[] = {                                   // according to rfc 1213
// level       path
//     /--------/\------------\    access type              set/get handler
//
    {0, 6,                         (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6                [iso.org.dod]
    {1,    1,                      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1              [iso.org.dod.internet]
    {2,       1,                   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.1            [iso.org.dod.internet.directory]
    {2,       2,                   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2            [iso.org.dod.internet.mgmt]
    {3,           1,               (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1          [iso.org.dod.internet.mgmt.mib-2]
    {4,               1,           (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.1        [iso.org.dod.internet.mgmt.mib-2.system] - implementation of the Interfaces group is mandatory for all systems
    {5,                   1,       (MIB_R_ONLY),            fn_sysDescr,         },  // 1.3.6.1.2.1.1.1      [iso.org.dod.internet.mgmt.mib-2.system.sysDescr]
    {5,                   2,       (MIB_R_ONLY),            fn_sysObjectID,      },  // 1.3.6.1.2.1.1.2      [iso.org.dod.internet.mgmt.mib-2.system.sysObjectID]
    {5,                   3,       (MIB_R_ONLY),            fn_sysUpTime,        },  // 1.3.6.1.2.1.1.3      [iso.org.dod.internet.mgmt.mib-2.system.sysUpTime]
    {5,                   4,       (MIB_R_W),               fn_sysContact,       },  // 1.3.6.1.2.1.1.4      [iso.org.dod.internet.mgmt.mib-2.system.sysContact]
    {5,                   5,       (MIB_R_W),               fn_sysName,          },  // 1.3.6.1.2.1.1.5      [iso.org.dod.internet.mgmt.mib-2.system.sysName]
    {5,                   6,       (MIB_R_W),               fn_sysLocation,      },  // 1.3.6.1.2.1.1.6      [iso.org.dod.internet.mgmt.mib-2.system.sysLocation]
    {5,                   7,       (MIB_R_W),NOT_SUPPORTED/*fn_sysServices*/,    },  // 1.3.6.1.2.1.1.7      [iso.org.dod.internet.mgmt.mib-2.system.sysServices]
    {5,                   8,       (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.1.8      [iso.org.dod.internet.mgmt.mib-2.system.sysORLastChange]
    {5,                   9,       (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.1.9      [iso.org.dod.internet.mgmt.mib-2.system.sysORTable]
    {6,                      1,    (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.1.9.1    [iso.org.dod.internet.mgmt.mib-2.system.sysORTable.sysOREntry]
    {7,                         1, (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.1.9.1.1  [iso.org.dod.internet.mgmt.mib-2.system.sysORTable.sysOREntry.sysORIndex]
    {7,                         2, (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.1.9.1.2  [iso.org.dod.internet.mgmt.mib-2.system.sysORTable.sysOREntry.sysORID]
    {7,                         3, (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.1.9.1.3  [iso.org.dod.internet.mgmt.mib-2.system.sysORTable.sysOREntry.sysORDescr]
    {7,                         4, (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.1.9.1.4  [iso.org.dod.internet.mgmt.mib-2.system.sysORTable.sysOREntry.sysORUpTime]
    {4,               2,           (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.2        [iso.org.dod.internet.mgmt.mib-2.interfaces] - implementation of the Interfaces group is mandatory for all systems
    {5,                  1,        (MIB_R_ONLY),            fn_ifNumber,         },  // 1.3.6.1.2.1.2.1      [iso.org.dod.internet.mgmt.mib-2.interfaces.ifNumber]
    {5,                  2,        (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.2.2      [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable]
    {6,                     1,     (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.2.2.1    [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry]
    {7,                         1, (MIB_R_ONLY | MIB_TAB),  fn_ifIndex,          },  // 1.3.6.1.2.1.2.2.1.1  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifIndex]
    {7,                         2, (MIB_R_ONLY | MIB_TAB),  fn_ifDescr,          },  // 1.3.6.1.2.1.2.2.1.2  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifDescr]
    {7,                         3, (MIB_R_ONLY | MIB_TAB),  fn_ifType,           },  // 1.3.6.1.2.1.2.2.1.3  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifType]
    {7,                         4, (MIB_R_ONLY | MIB_TAB),  fn_ifMtu,            },  // 1.3.6.1.2.1.2.2.1.4  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifMtu]
    {7,                         5, (MIB_R_ONLY | MIB_TAB),  fn_ifSpeed,          },  // 1.3.6.1.2.1.2.2.1.5  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifSpeed]
    {7,                         6, (MIB_R_ONLY | MIB_TAB),  fn_ifPhysAddress,    },  // 1.3.6.1.2.1.2.2.1.6  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifPhyAddress]
    {7,                         7, (MIB_R_W | MIB_TAB),     fn_ifAdminStatus,    },  // 1.3.6.1.2.1.2.2.1.7  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifAdminStatus]
    {7,                         8, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifOperStatus*/,},  // 1.3.6.1.2.1.2.2.1.8  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOperStatus]
    {7,                         9, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifLastChange*/,     },  // 1.3.6.1.2.1.2.2.1.9  [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifLastChange]
    {7,                        10, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifInOctets*/,       },  // 1.3.6.1.2.1.2.2.1.10 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifInOctets]
    {7,                        11, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifInUcastPkts*/,    },  // 1.3.6.1.2.1.2.2.1.11 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifInUcastPkts]
    {7,                        12, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifInNUcastPkts*/,   },  // 1.3.6.1.2.1.2.2.1.12 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifInNUcastPkts]
    {7,                        13, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifInDiscards*/,     },  // 1.3.6.1.2.1.2.2.1.13 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifInDiscards]
    {7,                        14, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifInErrors*/,       },  // 1.3.6.1.2.1.2.2.1.14 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifInErrors]
    {7,                        15, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifInUnknownProtos*/,},  // 1.3.6.1.2.1.2.2.1.15 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifInUnknownProtos]
    {7,                        16, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifOutOctets*/,      },  // 1.3.6.1.2.1.2.2.1.16 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOutOctets]
    {7,                        17, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifOutUcastPkts*/,   },  // 1.3.6.1.2.1.2.2.1.17 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOutUcastPkts]
    {7,                        18, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifOutNUcastPkts*/,  },  // 1.3.6.1.2.1.2.2.1.18 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOutNUcastPkts]
    {7,                        19, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifOutDiscards*/,    },  // 1.3.6.1.2.1.2.2.1.19 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOutDIscards]
    {7,                        20, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifOutErrors*/,      },  // 1.3.6.1.2.1.2.2.1.20 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOutErrors]
    {7,                        21, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifOutQLen*/,        },  // 1.3.6.1.2.1.2.2.1.21 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOutQLen]
    {7,                        22, (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED/*fn_ifSpecific*/,       },  // 1.3.6.1.2.1.2.2.1.22 [iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifSpecific]
    {4,               3,           (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.3        [iso.org.dod.internet.mgmt.mib-2.atTable] - entire address translation group depreciated
    {4,               4,           (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.4        [iso.org.dod.internet.mgmt.mib-2.ip] - implementation of the Interfaces group is mandatory for all systems
    {5,                  1,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipForwarding*/,     },  // 1.3.6.1.2.1.4.1      [iso.org.dod.internet.mgmt.mib-2.ip.ipForwarding]
    {5,                  2,        (MIB_R_W),               NOT_SUPPORTED/*fn_ipDefaultTTL*/,     },  // 1.3.6.1.2.1.4.2      [iso.org.dod.internet.mgmt.mib-2.ip.ipDefaultTTL]
    {5,                  3,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipInReceives*/,     },  // 1.3.6.1.2.1.4.3      [iso.org.dod.internet.mgmt.mib-2.ip.ipInReceives]
    {5,                  4,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipInHdrErrors*/,    },  // 1.3.6.1.2.1.4.4      [iso.org.dod.internet.mgmt.mib-2.ip.ipInHdrErrors]
    {5,                  5,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipInAddrErrors*/,   },  // 1.3.6.1.2.1.4.5      [iso.org.dod.internet.mgmt.mib-2.ip.ipInAddrErrors]
    {5,                  6,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipForwDatagrams*/,  },  // 1.3.6.1.2.1.4.6      [iso.org.dod.internet.mgmt.mib-2.ip.ipForwDatagrams]
    {5,                  7,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipInUnknownProtos*/,},  // 1.3.6.1.2.1.4.7      [iso.org.dod.internet.mgmt.mib-2.ip.ipInUnknownProtos]
    {5,                  8,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipInDiscards*/,     },  // 1.3.6.1.2.1.4.8      [iso.org.dod.internet.mgmt.mib-2.ip.ipInDiscards]
    {5,                  9,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipInDelivers*/,     },  // 1.3.6.1.2.1.4.9      [iso.org.dod.internet.mgmt.mib-2.ip.ipInDelivers]
    {5,                 10,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipOutRequests*/,    },  // 1.3.6.1.2.1.4.10     [iso.org.dod.internet.mgmt.mib-2.ip.ipOutRequests]
    {5,                 11,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipOutDiscards*/,    },  // 1.3.6.1.2.1.4.11     [iso.org.dod.internet.mgmt.mib-2.ip.ipOutDiscards]
    {5,                 12,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipOutNoRoutes*/,    },  // 1.3.6.1.2.1.4.12     [iso.org.dod.internet.mgmt.mib-2.ip.ipOutNoRoutes]
    {5,                 13,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipReasmTimeout*/,   },  // 1.3.6.1.2.1.4.13     [iso.org.dod.internet.mgmt.mib-2.ip.ipReasmTimeout]
    {5,                 14,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipReasmReqds*/,     },  // 1.3.6.1.2.1.4.14     [iso.org.dod.internet.mgmt.mib-2.ip.ipReasmReqds]
    {5,                 15,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipReasmOKs*/,       },  // 1.3.6.1.2.1.4.15     [iso.org.dod.internet.mgmt.mib-2.ip.ipReasmOKs]
    {5,                 16,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipReasmFails*/,     },  // 1.3.6.1.2.1.4.16     [iso.org.dod.internet.mgmt.mib-2.ip.ipReasmFails]
    {5,                 17,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipFragOKs*/,        },  // 1.3.6.1.2.1.4.17     [iso.org.dod.internet.mgmt.mib-2.ip.ipFragOKs]
    {5,                 18,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipFragFails*/,      },  // 1.3.6.1.2.1.4.18     [iso.org.dod.internet.mgmt.mib-2.ip.ipFragFails]
    {5,                 19,        (MIB_R_ONLY),            NOT_SUPPORTED/*fn_ipFragCreates*/,    },  // 1.3.6.1.2.1.4.19     [iso.org.dod.internet.mgmt.mib-2.ip.ipFragCreates]
    {5,                 20,        (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.4.20     [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable]
    {6,                    1,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.4.20.1   [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipAddrEntry]
    {7,                       1,   (MIB_R_ONLY | MIB_TAB),  fn_ipAdEntAddr,      },  // 1.3.6.1.2.1.4.20.1.1 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntAddr]
    {7,                       2,   (MIB_R_ONLY | MIB_TAB),  fn_ipAdEntIfIndex,   },  // 1.3.6.1.2.1.4.20.1.2 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntIfIndex]
    {7,                       3,   (MIB_R_ONLY | MIB_TAB),  fn_ipAdEntNetMask,   },  // 1.3.6.1.2.1.4.20.1.3 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntNetMask]
    {7,                       4,   (MIB_R_ONLY | MIB_TAB),  fn_ipAdEntBcastAddr, },  // 1.3.6.1.2.1.4.20.1.4 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntBcastAddr]
    {7,                       5,   (MIB_R_ONLY | MIB_TAB),  fn_ipAdEntReasmMaxSize,},// 1.3.6.1.2.1.4.20.1.5 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.IpAddrEntry.ipAdEntReasmMaxSize]
    {5,                 21,        (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.4.21     [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable]
    {6,                    1,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.4.21.1   [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry]
    {7,                       1,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.1 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteDest]
    {7,                       2,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.2 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteIfIndex]
    {7,                       3,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.3 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteMetric1]
    {7,                       4,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.4 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteMetric2]
    {7,                       5,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.5 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteMetric3]
    {7,                       6,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.6 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteMetric4]
    {7,                       7,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.7 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteNextHop]
    {7,                       8,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.8 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteType]
    {7,                       9,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.9 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteProto]
    {7,                      10,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.10[iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteAge]
    {7,                      11,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.11[iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteMask]
    {7,                      12,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.12[iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteMetric5]
    {7,                      13,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.21.1.13[iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRouteTable.ipRouteEntry.ipRouteInfo]
    {5,                 22,        (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.4.22     [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipNetToMediaTable]
    {6,                    1,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.4.22.1   [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipNetToMediaTable.ipNetToMediaEntry]
    {7,                       1,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.22.1.1 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipNetToMediaTable.ipNetToMediaEntry.ipNetToMediaIfIndex]
    {7,                       2,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.22.1.2 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipNetToMediaTable.ipNetToMediaEntry.ipNetToMediaPhysAddress]
    {7,                       3,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.22.1.3 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipNetToMediaTable.ipNetToMediaEntry.ipNetToMediaNetAddress]
    {7,                       4,   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.22.1.4 [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipNetToMediaTable.ipNetToMediaEntry.ipNetToMediaType]
    {5,                 23,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.4.23     [iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipRoutingDiscards]
    {4,               5,           (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5        [iso.org.dod.internet.mgmt.mib-2.icmp] - implementation of the ICMP group is mandatory for all systems
    {5,                  1,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.1      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInMsgs]
    {5,                  2,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.2      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInErrors]
    {5,                  3,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.3      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInDestUnreachs
    {5,                  4,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.4      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInTimeExcds]
    {5,                  5,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.5      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInParmProbs]
    {5,                  6,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.6      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInSrcQuenchs]
    {5,                  7,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.7      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInRedirects]
    {5,                  8,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.8      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInEchos]
    {5,                  9,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.9      [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInEchoReps]
    {5,                 10,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.10     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInTimestamps]
    {5,                 11,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.11     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInTimestampReps]
    {5,                 12,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.12     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInAddrMasks]
    {5,                 13,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.13     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpInAddrMaskReps]
    {5,                 14,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.14     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutMsgs]
    {5,                 15,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.15     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutErrors]
    {5,                 16,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.16     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutDestUnreachs]
    {5,                 17,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.17     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutTimeExcds]
    {5,                 18,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.18     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutParmProbs]
    {5,                 19,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.19     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutSrcQuenchs]
    {5,                 20,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.20     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutRedirects]
    {5,                 21,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.21     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutEchos]
    {5,                 22,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.22     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutEchoReps]
    {5,                 23,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.23     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutTimestamps]
    {5,                 24,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.24     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutTimestampReps]
    {5,                 25,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.25     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutAddrMasks]
    {5,                 26,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.5.26     [iso.org.dod.internet.mgmt.mib-2.icmp.icmpOutAddrMaskReps] 
    {4,               6,           (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6        [iso.org.dod.internet.mgmt.mib-2.tcp] - implementation of the TCP group is mandatory for all systems which implement TCP
    {5,                  1,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.1      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpRtoAlgorithm]
    {5,                  1,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.1      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpRtoAlgorithm]
    {5,                  2,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.2      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpRtoMin]
    {5,                  3,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.3      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpRtoMax]
    {5,                  4,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.4      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpMaxConn]
    {5,                  5,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.5      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpActiveOpens]
    {5,                  6,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.6      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpPassiveOpens]
    {5,                  7,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.7      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpAttemptFails]
    {5,                  8,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.8      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpEstabResets]
    {5,                  9,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.9      [iso.org.dod.internet.mgmt.mib-2.tcp.tcpCurrEstab]
    {5,                 10,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.10     [iso.org.dod.internet.mgmt.mib-2.tcp.tcpInSegs]
    {5,                 11,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.11     [iso.org.dod.internet.mgmt.mib-2.tcp.tcpOutSegs]
    {5,                 12,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.12     [iso.org.dod.internet.mgmt.mib-2.tcp.tcpRetransSegs]
    {5,                 13,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.13     [iso.org.dod.internet.mgmt.mib-2.tcp.tcpConnTable]
    {6,                    1,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.13.1   [iso.org.dod.internet.mgmt.mib-2.tcp.tcpConnTable.tcpConnEntry]
    {7,                       1,   (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.13.1.1 [iso.org.dod.internet.mgmt.mib-2.tcp.tcpConnTable.tcpConnState]
    {7,                       2,   (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.13.1.2 [iso.org.dod.internet.mgmt.mib-2.tcp.tcpConnTable.tcpConnLocalAddress]
    {7,                       3,   (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.13.1.3 [iso.org.dod.internet.mgmt.mib-2.tcp.tcpConnTable.tcpConnLocalPort]
    {7,                       4,   (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.13.1.4 [iso.org.dod.internet.mgmt.mib-2.tcp.tcpConnTable.tcpConnRemAddress]
    {7,                       5,   (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.13.1.5 [iso.org.dod.internet.mgmt.mib-2.tcp.tcpConnTable.tcpConnRemPort]
    {5,                 14,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.14     [iso.org.dod.internet.mgmt.mib-2.tcp.tcpInErrs]
    {5,                 15,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.6.15     [iso.org.dod.internet.mgmt.mib-2.tcp.tcpOutRsts]
    {4,               7,           (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.7        [iso.org.dod.internet.mgmt.mib-2.udp] - implementation of the UDP group is mandatory for all systems which implement UDP
    {5,                  1,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.7.1      [iso.org.dod.internet.mgmt.mib-2.udp.udpInDatagrams]
    {5,                  2,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.7.2      [iso.org.dod.internet.mgmt.mib-2.udp.udpNoPorts]
    {5,                  3,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.7.3      [iso.org.dod.internet.mgmt.mib-2.udp.udpInErrors]
    {5,                  4,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.7.4      [iso.org.dod.internet.mgmt.mib-2.udp.udpOutDatagrams]
    {5,                  5,        (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.7.5      [iso.org.dod.internet.mgmt.mib-2.udp.udpTable]
    {6,                     1,     (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.7.5.1    [iso.org.dod.internet.mgmt.mib-2.udp.udpEntry]
    {7,                        1,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.7.5.1.1  [iso.org.dod.internet.mgmt.mib-2.udp.udpEntry.udpLocalAddress]
    {7,                        2,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.7.5.1.2  [iso.org.dod.internet.mgmt.mib-2.udp.udpEntry.udpLocalPort]
    {4,               8,           (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.8        [iso.org.dod.internet.mgmt.mib-2.egp] - implementation of the EGP group is mandatory for all systems which implement EGP
    {5,                  1,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.1      [iso.org.dod.internet.mgmt.mib-2.egp.egpInMsgs]
    {5,                  2,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.2      [iso.org.dod.internet.mgmt.mib-2.egp.egpInErrors]
    {5,                  3,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.3      [iso.org.dod.internet.mgmt.mib-2.egp.egpOutMsgs]
    {5,                  4,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.4      [iso.org.dod.internet.mgmt.mib-2.egp.egpOutErrors]
    {5,                  5,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5      [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable]
    {5,                     1,     (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.8.5.1    [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry]
    {6,                        1,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.1  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighState]
    {6,                        2,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.2  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighAddr]
    {6,                        3,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.3  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighAs]
    {6,                        4,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.4  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighInMsgs]
    {6,                        5,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.5  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighInErrs]
    {6,                        6,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.6  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighOutMsgs]
    {6,                        7,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.7  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighOutErrs]
    {6,                        8,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.8  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighInErrMsgs]
    {6,                        9,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.9  [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighOutErrMsgs]
    {6,                       10,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.10 [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighStateUps]
    {6,                       11,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.11 [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighStateDowns]
    {6,                       12,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.12 [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighIntervalHello]
    {6,                       13,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.13 [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighIntervalPoll]
    {6,                       14,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.14 [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighMode]
    {6,                       15,  (MIB_R_ONLY | MIB_TAB),  NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.5.1.15 [iso.org.dod.internet.mgmt.mib-2.egp.egpNeighTable.egpNeighEntry.egpNeighEventTrigger]
    {5,                  6,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.8.6      [iso.org.dod.internet.mgmt.mib-2.egp.egpAs]
    {4,              10,           (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.10       [iso.org.dod.internet.mgmt.mib-2.transmission]
    {4,              11,           (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11       [iso.org.dod.internet.mgmt.mib-2.snmp] - implementation of the Interfaces group is mandatory for all systems which support an SNMP entity
    {5,                  1,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.1     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInPkts]
    {5,                  2,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.2     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutPkts]
    {5,                  3,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.3     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInBadVersions]
    {5,                  4,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.4     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInBadCommunityNames]
    {5,                  5,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.5     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInBadCommunityUsers]
    {5,                  6,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.6     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInASNParseErrs]
    {5,                  8,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.8     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInTooBigs]
    {5,                  9,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.9     [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInNoSuchNames]
    {5,                 10,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.10    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInBadValues]
    {5,                 11,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.11    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInReadOnlys]
    {5,                 12,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.12    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInGenErrs]
    {5,                 13,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.13    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInTotalReqVars]
    {5,                 14,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.14    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInTotalSetVars]
    {5,                 15,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.15    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInGetRequests]
    {5,                 16,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.16    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInGetNexts]
    {5,                 17,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.17    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInSetRequests]
    {5,                 18,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.18    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInGetResponses]
    {5,                 19,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.19    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpInTraps]
    {5,                 20,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.20    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutTooBigs]
    {5,                 21,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.21    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutNoSuchNames]
    {5,                 22,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.22    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutBadValues]
    {5,                 24,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.24    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutGenErrs]
    {5,                 25,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.25    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutGetRequests]
    {5,                 26,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.26    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutGetNexts]
    {5,                 27,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.27    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutSetRequests]
    {5,                 28,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.28    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutGetResponses]
    {5,                 29,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.29    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpOutTraps]
    {5,                 30,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.30    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpEnableAuthenTraps]
    {5,                 31,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.31    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpSilentDrops]
    {5,                 32,        (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.11.32    [iso.org.dod.internet.mgmt.mib-2.snmp.snmpProxyDrops]
    {4,              25,           (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25       [iso.org.dod.internet.mgmt.mib-2.host] rfc1514 host resources MIB
    {5,                 1,         (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.1     [iso.org.dod.internet.mgmt.mib-2.host.hrSystem]
    {6,                    1,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.1.1   [iso.org.dod.internet.mgmt.mib-2.host.hrSystem.hrSystemUptime]
    {6,                    2,      (MIB_R_W),               NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.1.2   [iso.org.dod.internet.mgmt.mib-2.host.hrSystem.hrSystemDate]
    {6,                    3,      (MIB_R_W),               NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.1.3   [iso.org.dod.internet.mgmt.mib-2.host.hrSystem.hrSystemInitialLoadDevice]
    {6,                    4,      (MIB_R_W),               NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.1.4   [iso.org.dod.internet.mgmt.mib-2.host.hrSystem.hrSystemInitialLoadParameters]
    {6,                    5,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.1.5   [iso.org.dod.internet.mgmt.mib-2.host.hrSystem.hrSystemNumUsers]
    {6,                    6,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.1.6   [iso.org.dod.internet.mgmt.mib-2.host.hrSystem.hrSystemProcesses]
    {6,                    7,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.1.7   [iso.org.dod.internet.mgmt.mib-2.host.hrSystem.hrSystemMaxProcesses]
    {5,                 2,         (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.2     [iso.org.dod.internet.mgmt.mib-2.host.hrStorage]
    {6,                    1,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.1   [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTypes]
    {6,                    2,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.2   [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrMemorySize]
    {6,                    3,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.2.3   [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.2.3.1 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.3.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry.hrStorageIndex]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.3.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry.hrStorageType]
    {8,                          3,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.3.1.3 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry.hrStorageDescr]
    {8,                          4,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.3.1.4 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry.hrStorageAllocationUnits]
    {8,                          5,(MIB_R_W),               NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.3.1.5 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry.hrStorageSize]
    {8,                          6,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.3.1.6 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry.hrStorageUsed
    {8,                          7,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.2.3.1.7 [iso.org.dod.internet.mgmt.mib-2.host.hrStorage.hrStorageTable.hrStorageEntry.hrStorageAllocationFailures]
    {5,                 3,         (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3     [iso.org.dod.internet.mgmt.mib-2.host.hrDevice]
    {6,                    1,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.1   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTypes]
    {6,                    2,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.2   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.2.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable.hrDeviceEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.2.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceIndex]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.2.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceType]
    {8,                          3,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.2.1.3 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceDescr]
    {8,                          4,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.2.1.4 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceID]
    {8,                          5,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.2.1.5 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceStatus]
    {8,                          6,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.2.1.6 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceErrors]
    {6,                    3,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.3   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrProcessorTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.3.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrProcessorTable.hrProcessorEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.3.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrProcessorTable.hrProcessorEntry.hrProcessorFrwID]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.3.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrProcessorTable.hrProcessorEntry.hrProcessorLoad]
    {6,                    4,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.4   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrNetworkTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.4.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrNetworkTable.hrNetworkEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.4.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrNetworkTable.hrNetworkEntry.hrNetworkIfIndex]
    {6,                    5,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.5   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPrinterTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.5.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPrinterTable.hrPrinterEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.5.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPrinterTable.hrPrinterEntry.hrPrinterStatus]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.5.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPrinterTable.hrPrinterEntry.hrPrinterDetectedErrorState]
    {6,                    6,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.6   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDiskStorageTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.6.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDiskStorageTable.hrDiskStorageEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.6.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDiskStorageTable.hrDiskStorageEntry.hrDiskStorageAccess]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.6.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDiskStorageTable.hrDiskStorageEntry.hrDiskStorageMedia]
    {8,                          3,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.6.1.3 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDiskStorageTable.hrDiskStorageEntry.hrDiskStorageRemoveble]
    {8,                          4,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.6.1.4 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrDiskStorageTable.hrDiskStorageEntry.hrDiskStorageCapacity]
    {6,                    7,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.7   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPartitionTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.7.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPartitionTable.hrPartitionEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.7.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPartitionTable.hrPartitionEntry.hrPartitionIndex]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.7.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPartitionTable.hrPartitionEntry.hrPartitionLabel]
    {8,                          3,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.7.1.3 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPartitionTable.hrPartitionEntry.hrPartitionID]
    {8,                          4,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.7.1.4 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPartitionTable.hrPartitionEntry.hrPartitionSize]
    {8,                          5,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.7.1.5 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrPartitionTable.hrPartitionEntry.hrPartitionFSIndex]
    {6,                    8,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.8   [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.3.8.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSIndex]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSMountPoint]
    {8,                          3,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.3 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSRemoteMountPoint]
    {8,                          4,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.4 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSType]
    {8,                          5,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.5 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSAccess]
    {8,                          6,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.6 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSBootable]
    {8,                          7,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.7 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSStorageIndex]
    {8,                          8,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.8 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSLastFullBackupDate]
    {8,                          9,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.3.8.1.9 [iso.org.dod.internet.mgmt.mib-2.host.hrDevice.hrFSTable.hrFSEntry.hrFSLastPartialBackupDate]
    {5,                 4,         (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.4     [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun]
    {6,                    1,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.1   [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWOSIndex]
    {6,                    2,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.4.2   [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.4.2.1 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.2.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry.hrSWRunIndex]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.2.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry.hrSWRunName]
    {8,                          3,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.2.1.3 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry.hrSWRunID]
    {8,                          4,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.2.1.4 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry.hrSWRunPath]
    {8,                          5,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.2.1.5 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry.hrSWRunParameters]
    {8,                          6,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.2.1.6 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry.hrSWRunType]
    {8,                          7,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.4.2.1.7 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRun.hrSWRunTable.hrSWRunEntry.hrSWRunStatus]
    {5,                 5,         (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.5     [iso.org.dod.internet.mgmt.mib-2.host.hrSWRunPerf]
    {6,                    1,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.5.1   [iso.org.dod.internet.mgmt.mib-2.host.hrSWRunPerf.hrSWRunPerfTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.5.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRunPerf.hrSWRunPerfTable.hrSWRunPerfEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.5.1.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRunPerf.hrSWRunPerfTable.hrSWRunPerfEntry.hrSWRunPerfCPU]
    {8,                          2,(MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.5.1.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrSWRunPerf.hrSWRunPerfTable.hrSWRunPerfEntry.hrSWRunPerfMem]
    {5,                 6,         (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6     [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled]
    {6,                    1,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.6.1   [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledLastChange]
    {6,                    2,      (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.2.1.25.6.2   [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledLastUpdateTime]
    {6,                    3,      (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6.3   [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledTable]
    {7,                       1,   (MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6.3.1 [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledTable.hrSWInstalledEntry]
    {8,                          1,(MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6.3.1.1 [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledTable.hrSWInstalledEntry.hrSWInstalledIndex]
    {8,                          2,(MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6.3.1.2 [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledTable.hrSWInstalledEntry.hrSWInstalledName]
    {8,                          3,(MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6.3.1.3 [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledTable.hrSWInstalledEntry.hrSWInstalledID]
    {8,                          4,(MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6.3.1.4 [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledTable.hrSWInstalledEntry.hrSWInstalledType]
    {8,                          5,(MIB_R_ONLY),            NOT_ACCESSIBLE       },  // 1.3.6.1.2.1.25.6.3.1.5 [iso.org.dod.internet.mgmt.mib-2.host.hrSWInstalled.hrSWInstalledTable.hrSWInstalledEntry.hrSWInstalledDate]
    {2,       3,                   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.3            [iso.org.dod.internet.experimental]
    {2,       4,                   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.4            [iso.org.dod.internet.private]
    {3,          1,                (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.4.1          [iso.org.dod.internet.private.enterprises]

                                                                                     // 1.3.6.1.4.1.x.x.x    include project specific entries here

    {2,       5,                   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.5            [iso.org.dod.internet.security]
#if defined SUPPORT_SNMPV2C
    {2,       6,                   (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6            [iso.org.dod.internet.snmpV2]
    {3,          1,                (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1          [iso.org.dod.internet.snmpV2.snmpDomains]
    {4,             1,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1.1        [iso.org.dod.internet.snmpV2.snmpDomains.snmpUDPDomain]
    {4,             2,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1.2        [iso.org.dod.internet.snmpV2.snmpDomains.snmpCLNSDomain]
    {4,             3,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1.3        [iso.org.dod.internet.snmpV2.snmpDomains.snmpCONSDomain]
    {4,             4,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1.4        [iso.org.dod.internet.snmpV2.snmpDomains.snmpDDPDomain]
    {4,             5,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1.5        [iso.org.dod.internet.snmpV2.snmpDomains.snmpIPXDomain]
    {3,          2,                (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.2          [iso.org.dod.internet.snmpV2.snmpProxys]
    {4,             1,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1.1        [iso.org.dod.internet.snmpV2.snmpProxys.rfc1157Proxy]
    {5,                1,          (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.1.1.1      [iso.org.dod.internet.snmpV2.snmpProxys.rfc1157Proxy.rfc1157Domain]
    {3,          3,                (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3          [iso.org.dod.internet.snmpV2.snmpModules]
    {4,             1,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1        [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB]
    {5,                1,          (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1      [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects]
    {6,                   4,       (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.4    [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpTrap]
    {7,                      1,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.4.1  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpTrap.snmpTrapOID]
    {7,                      3,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.4.3  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpTrap.snmpTrapEnterprise]
    {6,                   5,       (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.5    [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpTraps]
    {7,                      1,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.5.1  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpTraps.coldStart]
    {7,                      2,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.5.2  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpTraps.warmStart]
    {7,                      5,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.5.5  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpTraps.authenticationFailure]
    {6,                   6,       (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.6    [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpSet]
    {7,                      1,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.1.6.1  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBObjects.snmpSet.snmpSetSerialNo]
    {5,                2,          (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2      [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance]
    {6,                   1,       (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.1    [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBCompliances]
    {7,                      2,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.1.2  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBCompliances.snmpBasicCompliance]
    {7,                      3,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.1.3  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBCompliances.snmpBasicComplianceRev2]
    {6,                   2,       (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2    [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups]
    {7,                      5,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.5  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.snmpSetGroup]
    {7,                      6,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.6  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.systemGroup]
    {7,                      7,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.7  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.snmpBasicNotificationGroup]
    {7,                      8,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.8  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.snmpGroup]
    {7,                      9,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.9  [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.snmpCommunityGroup]
    {7,                     10,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.10 [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.snmpObsoleteGroup]
    {7,                     11,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.11 [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.snmpWarmStartNotificationGroup]
    {7,                     12,    (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.1.2.2.12 [iso.org.dod.internet.snmpV2.snmpModules.snmpMIB.snmpMIBConformance.snmpMIBGroups.snmpSetGroup.snmpNotificationGroup]
    {4,            19,             (MIB_R_ONLY),            NOT_SUPPORTED        },  // 1.3.6.1.6.3.19       [iso.org.dod.internet.snmpV2.snmpModules.snmpv2tm]
#endif                
    {-1, 0, 0, 0},                                                       // end
};

extern void fnInitialiseSNMP(void)
{
    SNMP_manager_details[0].snmp_manager_ip_address[0] = 192;            // addresses of managers on management network
    SNMP_manager_details[0].snmp_manager_ip_address[1] = 168;
    SNMP_manager_details[0].snmp_manager_ip_address[2] = 0;
    SNMP_manager_details[0].snmp_manager_ip_address[3] = 35;
    SNMP_manager_details[1].snmp_manager_ip_address[0] = 192;
    SNMP_manager_details[1].snmp_manager_ip_address[1] = 168;
    SNMP_manager_details[1].snmp_manager_ip_address[2] = 0;
    SNMP_manager_details[1].snmp_manager_ip_address[3] = 99;
    SNMP_manager_details[2].snmp_manager_ip_address[0] = 192;
    SNMP_manager_details[2].snmp_manager_ip_address[1] = 168;
    SNMP_manager_details[2].snmp_manager_ip_address[2] = 0;
    SNMP_manager_details[2].snmp_manager_ip_address[3] = 97;
    SNMP_manager_details[0].snmp_manager_details = (defineNetwork(DEFAULT_NETWORK) | defineInterface(DEFAULT_IP_INTERFACE));
    SNMP_manager_details[1].snmp_manager_details = (defineNetwork(DEFAULT_NETWORK) | defineInterface(DEFAULT_IP_INTERFACE));
    SNMP_manager_details[2].snmp_manager_details = (defineNetwork(DEFAULT_NETWORK) | defineInterface(DEFAULT_IP_INTERFACE));
    SNMP_manager_details[0].ucSNMP_versions = (SNMP_MANAGER_V1 | SNMP_MANAGER_V2); // use SNMPv1 and SNMPv2c
    SNMP_manager_details[1].ucSNMP_versions = (SNMP_MANAGER_V2);     // use only SNMPv2c at this manager
    SNMP_manager_details[2].ucSNMP_versions = (SNMP_MANAGER_V1);     // use only SNMPv1 at this manager
    fnStartSNMP(fnSNMP_callback, (SNMP_MANAGER *)SNMP_manager_details, mib_table);
}

#endif
