/**
  ******************************************************************************
  * @file    main.c
  * @author  yueran
  * @version V1.0 裸机
  * @date    2020-03-25
  * @brief   无锡固废项目--大爪子
  *****************************************************************************
  * @attention
  *
  * 实验平台: STM32 F407VET6
  *
  ******************************************************************************
  */
	
	
 /*
 *************************************************************************
 *                        包含的头文件
 *************************************************************************  
 */
#include "stm32f4xx.h"
#include "bsp_SysTick.h"
#include "GPIO.h"
#include "bsp_led.h" 
#include "bsp_basic_tim.h"
#include "ADC.h"
#include "relay.h"
#include "exti.h"
#include "alarm.h"
#include "Data_type.h" 
#include "bsp_key.h" 
#include "usart1.h"
#include "usart2.h"
#include "uart4.h"
#include "task.h"
#include "Control.h"	

 /*
 *************************************************************************
 *                          全局变量
 *************************************************************************  
 */
 //等待标志位  上位机数据传来时 WaitFlag = 1；否则WaitFlag = 0。
 uint8_t WaitFlag = 0;

 /*
 *************************************************************************
 *                        函数声明
 *************************************************************************  
 */
void BSP_Init(void);
int main(void)
{	
	//uint8_t Choice = 0;
	SysTick_Init();                 //系统时钟初始化
  BSP_Init();                     //相关硬件初始化
	
	RequestStop(ALL_DEV);//初始化请求所有433关闭发送
	
	//初始值
	target.x[0] = 13000;
	target.y[0] = 4000; 
	target.z[0] = 1600; 
	origin.x[0] = 7900; 
	origin.y[0] = 4000; 
	origin.z[0] = 1700; 

  SelfCheckStatus();//开机启动自检程序
	
	while(1) 
  { 
		if(task_tim.time_10ms >= 20)//运行任务每次10ms
		{
			if(1 == WaitFlag)//已收到上位机传来数据
			{				
				//ChoseTask(Choice);
				HTaskModeFlag=0;
				WaitFlag = 2;//已收到指令正在运行
			}
			else
			{
				/*等待上位机发送命令*/
			}
			
			//根据指令选择要执行的任务
			switch(HTaskModeFlag)
			{
				case 1: //停止
					RelayOff();
				  HTaskModeFlag=0;
					break;
				case 2: //X
					XMoving(target.x[0]);
					break;
				case 3: //Y
					YMoving(target.y[0]);
					break;
				case 4: //上升
					
					break;				
				case 5: //下降
					
					break;
				case 6: //抓
					
					break;	
				case 7: //松
					
					break;				
				default:
					break;
			}

		}
		else if(task_tim.time_100ms >= 200)
		{
			
		}
		BigCarTask();
	}	 
}

void BSP_Init(void)
{
  LED_Init();                     //LED初始化
	Key_Init();                     //按键初始化
	ADC_DMA_Init();                 //ADC_DMA初始化
	Alarm_GPIO_Init();              //报警端口初始化
	Relay_GPIO_Init();              //继电器端口初始化
	EXTI10_Init();                  //外部中断初始化(用于紧急停止按钮 )
	USART1_Init(115200);            //USART1初始化(接收传感器的433)
	USART2_Init(115200);            //USART2初始化(与电脑端通信的433)
	UART4_Init(115200);	            //UART4初始化(用于调试用)
	TIM7_Init(500-1,84-1);          //f=2kHZ,T=0.5ms 
}


