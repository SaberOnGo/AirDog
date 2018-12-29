#ifndef TEST1_DMA_H
#define TEST1_DMA_H

#include "tft_control.h"
#include <stm32f10x_dma.h>
#include "RegLib.h"

#define dmaWait() while(SPI_I2S_GetFlagStatus(SPI_MASTER,SPI_I2S_FLAG_BSY) == SET);

#if 0
#define dmaStartRx() DMA_Init(DMA1_Channel2, &dmaStructure); \
    DMA_Cmd(DMA1_Channel2, ENABLE);

#define dmaStartTx() DMA_Init(DMA1_Channel3, &dmaStructure); \
    DMA_Cmd(DMA1_Channel3, ENABLE);
#else
#define dmaStartRx() STM32_DMA_Init(DMA2_Channel1, &dmaStructure); \
	DMA2_Channel1->CCR |= DMA_CCR1_EN;


#define dmaStartTx() STM32_DMA_Init(DMA2_Channel2, &dmaStructure); \
	DMA2_Channel2->CCR |= DMA_CCR1_EN;
#endif

void dmaInit(void);

void dmaSendCmd(u8 cmd);
void dmaSendCmdCont(u8 cmd);

void dmaReceiveDataCont8(u8 *data);

void dmaSendData8(u8 *data, u32 n);
void dmaSendData16(u16 *data, u32 n);

void dmaSendDataCont8(u8 *data, u32 n);
void dmaSendDataCont16(u16 *data, u32 n);

void dmaSendDataBuf16(void);
void dmaSendDataContBuf16(u16 *data, u32 n);

void dmaFill16(u16 color, u32 n);
void dmaFill16Raw(u16 color, u32 n);

void dmaFill16Rect(u16 * colorBuf, u32 n);

#endif //TEST1_DMA_H
