#include <STC15.H>
#include <intrins.h>

//sbit IR_IO = P0^0; 
sbit IR_IO = P3^3;
sbit HB_Led = P3^1;
 
//#define I2CDelay()  {_nop_();_nop_();_nop_();_nop_();}
//sbit I2C_SCL = P3^4;
//sbit I2C_SDA = P3^5;

extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);	  
extern void IR_Nec(unsigned char iRCode);	//发送串灯控制信号
extern void IR_PS(unsigned int ir_code);	  
extern void IR_ALS(unsigned long ir_code);
	  	 	 
extern void Delay20ms();
extern void Delay100ms();
extern void Delay3000ms();	
extern void DelaySec(unsigned char sec);		
extern void PS_TurnOn();  
extern void PS_TurnOff();  
extern unsigned int Read_PS_Dat();
extern void ALS_TurnOn(unsigned char gain);	
extern void ALS_TurnOff();	   
extern unsigned int Read_ALS_CH1_Dat();
extern unsigned int Read_ALS_CH0_Dat();
extern unsigned char ReadStatus();	 
extern void OD();

void Delay3000ms();
void Delay500ms();
void PowerModeTest();	

void PS_Data_Get(); 
void PS_Get();


bit I2CAddresing(unsigned char addr);

 

void main()
{		
	bit ack;
	bit TrunOn = 0;
	
	unsigned char SleepCnt = 0;

	unsigned char TrunOnCnt = 5;  
	unsigned char TrunOffCnt = 5;

	unsigned char ret_dat_0 = 0xFF;  
	unsigned char ret_dat_1 = 0xFF; 
	unsigned char hbCount = 0;
	unsigned long als_ch1 =  0xFF;
	unsigned long als_ch0 =  0xFF; 
	unsigned int ps_dat =  0xFF;
	unsigned char status = 0x00;
	unsigned long Trun_On_Top = ((unsigned long)5358 << 16 | (unsigned long)8458);
	unsigned long Trun_Off_Bottom = ((unsigned long)22432 << 16 | (unsigned long)65529);
	unsigned long Lux = 0x00;
	OD();	//初始化立即设置I2C开漏模式
	/*
	Gain1: 	0b000<<2	0	1~64k	
	Gain2: 	0b001<<2	1	0.5~32k
	Gain4: 	0b010<<2	2	0.25~16k
	Gain8: 	0b011<<2	3	0.125~8k
	Gain48: 0b110<<2	6	0.02~1.3k
	Gain96: 0b111<<2	7	0.01~600   	 
	*/
	Delay500ms();//上电等待ALS传感器准备就绪，手册写最小100ms
	WKTCL = 0xFF;
	WKTCH = 0x8C;

	while(1)
	{			  	 
	 				 
		if(!SleepCnt)	
		{				   
			ALS_TurnOn(0x07);
			HB_Led = 0;			  
			Delay100ms();
			status = ReadStatus();
			if(status ^ 0x80)
			{
				als_ch1 = Read_ALS_CH1_Dat();
				als_ch0 = Read_ALS_CH0_Dat();
				Lux = ((unsigned long)als_ch1) << 16 | als_ch0;		   
	 
	
				if(Lux > Trun_Off_Bottom && TrunOffCnt)
				{
					//关机	
					IR_Nec(0x47);
					TrunOnCnt = 5;
					TrunOffCnt--;
				}
				else if(Lux < Trun_On_Top && TrunOnCnt)
				{
					//亮度低于一定值 开机
					IR_Nec(0x45);
					Delay100ms();
					IR_Nec(0x5E);
					TrunOffCnt = 5;
					TrunOnCnt--;
				}
			}  	 
	 		HB_Led = 1;
			SleepCnt = 10;
			ALS_TurnOff();
		}										   
 		
		//Delay20ms();

	 	if(SleepCnt % 2 == 0)
		{				 
			PS_TurnOn();
			Delay20ms();
			//距离检查
			ps_dat = Read_PS_Dat();
	 		//IR_PS(ps_dat);
			if(ps_dat > 511)
			{
				TrunOnCnt = 2;
				TrunOffCnt = 2;
				SleepCnt = 1;
			}
			PS_TurnOff();
		}		  
	 	SleepCnt--;
		PCON |= 0x02;
		//break;
	}
	while(1);
}