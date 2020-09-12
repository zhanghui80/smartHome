#include <ESP8266WiFi.h>
#define AP_SSID "Maker" //这里改成你的wifi名字
#define AP_PSW  "sjzb@1024"//这里改成你的wifi密码
const uint16_t port = 2346;
const char * host = "139.159.153.129"; // ip or dns
WiFiClient client;//创建一个tcp client连接
int wifista=1,connectwifi=0;

//要使用Serial1，请调用Serial.begin(baudrate)。代码如下：
void setup() {
    // 这里开始写初始化代码，只会执行一次
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID,AP_PSW);
    WiFi.setAutoConnect(true);//自动连接
    WiFi.setAutoReconnect(true);//自动重连
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
    }
    Serial.begin(115200);
    connectwifi=1;
}

void loop() {
   //连接端口 
   delay(200);
   //ZigBee发送数据给我
   if (Serial.available()) 
   {
     String data=Serial.readStringUntil('b');
    //这里返回wifi和tcp的状态
    if(data=="wifi") Serial.println(String(connectwifi));
    if(wifista){ Serial.println("Maker116114000000");wifista=0;}
    client.println(data);
  }
  //tcp发送数据给我
  if(client.available())
  {
     String data=client.readStringUntil('b');
     Serial.println(data);
  }
 
  if(!client.connected()){
    if(!client.connect(host, port)){
        delay(1000);
        return;
      }
   }

}
