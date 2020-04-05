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



/************************************************************/
void FullRisePawFromPlatform(void);
void FullHorizontalMoving(float x,float y);
void FullDowntoLitterPool(float z);
void FullDowntoLitterPool(float z);
void FullClosePaw(void);
void FullRisePawFromLitterPool(void);
void FullDownClawtoBurnPool(void);
void FullOpenPaw(void);
void FullRisePawFromBurnPool(void);
void FullDownToOrigin(float z);
#endif /* __TASK_H */

