
#ifndef __DEBUG_QUEUE_H__
#define  __DEBUG_QUEUE_H__

#include "queue.h"
#include "Uart_Drv.h"

extern T_QueueObj debugQObj;

// 在新接收的数据是否含有 match 子字符串
__INLINE SYS_RESULT DebugUart_RxQueueStrStr(char * match)
{
   return queue_rx_str_str(&debugQObj, (uint8_t *)match);
}

__INLINE SYS_RESULT DebugUart_RxQueueStrChr(char c)
{
   return queue_rx_str_chr(&debugQObj, c);
}

// 接收是否为空: 1: 为空; 0: 不为空
__INLINE uint8_t DebugUart_IsRxEmpty(void)
{
    return queue_is_rx_buf_empty(&debugQObj);
}

// 读取一个字节并出队
__INLINE UartStatus DebugUart_ReadDequeueByte(uint8_t * pdata)
{
    return queue_pull_rx(&debugQObj, (QueueMemType *)pdata);
}

// 读取buff 并出队
__INLINE uint16_t  DebugUart_ReadDequeueBuff(uint8_t * pdata)
{
    return queue_pull_rx_all(&debugQObj, (QueueMemType *)pdata);
}

__INLINE void DebugUart_RxToTx(uint8_t * out_buf, uint16_t size)
{
   queue_rx_to_tx(&debugQObj,  (QueueMemType *)out_buf, size);
}



__INLINE UartStatus DebugUart_SendByte(uint8_t data)
{
   return queue_send(&debugQObj, data);
}

__INLINE void DebugUart_SendNByte(uint8_t * buf, uint16_t len)
{
    queue_send_n(&debugQObj, buf, len);
}

__INLINE void DebugUart_SendString(uint8_t * sring)
{
    queue_send_string(&debugQObj, sring);
}



void DebugQueue_Init(void);


#endif

