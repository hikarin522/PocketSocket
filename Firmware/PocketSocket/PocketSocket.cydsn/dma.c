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

#include "dma.h"

int16 buf_v[1];
int16 buf_i[1];
static int16 dma_ignore[1];
int16 buf_bat[2][DMA_BAT_SIZE];

/* Defines for DMA_V */
#define DMA_V_BYTES_PER_BURST 2
#define DMA_V_REQUEST_PER_BURST 1
#define DMA_V_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_V_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_V */
/* Move these variable declarations to the top of the function */
uint8 DMA_V_Chan;
uint8 DMA_V_TD[1];

/* Defines for DMA_I */
#define DMA_I_BYTES_PER_BURST 2
#define DMA_I_REQUEST_PER_BURST 1
#define DMA_I_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_I_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_I */
/* Move these variable declarations to the top of the function */
uint8 DMA_I_Chan;
uint8 DMA_I_TD[1];

/* Defines for DMA_A */
#define DMA_A_BYTES_PER_BURST sizeof(buf_v[0])
#define DMA_A_REQUEST_PER_BURST 1
#define DMA_A_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_A_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_A */
/* Move these variable declarations to the top of the function */
uint8 DMA_A_Chan;
uint8 DMA_A_TD[2];

/* Defines for DMA_B */
#define DMA_B_BYTES_PER_BURST sizeof(buf_i[0])
#define DMA_B_REQUEST_PER_BURST 1
#define DMA_B_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_B_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_B */
/* Move these variable declarations to the top of the function */
uint8 DMA_B_Chan;
uint8 DMA_B_TD[2];

/* Defines for DMA_Bat */
#define DMA_Bat_BYTES_PER_BURST sizeof(buf_bat[0][0])
#define DMA_Bat_REQUEST_PER_BURST 1
#define DMA_Bat_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_Bat_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_Bat */
/* Move these variable declarations to the top of the function */
uint8 DMA_Bat_Chan;
uint8 DMA_Bat_TD[2];

void DMA_init(const uint16 skip) {
	/* DMA Configuration for DMA_V */
	DMA_V_Chan = DMA_V_DmaInitialize(DMA_V_BYTES_PER_BURST, DMA_V_REQUEST_PER_BURST, HI16(DMA_V_SRC_BASE), HI16(DMA_V_DST_BASE));
	DMA_V_TD[0] = CyDmaTdAllocate();
	CyDmaTdSetConfiguration(DMA_V_TD[0], 2, DMA_V_TD[0], 0);
	CyDmaTdSetAddress(DMA_V_TD[0], LO16((uint32)ADC_V_SAR_WRK0_PTR), LO16((uint32)Filter_STAGEA_PTR));
	CyDmaChSetInitialTd(DMA_V_Chan, DMA_V_TD[0]);
	CyDmaChRoundRobin(DMA_V_Chan, 1);
	CyDmaChEnable(DMA_V_Chan, 1);
	
	/* DMA Configuration for DMA_I */
	DMA_I_Chan = DMA_I_DmaInitialize(DMA_I_BYTES_PER_BURST, DMA_I_REQUEST_PER_BURST, HI16(DMA_I_SRC_BASE), HI16(DMA_I_DST_BASE));
	DMA_I_TD[0] = CyDmaTdAllocate();
	CyDmaTdSetConfiguration(DMA_I_TD[0], 2, DMA_I_TD[0], 0);
	CyDmaTdSetAddress(DMA_I_TD[0], LO16((uint32)ADC_I_SAR_WRK0_PTR), LO16((uint32)Filter_STAGEB_PTR));
	CyDmaChSetInitialTd(DMA_I_Chan, DMA_I_TD[0]);
	CyDmaChRoundRobin(DMA_I_Chan, 1);
	CyDmaChEnable(DMA_I_Chan, 1);

	/* DMA Configuration for DMA_A */
	DMA_A_Chan = DMA_A_DmaInitialize(DMA_A_BYTES_PER_BURST, DMA_A_REQUEST_PER_BURST, HI16(DMA_A_SRC_BASE), HI16(DMA_A_DST_BASE));
	DMA_A_TD[0] = CyDmaTdAllocate();
	DMA_A_TD[1] = CyDmaTdAllocate();
	CyDmaTdSetConfiguration(DMA_A_TD[0], skip * sizeof(buf_v), DMA_A_TD[1], 0);
	CyDmaTdSetConfiguration(DMA_A_TD[1], sizeof(buf_v), DMA_A_TD[0], DMA_A__TD_TERMOUT_EN);
	CyDmaTdSetAddress(DMA_A_TD[0], LO16((uint32)Filter_HOLDA_PTR), LO16((uint32)dma_ignore));
	CyDmaTdSetAddress(DMA_A_TD[1], LO16((uint32)Filter_HOLDA_PTR), LO16((uint32)buf_v));
	CyDmaChSetInitialTd(DMA_A_Chan, DMA_A_TD[0]);
	CyDmaChEnable(DMA_A_Chan, 1);

	/* DMA Configuration for DMA_B */
	DMA_B_Chan = DMA_B_DmaInitialize(DMA_B_BYTES_PER_BURST, DMA_B_REQUEST_PER_BURST, HI16(DMA_B_SRC_BASE), HI16(DMA_B_DST_BASE));
	DMA_B_TD[0] = CyDmaTdAllocate();
	DMA_B_TD[1] = CyDmaTdAllocate();
	CyDmaTdSetConfiguration(DMA_B_TD[0], skip * sizeof(buf_i), DMA_B_TD[1], 0);
	CyDmaTdSetConfiguration(DMA_B_TD[1], sizeof(buf_i), DMA_B_TD[0], DMA_B__TD_TERMOUT_EN);
	CyDmaTdSetAddress(DMA_B_TD[0], LO16((uint32)Filter_HOLDB_PTR), LO16((uint32)dma_ignore));
	CyDmaTdSetAddress(DMA_B_TD[1], LO16((uint32)Filter_HOLDB_PTR), LO16((uint32)buf_i));
	CyDmaChSetInitialTd(DMA_B_Chan, DMA_B_TD[0]);
	CyDmaChEnable(DMA_B_Chan, 1);

	/* DMA Configuration for DMA_Bat */
	DMA_Bat_Chan = DMA_Bat_DmaInitialize(DMA_Bat_BYTES_PER_BURST, DMA_Bat_REQUEST_PER_BURST, HI16(DMA_Bat_SRC_BASE), HI16(DMA_Bat_DST_BASE));
	DMA_Bat_TD[0] = CyDmaTdAllocate();
	DMA_Bat_TD[1] = CyDmaTdAllocate();
	CyDmaTdSetConfiguration(DMA_Bat_TD[0], sizeof(buf_bat[0]), DMA_Bat_TD[1], TD_INC_DST_ADR | DMA_Bat__TD_TERMOUT_EN);
	CyDmaTdSetConfiguration(DMA_Bat_TD[1], sizeof(buf_bat[1]), DMA_Bat_TD[0], TD_INC_DST_ADR | DMA_Bat__TD_TERMOUT_EN);
	CyDmaTdSetAddress(DMA_Bat_TD[0], LO16((uint32)ADC_Bat_DEC_SAMP_PTR), LO16((uint32)buf_bat[0]));
	CyDmaTdSetAddress(DMA_Bat_TD[1], LO16((uint32)ADC_Bat_DEC_SAMP_PTR), LO16((uint32)buf_bat[1]));
	CyDmaChSetInitialTd(DMA_Bat_Chan, DMA_Bat_TD[0]);
	CyDmaChRoundRobin(DMA_Bat_Chan, 1);
	CyDmaChEnable(DMA_Bat_Chan, 1);
}

/* [] END OF FILE */
