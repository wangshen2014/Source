#include "ws_sensor.h"
#include <ioCC2530.h>
#include "hal_adc.h"


#define ADC_REF_115V 0x00
#define ADC_DEC_256  0x20
#define ADC_DEC_64  0x00
#if defined (WS_KINGVIEW)

/***************************************************************************************************
 * @fn      CalXor
 *
 * @brief   计算校验位
 *
 * @param   len  需要校验的个数
 *
 * @return  None
***************************************************************************************************/

uint8 CalXor(uint8 *buf,uint8 start,uint8 end)
{
    uint8 XorVal=0;
    char i;
    for(i=start;i<=end;i++)
    {
        XorVal^=buf[i];
    }
    return XorVal;
}
/***************************************************************************************************
 * @fn      CvtRecBuf
 *
 * @brief   每次转换两个ASCII字符到一个HEX字节
 *
 * @param   len  需要校验的个数
 *
 * @return  None
***************************************************************************************************/

uint8 CvtRecBuf(uint8 *asciibuf,uint8 *hexbuf,uint8 number)  //直接转换接收缓冲区，不传参数
{
    char i;
    uint8 tmp;
    uint8 index=0;
    uint8 high;
    uint8 low;

    for(i=data_start;i<=data_end;i+=2)	//每次转换两个ASCII字符到一个HEX字节
    {
        ASCII2HEX(tmp, asciibuf[i], asciibuf[i+1],high,low);
        hexbuf[index++]=tmp;	//存入其他的缓冲区
    }
    return index;//这个是去掉@ 和0d 后将剩余的ascii每两个转换成一个hex的数量。
    //RecCount=RecCount>>1;	//长度减半
    
}
/***************************************************************************************************
 * @fn      ReadCvtSendBuf
 *
 * @brief   构建发送包  回复读
 *
 * @param   len  需要校验的个数
 *
 * @return  None
***************************************************************************************************/

void ReadCvtSendBuf(uint8 *hexbuf,uint8 *sendbuf, char number)  
{
    char i;
    uint8 tmp1=0;
    uint8 tmp2=0;
    sendbuf[0]=head;//填充头部 '0x40'
    for(i=number;i>0;i--)	//
    {
        HEX2ASCII(hexbuf[i-1], tmp1, tmp2);
        sendbuf[((i-1)<<1)+1]=tmp1;	
        sendbuf[((i-1)<<1)+2]=tmp2;
    }
    number=number<<1;
    tmp1=CalXor(sendbuf,1,number);//计算异或值
    HEX2ASCII(tmp1,sendbuf[number+1],sendbuf[number+2]);
    sendbuf[number+3]=tail;//填充头部 '0x0d'
}
/***************************************************************************************************
 * @fn      WriteCvtSendBuf
 *
 * @brief   构建写数据回复包（正确情况）  
 *
 * @param   len  需要校验的个数
 *
 * @return  None
***************************************************************************************************/

void WriteCvtSendBuf(uint8 hex,uint8 *sendbuf)  
{
    char number=0;
    uint8 tmp1=0;
    uint8 tmp2=0;
    sendbuf[number++]=head;//填充头部 '0x40'
    HEX2ASCII(hex, tmp1, tmp2);
    sendbuf[number++]=tmp1;	
    sendbuf[number++]=tmp2;
    sendbuf[number++]='#';
    sendbuf[number]='#';
    tmp1=CalXor(sendbuf,1,number);//计算异或值
    HEX2ASCII(tmp1,sendbuf[number+1],sendbuf[number+2]);
    sendbuf[number+3]=tail;//填充头部 '0x0d'
}
#endif
/*
in:char* c
   要转化的ASII码字符，应为4个字符。
Return :转换后的浮点数。
*/
#if 0
float C4toD(char * c)
{
	/*BYTE*/char     Hd[30], Jiema[30];
	float   DTc[30];
	float  Decimal = 0;

	osal_memset(Hd, 0, sizeof(Hd));
	osal_memset(Jiema, 0, sizeof(Jiema));
	osal_memset(DTc, 0, sizeof(DTc));

	float   returnflo = 0;
	bool     ShuFU = FALSE, JieFU = FALSE; 

	if((c[7] > 0x40) && (c[7] < 0x47))
		Hd[7] = ((c[7]  - 0x37) & 0x0f);
	else if((c[7] > 0x60) && (c[7] < 0x67))
		Hd[7] = ((c[7]  - 0x57) & 0x0f);
	else 
		Hd[7] = ((c[7]  - 0x30) & 0x0f);

	if((c[6] > 0x40) && (c[6] < 0x47))
		Hd[6] = ((c[6]  - 0x37) & 0x0f);
	else if((c[6] > 0x60) && (c[6] < 0x67))
		Hd[6] = ((c[6]  - 0x57) & 0x0f);
	else 
		Hd[6] = ((c[6]  - 0x30) & 0x0f);

	DTc[2] = (float)(((float)(Hd[6] * 16.0) + (float)(Hd[7])) / 256.0);

	if((c[5] > 0x40) && (c[5] < 0x47))
		Hd[5] = ((c[5]  - 0x37) & 0x0f);
	else if((c[5] > 0x60) && (c[5] < 0x67))
		Hd[5] = ((c[5]  - 0x57) & 0x0f);
	else 
		Hd[5] = ((c[5]  - 0x30) & 0x0f);

	if((c[4] > 0x40) && (c[4] < 0x47))
		Hd[4] = ((c[4]  - 0x37) & 0x0f);
	else if((c[4] > 0x60) && (c[4] < 0x67))
		Hd[4] = ((c[4]  - 0x57) & 0x0f);
	else 
		Hd[4] = ((c[4]  - 0x30) & 0x0f);

	DTc[1] = (float)((((float)(Hd[4] * 16.0) + (float)Hd[5]) + DTc[2]) / 256.0);

	if((c[3] > 0x40) && (c[3] < 0x47))
		Hd[3] = ((c[3]  - 0x37) & 0x0f);
	else if((c[3] > 0x60) && (c[3] < 0x67))
		Hd[3] = ((c[3]  - 0x57) & 0x0f);
	else 
		Hd[3] = ((c[3]  - 0x30) & 0x0f);

	if((c[2] > 0x40) && (c[2] < 0x47))
		Hd[2] = ((c[2]  - 0x37) & 0x0f);
	else if((c[2] > 0x60) && (c[2] < 0x67))
		Hd[2] = ((c[2]  - 0x57) & 0x0f);
	else 
		Hd[2] = ((c[2]  - 0x30) & 0x0f);

	Decimal = (float)(((float)(Hd[2] * 16) + (float)(Hd[3]) + DTc[1])/ 256.0);

	if((c[1] > 0x40) && (c[1] < 0x47))
		Jiema[1] = ((c[1]  - 0x37) & 0x0f);
	else if((c[1] > 0x60) && (c[1] < 0x67))
		Jiema[1] = ((c[1]  - 0x57) & 0x0f);	
	else 
		Jiema[1] = ((c[1]  - 0x30) & 0x0f);

	if((c[0] > 0x40) && (c[0] < 0x47))
		Jiema[0] = ((c[0]  - 0x37) & 0x0f);
	else if((c[0] > 0x60) && (c[0] < 0x67))
		Jiema[0] = ((c[0]  - 0x57) & 0x0f);
	else 
		Jiema[0] = ((c[0]  - 0x30) & 0x0f);


	ShuFU = ((Jiema[0] & 0x08) >> 3) > 0;
	JieFU = ((Jiema[0] & 0x04) >> 2) > 0;

	Jiema[2] = (Jiema[0] & 0x03) * 16 + Jiema[1];
	
	if(JieFU)
		returnflo = (float)pow(2, (-1) * Jiema[2]) * Decimal;
	else
		returnflo = (float)pow(2, Jiema[2]) * Decimal;

	if(ShuFU)
		returnflo = (-1) * returnflo;

	return returnflo;
}
/*
in:char * c:
   存储浮点数转换后的ASCII码字符。
   Float d:
   要转换的浮点数。
Return : 无。
*/
#endif

#if defined(WS_KINGVIEW)
//#if !defined(WS_COORD)
void D4toC(char * c,float d)
{
    /*BYTE*/char    i = 0;
        char Jiema = 0;
	char    inbyte1[30];
	bool    ShuFu = FALSE, JieFu = FALSE;
	int     inbyte2 = 0, inbyte3 = 0, inbyte4 = 0;
	char    afterbyte2[30], afterbyte3[30], afterbyte4[30];
	float   F_afterbyte2 = 0, F_afterbyte3 = 0; 
        float F_afterbyte4 = 0;

	osal_memset(inbyte1, 0x30, sizeof(inbyte1));
	osal_memset(afterbyte2, 0x30, sizeof(afterbyte2));
	osal_memset(afterbyte3, 0x30, sizeof(afterbyte3));
	osal_memset(afterbyte4, 0x30, sizeof(afterbyte4));
	
	inbyte1[10] = 0x0;
	afterbyte2[10] = 0x0;
	afterbyte3[10] = 0x0;
	afterbyte4[10] = 0x0;
	
	if(d == 0)
	{
		for(int j = 0; j < 8; j++)
			c[j] = 0x30;
		return;
	}
	if(d < 0)
	{
		ShuFu = TRUE;
		d = (-1) * d;
	}
	
	while(d > 1)
	{
		d =(float)(d / 2.0);
		i ++;
	}

	while(d <= 0.5)
	{
		JieFu = TRUE;
		d = (float)(d * 2.0);
		i ++;
	}

	if(d == 1)
	{
		for(int j = 2; j < 8; j++)
			c[j] = 0x46;		
	}	
	else
	{	
		inbyte2 = (int)(d * 256);
		F_afterbyte2 = (d * 256) - (int)(d * 256);
		inbyte3 = (int)(F_afterbyte2 * 256);
		F_afterbyte3 = (F_afterbyte2 * 256) - (int)(F_afterbyte2 * 256);
		inbyte4 = (int)(F_afterbyte3 * 256);
		F_afterbyte4 = (F_afterbyte3 * 256) - (int)(F_afterbyte3 * 256);

		_itoa(inbyte2, afterbyte2, 16);
		_itoa(inbyte3, afterbyte3, 16);
		_itoa(inbyte4, afterbyte4, 16);

		if(inbyte2 == 0)
		{
			c[2] = 0x30;
			c[3] = 0x30;
		}
		else if(inbyte2 < 16)
		{
			c[2] = 0x30;
			c[3] = afterbyte2[0];
		}
		else
		{
			c[2] = afterbyte2[0];
			c[3] = afterbyte2[1];
		}
		if(inbyte3 == 0)
		{
			c[4] = 0x30;
			c[5] = 0x30;
		}
		else if(inbyte3 < 16)
		{
			c[4] = 0x30;
			c[5] = afterbyte3[0];
		}
		else
		{
			c[4] = afterbyte3[0];
			c[5] = afterbyte3[1];
		}
		if(inbyte4 == 0)
		{
			c[6] = 0x30;
			c[7] = 0x30;
		}
		else if(inbyte4 < 16)
		{
			c[6] = 0x30;
			c[7] = afterbyte4[0];
		}
		else
		{
			c[6] = afterbyte4[0];
			c[7] = afterbyte4[1];
		}
	}

	if(JieFu)
	{
		if(i > 0x3f)
			i = 0x3f;
	}
	else if(i > 0x32)
		i = 32;
	if(ShuFu)
		i = i | 0x80;
	if(JieFu)
		i = i | 0x40;
	_itoa(i, inbyte1, 16);

	if(inbyte1 == 0)
	{
		c[0] = 0x30;
		c[1] = 0x30;
	}
	else if(i < 16)
	{
		c[0] = 0x30;
		c[1] = inbyte1[0];
	}
	else
	{
		c[0] = inbyte1[0];
		c[1] = inbyte1[1];
	}
	
	for(i = 0; i < 8; i ++)
	{
		if((c[i] > 0x60) && (c[i] < 0x67))
			c[i] = c[i] - 0x20;
	}

	c[8] = 0x00;
}
#endif
//#endif