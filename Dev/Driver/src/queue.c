

#include "queue.h"
#include "os_global.h"
#include "Uart_Drv.h"
#include "delay.h"



/*****************************************************************************
 * @\fn  : queue_pull_tx
 * @\author : pi
 * @\date : 2016 - 6 - 22
 * @\brief :  从环形队列中的尾部拉取一个数据, 串口发送出去
 * @\param[in] : none
 * @\param[out] : none
 * @\return : ERROR, SUCCESS
 * @\attention : 
 * @\note [others] : 内部调用

*****************************************************************************/
UartStatus queue_pull_tx(T_QueueObj * obj)
{
    T_UartQueue * q = &obj->op.TxQ;
	
    if(q->QHead == q->QTail)  //缓冲区为空, 无发送数据
    {
       obj->op.lock = UART_IDLE; //串口空闲
       obj->op.dev.TxIntDis(obj->op.dev.USARTx); // 禁止发送中断
       return ERROR;
    }
	
	
    //串口寄存器发送数据
	obj->op.dev.USARTx->DR = ((q->Base[q->QTail++]) & (uint16_t)0x01FF);
		
    // 更新尾指针的位置，如果到了缓冲区的末端，就自动返回到缓冲区的起始处
    q->QTail %= q->buf_size; //尾指针+1, 缓冲区回绕	
    
    return SUCCESS;
}


/*****************************************************************************
 * @\fn  :  queue_push_tx
 * @\author : pi
 * @\date : 2016 - 6 - 23
 * @\brief :  发送一个字节;
 *                 将数据写入发送队列, 并启动发送
 * @\param[in] : none
 * @\param[out] : none
 * @\return : 
 * @\attention : 
 * @\note [others] : 

*****************************************************************************/
UartStatus  queue_push_tx(T_QueueObj * obj, uint8_t data)
{
    T_UartQueue *q = &obj->op.TxQ;
    QueuePointerType tail = q->QTail;
    uint16_t cnt = 2000;

    while(((q->QHead + 1) % q->buf_size) == tail && cnt--)
   {
           delay_us(500);
           tail = q->QTail;
   }
   if(!cnt)return ERROR;
   
   q->Base[q->QHead++] = data;
   
   //更新头指针，如果到了缓冲区的末端，就自动返回到缓冲区的起始处
   q->QHead %= q->buf_size; //缓冲区回绕
   

    if(! obj->op.lock) //串口空闲
    {
              obj->op.dev.TxIntEn(obj->op.dev.USARTx); // 使能发送中断
     }
     obj->op.lock = UART_BUSY; //串口忙
	
	return SUCCESS;
}
/*****************************************************************************
 * @\fn  : queue_send_n
 * @\author : pi
 * @\date : 2016 - 6 - 22
 * @\brief : 将N个数据写入发送队列, 并启动发送
 * @\param[in] : uint8_t * buf: 发送数据的指针
               uint16_t len      数据长度
 * @\param[out] : none
 * @\return : 发送状态
 * @\attention : 
 * @\note [others] : 外部调用

*****************************************************************************/
void queue_send_n(T_QueueObj * obj, uint8_t * buf, uint16_t len)
{
       T_UartQueue * q = &obj->op.TxQ;
       QueuePointerType tail  = q->QTail;
       uint16_t index = 0;
       uint16_t cnt = 2000;
       
	if(((q->QHead + 1) % q->buf_size) == tail)return;  // 缓冲区撑满
	
	while(len--)
	{
	     q->Base[q->QHead++] = buf[index++];

            //更新头指针，如果到了缓冲区的末端，就自动返回到缓冲区的起始处
            q->QHead %= q->buf_size; //缓冲区回绕

	     if(! obj->op.lock) //串口空闲
	     {
                  obj->op.dev.TxIntEn(obj->op.dev.USARTx); // 使能发送中断
	     }
	     obj->op.lock = UART_BUSY; //串口忙
	    
            while(((q->QHead + 1) % q->buf_size) == tail  && cnt--)
            {
                  delay_us(500);
                  tail  = q->QTail;
            }
            if(!cnt)break;
	}
}

void queue_send_string(T_QueueObj * obj, uint8_t * sring)
{
    uint16_t len = 0;
    len = os_strlen((void *)sring);
    queue_send_n(obj, sring, len);
}






/*****************************************************************************
 * @\fn  : queue_get_rx_flag
 * @\author : pi
 * @\date : 2016 - 6 - 22
 * @\brief : check uart receive data or not 
 * @\param[in] : void  void
 * @\param[out] : none
 * @\return :  1: has data;  0: no data
 * @\attention : 
 * @\note [others] : 外部调用
*****************************************************************************/


uint16_t queue_get_rx_flag(T_QueueObj * obj)
{
    return obj->op.flag;
}


void queue_clear_rx_flag(T_QueueObj * obj)
{
   obj->op.flag = 0;
}



/***********************
功能: 清除接收缓冲区的buff
参数:  队列对象
返回值:  SYS_SUCCESS: 清除成功;  SYS_FAILED: 清除失败
***********************/
void queue_clear_rx_q_buf(T_QueueObj * obj)
{
   obj->op.dev.RxIntDis(obj->op.dev.USARTx);
   obj->op.RxQ.QTail = obj->op.RxQ.QHead;
   obj->op.dev.RxIntEn(obj->op.dev.USARTx);
}

/*****************************************************************************
 * @\fn  : queue_is_rx_buf_size_equal_than
 * @\author : pi
 * @\date : 2016 - 6 - 24
 * @\brief : 判断接收缓冲区的数据长度比compare_size 大, 小, 或等于
 * @\param[in] : uint16_t compare_size  要比较的大小值
 * @\param[out] :  
 * @\return : 比较结果: 缓冲区数据长度与参数的比较结果: More, Less, Equal
 * @\attention : 
 * @\note [others] : 

*****************************************************************************/
Compare_State queue_is_rx_buf_size_equal_than(T_QueueObj * obj, uint16_t compare_size)
{
    uint16_t BuffDataLen = 0; //输出的数据长度
    T_UartQueue * q = &obj->op.RxQ;
    QueuePointerType QHead = q->QHead;
	
	if(QHead == q->QTail) //缓冲区为空
	{
	   return LESS_STATE;
	}
    else if(QHead > q->QTail) //0  -> QTail -> QHead -> End -> 0, 头指针大于尾指针
	{
	   BuffDataLen = QHead - q->QTail;
	}else  //0  -> QHead -> QTail -> End -> 0
	{
	    BuffDataLen = QHead + obj->op.RxQ.buf_size - q->QTail;
	}

	if(BuffDataLen == compare_size)
	{
	   return EQUAL_STATE;  
	}
	else if(BuffDataLen > compare_size)
	{
	   return MORE_STATE;
	}
	else
	{
	   return LESS_STATE;
	}
}




/*****************************************************************************
 * @\fn  : queue_push_rx
 * @\author : pi
 * @\date : 2016 - 6 - 22
 * @\brief : 向接收队列存入一个数据
 * @\param[in] : QueueMemType data  no
 * @\param[out] : none
 * @\return : ERROR, SUCCESS
 * @\attention : 
 * @\note [others] : 

*****************************************************************************/
UartStatus queue_push_rx(T_QueueObj * obj, QueueMemType data)
{
    T_UartQueue * q = &obj->op.RxQ;
	
    if(((q->QHead + 1) % q->buf_size ) == q->QTail) //缓冲区撑满
	{
	   if(! obj->op.flag)
	   {
	      obj->op.flag = 1; // 置接收满标志位为 1
	   }
	   return ERROR;
	}
	q->Base[q->QHead++] = data; //存入一个数据
    // 更新头指针，如果到了缓冲区的末端，就自动返回到缓冲区的起始处
    q->QHead %= q->buf_size; //头指针 + 1, 缓冲区回绕
	
    return SUCCESS;
}

/*****************************************************************************
 * @\fn  : queue_pull_rx
 * @\author : pi
 * @\date : 2016 - 6 - 22
 * @\brief : 从接收队列中读取一个数据
 * @\param[in] : QueueMemType *data  pointer
 * @\param[out] : 从接收队列中读取的一个数据的指针
 * @\return : ERROR, SUCCESS
 * @\attention : 
 * @\note [others] : 

*****************************************************************************/
UartStatus queue_pull_rx(T_QueueObj * obj, QueueMemType *pdata)
{
    T_UartQueue * q = &obj->op.RxQ;
    QueuePointerType QHead = q->QHead;
	
    if(QHead == q->QTail) //接收缓冲区为空,无数据
    {
       return ERROR;
    }
	*pdata = q->Base[q->QTail++]; //从队列中取出数据
	q->QTail %= q->buf_size;      //尾指针+1, 缓冲区回绕

	
	return SUCCESS;
}

/*
判断新接收的队列中是否有匹配的字符

*/
SYS_RESULT queue_rx_str_str(T_QueueObj * obj, uint8_t * match)
{
    T_UartQueue * q = &obj->op.RxQ;
	uint16_t head = q->QHead;
	uint16_t tail = q->QTail;
	uint16_t n = 0, offset = 0;

    if(head == tail) //接收缓冲区为空,无数据
    {
       return SYS_FAILED;
    }
	while(((tail + n + offset) % q->buf_size) != head)
	{
	   
	   if(q->Base[(tail + n + offset) % q->buf_size] == match[n])
	   {
          n++;  // 比较下一个字符
	   }
	   else  // 进行下一轮比较
	   {
	      offset++;
		  n = 0;
	   }
	   if(match[n] == 0)return SYS_SUCCESS;
	}
	if(match[n] == 0)return SYS_SUCCESS;  // 刚好匹配完成
	return SYS_FAILED;

}

SYS_RESULT queue_rx_str_chr(T_QueueObj * obj, char c)
{
    T_UartQueue * q = &obj->op.RxQ;
	uint16_t head = q->QHead;
	uint16_t tail  = q->QTail;
	
    if(head == tail)return SYS_FAILED; //接收缓冲区为空,无数据
	for(; tail != head;)
	{
	    if(q->Base[tail++] == c)return SYS_SUCCESS;
		tail %= q->buf_size;
	}
	return SYS_FAILED;
}

/*****************************************************************************
 * @\fn  : queue_pull_all
 * @\author : pi
 * @\date : 2016 - 6 - 22
 * @\brief : 从串口接收缓冲区读取全部数据, 并且数据已出队
 * @\param[in] : QueueMemType * out_buf
 * @\param[out] : out_buf: 输出的缓冲区数据指针
 * @\return : 
 * @\attention : 
 * @\note [others] : 外部调用

*****************************************************************************/
uint16_t queue_pull_rx_all(T_QueueObj * obj, QueueMemType * out_buf)
{
	register uint16_t index = 0;
	#if 0
	while(queue_pull_rx(obj, &out_buf[index]) == SUCCESS)
	{
	   index++;
	}
    #else

    T_UartQueue * q = &obj->op.RxQ;
    QueuePointerType QHead = q->QHead;
	
    if(QHead == q->QTail) //接收缓冲区为空,无数据
    {
       return ERROR;
    }
	for(; q->QTail != QHead; index++)
	{
	    out_buf[index] = q->Base[q->QTail++];
		q->QTail %= q->buf_size;
	}
	out_buf[index] = 0;
	#endif

	return index;
}

/*
功能:   与queue_pull_rx_all 功能类似， 但是数据仍在队列中
*/
uint16_t queue_get_rx_all(T_QueueObj * obj, QueueMemType * out_buf)
{
	register uint16_t index = 0;

    T_UartQueue * q = &obj->op.RxQ;
    QueuePointerType QHead = q->QHead;
	QueuePointerType tail = q->QTail;
	
    if(QHead == tail) //接收缓冲区为空,无数据
    {
       return ERROR;
    }
	for(; tail != QHead; index++)
	{
	    out_buf[index] = q->Base[tail++];
		tail  %= q->buf_size;
	}
	out_buf[index] = 0;

	return index;
}

/*****************************************************************************
 * @\fn  : queue_rx_to_tx
 * @\author : pi
 * @\date : 2016 - 6 - 24
 * @\brief : 将串口接收到的数据即刻发送出去
 * @\param[in] :  uint16_t MaxSize: 数组长度 
 * @\param[out] : uint8_t * ReadOutBuff ： 输出数据的指针
 * @\return : 
 * @\attention : 
 * @\note [others] : 

*****************************************************************************/
void queue_rx_to_tx(T_QueueObj * obj,  QueueMemType * ReadOutBuff, uint16_t MaxSize)
{
    uint16_t len = 0;

	len = queue_pull_rx_all(obj, ReadOutBuff);
	
	if(len > MaxSize)
	{
	   len = MaxSize;
	}
	if(len)
	{
	   queue_send_n(obj, ReadOutBuff, len);
	}
}








// 队列设备对象初始化
static void queue_dev_init(T_QueueDev * QDev, 
                   USART_TypeDef * USARTx, 
                   uint32_t baudrate, 
                   void (* pDevInit)(uint32_t),
                   void (*TxIntEn)(USART_TypeDef * USARTx),  // 发送中断使能函数指针
                   void (*TxIntDis)(USART_TypeDef * USARTx), // 发送中断禁止的函数指针
                   void (*RxIntEn)(USART_TypeDef * USARTx),  // 接收中断使能函数指针
                   void (*RxIntDis)(USART_TypeDef * USARTx)  ) // 接收中断禁止的函数指针
{
   QDev->USARTx   = USARTx;
   QDev->baudrate = baudrate;
   QDev->DevInit  = pDevInit;
   QDev->TxIntEn  = TxIntEn;
   QDev->TxIntDis = TxIntDis;
   QDev->RxIntEn  = RxIntEn;
   QDev->RxIntDis = RxIntDis;

   QDev->DevInit(QDev->baudrate);  // 初始化串口外设
}

// 公共方法绑定及初始化公共成员
static void operation_init(T_QueueObj * obj, 
                                 uint8_t * tx_buf, uint16_t tx_buf_size, 
                                 uint8_t * rx_buf, uint16_t rx_buf_size)
{
   obj->QueueSendByte      = queue_push_tx;
   obj->QueueSendN             = queue_send_n;
   obj->QueueSendString = queue_send_string;

   obj->op.lock = 0;
   obj->op.flag = 0;

   os_memset(&obj->op.RxQ, 0, sizeof(obj->op.RxQ));
   os_memset(&obj->op.TxQ, 0, sizeof(obj->op.TxQ));

   // 初始化队列
   obj->op.TxQ.Base      = tx_buf;
   obj->op.TxQ.buf_size  = tx_buf_size;
   obj->op.RxQ.Base      = rx_buf;
   obj->op.RxQ.buf_size  = rx_buf_size;
}












// 对象实例初始化
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
						void (*RxIntDis)(USART_TypeDef * ) ) // 接收中断禁止的函数指针)
{
	operation_init(obj, tx_buf, tx_buf_size, rx_buf, rx_buf_size);
	queue_dev_init(&obj->op.dev, 
		             USARTx, 
		             baudrate, 
		             pDevInit,
		             TxIntEn, 
		             TxIntDis,
		             RxIntEn,
		             RxIntDis);
}




