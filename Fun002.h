#include "BaseDef.h"
#include <IRLib2.h>

#define INITIAL_FUNCTION InitlizeAll_F002()
#define PARASE_CMD Parase_F002()
#define LOGIC_CONTROL LogicControl_F002()
#define CONTORL_OUTPUT ControlOutput_F002()
#define UPLOAD_INFO UploadInfo_F002()

int LedCount = 1;    //LED延时计数
int IRcount = 1;    //IR延时计数
int deviceCount = 1;
int LedFrequency;   //LED频率
int RECV_PIN = 2;        //定义红外接收器的引脚为2
float device_ID;
int delayTime;
bool LedState = 1;
bool EnableFlag;
bool LedUpload;
bool device_flag;

IRrecvPCI irrecv(RECV_PIN);
IRdecode decoder;

void Led_Fun(void);
void IRremote(void);

void InitlizeAll_F002(void)
{
  pinMode(13, OUTPUT);
  FunNum = 1;
  RX_PrmNum[FunNum] = 2;        //代表【功能1】的参数个数为1个 
  TX_PrmNum[FunNum] = 2;
  irrecv.enableIRIn();          //启动接收器
}

void Parase_F002(void)//将需要的控制信息从Buf中解析出来
{
  if (NewDataDownloadFlag == 1)      //若有数据下载
  {
    EnableFlag = Buf[0];
    LedFrequency = Buf[1];
    NewDataDownloadFlag = 0;            //flag置零
  }
}

void LogicControl_F002(void)
{
  Led_Fun();
  IRremote();
}

void Led_Fun(void)
{
  if (LedFrequency == 1)
  {
    delayTime = 500;
    LedUpload = 1;
  }
  else
  {
    delayTime = 100;
    LedUpload = 0;
  }
  //  delayTime = 1000/(LedFrequency+1)/2;
  if (LedCount % (delayTime / 10) == 0)
  {
    LedCount = 0;
    LedState = !LedState;
    digitalWrite(13, LedState);
  }
  LedCount++;
}

void IRremote(void)
{
  if (IRcount == 10)
  {
    if (irrecv.getResults())
    {
      if (decoder.decode())
      {
        if (decoder.value == 0x001)
        {
          device_ID = 50;
          device_flag = 1;
        }
        else if (decoder.value == 0x002)
        {
          device_ID = 20;
          device_flag = 1;
        }
      }
    }
    irrecv.enableIRIn();
    IRcount = 0;
  }
  IRcount++;
  if (device_flag == 1)
  {
    if (deviceCount == 100)
    {
      device_ID = 0;
      deviceCount = 0;
      device_flag = 0;
    }
    deviceCount++;
  }
}

void ControlOutput_F002(void)
{
}

void UploadInfo_F002(void)
{
  UploadBuf[0] = device_ID;
  UploadBuf[1] = LedUpload;
}
