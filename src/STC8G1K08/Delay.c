#include <STC8G.H>
#include <intrins.h>

void Delay20ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 2;
	j = 32;
	k = 60;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 6;
	j = 157;
	k = 59;
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

	i = 29;
	j = 14;
	k = 54;
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