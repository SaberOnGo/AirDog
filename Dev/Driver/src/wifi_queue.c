
#include  "wifi_queue.h"

static QueueMemType wifi_rx_buf[WIFI_UART_RX_BUF_SIZE] = {0};
static QueueMemType wifi_tx_buf[WIFI_UART_TX_BUF_SIZE] = {0};

T_QueueObj WifiQObj;



void WifiQueue_Init(void)
{
    Queue_InstanceInit(&WifiQObj, 
		WIFI_USART,
		WIFI_UART_BAUDRATE,
		wifi_tx_buf,
		WIFI_UART_TX_BUF_SIZE,
		wifi_rx_buf,
		WIFI_UART_RX_BUF_SIZE,
		WIFI_USART_Init,
		queue_enable_tx_int,
		queue_disable_tx_int,
		queue_enable_rx_int,
		queue_disable_rx_int);
}

