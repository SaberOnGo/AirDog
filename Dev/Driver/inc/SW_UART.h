
#ifndef __SW_UART_H__
#define __SW_UART_H__

#include "GlobalDef.h"

#define   SW_UART1_TX_EN      0   // 发送使能:1 ; 0: 禁止
#define   SW_UART1_RX_EN      1   // 接收使能(1), 0: 禁止


#define   SW_UART1_RX_MAX_LEN  	64   // 最大接收字节数


 // SW UART  TX: PC.2
#define   RCC_APB2Periph_SW_UART1_TX_PIN    RCC_APB2Periph_GPIOC
#define   SW_UART1_TX_PIN                    GPIO_Pin_2 
#define   SW_UART1_TX_PORT               GPIOC
#define   SW_UART1_TX_H()                    IO_H(SW_UART1_TX_PORT, SW_UART1_TX_PIN)
#define   SW_UART1_TX_L()                     IO_L(SW_UART1_TX_PORT, SW_UART1_TX_PIN)


// SW UART RX: PC.1
#define   RCC_APB2Periph_SW_UART1_RX_PIN    RCC_APB2Periph_GPIOC
#define   SW_UART1_RX_PIN                    GPIO_Pin_1   // PC 1
#define   SW_UART1_RX_PORT               GPIOC
#define   GPIO_PortSource_SW_UART1_RX              GPIO_PortSourceGPIOC
#define   GPIO_PinSource_SW_UART1_RX                 GPIO_PinSource1
#define    EXTI_Line_SW_UART1_RX                              EXTI_Line1
#define    EXTI_SW_UART1_RX_IRQn                           EXTI1_IRQn

#define    EXTI_SW_UART1_RX_IRQHandler             EXTI1_IRQHandler





void SWUART1_Init(void);
void SWUART1_Send(uint8_t Byte);

void SW_UART1_IRQHandler(uint8_t data);


#endif

