
#include "rom_flash_interface.h"
#include "stm32f10x.h"
#include "GlobalDef.h"

int32_t rom_flash_initialize(void)
{
       FLASH_Unlock();
	return 0;
}


DRESULT rom_flash_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC:
    res = RES_OK;
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD*)buff = ROM_FLASH_SIZE / ROM_FLASH_SECTOR_SIZE;
    res = RES_OK;
    break;
  
  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(WORD*)buff = ROM_FLASH_SECTOR_SIZE;
    res = RES_OK;
    break;
  
  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = ROM_FLASH_SECTOR_SIZE;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}

uint16_t rom_flash_mal_write_ex(uint8_t lun, uint32_t Memory_Offset, uint32_t * Writebuff, uint16_t Transfer_Length)
{
	uint16_t i;

	os_printf("2 w: ad = 0x%x, len = %d\r\n", Memory_Offset, Transfer_Length); //测试用
	
    for( i = 0; i < Transfer_Length; i += ROM_FLASH_PAGE_SIZE )
    { 
       if( FLASH_WaitForLastOperation(ROM_FLASH_WAIT_TIMEOUT) != FLASH_TIMEOUT )
	   {
	       FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	   } 	
	   FLASH_ErasePage(ROM_FLASH_START_ADDR + Memory_Offset + i); 
    }			
	 
    for( i = 0; i < Transfer_Length; i += 4 )
    { 
	   if( FLASH_WaitForLastOperation(ROM_FLASH_WAIT_TIMEOUT) != FLASH_TIMEOUT )
	   {
	       FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR); 
	   } 
	   FLASH_ProgramWord(ROM_FLASH_START_ADDR + Memory_Offset + i , Writebuff[i >> 2]); 
    }
	
    return 0;
}


#include "os_global.h"
uint16_t rom_flash_mal_write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{
	uint16_t i;

    os_printf("0 w: ad = 0x%x, len = %d\r\n", Memory_Offset, Transfer_Length); //测试用
	for( i = 0; i < Transfer_Length; i += ROM_FLASH_PAGE_SIZE )
	{ 
		if( FLASH_WaitForLastOperation(ROM_FLASH_WAIT_TIMEOUT) != FLASH_TIMEOUT )
		{
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		} 	
		FLASH_ErasePage(ROM_FLASH_START_ADDR + Memory_Offset + i); 
	}			

	for(i = 0; i < Transfer_Length; i += 4 )
	{ 
		if( FLASH_WaitForLastOperation(ROM_FLASH_WAIT_TIMEOUT) != FLASH_TIMEOUT )
		{
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
		} 
		FLASH_ProgramWord(ROM_FLASH_START_ADDR + Memory_Offset + i, Writebuff[i >> 2]); 
	}
	return 0;
}

uint16_t rom_flash_mal_read(uint8_t lun, uint32_t Memory_Offset, uint32_t * Readbuff, uint16_t Transfer_Length)
{
	uint16_t i;

    os_printf("0 r: ad = 0x%x, len = %d\r\n", Memory_Offset, Transfer_Length); //测试用
	for(i = 0; i < Transfer_Length; i += 4)
	{
		Readbuff[i >> 2] = ((vu32*)(ROM_FLASH_START_ADDR + Memory_Offset))[i >> 2]; 
	}
	return 0;
}


/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA), 扇区号: 0-N
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT rom_flash_disk_read(BYTE lun, BYTE * buff, DWORD sector, UINT count)
{
   uint16_t i;

   for(i = 0; i < count; i++)
   {
      rom_flash_mal_read(lun, (sector + i) * ROM_FLASH_SECTOR_SIZE, (uint32_t *)buff, ROM_FLASH_SECTOR_SIZE);
	  buff += ROM_FLASH_SECTOR_SIZE;
   }
   return RES_OK;
}

// 功能: 提供给 fatfs 的disk write 写数据接口
// DWORD sector: 起始扇区号, 0-N
// UINT  count:  需要写入的扇区总数
DRESULT rom_flash_disk_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
   uint16_t i;

   for(i = 0; i < count; i++)
   {
       rom_flash_mal_write(lun, (sector + i) * ROM_FLASH_SECTOR_SIZE, (uint32_t *)buff, ROM_FLASH_SECTOR_SIZE);
	   buff += ROM_FLASH_SECTOR_SIZE;
   }
   return RES_OK;
}

