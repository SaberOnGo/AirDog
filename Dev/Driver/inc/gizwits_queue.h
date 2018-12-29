
#ifndef __GIZWITS_QUEUE_H__
#define __GIZWITS_QUEUE_H__

#include "queue.h"
#include "Uart_Drv.h"

extern T_QueueObj gizwitsQObj;


__INLINE UartStatus GizwitsUart_SendByte(uint8_t data)
{
   return queue_send(&gizwitsQObj, data);
}


void GizwitsQueue_Init(void);



#endif

