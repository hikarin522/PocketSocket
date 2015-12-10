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

static uint8 USB_Main(void);
static int USB_Printf(const char *__restrict, ...);

int main()
{
	CyGlobalIntEnable;
	LED_Write(1);
    
    VDAC_MAX_Start();
    VDAC_MIN_Start();
    Comp_MAX_Start();
    Comp_MIN_Start();
    
	ADC_V_Start();
	ADC_I_Start();
	ADC_B_Start();
	I2C_Start();
	USBUART_Start(0, USBUART_5V_OPERATION);
	
#define PWM(x, i, name) BOOST_PP_CAT(PWM_, BOOST_PP_CAT(i, BOOST_PP_CAT(_, name)))
	BOOST_PP_REPEAT(12, PWM, Start();)
	
	ADC_V_StartConvert();
	ADC_I_StartConvert();
	ADC_B_StartConvert();
	
	CyDelay(100);
	SysTick_Start();
	I2C_LCD_Start();
	I2C_LCD_PutString("Start\n");
	
	EN_Write(1);
	
	SysTick_t timer = SysTick_GetTime();
	uint16 v = 0, i = 0;
	uint8 led = 0;
	for (;;) {
		USB_Main();
		I2C_LCD_Main();
		
		if (ADC_V_IsEndConversion(ADC_V_RETURN_STATUS)) {
			v = ADC_V_GetResult16();
		}
		if (ADC_I_IsEndConversion(ADC_I_RETURN_STATUS)) {
			i = ADC_I_GetResult16();
		}
		if (SysTick_GetInterval(timer) > SysTick_ms(500)) {
			const uint8 usb_active = USBUART_CheckActivity();
			const int16 mv = ADC_V_CountsTo_mVolts(v);
			const int16 ma = ADC_I_CountsTo_mVolts(i);
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

static uint8 usb_it = 0;
static char8 usb_buf[256];
static int USB_Printf(const char *__restrict format, ...) {
	va_list arg;
	va_start(arg, format);
	const int result = vsnprintf(usb_buf + usb_it, sizeof(usb_buf) - 1 - usb_it, format, arg);
	va_end(arg);
	if (result > 0) {
		usb_it += result;
	}
	return result;
}

static uint8 USB_Main(void) {
	static uint8 state = 0;
	
	switch (state) {
		case 0: {
			if (USBUART_GetConfiguration() == 0) {
				return state;
			}
			USBUART_CDC_Init();
			++state;
			return 1;
		}
		case 1: {
			if (USBUART_IsConfigurationChanged()) {
				return state = 0;
			}
			if (!USBUART_CDCIsReady() || usb_it == 0) {
				return state;
			}
			usb_it = 0;
			USBUART_PutString(usb_buf);
			return state;
		}
	}
	
	return state = 0;
}

/* [] END OF FILE */
