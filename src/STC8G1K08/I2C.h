#ifndef _I2C_H_
#define _I2C_H_

#include "config.h"

void Init_I2C(unsigned char speed);
void I2CStart();
void I2CStop();
unsigned char I2CReadACK();
unsigned char I2CReadNAK();
bit I2CWrite(unsigned char dat);
void UnInit_I2C(void);

#endif