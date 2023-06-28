#include <STC15.H>
#include <intrins.h>
/* IR Code Begin */	   
sbit IR_IO = P3^3; 								 
void InitIR();
void SendCode(unsigned char iRCode); 
void Normal_Code(unsigned char *ircode,unsigned char len);	
void Send_Byte(unsigned char dat);
void Lead_Code();
void Stop_Code();
unsigned char sign;		
unsigned char rst;	
unsigned int irCyc;

void IR_Nec(unsigned char ir_code)
{	 
    InitIR();
	SendCode(ir_code);
}
void IR_PS(unsigned int ir_code)
{	 	  
	unsigned char ircode[3]; 	 
    InitIR();
	ircode[0] = 0x01;	 
	ircode[1] = ir_code >> 8; 
	ircode[2] = ir_code & 0xFF;
	Normal_Code(&ircode,3);
}
void IR_ALS(unsigned long ir_code)
{	 
	unsigned char ircode[5];   
    InitIR();	
	ircode[0] = 0x02;	 
	ircode[1] = ir_code >> 24;
	ircode[2] = (ir_code & 0xFF0000) >> 16;
	ircode[3] = (ir_code & 0xFF00) >> 8;
	ircode[4] = ir_code & 0xFF; 
	Normal_Code(&ircode,5);
}
void enTimer0(bit e)
{
	irCyc = 0;
	ET0 = e;
	TR0 = e;	
}
void InitIR()
{			
	//T2定时器
 	//T2L = 0xCF;		//设置定时初始值
	//T2H = 0xE7;		//设置定时初始值
	//Delay3000ms();
	//Delay3000ms();
	rst = 2;
	AUXR |= 0xC0;  //开启定时器0、定时器1的1T速度
	IR_IO = 1;
	//使用定时器中断1产生38K（38.4615K） 
	TMOD = 0x12;		//模式设置：定时器0：自动重装，定时器1：16bit模式
	TL0 = 0x9F;		//设置定时初始值
	TH0 = 0x9F;		//设置定时初始值
	TR0 = 1;
	EA =1;
	ET0 = 0;
}
void SendCode(unsigned char iRCode)
{
	unsigned char irCodes[2] = {0x00,0x00};
	irCodes[1] = iRCode;
	Normal_Code(irCodes,2);
}	
void _1T()		//@11.0592MHz
{
	unsigned char i, j;	 
	_nop_();
	i = 7;
	j = 2;
	do
	{
		while (--j);
	} while (--i);
} 
void Delay_560us()
{  			
	_1T(); 	
}
void Delay_560us_()
{
	while(irCyc<66);	 	   	 
}
void Delay_1_68ms()
{			
	_1T();			
	_1T();			
	_1T();
}

void Delay_4_5ms()
{		  			
	unsigned char i = 8;	
	while(i--)		
	{
		_1T();			
	}
}	  
void Delay_9ms()
{		
	unsigned char i = 16;	
	while(i--)		
	{
		_1T();			
	}
}
void Delay_9ms_()
{
	while(irCyc<1038);
}
void _1T_()		//@11.0592MHz
{
	unsigned char i, j;	 
	_nop_();
	i = 7;
	j = 2;
	do
	{
		while (--j);
	} while (--i);
} 
void Lead_Code()
{		   
	/* L byte*/	
	enTimer0(1);
	Delay_9ms_();
	enTimer0(0);
	IR_IO = 1; 
	Delay_4_5ms();
}
void Stop_Code()
{
  	/* S byte*/			   
	enTimer0(1);   
	Delay_560us_();
	enTimer0(0);
	IR_IO = 1;				
}
void Send_1_Bit()
{
	/* 1 bit */
	enTimer0(1);   
	Delay_560us_();
	enTimer0(0);
	IR_IO = 1;
	Delay_1_68ms();
}
void Send_0_Bit()
{
	/* 0 bit */
	enTimer0(1);  
	Delay_560us_();
	enTimer0(0); 
	IR_IO = 1;
	Delay_560us();
}
void Send_Byte(unsigned char dat)
{
    signed char i;
	for(i=0;i<=7;i++)
	{
		if((1 << i) & dat)
		{
			Send_1_Bit();
		}
		else
		{
			Send_0_Bit();
		}
	}
}
void Normal_Code(unsigned char *ircode,unsigned char len)
{			
	unsigned char i =0;
	Lead_Code();
	for(;i < len;i++)
	{
		Send_Byte(*(ircode + i));
		Send_Byte(~(*(ircode + i)));		
	}
	Stop_Code();
}
void InterruptTimer0() interrupt 1
{
	if(sign % 3)
	{
		IR_IO = 1;
	}
	else
	{
		IR_IO = 0;
	}  
	sign++;
	if(sign > rst)
	{
		sign = 0;
	}
	irCyc++;
}
/* IR Code End */