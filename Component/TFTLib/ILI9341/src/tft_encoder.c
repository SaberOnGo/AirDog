#include "tft_encoder.h"

u16 ENC_getValue(void) {
    return (u16) (ENCODER_TIMx->CNT / 2);
}

void ENC_init(void)
{
	GPIO_InitTypeDef gpio;
	TIM_TimeBaseInitTypeDef timer_base;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    gpio.GPIO_Mode  = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Pin   = ENC_CLK_PIN | ENC_DT_PIN;
    GPIO_Init(ENCODER_GPIOx, &gpio);

    timer_base.TIM_Period      = 0xFFFF;
    timer_base.TIM_Prescaler   = 0;
    timer_base.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER_TIMx, &timer_base);

    TIM_EncoderInterfaceConfig(ENCODER_TIMx, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_Cmd(ENCODER_TIMx, ENABLE);
}

