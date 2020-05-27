#include "alarm.h"

void Alarm_GPIO_Init(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); 

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOD, &GPIO_InitStructure);	  
		
}


/*********************************************END OF FILE**********************/
