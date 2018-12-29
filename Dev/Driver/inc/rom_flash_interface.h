
/* ******************************MCU FLASH 存储接口访问头文件 ********************************************/
#ifndef __ROM_FLASH_INTERFACE_H__
#define  __ROM_FLASH_INTERFACE_H__

#include <stdint.h>

#include "diskio.h"
#include "Integer.h"


// MCU 内部 FLASH 的块定义
#define ROM_FLASH_SIZE			           (32 * 1024L)      // 32 K
#define ROM_FLASH_START_ADDR	    (0x08000000 + MCU_ROM_SIZE - ROM_FLASH_SIZE)	 // Flash start address
#define ROM_FLASH_PAGE_SIZE		    0x400		 // 1K per page
#define ROM_FLASH_SECTOR_SIZE       ROM_FLASH_PAGE_SIZE
#define ROM_FLASH_WAIT_TIMEOUT	    100000



int32_t  rom_flash_initialize(void);
DRESULT rom_flash_ioctl(BYTE lun, BYTE cmd, void *buff);
DRESULT rom_flash_disk_read(BYTE lun, BYTE * buff, DWORD sector, UINT count);
DRESULT rom_flash_disk_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);

uint16_t rom_flash_mal_write(uint8_t lun, uint32_t Memory_Offset, uint32_t * Writebuff, uint16_t Transfer_Length);
uint16_t rom_flash_mal_read(uint8_t lun, uint32_t Memory_Offset, uint32_t * Readbuff, uint16_t Transfer_Length);





#endif


