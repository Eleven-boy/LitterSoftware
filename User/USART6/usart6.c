#include "usart6.h"


#define U6_BUFFSIZERECE  100
#define U6_BUFFSIZESEND  100

uint8_t u6_receive_buff[U6_BUFFSIZERECE] = {0};


/*
***************************************************************
*  USART6
*  TX   PC6
*  RX   PC7
***************************************************************
*/

#define USARTx                           USART6
#define USARTx_CLK                       RCC_APB2Periph_USART6
#define USARTx_CLK_INIT                  RCC_APB2PeriphClockCmd
#define USARTx_IRQn                      USART6_IRQn
#define USARTx_IRQHandler                USART6_IRQHandler

#define USARTx_TX_PIN                    GPIO_Pin_6            
#define USARTx_TX_GPIO_PORT              GPIOC                     
#define USARTx_TX_GPIO_CLK               RCC_AHB1Periph_GPIOC
#define USARTx_TX_SOURCE                 GPIO_PinSource6
#define USARTx_TX_AF                     GPIO_AF_USART6

#define USARTx_RX_PIN                    GPIO_Pin_7        
#define USARTx_RX_GPIO_PORT              GPIOC                    
#define USARTx_RX_GPIO_CLK               RCC_AHB1Periph_GPIOC
#define USARTx_RX_SOURCE                 GPIO_PinSource7
#define USARTx_RX_AF                     GPIO_AF_USART6

#define USARTx_DMA                       DMA2
#define USARTx_DMAx_CLK                  RCC_AHB1Periph_DMA2
#define DMACLEAR                         DMA2->HIFCR = DMA_Stream6_IT_MASK;



#define USART_RX_DMA 							DMA2_Stream1
#define USART_RX_DMA_Channel   					DMA_Channel_5
#define USART_RX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))

#define USART_TX_DMA 							DMA2_Stream6
#define USART_TX_DMA_Channel   					DMA_Channel_5
#define USART_TX_DMA_PeripheralBaseAddr  		(uint32_t) (&(USARTx->DR))


static void bsp_initUSART(u32 bound);
static void USART_DMA_RxConfig(void);
static void USART_RX_DMAReset(void);
static void USART_DMA_Tx_init(uint32_t *BufferSRC, uint32_t BufferSize);

void USART6_Init(u32 bound)
{
	bsp_initUSART(bound);
}

static void bsp_initUSART(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/*************************************��ʼ������1 ����˫��ͨѶ*************************************/
	/* ʹ�� GPIO clock */
	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK|USARTx_RX_GPIO_CLK, ENABLE);

	/* ʹ�� USART clock */
	USARTx_CLK_INIT(USARTx_CLK, ENABLE);

	/* ʹ��  DMA clock */
	RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);

	/* USART6 GPIO configuration -----------------------------------------------*/ 
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
	
	DMA_InitStructure.DMA_BufferSize = U6_BUFFSIZERECE;                 /* ����DMA��С */
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
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)u6_receive_buff ; /* �����ڴ��ַ */
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
	//DMA_ITConfig(USART_TX_DMA, DMA_IT_TC, ENABLE);  		
	/* ʹ�� DMA USART TX Stream */
//	RS485SEND_1();
//	DMA_Cmd(DMA2_Stream6, ENABLE); 
	
	/* 
	   ���DMA Stream �Ƿ���ȷ��ʹ��.���DMA�Ĳ������ô����ˣ���ôDMA Stream
	   ��ʹ��λ�ᱻӲ����������Ӷ�ʹ�ô���ֹͣ����FIFO�ķ�ֵ���ô���
	*/
}

void USART6_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
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
 	DMA_SetCurrDataCounter(USART_RX_DMA,U6_BUFFSIZERECE);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(USART_RX_DMA, ENABLE); 
}	

uint8_t Usart6_Rec_Len;
//���ж�
void USARTx_IRQHandler(void)
{	
	if (USART_GetITStatus(USARTx, USART_IT_IDLE) == SET)       //�����յ��������
	{
	//	 bsp_LedToggle(1);
		 USART_ReceiveData(USARTx);                         //��ȡ���� ע�⣺������Ҫ�������ܹ�����жϱ�־λ��
		 Usart6_Rec_Len = U6_BUFFSIZERECE - DMA_GetCurrDataCounter(USART_RX_DMA);			//����ӱ�֡���ݳ���
	    USART_RX_DMAReset();
		 //����֡����
		 //ֱ�����ж��н��մ������� 

	}else if(USART_GetITStatus(USARTx, USART_IT_TC) == SET)
	{
		USART_ClearITPendingBit(USARTx, USART_IT_TC);         //����жϱ�־
		
		DMACLEAR;
		DMA_Cmd(USART_TX_DMA, DISABLE); 
	}

}

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))      
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
void USART6_Upper_f_Computer(float data1,float data2,float data3,float data4,float data5,float data6)
{
	u8 i,sum;
	static u8 Send_DATA[29];       
	Send_DATA[0] =0xAA;    
	Send_DATA[1] =0xAA;			
	Send_DATA[2] =0xF1;    
	Send_DATA[3] =24;      
	
	Send_DATA[4]  = BYTE3(data1);  
	Send_DATA[5]  = BYTE2(data1);
	Send_DATA[6]  = BYTE1(data1);
	Send_DATA[7]  = BYTE0(data1);
	
	Send_DATA[8]  = BYTE3(data2);
	Send_DATA[9]  = BYTE2(data2);
	Send_DATA[10] = BYTE1(data2);
	Send_DATA[11] = BYTE0(data2);
	
	Send_DATA[12] = BYTE3(data3);
	Send_DATA[13] = BYTE2(data3);
	Send_DATA[14] = BYTE1(data3);
	Send_DATA[15] = BYTE0(data3);
	
	Send_DATA[16] = BYTE3(data4);
	Send_DATA[17] = BYTE2(data4);
	Send_DATA[18] = BYTE1(data4);
	Send_DATA[19] = BYTE0(data4);

	Send_DATA[20] = BYTE3(data5);
	Send_DATA[21] = BYTE2(data5);
	Send_DATA[22] = BYTE1(data5);
	Send_DATA[23] = BYTE0(data5);
	
	Send_DATA[24] = BYTE3(data6);
	Send_DATA[25] = BYTE2(data6);
	Send_DATA[26] = BYTE1(data6);
	Send_DATA[27] = BYTE0(data6);

	sum = 0;
	for(i=0;i<28;i++)
		sum += Send_DATA[i];
	
	Send_DATA[28]=sum;
	USART6_DMA_TxConfig((u32*)Send_DATA,29);
}

