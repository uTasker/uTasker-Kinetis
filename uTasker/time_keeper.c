/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      time_keeper.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    19.08.2015 Use uMemset() to initialise strcture to avoid GCC using memset() {1}
    15.09.2015 Add dusk and dawn calculations                            {2}

*/
          
#include "config.h"
#if defined DUSK_AND_DAWN
    #include <math.h>
#endif

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                  TASK_TIME_KEEPER
#if !defined ALARM_TASK
    #define ALARM_TASK             OWN_TASK
#endif

#if defined SUPPORT_SW_RTC && (defined USE_SNTP || defined USE_TIME_SERVER) || (defined USE_SNTP && defined USE_TIME_SERVER)
    #if !defined GLOBAL_TIMER_TASK
        #error "GLOBAL_TIMER_TASK is required for multiple task timers in the timer keeper!"
    #endif
    #define UTASKERMONOTIMER(a, b, c) uTaskerGlobalMonoTimer(a, b, c)    // multiple timers required
#else
    #define UTASKERMONOTIMER(a, b, c) uTaskerMonoTimer(a, b, c)          // single task timer adequate
#endif

#define SNTP_DISPLAY_TIME_STAMPS                                         // activate for SNTP debug output

#define NTP_TO_1970_TIME          2208988800u                            // seconds from the start of NTP (01.01.1900 00:00:00 to 01.01.1970 00:00:00 GMT)
#define LEAP_YEAR_SECONDS         (366 * 24 * 60 * 60)                   // seconds in a leap year
#define COMMON_YEAR_SECONDS       (365 * 24 * 60 * 60)                   // seconds in a common year
#define LEAP_YEAR(year)           ((year % 4)==0)                        // valid from 1970 to 2038
#define LEAP_YEAR_DAYS            (366)
#define COMMON_YEAR_DAYS          (365)

#define DAYLIGHT_SAVING_SECONDS   (1 * 60 * 60)                          // 0 or +1 hour
#define QUARTER_OF_HOURS_SECONDS  (15 * 60)                              // seconds in a quarter of an hour

#define SNTP_STATE_IDLE           0
#define SNTP_STATE_RESOLVING      1
#define SNTP_STATE_NO_RESPONSE    2
#define SNTP_STATE_SUCCESSFUL     3

#define MAX_SNTP_SERVER_ATTEMPTS  5                                      // maximum attempts made to retrieve the time from a single SNTP server

#define FIRST_SNTP_REQUEST_DELAY   (DELAY_LIMIT)(5.0 * SEC)              // delay between starting RTC and requesting first SNTP synchronisation
#define SNTP_RETRY_DELAY           (DELAY_LIMIT)(15 * SEC)               // retry time if there is no answer
#define SNTP_PAUSE_DELAY           (DELAY_LIMIT)(5 * 60 * SEC)           // retry after a pause
#if !defined SNTP_RESYNC_PERIOD
    #define SNTP_RESYNC_PERIOD     (DELAY_LIMIT)(60 * 60 * SEC)          // a new synchronisation is performed periodically at this rate
#endif

#define REFERENCE_TIME (0xce25e413 - (55 + (21 * 60) + (23 * 60 * 60)))  // 6th August 2009 at 0:0:0 (23:21:55)

// Hardware specific defines
//
#if defined _M5223X
    #define TIMER_PERIOD_FULL_SCALE    1000000                           // timer counts us
    #define GET_TIMER_PRESENT_VALUE()  DTCN1                             // counter value (0..999999)
    #define RESET_US_COUNTER()         DTCN1 = 0                         // reset/synchronise the us counter
#elif defined _KINETIS
    #define GET_TIMER_PRESENT_VALUE() 0
    #define RESET_US_COUNTER()
    #define TIMER_PERIOD_FULL_SCALE   1
#endif


#define E_RTC_OSC_STAB_DELAY      1                                      // time keeper timer events
#define E_FIRST_TIME_SYNC_SNTP    2
#define E_NEXT_TIME_SYNC_SNTP     3
#define E_FIRST_TIME_SERVER       4
#define E_NEXT_TIME_SERVER        5
#define E_SECOND_TICK             6


// Some well know SNTP server addresses for reference
//
// 194.0.229.52                                                          // stratum 1 - ntpstm.netbone-digital.com St. Moritz
// 131.188.3.220                                                         // stratum 1 - ntp0.fau.de University Erlangen-Nuernberg, D-91058 Erlangen, FRG
// 194.42.48.120                                                         // stratum 2 - clock.tix.ch CH-8005 Zurich, Switzerland
// 207.46.232.182                                                        // time-b.nist.gov

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stSNTP_MESSAGE
{
    UDP_HEADER     tUDP_Header;                                          // reserve header space
    NTP_FRAME      sntp_data_content;                                    // reserve message space
} SNTP_MESSAGE;

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined SUPPORT_RTC || defined USE_SNTP || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC
static const unsigned char monthDays[] = {
    (31),
    (28),                                                                // February without leap day
    (31),
    (30),
    (31),
    (30),
    (31),
    (31),
    (30),
    (31),
    (30),
    (31)
};
#endif

#if defined USE_SNTP || defined USE_TIME_SERVER
static const signed char cTIME_ZONE[] = {                                // time zone expressed in quarter of hours
    (-12 * 4),                                                           // UTC-12:00
    (-11 * 4),                                                           // UTC-11:00
    (-10 * 4),                                                           // UTC-10:00
    (-9 * 4),                                                            // UTC-9:00
    (-8 * 4),                                                            // UTC-8:00
    (-7 * 4),                                                            // UTC-7:00
    (-6 * 4),                                                            // UTC-6:00
    (-5 * 4),                                                            // UTC-5:00
    ((-9 * 4)/2),                                                        // UTC-4:30 Caracas
    (-4 * 4),                                                            // UTC-4:00
    ((-7 * 4)/2),                                                        // UTC-3:30 Newfoundland
    (-3 * 4),                                                            // UTC-3:00
    (-2 * 4),                                                            // UTC-2:00
    (-1 * 4),                                                            // UTC-1:00
    (0 * 4),                                                             // UTC+0:00
    (1 * 4),                                                             // UTC+1:00
    (2 * 4),                                                             // UTC+2:00
    (3 * 4),                                                             // UTC+3:00
    ((7 * 4)/2),                                                         // UTC+3:30 Tehran
    (4 * 4),                                                             // UTC+4:00
    ((9 * 4)/2),                                                         // UTC+4:30 Kabul
    (5 * 4),                                                             // UTC+5:00
    ((11 * 4)/2),                                                        // UTC+5:30 New Delhi
    ((23 * 4)/4),                                                        // UTC+5:45 Catmandu
    (6 * 4),                                                             // UTC+6:00
    ((13 * 4)/2),                                                        // UTC+6:30 Rangoon
    (7 * 4),                                                             // UTC+7:00
    (8 * 4),                                                             // UTC+8:00
    (9 * 4),                                                             // UTC+9:00
    ((19 * 4)/2),                                                        // UTC+9:30 Darwin
    (10 * 4),                                                            // UTC+10:00
    (11 * 4),                                                            // UTC+11:00
    (12 * 4),                                                            // UTC+12:00
    (13 * 4)                                                             // UTC+13:00
};

static const SNTP_TIME zero_time = {0};
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined USE_SNTP
    static void fnSynchroniseLocalTime(SNTP_TIME *NewTime, unsigned long ulSeconds);
    static int  fnSNTP_client(USOCKET c, unsigned char uc, unsigned char *ucIP, unsigned short us, unsigned char *data, unsigned short us2);
    static int  fnRequestSNTP_time(int iTryNext);
#endif

#if defined USE_TIME_SERVER
    #if defined SUPPORT_LCD
        static void fnDisplayTime(void);
    #endif
    static void fnRequestTimeServer(void);
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */
#if defined SUPPORT_RTC || defined USE_SNTP || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC
    static unsigned long  ulLocalUTC = 0;                                // local seconds count value (UTC plus timezone and daylight saving adjustment)
    static unsigned short usYear = 1970;                                 // local counters for maintaining simplified time and date
    static unsigned char  ucMonthOfYear = 1;                             // January
    static unsigned char  ucDayOfMonth = 1;                              // 1st
    static unsigned char  ucDayOfWeek = 4;                               // 1970 was a common year (not leap year) starting on a Thursday
    static unsigned char  ucSeconds = 0;                                 // 00:00:00
    static unsigned char  ucMinutes = 0;
    static unsigned char  ucHours = 0;
#endif

#if defined SUPPORT_RTC
    static void (*_seconds_callback)(void) = 0;
#endif

#if defined USE_SNTP
    static USOCKET SNTP_Socket = -1;
    static unsigned long ulFractionResync = 0;
    static signed long   slFractionOffset = 0;                           // offset to hardware timer counter and its interrupt
    static int iSNTP_reps = 0;
    static SNTP_TIME sntp_update_time = {0xd7e00621, 0x00000000};        // time in the past as default (08.10.2014)
    static SNTP_TIME clientRequestTime = {0};                            // time of last client request
    static unsigned char ucServerIndex = 0;                              // the server presently being requested from
    #if SNTP_SERVERS > 1
    static unsigned char ucAvoidServer[SNTP_SERVERS] = {0};
    #endif
#endif

#if defined USE_TIME_SERVER
    static unsigned long ulTimeServerTime = 0;
    static unsigned char ucTimeServerTry = 0;
    static USOCKET TIME_TCP_socket = -1;
#endif



#if defined SUPPORT_RTC || defined USE_SNTP || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC
// Time keeping task
//
extern void fnTimeKeeper(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE        PortIDInternal = ptrTaskTable->TaskID;           // queue ID for task input
    unsigned char       ucInputMessage[SMALL_MESSAGE];                   // reserve space for receiving messages

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on message source
    #if (defined SUPPORT_RTC  || defined SUPPORT_SW_RTC) && (ALARM_TASK == OWN_TASK)
        case INTERRUPT_EVENT:
            if (RTC_ALARM_INTERRUPT_EVENT == ucInputMessage[MSG_TIMER_EVENT]) {
        #if defined SUPPORT_LOW_POWER
                fnSetLowPowerMode(WAIT_MODE);
        #endif
                fnDebugMsg("RTC Alarm fired\r\n");
            }
            break;
    #endif
        case TIMER_EVENT:
            switch (ucInputMessage[MSG_TIMER_EVENT]) {
    #if defined SUPPORT_RTC
            case E_RTC_OSC_STAB_DELAY:                                   // the real time clock oscillator required a stabilisation delay after being started for the first time
                fnStartRTC(_seconds_callback);                           // RTC oscillator needed to be started and a stabilisation delay has passed - contnue
                break;
    #endif
    #if defined USE_SNTP
            case E_FIRST_TIME_SYNC_SNTP:
                fnRequestSNTP_time(0);
                break;
            case E_NEXT_TIME_SYNC_SNTP:
                fnRequestSNTP_time(1);                                   // request next synchronisation from next server
                break;
    #endif
    #if defined USE_TIME_SERVER
            case E_FIRST_TIME_SERVER:
                fnRequestTimeServer();
                break;
    #endif
    #if defined SUPPORT_SW_RTC || (defined USE_TIME_SERVER && !defined SUPPORT_RTC)
            case E_SECOND_TICK:
                {
        #if defined SUPPORT_SW_RTC
                    RTC_SETUP rtc_setup;
                    rtc_setup.command = (RTC_TICK_SEC | RTC_INCREMENT);  // increment the RTC seconds
                    fnConfigureRTC(&rtc_setup);
        #else
                    fnSecondsTick(0, 0);
        #endif
        #if defined SUPPORT_LCD
                    fnDisplayTime();                                     // update the time in the display
        #endif
                    UTASKERMONOTIMER(OWN_TASK, (DELAY_LIMIT)(1 * SEC), E_SECOND_TICK); // start next seconds interval
                }
                break;
    #endif
            }
            break;
    #if defined USE_SNTP
        case TASK_ARP:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the contents
            switch (ucInputMessage[0]) {                                 // ARP sends us either ARP resolution success or failed
            case ARP_RESOLUTION_SUCCESS:                                 // IP address has been resolved (repeat UDP frame).
                fnRequestSNTP_time(0);                                   // repeat SNTP request after address has been resolved
                break;

            case ARP_RESOLUTION_FAILED:                                  // IP address could not be resolved... (server not in local network or no gateway)
                UTASKERMONOTIMER(OWN_TASK, SNTP_PAUSE_DELAY, E_FIRST_TIME_SYNC_SNTP); // try again after a longer delay in case the server is ignoring us at the moment
                break;
            }
            break;
    #endif
        }
    }
}
#endif


#if defined SUPPORT_RTC || defined USE_SNTP || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC
// RTC alarm callback interrupt
//
static void fnAlarmFired(void)
{
    fnInterruptMessage(ALARM_TASK, RTC_ALARM_INTERRUPT_EVENT);
}

// Add week days and keep track of the day of the week (0..6 for Sunday, Monday, ... Saturday)
//
static unsigned char fnIncDayOfWeek(unsigned char ucDayOfWeek, unsigned char ucInc)
{
    ucDayOfWeek += ucInc;
    while (ucDayOfWeek > 6) {
        ucDayOfWeek -= 7;
    }
    return ucDayOfWeek;
}

// RTC seconds interrupt (interrupt)
// - this routine is called inherently when the RTC operates and is not an application callback
//
extern void fnSecondsTick(void (*rtc_interrupt_handler[6])(void), int rtc_interrupts)
{
    #if defined USE_SNTP
    if (ulFractionResync != 0) {                                         // just synchronised so we can synchronise the time base to correct form fractions of a second
        slFractionOffset = (GET_TIMER_PRESENT_VALUE() - ulFractionResync); // the fraction adjustment
        ulFractionResync = 0;                                            // synchronisation performed
    }
    RESET_US_COUNTER();                                                  // reset the us conter value
    #endif
    ulLocalUTC++;                                                        // local count of UTC seconds value
    if (++ucSeconds >= 60) {
        if (++ucMinutes >= 60) {
            if (++ucHours >= 24) {                                       // new day
                ucHours = 0;
                ++ucDayOfMonth;
                ucDayOfWeek = fnIncDayOfWeek(ucDayOfWeek, 1);
                if ((ucDayOfMonth == 29) && (ucMonthOfYear == 2)) {      // 29th Februar
                    if ((LEAP_YEAR(usYear) == 0)) {
                        ucDayOfMonth = 1;
                        ucMonthOfYear = 3;
                    }
                    else {
                        if (ucDayOfMonth > monthDays[ucMonthOfYear - 1]) {
                            ucDayOfMonth = 1;
                            ucMonthOfYear++;
                            if (ucMonthOfYear > 12) {
                                ucMonthOfYear = 1;                       // roll over to new year
                                usYear++;
                            }
                        }
                    }
                }
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
                if (rtc_interrupts & RTC_DAY_INT) {                      // if days interrupt enabled
                    if (rtc_interrupt_handler[3] != 0) {                 // days interrupt call back
                        uDisable_Interrupt();
                            rtc_interrupt_handler[3]();                  // call handling function (days interrupt)
                        uEnable_Interrupt();
                    }
                }
    #endif
            }
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
            if (rtc_interrupts & RTC_HOUR_INT) {                         // hours interrupt enabled
                if (rtc_interrupt_handler[5] != 0) {                     // hours interrupt call back
                    uDisable_Interrupt();
                        rtc_interrupt_handler[5]();                      // call handling function (hours interrupt)
                    uEnable_Interrupt();
                }
            }
    #endif
            ucMinutes = 0;
        }
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
        if (rtc_interrupts & RTC_MINUTE_INT) {                           // minutes interrupt enabled
            if (rtc_interrupt_handler[1] != 0) {                         // minutes interrupt call back
                uDisable_Interrupt();
                    rtc_interrupt_handler[1]();                          // call handling function (minutes interrupt)
                uEnable_Interrupt();
            }
        }
    #endif
        ucSeconds = 0;
    }
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
    if (rtc_interrupts & RTC_ALARM_INT_MATCH) {
        if (rtc_interrupt_handler[2] != 0) {
            uDisable_Interrupt();
                rtc_interrupt_handler[2]();                              // call handling function (alarm match interrupt)
            uEnable_Interrupt();
        }
    }
    if (rtc_interrupts & RTC_STOPWATCH_MATCH) {                          // stopwatch interrupt enabled
        if (rtc_interrupt_handler[0] != 0) {                             // stopwatch interrupt call back
            uDisable_Interrupt();
                rtc_interrupt_handler[0]();                              // call handling function (stopwatch interrupt)
            uEnable_Interrupt();
        }
    }
    if (rtc_interrupt_handler[4] != 0) {                                 // seconds interrupt call back
        uDisable_Interrupt();
            rtc_interrupt_handler[4]();                                  // call handling function (seconds interrupt)
        uEnable_Interrupt();
    }
        #if defined SUPPORT_LCD && defined USE_TIME_SERVER
    if (ulTimeServerTime != 0) {
        fnDisplayTime();
    }
        #endif
    #endif
}

extern void fnGetRTC(RTC_SETUP *ptrSetup)
{
    do {                                                                 // repeat in case seconds increment while copying
        ptrSetup->ucSeconds = ucSeconds;
        ptrSetup->usYear = usYear;
        ptrSetup->ucMonthOfYear = ucMonthOfYear;
        ptrSetup->ucDayOfMonth = ucDayOfMonth;
        ptrSetup->ucDayOfWeek = ucDayOfWeek;
        ptrSetup->ucHours = ucHours;
        ptrSetup->ucMinutes = ucMinutes;
        ptrSetup->ulLocalUTC = ulLocalUTC;
    } while (ptrSetup->ucSeconds != ucSeconds);
    ptrSetup->command = RTC_RETURNED_TIME;
}


extern unsigned char fnSetShowTime(int iSetDisplay, CHAR *ptrInput)
{
    RTC_SETUP rtc_setup;// = {0};
    CHAR *ptrString = ptrInput;
    int iRelativeAlarm = 0;
    uMemset(&rtc_setup, 0, sizeof(rtc_setup));                           // {1} start with blank structure
    if (iSetDisplay == DISPLAY_RTC_ALARM) {
        rtc_setup.command = RTC_GET_ALARM;
        if (ptrInput != 0) {
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
            fnConfigureRTC(&rtc_setup);                                  // get the alarm time set
    #endif
            if (rtc_setup.ulLocalUTC == 0) {                             // if the seconds value is zero it means that no alarm is enabed
                return (uStrcpy(ptrInput, "No alarm enabled") - ptrInput); // the display length
            }
        }
    }
    else {
        fnGetRTC(&rtc_setup);                                            // get the local time structure content
    }
    if (iSetDisplay & (DISPLAY_RTC_TIME_DATE | DISPLAY_RTC_ALARM)) {     // display time and date
        CHAR *ptrTimeBuffer = ptrInput;
        if (ptrInput != 0) {                                             // if buffer specified
            if (iSetDisplay & (DISPLAY_RTC_DATE | DISPLAY_RTC_ALARM)) {
                ptrTimeBuffer = fnBufferDec(rtc_setup.ucDayOfMonth, 0, ptrTimeBuffer);
                *ptrTimeBuffer++ = '.';
                ptrTimeBuffer = fnBufferDec(rtc_setup.ucMonthOfYear, LEADING_ZERO, ptrTimeBuffer);
                *ptrTimeBuffer++ = '.';
                ptrTimeBuffer = fnBufferDec(rtc_setup.usYear, 0, ptrTimeBuffer);
            }
            if (iSetDisplay & (DISPLAY_RTC_TIME | DISPLAY_RTC_ALARM)) {
                if (iSetDisplay & (DISPLAY_RTC_DATE | DISPLAY_RTC_ALARM)) {
                    *ptrTimeBuffer++ = ' ';
                }
                ptrTimeBuffer = fnBufferDec(rtc_setup.ucHours, (LEADING_ZERO), ptrTimeBuffer);
                *ptrTimeBuffer++ = ':';
                ptrTimeBuffer = fnBufferDec(rtc_setup.ucMinutes, (LEADING_ZERO), ptrTimeBuffer);
                *ptrTimeBuffer++ = ':';
                ptrTimeBuffer = fnBufferDec(rtc_setup.ucSeconds, (LEADING_ZERO), ptrTimeBuffer);
            }
        }
        return (ptrTimeBuffer - ptrInput);                               // the display length
    }
    if (ptrString != 0) {                                                // a zero pointer will cause the time/date to be set to zero (clearing an alarm)
        do {
            unsigned char ucDay_hour;
            unsigned char ucMinute_month = 0;
            unsigned short usSecond_year = 0;
            if (SET_RTC_ALARM_TIME == iSetDisplay) {
                if (*ptrString == '+') {                                 // set an absolute alarm time
                    ptrString++;
                    while (*ptrString == ' ') {
                        ptrString++;
                    }
                    iRelativeAlarm = 1;
                }
            }
            else if (iSetDisplay == (SET_RTC_ALARM_TIME | SET_RTC_DATE)) {
                iSetDisplay = (SET_RTC_ALARM_TIME);
            }
            ucDay_hour = (unsigned char)fnDecStrHex(ptrString);          // hour or day
            while ((*ptrString >= '0') && (*ptrString <= '9')) {
                ptrString++;
            }
            if (*ptrString != 0) {
                ptrString++;                                             // move over ':'
                ucMinute_month = (unsigned char)fnDecStrHex(ptrString);  // minute or month
                while ((*ptrString >= '0') && (*ptrString <= '9')) {
                    ptrString++;
                }
                if (iRelativeAlarm != 0) {
                    iRelativeAlarm++;
                }
            }
            if (*ptrString != 0) {
                ptrString++;                                             // move over ':'
                usSecond_year = (unsigned short)fnDecStrHex(ptrString);  // seconds or year
                if (iRelativeAlarm != 0) {
                    iRelativeAlarm++;
                }
            }
            if (SET_RTC_ALARM_TIME == iSetDisplay) {
                while ((*ptrString >= '0') && (*ptrString <= '9')) {
                    ptrString++;
                }
                if (*ptrString == ' ') {
                    ptrString++;
                    if ((*ptrString >= '0') && (*ptrString <= '9')) {
                        iSetDisplay |= SET_RTC_DATE;
                    }
                }
            }
            if (SET_RTC_DATE & iSetDisplay) {
                rtc_setup.ucDayOfMonth = ucDay_hour;
                rtc_setup.ucMonthOfYear = ucMinute_month;
                rtc_setup.usYear = usSecond_year;
                if (rtc_setup.usYear < 2000) {
                    rtc_setup.usYear += 2000;
                }
            }
            else {                                                       // set time or alarm time
                switch (iRelativeAlarm) {
                case 0:                                                  // not relative
                    rtc_setup.ucHours = ucDay_hour;
                    rtc_setup.ucMinutes = ucMinute_month;
                    rtc_setup.ucSeconds = (unsigned char)usSecond_year;
                    break;
                case 1:                                                  // relative seconds
                    rtc_setup.ulLocalUTC += ucDay_hour;
                    break;
                case 2:                                                  // relative minutes:seconds
                    rtc_setup.ulLocalUTC += (ucDay_hour * 60);
                    rtc_setup.ulLocalUTC += ucMinute_month;
                    break;
                case 3:                                                  // relative hours:minutes:seconds
                    rtc_setup.ulLocalUTC += (ucDay_hour * 60 * 60);
                    rtc_setup.ulLocalUTC += (ucMinute_month * 60);
                    rtc_setup.ulLocalUTC += (unsigned char)usSecond_year;
                    break;
                }
                if (iRelativeAlarm != 0) {
                    fnConvertSecondsTime(&rtc_setup, rtc_setup.ulLocalUTC);
                }
            }
        } while (iSetDisplay == (SET_RTC_ALARM_TIME | SET_RTC_DATE));
    }
    if (SET_RTC_ALARM_TIME == iSetDisplay) {
        rtc_setup.int_handler = fnAlarmFired;                            // alarm callback
        rtc_setup.command = RTC_ALARM_TIME;                              // set an alarm time to the RTC
    }
    else {
        rtc_setup.command = RTC_TIME_SETTING;                            // set the time to the RTC
    }
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
    fnConfigureRTC(&rtc_setup);                                          // save new value to RTC
    #endif
    return 0;
}

// Convert UTC seconds from 1970 to present time and date
//
extern void fnConvertSecondsTime(RTC_SETUP *ptr_rtc_setup, unsigned long ulSecondsTime)
{
    int iLeapYear = 0;
    unsigned long ulOriginalSecondsValue = ulSecondsTime;
    unsigned char _ucMonthOfYear = 0;
    unsigned char _ucDayOfMonth = 1;
    unsigned char _ucDayOfWeek = 4;                                      // 1970 was a common year (not leap year) starting on a Thursday
    unsigned char _ucSeconds = 0;
    unsigned char _ucMinutes = 0;
    unsigned char _ucHours = 0;
    unsigned short _usYear = 1970;                                       // referenced to 1970
    if (ptr_rtc_setup == 0) {
        ulLocalUTC = ulSecondsTime;                                      // set the local seconds count
    }
    while (1) {
        iLeapYear = LEAP_YEAR(_usYear);
        if (iLeapYear != 0) {                                            // if present year is a leap year
            if (ulSecondsTime > LEAP_YEAR_SECONDS) {
                ulSecondsTime -= LEAP_YEAR_SECONDS;
                _usYear++;                                               // count the years past
                _ucDayOfWeek = fnIncDayOfWeek(_ucDayOfWeek, 2);
                continue;
            }
        }
        else {                                                           // common year
            if (ulSecondsTime > COMMON_YEAR_SECONDS) {
                ulSecondsTime -= COMMON_YEAR_SECONDS;
                _usYear++;                                               // count the years past (since 1970)
                _ucDayOfWeek = fnIncDayOfWeek(_ucDayOfWeek, 1);
                continue;
            }
        }

        // In final year (remaining seconds value is in the year)
        //
        while (1) {
            if ((iLeapYear != 0) && (_ucMonthOfYear == 1)) {             // February of a leap year
                if (ulSecondsTime > (29 * 24 * 60 * 60)) {
                    ulSecondsTime -= (29 * 24 * 60 * 60);
                    _ucDayOfWeek = fnIncDayOfWeek(_ucDayOfWeek, 29);
                    _ucMonthOfYear++;
                    continue;
                }
            }
            else {
                if (ulSecondsTime > ((unsigned long)monthDays[_ucMonthOfYear] * 24 * 60 * 60)) { // the seconds in this month
                    ulSecondsTime -= ((unsigned long)monthDays[_ucMonthOfYear] * 24 * 60 * 60);
                    _ucDayOfWeek = fnIncDayOfWeek(_ucDayOfWeek, monthDays[_ucMonthOfYear]); // {69}
                    _ucMonthOfYear++;
                    continue;
                }
            }

            // In final month (remaining seconds value is in the month)
            //
            _ucMonthOfYear++;                                            // change from index to month (1..12)
            while (1) {
                if (ulSecondsTime > (60 * 60 * 24)) {                    // seconds in a day
                    ulSecondsTime -= (60 * 60 * 24);                     // for each day in final month
                    _ucDayOfMonth++;
                    _ucDayOfWeek = fnIncDayOfWeek(_ucDayOfWeek, 1);
                    continue;
                }

                // In final day (remaining seconds value is in the day)
                //
                while (1) {
                    if (ulSecondsTime >= (60 * 60)) {
                        ulSecondsTime -= (60 * 60);                      // for each hour in final day
                        _ucHours++;
                        continue;
                    }

                    // In final hour (remaining seconds value in the hour)
                    //
                    while (1) {
                        if (ulSecondsTime >= (60)) {                     // {100a} changed to >= to avoid 60 seconds !!
                            ulSecondsTime -= (60);                       // for each minute in final hour
                            _ucMinutes++;
                            continue;
                        }

                        // In final minute (remaining seconds can be used directly)
                        //
                        _ucSeconds = (unsigned char)ulSecondsTime;
                        if (ptr_rtc_setup != 0) {                        // put the time and data to the calling structure
                            ptr_rtc_setup->ucMonthOfYear = _ucMonthOfYear;
                            ptr_rtc_setup->ucDayOfMonth = _ucDayOfMonth;
                            ptr_rtc_setup->ucDayOfWeek = _ucDayOfWeek;
                            ptr_rtc_setup->ucSeconds = _ucSeconds;
                            ptr_rtc_setup->ucMinutes = _ucMinutes;
                            ptr_rtc_setup->ucHours = _ucHours;
                            ptr_rtc_setup->usYear = _usYear;
                            ptr_rtc_setup->ulLocalUTC = ulOriginalSecondsValue;
                        }
                        else {
                            ucMonthOfYear = _ucMonthOfYear;              // set the calculated values for internal use
                            ucDayOfMonth = _ucDayOfMonth;
                            ucDayOfWeek = _ucDayOfWeek;
                            ucSeconds = _ucSeconds;
                            ucMinutes = _ucMinutes;
                            ucHours = _ucHours;
                            usYear = _usYear;
                        }
                        return;                                          // conversion completed
                    }
                }
            }
        }
    }
}

// Convert time and date to seconds since 1970
//
extern unsigned long fnConvertTimeSeconds(RTC_SETUP *ptr_rtc_setup, int iSetTime)
{
    unsigned long ulPassedSeconds = 0;
    unsigned long ulUnit = 1970;
    unsigned char ucNewWeekDay = 4;
    int iLeapYear;
    while (ulUnit < ptr_rtc_setup->usYear) {                             // for each passed year
        iLeapYear = LEAP_YEAR(ulUnit++);
        if (iLeapYear != 0) {
            ulPassedSeconds += LEAP_YEAR_SECONDS;
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 2);
        }
        else {
            ulPassedSeconds += COMMON_YEAR_SECONDS;
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
        }
    }
    iLeapYear = LEAP_YEAR(ulUnit);                                       // check whether present year is a leap year
    ulUnit = (ptr_rtc_setup->ucMonthOfYear - 1);                         // the full months passed in this year
    while (ulUnit != 0) {
        ulPassedSeconds += ((unsigned long)(monthDays[ulUnit - 1]) * (60 * 60 * 24)); // count seconds in full months passed
        ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, monthDays[ulUnit-- - 1]);
    }
    if ((iLeapYear != 0) && (ptr_rtc_setup->ucMonthOfYear > 2))  {       // leap year and data past February so count leap day
        ulPassedSeconds += (60 * 60 * 24);
        ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
    }
    ulUnit = (ptr_rtc_setup->ucDayOfMonth - 1);                          // passed days in month
    ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, (unsigned char)ulUnit);
    while (ulUnit--) {
        ulPassedSeconds += (60 * 60 * 24);                               // count passed days in present month
    }
    ulUnit = ptr_rtc_setup->ucHours;                                     // passed hours in day
    while (ulUnit--) {
        ulPassedSeconds += (60 * 60);                                    // count passed hours in present day
    }
    ulUnit = ptr_rtc_setup->ucMinutes;                                   // passed minutes in day
    while (ulUnit--) {
        ulPassedSeconds += (60);                                         // count passed minutes in present hour
    }
    ulPassedSeconds += ptr_rtc_setup->ucSeconds;                         // finally add the present seconds count
    if (iSetTime != 0) {
        usYear = ptr_rtc_setup->usYear;                                  // set the new time
        ucMonthOfYear = ptr_rtc_setup->ucMonthOfYear;
        ucDayOfMonth = ptr_rtc_setup->ucDayOfMonth;
        ucHours = ptr_rtc_setup->ucHours;
        ucMinutes = ptr_rtc_setup->ucMinutes;
        ucSeconds = ptr_rtc_setup->ucSeconds;
        ucDayOfWeek = ucNewWeekDay;
        ulLocalUTC = ulPassedSeconds;
    }
    return ulPassedSeconds;
}

// Convert time and date to days since 1970
//
extern unsigned short fnConvertTimeDays(RTC_SETUP *ptr_rtc_setup, int iSetTime)
{
    unsigned short ulPassedDays = 0;
    unsigned long ulUnit = 1970;
    unsigned char ucNewWeekDay = 4;
    int iLeapYear;
    while (ulUnit < ptr_rtc_setup->usYear) {                             // for each passed year
        iLeapYear = LEAP_YEAR(ulUnit++);
        if (iLeapYear != 0) {
            ulPassedDays += LEAP_YEAR_DAYS;
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 2);
        }
        else {
            ulPassedDays += COMMON_YEAR_DAYS;
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
        }
    }
    iLeapYear = LEAP_YEAR(ulUnit);                                       // check whether present year is a leap year
    ulUnit = (ptr_rtc_setup->ucMonthOfYear - 1);                         // the full months passed in this year
    while (ulUnit != 0) {
        ulPassedDays += (monthDays[ulUnit - 1]);                         // count days in full months passed
        ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, monthDays[ulUnit-- - 1]);
    }
    if ((iLeapYear != 0) && (ptr_rtc_setup->ucMonthOfYear > 2))  {       // leap year and date past February so count leap day
        ulPassedDays++;
        ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
    }
    ulUnit = (ptr_rtc_setup->ucDayOfMonth - 1);                          // passed days in month
    ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, (unsigned char)ulUnit);
    if (iSetTime != 0) {
        usYear = ptr_rtc_setup->usYear;                                  // set the new time
        ucMonthOfYear = ptr_rtc_setup->ucMonthOfYear;
        ucDayOfMonth = ptr_rtc_setup->ucDayOfMonth;
        ucHours = ptr_rtc_setup->ucHours;
        ucMinutes = ptr_rtc_setup->ucMinutes;
        ucSeconds = ptr_rtc_setup->ucSeconds;
        ucDayOfWeek = ucNewWeekDay;
    }
    ulPassedDays += (unsigned short)ulUnit;                              // count passed days in present month
    return ulPassedDays;
}


    #if (defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST) && defined SUPPORT_FILE_TIME_STAMP
// This routine supports date and time stamping of files and directories. If the information is not available it could return 1
// so that a fixed stamp is set. It is assumed that the RTC in the Kinetis will be used if the support is activated
//
extern int fnGetLocalFileTime(unsigned short *ptr_usCreationTime, unsigned short *ptr_usCreationDate)
{
    unsigned short usSec;
    unsigned short usMinutes;
    unsigned short usHours;
    unsigned short usDayofMonth;
    unsigned short usMonthofYear;
    unsigned short _usYear;

    do {
        usSec = ucSeconds;                                               // get a snap shot from RTC variables
        usMinutes = ucMinutes;
        usHours = ucHours;
        usDayofMonth = ucDayOfMonth;
        usMonthofYear = ucMonthOfYear;
        _usYear = usYear;
    } while (usSec != ucSeconds);                                        // if there was a second overflow while collecting the date repeat (saves disabling interrupts)
    if (usYear < 2015) {                                                 // if the year is invalid report that there is no valid time/date
        return 1;                                                        // utFAT will take fixed timestamp instead
    }
    *ptr_usCreationTime = (usSec | (usMinutes << 5) | (usHours << 11));
    *ptr_usCreationDate = (usDayofMonth | (usMonthofYear << 5) | ((_usYear - 1980) << 9));
    return 0;                                                            // successful
}
    #endif
#endif

#if defined USE_SNTP || defined USE_TIME_SERVER
// New NTP synchronisation time received
//
static void fnSynchroniseLocalTime(SNTP_TIME *NewTime, unsigned long ulSeconds)
{
    unsigned long ulUTC = (ulSeconds - NTP_TO_1970_TIME);                // convert to pure UTC seconds
    RTC_SETUP rtc_setup;
    rtc_setup.ulLocalUTC = (ulUTC + (cTIME_ZONE[temp_pars->temp_parameters.ucTimeZoneFlags & TIME_ZONE_MASK] * QUARTER_OF_HOURS_SECONDS)); // respect the time zone the local time has
    if (temp_pars->temp_parameters.ucTimeZoneFlags & DAYLIGHT_SAVING_SUMMER) { // compensate for daylight saving if active
        rtc_setup.ulLocalUTC += DAYLIGHT_SAVING_SECONDS;                 // adjust for daylight saving time
    }
    #if defined USE_SNTP
    if (NewTime != 0) {
        slFractionOffset = (NewTime->ulFraction/TIMER_PERIOD_FULL_SCALE);// the fraction of a second offset to second interrupt and counter value
        ulFractionResync = GET_TIMER_PRESENT_VALUE();
        if (ulFractionResync == 0) {                                     // avoid zero
            ulFractionResync = 1;
        }
    }
    #endif
    #if defined SUPPORT_RTC
    rtc_setup.command = (RTC_TIME_SETTING | RTC_SET_UTC);                // synchronise the RTC
    fnConfigureRTC(&rtc_setup);                                          // synchronise the RTC
    #else
    fnConvertSecondsTime(0, rtc_setup.ulLocalUTC);                       // convert to time for internal use
    #endif
}
#endif

#if defined USE_SNTP
// Get the present time stamp
//
static void fnGetPresentTime(SNTP_TIME *PresentTime)
{
    signed long slPresentFraction;
    uDisable_Interrupt();                                                // don't allow second interrupt to disturb, but don't stop timer either
    PresentTime->ulSeconds = ulLocalUTC;                                 // present local seconds value (with time zone and daylight saving adjustment)
    PresentTime->ulFraction = GET_TIMER_PRESENT_VALUE();                 // get fraction from hardware timer count value
    if (PresentTime->ulFraction < 100) {                                 // is it possible that the timer overflowed while reading present value?
        uEnable_Interrupt();                                             // if a second overflow occurred during the protected region it will be take place now
            PresentTime->ulSeconds = ulLocalUTC;                         // present seconds value, possibly after interrupt increment
        uDisable_Interrupt();                                            // block again
    }
    uEnable_Interrupt();                                                 // allow timer to interrupt again since the values have been captured and are synchronised
    PresentTime->ulSeconds += NTP_TO_1970_TIME;                          // convert to NTC seconds
    slPresentFraction = (PresentTime->ulFraction + slFractionOffset);    // respect offset to the local timer
    PresentTime->ulFraction += slFractionOffset;                     
    if (slPresentFraction >= TIMER_PERIOD_FULL_SCALE) {
        PresentTime->ulSeconds++;
        slPresentFraction -= TIMER_PERIOD_FULL_SCALE;
    }
    else if ((signed long)(PresentTime->ulFraction) < 0) {
        PresentTime->ulSeconds--;
        slPresentFraction += TIMER_PERIOD_FULL_SCALE;
    }
    PresentTime->ulFraction = (unsigned long)slPresentFraction;
    PresentTime->ulFraction *= (0xffffffff/TIMER_PERIOD_FULL_SCALE);     // convert to fraction of a second
}

static void fnSetTimeField(unsigned char *time_field, SNTP_TIME *sntp_time_stamp)
{
    *time_field++ = (unsigned char)(sntp_time_stamp->ulSeconds >> 24);
    *time_field++ = (unsigned char)(sntp_time_stamp->ulSeconds >> 16);
    *time_field++ = (unsigned char)(sntp_time_stamp->ulSeconds >> 8);
    *time_field++ = (unsigned char)(sntp_time_stamp->ulSeconds);
    *time_field++ = (unsigned char)(sntp_time_stamp->ulFraction >> 24);
    *time_field++ = (unsigned char)(sntp_time_stamp->ulFraction >> 16);
    *time_field++ = (unsigned char)(sntp_time_stamp->ulFraction >> 8);
    *time_field   = (unsigned char)(sntp_time_stamp->ulFraction);
}

#if defined SNTP_DISPLAY_TIME_STAMPS
static void fnDisplayUTC(SNTP_TIME *utc_time)
{
    unsigned long ulDays;
    unsigned long ulHours;
    unsigned long ulMinutes;
    unsigned long ulSeconds  = utc_time->ulSeconds;
    unsigned long ulFraction = utc_time->ulFraction;
    unsigned long ulFractionBackup;

    fnDebugHex(utc_time->ulSeconds, (WITH_LEADIN | sizeof(utc_time->ulSeconds)));
    fnDebugMsg(":");
    fnDebugHex(utc_time->ulFraction, (WITH_LEADIN | sizeof(utc_time->ulFraction)));

    ulSeconds -= REFERENCE_TIME;                                         // reference from 6th August 2009 at 0:0:0
    ulDays = (ulSeconds / (60 * 60 * 24));                               // days elapsed
    ulSeconds -= (ulDays * (60 * 60 * 24));
    ulHours = (ulSeconds / (60 * 60));                                   // additional hours elapsed
    ulSeconds -= (ulHours * (60 * 60));
    ulMinutes = (ulSeconds / 60);                                        // additional minutes elapsed
    ulSeconds -= (ulMinutes * 60);
    fnDebugDec(ulDays, (WITH_SPACE));                                    // days elapsed since reference data
    fnDebugMsg(":");
    fnDebugDec(ulHours, 0);                                              // hours elapsed since reference data
    fnDebugMsg(":");
    fnDebugDec(ulMinutes, LEADING_ZERO);                                 // minutes elapsed since reference data
    fnDebugMsg(":");
    fnDebugDec(ulSeconds, LEADING_ZERO);                                 // minutes elapsed since reference data
    fnDebugMsg(".");
    ulFraction /= (0xffffffff/1000000);                                  // convert to millionths of a second
    ulFractionBackup = ulFraction;
    if (ulFraction == 0) {                                               // handle special case when 0
        fnDebugMsg("000000\r\n");
    }
    else if (ulFraction >= 1000000) {
        fnDebugMsg("999999\r\n");
    }
    else {
        while (ulFraction < 100000) {
            fnDebugMsg("0");                                             // add leading zeros
            ulFraction = ((ulFraction * 8) + (ulFraction * 2));          // multiply by 10
        }
        fnDebugDec(ulFractionBackup, (WITH_CR_LF));
    }
}
#endif

static int fnRequestSNTP_time(int iTryNext)
{
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & (ACTIVE_SNTP | SNTP_BROADCAST)) == (ACTIVE_SNTP)) { // ignore if SNTP has been disabled
#if SNTP_SERVERS > 1
        int iJumpedServers = 0;
#endif
        SNTP_MESSAGE sntp_message;
        uMemset(&sntp_message, 0, sizeof(sntp_message));                 // zero message content

        if (iTryNext != 0) {                                             // repeated attempts
            if (++iSNTP_reps >= MAX_SNTP_SERVER_ATTEMPTS) {
                UTASKERMONOTIMER(OWN_TASK, SNTP_PAUSE_DELAY, E_FIRST_TIME_SYNC_SNTP); // try again after a longer delay in case the server is ignoring us at the moment
                return 0;
            }
        }
        else {
            iSNTP_reps = 0;                                              // reset the repetition counter
        }
#if SNTP_SERVERS > 1
        while ((iTryNext != 0) || (uMemcmp(temp_pars->temp_parameters.ucSNTP_server_ip[ucServerIndex], cucNullMACIP, IPV4_LENGTH) == 0) || (ucAvoidServer[ucServerIndex] != 0)) { // on repetition, empty server entry or a server on the avoid list
            if (++ucServerIndex >= SNTP_SERVERS) {                       // move to next server
                ucServerIndex = 0;
            }
            if (++iJumpedServers >= SNTP_SERVERS) {                      // if no available server has been found
                return -1;
            }
            iTryNext = 0;
        }
#else
        if (uMemcmp(temp_pars->temp_parameters.ucSNTP_server_ip[0], cucNullMACIP, IPV4_LENGTH) == 0) {
            return - 1;                                                  // if no server available      
        }
#endif
        sntp_message.sntp_data_content.ucFlags = (NTP_FLAG_LI_CLOCK_NOT_SYNCHRONISED | NTP_FLAG_VN_3 | NTP_FLAG_MODE_CLIENT);
        sntp_message.sntp_data_content.ucPeerPollingInterval = 0x0a;     // 1024 seconds
        sntp_message.sntp_data_content.ucPeerClockPrecision = 0xfa;
        sntp_message.sntp_data_content.ucRootDispersion[0] = 0x00;
        sntp_message.sntp_data_content.ucRootDispersion[1] = 0x01;
        sntp_message.sntp_data_content.ucRootDispersion[2] = 0x03;
        sntp_message.sntp_data_content.ucRootDispersion[3] = 0xfe;
        fnSetTimeField(sntp_message.sntp_data_content.ucReferenceClockUpdateTime, &sntp_update_time); // the last time that we requested
        fnGetPresentTime(&clientRequestTime);                            // the local time that we request at
        fnSetTimeField(sntp_message.sntp_data_content.ucTransmitTimeStamp, &clientRequestTime);
        if (fnSendUDP(SNTP_Socket, temp_pars->temp_parameters.ucSNTP_server_ip[ucServerIndex], SNTP_PORT, (unsigned char *)&sntp_message, sizeof(sntp_message.sntp_data_content), OWN_TASK) > 0) { // request time from SNTP server
            if (clientRequestTime.ulSeconds > sntp_update_time.ulSeconds) {
                uMemcpy(&sntp_update_time, &clientRequestTime, sizeof(sntp_update_time)); // save the time that we last requested at
            }
        }
        #if defined SNTP_DISPLAY_TIME_STAMPS
        fnDebugMsg("\r\nTime requested at ");
        fnDisplayUTC(&clientRequestTime);
        #endif
        UTASKERMONOTIMER(OWN_TASK, SNTP_RETRY_DELAY, E_NEXT_TIME_SYNC_SNTP);  // the next attempt will be made after this delay if there is no answer to the request
    }
    return 0;
}

// Subtract a more ancient time stamp from a more recent
// Note that negative results are not expected and are set to zero if they occur
//
static void fnSubtractTime(SNTP_TIME *result, SNTP_TIME *input, SNTP_TIME *minus)
{
    if (minus->ulSeconds <= input->ulSeconds) {                          // result will not be negative
        result->ulSeconds = (input->ulSeconds - minus->ulSeconds);       // subtract the full seconds
        if (minus->ulFraction <= input->ulFraction) {                    // if the fraction minus will not result in an underflow
            result->ulFraction = (input->ulFraction - minus->ulFraction);
            return;
        }
        if (result->ulSeconds != 0) {                                    // if the seconds underflow can be accepted
            result->ulSeconds--;
            return;
        }
    }
    result->ulFraction = result->ulSeconds = 0;                          // don't allow negative results (set to zero)
}

static void fnAdditionTime(SNTP_TIME *result, SNTP_TIME *input, SNTP_TIME *add)
{
    unsigned long ulFraction;
    result->ulSeconds = (input->ulSeconds + add->ulSeconds);
    ulFraction = (input->ulFraction + add->ulFraction);
    if (ulFraction < input->ulFraction) {
        result->ulSeconds++;
    }
    result->ulFraction = ulFraction;
}

static void fnDiv2Time(SNTP_TIME *result, SNTP_TIME *input)
{
    register unsigned long ulIntermediateSeconds;
    ulIntermediateSeconds = (input->ulSeconds/2);
    result->ulFraction = (input->ulFraction/2);
    if ((ulIntermediateSeconds * 2) != input->ulSeconds) {
        result->ulFraction += 0x80000000;                                // add half to the fraction result
        if (result->ulFraction < 0x80000000) {                           // check for overrun
            ulIntermediateSeconds++;
        }
    }
    result->ulSeconds = ulIntermediateSeconds;
}

// An answer has been received from a SNTP server
//
static int fnUpdateSNTP_time(NTP_FRAME *ntp_frame, unsigned short usLength)
{
    if (ntp_frame->ucPeerClockStratum == STRATUM_KISS_O_DEATH) {         // if the server sends us a kiss-o-death we may not use the server again
#if SNTP_SERVERS > 1
        ucAvoidServer[ucServerIndex] = 1;                                // do not use this server again (until after next reset)
#endif
        return USE_OTHER_SNTP_SERVER;                                    // the server is requesting that we no longer use it
    }
    if ((ntp_frame->ucFlags & NTP_FLAG_LI_MASK) == NTP_FLAG_LI_CLOCK_NOT_SYNCHRONISED) {
        return SNTP_SERVER_NOT_SYNCHRONISED;                             // server is not synchronised
    }
    if ((uMemcmp(ntp_frame->ucTransmitTimeStamp, &zero_time, sizeof(SNTP_TIME))) == 0) {
        return SNTP_SERVER_FORMAT_ERROR;                                 // reject invalid time stamp
    }
    if (((ntp_frame->ucFlags & NTP_FLAG_MODE_MASK) != NTP_FLAG_MODE_SERVER) && ((ntp_frame->ucFlags & NTP_FLAG_MODE_MASK) != NTP_FLAG_MODE_BROADCAST)) {
        return SNTP_SERVER_MODE_INVALID;                                 // reject any sources which are not servers or broadcast servers
    }
    else {
        SNTP_TIME Calculate;
        SNTP_TIME RoundTripTime;
        SNTP_TIME PresentTime;
        SNTP_TIME ServerTransmitTimeStamp;
        SNTP_TIME ServerReceiveTimeStamp;
        uMemset(&Calculate, 0, sizeof(Calculate));                       // zero the structs
        uMemset(&RoundTripTime, 0, sizeof(RoundTripTime));
        if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & SNTP_BROADCAST) != 0) { // if  operating with a broadcast server
            PresentTime.ulSeconds   = (ntp_frame->ucTransmitTimeStamp[0] << 24); // use the tramitted time directly
            PresentTime.ulSeconds  |= (ntp_frame->ucTransmitTimeStamp[1] << 16);
            PresentTime.ulSeconds  |= (ntp_frame->ucTransmitTimeStamp[2] << 8);
            PresentTime.ulSeconds  |= (ntp_frame->ucTransmitTimeStamp[3]);
            PresentTime.ulFraction  = (ntp_frame->ucTransmitTimeStamp[4] << 24);
            PresentTime.ulFraction |= (ntp_frame->ucTransmitTimeStamp[5] << 16);
            PresentTime.ulFraction |= (ntp_frame->ucTransmitTimeStamp[6] << 8);
            PresentTime.ulFraction |= (ntp_frame->ucTransmitTimeStamp[7]);
        }
        else {
            ServerTransmitTimeStamp.ulSeconds   = (ntp_frame->ucTransmitTimeStamp[0] << 24);
            ServerTransmitTimeStamp.ulSeconds  |= (ntp_frame->ucTransmitTimeStamp[1] << 16);
            ServerTransmitTimeStamp.ulSeconds  |= (ntp_frame->ucTransmitTimeStamp[2] << 8);
            ServerTransmitTimeStamp.ulSeconds  |= (ntp_frame->ucTransmitTimeStamp[3]);
            ServerTransmitTimeStamp.ulFraction  = (ntp_frame->ucTransmitTimeStamp[4] << 24);
            ServerTransmitTimeStamp.ulFraction |= (ntp_frame->ucTransmitTimeStamp[5] << 16);
            ServerTransmitTimeStamp.ulFraction |= (ntp_frame->ucTransmitTimeStamp[6] << 8);
            ServerTransmitTimeStamp.ulFraction |= (ntp_frame->ucTransmitTimeStamp[7]);
            ServerReceiveTimeStamp.ulSeconds    = (ntp_frame->ucReceiveTimeStamp[0] << 24);
            ServerReceiveTimeStamp.ulSeconds   |= (ntp_frame->ucReceiveTimeStamp[1] << 16);
            ServerReceiveTimeStamp.ulSeconds   |= (ntp_frame->ucReceiveTimeStamp[2] << 8);
            ServerReceiveTimeStamp.ulSeconds   |= (ntp_frame->ucReceiveTimeStamp[3]);
            ServerReceiveTimeStamp.ulFraction   = (ntp_frame->ucReceiveTimeStamp[4] << 24);
            ServerReceiveTimeStamp.ulFraction  |= (ntp_frame->ucReceiveTimeStamp[5] << 16);
            ServerReceiveTimeStamp.ulFraction  |= (ntp_frame->ucReceiveTimeStamp[6] << 8);
            ServerReceiveTimeStamp.ulFraction  |= (ntp_frame->ucReceiveTimeStamp[7]);

            fnGetPresentTime(&PresentTime);                              // the present local time stamp
        #if defined SNTP_DISPLAY_TIME_STAMPS
            fnDebugMsg("\r\nServer rx: ");
            fnDisplayUTC(&ServerReceiveTimeStamp);

            fnDebugMsg("Server tx: ");
            fnDisplayUTC(&ServerTransmitTimeStamp);

            fnDebugMsg("Received at: ");
            fnDisplayUTC(&PresentTime);
        #endif
            // The present time is calculated by (ServerTransmitTimeStamp + (((PresentTime - clientRequestTime) - (ServerTransmitTimeStamp - ServerReceiveTimeStamp))/2))
            // which can be explained by         (time at server when response sent + (((delay from sending request to receiving answer) - (server internal delay))/2))
            // which is the time that the server sent its time stamp plus the calculated delay for frame to reach us, assumiong rx and tx delays are equal
            //
            fnSubtractTime(&RoundTripTime, &PresentTime, &clientRequestTime);// (PresentTime - clientRequestTime) is the delay between sending the request and receiving the response
            fnSubtractTime(&Calculate, &ServerTransmitTimeStamp, &ServerReceiveTimeStamp); // (ServerTransmitTimeStamp - ServerReceiveTimeStamp) is the server's internal delay
            fnSubtractTime(&RoundTripTime, &RoundTripTime, &Calculate);  // round trip time (PresentTime - clientRequestTime) - (ServerTransmitTimeStamp - ServerReceiveTimeStamp) is the time that tx and rx frames were under way for
            fnDiv2Time(&RoundTripTime, &RoundTripTime);                  // divide by 2 to estimate the frame delay from the server to us
            fnAdditionTime(&PresentTime, &ServerTransmitTimeStamp, &RoundTripTime); // using the servers transmit time stamp as reference and adding the frame transmission delay gives the correct local time
            UTASKERMONOTIMER(OWN_TASK, SNTP_RESYNC_PERIOD, E_NEXT_TIME_SYNC_SNTP); // the next synchronisation takes place after this delay
        }
        fnSynchroniseLocalTime(&PresentTime, PresentTime.ulSeconds);     // synchronise local time
    #if defined SNTP_DISPLAY_TIME_STAMPS
        fnDebugMsg("\r\nNew time: ");
        fnDisplayUTC(&PresentTime);
    #endif
        return SNTP_SYNCHRONISED;
    }
}

// SNTP UDP client - reception call back function
//
extern int fnSNTP_client(USOCKET SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPortNr, unsigned char *data, unsigned short usLength)
{
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_SNTP) == 0) { // if the mode has been disabled ignore any receptions on the port
        return 0;
    }
    switch (ucEvent) {
    case UDP_EVENT_RXDATA:
        if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & SNTP_BROADCAST) == 0) { // if not accepting broadcasts
            if (uMemcmp(temp_pars->temp_parameters.ucSNTP_server_ip[ucServerIndex], ucIP, IPV4_LENGTH) != 0) {
                break;                                                   // ignore if not from expected IP address
            }
        }
        fnUpdateSNTP_time((NTP_FRAME *)data, usLength);                  // synchronise the local time
        break;

    case UDP_EVENT_PORT_UNREACHABLE:                                     // we have received information that this port is not available at the destination so quit
        break;
    }
    return 0;
}

    #if defined SUPPORT_RTC
// This is called when there is a change in the time zone or daylight saving setting
// - the local time is adjusted by the difference and the new setting stored
//
extern void fnAdjustLocalTime(unsigned char ucNewTimeZone, int iSNTP_active)
{
    if (iSNTP_active != 0) {                                             // only make immediate adjustments to local time when SNTP is being used
        RTC_SETUP rtc_setup;
        unsigned long ulAdjustmentTimezone = (cTIME_ZONE[ucNewTimeZone & TIME_ZONE_MASK] * QUARTER_OF_HOURS_SECONDS); // new adjustment
        ulAdjustmentTimezone -= (cTIME_ZONE[temp_pars->temp_parameters.ucTimeZoneFlags & TIME_ZONE_MASK] * QUARTER_OF_HOURS_SECONDS); // minus previous adjustment
        if (ucNewTimeZone & DAYLIGHT_SAVING_SUMMER) {                    // new daylight saving
            if ((temp_pars->temp_parameters.ucTimeZoneFlags & DAYLIGHT_SAVING_SUMMER) == 0) { // if being activated
                ulAdjustmentTimezone += DAYLIGHT_SAVING_SECONDS;
            }
        }
        else {
            if ((temp_pars->temp_parameters.ucTimeZoneFlags & DAYLIGHT_SAVING_SUMMER) != 0) { // if being disabled
                ulAdjustmentTimezone -= DAYLIGHT_SAVING_SECONDS;
            }
        }
        rtc_setup.command = RTC_GET_TIME;
        uDisable_Interrupt();                                            // ensure the interrupt cannot overflow before setting the seconds
            fnConfigureRTC(&rtc_setup);                                  // get local time
            rtc_setup.command = RTC_CONVERT_TO_UTC;
            fnConfigureRTC(&rtc_setup);                                  // convert to UTC local time
            if ((rtc_setup.ulLocalUTC + ulAdjustmentTimezone) < 0xffd00000) { // ignore if it would result in an underflow from 1970 to 2106
                rtc_setup.ulLocalUTC += ulAdjustmentTimezone;            // make adjustment due to change in setting
            }
            rtc_setup.command = (RTC_TIME_SETTING | RTC_SET_UTC);
            fnConfigureRTC(&rtc_setup);                                  // get local time as UTC
        uEnable_Interrupt();
    }
    temp_pars->temp_parameters.ucTimeZoneFlags = ucNewTimeZone;          // commit new daylight saving and time zone settings
}
    #endif


extern void fnStartSNTP(DELAY_LIMIT syncDelay)
{
    if (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_SNTP) {
        if (SNTP_Socket < 0) {
            if ((SNTP_Socket = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnSNTP_client, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS))) >= 0) {
                fnBindSocket(SNTP_Socket, SNTP_PORT);                    // bind socket
            }
        }
        UTASKERMONOTIMER(OWN_TASK, syncDelay, E_FIRST_TIME_SYNC_SNTP);   // request first time synchronisation after a delay to allow the network to become available
    }
}
#endif


#if defined USE_TIME_SERVER
    #if defined SUPPORT_LCD
static void fnDisplayTime(void)
{
    CHAR cNewTimeLCD[17];
    cNewTimeLCD[0] = (signed char)0x80;                                  // position to start of first line
    uMemset(&cNewTimeLCD[1], ' ', 16);
    fnSetShowTime(DISPLAY_RTC_TIME, &cNewTimeLCD[5]);
    cNewTimeLCD[13] = ' ';
    fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cNewTimeLCD, 17);
}
    #endif

static int fnTimeListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    #if defined SUPPORT_LCD
    static const CHAR cErrorLCDGW[] = "\x01  No gateway?   ";            // TCP socket error message
    static const CHAR cNextServerLCD[] = "\x01 Try next server?";        // next server try text
    static const CHAR cNoTimeLCD[] = "\x01 Sorry no time  ";             // failed text
    #endif

    switch (ucEvent) {
    case TCP_EVENT_ARP_RESOLUTION_FAILED:
    #if defined SUPPORT_LCD
        fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cErrorLCDGW, (sizeof(cErrorLCDGW) - 1));
    #endif
        break;

    case TCP_EVENT_DATA:                                                 // a time server sends the time in seconds from 0:0:0 1900 and terminates
        {
            ulTimeServerTime = *ucIp_Data++;
            ulTimeServerTime <<= 8;
            ulTimeServerTime |= *ucIp_Data++;
            ulTimeServerTime <<= 8;
            ulTimeServerTime |= *ucIp_Data++;
            ulTimeServerTime <<= 8;
            ulTimeServerTime |= (*ucIp_Data);
            fnSynchroniseLocalTime(0, ulTimeServerTime);                 // synchronise local time
        #if defined SUPPORT_LCD
            fnDisplayTime();                                             // display the time in the LCD
        #endif
        #if !defined SUPPORT_RTC
            UTASKERMONOTIMER(OWN_TASK, (DELAY_LIMIT)(1 * SEC), E_SECOND_TICK);
        #endif
        }
        break;

    case TCP_EVENT_CLOSE:
    case TCP_EVENT_CLOSED:
        if (ulTimeServerTime != 0) {                                     // if the connection is closed without receiving the time we try the next server - if the time was received the work is done
            break;
        }
                                                                         // if remote server closed before we received the time, try next
    case TCP_EVENT_ABORT:                                                // no connection was established
        if (ucTimeServerTry < NUMBER_OF_TIME_SERVERS) {
            fnTCP_Connect(TIME_TCP_socket, temp_pars->temp_parameters.ucTime_server_ip[ucTimeServerTry++], TIME_PORT, 0, 0); // ucTimeServerTry incremented after use and not before
    #if defined SUPPORT_LCD
            fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cNextServerLCD, (sizeof(cNextServerLCD) - 1));
    #endif
        }
    #if defined SUPPORT_LCD
        else {
            fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cNoTimeLCD, (sizeof(cNoTimeLCD) - 1));
        }
    #endif
        break;
    }
    return APP_ACCEPT;
}


static void fnRequestTimeServer(void)
{
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_TIME_SERVER) == 0) {
        return;
    }
    ulTimeServerTime = 0;
    fnTCP_Connect(TIME_TCP_socket, temp_pars->temp_parameters.ucTime_server_ip[ucTimeServerTry++], TIME_PORT, 0, 0);
}

extern void fnStartTimeServer(DELAY_LIMIT syncDelay)
{
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_TIME_SERVER) == 0) {
        return;
    }
    if (TIME_TCP_socket < 0) {                                           // bind socket on first call
        TIME_TCP_socket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, TCP_DEFAULT_TIMEOUT, fnTimeListener);
    }
    UTASKERMONOTIMER(OWN_TASK, syncDelay, E_FIRST_TIME_SERVER);          // request first time synchronisation after a delay to allow the network to become available
}
#endif

#if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
    #if defined USE_SNTP && defined _M5223X
// This interrupt will normally never fire since the RTC always resynchronised the timer before its times out
//
static void DMA_timer_int(void)
{
}
    #endif

extern void fnStartRTC(void (*seconds_callback)(void))
{
    RTC_SETUP rtc_setup;
    rtc_setup.command = (RTC_TICK_SEC | RTC_INITIALISATION);             // start the RTC (if not already operating) and configure 1s interrupt rate
    rtc_setup.int_handler = seconds_callback;                            // interrupt handler
    if (fnConfigureRTC(&rtc_setup) == WAIT_STABILISING_DELAY) {
    #if defined SUPPORT_RTC
        #if !defined RTC_OSCILLATOR_STABILISATION_DELAY
            #define RTC_OSCILLATOR_STABILISATION_DELAY    (1 * SEC)      // when nothing else defined, default to 1 second
        #endif
        _seconds_callback = seconds_callback;                            // remember the callback required by the application so that it can be set after the stabilisation delay
        UTASKERMONOTIMER(OWN_TASK, (DELAY_LIMIT)(RTC_OSCILLATOR_STABILISATION_DELAY), E_RTC_OSC_STAB_DELAY); // if the RTC oscillator had to be started we wait for it to stabilise before continuing
    #endif
    }
    #if defined USE_SNTP
    else {
        #if defined _M5223X                                              // user DMA timer in M522xx for SNMP fraction timer
        DMA_TIMER_SETUP dma_timer_setup;                                 // interrupt configuration parameters
        dma_timer_setup.int_type = DMA_TIMER_INTERRUPT;
        dma_timer_setup.int_handler = DMA_timer_int;
        dma_timer_setup.channel = 1;                                     // DMA timer channel 1
        dma_timer_setup.int_priority = DMA_TIMER1_INTERRUPT_PRIORITY;    // define interrupt priority
        dma_timer_setup.mode = (DMA_TIMER_INTERNAL_CLOCK_DIV_16 | DMA_TIMER_ENABLE_CLOCK_DIVIDER | DMA_TIMER_PERIODIC_INTERRUPT | DMA_TIMER_RESTART_ON_MATCH);
        dma_timer_setup.clock_divider = 5;
        dma_timer_setup.count_delay = DMA_TIMER_S_DELAY(16,5,4);         // 4s delay using 80MHz/16/5 -> 1MHz clock (this never actually fires since the RTC will synchronise it)
        fnConfigureInterrupt((void *)&dma_timer_setup);                  // enter interrupt for DMA timer test
        #endif
    }
    #endif
    #if defined SUPPORT_SW_RTC
    UTASKERMONOTIMER(OWN_TASK, (DELAY_LIMIT)(1 * SEC), E_SECOND_TICK);   // perform software seconds tick
    #endif
}
#endif

extern CHAR *fnUpTime(CHAR *cValue)
{
    unsigned long ulUpTimeSecs = (uTaskerSystemTick / SEC);
    unsigned long ulUpTime     = ulUpTimeSecs/60/60/24;                  // the up time in days
    CHAR *cPtr = fnBufferDec(ulUpTime, 0, cValue);
    *cPtr++ = ' ';
    *cPtr++ = 'D';
    *cPtr++ = 'a';
    *cPtr++ = 'y';
    *cPtr++ = 's';
    *cPtr++ = ' ';
    ulUpTimeSecs -= (ulUpTime * 60 * 60 * 24);                           // minus the days
    ulUpTime = (ulUpTimeSecs / (60 * 60));                               // the UP time in hours
    cPtr = fnBufferDec(ulUpTime, 0, cPtr);
    *cPtr++ = ':';
    ulUpTimeSecs -= (ulUpTime * 60 * 60);                                // minus the hours
    ulUpTime = (ulUpTimeSecs / 60);                                      // the UP time in minutes
    cPtr = fnBufferDec(ulUpTime, LEADING_ZERO, cPtr);
    *cPtr++ = ':';
    ulUpTimeSecs -= (ulUpTime * 60);                                     // minus the minutes
    cPtr = fnBufferDec(ulUpTimeSecs, LEADING_ZERO, cPtr);
    return cPtr;
}

#if defined DUSK_AND_DAWN                                                // {2}

static int isSummerTime(unsigned long timeStamp);

// Globaler Struct für Sonnenaufgang/Untergangszeiten
struct sonnenstand
{
    unsigned short SAG_Std,        // Sonnenaufgang h
            SAG_Min,        // Sonnenaufgang min
            SUG_Std,        // Sonnenuntergang h
            SUG_Min,        // Sonnenuntergang min
            TagesStunden;
    float   f_SAG,          // Sonnenaufgang float Wert zB 7,32 Uhr
            f_SUG;
};
// Geo Koordinaten des Standortes

#define O_LAENGE    6.1
#define N_BREITE    50.7667
// timeStamp: current Time, sonnenstand: struct to hold the values, T: day of year
short fnCalculateDawnDusk(unsigned long timeStamp, struct sonnenstand *sonne,unsigned short T)
{
    #define hoehe   -0.0145     // Sonnenhöhe in RAD
    #define PI        3.141592654

    float   B,
            Deklination,        // Deklination der Sonne
            Zeitdiff,           // Zeitdifferenz
            DIFF_WOZ_MOZ,
            Aufgang_MOZ,        // Aufgang mittlere Ortzeit
            Untergang_MOZ,      // Aufgang mittlere Ortszeit
            Aufgang_WOZ,        // Aufgang wahre Ortszeit
            Untergang_WOZ;      // Untergang wahre Ortszeit


    // Calculations
    B = PI *N_BREITE / 180;
    Deklination = 0.4095*sin(0.016906*(T-80.086));
    Zeitdiff = 12*acos((sin(hoehe) - sin(B)*sin(Deklination)) / (cos(B)*cos(Deklination)))/PI;
    DIFF_WOZ_MOZ = -0.171*sin(0.0337*T+0.465) - 0.1299*sin(0.01787*T-0.168);    // WOZ-MOZ

    Aufgang_MOZ   = 12 - Zeitdiff - DIFF_WOZ_MOZ;
    Untergang_MOZ = 12 + Zeitdiff - DIFF_WOZ_MOZ;

    Aufgang_WOZ   = Aufgang_MOZ + (15 - O_LAENGE)*(float)4/60;
    Untergang_WOZ = Untergang_MOZ + (15 - O_LAENGE)*(float)4/60;


    // Check for Summertime
    if (isSummerTime(timeStamp)) {
        Aufgang_WOZ = Aufgang_WOZ + 1;
        Untergang_WOZ = Untergang_WOZ + 1;
    }

    // set global float values
    sonne->f_SAG = Aufgang_WOZ;
    sonne->f_SUG = Untergang_WOZ;
    sonne->TagesStunden = sonne->f_SUG - sonne->f_SAG;

    // Calc hours minutes

    sonne->SAG_Std = floor(Aufgang_WOZ);                    // Dawn
    sonne->SAG_Min = (Aufgang_WOZ - (float)sonne->SAG_Std) * 60;

    sonne->SUG_Std = ceil(Untergang_WOZ)-1; // Dusk
    sonne->SUG_Min = (Untergang_WOZ - (float)sonne->SUG_Std) * 60;

    return 1;
}

static unsigned short fnGetEventTime(unsigned long timeStamp, int i)
{
    return 0;
}

unsigned short CalcDayOfYear(unsigned long timeStamp)
{
    unsigned short days,month,year;
    short  i;

    days = fnGetEventTime(timeStamp,1);    // Day of month
    month = fnGetEventTime(timeStamp,2);    // Month of year
    year = fnGetEventTime(timeStamp,6);        //year
    // Tage der Monate addieren
    for (i = 1;i < month;i++){
        if(i == 2 && LEAP_YEAR(year))
            days++;
        days = days + monthDays[i-1];
    }

    return days;
}
static int isSummerTime(unsigned long timeStamp){
    short day = fnGetEventTime(timeStamp,1);
    short month = fnGetEventTime(timeStamp,2);
    short wday = fnGetEventTime(timeStamp,0);
    short hour = fnGetEventTime(timeStamp,3);

    if( month < 3 || month > 10 )            // month 1, 2, 11, 12
        return 0;                    // -> Winter

    if( day - wday >= 25 && (wday || hour >= 2) ){ // after last Sunday 2:00
        if( month == 10 )                // October -> Winter
          return 0;
      }else{                    // before last Sunday 2:00
        if( month == 3 )                // March -> Winter
          return 0;
      }

    return 1;
} 




#if defined USE_TWILIGHT
    #include <math.h>
#endif


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define DAWN	1
#define DUSK	0

#define DegtoRad (2.0 * pi/360.0)

// Aachen
// Latitude: 50°46'N (+50.77)
// Longitude: 6°05'E (+6.09)
// LOCATION loc = {{50,46,30,NORTH},{6,5,3,EAST}};
// TIMEZONE tz = {{'C','E','T'}, 3600, 3600};

/* =================================================================== */
/*                      local struct definitions                       */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */
    static double twilight_time(RTC_SETUP *t, LOCATION *loc, enum twilight tw, unsigned char dawn);
    static double calc_twilight_time(int day, int month, int year, int dawn, enum twilight tw, double longitude, double latitude);

    // convert latitude, longitude to a double value
    static double coord2double(COORDINATE *c);
  //static double latitude(COORDINATE *c);
  //static double longitude(COORDINATE *c);

    static int day_of_year(int day, int month, int year);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */





double dark(enum twilight tw)
{
	RTC_SETUP t;
	double dawn;
	double dusk;
	double now;
	double time_next;
	LOCATION *loc = &temp_pars->temp_parameters.ourLocation;

	t.command = RTC_GET_TIME;
	fnConfigureRTC(&t);                                 // get the present UTC time and date
	
	now = (double)t.ucHours + ((double)t.ucMinutes / 60.0) + ((double)t.ucSeconds / 3600);
	
	dusk = twilight_time(&t, loc, tw, DUSK);
	if (now >= dusk)
	{
		// it is already dark
		t.ucDayOfMonth += 1;
		if (t.ucDayOfMonth > days_in_month(t.ucMonthOfYear, t.usYear)) 
		{
			t.ucDayOfMonth = 1;
			t.ucMonthOfYear += 1;
			if (t.ucMonthOfYear > 12)
			{
				t.ucMonthOfYear = 1;
				t.usYear += 1;
			}
		}
		dawn = twilight_time(&t, loc, tw, DAWN);
		time_next = 24.00 - now + dawn;
	}
	else 
	{
		dawn = twilight_time(&t, loc, tw, DAWN);
		if (dawn <= now)
		{
			// it is day
			// return negative value
			time_next = (-1.0)*(dusk - now);
		}
		else
		{
			// it is still dark
			time_next = dawn - now;
		}
	}

	return time_next;
}

double dusk(enum twilight tw)
{
	RTC_SETUP t;
	LOCATION *loc = &temp_pars->temp_parameters.ourLocation;

	t.command = RTC_GET_TIME;
	fnConfigureRTC(&t);                                 // get the present UTC time and date

	return twilight_time(&t, loc, tw, DUSK);
}

double dawn(enum twilight tw)
{
	RTC_SETUP t;
	LOCATION *loc = &temp_pars->temp_parameters.ourLocation;

	t.command = RTC_GET_TIME;
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
	fnConfigureRTC(&t);                                 // get the present UTC time and date
    #else
    _EXCEPTION("To do  what happens when there is no RTC???");
    #endif

	return twilight_time(&t, loc, tw, DAWN);
}

static double twilight_time(RTC_SETUP *t, LOCATION *loc, enum twilight tw, unsigned char dawn)
{
	double la, lo;
	double t_tw = 0.0;

	la = coord2double(&loc->latitude); //latitude(&loc->latitude);
	if (la > 90)
	{
		// error
	}
	lo = coord2double(&loc->longitude);//longitude(&loc->longitude);
	if (lo > 180)
	{
		// error
	}
	t_tw = calc_twilight_time(t->ucDayOfMonth, t->ucMonthOfYear, t->usYear, dawn, tw, lo, la);

	return t_tw;
}

/*
	zenith:        Sun's zenith for sunrise/sunset
	  offical      = 90 degrees 50'
	  civil        = 96 degrees
	  nautical     = 102 degrees
	  astronomical = 108 degrees
*/
static const double zenith[] = {90.83, 96.0, 102.0, 108.0};

static const double pi = 3.14159265358979323846;

static double calc_twilight_time(int day, int month, int year, int dawn, enum twilight tw, double longitude, double latitude)
{
	double time;
	double sl;
	double ra;
	double HA;
	double MT;
	double UTC;

	double lngHour;
	double M;
	double M_rad;
	double tan_sl;
	double latitude_rad;
	double sinDec;
	double cosDec;
	double cosH;

	lngHour = longitude / 15.0;
	if (dawn)
	{
	  time = (double) day_of_year(day, month, year) + ((6.0 - lngHour) / 24.0);
	}
	else
	{
	  time = (double) day_of_year(day, month, year) + ((18.0 - lngHour) / 24.0);
	}
	M = (0.9856 * time) - 3.289;
	M_rad = DegtoRad * M;
	sl = M + (1.916 * sin(M_rad)) + (0.020 * sin(2.0 * M_rad)) + 282.634;
	if (sl > 360.0)
		sl-= 360.0;
	else if (sl < 0.0)
		sl += 360.0;

	tan_sl = tan(DegtoRad * sl);
	ra = (180.0 / pi) * atan(0.91764 * tan_sl);
	if (ra >= 360.0)
		ra-= 360.0;
	else if (ra < 0.0)
		ra += 360.0;

	ra = (ra + (((floor( sl/90)) - (floor(ra/90)))*90)) / 15.0;

	latitude_rad = DegtoRad * latitude;

	sinDec = 0.39782 * sin(DegtoRad * sl);
	cosDec = cos(asin(sinDec));

	cosH = ((cos(DegtoRad * zenith[tw-1])) - (sinDec * sin(latitude_rad))) / (cosDec * cos(latitude_rad));
	
	if (cosH >  1) 
	{
		//   the sun never rises on this location (on the specified date)
		HA = -1.00;
	}
	if (cosH < -1) 
	{
		//   the sun never sets on this location (on the specified date)
		HA = -2.00;
	}
	else
	{
		HA = (180.0 / pi) * acos(cosH);
		if (dawn)
			HA = 360.0 - HA;
	
		HA = HA / 15.0;
	}
	if (HA >= 0.0)
	{
		MT = HA + ra - (0.06571 * time) - 6.622;

		UTC = MT - lngHour;
		if (UTC >= 24.0)
			UTC -=24.0;
		else if (UTC < 0.0)
			UTC += 24.0;
	}
	else if (!dawn)
	{
		UTC = 23.999;
	}
	else
	{
		UTC = 0.0;
	}

	return UTC;
}

static double coord2double(COORDINATE *c)
{
	return (c->scDegree + (c->ucMinute / 60.0) + (c->ucSecond / 3600.0));
}


static const int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define __isleap(year)	\
    ((((year) % 4) == 0) && ((((year) % 100) != 0) || (((year) % 400) == 0)))


static int day_of_year(int day, int month, int year)
{
	int i;
	int yday = 0;

	for (i = 0; i < (month-1); i++)
	{
		yday += month_days[i];
	}
	if ((month > 2)&&(__isleap(year)))
		yday += 1;
	yday += day; 

	return yday;
}

enum twilight c2tw(char c)
{
	switch (c) {
	case 'O':
	case 'o':
		return OFFICIAL;
	case 'C':
	case 'c':
		return CIVIL;
	case 'N':
	case 'n':
		return NAUTICAL;
	case 'A':
	case 'a':
		return ASTRONOMICAL;
	default:
		return UNKNOWN;
	}
}

int days_in_month(int month, int year)
{
	if (month == 2)
	{
		return month_days[month - 1] + __isleap(year) ? 1 : 0;
	}
	return month_days[month - 1];
}

#endif
