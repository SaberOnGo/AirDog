
/******************** Sensor Data Record  Repository  传感器数据记录存储头文件   *********************** */
#ifndef __SDRR_H__
#define  __SDRR_H__

#include "GlobalDef.h"

// 传感器枚举
typedef enum
{
	SENSOR_PM25 = 0,   
	SENSOR_HCHO = 1,
	SENSOR_TEMP = 2,
	SENSOR_HUMI = 3,
	
	SENSOR_Light = 4,
       SENSOR_TVOC  = 5,
	SENSOR_CO2   = 6,

	SENSOR_CO    = 7,
	SENSOR_PM10,   // PM10
	SENSOR_BATTERY,   // 电池
	SENSOR_TIME,   

	SENSOR_0P3_UM,    // PM0.3 粒子计数
	SENSOR_PM10_UM,  // PM10 粒子计数
	SENSOR_RESERVED,  // 保留
	
	SENSOR_END,
}E_SnsType;

typedef struct struct_sensor_item T_SENSOR_ITEM;
typedef struct struct_fl_sns_item T_FL_SNS_ITEM;


// 每个扇区内能保存的传感器条目数
#define ITEM_SIZE_PER_SECTOR  (4096L / sizeof(T_FL_SNS_ITEM))
 

extern char  sector_buf[];

extern T_SENSOR_ITEM tSDRR;

#define SNS_ITEM_LEN   85  // 单条传感器条目的字符串长度


void SDRR_Init(void);
void FILE_ReadConfig(void);
E_RESULT SDRR_PushItemToFile(T_SENSOR_ITEM * item);
E_RESULT SDRR_SaveSensorPoint(E_SnsType  type, void  * data);
uint32_t FILE_GetLastItemIndex(uint32_t time_stamp);



#endif

