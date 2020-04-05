#ifndef __LCFUNC_H
#define	__LCFUNC_H

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
#define ADD_X            11400       //料坑台x轴长
#define BIG_CLAW_UP_STOP 5400        //爪子上升停止时打在爪子激光上的距离
#define BIG_CLAW_DOWN_STOP2 4500     //爪子下降到料坑上方停止时激光打在爪子上的距离
#define BIG_CLAW_DOWN_STOP3 3500     //爪子下降到料坑上方停止时激光打在爪子上的距离
#define BIG_CLAW_DOWN_STOP4 4500     //爪子下降到料坑上方停止时激光打在爪子上的距离
#define ORIGIN_X            7900     //大爪子初始X位置
#define ORIGIN_Y            4000     //大爪子初始Y位置
#define ORIGIN_Z            1700     //大爪子初始Z位置
#define BIG_CLAW_BASE_DIS  1600      //爪子着地时激光距地面的距离
#define BIG_CAR_X_OFFSET  2000
#define BURN_POOL_X  4600            //大行车停留在料坑上方时x坐标
#define BURN_POOL_Y  4400            //大行车停留在料坑上方时y坐标
#define BURN_POOL_DZ  5000            //大行车在焚料池上方下放时上面激光剧顶部距离
#define BURN_POOL_UZ  3000            //大行车在焚料池上方上升时上面激光剧顶部距离
#define PLATFORM_UZ  3000            //大行车在五楼平台上方上升时上面激光剧顶部距离

#define abs(x)  ((x<0)?(-x):x)       //求x的绝对值

extern uint8_t X_MOVE_BIT;
extern uint8_t Y_MOVE_BIT;
extern uint8_t DOWN_BIT;
extern uint8_t UP_BIT;


extern uint8_t BigCarDataCorrect; 
extern uint8_t BigClawDataCorrect;
extern uint8_t HTaskModeFlag;
extern uint8_t ReverseStop;
extern int ReverseTime;
extern uint8_t PointMove;
extern int PointMoveTime;
extern uint8_t UpOrDown;
extern uint8_t ErrorBigCar;
//1：表示合拢
extern uint8_t CloseFlag;
//合拢延时
extern int     CloseDelay;
//1:表示打开
extern uint8_t OpenFlag;
//打开延时
extern int     OpenDelay;

extern int64_t Big_Claw_Up_Delay;//延时时间 S
extern uint8_t Big_Claw_Up_Delay_Flag;//打开定时器标志位

extern int64_t Big_Claw_Up_Delay_Pool;
extern uint8_t Big_Claw_Up_Delay_Pool_Flag;
/************************************************************/

void ClosePaw(void);
void ReleasePaw(void);
void XMoving(float x);
void YMoving(float y);
void UpPawFromBurnPool(float z);
void UpPawFromPlatform(float z);
void UpPawFromLitterPool(float z);
void DownPawToBurnPool(float z);
void DownPawToLitterPool(float z);
void DownPawToPlatform(float z);
void SelfCheckStatus(void);
void DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff);
void RevStop(float err);
void HorizontalDotMove(float err_x,float err_y);
void VerticalDotMove(float err_z);
void ResetFlagBit(void);
void RequestStop(uint8_t dev);
void RequestStart(uint8_t dev);
#endif /* __TASK_H */

