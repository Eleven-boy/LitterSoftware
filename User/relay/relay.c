#include "relay.h"

int8_t RelayOnflag = -1;

int8_t RelayOffflag = -1;

void Relay_GPIO_Init(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 

													   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOD, &GPIO_InitStructure);	

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOE, &GPIO_InitStructure);	  
		
}
//��ң����
void RelayOn(void)
{
	if(RelayOnflag>=0 && RelayOnflag <=10)
	{
		CAR_START(ON);
	}
	else
	{
		RelayOnflag = -2;
		CAR_START(OFF); 
	}	
	 
}
//�ر�ң����
void RelayOff(void)
{
	if(RelayOffflag>=0 && RelayOffflag <=10)
	{
		CAR_STOP(ON);
	}
	else
	{
		RelayOffflag = -2;
		CAR_STOP(OFF);
	}	  
}
//�ϵ�
void PowerOn(void)
{
	if(RelayOnflag==-1)//��ң����
	{
		RelayOnflag = 0;
	}
	if(RelayOnflag>=0)
	{
		RelayOn();//��ң����
	}
}
//�ϵ�
void PowerOff(void)
{
		if(RelayOffflag==-1)//��ң����
		{
			RelayOffflag = 0;
		}
		if(RelayOffflag>=0)
		{
			RelayOff();//�ر�ң����
		}
}
/*********************************************END OF FILE**********************/
