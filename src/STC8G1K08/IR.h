#ifndef _IR_H_
#define _IR_H_

#include "config.h"

#define IR_IO P10
void IR_Nec(unsigned char iRCode);	//发送串灯控制信号
void IR_PS(unsigned int ir_code);	  
void IR_ALS(unsigned long ir_code);

#endif