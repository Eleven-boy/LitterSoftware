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
#define Manual 3  //1:手动方式1 2：手动方式2  3：手动方式3

extern uint8_t HTaskModeFlag;

/************************************************************/
#if Manual == 1
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
extern void ManualPowerOn(void);
extern void ManualPowerOff(void);
#elif Manual == 2
extern void ManualMoveToNorth(void);
extern void ManualMoveToSouth(void);
extern void ManualMoveToWest(void);
extern void ManualMoveToEast(void);
extern void ManualRaisePaw(void);
extern void ManualDownPaw(void);
extern void ManualClosePaw(void);
extern void ManualOpenPaw(void);
extern void ManualPowerOn(void);
extern void ManualPowerOff(void);
#elif Manual == 3
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
extern void ManualPowerOn(void);
extern void ManualPowerOff(void);
extern void ManualRaiseBigPaw(float z);
#endif
#endif /* __TASK_H */

