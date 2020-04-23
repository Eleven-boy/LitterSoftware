#include "usart3.h"
#include "Data_type.h"
#include "Manual.h"
#include "RS485.h"
#include "CRC.h"
#define U3_BUFFSIZERECE  100
#define U3_BUFFSIZESEND  100

uint8_t u3_receive_buff[U3_BUFFSIZERECE] = {0};
extern int8_t IsExecute;//1:ִ����һ��,0����ִ����һ��

/*
***************************************************************
*  USART3
*  TX   PB10
*  RX   PB11
*  RE   PA8
***************************************************************
*/

#define USARTx                           USART3
#define USARTx_CLK                       RCC_APB1Periph_USART3
#define USARTx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define USARTx_IRQn                      USART3_IRQn
#define USARTx_IRQHandler                USART3_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_10            
#define USARTx_TX_GPIO_PORT              GPIOB                      
#define USARTx_TX_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define USARTx_TX_SOURCE                 GPIO_PinSource10
#define USARTx_TX_AF                     GPIO_AF_USART3

#define USARTx_RX_PIN                    GPIO_Pin_11        
#define USARTx_RX_GPIO_PORT              GPIOB                    
#define USARTx_RX_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define USARTx_RX_SOURCE                 GPIO_PinSource11
#define USARTx_RX_AF                     GPIO_AF_USART3

#define USARTx_DMA                       DMA1
#define USARTx_DMAx_CLK                  RCC_AHB1Periph_DMA1
#define DMACLEAR                         DMA1->HIFCR = DMA_Stream3_IT_MASK;


#define USART_RX_DMA 										DMA1_Stream1
#define USART_RX_DMA_Channel   					DMA_Channel_4
#define USART_RX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))

#define USART_TX_DMA 										DMA1_Stream3
#define USART_TX_DMA_Channel   					DMA_Channel_4
#define USART_TX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))


static void bsp_initUSART(u32 bound);
static void USART_DMA_RxConfig(void);
static void USART_RX_DMAReset(void);
static void USART_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize);

void USART3_Init(u32 bound)
{
	bsp_initUSART(bound);
}


static void bsp_initUSART(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/*************************************��ʼ������3 ����˫��ͨѶ*************************************/
	/* ʹ�� GPIO clock */
	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK|USARTx_RX_GPIO_CLK, ENABLE);

	/* ʹ�� USART clock */
	USARTx_CLK_INIT(USARTx_CLK, ENABLE);

	/* ʹ��  DMA clock */
	RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);

	/* USART1 GPIO configuration -----------------------------------------------*/ 
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
	GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN; 
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);
	
	USART_DMA_RxConfig();   //����DMA
	USART_DMA_Tx_init(0,10);   //���÷���DMA
	/* ʹ�� USART DMA RX ���� */
	USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);

	/* ʹ�� USART DMA TX ���� */
	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	/* ʹ�ܷ��ʹ�������ж� */
	DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  	
	
   //USART NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         	//�����ȼ�1  ��һʱ����Ӧ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	//�����ж� 
	USART_ITConfig(USARTx,USART_IT_IDLE,ENABLE);
	//TC�жϣ���DMA�����������ʱ������TC�ж�
	USART_ITConfig(USARTx,USART_IT_TC,ENABLE);
	/* Enable USART */
	USART_Cmd(USARTx, ENABLE);
	USART_ClearFlag(USARTx, USART_FLAG_TC);
}

static void USART_DMA_RxConfig(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	/* 
	  ʹ��ǰ���DMA�Ƿ񱻽�ֹ��
	  ��ע�⣬��ʹ����ͬ�������Ķ�δ���ʱ����һ�������ã�ʹ�ܣ�Ȼ���ֹ��Ȼ��ʹ��
	  ..... ����������£�ֻ�������ݴ��������ʱ��DMA�Ľ�ֹ����Ч����ȷ��ʹ��λ��
	  Ӳ�����֮ǰ��ȷ���ǲ����ܵģ����DMA����ֻ��һ�Σ���һ�����Ժ��ԡ�
	*/
	DMA_DeInit(USART_RX_DMA);
	DMA_Cmd(USART_RX_DMA, DISABLE); 
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
	
	DMA_InitStructure.DMA_BufferSize = U3_BUFFSIZERECE;                 /* ����DMA��С */
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;          /* �������������ʹ�ܻ��߽�ֹ�����Ե� */
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;  /* ���÷�ֵ */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;   /* �����ڴ�Ϊ���ֽ�ͻ��ģʽ */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;/* �����ڴ����ݵ�λ�����ֽ� */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;        /* ʹ�ܵ�ַ���� */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                  /* ����DMA������ģʽ */
	DMA_InitStructure.DMA_PeripheralBaseAddr =USART_RX_DMA_PeripheralBaseAddr ;  /* ���������ַ */
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     /* ��������Ϊ���ֽ�ͻ��ģʽ */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; /* �����������ݵ�λ�����ֽ� */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;      /* ��ֹ�����ַ���� */
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   /* �������ȼ� */
	
	/* ���� RX DMA */
	DMA_InitStructure.DMA_Channel = USART_RX_DMA_Channel ;      /* ���ý���ͨ�� */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;     /* ���ô����赽�ڴ� */
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)u3_receive_buff ; /* �����ڴ��ַ */
	DMA_Init(USART_RX_DMA,&DMA_InitStructure);
	
	/* ʹ�� DMA USART TX Stream */
	DMA_Cmd(USART_RX_DMA, ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: USART_DMA_TxConfig
*	����˵��: ���ڵ�DMA��������
*	��    �Σ�BufferSRC   Դ��ַָ��
*             BufferSize  ��������С
*	�� �� ֵ: ��
*********************************************************************************************************
*/

#define TRANSFER_IT_ENABLE_MASK (uint32_t)(DMA_SxCR_TCIE | DMA_SxCR_HTIE | \
                                           DMA_SxCR_TEIE | DMA_SxCR_DMEIE)

#define DMA_Stream0_IT_MASK     (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                           DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                           DMA_LISR_TCIF0)

#define DMA_Stream1_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 6)
#define DMA_Stream2_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 16)
#define DMA_Stream3_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 22)
#define DMA_Stream4_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream5_IT_MASK     (uint32_t)(DMA_Stream1_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream6_IT_MASK     (uint32_t)(DMA_Stream2_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream7_IT_MASK     (uint32_t)(DMA_Stream3_IT_MASK | (uint32_t)0x20000000)


static void USART_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize)
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	/* ��λ DMA Stream �Ĵ��� (���ڵ���Ŀ��) */
 	/* DMA_DeInit(USARTx_TX_DMA_STREAM); */

    /* 
	  ʹ��ǰ���DMA�Ƿ񱻽�ֹ��
	  ��ע�⣬��ʹ����ͬ�������Ķ�δ���ʱ����һ�������ã�ʹ�ܣ�Ȼ���ֹ��Ȼ��ʹ��
	  ..... ����������£�ֻ�������ݴ��������ʱ��DMA�Ľ�ֹ����Ч����ȷ��ʹ��λ��
	  Ӳ�����֮ǰ��ȷ���ǲ����ܵģ����DMA����ֻ��һ�Σ���һ�����Ժ��ԡ�
	*/
	DMA_DeInit(USART_TX_DMA);	
	DMA_Cmd(USART_TX_DMA, DISABLE); 
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	DMA_InitStructure.DMA_BufferSize = BufferSize;/* ����DMA��С */
 	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;                  /* �������������ʹ�ܻ��߽�ֹ�����Ե� */
 	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;          /* ���÷�ֵ */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;           /* �����ڴ�Ϊ���ֽ�ͻ��ģʽ */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;        /* �����ڴ����ݵ�λ�����ֽ� */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                /* ʹ�ܵ�ַ���� */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                          /* ����DMA������ģʽ */
	DMA_InitStructure.DMA_PeripheralBaseAddr =USART_TX_DMA_PeripheralBaseAddr; /* ���������ַ */
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    /* ��������Ϊ���ֽ�ͻ��ģʽ */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;; /* �����������ݵ�λ�����ֽ� */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/* ��ֹ�����ַ���� */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;             /* �������ȼ� */
	
	/* ���� TX DMA */
	DMA_InitStructure.DMA_Channel = USART_TX_DMA_Channel ;      /* ���÷���ͨ�� */
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;     /* ���ô��ڴ浽���� */
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)BufferSRC ; /* �����ڴ��ַ */
	DMA_Init(USART_TX_DMA,&DMA_InitStructure);
	/* ʹ�ܷ��ʹ�������ж� */
	DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  		
	/* ʹ�� DMA USART TX Stream */
//	RS485SEND_1();
	DMA_Cmd(USART_TX_DMA, ENABLE); 
	
	/* 
	   ���DMA Stream �Ƿ���ȷ��ʹ��.���DMA�Ĳ������ô����ˣ���ôDMA Stream
	   ��ʹ��λ�ᱻӲ����������Ӷ�ʹ�ô���ֹͣ����FIFO�ķ�ֵ���ô���
	*/
}

void USART3_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
	
	/* ��λ DMA Stream �Ĵ��� (���ڵ���Ŀ��) */
 	/* DMA_DeInit(USARTx_TX_DMA_STREAM); */

    /* 
	  ʹ��ǰ���DMA�Ƿ񱻽�ֹ��
	  ��ע�⣬��ʹ����ͬ�������Ķ�δ���ʱ����һ�������ã�ʹ�ܣ�Ȼ���ֹ��Ȼ��ʹ��
	  ..... ����������£�ֻ�������ݴ��������ʱ��DMA�Ľ�ֹ����Ч����ȷ��ʹ��λ��
	  Ӳ�����֮ǰ��ȷ���ǲ����ܵģ����DMA����ֻ��һ�Σ���һ�����Ժ��ԡ�
	*/
	//DMA_DeInit(DMA1_Stream0);
	//DMA1_Stream0->CR  = 0;		
	DMA_Cmd(USART_TX_DMA, DISABLE);
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	USART_TX_DMA->NDTR = BufferSize;
	USART_TX_DMA->M0AR = (uint32_t)BufferSRC;
	/* ʹ�ܷ��ʹ�������ж� */
	//DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  		
	/* ʹ�� DMA USART TX Stream */
	DMA_Cmd(USART_TX_DMA, ENABLE); 
	
	/* 
	   ���DMA Stream �Ƿ���ȷ��ʹ��.���DMA�Ĳ������ô����ˣ���ôDMA Stream
	   ��ʹ��λ�ᱻӲ����������Ӷ�ʹ�ô���ֹͣ����FIFO�ķ�ֵ���ô���
	*/
	
}

static void USART_RX_DMAReset(void)
{ 
	DMA_Cmd(USART_RX_DMA, DISABLE);   
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
 	DMA_SetCurrDataCounter(USART_RX_DMA,U3_BUFFSIZERECE);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(USART_RX_DMA, ENABLE); 
}	

//���ж�

extern uint8_t WaitFlag;
extern uint8_t Run_Mode;
extern uint8_t Up_Data_Flag;
void USARTx_IRQHandler(void)
{	
	u8 i=0;
  u16 CRC_NUM;
	u8 valid_data[100];
	
	if (USART_GetITStatus(USARTx, USART_IT_IDLE) == SET)       //�����յ��������
	{
		 USART_ReceiveData(USARTx);                         //��ȡ���� ע�⣺������Ҫ�������ܹ�����жϱ�־λ��
		 uint8_t Uart1_Rec_Len = U3_BUFFSIZERECE - DMA_GetCurrDataCounter(USART_RX_DMA);			//����ӱ�֡���ݳ���
	   USART_RX_DMAReset();
		
		 //����֡����
		 if(0xAB == u3_receive_buff[0])//��ַ��
		 {
			 //CRCУ��
			 for(int j = 1;j<u3_receive_buff[1]-2;j++)
					valid_data[i++] = u3_receive_buff[j];
			 CRC_NUM = (u3_receive_buff[u3_receive_buff[1]-2] << 8 | u3_receive_buff[u3_receive_buff[1]-1]);
			 
			 if(CRC_NUM == CRC16(valid_data,i))//CRCУ��ͨ��
			 {
					if(0==(u3_receive_buff[2]&0x07))//����,��ʱ������״̬
					{
						//WaitFlag = 0;
						Up_Data.Status &= 0xF8;
						Up_Data.Status |= 0x00;//���ظ�ֵ��˵���ѳɹ����յ�	
					}
					else if(1==(u3_receive_buff[2]&0x07))//�ֶ�
					{
						WaitFlag = 1;
						Run_Mode = 1;
						HTaskModeFlag = u3_receive_buff[2]>>3;
						
						Up_Data.Status &= 0xF8;
						Up_Data.Status |= 0x01;//���ظ�ֵ��˵���ѳɹ����յ�									
					}
					else if(2==(u3_receive_buff[2]&0x07))//���Զ�
					{
						target.x[0] = (u3_receive_buff[12]<<16)|(u3_receive_buff[13]<<16)|(u3_receive_buff[14]);
						target.y[0] = (u3_receive_buff[14]<<15)|(u3_receive_buff[16]<<16)|(u3_receive_buff[17]); 
						
						WaitFlag = 1;
						Run_Mode = 2;	
						
						Up_Data.Status &= 0xF8;
						Up_Data.Status |= 0x02;//���ظ�ֵ��˵���ѳɹ����յ�			
					}
					else if(3==(u3_receive_buff[2]&0x07))//ȫ�Զ�
					{
						target.x[0] = (u3_receive_buff[12]<<16)|(u3_receive_buff[13]<<16)|(u3_receive_buff[14]);
						target.y[0] = (u3_receive_buff[14]<<15)|(u3_receive_buff[16]<<16)|(u3_receive_buff[17]); 
						
						if((u3_receive_buff[21]>>4)==0x01)
							IsExecute = 1;//ȷ��ִ����һ��
						
						WaitFlag = 1;
						Run_Mode = 3;		
						Up_Data.Status &= 0xF8;
						Up_Data.Status |= 0x03;//���ظ�ֵ��˵���ѳɹ����յ�
					}
					else if(4==(u3_receive_buff[2]&0x07))//  ��ͣ
					{
						
					}
					else if(5==(u3_receive_buff[2]&0x07))//  �ָ�
					{
					
					}
					else if(6==(u3_receive_buff[2]&0x07))//ֹͣ
					{
					
					}
					else if(7==(u3_receive_buff[2]&0x07))//����
					{
						
					}			 
			 }
				
				Up_Data_Flag = 1;
		 }
		
		 //ֱ�����ж��н��մ������� 

	}else if(USART_GetITStatus(USARTx, USART_IT_TC) == SET)
	{
		USART_ClearITPendingBit(USARTx, USART_IT_TC);         //����жϱ�־
		
		DMACLEAR;
		DMA_Cmd(USART_TX_DMA, DISABLE); 
	}

}




