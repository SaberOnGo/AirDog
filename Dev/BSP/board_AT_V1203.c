
#include "board_version.h"
#include "os_timer.h"

static os_timer_t tPowerOnLedTimer;  // 电源指示 LED 闪烁定时器
static uint16_t flashTimes = 0;    
static uint8_t  flashFlag = 0;
static void PowerOnLedFlashTimer_CallBack(void * arg)
{
   if(! flashFlag)
   {
	  //PowerOn_Ctrl_Open();  // 指示灯打开
	  //LCD_Power_Ctrl_Open();
	  
	  if(flashTimes)
	  {
	  	 //flashTimes--;
	  }
   }
   else
   {
      //PowerOn_Ctrl_Close();  // 指示灯关闭
      //LCD_Power_Ctrl_Close();
	 
   }
   if(flashTimes)os_timer_arm(&tPowerOnLedTimer, 3000, 0);
   else {

   //	PowerOn_Ctrl_Close(); 
	}
   flashFlag ^= 1;
}

// LED 闪烁
// 参数: uint16_t times: 闪烁次数
//static 
void PowerOnLed_Flash(uint16_t times)
{
   if(times)
   {
      flashTimes = times;
      os_timer_setfn(&tPowerOnLedTimer, PowerOnLedFlashTimer_CallBack, NULL);
	  os_timer_arm(&tPowerOnLedTimer, 3000, 0);
   }  
}

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

void SENSOR_Ctrl_Set(E_SW_STATE sta)
{
    PowerIO_Ctrl(Sensor_Power_Ctrl_PORT, Sensor_Power_Ctrl_Pin, sta);
}

void TVOC_Ctrl_Set(E_SW_STATE sta)
{
	PowerIO_Ctrl(TVOC_Power_Ctrl_PORT, TVOC_Power_Ctrl_Pin, sta);
}

void LCD_Ctrl_Set(E_SW_STATE sta)
{
	PowerIO_Ctrl(LCD_Power_Ctrl_PORT, LCD_Power_Ctrl_Pin, sta);
}

void GSM_Ctrl_Set(E_SW_STATE sta)
{
    PowerIO_Ctrl(GSM_Power_Ctrl_PORT, GSM_Power_Ctrl_Pin, sta);
}

void BAT_CE_Set(E_SW_STATE sta)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin   = BAT_CE_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	if(SW_OPEN == sta)
	{
	   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
       STM32_GPIO_Init(BAT_CE_PORT, &GPIO_InitStructure);
	}
	else 
	{
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        STM32_GPIO_Init(BAT_CE_PORT, &GPIO_InitStructure);
		
		BAT_CE_L();  // 禁止电池充电
	}
}




#include "delay.h"
// 设置数据口为输入或输出
// 板级管脚初始化
void Board_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 时钟使能
    STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	
    // XR1151 5V EN  
	GPIO_InitStructure.GPIO_Pin   = XR1151_EN_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		
	STM32_GPIO_Init(XR1151_EN_PORT, &GPIO_InitStructure);
	XR1151_EN_Open();

    #if 1
	// PWR_SW, 3.3V稳压器开关
	GPIO_InitStructure.GPIO_Pin   = PWR_SW_Pin;
	STM32_GPIO_Init(PWR_SW_PORT, &GPIO_InitStructure);
	PWR_SW_Open();  // 打开电源
	
	// LED_EN_Ctrl
	GPIO_InitStructure.GPIO_Pin   = LED_EN_Ctrl_Pin;
	STM32_GPIO_Init(LED_EN_Ctrl_PORT, &GPIO_InitStructure);
	LED_EN_Ctrl_Open();
	
	// SENSOR_Power Ctrl
	SENSOR_Ctrl_Set(SW_OPEN);  // 导通

	
	// TVOC_Power_Ctrl
	TVOC_Ctrl_Set(SW_OPEN);   // 导通  

	
	// GSM_Power_Ctrl 
	GSM_Ctrl_Set(SW_OPEN);

	

	// LCD_Power, 屏幕电源开关
    LCD_Ctrl_Set(SW_OPEN);
	
	// 充电检测管脚
	GPIO_InitStructure.GPIO_Pin   = CHRG_Indicate_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	STM32_GPIO_Init(CHRG_Indicate_PORT, &GPIO_InitStructure);

	// BEEP IO 管脚初始化
    GPIO_InitStructure.GPIO_Pin   = BEEP_IO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	STM32_GPIO_Init(BEEP_IO_PORT, &GPIO_InitStructure);
	BEEP_IO_Close();

	// 按键管脚初始化
	GPIO_InitStructure.GPIO_Pin   = KEY0_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;  // 上拉输入
	STM32_GPIO_Init(KEY0_PORT, &GPIO_InitStructure);
	#endif
}

