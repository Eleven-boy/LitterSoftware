#include "Manual.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       全局变量
 *************************************************************************  
 */
extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

//手动状态下的运行步骤 1:停止,2:X,3:Y,4:上,4:下,6:抓,7:松,
uint8_t HTaskModeFlag = 0;

uint8_t ManualError = 0;
/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float x)
*
*Description : 手动X方向移动
*Arguments   : 目标点x坐标
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualXMoving(float x)
{
//------------------------------------------------------------
	PowerOn();//行车上电
//------------------------------------------------------------	
	if(1==BigCarDataCorrect)//数据正常
	{		
		if(0==X_MOVE_BIT)
		{
				XMoving(x);	
		}
		else if(1==X_MOVE_BIT)//运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				X_MOVE_BIT = 0;//复位
				Run_Mode = 0;//模式复位
				
			}
		}
		else if(2==X_MOVE_BIT)//运行异常导致结束
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{
				
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				X_MOVE_BIT = 0;//复位
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualXMoxing(float y)
*
*Description : 手动Y方向移动
*Arguments   : 目标点y坐标
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualYMoving(float y)
{
//------------------------------------------------------------
	PowerOn();//行车上电
//------------------------------------------------------------	
	if(1==BigCarDataCorrect)//数据正常
	{		
		if(0==Y_MOVE_BIT)
		{
				YMoving(y);
		}
		else if(1==Y_MOVE_BIT)//运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				Y_MOVE_BIT = 0;//复位
			}
		}
		else if(2==Y_MOVE_BIT)//运行结束，出错
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{
				
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				Y_MOVE_BIT = 0;//复位
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualClose(void)
*
*Description : 合拢大爪子
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualClose(void)
{
	//------------------------------------------------------------
	PowerOn();//行车上电
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
		PowerOff();//行车断电
		ManualError = 1;//表示出错
		if(RelayOffflag==-2)
		{

			RelayOnflag = -1;//复位
			RelayOffflag = -1;//复位
			HTaskModeFlag=0;//无任务模式
			WaitFlag = 0;//进入等待状态
			CloseFlag = 0;//复位
		}		
	}
}
/*
*****************************************************************************************************************
*                                     void ManualOpen(void)
*
*Description : 张开大爪子
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualOpen(void)
{
	//------------------------------------------------------------
	PowerOn();//行车上电
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
		PowerOff();//行车断电
		ManualError = 1;//表示出错
		if(RelayOffflag==-2)
		{	
			RelayOnflag = -1;//复位
			RelayOffflag = -1;//复位
			HTaskModeFlag=0;//无任务模式
			WaitFlag = 0;//进入等待状态
			OpenFlag = 0;//复位
		}		
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualRaisePawFromLitterPool(float z)
*
*Description : 将爪子从垃圾池抬起
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRaisePawFromLitterPool(float z)
{
	//------------------------------------------------------------
	PowerOn();//行车上电
  //------------------------------------------------------------
	
	if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)//正在运行
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(z);		
		}
		else if(UP_BIT == 1)//正常运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}
		}
		else if(UP_BIT == 2)//运行过程中出错
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{			
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}	
		}
	}		
}
/*
*****************************************************************************************************************
*                                    void RisePawFromPlatform(float z)
*
*Description : 从四楼平台抬升爪子
*Arguments   : none
*Returns     : none
*Notes       : 使用往下射的激光laser.dis1
*****************************************************************************************************************
*/
void ManualRisePawFromPlatform(float z)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(z);		
		}
		else if(UP_BIT == 1)//正常运行结束
		{
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}
		}
		else if(UP_BIT == 2)//运行过程中出错
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}	
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ManualRisePawFromBurnPool(float z)
*
*Description : 从焚烧池抬升爪子
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualRisePawFromBurnPool(float z)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(z);		
		}
		else if(DOWN_BIT == 1)//正常运行结束
		{
			/*上升结束*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}
		}
		else if(DOWN_BIT == 2)//运行过程中出错
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				UP_BIT = 0;//复位
			}
		}
	}	
}
/*
*****************************************************************************************************************
*                                   void ManualDowntoLitterPool(float z)
*
*Description : 竖直下降到料池底部抓料
*Arguments   : z:爪子下降到距垃圾的高度
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDowntoLitterPool(float z)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)//正常运行结束
		{
			/*下降结束*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				DOWN_BIT = 0;//复位
			}
		}
		else if(DOWN_BIT == 2)//运行过程中出错
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				DOWN_BIT = 0;//复位
			}
		}
	}
}
/*
*****************************************************************************************************************
*                                void ManualDownClawtoBurnPool(float z)
*
*Description : 下放爪子至焚料池
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void ManualDownClawtoBurnPool(float z)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)//正常运行结束
		{
			/*下降结束*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				DOWN_BIT = 0;//复位
			}
		}
		else if(DOWN_BIT == 2)//运行过程中出错
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				DOWN_BIT = 0;//复位
			}
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void ManualDownToOrigin(float z)
*
*Description : 竖直降落到初始位置
*Arguments   : none
*Returns     : none
*Notes       : 用大爪子往下射的激光
*****************************************************************************************************************
*/
void ManualDownToOrigin(float z)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToPlatform(z);		
			
		}
		else if(DOWN_BIT == 1)//正常运行结束
		{
			/*下降结束*/
			PowerOff();
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				DOWN_BIT = 0;//复位
			}
		}
		else if(DOWN_BIT == 2)//运行过程中出错
		{
			PowerOff();
			ManualError = 1;//表示出错
			if(RelayOffflag==-2)
			{
				RelayOnflag = -1;//复位
				RelayOffflag = -1;//复位
				HTaskModeFlag=0;//无任务模式
				WaitFlag = 0;//进入等待状态
				Run_Mode = 0;//模式复位
				DOWN_BIT = 0;//复位
			}
		}
	}
}
