
#include "F10X_Flash_If.h"
#include "os_global.h"




/* Private function prototypes -----------------------------------------------*/   
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
uint32_t BlockNbr = 0, UserMemoryMask = 0;


void F10X_FLASH_Init(void)
{
    STM32_FLASH_Unlock();
	
	/* Clear pending flags (if any) */  
    STM32_FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPTERR | FLASH_FLAG_WRPRTERR | 
                  FLASH_FLAG_PGERR);
}

#define F10X_FLASH_Lock()  STM32_FLASH_Lock()


// 计算 UserMemoryMask
// 返回值: 是否写保护
uint8_t F10X_FLASH_GetWriteProtectionFlag(void)
{
    uint8_t FlashProtection = 0;  // 是否写保护: 1: 写保护; 0: 无写保护
		
    /* Get the number of block (4 or 2 pages) from where the user program will be loaded */
    BlockNbr = (APPLICATION_ADDRESS - 0x08000000) >> 12;

/* Compute the mask to test if the Flash memory, where the user program will be
     loaded, is write protected */
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
  UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
#else /* USE_STM3210E_EVAL */
  if (BlockNbr < 62)
  {
    UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
  }
  else
  {
    UserMemoryMask = ((uint32_t)0x80000000);
  }
#endif /* (STM32F10X_MD) || (STM32F10X_MD_VL) */

/* Test if any page of Flash memory where program user will be loaded is write protected */
  if ((STM32_FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
  {
    os_printf("STM32F10X FLASH Write Protection\n");
    FlashProtection = 1;
  }
  else
  {
    os_printf("STM32F10X FLASH No Write Protection\n");
    FlashProtection = 0;
  }

   return FlashProtection;

}

/**
  * @brief  Disable the write protection of desired pages
  * @param  None
  * @retval None
  */
void F10X_FLASH_DisableWriteProtectionPages(void)
{
  uint32_t useroptionbyte = 0, WRPR = 0;
  uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
  FLASH_Status status = FLASH_BUSY;

  

  
  WRPR = STM32_FLASH_GetWriteProtectionOptionByte();

  /* Test if user memory is write protected */
  if ((WRPR & UserMemoryMask) != UserMemoryMask)
  {
    useroptionbyte = STM32_FLASH_GetUserOptionByte();

    UserMemoryMask |= WRPR;

    status = FLASH_EraseOptionBytes();

    if (UserMemoryMask != 0xFFFFFFFF)
    {
      os_printf("enable write protection file = %s, line = %d\n", __FILE__, __LINE__);
      status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
    }

    /* Test if user Option Bytes are programmed */
    if ((useroptionbyte & 0x07) != 0x07)
    { 
      /* Restore user Option Bytes */
      if ((useroptionbyte & 0x01) == 0x0)
      {
        var1 = OB_IWDG_HW;
      }
      if ((useroptionbyte & 0x02) == 0x0)
      {
        var2 = OB_STOP_RST;
      }
      if ((useroptionbyte & 0x04) == 0x0)
      {
        var3 = OB_STDBY_RST;
      }

      FLASH_UserOptionByteConfig(var1, var2, var3);
    }

    if (status == FLASH_COMPLETE)
    {
      os_printf("Write Protection disabled...\r\n");

      os_printf("...and a System Reset will be generated to re-load the new option bytes\r\n");

      /* Generate System Reset to load the new option byte values */
      NVIC_SystemReset();
    }
    else
    {
      os_printf("Error: Flash write unprotection failed...\r\n");
    }
  }
  else
  {
    os_printf("Flash memory not write protected\r\n");
  }
}

/**
  * @brief  Calculate the number of pages, 计算 bin 文件占用多少页(中等密度 1页为1KB)
  * @param  Size: The image size, bin 文件的大小, 单位: 字节
  * @retval The number of pages, 返回值: 页数目
  */
static uint32_t Flash_PagesMask(__IO uint32_t Size)
{
  uint32_t pagenumber = 0x0;
  uint32_t size = Size;

  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;

}

/*******************************
功能: 擦除FLASH
参数: uint32_t  start_addr : 应用程序起始地址
             uint32_t  size:  需要擦除的大小, unit: Byte, 一般是新bin文件的大小
 返回值: 操作是否成功: 0:成功;   1: 失败
**********************************/
uint32_t F10X_FLASH_Erase(uint32_t start_addr, uint32_t size)
{
    uint32_t EraseCounter = 0;
    uint32_t NbrOfPage    = 0;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	
	/* Erase the needed pages where the user application will be loaded */
    /* Define the number of page to be erased  需要擦除的页总数 */
	
    NbrOfPage = Flash_PagesMask(size);

	/* Erase the FLASH pages */
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
         FLASHStatus = FLASH_ErasePage(start_addr + (PAGE_SIZE * EraseCounter));
    }
	if(FLASHStatus != FLASH_COMPLETE)return FLASH_FAILED;

	return FLASH_SUCCESS;
}



/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   , 注意DataLength 是以32bit (字长为单位的)
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t F10X_FLASH_Write(__IO uint32_t * FlashAddress, uint32_t* Data, uint32_t DataLength)
{
	uint32_t i = 0;
	
	for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS - 4)); i++)
	{
	  /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
		 be done by word */ 
	  if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data + i)) == FLASH_COMPLETE)
	  {
	   /* Check the written value */
		if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data + i))
		{
		  /* Flash content doesn't match SRAM content */
		  return(2);
		}
		/* Increment FLASH destination address */
		*FlashAddress += 4;
	  }
	  else
	  {
		/* Error occurred while writing data in Flash memory */
		return (1);
	  }
	}
	
	return (0);
}


#include "sfud.h"

extern uint8_t spi_flash_buf[4096];

// 写入APP 属性
// 成功: 返回0;  失败； 1
uint8_t F10X_FLASH_WriteAppAttr(uint32_t flashAddress, T_APP_FLASH_ATTR * newAttr)
{
   const sfud_flash *flash = sfud_get_device_table() + 0;
   T_APP_FLASH_ATTR * flashAttr = (T_APP_FLASH_ATTR *)spi_flash_buf;
   
   if(NULL == newAttr){ INSERT_ERROR_INFO(0); return FLASH_FAILED; }
   os_printf("write flashAddr = 0x%x, sector = %d\n", flashAddress, flashAddress >> 12);
   
   sfud_read(flash, flashAddress, 4096, spi_flash_buf);
   os_memcpy(spi_flash_buf, newAttr, sizeof(T_APP_FLASH_ATTR));
   
   flashAttr->checkSum		  =  Sys_GenSum32(flashAttr, sizeof(T_APP_FLASH_ATTR) - 8);
   flashAttr->checkSumInverse	=  ~(flashAttr->checkSum);
   os_printf("w: check = 0x%x, checkInverse = 0x%x\n", flashAttr->checkSum, flashAttr->checkSumInverse);
   
   sfud_erase(flash, flashAddress, 4096);
   sfud_write(flash, flashAddress, 4096, spi_flash_buf);

   sfud_read(flash, flashAddress, 4096, spi_flash_buf);
   os_printf("r: check = 0x%x, checkInverse = 0x%x\n", flashAttr->checkSum, flashAttr->checkSumInverse);
   
   return FLASH_SUCCESS;
}

// 读取APP 属性
// 成功: 返回0;  失败； 1
uint8_t F10X_FLASH_ReadAppAttr(uint32_t flashAddress, T_APP_FLASH_ATTR * outAttr)
{
       uint8_t buf[sizeof(T_APP_FLASH_ATTR)];
       uint32_t sumResult = 0;
      const sfud_flash *flash = sfud_get_device_table() + 0;


	if(NULL == outAttr){ INSERT_ERROR_INFO(0); return FLASH_FAILED; }
	os_printf("read flashAddr = 0x%x, sector = %d\n", flashAddress, flashAddress >> 12);
	
       sfud_read(flash, flashAddress, sizeof(buf), buf); 
	sumResult = Sys_GenSum32(buf, sizeof(T_APP_FLASH_ATTR) - 8);  // 除8个字节的校验码外, 其它都需校验
	os_memcpy(outAttr, buf, sizeof(T_APP_FLASH_ATTR));

	if(sumResult != outAttr->checkSum || (outAttr->checkSum != (~outAttr->checkSumInverse)))
	{
	      os_printf("read APP attr failed\n");
	      os_printf("read APP attr failed, newSum = 0x%x, checkSum = 0x%x, checkSumInverse = 0x%x\r\n", 
	   	                    sumResult, outAttr->checkSum, outAttr->checkSumInverse);
	   return FLASH_FAILED;  // failed
	}

	return FLASH_SUCCESS;  // success
}

/************************************
功能: 读取SPI FLASH的升级文件并升级
参数: 

************************************/
uint8_t F10X_FLASH_Upgrade(void)
{
	uint32_t alignSize = 0;  // 按4字节对齐后的bin文件大小
	uint32_t readSize  = 0;
	uint8_t res = FLASH_SUCCESS;
	const sfud_flash *flash = sfud_get_device_table() + 0;
       T_APP_FLASH_ATTR appAttr;
	__IO uint32_t appAddress = APPLICATION_ADDRESS;               // 应用程序的 FLASH ROM 起始地址
	uint32_t externAppSectorAddr = FLASH_APP1_START_SECTOR;        // 存在外部 SPI FLASH 的新bin文件的起始扇区

	os_printf("start upgrade\n");
	if(Sys_ReadAppAttr(&appAttr) != FLASH_SUCCESS){ INSERT_ERROR_INFO(0); return FLASH_FAILED; }
       os_printf("check upgrade para ok\n");
	
	alignSize = appAttr.fileLen / 4;
	if(appAttr.fileLen % 4)
	{
	       alignSize += 1;
	}
	alignSize *= 4;

       F10X_FLASH_Init();
	F10X_FLASH_Erase(appAddress, alignSize);  // 擦除应用程序
	 while(alignSize)
	 {
	     if(alignSize >= sizeof(spi_flash_buf))
	     {
	         alignSize -= sizeof(spi_flash_buf);
			 readSize = sizeof(spi_flash_buf);
	     }
		 else if(alignSize)
		 {
		    readSize = alignSize;
			alignSize = 0;
		 }
	        sfud_read(flash, externAppSectorAddr << 12, sizeof(spi_flash_buf), spi_flash_buf); 
		 if(F10X_FLASH_Write(&appAddress, (uint32_t *)spi_flash_buf, readSize / 4) != 0)
		 {
		       res = FLASH_FAILED;
			os_printf("upgrade error, res = %d\n", res);
			break;
		 }
		 os_printf("pro next, alignSize = %d\n", alignSize);
		 externAppSectorAddr += 1;  // 扇区 + 1, 一个扇区为 4096 B
	 }
        F10X_FLASH_Lock();

	 

	 if(res == FLASH_SUCCESS)	
	     os_printf("programm success, alignSize = %ld Bytes, fileSize = %ld Bytes\r\n", 
				     alignSize, appAttr.fileLen);
	return res;	 
}

/***************************************升级功能相关 end *****************************/


// 读取系统环境变量
// 成功: 返回0;  失败； 1
uint8_t F10X_FLASH_ReadSysEnv(T_SYS_ENV * outEnv)
{
    uint8_t buf[sizeof(T_SYS_ENV)];
	
    uint32_t sumResult = 0;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    uint32_t flashAddress = (FLASH_SYS_ENV_START_SECTOR << 12) + DWORD_ALIGNED_LEN(T_APP_FLASH_ATTR);

	if(NULL == outEnv){ INSERT_ERROR_INFO(0); return FLASH_FAILED; }
	
    sfud_read(flash, flashAddress, sizeof(buf), buf); 
	sumResult = Sys_GenSum32(buf, sizeof(T_SYS_ENV) - 8);  // 除8个字节的校验码外, 其它都需校验
	os_memcpy(outEnv, buf, sizeof(T_SYS_ENV));
	
	if(sumResult != outEnv->checkSum || (outEnv->checkSum != (~outEnv->checkSumInverse)))
	{
	   os_printf("read SYS ENV failed, newSum = 0x%x, checkSum = 0x%x, checkSumInverse = 0x%x\r\n", 
	   	           sumResult, outEnv->checkSum, outEnv->checkSumInverse);
	   return FLASH_FAILED;  // failed
	}

	return FLASH_SUCCESS;  // success    
}

// 写入系统环境变量
// 成功: 返回0;  失败； 1
uint8_t F10X_FLASH_WriteSysEnv(T_SYS_ENV * pEnv)
{
	const sfud_flash *flash = sfud_get_device_table() + 0;
       T_SYS_ENV * flashEnv = (T_SYS_ENV *)&spi_flash_buf[DWORD_ALIGNED_LEN(T_APP_FLASH_ATTR)];
        uint32_t flashAddress = FLASH_SYS_ENV_START_SECTOR << 12;
		
	if(NULL == flashEnv){ INSERT_ERROR_INFO(0); return FLASH_FAILED; }

	sfud_read(flash, flashAddress, 4096, spi_flash_buf);
	os_memcpy(flashEnv, pEnv, sizeof(T_SYS_ENV));

	flashEnv->checkSum		  =  Sys_GenSum32(flashEnv, sizeof(T_SYS_ENV) - 8);
	flashEnv->checkSumInverse	=  ~(flashEnv->checkSum);
	os_printf("SYS ENV w: check = 0x%x, checkInverse = 0x%x\n", flashEnv->checkSum, flashEnv->checkSumInverse);

	sfud_erase(flash, flashAddress, 4096);
	sfud_write(flash, flashAddress, 4096, spi_flash_buf);

	sfud_read(flash, flashAddress, 4096, spi_flash_buf);
	os_printf("SYS ENV r: check = 0x%x, checkInverse = 0x%x\n", flashEnv->checkSum, flashEnv->checkSumInverse);

	return FLASH_SUCCESS;

}
