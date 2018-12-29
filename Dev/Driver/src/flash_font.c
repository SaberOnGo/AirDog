
#include "flash_font.h"


#include "ff.h"
#include "GlobalDef.h"
#include "fatfs_demo.h"
#include "os_global.h"

#include <stdio.h>
#include <string.h>
#include "sfud.h"
#include "spi_flash_interface.h"
#include "ff.h"


#define  READ_DBG_EN  1

#if READ_DBG_EN
#define READ_DBG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define READ_DBG(...)
#endif


#define  FLASH_CODE_BASE_OFFSET    

FONT_DESC font_desc[23] = 
{
      {
           0x99,   0,  // hash, index
           "font_4x6",
           256,  6,    // d0, d1
           1536,  // size, 数组大小
           0,         // offset, 相对偏移
           0x2132,  // font_sum
           0,           // reserved
           0x32,   // font_u8_sum
           0,          // desc_sum 
      },   

      {
           0xd8,   1,  // hash, index
           "font_5x8",
           256,  8,    // d0, d1
           2048,  // size, 数组大小
           0x800,         // offset, 相对偏移
           0x3e0d,  // font_sum
           0,           // reserved
           0x0d,   // font_u8_sum
           0,          // desc_sum 
      },   

       {
           0xdc,   2,  // hash, index
           "font_5x12",
           256,  12,    // d0, d1
           3072,  // size, 数组大小
           0x1000,         // offset, 相对偏移
           0x40d8,  // font_sum
           0,           // reserved
           0xd8,   // font_u8_sum
           0,          // desc_sum 
       },  

       {
           0x65 ,  3,  // hash, index
           "font_6x8",
           256,  8,    // d0, d1
           2048,  // size, 数组大小
           0x2000,         // offset, 相对偏移
           0x8930,  // font_sum
           0,           // reserved
           0x30,   // font_u8_sum
           0,          // desc_sum 
       },  

       {
           0x49 ,  4,  // hash, index
           "font_8x8",
           256,  8,    // d0, d1
           2048,  // size, 数组大小
           0x2800,         // offset, 相对偏移
           0x17dd2,  // font_sum
           0,           // reserved
           0xd2,   // font_u8_sum
           0,          // desc_sum 
       },

       {
           0xe8 ,  5,  // hash, index
           "font_6x10",
           256,  10,    // d0, d1
           2560,  // size, 数组大小
           0x3000,    // offset, 相对偏移
           0x6c28,   // font_sum
           0,               // reserved
           0x28,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0xf0,  6,  // hash, index
           "font_7x12",
           256,  12,    // d0, d1
           3072,  // size, 数组大小
           0x4000,    // offset, 相对偏移
           0x12fb7,   // font_sum
           0,               // reserved
           0xb7,       // font_u8_sum
           0,              // desc_sum 
       },

        {
           0x22,  7,  // hash, index
           "font_8x12",
           256,  12,    // d0, d1
           3072,  // size, 数组大小
           0x5000,    // offset, 相对偏移
           0x1e211,   // font_sum
           0,               // reserved
           0x11,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0x30,  8,  // hash, index
           "font_8x14",
           256,  14,    // d0, d1
           3584,  // size, 数组大小
           0x6000,    // offset, 相对偏移
           0x1d070,   // font_sum
           0,               // reserved
           0x70,       // font_u8_sum
           0,              // desc_sum 
       },

        {
           0x38,  9,  // hash, index
           "ft_unit_24_16",
           192,  1,    // d0, d1
           192,  // size, 数组大小
           0x7000,    // offset, 相对偏移
           0x110f,   // font_sum
           0,               // reserved
           0x0f,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0x25 ,  10,  // hash, index
           "font10_16",
           32,  1,    // d0, d1
           32,  // size, 数组大小
           0x7400,    // offset, 相对偏移
           0xd0,   // font_sum
           0,               // reserved
           0xd0,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0x31 ,  11,  // hash, index
           "font16_8",
           1521,  1,    // d0, d1
           1521,  // size, 数组大小
           0x7800,    // offset, 相对偏移
           0x117fd,   // font_sum
           0,               // reserved
           0xfd,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0x9f ,  12,  // hash, index
           "font20_12",
           3800,  1,    // d0, d1
           3800,  // size, 数组大小
           0x8000,    // offset, 相对偏移
           0x225c6,   // font_sum
           0,               // reserved
           0xc6,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0xdb ,  13,  // hash, index
           "font24_16",
           4560,  1,    // d0, d1
           4560,  // size, 数组大小
           0x9000,    // offset, 相对偏移
           0x14b2a,   // font_sum
           0,               // reserved
           0x2a,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0xf7 ,  14,  // hash, index
           "font26_16",
           4940,  1,    // d0, d1
           4940,  // size, 数组大小
           0xB000,    // offset, 相对偏移
          0x2c7fd,   // font_sum
           0,               // reserved
           0xfd,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0x4e ,  15,  // hash, index
           "num32_24",
           1536,  1,    // d0, d1
           1536,  // size, 数组大小
           0xD000,    // offset, 相对偏移
           0xdb72,   // font_sum
           0,               // reserved
           0x72,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0x46 ,  16,  // hash, index
           "font_10x16",
           256,  32,    // d0, d1
           0x2000,  // size, 数组大小
           0xE000,    // offset, 相对偏移
           0x2f379,   // font_sum
           0,               // reserved
           0x79,       // font_u8_sum
           0,              // desc_sum 
       },

        {
           0x6a ,  17,  // hash, index
           "font_12x16",
           256,  32,    // d0, d1
           0x2000,  // size, 数组大小
           0x10000,    // offset, 相对偏移
           0x55c24,   // font_sum
           0,               // reserved
           0x24,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0x47 ,  18,  // hash, index
           "font_12x20",
           256,  40,    // d0, d1
           0x2800,  // size, 数组大小
           0x12000,    // offset, 相对偏移
           0x5e515,   // font_sum
           0,               // reserved
           0x15,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0xd ,  19,  // hash, index
           "font_16x26",
           256,  52,    // d0, d1
           0x3400,  // size, 数组大小
           0x15000,    // offset, 相对偏移
           0x9eed6,   // font_sum
           0,               // reserved
           0xd6,       // font_u8_sum
           0,              // desc_sum 
       },

        {
           0xe6  ,  20,  // hash, index
           "font_22x36",
           256,  108,    // d0, d1
           0x6C00,  // size, 数组大小
           0x19000,    // offset, 相对偏移
           0x11fa89,   // font_sum
           0,               // reserved
           0x89,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0xeb,  21,  // hash, index
           "font_24x40",
           256,  120,    // d0, d1
           0x7800,  // size, 数组大小
           0x20000,    // offset, 相对偏移
           0x162d6a,   // font_sum
           0,               // reserved
           0x6a,       // font_u8_sum
           0,              // desc_sum 
       },

       {
           0xe1 ,  22,    // hash, index
           "font_32x53",
           256,  212,     // d0, d1
           0xD400,        // size, 数组大小
           0x28000,      // offset, 相对偏移
          0x79da0, // 0x2aa452,   // font_sum
           0,                    // reserved
           0x52,             // font_u8_sum
           0,                    // desc_sum 
       },
};


// 获取字体描述信息, 从FLASH中
// 参数: index: 必须大于或等于 1
//              FONT_DESC * desc: 输出的字体描述信息
SYS_RESULT FlashFont_GetFontDesc(uint8_t index,   FONT_DESC * desc)
{
       const sfud_flash *flash = sfud_get_device_table() + 0;
       uint32_t flashAddress = (FLASH_FONT_START_SECTOR) << 12;
        uint32_t flash_offset   =  4 + sizeof(FONT_DESC) * (index - 1);
        
       if(index == 0)
       {
                 READ_DBG("err: %s %d \r\n",   __FILE__, __LINE__);
                 return SYS_FAILED;
       }

       sfud_read(flash, flashAddress, 4096, spi_flash_buf);
       os_memcpy(desc,  &spi_flash_buf[flash_offset],   sizeof(FONT_DESC));
       return SYS_SUCCESS;
}

// 实际字体数目 
// 读取当前字体总数
uint32_t FlashFont_GetTotalFontNum(void)
{
        
       const sfud_flash *flash = sfud_get_device_table() + 0;
       sfud_err res;
       uint8_t  buffer[4];
       uint32_t *total = (uint32_t *)buffer;
       
	res = sfud_read(flash, FLASH_FONT_START_SECTOR << 12, 4,  buffer);
	if(res)
	{
              *total = 0xFF;
	}
	return *total;
}

// 创建字体描述信息并保存在FLASH中
void FlashFont_CreateFontDesc(void)
{
       uint32_t total_num = 23;
       const sfud_flash *flash = sfud_get_device_table() + 0;
       uint32_t flashAddress = FLASH_FONT_START_SECTOR << 12;
       
       os_memcpy(spi_flash_buf,  &total_num ,  4);
       os_memcpy(&spi_flash_buf[4],  font_desc,  sizeof(font_desc));
       
	//sfud_read(flash, flashAddress, 4096, spi_flash_buf);
	
	sfud_erase(flash, flashAddress, 4096);
	sfud_write(flash, flashAddress, 4096, spi_flash_buf);
}


// 字体编码文件校验
// 参数: uint32_t addr:  flash 地址
//              uint32_t: 要读取的FLASH 长度
// 返回值校验码
uint32_t GetFontCodeSum(uint32_t addr, uint32_t size)
{
        const sfud_flash *flash = sfud_get_device_table() + 0;
        uint32_t len;  // 单次读出长度
        uint32_t sum = 0;
        uint32_t offset_in_sect = addr % 4096;  // 扇区内的偏移

        while(size)
        {
                 len = (offset_in_sect  + size) > 4096 ? (4096 - offset_in_sect ) : size;  // 本次读出长度 : 1 - 4096 B
                 sfud_read(flash, addr, 4096, spi_flash_buf);

                 sum += Sys_GenSum32(spi_flash_buf, len);
                 // 单次读取完毕, 调整相关偏移量
                 offset_in_sect = 0;      // 地址偏移归零
                 addr += 4096;              // 扇区地址增加
                 size   -= len;                 // 待读取长度减少
        }
        return sum;        
}

// 字体编码文件验证

// index : 字体编码描述信息的序号： 0 - 22
void FlashFont_Check(uint8_t index)
{
      FONT_DESC desc;
      uint32_t base  = FLASH_FONT_LIB_START_SECTOR << 12;
      uint32_t new_sum = 0;
      
       FlashFont_GetFontDesc(index + 1, &desc);
       
       READ_DBG("idx=%d, offset=0x%lx, size=0x%lx\r\n", index, desc.offset, desc.size);
       
       new_sum = GetFontCodeSum(base + desc.offset, desc.size);

       desc.font_name[FONT_NAME_STR_LEN - 1] = 0;
       READ_DBG("%s valid %s, 0x%lx, 0x%lx\r\n",  desc.font_name,  
                                 (new_sum == desc.font_sum ?  "ok" : "failed"),  new_sum,  desc.font_sum);
}

// 字体编码是否已写入
// 返回值: E_TRUE: 已写入;   E_FALSE:  未写入
E_BOOL IsFontCodeWritten(void)
{
      
        FONT_DESC desc;
        uint32_t base  = FLASH_FONT_LIB_START_SECTOR << 12;
        uint32_t new_sum = 0;
        uint8_t i;
        
        for(i = 0; i < 23; i++)
        {
               FlashFont_GetFontDesc(i + 1, &desc);
               new_sum = GetFontCodeSum(base + desc.offset, desc.size);
               if(new_sum != desc.font_sum)
               {
                       READ_DBG("font idx = %d un written\r\n",  i);
                       return E_FALSE;
               }
        }
        READ_DBG("\r\n----------font code all check pass----------\r\n");
        return E_TRUE;
     
}

// 验证所有的字体编码有效性
void FlashFont_CheckAllFontCode(void)
{
         uint32_t total;
         uint8_t i;
         
         total = FlashFont_GetTotalFontNum();
         READ_DBG("has %d font_code \r\n",   total);
         for(i = 0; i < total;  i++)
         {
                 FlashFont_Check(i);
         }
}

// 写入字体编码
void FlashFont_WriteFontCode(FIL * pFileFIL, TCHAR *  fname)
{
      char cur_dir[64];
      FRESULT res = FR_OK;
      uint32_t bin_sum = 0;
      uint32_t total_len = 0;
      
      os_snprintf(cur_dir, sizeof(cur_dir), "0:/font/%s",  fname); 
	if(res == FR_OK)
	{
	    READ_DBG("find file path = %s\n",  cur_dir);
	    res = f_open(pFileFIL,  cur_dir, FA_OPEN_EXISTING | FA_READ);	  //打开文件
           if(res == FR_OK)
          {
                     uint32_t bytes_to_read = 0;
                     uint32_t flashAddress = FLASH_FONT_LIB_START_SECTOR << 12;
                     const sfud_flash *flash = sfud_get_device_table() + 0;
                     
			while(1)
			{
			    res = f_read(pFileFIL, spi_flash_buf, sizeof(spi_flash_buf), (UINT *)&bytes_to_read);  // 每次读取 4KB
		           if(res || bytes_to_read == 0)  /* 文件结束错误 */
		          {
		                       f_close(pFileFIL);
		                       READ_DBG("read file: res = %d, br = %d\r\n", res, bytes_to_read);
		                       // 文件全部读取完成, 则 res = 0, 同时 bytes_to_read 为 0
                                    if(FR_OK == res && 0 == bytes_to_read)
					{
					       READ_DBG("read file all done\n"); 
                                          do{
						       READ_DBG("\r\nfile: %s\r\n\r\n",  cur_dir);
						       READ_DBG("read ok, len = %d, file_size = %ld Bytes\r\n", 
						                                 bytes_to_read, pFileFIL->fsize);
						}while(0);
					}
					break;
		              }
				else
				{
				      sfud_err err0, err1;
				      
                                   err0 = sfud_erase(flash, flashAddress, 4096);
                                   err1 = sfud_write(flash, flashAddress, 4096, spi_flash_buf);
                                   flashAddress += 4096;
                                   bin_sum = Sys_GenSum32(spi_flash_buf,  bytes_to_read);
                                   total_len += bytes_to_read;
                                   
					if(bytes_to_read < sizeof(spi_flash_buf)) 
					{
                                          READ_DBG("bin sum = 0x%lx, total_len = %ld \r\n",  bin_sum,  total_len);
					}  // 文件结束
				       READ_DBG("read ok, len = %d, err0 = %d, err1 = %d\r\n", 
				                                 bytes_to_read,   err0, err1);
				}
			}  
              }
		else{ READ_DBG("can't open file, error = %d, %s %d\n", res, __FILE__, __LINE__); }
	}
}

// 找到目录内匹配的文件
void FlashFont_FindMatchFile(void)
{
    FRESULT fr;     /* Return value */
    DIR dj;         /* Directory search object */
    FILINFO fno;    /* File information */

    fr = f_findfirst(&dj, &fno, "0:/font",  "*.bin");  /* Start to search for photo files */
    if(fr)
    {
           READ_DBG("res = %d %s %d \r\n",  fr,   __FILE__,  __LINE__);
    }
    while (fr == FR_OK && fno.fname[0]) {         /* Repeat while an item is found */
          os_printf("find: %s\n", fno.fname);                /* Display the object name */
          FlashFont_WriteFontCode(&fileFIL, fno.fname);
          fr = f_findnext(&dj, &fno);               /* Search for next item */
    }
    f_closedir(&dj);
}

uint8_t flash_font_write = 0;
#include "delay.h"

void FlashFont_Task(void)
{
#if (UGUI_USE_FONT != FONT_IN_ROM_FLASH)
      #if 0
      if(FlashFont_GetTotalFontNum() > 128)
      {
            READ_DBG("write font desc into flash\r\n");
            FlashFont_CreateFontDesc();
      }
      else
      {
              READ_DBG("font desc is written\r\n");
      }
      #endif
      
       //if(! IsFontCodeWritten())   // 字体库未写入
       if(flash_font_write)
       {
             READ_DBG("ready to write font code \r\n");
             FlashFont_CreateFontDesc();
             FlashFont_FindMatchFile();   // 查找字体库 bin文件并写入
             FlashFont_CheckAllFontCode();

              flash_font_write = 0;
       } 
       else
       {
              READ_DBG("font code is written\r\n");
       }
#endif
}


