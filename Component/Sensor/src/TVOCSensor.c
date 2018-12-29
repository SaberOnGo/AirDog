
#include "sensor.h"

#include "IIC_Drv.h"
#include "os_timer.h"
#include "SnsGUI.h"

#if TVOC_DBG_EN
#define TVOC_DBG(fmt, ...)    os_printf(fmt, ##__VA_ARGS__)
#else
#define TVOC_DBG(...)
#endif




IIC_BUS tvoc_bus = 
{
        { GPIO_Pin_4,  GPIOA  },    // SDA
        { GPIO_Pin_5,  GPIOA  },    // SCL
};

SYS_RESULT ZM01_Read(uint8_t * data)
{
        SYS_RESULT  res;
        
        res =  IIC_ReadNByteExt(&tvoc_bus,  0x55, 0xAA,  data,  1,  1);
        TVOC_DBG("buf: 0x%02x\r\n",   *data);
        return res;
}

// 5 ppm 量程, 值范围: 1 - 200
// ppb = x * 5000 / 200 = x * 25
#define  tvoc_val_2_ppb(v)   ( (uint16_t)v * 25)

static void TVOC_GUI_Display(uint16_t ppb)
{
     //  if(tvoc_ppm == 0xFFFF)global_tvoc_ppb = 0xFFFF;
	//else global_tvoc_ppb = tvoc_ppm * 100;
	
	// 显示
    do
    {
             static u8 first_time = 1;
	      static u32 last_sec  = 0;
		u32 sec = os_get_tick() / 100;

	    if(sec < 180)  // 小于 180 s
	    {
	          sec = 180 - sec;
	    }
	    else sec = 0;
		
           if(ppb != 0xFFFF)
          {
          
                if(first_time)
               {
                      first_time = 0;
		        SnsGUI_ClearCircle(CIRCLE_TVOC);
                }  
		  SnsGUI_DisplayTVOC(ppb );
            }
	     else
	    {
		    if(last_sec != sec)
		    {
			   last_sec = sec;
			   // 显示秒数
			   SnsGUI_TVOCWait(sec);
		    }
		    TVOC_DBG("wait tvoc: left sec = %ld\n", sec);
           }
     }
     while(0);
}

static os_timer_t tTimerZM01;
static void TimerZM01_CallBack(void  * arg)
{
        uint8_t val = 0;
        uint16_t ppb = 0;
        
        if(ZM01_Read(&val) == SYS_SUCCESS)
        {

                 ppb = tvoc_val_2_ppb(val);
                TVOC_DBG("read ZM01 success: tvoc = %d ppb \r\n",  ppb);
                TVOC_GUI_Display(ppb);
                SDRR_SaveSensorPoint(SENSOR_TVOC,  &ppb);
        }
        else
        {
                TVOC_DBG("read ZM01 err\r\n");
        }
        os_timer_arm(&tTimerZM01,  SEC(1),  0);
}

void TVOC_Sensor_Init(void)
{
        IIC_Init(&tvoc_bus);
        os_timer_setfn(&tTimerZM01,  TimerZM01_CallBack,  NULL);
        os_timer_arm(&tTimerZM01,   SEC(1),  0);
}





