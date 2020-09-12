#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

#define SEND_TO_ALL_EVENT       0X01
#define UART_TX_EVENT          0X0001


const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
  GENERICAPP_CLUSTERID
};

const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int Endpoint;
  GENERICAPP_PROFID,                //  uint16 AppProfId[2];
  GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  GENERICAPP_FLAGS,                 //  int   AppFlags:4;
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList;
  0,
  (cId_t *)NULL
};

  
typedef struct DataPack
{
 unsigned char username[5];                     //用户名：Maker 
 unsigned char password[6];                     //密码：116114
 unsigned char LightStatus;
 unsigned char CurtainStatus;                    //窗帘设置四档  1 2 3 4 
 unsigned char AirConditioning[3];                  //空调开关及调温
 unsigned char DrinkingFountainStatus;
 unsigned char endl; 
}DataPack;

endPointDesc_t GenericApp_epDesc;
byte GenericApp_TaskID; 
byte GenericApp_TransID;

unsigned char *reply;
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void GenericApp_SendTheMessage(void);
void delay(long);
static void rxCB(uint8 port,uint8 event);
void DataProcessing(unsigned char AF_Data[4]);
void DataInit(void);
/***用户自定义变(常)量区***/
unsigned char cache[128];
unsigned char AF_DataPack[7];
DataPack UART_DataPack;
char This_username[] = "Maker";
char This_password[] = "116114";

/**************************/

void GenericApp_Init(byte task_id)
{
  halUARTCfg_t uartConfig;             //串口配置 halUARTCfg_t 为结构体
  GenericApp_TaskID             = task_id;
  GenericApp_TransID            = 0;
  GenericApp_epDesc.endPoint    = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id     = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc  = 
        (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq  = noLatencyReqs;
  afRegister(&GenericApp_epDesc);
  
  DataInit();
  
  uartConfig.configured         = TRUE;
  uartConfig.baudRate           = HAL_UART_BR_115200;
  uartConfig.flowControl        = FALSE;
  uartConfig.callBackFunc       = rxCB;
  HalUARTOpen (0,&uartConfig);
}

void DataInit(void)
{
  for(int i=0;i<5;i++)
  {
    UART_DataPack.username[i] = This_username[i];
  }
  
  for(int j=0;j<6;j++)
  {
    UART_DataPack.password[j] = This_password[j];
  }
  UART_DataPack.LightStatus = '0';
  UART_DataPack.CurtainStatus = '0';
  UART_DataPack.DrinkingFountainStatus = '0';
  UART_DataPack.AirConditioning[0] = '0';
  UART_DataPack.AirConditioning[1] = '0';
  UART_DataPack.AirConditioning[2] = '0';
  UART_DataPack.endl = 'b';
}

UINT16 GenericApp_ProcessEvent( uint8 task_id, uint16 events )
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
          GenericApp_MessageMSGCB( MSGpkt );
          break;
        default: 
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
 

  // Discard unknown events
  return 0;
}



static void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  unsigned char AF_Data[4];
  switch  (pkt -> clusterId)
  {
  case GENERICAPP_CLUSTERID:
    osal_memcpy(&AF_Data,pkt->cmd.Data,sizeof(AF_Data));
    DataProcessing(AF_Data); 
    HalUARTWrite(0,(uint8*)&UART_DataPack,sizeof(UART_DataPack));
   // delay(40000);
   // osal_start48_timerEx(GenericApp_TaskID,UART_RX_CB_EVT,1000);
    
    break;
  }
/*
    
  switch ( pkt->clusterId )
  {
    case GENERICAPP_CLUSTERID:
      osal_memcpy(&temperature,pkt->cmd.Data,sizeof(temperature));
      HalUARTWrite(0,(uint8*)&temperature,sizeof(temperature)-1);
    break;
  }*/
}

void DataProcessing(unsigned char AF_Data[4])                //对终端节点传来的数据进行处理
{
  if(AF_Data[0] == '1')
  {
    UART_DataPack.LightStatus = AF_Data[1];
  }
  if(AF_Data[0] == '2')
  {
    UART_DataPack.CurtainStatus = AF_Data[1];
  }
  if(AF_Data[0] == '3')
  {
    UART_DataPack.AirConditioning[0] = AF_Data[1];
    UART_DataPack.AirConditioning[1] = AF_Data[2];
    UART_DataPack.AirConditioning[2] = AF_Data[3];
  }
  if(AF_Data[0] == '4')
  {
    UART_DataPack.DrinkingFountainStatus = AF_Data[1];
  }
}


static void GenericApp_SendTheMessage(void)
{
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  my_DstAddr.endPoint = GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr = 0XFFFF;
  AF_DataRequest(      &my_DstAddr,
                       &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID,
                       7,
                       AF_DataPack,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS );
}

                                                                                                               
static void rxCB(uint8 port,uint8 event)     //port 端口   采用串口‘0’
{
   HalUARTRead(0,cache,17);
   int flag = 1;
   if(osal_memcmp(cache,"Maker116114",11) && flag == 1)
   {
     for(int i=0;i<7;i++)
     {
        AF_DataPack[i] = cache[i+11];
     }
     GenericApp_SendTheMessage();
     HalLedBlink(HAL_LED_2,0,50,500);
     flag = 0;
   }

}

void delay(long i)
{
  while(i--);
}
