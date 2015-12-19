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

#include "isr.h"
#include "dma.h"
#include "boost/preprocessor.hpp"

static inline void PWM_control(int32);
static inline void calcW(uint8);

int16 vc = 0;
int16 ic = 0;
int32 wc = 0;
int16 lv = 0;
int16 li = 0;
int32 lw = 0;
int32 bat_ave = 0;

#define V_F 1
#define I_F 2

CY_ISR(isr_v) {
	static uint8 c = 0;
	static int32 _lv = 0;
	vc = *buf_v;
	calcW(V_F);
	_lv += (int32)vc;
	if (++c == 0) {
		lv = _lv >> 8;
		_lv = 0;
	}
}

CY_ISR(isr_i) {
	static uint8 c = 0;
	static int32 _li = 0;
	ic = *buf_i;
	calcW(I_F);
	_li += (int32)ic;
	if (++c == 0) {
		li = _li >> 8;
		_li = 0;
	}
}

CY_ISR(isr_bat) {
	uint8 i;
	int32 bat_sum = 0;
	for (i = 0; i < DMA_BAT_SIZE; ++i) {
		bat_sum += buf_bat[i];
	}
	bat_ave = bat_sum / DMA_BAT_SIZE;
}

static inline void calcW(const uint8 vi) {
	static int32 _lw = 0;
	static uint8 f = 0, c = 0;
	f |= vi;
	if (f == (V_F | I_F)) {
		f = 0;
		wc = (int32)vc * (int32)ic;
		_lw += wc;
		if (++c == 0) {
			lw = _lw >> 8;
			_lw = 0;
		}
		PWM_control(wc);
	}
}

typedef struct {
	uint8 pwm;
	uint8 comp;
} pwm_state;

static inline void PWM_WriteCompare(const pwm_state s) {
#define PWM(x, i, name) BOOST_PP_CAT(PWM_, BOOST_PP_CAT(i, BOOST_PP_CAT(_, name)))
#define PWM_STATE(x, i, t) \
case 2*i:\
	PWM(x, i, WriteCompare1)(s.comp);\
	break;\
case 2*i+1:\
	PWM(x, i, WriteCompare2)(s.comp);\
	break;

	switch (s.pwm) {
		BOOST_PP_REPEAT(12, PWM_STATE, null)
	}
#undef PWM_STATE
#undef PWM
}

static inline pwm_state pwm_add(pwm_state s, int16 count) {
	count += s.comp;
	for (;;) {
		if (count > 25) {
			s.comp = 25;
			PWM_WriteCompare(s);
			if (s.pwm >= 23) {
				return s;
			} else {
				++s.pwm;
				count -= 25;
			}
		} else if (count < 0) {
			s.comp = 0;
			PWM_WriteCompare(s);
			if (s.pwm == 0) {
				return s;
			} else {
				--s.pwm;
				count += 25;
			}
		} else {
			s.comp = count;
			PWM_WriteCompare(s);
			return s;
		}
	}
	return s;
}

static inline pwm_state pwm_inc(pwm_state s) {
	if (s.comp >= 25 && s.pwm >= 23) {
		return s;
	}
	else if (s.comp >= 25 && s.pwm < 23) {
		++s.pwm;
		s.comp = 1;
		PWM_WriteCompare(s);
		return s;
	} else {
		++s.comp;
		PWM_WriteCompare(s);
		return s;
	}
}

static inline pwm_state pwm_dec(pwm_state s) {
	if (s.comp == 0 && s.pwm == 0) {
		return s;
	}
	else if (s.comp == 0 && s.pwm > 0) {
		--s.pwm;
		s.comp = 24;
		PWM_WriteCompare(s);
		return s;
	} else {
		--s.comp;
		PWM_WriteCompare(s);
		return s;
	}
}
/*
#define P 1
static inline void PWM_control2(const int32 w) {
	static uint8 state = 0;
	static pwm_state s = {0, 0};
	static int32 pre = 0, ave = 0;
	static uint16 count = 0;
	
	
}*/

#define SKIP 8
#define AVE 128
#define TH 0x1000
static inline void PWM_control(const int32 w) {
	static uint8 state = 0;
	static pwm_state s = {0, 0};
	static int32 pre = 0;
	static int32 ave = 0;
	static uint16 count = 0;
	
	if (vc < 0x220) {
		s = pwm_dec(s);
		return;
	}
	if (++count < SKIP)
		return;
	ave += w;
	if (++count < SKIP + AVE)
		return;
	count = 0;
	
	switch (state) {
	case 0:
		if (ave >= pre) {
			s = pwm_inc(s);
		} else {
			s = pwm_dec(s);
			state = 1;
		}
		break;
	case 1:
		if (ave >= pre) {
			s = pwm_dec(s);
		} else {
			s = pwm_inc(s);
			state = 0;
		}
		break;
	}
	pre = ave - TH;
	ave = 0;
}

/* [] END OF FILE */
