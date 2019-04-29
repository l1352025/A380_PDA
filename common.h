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

#define TXTBUF_LEN	20  // 文本输入最大字符数
#define RELAY_MAX   3   // 最大中继个数

typedef enum{
    Color_White     = 0,
    Color_Black     = 1
}GUI_COLOR;

typedef struct{
	uint8 buf[200];
	uint8 *items[10];
	uint8 itemCnt;
    uint8 lastItemLen;
}ParamsBuf;

typedef struct{
    uint8 x;
    uint8 y;
    char *title;
    char *text;
    uint8 dataLen;
    uint8 width;
}InputItem;

typedef struct{
    InputItem items[8];
    uint8 cnt;
}InputItemBuf;


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
void InputBoxShow(InputItem *item, uint8 x, uint8 y, const char * title, char * text, uint8 maxLen, uint8 width);
uint8 InputBoxGetStr(uint8 x, uint8 y, const char * title, char * text, uint8 maxLen);
uint8 ShowInputUI(InputItemBuf inputList, uint8 *itemNo);
void PrintfXyMultiLine_VaList(uint8 x, uint8 y, const char * format, ...);
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf);

//--------------------------------		全局变量	 ---------------------------------------
extern uint8 TmpBuf[];
extern uint8 LocalAddr[];

#endif
