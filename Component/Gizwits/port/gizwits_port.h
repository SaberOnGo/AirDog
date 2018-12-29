
#ifndef  __GIZWITS_PORT_H__
#define  __GIZWITS_PORT_H__

#include "stm32f10x.h"



void gizwits_user_init(void);
void gizwits_user_task(void);

void  WIFI_SetConnectStatus(uint8_t conn_sta);
uint8_t WIFI_GetConnectStatus(void);

char *PRODUCT_SECRET_STRING(void);
char * PRODUCT_KEY_STRING(void);


#endif

