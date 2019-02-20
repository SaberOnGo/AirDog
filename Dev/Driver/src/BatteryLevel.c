
/*-----------------------------电池电量显示操作------------------------------------------*/

#include "BatteryLevel.h"
#include "os_timer.h"
#include "os_global.h"
#include "ADC_Drv.h"
#include "board_version.h"
//#include "PowerCtrl.h"
#include "font_api.h"
#include "ExtiDrv.h"


#if BAT_DEBUG_EN
//#define BAT_DEBUG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#define  BAT_DEBUG   dbg_print_detail
#else
#define BAT_DEBUG(...)
#endif


// 电池电量对应的电量百分比
typedef struct
{
    uint8_t  percent;  // 剩余电量百分比, 如: 15 表示: 15%
	uint16_t volt;    // 电压: 如 3345 表示: 3.345V, 即单位: mV
}T_BAT_LEVEL_MAP;

// 
static const T_BAT_LEVEL_MAP BatLevMap[] = 
{
     {0,  3300},   // 0%, 3.300 V: 表示: <= 3.300 V时, 电池电量为 0%
     {5,  3380},
     {10, 3450},   // 10 %, 3.45 V,
     {25, 3500},   // 25 %, 3.50 V
     {30, 3550},   // 30 %, 3.55 V
     {40, 3600},   // 40 %, 3.60 V
     {50, 3650},   // 50 %, 3.65 V
     {60, 3700},   // 60 %, 3.70 V
     {70, 3800},
     {80, 3850},   // 80 %, 3.85 V
     {85, 3950},
     {90, 4050},   // 90 %, 4.05 V
     {95, 4130},  
     {98, 4170},
     {100, 4200}, // 100 %, 4.20 V
};

#define BAT_LEVELS   (sizeof(BatLevMap) / sizeof(BatLevMap[0]))

uint8_t bat_lev_percent = 0;  // 电池电量

// 获取电池电量百分比
uint8_t BatLev_GetPercent(void)
{
   return bat_lev_percent;
}


// 将电池电压转换成百分比
//static 
// 参数: uint16_t bat_volt, 电池电压, 单位: mV
void BatLev_VoltToPercent(uint16_t bat_volt)
{
   uint8_t i;
   //uint8_t usb_sta = 0;

   if(bat_volt <= BatLevMap[0].volt){ bat_lev_percent  = 0; }
   else if(bat_volt >= BatLevMap[BAT_LEVELS - 1].volt )
   { 
       bat_lev_percent  = 100; 
   }
   else
   {
      for(i = 1; i < BAT_LEVELS; i++)
      {
          if(BatLevMap[i - 1].volt <= bat_volt && bat_volt < BatLevMap[i].volt)
          {
             bat_lev_percent = BatLevMap[i - 1].percent;
             //BAT_DEBUG("bat lev = %d, bat_volt = %d.%03d V \r\n", i, bat_volt / 1000, bat_volt % 1000);
			 break;
          }
      }
   }
   BAT_DEBUG("bat_volt = %d.%03d V, bat=%02d%%, t = %ld\n", bat_volt / 1000, bat_volt % 1000, 
   	           bat_lev_percent, os_get_tick());
   BAT_DEBUG("bat percent = %d, bat_is_charg = %d, BatIsCharg() = %d\n",
   	           bat_lev_percent, battery_is_charging, BatteryIsCharging());
  
   
   if( (bat_lev_percent < 30) && (VIN_DETECT_Read() == 0) )  // 电量小于 30%, 关闭所有传感器
   {
       BAT_DEBUG("bat=%02d%%, pwr dn\n", bat_lev_percent);
	   
	   // 关闭传感器
	   SNS_Ctrl_Set(SW_CLOSE);
	   LCD_BackLight_Ctrl_Set(SW_CLOSE);
	   LCD_Ctrl_Set(SW_CLOSE);
	   WIFI_Power_Ctrl_Close();
   }
}


uint8_t BatteryIsCharging(void)
{
     uint8_t sta = 0;

     if(VIN_DETECT_Read())
    {
	   sta = CHRG_Indicate_Read();
	   if(sta == 0)return E_TRUE;
	   else{ return E_FALSE;  }
    }
    else 
   {
	   return E_FALSE;
    }
}
