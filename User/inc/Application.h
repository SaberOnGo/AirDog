
#ifndef __APPLICATION_H__
#define  __APPLICATION_H__

void AppInit(void);
void SysClockConfig(void);
void PowerCtrl_SetClock(void);
void RCC_PeriphInit(void);
void NVIC_Configuration(void);
uint8_t App_GetRunMode(void);
void App_StartCO2BackgndCali(void);
void SysClockHSEConfig(void);

#define  SOFT_VERSION    "HV1.32 SV1.86"

#endif

