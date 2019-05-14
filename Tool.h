#ifndef Tool_H
#define Tool_H

#include "HJLIB.H"
#include "stdarg.h"
#include "Common.h"

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
* 描  述：打印日志到文件
* 参  数：fileName	 - 文件名
		  format	- 字符串格式
		  ... 		- 可变参数
* 返回值：void
*/
void LogToFile(const char * fileName, const char * format, ...)
{
	static uint8 buf[512] = {0};
	int fp;
	uint32 len = 0; 
	va_list ap;

	va_start(ap, format);
	len += vsprintf(&buf[0], format, ap);
	buf[len++] = '\n';
	buf[len++] = '\0';
	va_end(ap);
	
	if(_Access("debug.txt", 0) < 0){
		fp = _Fopen("debug.txt", "W");
	}else{
		fp = _Fopen("debug.txt", "RW");
	}

	_Lseek(fp, 0, 2);
	_Fwrite(buf, len, fp);
	_Fclose(fp);

	//_SendComStr(buf, len);
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
* 描  述：获取输入的字符串
* 参  数：inputSt - 输入框结构指针
* 返回值：uint8  - 输入后返回的按键 ： 上/下键，确认键，取消键
*/
uint8 GetInputNumStr(_GuiInputBoxStru * inputSt)
{
	static uint8 keyBuf[TXTBUF_LEN] = {0};
	uint8 key, idx, cleared = false;
	char keyStr[2] = {0};
	uint8 x = inputSt->left;
	uint8 y = inputSt->top;

	memcpy(keyBuf, inputSt->context, TXTBUF_LEN);
	_Printfxy(x, y, keyBuf, Color_White);
	_toxy(x, y + inputSt->hight);
	_ShowCur();
	idx = 0;

	while(1){
		key = _ReadKey();

		if(key >= KEY_0 && key <= KEY_9){

			if(inputSt->IsClear && cleared == false && idx == 0){
				memset(keyBuf, 0x00, TXTBUF_LEN);
				_GUIRectangleFill(inputSt->left, inputSt->top, 
					(inputSt->left + inputSt->width), 
					(inputSt->top + inputSt->hight), Color_White);
				cleared = true;
			}

			keyBuf[idx] = key;
			keyStr[0] = key;
			_Printfxy(x, y, keyStr, Color_White);
			if(idx != inputSt->datelen -1){
				idx++;
				x += 8;
			}
		}
		else if(key == KEY_LEFT){
			if(idx != 0){
				idx--;
				x -= 8;
			}
		}
		else if(key == KEY_RIGHT && keyBuf[idx] >= '0' && keyBuf[idx] <= '9'){
			if(idx != inputSt->datelen - 1){
				idx++;
				x += 8;
			}
		}
		else if(key == KEY_DELETE){
			if(idx == inputSt->datelen -1 && keyBuf[idx] != 0x00){
				keyBuf[idx] = 0x00;
				_Printfxy(x, y, " ", Color_White);
			}
			else if(idx > 0 && keyBuf[idx] == 0x00){
				keyBuf[idx -1] = 0x00;
				_Printfxy(x - 8, y, " ", Color_White);
				idx--;
				x -= 8;
			}
			else if(idx == 0 && keyBuf[idx] != 0x00){
				memset(keyBuf, 0x00, TXTBUF_LEN);
				_GUIRectangleFill(inputSt->left, inputSt->top, 
					(inputSt->left + inputSt->width), 
					(inputSt->top + inputSt->hight), Color_White);
			}
		}
		else if(key == KEY_UP || key == KEY_DOWN
			|| key == KEY_ENTER || key == KEY_CANCEL){
			break;
		}
		else{
			// do nothing
		}

		_toxy(x, y + inputSt->hight);
	}

	if(key != KEY_CANCEL){
		memcpy(inputSt->context, keyBuf, TXTBUF_LEN);
	}

	_HideCur();

	return key;
}

/*
* 描  述：创建输入框
* 参  数：x, y		- UI组件前面的标签的起始坐标
*		 item		- UI组件属性
*		 title		- UI组件前面的标签
*		 text		- 字符串缓冲区
*		 maxLen		- 最大字符串长度
*		 width		- 输入框长度
* 返回值：void
*/
void TextBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, char * text, uint8 maxLen, uint8 width)
{
	item->x = x;
	item->y = y;
	item->title = title;
	item->x1 = x + strlen(title) * 8;
	item->y1 = y;
	item->text = text;
	item->width = width;
	item->txtbox.dataLen = maxLen;
}

/*
* 描  述：获取输入框中的字符串
* 参  数：x, y		- UI组件前面的标签的起始坐标
*		 title		- UI组件前面的标签
*		 text		- 字符串缓冲区
*		 maxLen		- 最大字符串长度
* 返回值：uint8 - 输入完成后的按键值： 上/下键，确认键，取消键
*/
uint8 TextBoxGetStr(uint8 x, uint8 y, const char * title, char * text, uint8 maxLen)
{
	const char * ZeroAddr = "000000000000";
	static _GuiInputBoxStru NewInput;
	uint8 retKey = 0;

	// 输入框设置
	NewInput.top = y;
	NewInput.left = x + strlen(title) * 8;
	NewInput.width = 10 * 16;	// 不显示输入框，设为最大
	NewInput.hight = 16;
	NewInput.caption = "";
	NewInput.context = text;
	NewInput.type = 1;			// 数字
	NewInput.datelen = maxLen;	// 最大长度
	NewInput.keyUpDown = 1; 
	NewInput.IsClear = 1;
	_SetInputMode(1); 			//设置输入方式 
	_DisInputMode(1);			//输入法是否允许切换

	do{
		//接收输入
		retKey = GetInputNumStr(&NewInput);

		if( NewInput.context[0] >= '0' && NewInput.context[0] <= '9'){

			if(maxLen == 12 && (retKey == KEY_ENTER || retKey == KEY_UP || retKey == KEY_DOWN)){
				_leftspace(NewInput.context, maxLen, '0');
			}

			if(strncmp(ZeroAddr, NewInput.context, maxLen) == 0){
				sprintf(NewInput.context, "地址不能为0");
				retKey = 0xFF;
			}

			_Printfxy(NewInput.left, NewInput.top, NewInput.context, Color_White);
		}
	}while(retKey == 0xFF);

	return retKey;
}

/*
* 描  述：显示UI界面 
* 参  数：uiList	- UI组件列表结构
*		 *itemNo	- 初始化时定位到哪个输入项
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowUI(UI_ItemList uiList, uint8 *itemNo)
{
	uint8 key;
	uint8 i;
	UI_Item *ptr;

	for(i = 0; i < uiList.cnt; i++){
		ptr = &uiList.items[i];
		_Printfxy(ptr->x, ptr->y, ptr->title, Color_White);
		_Printfxy(ptr->x1, ptr->y1, ptr->text, Color_White);
	}

	(*itemNo) = ((*itemNo) > uiList.cnt -1 ? 0 : (*itemNo));

	while(1){

		ptr = &uiList.items[(*itemNo)];

		if(ptr->type == UI_TxtBox){
			key = TextBoxGetStr(ptr->x, ptr->y, ptr->title, ptr->text, ptr->txtbox.dataLen);
		}
		else if(ptr->type == UI_CombBox){

		}

		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}

		if(key == KEY_DOWN){
			if((*itemNo) < uiList.cnt -1){
				(*itemNo)++;
			}
			continue;
		}
		else if(key == KEY_UP){
			if((*itemNo) > 0){
				(*itemNo)--;
			}
			continue;
		}
		else{
			continue;
		}

	}

	// set the not-number str to null when back
	if(ptr->text[0] > '9' || ptr->text[0] < '0'){
		ptr->text[0] = 0x00;
	}

	return key;
}

/*
* 描  述：获取在屏幕x坐标开始显示多行字符串的行数
* 参  数：x			- 屏幕中显示的x坐标
		  buf		- 字符串起始地址
		  lines		- 每行的起始地址
* 返回值：uint8	 字符串总行数
*/
uint8 GetPrintLines(uint8 x, const char * buf, char * lines[])
{
	uint8 lineCnt = 0, col = 0; 
	uint8 * pr = buf;

	// first line
	lines[lineCnt] = pr;
	lineCnt++;

	// next lines
	while(*pr != 0x00){
		if(*pr == '\n' || (x + col * 8) > 160){
			
			col = 0;
			x = 0;
			pr = (*pr == '\n' ? pr + 1 : pr);

			lines[lineCnt] = pr;
			lineCnt++;

			if(*pr == 0x00){
				lineCnt--;
				break;
			}
		}
		pr++;
		col++;
	}

#if LogEnable
	LogToFile(LogName, " lineCnt: %d \n disp: %s", lineCnt, buf);
#endif

	return lineCnt;
}

/*
* 描  述：在屏幕 x,y 坐标显示多行字符串，可自动换行
* 参  数：x, y		- 屏幕中坐标
		  buf		- 字符串起始地址
		  maxLines	- 最多能显示的行数
* 返回值：void
*/
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf, uint8 maxLines)
{
	static uint8 dispLine[21] = {0};
	uint8 row = 0, col = 0; 
	uint8 * pcol, *prow;

	pcol = buf;

	for(row = 0; row < maxLines; row++){

		prow = pcol;

		for(col = x/8; col < 20; col++){
			if(col == 19 && *(pcol + 1) == '\n'){
				pcol++;
			}
			else if(*pcol == '\n'){
				pcol++;
				break;
			}
			else if(*pcol == 0x00){
				break;
			}
			pcol++;
		}
		memcpy(dispLine, prow, col);
		dispLine[col] = 0x00;
		_Printfxy(x, y, dispLine, Color_White);

		if(*pcol == 0x00){
			break;
		}

		col = 0;
		x = 0;
		y += 16;
	}
}

/*
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
	uint8 *pr = buf, col = 0; 
	va_list ap;

	va_start(ap, format);
	len = vsprintf(buf, format, ap);
	buf[len] = '\0';
	va_end(ap);

	// first line
	_Printfxy(x, y, pr, 0);
	
	// next lines
	len = 0;
	while(*pr != '\0'){
		if(*pr == '\n' || (x + col * 8) > 160){
			col = 0;
			x = 0;
			y += 16;
			pr = (*pr == '\n' ? pr + 1 : pr);

			if(*pr == '\0'){
				break;
			}
			_Printfxy(x, y, pr, Color_White);
		}
		pr++;
		col++;
	}
}

/*
* 描  述：在屏幕 x,y 坐标显示实心三角形
* 参  数：x, y		- 屏幕中坐标
		  direction	- 三角形方向：0 - 向上 ▲ ， 1 - 向下 ▼
* 返回值：void
*/
void PrintXyTriangle(uint8 x, uint8 y, uint8 direction)
{
	uint8 x1, y1;

	if(direction == 0){
		x1 = 15/2;
		for(y1 = 0; y1 < 15/2 + 1; y1++){
			_GUIHLine(x + x1, y + y1, x + 15 - x1, Color_Black);
			x1--;
		}
	}else{
		x1 = 0;
		for(y1 = 0; y1 < 15/2 + 1; y1++){
			_GUIHLine(x + x1, y + y1, x + 15 - x1, Color_Black);
			x1++;
		}
	}
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
	const char decHex[16] = {'0', '1', '2', '3','4', '5', '6', '7','8', '9', 'A', 'B','C', 'D', 'E', 'F'};
	
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
		  iLength - 数组中可保存最大长度
		  strHex - 源字符串缓冲区地址
		  separate - 字符串中Hex字节之间的间隔符：0 - 无间隔符， 其他字符 - 如空格或逗号
		  reverse - 是否需要倒序：false - 不倒序， true - 倒序
* 返回值：int - 转换后的字节数：0 - 转换失败
*/
int GetBytesFromStringHex(uint8 bytes[], int iStart, int iLength, const char * strHex, char separate, bool reverse)
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

	while (iLoop < strHexLen - 1 && index < iLength)
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