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
#include "pwm.h"
#include "boost/preprocessor.hpp"

static inline void calcW(uint8, int32);

int32 bat_ave = 0;
uint16 vr = 2;

int16 vc;
int16 ic;
int32 lv;
int32 li;

#define V_F 1
#define I_F 2

#define BUF_SIZE 1
#define SKIP_COUNT 0
CY_ISR(isr_v) {
	static uint8 it = 0;
	static int16 buf[BUF_SIZE] = {0};
	static int32 sum = 0;
	static uint8 count = 0;
	
	sum -= buf[it];
	buf[it] = buf_v[0];
	sum += buf[it];
	if (++it >= BUF_SIZE) {
		it = 0;
	}
	
	if (count++ >= SKIP_COUNT) {
		count = 0;
		calcW(V_F, sum);
		lv = sum; 
		vc = sum;
	}
}

CY_ISR(isr_i) {
	static uint8 it = 0;
	static int16 buf[BUF_SIZE] = {0};
	static int32 sum = 0;
	static uint8 count = 0;
	
	sum -= buf[it];
	buf[it] = buf_i[0];
	sum += buf[it];
	if (++it >= BUF_SIZE) {
		it = 0;
	}
	
	if (count++ >= SKIP_COUNT) {
		count = 0;
		calcW(I_F, sum);
		li = sum;
		ic = sum;
	}
}

CY_ISR(isr_bat) {
	/*
	uint8 i;
	int32 bat_sum = 0;
	for (i = 0; i < DMA_BAT_SIZE; ++i) {
		bat_sum += buf_bat[i];
	}
	bat_ave = bat_sum / DMA_BAT_SIZE;
	*/
}

static inline void calcW(const uint8 sel, const int32 val) {
	static int32 v_sum = 0;
	static int32 i_sum = 0;
	static pwm_state state = {0, 0};
	
	if (sel == V_F) {
		v_sum = val;
	} else if (sel == I_F) {
		if (val < 0x10) {
			state = pwm_add(state, -25);
		}
		i_sum = val;
	}

	 static uint8 f = 0;
	
	f |= sel;
	if (f == (V_F | I_F)) {
		f = 0;
		state = pwm_set(state, (vr * v_sum) >> 4);
		/*
		const int32 w_sum = v_sum * i_sum;
		const int32 e = v_sum - vr * i_sum;
		if (e >= 0) {
			state = pwm_inc(state);
		} else {
			state = pwm_dec(state);
		}
		*/
	}
}

/* [] END OF FILE */
