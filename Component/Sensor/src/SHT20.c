
#include "SHT20.h"
#include "IIC_Drv.h"
#include "os_timer.h"
#include "sensor.h"
//#include "SDRR.h"


#include "fifo_queue.h"



#if SHT_DEBUG_EN
#include "debugfile.h"
//#define SHT_DBG(fmt, ...)   printf(fmt, ##__VA_ARGS__)
#define  SHT_DBG  dbg_print_detail
#else
#define SHT_DBG(...)
#endif



#ifdef USING_NOS_TIMER 
#define SHT_USING_NOS_TIMER   1   // SHT20 的软件定时器 使用 ucos 的定时器
#endif

#define SHT20_I2C_ADDR  0x80    // SHT20 I2C设备地址



#define SHT_T_Hold_CMD        0xE3   // 温度测量, 保持主机
#define SHT_RH_Hold_CMD       0xE5   // 湿度测量, 保持主机

#define SHT_T_NotHold_CMD     0xF3   // 温度测量, 非保持主机
#define SHT_RH_NotHold_CMD    0xF5   // 湿度测量, 非保持主机

#define SHT_WriteReg_CMD      0xE6   // 写用户寄存器
#define SHT_ReadReg_CMD       0xE7   // 读用户寄存器

#define SHT_SoftReset_CMD     0xFE   // 软复位

// 数据寄存器位定义
#define SHT_DATA_REG_STA_BitMask       0x02  // 数据字节的低八位的状态位(bit 1)掩码

#define SHT_DATA_REG_STA_Bit_IsTemp    0x00  // bit1: 0
#define SHT_DATA_REG_STA_Bit_IsHumi    0x02  // bit1: 1

// 用户寄存器位定义

//温湿度测量精度设置的掩码位
#define SHT_USER_REG_RH_T_BitMask       0x81   // 测量设置掩码： bit7, bit0

#define SHT_USER_REG_RH12_T14   0x00   // 湿度测量 12 bit, 温度测量 14bit
#define SHT_USER_REG_RH8_T12    0x01   // 湿度测量8bit, 温度测量 12bit
#define SHT_USER_REG_RH10_T13   0x80   // 湿度测量 10bit, 温度测量 13bit
#define SHT_USER_REG_RH11_T11   0x81   // 湿度测量 11bit, 温度测量 11bit 

// 电池状态掩码位, bit6
#define SHT_USER_REG_EndOfBat_BitMask         0x40   

#define SHT_USER_REG_EndOfBat_MoreThan2P25V    0x00   // 电池电压大于 2.25 V
#define SHT_USER_REG_EndOfBat_LessThan2P25V    0x40   // 电池电压小于 2.25 V

// 片上加热器状态掩码位
#define SHT_USER_REG_Heat_BitMask   0x04             

#define SHT_USER_REG_Heat_OnLine    0x04             // 在加热
#define SHT_USER_REG_Heat_OffLine   0x00             // 无加热



#if SHT_DEBUG_EN
static uint32_t  read_sht20_err_count = 0;
static uint32_t read_sht20_ok_cnt = 0;
#endif

//static uint8_t first_temp_save = E_FALSE;

#if SHT_DEBUG_EN
//static int16_t first_temp;   // unit: 'C
#endif

extern uint16_t ADCDrv_GetFirstCoreTemp(void);
extern uint16_t ADCDrv_GetCurCoreTemp(void);


// SHT 20 启动加热
// SHT 20 的加热器用于诊断 SHT20 是否损坏
//参数: uint16_t * temp: 之前的平均温度值, 为实际温度值的10倍, 即 350 = 35.0 'C
//      uint16_t * humi: 之前的平均湿度值, 如 35 = 35 % RH
//static uint16_t last_temp;
//static uint16_t last_humi;

//static 
//os_timer_t tTimerHeating;

static os_timer_t tTimerSHT20;



IIC_BUS sht_bus = 
{
        #if  0
        { GPIO_Pin_7,  GPIOA  },    // SDA
        { GPIO_Pin_6,  GPIOA  },    // SCL
        #else
        { GPIO_Pin_0,  GPIOA  },    // SDA
        { GPIO_Pin_2,  GPIOA  },    // SCL
        #endif
};


SYS_RESULT  SHT_ReadNByteDirectly(uint8_t * p, uint8_t size)
{
        return IIC_ReadNByteDirectly(&sht_bus,  SHT20_I2C_ADDR,  p, size);
}

SYS_RESULT SHT_WriteNByte(uint16_t data_addr,  uint8_t *  p,  uint8_t size)
{
       return IIC_WriteNByte(&sht_bus,  SHT20_I2C_ADDR, data_addr,  p,  size);
}

SYS_RESULT  SHT_ReadNByteExt(uint16_t data_addr,  uint8_t * p,  uint8_t size, uint8_t restart_iic)
{
       return IIC_ReadNByteExt(&sht_bus, SHT20_I2C_ADDR,  data_addr, p,  size, restart_iic);
}


SYS_RESULT  SHT2x_SoftReset(void)
{
      uint8_t p = 0;
      
      return  SHT_WriteNByte(SOFT_RESET,  &p, 0);
}

#define TEMP_INDEX 0
#define HUMI_INDEX 1

#define READ_TIMES    4   // 读取次数
static uint16_t sht_buf[2][READ_TIMES];  
static uint8_t temp_read_count = 0;
static uint8_t humi_read_count = 0;
static uint8_t sht20_read_count = 0; 
static uint8_t next_read_which = 0; // 下一次读温度 0, 还是湿度: 1


//  CRC
const  uint16_t POLYNOMIAL = 0x131;  //P(x)=x^8+x^5+x^4+1 = 100110001

SYS_RESULT SHT20_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
{
  uint8_t crc = 0;	
  uint8_t byteCtr;
  uint8_t bit;
  
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  {
        crc ^= (data[byteCtr]);
        for (bit = 8; bit > 0; --bit)
        {
              if (crc & 0x80) 
                      crc = (crc << 1) ^ POLYNOMIAL;
              else 
                    crc = (crc << 1);
        }
   }
   if (crc != checksum) return SYS_FAILED;
   else return SYS_SUCCESS;
}


// 公式: T = - 46.85 + (175.72 * T) / (2 ^ 16)
// 返回值: -1 说明温度值 < 0, 1: 温度值 > 0
int8_t SHT20_CalculateTemp(uint16_t adc, uint16_t * out_temp)
{
   double real_temp = 0.0;
   int8_t sign = 0;  

   adc &= 0xFFFC;  // 最后2位是标志位, 丢弃
   real_temp = ((175.72 * adc) / 65536.0) - 46.85;
   if(real_temp < 0)
   {
         sign = -1;
	  real_temp = -real_temp;  
   }
   *out_temp = (uint16_t)(real_temp * 100);
   return sign;
}

int8_t SHT20_CalculateHumi(uint16_t adc, uint16_t *out_humi)
{
    int8_t sign = 0;
	double real_humi = 0.0;

	adc &= 0xFFFC;
	real_humi = (125.0 * adc) / 65536.0 - 6;
	if(real_humi < 0)
	{
	    sign = -1;
		real_humi = -real_humi;
	}
	*out_humi = (uint16_t)(real_humi * 100);
	return sign;
}



uint16_t SHT_CalculateTemp(uint16_t tmp)
{
       uint8_t i;
       bool res;
       uint16_t val = 0;
       uint32_t total = 0;
       uint16_t avr = 0;
       
       if(pipe_is_full())
       {
               pipe_read_word(&val);
       }
       pipe_write_word(tmp);

       for(i = 0; i < 16; val = 0, i++)
       {
            res = pipe_peek_word(&val);
            if(! res)break;
            total += val;
      }
      avr = total / i;
      
	return avr;
}

static 
void TimerSHT20Sensor_CallBack(void * arg)
{
	SYS_RESULT res;
	SYS_RESULT chk_res;
	
       uint8_t buf[3] = {0, 0, 0};
	uint8_t val_type = 0;  // 值类型: 温度: 0; 湿度: 1
	uint8_t reg_val = 0;
	int8_t  sign = 0;
	uint16_t temp_humi = 0;
       
	res = SHT_ReadNByteDirectly(buf, 3);
	chk_res = SHT20_CheckCrc(buf, 2, buf[2]);
       if(res || chk_res)  // IIC error or check crc error
      {
            os_timer_arm(&tTimerSHT20, 200, 0);   // after 2 sec

#if SHT_DEBUG_EN
            read_sht20_err_count++;
            SHT_DBG("read sht20 failed: iic_res = %d, chk_res = %d ",  res, chk_res);
            SHT_DBG("\t tick = %ld, err_count = %ld\r\n", os_get_tick(), read_sht20_err_count);
#endif

            return;    // 读取失败, 直接返回
    }
   else
   {
                val_type = ((buf[1] & SHT_DATA_REG_STA_BitMask) ? HUMI_INDEX : TEMP_INDEX);
               
#if SHT_DEBUG_EN
                read_sht20_ok_cnt++;
                SHT_DBG("read SHT20 %s success, tick = %ld, err_count = %ld, ok_cnt = %ld\r\n", (val_type ? "humi" : "temp"), 
                os_get_tick(), read_sht20_err_count, read_sht20_ok_cnt);
                SHT_DBG("buf[0] = 0x%x, buf[1] = 0x%x, buf[2] = 0x%x\r\n", buf[0], buf[1], buf[2]);
#endif

                if(val_type)  // 1: 为湿度值
                {
                            sht_buf[val_type][humi_read_count] = ((uint16_t)buf[0] << 8) + buf[1];   // 保存湿度值
                            sign = SHT20_CalculateHumi(sht_buf[val_type][humi_read_count], &temp_humi);

#if SHT_DEBUG_EN
                              SHT_DBG("humi = 0x%x, %c%d.%02d%% RH \r\n", sht_buf[val_type][humi_read_count], 
                                                         (sign == 0 ? ' ' : '-'), temp_humi / 100, temp_humi);
                             SHT_DBG("humi count = %d\r\n", humi_read_count);
#endif

                             //tTempHumi.humi = temp_humi;


                            temp_humi     = temp_humi % 10000 / 100;
                            SDRR_SaveSensorPoint(SENSOR_HUMI, &temp_humi);  // 刷新湿度数据点
                            SnsGUI_DisplayHumi(temp_humi);

                           
                            
                            if(++humi_read_count >= READ_TIMES)humi_read_count = 0;
                            if(next_read_which == HUMI_INDEX)next_read_which = TEMP_INDEX;
                }
                else  // 0: 为温度值
                {
                           sht_buf[val_type][temp_read_count] = ((uint16_t)buf[0] << 8) + buf[1]; // 保持温度值 
                           sign = SHT20_CalculateTemp(sht_buf[val_type][temp_read_count], &temp_humi);
           
            #if SHT_DEBUG_EN
                            SHT_DBG("temp = 0x%x, %c%d.%02d 'C \r\n", sht_buf[val_type][temp_read_count], 
                                                     (sign == 0 ? ' ' : '-'), temp_humi / 100, temp_humi % 100);
                            SHT_DBG("temp count = %d\r\n", temp_read_count);
           #endif
           
       

                           do
                          {
                                  int16_t temp;

                                  if(sign < 0)temp = -temp_humi;
                                  else{ temp = temp_humi;  }
                                  temp -= 200;  // 减去2度
                                  SnsGUI_DisplayTemp(temp);

                                  if(sign < 0)temp =- (temp_humi / 10);
                                  else { temp = temp_humi / 10; }
                                  temp -= 20;  // 减去2度
                                  SDRR_SaveSensorPoint(SENSOR_TEMP, &temp );  // 刷新温度数据点
                          }while(0);
                         
                             
     
                            
                           if(++temp_read_count >= READ_TIMES)temp_read_count = 0;
                           if(next_read_which == TEMP_INDEX)next_read_which = HUMI_INDEX;
                }

                // TempHumi_SetSensorExisted(E_TRUE);
                sht20_read_count++;
                if(sht20_read_count  >= (READ_TIMES * 2))
                {
                          sht20_read_count = 0;
                }


                if(next_read_which == TEMP_INDEX)  // 下一次需要读温度
                {
                          SHT_WriteNByte(SHT_T_NotHold_CMD, &reg_val,  0);     // 启动温度测量
                }
                else
                {
                         SHT_WriteNByte(SHT_RH_NotHold_CMD, &reg_val, 0);     // 启动湿度测量
                }
                os_timer_arm(&tTimerSHT20, 135, 0);  // 1.35 sec 测量一次, 不宜太频繁, 否则芯片自身会升温
	}
}


/********************************
功能: SHT20 温湿度寄存器配置
参数: uint8_t precision_mask: 温湿度精度配置, 值为:
                         SHT_USER_REG_RH12_T14
                         SHT_USER_REG_RH8_T12
                         SHT_USER_REG_RH10_T13
                         SHT_USER_REG_RH11_T11
             uint8_t is_heated: 是否加热

*********************************/
void SHT20_RegConfig(uint8_t precision_mask, uint8_t is_heated)
{
	uint8_t reg_val = 0;
    uint8_t res = 0;
    uint8_t old_config = 0; // 原配置

    if(   precision_mask != SHT_USER_REG_RH12_T14
	   && precision_mask != SHT_USER_REG_RH8_T12
	   && precision_mask != SHT_USER_REG_RH10_T13
	   && precision_mask != SHT_USER_REG_RH11_T11)
    {
             SHT_DBG("err: %s %d\r\n",  __FILE__, __LINE__);
    }
	
    res = SHT_ReadNByteExt(SHT_ReadReg_CMD, &reg_val, 1, 1);
    if(res){ SHT_DBG("res err: %d %s %d\r\n", res, __FILE__, __LINE__);   return; }
	
	#if SHT_DEBUG_EN
    SHT_DBG("first read user_reg = 0x%x, line = %d\n", reg_val, __LINE__);
    #endif
	
	old_config = reg_val & SHT_USER_REG_RH_T_BitMask;
	if(old_config != precision_mask)
	{
	        #if SHT_DEBUG_EN
	       SHT_DBG("sht precision change: old = 0x%x, new = 0x%x\r\n", old_config, precision_mask);
		#endif
		
           reg_val &= ~SHT_USER_REG_RH_T_BitMask;  // 设置为 RH 12bit, T 14 bit
	    if(precision_mask)
	    {
	           reg_val |= precision_mask;
	    }
	}
    
	old_config  = reg_val & SHT_USER_REG_Heat_OnLine;
	if(old_config != is_heated)
	{
	   #if SHT_DEBUG_EN
	   SHT_DBG("sht heat config change: old = %d, new = %d\r\n", old_config, is_heated);
	   #endif
	   
	   if(is_heated)
           reg_val |= SHT_USER_REG_Heat_OnLine;   // 启动片上加热器
       else
	       reg_val &= ~SHT_USER_REG_Heat_OnLine;  // 停止片上加热器
	}
	   
    res = SHT_WriteNByte(SHT_WriteReg_CMD,  &reg_val,  1);
    if(res){ SHT_DBG("res err: %d %s %d\r\n", res, __FILE__, __LINE__);   return; }
    
    reg_val = 0;
    res = SHT_ReadNByteExt(SHT_ReadReg_CMD, &reg_val, 1, 1);
    if(res){ SHT_DBG("res err: %d %s %d\r\n", res, __FILE__, __LINE__);   return; }
	
     #if SHT_DEBUG_EN
     SHT_DBG("reread user_reg = 0x%x, line = %d\n", reg_val, __LINE__);
    #endif
	
    if((reg_val & SHT_USER_REG_RH_T_BitMask) == precision_mask)  
    {
 	  SHT_WriteNByte(SHT_T_NotHold_CMD, &reg_val, 0);  // 启动温度测量
    }
    #if SHT_DEBUG_EN
    else
    {
 	  SHT_DBG("SHT RegConfig precision mask Failed, user_reg = 0x%x, tick = %ld, %s, %d\r\n", 
 				  reg_val, os_get_tick(), __FILE__, __LINE__);
    }
	
	if(is_heated)
	{
	    if((reg_val & SHT_USER_REG_Heat_BitMask) == 0)  // 需要加热, 但回读值却为不加热, 寄存器设置错误
	    {
	       SHT_DBG("SHT RegConfig Heat Failed, user_reg = 0x%x, tick = %ld, %s, %d\r\n", 
 				  reg_val, os_get_tick(), __FILE__, __LINE__);
	    }
	}
	else  // 不需要加热
	{
	    if((reg_val & SHT_USER_REG_Heat_BitMask))  
	    {
	       SHT_DBG("SHT RegConfig Cancel Heat Failed, user_reg = 0x%x, tick = %ld, %s, %d\r\n", 
 				  reg_val, os_get_tick(), __FILE__, __LINE__);
	    }
	}
	#endif
}

void SHT20_Init(void)
{
       sht_queue_init();
       
       IIC_Init(&sht_bus);
       SHT20_RegConfig(SHT_USER_REG_RH12_T14, 0);
       os_timer_setfn(&tTimerSHT20, TimerSHT20Sensor_CallBack, NULL);
	os_timer_arm(&tTimerSHT20,   500, 0);
}

