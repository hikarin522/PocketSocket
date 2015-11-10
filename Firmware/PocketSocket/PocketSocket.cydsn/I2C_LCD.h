/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef I2C_LCD_H
#define I2C_LCD_H

#include <project.h>
#include "SysTick.h"

#define I2C_LCD_NAME I2C_LCD
#define I2C_NAME I2C
	
#define I2C_LCD_ADDRESS 0x3E
#define I2C_LCD_WAIT_TICK (SysTick_ms(2))
#define I2C_LCD_DDRAM_LINE 2
#define I2C_LCD_DDRAM_SIZE 16
#define I2C_LCD_DDRAM_LINE_OFFSET 0x40
	
#define I2C_LCD_MasterWriteBuf(buf, it) (I2C_MasterWriteBuf(I2C_LCD_ADDRESS, (buf), (it), I2C_MODE_COMPLETE_XFER))
#define I2C_LCD_MasterStatus() (I2C_MasterStatus())
#define I2C_LCD_MasterStatusMask (I2C_MSTAT_ERR_XFER | I2C_MSTAT_XFER_INP | I2C_MSTAT_XFER_HALT)

#define I2C_LCD_RS 0x40
#define I2C_LCD_Co 0x80
#define I2C_LCD_IS 0x01

#define ENTRYMODE_DEFAULT IncrementCursor
#define DISPLAYSW_DEFAULT DisplayOnCursorOnBlinkOn
#define FUNCTION_DEFAULT Display2Line
#define FREQUENCY_DEFAULT Frame192Hz
#define BIAS_DEFAULT BiasFrac1_5
#define BOOSTERSW_DEFAULT BoosterOn
#define ICONSW_DEFAULT IconOff
#define CONTRAST_DEFAULT 0x18
#define FOLLOWERSW_DEFAULT FollowerOn
#define GAIN_DEFAULT GAIN1_50

enum I2C_LCD_ENTRYMODE {
	IncrementCursor = 0x06,
	DecrementCursor = 0x04,
	IncrementDisplay = 0x07,
	DecrementDisplay = 0x05
};

enum I2C_LCD_DISPLAYSW {
	DisplayOff = 0x08,
	DisplayOnCursorOffBlinkOff = 0x0C,
	DisplayOnCursorOnBlinkOff = 0x0E,
	DisplayOnCursorOffBlinkOn = 0x0D,
	DisplayOnCursorOnBlinkOn = 0x0F
};

enum I2C_LCD_FUNCTION {
	Display2Line = 0x38,
	Display1LineFontNormal = 0x30,
	Display1LineFontDouble = 0x34
};

enum I2C_LCD_FREQUENCY {
	Frame120Hz = 0x10,
	Frame133Hz = 0x11,
	Frame149Hz = 0x12,
	Frame167Hz = 0x13,
	Frame192Hz = 0x14,
	Frame227Hz = 0x15,
	Frame277Hz = 0x16,
	Frame347Hz = 0x17
};

enum I2C_LCD_BIAS {
	BiasFrac1_4 = 0x18,
	BiasFrac1_5 = 0x10
};

enum I2C_LCD_BOOSTERSW {
	BoosterOff = 0x50,
	BoosterOn = 0x54
};

enum I2C_LCD_ICONSW {
	IconOff = 0x50,
	IconOn = 0x58
};

enum I2C_LCD_FOLLOWERSW {
	FollowerOff = 0x60,
	FollowerOn = 0x68
};

enum I2C_LCD_GAIN {
	GAIN1_00 = 0x60,
	GAIN1_25 = 0x61,
	GAIN1_50 = 0x62,
	GAIN1_80 = 0x63,
	GAIN2_00 = 0x64,
	GAIN2_50 = 0x65,
	GAIN3_00 = 0x66,
	GAIN3_75 = 0x67
};

typedef uint8 I2C_LCD_Font[8];

cystatus I2C_LCD_Start(void);

cystatus I2C_LCD_Main(void);

cystatus I2C_LCD_SetPosition(const uint8, const uint8);

cystatus I2C_LCD_ScrollLine(int8);

cystatus I2C_LCD_PutChar(char);

cystatus I2C_LCD_PutString(const char*);

cystatus I2C_LCD_ClearDisplay(void);

cystatus I2C_LCD_ReturnHome(void);

void I2C_LCD_EntryModeSet(enum I2C_LCD_ENTRYMODE);

void I2C_LCD_DisplaySW(enum I2C_LCD_DISPLAYSW);

void I2C_LCD_FunctionSet(enum I2C_LCD_FUNCTION);

void I2C_LCD_FrequencySet(enum I2C_LCD_FREQUENCY);

void I2C_LCD_BiasSet(enum I2C_LCD_BIAS);

void I2C_LCD_BoosterSW(enum I2C_LCD_BOOSTERSW);

void I2C_LCD_IconSW(enum I2C_LCD_ICONSW);

void I2C_LCD_ContrastSet(uint8 c);

void I2C_LCD_FollowerSW(enum I2C_LCD_FOLLOWERSW);

void I2C_LCD_GainSet(enum I2C_LCD_GAIN);

cystatus I2C_LCD_FontSet(const I2C_LCD_Font, uint8);

#endif // I2C_LCD_H
