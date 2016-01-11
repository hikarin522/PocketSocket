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

#define LED_TEST 0

#include <project.h>
#include <stdio.h>
#include <stdarg.h>
#include "boost/preprocessor.hpp"
#include "dma.h"
#include "usb.h"
#include "isr.h"
#include "pwm.h"

#define PWM(x, i, name) BOOST_PP_CAT(PWM_, BOOST_PP_CAT(i, BOOST_PP_CAT(_, name)))
static inline void init(void);
static inline int format(char[17], int32, int32);

int main() {
	CyGlobalIntEnable;
	LED_Write(1);
    init();
	CyDelay(500);
	I2C_LCD_PutString("Start test");
	//USB_Printf("Start\n");
	
	SysTick_t timer = SysTick_GetTime();
	uint8 led = 0;
	LED_Write(0);
	uint8 sw_f = 1;
	
#if LED_TEST == 1
	uint8 pwm = 0;

	for (;;) {
		
		if (sw_f && SW_Read()) {
			sw_f = 0;
			uint8 i;
			for (i = 0; i < 24; i++) {
				PWM_WriteComp[i](pwm);
			}
			if (++pwm > 25) {
				pwm = 0;
			}
		} else if (!sw_f && !SW_Read()) {
			sw_f = 1;
		}
		if (SysTick_GetInterval(timer) > SysTick_ms(500)) {
				LED_Write(led ^= 1);
			timer = SysTick_GetTime();
		}
	}
#elif LED_TEST == 2
	for (;;) {
		uint8 i;
		for (i = 0; i < 24; i++) {
			PWM_WriteComp[i](1);
			LED_Write(led ^= 1);
			CyDelay(1000);
			PWM_WriteComp[i](0);
		}
	}
#else
	for (;;) {
		USB_Main();
		I2C_LCD_Main();
		
		if (sw_f && SW_Read()) {
			sw_f = 0;
			++vr;
			if (vr > 16) {
				vr = 1;
			}
		} else if (!sw_f && !SW_Read()) {
			sw_f = 1;
		}
		
		
		
		if (SysTick_GetInterval(timer) > SysTick_ms(500)) {
			if (!EN_Read()) {
				EN_Reg_Write(1);
			}
			//const uint8 usb_active = USBUART_CheckActivity();
			//const int16 bat = ADC_Bat_CountsTo_mVolts(bat_res);
			//USB_Printf("%d[mV], %d[mA]\n", mv, ma);
			I2C_LCD_ClearDisplay();
			
			// x186.2/6.2 = 931/31
			//const int64 tv = (int64)ADC_V_CountsTo_mVolts(vc) * 931;
			//const int32 mv = (tv >> 5) + (tv >> 10) + (tv >> 15) + (tv >> 20);
			const int32 mv = (int64)ADC_V_CountsTo_mVolts(vc) * 931 / 31;

			const int16 ma = ADC_I_CountsTo_mVolts(ic);
			const int16 bat = ADC_Bat_CountsTo_mVolts(bat_ave);
			char buf1[17], buf2[17];
			format(buf1, mv, ma);
			snprintf(buf2, sizeof(buf2), "%08lx%08lx", lv, li);
			I2C_LCD_SetPosition(0, 0);
			I2C_LCD_PutString(buf1);
			I2C_LCD_SetPosition(1, 0);
			I2C_LCD_PutString(buf2);
    		LED_Write(led ^= 1);
			timer = SysTick_GetTime();
		}
	}
#endif // LED_TEST
}

static inline int format(char str[17], const int32 mv, const int32 ma) {
	char mv_str[8], ma_str[7];
	snprintf(mv_str, sizeof(mv_str), "%+07ld", mv);
	snprintf(ma_str, sizeof(ma_str), "%+06ld", ma);
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

static inline void init() {
	VDAC_I_Start();
    VDAC_V_Start();
    Comp_I_Start();
    Comp_V_Start();
	
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

	
#if !LED_TEST
	DMA_init(4);
	isr_V_StartEx(isr_v);
	isr_I_StartEx(isr_i);
	isr_Bat_StartEx(isr_bat);
#endif
	I2C_Start();
	USBUART_Start(0, USBUART_5V_OPERATION);
	
	CyDelay(100);
	I2C_LCD_Start();
	
	SysTick_Start();

#if !LED_TEST
	ADC_V_StartConvert();
	ADC_I_StartConvert();
	ADC_Bat_StartConvert();
#endif
	EN_Reg_Write(1);
}


/* [] END OF FILE */
