
#include "ExtiDrv.h"
#include "os_global.h"
#include "os_timer.h"
#include "board_version.h"
#include "BatteryLevel.h"
#include "PowerCtrl.h"
#include "FatFs_Demo.h"
#include "Application.h"
#include "RTCDrv.h"
#include "ADC_Drv.h"
#include "SnsGUI.h"
#include "cfg_variable.h"


#if EXTI_DEBUG_EN 
#define EXT_DEBUG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define EXT_DEBUG(...)
#endif

uint8_t battery_is_charging = 0; // 电池是否正在充电, 高4位表示: USB是否插入; 低 4位表示: 电池是否在充电

static os_timer_t tUSBDetectTimer;   // USB插入检测 外部中断延时检测定时器
static os_timer_t tTimerCapKeyTrigger;
static os_timer_t tTimerCapKeyTotal;

static volatile uint16_t vibration_cnt = 0;  // 震动次数
static os_timer_t tTimerTFTDispCntDown;
static uint16_t  tft_left_disp_sec; // TFT 屏幕剩余显示时间
static uint16_t sns_left_sec = 0;  // 传感器剩余运行时间


static void USBDetectTimer_CallBack(void * arg)
{
        if(VIN_DETECT_Read())		// 当前为高电平, 说明是上升沿
	{
		EXT_DEBUG("usb pluged\n");
		battery_is_charging |= USB_PLUGED_MASK;

	}
	else  // 当前是低电平, 说明是下降沿
	{
	      EXT_DEBUG("usb unpluged\n");
	      battery_is_charging = 0;
	}
}

extern void DbgInfo_FreeDisk(void);

static void TimerTFTDispCntDown_CallBack(void * arg)
{
         if(tft_left_disp_sec)
         {
               SnsGUI_DisplayTime(&calendar);
               tft_left_disp_sec--;
               if(tft_left_disp_sec == 0)
               {
                        // 关闭 TFT 屏幕
                        TFT_Ctrl(SW_CLOSE);
                        // 默认 5 min 后关闭传感器, 可在 config.txt 文件修改此值
                        sns_left_sec  = cfgVar_SnsRemainRunTime;  
                        EXT_DEBUG("close tft power, sec = %d,  sns_left = %d sec \r\n", 
                                                       os_get_tick() / 100,   sns_left_sec);
                        SET_REG_32_BIT(EXTI->IMR, EXTI_Line_CAPKEY);  // 使能外部中断
               }
               //DbgInfo_FreeDisk();
         }
         else
         {
                if(sns_left_sec < 0xFFFF)
                {
                      sns_left_sec--;
                      if(sns_left_sec == 0)
                      {
                             EXT_DEBUG("close sns power, sec = %d \r\n",  os_get_tick() / 100);
                             SNS_Ctrl_Set(SW_CLOSE);
                      }
                }
         }
         if(! CHG_STAT_Read())   // 低电平
         {
                 if(VIN_DETECT_Read())
                 {
                        battery_is_charging |= USB_CHARG_MASK;     
                 }
         }
         else
         {
                 if(VIN_DETECT_Read())
                 {
                       battery_is_charging = USB_PLUGED_MASK;
                 }
                 else
                 {
                       battery_is_charging = 0;
                 }
         }
         os_timer_arm(&tTimerTFTDispCntDown,   SEC(1),   0);
}

#include "TFT_API.h"
#include "QDTFT_Demo.h"
#include "TFT_Demo.h"
#include "UGUI_Demo.h"
#include "SnsGUI.h"



static void TimerCapKeyTrigger_CallBack(void * arg)
{
      EXT_DEBUG("capkey cnt = %d \r\n", vibration_cnt);
      if(vibration_cnt)
      {
            if(tft_left_disp_sec == 0)
            {
                    if(!SNS_Power_Is_Open())
                    {
                          #if 0 // tofix: 硬件问题: 重新打开传感器电源会导致重启
                          SNS_Ctrl_Set(SW_OPEN);
                          LCD_Ctrl_Set(SW_OPEN);
                          UserGUI_Init();
                          SnsGUI_DisplayNormal();
                          ADCDrv_DrawBatCapacity(1);
                          #else
                          JumpToBootloader();
                          #endif
                    }
                    LCD_BackLight_Ctrl_Set(SW_OPEN);
                    
            }
            tft_left_disp_sec = cfgVar_LcdBackLightSec;  // TFT 屏幕背光持续时间
            EXT_DEBUG("set tft disp sec = %d sec \r\n", tft_left_disp_sec);
      }
}

static void TimerCapKeyTotal_CallBack(void * arg)
{
         vibration_cnt = 0;
}


void USBDetect_Init(void)
{
        EXTI_InitTypeDef   EXTI_InitStructure;
        GPIO_InitTypeDef   GPIO_InitStructure;


	 os_timer_setfn(&tUSBDetectTimer,  USBDetectTimer_CallBack, NULL);
	 
	 // 使能 IO 时钟
	 VIN_DETECT_RCC_APBPeriphClockCmdEnable();
	 
	 GPIO_InitStructure.GPIO_Pin  = VIN_DETECT_Pin;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	 STM32_GPIO_Init(VIN_DETECT_PORT, &GPIO_InitStructure);
	 
	 /* Enable AFIO clock */
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   
	 /* Connect EXTIn 外部中断线到 IO 管脚  */
	 GPIO_EXTILineConfig(VIN_DETECT_PortSource,  VIN_DETECT_PinSource);
        STM32_EXTI_ClearITPendingBit(EXTI_Line_VinDetect);  // 清除中断标志位

	 
	 /* Configure EXTI0 line */
	 EXTI_InitStructure.EXTI_Line = EXTI_Line_VinDetect;
	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
	 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&EXTI_InitStructure);
   
	 /* Enable and set EXTI0 Interrupt to the lowest priority */
	 STM32_NVICInit(EXTI_VinDetect_IRQn, 4, 6, 0);	 // 第4组优先级, 4位抢占优先级, 0位响应优先级
}

void CapKey_Init(void)
{
       EXTI_InitTypeDef   EXTI_InitStructure;
       GPIO_InitTypeDef   GPIO_InitStructure;


	 os_timer_setfn(&tTimerCapKeyTrigger,      TimerCapKeyTrigger_CallBack,       NULL);
	 os_timer_setfn(&tTimerCapKeyTotal,           TimerCapKeyTotal_CallBack,             NULL);
        os_timer_setfn(&tTimerTFTDispCntDown, TimerTFTDispCntDown_CallBack,  NULL);
	 os_timer_arm(&tTimerTFTDispCntDown,   SEC(1),   0);

	 tft_left_disp_sec = cfgVar_FirstLcdBackLightSec;
	 
	 // 使能 IO 时钟
	 // add code here
	 
	 GPIO_InitStructure.GPIO_Pin  = CAPKEY_Pin;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	 STM32_GPIO_Init(CAPKEY_PORT, &GPIO_InitStructure);
	 
	 /* Enable AFIO clock */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   
	 /* Connect EXTIn 外部中断线到 IO 管脚  */
	 GPIO_EXTILineConfig(CAPKEY_PortSource ,  CAPKEY_PinSource );
        STM32_EXTI_ClearITPendingBit(EXTI_Line_CAPKEY);  // 清除中断标志位

	 
	 /* Configure EXTI0 line */
	 EXTI_InitStructure.EXTI_Line = EXTI_Line_CAPKEY;
	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
	 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&EXTI_InitStructure);
   
	 /* Enable and set EXTI0 Interrupt to the lowest priority */
	 STM32_NVICInit(EXTI_CAPKEY_IRQn, 4,  6, 0);	 // 第4组优先级, 4位抢占优先级, 0 位响应优先级
}


os_timer_t tTimerPwrOffDetTrigger;
volatile uint8_t shutoff_dbg_cnt = 0;
void TimerPwrOffDetTrigger_CallBack(void * arg)
{
      static uint16_t  wait_cnt = 0;
       EXT_DEBUG("pwr off det = %d \r\n", PwrOffDet_Read());
       if(wait_cnt == 0)
       {
                 // 关闭 TFT 屏幕
                 LCD_BackLight_Ctrl_Set(SW_CLOSE);
                 SNS_Ctrl_Set(SW_CLOSE);
                 LCD_Ctrl_Set(SW_CLOSE);
                 EXT_DEBUG("close tft & sns power, tick = %ld\r\n", 
                                  os_get_tick() / 100);
       }
       if(FILE_GetLock())
       {
             wait_cnt++;
             shutoff_dbg_cnt ++;
             EXT_DEBUG("power wait %d to shut off \r\n",  shutoff_dbg_cnt);
             os_timer_arm(&tTimerPwrOffDetTrigger, 20,  0);
       }
       else
       {
              #if EXTI_DEBUG_EN
              FRESULT res0, res1;
              #endif
              
              EXT_DEBUG("ready to shutoff system, wait_cnt = %d \r\n", shutoff_dbg_cnt);
              EXT_DEBUG("shut off record end = %ld\r\n", os_get_tick());

              #if EXTI_DEBUG_EN
              res0 = 
              #endif
              f_mount(NULL,  "0:", 1);

              #if EXTI_DEBUG_EN
              res1 = 
              #endif
              f_mount(NULL,  "1:", 1);
              EXT_DEBUG("umount 0: %d; 1: %d \r\n",  res0, res1);
              delay_ms(100);
              RT_EN_Close();
       }
}

void PowerOffDectect_Init(void)
{
       EXTI_InitTypeDef   EXTI_InitStructure;
       GPIO_InitTypeDef   GPIO_InitStructure;


	 os_timer_setfn(&tTimerPwrOffDetTrigger,      TimerPwrOffDetTrigger_CallBack,       NULL);

	 
	 // 使能 IO 时钟
	 // add code here
	 
	 GPIO_InitStructure.GPIO_Pin  = PwrOffDet_Pin;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	 STM32_GPIO_Init(PwrOffDet_PORT, &GPIO_InitStructure);
	 
	 /* Enable AFIO clock */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   
	 /* Connect EXTIn 外部中断线到 IO 管脚  */
	 GPIO_EXTILineConfig(PwrOffDet_PortSource ,  PwrOffDet_PinSource );
        STM32_EXTI_ClearITPendingBit(EXTI_Line_PwrOffDet);  // 清除中断标志位

	 
	 /* Configure EXTI0 line */
	 EXTI_InitStructure.EXTI_Line = EXTI_Line_PwrOffDet;
	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&EXTI_InitStructure);
   
	 /* Enable and set EXTI0 Interrupt to the lowest priority */
	 STM32_NVICInit(EXTI_PwrOffDet_IRQn, 4,  0, 0);	 // 第4组优先级, 4位抢占优先级, 0 位响应优先级
}


void ExtiDrv_Init(void)
{
       //USBDetect_Init();   // 检测 USB 是否插入, 中断初始化
       if(! App_GetRunMode())
       {
              CapKey_Init();         //  振动检测 中断初始化
        }
}


void EXTI_CAPKEY_IRQHandler(void)
{

    CLEAR_REG_32_BIT(EXTI->IMR, EXTI_Line_CAPKEY);  // 禁止外部中断
    if(READ_REG_32_BIT(EXTI->PR, EXTI_Line_CAPKEY))  // PB0 管脚的中断
    {
		EXTI->PR = EXTI_Line_CAPKEY;   // 往挂起位写 1 清中断标志
		vibration_cnt++;
		os_timer_arm(&tTimerCapKeyTrigger,  2, 0);  // 延时 10 ms  
              if(vibration_cnt )
              {
                    os_timer_arm(&tTimerCapKeyTotal,  SEC(3),  0);
              }

                       
    }
    SET_REG_32_BIT(EXTI->IMR, EXTI_Line_CAPKEY);
}


//外部中断服务程序 
void EXTI15_10_IRQHandler(void)
{
    CLEAR_REG_32_BIT(EXTI->IMR, EXTI_Line_VinDetect);  // 禁止外部中断
    CLEAR_REG_32_BIT(EXTI->IMR, EXTI_Line_PwrOffDet);  // 禁止外部中断
    
    if(READ_REG_32_BIT(EXTI->PR, EXTI_Line_VinDetect))  // PA7 管脚的中断
    {
		#if 0
		EXTI_ClearITPendingBit(EXTI_Line_VinDetect); //清除LINE0上的中断标志位  
		#else
              EXTI->PR = EXTI_Line_VinDetect; // 往挂起位写 1 清中断标志
		#endif

		os_timer_arm(&tUSBDetectTimer, 1, 0);  // 延时 10 ms 检测
		SET_REG_32_BIT(EXTI->IMR, EXTI_Line_VinDetect);  // 使能外部中断
    }
    
    
    if(READ_REG_32_BIT(EXTI->PR, EXTI_Line_PwrOffDet))  // PC14 管脚的中断
    {
               EXTI->PR = EXTI_Line_PwrOffDet; // 往挂起位写 1 清中断标志

               if(FILE_GetLock())
               {
                      shutoff_dbg_cnt++;
                      EXT_DEBUG("shut off record begin = %ld\r\n", os_get_tick());
                      os_timer_arm(&tTimerPwrOffDetTrigger, 10, 0);  // 延时 100 ms 检测
               }
               else
               {
                     #if EXTI_DEBUG_EN
                     FRESULT res0, res1;
                     #endif
                     
                      GLOBAL_DISABLE_IRQ();
                      // 关闭 TFT 屏幕
                     TFT_Ctrl(SW_CLOSE);
                      SNS_Ctrl_Set(SW_CLOSE);
                      EXT_DEBUG("close tft & sns power immediately\r\n");
                      EXT_DEBUG("ready to shutoff system\r\n");

                      #if EXTI_DEBUG_EN
                      res0 = 
                      #endif
                      f_mount(NULL,  "0:",  1);

                      #if EXTI_DEBUG_EN
                      res1 = 
                      #endif
                      f_mount(NULL,  "1:",  1);
                      
                      EXT_DEBUG("umount 0: %d; 1: %d \r\n",  res0, res1);
                      RT_EN_Close();
               }
		 
		 SET_REG_32_BIT(EXTI->IMR, EXTI_Line_PwrOffDet);  // 使能外部中断
    }
}

