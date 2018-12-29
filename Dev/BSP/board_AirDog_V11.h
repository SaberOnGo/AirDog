

#ifndef __BOARD_AIRDOG_V11_H__
#define  __BOARD_AIRDOG_V11_H__

/**************************************************************************************
主板管脚定义文件
主板硬件版本: AirTerminator V1.203 版本, 2017-12-09
日期: 2017-12-09

**************************************************************************************/

#include "GlobalDef.h"
#include "stm32f10x.h"

// PM2.5 休眠使能控制管脚, 高电平正常工作
// PM25_SET  ->  
#define PM25_SET_Pin              
#define PM25_SET_PORT         
#define PM25_SET_Open()      
#define PM25_SET_Close()      


// BEPP_IO  ->  PB1  , 蜂鸣器管脚定义, IO 输出高电平导通
#define BEEP_IO_Pin                  GPIO_Pin_1
#define BEEP_IO_PORT            GPIOB
#define BEEP_IO_Open()          SET_REG_32_BIT(BEEP_IO_PORT->BSRR, BEEP_IO_Pin)  // 输出高, GPIO_SetBits()
#define BEEP_IO_Close()          SET_REG_32_BIT(BEEP_IO_PORT->BRR,  BEEP_IO_Pin)  // 输出低  GPIO_ResetBits()





// VIN_DETECT -> PC15  USB电源插入读取管脚, 高电平: USB 5V 电源插入; 低电平: USB 电源没有插入
#define VIN_DETECT_Pin                    GPIO_Pin_15
#define VIN_DETECT_PORT              GPIOC
#define VIN_DETECT_PinSource     GPIO_PinSource15
#define VIN_DETECT_PortSource  GPIO_PortSourceGPIOC
#define EXTI_Line_VinDetect            EXTI_Line15
#define EXTI_VinDetect_IRQn         EXTI15_10_IRQn
#define EXTI_VinDetect_IRQHandler        EXTI15_10_IRQHandler

#define VIN_DETECT_Open()   SET_REG_32_BIT(VIN_DETECT_PORT->BSRR, VIN_DETECT_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define VIN_DETECT_Close()   SET_REG_32_BIT(VIN_DETECT_PORT->BRR,  VIN_DETECT_Pin)   // 输出低  GPIOx->BRR = GPIO_Pin;
#define VIN_DETECT_RCC_APBPeriphClockCmdEnable()       SET_REG_32_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOC)   // PCLK2 = HCLK = 48MHz
#define VIN_DETECT_Read()     IO_READ_IN(VIN_DETECT_PORT, VIN_DETECT_Pin)   // GPIO_ReadInputDataBit(LCD_DATA_PORT, GPIO_Pin_4)
#define IsUSBPlugged()                  VIN_DETECT_Read()   // USB 是否已插入: 1: 插入; 0: 没有插入


// CHRG_Indicate ->   充电状态指示管脚, 读该管脚状态, 低电平: 正在充电; 高阻态: 没有在充电 
// CHG_STAT :  充电状态检测 管脚
#define CHG_STAT_Pin             GPIO_Pin_4
#define CHG_STAT_PORT       GPIOC
#define CHG_STAT_Read()    IO_READ_IN(CHG_STAT_PORT, CHG_STAT_Pin)  

#define CHRG_Indicate_Pin           CHG_STAT_Pin
#define CHRG_Indicate_PORT     CHG_STAT_PORT
#define CHRG_Indicate_Read()   CHG_STAT_Read()


// CO2_CAL -> PC14, CO2 气体传感器校正管脚
#define CO2_CAL_Pin             GPIO_Pin_14
#define CO2_CAL_PORT       GPIOC
#define CO2_CAL_Open()    IO_H(CO2_CAL_PORT,   CO2_CAL_Pin)
#define CO2_CAL_Close()    IO_L(CO2_CAL_PORT,   CO2_CAL_Pin)
#define CO2_CAL_H()             (CO2_CAL_PORT->BSRR = CO2_CAL_Pin)
#define CO2_CAL_L()             (CO2_CAL_PORT->BRR     = CO2_CAL_Pin)
#define CO2_CAL_Read()    IO_READ_IN(CO2_CAL_PORT, CO2_CAL_Pin)   // GPIO_ReadInputDataBit(LCD_DATA_PORT, GPIO_Pin_4)




// Sensor_Power_Ctrl -> PA10  IO 输出高电平导通
#define SNS_Power_Ctrl_Pin             GPIO_Pin_10
#define SNS_Power_Ctrl_PORT       GPIOA
#define SNS_Power_Ctrl_Open()    IO_H(SNS_Power_Ctrl_PORT,  SNS_Power_Ctrl_Pin)
#define SNS_Power_Ctrl_Close()    IO_L(SNS_Power_Ctrl_PORT,   SNS_Power_Ctrl_Pin)
#define SNS_Power_Ctrl_H()             IO_H(SNS_Power_Ctrl_PORT,  SNS_Power_Ctrl_Pin)
#define SNS_Power_Ctrl_L()              IO_L(SNS_Power_Ctrl_PORT,   SNS_Power_Ctrl_Pin)



// LCD_Power_Ctrl -> PA9    IO 输出高电平导通
#define LCD_Power_Ctrl_Pin             GPIO_Pin_9
#define LCD_Power_Ctrl_PORT       GPIOA
#define LCD_Power_Ctrl_Open()   IO_H(LCD_Power_Ctrl_PORT,  LCD_Power_Ctrl_Pin)
#define LCD_Power_Ctrl_Close()   IO_L(LCD_Power_Ctrl_PORT,  LCD_Power_Ctrl_Pin)
#define LCD_Power_Ctrl_H()           IO_H(LCD_Power_Ctrl_PORT,  LCD_Power_Ctrl_Pin)
#define LCD_Power_Ctrl_L()            IO_L(LCD_Power_Ctrl_PORT,  LCD_Power_Ctrl_Pin)

#define LCD_BackLight_Ctrl_Pin            GPIO_Pin_0
#define LCD_BackLight_Ctrl_PORT      GPIOA
#define LCD_BackLight_Ctrl_Open()   IO_H(LCD_BackLight_Ctrl_PORT,  LCD_BackLight_Ctrl_Pin)
#define LCD_BackLight_Ctrl_Close()   IO_L(LCD_BackLight_Ctrl_PORT,  LCD_BackLight_Ctrl_Pin)
#define LCD_BackLight_Ctrl_H()           IO_H(LCD_BackLight_Ctrl_PORT,  LCD_BackLight_Ctrl_Pin)
#define LCD_BackLight_Ctrl_L()            IO_L(LCD_BackLight_Ctrl_PORT,  LCD_BackLight_Ctrl_Pin)


// WIFI Power Ctrl -> PC3
#define WIFI_Power_Ctrl_Pin              GPIO_Pin_3       
#define WIFI_Power_Ctrl_PORT         GPIOC
#define WIFI_Power_Ctrl_Open()      IO_H(WIFI_Power_Ctrl_PORT,  WIFI_Power_Ctrl_Pin)  
#define WIFI_Power_Ctrl_Close()      IO_L(WIFI_Power_Ctrl_PORT,  WIFI_Power_Ctrl_Pin)  

// 按键管脚定义 -------------------------------------begin------------------------------*/
#define KEY0_PORT              GPIOC
#define KEY0_GPIO_Pin     GPIO_Pin_6

#define KEY1_PORT             GPIOC
#define KEY1_GPIO_Pin    GPIO_Pin_7

//#define KEY2_PORT      GPIOC
//#define KEY2_GPIO_Pin  GPIO_Pin_4


#define KEY0_INPUT        ((IO_READ_IN(KEY0_PORT, KEY0_GPIO_Pin)) << 0)
#define KEY1_INPUT        ((IO_READ_IN(KEY1_PORT, KEY1_GPIO_Pin)) << 1)
//#define KEY2_INPUT   ((STM32_GPIO_ReadInputDataBit(KEY2_PORT, KEY2_GPIO_Pin)) << 2)

//#define KEY_INPUT         ((uint8_t)KEY0_INPUT)   //((uint8_t)((KEY0_INPUT) |(KEY1_INPUT) |(KEY2_INPUT) ))
#define KEY_INPUT         ((uint8_t)( KEY0_INPUT | KEY1_INPUT ) )

// 按键管脚定义 -------------------------------------end------------------------------*/



/*--------------------------ADC 管脚定义 begin ---------------------*/



// 0: BAT_ADC -> PC0  ->  ADC1 Channel 10
#define BAT_ADC_Pin                  GPIO_Pin_0
#define BAT_ADC_PORT            GPIOC
#define BAT_ADC_x                      ADC1
#define BAT_ADC_Channel       ADC_Channel_10


// 1: Light_1 -> PB0 ->  ADC1 Channel 8
#define LIGHT_1_Pin                   GPIO_Pin_0
#define LIGHT_1_PORT             GPIOB
#define LIGHT_1_ADC_x           ADC1
#define LIGHT_1_Channel        ADC_Channel_8




// 外部中断硬件IO 定义
#define CAPKEY_Pin                          GPIO_Pin_5
#define CAPKEY_PORT                    GPIOC
#define CAPKEY_PinSource           GPIO_PinSource5
#define CAPKEY_PortSource        GPIO_PortSourceGPIOC
#define EXTI_Line_CAPKEY           EXTI_Line5
#define EXTI_CAPKEY_IRQn        EXTI9_5_IRQn
#define CAPKEY_Read()                 IO_READ_IN(CAPKEY_PORT, CAPKEY_Pin)   
#define EXTI_CAPKEY_IRQHandler        EXTI9_5_IRQHandler 




// 2: 内部温度传感器

// 3: 内部ADC 参考电压源


/*--------------------------ADC 管脚定义 end ---------------------*/



/* ----------------------TFT 显示屏 管脚定义 begin  -----------------*/
#if 1
#define LCD_WR_PIN        GPIO_Pin_8
#define LCD_WR_PORT   GPIOB
#define LCD_WR_H()       (LCD_WR_PORT->BSRR = LCD_WR_PIN)
#define LCD_WR_L()       (LCD_WR_PORT->BRR     = LCD_WR_PIN)

#define LCD_CS_PIN         GPIO_Pin_9   
#define LCD_CS_PORT    GPIOB
#define LCD_CS_H()         (LCD_CS_PORT->BSRR = LCD_CS_PIN)
#define LCD_CS_L()          (LCD_CS_PORT->BRR    = LCD_CS_PIN)


#define LCD_SCLK_PIN         GPIO_Pin_3 
#define LCD_SCLK_PORT    GPIOB
#define LCD_SCLK_H()         (LCD_SCLK_PORT->BSRR = LCD_SCLK_PIN)
#define LCD_SCLK_L()          (LCD_SCLK_PORT->BRR    = LCD_SCLK_PIN)

#define LCD_SDO_PIN           GPIO_Pin_4   
#define LCD_SDO_PORT      GPIOB
#define LCD_SDO_H()           (LCD_SDO_PORT->BSRR = LCD_SDO_PIN)
#define LCD_SDO_L()            (LCD_SDO_PORT->BRR     = LCD_SDO_PIN)
#define LCD_SDO_Read()    IO_READ_IN(LCD_SDO_PORT, LCD_SDO_PIN)

#define LCD_SDA_PIN           GPIO_Pin_5
#define LCD_SDA_PORT       GPIOB
#define LCD_SDA_H()           (LCD_SDA_PORT->BSRR = LCD_SDA_PIN)
#define LCD_SDA_L()            (LCD_SDA_PORT->BRR     = LCD_SDA_PIN)
#define LCD_SDA_Read()   IO_READ_IN(LCD_SDA_PORT, LCD_SDA_PIN)
#endif
/* ----------------------TFT 显示屏 管脚定义 end  -----------------*/





/*----------------------I2C 硬件管脚定义 begin ----------------*/

/*----------------------I2C 硬件管脚定义 end ----------------*/


/****************************硬件定义 end ****************************************/
void SNS_Ctrl_Set(E_SW_STATE sta);
void LCD_Ctrl_Set(E_SW_STATE sta);
void LCD_BackLight_Ctrl_Set(E_SW_STATE sta);
void Board_GpioInit(void);
void BEEP(uint16_t freq);
void TFT_Ctrl(E_SW_STATE sta);


#endif


