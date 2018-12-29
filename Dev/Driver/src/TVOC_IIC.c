
#include "TVOC_IIC.h"


//============================================================================

//注意：不同MCU系统，指令周期不同，但必须满足SCL引脚高低电平最小脉宽5us
//IIC为标准时序，最大工作频率100K.
//如果发现读数全部为0XFF，首先用示波器观察SCL 发送时钟的脉宽。

//==============================================================================
//1 用户需自行定义IO口;
//2 用户需自定义delay_us延时函数；
//3 直接调用iic_read(unsigned char *reg)，定义目标存储数组地址
//==============================================================================


//==============================================================================
//1 用户需更改此IO定义
//==============================================================================
	#define 	MI_SDA()		_IO_MI(PB,5)	//设置SDA脚为输入模式
	#define 	MO_SDA()		_IO_MO(PB,5)	//设置SDA脚为输出模式
	#define		OH_SDA()		_IO_OH(PB,5)	//设置SDA脚输出高电平	
	#define		OL_SDA()		_IO_OL(PB,5)	//设置SDA脚输出低电平	
	#define		DI_SDA()		_IO_DI(PB,5)    //读SDA引脚的输入值
	
	#define		OH_SCL()		_IO_OH(PB,4)	//设置SCL脚输出高电平	
	#define		OL_SCL()		_IO_OL(PB,4)	//设置SCL脚输出低电平	
        #define 	MO_SCL()		_IO_MO(PB,4)	//设置SCL为输出状态


//==============================================================================
//1 用户需自行更改此延时函数
//==============================================================================
volatile void delay_us(unsigned int t)
{
	while(t>0)
	{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");	
		t--;
	}
}

//==============================================================================
//因代码是在16M主频MCU系统上测试，有些时间是延时时间与指令周期累加而成，针对不同MCU
//如果发现延时时间不满足要求，可适当调整延时时间。
//以下代码仅可更改延时时间大小，其他部分请勿更改。
//==============================================================================
void	iic_start(void)	
{
	MO_SDA();
	delay_us(0x05);
	OH_SDA();
	delay_us(0x05);
	OH_SCL();
	delay_us(0x02);
	OL_SDA();
	delay_us(0x05);
	OL_SCL();
}

void	iic_stop(void)	
{
	MO_SDA();
	OL_SDA();
	delay_us(0x05);
	OH_SCL();
	delay_us(0x01);
	OH_SDA();
	delay_us(0x01);
}

_Bool iic_datasend(unsigned char iic_data)
{
	unsigned char n;
        MO_SCL();
        OL_SCL();
        MO_SDA();
	for(n=8;n>0;n--)
	{
		delay_us(0x1);		//发送地址字节，时钟低电平脉宽时间调整，指令周期+延时时间 需>5us
		if(iic_data&0x80) 
		{OH_SDA();}
		else
		{OL_SDA();}
		delay_us(0x1);		//此延时位数据发送建立时间，太小会导致数据引脚电平未稳定，模块无法识别地址
		OH_SCL();
		delay_us(0x05);		//发送地址字节，时钟高电平脉宽时间调整，需>5us
		OL_SCL();
		iic_data<<=1;
	}
	MI_SDA();
	delay_us(0x04);	
	OH_SCL();
	delay_us(0x04);			//等待模块应答时钟高电平延时
	if(DI_SDA())
	{	
		OL_SCL();
		return 0;
	}
	else
	{
		OL_SCL();
		return 1;
	}
}
	

void	iic_ack(_Bool flag)
{
	MO_SDA();
	if(flag)
	{OL_SDA();}
	else
	{OH_SDA();}
	delay_us(0x01);			//数据建立时间延时
	OH_SCL();
	delay_us(0x05);			//应答位时钟高电平脉宽调整，需>5us
	OL_SCL();
	delay_us(0x01);
	OH_SDA();
}

unsigned char iic_datareceive(void)
{
	unsigned char n;
	unsigned char data=0;
	MI_SDA();
	for(n=8;n>0;n--)
	{
		delay_us(0x04);
		OH_SCL();
		delay_us(0x02);      //数据接收中，时钟高电平脉宽时间调整，此时间过小会导致读数不正确
		if(DI_SDA())	     //时钟高电平脉宽时间需要 >5us
		{data|=0x01;}
        	if(n>1)
        	{data<<=1;}
		delay_us(0x01);	    
		OL_SCL();
	}
	delay_us(0x01);		    //数据接收中，时钟低电平脉冲时间调整。延时+指令周期需 >5us
	return data;
}

//=============================================================================
//IIC 读数据调用此函数。 
//输入：需要存储的数据指针，4byte
//==============================================================================
void iic_read(unsigned char *reg)
{
	unsigned char i;
        MO_SCL();				//时钟引脚切换到输出模式
	OL_SCL();				//时钟引脚输出低电平
	MO_SDA();				//数据引脚切换到输出模式
	OL_SDA();				//数据引脚输出低电平
	delay_us(0x05);
	OH_SCL();				//时钟输出高电平
	delay_us(0x01);
	OH_SDA();				//数据输出高电平   
	delay_us(5);				//以上步骤是对IIC总线复位，产生了一个IIC停止时序。预防引脚电平错误导致IIC总线读取数据出错
						
	iic_start();				//IIC总线起始信号
	iic_datasend(0x5f);			//地址0x2f + 读标志位，发送0x5f
	for(i=0;i<4;i++)
	{
                delay_us(30);			//读下个字节前需延时30us以上
		*(reg+i)=iic_datareceive();
		if(i<3)
		{iic_ack(1);}
		else
		{
			iic_ack(0);
			delay_us(30);
		}	
	}
	iic_stop();
}


//==============================================================================
//以下为IIC读取数据举例，仅供参考
//==============================================================================
unsigned char dt[4];
main()
{
	while (1)
	{	
                iic_read(dt);
                if((dt[0]+dt[1]+dt[2])==dt[3])
                {
                         
                }                                                          
	}
}





