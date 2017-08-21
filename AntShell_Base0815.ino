/*以下为包含头文件*/
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <FlexiTimer2.h>
#include "EthernetCom.h"
/*以下为全局变量*/
float Buf[255] = {0};
volatile int NewDataDownloadFlag = 0;
float UploadBuf[255] = {0};
/*以下为功能预定义项*/
#define F000 1
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#ifdef F000
#include "Fun000.h"
#endif
#ifdef F001
#include "Fun001.h"
#endif
#ifdef F002
#include "Fun002.h"
#endif

int FunNum;                           //功能编号
int RX_PrmNum[255];                   //参数个数
int TX_PrmNum[255];
int SerialCmd[255] = {0};

void setup()
{
  System_Init();
  Data_Init();
  //INITIAL_ETHERNET;
  INITIAL_FUNCTION;
  FlexiTimer2::set(10, 1.0 / 1000, Refresh);
  FlexiTimer2::start();
}

void loop()
{
  //ETHERNET_COM;
}

void System_Init(void)
{
  Serial.begin(115200);
  while (!Serial);
  delay(1000);
}

void Data_Init(void)
{
}

void Refresh(void)//10ms中断
{
  static int RefreshCount = 0;
  RefreshCount++;
  if (RefreshCount == 100)
  {
    DebugInfoLine(String(millis()) + "  " + "Main Interrupt...");
    RefreshCount = 0;
  }
  ETHERNET_HEARTBEAT;
  INITIAL_FUNCTION;
  PARASE_CMD;
  LOGIC_CONTROL;
  CONTORL_OUTPUT;
  UPLOAD_INFO;
  GetSerialCmd();
}

void DebugInfoLine(String str)
{
  if(SerialCmd[0] == 1)
    Serial.println(str);
  else
    return;
}

int GetSerialCmd(void)
{
  static unsigned char Data[50];
  static unsigned char DataCount = 0;

  while (Serial.available())         //读取上位机数据
  {
    Data[DataCount] = Serial.read();
    DataCount++;
  }

  if(DataCount >  0)
  {
    Serial.print("DataCount:  ");
    Serial.println(DataCount);
  }
  else
  {
    DataCount = 0;
    return 0;
  }

  if(DataCount < 4)
    return 0;
  
  for (unsigned char i = 0; i < DataCount; i++)
  {
    if (Data[i] == 0xA5 && Data[i + 1] == 0x5A)
    {
      for(int j=0;j<255;j++)
      {
        if(Data[i+2+j] == 0x55)
        {
          SerialCmd[j] = Data[i+2+j];
          break;
        }
        else
          SerialCmd[j] = Data[i+2+j];
      }
      DataCount = 0;
      int j = 0;
      while(SerialCmd[j++] != 0x55)
        Serial.println(SerialCmd[j-1]);
      break;
    }
  }
  DataCount = 0;
  for(int k=0;k<50;k++)
    Data[k] = 0;
  return 1;
}
