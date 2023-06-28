#include <STC8G.H>
#include <intrins.h>

//sbit IR_IO = P0^0; 
sbit IR_IO = P1^0;
sbit HB_LED = P3^7;
sbit WORK_LED = P1^1;
sbit BATLOW_LED = P5^5;

unsigned char cnt = 0; //�����ۼӶ�ʱ���жϴ���
//#define I2CDelay()  {_nop_();_nop_();_nop_();_nop_();}
//sbit I2C_SCL = P3^4;
//sbit I2C_SDA = P3^5;
						 
extern void Init_I2C(void);	   
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);	  
extern void IR_Nec(unsigned char iRCode);	//���ʹ��ƿ����ź�
extern void IR_PS(unsigned int ir_code);	  
extern void IR_ALS(unsigned long ir_code);
	  	 	 
extern void Delay20ms();
extern void Delay100ms();
extern void Delay500ms();
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

extern void ADCInit();
extern void ADCDisable();
extern unsigned int GetVccVoltage();

void Delay3000ms();
void PowerModeTest();	

void PS_Data_Get(); 
void PS_Get();


bit I2CAddresing(unsigned char addr);

 

void main()
{		
	bit ack;
	bit TrunOn = 0;
	
	unsigned int Vcc = 0;

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
	unsigned long Trun_On_Top = 0x044803CF;//((unsigned long)5358 << 16 | (unsigned long)8458);
	unsigned long Trun_Off_Bottom = 0x07B20564;//((unsigned long)22432 << 16 | (unsigned long)65529);
	unsigned long Lux = 0x00;
	//STC8G Ĭ�ϸ���̬ģʽ
	P1M0 = 0x00;
	P1M1 = 0x00;  
	P3M0 = 0x00;
	P3M1 = 0x00; 
	P5M0 = 0x00 & 0xCF;
	P5M1 = 0x00 & 0xCF;
	//���ж�
	EA = 1;
	ET1 = 1; 	
	//���ö�ʱ��1���ģʽ�����ڻ��ѵ�Ƭ��������ģʽ�£�
	AUXR &= 0xBF;			//��ʱ��ʱ��12Tģʽ	   
	TMOD &= 0x0F;			//���ö�ʱ��ģʽ
	TMOD |= 0x10;			//���ö�ʱ��ģʽ
	//���Ե͵�ѹ��λ
	//RSTCFG |= 0x41;
	//HB_LED = 0;
	Init_I2C();
	/*
	Gain1: 	0b000<<2	0	1~64k	
	Gain2: 	0b001<<2	1	0.5~32k
	Gain4: 	0b010<<2	2	0.25~16k
	Gain8: 	0b011<<2	3	0.125~8k
	Gain48: 0b110<<2	6	0.02~1.3k
	Gain96: 0b111<<2	7	0.01~600   	 
	*/
	//Delay500ms();//�ϵ�ȴ�ALS������׼���������ֲ�д��С100ms
	//WKTCL = 0xFF;
	//WKTCH = 0x8C;	
	//����ָʾ����һ�� BEGIN
	BATLOW_LED = 0;	
	WORK_LED = 0;
	HB_LED = 0;
	Delay500ms(); //�ϵ�ȴ�ALS������׼���������ֲ�д��С100ms
	BATLOW_LED = 1;	
	WORK_LED = 1;
	HB_LED = 1;
	//����ָʾ����һ�� END
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
					//�ػ�	
					WORK_LED = 0;
					IR_Nec(0x47); 
					WORK_LED = 1;
					TrunOnCnt = 5;
					TrunOffCnt--;
				}
				else if(Lux < Trun_On_Top && TrunOnCnt)
				{
					//���ȵ���һ��ֵ ���� 
					WORK_LED = 0;
					IR_Nec(0x45);
					WORK_LED = 1;
					Delay100ms();
					IR_Nec(0x5E);
					TrunOffCnt = 5;
					TrunOnCnt--;
				}
			}  	 		   
			SleepCnt = 10;
			ALS_TurnOff();
		}										   
 		
		if(!cnt)
		{			   
			if(SleepCnt % 2 == 0)
			{			
				HB_LED = 0;	
				ADCInit(); //��ADCת��������ã���Ҫ�ȴ�1ms
				PS_TurnOn();
				Delay20ms(); 
				HB_LED = 1;
				//������
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
			//P55 = ~P55;
		}
		//50�����ʱ��1�жϣ��ӿ���ģʽ����
		TL1 = 0x00;				//���ö�ʱ��ʼֵ
		TH1 = 0x4C;				//���ö�ʱ��ʼֵ
		TR1 = 1;				//��ʱ��1��ʼ��ʱ	 
		PCON |= 0x01;
	}
	while(1);
}

void InterruptTimer1() interrupt 3
{				 
	//P54 = ~P54;
	cnt--;	    
}