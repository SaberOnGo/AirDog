
#include "gizwits_uart.h"
#include "os_timer.h"
#include "board_version.h"
#include "uart_drv.h"
#include  "wifi_queue.h"

#if   1
#define GIZ_PRINT   os_printf
#else
#define GIZ_PRINT(...)
#endif



static T_UART_Rx Rx;


#pragma pack (1)
typedef struct
{
   uint8_t cmd;
   E_RESULT (*ParseCmdFunc)(void * msg);  // 解析命令处理
}T_CMD_MAP;  // 命令映射处理表
#pragma pack()
//#pragma upack (1)

void  uart_send(uint8_t * buf, uint16_t len)
{
      uint16_t i = 0;
      
       while(len--)
       {
                /* e.g. write a character to the USART */
                WIFI_USART->DR = (buf[i++] & (uint16_t)0x01FF);
        		
                /* Loop until the end of transmission */
                while (!(WIFI_USART->SR & USART_FLAG_TXE));
        }
}

void gizwits_set_wifi_mode(uint8_t mode)
{
        uint8_t buf[32];
        uint16_t len = 0;
        uint8_t sum = 0;
        
        buf[len++] = 0xFF;
        buf[len++] = 0xFF;
        buf[len++] = 0;
        buf[len++] = 3;
        sum += buf[len++] = CMD_M2W_SetWiFi;
        sum += buf[len++] = mode;
        buf[len++] = sum;

        do
        {
               uint16_t i;

               GIZ_PRINT("tx to wifi: %d B \r\n",  len);
               for(i = 0; i < len; i++)
               {
                      GIZ_PRINT("0x%x\t", buf[i]);
               }
               GIZ_PRINT("\r\nend\r\n");
        }while(0);
        uart_send(buf, len);
}

static E_RESULT  FLASH_SAVE CMDFN_ParseNothing(void * msg)
{

 
        return APP_SUCCESS;
}

static E_RESULT  FLASH_SAVE CMDFN_ParseCtrlDev(void * msg)
{
       uint8_t * cmd_buf = (uint8_t *)msg;
       uint32_t info = 0;
       uint8_t type = 0;

       type = cmd_buf[5];
       info     = (uint32_t)cmd_buf[6] << 24;
       info  |= (uint32_t)cmd_buf[7] << 16;
       info  |= (uint32_t)cmd_buf[8] << 8;
       info  |= cmd_buf[9];
       GIZ_PRINT("parse ctrl dev cmd, type = %d, info = %ld\r\n", type, info);

        switch(type)
        {
                case E_INFO_CO2:
                {

                }break;
                case E_INFO_temp:
                {

                }break;
                case E_INFO_hcho_ppb:
                {

                }break;
                case E_INFO_tvoc_ppb:
                {

                }break;
                case E_INFO_pm10_ug:
                {

                }break;
                case E_INFO_pm2p5_ug:
                {

                }break;
                default:
                {

                }break;
        }
        return APP_SUCCESS;
}






// 命令映射表
static T_CMD_MAP WiFi2MCU_CMD[] = 
{
      {CMD_W2M_CtrlDev,             CMDFN_ParseCtrlDev,  },
      {CMD_W2M_Reserved,         CMDFN_ParseNothing,  }, 
};

#define  W2M_CMD_SIZE  (sizeof(WiFi2MCU_CMD) / sizeof(WiFi2MCU_CMD[0]))

static SYS_RESULT  GizBufCheck(uint8_t * buf, uint16_t len)
{
	 uint8_t new_sum = 0;  // 待计算的校验和
	 
       if(buf[0] != 0xFF && buf[1] != 0xFF || len < 5)
       {
              GIZ_PRINT("GizBufCheck: header err\r\n");
		return  SYS_FAILED;
       }
        new_sum = sys_gen_sum_8(&buf[4], len - 5);
        if(new_sum ==  buf[len - 1])return SYS_SUCCESS;

        GIZ_PRINT("GizBufCheck sum err\r\n");
        return SYS_FAILED;
}

static os_timer_t tTimerGizwits;   
static void TimerGizwits_CallBack(void * arg)
{
        uint8_t cmdIdx = 0;
        uint8_t cmdCode = 0;

        if(GizBufCheck(Rx.buf,  Rx.rx_cnt) != SYS_SUCCESS)return;
        
        cmdCode = Rx.buf[4];
        for(cmdIdx = 0; cmdIdx < W2M_CMD_SIZE; cmdIdx++)
       {
              if(cmdCode == WiFi2MCU_CMD[cmdIdx].cmd)
              {
                       GIZ_PRINT("exe cmd 0x%x \r\n",  cmdCode);
                       WiFi2MCU_CMD[cmdIdx].ParseCmdFunc(Rx.buf);
                       break;
              }
        }
}


//注意,读取USARTx->SR能避免莫名其妙的错误 
#if GIZWITS_TYPE  == GIZ_SOC
void WIFI_USART_IRQHandler(void)	
{
        if(READ_REG_32_BIT(WIFI_USART->SR, USART_SR_RXNE))
       {
                uint8_t data = 0;
	         data = (uint16_t)(WIFI_USART->DR & (uint16_t)0x01FF);
	    
               if(Rx.last_val == 0xFF && data == 0xFF)  // 起始头
		 {
			Rx.rx_cnt = 0;
			Rx.buf[Rx.rx_cnt++] = 0xFF;
		 }
		 else if(Rx.rx_cnt >= Rx.size)
		 {
			Rx.rx_cnt = 0;
			// 或者这里可以禁止接收中断
		 }
		 Rx.buf[Rx.rx_cnt++] = data;
		 Rx.last_val = data;
		 if(Rx.rx_cnt > 3)
		{
    		        uint16_t len;
    		   
    		        len = (((uint16_t)Rx.buf[2]) << 8) + Rx.buf[3];
    		        if((len + 4) == Rx.rx_cnt)
    		        {
    			          os_timer_arm(&tTimerGizwits,  0, 0);
    			          Rx.rx_cnt = 0;
    		       }
	       }
       }
}
// wifi 数据接收
#elif GIZWITS_TYPE == GIZ_MCU
void WIFI_USART_IRQHandler(void)
{
        if(READ_REG_32_BIT(WIFI_USART->SR, USART_SR_RXNE))
        {
               uint8_t data;

               data = (uint16_t)(WIFI_USART->DR & (uint16_t)0x01FF);
               gizPutData((uint8_t *)&data,  1);
        }
        
         //if(USART_GetFlagStatus(DEBUG_UART, USART_FLAG_TC))   // 发送数据空
        #if (GIZ_UART_SEL == UART_QUEUE)
        if(READ_REG_32_BIT(WIFI_USART->SR, USART_SR_TXE))
       {   
              /* Write one byte to the transmit data register */
              queue_pull_tx(&WifiQObj);
       }
       #endif
}
#endif


void giz_uart_write(uint8_t data)
{
            #if (GIZ_UART_SEL == UART_BLOCK)
             /* e.g. write a character to the USART */
             WIFI_USART->DR = (data & (uint16_t)0x01FF);
  
            /* Loop until the end of transmission */
            while (!(WIFI_USART->SR & USART_FLAG_TXE));
            #else
             WifiUart_SendByte(data);
            #endif
}




static uint8_t giz_rx_buf[64];

void gizwits_uart_init(void)
{
        Rx.buf   = giz_rx_buf;
        Rx.size = sizeof(giz_rx_buf);
        os_timer_setfn(&tTimerGizwits,  TimerGizwits_CallBack, NULL);
}

