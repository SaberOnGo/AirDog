
#ifndef __BOARD_AT_V1203_H__
#define  __BOARD_AT_V1203_H__

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
#define PM25_SET_RCC_APBPeriphClockCmdEnable()      

// BEPP_IO  ->  PA1  , 蜂鸣器管脚定义, IO 输出高电平导通
#define BEEP_IO_Pin             GPIO_Pin_1
#define BEEP_IO_PORT            GPIOA
#define BEEP_IO_Open()          SET_REG_32_BIT(BEEP_IO_PORT->BSRR, BEEP_IO_Pin)  // 输出高, GPIO_SetBits()
#define BEEP_IO_Close()         SET_REG_32_BIT(BEEP_IO_PORT->BRR,  BEEP_IO_Pin)  // 输出低  GPIO_ResetBits()
#define BEEP_IO_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)   // PCLK2 = HCLK = 48MHz


// // LED_EN  屏幕背光控制使能管脚, 输出高电平导通
// LED_EN  ->  PA8
#define LED_EN_Ctrl_Pin       GPIO_Pin_8
#define LED_EN_Ctrl_PORT      GPIOA
#define LED_EN_Ctrl_Open()    SET_REG_32_BIT(LED_EN_Ctrl_PORT->BSRR, LED_EN_Ctrl_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define LED_EN_Ctrl_Close()   SET_REG_32_BIT(LED_EN_Ctrl_PORT->BRR,  LED_EN_Ctrl_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define LED_EN_Ctrl_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)   // PCLK2 = HCLK = 48MHz


// TP4056 电池充电管脚使能控制管脚, 高电平: 正常工作; 低电平: 禁止充电
// BAT_CE  ->  PA9
#define BAT_CE_Pin          GPIO_Pin_9
#define BAT_CE_PORT         GPIOA
#define BAT_CE_Open()       SET_REG_32_BIT(BAT_CE_PORT->BSRR, BAT_CE_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define BAT_CE_Close()      SET_REG_32_BIT(BAT_CE_PORT->BRR,  BAT_CE_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define BAT_CE_H()          SET_REG_32_BIT(BAT_CE_PORT->BSRR, BAT_CE_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define BAT_CE_L()          SET_REG_32_BIT(BAT_CE_PORT->BRR,  BAT_CE_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define BAT_CE_RCC_APBPeriphClockCmdEnable()       SET_REG_32_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOA)    // PCLK2 = HCLK = 48MHz
#define BAT_CE_Read()       STM32_GPIO_ReadInputDataBit(BAT_CE_PORT, BAT_CE_Pin) 

// RT9193 电源使能控制管脚, 上升沿使能
// PWR_SW  ->  PA15
#define PWR_SW_Pin          GPIO_Pin_15
#define PWR_SW_PORT         GPIOA
#define PWR_SW_Open()       SET_REG_32_BIT(PWR_SW_PORT->BSRR, PWR_SW_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define PWR_SW_Close()      SET_REG_32_BIT(PWR_SW_PORT->BRR,  PWR_SW_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define PWR_SW_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)   // PCLK2 = HCLK = 48MHz

// VIN_DETECT -> PB0  USB电源插入读取管脚, 高电平: USB 5V 电源插入; 低电平: USB 电源没有插入
#define VIN_DETECT_Pin         GPIO_Pin_0
#define VIN_DETECT_PORT        GPIOB
#define VIN_DETECT_PinSource   GPIO_PinSource0
#define VIN_DETECT_PortSource  GPIO_PortSourceGPIOB
#define EXTI_Line_VinDetect    EXTI_Line0
#define EXTI_VinDetect_IRQn    EXTI0_IRQn
#define EXTI_VinDetect_IRQHandler        EXTI0_IRQHandler 

#define VIN_DETECT_Open()   SET_REG_32_BIT(VIN_DETECT_PORT->BSRR, VIN_DETECT_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define VIN_DETECT_Close()  SET_REG_32_BIT(VIN_DETECT_PORT->BRR,  VIN_DETECT_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define VIN_DETECT_RCC_APBPeriphClockCmdEnable()       SET_REG_32_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOB)   // PCLK2 = HCLK = 48MHz
#define VIN_DETECT_Read()    STM32_GPIO_ReadInputDataBit(VIN_DETECT_PORT, VIN_DETECT_Pin)   // GPIO_ReadInputDataBit(LCD_DATA_PORT, GPIO_Pin_4)
#define IsUSBPlugged()        VIN_DETECT_Read()   // USB 是否已插入: 1: 插入; 0: 没有插入

// XR1151 EN ->  PB1, 高电平使能
#define XR1151_EN_Pin             GPIO_Pin_1
#define XR1151_EN_PORT            GPIOB
#define XR1151_EN_Open()          SET_REG_32_BIT(XR1151_EN_PORT->BSRR, XR1151_EN_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define XR1151_EN_Close()         SET_REG_32_BIT(XR1151_EN_PORT->BRR,  XR1151_EN_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define XR1151_EN_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)   // PCLK2 = HCLK = 48MHz

// CHRG_Indicate -> PB4  充电状态指示管脚, 读该管脚状态, 低电平: 正在充电; 高阻态: 没有在充电 
#define CHRG_Indicate_Pin      GPIO_Pin_4
#define CHRG_Indicate_PORT     GPIOB
#define CHRG_Indicate_Open()   SET_REG_32_BIT(CHRG_Indicate_PORT->BSRR, CHRG_Indicate_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define CHRG_Indicate_Close()  SET_REG_32_BIT(CHRG_Indicate_PORT->BRR,  CHRG_Indicate_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define CHRG_Indicate_RCC_APBPeriphClockCmdEnable()       SET_REG_32_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOB)  // PCLK2 = HCLK = 48MHz
#define CHRG_Indicate_Read()   STM32_GPIO_ReadInputDataBit(CHRG_Indicate_PORT, CHRG_Indicate_Pin)   // GPIO_ReadInputDataBit(LCD_DATA_PORT, GPIO_Pin_4)


// CO2_CAL -> PB5, CO2 气体传感器校正管脚
#define CO2_CAL_Pin      //GPIO_Pin_5
#define CO2_CAL_PORT    // GPIOB
#define CO2_CAL_Open()   SET_REG_32_BIT(CO2_CAL_PORT->BSRR, CO2_CAL_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define CO2_CAL_Close()  SET_REG_32_BIT(CO2_CAL_PORT->BRR,  CO2_CAL_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define CO2_CAL_H()      (CO2_CAL_PORT->BSRR = CO2_CAL_Pin)
#define CO2_CAL_L()      (CO2_CAL_PORT->BRR  = CO2_CAL_Pin)
#define CO2_CAL_RCC_APBPeriphClockCmdEnable()       SET_REG_32_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOB)  // PCLK2 = HCLK = 48MHz
#define CO2_CAL_Read()   STM32_GPIO_ReadInputDataBit(CO2_CAL_PORT, CO2_CAL_Pin)   // GPIO_ReadInputDataBit(LCD_DATA_PORT, GPIO_Pin_4)






// TVOC_Power_Ctrl -> PC6  IO 输出高电平导通
#define TVOC_Power_Ctrl_Pin      GPIO_Pin_6
#define TVOC_Power_Ctrl_PORT     GPIOC
#define TVOC_Power_Ctrl_Open()   SET_REG_32_BIT(TVOC_Power_Ctrl_PORT->BSRR, TVOC_Power_Ctrl_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define TVOC_Power_Ctrl_Close()  SET_REG_32_BIT(TVOC_Power_Ctrl_PORT->BRR,  TVOC_Power_Ctrl_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define TVOC_Power_Ctrl_H()      SET_REG_32_BIT(TVOC_Power_Ctrl_PORT->BSRR, TVOC_Power_Ctrl_Pin)
#define TVOC_Power_Ctrl_L()      SET_REG_32_BIT(TVOC_Power_Ctrl_PORT->BRR,  TVOC_Power_Ctrl_Pin)
#define TVOC_Power_Ctrl_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)   // PCLK2 = HCLK = 48MHz

// GSM_Power_Ctrl -> PC7  IO 输出高电平导通
#define GSM_Power_Ctrl_Pin      GPIO_Pin_7
#define GSM_Power_Ctrl_PORT     GPIOC
#define GSM_Power_Ctrl_Open()   SET_REG_32_BIT(GSM_Power_Ctrl_PORT->BSRR, GSM_Power_Ctrl_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define GSM_Power_Ctrl_Close()  SET_REG_32_BIT(GSM_Power_Ctrl_PORT->BRR,  GSM_Power_Ctrl_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define GSM_Power_Ctrl_H()      SET_REG_32_BIT(GSM_Power_Ctrl_PORT->BSRR, GSM_Power_Ctrl_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define GSM_Power_Ctrl_L()      SET_REG_32_BIT(GSM_Power_Ctrl_PORT->BRR,  GSM_Power_Ctrl_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define GSM_Power_Ctrl_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)   // PCLK2 = HCLK = 48MHz

// Sensor_Power_Ctrl -> PC8  IO 输出高电平导通
#define Sensor_Power_Ctrl_Pin       GPIO_Pin_8
#define Sensor_Power_Ctrl_PORT      GPIOC
#define Sensor_Power_Ctrl_Open()    SET_REG_32_BIT(HCHO_Power_Ctrl_PORT->BSRR, HCHO_Power_Ctrl_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define Sensor_Power_Ctrl_Close()   SET_REG_32_BIT(HCHO_Power_Ctrl_PORT->BRR,  HCHO_Power_Ctrl_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define Sensor_Power_Ctrl_H()       SET_REG_32_BIT(HCHO_Power_Ctrl_PORT->BSRR, HCHO_Power_Ctrl_Pin)
#define Sensor_Power_Ctrl_L()       SET_REG_32_BIT(HCHO_Power_Ctrl_PORT->BRR,  HCHO_Power_Ctrl_Pin) 
#define Sensor_Power_Ctrl_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)   // PCLK2 = HCLK = 48MHz


// LCD_Power_Ctrl -> PC9  IO 输出高电平导通
#define LCD_Power_Ctrl_Pin      GPIO_Pin_9
#define LCD_Power_Ctrl_PORT     GPIOC
#define LCD_Power_Ctrl_Open()   SET_REG_32_BIT(LCD_Power_Ctrl_PORT->BSRR, LCD_Power_Ctrl_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define LCD_Power_Ctrl_Close()  SET_REG_32_BIT(LCD_Power_Ctrl_PORT->BRR,  LCD_Power_Ctrl_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define LCD_Power_Ctrl_H()   SET_REG_32_BIT(LCD_Power_Ctrl_PORT->BSRR, LCD_Power_Ctrl_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define LCD_Power_Ctrl_L()  SET_REG_32_BIT(LCD_Power_Ctrl_PORT->BRR,  LCD_Power_Ctrl_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define LCD_Power_Ctrl_RCC_APBPeriphClockCmdEnable()       STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)   // PCLK2 = HCLK = 48MHz





// 按键管脚定义 -------------------------------------begin------------------------------*/
#define KEY0_PORT         GPIOB
#define KEY0_GPIO_Pin     GPIO_Pin_9
#define KEY0_Read()       STM32_GPIO_ReadInputDataBit(KEY0_PORT, KEY0_GPIO_Pin)

#define KEY1_PORT      GPIOB
#define KEY1_GPIO_Pin  GPIO_Pin_8

//#define KEY2_PORT      GPIOC
//#define KEY2_GPIO_Pin  GPIO_Pin_4


#define KEY0_INPUT        ((STM32_GPIO_ReadInputDataBit(KEY0_PORT, KEY0_GPIO_Pin)) << 0)
#define KEY1_INPUT        ((STM32_GPIO_ReadInputDataBit(KEY1_PORT, KEY1_GPIO_Pin)) << 1)
//#define KEY2_INPUT   ((STM32_GPIO_ReadInputDataBit(KEY2_PORT, KEY2_GPIO_Pin)) << 2)

//#define KEY_INPUT         ((uint8_t)KEY0_INPUT)   //((uint8_t)((KEY0_INPUT) |(KEY1_INPUT) |(KEY2_INPUT) ))
#define KEY_INPUT         ((uint8_t)( KEY0_INPUT | KEY1_INPUT ) )

// 按键管脚定义 -------------------------------------end------------------------------*/



/*--------------------------ADC 管脚定义 begin ---------------------*/

// 0: 2V_REF_IN  -> PA0
#define HCHO_2V_Ref_IN_Pin      GPIO_Pin_0
#define HCHO_2V_Ref_IN_PORT     GPIOA
#define HCHO_2V_Ref_IN_ADC_x    ADC1
#define HCHO_2V_Ref_IN_Channel  ADC_Channel_0


// 1: BAT_ADC -> PC0  ->  ADC1 Channel 10
#define BAT_ADC_Pin             GPIO_Pin_0
#define BAT_ADC_PORT            GPIOC
#define BAT_ADC_x               ADC1
#define BAT_ADC_Channel         ADC_Channel_10


// 2: USB_ADC -> PC1  ->  ADC1 Channel 11
#define USB_ADC_Pin             GPIO_Pin_1
#define USB_ADC_PORT            GPIOC
#define USB_ADC_x               ADC1
#define USB_ADC_Channel         ADC_Channel_11


// 3: Light_1 -> PC2 ->  ADC1 Channel 12
#define LIGHT_1_Pin             GPIO_Pin_2
#define LIGHT_1_PORT            GPIOC
#define LIGHT_1_ADC_x           ADC1
#define LIGHT_1_Channel         ADC_Channel_12

// 4: Light2 ->  PC3
#define LIGHT_2_Pin             GPIO_Pin_3
#define LIGHT_2_PORT            GPIOC
#define LIGHT_2_ADC_x           ADC1
#define LIGHT_2_Channel         ADC_Channel_13

// 5: 振动传感器 CAPKEY -> PC4
#define CAPKEY_Pin              GPIO_Pin_4
#define CAPKEY_PORT             GPIOC
#define CAPKEY_ADC_x            ADC1
#define CAPKEY_Channel          ADC_Channel_14

// 外部中断硬件IO 定义
#define CAPKEY_PinSource       GPIO_PinSource4
#define CAPKEY_PortSource      GPIO_PortSourceGPIOC
#define EXTI_Line_CAPKEY       EXTI_Line4
#define EXTI_CAPKEY_IRQn       EXTI4_IRQn
#define CAPKEY_Read()          STM32_GPIO_ReadInputDataBit(CAPKEY_PORT, CAPKEY_Pin)   
#define EXTI_CAPKEY_IRQHandler        EXTI4_IRQHandler 

// 6: 内部温度传感器

// 7: 内部ADC 参考电压源


/*--------------------------ADC 管脚定义 end ---------------------*/


/* ----------------------LCD 1602 管脚定义 begin  -----------------*/
#define LCD_RS_PIN   GPIO_Pin_13   // PC13
#define LCD_RS_PORT  GPIOC
#define LCD_RS_H()   (LCD_RS_PORT->BSRR =  LCD_RS_PIN)   // GPIO_SetBits(LCD_RS_PORT, LCD_RS_PIN);
#define LCD_RS_L()   (LCD_RS_PORT->BRR  =  LCD_RS_PIN)



#define LCD_RW_PIN   GPIO_Pin_9   // PB9
#define LCD_RW_PORT  GPIOB
#define LCD_RW_H()   (LCD_RW_PORT->BSRR = LCD_RW_PIN)
#define LCD_RW_L()   (LCD_RW_PORT->BRR  = LCD_RW_PIN)


#define LCD_EN_PIN   GPIO_Pin_8   // PB8
#define LCD_EN_PORT  GPIOB
#define LCD_EN_H()   (LCD_EN_PORT->BSRR = LCD_EN_PIN)
#define LCD_EN_L()   (LCD_EN_PORT->BRR  = LCD_EN_PIN)

// LCD_D4  -> PB5
#define  LCD_D4_Pin    GPIO_Pin_5
#define  LCD_D4_PORT   GPIOB

// LCD_D5  -> PB4
#define  LCD_D5_Pin    GPIO_Pin_4
#define  LCD_D5_PORT   GPIOB

// LCD_D6  -> PB3
#define  LCD_D6_Pin    GPIO_Pin_3
#define  LCD_D6_PORT   GPIOB

// LCD_D7  -> PA15
#define  LCD_D7_Pin    GPIO_Pin_15
#define  LCD_D7_PORT   GPIOA


#define GPIO_WRITE_BIT(GPIOx, GPIO_Pin, bitVal) \
	((bitVal) ? (GPIOx->BSRR = GPIO_Pin) : (GPIOx->BRR = GPIO_Pin))


#define LCD_D7_WRITE(bitVal)  GPIO_WRITE_BIT(LCD_D7_PORT, LCD_D7_Pin, bitVal)  // GPIO_WriteBit(LCD_D7_PORT, LCD_D7_Pin, ((bitVal) ? Bit_SET : Bit_RESET) )
#define LCD_D6_WRITE(bitVal)  GPIO_WRITE_BIT(LCD_D6_PORT, LCD_D6_Pin, bitVal)  // GPIO_WriteBit(LCD_D6_PORT, LCD_D6_Pin, ((bitVal) ? Bit_SET : Bit_RESET) )
#define LCD_D5_WRITE(bitVal)  GPIO_WRITE_BIT(LCD_D5_PORT, LCD_D5_Pin, bitVal)  // GPIO_WriteBit(LCD_D5_PORT, LCD_D5_Pin, ((bitVal) ? Bit_SET : Bit_RESET) )
#define LCD_D4_WRITE(bitVal)  GPIO_WRITE_BIT(LCD_D4_PORT, LCD_D4_Pin, bitVal)  // GPIO_WriteBit(LCD_D4_PORT, LCD_D4_Pin, ((bitVal) ? Bit_SET : Bit_RESET) )

#define LCD_D7_READ()  READ_REG_32_BIT(LCD_D7_PORT, LCD_D7_Pin)
#define LCD_D6_READ()  READ_REG_32_BIT(LCD_D6_PORT, LCD_D6_Pin)
#define LCD_D5_READ()  READ_REG_32_BIT(LCD_D5_PORT, LCD_D5_Pin)
#define LCD_D4_READ()  READ_REG_32_BIT(LCD_D4_PORT, LCD_D4_Pin)



#define LCD_DATA_4BIT_H()   {\
	LCD_D7_WRITE(Bit_SET);\
	LCD_D6_WRITE(Bit_SET);\
	LCD_D5_WRITE(Bit_SET);\
	LCD_D4_WRITE(Bit_SET);\
	}  // 数据位 4bit 都置为 1
	
#define LCD_DATA_4BIT_L()   {\
	LCD_D7_WRITE(Bit_RESET);\
	LCD_D6_WRITE(Bit_RESET);\
	LCD_D5_WRITE(Bit_RESET);\
	LCD_D4_WRITE(Bit_RESET);\
	}

// 发送val的高4位值
#define LCD_DATA_4BIT_OUT(val)   {\
	LCD_D7_WRITE(val & 0x80);\
	LCD_D6_WRITE(val & 0x40);\
	LCD_D5_WRITE(val & 0x20);\
	LCD_D4_WRITE(val & 0x10);\
	}

#define RCC_APB2Periph_LCD_GPIO  ( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA)
/* ----------------------LCD 1602 管脚定义 end  -----------------*/



/* ----------------------TFT 显示屏 管脚定义 begin  -----------------*/
#if 0
#define LCD_WR_PIN   GPIO_Pin_13
#define LCD_WR_PORT  GPIOC
#define LCD_WR_H()   (LCD_WR_PORT->BSRR = LCD_WR_PIN)
#define LCD_WR_L()   (LCD_WR_PORT->BRR  = LCD_WR_PIN)

#define LCD_CS_PIN    GPIO_Pin_4   // PA4
#define LCD_CS_PORT   GPIOA
#define LCD_CS_H()    (LCD_CS_PORT->BSRR = LCD_CS_PIN)
#define LCD_CS_L()    (LCD_CS_PORT->BRR  = LCD_CS_PIN)


#define LCD_SCLK_PIN     GPIO_Pin_5   // PA5
#define LCD_SCLK_PORT    GPIOA
#define LCD_SCLK_H()     (LCD_SCLK_PORT->BSRR = LCD_SCLK_PIN)
#define LCD_SCLK_L()     (LCD_SCLK_PORT->BRR  = LCD_SCLK_PIN)

#define LCD_SDO_PIN     GPIO_Pin_6   // PA6
#define LCD_SDO_PORT    GPIOA
#define LCD_SDO_H()     (LCD_SDO_PORT->BSRR = LCD_SDO_PIN)
#define LCD_SDO_L()     (LCD_SDO_PORT->BRR  = LCD_SDO_PIN)
#define LCD_SDO_Read()  STM32_GPIO_ReadInputDataBit(LCD_SDO_PORT, LCD_SDO_PIN)

#define LCD_SDA_PIN     GPIO_Pin_7   // PA7
#define LCD_SDA_PORT    GPIOA
#define LCD_SDA_H()     (LCD_SDA_PORT->BSRR = LCD_SDA_PIN)
#define LCD_SDA_L()     (LCD_SDA_PORT->BRR  = LCD_SDA_PIN)
#define LCD_SDA_Read()  STM32_GPIO_ReadInputDataBit(LCD_SDA_PORT, LCD_SDA_PIN)
#endif
/* ----------------------TFT 显示屏 管脚定义 end  -----------------*/


/* ----------------------段式显示屏 管脚定义 begin  -----------------*/
#define LCD_CS_PIN    GPIO_Pin_4   // PA4
#define LCD_CS_PORT   GPIOA
#define LCD_CS_H()    (LCD_CS_PORT->BSRR = LCD_CS_PIN)
#define LCD_CS_L()    (LCD_CS_PORT->BRR  = LCD_CS_PIN)

#define LCD_WR_PIN   GPIO_Pin_6
#define LCD_WR_PORT  GPIOA
#define LCD_WR_H()   (LCD_WR_PORT->BSRR = LCD_WR_PIN)
#define LCD_WR_L()   (LCD_WR_PORT->BRR  = LCD_WR_PIN)

#define LCD_SDA_PIN     GPIO_Pin_7   // PA7
#define LCD_SDA_PORT    GPIOA
#define LCD_SDA_H()     (LCD_SDA_PORT->BSRR = LCD_SDA_PIN)
#define LCD_SDA_L()     (LCD_SDA_PORT->BRR  = LCD_SDA_PIN)
#define LCD_SDA_Read()  STM32_GPIO_ReadInputDataBit(LCD_SDA_PORT, LCD_SDA_PIN)

#define RCC_APB2Periph_GPIO_LCD   RCC_APB2Periph_GPIOA
/* ----------------------段式液晶显示屏 管脚定义 end  -----------------*/


/*----------------------I2C 硬件管脚定义 begin ----------------*/
// IIC_SDA -> PB7  
#define IIC_SDA_Pin       GPIO_Pin_7
#define IIC_SDA_PORT      GPIOB
#define IIC_SDA_H()       SET_REG_32_BIT(IIC_SDA_PORT->BSRR, IIC_SDA_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define IIC_SDA_L()       SET_REG_32_BIT(IIC_SDA_PORT->BRR,  IIC_SDA_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define IIC_SDA_READ()    READ_REG_32_BIT(IIC_SDA_PORT->IDR, IIC_SDA_Pin)  // STM32_GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_Pin)

// IIC_SCL -> PB6
#define IIC_SCL_Pin       GPIO_Pin_6
#define IIC_SCL_PORT      GPIOB
#define IIC_SCL_H()       SET_REG_32_BIT(IIC_SCL_PORT->BSRR, IIC_SCL_Pin)  // 输出高, GPIOx->BSRR = GPIO_Pin;
#define IIC_SCL_L()       SET_REG_32_BIT(IIC_SCL_PORT->BRR,  IIC_SCL_Pin)  // 输出低  GPIOx->BRR = GPIO_Pin;
#define IIC_APBPeriphClockCmdEnable()       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)   // PCLK2 = HCLK = 48MHz

#define IIC_BUS_PORT      GPIOB
/*----------------------I2C 硬件管脚定义 end ----------------*/


/****************************硬件定义 end ****************************************/
void SENSOR_Ctrl_Set(E_SW_STATE sta);
void TVOC_Ctrl_Set(E_SW_STATE sta);
void LCD_Ctrl_Set(E_SW_STATE sta);
void GSM_Ctrl_Set(E_SW_STATE sta);
void BAT_CE_Set(E_SW_STATE sta);

void Board_GpioInit(void);

#endif


