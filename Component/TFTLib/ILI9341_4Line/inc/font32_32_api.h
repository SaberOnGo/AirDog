
#ifndef FONT32_32_API_H__
#define  FONT32_32_API_H__

#include "GlobalDef.h"

typedef enum
{
    FONT_UNIT_SQUARE  = 0,  // 2
    FONT_UNIT_Cube    = 1,  // 3
    FONT_UNIT_Celsius = 2, // 'C
    FONT_UNIT_Percent = 3, // %
    
	FONT_UNIT_End,
}FONT_UNIT;



void FT3224_DrawString(u8 row,  u8 column, u16 fc, u16 bc, u8 * s);
void FT1608_DrawString(u8 row, u8 column, u16 fc, u16 bc, u8 *s);
void FT2416_DrawString(u8 row, u8 column, u16 fc, u16 bc, u8 *s);
void FT2416_DrawUnit(u8 row, u8 column, u16 fc, u16 bc, FONT_UNIT unit);

void FT2012_DrawString(u8 row, u8 column, u16 fc, u16 bc, u8 *s);
void FT2616_DrawString(u8 row,  u8 column, u16 fc, u16 bc, u8 *s);

void FT1016_Draw_Subscript2(u8 row, u8 column, u16 fc, u16 bc);
void ICO_DrawBatIsCharging(void);
void ICO_DrawBattery(void);
void ICO_SetBatPercent(u8 percent, u8 is_charging);



void FT_DrawSpace(u16 x, u16 y, u16 fc, u16 bc, u8 w, u8 h);
void FT_WriteInt(u8 row, u8 column, u32 val, uint8_t placeholder_size);

#endif

