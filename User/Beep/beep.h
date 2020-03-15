#ifndef __BEEP_H
#define	__BEEP_H

#include "stm32f4xx.h"

//���Ŷ���
/*******************************************************/

#define BEEP_PIN                  GPIO_Pin_6                 
#define BEEP_GPIO_PORT            GPIOE                     
#define BEEP_GPIO_CLK             RCC_AHB1Periph_GPIOE

/************************************************************/


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			 {p->BSRRL=i;}		//����Ϊ�ߵ�ƽ
#define digitalLo(p,i)			 {p->BSRRH=i;}		//����͵�ƽ
#define digitalToggle(p,i)	 {p->ODR ^=i;}		//�����ת״̬

/* �������IO�ĺ� */
#define BEEP_TOGGLE		digitalToggle(BEEP_GPIO_PORT,BEEP_PIN)
#define BEEP_ON			  digitalHi(BEEP_GPIO_PORT,BEEP_PIN)
//#define BEEP_OFF			digitalLo(BEEP_GPIO_PORT,BEEP_PIN)
#define BEEP_OFF			digitalLo(BEEP_GPIO_PORT,BEEP_PIN)

void BEEP_Init(void);


#endif /* __LED_H */