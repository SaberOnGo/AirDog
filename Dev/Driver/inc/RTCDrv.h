
#ifndef __RTC_DRV_H__
#define  __RTC_DRV_H__

#include "GlobalDef.h"

#define  ONE_DAY_SEC    86400L   // 1 天的秒数

//时间结构体
#pragma pack(1)
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;
	
	//公历日月年周
	vu16 year;
	vu8  month;
	vu8  day;
	vu8  week;		 
}T_Calendar_Obj;		
#pragma pack()

extern const uint8_t mon_table[];
extern T_Calendar_Obj calendar;

void    RTCDrv_Init(void);
uint8_t RTCDrv_GetTime(T_Calendar_Obj * cal);
uint8_t RTCDrv_SetAlarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t RTCDrv_SetTime(uint16_t syear,uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t RTCDrv_SetUnixTimeStamp(uint32_t time_stamp);

// 获取设备的当前的Unix 时间戳
#define RTCDrv_GetUnixTimeStamp()     RTC_GetCounter()

void RTCDrv_CalToRTCTime(T_RTC_TIME * rtc_time);

#include "RTCDrv.h"
void     RTCDrv_SecToCalendar(uint16_t fromYear, uint32_t sec, T_Calendar_Obj * cal, uint8_t one_more_day);
uint32_t RTCDrv_CalendarToSec(uint16_t fromYear, T_Calendar_Obj * cal);
uint8_t RTCDrv_GetWeek(uint16_t year, uint8_t month, uint8_t day);

void    RTCDrv_Test(void);



#endif

