#include "Control.h"
#include "bsp_led.h"
#include "Data_type.h"   
#include "uart4.h"
#include "usart2.h"
#include "beep.h"
#include "task.h"

Task_loop_t task_tim;
										 
void control_task(void)
{

	task_tim.time_1ms++;
	task_tim.time_2ms++;	
	task_tim.time_5ms++;
	task_tim.time_10ms++;	
	task_tim.time_25ms++;
	task_tim.time_100ms++;	
	task_tim.time_1000ms++;	
	
  if(task_tim.time_1ms >= 2)
	{
	  task_tim.time_1ms = 0;
		
	}	
	
  if(task_tim.time_2ms >= 4)
	{
	  task_tim.time_2ms -= 4;

	}
	else if(task_tim.time_5ms >= 10)
	{
	  task_tim.time_5ms -= 10;

	}
	else if(task_tim.time_25ms >= 50)
	{
	  task_tim.time_25ms -= 50;

	}
	else if(task_tim.time_100ms >= 200)
	{
	  task_tim.time_100ms -= 200;

	}
	else if(task_tim.time_1000ms >= 2000)
	{
	  task_tim.time_1000ms -= 2000;
    if (1==Big_Claw_Up_Delay_Flag)
    {
        Big_Claw_Up_Delay = Big_Claw_Up_Delay+1;
    }
    if (1==Big_Claw_Up_Delay_Pool_Flag)
    {
        Big_Claw_Up_Delay_Pool = Big_Claw_Up_Delay_Pool+1;
    }   
	}	
}


