
#include "stm32f10x.h"
#include "stdio.h"
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"
#include "GlobalDef.h"

extern uint16_t MAL_Init (uint8_t lun);

#if 0
typedef  void (*pFunction)(void);
#define ApplicationAddress 0x08008800
uint32_t JumpAddress;
pFunction Jump_To_Application;
#endif

void USB_BspGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIO_ResetBits(GPIOA, GPIO_Pin_12);
	
	//GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);
}



#define DELAY_TIME 0x7ffffful	

void usb_main(void)
{
		//SysClockConfig();
	    //RCC_PeriphInit();

		//USB_BspGpioInit();
	    uint32_t count = 0;

	
		USBSet_System();
		USBSet_USBClock();
		USB_Interrupts_Config();
		USB_Init();


        while ((bDeviceState != CONFIGURED) && (count++ <= DELAY_TIME));

		os_printf("USB config sta = %d\r\n", bDeviceState);
		
	    //关闭USB 接收和发送中断
	    //NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	    //NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
 
        #if 0
		__set_PRIMASK(1);
		if(((*(__IO uint32_t *) ApplicationAddress) & 0x2FFE0000) == 0x20000000)
				{
					JumpAddress = *(__IO uint32_t *) (ApplicationAddress + 4);
					Jump_To_Application = (pFunction) JumpAddress;
					
					__set_MSP(*(__IO uint32_t *) ApplicationAddress);
					Jump_To_Application();
				}
		#endif
    
}	


	 
