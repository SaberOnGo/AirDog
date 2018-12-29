
#ifndef __OS_TIMER_H__
#define  __OS_TIMER_H__

#include "os_global.h"
#include <string.h>



#define TIMER_DEBUG_EN 0

#if TIMER_DEBUG_EN
#define TIMER_DEBUG  os_printf
#else
#define TIMER_DEBUG(...)
#endif

#define OS_TIMER_FLAG_DEACTIVATED 0x00   // 去激活定时器
#define OS_TIMER_FLAG_ACTIVATED   0x01   // 激活定时器
#define OS_TIMER_FLAG_ONE_SHOT    0x00   // 单次定时
#define OS_TIMER_FLAG_PERIODIC    0x02   // 周期定时

#define OS_TIMER_FLAG_PARAM_INITED      0x80   // 表明定时器参数已初始化

#define TIMER_ONE     0   // 只定时一次
#define TIMER_REPEAT  1   // 重复定时

typedef void (*timeout_callback)(void * param);
struct struct_os_timer;

typedef void os_timer_func_t(void *timer_arg);




typedef struct struct_os_timer
{
   struct struct_os_timer * prev;    // 2B
   struct struct_os_timer * next;    // 2B
   uint8_t         flag;
   uint32_t        init_tick;
   uint32_t        timeout_tick;
   timeout_callback timeout_func;  // 2B
   void * param;
}T_OS_TIMER;
typedef T_OS_TIMER  os_timer_t;

void OS_TimerInit(T_OS_TIMER * timer, timeout_callback timeout_func, void * param, uint32_t time, uint8_t flag);
OS_RESULT OS_TimerStart(T_OS_TIMER * timer);
void OS_TimerStop(T_OS_TIMER * timer);
uint8_t OS_TimerIsStop(T_OS_TIMER * timer);
void OS_TimerCheck(void);
void OS_TimerTickIncrease(uint32_t tick);
//void OS_TimerTask(void);

#define  os_timer_disarm(timer)  OS_TimerStop(timer)
#if 0
void os_timer_setfn(os_timer_t *timer, os_timer_func_t * func, void *parg)               // 函数实现
#else
#define os_timer_setfn(timer, func, parg)   OS_TimerInit(timer, func, parg, 0, 0)  // 宏实现
#endif
void os_timer_arm(os_timer_t * timer, uint32_t tick, uint8_t is_repeat);
#define os_timer_is_stop(timer)   OS_TimerIsStop(timer)

#endif


