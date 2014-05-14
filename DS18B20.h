#ifndef DS18B20_H
#define DS18B20_H

#include "ioCC2530.h"
#include "hal_types.h"

/******************************************************************************
*******************              Commonly used types        *******************
******************************************************************************/
typedef unsigned char       BOOL;

// Data
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
// Signed numbers
typedef signed char         INT8;
//typedef signed short        INT16;
typedef signed long         INT32;
#define HAL_ADC_DEC_128 0x10    /* Decimate by 128 : 10-bit resolution */


#define IN_DQ  P0_1
#define CL_DQ  IN_DQ=0;asm("NOP");asm("NOP")
#define SET_DQ IN_DQ=1;asm("NOP");asm("NOP") 
#define SET_OUT P0DIR|=0x02;asm("NOP");asm("NOP")
#define SET_IN  P0DIR&=~0x02;asm("NOP");asm("NOP")
/*
extern unsigned char id[8];
extern unsigned char sensor_data_value[2];
extern uint8 ch[9];

void Delay_nus(uint16 n) ;
void write_1820(unsigned char x) ; 
unsigned char read_1820(void);  
void init_1820(void) ; 
void get_id(void);



*/
void Hal_HW_WaitUs(uint16 microSecs);
void Delay_ms(uint16 k);
char init_1820(void);
void write_1820(uint16 x);
uint8 read_1820(void);
extern uint16 read_data(void);
void get_id(void);

//void ds18b20_main(void);
//extern void DataChange(uint8 * pSensorValue,uint8 * pChBuf);
#endif

