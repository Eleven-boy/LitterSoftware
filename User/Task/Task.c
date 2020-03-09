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

void BigCarRunning(void)
{
	/*big car starts running!*/
	RelayOn();//打开遥控器
	
	RisePawFromBurnPool();//开始提升爪子
	Target.x[0] = (GARBAGE_X-Target.x[0]-BIG_CAR_X_OFFSET)+ADD_X;
	HorizontalMoving(Target.x[0],Target.y[0]);//水平移动至垃圾池
	VerticalMoving(Target.z[0]); 
	ClosePaw();
	ResetFlagBit(); 
	
	RisePawFromLitterPool();//向上提升爪子
	HorizontalMoving(BURN_POOL_X,BURN_POOL_Y);//将爪子移至焚烧池正上方
	LowerClawtoPool();//下降爪子至焚烧池
	ReleasePaw(); 
	ResetFlagBit(); 
	
	RisePawFromBurnPool();//从焚烧池向上提爪子
	HorizontalMoving(Origin.x[0],Origin.y[0]);//水平移动之初始位置
	VerticalMoveOrigin(Origin.z[0]);//下降至四楼平台
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
				RequestStart(BIG_CAR); //请求大车433发送数据	
		}
	}
	
	err_x = laser.dis6-x;
	err_y = laser.dis7-y;
	
	//大行车X方向移动
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
		else if( (err_x>0) && (0==X_MOVE_BIT) )//大行车开始向北运动
		{
			//大行车开始向北运动
			CAR_NORTH(ON);
			while(err_x>2000)
			{
					//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
					delay_ms(100); 
					err_x = laser.dis6 - x;    //24米             
			}  			
			CAR_NORTH(OFF);
			//大行车反向制动
			for (uint8_t i = 0; i < 2; i++)//大行车反向制动
			{
					CAR_SOUTH(ON);
					for (uint8_t j = 0; j < 10; j++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_x = laser.dis6 - x;    //24米
					}
					CAR_SOUTH(OFF);
					delay_ms(1000);
			}	
			X_MOVE_BIT = 1;
		}
		delay_ms(100);
	}
	//大行车X方向开始点动
	if((abs(err_x)<2000)||(1==X_MOVE_BIT))//大行车X方向点动模式
	{
		CAR_SOUTH(OFF);
		CAR_NORTH(OFF);
		if(err_x<0)//大行车向南点动
		{
			while(err_x<(-200))
			{
					CAR_SOUTH(ON);
					for (uint8_t i = 0; i < 8; i++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_x = laser.dis6 - x;    //24米
					}
					CAR_SOUTH(OFF);
					delay_ms(1000); 
			}
			CAR_SOUTH(OFF);			
		}
		else if(err_x>=0)//大行车向北点动
		{
			while(err_x>200)
			{
					CAR_NORTH(ON);
					for (uint8_t i = 0; i < 8; i++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_x = laser.dis6 - x;    //24米  
					}
					CAR_NORTH(OFF);
					delay_ms(500);
			}
			CAR_NORTH(OFF);			
		}
		X_MOVE_BIT = 2; 
	}
		
	//大行车Y方向移动
	while(abs(err_y)>2000)//大行车Y方向移动
	{
		err_x = laser.dis6 - x;    //24米
		err_y = laser.dis7 - y;    //12米
		
		//Now is moving to the Y destnation
		if ((err_y>0)&&(0==Y_MOVE_BIT)&&(2==X_MOVE_BIT))//大行车向东运动
		{
			CAR_EAST(ON); 
			while(abs(err_y)>2000)
			{
				//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
				delay_ms(100); 
				err_y = laser.dis7 - y;    //12米
			}
			Y_MOVE_BIT = 1;
			CAR_EAST(OFF); 
		}
		else if ((err_y<0) && (0==Y_MOVE_BIT)&&(2==X_MOVE_BIT))//大行车向西运动
		{
			CAR_WEST(ON);
			while(abs(err_y)>2000)
			{
				//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
				delay_ms(100); 
				err_y = laser.dis7 - y;    //12米		
			}
			Y_MOVE_BIT = 1;
			CAR_WEST(OFF);
		}	
		delay_ms(100);
	}
	//大行车Y方向点动
	if ((abs(err_y)<2000)||(1==Y_MOVE_BIT))//大行车Y方向点动
	{
		CAR_WEST(OFF);
		CAR_EAST(OFF);
		while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//等待mpu6050数据稳定
		{
			 
		}   		
		if(err_y<0)//大行车向西点动
		{
			while(err_y<(-200))
			{
					CAR_WEST(ON);
					for (uint8_t i = 0; i < 8; i++)
					{
							//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
							delay_ms(100); 
							err_y = laser.dis7 - y;    //12米
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
							err_y = laser.dis7 - y;    //12米
					}
					CAR_EAST(OFF);
					delay_ms(500);
			}
			CAR_EAST(OFF);		
		}
		Y_MOVE_BIT = 2; 
		RequestStop(BIG_CAR);//请求大行车433停止数据发送
	}		
}
//竖直移动
void VerticalMoving(float z)
{
	float paw_err=0; 
	float paw_nowDis = 0;
	float paw_err_last=0;
	uint8_t count = 0;
	uint8_t same_dis_count=0;
	
	RequestStop(BIG_CAR); //请求大车433停止发送数据	
	RequestStop(BURN_POOL); //请求料坑433停止发送数据	
	RequestStart(BIG_CLAW); //请求大抓433发送数据		
	
	while (laser.dis1<=0)//判断大爪上传数据是否正常
	{
		PAW_DOWN(ON);
		count++;
		if (count>5)
		{   
				for (uint8_t i = 0; i < 5; i++)
				{
					RequestStop(BIG_CAR); //请求大车433停止发送数据	
					RequestStop(BURN_POOL); //请求料坑433停止发送数据	
					RequestStart(BIG_CLAW); //请求大抓433发送数据	
				}
				count=0;
		}		
		PAW_DOWN(OFF);
		delay_ms(10);
	}
	paw_err = laser.dis1 - z;
	//大爪下降
	while(abs(paw_err)>1000)//大爪下降
	{
		if((paw_err>=0)&&(DOWN_BIT==0))//大爪下降
		{
			PAW_DOWN(ON);
			while(paw_err>1000)
			{                 
				//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
				delay_ms(100);   
				if (laser.dis1<0)//滤除偶尔出现的错误值
				{
					laser.dis1=laser.last_dis1;
				}				
				paw_nowDis=laser.dis1;     
				paw_err = paw_nowDis - z;    
				laser.last_dis1 = laser.dis1;   
			}			
			PAW_DOWN(OFF);
			DOWN_BIT=1;  //下降标志位
		}
		paw_err = laser.dis1 - z;
		delay_ms(100);
	}
	//大爪下降到指定高度，准备点动
	if((abs(paw_err)<1000)&&(1==DOWN_BIT))//大爪下降到指定高度，准备点动
 	{
		while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//等待mpu6050数据稳定
		{
			//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
			delay_ms(100);
		}
		PAW_DOWN(ON);
		while(paw_err>200)//开始点动
		{
			PAW_DOWN(ON);
			for (uint8_t i = 0; i < 10; i++)//点动程序
			{
				//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
				delay_ms(100);  
				paw_nowDis=laser.dis1;        
				paw_err = paw_nowDis - z;      
			}
			PAW_DOWN(OFF);		
			delay_ms(500); 
			if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//爪子倾斜超过一定角度，停止下降，跳出循环
			{
					PAW_DOWN(OFF);
					break;
			} 
			
			if (abs(paw_err-paw_err_last)<100)//处理已经下降到底部，但还在下降的情况
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
		//大爪下降结束
		RequestStop(BIG_CLAW);//请求大爪433停止发送数据
	}
}
//竖直降落到初始位置
void VerticalMoveOrigin(float z)
{
	float paw_err=0; 
	float paw_nowDis = 0;
	float paw_err_last=0;
	uint8_t count = 0;
	uint8_t same_dis_count=0;	
	
	RequestStop(BIG_CAR); //请求大车433停止发送数据	
	RequestStop(BURN_POOL); //请求料坑433停止发送数据	
	RequestStart(BIG_CLAW); //请求大抓433发送数据		
	
	while (laser.dis1<=0)//判断大爪上传数据是否正常
	{
		count++;
		if (count>5)
		{   
				for (uint8_t i = 0; i < 5; i++)
				{
					RequestStop(BIG_CAR); //请求大车433停止发送数据	
					RequestStop(BURN_POOL); //请求料坑433停止发送数据	
					RequestStart(BIG_CLAW); //请求大抓433发送数据	
				}
				count=0;
		}		
		delay_ms(100);
	}
	paw_err = laser.dis1 - z;
	//printf("down:acc_z=%f,gyro_z=%f,angle_x=%f,angle_y=%f,angle_z=%f,dis=%f\r\n",mpu.acc_z,mpu.gyro_z,mpu.angle_x,mpu.angle_y,mpu.angle_z,laser.dis1);
	
	while(paw_err>800)//判断是否要下降
	{
		if((paw_err>=0)&&(DOWN_BIT==0))//满足条件，爪子向下移动
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
	//爪子准备点动下降
	if((abs(paw_err)<800)&&(1==DOWN_BIT))
	{
		while ((abs(mpu.angle_x)>10)||(abs(mpu.angle_y)>10))//等待mpu6050数据稳定
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
			
			if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>11.0f))//如果下降过程中爪子倾斜一定程度，结束下降
			{
				PAW_DOWN(OFF);
				break;
			}   
			
			if (abs(paw_err-paw_err_last)<20)//滤除误差
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
		//爪子下降结束
		RequestStop(BIG_CLAW);//请求大爪433停止发送数据
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
}
//从垃圾池抬升爪子
void RisePawFromLitterPool(void)
{
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
}
//下放爪子至料坑
void LowerClawtoPool(void)
{
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
}
//复位标志位
void ResetFlagBit(void)
{
	 X_MOVE_BIT=0;
	 Y_MOVE_BIT=0;
	 DOWN_BIT=0;
	 UP_BIT=0;	
}


