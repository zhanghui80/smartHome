#include "reg51.h"
#include "intrins.h"
#include "wifi.h"
#include "remote.h"

#define FOSC 11059200L          //系统频率
#define BAUD 9600			//串口波特率

sfr AUXR  = 0x8e;               //辅助寄存器
sfr S2CON = 0x9a;               //UART2 控制寄存器
sfr S2BUF = 0x9b;               //UART2 数据寄存器
sfr T2H   = 0xd6;               //定时器2高8位
sfr T2L   = 0xd7;               //定时器2低8位
sfr IE2   = 0xaf;               //中断控制寄存器2

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

sfr P_SW2   = 0xBA;             //外设功能切换寄存器2
#define S2_S0 0x01              //P_SW2.0
bit busy2;


bit flag = 0; // 用来判断当前是否接收到了空调数据
int index = 0; // 当前接收到数据的位置
// 使用全局变量来存储接收到的串口数据
unsigned char tempData[3] = {0,0,0};

// 串口2的初始化
void u2Init()
{
	//P_SW2负责控制串口2在2组管脚之间切换的控制位
    P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
//  P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)
	//这个负责控制串口2的工作方式，这里我们配置的和串口1一样
    S2CON = 0x50;               //8位可变波特率
	//T2L 和T2H 分为控制定时器2的高八位和低八位，主要负责波特率的计算
	T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
	//AUXR 是一个辅助寄存器，主要负责控制定时器2的控制
	// 我们这里是 00010100 T2R为1 T2X12为1
	//01010100
	//T2R是定时器2的运行控制位，为1就代表允许定时器2运行
	//T2X12为定时器2速度控制。我们这里为1就是说明定时器2的速度为传统51的12倍，不分频
	//01000000 T1x12为1表示定时器1的速度是传统8051的12倍
    AUXR = AUXR|0x14;                //T2为1T模式, 并启动定时器2
	//开启串口2中断，这里不需要TR2因为TR2在AUXR里面
    IE2 = 0x01;                 //使能串口2中断
	// 开启系统中断
    EA = 1;
}

/*----------------------------
UART2 中断服务程序
-----------------------------*/
void Uart2() interrupt 8
{
	unsigned char dat;
    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //清除S2RI位
        dat = S2BUF;             // 获取串口数据
		// 判断是否准备接收
		if (flag) {
			// 判断数组是否越界
			if (index > 1) {
				// 我们这里判断一下数据的范围
				if(tempData[0]<16 || tempData[0]>30) tempData[0] = 0;
				// 注意这里不要直接控制串口1发送数据，会造成堵塞
				//SendData(tempData[0]);
				// 我们开始设置空调温度
				//AirControl(tempData[0]);
				flag=0;
				index=0;
			} else {	
				// 接收数据
				tempData[index] = dat;
				index ++ ;
			}
		}
		// 判断是否可以开始接收数据
		if (dat==0xfd){			// 准备接收数据
			flag = 1;
		}
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //清除S2TI位
        busy2 = 0;               //清忙标志
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData2(unsigned char dat)
{
    while (busy2);               //等待前面的数据发送完成
    busy2 = 1;
    S2BUF = dat;                //写数据到UART2数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void SendString2(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData2(*s++);         //发送当前字符
    }
}


/*获取当前空调的温度*/
unsigned char getTemp(){
	return tempData[0];
}