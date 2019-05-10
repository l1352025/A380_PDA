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

#define VERSION_Name    "桑锐6009手持机"     // 程序名
#define VERSION_RevNo   "1.0"               // 版本号
#define VERSION_Date    "2019-5-9"          // 版本日期

#define TXTBUF_LEN	20      // 文本输入最大字符数
#define RELAY_MAX   3       // 最大中继个数
#define UI_MAX      10
#define LogName     "debug.log"  // 日志文件名

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
	#define CurrBaud    (uint8 *)"4800,E,8,1"	
#else //defined(Project_6009_RF)
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1"	
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
    UI_CombBox
}UI_Type;

typedef struct{
    
    uint8 x;        // title position
    uint8 y;
    char *title;
    uint8 x1;       // text position
    uint8 y1;
    char *text;
    uint8 width;    // text width
    UI_Type type;

    struct{
		uint8 cnt;
		uint8 currIdx;
	}combox;

	struct {
		uint8 dataLen;
	}txtbox;
}UI_Item;

typedef struct{
    uint8 uiBuf[200];
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
int GetStringHexFromBytes(char * strHex, uint8 bytes[], int iStart, int iLength, char separate, bool reverse);
int GetBytesFromStringHex(uint8 bytes[], int iStart, int iLength, const char * strHex, char separate, bool reverse);
void StringPadLeft(const char * srcStr, int totalLen, char padChar);
int StringTrimStart(const char * srcStr, char trimChar);
void ShowProgressBar(uint8 y, uint32 maxValue, uint32 currValue);
void TextBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, char * text, uint8 maxLen, uint8 width);
uint8 TextBoxGetStr(uint8 x, uint8 y, const char * title, char * text, uint8 maxLen);
uint8 ShowUI(UI_ItemList inputList, uint8 *itemNo);
void PrintfXyMultiLine_VaList(uint8 x, uint8 y, const char * format, ...);
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf, uint8 maxLines);
uint8 GetPrintLines(uint8 x, const char * buf, char * lines[]);
void LogToFile(const char * fileName, const char * format, ...);

//--------------------------------		全局变量	 ---------------------------------------
extern char Screenbuff[160*(160/3+1)*2]; 
extern uint8 TmpBuf[1080];
extern uint8 TxBuf[1080];
extern uint8 RxBuf[1080];
extern uint32 RxLen, TxLen;
extern uint8 LocalAddr[7];
extern uint8 DstAddr[7];
extern uint8 VersionInfo[40];
extern uint8 StrDstAddr[TXTBUF_LEN];
extern uint8 StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
extern UI_ItemList UiList;
extern uint8 CurrCmd;
extern ParamsBuf Addrs;		
extern ParamsBuf Args;
extern ParamsBuf Disps;

#endif
