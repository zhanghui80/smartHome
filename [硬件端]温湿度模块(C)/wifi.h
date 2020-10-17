// 该函数用于向wifi模块发送数据
#ifndef __WIFI_H
#define __WIFI_H
void u2Init();
void SendData2(unsigned char dat);
void SendString2(char *s);
unsigned char getTemp();
#endif