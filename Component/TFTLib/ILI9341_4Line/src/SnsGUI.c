


#if 0

#include "SnsGUI.h"
#include "GlobalDef.h"
#include "os_global.h"
#include "delay.h"
#include "tft_graph.h"
#include "font32_32_api.h"	

// 空气污染颜色等级
const u16 air_color_lev[6] = 
{
       0x4cc,   // 深绿, 优, Good,     PM2.5 AQI: 0  - 50
	0xfee6,  // 黄色, 良, Moderate, PM2.5 AQI: 51 - 100
	0xfc84,  // 橙色, 轻度污染, Unhealthy for Sensitive Groups, PM2.5 AQI: 101 - 150
	
	0xe004,  // 深红, 中度污染, Unhealthy, PM2.5 AQI: 151 - 200
	0x6892,  // 深紫, 重度污染, Very Unhealthy, PM2.5 AQI: 201 - 300
	0x8803,  // 褐色, 严重污染, Dangerous,      PM2.5 AQI: 301 - 500
};


// 将甲醛浓度转换为空气质量等级
// 参数: u16 ppb: ppb 浓度
// 返回值: 空气质量等级: 1 - 6
u8 getHchoLevel(u16 ppb)
{
   u8 lev = 0;
   
   if(ppb <= 80)lev = 1;
   else if(ppb <= 100)lev  = 2;
   else if(ppb <= 500)lev  = 3;
   else if(ppb <= 1000)lev = 4;
   else if(ppb <= 2000)lev = 5;
   else if(ppb < 0xFFFF) lev = 6;

   return lev;
}



// 0.01 ppm
void SnsGUI_DisplayHCHO(uint16_t ppb)
{
        u8 str[5];

        str[0] = ppb % 10000 / 1000 + 0x30;  // ppm
	 str[1] = '.';
	 str[2] = ppb % 1000 / 100 + 0x30;  // 0.x ppm
	 str[3] = ppb % 100 / 10 + 0x30;      // 0.0x ppm
	 str[4] = 0;

	 FONT2616_DrawString(3,  10, C_WHITE, C_BLUE,  str);
}

void SnsGUI_DisplayCO(uint16_t ppm)
{
    
}

void SnsGUI_DisplayTVOC(uint16_t ppb)
{
      
}


// 显示温湿度like:  "2.1", "-2.1", "12.1", "-12.1"
// 参数: int16_t temp: 实际温度的100倍, 即单位为 0.01 'C, < 0 表示温度为负值, >= 0 表示温度为正值
// u8 humi: 湿度的百分比: 0 - 100
void SnsGUI_DisplayTemp(int16_t temp)
{
       u8 str[4];  
       u8 dig[3] = { '.', 0, 0};
       u8 *buf = str;
       int8_t negative = 0;  //  1: 表示为负数
       uint16_t  m;  // m 为温度整数
       uint8_t len;
       
       if(temp < 0)
       {
              temp = -temp;
              negative = 1;
       }
       m = (uint16_t)temp / 100;
       m %= 100; // 百位以上的数字丢弃, 则温度只能显示不超过 100 'C
       dig[1]   = (uint8_t)((uint16_t)temp % 100 / 10 + 0x30);  // 只取小数点后一位
       
       len = int_to_string(m, &str[1], 3);
       if(len < 2){  buf = &str[1];  }

       if(negative){ buf[0] = '-'; }
       else { buf[0] = ' '; }

       
       FONT2616_DrawString(8, 1, C_WHITE, C_BLUE,  buf);
       FONT2616_DrawString(8, 1 + os_strlen(buf), C_WHITE, C_BLUE,  dig);
}

void SnsGUI_DisplayHumi(u8 humi)
{
	u8 str[4];
   
	if(humi > 100)humi = 100;

	int_to_string(humi, str, sizeof(str));
	FONT2616_DrawString(8, 10, C_WHITE, C_BLUE,  str);
}

void SnsGUI_DisplayPM10(uint16_t pm10, uint8_t lev)
{
       u8 str[4];

       if(pm10 > 999)pm10 = 999;

       int_to_string(pm10, str,  sizeof(str));
       FONT2616_DrawString(2,  10,  C_WHITE,  C_BLUE,  str);
}

void SnsGUI_DisplayPM25(uint16_t pm25, uint8_t lev)
{
       u8 str[4];

       if(pm25 > 999)pm25 = 999;

       int_to_string(pm25, str,  sizeof(str));
       FONT2616_DrawString(1,  10,  C_WHITE,  C_BLUE,  str);
}

void SnsGUI_DisplayCO2(uint16_t co2_ppm)
{
       //char str_buf[5];
       
}

// 显示电池百分比
// 参数: percent : 0 - 100 表示电池剩余电量
void SnsGUI_DisplayBatPercent(uint8_t percent)
{
       char str_buf[4];

	if(percent >= 100)percent = 99;
	
	if(percent < 10)str_buf[0] = ' ';
	else str_buf[0] = percent / 10 + 0x30;

	str_buf[1] = percent % 10 + 0x30;
	str_buf[2] = '%';
	str_buf[3] = 0;
		
       UG_FontSelect(&FONT_8X12);  // 字符宽 8 pixel, 高为 12 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	UG_PutString(320 - 3 * 12, 0, str_buf);
}
#endif

#include "SnsGUI.h"
#include "GlobalDef.h"
#include "os_global.h"
#include "delay.h"
#include "tft_graph.h"

//#include "font32_32_api.h"	
#include "TFT_API.h"
#include "font_api.h"	


// 空气污染颜色等级
const u16 air_color_lev[6] = 
{
       0x4cc,   // 深绿, 优, Good,     PM2.5 AQI: 0  - 50
	0xfee6,  // 黄色, 良, Moderate, PM2.5 AQI: 51 - 100
	0xfc84,  // 橙色, 轻度污染, Unhealthy for Sensitive Groups, PM2.5 AQI: 101 - 150
	
	0xe004,  // 深红, 中度污染, Unhealthy, PM2.5 AQI: 151 - 200
	0x6892,  // 深紫, 重度污染, Very Unhealthy, PM2.5 AQI: 201 - 300
	0x8803,  // 褐色, 严重污染, Dangerous,      PM2.5 AQI: 301 - 500
};

// 圆弧座标及大小
#define  COR_XY 2

SNS_CIRCLE circle[] =
{
    #if (COR_XY == 0)
    {  46,      46,    40,   4   ,  SNS_COLOR_BC_WHITE},     // PM10
    {  46,    196,    40,   4  ,   SNS_COLOR_BC_WHITE},    // VOCs
    { 140,   120,   60,   8  ,   SNS_COLOR_BC_WHITE},    // PM2.5
    { 260,     80,   45,   8  ,   SNS_COLOR_BC_WHITE},     // HCHO
    { 255,  190,   50,   8   ,  SNS_COLOR_BC_WHITE},      // CO2
    
    { 260,  190,   30,   4   ,  SNS_COLOR_BC_WHITE},      // CO
    #elif (COR_XY == 1)
    {  52,      46,    38,   4   ,  SNS_COLOR_BC_WHITE},     // PM10
    {  52,    196,    38,   4  ,   SNS_COLOR_BC_WHITE},    // VOCs
    { 138,   120,   55,   8  ,   SNS_COLOR_BC_WHITE},    // PM2.5
    { 260,     80,   45,   8  ,   SNS_COLOR_BC_WHITE},     // HCHO
    { 255,  190,   50,   8   ,  SNS_COLOR_BC_WHITE},      // CO2
    
    { 260,  190,   30,   4   ,  SNS_COLOR_BC_WHITE},      // CO
    #elif (COR_XY == 2)
    {  48,      46,    38,   4   ,  SNS_COLOR_BC_WHITE},     // PM10
    {  48,    196,    38,   4  ,   SNS_COLOR_BC_WHITE},    // VOCs
    { 135,   120,   55,   8  ,   SNS_COLOR_BC_WHITE},    // PM2.5
    { 255,     80,   45,   8  ,   SNS_COLOR_BC_WHITE},     // HCHO
    { 250,  190,   50,   8   ,  SNS_COLOR_BC_WHITE},      // CO2
    
    { 255,  190,   30,   4   ,  SNS_COLOR_BC_WHITE},      // CO
    #endif
};

void SnsGUI_DrawArc( UG_S16 x0, UG_S16 y0, UG_S16 r, UG_U8 s, UG_COLOR c )
{
   UG_S16 x, y, d;

   if ( x0<0 ) return;
   if ( y0<0 ) return;
   if ( r<=0 ) return;

   d = 3 - 2 * r;
   x = 0;
   y = r;

   while ( x < y)
   {
      // Q1
      if ( s == 3) LCD_putPixel(x0 + x, y0 - y, c);  // 
      if ( s == 4) LCD_putPixel(x0 + y, y0 - x, c);

      // Q2
      if ( s == 1) LCD_putPixel(x0 - y, y0 - x, c);
      if ( s == 2) LCD_putPixel(x0 - x, y0 - y, c);

      // Q3
      if ( s == 7) LCD_putPixel(x0 - x, y0 + y, c);
      if ( s == 0) LCD_putPixel(x0 - y, y0 + x, c);

      // Q4
      if ( s == 5) LCD_putPixel(x0 + y, y0 + x, c);
      if ( s == 6) LCD_putPixel(x0 + x, y0 + y, c);

      if(d < 0)
      {
         d = d + 4 * x + 6;
      }
	  else 
	  {
	     d = d + 4 * (x - y) + 10;
		 y--;
	  }
	  x++;
   }
}
void SnsGUI_DrawArc2( UG_S16 x0, UG_S16 y0, UG_S16 r, UG_U8 s, UG_COLOR c )
{
   UG_S16 x,y,xd,yd,e;

   if ( x0<0 ) return;
   if ( y0<0 ) return;
   if ( r<=0 ) return;

   xd = 1 - (r << 1);
   yd = 0;
   e = 0;
   x = r;
   y = 0;

   while ( x >= y )
   {
      // Q1
      if ( s == 4) LCD_putPixel(x0 + x, y0 - y, c);
      if ( s == 3) LCD_putPixel(x0 + y, y0 - x, c);

      // Q2
      if ( s == 2) LCD_putPixel(x0 - y, y0 - x, c);
      if ( s == 1) LCD_putPixel(x0 - x, y0 - y, c);

      // Q3
      if ( s == 0) LCD_putPixel(x0 - x, y0 + y, c);
      if ( s == 7) LCD_putPixel(x0 - y, y0 + x, c);

      // Q4
      if ( s == 6) LCD_putPixel(x0 + y, y0 + x, c);
      if ( s == 5) LCD_putPixel(x0 + x, y0 + y, c);

      y++;
      e += yd;
      yd += 2;
      if ( ((e << 1) + xd) > 0 )
      {
         x--;
         e += xd;
         xd += 2;
      }
   }
}



void SnsGUI_DrawThickRing(s16 x0, s16 y0, s16 r1, u16 thickness, u8 sector, UG_COLOR color)
{
       u8 i;
	for(i = 0; i < thickness; i++)
	{
	      SnsGUI_DrawArc(x0, y0, r1 + i, sector, color);
	}
	for(i = 0; i < thickness; i++)
	{
	      SnsGUI_DrawArc2(x0, y0, r1 + i, sector, color);
	}
}

/*
功能: // 通过圆弧显示污染等级
参数: s16 x0: 圆弧中心x座标, unit: pixel
             s16 y0: 圆弧中心y座标, unit: pixel
             s16 r:    圆弧半径
             u16 thick: 圆弧线条的粗细
             u18 lev: 污染等级: 0 - 6, 0等级: 不显示弧段; 6: 显示 6 个弧段
             u16 bc: 背景色: 用于清除之前的弧段
*/
void SNSGUI_DrawArcLev(s16 x0, s16 y0, s16 r, u16 thick, u8 lev, u16 bc)
{
#if MODULE_GUI_EN
      u8 i;

      if(lev > 6)lev = 6;

	// 显示灰色
	for(i = 0; i < 6; i++)
	{
	        //UGUI_DrawThickArc(x0, y0, r, thick, i, C_WHITE_SMOKE);
               SnsGUI_DrawThickRing(x0, y0, r, thick, i, SNS_COLOR_BC_WHITE);
	}
        // 显示颜色等级
       if(lev)

       {
       	for(i = 0; i < lev; i++)
       	{
       	       //UGUI_DrawThickArc(x0, y0, r, thick, i, air_color_lev[lev - 1]);
       	        SnsGUI_DrawThickRing(x0, y0, r, thick, i, air_color_lev[lev - 1]);
       	}
	}
#endif	
}

void SNSGUI_DrawIndicateRing(u8 sns, u8 lev)
{
#if MODULE_GUI_EN
      if(sns < (u8)SnsCircle_End)
      {
            SNSGUI_DrawArcLev(circle[sns].x0, circle[sns].y0, 
	                                                      circle[sns].r, circle[sns].thick, lev, C_WHITE);  
      }
#endif      
}

// 将甲醛浓度转换为空气质量等级
// 参数: u16 ppb: ppb 浓度
// 返回值: 空气质量等级: 1 - 6
u8 getHchoLevel(u16 ppb)
{
   u8 lev = 0;
   
   if(ppb <= 80)lev = 1;
   else if(ppb <= 100)lev  = 2;
   else if(ppb <= 500)lev  = 3;
   else if(ppb <= 1000)lev = 4;
   else if(ppb <= 2000)lev = 5;
   else if(ppb < 0xFFFF) lev = 6;

   return lev;
}

// 将TVOC 浓度转换为空气质量等级
// 参数: u16 ppb: ppb 浓度
// 返回值: 空气质量等级: 1 - 6
u8 getTVOCLevel(u16 ppb)
{
    u8 lev = 0;
	
    if(ppb <= 500)lev = 1;
	else if(ppb <= 600)lev = 2;
	else if(ppb < 0xFFFF) lev = 4;
	
	return lev;
}

// 0.01 ppm
void SnsGUI_DisplayHCHO(uint16_t ppb)
{
#if MODULE_GUI_EN
      char str_buf[5];
      static uint8_t last_lev = 0xFF;
      u8 lev = 0;

	// 显示效果: 0.01 mg/m3
	if(ppb != 0xFFFF)
	{
	   //os_printf("hcho ppb = %d\n", ppb);
           str_buf[0] = ppb / 1000 + 0x30;   // 整数位
	    str_buf[2] = ppb % 1000 / 100 + 0x30;  // 小数点后1位
	    str_buf[3] = ppb % 100 / 10 + 0x30;       // 小数点后第2位
	   // str_buf[4] = ppb % 10 + 0x30;
	}
	else
	{
	     os_memset(str_buf, '0', 5);
	}
	str_buf[1] = '.';
	str_buf[4] = 0;
	
       // HCHO 数据
       UG_FontSelect(&FONT_16X26);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(circle[CIRCLE_HCHO].x0 - (16 * 2),  circle[CIRCLE_HCHO].y0 - 13, str_buf);
    
	lev = getHchoLevel(ppb);
	if(last_lev != lev)
	{
	      last_lev = lev;
	     SNSGUI_DrawIndicateRing(CIRCLE_HCHO, lev);
	}
#endif
}

void SnsGUI_DisplayCO(uint16_t ppm)
{
#if MODULE_GUI_EN
       char str_buf[4];
       static uint8_t last_lev = 0xFF;
       u8 lev = 0;
	
       str_buf[0] = ppm / 100 + 0x30;
	str_buf[1] = ppm % 100 / 10 + 0x30;
	str_buf[2] = ppm % 10 + 0x30;
	str_buf[3] = 0;

       // CO 数据
	UG_FontSelect(&FONT_12X20);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(circle[CIRCLE_CO].x0 - 12 * 2, circle[CIRCLE_CO].y0 - 10, str_buf);

	lev = 1;
	if(last_lev != lev)
	{
               last_lev = lev;
               SNSGUI_DrawIndicateRing(CIRCLE_CO, 1);
	}
#endif	
}


// 显示效果: 0.1 ppm
void SnsGUI_DisplayTVOC(uint16_t ppb)
{
#if MODULE_GUI_EN
       char str_buf[5];
       static uint8_t last_lev = 0xFF;
       u8 lev = 0;

       str_buf[1] = '.';
	if(ppb != 0xFFFF)
	{
               str_buf[0] = ppb / 1000 + 0x30;   // 整数位
               str_buf[2] = ppb % 1000 / 100 + 0x30;  // 小数点第1位
               str_buf[3] = (ppb + 5) % 100 / 10 + 0x30;   // 小数点后第2位
	}
	else
	{
	       str_buf[0] = '0';
	       str_buf[2] = '0';
	       str_buf[3] = '0';
	      
	}
	 str_buf[4] = 0;

	
    // CO 数据
	UG_FontSelect(&FONT_10X16);          
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(circle[CIRCLE_TVOC].x0 - 10 * 2, circle[CIRCLE_TVOC].y0 - 8, str_buf);

	lev = getTVOCLevel(ppb);
	if(last_lev != lev)
	{
	       last_lev = lev;
	       SNSGUI_DrawIndicateRing(CIRCLE_TVOC, lev);
	}
#endif
}
void SnsGUI_TVOCWait(uint8_t sec)
{
#if MODULE_GUI_EN
       char str_buf[5];
    //u8 lev = 0;
	
       str_buf[0] = sec / 100 + 0x30;
	str_buf[1] = sec % 100 / 10 + 0x30;
	str_buf[2] = sec % 10 + 0x30;
	str_buf[3] = ' ';
	str_buf[4] = 0;


    // TVOC 数据
	UG_FontSelect(&FONT_10X16);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(circle[CIRCLE_TVOC].x0 - 10 * 2,  circle[CIRCLE_TVOC].y0  - 8, str_buf);


       UG_FontSelect(&FONT_8X12);  // 字符宽 8 pixel, 高为 12 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	
	// 显示等待的秒数
    //LCD_fillRect(50 - 8 * 1.5, 200 - 10 + 12 + 8, 24, 12, C_WHITE);
	UG_PutString(circle[CIRCLE_TVOC].x0 - 8 * 1.5,  circle[CIRCLE_TVOC].y0 - 8 + 12 + 8,   "sec");
#endif
}

// 显示温湿度
// 参数: int16_t temp: 实际温度的100倍, 即单位为 0.01 'C, < 0 表示温度为负值, >= 0 表示温度为正值
// u8 humi: 湿度的百分比: 0 - 100
void SnsGUI_DisplayTemp(int16_t temp)
{
#if MODULE_GUI_EN
      char str_temp[6];
      u16 d;
	
      if(temp < 0)
     {
		str_temp[0] = '-';
		temp = -temp;
      }
	else str_temp[0] = ' ';

	d = temp / 100;  // 温度整数值
	#if 0
	str_temp[1] = d % 100 / 10 + 0x30;
	str_temp[2] = d % 10 + 0x30;
	str_temp[3] = '.';
	str_temp[4] = temp % 100 / 10 + 0x30; // 温度小数点后一位
       str_temp[5] = 0;
       #else
       os_snprintf(&str_temp[1],  5,  "%2d.%1d",  d,  temp % 100 / 10);
       #endif
       
    // 温度 数据
	UG_FontSelect(&FONT_12X20);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(88,               187, str_temp);
	UG_PutString(88 + 12  * 5 + 2 ,  187, "'C");
#endif
}

void SnsGUI_DisplayHumi(u8 humi)
{
#if MODULE_GUI_EN
	char str_humi[3];
   
	os_snprintf(str_humi,  sizeof(str_humi),  "%2d",  humi);
	
	UG_FontSelect(&FONT_12X20);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);

	UG_PutString(103,                   212,      str_humi);
	UG_PutString(103 + 12 * 2 + 6,        212, "% RH");
#endif
}

void SnsGUI_DisplayTime(T_Calendar_Obj *cal)
{
#if MODULE_GUI_EN
      char date_buf[9];
      char time_buf[9];
      UG_S16 y = 6;  // 2
      UG_S16 x = 133;
      
      os_snprintf(date_buf,  sizeof(date_buf),  "%02d-%02d-%02d",  
                                 cal->year - 2000,  cal->month, cal->day);
      os_snprintf(time_buf,  sizeof(time_buf),  "%02d:%02d:%02d",  
                                 cal->hour,  cal->min, cal->sec);

      UG_FontSelect((const UG_FONT *)&FONT_6X8);  // 字符宽 6 pixel, 高为 8 pixel
      UG_SetBackcolor(C_WHITE);
      UG_SetForecolor(C_LightSlateGray);
      UG_PutString(x, y,    date_buf);
      UG_PutString(x, y + 8,  time_buf);
#endif      
}

// 显示WIFI 提示字符
void SnsGUI_DisplayWifiTip(WifiTipType TipType)
{
#if MODULE_GUI_EN
      //UG_S16 x = 130, y = 25;
      UG_S16 x = 130, y = 30;
      
      UG_FontSelect(&FONT_8X12);  // 字符宽 8 pixel, 高为 12 pixel
      UG_SetBackcolor(C_WHITE);
      UG_SetForecolor(C_LightSlateGray);

      switch(TipType)
      {
              case TIP_WIFI_OK:
              {
                      UG_PutString(x, y,    "WIFI OK ");
              }break;
              case TIP_WIFI_UnConn:
              {
                     UG_PutString(x, y,    "UnConn  ");
              }break;
              case TIP_WIFI_TryCon:
              {
                     UG_PutString(x, y,    "TryCon..");
              }break;
              case TIP_WIFI_Reset:
              {
                     UG_PutString(x, y,    "Reset   ");
              }break;
              case TIP_WIFI_Failed:
              {
                     UG_PutString(x, y,    "Failed  ");
              }break;
              case TIP_WIFI_Erase:
              {
                      UG_PutString(x, y,    "            ");
              }break;
              default:
                     UG_PutString(x, y,    "Error   ");
                    break;
      }
#endif      
}

// 显示CO2 提示字符
void SnsGUI_DisplayCO2Tip(CO2TipType TipType)
{
#if MODULE_GUI_EN
      //UG_S16 x = 130, y = 25;
      UG_S16 x = 224, y = 224;
      
      UG_FontSelect(&FONT_8X12);  // 字符宽 8 pixel, 高为 12 pixel
      UG_SetBackcolor(C_WHITE);
      UG_SetForecolor(C_LightSlateGray);

      switch(TipType)
      {
              case TIP_CO2_CALI_ON:
              {
                      UG_PutString(x, y,    "CALI... ");
              }break;
              case TIP_CO2_CALI_OK:
              {
                     UG_PutString(x, y,    "CALI OK");
              }break;
              case TIP_CO2_CALI_FAILED:
              {
                     UG_PutString(x, y,    "Failed   ");
              }break;
              case TIP_CO2_CALI_Erase:
              {
                     UG_PutString(x, y,    "             ");
              }break;
              default:
                     UG_PutString(x, y,    "Error      ");
                    break;
      }
#endif      
}


void SnsGUI_DisplayPM10(uint16_t pm10, uint8_t lev)
{
#if MODULE_GUI_EN
       char str_buf[4];
       static uint8_t last_lev = 0xFF;
       //u8 x_off = 3;
       
       pm10 %= 1000;

	os_snprintf(str_buf,  sizeof(str_buf),  "%3d",  pm10);
	
      // PM10 数据
       UG_FontSelect(&FONT_12X20);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(circle[CIRCLE_PM10].x0- (6 * 3), 
	                                circle[CIRCLE_PM10].y0 - 10,     str_buf);

      // if(lev == 0) lev = 1;
       if(last_lev != lev)
       {
              last_lev = lev;
   	       SNSGUI_DrawIndicateRing(CIRCLE_PM10, lev);
       }
#endif
}

void SnsGUI_DisplayPM25(uint16_t pm25, uint8_t lev)
{
#if MODULE_GUI_EN
       static uint8_t last_lev = 0xFF;  // 上一次的等级值
       char buf[4];
       
	if(pm25 > 999) pm25 = 999;

       buf[0] = pm25 / 100;
       buf[1] = pm25 % 100 / 10;
       buf[2] = pm25 % 10;

       if(buf[0] == 0)
       {
              buf[0] = 15;
              if(buf[1] == 0)buf[1] = 15;
       }
        buf[3] = 0;
        
    // PM2.5显示数字 “125”
      #if 1
	FONT3224_DrawNum(circle[CIRCLE_PM2P5].x0 - 36,      
	                                                   circle[CIRCLE_PM2P5].y0 - 12,  
	                                                   C_BLACK,  C_WHITE,  buf[0]);
	                                                   
	FONT3224_DrawNum(circle[CIRCLE_PM2P5].x0 - 36 + 24, 
	                                                  circle[CIRCLE_PM2P5].y0 - 12, 
	                                                  C_BLACK, C_WHITE,  buf[1]);
	                                                  
	FONT3224_DrawNum(circle[CIRCLE_PM2P5].x0 - 36 + 48, 
	                                                  circle[CIRCLE_PM2P5].y0 - 12, 
	                                                  C_BLACK, C_WHITE,  buf[2]);
     // UG_PutString(circle[CIRCLE_PM2P5].x0 - 12 * 2.5,  circle[CIRCLE_PM2P5].y0 + 24,  "ug/m3");
      #else

      #endif
      
    //if(lev == 0) lev = 1;
	if(last_lev != lev)
	{
	      last_lev = lev;
	      SNSGUI_DrawIndicateRing(CIRCLE_PM2P5, lev);
	}
#endif
}

void SnsGUI_DisplayCO2(uint16_t co2_ppm)
{
#if MODULE_GUI_EN
        char str_buf[5];
	 static uint8_t last_lev = 0xFF;  // 上一次的等级
        uint8_t co2_lev = 1; // CO2 等级
	
       co2_ppm %= 10000;
	
	os_snprintf(str_buf,  sizeof(str_buf), "%4d",  co2_ppm);

	// CO2 数据
       UG_FontSelect(&FONT_16X26);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(circle[CIRCLE_CO2].x0 - (16 * 2),  circle[CIRCLE_CO2].y0 - 13, str_buf);

	if(co2_ppm < 600)co2_lev = 1;
	else if(co2_ppm < 1000)co2_lev = 2;
	else if(co2_ppm < 2000)co2_lev = 3;
	else { co2_lev = 4; }

//	os_printf("co2_ppm = %d, lev = %d\n", co2_ppm, co2_lev);
	
	if(last_lev != co2_lev)
	{
	      last_lev = co2_lev;
	      SNSGUI_DrawIndicateRing(CIRCLE_CO2, co2_lev);
	}
#endif
}

// 显示电池百分比
// 参数: percent : 0 - 100 表示电池剩余电量
void SnsGUI_DisplayBatPercent(uint8_t percent)
{
#if MODULE_GUI_EN
       char str_buf[4];

	if(percent >= 100)percent = 99;
	
	if(percent < 10)str_buf[0] = ' ';
	else str_buf[0] = percent / 10 + 0x30;

	str_buf[1] = percent % 10 + 0x30;
	str_buf[2] = '%';
	str_buf[3] = 0;
		
      UG_FontSelect(&FONT_8X12);  // 字符宽 8 pixel, 高为 12 pixel
      UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	UG_PutString(320 - 3 * 12, 0, str_buf);
#endif	
}

#include "os_timer.h"




void drawSNSArc(SnsCircleIndex sns, u8 sector, u16 color)
{
      SnsGUI_DrawThickRing(circle[sns].x0, circle[sns].y0, 
                                                           circle[sns].r, circle[sns].thick, sector,  color);
}

// 左上角
void drawPM10Arc(u8 sector, u16 color)
{
       drawSNSArc(CIRCLE_PM10, sector, color);
}
// 左下角 
void drawTVOCArc(u8 sector, u16 color)
{
       drawSNSArc(CIRCLE_TVOC, sector, color);
}
// 中间
void drawPM25Arc(u8 sector, u16 color)
{
      drawSNSArc(CIRCLE_PM2P5, sector, color);
}
// 右上角
void drawHCHOArc(u8 sector, u16 color)
{
       drawSNSArc(CIRCLE_HCHO, sector, color);
}
// 右下角
void drawCO2Arc(u8 sector, u16 color)
{
       drawSNSArc(CIRCLE_CO2, sector, color);
}

void drawCOArc(u8 sector, u16 color)
{
       drawSNSArc(CIRCLE_CO, sector, color);
}

typedef void (* drawSnsArc)(u8 sector, u16 colour);

drawSnsArc drawSnsArcFunc[] = {
   drawPM10Arc,       // 左上角
   drawTVOCArc,      // 左下角 
   drawPM25Arc,      // 中间
   drawHCHOArc,     // 右上角
   drawCO2Arc,        // 右下角
   //drawCOArc,
};

void SnsGUI_DisplayVersion(char * version, char * date, char * time)
{
#if MODULE_GUI_EN
       UserGUI_LCDClear(C_GRAY);
       UG_FontSelect(&FONT_12X20);  
       UG_SetBackcolor(C_GRAY);
	UG_SetForecolor(C_BLACK);
	UG_PutString(118, 85,       "AIR DOG");
	UG_PutString(82, 110,       version);
	UG_PutString(94, 135,    date);
	UG_PutString(94, 160,   time);
#endif	
}

void SnsGUI_DisplaySDCardMode(void)
{
#if MODULE_GUI_EN
       UserGUI_LCDClear(C_WHITE);
       UG_FontSelect(&FONT_16X26);  
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_BLACK);
	UG_PutString(24, 107,       "READ SD CARD MODE");
#endif	
}

void SnsGUI_DisplayNormal(void)
{
#if MODULE_GUI_EN
      UserGUI_LCDClear(C_WHITE);

       //时间
       SnsGUI_DisplayTime(&calendar);  // 给一个初始数据

       
       #if 1
       // PM10 字符
       UG_FontSelect(&FONT_8X12);  // 字符宽 6 pixel, 高为 8 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	UG_PutString(circle[CIRCLE_PM10].x0 - (8 * 2), 
	                                circle[CIRCLE_PM10].y0 - 10 - 12,  "PM10");
       // PM10 单位: ug/m3
       UG_PutString(circle[CIRCLE_PM10].x0 - 8 * 2.5,  circle[CIRCLE_PM10].y0 + 10, "ug/m3");
       SnsGUI_DisplayPM10(0, 0);  // 给一个初始数据
       
       // PM2.5
       UG_FontSelect(&FONT_12X20);  
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	UG_PutString(circle[CIRCLE_PM2P5].x0 - 12 * 2.5, circle[CIRCLE_PM2P5].y0 - 20 - 16,   "PM2.5");
       SnsGUI_DisplayPM25(0, 0);  // 给一个初始数据

	// PM2.5 单位: ug/m3
	UG_FontSelect(&FONT_10X16);  
	//UG_PutString(circle[CIRCLE_PM2P5].x0 - 12 * 2.5,  circle[CIRCLE_PM2P5].y0 + 24,  "ug/m3");
	UG_PutString(circle[CIRCLE_PM2P5].x0 - 10 * 2.5,  circle[CIRCLE_PM2P5].y0 + 24,  "ug/m3");
	
	// TVOC 
	UG_FontSelect(&FONT_8X12);  
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	UG_PutString(circle[CIRCLE_TVOC].x0 - (8 * 2), 
	                                circle[CIRCLE_TVOC].y0 - 12  - 10,  "TVOC");
       // TVOC 单位: ppm
       UG_PutString(circle[CIRCLE_TVOC].x0 - 8 * 1.5,  circle[CIRCLE_TVOC].y0 + 10, "ppm");
       SnsGUI_DisplayTVOC(1200);  // 给一个初始数据

	// HCHO 字符
       UG_FontSelect(&FONT_10X16);  
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	UG_PutString(circle[CIRCLE_HCHO].x0 - 10 * 2, 
	                                circle[CIRCLE_HCHO].y0 - 13 - 16,   "HCHO");
	// HCHO 单位: ug/m3
	UG_PutString(circle[CIRCLE_HCHO].x0 - 15,  circle[CIRCLE_HCHO].y0 - 13 + 16 + 10 , "mg/m3");
       SnsGUI_DisplayHCHO(123);
	
	// CO2 字符
       UG_FontSelect(&FONT_12X20);  // 字符宽 12 pixel, 高为 20 pixel
       UG_SetBackcolor(C_WHITE);
	UG_SetForecolor(C_LightSlateGray);
	
	UG_PutString(circle[CIRCLE_CO2].x0 - 12, 
	                                circle[CIRCLE_CO2].y0 - 13 - 20,   "CO");
	FONT1016_Draw_Subscript2(circle[CIRCLE_CO2].x0 - 12 + 24,
	                                                                  circle[CIRCLE_CO2].y0 - 13 - 20,   C_LightSlateGray, C_WHITE);
	// CO2 单位: ppm
	UG_FontSelect(&FONT_10X16);  
	UG_PutString(circle[CIRCLE_CO2].x0 - 15,  circle[CIRCLE_CO2].y0 + 13 , "ppm");
       SNSGUI_DrawIndicateRing(CIRCLE_CO2, 0);
       
	ICON_DrawBattery();
       #endif
#endif       
}

void SnsGUI_Demo(void)
{
#if MODULE_GUI_EN
       SnsGUI_DisplayNormal();
	SnsGUI_Wait();
#endif
}


#define  CIRCLE_SIZE  (sizeof(drawSnsArcFunc) / sizeof(drawSnsArcFunc[0]))

#define  IDX_STATUS    0     // circle 状态索引
#define  IDX_SECTOR    1    // 扇区位置索引

#define  C_TO_STOP    0    // 停止转圈
#define  C_TO_TURN    1   // 需要转圈

// N  个圆圈, a[x][0] 值为 0, 表示停止转圈, 为 1 表示需要转圈
// a[x][1]: 为转到第几扇区
u8 snsTurn[CIRCLE_SIZE][2];  
os_timer_t tTimerTurnCircle;
void Timer_TurnCircle_CallBack(void * arg)
{

   u8 j;
   E_BOOL is_stop = E_TRUE; // 是否全部停止转圈
   
   for(j = 0; j < CIRCLE_SIZE; j++)
   {
         if(snsTurn[j][IDX_STATUS] == C_TO_TURN)  // 需要转圈
         {
                drawSnsArcFunc[j](snsTurn[j][IDX_SECTOR], SNS_COLOR_BC_WHITE);  // 画弧线, 转圈
		  snsTurn[j][IDX_SECTOR]++;
		  if(snsTurn[j][IDX_SECTOR] >= 6)snsTurn[j][IDX_SECTOR] = 0;
		  drawSnsArcFunc[j](snsTurn[j][IDX_SECTOR], C_BLACK);  // 画弧线, 转圈

		  is_stop = E_FALSE;
          }
   }
   if(! is_stop)
   {
         os_timer_arm(&tTimerTurnCircle,   70, 0);
   }
}


// 清除圆圈
// index: 0 - 5
void SnsGUI_ClearCircle(SnsCircleIndex index)
{
#if MODULE_GUI_EN
   u8 j;

   if(index >= CIRCLE_SIZE)return;
   
   snsTurn[(u8)index][IDX_STATUS] = C_TO_STOP;
   for(j = 0; j < 8; j++)  // 清除弧段
   {
          drawSnsArcFunc[(u8)index](j, C_WHITE);
   }
#endif   
}

// 转圈
void SnsGUI_Wait(void)
{
   #if MODULE_GUI_EN
   u8 i;

   for(i = 0; i < CIRCLE_SIZE; i++)
   {
         snsTurn[i][IDX_STATUS] = C_TO_TURN;
	  snsTurn[i][IDX_SECTOR] = 0;
   }
   
   
   os_timer_setfn(&tTimerTurnCircle, Timer_TurnCircle_CallBack, NULL);
   os_timer_arm(&tTimerTurnCircle,   50, 0);
   #endif
}






