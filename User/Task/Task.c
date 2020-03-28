#include "task.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       全局变量
 *************************************************************************  
 */
 
 

POSITION origin;//起始位置
POSITION target;//目标位置 

extern uint8_t WaitFlag;

uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;

//0:数据不正常，1:数据正常
uint8_t DataCorrect = 0; 
//半自动状态下的运行步骤 1:停止,2:X,3:Y,4:上,4:下,6:抓,7:松,
uint8_t HTaskModeFlag = 0;
//大行车反向停止标志位
uint8_t ReverseStop = 0;
//反向时间
int ReverseTime = 0; 
//反向操作次数
uint8_t ReverseCount = 0;

//大行车点动标志位
uint8_t PointMove = 0;
//电动时间
int PointMoveTime = 0; 

int64_t Big_Claw_Up_Delay = 0;//延时时间 S
uint8_t Big_Claw_Up_Delay_Flag = 0;//打开定时器标志位

int64_t Big_Claw_Up_Delay_Pool = 0;
uint8_t Big_Claw_Up_Delay_Pool_Flag = 0;

void BigCarTask(void)
{
	BigCarRunning();
}
void BigCarRunning(void)
{
	/*big car starts running!*/
	RelayOn();//打开遥控器

  RisePawFromBurnPool();
	target.x[0] = (GARBAGE_X-target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
	HorizontalMoving(target.x[0],target.y[0]);//水平移动爪子至垃圾池
	VerticalMoving(target.z[0]);//爪子下降
	ClosePaw();
	ResetFlagBit(); 
	
	RisePawFromLitterPool();//向上提升爪子
	HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);//将爪子移至焚烧池正上方
	LowerClawtoBurnPool();//下降爪子至焚烧池
	ReleasePaw(); 
	ResetFlagBit(); 
	
	RisePawFromBurnPool();//从焚烧池向上提爪子
	HorizontalMoving(origin.x[0],origin.y[0]);//水平移动之初始位置
	VerticalMoveOrigin(origin.z[0]);//下降至四楼平台
	RelayOff();//关闭遥控器 
}
//水平移动
void HorizontalMoving(float x,float y)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag);//请求数据
	}
	else if(1==DataCorrect)//数据正常
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
			RequestStop(BIG_CAR); //请求大车433停止发送数据	
		}
	}
}
//竖直移动去抓料
void VerticalMoving(float z)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag);//请求数据
		laser.last_dis1 = laser.dis1; //先给laser.last_dis1初始化一个值
		laser.last_dis8 = laser.dis8; //先给laser.last_dis8初始化一个值
	}
	else if(1==DataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToPlatform(z);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}
}
//竖直降落到初始位置
void VerticalMoveOrigin(float z)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag);//请求数据
		laser.last_dis1 = laser.dis1; //先给laser.last_dis1初始化一个值

	}
	else if(1==DataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(z);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}
}
//合紧爪子
void ClosePaw(void)
{
		PAW_CLOSE(ON);
		for (int i = 0; i < 20; i++)
		{
				delay_ms(1000);
		}
		PAW_CLOSE(OFF);	
}
//松开爪子
void ReleasePaw(void)
{
		PAW_RELEASE(ON);
		for (int i = 0; i < 10; i++)
		{  
				delay_ms(1000);   
		}
		PAW_RELEASE(OFF);	
}
//从焚烧池抬升爪子
void RisePawFromBurnPool(void)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag);//请求数据
		laser.last_dis8 = laser.dis8; //先给laser.last_dis8初始化一个值

	}
	else if(1==DataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromBurnPool(BURN_POOL_UZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}		
	/*
	float err = 0.0f;
  uint8_t count=0;
  uint8_t stop_count=0;	
	
	laser.last_dis5 = laser.dis5;
	RequestStart(BIG_CAR);//请求大车433发送数据
	err=laser.last_dis5 - laser.dis5;	
	
	while(err == 0.0f)
	{
		laser.last_dis5 = laser.dis5;
		RequestStart(BIG_CAR);//请求大车433发送数据
		delay_ms(100);
		err=laser.last_dis5 - laser.dis5;		
	}
	
	if (laser.dis5<2000)//消除误检
	{
			laser.dis5 = 7000;
	}	
	
	err = laser.dis5 - BIG_CLAW_UP_STOP;
	
	if(abs(err)>200)//绝对值大于200
	{
		Big_Claw_Up_Delay_Pool_Flag=1;
		PAW_UP(ON);//爪子开始上升
	}
	
	while(abs(err)>200)
	{
		if (laser.dis5<2000)//消除误检
		{
			laser.dis5 = 7000;
		}
		err = laser.dis5 - BIG_CLAW_UP_STOP;		
		
		if (abs(err)<200)//滤除误检
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

		if (laser.dis5<5900)//判断限位使爪子停住
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
		if (Big_Claw_Up_Delay_Pool>10)//时间到了也会退出
		{
				Big_Claw_Up_Delay_Pool=0;
				Big_Claw_Up_Delay_Pool_Flag=0;
				break;
		}
	}
	PAW_UP(OFF);//爪子停止上升
	
	while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//等待mpu6050数值稳定
	{
					
	}	
	
	RequestStop(BIG_CAR);//请求大车433停止发送数据
	laser.dis5 = 0.0f;
	*/
}
//从五楼平台抬升爪子
void RisePawFromPlatform(void)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag);//请求数据
		laser.last_dis1 = laser.dis1; //先给laser.last_dis1初始化一个值

	}
	else if(1==DataCorrect)//数据正常
	{
		if(UP_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  UpPawFromPlatform(PLATFORM_UZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}
}
//从垃圾池抬升爪子
void RisePawFromLitterPool(void)
{
	
	/*
	float err = 0.0;
	uint8_t count=0;
	uint8_t stop_count=0;	
	
	laser.last_dis5 = laser.dis5;
	RequestStart(BIG_CAR);//请求大车433发送数据
	err=laser.last_dis5 - laser.dis5;	

	while(err == 0.0f)
	{
		laser.last_dis5 = laser.dis5;
		RequestStart(BIG_CAR);//请求大车433发送数据
		delay_ms(100);
		err=laser.last_dis5 - laser.dis5;		
	}
	
	if (laser.dis5<2000)//消除误检
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
		
		if (abs(err<200))//滤除误差
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
		if (((laser.dis5<5900)&&(laser.dis5>5500)))//判断限位使爪子停住
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
		if (Big_Claw_Up_Delay>70)//延时
		{
				Big_Claw_Up_Delay = 0;
				Big_Claw_Up_Delay_Flag = 0;
				break;
		}		
	}
	PAW_UP(OFF);
	//爪子上升结束
	RequestStop(BIG_CAR);//请求大车433停止发送数据
	laser.dis5 = 0.0f;
	*/
}
//下放爪子至焚料池
void LowerClawtoBurnPool(void)
{
	if((0!=HTaskModeFlag)&&(0==DataCorrect))//数据不正常
	{
		DataCommunicateManage(HTaskModeFlag);//请求数据
		laser.last_dis8 = laser.dis8; //先给laser.last_dis8初始化一个值

	}
	else if(1==DataCorrect)//数据正常
	{
		if(DOWN_BIT == 0)
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
		  DownPawToLitterPool(BURN_POOL_DZ);		
		}
		else if(DOWN_BIT == 1)
		{
			RequestStop(BIG_CLAW);//请求大爪433停止发送数据
		}
	}	
/*
	float err2 = 0.0,err3 = 0.0,err4 = 0.0;
	uint8_t count = 0;	
	
	RequestStart(BURN_POOL);//给焚烧池料坑上433发送启动发送数据指令

	while ((err2==0)&&(err3==0)&&(err4==0))//判断数据是否正常
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
			//爪子正在下降
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
	//爪子下降结束
	RequestStop(BURN_POOL);
*/
}
//单独X方向运动
void XMoving(float x)
{
	float err_x = 0;
	err_x = laser.dis6-x;
	
	if(abs(err_x)>2000)//偏差大于一定范围时，要移动	
	{
		err_x = laser.dis6 - x;    //24米
		//Now is moving to the X destination
		if(err_x<0)//大行车向南移动
		{
			//大行车开始向南移动
			CAR_SOUTH(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);  		
		}
		else if(err_x>0)//大行车开始向北运动
		{
			//大行车开始向北运动
			CAR_NORTH(ON);		
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);	
		}		
	}	
	//大行车X方向开始点动
	else if(abs(err_x)<2000)//大行车X方向点动模式
	{		
		if(err_x<0)//大行车向南点动
		{
			//大行车反向停止
			if((err_x<(-200))&&(ReverseCount<2))
			{
				RevStop(err_x);			
			}
			else if((err_x<(-200))&&(ReverseCount>=2))//大行车向南点动
			{
				HorizontalDotMove(err_x,0.0f);
			}
		  else/*X方向移动结束*/
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
		else if(err_x>=0)//大行车向北点动
		{
			//大行车反向制动			
			if((err_x>200)&&(ReverseCount<2))
			{
				RevStop(err_x);
			}			
			else if((err_x>200)&&((ReverseCount)>=2))//大行车向北点动
			{
				HorizontalDotMove(err_x,0.0f);
			}
			else/*X方向移动结束*/
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
//单独Y方向运动
void YMoving(float y)
{
	float err_y = 0;
	err_y = laser.dis7-y;
	
	//大行车Y方向移动
	if(abs(err_y)>2000)//大行车Y方向移动
	{	
		//Now is moving to the Y destnation
		if ((err_y>0))//大行车向东运动
		{
			CAR_EAST(ON); 
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}
		else if ((err_y<0))//大行车向西运动
		{
			CAR_WEST(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}	
	}
	//大行车Y方向点动
	else if ((abs(err_y)<2000))//大行车Y方向点动
	{	
		if(err_y<0)//大行车向西点动
		{
			if(err_y<(-200))
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y方向移动结束
			{
				CAR_WEST(OFF);
				HTaskModeFlag = 0;
				WaitFlag = 0;
				
				PointMove = 0;
				PointMoveTime = 0;	
				Y_MOVE_BIT = 1;				
			}
		}
		else if(err_y>0)//大行车向东点动
		{
			if(err_y>200)
			{
				HorizontalDotMove(0,err_y);
			}
			else//Y方向移动结束
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
//爪子上升程序（焚料池上上升），用大爪往上射的激光
void UpPawFromBurnPool(float z)
{
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis8<0)//滤除偶尔出现的错误值
	{
		laser.dis8=laser.last_dis8;
	}				
	paw_err = laser.dis8 - z; 
	laser.last_dis8 = laser.dis8;   
	//大爪上升
	/*情况1：正常情况运行*/
	if(abs(paw_err)>300)//实际距离和期望距离偏差大于0.3米，大爪上升
 	{
		if(paw_err>=0)//大爪上升
		{
			PAW_UP(ON);	
		}
	}
	else if(abs(paw_err)<300)//实际距离和期望距离偏差小于0.3米，大爪停止上升
	{
		PAW_UP(OFF);	
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		UP_BIT = 1;//上升完成标志位置1
	}	
	/*情况2：爪子无法上升，但是abs(paw_err)>300*/
	if (abs(paw_err-paw_err_last)<100)//处理已经上升到限位的情况
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
//爪子上升程序（从五楼平台上升）
void UpPawFromPlatform(float z)
{
	float paw_err=0; 
	float paw_nowDis=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis1<0)//滤除偶尔出现的错误值
	{
		laser.dis1=laser.last_dis1;
	}				
	paw_nowDis=laser.dis1;     
	paw_err = paw_nowDis - z;    
	laser.last_dis1 = laser.dis1;   
		
	paw_err = laser.dis1 - z;	
	//大爪上升
	/*情况1：正常情况运行*/
	if(abs(paw_err)>300)//实际距离和期望距离偏差大于0.3米，大爪上升
 	{
		if(paw_err<=0)//大爪上升
		{
			PAW_UP(ON);	
		}
	}
	else if(abs(paw_err)<300)//实际距离和期望距离偏差小于0.3米，大爪停止上升
	{
		PAW_UP(OFF);	
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		UP_BIT = 1;//上升完成标志位置1
	}	
	/*情况2：爪子无法上升，但是abs(paw_err)>300*/
	if (abs(paw_err-paw_err_last)<100)//处理已经上升到限位的情况
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
//爪子上升程序（从料坑池上升）
void UpPawFromLitterPool(float z)
{

}
//爪子下降程序（下降到焚料池），用大爪往上射的激光
void DownPawToBurnPool(float z)
{
	float paw_err=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis8<0)//滤除偶尔出现的错误值
	{
		laser.dis8=laser.last_dis8;
	}				
	paw_err = laser.dis8 - z; 
	laser.last_dis8 = laser.dis8;   

	//大爪下降
	/*情况1：正常情况运行*/
	if(abs(paw_err)>300)//实际距离和期望距离偏差大于0.3米，大爪下降
	{
		if(paw_err<=0)//大爪下降
		{
			PAW_DOWN(ON);	
		}
	}
	else if(abs(paw_err)<300)//实际距离和期望距离偏差小于0.3米，大爪停止下降
	{
		PAW_DOWN(OFF);	
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		DOWN_BIT = 1;//下降完成标志位置1
	}
	/*情况2：在点动时爪子倾斜过大*/
	if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//爪子倾斜超过一定角度，停止下降，跳出循环
	{
		PAW_DOWN(OFF);
		HTaskModeFlag = 0;
		WaitFlag = 0;
		
		DOWN_BIT = 1;
	} 		
	/*情况3：爪子无法下降，但是绳索仍然下降*/
	if (abs(paw_err-paw_err_last)<100)//处理已经下降到底部，但还在下降的情况
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
//爪子下降程序（下降到料坑抓料）
void DownPawToLitterPool(float z)
{
//	还是用24米的吧
	/*
		大爪子下降抓料时下降的过程中要用到上下两个激光传感器，
		在上半部分时，用往上打的激光传感器
		在下半部分时，用往下打的激光传感器
		但是中间有一段盲区
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
//爪子下降程序（下降到5楼平台）
void DownPawToPlatform(float z)
{
	float paw_err=0; 
	float paw_nowDis=0; 
	float paw_err_last=0;
	static uint8_t same_dis_count=0;
	
	if (laser.dis1<0)//滤除偶尔出现的错误值
	{
		laser.dis1=laser.last_dis1;
	}				
	paw_nowDis=laser.dis1;     
	paw_err = paw_nowDis - z;    
	laser.last_dis1 = laser.dis1;   
		
	paw_err = laser.dis1 - z;

	//大爪下降
	if(abs(paw_err)>1000)//大爪下降
	{
		if(paw_err>=0)//大爪下降
		{
			PAW_DOWN(ON);	
		}
	}
	//大爪下降到指定高度，准备点动
	else if(abs(paw_err)<1000)//大爪下降到指定高度，准备点动
 	{
		/*情况1：正常情况运行*/
		if(paw_err>200)//开始点动
		{
			VerticalDotMove(paw_err);
		}
		else//点动结束
		{
			PAW_DOWN(OFF);			
			HTaskModeFlag = 0;
			WaitFlag = 0;
			
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		}
		/*情况2：在点动时爪子倾斜过大*/
		if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//爪子倾斜超过一定角度，停止下降，跳出循环
		{
			PAW_DOWN(OFF);
			HTaskModeFlag = 0;
			WaitFlag = 0;
			
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		} 		
		/*情况3：爪子无法下降，但是绳索仍然下降*/
		if (abs(paw_err-paw_err_last)<100)//处理已经下降到底部，但还在下降的情况
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
//大行车自检状态
void SelfCheckStatus(void)
{
	uint8_t count1=0;
	uint8_t count2=0;
	
  RelayOn();//打开遥控器
	RequestStart(BIG_CAR);//请求大车433发送数据
	
	while ((laser.dis6<=0)||(laser.dis7<=0))//判断大行车数据是否正常
	{
		RequestStart(BIG_CAR); //请求大车433发送数据	
		
		count1++;
		if(count1>10)//请求次数大于10次还没有得到正确数据，可能是数据传输链路有问题，需要检查
		{
			break;
		}
	}
	if(count1<=10)//数据正常
	{
		/*
			数据正常，上传数据，此段函数需要补充
		*/
		
		//判断行车位置
		if((abs(laser.dis6-ORIGIN_X)<200)&&(abs(laser.dis7-ORIGIN_Y)<200))
		{
			//(x,y)坐标在初始位置，继续判断z轴数据是否在初始位置附近
			RequestStop(BIG_CAR); //请求大车433停止发送数据	
			RequestStart(BIG_CLAW); //请求大爪433发送数据	
			
			while (laser.dis1<=0)//判断大爪上传数据是否正常
			{
				count2++;
				if(count2>10)//请求次数大于10次还没有得到正确数据，可能是数据传输链路有问题，需要检查
				{
					break;
				}
				for (uint8_t i = 0; i < 5; i++)
				{
					RequestStop(BIG_CAR); //请求大车433停止发送数据	
					RequestStart(BIG_CLAW); //请求大抓433发送数据	
				}			 
			}		
			if(abs(laser.dis1-ORIGIN_Z)<200)
			{
				/*不需要动*/
				RequestStop(BIG_CLAW); //请求大爪433停止发送数据	
			}
			else
			{
				VerticalMoveOrigin(ORIGIN_Z);//移动到初始位置
			}
		}
		else
		{
			//继续判断大爪子在在垃圾池上方还是平台上方
			if(laser.dis6<ADD_X)//在平台上方
			{		
				RisePawFromBurnPool();
				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
				VerticalMoveOrigin(ORIGIN_Z);
			}
			else//在垃圾池上方
			{
				RisePawFromLitterPool();
				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
				VerticalMoveOrigin(ORIGIN_Z);				
			}			
		}
	}
	ResetFlagBit();
	RelayOff();//关闭遥控器 
}
//数据通讯管理
void DataCommunicateManage(uint8_t task_mode)
{
	if((2==task_mode)||(3==task_mode)||(4==task_mode))//X,Y,上升
	{	
		if ((laser.dis6<=0)||(laser.dis7<=0)||(laser.dis5<=0))//判断大行车数据是否正常
		{
			DataCorrect = 0;
			RequestStop(BURN_POOL); //请求料池433停止发送数据	
			RequestStop(BIG_CLAW);  //请求大爪433停止发送数据
			RequestStart(BIG_CAR);  //请求大车433发送数据	
		}	
		else
			DataCorrect = 1;
	}
	else if(5==task_mode)
	{
		if (laser.dis1<=0 && laser.dis8<=0)//判断大爪子数据是否正常
		{
			DataCorrect = 0;
			RequestStop(BURN_POOL);  //请求料池433停止发送数据	
			RequestStop(BIG_CAR);    //请求大车433停止发送数据
			RequestStart(BIG_CLAW);  //请求大爪433发送数据	
		}	
		else
			DataCorrect = 1;	
	}
}
//半自动单独X方向移动
void HXMoxing(float x)
{
	if((0!=HTaskModeFlag)||(0==DataCorrect))
	{
		DataCommunicateManage(HTaskModeFlag);
	}
	else if(1==DataCorrect)//数据正常
	{
		
		XMoving(x);
	}
}
//反向停止
void RevStop(float err)
{
	if(err>0)
	{
		//大行车向南反向制动	
		if(ReverseStop == 0)
		{
			CAR_NORTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s开
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
			if(ReverseTime<=2)//0.2s停
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
	//大行车向北反向制动
	else
	{
		//大行车反向停止
		if(ReverseStop == 0)
		{
			CAR_SOUTH(OFF);
			ReverseStop=1;	       				
		}
		else if(ReverseStop==1)
		{
			if(ReverseTime<=10)//1s开
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
			if(ReverseTime<=2)//0.2s停
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
//大行车点动
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

//大爪子点动
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
}//复位标志位
void ResetFlagBit(void)
{
	 X_MOVE_BIT=0;
	 Y_MOVE_BIT=0;
	 DOWN_BIT=0;
	 UP_BIT=0;	
}
