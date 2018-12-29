
#ifndef __EXTI_DRV_H__
#define  __EXTI_DRV_H__

#include "GlobalDef.h"

#define USB_PLUGED_MASK  0xF0
#define USB_CHARG_MASK   0x0F


extern uint8_t battery_is_charging;

void ExtiDrv_Init(void);
extern uint8_t App_GetRunMode(void);
void PowerOffDectect_Init(void);

#endif



