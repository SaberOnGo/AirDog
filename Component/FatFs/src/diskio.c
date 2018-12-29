#include "diskio.h"
#include <stdint.h>

#include "spi_flash_interface.h"
#include "rom_flash_interface.h"
#include "sd_card_interface.h"
#include "GlobalDef.h"

// Inidialize a Drive
DSTATUS disk_initialize(BYTE drv)		/* Physical drive nmuber(0..) */
{
	DSTATUS res = STA_NOINIT;
	
	switch(drv)
	{
	   #if FLASH_DISK_EN
	   case FLASH_DISK:  // spi flash
	   {
	   	   res = (flash_initialize() ? STA_NOINIT : 0);
	   }break;
       #endif
	   
	   #if SD_DISK_EN
	   case SD_DISK:  // SD 卡 
	   {
	   	   res = (sd_initialize() ? STA_NOINIT : 0);
	   }break;
       #endif
	   
	   #if ROM_DISK_EN
	   case ROM_DISK:  // rom flash 
	   {
	   	   res = (rom_flash_initialize() ? STA_NOINIT : 0);
	   }break;
	   #endif
	   
	   default:
	   	 break;
	}
    return res;
}



DSTATUS disk_status(BYTE drv)			/* Physical drive nmuber(0..) */
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read(
	BYTE drv,		/* Physical drive nmuber(0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address(LBA) */
	UINT count		/* Number of sectors to read(1..255) */
)
{	
    if(!count)
    {
		return RES_PARERR; // count不能等于0，否则返回参数错误
    }
	
	switch(drv)
	{
	    #if FLASH_DISK_EN
	    case FLASH_DISK:
		{
		    flash_disk_read(drv, buff, sector, count);
		}break;
		#endif

		#if SD_DISK_EN
		case SD_DISK:
		{
                    //sd_disk_read(drv, buff, sector, count);
                    SD_ReadDisk(drv, buff, sector, count);
		}break;
		#endif

		#if ROM_DISK_EN
		case ROM_DISK:
		{
		    rom_flash_disk_read(drv, buff, sector, count);
		}break;
		#endif
		
		default:
		  return RES_PARERR;
	}
	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _USE_WRITE == 1
DRESULT disk_write(
	BYTE drv,			/* Physical drive nmuber(0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address(LBA) */
	UINT count			/* Number of sectors to write(1..255) */
)
{
	if(!count)
    {
		return RES_PARERR; // count不能等于0，否则返回参数错误
    }
	 
	switch(drv)
	{
	    #if FLASH_DISK_EN
	    case FLASH_DISK:
		{
		    flash_disk_write(drv, buff, sector, count);
		}break;
		#endif

		#if SD_DISK_EN
		case SD_DISK:
		{
                     //sd_disk_write(drv, buff, sector, count);		
                     SD_WriteDisk(drv, (BYTE *)buff, sector, count);
		}break;
		#endif

		#if ROM_DISK_EN
		case ROM_DISK:
		{
		    rom_flash_disk_write(drv, buff, sector, count);
		}break;
		#endif
		
		default:
		  return RES_PARERR;
	}
	
	return RES_OK;	
}
#endif /* _USE_WRITE == 1 */




/************************************************************************************
CTRL_SYNC			Make sure that the disk drive has finished pending write process. 
					When the disk I/O module has a write back cache, flush the dirty sector immediately. 
					This command is not used in read-only configuration. 
GET_SECTOR_SIZE		Returns sector size of the drive into the WORD variable pointed by Buffer. 
					This command is not used in fixed sector size configuration, _MAX_SS is 512. 
GET_SECTOR_COUNT	Returns number of available sectors on the drive into the DWORD variable pointed by Buffer.
					This command is used by only f_mkfs function to determine the volume size to be created. 
GET_BLOCK_SIZE		Returns erase block size of the flash memory in unit of sector into the DWORD variable pointed by Buffer.
					The allowable value is 1 to 32768 in power of 2. 
					Return 1 if the erase block size is unknown or disk devices. 
					This command is used by only f_mkfs function and it attempts to align data area to the erase block boundary. 
CTRL_ERASE_SECTOR	Erases a part of the flash memory specified by a DWORD array {<start sector>, <end sector>} pointed by Buffer. 
					When this feature is not supported or not a flash memory media, this command has no effect.
					The FatFs does not check the result code and the file function is not affected even if the sectors are not erased well. 
					This command is called on removing a cluster chain when _USE_ERASE is 1. 
************************************************************************************/
DRESULT disk_ioctl(
	BYTE drv,		/* Physical drive nmuber(0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	
	switch(drv)
	{
	    #if FLASH_DISK_EN
	    case FLASH_DISK:
		{
		    res = flash_ioctl(drv, ctrl, buff);
		}break;
		#endif

		#if SD_DISK_EN
		case SD_DISK:
		{
			res = sd_ioctl(drv, ctrl, buff);
		}break;
		#endif

		#if ROM_DISK_EN
		case ROM_DISK:
		{
		    res = rom_flash_ioctl(drv, ctrl, buff);
		}break;
		#endif
		
		default:
		  return RES_PARERR;
	}
	
	return res;
}

DWORD get_fattime(void)
{
/*
 t = Time_GetCalendarTime();
    t.tm_year -= 1980;  //年份改为1980年起
    t.tm_mon++;          //0-11月改为1-12月
    t.tm_sec /= 2;       //将秒数改为0-29
    
    date = 0;
    date = (t.tm_year << 25) | (t.tm_mon<<21) | (t.tm_mday<<16)|\
            (t.tm_hour<<11) | (t.tm_min<<5) | (t.tm_sec);

    return date;
*/
	return 0;
}

