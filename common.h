#ifndef COMMON_H
#define COMMON_H


// --------------------------------		类型定义	-----------------------------------------

typedef unsigned char bool;
#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif

#define VerInfo_Name    (char *)"桑锐6009手持机"     // 程序名
#define VerInfo_RevNo   (char *)"2.2"               // 版本号
#define VerInfo_RevDate (char *)"2019-6-3"         // 版本日期
//#define VerInfo_Release                             // 发布时必须定义该宏， 调试时注释


#ifndef VerInfo_Release
#define Log_On      1           // 调试日志开关
#define LogName     "debug.txt" // 日志文件名
#define LogScom_On  1           // 日志串口开关：1- 输出到串口，0 -输出到文件
#define RxBeep_On   1       // 串口接收完成-响铃提示开关： 响一下- 解析成功， 响两下 - 解析失败
#else
#define Log_On      0 
#define RxBeep_On   1       
#endif

#define TXTBUF_LEN	20      // 文本输入最大字符数
#define RELAY_MAX   3       // 最大中继个数
#define UI_MAX      10      // 最大UI控件数


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
	#define CurrPort    Trans_IR                
	#define CurrBaud    (uint8 *)"1200,E,8,1"
    #define TransType   "红外透传"      // 通信方式	
    #define AddrLen     8
#else //defined(Project_6009_RF)
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1"
    #define Timeout     Trans_IR 
    #define TransType   "Lora透传"      // 通信方式	
    #define AddrLen     6
#endif

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
    UI_Lable
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
    
    struct{
        char *strs[10];
		uint8 cnt;
		uint8 *currIdx;
	}combox;

	struct {
		uint8 dataLen;
        uint8 isClear;
        uint8 dotEnable;
	}txtbox;
}UI_Item;

typedef struct{
    UI_Item items[UI_MAX];
    uint8 cnt;
}UI_ItemList;


//---------------------------------		函数声明	 -----------------------------------------
int IndexOf(const uint8 * srcArray, int srcLen, const uint8 * dstBytes, int dstLen, int startIndex, int offset);
uint16 GetCrc16(uint8 *Buf, uint16 Len, uint16 Seed);
uint8 GetCrc8(uint8 *Buf, int len);
uint8 GetSum8(uint8 *buf, uint16 len);
char HexToChar(uint8 b);
uint8 CharToHex(char c);
uint8 BcdToDec(uint8 bcd);
uint8 DecToBcd(uint8 dec);
int GetStringHexFromBytes(char * strHex, uint8 bytes[], int iStart, int iLength, char separate, bool reverse);
int GetBytesFromStringHex(uint8 bytes[], int iStart, int iLength, const char * strHex, char separate, bool reverse);
void StringPadLeft(const char * srcStr, int totalLen, char padChar);
int StringTrimStart(const char * srcStr, char trimChar);
void ShowProgressBar(uint8 y, uint32 maxValue, uint32 currValue);
void LableCreate(UI_Item *item, uint8 x, uint8 y, const char * title);
void TextBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, char * text, uint8 maxLen, uint8 width, bool isClear);
void CombBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, uint8 * currIdx, uint32 maxCnt, ...);
uint8 ShowUI(UI_ItemList inputList, uint8 *itemNo);
void PrintfXyMultiLine_VaList(uint8 x, uint8 y, const char * format, ...);
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf, uint8 maxLines);
void PrintXyTriangle(uint8 x, uint8 y, uint8 direction);
uint8 GetPrintLines(uint8 x, const char * buf, char * lines[]);
void LogPrint(const char * format, ...);
void LogPrintBytes(const char *title, uint8 *buf, uint16 size);

//--------------------------------		全局变量	 ---------------------------------------
extern char Screenbuff[160*(160/3+1)*2]; 
extern uint8 TmpBuf[1080];
extern uint8 TxBuf[1080];
extern uint8 RxBuf[1080];
extern uint8 DispBuf[2048];
extern uint32 RxLen, TxLen;
extern const uint8 LocalAddr[9];
extern uint8 DstAddr[9];
extern uint8 VerInfo[41];
extern char StrBuf[10][TXTBUF_LEN];    // extend input buffer
extern char StrDstAddr[TXTBUF_LEN];
extern char StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
extern UI_ItemList UiList;
extern uint16 CurrCmd;
extern ParamsBuf Addrs;		
extern ParamsBuf Args;

#endif
