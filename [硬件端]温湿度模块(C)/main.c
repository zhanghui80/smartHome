/************************************* 
家用空调控制程序代码
OLED显示屏
SCL P0.2
SDA P0.1

DHT温湿度传感器
数据接口 P0.0

红外发射模块


**************************************/
#include "REG51.h"
#include "oled.h"
#include "bmp.h"
#include "DHT111.h"
#include "remote.h"
#include "wifi.h"

/*事先定义函数*/
void showTemperature();
void keyScan();
void airContorl(int tmp2);
void Delay(int i);

int main(void)
{	
	/*温度传感器每隔1S才生效，所以这里加上两个变量控制*/
    u32 i=0,i2=0;
	unsigned char now,temp=0; // 前面那个是当前的温度，后面那个是获取到的温度
	OLED_Init();			//初始化OLED 
	uInit();          //初始化串口
	u2Init();         //初始化串口2
	OLED_Clear();     //清除OLED
	while(1) 
	{	
		/*这个用来显示温度*/
		i++;
		if(i>50000){
			// 这里我们实时判断空调温度是否发送变化了
			temp = getTemp();
			if (temp!=now){
				now = temp;
				AirControl(now);
			}
			i=0;
			i2++;
			if(i2==4)
			{
				i2=0;
				showTemperature();
			}
		}
	}	  
	
}

/*该函数用于显示温度*/
void showTemperature()
{
	int h,l,tmp;
	u32 T[5]={1,1,1,1,1};//前3位为温度（后一位是小数位），后面两位是湿度的单位
	//获取温度参数
	DHT_Collect(&T);
	// wifi模块发送当前数据
	SendString2("$temp,"); 
	// 发送温度数据
	SendData2(T[0]+48);
	SendData2(T[1]+48);
	SendString2(".");
	SendData2(T[2]+48);
	SendString2(",");
	// 发送湿度数据
	SendData2(T[3]+48);
	SendData2(T[4]+48);
	SendString2(",");
	// 发送空调当前的温度
	tmp = getTemp();
	if(tmp){
		SendData2(tmp/10+48);
		SendData2(tmp%10+48);
	} else {
		SendString2("0");
	}
	// 最后一位避免转换出错
	SendString2(",0");
	SendString2("b");
	
	/*这里显示温度*/
	OLED_ShowCHinese(0,0,0);//温
	OLED_ShowCHinese(17,0,2);//度
	OLED_ShowString(34,0,":",16); 
	OLED_ShowChar(43,0,T[0]+48,16);//显示ASCII字符
	OLED_ShowChar(52,0,T[1]+48,16);//显示ASCII字符
	OLED_ShowString(61,0,".",16);
	OLED_ShowChar(70,0,T[2]+48,16);//显示ASCII字符 			
	OLED_ShowCHinese(79,0,3);//温度
	/*这里显示湿度*/
	OLED_ShowCHinese(0,2,1);//湿
	OLED_ShowCHinese(17,2,2);//度
	OLED_ShowString(34,2,":",16); 
	OLED_ShowChar(43,2,T[3]+48,16);//显示ASCII字符
	OLED_ShowChar(52,2,T[4]+48,16);//显示ASCII字符
	OLED_ShowString(61,2,"%",16);
	/*这里显示空调*/
	OLED_ShowCHinese(0,4,4);//温
	OLED_ShowCHinese(17,4,5);//度
	OLED_ShowString(34,4,":",16);
	//温度处理
	if(tmp)
	{
		//先想办法获取空调的高位
		h=tmp/10+48;
		l=tmp%10+48;
		OLED_ShowChar(42,4,h,16);
		OLED_ShowChar(50,4,l,16);
		OLED_ShowCHinese(58,4,3);//温度
	}		
	else 
	{
		OLED_ShowString(42,4,"off    ",16);
	}
}

/*延时函数*/
void Delay(int i)
{
	while(i--);
}


