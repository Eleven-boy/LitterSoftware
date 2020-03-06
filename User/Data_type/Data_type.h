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
	float dis1;
	float dis2;
	float dis3;
	float dis4;
	float dis5;
	float dis6;
	float dis7;
	
	float last_dis1;
	float last_dis2;
	float last_dis3;
	float last_dis4;
	float last_dis5;
	float last_dis6;
	float last_dis7;
		
	float sampleval1;
	float sampleval2;
	float sampleval3;
	float sampleval4;
	float sampleval5;
	float sampleval6;
	float sampleval7;
}Laser;
extern Laser laser;

//mpu6050
typedef struct{
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
}POSITION;
extern POSITION Origin;//起始位置
extern POSITION Target;//目标位置
extern POSITION BURN_POOL_Target;//料坑目标位置

typedef struct
{																						  
	u16 time_1ms;                             //
	u16 time_2ms;                             //
	u16	time_5ms;                             //
	u16 time_10ms; 														//
	u16	time_25ms;	                          //
	u16 time_100ms;                           //
	u16 time_1000ms;                           //
} Task_loop_t;
extern Task_loop_t task_tim;

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
