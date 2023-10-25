#include "I2C.h"

void OD()
{
	P1M0 |= 0x30;
	P1M1 |= 0x30;
}
void UnOD()
{
	P1M0 &= 0xCF;
	P1M1 &= 0xCF;	
}
void Init_I2C(unsigned char speed)
{
	OD();
	P_SW2 = 0x80;
	I2CCFG |= (0xC0 | (speed & 0x3F));
}
void UnInit_I2C(void)
{
	UnOD();
	P_SW2 &= 0x7F;	 
}
void Wait(void)
{
	while(!(I2CMSST & 0x40));
	I2CMSST &= ~0x40;
}
void I2CStart()
{
	I2CMSCR = 0x01;
	Wait();	
}

void RecvACK(void)
{
	I2CMSCR = 0x03;
	Wait();
}
void SendACK(void)
{
	I2CMSST = 0x00;
	I2CMSCR = 0x05;
	Wait();		
}
void SendNAK(void)
{ 
	I2CMSST = 0x01;
	I2CMSCR = 0x05;
	Wait();		
}

/* 产生总线停止信号 */
void I2CStop()
{
	I2CMSCR = 0x06;
	Wait();	
}
/* I2C总线写操作，dat-待写入字节，返回值-从机应答位的值 */
bit I2CWrite(unsigned char dat)
{
    bit ack = 0;  //用于暂存应答位的值
    I2CTXD = dat;
	I2CMSCR = 0x02;
	Wait();
	RecvACK();
    return ack;
}

unsigned char RecvData(void)
{
	I2CMSCR = 0x04;
	I2CMSCR = 0x04;
	Wait();
	return I2CRXD;
}
/* I2C总线读操作，并发送非应答信号，返回值-读到的字节 */
unsigned char I2CReadNAK()
{
    unsigned char dat;
	dat = RecvData();	
	SendNAK();
    return dat;
}
/* I2C总线读操作，并发送应答信号，返回值-读到的字节 */
unsigned char I2CReadACK()
{
    unsigned char dat;
	dat = RecvData();	
	SendACK();
    return dat;
}