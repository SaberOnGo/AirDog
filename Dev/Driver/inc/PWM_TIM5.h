
#ifndef  __PWM_TIM5_H__
#define   __PWM_TIM5_H__


#include "GlobalDef.h"


void  PWM_TIM5_Init(void);
void  PWM_TIM5_SetDuty(uint16_t pulse);
void  PWM_TIM5_Cmd(FunctionalState NewState);
void  PWM_TIM5_SetFreq(uint16_t freq);


#endif

