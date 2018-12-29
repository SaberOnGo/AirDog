
#include "PWM_TIM5.h"
#include "PWM.h"


static void PWM_TIM5_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	/* GPIOA Configuration: Channe 3 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

#define PWM_TIM5_HZ    40000L

  /* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    The TIM3CLK frequency is set to SystemCoreClock (Hz), to get TIM3 counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices

    The TIM3 is running at 36 KHz: TIM3 Frequency = TIM3 counter clock/(ARR + 1)
                                                  = 24 MHz / 666 = 36 KHz
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
  ----------------------------------------------------------------------- */
void  PWM_TIM5_Init(void)
{
       TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
       TIM_OCInitTypeDef  TIM_OCInitStructure;

      uint16_t TimerPeriod = 0;
      uint16_t ChannelPulse = 0;
      
       /* TIM5 clocks enable */
       RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5 ,  ENABLE);
       
       /* GPIOA and AFIO clocks enable */
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO,  ENABLE);

       PWM_TIM5_GPIO_Configuration();

       TimerPeriod = (SystemCoreClock / PWM_TIM5_HZ ) - 1;
       /* Compute CCR1 value to generate a duty cycle at 50% for channel 1  */
	ChannelPulse = (uint16_t) (((uint32_t) 500 * (TimerPeriod - 1)) / 1000);
	
       /* Time base configuration */
       TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
       TIM_TimeBaseStructure.TIM_Prescaler = 0;
       TIM_TimeBaseStructure.TIM_ClockDivision = 0;
       TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
     
       TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
     
       /* PWM1 Mode configuration: Channel1 */
       
       TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
       TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

        #if 0
       TIM_OC1Init(TIM5, &TIM_OCInitStructure);
     
       TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
        #endif
        
       /* PWM1 Mode configuration: Channel2 */
       TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
       TIM_OCInitStructure.TIM_Pulse = ChannelPulse;
     
       TIM_OC2Init(TIM5, &TIM_OCInitStructure);
     
       TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);

       #if 0
       /* PWM1 Mode configuration: Channel3 */
       TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
       TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
     
       TIM_OC3Init(TIM5, &TIM_OCInitStructure);
     
       TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
     
       /* PWM1 Mode configuration: Channel4 */
       TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
       TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
     
       TIM_OC4Init(TIM5, &TIM_OCInitStructure);
     
       TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
        #endif
        
       TIM_ARRPreloadConfig(TIM5, ENABLE);
     
       /* TIM5 enable counter */
       TIM_Cmd(TIM5, ENABLE);
}

// 设置占空比
// 设置占空比
// 参数: uint8_t ch: 通道号:  1 - 4,  PWM 1 - PWM4
// 参数: uint16_t pulse: 设置占空比, 如值为 500, 则表示占空比为50%
// 参数范围: 0 - 1000,  表示占空比: 0 % - 100 %
void PWM_TIM5_SetDuty_Ch(PWM_CH ch, uint16_t pulse)
{
    uint16_t TimerPeriod = 0;
    uint16_t ChannelPulse;

     TIM_CtrlPWMOutputs(TIM5, DISABLE); 
     TimerPeriod = (SystemCoreClock / PWM_TIM5_HZ ) - 1;  
     ChannelPulse = (uint16_t) (((uint32_t) pulse * (TimerPeriod - 1)) / 1000);

    switch((uint8_t)ch)
    {
               case 1:  TIM5->CCR1 = ChannelPulse; break;
		 case 2:  TIM5->CCR2 = ChannelPulse; break;
		 case 3:  TIM5->CCR3 = ChannelPulse; break;
		 case 4:  TIM5->CCR4 = ChannelPulse; break;
		 default: TIM5->CCR2 = ChannelPulse; break;
     }
	 TIM_CtrlPWMOutputs(TIM5, ENABLE); 
}

void PWM_TIM5_SetDuty(uint16_t pulse)
{
       PWM_TIM5_SetDuty_Ch((PWM_CH)2, pulse);
}

void PWM_TIM5_Cmd(FunctionalState NewState)
{
       PWM_TIM_Cmd(TIM5,  NewState);
}

// 设置频率
void PWM_TIM5_SetFreq(uint16_t freq)
{
       uint16_t TimerPeriod = 0;
       uint16_t ChannelPulse;

	TimerPeriod = (SystemCoreClock / freq) - 1;  
	ChannelPulse = (uint16_t) (((uint32_t) 500  * (TimerPeriod - 1)) / 1000);

       TIM5->CCR1 = ChannelPulse;
}
