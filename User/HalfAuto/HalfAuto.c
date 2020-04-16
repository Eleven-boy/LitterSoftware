/***************************************************************************

��ν���Զ�������צ��ץ������������ߵ��צ��ˮƽ�ƶ������ϳ��Ϸ�ʱ����ȷ��

****************************************************************************/
#include "HalfAuto.h"
#include "FullAuto.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h"

 /*
 *************************************************************************
 *                        �궨��
 *************************************************************************  
 */
 //1:�ڴ˲���ɺ���Ҫ�����Ƿ�ִ����һ��
 //0:�ڴ˲���ɺ���Ҫ����ֱ��ִ����һ��
 #define RESTEP_1      0// �Ƿ����¥ƽ̨̧��צ��
 #define RESTEP_2      0// �Ƿ�ˮƽ�ƶ���ץ�ϴ����Ϸ�
 #define RESTEP_3      0// �Ƿ�צ�ӿ�ʼ�½�ȥץ��
 #define RESTEP_4      0// �Ƿ�ִ��ץ�϶���
 #define RESTEP_5      0// �Ƿ�ִ�д�צ��������
 #define RESTEP_6      0// �Ƿ�ˮƽ�ƶ������ճ��Ϸ�
 #define RESTEP_7      0// �Ƿ��·Ŵ�צ�ӿ������ճ�
 #define RESTEP_8      0// �Ƿ�����צ������ȫ�߶�
 #define RESTEP_9      0// �Ƿ�ˮƽ�ƶ�����ʼλ���Ϸ�
 #define RESTEP_10     0// �Ƿ�צ���½�����¥ƽ̨
 #define RESTEP_11     0// �Ƿ�Ͽ���Դ,���н���
 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
extern POSITION origin;//��ʼλ��
extern POSITION target;//Ŀ��λ�� 

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

uint8_t HalfAutoStep = 0;
uint8_t SingleStepOver = 0;//0:����˲�����ִ�У�1:�����ڴ˲�ִ�н���
int8_t IsExecute = -1;//1:ִ����һ��,0����ִ����һ��
/*
*****************************************************************************************************************
*                                    void BigCarHalfAutoMode(void)
*
*Description : ���Զ�����ִ�к���
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void BigCarHalfAutoMode(void)
{
	if(0==HalfAutoStep)//�г��ϵ�
 	{
		if(0==SingleStepOver)
		{
			PowerOn();
			if(RelayOnflag==-2)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				RelayOnflag=-1;//��־λ��λ			
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
		Up_Data.Status = (Up_Data.Status&0xF0)|(0xF1);
	}
	else if(1==HalfAutoStep)// ����¥ƽ̨̧��צ��
	{
		if(0==SingleStepOver)
		{
			RisePawFromPlatform();
			if(UP_BIT == 1)//��������
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				UP_BIT=0;//��־λ��λ
			}			
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
	}
	else if(2==HalfAutoStep)//ˮƽ�ƶ���ץ�ϴ����Ϸ�
	{
		if(0==SingleStepOver)
		{
			target.x[0] = (GARBAGE_X-target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
			HorizontalMoving(target.x[0],target.y[0]);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				  UpOrDown = 0;//0�����ϰ벿��1�����°벿,Ϊ�����½�������׼��
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;//��־λ��λ
				Y_MOVE_BIT=0;//��־λ��λ				
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				UpOrDown = 0;//0�����ϰ벿��1�����°벿,Ϊ�����½�������׼��
				IsExecute=0;
			}			
		}
	}
	else if(3==HalfAutoStep)//צ�ӿ�ʼ�½�ȥץ��
	{
		if(0==SingleStepOver)
		{
			DowntoLitterPool(target.z[0]);
			if(1==DOWN_BIT)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				  CloseFlag=1;//������ִ�е�ץ�ϱ�־λ��1
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				DOWN_BIT = 0;//��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				CloseFlag=1;//������ִ�е�ץ�ϱ�־λ��1
				IsExecute=0;
			}				
		}
	}
	else if(4==HalfAutoStep)//ִ��ץ�϶���
	{
		if(0==SingleStepOver)
		{
			HFClosePaw();
			if(2==CloseFlag)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				  UpOrDown = 1;  //0�����ϰ벿��1�����°벿,��צ����̧��־λ��1
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				CloseFlag = 0; //��־λ��λ				
			}			
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				UpOrDown = 1;  //0�����ϰ벿��1�����°벿,��צ����̧��־λ��1
				IsExecute=0;
			}		
		}
	}
	else if(5==HalfAutoStep)//ִ�д�צ��������
	{
		if(0==SingleStepOver)
		{
			RisePawFromLitterPool();
			if(1==UP_BIT)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				UP_BIT = 0;//��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}	
		}
	}
	else if(6==HalfAutoStep)//ˮƽ�ƶ������ճ��Ϸ�
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;  //��־λ��λ
				Y_MOVE_BIT=0;  //��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(7==HalfAutoStep)//�·Ŵ�צ�ӿ������ճ�
	{
		if(0==SingleStepOver)
		{
			DownClawtoBurnPool();
			if(1==DOWN_BIT)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				  OpenFlag=1;    //���ɿ�צ�ӱ�־λ��1
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				DOWN_BIT=0;    //��־λ��λ	
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				OpenFlag=1;    //���ɿ�צ�ӱ�־λ��1
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(7==HalfAutoStep)//�ɿ�צ�ӷ���
	{
		if(0==SingleStepOver)
		{
			HFOpenPaw();
			if(2==OpenFlag)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				OpenFlag=0;    //��־λ��λ	
			}
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(8==HalfAutoStep)//����צ������ȫ�߶�
	{
		if(0==SingleStepOver)
		{
			RisePawFromBurnPool();
			if(1==UP_BIT)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				UP_BIT = 0;
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(9==HalfAutoStep)//ˮƽ�ƶ�����ʼλ���Ϸ�
	{
		if(0==SingleStepOver)
		{
			HorizontalMoving(ORIGIN_X,ORIGIN_Y);
			if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				X_MOVE_BIT=0;//��־λ��λ
				Y_MOVE_BIT=0;//��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(10==HalfAutoStep)//��צ���½�����¥ƽ̨
	{
		if(0==SingleStepOver)
		{
			DownToOrigin(ORIGIN_Z);
			if(1==DOWN_BIT)
			{
				#if RESTEP_1==0  //ֱ�ӽ�����һ��
					HalfAutoStep++;
				#elif RESTEP_1==1 //��ʾ�˲�ִ�н���
					SingleStepOver=1;
				#endif
				DOWN_BIT=0;//��־λ��λ
			}		
		}
		else if(1==SingleStepOver)
		{
			ConfirmNextStep(HalfAutoStep+1);
			if(1==IsExecute)//����λ��ȷ��ִ����һ��
			{
				Up_Data.HalfStep = 0;
				HalfAutoStep++;
				SingleStepOver=0;
				IsExecute=0;
			}
		}
	}
	else if(11==HalfAutoStep)//�Ͽ���Դ,���н���
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			RelayOffflag=-1;
			WaitFlag=0;      
			Run_Mode=0;
		}
	}
	
	if((2==X_MOVE_BIT)||(2==Y_MOVE_BIT)||(2==DOWN_BIT)||(2==UP_BIT))//���г���
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			ErrorBigCar = HalfAutoStep;//��¼��������һ��
			Up_Data.Status =  Up_Data.Status&0xF0;
			RelayOffflag=-1;
			HalfAutoStep=0;
			X_MOVE_BIT=0;//��־λ��λ
			Y_MOVE_BIT=0;//��־λ��λ
			DOWN_BIT=0;
			UP_BIT=0;
			WaitFlag=0;      
			Run_Mode=0;
		}	
	}
}	
/*
*****************************************************************************************************************
*                                    void ConfirmNextStep(uint8_t step)
*
*Description : ������ÿһ���м�����ȷ���Ƿ�ִ����һ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ConfirmNextStep(uint8_t step)
{
	Up_Data.HalfStep = step;
}
