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

CY_ISR(isr_v) {
}

CY_ISR(isr_i) {
}

CY_ISR(isr_bat) {
}

int main() {
	CyGlobalIntEnable;
	LED_Write(1);
    init();
	
	I2C_LCD_PutString("Start");
	USB_Printf("Start\n");
	
	SysTick_t timer = SysTick_GetTime();
	SysTick_t tpwm = timer;
	uint8 pwmState = -1;
	uint8 pwmComp = -1;
	uint8 led = 0;
	for (;;) {
		USB_Main();
		I2C_LCD_Main();
		if (SysTick_GetInterval(tpwm) > SysTick_ms(250)) {
			if (++pwmComp > 25) {
				pwmComp = 0;
				if (++pwmState >= 24)
					pwmState = 0;
			}
			
#define PWM_STATE(x, i, t) \
case 2*i:\
	PWM(x, i, WriteCompare1)(pwmComp);\
	break;\
case 2*i+1:\
	PWM(x, i, WriteCompare2)(pwmComp);\
	break;
			switch (pwmState) {
				BOOST_PP_REPEAT(12, PWM_STATE, null)
			}
			tpwm = SysTick_GetTime();
		}
		
		if (SysTick_GetInterval(timer) > SysTick_ms(500)) {
			const uint8 usb_active = USBUART_CheckActivity();
			const int16 mv = ADC_V_CountsTo_mVolts(buf_v[1]);
			const int16 ma = ADC_I_CountsTo_mVolts(buf_i[1]);
			char buf1[17], buf2[17];
			USB_Printf("%d[mV], %d[mA]\n", mv, ma);
			I2C_LCD_ClearDisplay();
			snprintf(buf1, sizeof(buf1), "%4d[mV]", mv);
			snprintf(buf2, sizeof(buf2), "%4d[mA]", ma);
			I2C_LCD_SetPosition(0, 0);
			I2C_LCD_PutString(buf1);
			I2C_LCD_SetPosition(1, 0);
			I2C_LCD_PutString(buf2);
			LED_Write(led ^= 1);
			timer = SysTick_GetTime();
		}
	}
}

static inline void init() {
	VDAC_MAX_Start();
    VDAC_MIN_Start();
    Comp_MAX_Start();
    Comp_MIN_Start();
	
    LPF_V_Start();
	LPF_I_Start();
	ADC_V_Start();
	ADC_I_Start();
	ADC_Bat_Start();
	
	BOOST_PP_REPEAT(12, PWM, Start();)
	
	Filter_Start();
	Filter_SetCoherencyEx(Filter_STAGEA_COHER, Filter_KEY_MID);
	Filter_SetCoherencyEx(Filter_STAGEB_COHER, Filter_KEY_MID);
	Filter_SetCoherencyEx(Filter_HOLDA_COHER, Filter_KEY_MID);
	Filter_SetCoherencyEx(Filter_HOLDB_COHER, Filter_KEY_MID);
	Filter_SetDalign(Filter_STAGEA_DALIGN, Filter_ENABLED);
	Filter_SetDalign(Filter_STAGEB_DALIGN, Filter_ENABLED);
	Filter_SetDalign(Filter_HOLDA_DALIGN, Filter_ENABLED);
	Filter_SetDalign(Filter_HOLDB_DALIGN, Filter_ENABLED);
	
	ADC_Bat_SetCoherency(ADC_Bat_COHER_HIGH);
	DMA_init();

	isr_V_StartEx(isr_v);
	isr_I_StartEx(isr_i);
	isr_Bat_StartEx(isr_bat);
	
	I2C_Start();
	USBUART_Start(0, USBUART_5V_OPERATION);
	
	CyDelay(50);
	I2C_LCD_Start();
	SysTick_Start();
	
	ADC_V_StartConvert();
	ADC_I_StartConvert();
	ADC_Bat_StartConvert();
	
	EN_Write(1);
}


/* [] END OF FILE */
