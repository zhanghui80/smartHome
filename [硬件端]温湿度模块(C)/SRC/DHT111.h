/*****
程序名: DHT111 温度传感器驱动 头文件
作者:游磊 
版本:V 1.0

*****/

#ifndef __DHT111_H
#define __DHT111_H	

sbit DHT=P0^0;//DHT11模块

void Delay30us();
void Delay20ms();
void Delay1s();
void DHT_Collect(unsigned int* T);
#endif 