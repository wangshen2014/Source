/**************************************************************************************************
  Filename:       GenericApp.h
  Revised:        $Date: 2012-02-12 15:58:41 -0800 (Sun, 12 Feb 2012) $
  Revision:       $Revision: 29216 $

  Description:    This file contains the Generic Application definitions.


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
**************************************************************************************************/

#ifndef GENERICAPP_H
#define GENERICAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "mt_uart.h"
#include "MT.h"
/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define GENERICAPP_ENDPOINT           10

#define GENERICAPP_PROFID             0x0F04
#define GENERICAPP_DEVICEID           0x0001
#define GENERICAPP_DEVICE_VERSION     0
#define GENERICAPP_FLAGS              0

#define GENERICAPP_MAX_CLUSTERS       1
#define GENERICAPP_CLUSTERID          1

// Send Message Timeout
#define GENERICAPP_SEND_MSG_TIMEOUT   5000     // Every 5 seconds

// Application Events (OSAL) - These are bit weighted definitions.
#define GENERICAPP_SEND_MSG_EVT       0x0001
  
#if defined(WS_ORDINARY)
#define GENERICAPP_RUN_TIMEOUT   3000   // Every 2 seconds
#define GENERICAPP_RUN_EVT       0x0004
#endif
  
#if defined(WS_DATAACQ)
#define GENERICAPP_DATAACQ_TIMEOUT   3000     // Every 2 seconds
#define GENERICAPP_DATAACQ_EVT       0x0008 
#endif

#if defined(WS_TOPOLOGY)
typedef struct RFTXBUF
{
  uint8 type[3];
  uint8 myNWK[4];
  uint8 pNWK[4];
} RFTX;
#endif


#if defined(WS_KINGVIEW)
#if defined(WS_COORD)// 协调器
typedef struct value
{
  uint8 nodeid;
  float temp_value;
  float volt1_value;
  float volt2_value; 
} rftx_value;
#endif
#if !defined(WS_COORD)//fei 协调器
typedef struct RFTXBUF
{
  char id[4];
  char temp_value[4];
  char volt1_value[4];
  char volt2_value[4];
} rftx;
#endif
#endif
/*
#if defined(WS_ORDINARY)
typedef struct BAT
{
  uint8 warn[13];//lower battery
  uint8 myNWK[4];
  uint8 pNWK[4];
} BATRY_LOWER;
#endif*/
#if 0
typedef union h 
{
 
#if 0
  uint8 databuf[26];
  struct buf
  {
    uint8 struct_head[2];
    uint8 struct_type[3];
    uint8 myNWK[4];
    uint8 pNWK[4];
    uint8 temp_value[4];
    uint8 volt_value[4];
    uint8 volt2_value[4];
    uint8 struct_tail[1];
  }SENSORBUF;
#endif
  char databuf[8];
  struct buf
  {
    char temp_value[8];
    //uint8 volt_value[4];
    //uint8 volt2_value[4];
  }SENSORBUF;
} SEN_RFTX;
#endif
extern byte GenericApp_TaskID;   // Task ID for internal task/event processing

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */
#if defined(WS_KINGVIEW)
uint8 CalXor(uint8 *buf,uint8 start,uint8 end);
uint8 CvtRecBuf(uint8 *asciibuf,uint8 *hexbuf,uint8 number);  //直接转换接收缓冲区，不传参数
void ReadCvtSendBuf(uint8 *hexbuf,uint8 *sendbuf, char number);
void WriteCvtSendBuf(uint8 hex,uint8 *sendbuf);
//void hex2ascii(uint8 hex,uint8* asch,uint8* ascl);
#endif
/*
 * Task Initialization for the Generic Application
 */
extern void GenericApp_Init( byte task_id );

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GENERICAPP_H */
