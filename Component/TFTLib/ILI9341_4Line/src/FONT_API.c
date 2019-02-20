#include "stm32f10x.h"
#include "ILI9341_Drv.h"
#include "font32_32.h"
#include "font24_16.h"
#include "font20_12.h"
#include "font26_16.h"
#include "font10_16.h"
#include "font_api.h"
#include "icon_battery.h"
#include "delay.h"
#include "ILI9341_4Line_SPI.h"
#include "os_global.h"

/*
功能:  // 写一个空白字符
参数: u16 x: X轴像素位置: 0 - 319
             u16 y: Y轴像素位置: 0 - 239
             u16 fc:  前景色
             u16 bc: 背景色
             u8  w: 宽度
             u8  h:  高度
*/
void FONT_DrawSpace(u16 x, u16 y, u16 fc, u16 bc, u8 w, u8 h)
{
    u8 i, j;
	
	for(i = 0; i < h; i++)  // 共 32列, Y轴像素
	{
	    for(j = 0; j < w; j++) 
	    {
	        if(fc != bc) // 前景色与背景色不一致
		    	UserGUI_DrawPoint(x + j, y + i, fc);  // 每字节8个像素
			else
				UserGUI_DrawPoint(x + j, y + i, bc);
	    }
	}
}

void FONT3232_DrawNum(u16 x, u16 y, u16 fc, u16 bc, u8 num)
{
    
}

void FONT3224_DrawNum(u16 x, u16 y, u16 fc, u16 bc, u8 num)
{
    u8 i, j, k;
	u8 val;  // 字节值
	
	for(i = 0; i < 32; i++)  // 共 32列, Y轴像素
	{
	    for(j = 0; j < 3; j++) // 3个字节, 每字节8个像素, X轴像素
	    {
	       val = num32_24[(u16)num * 96 + i * 3 + j];
		   for(k = 0; k < 8; k++)	
		   {
		    	if(val & (0x80 >> k))	UserGUI_DrawPoint(x + j * 8 + k, y + i, fc);  // 每字节8个像素
				else {
					if (fc != bc) UserGUI_DrawPoint(x + j * 8 + k, y + i, bc);
				}
		    }
	    }
	}
}

                


#define CHARGE_X_OFFSET (320 - 14 - 4 - 10)          // 充电图标 X轴起始偏移
#define BAT_X_OFFSET (CHARGE_X_OFFSET - 45)   // 电池图标 X 轴起始偏移
#define BAT_Y_OFFSET (6)                        // 电池图标 Y起始偏移
#define ICON_BAT_H       18  // 电池及充电图标高度
#define ICON_BAT_W       45  // 电池宽度
#define ICON_CHARGE_W    14  // 充电图标高度

// 闪电图标,  表示电池正在充电, width = 14 pixel, height = 18 pixel
void ICON_DrawBatIsCharging(void)
{
    u16 i, j;
	//UserGUI_DrawRegion(320 - 15, 0, 15, 24, (u16 *)icon_charging); //坐标设置
	//UserGUI_SetRegion(320 - 15, 0, 319, 23);
	for(i = 0; i < ICON_BAT_H; i++)
	{
	   for(j = 0; j < ICON_CHARGE_W; j++)
	   {
	      UserGUI_DrawPoint(CHARGE_X_OFFSET + j, BAT_Y_OFFSET + i, icon_charging_1418[i * ICON_CHARGE_W + j]);
	   }
	}
	UserGUI_WriteData16Bit_End();
}





// 电池图标, width = 45 pixel, height = 18 pixel
void ICON_DrawBattery(void)
{
    u16 i, j;

	for(i = 0; i < ICON_BAT_H; i++)
	{
	   for(j = 0; j < ICON_BAT_W; j++)
	   {
	      UserGUI_DrawPoint(BAT_X_OFFSET + j, BAT_Y_OFFSET + i, icon_battery_4518[i * ICON_BAT_W + j]);
	   }
	}
	UserGUI_WriteData16Bit_End();

	
}


#include "ugui.h"

//#define  BAT_DIM_GREY    0xF7BE   // 浅灰色, p >= 70 %
#define  BAT_DARK_GREEN  0x4cc   // 深绿, 正在充电时的颜色
#define  BAT_YELLOW	     0xfee6  // 黄色, 45 <= p  < 70 %
#define  BAT_DEEP_RED    0xe004  // 深红, < 45 %

#define DIFFER_PERCENT(a0, a1)  ((a1) > (a0)  ? ((a1) - (a0)) : ((a0) - (a1)))

// 得到电池容量的像素宽度
u16  getBatIconWidth(u8 percent)
{
   u16 w = (34 * percent / 100.0);
   if(percent >= 100 || w > 34)w = 34;

   return w;
}

// 设置电池容量百分比
// 参数: u8 percent: 剩余的电池容量百分值: 0 - 100
//       u8 is_charging: 是否在充电: 0, 没有充电; 1: 正在充电
void ICON_SetBatPercent(u8 percent, u8 is_charging, u8 reflush)
{
   static u8 last_charging = 0;   // 上一次是否显示了充电图标
   static u8 last_percent    = 0; // 上一次的电池百分比
   u16 color = C_GRAY;
   u8 diff = DIFFER_PERCENT(last_percent, percent);  // 与上一次的电量差值
   E_BOOL draw_bat = E_FALSE;  // 是否需要绘制电池容量
   u16 w = 0;

   // fixed: 充电图标一直提示
   if(percent >= 98)is_charging = 0;

   if((is_charging != last_charging)  || reflush)  // 需要修改充电状态
   {
          last_charging = is_charging;
	   if(is_charging)
	   {
	       ICON_DrawBatIsCharging();  // 显示充电图标
	       w = getBatIconWidth(percent);
		   draw_bat = E_TRUE;
		   color = BAT_DARK_GREEN;
	   }
	   else
	   {
	      LCD_fillRect(CHARGE_X_OFFSET, BAT_Y_OFFSET, ICON_CHARGE_W, ICON_BAT_H, C_WHITE); // 清除充电图标
	   }
   }
   if(diff > 1)
   {
       w = getBatIconWidth(percent);
	   draw_bat = E_TRUE;
	   if(! is_charging)// 没有在充电
	   {
	       if(percent >= 70)color = C_GRAY; 
		   else if(percent >= 45)color = BAT_YELLOW;
		   else color = BAT_DEEP_RED;
	   }
	   else
	   {
	       color = BAT_DARK_GREEN;
	   }
   }
   if(draw_bat)
   {
         // os_printf("bat w = %d, color = 0x%x, diff=%d\n", w, color, diff);
         LCD_fillRect(BAT_X_OFFSET + 3,     BAT_Y_OFFSET + 3,  w,     13, color);
	  LCD_fillRect(BAT_X_OFFSET + 3 + w, BAT_Y_OFFSET + 3, 34 - w, 13, C_WHITE);  // 先清除之前的颜色
   }
}




/*
// 往指定坐标显示整数 val, 前面的0不显示
// uint8_t placeholder_size: 占位符个数, 有6个占位，则只能显示6个字符在LCD上
// 字符显示右对齐
// 参数: u8 width: 字符宽度; 
                  u8 height: 字符高度

*/
void FONT_WriteInt(u16 x, u16 y, u16 fc, u16 bc, u8 width, u8 height, u32 val, uint8_t placeholder_size)
{
    uint8_t num_string[10];  // 最多显示10个整数
    uint32_t div = 1000000000;
    uint8_t i, valid_size;  // valid_size 为有效显示的数字个数, 如 00123, 则valid_size = 3

	os_memset(num_string, 0, sizeof(num_string));
	for(i = 0; i < 10; i++)
	{
	    num_string[i] = val / div;
		val %= div;
		div /= 10;
	}

    // 跳过前面的连续 0
    for(i = 0; i < 10; i++)
    {
       if(num_string[i] != 0)break;
    }
	valid_size = 10 - i;
	if(valid_size == 0){ i = 9, valid_size = 1; }
	
	if(placeholder_size >= valid_size)
	{
	   uint8_t space_size = placeholder_size - valid_size;  // 空白字符个数
	   FONT_DrawSpace(x, y, bc, bc, width * space_size, height);
	   x += space_size * width;   // 右对齐
	}
	else  // 占位符不够, 只显示小值部分
	{
	   i += valid_size - placeholder_size; // 前面部分不显示, 如 123456, 占位符为4, 则显示 3456
	}

	for(; i < 10; i++)
	{
	   FONT3224_DrawNum(x, y, fc, bc, num_string[i]);
	   x += width;  // x 轴座标像素递增 32
	}
	
}


// X 轴 8 个像素, Y 轴 为 16个像素
void FONT1608_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	u8 i,j;
	u16 k;

	while(*s) 
	{	
		k=*s;
		if (k >= ' ')k -= ' ';  // 0x20 开始后的字符, 0x20 为 空格字符
		else k = 0;

	    // Y 轴像素
	    for(i = 0; i < 16; i++) // i 为该字符取模中的值, 比如一个字符取模 为 16B, 则 i 为 0 - 15
	    {
		    for(j = 0; j < 8; j++)   // X 轴像素 
			{
		    	if(font16_8[k * 16 + i] & ( 0x80 >> j))	
					UserGUI_DrawPoint(x + j, y + i, fc);  // 前景色
				else {
					if(fc != bc)UserGUI_DrawPoint(x + j, y + i, bc);  // 背景色
				}
			}
	    }
		x += 8;
		s++;
	}
}

// X 轴 8 个像素, Y 轴 为 16个像素
// 功能: 写字符 单位
void FONT2416_DrawUnit(u16 x, u16 y, u16 fc, u16 bc, FONT_UNIT unit)
{
	u8 i,j, val, k;
    const u8 * font_buf = NULL;
   
	if(unit >= FONT_UNIT_End)return;
	
	font_buf = &ft_unit_24_16[(u16)unit * 48];

	 // Y 轴像素
    for(i = 0; i < 24; i++)  // 共 24列, Y轴像素
	{
	    for(j = 0; j < 2; j++) // 2个字节, 每字节8个像素, X轴像素
	    {
	       val = font_buf[i * 2 + j];
		   for(k = 0; k < 8; k++)	
		   {
		    	if(val & (0x80 >> k))	UserGUI_DrawPoint(x + j * 8 + k, y + i, fc);  // 每字节8个像素
				else {
					if (fc != bc) UserGUI_DrawPoint(x + j * 8 + k, y + i, bc);
				}
		    }
	    }
	}
}

// Y 轴 24个像素, X轴 16个像素
void FONT2416_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
    u8 idx;
    u8 i, j, k;
	u8 val;  // 字节值
	
	while(*s)
	{
	    idx = *s;
		if(idx >= ' ')idx -= ' ';
		else idx = 0;

		for(i = 0; i < 24; i++)  // 共 24列, Y轴像素
		{
		    for(j = 0; j < 2; j++) // 2个字节, 每字节8个像素, X轴像素
		    {
		       val = font24_16[(u16)idx * 48 + i * 2 + j];
			   for(k = 0; k < 8; k++)	
			   {
			    	if(val & (0x80 >> k))	UserGUI_DrawPoint(x + j * 8 + k, y + i, fc);  // 每字节8个像素
					else {
						if (fc != bc) UserGUI_DrawPoint(x + j * 8 + k, y + i, bc);
					}
			    }
		    }
		}
		x += 16;  // x轴像素递增 16
		if(x >= X_MAX_PIXEL)
		{
			 x %= X_MAX_PIXEL;
			 y += 24;
		}
		s++;
	}
}

// Y 轴 26个像素, X轴 16个像素
void FONT2616_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
    u8 idx;
    u8 i, j, k;
	u8 val;  // 字节值
	
	while(*s)
	{
	    idx = *s;
		if(idx >= ' ')idx -= ' ';
		else idx = 0;

		for(i = 0; i < 26; i++)  // 共 24列, Y轴像素
		{
		    for(j = 0; j < 2; j++) // 2个字节, 每字节8个像素, X轴像素
		    {
		       val = font26_16[(u16)idx * 52 + i * 2 + j];
			   for(k = 0; k < 8; k++)	
			   {
			    	if(val & (0x80 >> k))	UserGUI_DrawPoint(x + j * 8 + k, y + i, fc);  // 每字节8个像素
					else {
						if (fc != bc) UserGUI_DrawPoint(x + j * 8 + k, y + i, bc);
					}
			    }
		    }
		}
		x += 16;  // x轴像素递增 16
		if(x >= X_MAX_PIXEL)
		{
			 x %= X_MAX_PIXEL;
			 y += 26;
		}
		s++;
	}
}

// Y 轴 20个像素, X轴 12个像素
void FONT2012_DrawString(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
    u8 idx;
    u8 i, k;
	u8 val;  // 字节值
	
	while(*s)
	{
	    idx = *s;
		if(idx >= ' ')idx -= ' ';
		else idx = 0;

		for(i = 0; i < 20; i++)  // 共 24列, Y轴像素
		{
		       val = font20_12[(u16)idx * 40 + i * 2 ];
			   for(k = 0; k < 8; k++)	
			   {
			    	if(val & (0x80 >> k))	UserGUI_DrawPoint(x + k, y + i, fc);  // 每字节8个像素
					else {
						if (fc != bc) UserGUI_DrawPoint(x + k, y + i, bc);
					}
			   }

			   val = font20_12[(u16)idx * 40 + i * 2 + 1];
			   for(k = 0; k < 4; k++)	
			   {
			    	if(val & (0x80 >> k))	UserGUI_DrawPoint(x + 8 + k, y + i, fc);  // 每字节8个像素
					else {
						if (fc != bc) UserGUI_DrawPoint(x + 8 + k, y + i, bc);
					}
			    }
		}
		x += 12;  // x轴像素递增 12
		if(x >= X_MAX_PIXEL)
		{
			 x %= X_MAX_PIXEL;
			 y += 20;
		}
		s++;
	}
}

// Y 轴 16个像素, X轴 10个像素
// 下标 2  字符
void FONT1016_Draw_Subscript2(u16 x, u16 y, u16 fc, u16 bc)
{
    u8 i, j, k;
	u8 val;  // 字节值
	
	do
	{
		for(i = 0; i < 16; i++)  // 共 16列, Y轴像素
		{
		    for(j = 0; j < 2; j++) // 2个字节, 每字节8个像素, X轴像素
		    {
		       val = font10_16[i * 2 + j];
			   for(k = 0; k < 8; k++)	
			   {
			    	if(val & (0x80 >> k))	UserGUI_DrawPoint(x + j * 8 + k, y + i, fc);  // 每字节8个像素
					else {
						if (fc != bc) UserGUI_DrawPoint(x + j * 8 + k, y + i, bc);
					}
			    }
		    }
		}	
	}while(0);
}
