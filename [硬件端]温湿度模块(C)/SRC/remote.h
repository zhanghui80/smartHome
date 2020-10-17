/*
	该函数用于控制空调
*/
#ifndef __REMOTE_H
#define __REMOTE_H


/*下面这些是空调的一些函数*/

void uInit();
void SendData(unsigned char dat);
void SendString(char *s);
void AirControl(unsigned char tmp);


#endif