/**
  ******************************************************************************
  * @file    main.c
  * @author  yueran
  * @version V1.0 ���
  * @date    2020-03-16
  * @brief   �����̷���Ŀ--��צ��
  *****************************************************************************
  * @attention
  *
  * ʵ��ƽ̨: STM32 F407VET6
  *
  ******************************************************************************
  */
	
	
 /*
 *************************************************************************
 *                        ������ͷ�ļ�
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
 *                        ��������
 *************************************************************************  
 */
void BSP_Init(void);
int main(void)
{	
	SysTick_Init();                 //ϵͳʱ�ӳ�ʼ��
  BSP_Init();                     //���Ӳ����ʼ��
	
	RequestStop(ALL_DEV);//��ʼ����������433�رշ���
	
	//��ʼֵ
	target.x[0] = 13000;
	target.y[0] = 4000; 
	target.z[0] = 1600; 
	origin.x[0] = 7900; 
	origin.y[0] = 4000; 
	origin.z[0] = 1700; 
	
	while(1) 
  { 
		if(task_tim.time_100ms >= 200)
		{
			
		}
		BigCarTask();
		delay_ms(200);
	}	 
}

void BSP_Init(void)
{
  LED_Init();                     //LED��ʼ��
	Key_Init();                     //������ʼ��
	ADC_DMA_Init();                 //ADC_DMA��ʼ��
	Alarm_GPIO_Init();              //�����˿ڳ�ʼ��
	Relay_GPIO_Init();              //�̵����˿ڳ�ʼ��
	EXTI10_Init();                  //�ⲿ�жϳ�ʼ��(���ڽ���ֹͣ��ť )
	USART1_Init(115200);            //USART1��ʼ��(���մ�������433)
	USART2_Init(115200);            //USART2��ʼ��(����Զ�ͨ�ŵ�433)
	UART4_Init(115200);	            //UART4��ʼ��(���ڵ�����)
	TIM7_Init(500-1,84-1);          //f=2kHZ,T=0.5ms 
}