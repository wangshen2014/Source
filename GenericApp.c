/******************************************************************************
  Filename:       GenericApp.c
  Revised:        $Date: 2012-03-07 01:04:58 -0800 (Wed, 07 Mar 2012) $
  Revision:       $Revision: 29656 $

  Description:    Generic Application (no Profile).


  Copyright 2004-2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License"). You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product. Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
******************************************************************************/

/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends "Hello World" to another "Generic"
  application every 5 seconds.  The application will also
  receives "Hello World" packets.

  The "Hello World" messages are sent/received as MSG type message.

  This applications doesn't have a profile, so it handles everything
  directly - itself.

  Key control:
    SW1:
    SW2:  initiates end device binding
    SW3:
    SW4:  initiates a match description request
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "GenericApp.h"
#include "DebugTrace.h"
#include "ws_dataacq.h"
#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
//#include "hal_lcd.h"
#include "lcd12864.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#if defined(WS_KINGVIEW)
#include "ws_sensor.h"
#include "hal_adc.h"
#include "DS18B20.h"
#endif


/*********************************************************************
 * CONSTANTS
 */
#if !defined( GENERIC_APP_PORT )
#define GENERIC_APP_PORT  0
#endif

#if !defined( GENERIC_APP_BAUD )
#define GENERIC_APP_BAUD  HAL_UART_BR_57600
//#define GENERIC_APP_BAUD  HAL_UART_BR_115200
#endif

// When the Rx buf space is less than this threshold, invoke the Rx callback.
#if !defined( GENERIC_APP_THRESH )
#define GENERIC_APP_THRESH  64
#endif

#if !defined( GENERIC_APP_RX_SZ )
#define GENERIC_APP_RX_SZ  128
#endif

#if !defined( GENERIC_APP_TX_SZ )
#define GENERIC_APP_TX_SZ  128
#endif

// Millisecs of idle time after a byte is received before invoking Rx callback.
#if !defined( GENERIC_APP_IDLE )
#define GENERIC_APP_IDLE  6
#endif

// Loopback Rx bytes to Tx for throughput testing.
#if !defined( GENERIC_APP_LOOPBACK )
#define GENERIC_APP_LOOPBACK  FALSE
#endif

// This is the max byte count per OTA message.
#if !defined( GENERIC_APP_TX_MAX )
#define GENERIC_APP_TX_MAX  80
#endif

#define GENERIC_APP_RSP_CNT  4
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// This list should be filled with Application specific Cluster IDs.
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
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in GenericApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t GenericApp_epDesc;

#if defined(WS_COORD)
//rftx_value coord_pack[4];//定义4个  也就是说end最多有4个
rftx_value packet;
//uint8 global_sensor[72]={0};
#endif

/*********************************************************************
 * EXTERNAL VARIABLES
 */
byte GenericApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // GenericApp_Init() is called.

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 GenericApp_TxLen;

devStates_t GenericApp_NwkState;
byte GenericApp_TransID;  // This is the unique message ID (counter)
afAddrType_t GenericApp_DstAddr;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void GenericApp_HandleKeys( byte shift, byte keys );
static void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
static void GenericApp_SendTheMessage( uint8 * );
#if defined(WS_ORDINARY)
//static void GenericApp_SerialMSGCB(mtOSALSerialData_t *pkt);
static void GenericApp_CallBack(uint8 port, uint8 event);
#endif
#if defined(WS_BATTERYLOW)
void WarnBatteryLower(uint8 *data);  
#endif


#if defined(WS_DATAACQ)
//static int8 ReadTemp(void);
static uint8 ReadVoltage(uint8 channel);  //这里有个static的问题，现在不管。等出问题了，在将代码仿照sensordem中的方法
#endif


/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */


/***************************************************************************************************
 *                                          LOCAL FUNCTIONS
 ***************************************************************************************************/


/*********************************************************************
 * @fn      GenericApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void GenericApp_Init( uint8 task_id )
{
  GenericApp_TaskID = task_id;
  halUARTCfg_t uartConfig;
  GenericApp_NwkState = DEV_INIT;
  GenericApp_TransID = 0;

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  GenericApp_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  GenericApp_DstAddr.endPoint = 0;
  GenericApp_DstAddr.addr.shortAddr = 0;
#if 0
#endif

  // Fill out the endpoint description.
  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &GenericApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( GenericApp_TaskID );
  
  uartConfig.configured           = TRUE;              // 2x30 don't care - see uart driver.
  uartConfig.baudRate             = GENERIC_APP_BAUD;
  uartConfig.flowControl          = FALSE;//改动TRUE;//
  uartConfig.flowControlThreshold = GENERIC_APP_THRESH; // 2x30 don't care - see uart driver.
  uartConfig.rx.maxBufSize        = GENERIC_APP_RX_SZ;  // 2x30 don't care - see uart driver.
  uartConfig.tx.maxBufSize        = GENERIC_APP_TX_SZ;  // 2x30 don't care - see uart driver.
  uartConfig.idleTimeout          = GENERIC_APP_IDLE;   // 2x30 don't care - see uart driver.
  uartConfig.intEnable            = TRUE;              // 2x30 don't care - see uart driver.
  uartConfig.callBackFunc         = GenericApp_CallBack;
  HalUARTOpen (GENERIC_APP_PORT, &uartConfig);

  ZDO_RegisterForZDOMsg( GenericApp_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( GenericApp_TaskID, Match_Desc_rsp );

}

/*********************************************************************
 * @fn      GenericApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 GenericApp_ProcessEvent( uint8 task_id, uint16 events )
{

  afAddrType_t My_DstAddr;


  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;

  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          GenericApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );

          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          GenericApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (GenericApp_NwkState == DEV_ZB_COORD)
              || (GenericApp_NwkState == DEV_ROUTER)
              || (GenericApp_NwkState == DEV_END_DEVICE) )
          {
#if defined(WS_ORDINARY)
            osal_start_timerEx( GenericApp_TaskID,
                                GENERICAPP_RUN_EVT,
                                GENERICAPP_RUN_TIMEOUT );
             osal_start_timerEx( GenericApp_TaskID,
                    GENERICAPP_DATAACQ_EVT,
                    GENERICAPP_DATAACQ_TIMEOUT );
#endif//WS_ORDINARY

          }
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


#if defined(WS_ORDINARY)
  if ( events & GENERICAPP_RUN_EVT )
  {
    HalLcd_HW_WriteLine(6,0,"run");
#if defined(WS_COORD)
    char theMessageData[] = "Hello World";
    My_DstAddr.addrMode = afAddrBroadcast;//(afAddrMode_t)AddrNotPresent;
    My_DstAddr.endPoint = GENERICAPP_ENDPOINT;
    My_DstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
    if ( AF_DataRequest( &My_DstAddr, &GenericApp_epDesc,
                         GENERICAPP_CLUSTERID,
                         (byte)osal_strlen( theMessageData ) + 1,
                         (byte *)&theMessageData,
                         &GenericApp_TransID,
                         AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
    {
      //WsUARTWrite(0,"sendko",6);
    }
    else
    {
      //WsUARTWrite(0,"sendfail",8);
    }
#endif
    osal_start_timerEx( GenericApp_TaskID,
                      GENERICAPP_RUN_EVT,
                      GENERICAPP_RUN_TIMEOUT );
    return (events ^ GENERICAPP_RUN_EVT);
  }
#endif
#if defined(WS_DATAACQ)
#if !defined(WS_COORD)//非协调器&&数据采集
  if ( events & GENERICAPP_DATAACQ_EVT )
  {
    rftx end_pack;
    uint16 int_tmp=0;
    float float_tmp;
    static uint8 temp1=0;

    ws_itoa(END_ID,end_pack.id,10);

    
    float_tmp=read_data();//读取温度数据
    int_tmp=temp1++;//(uint16)(float_tmp*100);//将温度数据放大100倍，这样就可以变成整数传输
    if(temp1==100)
    {
      temp1=0;
    }
    ws_itoa(int_tmp,end_pack.temp_value,10);
    int_tmp=temp1++;//ReadVoltage(HAL_ADC_CHN_AIN6);
    if(temp1==100)
    {
      temp1=0;
    }
    ws_itoa(int_tmp,end_pack.volt1_value,10);
    int_tmp=temp1++;//ReadVoltage(HAL_ADC_CHN_AIN7);
    if(temp1==100)
    {
      temp1=0;
    }
    ws_itoa(int_tmp,end_pack.volt2_value,10);
    //WsUARTWrite( 0,end_pack,);//显示协调器发送给自己的消息
   //发送给协调器

    My_DstAddr.addrMode = afAddr16Bit;//(afAddrMode_t)AddrNotPresent;
    My_DstAddr.endPoint = GENERICAPP_ENDPOINT;
    My_DstAddr.addr.shortAddr = 0;
    if ( AF_DataRequest( &My_DstAddr, &GenericApp_epDesc,
                         GENERICAPP_CLUSTERID,
                         16,
                         (uint8 *)&end_pack,
                         &GenericApp_TransID,
                         AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
    {
      //WsUARTWrite(0,"sendko",6);
    }
    else
    {
      //WsUARTWrite(0,"sendfail",8);
    }
    osal_start_timerEx( GenericApp_TaskID,
                        GENERICAPP_DATAACQ_EVT,
                        GENERICAPP_DATAACQ_TIMEOUT );

    // return unprocessed events
    return (events ^ GENERICAPP_DATAACQ_EVT);
  }
#endif //!WS_COORD
#endif//WS_DATAACQ


  // Discard unknown events
  return 0;
}


static void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
}





static void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  char buf[10]={0};
  uint16 rssi;

  switch ( pkt->clusterId )
  {
    case GENERICAPP_CLUSTERID:
    HalLedBlink(HAL_LED_2,2,50,500);
    HalLcd_HW_WriteLine(1,1,"rcvd");
    
    HalLcd_HW_WriteLine(2,1,"LinkQuality");
    _itoa(pkt->LinkQuality,buf,10);
    HalLcd_HW_WriteLine(3,1,buf);
    HalLcd_HW_WriteLine(4,1,"rssi");
    if(pkt->rssi<0)
    {
      rssi=-(pkt->rssi);
      _itoa(rssi,buf,10);
      HalLcd_HW_WriteLine(5,0,"-");
      HalLcd_HW_WriteLine(5,3,buf);
    }
    else
    {
      rssi=(pkt->rssi);
      _itoa(rssi,buf,10);
      HalLcd_HW_WriteLine(5,0,buf);
    }
      break;
  }
}

/*********************************************************************
 * @fn      GenericApp_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
static void GenericApp_SendTheMessage( uint8 *theMessageData)
{
#if defined(WS_COORD)//如果是协调器就广播
  GenericApp_DstAddr.addrMode = afAddrBroadcast;//(afAddrMode_t)AddrNotPresent;
  GenericApp_DstAddr.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_DstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
#endif
#if !defined(WS_COORD)//如果不是协调器就就将自己的信息发送给协调器  有函数可以利用 先不管
  GenericApp_DstAddr.addrMode = afAddr16Bit;//(afAddrMode_t)AddrNotPresent;
  GenericApp_DstAddr.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_DstAddr.addr.shortAddr = 0;
#endif
  if ( AF_DataRequest( &GenericApp_DstAddr, &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID,
                       (byte)osal_strlen( theMessageData ) + 1,//若theMESSAGEDATA 不是以/0结尾的，这个就会出问题了
                       (byte *)theMessageData,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
      //WsUARTWrite(0,"SEND OK",7);

  }
  else
  {
      //WsUARTWrite(0,"FAIL",4);
  }
}

/*********************************************************************
 * @fn      GenericApp_CallBack
 *
 * @brief   Send data OTA.
 *
 * @param   port - UART port.
 * @param   event - the UART port event flag.
 *
 * @return  none
 */
static void GenericApp_CallBack(uint8 port, uint8 event)
{
  (void)port;
  (void)event;  // Intentionally unreferenced parameter
  if ((event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT)) &&
  #if Generic_APP_LOOPBACK
        (GenericApp_TxLen < GENERIC_APP_TX_MAX))
  #else
        !GenericApp_TxLen)
  #endif
        {
#if defined(WS_KINGVIEW)
#if defined(WS_COORD)         
            uint8  buf[25]={0};
            uint8 hexbuf[7]={0};
            uint8 number=0;
            uint8 geshubuf[5]={0};
            uint8 xorvalue;
            uint8 cTmp1;
            uint8 tmp1;
            char index=0;
            uint8 high;
            uint8 low;
            char c[9];
            static float temp_var=4.456;
            static float pressure_var=2.234;
            static float PH_var=8.901;
            static int temp1=0;
            static int pressure1=2;
            static int ph1=3;
            static uint8 flag_motor=0;
            halIntState_t  s;
            number=HalUARTRead (0, buf, 25);
            //ws_itoa(number,geshubuf,10);
            //HalUARTWrite(0, geshubuf, 5);//看看收到几个字节的数据
            if(buf[0]==head && buf[number-1]==tail)//收到完整帧
            {
              //计算校验位
              xorvalue=CalXor(buf,data_start,data_end);//第1位到第10位的校验值
              ASCII2HEX(cTmp1,buf[number-3],buf[number-2],high,low);
              if(cTmp1 == xorvalue)//校验正确
              {
                CvtRecBuf(buf,hexbuf,number);//ascii转换为hex
                //最后一位是0 是读命令
                if(((0x01)&(hexbuf[FLAG_HEX_RCV]))==0)//读
                {//读
                  if(((0x08)&(hexbuf[FLAG_HEX_RCV]))==0)//字节 1byte
                  {
                      index=0;
                      hexbuf[index++]=hexbuf[COM_HEX_RCV];
                      hexbuf[index++]=hexbuf[NUM_HEX_RCV];
                      switch (hexbuf[VAR_HEX_RCV])
                      {//switch
                          case NODE_ID:
                              /*hexbuf[index++]=temp1++;
                              if(temp1==100)
                              {
                                temp1=0;
                              }*/
                              hexbuf[index++]=packet.nodeid;
                              ReadCvtSendBuf(hexbuf,buf,index);//封装完成；
                              HalUARTWrite(0,buf,((index+2)<<1));
                            break;
                          case TEMP:
                              hexbuf[index++]=packet.temp_value;
                              ReadCvtSendBuf(hexbuf,buf,index);//封装完成；
                              HalUARTWrite(0,buf,((index+2)<<1));
                            break;
                          case NO:
                              hexbuf[index++]=packet.volt1_value;
                              ReadCvtSendBuf(hexbuf,buf,index);//封装完成；
                              HalUARTWrite(0,buf,((index+2)<<1));                              break;
                          case PH:
                              hexbuf[index++]=packet.volt2_value;                  ReadCvtSendBuf(hexbuf,buf,index);//封装完成；
                              HalUARTWrite(0,buf,((index+2)<<1));                                break;
                            
                          default ://hexbuf[index++]=0;如果上位机没有收到信号，那么
                              hexbuf[index++]=0;
                              ReadCvtSendBuf(hexbuf,buf,index);//封装完成；
                              HalUARTWrite(0,buf,((index+2)<<1));
                           
                        }//switch
                    
                  }//读字节
                  if(((0x08)&(hexbuf[FLAG_HEX_RCV]))==0x08)//float
                  {
                    switch (hexbuf[VAR_HEX_RCV])
                      {//switch
                          case TEMP:
                            buf[0]=head;
                            HEX2ASCII(hexbuf[COM_HEX_RCV], buf[1],buf[2]);
                            HEX2ASCII(hexbuf[NUM_HEX_RCV], buf[3],buf[4]);
                            if(init_1820())//存在
                            {
                              HAL_ENTER_CRITICAL_SECTION(s);
                              temp_var=23.456;//coord_pack[hexbuf[NET_HEX_RCV]-1].temp_value;
                              HAL_EXIT_CRITICAL_SECTION(s);
                            }
                            else
                            {
                              temp_var=255;//coord_pack[hexbuf[NET_HEX_RCV]-1].temp_value;
                            }
                            D4toC(c,temp_var);
                            osal_memcpy(&buf[5],c,8);
                            
                            tmp1=CalXor(buf,1,12);//计算异或值
                            HEX2ASCII(tmp1, buf[13],buf[14]);
                            buf[15]=tail;//填充头部 '0x0d'
                            HalUARTWrite(0,buf,16);

                            break;
                            
                            
                          case NO:
                              buf[0]=head;
                              HEX2ASCII(hexbuf[COM_HEX_RCV], buf[1],buf[2]);
                              HEX2ASCII(hexbuf[NUM_HEX_RCV], buf[3],buf[4]);
    
                              pressure_var=45.34;//coord_pack[hexbuf[NET_HEX_RCV]-1].volt1_value;
                              D4toC(c,pressure_var);
                              osal_memcpy(&buf[5],c,8);
                              
                              tmp1=CalXor(buf,1,12);//计算异或值
                              HEX2ASCII(tmp1, buf[13],buf[14]);
                              buf[15]=tail;//填充头部 '0x0d'
                              HalUARTWrite(0,buf,16);
                              break;
                          case PH:
                              buf[0]=head;
                              HEX2ASCII(hexbuf[COM_HEX_RCV], buf[1],buf[2]);
                              HEX2ASCII(hexbuf[NUM_HEX_RCV], buf[3],buf[4]);
    
                              PH_var=10.2;//coord_pack[hexbuf[NET_HEX_RCV]-1].volt2_value;
                              D4toC(c,PH_var);
                              osal_memcpy(&buf[5],c,8);
                              
                              tmp1=CalXor(buf,1,12);//计算异或值
                              HEX2ASCII(tmp1, buf[13],buf[14]);
                              buf[15]=tail;//填充头部 '0x0d'
                              HalUARTWrite(0,buf,16);
                              break;
                            
                          default ://hexbuf[index++]=0;如果上位机没有收到信号，那么
                              index=0;
                              hexbuf[index++]=hexbuf[COM_HEX_RCV];
                              hexbuf[index++]=hexbuf[NUM_HEX_RCV];
                              hexbuf[index++]=0;
                              ReadCvtSendBuf(hexbuf,buf,index);//封装完成；
                              HalUARTWrite(0,buf,((index+2)<<1));
                        }//switch
  
                    }//读float

                }//读
                else
                {//写
                  if((((0x08)&(hexbuf[FLAG_HEX_RCV]))==0)&&(hexbuf[NUM_HEX_RCV]==2))//字节 2byte
                  {
                      uint8 motor=0;
                      uint8 endid_motor=0;
                      uint8 sendbuf[10]={0};
                      uint8 buf1[4]={0};
                      uint8 buf2[4]={0};
                      HalLcd_HW_WriteLine(0,1,"xie");
                      switch (hexbuf[VAR_HEX_RCV])
                      {//switch
                          /*case ID_MOTOR:
                              endid_motor=hexbuf[DATA_HEX_RCV];
                              WriteCvtSendBuf(hexbuf[COM_HEX_RCV],buf);//封装完成；
                              HalUARTWrite(0,buf,8);//回复的数据是固定的8个
                              flag_motor=flag_motor | 0x01;
                              HalLcd_HW_WriteLine(0,6,"rcvd id");

                            break;*/
                          case MOTOR_WRITE://操作电机
                              endid_motor=hexbuf[DATA_HEX_RCV];
                              motor=hexbuf[DATA_HEX_RCV+1];
                              WriteCvtSendBuf(hexbuf[COM_HEX_RCV],buf);//封装完成；
                              HalUARTWrite(0,buf,8);//回复的数据是固定的8个
                              //flag_motor=flag_motor | 0x02;
                              HalLcd_HW_WriteLine(1,6,"rcvd idmotor");
                              ws_itoa(endid_motor,buf1,10);//类似0002
                              ws_itoa(motor,buf2,10);//类似0255
                              sendbuf[0]='w';
                              osal_memcpy(sendbuf+1,buf1,4);
                              osal_memcpy((sendbuf+5),buf2,4);
                              sendbuf[9]=NULL;
                              HalLcd_HW_WriteLine(3,0,sendbuf);
                              GenericApp_SendTheMessage(sendbuf);                            
                              break;
                            
                          default :
                              WriteCvtSendBuf(hexbuf[COM_HEX_RCV],buf);//封装完成；
                              HalUARTWrite(0,buf,8);//回复的数据是固定的8个
                                                         
                        }//switch
                      if(flag_motor==3)//如果要写的id和motor 都全了，那么ota出去，先采用广播的方法
                      {

                        flag_motor=0;

                        
                      }
                    }//一个字节
                  }//写
              }//校验正确
            }
          
#endif
#endif       

}
}