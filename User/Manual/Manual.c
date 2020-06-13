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
extern uint8_t Run_Mode;

//�ֶ�״̬�µ����в��� 1:ֹͣ,2:X,3:Y,4:��,4:��,6:ץ,7:��,
uint8_t HTaskModeFlag = 0;

uint8_t ManualError = 0;

int8_t ManualPower;
#if Manual == 1  //�ֶ�ģʽ1
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
	if((1==BigCarDataCorrect)&&(RelayOnflag == -2))//��������
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
				Run_Mode = 0;//ģʽ��λ
				
			}
		}
		else if(2==X_MOVE_BIT)//�����쳣���½���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
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
	if((1==BigCarDataCorrect)&&(RelayOnflag == -2))//��������
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
				Run_Mode = 0;//ģʽ��λ
				Y_MOVE_BIT = 0;//��λ
			}
		}
		else if(2==Y_MOVE_BIT)//���н���������
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				Y_MOVE_BIT = 0;//��λ
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
	if((CloseFlag==0)&&(RelayOnflag == -2))
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
		ManualError = 1;//��ʾ����
		if(RelayOffflag==-2)
		{
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
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
	if((OpenFlag == 0)&&(RelayOnflag == -2))
	{
		OpenFlag=1;
	}
	else if(OpenFlag==1)
	{
		ReleasePaw();
	}
	else if(OpenFlag==2)
	{
		PowerOff();//�г��ϵ�
		ManualError = 1;//��ʾ����
		if(RelayOffflag==-2)
		{	
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			RelayOnflag = -1;//��λ
			RelayOffflag = -1;//��λ
			HTaskModeFlag = 0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			OpenFlag = 0;//��λ 
		}		
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualRaisePawFromLitterPool(float z)
*
*Description : ��צ�Ӵ�������̧��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRaisePawFromLitterPool(float z)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(UP_BIT == 0)//��������
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}
		}
		else if(UP_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{		
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}	
		}
	}		
}
/*
*****************************************************************************************************************
*                                    void RisePawFromPlatform(float z)
*
*Description : ����¥ƽ̨̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ��������ļ���laser.dis1
*****************************************************************************************************************
*/
void ManualRisePawFromPlatform(float z)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}
		}
		else if(UP_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}	
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualRisePawFromBurnPool(float z)
*
*Description : �ӷ��ճ�̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRisePawFromBurnPool(float z)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			/*��������*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}
		}
		else if(UP_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}
		}
	}	
}
/*
*****************************************************************************************************************
*                                   void ManualDowntoLitterPool(float z)
*
*Description : ��ֱ�½����ϳصײ�ץ��
*Arguments   : z:צ���½����������ĸ߶�
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDowntoLitterPool(float z)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------	
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)//�������н���
		{
			/*�½�����*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				DOWN_BIT = 0;//��λ
			}
		}
		else if(DOWN_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				DOWN_BIT = 0;//��λ
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                void ManualDownClawtoBurnPool(float z)
*
*Description : �·�צ�������ϳ�
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDownClawtoBurnPool(float z)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToBurnPool(z);		
		}
		else if(DOWN_BIT == 1)//�������н���
		{
			/*�½�����*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				DOWN_BIT = 0;//��λ
			}
		}
		else if(DOWN_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				DOWN_BIT = 0;//��λ
			}
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualDownToOrigin(float z)
*
*Description : ��ֱ���䵽��ʼλ��
*Arguments   : none
*Returns     : none
*Notes       : �ô�צ��������ļ���
*****************************************************************************************************************
*/
void ManualDownToOrigin(float z)
{
	//------------------------------------------------------------
	PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToPlatform(z);		
			
		}
		else if(DOWN_BIT == 1)//�������н���
		{
			/*�½�����*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				DOWN_BIT = 0;//��λ
			}
		}
		else if(DOWN_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			if(RelayOffflag==-2)
			{
				Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				DOWN_BIT = 0;//��λ
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualPowerOn(void)
*
*Description : ��ң����
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualPowerOn(void){
	
	//------------------------------------------------------------	
	PowerOn();//�г��ϵ�
	//------------------------------------------------------------
	if(RelayOnflag == -2){//ң�����Ѵ�
		/*����һ���Ѵ�ң������ָ��*/
		Up_Data.Status = (Up_Data.Status&0x87)|0x60;//��ʱ��״ֵ̬  ң�����Ѵ�	
		WaitFlag = 0;
		Run_Mode = 0; 
		HTaskModeFlag = 0;
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualPowerOff(void)
*
*Description : �ر�ң����
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualPowerOff(void){
	//------------------------------------------------------------
	PowerOff();//�ر�ң����
	//------------------------------------------------------------
	if(RelayOffflag == -2){//ң�����ѹر�
		/*����һ���ѹر�ң������ָ��*/
		Up_Data.Status = (Up_Data.Status&0x87)|0x68;//��ʱ��״ֵ̬  ң�����ѹر�
		WaitFlag = 0;
		Run_Mode = 0; 
		HTaskModeFlag = 0;	
		RelayOnflag = -1;
		RelayOnflag = -1;		
	}
}
#elif Manual == 2  //�ֶ�ģʽ2     
/*
*****************************************************************************************************************
*                                     void MoveToNorth(void)
*
*Description : �г�������(X+)
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualMoveToNorth(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //-----------------------------------------------------------
	if((1==BigCarDataCorrect)&&(RelayOnflag == -2)){
		CAR_NORTH(ON);
		/*�˴�Ҫ����λ*/
	}
}
/*
*****************************************************************************************************************
*                                     void MoveToSouth(void)
*
*Description : �г���������(X-)
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualMoveToSouth(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //-----------------------------------------------------------
	 if((1==BigCarDataCorrect)&&(RelayOnflag == -2)){
		CAR_SOUTH(ON);
		/*�˴�Ҫ����λ*/
	}
}
/*
*****************************************************************************************************************
*                                     void MoveToWest(void)
*
*Description : �г���������(Y-)
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualMoveToWest(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	 if((1==BigCarDataCorrect)&&(RelayOnflag == -2)){
		CAR_WEST(ON);
		/*�˴�Ҫ����λ*/
	}	
}
/*
*****************************************************************************************************************
*                                     void MoveToEast(void)
*
*Description : �г�������(Y-)
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualMoveToEast(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	 if((1==BigCarDataCorrect)&&(RelayOnflag == -2)){
		CAR_EAST(ON);
		/*�˴�Ҫ����λ*/
	}		
}
/*
*****************************************************************************************************************
*                                     void RaisePaw(void)
*
*Description : ����צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRaisePaw(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2)){
		PAW_UP(ON);
		/*�˴�Ҫ����λ*/
	}			
}
/*
*****************************************************************************************************************
*                                     void DownPaw(void)
*
*Description : ����צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDownPaw(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2)){
		PAW_DOWN(ON);
		/*�˴�Ҫ����λ*/
	}	
}

/*
*****************************************************************************************************************
*                                     void ManualClosePaw(void)
*
*Description : ����צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualClosePaw(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if(RelayOnflag == -2){
		PAW_CLOSE(ON);
	}
}
/*
*****************************************************************************************************************
*                                     void ManualOpenPaw(void)
*
*Description : �ſ�צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualOpenPaw(void){
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if(RelayOnflag == -2){
		PAW_RELEASE(ON);
	}
}

#elif Manual == 3  //�ֶ�ģʽ3
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
//	PowerOn();//�г��ϵ�
//------------------------------------------------------------	
	if((1==BigCarDataCorrect)&&(RelayOnflag == -2))//��������
	{		
		if(0==X_MOVE_BIT)
		{
				XMoving(x);	
		}
		else if(1==X_MOVE_BIT)//���н���
		{
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			X_MOVE_BIT = 0;//��λ
			Run_Mode = 0;//ģʽ��λ
			PointMove = 0;
			ReverseStop = 0;
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(2==X_MOVE_BIT)//�����쳣���½���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
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
	if((1==BigCarDataCorrect)&&(RelayOnflag == -2))//��������
	{		
		if(0==Y_MOVE_BIT)
		{
				YMoving(y);
		}
		else if(1==Y_MOVE_BIT)//���н���
		{
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			Y_MOVE_BIT = 0;//��λ
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(2==Y_MOVE_BIT)//���н���������
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				Y_MOVE_BIT = 0;//��λ
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
	ClosePaw();

	if(CloseFlag==-2)
	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		HTaskModeFlag=0;//������ģʽ
		WaitFlag = 0;//����ȴ�״̬
		Run_Mode = 0;//ģʽ��λ
		OpenFlag = -1;//��λ	
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
	ReleasePaw();

	if(OpenFlag==-2)
	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		HTaskModeFlag = 0;//������ģʽ
		WaitFlag = 0;//����ȴ�״̬
		Run_Mode = 0;//ģʽ��λ
		CloseFlag = -1;//��λ 	
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualRaisePawFromLitterPool(float z)
*
*Description : ��צ�Ӵ�������̧��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRaisePawFromLitterPool(float z)
{
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(UP_BIT == 0)//��������
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			UP_BIT = 0;//��λ
		}
		else if(UP_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			if(RelayOffflag==-2)
			{		
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}	
		}
	}		
}
/*
*****************************************************************************************************************
*                                    void RisePawFromPlatform(float z)
*
*Description : ����¥ƽ̨̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : ʹ��������ļ���laser.dis1
*****************************************************************************************************************
*/
void ManualRisePawFromPlatform(float z)
{
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			UP_BIT = 0;//��λ
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(UP_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}	
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualRisePawFromBurnPool(float z)
*
*Description : �ӷ��ճ�̧��צ��
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRisePawFromBurnPool(float z)
{
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			/*��������*/
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			UP_BIT = 0;//��λ
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(UP_BIT == 2)//���й����г���
		{
			PowerOff();
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//��λ
				RelayOffflag = -1;//��λ
				HTaskModeFlag=0;//������ģʽ
				WaitFlag = 0;//����ȴ�״̬
				Run_Mode = 0;//ģʽ��λ
				UP_BIT = 0;//��λ
			}
		}
	}	
}
/*
*****************************************************************************************************************
*                                   void ManualDowntoLitterPool(float z)
*
*Description : ��ֱ�½����ϳصײ�ץ��
*Arguments   : z:צ�Ӿ���г��ľ��룬��uwb���
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDowntoLitterPool(float z)
{
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------	
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  //DownPawToLitterPool(z);	
			DeclineBigPawtoLitterPool(z);			
		}
		else if(DOWN_BIT == 1)//�������н���
		{
			/*�½�����*/
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			DOWN_BIT = 0;//��λ
		}
		else if(DOWN_BIT == 2)//���й����г���
		{
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			DOWN_BIT = 0;//��λ
		}
	}
}
/*
*****************************************************************************************************************
*                                void ManualDownClawtoBurnPool(float z)
*
*Description : �·�צ�������ϳ�
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDownClawtoBurnPool(float z)
{
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  //DownPawToBurnPool(z);		
			DeclineBigPawtoBurnPool(z);
		}
		else if(DOWN_BIT == 1)//�������н���
		{
			/*�½�����*/
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			DOWN_BIT = 0;//��λ
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(DOWN_BIT == 2)//���й����г���
		{ 
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			DOWN_BIT = 0;//��λ
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualDownToOrigin(float z)
*
*Description : ��ֱ���䵽��ʼλ��
*Arguments   : none
*Returns     : none
*Notes       : �ô�צ��������ļ���
*****************************************************************************************************************
*/
void ManualDownToOrigin(float z)
{
	//------------------------------------------------------------
	//PowerOn();//�г��ϵ�
  //------------------------------------------------------------
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  //DownPawToPlatform(z);		
			DeclineBigPawtoPlatForm(z);
		}
		else if(DOWN_BIT == 1)//�������н���
		{
			/*�½�����*/
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			DOWN_BIT = 0;//��λ
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(DOWN_BIT == 2)//���й����г���
		{
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			DOWN_BIT = 0;//��λ
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualRaiseBigPaw(float z)
*
*Description : ��צ������̧��
*Arguments   : none
*Returns     : none
*Notes       : z�Ǵ�צ�Ӿ���г��ľ���,��uwb���
*****************************************************************************************************************
*/
void ManualRaiseBigPaw(float z){
	if((1==BigClawDataCorrect)&&(RelayOnflag == -2))//��������
	{
		if(UP_BIT == 0)
		{
		  RaiseBigPaw(z);		
		}
		else if(UP_BIT == 1)//�������н���
		{
			/*��������*/
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			UP_BIT = 0;//��λ
			Up_Data.Status = (Up_Data.Status&0x87)|0x48;//��ʱ��״ֵ̬ �������
		}
		else if(UP_BIT == 2)//���й����г���
		{
			ManualError = 1;//��ʾ����
			Up_Data.Status = Up_Data.Status&0x7F;//��ʱ��״̬  ���г���
			HTaskModeFlag=0;//������ģʽ
			WaitFlag = 0;//����ȴ�״̬
			Run_Mode = 0;//ģʽ��λ
			UP_BIT = 0;//��λ

		}
	}		
}
/*
*****************************************************************************************************************
*                                     void ManualPowerOn(void)
*
*Description : ��ң����
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualPowerOn(void){
	
	//------------------------------------------------------------	
	PowerOn();//�г��ϵ�
	//------------------------------------------------------------
	if(RelayOnflag == -2){//ң�����Ѵ�
		/*����һ���Ѵ�ң������ָ��*/
		Up_Data.Status = (Up_Data.Status&0x87)|0x68;//��ʱ��״ֵ̬  ң�����Ѵ�	
		WaitFlag = 0;
		Run_Mode = 0; 
		HTaskModeFlag = 0;
		RelayOffflag = -1;
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualPowerOff(void)
*
*Description : �ر�ң����
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualPowerOff(void){
	//------------------------------------------------------------
	PowerOff();//�ر�ң����
	//------------------------------------------------------------
	if(RelayOffflag == -2){//ң�����ѹر�
		/*����һ���ѹر�ң������ָ��*/
		Up_Data.Status = (Up_Data.Status&0x87)|0x70;//��ʱ��״ֵ̬  ң�����ѹر�
		WaitFlag = 0;
		Run_Mode = 0; 
		HTaskModeFlag = 0;	
		RelayOnflag = -1;		
	}
}
#endif

