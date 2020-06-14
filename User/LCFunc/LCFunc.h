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
#define PLATFORM_UZ   3000            //���г�����¥ƽ̨�Ϸ�����ʱ���漤���ײ�����   


#define UWBUPDIS               3000            //���г�̧����Ŀ��λ�ô�צ�Ӿ�󳵵ľ���
#define UWBDOWNBURNPOOLDIS     5000            //���г��½������ϳ��ϵ�Ŀ��λ�ô�צ�Ӿ�󳵵ľ���

#define abs(x)  ((x<0)?(-(x)):x)       //��x�ľ���ֵ

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
//1����ʾ��£
extern int CloseFlag;
//��£��ʱ
extern int     CloseDelay;
//1:��ʾ��
extern int OpenFlag;
//����ʱ
extern int     OpenDelay;

extern int64_t Big_Claw_Up_Delay;//��ʱʱ�� S
extern uint8_t Big_Claw_Up_Delay_Flag;//�򿪶�ʱ����־λ

extern int64_t Big_Claw_Up_Delay_Pool;
extern uint8_t Big_Claw_Up_Delay_Pool_Flag;

extern u8 CommunicatDelay;

//��ͣʱ��Ҫ�����ı���
typedef struct
{
	uint8_t _WaitFlag;//��ʱͨѶ״̬
	uint8_t _Run_Mode;//����ģʽ
	uint8_t _ReverseStop;//����ֹͣ��־λ
	uint8_t _PointMove;//�㶯��־λ
	uint8_t _CloseFlag;//צ�Ӻ�£��־λ
	uint8_t _OpenFlag;//צ���ſ���־λ
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

