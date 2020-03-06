#include "task.h"
#include "Data_type.h"
#include "usart1.h"
#include "relay.h"

 /*
 *************************************************************************
 *                       全局变量
 *************************************************************************  
 */
uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;
 
int64_t Big_Claw_Up_Delay = 0;//延时时间 S
uint8_t Big_Claw_Up_Delay_Flag = 0;//打开定时器标志位

int64_t Big_Claw_Up_Delay_Pool = 0;
uint8_t Big_Claw_Up_Delay_Pool_Flag = 0;



void BigCarRunning()
{
	/*big car starts running!*/
	RelayOn();//打开遥控器
	RisePawFromBurnPool();
	
	RelayOff();//关闭遥控器
}
//水平移动
void HorizontalMoving(float x,float y)
{
	float   err_x=0;     // 默认为0，出错不会移动
	float   err_y=0;
	uint8_t count=0;
	
	RequestStop(BIG_CLAW); //请求大爪433停止发送数据	
	RequestStart(BIG_CAR); //请求大车433发送数据	
	
	while ((laser.dis6<=0)||(laser.dis7<=0))//判断大行车数据是否正常
	{
		for (uint8_t i = 0; i < 3; i++)
		{
				RequestStop(BIG_CLAW); //请求大爪433停止发送数据
				delay_ms(200);
				RequestStart(BIG_CAR); //请求大车433发送数据	
				delay_ms(200);
		}
	}
	
	err_x = laser.dis6-x;
	err_y = laser.dis7-y;
	
	while(abs(err_x)>2000)//偏差大于一定范围时，要移动
	{
		err_x = laser.dis6 - x;    //24米
		err_y = laser.dis7 - y;    //12米	
		//Now is moving to the X destination
		if( (err_x<0) && (0==X_MOVE_BIT) )//大行车向南移动
		{
			//大行车开始向南移动
			CAR_SOUTH(ON);
			while(err_x<(-2000))
			{
				//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
				delay_ms(100);    
				err_x = laser.dis6 - x;    //24米
			}
			CAR_SOUTH(OFF);
			//大行车反向停止
			for(uint8_t i=0;i<2;i++)
			{
				CAR_NORTH(ON);
				for (uint8_t i = 0; i < 10; i++)
				{
						//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
						delay_ms(100);
						err_x = laser.dis6 - x;    //24米 
				}				
				CAR_NORTH(OFF);
				delay_ms(1000);
			}
			X_MOVE_BIT = 1;
			//大行车开始向南点动
		}
		else if( (err_x>0) && (0==X_MOVE_BIT) )
		{
			//大行车开始向北运动
			 
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
//竖直移动
void VerticalMoving(float z)
{

}
//数值降落到初始位置
void VerticalMoveOrigin(float z)
{

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
//从焚烧池或四楼平台抬升爪子
void RisePawFromBurnPool(void)
{
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
		CAR_UP(ON);//爪子开始上升
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
	CAR_UP(OFF);//爪子停止上升
	
	while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//等待mpu6050数值稳定
	{
					
	}	
	
	RequestStop(BIG_CAR);//请求大车433停止发送数据
	laser.dis5 = 0.0f;
}
//从垃圾池抬升爪子
void RisePawFromLitterPool(void)
{

}


