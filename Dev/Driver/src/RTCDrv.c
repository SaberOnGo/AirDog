
#include "RTCDrv.h"







#if RTC_DEBUG_EN
#define RTC_DEBUG(fmt, ...)  printf(fmt, ##__VA_ARGS__) 
#else
#define RTC_DEBUG(...)
#endif


T_Calendar_Obj calendar;

#ifndef USE_STD_LIB
#define  RCC_APB1PeriphClockCmd_Enable_PWR_BKP() \
	SET_REG_32_BIT(RCC->APB1ENR, RCC_APB1Periph_PWR | RCC_APB1Periph_BKP)
	
#define RTCDRV_EnterConfigMode()     (RTC->CRL |= RTC_CRL_CNF)
#define RTCDRV_ExitConfigMode()      (RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF))
#define SCB_AIRCR_VECTKEY_MASK       ((uint32_t)0x05FA0000)

/* Alias word address of DBP bit */
#define RTCDRV_PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

#define RTCDRV_CR_OFFSET                (RTCDRV_PWR_OFFSET + 0x00)
#define RTCDRV_DBP_BitNumber            0x08
#define RTCDRV_CR_DBP_BB                (PERIPH_BB_BASE + (RTCDRV_CR_OFFSET * 32) + (RTCDRV_DBP_BitNumber * 4))

#define RTCDRV_RCC_OFFSET                (RCC_BASE - PERIPH_BASE)

/* Alias word address of RTCEN bit */
#define RTCDRV_BDCR_OFFSET               (RTCDRV_RCC_OFFSET + 0x20)
#define RTCDRV_RTCEN_BitNumber           0x0F
#define RTCDRV_BDCR_RTCEN_BB             (PERIPH_BB_BASE + (RTCDRV_BDCR_OFFSET * 32) + (RTCDRV_RTCEN_BitNumber * 4))

/* Alias word address of BDRST bit */
#define RTCDRV_BDRST_BitNumber           0x10
#define RTCDRV_BDCR_BDRST_BB             (PERIPH_BB_BASE + (RTCDRV_BDCR_OFFSET * 32) + (RTCDRV_BDRST_BitNumber * 4))

#define RTCDRV_BDCR_ADDRESS              (PERIPH_BASE + RTCDRV_BDCR_OFFSET)



#define RTCDRV_BKP_WriteBackupRegister(BKP_DR, Data)  (*(__IO uint32_t *) ((uint32_t)BKP_BASE + BKP_DR) = Data)
#define RTCDRV_BKP_ReadBackupRegister(BKP_DR)        (*(__IO uint16_t *) (((uint32_t)BKP_BASE) + (BKP_DR)))

#define RTCDRV_WAIT_TIMEOUT          ((uint32_t) 0x00020000)


static uint8_t RTCDRV_WaitForLastTask(void)
{
  uint32_t count = 0;
  
  /* Loop until RTOFF flag is set */
  while ( (! (RTC->CRL & RTC_CRL_RTOFF)) && (count++ < RTCDRV_WAIT_TIMEOUT))
  {
  }
  return ( (count >= RTCDRV_WAIT_TIMEOUT) ? 1 : 0);
}



static uint8_t RTCDRV_WaitForSynchro(void)
{
  uint32_t count = 0;
  
  /* Clear RSF flag */
  RTC->CRL &= (uint16_t)~RTC_CRL_RSF;
  /* Loop until RSF flag is set */
  while ((! (RTC->CRL & RTC_CRL_RSF)) && (count++ < RTCDRV_WAIT_TIMEOUT) )
  {
  }
  return ( (count >= RTCDRV_WAIT_TIMEOUT) ? 1 : 0);
}

#endif

#ifdef USE_STD_LIB
static void RTC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
 // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
#else
#define RTC_NVIC_Config()  STM32_NVICInit(RTC_IRQn, 4, 6, 0)   	// 第3组优先级, 3位抢占优先级, 1位响应优先级
#endif

/**************************
功能: RTC 初始化配置
参数: None
返回值: 1: 初始化失败; 0: 成功
***************************/
#include "delay.h"



static uint8_t RTC_Configuration(void)
{
  uint16_t count = 0;
  uint8_t ret = 0;
  
#ifdef USE_STD_LIB
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);  /* Enable PWR and BKP clocks */
  PWR_BackupAccessCmd(ENABLE);  /* Allow access to BKP Domain */
  BKP_DeInit();                   /* Reset Backup Domain */
  RCC_LSEConfig(RCC_LSE_ON);    /* Enable LSE */
#else
  RCC_APB1PeriphClockCmd_Enable_PWR_BKP();                /* Enable PWR and BKP clocks */
  *(__IO uint32_t *) RTCDRV_CR_DBP_BB = (uint32_t)ENABLE;  /* Allow access to BKP Domain */

  /* Reset Backup Domain */
  *(__IO uint32_t *) RTCDRV_BDCR_BDRST_BB = (uint32_t)ENABLE;
  *(__IO uint32_t *) RTCDRV_BDCR_BDRST_BB = (uint32_t)DISABLE;
  
   /* Enable LSE */
  *(__IO uint8_t *) RTCDRV_BDCR_ADDRESS = RCC_LSE_OFF;
  *(__IO uint8_t *) RTCDRV_BDCR_ADDRESS = RCC_LSE_OFF;
  *(__IO uint8_t *) RTCDRV_BDCR_ADDRESS = RCC_LSE_ON;
#endif
  
  /* Wait till LSE is ready */
#ifdef USE_STD_LIB
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && count < 350)
#else
  while((! READ_REG_32_BIT(RCC->BDCR, RCC_BDCR_LSERDY)) && count < 350)
#endif
  {
     count++;
	 delay_ms(10);
  }
  if(count >= 350)
  {
     RTC_DEBUG("RTC LSE Init failed, %s, %d\r\n", __FILE__, __LINE__);
     return 1;
  }
  
  
#ifdef USE_STD_LIB
   RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                       /* Select LSE as RTC Clock Source */
   RCC_RTCCLKCmd(ENABLE);                                          /*  Enable RTC Clock */
  
   /* Wait for RTC registers synchronization */
   ret = RTC_WaitForSynchro();
   if(ret)return 1;
  
   /* Wait until last write operation on RTC registers has finished */
   ret = RTC_WaitForLastTask();
   if(ret)return 1;
 
   RTC_ITConfig(RTC_IT_SEC, ENABLE);  /* Enable the RTC Second */

    /* Wait until last write operation on RTC registers has finished */
   ret = RTC_WaitForLastTask();
   if(ret)return 1;

   /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
   
    /* Wait until last write operation on RTC registers has finished */
    ret = RTC_WaitForLastTask();
    if(ret)return 1;
#else
   SET_REG_32_BIT(RCC->BDCR, RCC_RTCCLKSource_LSE);             /* Select LSE as RTC Clock Source */
   *(__IO uint32_t *) RTCDRV_BDCR_RTCEN_BB = (uint32_t)ENABLE;   /*  Enable RTC Clock */

   /* Wait for RTC registers synchronization */
   ret = RTCDRV_WaitForSynchro();
   if(ret)return 1;
  
   /* Wait until last write operation on RTC registers has finished */
   ret = RTCDRV_WaitForLastTask();
   if(ret)return 1;

   RTC->CRH |= RTC_IT_SEC;   /* Enable the RTC Second */

   /* Wait until last write operation on RTC registers has finished */
   ret = RTCDRV_WaitForLastTask();
   if(ret)return 1;

   /* Set RTC prescaler: set RTC period to 1sec */
   /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
   RTCDRV_EnterConfigMode();
   RTC->PRLH = (32767 & ((uint32_t)0x000F0000)) >> 16;
   RTC->PRLL = (32767 & ((uint32_t)0x0000FFFF));
   RTCDRV_ExitConfigMode();

   /* Wait until last write operation on RTC registers has finished */
   ret = RTCDRV_WaitForLastTask();
   if(ret)return 1;
#endif

   return 0;  
}


void RTCDrv_Test(void)
{
    T_Calendar_Obj cal, new_cal;
    uint32_t sec;
	
	cal.year  = 2018;
	cal.month = 1;
	cal.day   = 2;
	cal.hour  = 16;
	cal.min   = 16;
	cal.sec   = 30;
	cal.week  = 1;

    sec = RTCDrv_CalendarToSec(1970, &cal);

	RTC_DEBUG("old cal: %02d-%02d-%02d %02d:%02d:%02d unix_stamp = %ld\n", cal.year % 100, cal.month, cal.day,
		        cal.hour, cal.min, cal.sec, sec);
	
	RTCDrv_SecToCalendar(1970, sec, &new_cal, E_TRUE);
	RTC_DEBUG("new cal: %02d-%02d-%02d %02d:%02d:%02d unix_stamp = %ld\n", new_cal.year % 100, new_cal.month, new_cal.day,
		        new_cal.hour, new_cal.min, new_cal.sec, sec);
}

void RTCDrv_Init(void)
{
    uint8_t ret;

	
#ifdef USE_STD_LIB
	RTC_NVIC_Config();
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
       RTC_DEBUG("\r\n\n RTC not yet configured....");

       /* RTC Configuration */
       RTC_Configuration();

       RTC_DEBUG("\r\n RTC configured....");

       /* Adjust time by values entered by the user on the hyperterminal */
       RTCDrv_SetTime(2017, 11, 14, 12, 0, 30);

       BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
   }
   else
   {
       /* Check if the Power On Reset flag is set */
       if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
       {
           RTC_DEBUG("\r\n\n Power On Reset occurred....");
       }
       /* Check if the Pin Reset flag is set */
       else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
       {
           RTC_DEBUG("\r\n\n External Reset occurred....");
       }
       RTC_DEBUG("\r\n No need to configure RTC....");
       /* Wait for RTC registers synchronization */
       ret = RTC_WaitForSynchro();
       if(ret){ RTC_DEBUG("WaitForSync Failed: %s, %d\r\n", __FILE__, __LINE__); }
	   
       /* Enable the RTC Second */
       RTC_ITConfig(RTC_IT_SEC, ENABLE);
	   
       /* Wait until last write operation on RTC registers has finished */
       ret = RTC_WaitForLastTask();
	   if(ret){ RTC_DEBUG("WaitForLastTask Failed: %s, %d\r\n", __FILE__, __LINE__); }
    }
#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

  /* Clear reset flags */
  RCC_ClearFlag();

#else  // #ifdef USE_STD_LIB
    RTC_NVIC_Config();
	if (RTCDRV_BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
       RTC_DEBUG("\r\n\n RTC not yet configured....");

	   
       /* RTC Configuration */
       RTC_Configuration();

       RTC_DEBUG("\r\n RTC configured....");

       /* Adjust time by values entered by the user on the hyperterminal */
       RTCDrv_SetTime(2017, 10, 18, 10, 54, 25);

       RTCDRV_BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
   }
   else
   {
       /* Check if the Power On Reset flag is set */
       if(READ_REG_32_BIT(RCC->CSR, RCC_CSR_PORRSTF))
       {
           RTC_DEBUG("\r\n\n Power On Reset occurred....");
       }
       else if(READ_REG_32_BIT(RCC->CSR, RCC_CSR_PINRSTF))  /* Check if the Pin Reset flag is set */
       {
           RTC_DEBUG("\r\n\n External Reset occurred....");
       }
       RTC_DEBUG("\r\n No need to configure RTC....");
      
       ret = RTCDRV_WaitForSynchro();   /* Wait for RTC registers synchronization */
       if(ret){ RTC_DEBUG("WaitForSync Failed: %s, %d\r\n", __FILE__, __LINE__); }
	   
       /* Enable the RTC Second */
	   RTC->CRH |= RTC_IT_SEC;
	   
       ret = RTCDRV_WaitForLastTask();  /* Wait until last write operation on RTC registers has finished */
	   if(ret){ RTC_DEBUG("WaitForLastTask Failed: %s, %d\r\n", __FILE__, __LINE__); }
   }
   /* Clear reset flags */
   RCC->CSR |= RCC_CSR_RMVF;

#endif
    
}		 		


 			
// 闰年的条件: 能被4整除且不能被100 整除, 或者能被400 整除
#define  IS_LEAP_YEAR(year)  (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))


//月份数据表											 
uint8_t const table_week[12] = { 0,3,3,6,1,4,6,2,5,0,3,5 }; //月修正数据表	  

//平年的月份日期表
const uint8_t mon_table[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

/************************
功能: 计算从某年的1月1日0时0分0秒开始到现在的秒数
参数: uint16_t fromYear: 指定从某年开始
             T_RTC_TIME * cal: 当前的时间
返回值: 从指定年的1月1日0时0分0秒开始到现在的秒数
*************************/
uint32_t RTCDrv_CalendarToSec(uint16_t fromYear, T_Calendar_Obj * cal)
{
       uint16_t t;
	uint32_t seccount = 0;
       uint8_t month_idx;
       
    #if RTC_DEBUG_EN
	if(fromYear < 1970 || fromYear > 2099 || NULL == cal)
		{ RTC_DEBUG("param err: %s, %d\r\n", __FILE__, __LINE__);  return 1; }
	#endif
	
	for(t = fromYear; t < cal->year; t++)	//把所有年份的秒钟相加
	{
		if(IS_LEAP_YEAR(t))seccount += 31622400;  //闰年的秒钟数
		else seccount += 31536000;			       //平年的秒钟数
	}

	month_idx = cal->month - 1;
	for(t = 0; t < month_idx; t++)	   //把前面月份的秒钟数相加
	{
		seccount += (uint32_t)mon_table[t] * ONE_DAY_SEC;  //月份秒钟数相加, 86400 为一天的秒数
		if(IS_LEAP_YEAR(cal->year) && t == 1)seccount += ONE_DAY_SEC;  //闰年2月份增加一天的秒钟数	   
	}
	seccount += (uint32_t)(cal->day - 1) * ONE_DAY_SEC;  //把前面日期的秒钟数相加 
	seccount += (uint32_t)cal->hour * 3600;        //小时秒钟数
    seccount += (uint32_t)cal->min * 60;	          //分钟秒钟数
	seccount += cal->sec;                          //最后的秒钟加上去

	return seccount;
}

//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日 
//返回值：星期号: 0 - 6, 表示 星期日 - 星期六		
#if 1
uint8_t RTCDrv_GetWeek(uint16_t year, uint8_t month, uint8_t day)
{	
	uint16_t temp;
	uint8_t yearH, yearL;

    #if RTC_DEBUG_EN
	if(year < 1901 || year > 2099)
		{ RTC_DEBUG("param err: %s, %d\r\n", __FILE__, __LINE__);  return 0; }
	#endif
	
	yearH = year / 100;	
	yearL = year % 100; 
	
	// 如果为21世纪,年份数加100  
	if (yearH > 19) yearL+= 100;
	
	temp = yearL + yearL / 4;  // 所过闰年数只算1900年之后的  
	temp %= 7; 
	temp = temp + day + table_week[month-1];  // 获得总日数
	if (yearL % 4 == 0 && month < 3)temp--;
	return ( temp % 7 );
}	
#endif
	
/************************
功能: 计算从指定年的1月1日0时0分0秒开始到现在的秒数转换为当前时间
输入参数:  uint16_t fromYear: 指定从某年开始
                         uint32_t sec: 需要转换的秒值
                         uint8_t   one_more_day:  秒数是否不止一天, 如果不止一天需重新计算年月日
输出参数:  T_RTC_TIME * cal: 当前的时间
返回值:       1: 失败; 0: 成功
*************************/
void RTCDrv_SecToCalendar(uint16_t fromYear, uint32_t sec, T_Calendar_Obj * cal, uint8_t one_more_day)
{
	uint32_t temp = 0;
	uint16_t y    = 0;	  

    #if RTC_DEBUG_EN
    if(fromYear < 1970 || fromYear > 2099 || NULL == cal)
		{ RTC_DEBUG("param err: %s, %d\r\n", __FILE__, __LINE__);  return; }
	#endif
	
 	temp = sec / ONE_DAY_SEC;   //得到天数(秒钟数对应的)
	if(one_more_day)
	{	  
		y       = fromYear;	      // 从1970年开始
		while(temp >= 365)  // 超过 1 年
		{				 
			if(IS_LEAP_YEAR(y))  //是闰年
			{
				if(temp >= 366)temp -= 366;  //闰年的日数
				else
				{  
				   y++; break;
				}  
			}
			else temp-= 365; // 平年 
			y++;  
		}   
		cal->year = y;       // 得到年份
		y = 0;
		while(temp >= 28)  // 日数不止一个月的日数
		{
			if(IS_LEAP_YEAR(cal->year) && y == 1)  //是否为闰年的2月份
			{
				if(temp >= 29)temp -= 29;  //减去2月份的日数
				else break; 
			}
			else // 为闰年但不是2月份, 或者是平年的月份
			{
				if(temp >= mon_table[y])temp -= mon_table[y];  
				else break;
			}
			y++;  
		}
		cal->month = y    + 1;	// 得到月份
		cal->day   = temp + 1;  	// 得到日期 
	}
	temp       = sec % ONE_DAY_SEC;     //得到秒钟数   	   
	cal->hour = temp / 3600;            //小时
	cal->min  = (temp % 3600 ) / 60;   //分钟	
	cal->sec  = (temp % 3600 ) % 60;   //秒钟
	//cal->week = RTCDrv_GetWeek(cal->year, cal->month, cal->day);  //获取星期   
}
	
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
uint8_t RTCDrv_SetTime(uint16_t syear,uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
	uint32_t seccount;
	T_Calendar_Obj cal;

	cal.year = syear; cal.month = smon; cal.day = sday; 
	cal.hour = hour;  cal.min   = min;   cal.sec = sec;
	seccount = RTCDrv_CalendarToSec(1970, &cal);

    return RTCDrv_SetUnixTimeStamp(seccount);    
}

/*
功能: RTC 设置时间, 以时间戳的形式
参数: 待设置秒值, 从1970-1-1 00:00:00 开始的秒值
返回值: 0: 设置成功;  1: 设置失败
*/
uint8_t RTCDrv_SetUnixTimeStamp(uint32_t seccount)
{
#ifdef USE_STD_LIB
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟	
		PWR_BackupAccessCmd(ENABLE);	  // 使能RTC和后备寄存器访问 
		RTC_SetCounter(seccount);	  // 设置RTC计数器的值
	
		if(RTC_WaitForLastTask())	// 等待最近一次对RTC寄存器的写操作完成		
			{ RTC_DEBUG("RTC_SET() failed: %s, %d\r\n", __FILE__, __LINE__); return 1; }   
#else
    RCC_APB1PeriphClockCmd_Enable_PWR_BKP();
	*(__IO uint32_t *) RTCDRV_CR_DBP_BB = (uint32_t)ENABLE;

	RTCDRV_EnterConfigMode();
    RTC->CNTH = seccount >> 16;                         /* Set RTC COUNTER MSB word */
    RTC->CNTL = (seccount & ((uint32_t)0x0000FFFF));  /* Set RTC COUNTER LSB word */
    RTCDRV_ExitConfigMode();

	if(RTCDRV_WaitForLastTask())   // 等待最近一次对RTC寄存器的写操作完成  	
		{ RTC_DEBUG("RTC_SET() failed: %s, %d\r\n", __FILE__, __LINE__); return 1; }   
#endif

    return 0;
}
	
//初始化闹钟		  
//以1970年1月1日为基准
//1970~2099年为合法年份
//syear,smon,sday,hour,min,sec：闹钟的年月日时分秒   
//返回值:0,成功;其他:错误代码.
uint8_t RTCDrv_SetAlarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
	uint32_t seccount;
	T_Calendar_Obj cal;

    //下面三步是必须的!
	cal.year = syear; cal.month = smon; cal.day = sday; 
	cal.hour = hour;  cal.min   = min;   cal.sec = sec;
	seccount = RTCDrv_CalendarToSec(1970, &cal);
	
	//设置时钟
#ifdef USE_STD_LIB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  
	
	RTC_SetAlarm(seccount);

	if(RTC_WaitForLastTask())   // 等待最近一次对RTC寄存器的写操作完成  	
		{ RTC_DEBUG("RTC_SET() failed: %s, %d\r\n", __FILE__, __LINE__); return 1; }  	
#else
	RCC_APB1PeriphClockCmd_Enable_PWR_BKP();
    *(__IO uint32_t *) RTCDRV_CR_DBP_BB = (uint32_t)ENABLE;
	
	RTCDRV_EnterConfigMode();
    RTC->ALRH = seccount >> 16;                          /* Set the ALARM MSB word */
    RTC->ALRL = (seccount & ((uint32_t)0x0000FFFF));   /* Set the ALARM LSB word */
    RTCDRV_ExitConfigMode();

	if(RTCDRV_WaitForLastTask())   // 等待最近一次对RTC寄存器的写操作完成  	
		{ RTC_DEBUG("RTC_SET() failed: %s, %d\r\n", __FILE__, __LINE__); return 1; }  	
#endif
	
	return 0;	    
}

// 获取RTC时间
void RTCDrv_CalToRTCTime(T_RTC_TIME * rtc_time)
{
   rtc_time->year  = calendar.year % 100;
   rtc_time->month = calendar.month;
   rtc_time->day   = calendar.day;

   rtc_time->hour  = calendar.hour;
   rtc_time->min   = calendar.min;
   rtc_time->sec   = calendar.sec;
   rtc_time->week  = calendar.week;
}

//得到当前的时间
//返回值:0,成功;其他:错误代码.
uint8_t RTCDrv_GetTime(T_Calendar_Obj * cal)
{
	uint32_t sec = 0;
    static uint16_t day_count = 0;  // 记录上一次保存的日数
	uint16_t cur_day_count = 0;
    uint8_t  one_more_day = E_FALSE;

    sec = RTC_GetCounter();	 
	cur_day_count = sec / ONE_DAY_SEC;
    if(day_count != cur_day_count)
    {
        day_count    = cur_day_count;
		one_more_day = E_TRUE;
    }
 	RTCDrv_SecToCalendar(1970, sec, cal, one_more_day);
	
	return 0;
}	 	  




//RTC时钟中断
//每秒触发一次  
/*
void RTC_IRQHandler(void)
{		 
    #if 0
	if (RTC_GetITStatus(RTC_IT_SEC))  //秒钟中断
	#else
    if( (RTC->CRH & RTC_IT_SEC) && (RTC->CRL & RTC_IT_SEC)) //秒钟中断
	#endif
	{			
		RTCDrv_GetTime(&calendar);   // 更新时间
		RTC_DEBUG("RTC Time:%04d-%02d-%02d %02d:%02d:%02d, week = %d\n", calendar.year, calendar.month, calendar.day, 
  	                calendar.hour, calendar.min, calendar.sec, calendar.week);
 	}
	
	#if 0
	if(RTC_GetITStatus(RTC_IT_ALR))   //闹钟中断
	#else
    if((RTC->CRH & RTC_IT_ALR) && (RTC->CRL & RTC_IT_ALR)) //闹钟中断
	#endif
	{ 	
		RTC->CRL &= (uint16_t)~RTC_IT_ALR;  //清闹钟中断	

	    RTCDrv_GetTime(&calendar);   // 更新时间  
  	    RTC_DEBUG("Alarm Time:%04d-%02d-%02d %02d:%02d:%02d, week = %d\n", calendar.year, calendar.month, calendar.day, 
  	                calendar.hour, calendar.min, calendar.sec, calendar.week);
  	} 				  								 	    						 	   	 
	RTC->CRL &= (uint16_t)~ (RTC_IT_ALR | RTC_IT_SEC);  // 清中断标志位
}
*/
