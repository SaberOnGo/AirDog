#ifndef FONT_API_H__
#define  FONT_API_H__

#include "GlobalDef.h"
//#include "font32_32_api.h"


typedef enum
{
    FONT_UNIT_SQUARE  = 0,  // 2
    FONT_UNIT_Cube    = 1,  // 3
    FONT_UNIT_Celsius = 2, // 'C
    FONT_UNIT_Percent = 3, // %
    
	FONT_UNIT_End,
}FONT_UNIT;


void FONT3232_DrawNum(u16 x, u16 y, u16 fc, u16 bc, u8 num);
void FONT3224_DrawNum(u16 x, u16 y, u16 fc, u16 bc, u8 num);
void FONT1608_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void FONT2416_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void FONT2416_DrawUnit(u16 x, u16 y, u16 fc, u16 bc, FONT_UNIT unit);
void FONT2012_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void FONT2616_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void FONT1016_Draw_Subscript2(u16 x, u16 y, u16 fc, u16 bc);
void ICON_DrawBatIsCharging(void);
void ICON_DrawBattery(void);
void ICON_SetBatPercent(u8 percent, u8 is_charging,  u8 reflush);



void FONT_DrawSpace(u16 x, u16 y, u16 fc, u16 bc, u8 w, u8 h);
void FONT_WriteInt(u16 x, u16 y, u16 fc, u16 bc, u8 width, u8 height, u32 val, uint8_t placeholder_size);

#endif

