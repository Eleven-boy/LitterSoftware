#ifndef UART2_H__
#define UART2_H__

#include "stm32f4xx.h"

extern int Openmv_Up_Dis_F;
extern int Openmv_Up_Dis_B;
extern int Openmv_Up_RTDis_F;
extern int Openmv_Up_RTDis_B;

extern void USART2_Init(u32 bound);
extern void USART2_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
extern void USART2_Upper_f_Computer(float data1,float data2,float data3,float data4);	 

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	 


#endif

