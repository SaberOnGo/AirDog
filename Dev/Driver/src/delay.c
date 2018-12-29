
#include "stm32f10x.h"
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
#include <stdint.h>
#include "delay.h"
#include "GlobalDef.h"

#if 0
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//使用SysTick的普通计数模式对延迟进行管理
//包括delay_us,delay_ms
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/5/27
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.2修改说明
//修正了中断中调用出现死循环的错误
//防止延时不准确,采用do while结构!
//////////////////////////////////////////////////////////////////////////////////	 
static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
//初始化延迟函数
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init(u8 SYSCLK)
{
	//SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								    
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
}   
//延时nus
//nus为要延时的us数.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}


#else

#define DELAY_TIM_FREQUENCY_US 1000000		/* = 1MHZ -> timer runs in microseconds */
#define DELAY_TIM_FREQUENCY_MS 1000			/* = 1kHZ -> timer runs in milliseconds */

#define TIM2_TimeBaseInit(TIMx, TIM_TimeBaseInitStruct) {\
	TIMx->CR1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));\
	TIMx->CR1 |= (uint32_t)TIM_TimeBaseInitStruct.TIM_CounterMode;\
	TIMx->ARR = TIM_TimeBaseInitStruct.TIM_Period;\
	TIMx->PSC = TIM_TimeBaseInitStruct.TIM_Prescaler;\
	TIMx->EGR = TIM_PSCReloadMode_Immediate;\
	}

// Init timer for Microseconds delays
#if 0
void _init_us(void) 
{
	TIM_TimeBaseInitTypeDef TIM;

	// Enable clock for TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	// Time base configuration
	TIM_TimeBaseStructInit(&TIM);
	TIM.TIM_Prescaler = (SystemCoreClock/DELAY_TIM_FREQUENCY_US)-1;
	TIM.TIM_Period = UINT16_MAX;
	TIM.TIM_ClockDivision = 0;
	TIM.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM);

	// Enable counter for TIM2
	TIM_Cmd(TIM2,ENABLE);
}
#else

#if 1
#define _init_us() {\
	SET_REG_32_BIT(RCC->APB1ENR, RCC_APB1Periph_TIM2);\
	TIM2->PSC  = (SystemCoreClock / DELAY_TIM_FREQUENCY_US) - 1;\
	TIM2->ARR  = UINT16_MAX;\
	TIM2->EGR  = TIM_PSCReloadMode_Immediate;\
	TIM2->CR1 |= TIM_CR1_CEN;\
	}
#else
#define _init_us() {\
	TIM_TimeBaseInitTypeDef TIM;\
	SET_REG_32_BIT(RCC->APB1ENR, RCC_APB1Periph_TIM2);\
	TIM.TIM_Prescaler = (SystemCoreClock / DELAY_TIM_FREQUENCY_US) - 1;\
	TIM.TIM_Period = UINT16_MAX;\
	TIM.TIM_ClockDivision = 0;\
	TIM.TIM_CounterMode = TIM_CounterMode_Up;\
	TIM2_TimeBaseInit(TIM2, TIM);\
	SET_REG_32_BIT(TIM2->CR1, TIM_CR1_CEN);\
	}
#endif

#endif

// Init and start timer for Milliseconds delays
#if 0
void _init_ms(void) 
{

    TIM_TimeBaseInitTypeDef TIM;
	
	// Enable clock for TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	// Time base configuration
	
	TIM_TimeBaseStructInit(&TIM);
	TIM.TIM_Prescaler = (SystemCoreClock/DELAY_TIM_FREQUENCY_MS)-1;
	TIM.TIM_Period = UINT16_MAX;
	TIM.TIM_ClockDivision = 0;
	TIM.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM);

	// Enable counter for TIM2
	TIM_Cmd(TIM2,ENABLE);
}
#else
#define _init_ms() {\
	SET_REG_32_BIT(RCC->APB1ENR, RCC_APB1Periph_TIM2);\
	TIM2->PSC  = (SystemCoreClock / DELAY_TIM_FREQUENCY_MS) - 1;\
	TIM2->ARR  = UINT16_MAX;\
	TIM2->EGR  = TIM_PSCReloadMode_Immediate;\
	TIM2->CR1 |= TIM_CR1_CEN;\
	}
#endif

// Stop timer
#if 0
void _stop_timer(void) {
	TIM_Cmd(TIM2,DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE); // Powersavings?
}
#else
#define _stop_timer() {\
	CLEAR_REG_32_BIT(TIM2->CR1, TIM_CR1_CEN);\
	CLEAR_REG_32_BIT(RCC->APB1ENR, RCC_APB1Periph_TIM2);\
	}
#endif


// Do delay for nTime milliseconds
void delay_ms(uint32_t mSecs) 
{
       #if 0
        uint32_t start;
   
	// Init and start timer
	_init_ms();

	// Dummy loop with 16 bit count wrap around
	TIM2->CNT = 0;
	start = TIM2->CNT;
	while((TIM2->CNT-start) <= mSecs);

	// Stop timer
	_stop_timer();
	#else
	while(mSecs--)
	{
              delay_us(1000);
       }
	#endif
}


// Do delay for nTime microseconds
WEAK_ATTR void delay_us(uint32_t uSecs) 
{
    volatile uint32_t start;
	
	// Init and start timer
	_init_us();

	// Dummy loop with 16 bit count wrap around
	start = TIM2->CNT;
	while((TIM2->CNT-start) <= uSecs);

	// Stop timer
	_stop_timer();
}

#endif































