#include <STC8G.H>
#include <intrins.h>
							   
extern void Init_I2C(void);	   
extern void UnInit_I2C(void);
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);	 
extern void OD();
extern void Delay20ms();
extern void Delay100ms();
extern void Delay3000ms();	
extern void DelaySec(unsigned char sec);	

unsigned char ReadDat(unsigned char addr);

void PS_TurnOn()
{
	bit ack = 0;
	Init_I2C();
	Delay20ms();	
	I2CStart();
	ack = I2CWrite(0x46); 
	ack = I2CWrite(0x81); 
	ack = I2CWrite(0x02);
	I2CStop();  

	I2CStart();			 
	ack = I2CWrite(0x46); 
	ack = I2CWrite(0x82); 
	ack = I2CWrite(0x7F);//100mA	  
	//ack = I2CWrite(0x7B);//50mA
	//ack = I2CWrite(0x79);//10mA
	I2CStop(); 	  	
	UnInit_I2C();
				
}
void PS_TurnOff()
{		   
	bit ack = 0;	
	Init_I2C();
	Delay20ms();
	I2CStart();
	ack = I2CWrite(0x46); 
	ack = I2CWrite(0x81); 
	ack = I2CWrite(0x00);
	I2CStop(); 	   
	UnInit_I2C();
}
unsigned int Read_PS_Dat()
{		  
	bit ack;
	unsigned char ret_dat_0=0xFF;  
	unsigned char ret_dat_1=0xFF;  	
	Init_I2C();
	Delay20ms();
	ret_dat_0 = ReadDat(0x8D);	//PS_Data_0
	//I2CStop();	

	Delay100ms();
	ret_dat_1 = ReadDat(0x8E);	//PS_Data_1	  
	UnInit_I2C();
	return ret_dat_1<<8 | ret_dat_0;	 
}

void ALS_TurnOn(unsigned char gain)
{
	/*
	Gain1: 	0b000<<2	0	1~64k	
	Gain2: 	0b001<<2	1	0.5~32k
	Gain4: 	0b010<<2	2	0.25~16k
	Gain8: 	0b011<<2	3	0.125~8k
	Gain48: 0b110<<2	6	0.02~1.3k
	Gain96: 0b111<<2	7	0.01~600   	 
	*/
	bit ack = 0;   
	Init_I2C();
	Delay20ms();	
	//测量模式
	I2CStart();
	ack = I2CWrite(0x46); 
	ack = I2CWrite(0x80); 
	ack = I2CWrite(gain << 2 | 0x01);
	I2CStop();  

	//测量速率
	I2CStart();			 
	ack = I2CWrite(0x46); 
	ack = I2CWrite(0x85); 
	ack = I2CWrite(0x03);
	I2CStop(); 	  		
	UnInit_I2C();
}

void ALS_TurnOff()
{
	bit ack = 0;	 
	Init_I2C();
	Delay20ms();	
	I2CStart();
	ack = I2CWrite(0x46); 
	ack = I2CWrite(0x80); 
	ack = I2CWrite(0x00); 
	I2CStop(); 	
	UnInit_I2C();	
}

unsigned int Read_ALS_CH1_Dat()
{	
	//88 89 8A 8B
	unsigned char ret_dat_ch1_0=0xFF;  
	unsigned char ret_dat_ch1_1=0xFF; 	
	Init_I2C();
	Delay20ms(); 	
	//CH1_0
	ret_dat_ch1_0 = ReadDat(0x88);
	Delay20ms();
	
	//CH1_1;
	ret_dat_ch1_1 = ReadDat(0x89); 
	UnInit_I2C();
	return ret_dat_ch1_1 << 8 | ret_dat_ch1_0;
}
unsigned int Read_ALS_CH0_Dat()
{	
	//88 89 8A 8B
	unsigned char ret_dat_ch0_0=0xFF;  
	unsigned char ret_dat_ch0_1=0xFF; 
	Init_I2C();
	Delay20ms();
	//CH0_0 
	ret_dat_ch0_0 = ReadDat(0x8A); 	
	Delay20ms();	
	//CH0_1	 
	ret_dat_ch0_1 = ReadDat(0x8B);
	/*
	unsigned char ret_dat_ch1_0=0xFF;  
	unsigned char ret_dat_ch1_1=0xFF; 
	unsigned char ret_dat_ch0_0=0xFF;  
	unsigned char ret_dat_ch0_1=0xFF; 
	*/								 
	UnInit_I2C();
	return ret_dat_ch0_1 << 8 | ret_dat_ch0_0;
}

unsigned char ReadStatus()
{
	Delay20ms();
	return ReadDat(0x8C);
}

unsigned char PS_DataReady()
{
	return ReadStatus() & 0x01;
} 
unsigned char ALS_DataReady()
{
	return ReadStatus() ^ 0x80;
} 
unsigned char ReadDat(unsigned char addr)
{
	//写寄存器地址，读取数据  
	bit ack;
	unsigned char dat_ret = 0x00;	
	Init_I2C();
	I2CStart();
	ack = I2CWrite(0x46); 
	ack = I2CWrite(addr); 
	I2CStop();
	I2CStart();
	ack = I2CWrite(0x47);
	dat_ret = I2CReadNAK();
	I2CStop();		 
	UnInit_I2C();
	return dat_ret;
}

