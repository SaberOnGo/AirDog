
#ifndef __CO2_H__
#define  __CO2_H__

/*******************************************************

SENSE AIR CO2 Sensor S8 0053  Driver, MODBUS protocol
SenseAir  CO2 传感器系列 S8 0053 驱动
使用 非标准MODBUS 协议

*********************************************************/
// CO2 校正模式
typedef enum
{
   CO2_CalMode_Air  = 0x06,  // 干净空气中校正
   CO2_CalMode_Zero = 0x07,  // 0 校正, 即氮气中校正
}CO2_CalMode;

extern uint16_t global_co2_ppm;

void CO2_SendCmdReadPPM(void);
void CO2_SendCmdReadMeterStatus(void);
void CO2_SendCmdReadCO2AndMeterStatus(void);

void CO2_SendCmdCalibration_Step1(void);
void CO2_SendCmdCalibration_Step2(CO2_CalMode mode);
void CO2_SendCmdCalibration_Step3(void);
void CO2_SendCmdCalibration_Step4(void);

void CO2_SendCmdReadABCPeriod(void);
void CO2_SendCmdEnableABCPeriod(void);
void CO2_SendCmdDisableABCPeriod(void);

void CO2_StartBackgndCali(void);

#endif


