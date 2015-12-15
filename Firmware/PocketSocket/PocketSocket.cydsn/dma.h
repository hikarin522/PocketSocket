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

#define DMA_BUF_SIZE 64
#define DMA_BAT_SIZE 32

extern uint16 buf_v[DMA_BUF_SIZE];
extern uint16 buf_i[DMA_BUF_SIZE];
extern uint32 buf_bat[DMA_BAT_SIZE];

void DMA_init(void);


#endif // DMA_H

/* [] END OF FILE */
