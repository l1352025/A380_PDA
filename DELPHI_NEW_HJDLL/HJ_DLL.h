// 下列 ifdef 块是创建使从 DLL 导出更简单的

typedef struct
{
	int nPort;
	int nBaudrate;
}PortConfig;


extern "C"  PortConfig __stdcall g_PortConfig;

extern "C" 	 int __stdcall OpenPort(int iPort,int iBaud);
extern "C" 	 int __stdcall ClosePort(void);
extern "C" 	 int __stdcall ReadPort(char *dataBuff);
extern "C" 	 int __stdcall WritePort(char *dataBuff,int iLength);

extern "C"   int __stdcall  GetHcNum(char *PORT,char *BAUDRATE,char *HCNUM);
extern "C"   int __stdcall  GetDateTime(char *PORT,char *BAUDRATE,char *DATE,char *TIME);
extern "C"   int __stdcall  SetDateTime(char *PORT,char *BAUDRATE,char *DATE,char *TIME);
extern "C"   int __stdcall  SendFileToHc(char *PORT,char *BAUDRATE,char *DATAFILE, char *APPFILE, char *PROMPT);
extern "C"   int __stdcall  SendFileToHcEx(char *PORT,char *BAUDRATE,char *PCFILE,char *HCFILE,  char *PROMPT);
extern "C"   int __stdcall  SendFileToPc(char *PORT,char *BAUDRATE,char *DATAFILE, char *FILESAVEAS, char *PROMPT);
extern "C"   int __stdcall  GetHcDir(char *PORT,char *BAUDRATE,char *PATH,char* FILEDIR);
extern "C"  int __stdcall   DelHcFile(char *PORT,char *BAUDRATE,char *HCFILE);
extern "C"  int __stdcall   DelHcDir(char *PORT,char *BAUDRATE,char *HCDIR);
extern "C"  int __stdcall   CreateHcDir(char *PORT,char *BAUDRATE,char *HCDIR);
extern "C"  int __stdcall   DebugHex(char *PORT,char *BAUDRATE,char *PCFILE,char *HCFILE);
extern "C"   int __stdcall  TestDll(void);




