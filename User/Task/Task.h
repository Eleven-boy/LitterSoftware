#ifndef __TASK_H
#define	__TASK_H

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
#define ADD_X            11400       //�Ͽ�̨y�᳤
#define BIG_CLAW_UP_STOP 5400        //צ������ֹͣʱ����צ�Ӽ����ϵľ���
#define BIG_CLAW_DOWN_STOP2 4500     //צ���½����Ͽ��Ϸ�ֹͣʱ�������צ���ϵľ���
#define BIG_CLAW_DOWN_STOP3 3500     //צ���½����Ͽ��Ϸ�ֹͣʱ�������צ���ϵľ���
#define BIG_CLAW_DOWN_STOP4 4500     //צ���½����Ͽ��Ϸ�ֹͣʱ�������צ���ϵľ���
#define BIG_CLAW_BASE_DIS  1600      //צ���ŵ�ʱ��������ľ���
#define BIG_CAR_X_OFFSET  2000
#define BURN_POOL_X  4600            //���г�ͣ�����Ͽ��Ϸ�ʱx����
#define BURN_POOL_Y  4400            //���г�ͣ�����Ͽ��Ϸ�ʱy����


#define abs(x)  ((x<0)?(-x):x)       //��x�ľ���ֵ

extern int64_t Big_Claw_Up_Delay;//��ʱʱ�� S
extern uint8_t Big_Claw_Up_Delay_Flag;//�򿪶�ʱ����־λ

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

