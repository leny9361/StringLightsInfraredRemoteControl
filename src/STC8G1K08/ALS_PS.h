#ifndef _ALS_PS_H_
#define _ALS_PS_H_

#include "config.h"
#include "Delay.h"
#include "I2C.h"

void PS_TurnOn();
void PS_TurnOff();
unsigned int Read_PS_Dat();
void ALS_TurnOn(unsigned char gain);
void ALS_TurnOff();
unsigned int Read_ALS_CH1_Dat();
unsigned int Read_ALS_CH0_Dat();
unsigned char ReadStatus();

#endif