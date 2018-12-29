
#ifndef __ADC_DRV_H__
#define  __ADC_DRV_H__


#include "GlobalDef.h"



void ADCDrv_DMALaunch(void);
void ADCDrv_SilentInit(void);
void ADCDrv_NormalLaunch(void);
void ADCDrv_DrawBatCapacity(uint8_t reflush);
void ADCDrv_TrigBatMeasure(void);
uint16_t ADCDrv_GetFirstCoreTemp(void);
uint16_t ADCDrv_GetCurCoreTemp(void);

#endif

