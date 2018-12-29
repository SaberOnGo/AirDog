
#include "sensor.h"
#include "uart_drv.h"
#include "os_timer.h"



#if CO_DBG_EN
#define CO_DBG(fmt, ...)    os_printf(fmt, ##__VA_ARGS__)
#else
#define CO_DBG(...)
#endif





static T_UART_Rx Rx;
static os_timer_t tTimerCO;


static SYS_RESULT  COBufCheck(uint8_t * buf, uint16_t len)
{
	 uint8_t new_sum = 0;  // 待计算的校验和
	 
       if(buf[0] != 0xFF && buf[1] != 0x04 || len < 2)
       {
              CO_DBG("COBufCheck: header err\r\n");
		return  SYS_FAILED;
       }
        new_sum = sys_gen_sum_8(&buf[1], len - 2);
        new_sum = (~new_sum) + 1;
        if(new_sum ==  buf[len - 1])return SYS_SUCCESS;

        CO_DBG("COBufCheck sum err\r\n");
        return SYS_FAILED;
}

static void TimerCO_CallBack(void * arg)
{
         if(COBufCheck(Rx.buf,  Rx.rx_cnt) != SYS_SUCCESS)return;

         
}

#if 0
void CO_USART_IRQHandler(void)
{ 
       if(READ_REG_32_BIT(CO_USART->SR, USART_SR_RXNE))
       {
                uint8_t data = 0;
	         data = (uint16_t)(CO_USART->DR & (uint16_t)0x01FF);
	    
               if(Rx.last_val == 0xFF && data == 0x04)  // header
		 {
			Rx.rx_cnt = 0;
			Rx.buf[Rx.rx_cnt++] = 0xFF;
		 }
		 else if(Rx.rx_cnt >= Rx.size)
		 {
			Rx.rx_cnt = 0;
                     // try to disable interrupt here
		 }
		 Rx.buf[Rx.rx_cnt++] = data;
		 Rx.last_val = data;
		 if(Rx.rx_cnt == 9)
		{
    			   // start rx timer here
    			   os_timer_arm(&tTimerCO,  0,  0);
    			   Rx.rx_cnt = 0;
	       }
       }
}
#endif

static uint8_t co_rx_buf[16];
void CO_Sensor_Init(void)
{
       CO_USART_Init(9600);
       Rx.buf    = co_rx_buf;
       Rx.size  = sizeof(co_rx_buf);
       os_timer_setfn(&tTimerCO,  TimerCO_CallBack,  NULL);
}

