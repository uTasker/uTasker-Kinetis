/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      zero_config.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    03.08.2011 Add optional simulator validation test and simulator state display  {1}
    03.08.2011 Ensure new link local address is used after collision during use {2}
    21.11.2011 Only defend the link-local address when TCP is enabled    {3}
    15.12.2012 zero config is presently restricted to a single network on the default interface - set these if modes enabled {4}

*/        

#include "config.h"

#ifdef USE_ZERO_CONFIG

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

//#define _VALIDATION_TEST                                               // {1} perform validation tests in simulator

#define OWN_TASK                         TASK_ZERO_CONFIG

#define ZERO_CONFIG_PROBE_WAIT           (DELAY_LIMIT)(0.5 * SEC)        // second   (initial random delay) 0..1s
#define ZERO_CONFIG_PROBE_NUM            3                               //          (number of probe packets)
#define ZERO_CONFIG_PROBE_MIN            1                               // second   (minimum delay till repeated probe)
#define ZERO_CONFIG_PROBE_NEXT           (DELAY_LIMIT)(1.5 * SEC)        // second   (initial random delay) 1..2s
#define ZERO_CONFIG_PROBE_MAX            2                               // seconds  (maximum delay till repeated probe)
#define ZERO_CONFIG_ANNOUNCE_WAIT        (DELAY_LIMIT)(2 * SEC)          // seconds  (delay before announcing)
#define ZERO_CONFIG_ANNOUNCE_NUM         2                               //          (number of announcement packets)
#define ZERO_CONFIG_ANNOUNCE_INTERVAL    2                               // seconds  (time between announcement packets)
#define ZERO_CONFIG_MAX_CONFLICTS        10                              //          (max conflicts before rate limiting)
#define ZERO_CONFIG_RATE_LIMIT_INTERVAL  (DELAY_LIMIT)(60.5 * SEC)       // seconds  (delay between successive attempts) after maximum conflicts
#define ZERO_CONFIG_DEFEND_INTERVAL      (DELAY_LIMIT)(10 * SEC)         // seconds  (minimum interval between defensive ARPs)

#define E_ZERO_CONFIG_START                    1
#define E_ZERO_CONFIG_PROBE                    2
#define E_ZERO_CONFIG_ANNOUNCE                 3
#define E_ZERO_CONFIG_DEFENSE_INTERVAL_EXPIRED 4
#define E_ZERO_CONFIG_COLLISION                5
#define E_ZERO_CONFIG_DEFEND                   6
#define E_ZERO_CONFIG_STOP                     7
#define E_ZERO_CONFIG_TEST_DEFENCE             8

#define ARP_PROBE                        0
#define ARP_ANNOUNCEMENT                 1
#define ARP_DEFENCE                      2


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnStateEventZeroConfig(unsigned char ucEvent);

#if defined _WINDOWS && defined _VALIDATION_TEST
    static void _Validate(unsigned char ucProgress, unsigned char ucProbeCount);
#else
    #define _Validate(x, y)
#endif


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static UTASK_TASK MasterTask = 0;
static unsigned char ucZeroConfigState = ZERO_CONFIG_OFF;
static unsigned char ucLinkLocalAddress[IPV4_LENGTH];                    // provisional link-local address
static unsigned char ucConflictCount = 0;




// Zero-configuration task
//
extern void fnZeroConfig(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input 
    unsigned char ucInputMessage[HEADER_LENGTH];                         // reserve space for receiving messages 

    if (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {         // check input queue
        if (TIMER_EVENT == ucInputMessage[MSG_SOURCE_TASK]) {            // only timer events expetced
            if (ucZeroConfigState > ZERO_CONFIG_OFF) {                   // timer events not allowed in the off state
                fnStateEventZeroConfig(ucInputMessage[MSG_TIMER_EVENT]); // handle timer event
            }
        }
    }
}

// This routine randomises the value to +/- 0.5 second of passed time value
//
static void fnRandomise(DELAY_LIMIT ZeroConfigTimeout, unsigned char ucTimerEvent)
{
    #ifdef RANDOM_NUMBER_GENERATOR
    DELAY_LIMIT random_sec = (unsigned short)(((unsigned long)fnRandom() * (1 * SEC)) / 0x10000);
    if (random_sec > (DELAY_LIMIT)(0.5 * SEC)) {
        ZeroConfigTimeout -= ((DELAY_LIMIT)(1 * SEC) - random_sec);      // decrease of 0..0.5s
    }
    else {
        ZeroConfigTimeout += random_sec;                                 // increase of 0..0.5s   
    }
    #else                                                                // simply change time +/- 0.25s each time
    static unsigned char random_sec = 0;
    if (random_sec != 0) {
        ZeroConfigTimeout += (DELAY_LIMIT)(0.25 * SEC);
    }
    else {
        ZeroConfigTimeout -= (DELAY_LIMIT)(0.25 * SEC);
    }
    random_sec ^= 1;
    #endif
    uTaskerMonoTimer(OWN_TASK, ZeroConfigTimeout, ucTimerEvent);                               
}

// Generate a random link-local address in the range 169.254.1.0 to 169.254.254.255
//
static void fnGenerateLinkLocalIP4V_address(void)
{
    #ifdef RANDOM_NUMBER_GENERATOR                                       // random number generator is recommended when zero-configuration is used
    unsigned short usIP_rand = fnRandom();
    usIP_rand ^= (network[DEFAULT_NETWORK].ucOurMAC[4] | network[DEFAULT_NETWORK].ucOurMAC[5]); // add additional entropy
    #else
    static unsigned short usIP_rand = 0;
    usIP_rand += (network[DEFAULT_NETWORK].ucOurMAC[4] | network.ucOurMAC[5]); // use MAC address as simple random number
    usIP_rand = ~usIP_rand;
    #endif
    ucLinkLocalAddress[0] = 169;
    ucLinkLocalAddress[1] = 254;
    if (!uMemcmp(network[DEFAULT_NETWORK].ucOurIP, ucLinkLocalAddress, 2)) { // if there is initially a default IP address in the link-local range try to use this as default
        uMemcpy(&ucLinkLocalAddress[2], &network[DEFAULT_NETWORK].ucOurIP[2], 2);
    }
    else {
        ucLinkLocalAddress[2] = (unsigned char)(usIP_rand >> 8);
        ucLinkLocalAddress[3] = (unsigned char)usIP_rand;
    }
    if ((ucLinkLocalAddress[2] < 1) || (ucLinkLocalAddress[2] > 254)) {  // avoid 169.254.0.x and 169.255.0.x range which is not allowed
        ucLinkLocalAddress[2] += 128;
    }
    uMemset(network[DEFAULT_NETWORK].ucDefGW, 0, IPV4_LENGTH);           // reset IP settings before starting
    uMemset(network[DEFAULT_NETWORK].ucOurIP, 0, IPV4_LENGTH);
    uMemset(network[DEFAULT_NETWORK].ucNetMask, 0, IPV4_LENGTH);
    fnDeleteArp();                                                       // ensure that the ARP cache is empty
}

// Send an ARP probe or announcement
//
static void fnProbeAddress(int iAnnounce)
{
    ARP_TAB temp;
    if (ARP_PROBE != iAnnounce) {                                        // announce or defense - using link local source IP address
        uMemcpy(network[DEFAULT_NETWORK].ucOurIP, ucLinkLocalAddress, IPV4_LENGTH); // temporarily set the link local IP address as source address in the ARP request
    }                                                                    // else probe with local source address of 0.0.0.0
#if IP_NETWORK_COUNT > 1                                                 // {4}
    temp.ucNetworkID = DEFAULT_NETWORK;
#endif
#if IP_INTERFACE_COUNT > 1                                               // {4}
    temp.ucInterface = 1;
#endif
    uMemcpy(temp.ucIP, ucLinkLocalAddress, IPV4_LENGTH);                 // set link local address as destination IP address
    fnSendARP_request(&temp);                                            // send an ARP request
    if (ARP_ANNOUNCEMENT == iAnnounce) {                                 // if we are sending an announcement we need to reset our IP address to 0.0.0.0 since it is not yet in use
        uMemset(network[DEFAULT_NETWORK].ucOurIP, 0x00, IPV4_LENGTH);    // reset local IP address to 0.0.0.0 after announcement sent
    }
}

// This function handles the zero-configuration state event machine
//
static void fnStateEventZeroConfig(unsigned char ucEvent)
{
    static unsigned char ucProbeCount = 0;

    switch (ucEvent) {
    case E_ZERO_CONFIG_COLLISION:
        fnInterruptMessage(MasterTask, ZERO_CONFIG_COLLISION);           // inform of a collision
        network[DEFAULT_NETWORK].ucOurIP[0] = 0;                         // {2} ensure new link local address is used
        // fall-through intentional
        //
    case E_ZERO_CONFIG_START:
        ucProbeCount = 0;
        ucZeroConfigState = ZERO_CONFIG_PROBING;
        fnGenerateLinkLocalIP4V_address();                               // generate a new link-local address which we will attempt to obtain
        if (++ucConflictCount > ZERO_CONFIG_MAX_CONFLICTS) {             // after the maximum number of conflicts the delay between attempts is increased
            ucConflictCount = ZERO_CONFIG_MAX_CONFLICTS;
            fnRandomise(ZERO_CONFIG_RATE_LIMIT_INTERVAL, E_ZERO_CONFIG_PROBE); // start probing after a delay
        }
        else {        
            fnRandomise(ZERO_CONFIG_PROBE_WAIT, E_ZERO_CONFIG_PROBE);    // start probing after a short delay
        }
        break;

    case E_ZERO_CONFIG_PROBE:                                            // probe our address
        if (ucProbeCount < ZERO_CONFIG_PROBE_NUM) {
            fnProbeAddress(ARP_PROBE);                                   // send an ARP probe to the link local address that we would like to use
            ucProbeCount++;
            fnRandomise(ZERO_CONFIG_PROBE_NEXT, E_ZERO_CONFIG_PROBE);    // start next probing after a delay
            _Validate(E_ZERO_CONFIG_PROBE, ucProbeCount);                // allow simulated collision during validation testing
        }
        else {                                                           // maximum number of probes have been performed
            ucProbeCount = 0;
            uTaskerMonoTimer(OWN_TASK, ZERO_CONFIG_ANNOUNCE_WAIT, E_ZERO_CONFIG_ANNOUNCE); // after waiting 2 seconds to ensure that there is no response we can announce the new address
        }
        break;

    case E_ZERO_CONFIG_ANNOUNCE:
        fnProbeAddress(ARP_ANNOUNCEMENT);                                // announce our address
        if (++ucProbeCount < ZERO_CONFIG_ANNOUNCE_NUM) {
            uTaskerMonoTimer(OWN_TASK, ZERO_CONFIG_ANNOUNCE_WAIT, E_ZERO_CONFIG_ANNOUNCE); // after waiting 2 seconds a further announcement is sent
        }
        else {
            ucZeroConfigState = ZERO_CONFIG_ACTIVE;                      // mark that the link-local address is being used
            uMemset(network[DEFAULT_NETWORK].ucNetMask, 255, 2);         // set sub-net mask to 255.255.0.0
            uMemcpy(network[DEFAULT_NETWORK].ucOurIP, ucLinkLocalAddress, IPV4_LENGTH); // enable use of the link local address
            fnInterruptMessage(MasterTask, ZERO_CONFIG_SUCCESSFUL);      // inform that zero configuration has completed successfully
        }
        _Validate(E_ZERO_CONFIG_ANNOUNCE, ucProbeCount);                 // allow simulated collision during validation testing
        break;

    case E_ZERO_CONFIG_DEFEND:
        ucZeroConfigState = ZERO_CONFIG_ACTIVE_DEFENDED;                 // mark that we have defended the link since a second defense is only allowed after the defense interval has expired
        fnProbeAddress(ARP_DEFENCE);                                     // send an announcement as defense
        uTaskerMonoTimer(OWN_TASK, ZERO_CONFIG_DEFEND_INTERVAL, E_ZERO_CONFIG_DEFENSE_INTERVAL_EXPIRED);
        fnInterruptMessage(MasterTask, ZERO_CONFIG_DEFENDED);            // inform of a defended link-local address
    #if defined _WINDOWS && defined _VALIDATION_TEST
        _Validate(ZERO_CONFIG_ACTIVE_DEFENDED, 0);                       // test defense again
    #endif
        break;

    case E_ZERO_CONFIG_DEFENSE_INTERVAL_EXPIRED:                         // defense interval expired
        if (ucZeroConfigState == ZERO_CONFIG_ACTIVE_DEFENDED) {
            ucZeroConfigState = ZERO_CONFIG_ACTIVE;                      // defending is allowed again
    #if defined _WINDOWS && defined _VALIDATION_TEST
            _Validate(ZERO_CONFIG_ACTIVE_DEFENDED, 0);                   // test defense again
    #endif
        }
        break;

    case E_ZERO_CONFIG_STOP:                                             // stop all zero-configuration activity
        ucZeroConfigState = ZERO_CONFIG_OFF;
        uTaskerStopTimer(OWN_TASK);                                      // stop any active timers
        ucConflictCount = 0;                                             // reset the conflict counter
        break;

    #if defined _WINDOWS && defined _VALIDATION_TEST
    case E_ZERO_CONFIG_TEST_DEFENCE:
        _Validate(0, 0);                                                 // test defense
        break;
    #endif
    }
    #ifdef _WINDOWS
    fnUpdateIPConfig();                                                  // update display in simulator
    #endif
}

// Handle a collision
//
static void fnZeroConfigCollision(void)
{
    if (ucZeroConfigState < ZERO_CONFIG_ACTIVE) {                        // collision before link-local address could be used or during a defend interval
    #if defined USE_TCP                                                  // {3}
        if (ucZeroConfigState == ZERO_CONFIG_ACTIVE_DEFENDED) {          // a collision during a defend interval
            fnActiveTCP_connections(RESET_CONNECTIONS);                  // allow all active TCP connections to send a reset before the link-local address is lost
        }
    #endif
        fnStateEventZeroConfig(E_ZERO_CONFIG_COLLISION);                 // restart the process
    }
    else {                                                               // collision during operation
        // If there are active TCP connections we will defend the link-local address if possible
        // otherwise the zero-configuration process is simply restarted
        //
    #if defined USE_TCP                                                  // {3}
        if (fnActiveTCP_connections(SEARCH_CONNECTION) != 0) {           // check active TCP connections to see whether we would prefer to defend the link-local address
            fnStateEventZeroConfig(E_ZERO_CONFIG_DEFEND);
        }
        else {                                                           // no defense is required
            fnStateEventZeroConfig(E_ZERO_CONFIG_COLLISION);             // restart the process
        }
    #else
        fnStateEventZeroConfig(E_ZERO_CONFIG_COLLISION);                 // restart the process
    #endif
    }
}


// Handle an ARP reception to check for collision
//
extern void fnCheckZeroConfigCollision(unsigned char *ptrData)
{
    if (ucZeroConfigState > ZERO_CONFIG_OFF) {                           // only check when zero-configuration is active
        if (uMemcmp(ucLinkLocalAddress, (ptrData + MAC_LENGTH), IPV4_LENGTH) != 0) { // if source IP address of ARP request or response is not equal to the link local address being used it is not a collision
            return;                                                      // no collision
        }
        if (ucZeroConfigState == ZERO_CONFIG_PROBING) {
            if (uMemcmp(ucLinkLocalAddress, (ptrData + (2 * MAC_LENGTH) + IPV4_LENGTH), IPV4_LENGTH) != 0) { // if destination IP address of ARP request or response is not equal to the link local address being used it is not a collision
                return;                                                  // no collision
            }
        }
        if (uMemcmp(network[DEFAULT_NETWORK].ucOurMAC, ptrData, MAC_LENGTH) == 0) { // if source MAC address of ARP request or response is equal to the local HW address it is not a collision
            return;                                                      // no collision
        }
        fnZeroConfigCollision();                                         // collision detected
    }
}

// Start the zero configuration process
//
extern void fnStartZeroConfig(UTASK_TASK Task)
{
    MasterTask = Task;                                                   // save the task to inform in case of zero-configuration events
    fnStateEventZeroConfig(E_ZERO_CONFIG_START);
}

// Stop the zero configuration process
//
extern void fnStopZeroConfig(void)
{
    fnStateEventZeroConfig(E_ZERO_CONFIG_STOP);
}

#if defined _WINDOWS
extern unsigned char fnGetZeroConfig_State(int iNetwork)                 // {1}
{
    if (iNetwork != 0) {                                                 // only support network 0
        return 0;
    }
    return ucZeroConfigState;
}
#endif

#if defined _WINDOWS && defined _VALIDATION_TEST                         // {1}

static void _generate_collision(void)
{
    ETHERNET_ARP_FRAME apr_frame;

    uMemset(apr_frame.ethernet_header.destination_mac_address, 0xff, MAC_LENGTH);
    uMemcpy(apr_frame.ethernet_header.source_mac_address, network[DEFAULT_NETWORK].ucOurMAC, MAC_LENGTH);
    apr_frame.ethernet_header.source_mac_address[MAC_LENGTH - 1]++;      // make different from our own MAC address
    apr_frame.ethernet_header.ethernet_type[0] = (unsigned char)(PROTOCOL_ARP >> 8);
    apr_frame.ethernet_header.ethernet_type[1] = (unsigned char)PROTOCOL_ARP;

    apr_frame.arp_content.ucHardware_size = 6;
    apr_frame.arp_content.ucHardwareType[0] = (unsigned char)(ETHERNET_HARDWARE >> 8);
    apr_frame.arp_content.ucHardwareType[1] = (unsigned char)(ETHERNET_HARDWARE);
    apr_frame.arp_content.ucOpCode[0] = 0x00;
    apr_frame.arp_content.ucOpCode[1] = 0x01;                            // request
    apr_frame.arp_content.ucProtocolSize = 0x04;
    apr_frame.arp_content.ucProtocolType[0] = (unsigned char)(PROTOCOL_IPv4 >> 8);
    apr_frame.arp_content.ucProtocolType[1] = (unsigned char)(PROTOCOL_IPv4);
    uMemcpy(apr_frame.arp_content.ucSender_IP_address, ucLinkLocalAddress, IPV4_LENGTH); // collison address    
    uMemcpy(apr_frame.arp_content.ucSenderMAC_address, apr_frame.ethernet_header.source_mac_address, MAC_LENGTH);
    uMemset(apr_frame.arp_content.ucTargetMAC_address, 0, MAC_LENGTH);
    uMemcpy(apr_frame.arp_content.ucTarget_IP_address, ucLinkLocalAddress, IPV4_LENGTH); // collison address
    
    if (fnWrite(Ethernet_handle[0], (unsigned char *)&apr_frame, sizeof(ETHERNET_ARP_FRAME))) { // prepare message
        fnWrite(Ethernet_handle[0], 0, 0);                                  // release message
    }
}

// The validation test verifies that collisions taking place suring the process are handled correctly
// 20s after the link-local address has been accepted a further collison occurs so that the defense can be verified (when a TCP socket uses the address) or the
// process restarts correctly
//
static void _Validate(unsigned char ucProgress, unsigned char ucProbeCount)
{
    static int iState = 0;
    switch (iState) {
    case 0:
        if (ucProbeCount == 1) {                                         // collision on first probe
            _generate_collision();
            iState = 1;
        }
        break;

    case 1:
        if (ucProbeCount == 2) {                                         // collision on second probe
            _generate_collision();
            iState = 2;
        }
        break;

    case 2:
        if (ucProbeCount == 3) {                                         // collision during announce wait time
            _generate_collision();
            iState = 3;
        }
        break;

    case 3:
        if ((ucProgress == E_ZERO_CONFIG_ANNOUNCE) && (ucProbeCount == 1)) { // collision during announce
            _generate_collision();
            iState = 4;
        }
        break;

    case 4:
        if ((ucProgress == E_ZERO_CONFIG_ANNOUNCE) && (ucProbeCount == ZERO_CONFIG_ANNOUNCE_NUM)) { // address now being used
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(20 * SEC), E_ZERO_CONFIG_TEST_DEFENCE); // test defending address after 20s to allow TCP cpnnections to be established if desired
            iState = 5;
        }
        break;
    case 5:
        _generate_collision();                                           // test defence
        iState = 6;
        break;
    case 6:
        iState = 7;                                                      // ignore after defence
        break;
    case 7:
        _generate_collision();                                           // test defence again
        iState = 8;
        break;
    case 8:
        _generate_collision();                                           // test defence again within defence time
        iState = 9;                                                      // end
        break;
    }
}
#endif

#endif

