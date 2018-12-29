#ifndef  __GIZWITS_FLASH_H__
#define __GIZWITS_FLASH_H__


#include "stm32f10x.h"

//typedef uint32_t  u32;
//typedef uint16_t u16;
//typedef uint8_t  u8;

//typedef __IO uint32_t  vu32;
//typedef __IO uint16_t vu16;
//typedef __IO uint8_t  vu8;

//#include "stm32f10x.h"
/* BootLoader Flash首地址 */
#define SYS_Bootloader_SAVE_ADDR_BASE       0x08000000//Bootloader首地址
/* 升级参数存储 */
#define UPDATE_PARAM_SAVE_ADDR_BASE         0x08006C00
#define UPDATE_PARAM_MAX_SIZE               (1*1024)//支持参数大小1KB
/* APP Flash首地址 */
#define SYS_APP_SAVE_ADDR_BASE              0x08007000
#define APP_DATA_MAX_SIZE                   (110*1024L)//支持APP大小110KB
/* APP BAK Flash首地址 */
#define SYS_APP_BAK_SAVE_ADDR_BASE          0x08022800
#define APP_BAK_DATA_MAX_SIZE               (110*1024L)//支持APP_BAK大小110KB


/* FLASH页大小 */
//#define FLASH_PAGE_SIZE           0x400U   //1KB

#define NEED_UPDATA_PARAM 0xA5A5//////10100101 10100101
#define DONT_UPDATA_PARAM 0x5A5A//////01011010 01011010

//void save_param_to_flash(uint16_t * buf_to_save,uint16_t len );
//void read_param_from_flash(uint16_t * buf_to_get,uint16_t len);
//void set_flash_flag_to_updata(uint16_t crc_code);
void flash_erase(uint32_t size , uint32_t addr_base);
//void EraseFlash(uint32_t size);
void rFlashData(uint8_t * buf_to_get , uint16_t len , uint32_t rFlashAddr);
void wFlashData(uint8_t * buf_to_save , uint16_t len , uint32_t wFlashAddr);
void iap_load_app(u32 appxaddr);

#endif

