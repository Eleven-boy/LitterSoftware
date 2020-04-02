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

extern uint8_t HTaskModeFlag;

/************************************************************/

void BigCarTask(void);
void BigCarRunning(void);
void HorizontalMoving(float x,float y);
void VerticalMoving(float z);
void VerticalMoveOrigin(float z);
void RisePawFromBurnPool(void);
void RisePawFromPlatform(void);
void RisePawFromLitterPool(void);
void LowerClawtoBurnPool(void);
#endif /* __TASK_H */

