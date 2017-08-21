#include "Arduino.h"

#define PORT  8007
#define CLIENTIP  101
#define SERVERIP  109
#define DataRefreshCycle 100

extern float Buf[255];
extern volatile int NewDataDownloadFlag;
extern float UploadBuf[255];
extern int FunNum;                           //功能编号
extern int RX_PrmNum[255];                      //参数个数
extern int TX_PrmNum[255]; 
extern byte mac[];
extern void DebugInfoLine(String str);
