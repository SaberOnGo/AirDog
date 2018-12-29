
#include "GlobalDef.h"
#if (UGUI_USE_FONT != FONT_IN_ROM_FLASH)
#include "ugui_font.h"
#include "flash_font.h"
#include "os_global.h"
#include "sfud.h"


extern FONT_DESC font_desc[23];
extern uint8_t spi_flash_buf[4096];


FONT_DESC *  font_4x6    = &font_desc[0];
FONT_DESC *  font_5x8    = &font_desc[1];
FONT_DESC *  font_5x12  = &font_desc[2];
FONT_DESC *  font_6x8    = &font_desc[3];
FONT_DESC *  font_8x8    = &font_desc[4];
FONT_DESC *  font_6x10 = &font_desc[5];
 FONT_DESC *  font_7x12 = &font_desc[6];
FONT_DESC *  font_8x12 = &font_desc[7];
 FONT_DESC *  font_8x14 = &font_desc[8];

 FONT_DESC *  font_10x16 = &font_desc[16];
FONT_DESC *  font_12x16 =&font_desc[17];
FONT_DESC *  font_12x20 = &font_desc[18];
 FONT_DESC *  font_16x26 = &font_desc[19];
        
FONT_DESC *  font_22x36 = &font_desc[20];
FONT_DESC *  font_24x40 = &font_desc[21];
FONT_DESC *  font_32x53 = &font_desc[22];



 void UGUI_FONT_Init(void)
 {
        font_4x6    = &font_desc[0];
        font_5x8    = &font_desc[1];
        font_5x12 = &font_desc[2];
        font_6x8    = &font_desc[3];
        font_8x8    = &font_desc[4];
        font_6x10 = &font_desc[5];
        font_7x12 = &font_desc[6];
        font_8x12 = &font_desc[7];
        font_8x14 = &font_desc[8];

        font_10x16 = &font_desc[16];
        font_12x16 =&font_desc[17];
        font_12x20 = &font_desc[18];
        font_16x26 = &font_desc[19];
        
        font_22x36 = &font_desc[20];
        font_24x40 = &font_desc[21];
        font_32x53 = &font_desc[22];
 }





 
// 从 SPI FLASH 中获取pixel
// 参数: uint32_t offset: 字体编码的相对偏移
//              uint32_t size: 该字体编码的总长度
//              uint32_t index:   该像素点在该字体编码中的索引
// 返回值: 该像素点的值
uint8_t UGUI_FONT_GetPixel(FONT_DESC * desc, uint32_t index)
{
       static uint8_t pixel_buf[4096];
       static uint32_t last_index = 0;
       static FONT_DESC * last_desc = NULL;
       uint8_t pixel;
       uint32_t offset_in_sect = 0;  // 扇区内偏移
       static uint32_t left_read = 0;  // 已读长度
       

        if(last_index  <= index && index < (left_read + last_index) && last_desc == desc)
        {
              pixel = pixel_buf[index];
        }
        else
        {
               const sfud_flash *flash = sfud_get_device_table() + 0;
               uint32_t flashAddress = FLASH_FONT_LIB_START_SECTOR << 12;
               flashAddress += ((desc->offset +index) >> 12) << 12;
               offset_in_sect  = (desc->offset + index) % 4096;  // 扇区内偏移
               
               sfud_read(flash, flashAddress, 4096, pixel_buf);
              // os_memcpy(pixel_buf,  spi_flash_buf,  4096);
               pixel             = pixel_buf[offset_in_sect];
               left_read    = 4096 - offset_in_sect;
               last_index = index;
               last_desc        = desc;
        }
       return pixel;
}
#endif

