
#include "board_V1_2_3.h"
#include "os_timer.h"
#include "delay.h"
#include "RegLib.h"






// 设置数据口为输入或输出
// 板级管脚初始化
void Board_GpioInit(void)
{
       GPIO_InitTypeDef GPIO_InitStructure;

       // 时钟使能
	STM32_RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB 
	                                                                             | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD ,  ENABLE);
	

	
	GPIO_InitStructure.GPIO_Pin   = LED1_Pin;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	STM32_GPIO_Init(LED1_PORT,  &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = LED0_Pin;
	STM32_GPIO_Init(LED0_PORT,  &GPIO_InitStructure);

       LED0(0);
       LED1(0);
       


}





