/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      iic_tests.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    28.09.2010 Add TEST_SENSIRION
    09.04.2014 Add MMA8451Q, MMA7660F and FXOS8700 - 3/6-axis accelerometer/orientation/motion detection {1}
    17.06.2014 Allow the control of multi-coloured LEDs based on accelerometer values {2}
    10.09.2015 Add FXOS8700_14BIT_RES to allow 14 bit mode
    24.09.2015 Add I2C slave test                                        // {3}

    The file is otherwise not specifically linked in to the project since it
    is included by application.c when needed.

*/

#if defined IIC_INTERFACE && !defined _IIC_CONFIG
    #define _IIC_CONFIG

    #if defined IIC_SLAVE_MODE
      //#define TEST_IIC_SLAVE                                           // test behaving as I2C slave
            #define OUR_SLAVE_ADDRESS   0xd0
    #endif
  //#define TEST_IIC                                                     // test IIC EEPROM
  //#define TEST_IIC_INTENSIVE                                           // intensive transmitter test
  //#define TEST_DS1307                                                  // test DS1307 RTC via IIC bus
  //#define TEST_SENSIRION                                               // test reading temperature and humidity 
    #define TEST_MMA8451Q                                                // test monitoring the 3-axis accelerometer
    #if defined TEST_MMA8451Q
        #define MMA8451Q_14BIT_RES
      //#define INTERRUPT_ON_READY                                       // enable tap detection and interrupt
    #endif
  //#define TEST_MMA7660F                                                // test monitoring the 3-axis accelerometer
  //#define TEST_FXOS8700                                                // test monitoring the 6-axis sensor
    #if defined TEST_FXOS8700
      //#define FXOS8700_14BIT_RES
    #endif
  //#define DISPLAY_ACCELEROMETER_VALUES                                 // print values to debug output irrespective of debug setting

    #if defined TEST_IIC
        #define ADD_EEPROM_READ           0xa5                           // read address of I2C EEPROM
        #define ADD_EEPROM_WRITE          0xa4                           // write address of I2C EEPROM
    #endif
    #if defined TEST_DS1307
        #define ADDRTC_READ               0xd1                           // read address of DS1307
        #define ADDRTC_WRITE              0xd0                           // write address of DS1307
        #define RTC_CONTROL               0x07                           // location of the RTC control register
        #define RTC_MODE                  0x10                           // enable 1Hz square wave output
        #define CLOCK_NOT_ENABLED         0x80                           // not-enabled bit

        #define STATE_INIT_RTC            0x01                           // in the process of initialising the RTC
        #define STATE_GET_RTC             0x02                           // in the process of retrieving the present time
    #endif
    #if defined TEST_SENSIRION
        #define ADDSHT21_READ             0x81                           // read address of SHT21
        #define ADDSHT21_WRITE            0x80                           // write address of SHT21

        #define TRIGGER_TEMPERATURE_HOLD_MASTER 0xe3
        #define TRIGGER_HUMIDITY_HOLD_MASTER    0xe5

        #define STATE_READING_TEMPERATURE 0x01
        #define STATE_READING_HUMIDITY    0x02
        #define STATE_PAUSE               0x03
    #endif
    #if defined TEST_MMA8451Q                                            // {1}
        #if defined FRDM_K22F || defined TWR_K21F120M || defined TWR_K24F120M || defined TWR_K64F120M || defined TWR_K21D50M || defined TWR_K65F180M || defined FRDM_K66F
            #define MMA8451Q_READ_ADDRESS     0x39                       // read address of MMA8451Q [SA0 is '0']
            #define MMA8451Q_WRITE_ADDRESS    0x38                       // write address of MMA8451
        #elif defined TWR_K22F120M
            #define MMA8451Q_READ_ADDRESS     0x3f                       // read address of MMA8451Q [SA0 is '0']
            #define MMA8451Q_WRITE_ADDRESS    0x3e                       // write address of MMA8451Q
        #else
            #define MMA8451Q_READ_ADDRESS     0x3b                       // read address of MMA8451Q [SA0 is '1']
            #define MMA8451Q_WRITE_ADDRESS    0x3a                       // write address of MMA8451Q
        #endif

        #define ACC_CONTROL_REGISTER      0x2a
        #define ACC_CONTROL_REGISTER_ACTIVE            0x01
        #define ACC_CONTROL_REGISTER_F_READ            0x02
        #define ACC_CONTROL_REGISTER_LNOISE            0x04
        #define ACC_CONTROL_REGISTER_DATA_RATE_800Hz   0x00
        #define ACC_CONTROL_REGISTER_DATA_RATE_400Hz   0x08
        #define ACC_CONTROL_REGISTER_DATA_RATE_200Hz   0x10
        #define ACC_CONTROL_REGISTER_DATA_RATE_100Hz   0x18
        #define ACC_CONTROL_REGISTER_DATA_RATE_50Hz    0x20
        #define ACC_CONTROL_REGISTER_DATA_RATE_12_5Hz  0x28
        #define ACC_CONTROL_REGISTER_DATA_RATE_6_25Hz  0x30
        #define ACC_CONTROL_REGISTER_DATA_RATE_1_56Hz  0x38
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_50Hz   0x00
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_12_5Hz 0x40
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_6_25Hz 0x80
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_1_56Hz 0xc0

        #define ACC_CONTROL_REGISTER_3    0x2c
        #define ACC_CONTROL_REGISTER_3_OPEN_DRAIN      0x01
        #define ACC_CONTROL_REGISTER_3_IPOL_ACT_HIGH   0x02
        #define ACC_CONTROL_REGISTER_4    0x2d
        #define ACC_CONTROL_REGISTER_4_INT_EN_DATA_RDY 0x01
        #define ACC_CONTROL_REGISTER_4_INT_EN_FREEFALL 0x04
        #define ACC_CONTROL_REGISTER_4_INT_EN_PULSE    0x08
        #define ACC_CONTROL_REGISTER_4_INT_EN_LAND_POR 0x10
        #define ACC_CONTROL_REGISTER_4_INT_EN_TRANS   0x20
        #define ACC_CONTROL_REGISTER_4_INT_EN_FIFO     0x40
        #define ACC_CONTROL_REGISTER_4_INT_EN_SLEEP_WK 0x80
        #define ACC_CONTROL_REGISTER_5    0x2e
        #define ACC_CONTROL_REGISTER_5_INT_1_DATA_RDY  0x01
        #define ACC_CONTROL_REGISTER_5_INT_2_DATA_RDY  0x00
        #define ACC_CONTROL_REGISTER_5_INT_1_FREEFALL  0x04
        #define ACC_CONTROL_REGISTER_5_INT_2_FREEFALL  0x00
        #define ACC_CONTROL_REGISTER_5_INT_1_PULSE     0x08
        #define ACC_CONTROL_REGISTER_5_INT_2_PULSE     0x00
        #define ACC_CONTROL_REGISTER_5_INT_1_LAND_POR  0x10
        #define ACC_CONTROL_REGISTER_5_INT_2_LAND_POR  0x00
        #define ACC_CONTROL_REGISTER_5_INT_1_TRANS     0x20
        #define ACC_CONTROL_REGISTER_5_INT_2_LAND_POR  0x00
        #define ACC_CONTROL_REGISTER_5_INT_1_FIFO      0x40
        #define ACC_CONTROL_REGISTER_5_INT_2_FIFO      0x00
        #define ACC_CONTROL_REGISTER_5_INT_1_SLEEP_WK  0x80
        #define ACC_CONTROL_REGISTER_5_INT_2_SLEEP_WK  0x00

        #define ACC_START_ADDRESS         9                              // start at the F_SETUP register
        #define ACC_READ_LENGTH           41                             // read 41 registers from the start location
    #elif defined TEST_MMA7660F
        #define MMA7660F_READ_ADDRESS     0x99                           // read address of MMA7660F
        #define MMA7660F_WRITE_ADDRESS    0x98                           // write address of MMA7660F

        #define ACC_MODE_REGISTER         7
        #define ACC_MODE_REGISTER_ACTIVE  0x01

        #define ACC_START_ADDRESS         0                              // start at the XOUT register
        #define ACC_READ_LENGTH           11                             // read 11 registers from the start location
    #elif defined TEST_FXOS8700
        #if defined FRDM_K22F || defined TWR_K22F120M
            #define FXOS8700_READ_ADDRESS     0x3f                       // read address of FXOS8700 // assumes SA1 is '1' and SA0 is '1'
            #define FXOS8700_WRITE_ADDRESS    0x3e                       // write address of FXOS8700
        #elif defined KINETIS_K80
            #define FXOS8700_READ_ADDRESS     0x39                       // read address of FXOS8700 // assumes SA1 is '1' and SA0 is '0'
            #define FXOS8700_WRITE_ADDRESS    0x38                       // write address of FXOS8700
        #else
            #define FXOS8700_READ_ADDRESS     0x3b                       // read address of FXOS8700 // assumes SA1 is '0' and SA0 is '1' {SA1/0 = '00' would be 0x3c} {SA1/0 = '10' would be 0x38} {SA1/0 = '11' would be 0x3e}
            #define FXOS8700_WRITE_ADDRESS    0x3a                       // write address of FXOS8700
        #endif

        #define ACC_CONTROL_REGISTER      0x2a
        #define ACC_CONTROL_REGISTER_ACTIVE            0x01
        #define ACC_CONTROL_REGISTER_F_READ            0x02
        #define ACC_CONTROL_REGISTER_LNOISE            0x04
        #define ACC_CONTROL_REGISTER_DATA_RATE_800Hz   0x00
        #define ACC_CONTROL_REGISTER_DATA_RATE_400Hz   0x08
        #define ACC_CONTROL_REGISTER_DATA_RATE_200Hz   0x10
        #define ACC_CONTROL_REGISTER_DATA_RATE_100Hz   0x18
        #define ACC_CONTROL_REGISTER_DATA_RATE_50Hz    0x20
        #define ACC_CONTROL_REGISTER_DATA_RATE_12_5Hz  0x28
        #define ACC_CONTROL_REGISTER_DATA_RATE_6_25Hz  0x30
        #define ACC_CONTROL_REGISTER_DATA_RATE_1_56Hz  0x38
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_50Hz   0x00
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_12_5Hz 0x40
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_6_25Hz 0x80
        #define ACC_CONTROL_REGISTER_SLEEP_RATE_1_56Hz 0xc0

        #define M_OUT_ADDRESS             0x33

        #define ACC_START_ADDRESS         9                              // start at the F_SETUP register
        #define ACC_READ_LENGTH           112                            // read 112 registers from the start location
    #endif

    #define ACC_INITIALISING              0
    #define ACC_X_Y_Z                     1                              // reading X,Y,Z data
    #define ACC_WAITING                   2                              // waiting for accelerometer to interrupt
    #define ACC_TRIGGERED                 3                              // accelerometer has interrupted so we should read data
    #define ACC_MAGNETOMETER              4                              // reading magnetic data

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

    #if defined TEST_DS1307                                              // local structure of present time
        typedef struct stTIME_STRUCTURE
        {
          unsigned char ucYear;                                          // 6 is 2006, 7 is 2007 etc.
          unsigned char ucMonthOfYear;                                   // 0..11
          unsigned char ucDayOfMonth;                                    // 0..31
          unsigned char ucDayOfWeek;                                     // 0..6
          unsigned char ucHourOfDay;                                     // 0..23
          unsigned char ucMinuteOfHour;                                  // 0..59
          unsigned char ucSeconds;                                       // 0..59
        } TIME_STRUCTURE;
    #endif


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

    #if defined TEST_IIC || defined IIC_SLAVE_MODE || defined TEST_DS1307 || defined TEST_SENSIRION || defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700
        static void fnConfigIIC_Interface(void);
    #endif
    #if defined TEST_MMA8451Q && defined INTERRUPT_ON_READY
        static void acc_data_ready(void);
    #endif

    #if defined TEST_DS1307
        static void fnSaveTime(void);
        static void fnSetTimeStruct(unsigned char *ucInputMessage);
        static void seconds_interrupt(void);
    #endif
    #if defined TEST_SENSIRION
        static void fnNextSensorRequest(void);
    #endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

    #if defined TEST_IIC || defined IIC_SLAVE_MODE || defined TEST_DS1307 || defined TEST_SENSIRION || defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700
        QUEUE_HANDLE IICPortID = NO_ID_ALLOCATED;                        // handle of I2C interface (global so that the debug task can access it)
    #endif

    #if defined TEST_MMA8451Q && defined USE_USB_HID_MOUSE               // tilt mouse values
        int iLeftTilt = 0;
        int iRightTilt = 0;
        int iUpTilt = 0;
        int iDownTilt = 0;
    #endif


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


    #if defined TEST_MMA8451Q                                            // {1}
        static const unsigned char ucSetAccelerometerAddress[] = {MMA8451Q_WRITE_ADDRESS, ACC_START_ADDRESS}; // command to set address to read to the start address
        static const unsigned char ucReadAccelerometerRegisters[] = {ACC_READ_LENGTH, MMA8451Q_READ_ADDRESS, OWN_TASK}; // command to start a read the defined amount of bytes with the task scheduled when the read has completed
        static const unsigned char ucSetAccelerometerRead[] = {MMA8451Q_WRITE_ADDRESS, 0}; // command to set address to read to the first register address (status)
        #if defined MMA8451Q_14BIT_RES
            #define RESULT_LENGTH 7
        #else
            #define RESULT_LENGTH 4
        #endif
        static const unsigned char ucReadAccelerometerState[] = {RESULT_LENGTH, MMA8451Q_READ_ADDRESS, OWN_TASK}; // command to start a read the defined amount of bytes with the task scheduled when the read has completed
        #if defined INTERRUPT_ON_READY
            #if defined ACC_USE_INT2
                static const unsigned char ucRouteIrq[] = {MMA8451Q_WRITE_ADDRESS, ACC_CONTROL_REGISTER_5, (ACC_CONTROL_REGISTER_5_INT_2_DATA_RDY)}; // route data ready interrupt to INT2
            #else
                static const unsigned char ucRouteIrq[] = {MMA8451Q_WRITE_ADDRESS, ACC_CONTROL_REGISTER_5, (ACC_CONTROL_REGISTER_5_INT_1_DATA_RDY)}; // route data ready interrupt to INT1
            #endif
            static const unsigned char ucConfigureIrq[] = {MMA8451Q_WRITE_ADDRESS, ACC_CONTROL_REGISTER_4, (ACC_CONTROL_REGISTER_4_INT_EN_DATA_RDY)}; // command to enable interrupt
        #endif
        #if defined MMA8451Q_14BIT_RES
            static const unsigned char ucSetAccelerometerMode[] = {MMA8451Q_WRITE_ADDRESS, ACC_CONTROL_REGISTER, (ACC_CONTROL_REGISTER_ACTIVE | ACC_CONTROL_REGISTER_DATA_RATE_50Hz | ACC_CONTROL_REGISTER_SLEEP_RATE_6_25Hz | ACC_CONTROL_REGISTER_LNOISE)}; // command to set the 14-bit resolution mode
        #else
            static const unsigned char ucSetAccelerometerMode[] = {MMA8451Q_WRITE_ADDRESS, ACC_CONTROL_REGISTER, (ACC_CONTROL_REGISTER_ACTIVE | ACC_CONTROL_REGISTER_DATA_RATE_50Hz | ACC_CONTROL_REGISTER_SLEEP_RATE_6_25Hz | ACC_CONTROL_REGISTER_LNOISE | ACC_CONTROL_REGISTER_F_READ)}; // command to set the 8 bit (fast) mode
        #endif
    #elif defined TEST_FXOS8700
        #if defined FXOS8700_14BIT_RES
            #define RESULT_LENGTH   7
        #else
            #define RESULT_LENGTH   4
        #endif
        static const unsigned char ucSetAccelerometerAddress[] = {FXOS8700_WRITE_ADDRESS, ACC_START_ADDRESS}; // command to set address to read to the start address
        static const unsigned char ucReadAccelerometerRegisters[] = {ACC_READ_LENGTH, FXOS8700_READ_ADDRESS, OWN_TASK}; // command to start a read the defined amount of bytes with the task scheduled when the read has completed
        static const unsigned char ucSetAccelerometerRead[] = {FXOS8700_WRITE_ADDRESS, 0}; // command to set address to read to the first register address (status)
        static const unsigned char ucReadAccelerometerState[] = {RESULT_LENGTH, FXOS8700_READ_ADDRESS, OWN_TASK}; // command to start a read of the defined amount of bytes with the task scheduled when the read has completed
        static const unsigned char ucSetMagnetometerRead[] = {FXOS8700_WRITE_ADDRESS, M_OUT_ADDRESS}; // command to set address to read to the first magnetic data register address
        static const unsigned char ucReadMagnetometerState[] = {6, FXOS8700_READ_ADDRESS, OWN_TASK}; // command to start a read the defined amount of bytes with the task scheduled when the read has completed
        #if defined FXOS8700_14BIT_RES
            static const unsigned char ucSetAccelerometerMode[] = {FXOS8700_WRITE_ADDRESS, ACC_CONTROL_REGISTER, (ACC_CONTROL_REGISTER_ACTIVE | ACC_CONTROL_REGISTER_DATA_RATE_6_25Hz | ACC_CONTROL_REGISTER_SLEEP_RATE_6_25Hz | ACC_CONTROL_REGISTER_LNOISE)}; // command to set the 14-bit resolution mode
        #else
            static const unsigned char ucSetAccelerometerMode[] = {FXOS8700_WRITE_ADDRESS, ACC_CONTROL_REGISTER, (ACC_CONTROL_REGISTER_ACTIVE | ACC_CONTROL_REGISTER_DATA_RATE_6_25Hz | ACC_CONTROL_REGISTER_SLEEP_RATE_6_25Hz | ACC_CONTROL_REGISTER_LNOISE | ACC_CONTROL_REGISTER_F_READ)}; // command to set the mode
        #endif
    #elif defined TEST_MMA7660F
        #define RESULT_LENGTH   4
        static const unsigned char ucSetAccelerometerAddress[] = {MMA7660F_WRITE_ADDRESS, ACC_START_ADDRESS}; // command to set address to read to the start address
        static const unsigned char ucReadAccelerometerRegisters[] = {ACC_READ_LENGTH, MMA7660F_READ_ADDRESS, OWN_TASK}; // command to start a read of the defined amount of bytes with the task scheduled when the read has completed
        static const unsigned char ucSetAccelerometerRead[] = {MMA7660F_WRITE_ADDRESS, 0}; // command to set address to read to the first register address (status)
        static const unsigned char ucReadAccelerometerState[] = {4, MMA7660F_READ_ADDRESS, OWN_TASK}; // command to start a read the defined amount of bytes with the task scheduled when the read has completed
        static const unsigned char ucSetAccelerometerMode[] = {MMA7660F_WRITE_ADDRESS, ACC_MODE_REGISTER, (ACC_MODE_REGISTER_ACTIVE)}; // command to set the active mode
    #endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

    #if defined TEST_DS1307
        static int iRTC_state = 0;
        static TIME_STRUCTURE stPresentTime;                             // time structure with present data and time
    #elif defined TEST_SENSIRION
        static int iSensor_state = STATE_PAUSE;
    #elif defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700 // {1}
        static int iAccelerometerState = ACC_INITIALISING;
    #endif
#endif


#if defined _IIC_INIT_CODE && (defined TEST_IIC || defined IIC_SLAVE_MODE || defined TEST_IIC_SLAVE || defined TEST_DS1307 || defined TEST_SENSIRION || defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700)

    #if defined TEST_IIC_SLAVE
// The function is called during I2C slave interrupt handling so that the application can immediately respond in order to realise an I2C save device
//
static int fnI2C_SlaveCallback(int iChannel, unsigned char *ptrDataByte, int iType)
{
    #define I2C_RAM_IDLE              0                                  // RAM pointer states
    #define SET_ADDRESS_POINTER       1
    static unsigned char usRAM[256] = {0};                               // RAM buffer, initially zeroed
    static unsigned char ucState = I2C_RAM_IDLE;                         // initially idle
    static unsigned char ucAddress = 0;                                  // RAM address pointer is reset to zero
    switch (iType) {                                                     // the interrupt callback type
    case I2C_SLAVE_ADDRESSED_FOR_READ:                                   // the slave is being addressed for reading from
    case I2C_SLAVE_READ:                                                 // further reads
        *ptrDataByte = usRAM[ucAddress++];                               // return the data and increment the address pointer
        ucState = I2C_RAM_IDLE;                                          // return to the idle state
        return I2C_SLAVE_TX_PREPARED;                                    // the prepared byte is to to be sent
    case I2C_SLAVE_READ_COMPLETE:                                        // complete read is complete
        break;

    case I2C_SLAVE_ADDRESSED_FOR_WRITE:                                  // the slave is being addressed to write to
        // *ptrDataByte is our address
        //
        ucState = SET_ADDRESS_POINTER;                                   // a write is being received and we expect the address followed by a number of data bytes
        return I2C_SLAVE_RX_CONSUMED;                                    // the byte has been consumed and nothing is to be put in the queue buffer

    case I2C_SLAVE_WRITE:                                                // data byte received
        // *ptrDataByte is the data received
        //
        if (ucState == SET_ADDRESS_POINTER) {                            // we are expecting the address to be received
            ucAddress = *ptrDataByte;                                    // set the single-byte address
            ucState = I2C_RAM_IDLE;                                      // return to data reception
        }
        else {
            usRAM[ucAddress++] = *ptrDataByte;                           // save the data and increment the address pointer
        }
        return I2C_SLAVE_RX_CONSUMED;                                    // the byte has been consumed and nothing is to be put in the queue buffer

    case I2C_SLAVE_WRITE_COMPLETE:                                       // the write has terminated
        // ptrDataByte is 0 and so should not be used
        //
        return I2C_SLAVE_RX_CONSUMED;
    }
    return I2C_SLAVE_BUFFER;                                             // use the buffer for this transaction
}
    #endif

// Open IIC interface to communicate with an EEPROM, RTC, etc.
//
static void fnConfigIIC_Interface(void)
{
    IICTABLE tIICParameters;

    tIICParameters.Channel = OUR_IIC_CHANNEL;
    #if defined TEST_IIC_SLAVE                                           // {3}
    tIICParameters.ucSlaveAddress = OUR_SLAVE_ADDRESS;                   // slave address
    tIICParameters.usSpeed = 0;                                          // select slave mode of operation
    tIICParameters.fnI2C_SlaveCallback = fnI2C_SlaveCallback;            // the I2C save interrupt callback on reception
    #elif (defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700) && !defined KL43Z_256_32_CL // {1}
    tIICParameters.usSpeed = 50;                                         // 50k
    #else
    tIICParameters.usSpeed = 100;                                        // 100k
    #endif
    #if defined KL43Z_256_32_CL
    tIICParameters.Rx_tx_sizes.TxQueueSize = 256;                        // transmit queue size
    #else
    tIICParameters.Rx_tx_sizes.TxQueueSize = 64;                         // transmit queue size
    #endif
    #if defined TEST_FXOS8700
        tIICParameters.Rx_tx_sizes.RxQueueSize = 128;                    // receive queue size
    #else
        tIICParameters.Rx_tx_sizes.RxQueueSize = 64;                     // receive queue size
    #endif
    #if defined TEST_IIC_SLAVE
    tIICParameters.Task_to_wake = OWN_TASK;                              // wake application task when slave transaction has completed
    #else
    tIICParameters.Task_to_wake = 0;                                     // no wake on transmission
    #endif

    if ((IICPortID = fnOpen(TYPE_IIC, FOR_I_O, &tIICParameters)) != NO_ID_ALLOCATED) { // open the channel with defined configurations
    #if !defined TEST_IIC_SLAVE                                          // when slave we don't initiate any activity but instead wait to be addressed
        #if defined TEST_IIC
            #if defined TEST_IIC_INTENSIVE
        iAppState |= STATE_POLLING;                                      // mark test running
        uTaskerStateChange(OWN_TASK, UTASKER_GO);                        // set to polling mode
            #else
        static const unsigned char ucSetEEPROMAddress0[] = {ADD_EEPROM_WRITE, 0}; // command to set address to read to 0
        static const unsigned char ucReadEEPROM[] = {16, ADD_EEPROM_READ, OWN_TASK}; // command to start a read of 16 bytes with the task scheduled when the read has completed
        fnWrite(IICPortID, (unsigned char *)ucSetEEPROMAddress0, sizeof(ucSetEEPROMAddress0)); // write the EEPROM address to read
        fnRead(IICPortID, (unsigned char *)ucReadEEPROM, 0);             // start the read process of 16 bytes
            #endif	
        #elif defined TEST_DS1307
        static const unsigned char ucStartRTC[] = {ADDRTC_WRITE, RTC_CONTROL, RTC_MODE};
        fnWrite(IICPortID, (unsigned char *)ucStartRTC, sizeof(ucStartRTC)); // initialise RTC - set 1Hz output mode
        fnGetRTCTime();                                                  // get the time (start RTC if not yet running)
        iRTC_state = STATE_INIT_RTC;                                     // mark that we are initialising
        #elif defined TEST_SENSIRION
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(4.0 * SEC), E_NEXT_SENSOR_REQUEST); // start reading sequence after a delay
        #elif (defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700) && !defined KL43Z_256_32_CL // {1}
        fnWrite(IICPortID, (unsigned char *)ucSetAccelerometerAddress, sizeof(ucSetAccelerometerAddress)); // write the register address to read from
        fnRead(IICPortID, (unsigned char *)ucReadAccelerometerRegisters, 0); // start the read process of the required amount of bytes
        #endif
    #endif
    }
}

    #if defined TEST_MMA8451Q && defined INTERRUPT_ON_READY
// Accelerometer data ready (interrupt)
//
static void acc_data_ready(void)
{
    if (ACC_WAITING == iAccelerometerState) {                            // ignore if the interrupt is no expeced
        iAccelerometerState = ACC_TRIGGERED;
        uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);                  // schedule the task to handle the new state and request the data
    }
}
    #endif
#endif


#if defined _IIC_READ_CODE && (defined TEST_IIC || defined TEST_IIC_SLAVE)
    #if defined TEST_IIC_INTENSIVE && !defined TEST_IIC_SLAVE
    if (iAppState & STATE_POLLING) {                                     // if I2C intensive test active
        static unsigned char iic_testMsg[] = {0xaa, 0x00};               // fictional I2C address
        int iLoop = 20;
        while (iLoop--) {
            if (fnWrite(IICPortID, 0, sizeof(iic_testMsg)) > 0) {        // check for room in output queue
                if (fnWrite(IICPortID, iic_testMsg, sizeof(iic_testMsg)) < sizeof(iic_testMsg)) {
                    iic_testMsg[1] = 0;                                  // error
                }
                iic_testMsg[1]++;
            }
            else {
                break;
            }
        }
        uTaskerStateChange(OWN_TASK, UTASKER_GO);                        // ensure we remain in polling mode
    }
    #endif
    if (fnMsgs(IICPortID) != 0) {                                        // if I2C message waiting
    #if defined TEST_IIC_SLAVE
        while (fnRead(IICPortID, ucInputMessage, 1) != 0) {              // while messages
            unsigned char ucMessageLength = ucInputMessage[0];           // the length of the content
            int x = 0;
            unsigned char ucReject;
            fnDebugMsg("I2C Slave reception: ");
            if (ucMessageLength > sizeof(ucInputMessage)) {
                ucReject = (ucMessageLength - sizeof(ucInputMessage));
                ucMessageLength = sizeof(ucInputMessage);
            }
            else {
                ucReject = 0;
            }
            fnRead(IICPortID, ucInputMessage, ucMessageLength);          // read the message content
            while (x < ucMessageLength) {                                // display received bytes
                fnDebugHex(ucInputMessage[x++], (WITH_LEADIN | WITH_SPACE | 1));
            }
            while (ucReject != 0) {
                fnRead(IICPortID, ucInputMessage, 1);                    // empty in case of over-length message
                ucReject--;
            }
            fnDebugMsg("\r\n");
        }
    #else
        while ((Length = fnRead(IICPortID, ucInputMessage, MEDIUM_MESSAGE)) != 0) {
            static const unsigned char ucSetWriteEEPROM1[] = {ADD_EEPROM_WRITE, 3, 5}; // prepare write of one byte to address 3
            static const unsigned char ucSetWriteEEPROM2[] = {ADD_EEPROM_WRITE, 5, 3, 4, 5, 6, 7, 8, 9, 10}; // prepare write of multiple bytes to address 5

            int x = 0;
            while (x < Length) {                                         // display received bytes
                fnDebugHex(ucInputMessage[x++], (WITH_LEADIN | WITH_SPACE | 1));
            }
            fnDebugMsg("\r\n");
                                                                         // now change the contents using different writes
            fnWrite(IICPortID, (unsigned char *)&ucSetWriteEEPROM1, sizeof(ucSetWriteEEPROM1)); // start single byte write
            fnWrite(IICPortID, (unsigned char *)&ucSetWriteEEPROM2, sizeof(ucSetWriteEEPROM2)); // start page write
        }
    #endif
    }
#elif defined _IIC_READ_CODE && defined TEST_DS1307
    if ((iRTC_state & (STATE_INIT_RTC | STATE_GET_RTC)) && (fnMsgs(IICPortID) >= 7)) {
        fnRead(IICPortID, ucInputMessage, 7);                            // get the time and put it into the local time structure
        if (ucInputMessage[0] & CLOCK_NOT_ENABLED) {
            uMemset(&stPresentTime, 0, sizeof(stPresentTime));           // start with cleared memory
            uMemset(ucInputMessage, 0, 7);
            fnSaveTime();                                                // write to RTC, which will also enable oscillator (performed once after power up only)
        }
        fnSetTimeStruct(ucInputMessage);                                 // convert the received time and date to a local format

        if (iRTC_state & STATE_INIT_RTC) {                               // define an input for 1s interrupt
            INTERRUPT_SETUP interrupt_setup;

            interrupt_setup.int_type = PORT_INTERRUPT;                   // identifier when configuring
            interrupt_setup.int_handler = seconds_interrupt;             // handling function
            interrupt_setup.int_port_sense = IRQ_RISING_EDGE;            // interrupt on this edge
    #if defined _M5223X
            interrupt_setup.int_priority = (INTERRUPT_LEVEL_6);          // edge port interrupt priority
            interrupt_setup.int_port_bit = 1;                            // the IRQ input connected
    #elif defined _HW_SAM7X
            interrupt_setup.int_priority = (0);                          // port interrupt priority
            interrupt_setup.int_port = PORT_A;                           // the port used
            interrupt_setup.int_port_bits = PA30;                        // the input connected
    #elif defined _HW_AVR32
            interrupt_setup.int_priority = PRIORITY_GPIO;                // port interrupt priority
            interrupt_setup.int_port = PORT_1;                           // the port used
            interrupt_setup.int_port_bits = PB30;                        // the input connected
    #elif defined _LM3SXXXX
            interrupt_setup.int_priority = 3;                            // port interrupt priority
            interrupt_setup.int_port = PORT_C;                           // the port used
            interrupt_setup.int_port_bit = 6;                            // the input connected
            interrupt_setup.int_port_characteristic = PULLUP_ON;         // enable a pullup
    #elif defined _LPC23XX
            interrupt_setup.int_priority = 8;                            // port interrupt priority
            interrupt_setup.int_port = PORT_0;                           // the port used
            interrupt_setup.int_port_bits = PORT0_BIT7;                  // the input connected
            interrupt_setup.int_port_sense = (IRQ_RISING_EDGE | PULLUP_ON); // interrupt on this edge and activate pullup resistor
    #elif defined _KINETIS
            interrupt_setup.int_priority = PRIORITY_PORT_D_INT;
            interrupt_setup.int_port = PORTD;
            interrupt_setup.int_port_bits = PORTD_BIT1;
    #endif
            fnConfigureInterrupt((void *)&interrupt_setup);              // configure the 1 second interrupt
        }
        iRTC_state = 0;                                                  // RTC state idle
    }
#elif defined _IIC_READ_CODE && defined TEST_SENSIRION
    if (fnRead(IICPortID, ucInputMessage, 3) != 0) {                     // one result at a time, always 3 bytes in length
        switch (iSensor_state) {
        case STATE_READING_TEMPERATURE:                                  // result is temperature measurement result
            {
                static const unsigned char ucTriggerHumidity[] = {ADDSHT21_WRITE, TRIGGER_HUMIDITY_HOLD_MASTER};
                static const unsigned char ucReadHumidity[] = {3, ADDSHT21_READ, OWN_TASK};
                signed long slTemperature;
    #if defined TEMP_HUM_TEST
                GLCD_TEXT_POSITION text_pos;// = {PAINT_LIGHT, 2, 0, FONT_NINE_DOT};
                CHAR cTemp[20];
                CHAR *ptrDecimalPoint;

                #define ABOVE_LEFT_X   0
                #define ABOVE_LEFT_Y   0
                #define BOTTOM_RIGHT_X ((GLCD_X/CGLCD_PIXEL_SIZE) - 1)
                #define BOTTOM_RIGHT_Y ((GLCD_Y/CGLCD_PIXEL_SIZE) - 1)

                GLCD_STYLE graphic_style;

                GLCD_RECT_BLINK rect1;
                rect1.ucMode = (PAINT_DARK);
                rect1.rect_corners.usX_start = ABOVE_LEFT_X;
                rect1.rect_corners.usY_start = ABOVE_LEFT_Y + 55;
                rect1.rect_corners.usX_end = BOTTOM_RIGHT_X;
                rect1.rect_corners.usY_end = rect1.rect_corners.usY_start + 25;
                fnDoLCD_rect(&rect1);

                fnWrite(IICPortID, (unsigned char *)ucTriggerHumidity, sizeof(ucTriggerHumidity));
                fnRead(IICPortID, (unsigned char *)ucReadHumidity, 0);   // start the read process of humidity
                iSensor_state = STATE_READING_HUMIDITY;
                slTemperature = ucInputMessage[0];
                slTemperature <<= 8;
                slTemperature |= (ucInputMessage[1] & 0xfc);             // remove status bits
                slTemperature *= 17572;
                slTemperature >>= 16;
                slTemperature -= 4685;                                   // calculated temperature in °C x 100
                ptrDecimalPoint = fnBufferDec(slTemperature, DISPLAY_NEGATIVE, cTemp);
                *ptrDecimalPoint = *(ptrDecimalPoint - 1);
                *(ptrDecimalPoint - 1) = *(ptrDecimalPoint - 2);
                *(ptrDecimalPoint - 2) = '.';
                *(ptrDecimalPoint + 1) = ' ';
                *(ptrDecimalPoint + 2) = 'C';
                *(ptrDecimalPoint + 3) = 0;
              //fnDebugMsg("Temperature = ");
              //fnDebugMsg(cTemp);
              //fnDebugMsg(" C ");

                graphic_style.ucMode = STYLE_PIXEL_COLOR;
                slTemperature /= 100;
                if (slTemperature < 20) {
                    slTemperature = 20;
                }
                else if (slTemperature > 30) {
                    slTemperature = 30;
                }
                slTemperature -= 20;                                     // referenced to 20°C (max °10)
                slTemperature *= 20;                                     // range 0..200
                slTemperature += 55;                                     // range 55..255
                graphic_style.color = (COLORREF)RGB((unsigned char)slTemperature, (255 - ((unsigned char)slTemperature)), (255 - ((unsigned char)slTemperature))); // set color according to temperature
                fnDoLCD_style(&graphic_style);

                text_pos.usX = 40;
                text_pos.usY = 60;
                text_pos.ucFont = FONT_FIFTEEN_DOT;
                text_pos.ucMode = (PAINT_LIGHT | REDRAW /*| GIVE_ACK*/);
                fnDoLCD_text(&text_pos, cTemp);
                graphic_style.ucMode = STYLE_PIXEL_COLOR;
                graphic_style.color = (COLORREF)RGB(255,255,0);
                fnDoLCD_style(&graphic_style);                           // return text color
    #else
                CHAR cTemp[20];
                CHAR *ptrDecimalPoint;
                fnWrite(IICPortID, (unsigned char *)ucTriggerHumidity, sizeof(ucTriggerHumidity));
                fnRead(IICPortID, (unsigned char *)ucReadHumidity, 0);   // start the read process of humidity
                iSensor_state = STATE_READING_HUMIDITY;
                slTemperature = ucInputMessage[0];
                slTemperature <<= 8;
                slTemperature |= (ucInputMessage[1] & 0xfc);             // remove status bits
                slTemperature *= 17572;
                slTemperature >>= 16;
                slTemperature -= 4685;                                   // calculated temperature in °C x 100
                ptrDecimalPoint = fnBufferDec(slTemperature, DISPLAY_NEGATIVE, cTemp);
                *ptrDecimalPoint = *(ptrDecimalPoint - 1);
                *(ptrDecimalPoint - 1) = *(ptrDecimalPoint - 2);
                *(ptrDecimalPoint - 2) = '.';
                *(ptrDecimalPoint + 1) = 0;
                fnDebugMsg("Temperature = ");
                fnDebugMsg(cTemp);
                fnDebugMsg(" C ");
    #endif
            }
            break;
        case STATE_READING_HUMIDITY:                                     // result is humidity measurement result
            {
                CHAR cHumidity[20];
                CHAR *ptrDecimalPoint;
                unsigned long ulHumidity;
    #if defined TEMP_HUM_TEST
                GLCD_TEXT_POSITION text_pos;// = {PAINT_LIGHT, 2, 0, FONT_NINE_DOT};
                GLCD_RECT_BLINK rect1;

                rect1.ucMode = (PAINT_DARK);
                rect1.rect_corners.usX_start = ABOVE_LEFT_X;
                rect1.rect_corners.usY_start = ABOVE_LEFT_Y + 80;
                rect1.rect_corners.usX_end = BOTTOM_RIGHT_X;
                rect1.rect_corners.usY_end = BOTTOM_RIGHT_Y;
                fnDoLCD_rect(&rect1);
                ulHumidity = ucInputMessage[0];
                ulHumidity <<= 8;
                ulHumidity |= (ucInputMessage[1] & 0xfc);             // remove status bits
                ulHumidity *= 12500;
                ulHumidity >>= 16;
                ulHumidity -= 600;                                         // calculated humidity in % x 100
                ptrDecimalPoint = fnBufferDec(ulHumidity, 0, cHumidity);
                *ptrDecimalPoint = *(ptrDecimalPoint - 1);
                *(ptrDecimalPoint - 1) = *(ptrDecimalPoint - 2);
                *(ptrDecimalPoint - 2) = '.';
                *(ptrDecimalPoint + 1) = ' ';
                *(ptrDecimalPoint + 2) = '%';
                *(ptrDecimalPoint + 3) = 0;
              //fnDebugMsg("Humidity = ");
              //fnDebugMsg(cHumidity);
              //fnDebugMsg("%\r\n");


                rect1.ucMode = (PAINT_LIGHT);
                rect1.rect_corners.usX_start = 5;
                rect1.rect_corners.usY_start = 110;
                rect1.rect_corners.usX_end = 155;
                rect1.rect_corners.usY_end = 115;
                fnDoLCD_rect(&rect1);

                rect1.ucMode = (PAINT_DARK);
                ulHumidity /= 100;
                ulHumidity += (ulHumidity/2);
                ulHumidity += 6;
                rect1.rect_corners.usX_start = (unsigned char)ulHumidity;
                rect1.rect_corners.usY_start = 111;
                rect1.rect_corners.usX_end = 154;
                rect1.rect_corners.usY_end = 114;
                fnDoLCD_rect(&rect1);

                text_pos.usX = 25;
                text_pos.usY = 85;
                text_pos.ucFont = FONT_EIGHTEEN_DOT;
                text_pos.ucMode = (PAINT_LIGHT | REDRAW /*| GIVE_ACK*/);
                fnDoLCD_text(&text_pos, cHumidity);
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1.0 * SEC), E_NEXT_SENSOR_REQUEST); // repeat measurement pair after a delay
    #else
                ulHumidity = ucInputMessage[0];
                ulHumidity <<= 8;
                ulHumidity |= (ucInputMessage[1] & 0xfc);                // remove status bits
                ulHumidity *= 12500;
                ulHumidity >>= 16;
                ulHumidity -= 600;                                       // calculated humidity in % x 100
                ptrDecimalPoint = fnBufferDec(ulHumidity, 0, cHumidity);
                *ptrDecimalPoint = *(ptrDecimalPoint - 1);
                *(ptrDecimalPoint - 1) = *(ptrDecimalPoint - 2);
                *(ptrDecimalPoint - 2) = '.';
                *(ptrDecimalPoint + 1) = 0;
                fnDebugMsg("Humidity = ");
                fnDebugMsg(cHumidity);
                fnDebugMsg("%\r\n");
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(8.0 * SEC), E_NEXT_SENSOR_REQUEST); // repeat measurement pair after a delay
    #endif
                iSensor_state = STATE_PAUSE;
            }
            break;
        }
    }
#elif defined _IIC_READ_CODE && (defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700) // {1}
    switch (iAccelerometerState) {
    case ACC_INITIALISING:
        if (fnRead(IICPortID, ucInputMessage, ACC_READ_LENGTH) != 0) {   // if the read has completed
            int i = 0;
            int iLine;
            fnDebugMsg("3-axis accelerometer:\r\n");
            while (i < ACC_READ_LENGTH) {
                for (iLine = 0; iLine < 15; iLine++) {
                    fnDebugHex(ucInputMessage[i], (sizeof(ucInputMessage[i]) | WITH_LEADIN | WITH_SPACE)); // display the received register contents
                    if (++i >= ACC_READ_LENGTH) {
                        break;
                    }
                }
                fnDebugMsg("\r\n");
            }
            // We now set the operating mode
            //
    #if defined TEST_MMA8451Q && defined INTERRUPT_ON_READY              // interrupt setup
            {                                                            // configure a falling edge sensitive interrupt in INT1 output
                INTERRUPT_SETUP interrupt_setup;                         // interrupt configuration parameters
                interrupt_setup.int_type       = PORT_INTERRUPT;         // identifier to configure port interrupt
                interrupt_setup.int_handler    = acc_data_ready;         // handling function
                interrupt_setup.int_priority   = ACC_INT_PRIORITY;       // interrupt priority level
                interrupt_setup.int_port       = ACC_INT_PORT;           // the port that the interrupt input is on
                interrupt_setup.int_port_bits  = ACC_INT_BIT;            // the IRQ input connected
                interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON); // interrupt is to be falling edge sensitive
                fnConfigureInterrupt((void *)&interrupt_setup);          // configure interrupt
            }
            iAccelerometerState = ACC_WAITING;                           // waiting for interrupts from the accelerometer
            fnWrite(IICPortID, (unsigned char *)ucRouteIrq, sizeof(ucRouteIrq)); // route interrupt output to INT1
            fnWrite(IICPortID, (unsigned char *)ucConfigureIrq, sizeof(ucConfigureIrq)); // configure interrupt
            fnWrite(IICPortID, (unsigned char *)ucSetAccelerometerMode, sizeof(ucSetAccelerometerMode)); // write the operating mode
            if (ACC_INT_ASSERTED()) {                                    // if the accelerometer is already signalling that it has data (it is possible that it was previouly operating and has data ready, signaled by the interrupt line laread being low)
                acc_data_ready();                                        // start an initial read
            }
    #else
            fnWrite(IICPortID, (unsigned char *)ucSetAccelerometerMode, sizeof(ucSetAccelerometerMode)); // write the operating mode

            // Followed by the first status read
            //
            fnWrite(IICPortID, (unsigned char *)ucSetAccelerometerRead, sizeof(ucSetAccelerometerRead)); // write the register address to read
            fnRead(IICPortID, (unsigned char *)ucReadAccelerometerState, 0); // start the read process of the status
            iAccelerometerState = ACC_X_Y_Z;
    #endif
        }
        break;
    #if defined TEST_MMA8451Q && defined INTERRUPT_ON_READY
    case ACC_WAITING:                                                    // ignore in this state
        break;
    case ACC_TRIGGERED:                                                  // accelerometer has indicated that there is data to be read
        iAccelerometerState = ACC_X_Y_Z;
      //fnWrite(IICPortID, (unsigned char *)ucSetAccelerometerRead, sizeof(ucSetAccelerometerRead)); // write the register address to read [it is not necessary to set the address pointer since it operates in overflow mode]
        fnRead(IICPortID, (unsigned char *)ucReadAccelerometerState, 0); // start the read process of the status
        break;
    #endif
    case ACC_X_Y_Z:                                                      // we are expecting status data from the accelerometer to arrive
        if (fnRead(IICPortID, ucInputMessage, RESULT_LENGTH) != 0) {     // if the result read has completed
            static int iDisplayRate = 0;
    #if defined TEST_MMA8451Q && defined INTERRUPT_ON_READY
            #define ACC_DISPLAY_FILTER  25
            iAccelerometerState = ACC_WAITING;
    #else
            #define ACC_DISPLAY_FILTER  100
    #endif
    #if defined TEST_MMA8451Q && defined PWM_LED_CONTROL
        #if defined MMA8451Q_14BIT_RES
            fnSetColor((signed char)ucInputMessage[1], (signed char)ucInputMessage[3]); // this require PWM/timer output handling in ADC_Timers.h
        #else
            fnSetColor((signed char)ucInputMessage[1], (signed char)ucInputMessage[2]); // {2} this require PWM/timer output handling in ADC_Timers.h
        #endif
    #endif
            if (++iDisplayRate >= ACC_DISPLAY_FILTER) {
    #if !defined DISPLAY_ACCELEROMETER_VALUES && defined USE_MAINTENANCE
                if (iAccelOutput != 0) {
    #endif
                    int i = 0;
                    fnDebugMsg("3-axis state:");                         // display the status on a regular basis
                    while (i < RESULT_LENGTH) {                          // display 4 values
    #if defined MMA8451Q_14BIT_RES || defined FXOS8700_14BIT_RES
                        if (i == 0) {
                            fnDebugHex(ucInputMessage[i], (sizeof(ucInputMessage[i]) | WITH_LEADIN | WITH_SPACE)); // display the received register contents
                        }
                        else {
                            unsigned short usValue = ucInputMessage[i++];
                            usValue <<= 8;
                            usValue |= ucInputMessage[i];
                            fnDebugHex(usValue, (sizeof(usValue) | WITH_LEADIN | WITH_SPACE)); // display the received register contents
                        }
    #else                                                                // status, x, y, z with 8 bit resolution
                        fnDebugHex(ucInputMessage[i], (sizeof(ucInputMessage[i]) | WITH_LEADIN | WITH_SPACE)); // display the received register contents
    #endif
                        i++;
                    }
                    fnDebugMsg("\r\n");
    #if !defined DISPLAY_ACCELEROMETER_VALUES && defined USE_MAINTENANCE
                }
    #endif
    #if defined TEST_MMA8451Q && defined USE_USB_HID_MOUSE               // tilt mouse values
                iLeftTilt = 0;
                iRightTilt = 0;
                iUpTilt = 0;
                iDownTilt = 0;
                if ((signed char)ucInputMessage[2] > (signed char)0) {
                    iLeftTilt = ucInputMessage[2];
                }
                else if ((signed char)ucInputMessage[2] < (signed char)0) {
                    iRightTilt = -(signed char)ucInputMessage[2];
                }
                if ((signed char)ucInputMessage[1] > (signed char)0) {
                    iUpTilt = ucInputMessage[1];
                }
                else if ((signed char)ucInputMessage[1] < (signed char)0) {
                    iDownTilt = -(signed char)ucInputMessage[1];
                }
    #endif
                iDisplayRate = 0;
    #if defined TEST_FXOS8700
                fnWrite(IICPortID, (unsigned char *)ucSetMagnetometerRead, sizeof(ucSetMagnetometerRead)); // write the register address to read
                fnRead(IICPortID, (unsigned char *)ucReadMagnetometerState, 0); // start the read process of the next status
                iAccelerometerState = ACC_MAGNETOMETER;
                break;;
    #endif
            }
    #if defined TEST_MMA8451Q && defined INTERRUPT_ON_READY
            if (ACC_INT_ASSERTED()) {                                    // if the accelerometer is already signalling that it has data (this only happens when debugging since the IRQ line doesn't return high, causing he edge to be missed)
                acc_data_ready();                                        // start an initial read
            }
    #else
          //fnWrite(IICPortID, (unsigned char *)ucSetAccelerometerRead, sizeof(ucSetAccelerometerRead)); // write the register address to read (it is not necessary to set the address pointer since it operates in overflow mode)
            fnRead(IICPortID, (unsigned char *)ucReadAccelerometerState, 0); // start the read process of the next status
    #endif
        }
        break;
    #if defined TEST_FXOS8700
    case ACC_MAGNETOMETER:
        if (fnRead(IICPortID, ucInputMessage, 6) != 0) {                 // if the status read has completed
            static int iDisplayRate = 0;
            if (++iDisplayRate > 100) {
                int i = 0;
                unsigned short usState;
                fnDebugMsg("Mag. state:");                               // display the status on a regular basis
                while (i < 6) {
                    usState = (ucInputMessage[i++] << 8);                // 16 bit value
                    usState |= ucInputMessage[i++];
                    fnDebugHex(usState, (sizeof(usState) | WITH_LEADIN | WITH_SPACE)); // display the received register contents
                }
                fnDebugMsg("\r\n");
                iDisplayRate = 0;
                fnWrite(IICPortID, (unsigned char *)ucSetAccelerometerRead, sizeof(ucSetAccelerometerRead)); // write the register address to read
                fnRead(IICPortID, (unsigned char *)ucReadAccelerometerState, 0); // start the read process of the next status
                iAccelerometerState = ACC_X_Y_Z;
            }
            fnWrite(IICPortID, (unsigned char *)ucSetMagnetometerRead, sizeof(ucSetMagnetometerRead)); // write the register address to read
            fnRead(IICPortID, (unsigned char *)ucReadMagnetometerState, 0); // start the read process of the next status
        }
        break;
    #endif
    }
#endif


#if defined _IIC_SENSOR_CODE && defined TEST_SENSIRION
// This routine is called at a periodic rate to start next sensor value requests
//
static void fnNextSensorRequest(void)
{
    static const unsigned char ucTriggerTemperatur[] = {ADDSHT21_WRITE, TRIGGER_TEMPERATURE_HOLD_MASTER};
    static const unsigned char ucReadTemperature[] = {3, ADDSHT21_READ, OWN_TASK};
    fnWrite(IICPortID, (unsigned char *)ucTriggerTemperatur, sizeof(ucTriggerTemperatur));
    fnRead(IICPortID, (unsigned char *)ucReadTemperature, 0);            // start the read process of temperature
    iSensor_state = STATE_READING_TEMPERATURE;                           // mark that we expect the temperature result
}
#endif

#if defined _IIC_RTC_CODE && defined TEST_DS1307

// Get the time from the RTC - start RTC if it is not yet running
//
static void fnGetRTCTime(void)
{
    static const unsigned char ucGetTime[] =  {ADDRTC_WRITE, 0};
    static const unsigned char ucSlave[] = {7, ADDRTC_READ, OWN_TASK};   // read 7 bytes from this address
    fnWrite(IICPortID, (unsigned char*)ucGetTime, sizeof(ucGetTime));    // set the read address
    fnRead(IICPortID, (unsigned char *)ucSlave, 0);                      // start the read process of 7 bytes
}

// Converts between hex number and BCD, avoiding divides...
//
static unsigned char fnBCD(unsigned char ucHex)
{
    unsigned char ucTens = 0;

    while (ucHex >= 10) {
        ucHex -= 10;
        ucTens++;
    }
    ucTens <<= 4;
    ucTens += ucHex;
    return (ucTens);
}

// Save time to RTC and save user settings
//
static void fnSaveTime(void)
{
    unsigned char ucSetRTC[9]; //= {ADDRTC_WRITE, 0, 0, 0, 0, 0, 0, 0, 0}; {35}
    uMemset(ucSetRTC, 0, sizeof(ucSetRTC));
    ucSetRTC[0] = ADDRTC_WRITE;

    uDisable_Interrupt();                                                // protect from interrupts (increments of time) when converting
    ucSetRTC[2] = fnBCD(stPresentTime.ucSeconds);                        // Convert local time format to RTC format before saving
    ucSetRTC[3] = fnBCD(stPresentTime.ucMinuteOfHour);
    ucSetRTC[4] = fnBCD(stPresentTime.ucHourOfDay);
    ucSetRTC[6] = fnBCD(stPresentTime.ucDayOfMonth);
    ucSetRTC[7] = fnBCD(stPresentTime.ucMonthOfYear);
    ucSetRTC[8] = fnBCD(stPresentTime.ucYear);
    uEnable_Interrupt();

    fnWrite(IICPortID, (unsigned char *)ucSetRTC, sizeof(ucSetRTC));     // set new date and time
}

// Convert the received time to a local format
//
static void fnSetTimeStruct(unsigned char *ucInputMessage)
{
    stPresentTime.ucSeconds = ((*ucInputMessage & 0x70)>>4) * 10;
    stPresentTime.ucSeconds += *ucInputMessage++ & 0x0f;

    stPresentTime.ucMinuteOfHour = (*ucInputMessage >> 4) * 10;
    stPresentTime.ucMinuteOfHour += *ucInputMessage++ & 0x0f;

    stPresentTime.ucHourOfDay = ((*ucInputMessage & 0x30) >> 4) * 10;
    stPresentTime.ucHourOfDay += *ucInputMessage++ & 0x0f;

    stPresentTime.ucDayOfWeek = *ucInputMessage++;

    stPresentTime.ucDayOfMonth = (*ucInputMessage >> 4) * 10;
    stPresentTime.ucDayOfMonth += *ucInputMessage++ & 0x0f;

    stPresentTime.ucMonthOfYear = (*ucInputMessage >> 4) * 10;
    stPresentTime.ucMonthOfYear += *ucInputMessage++ & 0x0f;

    stPresentTime.ucYear = (*ucInputMessage >> 4) * 10;
    stPresentTime.ucYear += *ucInputMessage & 0x0f;
}

// This is called from an interrupt routine once a second to update the local time synchronous to the hardware RTC
//
static void seconds_interrupt(void)
{
    if (++stPresentTime.ucSeconds >= 60) {
        stPresentTime.ucSeconds = 0;
        if (++stPresentTime.ucMinuteOfHour >= 60) {
            stPresentTime.ucMinuteOfHour = 0;
            if (++stPresentTime.ucHourOfDay >= 24) {
                stPresentTime.ucHourOfDay = 0;
                if ((iRTC_state & (STATE_GET_RTC | STATE_INIT_RTC)) == 0) { // if we are not already getting the time
                    iRTC_state |= STATE_GET_RTC;                         // mark that we are expecting the time information
                    fnGetRTCTime();                                      // midnight - we update the date here since the hardware RTC handles the details of date changes
                }
            }
        }
    }
    TOGGLE_TEST_OUTPUT();
}
#endif

