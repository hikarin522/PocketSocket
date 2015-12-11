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

#include "`$INSTANCE_NAME`.h"
#include "`$SYSTICK_NAME`.h"
#include "`$I2C_MASTER_NAME`.h"
#if defined(`$I2C_MASTER_NAME`_SCB_MODE)
    #include "`$I2C_MASTER_NAME`_I2C.h"
#endif // I2C_M_SCB_MODE

#define WriteBuf(buf, it) (`$I2C_MASTER_NAME`_MasterWriteBuf(`$I2C_ADDRESS`, (buf), (it), `$I2C_MASTER_NAME`_MODE_COMPLETE_XFER))
#define Status() (`$I2C_MASTER_NAME`_MasterStatus())
#define StatusMask (`$I2C_MASTER_NAME`_MSTAT_ERR_XFER | `$I2C_MASTER_NAME`_MSTAT_XFER_INP | `$I2C_MASTER_NAME`_MSTAT_XFER_HALT)

#define RS 0x40
#define CO 0x80
#define IS 0x01

#define WAIT_TICK (`$SYSTICK_NAME`_ms(2))

static uint8 str_sts = 0;
static uint8 str_buf[`$DDRAM_LINE`][`$DDRAM_SIZE`];
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
static `$INSTANCE_NAME`_Font font[8];

static uint8 buf[256];

static inline uint8 GetLine(const uint8 it) {
	const uint8 line = line_base + it;
	return line < `$DDRAM_LINE` ? line : line - `$DDRAM_LINE`;
}

inline cystatus `$INSTANCE_NAME`_SetPosition(const uint8 line, const uint8 offset) {
	if (line >= `$DDRAM_LINE` || offset >= `$DDRAM_SIZE`) {
		return CYRET_BAD_PARAM;
	}
	line_it = line;
	str_it = offset;
	return CYRET_SUCCESS;
}

inline cystatus `$INSTANCE_NAME`_ScrollLine(const int8 n) {
	if (n >= `$DDRAM_LINE` || n <= -`$DDRAM_LINE`) {
		return CYRET_BAD_PARAM;
	}
	const uint8 next_base = line_base + (n < 0 ? n + `$DDRAM_LINE` : n);
	line_base = next_base >= `$DDRAM_LINE` ? next_base - `$DDRAM_LINE` : next_base;
	str_sts = 0xFF;
	return CYRET_SUCCESS;
}

inline cystatus `$INSTANCE_NAME`_PutString(const char *str) {
	if (str == NULL || *str == '\0') {
		return CYRET_BAD_PARAM;
	}
	const uint8 line = GetLine(line_it);
	while (*str != '\0' && str_it < `$DDRAM_SIZE`) {
		str_buf[line][str_it++] = *(str++);
	}
	str_sts |= 1 << line;
	return *str == '\0' ? CYRET_SUCCESS : CYRET_MEMORY;
}

inline cystatus `$INSTANCE_NAME`_PutChar(const char ch) {
	if (str_it >= `$DDRAM_SIZE`) {
		return CYRET_MEMORY;
	}
	const uint8 line = GetLine(line_it);
	str_buf[line][str_it++] = ch;
	str_sts |= 1 << line;
	return CYRET_SUCCESS;
}

inline cystatus `$INSTANCE_NAME`_ClearDisplay(void) {
	uint8 i = sizeof(str_buf) - 1;
	do {
		((uint8 *)str_buf)[i] = ' ';
	} while (i--);
	`$INSTANCE_NAME`_SetPosition(0, 0);
	sts.displayReset = 0x01;
	return CYRET_SUCCESS;
}

inline cystatus `$INSTANCE_NAME`_ReturnHome(void) {
	`$INSTANCE_NAME`_SetPosition(0, 0);
	if (sts.displayReset != 0x01) {
		sts.displayReset = 0x02;
	}
	return CYRET_SUCCESS;
}

inline void `$INSTANCE_NAME`_EntryModeSet(const enum `$INSTANCE_NAME`_ENTRYMODE cmd) {
	sts.entryModeSet = cmd;
}

inline void `$INSTANCE_NAME`_DisplaySW(const enum `$INSTANCE_NAME`_DISPLAY cmd) {
	sts.displayOnOff = cmd;
}

inline void `$INSTANCE_NAME`_FunctionSet(const enum `$INSTANCE_NAME`_FUNCTION cmd) {
	sts.functionSet = cmd;
}

inline void `$INSTANCE_NAME`_FrequencySet(const enum `$INSTANCE_NAME`_FREQUENCY f) {
	sts.oscFrequency = (sts.oscFrequency & 0x08) | f;
}

inline void `$INSTANCE_NAME`_BiasSet(const enum `$INSTANCE_NAME`_BIAS cmd) {
	sts.oscFrequency = (sts.oscFrequency & 0x07) | cmd;
}

inline void `$INSTANCE_NAME`_BoosterSW(const enum `$INSTANCE_NAME`_BOOSTER cmd) {
	sts.powerIconContrast = (sts.powerIconContrast & 0x0B) | cmd;
}

inline void `$INSTANCE_NAME`_IconSW(const enum `$INSTANCE_NAME`_ICON cmd) {
	sts.powerIconContrast = (sts.powerIconContrast & 0x07) | cmd;
}

inline void `$INSTANCE_NAME`_ContrastSet(uint8 c) {
	if (c > 0x3F)
		c = 0x3F;
	sts.powerIconContrast = 0x50 | (sts.powerIconContrast & 0x0C) | (c >> 4);
	sts.contrastSet = 0x70 | c;
}

inline void `$INSTANCE_NAME`_FollowerSW(const enum `$INSTANCE_NAME`_FOLLOWER cmd) {
	sts.follower = (sts.follower & 0x07) | cmd;
}

inline void `$INSTANCE_NAME`_GainSet(const enum `$INSTANCE_NAME`_GAIN cmd) {
	sts.follower = (sts.follower & 0x08) | cmd;
}

inline cystatus `$INSTANCE_NAME`_FontSet(const `$INSTANCE_NAME`_Font f, const uint8 ch) {
	if (ch >= (sizeof(font) / sizeof(`$INSTANCE_NAME`_Font))) {
		return CYRET_BAD_PARAM;
	}
	uint8 i = sizeof(`$INSTANCE_NAME`_Font) - 1;
	do {
		font[ch][i] = f[i] & 0x1F;
	} while (i--);
	font_sts |= 1 << ch;
	return CYRET_SUCCESS;
}

static inline uint8 CMD_Push(const uint8 it_base) {
#define PUSH(member) \
	if (sts.member != prests.member) {\
		buf[it++] = CO;\
		buf[it++] = prests.member = sts.member;\
	}

	uint8 it = it_base + 2;
	PUSH(oscFrequency)
	PUSH(contrastSet)
	PUSH(powerIconContrast)
	PUSH(follower)
	if (it > it_base + 2) {
		buf[it_base] = CO;
		buf[it_base + 1] = sts.functionSet | IS;
		buf[it++] = CO;
		buf[it++] = prests.functionSet = sts.functionSet;
	} else {
		it = it_base;
		PUSH(functionSet)
	}
	PUSH(entryModeSet)
	PUSH(displayOnOff)
	return it;

#undef PUSH
}

static inline uint8 Font_Push(const uint8 it_base) {
	uint8 it = it_base;
	if (font_sts) {
		uint8 i, addr = 0xFF, sts = 1;
		for (i = 0; i < (sizeof(font) / sizeof(`$INSTANCE_NAME`_Font)); ++i) {
			if (font_sts & sts) {
				uint8 j;
				if (i != addr - 1) {
					buf[it++] = CO;
					buf[it++] = 0x40 | (i << 3);
				}
				for (j = 0; j < sizeof(`$INSTANCE_NAME`_Font); ++j) {
					buf[it++] = CO | RS;
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

static inline uint8 Str_Push(const uint8 it_base) {
	uint8 it = it_base;
	if (sts.displayReset) {
		buf[it++] = 0;
		buf[it++] = sts.displayReset;
		sts.displayReset = 0;
		return it;
	}
	if (str_sts) {
		uint8 i, addr = 0;
		for (i = 0; i < `$DDRAM_LINE`; ++i) {
			uint8 line = GetLine(i);
			if (str_sts & (1 << line)) {
				uint8 j;
				buf[it++] = CO;
				buf[it++] = 0x80 | addr;
				for (j = 0; j < `$DDRAM_SIZE`; ++j) {
					buf[it++] = CO | RS;
					buf[it++] = str_buf[line][j];
				}
			}
			addr += `$DDRAM_OFFSET`;
		}
		str_sts = 0;
	}
	return it;
}

cystatus `$INSTANCE_NAME`_Main(void) {
	static uint8 state = 0;
	static `$SYSTICK_NAME`_t timer;
	
	switch (state) {
		case 0: {
			if (Status() & StatusMask) {
				return CYRET_SUCCESS;
			}
			timer = `$SYSTICK_NAME`_GetTime();
			++state;
			return CYRET_SUCCESS;
		}
		case 1: {
			if (`$SYSTICK_NAME`_GetInterval(timer) < WAIT_TICK) {
				return CYRET_SUCCESS;
			}
			++state;
		}
		case 2: {
			if (Status() & StatusMask) {
				return CYRET_SUCCESS;
			}
			uint8 it = CMD_Push(0);
			it = Font_Push(it);
			it = Str_Push(it);
			if (it) {
				buf[it - 2] &= ~CO;
				WriteBuf(buf, it);
			}
			state = 0;
			return CYRET_SUCCESS;
		}
		
	}
	state = 0;
	return CYRET_INVALID_STATE;
}

cystatus `$INSTANCE_NAME`_Start(void) {
	uint8 it = 0;
	
	`$INSTANCE_NAME`_EntryModeSet(`$ENTRYMODE`);
	`$INSTANCE_NAME`_DisplaySW(`$DISPLAY`);
	`$INSTANCE_NAME`_FunctionSet(`$FUNCTIONMODE`);
	`$INSTANCE_NAME`_FrequencySet(`$FREQUENCY`);
	`$INSTANCE_NAME`_BiasSet(`$BIAS`);
	`$INSTANCE_NAME`_BoosterSW(`$BOOSTER`);
	`$INSTANCE_NAME`_IconSW(`$ICON`);
	`$INSTANCE_NAME`_ContrastSet(`$CONTRAST`);
	`$INSTANCE_NAME`_FollowerSW(`$FOLLOWER`);
	`$INSTANCE_NAME`_GainSet(`$GAIN`);
	`$INSTANCE_NAME`_ClearDisplay();
	
	buf[it++] = CO;
	buf[it++] = sts.functionSet;
	buf[it++] = CO;
	buf[it++] = sts.functionSet | IS;
	buf[it++] = CO;
	buf[it++] = sts.oscFrequency;
	buf[it++] = CO;
	buf[it++] = sts.contrastSet;
	buf[it++] = CO;
	buf[it++] = sts.powerIconContrast;
	buf[it++] = CO;
	buf[it++] = sts.follower;
	buf[it++] = CO;
	buf[it++] = sts.functionSet;
	buf[it++] = CO;
	buf[it++] = sts.entryModeSet;
	buf[it++] = CO;
	buf[it++] = sts.displayOnOff;
	buf[it++] = 0;
	buf[it++] = sts.displayReset;
	prests = sts;

	return WriteBuf(buf, it);
}

/* [] END OF FILE */
