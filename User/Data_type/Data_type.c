#include "Data_type.h"   

//���͸� BIG_CAR/BIG_CLAW/BURN_POOL  433��������ָ��
unsigned char send_request_to_xxx[3][6] = {{0x00,0x06,0x0F},//BIG_CAR
																					 {0x00,0x04,0x0F},//BIG_CLAW
																					 {0x00,0x05,0x0F} //BURN_POOL
																				   };
Laser laser;
MPU6050 mpu; 
UP_DATA Up_Data;
//POSITION origin;//��ʼλ��
//POSITION target;//Ŀ��λ�� 
/*********************************************END OF FILE**********************/
