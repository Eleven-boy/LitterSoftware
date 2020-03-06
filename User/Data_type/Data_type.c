#include "Data_type.h"   

//发送给 BIG_CAR/BIG_CLAW/BURN_POOL  433传感器的指令
unsigned char send_request_to_xxx[3][6] = {{0x00,0x06,0x0F},//BIG_CAR
																					 {0x00,0x04,0x0F},//BIG_CLAW
																					 {0x00,0x05,0x0F} //BURN_POOL
																				   };
/*********************************************END OF FILE**********************/
