#include "stm32f10x.h"
#include "sd_card_interface.h"

#include "sdcard_include.h"

#include "Ffconf.h"
#include "os_global.h"



int32_t sd_init_status = 0;

int32_t sd_initialize(void)
{
	sd_init_status = SD_Init();
	return sd_init_status;
}

DRESULT sd_get_init_status(void)
{
       return (DRESULT)sd_init_status;
}

#define ALIGN_4  __align(4)

ALIGN_4 uint8_t sd_align_buf[512];	


volatile uint16_t sd_read_lock = 0;
volatile uint16_t sd_write_lock = 0;

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA), 扇区号: 0-N
  * @param  count: Number of sectors to read (1..255)
  * @retval DRESULT: Operation result
  */
DRESULT sd_disk_read(BYTE lun, BYTE * buff, DWORD sector, UINT count)
{
	SD_Error sta = SD_OK;
	DRESULT res = RES_OK;
	DWORD  lsector  = sector;
	UINT n;
	 uint8_t * pbuf = (uint8_t *)buff;
	 
	lsector <<= 9;
	if(((uint32_t)pbuf % 4) != 0)
	{
	 	for(n = 0; n < count; n++)
		{
		       while(sd_read_lock);
		 	sta=SD_ReadBlock(sd_align_buf, lsector + 512 * n, 512);
			os_memcpy(pbuf,  sd_align_buf, 512);
			pbuf +=512;
		} 
	}
	else
	{
		if(count ==1)
		{
		       while(sd_read_lock);
		       sta = SD_ReadBlock(pbuf, lsector, 512);    
		 }
		else 
		 {
		       while(sd_read_lock);
		       sta = SD_ReadMultiBlocks(pbuf, lsector, 512, count);
		  }
	}
	res = (sta == SD_OK ?  RES_OK : RES_ERROR);
	return res;
}


DRESULT sd_disk_write(BYTE lun, const BYTE * buff, DWORD sector, UINT count)
{
	SD_Error sta = SD_OK;
	DRESULT res = RES_OK;
	UINT n;
	DWORD lsector = sector;
	 uint8_t * pbuf = (uint8_t *)buff;
	  
	lsector <<= 9;
	if(((uint32_t)pbuf % 4) != 0)
	{
	 	for(n = 0; n < count; n++)
		{
			os_memcpy(sd_align_buf, pbuf, 512);
			while(sd_write_lock);
		 	sta=SD_WriteBlock(sd_align_buf,  lsector + 512 * n,  512);  // write operation for one sector
			pbuf += 512;
		} 
	}
	else
	{
		if(count == 1)
		{
		        while(sd_write_lock);
                      sta = SD_WriteBlock(pbuf, lsector, 512);    
		}
		          
		else
		{
		       while(sd_write_lock);
                     sta = SD_WriteMultiBlocks(pbuf, lsector, 512, count);
		}
		          	
	}
	res = (sta == SD_OK ?  RES_OK : RES_ERROR);
	return res;
}




uint16_t sd_mal_read(uint8_t lun, uint64_t mem_offset, uint32_t * buff, uint16_t len)
{
       #if 0
       SD_Error sta = SD_OK;
       uint16_t read_len;
       uint8_t * pbuf = (uint8_t *)buff;
       
       if(((uint32_t)pbuf % 4) != 0)
	{
	 	while(len)
		{
                     read_len = (len > 512 ?  512: len);
		 	sta =SD_ReadBlock(sd_align_buf,  mem_offset, 512);
			os_memcpy(pbuf,  sd_align_buf, read_len);
			len    -= read_len;
			pbuf += read_len;
			mem_offset += read_len;
		} 
	}
	else
	{
              uint16_t left_len = len % 512;
              uint16_t count      = len / 512;   // sectors to be read

		if(count)
              {
                       if(count == 1)
                       {
                              sta = SD_ReadBlock(pbuf, mem_offset, 512);
                       }
                       else
                       {
                                sta = SD_ReadMultiBlocks(pbuf, mem_offset, 512, count);
                       }   
              }
              if(left_len)
              {
                      sta =SD_ReadBlock(sd_align_buf, mem_offset + (uint64_t)count * 512,  512);
                      os_memcpy(&pbuf[count * 512],  sd_align_buf, left_len);
              }
	}
	return (uint16_t)sta;
	#else
	  uint8_t  res;

	  sd_read_lock++;
         res=SD_ReadDisk(lun, (u8*)buff, mem_offset>>9, len>>9);
         sd_read_lock--;
         return res;
	#endif
}

#if 0
uint16_t sd_mal_write(uint8_t lun, uint64_t mem_offset, uint32_t * buff, uint16_t len)
{
       SD_Error sta = SD_OK;
	uint16_t write_len;
	uint8_t * pbuf = (uint8_t *)buff;
	
	if(((uint32_t)pbuf % 4) != 0)
	{
	 	while(len)
		{
		       write_len = (len > 512 ?  512: len);

		       // read first, then write in
			if(write_len < 512)
			{
                            sta = SD_ReadBlock(sd_align_buf, mem_offset,  512);
			}
			os_memcpy(sd_align_buf, pbuf, write_len);
		 	sta = SD_WriteBlock(sd_align_buf,  mem_offset, 512);   // write operation for one sector
		 	len    -= write_len;
			pbuf += write_len;
			mem_offset += write_len;
		} 
	}
	else
	{
		 uint16_t left_len = len % 512;
               uint16_t count      = len / 512;   //  sectors to be write

               if(count)
               {
                      if(count == 1)
                      {
                              sta = SD_WriteBlock(pbuf, mem_offset, 512);
                      }
                      else
                      {
                             sta = SD_WriteMultiBlocks(pbuf,  mem_offset, 512,  count);
                      }
               }
               if(left_len)
               {
                       sta = SD_ReadBlock(sd_align_buf,  mem_offset + (uint64_t)count * 512,  512);  
                       os_memcpy(sd_align_buf, &pbuf[count * 512],  left_len);
                       sta = SD_WriteBlock(sd_align_buf,  mem_offset + (uint64_t)count * 512, 512);  
               }
	}
	return (uint16_t)sta;
}
#else
uint16_t sd_mal_write(uint8_t lun, uint64_t mem_offset, uint32_t * buff, uint16_t len)
{
      #if 0
     SD_Error res;

    
    //  return  SD_WriteDisk(lun, (const BYTE *)buff, mem_offset >> 9, len >> 9);
    res =  SD_WriteBlock((uint8_t*)buff, mem_offset, len);
    //os_printf("sd w: ad = 0x%lx, len = %d, res = %d\r\n", (u32)mem_offset,  len, res); //测试用
    return (uint16_t)res;
    #else
      uint8_t  res;
      sd_write_lock++;
      res=SD_WriteDisk(lun, (u8*)buff, mem_offset>>9, len>>9);   	
      sd_write_lock--;
     return res;
    #endif
}
#endif

DRESULT sd_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD*)buff = (DWORD)(SDCardInfo.CardCapacity /512);
    res = RES_OK;
    break;
  
  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
   *(WORD *)buff = 512;
   
    res = RES_OK;
    break;
  
  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = SDCardInfo.CardBlockSize;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}

