/***
	空调控制函数
***/
#include "reg51.h"
#include "intrins.h"
#include "remote.h"
#include "aircode.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 11059200L          //系统频率
#define BAUD 9600             //串口波特率

sfr AUXR  = 0x8e;               //辅助寄存器

sfr P_SW1   = 0xA2;             //外设功能切换寄存器1

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

bit busy;


/*串口初始化*/
void uInit()
{
	//ACC 是一个累加器，主要用来存放数据
	ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
	//P_SW1是一个特殊功能的寄存器，用来将串口1进行切换
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
	// 串行控制器SCON为01010000
	//SM0为0 SM1为1 说明工作方式为方式1 8位UART，波特率可变
	//SM2为0 说明不是多机通信
	//REN为1 允许串行接收
	//TB8为0 在方式2或3中表示发送的第9位数据，我们用的是方式1不管
	//RB8为0 在方式2或3中表示接收的第9位数据，我们也不管
	//TI为0 主要用于发送中断请求的标志位
	//RI为0 用于接收中断的标志位
    SCON = 0x50;                //8位可变波特率
	//AUXR是辅助寄存器用来控制定时器的分频
	//01000000 T1x12为1表示定时器1的速度是传统8051的12倍
    AUXR = 0x40;                //定时器1为1T模式
	//TMOD是定时器/计数器的工作模式寄存器，用来控制定时器的操作模式，我们设置的是模式0
    TMOD = 0x00;                //定时器1为模式0(16位自动重载)
	//TL1是定时器的低8位，TH1是定时器的高8位，这个定时器主要是用来控制波特率的
    TL1 = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
	//TR1为1表示定时器1的控制为，如果为1就表示开始启动定时器
    TR1 = 1;                    //定时器1开始启动
	//ES主要是串口中断控制为，如果为1表示允许串口中断
    ES = 1;                     //使能串口中断
	//EA表示CPU总的中断允许位，如果为1，那么就表示开放CPU中断
    EA = 1;
 }


/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                 //清除RI位
        //P0 = SBUF;              //P0显示串口数据
    }
    if (TI)
    {
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    //ACC = dat;                  //获取校验位P (PSW.0)
    busy = 1;
    SBUF = dat;                 //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}


// 空调控制函数，用于向空调发送温度信息
void AirControl(unsigned char tmp)
{
	unsigned char position;
	int i;
	if(tmp>=16 && tmp<=30) position=tmp-15;
	else position = 0;
	for(i=0;i<236;i++){
		SendData(AirData[position][i]);
	}
}

