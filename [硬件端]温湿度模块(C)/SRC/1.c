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

	InitADC();                      //��ʼ��ADC
    while (1)
    {
		Delay(300);
		ShowResult(0);              //��ʾͨ��0
    }
}

/*----------------------------
����ADC�����PC
----------------------------*/
void ShowResult(BYTE ch)
{
    BYTE d;
		float v;
		char sendh,sendl,sendll;
		SendString("1.");
		d=GetADCResult(ch);//��ȡͨ��1������
                //������ʹ�ù�ʽ���������ADC��ֵ
		v=(d*5)/256;
}


/*----------------------------
��ȡADC���
----------------------------*/
BYTE GetADCResult(BYTE ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
    _nop_();                        //�ȴ�4��NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//�ȴ�ADCת�����
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC

    return ADC_RES;                 //����ADC���
}



/*----------------------------
��ʼ��ADC
----------------------------*/
void InitADC()
{
    P1ASF = 0xff;                   //����P1��ΪAD��
    ADC_RES = 0;                    //�������Ĵ���
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    Delay(2);                       //ADC�ϵ粢��ʱ
}




