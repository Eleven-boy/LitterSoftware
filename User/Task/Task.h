#ifndef __TASK_H
#define	__TASK_H

#include "stm32f4xx.h"




/*******************************************************
 *                     结构体定义
 *******************************************************/

 /*
 *************************************************************************
 *                        宏定义
 *************************************************************************  
 */
#define GARBAGE_X        16000       //垃圾池y轴长
#define GARBAGE_Y        500         //垃圾池x轴长
#define ADD_X            11400       //料坑台y轴长
#define BIG_CLAW_UP_STOP 5400        //爪子上升停止时打在爪子激光上的距离
#define BIG_CLAW_DOWN_STOP2 4500     //爪子下降到料坑上方停止时激光打在爪子上的距离
#define BIG_CLAW_DOWN_STOP3 3500     //爪子下降到料坑上方停止时激光打在爪子上的距离
#define BIG_CLAW_DOWN_STOP4 4500     //爪子下降到料坑上方停止时激光打在爪子上的距离
#define BIG_CLAW_BASE_DIS  1600      //爪子着地时激光距地面的距离
#define BIG_CAR_X_OFFSET  2000
#define BURN_POOL_X  4600            //大行车停留在料坑上方时x坐标
#define BURN_POOL_Y  4400            //大行车停留在料坑上方时y坐标


#define abs(x)  ((x<0)?(-x):x)       //求x的绝对值

extern int64_t Big_Claw_Up_Delay;//延时时间 S
extern uint8_t Big_Claw_Up_Delay_Flag;//打开定时器标志位

extern int64_t Big_Claw_Up_Delay_Pool;
extern uint8_t Big_Claw_Up_Delay_Pool_Flag;
/************************************************************/

void BigCarRunning(void);
void HorizontalMoving(float x,float y);
void VerticalMoving(float z);
void VerticalMoveOrigin(float z);
void ClosePaw(void);
void ReleasePaw(void);
void RisePawFromBurnPool(void);
void RisePawFromLitterPool(void);
void LowerClawtoPool(void);
void XMoving(float x);
void YMoving(float y);
void DownPaw(float z);
void ResetFlagBit(void);
#endif /* __TASK_H */

