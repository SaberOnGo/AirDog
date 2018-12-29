
#include "gizwits_queue.h"
#include "Uart_Drv.h"
#include "os_global.h"



//#include "nos_api.h"

static QueueMemType gizwits_rx_buf[1] = {0};
static QueueMemType gizwits_tx_buf[64] = {0};

T_QueueObj gizwitsQObj;



void GizwitsQueue_Init(void)
{
    Queue_InstanceInit(&gizwitsQObj, 
		DEBUG_UART,
		DEBUG_UART_BAUDRATE,
		gizwits_tx_buf,
		DEBUG_UART_TX_BUF_SIZE,
		gizwits_rx_buf,
		DEBUG_UART_RX_BUF_SIZE,
		DebugUart_Init,
		queue_enable_tx_int,
		queue_disable_tx_int,
		queue_enable_rx_int,
		queue_disable_rx_int);
}




#if PM25_UART_TYPE == HW_UART_TYPE
extern void HW_USART2_IRQHandler(void);
#endif

void DEBUG_UART_IRQHandler(void)
{
  #if 0
  if(STM32_USART_ReadFlagStatus(DEBUG_UART, USART_SR_RXNE))  // 接收非空
  {
     uint8_t data = STM32_USART_ReceiveData(DEBUG_UART);
	 
      /* Read one byte from the receive data register */
	 
     queue_push_rx(&debugQObj, data);
	 if(data == '\n')
	 {
	    cloud_sem_release_rx_debug_uart();
	 }
  }
  #else
  #if (PM25_UART_TYPE == HW_UART_TYPE)
  HW_USART2_IRQHandler();
  #endif
  #endif

  //if(USART_GetFlagStatus(DEBUG_UART, USART_FLAG_TC))   // 发送数据空
   if(STM32_USART_ReadFlagStatus(DEBUG_UART, USART_SR_TXE))   // 发送数据空
   {   
      /* Write one byte to the transmit data register */
       queue_pull_tx(&debugQObj);
   }
}