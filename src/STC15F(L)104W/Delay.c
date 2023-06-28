#include <STC15.H>
#include <intrins.h>

void Delay20ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 216;
	j = 37;
	do
	{
		while (--j);
	} while (--i);
}
void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k; 
	_nop_();
	_nop_();
	i = 5;
	j = 52;		
	k = 195;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
} 
void Delay500ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 22;
	j = 3;
	k = 227;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay3000ms()
{
	unsigned char i = 30;
	while(i--)
	{
		Delay100ms();
	}
}
void DelaySec(unsigned char sec)
{
	sec *= 10;
	while(sec--)
	{
		Delay100ms();
	}
}