
#ifndef __PCF8563_H__
#define __PCF8563_H__

#include "GlobalDef.h"


#define  PCF_SEC_REG       0x02
#define  PCF_MIN_REG      0x03
#define  PCF_HOUR_REG  0x04
#define  PCF_DAY_REG      0x05
#define  PCF_WEEK_REG   0x06
#define  PCF_MON_REG     0x07
#define  PCF_YEAR_REG   0x08


void PCF8563_Set(T_RTC_TIME * time);
void PCF8563_SetTime(uint8_t hour, uint8_t min, uint8_t sec);
void PCF8563_SetDate(uint8_t year, uint8_t month, uint8_t day, uint8_t weekday);
SYS_RESULT PCF8563_GetTime(T_RTC_TIME * t);
void  PCF8563_Init(void);


#endif

