
#ifndef __QUEUE_DEF_H__
#define  __QUEUE_DEF_H__
/****************************************************************************
队列数据类型定义文件
****************************************************************************/

#include "GlobalDef.h"

typedef enum
{
   LESS_STATE = 0,  //小于
   EQUAL_STATE,     //等于
   MORE_STATE,      //大于
}Compare_State;  //比较状态值
//信号量的值
typedef enum
{
    UART_IDLE = 0,  //解锁
    UART_BUSY = !UART_IDLE
}Sema_State;


typedef ErrorStatus UartStatus; 
typedef uint8_t  QueueMemType;       //队列缓冲区存储类型
typedef uint16_t QueuePointerType;  //队列指针类型

typedef struct
{
   QueueMemType *Base;  //队列缓冲区存储位置
   uint16_t buf_size;    // 队列长度
   volatile QueuePointerType QHead;  //队列头指针, 向队列写入数据
   volatile QueuePointerType QTail;  //队列尾指针, 对发送来说,输出数据是向串口发送; 对接收来说, 输出是从接收队列中取
}T_UartQueue;

// 串口队列对象
typedef struct struct_queue_obj T_QueueObj;

// 队列绑定的设备
typedef struct
{
   USART_TypeDef * USARTx;  // 绑定的串口设备
   uint32_t baudrate;         // 波特率

   void (* DevInit)(uint32_t baudrate);  // 串口外设初始化
   
   void (*TxIntEn)(USART_TypeDef * USARTx);  // 使能发送中断的函数指针
   void (*TxIntDis)(USART_TypeDef * USARTx); // 禁止发送中断的函数指针
   void (*RxIntEn)(USART_TypeDef * USARTx);  // 使能接收中断的函数指针
   void (*RxIntDis)(USART_TypeDef * USARTx); // 禁止接收中断的函数指针
}T_QueueDev;

// 队列实现的操作: 出队, 入队
typedef struct
{
   T_QueueDev dev;       // 绑定的串口外设
   
   T_UartQueue TxQ;     // 发送队列
   T_UartQueue RxQ;     // 接收队列
   
   volatile uint8_t lock;    // 发送的信号量
   volatile uint8_t flag;   // 接收标志位

}T_QueueOperation;

// 串口队列对象
typedef struct struct_queue_obj
{
   T_QueueOperation op;
   
   UartStatus (*QueueSendByte)(T_QueueObj * obj, uint8_t data);  // 队列发送一个数据
   void (*QueueSendN   )(T_QueueObj * obj, uint8_t * buf, uint16_t len); // 队列发送N个数据
   void (*QueueSendString)(T_QueueObj * obj, uint8_t * string);     // 队列发送字符串
}T_QueueObj;


#endif

