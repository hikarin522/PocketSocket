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

#include "I2C_LCD.h"

static uint8 str_sts = 0;
static uint8 str_buf[I2C_LCD_DDRAM_LINE][I2C_LCD_DDRAM_SIZE];
static uint8 line_base = 0;
static uint8 line_it = 0;
static uint8 str_it = 0;

static struct {
	uint8 displayReset;
	uint8 entryModeSet;
	uint8 displayOnOff;
	uint8 functionSet;
	uint8 oscFrequency;
	uint8 powerIconContrast;
	uint8 contrastSet;
	uint8 follower;
} sts, prests;

static uint8 font_sts = 0;
static I2C_LCD_Font font[8];

static uint8 buf[256];

__STATIC_INLINE uint8 GetLine(const uint8 it) {
	const uint8 line = line_base + it;
	return line < I2C_LCD_DDRAM_LINE ? line : line - I2C_LCD_DDRAM_LINE;
}

__INLINE cystatus I2C_LCD_SetPosition(const uint8 line, const uint8 offset) {
	if (line >= I2C_LCD_DDRAM_LINE || offset >= I2C_LCD_DDRAM_SIZE) {
		return CYRET_BAD_PARAM;
}
	line_it = line;
	str_it = offset;
	return CYRET_SUCCESS;
}

__INLINE cystatus I2C_LCD_ScrollLine(const int8 n) {
	if (n >= I2C_LCD_DDRAM_LINE || n <= -I2C_LCD_DDRAM_LINE) {
		return CYRET_BAD_PARAM;
	}
	const uint8 next_base = line_base + (n < 0 ? n + I2C_LCD_DDRAM_LINE : n);
	line_base = next_base >= I2C_LCD_DDRAM_LINE ? next_base - I2C_LCD_DDRAM_LINE : next_base;
	str_sts = 0xFF;
	return CYRET_SUCCESS;
}

__INLINE cystatus I2C_LCD_PutString(const char *str) {
	if (str == NULL || *str == '\0') {
		return CYRET_BAD_PARAM;
	}
	const uint8 line = GetLine(line_it);
	while (*str != '\0' && str_it < I2C_LCD_DDRAM_SIZE) {
		str_buf[line][str_it++] = *(str++);
	}
	str_sts |= 1 << line;
	return *str == '\0' ? CYRET_SUCCESS : CYRET_MEMORY;
}

__INLINE cystatus I2C_LCD_PutChar(const char ch) {
	if (str_it >= I2C_LCD_DDRAM_SIZE) {
		return CYRET_MEMORY;
	}
	const uint8 line = GetLine(line_it);
	str_buf[line][str_it++] = ch;
	str_sts |= 1 << line;
	return CYRET_SUCCESS;
}

__INLINE cystatus I2C_LCD_ClearDisplay(void) {
	uint8 i = sizeof(str_buf) - 1;
	do {
		((uint8 *)str_buf)[i] = ' ';
	} while (i--);
	I2C_LCD_SetPosition(0, 0);
	sts.displayReset = 0x01;
	return CYRET_SUCCESS;
}

__INLINE cystatus I2C_LCD_ReturnHome(void) {
	I2C_LCD_SetPosition(0, 0);
	if (sts.displayReset != 0x01) {
		sts.displayReset = 0x02;
	}
	return CYRET_SUCCESS;
}

__INLINE void I2C_LCD_EntryModeSet(const enum I2C_LCD_ENTRYMODE cmd) {
	sts.entryModeSet = cmd;
}

__INLINE void I2C_LCD_DisplaySW(const enum I2C_LCD_DISPLAYSW cmd) {
	sts.displayOnOff = cmd;
}

__INLINE void I2C_LCD_FunctionSet(const enum I2C_LCD_FUNCTION cmd) {
	sts.functionSet = cmd;
}

__INLINE void I2C_LCD_FrequencySet(const enum I2C_LCD_FREQUENCY f) {
	sts.oscFrequency = (sts.oscFrequency & 0x08) | f;
}

__INLINE void I2C_LCD_BiasSet(const enum I2C_LCD_BIAS cmd) {
	sts.oscFrequency = (sts.oscFrequency & 0x07) | cmd;
}

__INLINE void I2C_LCD_BoosterSW(const enum I2C_LCD_BOOSTERSW cmd) {
	sts.powerIconContrast = (sts.powerIconContrast & 0x0B) | cmd;
}

__INLINE void I2C_LCD_IconSW(const enum I2C_LCD_ICONSW cmd) {
	sts.powerIconContrast = (sts.powerIconContrast & 0x07) | cmd;
}

__INLINE void I2C_LCD_ContrastSet(uint8 c) {
	if (c > 0x3F)
		c = 0x3F;
	sts.powerIconContrast = 0x50 | (sts.powerIconContrast & 0x0C) | (c >> 4);
	sts.contrastSet = 0x70 | c;
}

__INLINE void I2C_LCD_FollowerSW(const enum I2C_LCD_FOLLOWERSW cmd) {
	sts.follower = (sts.follower & 0x07) | cmd;
}

__INLINE void I2C_LCD_GainSet(const enum I2C_LCD_GAIN cmd) {
	sts.follower = (sts.follower & 0x08) | cmd;
}

__INLINE cystatus I2C_LCD_FontSet(const I2C_LCD_Font f, const uint8 ch) {
	if (ch >= (sizeof(font) / sizeof(I2C_LCD_Font))) {
		return CYRET_BAD_PARAM;
	}
	uint8 i = sizeof(I2C_LCD_Font) - 1;
	do {
		font[ch][i] = f[i] & 0x1F;
	} while (i--);
	font_sts |= 1 << ch;
	return CYRET_SUCCESS;
}

__STATIC_INLINE uint8 I2C_LCD_CMD_Push(const uint8 it_base) {
#define I2C_LCD_PUSH(member) \
	if (sts.member != prests.member) {\
		buf[it++] = I2C_LCD_Co;\
		buf[it++] = prests.member = sts.member;\
	}

	uint8 it = it_base + 2;
	I2C_LCD_PUSH(oscFrequency)
	I2C_LCD_PUSH(contrastSet)
	I2C_LCD_PUSH(powerIconContrast)
	I2C_LCD_PUSH(follower)
	if (it > it_base + 2) {
		buf[it_base] = I2C_LCD_Co;
		buf[it_base + 1] = sts.functionSet | I2C_LCD_IS;
		buf[it++] = I2C_LCD_Co;
		buf[it++] = prests.functionSet = sts.functionSet;
	} else {
		it = it_base;
		I2C_LCD_PUSH(functionSet)
	}
	I2C_LCD_PUSH(entryModeSet)
	I2C_LCD_PUSH(displayOnOff)
	return it;

#undef I2C_LCD_PUSH
}

__STATIC_INLINE uint8 I2C_LCD_Font_Push(const uint8 it_base) {
	uint8 it = it_base;
	if (font_sts) {
		uint8 i, addr = 0xFF, sts = 1;
		for (i = 0; i < (sizeof(font) / sizeof(I2C_LCD_Font)); ++i) {
			if (font_sts & sts) {
				uint8 j;
				if (i != addr - 1) {
					buf[it++] = I2C_LCD_Co;
					buf[it++] = 0x40 | (i << 3);
				}
				for (j = 0; j < sizeof(I2C_LCD_Font); ++j) {
					buf[it++] = I2C_LCD_Co | I2C_LCD_RS;
					buf[it++] = font[i][j];
				}
				addr = i;
			}
			sts <<= 1;
		}
		font_sts = 0;
	}
	return it;
}

__STATIC_INLINE uint8 I2C_LCD_Str_Push(const uint8 it_base) {
	uint8 it = it_base;
	if (sts.displayReset) {
		buf[it++] = 0;
		buf[it++] = sts.displayReset;
		sts.displayReset = 0;
		return it;
	}
	if (str_sts) {
		uint8 i, addr = 0;
		for (i = 0; i < I2C_LCD_DDRAM_LINE; ++i) {
			uint8 line = GetLine(i);
			if (str_sts & (1 << line)) {
				uint8 j;
				buf[it++] = I2C_LCD_Co;
				buf[it++] = 0x80 | addr;
				for (j = 0; j < I2C_LCD_DDRAM_SIZE; ++j) {
					buf[it++] = I2C_LCD_Co | I2C_LCD_RS;
					buf[it++] = str_buf[line][j];
				}
			}
			addr += I2C_LCD_DDRAM_LINE_OFFSET;
		}
		str_sts = 0;
	}
	return it;
}

cystatus I2C_LCD_Main(void) {
	static uint8 state = 0;
	static SysTick_t timer;
	
	switch (state) {
		case 0: {
			if (I2C_LCD_MasterStatus() & I2C_LCD_MasterStatusMask) {
				return CYRET_SUCCESS;
			}
			timer = SysTick_GetTime();
			++state;
			return CYRET_SUCCESS;
		}
		case 1: {
			if (SysTick_GetInterval(timer) < I2C_LCD_WAIT_TICK) {
				return CYRET_SUCCESS;
			}
			++state;
		}
		case 2: {
			if (I2C_LCD_MasterStatus() & I2C_LCD_MasterStatusMask) {
				return CYRET_SUCCESS;
			}
			uint8 it = I2C_LCD_CMD_Push(0);
			it = I2C_LCD_Font_Push(it);
			it = I2C_LCD_Str_Push(it);
			if (it) {
				buf[it - 2] &= ~I2C_LCD_Co;
				I2C_LCD_MasterWriteBuf(buf, it);
			}
			state = 0;
			return CYRET_SUCCESS;
		}
		
	}
	state = 0;
	return CYRET_INVALID_STATE;
}

cystatus I2C_LCD_Start(void) {
	uint8 it = 0;
	
	I2C_LCD_EntryModeSet(ENTRYMODE_DEFAULT);
	I2C_LCD_DisplaySW(DISPLAYSW_DEFAULT);
	I2C_LCD_FunctionSet(FUNCTION_DEFAULT);
	I2C_LCD_FrequencySet(FREQUENCY_DEFAULT);
	I2C_LCD_BiasSet(BIAS_DEFAULT);
	I2C_LCD_BoosterSW(BOOSTERSW_DEFAULT);
	I2C_LCD_IconSW(ICONSW_DEFAULT);
	I2C_LCD_ContrastSet(CONTRAST_DEFAULT);
	I2C_LCD_FollowerSW(FOLLOWERSW_DEFAULT);
	I2C_LCD_GainSet(GAIN_DEFAULT);
	I2C_LCD_ClearDisplay();
	
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.functionSet;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.functionSet | I2C_LCD_IS;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.oscFrequency;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.contrastSet;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.powerIconContrast;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.follower;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.functionSet;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.entryModeSet;
	buf[it++] = I2C_LCD_Co;
	buf[it++] = sts.displayOnOff;
	buf[it++] = 0;
	buf[it++] = sts.displayReset;
	prests = sts;

	return I2C_LCD_MasterWriteBuf(buf, it);
}
