
#ifndef __BATTERY_LEVEL_H__
#define  __BATTERY_LEVEL_H__

#include "GlobalDef.h"



uint8_t BatLev_GetPercent(void);
void BatLev_VoltToPercent(uint16_t bat_volt);
uint8_t BatteryIsCharging(void);

#endif

