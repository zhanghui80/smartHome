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

/*延时1s的函数*/
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

void DHT_Collect(u32 *T)//温度采集函数
{
	unsigned char i,j;//for循环变量
	unsigned char t;//超时判断
	unsigned char dat8=0;//一次读取的8位数据，需要读5次
	unsigned int CheckValue=0;//校验和
	
	DHT=0;//主机发起始信号
	Delay20ms();//主机拉低总线至少18ms
	DHT=1;//主机拉高总线20~40us
	Delay30us();
	
	t=80;//设置超时等待时间
	while(DHT&&t--);//等待DHT11拉低总线
	if(t==0)//超时
	{
		DHT=1;
	}
	t=250;//设置超时等待时间
	while(!DHT&&t--);//等待DHT11拉高总线
	if(t==0)//超时
	{
		DHT=1;
	}
	t=250;//设置超时等待时间
	while(DHT&&t--);//等待DHT11拉低总线
	if(t==0)//超时
	{
		DHT=1;
	}

	for(j=0;j<5;j++)//5次读取
	{
		for(i=0;i<8;i++)//1次8个位
		{
	//等待50us开始时隙
			t=150;//设置超时等待时间
			while(!DHT&&t--);//等待DHT11拉高总线
			if(t==0)//超时
			{
				DHT=1;
			}
			t=0;//记录时间清零
			while(DHT&&++t);//等待并记录高电平持续时间
			dat8<<=1;
			if(t>30)//高电平持续时间较长(70us)
			dat8+=1;//传输值为1
		}
		Data_Receive[j]=dat8;
	}
	
	Delay30us();//等待DHT11拉低50us
	Delay30us();
	DHT=1;

	for(i=0;i<4;i++)
	{
		CheckValue+=Data_Receive[i];//计算校验值
	}
	if((unsigned char)CheckValue==Data_Receive[4])//比对
	{
		T[0]=Data_Receive[2]/10;
		T[1]=Data_Receive[2]%10;
		T[2]=Data_Receive[3]%10;
		T[3]=Data_Receive[0]/10;
		T[4]=Data_Receive[0]%10;
	}
}

