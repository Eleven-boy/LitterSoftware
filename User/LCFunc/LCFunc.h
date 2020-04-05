#ifndef __LCFUNC_H
#define	__LCFUNC_H

#include "stm32f4xx.h"




/*******************************************************
 *                     �ṹ�嶨��
 *******************************************************/

 /*
 *************************************************************************
 *                        �궨��
 *************************************************************************  
 */
#define GARBAGE_X        16000       //������y�᳤
#define GARBAGE_Y        500         //������x�᳤
#define ADD_X            11400       //�Ͽ�̨x�᳤
#define BIG_CLAW_UP_STOP 5400        //צ������ֹͣʱ����צ�Ӽ����ϵľ���
#define BIG_CLAW_DOWN_STOP2 4500     //צ���½����Ͽ��Ϸ�ֹͣʱ�������צ���ϵľ���
#define BIG_CLAW_DOWN_STOP3 3500     //צ���½����Ͽ��Ϸ�ֹͣʱ�������צ���ϵľ���
#define BIG_CLAW_DOWN_STOP4 4500     //צ���½����Ͽ��Ϸ�ֹͣʱ�������צ���ϵľ���
#define ORIGIN_X            7900     //��צ�ӳ�ʼXλ��
#define ORIGIN_Y            4000     //��צ�ӳ�ʼYλ��
#define ORIGIN_Z            1700     //��צ�ӳ�ʼZλ��
#define BIG_CLAW_BASE_DIS  1600      //צ���ŵ�ʱ��������ľ���
#define BIG_CAR_X_OFFSET  2000
#define BURN_POOL_X  4600            //���г�ͣ�����Ͽ��Ϸ�ʱx����
#define BURN_POOL_Y  4400            //���г�ͣ�����Ͽ��Ϸ�ʱy����
#define BURN_POOL_DZ  5000            //���г��ڷ��ϳ��Ϸ��·�ʱ���漤��綥������
#define BURN_POOL_UZ  3000            //���г��ڷ��ϳ��Ϸ�����ʱ���漤��綥������
#define PLATFORM_UZ  3000            //���г�����¥ƽ̨�Ϸ�����ʱ���漤��綥������

#define abs(x)  ((x<0)?(-x):x)       //��x�ľ���ֵ

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
//1����ʾ��£
extern uint8_t CloseFlag;
//��£��ʱ
extern int     CloseDelay;
//1:��ʾ��
extern uint8_t OpenFlag;
//����ʱ
extern int     OpenDelay;

extern int64_t Big_Claw_Up_Delay;//��ʱʱ�� S
extern uint8_t Big_Claw_Up_Delay_Flag;//�򿪶�ʱ����־λ

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

