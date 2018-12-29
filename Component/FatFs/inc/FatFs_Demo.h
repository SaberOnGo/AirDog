
#ifndef __FATFS_DEMO_H__
#define  __FATFS_DEMO_H__

#include <stdint.h>
#include "ff.h"
#include "GlobalDef.h"



#define SENSOR_TXT_FILE_NAME  "sensor_data.txt"

// 磁盘信息
typedef struct
{
      uint8_t mount; // 挂载是否成功; 0: 成功; 非0: 失败
      uint32_t total;  // 总容量, KB
      uint32_t free;   // 剩余容量, KB
}DiskInfo;

extern DiskInfo diskInfo[];

extern FIL   fileFIL;
extern long strtol(const char *str, char **endptr, int base);
	
void FatFs_Demo(void);
FRESULT FILE_Open(FIL * fileFIL, char * file_name);
E_RESULT FILE_Write(char * file_name, char write_buf[]);
void FILE_FormatDisk(FATFS * disk,  const char * diskPath,  uint8_t format);
uint32_t FILE_GetSensorTotalItems(void);
void FILE_SearchUpdateBinFile(const char * diskPath);
void FILE_ReadConfig(void);

void FILE_TimeStampToFileName(uint32_t time_stamp, char * fileName, uint16_t  buf_size);
uint8_t FILE_GetLock(void);





#endif  //end of file

