
#include "UGUI_Demo.h"
#include "GlobalDef.h"

#include "stm32f10x.h"
#include "ILI9341_4Line_SPI.h"
#include "TFT_API.h"
#include "UserGUI.h"
//#include "Font1.h"
#include "delay.h"
//#include "Picture.h"
//#include "QDTFT_demo.h"
#include "tft_graph.h"

UG_GUI gui;

typedef void (*gui_init_func)(UG_S16,UG_S16,UG_COLOR);

u16 UGUI_RGB888_TO_RGB565(u32 rgb)
{
       u16 color;
	u8 r, g, b;

	r = (u8)((rgb & 0xFF0000) >> 16);
	g = (u8)((rgb & 0x00FF00) >> 8);
	b = (u8)(rgb & 0x0000FF);

	color = ILI9341_COLOR(r, g, b);

	return color;
}

// UGUI 库初始化
void UGUI_Init(void)
{
       UG_Init(&gui, (gui_init_func)UserGUI_DrawPoint, 320, 240);
}

void UGUI_StringDemo(void)
{
       u16 bc = UGUI_RGB888_TO_RGB565(SkyBlue);
	u16 fc = UGUI_RGB888_TO_RGB565(MintCream);
	//u16 data_x_offset = 110;   // 数据的X轴起始位置
	//u16 unit_x_offset   = 200; // 单位的X轴起始位置
	u16 y_base_offset = 48;    // Y轴起始偏移
  //  u16 sns_color = BLACK, unit_color = BLACK;
	
    

       UserGUI_LCDClear(bc);
	
       UG_FontSelect(&FONT_16X26);  // 字符宽 12 pixel, 高为 20 pixel
	UG_SetBackcolor(bc);
	UG_SetForecolor(fc);
	UG_PutString(40, y_base_offset + 0,      "PM2.5");
	UG_PutString(40, y_base_offset + 26,     "PM10");
	UG_PutString(40, y_base_offset + 26 * 2, "CO");
	UG_PutString(40, y_base_offset + 26 * 3, "TVOC");
	UG_PutString(40, y_base_offset + 26 * 4, "Temp");
	UG_PutString(40, y_base_offset + 26 * 5, "Humi");
}

void UGUI_ArcDemo1(void)
{
    u16 sec;
    u8 j , tog;
    while (1) 
    {
	   for (sec = 0; sec < 8; sec++)
	   {
	      j++;
	      if( j >=9 )
	      {
	         j   = 0;
	         tog = ! tog ;
	      }
		  if(tog)
		  {
		      UG_DrawArc(160, 120, 60, 0x01 << sec, C_BLACK);
			  os_printf("arc sec = %d, color = black\n", sec);
		  }
		  else
		  {
		      UG_DrawArc(160, 120, 60, 0x01 << sec, C_WHITE);
			  os_printf("arc sec = %d, color = white\n", sec);
		  }
		  delay_ms(1000);
       }
    }
}




extern void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color);
extern void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color);

//LCD_drawFastHLine(x0, x0 + w, )
void xLine(u16 x0, u16 x1, u16 y, u16 color)
{
    LCD_drawFastHLine(x0, y, x1 - x0, color);
}

void yLine(u16 x, u16 y0, u16 y1, u16 color)
{
    LCD_drawFastVLine(x, y0, y1 - y0, color);
}



/*
// 画粗线条的圆弧, 并且指定厚度
// 参数: s16 x0: 圆弧圆心 x座标像素
                  s16 y0: 圆弧圆心y座标像素
                  s16 rad: 圆弧半径, 注意不能超出屏幕
                  u8 sector: 扇区, 0 - 7, 顺时针

                           2  |   3
                     1        |       4
                    ------------
                     0        |       5
                           7  |   6 

                   u16  thickness: 圆弧线条的粗细
                   UG_COLOR c: 圆弧的颜色
*/
void UGUI_DrawThickArc(s16 x0, s16 y0, s16 r1, u16 thickness, u8 sector, UG_COLOR color)
{
    s16 x2 = r1 + thickness;
    s16 x1 = r1;
    s16 y = 0;
   // s16 xd2  = 1 - (x2 << 1);  // 因为是在 (2r, 0)座标处开始绘制, 所以是 1 - 2r
   // s16 xd1  = 1 - (x1 << 1);
    s16 xd2  = 1 - x2;  // 因为是在 (2r, 0)座标处开始绘制, 所以是 1 - 2r
    s16 xd1  = 1 - x1;
   
    if(x2 == x1)x2 += 1; // 厚度至少为1
	
    while(x2 >= y) {
        if(sector == 4)xLine(x0 + x1,   x0 + x2,  y0 - y,   color);
        if(sector == 3)yLine(x0 + y,    y0 - x2,  y0 - x1,  color);
		if(sector == 2)yLine(x0 - y,    y0 - x2,  y0 - x1,  color);
		if(sector == 1)xLine(x0 - x2,   x0 - x1,  y0 - y,   color);
		if(sector == 0)xLine(x0 - x2,   x0 - x1,  y0 + y,   color);
		if(sector == 7)yLine(x0 - y,    y0 + x1,  y0 + x2,  color);
		if(sector == 6)yLine(x0 + y,    y0 + x1,  y0 + x2,  color);
        if(sector == 5)xLine(x0 + x1,   x0 + x2,  y0 + y,   color);
		
        y++;

        if (xd2 < 0) {
            //xd2 += 2 * y + 1;
            xd2 += 3 * y + 1;
        } else {
            x2--;
            //xd2 += 2 * (y - x2 + 1);
            xd2 += 3 * (y - x2 + 1);
        }

        if (y > r1) {
            x1 = y;
        } else {
            if (xd1 < 0) {
                //xd1 += 2 * y + 1;
                xd1 += 3 * y + 1;
            } else {
                x1--;
                //xd1 += 2 * (y - x1 + 1);
                xd1 += 3 * (y - x1 + 1);
            }
        }
    }
}


void UGUI_DrawThickCircle(s16 x0, s16 y0, s16 r1, u16 thickness, UG_COLOR color)
{
    s16 x2 = r1 + thickness;
    s16 x1 = r1;
    s16 y = 0;
    s16 xd2  = 1 - (x2 << 1);  // 因为是在 (2r, 0)座标处开始绘制, 所以是 1 - 2r
    s16 xd1  = 1 - (x1 << 1);

    if(x2 == x1)x2 += 1; // 厚度至少为1
	
    while(x2 >= y) {
        xLine(x0 + x1,   x0 + x2,  y0 + y,   color);
        yLine(x0 + y,    y0 + x1,  y0 + x2,  color);
        xLine(x0 - x2,   x0 - x1,  y0 + y,   color);
        yLine(x0 - y,    y0 + x1,  y0 + x2,  color);
        xLine(x0 - x2,   x0 - x1,  y0 - y,   color);
        yLine(x0 - y,    y0 - x2,  y0 - x1,  color);
        xLine(x0 + x1,   x0 + x2,  y0 - y,   color);
        yLine(x0 + y,    y0 - x2,  y0 - x1,  color);

        y++;

        if (xd2 < 0) {
            //xd2 += 2 * y + 1;
            xd2 += 3 * y + 1;
        } else {
            x2--;
            //xd2 += 2 * (y - x2 + 1);
            xd2 += 3 * (y - x2 + 1);
        }

        if (y > r1) {
            x1 = y;
        } else {
            if (xd1 < 0) {
                //xd1 += 2 * y + 1;
                xd1 += 3 * y + 1;
            } else {
                x1--;
                //xd1 += 2 * (y - x1 + 1);
                xd1 += 3 * (y - x1 + 1);
            }
        }
    }
}

void UGUI_ArcDemo2(void)
{
      //u8 i;

      UG_FontSelect(&FONT_16X26);  // 字符宽 12 pixel, 高为 20 pixel
	//UG_SetBackcolor(C_RED);
	//UG_SetForecolor(C_YELLOW);

	
	UGUI_DrawThickCircle(160, 120, 60, 5, C_GOLDEN_ROD);
}

void UGUI_SnsGraph(void)
{
      u8 x_base_offset = 40; // x 轴 偏移

	UGUI_DrawThickCircle(x_base_offset + 50,  40,  30, 4, C_GREEN);        // PM10 的圆环
	UGUI_DrawThickCircle(x_base_offset + 230, 50,  40, 4, C_OLIVE_DRAB);  // CO 的圆环
	UGUI_DrawThickCircle(x_base_offset + 140, 120, 60, 4, C_BROWN);        // PM2.5 的圆环
	UGUI_DrawThickCircle(x_base_offset + 50,  200, 30, 4, C_WHITE_SMOKE);  // VOCs 的圆环
	UGUI_DrawThickCircle(x_base_offset + 230, 190, 40, 4, C_ROYAL_BLUE);   // VOCs 的圆环
}


// 将RGB888 转为 RGB565
#if 0
void UGUI_PrintfRGB565(void)
{
       u16 rgb565[10];
       u8 i;
	

	rgb565[0] = UGUI_RGB888_TO_RGB565(SNS_RGB888_0);
	rgb565[1] = UGUI_RGB888_TO_RGB565(SNS_RGB888_1);
	rgb565[2] = UGUI_RGB888_TO_RGB565(SNS_RGB888_2);
	rgb565[3] = UGUI_RGB888_TO_RGB565(SNS_RGB888_3);
	rgb565[4] = UGUI_RGB888_TO_RGB565(SNS_RGB888_4);
	rgb565[5] = UGUI_RGB888_TO_RGB565(SNS_RGB888_5);
       rgb565[6] = UGUI_RGB888_TO_RGB565(SNS_RGB888_6);
	rgb565[7] = UGUI_RGB888_TO_RGB565(SNS_RGB888_7);
	rgb565[8] = UGUI_RGB888_TO_RGB565(SNS_RGB888_8);
	rgb565[9] = UGUI_RGB888_TO_RGB565(SNS_RGB888_9);
	
	for(i = 0; i < 10; i++)
	{
	      os_printf("rgb565[%d] = 0x%x\n", i, rgb565[i]);
	}
}
#endif

void UGUI_Demo(void)
{
       //u8 i;
	
       UGUI_Init();
       //delay_ms(100);
	UserGUI_LCDClear(C_WHITE);

	
	//UGUI_ArcDemo2();
	//UGUI_SnsGraph();

    #if 0
	for(i = 0; i < 8; i++)
	{
	    UGUI_DrawThickArc(160, 120, 80, 5, i, C_WHITE);
		delay_ms(1500);
	}
	#endif
	
	//UGUI_PrintfRGB565();
}

