
#ifndef __GLOBAL_DEF_H__
#define  __GLOBAL_DEF_H__

#include "stm32f10x.h"

/******************************** 编译开关begin *********************************/
#define SYS_DEBUG  //调试

#ifdef SYS_DEBUG
#define UART_DEBUG  //串口调试
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <reglib.h>




void DbgInfo_Print(char * fmt,  ...);

#define dbg_print_detail(fmt, ...)  { DbgInfo_Print(fmt, ##__VA_ARGS__); \
/*DbgInfo_Print("%s %d\r\n",  __FILE__,  __LINE__); */}

#if 1
//#define os_printf(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#define os_printf    dbg_print_detail
#else
#define os_printf(...)
#endif





// 模块调试使能开关, 1: 使能; 0: 禁止
#define DEBUG_ADC_EN      0    // ADC 调试
#define BAT_DEBUG_EN      1    // 电池电量调试
#define EXTI_DEBUG_EN     0    // 外部中断调试使能(1), 禁止(0)
#define RTC_DEBUG_EN      0
#define DEBUG_KEY_EN      0
#define FAT_DEBUG_EN      1    // 文件操作调试使能
#define SDRR_DBG_EN        0    
#define SYSTEM_DBG_EN     0    // 系统调试输出
#define BEEP_DBG_EN            0   // 蜂鸣器调试使能


// 模式使能
#define MODULE_USB_EN     1  // USB, FatFs 模式使能(1), 禁止(0)
#define MODULE_GUI_EN      1   // GUI 界面是否使能
#define MODULE_SDRR_EN  1



#define  UART_BLOCK    0    // 串口阻塞式打印, 查询等待
#define  UART_QUEUE   1    // 串口队列式打印, 中断循环缓冲区实现

#define PRINT_SELECT   UART_QUEUE


#define GIZ_MCU  1    // 机智云引擎在MCU端，使用机智云串口协议
#define GIZ_SOC  2    // 机智云引擎在WIFI端, 使用自定义串口协议


#define  GIZWITS_TYPE    GIZ_MCU 
#define  GIZ_UART_SEL     UART_QUEUE    // 队列式输出

#if GIZWITS_TYPE
#define MODULE_GIZ_OTA_EN   0   // 机智云 OTA 升级功能
#endif

#define GIZ_SOFT_VERSION "03010101"
#define GIZ_HARD_VERSION "03010100"


#define LITTLE_ENDIAN   0
#define BIG_ENDIAN      1

#define FONT_IN_SPI_FLASH       1  // use font lib saved outside, in spi flash
#define FONT_IN_ROM_FLASH    2   // use font lib saved in MCU ROM, if use this, rom will be used more than 35KB

#define UGUI_USE_FONT   FONT_IN_SPI_FLASH


extern void print_info(char * info, u8 * file, u32 line);

#define INSERT_DEBUG_INFO(expr, string) ((expr) ? (void)0 : print_info(string, (uint8_t *)__FILE__, __LINE__))
#define INSERT_ERROR_INFO(err)   //INSERT_DEBUG_INFO(0, err)
#define ASSERT_PARAM(param)      //INSERT_DEBUG_INFO(param, " ")

#define  MCU_ROM_SIZE           (256 * 1024L)   // 256 KB ROM FLASH

/************************************************************************************************************************************************************
 spi flash 物理扇区大小为4KB, 物理总扇区数为512, 容量 2MB
 其中 260KB 空间 为 应用程序和系统环境变量存储区
| <---sector 0 - sector 63--->  | <-----------sector 64 ~ 127-----------> | <--sector 128 ----- >  |<-sector 65 ~ sector 511-------->   |
| <-----256 KB -----------> | <--------256 KB--------------------> | <------4KB------->|---------文件系统---------- -|
| APP1 备份区(新程序)     |  APP2 备份区(上一个版本程序)    |   系统参数区        |---------文件系统----------- |  
|-------------------------- 516 KB (129个物理扇区)    --------------------------------|--剩余为 FatFs 文件系统区---|
*************************************************************************************************************************************************************/
// APP1 BIN 备份地址: 256 KB, sector 0 - sector 63
#define FLASH_APP1_START_SECTOR  ((uint32_t)0L)

// APP2 BIN 备份地址: 256 KB, sector 64 - sector 127
#define FLASH_APP2_START_SECTOR  ((uint32_t)64L)

// 系统环境参数起始扇区地址, sector 128 扇区, 大小 4KB
#define  FLASH_SYS_ENV_START_SECTOR    ((uint32_t)128L) 

// 字体描述信息起始地址
#define  FLASH_FONT_START_SECTOR (FLASH_SYS_ENV_START_SECTOR + 1)  //(FLASH_SYS_ENV_START_SECTOR + 1)

// 文件系统起始地址
#define  FLASH_FATFS_START_SECTOR   (FLASH_FONT_START_SECTOR + 128)

/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x08006FFF is reserved for the IAP code */
#define BOOT_START_ADDR                 ((uint32_t)0x08000000)
#define BOOT_FLASH_SIZE                    ((uint32_t)0x8000)  // boot size: 32 KB
#define BOOT_FLASH_END_ADDR      (BOOT_START_ADDR + BOOT_FLASH_SIZE - 1)  


#define APP_START_ADDR                 ((uint32_t)(BOOT_FLASH_END_ADDR + 1))  
#define APP_FLASH_SIZE                    ((uint32_t)0x40000)   // high desity 256 KB 
#define APP_FLASH_END_ADDR      (BOOT_START_ADDR + APP_FLASH_SIZE - 1)  

#define APP_VECTOR_OFFSET       ((uint32_t)BOOT_FLASH_SIZE)

/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x08002FFF is reserved for the IAP code */
#define APPLICATION_ADDRESS   (uint32_t)APP_START_ADDR 

/* End of the Flash address */
#define USER_FLASH_END_ADDRESS        APP_FLASH_END_ADDR
/* Define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)




#define FREQ_512KHz       (512000L)
#define FREQ_1MHz        (1000000L)
#define FREQ_2MHz        (2000000L)
#define FREQ_8MHz        (8000000L)
#define FREQ_16MHz      (16000000L)
#define FREQ_24MHz      (24000000L)
#define FREQ_48MHz      (48000000L)
#define FREQ_72MHz      (72000000L)


#define CPU_CLOCK       FREQ_72MHz  // MCU 时钟


#define HSIENABLE             0     // HSI 时钟使能, 内部 8 M
#define SYS_USING_BOOTLOADER  1      // 使能 bootloader
#define OS_SYS_TICK_EN                // TIM1 10 ms中断一次 作为 Sys Tick
#define OS_PER_TICK_MS   (10)         // 每个tick的时间: 10ms


#define  SD_DISK_EN            1   // SD 卡存储使能
#define  FLASH_DISK_EN    1   // SPI FLASH DISK 存储使能(1)
#define  ROM_DISK_EN       0  // ROM FLASH DISK 存储使能

#define  MAX_LUN         1    // MAX_LUN + 1 个可移动磁盘 :   SD卡 + SPI FLASH

typedef enum
{
   FLASH_DISK = 0,
   SD_DISK       = 1, 
   ROM_DISK    = 2,
}DISK_ENUM;


#define SD_DISK_NAME          "1:"  //"1:"
#define FLASH_DISK_NAME  "0:"// "0:"



#define  PCB_VERSION       "1.32"


// 中断优先级
#define  SDIO_PRI         0
#define USB_WakeUp_PRI   0

/******************************** 编译开关end *********************************/

/*********************************内存分配begin ******************************/
//用户可根据需要更改


/*********************************内存分配end ******************************/
/***************************** 类型定义begin  ************************************/
typedef enum
{
   INPUT  =  0,
   OUTPUT =  1,
}E_IO_DIR;   // IO 输出或输入方向


typedef enum
{
   SW_OPEN  = 0,
   SW_CLOSE = 1,
}E_SW_STATE;  // IO 开关状态

typedef enum
{
    E_FALSE = 0,
	E_TRUE = !E_FALSE
}E_BOOL;

typedef enum
{
   APP_SUCCESS = 0,
   APP_FAILED = 1,
   APP_NULL = 0xFF,
}E_RESULT;

typedef enum
{
   SYS_SUCCESS = 0,
   SYS_FAILED  = 1,
}SYS_RESULT;




typedef struct struc_rtc_time
{
   uint8_t year;
   uint8_t month;
   uint8_t day;
   uint8_t week;   // 0 - 6
   uint8_t hour;
   uint8_t min;
   uint8_t sec;
   uint8_t century;
}T_RTC_TIME;
/***************************** 类型定义end  **************************************************************/





/********************************* 宏 函数begin **********************************************/
#define NOP()   __NOP()
#define GLOBAL_DISABLE_IRQ()      __disable_irq()  //Global interrupt disable
#define GLOBAL_ENABLE_IRQ()       __enable_irq()   //Global interrupt enable

#define WEAK_ATTR   __attribute__((weak))

#define READ_REG_32_BIT(reg,  b)      ((uint32_t)((reg)&((uint32_t)b)))      //b必须为整数, reg为32 bit 寄存器
#define CLEAR_REG_32_BIT(reg, b)      ((reg)&=(uint32_t)(~((uint32_t)b)))   //b必须为整数, reg为32 bit 寄存器
#define SET_REG_32_BIT(reg,   b)      ((reg)|=(uint32_t)(b))                  //b必须为整数, reg为32 bit 寄存器

#define READ_REG_8_BIT(reg,b)     ((uint8_t)((reg)&((uint8_t)b)))           //b必须为8bit整数, reg为寄存器
#define CLEAR_REG_8_BIT(reg,b)    ((reg)&=(uint8_t)(~((uint8_t)b)))        //b必须为8bit整数, reg为寄存器
#define SET_REG_8_BIT(reg,b)      ((reg)|=(uint8_t)(b))                      //b必须为8bit整数, reg为寄存器

// FLASH 存储对齐后的长度: 按4字节对齐
#define DWORD_ALIGNED_LEN(len)   ( (sizeof(len) + sizeof(int) - 1) &  ~(sizeof(int) - 1) ) 


/********************************* 宏 函数end  **********************************************/
#endif  // __GLOBAL_DEF_H__


