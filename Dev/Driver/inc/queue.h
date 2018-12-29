
#ifndef __QUEUE_H__
#define  __QUEUE_H__

#include <stdint.h>
#include "QueueDef.h"





static __inline void queue_do_nothing(USART_TypeDef * USARTx)
{

}


/*****************************************************************************
 * @\fn  : queue_disable_tx_int
 * @\author : pi
 * @\date : 2016 - 6 - 23
 * @\brief : 禁止UART TX中断
 * @\param[in] : void  
 * @\param[out] : none
 * @\return : 
 * @\attention : 
 * @\note [others] : 清除SR寄存器的TC,TXE bit, 禁止TIEN,TCIEN,TEN

*****************************************************************************/
static __inline void queue_disable_tx_int(USART_TypeDef * USARTx)
{
	 CLEAR_REG_32_BIT(USARTx->CR1, USART_CR1_TXEIE);  // 禁止发送空中断
	 CLEAR_REG_32_BIT(USARTx->CR1, USART_CR1_TCIE);    // 禁止发送完成中断
	 CLEAR_REG_32_BIT(USARTx->SR,    USART_SR_TC);            // 清中断完成标志位
}


// 使能发送中断
static __inline void queue_enable_tx_int(USART_TypeDef * USARTx)
{
     SET_REG_32_BIT(USARTx->CR1, USART_CR1_TXEIE);  //发送使能: 使能发送空中断
}


// 禁止接收中断
static __inline void queue_disable_rx_int(USART_TypeDef * USARTx)
{
    CLEAR_REG_32_BIT(USARTx->CR1, USART_CR1_RXNEIE);  // 禁止接收非空中断
}


// 使能接收中断
static __inline void queue_enable_rx_int(USART_TypeDef * USARTx)
{
    SET_REG_32_BIT(USARTx->CR1, USART_CR1_RXNEIE);  //接收使能: 使能接收非空中断
}
UartStatus queue_push_tx(T_QueueObj * obj, uint8_t data);
UartStatus queue_pull_tx(T_QueueObj * obj);

UartStatus queue_push_rx(T_QueueObj * obj, QueueMemType data);
UartStatus queue_pull_rx(T_QueueObj * obj, QueueMemType *pdata);
uint16_t queue_pull_rx_all(T_QueueObj * obj, QueueMemType * out_buf);
uint16_t queue_get_rx_all(T_QueueObj * obj, QueueMemType * out_buf);

void queue_rx_to_tx(T_QueueObj * obj,  QueueMemType * ReadOutBuff, uint16_t MaxSize);


// 接收队列是否为空
// 返回值: 1: 为空; 0: 不为空
#define queue_is_rx_buf_empty(obj)  ((obj)->op.RxQ.QHead == (obj)->op.RxQ.QTail)
void queue_clear_rx_q_buf(T_QueueObj * obj);
SYS_RESULT queue_rx_str_str(T_QueueObj * obj, uint8_t * match);
SYS_RESULT queue_rx_str_chr(T_QueueObj * obj, char c);

#define      queue_send(obj, data)  queue_push_tx(obj, data)
void queue_send_n(T_QueueObj * obj, uint8_t * buf, uint16_t len);
void queue_send_string(T_QueueObj * obj, uint8_t * sring);



void Queue_InstanceInit(T_QueueObj * obj,
                        USART_TypeDef * USARTx, 
                        uint32_t baudrate, 
                        uint8_t *tx_buf,
						uint16_t tx_buf_size,
						uint8_t *rx_buf,
						uint16_t rx_buf_size,
						void (* pDevInit)(uint32_t),
						void (*TxIntEn) (USART_TypeDef * ),   // 发送中断使能函数指针
						void (*TxIntDis)(USART_TypeDef * ),  // 发送中断禁止的函数指针
						void (*RxIntEn) (USART_TypeDef * ),  // 接收中断使能函数指针
						void (*RxIntDis)(USART_TypeDef * ) ); // 接收中断禁止的函数指针)


#endif

