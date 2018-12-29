
#ifndef __TIME_DRV_H__
#define  __TIME_DRV_H__


#include "GlobalDef.h"

void TIM3_Int_Init(u16 arr, u16 psc);
void TIM4_Int_Init(u16 arr, u16 psc);
void TIM5_Int_Init(u16 arr, u16 psc);

#define  SWUART1_TIM                                  TIM4
#define  SWUART1_TIM_IRQHandler      TIM4_IRQHandler
#define  SWUART1_TIM_Init()                    TIM4_Int_Init(103,  SystemCoreClock / FREQ_1MHz -1)

#define  SWUART2_TIM                                   TIM5
#define  SWUART2_TIM_IRQHandler      TIM5_IRQHandler
#define  SWUART2_TIM_Init()                    TIM5_Int_Init(103,  SystemCoreClock / FREQ_1MHz -1)


#endif

