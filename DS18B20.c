#include "DS18B20.h"
#include "OnBoard.h"
uint8 id[8];
uint8 sensor_data_value[2];
uint8 ch[8];
uint8 flag; 


/**************************************************************************************************
 * @fn      HalLcd_HW_WaitUs
 *
 * @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
 *
 * @param   x us. range[0-65536]
 *
 * @return  None
 **************************************************************************************************/
#pragma optimize=none
void Hal_HW_WaitUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 3 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop");
  }
}
char init_1820(void) 
{
  char ds18b20_cunzai;
  SET_OUT;//ʹP11Ϊ���״̬ 
  SET_DQ;//���1
  Hal_HW_WaitUs(7);
  CL_DQ; //���0
  Hal_HW_WaitUs(530);//����һ��ʱ�� 
  
  SET_DQ;//ʹP11Ϊ�ߣ�  
  SET_IN;//ʹP11��Ϊ����״̬���ȴ�18b20�Ĵ��ڵ�������� 
  Hal_HW_WaitUs(44); //ʹP11Ϊ1��ȴ�15-60us������ȴ�44us
  if( IN_DQ == 0)
    ds18b20_cunzai = 1;
      else
  ds18b20_cunzai = 0;//�ȴ��ظ� 
  Hal_HW_WaitUs(150);//�ظ��ĵ͵�ƽ��60��240us  
  SET_OUT; 
  SET_DQ;//�ص���ʼDQ=1��
  return ds18b20_cunzai;
}
void write_1820(uint16 x)  
{  
  uint8 m;    
  SET_OUT;
  for(m=0;m<8;m++)  
  {  
     CL_DQ;
     Hal_HW_WaitUs(1);//��ʱ��ͼ��������ʱ1us���Ų���д"ʱ��϶" 
     if(x&(1<<m))    //д���ݣ��ӵ�λ��ʼ  
     {
       SET_DQ; 
     }
     else  
     {
       CL_DQ;  
     }
     Hal_HW_WaitUs(40);   //15~60us  
     SET_DQ;  
  }
  SET_DQ;  
} 


uint8 read_1820(void)  
{   
  uint8 temp,k,n;    
  temp=0;  
  for(n=0;n<8;n++)  
  {  
    CL_DQ; 
    Hal_HW_WaitUs(1);  //��ʱ϶��ʼ��΢�������������õ�����1΢��   
    SET_DQ;            //�������ߺ�����ͷ����ߣ��ôӻ�18b20�ܹ��ӹ����ߣ������Ч����
    SET_IN;  
    k=IN_DQ;    //������,�ӵ�λ��ʼ  
    if(k)  
    temp|=(1<<n);  
    else  
    temp&=~(1<<n);  
    Hal_HW_WaitUs(70); //60~120us      
    SET_OUT;
  
  }  
  return (temp);  
}  
//����ID

void get_id()
{     
  init_1820();        //resert
  write_1820(0x33); 
  
  for(unsigned char ii=0;ii<8;ii++)
  {
    id[ii]=read_1820();
  }
}


uint16 read_data(void)
{
  uint8 a,b,c;
  uint8 teml,temh;
  uint16 wendu=0;
  uint8 buf[5]={0};
  init_1820();  //��λ18b20  
  write_1820(0xcc);   // ����ת������ �������� 
  write_1820(0x44);     //����  
  Hal_HW_WaitUs(2000);
  init_1820();  
  write_1820(0xcc);
  write_1820(0xbe); 
       
  teml=read_1820();  //������  
  temh=read_1820();
//  ws_itoa(teml,buf,10);
 // WsUARTWrite(0,buf,5);
//  ws_itoa(temh,buf,10);
//  WsUARTWrite(0,buf,5);

  if(temh&0x80)//�ж�����
  {
    flag=1;
    c=0;
    c=c|temh;
    c=c&0x00ff;
    c=c<<8;
    a=c;

    a=c|teml;

    a=(a^0xffff);//���
    a=a+1;  //ȡ����1
    teml=a&0x0f;
    temh=a>>4;
  }
  else
  {
    flag=0;   //Ϊ��
    a=temh<<4;
    a+=(teml&0xf0)>>4; //�õ���λ��ֵ
    b=teml&0x0f;
    temh=a;
    teml=b&0x00ff;
  }
  wendu = teml*6;
  wendu =wendu +temh*100;
  return wendu;
}

























/*
cc2430��1���������� = 1��������
��32MHz��Ƶ�¹�����
1�������� = 1/32  uS ���� 1����������Ҳ��1/32 uS
�������ִ��һ������ָ����Ҫ1���������ڣ���ô�Ϳ���ͨ��������ʵ�ֶ�ʱ
*/

/*
#pragma optimize=none
void Delay_nus(uint16 s) 
{  
  while (s--)
  {
    asm("NOP");
    asm("NOP");
    asm("NOP");
    
    asm("NOP");
    asm("NOP");
    asm("NOP");
  }

}

void init_1820(void) //��ʼ��18b20
{
  SET_OUT; 
  SET_DQ;//���1
  CL_DQ; 
  Delay_nus(550);//����һ��ʱ�� 550
  SET_DQ;//�ͷ� 
  SET_IN;//���� 
  Delay_nus(20); //�ͷ����ߺ�ȴ�15-60us 40
  while(IN_DQ)  {;}//�ȴ��ظ� 
  Delay_nus(240);//�ظ��ĵ͵�ƽ��60��240us  
  SET_OUT; 
  SET_DQ;//�ص���ʼDQ=1��
}

void write_1820(uint8 x)//��18b20дһ���ֽ�����
{  
  uint8 m;    
  SET_OUT;
  for(m=0;m<8;m++)  
  {  
     CL_DQ;
     if(x&(1<<m))    //д���ݣ��ӵ�λ��ʼ  
     {
      SET_DQ; 
     }
     else  
     {
      CL_DQ;
     }
     Delay_nus(40);   //15~60us  
     SET_DQ;  
  }
  SET_DQ;  
} 




uint8 read_1820(void)  //��18b20��һ������
{   
  uint8 temp,k,n;    
  temp=0;  
  for(n=0;n<8;n++)  
  {  
    CL_DQ;    
    asm("nop");
    asm("nop");
    SET_DQ; 
    
    SET_IN;
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    k=IN_DQ;    //������,�ӵ�λ��ʼ  
    if(k)  
    temp|=(1<<n);  
    else  
    temp&=~(1<<n);  
    Delay_nus(70); //60~120us      70
    SET_OUT;
  
  }  
  return (temp);  
}  
//����ID
void get_id()//���18b20��id��
{     
  init_1820();        //resert
  write_1820(0x33); 
  
  for(unsigned char ii=0;ii<8;ii++)
  {
    id[ii]=read_1820();
  }
}

//����ƥ��
uint8 match_rom(uint8 *rom);
uint8 match_rom(uint8 *rom)
{
  unsigned char i;
  init_1820();
  write_1820(0x55);
  for(i=8;i>0;i--)
  {
          write_1820(*(rom+i-1));
  }
  return 1;
}
 
float read_data()//��ȡ�¶�����
{
  uint8 temh,teml; 
  uint8 a,b,c;
  int j=0;
  float wendu=0;

  init_1820();  //��λ18b20  
  write_1820(0xcc);   // ����ת������ �������� 
  write_1820(0x44);     //����  
  //Delay_nus(50000);
  for(j=20;j>1;j--)  
    Delay_nus(500);
  init_1820();  
  write_1820(0xcc);
  write_1820(0xbe); 
       
  teml=read_1820();  //������  
  temh=read_1820();  
  if(temh&0x80)//�ж�����
  {
    flag=1;
    c=0;
    c=c|temh;
    c=c&0x00ff;
    c=c<<8;
    a=c;

    a=c|teml;

    a=(a^0xffff);//���
    a=a+1;  //ȡ����1
    teml=a&0x0f;
    temh=a>>4;
  }
  else
  {
    flag=0;   //Ϊ��
    a=temh<<4;
    a+=(teml&0xf0)>>4; //�õ���λ��ֵ
    b=teml&0x0f;
    temh=a;
    teml=b&0x00ff;
  }
  wendu = (float)teml*625/1000.0;
  wendu =wendu +temh;
  return wendu;
}

#if 0   
////////////�������ݲ���/////////////////
void DataChange(uint8 * pSensorValue,uint8 * pChBuf)
{
  uint8 temh,teml;
  //uint8 ch[9];
  uint16 num;
  //uint8 i;
  
  teml=pSensorValue[0];
  temh=pSensorValue[1];
  //i=(Temp_channel-0x31)*2;
  pChBuf[0]='T';//��ǰ��ʾ�Ĵ������ı��
  pChBuf[1]=':';
  num=teml*625;          //С�����ֵ�ȡֵÿλ����0.0625�����ȣ�
  if(flag==1)            //�ж������¶�
  {
    pChBuf[2]='-';              //+0x2d Ϊ��"-"ASCII��
  }
  else 
    pChBuf[2]='+';   
  if(temh/100==0)
    pChBuf[3]=' ';
  else
    pChBuf[3]=temh/100+0x30;      //+0x30 Ϊ�� 0~9 ASCII��
  if((temh/10%10==0)&&(temh/100==0))
   pChBuf[4]=' ';
  else
    pChBuf[4]=temh/10%10+0x30;
  pChBuf[5]=temh%10+0x30;
  pChBuf[6]='.';
  pChBuf[7]=num/1000+0x30;//����С�����1λ����
  //Print(i,15,ch,1);   //��ʾ�¶�ֵ
  //Print(i,80,"��",1); //��ʾ���϶ȱ�ʾ  ��
  //�˴���Ӿ������ʾ����
  //LCD_write_english_string(8,1,(char *)ch);
}

void ds18b20_main()
{
 
  while(1)
  {
    read_data(sensor_data_value);// ��ȡ�¶�
    DataChange(sensor_data_value,ch); //���ݴ���
    //LCD_write_english_string(8,1,(char *)ch);
    
    Delay_nus(10000);
  }
}
#endif
*/

