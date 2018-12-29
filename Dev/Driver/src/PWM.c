
#include "PWM.h"





static void PWM_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	/* GPIOA Configuration: Channe 3 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#define PWM_HZ  20

void  PWM_Init(void)
{
       TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
       TIM_OCInitTypeDef  TIM_OCInitStructure;
       uint16_t TimerPeriod = 0;
       uint16_t Channel3Pulse = 0;

      /* TIM1, GPIOB and AFIO clocks enable */
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 |RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO,  ENABLE);

      /* GPIO Configuration */
      PWM_GPIO_Configuration();

      // 设置TIM1_CH3N 重映射到PB1
      AFIO->MAPR &= (uint32_t)0XFFFFFF3F;
      AFIO->MAPR |= (uint32_t)0x00000040;
      
	/* TIM1 Configuration ---------------------------------------------------
	Generate 7 PWM signals with 4 different duty cycles:
	TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
	SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
	and Connectivity line devices and to 24 MHz for Low-Density Value line and
	Medium-Density Value line devices

	The objective is to generate 7 PWM signal at 40 KHz:
	 - TIM1_Period = (SystemCoreClock / 40000) - 1
	The channel 3 is set to 50 %
	The Timer pulse is calculated as follows:
	 - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
	----------------------------------------------------------------------- */
	/* Compute the value to be set in ARR regiter to generate signal frequency at 40 Khz */
	TimerPeriod = (SystemCoreClock / PWM_HZ ) - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1  */
	Channel3Pulse = (uint16_t) (((uint32_t) 500 * (TimerPeriod - 1)) / 1000);
	
	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

        /* Channel 1, 2,3 and 4 Configuration in PWM mode */
       TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
       TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
       TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
       TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
       //TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
       TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
     //  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
       TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 counter enable */
	//TIM_Cmd(TIM1, ENABLE);

	/* TIM1 Main Output Enable */
	//TIM_CtrlPWMOutputs(TIM1, ENABLE); 
}

// 设置占空比
// 参数: uint8_t ch: 通道号:  1 - 4,  PWM 1 - PWM4
// 参数: uint16_t pulse: 设置占空比, 如值为 500, 则表示占空比为50%
// 参数范围: 0 - 1000,  表示占空比: 0 % - 100 %
void PWM_SetDuty(uint16_t pulse)
{
       uint16_t TimerPeriod = 0;
       uint16_t ChannelPulse;

	TimerPeriod = (SystemCoreClock /  PWM_HZ ) - 1;  
	ChannelPulse = (uint16_t) (((uint32_t) pulse * (TimerPeriod - 1)) / 1000);

       TIM1->CCR1 = ChannelPulse;
}

// 设置占空比
// 参数: uint8_t ch: 通道号:  1 - 4,  PWM 1 - PWM4
// 参数: uint16_t pulse: 设置占空比, 如值为 500, 则表示占空比为50%
// 参数范围: 0 - 1000,  表示占空比: 0 % - 100 %
void PWM_SetFreq(uint16_t freq)
{
       uint16_t TimerPeriod = 0;
       uint16_t ChannelPulse;

	TimerPeriod = (SystemCoreClock / freq) - 1;  
	ChannelPulse = (uint16_t) (((uint32_t) 500  * (TimerPeriod - 1)) / 1000);

       TIM1->CCR1 = ChannelPulse;
}



void PWM_TIM_Cmd(TIM_TypeDef* TIMx,   FunctionalState NewState)
{
       if(NewState == ENABLE)
       {
               /* Enable the TIM Counter */
               TIMx->CR1 |= TIM_CR1_CEN;
               
               /* Enable the TIM Main Output */
              TIMx->BDTR |= TIM_BDTR_MOE;
       }
       else
       {
               /* Disable the TIM Counter */
               TIMx->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
               /* Disable the TIM Main Output */
              TIMx->BDTR &= (uint16_t)(~((uint16_t)TIM_BDTR_MOE));
       }
}

void PWM_Cmd(FunctionalState NewState)
{
       PWM_TIM_Cmd(TIM1, NewState);
}
