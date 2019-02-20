
#include "stm32f10x.h"
#include "GlobalDef.h"
#include "board_version.h"
#include "uart_drv.h"
#include "TimerManager.h"
#include "os_timer.h"
#include "os_global.h"
#include "delay.h"
#include "sensor.h"
#include "QDTFT_demo.h"
#include "Application.h"



#ifndef USE_STD_LIB
#define REG_HSI_CR_HSION_BB  ((uint32_t)0x42420000)
#define REG_CR_PLLON_BB      ((uint32_t)0x42420060)


#define ACR_LATENCY_Mask         ((uint32_t)0x00000038)
#define ACR_PRFTBE_Mask          ((uint32_t)0xFFFFFFEF)

#define CFGR_PPRE1_Reset_Mask     ((uint32_t)0xFFFFF8FF)
#define CFGR_PPRE2_Reset_Mask     ((uint32_t)0xFFFFC7FF)
#define CFGR_HPRE_Reset_Mask      ((uint32_t)0xFFFFFF0F)

#define CFGR_SW_Mask              ((uint32_t)0xFFFFFFFC)
#define CFGR_SWS_Mask             ((uint32_t)0x0000000C)


/* CFGR register bit mask */
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) 
 #define CFGR_PLL_Mask            ((uint32_t)0xFFC2FFFF)
#else
 #define CFGR_PLL_Mask            ((uint32_t)0xFFC0FFFF)
#endif /* STM32F10X_CL */ 


// 参数: state: ENABLE  or DISABLE
#define STM32_RCC_HSICmd(state) \
	(*(__IO uint32_t *) REG_HSI_CR_HSION_BB = (uint32_t)state)  //打开内部高速时钟

#define STM32_FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer) \
	(FLASH->ACR |= (FLASH->ACR & ACR_PRFTBE_Mask) + FLASH_PrefetchBuffer)

#define  STM32_FLASH_SetLatency(FLASH_Latency) \
	(FLASH->ACR  = (FLASH->ACR & ACR_LATENCY_Mask) | FLASH_Latency)

#define  STM32_RCC_HCLKConfig(RCC_SYSCLK) \
	(RCC->CFGR	= (RCC->CFGR & CFGR_HPRE_Reset_Mask ) | RCC_SYSCLK)

#define  STM32_RCC_PCLK2Config(RCC_HCLK) \
	(RCC->CFGR   = (RCC->CFGR & CFGR_PPRE2_Reset_Mask) | (RCC_HCLK << 3))

#define  STM32_RCC_PCLK1Config(RCC_HCLK) \
	(RCC->CFGR	= (RCC->CFGR & CFGR_PPRE1_Reset_Mask) | (RCC_HCLK))

#define   STM32_RCC_PLLConfig(RCC_PLLSource, RCC_PLLMul) \
	(RCC->CFGR = (RCC->CFGR & CFGR_PLL_Mask) | RCC_PLLSource | RCC_PLLMul) 

// 参数: state: ENABLE  or DISABLE
#define STM32_RCC_PLLCmd(state) \
	(*(__IO uint32_t *) REG_CR_PLLON_BB = (uint32_t)state)


#define STM32_RCC_SYSCLKConfig(RCC_SYSCLKSource) \
	(RCC->CFGR = (RCC->CFGR & CFGR_SW_Mask) | RCC_SYSCLKSource)



#endif

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : 配置不同的系统时钟
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysClockConfig(void)
{

#ifdef USE_STD_LIB

   #if (! HSIENABLE)
		ErrorStatus HSEStartUpStatus;
   #endif

        /* RCC system reset(for debug purpose) */
        RCC_DeInit();

    #if    HSIENABLE                //当使用HSI高速内部时钟作为系统时钟时
        RCC_HSICmd(ENABLE);        //打开内部高速时钟
        
        //等待HSI准备好
        #if USE_STD_LIB
        while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
		#else
        while(! (READ_REG_32_BIT(RCC->CR, RCC_CR_HSIRDY)));
		#endif

        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);        //开启FLASH预取指功能
        //FLASH时序控制
        //推荐值:SYSCLK = 0~24MHz   Latency=0
        //       SYSCLK = 24~48MHz  Latency=1
        //       SYSCLK = 48~72MHz  Latency=2
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1);        //设置HCLK(AHB时钟)=SYSCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);                //PCLK2(APB2) = HCLK
        RCC_PCLK1Config(RCC_HCLK_Div2);                //PCLK1(APB1) = HCLK / 2= 24 MHz

        //PLL设置 SYSCLK: HSI / 2 * 12 = 4*12 = 48MHz
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
        //启动PLL
        RCC_PLLCmd(ENABLE);//如果PLL被用于系统时钟,不能被DISABLE
        //等待PLL稳定
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){;}

        //设置系统时钟SYSCLK = PLL输出
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        //等待PLL成功用作于系统时钟的时钟源,并等待稳定
        // 0x00:HSI作为系统时钟
        // 0x04:HSE作为系统时钟
        // 0x08:PLL作为系统时钟
        while(RCC_GetSYSCLKSource() != 0x08);
		
		SystemCoreClock = CPU_CLOCK;
    #else
        /* Enable HSE */
        RCC_HSEConfig(RCC_HSE_ON);

        /* Wait till HSE is ready */
        HSEStartUpStatus = RCC_WaitForHSEStartUp();

        if(HSEStartUpStatus == SUCCESS)
        {
                /* HCLK = SYSCLK */
                RCC_HCLKConfig(RCC_SYSCLK_Div1);
                /* PCLK2 = HCLK */
                RCC_PCLK2Config(RCC_HCLK_Div1);
                /* PCLK1 = HCLK/2 */
                RCC_PCLK1Config(RCC_HCLK_Div2);
                /* Flash 2 wait state */
                FLASH_SetLatency(FLASH_Latency_2);
                /* Enable Prefetch Buffer */
                FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
                /* PLLCLK = 8MHz * 9 = 72 MHz */
                RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
                /* Enable PLL */
                RCC_PLLCmd(ENABLE);
                /* Wait till PLL is ready */
                while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
                {
                }
                /* Select PLL as system clock source */
                RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
                /* Wait till PLL is used as system clock source */
                while(RCC_GetSYSCLKSource() != 0x08);
		  SystemCoreClock = CPU_CLOCK;
        }
   #endif

#else   // #ifdef USE_STD_LIB

     RCC_DeInit();
     STM32_RCC_HSICmd(ENABLE);  //打开内部高速时钟
     while(! (READ_REG_32_BIT(RCC->CR, RCC_CR_HSIRDY)));
     STM32_FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);    //开启FLASH预取指功能
     STM32_FLASH_SetLatency(FLASH_Latency_2);                          // FLASH时序控制, SYSCLK = 48~72MHz  Latency=2
     STM32_RCC_HCLKConfig(RCC_SYSCLK_Div1);                            // 设置HCLK(AHB时钟)=SYSCLK
     STM32_RCC_PCLK2Config(RCC_HCLK_Div1);                             // PCLK2(APB2) = HCLK
     STM32_RCC_PCLK1Config(RCC_HCLK_Div2);                             // PCLK1(APB1) = HCLK / 2= 24 MHz

     //PLL设置 SYSCLK: HSI / 2 * 12 = 4*12 = 48MHz
     STM32_RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
     STM32_RCC_PLLCmd(ENABLE); // 启动PLL, 如果PLL被用于系统时钟,不能被DISABLE

	 //等待PLL稳定
	 while( ! READ_REG_32_BIT(RCC->CR, RCC_CR_PLLRDY));
	 //设置系统时钟SYSCLK = PLL输出
     STM32_RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	 
	 // 等待PLL成功用作于系统时钟的时钟源,并等待稳定
       // 0x00:HSI作为系统时钟
       // 0x04:HSE作为系统时钟
       // 0x08:PLL作为系统时钟
	 while( ((uint8_t)READ_REG_32_BIT(RCC->CFGR, CFGR_SWS_Mask)) != 0x08);
	 SystemCoreClock = CPU_CLOCK;	 
#endif

}

void SysClockHSEConfig(void)
{
   #if (! HSIENABLE)
		ErrorStatus HSEStartUpStatus;
   #endif

        /* RCC system reset(for debug purpose) */
        RCC_DeInit();

    
        /* Enable HSE */
        RCC_HSEConfig(RCC_HSE_ON);

        /* Wait till HSE is ready */
        HSEStartUpStatus = RCC_WaitForHSEStartUp();

        if(HSEStartUpStatus == SUCCESS)
        {
                /* HCLK = SYSCLK */
                RCC_HCLKConfig(RCC_SYSCLK_Div1);
                /* PCLK2 = HCLK */
                RCC_PCLK2Config(RCC_HCLK_Div1);
                /* PCLK1 = HCLK/2 */
                RCC_PCLK1Config(RCC_HCLK_Div2);
                /* Flash 2 wait state */
                FLASH_SetLatency(FLASH_Latency_2);
                /* Enable Prefetch Buffer */
                FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
                /* PLLCLK = 8MHz * 9 = 72 MHz */
                RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
                /* Enable PLL */
                RCC_PLLCmd(ENABLE);
                /* Wait till PLL is ready */
                while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
                {
                }
                /* Select PLL as system clock source */
                RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
                /* Wait till PLL is used as system clock source */
                while(RCC_GetSYSCLKSource() != 0x08);
		  SystemCoreClock = CPU_CLOCK;
        }
}
void PowerCtrl_SetClock(void)
{
     RCC_DeInit();
     STM32_RCC_HSICmd(ENABLE);  //打开内部高速时钟
     while(! (READ_REG_32_BIT(RCC->CR, RCC_CR_HSIRDY)));
     STM32_FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);    //开启FLASH预取指功能
     STM32_FLASH_SetLatency(FLASH_Latency_2);                          // FLASH时序控制, SYSCLK = 48~72MHz  Latency=2
     STM32_RCC_HCLKConfig(RCC_SYSCLK_Div8);                            // 设置HCLK(AHB时钟)=SYSCLK
     STM32_RCC_PCLK2Config(RCC_HCLK_Div8);                             // PCLK2(APB2) = HCLK
     STM32_RCC_PCLK1Config(RCC_HCLK_Div8);                             // PCLK1(APB1) = HCLK / 2= 4 MHz

     //PLL设置 SYSCLK: HSI / 2 * 12 = 4*12 = 48MHz
     //STM32_RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
     //STM32_RCC_PLLCmd(ENABLE); // 启动PLL, 如果PLL被用于系统时钟,不能被DISABLE

	 //等待PLL稳定
	// while( ! READ_REG_32_BIT(RCC->CR, RCC_CR_PLLRDY));
	 //设置系统时钟SYSCLK = PLL输出
     STM32_RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	 
	 //等待PLL成功用作于系统时钟的时钟源,并等待稳定
     // 0x00:HSI作为系统时钟
     // 0x04:HSE作为系统时钟
     // 0x08:PLL作为系统时钟
	 while( ((uint8_t)READ_REG_32_BIT(RCC->CFGR, CFGR_SWS_Mask)) != 0x00);
	 SystemCoreClock = FREQ_1MHz;	 
}



// 外设时钟初始化
void RCC_PeriphInit(void)
{
       RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
		                      | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
       GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // 禁止JTAG
    //GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);  // 使能PD0 PD1

}







#define NVIC_SET_VECTOR_TABLE(NVIC_VectTab, Offset) (SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80))

/**
 * NVIC Configuration
 */
void NVIC_Configuration(void)
{

    #if SYS_USING_BOOTLOADER
    NVIC_SET_VECTOR_TABLE(NVIC_VectTab_FLASH, APP_VECTOR_OFFSET);
    #else
	 /* Set the Vector Table base location at 0x08000000 */
    NVIC_SET_VECTOR_TABLE(NVIC_VectTab_FLASH, 0x0);
	#endif
}




#include "gizwits_port.h"
#include "key_drv.h"

#include "sfud_demo.h"
#include "UsbApp.h"
#include "FatFs_Demo.h"
#include "ff.h"
#include "SDRR.h"
#include "os_global.h"
#include "F10X_Flash_If.h"

#include "TFT_API.h"
#include "QDTFT_Demo.h"
#include "TFT_Demo.h"
#include "UGUI_Demo.h"
#include "SnsGUI.h"
#include "ADC_Drv.h"
#include "sd_card_interface.h"

void print_info(char * info, u8 * file, u32 line)
{
    char string[] = " ";
	
    if(! info)info = string;
    printf("debug: %s %s:%d \r\n", info, file, line);
}

void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{       
 #if 0
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
    #else
    printf("%s \r\n", p);
    #endif
    
}




#include "sdcard_include.h"
#include "mass_mal.h"
#include "memory.h"
#include "usb_pwr.h"
#include "ExtiDrv.h"
//#include "uart_queue.h"
#include "debug_queue.h"
#include "fifo_queue.h"
#include "PCF8563.h"
#include "beep_drv.h"
#include "flash_font.h"
#include "PWM_TIM5.h"
#include "cfg_variable.h"

#if MODULE_GIZ_OTA_EN
extern void Pro_D2W_Ask_Module_Reboot(void);
#endif

uint8_t sys_run_mode = 0;   // 系统运行模式: 0: 正常; 1: 读取SD卡模式

uint8_t App_GetRunMode(void)
{
      return sys_run_mode;
}
void AppInit(void)
{
#if 1
       RCC_ClocksTypeDef rcc_clocks;
       GLOBAL_DISABLE_IRQ();
       NVIC_Configuration();
       SysTick_Init();
       RCC_PeriphInit();

       Sensor_PreInit();
       #if (PRINT_SELECT == UART_BLOCK)
       USART2_Init(9600);   //  usart 2 for debug output
       #else
       DebugQueue_Init();
       #endif
       
       Board_GpioInit();
      
       GLOBAL_ENABLE_IRQ();
       sd_initialize();
       usb_main();
       
       key_gpio_init();
       OS_GetCPUEndianness(); 
       
       PowerOffDectect_Init();
       PCF8563_Init();
       Beep_Init(); 
       if(KEY0_INPUT == 0)
       {
                int32_t res = 1;
                os_printf("detect 2 keys pressed, t = %ld\r\n", os_get_tick());
                sys_run_mode = 1;

                #if MODULE_GUI_EN
                UserGUI_Init();
                UGUI_Demo();
                SnsGUI_DisplaySDCardMode();
                #endif  

                res = sd_initialize();
                usb_main();
                while(!KEY_INPUT)
                {
                       if(res)
              	 {
              	        res = sd_initialize();
              	        if(res)
              	        {
                                    os_printf("SD Card Error!");				
              		        os_printf("Please Check! ");
              	        } 
              	 }
        	 }
                os_printf("2 keys released, t = %ld \r\n",  os_get_tick());
               
                while(1)
                {
                       if(flag_switch)
                      {
                            OS_TimerCheck();	
                      }
                }
       }
       else
       {
               os_printf("no detect 2 keys press, no usb init\r\n");
       }
       
#if MODULE_GUI_EN
       UserGUI_Init();
       UGUI_Demo();

       SnsGUI_DisplayVersion(SOFT_VERSION,  __DATE__, __TIME__);
       Beep_Ctrl();
       delay_ms(1000);
#endif

       FatFs_Demo();
       FILE_ReadConfig();
       ADCDrv_DMALaunch();
       Sensor_Init();
       ExtiDrv_Init();
       PWM_TIM5_Init();
       PWM_TIM5_SetDuty(50);
       RCC_GetClocksFreq(&rcc_clocks);  // read system clock config
       os_printf("SystemClock = %ld \r\n", SystemCoreClock);
       os_printf("SysClk = %d, HCLK = %d, PCLK1 = %d, PCLK2 = %d, ADCLK = %d MHz\r\n", 
	   	                 rcc_clocks.SYSCLK_Frequency / FREQ_1MHz,  rcc_clocks.HCLK_Frequency / FREQ_1MHz,
	   	                 rcc_clocks.PCLK1_Frequency / FREQ_1MHz,     rcc_clocks.PCLK2_Frequency / FREQ_1MHz,
	   	                 rcc_clocks.ADCCLK_Frequency / FREQ_1MHz);  
       os_printf("\r\n%s\r\n",   SOFT_VERSION);

        os_printf("sizeof(unsigned int) = %d\r\n",  sizeof(unsigned int));

        FlashFont_Task();
#if MODULE_SDRR_EN
        SDRR_Init();
#endif
        

#if MODULE_GUI_EN     
       SnsGUI_Demo();
       ADCDrv_DrawBatCapacity(0);
#endif  
       
#if GIZWITS_TYPE
       gizwits_user_init();  

       /* USER CODE END 2 */
       #if MODULE_GIZ_OTA_EN
	os_printf("MCU HAL Success , SoftVersion = %s\r\n",  GIZ_SOFT_VERSION);

	Pro_D2W_Ask_Module_Reboot();
	#endif
#endif
     
 
#else

   RCC_ClocksTypeDef rcc_clocks;
   
   
   GLOBAL_DISABLE_IRQ();
   NVIC_Configuration();
   SysClockHSEConfig();
  // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
   SysTick_Init();
   RCC_PeriphInit();

   #if (PRINT_SELECT == UART_BLOCK)
   //USART2_Init(9600);   //  usart 2 for debug output
   #else
   //DebugQueue_Init();
   #endif
   
   
   
   // 主板管脚初始化
   //Board_GpioInit();
   
   #if 1
    RCC_GetClocksFreq(&rcc_clocks);  // 读取系统时钟
    os_printf("SysClk = %d, HCLK = %d, PCLK1 = %d, PCLK2 = %d, ADCLK = %d MHz\r\n", rcc_clocks.SYSCLK_Frequency / FREQ_1MHz,
   	               rcc_clocks.HCLK_Frequency / FREQ_1MHz, rcc_clocks.PCLK1_Frequency / FREQ_1MHz, 
   	               rcc_clocks.PCLK2_Frequency / FREQ_1MHz, rcc_clocks.ADCCLK_Frequency / FREQ_1MHz);
    os_printf("SysCoreClk = %ld \r\n",  SystemCoreClock);
    os_printf("app version: %s %s %s\r\n", SOFT_VERSION, __DATE__, __TIME__);
   #else
    os_printf("%s %s %s\r\n", SOFT_VERSION, __DATE__, __TIME__);
   #endif
   
   
   #if MODULE_USB_EN   
   sd_initialize();
   usb_main();
   //FatFs_Demo();
  // SDRR_Init();
   #endif

   


#if MODULE_USB_EN
   //FILE_ReadConfig();
#endif
   

   GLOBAL_ENABLE_IRQ();
#endif
}

extern void CO2_StartBackgndCali(void);
void App_StartCO2BackgndCali(void)
{
       Beep(10,  5000);
	CO2_StartBackgndCali();
	SnsGUI_DisplayCO2Tip(TIP_CO2_CALI_ON);
}

