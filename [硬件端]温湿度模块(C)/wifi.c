#include "reg51.h"
#include "intrins.h"
#include "wifi.h"
#include "remote.h"

#define FOSC 11059200L          //ϵͳƵ��
#define BAUD 9600			//���ڲ�����

sfr AUXR  = 0x8e;               //�����Ĵ���
sfr S2CON = 0x9a;               //UART2 ���ƼĴ���
sfr S2BUF = 0x9b;               //UART2 ���ݼĴ���
sfr T2H   = 0xd6;               //��ʱ��2��8λ
sfr T2L   = 0xd7;               //��ʱ��2��8λ
sfr IE2   = 0xaf;               //�жϿ��ƼĴ���2

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

sfr P_SW2   = 0xBA;             //���蹦���л��Ĵ���2
#define S2_S0 0x01              //P_SW2.0
bit busy2;


bit flag = 0; // �����жϵ�ǰ�Ƿ���յ��˿յ�����
int index = 0; // ��ǰ���յ����ݵ�λ��
// ʹ��ȫ�ֱ������洢���յ��Ĵ�������
unsigned char tempData[3] = {0,0,0};

// ����2�ĳ�ʼ��
void u2Init()
{
	//P_SW2������ƴ���2��2��ܽ�֮���л��Ŀ���λ
    P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
//  P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)
	//���������ƴ���2�Ĺ�����ʽ�������������õĺʹ���1һ��
    S2CON = 0x50;               //8λ�ɱ䲨����
	//T2L ��T2H ��Ϊ���ƶ�ʱ��2�ĸ߰�λ�͵Ͱ�λ����Ҫ�������ʵļ���
	T2L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD))>>8;
	//AUXR ��һ�������Ĵ�������Ҫ������ƶ�ʱ��2�Ŀ���
	// ���������� 00010100 T2RΪ1 T2X12Ϊ1
	//01010100
	//T2R�Ƕ�ʱ��2�����п���λ��Ϊ1�ʹ�������ʱ��2����
	//T2X12Ϊ��ʱ��2�ٶȿ��ơ���������Ϊ1����˵����ʱ��2���ٶ�Ϊ��ͳ51��12��������Ƶ
	//01000000 T1x12Ϊ1��ʾ��ʱ��1���ٶ��Ǵ�ͳ8051��12��
    AUXR = AUXR|0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
	//��������2�жϣ����ﲻ��ҪTR2��ΪTR2��AUXR����
    IE2 = 0x01;                 //ʹ�ܴ���2�ж�
	// ����ϵͳ�ж�
    EA = 1;
}

/*----------------------------
UART2 �жϷ������
-----------------------------*/
void Uart2() interrupt 8
{
	unsigned char dat;
    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //���S2RIλ
        dat = S2BUF;             // ��ȡ��������
		// �ж��Ƿ�׼������
		if (flag) {
			// �ж������Ƿ�Խ��
			if (index > 1) {
				// ���������ж�һ�����ݵķ�Χ
				if(tempData[0]<16 || tempData[0]>30) tempData[0] = 0;
				// ע�����ﲻҪֱ�ӿ��ƴ���1�������ݣ�����ɶ���
				//SendData(tempData[0]);
				// ���ǿ�ʼ���ÿյ��¶�
				//AirControl(tempData[0]);
				flag=0;
				index=0;
			} else {	
				// ��������
				tempData[index] = dat;
				index ++ ;
			}
		}
		// �ж��Ƿ���Կ�ʼ��������
		if (dat==0xfd){			// ׼����������
			flag = 1;
		}
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //���S2TIλ
        busy2 = 0;               //��æ��־
    }
}

/*----------------------------
���ʹ�������
----------------------------*/
void SendData2(unsigned char dat)
{
    while (busy2);               //�ȴ�ǰ������ݷ������
    busy2 = 1;
    S2BUF = dat;                //д���ݵ�UART2���ݼĴ���
}

/*----------------------------
�����ַ���
----------------------------*/
void SendString2(char *s)
{
    while (*s)                  //����ַ���������־
    {
        SendData2(*s++);         //���͵�ǰ�ַ�
    }
}


/*��ȡ��ǰ�յ����¶�*/
unsigned char getTemp(){
	return tempData[0];
}