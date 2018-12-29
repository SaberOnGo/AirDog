
#include "ILI9341_4Line_SPI.h"
#include "delay.h"
//#include "font.h" 

/***************************************************************************************
STM32测试平台介绍:
开发板：正点原子MiniSTM32开发板
MCU ：STM32_F103_RBT6
晶振 ：12MHZ
主频 ：72MHZ
接线说明:
//-------------------------------------------------------------------------------------
#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
#define LCD_LED        	GPIO_Pin_9  //PB9 连接至TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10连接至TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 连接至TFT --CS
#define LCD_RST     	GPIO_Pin_12	//PB12连接至TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13连接至TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15连接至TFT - SDI
#define LCD_SDO        	GPIO_Pin_14	//MCU_PB14 MISO	
//VCC:可以接5V也可以接3.3V
//LED:可以接5V也可以接3.3V或者使用任意空闲IO控制(高电平使能)
//GND：接电源地
//说明：如需要尽可能少占用IO，可以将LCD_CS接地，LCD_LED接3.3V，LCD_RST接至单片机复位端，
//将可以释放3个可用IO
//接口定义在Lcd_Driver.h内定义，
//如需变更IO接法，请根据您的实际接线修改相应IO初始化LCD_GPIO_Init()
//-----------------------------------------------------------------------------------------
例程功能说明：
1.	简单刷屏测试
2.	英文显示测试示例
3.	中文显示测试示例
4.	数码管字体显示示例
5.	图片显示示例
6.	2D按键菜单示例
7.	本例程支持横屏/竖屏切换(开启宏USE_HORIZONTAL,详见Lcd_Driver.h)
8.	本例程支持软件模拟SPI/硬件SPI切换(开启宏USE_HARDWARE_SPI,详见Lcd_Driver.h)
**********************************************************************************************/

u16  Lcd_ReadPoint(u16 x,u16 y);
void Lcd_Clear(u16 Color);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
void Lcd_SetRegion(u16 x1, u16 y1, u16 x2, u16 y2);

//---------------------------------function----------------------------------------------------//

/****************************************************************************
* 名    称：void LCD_GPIO_Init(void)
* 功    能：STM32_模拟SPI所用到的GPIO初始化
* 入口参数：无
* 出口参数：无
* 说    明：初始化模拟SPI所用的GPIO
****************************************************************************/
void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TFT_DC_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(TFT_DC_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TFT_RESET_PIN;
	GPIO_Init(TFT_RESET_PORT, &GPIO_InitStructure);
      
}
/****************************************************************************
* 名    称：void  SPIv_WriteData(u8 Data)
* 功    能：STM32_模拟SPI写一个字节数据底层函数
* 入口参数：Data
* 出口参数：无
* 说    明：STM32_模拟SPI读写一个字节数据底层函数
****************************************************************************/
void  SPIv_WriteData(u8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
	  LCD_SDA_SET; //输出数据
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

/****************************************************************************
* 名    称：u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* 功    能：STM32_硬件SPI读写一个字节数据底层函数
* 入口参数：SPIx,Byte
* 出口参数：返回总线收到的数据
* 说    明：STM32_硬件SPI读写一个字节数据底层函数
****************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
{
	while((SPIx->SR&SPI_I2S_FLAG_TXE) == RESET);		//等待发送区空	  
	SPIx->DR = Byte;	 	//发送一个byte   
	while( (SPIx->SR&SPI_I2S_FLAG_RXNE) == RESET);     //等待接收完一个byte  
	return SPIx->DR;          	     //返回收到的数据			
} 

/**
 * retry process
 *
 * @param delay delay function for every retry. NULL will not delay for every retry.
 * @param retry retry counts
 * @param result SFUD_ERR_TIMEOUT: retry timeout
 */
#define TFT_SPI_RETRY_PROCESS(delay, retry, result)                               \
    { void (*__delay_temp)(void) = (void (*)(void))delay;                        \
    if (retry == 0) {result = TFT_SPI_ERR_TIMEOUT;break;}                         \
    else {if (__delay_temp) {__delay_temp();} retry --;}}

tft_spi_err spi_write_read(SPI_TypeDef* SPIx, uint8_t send_data, uint8_t * read_data) 
{
    uint32_t retry_times = 0;
    tft_spi_err result = TFT_SPI_SUCCESS;
    uint8_t tmp = 0;
		
   // LCD_CS_CLR;
	
    /* 发送数据 */
    retry_times = 8000;
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET) {
        TFT_SPI_RETRY_PROCESS(NULL, retry_times, result);
    }
    if (result != TFT_SPI_SUCCESS) {
        goto exit;
    }
    SPIx->DR = send_data;
		
    /* 接收数据 */
	retry_times = 8000;
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET) {
	     TFT_SPI_RETRY_PROCESS(NULL, retry_times, result);
	}
	if (result != TFT_SPI_SUCCESS) {
	     goto exit;
	}
	tmp = SPIx->DR;
	if(read_data)*read_data = tmp;
exit:
    //LCD_CS_SET;

    return result;
}




/****************************************************************************
* 名    称：void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
* 功    能：设置SPI的速度
* 入口参数：SPIx,SpeedSet
* 出口参数：无
* 说    明：SpeedSet:1,高速;0,低速;
****************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
{
	SPIx->CR1&=0XFFC7;
	if(SpeedSet==1)//高速
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_2;//Fsck=Fpclk/2	
	}
	else//低速
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_32; //Fsck=Fpclk/32
	}
	SPIx->CR1|=1<<6; //SPI设备使能
} 

/****************************************************************************
* 名    称：SPI3_Init(void)
* 功    能：STM32_SPI2硬件配置初始化
* 入口参数：无
* 出口参数：无
* 说    明：STM32_SPI2硬件配置初始化
****************************************************************************/
	void SPI3_Init(void)	
	{
		SPI_InitTypeDef  SPI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		 
		//配置SPI3 管脚
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;	  
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = TFT_DC_PIN;
		GPIO_Init(TFT_DC_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = TFT_RESET_PIN;
		GPIO_Init(TFT_RESET_PORT, &GPIO_InitStructure);
		
		//SPI3 配置选项
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3 ,ENABLE);
		   
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

		// 预分频 = 2 , SPI 速率 (最大为18 Mbit/s) = 48M / 2 = 24M
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //SPI_BaudRatePrescaler_2;  // PCLK2 = HCLK = 48MHz
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(SPI3, &SPI_InitStructure);
	
		//使能SPI3
		SPI_Cmd(SPI3, ENABLE);	 
	}



/****************************************************************************
* 名    称：Lcd_WriteIndex(u8 Index)
* 功    能：向液晶屏写一个8位指令
* 入口参数：Index   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
   LCD_CS_CLR;
   TFT_DC_COMMAND;
#if USE_HARDWARE_SPI   
   SPI_WriteByte(SPI3, Index);
   //spi_write_read(SPI3, Index, NULL);
#else
   SPIv_WriteData(Index);
#endif 
   LCD_CS_SET;
}

/****************************************************************************
* 名    称：Lcd_WriteData(u8 Data)
* 功    能：向液晶屏写一个8位数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，内部函数
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
   LCD_CS_CLR;
   TFT_DC_DATA;
#if USE_HARDWARE_SPI   
   SPI_WriteByte(SPI3, Data);
   //spi_write_read(SPI3, Data, NULL);
#else
   SPIv_WriteData(Data);
#endif 
   LCD_CS_SET;
}

/****************************************************************************
* 名    称：void LCD_WriteReg(u8 Index,u16 Data)
* 功    能：写寄存器数据
* 入口参数：Index,Data
* 出口参数：无
* 说    明：本函数为组合函数，向Index地址的寄存器写入Data值
****************************************************************************/
#if 0
void LCD_WriteReg(u8 Index,u16 Data)
{
	Lcd_WriteIndex(Index);
  	Lcd_WriteData_16Bit(Data);
}
#endif

/****************************************************************************
* 名    称：void Lcd_WriteData_16Bit(u16 Data)
* 功    能：向液晶屏写一个16位数据
* 入口参数：Data
* 出口参数：无
* 说    明：向控制器指定地址写入一个16位数据
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{	
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);	
}

/****************************************************************************
* 名    称：void Lcd_Reset(void)
* 功    能：液晶硬复位函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化前需执行一次复位操作
****************************************************************************/
void Lcd_Reset(void)
{
       LCD_RST_CLR;   // 低电平复位
	delay_ms(100);
	LCD_RST_SET;   // 高电平使能
	delay_ms(100);
}
/****************************************************************************
* 名    称：void Lcd_Init(void)
* 功    能：液晶初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化_ILI9225_176X220
****************************************************************************/
void LCD_Setup(void)
{	
#if USE_HARDWARE_SPI //使用硬件SPI
	SPI3_Init();
#else	
	LCD_GPIO_Init(); //使用模拟SPI
#endif
	Lcd_Reset(); //Reset before LCD Init.

#if 1
	Lcd_WriteIndex(0xCB);  
        Lcd_WriteData(0x39); 
        Lcd_WriteData(0x2C); 
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x34); 
        Lcd_WriteData(0x02); 

        Lcd_WriteIndex(0xCF);  
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0XC1); 
        Lcd_WriteData(0X30); 
 
        Lcd_WriteIndex(0xE8);  
        Lcd_WriteData(0x85); 
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x78); 
 
        Lcd_WriteIndex(0xEA);  
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x00); 
 
        Lcd_WriteIndex(0xED);  
        Lcd_WriteData(0x64); 
        Lcd_WriteData(0x03); 
        Lcd_WriteData(0X12); 
        Lcd_WriteData(0X81); 

        Lcd_WriteIndex(0xF7);  
        Lcd_WriteData(0x20); 
  
        Lcd_WriteIndex(0xC0);    //Power control 
        Lcd_WriteData(0x23);   //VRH[5:0] 
 
        Lcd_WriteIndex(0xC1);    //Power control 
        Lcd_WriteData(0x10);   //SAP[2:0];BT[3:0] 
 
        Lcd_WriteIndex(0xC5);    //VCM control 
        Lcd_WriteData(0x3e); //对比度调节
        Lcd_WriteData(0x28); 
 
        Lcd_WriteIndex(0xC7);    //VCM control2 
        Lcd_WriteData(0x86);  //--
 
        Lcd_WriteIndex(0x36);    // Memory Access Control 
#ifdef USE_HORIZONTAL
        Lcd_WriteData(0x28); //C8	   //48 68竖屏//28 E8 横屏
#else
		Lcd_WriteData(0x48); 
#endif

        Lcd_WriteIndex(0x3A);    
        Lcd_WriteData(0x55); 

        Lcd_WriteIndex(0xB1);    
        Lcd_WriteData(0x00);  
        Lcd_WriteData(0x18); 
 
        Lcd_WriteIndex(0xB6);    // Display Function Control 
        Lcd_WriteData(0x08); 
        Lcd_WriteData(0x82);
        Lcd_WriteData(0x27);  
 
        Lcd_WriteIndex(0xF2);    // 3Gamma Function Disable 
        Lcd_WriteData(0x00); 
 
        Lcd_WriteIndex(0x26);    //Gamma curve selected 
        Lcd_WriteData(0x01); 
 
        Lcd_WriteIndex(0xE0);    //Set Gamma 
        Lcd_WriteData(0x0F); 
        Lcd_WriteData(0x31); 
        Lcd_WriteData(0x2B); 
        Lcd_WriteData(0x0C); 
        Lcd_WriteData(0x0E); 
        Lcd_WriteData(0x08); 
        Lcd_WriteData(0x4E); 
        Lcd_WriteData(0xF1); 
        Lcd_WriteData(0x37); 
        Lcd_WriteData(0x07); 
        Lcd_WriteData(0x10); 
        Lcd_WriteData(0x03); 
        Lcd_WriteData(0x0E); 
        Lcd_WriteData(0x09); 
        Lcd_WriteData(0x00); 

        Lcd_WriteIndex(0XE1);    //Set Gamma 
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x0E); 
        Lcd_WriteData(0x14); 
        Lcd_WriteData(0x03); 
        Lcd_WriteData(0x11); 
        Lcd_WriteData(0x07); 
        Lcd_WriteData(0x31); 
        Lcd_WriteData(0xC1); 
        Lcd_WriteData(0x48); 
        Lcd_WriteData(0x08); 
        Lcd_WriteData(0x0F); 
        Lcd_WriteData(0x0C); 
        Lcd_WriteData(0x31); 
        Lcd_WriteData(0x36); 
        Lcd_WriteData(0x0F); 
 
        Lcd_WriteIndex(0x11);    //Exit Sleep 
        delay_ms(120); 
				
        Lcd_WriteIndex(0x29);    //Display on 
        Lcd_WriteIndex(0x2c); 
	
        Lcd_Clear(WHITE);//清屏	
 #else
 //2.2inch TM2.2-G2.2 Init 20171020 
	 Lcd_WriteIndex(0x11);	
	 Lcd_WriteData(0x00); 
 
	 Lcd_WriteIndex(0xCF);	
	 Lcd_WriteData(0X00); 
	 Lcd_WriteData(0XC1); 
	 Lcd_WriteData(0X30);
 
	 Lcd_WriteIndex(0xED);	
	 Lcd_WriteData(0X64); 
	 Lcd_WriteData(0X03); 
	 Lcd_WriteData(0X12);
	 Lcd_WriteData(0X81);
 
	 Lcd_WriteIndex(0xE8);	
	 Lcd_WriteData(0X85); 
	 Lcd_WriteData(0X11); 
	 Lcd_WriteData(0X78);
 
	 Lcd_WriteIndex(0xF6);	
	 Lcd_WriteData(0X01); 
	 Lcd_WriteData(0X30); 
	 Lcd_WriteData(0X00);
 
	 Lcd_WriteIndex(0xCB);	
	 Lcd_WriteData(0X39); 
	 Lcd_WriteData(0X2C); 
	 Lcd_WriteData(0X00);
	 Lcd_WriteData(0X34);
	 Lcd_WriteData(0X05);
 
	 Lcd_WriteIndex(0xF7);	
	 Lcd_WriteData(0X20); 
 
	 Lcd_WriteIndex(0xEA);	
	 Lcd_WriteData(0X00); 
	 Lcd_WriteData(0X00); 
 
	 Lcd_WriteIndex(0xC0);	
	 Lcd_WriteData(0X20); 
 
	 Lcd_WriteIndex(0xC1);	
	 Lcd_WriteData(0X11); 
 
	 Lcd_WriteIndex(0xC5);	
	 Lcd_WriteData(0X31); 
	 Lcd_WriteData(0X3C); 
 
	 Lcd_WriteIndex(0xC7);	
	 Lcd_WriteData(0XA9); 
 
	 Lcd_WriteIndex(0x3A);	
	 Lcd_WriteData(0X55); 
	 
	 Lcd_WriteIndex(0x36);	
#if USE_HORIZONTAL
	 Lcd_WriteData(0xE8);//横屏参数
#else
	 Lcd_WriteData(0x48);//竖屏参数 
#endif
 
	 Lcd_WriteIndex(0xB1);	
	 Lcd_WriteData(0X00); 
	 Lcd_WriteData(0X18); 
 
	 Lcd_WriteIndex(0xB4);	
	 Lcd_WriteData(0X00); 
	 Lcd_WriteData(0X00); 
 
	 Lcd_WriteIndex(0xF2);	
	 Lcd_WriteData(0X00); 
 
	 Lcd_WriteIndex(0x26);	
	 Lcd_WriteData(0X01); 
 
	 Lcd_WriteIndex(0xE0);	
	 Lcd_WriteData(0X0F); 
	 Lcd_WriteData(0X17); 
	 Lcd_WriteData(0X14); 
	 Lcd_WriteData(0X09); 
	 Lcd_WriteData(0X0C); 
	 Lcd_WriteData(0X06); 
	 Lcd_WriteData(0X43); 
	 Lcd_WriteData(0X75); 
	 Lcd_WriteData(0X36); 
	 Lcd_WriteData(0X08); 
	 Lcd_WriteData(0X13); 
	 Lcd_WriteData(0X05); 
	 Lcd_WriteData(0X10); 
	 Lcd_WriteData(0X0B); 
	 Lcd_WriteData(0X08); 
 
 
	 Lcd_WriteIndex(0xE1);	
	 Lcd_WriteData(0X00); 
	 Lcd_WriteData(0X1F); 
	 Lcd_WriteData(0X23); 
	 Lcd_WriteData(0X03); 
	 Lcd_WriteData(0X0E); 
	 Lcd_WriteData(0X04); 
	 Lcd_WriteData(0X39); 
	 Lcd_WriteData(0X25); 
	 Lcd_WriteData(0X4D); 
	 Lcd_WriteData(0X06); 
	 Lcd_WriteData(0X0D); 
	 Lcd_WriteData(0X0B); 
	 Lcd_WriteData(0X33); 
	 Lcd_WriteData(0X37); 
	 Lcd_WriteData(0X0F); 
 
	 Lcd_WriteIndex(0x29);	 

 #endif
}



/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData_16Bit(Xpos);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData_16Bit(Ypos);
	Lcd_WriteIndex(0x2c);	
} 
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
//设置显示窗口
void Lcd_SetRegion(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData_16Bit(xStar);
	Lcd_WriteData_16Bit(xEnd);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData_16Bit(yStar);
	Lcd_WriteData_16Bit(yEnd);
	Lcd_WriteIndex(0x2c);
}

uint32_t rp_err_cnt = 0;  // 读错错误的点计数
uint32_t rp_ok_cnt   = 0;

/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：xy坐标和颜色数据
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
    
    uint16_t rp = 0;
	
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);

    rp =  Lcd_ReadPoint(x, y);

	if(Data != rp)
	{
	   rp_err_cnt++;
	}
}    

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)				
{	
   unsigned int i;
   Lcd_SetRegion(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1);
   LCD_CS_CLR;
   TFT_DC_DATA;
   for(i = 0; i < X_MAX_PIXEL * Y_MAX_PIXEL; i++)
   {	
		//Lcd_WriteData_16Bit(Color);
#if USE_HARDWARE_SPI   
		SPI_WriteByte(SPI3, Color>>8);
		SPI_WriteByte(SPI3, Color);
		//spi_write_read(SPI3, Color >> 8, NULL);
		//spi_write_read(SPI3, Color >> 8, NULL);
#else
		SPIv_WriteData(Color>>8);
		SPIv_WriteData(Color);
		delay_us(1);
#endif 
   }   
   LCD_CS_SET;
}





u8 Lcd_ReadData(u8 Data)
{
	u8 temp = 0;
	LCD_CS_CLR;
       TFT_DC_DATA; 
       temp = SPI_WriteByte(SPI3, Data);
 // spi_write_read(SPI3, Data, &temp);
 // LCD_CS_SET;
	return temp;
}

u16 Lcd_ReadPoint(u16 x,u16 y)
{

	u8 r = 0, g = 0, b = 0;
	u16 R,G,B,Data;
	Lcd_SetRegion(x,y,x,y);  
	Lcd_WriteIndex(0X2E); 

       Lcd_ReadData(0xff);
       Lcd_ReadData(0xff);
	g=Lcd_ReadData(0xff); 
	b=Lcd_ReadData(0xff); 
	r=Lcd_ReadData(0xff); //独读出的数据是按照GBR顺序的

	R = (r >> 3) & 0x00ff;
	G = (g >> 2) & 0x00ff;
	B = (b >> 3) & 0x00ff;
	
	Data = (R << 11) | (G << 5) | (B);
	return Data;
}

//在指定位置显示一个字符
//x:0~234
//y:0~308
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
//在指定位置显示一个字符
//x:0~234
//y:0~308
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size, u8 mode, uint16_t data)
{  
    #if 0
    u8 temp;
    u8 pos,t;
	u16 x0 = x;
	u16 colortemp; 
	uint16_t read = 0;
	
    if(x > X_MAX_PIXEL || y > Y_MAX_PIXEL)
	{
       os_printf("param err: %s, %d\n", __FILE__, __LINE__);
	   return;
	}	    
	
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)colortemp = data;
				else colortemp = WHITE;
				//os_printf("draw = 0x%x\n", colortemp);
				Gui_DrawPoint(x, y, colortemp);	
				//read = Lcd_ReadPoint(x, y);
				//os_printf("read = 0x%x\n", read);
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}
	else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)Gui_DrawPoint(x+t, y+pos, data);//画一个点     
		        temp>>=1; 
		    }
		}
	}  	   	 
	#endif
}   

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
/*
void LCD_ShowString(u16 x, u16 y,const u8 *p)
{         
    while(*p!='\0')
    {       
        if(x>X_MAX_PIXEL){x=0;y+=16;}
        if(y>Y_MAX_PIXEL){y=x=0;Lcd_Clear(WHITE);}
        LCD_ShowChar(x,y,*p,16,0, RED);
        x += 8;
        p++;
    }  
}
*/

