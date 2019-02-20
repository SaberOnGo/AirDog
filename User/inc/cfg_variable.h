
#ifndef __CFG_VARIABLE_H__
#define __CFG_VARIABLE_H__

#include <stdint.h>

extern uint16_t record_gap;     // 传感器记录间隔: unit: sec
extern uint16_t  cfgVar_LcdBackLightSec;  // LCD 背光显示时间, unit: sec
extern uint16_t cfgVar_FirstLcdBackLightSec; // 开机后第一次显示时间
extern uint16_t cfgVar_SnsRemainRunTime;  // 传感器剩余运行时间
extern uint8_t   cfgVar_DbgWritten;
extern uint16_t cfgVar_DiskFreeSize;

#if 1
#define  STR_FirstLcdBackLightSec   "FirstLcdBackLightSec=\0"
#define  STR_LcdBackLightSec            "LcdBackLightTime=\0"
#define  STR_SnsRemainRunTime      "SnsRemainRunTime=\0"
#else
extern const char STR_FirstLcdBackLightSec[];
extern const char STR_LcdBackLightSec[];
extern const char STR_SnsRemainRunTime[];
#endif

#endif

