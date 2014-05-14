#include <ioCC2530.h>
#include "lcd12864.h"

__code const uint8 ascii_table_5x7[][5];

/**************************************************************************************************
 * @fn      HalLcd_HW_WaitUs
 *
 * @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
 *
 * @param   x us. range[0-65536]
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WaitUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}

uint8 getStrlen(char *p)
{
    int len=0;
    while(*p!='\0')
    {
        p++;
        len++;
    }
    return len;
}

/**************************************************************************************************
 * @fn      halLcd_ConfigIO
 *
 * @brief   Configure IO lines needed for LCD control.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
static void halLcd_ConfigIO(void)
{
    #define cs1    P1_5	
    #define rs     P1_6	
    #define sclk   P1_2     
    #define sid    P0_0
        P0SEL &= 0xFE;           //��P00Ϊ��ͨIO�ڣ�
        P0DIR |= 0x01;           //��P00Ϊ���
        P1SEL &= 0x9b;           //��P12��P15��P16Ϊ��ͨIO��
        P1DIR |= 0x6C;           //��P12��P15��P16 ����Ϊ���
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Control
 *
 * @brief   Write 1 command to the LCD
 *
 * @param   uint8 cmd - command to be written to the LCD
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Control(uint8 cmd)
{
  	unsigned char j;

	cs1 = 0;
        rs = 0;
	
	for (j = 0; j < 8; j++)
	      {
		sclk=0;
                  if(cmd&0x80)  sid = 1;
                else sid = 0;
                HalLcd_HW_WaitUs(1);
		sclk = 1;
		HalLcd_HW_WaitUs(1);
		cmd = cmd << 1;
	      }
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Write
 *
 * @brief   Write 1 byte to the LCD
 *
 * @param   uint8 data - data to be written to the LCD
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Write(uint8 data)
{
  	char j;

	cs1 = 0;
	rs = 1;
	
	for (j = 0; j < 8; j++)
	{
		sclk = 0;
                  if(data&0x80)  sid = 1;
                else sid = 0;
		sclk = 1;
		data = data << 1;
	}
}


/**************************************************************************************************
 * @fn      HalLcd_HW_Clear
 *
 * @brief   Clear the HW LCD
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Clear(void)
{
    	int i,j;
	for (i = 0; i < 9; i++)
	{
          cs1 = 0;
		HalLcd_HW_Control(0xb0+i);
		HalLcd_HW_Control(0x10);
		HalLcd_HW_Control(0x00);
		for (j = 0; j < 132; j++)
		{
			HalLcd_HW_Write(0x00);
		}
	}
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Init
 *
 * @brief   Initilize HW LCD Driver.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Init(void)
{       
    HalLcd_HW_WaitUs(20);
    halLcd_ConfigIO();
        
	HalLcd_HW_Control(0xe2);          //��λ
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x2c);          //��ѹ����1
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0x2e);          //��ѹ����2
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0x2f);          //��ѹ����
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0x23);          //�ֵ��Աȶȣ������÷�Χ20��27
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x81);          //΢���Աȶ�
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x1f); 
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0xa1);
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0xc8);
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0xa0);          //��ɨ��˳�򣺴��ϵ���
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x40);          //��ѹ��·����ָ�����
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0xaf);          //����ʾ 
        HalLcd_HW_Clear(); 
}

/*
����    ����LCD �ı���ʾ����ʵ�к���
����1   line,��Χ:0~7,���ܹ���ʾ����Ϊ1~8�У�Ҳ����lcd�ֲ����ᵽ��page
����2   col,��Χ:0~127,��lcd������������ʾ����ʼλ�ÿ������õ�ÿһ��
*/
void set_ddram_line_col(uint8 line,uint8 col)
{

  uint8 page,coll,coll_l,coll_h;
  page = line;
  coll = col;
  coll_h = coll>>4;
  coll_l = coll&0x0f;
  HalLcd_HW_Control(0xB0+page);
  HalLcd_HW_WaitUs(15); // 15 us
  HalLcd_HW_Control(0x10+coll_h);
  HalLcd_HW_WaitUs(15); // 15 us
  HalLcd_HW_Control(0x00+coll_l);
  HalLcd_HW_WaitUs(15); // 15 us
}

/*
����     ��ʾһ���ֽڵ��ַ������ַ���СΪ��5���㣬��7����
����1    page,��Χ0~7,��8��
����2    column,��Χ0~127
����3    text,Ҫ��ʾ���ַ�����ֵΪascii��
*/
void DisplayByte_5x7(uint8 page,uint8 column,char text)
{
	int j,k;

	if((text>=0x20)&&(text<0x7e)){/*��Ҫ��ʾ������*/
		j=text-0x20;/*Ѱַ��ͨ���ַ���ascii���ҵ�������еĸ��ַ���λ��*/
		set_ddram_line_col(page,column);
		for(k=0;k<5;k++)
		{
			HalLcd_HW_Write(ascii_table_5x7[j][k]);/*��ʾ5x7��ASCII�ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ�����Ϊ����*/
		}
		//������д��0���������һ��������������
		HalLcd_HW_Write(0x00);
		column+=6;
	}else if(text==0x00){/*����Ҫ��ʾ�����ָ��λ��*/
		set_ddram_line_col(page,column);
		for(k=0;k<5;k++){
			HalLcd_HW_Write(0x00); //���ָ�����ַ�λ��
		}
	}

}

/**************************************************************************************************
����    ��ָ�����к���д��һ���ַ�
����1   line����Χ1~8,����ʾ����,ע������ķ�Χ��1~8,������0~7,Ŀ���Ǽ����ϲ�Ĵ���
����2   col����Χ1~LCD_MAX_LINE_LENGTH,����ʾ����,ע�⣬���ｫ128�ȷֳ�LCD_MAX_LINE_LENGTH������ÿ��������ʾһ���ַ�
����3   text����Ҫ��ʾ��ascii�ַ�
 **************************************************************************************************/
/**************************************************************************************************
 * @fn      HalLcd_HW_WriteChar
 *
 * @brief   Write one char to the display
 *
 * @param   uint8 line - line number that the char will be displayed
 *          uint8 col - colum where the char will be displayed
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WriteChar(uint8 line, uint8 col, char text)
{
  //����������������ε�������Լ���
  /*if (col < HAL_LCD_MAX_CHARS)
  {
    SET_DDRAM_ADDR((line - 1) * HAL_LCD_MAX_CHARS + col);
    HalLcd_HW_Write(text);
  }
  else
  {
    return;
  }*/
  
  //�������Ҽӵ�
  	uint8 column = 1+col*6;
	uint8 page = line-1;
	if(col > 21)/*�������ֲ���ʾ*/
		return;
	DisplayByte_5x7(page,column,text);
}
/**************************************************************************************************
����    ��ָ����ָ����д��һ���ַ���
����1   line����Χ1~8
����2   pText������ʾ���ַ���
 **************************************************************************************************/
void HalLcd_HW_WriteLine(uint8 line,uint8 colum, char *pText)
{

     uint8 count; 
     uint8 totalLength = getStrlen( (char *)pText );
     for (count=colum; count<totalLength+colum; count++)
     {
        HalLcd_HW_WriteChar(line,count,*pText);
        pText++;
     }  
            /* Write blank spaces to rest of the line */
  for(count=totalLength+colum; count<21;count++)
  {
    HalLcd_HW_WriteChar(line, count, 0x00);
  }
}



__code const uint8 ascii_table_5x7[][5] = {
  {0x00,0x00,0x00,0x00,0x00},//space
  {0x00,0x00,0x4f,0x00,0x00},//!
  {0x00,0x07,0x00,0x07,0x00},//"
  {0x14,0x7f,0x14,0x7f,0x14},//#
  {0x24,0x2a,0x7f,0x2a,0x12},//$
  {0x23,0x13,0x08,0x64,0x62},//%
  {0x36,0x49,0x55,0x22,0x50},//&
  {0x00,0x05,0x07,0x00,0x00},//]
  {0x00,0x1c,0x22,0x41,0x00},//(
  {0x00,0x41,0x22,0x1c,0x00},//)
  {0x14,0x08,0x3e,0x08,0x14},//*
  {0x08,0x08,0x3e,0x08,0x08},//+
  {0x00,0x50,0x30,0x00,0x00},//,
  {0x08,0x08,0x08,0x08,0x08},//-
  {0x00,0x60,0x60,0x00,0x00},//.
  {0x20,0x10,0x08,0x04,0x02},///
  {0x3e,0x51,0x49,0x45,0x3e},//0
  {0x00,0x42,0x7f,0x40,0x00},//1
  {0x42,0x61,0x51,0x49,0x46},//2
  {0x21,0x41,0x45,0x4b,0x31},//3
  {0x18,0x14,0x12,0x7f,0x10},//4
  {0x27,0x45,0x45,0x45,0x39},//5
  {0x3c,0x4a,0x49,0x49,0x30},//6
  {0x01,0x71,0x09,0x05,0x03},//7
  {0x36,0x49,0x49,0x49,0x36},//8
  {0x06,0x49,0x49,0x29,0x1e},//9
  {0x00,0x36,0x36,0x00,0x00},//:
  {0x00,0x56,0x36,0x00,0x00},//;
  {0x08,0x14,0x22,0x41,0x00},//<
  {0x14,0x14,0x14,0x14,0x14},//=
  {0x00,0x41,0x22,0x14,0x08},//>
  {0x02,0x01,0x51,0x09,0x06},//?
  {0x32,0x49,0x79,0x41,0x3e},//@
  {0x7e,0x11,0x11,0x11,0x7e},//A
  {0x7f,0x49,0x49,0x49,0x36},//B
  {0x3e,0x41,0x41,0x41,0x22},//C
  {0x7f,0x41,0x41,0x22,0x1c},//D
  {0x7f,0x49,0x49,0x49,0x41},//E
  {0x7f,0x09,0x09,0x09,0x01},//F
  {0x3e,0x41,0x49,0x49,0x7a},//G
  {0x7f,0x08,0x08,0x08,0x7f},//H
  {0x00,0x41,0x7f,0x41,0x00},//I
  {0x20,0x40,0x41,0x3f,0x01},//J
  {0x7f,0x08,0x14,0x22,0x41},//K
  {0x7f,0x40,0x40,0x40,0x40},//L
  {0x7f,0x02,0x0c,0x02,0x7f},//M
  {0x7f,0x04,0x08,0x10,0x7f},//N
  {0x3e,0x41,0x41,0x41,0x3e},//O
  {0x7f,0x09,0x09,0x09,0x06},//P
  {0x3e,0x41,0x51,0x21,0x5e},//Q
  {0x7f,0x09,0x19,0x29,0x46},//R
  {0x46,0x49,0x49,0x49,0x31},//S
  {0x01,0x01,0x7f,0x01,0x01},//T
  {0x3f,0x40,0x40,0x40,0x3f},//U
  {0x1f,0x20,0x40,0x20,0x1f},//V
  {0x3f,0x40,0x38,0x40,0x3f},//W
  {0x63,0x14,0x08,0x14,0x63},//X
  {0x07,0x08,0x70,0x08,0x07},//Y
  {0x61,0x51,0x49,0x45,0x43},//Z
  {0x00,0x7f,0x41,0x41,0x00},//[
  {0x02,0x04,0x08,0x10,0x20},// б��
  {0x00,0x41,0x41,0x7f,0x00},//]
  {0x04,0x02,0x01,0x02,0x04},//^
  {0x40,0x40,0x40,0x40,0x40},//_
  {0x01,0x02,0x04,0x00,0x00},//`
  {0x20,0x54,0x54,0x54,0x78},//a
  {0x7f,0x48,0x48,0x48,0x30},//b
  {0x38,0x44,0x44,0x44,0x44},//c
  {0x30,0x48,0x48,0x48,0x7f},//d
  {0x38,0x54,0x54,0x54,0x58},//e
  {0x00,0x08,0x7e,0x09,0x02},//f
  {0x48,0x54,0x54,0x54,0x3c},//g
  {0x7f,0x08,0x08,0x08,0x70},//h
  {0x00,0x00,0x7a,0x00,0x00},//i
  {0x20,0x40,0x40,0x3d,0x00},//j
  {0x7f,0x20,0x28,0x44,0x00},//k
  {0x00,0x41,0x7f,0x40,0x00},//l
  {0x7c,0x04,0x38,0x04,0x7c},//m
  {0x7c,0x08,0x04,0x04,0x78},//n
  {0x38,0x44,0x44,0x44,0x38},//o
  {0x7c,0x14,0x14,0x14,0x08},//p
  {0x08,0x14,0x14,0x14,0x7c},//q
  {0x7c,0x08,0x04,0x04,0x08},//r
  {0x48,0x54,0x54,0x54,0x24},//s
  {0x04,0x04,0x3f,0x44,0x24},//t
  {0x3c,0x40,0x40,0x40,0x3c},//u
  {0x1c,0x20,0x40,0x20,0x1c},//v
  {0x3c,0x40,0x30,0x40,0x3c},//w
  {0x44,0x28,0x10,0x28,0x44},//x
  {0x04,0x48,0x30,0x08,0x04},//y
  {0x44,0x64,0x54,0x4c,0x44},//z
  {0x08,0x36,0x41,0x41,0x00},//{
  {0x00,0x00,0x77,0x00,0x00},//|
  {0x00,0x41,0x41,0x36,0x08},//}
  {0x04,0x02,0x02,0x02,0x01},//~
};