/*
******************************************************************************************************************
*Filename      :RS485.c
*Programmer(s) :chu
*Description   :Design for RS485
               
								485_TX---PC12---TX5
								485_RX---PD2----RX5
								485_RE---PD3

******************************************************************************************************************
*/

#include "usart3.h"
#include "RS485.h"


/*
******************************************************************************************************************
*                                            CONSTANTS & MACROS
******************************************************************************************************************
*/

/*
*****************************************************************************************************************
*                                     void RS485_Init(void) 
*
*Description : RS485���ں���λ��ͨѶ
*Arguments   : none
*Returns     : none
*Notes       : none
*******************************************************************************************************************
*/	

void RS485_Init(void)
{
	USART3_Init(115200);												        //��ʼ������5
	GPIO_InitTypeDef  GPIO_InitStructure;							  //��ʼ��ģʽ���ö˿�RE
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);			  //ʹ��GPIOCʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;					  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					  //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);							  //��ʼ��GPIO
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);								  //���ͣ�Ĭ��Ϊ����ģʽ
}



/*********************************************************************
*							�ӿں���:��������
*����:BufferSRC:�������ݴ�ŵ�ַ
*	  BufferSize:���������ֽ���
**********************************************************************/
void RS485_Send_Data(uint32_t *BufferSRC, uint32_t BufferSize)//UASRT DMA��������
{
	//RS485_mode=1;
	DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);
	DMA1_Stream7->NDTR = BufferSize;    		    //����BUFFER��С
	DMA1_Stream7->M0AR = (uint32_t)BufferSRC;   //���õ�ַ
	DMA1_Stream7->CR |= DMA_SxCR_EN;			      //��DMA,��ʼ����
	//delay_ms(1);
	//RS485_mode=0;
	
}






