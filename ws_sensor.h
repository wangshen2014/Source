#ifndef SENSOR_H
#define SENSOR_H
#include <hal_types.h>
#include "osal.h"
#include "math.h"
#include "OnBoard.h"
/*********************************************************************
 * MACROS
 */
#if defined(WS_KINGVIEW)
  #define COORD_ID 0X00
  #define END_ID 0X02
#define HAL_ADC_DEC_128 0x10
#endif


//#define WS_SENSORTEST 1
#define head  0x40	//接收原始数据包的头的值
#define tail  0x0D	//接收原始数据包的尾的值

#define data_start  1	//实际的负荷开头在 buf中的位置
#define data_end  (number-4)	//实际的负荷结束在buf中的位置
#define COM  0x01	//定义本机地址   //如只有单机，可不判断地址

///接收的实际负荷在hexbuf中的位置
/***************************************************************************************************
 * @接收hexbuf   | 单片机地址 | 标志位  |   xdd   | 请求字节个数  |
 *               |  0         |  1      |    2   |  3            |
 ***************************************************************************************************/
#define COM_HEX_RCV  0     //本机地址在 hexbuf中的位置
#define FLAG_HEX_RCV 1     //标志地址在 hexbuf中的位置
#define NET_HEX_RCV  2	//XDD 前一个D在hexbuf中的位置 第三个 所以是2
#define VAR_HEX_RCV  3	//XDD 后一个D在hexbuf中的位置
#define NUM_HEX_RCV  4	//要请求的字节个数在hexbuf中的位置
#define DATA_HEX_RCV  5	//要请求写入的数据在hexbuf中的位置 从第五个开始 
/***************************************************************************************************
 * @发送hexbuf   | 单片机地址 | 请求字节个数  |   发送的数据 ....    |校验（自己写程序生成）
 *               |  0         |  1            |    2 .....           |  2+字节数            |
 ***************************************************************************************************/

//#define COM_HEX_SEND  0     //本机地址在 hexbuf中的位置
//#define NUM_HEX_SEND  1     //标志地址在 hexbuf中的位置
//#define DATA_HEX_SEND  2	//XDD 前一个D在hexbuf中的位置 第三个 所以是2
//#define CRC_HEX_SEND  (DATA_HEX_SEND+字节数
#define NODE_ID 0X09
#define TEMP 0x10
#define NO 0x11
#define PH 0x12
#define STATUS 0X13


#define MOTOR_WRITE 0x14
#define MOTOR_STATUS 0x15
#define ID_MOTOR 0X16
//一字节十六进制数转为两个ASCII字符
#define HEX2ASCII(HEX,ASCH,ASCL)  {\
	ASCH=((HEX)&0xF0)>>4;\
	if(ASCH<10)	ASCH+='0';\
	else		ASCH+='7';\
	ASCL=((HEX)&0x0F);\
	if(ASCL<10)	ASCL+='0';\
	else		ASCL+='7';\
    }

//两个ASCII字符转为一字节十六进制数
#define ASCII2HEX(HEX,ASCH,ASCL,HIGH,LOW)  {\
	if(ASCH<'A')	HIGH=ASCH-'0';\
	else		HIGH=ASCH-'7';\
	if(ASCL<'A')	LOW=ASCL-'0';\
	else		LOW=ASCL-'7';\
	HEX=(HIGH<<4)|LOW;\
    }

#if defined(WS_DATAACQ)
#define HAL_ADC_DEC_128 0x10    /* Decimate by 128 : 10-bit resolution */
//#define HAL_ADC_DEC_256     0x20    /* Decimate by 256 : 12-bit resolution */
//#define HAL_ADC_DEC_512     0x30    /* Decimate by 512 : 14-bit resolution */
#endif

extern void D4toC(char * c,float d);
extern float C4toD(char * c);
extern uint8 CalXor(uint8 *buf,uint8 start,uint8 end);
extern uint8 CvtRecBuf(uint8 *asciibuf,uint8 *hexbuf,uint8 number);  //直接转换接收缓冲区，不传参数
extern void ReadCvtSendBuf(uint8 *hexbuf,uint8 *sendbuf, char number);
extern void WriteCvtSendBuf(uint8 hex,uint8 *sendbuf);
//extern int8 ReadTemp(void);
#endif