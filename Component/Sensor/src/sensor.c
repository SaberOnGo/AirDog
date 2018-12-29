
#include "sensor.h"
#include "board_version.h"



// 队列中剩余可用长度
uint16_t q_free_size(T_ROUND_QUEUE  * q)
{
	uint16_t end = q->size - 1 - q->head;

	return (end + q->tail) % q->size;
}

// 队列已用长度
uint16_t  q_used_size(T_ROUND_QUEUE  * q)
{
	uint16_t end = q->size - q->tail;
	return (end + q->head) % q->size;
}

/*********************
* 功能: 将数据放入队列
* 参数:  T_ROUND_QUEUE  * q: 队列指针
*               uint16_t data: 待放入队列的数据
* 返回值: SYS_SUCCESS  or  SYS_FAILED
*****************/
SYS_RESULT q_push(T_ROUND_QUEUE  * q, uint16_t data)
{

	if(((q->head + 1) % q->size) == q->tail)  // 队列队尾, 队列已全部填满
	{
		//os_printf("queue is full\r\n");
		return SYS_FAILED;
	}
	q->buf[q->head] = data;
	q->head = (q->head + 1) % q->size;
	// os_printf("q->head = %d, q->size = %d\r\n", q->head, q->size);
	return SYS_SUCCESS;
}

/*********************
* 功能:  将数据从队列中取出
* 参数:  T_ROUND_QUEUE  * q: 队列指针
*               uint16_t * data:  从队列中取出的数据
* 返回值: SYS_SUCCESS  or  SYS_FAILED
*****************/
SYS_RESULT q_pull(T_ROUND_QUEUE  * q, uint16_t *data)
{
	if(q->tail == q->head) // queue is empty
	{
		//os_printf("queue is empty\r\n");
		return SYS_FAILED;
	}
	*data = q->buf[q->tail];
	 q->tail = (q->tail + 1) % q->size;
	// os_printf("q->tail = %d, *data = %d \r\n", q->tail, *data);
	return SYS_SUCCESS;
}

/*
功能: 队列初始化, 初始化队列缓冲区指针及大小等
*/
void q_init(T_ROUND_QUEUE  * q, uint16_t * buf,  uint16_t buf_size)
{
      q->buf      = buf;
      q->size    = buf_size;
      q->head  = 0;
      q->tail      = 0;
}

extern void CO2_Sensor_Init(void);
extern void CO_Sensor_Init(void);
extern void HCHO_Sensor_Init(void);
extern void PM25_Sensor_Init(void);
extern void SHT20_Init(void);
extern void TVOC_Sensor_Init(void);
extern void PCF8563_Init(void);

extern void PM25_Sensor_PreInit(void);


void Sensor_PreInit(void)
{
#if 0
       GPIO_InitTypeDef GPIO_InitStructure;
	
	/* config USART2 clock */
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

       
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
       GPIO_Init(GPIOA, &GPIO_InitStructure);
 #endif      
       PM25_Sensor_PreInit();
}
void Sensor_Init(void)
{
         Sensor_PreInit();
         CO2_Sensor_Init();
      //   CO_Sensor_Init();
         HCHO_Sensor_Init();
         PM25_Sensor_Init();
         SHT20_Init();
        TVOC_Sensor_Init(); 
        
}

