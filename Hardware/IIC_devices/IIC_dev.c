/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      serial_dev.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    14.03.2007 Added MAX543X Digital Potentiometer (L,M,N,P suffixes)
    29.09.2007 Added LM80 (microprocessor system hardware monitor) and PCF8574 port expander
    13.10.2007 Added MAX3353 USB OTG Charge Pump with switchable Pullup/Pulldown resistors
    23.05.2008 Added synchronisation of internal battery backed up RTC    {1}
    23.10.2008 Added fixed part type identifier in serial number of DS3640{2}
    30.07.2009 Added LM75A temperature sensor                             {3}
    01.11.2009 Added DS1621 register initialisation and extended operation{4}
    15.08.2010 Added STMPE811 and touch screen interface                  {5}
    28.09.2010 Added Sensirion SHT21 temperature and humidity sensor      {6}
    27.08.2011 Added M24M01 EEPROMs                                       {7}
    13.03.2012 Added PCA9539s                                             {8}
    30.03.2012 Added PCF2129A                                             {9}
    09.10.2012 Added PCF8575                                              {10}
    09.04.2014 Added MMA8451Q, MMA7660F and FXOS8700 - 3/6-axis accelerometer/orientation/motion detection {11}

*/                            

// I2C Device simulation when communicating over I2C bus

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */


#include "config.h"



/**************************************************************************/
/*                  Dallas DS1621 Temperature sensor                      */
/**************************************************************************/

// Address 0x90
//
typedef struct stDS1621
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucCommand;
    unsigned char  ucConfigReg;
    unsigned char  ucOperation;

    unsigned char  ucTemperature[2];
} DS1621;

static DS1621 simDS1621 = {0x90, 0, 0, 0, 0, 0, 10, 0};                  // {4}


/**************************************************************************/
/*                  National LM75A Temperature sensor                     */
/**************************************************************************/

// Address 0x9e (Address pins pulled high)
//
typedef struct stLM75A
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucInternalPointer;

    unsigned char  ucTemperature[2];
    unsigned char  ucConfig;
    unsigned char  ucHyst[2];
    unsigned char  ucOS[2];
    unsigned char  ucID;
} LM75A;

static LM75A simLM75A = {0x9e, 0, 0, 0, {0x1a, 0x80}, 0, {0x4b, 0x00}, {0x50, 0x00}, 0xa1};


/**************************************************************************/
/*                  Dallas DS1307 RTC                                     */
/**************************************************************************/

// Address 0xd0
//
typedef struct stTIME_BLOCK
{     
    unsigned char ucSeconds;
    unsigned char ucMinutes;
    unsigned char ucHours;
    unsigned char ucDayOfWeek;
    unsigned char ucDayOfMonth;
    unsigned char ucMonth;
    unsigned char ucYear;

    unsigned char ucControl;
} TIME_BLOCK;


typedef struct stDS1307
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucInternalPointer;

    TIME_BLOCK     bTime;
    unsigned char  ucRAM[56];
} DS1307;

static DS1307 simDS1307 = {0xd0, 0};

/**************************************************************************/
/*                      PCF2129A RTC                                      */
/**************************************************************************/

// Address 0xa2                                                          {9}
//
typedef struct stPCF2129A_TIME_BLOCK
{
    unsigned char ucControl1;
    unsigned char ucControl2;
    unsigned char ucControl3;
    unsigned char ucSeconds;
    unsigned char ucMinutes;
    unsigned char ucHours;
    unsigned char ucDayOfMonth;
    unsigned char ucDayOfWeek;
    unsigned char ucMonth;
    unsigned char ucYears;
    unsigned char ucSecondAlarm;
    unsigned char ucMinuteAlarm;
    unsigned char ucHourAlarm;
    unsigned char ucDayAlarm;
    unsigned char ucWeekdayAlarm;
    unsigned char ucClockoutCtr;
    unsigned char ucWatchdogTimCtr;
    unsigned char ucWatchdogTimVal;
    unsigned char ucTimeStampCtr;
    unsigned char ucTimeStampSecond;
    unsigned char ucTimeStampMinute;
    unsigned char ucTimeStampHour;
    unsigned char ucTimeStampDay;
    unsigned char ucTimeStampMonth;
    unsigned char ucTimeStampYear;
    unsigned char ucAgingOffset;
} PCF2129A_TIME_BLOCK;


typedef struct stPCF2129A
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucInternalPointer;

    PCF2129A_TIME_BLOCK bTime;
} PCF2129A;

static PCF2129A simPCF2129A = {0xa2, 0};


/**************************************************************************/
/*                  Dallas DS3640 RTC and secure memory                   */
/**************************************************************************/

// Address 0xa0
//
typedef struct stTIME_BLOCK_S
{     
    unsigned char ucHundreds;
    unsigned char ucSeconds;
    unsigned char ucMinutes;
    unsigned char ucHours;
    unsigned char ucDayOfWeek;
    unsigned char ucDayOfMonth;
    unsigned char ucMonth;
    unsigned char ucYear;
    unsigned char ucWD[2];
    unsigned char ucAlarmMins;
    unsigned char ucAlarmHours;
    unsigned char ucRTC_control;
    unsigned char ucFilter;
    unsigned char ucTamperLatch1;
    unsigned char ucTamperLatch2;
    unsigned char ucSerialNumber[8];
    unsigned char ucRes1[7];
    unsigned char ucID_code;
    unsigned char ucControl[2];
    unsigned char ucStatus[2];
    unsigned char ucBattery[2];
    unsigned char ucRAW_temp[2];
    unsigned char ucHighTemp;
    unsigned char ucRes2;
    unsigned char ucLowTemp;
    unsigned char ucRes3;
    unsigned char ucDeltaTemp;
    unsigned char ucRes4[3];
    unsigned char ucTamperTimeStamp[8];
} TIME_BLOCK_S;


/**************************************************************************/
/*                      M24M01 EEPROM                                     */
/**************************************************************************/

#ifdef I2C_EEPROM_FILE_SYSTEM                                            // {7} M24M01 (note that these conflict with addresses for 24C01 and DS3640 so are not used together
    unsigned char M24M01_eeprom[2][128 * 1024];

    typedef struct stM24M01
    {     
        unsigned long  ulMaxEEPROMLength;
	    unsigned char  address;
        unsigned char  ucState;
        unsigned char  ucRW;
        unsigned long  ulInternalPointer;
    } M24M01;

    static M24M01 simM24M01[2] = {
        {0x20000, 0xa0, 0},
        {0x20000, 0xa4, 0},
    };

    // Initialise to deleted state
    //
    extern void fnInitI2C_EEPROM(void)
    {
        uMemset(M24M01_eeprom, 0xff, sizeof(M24M01_eeprom));
    }

    extern unsigned char *fnGetI2CEEPROMStart(void)
    {
        return &M24M01_eeprom[0][0];
    }

    extern unsigned long fnGetI2CEEPROMSize(void)
    {
        return (sizeof(M24M01_eeprom));
    }
#else
    typedef struct stEEPROM_24C01
    {     
        unsigned short usMaxEEPROMLength;
	    unsigned char  address;
        unsigned char  ucState;
        unsigned char  ucRW;
        unsigned char  ucInternalAddress;
        unsigned char  ucEEPROM[128];
    } EEPROM_24C01;

    static EEPROM_24C01 sim24C01 = {128, 0xa4, 0};

    typedef struct stDS3640_DATA
    {     
        TIME_BLOCK_S   bTime;
        unsigned char  ucRAM[64];
        unsigned char  ucRes[7];
        unsigned char  ucBankSelect;

        unsigned char  ucKeyPage[8][128];
    } DS3640_DATA;

    typedef struct stDS3640
    {     
	    unsigned char  address;
        unsigned char  ucState;
        unsigned char  ucRW;
        unsigned char  ucInternalPointer;

        DS3640_DATA    ucData;
    } DS3640;

    static DS3640 simDS3640 = {0xa0, 0};
#endif
/**************************************************************************/
/*        STMPE811 port expander with touch screen controller             */
/**************************************************************************/

typedef struct stSTMPE811
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucInternalPointer;

    unsigned char  CHIP_ID[2];                                           // address 0
    unsigned char  ID_VER;                                               // address 2
    unsigned char  SYS_CTRL1;                                            // address 3 - reset control
    unsigned char  SYS_CTRL2;                                            // address 4 - clock control
    unsigned char  Res0[3];
    unsigned char  SPI_CFG;                                              // address 8 - SPI configuration
    unsigned char  INT_CTRL;                                             // address 9 - interrupt control register
    unsigned char  INT_EN;                                               // address 10 - interrupt enable register
    unsigned char  INT_STA;                                              // address 11 - interrupt status register (read-only)
    unsigned char  GPIO_EN;                                              // address 12 - GPIO interrupt enable register
    unsigned char  GPIO_INT_STA;                                         // address 13 - GPIO interrupt status register (read-only)
    unsigned char  ADC_INT_EN;                                           // address 14 - ADC interrupt enable register
    unsigned char  ADC_INT_STA;                                          // address 15 - ADC interrupt status register (read-only)
    unsigned char  GPIO_SET_PIN;                                         // address 16 - GPIO set pin register
    unsigned char  GPIO_CLR_PIN;                                         // address 17 - GPIO clear pin register
    unsigned char  GPIO_MPA_STA;                                         // address 18 - GPIO monitor pin state register
    unsigned char  GPIO_DIR;                                             // address 19 - GPIO direction register
    unsigned char  GPIO_ED;                                              // address 20 - GPIO edge detect register
    unsigned char  GPIO_RE;                                              // address 21 - GPIO rising edge register
    unsigned char  GPIO_FE;                                              // address 22 - GPIO falling edge register
    unsigned char  GPIO_AF;                                              // address 23 - GPIO alternate function register
    unsigned char  Res1[8];
    unsigned char  _ADC_CTRL1;                                           // address 32 - ADC control
    unsigned char  _ADC_CTRL2;                                           // address 33 - ADC control
    unsigned char  ADC_CAPT;                                             // address 34 - ADC acquisition control
    unsigned char  Res2[13];
    unsigned char  ADC_DATA_CH0[2];                                      // address 48 - ADC channel 0
    unsigned char  ADC_DATA_CH1[2];                                      // address 50 - ADC channel 1
    unsigned char  ADC_DATA_CH2[2];                                      // address 52 - ADC channel 2
    unsigned char  ADC_DATA_CH3[2];                                      // address 54 - ADC channel 3
    unsigned char  ADC_DATA_CH4[2];                                      // address 56 - ADC channel 4
    unsigned char  ADC_DATA_CH5[2];                                      // address 58 - ADC channel 5
    unsigned char  ADC_DATA_CH6[2];                                      // address 60 - ADC channel 6
    unsigned char  ADC_DATA_CH7[2];                                      // address 62 - ADC channel 7
    unsigned char  TSC_CTRL;                                             // address 64 - 4-wire touch screen controller setup
    unsigned char  TSC_CFG;                                              // address 65 - touch screen controller configuration
    unsigned char  WDW_TR_X[2];                                          // address 66 - window setup for top right X
    unsigned char  WDW_TR_Y[2];                                          // address 68 - window setup for top right Y
    unsigned char  WDW_BL_X[2];                                          // address 70 - window setup for bottom left X
    unsigned char  WDW_BL_Y[2];                                          // address 72 - window setup for bottom left Y
    unsigned char  FIFO_TH;                                              // address 74 - FIFO level to generate interrupt
    unsigned char  FIFO_STA;                                             // address 75 - FIFO status
    unsigned char  FIFO_SIZE;                                            // address 76 - FIFO size (read-only)
    unsigned char  TSC_DATA_X[2];                                        // address 77 - touch screen controller data access (read-only)
    unsigned char  TSC_DATA_Y[2];                                        // address 79 - touch screen controller data access (read-only)
    unsigned char  TSC_DATA_Z;                                           // address 81 - touch screen controller data access (read-only)
    unsigned char  TSC_DATA_XYZ[4];                                      // address 82 - touch screen controller data access (read-only)
    unsigned char  TSC_FRACT_XYZ;                                        // address 86 - touch screen controller FRACTION_XYZ
    unsigned char  TSC_DATA;                                             // address 87 - touch screen controller data access (read-only)
    unsigned char  TSC_I_DRIVE;                                          // address 88 - touch screen controller drive I
    unsigned char  TSC_SHIELD;                                           // address 89 - touch screen controller shield
    unsigned char  Res3[6];
    unsigned char  TEMP_CTRL;                                            // address 96 - temperature sensor setup
    unsigned char  TEMP_DATA;                                            // address 97 - temperature data (read-only)
    unsigned char  TEMP_TH;                                              // address 98 - threshold for temperature controlled interrupt
} STMPE811;

static STMPE811 simSTMPE811 = {0x82, 0, 0, 0,
    0x08, 0x11,
    0x03,
    0x00, 0x0f,
    0xff, 0xff, 0xff,
    0x01, 0x00, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x9c, 0x01, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x90, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};


/**************************************************************************/
/*               SHT21 temperature and humidity sensor                    */
/**************************************************************************/

typedef struct stSHT21
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucInternalPointer;

    unsigned char  ucUserRegister;
    unsigned char  usTemperatur[3];
    unsigned char  usHumidity[3];
} SHT21;

static SHT21 simSHT21 = {0x80, 0, 0, 0, 0x02, 0x61, 0x64, 0x55, 0x63, 0x52, 0x63}; // temperatire 20.0°C, humidity 42.5%


#ifdef USE_USB_OTG_CHARGE_PUMP
/**************************************************************************/
/* MAX3353 USB OTG Charge Pump with switchable Pullup/Pulldown resistors  */
/**************************************************************************/

typedef struct stMAX3353_MEMORY
{     
    unsigned char  manufacturer_regs[4];
    unsigned char  manufacturer_id[4];
    unsigned char  res1[8];
    unsigned char  control_regs[2];
    unsigned char  res2;
    unsigned char  status_reg;
    unsigned char  interrupt_mask;
    unsigned char  interrupt_edge;
    unsigned char  interrupt_latch;
    unsigned char  res3[233];
} MAX3353_MEMORY;

// Address 0x58/59 (OTG_CHARGE_PUMP_I2C_ADD)

typedef struct stMAX3353
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucCommand;

    MAX3353_MEMORY memory;
} MAX3353;

static MAX3353 simMAX3353 = {OTG_CHARGE_PUMP_I2C_ADD, 0, 0, 0,
                             0x6a, 0x0b, 0x53, 0x33,                     // manufacturer regs
                             0x48, 0x5a, 0x42, 0x01,                     // manufaturer id
                             0,0,0,0,0,0,0,0,                            // res 1
                             0, 0x01,                                    // control regs
                             0
};
#endif


/**************************************************************************/
/*                  Wolfson WM8510 Audio Codec                            */
/**************************************************************************/

typedef struct stWM8510
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucInternalRegister;
    unsigned short usData;

    unsigned short  usRegisters[56];
} WM8510;

static WM8510 simWM8510 = {0x34, 0};


/**************************************************************************/
/*                  MAX543X Digital Potentiometer                         */
/**************************************************************************/

#define ADDRESS_MAX543X 0x28
#ifdef MAX543X_PROJECT_ADDRESS
    #undef ADDRESS_MAX543X
    #define ADDRESS_MAX543X MAX543X_PROJECT_ADDRESS                      // allow project address define
#endif

typedef struct stMAX543X
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucCommand;
    unsigned char  ucVREG;                                               // volatile register
    unsigned char  ucNVREG;                                              // non-volatile register

    unsigned char  ucWiperPosition;
} MAX543X;

static MAX543X simMAX543X = {ADDRESS_MAX543X, 0};


/**************************************************************************/
/*            NATIONAL LM80 Microprocessor System Hardware Monitor        */
/**************************************************************************/

#define ADDRESS_LM80 0x50

#define LM80_CONFIG_START          0x01
#define LM80_CONFIG_INT_ENABLE     0x02
#define LM80_CONFIG_INT_ACT_HIGH   0x04
#define LM80_CONFIG_INT_CLEAR      0x08
#define LM80_CONFIG_RESET          0x10
#define LM80_CONFIG_CHASSIS_CLEAR  0x20
#define LM80_CONFIG_GPO            0x40
#define LM80_CONFIG_INITIALIZATION 0x80


typedef struct stNATIONAL_LM80
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucInternalPointer;

    unsigned char  ucConfigurationRegister;
    unsigned char  ucInterruptStatusRegister1;
    unsigned char  ucInterruptStatusRegister2;
    unsigned char  ucInterruptMaskRegister1;
    unsigned char  ucInterruptMaskRegister2;
    unsigned char  ucFan_RST_OS_Register;
    unsigned char  ucOS_config_Temperature_Register;
    unsigned char  ucNoRegs[25];
    unsigned char  ucValueRAM[32];
} NATIONAL_LM80;

static NATIONAL_LM80 simLM80 = {ADDRESS_LM80, 0};


/**************************************************************************/
/*                    PHILIPS PCF8574 8 bit Port Expander                 */
/**************************************************************************/

#define ADDRESS_PCF8574  0x70

typedef struct stPCF8574
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucOutput;
} PCF8574;

static PCF8574 simPCF8574 = {ADDRESS_PCF8574, 0};

/**************************************************************************/
/*                    Freescale MMA8451Q 3-axis accelerometer             */
/**************************************************************************/

#define ADDRESS_MMA8451Q  0x3a                                           // assumes SA0 is '1' {SA0 = '0' would be 0x38}

typedef struct stMMA8451Q_REGS
{     
	unsigned char  ucStatus;                                             // read-only
	unsigned char  ucOut_X_MSB;                                          // read-only
    unsigned char  ucOut_X_LSB;                                          // read-only
	unsigned char  ucOut_Y_MSB;                                          // read-only
    unsigned char  ucOut_Y_LSB;                                          // read-only
	unsigned char  ucOut_Z_MSB;                                          // read-only
    unsigned char  ucOut_Z_LSB;                                          // read-only
    unsigned char  ucRes0;
    unsigned char  ucRes1;
    unsigned char  ucF_Setup;
    unsigned char  ucTrig_cfg;
    unsigned char  ucSysMod;                                             // read-only
    unsigned char  ucInt_Source;                                         // read-only
    unsigned char  ucWhoAmI;                                             // read-only
    unsigned char  ucXYZ_Data_Cfg;                                       // dynamic range settings
    unsigned char  ucHP_Filter_Cutoff;                                   // cutoff frequency defaults to 16Hz @ 800Hz
    unsigned char  ucPL_Status;                                          // read-only
    unsigned char  ucPL_CFG;
    unsigned char  ucPL_Count;
    unsigned char  ucPL_BF_Zcomp;
    unsigned char  ucP_L_Ths_Reg;
    unsigned char  ucFF_MT_Cfg;
    unsigned char  ucFF_MT_SRC;                                          // read-only
    unsigned char  ucFF_MT_THS;
    unsigned char  ucFF_MT_Count;
    unsigned char  ucRes2;
    unsigned char  ucRes3;
    unsigned char  ucRes4;
    unsigned char  ucRes5;
    unsigned char  ucTransient_CFG;
    unsigned char  ucTransient_SCR;                                      // read-only
    unsigned char  ucTransient_THS;
    unsigned char  ucTransient_Count;
    unsigned char  ucPulse_CFG;
    unsigned char  ucPulse_SRC;                                          // read-only
    unsigned char  ucPulse_ThsX;
    unsigned char  ucPulse_ThsY;
    unsigned char  ucPulse_ThsZ;
    unsigned char  ucPulse_Tmlt;
    unsigned char  ucPulse_Ltcy;
    unsigned char  ucPulse_Wind;
    unsigned char  ucAslp_Count;
    unsigned char  ucCtrl_Reg1;
    unsigned char  ucCtrl_Reg2;
    unsigned char  ucCtrl_Reg3;
    unsigned char  ucCtrl_Reg4;
    unsigned char  ucCtrl_Reg5;
    unsigned char  ucOff_X;
    unsigned char  ucOff_Y;
    unsigned char  ucOff_Z;
} MMA8451Q_REGS;

typedef struct stMMA8451Q                                                // {11}
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;

    unsigned char  ucInternalPointer;

    MMA8451Q_REGS  MMA8451Q_registers;
} MMA8451Q;

static MMA8451Q simMMA8451Q = {ADDRESS_MMA8451Q, 0, 0, 0};


/**************************************************************************/
/*                    Freescale FXOS8700 6-axis sensor                    */
/**************************************************************************/

#define ADDRESS_FXOS8700  0x3a                                           // assumes SA1 is '0' and SA0 is '1' {SA1/0 = '00' would be 0x3c} {SA1/0 = '10' would be 0x38} {SA1/0 = '11' would be 0x3e}

typedef struct stFXOS8700_REGS
{     
	unsigned char  ucStatus;                                             // read-only
	unsigned char  ucOut_X_MSB;                                          // read-only
    unsigned char  ucOut_X_LSB;                                          // read-only
	unsigned char  ucOut_Y_MSB;                                          // read-only
    unsigned char  ucOut_Y_LSB;                                          // read-only
	unsigned char  ucOut_Z_MSB;                                          // read-only
    unsigned char  ucOut_Z_LSB;                                          // read-only
    unsigned char  ucRes0;
    unsigned char  ucRes1;
    unsigned char  ucF_Setup;
    unsigned char  ucTrig_cfg;
    unsigned char  ucSysMod;                                             // read-only
    unsigned char  ucInt_Source;                                         // read-only
    unsigned char  ucWhoAmI;                                             // read-only
    unsigned char  ucXYZ_Data_Cfg;                                       // dynamic range settings
    unsigned char  ucHP_Filter_Cutoff;                                   // cutoff frequency defaults to 16Hz @ 800Hz
    unsigned char  ucPL_Status;                                          // read-only
    unsigned char  ucPL_CFG;
    unsigned char  ucPL_Count;
    unsigned char  ucPL_BF_Zcomp;
    unsigned char  ucP_L_Ths_Reg;
    unsigned char  ucFF_MT_Cfg;
    unsigned char  ucFF_MT_SRC;                                          // read-only
    unsigned char  ucFF_MT_THS;
    unsigned char  ucFF_MT_Count;
    unsigned char  ucRes2;
    unsigned char  ucRes3;
    unsigned char  ucRes4;
    unsigned char  ucRes5;
    unsigned char  ucTransient_CFG;
    unsigned char  ucTransient_SCR;                                      // read-only
    unsigned char  ucTransient_THS;
    unsigned char  ucTransient_Count;
    unsigned char  ucPulse_CFG;
    unsigned char  ucPulse_SRC;                                          // read-only
    unsigned char  ucPulse_ThsX;
    unsigned char  ucPulse_ThsY;
    unsigned char  ucPulse_ThsZ;
    unsigned char  ucPulse_Tmlt;
    unsigned char  ucPulse_Ltcy;
    unsigned char  ucPulse_Wind;
    unsigned char  ucAslp_Count;
    unsigned char  ucCtrl_Reg1;
    unsigned char  ucCtrl_Reg2;
    unsigned char  ucCtrl_Reg3;
    unsigned char  ucCtrl_Reg4;
    unsigned char  ucCtrl_Reg5;
    unsigned char  ucOff_X;
    unsigned char  ucOff_Y;
    unsigned char  ucOff_Z;
    unsigned char  ucM_Dr_Status;                                        // read-only
    unsigned char  ucM_Out_X_Msb;                                        // read-only
    unsigned char  ucM_Out_X_Lsb;                                        // read-only
    unsigned char  ucM_Out_Y_Msb;                                        // read-only
    unsigned char  ucM_Out_Y_Lsb;                                        // read-only
    unsigned char  ucM_Out_Z_Msb;                                        // read-only
    unsigned char  ucM_Out_Z_Lsb;                                        // read-only
    unsigned char  ucCmp_X_Msb;                                          // read-only
    unsigned char  ucCmp_X_Lsb;                                          // read-only
    unsigned char  ucCmp_Y_Msb;                                          // read-only
    unsigned char  ucCmp_Y_Lsb;                                          // read-only
    unsigned char  ucCmp_Z_Msb;                                          // read-only
    unsigned char  ucCmp_Z_Lsb;                                          // read-only
    unsigned char  ucM_Off_X_Msb;
    unsigned char  ucM_Off_X_Lsb;
    unsigned char  ucM_Off_Y_Msb;
    unsigned char  ucM_Off_Y_Lsb;
    unsigned char  ucM_Off_Z_Msb;
    unsigned char  ucM_Off_Z_Lsb;
    unsigned char  ucMax_X_Msb;                                          // read-only
    unsigned char  ucMax_X_Lsb;                                          // read-only
    unsigned char  ucMax_Y_Msb;                                          // read-only
    unsigned char  ucMax_Y_Lsb;                                          // read-only
    unsigned char  ucMax_Z_Msb;                                          // read-only
    unsigned char  ucMax_Z_Lsb;                                          // read-only
    unsigned char  ucMin_X_Msb;                                          // read-only
    unsigned char  ucMin_X_Lsb;                                          // read-only
    unsigned char  ucMin_Y_Msb;                                          // read-only
    unsigned char  ucMin_Y_Lsb;                                          // read-only
    unsigned char  ucMin_Z_Msb;                                          // read-only
    unsigned char  ucMin_Z_Lsb;                                          // read-only
    unsigned char  ucTemp;                                               // read-only
    unsigned char  ucM_Ths_Cfg;
    unsigned char  ucM_Ths_Src;                                          // read-only
    unsigned char  ucM_Ths_X_Msb;
    unsigned char  ucM_Ths_X_Lsb;
    unsigned char  ucM_Ths_Y_Msb;
    unsigned char  ucM_Ths_Y_Lsb;
    unsigned char  ucM_Ths_Z_Msb;
    unsigned char  ucM_Ths_Z_Lsb;
    unsigned char  ucM_Ths_Count;
    unsigned char  ucM_Ctrl_Reg1;
    unsigned char  ucM_Ctrl_Reg2;
    unsigned char  ucM_Ctrl_Reg3;
    unsigned char  ucM_Int_Src;                                          // read-only
    unsigned char  ucA_Vecm_Cfg;
    unsigned char  ucA_Vecm_Ths_Msb;
    unsigned char  ucA_Vecm_Ths_Lsb;
    unsigned char  ucA_Vecm_Cnt;
    unsigned char  ucA_Vecm_InitX_Msb;
    unsigned char  ucA_Vecm_InitX_Lsb;
    unsigned char  ucA_Vecm_InitY_Msb;
    unsigned char  ucA_Vecm_InitY_Lsb;
    unsigned char  ucA_Vecm_InitZ_Msb;
    unsigned char  ucA_Vecm_InitZ_Lsb;
    unsigned char  ucM_Vecm_Cfg;
    unsigned char  ucM_Vecm_Ths_Msb;
    unsigned char  ucM_Vecm_Ths_Lsb;
    unsigned char  ucM_Vecm_Cnt;
    unsigned char  ucM_Vecm_InitX_Msb;
    unsigned char  ucM_Vecm_InitX_Lsb;
    unsigned char  ucM_Vecm_InitY_Msb;
    unsigned char  ucM_Vecm_InitY_Lsb;
    unsigned char  ucM_Vecm_InitZ_Msb;
    unsigned char  ucM_Vecm_InitZ_Lsb;
    unsigned char  ucA_FFMT_Ths_X_Msb;
    unsigned char  ucA_FFMT_Ths_X_Lsb;
    unsigned char  ucA_FFMT_Ths_Y_Msb;
    unsigned char  ucA_FFMT_Ths_Y_Lsb;
    unsigned char  ucA_FFMT_Ths_Z_Msb;
    unsigned char  ucA_FFMT_Ths_Z_Lsb;
} FXOS8700_REGS;

typedef struct stFXOS8700                                                // {11}
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;

    unsigned char  ucInternalPointer;

    FXOS8700_REGS  FXOS8700_registers;
} FXOS8700;

static FXOS8700 simFXOS8700 = {ADDRESS_FXOS8700, 0, 0, 0};

/**************************************************************************/
/*                   Freescale MMA7660F 3-axis accelerometer              */
/**************************************************************************/

#define ADDRESS_MMA7660F  0x98

typedef struct stMMA7660F_REGS
{     
    unsigned char  ucXout;
    unsigned char  ucYout;
    unsigned char  ucZout;
    unsigned char  ucTilt;
    unsigned char  ucSrst;
    unsigned char  ucSPcnt;
    unsigned char  ucIntsu;
    unsigned char  ucMode;
    unsigned char  ucSR;
    unsigned char  ucPdet;
    unsigned char  ucPD;
} MMA7660F_REGS;

typedef struct stMMA7660F                                                // {11}
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;

    unsigned char  ucInternalPointer;

    MMA7660F_REGS  MMA7660F_registers;
} MMA7660F;

static MMA7660F simMMA7660F = {ADDRESS_MMA7660F, 0, 0, 0};

/**************************************************************************/
/*                       NXP PCF8575 16 bit Port Expander                 */
/**************************************************************************/

#if defined PCF8575_CNT
#define ADDRESS_PCF8575  0x40

typedef struct stPCF8575
{     
	unsigned char  address;
    unsigned char  ucState;
    unsigned char  ucRW;
    unsigned char  ucOutput[2];
    unsigned char  ucInput[2];
} PCF8575;

static PCF8575 simPCF8575[PCF8575_CNT] = {{ADDRESS_PCF8575, 0}};

    // Set the pin state of port expander
    //
    extern void fnSetI2CPort(int port, int iChange, unsigned long bit)
    {
        int iPortIndex = (port - _PORT_EXP_0);
        int iByteOffset = 0;
        if (iPortIndex >= PCF8575_CNT) {
            return;                                                      // invalid device
        }
        if (bit & 0xffff0000) {                                          // 16 bit supported
            return;                                                      // invalid
        }
        if (bit & 0xff00) {
            bit >>= 8;
            iByteOffset = 1;
        }
        if (!(simPCF8575[iPortIndex].ucOutput[iByteOffset] & bit)) {     // any outputs that are driving low are ignored
            return;
        }
        if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
            simPCF8575[iPortIndex].ucInput[iByteOffset] ^= bit;          // toggle the input state
        }
        else if (iChange == SET_INPUT) {
            simPCF8575[iPortIndex].ucInput[iByteOffset] |= bit;          // set the input high
        }
        else  {
            simPCF8575[iPortIndex].ucInput[iByteOffset] &= ~bit;         // set the input low
        }             
    }

    // Get the pin state of port expander
    //
    extern unsigned long fnGetExtPortState(int iExtPortReference)
    {
        int iPortIndex = (iExtPortReference - _PORT_EXP_0);
        unsigned long ulPinState = 0xffffffff;
        if (iPortIndex >= PCF8575_CNT) {
            return 0;                                                    // invalid device
        }
        ulPinState &= (simPCF8575[iPortIndex].ucOutput[0] | (simPCF8575[iPortIndex].ucOutput[1] << 8)); // any outputs that are driving '0' mean that the output is '0'
        ulPinState &= (simPCF8575[iPortIndex].ucInput[0] | (simPCF8575[iPortIndex].ucInput[1] << 8)); // any input that are set to zero when the output is set to '1' also causes a '0' to be seen at the input
        return ulPinState;
    }

    // Get the data direction of port expander pins
    //
    extern unsigned long fnGetExtPortDirection(int iExtPortReference)
    {
        int iPortIndex = (iExtPortReference - _PORT_EXP_0);
        unsigned long ulPinState = 0xffffffff;                           // all ports set to '1' are considered to be outputs
        if (iPortIndex >= PCF8575_CNT) {
            return 0;                                                    // invalid device
        }
       ulPinState &= ((simPCF8575[iPortIndex].ucInput[0] | (simPCF8575[iPortIndex].ucInput[1] << 8)) | ~(simPCF8575[iPortIndex].ucOutput[0] | (simPCF8575[iPortIndex].ucOutput[1] << 8))); // any outputs that are driving '1' but have inputs set at '0' are considered to be inputs held to '0'
       return ulPinState;
    }
#endif


/**************************************************************************/
/*                         NXP PCA9539 Port Expander                      */
/**************************************************************************/

#if defined PCA9539_CNT
    #define ADDRESS_PCA9539_0  0xe8                                      // incrementing addresses
    #define ADDRESS_PCA9539_1  0xea
    #define ADDRESS_PCA9539_2  0xec
    #define ADDRESS_PCA9539_3  0xee

    typedef struct stPCA9539_REG
    {     
	    unsigned char  ucInput0;
        unsigned char  ucInput1;
	    unsigned char  ucOutput0;
        unsigned char  ucOutput1;
	    unsigned char  ucPolInverse0;
        unsigned char  ucPolInverse1;
	    unsigned char  ucConfig0;
        unsigned char  ucConfig1;
    } PCA9539_REG;

    typedef struct stPCA9539
    {     
	    unsigned char  address[PCA9539_CNT];
        unsigned char  external_input0[PCA9539_CNT];
        unsigned char  external_input1[PCA9539_CNT];
        unsigned char  ucState[PCA9539_CNT];
        unsigned char  ucRW[PCA9539_CNT];
        unsigned char  ucAdd[PCA9539_CNT];
        PCA9539_REG    regs[PCA9539_CNT];
    } PCA9539;


    // Get the data direction of port expander pins
    //
    extern unsigned long fnGetExtPortDirection(int iExtPortReference)
    {
        switch (iExtPortReference) {
        case _PORT_EXP_0:
            return (simPCA9539.regs[0].ucConfig0);
        case _PORT_EXP_1:
            return (simPCA9539.regs[0].ucConfig1);
    #if PCA9539_CNT > 1
        case _PORT_EXP_2:
            return (simPCA9539.regs[1].ucConfig0);
        case _PORT_EXP_3:
            return (simPCA9539.regs[1].ucConfig1);
    #endif
        }
        return 0;
    }

    // Get the pin state of port expander
    //
    extern unsigned long fnGetExtPortState(int iExtPortReference)
    {
        switch (iExtPortReference) {
        case _PORT_EXP_0:
            simPCA9539.regs[0].ucInput0 = ((simPCA9539.regs[0].ucConfig0 & simPCA9539.regs[0].ucOutput0) | (~(simPCA9539.regs[0].ucConfig0) & simPCA9539.external_input0[0]));
            return ((simPCA9539.regs[0].ucConfig0 & simPCA9539.regs[0].ucOutput0) | (~(simPCA9539.regs[0].ucConfig0) & simPCA9539.external_input0[0]));
        case _PORT_EXP_1:
            simPCA9539.regs[0].ucInput1 = ((simPCA9539.regs[0].ucConfig1 & simPCA9539.regs[0].ucOutput1) | (~(simPCA9539.regs[0].ucConfig1) & simPCA9539.external_input1[0]));
            return ((simPCA9539.regs[0].ucConfig1 & simPCA9539.regs[0].ucOutput1) | (~(simPCA9539.regs[0].ucConfig1) & simPCA9539.external_input1[0]));
    #if PCA9539_CNT > 1
        case _PORT_EXP_2:
            simPCA9539.regs[1].ucInput0 = ((simPCA9539.regs[1].ucConfig0 & simPCA9539.regs[1].ucOutput0) | (~(simPCA9539.regs[1].ucConfig0) & simPCA9539.external_input0[1]));
            return ((simPCA9539.regs[1].ucConfig0 & simPCA9539.regs[1].ucOutput0) | (~(simPCA9539.regs[1].ucConfig0) & simPCA9539.external_input0[1]));
        case _PORT_EXP_3:
            simPCA9539.regs[1].ucInput1 = ((simPCA9539.regs[1].ucConfig1 & simPCA9539.regs[1].ucOutput1) | (~(simPCA9539.regs[1].ucConfig1) & simPCA9539.external_input1[1]));
            return ((simPCA9539.regs[1].ucConfig1 & simPCA9539.regs[1].ucOutput1) | (~(simPCA9539.regs[1].ucConfig1) & simPCA9539.external_input1[1]));
    #endif
        }
        return 0;
    }

    // Set the pin state of port expander
    //
    extern void fnSetI2CPort(int port, int iChange, unsigned long bit)
    {
        switch (port) {
        case _PORT_EXP_0:
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                simPCA9539.external_input0[0] ^= bit;
            }
            else if (iChange == SET_INPUT) {
                simPCA9539.external_input0[0] |= bit;                    // set the input high
            }
            else  {
                simPCA9539.external_input0[0] &= ~bit;                   // set the input low
            }             
            simPCA9539.regs[0].ucInput0 = ((simPCA9539.regs[0].ucConfig0 & simPCA9539.regs[0].ucOutput0) | (~(simPCA9539.regs[0].ucConfig0) & simPCA9539.external_input0[0]));
            break;
        case _PORT_EXP_1:
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                simPCA9539.external_input1[0] ^= bit;
            }
            else if (iChange == SET_INPUT) {
                simPCA9539.external_input1[0] |= bit;                    // set the input high
            }
            else  {
                simPCA9539.external_input1[0] &= ~bit;                   // set the input low
            }             
            simPCA9539.regs[0].ucInput1 = ((simPCA9539.regs[0].ucConfig1 & simPCA9539.regs[0].ucOutput1) | (~(simPCA9539.regs[0].ucConfig1) & simPCA9539.external_input1[0]));
            break;
    #if PCA9539_CNT > 1
        case _PORT_EXP_2:
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                simPCA9539.external_input0[1] ^= bit;
            }
            else if (iChange == SET_INPUT) {
                simPCA9539.external_input0[1] |= bit;                    // set the input high
            }
            else  {
                simPCA9539.external_input0[1] &= ~bit;                   // set the input low
            }             
            simPCA9539.regs[1].ucInput0 = ((simPCA9539.regs[1].ucConfig0 & simPCA9539.regs[1].ucOutput0) | (~(simPCA9539.regs[1].ucConfig0) & simPCA9539.external_input0[1]));
            break;
        case _PORT_EXP_3:
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                simPCA9539.external_input1[1] ^= bit;
            }
            else if (iChange == SET_INPUT) {
                simPCA9539.external_input1[1] |= bit;                    // set the input high
            }
            else  {
                simPCA9539.external_input1[1] &= ~bit;                   // set the input low
            }             
            simPCA9539.regs[1].ucInput1 = ((simPCA9539.regs[1].ucConfig1 & simPCA9539.regs[1].ucOutput1) | (~(simPCA9539.regs[1].ucConfig1) & simPCA9539.external_input1[1]));
            break;
    #endif
        }
    }
#endif

static void fnInitialiseI2CDevices(void)                                 // {10}
{
    int i = 0;

    simLM80.ucConfigurationRegister = LM80_CONFIG_INT_CLEAR;
    simLM80.ucFan_RST_OS_Register   = 0x14;
    simLM80.ucOS_config_Temperature_Register = 0x01;

    simPCF8574.ucOutput = 0xff;

#if defined PCA9539_CNT
    i = 0;
    while (i < PCA9539_CNT) {
        simPCA9539[i].address = (ADDRESS_PCA9539_0 + (i << 1));          // assume addresses increment
        simPCA9539[i].external_input0 = 0xff;                            // after a power up the ports default to inputs
        simPCA9539[i].external_input1 = 0xff;
        i++;
    }
#endif
#if defined PCF8575_CNT
    i = 0;
    while (i < PCF8575_CNT) {
        simPCF8575[i].address = (ADDRESS_PCF8575 + (i << 1));            // assume addresses increment
        simPCF8575[i].ucOutput[0] = 0xff;                                // after a power up the ports default to inputs
        simPCF8575[i].ucOutput[1] = 0xff;
        simPCF8575[i].ucInput[0] = 0xff;                                 // assume inputs are initially open/or '1'
        simPCF8575[i].ucInput[1] = 0xff;
        i++;
    }
#endif
    simMMA8451Q.MMA8451Q_registers.ucWhoAmI = 0x1a;                      // {1}
    simMMA8451Q.MMA8451Q_registers.ucPL_CFG = 0x80;
    simMMA8451Q.MMA8451Q_registers.ucPL_BF_Zcomp = 0x44;
    simMMA8451Q.MMA8451Q_registers.ucP_L_Ths_Reg = 0x84;

    simFXOS8700.FXOS8700_registers.ucWhoAmI = 0xc7;
    simFXOS8700.FXOS8700_registers.ucPL_CFG = 0x80;
    simFXOS8700.FXOS8700_registers.ucPL_BF_Zcomp = 0x44;
    simFXOS8700.FXOS8700_registers.ucP_L_Ths_Reg = 0x84;

}

// When one particular device is addressed all others are reset using this routine
//
static void fnResetOthers(unsigned char ucAddress)
{
    int i = 0;
    if (ucAddress != simDS1621.address) {
        simDS1621.ucState = 0;
    }

    if (ucAddress != simMAX543X.address) {
        simMAX543X.ucState = 0;
    }

    if (ucAddress != simDS1307.address) {
        simDS1307.ucState = 0;
    }

    if (ucAddress != simPCF2129A.address) {                              // {9}
        simPCF2129A.ucState = 0;
    }    

    if (ucAddress != simWM8510.address) {
        simWM8510.ucState = 0;
    }

#ifdef I2C_EEPROM_FILE_SYSTEM                                            // {7}
    if ((ucAddress & ~0x02) != simM24M01[0].address) {
        simM24M01[0].ucState = 0;
    }
    if ((ucAddress & ~0x02) != simM24M01[1].address) {
        simM24M01[1].ucState = 0;
    }
#else
    if (ucAddress != sim24C01.address) {
        sim24C01.ucState = 0;
    }
    if (ucAddress != simDS3640.address) {
        simDS3640.ucState = 0;
    }
#endif

    if (ucAddress != simLM80.address) {
        simLM80.ucState = 0;
    }

    if (ucAddress != simPCF8574.address) {
        simPCF8574.ucState = 0;
    }

    if (ucAddress != simLM75A.address) {                                 // {3}
        simLM75A.ucState = 0;
    }

    if (ucAddress != simSTMPE811.address) {                              // {5}
        simSTMPE811.ucState = 0;
    }

    if (ucAddress != simSHT21.address) {                                 // {6}
        simSHT21.ucState = 0;
    }
#if defined PCA9539_CNT                                                  // {8}
    i = 0;
    while (i < PCA9539_CNT) {
        if (ucAddress != simPCA9539[i].address) {                        // devices not being addressed
            simPCA9539[i].ucState = 0;
        }
        i++;
    }
#endif
#if defined PCF8575_CNT                                                  // {10}
    i = 0;
    while (i < PCF8575_CNT) {
        if (ucAddress != simPCF8575[i].address) {                        // devices not being addressed
            simPCF8575[i].ucState = 0;
        }
        i++;
    }
#endif
#ifdef USE_USB_OTG_CHARGE_PUMP
    if (ucAddress != simMAX3353.address) {
        simMAX3353.ucState = 0;
    }
#endif
    if (ucAddress != simMMA8451Q.address) {                              // {11}
        simMMA8451Q.ucState = 0;
    }
    if (ucAddress != simFXOS8700.address) {
        simFXOS8700.ucState = 0;
    }
    if (ucAddress != simMMA7660F.address) {
        simMMA7660F.ucState = 0;
    }
}

// Increment the internal register pointer in the MMA8451Q
//
static void fnIncMMA8451Q_pointer(MMA8451Q *ptr_simMMA8451Q)
{
    if ((ptr_simMMA8451Q->ucInternalPointer == 0) || ((ptr_simMMA8451Q->ucInternalPointer >= 0x09) && (ptr_simMMA8451Q->ucInternalPointer <= 0x30))) { // standard auto-increment in this range
        ptr_simMMA8451Q->ucInternalPointer++;
    }
    else if (ptr_simMMA8451Q->ucInternalPointer == 0x31) {
        ptr_simMMA8451Q->ucInternalPointer = 0x0d;                       // wrap from 0x32 to 0x0d
    }
    else if (ptr_simMMA8451Q->ucInternalPointer == 1) {
        if ((ptr_simMMA8451Q->MMA8451Q_registers.ucF_Setup >> 6) == 0) { // only increment when F_MODE is 0
            if (ptr_simMMA8451Q->MMA8451Q_registers.ucCtrl_Reg1 & 0x02) {// if F_READ increment two
                ptr_simMMA8451Q->ucInternalPointer += 2;
            }
            else {
                ptr_simMMA8451Q->ucInternalPointer++;
            }
        }
    }
    else if (ptr_simMMA8451Q->ucInternalPointer == 2) {
        if (ptr_simMMA8451Q->MMA8451Q_registers.ucCtrl_Reg1 & 0x02) {    // if F_READ go back to 0
            ptr_simMMA8451Q->ucInternalPointer = 0;
        }
        else {
            ptr_simMMA8451Q->ucInternalPointer++;
        }
    }
    else if (ptr_simMMA8451Q->ucInternalPointer == 3) {
        if (ptr_simMMA8451Q->MMA8451Q_registers.ucCtrl_Reg1 & 0x02) {    // if F_READ
            if ((ptr_simMMA8451Q->MMA8451Q_registers.ucF_Setup >> 6) == 0) {
                ptr_simMMA8451Q->ucInternalPointer += 2;
            }
            else {
                ptr_simMMA8451Q->ucInternalPointer = 0;
            }
        }
        else {
            ptr_simMMA8451Q->ucInternalPointer++;
        }
    }
    else if ((ptr_simMMA8451Q->ucInternalPointer == 4) || (ptr_simMMA8451Q->ucInternalPointer == 5)) {
        if (ptr_simMMA8451Q->MMA8451Q_registers.ucCtrl_Reg1 & 0x02) {    // if F_READ
            ptr_simMMA8451Q->ucInternalPointer = 0;
        }
        else {
            ptr_simMMA8451Q->ucInternalPointer++;
        }
    }
    else if (ptr_simMMA8451Q->ucInternalPointer == 6) {
        ptr_simMMA8451Q->ucInternalPointer = 0;
    }
    else {
        _EXCEPTION("To do");
    }
}

// Increment the internal register pointer in the FXOS8700
//
static void fnIncFXOS8700_pointer(FXOS8700 *ptr_simFXOS8700)
{
    if ((ptr_simFXOS8700->ucInternalPointer == 0) || ((ptr_simFXOS8700->ucInternalPointer >= 0x09) && (ptr_simFXOS8700->ucInternalPointer <= 0x78))) { // standard auto-increment in this range
        ptr_simFXOS8700->ucInternalPointer++;
    }
    else if (ptr_simFXOS8700->ucInternalPointer == 0x31) {
        ptr_simFXOS8700->ucInternalPointer = 0x0d;                       // wrap from 0x32 to 0x0d
    }
    else if (ptr_simFXOS8700->ucInternalPointer == 1) {
        if ((ptr_simFXOS8700->FXOS8700_registers.ucF_Setup >> 6) == 0) { // only increment when F_MODE is 0
            if (ptr_simFXOS8700->FXOS8700_registers.ucCtrl_Reg1 & 0x02) {// if F_READ increment two
                ptr_simFXOS8700->ucInternalPointer += 2;
            }
            else {
                ptr_simFXOS8700->ucInternalPointer++;
            }
        }
    }
    else if (ptr_simFXOS8700->ucInternalPointer == 2) {
        if (ptr_simFXOS8700->FXOS8700_registers.ucCtrl_Reg1 & 0x02) {    // if F_READ go back to 0
            ptr_simFXOS8700->ucInternalPointer = 0;
        }
        else {
            ptr_simFXOS8700->ucInternalPointer++;
        }
    }
    else if (ptr_simFXOS8700->ucInternalPointer == 3) {
        if (ptr_simFXOS8700->FXOS8700_registers.ucCtrl_Reg1 & 0x02) {    // if F_READ
            if ((ptr_simFXOS8700->FXOS8700_registers.ucF_Setup >> 6) == 0) {
                ptr_simFXOS8700->ucInternalPointer += 2;
            }
            else {
                ptr_simFXOS8700->ucInternalPointer = 0;
            }
        }
        else {
            ptr_simFXOS8700->ucInternalPointer++;
        }
    }
    else if ((ptr_simFXOS8700->ucInternalPointer == 4) || (ptr_simFXOS8700->ucInternalPointer == 5)) {
        if (ptr_simFXOS8700->FXOS8700_registers.ucCtrl_Reg1 & 0x02) {    // if F_READ
            ptr_simFXOS8700->ucInternalPointer = 0;
        }
        else {
            ptr_simFXOS8700->ucInternalPointer++;
        }
    }
    else if (ptr_simFXOS8700->ucInternalPointer == 6) {
        ptr_simFXOS8700->ucInternalPointer = 0;
    }
    else {
  //    _EXCEPTION("To do");
    }
}

// Initialise the simulated RTC with security memory from PC's clock
//
static void fnInitTimeDS1307(char *argv[])
{
    unsigned short *ptrShort = (unsigned short *)*argv;
    unsigned char ucTens;
    unsigned char ucUnits;

    simDS1307.bTime.ucYear = (unsigned char)(*ptrShort++ - 2000);
    simDS1307.bTime.ucMonth = (unsigned char)(*ptrShort++);
    simDS1307.bTime.ucDayOfWeek = (unsigned char)(*ptrShort++);
    simDS1307.bTime.ucDayOfMonth = (unsigned char)(*ptrShort++);
    simDS1307.bTime.ucHours = (unsigned char)(*ptrShort++);
    simDS1307.bTime.ucMinutes = (unsigned char)(*ptrShort++);
    simDS1307.bTime.ucSeconds = (unsigned char)(*ptrShort++);

    simPCF2129A.bTime.ucSeconds = simDS1307.bTime.ucSeconds;             // {9}
    simPCF2129A.bTime.ucMinutes = simDS1307.bTime.ucMinutes;
    simPCF2129A.bTime.ucHours = simDS1307.bTime.ucHours;                 // 24 hour mode
    simPCF2129A.bTime.ucDayOfMonth = simDS1307.bTime.ucDayOfMonth;
    simPCF2129A.bTime.ucDayOfWeek = simDS1307.bTime.ucDayOfWeek;
    simPCF2129A.bTime.ucMonth = simDS1307.bTime.ucMonth;
    simPCF2129A.bTime.ucYears = simDS1307.bTime.ucYear;

    // We have to convert to the correct format for this device here
    //
    ucTens = (simDS1307.bTime.ucSeconds/10);
    ucUnits = (simDS1307.bTime.ucSeconds - (10 * ucTens));
    simDS1307.bTime.ucSeconds = ((ucTens << 4) + ucUnits);

    ucTens = (simDS1307.bTime.ucMinutes/10);
    ucUnits = (simDS1307.bTime.ucMinutes - (10 * ucTens));
    simDS1307.bTime.ucMinutes = ((ucTens << 4) + ucUnits);

    ucTens = (simDS1307.bTime.ucHours/10);
    ucUnits = (simDS1307.bTime.ucHours - (10 * ucTens));
    simDS1307.bTime.ucHours = ((ucTens << 4) + ucUnits);

    ucTens = (simDS1307.bTime.ucDayOfMonth/10);
    ucUnits = (simDS1307.bTime.ucDayOfMonth - (10 * ucTens));
    simDS1307.bTime.ucDayOfMonth = ((ucTens << 4) + ucUnits);

    ucTens = (simDS1307.bTime.ucMonth/10);
    ucUnits = (simDS1307.bTime.ucMonth - (10 * ucTens));
    simDS1307.bTime.ucMonth = ((ucTens << 4) + ucUnits);

    ucTens = (simDS1307.bTime.ucYear/10);
    ucUnits = (simDS1307.bTime.ucYear - (10 * ucTens));
    simDS1307.bTime.ucYear = ((ucTens << 4) + ucUnits);
}


// Initialise the simulated RTC from PC's clock
//
static void fnInitTimeDS3640(char *argv[])
{
#ifndef I2C_EEPROM_FILE_SYSTEM                                           // {7}
    unsigned short *ptrShort = (unsigned short *)*argv;
    unsigned char ucTens;
    unsigned char ucUnits;
    simDS3640.ucData.bTime.ucYear = (unsigned char)(*ptrShort++ - 2000);
    simDS3640.ucData.bTime.ucMonth = (unsigned char)(*ptrShort++);
    simDS3640.ucData.bTime.ucDayOfWeek = (unsigned char)(*ptrShort++);
    simDS3640.ucData.bTime.ucDayOfMonth = (unsigned char)(*ptrShort++);
    simDS3640.ucData.bTime.ucHours = (unsigned char)(*ptrShort++);
    simDS3640.ucData.bTime.ucMinutes = (unsigned char)(*ptrShort++);
    simDS3640.ucData.bTime.ucSeconds = (unsigned char)(*ptrShort++);

    // We have to convert to the correct format for this device here
    //
    ucTens = (simDS3640.ucData.bTime.ucSeconds/10);
    ucUnits = (simDS3640.ucData.bTime.ucSeconds - (10 * ucTens));
    simDS3640.ucData.bTime.ucSeconds = ((ucTens << 4) + ucUnits);

    ucTens = (simDS3640.ucData.bTime.ucMinutes/10);
    ucUnits = (simDS3640.ucData.bTime.ucMinutes - (10 * ucTens));
    simDS3640.ucData.bTime.ucMinutes = ((ucTens << 4) + ucUnits);

    ucTens = (simDS3640.ucData.bTime.ucHours/10);
    ucUnits = (simDS3640.ucData.bTime.ucHours - (10 * ucTens));
    simDS3640.ucData.bTime.ucHours = ((ucTens << 4) + ucUnits);

    ucTens = (simDS3640.ucData.bTime.ucDayOfMonth/10);
    ucUnits = (simDS3640.ucData.bTime.ucDayOfMonth - (10 * ucTens));
    simDS3640.ucData.bTime.ucDayOfMonth = ((ucTens<<4) + ucUnits);

    ucTens = (simDS3640.ucData.bTime.ucMonth/10);
    ucUnits = (simDS3640.ucData.bTime.ucMonth - (10 * ucTens));
    simDS3640.ucData.bTime.ucMonth = ((ucTens<<4) + ucUnits);

    ucTens = (simDS3640.ucData.bTime.ucYear/10);
    ucUnits = (simDS3640.ucData.bTime.ucYear - (10 * ucTens));
    simDS3640.ucData.bTime.ucYear = ((ucTens<<4) + ucUnits);

    simDS3640.ucData.bTime.ucFilter = 0x54;
    simDS3640.ucData.bTime.ucControl[0] = 0xcf;
    simDS3640.ucData.bTime.ucControl[1] = 0xd8;

    simDS3640.ucData.bTime.ucHighTemp = 0x30;
    simDS3640.ucData.bTime.ucLowTemp = 0xfc;
    simDS3640.ucData.bTime.ucDeltaTemp = 0x0a;

    // Add a serial number
    //
    simDS3640.ucData.bTime.ucSerialNumber[0] = 0x83;                     // {2} chip identifier byte
    simDS3640.ucData.bTime.ucSerialNumber[1] = 0x22;
    simDS3640.ucData.bTime.ucSerialNumber[2] = 0x33;
    simDS3640.ucData.bTime.ucSerialNumber[3] = 0x44;
    simDS3640.ucData.bTime.ucSerialNumber[4] = 0x55;
    simDS3640.ucData.bTime.ucSerialNumber[5] = 0x66;
    simDS3640.ucData.bTime.ucSerialNumber[6] = 0x77;
    simDS3640.ucData.bTime.ucSerialNumber[7] = 0x88;
#endif
}



// Set RTC to system time on startup
//
extern void fnInitTime(char *argv[])
{
#if defined SUPPORT_RTC || defined SUPPORT_SW_RTC                        // {1}
    extern void fnInitInternalRTC(char *argv[]);
    fnInitInternalRTC(argv);                                             // allow initialisation of an internal RTC
#endif
    fnInitTimeDS1307(argv);
    fnInitTimeDS3640(argv);

    fnInitialiseI2CDevices();                                            // {10} initialise also some register values at reset
}


extern int fnCheckRTC(void)
{
    if (((simDS1307.bTime.ucSeconds & 0x80) == 0) && ((simDS1307.bTime.ucControl & 0x10) != 0)) {
        if ((simDS1307.bTime.ucControl & 0x03) != 0) {
            return 1;                                                    // fastest rate possible
        }
        else {
            static unsigned char ucRTC_Output = 0;
            if (++ucRTC_Output >= (500000/(TICK_RESOLUTION))) {
                ucRTC_Output = 0;
                return 1;                                                // 1 Hz TICK rate
            }
        }
    }
    if ((simPCF2129A.bTime.ucControl1 & 0x20) == 0) {                    // {9} if the RTC is not stopped
        if (simPCF2129A.bTime.ucControl1 & 0x01) {                       // if seconds interrupt is enabled
            static unsigned char ucRTC_Output = 0;
            if (++ucRTC_Output >= (500000/(TICK_RESOLUTION))) {
                ucRTC_Output = 0;
                return 1;                                                // 1 Hz TICK rate
            }
        }
    }
    return 0;
}

unsigned char fnSimI2C_devices(unsigned char ucType, unsigned char ucData)
{
    switch (ucType) {
    case I2C_ADDRESS:
        if ((ucData & ~0x01) == simDS1307.address) {                     // DS1307 is being addressed
            simDS1307.ucState = 1;
            simDS1307.ucRW = (ucData & 0x01);
        }
        else if ((ucData & ~0x01) == simFXOS8700.address) {              // {11} 6-axis sensor is being addressed
            simFXOS8700.ucState = 1;
            simFXOS8700.ucRW = (ucData & 0x01);                          // mark whether read or write
        }
        else if ((ucData & ~0x01) == simMMA8451Q.address) {              // {11} 3-axis accelerometer is being addressed
            simMMA8451Q.ucState = 1;
            simMMA8451Q.ucRW = (ucData & 0x01);                          // mark whether read or write
        }
        else if ((ucData & ~0x01) == simMMA7660F.address) {              // {11} 3-axis accelerometer is being addressed
            simMMA7660F.ucState = 1;
            simMMA7660F.ucRW = (ucData & 0x01);                          // mark whether read or write
        }
        else if ((ucData & ~0x01) == simPCF2129A.address) {              // {9} PCF2129A is being addressed
            simPCF2129A.ucState = 1;
            simPCF2129A.ucRW = (ucData & 0x01);
        }
        else if ((ucData & ~0x01) == simDS1621.address) {
            simDS1621.ucState = 1;
            simDS1621.ucRW = (ucData & 0x01);
            if ((simDS1621.ucCommand == 0xaa) & (ucData & 0x01)) {       // start reading temperature
                simDS1621.ucState = 2;                                   // 2 bytes to be read
            }
        }
        else if ((ucData & ~0x01) == simWM8510.address) {
            simWM8510.ucState = 1;
            simWM8510.ucRW = (ucData & 0x01);
        }
#ifdef I2C_EEPROM_FILE_SYSTEM                                            // {7}
        else if ((ucData & ~0x03) == simM24M01[0].address) {
            simM24M01[0].ucRW = (ucData & 0x01);                         // being read or written
            if ((simM24M01[0].ucRW) && (simM24M01[0].ucState >= 3)) {    // repeated start after setting address and moving to sequential read mode
                simM24M01[0].ucState = 4;
            }
            else {
                simM24M01[0].ucState = 1;                                // being addressed
                simM24M01[0].ulInternalPointer = ((unsigned long)(ucData & 0x02) << 15); // extended address bit
            }
        }
        else if ((ucData & ~0x03) == simM24M01[1].address) {
            simM24M01[1].ucRW = (ucData & 0x01);                         // being read or written
            if ((simM24M01[1].ucRW) && (simM24M01[1].ucState >= 3)) {    // repeated start after setting address and moving to sequential read mode
                simM24M01[1].ucState = 4;
            }
            else {
                simM24M01[1].ucState = 1;                                // being addressed
                simM24M01[1].ulInternalPointer = ((unsigned long)(ucData & 0x02) << 15); // extended address bit
            }
        }
#else
        else if ((ucData & ~0x01) == sim24C01.address) {
            sim24C01.ucState++;                                          // being addressed
            sim24C01.ucRW = (ucData & 0x01);                             // being read or written
            if (sim24C01.ucState >= 4) {
                sim24C01.ucState = 1;
            }
        }
        else if ((ucData & ~0x01) == simDS3640.address) {
            simDS3640.ucState++;                                         // being addressed
            simDS3640.ucRW = (ucData & 0x01);                            // being read or written
            if (simDS3640.ucState >= 4) {
                simDS3640.ucState = 1;
            }
        }
#endif
        else if ((ucData & ~0x01) == simMAX543X.address) {               // being addressed
            simMAX543X.ucState = 1;
            simMAX543X.ucRW = (ucData & 0x01);
        }        
        else if ((ucData & ~0x01) == simLM80.address) {                  // being addressed
            simLM80.ucState = 1;
            simLM80.ucRW = (ucData & 0x01);
        }
        else if ((ucData & ~0x01) == simPCF8574.address) {               // being addressed
            simPCF8574.ucState = 1;
            simPCF8574.ucRW = (ucData & 0x01);
        }
        else if ((ucData & ~0x01) == simLM75A.address) {                 // {3} being addressed
            simLM75A.ucState = 1;
            simLM75A.ucRW = (ucData & 0x01);
        }
        else if ((ucData & ~0x01) == simSTMPE811.address) {              // {5} being addressed
            simSTMPE811.ucState = 1;
            simSTMPE811.ucRW = (ucData & 0x01);
        }
        else if ((ucData & ~0x01) == simSHT21.address) {                 // {6} being addressed
            simSHT21.ucState = 1;
            simSHT21.ucRW = (ucData & 0x01);
        }
#if defined PCA9539_CNT
        else if ((ucData & ~0x01) == simPCA9539.address[0]) {            // {8}
            simPCA9539.ucState[0] = 1;
            simPCA9539.ucRW[0] = (ucData & 0x01);
        }
    #if PCA9539_CNT > 1
        else if ((ucData & ~0x01) == simPCA9539.address[1]) {
            simPCA9539.ucState[1] = 1;
            simPCA9539.ucRW[1] = (ucData & 0x01);
        }
    #endif
#endif
#ifdef USE_USB_OTG_CHARGE_PUMP
        else if ((ucData & ~0x01) == simMAX3353.address) {               // being addressed
            simMAX3353.ucState = 1;
            simMAX3353.ucRW = (ucData & 0x01);
        }
#endif
        else {
            int i = 0;
#if defined PCF8575_CNT
            i = 0;
            while (i < PCF8575_CNT) {
                if ((ucData & ~0x01) == simPCF8575[i].address) {         // check each possible address
                    simPCF8575[i].ucState = 1;                           // this device is being addressed
                    simPCF8575[i].ucRW = (ucData & 0x01);                // note that access type
                    break;
                }
                i++;
            }
#endif
        }
        fnResetOthers((unsigned char)(ucData & ~0x01));
        break;

    case I2C_TX_DATA:
        if (simDS1307.ucState == 1) {                                    // DS1307 is being written to
            simDS1307.ucInternalPointer = ucData;
            simDS1307.ucState++;
        }     
        else if (simDS1307.ucState > 1) {                                // date being written
            unsigned char *ptr = (unsigned char *)&simDS1307.bTime;
            ptr += simDS1307.ucInternalPointer++;
            *ptr = ucData;                                               // set the data
        }
        else if (simMMA8451Q.ucState == 1) {                             // {11} 3-axis accelerometer is being written to
            simMMA8451Q.ucInternalPointer = ucData;                      // write the internal register pointer
            simMMA8451Q.ucState++;
        } 
        else if (simMMA8451Q.ucState > 1) {                              // {11} 3-axis accelerometer data write
            unsigned char *ptr = (unsigned char *)&simMMA8451Q.MMA8451Q_registers;
            ptr += simMMA8451Q.ucInternalPointer;                        // move to the register to write to
            *ptr = ucData;                                               // set the data
            fnIncMMA8451Q_pointer(&simMMA8451Q);
        }
        else if (simFXOS8700.ucState == 1) {                             // {11} 3-axis sensor is being written to
            simFXOS8700.ucInternalPointer = ucData;                      // write the internal register pointer
            simFXOS8700.ucState++;
        } 
        else if (simFXOS8700.ucState > 1) {                              // {11} 3-axis sensor data write
            unsigned char *ptr = (unsigned char *)&simFXOS8700.FXOS8700_registers;
            ptr += simFXOS8700.ucInternalPointer;                        // move to the register to write to
            *ptr = ucData;                                               // set the data
            fnIncFXOS8700_pointer(&simFXOS8700);
        }
        else if (simMMA7660F.ucState == 1) {                             // {11} 3-axis accelerometer is being written to
            simMMA7660F.ucInternalPointer = ucData;                      // write the internal register pointer
            simMMA7660F.ucState++;
        } 
        else if (simMMA7660F.ucState > 1) {                              // {11} 3-axis accelerometer data write
            unsigned char *ptr = (unsigned char *)&simMMA7660F.MMA7660F_registers;
            ptr += simMMA7660F.ucInternalPointer++;                      // move to the register to write to
            *ptr = ucData;                                               // set the data
        }  
        else if (simPCF2129A.ucState == 1) {                             // {9} PCF2129A is being written to
            simPCF2129A.ucInternalPointer = ucData;
            simPCF2129A.ucState++;
        }        
        else if (simPCF2129A.ucState > 1) {                              // date being written
            unsigned char *ptr = (unsigned char *)&simPCF2129A.bTime;
            ptr += simPCF2129A.ucInternalPointer++;
            *ptr = ucData;                                               // set the data
        }
        else if (simDS1621.ucState != 0) {                               // DS1307 is being written to
            switch (simDS1621.ucCommand) {                               // {4}
            case 0xac:
                simDS1621.ucConfigReg = ucData;                          // write configuration
                simDS1621.ucCommand = 0;
                simDS1621.ucState = 0;
                break;
            default:
                simDS1621.ucCommand = ucData;                            // commands to the temperature sensor
                if (ucData == 0xee) {                                    // start conversion
                    simDS1621.ucState = 0;
                }
                else if (ucData == 0xaa) {                               // command to read temperature
                    simDS1621.ucState = 0;
                }
                break;
            }
        }
        else if (simLM75A.ucState >= 1) {                                // {3} LM75A is being written to
            switch (simLM75A.ucState) {
            case 1:
                simLM75A.ucState++;
                simLM75A.ucInternalPointer = ucData;                     // commands to the temperature sensor
                break;
            case 2:
                if (simLM75A.ucInternalPointer == 1) {                   // write config
                    simLM75A.ucConfig = ucData;
                }
                else if (simLM75A.ucInternalPointer == 2) {              // write hysteresis
                    simLM75A.ucHyst[0] = ucData;
                    simLM75A.ucState++;
                }
                else if (simLM75A.ucInternalPointer == 3) {              // write offset
                    simLM75A.ucOS[0] = ucData;
                    simLM75A.ucState++;
                }
                break;
            case 3:
                if (simLM75A.ucInternalPointer == 2) {                   // write hysteresis
                    simLM75A.ucHyst[1] = ucData;
                }
                else if (simLM75A.ucInternalPointer == 3) {              // write offset
                    simLM75A.ucOS[1] = ucData;
                }
                break;
            default:
                break;
            }
        }
        else if (simSHT21.ucState >= 1) {                                // {6} SHT21 being written to
            simSHT21.ucState++;
            simSHT21.ucInternalPointer = ucData;                         // commands to the temperature/humidity sensor
        }
        else if (simWM8510.ucState == 1) {                               // WM8510 is being written to (command byte 1)
            simWM8510.ucInternalRegister = (ucData>>1);                  // register to be written
            simWM8510.usData = (ucData<<8);                              // save 9th bit
            simWM8510.usData &= 0x100;
            simWM8510.ucState++;
        }
        else if (simWM8510.ucState == 2) {                               // WM8510 is being written to (command byte 2)
            simWM8510.usData |= (ucData);                                // complete register contends received
            simWM8510.usRegisters[simWM8510.ucInternalRegister] = simWM8510.usData;
            simWM8510.ucState = 0;
        }
#ifdef I2C_EEPROM_FILE_SYSTEM                                            // {7}
        else if ((simM24M01[0].ucState == 3) && (simM24M01[0].ucRW == 0)) { // writing
            M24M01_eeprom[0][simM24M01[0].ulInternalPointer++] = ucData;
            if ((simM24M01[0].ulInternalPointer % 128) == 0) {           // handle page write
                simM24M01[0].ulInternalPointer &= ~0x3f;
            }
        }
        else if ((simM24M01[1].ucState == 3) && (simM24M01[1].ucRW == 0)) { // writing
            M24M01_eeprom[1][simM24M01[1].ulInternalPointer++] = ucData;
            if ((simM24M01[1].ulInternalPointer % 128) == 0) {           // handle page write
                simM24M01[1].ulInternalPointer &= ~0x3f;
            }
        }
        else if ((simM24M01[1].ucState == 1) && (simM24M01[1].ucRW == 0)) { // collecting address
            simM24M01[1].ulInternalPointer += ((unsigned long)ucData << 8); // set internal pointer
            simM24M01[1].ucState++;
        }
        else if ((simM24M01[0].ucState == 1) && (simM24M01[0].ucRW == 0)) { // collecting address
            simM24M01[0].ulInternalPointer += ((unsigned long)ucData << 8); // set internal pointer
            simM24M01[0].ucState++;
        }
        else if ((simM24M01[1].ucState == 1) && (simM24M01[1].ucRW == 0)) { // collecting address
            simM24M01[1].ulInternalPointer += ((unsigned long)ucData << 8); // set internal pointer
            simM24M01[1].ucState++;
        }
        else if ((simM24M01[0].ucState == 2) && (simM24M01[0].ucRW == 0)) { // collecting address
            simM24M01[0].ulInternalPointer += ucData;                    // set internal pointer
            simM24M01[0].ucState++;
        }
        else if ((simM24M01[1].ucState == 2) && (simM24M01[1].ucRW == 0)) { // collecting address
            simM24M01[1].ulInternalPointer += ucData;                    // set internal pointer
            simM24M01[1].ucState++;
        }
#else
        else if ((sim24C01.ucState == 1) && (sim24C01.ucRW == 0)) {      // being addressed for write
            sim24C01.ucInternalAddress = ucData;                         // set internal pointer
            sim24C01.ucState++;
        }
        else if ((sim24C01.ucState >= 2) && (sim24C01.ucRW == 0)) {      // being addressed for write
            sim24C01.ucEEPROM[sim24C01.ucInternalAddress++] = ucData;    // write new data to EEPROM
            if (sim24C01.ucInternalAddress >= sim24C01.usMaxEEPROMLength) {
                sim24C01.ucInternalAddress = 0;
            }
            sim24C01.ucState++;
        }
        else if ((simDS3640.ucState == 1) && (simDS3640.ucRW == 0)) {    // being addressed for write
            simDS3640.ucInternalPointer = ucData;                        // set internal pointer
            simDS3640.ucState++;
        }
#endif
        else if ((simSTMPE811.ucState == 1) && (simSTMPE811.ucRW == 0)) {// {5} being addressed for write
            simSTMPE811.ucInternalPointer = ucData;                      // set internal pointer
            simSTMPE811.ucState++;
        }
        else if ((simSTMPE811.ucState == 2) && (simSTMPE811.ucRW == 0)) {// {5} being addressed for register write
            unsigned char *ptrData = (unsigned char *)&simSTMPE811.CHIP_ID[0];
            ptrData += simSTMPE811.ucInternalPointer;
            switch (simSTMPE811.ucInternalPointer) {
            case 0x0b:
                if (ucData & 0x01) {
#if defined _STM32 && defined MB785_GLCD_MODE
                    RESET_IRQ_LINE();                                    // set interrupt line back to '1'
#endif
                }
                *ptrData = ucData;
                break;
            default:
                *ptrData = ucData;
                break;
            }
            simSTMPE811.ucInternalPointer++;
        }
#ifndef I2C_EEPROM_FILE_SYSTEM                                           // {7}
        else if ((simDS3640.ucState >= 2) && (simDS3640.ucRW == 0)) {    // being addressed for write
            unsigned char *ptrData = (unsigned char *)&simDS3640.ucData;
            ptrData += simDS3640.ucInternalPointer;
            if (simDS3640.ucInternalPointer >= 0x80) {                   // in paged memory
                ptrData += 128*simDS3640.ucData.ucBankSelect;
            }
            simDS3640.ucInternalPointer++;
            *ptrData = ucData;                                           // write new data to device

            simDS3640.ucState++;
        }
#endif
        else if ((simMAX543X.ucState == 1) && (simMAX543X.ucRW == 0)) {  // being addressed for write
            simMAX543X.ucCommand = ucData;                               // set command
            simMAX543X.ucState++;
        }
        else if ((simMAX543X.ucState == 2) && (simMAX543X.ucRW == 0)) {  // being addressed for write
            switch (simMAX543X.ucCommand) {
                case 0x11:                                               // command VREG
                    simMAX543X.ucVREG = (ucData & 0xf8);
                    simMAX543X.ucWiperPosition = (simMAX543X.ucVREG >> 3);
                    break;

                case 0x21:                                               // command NVREG
                    simMAX543X.ucNVREG = (ucData & 0xf8);
                    break;

                case 0x61:                                               // command NVREGxVREG
                    simMAX543X.ucVREG = simMAX543X.ucNVREG;
                    simMAX543X.ucWiperPosition = (simMAX543X.ucVREG >> 3);
                    break;

                case 0x51:                                               // command VREGxNVREG
                    simMAX543X.ucNVREG = simMAX543X.ucVREG;
                    break;
            }
            simMAX543X.ucState = 0;;
        }
        else if ((simLM80.ucState > 0) && (simLM80.ucRW == 0)) {         // being addressed for write
            if (simLM80.ucState == 1) {
                simLM80.ucInternalPointer = ucData;                      // set internal pointer
                simLM80.ucState++;
            }
            else if (simLM80.ucState == 2) {
                unsigned char *ptr = &simLM80.ucConfigurationRegister;
                simLM80.ucState = 0;
                if (simLM80.ucInternalPointer <= 0x3f) {
                    ptr += simLM80.ucInternalPointer;
                    *ptr = ucData;
                }
            }
        }
        else if ((simPCF8574.ucState > 0) && (simPCF8574.ucRW == 0)) {   // being addressed for write
            simPCF8574.ucOutput = ucData;
            simPCF8574.ucState = 0;
        }
#if defined PCA9539_CNT
        else if ((simPCA9539.ucState[0] >= 1) && (simPCA9539.ucRW[0] == 0)) { // {8} setting internal register
            if (simPCA9539.ucState[0] == 1) {
                simPCA9539.ucAdd[0] = (ucData & 0x07);                   // set the address
                simPCA9539.ucState[0]++;                                 // next access is to/from the register
            }
            else {
                unsigned char *ptrReg = &simPCA9539.regs[0].ucInput0;
                ptrReg += simPCA9539.ucAdd[0];
                *ptrReg = ucData;                                        // set register
            }
        }
    #if PCA9539_CNT > 1
        else if ((simPCA9539.ucState[1] >= 1) && (simPCA9539.ucRW[1] == 0)) { // setting internal register
            if (simPCA9539.ucState[1] == 1) {
                simPCA9539.ucAdd[1] = (ucData & 0x07);                   // set the address
                simPCA9539.ucState[1]++;                                 // next access is to/from the register
            }
            else {
                unsigned char *ptrReg = &simPCA9539.regs[1].ucInput0;
                ptrReg += simPCA9539.ucAdd[1];
                *ptrReg = ucData;                                        // set register
            }
        }
    #endif
#endif
#ifdef USE_USB_OTG_CHARGE_PUMP
        else if ((simMAX3353.ucState > 0) && (simMAX3353.ucRW == 0)) {   // being addressed for write
            if (simMAX3353.ucState == 1) {
                simMAX3353.ucCommand = ucData;                           // set the control register being written
                simMAX3353.ucState++;
            }
            else if (simMAX3353.ucState = 2) {
                unsigned char *ptr = (unsigned char *)&simMAX3353.memory;
                ptr += simMAX3353.ucCommand;
                *ptr = ucData;
                simMAX3353.ucState = 0;
            }
        }
#endif
        else {
            int i = 0;
#if defined PCF8575_CNT
            i = 0;
            while (i < PCF8575_CNT) {
                if ((simPCF8575[i].ucState >= 1) && (!(simPCF8575[i].ucRW & 0x01))) { // device being addressed as write
                    simPCF8575[i].ucOutput[simPCF8575[i].ucState - 1] = ucData; // write the data
                    if (++simPCF8575[i].ucState > 2) {                   // two output bytes
                        simPCF8575[i].ucState = 1;
                    }
                    break;
                }
                i++;
            }
#endif
        }
        break;

    case I2C_RX_DATA:
        if (simDS1307.ucRW && (simDS1307.ucState == 1)) {
            unsigned char *ptr = (unsigned char *)&simDS1307.bTime;
            ptr += simDS1307.ucInternalPointer++;
            return (*ptr);
        }
        else if (simPCF2129A.ucRW && (simPCF2129A.ucState == 1)) {       // {9}
            unsigned char *ptr = (unsigned char *)&simPCF2129A.bTime;
            ptr += simPCF2129A.ucInternalPointer++;
            return (*ptr);
        }
        else if (simMMA8451Q.ucRW && (simMMA8451Q.ucState == 1)) {       // {11}
            unsigned char ucRegisterValue;
            unsigned char *ptr = (unsigned char *)&simMMA8451Q.MMA8451Q_registers;
            ptr += simMMA8451Q.ucInternalPointer;                        // move to the present internal register
            ucRegisterValue = *ptr;
            fnIncMMA8451Q_pointer(&simMMA8451Q);
            return (ucRegisterValue);                                    // return the register value
        }
        else if (simFXOS8700.ucRW && (simFXOS8700.ucState == 1)) {       // {11}
            unsigned char ucRegisterValue;
            unsigned char *ptr = (unsigned char *)&simFXOS8700.FXOS8700_registers;
            ptr += simFXOS8700.ucInternalPointer;                        // move to the present internal register
            ucRegisterValue = *ptr;
            fnIncFXOS8700_pointer(&simFXOS8700);
            return (ucRegisterValue);                                    // return the register value
        }
        else if (simMMA7660F.ucRW && (simMMA7660F.ucState == 1)) {       // {11}
            unsigned char ucRegisterValue;
            unsigned char *ptr = (unsigned char *)&simMMA7660F.MMA7660F_registers;
            ptr += simMMA7660F.ucInternalPointer++;                      // move to the present internal register
            ucRegisterValue = *ptr;
            return (ucRegisterValue);                                    // return the register value
        }
        else if (simDS1621.ucRW && (simDS1621.ucState >= 1)) {
            if (simDS1621.ucCommand == 0xaa) {                           // if the get temperature command was previously executed 
                if (simDS1621.ucState == 2) {
                    simDS1621.ucState--;
                    if (simDS1621.ucTemperature[0] == 10) {              // change between 10 and 35°C on each read
                        simDS1621.ucTemperature[0] = 35;
                    }
                    else {
                        simDS1621.ucTemperature[0] = 10;
                    }
                    return (simDS1621.ucTemperature[0]);
                }
                else {
                    simDS1621.ucState = 0;
                    simDS1621.ucCommand = 0;
                    return (simDS1621.ucTemperature[1]);
                }
            }
            else {
                return 0;
            }
        }
#ifdef I2C_EEPROM_FILE_SYSTEM                                            // {7}
        else if ((simM24M01[0].ucRW) && (simM24M01[0].ucState == 4)) {   // reading
            unsigned char ucReturn = M24M01_eeprom[0][simM24M01[0].ulInternalPointer++];
            if (simM24M01[0].ulInternalPointer >= simM24M01[0].ulMaxEEPROMLength) {
                simM24M01[0].ulInternalPointer = 0;
            }
            return ucReturn;
        }
        else if ((simM24M01[1].ucRW) && (simM24M01[1].ucState == 4)) {   // reading
            unsigned char ucReturn = M24M01_eeprom[1][simM24M01[1].ulInternalPointer++];
            if (simM24M01[1].ulInternalPointer >= simM24M01[1].ulMaxEEPROMLength) {
                simM24M01[1].ulInternalPointer = 0;
            }
            return ucReturn;
        }
#else
        else if (sim24C01.ucRW && (sim24C01.ucState >= 3)) {             // repeated start - first byte is data
            unsigned char ucReturn = sim24C01.ucEEPROM[sim24C01.ucInternalAddress++];
            if (sim24C01.ucInternalAddress >= sim24C01.usMaxEEPROMLength) {
                sim24C01.ucInternalAddress = 0;
            }
            return (ucReturn);
        }
        else if (simDS3640.ucRW && (simDS3640.ucState >= 3)) {           // repeated start - first byte is data
            unsigned char *ptrData = (unsigned char *)&simDS3640.ucData;
            unsigned char ucReturn;

            ptrData += simDS3640.ucInternalPointer;
            if (simDS3640.ucInternalPointer++ >= 0x80) {                 // in paged memory
                ptrData += 128*simDS3640.ucData.ucBankSelect;
            }
            ucReturn = *ptrData;
            return (ucReturn);
        }
#endif
        else if (simSTMPE811.ucRW && (simSTMPE811.ucState != 0)) {       // {5} read from STMPE811
            unsigned char *ptrData = (unsigned char *)&simSTMPE811.CHIP_ID[0];
            ptrData += simSTMPE811.ucInternalPointer;
            simSTMPE811.ucInternalPointer++;
            return (*ptrData);
        }
        else if (simLM75A.ucRW && (simLM75A.ucState != 0)) {             // {3} LM75A is being read
            switch (simLM75A.ucInternalPointer) {
            case 0:                                                      // temperature value
                if (simLM75A.ucState == 1) {
                    simLM75A.ucState++;
                    return (simLM75A.ucTemperature[0]);
                }
                else if (simLM75A.ucState == 2) {
                    simLM75A.ucState++;
                    return (simLM75A.ucTemperature[1]);
                }
                else {
                    return 0xff;
                }
                break;
            case 1:                                                      // configuration
                if ((simLM75A.ucState == 1) || (simLM75A.ucState == 2)) {
                    simLM75A.ucState++;
                    return (simLM75A.ucConfig);
                }
                else {
                    return 0xff;
                }
                break;
            case 2:                                                      // temperature hyst
                if (simLM75A.ucState == 1) {
                    simLM75A.ucState++;
                    return (simLM75A.ucHyst[0]);
                }
                else if (simLM75A.ucState == 2) {
                    simLM75A.ucState++;
                    return (simLM75A.ucHyst[1]);
                }
                else {
                    return 0xff;
                }
                break;
            case 3:                                                      // temperature offset
                if (simLM75A.ucState == 1) {
                    simLM75A.ucState++;
                    return (simLM75A.ucOS[0]);
                }
                else if (simLM75A.ucState == 2) {
                    simLM75A.ucState++;
                    return (simLM75A.ucOS[1]);
                }
                else {
                    return 0xff;
                }
                break;
            case 7:                                                      // product ID
                if ((simLM75A.ucState == 1) || (simLM75A.ucState == 2)) {
                    simLM75A.ucState++;
                    return (simLM75A.ucID);
                }
                else {
                    return 0xff;
                }
                break;
            default:
                return 0xff;
            }
        }
        else if (simSHT21.ucRW && (simSHT21.ucState != 0)) {             // {6} SHT21 being read
            switch (simSHT21.ucInternalPointer) {
            case 0xe3:                                                   // trigger temperature measurement, hold master
            case 0xf3:                                                   // trigger temperature measurement, no-hold master
                return (simSHT21.usTemperatur[simSHT21.ucState++ - 1]);
            case 0xe5:                                                   // trigger humidity measurement, hold master
            case 0xf5:                                                   // trigger humidity measurement, no-hold master
                return (simSHT21.usHumidity[simSHT21.ucState++ - 1]);
            case 0xe7:                                                   // read user register
                return (simSHT21.ucUserRegister);
            default:
                break;
            }
            return (0xff);
        }
#if defined PCA9539_CNT
        else if ((simPCA9539.ucState[0] != 0) && (simPCA9539.ucRW[0])) { // {8} setting internal register
            unsigned char *ptrReg = &simPCA9539.regs[0].ucInput0;
            ptrReg += simPCA9539.ucAdd[0];
            return (*ptrReg);                                            // read register value
        }
    #if PCA9539_CNT > 1
        else if ((simPCA9539.ucState[1] != 0) && (simPCA9539.ucRW[1])) { // setting internal register
            unsigned char *ptrReg = &simPCA9539.regs[1].ucInput0;
            ptrReg += simPCA9539.ucAdd[1];
            return (*ptrReg);                                            // read register value
        }
    #endif
#endif
#ifdef USE_USB_OTG_CHARGE_PUMP
        else if (simMAX3353.ucRW && (simMAX3353.ucState >= 3)) {         // repeated start - first byte is data
            return (0);
        }
#endif
        else {
            int i = 0;
#if defined PCF8575_CNT
            i = 0;
            while (i < PCF8575_CNT) {
                if ((simPCF8575[i].ucState >= 1) && (simPCF8575[i].ucRW & 0x01)) { // device being addressed as read
                    unsigned char ucResult = simPCF8575[i].ucInput[simPCF8575[i].ucState - 1];
                    ucResult &= simPCF8575[i].ucOutput[simPCF8575[i].ucState - 1]; // when outputs are not set to '1' they are driving and we read this value
                    if (++simPCF8575[i].ucState > 2) {                   // two input bytes
                        simPCF8575[i].ucState = 1;
                    }
                    return ucResult;
                }
                i++;
            }
#endif
        }
        break;

    case I2C_RX_COMPLETE:
    case I2C_TX_COMPLETE:
        fnResetOthers(0);
        break;
    }
    return 0;
}


#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE              // {5}

#define MIN_X_TOUCH        0x00f0
#define MAX_X_TOUCH        0x0f26
#define MIN_Y_TOUCH        0x0110
#define MAX_Y_TOUCH        0x0f10


// This routine is used to set touch screen ADC values
//
extern void fnSTMPE811(int iX, int iY)
{
    unsigned short usX, usY;
    if (iX < 0) {                                                        // pen lifted
        simSTMPE811.FIFO_STA = 0x20;                                     // FIFO empty
        simSTMPE811.TSC_CTRL &= ~0x80;
        return;
    }
    simSTMPE811.FIFO_STA = 0;                                            // FIFO not empty
    simSTMPE811.TSC_CTRL |= 0x80;
    iX -= 6;
    iY -= 4;
    usX = (MIN_X_TOUCH + ((iX * (MAX_X_TOUCH - MIN_X_TOUCH))/GLCD_X));
    usY = (MIN_Y_TOUCH + ((iY * (MAX_Y_TOUCH - MIN_Y_TOUCH))/GLCD_Y));
    simSTMPE811.TSC_DATA_X[0] = (unsigned char)(usY >> 8);
    simSTMPE811.TSC_DATA_X[1] = (unsigned char)(usY);
    simSTMPE811.TSC_DATA_Y[0] = (unsigned char)(usX >> 8);
    simSTMPE811.TSC_DATA_Y[1] = (unsigned char)(usX);
}
#endif
