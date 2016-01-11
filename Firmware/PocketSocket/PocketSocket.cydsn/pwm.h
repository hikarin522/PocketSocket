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

#ifndef PWM_H
#define PWM_H

#include "Project.h"

typedef struct {
	uint8 pwm;
	uint8 comp;
} pwm_state;

typedef void (*WriteCompF)(uint8);
extern const WriteCompF PWM_WriteComp[24];

pwm_state pwm_inc(pwm_state);
pwm_state pwm_dec(pwm_state);
pwm_state pwm_add(pwm_state, int16);
pwm_state pwm_set(pwm_state, int16);
pwm_state createState(uint8, uint8);
#endif // PWM_H

/* [] END OF FILE */
