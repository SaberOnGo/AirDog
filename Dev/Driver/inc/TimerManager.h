
#ifndef __TIME_MANAGER_H__
#define  __TIME_MANAGER_H__

#include "stm32f10x.h"
#include <stdint.h>

#define SysTick_Open()  SysTick->CTRL |=  (SysTick_CTRL_ENABLE_Msk)
#define SysTick_Close() SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk)


extern uint8_t volatile flag10ms;
extern uint8_t volatile flag100ms;
extern uint8_t volatile flag_switch;
extern volatile uint16_t key_result;

void SysTick_Increment(void);
void SysTick_Init(void);
uint32_t OS_GetSysTick(void);

#endif

