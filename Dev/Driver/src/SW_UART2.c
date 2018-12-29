
#include "sw_uart2.h"
#include "delay.h"
#include "reglib.h"
#include "os_global.h"
#include "timer_drv.h"

void SWUART2_GPIO_Init(void)
{
#if (SW_UART2_RX_EN | SW_UART2_TX_EN)
    GPIO_InitTypeDef GPIO_InitStructure;
#endif

#if SW_UART2_RX_EN
   EXTI_InitTypeDef EXTI_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SW_UART2_RX_PIN |RCC_APB2Periph_AFIO,ENABLE);  //外部中断，需要使能AFIO时钟
#endif

#if SW_UART2_TX_EN
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SW_UART2_TX_PIN, ENABLE); 
   
   GPIO_InitStructure.GPIO_Pin = SW_UART2_TX_PIN;	  
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 
   GPIO_Init(SW_UART2_TX_PORT, &GPIO_InitStructure);	 
   GPIO_SetBits(SW_UART2_TX_PORT,  SW_UART2_TX_PIN);	 
#endif

#if SW_UART2_RX_EN
   GPIO_InitStructure.GPIO_Pin = SW_UART2_RX_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //上拉输入//浮空输入
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	 
   GPIO_Init(SW_UART2_RX_PORT, &GPIO_InitStructure);	  
   GPIO_SetBits(SW_UART2_RX_PORT, SW_UART2_RX_PIN);                           
   
    
  GPIO_EXTILineConfig(GPIO_PortSource_SW_UART2_RX, GPIO_PinSource_SW_UART2_RX);

  EXTI_InitStructure.EXTI_Line            = EXTI_Line_SW_UART2_RX; 
  EXTI_InitStructure.EXTI_Mode         = EXTI_Mode_Interrupt; 
  EXTI_InitStructure.EXTI_Trigger     = EXTI_Trigger_Falling;  // 下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	

    NVIC_InitStructure.NVIC_IRQChannel = EXTI_SW_UART2_RX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
#endif
}


void EXTI_SW_UART2_RX_IRQHandler(void)
{
     if ((EXTI->PR & EXTI_Line_SW_UART2_RX))
    //if(EXTI_GetFlagStatus( EXTI_Line_SW_UART2_RX) != RESET) //外部中断标志位置位
   {
         EXTI->IMR &= ~(EXTI_Line_SW_UART2_RX);   // 屏蔽外部中断
         EXTI->PR       = EXTI_Line_SW_UART2_RX;           // 清除外部中断标记位
         
         SWUART2_TIM->CNT   = 0;                                    // TIM_SetCounter(TIM4, 0);
         SWUART2_TIM->CR1 |= TIM_CR1_CEN;    // TIM_Cmd(TIM4, ENABLE);  
    }
}






WEAK_ATTR  void SW_UART2_IRQHandler(uint8_t data)
{
       // do nothing
}

void SWUART2_TIM_IRQHandler(void)
{ 
       static uint8_t data       = 0;
       static uint8_t bit_cnt = 0; 
       
       if ((SWUART2_TIM->SR &  TIM_IT_Update) )
      {
                 uint8_t tmp = 0;
                 
                /* Clear the IT pending Bit */
               SWUART2_TIM->SR = (uint16_t)~TIM_FLAG_Update;
                tmp = IO_READ_IN(SW_UART2_RX_PORT, SW_UART2_RX_PIN);
                if(tmp)data |= (1 << bit_cnt); // LSB first
                 bit_cnt++;
                 if(bit_cnt  >= 8)
                {
                            bit_cnt = 0;
                            SWUART2_TIM->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));    /* Disable the TIM Counter */
                            SW_UART2_IRQHandler(data);
                            data = 0; 
                            EXTI->IMR |= EXTI_Line_SW_UART2_RX;   //开启外部中断
                }
        }
}


void SWUART2_Send(uint8_t Byte)
{
      uint8_t  i = 8;
      uint8_t tmp;
      SW_UART2_TX_L(); //发送起始位
      delay_us(104);
      
      //发送8位数据
      for(i = 0; i < 8; i++)
      {
              tmp = (Byte >> i)  & 0x01;  //低位在前
              if(tmp )
             {
                    SW_UART2_TX_H();
                    delay_us(104);
             }
             else
             {
                    SW_UART2_TX_L();
                    delay_us(104);	//1 
             } 
       }
       SW_UART2_TX_H();
       delay_us(104);
}



void SWUART2_Init(void)
{
        SWUART2_TIM_Init();
        SWUART2_GPIO_Init();
}





 	 










