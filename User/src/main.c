
#include "GlobalDef.h"
#include "Application.h"
#include "os_timer.h"
#include "TimerManager.h"
#include "gizwits_port.h"
#include "key_drv.h"

int  main(void)
{
      //uint16_t key_result;
      
       AppInit();
       while(1)
	{
		if(flag10ms)
             {
                    flag10ms = 0;
                    key_result = key_read();
		      key_process(key_result);
              }

              
              if(flag_switch)
              {
                    OS_TimerCheck();	
              }
              else
              {
#if GIZWITS_TYPE
                     gizwits_user_task();
#endif

              }
              if(flag100ms)
              {
                    flag100ms = 0;
              }

	}
}
