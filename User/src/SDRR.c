

/******************** Sensor Data Record  Repository  传感器数据记录处理文件   *********************** */


#include "Sensor.h"
#include "os_global.h"
#include "FatFs_Demo.h"
#include <string.h>
#include "RTCDrv.h"
#include "cfg_variable.h"

// 此数组升级的时候也共用buf
char sector_buf[512];     // 以一个FLASH的物理扇区大小为容量, 减少对FLASH的擦写次数

static uint16_t cur_str_len    = 0;   // 当前字符串占用的长度

#if   SDRR_DBG_EN
#define SDRR_DBG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define SDRR_DBG(...)
#endif

#define ERR_TO_RETURN()  { SDRR_DBG("err %s %d\n", __FILE__, __LINE__); return result; } 






// 传感器数据记录模块初始化
void SDRR_Init(void)
{

}

#define SNS_INT_TO_STRING_03D_PERCENT(TYPE_MAX, sns) left_size = *size - len;\
	if(item->sns != TYPE_MAX){ len += os_snprintf(&buf[len], left_size, "%3d%%", item->sns); }\
		else { len += os_snprintf(&buf[len], left_size, "  NC "); }
		
#define SNS_INT_TO_STRING_03D(TYPE_MAX, sns) left_size = *size - len;\
	if(item->sns != TYPE_MAX){ len += os_snprintf(&buf[len], left_size, "%3d", item->sns); }\
		else { len += os_snprintf(&buf[len], left_size, " NC "); }
		
#define SNS_INT_TO_STRING_05D(TYPE_MAX, sns) left_size = *size - len;\
	if(item->sns != TYPE_MAX){ len += os_snprintf(&buf[len], left_size, "%5d ", item->sns); }\
		else { len += os_snprintf(&buf[len], left_size, " NC "); }


// 供系统查看的数据, 不对外公开
// 单条: 30B,  异常时  1 min  一条.  正常时  5 min 一条 , 则一天: 288 - 1440条。 
// 假设：一条平均 3 min 一条, 则一天 480 * 30 B = 13 KB, 7MB 可存储 550 天数据


// 供用户查看的数据
// 将传感器数据转化为字符串
// 参数: T_SENSOR_ITEM * item: 传感器数据记录结构变量
//       uint8_t * buf: 输出字符串
//       uint16_t * size: 作输入时是buf的最大长度, 输出时是字符串的长度
//  单条长度: 8 + 17 + 8 + 6 + 7 + 6 + 6 + 5 * 4 + 5 + 2 = 85 B
//  一条占用 85 字节, 假设 10 min  一条, 一天 : 144条; 12 KB
//  假设 FLASH 为 16MB字节, 可用 8MB, 则可保存数据 682 天
//  index    date      time      pm2.5_ug  pm0.5_ug  hcho_ppb    temp     tvoc_ppb   co2_ppm  humi  light  damage  move  bat  reserved   
//  "0-65535 Y-Mon-Day HH:MM:SS  ug/m3     ug/m3     ppb         'C       ppb        ppm      %                          %    ppb
//  [00005] 17-05-01 22:59:51    031       008       00080       123.1    00100      15000    033   100    100     100   100  65535
// "[00006] 17-05-01 23:59:51    031       008       00100       -010.1   00100      03300    012   000    000     000   030  65535"
E_RESULT SDRR_SensorPointToString(T_SENSOR_ITEM * item, uint16_t  item_count, uint8_t * buf, uint16_t * size)
{
   uint16_t left_size = *size;   // 最大可用长度
   uint16_t len = 0;
   uint16_t temp;
   T_Calendar_Obj cal;
   
   if(NULL == item || NULL == buf || NULL == size){ INSERT_ERROR_INFO(0);  return APP_FAILED; }

   // 检查所有传感器的数据是否保存
   #if 0 // 暂时屏蔽
   if(sdrr->sensor_mask < mask)
   {
      SDRR_DBG("sensor pointer not save all, sensor_mask = 0x%x\n", sdrr->sensor_mask);
	  return APP_FAILED;
   }
   #endif

   // 条目 日期 时间
   RTCDrv_SecToCalendar(1970, item->time_stamp, &cal, E_TRUE);
   len += os_snprintf(&buf[len], left_size, "\r\n[%5d]   %02d-%02d-%02d %02d:%02d:%02d  ", item_count, 
                        cal.year % 100, cal.month, cal.day, cal.hour, cal.min, cal.sec);
   SDRR_DBG("\r\n[%5d]  %02d-%02d-%02d %02d:%02d:%02d     ", item_count, 
                        cal.year % 100, cal.month, cal.day, cal.hour, cal.min, cal.sec);
   // pm2p5_ug, pm0p5_ug
   SNS_INT_TO_STRING_03D(UINT16_MAX, pm2p5_ug);
   SNS_INT_TO_STRING_03D(UINT16_MAX, pm10_ug);
   
   // hcho_ppb
   SNS_INT_TO_STRING_05D(UINT16_MAX, hcho_ppb);

   // tvoc_ppb
   SNS_INT_TO_STRING_05D(UINT16_MAX, tvoc_ppb);
   
   // temp
   if(item->temp < 0)temp = (uint16_t)(-item->temp);
   else temp = (uint16_t)item->temp;
   left_size = *size - len; 
   if(item->temp != UINT16_MAX){ len += os_snprintf(&buf[len], left_size, "%c%3d.%01d ", (item->temp < 0 ? '-' : ' '), temp / 10, temp % 10); }  
   else { len += os_snprintf(&buf[len], left_size, "NC "); }

  SNS_INT_TO_STRING_03D_PERCENT(UINT8_MAX, humi);         // humi

  // bat percent
  SNS_INT_TO_STRING_03D_PERCENT(UINT8_MAX, bat_percent);
  
   // co2_ppm
   SNS_INT_TO_STRING_05D(UINT16_MAX, co2_ppm);

   // co_ppm
   SNS_INT_TO_STRING_05D(UINT16_MAX, co_ppm);

   // reserved
   SNS_INT_TO_STRING_05D(UINT16_MAX, reserved);
   
   
   *size = len;  // 输出格式化后的字符串长度
   
   return APP_SUCCESS;
}



/***********************************
功能: 往指定文件中写入传感器记录
参数: char * file_name: 文件名
************************************/
E_RESULT SDRR_WriteRecordToFile(uint32_t time_stamp, T_SENSOR_ITEM * item)
{
       uint16_t item_len = 0; // 本次数据转化为字符串后的长度
	uint16_t left_len = 0;
	E_RESULT res = APP_FAILED;
	char fName[16];
	static uint32_t backup_days = 0;
       static uint16_t backup_item_index = 0;
       uint32_t cur_days = 0;
       
      
	// 先判断当前写入的是不是跟上一次写入的是同一个文件：
     cur_days = time_stamp / 86400;
     SDRR_DBG("SDRR time  stamp = %ld,  cur_days = %d, bk_days = %d\r\n",  
                           time_stamp, cur_days, backup_days);
    if( cur_days != backup_days)
    {
          backup_days =  cur_days;
	   backup_item_index = FILE_GetLastItemIndex(time_stamp);  // 获取最后一条的序号
	   SDRR_DBG("re get bk index = %d \r\n",  backup_item_index);
    }
    
	item_len = sizeof(sector_buf) - cur_str_len;  // 缓冲区剩余可用的长度 
	backup_item_index += 1;
	res = SDRR_SensorPointToString(item, backup_item_index, (uint8_t *)&sector_buf[cur_str_len], &item_len);
	
	cur_str_len += item_len;
	left_len = sizeof(sector_buf) - cur_str_len;

      SDRR_DBG("item_len = %d, cur_str_len = %d, left_len = %d \r\n",  item_len, cur_str_len, left_len);
	if(left_len < item_len) // 剩下的不够一条数据长度
	{
	   if(left_len > 2)
	   {
	         os_memset(&sector_buf[cur_str_len], ' ', left_len - 2);
		  os_strncpy(&sector_buf[sizeof(sector_buf) - 2], "\r\n", 2);
	   }
	   else if(left_len == 2)
	   {
	      os_strncpy(&sector_buf[sizeof(sector_buf) - 2], "\r\n", 2);
	   }

	   
	   
	  //  将数据保存到FAT 文件中
	   FILE_TimeStampToFileName(time_stamp, fName, sizeof(fName));
	   res = FILE_Write(fName, (char *)sector_buf);
	   SDRR_DBG("sector_buf add = 0x%lx \r\n", (uint32_t)sector_buf);
	   if(! res){ SDRR_DBG("write sns point success, len = %d: %s\n", cur_str_len, sector_buf);  } // 条目写入到文件成功
	   else { SDRR_DBG("write sns point failed %s %d: res = %d \r\n",   __FILE__, __LINE__,  res);  }
	   
	   os_memset(sector_buf, 0, sizeof(sector_buf));
	   cur_str_len  = 0;
	}
	
	return res;
}



// 将传感器条目保存到当天的传感器文件中
E_RESULT SDRR_PushItemToFile(T_SENSOR_ITEM * item)
{
   return SDRR_WriteRecordToFile(item->time_stamp, item);
}

#define  ERR_TO_LEAVE()  { SDRR_DBG("err: %s %d\n", __FILE__, __LINE__);  return result; }


/*
功能:  将标准时间字符串, 形如"17-12-10 01:23:01" 转换为时间戳
参数: char * buf: 字符串缓冲区
             char * c: 起始的匹配字符
             char ** p_start: 匹配成功后该字符串的起始位置
             char ** p_end :  匹配成功后该字符串的结束后的位置
             uint32_t * time_stamp: 输出的时间戳
返回值:  转换成功: APP_SUCCESS;   转换失败: APP_FAILED

字符串格式形如: 
//  单条长度: 8 + 17 + 8 + 6 + 7 + 6 + 6 + 5 * 4 + 5 + 2 = 85 B
//  一条占用 85 字节, 假设 60 sec - 5 min  一条, 一天 : 288 - 1440; 24 KB - 120 KB
//  假设 FLASH 为 8MB字节, 可用 7MB, 则可保存数据 60 - 300 天
//  index    date      time      pm2.5_ug  pm0.5_ug  hcho_ppb    temp     tvoc_ppb   co2_ppm  humi  light  damage  move  bat  reserved   
//  "0-65535 Y-Mon-Day HH:MM:SS  ug/m3     ug/m3     ppb         'C       ppb        ppm      %                          %    ppb
//  [00005] 17-05-01 22:59:51    031       008       00080       123.1    00100      15000    033   100    100     100   100  65535
// "[00006] 17-05-01 23:59:51    031       008       00100       -010.1   00100      03300    012   000    000     000   030  65535"
*/
E_RESULT SDRR_StrToTimeStamp(char * buf, char c, char ** p_start, char ** p_end, uint32_t * time_stamp)
{
    E_RESULT result = APP_FAILED;
	char  * p1 = NULL;
	uint16_t n = 0;

	*time_stamp = 0;
	p1 = os_strchr(buf, c);
	if(p1)
	{
	   T_Calendar_Obj cal;
	   
	   p1++;  // 跳过匹配的字符
	   if(! StrChrToInt(p1, ' ', 0, 65535, &p1, &n))  // 年
	   {
	      if(p_start)*p_start = p1; // 输出时间字符串起始指针
		  
          cal.year = 2000 + n;
		  if(StrChrToInt(p1, '-', 1, 12, &p1, &n))ERR_TO_LEAVE();
		  cal.month = n;
		  if(StrChrToInt(p1, '-', 1, 31, &p1, &n))ERR_TO_LEAVE();
		  cal.day   = n;
		  if(StrChrToInt(p1, ' ', 0, 23, &p1, &n))ERR_TO_LEAVE();   // 小时
		  cal.hour   = n;
		  if(StrChrToInt(p1, ':', 0, 59, &p1, &n))ERR_TO_LEAVE();  
		  cal.min  = n;
		  if(StrChrToInt(p1, ':', 0, 59, &p1, &n))ERR_TO_LEAVE();  
		  cal.sec  = n;

          if(p_end)*p_end = (char *)(p1 + 2);
		  *time_stamp = RTCDrv_CalendarToSec(1970, &cal);
		  result = APP_SUCCESS;
	   }
	}
	return result;
}

// 将字符串转换为INT 数据, 遇到 "NC", 转换为 该类型的最大值: 0xFFFF 或 0xFF
SYS_RESULT SDRR_StrChrToInt(char * buf, char c, uint16_t min, uint16_t max, char * * p, uint16_t * num)
{
   if( os_strncmp(&buf[1], "NC", 2) == 0 || os_strncmp(&buf[1], "nc", 2) == 0) 
   	   { *num = 0xFFFF;  if(p)*p = &buf[3]; return SYS_SUCCESS; }
   return StrChrToInt(buf, c, min, max, p, num); // 转换失败
}

// 将字符串转换为传感器条目
/*
功能: 将 字符串转换为传感器条目
参数: char * buf:  数据起始指针
             char ** p_start: 查找条目成功时, 该条目的起始字符串位置; 失败为NULL
             char ** p_end   查找条目成功时, 该条目的结束字符串位置; 失败为NULL
             T_SENSOR_ITEM * item: 查找成功时, 输出的条目结构数据; 失败为NULL
返回: 查找成功返回 APP_SUCCESS； 失败: APP_FAILED

字符串格式形如: 
//  单条长度: 8 + 17 + 8 + 6 + 7 + 6 + 6 + 5 * 4 + 5 + 2 = 85 B
//  一条占用 85 字节, 假设 60 sec - 5 min  一条, 一天 : 288 - 1440; 24 KB - 120 KB
//  假设 FLASH 为 8MB字节, 可用 7MB, 则可保存数据 60 - 300 天
//  index         date               time             pm2.5_ug   pm0.5_ug  hcho_ppb     temp      tvoc_ppb   co2_ppm  humi   light   damage  move  bat    reserved   
//  "0-65535  Y-Mon-Day  HH:MM:SS   ug/m3        ug/m3       ppb               'C           ppb            ppm          %                              
//  [00005]     17-05-01      22:59:51      031             008             00080            123.1    00100        15000       033     100    100         100     100   65535
// "[00006]     17-05-01     23:59:51       031             008             00100           -010.1   00100        03300       012     000    000         000     030   65535"
*/
E_RESULT SDRR_StrToItem(char * buf, char ** p_start, char ** p_end, T_SENSOR_ITEM * item)
{
    char * p1 = NULL;
    E_RESULT result = APP_FAILED;
	uint32_t m = 0;
    uint16_t n = 0;
	
	if(SDRR_StrToTimeStamp(buf, ']', NULL, &p1, &m))ERR_TO_RETURN();  // 转换失败
	item->time_stamp = m;
	
	if(SDRR_StrChrToInt(p1, ' ', 0, 999,   &p1, &n))ERR_TO_RETURN();    // pm2p5_ug 
	item->pm2p5_ug = n;
	

	if(SDRR_StrChrToInt(p1, ' ', 0, 999,   &p1, &n))ERR_TO_RETURN();   // pm10_ug 
	item->pm10_ug = n;
	
	if(SDRR_StrChrToInt(p1, ' ', 0, UINT16_MAX, &p1, &n))ERR_TO_RETURN();
	item->hcho_ppb = n;
	
	//item->hcho_ug = hcho_ppb_to_ug(item->hcho_ppb);

	// 温度转换
	if(SDRR_StrChrToInt(p1, ' ', 0, UINT16_MAX, &p1, &n))ERR_TO_RETURN();
	item->temp = n;
	
	if(SDRR_StrChrToInt(p1, '.', 0, 9, &p1, &n))ERR_TO_RETURN();
	if(item->temp < 0){ item->temp = (int16_t)((-item->temp) * 10 + n); }
	else { item->temp = item->temp * 10 + n; }

	if(SDRR_StrChrToInt(p1, ' ', 0, UINT16_MAX, &p1, &n))ERR_TO_RETURN();
	item->tvoc_ppb = n;
	
	if(SDRR_StrChrToInt(p1, ' ', 0, UINT16_MAX, &p1, &n))ERR_TO_RETURN();
	item->co2_ppm = n;

	if(SDRR_StrChrToInt(p1, ' ', 0, UINT16_MAX, &p1, &n))ERR_TO_RETURN();
	item->humi = (n == UINT16_MAX ? UINT8_MAX : n);

	if(SDRR_StrChrToInt(p1, ' ', 0, UINT16_MAX, &p1, &n))ERR_TO_RETURN();
	item->bat_percent = (n == UINT16_MAX ? UINT8_MAX : n);

	if(SDRR_StrChrToInt(p1, ' ', 0, UINT16_MAX, &p1, &n))ERR_TO_RETURN();
	item->reserved = n;

	return APP_SUCCESS;
}

T_SENSOR_ITEM tSDRR;

E_RESULT SDRR_SaveSensorPoint(E_SnsType type, void  * data)
{
	static uint32_t save_time_out = 0;
	
	switch(type)
	{
	    case SENSOR_HCHO:
	   	{
			tSDRR.hcho_ppb = *((uint16_t *)data);
		}break;
		case SENSOR_PM25:
		{
		   tSDRR.pm2p5_ug   = *((uint16_t *)data);
		}break;
              case SENSOR_PM10:
		{
                   tSDRR.pm10_ug    = *((uint16_t *)data);
		}break;
		case SENSOR_0P3_UM:
		{
                   tSDRR.pm0p3_um = *((uint16_t *)data);
		}break;
		case SENSOR_PM10_UM:
		{
                  tSDRR.pm10_um   = *((uint16_t *)data);
		}break;
		case SENSOR_TEMP:
		{
		   tSDRR.temp = *((uint16_t *)data);
		}break;
		case SENSOR_HUMI:
		{
			tSDRR.humi  = *((uint8_t *)data);
		}break;
		case SENSOR_TVOC:
		{
                    tSDRR.tvoc_ppb = *((uint16_t *)data);
		}break;

		case SENSOR_CO:
		{
                    tSDRR.co_ppm  = *((uint16_t *)data);
		}break;

		case SENSOR_CO2:
		{
                    tSDRR.co2_ppm = *((uint16_t *)data);
		}break;
		
		default:
		  break;
	}

      /*
      SDRR_DBG("%02d-%02d-%02d %02d:%02d:%02d \r\n",
                                    calendar.year, calendar.month, calendar.day,
                                    calendar.hour, calendar.min, calendar.sec);
     */
      tSDRR.time_stamp = RTCDrv_CalendarToSec(1970, &calendar);
       tSDRR.bat_percent = BatLev_GetPercent();
	{
	   if(OS_IsTimeout(save_time_out))
	   {		   
              T_Calendar_Obj  cal;
              
	       save_time_out = OS_SetTimeout(SEC(record_gap));  // 默认 60 sec 保存一次记录
	       SDRR_DBG("ready to wirte data point to file, tick = %ld\n", OS_GetSysTick());

	       RTCDrv_SecToCalendar(1970,  tSDRR.time_stamp,  &cal,  E_TRUE);
	       SDRR_DBG("item->time_stamp = %ld \r\n",  tSDRR.time_stamp);
              SDRR_DBG("%04d-%02d-%02d %02d:%02d:%02d \r\n",
                                     cal.year, cal.month, cal.day, cal.hour, cal.min, cal.sec);

	       #if MODULE_SDRR_EN
	              SDRR_PushItemToFile(&tSDRR);
	       #endif
	   }
	}

	return APP_SUCCESS;
}







