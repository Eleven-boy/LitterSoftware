#include "ADC.h"
#include "bsp_SysTick.h"

u16 ADC_Converted_Buff[2];  //ADC���ݻ���

void DMA_ADC_Init(void)
{
	DMA_InitTypeDef    DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA1ʱ��ʹ�� 
	DMA_DeInit(DMA2_Stream0);
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);//�ȴ�DMA��������

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;//(u32)&ADC1->DR;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ADC_Converted_Buff;//DMA �ڴ����ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = 2;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢��������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;//�洢�����ݳ���:16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);//��ʼ��DMA Stream
 
	DMA_Cmd(DMA2_Stream0,ENABLE);                           //��DMA����
}


void  ADC_DMA_Init(void)
{
  GPIO_InitTypeDef  		GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);			//ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);			//ʹ��GPIOCʱ��
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);			//ʹ��ADC1ʱ��

	//�ȳ�ʼ��IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;							//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);										//��ʼ��  
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;							//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;					//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);										//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);				//ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);			//��λ����	 
 
 
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;	//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //DMAʧ�ܣ�ÿ���һ��ת������һ��DMA����
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; 
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;								//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;													//��ͨ��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;										//����ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;								//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 2;														//2��ת���ڹ��������� Ҳ����ֻת����������2 
  ADC_Init(ADC1, &ADC_InitStructure);
	 
	ADC_Cmd(ADC1, ENABLE);//����ADת����	 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 2, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    

	DMA_ADC_Init();//DMA ADC1 ��ʼ��
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������		
	
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//����Դ�任ʱ����DMA����
	ADC_DMACmd(ADC1, ENABLE);       //ADC1 DMA�������ݴ�		
}

/*
********************************************************************************************************************
*                  u16 Get_Vol(u16 AdcVal)
*
*Description :�õ���ѹֵ
*Arguments   :AdcVal:  ADC�Ĳ���ֵ
*Returns     :��צ������
*Notes       : none
********************************************************************************************************************
*/
float Get_Weight(u16 AdcVal)
{
//	static float vol;

//	vol=0.98f*vol+0.02f*AdcVal;
//	
//	return (vol*0.013293457f+1.5f);
	return 1;
}







