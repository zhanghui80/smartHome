/*****
������: DHT111 �¶ȴ��������� ͷ�ļ�
����:���� 
�汾:V 1.0

*****/

#ifndef __DHT111_H
#define __DHT111_H	

sbit DHT=P0^0;//DHT11ģ��

void Delay30us();
void Delay20ms();
void Delay1s();
void DHT_Collect(unsigned int* T);
#endif 