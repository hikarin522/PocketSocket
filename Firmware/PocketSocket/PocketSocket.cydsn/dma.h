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

#ifndef DMA_H
#define DMA_H

#include <project.h>

extern int16 vc;
extern int16 ic;
extern int32 wc;
extern int16 lv;
extern int16 li;
extern int32 lw;

void DMA_init(uint16);
void PWM_control(int32 w);


#endif // DMA_H

/* [] END OF FILE */
