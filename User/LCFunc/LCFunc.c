 /*
 *************************************************************************
 * LCFunc.c  
 * ��Ҫ�����г���ײ��ÿһ�����ڵ�ֱ�ӿ��ƣ����Ҫ����
 * ��Ͽ��ƣ�ֻ��Ҫ������ĺ����������
 *************************************************************************  
 */

#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       ȫ�ֱ���
 *************************************************************************  
 */
 
 //0:û�����У�1���������н�����2�����ݳ��Ĺ��ϲ����ѽ���
uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;

//0:���ݲ�������1:��������
uint8_t DataCorrect = 0; 

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

//��צ�����Ͽ�����ʱ�õ�
uint8_t UpOrDown = 0;//0�����ϰ벿��1�����°벿

//1����ʾ��£
uint8_t CloseFlag = 0;
//��£��ʱ
int     CloseDelay = 0;
//1:��ʾ��
uint8_t OpenFlag = 0;
//����ʱ
int     OpenDelay = 0;

int64_t Big_Claw_Up_Delay = 0;//��ʱʱ�� S
uint8_t Big_Claw_Up_Delay_Flag = 0;//�򿪶�ʱ����־λ

int64_t Big_Claw_Up_Delay_Pool = 0;
uint8_t Big_Claw_Up_Delay_Pool_Flag = 0;

/*
*****************************************************************************************************************
*                                     void ClosePaw(void) 
*
*Description : �Ͻ�צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ�ô˺���ǰCloseFlag��1���˹��ܽ�����ὫCloseFlag��CloseDelay����
*****************************************************************************************************************
*/
void ClosePaw(void)
{
	if(1 == CloseFlag)
	{
		if(CloseDelay<100)
		{
			PAW_CLOSE(ON);
		}
		else
		{
			PAW_CLOSE(OFF);	
			CloseFlag = 0;
			CloseDelay = 0;
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ReleasePaw(void)
*
*Description : �ɿ�צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ�ô˺���ǰOpenFlag��1���˹��ܽ�����ὫOpenFlag��OpenDelay����
*****************************************************************************************************************
*/
void ReleasePaw(void)
{
	if(1 == OpenFlag)
	{
		if(OpenDelay<100)
		{
			PAW_RELEASE(ON);
		}
		else
		{
			PAW_RELEASE(OFF);	
			OpenFlag = 0;
			OpenDelay = 0;
		}
	}
}
/*
*****************************************************************************************************************
*                                     void XMoving(float x)(�����䣬Ŀǰֻ����һ·���⣬Ҫ����·)
*
*Description : X�����˶�
*Arguments   : float x��Ŀ��λ��x����
*Returns     : none
*Notes       : X�����ƶ�ʹ�ü���laser.dis6
*****************************************************************************************************************
*/
void XMoving(float x)
{
	float err_x = 0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis6<0)//�˳�ż�����ֵĴ���ֵ
	{
		if(same_dis_count<10)
		{
			same_dis_count++;
			laser.dis6=laser.last_dis6;
		}
		else//һֱ���ָ�ֵ��ֹͣ�˶���������
		{
			CAR_NORTH(OFF);
			CAR_SOUTH(OFF);
			same_dis_count=0;
			X_MOVE_BIT = 2;			
		}
	}		
	else
	{
		same_dis_count=0;
	}
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
				
				ReverseCount = 0;
				PointMove = 0;
				PointMoveTime = 0;
				X_MOVE_BIT = 1;
			}
			
		}
	}
}
/*
*****************************************************************************************************************
*                                     void YMoving(float y)
*
*Description : Y�����˶�
*Arguments   : float y��Ŀ��λ��y����
*Returns     : none
*Notes       : Y�����ƶ�ʹ�ü���laser.dis7
*****************************************************************************************************************
*/
void YMoving(float y)
{
	float err_y = 0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis7<0)//�˳�ż�����ֵĴ���ֵ
	{
		if(same_dis_count<10)
		{
			same_dis_count++;
			laser.dis7=laser.last_dis7;
		}
		else//һֱ���ָ�ֵ��ֹͣ�˶���������
		{
			CAR_WEST(OFF);
			CAR_EAST(OFF);
			same_dis_count=0;
			Y_MOVE_BIT = 2;			
		}
	}		
	else
	{
		same_dis_count=0;
	}
	err_y = laser.dis7-y;
	laser.last_dis7 = laser.dis7;   	
	
	
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
				
				PointMove = 0;
				PointMoveTime = 0;	
				Y_MOVE_BIT = 1;							
			}
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void UpPawFromBurnPool(float z)
*
*Description : צ���������򣨷��ϳ������������ô�צ������ļ���
*Arguments   : float z��������ݶ��ĸ߶�
*Returns     : none
*Notes       : ��ʱ�ü���laser.dis8
*****************************************************************************************************************
*/
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
			
			UP_BIT = 1;
		}	
	}
	else
	{
		same_dis_count=0;
	}
	paw_err_last=paw_err;			
}
/*
*****************************************************************************************************************
*                                     void UpPawFromPlatform(float z)
*
*Description : צ���������򣨴���¥ƽ̨�������������漤�⣩
*Arguments   : float z���������¥ƽ̨�ĸ߶�
*Returns     : none
*Notes       : ��ʱ�ü���laser.dis1
*****************************************************************************************************************
*/
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
			
			UP_BIT = 1;
		}	
	}
	else
	{
		same_dis_count=0;
	}
	paw_err_last=paw_err;			
}
/*
*****************************************************************************************************************
*                                     void UpPawFromLitterPool(float z)
*
*Description : צ���������򣨴��Ͽӳ�������
*Arguments   : float z��������ݶ��ĸ߶�
*Returns     : none
*Notes       : ����������ļ���laser.dis1������������ļ���laser.dis8
*****************************************************************************************************************
*/
void UpPawFromLitterPool(float z)
{
  //	dis8  ������24�׵İ�
	/*
    ��צ��ץ�Ϻ�����������Ҫ�õ������������⴫������
		���ϰ벿��ʱ�������ϴ�ļ��⴫����
		���°벿��ʱ�������´�ļ��⴫����
	*/
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;


	if(1==UpOrDown)//���°벿�֣���������ļ��� 12�� dis1
	{
		if((laser.dis1>0 && laser.dis1<10000) && laser.dis8<0)
		{
			PAW_DOWN(ON);	
		}
		
		if (laser.dis1<0 && laser.dis8>0)//��������ת����һ�����߼�
		{
			same_dis_count = same_dis_count+1;			
			if (same_dis_count>5)
			{
				UpOrDown = 0;
				same_dis_count=0;
				laser.last_dis8 = laser.dis8;   
			}	
		}
		else
		{
			same_dis_count=0;
		}	
	}
	else if(0==UpOrDown)//���ϰ벿�֣���������ļ��⣬24�ף�dis8
	{
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
				
				UP_BIT = 1;
			}	
		}
		else
		{
			same_dis_count=0;
		}
		paw_err_last=paw_err;		
		}
}
/*
*****************************************************************************************************************
*                                     void DownPawToBurnPool(float z)
*
*Description : צ���½������½������ϳأ����ô�צ������ļ���
*Arguments   : float z��������ݶ��ĸ߶�
*Returns     : none
*Notes       : ��ʱ�ü���laser.dis8
*****************************************************************************************************************
*/
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
		
		DOWN_BIT = 1;//�½���ɱ�־λ��1
	}
	/*���2���ڵ㶯ʱצ����б����*/
	if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
	{
		PAW_DOWN(OFF);
		
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
			
			DOWN_BIT = 1;
		}	
	}
	else
	{
		same_dis_count=0;
	}
	paw_err_last=paw_err;		
}
/*
*****************************************************************************************************************
*                                     void DownPawToLitterPool(float z)
*
*Description : צ���½������½����Ͽ�ץ�ϣ����������漤�⣬�������漤�⣩
*Arguments   : float z���������������ĸ߶ȣ����ϰ벿����ֵΪ��ֵ
*Returns     : none
*Notes       : ����������ļ���laser.dis8������������ļ���laser.dis1
*****************************************************************************************************************
*/
void DownPawToLitterPool(float z)
{
  //	dis8  ������24�׵İ�
	/*
		��צ���½�ץ��ʱ�½��Ĺ�����Ҫ�õ������������⴫������
		���ϰ벿��ʱ�������ϴ�ļ��⴫����
		���°벿��ʱ�������´�ļ��⴫����
	*/
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if(0==UpOrDown)//��������ļ���  24��  dis8
	{		
		if((laser.dis8>0 && laser.dis8<21000) && laser.dis1<0)
		{
			PAW_DOWN(ON);	
		}
		if (laser.dis8<0 && laser.dis1>0)
		{
			same_dis_count = same_dis_count+1;			
			if (same_dis_count>5)
			{
				UpOrDown = 1;
				same_dis_count=0;
				laser.last_dis1 = laser.dis1; 
			}	
		}
		else
		{
			same_dis_count=0;
		}
	}
	else if(1==UpOrDown)//��������ļ��� 12�� dis1
	{
		if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
		{
			laser.dis1=laser.last_dis1;
		}				
		paw_err = laser.dis1 - z;   
		laser.last_dis1 = laser.dis1;   
			
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
				
				PointMove = 0;
				PointMoveTime = 0;	
				DOWN_BIT = 1;
			}
			/*���2���ڵ㶯ʱצ����б����*/
			if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
			{
				PAW_DOWN(OFF);
				
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
					
					PointMove = 0;
					PointMoveTime = 0;
					same_dis_count=0;				
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
}
/*
*****************************************************************************************************************
*                                     void DownPawToPlatform(float z)
*
*Description : צ���½������½���5¥ƽ̨��(�����´�ļ���)
*Arguments   : float z���������¥ƽ̨�ĸ߶�
*Returns     : none
*Notes       : ��ʱ�ü���laser.dis1
*****************************************************************************************************************
*/
void DownPawToPlatform(float z)
{
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis1<0)//�˳�ż�����ֵĴ���ֵ
	{
		laser.dis1=laser.last_dis1;
	}
	paw_err = laser.dis1 - z;   
	laser.last_dis1 = laser.dis1;   
		
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
			
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		}
		/*���2���ڵ㶯ʱצ����б����*/
		if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//צ����б����һ���Ƕȣ�ֹͣ�½�������ѭ��
		{
			PAW_DOWN(OFF);
			
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
				
				PointMove = 0;
				PointMoveTime = 0;
				same_dis_count=0;				
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
//	uint8_t count1=0;
//	uint8_t count2=0;
//	
//  RelayOn();//��ң����
//	RequestStart(BIG_CAR);//�����433��������
//	
//	while ((laser.dis6<=0)||(laser.dis7<=0))//�жϴ��г������Ƿ�����
//	{
//		RequestStart(BIG_CAR); //�����433��������	
//		
//		count1++;
//		if(count1>10)//�����������10�λ�û�еõ���ȷ���ݣ����������ݴ�����·�����⣬��Ҫ���
//		{
//			break;
//		}
//	}
//	if(count1<=10)//��������
//	{
//		/*
//			�����������ϴ����ݣ��˶κ�����Ҫ����
//		*/
//		
//		//�ж��г�λ��
//		if((abs(laser.dis6-ORIGIN_X)<200)&&(abs(laser.dis7-ORIGIN_Y)<200))
//		{
//			//(x,y)�����ڳ�ʼλ�ã������ж�z�������Ƿ��ڳ�ʼλ�ø���
//			RequestStop(BIG_CAR); //�����433ֹͣ��������	
//			RequestStart(BIG_CLAW); //�����צ433��������	
//			
//			while (laser.dis1<=0)//�жϴ�צ�ϴ������Ƿ�����
//			{
//				count2++;
//				if(count2>10)//�����������10�λ�û�еõ���ȷ���ݣ����������ݴ�����·�����⣬��Ҫ���
//				{
//					break;
//				}
//				for (uint8_t i = 0; i < 5; i++)
//				{
//					RequestStop(BIG_CAR); //�����433ֹͣ��������	
//					RequestStart(BIG_CLAW); //�����ץ433��������	
//				}			 
//			}		
//			if(abs(laser.dis1-ORIGIN_Z)<200)
//			{
//				/*����Ҫ��*/
//				RequestStop(BIG_CLAW); //�����צ433ֹͣ��������	
//			}
//			else
//			{
//				VerticalMoveOrigin(ORIGIN_Z);//�ƶ�����ʼλ��
//			}
//		}
//		else
//		{
//			//�����жϴ�צ�������������Ϸ�����ƽ̨�Ϸ�
//			if(laser.dis6<ADD_X)//��ƽ̨�Ϸ�
//			{		
//				RisePawFromBurnPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);
//			}
//			else//���������Ϸ�
//			{
//				RisePawFromLitterPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);				
//			}			
//		}
//	}
//	ResetFlagBit();
//	RelayOff();//�ر�ң���� 
//	
}
/*
*****************************************************************************************************************
*                                     void DataCommunicateManage(uint8_t task_mode)(��Ҫ����)
*
*Description : ����ͨѶ�������ڴ�ĳһ·�ϴ���������
*Arguments   : uint8_t task_mode������ѡ��ͬ�豸�ϵļ���
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
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
/*
*****************************************************************************************************************
*                                    void RevStop(float err)
*
*Description : ����ֹͣ
*Arguments   : float err��ʵ��λ�þ�Ŀ��λ��ƫ��
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
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
/*
*****************************************************************************************************************
*                                    void HorizontalDotMove(float err_x,float err_y)
*
*Description : ���г��㶯
*Arguments   : float err_x,float err_y������λ��ƫ����õ���һ���ᴫ0
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
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

/*
*****************************************************************************************************************
*                                    void VerticalDotMove(float err_z)
*
*Description : ��צ�ӵ㶯
*Arguments   : float err_z������λ��ƫ��
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
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
/*
*****************************************************************************************************************
*                                    void ResetFlagBit(void)
*
*Description : ��λ��־λ
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ResetFlagBit(void)
{
	 X_MOVE_BIT=0;
	 Y_MOVE_BIT=0;
	 DOWN_BIT=0;
	 UP_BIT=0;	
}
