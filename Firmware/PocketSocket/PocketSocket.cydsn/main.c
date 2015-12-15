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

static inline void PWM_Main(void);

static uint8 vf = 0;
CY_ISR(isr_v) {
	vf |= 1;
}

CY_ISR(isr_i) {
	vf |= 2;
}

CY_ISR(isr_bat) {
	vf |= 4;
}

int main() {
	CyGlobalIntEnable;
	LED_Write(1);
    init();
	CyDelay(500);
	I2C_LCD_PutString("Start test");
	I2C_LCD_SetPosition(1, 0);
	I2C_LCD_PutString("Hello");
	//USB_Printf("Start\n");
	
	SysTick_t timer = SysTick_GetTime();
	uint8 led = 0;
	for (;;) {
		//USB_Main();
		I2C_LCD_Main();
		//PWM_Main();
		if (SysTick_GetInterval(timer) > SysTick_ms(1000)) {
			//const uint8 usb_active = USBUART_CheckActivity();
			//const int16 mv = ADC_V_CountsTo_mVolts(buf_v[0]);
			//const int16 ma = ADC_I_CountsTo_mVolts(buf_i[0]);
			
			char buf1[17] = "test1";
			char buf2[17] = "test2";
			//USB_Printf("%d[mV], %d[mA]\n", mv, ma);
			//I2C_LCD_ClearDisplay();
			//snprintf(buf1, sizeof(buf1), "%4d[mV]", mv);
			//snprintf(buf2, sizeof(buf2), "%4d[mA]", ma);
			I2C_LCD_SetPosition(0, 0);
			I2C_LCD_PutString(buf1);
			I2C_LCD_SetPosition(1, 0);
			I2C_LCD_PutString(buf2);
            if (vf == 0x7)
    			LED_Write(led ^= 1);
            vf = 0;
			timer = SysTick_GetTime();
		}
	}
}

static inline void PWM_Main(void) {
	static SysTick_t timer;
	static uint8 state = -1, comp = 0;
	if (SysTick_GetInterval(timer) < SysTick_ms(250))
		return;
	
    if (comp == 0 && ++state >= 24)
		state = 0;
	if (++comp > 25)
		comp = 0;

#define PWM_STATE(x, i, t) \
case 2*i:\
	PWM(x, i, WriteCompare1)(comp);\
	break;\
case 2*i+1:\
	PWM(x, i, WriteCompare2)(comp);\
	break;

	switch (state) {
		BOOST_PP_REPEAT(12, PWM_STATE, null)
	}
	timer = SysTick_GetTime();
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
	Filter_SetCoherency(Filter_CHANNEL_A, Filter_KEY_MID);
	Filter_SetCoherency(Filter_CHANNEL_B, Filter_KEY_MID);
    Filter_SetDalign(Filter_STAGEA_DALIGN | Filter_HOLDA_DALIGN | Filter_STAGEB_DALIGN | Filter_HOLDB_DALIGN, Filter_ENABLED);
	ADC_Bat_SetCoherency(ADC_Bat_COHER_HIGH);

	DMA_init();
	isr_V_StartEx(isr_v);
	isr_I_StartEx(isr_i);
	isr_Bat_StartEx(isr_bat);
	
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
