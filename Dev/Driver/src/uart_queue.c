
#include "uart_queue.h"
#include "delay.h"

uint8_t uart_snd_buf[64];

void uart_fifo_push(T_UART_QUEUE * q, uint8_t data)
{

	if(((q->head + 1) % q->size) == q->tail)  // 队列头已到队列尾后面
	{
	       // 丢弃一个数据, 确保 q->tail 指向最早的数据
		//q->tail = (q->tail + 1) % q->size;  
		uint32_t cnt = 2000;
		
		do
		{
                      delay_us(1);
		}
		while(((q->head + 1) % q->size) == q->tail && cnt--);
	}
	q->buf[q->head] = data;
	q->head = (q->head + 1) % q->size;
}

SYS_RESULT uart_fifo_pull(T_UART_QUEUE * q, uint8_t * data)
{
	if(q->tail == q->head) // queue is empty
	{
		return SYS_FAILED;
	}
	*data = q->buf[q->tail];
	 q->tail = (q->tail + 1) % q->size;
	return SYS_SUCCESS;
}

static void uart_fifo_init(T_UART_QUEUE * q,  uint8_t * buf, uint16_t buf_size)
{
      q->buf     = buf;
      q->size   = buf_size;
      q->head = 0;
      q->tail     = 0;
}

T_UART_QUEUE uart_q;
void uart_q_init(void)
{
      uart_fifo_init(&uart_q,  uart_snd_buf, sizeof(uart_snd_buf));
}

//volatile uint8_t lock_snd = 0;
void uart_q_push(uint8_t data)
{
      uart_fifo_push(&uart_q, data);
      EN_USART_TXEIE();
      
}

#if PM25_UART_TYPE == HW_UART_TYPE
extern void HW_USART2_IRQHandler(void);
#endif

void USART2_IRQHandler(void)
{
#if PM25_UART_TYPE == HW_UART_TYPE
       HW_USART2_IRQHandler();
#endif  
       if(USART2->SR & USART_SR_TXE)
       {
                uint8_t data = 0;

                USART2->SR &= ~ (USART_SR_TXE);
                if(! uart_fifo_pull(&uart_q, &data))
                {
                       USART2->DR = (data & (uint16_t)0x01FF);
                }
                else
                {
                       DIS_USART_TXEIE();
                }
       }
}

