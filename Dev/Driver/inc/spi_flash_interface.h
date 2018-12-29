
/*********************************** SPI FLASH 存储接口访问头文件 ********************************************/
#ifndef __SPI_FLASH_INTERFACE_H__
#define  __SPI_FLASH_INTERFACE_H__

#include <stdint.h>
#include "diskio.h"
#include "Integer.h"

#include "Ffconf.h"
#include "GlobalDef.h"

//Flash相关参数
/************************************************************************************************************************************************************
 spi flash 物理扇区大小为4KB, 物理总扇区数为512, 容量 2MB
 其中 260KB 空间 为 应用程序和系统环境变量存储区
| <---sector 0 - sector 31--->  | <-----------sector 32 ~ 63-----------> | <--sector 64 ----- >  |<-sector 65 ~ sector 511-------->   |
| <-----128 KB -----------> | <--------128 KB--------------------> | <------4KB------->|---------文件系统---------- -|
| APP1 备份区(新程序)     |  APP2 备份区(上一个版本程序)    |   系统参数区        |---------文件系统----------- |  
|-------------------------- 260 KB (65个物理扇区)    --------------------------------|--1788 KB FatFs 文件系统区---|
*************************************************************************************************************************************************************/
#define FL_SIZE_KB(count)    (((uint32_t)count) * 1024L )    // 表示有多少KB


// SPI FLASH 物理大小实际定义
extern uint32_t flash_get_total_capacity(void);

// SPI FLASH 物理大小实际定义
#if 1
#define FL_TOTAL_SIZE      flash_get_total_capacity()
#else
#define FL_TOTAL_SIZE      FL_SIZE_KB(4096)     // W25Q32
#endif

#define FL_SECTOR_SIZE     (4096L)     // 物理扇区实际大小
#define FL_PAGE_SIZE	   256L        //  可编程的最小单位: 页, 的大小, Unit: B
#define FL_PAGES_PER_SECTOR	(FL_SECTOR_SIZE / FL_PAGE_SIZE)   // 每扇区的页数


// FAT 逻辑扇区相关定义
#if (_MAX_SS == 4096)
#define SPI_FLASH_SECTOR_SIZE		(4096L)            //  FatFs 扇区大小: 4KB, 与 SPI FLASH 的扇区大小一致(4KB)
#elif (_MAX_SS == 512)
#define SPI_FLASH_SECTOR_SIZE		(512L)             //  FatFs扇区大小: 512B, 与 SPI FLASH的扇区(4KB)大小不同
#else
#define "flash macro define error"
#endif

/************************************************************************************************************************************************************
 spi flash 物理扇区大小为4KB, 物理总扇区数为512, 容量 2MB
 其中 260KB 空间 为 应用程序和系统环境变量存储区
| <---sector 0 - sector 31--->  | <-----------sector 32 ~ 63-----------> | <--sector 64 ----- >  |<-sector 65 ~ sector 511-------->   |
| <-----128 KB -----------> | <--------128 KB--------------------> | <------4KB------->|---------文件系统---------- -|
| APP1 备份区(新程序)     |  APP2 备份区(上一个版本程序)    |   系统参数区        |---------文件系统----------- |  
|-------------------------- 260 KB (65个物理扇区)    --------------------------------|--1788 KB FatFs 文件系统区---|
*************************************************************************************************************************************************************/

#define SPI_FLASH_BLOCK_SIZE		FL_SECTOR_SIZE               // 65536    // 块大小: 64 KB,  Block Size in Bytes 
#define SPI_FLASH_START_ADDR        (FLASH_FATFS_START_SECTOR * FL_SECTOR_SIZE)        //  FATFS系统起始地址, 从第65物理扇区开始
#define SPI_FLASH_SECTOR_COUNT		((FL_TOTAL_SIZE  - SPI_FLASH_START_ADDR) / SPI_FLASH_SECTOR_SIZE)  //  SPI FLASH 中的FatFs可用总扇区数

	

//CS片选信号
#define SPI_FLASH_CS_0()			(GPIOB->BRR  = GPIO_Pin_12)  // GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_FLASH_CS_1()			(GPIOB->BSRR = GPIO_Pin_12)  // GPIO_SetBits(GPIOB,   GPIO_Pin_12)

extern uint8_t spi_flash_buf[4096];

void flash_reset(void);

int32_t  flash_initialize(void);
DRESULT flash_ioctl(BYTE lun, BYTE cmd, void *buff);
DRESULT flash_disk_read(BYTE lun, BYTE *buff, DWORD sector, UINT count);
DRESULT flash_disk_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);

uint16_t flash_mal_write(uint8_t lun, uint32_t addr, uint32_t * buffer, uint16_t bytes);
uint16_t flash_mal_read(uint8_t lun, uint32_t addr, uint32_t * buffer, uint16_t bytes);

void flash_set_total_capacity(uint32_t capacity);
uint32_t flash_get_total_capacity(void);


#endif   // end of file

