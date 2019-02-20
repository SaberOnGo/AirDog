#include "cfg_variable.h"

/*
以下变量均在磁盘根目录的config.txt文件修改此值

*/
uint16_t record_gap = 20;  // 记录间隔, 默认每隔 15 秒记录一次传感器数据
uint16_t  cfgVar_LcdBackLightSec  = 45; // LCD 背光显示持续时间
uint16_t cfgVar_FirstLcdBackLightSec = 180;   // 开机后第一次显示时间
uint16_t cfgVar_SnsRemainRunTime = 300;  // 默认 5 分钟
uint8_t   cfgVar_DbgWritten = 0;  // 是否写入调试信息到文件中
uint16_t cfgVar_DiskFreeSize = 0;   // 文件系统需要保留的最小空闲大小, 单位KB

//const char STR_FirstLcdBackLightSec[32] = "FirstLcdBackLightSec=\0";
//const char STR_LcdBackLightSec[32] =            "LcdBackLightTime=\0";
//const char STR_SnsRemainRunTime[32] =      "SnsRemainRunTime=\0";

