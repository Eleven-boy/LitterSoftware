#include "exti.h"
#include "bsp_led.h" 
void EXTIX_Init(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;

		//IO�ڳ�ʼ��
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource11);//PD11 ������11
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource12);//PD12 ������12
	
		/* ����EXTI_Line11 */
		EXTI_InitStructure.EXTI_Line = EXTI_Line11|EXTI_Line12;//LINE11,LINE12
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //��������
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE10
		EXTI_Init(&EXTI_InitStructure);
  
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�15-10
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//��Ӧ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�
		NVIC_Init(&NVIC_InitStructure);//NVIC		
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ���
	{
		
		EXTI_ClearITPendingBit(EXTI_Line11); //���LINE11�ϵ��жϱ�־λ
	}
	
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ���
	{
		
		EXTI_ClearITPendingBit(EXTI_Line12); //���LINE12�ϵ��жϱ�־λ
	}	
}
/*********************************************END OF FILE**********************/
