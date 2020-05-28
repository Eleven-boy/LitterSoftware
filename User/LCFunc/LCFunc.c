 /*
 *************************************************************************
 * LCFunc.c  
 * 主要关于行车最底层的每一个环节的直接控制，如果要进行
 * 组合控制，只需要将这里的函数进行组合
 *************************************************************************  
 */

#include "LCFunc.h"
#include "Data_type.h"
#include "usart1.h"
#include "usart2.h"
#include "relay.h"
#include "bsp_led.h"

 /*
 *************************************************************************
 *                       全局变量
 *************************************************************************  
 */
extern uint8_t WaitFlag;
extern uint8_t Run_Mode;

 //0:没有运行，1：正常运行结束，2：数据出的故障不得已结束
uint8_t X_MOVE_BIT=0;
uint8_t Y_MOVE_BIT=0;
uint8_t DOWN_BIT=0;
uint8_t UP_BIT=0;

//0:数据不正常，1:数据正常
uint8_t BigCarDataCorrect = 0; 
uint8_t BigClawDataCorrect = 0;

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

//大爪子在料坑上下时用到
uint8_t UpOrDown = 0;//0代表上半部，1代表下半部

//1：表示合拢
uint8_t CloseFlag = 0;
//合拢延时
int     CloseDelay = 0;
//1:表示打开
uint8_t OpenFlag = 0;
//打开延时
int     OpenDelay = 0;
//出错状态
uint8_t ErrorBigCar = 0;

int64_t Big_Claw_Up_Delay = 0;//延时时间 S
uint8_t Big_Claw_Up_Delay_Flag = 0;//打开定时器标志位

int64_t Big_Claw_Up_Delay_Pool = 0;
uint8_t Big_Claw_Up_Delay_Pool_Flag = 0;

u8 CommunicatDelay = 0;//通讯延迟

Provar BCVar;
/*
*****************************************************************************************************************
*                                     void ClosePaw(void) 
*
*Description : 合紧爪子
*Arguments   : none
*Returns     : none
*Notes       : 使用此函数前CloseFlag置1，此功能结束后会将CloseFlag和CloseDelay置零
*****************************************************************************************************************
*/
void ClosePaw(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x30;//此时的状态值 close
	if(1 == CloseFlag)
	{
		if(CloseDelay<100)
		{
			PAW_CLOSE(ON);
		}
		else
		{
			PAW_CLOSE(OFF);	
			CloseFlag = 2;
			CloseDelay = 0;
		}
	}
}
/*
*****************************************************************************************************************
*                                     void ReleasePaw(void)
*
*Description : 松开爪子
*Arguments   : none
*Returns     : none
*Notes       : 使用此函数前OpenFlag置1，此功能结束后会将OpenFlag和OpenDelay置零
*****************************************************************************************************************
*/
void ReleasePaw(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x38;//此时的状态值  release
	if(1 == OpenFlag)
	{
		if(OpenDelay<100)
		{
			PAW_RELEASE(ON);
		}
		else
		{
			PAW_RELEASE(OFF);	
			OpenFlag = 2;
			OpenDelay = 0;
		}
	}
}
/*
*****************************************************************************************************************
*                                     void XMoving(float x)(待补充，目前只用了一路激光，要用两路)
*
*Description : X方向运动
*Arguments   : float x：目标位置x坐标
*Returns     : none
*Notes       : X方向移动使用激光laser.dis6
*****************************************************************************************************************
*/
void XMoving(float x)
{
	float err_x = 0;
//	static uint8_t same_dis_count=0;
	static uint8_t dis6_error_count=0;
	
	if (laser.dis6<0)//滤除偶尔出现的错误值
	{
		if(dis6_error_count<10)
		{
			dis6_error_count++;
			laser.dis6=laser.last_dis6;
		}
		else//一直出现负值，停止运动出问题了
		{
			CAR_NORTH(OFF);
			CAR_SOUTH(OFF);
			dis6_error_count=0;
			X_MOVE_BIT = 2;			
		}
	}		
	else
	{
		dis6_error_count=0;
	}

	err_x = laser.dis6-x;
	laser.last_dis6 = laser.dis6;
	
	if((abs(err_x)>2000)&&(laser.dis6>0))//偏差大于一定范围时，要移动	
	{
		
		err_x = laser.dis6 - x;    //24米
		//Now is moving to the X destination
		if(err_x<0)//大行车向南移动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x08;//此时的状态值 x-
			//大行车开始向南移动
			CAR_SOUTH(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);  		
		}
		else if(err_x>0)//大行车开始向北运动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x00;//此时的状态值 x+
			//大行车开始向北运动
			CAR_NORTH(ON);		
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);	
		}		
	}	
	//大行车X方向开始点动
	else if((abs(err_x)<2000)&&(laser.dis6>0))//大行车X方向点动模式
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
*Description : Y方向运动
*Arguments   : float y：目标位置y坐标
*Returns     : none
*Notes       : Y方向移动使用激光laser.dis7
*****************************************************************************************************************
*/
void YMoving(float y)
{
	float err_y = 0;
//	static uint8_t same_dis_count=0;
	static uint8_t dis7_error_count=0;
	
	if (laser.dis7<0)//滤除偶尔出现的错误值
	{
		if(dis7_error_count<10)
		{
			dis7_error_count++;
			laser.dis7=laser.last_dis7;
		}
		else//一直出现负值，停止运动出问题了
		{
			CAR_WEST(OFF);
			CAR_EAST(OFF);
			dis7_error_count=0;
			Y_MOVE_BIT = 2;			
		}
	}		
	else
	{
		dis7_error_count=0;
	}
	err_y = laser.dis7-y;
	laser.last_dis7 = laser.dis7;   	
	
	
	//大行车Y方向移动
	if((abs(err_y)>2000)&&(laser.dis7>0))//大行车Y方向移动
	{	
		//Now is moving to the Y destnation
		if ((err_y>0))//大行车向东运动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x10;//此时的状态值 y+
			CAR_EAST(ON); 
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}
		else if ((err_y<0))//大行车向西运动
		{
			Up_Data.Status = (Up_Data.Status&0x87)|0x18;//此时的状态值 y-
			CAR_WEST(ON);
			//printf("X_south:nowDis_x=%f,err_x=%f,nowDis_y=%f,err_y=%f\r\n",laser.dis6,err_x,laser.dis7,err_y);
		}	
	}
	//大行车Y方向点动
	else if ((abs(err_y)<2000)&&(laser.dis7>0))//大行车Y方向点动
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
*Description : 爪子上升程序（焚料池上上升），用大爪往上射的激光
*Arguments   : float z：激光距屋顶的高度
*Returns     : none
*Notes       : 此时用激光laser.dis8    情况2：需要现场微调
*****************************************************************************************************************
*/
void UpPawFromBurnPool(float z)
{
	float paw_err=0; 
	static float paw_err_last=0;
	static uint8_t same_dis_count=0;
	static uint8_t dis8_error_count=0;
	
	if (laser.dis8<0)//滤除偶尔出现的错误值
	{
		if(dis8_error_count<100)
		{
			dis8_error_count++;
			laser.dis8=laser.last_dis8;
		}
		else//一直出现负值，停止运动出问题了
		{
			PAW_UP(OFF);
			dis8_error_count=0;
			UP_BIT = 2;			
		}
	}				
	paw_err = laser.dis8 - z; 
	laser.last_dis8 = laser.dis8;   
	//大爪上升
	/*情况1：正常情况运行*/
	if((abs(paw_err)>300)&&(laser.dis8>0))//实际距离和期望距离偏差大于0.3米，大爪上升
 	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x20;//此时的状态值 up
		if(paw_err>=0)//大爪上升
		{
			PAW_UP(ON);	
			LED1_TOGGLE;
		}
	}
	else if((abs(paw_err)<300)&&(laser.dis8>0))//实际距离和期望距离偏差小于0.3米，大爪停止上升
	{
		PAW_UP(OFF);			
		UP_BIT = 1;//上升完成标志位置1
	}	
	/*情况2：爪子无法上升，但是abs(paw_err)>300*/
	if (abs(paw_err-paw_err_last)<50)//处理已经上升到限位的情况   此处的距离和累计次数需要微调
	{
		same_dis_count = same_dis_count+1;
		
		if (same_dis_count>100)
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
*Description : 爪子上升程序（从五楼平台上升）（用下面激光）
*Arguments   : float z：激光距四楼平台的高度
*Returns     : none
*Notes       : 此时用激光laser.dis1    情况2：需要现场微调
*****************************************************************************************************************
*/
float aaerr = 0;
void UpPawFromPlatform(float z)
{
	float paw_err=0; 
	static float paw_err_last=0;
	static uint8_t same_dis_count=0;
	static uint8_t dis1_error_count=0;
	
	if (laser.dis1<0)//滤除偶尔出现的错误值
	{
		if(dis1_error_count<10)
		{
			dis1_error_count++;
			laser.dis1=laser.last_dis1;
		}
		else//一直出现负值，停止运动出问题了
		{
			PAW_UP(OFF);
			dis1_error_count = 0;
			UP_BIT = 2;			
		}
	}				     
	paw_err = laser.dis1 - z;    
	laser.last_dis1 = laser.dis1;   
	//大爪上升
	/*情况1：正常情况运行*/
	if((abs(paw_err)>300)&&(laser.dis1>0))//实际距离和期望距离偏差大于0.3米，大爪上升
 	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x20;//此时的状态值 up
		if(paw_err<=0)//大爪上升
		{
			PAW_UP(ON);	
		}
	}
	else if((abs(paw_err)<300)&&(laser.dis1>0))//实际距离和期望距离偏差小于0.3米，大爪停止上升
	{
		PAW_UP(OFF);		
		UP_BIT = 1;//上升完成标志位置1
	}	
	/*情况2：爪子无法上升，但是abs(paw_err)>300*/
	if (abs(paw_err-paw_err_last)<50)//处理已经上升到限位的情况  此处的距离和累计次数需要微调
	{
		same_dis_count = same_dis_count+1;
		
		if (same_dis_count>100)
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
*Description : 爪子上升程序（从料坑池上升）
*Arguments   : float z：激光距屋顶的高度
*Returns     : none
*Notes       : 先用往下射的激光laser.dis1，再用往上射的激光laser.dis8
*****************************************************************************************************************
*/
void UpPawFromLitterPool(float z)
{
  //	dis8  还是用24米的吧
	/*
    大爪子抓料后上升过程中要用到上下两个激光传感器，
		在上半部分时，用往上打的激光传感器
		在下半部分时，用往下打的激光传感器
	*/
	float paw_err=0; 
	static float paw_err_last=0;
	static uint8_t same_dis_count=0;
	static uint8_t dis1_error_count=0;
	static uint8_t dis8_error_count=0;

	if(1==UpOrDown)//在下半部分，用往下射的激光 12米 dis1
	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x20;//此时的状态值 up
		
		if((laser.dis1>0 && laser.dis1<10000) && laser.dis8<0)
		{
			PAW_UP(ON);	
		}
		
		if (laser.dis1<0 && laser.dis8>0)//满足条件转到下一控制逻辑
		{
			same_dis_count = same_dis_count+1;			
			if (same_dis_count>100)
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
	else if(0==UpOrDown)//在上半部分，用往上射的激光，24米，dis8
	{
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
			UP_BIT = 1;//上升完成标志位置1
		}	
		/*情况2：爪子无法上升，但是abs(paw_err)>300*/
		if (abs(paw_err-paw_err_last)<50)//处理已经上升到限位的情况
		{
			same_dis_count = same_dis_count+1;
			
			if (same_dis_count>100)
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
*Description : 爪子下降程序（下降到焚料池），用大爪往上射的激光
*Arguments   : float z：激光距屋顶的高度
*Returns     : none
*Notes       : 此时用激光laser.dis8    情况3：此处需要微调
*****************************************************************************************************************
*/
void DownPawToBurnPool(float z)
{
	float paw_err=0; 
	static float paw_err_last=0;
	static uint8_t same_dis_count=0;
	static uint8_t dis8_error_count=0;

	
	if (laser.dis8<0)//滤除偶尔出现的错误值
	{
		if(dis8_error_count<10)
		{
			dis8_error_count++;
			laser.dis8=laser.last_dis8;
		}
		else//一直出现负值，停止运动出问题了
		{
			PAW_DOWN(OFF);
			dis8_error_count=0;
			UP_BIT = 2;			
		}
	}				
	paw_err = laser.dis8 - z; 
	laser.last_dis8 = laser.dis8;   

	//大爪下降
	/*情况1：正常情况运行*/
	if((abs(paw_err)>300)&&(laser.dis8>0))//实际距离和期望距离偏差大于0.3米，大爪下降
	{
		Up_Data.Status = (Up_Data.Status&0x87)|0x28;//此时的状态值 down
		if(paw_err<=0)//大爪下降
		{
			PAW_DOWN(ON);	
		}
	}
	else if((abs(paw_err)<300)&&(laser.dis8>0))//实际距离和期望距离偏差小于0.3米，大爪停止下降
	{
		PAW_DOWN(OFF);	
		
		DOWN_BIT = 1;//下降完成标志位置1
	}
	/*情况2：在点动时爪子倾斜过大*/
	if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//爪子倾斜超过一定角度，停止下降，跳出循环
	{
		PAW_DOWN(OFF);
		
		DOWN_BIT = 1;
	} 		
	/*情况3：爪子无法下降，但是绳索仍然下降*/
	if (abs(paw_err-paw_err_last)<50)//处理已经下降到底部，但还在下降的情况  此处的距离和累计次数需要微调
	{
		same_dis_count = same_dis_count+1;
		
		if (same_dis_count>100)
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
*Description : 爪子下降程序（下降到料坑抓料）（先用上面激光，再用下面激光）
*Arguments   : float z：激光距地面垃圾的高度，在上半部分数值为负值
*Returns     : none
*Notes       : 先用往上射的激光laser.dis8，再用往下射的激光laser.dis1
*****************************************************************************************************************
*/
void DownPawToLitterPool(float z)
{
  //	dis8  还是用24米的吧
	/*
		大爪子下降抓料时下降的过程中要用到上下两个激光传感器，
		在上半部分时，用往上打的激光传感器
		在下半部分时，用往下打的激光传感器
	*/
	float paw_err=0; 
	static float paw_err_last=0;
	static uint8_t same_dis_count=0;
	static uint8_t dis1_error_count=0;
	
	if(0==UpOrDown)//用往上射的激光  24米  dis8
	{		
		Up_Data.Status = (Up_Data.Status&0x87)|0x28;//此时的状态值 down
		if((laser.dis8>0 && laser.dis8<21000) && laser.dis1<0)
		{
			PAW_DOWN(ON);	
		}
		
		if (laser.dis8<0 && laser.dis1>0)
		{
			same_dis_count = same_dis_count+1;			
			if (same_dis_count>100)
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
	else if(1==UpOrDown)//用往下射的激光 12米 dis1
	{
		if (laser.dis1<0)//滤除偶尔出现的错误值
		{
			if(dis1_error_count<100)
			{
				dis1_error_count++;
				laser.dis1=laser.last_dis1;
			}
			else//一直出现负值，停止运动出问题了
			{
				PAW_DOWN(OFF);
				dis1_error_count=0;
				DOWN_BIT = 2;			
			}
		}				
		paw_err = laser.dis1 - z;   
		laser.last_dis1 = laser.dis1;   
			
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
				
				PointMove = 0;
				PointMoveTime = 0;	
				DOWN_BIT = 1;
			}
			/*情况2：在点动时爪子倾斜过大*/
			if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//爪子倾斜超过一定角度，停止下降，跳出循环
			{
				PAW_DOWN(OFF);
				
				PointMove = 0;
				PointMoveTime = 0;	
				DOWN_BIT = 1;
			} 		
			/*情况3：爪子无法下降，但是绳索仍然下降*/
			if (abs(paw_err-paw_err_last)<50)//处理已经下降到底部，但还在下降的情况
			{
				same_dis_count = same_dis_count+1;
				
				if (same_dis_count>100)
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
*Description : 爪子下降程序（下降到5楼平台）(用往下打的激光)
*Arguments   : float z：激光距四楼平台的高度
*Returns     : none
*Notes       : 此时用激光laser.dis1    情况3：需要微调
*****************************************************************************************************************
*/
void DownPawToPlatform(float z)
{
	float paw_err=0; 
	static float paw_err_last=0;
	static uint8_t same_dis_count=0;
	static uint8_t dis1_error_count=0;
	
	if (laser.dis1<0)//滤除偶尔出现的错误值
	{
		laser.dis1=laser.last_dis1;
		if(dis1_error_count<10)
		{
			dis1_error_count++;
			laser.dis1=laser.last_dis1;
		}
		else//一直出现负值，停止运动出问题了
		{
			PAW_UP(OFF);
			dis1_error_count=0;
			UP_BIT = 2;			
		}
	}
	paw_err = laser.dis1 - z;   
	laser.last_dis1 = laser.dis1;   
	
  Up_Data.Status = (Up_Data.Status&0x87)|0x28;//此时的状态值 down	
	//大爪下降
	if((abs(paw_err)>1000)&&(laser.dis1>0))//大爪下降
	{
		if(paw_err>=0)//大爪下降
		{
			PAW_DOWN(ON);	
		}
	}
	//大爪下降到指定高度，准备点动
	else if((abs(paw_err)<1000)&&(laser.dis1>0))//大爪下降到指定高度，准备点动
 	{
		/*情况1：正常情况运行*/
		if(paw_err>200)//开始点动
		{
			VerticalDotMove(paw_err);
		}
		else//点动结束
		{
			PAW_DOWN(OFF);			
			
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		}
		/*情况2：在点动时爪子倾斜过大*/
		if ((abs(mpu.angle_x)>10.0f)||(abs(mpu.angle_y)>10.0f))//爪子倾斜超过一定角度，停止下降，跳出循环
		{
			PAW_DOWN(OFF);		
			PointMove = 0;
			PointMoveTime = 0;	
			DOWN_BIT = 1;
		} 		
		/*情况3：爪子无法下降，但是绳索仍然下降*/
		if (abs(paw_err-paw_err_last)<50)//处理已经下降到底部，但还在下降的情况   此处的距离和累计次数需要微调
		{
			same_dis_count = same_dis_count+1;
			
			if (same_dis_count>100)
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
//大行车自检状态
void SelfCheckStatus(void)
{
//	uint8_t count1=0;
//	uint8_t count2=0;
//	
//  RelayOn();//打开遥控器
//	RequestStart(BIG_CAR);//请求大车433发送数据
//	
//	while ((laser.dis6<=0)||(laser.dis7<=0))//判断大行车数据是否正常
//	{
//		RequestStart(BIG_CAR); //请求大车433发送数据	
//		
//		count1++;
//		if(count1>10)//请求次数大于10次还没有得到正确数据，可能是数据传输链路有问题，需要检查
//		{
//			break;
//		}
//	}
//	if(count1<=10)//数据正常
//	{
//		/*
//			数据正常，上传数据，此段函数需要补充
//		*/
//		
//		//判断行车位置
//		if((abs(laser.dis6-ORIGIN_X)<200)&&(abs(laser.dis7-ORIGIN_Y)<200))
//		{
//			//(x,y)坐标在初始位置，继续判断z轴数据是否在初始位置附近
//			RequestStop(BIG_CAR); //请求大车433停止发送数据	
//			RequestStart(BIG_CLAW); //请求大爪433发送数据	
//			
//			while (laser.dis1<=0)//判断大爪上传数据是否正常
//			{
//				count2++;
//				if(count2>10)//请求次数大于10次还没有得到正确数据，可能是数据传输链路有问题，需要检查
//				{
//					break;
//				}
//				for (uint8_t i = 0; i < 5; i++)
//				{
//					RequestStop(BIG_CAR); //请求大车433停止发送数据	
//					RequestStart(BIG_CLAW); //请求大抓433发送数据	
//				}			 
//			}		
//			if(abs(laser.dis1-ORIGIN_Z)<200)
//			{
//				/*不需要动*/
//				RequestStop(BIG_CLAW); //请求大爪433停止发送数据	
//			}
//			else
//			{
//				VerticalMoveOrigin(ORIGIN_Z);//移动到初始位置
//			}
//		}
//		else
//		{
//			//继续判断大爪子在在垃圾池上方还是平台上方
//			if(laser.dis6<ADD_X)//在平台上方
//			{		
//				RisePawFromBurnPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);
//			}
//			else//在垃圾池上方
//			{
//				RisePawFromLitterPool();
//				HorizontalMoving(ORIGIN_X,ORIGIN_Y);
//				VerticalMoveOrigin(ORIGIN_Z);				
//			}			
//		}
//	}
//	ResetFlagBit();
//	RelayOff();//关闭遥控器 
//	
}
/*
*****************************************************************************************************************
*                                     DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff)
*
*Description : 数据通讯管理，用于打开某一路上传激光数据
*Arguments   : uint8_t task_mode：用于选择不同设备上的激光 OnorOff:1代表on,0代表off
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
//数据通讯管理
void DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff)
{
	float dis1_err = 0.0f;
	float dis5_err = 0.0f;
	float dis6_err = 0.0f;
	float dis7_err = 0.0f;
	float dis8_err = 0.0f;
	
	if(BIG_CAR==task_mode)//请求大行车433
	{	
		if(1==OnorOff)
		{
			dis5_err = laser.dis5 - laser.last_dis5;
			dis6_err = laser.dis6 - laser.last_dis6;
			dis7_err = laser.dis7 - laser.last_dis7; 
			
			if ((dis5_err==0)&&(dis6_err==0)&&(dis7_err==0))//判断大行车数据是否正常
			{
				BigCarDataCorrect = 0;
				RequestStart(BIG_CAR);  //请求大车433发送数据	
			}	
			else
				BigCarDataCorrect = 1;	
			
			laser.last_dis5 = laser.dis5;//保存历史值
			laser.last_dis6 = laser.dis6;
			laser.last_dis7 = laser.dis7;
		}
		else if(0==OnorOff)
		{
			RequestStop(BIG_CAR);
		}
	}
	else if(BIG_CLAW==task_mode)//请求大爪433
	{
		if(1==OnorOff)
		{
			dis1_err = laser.dis1 - laser.last_dis1;
			dis8_err = laser.dis8 - laser.last_dis8;
			
			if (dis1_err == 0.0f && dis8_err == 0.0f)//判断大爪子数据是否正常
			{
				BigClawDataCorrect = 0;
				RequestStart(BIG_CLAW);  //请求大爪433发送数据	
			}	
			else
				BigClawDataCorrect = 1;		
			
			laser.last_dis1 = laser.dis1;//保存历史值
			laser.last_dis8 = laser.dis8;
		}
		else if(0==OnorOff)
		{
			RequestStop(BIG_CLAW);
		}
	}
}
/*
*****************************************************************************************************************
*                                    void RevStop(float err)
*
*Description : 反向停止
*Arguments   : float err：实际位置距目标位置偏差
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
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
		else if(ReverseStop==2)
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
		else if(ReverseStop==2)
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
/*
*****************************************************************************************************************
*                                    void HorizontalDotMove(float err_x,float err_y)
*
*Description : 大行车点动
*Arguments   : float err_x,float err_y：传入位置偏差，不用的那一个轴传0
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
*Description : 大爪子点动
*Arguments   : float err_z：传入位置偏差
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
}//复位标志位
/*
*****************************************************************************************************************
*                                    void ResetFlagBit(void)
*
*Description : 复位标志位
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
/*
*****************************************************************************************************************
*                                    void RequestStop(uint8_t dev)
*
*Description : 请求大行车/大爪子停止发送指令
*Arguments   : dev:设备号
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RequestStop(uint8_t dev)
 {
	switch(dev)
	{
		case BIG_CAR:
					RequestStopToBigCar();
					break;
		case BIG_CLAW:
					RequestStopToBigClaw();
					break;		
		default:
				break;
	}
}
/*
*****************************************************************************************************************
*                                    void RequestStart(uint8_t dev)
*
*Description : 请求大行车/大爪子开始发送指令
*Arguments   : dev:设备号
*Returns     : none
*Notes       : none
*****************************************************************************************************************
*/
void RequestStart(uint8_t dev)
{
	switch(dev)
	{
		case BIG_CAR:
					RequestStartToBigCar();
					break;
		case BIG_CLAW:
				 	RequestStartToBigClaw();	
					break;		
		default:
				  break;
	}
}
//暂停
void TaskSuspend(void)
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x50;//此时的状态值 暂停
	
	BCVar._CloseFlag = CloseFlag;
	BCVar._OpenFlag = OpenFlag;
	BCVar._PointMove = PointMove;
	BCVar._ReverseStop = ReverseStop;
	BCVar._Run_Mode = Run_Mode;
	BCVar._WaitFlag = WaitFlag;
	
	PowerOff();//给遥控器断电
	
	CloseFlag = 0;
	OpenFlag = 0;
	PointMove = 0;
	ReverseStop = 0;
	Run_Mode = 0;
	WaitFlag = 0;
	//Up_Data.Status   //暂停
}
//恢复运行
void ConExecute(void)
{

	Up_Data.Status = (Up_Data.Status&0x87)|0x58;//此时的状态值 恢复
	CloseFlag = BCVar._CloseFlag;
	OpenFlag = BCVar._OpenFlag;
	PointMove = BCVar._PointMove;
	ReverseStop = BCVar._ReverseStop;
	Run_Mode = BCVar._Run_Mode;
	WaitFlag = BCVar._WaitFlag;
	
	PowerOn();//给遥控器上电

	BCVar._CloseFlag = 0;
	BCVar._OpenFlag = 0;
	BCVar._PointMove = 0;
	BCVar._ReverseStop = 0;
	BCVar._Run_Mode = 0;
	BCVar._WaitFlag = 0;

}

void BigCarStop(void)//还需斟酌
{
	Up_Data.Status = (Up_Data.Status&0x87)|0x48;//此时的状态值 down
	PowerOff();//给遥控器断电
	
	CloseFlag = 0;
	OpenFlag = 0;
	PointMove = 0;
	ReverseStop = 0;
	Run_Mode = 0;
	WaitFlag = 0;	
}
