#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx.h"

extern u16 ADC_Converted_Buff; //ADC���ݻ���

void ADC_DMA_Init(void);
float Get_Vol(u16 AdcVal);

#endif
