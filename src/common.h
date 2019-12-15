#ifndef COMMON_H
#define COMMON_H

typedef unsigned char  uint8;                
typedef signed   char  int8;                     
typedef unsigned short uint16;                  
typedef signed   short int16;                   
typedef unsigned int   uint32;                 
typedef signed   int   int32;   
typedef unsigned char bool;
#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif
#ifndef NULL
#define NULL   0
#endif

// --------------------------------		类型定义	-----------------------------------------
/*  串口物理端口： NO.1 / NO.2 / NO.3
        物理NO.1 (TP_PORT_TXD / TP_PORT_RXD)
        物理NO.2 (TP_COM_TXD / TP_COM_RXD)
        物理NO.3 (TP_SCAN_TXD / TP_SCAN_RXD) 
    串口逻辑端口：1.串口 / 2.普通红外 / 3.高速红外
        _ComSetTran(logicPort)
        （使用前先设置有效的物理端口 NO.1 / NO.2 / No.3）
        如使用”高速红外“,则先选择物理端口NO.1 或 No.3： 
            系统设置->模块设置->红外设置->高速红外->
*/
#define Trans_Scom          1   // 扫描端口
#define Trans_IR            2   // 普通红外 
#define Trans_IR_Quick      3   // 高速红外

// 当前透传模块使用的通信端口 和 波特率
#if defined(Project_6009_IR)
    #define VerInfo_Name    (char *)"桑锐6009手持机"    // 程序名
    #define VerInfo_RevNo   (char *)"2.8.预览1"	           // 版本号
    #define VerInfo_RevDate (char *)"2019-12-5"        // 版本日期
    #define TransType   (char *)"红外透传"              // 通信方式	
	#define CurrPort    Trans_IR                
	#define CurrBaud    (uint8 *)"9600,E,8,1"
    #define AddrLen     8           // 地址长度(byte)：8 
    #define VerLen      40          // 版本长度(byte)：40 
    #define LogPort     CurrPort            // 日志输出串口
    #define UseBroadAddr    0               // 使用广播地址抄表 D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    1       // 使用大文件缓存：整个App文件读到内存缓存 *FileBuf
#elif defined(Project_6009_RF)
    #define VerInfo_Name    (char *)"桑锐6009手持机"     // 程序名
    #define VerInfo_RevNo   (char *)"2.5"               // 版本号
    #define VerInfo_RevDate (char *)"2019-08-01"        // 版本日期
    #define TransType   (char *)"Lora透传"              // 通信方式	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define AddrLen     6           // 地址长度(byte)：6 
    #define VerLen      40          // 版本长度(byte)：40 
    #define LogPort     CurrPort            // 日志输出串口
    #define CenterCmd_Enable        0       // 集中器命令可使用：目前不可用
    #define Upgrd_FileBuf_Enable    0       // 使用大文件缓存：整个App文件读到内存缓存 *FileBuf
#else // defined(Project_8009_RF)
    #define VerInfo_Name    (char *)"桑锐8009手持机"     // 程序名
    #define VerInfo_RevNo   (char *)"1.0"               // 版本号
    #define VerInfo_RevDate (char *)"2019-09-04"        // 版本日期
    #define TransType   (char *)"Lora透传"              // 通信方式	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define AddrLen     5           // 地址长度(byte)：5 
    #define VerLen      24          // 版本长度(byte)：24 
    #define LogPort     CurrPort            // 日志输出串口
    #define CenterCmd_Enable        0       // 集中器命令可使用：目前不可用
    #define Upgrd_FileBuf_Enable    0       // 使用大文件缓存：整个App文件读到内存缓存 *FileBuf
#endif


#define VerInfo_Previwer    (char *)"  <去掉D4D4抄表>"    // 预览版时,定义该宏
#define VerInfo_Release                     // 发布时必须定义该宏， 调试时注释

#ifndef VerInfo_Release
#define LOG_ON      1           // 调试日志开关
#define LogFileName "debug.txt" // 日志文件名
#define LogFileSize (100*1024)  // 日志文件最大字节数
#define LogScom_On  0           // 日志串口开关：1- 输出到串口，0 -输出到文件
#define LogTxRx     1           // 日志输出Tx/Rx数据：0 - 不输出， 1 - 输出
#define RxBeep_On   0           // 串口接收完成-响铃提示开关： 响一下- 解析成功， 响两下 - 解析失败
#else
#define LOG_ON      0 
#define RxBeep_On   1       
#endif

#define RELAY_MAX   3       // 最大中继个数: 3
#define TXTBUF_MAX  20      // 文本输入缓冲区最大个数
#define TXTBUF_LEN	20      // 文本输入缓冲区最大字符数
#define UI_MAX      20      // 最大UI控件数
#define ListStrMax  300     // 最大列表字符串数
#define STR_Size    50      // 默认字符串字节数
#define CbxItem_Max 15      // 单选框最大项

typedef enum{
    Color_White = 0,
    Color_Black = 1
}GUI_COLOR;

typedef struct{
	uint8 buf[200];
	uint8 *items[10];
	uint8 itemCnt;
    uint8 lastItemLen;
}ParamsBuf;

typedef enum{
    UI_TxtBox,
    UI_CombBox,
    UI_Lable,
    UI_Button
}UI_Type;

typedef struct{
    
    uint8 x;        // UI title position
    uint8 y;
    char *title;
    uint8 x1;       // UI text position
    uint8 y1;
    char *text;
    uint8 width;    // UI width
    uint8 height;   // UI high
    UI_Type type;
    
    union {
        struct{
            char *strs[CbxItem_Max];
            uint8 cnt;
            uint8 *currIdx;
        }combox;

        struct{
            uint8 dataLen;
            uint8 isClear;
            uint8 dotEnable;
            uint8 isInputAny;
        }txtbox;
    }ui;
}UI_Item;

typedef struct{
    UI_Item items[UI_MAX];
    uint8 cnt;
}UI_ItemList;

typedef void (*FillListFunc)(char **strs, int16 dstIdx, int16 srcIdx, uint16 cnt);

typedef struct{
    uint8 x;        
    uint8 y;
    uint8 maxRow;       // 一页最多显示行数 ： 最大8行
    uint8 maxCol;       // 一行最多显示列数：最大20个英文字符
    uint8 isCircle;     // 可循环列表标识 : 默认可循环
    uint16 dispStartIdx; // 当前页显示的第一条记录在显示缓冲区的位置 
    uint16 totalCnt;    // 数据库记录总数
    int16 currIdx;      // 数据库当前记录的位置
    char *title;
    char *str[ListStrMax];  // 字符串列表
    uint16 strCnt;          // 字符串列表中记录总数
    int16 strIdx;          // 字符串列表中当前位置        确认键/取消键退出列表时的位置  
    FillListFunc fillStrsFunc;      // 翻页时回调函数

}ListBox;

typedef enum{
	CmdResult_Ok,
	CmdResult_Failed,
	CmdResult_CrcError,
	CmdResult_Timeout,
    CmdResult_Cancel
}CmdResult;

typedef	enum{
    Cmd_Send,
    Cmd_RecvOk,
    Cmd_RecvNg,
    Cmd_Finish,
    Cmd_Exit
}CmdState;

typedef void (*FuncCmdCycleHandler)(uint8 currKey);
typedef uint8 (*FuncCmdFramePack)(uint8 *txBuf, ParamsBuf *addrs, uint16 cmdid, ParamsBuf *args, uint8 sendCnt);
typedef uint8 (*FuncCmdFrameExplain)(uint8 *rxBuf, uint16 rxlen, const uint8 *dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

//---------------------------------		函数声明	 -----------------------------------------
int IndexOf(const uint8 * srcArray, int srcLen, const uint8 * dstBytes, int dstLen, int startIndex, int offset);
uint16 GetCrc16(uint8 *Buf, uint32 Len, uint16 Seed);
uint16 GetCrc16_Continue(uint8 *Buf, uint32 Len, uint16 Seed, uint16 *CrcKeep);
uint8 GetCrc8(uint8 *Buf, uint32 len);
uint8 GetSum8(uint8 *buf, uint32 len);
char HexToChar(uint8 b);
uint8 CharToHex(char c);
uint8 BcdToDec(uint8 bcd);
uint8 DecToBcd(uint8 dec);
int GetStringHexFromBytes(char * strHex, uint8 bytes[], int iStart, int iLength, char separate, bool reverse);
int GetBytesFromStringHex(uint8 bytes[], int iStart, int iLength, const char * strHex, char separate, bool reverse);
void StringPadLeft(const char * srcStr, int totalLen, char padChar);
int StringTrimStart(const char * srcStr, char trimChar);
int StringTrimEnd(const char * srcStr, char trimChar);
void StringFixGbkStrEndError(const char * srcStr);
void ShowProgressBar(uint8 y, uint32 maxValue, uint32 currValue);
void LableCreate(UI_Item *item, uint8 x, uint8 y, const char * title);
void TextBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, char * text, uint8 maxLen, uint8 width, bool isClear);
void CombBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, uint8 * currIdx, uint32 maxCnt, ...);
uint8 ShowUI(UI_ItemList inputList, uint8 *itemNo);
void ListBoxCreate(ListBox *lbx, uint8 x, uint8 y, uint8 maxCol, uint8 maxRow, uint16 totalCnt, FillListFunc fillStrsFunc, const char *title, uint32 strCnt, ...);
void ListBoxCreateEx(ListBox *lbx, uint8 x, uint8 y, uint8 maxCol, uint8 maxRow, uint16 totalCnt, FillListFunc fillStrsFunc, const char *title, char **strs, uint8 strLen, uint8 strCnt);
uint8 ShowListBox(ListBox *lbx);
void ShowMsg(uint8 x, uint8 y, char *str, uint16 waitMs);
void PrintfXyMultiLine_VaList(uint8 x, uint8 y, const char * format, ...);
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf, uint8 maxLines);
void PrintXyTriangle(uint8 x, uint8 y, uint8 direction);
uint8 GetPrintLines(uint8 x, const char * buf, char * lines[]);
uint8 ShowScrollStr(char *strBuf, uint8 lineStep);
uint8 ShowScrollStrEx(char *strBuf, uint8 lineStep);
void LogPrint(const char * format, ...);
void LogPrintBytes(const char *title, uint8 *buf, uint16 size);
void DatetimeToTimeStrs(const char *datetime, char *year, char *month, char *day, char *hour, char *min, char *sec);
uint8 TimeStrsToTimeBytes(uint8 bytes[], char *year, char *month, char *day, char *hour, char *min, char *sec);
uint8 IpStrsToIpBytes(uint8 ip[], char *strIp1, char *strIp2, char *strIp3, char *strIp4);
CmdResult CommandTranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt);
void CycleInvoke_OpenLcdLight_WhenKeyPress(uint8 currKey);
void LcdLightCycleCtrl(uint8 *lcdCtrl, uint8 closeCnt);
void MeterNoSave(uint8 *mtrNo, uint8 type);
void MeterNoLoad(uint8 *mtrNo, uint8 type);

//--------------------------------		全局变量	 ---------------------------------------
//extern char Screenbuff[160*(160/3+1)*2]; 
#if Upgrd_FileBuf_Enable
extern uint8 DispBuf[128 * 1024];					    // 4k ~ 128K
#else
extern uint8 DispBuf[14 * 1024];					    // 4k ~ 14K
#endif
extern uint8 * const LogBuf; // = &DispBuf[4096];     	// 4k ~ 
extern uint8 * const TmpBuf; // = &DispBuf[8192];     	// 2K ~ 
extern uint8 * const BackupBuf; // = &DispBuf[10240];	// 2k ~
#if Upgrd_FileBuf_Enable
extern uint8 * const FileBuf; // = &DispBuf[14336];	    // 116k 
#endif
extern uint8 TxBuf[1024];
extern uint8 RxBuf[1024];
extern uint32 RxLen, TxLen;
extern const uint8 LocalAddr[10];	// 地址 201900002019/0000，12/16字符
extern uint8 DstAddr[10];
extern uint8 VerInfo[42];
extern uint16 CurrCmd;
extern ParamsBuf Addrs;		
extern ParamsBuf Args;
extern char StrBuf[TXTBUF_MAX][TXTBUF_LEN];    // extend input buffer
extern char StrDstAddr[TXTBUF_LEN];
extern char StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
extern UI_ItemList UiList;
extern bool LcdOpened;
extern bool IsNoAckCmd;
extern FuncCmdCycleHandler TranceiverCycleHook;
extern FuncCmdFramePack FramePack;
extern FuncCmdFrameExplain FrameExplain;
#endif
