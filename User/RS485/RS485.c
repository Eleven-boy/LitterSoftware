/*
******************************************************************************************************************
*Filename      :RS485.c
*Programmer(s) :chu
*Description   :Design for RS485
******************************************************************************************************************
*/
#include "usart3.h"
#include "RS485.h"
#include "GPIO.h"
#include "CRC.h"
#include "Data_type.h"   
#include "bsp_SysTick.h"
/*
******************************************************************************************************************
*                                            CONSTANTS & MACROS
******************************************************************************************************************
*/

#define RS485_DE_PIN                     GPIO_Pin_1            
#define RS485_DE_GPIO_PORT               GPIOA                      
#define RS485_DE_GPIO_CLK                RCC_AHB1Periph_GPIOA
#define RS485_DE_SOURCE                  GPIO_PinSource1


#define RS485_RE_PIN                     GPIO_Pin_2           
#define RS485_RE_GPIO_PORT               GPIOA                      
#define RS485_RE_GPIO_CLK                RCC_AHB1Periph_GPIOA
#define RS485_RE_SOURCE                  GPIO_PinSource2
/*
*****************************************************************************************************************
*                                     void RS485_Init(u32 bound) 
*
*Description : RS485���ں���λ��ͨѶ
*Arguments   : none
*Returns     : none
*Notes       : none
*******************************************************************************************************************
*/	

void RS485_Init(u32 bound)
{
	USART3_Init(bound);												        //��ʼ������3
	GPIO_InitTypeDef  GPIO_InitStructure;							  //��ʼ��ģʽ���ö˿�RE,DE
	
	RCC_AHB1PeriphClockCmd(RS485_RE_GPIO_CLK, ENABLE);		//ʹ��GPIOCʱ��
	
	GPIO_InitStructure.GPIO_Pin = RS485_DE_PIN|RS485_RE_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;					  //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					  //����
	GPIO_Init(RS485_DE_GPIO_PORT, &GPIO_InitStructure);			//��ʼ��GPIO
      
	RS485_DE = 0;//���ͣ�Ĭ��Ϊ����ģʽ
	RS485_RE = 0;//���ͣ�Ĭ��Ϊ����ģʽ
}


#define BYTE0(dwTemp)       (*(char *)(&dwTemp))      
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
/*********************************************************************
*							�ӿں���:��������
*����:none
*	  
****************************************** ****************************/
void RS485_Send_Data(void)//UASRT DMA��������
{
	u8 _cnt = 0,c_cnt = 0;
	u8 i = 0;
	u16 CRC_NUM = 0;
	static u8  send_data[100]={0};
	u8 valid_data[100]={0};
	
//	Up_Data.P_x =1234;
//	Up_Data.P_y =-2;
//	Up_Data.P_z = 20000;
//	Up_Data.A_x = 10;
//	Up_Data.A_y = 12;
	
	Up_Data.Add = 0xAA;
	
	send_data[_cnt++]  = Up_Data.Add;  //��ַ��
	
	_cnt++;                                 //��һ����������ŷ��͵����ֽ���
	
	send_data[_cnt++]  = BYTE0(Up_Data.P_x);//x����
	send_data[_cnt++]  = BYTE1(Up_Data.P_x);
	send_data[_cnt++]  = BYTE2(Up_Data.P_x);

	send_data[_cnt++]  = BYTE0(Up_Data.P_y);//y����
	send_data[_cnt++]  = BYTE1(Up_Data.P_y);
	send_data[_cnt++]  = BYTE2(Up_Data.P_y);
	
	send_data[_cnt++]  = BYTE0(Up_Data.P_z);//z����
	send_data[_cnt++]  = BYTE1(Up_Data.P_z);
	send_data[_cnt++]  = BYTE2(Up_Data.P_z);

	send_data[_cnt++]  = BYTE0(Up_Data.A_x);//6050x��Ƕ�
	send_data[_cnt++]  = BYTE1(Up_Data.A_x);
	
	send_data[_cnt++]  = BYTE0(Up_Data.A_y);//6050y��Ƕ�
	send_data[_cnt++]  = BYTE1(Up_Data.A_y);
	
	send_data[_cnt++]  = Up_Data.HalfStep;  //��0��Ч����Ҫ������²�����
	
	send_data[_cnt++]  = Up_Data.Status;    //�г�����״̬
	
	//CRCУ��
	for(c_cnt=2;c_cnt < _cnt;c_cnt++) valid_data[i++] = send_data[c_cnt];
	CRC_NUM = CRC16(valid_data,i);

	send_data[_cnt++] = BYTE0(CRC_NUM);
	send_data[_cnt++]	= BYTE1(CRC_NUM);
	
	send_data[_cnt++] = 0xEE;//������
	
	send_data[1] = _cnt;//�������ݵ����ֽ���
	
	RS485_DE = 1;
	RS485_RE = 1;
	USART3_DMA_TxConfig((u32*)send_data, _cnt);
	
}






