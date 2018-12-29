
#include "beep_drv.h"
#include "PWM.h"
#include "timer_drv.h"
#include "board_version.h"
#include "os_global.h"
#include "delay.h"

#if   BEEP_DBG_EN
#define BEEP_DBG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define BEEP_DBG(...)
#endif

void Beep_Init(void)
{
       PWM_Init();
       TIM3_Int_Init(999,  SystemCoreClock / 100000  -1);  //   10 ms 定时
} 

/*
参数:  uint16_t  tick: 持续时间, unit: 1 0 ms, 最大时间: 655 sec
              uint32_t freq: 鸣叫频率
*/
//TIM3->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));    /* Disable the TIM Counter */
static volatile uint32_t beep_tick = 0;
static volatile uint32_t beep_cnt  = 0;

void Beep(uint16_t tick,  uint32_t freq)
{
        if(beep_tick)return;
        beep_tick = tick;
        PWM_SetFreq(freq);
        PWM_Cmd(ENABLE);
        BEEP_DBG("beep begin tick = %ld \r\n",  os_get_tick());
        TIM3->CR1 |= TIM_CR1_CEN;
}

void Beep_Ctrl(void)
{
       Beep(50,  2000);
      
}

void TIM3_IRQHandler(void)
{ 
       if ((TIM3->SR &  TIM_IT_Update) )
      {    
                /* Clear the IT pending Bit */
                TIM3->SR = (uint16_t)~TIM_FLAG_Update;
                beep_cnt++;
                if(beep_cnt >= beep_tick)
                {
                      BEEP_DBG("beep end tick = %ld \r\n",  os_get_tick());
                      beep_cnt = beep_tick = 0;
                      TIM3->CR1 &= (uint16_t)(~TIM_CR1_CEN);
                      PWM_Cmd(DISABLE);
                      BEEP_IO_Close();
                }
        }
}

