#include "config.h"
#include "I2C.h"
#include "Delay.h"
#include "adc.h"
#include "IR.h"
#include "ALS_PS.h"

unsigned char cnt = 0; //用于累加定时器中断次数

void Delay3000ms();
void PowerModeTest();	

void PS_Data_Get(); 
void PS_Get();
bit I2CAddresing(unsigned char addr); 
void SetPowerDownTimerReg(unsigned int ms);

void main()
{
	bit ack;
	bit TrunOn = 0;
	
	//进入掉电模式标识
	bit PDModel = 0;
	
	//达到一定计数后转换为掉电模式（红LED闪烁一次为计数点）
	unsigned char ChangeModelCnt = 0;
	
	unsigned int PDF32K = 0;
	
	unsigned int Vcc = 0;

	unsigned char SleepCnt = 0;

	unsigned char TrunOnCnt = 2;  
	unsigned char TrunOffCnt = 2;

	unsigned char ret_dat_0 = 0xFF;  
	unsigned char ret_dat_1 = 0xFF; 
	unsigned char hbCount = 0;
	unsigned long als_ch1 =  0xFF;
	unsigned long als_ch0 =  0xFF; 
	unsigned int ps_dat =  0xFF;
	unsigned char status = 0x00;
	unsigned long Trun_On_Top = 0x044803CF;//((unsigned long)5358 << 16 | (unsigned long)8458);
	unsigned long Trun_Off_Bottom = 0x07B20564;//((unsigned long)22432 << 16 | (unsigned long)65529);
	unsigned long Lux = 0x00;
	//STC8G 默认高阻态模式
	P1M0 = 0x00;
	P1M1 = 0x00;  
	P3M0 = 0x00;
	P3M1 = 0x00; 
	P5M0 = 0x00 & 0xCF;
	P5M1 = 0x00 & 0xCF;
	//打开中断
	EA = 1;
	ET1 = 1;
	//设置定时器1相关模式，用于唤醒单片机（空闲模式下）
	AUXR &= 0xBF;			//定时器时钟12T模式	   
	TMOD &= 0x0F;			//设置定时器模式
	TMOD |= 0x10;			//设置定时器模式
	//测试低电压复位
	//RSTCFG |= 0x41;
	//HB_LED = 0;
	//Init_I2C(0xFF);
	/*
	Gain1: 	0b000<<2	0	1~64k	
	Gain2: 	0b001<<2	1	0.5~32k
	Gain4: 	0b010<<2	2	0.25~16k
	Gain8: 	0b011<<2	3	0.125~8k
	Gain48: 0b110<<2	6	0.02~1.3k
	Gain96: 0b111<<2	7	0.01~600   	 
	*/
	//Delay500ms();//上电等待ALS传感器准备就绪，手册写最小100ms
	//WKTCL = 0xFF;
	//WKTCH = 0x8C;	
	//所有指示灯亮一次 BEGIN
	BATLOW_LED = 0;	
	WORK_LED = 0;
	HB_LED = 0;
	Delay500ms(); //上电等待ALS传感器准备就绪，手册写最小100ms
	BATLOW_LED = 1;	
	WORK_LED = 1;
	HB_LED = 1;
	//所有指示灯亮一次 END
	while(1)
	{
		if(!SleepCnt)	
		{
			ALS_TurnOn(0x07);
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
					WORK_LED = 0;
					IR_Nec(0x47); 
					WORK_LED = 1;
					TrunOnCnt = 2;
					TrunOffCnt--;
				}
				else if(Lux < Trun_On_Top && TrunOnCnt)
				{
					//亮度低于一定值 开机 
					WORK_LED = 0;
					IR_Nec(0x45);
					WORK_LED = 1;
					Delay100ms();
					IR_Nec(0x5E);
					TrunOffCnt = 2;
					TrunOnCnt--;
				}
			}  	 		   
			SleepCnt = 10;
			ALS_TurnOff();
		}
		
		if(!cnt || PDModel)
		{			   
			if(SleepCnt % 2 == 0 || PDModel)
			{
				if(!PDModel)
				{
					//非深度节能模式才计数
					ChangeModelCnt++;
				}
				if(ChangeModelCnt > 3) //红LED闪烁次数后转为深度节能模式
				{
					ChangeModelCnt = 0;//开始进入深度节能模式，置避免下次重新进入，浪费CPU时间和重复计算设置掉电唤醒定时器值
					PDModel = 1;
					/*
					 *进入深度节能模式，开启掉电唤醒计时器.
					 *注意：掉电唤醒定时器寄存器读取时，实际寄存器是WKTCH_CNT和WKTCL_CNT.
					 *STC8手册没有提到，而STC15手册有此提示.
					*/
					SetPowerDownTimerReg(3700);
				}
				HB_LED = 0;	
				ADCInit(); //打开ADC转换，（最好）需要等待1ms
				PS_TurnOn();
				HB_LED = 1;
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
				Vcc = GetVccVoltage();
				ADCDisable();
				if(Vcc < 2100)
				{
					BATLOW_LED = 0;
				}
				else
				{
					BATLOW_LED = 1;
				}
			}
			SleepCnt--;
			cnt = 40;
		}
		if(PDModel)
		{
			PCON &= 0xFE;
			PCON |= 0x02;
			//注意掉电唤醒后先执行几个空操作（猜测：主时钟刚起振，未进入稳定状态）
			_nop_();
			_nop_();
			_nop_();
			_nop_();
		}
		else
		{
			//50毫秒后定时器1中断，从空闲模式唤醒
			TL1 = 0x00;				//设置定时初始值
			TH1 = 0x4C;				//设置定时初始值
			TR1 = 1;				  //定时器1开始计时	
			PCON &= 0xFD; 
			PCON |= 0x01;
		}
	}
	while(1);
}

void InterruptTimer1() interrupt 3
{
	cnt--;
}

unsigned int GetPD32KRealFrequency()
{
	unsigned int *F32K = 0;
	F32K = (unsigned int code *)0x1FF5;
	if(*F32K == 0xFFFF)
	{
		F32K = (unsigned int idata *)0xF8;
	}
	return *F32K;
}

/*根据出厂时记录的掉电定时器时钟频率，计算给定毫秒数掉电定时器寄存器的计数值*/
void SetPowerDownTimerReg(unsigned int ms)
{
	unsigned long fre = (long)GetPD32KRealFrequency();
	unsigned int cnter = 0x00;
	/*
	公式：计数值 = 微秒数 ÷ (10⁶ × 16)
	*/
	cnter = (fre * ms) / 16000 - 1;
	switch(cnter)
	{
		case 0:
			cnter = 1;
		break;
		case 0x7FFF:
			cnter = 0x7FFE;
		break;
		default:
			break;
	}
	WKTCL = cnter & 0xFF;
	WKTCH = (cnter >> 8) | 0x80;
}
