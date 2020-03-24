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


#define abs(x)  ((x<0)?(-x):x)       //��x�ľ���ֵ

extern uint8_t DataCorrect;
extern uint8_t HTaskModeFlag;
extern uint8_t ReverseStop;
extern int ReverseTime;
extern uint8_t PointMove;
extern int PointMoveTime;

extern int64_t Big_Claw_Up_Delay;//��ʱʱ�� S
extern uint8_t Big_Claw_Up_Delay_Flag;//�򿪶�ʱ����־λ

extern int64_t Big_Claw_Up_Delay_Pool;
extern uint8_t Big_Claw_Up_Delay_Pool_Flag;
/************************************************************/

void BigCarTask(void);
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
void SelfCheckStatus(void);
void DataCommunicateManage(uint8_t task_mode);
void RevStop(float err);
void DotMove(float err_x,float err_y);
void ResetFlagBit(void);
#endif /* __TASK_H */

