#include <STC8G.H>
#include <intrins.h>	
#define ADCTIM  (*(unsigned char volatile xdata *)0xfea8)

void ADCInit()
{
    P_SW2 |= 0x80;
    ADCTIM = 0x3F;                              //����ADC�ڲ�ʱ��
    P_SW2 &= 0x7F;

    ADCCFG = 0x2F;                              //����ADCʱ��Ϊϵͳʱ��/2/16
    ADC_CONTR = 0x8F;                           //ʹ��ADCģ��,��ѡ���15ͨ��
}

void ADCDisable()
{
	ADC_CONTR &= 0x7F;
}
unsigned int ADCRead()
{
    unsigned int res;

    ADC_CONTR |= 0x40;                          //����ADת��
    _nop_();
    _nop_();
    while (!(ADC_CONTR & 0x20));                //��ѯADC��ɱ�־
    ADC_CONTR &= ~0x20;                         //����ɱ�־
    res = (ADC_RES << 8) | ADC_RESL;            //��ȡADC���

    return res;
}

unsigned int GetVccVoltage()
{
	unsigned int *BGV;                                       //�ڲ�1.19V�ο��ź�Դֵ�����idata��
	unsigned char i = 0;
	unsigned res = 0,vcc = 0;
	BGV = (int idata *)0xef;
	for (i=0; i<8; i++)
	{
		res += ADCRead();                       //��ȡ8������
	}
	res >>= 3;                                  //ȡƽ��ֵ 	
	vcc = (int)(1024L * *BGV / res);            //(10λADC�㷨)����VREF�ܽŵ�ѹ,����ص�ѹ
	return vcc;
}