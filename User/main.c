/**
  ******************************************************************************
  * @file    main.c
  * @author  yueran
  * @version V1.0 ���
  * @date    2020-04-18
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
#include "RS485.h"
#include "uart4.h"
#include "Control.h"	
#include "Manual.h"
#include "LCFunc.h"
#include "FullAuto.h"
#include "HalfAuto.h"
 /*
 *************************************************************************
 *                          ȫ�ֱ���
 *************************************************************************  
 */
//�ȴ���־λ  ��λ�����ݴ���ʱ WaitFlag = 1������WaitFlag = 0��
uint8_t WaitFlag = 0;
//����ģʽ 1���ֶ�   2�����Զ�   3��ȫ�Զ�
uint8_t Run_Mode = 0;
//0:���ϴ���1:�ϴ�
uint8_t Up_Data_Flag = 0;

POSITION origin;//��ʼλ��
POSITION target;//Ŀ��λ�� 

 /*
 *************************************************************************
 *                        ��������
 *************************************************************************  
 */
void BSP_Init(void);
int main(void)
{	
	//uint8_t Choice = 0;
	SysTick_Init();                 //ϵͳʱ�ӳ�ʼ��
  BSP_Init();                     //���Ӳ����ʼ�� 
	
	while((0==BigClawDataCorrect)||(0==BigCarDataCorrect))
	{
		DataCommunicateManage(BIG_CLAW,1);//��������
		DataCommunicateManage(BIG_CAR,1);//��������
	}
	Up_Data.Status = Up_Data.Status|0x80;	//��ʼ״̬��Ϊ����״̬�����λ��1
	
	//��ʼֵ
//	target.x[0] = 13000;
//	target.y[0] = 4000; 
//	target.z[0] = 1600; 
//	origin.x[0] = 7900; 
//	origin.y[0] = 4000; 
//	origin.z[0] = 1700;  

  //SelfCheckStatus();//���������Լ����
	
	while(1) 
  { 
		if(task_tim.time_10ms >= 20)//��������ÿ��10ms
		{
			if(1 == WaitFlag)//���յ���λ����������
			{				
				//ChoseTask(Choice);
				/*��Run_Mode��ֵ��ָʾ��������ģʽ*/
				//HTaskModeFlag = 0;
				WaitFlag = 2;//���յ�ָ����������
			}
			else if(0 == WaitFlag)/*�ȴ���λ����������*/
			{
				/*�ȴ���λ����������*/
			}
			else if(2 == WaitFlag)//���յ�ָ����������
			{
				if(1==Run_Mode)//�ֶ�
				{
					//����ָ��ѡ��Ҫִ�е�����
					switch(HTaskModeFlag)
					{
						case 1: //X
							ManualXMoving(target.x[0]);
							break;
						case 2: //Y
							ManualYMoving(target.y[0]);
							break;
						case 3: //���Ͽ�����
							
							break;				
						case 4: //�ӷ��ճ�����
							
							break;
						case 5: //��ƽ̨����
							
							break;	
						case 6: //�½���������
							
							break;		
						case 7: //�½������ϳ�
							
							break;
						case 8: //�½���ƽ̨
							
							break;		
						case 9: //ץ��
							ManualClose();
							break;	
						case 10: //����
							ManualOpen();
							break;							
						default:
							break;
					}			
				}
				else if(2==Run_Mode)//���Զ�
				{
					BigCarHalfAutoMode();
				}
				else if(3==Run_Mode)//ȫ�Զ�
				{
					BigCarFullAutoMode();
				}
			}
		}
		
		if(task_tim.time_100ms >= 200)//100ms����
		{
			if(1==Up_Data_Flag)
			{
				RS485_Send_Data();//100ms�ϴ�һ������		
				Up_Data_Flag=0;				
			}
			//������
			if(!ErrorBigCar)
			{
				ALARM_ON;
			}
		}
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
	RS485_Init(115200);             //RS485��ʼ��
	//UART4_Init(115200);	            //UART4��ʼ��(���ڵ�����)
	TIM7_Init(500-1,84-1);          //f=2kHZ,T=0.5ms 
}


