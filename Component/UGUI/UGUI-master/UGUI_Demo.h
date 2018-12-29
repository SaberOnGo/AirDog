
#ifndef __UGUI_DEMO_H_
#define  __UGUI_DEMO_H_

#include "ugui.h"
#include <stm32f10x.h>


#define  SNS_RGB888_0   0x039962
#define  SNS_RGB888_1   0xffde35
#define  SNS_RGB888_2   0xff9124
#define  SNS_RGB888_3   0xe60224
#define  SNS_RGB888_4   0x6d1090
#define  SNS_RGB888_5   0x8c031e
#define  SNS_RGB888_6   0xe9e9e9   // 背景灰
#define  SNS_RGB888_7   0xd8d91b   // 橘黄
#define  SNS_RGB888_8   0xBEBEBE   // Grey
#define  SNS_RGB888_9   0x778899   // LightSlateGray


void UGUI_Demo(void);
u16  UGUI_RGB888_TO_RGB565(u32 rgb);
void UGUI_DrawThickArc(s16 x0, s16 y0, s16 r1, u16 thickness, u8 sector, UG_COLOR color);
void UGUI_DrawThickCircle(s16 x0, s16 y0, s16 r1, u16 thickness, UG_COLOR color);

#endif

