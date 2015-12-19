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

extern int16 vc;
extern int16 ic;
extern int32 wc;
extern int16 lv;
extern int16 li;
extern int32 lw;
extern int32 bat_ave;
	
CY_ISR_PROTO(isr_v);
CY_ISR_PROTO(isr_i);
CY_ISR_PROTO(isr_bat);
	
#endif // ISR_H

/* [] END OF FILE */
