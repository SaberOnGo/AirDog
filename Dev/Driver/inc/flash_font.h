
#ifndef  __FLASH_FONT_H__
#define  __FLASH_FONT_H__


#include <stdint.h>



#pragma pack(1)

#define FONT_NAME_STR_LEN  20

// 字体编码起始扇区
#define  FLASH_FONT_LIB_START_SECTOR  (FLASH_FONT_START_SECTOR + 1)


// 字体信息描述
typedef struct
{
       uint8_t     name_hash;  // font_name 的hash 编码
       uint16_t   index;    // 索引
       uint8_t    font_name[FONT_NAME_STR_LEN];
       uint16_t   d0;                // 1维数组大小
       uint16_t   d1;               // 2维数组大小
      // uint16_t   char_size;    // 单个字符长度： B
       uint32_t   size;             // 数组总长度
       uint32_t   offset;          // 在FLASH中的地址偏移
       uint32_t   font_sum;    // 字体编码的校验和
       uint32_t   reserved;    // 保留
       
       uint8_t     font_u8_sum;
       uint8_t     desc_sum;  // 字体描述信息结构体的校验和
}FONT_DESC;
#pragma pack()


void FlashFont_Task(void);

#endif

