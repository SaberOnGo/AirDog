
/***************************** SD CARD 存储接口访问头文件 ****************************************************/
#ifndef __SD_CARD_INTERFACE_H__
#define  __SD_CARD_INTERFACE_H__

#include <stdint.h>

#include "diskio.h"
#include "Integer.h"

#define SD_CARD_DISK_SIZE            ((uint64_t)((uint64_t)2 * 1024 * 1024 * 1024))   // 2GB SD 卡
#define SD_CARD_SECTOR_SIZE          512                           // 扇区大小
#define SD_CARD_BLOCK_SIZE           512           // 块大小

int32_t sd_initialize(void);
DRESULT sd_ioctl(BYTE lun, BYTE cmd, void *buff);
DRESULT sd_disk_read(BYTE lun, BYTE * buff, DWORD sector, UINT count);
DRESULT sd_disk_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);
DRESULT sd_get_init_status(void);


uint16_t sd_mal_read(uint8_t lun, uint64_t mem_offset, uint32_t * buff, uint16_t len);
uint16_t sd_mal_write(uint8_t lun, uint64_t mem_offset, uint32_t * buff, uint16_t len);

#include "sdcard_include.h"
//DRESULT  SD_ReadDisk(BYTE lun, const BYTE * buff, DWORD sector, UINT cnt);
//DRESULT  SD_WriteDisk(BYTE lun, const BYTE * buf, DWORD sector, UINT cnt);

#endif

