
#ifndef __TFT_API_H__
#define  __TFT_API_H__

#include "GlobalDef.h"


#define TFT_SPI_NORMAL  0  
#define TFT_SPI_DMA     1  // DMA

#define   TFT_SPI_MODE   TFT_SPI_DMA





void UserGUI_Init(void);
u16  UserGUI_ReadPoint(u16 x,u16 y);
void UserGUI_LCDClear(u16 Color);
void UserGUI_SetXY(u16 x,u16 y);
void UserGUI_DrawPoint(u16 x,u16 y,u16 Data);
void UserGUI_SetRegion(u16 x1, u16 y1, u16 x2, u16 y2);
void UserGUI_WriteData_16Bit(u16 Data);
void UserGUI_WriteData16Bit_End(void);
void UserGUI_LCDClear(u16 Color);

#include "tft_graph.h"
__INLINE void UserGUI_DrawRegion(u16 x1, u16 y1, u16 w, u16 h, u16 * colorBuf)
{
     LCD_putPixelRect(x1, y1, w, h, colorBuf);
}

#endif

