/**************************************************************************************************
  Filename:       ExampleApp.h
  Revised:        $Date: 2012-02-12 15:58:41 -0800 (Sun, 12 Feb 2012) $
  Revision:       $Revision: 29216 $

  Description:    This file contains the Example Application definitions.


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
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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

#ifndef EXAMPLEAPP_H
#define EXAMPLEAPP_H

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
#define EXAMPLEAPP_ENDPOINT           11

#define EXAMPLEAPP_PROFID             0x0F03
#define EXAMPLEAPP_DEVICEID           0x0002
#define EXAMPLEAPP_DEVICE_VERSION     0
#define EXAMPLEAPP_FLAGS              0

#define EXAMPLEAPP_MAX_CLUSTERS       1
#define EXAMPLEAPP_CLUSTERID          1

#if defined(WS_ORDINARY)
#define EXAMPLEAPP_RUN_TIMEOUT   2000     // Every 2 seconds
#define EXAMPLEAPP_RUN_EVT       0x0010
#endif
  
#if defined(WS_DATAACQ)
#define EXAMPLEAPP_DATAACQ_TIMEOUT   5000     // Every 1 seconds
#define EXAMPLEAPP_DATAACQ_EVT       0x0008 
//extern byte ExampleApp_TaskID;   // Task ID for internal task/event processing
#endif
  






/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */
/*
 * Task Initialization for the Example Application
 */
extern void ExampleApp_Init( byte task_id );

/*
 * Task Event Processor for the Example Application
 */
extern UINT16 ExampleApp_ProcessEvent( byte task_id, UINT16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLEAPP_H */
