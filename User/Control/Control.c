#include "Control.h"
#include "bsp_led.h"
#include "Data_type.h"   
#include "uart4.h"
#include "usart2.h"
#include "beep.h"
#include "task.h"
#include "LCFunc.h"
#include "relay.h"

 /*
 *************************************************************************
 *                          全局变量
 *************************************************************************  
 */



Task_loop_t task_tim;

void ChoseTask(uint8_t choice)
{
  //TaskModeFlag = choice;
}


extern uint8_t Up_Data_Flag;
void control_task(void)
{

	task_tim.time_10ms++;	
	task_tim.time_25ms++;
	task_tim.time_100ms++;	
	task_tim.time_200ms++;	
	task_tim.time_1000ms++;	
	

	if(task_tim.time_10ms >= 20)
	{
	  
	}
	else if(task_tim.time_25ms >= 50)
	{
	  task_tim.time_25ms -= 50;
	}
	else if(task_tim.time_100ms >= 200)
	{	
		//LED1_TOGGLE;
	  task_tim.time_100ms -= 200;
		if((ReverseStop == 1)||(ReverseStop == 2))//反向制动计时
		{
			ReverseTime++;
		}
		
		if((PointMove == 1)||(PointMove == 2))//点动计时
		{
			PointMoveTime++;
		}
		
		if(CloseFlag == 1)//合爪计时
		{
			CloseDelay++;
		}
		
		if(OpenFlag == 1)//松爪计时
		{
			OpenDelay++;
		}		
		
		if(RelayOnflag>=0)//开遥控器间隔计时
		{
			RelayOnflag++;
		}
		
		if(RelayOffflag>=0)//管遥控器间隔计时
		{
			RelayOffflag++;
		}
		
    if(0==Up_Data_Flag)
		{
			CommunicatDelay++;
		}
	}
	else if(task_tim.time_200ms >= 400)
	{
		
		
	}
	else if(task_tim.time_1000ms >= 2000)
	{
		
	  task_tim.time_1000ms -= 2000;
//    if (1==Big_Claw_Up_Delay_Flag)
//    {
//        Big_Claw_Up_Delay = Big_Claw_Up_Delay+1;
//    }
//    if (1==Big_Claw_Up_Delay_Pool_Flag)
//    {
//        Big_Claw_Up_Delay_Pool = Big_Claw_Up_Delay_Pool+1;
//    }   
	}	
}



