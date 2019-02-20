
#include "board_version.h"
#include "os_timer.h"



static void PowerIO_Ctrl(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pinx, E_SW_STATE sta)
{
       GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pinx;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	if(SW_OPEN == sta)
	{
	       GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		STM32_GPIO_Init(GPIOx, &GPIO_InitStructure);
		GPIOx->BRR = GPIO_Pinx;  // 低电平导通
	}
	else // 关闭: 输出高电平
	{
	      GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	      STM32_GPIO_Init(GPIOx, &GPIO_InitStructure);
	}
}

static E_SW_STATE sns_sta = SW_CLOSE;
void SNS_Ctrl_Set(E_SW_STATE sta)
{
        sns_sta = sta;
        PowerIO_Ctrl(SNS_Power_Ctrl_PORT, SNS_Power_Ctrl_Pin, sta);
}

E_SW_STATE SNS_Ctrl_Get(void)
{
      return sns_sta;
}

void LCD_Ctrl_Set(E_SW_STATE sta)
{
	PowerIO_Ctrl(LCD_Power_Ctrl_PORT, LCD_Power_Ctrl_Pin, sta);
}
void LCD_BackLight_Ctrl_Set(E_SW_STATE sta)
{
	PowerIO_Ctrl(LCD_BackLight_Ctrl_PORT, LCD_BackLight_Ctrl_Pin , sta);
}

void TFT_Ctrl(E_SW_STATE sta)
{
       //LCD_Ctrl_Set(sta);
       LCD_BackLight_Ctrl_Set(sta);
}

#include "delay.h"
// 设置数据口为输入或输出
// 板级管脚初始化
void Board_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 时钟使能
    STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	


    #if 1
	// PWR_SW, 3.3V稳压器开关
	//GPIO_InitStructure.GPIO_Pin   = PWR_SW_Pin;
	//STM32_GPIO_Init(PWR_SW_PORT, &GPIO_InitStructure);
	//PWR_SW_Open();  // 打开电源
	

	
	// SENSOR_Power Ctrl
	SNS_Ctrl_Set(SW_OPEN);  


	
	
	
       // WIFI Power Ctrl
	GPIO_InitStructure.GPIO_Pin   = WIFI_Power_Ctrl_Pin;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	STM32_GPIO_Init(WIFI_Power_Ctrl_PORT, &GPIO_InitStructure);

	WIFI_Power_Ctrl_Open();


      //SNS_Ctrl_Set(SW_CLOSE);  
    //  LCD_BackLight_Ctrl_Set(SW_CLOSE);
    //  while(1);
       // RT_EN 
	GPIO_InitStructure.GPIO_Pin         = RT_EN_Pin;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	STM32_GPIO_Init(RT_EN_PORT, &GPIO_InitStructure);
       RT_EN_Open();

       
       
       // LCD_Power, 屏幕电源开关
       LCD_Ctrl_Set(SW_OPEN);
       TFT_Ctrl(SW_OPEN);
       
	// BEEP IO 管脚初始化
       GPIO_InitStructure.GPIO_Pin   = BEEP_IO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	STM32_GPIO_Init(BEEP_IO_PORT, &GPIO_InitStructure);
	BEEP_IO_Close();

	// 按键管脚初始化
	//GPIO_InitStructure.GPIO_Pin       = KEY0_GPIO_Pin;
       //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;  // 上拉输入
	//STM32_GPIO_Init(KEY0_PORT, &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Pin       = KEY1_GPIO_Pin;
	//STM32_GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
	#endif


	 GPIO_InitStructure.GPIO_Pin      = VIN_DETECT_Pin;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	 STM32_GPIO_Init(VIN_DETECT_PORT, &GPIO_InitStructure);
	 

	 GPIO_InitStructure.GPIO_Pin      =  CHG_STAT_Pin;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	 STM32_GPIO_Init(CHG_STAT_PORT, &GPIO_InitStructure);
}


void BEEP(uint16_t freq)
{
        u32 i;
        for(i = 0; i < freq; i++)
        {
                 BEEP_IO_Close();
                 delay_ms(100);
                 BEEP_IO_Open();
                 delay_ms(100);
        }
        
}



