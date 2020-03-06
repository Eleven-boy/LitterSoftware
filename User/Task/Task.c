#include "task.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;
 
int64_t Big_Claw_Up_Delay = 0;//��ʱʱ�� S
uint8_t Big_Claw_Up_Delay_Flag = 0;//�򿪶�ʱ����־λ

int64_t Big_Claw_Up_Delay_Pool = 0;
uint8_t Big_Claw_Up_Delay_Pool_Flag = 0;



void BigCarRunning()
{
	/*big car starts running!*/
	RelayOn();//��ң����
	RisePawFromBurnPool();
	
	RelayOff();//�ر�ң����
}
//ˮƽ�ƶ�
void HorizontalMoving(float x,float y)
{
	float   err_x=0;     // Ĭ��Ϊ0���������ƶ�
	float   err_y=0;
	uint8_t count=0;
	
	RequestStop(BIG_CLAW); //�����צ433ֹͣ��������	
	RequestStart(BIG_CAR); //�����433��������	
	
	while ((laser.dis6<=0)||(laser.dis7<=0))//�жϴ��г������Ƿ�����
	{
		for (uint8_t i = 0; i < 3; i++)
		{
				RequestStop(BIG_CLAW); //�����צ433ֹͣ��������
				delay_ms(200);
				RequestStart(BIG_CAR); //�����433��������	
				delay_ms(200);
		}
	}
	
	err_x = laser.dis6-x;
	err_y = laser.dis7-y;
	
	while(abs(err_x)>2000)//ƫ�����һ����Χʱ��Ҫ�ƶ�
	{
		err_x = laser.dis6 - x;    //24��
		err_y = laser.dis7 - y;    //12��	
		//Now is moving to the X destination
		if( (err_x<0) && (0==X_MOVE_BIT) )//���г������ƶ�
		{
			//���г���ʼ�����ƶ�
			CAR_SOUTH(ON);
			while(err_x<(-2000))
			{
				//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
				delay_ms(100);    
				err_x = laser.dis6 - x;    //24��
			}
			CAR_SOUTH(OFF);
			//���г�����ֹͣ
			for(uint8_t i=0;i<2;i++)
			{
				CAR_NORTH(ON);
				for (uint8_t i = 0; i < 10; i++)
				{
						//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
						delay_ms(100);
						err_x = laser.dis6 - x;    //24�� 
				}				
				CAR_NORTH(OFF);
				delay_ms(1000);
			}
			X_MOVE_BIT = 1;
			//���г���ʼ���ϵ㶯
		}
		else if( (err_x>0) && (0==X_MOVE_BIT) )
		{
			//���г���ʼ���˶�
			 
		}
		delay_ms(100);
	}
	if((abs(err_x)<2000)||(1==X_MOVE_BIT))
	{
	
	}
		
	while(abs(err_y)>2000)
	{}
	if ((abs(err_y)<2000)||(1==Y_MOVE_BIT))
	{}		
}
//��ֱ�ƶ�
void VerticalMoving(float z)
{

}
//��ֵ���䵽��ʼλ��
void VerticalMoveOrigin(float z)
{

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
//�ӷ��ճػ���¥ƽ̨̧��צ��
void RisePawFromBurnPool(void)
{
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
		CAR_UP(ON);//צ�ӿ�ʼ����
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
	CAR_UP(OFF);//צ��ֹͣ����
	
	while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//�ȴ�mpu6050��ֵ�ȶ�
	{
					
	}	
	
	RequestStop(BIG_CAR);//�����433ֹͣ��������
	laser.dis5 = 0.0f;
}
//��������̧��צ��
void RisePawFromLitterPool(void)
{

}


