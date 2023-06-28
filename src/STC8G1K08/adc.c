#include <STC8G.H>
#include <intrins.h>	
#define ADCTIM  (*(unsigned char volatile xdata *)0xfea8)

void ADCInit()
{
    P_SW2 |= 0x80;
    ADCTIM = 0x3F;                              //设置ADC内部时序
    P_SW2 &= 0x7F;

    ADCCFG = 0x2F;                              //设置ADC时钟为系统时钟/2/16
    ADC_CONTR = 0x8F;                           //使能ADC模块,并选择第15通道
}

void ADCDisable()
{
	ADC_CONTR &= 0x7F;
}
unsigned int ADCRead()
{
    unsigned int res;

    ADC_CONTR |= 0x40;                          //启动AD转换
    _nop_();
    _nop_();
    while (!(ADC_CONTR & 0x20));                //查询ADC完成标志
    ADC_CONTR &= ~0x20;                         //清完成标志
    res = (ADC_RES << 8) | ADC_RESL;            //读取ADC结果

    return res;
}

unsigned int GetVccVoltage()
{
	unsigned int *BGV;                                       //内部1.19V参考信号源值存放在idata中
	unsigned char i = 0;
	unsigned res = 0,vcc = 0;
	BGV = (int idata *)0xef;
	for (i=0; i<8; i++)
	{
		res += ADCRead();                       //读取8次数据
	}
	res >>= 3;                                  //取平均值 	
	vcc = (int)(1024L * *BGV / res);            //(10位ADC算法)计算VREF管脚电压,即电池电压
	return vcc;
}