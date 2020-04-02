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
*Description : RS485用于和上位机通讯
*Arguments   : none
*Returns     : none
*Notes       : none
*******************************************************************************************************************
*/	

void RS485_Init(void)
{
	USART3_Init(115200);												        //初始化串口5
	GPIO_InitTypeDef  GPIO_InitStructure;							  //初始化模式设置端口RE
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);			  //使能GPIOC时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;					  //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				  //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					  //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);							  //初始化GPIO
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);								  //拉低，默认为接受模式
}



/*********************************************************************
*							接口函数:发送数据
*参数:BufferSRC:发送数据存放地址
*	  BufferSize:发送数据字节数
**********************************************************************/
void RS485_Send_Data(uint32_t *BufferSRC, uint32_t BufferSize)//UASRT DMA发送设置
{
	//RS485_mode=1;
	DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);
	DMA1_Stream7->NDTR = BufferSize;    		    //配置BUFFER大小
	DMA1_Stream7->M0AR = (uint32_t)BufferSRC;   //配置地址
	DMA1_Stream7->CR |= DMA_SxCR_EN;			      //打开DMA,开始发送
	//delay_ms(1);
	//RS485_mode=0;
	
}






