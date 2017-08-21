#include "BaseDef.h"
#define ETHERNET_COM Ethernet_COM()
#define ETHERNET_HEARTBEAT Ethernet_HeartBeat()
#define INITIAL_ETHERNET Ethernet_Init()

IPAddress ip(192, 168, 1, CLIENTIP);
IPAddress server(192, 168, 1, SERVERIP);
EthernetClient client;

int HEART_BEAT_COUNTER = 0;            //心跳包计时
char RX_DATA[50] = {};                 //下载缓存
volatile int HEART_BEAT_FLAG;          //心跳包标志

union DeFloat     //用以解构float
{
  float Float;
  byte Byte[4];
};
DeFloat FLOAT;

union DeInt {     //用以解构int
  int Int;
  byte Byte[2];
};
DeInt INT;

void NetSend(int CmdNum);              //****数据发送函数****
void StartConnect(void);
void DataExtra(void);
int GetCmd(void);
int GetCmd_NEW(void);
void NetSend(int CmdNum);              //****数据发送函数****

void Ethernet_Init(void)             //建立TCP连接
{
  Ethernet.begin(mac, ip);
  delay(1000);
  StartConnect();
}

void Ethernet_HeartBeat(void)        //通信心跳
{
  static int heartBeatCnt = 0;
  heartBeatCnt++;
  if (heartBeatCnt == DataRefreshCycle / 10)
  {
    HEART_BEAT_FLAG = 1;
    heartBeatCnt = 0;
  }
}

void Ethernet_COM(void)               //****通信接收主程序****
{
  if (HEART_BEAT_FLAG)               //发送心跳包
  {
    NetSend(FunNum);
    HEART_BEAT_FLAG = 0;
    DebugInfoLine(String(millis()) + "  " + "Send Over...");
  }
  if (!client.connected())         //检测连接状态
  {
    DebugInfoLine(String(millis()) + "  " + "DisConnected...");
    client.stop();
    DebugInfoLine(String(millis()) + "  " + "Connect Stopped...");
    StartConnect();
  }
  GetCmd();
}

void StartConnect()         //建立TCP连接
{
  while (!client.connected())
  {
    DebugInfoLine(String(millis()) + "  " + "ReConnecting...");
    if (client.connect(server, PORT))
    {
      DebugInfoLine(String(millis()) + "  " + "Connected...");
    }
    else
    {
      DebugInfoLine(String(millis()) + "  " + "Connect Failed...");
    }
  }
}

void NetSend(int CmdNum)              //****数据发送函数****
{
  String TX_DATA = "";
  INT.Int = CmdNum;
  TX_DATA += char(0xA5), TX_DATA += char(0x5A);                   //起始字节
  TX_DATA += char(INT.Byte[1]), TX_DATA += char(INT.Byte[0]);     //变量编号
  for (int j = 0; j < TX_PrmNum[CmdNum]; j++) {                      //数据
    FLOAT.Float = UploadBuf[j];
    TX_DATA += char(FLOAT.Byte[0]);
    TX_DATA += char(FLOAT.Byte[1]);
    TX_DATA += char(FLOAT.Byte[2]);
    TX_DATA += char(FLOAT.Byte[3]);
  }
  TX_DATA += char(0xFF);                                          //结束字节
  client.print(TX_DATA);
}

int GetCmd(void)
{
  unsigned char TemDataCount;
  unsigned char Data[50];
  unsigned char DataCount = 0;
  unsigned char TemCmdNum = 0;

  while (client.available())         //读取上位机数据
  {
    Data[DataCount] = client.read();
    DataCount++;
  }
  if (DataCount != 0)
  {
    TemDataCount = DataCount;
    String str = "";
    while (TemDataCount-- > 0)
    {
      str += String((Data[DataCount - TemDataCount - 1]), HEX);
      str += " ";
    }
    //DebugInfoLine(String(millis()) + "  " + "Raw Data: " + str);
  }

  for (unsigned char i = 0; i < DataCount; i++)
  {
    if (Data[i] == 0xA5 && Data[i + 1] == 0x5A)
    {
      TemCmdNum = int(Data[i + 2]) * 256 + int(Data[i + 3]); //提取第2、3字节存储的《功能编号》，赋值给FunNum
      String str = "";
      for (int j = 0; j < RX_PrmNum[TemCmdNum]; j++)
      { //提取第4、5、6、7字节存储的《数据》，赋值给相应Buf[0],以此类推
        FLOAT.Byte[0] = Data[4 * j + 4 + i];
        FLOAT.Byte[1] = Data[4 * j + 5 + i];
        FLOAT.Byte[2] = Data[4 * j + 6 + i];
        FLOAT.Byte[3] = Data[4 * j + 7 + i];
        Buf[j] = FLOAT.Float;
        str += String(Buf[j]);
        str += " ";
      }
      DebugInfoLine(String(millis()) + "  " + "Data Value: " + str);
      NewDataDownloadFlag = 1;
      DataCount = 0;
      break;    //i=i+4*j;
    }
  }
}

/*
  void DataExtra(void)
  {
  FunNum = int(RX_DATA[3]) * 256 + int(RX_DATA[2]);     //提取第2、3字节存储的《功能编号》，赋值给FunNum
  for (int j = 0; j < PrmNum[FunNum]; j++)
  { //提取第4、5、6、7字节存储的《数据》，赋值给相应Buf[0],以此类推
    FLOAT.Byte[0] = RX_DATA[4 * j + 4];
    FLOAT.Byte[1] = RX_DATA[4 * j + 5];
    FLOAT.Byte[2] = RX_DATA[4 * j + 6];
    FLOAT.Byte[3] = RX_DATA[4 * j + 7];
    Buf[j] = FLOAT.Float;
  }
  }*/
/*
  int n = 0;
  int flag1 = 0;
  int flag2 = 0;

  while(0)                  //读取上位机数据
  {
    byte c = client.read();
    if (c<0)
      break;
    if(c == 0xA5)
    {
      Serial.println("A5");
      RX_DATA[0] = c;
      n = 1;
      flag1 = 1;
    }
    if(flag1 != 1)
      break;
    if(c == 0x5A)
    {
      Serial.println("5A");
      RX_DATA[1] = c;
      n = 2;
      flag2 = 2;
    }
    if(flag2 != 2)
      break;
    RX_DATA[n++] = char(c);                    //写入缓冲区
    Serial.println(c);
    if (c == 0xFF)                                //如果读到结束符
    {
      DataExtra();                                //提取数据
      n = 0;
      NewDataDownloadFlag = 1;                    //下载标识符置1
      flag1 = 0;
      flag2 = 0;
      break;                                      //结束读取
    }
  }


  while(!client.connected())
  {
    if (!client.connected())                        //检测连接状态
    {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
      StartConnect();
    }
  }

  if(heartBeatCnt == 100)
  {
    heartBeatCnt = 0;
    UploadBuf[0] = 3.14;
    UploadBuf[1] = 1.23;
    //NetSend(2);
    while(!client.connected())
    {
      client.print("ABCDE");
      Serial.println("Sending...\n");
    }
  }

  while (Serial.available() > 0)    //读取Serial发送至上位机
  {
    char inChar = Serial.read();
    if (client.connected()) {
      //client.print(inChar);
      client.print("ABC");
      //client.print(inChar);
    }
  }

  while (client.available())         //读取上位机数据
  {
    char c = client.read();
    Serial.print(c);
  }

    while (Serial.available() > 0)    //读取Serial发送至上位机
  {
    char inChar = Serial.read();
    if (client.connected()) {
      //client.print(inChar);
      client.print("ABC");
      //client.print(inChar);
    }
  }
*/
