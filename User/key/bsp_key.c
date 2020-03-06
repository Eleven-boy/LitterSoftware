#include "bsp_key.h" 
#include "bsp_SysTick.h"
#include "GPIO.h"


/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */
	
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(KEY1_GPIO_CLK,ENABLE);
	
  /*ѡ�񰴼�������*/
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN|KEY2_PIN|KEY3_PIN; 
  
  /*��������Ϊ����ģʽ*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	
  
  /*�������Ų�����Ҳ������*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	/*������������Ϊ2MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
  /*ʹ������Ľṹ���ʼ������*/
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);   
  
}

u8 Key_Scan(void)
{  

	 static u8 key_up=1;
	 if(key_up&&((KEY1==0)||(KEY2==0)||(KEY3==0)))
	 {
		 delay_ms(10);
		 key_up=0;
		 if(KEY1==0)      return KEY1_PRES;
		 else if(KEY2==0) return KEY2_PRES;
		 else if(KEY3==0) return KEY3_PRES; 
	 }
    else if(KEY1==1&&KEY2==1&&KEY3==1)
		key_up=1;
	  return 0;	
}



/*********************************************END OF FILE**********************/
