#include <STC8G.H>

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
void Init_I2C(void)
{
	OD();
	P_SW2 = 0x80;
	I2CCFG |= 0xFF;	
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

/* ��������ֹͣ�ź� */
void I2CStop()
{
	I2CMSCR = 0x06;
	Wait();	
}
/* I2C����д������dat-��д���ֽڣ�����ֵ-�ӻ�Ӧ��λ��ֵ */
bit I2CWrite(unsigned char dat)
{
    bit ack = 0;  //�����ݴ�Ӧ��λ��ֵ
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
/* I2C���߶������������ͷ�Ӧ���źţ�����ֵ-�������ֽ� */
unsigned char I2CReadNAK()
{
    unsigned char dat;
	dat = RecvData();	
	SendNAK();
    return dat;
}
/* I2C���߶�������������Ӧ���źţ�����ֵ-�������ֽ� */
unsigned char I2CReadACK()
{
    unsigned char dat;
	dat = RecvData();	
	SendACK();
    return dat;
}