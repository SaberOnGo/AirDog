


#ifndef __BOARD_V1_2_3_H__
#define  __BOARD_V1_2_3_H__



#include "stm32f10x.h"
#include "reglib.h"
#include "GlobalDef.h"

// LED0  ->  PA8
#define LED0_Pin                      GPIO_Pin_8
#define LED0_PORT                GPIOA
#define LED0_H()                          (LED0_PORT->BSRR =  LED0_Pin)   
#define LED0_L()                          (LED0_PORT->BRR     = LED0_Pin)     
#define LED0(v)                           (v ? LED0_H()  : LED0_L() )
#define LED0_Read()                    IO_READ_OUT(LED0_PORT,  LED0_Pin)


// LED1  ->  PD2
#define LED1_Pin                      GPIO_Pin_2
#define LED1_PORT                GPIOD
#define LED1_H()                          (LED1_PORT->BSRR =  LED1_Pin)   
#define LED1_L()                          (LED1_PORT->BRR     = LED1_Pin)     
#define LED1(v)                            (v ? LED1_H()  : LED1_L() )
#define LED1_Read()                     IO_READ_OUT(LED1_PORT,  LED1_Pin)



// KEY0 -> PC1
#define  KEY0_GPIO_Pin       GPIO_Pin_1
#define  KEY0_PORT                GPIOC
#define  KEY0_READ()            IO_READ_IN(KEY0_PORT,  KEY0_GPIO_Pin)

// KEY1 -> PC13
#define  KEY1_GPIO_Pin       GPIO_Pin_13
#define  KEY1_PORT                GPIOC
#define  KEY1_READ()           IO_READ_IN(KEY1_PORT,  KEY1_GPIO_Pin)


void Board_GpioInit(void);




#endif

