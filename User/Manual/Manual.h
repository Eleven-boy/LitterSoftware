#ifndef __MANUAL_H
#define	__MANUAL_H

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

void ManualXMoving(float x);
void ManualYMoving(float y); 
void ManualClose(void);
void ManualOpen(void);
#endif /* __TASK_H */
