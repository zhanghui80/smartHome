#include "REG51.h"
#include "DHT111.h"
#include "oled.h"

u8 Data_Receive[5];

void Delay30us()//@12.000MHz
{
	unsigned char i;
	i=87;
	while(--i);
}

void Delay20ms()//@12.000MHz
{
	unsigned char i,j,k;
	i=1;
	j=234;
	k=113;
	do
	{
		do
		{
			while(--k);
		}while(--j);
	}while(--i);
}

/*��ʱ1s�ĺ���*/
void Delay1s()//@12.000MHz
{
	unsigned char i,j,k;
	i=46;
	j=153;
	k=245;
	do
	{
		do
		{
			while(--k);
		}while(--j);
	}while(--i);
}

void DHT_Collect(u32 *T)//�¶Ȳɼ�����
{
	unsigned char i,j;//forѭ������
	unsigned char t;//��ʱ�ж�
	unsigned char dat8=0;//һ�ζ�ȡ��8λ���ݣ���Ҫ��5��
	unsigned int CheckValue=0;//У���
	
	DHT=0;//��������ʼ�ź�
	Delay20ms();//����������������18ms
	DHT=1;//������������20~40us
	Delay30us();
	
	t=80;//���ó�ʱ�ȴ�ʱ��
	while(DHT&&t--);//�ȴ�DHT11��������
	if(t==0)//��ʱ
	{
		DHT=1;
	}
	t=250;//���ó�ʱ�ȴ�ʱ��
	while(!DHT&&t--);//�ȴ�DHT11��������
	if(t==0)//��ʱ
	{
		DHT=1;
	}
	t=250;//���ó�ʱ�ȴ�ʱ��
	while(DHT&&t--);//�ȴ�DHT11��������
	if(t==0)//��ʱ
	{
		DHT=1;
	}

	for(j=0;j<5;j++)//5�ζ�ȡ
	{
		for(i=0;i<8;i++)//1��8��λ
		{
	//�ȴ�50us��ʼʱ϶
			t=150;//���ó�ʱ�ȴ�ʱ��
			while(!DHT&&t--);//�ȴ�DHT11��������
			if(t==0)//��ʱ
			{
				DHT=1;
			}
			t=0;//��¼ʱ������
			while(DHT&&++t);//�ȴ�����¼�ߵ�ƽ����ʱ��
			dat8<<=1;
			if(t>30)//�ߵ�ƽ����ʱ��ϳ�(70us)
			dat8+=1;//����ֵΪ1
		}
		Data_Receive[j]=dat8;
	}
	
	Delay30us();//�ȴ�DHT11����50us
	Delay30us();
	DHT=1;

	for(i=0;i<4;i++)
	{
		CheckValue+=Data_Receive[i];//����У��ֵ
	}
	if((unsigned char)CheckValue==Data_Receive[4])//�ȶ�
	{
		T[0]=Data_Receive[2]/10;
		T[1]=Data_Receive[2]%10;
		T[2]=Data_Receive[3]%10;
		T[3]=Data_Receive[0]/10;
		T[4]=Data_Receive[0]%10;
	}
}

