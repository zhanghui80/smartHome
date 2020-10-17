#include "STC15F2K60S2.H"
#include "intrins.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

void InitADC();
void SendData(BYTE dat);
void SendString(char *s);
void Delay(WORD n);
void ShowResult(BYTE ch);
char chagneInt(int a);
BYTE GetADCResult(BYTE ch);

void main()
{

	InitADC();                      //初始化ADC
    while (1)
    {
		Delay(300);
		ShowResult(0);              //显示通道0
    }
}

/*----------------------------
发送ADC结果到PC
----------------------------*/
void ShowResult(BYTE ch)
{
    BYTE d;
		float v;
		char sendh,sendl,sendll;
		SendString("1.");
		d=GetADCResult(ch);//获取通道1的数据
                //这里是使用公式计算出来的ADC的值
		v=(d*5)/256;
}


/*----------------------------
读取ADC结果
----------------------------*/
BYTE GetADCResult(BYTE ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
    _nop_();                        //等待4个NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//等待ADC转换完成
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC

    return ADC_RES;                 //返回ADC结果
}



/*----------------------------
初始化ADC
----------------------------*/
void InitADC()
{
    P1ASF = 0xff;                   //设置P1口为AD口
    ADC_RES = 0;                    //清除结果寄存器
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    Delay(2);                       //ADC上电并延时
}




