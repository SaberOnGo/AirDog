
#ifndef __WIFI_QUEUE_H__
#define __WIFI_QUEUE_H__

#include "queue.h"
#include "Uart_Drv.h"

extern T_QueueObj WifiQObj;


__INLINE UartStatus WifiUart_SendByte(uint8_t data)
{
       return queue_send(&WifiQObj, data);
}

void WifiQueue_Init(void);


#endif

