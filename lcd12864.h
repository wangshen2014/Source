#ifndef LCD12864_H
#define LCD12864_H

#include "ioCC2530.h"
#include "hal_types.h"
#include "osal.h"

void HalLcd_HW_Init(void);
void HalLcd_HW_WriteLine(uint8 line,uint8 colum,char *pText);
void HalLcd_HW_WaitUs(uint16 microSecs);
uint8 getStrlen(char *p);//»»µô osal_strlen
void HalLcd_HW_Control(uint8 cmd);
void HalLcd_HW_Write(uint8 data);
void HalLcd_HW_Clear(void);
void set_ddram_line_col(uint8 line,uint8 col);
void DisplayByte_5x7(uint8 page,uint8 column,char text);
void HalLcd_HW_WriteChar(uint8 line, uint8 col, char text);

#endif