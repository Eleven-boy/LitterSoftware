#include "FullAuto.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h" 

/*
*************************************************************************
*                       ȫ�ֱ���
*************************************************************************
*/

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

extern POSITION origin;//��ʼλ��
extern POSITION target;//Ŀ��λ�� 

uint8_t FullAutoStep = 0;
/*
*****************************************************************************************************************
*                                    void BigCarFullAutoMode(void)
*
*Description : ȫ�Զ�����ִ�к���
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void BigCarFullAutoMode(void)
{
	if(0==FullAutoStep)//�г��ϵ�
 	{
		PowerOn();
		if(RelayOnflag==-2)
		{
			FullAutoStep++;//������һ��
			RelayOnflag=-1;//��־λ��λ
		}
	}
	else if(1==FullAutoStep)// ����¥ƽ̨̧��צ��
	{
		RisePawFromPlatform();
		if(UP_BIT == 1)//��������
		{
			FullAutoStep++;//������һ��
			UP_BIT=0;//��־λ��λ
		}
	}
	else if(2==FullAutoStep)//ˮƽ�ƶ���ץ�ϴ����Ϸ�
	{
		target.x[0] = (GARBAGE_X-target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
		HorizontalMoving(target.x[0],target.y[0]);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//������һ��
			X_MOVE_BIT=0;//��־λ��λ
			Y_MOVE_BIT=0;//��־λ��λ
			UpOrDown = 0;//0�����ϰ벿��1�����°벿,Ϊ�����½�������׼��
		}
	}
	else if(3==FullAutoStep)//צ�ӿ�ʼ�½�ȥץ��
	{
		DowntoLitterPool(target.z[0]);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//������һ��
			DOWN_BIT = 0;//��־λ��λ
			CloseFlag=1;//������ִ�е�ץ�ϱ�־λ��1
		}
	}
	else if(4==FullAutoStep)//ִ��ץ�϶���
	{
		HFClosePaw();
		if(2==CloseFlag)
		{
			FullAutoStep++;//������һ��
			CloseFlag = 0; //��־λ��λ
			UpOrDown = 1;  //0�����ϰ벿��1�����°벿,��צ����̧��־λ��1
		}
	}
	else if(5==FullAutoStep)//ִ�д�צ��������
	{
		RisePawFromLitterPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//������һ��
			UP_BIT = 0;//��־λ��λ
		}
	}
	else if(6==FullAutoStep)//ˮƽ�ƶ������ճ��Ϸ�
	{
		HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//������һ��
			X_MOVE_BIT=0;  //��־λ��λ
			Y_MOVE_BIT=0;  //��־λ��λ
		}
	}
	else if(7==FullAutoStep)//�·Ŵ�צ�ӿ������ճ�
	{
		DownClawtoBurnPool();
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//������һ��
			DOWN_BIT=0;    //��־λ��λ	
			OpenFlag=1;    //���ɿ�צ�ӱ�־λ��1
		}
	}
	else if(7==FullAutoStep)//�ɿ�צ�ӷ���
	{
		HFOpenPaw();
		if(2==OpenFlag)
		{
			FullAutoStep++;//������һ��
			OpenFlag=0;    //��־λ��λ	
		}
	}
	else if(8==FullAutoStep)//����צ������ȫ�߶�
	{
		RisePawFromBurnPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//������һ��
			UP_BIT = 0;
		}
	}
	else if(9==FullAutoStep)//ˮƽ�ƶ�����ʼλ���Ϸ�
	{
		HorizontalMoving(ORIGIN_X,ORIGIN_Y);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//������һ��
			X_MOVE_BIT=0;//��־λ��λ
			Y_MOVE_BIT=0;//��־λ��λ
		}
	}
	else if(10==FullAutoStep)//��צ���½�����¥ƽ̨
	{
		DownToOrigin(ORIGIN_Z);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//������һ��
			DOWN_BIT=0;//��־λ��λ
		}
	}
	else if(11==FullAutoStep)//�Ͽ���Դ,���н���
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
			ErrorBigCar = FullAutoStep;//��¼��������һ��
			RelayOffflag=-1;
			FullAutoStep=0;
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
*                                    void RisePawFromPlatform(void)
*
*Description : ����¥ƽ̨̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ��������ļ���laser.dis1
*****************************************************************************************************************
*/
void RisePawFromPlatform(void)
{
	if(1==BigClawDataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(PLATFORM_UZ);		
		}
		else if(UP_BIT == 1)
		{
			/*��������*/
		}
	}
}
/*
*****************************************************************************************************************
*                                    void HorizontalMoving(float x,float y)
*
*Description : ˮƽ�ƶ�
*Arguments   : ��x,y��:Ŀ�ĵ�����
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HorizontalMoving(float x,float y)
{
	if(1==BigCarDataCorrect)//��������
	{
		if(X_MOVE_BIT == 0)
		{
			//���г�X�����ƶ�
			XMoving(x);			
		}
		else if(Y_MOVE_BIT == 0)
		{
			//���г�Y�����ƶ�
			YMoving(y);		
		}
		else
		{
			/*ˮƽ�ƶ�����*/
		}
	}
}

/*
*****************************************************************************************************************
*                                   void DowntoLitterPool(float z)
*
*Description : ��ֱ�½����ϳصײ�ץ��
*Arguments   : z:צ���½����������ĸ߶�
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void DowntoLitterPool(float z)
{
	if(1==BigClawDataCorrect)//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)
		{
			/*�½�����*/
		}
	}
}
/*
*****************************************************************************************************************
*                                     void HClosePaw(void)
*
*Description : ץ�Ϻ���
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HFClosePaw(void)
{
	ClosePaw();
}
/*
*****************************************************************************************************************
*                                void RisePawFromLitterPool(void)
*
*Description : ��������̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : �տ�ʼ��������ļ��⣬���е��ϰ벿����������ļ���
*****************************************************************************************************************
*/
void RisePawFromLitterPool(void)
{
	if(1==BigClawDataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(BURN_POOL_UZ);		
		}
		else if(UP_BIT == 1)
		{
			/*��������*/
		}
	}	
}
/*
*****************************************************************************************************************
*                                void DownClawtoBurnPool(void)
*
*Description : �·�צ�������ϳ�
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void DownClawtoBurnPool(void)
{
	if(1==BigClawDataCorrect)//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(BURN_POOL_DZ);		
		}
		else if(DOWN_BIT == 1)
		{
			/*�·Ž���*/
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void HFOpenPaw(void)
*
*Description : ���Ϻ���
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HFOpenPaw(void)
{
	ReleasePaw();
}
/*
*****************************************************************************************************************
*                                     void RisePawFromBurnPool(void)
*
*Description : �ӷ��ճ�̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RisePawFromBurnPool(void)
{
	if(1==BigClawDataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(BURN_POOL_UZ);		
		}
		else if(DOWN_BIT == 1)
		{
			/*��������*/
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void DownToOrigin(void)
*
*Description : ��ֱ���䵽��ʼλ��
*Arguments   : none
*Returns     : none
*Notes       : �ô�צ��������ļ���
*****************************************************************************************************************
*/
void DownToOrigin(float z)
{
	if(1==BigClawDataCorrect)//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToPlatform(z);		
			
		}
		else if(DOWN_BIT == 1)
		{
			/*�½�����*/
		}
	}
}
