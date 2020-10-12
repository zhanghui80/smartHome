#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"
#include "Sensor.h"
#include "Infrared.h"
#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

#define SEND_DATA_EVENT 0X01

//#define Drinking 1
#define Curtain  1
//#define Light_D  1
//#define AirConditioning 1

#define Light                   P1_0
#define CurtainOFF              P1_0
#define CurtainON               P1_1
#define DrinkingFountain        P1_2


       

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
  GENERICAPP_CLUSTERID
};


const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int Endpoint;
  GENERICAPP_PROFID,                //  uint16 AppProfId[2];
  GENERICAPP_DEVICEID,              //  u                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            int16 AppDeviceId[2];
  GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  GENERICAPP_FLAGS,       
  0,                                //  int   AppFlags:4;
  (cId_t *)NULL,
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList   //  byte *pAppInClusterList;
};


typedef struct DataPack
{
 unsigned char username[5];                     //用户名：Maker 
 unsigned char password[6];                     //密码：116114
 unsigned char LightStatus[2];
 unsigned char CurtainStatus[2];                    //窗帘设置四档  1 2 3 4 
 unsigned char AirConditioning[4];                  //空调开关及调温
 unsigned char DrinkingFountainStatus[2];
  
}DataPack;


endPointDesc_t GenericApp_epDesc;
byte GenericApp_TaskID;
byte GenericApp_TransID;
devStates_t GenericApp_NwkState;
DataPack AF_DataPack;
unsigned char This_username[] = "Maker";
unsigned char This_password[] = "116114";

void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void GenericApp_SendTheMessage( void );
void DataInit(void);
void HomeInit(unsigned int);
void delay(uint8 );

//int8 readTemp(void);
//static void rxCB(uint8 port,uint8 event);


void GenericApp_Init( uint8 task_id )
{
  halUARTCfg_t uartConfig;
  GenericApp_TaskID                     = task_id;
  GenericApp_NwkState                   = DEV_INIT;
  GenericApp_TransID                    = 0;

  GenericApp_epDesc.endPoint            = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id             = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc          =
         (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &GenericApp_epDesc );
  P1SEL                                &= 0xF8;         //初始化P1_0,P1_1 P1_2为输出状态
  P1DIR                                |= 0x07;
  
  DataInit();
  HomeInit(2);
  /***初始化串口***/
  uartConfig.configured         = TRUE;
  uartConfig.baudRate           = HAL_UART_BR_9600;
  uartConfig.flowControl        = FALSE;
  uartConfig.callBackFunc       = NULL;
  HalUARTOpen (0,&uartConfig);
}


void HomeInit(unsigned int a)            //按照参数对不同家居进行初始化
{
  if(a == 1)                    //灯
  {
    Light = 0;
    //HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
  }
  if(a == 2)                    //窗帘
  {
    CurtainOFF = 1;
    CurtainON  = 0;
   // HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
  }
  if(a == 3)
  {
    DrinkingFountain = 0;
  //  HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
  }
}

void DataInit(void)                     //初始化数据包
{
  for(int i=0;i<5;i++)
  {
    AF_DataPack.username[i] = This_username[i];
  }
  
  for(int j=0;j<6;j++)
  {
    AF_DataPack.password[j] = This_password[j];
  }
  AF_DataPack.LightStatus[0] = '1';
  AF_DataPack.LightStatus[1] = '0';
  AF_DataPack.CurtainStatus[0] = '2';
  AF_DataPack.CurtainStatus[1] = '0';
  AF_DataPack.DrinkingFountainStatus[0] = '4';
  AF_DataPack.DrinkingFountainStatus[1] = '0';
  AF_DataPack.AirConditioning[0] = '3';
  AF_DataPack.AirConditioning[1] = '0';
  AF_DataPack.AirConditioning[2] = '0';
  AF_DataPack.AirConditioning[3] = '0';
  
}

UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
      case AF_INCOMING_MSG_CMD:
        GenericApp_MessageMSGCB(MSGpkt);
        break;
        case ZDO_STATE_CHANGE:
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( GenericApp_NwkState == DEV_END_DEVICE )
          {
                osal_set_event(GenericApp_TaskID,SEND_DATA_EVENT);     
          }
          break;

        default:
          break;
      }
        osal_msg_deallocate( (uint8 *)MSGpkt );
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
      }
      return (events ^ SYS_EVENT_MSG);
    }
    if(events & SEND_DATA_EVENT)
    {
      GenericApp_SendTheMessage();
      osal_start_timerEx(GenericApp_TaskID,SEND_DATA_EVENT,2000);
      return (events ^ SEND_DATA_EVENT);
    }
  return 0;
}

static void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
#ifdef AirConditioning
 unsigned char airConditioning[5];
  int tp;
  
  switch (pkt -> clusterId)
  {
  case GENERICAPP_CLUSTERID:
    osal_memcpy(airConditioning,pkt->cmd.Data,5);
    tp = (airConditioning[3] - '0')*10 + (airConditioning[4] - '0');
    if(airConditioning[2] == '1')
    {
     //Air_conditioner_on(1);
      Air_conditioner_control(tp);
      AF_DataPack.AirConditioning[1] = '1';
      AF_DataPack.AirConditioning[2] = airConditioning[3];
      AF_DataPack.AirConditioning[3] = airConditioning[4];
    
    }
    if(airConditioning[2] == '0')
    {
      Air_conditioner_off(0);
      AF_DataPack.AirConditioning[1] = '0';
    }
  }
#elif Drinking
  //饮水机饮水机饮水机饮水机饮水机饮水机
  unsigned char Drinking_Fountain[6];
  switch (pkt -> clusterId)
  {
  case GENERICAPP_CLUSTERID:
    osal_memcpy(Drinking_Fountain,pkt->cmd.Data,6);
    if(Drinking_Fountain[5] == '1') 
    {
      DrinkingFountain = 1;
      AF_DataPack.DrinkingFountainStatus[1] = '1';
      HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
    }
    if(Drinking_Fountain[5] == '0')
    {
      DrinkingFountain = 0;
      AF_DataPack.DrinkingFountainStatus[1] = '0';
      HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
    }
  }
#elif Curtain
  //窗帘窗帘窗帘窗帘窗帘窗帘窗帘窗帘窗帘窗帘
  unsigned char curtain[2];
  switch (pkt -> clusterId)
  {
  case GENERICAPP_CLUSTERID:
    osal_memcpy(curtain,pkt->cmd.Data,2);
    if(curtain[1] == '0')
    {
      CurtainON = 1;
      AF_DataPack.CurtainStatus[1] = '0';
      HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
    }
    if(curtain[1] == '2')
    {
      CurtainOFF = 0;
      AF_DataPack.CurtainStatus[1] = '2';
      HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
    }
    if(curtain[1] == '1')
    {
      CurtainOFF = 1;
      CurtainON = 0;
      AF_DataPack.CurtainStatus[1] = '1';
      HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
    }
  }
#elif Light_D
  //灯灯灯灯灯灯灯灯灯灯灯灯灯灯灯灯灯灯灯灯灯
  unsigned char light[1];
  switch (pkt -> clusterId)
  {
  case GENERICAPP_CLUSTERID:
    osal_memcpy(light,pkt->cmd.Data,1);
    if(light[0] == '1')
    {
      Light = 0;
      AF_DataPack.LightStatus[1] = '1';
      HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
    }
    if(light[0] == '0')
    {
      Light = 1;
      AF_DataPack.LightStatus[1] = '0';
      HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
    }
  }
#endif
  
}

/*********发送的数据包需要更换**********/
void GenericApp_SendTheMessage( void )
{
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  my_DstAddr.endPoint = GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr = 0X0000;
#ifdef Curtain
  AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                  GENERICAPP_CLUSTERID,
                  sizeof(AF_DataPack.CurtainStatus),            //对应模块发个自对应的数据包中所属的部分
                  (uint8*)&AF_DataPack.CurtainStatus,
                  &GenericApp_TransID,
                  AF_DISCV_ROUTE,
                  AF_DEFAULT_RADIUS );
#elif Drinking
  AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                  GENERICAPP_CLUSTERID,
                  sizeof(AF_DataPack.DrinkingFountainStatus),            //对应模块发个自对应的数据包中所属的部分
                  (uint8*)&AF_DataPack.DrinkingFountainStatus,
                  &GenericApp_TransID,
                  AF_DISCV_ROUTE,
                  AF_DEFAULT_RADIUS );
#elif Light_D
  AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                  GENERICAPP_CLUSTERID,
                  sizeof(AF_DataPack.LightStatus),            //对应模块发个自对应的数据包中所属的部分
                  (uint8*)&AF_DataPack.LightStatus,
                  &GenericApp_TransID,
                  AF_DISCV_ROUTE,
                  AF_DEFAULT_RADIUS );
#elif AirConditioning
   AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                  GENERICAPP_CLUSTERID,
                  sizeof(AF_DataPack.AirConditioning),            //对应模块发个自对应的数据包中所属的部分
                  (uint8*)&AF_DataPack.AirConditioning,
                  &GenericApp_TransID,
                  AF_DISCV_ROUTE,
                  AF_DEFAULT_RADIUS );
#endif
}


void delay(uint8 i)
{
  while(i--);
}
/*static void rxCB(uint8 port,uint8 event)     //port 端口   采用串口‘0’
{
  HalUARTRead(0,uartbuf,2);
  GenericApp_SendTheMessage();
}
*/
