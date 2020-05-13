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
 *                          ȫ�ֱ���
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
		if((ReverseStop == 1)||(ReverseStop == 2))//�����ƶ���ʱ
		{
			ReverseTime++;
		}
		
		if((PointMove == 1)||(PointMove == 2))//�㶯��ʱ
		{
			PointMoveTime++;
		}
		
		if(CloseFlag == 1)//��צ��ʱ
		{
			CloseDelay++;
		}
		
		if(OpenFlag == 1)//��צ��ʱ
		{
			OpenDelay++;
		}		
		
		if(RelayOnflag>=0)//��ң���������ʱ
		{
			RelayOnflag++;
		}
		
		if(RelayOffflag>=0)//��ң���������ʱ
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



