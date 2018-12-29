
#ifndef __SW_UART2_H__
#define __SW_UART2_H__

#include "GlobalDef.h"

#define   SW_UART2_TX_EN      0   // 发送使能:1 ; 0: 禁止
#define   SW_UART2_RX_EN      1   // 接收使能(1), 0: 禁止


#define   SW_UART2_RX_MAX_LEN  	64   // 最大接收字节数


 // SW UART2  TX: PA.2
#define   RCC_APB2Periph_SW_UART2_TX_PIN    RCC_APB2Periph_GPIOA
#define   SW_UART2_TX_PIN                    GPIO_Pin_2 
#define   SW_UART2_TX_PORT               GPIOA
#define   SW_UART2_TX_H()                    IO_H(SW_UART2_TX_PORT, SW_UART2_TX_PIN)
#define   SW_UART2_TX_L()                     IO_L(SW_UART2_TX_PORT, SW_UART2_TX_PIN)


// SW UART2 RX: PA.3
#define   RCC_APB2Periph_SW_UART2_RX_PIN    RCC_APB2Periph_GPIOA
#define   SW_UART2_RX_PIN                    GPIO_Pin_3
#define   SW_UART2_RX_PORT               GPIOA
#define   GPIO_PortSource_SW_UART2_RX             GPIO_PortSourceGPIOA
#define   GPIO_PinSource_SW_UART2_RX                GPIO_PinSource3
#define    EXTI_Line_SW_UART2_RX                              EXTI_Line3
#define    EXTI_SW_UART2_RX_IRQn                           EXTI3_IRQn
#define    EXTI_SW_UART2_RX_IRQHandler             EXTI3_IRQHandler

void SWUART2_Init(void);
void SWUART2_Send(uint8_t Byte);

void SW_UART2_IRQHandler(uint8_t data);

#endif

