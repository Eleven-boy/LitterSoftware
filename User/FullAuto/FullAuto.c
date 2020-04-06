#include "FullAuto.h"
#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h" 

/*
*************************************************************************
*                       全局变量
*************************************************************************
*/

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

extern POSITION origin;//起始位置
extern POSITION target;//目标位置 

uint8_t FullAutoStep = 0;
/*
*****************************************************************************************************************
*                                    void BigCarFullAutoMode(void)
*
*Description : 全自动任务执行函数
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void BigCarFullAutoMode(void)
{
	if(0==FullAutoStep)//行车上电
 	{
		PowerOn();
		if(RelayOnflag==-2)
		{
			FullAutoStep++;//进行下一步
			RelayOnflag=-1;//标志位复位
		}
	}
	else if(1==FullAutoStep)// 从四楼平台抬升爪子
	{
		RisePawFromPlatform();
		if(UP_BIT == 1)//上升结束
		{
			FullAutoStep++;//进行下一步
			UP_BIT=0;//标志位复位
		}
	}
	else if(2==FullAutoStep)//水平移动到抓料处正上方
	{
		target.x[0] = (GARBAGE_X-target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
		HorizontalMoving(target.x[0],target.y[0]);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//进行下一步
			X_MOVE_BIT=0;//标志位复位
			Y_MOVE_BIT=0;//标志位复位
			UpOrDown = 0;//0代表上半部，1代表下半部,为下面下降程序做准备
		}
	}
	else if(3==FullAutoStep)//爪子开始下降去抓料
	{
		DowntoLitterPool(target.z[0]);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//进行下一步
			DOWN_BIT = 0;//标志位复位
			CloseFlag=1;//将下面执行的抓料标志位置1
		}
	}
	else if(4==FullAutoStep)//执行抓料动作
	{
		HFClosePaw();
		if(2==CloseFlag)
		{
			FullAutoStep++;//进行下一步
			CloseFlag = 0; //标志位复位
			UpOrDown = 1;  //0代表上半部，1代表下半部,将爪子上抬标志位置1
		}
	}
	else if(5==FullAutoStep)//执行大爪上升动作
	{
		RisePawFromLitterPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//进行下一步
			UP_BIT = 0;//标志位复位
		}
	}
	else if(6==FullAutoStep)//水平移动至焚烧池上方
	{
		HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//进行下一步
			X_MOVE_BIT=0;  //标志位复位
			Y_MOVE_BIT=0;  //标志位复位
		}
	}
	else if(7==FullAutoStep)//下放大爪子靠近焚烧池
	{
		DownClawtoBurnPool();
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//进行下一步
			DOWN_BIT=0;    //标志位复位	
			OpenFlag=1;    //将松开爪子标志位置1
		}
	}
	else if(7==FullAutoStep)//松开爪子放料
	{
		HFOpenPaw();
		if(2==OpenFlag)
		{
			FullAutoStep++;//进行下一步
			OpenFlag=0;    //标志位复位	
		}
	}
	else if(8==FullAutoStep)//提升爪子至安全高度
	{
		RisePawFromBurnPool();
		if(1==UP_BIT)
		{
			FullAutoStep++;//进行下一步
			UP_BIT = 0;
		}
	}
	else if(9==FullAutoStep)//水平移动到初始位置上方
	{
		HorizontalMoving(ORIGIN_X,ORIGIN_Y);
		if((1==X_MOVE_BIT)&&(1==Y_MOVE_BIT))
		{
			FullAutoStep++;//进行下一步
			X_MOVE_BIT=0;//标志位复位
			Y_MOVE_BIT=0;//标志位复位
		}
	}
	else if(10==FullAutoStep)//将爪子下降到四楼平台
	{
		DownToOrigin(ORIGIN_Z);
		if(1==DOWN_BIT)
		{
			FullAutoStep++;//进行下一步
			DOWN_BIT=0;//标志位复位
		}
	}
	else if(11==FullAutoStep)//断开电源,运行结束
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			RelayOffflag=-1;
			WaitFlag=0;      
			Run_Mode=0;
		}
	}
	if((2==X_MOVE_BIT)||(2==Y_MOVE_BIT)||(2==DOWN_BIT)||(2==UP_BIT))//运行出错
	{
		PowerOff();
		if(RelayOffflag==-2)
		{
			ErrorBigCar = FullAutoStep;//记录出错在哪一步
			RelayOffflag=-1;
			FullAutoStep=0;
			X_MOVE_BIT=0;//标志位复位
			Y_MOVE_BIT=0;//标志位复位
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
*Description : 从四楼平台抬升爪子
*Arguments   : none
*Returns     : none
*Notes       : 使用往下射的激光laser.dis1
*****************************************************************************************************************
*/
void RisePawFromPlatform(void)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(PLATFORM_UZ);		
		}
		else if(UP_BIT == 1)
		{
			/*上升结束*/
		}
	}
}
/*
*****************************************************************************************************************
*                                    void HorizontalMoving(float x,float y)
*
*Description : 水平移动
*Arguments   : （x,y）:目的地坐标
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void HorizontalMoving(float x,float y)
{
	if(1==BigCarDataCorrect)//数据正常
	{
		if(X_MOVE_BIT == 0)
		{
			//大行车X方向移动
			XMoving(x);			
		}
		else if(Y_MOVE_BIT == 0)
		{
			//大行车Y方向移动
			YMoving(y);		
		}
		else
		{
			/*水平移动结束*/
		}
	}
}

/*
*****************************************************************************************************************
*                                   void DowntoLitterPool(float z)
*
*Description : 竖直下降到料池底部抓料
*Arguments   : z:爪子下降到距垃圾的高度
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void DowntoLitterPool(float z)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)
		{
			/*下降结束*/
		}
	}
}
/*
*****************************************************************************************************************
*                                     void HClosePaw(void)
*
*Description : 抓料函数
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
*Description : 从垃圾池抬升爪子
*Arguments   : none
*Returns     : none
*Notes       : 刚开始用往下射的激光，运行到上半部分用往上射的激光
*****************************************************************************************************************
*/
void RisePawFromLitterPool(void)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromLitterPool(BURN_POOL_UZ);		
		}
		else if(UP_BIT == 1)
		{
			/*上升结束*/
		}
	}	
}
/*
*****************************************************************************************************************
*                                void DownClawtoBurnPool(void)
*
*Description : 下放爪子至焚料池
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void DownClawtoBurnPool(void)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(BURN_POOL_DZ);		
		}
		else if(DOWN_BIT == 1)
		{
			/*下放结束*/
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void HFOpenPaw(void)
*
*Description : 放料函数
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
*Description : 从焚烧池抬升爪子
*Arguments   : none
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RisePawFromBurnPool(void)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(BURN_POOL_UZ);		
		}
		else if(DOWN_BIT == 1)
		{
			/*上升结束*/
		}
	}	
}
/*
*****************************************************************************************************************
*                                     void DownToOrigin(void)
*
*Description : 竖直降落到初始位置
*Arguments   : none
*Returns     : none
*Notes       : 用大爪子往下射的激光
*****************************************************************************************************************
*/
void DownToOrigin(float z)
{
	if(1==BigClawDataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToPlatform(z);		
			
		}
		else if(DOWN_BIT == 1)
		{
			/*下降结束*/
		}
	}
}
