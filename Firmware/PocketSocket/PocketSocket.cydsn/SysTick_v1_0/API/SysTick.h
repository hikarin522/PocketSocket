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

#ifndef SysTick_`$INSTANCE_NAME`_H
#define SysTick_`$INSTANCE_NAME`_H

#include "cytypes.h"
#include "CyLib.h"
#include "core_cm3_psoc5.h"

`#DECLARE_ENUM SysTick_CLKSOURCE`

#if `$CLOCK_SOURCE` == `$INSTANCE_NAME`__SysTick_ILO
	#define `$INSTANCE_NAME`_CLKSOURCE_Msk 0
#elif `$CLOCK_SOURCE` == `$INSTANCE_NAME`__SysTick_BUS_CLK
	#define `$INSTANCE_NAME`_CLKSOURCE_Msk SysTick_CTRL_CLKSOURCE_Msk;
#else
	#error "`$INSTANCE_NAME`: CLOCK_SOURCE is invalid value `$CLOCK_SOURCE`"
#endif

typedef uint32_t `$INSTANCE_NAME`_t;

static inline void `$INSTANCE_NAME`_Start() {
	SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
	SysTick->VAL  = 0;
	SysTick->CTRL = `$INSTANCE_NAME`_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

static inline cystatus `$INSTANCE_NAME`_StartEx(const `$INSTANCE_NAME`_t ticks, const cyisraddress address) {
	if ((ticks - 1) > SysTick_LOAD_RELOAD_Msk)
		return CYRET_BAD_PARAM;
	CyIntSetSysVector(SysTick_IRQn + 16, address);
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	SysTick->LOAD = ticks - 1;
	SysTick->VAL  = 0;
	SysTick->CTRL = `$INSTANCE_NAME`_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	return CYRET_SUCCESS;
}

static inline `$INSTANCE_NAME`_t `$INSTANCE_NAME`_ns(const uint32_t val) {
	return val * (`$FREQUENCY` / (1000 * 1000 * 1000));
}

static inline `$INSTANCE_NAME`_t `$INSTANCE_NAME`_us(const uint32_t val) {
	return val * (`$FREQUENCY` / (1000 * 1000));
}

static inline `$INSTANCE_NAME`_t `$INSTANCE_NAME`_ms(const uint32_t val) {
	return val * (`$FREQUENCY` / 1000);
}

static inline `$INSTANCE_NAME`_t `$INSTANCE_NAME`_s(const uint32_t val) {
	return val * `$FREQUENCY`;
}

static inline `$INSTANCE_NAME`_t `$INSTANCE_NAME`_GetTime() {
	return SysTick->VAL;
}

static inline `$INSTANCE_NAME`_t `$INSTANCE_NAME`_GetInterval(const `$INSTANCE_NAME`_t time) {
	return (time - SysTick->VAL) & SysTick_VAL_CURRENT_Msk;
}
	
#endif // SysTick_`$INSTANCE_NAME`_H

/* [] END OF FILE */
