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



/************************************************************/

static void BigCarTask(void);
static void BigCarRunning(void);
static void HorizontalMoving(float x,float y);
static void VerticalMoving(float z);
static void VerticalMoveOrigin(float z);
static void RisePawFromBurnPool(void);
static void RisePawFromPlatform(void);
static void RisePawFromLitterPool(void);
static void LowerClawtoBurnPool(void);
#endif /* __TASK_H */

