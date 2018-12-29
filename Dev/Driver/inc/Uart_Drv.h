
#ifndef __UART_DRV__
#define __UART_DRV__

#include <stdint.h>
#include "stm32f10x.h"
#include "GlobalDef.h"

#pragma pack(1)
// 传感器接收管理
typedef struct
{                                
       uint16_t      size;                                 // 接收缓冲区长度
	//volatile
	uint16_t  rx_cnt;          // 接收计数
	//volatile
	uint16_t rx_len;           // 接收的数据长度
	//volatile 
	uint8_t   last_val;        // 上一次接收的值
	//volatile 
	uint8_t   is_rx;            // 包接收标志: 是否已接收到
	uint8_t   * buf;   // 接收缓冲区
}T_UART_Rx;	
#pragma pack()


void USART1_Init(uint32_t baudrate);
void USART2_Init(uint32_t baudrate);
void USART3_Init(uint32_t baudrate);
void USART4_Init(uint32_t baudrate);
void USART5_Init(uint32_t baudrate);


//#define  PM25_USART_IRQHandler       USART2_IRQHandler
#define  PM25_USART                                 USART2
#define  PM25_USART_Init                       USART2_Init

#define DEBUG_UART_RX_BUF_SIZE  1
#define DEBUG_UART_TX_BUF_SIZE  256

// 串口调试口也是 PM2.5 的接收口
#define DEBUG_UART             USART2
#define DebugUart_Init         USART2_Init
#define DEBUG_UART_BAUDRATE    9600
#define DEBUG_UART_IRQHandler  USART2_IRQHandler
#define DEBUG_UART_Close() {\
	STM32_RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);\
	STM32_USART_Cmd(DEBUG_UART, DISABLE);\
	}


	
#define  CO_USART_IRQHandler          USART3_IRQHandler
#define  CO_USART                                       USART3
#define  CO_USART_Init                             USART3_Init


#define  HCHO_USART_IRQHandler    //USART3_IRQHandler
#define  HCHO_USART                                 //USART3
#define  HCHO_USART_Init                      // USART3_Init

#define  WIFI_USART_IRQHandler       USART1_IRQHandler
#define  WIFI_USART                                   USART1
#define  WIFI_USART_Init                         USART1_Init
#define WIFI_UART_BAUDRATE           9600
#define WIFI_UART_TX_BUF_SIZE     256
#define WIFI_UART_RX_BUF_SIZE     1

#define  CO2_USART_IRQHandler          USART3_IRQHandler
#define  CO2_USART                                       USART3
#define  CO2_USART_Init                             USART3_Init



#endif

