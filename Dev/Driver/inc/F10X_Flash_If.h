
#ifndef __F10X_FLASH_IF_H__
#define  __F10X_FLASH_IF_H__

#include "GlobalDef.h"

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif



#pragma pack(1)
// 存储在FLASH 的升级相关的属性
typedef struct
{
       uint8_t  fileName[64];     // 是否升级
	uint32_t fileVersion;      // 版本号
	uint32_t fileLen;           // 文件大小, unit: Byte
	uint32_t fileSum;           // 文件校验和
	T_RTC_TIME time;           // 烧写时间
	uint32_t upgrade;           // 是否升级
	uint32_t upgrade_inverse;  // 是否升级的反码
	uint32_t checkSum;          // 对以上数据的和校验
	uint32_t checkSumInverse;  // 校验和的反码
}T_APP_FLASH_ATTR;

// 存储在SPI FLASH 的系统环境变量
typedef struct
{
    uint8_t   is_hcho_cal;       // 甲醛是否已校正
    uint8_t   hold;               // 是否保持显示
    uint8_t   reserved[2];
	uint16_t  cali_ppb;
	uint16_t  cali_mass;
	uint32_t  checkSum;          // 对以上数据的校验和
	uint32_t checkSumInverse;  // 校验和的反码
}T_SYS_ENV;
#pragma pack()


#define  FLASH_SUCCESS     0
#define  FLASH_FAILED      1


void     F10X_FLASH_Init(void);
uint32_t    F10X_FLASH_Erase(uint32_t start_addr, uint32_t size);
uint32_t    F10X_FLASH_Write(__IO uint32_t * FlashAddress, uint32_t* Data, uint32_t DataLength);
uint8_t      F10X_FLASH_GetWriteProtectionFlag(void);
void     F10X_FLASH_DisableWriteProtectionPages(void);
uint8_t      F10X_FLASH_Upgrade(void);


uint8_t F10X_FLASH_WriteAppAttr(uint32_t flashAddress, T_APP_FLASH_ATTR * newAttr);
uint8_t F10X_FLASH_ReadAppAttr(uint32_t flashAddress, T_APP_FLASH_ATTR * outAttr);

uint8_t F10X_FLASH_WriteSysEnv(T_SYS_ENV * pEnv);
uint8_t F10X_FLASH_ReadSysEnv(T_SYS_ENV * outEnv);


#define Sys_ReadAppAttr(pAttr)   F10X_FLASH_ReadAppAttr(FLASH_SYS_ENV_START_SECTOR << 12,  pAttr)
#define Sys_WriteAppAttr(pAttr)  F10X_FLASH_WriteAppAttr(FLASH_SYS_ENV_START_SECTOR << 12, pAttr)

#endif

