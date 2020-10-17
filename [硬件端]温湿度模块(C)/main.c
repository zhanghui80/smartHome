/************************************* 
���ÿյ����Ƴ������
OLED��ʾ��
SCL P0.2
SDA P0.1

DHT��ʪ�ȴ�����
���ݽӿ� P0.0

���ⷢ��ģ��


**************************************/
#include "REG51.h"
#include "oled.h"
#include "bmp.h"
#include "DHT111.h"
#include "remote.h"
#include "wifi.h"

/*���ȶ��庯��*/
void showTemperature();
void keyScan();
void airContorl(int tmp2);
void Delay(int i);

int main(void)
{	
	/*�¶ȴ�����ÿ��1S����Ч�������������������������*/
    u32 i=0,i2=0;
	unsigned char now,temp=0; // ǰ���Ǹ��ǵ�ǰ���¶ȣ������Ǹ��ǻ�ȡ�����¶�
	OLED_Init();			//��ʼ��OLED 
	uInit();          //��ʼ������
	u2Init();         //��ʼ������2
	OLED_Clear();     //���OLED
	while(1) 
	{	
		/*���������ʾ�¶�*/
		i++;
		if(i>50000){
			// ��������ʵʱ�жϿյ��¶��Ƿ��ͱ仯��
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

/*�ú���������ʾ�¶�*/
void showTemperature()
{
	int h,l,tmp;
	u32 T[5]={1,1,1,1,1};//ǰ3λΪ�¶ȣ���һλ��С��λ����������λ��ʪ�ȵĵ�λ
	//��ȡ�¶Ȳ���
	DHT_Collect(&T);
	// wifiģ�鷢�͵�ǰ����
	SendString2("$temp,"); 
	// �����¶�����
	SendData2(T[0]+48);
	SendData2(T[1]+48);
	SendString2(".");
	SendData2(T[2]+48);
	SendString2(",");
	// ����ʪ������
	SendData2(T[3]+48);
	SendData2(T[4]+48);
	SendString2(",");
	// ���Ϳյ���ǰ���¶�
	tmp = getTemp();
	if(tmp){
		SendData2(tmp/10+48);
		SendData2(tmp%10+48);
	} else {
		SendString2("0");
	}
	// ���һλ����ת������
	SendString2(",0");
	SendString2("b");
	
	/*������ʾ�¶�*/
	OLED_ShowCHinese(0,0,0);//��
	OLED_ShowCHinese(17,0,2);//��
	OLED_ShowString(34,0,":",16); 
	OLED_ShowChar(43,0,T[0]+48,16);//��ʾASCII�ַ�
	OLED_ShowChar(52,0,T[1]+48,16);//��ʾASCII�ַ�
	OLED_ShowString(61,0,".",16);
	OLED_ShowChar(70,0,T[2]+48,16);//��ʾASCII�ַ� 			
	OLED_ShowCHinese(79,0,3);//�¶�
	/*������ʾʪ��*/
	OLED_ShowCHinese(0,2,1);//ʪ
	OLED_ShowCHinese(17,2,2);//��
	OLED_ShowString(34,2,":",16); 
	OLED_ShowChar(43,2,T[3]+48,16);//��ʾASCII�ַ�
	OLED_ShowChar(52,2,T[4]+48,16);//��ʾASCII�ַ�
	OLED_ShowString(61,2,"%",16);
	/*������ʾ�յ�*/
	OLED_ShowCHinese(0,4,4);//��
	OLED_ShowCHinese(17,4,5);//��
	OLED_ShowString(34,4,":",16);
	//�¶ȴ���
	if(tmp)
	{
		//����취��ȡ�յ��ĸ�λ
		h=tmp/10+48;
		l=tmp%10+48;
		OLED_ShowChar(42,4,h,16);
		OLED_ShowChar(50,4,l,16);
		OLED_ShowCHinese(58,4,3);//�¶�
	}		
	else 
	{
		OLED_ShowString(42,4,"off    ",16);
	}
}

/*��ʱ����*/
void Delay(int i)
{
	while(i--);
}


