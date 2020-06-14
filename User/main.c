/**
  ******************************************************************************
  * @file    main.c
  * @author  yueran
  * @version V1.0 ���
  * @date    2020-04-25
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

uint8_t pin1State,pin2State;



 /*
 *************************************************************************
 *                        ��������
 *************************************************************************  
 */
void BSP_Init(void);
char a[6] = {0x00,0x02,0x0A,0x02,0x12,0x11};
int adc1,adc2;
int main(void)
{	
	SysTick_Init();                 //ϵͳʱ�ӳ�ʼ��
  BSP_Init();                     //���Ӳ����ʼ�� 

		
	laser.last_dis5 = laser.dis5;//������ʷֵ
	laser.last_dis6 = laser.dis6;
	laser.last_dis7 = laser.dis7;
	laser.last_dis1 = laser.dis1;//������ʷֵ
	laser.last_dis8 = laser.dis8;	

//	while((0==BigClawDataCorrect))
//	{
//		DataCommunicateManage(BIG_CLAW,1);//�����צ����
//		delay_ms(1000);
//	}
	
//	while((0==BigCarDataCorrect))
//	{
//		DataCommunicateManage(BIG_CAR,1);//�����צ����
//		delay_ms(1000);
//	}
	Up_Data.Status = Up_Data.Status|0xF0;	//��ʼ״̬��Ϊ����״̬�����λ��1


  //SelfCheckStatus();//���������Լ����

/******************���Դ�X�����ƶ�ok**************************/
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 1;
//	target.x[0] = 500;
/******************���Դ�Y�����ƶ�ok**************************/
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 2;
//	target.y[0] = 500;
/******************���Դ�צ�½���������ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 3;
///******************���Դ�צ�ӷ��ϳ�����ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 4;
//	target.uwbdis[0] = 500;
//	RelayOnflag = -2;
/******************���Դ�צ��ƽ̨����ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 5;
/******************���Դ�צ�½���������ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 6;
/******************���Դ�צ�½������ճ�ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 7;
//	target.uwbdis[0] = 2500;
//	RelayOnflag = -2;
/******************���Դ�צ�½���ƽ̨ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 8;
//	target.uwbdis[0] = 3000;
//	RelayOnflag = -2;
/******************���Դ�צץ��ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 9;
/******************���Դ�צץ��ok**************************/	
//	delay_ms(1000);
//	WaitFlag = 1;
//	Run_Mode = 1;
//	HTaskModeFlag = 10;

	//laser.dis6 = 5000;
	TIM_Cmd(TIM7,ENABLE); //�򿪶�ʱ��
	
	while(1) 
  { 		
		if(task_tim.time_10ms >= 20)//��������ÿ��10ms
		{			
			if(1 == WaitFlag)//���յ���λ����������
			{				
				//ChoseTask(Choice);
				/*��Run_Mode��ֵ��ָʾ��������ģʽ*/
				//HTaskModeFlag = 0;
//				if ((Run_Mode == 1)&&(HTaskModeFlag == 3))
//					UpOrDown = 1;
//				else if ((Run_Mode == 1)&&(HTaskModeFlag == 6))
//					UpOrDown = 0; 
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
						#if Manual == 1
						case 1: //X
							ManualXMoving(target.x[0]);
							break;
						case 2: //Y
							ManualYMoving(target.y[0]);
							break;
						case 3: //���Ͽ�����
							ManualRaisePawFromLitterPool(BURN_POOL_UZ);
							break;				
						case 4: //�ӷ��ճ�����
							ManualRisePawFromBurnPool(BURN_POOL_UZ);
							break;
						case 5: //��ƽ̨����
							ManualRisePawFromPlatform(PLATFORM_UZ);
							break;	
						case 6: //�½���������
							ManualDowntoLitterPool(BIG_CLAW_BASE_DIS);
							break;		
						case 7: //�½������ϳ�
							ManualDownClawtoBurnPool(BURN_POOL_DZ);
							break;
						case 8: //�½���ƽ̨
							ManualDownToOrigin(ORIGIN_Z);
							break;		
						case 9: //ץ��
							ManualClose();
							break;	
						case 10: //����
							ManualOpen();
							break;									
						default:
							break;
						#elif Manual == 2
						case 0:
							WaitFlag = 0;
							Run_Mode = 0;
							CAR_NORTH(OFF);
							CAR_SOUTH(OFF);
							CAR_WEST(OFF);
							CAR_EAST(OFF);
							PAW_UP(OFF);
							PAW_DOWN(OFF);
							PAW_CLOSE(OFF);
							PAW_RELEASE(OFF);
							break;
						case 1: //X+
							ManualMoveToNorth();
							break;
						case 2: //X-
							ManualMoveToSouth();
							break;
						case 3: //Y+
							ManualMoveToWest();
							break;				
						case 4: //Y-
							ManualMoveToEast();
							break;
						case 5: //��
							ManualRaisePaw();
							break;	
						case 6: //��
							ManualDownPaw();
							break;		
						case 7: //ץ��
							ManualClosePaw();
							break;
						case 8: //����
							ManualOpenPaw();
							break;		
						case 9: //��ң����
							ManualPowerOn();
							break;	
						case 10://�ر�ң����
							ManualPowerOff();
							break;							
						default:
							break;	
						#elif Manual == 3
						case 1: //X
							ManualXMoving(target.x[0]);
							break;
						case 2: //Y
							ManualYMoving(target.y[0]);
							break;
						case 3: //���Ͽ�����
//							ManualRaisePawFromLitterPool(BURN_POOL_UZ);
							ManualRaiseBigPaw(target.uwbdis[0]);
							break;				
						case 4: //�ӷ��ճ�����
							//ManualRisePawFromBurnPool(target.z[0]);
							ManualRaiseBigPaw(target.uwbdis[0]);
							break;
						case 5: //��ƽ̨����
							//ManualRisePawFromPlatform(target.z[0]);
							ManualRaiseBigPaw(target.uwbdis[0]);
							break;	
						case 6: //�½���������
//							ManualDowntoLitterPool(BIG_CLAW_BASE_DIS);
							ManualDowntoLitterPool(target.uwbdis[0]);
							break;		
						case 7: //�½������ϳ�
							//ManualDownClawtoBurnPool(target.z[0]);
							ManualDownClawtoBurnPool(target.uwbdis[0]);
							break;
						case 8: //�½���ƽ̨
							//ManualDownToOrigin(target.z[0]);
							ManualDownToOrigin(target.uwbdis[0]);
							break;		
						case 9: //ץ��
							ManualClose();
							break;	
						case 10: //����
							ManualOpen();
							break;		
						case 11: //��ң����
							ManualPowerOn();
							break;	
						case 12://�ر�ң����
							ManualPowerOff();
							break;								
						default:
							break;						
						#endif
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
			task_tim.time_10ms -= 20;
		}
		
		if(task_tim.time_200ms >= 400)//200ms����
		{
			if(1==Up_Data_Flag)
			{
				RS485_Send_Data();//200ms�ϴ�һ������		
				Up_Data_Flag = 0;			
				CommunicatDelay = 0;								
			}
			//������
			if((ErrorBigCar != 0)||(1==ManualError))
			{
				ALARM_ON;
			}
			task_tim.time_200ms -= 400;
		}		
	}	 
}

void BSP_Init(void)
{
  LED_Init();                     //LED��ʼ��  ok
	Key_Init();                     //������ʼ�� ok  
	ADC_DMA_Init();                 //ADC_DMA��ʼ��  ok 
	Alarm_GPIO_Init();              //�����˿ڳ�ʼ�� ok
	Relay_GPIO_Init();              //�̵����˿ڳ�ʼ�� ok
	EXTIX_Init();                  //�ⲿ�жϳ�ʼ��(���ڽ���ֹͣ��ť ) ok
	USART1_Init(115200);            //USART1��ʼ��(���մ�������433) ok 
	USART2_Init(115200);            //USART2��ʼ��(����Զ�ͨ�ŵ�433) ok
	RS485_Init(115200);             //RS485��ʼ�� ok
	UART4_Init(115200);	            //UART4��ʼ��(���ڵ�����) ok
	TIM7_Init(500-1,84-1);          //f=2kHZ,T=0.5ms  ok
}


