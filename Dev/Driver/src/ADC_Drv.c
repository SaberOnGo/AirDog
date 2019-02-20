
#include "ADC_Drv.h"
#include "board_version.h"


#include <stdio.h>
#include <stdarg.h>
#include "delay.h"
#include "FONT_API.h"

#if   DEBUG_ADC_EN
//#define ADC_DEBUG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#define  ADC_DEBUG  dbg_print_detail
#else
#define ADC_DEBUG(...)
#endif


// 计算电压值
// 比实际值大1000倍, 即将后3位小数转为整数
#define  GetVoltValue(adc_val) \
	((uint16_t)((double)adc_val * 3300 / 4096))  

// 计算电池电压
// 参数: uint16_t volt: ADC测量的电压, 单位: mV
// 返回值: uint16_t 电池电压: 单位: mV
#define  GetBatVolt(volt)   ((uint16_t)(( 635.0 / 470.0) * (volt)))




#define  ADC_DMA_MODE_EN    1    // ADC DMA 模式使能(1)


#define  N   10    // 每通道采 10 次
#define  M   3     // 3 个通道

vu16 AD_Value[N][M];    // 用来存放 ADC 转换结果，也是 DMA 的目标地址
uint16_t aver_volt[M];           // 管脚的平均电压值, 不是AD值, unit: mV


#if DEBUG_ADC_EN
#if 1
static const 
#else
static __eeprom	
#endif

uint8_t AdcChannelString[][12] = {
{"BAT_ADC  "},    // 0
{"Light_1  "},    // 1

{"TempSensor"},    // 2
//{"Vrefint   "},    // 3, 内部参考电压值一般为 1.16 V - 1.26 V
};
#endif

typedef enum
{
	E_BAT_ADC   = 0,
	E_LIGHT_1    = 1,
	E_TEMP_SNS = 2,  // MCU 内部温度传感器
	E_Vrefint,  // MCU ADC 内部参考电压
}E_ADC_CH_NAME;

// 存放ADC1 转换结果的寄存器的地址是 ADC1->DR
// 地址计算: 0x40000000 + 0x10000 + 0x2400 + 0x4C = 0x40012400 + 0x4C
// 0x4C = 76, 76 / 4 = 19, 即DR寄存器在 ADC1 基地址上偏移了19个寄存器(一个寄存器为32位)

void adc_gpio_init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;

	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  //模拟输入引脚


	 // 电池电压测量输入管脚
       GPIO_InitStructure.GPIO_Pin  = BAT_ADC_Pin;
       GPIO_Init(BAT_ADC_PORT, &GPIO_InitStructure);
	 

        // Light 1 光敏传感器输入
	 GPIO_InitStructure.GPIO_Pin  = LIGHT_1_Pin;
        GPIO_Init(LIGHT_1_PORT, &GPIO_InitStructure);
}

// ADC 外设使能
void adc_peripheral_enable(void)
{
	// 开启 ADC 的 DMA 支持(要实现 DMA 功能，还需独立配置 DMA 通道等参数)
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);  // 使能指定的 ADC1

    ADC_ResetCalibration(ADC1); //使能复位校准  
    while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准结束

    ADC_StartCalibration(ADC1); //开启AD校准
    while(ADC_GetCalibrationStatus(ADC1));  //等待校准结束
}

void adc_peripheral_disable(void)
{
    ADC_DMACmd(ADC1, DISABLE);
    ADC_Cmd(ADC1, DISABLE);  // 禁止指定的 ADC1
	     
    DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, DISABLE);   // 禁止DMA 传输完成中断
    DMA_Cmd(DMA1_Channel1, DISABLE); //禁止DMA 通道
}

// ADC DMA 模式初始化
void adc_dma_mode_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;

	ADC_DeInit(ADC1);  //重设为缺省值

	/*ADC1configuration ----------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  // ADC 工作模式:ADC1 和 ADC2工作在独立模式
    
    ADC_InitStructure.ADC_ScanConvMode        = ENABLE;  // 扫描模式, 即对多通道采样
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //DISABLE;  // 持续转换禁止, 即只单次转换
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; // 外部触发关闭
    ADC_InitStructure.ADC_DataAlign           = ADC_DataAlign_Right;       //  ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel        = M;      //顺序进行规则转换的 ADC 通道的数目
    
    ADC_Init(ADC1, &ADC_InitStructure);        //设置 寄存器

	/*ADC1regularchannel11configuration*/
     //设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
     //ADC1, ADC 通道 x,规则采样顺序值为 y,采样时间为 239.5 周期
     ADC_RegularChannelConfig(BAT_ADC_x,              BAT_ADC_Channel,            1,   ADC_SampleTime_239Cycles5);
     ADC_RegularChannelConfig(LIGHT_1_ADC_x,          LIGHT_1_Channel,       2,   ADC_SampleTime_239Cycles5);
     ADC_RegularChannelConfig(ADC1,                    ADC_Channel_TempSensor,  3,  ADC_SampleTime_239Cycles5);
     //ADC_RegularChannelConfig(ADC1,                    ADC_Channel_Vrefint,      4,  ADC_SampleTime_239Cycles5);

    /* Enable the temperature sensor and vref internal channel */
	ADC_TempSensorVrefintCmd(ENABLE);   // 温度传感器, 内部参考电压测量使能
	//adc_peripheral_enable();             // ADC 外设使能
}

static void adc_dma_config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
	
    DMA_DeInit(DMA1_Channel1); // 复位寄存器
    
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (u32)&(ADC1->DR); // DMA 外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr      = (u32)&AD_Value;    // DMA 内存基地址
    DMA_InitStructure.DMA_DIR                  =  DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize          =  N * M;   // 指定 DMA通道缓存大小
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;  //外设寄存器地址固定
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;        //内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据宽度为 16 位
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;     //内存数据宽度为 16 位
    DMA_InitStructure.DMA_Mode                 = DMA_Mode_Circular;     // 循环模式
    DMA_InitStructure.DMA_Priority             = DMA_Priority_High;  // DMA 通道 拥有高优先级
    DMA_InitStructure.DMA_M2M                  = DMA_M2M_Disable;     // DMA 通道 x 禁止设置为内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // 设置寄存器

    DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);   // 使能 DMA 传输完成中断
    DMA_Cmd(DMA1_Channel1, ENABLE); //启动 DMA 通道
}

// DMA 重新配置
void adc_dma_reconfig(void)
{
   adc_dma_config();
}


// 计算电压值
// 比实际值大1000倍, 即将后3位小数转为整数
uint16_t adc_get_volt_value(uint16_t adc_val)
{
    return (uint16_t)((double)adc_val * 3300 / 4096);  // 比实际值大1000倍, 即将后3位小数转为整数
}

// 计算电池电压
// 参数: uint16_t volt: ADC测量的电压, 单位: mV
// 返回值: uint16_t 电池电压: 单位: mV
//#define  adc_get_bat_volt(volt)   ((uint16_t)(((double)630.0 * (volt)) / 470.0))  //((uint16_t)(( 635.0 / 470.0) * (volt)))

uint16_t adc_get_bat_volt(uint16_t volt)
{
       uint16_t mV;

       mV = ((double)630) * volt / 470.0;
       return mV;
}
static uint8_t  first_core_temp_save = E_FALSE;
static uint16_t first_core_temp;



// 求平均值
void filter(void)
{
    uint16_t i, count;
    double sum = 0;
	
	for(i = 0; i < M; i++)   // 一共 M 个通道
	{
	    for(count = 0; count < N; count++)  // 采样 N 次
	    {
	        sum += AD_Value[count][i];
	    }
		aver_volt[i] = adc_get_volt_value(sum / N);
		sum = 0;
	}
}

uint16_t adc_get_temperature(uint16_t val)
{
    int16_t aver_temp;

	#if 0  // 此部分的 val  是 AD 测量值
    double Vsense = 0.0; //  温度传感器在指定温度时的电压值,unit: V
    
    // 温度计算公式: temp = (V25 - Vsense) / Avg_Slope + 25;
	// 其中: V25 = 1.43V, Vsense = val * 3.3 / 4096, Avg_Slope = 4.3 mV = (4.3 / 1000) V
	Vsense = (val * 3.3 / 4096);
	if(1.43 > Vsense)  // 温度高于 25 'C
	{
	    aver_temp = (1.43 - Vsense) * 1000 / 4.3 + 25;
	}
	else  // 温度低于 25 'C
	{
	    aver_temp = 25 - (Vsense - 1.43) * 1000 / 4.3;
	}
	#else   // 这里的val  是 测量的电压值, 单位: mV
    if(1430 > val)  // 温度高于 25 'C
    {
           aver_temp = ((double)(1430 - val)) / 4.3 + 25;
    }
    else  // 温度低于 25 'C
   {
	    aver_temp = 25 - ((double)(val - 1430)) / 4.3;
    }
    #endif
	
    return aver_temp;
}

#include "os_timer.h"
static os_timer_t  tTimerADCGetValue;
static os_timer_t  tTimerADCMeasure;      // 启动ADC 测量的定时器
static os_timer_t  tTimerADCRealStart;  

extern void BatLev_VoltToPercent(uint16_t bat_volt);

// 内核温度
uint16_t ADCDrv_GetFirstCoreTemp(void)
{
      return first_core_temp;
}

uint16_t ADCDrv_GetCurCoreTemp(void)
{
      return adc_get_temperature(aver_volt[E_TEMP_SNS]); 
}

static void Timer_ADCGetValue_CallBack(void * arg)
{
       ADC_DEBUG("ADCGet Value cb t = %ld \r\n", os_get_tick());
       filter();

        if(! first_core_temp_save)
	 {
	        uint16_t temp;

	       temp  = adc_get_temperature(aver_volt[E_TEMP_SNS]);                                                  
	       if(temp < 100)
	       {
                     first_core_temp = temp;
                     first_core_temp_save = E_TRUE;
	       }
	 }
	 else
	 {
               ADC_DEBUG("first core temp = %d 'C \r\n",   first_core_temp);
	 }
	 
	ADCDrv_DrawBatCapacity(0); 
}

extern uint8_t BatLev_GetPercent(void);
extern uint8_t BatteryIsCharging(void);
void ADCDrv_DrawBatCapacity(uint8_t reflush)
{
      #if DEBUG_ADC_EN
       uint8_t i;
       #endif
      
       uint16_t v = 0;
      
       v = adc_get_bat_volt(aver_volt[E_BAT_ADC]);

       #if DEBUG_ADC_EN
	 ADC_DEBUG("\r\n");
	 for(i = 0; i < M; i++)
	 {
		 ADC_DEBUG("%s volt[%d] = %01d.%03d V\r\n", AdcChannelString[i], i, aver_volt[i] / 1000, aver_volt[i] % 1000);
	 }
	 ADC_DEBUG("temp = %d 'C\n", adc_get_temperature(aver_volt[E_TEMP_SNS]));
	 
	 ADC_DEBUG("battery volt = %01d.%03d V\n", v / 1000, v % 1000);
	 ADC_DEBUG("\r\n\r\n");
	 #endif

	  BatLev_VoltToPercent(v);
	  ICON_SetBatPercent(BatLev_GetPercent(), BatteryIsCharging(),  reflush);
	  ADC_DEBUG("bat percent = %3d%% \r\n",  BatLev_GetPercent());
}

// 这时才真正启动ADC 测量
static void Timer_ADCRealStart_CallBack(void * arg)
{
    adc_dma_reconfig();
    adc_peripheral_enable();
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  /* Start ADC1 Software Conversion */ 

    // 测量完毕, 启动下一个采样循环
    os_timer_arm(&tTimerADCMeasure, SEC(10), 0);
}

// 测试 , 间隔 10 s 启动一次
static void Timer_ADCMeasure_CallBack(void * arg)
{
	//BAT_CE_Set(SW_CLOSE);  // 先关闭电池充电, 再检测电池电压, 这样测量的电池电压才准确
    
	// 500 ms 后启动, 稍微等待 500ms 是为了 CE 管脚的电压完全降低到实际的电池电压
      os_timer_arm(&tTimerADCRealStart, 1, 0);  
}



// ADC 启动初始化
void ADCDrv_DMALaunch(void)
{
      STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC 
		                            | RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟

	// PCLK2 = 48MHz, ADC时钟需要分频
       // ADCCLK = PCLK2 / 6 = 72 MHz / 6 = 12M
	STM32_RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6, 72 M /6 = 12, ADC最大时间不能超过14M
	STM32_RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能 DMA 传输

       adc_gpio_init();
	adc_dma_mode_init();
	
      // DMA 通道优先级设置
      STM32_NVICInit(DMA1_Channel1_IRQn, 4, 6, 0);	// 第4 组优先级, 4位抢占优先级, 0位响应优先级
    
	os_timer_setfn(&tTimerADCGetValue, Timer_ADCGetValue_CallBack, NULL);
	os_timer_setfn(&tTimerADCMeasure,   Timer_ADCMeasure_CallBack,  NULL);
	os_timer_setfn(&tTimerADCRealStart, Timer_ADCRealStart_CallBack, NULL);
	os_timer_arm(&tTimerADCMeasure, 0, 0);

	// 第一次采样
	adc_dma_config();
	adc_peripheral_enable();
       ADC_SoftwareStartConvCmd(ADC1, ENABLE);  /* Start ADC1 Software Conversion */ 
}

// 静默状态初始化
void ADCDrv_SilentInit(void)
{
   STM32_RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE); //禁止 DMA 传输
   adc_peripheral_disable();

   os_timer_disarm(&tTimerADCGetValue);
   os_timer_disarm(&tTimerADCMeasure);
   os_timer_disarm(&tTimerADCRealStart);
}

// DMA 中断完成
void DMA1_Channel1_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA1_IT_TC1))  // DMA 传输中断完成
   {
	   DMA_ClearITPendingBit(DMA1_IT_GL1);
	   DMA_ClearFlag(DMA1_FLAG_TC1);
          adc_peripheral_disable();
	  // BAT_CE_Set(SW_OPEN);  // 打开 TP4056 进行充电

	   os_timer_arm(&tTimerADCGetValue, 0, 0);  // 启动定时器获取数据
   }
}

/************************************电池电压测量功能 end ***********************************************/


////////////////////////////////////////////////////////////////////////////////////////////
/************************************电池电压测量功能 begin  ***********************************************/

// 常规的ADC 模式初始化, 电池电压测量
void ADCDrv_BatLevel_NormalModeInit(void)
{
       ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	STM32_RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PC0 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	STM32_GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}

// ADC 电池电压测量启动
void ADCDrv_BatLevelMeasureStart(void)
{
    ADCDrv_BatLevel_NormalModeInit();
    STM32_ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5 );
}	


static os_timer_t tTimerBatLevelADC;

#define MAX_TIMES   40    // 40 次平均
static uint32_t total_sum = 0;
static uint16_t get_adc_count = 0;







// 触发电池测量
void ADCDrv_TrigBatMeasure(void)
{
      STM32_ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // 软件启动 AD 转换
      os_timer_arm(&tTimerBatLevelADC, 5, 0);  
}

static void Timer_BatLevelADC_CallBack(void * arg)
{
       uint32_t tick = 2;
	if(READ_REG_32_BIT(ADC1->SR, ADC_SR_EOC)) //转换结束  
	{
	     #if DEBUG_ADC_EN
	      uint16_t cur_volt = 0;  //电压
		{
		    
			
			cur_volt = (uint16_t)ADC1->DR;
			cur_volt = 3300 * cur_volt / 4096;
			cur_volt = adc_get_bat_volt(cur_volt);
			ADC_DEBUG("1th=%d.%03d V\n", cur_volt / 1000, cur_volt % 1000);
		}
		#endif
		
	       total_sum += (uint16_t)ADC1->DR;
		get_adc_count++;
		if(get_adc_count >= MAX_TIMES)
		{
		   uint16_t aver_volt, aver_adc, bat_volt;
		   
		   aver_adc = total_sum / MAX_TIMES;

                 aver_volt = adc_get_volt_value(aver_adc); // 平均电压值
		   bat_volt  = adc_get_bat_volt(aver_volt);
		   
		   ADC_DEBUG("adc=%d, v=%d.%03d V, bat=%d.%03d V, %ld\n", aver_adc, aver_volt / 1000, aver_volt % 1000, 
		   	           bat_volt / 1000, bat_volt % 1000, os_get_tick());
		   
		   total_sum = 0;
		   get_adc_count = 0;
		   
		   if(arg)
		   {
		       ((void (*)(uint16_t))(arg))(bat_volt);  // 将电池电压转换为百分比
		   }
		   return;
		}
	}
	else
	{
	       ADC_DEBUG("adc not end\n");
	       tick = 50;
	}
       STM32_ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // 软件启动 AD 转换
       os_timer_arm(&tTimerBatLevelADC, tick, 0);  
}



/*****************************
功能: 启动 ADC 测量电池剩余电量
             测量结束后执行(*end_exe_func)(uint16_t ) 回调函数
参数: end_exe_func: 测量结束后需要执行的操作
********************************/
void ADCDrv_StartBatMeasure(void  (*end_exe_func)(uint16_t arg))
{
       ADCDrv_BatLevelMeasureStart();
	STM32_ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // 软件启动 AD 转换
	
	os_timer_setfn(&tTimerBatLevelADC, Timer_BatLevelADC_CallBack, end_exe_func);
       os_timer_arm(&tTimerBatLevelADC, 5, 0);
}

extern void BatLev_VoltToPercent(uint16_t bat_volt);

static os_timer_t tTimerBatRoutine;
static void TimerBatRoutine_CallBack(void * arg)
{
      ADCDrv_TrigBatMeasure();
      os_timer_arm(&tTimerBatRoutine,    SEC(8),  0);
}

void ADCDrv_NormalLaunch(void)
{
      ADCDrv_StartBatMeasure(BatLev_VoltToPercent);
      os_timer_setfn(&tTimerBatRoutine,  TimerBatRoutine_CallBack,  NULL);
      os_timer_arm(&tTimerBatRoutine,    0,  0);
}

