/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      slcd_time.h
    Project:   uTasker Demonstration project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
*/

#if defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_K40D100M
// Register LCD_WF15TO12
//
#define SEVEN_SEGMENT_1_TOP         0x00010000
#define SEVEN_SEGMENT_1_MID         0x02000000
#define SEVEN_SEGMENT_1_BOT         0x00080000
#define SEVEN_SEGMENT_1_T_L         0x01000000
#define SEVEN_SEGMENT_1_B_L         0x04000000
#define SEVEN_SEGMENT_1_T_R         0x00020000
#define SEVEN_SEGMENT_1_B_R         0x00040000

#define SEVEN_SEGMENT_1             (SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_MID | SEVEN_SEGMENT_1_BOT | SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)

#define SEVEN_SEGMENT_1_0           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_MID))
#define SEVEN_SEGMENT_1_1           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)
#define SEVEN_SEGMENT_1_2           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_R))
#define SEVEN_SEGMENT_1_3           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L))
#define SEVEN_SEGMENT_1_4           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_BOT))
#define SEVEN_SEGMENT_1_5           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_6           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_7           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R | SEVEN_SEGMENT_1_TOP)
#define SEVEN_SEGMENT_1_8           SEVEN_SEGMENT_1
#define SEVEN_SEGMENT_1_9           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_B_L))


#define SEVEN_SEGMENT_2_TOP         0x00000001
#define SEVEN_SEGMENT_2_MID         0x00000200
#define SEVEN_SEGMENT_2_BOT         0x00000008
#define SEVEN_SEGMENT_2_T_L         0x00000100
#define SEVEN_SEGMENT_2_B_L         0x00000400
#define SEVEN_SEGMENT_2_T_R         0x00000002
#define SEVEN_SEGMENT_2_B_R         0x00000004

#define SEVEN_SEGMENT_2             (SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_MID | SEVEN_SEGMENT_2_BOT | SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)

#define SEVEN_SEGMENT_2_0           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_MID))
#define SEVEN_SEGMENT_2_1           (SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)
#define SEVEN_SEGMENT_2_2           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_R))
#define SEVEN_SEGMENT_2_3           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L))
#define SEVEN_SEGMENT_2_4           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_BOT))
#define SEVEN_SEGMENT_2_5           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_T_R))
#define SEVEN_SEGMENT_2_6           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_R))
#define SEVEN_SEGMENT_2_7           (SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R | SEVEN_SEGMENT_2_TOP)
#define SEVEN_SEGMENT_2_8           SEVEN_SEGMENT_2
#define SEVEN_SEGMENT_2_9           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_B_L))

// Register LCD_WF23TO20
//
#define SEGMENT1                    0x00080000
#define SEGMENT_AM                  0x00000400
#define SEGMENT_PM                  0x00000800
#define SEVEN_SEGMENT_3_TOP         0x00000001
#define SEVEN_SEGMENT_3_MID         0x00020000
#define SEVEN_SEGMENT_3_BOT         0x00000008
#define SEVEN_SEGMENT_3_T_L         0x00010000
#define SEVEN_SEGMENT_3_B_L         0x00040000
#define SEVEN_SEGMENT_3_T_R         0x00000002
#define SEVEN_SEGMENT_3_B_R         0x00000004

#define SEGMENT_COLON               0x00000800

#define SEVEN_SEGMENT_3             (SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_MID | SEVEN_SEGMENT_3_BOT | SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)

#define SEVEN_SEGMENT_3_0           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_MID))
#define SEVEN_SEGMENT_3_1           (SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)
#define SEVEN_SEGMENT_3_2           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_R))
#define SEVEN_SEGMENT_3_3           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L))
#define SEVEN_SEGMENT_3_4           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_BOT))
#define SEVEN_SEGMENT_3_5           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_6           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_7           (SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R | SEVEN_SEGMENT_3_TOP)
#define SEVEN_SEGMENT_3_8           SEVEN_SEGMENT_3
#define SEVEN_SEGMENT_3_9           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_B_L))

static const unsigned short usSegment1[] = {                             // hours 0..9
    (SEVEN_SEGMENT_1_0 >> 16),
    (SEVEN_SEGMENT_1_1 >> 16),
    (SEVEN_SEGMENT_1_2 >> 16),
    (SEVEN_SEGMENT_1_3 >> 16),
    (SEVEN_SEGMENT_1_4 >> 16),
    (SEVEN_SEGMENT_1_5 >> 16),
    (SEVEN_SEGMENT_1_6 >> 16),
    (SEVEN_SEGMENT_1_7 >> 16),
    (SEVEN_SEGMENT_1_8 >> 16),
    (SEVEN_SEGMENT_1_9 >> 16),
};

static const unsigned short usSegment2[] = {                             // tens of minutes 0..5
    SEVEN_SEGMENT_2_0,
    SEVEN_SEGMENT_2_1,
    SEVEN_SEGMENT_2_2,
    SEVEN_SEGMENT_2_3,
    SEVEN_SEGMENT_2_4,
    SEVEN_SEGMENT_2_5,
};

static const unsigned long ulSegment3[] = {                              // minutes 0..9
    (SEVEN_SEGMENT_3_0 >> 0),
    (SEVEN_SEGMENT_3_1 >> 0),
    (SEVEN_SEGMENT_3_2 >> 0),
    (SEVEN_SEGMENT_3_3 >> 0),
    (SEVEN_SEGMENT_3_4 >> 0),
    (SEVEN_SEGMENT_3_5 >> 0),
    (SEVEN_SEGMENT_3_6 >> 0),
    (SEVEN_SEGMENT_3_7 >> 0),
    (SEVEN_SEGMENT_3_8 >> 0),
    (SEVEN_SEGMENT_3_9 >> 0),
};

static void fnTimeDisplay(unsigned char ucHours, unsigned char ucMinutes, unsigned char ucSeconds)
{
    unsigned long ulRegister1 = LCD_WF15TO12;                            // backup the present values
    unsigned long ulRegister2 = LCD_WF23TO20;

    ulRegister1 &= ~(SEVEN_SEGMENT_1 | SEVEN_SEGMENT_2);                 // clear all segments that represent the time
    ulRegister2 &= ~(SEGMENT1 | SEVEN_SEGMENT_3 | SEGMENT_AM | SEGMENT_PM);

    if (ucHours >= 12) {                                                  // decide whether AM or PM
        ulRegister2 |= SEGMENT_PM;
        ucHours -= 12;
    }
    else {
        ulRegister2 |= SEGMENT_AM;
    }
    if (ucHours >= 10) {                                                 // convert to 12 hour clock mode
        ulRegister2 |= SEGMENT1;
        ucHours -= 10;
    }
    ulRegister1 |= (usSegment1[ucHours] << 16);                          // set the hours
    ulRegister1 |= usSegment2[ucMinutes/10];                             // set the tens of minutes
    ucMinutes -= ((ucMinutes/10) * 10);
    ulRegister2 |= (ulSegment3[ucMinutes] << 0);                         // set the minutes
    WRITE_SLCD(15TO12, ulRegister1);                                     // write new values
    WRITE_SLCD(23TO20, ulRegister2);
    TOGGLE_SLCD(15TO12, SEGMENT_COLON);                                  // toggle ':'
}
#elif defined TWR_KL46Z48M || defined TWR_KL43Z48M
// Registers LCD_WF23TO20 and LCD_WF27TO24
//
#define SEVEN_SEGMENT_1_TOP         0x00010000                           // 22-1
#define SEVEN_SEGMENT_1_MID         0x00000002                           // 24-2
#define SEVEN_SEGMENT_1_BOT         0x00080000                           // 22-8
#define SEVEN_SEGMENT_1_T_L         0x01000001                           // 24-1
#define SEVEN_SEGMENT_1_B_L         0x00000004                           // 24-4
#define SEVEN_SEGMENT_1_T_R         0x00020000                           // 22-2
#define SEVEN_SEGMENT_1_B_R         0x00040000                           // 22-4

#define SEVEN_SEGMENT_1_A           (SEVEN_SEGMENT_1_MID | SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L)
#define SEVEN_SEGMENT_1_B           (SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_BOT | SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)
#define SEVEN_SEGMENT_1             (SEVEN_SEGMENT_1_A | SEVEN_SEGMENT_1_B)

#define SEVEN_SEGMENT_1_0           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_MID))
#define SEVEN_SEGMENT_1_1           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)
#define SEVEN_SEGMENT_1_2           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_R))
#define SEVEN_SEGMENT_1_3           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L))
#define SEVEN_SEGMENT_1_4           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_BOT))
#define SEVEN_SEGMENT_1_5           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_6           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_7           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R | SEVEN_SEGMENT_1_TOP)
#define SEVEN_SEGMENT_1_8           SEVEN_SEGMENT_1
#define SEVEN_SEGMENT_1_9           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_B_L))

// Register LCD_WF23TO20 and LCD_WF3TO0
//
#define SEVEN_SEGMENT_2_TOP         0x01000000                           // 3-1
#define SEVEN_SEGMENT_2_MID         0x00000002                           // 20-2
#define SEVEN_SEGMENT_2_BOT         0x08000000                           // 3-8
#define SEVEN_SEGMENT_2_T_L         0x00000001                           // 20-1
#define SEVEN_SEGMENT_2_B_L         0x00000004                           // 20-4
#define SEVEN_SEGMENT_2_T_R         0x02000000                           // 3-2
#define SEVEN_SEGMENT_2_B_R         0x04000000                           // 3-4

#define SEVEN_SEGMENT_2_A           (SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_BOT | SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)
#define SEVEN_SEGMENT_2_B           (SEVEN_SEGMENT_2_MID | SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L)
#define SEVEN_SEGMENT_2             (SEVEN_SEGMENT_2_A | SEVEN_SEGMENT_2_B)

#define SEVEN_SEGMENT_2_0           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_MID))
#define SEVEN_SEGMENT_2_1           (SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)
#define SEVEN_SEGMENT_2_2           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_R))
#define SEVEN_SEGMENT_2_3           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L))
#define SEVEN_SEGMENT_2_4           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_BOT))
#define SEVEN_SEGMENT_2_5           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_T_R))
#define SEVEN_SEGMENT_2_6           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_R))
#define SEVEN_SEGMENT_2_7           (SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R | SEVEN_SEGMENT_2_TOP)
#define SEVEN_SEGMENT_2_8           SEVEN_SEGMENT_2
#define SEVEN_SEGMENT_2_9           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_B_L))

// Register LCD_WF3TO0
//
#define SEGMENT1                    0x00000800                           // 1-8
#define SEGMENT_AM                  0x00000004                           // 0-4
#define SEGMENT_PM                  0x00000008                           // 0-8

// Register LCD_WF23TO20 and LCD_WF3TO0
//
#define SEVEN_SEGMENT_3_TOP         0x01000000                           // 23-1
#define SEVEN_SEGMENT_3_MID         0x00000200                           // 1-2
#define SEVEN_SEGMENT_3_BOT         0x08000000                           // 23-8
#define SEVEN_SEGMENT_3_T_L         0x00000100                           // 1-1
#define SEVEN_SEGMENT_3_B_L         0x00000400                           // 1-4
#define SEVEN_SEGMENT_3_T_R         0x02000000                           // 23-2
#define SEVEN_SEGMENT_3_B_R         0x04000000                           // 23-4

#define SEGMENT_COLON               0x00000008                           // 20-8

#define SEVEN_SEGMENT_3_A           (SEVEN_SEGMENT_3_MID | SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L)
#define SEVEN_SEGMENT_3_B           (SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_BOT | SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)
#define SEVEN_SEGMENT_3             (SEVEN_SEGMENT_3_A | SEVEN_SEGMENT_3_B)

#define SEVEN_SEGMENT_3_0           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_MID))
#define SEVEN_SEGMENT_3_1           (SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)
#define SEVEN_SEGMENT_3_2           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_R))
#define SEVEN_SEGMENT_3_3           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L))
#define SEVEN_SEGMENT_3_4           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_BOT))
#define SEVEN_SEGMENT_3_5           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_6           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_7           (SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R | SEVEN_SEGMENT_3_TOP)
#define SEVEN_SEGMENT_3_8           SEVEN_SEGMENT_3
#define SEVEN_SEGMENT_3_9           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_B_L))

static const unsigned long ulSegment1[] = {                              // hours 0..9
    SEVEN_SEGMENT_1_0,
    SEVEN_SEGMENT_1_1,
    SEVEN_SEGMENT_1_2,
    SEVEN_SEGMENT_1_3,
    SEVEN_SEGMENT_1_4,
    SEVEN_SEGMENT_1_5,
    SEVEN_SEGMENT_1_6,
    SEVEN_SEGMENT_1_7,
    SEVEN_SEGMENT_1_8,
    SEVEN_SEGMENT_1_9,
};

static const unsigned long ulSegment2[] = {                             // tens of minutes 0..5
    SEVEN_SEGMENT_2_0,
    SEVEN_SEGMENT_2_1,
    SEVEN_SEGMENT_2_2,
    SEVEN_SEGMENT_2_3,
    SEVEN_SEGMENT_2_4,
    SEVEN_SEGMENT_2_5,
};

static const unsigned long ulSegment3[] = {                              // minutes 0..9
    SEVEN_SEGMENT_3_0,
    SEVEN_SEGMENT_3_1,
    SEVEN_SEGMENT_3_2,
    SEVEN_SEGMENT_3_3,
    SEVEN_SEGMENT_3_4,
    SEVEN_SEGMENT_3_5,
    SEVEN_SEGMENT_3_6,
    SEVEN_SEGMENT_3_7,
    SEVEN_SEGMENT_3_8,
    SEVEN_SEGMENT_3_9,
};

static void fnTimeDisplay(unsigned char ucHours, unsigned char ucMinutes, unsigned char ucSeconds)
{
    unsigned long ulRegister1;                                           // backup the present values
    unsigned long ulRegister2;
    unsigned long ulRegister3;

    ulRegister1 = LCD_WF23TO20;
    ulRegister1 &= ~(SEVEN_SEGMENT_3_B | SEVEN_SEGMENT_2_B | SEVEN_SEGMENT_1_B);
    ulRegister2 = LCD_WF3TO0;
    ulRegister2 &= ~(SEVEN_SEGMENT_3_A | SEVEN_SEGMENT_2_A | SEGMENT1 | SEGMENT_PM);
    ulRegister3 = LCD_WF27TO24;
    ulRegister3 &= ~(SEVEN_SEGMENT_1_A);

    if (ucHours >= 12) {                                                 // decide whether AM or PM
        ulRegister2 |= SEGMENT_PM;
        ucHours -= 12;
    }
    else {
        ulRegister2 |= SEGMENT_AM;                                       // set PM
    }
    if (ucHours >= 10) {                                                 // convert to 12 hour clock mode
        ulRegister2 |= SEGMENT1;
        ucHours -= 10;
    }

    ulRegister1 |= (ulSegment1[ucHours] & SEVEN_SEGMENT_1_B);            // set the hours
    ulRegister3 |= (ulSegment1[ucHours] & SEVEN_SEGMENT_1_A);

    ulRegister1 |= (ulSegment2[ucMinutes/10] & SEVEN_SEGMENT_2_B);       // tenths of minutes
    ulRegister2 |= (ulSegment2[ucMinutes/10] & SEVEN_SEGMENT_2_A);

    ucMinutes -= ((ucMinutes/10) * 10);

    ulRegister1 |= (ulSegment3[ucMinutes] & SEVEN_SEGMENT_3_B);
    ulRegister1 ^= SEGMENT_COLON;                                        // toggle ':'

    ulRegister2 |= (ulSegment3[ucMinutes] & SEVEN_SEGMENT_3_A);

    WRITE_SLCD(23TO20, ulRegister1);
    WRITE_SLCD(3TO0,   ulRegister2);
    WRITE_SLCD(27TO24, ulRegister3);
}
#elif defined FRDM_KL43Z
// Register LCD_WF27TO24 and LCD_WF23TO20 (tens of hours)
//
#define SEVEN_SEGMENT_0_TOP         0x00000008                           // 24-4
#define SEVEN_SEGMENT_0_MID         0x00000004                           // 20-3
#define SEVEN_SEGMENT_0_BOT         0x00000001                           // 20-1
#define SEVEN_SEGMENT_0_T_L         0x00000008                           // 20-4
#define SEVEN_SEGMENT_0_B_L         0x00000002                           // 20-2
#define SEVEN_SEGMENT_0_T_R         0x00000004                           // 24-3
#define SEVEN_SEGMENT_0_B_R         0x00000002                           // 24-2

#define SEVEN_SEGMENT_0_20          (SEVEN_SEGMENT_0_MID | SEVEN_SEGMENT_0_BOT | SEVEN_SEGMENT_0_T_L | SEVEN_SEGMENT_0_B_L)
#define SEVEN_SEGMENT_0_24          (SEVEN_SEGMENT_0_TOP | SEVEN_SEGMENT_0_T_R | SEVEN_SEGMENT_0_B_R)
#define SEVEN_SEGMENT_0_0           (SEVEN_SEGMENT_0_20 | SEVEN_SEGMENT_0_24)

#define SEVEN_SEGMENT_0_1           (SEVEN_SEGMENT_0_T_R | SEVEN_SEGMENT_0_B_R)
#define SEVEN_SEGMENT_0_2           ((SEVEN_SEGMENT_0_TOP | SEVEN_SEGMENT_0_T_R | SEVEN_SEGMENT_0_MID | SEVEN_SEGMENT_0_B_L | SEVEN_SEGMENT_0_BOT) & SEVEN_SEGMENT_0_17)


// Register LCD_WF27TO24 (hours)
//
#define SEVEN_SEGMENT_1_TOP         0x08000000                           // 27-4
#define SEVEN_SEGMENT_1_MID         0x00040000                           // 26-3
#define SEVEN_SEGMENT_1_BOT         0x00010000                           // 26-1
#define SEVEN_SEGMENT_1_T_L         0x00080000                           // 26-4
#define SEVEN_SEGMENT_1_B_L         0x00020000                           // 26-2
#define SEVEN_SEGMENT_1_T_R         0x04000000                           // 27-3
#define SEVEN_SEGMENT_1_B_R         0x02000000                           // 27-2


#define SEVEN_SEGMENT_1_ALL         (SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_MID | SEVEN_SEGMENT_1_BOT | SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)
#define SEVEN_SEGMENT_1_0           (SEVEN_SEGMENT_1_ALL & ~(SEVEN_SEGMENT_1_MID))
#define SEVEN_SEGMENT_1_1           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)
#define SEVEN_SEGMENT_1_2           (SEVEN_SEGMENT_1_ALL & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_R))
#define SEVEN_SEGMENT_1_3           (SEVEN_SEGMENT_1_ALL & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L))
#define SEVEN_SEGMENT_1_4           (SEVEN_SEGMENT_1_ALL & ~(SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_BOT))
#define SEVEN_SEGMENT_1_5           (SEVEN_SEGMENT_1_ALL & ~(SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_6           (SEVEN_SEGMENT_1_ALL & ~(SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_7           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R | SEVEN_SEGMENT_1_TOP)
#define SEVEN_SEGMENT_1_8           SEVEN_SEGMENT_1_ALL
#define SEVEN_SEGMENT_1_9           (SEVEN_SEGMENT_1_ALL & ~(SEVEN_SEGMENT_1_B_L))

// Registers LCD_WF43TO40 (tens of minutes)
//
#define SEVEN_SEGMENT_2_TOP         0x00080000                           // 42-4
#define SEVEN_SEGMENT_2_MID         0x00000004                           // 40-3
#define SEVEN_SEGMENT_2_BOT         0x00000001                           // 40-1
#define SEVEN_SEGMENT_2_T_L         0x00000008                           // 40-4
#define SEVEN_SEGMENT_2_B_L         0x00000002                           // 40-2
#define SEVEN_SEGMENT_2_T_R         0x00040000                           // 42-3
#define SEVEN_SEGMENT_2_B_R         0x00020000                           // 42-2

#define SEVEN_SEGMENT_2_ALL         (SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_MID | SEVEN_SEGMENT_2_BOT | SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)

#define SEVEN_SEGMENT_2_0           (SEVEN_SEGMENT_2_ALL & ~(SEVEN_SEGMENT_2_MID))
#define SEVEN_SEGMENT_2_1           (SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)
#define SEVEN_SEGMENT_2_2           (SEVEN_SEGMENT_2_ALL & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_R))
#define SEVEN_SEGMENT_2_3           (SEVEN_SEGMENT_2_ALL & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L))
#define SEVEN_SEGMENT_2_4           (SEVEN_SEGMENT_2_ALL & ~(SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_BOT))
#define SEVEN_SEGMENT_2_5           (SEVEN_SEGMENT_2_ALL & ~(SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_T_R))
#define SEVEN_SEGMENT_2_6           (SEVEN_SEGMENT_2_ALL & ~(SEVEN_SEGMENT_2_T_R))
#define SEVEN_SEGMENT_2_7           (SEVEN_SEGMENT_2_ALL | SEVEN_SEGMENT_2_B_R | SEVEN_SEGMENT_2_TOP)
#define SEVEN_SEGMENT_2_8           (SEVEN_SEGMENT_2_ALL)
#define SEVEN_SEGMENT_2_9           (SEVEN_SEGMENT_2_ALL & ~(SEVEN_SEGMENT_2_B_L))

// Register LCD_WF43TO40 and LCD_WF47TO44 (minutes)
//
#define SEVEN_SEGMENT_3_TOP         0x00000008                           // 44-4
#define SEVEN_SEGMENT_3_MID         0x04000000                           // 43-3
#define SEVEN_SEGMENT_3_BOT         0x01000000                           // 43-1
#define SEVEN_SEGMENT_3_T_L         0x08000000                           // 43-4
#define SEVEN_SEGMENT_3_B_L         0x02000000                           // 43-2
#define SEVEN_SEGMENT_3_T_R         0x00000004                           // 44-3
#define SEVEN_SEGMENT_3_B_R         0x00000002                           // 44-2

#define SEGMENT_COLON               0x01

#define SEVEN_SEGMENT_3_44_MASK     (SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)
#define SEVEN_SEGMENT_3_43_MASK     (SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_BOT | SEVEN_SEGMENT_3_MID)
#define SEVEN_SEGMENT_3_ALL         (SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_MID | SEVEN_SEGMENT_3_BOT | SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)

#define SEVEN_SEGMENT_3_0           (SEVEN_SEGMENT_3_ALL & ~(SEVEN_SEGMENT_3_MID))
#define SEVEN_SEGMENT_3_1           (SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)
#define SEVEN_SEGMENT_3_2           (SEVEN_SEGMENT_3_ALL & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_R))
#define SEVEN_SEGMENT_3_3           (SEVEN_SEGMENT_3_ALL & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L))
#define SEVEN_SEGMENT_3_4           (SEVEN_SEGMENT_3_ALL & ~(SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_BOT))
#define SEVEN_SEGMENT_3_5           (SEVEN_SEGMENT_3_ALL & ~(SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_6           (SEVEN_SEGMENT_3_ALL & ~(SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_7           (SEVEN_SEGMENT_3_ALL | SEVEN_SEGMENT_3_B_R | SEVEN_SEGMENT_3_TOP)
#define SEVEN_SEGMENT_3_8           SEVEN_SEGMENT_3_ALL
#define SEVEN_SEGMENT_3_9           (SEVEN_SEGMENT_3_ALL & ~(SEVEN_SEGMENT_3_B_L))


static const unsigned long ulSegment1[] = {                             // hours 0..9
    SEVEN_SEGMENT_1_0,
    SEVEN_SEGMENT_1_1,
    SEVEN_SEGMENT_1_2,
    SEVEN_SEGMENT_1_3,
    SEVEN_SEGMENT_1_4,
    SEVEN_SEGMENT_1_5,
    SEVEN_SEGMENT_1_6,
    SEVEN_SEGMENT_1_7,
    SEVEN_SEGMENT_1_8,
    SEVEN_SEGMENT_1_9,
};

static const unsigned long ulSegment2[] = {                              // tens of minutes 0..5
    SEVEN_SEGMENT_2_0,
    SEVEN_SEGMENT_2_1,
    SEVEN_SEGMENT_2_2,
    SEVEN_SEGMENT_2_3,
    SEVEN_SEGMENT_2_4,
    SEVEN_SEGMENT_2_5,
};

static const unsigned long ulSegment3[] = {                              // minutes 0..9
    SEVEN_SEGMENT_3_0,
    SEVEN_SEGMENT_3_1,
    SEVEN_SEGMENT_3_2,
    SEVEN_SEGMENT_3_3,
    SEVEN_SEGMENT_3_4,
    SEVEN_SEGMENT_3_5,
    SEVEN_SEGMENT_3_6,
    SEVEN_SEGMENT_3_7,
    SEVEN_SEGMENT_3_8,
    SEVEN_SEGMENT_3_9,
};

static void fnTimeDisplay(unsigned char ucHours, unsigned char ucMinutes, unsigned char ucSeconds)
{
    unsigned long ulRegister = 0;

    switch (ucHours/10) {
    case 0:
        WRITE_SLCD(23TO20, 0);                                           // prepare the tens of hours (' ')
        break;
    case 1:
        ulRegister = (SEVEN_SEGMENT_0_B_R | SEVEN_SEGMENT_0_T_R);        // prepare the tens of hours ('1')
        WRITE_SLCD(23TO20, 0);
        break;
    case 2:
        ulRegister = (SEVEN_SEGMENT_0_TOP | SEVEN_SEGMENT_0_T_R);        // prepare the tens of hours ('2')
        WRITE_SLCD(23TO20, (SEVEN_SEGMENT_0_MID | SEVEN_SEGMENT_0_BOT | SEVEN_SEGMENT_0_B_L));
        break;
    }
    ucHours -= ((ucHours/10) * 10);
    WRITE_SLCD(27TO24, (ulRegister | ulSegment1[ucHours]));              // write hours
    ulRegister = (ulSegment2[ucMinutes/10] & SEVEN_SEGMENT_2_ALL);       // prepare tens of minutes
    ucMinutes -= ((ucMinutes/10) * 10);
    ulRegister |= (ulSegment3[ucMinutes] & SEVEN_SEGMENT_3_43_MASK);     // add minutes
    WRITE_SLCD(43TO40, ulRegister);                                      // write tens of minutes and part of minutes
    ulRegister = LCD_WF47TO44;                                           // read present value
    ulRegister &= ~SEVEN_SEGMENT_3_44_MASK;                              // mask out the minutes part
    ulRegister ^= SEGMENT_COLON;                                         // toggle ':'
    ulRegister |= (ulSegment3[ucMinutes] & SEVEN_SEGMENT_3_44_MASK);     // add second part of minutes
    WRITE_SLCD(47TO44, ulRegister);                                      // complete minutes and colon blink
}
#elif defined FRDM_KL46Z
// Register LCD_WF19TO16 and LCD_WF39TO36 (tens of hours)
//
#define SEVEN_SEGMENT_0_TOP         0x00000800                           // 17-8
#define SEVEN_SEGMENT_0_MID         0x00000400                           // 37-4
#define SEVEN_SEGMENT_0_BOT         0x00000100                           // 37-1
#define SEVEN_SEGMENT_0_T_L         0x00000800                           // 37-8
#define SEVEN_SEGMENT_0_B_L         0x00000200                           // 37-2
#define SEVEN_SEGMENT_0_T_R         0x00000400                           // 17-4
#define SEVEN_SEGMENT_0_B_R         0x00000200                           // 17-2

#define SEVEN_SEGMENT_0_17          (SEVEN_SEGMENT_0_TOP | SEVEN_SEGMENT_0_T_R | SEVEN_SEGMENT_0_B_R)
#define SEVEN_SEGMENT_0_37          (SEVEN_SEGMENT_0_MID | SEVEN_SEGMENT_0_BOT | SEVEN_SEGMENT_0_T_L | SEVEN_SEGMENT_0_B_L)
#define SEVEN_SEGMENT_0_0           (SEVEN_SEGMENT_0_17 | SEVEN_SEGMENT_0_37)

#define SEVEN_SEGMENT_0_1           (SEVEN_SEGMENT_0_T_R | SEVEN_SEGMENT_0_B_R)
#define SEVEN_SEGMENT_0_2           ((SEVEN_SEGMENT_0_TOP | SEVEN_SEGMENT_0_T_R | SEVEN_SEGMENT_0_MID | SEVEN_SEGMENT_0_B_L | SEVEN_SEGMENT_0_BOT) & SEVEN_SEGMENT_0_17)


// Register LCD_WF11TO8 and LCD_WF7TO4 (hours)
//
#define SEVEN_SEGMENT_1_TOP         0x00000008                           // 8-8
#define SEVEN_SEGMENT_1_MID         0x04000000                           // 7-4
#define SEVEN_SEGMENT_1_BOT         0x01000000                           // 7-1
#define SEVEN_SEGMENT_1_T_L         0x08000000                           // 7-8
#define SEVEN_SEGMENT_1_B_L         0x02000000                           // 7-2
#define SEVEN_SEGMENT_1_T_R         0x00000004                           // 8-4
#define SEVEN_SEGMENT_1_B_R         0x00000002                           // 8-2

#define SEVEN_SEGMENT_1__7          (SEVEN_SEGMENT_1_MID | SEVEN_SEGMENT_1_BOT | SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L)
#define SEVEN_SEGMENT_1__8          (SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)
#define SEVEN_SEGMENT_1             (SEVEN_SEGMENT_1__7 | SEVEN_SEGMENT_1__8)

#define SEVEN_SEGMENT_1_0           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_MID))
#define SEVEN_SEGMENT_1_1           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R)
#define SEVEN_SEGMENT_1_2           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_R))
#define SEVEN_SEGMENT_1_3           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_L | SEVEN_SEGMENT_1_B_L))
#define SEVEN_SEGMENT_1_4           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_TOP | SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_BOT))
#define SEVEN_SEGMENT_1_5           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_B_L | SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_6           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_T_R))
#define SEVEN_SEGMENT_1_7           (SEVEN_SEGMENT_1_T_R | SEVEN_SEGMENT_1_B_R | SEVEN_SEGMENT_1_TOP)
#define SEVEN_SEGMENT_1_8           SEVEN_SEGMENT_1
#define SEVEN_SEGMENT_1_9           (SEVEN_SEGMENT_1 & ~(SEVEN_SEGMENT_1_B_L))

// Registers LCD_WF39TO36 and LCD_WF55TO52 (tens of minutes)
//
#define SEVEN_SEGMENT_2_TOP         0x00080000                           // 38-8
#define SEVEN_SEGMENT_2_MID         0x00000400                           // 53-4
#define SEVEN_SEGMENT_2_BOT         0x00000100                           // 53-1
#define SEVEN_SEGMENT_2_T_L         0x00000800                           // 53-8
#define SEVEN_SEGMENT_2_B_L         0x00000200                           // 53-2
#define SEVEN_SEGMENT_2_T_R         0x00040000                           // 38-4
#define SEVEN_SEGMENT_2_B_R         0x00020000                           // 38-2

#define SEVEN_SEGMENT_2_38          (SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)
#define SEVEN_SEGMENT_2_53          (SEVEN_SEGMENT_2_MID | SEVEN_SEGMENT_2_BOT | SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L)
#define SEVEN_SEGMENT_2             (SEVEN_SEGMENT_2_38 | SEVEN_SEGMENT_2_53)

#define SEVEN_SEGMENT_2_0           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_MID))
#define SEVEN_SEGMENT_2_1           (SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R)
#define SEVEN_SEGMENT_2_2           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_R))
#define SEVEN_SEGMENT_2_3           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_L | SEVEN_SEGMENT_2_B_L))

#define SEVEN_SEGMENT_2_4           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_TOP | SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_BOT))

#define SEVEN_SEGMENT_2_5           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_B_L | SEVEN_SEGMENT_2_T_R))

#define SEVEN_SEGMENT_2_6           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_T_R))

#define SEVEN_SEGMENT_2_7           (SEVEN_SEGMENT_2_T_R | SEVEN_SEGMENT_2_B_R | SEVEN_SEGMENT_2_TOP)
#define SEVEN_SEGMENT_2_8           (SEVEN_SEGMENT_2)
#define SEVEN_SEGMENT_2_9           (SEVEN_SEGMENT_2 & ~(SEVEN_SEGMENT_2_B_L))

// Register LCD_WF11TO8 (minutes)
//
#define SEVEN_SEGMENT_3_TOP         0x08000000                           // 11-8
#define SEVEN_SEGMENT_3_MID         0x00040000                           // 10-4
#define SEVEN_SEGMENT_3_BOT         0x00010000                           // 10-1
#define SEVEN_SEGMENT_3_T_L         0x00080000                           // 10-8
#define SEVEN_SEGMENT_3_B_L         0x00020000                           // 10-2
#define SEVEN_SEGMENT_3_T_R         0x04000000                           // 11-4
#define SEVEN_SEGMENT_3_B_R         0x02000000                           // 11-2

#define SEGMENT_COLON               0x01

#define SEVEN_SEGMENT_3             (SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_MID | SEVEN_SEGMENT_3_BOT | SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)

#define SEVEN_SEGMENT_3_0           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_MID))
#define SEVEN_SEGMENT_3_1           (SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R)
#define SEVEN_SEGMENT_3_2           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_R))
#define SEVEN_SEGMENT_3_3           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_L | SEVEN_SEGMENT_3_B_L))
#define SEVEN_SEGMENT_3_4           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_TOP | SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_BOT))
#define SEVEN_SEGMENT_3_5           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_B_L | SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_6           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_T_R))
#define SEVEN_SEGMENT_3_7           (SEVEN_SEGMENT_3_T_R | SEVEN_SEGMENT_3_B_R | SEVEN_SEGMENT_3_TOP)
#define SEVEN_SEGMENT_3_8           SEVEN_SEGMENT_3
#define SEVEN_SEGMENT_3_9           (SEVEN_SEGMENT_3 & ~(SEVEN_SEGMENT_3_B_L))


static const unsigned long ulSegment1[] = {                             // hours 0..9
    SEVEN_SEGMENT_1_0,
    SEVEN_SEGMENT_1_1,
    SEVEN_SEGMENT_1_2,
    SEVEN_SEGMENT_1_3,
    SEVEN_SEGMENT_1_4,
    SEVEN_SEGMENT_1_5,
    SEVEN_SEGMENT_1_6,
    SEVEN_SEGMENT_1_7,
    SEVEN_SEGMENT_1_8,
    SEVEN_SEGMENT_1_9,
};

static const unsigned long ulSegment2[] = {                              // tens of minutes 0..5
    SEVEN_SEGMENT_2_0,
    SEVEN_SEGMENT_2_1,
    SEVEN_SEGMENT_2_2,
    SEVEN_SEGMENT_2_3,
    SEVEN_SEGMENT_2_4,
    SEVEN_SEGMENT_2_5,
};

static const unsigned long ulSegment3[] = {                              // minutes 0..9
    (SEVEN_SEGMENT_3_0 >> 0),
    (SEVEN_SEGMENT_3_1 >> 0),
    (SEVEN_SEGMENT_3_2 >> 0),
    (SEVEN_SEGMENT_3_3 >> 0),
    (SEVEN_SEGMENT_3_4 >> 0),
    (SEVEN_SEGMENT_3_5 >> 0),
    (SEVEN_SEGMENT_3_6 >> 0),
    (SEVEN_SEGMENT_3_7 >> 0),
    (SEVEN_SEGMENT_3_8 >> 0),
    (SEVEN_SEGMENT_3_9 >> 0),
};

static void fnTimeDisplay(unsigned char ucHours, unsigned char ucMinutes, unsigned char ucSeconds)
{
    unsigned long ulRegister;
    
    ulRegister = LCD_WF19TO16;
    ulRegister &= ~(SEVEN_SEGMENT_0_17);
    switch (ucHours/10) {
    case 1:
        ulRegister |= (SEVEN_SEGMENT_0_B_R | SEVEN_SEGMENT_0_T_R);       // set the tens of hours ('1')
        break;
    case 2:
        ulRegister |= (SEVEN_SEGMENT_0_TOP | SEVEN_SEGMENT_0_T_R);       // set the tens of hours ('2')
        break;
    }
    WRITE_SLCD(19TO16, ulRegister);                                      // tens of hours (part 1)
    ulRegister = LCD_WF39TO36;
    ulRegister &= ~(SEVEN_SEGMENT_0_37);
    switch (ucHours/10) {
    case 2:
        ulRegister |= (SEVEN_SEGMENT_0_MID | SEVEN_SEGMENT_0_B_L | SEVEN_SEGMENT_0_BOT); // set the tens of hours ('2')
        break;
    }
    WRITE_SLCD(39TO36, ulRegister);                                      // tens of hours (part 2)

    ucHours -= ((ucHours/10) * 10);
    ulRegister = LCD_WF7TO4;
    ulRegister &= ~(SEVEN_SEGMENT_1__7);
    ulRegister |= (ulSegment1[ucHours] & SEVEN_SEGMENT_1__7);            // set the hours
    WRITE_SLCD(7TO4, ulRegister);                                        // hours (part 1)

    ulRegister = LCD_WF11TO8;
    ulRegister &= ~(SEVEN_SEGMENT_1__8);
    ulRegister |= (ulSegment1[ucHours] & SEVEN_SEGMENT_1__8);            // set the hours
    WRITE_SLCD(11TO8, ulRegister);                                       // hours (part 2)

    ulRegister = LCD_WF39TO36;
    ulRegister &= ~(SEVEN_SEGMENT_2_38);
    ulRegister |= (ulSegment2[ucMinutes/10] & SEVEN_SEGMENT_2_38);       // set the tens of minutes
    WRITE_SLCD(39TO36, ulRegister);                                      // tens of minutes (part 1)
    ulRegister = LCD_WF55TO52;
    ulRegister &= ~(SEVEN_SEGMENT_2_53);
    ulRegister |= (ulSegment2[ucMinutes/10] & SEVEN_SEGMENT_2_53);       // set the tens of minutes
    WRITE_SLCD(55TO52, ulRegister);                                      // tens of minutes (part 2)

    ulRegister = LCD_WF11TO8;
    ulRegister &= ~(SEVEN_SEGMENT_3);
    ucMinutes -= ((ucMinutes/10) * 10);
    ulRegister |= ulSegment3[ucMinutes];
    WRITE_SLCD(11TO8, ulRegister);                                       // minutes
    TOGGLE_SLCD(11, SEGMENT_COLON);                                      // toggle ':'
}
#elif defined KWIKSTIK

static const unsigned char ucDigits[12][5] = {
    {0x3e, 0x51, 0x49, 0x45, 0x3e},                                      // '0'
    {0x00, 0x42, 0x7f, 0x40, 0x00},                                      // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46},                                      // '2'
    {0x21, 0x41, 0x45, 0x4b, 0x31},                                      // '3'
    {0x18, 0x14, 0x12, 0x7f, 0x10},                                      // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39},                                      // '5'
    {0x3c, 0x4a, 0x49, 0x49, 0x30},                                      // '6'
    {0x01, 0x71, 0x09, 0x05, 0x03},                                      // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36},                                      // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1e},                                      // '9'
    {0x00, 0x00, 0x00, 0x00, 0x00},                                      // ' '
};

#define CLEAR_DIGIT         10

static const unsigned char ucSmallDigits[11][3] = {
    {0x3e, 0x22, 0x3e},                                                  // '0'
    {0x00, 0x02, 0x3e},                                                  // '1'
    {0x3a, 0x2a, 0x2e},                                                  // '2'
    {0x2a, 0x2a, 0x3e},                                                  // '3'
    {0x0e, 0x08, 0x3e},                                                  // '4'
    {0x2e, 0x2a, 0x3a},                                                  // '5'
    {0x3e, 0x2a, 0x3a},                                                  // '6'
    {0x02, 0x3a, 0x0e},                                                  // '7'
    {0x3e, 0x2a, 0x3e},                                                  // '8'
    {0x2e, 0x2a, 0x3e},                                                  // '9'
    {0x36, 0x00, 0x00},                                                  // ':'
};

#define DOUBLE_POINT_DIGIT  10

// 5 x 7 digit
//
static void fnDrawDigit(int iRegisterOffset, int iShift, unsigned char ucDigit)
{
    unsigned long *ptrRegister = LCD_WF3TO0_ADDR;
    unsigned long ulRegister1, ulRegister2;
    unsigned char *ptrDigitContent;

    ptrRegister += iRegisterOffset;

    ulRegister1 = *ptrRegister++;
    ulRegister2 = *ptrRegister--;

    ptrDigitContent = (unsigned char *)&ucDigits[ucDigit];

    switch (iShift) {
    case 0:
        ulRegister1  = *ptrDigitContent++;
        ulRegister1 |= (*ptrDigitContent++ << 8);
        ulRegister1 |= (*ptrDigitContent++ << 16);
        ulRegister1 |= (*ptrDigitContent++ << 24);
        ulRegister2 &= ~0x000000ff;
        ulRegister2 |= *ptrDigitContent;
        break;
    case 1:
        ulRegister1 &= ~0xffffff00;
        ulRegister1 |= *ptrDigitContent++ << 8;
        ulRegister1 |= (*ptrDigitContent++ << 16);
        ulRegister1 |= (*ptrDigitContent++ << 24);
        ulRegister2 &= ~0x0000ffff;
        ulRegister2 |= *ptrDigitContent++;
        ulRegister2 |= (*ptrDigitContent << 8);
        break;
    case 2:
        ulRegister1 &= ~0xffff0000;
        ulRegister1 |= (*ptrDigitContent++ << 16);
        ulRegister1 |= (*ptrDigitContent++ <<24);
        ulRegister2 &= ~0x00ffffff;
        ulRegister2 |= *ptrDigitContent++;
        ulRegister2 |= (*ptrDigitContent++ << 8);
        ulRegister2 |= (*ptrDigitContent << 16);
        break;
    case 3:
        ulRegister1 &= ~0xff000000;
        ulRegister1 |= (*ptrDigitContent++ << 24);
        ulRegister2  = *ptrDigitContent++;
        ulRegister2 |= (*ptrDigitContent++ << 8);
        ulRegister2 |= (*ptrDigitContent++ << 16);
        ulRegister2 |= (*ptrDigitContent++ << 24);
        break;
    }
    *ptrRegister++ = ulRegister1;
    *ptrRegister = ulRegister2;
}

static void fnDrawSmallDigit(int iRegisterOffset, int iShift, unsigned char ucDigit)
{
    unsigned long *ptrRegister = LCD_WF3TO0_ADDR;
    unsigned long ulRegister1, ulRegister2;
    unsigned char *ptrDigitContent;

    ptrRegister += iRegisterOffset;

    ulRegister1 = *ptrRegister++;
    ulRegister2 = *ptrRegister--;

    ptrDigitContent = (unsigned char *)&ucSmallDigits[ucDigit];

    switch (iShift) {
    case 0:
        ulRegister1 &= ~0x00ffffff;
        ulRegister1  = *ptrDigitContent++;
        ulRegister1 |= (*ptrDigitContent++ << 8);
        ulRegister1 |= (*ptrDigitContent++ << 16);
        *ptrRegister = ulRegister1;
        return;
    case 1:
        ulRegister1 &= ~0xffffff00;
        ulRegister1 |= *ptrDigitContent++ << 8;
        ulRegister1 |= (*ptrDigitContent++ << 16);
        ulRegister1 |= (*ptrDigitContent++ << 24);
        *ptrRegister = ulRegister1;
        return;
    case 2:
        ulRegister1 &= ~0xffff0000;
        ulRegister1 |= (*ptrDigitContent++ << 16);
        ulRegister1 |= (*ptrDigitContent++ <<24);
        ulRegister2 &= ~0x000000ff;
        ulRegister2 |= *ptrDigitContent;
        break;
    case 3:
        ulRegister1 &= ~0xff000000;
        ulRegister1 |= (*ptrDigitContent++ << 24);
        ulRegister2 &= ~0x0000ffff;
        ulRegister2 |= *ptrDigitContent++;
        ulRegister2 |= (*ptrDigitContent++ << 8);
        break;
    }
    *ptrRegister++ = ulRegister1;
    *ptrRegister = ulRegister2;
}

static void fnTimeDisplay(unsigned char ucHours, unsigned char ucMinutes, unsigned char ucSeconds)
{
    if (ucHours < 10) {
        fnDrawDigit(1, 1, CLEAR_DIGIT);
    }
    else {
        fnDrawDigit(0, 2, (unsigned char)(ucHours/10));
    }
    ucHours -= ((ucHours/10) * 10);
    fnDrawDigit(2, 0, ucHours);

    fnDrawSmallDigit(3, 2, DOUBLE_POINT_DIGIT);
    
    fnDrawDigit(4, 0, (unsigned char)(ucMinutes/10));
    ucMinutes -= ((ucMinutes/10) * 10);
    fnDrawDigit(5, 2, ucMinutes);

    fnDrawSmallDigit(7, 0, DOUBLE_POINT_DIGIT);

    fnDrawSmallDigit(7, 2, (unsigned char)(ucSeconds/10));
    ucSeconds -= ((ucSeconds/10) * 10);
    fnDrawSmallDigit(8, 2, ucSeconds);

    #if defined _WINDOWS
    fnSimulateSLCD();
    #endif
}
#endif

static void _rtc_second_interrupt(void)                                  // seconds interrupt call-back handler
{
    RTC_SETUP rtc_setup;
    rtc_setup.command = RTC_GET_TIME;
    fnConfigureRTC(&rtc_setup);                                          // get the present time
    #if defined KINETIS_KL
    if (IS_POWERED_UP(5, SIM_SCGC5_SLCD)) {                              // display time as long as the SLCD has been enabled
        fnTimeDisplay(rtc_setup.ucHours, rtc_setup.ucMinutes, rtc_setup.ucSeconds);
    }
    #else
    if (IS_POWERED_UP(3, SIM_SCGC3_SLCD)) {                              // display time as long as the SLCD has been enabled
        fnTimeDisplay(rtc_setup.ucHours, rtc_setup.ucMinutes, rtc_setup.ucSeconds);
    }
    #endif
}