 #include "debugfile.h"
 #include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "os_global.h"
#include "ff.h"




#if 1
#define DbgInfo_DBG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define DbgInfo_DBG(...)
#endif
 
 FIL   dbgFIL;


 // 打开指定文件
FRESULT DbgInfo_Open(FIL * pFileFIL, char * file_name)
{
   FRESULT res;
   char cur_dir[32]="0:/dbg";        // 调试信息保存的目录
   int len = 0;
   DIR dirs;

  
   // 判断目录是否存在, 不存在则创建
   res = f_opendir(&dirs, (const TCHAR *)cur_dir);  // 打开所在目录
   DbgInfo_DBG("open dir %s %s\n", cur_dir, (res == FR_OK ? "ok" : "failed"));
   if(res == FR_NO_PATH)  
   {
            // 创建该目录
          res = f_mkdir((const TCHAR *)cur_dir);
	   if(res != FR_OK){ DbgInfo_DBG("create dir %s failed, res = %d\n", cur_dir, res); return res; }
	   else {DbgInfo_DBG("create dir %s success\n", cur_dir); }
	  
   }
   if(res){  DbgInfo_DBG("file err: %s %d: res = %d \r\n",  __FILE__, __LINE__,  res);  return res; }
   
   // 打开文件获取文件指针
   len = os_strlen(cur_dir);
   cur_dir[len] = '/';
   os_strncpy(&cur_dir[len + 1],  file_name, sizeof(cur_dir));

   res = f_open(pFileFIL, (const TCHAR * )cur_dir, FA_CREATE_NEW | FA_WRITE );  // 创建文件
   f_sync(pFileFIL);
   DbgInfo_DBG("f_open: res = %d, %s %d\r\n",   res, __FILE__,  __LINE__);
   if(res == FR_OK){}
   else if(res == FR_EXIST)
   {
           res = f_open(pFileFIL,  (const TCHAR *)cur_dir,  FA_READ | FA_WRITE);
           DbgInfo_DBG("f_open: res = %d, fsize = %d, %s, %d\r\n",  res, pFileFIL->fsize, __FILE__,  __LINE__);
   }
   else
   {
         f_closedir(&dirs);
         f_close(pFileFIL);  
   }
   f_sync(pFileFIL);
    return res;
}


 // 创建文件夹并写入调试信息
E_RESULT DbgInfo_Write(char * file_name, char write_buf[])
{
    FRESULT res;
    int  len = 0;
   UINT bw;  // 输出参数, 写入后长度 


   res = DbgInfo_Open(&dbgFIL, file_name);
   if(res)
   {
         DbgInfo_DBG("file open failed: %s  res = %d \r\n",  file_name,  res);
         return APP_FAILED;
   }
   res = f_lseek(&dbgFIL,  dbgFIL.fsize); // 文件指针移到字符串结束位置
   f_sync(&dbgFIL);
   len = os_strlen(write_buf);
   res = f_write(&dbgFIL, write_buf, len, &bw);
   f_sync(&dbgFIL);
   f_close(&dbgFIL);
   
   return (E_RESULT)res;
}

#include "RTCDrv.h"
//extern T_Calendar_Obj calendar;
static char dbg_buf[4096];
static uint16_t written_len = 0;

#define dbg_buf_len  (sizeof(dbg_buf) - os_strlen("[18-02-03 11:59:20 123456789] ") )
extern void FILE_TimeStampToFileName(uint32_t time_stamp, char * fileName, uint16_t  buf_size);


#include <stdio.h>
#include <stdarg.h>
#include "cfg_variable.h"
extern uint16_t FILE_GetDiskFree(uint8_t disk_index);





void dbg_disk_write(int16_t len)
{
       if((len + written_len) > dbg_buf_len)
      {
              uint16_t free_disk = 0;

              free_disk = FILE_GetDiskFree(0);
              DbgInfo_DBG("free = %ld, freesize = %d\r\n",  free_disk,  cfgVar_DiskFreeSize);
              if(free_disk > cfgVar_DiskFreeSize)
              {
                      uint32_t time_stamp;
                      char fName[32];
                       E_RESULT res;
                       
                      time_stamp = RTCDrv_CalendarToSec(1970, &calendar);
                      FILE_TimeStampToFileName(time_stamp, fName, sizeof(fName));
                      res = DbgInfo_Write(fName, (char *)dbg_buf);
                      if(res);//DbgInfo_DBG("res = %d,  %s %d\r\n",   res, __FILE__,  __LINE__);
              }
              written_len = 0;
              os_memset(dbg_buf, 0, sizeof(dbg_buf));
      }            
}

void DbgInfo_Print(char * fmt,  ...)
{
      if(cfgVar_DbgWritten == 1)
      {
               char tmp_buf[100];
               int16_t len = 0;
               T_Calendar_Obj *c = &calendar;
               va_list vArgList;
               
               os_memset(tmp_buf, 0, sizeof(tmp_buf));
               
               va_start (vArgList, fmt);
               len = vsnprintf(tmp_buf, sizeof(tmp_buf), fmt, vArgList); 
               va_end(vArgList);  
         
               if(len <= 0)return;
               dbg_disk_write(len);

                if(len > 10)
               {
               written_len += os_snprintf(&dbg_buf[written_len],  sizeof(dbg_buf) - written_len, 
                                                         "[%02d-%02d-%02d %02d:%02d:%02d %ld] ", 
                                                         c->year % 100,  c->month, c->day, c->hour, c->min,  c->sec, os_get_tick());
               }                
               os_memcpy(&dbg_buf[written_len],  tmp_buf,  len);
               written_len += len;
      }
}



void DbgInfo_FreeDisk(void)
{
      uint16_t free_disk = 0;

      free_disk = FILE_GetDiskFree(0);
      //DbgInfo_Print("free = %ld, freesize = %d\r\n",  free_disk,  cfgVar_DiskFreeSize);
}

