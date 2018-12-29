

#include "gizwits_port.h"
#include "gizwits_protocol.h"
#include "gizwits_product.h"
#include "os_global.h"
#include "board_version.h"
#include "gizwits_uart.h"
#include "uart_drv.h"

#include "Sensor.h"
#include "SDRR.h"
#include "RTCDrv.h"
#include "BatteryLevel.h"
#include "Fatfs_Demo.h"
#include "delay.h"
#include "PCF8563.h"
#include "SnsGUI.h"
#include "TFT_API.h"
#include "ADC_Drv.h"

#include  "wifi_queue.h"

#include "os_timer.h"
#include "SnsGUI.h"
#include "Application.h"

/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void userInit(void)
{
        os_memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
    
      currentDataPoint.valueRelay_Ctrl = 0;
      currentDataPoint.valueLED0_Ctrl = 0;
      currentDataPoint.valueLCD_Power_En = LCD_Power_En_Status();
      currentDataPoint.valueSns_Power_En  = SNS_Power_Is_Open();
      currentDataPoint.valueWifi_Power_En = WIFI_Power_Ctrl_Read();
      currentDataPoint.valuepower_mode = 1;
      currentDataPoint.valuelang_select = 0;
      currentDataPoint.valuesns_cal = 0;
      currentDataPoint.valuetime_stamp = RTCDrv_CalendarToSec(1970, &calendar);;
      currentDataPoint.valuehumi = tSDRR.humi;
      currentDataPoint.valuelight_level = 0;
      currentDataPoint.valuebattery_percent = BatLev_GetPercent();
      currentDataPoint.valuecharge_state = BatteryIsCharging();
      currentDataPoint.valueCO2 = tSDRR.co2_ppm;
      currentDataPoint.valuehcho_ppb = tSDRR.hcho_ppb;
      currentDataPoint.valuetvoc_ppb = tSDRR.tvoc_ppb;
      currentDataPoint.valuetemp = tSDRR.temp / 10;
      currentDataPoint.valuepm10_ug = tSDRR.pm10_ug;
      currentDataPoint.valuepm2p5_ug = tSDRR.pm2p5_ug;
      currentDataPoint.valueco_ppm = tSDRR.co_ppm;
      currentDataPoint.valuePtCnt_0p3um = tSDRR.pm0p3_um;
      currentDataPoint.valuePtCnt_10p0um = tSDRR.pm10_um;
      currentDataPoint.valueflash_free   = diskInfo[FLASH_DISK].free;
      currentDataPoint.valueflash_total = diskInfo[FLASH_DISK].total;
      currentDataPoint.valuesd_free = diskInfo[SD_DISK].free;
      currentDataPoint.valuesd_total = diskInfo[SD_DISK].total;
       currentDataPoint.valuecore_temp = tSDRR.core_temp;
}




/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void userHandle(void)
{

    currentDataPoint.valuehumi =   tSDRR.humi;//Add Sensor Data Collection
    currentDataPoint.valuelight_level = 0;//Add Sensor Data Collection
    currentDataPoint.valuebattery_percent = BatLev_GetPercent();//Add Sensor Data Collection
    currentDataPoint.valuecharge_state = BatteryIsCharging();//Add Sensor Data Collection
    currentDataPoint.valueCO2 = tSDRR.co2_ppm;//Add Sensor Data Collection
    currentDataPoint.valuehcho_ppb = tSDRR.hcho_ppb;//Add Sensor Data Collection
    currentDataPoint.valuetvoc_ppb = tSDRR.tvoc_ppb;//Add Sensor Data Collection
    currentDataPoint.valuetemp = tSDRR.temp / 10;//Add Sensor Data Collection
    currentDataPoint.valuepm10_ug = tSDRR.pm10_ug;//Add Sensor Data Collection
    currentDataPoint.valuepm2p5_ug = tSDRR.pm2p5_ug;//Add Sensor Data Collection
    currentDataPoint.valueco_ppm = tSDRR.co_ppm;//Add Sensor Data Collection
    currentDataPoint.valuePtCnt_0p3um = tSDRR.pm0p3_um;//Add Sensor Data Collection
    currentDataPoint.valuePtCnt_10p0um = tSDRR.pm10_um;//Add Sensor Data Collection
    currentDataPoint.valueflash_free   = diskInfo[FLASH_DISK].free;//Add Sensor Data Collection
    currentDataPoint.valueflash_total = diskInfo[FLASH_DISK].total;//Add Sensor Data Collection
    currentDataPoint.valuesd_free         = diskInfo[SD_DISK].free;    // Add Sensor Data Collection
    currentDataPoint.valuesd_total        = diskInfo[SD_DISK].total;   // Add Sensor Data Collection
    currentDataPoint.valuecore_temp = tSDRR.core_temp;//Add Sensor Data Collection

    //XXX is Extend Datapoint Address ,User defined
    os_strncpy((uint8_t *)currentDataPoint.valuefault_info, "None",   sizeof(currentDataPoint.valuefault_info));
    //XXX is Extend Datapoint Address ,User defined
    os_strncpy((uint8_t *)currentDataPoint.valuesoft_version,  SOFT_VERSION, sizeof(currentDataPoint.valuesoft_version));
    //XXX is Extend Datapoint Address ,User defined
    os_strncpy((uint8_t *)currentDataPoint.valuewifi_version, "WIFI 0.1",sizeof(currentDataPoint.valuewifi_version));
    //XXX is Extend Datapoint Address ,User defined
    os_strncpy((uint8_t *)currentDataPoint.valuepcb_version,  PCB_VERSION,sizeof(currentDataPoint.valuepcb_version));
    //XXX is Extend Datapoint Address ,User defined
    os_strncpy((uint8_t *)currentDataPoint.valuesn, "none",sizeof(currentDataPoint.valuesn));
    
         

}

/**
* @brief mcuRestart

* MCU Reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
      GIZWITS_LOG("gizwits module ready to reboot mcu ! \r\n");
      // close all interrupt
      GLOBAL_DISABLE_IRQ();
     // __set_FAULTMASK(1);

      NVIC_SystemReset();
}


os_timer_t tTimerWiFiRoutine;

//static uint8_t enter_wifi_gui = 0;   // 是否进入 WIFI 显示界面
static uint8_t wifi_connect_sta = 0xFF;  // WIFI 连接状态: 0: 已连接;  1: 未连接; 2: No Resp
static uint8_t recv_wifi_ip = 0;         // 是否接收到WIFI IP:  1: 接受到
static uint16_t recv_timeout_cnt = 0;   // 接收超时计数，  超过 40 sec, WIFI 无响应, 则任认为断网


// 设置网络连接状态
void  WIFI_SetConnectStatus(uint8_t conn_sta)
{
      wifi_connect_sta = conn_sta;
}


// 获取网络连接状态
uint8_t WIFI_GetConnectStatus(void)
{
       return wifi_connect_sta;
}

// 设置接收到IP 状态
void WIFI_SetIPRecvStatus(uint8_t recv_ip)
{
     recv_wifi_ip = recv_ip;
     recv_timeout_cnt = 0;
}

//static 
void TimerWiFiRoutine_CallBack(void * arg)
{
      static uint8_t cnt = 0;
      
      cnt++;
      if(cnt > 10)
      {
               cnt = 0;
                recv_timeout_cnt++;
               if( recv_timeout_cnt % 30 == 0)  // 每 20 sec 查询一次
               {
                      GIZWITS_LOG("query wifi  sec = %ld \r\n",  os_get_tick() / 100);
                      gizwitsGetModuleInfo();
               }
               if(recv_timeout_cnt > 60)
               {
                        GIZWITS_LOG("WIFI Disconnect\r\n");
                        GIZWITS_LOG("WiFi No Resp\r\n");
                        
                        SnsGUI_DisplayWifiTip(TIP_WIFI_UnConn);
                        
                        recv_timeout_cnt = 0;
               }
               if(recv_wifi_ip == 0xFF)
              {
                     GIZWITS_LOG("query ip  sec = %ld \r\n",  os_get_tick() / 100);
                      gizwitsGetModuleInfo();
              }
      }
}

// gizwits init
void gizwits_user_init(void)
{
        //WIFI_Power_Ctrl_Open();
#if GIZWITS_TYPE == GIZ_MCU

       #if (GIZ_UART_SEL == UART_BLOCK)
       WIFI_USART_Init(9600);          //  usart for mcu <-> wifi
       #else
       WifiQueue_Init();
       #endif
       
       userInit();
       gizwitsInit();
       os_timer_setfn(&tTimerWiFiRoutine,   TimerWiFiRoutine_CallBack,  NULL);
       os_timer_arm(&tTimerWiFiRoutine,  10,  0);
       
#elif GIZWITS_TYPE == GIZ_SOC
       WIFI_USART_Init(115200);
       gizwits_uart_init();
#endif
}

// 主循环任务
void gizwits_user_task(void)
{
#if GIZWITS_TYPE == GIZ_MCU
    static uint8_t cnt = 0;

     cnt++;
     if(cnt >= 15)
     {
           cnt = 0;
           userHandle();
     }
      gizwitsHandle((dataPoint_t *)&currentDataPoint);
#elif GIZWITS_TYPE == GIZ_SOC

#endif
}

// cloud command mcu to do some process to ctrl
void gizwits_user_event_process( dataPoint_t * dataPointPtr,  uint8_t event)
{
       if(dataPointPtr == NULL)
       {
                GIZWITS_LOG("dataPointPtr is null \r\n");
                return;
       }
       
       switch(event)
       {
               case EVENT_Relay_Ctrl:
                currentDataPoint.valueRelay_Ctrl = dataPointPtr->valueRelay_Ctrl;
                GIZWITS_LOG("Evt: EVENT_Relay_Ctrl %d \n", currentDataPoint.valueRelay_Ctrl);
                if(0x01 == currentDataPoint.valueRelay_Ctrl)
                {
                }
                else
                {
                }
                break;
              case EVENT_LED0_Ctrl:
                currentDataPoint.valueLED0_Ctrl = dataPointPtr->valueLED0_Ctrl;
                GIZWITS_LOG("Evt: EVENT_LED0_Ctrl %d \n", currentDataPoint.valueLED0_Ctrl);
                if(0x01 == currentDataPoint.valueLED0_Ctrl)
                {
                       LCD_BackLight_Ctrl_Set(SW_OPEN);
                }
                else
                {
                       LCD_BackLight_Ctrl_Set(SW_CLOSE);
                }
                break;
              case EVENT_LCD_Power_En:
                currentDataPoint.valueLCD_Power_En = dataPointPtr->valueLCD_Power_En;
                GIZWITS_LOG("Evt: EVENT_LCD_Power_En %d \n", currentDataPoint.valueLCD_Power_En);
                if(0x01 == currentDataPoint.valueLCD_Power_En)
                {
                           TFT_Ctrl(SW_OPEN);

                           UserGUI_Init();
                           SnsGUI_DisplayNormal();
                           ADCDrv_DrawBatCapacity(1);
                }
                else
                {
                         TFT_Ctrl(SW_CLOSE);
                }
                break;
              case EVENT_Sns_Power_En:
                currentDataPoint.valueSns_Power_En = dataPointPtr->valueSns_Power_En;
                GIZWITS_LOG("Evt: EVENT_Sns_Power_En %d \n", currentDataPoint.valueSns_Power_En);
                if(0x01 == currentDataPoint.valueSns_Power_En)
                {
                         SNS_Ctrl_Set(SW_OPEN);
                }
                else
                {
                        SNS_Ctrl_Set(SW_CLOSE);
                }
                break;
              case EVENT_Wifi_Power_En:
                currentDataPoint.valueWifi_Power_En = dataPointPtr->valueWifi_Power_En;
                GIZWITS_LOG("Evt: EVENT_Wifi_Power_En %d \n", currentDataPoint.valueWifi_Power_En);
                if(0x01 == currentDataPoint.valueWifi_Power_En)
                {  
                }
                else
                {
                }
                break;


              case EVENT_power_mode:
                currentDataPoint.valuepower_mode = dataPointPtr->valuepower_mode;
                GIZWITS_LOG("Evt:EVENT_power_mode %d\n",currentDataPoint.valuepower_mode);
                //user handle
                break;
              case EVENT_lang_select:
                currentDataPoint.valuelang_select = dataPointPtr->valuelang_select;
                GIZWITS_LOG("Evt:EVENT_lang_select %d\n",currentDataPoint.valuelang_select);
                //user handle
                break;
              case EVENT_sns_cal:
                currentDataPoint.valuesns_cal = dataPointPtr->valuesns_cal;
                GIZWITS_LOG("Evt:EVENT_sns_cal %d\n",currentDataPoint.valuesns_cal);
                //user handle
                break;
              case EVENT_time_stamp:
                currentDataPoint.valuetime_stamp = dataPointPtr->valuetime_stamp;
                GIZWITS_LOG("Evt:EVENT_time_stamp %d\n",currentDataPoint.valuetime_stamp);
                {
                           T_Calendar_Obj cal;

                          RTCDrv_SecToCalendar(1970,  currentDataPoint.valuetime_stamp, &cal, 1);
                          PCF8563_SetTime(cal.hour, cal.min,  cal.sec);
		            cal.week = RTCDrv_GetWeek(cal.year,  cal.month,  cal.day);
		            GIZWITS_LOG("set time: %02d-%02d-%02d \r\n", cal.hour, cal.min, cal.sec);
		            GIZWITS_LOG("set date: %04d-%02d-%02d %d \r\n", cal.year, cal.month, cal.day, cal.week);
		            PCF8563_SetDate(cal.year - 2000,  cal.month,  cal.day,  cal.week);
                }
                break;
        }
}




#include "key_drv.h"
#include "beep_drv.h"
#include "SnsGUI.h"
#include "PWM_TIM5.h"
extern void CO2_StartBackgndCali(void);

static os_timer_t tTimerEraseWifiTip;
//static uint8_t wifi_tip_type = 0;
static void TimerEraseWifiTip(void * arg)
{
      SnsGUI_DisplayWifiTip(TIP_WIFI_Erase);
}

void key0_hook(uint8_t key_state)
{
     switch(key_state)
	{
	        case L_key:  
		{
			KEY_DEBUG("L\n");
			KEY_DEBUG("enter WiFi AirLink connect mode \r\n");
                     Beep(30,  2);
			SnsGUI_DisplayWifiTip(TIP_WIFI_TryCon);
			os_timer_setfn(&tTimerEraseWifiTip, TimerEraseWifiTip,  NULL);
                     os_timer_arm(&tTimerEraseWifiTip,  SEC(10),  0);
			 //wifi_tip_type = TIP_WIFI_TryCon;
			 
#if GIZWITS_TYPE == GIZ_MCU			
			gizwitsSetMode(WIFI_AIRLINK_MODE);
#elif GIZWITS_TYPE == GIZ_SOC
                     gizwits_set_wifi_mode(WIFI_AIRLINK_MODE);
#endif			
		}break;
		case D_key:
		{
			KEY_DEBUG("D\n");	
		}break;
		case S_key:
		{
		       #if 0
		       static uint16_t duty = 5;
		       
		       Beep(20,  200);
		       duty++;
		       if(duty > 10)duty = 0;
		       
		       PWM_TIM5_SetDuty(duty * 100);
			KEY_DEBUG("S\n");
			#else
                     Beep(10,  5000);
			CO2_StartBackgndCali();
			SnsGUI_DisplayCO2Tip(TIP_CO2_CALI_ON);
			#endif
		}break;
	}
}

void key1_hook(uint8_t key_state)
{
     switch(key_state)
	{
	        case L_key:  
		{
			KEY_DEBUG("L\n");
			Beep(30,  2);
			KEY_DEBUG("wifi reset, please reconfig wifi account and pawd\r\n");
			
			SnsGUI_DisplayWifiTip(TIP_WIFI_Reset);
			os_timer_setfn(&tTimerEraseWifiTip, TimerEraseWifiTip,  NULL);
                     os_timer_arm(&tTimerEraseWifiTip,  SEC(10),  0);
			 //wifi_tip_type = TIP_WIFI_Reset;
#if GIZWITS_TYPE == GIZ_MCU	
			gizwitsSetMode(WIFI_RESET_MODE);
#elif GIZWITS_TYPE == GIZ_SOC
                     gizwits_set_wifi_mode(WIFI_RESET_MODE);
#endif
		}break;
		case D_key:
		{
		      KEY_DEBUG("D\n");
		      //Beep(50,  1000); 
		}break;
		case S_key:
		{
			KEY_DEBUG("S\n");
			//Beep(200,  200);
		}break;
	}
}

/*
void TIM4_IRQHandler(void)  
{
        if(READ_REG_32_BIT(TIM4->SR,  TIM_IT_Update))
	{
              CLEAR_REG_32_BIT(TIM4->SR,  TIM_IT_Update );
	       timerMsCount++;
	}
}
*/

char product_key_buf[32];
char product_secret_buf[32];
char * PRODUCT_KEY_STRING(void)
{
      return product_key_buf;
}

char *PRODUCT_SECRET_STRING(void)
{
      return product_secret_buf;
}
