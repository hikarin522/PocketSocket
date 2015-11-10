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

#ifndef SYSTICK_H
#define SYSTICK_H

#include <project.h>

#ifndef SysTick_FREQ
	#define SysTick_FREQ 100000
	#warning Please Global Define "SysTick_FREQ"
	#warning "SysTick_FREQ" = 100000 (100kHz)
#endif

typedef uint32_t SysTick_t;

enum SysTick_CLKSOURCE {
	SysTick_ILO = 0,
	SysTick_BUS_CLK = SysTick_CTRL_CLKSOURCE_Msk
};

__STATIC_INLINE void SysTick_Start(const enum SysTick_CLKSOURCE src) {
	SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | src;
}

__STATIC_INLINE cystatus SysTick_StartEx(const SysTick_t ticks, const cyisraddress address) {
	CyIntSetSysVector(SysTick_IRQn + 16, address);
	return SysTick_Config(ticks);
}

__STATIC_INLINE SysTick_t SysTick_ns(const uint32_t val) {
	return val * (SysTick_FREQ / (1000 * 1000 * 1000));
}

__STATIC_INLINE SysTick_t SysTick_us(const uint32_t val) {
	return val * (SysTick_FREQ / (1000 * 1000));
}

__STATIC_INLINE SysTick_t SysTick_ms(const uint32_t val) {
	return val * (SysTick_FREQ / 1000);
}

__STATIC_INLINE SysTick_t SysTick_s(const uint32_t val) {
	return val * SysTick_FREQ;
}

__STATIC_INLINE SysTick_t SysTick_GetTime() {
	return SysTick->VAL;
}

__STATIC_INLINE SysTick_t SysTick_GetInterval(const SysTick_t time) {
	return (time - SysTick->VAL) & SysTick_VAL_CURRENT_Msk;
}

#endif // SYSTICK_H

/* [] END OF FILE */
