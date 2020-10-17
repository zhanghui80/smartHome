#include <ESP8266WiFi.h>
// 引入延时函数
#include <Ticker.h>
// wifi账号密码
#define AP_SSID "xiaoyou" //这里改成你的wifi名字
#define AP_PSW  "1806040103"//这里改成你的wifi密码

//定义管脚
#define A0 16
#define A1 5
#define A2 4

// 远程服务器ip和端口
const uint16_t port = 8084;
const char * host = "120.79.4.151"; // ip or dns
WiFiClient client;//创建一个tcp client连接 


void setup()
{
  // 设置wifi为连接模式
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID,AP_PSW); // 连接wifi
  WiFi.setAutoConnect(true);//自动连接
  WiFi.setAutoReconnect(true);//自动重连
  // 设置管脚为输出模式
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);  
  // 设置门锁为默认情况
  normal();
  // 设置波特率为115200
  Serial.begin(115200);
  // 直接发送数据，不延迟发送
  client.setNoDelay(false);
  Serial.print("connecting to ");
  Serial.println(host);
  // 不断循环直到连接到wifi
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("."); 
    delay(500);
  }
}

// 定义一个开锁函数
void openLock(){
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
}

// 定义一个关锁函数
void closeLock(){
  digitalWrite(A0,HIGH);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
}

// 定义一个默认函数
void normal(){
  digitalWrite(A0,LOW);
  digitalWrite(A1,HIGH);
  digitalWrite(A2,LOW);
}


void loop()
{
  // 判断tcp是否连接成功
  if(!client.connected()){
    // 没有连接就尝试重新连接
    if(!client.connect(host, port)){
        Serial.println("reconnect"); 
        delay(1000);
      }
  }
  // 判断服务器那边是否有信息发送过来
  if (client.available()) {
        // 接收数据
        String data=client.readStringUntil('m');
        if(data=="0103"){//开锁命令
          // 开锁 
          openLock();
          // 发送成功命令
          client.println("$door,0110,0"); 
          delay(100);
          normal();
          delay(3000);
          // 关锁
          closeLock();
          delay(100);
          normal();
       } else {
        // 回应心跳包
        client.println("$door,0000,0"); 
       }
  }
  delay(100);
}
  
