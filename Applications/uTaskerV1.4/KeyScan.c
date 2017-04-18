/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      KeyScan.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    13.08.2009 Adapt to support non-muliplexed keys with up to 32 keys

    This task supports a matrix key pad with up to 8 columns and 8 rows [KEY_COLUMNS, KEY_ROWS]
    The hardware controls are defined in the project hardware file so that it can easily support various wiring
    on various processors. The ports used can be defined to be any bit on any port.

    06.11.2015 Touch sensor input read moved from Application.c to here  {1}

*/

#include "config.h"


#if defined SUPPORT_KEY_SCAN                                             // activate this define to support Key Scan

#if defined _WINDOWS
  #define SIM_MATRIX_KB()  fnSimMatrixKB()                               // interraction with the simulator
#else
  #define SIM_MATRIX_KB()                                                // no function on target
#endif

#define OWN_TASK                        TASK_KEY                         // task reference name

#if KEY_COLUMNS > 0
    unsigned char ucCols[KEY_COLUMNS] = {0};                             // present key state
    unsigned char ucColsLast[KEY_COLUMNS] = {0};                         // previous key state
#else
    unsigned long ulCols = INIT_KEY_STATE;                               // present key state
    unsigned long ulColsLast = INIT_KEY_STATE;                           // previous key state
#endif
#if KEY_COLUMNS > 0
    static void fnUpdateInputs(void);
    static void fnSendEvent(unsigned char ucChangedBits, unsigned char ucNewColumn, int iColumn);
#endif
static void fnCheckKeyChanges(void);
#if defined _KINETIS && defined TOUCH_SENSOR_INPUTS                      // {1}
    static unsigned long fnReadTouchSensorInputs(void);
#endif



// This task is assumed to be polling at an adequate rate (or called directly from a timer interrupt)
//
extern void fnKey(TTASKTABLE *ptrTaskTable)                              // key scan task
{
#if KEY_COLUMNS > 0                                                      // matrix keypad
    fnUpdateInputs();                                                    // update the present input states
#else
    ulCols = READ_KEY_INPUTS();                                          // read all key inputs
#endif
    fnCheckKeyChanges();                                                 // generate events on changes
}


#if KEY_COLUMNS > 0
// A column is being driven, collect the corresponding row inputs
//
static void fnGetCol(int iRow)
{
    unsigned char ucBits = 0x01;
    ucCols[iRow]    = 0x00;

    #if defined KEY_ROWS
    if ((KEY_ROW_IN_PORT_1 & KEY_ROW_IN_1) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
    #if KEY_ROWS > 1
    ucBits <<= 1;
    if ((KEY_ROW_IN_PORT_2 & KEY_ROW_IN_2) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
    #if KEY_ROWS > 2
    ucBits <<= 1;
    if ((KEY_ROW_IN_PORT_3 & KEY_ROW_IN_3) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
    #if KEY_ROWS > 3
    ucBits <<= 1;
    if ((KEY_ROW_IN_PORT_4 & KEY_ROW_IN_4) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
    #if KEY_ROWS > 4
    ucBits <<= 1;
    if ((KEY_ROW_IN_PORT_5 & KEY_ROW_IN_5) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
    #if KEY_ROWS > 5
    ucBits <<= 1;
    if ((KEY_ROW_IN_PORT_6 & KEY_ROW_IN_6) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
    #if KEY_ROWS > 6
    ucBits <<= 1;
    if ((KEY_ROW_IN_PORT_7 & KEY_ROW_IN_7) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
    #if KEY_ROWS > 7
    ucBits <<= 1;
    if ((KEY_ROW_IN_PORT_8 & KEY_ROW_IN_8) == 0) {
        ucCols[iRow] |= ucBits;
    }
    #endif
}

// Scan all columns - called periodically
//
static void fnUpdateInputs(void)
{
    #if defined KEY_COLUMNS
    DRIVE_COLUMN_1();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(0);
    RELEASE_COLUMN_1();                                                  // set column to high impedance (with active drive high)
    #endif
    #if KEY_COLUMNS > 1
    DRIVE_COLUMN_2();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(1);
    RELEASE_COLUMN_2();                                                  // set column to high impedance (with active drive high)
    #endif
    #if KEY_COLUMNS > 2
    DRIVE_COLUMN_3();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(2);
    RELEASE_COLUMN_3();                                                  // set column to high impedance (with active drive high)
    #endif
    #if KEY_COLUMNS > 3
    DRIVE_COLUMN_4();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(3);
    RELEASE_COLUMN_4();                                                  // set column to high impedance (with active drive high)
    #endif
    #if KEY_COLUMNS > 4
    DRIVE_COLUMN_5();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(4);
    RELEASE_COLUMN_5();                                                  // set column to high impedance (with active drive high)
    #endif
    #if KEY_COLUMNS > 5
    DRIVE_COLUMN_6();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(5);
    RELEASE_COLUMN_6();                                                  // set column to high impedance (with active drive high)
    #endif
    #if KEY_COLUMNS > 6
    DRIVE_COLUMN_7();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(6);
    RELEASE_COLUMN_7();                                                  // set column to high impedance (with active drive high)
    #endif
    #if KEY_COLUMNS > 7
    DRIVE_COLUMN_8();                               SIM_MATRIX_KB();     // set as output to drive column low
    fnGetCol(7);
    RELEASE_COLUMN_8();                                                  // set column to high impedance (with active drive high)
    #endif
    RESET_SCAN();                                                        // reset any changes ready for next scan sequence
}
#endif


// Check for key press changes. If changes are detected, generate an event for each change and send it to the controlling task
//
static void fnCheckKeyChanges(void)
{
#if KEY_COLUMNS > 0
    int i;
    for (i = 0; i < KEY_COLUMNS; i++) {
        if (ucCols[i] != ucColsLast[i]) {                                // has a change in key pad state been detected?
            fnSendEvent((unsigned char)(ucColsLast[i] ^ ucCols[i]), ucCols[i], i);
            ucColsLast[i] = ucCols[i];                                   // update the backup
        }
    }
#else
    unsigned long ulInput = 0x00000001;
    unsigned long ulChanges = (ulColsLast ^ ulCols);
    unsigned char ucEvent = KEY_EVENT_COL_1_ROW_1_PRESSED;               // event on first input pressed
    while (ulChanges != 0) {
        if (ulChanges & ulInput) {
            ulChanges &= ~ulInput;
            if (ulCols & ulInput) {
                fnInterruptMessage(KEYPAD_PARTNER_TASK, ucEvent);        // send as pressed interrupt event
            }
            else {
                fnInterruptMessage(KEYPAD_PARTNER_TASK, (unsigned char)(ucEvent + 1)); // send as released interrupt event
            }
        }
        ucEvent += 2;
        ulInput <<= 1;
    }
    ulColsLast = ulCols;
#endif
}

#if KEY_COLUMNS > 0
// This is called when one or more key press/releases have been detected in a column.
// It generates one event per change and send it as interrupt event to the application task
//
static void fnSendEvent(unsigned char ucChangedBits, unsigned char ucNewColumn, int iColumn)
{
    unsigned char ucBit = 0x01;
    unsigned char ucEvent = KEY_EVENT_COL_1_ROW_1_PRESSED + (iColumn * 2 * KEY_ROWS);

    while (ucChangedBits != 0) {
        if ((ucChangedBits & ucBit) != 0) {
            if ((ucChangedBits & ucNewColumn) != 0) {
                fnInterruptMessage(KEYPAD_PARTNER_TASK, ucEvent);        // send as pressed interrupt event
            }
            else {
                fnInterruptMessage(KEYPAD_PARTNER_TASK, (unsigned char)(ucEvent + 1)); // send as released interrupt event
            }
            ucChangedBits &= ~ucBit;
        }
        ucBit <<= 1;
        ucEvent += 2;
    }
}
#endif


#if defined _KINETIS && defined TOUCH_SENSOR_INPUTS                      // {1}
static unsigned long fnReadTouchSensorInputs(void)
{
    #if defined FRDM_KL26Z || defined FRDM_KL25Z || defined rcARM_KL26
        #define CALIBRATE_CYCLES   (8 * KEY_ROWS)                        // first 8 scan cycles used for calibration (assumed not pressed)
    #else
        #define CALIBRATE_CYCLES   8                                     // first 8 samples used for calibration (assumed not pressed)
    #endif
    static int iCalibrate = 0;
    static unsigned long ulCalibrationValue[VIRTUAL_KEY_ROWS] = {0};

    unsigned long ulCols = 0;
    #if defined TWR_K20D50M || defined TWR_K20D72M
    unsigned long ulKey1 = TSI0_CNTR1;                                   // read the touch sensor inputs to see which keys are pressed
    unsigned long ulKey2 = TSI0_CNTR7;
    ulKey1 &= 0x0000ffff;
    ulKey2 &= 0x0000ffff;
    if (iCalibrate < CALIBRATE_CYCLES) {
        ulCalibrationValue[0] += ulKey1;
        ulCalibrationValue[1] += ulKey2;
        if (++iCalibrate == CALIBRATE_CYCLES) {                          // has calibration period expired?
            ulCalibrationValue[0] /= CALIBRATE_CYCLES;
            ulCalibrationValue[0] += 0x15;                               // raise reference over calibrated value
            TSI0_THRESHLD0 = (unsigned short)ulCalibrationValue[0];
            ulCalibrationValue[1] /= CALIBRATE_CYCLES;                   // average measured value
            ulCalibrationValue[1] += 0x15;
        #if KINETIS_MAX_SPEED == 100000000                               // some devices have only a single threshold register
            TSI0_THRESHLD6 = (unsigned short)ulCalibrationValue[1];      // enter threshold value
        #endif
        }
    }
    else {
        ulCols = 0;
        if (ulKey1 > (TSI0_THRESHLD0)) {                                 // if the value is higher than the threshold it is pressed
            ulCols |= 0x1;
        }
        #if KINETIS_MAX_SPEED == 100000000
        if (ulKey2 > (TSI0_THRESHLD6)) {                                 // if the value is higher than the threshold it is pressed
            ulCols |= 0x2;
        }
        #else
        if (ulKey2 > (ulCalibrationValue[1])) {                          // if the value is higher than the threshold it is pressed
            ulCols |= 0x2;
        }
        #endif
    }
    #else
        #if defined FRDM_KL26Z || defined FRDM_KL25Z || defined rcARM_KL26
    static unsigned long ulLastCols = 0;
    unsigned long ulKeys[KEY_ROWS];
    #if defined USE_SLIDER
    static signed long slLast[KEY_ROWS] = {0};
    static int iSliderDown = 0;
    signed long sSlider;
    #endif
    uMemset(ulKeys, 0, sizeof(ulKeys));
    switch (TSI0_DATA & TSI0_DATA_TSICH_15) {                            // previous input sampled
    case FIRST_TSI_INPUT:
        ulKeys[0] = (TSI0_DATA & TSI0_DATA_TSICNT);                      // key 1 measurement ready
        TSI0_GENCS = TSI0_GENCS;                                         // needed by KL25 but not KL26
        TSI0_DATA = (TSI0_DATA_SWTS | SECOND_TSI_INPUT);                 // start  measurement for following key
        ulCols = (ulLastCols & ~0x01);                                   // reset only key 1 state
    #if defined USE_SLIDER
        slLast[0] = (signed long)(ulKeys[0] - ulCalibrationValue[0]);
    #endif
        break;
    case SECOND_TSI_INPUT:
        ulKeys[1] = (TSI0_DATA & TSI0_DATA_TSICNT);                      // key 2 measurement ready
        TSI0_GENCS = TSI0_GENCS;                                         // needed by KL25 but not KL26
    #if VIRTUAL_KEY_ROWS > 2
        TSI0_DATA = (TSI0_DATA_SWTS | THIRD_TSI_INPUT);                  // start next measurement for following key
    #else
        TSI0_DATA = (TSI0_DATA_SWTS | FIRST_TSI_INPUT);                  // start next measurement for following key
    #endif
        ulCols = (ulLastCols & ~0x02);                                   // reset only key 2 state
    #if defined USE_SLIDER
        slLast[1] = (signed long)(ulKeys[1] - ulCalibrationValue[1]);
    #endif
        break;
    #if defined THIRD_TSI_INPUT
    case THIRD_TSI_INPUT:
        ulKeys[2] = (TSI0_DATA & TSI0_DATA_TSICNT);                      // key 3 measurement ready
        TSI0_GENCS = TSI0_GENCS;                                         // needed by KL25 but not KL26
        TSI0_DATA = (TSI0_DATA_SWTS | FIRST_TSI_INPUT);                  // start next measurement for following key
        ulCols = (ulLastCols & ~0x04);                                   // reset only key 3 state
        break;
    #endif
    }
        #else
    unsigned long ulKey1 = TSI0_CNTR5;                                   // read the touch sensor inputs to see which keys are pressed
    unsigned long ulKey2 = TSI0_CNTR9;
    unsigned long ulKey3 = TSI0_CNTR7;
    unsigned long ulKey4;
    ulKey1 >>= 16;
    ulKey4 = (ulKey2 >> 16);
    ulKey2 &= 0x0000ffff;
    ulKey3 >>= 16;
        #endif
    if (iCalibrate < CALIBRATE_CYCLES) {                                 // during calibration phase
        ulCalibrationValue[0] += ulKeys[0];                              // accumulate
        ulCalibrationValue[1] += ulKeys[1];
        #if VIRTUAL_KEY_ROWS > 2
        ulCalibrationValue[2] += ulKeys[2];
        #endif
        #if VIRTUAL_KEY_ROWS > 3
        ulCalibrationValue[3] += ulKeys[3];
        #endif
        if (++iCalibrate == CALIBRATE_CYCLES) {                          // has calibration period expired?
        #if defined FRDM_KL26Z || defined FRDM_KL25Z || defined rcARM_KL26
            ulCalibrationValue[0] /= (CALIBRATE_CYCLES/KEY_ROWS);
            ulCalibrationValue[0] += 0x15;                               // raise reference over calibrated value
            fnDebugMsg("Cal = ");
            fnDebugHex(ulCalibrationValue[0], WITH_LEADIN | sizeof(ulCalibrationValue[0]));
        #else
            ulCalibrationValue[0] /= CALIBRATE_CYCLES;                   // average measured value
            ulCalibrationValue[0] += 0x15;                               // raise reference over calibrated value
        #endif
        #if KINETIS_MAX_SPEED == 100000000
            TSI0_THRESHLD5 = (unsigned short)ulCalibrationValue[0];
        #endif
        #if defined FRDM_KL26Z || defined FRDM_KL25Z || defined rcARM_KL26
            ulCalibrationValue[1] /= (CALIBRATE_CYCLES/KEY_ROWS);
            ulCalibrationValue[1] += 0x15;                               // raise reference over calibrated value
            fnDebugMsg(", ");
            #if VIRTUAL_KEY_ROWS <= 2
            fnDebugHex(ulCalibrationValue[1], WITH_LEADIN | WITH_CR_LF | sizeof(ulCalibrationValue[1]));
            #else
            fnDebugHex(ulCalibrationValue[1], WITH_LEADIN | sizeof(ulCalibrationValue[1]));
            #endif
        #else
            ulCalibrationValue[1] /= CALIBRATE_CYCLES;                   // average measured value
            ulCalibrationValue[1] += 0x15;                               // raise reference over calibrated value
        #endif
        #if KINETIS_MAX_SPEED == 100000000
            TSI0_THRESHLD8 = (unsigned short)ulCalibrationValue[1];      // enter threshold value
        #endif
        #if VIRTUAL_KEY_ROWS > 2
            #if defined FRDM_KL26Z || defined FRDM_KL25Z || defined rcARM_KL26
            ulCalibrationValue[2] /= (CALIBRATE_CYCLES/KEY_ROWS);
            ulCalibrationValue[2] += 0x15;                               // raise reference over calibrated value
            fnDebugMsg(", ");
            fnDebugHex(ulCalibrationValue[2], WITH_LEADIN | WITH_CR_LF | sizeof(ulCalibrationValue[2]));
            #else
            ulCalibrationValue[2] /= CALIBRATE_CYCLES;                   // average measured value
            ulCalibrationValue[2] += 0x15;
            #endif
            #if KINETIS_MAX_SPEED == 100000000
            TSI0_THRESHLD7 = (unsigned short)ulCalibrationValue[2];      // enter threshold value
            #endif
        #endif
        #if VIRTUAL_KEY_ROWS > 3
            ulCalibrationValue[3] /= CALIBRATE_CYCLES;                   // average measured value
            ulCalibrationValue[3] += 0x15;
            #if KINETIS_MAX_SPEED == 100000000
            TSI0_THRESHLD9 = (unsigned short)ulCalibrationValue[3];      // enter threshold value
            #endif
        #endif
        }
    }
    else {                                                               // normal key scan operation
        ulCols = 0;
        #if KINETIS_MAX_SPEED == 100000000
        if (ulKey1 > (TSI0_THRESHLD5)) {                                 // if the value is higher than the threshold it is pressed
            ulCols |= 0x1;
        }
        if (ulKey2 > (TSI0_THRESHLD8)) {                                 // if the value is higher than the threshold it is pressed
            ulCols |= 0x2;
        }
        if (ulKey3 > (TSI0_THRESHLD7)) {                                 // if the value is higher than the threshold it is pressed
            ulCols |= 0x4;
        }
        if (ulKey4 > (TSI0_THRESHLD9)) {                                 // if the value is higher than the threshold it is pressed
            ulCols |= 0x8;
        }
        #elif defined USE_SLIDER
        if ((slLast[1] > 10) || (slLast[0] > 10)) {                      // if a touch is detected
            PWM_INTERRUPT_SETUP pwm_setup;
            pwm_setup.int_type = PWM_INTERRUPT;
            pwm_setup.pwm_mode = (PWM_SYS_CLK | PWM_PRESCALER_16);       // clock PWM timer from the system clock with /16 pre-scaler
            if (iSliderDown == 0) {
            #if defined FRDM_KL25Z
                _FLOAT_PORT(B, (BLINK_LED));
            #else
                _FLOAT_PORT(E, (BLINK_LED));
            #endif
            }
            iSliderDown = 20;
            sSlider = (slLast[1] - slLast[0]);
            sSlider += 400;
            sSlider /= 8;
            if (sSlider < 0) {
                sSlider = 0;
            }
            else if (sSlider > 100) {
                sSlider = 100;
            }
            #if defined FRDM_KL25Z
            pwm_setup.pwm_reference = (_TIMER_2 | 0);                    // timer module 2, channel 0 (red LED in RGB LED)
            #else
            pwm_setup.pwm_reference = (_TIMER_0 | 2);                    // timer module 0, channel 2 (red LED in RGB LED)
            #endif
            pwm_setup.pwm_frequency = PWM_TIMER_US_DELAY(TIMER_FREQUENCY_VALUE(1000), 16); // generate 1000Hz on PWM output
            pwm_setup.pwm_value   = _PWM_PERCENT((100 - (unsigned char)sSlider), pwm_setup.pwm_frequency);   // PWM (high/low)
            fnConfigureInterrupt((void *)&pwm_setup);                    // enter configuration for PWM test

            pwm_setup.pwm_value   = _PWM_PERCENT((unsigned char)sSlider, pwm_setup.pwm_frequency); // PWM (high/low)
            #if defined FRDM_KL25Z
                pwm_setup.pwm_reference = (_TIMER_0 | 1);                // timer module 0, channel 5 (blue LED in RGB LED)
            #else
                pwm_setup.pwm_reference = (_TIMER_0 | 5);                // timer module 0, channel 5 (blue LED in RGB LED)
            #endif
            fnConfigureInterrupt((void *)&pwm_setup);
            fnDebugMsg("Slider = ");
            fnDebugDec(sSlider, (DISPLAY_NEGATIVE | WITH_CR_LF));
        }
        else {
            if (iSliderDown != 0) {                                      // if a release has been detected
                iSliderDown--;
                if (iSliderDown == 0) {
            #if defined FRDM_KL25Z
                    _DRIVE_PORT_OUTPUT(B, (BLINK_LED));
                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, PORTB_BIT18, PORTB_BIT18, (PORT_SRE_SLOW | PORT_DSE_HIGH));
                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, PORTD_BIT1, PORTD_BIT1, (PORT_SRE_SLOW | PORT_DSE_HIGH));
            #else
                    _DRIVE_PORT_OUTPUT(E, (BLINK_LED));
                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, PORTE_BIT29, PORTE_BIT29, (PORT_SRE_SLOW | PORT_DSE_HIGH));
                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, PORTD_BIT5, PORTD_BIT5, (PORT_SRE_SLOW | PORT_DSE_HIGH));
            #endif
                }
            }
        }
        #else
        if (ulKeys[0] > (ulCalibrationValue[0])) {                       // if the present value is higher than the threshold it is pressed
            ulCols |= 0x1;
        }
        if (ulKeys[1] > (ulCalibrationValue[1])) {                       // if the present value is higher than the threshold it is pressed
            ulCols |= 0x2;
        }
            #if VIRTUAL_KEY_ROWS > 2
        if (ulKeys[2] > (ulCalibrationValue[2])) {                       // if the present value is higher than the threshold it is pressed
            ulCols |= 0x4;
        }
            #endif
            #if VIRTUAL_KEY_ROWS > 3
        if (ulKeys[3] > (ulCalibrationValue[3])) {                       // if the present value is higher than the threshold it is pressed
            ulCols |= 0x8;
        }
            #endif
            #if defined FRDM_KL26Z || defined FRDM_KL25Z || defined rcARM_KL26
            ulLastCols = ulCols;                                         // backup present logical state
            fnDebugMsg("Scan = ");
            fnDebugHex(ulKeys[0], WITH_LEADIN | sizeof(ulKeys[0]));
            fnDebugMsg(", ");
                #if VIRTUAL_KEY_ROWS > 2
            fnDebugHex(ulKeys[1], WITH_LEADIN | sizeof(ulKeys[1]));
            fnDebugMsg(", ");
            fnDebugHex(ulKeys[2], WITH_LEADIN | WITH_CR_LF | sizeof(ulKeys[2]));
                #else
            fnDebugHex(ulKeys[1], WITH_LEADIN | WITH_CR_LF | sizeof(ulKeys[1]));
                #endif
            #endif
        #endif
    }
    #endif
    #if !defined FRDM_KL26Z && !defined FRDM_KL25Z && !defined rcARM_KL26
    TSI0_GENCS = (TSI_GENCS_STM_SW_TRIG | TSI_GENCS_SWTS | TSI_GENCS_TSIEN | TSI_GENCS_PS_32); // re-trigger touch sensor measurement
    #endif
    return ulCols;
}
#endif
#endif
