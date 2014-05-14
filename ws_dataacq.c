#include "ws_dataacq.h"
#include <ioCC2530.h>
/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "ws_dataacq.h"
#include "DebugTrace.h"
#include "GenericApp.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_led.h"
#include "hal_uart.h"

#if defined(WS_KINGVIEW)
#include "ws_sensor.h"
#include "hal_adc.h"
#include "DS18B20.h"
#endif



/*********************************************************************
 * GLOBAL VARIABLES
 */
// This list should be filled with Application specific Cluster IDs.
const cId_t ExampleApp_ClusterList[EXAMPLEAPP_MAX_CLUSTERS] =
{
  EXAMPLEAPP_CLUSTERID
};

const SimpleDescriptionFormat_t ExampleApp_SimpleDesc =
{
  EXAMPLEAPP_ENDPOINT,              //  int Endpoint;
  EXAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  EXAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  EXAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  EXAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  EXAMPLEAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)ExampleApp_ClusterList,  //  byte *pAppInClusterList;
  EXAMPLEAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)ExampleApp_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in ExampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t ExampleApp_epDesc;
byte ExampleApp_TaskID;   // Task ID for internal task/event processing
devStates_t ExampleApp_NwkState;


//byte ExampleApp_TransID;  // This is the unique message ID (counter)

//afAddrType_t ExampleApp_DstAddr;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
//static void ExampleApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
//static void ExampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
//static void ExampleApp_SendTheMessage( uint8 * );

void ExampleApp_Init( uint8 task_id )
{
  ExampleApp_TaskID = task_id;
  //ExampleApp_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  //ExampleApp_DstAddr.endPoint = 0;
  //ExampleApp_DstAddr.addr.shortAddr = 0;

  // Fill out the endpoint description.
  //ExampleApp_epDesc.endPoint = EXAMPLEAPP_ENDPOINT;
  //ExampleApp_epDesc.task_id = &ExampleApp_TaskID;
  //ExampleApp_epDesc.simpleDesc
           // = (SimpleDescriptionFormat_t *)&ExampleApp_SimpleDesc;
  //ExampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  //afRegister( &ExampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  //RegisterForKeys( Example_TaskID );此task不响应按键事件
  
  ZDO_RegisterForZDOMsg( ExampleApp_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( ExampleApp_TaskID, Match_Desc_rsp );

}


/*********************************************************************
 * @fn      ExampleApp_ProcessEvent
 *
 * @brief   Example Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 ExampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
#if defined(WS_DATAACQ)
#if !defined(WS_COORD) //非协调器
  //SEN_RFTX sensor_rftx;
  //uint16 nwk;//临时储存地址
  float temp_tmp;//临时存储温度
  uint8 volt_tmp;//临时存储电压
  afAddrType_t My_DstAddr;
#endif//非 WS_COORD
#endif//WS_DATAACQ

  afIncomingMSGPacket_t *MSGpkt;

  // Data Confirmation message fields
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ExampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          //ExampleApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );

          break;


        case AF_INCOMING_MSG_CMD:
          //ExampleApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          ExampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (ExampleApp_NwkState == DEV_ZB_COORD)
              || (ExampleApp_NwkState == DEV_ROUTER)
              || (ExampleApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending "the" message in a regular interval.
//            osal_start_timerEx( ExampleApp_TaskID,
//                                EXAMPLEAPP_SEND_MSG_EVT,
//                                EXAMPLEAPP_SEND_MSG_TIMEOUT );
#if defined(WS_ORDINARY)
            osal_start_timerEx( ExampleApp_TaskID,
                                EXAMPLEAPP_RUN_EVT,
                                EXAMPLEAPP_RUN_TIMEOUT );
#endif//WS_ORDINARY
#if defined(WS_COORD)//终端&&数据采集开功能开启；
#if defined(WS_DATAACQ)
            osal_start_timerEx( ExampleApp_TaskID,
                                EXAMPLEAPP_DATAACQ_EVT,
                                EXAMPLEAPP_DATAACQ_TIMEOUT );
#endif//!WS_COORD
#endif//WS_END
          }
          break;
        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ExampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in ExampleApp_Init()).
#if defined(WS_ORDINARY)
  if ( events & EXAMPLEAPP_RUN_EVT )
  {
    HalLedSet( HAL_LED_4, HAL_LED_MODE_TOGGLE );
    //WsUARTWrite(0,"sRUN",4);
    // Setup to send message again
    //osal_start_timerEx( ExampleApp_TaskID,
    //                    EXAMPLEAPP_RUN_EVT,
    //                    EXAMPLEAPP_RUN_TIMEOUT );

    // return unprocessed events
    return (events ^ EXAMPLEAPP_RUN_EVT);
  }
#endif
#if defined(WS_DATAACQ)
#if defined(WS_COORD)//非协调器&&数据采集
  if ( events & EXAMPLEAPP_DATAACQ_EVT )
  {
 //   osal_start_timerEx( ExampleApp_TaskID,
 //                       EXAMPLEAPP_DATAACQ_EVT,
 //                       EXAMPLEAPP_DATAACQ_TIMEOUT );
    /*char c[2]={0};
    rftx end_pack;
    uint16 int_tmp=0;
    float float_tmp;

        c[0]=(END3_ID/10)+'0';//id号码
        c[1]=(END3_ID%10)+'0';
        osal_memcpy(end_pack.id,c,2);
        
        int_tmp=read_data();//读取温度数据
//        int_tmp=(uint16)(float_tmp*100);//将温度数据放大100倍，这样就可以变成整数传输
        ws_itoa(int_tmp,end_pack.temp_value,10);
        int_tmp=ReadVoltage(HAL_ADC_CHN_AIN6);
        ws_itoa(int_tmp,end_pack.volt1_value,10);
        int_tmp=ReadVoltage(HAL_ADC_CHN_AIN7);
        ws_itoa(int_tmp,end_pack.volt2_value,10);
        
        My_DstAddr.addrMode = afAddr16Bit;//(afAddrMode_t)AddrNotPresent;
        My_DstAddr.endPoint = EXAMPLEAPP_ENDPOINT;
        My_DstAddr.addr.shortAddr = 0;
        if ( AF_DataRequest( &My_DstAddr, &ExampleApp_epDesc,
                             EXAMPLEAPP_CLUSTERID,
                             14,
                             (uint8 *)&end_pack,
                             &ExampleApp_TransID,
                             AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
        {
          WsUARTWrite(0,"sendko",6);
        }
        else
        {
          WsUARTWrite(0,"sendfail",8);
        }*/
   uint16 i=4500;
    //osal_start_timerEx( GenericApp_TaskID,
     ///         GENERICAPP_RUN_EVT,
      //        1 );// 如果是抢占式的会立即执行
    while(i--)
    {
      P0_1=~P0_1;
      Hal_HW_WaitUs(1000);
    }
    osal_start_timerEx( ExampleApp_TaskID,
                        EXAMPLEAPP_DATAACQ_EVT,
                        EXAMPLEAPP_DATAACQ_TIMEOUT );

    // return unprocessed events
    return (events ^ EXAMPLEAPP_DATAACQ_EVT);
  }
#endif //!WS_COORD
#endif//WS_DATAACQ


  // Discard unknown events
  return 0;
}


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      ExampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *          如果打开了拓扑功能，如果是协调器，只是简单的显示其他节点发送
 *          过来的信息，如果是非协调器，那么，接收到协调器发送的“topology”
 *          指令后将自己的网络地址和父节点的网络地址发送给协调器
 *          
 *          如果打开了数据采集功能，这里协调器器只是显示其他节点发送过来的
 *          温度 电压等信息
 *          
 *          
 * @param   none
 *
 * @return  none
 */


/*********************************************************************
 */
#if defined(WS_KINGVIEW)
#if defined(WS_COORD)

/******************************************************************************
 * @fn          readVoltage
 *
 * @brief       read voltage from ADC
 *
 * @param       none
 *              
 * @return      voltage
 */
static uint8 ReadVoltage(uint8 channel)  //这里有个static的问题，现在不管。等出问题了，在将代码仿照sensordem中的方法
{
  uint16 value;

  // Clear ADC interrupt flag 
  ADCIF = 0;

  ADCCON3 = (HAL_ADC_REF_125V | HAL_ADC_DEC_128 | channel);

  // Wait for the conversion to finish 
  while ( !ADCIF );

  // Get the result
  value = ADCL;
  value |= ((uint16) ADCH) << 8;

  
  // value now contains measurement of Vdd/3
  // 0 indicates 0V and 32767 indicates 1.25V
  // voltage = (value*3*1.25)/32767 volts
  // we will multiply by this by 10 to allow units of 0.1 volts
  value = value >> 6;   // divide first by 2^6
  if(channel == HAL_ADC_CHN_VDD3)
  {
  //  value = (uint16)(value * 37.5); 浮点运算较慢
  //  value = value >> 9;   // ...and later by 2^9...to prevent overflow during multiplication
    value = (uint16)(value * 75);
    value = value >> 10;   // ...and later by 2^9...to prevent overflow during multiplication 
    return value;
  }
  else
  {
    value = (uint16)(value * 75);// 浮点运算较慢
    value = value >> 10;   // ...and later by 2^9...to prevent overflow during multiplication
    return value;
  }
}
#endif
#endif
