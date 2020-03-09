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

void BigCarRunning(void)
{
	/*big car starts running!*/
	RelayOn();//��ң����
	
	RisePawFromBurnPool();//��ʼ����צ��
	Target.x[0] = (GARBAGE_X-Target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
	HorizontalMoving(Target.x[0],Target.y[0]);//ˮƽ�ƶ���������
	VerticalMoving(Target.z[0]); 
	ClosePaw();
	ResetFlagBit(); 
	
	RisePawFromLitterPool();//��������צ��
	HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);//��צ���������ճ����Ϸ�
	LowerClawtoPool();//�½�צ�������ճ�
	ReleasePaw(); 
	ResetFlagBit(); 
	
	RisePawFromBurnPool();//�ӷ��ճ�������צ��
	HorizontalMoving(Origin.x[0],Origin.y[0]);//ˮƽ�ƶ�֮��ʼλ��
	VerticalMoveOrigin(Origin.z[0]);//�½�����¥ƽ̨
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
				RequestStart(BIG_CAR); //�����433��������	
		}
	}
	
	err_x = laser.dis6-x;
	err_y = laser.dis7-y;
	
	//���г�X�����ƶ�
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
		else if( (err_x>0) && (0==X_MOVE_BIT) )//���г���ʼ���˶�
		{
			//���г���ʼ���˶�
			CAR_NORTH(ON);
			while(err_x>2000)
			{
					//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
					delay_ms(100); 
					err_x = laser.dis6 - x;    //24��             
			}  			
			CAR_NORTH(OFF);
			//���г������ƶ�
			for (uint8_t i = 0; i < 2; i++)//���г������ƶ�
			{
					CAR_SOUTH(ON);
					for (uint8_t j = 0; j < 10; j++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_x = laser.dis6 - x;    //24��
					}
					CAR_SOUTH(OFF);
					delay_ms(1000);
			}	
			X_MOVE_BIT = 1;
		}
		delay_ms(100);
	}
	//���г�X����ʼ�㶯
	if((abs(err_x)<2000)||(1==X_MOVE_BIT))//���г�X����㶯ģʽ
	{
		CAR_SOUTH(OFF);
		CAR_NORTH(OFF);
		if(err_x<0)//���г����ϵ㶯
		{
			while(err_x<(-200))
			{
					CAR_SOUTH(ON);
					for (uint8_t i = 0; i < 8; i++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_x = laser.dis6 - x;    //24��
					}
					CAR_SOUTH(OFF);
					delay_ms(1000); 
			}
			CAR_SOUTH(OFF);			
		}
		else if(err_x>=0)//���г��򱱵㶯
		{
			while(err_x>200)
			{
					CAR_NORTH(ON);
					for (uint8_t i = 0; i < 8; i++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_x = laser.dis6 - x;    //24��  
					}
					CAR_NORTH(OFF);
					delay_ms(500);
			}
			CAR_NORTH(OFF);			
		}
		X_MOVE_BIT = 2; 
	}
		
	//���г�Y�����ƶ�
	while(abs(err_y)>2000)//���г�Y�����ƶ�
	{
		err_x = laser.dis6 - x;    //24��
		err_y = laser.dis7 - y;    //12��
		
		//Now is moving to the Y destnation
		if ((err_y>0)&&(0==Y_MOVE_BIT)&&(2==X_MOVE_BIT))//���г����˶�
		{
			CAR_EAST(ON); 
			while(abs(err_y)>2000)
			{
				//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
				delay_ms(100); 
				err_y = laser.dis7 - y;    //12��
			}
			Y_MOVE_BIT = 1;
			CAR_EAST(OFF); 
		}
		else if ((err_y<0) && (0==Y_MOVE_BIT)&&(2==X_MOVE_BIT))//���г������˶�
		{
			CAR_WEST(ON);
			while(abs(err_y)>2000)
			{
				//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
				delay_ms(100); 
				err_y = laser.dis7 - y;    //12��		
			}
			Y_MOVE_BIT = 1;
			CAR_WEST(OFF);
		}	
		delay_ms(100);
	}
	//���г�Y����㶯
	if ((abs(err_y)<2000)||(1==Y_MOVE_BIT))//���г�Y����㶯
	{
		CAR_WEST(OFF);
		CAR_EAST(OFF);
		while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//�ȴ�mpu6050�����ȶ�
		{
			 
		}   		
		if(err_y<0)//���г������㶯
		{
			while(err_y<(-200))
			{
					CAR_WEST(ON);
					for (uint8_t i = 0; i < 8; i++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_y = laser.dis7 - y;    //12��
					}
					CAR_WEST(OFF);
					delay_ms(500);
			}
			CAR_WEST(OFF);
		}
		else if(err_y>0)
		{
			while(err_y>200)
			{
					CAR_EAST(ON);
					for (uint8_t i = 0; i < 8; i++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_y = laser.dis7 - y;    //12��
					}
					CAR_EAST(OFF);
					delay_ms(500);
			}
			CAR_EAST(OFF);		
		}
		Y_MOVE_BIT = 2; 
		RequestStop(BIG_CAR);//������г�433ֹͣ���ݷ���
	}		
}
//��ֱ�ƶ�
void VerticalMoving(float z)
{
	float paw_err=0; 
	float paw_nowDis = 0;
	float paw_err_last=0;
	uint8_t count = 0;
	uint8_t same_dis_count=0;
	
	RequestStop(BIG_CAR); //�����433ֹͣ��������	
	RequestStop(BURN_POOL); //�����Ͽ�433ֹͣ��������	
	RequestStart(BIG_CLAW); //�����ץ433��������		
	
	while (laser.dis1<=0)//�жϴ�צ�ϴ������Ƿ�����
	{
		PAW_DOWN(ON);
		count++;
		if (count>5)
		{   
				for (uint8_t i = 0; i < 5; i++)
				{
					RequestStop(BIG_CAR); //�����433ֹͣ��������	
					RequestStop(BURN_POOL); //�����Ͽ�433ֹͣ��������	
					RequestStart(BIG_CLAW); //�����ץ433��������	
				}
				count=0;
		}		
		PAW_DOWN(OFF);
		delay_ms(10);
	}
	paw_err = laser.dis1 - z;
	//��צ�½�
	while(abs(paw_err)>1000)//��צ�½�
	{
		if((paw_err>=0)&&(DOWN_BIT==0))//��צ�½�
		{
			PAW_DOWN(ON);
			while(paw_err>1000)
			{                 
				//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
				delay_ms(100);   
				if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
				{
					laser.dis1=laser.last_dis1;
				}				
				paw_nowDis=laser.dis1;     
				paw_err = paw_nowDis - z;    
				laser.last_dis1 = laser.dis1;   
			}			
			PAW_DOWN(OFF);
			DOWN_BIT=1;  //�½���־λ
		}
		paw_err = laser.dis1 - z;
		delay_ms(100);
	}
	//��צ�½���ָ���߶ȣ�׼���㶯
	if((abs(paw_err)<1000)&&(1==DOWN_BIT))//��צ�½���ָ���߶ȣ�׼���㶯
 	{
		while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//�ȴ�mpu6050�����ȶ�
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
			delay_ms(100);
		}
		PAW_DOWN(ON);
		while(paw_err>200)//��ʼ�㶯
		{
			PAW_DOWN(ON);
			for (uint8_t i = 0; i < 10; i++)//�㶯����
			{
				//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
				delay_ms(100);  
				paw_nowDis=laser.dis1;        
				paw_err = paw_nowDis - z;      
			}
			PAW_DOWN(OFF);		
			delay_ms(500); 
			if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
			{
					PAW_DOWN(OFF);
					break;
			} 
			
			if (abs(paw_err-paw_err_last)<100)//�����Ѿ��½����ײ����������½������
			{
					same_dis_count = same_dis_count+1;
			}
			else
			{
					same_dis_count=0;
			}
			
			if (same_dis_count>5)
			{
					PAW_DOWN(OFF);
					break;
			}	
			paw_err_last=paw_err;			
		}
		PAW_DOWN(OFF);
		//��צ�½�����
		RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
	}
}
//��ֱ���䵽��ʼλ��
void VerticalMoveOrigin(float z)
{
	float paw_err=0; 
	float paw_nowDis = 0;
	float paw_err_last=0;
	uint8_t count = 0;
	uint8_t same_dis_count=0;	
	
	RequestStop(BIG_CAR); //�����433ֹͣ��������	
	RequestStop(BURN_POOL); //�����Ͽ�433ֹͣ��������	
	RequestStart(BIG_CLAW); //�����ץ433��������		
	
	while (laser.dis1<=0)//�жϴ�צ�ϴ������Ƿ�����
	{
		count++;
		if (count>5)
		{   
				for (uint8_t i = 0; i < 5; i++)
				{
					RequestStop(BIG_CAR); //�����433ֹͣ��������	
					RequestStop(BURN_POOL); //�����Ͽ�433ֹͣ��������	
					RequestStart(BIG_CLAW); //�����ץ433��������	
				}
				count=0;
		}		
		delay_ms(100);
	}
	paw_err = laser.dis1 - z;
	//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
	
	while(paw_err>800)//�ж��Ƿ�Ҫ�½�
	{
		if((paw_err>=0)&&(DOWN_BIT==0))//����������צ�������ƶ�
		{
			PAW_DOWN(ON);
			while(paw_err>800)
			{                 
				//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
				delay_ms(100);   
				if (laser.dis1<0)
				{
						laser.dis1=3000;
				}
				paw_nowDis=laser.dis1;    
				paw_err = paw_nowDis - z;     
			}				
			PAW_DOWN(OFF);
			DOWN_BIT=1;
		}
		delay_ms(100);  
		paw_err = laser.dis1 - z;
	}
	//צ��׼���㶯�½�
	if((abs(paw_err)<800)&&(1==DOWN_BIT))
	{
		while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//�ȴ�mpu6050�����ȶ�
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
			delay_ms(100);  
		}	
		PAW_DOWN(ON);
		while(paw_err>100)
		{
			PAW_DOWN(ON);
			for (uint8_t i = 0; i < 4; i++)
			{
				if (laser.dis1<0)
				{
						laser.dis1=2000;
				}
				//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
				delay_ms(100); 
				paw_nowDis=laser.dis1;                 
				paw_err = paw_nowDis - z;      
			}		
			PAW_DOWN(OFF);
			delay_ms(500); 
			
			if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>11.0f))//����½�������צ����бһ���̶ȣ������½�
			{
				PAW_DOWN(OFF);
				break;
			}   
			
			if (abs(paw_err-paw_err_last)<20)//�˳����
			{
				same_dis_count = same_dis_count+1;
			}
			else
			{
				same_dis_count=0;
			}
			
			if (same_dis_count>5)
			{
				PAW_DOWN(OFF);
				break;
			}
			
			paw_err_last=paw_err;
		}
		PAW_DOWN(OFF);
		//צ���½�����
		RequestStop(BIG_CLAW);//�����צ433ֹͣ��������
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
}
//��������̧��צ��
void RisePawFromLitterPool(void)
{
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
}
//�·�צ�����Ͽ�
void LowerClawtoPool(void)
{
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
}
//��λ��־λ
void ResetFlagBit(void)
{
	 X_MOVE_BIT=0;
	 Y_MOVE_BIT=0;
	 DOWN_BIT=0;
	 UP_BIT=0;	
}


