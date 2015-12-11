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

#ifndef LPF_`$INSTANCE_NAME`_H
#define LPF_`$INSTANCE_NAME`_H

#include "cytypes.h"

`#DECLARE_ENUM LPF_CutoffFreq`
enum `$INSTANCE_NAME`_CUTOFF_FREQ {
	`$INSTANCE_NAME`_15kHz = `$INSTANCE_NAME`__LPF_15KHZ,
	`$INSTANCE_NAME`_30kHz = `$INSTANCE_NAME`__LPF_30KHZ,
	`$INSTANCE_NAME`_76kHz = `$INSTANCE_NAME`__LPF_76KHZ,
	`$INSTANCE_NAME`_153kHz = `$INSTANCE_NAME`__LPF_153KHZ
};
#define `$INSTANCE_NAME`_CR_MASK (`$INSTANCE_NAME`__LPF_15KHZ | `$INSTANCE_NAME`__LPF_30KHZ | `$INSTANCE_NAME`__LPF_76KHZ | `$INSTANCE_NAME`__LPF_153KHZ)

static inline void `$INSTANCE_NAME`_SetCutoffFreq(const enum `$INSTANCE_NAME`_CUTOFF_FREQ cutoff) {
	const void CYFAR *pCR = (void CYFAR*)`$INSTANCE_NAME`_lpf__CR0;
	const uint8 nextCR = (CY_GET_XTND_REG8(pCR) & ~`$INSTANCE_NAME`_CR_MASK) | cutoff;
	CY_SET_XTND_REG8(pCR, nextCR);
}

static inline void `$INSTANCE_NAME`_Start(void) {
	`$INSTANCE_NAME`_SetCutoffFreq((enum `$INSTANCE_NAME`_CUTOFF_FREQ)`$CUTOFF_FREQ`);
}

#endif // LPF_`$INSTANCE_NAME`_H

/* [] END OF FILE */
