
#include "PCF8563.h"

#include "IIC_Drv.h"
#include "os_timer.h"
#include "os_global.h"
#include "sensor.h"
#include "delay.h"
#include "RTCDrv.h"
#include "SnsGUI.h"

#if PCF8563_DEBUG_EN
#define PCF_DBG(fmt, ...)    os_printf(fmt, ##__VA_ARGS__)
#else
#define PCF_DBG(...)
#endif




IIC_BUS pcf8563_bus = 
{
        { GPIO_Pin_4,  GPIOA  },    // SDA
        { GPIO_Pin_5,  GPIOA  },    // SCL
};



SYS_RESULT PCF8563_IIC_Write(uint8_t reg, uint8_t * data, uint8_t size)
{
       SYS_RESULT res;

       res = IIC_WriteNByte(&pcf8563_bus, 0xA2, reg, data, size);
       return res;
}

SYS_RESULT PCF8563_IIC_Read(uint8_t reg,  uint8_t * buf, uint8_t size)
{  
       SYS_RESULT res;

       res = IIC_ReadNByteExt(&pcf8563_bus, 0xA2, reg, buf, size, 1);
       return res;
}

SYS_RESULT PCF8563_SetReg(uint8_t reg, uint8_t data)
{
        return PCF8563_IIC_Write(reg, &data, 1);
}

SYS_RESULT PCF8563_GetReg(uint8_t reg, uint8_t * data)
{
      return PCF8563_IIC_Read(reg, data, 1);
}

// BCD 转换为 hex值
uint8_t BCD2HEX(uint8_t bcd)
{
        uint8_t hex = 0;
        
        hex =  (bcd >> 4) * 10 + (bcd & 0x0F);
        return hex;
}

uint8_t HEX2BCD(uint8_t hex)
{
        uint8_t bcd = 0;

        bcd = ((hex / 10) << 4) | (hex % 10);

        return bcd;
}

os_timer_t tTimerPCF8563;
T_RTC_TIME rtc_time;

extern uint8_t App_GetRunMode(void);
void TimerPCF8563_CallBack(void * arg)
{
          #if 1
           T_RTC_TIME * t = &rtc_time;
           
           if(PCF8563_GetTime(t) == SYS_SUCCESS)
           {  
                  if((os_get_tick() / 100) % 20 == 0)
                  {
                          PCF_DBG("%02d-%02d-%02d %02d:%02d:%02d %d \r\n",
                                    t->year, t->month, t->day, t->hour, t->min, t->sec,  t->week);
                   }              
                   calendar.year = t->year + 2000;
                   calendar.month = t->month;
                   calendar.day = t->day;
                   calendar.hour = t->hour;
                   calendar.min = t->min;
                   calendar.sec  = t->sec;

                   if(App_GetRunMode())
                   {
                           SnsGUI_DisplayTime(&calendar);
                   }
            }

            #else
          

            calendar.sec++;
            if(calendar.sec  > 59)
            {
                   calendar.sec = 0;
                   calendar.min++;
                   if(calendar.min > 59)
                   {
                          calendar.min = 0;
                          calendar.hour++;
                          if(calendar.hour > 23)
                          {
                                calendar.hour = 0;
                                calendar.day++;
                                if(calendar.day > mon_table[calendar.month - 1])
                                {
                                       calendar.day = 1;
                                       calendar.month++;
                                       if(calendar.month > 12)
                                       {
                                               calendar.month = 1;
                                               calendar.year++;
                                       }
                                }
                          }
                    }
            }
            #endif
            
           os_timer_arm(&tTimerPCF8563, SEC(1),  0);
}
void PCF8563_Set(T_RTC_TIME * time)
{
        PCF8563_SetReg(0, 0x28);
        PCF8563_SetReg(PCF_SEC_REG,    HEX2BCD(time->sec    & 0x7F));
        PCF8563_SetReg(PCF_MIN_REG,    HEX2BCD(time->min   & 0x7F));
        PCF8563_SetReg(PCF_HOUR_REG, HEX2BCD(time->hour & 0x3F));
        PCF8563_SetReg(PCF_DAY_REG,     HEX2BCD(time->day & 0x3F));
        PCF8563_SetReg(PCF_WEEK_REG, HEX2BCD(time->week & 0x07));
        PCF8563_SetReg(PCF_MON_REG,    HEX2BCD(time->month & 0x1F));
        PCF8563_SetReg(PCF_YEAR_REG,   HEX2BCD(time->year) );
        PCF8563_SetReg(0, 0x08);
}
void PCF8563_SetTime(uint8_t hour, uint8_t min, uint8_t sec)
{
        PCF8563_SetReg(0, 0x28);
        PCF8563_SetReg(PCF_SEC_REG,    HEX2BCD(sec    & 0x7F));
        PCF8563_SetReg(PCF_MIN_REG,    HEX2BCD(min   & 0x7F));
        PCF8563_SetReg(PCF_HOUR_REG, HEX2BCD(hour & 0x3F));
        PCF8563_SetReg(0, 0x08);
}
void PCF8563_SetDate(uint8_t year, uint8_t month, uint8_t day, uint8_t weekday)
{
        PCF8563_SetReg(0, 0x28);
        PCF8563_SetReg(PCF_DAY_REG,     HEX2BCD(day & 0x3F));
        PCF8563_SetReg(PCF_WEEK_REG, HEX2BCD(weekday & 0x07));
        PCF8563_SetReg(PCF_MON_REG,    HEX2BCD(month & 0x1F));
        PCF8563_SetReg(PCF_YEAR_REG,   HEX2BCD(year ));
        PCF8563_SetReg(0, 0x08);
}

SYS_RESULT PCF8563_GetTime(T_RTC_TIME * t)
{
       uint8_t buf[7];
       SYS_RESULT res;
       
       os_memset(buf, 0, sizeof(buf));
       res = PCF8563_IIC_Read(PCF_SEC_REG,  buf, sizeof(buf)); // 连续读 7 个字节
       if(! res)
       {
                PCF_DBG("read PCF8563 success \r\n");
                t->sec         = BCD2HEX(buf[0] & 0x7F);
                t->min       = BCD2HEX(buf[1] & 0x7F);
                t->hour     = BCD2HEX(buf[2] & 0x3F);
                t->day        = BCD2HEX(buf[3] & 0x3F);
                t->week    = BCD2HEX(buf[4] & 0x07);
                t->month = BCD2HEX(buf[5] & 0x1F);
                t->year      = BCD2HEX(buf[6]);

                #if 0
                do
                {
                        uint8_t i;
                        PCF_DBG("PCF8563 : \r\n");
                        for(i = 0; i < sizeof(buf); i++)
                        {
                                PCF_DBG("reg %d = %x \r\n",  i + PCF_SEC_REG,  buf[i]);
                        }
                        PCF8563_IIC_Read(0, buf, 1);
                        PCF_DBG("reg 0 = 0x%x \r\n",   buf[0]);
                }while(0);
                #endif
       }
       else
       {
              PCF_DBG("read PCF8563 failed \r\n");
       }
       return res;
}


void  PCF8563_Init(void)
{
        IIC_Init(&pcf8563_bus);

        #if 0
        //PCF8563_SetDate(18, 9, 19, 3);
        //delay_ms(100);
        //PCF8563_SetTime(23, 55, 30);
        #else
        calendar.year = 2018;
        calendar.month = 9;
        calendar.day = 1;
        calendar.hour = 23;
        calendar.min   = 58;
        calendar.sec    = 30;
        #endif
        
        os_timer_setfn(&tTimerPCF8563,  TimerPCF8563_CallBack,  NULL);
        os_timer_arm(&tTimerPCF8563,  0,  0);
}
