
#ifndef __UART_QUEUE_H__
#define __UART_QUEUE_H__


#include "GlobalDef.h"

#define  EN_USART_TCIE()       (USART1->CR1  |=    USART_CR1_TCIE)
#define  DIS_USART_TCIE()   (USART1->CR1 &= ~USART_CR1_TCIE)

#define  EN_USART_TXEIE()       (USART1->CR1  |=    USART_CR1_TXEIE)
#define  DIS_USART_TXEIE()   (USART1->CR1 &= ~USART_CR1_TXEIE)

// FIFO 队列
typedef struct
{
      uint8_t * buf;      // 队列缓冲区指针
      uint16_t   size;    // 队列缓冲区长度
      uint16_t   head;  // 环形队列头, 每接收到一个数据, head + 1
      uint16_t   tail;     // 环形队列尾, 每发送一个数据(该命令缓冲区为空), tail + 1
}T_UART_QUEUE;

void uart_q_init(void);
void uart_q_push(uint8_t data);

#endif

