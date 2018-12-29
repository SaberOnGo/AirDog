
#ifndef  __BEEP_DRV_H__
#define __BEEP_DRV_H__

#include "GlobalDef.h"

void Beep_Init(void);
void Beep(uint16_t tick,  uint32_t freq);
void Beep_Ctrl(void);


#endif

