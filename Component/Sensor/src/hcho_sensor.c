
#include "sensor.h"
#include "uart_drv.h"
#include "sw_uart.h"
#include "os_timer.h"
#include "SnsGUI.h"
#include <math.h>


#if HCHO_DBG_EN
#define HCHO_DBG(fmt, ...)    os_printf(fmt, ##__VA_ARGS__)
#else
#define HCHO_DBG(...)
#endif



#define HCHO_PPB_ARRAY_SIZE      18  // 保存的接收甲醛传感器的数据数目
uint16_t rx_ppb[HCHO_PPB_ARRAY_SIZE];  // 接收的甲醛浓度值缓冲区

// 甲醛传感器数据处理
typedef struct
{
	 uint16_t new_ppb;           // 最新的甲醛值
	 uint16_t aver_ppb;          // 平均 ppb 值 = 0.001 ppm
	 uint16_t cali_ppb;            // 校正基础值
	
	 uint8_t  cal_done;              // 是否已校正: 0 不校正; 1: 校正
}T_HCHO_SNS;

// FIFO 队列
typedef struct
{
      uint16_t * buf;      // 队列缓冲区指针
      uint16_t   size;    // 队列缓冲区长度
      uint16_t   head;  // 环形队列头, 每接收到一个数据, head + 1
      uint16_t   tail;     // 环形队列尾, 每发送一个数据(该命令缓冲区为空), tail + 1
}T_FIFO_QUEUE;

void fifo_push(T_FIFO_QUEUE  * q, uint16_t data)
{

	if(((q->head + 1) % q->size) == q->tail)  // 队列头已到队列尾后面
	{
	       // 丢弃一个数据, 确保 q->tail 指向最早的数据
		q->tail = (q->tail + 1) % q->size;   
	}
	q->buf[q->head] = data;
	q->head = (q->head + 1) % q->size;
}

SYS_RESULT fifo_pull(T_FIFO_QUEUE * q, uint16_t * data)
{
	if(q->tail == q->head) // queue is empty
	{
		return SYS_FAILED;
	}
	*data = q->buf[q->tail];
	 q->tail = (q->tail + 1) % q->size;
	return SYS_SUCCESS;
}

void fifo_init(T_FIFO_QUEUE  * q,  uint16_t * buf, uint16_t buf_size)
{
      q->buf     = buf;
      q->size   = buf_size;
      q->head = 0;
      q->tail     = 0;
}



static T_HCHO_SNS hcho_sns;
static T_UART_Rx Rx;
static os_timer_t tTimerHCHO;
static T_FIFO_QUEUE hcho_fifo;
static uint8_t hcho_uart_buf[16];  // 串口接收缓冲区

uint16_t hcho_ppb_to_ug(uint16_t ppb)
{
      return (uint16_t)((double)ppb * 30 / 22.4); 
}

uint16_t hcho_ug_to_ppb(uint16_t ug)
{
      return (uint16_t)((double)ug * 22.4 / 30); 
}

static SYS_RESULT  HCHOBufCheck(uint8_t * buf, uint16_t len)
{
	 uint8_t new_sum = 0;  // 待计算的校验和
	 uint8_t i;
	 HCHO_DBG("hcho buf: \r\n");
	 for(i = 0; i < len; i++)
        {
                       HCHO_DBG("0x%x ", buf[i]);
        }
        HCHO_DBG("\r\nend \r\n");
              
       if(buf[0] != 0xFF && buf[1] != 0x17 || len < 2)
       {

              HCHO_DBG("HCHOBufCheck: header err: len = %d\r\n", len);
             
		return  SYS_FAILED;
       }
        new_sum = sys_gen_sum_8(&buf[1], len - 2);
        new_sum = (~new_sum) + 1;
        if(new_sum ==  buf[len - 1])return SYS_SUCCESS;

        HCHO_DBG("HCHOBufCheck sum err\r\n");
        return SYS_FAILED;
}

void Sensor_SetHCHOCaliBase(void)
{
       hcho_sns.cali_ppb = hcho_sns.new_ppb;
}
static void TimerHCHO_CallBack(void * arg)
{
        uint16_t ppb_now;
        
        if(HCHOBufCheck(Rx.buf,  Rx.rx_len) != SYS_SUCCESS)
        {
                 return;
        }
        hcho_sns.new_ppb = ((uint16_t)Rx.buf[4] << 8) + Rx.buf[5];
        fifo_push(&hcho_fifo, hcho_sns.new_ppb);
        HCHO_DBG("HCHO = %01d.%03d mg/m3 , tick = %ld \r\n", 
                               hcho_sns.new_ppb % 10000 / 1000,
                               hcho_sns.new_ppb % 1000, os_get_tick());
        ppb_now = (hcho_sns.new_ppb > hcho_sns.cali_ppb) ? 
                                 (hcho_sns.new_ppb - hcho_sns.cali_ppb) : (0);
        SDRR_SaveSensorPoint(SENSOR_HCHO, &ppb_now);
        do
	{
	      static u8 first_time = 1;
	      if(first_time)
	      {
	            first_time = 0;
	            SnsGUI_ClearCircle(CIRCLE_HCHO);
	      }
             SnsGUI_DisplayHCHO(ppb_now);
	}while(0);
}



#if  (HCHO_UART_TYPE == HW_UART_TYPE)
void HCHO_USART_IRQHandler(void)
#else
void SW_UART1_IRQHandler(uint8_t data)
#endif
{ 
       #if (HCHO_UART_TYPE == HW_UART_TYPE)
       if(READ_REG_32_BIT(HCHO_USART->SR, USART_SR_RXNE))
       #endif
       {
               #if (HCHO_UART_TYPE == HW_UART_TYPE)
                uint8_t data;
	         data = (uint16_t)(HCHO_USART->DR & (uint16_t)0x01FF);
	         #endif
	         
               if(Rx.last_val == 0xFF && data == 0x17)  // header
		 {
			Rx.rx_cnt = 0;
			Rx.buf[Rx.rx_cnt++] = 0xFF;
		 }
		 else if(Rx.rx_cnt >= Rx.size)
		 {
		      Rx.rx_cnt = 0;
		 }
		 
		 Rx.buf[Rx.rx_cnt++] = data;
		 Rx.last_val = data;
		 if(Rx.rx_cnt >= 9)
		{
    			   // start rx timer here immediatly
    			   if(Rx.buf[0] == 0xFF && Rx.buf[1] == 0x17)
    			   {
    			         os_timer_arm(&tTimerHCHO,  1,  0);
    			         Rx.rx_len = Rx.rx_cnt;
    			   }
    			   else
    			   {
    			            #if 1
                                 uint8_t i;
                                 for(i = 0; i < Rx.rx_cnt; i++)
                                 {
                                           HCHO_DBG("%x ", Rx.buf[i]);
                                 }
                                 HCHO_DBG("\r\n");
                                 #endif
    			   }
    			   Rx.rx_cnt = 0;
	       }
       }
}


void HCHO_Sensor_Init(void)
{
       Rx.buf   = hcho_uart_buf;
       Rx.size = sizeof(hcho_uart_buf);
       os_timer_setfn(&tTimerHCHO, TimerHCHO_CallBack,  NULL);
       
       fifo_init(&hcho_fifo, rx_ppb,  sizeof(rx_ppb) / sizeof(rx_ppb[0]));   // FIFO 队列初始化
       
       #if (HCHO_UART_TYPE  == HW_UART_TYPE)
       HCHO_USART_Init(9600);
       #else
       SWUART1_Init();
       #endif  
}



