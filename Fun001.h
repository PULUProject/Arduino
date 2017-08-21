#include "BaseDef.h"

#define INITIAL_FUNCTION InitlizeAll_F001()
#define PARASE_CMD Parase_F001()
#define LOGIC_CONTROL LogicControl_F001()
#define CONTORL_OUTPUT ControlOutput_F001()
#define UPLOAD_INFO UploadInfo_F001()

int EnableFlag = 0;
int LedCtrl= 0;
int LedState = 0;
int ButtonState = 0;
int LightSensor = 0;
int ifUpload = 0;
void Led_Fun(void);

void InitlizeAll_F001(void)
{
  pinMode(13, OUTPUT);
  FunNum = 1;
  RX_PrmNum[FunNum] = 2;                          //代表【功能1】的参数个数为1个 
  TX_PrmNum[FunNum] = 2;
}

void Parase_F001(void)//将需要的控制信息从Buf中解析出来
{
  if(NewDataDownloadFlag==1)         //若有数据下载
  {
    EnableFlag = Buf[0];
    LedCtrl = Buf[1];
    NewDataDownloadFlag=0;            //flag置零
  }
}

void LogicControl_F001(void)
{
  if(EnableFlag == 1)
  {
    Led_Fun();
    LightSensor = analogRead(A0)/10;
  }
}

void ControlOutput_F001(void)
{
}

void UploadInfo_F001(void)
{
    UploadBuf[0] = LightSensor;
    UploadBuf[1] = LedState;
}

void Analog_Fun(void)
{
  if(ifUpload == 1)
  {
    if(analogRead(1) <= 200)
    {
      ButtonState = 0;
    }
    else
    {
      ButtonState = 1;
    }
  }  
}

void Led_Fun(void)
{
   if(LedCtrl == 0)
  {
    digitalWrite(13, LOW);
    LedState = 0;
  }
  else if(LedCtrl == 1)
  {
    digitalWrite(13, HIGH);
    LedState = 1;
  }
}

