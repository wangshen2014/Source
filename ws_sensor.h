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
#define head  0x40	//����ԭʼ���ݰ���ͷ��ֵ
#define tail  0x0D	//����ԭʼ���ݰ���β��ֵ

#define data_start  1	//ʵ�ʵĸ��ɿ�ͷ�� buf�е�λ��
#define data_end  (number-4)	//ʵ�ʵĸ��ɽ�����buf�е�λ��
#define COM  0x01	//���屾����ַ   //��ֻ�е������ɲ��жϵ�ַ

///���յ�ʵ�ʸ�����hexbuf�е�λ��
/***************************************************************************************************
 * @����hexbuf   | ��Ƭ����ַ | ��־λ  |   xdd   | �����ֽڸ���  |
 *               |  0         |  1      |    2   |  3            |
 ***************************************************************************************************/
#define COM_HEX_RCV  0     //������ַ�� hexbuf�е�λ��
#define FLAG_HEX_RCV 1     //��־��ַ�� hexbuf�е�λ��
#define NET_HEX_RCV  2	//XDD ǰһ��D��hexbuf�е�λ�� ������ ������2
#define VAR_HEX_RCV  3	//XDD ��һ��D��hexbuf�е�λ��
#define NUM_HEX_RCV  4	//Ҫ������ֽڸ�����hexbuf�е�λ��
#define DATA_HEX_RCV  5	//Ҫ����д���������hexbuf�е�λ�� �ӵ������ʼ 
/***************************************************************************************************
 * @����hexbuf   | ��Ƭ����ַ | �����ֽڸ���  |   ���͵����� ....    |У�飨�Լ�д�������ɣ�
 *               |  0         |  1            |    2 .....           |  2+�ֽ���            |
 ***************************************************************************************************/

//#define COM_HEX_SEND  0     //������ַ�� hexbuf�е�λ��
//#define NUM_HEX_SEND  1     //��־��ַ�� hexbuf�е�λ��
//#define DATA_HEX_SEND  2	//XDD ǰһ��D��hexbuf�е�λ�� ������ ������2
//#define CRC_HEX_SEND  (DATA_HEX_SEND+�ֽ���
#define NODE_ID 0X09
#define TEMP 0x10
#define NO 0x11
#define PH 0x12
#define STATUS 0X13


#define MOTOR_WRITE 0x14
#define MOTOR_STATUS 0x15
#define ID_MOTOR 0X16
//һ�ֽ�ʮ��������תΪ����ASCII�ַ�
#define HEX2ASCII(HEX,ASCH,ASCL)  {\
	ASCH=((HEX)&0xF0)>>4;\
	if(ASCH<10)	ASCH+='0';\
	else		ASCH+='7';\
	ASCL=((HEX)&0x0F);\
	if(ASCL<10)	ASCL+='0';\
	else		ASCL+='7';\
    }

//����ASCII�ַ�תΪһ�ֽ�ʮ��������
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
extern uint8 CvtRecBuf(uint8 *asciibuf,uint8 *hexbuf,uint8 number);  //ֱ��ת�����ջ���������������
extern void ReadCvtSendBuf(uint8 *hexbuf,uint8 *sendbuf, char number);
extern void WriteCvtSendBuf(uint8 hex,uint8 *sendbuf);
//extern int8 ReadTemp(void);
#endif