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
	
#define DMA_BAT_SIZE 64
extern int16 buf_v[1];
extern int16 buf_i[1];
extern int16 buf_bat[DMA_BAT_SIZE];

void DMA_init(uint16);


#endif // DMA_H

/* [] END OF FILE */
