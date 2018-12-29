
#ifndef __PWM_H__
#define __PWM_H__


#include "GlobalDef.h"

typedef enum
{
     MC2_PWM_CH  = 1,
     MC1_PWM_CH  = 2,
     RF1_PWM_CH  = 3,
     RF2_PWM_CH  = 4,
}PWM_CH;

void  PWM_Init(void);
void  PWM_SetDuty(uint16_t pulse);
void  PWM_Cmd(FunctionalState NewState);
void PWM_SetFreq(uint16_t freq);

void PWM_TIM_Cmd(TIM_TypeDef* TIMx,   FunctionalState NewState);

#endif


