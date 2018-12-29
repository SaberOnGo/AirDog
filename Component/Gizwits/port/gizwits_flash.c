#include "gizwits_flash.h"
#include "gizwits_protocol.h"
#include "F10X_Flash_If.h"


#define DEBUG

#define FLASH_PAGE_SIZE   PAGE_SIZE


volatile uint32_t flashWriteOffset = SYS_APP_BAK_SAVE_ADDR_BASE;
volatile uint32_t flashReadOffset = SYS_APP_BAK_SAVE_ADDR_BASE;

/* MCU OTA */
/*擦除指定的Flash页*/
void flash_erase_page(uint8_t flashPage , uint32_t addr_base)
{
      uint32_t PageAddress = addr_base+flashPage*FLASH_PAGE_SIZE;        

      FLASH_Unlock();
      FLASH_ErasePage(PageAddress );
      FLASH_Lock();
}
void flash_erase(uint32_t size , uint32_t addr_base)
{
    uint32_t flashPageSum;
		uint32_t i;
    /*如果小于1024做处理*/
    if(size < FLASH_PAGE_SIZE)
        size = FLASH_PAGE_SIZE;												//
    /* 计算需要擦写的Flash页 */
    if((size % FLASH_PAGE_SIZE) == 0)
    {
        flashPageSum = size / FLASH_PAGE_SIZE;				//小于一页擦除一页
    }
    else
    {
        flashPageSum = (size / FLASH_PAGE_SIZE) + 1;	//大于一页擦除n+1页
    }
    for(i = 0;i<flashPageSum;i++)
    {
		flash_erase_page(i, addr_base);								//基址累加擦除flash
    }
}

void writeFlash(uint16_t * buf_to_save , uint16_t len , uint32_t wFlashAddr)
{
    uint16_t count = 0;

    #if 0
    if(wFlashAddr >= 0x08010000)
    {
#ifdef DEBUG
        os_printf("Waring:Flash Write Addr Error\r\n");
#endif
        flashWriteOffset = SYS_APP_BAK_SAVE_ADDR_BASE;
        return;
    }
    #endif
    
     FLASH_Unlock();

     while(count < len)
     {
              FLASH_ProgramHalfWord((wFlashAddr + count * 2), buf_to_save[count]);
              count ++;     
      }
      FLASH_Lock();
}

void readFlash(uint16_t * buf_to_get,uint16_t len , uint32_t readFlashAddr)
{
	uint16_t count=0;
	while(count<len)
	{
	 	buf_to_get[count]=*(uint16_t *)(readFlashAddr + count*2);
		count++;
	}
}
/*写Flash,控制写长度,Flash地址偏移*/
void wFlashData(uint8_t * buf_to_save , uint16_t len , uint32_t wFlashAddr)
{
    uint8_t WriteFlashTempBuf[PIECE_MAX_LEN];//写Flash临时缓冲区
    uint16_t WriteFlashTempLen = 0;//写Flash长度
    memset(WriteFlashTempBuf,0xEE,sizeof(WriteFlashTempBuf));//写Flash临时缓冲区首先全部填充0xEE
    memcpy(WriteFlashTempBuf,buf_to_save,len);//临时缓冲区
    WriteFlashTempLen = len;
    if(len%2 != 0)
        WriteFlashTempLen += 1;//因为Flash只能写半字
    writeFlash((uint16_t *)&WriteFlashTempBuf ,  WriteFlashTempLen/2 , wFlashAddr);
}
void rFlashData(uint8_t * buf_to_get , uint16_t len , uint32_t rFlashAddr)
{
    uint8_t ReadFlashTempBuf[PIECE_MAX_LEN];//读Flash临时缓冲区
    uint16_t ReadFlashTempLen = 0;//读Flash长度
    
    if(len%2 == 0)
    {
        ReadFlashTempLen = len;
        readFlash((uint16_t *)&ReadFlashTempBuf,ReadFlashTempLen/2 , rFlashAddr);
        memcpy(buf_to_get,ReadFlashTempBuf,len);
    }
    else
    {
        ReadFlashTempLen = len + 1;//因为Flash只能读半字
        readFlash((uint16_t *)&ReadFlashTempBuf,ReadFlashTempLen/2 , rFlashAddr);
        memcpy(buf_to_get,ReadFlashTempBuf,len);
    }
}
/****IAP*****/
typedef  void (*iapfun)(void);				//??????`э???.
iapfun jump2app; 
u16 iapbuf[1024];   

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//设置栈顶指针
//__asm void MSR_MSP(u32 addr) 
//{
//    MSR MSP, r0 			//set Main Stack value
 //   BX r14
//}
#include "sys.h"
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//??????ˇ???.
	{ 
		os_printf("Stack Success!\r\n");
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//????????????????(?λ??)		
		MSR_MSP(*(vu32*)appxaddr);					//???APP????(??????????????????)
		jump2app();									//???APP.
	}
	else
	{
		os_printf("Stack Failed!\r\n");
	}
}


