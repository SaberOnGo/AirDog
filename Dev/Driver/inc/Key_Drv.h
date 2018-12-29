
#ifndef __KEY_DRV_H__
#define  __KEY_DRV_H__

#include "stm32f10x.h"
#include "GlobalDef.h"


#if   DEBUG_KEY_EN
#define KEY_DEBUG(fmt, ...)  os_printf(fmt, ##__VA_ARGS__)
#else
#define KEY_DEBUG(...)
#endif

/****************************硬件定义 begin  ****************************************/

/****************************硬件定义 end ****************************************/

#define key_state_0 	0
#define key_state_1		1
#define key_state_2		2
#define key_state_3	       3
#define key_state_4		4	//按键需退出检测


//按键状态
#define N_key       0             //无键 
#define S_key       1             //单键
#define D_key       2             //双击
#define L_key       3             //长键 


#define KEY_MASK    0x03    // 只有2个按键, bit1-bit0
#define NO_KEY          0x03   // 

#define KEY_0              0x02   // 10, BIT0 == 0
#define KEY_1              0x01   // 01, BIT1 == 0

#define FUNC_KEY    KEY_0
#define NEXT_KEY    KEY_1







void key_gpio_init(void);
uint16_t key_scan(void);
uint16_t key_read(void);

void key_process(uint16_t keyval);

extern void key0_hook(uint8_t key_state);
extern void key1_hook(uint8_t key_state);

#endif

