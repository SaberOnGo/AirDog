
#ifndef __GIZWITS_UART_H__
#define __GIZWITS_UART_H__

#include "stm32f10x.h"
#include "GlobalDef.h"
#include "gizwits_product.h"

typedef enum
{
      CMD_W2M_CtrlDev    = 0x02,    
   
   
      CMD_W2M_Reserved = 0xFF,
}CMD_WiFiToMCU;   // CMD  WIFI -> MCU 

typedef enum
{
     CMD_M2W_SetWiFi         = 0x01,    
     CMD_M2W_InfoReport    = 0x03,  
     CMD_M2W_Reserved      = 0xFF,
}CMD_MCUToWiFi;  // MCU -> WIFI

typedef enum
{
     E_INFO_Relay_Ctrl = EVENT_Relay_Ctrl,
     E_INFO_LED0_Ctrl  = EVENT_LED0_Ctrl,
     E_INFO_CO2,
     E_INFO_temp,
     E_INFO_hcho_ppb,
     E_INFO_humi,
     E_INFO_tvoc_ppb,
     E_INFO_pm10_ug,
     E_INFO_pm2p5_ug,
     E_INFO_END,
}E_INFO_TYPE;


void gizwits_uart_init(void);
void gizwits_set_wifi_mode(uint8_t mode);

#endif


