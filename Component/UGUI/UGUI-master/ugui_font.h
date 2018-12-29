
#ifndef __UGUI_FONT_H__
#define __UGUI_FONT_H__

#include "GlobalDef.h"
#if (UGUI_USE_FONT == FONT_IN_ROM_FLASH)
#include "ugui_fontlib.h"


#else
#include "flash_font.h"


extern FONT_DESC * font_4x6;
extern FONT_DESC * font_5x8;
extern FONT_DESC * font_5x12;
extern FONT_DESC * font_6x8;

extern FONT_DESC * font_6x10;
extern FONT_DESC * font_7x12;
extern FONT_DESC * font_8x8;
extern FONT_DESC * font_8x12;
extern FONT_DESC * font_8x14;

extern FONT_DESC * font_10x16;
extern FONT_DESC * font_12x16;
extern FONT_DESC * font_12x20;
extern FONT_DESC * font_16x26;

extern FONT_DESC * font_22x36;
extern FONT_DESC * font_24x40;
extern FONT_DESC * font_32x53;



uint8_t UGUI_FONT_GetPixel(FONT_DESC * desc, uint32_t index);
#endif

#endif

