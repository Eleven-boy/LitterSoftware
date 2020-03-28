#include "task.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
 
 

POSITION origin;//��ʼλ��
POSITION target;//Ŀ��λ�� 

extern uint8_t WaitFlag;

uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;

//0:���ݲ�������1:��������
uint8_t DataCorrect = 0; 
//���Զ�״̬�µ����в��� 1:ֹͣ,2:X,3:Y,4:��,4:��,6:ץ,7:��,
uint8_t HTaskModeFlag = 0;
//���г�����ֹͣ��־λ
uint8_t ReverseStop = 0;
//����ʱ��
int ReverseTime = 0; 
//�����������
uint8_t ReverseCount = 0;

//���г��㶯��־λ
uint8_t PointMove = 0;
//�綯ʱ��
int PointMoveTime = 0; 

int64_t Big_Claw_Up_Delay = 0;//��ʱʱ�� S
uint8_t Big_Claw_Up_Delay_Flag = 0;//�򿪶�ʱ����־λ

int64_t Big_Claw_Up_Delay_Pool = 0;
uint8_t Big_Claw_Up_Delay_Pool_Flag = 0;

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
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag);//��������
	}
	else if(1==DataCorrect)//��������
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
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag);//��������
		laser.last_dis1 = laser.dis1; //�ȸ�laser.last_dis1��ʼ��һ��ֵ
		laser.last_dis8 = laser.dis8; //�ȸ�laser.last_dis8��ʼ��һ��ֵ
	}
	else if(1==DataCorrect)//��������
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
//��ֱ���䵽��ʼλ��
void VerticalMoveOrigin(float z)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag);//��������
		laser.last_dis1 = laser.dis1; //�ȸ�laser.last_dis1��ʼ��һ��ֵ

	}
	else if(1==DataCorrect)//��������
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
//�Ͻ�צ��
void ClosePaw(void)
{
		PAW_CLOSE(ON);
		for (int i = 0; i < 20; i++)
		{
				delay_ms(1000);
		}
		PAW_CLOSE(OFF);	
}
//�ɿ�צ��
void ReleasePaw(void)
{
		PAW_RELEASE(ON);
		for (int i = 0; i < 10; i++)
		{  
				delay_ms(1000);   
		}
		PAW_RELEASE(OFF);	
}
//�ӷ��ճ�̧��צ��
void RisePawFromBurnPool(void)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag);//��������
		laser.last_dis8 = laser.dis8; //�ȸ�laser.last_dis8��ʼ��һ��ֵ

	}
	else if(1==DataCorrect)//��������
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
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag);//��������
		laser.last_dis1 = laser.dis1; //�ȸ�laser.last_dis1��ʼ��һ��ֵ

	}
	else if(1==DataCorrect)//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(PLATFORM_UZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
		}
	}
}
//��������̧��צ��
void RisePawFromLitterPool(void)
{
	
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
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//���ݲ�����
	{
		DataCommunicateManage(HTaskModeFlag);//��������
		laser.last_dis8 = laser.dis8; //�ȸ�laser.last_dis8��ʼ��һ��ֵ

	}
	else if(1==DataCorrect)//��������
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
//����X�����˶�
void XMoving(float x)
{
	float err_x = 0;
	err_x = laser.dis6-x;
	
	if(abs(err_x)>2000)//ƫ�����һ����Χʱ��Ҫ�ƶ�	
	{
		err_x = laser.dis6 - x;    //24��
		//Now is moving to the X destination
		if(err_x<0)//���г������ƶ�
		{
			//���г���ʼ�����ƶ�
			CAR_SOUTH(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);  		
		}
		else if(err_x>0)//���г���ʼ���˶�
		{
			//���г���ʼ���˶�
			CAR_NORTH(ON);		
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);	
		}		
	}	
	//���г�X����ʼ�㶯
	else if(abs(err_x)<2000)//���г�X����㶯ģʽ
	{		
		if(err_x<0)//���г����ϵ㶯
		{
			//���г�����ֹͣ
			if((err_x<(-200))&&(ReverseCount<2))
			{
				RevStop(err_x);			
			}
			else if((err_x<(-200))&&(ReverseCount>=2))//���г����ϵ㶯
			{
				HorizontalDotMove(err_x,0.0f);
			}
		  else/*X�����ƶ�����*/
			{
				CAR_SOUTH(OFF);
				HTaskModeFlag = 0;
				WaitFlag = 0;
				
				ReverseCount = 0;
				PointMove = 0;
				PointMoveTime = 0;
				X_MOVE_BIT = 1;
			}
		}
		else if(err_x>=0)//���г��򱱵㶯
		{
			//���г������ƶ�			
			if((err_x>200)&&(ReverseCount<2))
			{
				RevStop(err_x);
			}			
			else if((err_x>200)&&((ReverseCount)>=2))//���г��򱱵㶯
			{
				HorizontalDotMove(err_x,0.0f);
			}
			else/*X�����ƶ�����*/
			{
				CAR_NORTH(OFF);						
				HTaskModeFlag = 0;
				WaitFlag = 0;
				
				ReverseCount = 0;
				PointMove = 0;
				PointMoveTime = 0;
				X_MOVE_BIT = 1;
			}
			
		}
	}
}
//����Y�����˶�
void YMoving(float y)
{
	float err_y = 0;
	err_y = laser.dis7-y;
	
	//���г�Y�����ƶ�
	if(abs(err_y)>2000)//���г�Y�����ƶ�
	{	
		//Now is moving to the Y destnation
		if ((err_y>0))//���г����˶�
		{
			CAR_EAST(ON); 
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}
		else if ((err_y<0))//���г������˶�
		{
			CAR_WEST(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}	
	}
	//���г�Y����㶯
	else if ((abs(err_y)<2000))//���г�Y����㶯
	{	
		if(err_y<0)//���г������㶯
		{
			if(err_y<(-200))
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y�����ƶ�����
			{
				CAR_WEST(OFF);
				HTaskModeFlag = 0;
				WaitFlag = 0;
				
				PointMove = 0;
				PointMoveTime = 0;	
				Y_MOVE_BIT = 1;				
			}
		}
		else if(err_y>0)//���г��򶫵㶯
		{
			if(err_y>200)
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y�����ƶ�����
			{
				CAR_EAST(OFF);	
				HTaskModeFlag = 0;
				WaitFlag = 0;
				
				PointMove = 0;
				PointMoveTime = 0;	
				Y_MOVE_BIT = 1;							
			}
		}
	}	
}
//צ���������򣨷��ϳ������������ô�צ������ļ���
void UpPawFromBurnPool(float z)
{
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis8<0)//�˳�ż�����ֵĴ���ֵ
	{
		laser.dis8=laser.last_dis8;
	}				
	paw_err = laser.dis8 - z; 
	laser.last_dis8 = laser.dis8;   
	//��צ����
	/*���1�������������*/
	if(abs(paw_err)>300)//ʵ�ʾ������������ƫ�����0.3�ף���צ����
 	{
		if(paw_err>=0)//��צ����
		{
			PAW_UP(ON);	
		}
	}
	else if(abs(paw_err)<300)//ʵ�ʾ������������ƫ��С��0.3�ף���צֹͣ����
	{
		PAW_UP(OFF);	
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		UP_BIT = 1;//������ɱ�־λ��1
	}	
	/*���2��צ���޷�����������abs(paw_err)>300*/
	if (abs(paw_err-paw_err_last)<100)//�����Ѿ���������λ�����
	{
		same_dis_count = same_dis_count+1;
		
		if (same_dis_count>5)
		{
			PAW_UP(OFF);
			same_dis_count=0;
			HTaskModeFlag = 0;
			WaitFlag = 0;
			
			UP_BIT = 1;
		}	
	}
	else
	{
		same_dis_count=0;
	}
	paw_err_last=paw_err;			
}
//צ���������򣨴���¥ƽ̨������
void UpPawFromPlatform(float z)
{
	float paw_err=0; 
	float paw_nowDis=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
	{
		laser.dis1=laser.last_dis1;
	}				
	paw_nowDis=laser.dis1;     
	paw_err = paw_nowDis - z;    
	laser.last_dis1 = laser.dis1;   
		
	paw_err = laser.dis1 - z;	
	//��צ����
	/*���1�������������*/
	if(abs(paw_err)>300)//ʵ�ʾ������������ƫ�����0.3�ף���צ����
 	{
		if(paw_err<=0)//��צ����
		{
			PAW_UP(ON);	
		}
	}
	else if(abs(paw_err)<300)//ʵ�ʾ������������ƫ��С��0.3�ף���צֹͣ����
	{
		PAW_UP(OFF);	
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		UP_BIT = 1;//������ɱ�־λ��1
	}	
	/*���2��צ���޷�����������abs(paw_err)>300*/
	if (abs(paw_err-paw_err_last)<100)//�����Ѿ���������λ�����
	{
		same_dis_count = same_dis_count+1;
		
		if (same_dis_count>5)
		{
			PAW_UP(OFF);
			same_dis_count=0;
			HTaskModeFlag = 0;
			WaitFlag = 0;
			
			UP_BIT = 1;
		}	
	}
	else
	{
		same_dis_count=0;
	}
	paw_err_last=paw_err;			
}
//צ���������򣨴��Ͽӳ�������
void UpPawFromLitterPool(float z)
{

}
//צ���½������½������ϳأ����ô�צ������ļ���
void DownPawToBurnPool(float z)
{
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis8<0)//�˳�ż�����ֵĴ���ֵ
	{
		laser.dis8=laser.last_dis8;
	}				
	paw_err = laser.dis8 - z; 
	laser.last_dis8 = laser.dis8;   

	//��צ�½�
	/*���1�������������*/
	if(abs(paw_err)>300)//ʵ�ʾ������������ƫ�����0.3�ף���צ�½�
	{
		if(paw_err<=0)//��צ�½�
		{
			PAW_DOWN(ON);	
		}
	}
	else if(abs(paw_err)<300)//ʵ�ʾ������������ƫ��С��0.3�ף���צֹͣ�½�
	{
		PAW_DOWN(OFF);	
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		DOWN_BIT = 1;//�½���ɱ�־λ��1
	}
	/*���2���ڵ㶯ʱצ����б����*/
	if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
	{
		PAW_DOWN(OFF);
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		DOWN_BIT = 1;
	} 		
	/*���3��צ���޷��½�������������Ȼ�½�*/
	if (abs(paw_err-paw_err_last)<100)//�����Ѿ��½����ײ����������½������
	{
		same_dis_count = same_dis_count+1;
		
		if (same_dis_count>5)
		{
			PAW_DOWN(OFF);
			same_dis_count=0;
			HTaskModeFlag = 0;
			WaitFlag = 0;
			
			DOWN_BIT = 1;
		}	
	}
	else
	{
		same_dis_count=0;
	}
	paw_err_last=paw_err;		
}
//צ���½������½����Ͽ�ץ�ϣ�
void DownPawToLitterPool(float z)
{
//	������24�׵İ�
	/*
		��צ���½�ץ��ʱ�½��Ĺ�����Ҫ�õ������������⴫������
		���ϰ벿��ʱ�������ϴ�ļ��⴫����
		���°벿��ʱ�������´�ļ��⴫����
		�����м���һ��ä��
	*/
	if(laser.dis8>0 && laser.dis1<0)
	{
	
	}
	else if(laser.dis8<0 && laser.dis1<0)
	{
	
	}
	else if(laser.dis8<0 && laser.dis1>0)
	{
	
	}
}
//צ���½������½���5¥ƽ̨��
void DownPawToPlatform(float z)
{
	float paw_err=0; 
	float paw_nowDis=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
	{
		laser.dis1=laser.last_dis1;
	}				
	paw_nowDis=laser.dis1;     
	paw_err = paw_nowDis - z;    
	laser.last_dis1 = laser.dis1;   
		
	paw_err = laser.dis1 - z;

	//��צ�½�
	if(abs(paw_err)>1000)//��צ�½�
	{
		if(paw_err>=0)//��צ�½�
		{
			PAW_DOWN(ON);	
		}
	}
	//��צ�½���ָ���߶ȣ�׼���㶯
	else if(abs(paw_err)<1000)//��צ�½���ָ���߶ȣ�׼���㶯
 	{
		/*���1�������������*/
		if(paw_err>200)//��ʼ�㶯
		{
			VerticalDotMove(paw_err);
		}
		else//�㶯����
		{
			PAW_DOWN(OFF);			
			HTaskModeFlag = 0;
			WaitFlag = 0;
			
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		}
		/*���2���ڵ㶯ʱצ����б����*/
		if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
		{
			PAW_DOWN(OFF);
			HTaskModeFlag = 0;
			WaitFlag = 0;
			
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		} 		
		/*���3��צ���޷��½�������������Ȼ�½�*/
		if (abs(paw_err-paw_err_last)<100)//�����Ѿ��½����ײ����������½������
		{
			same_dis_count = same_dis_count+1;
			
			if (same_dis_count>5)
			{
				PAW_DOWN(OFF);
				same_dis_count=0;
				HTaskModeFlag = 0;
				WaitFlag = 0;
				
				PointMove = 0;
				PointMoveTime = 0;	
				DOWN_BIT = 1;
			}	
		}
		else
		{
			same_dis_count=0;
		}
		paw_err_last=paw_err;				
	}	
}
//���г��Լ�״̬
void SelfCheckStatus(void)
{
	uint8_t count1=0;
	uint8_t count2=0;
	
  RelayOn();//��ң����
	RequestStart(BIG_CAR);//�����433��������
	
	while ((laser.dis6<=0)||(laser.dis7<=0))//�жϴ��г������Ƿ�����
	{
		RequestStart(BIG_CAR); //�����433��������	
		
		count1++;
		if(count1>10)//�����������10�λ�û�еõ���ȷ���ݣ����������ݴ�����·�����⣬��Ҫ���
		{
			break;
		}
	}
	if(count1<=10)//��������
	{
		/*
			�����������ϴ����ݣ��˶κ�����Ҫ����
		*/
		
		//�ж��г�λ��
		if((abs(laser.dis6-ORIGIN_X)<200)&&(abs(laser.dis7-ORIGIN_Y)<200))
		{
			//(x,y)�����ڳ�ʼλ�ã������ж�z�������Ƿ��ڳ�ʼλ�ø���
			RequestStop(BIG_CAR); //�����433ֹͣ��������	
			RequestStart(BIG_CLAW); //�����צ433��������	
			
			while (laser.dis1<=0)//�жϴ�צ�ϴ������Ƿ�����
			{
				count2++;
				if(count2>10)//�����������10�λ�û�еõ���ȷ���ݣ����������ݴ�����·�����⣬��Ҫ���
				{
					break;
				}
				for (uint8_t i = 0; i < 5; i++)
				{
					RequestStop(BIG_CAR); //�����433ֹͣ��������	
					RequestStart(BIG_CLAW); //�����ץ433��������	
				}			 
			}		
			if(abs(laser.dis1-ORIGIN_Z)<200)
			{
				/*����Ҫ��*/
				RequestStop(BIG_CLAW); //�����צ433ֹͣ��������	
			}
			else
			{
				VerticalMoveOrigin(ORIGIN_Z);//�ƶ�����ʼλ��
			}
		}
		else
		{
			//�����жϴ�צ�������������Ϸ�����ƽ̨�Ϸ�
			if(laser.dis6<ADD_X)//��ƽ̨�Ϸ�
			{		
				RisePawFromBurnPool();
				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
				VerticalMoveOrigin(ORIGIN_Z);
			}
			else//���������Ϸ�
			{
				RisePawFromLitterPool();
				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
				VerticalMoveOrigin(ORIGIN_Z);				
			}			
		}
	}
	ResetFlagBit();
	RelayOff();//�ر�ң���� 
}
//����ͨѶ����
void DataCommunicateManage(uint8_t task_mode)
{
	if((2==task_mode)||(3==task_mode)||(4==task_mode))//X,Y,����
	{	
		if ((laser.dis6<=0)||(laser.dis7<=0)||(laser.dis5<=0))//�жϴ��г������Ƿ�����
		{
			DataCorrect = 0;
			RequestStop(BURN_POOL); //�����ϳ�433ֹͣ��������	
			RequestStop(BIG_CLAW);  //�����צ433ֹͣ��������
			RequestStart(BIG_CAR);  //�����433��������	
		}	
		else
			DataCorrect = 1;
	}
	else if(5==task_mode)
	{
		if (laser.dis1<=0 && laser.dis8<=0)//�жϴ�צ�������Ƿ�����
		{
			DataCorrect = 0;
			RequestStop(BURN_POOL);  //�����ϳ�433ֹͣ��������	
			RequestStop(BIG_CAR);    //�����433ֹͣ��������
			RequestStart(BIG_CLAW);  //�����צ433��������	
		}	
		else
			DataCorrect = 1;	
	}
}
//���Զ�����X�����ƶ�
void HXMoxing(float x)
{
	if((0!=HTaskModeFlag)||(0==DataCorrect))
	{
		DataCommunicateManage(HTaskModeFlag);
	}
	else if(1==DataCorrect)//��������
	{
		
		XMoving(x);
	}
}
//����ֹͣ
void RevStop(float err)
{
	if(err>0)
	{
		//���г����Ϸ����ƶ�	
		if(ReverseStop == 0)
		{
			CAR_NORTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s��
			{				
				CAR_SOUTH(ON);				
			}
			else
			{
				ReverseStop++;
				ReverseTime = 0;
			}	
		}
		else if(ReverseStop==0)
		{
			if(ReverseTime<=2)//0.2sͣ
			{				
				CAR_SOUTH(OFF);				
			}
			else
			{
				ReverseStop--;
				ReverseTime = 0;
				ReverseCount++;
			}					
		}					
	}
	//���г��򱱷����ƶ�
	else
	{
		//���г�����ֹͣ
		if(ReverseStop == 0)
		{
			CAR_SOUTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s��
			{				
				CAR_NORTH(ON);				
			}
			else
			{
				ReverseStop++;
				ReverseTime = 0;
			}	
		}
		else if(ReverseStop==0)
		{
			if(ReverseTime<=2)//0.2sͣ
			{				
				CAR_NORTH(OFF);				
			}
			else
			{
				ReverseStop--;
				ReverseTime = 0;
				ReverseCount++;
			}					
		}			
	}
}
//���г��㶯
void HorizontalDotMove(float err_x,float err_y)
{
	if(PointMove == 0)
	{
		ReverseStop = 0;
		ReverseTime = 0;
		PointMove = 1;
	}
	else if(PointMove == 1)
	{
		if(PointMoveTime<10)
		{
			if(err_x>0)	CAR_NORTH(ON);
			else if(err_x<0) CAR_SOUTH(ON);
			else if(err_y>0) CAR_EAST(ON);
			else if(err_y<0) CAR_WEST(ON);				
		}
		else
		{
			PointMove++;
			PointMoveTime=0;
		}
	}
	else if(PointMove == 2)
	{
		if(PointMoveTime<10)
		{
			if(err_x>0)	{CAR_NORTH(OFF);}
			else if(err_x<0) {CAR_SOUTH(OFF);}
			else if(err_y>0) {CAR_EAST(OFF);}
			else if(err_y<0) {CAR_WEST(OFF);}
		}
		else
		{
			PointMove--;
			PointMoveTime=0;
		}				
	}	
}

//��צ�ӵ㶯
void VerticalDotMove(float err_z)
{
	if(PointMove == 0)
	{
		PointMove = 1;
	}
	else if(PointMove == 1)
	{
		if(PointMoveTime<10)
		{
			if(err_z>0)	
			{
				PAW_DOWN(ON);		
			}	
		}
		else
		{
			PointMove++;
			PointMoveTime=0;
		}
	}
	else if(PointMove == 2)
	{
		if(PointMoveTime<10)
		{
			if(err_z>0)	PAW_DOWN(OFF);
		}
		else
		{
			PointMove--;
			PointMoveTime=0;
		}				
	}	
}//��λ��־λ
void ResetFlagBit(void)
{
	 X_MOVE_BIT=0;
	 Y_MOVE_BIT=0;
	 DOWN_BIT=0;
	 UP_BIT=0;	
}
