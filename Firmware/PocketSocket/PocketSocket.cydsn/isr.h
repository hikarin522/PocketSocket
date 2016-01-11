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

#ifndef ISR_H
#define ISR_H

#include <project.h>
	
extern uint16 vr;

extern int16 vc;
extern int16 ic;
extern int32 lv;
extern int32 li;
	
extern int32 bat_ave;
	
CY_ISR_PROTO(isr_v);
CY_ISR_PROTO(isr_i);
CY_ISR_PROTO(isr_bat);
	
#endif // ISR_H

/* [] END OF FILE */
