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
#define PLATFORM_UZ   3000            //大行车在五楼平台上方上升时下面激光距底部距离   


#define UWBUPDIS               3000            //大行车抬升到目标位置大爪子距大车的距离
#define UWBDOWNBURNPOOLDIS     5000            //大行车下降到焚料池上的目标位置大爪子距大车的距离

#define abs(x)  ((x<0)?(-(x)):x)       //求x的绝对值

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
extern uint8_t ManualError;
//1：表示合拢
extern int CloseFlag;
//合拢延时
extern int     CloseDelay;
//1:表示打开
extern int OpenFlag;
//打开延时
extern int     OpenDelay;

extern int64_t Big_Claw_Up_Delay;//延时时间 S
extern uint8_t Big_Claw_Up_Delay_Flag;//打开定时器标志位

extern int64_t Big_Claw_Up_Delay_Pool;
extern uint8_t Big_Claw_Up_Delay_Pool_Flag;

extern u8 CommunicatDelay;

//暂停时需要保护的变量
typedef struct
{
	uint8_t _WaitFlag;//此时通讯状态
	uint8_t _Run_Mode;//任务模式
	uint8_t _ReverseStop;//反向停止标志位
	uint8_t _PointMove;//点动标志位
	uint8_t _CloseFlag;//爪子合拢标志位
	uint8_t _OpenFlag;//爪子张开标志位
}Provar;
/************************************************************/

extern void ClosePaw(void);
extern void ReleasePaw(void);
extern void XMoving(float x);
extern void YMoving(float y);
extern void UpPawFromBurnPool(float z);
extern void UpPawFromPlatform(float z);
extern void UpPawFromLitterPool(float z);
extern void DownPawToBurnPool(float z);
extern void DownPawToLitterPool(float z);
extern void DownPawToPlatform(float z);
extern void SelfCheckStatus(void);
extern void DataCommunicateManage(uint8_t task_mode,uint8_t OnorOff);
extern void RevStop(float err);
extern void HorizontalDotMove(float err_x,float err_y);
extern void VerticalDotMove(float err_z);
extern void ResetFlagBit(void);
extern void RequestStop(uint8_t dev);
extern void RequestStart(uint8_t dev);
extern void TaskSuspend(void);
extern void ConExecute(void);
extern void BigCarStop(void);

extern void RaiseBigPaw(float z);
extern void DeclineBigPawtoLitterPool(float z);
extern void DeclineBigPawtoPlatForm(float z);
extern void DeclineBigPawtoBurnPool(float z);
#endif /* __TASK_H */

