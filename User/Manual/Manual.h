#ifndef __MANUAL_H
#define	__MANUAL_H

#include "stm32f4xx.h"

/*******************************************************
 *                     结构体定义
 *******************************************************/

 /*
 *************************************************************************
 *                        宏定义
 *************************************************************************  
 */

extern uint8_t HTaskModeFlag;

/************************************************************/

extern void ManualXMoving(float x);
extern void ManualYMoving(float y); 
extern void ManualClose(void);
extern void ManualOpen(void);
extern void ManualRaisePawFromLitterPool(float z);
extern void ManualRisePawFromPlatform(float z);
extern void ManualRisePawFromBurnPool(float z);
extern void ManualDowntoLitterPool(float z);
extern void ManualDownClawtoBurnPool(float z);
extern void ManualDownToOrigin(float z);
#endif /* __TASK_H */

