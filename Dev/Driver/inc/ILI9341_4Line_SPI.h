
#ifndef __ILI9341_4LINE_SPI_H__
#define  __ILI9341_4LINE_SPI_H__

#include "GlobalDef.h"


//#include "stm32f10x_spi.h"
//-----------------------------SPI 总线配置--------------------------------------//
#define USE_HARDWARE_SPI     1 ////  1:Enable Hardware SPI;0:USE Soft SPI

//-------------------------屏幕物理像素设置--------------------------------------//
#define LCD_X_SIZE	        240
#define LCD_Y_SIZE	        320

/////////////////////////////////////用户配置区///////////////////////////////////	 
//支持横竖屏快速定义切换
#define USE_HORIZONTAL  		1	//定义是否使用横屏 		0,不使用.1,使用.

#if USE_HORIZONTAL    //如果定义了横屏 
#define X_MAX_PIXEL	        LCD_Y_SIZE
#define Y_MAX_PIXEL	        LCD_X_SIZE
#else
#define X_MAX_PIXEL	        LCD_X_SIZE
#define Y_MAX_PIXEL	        LCD_Y_SIZE
#endif

//////////////////////////////////////////////////////////////////////////////////
	 

#if 0
#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0

#define CYAN          	 0x7FFF
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define MAGENTA       	 0xF81F
#endif

#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111

#define BLACK           0x0000      /*   0,   0,   0     */
#define NAVY            0x000F      /*   0,   0, 128   */
#define DGREEN          0x03E0      /*   0, 128,   0   */
#define DCYAN           0x03EF      /*   0, 128, 128 */
#define MAROON          0x7800      /* 128,   0,   0 */
#define PURPLE          0x780F      /* 128,   0, 128 */
#define OLIVE           0x7BE0      /* 128, 128,   0 */
#define LGRAY           0xC618      /* 192, 192, 192 */
#define DGRAY           0x7BEF      /* 128, 128, 128 */
#define BLUE            0x001F      /*   0,   0, 255 */
#define GREEN           0x07E0      /*   0, 255,   0 */
#define CYAN            0x07FF      /*   0, 255, 255 */
#define RED             0xF800      /* 255,   0,   0 */
#define MAGENTA         0xF81F      /* 255,   0, 255 */
#define YELLOW          0xFFE0      /* 255, 255,   0 */
#define WHITE           0xFFFF      /* 255, 255, 255 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define BROWN                 0xBC40 //
#define BRRED                 0xFC07 //



// 以下色值需要转换
#define  GhostWhite      0xF8F8FF
#define  WhiteSmoke      0xF5F5F5
#define  Ivory           0xFFFFF0
#define  MintCream       0xF5FFFA

#define  CornflowerBlue  0x6495ED
#define  LightSteelBlue  0xB0C4DE
#define  LightSkyBlue    0x87CEFA
#define  SkyBlue         0x87CEEB
#define  DeepSkyBlue     0x00BFFF






//本测试程序使用的是模拟SPI接口驱动
//可自由更改接口IO配置，使用任意最少4 IO即可完成本款液晶驱动显示
/******************************************************************************
接口定义在Lcd_Driver.h内定义，请根据接线修改并修改相应IO初始化LCD_GPIO_Init()
#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
#define LCD_LED        	GPIO_Pin_9  //PB9 连接至TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10连接至TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 连接至TFT --CS
#define LCD_RST     	GPIO_Pin_12	//PB12连接至TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13连接至TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15连接至TFT - SDI
*******************************************************************************/
#define TFT_SPI_PORT  	  	GPIOB		//定义TFT数据端口

#define TFT_CS_PORT               GPIOB
#define TFT_DC_PORT              GPIOB
#define TFT_RESET_PORT      GPIOA

#define TFT_DC_PIN         	GPIO_Pin_8         // PB8 --->>TFT --RS/DC
#define TFT_CS_PIN         	GPIO_Pin_9         // MCU_PB9--->>TFT --CS/CE
#define TFT_RESET_PIN     GPIO_Pin_15	// PA15--->>TFT --RST
#define TFT_SCL_PIN           GPIO_Pin_3   	// PB3 --->>TFT --SCL/SCK
#define TFT_SDA_PIN          GPIO_Pin_5	       // PB5 MOSI--->>TFT --SDA/DIN
#define TFT_SDO_PIN          GPIO_Pin_4	       // MCU_PB4 MISO	----> TFT --SDO 

//#define TFT_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//液晶控制口置1操作语句宏定义
#define	LCD_CS_SET  	TFT_SPI_PORT->BSRR = TFT_CS_PIN     
#define	LCD_RS_SET  	TFT_DC_PORT->BSRR  = TFT_DC_PIN    
#define	LCD_SDA_SET  	TFT_SPI_PORT->BSRR = TFT_SDA_PIN     
#define	LCD_SCL_SET  	TFT_SPI_PORT->BSRR = TFT_SCL_PIN     
#define LCD_RST_SET     TFT_RESET_PORT->BSRR = TFT_RESET_PIN

//液晶控制口置0操作语句宏定义
#define	LCD_CS_CLR  	TFT_SPI_PORT->BRR = TFT_CS_PIN     
#define	LCD_RS_CLR  	TFT_DC_PORT->BRR  = TFT_DC_PIN     
#define	LCD_SDA_CLR  	TFT_SPI_PORT->BRR = TFT_SDA_PIN     
#define	LCD_SCL_CLR  	TFT_SPI_PORT->BRR = TFT_SCL_PIN     
#define LCD_RST_CLR     TFT_RESET_PORT->BRR = TFT_RESET_PIN 

#define TFT_DC_DATA      LCD_RS_SET   // 高电平选择数据
#define TFT_DC_COMMAND   LCD_RS_CLR   // 低电平选择命令

/**
 * error code
 */
typedef enum {
    TFT_SPI_SUCCESS = 0,                                   /**< success */
    TFT_SPI_ERR_NOT_FOUND = 1,                                /**< not found or not supported */
    TFT_SPI_ERR_WRITE = 2,                                    /**< write error */
    TFT_SPI_ERR_READ = 3,                                     /**< read error */
    TFT_SPI_ERR_TIMEOUT = 4,                                  /**< timeout error */
    TFT_SPI_ERR_ADDR_OUT_OF_BOUND = 5,                        /**< address is out of flash bound */
} tft_spi_err;







//void LCD_ShowString(u16 x, u16 y,const u8 *p);

extern uint32_t rp_err_cnt;  // 读错错误的点计数
extern uint32_t rp_ok_cnt;

#endif

