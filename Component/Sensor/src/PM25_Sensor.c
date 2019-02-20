
#include "sensor.h"
#include "uart_drv.h"
#include "os_timer.h"
#include "SnSGUI.h"
#include "sw_uart2.h"

#if PM25_DBG_EN
//#define PM25_DBG(fmt, ...)    os_printf(fmt, ##__VA_ARGS__)
#define PM25_DBG    dbg_print_detail
#else
#define PM25_DBG(...)
#endif



static T_UART_Rx Rx;
static os_timer_t tTimerPM25;
static T_PM25CmdContent PM25;

#if 1
static const 
#else
static __eeprom	
#endif

uint8_t AqiLevelString[7][16] = {
{"Good          " },
{"Moderate      "},
{"Unhealthy     "},
{"Unhealthy     "},
{"Very Unhealthy"},
{"Just In Hell  "},
{"Heavy In Hell "},
};

//对于 E_AQI_STD
static const uint8_t AqiStdString[][7] = {
{"AQI US"},
{"AQI CN"},
};



// AQI 等级, 共 7 级
static const T_AQI AqiLevel[] = 
{
// C_low_us C_high_us, C_low_cn, C_high_cn, aqi_low,  aqi_high 
  	{0,       15.4,       0,         35,          0,          50  }, 
	{15.5,    40.4,      35.1,      75,          51,         100 }, 
  	{40.5,    65.4,      75.1,      115,         101,       150 },
  	{65.5,    150.4,     115.1,     150,        151,        200 },
    {150.5,   250.4,     150.1,     250,        201,        300},
    {250.5,   350.4,     250.1,     350,        301,        400},
    {350.5,   500.4,     350.1,     500,        401,        500},
};

#define AQI_LEVEL_STR_SIZE  ( sizeof(AqiLevelString) / sizeof(AqiLevelString[0]) )  
#define AQI_LEVEL_SIZE      (sizeof(AqiLevel) / sizeof(AqiLevel[0]))

// AQI 计算
// 参数: uint16_t pm25: PM25 浓度, 
// E_AQI_STD aqi_std: 标准选择
// uint8_t * level: 输出的aqi等级: 0-6 : 表示AQI 等级: 1-7
// 返回值: AQI
static uint16_t PM25_AqiCalculate(uint16_t pm25, E_AQI_STD aqi_std, uint8_t *level)
{
    float c_low = 0, c_high = 0;
	uint8_t i;
    float deltaC;     // 浓度差值
    uint16_t deltaI;  // AQI 差值
	uint16_t aqi = 0;
	
	if(pm25 > 500)pm25 = 500;
	
	if(aqi_std == AQI_CN)
	{
	    for(i = 0; i < AQI_LEVEL_SIZE; i++)
		{
		    if(AqiLevel[i].C_low_cn <= pm25 && pm25 <= AqiLevel[i].C_high_cn)
		    {
		        c_low  = AqiLevel[i].C_low_cn;
				c_high = AqiLevel[i].C_high_cn;
				break;
		    }
		}
	}
	else
	{
	    for(i = 0; i < AQI_LEVEL_SIZE; i++)
		{
		    if(AqiLevel[i].C_low_us <= pm25 && pm25 <= AqiLevel[i].C_high_us)
		    {
		       c_low  = AqiLevel[i].C_low_us;
			   c_high = AqiLevel[i].C_high_us;
			   break;
		    }
		}
	}

    if(i < AQI_LEVEL_SIZE)	
    {
        deltaC = c_high - c_low;
		deltaI = AqiLevel[i].I_high - AqiLevel[i].I_low;
		aqi = (uint16_t)(((double)deltaI) * (pm25 - c_low) / deltaC + AqiLevel[i].I_low);
    }
	else{ i = 0; }

	*level = i;
	return aqi;
}

#if PM25_SNS_EN
static void Sns_PM25_Display(void)
{


	 do
	 {
	     uint8_t lev = 0;
             static u8 first_time = 1;

		 if(first_time)
		 {
		       first_time = 0;
			SnsGUI_ClearCircle(CIRCLE_PM10);
			SnsGUI_ClearCircle(CIRCLE_PM2P5);
		 }
		 
		 PM25_AqiCalculate( PM25.pm2p5_air, AQI_US, &lev);
		 lev += 1;
               SnsGUI_DisplayPM25( PM25.pm2p5_air, lev);

		 PM25_AqiCalculate( PM25.pm10_air, AQI_US, &lev);
		 lev += 1;
               SnsGUI_DisplayPM10( PM25.pm10_air, lev);
	 }while(0);
}
#endif


// len: 为报文总长度
static SYS_RESULT  PM25BufCheck(uint8_t * buf, uint16_t len)
{
        uint16_t new_sum = 0;
        uint16_t old_sum = 0;
        uint16_t i;
        uint16_t sum_len;

         PM25_DBG("\r\npm25 buf: \r\n");
          for(i = 0; i < len; i++)
          {  
                  PM25_DBG("0x%x  ", Rx.buf[i]);
          }
          PM25_DBG("\r\n");
          
         if(buf[0] != 0x42 && buf[1] != 0x4d || len < 2)
        {
              PM25_DBG("PM25BufCheck: header err\r\n");
		return SYS_FAILED;
         }
        sum_len = len - 2;  // 校验的数据长度, 除校验码外
        for(i = 0; i < sum_len; i++)
        {
               new_sum += buf[i];
        }
        old_sum = ((uint16_t)buf[len - 2] << 8) + buf[len - 1];
        if(new_sum == old_sum)return SYS_SUCCESS;

        PM25_DBG("PM25BufCheck: sum err\r\n");
        return SYS_FAILED;
}
static void TimerPM25_CallBack(void * arg)
{
        uint16_t * p;
        uint8_t * buf = Rx.buf;
        uint8_t i;

        #if PM25_DBG_EN
        uint16_t pt_2p5um;     // pm2.5 粒子
        //uint16_t pt_10p0um;  // pm10 粒子
        #endif
        
        if(PM25BufCheck(Rx.buf,  Rx.rx_len + 4) != SYS_SUCCESS)return;

        PM25.len = (((uint16_t)buf[2]) << 8) + buf[3];
        p = (uint16_t *)&PM25.pm1p0_cf1;
	 for(i = 0; i < 14; i++)  // 包括校验和
	{
	      p[i] = (((uint16_t)buf[4 + i * 2]) << 8) + buf[5 + i * 2];
	}

       #if PM25_DBG_EN
       pt_2p5um =   PM25.PtCnt_2p5um + PM25.PtCnt_10p0um;
                                 
	PM25_DBG("pm2.5 ug = %d, pm2.5 cnt = %d \r\n", 
	                        PM25.pm2p5_air,  pt_2p5um);
       PM25_DBG("pm10  ug = %d, pm10 cnt = %d \r\n",
                               PM25.pm10_air,    PM25.PtCnt_10p0um);
       #endif

        SDRR_SaveSensorPoint(SENSOR_PM25,           &PM25.pm2p5_air);
        SDRR_SaveSensorPoint(SENSOR_PM10,           &PM25.pm10_air);
        SDRR_SaveSensorPoint(SENSOR_0P3_UM,      &PM25.PtCnt_0p3um);
        SDRR_SaveSensorPoint(SENSOR_PM10_UM,   &PM25.PtCnt_10p0um);
        Sns_PM25_Display();
}

#if (PM25_UART_TYPE == HW_UART_TYPE)
void HW_USART2_IRQHandler(void)
#else
void SW_UART2_IRQHandler(uint8_t data)
#endif
{ 
       #if (PM25_UART_TYPE == HW_UART_TYPE)
       if(READ_REG_32_BIT(PM25_USART->SR, USART_SR_RXNE))
       #endif
       {
                #if (PM25_UART_TYPE == HW_UART_TYPE)
                uint8_t data = 0;
	         data = (uint16_t)(PM25_USART->DR & (uint16_t)0x01FF);
	         #endif
	         
               if(Rx.last_val == 0x42 && data == 0x4d)  // header
		 {
			Rx.rx_cnt = 0;
			Rx.buf[Rx.rx_cnt++] = Rx.last_val;
		 }
		 else if(Rx.rx_cnt >= Rx.size)
		 {
			Rx.rx_cnt = 0;
                     // try to disable interrupt here
		 }
		 Rx.buf[Rx.rx_cnt++] = data;
		 Rx.last_val = data;
		 if(Rx.rx_cnt > 3)
		{
    		        Rx.rx_len = (((uint16_t)Rx.buf[2]) << 8) + Rx.buf[3];
    		        if((Rx.rx_len + 4) == Rx.rx_cnt)
    		        {
    			          os_timer_arm(&tTimerPM25,  0, 0);
    			          Rx.rx_cnt = 0;
    		       }
    		       else if(Rx.rx_cnt == 32)
    		       {
                             uint8_t i;

                             PM25_DBG("\r\n--------------error-----------\r\n");
                             for(i = 0; i < Rx.rx_cnt; i++)
                             {  
                                    PM25_DBG("%02x  ", Rx.buf[i]);
                             }
                             PM25_DBG("\r\n");
    		       }
	       }
       }
}

static uint8_t pm25_rx_buf[32];
void PM25_Sensor_PreInit(void)
{
       Rx.buf   = pm25_rx_buf;
       Rx.size = sizeof(pm25_rx_buf);
       os_timer_setfn(&tTimerPM25, TimerPM25_CallBack,  NULL);
}
void PM25_Sensor_Init(void)
{
       #if (PM25_UART_TYPE  == HW_UART_TYPE)
       PM25_USART->CR1 |= (USART_CR1_UE | USART_Mode_Rx );
       //PM25_USART->CR1 |= (USART_Mode_Tx);


      PM25_USART->CR1 &= ~(USART_Mode_Tx);

      PM25_USART->CR1 |= USART_CR1_RXNEIE;	// 接收中断使能
       #else
       SWUART2_Init();
       #endif 
}

