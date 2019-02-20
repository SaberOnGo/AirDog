

#include "sensor.h"
#include "board_version.h"
#include "os_timer.h"
#include "co2.h"
#include "Uart_Drv.h"
#include "os_global.h"
//#include "debug_queue.h"
#include "SnsGUI.h"



#if CO2_DBG_EN
//#define CO2_DBG(fmt, ...)    os_printf(fmt, ##__VA_ARGS__)
#define CO2_DBG    dbg_print_detail
#else
#define CO2_DBG(...)
#endif


static void InvertUint8(uint8_t *dBuf, uint8_t * srcBuf)  
{  
    uint16_t i;  
    uint8_t tmp = 0;  
    for(i = 0; i < 8; i++)  
    {  
      if((*srcBuf) & (1 << i))  
         tmp |= 1 << ( 7 - i);  
    }  
    *dBuf = tmp;      
}  

// 反转uint16_t 值
// 参数: uint16_t * dstBus: 输出结果
//       uint16_t * srcBuf: 输入数据
static void InvertUint16(uint16_t * dstBuf, uint16_t * srcBuf)  
{  
    uint16_t i;  
    uint16_t tmp = 0;  

    for(i = 0; i < 16; i++)  
    {  
      if((*srcBuf) & (1 << i))  
        tmp |= 1 << (15 - i);  
    }  
    *dstBuf = tmp;  
} 

// 计算CRC16_MODBUS
// CRC16 MODBUS 的多项式: X16 + X15 + X2 + 1 = 0x18005
// 返回值: CRC16
uint16_t crc16_modbus(uint8_t * puchMsg, uint16_t usDataLen)  
{  
  uint16_t wCRCin = 0xFFFF;  // CRC16 MODBUS 初值
  uint16_t wCPoly = 0x8005;  // CRC16 MODBUS 的多项式: X16 + X15 + X2 + 1 = 0x18005, 取uint16, = 0x8005
  uint8_t   wChar = 0;  
  uint16_t  i;
  
  while (usDataLen--)     
  {  
        wChar = *(puchMsg++);  
        InvertUint8(&wChar, &wChar);  
        wCRCin ^= (wChar << 8);  
        for(i = 0; i < 8; i++)  
        {  
          if(wCRCin & 0x8000)  
            wCRCin = (wCRCin << 1) ^ wCPoly;  
          else  
            wCRCin = wCRCin << 1;  
        }  
  }  
  InvertUint16(&wCRCin, &wCRCin);  
  return wCRCin;  
}  

#define ANY_SNS_ADDR   0xFE   // 任何 CO2 传感器的地址
#define MAX_RESP_TIME  180    // 最大响应超时时间: 180 ms

// 功能码
typedef enum
{
   FC_ReadHR  = 0x03,   // Read Holding Register
   FC_ReadIR  = 0x04,   // Read Input Register
   FC_WriteSR = 0x06,   // Write Single Register
}CO2_FuncCode;

// Input Register Addr
// 寄存器的值为 2B
typedef enum
{
   IR_MeterStatus = 0,   // 操作状态寄存器地址
   IR_AlarmStatus = 1,
   IR_OutStatus   = 2,
   IR_SpaceCO2    = 3,   // CO2 测量的结果存此
   IR_PWMOut      = 21,  // PWN 输出, reg val = 0x3FFF: 100% Output
   IR_SnsTypeID_High = 25, // 低 8 位
   IR_SnsTypeID_Low  = 26, // 2B, 共 3B 为Sensor Type ID
   IR_MemMapVersion  = 27,  // Memory Map Version
   IR_FWVersonMainSub = 28, // FW Version Main(MSB) & Sub(LSB)
   IR_SnsID_High = 29,      // Sensor ID High, 2B
   IR_SnsID_Low  = 30,      // Sensor ID Low, 2B
   IR_Addr_End   = 31,
}CO2_IRAddr;

typedef enum
{
   HR_AckReg       = 0,   // acknowledgement register address
   HR_SpecCmdReg   = 1,   // special cmd register, write-only
   HR_ABCPeriodReg = 31,   // ABC Period register, write 0 to disable ABC Func, write non-zero to enable ABC
   	
}CO2_HRAddr;

// 通用请求命令格式
// Big Endian, but CRC16 is Little Endian
// 除 CRC16为小端存储外, 其他为大端存储
typedef struct
{
   uint8_t  snsAddr;       // sensor addr: 0xFE
   uint8_t  funcCode; 
   uint16_t regAddr;       // 0 - 31
   uint16_t CntOrVal;      // 寄存器数目(Read)或寄存器的值(Write)
   uint8_t  crc16LSB;      // CRC16 LSB
   uint8_t  crc16MSB;      // crc16 MSB
}CO2_CMD_General_Req;

// 通用响应命令的报文头格式
typedef struct
{
   uint8_t  snsAddr;
   uint8_t  funcCode;  
   uint8_t  byteCnt;   // 字节计数 = 2N, N 为需要读取的寄存器数目
}CO2_General_Resp_Header;

// cmd format of Read Holding Register
typedef CO2_CMD_General_Req CO2_CMD_ReadHR_Req, CO2_CMD_ReadIR_Req;


// Read HR 响应的报文格式: head + 2N + CRC16
typedef CO2_General_Resp_Header 
CO2_CMD_ReadHR_Resp_Header, CO2_CMD_ReadIR_Resp_Header;


/*
功能: 填充命令报文头
参数:  uint8_t * buf: 缓冲区指针
             uint8_t * len: 输入时为缓冲区最大长度, 输出时为实际报文长度
              CO2_FuncCode funcCode: 命令的功能码
              uint16_t regAddr: 寄存器起始地址
              uint16_t val: 读命令为寄存器的数目; 写命令时为该寄存器的值
*/
void co2_fill_req_cmd(uint8_t * buf, uint8_t * len, CO2_FuncCode funcCode, uint16_t regAddr, uint16_t val)
{
   CO2_CMD_General_Req * req = (CO2_CMD_General_Req *)buf;
   uint16_t tmp = 0;

   *len = sizeof(CO2_CMD_General_Req);
   req->snsAddr  = ANY_SNS_ADDR;  //Any Sensor Addr
   req->funcCode = (uint8_t)funcCode;
   req->regAddr  = host2net(regAddr);  // 大端存储
   req->CntOrVal = host2net(val);

   tmp = crc16_modbus(buf, sizeof(CO2_CMD_General_Req) - 2);  // 除去  CRC16 字节长度
   req->crc16LSB = (uint8_t)tmp;
   req->crc16MSB = (uint8_t)(tmp >> 8);
}

#if   CO2_DBG_EN
void CO2_PRINT_BUF(uint8_t * buf, uint8_t len)
{
   do
   {
       uint8_t i;

       CO2_DBG("co2 sensor packet\t");
	   for(i = 0; i < len; i++)
	   {
	      CO2_DBG("%02X ", buf[i]);
	   }
	   CO2_DBG("\r\n");
   }while(0);
  
}
#else
#define CO2_PRINT_BUF(...)
#endif

/*
功能: 生成 读 CO2 的值的命令, 单位: ppm
参数: 
*/
static void MakeCmd_ReadPPM(uint8_t * buf, uint8_t * maxLen, uint8_t opt)
{
   if(*maxLen < sizeof(CO2_CMD_ReadIR_Req))
   	   { CO2_DBG("buf is not enough %s %d\n", __FILE__, __LINE__);  return; }
   
   co2_fill_req_cmd(buf, maxLen, FC_ReadIR,  IR_SpaceCO2, 1);  // read one register

   CO2_PRINT_BUF(buf, *maxLen);
}

// 读 CO2 传感器状态
static void MakeCmd_ReadMeterStatus(uint8_t * buf, uint8_t *maxLen, uint8_t opt)
{
   if(*maxLen < sizeof(CO2_CMD_ReadIR_Req))
   	   { CO2_DBG("buf is not enough %s %d\n", __FILE__, __LINE__);  return; }
   
   co2_fill_req_cmd(buf, maxLen, FC_ReadIR,  IR_MeterStatus, 1);  // read sensor status

   CO2_PRINT_BUF(buf, *maxLen);
}

// 读传感器CO2的测量值和状态
// CO2 返回 8B = 2N, N = 4 registers to read
static void MakeCmd_ReadCO2AndMeterStatus(uint8_t * buf, uint8_t *maxLen, uint8_t opt)
{
   if(*maxLen < sizeof(CO2_CMD_ReadIR_Req))
   	   { CO2_DBG("buf is not enough %s %d\n", __FILE__, __LINE__);  return; }
   
   co2_fill_req_cmd(buf, maxLen, FC_ReadIR,  IR_MeterStatus, 4);  // read register 0 - 3

   CO2_PRINT_BUF(buf, *maxLen);
}

/*
// 设置 CO2 校正
校正步骤:   1   wirte 0 to HR(0x0001), clear status of calibration
                          2   Write command to start background calibration. Parameter for background
calibration is 6 and for nitrogen calibration is 7
                          3   Wait at least 2 seconds for standard sensor with 2 sec lamp cycle
                          4   Read acknowledgement register. We use function 3 “Read Holding register” to
read HR1
                          5   Check that bit 5 (backgroud calibration)  or bit6(zero-calibration ) is 1. It is an 
acknowledgement of that the sensor has performed the calibration operation
// 参数: uint8_t mode: 校正模式: 0 : 背景校正(CO2: 400 ppm);  1: 氮气校正(CO2: 0 ppm)
*/
static void MakeCalibration_Step1(uint8_t * buf, uint8_t *maxLen, uint8_t opt)
{
    co2_fill_req_cmd(buf, maxLen, FC_WriteSR,  HR_AckReg, 0);  // clear HR1 to 0

    CO2_PRINT_BUF(buf, *maxLen);
}

// 参数: uint8_t mode: CO2_CalMode_Air, CO2_CalMode_Zero
static void MakeCalibration_Step2(uint8_t * buf, uint8_t *maxLen, uint8_t mode)
{
    uint16_t val;

    val = ((uint16_t)0x7C << 8) + (uint16_t)mode;
    co2_fill_req_cmd(buf, maxLen, FC_WriteSR,  HR_SpecCmdReg, val);  // clear HR1 to 0

    CO2_PRINT_BUF(buf, *maxLen);
}

static void MakeCalibration_Step3(uint8_t * buf, uint8_t *maxLen, uint8_t opt)
{
   // wait at least 2 seconds
}

// 读校正状态位, bit5: background calibration; bit6: zero-calibration
static void MakeCalibration_Step4(uint8_t * buf, uint8_t *maxLen, uint8_t opt)
{
    co2_fill_req_cmd(buf, maxLen, FC_ReadHR,  HR_AckReg, 1);  // read HR1 of calibration status

    CO2_PRINT_BUF(buf, *maxLen);
}

// 生成读 ABC 周期的报文
static void MakeCmd_ReadABCPeriod(uint8_t * buf, uint8_t *maxLen, uint8_t opt)
{
    co2_fill_req_cmd(buf, maxLen, FC_ReadHR,  HR_ABCPeriodReg, 1);  // read HR1 of calibration status

    CO2_PRINT_BUF(buf, *maxLen);
}

// 禁止 ABC 功能
static void MakeCmd_DisableABCPeriod(uint8_t * buf, uint8_t * maxLen, uint8_t opt)
{
    co2_fill_req_cmd(buf, maxLen, FC_WriteSR,  HR_ABCPeriodReg, 0);  // write 0 to disable ABC function

    CO2_PRINT_BUF(buf, *maxLen);
}

// 使能 ABC 功能
static void MakeCmd_EnableABCPeriod(uint8_t * buf, uint8_t * maxLen, uint8_t opt)
{
    // 使能 ABC 自动校正, 参数 = 180 hour = 7.5 day为一个校正周期
    co2_fill_req_cmd(buf, maxLen, FC_WriteSR,  HR_ABCPeriodReg, 180);  

    CO2_PRINT_BUF(buf, *maxLen);
}

// 发送的命令类型
typedef enum
{
   CCType_RdPPM = 0,         // 读 CO2 浓度
   CCType_RdMSta,               // 读 状态位
   CCType_RdPPMAndMSta,  // 读 CO2 浓度和状态位
   CCType_CaliStep1,
   CCType_CaliStep2,
   CCType_CaliStep3,
   CCType_CaliStep4,
   CCType_RdABCPeriod,  // 读  自动校正周期
   CCType_DisableABC,   // 禁止 自动校正
   CCType_EnableABC,    // 使能 自动校正

   CCType_Reserved,
}CO2CmdType;

// 响应命令与报文长度的映射
typedef struct
{
   uint8_t     cmdLen;
}T_RespCmdLenMap;

const T_RespCmdLenMap respMap[] = 
{
   	7,   // { CCType_RdPPM,  7 },
   	7,   // { CCType_RdMSta, 7 },
   	13,  // { CCType_RdPPMAndMSta, 13 },
   	8,   // { CCType_CaliStep1, 8},
   	8,   // { CCType_CaliStep2, 8},
   	0,  // { CCType_CaliStep3, 128},   // 无此报文
   	7,   // { CCType_CaliStep4, 7},
   	7,   // { CCType_RdABCPeriod, 7},
   	8,   // { CCType_DisableABC,  8},
   	8,   // { CCType_EnableABC,   8},
   	0,  // { CCType_Reserved,    128},  // 无此报文
};

typedef void (* CO2_MakeCmdFunc)(uint8_t * buf, uint8_t * len, uint8_t opt);

static uint8_t co2_rx_buf[64] = {0};



volatile uint8_t co2_rx_cnt = 0; // 接收计数


os_timer_t tTimerCO2GetVal;
os_timer_t tTimerCO2SendCmd;

CO2CmdType tx_co2_cmd_type = CCType_Reserved;  //发送的命令类型

void  co2_uart_send(uint8_t * buf, uint16_t len)
{
      uint16_t i = 0;
      
       while(len--)
       {
                /* e.g. write a character to the USART */
                CO2_USART->DR = (buf[i++] & (uint16_t)0x01FF);
        		
                /* Loop until the end of transmission */
                while (!(CO2_USART->SR & USART_FLAG_TXE));
        }
}

void TransmitCmd(CO2_MakeCmdFunc makeFunc, uint8_t opt)
{
	if(makeFunc == NULL)return;
	else
      {
	   uint8_t buf[64];
	   uint8_t len = sizeof(buf);

	   os_memset(buf, 0, sizeof(buf));
          len = sizeof(buf);
          makeFunc(buf, &len, opt);  // 生成发送报文
          co2_uart_send(buf, len);
	   os_timer_arm(&tTimerCO2GetVal, 20, 0);  // 200 ms 后启动接收
      }
}

void CO2_SendCmdReadPPM(void)
{
   tx_co2_cmd_type = CCType_RdPPM;
   TransmitCmd(MakeCmd_ReadPPM, 0);
}

void CO2_SendCmdReadMeterStatus(void)
{
   tx_co2_cmd_type = CCType_RdMSta;
   TransmitCmd(MakeCmd_ReadMeterStatus, 0);
}

void CO2_SendCmdReadCO2AndMeterStatus(void)
{
   tx_co2_cmd_type = CCType_RdPPMAndMSta;
   TransmitCmd(MakeCmd_ReadCO2AndMeterStatus, 0);
}

void CO2_SendCmdCalibration_Step1(void)
{
   tx_co2_cmd_type = CCType_CaliStep1;
   TransmitCmd(MakeCalibration_Step1, 0);
}
void CO2_SendCmdCalibration_Step2(CO2_CalMode mode)
{
   tx_co2_cmd_type = CCType_CaliStep2;
   TransmitCmd(MakeCalibration_Step2, (uint8_t)mode);
}
// 背景校正
void CO2_SendCmdBackgroundCali_Step2(void)
{
    CO2_SendCmdCalibration_Step2(CO2_CalMode_Air);
}
void CO2_SendCmdCalibration_Step3(void)
{
   tx_co2_cmd_type = CCType_CaliStep3;
   TransmitCmd(MakeCalibration_Step3, 0);
}
void CO2_SendCmdCalibration_Step4(void)
{
   tx_co2_cmd_type = CCType_CaliStep4;
   TransmitCmd(MakeCalibration_Step4, 0);
}


void CO2_SendCmdReadABCPeriod(void)
{
   tx_co2_cmd_type = CCType_RdABCPeriod;
   TransmitCmd(MakeCmd_ReadABCPeriod, 0);
}
void CO2_SendCmdDisableABCPeriod(void)
{
   tx_co2_cmd_type = CCType_DisableABC;
   TransmitCmd(MakeCmd_DisableABCPeriod, 0);
}
void CO2_SendCmdEnableABCPeriod(void)
{
   tx_co2_cmd_type = CCType_EnableABC;
   TransmitCmd(MakeCmd_EnableABCPeriod, 0);
}

typedef void (* CO2_SendCmdFunc)(void);


//static 
CO2_SendCmdFunc CO2_SendCmd[] = {
   CO2_SendCmdReadPPM,
   CO2_SendCmdReadMeterStatus,
   CO2_SendCmdReadCO2AndMeterStatus,
   CO2_SendCmdCalibration_Step1,
   CO2_SendCmdBackgroundCali_Step2,
   CO2_SendCmdCalibration_Step3,
   CO2_SendCmdCalibration_Step4,
   CO2_SendCmdReadABCPeriod,
   CO2_SendCmdDisableABCPeriod,
   CO2_SendCmdEnableABCPeriod,
};



// 传感器状态标志位

#define  MS_Bit_FatalErr      0x0001
#define  MS_Bit_OffsetErr     0x0002
#define  MS_Bit_AlgorithmErr  0x0004  // 运算错误
#define  MS_Bit_OutputErr     0x0008
#define  MS_Bit_SeflDiagnoErr 0x0010  // 自诊断错误
#define  MS_Bit_OutOfRangeErr 0x0020
#define  MS_Bit_MemErr        0x0040

#if  CO2_DBG_EN
const uint8_t CO2MeterStatusString[][24] = 
{
   "MS_Bit_FatalErr",
   "MS_Bit_OffsetErr",
   "MS_Bit_AlgorithmErr",
   "MS_Bit_OutputErr",
   
   "MS_Bit_SelfDiagnoErr",
   "MS_Bit_OutOfRangeErr",
   "MS_Bit_MemErr",
   "MS_Bit_Reserved",
};
#endif

// 传感器状态位检查
// 返回值: SYS_SUCCESS: 传感器无错误; SYS_FAILED: 传感器有错误发生
SYS_RESULT co2_meter_status_check(uint8_t * buf, uint8_t len, CO2CmdType cmdType)
{
       SYS_RESULT result = SYS_SUCCESS;
	uint16_t sta = 0;
	
    switch(cmdType)
    {
          case CCType_RdMSta:
	   case CCType_RdPPMAndMSta:
	   {
          sta = ((uint16_t)buf[3] << 8) + buf[4];

		  if(sta)
		  {
		         uint16_t i;

			 for(i = 0; i < 8; i++)
			 {
			    if(sta & (1 << i))  // 错误标志位
			    {
			            CO2_DBG("CO2 Meter Status Err: %s\n", CO2MeterStatusString[i]);
			    }
			 }
			 result = SYS_FAILED;
		  }
	   }break;
    }
	return result;
}

/*
co2  AQI 标准:
Good  (优):      0 - 600 ppm
Moderate (良): 601 - 1000 ppm
Unhealthy for Sensitive Groups(差): 1000 - 2000, 空气浑浊, 昏昏欲睡
Unhealthy (极差)：> 2000
*/

uint16_t global_co2_ppm = 0;

// CO2 传感器返回的报文校验
// 成功: 0   ;  错误:  ! 0
uint8_t  CO2_PacketCheck(void)
{
       uint16_t newCrc;
	uint16_t oldCrc;
	uint8_t  cmdLen = respMap[(uint8_t)tx_co2_cmd_type].cmdLen;
	
       CO2_DBG("rx co2 sns packet, co2_rx_cnt = %d: ", co2_rx_cnt);
       CO2_PRINT_BUF(co2_rx_buf, co2_rx_cnt);

       // 长度检查
	if(co2_rx_cnt < cmdLen) 
	
	{
	     CO2_DBG("rx co2 buf len err: cmdLen = %d, realLen = %d\n", 
	   	           respMap[(uint8_t)tx_co2_cmd_type].cmdLen, co2_rx_cnt);
	     return 1;
	}
       if(cmdLen < 2)
       {
            CO2_DBG("cmdLen < 2, cmd_type = %d \r\n",  tx_co2_cmd_type);
            return 1;
       }
	// 报文CRC核对
	newCrc = crc16_modbus(co2_rx_buf, cmdLen - 2);
       oldCrc = ((uint16_t)co2_rx_buf[cmdLen  - 1] << 8) + co2_rx_buf[cmdLen - 2];  // crc 为小端存储
       if(oldCrc != newCrc)
       {
             CO2_DBG("rx co2 buf crc err: old crc = 0x%x, new Crc = 0x%x\n", oldCrc, newCrc);
   	      return 2;
       }

	// 传感器状态检查
	if(co2_meter_status_check(co2_rx_buf,  cmdLen, tx_co2_cmd_type))
	{
              CO2_DBG("co2 sns error\n");
		return 3;
	}
	CO2_DBG("\r\nco2 packet ok\r\n");
	return 0;
}

void Timer_CO2GetVal_CallBack(void * arg)
{
        uint16_t co2_ppm = 0;
	 static u8 first_time = 1;
       uint32_t next_tick = 100;
	 
      if(CO2_PacketCheck())
     { 
	       // os_timer_arm(&tTimerCO2SendCmd, SEC(1), 0);
             // return; 
             next_tick = 40;
      }
      else if(tx_co2_cmd_type == CCType_RdPPM || tx_co2_cmd_type == CCType_RdPPMAndMSta)
     {
             uint8_t cmdLen = respMap[(uint8_t)tx_co2_cmd_type].cmdLen;
             
             co2_ppm = ((uint16_t)co2_rx_buf[cmdLen - 4] << 8) + co2_rx_buf[cmdLen - 3];

             SDRR_SaveSensorPoint(SENSOR_CO2,  &co2_ppm);

             
             #if CO2_DBG_EN
             CO2_DBG("co2 = %d ppm\n", co2_ppm);
             #endif

             if(global_co2_ppm != co2_ppm)
            {
                       global_co2_ppm = co2_ppm;
  			  if(first_time)
  			  {
  			        first_time = 0;
  				 SnsGUI_ClearCircle(CIRCLE_CO2);
  			  }
                       SnsGUI_DisplayCO2(co2_ppm);
            }
	}
	os_timer_arm(&tTimerCO2SendCmd,  next_tick, 0);
}

static os_timer_t tTimerBackgndCali;
static uint8_t co2_cali_step = 0;  // 校正步骤
static uint8_t co2_retry_cnt = 0; // 重试次数
//static uint32_t co2_cali_start_sec;

#define MAX_RETRY    32

os_timer_t tTimerCO2TipErase;

void TimerCO2TipErase(void * arg)
{
       SnsGUI_DisplayCO2Tip(TIP_CO2_CALI_Erase);
}

void CO2_ExitBackgndCali(uint8_t exit_sta)
{

      /*
       uint32_t sec = os_get_tick() / 100;

       if((sec - co2_cali_start_sec) < 15)
       {
               co2_cali_step = 0;
               os_timer_arm(&tTimerBackgndCali, 0, 0);  
               return;
       }*/
      os_timer_arm(&tTimerCO2SendCmd, 0, 0);
      if(exit_sta)SnsGUI_DisplayCO2Tip(TIP_CO2_CALI_FAILED);
      else { SnsGUI_DisplayCO2Tip(TIP_CO2_CALI_OK);  }
      os_timer_setfn(&tTimerCO2TipErase,  TimerCO2TipErase,   NULL);
      os_timer_arm(&tTimerCO2TipErase,    SEC(30),   0);
}


static void Timer_BackgndCali_CallBack(void * arg)
{
   CO2_DBG("co2_cali_step = %d\n", co2_cali_step);
   switch(co2_cali_step)
   {
           case 0:
	   {
	   	  co2_rx_cnt = 0;  // 接受数据清0
	   	  CO2_SendCmdCalibration_Step1();
		  os_timer_arm(&tTimerBackgndCali, 20, 0);  // 200 ms 后启动
		  co2_cali_step++;
	   }break;
	   case 1:
	   {
	   	  if(CO2_PacketCheck())
	   	  {
	   	        co2_retry_cnt++;
			 if(co2_retry_cnt > MAX_RETRY)
			 {
			       co2_retry_cnt = 0;
				CO2_DBG("exit co2 background cali step 1\n");
			       CO2_ExitBackgndCali(1);
			 }
			 else
			 {
			      co2_cali_step = 0;
			      os_timer_arm(&tTimerBackgndCali, 0, 0);  
			 }
	   	  }
		  else
		  {
		        CO2_SendCmdBackgroundCali_Step2();
			 os_timer_arm(&tTimerBackgndCali, 20, 0);  // 200 ms 后启动
		        co2_cali_step++;
		  }
	   }break;
	   case 2:
	   {
	   	  if(CO2_PacketCheck())
	   	  {
	   	        co2_retry_cnt++;
			 if(co2_retry_cnt > MAX_RETRY)
			 {
			       co2_retry_cnt = 0;
				CO2_DBG("exit co2 background cali step 2\n");
			       CO2_ExitBackgndCali(1);
			 }
			 else
			 {
			       co2_cali_step = 2;
			       CO2_SendCmdBackgroundCali_Step2();
			       os_timer_arm(&tTimerBackgndCali, 20, 0); 
			 }
	   	  }
		  else
		  {
		        co2_retry_cnt = 0;
			 os_timer_arm(&tTimerBackgndCali, SEC(3), 0);  // 至少等 2 sec
		        co2_cali_step++;
		  }
	   }break;
	   case 3:
	   {
	   	  CO2_SendCmdCalibration_Step4();
		  os_timer_arm(&tTimerBackgndCali,  20, 0);  // 200 ms 后启动
		  co2_cali_step++;
	   }break;
	   case 4:
	   {
                 if(CO2_PacketCheck())
	   	  {
	   	        co2_retry_cnt++;
			 if(co2_retry_cnt > MAX_RETRY)
			 {
			        co2_retry_cnt = 0;
				 CO2_DBG("exit co2 background cali, %d\n", __LINE__);
			        CO2_ExitBackgndCali(1);
			 }
			 else
			 {
                             co2_cali_step = 3;
                             os_timer_arm(&tTimerBackgndCali, 0, 0); 
			 }
	   	  }
		  else
		  {
			 if(co2_rx_buf[4] & (1 << 5)) // 检查校正标志位是否置位
			 {
                             CO2_DBG("co2 background cali success\n");
				 CO2_ExitBackgndCali(0); // 恢复正常查询
			 }
			 else
			 {
                             CO2_DBG("co2 background cali failed\n");
				 co2_retry_cnt++;
				 if(co2_retry_cnt > 50)
				 {
                                    co2_retry_cnt = 0;
					 CO2_DBG("exit co2 background cali, %d\n", __LINE__);
                                   CO2_ExitBackgndCali(1);
				 }
				 else
				 {
                                     co2_cali_step = 3;
                                     os_timer_arm(&tTimerBackgndCali, 0, 0); 
				 }
			 }
		  }
	   }break;
   }

   
}

// 启动背景校正
void CO2_StartBackgndCali(void)
{
    CO2_DBG("start co2 back ground cali, tick = %ld\n", os_get_tick());
	
    co2_cali_step = 0;
	
	os_timer_disarm(&tTimerCO2SendCmd);
	os_timer_disarm(&tTimerCO2GetVal);

	os_timer_arm(&tTimerBackgndCali, 20, 0);  // 200 ms 后启动
//	co2_cali_start_sec = os_get_tick() / 100;
}

void Timer_CO2SendCmd_CallBack(void * arg)
{
        CO2_SendCmdReadCO2AndMeterStatus();
	//os_timer_arm(&tTimerCO2SendCmd, SEC(3), 0);
}



void CO2_Sensor_Init(void)
{
      CO2_USART_Init(9600);
       os_timer_setfn(&tTimerCO2GetVal,         Timer_CO2GetVal_CallBack,        NULL);
       os_timer_setfn(&tTimerCO2SendCmd,  Timer_CO2SendCmd_CallBack,  NULL);
       os_timer_setfn(&tTimerBackgndCali,      Timer_BackgndCali_CallBack,      NULL);
       
       os_timer_arm(&tTimerCO2SendCmd,  SEC(1),  0);
}


void CO2_USART_IRQHandler(void)
{
    if(READ_REG_32_BIT(CO2_USART->SR, USART_SR_RXNE))
   {
          uint8_t data;
         
	   data = (uint16_t)(CO2_USART->DR & (uint16_t)0x01FF);

   	  if(data == ANY_SNS_ADDR)
   	  {
   	         co2_rx_cnt = 0;  
   	  }
   	  else if(co2_rx_cnt >= sizeof(co2_rx_buf))
   	  {
   	         co2_rx_cnt  = 0; 
   	  }
   	  co2_rx_buf[co2_rx_cnt++] = data;
     }
}




