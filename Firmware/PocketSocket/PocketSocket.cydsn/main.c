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

#include <project.h>
#include <stdio.h>
#include <stdarg.h>
#include "boost/preprocessor.hpp"
#include "dma.h"
#include "usb.h"

#define PWM(x, i, name) BOOST_PP_CAT(PWM_, BOOST_PP_CAT(i, BOOST_PP_CAT(_, name)))
static inline void init(void);

static inline int format(char[17], int16, int16);
uint8 vf;
int main() {
	CyGlobalIntEnable;
	LED_Write(1);
    init();
	CyDelay(500);
	I2C_LCD_PutString("Start test");
	//USB_Printf("Start\n");
	
	SysTick_t timer = SysTick_GetTime();
	uint8 led = 0;
	for (;;) {
		//USB_Main();
		I2C_LCD_Main();
		
		if (SysTick_GetInterval(timer) > SysTick_ms(250)) {
			//const uint8 usb_active = USBUART_CheckActivity();
			//const int16 bat = ADC_Bat_CountsTo_mVolts(bat_res);
			//USB_Printf("%d[mV], %d[mA]\n", mv, ma);
			I2C_LCD_ClearDisplay();
			
			const int16 mv = ADC_V_CountsTo_mVolts(vc) * 3;
			const int16 ma = ADC_I_CountsTo_mVolts(ic);
			char buf1[17], buf2[17];
			format(buf1, mv, ma);
			snprintf(buf2, sizeof(buf2), "%04x %04x %04x", 0, lv, li);
			I2C_LCD_SetPosition(0, 0);
			I2C_LCD_PutString(buf1);
			I2C_LCD_SetPosition(1, 0);
			I2C_LCD_PutString(buf2);
            if (vf & 0x4) {
    			LED_Write(led ^= 1);
            	vf = 0;
			}
			timer = SysTick_GetTime();
		}
	}
}

static inline int format(char str[17], const int16 mv, const int16 ma) {
	char mv_str[7], ma_str[7];
	snprintf(mv_str, sizeof(mv_str), "%+06d", mv);
	snprintf(ma_str, sizeof(ma_str), "%+06d", ma);
	uint8 i;
	for (i = 1; i < 3 && mv_str[i] == '0'; i++)
		mv_str[i] = ' ';
	if (mv_str[0] == '-')
		mv_str[i - 1] = '-';
	for (i = 1; i < 4 && ma_str[i] == '0'; i++)
		ma_str[i] = ' ';
	if (ma_str[0] == '-')
		ma_str[i - 1] = '-';
	return snprintf(str, 17, "%3.3s.%2.2sV %4.4s.%cmA", mv_str + 1, mv_str + 4, ma_str + 1, ma_str[5]);
}

typedef struct {
	uint8 pwm;
	uint8 comp;
} pwm_state;

static inline void PWM_WriteCompare(const pwm_state s) {
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

void PWM_control(const int32 w) {
	static uint8 state = 0;
	static pwm_state s = {0, 0};
	static int32 pre = 0, next = 0;
	
	switch (state) {
	case 0:
		pre = w;
		pwm_inc(s);
		state = 1;
		break;
	case 2:
		next = w;
		pwm_dec(s);
		state = 3;
		break;
	case 4:
		if (w > pre && w > next) {
			s = pwm_dec(s);
		} else if (next > w && next > pre) {
			s = pwm_inc(s);
		} else {
			PWM_WriteCompare(s);
		}
		state = -1;
		break;
	default:
		state++;
	}
	
	vf = 7;
}

static inline void init() {
	VDAC_MAX_Start();
    VDAC_MIN_Start();
    Comp_MAX_Start();
    Comp_MIN_Start();
	
	BOOST_PP_REPEAT(12, PWM, Start();)
	
    LPF_V_Start();
	LPF_I_Start();
	ADC_V_Start();
	ADC_I_Start();
	ADC_Bat_Start();
	
	Filter_Start();
	Filter_SetCoherencyEx(Filter_STAGEA_COHER | Filter_HOLDA_COHER | Filter_STAGEB_COHER | Filter_HOLDB_COHER, Filter_KEY_MID);
    Filter_SetDalign(Filter_STAGEA_DALIGN | Filter_HOLDA_DALIGN | Filter_STAGEB_DALIGN | Filter_HOLDB_DALIGN, Filter_ENABLED);
	ADC_Bat_SetCoherency(ADC_Bat_COHER_MID);

	DMA_init(60);

	I2C_Start();
	USBUART_Start(0, USBUART_5V_OPERATION);
	
	CyDelay(100);
	I2C_LCD_Start();
	
	SysTick_Start();
	
	ADC_V_StartConvert();
	ADC_I_StartConvert();
	ADC_Bat_StartConvert();
	
	EN_Write(1);
}


/* [] END OF FILE */
