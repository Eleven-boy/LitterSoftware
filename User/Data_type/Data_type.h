#ifndef __DATA_TYPE_H
#define	__DATA_TYPE_H

#include "stm32f4xx.h"

/*******************************************************
 *                     宏定义
 *******************************************************/
//限幅
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )

/*******************************************************
 *                     结构体定义
 *******************************************************/
 //单点激光
typedef struct{
	float dis1;//大爪子往下射的激光
	float dis2;
	float dis3;
	float dis4;
	float dis5;
	float dis6;
	float dis7;
	float dis8;//大爪子往上射的激光
	
	float last_dis1;
	float last_dis2;
	float last_dis3;
	float last_dis4;
	float last_dis5;
	float last_dis6;
	float last_dis7;
	float last_dis8;
		
	float sampleval1;
	float sampleval2;
	float sampleval3;
	float sampleval4;
	float sampleval5;
	float sampleval6;
	float sampleval7;
	float sampleval8;
}Laser;
extern Laser laser;

//mpu6050
typedef struct{
	int dis;
	float acc_z;
	float gyro_z;
	float angle_x;
	float angle_y;
	float angle_z;
}MPU6050;
extern MPU6050 mpu;


//位置坐标
typedef struct{
    int x[2];
    int y[2];
    int z[2];
	  int uwbdis[2];
}POSITION; 
extern POSITION origin;//起始位置
extern POSITION target;//目标位置 

//主控给上位机发送的数据内容
typedef struct
{
	uint8_t       Add;     //地址码
	int           P_x;     //x坐标
	int           P_y;     //y坐标
	int           P_z;	   //z坐标
	int16_t       A_x;     //6050x轴角度
	int16_t       A_y;     //6050y轴角度
	uint8_t       Status;  //行车运行状态
	uint8_t       HalfStep;//半自动状态下需要请求的下步动作
}UP_DATA;
extern UP_DATA Up_Data;
typedef struct
{																						  
	u16 time_1ms;                             //
	u16 time_2ms;                             //
	u16	time_5ms;                             //
	u16 time_10ms; 														//
	u16	time_25ms;	                          //
	u16 time_100ms;                           //
	u16 time_200ms;
	u16 time_1000ms;                           //
} Task_loop_t;
extern Task_loop_t task_tim;

typedef struct{
	int count;//请求次数
	int delayms;//间隔时间
}UART_REQUEST_433; 
//设备号
enum Dev_Obj
{
	BIG_CAR = 0,
	BIG_CLAW,
	BURN_POOL,
	ALL_DEV
};



extern unsigned char send_request_to_xxx[3][6];
#endif
