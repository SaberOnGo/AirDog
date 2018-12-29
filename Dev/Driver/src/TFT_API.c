
#include "TFT_API.h"
#include "ILI9341_4Line_SPI.h"


#if (TFT_SPI_MODE == TFT_SPI_NORMAL)
extern u16  Lcd_ReadPoint(u16 x,u16 y);
extern void Lcd_Clear(u16 Color);
extern void Lcd_SetXY(u16 x,u16 y);
extern void Gui_DrawPoint(u16 x,u16 y,u16 Data);
extern void Lcd_SetRegion(u16 x1, u16 y1, u16 x2, u16 y2);
extern void LCD_Setup(void);
#else
#include "tft_dma.h"
#include "tft_text.h"
#include "tft_itoa.h"
#include "tft_encoder.h"
#include "os_global.h"
#include "tft_core.h"
#endif

void UserGUI_Init(void)
{
#if (TFT_SPI_MODE == TFT_SPI_DMA)
   LCD_Config();
#else
   LCD_Setup();
#endif
}

u16 UserGUI_ReadPoint(u16 x,u16 y)
{
   #if (TFT_SPI_MODE == TFT_SPI_DMA)
   u16 pixel = 0;
   
   LCD_readPixels(x, y, x, y, &pixel);

   return pixel;
   #else
   Lcd_ReadPoint(x, y);
   #endif
}


void UserGUI_LCDClear(u16 Color)
{
#if (TFT_SPI_MODE == TFT_SPI_DMA)
    LCD_fillScreen(Color);
#else
    Lcd_Clear(Color);
#endif
}

void UserGUI_SetXY(u16 x,u16 y)
{
#if (TFT_SPI_MODE == TFT_SPI_DMA)
   LCD_setCursor(x, y);
#else
   Lcd_SetXY(x, y);
#endif
}

void UserGUI_DrawPoint(u16 x,u16 y,u16 Data)
{
   u16 rp = 0;
   
#if (TFT_SPI_MODE == TFT_SPI_DMA)
   LCD_putPixel(x, y, Data);
#else
   Gui_DrawPoint(x, y, Data);
#endif

   rp = UserGUI_ReadPoint(x, y);
   if(rp != Data)
   {
      rp_err_cnt++;
   }
   else
   {
      rp_ok_cnt++;
   }
}

void UserGUI_SetRegion(u16 x1, u16 y1, u16 x2, u16 y2)
{
#if (TFT_SPI_MODE == TFT_SPI_DMA)
   LCD_setAddressWindowToWrite(x1, y1, x2, y2);
#else
   Lcd_SetRegion(x1, y1, x2, y2);
#endif
}


void UserGUI_WriteData_16Bit(u16 Data)
{
#if (TFT_SPI_MODE == TFT_SPI_DMA)
   LCD_setSpi16();
   dmaFill16Raw(Data, 1);
   //LCD_setSpi8();
#else
   Lcd_WriteData_16Bit(Data);
#endif
}

void UserGUI_WriteData16Bit_End(void)
{
#if (TFT_SPI_MODE == TFT_SPI_DMA)
   LCD_setSpi8();
#endif
}





