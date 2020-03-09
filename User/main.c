/**
  ******************************************************************************
  * @file    main.c
  * @author  yueran
  * @version V1.0
  * @date    2020-03-09
  * @brief   无锡固废项目--大爪子
  ******************************************************************************
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


int main(void)
{	
	SysTick_Init();                 //系统时钟初始化
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

	
	RequestStop(ALL_DEV);//初始化请求所有433关闭发送
	
//	Target.x[0] = 13000;
//	Target.y[0] = 4000;
//	Target.z[0] = 1600; 
//	Origin.x[0] = 7900;
//	Origin.y[0] = 4000;
//	Origin.z[0] = 1700;	
	
	while(1) 
  { 
		
	}	 
}

