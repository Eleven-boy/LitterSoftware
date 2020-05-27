#include "usart3.h"
#include "Data_type.h"
#include "Manual.h"
#include "RS485.h"
#include "CRC.h"
#include "bsp_led.h" 

#define U3_BUFFSIZERECE  100
#define U3_BUFFSIZESEND  100

uint8_t u3_receive_buff[U3_BUFFSIZERECE] = {0};
uint8_t u3_send_buff[U3_BUFFSIZESEND] = {0};
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
#define DMACLEAR                         DMA1->LIFCR = DMA_Stream3_IT_MASK;


#define USART_RX_DMA 										DMA1_Stream1
#define USART_RX_DMA_Channel   					DMA_Channel_4
#define USART_RX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))

#define USART_TX_DMA 										DMA1_Stream3
#define USART_TX_DMA_Channel   					DMA_Channel_4
#define USART_TX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))


static void bsp_initUSART(u32 bound);
static void USART3_DMA_RxConfig(void);
static void USART3_RX_DMAReset(void);
static void USART3_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize);

void USART3_Init(u32 bound)
{
	bsp_initUSART(bound);
}


static void bsp_initUSART(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK|USARTx_RX_GPIO_CLK, ENABLE);//ʹ��GPIOAʱ��

	USARTx_CLK_INIT(USARTx_CLK, ENABLE);

	RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);  //ʹ��DMA1ʱ��

	//����4��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
	GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

	 //USART�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN; 
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  //UART4 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);
	
	USART3_DMA_RxConfig();   //����DMA
	USART3_DMA_Tx_init(0,10);   //���÷���DMA
	USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);

	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  //��������ж�	

   //USART NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;//����4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         	//�����ȼ�0  ��һʱ����Ӧ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	USART_ITConfig(USARTx,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USARTx,USART_IT_TC,ENABLE);
	USART_Cmd(USARTx, ENABLE);
	USART_ClearFlag(USARTx, USART_FLAG_TC);
}

static void USART3_DMA_RxConfig(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	DMA_DeInit(USART_RX_DMA);
	DMA_Cmd(USART_RX_DMA, DISABLE); 
	while (DMA_GetCmdStatus(USART_RX_DMA) != DISABLE)
	{
	}
	
	DMA_InitStructure.DMA_BufferSize = U3_BUFFSIZERECE;              
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;   
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;       
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                 
	DMA_InitStructure.DMA_PeripheralBaseAddr =USART_RX_DMA_PeripheralBaseAddr ; 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;     
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                  
	

	DMA_InitStructure.DMA_Channel = USART_RX_DMA_Channel ;    
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;    
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)u3_receive_buff ;
	DMA_Init(USART_RX_DMA,&DMA_InitStructure);
	
	DMA_Cmd(USART_RX_DMA, ENABLE); //����DMA����
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


static void USART3_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize)
{
	DMA_InitTypeDef  DMA_InitStructure;
	

	DMA_DeInit(USART_TX_DMA);	
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
}

void USART3_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
	
//	for(int i = 0;i < BufferSize;i++)
//	{
//		u3_send_buff[i] = BufferSRC[i];
//	}	
//	DMA_Cmd(USART_TX_DMA, DISABLE);
//	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
//	{
//	}
//	DMA_SetCurrDataCounter(USART_TX_DMA,BufferSize);	
//	DMA_Cmd(USART_TX_DMA, ENABLE); 
	DMA_Cmd(USART_TX_DMA, DISABLE);
	while (DMA_GetCmdStatus(USART_TX_DMA) != DISABLE)
	{
	}
	
	USART_TX_DMA->NDTR = BufferSize;
	USART_TX_DMA->M0AR = (uint32_t)BufferSRC;

	DMA_Cmd(USART_TX_DMA, ENABLE); 
}
/*
void DMA1_Stream3_IRQHandler(void)
{
	//DMA transmission completed?
	if(DMA_GetFlagStatus(USART_TX_DMA,DMA_FLAG_TCIF3) != RESET)
	{
		//Clear flag
		DMA_ClearFlag(USART_TX_DMA, DMA_FLAG_TCIF3 | DMA_FLAG_FEIF3 | 
					  DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3);
		
		//Wait for USART1 to send complete flag TC is set
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
		//Clear send complete flag
		USART_ClearFlag(USART3, USART_FLAG_TC); 
		
		RS485_DE = 0;
		RS485_RE = 1;
	}
}
*/
static void USART3_RX_DMAReset(void)
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
	   USART3_RX_DMAReset();
		
		 //����֡����
		 if(0xAB == u3_receive_buff[0])//��ַ��
		 {			 
			 Up_Data_Flag = 1;
			 
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
						target.x[0] = ((int)u3_receive_buff[12]<<16)|((int)u3_receive_buff[13]<<8)|(u3_receive_buff[14]);
						target.y[0] = ((int)u3_receive_buff[14]<<15)|((int)u3_receive_buff[16]<<8)|(u3_receive_buff[17]); 

						if((u3_receive_buff[21]>>4)==0x01)
							IsExecute = 1;//ȷ��ִ����һ��
						
						WaitFlag = 1;
						Run_Mode = 2;	
						
						Up_Data.Status &= 0xF8;
						Up_Data.Status |= 0x02;//���ظ�ֵ��˵���ѳɹ����յ�			
					}
					else if(3==(u3_receive_buff[2]&0x07))//ȫ�Զ�
					{
						target.x[0] = ((int)u3_receive_buff[12]<<16)|((int)u3_receive_buff[13]<<8)|(u3_receive_buff[14]);
						target.y[0] = ((int)u3_receive_buff[14]<<15)|((int)u3_receive_buff[16]<<8)|(u3_receive_buff[17]); 

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
//		DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3 | DMA_FLAG_FEIF3 | 
//					  DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3);
//		DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3);
		DMACLEAR;
		DMA_Cmd(USART_TX_DMA, DISABLE); 
		RS485_DE = 0;
		RS485_RE = 0;
	}
}




