/**
************************************************************
* @file         gizwits_product.h
* @brief        Corresponding gizwits_product.c header file (including product hardware and software version definition)
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值?|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#ifndef _GIZWITS_PRODUCT_H
#define _GIZWITS_PRODUCT_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "gizwits_protocol.h"

#include "GlobalDef.h"
//#define PROTOCOL_DEBUG                              ///<Protocol data print


/**
* MCU software version
*/

/**
* MCU Hardware version
*/


/**
* Communication module type
*/
#define MODULE_TYPE 0 //0,WIFI ;1,GPRS




//extern uint32_t volatile timerMsCount;
extern dataPoint_t currentDataPoint;

void userInit(void);
void userHandle(void);
void mcuRestart(void);
int32_t uartWrite(uint8_t *buf, uint32_t len);
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
