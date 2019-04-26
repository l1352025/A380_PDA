#ifndef Tool_H
#define Tool_H

#include <stdarg.h>
// 引用本文件前 先引用 <HJLIB.H> 和 "string.h"
extern void _GUIRectangleFill(uint32 x, uint32 y, uint32 x1, uint32 y1, uint8 color);
extern size_t strlen(const char * /*s*/);

//---------------------------------  通用方法  -------------------------------------
//
//	IndexOf				在数组中查找子数组，可指定查找的起始位置和范围
//	ShowProgressBar		显示进度条
//	StringPadLeft		字符串左侧填充
//	StringTrimStart		字符串头部裁剪
//	HexToChar			16进制数转换成对应的字符
//	CharToHex			字符转换成对应的16进制数
//	GetStringHexFromBytes	将字节数组转换成16进制字符串
//	GetBytesFromStringHex	将16进制字符串转换成字节数组
//	GetCrc16 			计算CRC16
//	GetSum8				计算8位累加和
//
//-----------------------------------------------------------------------------------

/*
* 函数名：IndexOf
* 描  述：在数组中查找子数组，可指定查找的起始位置和范围
* 参  数：srcArray - 源数组地址， srcLen - 源数组长度
		  dstBytes - 目的数组地址， dstLen - 目的数组长度 
		  startIndex - 源数组查找的起始位置，offset - 查找的范围
* 返回值：int 等于-1: 未找到， 大于/等于0 : 目的数组在源数组中的起始索引
*/
int IndexOf(const uint8 * srcArray, int srcLen, const uint8 * dstBytes, int dstLen, int startIndex, int offset)
{
    int index = -1, i, j;
	
    if (dstBytes == NULL || dstLen == 0) return index;

    if(offset > (srcLen - startIndex))
    {
        offset = (srcLen - startIndex);
    }

    for (i = startIndex; i <= (startIndex + offset - dstLen); i++)
    {
        if (srcArray[i] == dstBytes[0])
        {
            for (j = 0; j < dstLen; j++)
            {
                if (srcArray[i + j] != dstBytes[j])
                {
                    break;
                }
            }

            if (j == dstLen)
            {
                index = i;
                break;
            }
        }
    }

    return index;
}

/*
* 函数名：ShowProgressBar
* 描  述：显示进度条
* 参  数：y - 进度条y坐标，将显示在(0,y)位置，固定宽度为160，固定高度为16,黑色填充
		  maxValue - 进度条最大值
		  currValue - 进度条当前值
* 返回值：int 等于-1: 未找到， 大于/等于0 : 目的数组在源数组中的起始索引
*/
void ShowProgressBar(uint8 y, uint32 maxValue, uint32 currValue)
{
	uint32 width = (currValue >= maxValue? 160 : 160*currValue/maxValue);
	_GUIRectangleFill(0, y, width, y + 16, 1);
}

/*
* 函数名：PrintfXyMultiLine_VaList
* 描  述：在屏幕 x,y 坐标显示多行字符串，可自动换行
* 参  数：x, y		- 屏幕中坐标
		  format	- 字符串格式
* 返回值：void
*/
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf)
{
	uint16 len; 
	uint8 * pr = buf;

	// first line
	_Printfxy(x, y, pr, 0);
	
	// next lines
	len = 0;
	while(*pr != '\0'){
		if(*pr == '\n' || (x + len) == 20){
			pr = (*pr == '\n' ? pr + 1 : pr);
			len = 0;
			x = 0;
			y += 16;
			_Printfxy(x, y, pr, 0);
		}
		pr++;
		len++;
	}
}

/*
* 函数名：PrintfXyMultiLine_VaList
* 描  述：在屏幕 x,y 坐标显示多行字符串，可自动换行
* 参  数：x, y		- 屏幕中坐标
		  format	- 字符串格式
		  ... 		- 可变参数
* 返回值：void
*/
void PrintfXyMultiLine_VaList(uint8 x, uint8 y, const char * format, ...)
{
	static uint8 buf[512] = {0};
	uint16 len; 
	va_list ap;

	va_start(ap, format);
	len = vsprintf(buf, format, ap);
	buf[len] = '\0';
	va_end(ap);

	PrintfXyMultiLine(x, y, buf);
}
/*
* 函数名：StringPadLeft
* 描  述：字符串左侧填充
* 参  数：srcStr - 原字符串
		  totalLen - 总字符长度：原字符+左侧填充的字符（若原字符长度>=总长度，则无需填充）
		  padChar - 填充的字符
* 返回值：void
*/
void StringPadLeft(const char * srcStr, int totalLen, char padChar)
{
	uint32 srcStrLen, i = 0;
	char *pr, *pw;

	srcStrLen = strlen(srcStr);
	if(srcStrLen >= totalLen || padChar == 0x00){
		return;
	}

	pr = srcStr + srcStrLen - 1;
	pw = srcStr + totalLen - 1;
	*(pw + 1) = 0x00;
	
	for(i = 0; i < srcStrLen; i++){
		*pw = *pr;
		pr--;
		pw--;
	}

	while(i < totalLen){
		*pw = padChar;
		pw--;
		i++;
	}

}

/*
* 函数名：StringTrimStart
* 描  述：字符串头部裁剪
* 参  数：srcStr - 字符串起始地址
		  trimChar - 裁剪的字符
* 返回值：int 裁剪后的字符串长度
*/
int StringTrimStart(const char * srcStr, char trimChar)
{
	uint32 srcStrLen, i = 0;
	char *pr, *pw;

	srcStrLen = strlen(srcStr);
	if(srcStrLen == 0 || trimChar == 0x00){
		return 0;
	}

	pr = srcStr;
	pw = srcStr;
	
	for(i = 0; i < srcStrLen; i++){
		if(*pr != trimChar){
			break;
		}
		pr++;
	}
	if(pr != srcStr){
		while(pr < srcStr + srcStrLen){
			*pw = *pr;
			pr++;
			pw++;
		}
		*pw = 0x00;
	}

	return (srcStrLen - i);
}


/*
* 函数名：HexToChar
* 描  述：16进制数转换成对应的字符
*/
char HexToChar(uint8 b)
{
	char decHex[16] = {'0', '1', '2', '3','4', '5', '6', '7','8', '9', 'A', 'B','C', 'D', 'E', 'F'};
	
	if(b < 16){
		return decHex[b];
	}else{
		return '\0';
	}
}
/*
* 函数名：CharToHex
* 描  述：字符转换成对应的16进制数
*/
uint8 CharToHex(char c)
{
	uint8 hex;
	
	if(c >= '0' && c <= '9'){
		hex = c - '0';
	}else if(c >= 'A' && c <= 'F'){
		hex = c - 'A' + 10;
	}else if(c >= 'a' && c <= 'f'){
		hex = c - 'a' + 10;
	}else{
		hex = 0xFF;
	}

	return hex;
}

/*
* 函数名：GetStringHexFromBytes
* 描  述：将字节数组转换成16进制字符串
* 参  数：strHex - 目的字符串缓冲区地址
		  bytes - 源字节数组
		  iStart - 数组中需要转换的起始索引
		  iLength - 需要转换的长度
		  separate - 字符串中Hex字节之间的间隔符：0 - 无间隔符， 其他字符 - 如空格或逗号
		  reverse - 是否需要倒序：false - 不倒序， true - 倒序
* 返回值：int - 转换后的字符数：0 - 转换失败
*/
int GetStringHexFromBytes(char * strHex, uint8 bytes[], int iStart, int iLength, char separate, bool reverse)
{
	uint8 aByte;
	int iLoop, index = 0;
   
	if(iLength == 0 || iStart < 0){
		strHex[index] = 0;
		return 0;
	}
	
	for (iLoop = 0; iLoop < iLength; iLoop++)
	{
		if (reverse){
			aByte = bytes[iStart + iLength - 1 - iLoop];
		}
		else{
			aByte = bytes[iStart + iLoop];
		}
		strHex[index++] = HexToChar(aByte >> 4);
		strHex[index++] = HexToChar(aByte & 0x0F);
		if(separate != 0){
			strHex[index++] = separate;
		}
	}
	strHex[index++] = 0;

	return index;
}
/*
* 函数名：GetBytesFromStringHex
* 描  述：将16进制字符串转换成字节数组
* 参  数：bytes - 目的字节数组
		  iStart - 数组中保存的起始索引
		  strHex - 源字符串缓冲区地址
		  separate - 字符串中Hex字节之间的间隔符：0 - 无间隔符， 其他字符 - 如空格或逗号
		  reverse - 是否需要倒序：false - 不倒序， true - 倒序
* 返回值：int - 转换后的字节数：0 - 转换失败
*/
int GetBytesFromStringHex(uint8 bytes[], int iStart, const char * strHex, char separate, bool reverse)
{
	int iLoop = 0, index = 0;
	int bytesLen, strHexLen;
	uint8 aByte;
  
	strHexLen = strlen(strHex);
	if(separate != 0){
		bytesLen = (strHexLen + 1) / 3;
	}else{
		bytesLen = (strHexLen + 1) / 2;
	}

	if(bytesLen == 0 || iStart < 0){
		return 0;
	}

	while (iLoop < strHexLen - 1)
	{
		aByte = (CharToHex(strHex[iLoop]) << 4) | (CharToHex(strHex[iLoop + 1]) & 0x0F);
		iLoop += 2;

		if (reverse){
			bytes[iStart + bytesLen - 1 - index] = aByte;
		}
		else{
			bytes[iStart + index] = aByte;
		}
	
		if(separate != 0){
			iLoop++;
		}
		index++;
	}

	return index;
}

/*
* 函数名：GetCrc16
* 描  述：计算CRC16
* 参  数：Buf - 数据缓存起始地址
		  Len - 计算的总长度
		  Seed - 如电力/水力固定使用 0x8408
* 返回值：uint16 CRC16值
*/
uint16 GetCrc16(uint8 *Buf, uint16 Len, uint16 Seed)
{
    uint16 crc = 0xFFFF;
    uint8 i;

	while (Len--){
        crc ^= * Buf++;
        for(i = 0; i < 8; i++){
            if (crc & 0x0001){
                crc >>= 1;
                crc ^= Seed;
            }
            else{
                crc >>= 1;
            }
        }
    }
    crc ^= 0xFFFF;

    return crc;
}

/*
* 函数名：GetCrc16
* 描  述：计算CRC8
* 参  数：Buf - 数据缓存起始地址
		  Len - 计算的总长度
		  Seed - 如电力/水力固定使用 0x8408
* 返回值：uint8 CRC16值
*/
uint8 GetCrc8(uint8 *Buf, int len)
{
    uint8 i;
    uint8 crc = 0x0;

    while (len--)
    {
        crc ^= *Buf++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x01)
                crc = (crc >> 1) ^ 0x8C;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/*
* 函数名：GetSum8
* 描  述：计算8位累加和
* 参  数：Buf - 数据缓存起始地址
		  Len - 计算的总长度
* 返回值：uint8 累加和
*/
uint8 GetSum8(uint8 *buf, uint16 len)
{
    uint8 sum = 0;

	while(len-- > 0){
		sum += *buf++;
	}
	
    return sum;
}

#endif