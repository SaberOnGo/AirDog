
#include "PowerCtrl.h"
#include "Application.h"
#include "TimerManager.h"
#include "Uart_Drv.h"
void PowerCtrl_LowerPower(void)
{
   GLOBAL_DISABLE_IRQ();

   PowerCtrl_SetClock();

   #if 0
   SysTick_Init();
   #else
   SysTick->LOAD  = ((SystemCoreClock / 100) & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
   SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
   SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk   | SysTick_CTRL_ENABLE_Msk;  
   #endif
   
   #if 1
   STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO
		                            | RCC_APB2Periph_ADC1 | RCC_APB2Periph_USART1, DISABLE);
   STM32_RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB | RCC_APB1Periph_USART3| RCC_APB1Periph_SPI2, DISABLE);
   STM32_RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, DISABLE);
   #endif
   
   USART1->CR1 &=~ USART_CR1_UE;
   
   USART3->CR1 &=~ USART_CR1_UE;

   #if (EXTI_DEBUG_EN == 0)
   USART2->CR1 &=~ USART_CR1_UE;
   STM32_RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
   #endif

   //USART2_Init(FREQ_1MHz, 9600);

   GLOBAL_ENABLE_IRQ();
}

void PowerCtrl_ResumePower(void)
{
   GLOBAL_DISABLE_IRQ();
   RCC_DeInit();		
   SysClockConfig();
   SysTick_Config(SystemCoreClock / 100);  // 10 ms 中断一次

   #if 0
   STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
		                            | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO
		                            | RCC_APB2Periph_ADC1, ENABLE);
   STM32_RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB | RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3
   	                                | RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM2, ENABLE);
   #endif
   
   GLOBAL_ENABLE_IRQ();
}

