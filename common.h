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

typedef struct{
	uint8 buf[200];
	uint8 len;
	uint8 *items[10];
	uint8 cnt;
}ParamsBuf;


//---------------------------------		函数声明	-----------------------------------------
int IndexOf(const uint8 * srcArray, int srcLen, const uint8 * dstBytes, int dstLen, int startIndex, int offset);
uint16 GetCrc16(uint8 *Buf, uint16 Len, uint16 Seed);
uint8 GetCrc8(uint8 *Buf, int len);
uint8 GetSum8(uint8 *buf, uint16 len);
char HexToChar(uint8 b);
uint8 CharToHex(char c);

#endif
