
#ifndef  __SENSOR_H__
#define  __SENSOR_H__

#include "GlobalDef.h"
#include "SDRR.h"
#include "RTCDrv.h"
#include "SnsGUI.h"
#include "BatteryLevel.h"


#define  HW_UART_TYPE    0
#define  SW_UART_TYPE     1

#define HCHO_UART_TYPE    SW_UART_TYPE  


#define  PM25_UART_TYPE    HW_UART_TYPE


#define     PM25_DBG_EN           0
#define     SHT_DEBUG_EN        0     // 调试使能: 1; 禁止: 0
#define     CO_DBG_EN                0
#define    TVOC_DBG_EN        0
#define    HCHO_DBG_EN          0
#define    CO2_DBG_EN               1
#define  PCF8563_DEBUG_EN   0

#define   PM25_SNS_EN    1


#pragma pack(1)

// HCHO 浓度(PMS5003S)或版本号(PMS5003)
typedef union
{
   uint16_t hcho;   // 甲醛浓度, 真实浓度 = 本数值 / 1000, unit: mg/m3
   uint8_t  ver[2]; // ver[0] 为版本号, ver[1] 为错误代码
}U_HCHO_VERSION;

typedef struct
{
   uint8_t start_char_0;
   uint8_t start_char_1;
   uint16_t len;   // 数据长度: 2 x 13 + 2
   
   uint16_t pm1p0_cf1;     // PM1.0 ug/m3
   uint16_t pm2p5_cf1;    // PM25   ug/m3
   uint16_t pm10_cf1;    // PM10   ug/m3
   uint16_t pm1p0_air;     // PM1.0, 大气环境下
   uint16_t pm2p5_air;    // PM25, 大气环境下
   uint16_t pm10_air;    // PM10, 大气环境下
   
   uint16_t PtCnt_0p3um;      // Particle Count, 0.1 L 空气中0.3um以上颗粒物个数
   uint16_t PtCnt_0p5um;
   uint16_t PtCnt_1p0um;
   uint16_t PtCnt_2p5um;
   uint16_t PtCnt_5p0um;
   uint16_t PtCnt_10p0um;
   
   U_HCHO_VERSION extra;  // 额外数据
   uint16_t sum;             // 校验和
}T_PM25CmdContent;

// AQI 标准
typedef enum
{
   AQI_US = 0,
   AQI_CN,
}E_AQI_STD;

// AQI 计算
typedef struct
{
   float C_low_us;     // 浓度低值, 美国标准
   float C_high_us;    // 浓度高值, 美国标准
   float C_low_cn;     // 浓度低值, 中国标准
   float C_high_cn;    // 浓度高值, 中国标准
   uint16_t I_low;     // AQI 低值
   uint16_t I_high;    // AQI 高值
}T_AQI;

// 循环队列
typedef struct
{
      uint16_t * buf;      // 队列缓冲区指针
      uint16_t   size;    // 队列缓冲区长度
      uint16_t   head;  // 环形队列头, 每接收到一个数据, head + 1
      uint16_t   tail;     // 环形队列尾, 每发送一个数据(该命令缓冲区为空), tail + 1
}T_ROUND_QUEUE;


// 设备的传感器数据, 存在内部，不对外公开
// 一个扇区 sector = 4096B / 30 B = 136 条, 136 * 30 = 4080, 剩下 16 B

// 传感器条目, 24 B
typedef struct struct_sensor_item
{
       uint16_t index;
	uint32_t time_stamp;  // 时间戳
	
	uint16_t pm2p5_ug;    // PM2.5 浓度, 单位: ug/m3
	uint16_t pm10_ug;      // PM10 浓度, 单位: ug/m3
	uint16_t pm0p3_um;  // PM0.3 粒子计数
	uint16_t pm10_um;    // PM10 粒子计数
	
	uint16_t hcho_ppb;    // hcho 体积分数,  1 ppb = 0.001 ppm
	uint16_t tvoc_ppb;
	
	int16_t  temp;       // 温度, 单位: 0.1 'C
	uint8_t humi;
	uint8_t bat_percent;
	
	uint16_t co2_ppm;
	uint16_t co_ppm;  // 单位: 0.1 ppm
	int16_t  core_temp;  // 单位: 1 'C
	uint16_t reserved;
}T_SENSOR_ITEM;

// 30B
typedef struct struct_fl_sns_item
{
       uint32_t id;  // 4B 
	T_SENSOR_ITEM sns;  // 24B
	uint8_t flag;  // 标记是否已上报: 0: 已发送给服务器,但还未接收到服务器响应; 1: 接收到服务器响应
	uint8_t sum;
}T_FL_SNS_ITEM;
#pragma pack()


uint16_t q_free_size(T_ROUND_QUEUE  * q);
uint16_t  q_used_size(T_ROUND_QUEUE  * q);
SYS_RESULT q_push(T_ROUND_QUEUE  * q, uint16_t data);
SYS_RESULT q_pull(T_ROUND_QUEUE  * q, uint16_t *data);
void q_init(T_ROUND_QUEUE  * q, uint16_t * buf,  uint16_t buf_size);


void Sensor_Init(void);
void Sensor_PreInit(void);


#endif

