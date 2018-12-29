
#include "timer_drv.h"



//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为24 M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
//TIM3_Int_Init(999, SystemCoreClock / FREQ_1MHz -1);    // 1 MHz  freq of TIM4，when count to 1000 is 1 ms
void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

       TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE ); 
	//TIM_Cmd(TIM3, ENABLE); 	 
}

//TIM4_Int_Init(999, SystemCoreClock / FREQ_1MHz -1);    // 1 MHz  freq of TIM4，when count to 1000 is 1 ms
void TIM4_Int_Init(u16 arr, u16 psc)
{
       TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period        = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4,  &TIM_TimeBaseStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

       TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
       TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE ); 

	TIM_Cmd(TIM4,  DISABLE);  	// first disable  
}

//TIM5_Int_Init(999, SystemCoreClock / FREQ_1MHz -1);    // 1 MHz  freq of TIM5，when count to 1000 is 1 ms
void TIM5_Int_Init(u16 arr, u16 psc)
{
       TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 
	
	TIM_TimeBaseStructure.TIM_Period        = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM5,  &TIM_TimeBaseStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

       TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );
       TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE ); 

	TIM_Cmd(TIM5,  DISABLE);  	// first disable  
}

