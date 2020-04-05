#include "Manual.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
extern uint8_t WaitFlag;


//�ֶ�״̬�µ����в��� 1:ֹͣ,2:X,3:Y,4:��,4:��,6:ץ,7:��,
uint8_t HTaskModeFlag = 0;

/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float x)
*
*Description : �ֶ�X�����ƶ�
*Arguments   : Ŀ���x����
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualXMoving(float x)
{
//------------------------------------------------------------
	PowerOn();//�г��ϵ�
//------------------------------------------------------------	
	if((0!=HTaskModeFlag)||(0==BigCarDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(BIG_CAR,1);//�޸�		
	}
	else if(1==BigCarDataCorrect)//��������
	{		
		if(0==X_MOVE_BIT)
		{
				XMoving(x);	
		}
		else if(1==X_MOVE_BIT)//���н���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				X_MOVE_BIT = 0;//��λ
				RequestStop(BIG_CAR);
			}
		}
		else if(2==X_MOVE_BIT)//�����쳣���½���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				X_MOVE_BIT = 0;//��λ
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float y)
*
*Description : �ֶ�Y�����ƶ�
*Arguments   : Ŀ���y����
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualYMoving(float y)
{
//------------------------------------------------------------
	PowerOn();//�г��ϵ�
//------------------------------------------------------------	
	if((0!=HTaskModeFlag)||(0==BigCarDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(BIG_CAR,1);//�޸�		
	}
	else if(1==BigCarDataCorrect)//��������
	{
		
		if(0==Y_MOVE_BIT)
		{
				YMoving(y);
		}
		else if(1==Y_MOVE_BIT)//���н���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Y_MOVE_BIT = 0;//��λ
				RequestStop(BIG_CAR);
			}
		}
		else if(1==Y_MOVE_BIT)//���н���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Y_MOVE_BIT = 0;//��λ
				RequestStop(BIG_CAR);
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualClose(void)
*
*Description : ��£��צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualClose(void)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------	
	if(CloseFlag==0)
	{
		CloseFlag=1;
	}
	else if(CloseFlag==1)
	{
		ClosePaw();
	}
	else if(CloseFlag==2)
	{
		PowerOff();//�г��ϵ�
		if(RelayOffflag==-2)
		{
			RelayOnflag = -1;//��λ
			RelayOffflag = -1;//��λ
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			CloseFlag = 0;//��λ
		}		
	}
}
/*
*****************************************************************************************************************
*                                     void ManualOpen(void)
*
*Description : �ſ���צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualOpen(void)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if(OpenFlag==0)
	{
		OpenFlag=1;
	}
	else if(OpenFlag==1)
	{
		ClosePaw();
	}
	else if(OpenFlag==2)
	{
		PowerOff();//�г��ϵ�
		if(RelayOffflag==-2)
		{
			RelayOnflag = -1;//��λ
			RelayOffflag = -1;//��λ
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			OpenFlag = 0;//��λ
		}		
	}	
}

