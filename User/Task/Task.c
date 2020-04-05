#include "task.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
 
 

extern POSITION origin;//��ʼλ��
extern POSITION target;//Ŀ��λ�� 

extern uint8_t WaitFlag;





void BigCarTask(void)
{
	BigCarRunning();
}
void BigCarRunning(void)
{
	/*big car starts running!*/
	RelayOn();//��ң����

  RisePawFromBurnPool();
	target.x[0] = (GARBAGE_X-target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
	HorizontalMoving(target.x[0],target.y[0]);//ˮƽ�ƶ�צ����������
	VerticalMoving(target.z[0]);//צ���½�
	ClosePaw();
	ResetFlagBit(); 
	
	RisePawFromLitterPool();//��������צ��
	HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);//��צ���������ճ����Ϸ�
	LowerClawtoBurnPool();//�½�צ�������ճ�
	ReleasePaw(); 
	ResetFlagBit(); 
	
	RisePawFromBurnPool();//�ӷ��ճ�������צ��
	HorizontalMoving(origin.x[0],origin.y[0]);//ˮƽ�ƶ�֮��ʼλ��
	VerticalMoveOrigin(origin.z[0]);//�½�����¥ƽ̨
	RelayOff();//�ر�ң���� 
}
//ˮƽ�ƶ�
void HorizontalMoving(float x,float y)
{
	if((0!=HTaskModeFlag)&&(0==BigCarDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag,1);//��������
	}
	else if(1==BigCarDataCorrect)//��������
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
			RequestStop(BIG_CAR); //�����433ֹͣ��������	
		}
	}
}
//��ֱ�ƶ�ȥץ��
void VerticalMoving(float z)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag,1);//��������
		UpOrDown = 0;//0�����ϰ벿��1�����°벿
	}
	else if(1==BigClawDataCorrect)//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
		}
	}
}
//��ֱ���䵽��ʼλ��
void VerticalMoveOrigin(float z)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag,1);//��������
		laser.last_dis1 = laser.dis1; //�ȸ�laser.last_dis1��ʼ��һ��ֵ
	}
	else if(1==BigClawDataCorrect)//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToPlatform(z);		
			
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
		}
	}
}

//�ӷ��ճ�̧��צ��
void RisePawFromBurnPool(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag,1);//��������
		laser.last_dis8 = laser.dis8; //�ȸ�laser.last_dis8��ʼ��һ��ֵ

	}
	else if(1==BigClawDataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(BURN_POOL_UZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
		}
	}		
	/*
	float err = 0.0f;
  uint8_t count=0;
  uint8_t stop_count=0;	
	
	laser.last_dis5 = laser.dis5;
	RequestStart(BIG_CAR);//�����433��������
	err=laser.last_dis5 - laser.dis5;	
	
	while(err == 0.0f)
	{
		laser.last_dis5 = laser.dis5;
		RequestStart(BIG_CAR);//�����433��������
		delay_ms(100);
		err=laser.last_dis5 - laser.dis5;		
	}
	
	if (laser.dis5<2000)//�������
	{
			laser.dis5 = 7000;
	}	
	
	err = laser.dis5 - BIG_CLAW_UP_STOP;
	
	if(abs(err)>200)//����ֵ����200
	{
		Big_Claw_Up_Delay_Pool_Flag=1;
		PAW_UP(ON);//צ�ӿ�ʼ����
	}
	
	while(abs(err)>200)
	{
		if (laser.dis5<2000)//�������
		{
			laser.dis5 = 7000;
		}
		err = laser.dis5 - BIG_CLAW_UP_STOP;		
		
		if (abs(err)<200)//�˳����
		{
				count = count+1;
				err = 500;
				if (count>10)
				{
						count = 0;
						Big_Claw_Up_Delay_Pool=0;
						Big_Claw_Up_Delay_Pool_Flag=0;
						break;
				}
		}
		else
		{
				count = 0;
		}

		if (laser.dis5<5900)//�ж���λʹצ��ͣס
		{
				if (abs(laser.dis5-laser.last_dis5)<100)
				{
						stop_count=stop_count+1;
						if (stop_count>10)
						{
								stop_count=0;
								Big_Claw_Up_Delay_Pool=0;
								Big_Claw_Up_Delay_Pool_Flag=0;
								break;
						}						
				}
				else
				{
						stop_count=0;
				}
				
				laser.last_dis5 = laser.dis5;
		}
		if (Big_Claw_Up_Delay_Pool>10)//ʱ�䵽��Ҳ���˳�
		{
				Big_Claw_Up_Delay_Pool=0;
				Big_Claw_Up_Delay_Pool_Flag=0;
				break;
		}
	}
	PAW_UP(OFF);//צ��ֹͣ����
	
	while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//�ȴ�mpu6050��ֵ�ȶ�
	{
					
	}	
	
	RequestStop(BIG_CAR);//�����433ֹͣ��������
	laser.dis5 = 0.0f;
	*/
}
//����¥ƽ̨̧��צ��
void RisePawFromPlatform(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag,1);//��������
		laser.last_dis1 = laser.dis1; //�ȸ�laser.last_dis1��ʼ��һ��ֵ

	}
	else if(1==BigClawDataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(PLATFORM_UZ);		
		}
		else if(UP_BIT == 1)
		{
			RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
		}
	}
}
//��������̧��צ��
void RisePawFromLitterPool(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag,1);//��������
		UpOrDown = 1;//0�����ϰ벿��1�����°벿
	}
	else if(1==BigClawDataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(BURN_POOL_UZ);		
		}
		else if(UP_BIT == 1)
		{
			RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
		}
	}	
	/*
	float err = 0.0;
	uint8_t count=0;
	uint8_t stop_count=0;	
	
	laser.last_dis5 = laser.dis5;
	RequestStart(BIG_CAR);//�����433��������
	err=laser.last_dis5 - laser.dis5;	

	while(err == 0.0f)
	{
		laser.last_dis5 = laser.dis5;
		RequestStart(BIG_CAR);//�����433��������
		delay_ms(100);
		err=laser.last_dis5 - laser.dis5;		
	}
	
	if (laser.dis5<2000)//�������
	{
		laser.dis5 = 7000;
	}	
	
	err = laser.dis5 - BIG_CLAW_UP_STOP;	
	if(abs(err)>200)
	{
		Big_Claw_Up_Delay_Flag=1;
		PAW_UP(ON);
	}
	while (abs(err)>200)
	{
		//printf("up:NowDis=%f,err=%f\r\n",laser.dis5,err);
		delay_ms(100); 
		if (laser.dis5<2000)
		{
			laser.dis5 = 7000;
		}
		err = laser.dis5 - BIG_CLAW_UP_STOP;		
		
		if (abs(err<200))//�˳����
		{
			count = count+1;
			err = 500;
			if (count>10)
			{
				count = 0;
				Big_Claw_Up_Delay = 0;
				Big_Claw_Up_Delay_Flag = 0;
				break;
			}
		}
		else
		{
			count = 0;
		}		
		if (((laser.dis5<5900)&&(laser.dis5>5500)))//�ж���λʹצ��ͣס
		{
				if (abs(laser.dis5-laser.last_dis5)<100)
				{
						stop_count=stop_count+1;
						if (stop_count>10)
						{
								stop_count=0;
								Big_Claw_Up_Delay = 0;
								Big_Claw_Up_Delay_Flag = 0;
								break;
						}	
				}
				else
				{
						stop_count=0;
				}
				
				laser.last_dis5 = laser.dis5;
		}	
		if (Big_Claw_Up_Delay>70)//��ʱ
		{
				Big_Claw_Up_Delay = 0;
				Big_Claw_Up_Delay_Flag = 0;
				break;
		}		
	}
	PAW_UP(OFF);
	//צ����������
	RequestStop(BIG_CAR);//�����433ֹͣ��������
	laser.dis5 = 0.0f;
	*/
}
//�·�צ�������ϳ�
void LowerClawtoBurnPool(void)
{
	if((0!=HTaskModeFlag)&&(0==BigClawDataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag,1);//��������
		laser.last_dis8 = laser.dis8; //�ȸ�laser.last_dis8��ʼ��һ��ֵ

	}
	else if(1==BigClawDataCorrect)//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(BURN_POOL_DZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
		}
	}	
/*
	float err2 = 0.0,err3 = 0.0,err4 = 0.0;
	uint8_t count = 0;	
	
	RequestStart(BURN_POOL);//�����ճ��Ͽ���433����������������ָ��

	while ((err2==0)&&(err3==0)&&(err4==0))//�ж������Ƿ�����
	{
		laser.last_dis2 = laser.dis2;
		laser.last_dis3 = laser.dis3;
		laser.last_dis4 = laser.dis4;
		count++;
		if (count>5)
		{
				for (uint8_t i = 0; i < 3; i++)
				{
						RequestStop(BIG_CAR);
						RequestStart(BURN_POOL);
				}
				count=0;
		}
		delay_ms(100);
		err2 = laser.last_dis2 - laser.dis2;
		err3 = laser.last_dis3 - laser.dis3;
		err4 = laser.last_dis4 - laser.dis4;
		//printf("down:Dis2=%f,err2=%f,Dis3=%f,err3=%f,Dis4=%f,err4=%f\r\n",laser.dis2,err2,laser.dis3,err3,laser.dis4,err4);
	}	
	
	if((abs(err2)>500)&&(abs(err3)>500)&&(abs(err3)>500))
	{
			PAW_DOWN(ON);
			//צ�������½�
	}
	while ((abs(err2)>500)&&(abs(err3)>500)&&(abs(err3)>500))
	{
			//printf("down:Dis2=%f,err2=%f,Dis3=%f,err3=%f,Dis4=%f,err4=%f\r\n",laser.dis2,err2,laser.dis3,err3,laser.dis4,err4);
			delay_ms(100); 
			err2 = laser.dis2 - BIG_CLAW_DOWN_STOP2;
			err3 = laser.dis3 - BIG_CLAW_DOWN_STOP3;
			err4 = laser.dis4 - BIG_CLAW_DOWN_STOP4;
			if (((laser.dis2>0)&&(laser.dis2<4500))||((laser.dis3>0)&&(laser.dis3<3500))||((laser.dis4>0)&&(laser.dis4<4500)))
			{
					break;
			}
			
	}	
	PAW_DOWN(OFF);
	//צ���½�����
	RequestStop(BURN_POOL);
*/
}
