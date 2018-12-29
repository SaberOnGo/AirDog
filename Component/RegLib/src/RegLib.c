
#include "RegLib.h"
#include <math.h>

//函数作用：设置向量表偏移地址
//参数说明：NVIC_VectTab:基址，NVIC_Offset:偏移量
//测试结果：NOT CHECK
//完成日期：14.02.27
void STM32_NVICSetVectorTable(u32 NVIC_VectTab, u32 NVIC_Offset)
{
	//检查参数合法性
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(NVIC_Offset));
	SCB->VTOR = NVIC_VectTab|(NVIC_Offset & (u32)0x1fffff80);	//设置NVIC的向量表偏移寄存器
}


//函数作用：设置中断分组
//参数说明：NVIC_Group:中断分组
//测试结果：CHECK OK
//完成日期：14.02.22
//更新日期：15.10.19
void STM32_NVICPriorityGroupConfig(u8 NVIC_Group)													
{
	if(NVIC_Group<=4)		//参数检查
		SCB->AIRCR=((SCB->AIRCR)&0x0000f8ff)+0x05fa0000+((7-NVIC_Group)<<8);				
}

//函数作用：设置中断分组、优先级设置及中断使能
//参数说明：NVIC_Channel为中断通道（中断编号），NVIC_Group为中断分组，NVIC_PreemptionPriority为抢占优先级
// NVIC_SubPriority为响应优先级；
//测试结果：CHECK OK
//完成日期：14.02.22
//更新日期：15.10.19
void STM32_NVICInit(u8 NVIC_Channel,u8 NVIC_Group,u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority)	
{
	u8 Priority = 0;
	STM32_NVICPriorityGroupConfig(NVIC_Group);																//中断分组	
	if((NVIC_PreemptionPriority<ldexp(1,NVIC_GROUP))&&(NVIC_SubPriority<ldexp(1,4-NVIC_GROUP)))		//参数检查
	{
		Priority=(NVIC_PreemptionPriority<<(4-NVIC_Group))+NVIC_SubPriority;		//中断优先级设置
		NVIC->IP[NVIC_Channel/4]=(NVIC->IP[NVIC_Channel/4]&(~(0xf<<((NVIC_Channel%4)*8+4))))+(Priority<<((NVIC_Channel%4)*8+4));
	}
	NVIC->ISER[NVIC_Channel/32]|=1<<(NVIC_Channel%32);												//中断使能
}

//函数作用：初始化引脚工作模式
//参数说明：GPIOx:要初始化的引脚所属GPIO口；GPIOInit_pst:用于初始化引脚的结构体（详见stm32f10x_gpio.h）
//测试结果：NOT CHECK
//完成日期：14.03.01
void STM32_GPIO_Init(GPIO_TypeDef* GPIOx,GPIO_InitTypeDef* GPIOInit_pst)
{
	u8 pin=0,mode=0;								//pin用于从0到15遍历，查找操作引脚；mode用于得出并存放引脚模式的值
	for(pin=0;pin<16;pin++)
	{
		if(((GPIOInit_pst->GPIO_Pin>>pin)&0x1)==1)				//操作pin引脚
		{
			if(((GPIOInit_pst->GPIO_Mode>>4)&0x1)==1)			//输出
				mode=(GPIOInit_pst->GPIO_Mode&0xf)+GPIOInit_pst->GPIO_Speed;
			else												//输入
			{
				mode=GPIOInit_pst->GPIO_Mode&0xf;
				if(((GPIOInit_pst->GPIO_Mode>>4)&0xf)==0x2)			//引脚下拉
					GPIOx->ODR&=~(1<<pin);
				else if(((GPIOInit_pst->GPIO_Mode>>4)&0xf)==0x4)	//引脚上拉
					GPIOx->ODR|=1<<pin;
			}
				
			if(pin<8)												
				GPIOx->CRL=(GPIOx->CRL&(~(0xf<<(pin*4))))+(mode<<(pin*4));			//配置引脚工作模式
			else
				GPIOx->CRH=(GPIOx->CRH&(~(0xf<<((pin-8)*4))))+(mode<<((pin-8)*4));	//配置引脚工作模式
		}
	}
}

//函数作用：对IO口进行按位写操作
//参数说明：GPIOx:要进行位操作的引脚所属GPIO口；GPIO_Pin:要操作的引脚（支持或操作）；
//			BitVal:要把引脚设置为的值（Bit_SET或Bit_RESET）
//测试结果：NOT CHECK
//完成日期：14.03.01
void STM32_GPIOWriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
	if(Bit_SET==BitVal)			//pin置高
		GPIOx->BSRR=GPIO_Pin;
	else if(Bit_RESET==BitVal)	//pin置低
		GPIOx->BRR=GPIO_Pin;
}

//函数作用：对IO口进行按位读操作
//参数说明：GPIOx:要进行位操作的引脚所属GPIO口；GPIO_Pin:要操作的引脚
//返回值：要读取引脚的电平值（Bit_SET或Bit_RESET）
//测试结果：NOT CHECK
//完成日期：14.03.01
BitAction STM32_GPIOReadBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if(GPIOx->IDR&GPIO_Pin)		//高电平
		return Bit_SET;
	else						//低电平
		return Bit_RESET;
}

//函数作用：对IO口进行整体写操作
//参数说明：GPIOx:要进行整体写操作的GPIO口；Data:要写入IO口的值
//测试结果：NOT CHECK
//完成日期：14.03.01
void STM32_GPIOWriteData(GPIO_TypeDef* GPIOx,u16 Data)
{
	GPIOx->ODR=Data;
}

//函数作用：对IO口进行整体读操作
//参数说明：GPIOx:要进行整体读操作的GPIO口
//返回值：要读取的GPIO口的引脚电平值（16位数）
//测试结果：NOT CHECK
//完成日期：14.03.01
u16 STM32_GPIOReadData(GPIO_TypeDef* GPIOx)
{
	return (GPIOx->IDR&0xffff);
}


//函数作用：SPIx波特率分频设置
//参数说明：SPIx为SPI口，SPIBaudFreqDiv为波特率分频类型变量
//测试结果：SPI2,CHECK OK
//完成日期：15.02.05
//波特率分频设置:
//SPIBaudFreqDiv_2   2分频   (SPI 36M@sys 72M)
//SPIBaudFreqDiv_8   8分频   (SPI 9M@sys 72M)
//SPIBaudFreqDiv_16  16分频  (SPI 4.5M@sys 72M)
//SPIBaudFreqDiv_256 256分频 (SPI 281.25K@sys 72M)
void STM32_SPIxSetBaudFreqDiv(SPI_TypeDef *SPIx,SPIBaudFreqDiv_TypeDef SPIBaudFreqDiv) 	//设置SPI波特率分频  
{
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPIBaudFreqDiv<<3;	
	SPIx->CR1|=1<<6; //SPI设备使能	  
} 
//函数作用：SPIx 读写一个字节
//参数说明：SPIx为SPI口，TxData为要写入的字节。函数返回读取到的字节
//测试结果：SPI2,CHECK OK
//完成日期：15.02.05
u8 STM32_SPIxReadWriteByte(SPI_TypeDef *SPIx,u8 TxData)
{		
	u8 retry=0;				 
	while((SPIx->SR&1<<1)==0)	//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPIx->DR=TxData;	 	  		//发送一个byte 
	retry=0;
	while((SPIx->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPIx->DR;          //返回收到的数据				    
}

void STM32_SPI_I2S_DeInit(SPI_TypeDef * SPIx)
{
    switch((uint32_t)SPIx)
    {
        case  (uint32_t)SPI1:
		{
			SPI1_I2S_DEINIT();
		}break;
		case  (uint32_t)SPI2:
		{
			SPI2_I2S_DEINIT();
		}break;
		case  (uint32_t)SPI3:
		{
			SPI3_I2C_DEINIT();
		}break;
	}
}


#define READ_OLD_ADC_SMPR1(ADCx, ADC_Channel) \
	(ADCx->SMPR1 & (~(ADC_SMPR1_SMP10 << (3 * (ADC_Channel - 10)))))

#define READ_OLD_ADC_SMPR2(ADCx, ADC_Channel) \
	(ADCx->SMPR2 & (~(ADC_SMPR2_SMP0 << (3 * ADC_Channel))))

#define ADC_SMPR_NewSampleTime(ADC_SampleTime, ADC_Channel, index) \
	((uint32_t)ADC_SampleTime << (3 * (ADC_Channel - index)))

#define READ_OLD_ADC_SQR3(ADCx, Rank) \
	(ADCx->SQR3 & (~(ADC_SQR3_SQ1 << (5 * (Rank - 1)))))

#define READ_OLD_ADC_SQR2(ADCx, Rank) \
	(ADCx->SQR2 & (~(ADC_SQR2_SQ7 << (5 * (Rank - 7)))))

#define READ_OLD_ADC_SQR1(ADCx, Rank) \
	(ADCx->SQR1 & (~(ADC_SQR1_SQ13 << (5 * (Rank - 13)))))

#define ADC_SQR_NewRank(ADC_Channel, Rank, index) \
	((uint32_t)ADC_Channel << (5 * (Rank - index)))



void STM32_ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
  /* if ADC_Channel_10 ... ADC_Channel_17 is selected */
  if (ADC_Channel > ADC_Channel_9)
  {

	ADCx->SMPR1 =  (READ_OLD_ADC_SMPR1(ADCx, ADC_Channel)) | ADC_SMPR_NewSampleTime(ADC_SampleTime, ADC_Channel, 10);
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {	
	ADCx->SMPR2 =  (READ_OLD_ADC_SMPR2(ADCx, ADC_Channel)) | ADC_SMPR_NewSampleTime(ADC_SampleTime, ADC_Channel, 0);
  }


  	
  /* For Rank 1 to 6 */
  if (Rank < 7)
  {
	ADCx->SQR3 = READ_OLD_ADC_SQR3(ADCx, Rank) |  ADC_SQR_NewRank(ADC_Channel, Rank, 1);
  }
  /* For Rank 7 to 12 */
  else if (Rank < 13)
  {
    ADCx->SQR2 = READ_OLD_ADC_SQR2(ADCx, Rank) | ADC_SQR_NewRank(ADC_Channel, Rank, 7);
	
  }
  /* For Rank 13 to 16 */
  else
  {
    ADCx->SQR1 = READ_OLD_ADC_SQR1(ADCx, Rank) | ADC_SQR_NewRank(ADC_Channel, Rank, 13);
  }
}

/**********************
功能: ADC 初始化, 对应于库函数的ADC_Init()
参数:  uint32_t ADC_Mode:   值为 ADC_Mode_Independent ... ADC_Mode_AlterTrig
              uint8_t   ADC_ScanConvMode: DISABLE, or ENABLE
              uint8_t   ContinuousConvMode: 值为 DISABLE, or ENABLE
              uint32_t ExternalTrigConv:   值为ADC_ExternalTrigConv_None ... ADC_ExternalTrigConv_T1_CC1 等
              uint32_t DataAlign: ADC_DataAlign_Right, ADC_DataAlign_Left
              uint8_t  NbrOfChannel: 需要转换的通道数, 1 - 16
*************************/
void STM32_ADC_Init(ADC_TypeDef* ADCx, 
                          uint32_t ADC_Mode, 
                          uint8_t  ScanConvMode, 
                          uint8_t  ContinuousConvMode, 
                          uint32_t ExternalTrigConv,
                          uint32_t DataAlign,
                          uint8_t  NbrOfChannel)
{
    ADCx->CR1 = (ADCx->CR1 & (~ADC_CR1_DUALMOD)) | ADC_Mode;   //ADC工作模式
	ADCx->CR1 = (ADCx->CR1 & (~ADC_CR1_SCAN)) | ((uint8_t)ScanConvMode << 8);    
	
	ADCx->CR2 = (ADCx->CR2 & (~ADC_CR2_CONT)) | ((uint8_t)ContinuousConvMode << 1);
	ADCx->CR2 = (ADCx->CR2 & (~ADC_ExternalTrigConv_None)) | ExternalTrigConv;
	ADCx->CR2 = (ADCx->CR2 & (~ADC_CR2_ALIGN)) | DataAlign;
	ADCx->SQR1  &=~ ADC_SQR1_L;								      //先清除需要转换的通道数
	ADCx->SQR1  |= (NbrOfChannel -1) << 20;  
}

