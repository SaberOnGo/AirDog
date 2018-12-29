
#ifndef __IIC_DRV_H__
#define  __IIC_DRV_H__

#include "GlobalDef.h"

typedef  struct
{
      // uint8_t bit;   // 0 - 15
	uint16_t Pin;
	GPIO_TypeDef* GPIOx;
}bus_io;

typedef struct
{
        bus_io SDA;
	 bus_io SCL;
}IIC_BUS;

#define  IIC_SDA_H(b)   IO_H(b->SDA.GPIOx,  b->SDA.Pin)
#define  IIC_SDA_L(b)    IO_L(b->SDA.GPIOx,   b->SDA.Pin)
#define  IIC_SCL_H(b)    IO_H(b->SCL.GPIOx,   b->SCL.Pin)
#define  IIC_SCL_L(b)     IO_L(b->SCL.GPIOx,   b->SCL.Pin)
#define  IIC_SDA_READ(b)  IO_READ_IN(b->SDA.GPIOx, b->SDA.Pin)



#define IIC_READ  0x01
#define IIC_WRITE 0x00

void IIC_Init(IIC_BUS * b);

SYS_RESULT IIC_WriteNByte(IIC_BUS * b, uint8_t sla_addr, uint16_t data_addr, uint8_t * pdata, uint8_t size);

SYS_RESULT IIC_ReadNByteExt(IIC_BUS * b, uint8_t sla_addr, uint16_t data_addr, uint8_t * pdata, uint8_t size, uint8_t restart_iic);
SYS_RESULT IIC_ReadNByte(IIC_BUS * b, uint8_t sla_addr, uint16_t data_addr, uint8_t * pdata, uint8_t size);
SYS_RESULT IIC_ReadNByteDirectly(IIC_BUS * b, uint8_t sla_addr, uint8_t * pdata, uint8_t size);

SYS_RESULT TVOC_IIC_ReadNByteDirectly(IIC_BUS * b, uint8_t * pdata, uint8_t size);

#endif

