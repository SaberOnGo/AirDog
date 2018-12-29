
#include "os_timer.h"

#if 1
#define TIMER_ENTER_CRITICAL()    GLOBAL_DISABLE_IRQ()
#define TIMER_EXIT_CRITICAL()     GLOBAL_ENABLE_IRQ() 
#else
#define TIMER_ENTER_CRITICAL()    
#define TIMER_EXIT_CRITICAL()     
#endif





static T_OS_TIMER * head_handle = NULL;
static uint32_t timer_tick = 0;
	
void OS_TimerInit(T_OS_TIMER * timer, 
                                     timeout_callback timeout_func, 
                                     void * param,
                                     uint32_t time,
                                     uint8_t flag)
{
   timer->timeout_func = timeout_func;
   timer->param = param;
   timer->init_tick = time;
   timer->timeout_tick = os_get_tick() + time;
   timer->flag = flag;
   //timer->flag |= OS_TIMER_FLAG_PARAM_INITED;
}



OS_RESULT OS_TimerStart(T_OS_TIMER * timer)
{
   T_OS_TIMER * target = NULL;

   target = head_handle;
   while(target)
   {
      if(target == timer)
	  {
	     target->flag |= OS_TIMER_FLAG_ACTIVATED;
		 TIMER_DEBUG("timer 0x%lx existed\r\n", (uint32_t)(timer));
		 return OS_OK;
	  }
	  target = target->next;
   }
   TIMER_ENTER_CRITICAL();
   timer->next = head_handle;
   head_handle = timer;  // pointe to the last timer
   TIMER_EXIT_CRITICAL();
   
   timer->flag |= OS_TIMER_FLAG_ACTIVATED;
   TIMER_DEBUG("handle = 0x%lx, flag = 0x%lx\r\n", (uint32_t)head_handle, (uint32_t)timer->flag);
   
   return OS_OK;
}

void OS_TimerStop(T_OS_TIMER * timer)
{
   T_OS_TIMER * cur;
   
   for(cur = head_handle; cur; cur = cur->next)
   {
      
      if(cur == timer)
      {
         cur->flag &= ~ OS_TIMER_FLAG_ACTIVATED;
         TIMER_DEBUG("stop timer: 0x%lx\r\n", (uint32_t)(cur));
      }
   }
}


uint8_t OS_TimerIsStop(T_OS_TIMER * timer)
{
   return ( !(timer->flag  & OS_TIMER_FLAG_ACTIVATED));
}

void OS_TimerCheck(void)
{
   T_OS_TIMER *cur = NULL;

   for(cur = head_handle; cur; cur = cur->next)
   {
      //if((uint32_t)cur < 0x20000000 || (uint32_t)cur >= 0x2000C000)
	  //{
	  //    os_printf("mem access err: cur = 0x%lx, %s, %d\r\n", cur, __FILE__, __LINE__);
	  //}
      if(os_get_tick() >= cur->timeout_tick &&
	  	(cur->flag & OS_TIMER_FLAG_ACTIVATED))	
      {
		   TIMER_DEBUG("timer tick out: %ld ms\r\n", os_get_tick());
		   if(! (cur->flag & OS_TIMER_FLAG_PERIODIC))
		   {
		       cur->flag &= ~ OS_TIMER_FLAG_ACTIVATED;
		   }
		   else
		   {
		      cur->timeout_tick = os_get_tick() + cur->init_tick;
		   }
		   if(cur->timeout_func)cur->timeout_func(cur->param);
      }
   }
}

void OS_TimerTickIncrease(uint32_t tick)
{
   timer_tick++;
}

#if 0
void OS_TimerTask(void)
{
   if(os_get_tick() > (timer_tick + 10))  // 10 ms
   {
      timer_tick = Sys_GetRunTime();
      OS_TimerCheck();
   }
}
#endif

#if 0
void os_timer_setfn(os_timer_t *timer, os_timer_func_t * func, void *parg)
{
   OS_TimerInit(timer, func, parg, 0, 0);
}
#endif

// 设置定时器
// 参数: os_timer_t * timer : 定时器指针
// uint32_t tick: 定时的tick 数
// uint8_t is_repeat: 是否为循环定时器; 1: 循环定时器; 0: 单次定时器
void os_timer_arm(os_timer_t * timer, uint32_t tick, uint8_t is_repeat)
{
   timer->init_tick = tick;
   timer->timeout_tick = os_get_tick() + tick;
   if(is_repeat)
   {
      timer->flag |= OS_TIMER_FLAG_PERIODIC;  // timer repeat
   }
   OS_TimerStart(timer);
}



