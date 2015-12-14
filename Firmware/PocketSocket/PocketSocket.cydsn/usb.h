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

#ifndef USB_H
#define USB_H

#include <project.h>
#include <stdio.h>
#include <stdarg.h>

static uint8 usb_it = 0;
static char8 usb_buf[256];
static inline int USB_Printf(const char *__restrict format, ...) {
	va_list arg;
	va_start(arg, format);
	const int result = vsnprintf(usb_buf + usb_it, sizeof(usb_buf) - 1 - usb_it, format, arg);
	va_end(arg);
	if (result > 0) {
		usb_it += result;
	}
	return result;
}

static inline uint8 USB_Main(void) {
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

#endif // USB_H

/* [] END OF FILE */
