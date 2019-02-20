
#include "TimerManager.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "GlobalDef.h"

uint8_t volatile flag10ms = 0;
uint8_t volatile flag_switch = 0;

void SysTick_Init(void)
{
    if(SysTick_Config(SystemCoreClock / (1000 / OS_PER_TICK_MS)))  // 10 ms 中断一次
    {
        while(1);
    }

}

#if (! SYS_RTOS_EN)
static volatile uint32_t sSysTick = 0;

#include "key_drv.h"
volatile uint16_t key_result;

void SysTick_Increment(void)
{
   sSysTick++;
   flag10ms = 1;  

   
   flag_switch ^= 1;
}
#endif

uint32_t OS_GetSysTick(void)
{
   #if SYS_RTOS_EN
   return OSTimeGet();
   #else
   return sSysTick;
   #endif
}


