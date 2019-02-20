
#ifndef __SNSGUI_H__
#define  __SNSGUI_H__

#if 0
#include "UGUI_Demo.h"



#define SNS_COLOR_BC_WHITE      0xef5d    // 背景白
#define SNS_COLOR_OrangeYellow  0xdec3    // 橘黄




void SnsGUI_DisplayCO2(uint16_t co2_ppm);
void SnsGUI_DisplayPM25(uint16_t pm25, uint8_t lev);
void SnsGUI_DisplayPM10(uint16_t pm10, uint8_t lev);
void SnsGUI_DisplayHCHO(uint16_t ppb);
void SnsGUI_DisplayBatPercent(uint8_t percent);
void SnsGUI_DisplayCO(uint16_t ppm);
void SnsGUI_DisplayTVOC(uint16_t ppb);
void SnsGUI_DisplayTemp(int16_t temp);
void SnsGUI_DisplayHumi(u8 humi);
#else
#include "UGUI_Demo.h"
#include "RTCDrv.h"


#define SNS_COLOR_BC_WHITE      0xef5d    // 背景白
#define SNS_COLOR_OrangeYellow  0xdec3    // 橘黄

// 传感器的数据圆形UI显示
typedef struct
{
    u16 x0;     // 圆心 x 坐标
    u16 y0;      // 圆心 y 坐标
    u8  r;          // 圆半径
    u8  thick;    // 线条粗细
    u16  bc;     // 圆弧背景色
}SNS_CIRCLE;


typedef enum
{
       CIRCLE_PM10   = 0,
	CIRCLE_TVOC   = 1,
	CIRCLE_PM2P5 = 2,
	CIRCLE_HCHO  = 3,
	CIRCLE_CO2    = 4,
	CIRCLE_CO      = 5,
	SnsCircle_End,
}SnsCircleIndex;

typedef enum
{
       TIP_WIFI_OK         = 0,
       TIP_WIFI_UnConn = 1,
       TIP_WIFI_TryCon  = 2,
       TIP_WIFI_Reset    = 3, 
       TIP_WIFI_Failed   = 4,
       TIP_WIFI_Erase    = 5,
       TIP_WIFI_ReadyConn = 6,
       TIP_WIFI_ReadyReset = 7, 
       TIP_WIFI_Error,
}WifiTipType;

typedef enum
{
      TIP_CO2_CALI_ON        = 0,  // 校验  Cali...
      TIP_CO2_CALI_OK        = 1,  // Cali OK
      TIP_CO2_CALI_FAILED = 2, // Cali Failed
      TIP_CO2_CALI_Erase    = 3,
      TIP_CO2_CALI_Ready   = 4,
      TIP_CO2_End,
}CO2TipType;

void SNSGUI_DrawArcLev(s16 x0, s16 y0, s16 r, u16 thick, u8 lev, u16 bc);
void SNSGUI_DrawIndicateRing(u8 sns, u8 lev);
void SnsGUI_Demo(void);
void SnsGUI_DisplayNormal(void);
void SnsGUI_DisplayVersion(char * version, char * date, char * time);
void SnsGUI_DisplayCO2(uint16_t co2_ppm);
void SnsGUI_DisplayPM25(uint16_t pm25, uint8_t lev);
void SnsGUI_DisplayPM10(uint16_t pm10, uint8_t lev);
void SnsGUI_DisplayHCHO(uint16_t ppb);
void SnsGUI_DisplayBatPercent(uint8_t percent);
void SnsGUI_DisplayCO(uint16_t ppm);
void SnsGUI_DisplayTVOC(uint16_t ppb);
void SnsGUI_TVOCWait(uint8_t sec);
void SnsGUI_DisplayTemp(int16_t temp);
void SnsGUI_DisplayHumi(u8 humi);
void SnsGUI_DisplayTime(T_Calendar_Obj *cal);
void SnsGUI_Wait(void);
void SnsGUI_ClearCircle(SnsCircleIndex index);
void SnsGUI_DisplaySDCardMode(void);
void SnsGUI_DisplayWifiTip(WifiTipType TipType);
void SnsGUI_DisplayCO2Tip(CO2TipType TipType);

#endif



#endif

