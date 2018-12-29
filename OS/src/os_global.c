

#include "os_global.h"
#include <string.h>
#include "TimerManager.h"




/*****************************************************************************
 * 函 数 名  : OS_SetTimeout
 * 负 责 人  : pi
 * 创建日期  : 2016年8月18日
 * 函数功能  : 设置超时值
 * 输入参数  : uint32_t timeout  超时tick数, unit: 10ms
 * 输出参数  : 无
 * 返 回 值  :    超时值
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint32_t OS_SetTimeout(uint32_t tick)
{
   return OS_GetSysTick() + tick;
}

/*****************************************************************************
 * 函 数 名  : OS_IsTimeout
 * 负 责 人  : pi
 * 创建日期  : 2016年8月18日
 * 函数功能  : 判断是否超时
 * 输入参数  : uint32 timetick  超时值
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  :  超时: E_TRUE, 否: E_FALSE
 * 其    它  : 

*****************************************************************************/
E_BOOL OS_IsTimeout(uint32_t timetick)
{
   return ((OS_GetSysTick() >= timetick)? E_TRUE : E_FALSE);
}

E_BOOL gCpuIsBigEndian;  // 是否大端
// 判断CPU是否大端
// 返回值: E_TRUE: 大端; E_FALSE: 小端
E_BOOL FLASH_SAVE Sys_IsBigEndian(void)
{
   union
   {
      int  w16;
	  char c8;
   }uNum;

   uNum.w16 = 0x1234;
   if(0x12 == uNum.c8)  // low addr, MSB : bigEnddian
   {
      return E_TRUE;
   }
   else
   {
      return E_FALSE;
   }
}

// 获取CPU的字节序
void FLASH_SAVE OS_GetCPUEndianness(void)
{
   gCpuIsBigEndian = Sys_IsBigEndian();
   os_printf("Sys is %s\n", gCpuIsBigEndian ? "BigEndian" : "LittleEndian");
}

// CFGR 寄存器的APB1 预分频位的掩码
#define  RCC_CFGR_PPRE1_MASK       ((uint32_t)0x00000700)

static __I uint8_t APB1rescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

// 得到 APB1 的分频值
// 返回值: 1(即不分频), 2, 4, 6, 8, 16 
uint16_t SYS_GetAPB1Prescaler(void)
{
   uint32_t tmp;
   
   /* Get PCLK1 prescaler */
  tmp = RCC->CFGR & RCC_CFGR_PPRE1_MASK;
  tmp >>= 8;

  os_printf("APB1 Prescaler = %d\r\n", 1 << tmp);
  return ((uint16_t)1 << (tmp));
}

/*****************************************************************************
 * 函 数 名  : Util_NumToString
 * 负 责 人  : pi
 * 创建日期  : 2016年8月18日
 * 函数功能  : 将数字转换为字符串
 * 输入参数  : uint32_t num          数字                       
                             uint8_t placeholder_size  占位符长度
 * 输出参数  : uint8_t * pOutString  输出字符串
 * 返 回 值  : uint8_t: 生成的字符个数, 转换失败返回 0
 * 调用关系  : 
 * 其    它  : 
 uint8_t placeholder_size: 占位符个数, 有6个占位，则只能保存6个字符

*****************************************************************************/
uint8_t Util_NumToString(uint32_t val, uint8_t * out_buf, uint8_t placeholder_size)
{
   uint8_t num_string[12];    
   uint32_t div = 1000000000L;
   uint8_t i, valid_size;  // valid_size 为有效显示的数字个数, 如 00123, 则valid_size = 3
   uint8_t space_size;  // 空白字符个数
   
   if(NULL == out_buf)
   {
      return 0;
   }
   os_memset(num_string, 0, sizeof(num_string));
   for(i = 0; i < 10; i++)   // 32位最多有10个数字
   {
      num_string[i]  = val / div + 0x30;   // 取整
      val  %= div;         // 求余,取剩下的整数部分
      div /= 10;
   }
   
    // 跳过前面的连续 0
    for(i = 0; i < 10; i++)
    {
       if(num_string[i] != 0x30)break;  
    }
	valid_size = 10 - i;
	if(valid_size == 0){ i = 9, valid_size = 1; }

    /* 占位符位数比有效数字的个数多, 比如占位符个数为6, 数字为00123, 
        则有效数字的位数为3
       */
	if(placeholder_size >= valid_size)  
	{
	   space_size = placeholder_size - valid_size;  // 空白字符个数
	   
	   os_memset(out_buf, '0', space_size);  // 不够补0
	   os_memcpy(&out_buf[space_size], &num_string[i], valid_size);
	}
	else  // 占位符不够, 只显示小值部分
	{
	   i += valid_size - placeholder_size; // 前面部分不显示, 如 123456, 占位符为4, 则显示 3456
	   os_memcpy(out_buf, &num_string[i], placeholder_size);
	}
	
    return  placeholder_size;
}

// 跳转到 bootloader
#include "misc.h"
void JumpToBootloader(void)
{
    RCC_DeInit();
    GLOBAL_DISABLE_IRQ();
	NVIC_SystemReset();
	
	#if 0
	/* Set the Vector Table base location at 0x08000000 */
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    if (((*(__IO uint32_t*)BOOTLOADER_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    { 
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (BOOTLOADER_ADDRESS + 4);
      Jump_To_Bootloader = (pFunction) JumpAddress;
	  __set_PSP(*(__IO uint32_t*) BOOTLOADER_ADDRESS);
	  __set_CONTROL(0);  // 进入用户级线程模式 进入软中断后才可以回到特权级线程模式
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) BOOTLOADER_ADDRESS);
      Jump_To_Bootloader();
    }
	#endif
}

typedef  void (*pFunction)(void);
__IO uint32_t JumpAddress;
__IO pFunction Jump_To_Application;

void JumpToApplication(void)
{
    //uint32_t temp = 0;

    /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
	#if 1
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	#else
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x1FFE0000 ) == 0x10000000)
	#endif
    { 
      GLOBAL_DISABLE_IRQ();
	  //FLASH_If_EnableWriteProtection(APPLICATION_ADDRESS, USER_FLASH_END_ADDRESS);  // 写保护
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
      Jump_To_Application = (pFunction) JumpAddress;
	  //__set_PSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
	  __set_CONTROL(0);  // 进入用户级线程模式 进入软中断后才可以回到特权级线程模式
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
      Jump_To_Application();
    }
}

uint32_t Sys_GenSum32(void * data, uint32_t length)
{
   register uint32_t i;
   uint32_t sum = 0;
   uint8_t * p = (uint8_t *)data;
   
   for(i = 0; i < length; i++)
   {
      sum += p[i];
   }
   return sum;
}

unsigned long itoa(int value, char *sp, int radix) 
{
    unsigned long len = 0;
#if 1
    char          tmp[16], *tp = tmp;
    int           i, v, sign   = radix == 10 && value < 0;

    v = sign ? -value : value;

    while (v || tp == tmp) {
        i = v % radix;
        v /= radix;
        *tp++ = i < 10 ? (char) (i + '0') : (char) (i + 'a' - 10);
    }


    len = tp - tmp;

    if (sign) {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    *sp++ = '\0';
#endif
    return len;
}

/******************************************************************************
* Name:    CRC-8/ITU           x8+x2+x+1
* Poly:    0x07
* Init:    0x00
* Refin:   False
* Refout:  False
* Xorout:  0x55
* Alias:   CRC-8/ATM
*****************************************************************************/
uint8_t Sys_GenCRC8_ITU(uint8_t *data, uint16_t length)
{
    register uint8_t i;
    uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc ^ 0x55;
}


/*
功能: 在字符流中查找匹配条件的字符串并将其转换为数字
参数: char * buf: 待查找的字符串
             char * str: 匹配的子字符串
             uint16_t strLen: 子字符串的长度
             uint16_t base: 字符串采用的进制: 2 -16
             uint16_t min: 转换为数字, 数字的最小值
             uint16_t max: 转换为数字后的最大值
             char **p: 符合匹配条件的字符串的起始指针的指针
             uint16_t * num: 成功转换为数字的指针
返回值: 转换成功返回 APP_SUCCESS; 失败返回 APP_FAILED
*/
SYS_RESULT StrStrToIntRaw(char * buf, char * str, uint16_t strLen, uint16_t base, uint16_t min, uint16_t max, char ** p, uint16_t * num)
{
     char * p1 = NULL;
	 char * pEnd = NULL;
	 uint16_t len = 0;
	 uint16_t n = 0;

     //ASSERT_PARAM(buf && num);  // 参数必须不为空
	 *num = 0;
     p1 = os_strstr((const char *)buf, str);  // 查找匹配的子字符串
     if(p1)
     {
        p1 += strLen;
        len = os_strlen(p1);
		if(len)
		{
		    n = strtol(p1, &pEnd, 10);
			if(0 == n && pEnd == (char *)p1){ goto Exit; }
			if(min <= n && n <= max)
			{
			   if(p)*p   = p1;
			   *num = n;
			   return SYS_SUCCESS;
			}
			else { goto Exit; }
		}
     }
	 
Exit:
	if(p)*p = NULL;
	return SYS_FAILED;
}

/*
功能: 在字符流中查找匹配条件的字符串并将其转换为数字
参数: char * buf: 待查找的字符串
             char * str: 匹配的子字符串
             uint16_t strLen: 子字符串的长度
             uint16_t min: 转换为数字, 数字的最小值
             uint16_t max: 转换为数字后的最大值
             char **p: 符合匹配条件的字符串的起始指针的指针
             uint16_t * num: 成功转换为数字的指针
返回值: 转换成功返回 APP_SUCCESS; 失败返回 APP_FAILED
*/
SYS_RESULT StrStrToInt(char * buf, char * str, uint16_t strLen, uint16_t min, uint16_t max, char ** p, uint16_t * num)
{
    return StrStrToIntRaw(buf, str, strLen, 10, min, max, p, num);
}

/*
功能: 在字符流中查找匹配条件的字符并将其转换为数字
参数: char * buf: 待查找的字符串
             char * str: 匹配的子字符串
             uint16_t strLen: 子字符串的长度
             uint16_t base: 待转换的字符串采用的进制: 2-16
             uint16_t min: 转换为数字, 数字的最小值
             uint16_t max: 转换为数字后的最大值
             char **p: 符合匹配条件的字符串的起始指针的指针
             uint16_t * num: 成功转换为数字的指针
返回值: 转换成功返回 APP_SUCCESS; 失败返回 APP_FAILED
*/
SYS_RESULT StrChrToIntRaw(char * buf, char c, uint16_t base, uint16_t min, uint16_t max, char ** p, uint16_t * num)
{
     char * p1 = NULL;
	 char * pEnd = NULL;
	 uint16_t len = 0;
	 uint16_t n = 0;

     //ASSERT_PARAM(buf && num);  // 参数必须不为空
	 
	 *num = 0;
     p1 = os_strchr((const char *)buf, c);  // 查找匹配的子字符串
     if(p1)
     {
        p1 += 1;
        len = os_strlen(p1);
		if(len)
		{
		    n = strtol(p1, &pEnd, base);
			if(0 == n && pEnd == (char *)p1){ goto Exit; }
			if(min <= n && n <= max)
			{
			   if(p)*p   = p1;
			   *num = n;
			   return SYS_SUCCESS;
			}
			else { goto Exit; }
		}
     }
	 
Exit:
	if(p)*p = NULL;
	return SYS_FAILED;
}

/*
功能: 在字符流中查找匹配条件的字符并将其转换为数字
参数: char * buf: 待查找的字符串
             char * str: 匹配的子字符串
             uint16_t strLen: 子字符串的长度
             uint16_t min: 转换为数字, 数字的最小值
             uint16_t max: 转换为数字后的最大值
             char **p: 符合匹配条件的字符串的起始指针的指针
             uint16_t * num: 成功转换为数字的指针
返回值: 转换成功返回 APP_SUCCESS; 失败返回 APP_FAILED
*/
SYS_RESULT StrChrToInt(char * buf, char c, uint16_t min, uint16_t max, char ** p, uint16_t * num)
{
     return StrChrToIntRaw(buf, c, 10, min, max, p, num);
}

// 计算相加和的补码
uint8_t getComplementCode(uint8_t * buf, uint16_t len)
{
   register uint16_t i;
   uint8_t sum = 0;

   for(i = 0; i < len; i++)
   {
      sum += buf[i];
   }
   return ((~sum) + 1);
}


uint8_t sys_gen_sum_8(uint8_t * buf,  uint16_t len)
{
          register uint16_t i;
          uint8_t sum = 0;

          for(i = 0;  i  < len; i++)
          {
                 sum += buf[i];
          }
          return sum;
}

void  IntToString(u32 val, u8 * buf,  u8 *buf_size)
{
        uint8_t num_string[10];  // 最多显示10个整数
        uint32_t div = 1000000000;
        uint8_t i,  valid_size;  // valid_size 为有效显示的数字个数, 如 00123, 则valid_size = 3
        u8 size;
        
        os_memset(buf, ' ',  *buf_size);
        os_memset(num_string, ' ', sizeof(num_string));
        for(i = 0; i < 10; i++)
	{
	       num_string[i] = val / div + 0x30;
		val %= div;
		div /= 10;
	}

	// 跳过前面的连续 0
       for(i = 0; i < 10; i++)
      {
              if(num_string[i] != 0x30)break;
      }
	valid_size = 10 - i;
	if(valid_size == 0){ i = 9, valid_size = 1; }
	size = (valid_size <=*buf_size ? valid_size : *buf_size);
	os_memcpy(&buf[*buf_size - size], &num_string[10 - size],  size);
	*buf_size = size;
}

uint8_t  int_to_string(uint32_t val, uint8_t * buf, uint8_t buf_size)
{
       char str[11];
       int len;
       int free_len;  // 空出来的长度
       
       os_memset(str, 0, sizeof(str));
       os_snprintf(str,  buf_size, "%d", val); // len 为欲写入的字符串长度, 而不是实际长度
       len = os_strlen(str);
       free_len = buf_size - 1 - len;
       os_memcpy(&buf[free_len], str,  len);
       if(free_len)os_memset(buf, ' ', free_len);
       return (uint8_t)len;
}
