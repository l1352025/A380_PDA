#include "HJLIB.H"
#include "stdarg.h"
#include "string.h"
#include "common.h"

// --------------------------------  全局变量  -----------------------------------------
//char Screenbuff[160*(160/3+1)*2]; 
#if Upgrd_FileBuf_Enable
uint8 DispBuf[128 * 1024];					// 4k ~ 128K
#else
uint8 DispBuf[14 * 1024];					// 4k ~ 14K
#endif
uint8 * const LogBuf = &DispBuf[4096];     	// 4k ~ 
uint8 * const TmpBuf = &DispBuf[8192];     	// 2K ~ 
uint8 * const BackupBuf = &DispBuf[10240];	// 2k ~ 
uint8 * const ArgBuf = &DispBuf[12288];		// 2k ~ 
#if Upgrd_FileBuf_Enable
uint8 * const FileBuf = &DispBuf[14336];	// 116k 
#endif
uint8 TxBuf[1024];
uint8 RxBuf[1024];
uint32 RxLen, TxLen;
const uint8 LocalAddr[10] = { 0x20, 0x19, 0x00, 0x00, 0x20, 0x19, 0x00, 0x00, 0x00, 0x00};	// 本机地址 2019000020190000，10/12/16字符
const uint8 BroadAddr[10] = { 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0xD4, 0x00, 0x00};	// 广播地址 D4D4D4D4D4D4D4D4，10/12/16字符
uint8 DstAddr[10];
uint8 VerInfo[42];
uint16 CurrCmd;
char CurrCmdName[64];
ParamsBuf Addrs;		
ParamsBuf Args;
char StrBuf[TXTBUF_MAX][TXTBUF_LEN];    // extend input buffer
char StrDstAddr[TXTBUF_LEN];
char StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
UI_ItemList UiList;
bool LcdOpened;
bool IsNoAckCmd;

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
* 描  述：在数组中查找子数组，可指定查找的起始位置和范围
* 参  数：srcArray - 源数组地址， srcLen - 源数组长度
*		  dstBytes - 目的数组地址， dstLen - 目的数组长度 
*		  startIndex - 源数组查找的起始位置，offset - 查找的范围
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
* 描  述：打印日志到文件或串口, 通信串口在收发时不可打印到串口
*   1. LOG_ON && LogScom_On = 0  输出到日志文件 LogFileName "debug.txt" 
*	2. LOG_ON && LogScom_On = 1  输出到串口 LogPort 波特率为 "115200,E,8,1"
*	3. LOG_ON == 0 				 不输出日志
* 参  数：buf - 数据缓冲区
*		  len - 字节数
* 返回值：void
*/
void LogWrite(uint8 *buf, uint32 len)
{
#if LOG_ON
#if !(LogScom_On)
	int fp;
#endif

#if LogScom_On
	_CloseCom();
	_ComSetTran(LogPort);
	_ComSet((uint8 *)"115200,E,8,1", 2);
	_SendComStr(buf, len);
	_CloseCom();
#else
	if(_Access(LogFileName, 0) < 0){
		fp = _Fopen(LogFileName, "W");
	}else{
		fp = _Fopen(LogFileName, "RW");
	}
	
	if(_Filelenth(fp) > LogFileSize){		// 大于最大字节数时，重建日志文件
		if(_Access("debug.bak", 0) == 0){
			_Remove("debug.bak");
		}
		_Rename(LogFileName, "debug.bak");
		fp = _Fopen(LogFileName, "W");
	}
	_Lseek(fp, 0, 2);
	_Fwrite(buf, len, fp);
	_Fclose(fp);
#endif	// LogScom_On
#endif	// LOG_ON
}

/*
* 描  述：打印日志到文件或串口, 通信串口在收发时不可打印到串口
* 参  数：format - 字符串格式
*		  ...  - 可变参数
* 返回值：void
*/
void LogPrint(const char * format, ...)
{
#if LOG_ON
	uint32 len = 0; 
	va_list ap;
	uint8 *buf;
	char time[24];

	// buf = (uint8 *) _malloc(2048);	// 1.使用动态内存时
	buf = &LogBuf[0];					// 2.静态内存

	_GetDateTime(time, '-', ':');
	len += sprintf(&buf[len], "[%s] ", time);
	va_start(ap, format);
	len += vsprintf(&buf[len], format, ap);
	buf[len++] = '\n';
	va_end(ap);
	
	LogWrite(buf, len);

	//_free(buf);						// 使用动态内存时
#endif
}

/*
* 描  述：打印日志到文件或串口, 通信串口在收发时不可打印到串口
* 参  数：title	 - 标题
*		  buf	- 字节数组起始地址
*		  size	- 打印的字节数， 最大1024
* 返回值：void
*/
void LogPrintBytes(const char *title, uint8 *buf, uint16 size)
{
#if LOG_ON
	const char decHex[16] = {'0', '1', '2', '3','4', '5', '6', '7','8', '9', 'A', 'B','C', 'D', 'E', 'F'};
	uint16 i = 0;
	uint8 *tmp;
	char time[24];

	// tmp = (uint8 *) _malloc(2048);	// 1.使用动态内存时
	tmp = &LogBuf[0];					// 2.静态内存
	
	_GetDateTime(time, '-', ':');
	i += sprintf(&tmp[i], "[%s] %s [%d]\t", time, title, size);
	while(size--){
		tmp[i++] = decHex[*buf >> 4];
		tmp[i++] = decHex[*buf & 0x0F];
		tmp[i++] = ' ';
		buf++;
	}
	tmp[i++] = '\n';

	LogWrite(tmp, i);

	//_free(tmp);					// 使用动态内存时
#endif
}

/*
* 函数名：ShowProgressBar
* 描  述：显示进度条
* 参  数：y - 进度条y坐标，将显示在(0,y)位置，固定宽度为160，固定高度为16,黑色填充
*		  maxValue - 进度条最大值
*		  currValue - 进度条当前值
* 返回值：int 等于-1: 未找到， 大于/等于0 : 目的数组在源数组中的起始索引
*/
void ShowProgressBar(uint8 y, uint32 maxValue, uint32 currValue)
{
	uint32 width = (currValue >= maxValue? 160 : 160*currValue/maxValue);
	_GUIRectangleFill(0, y, width, y + 16, Color_Black);
}

/*
* 描  述：获取输入的字符串
* 参  数：uiItem - Ui组件结构指针
* 返回值：uint8  - 退出Ui组件时返回的按键 ： 上/下键，确认键，取消键
*/
static uint8 GetInputNumStr(UI_Item *uiItem)
{
	static uint8 keyBuf[TXTBUF_LEN] = {0};
	uint8 key, cleared = false;
	char keyStr[2] = {0};
	int idx = 0;
	int x = uiItem->x1;
	int y = uiItem->y1;

	memcpy(keyBuf, uiItem->text, TXTBUF_LEN);
	_Printfxy(x, y, keyBuf, Color_White);
	
	if((uiItem->ui.txtbox.dataLen == AddrLen * 2)
		&&(uiItem->text[0] >= '0' && uiItem->text[0] <= '9')){
		idx = strlen(uiItem->text);
		idx = (idx > 0 ? idx - 1 : idx);
	}
	x = x + idx * 8;
	_toxy(x, y + uiItem->height);
	_ShowCur();
	

	while(1){
		key = _ReadKey();

		if((key >= KEY_0 && key <= KEY_9) 
			||(key == KEY_DOT && uiItem->ui.txtbox.dotEnable && idx > 0 && idx < uiItem->ui.txtbox.dataLen -1)){

			if(uiItem->ui.txtbox.isClear && cleared == false && idx == 0){
				memset(keyBuf, 0x00, TXTBUF_LEN);
				_GUIRectangleFill(uiItem->x1, uiItem->y1, 
					(uiItem->x1 + uiItem->width), 
					(uiItem->y1 + uiItem->height), Color_White);
				cleared = true;
			}

			keyBuf[idx] = key;
			keyStr[0] = key;
			_Printfxy(x, y, keyStr, Color_White);
			if(idx != uiItem->ui.txtbox.dataLen -1){
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
		else if(key == KEY_RIGHT 
			&& ((keyBuf[idx] >= '0' && keyBuf[idx] <= '9') || (keyBuf[idx] == '.'))){
			if(idx != uiItem->ui.txtbox.dataLen - 1){
				idx++;
				x += 8;
			}
		}
		else if(key == KEY_DELETE){
			if(false == uiItem->ui.txtbox.isClear){
				keyBuf[idx] = '0';
				_Printfxy(x, y, "0", Color_White);
				idx--;
				x -= 8;
			}
			else if(idx == uiItem->ui.txtbox.dataLen -1 && keyBuf[idx] != 0x00){
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
				_GUIRectangleFill(uiItem->x1, uiItem->y1, 
					(uiItem->x1 + uiItem->width), 
					(uiItem->y1 + uiItem->height), Color_White);
			}

			if(idx < 0) idx = 0;
			if(x < uiItem->x1) x = uiItem->x1;
		}
		else if(key == KEY_UP || key == KEY_DOWN
			|| key == KEY_ENTER || key == KEY_CANCEL){
			break;
		}
		else{
			// do nothing
		}

		_toxy(x, y + uiItem->height);
	}

	if(key != KEY_CANCEL){
		memcpy(uiItem->text, keyBuf, TXTBUF_LEN);
	}

	_HideCur();

	return key;
}

/*
* 描  述：获取选项列表框当前选项
* 参  数：uiItem - Ui组件结构指针
* 返回值：uint8  - 退出Ui组件时返回的按键 ： 上/下键，确认键，取消键
*/
static uint8 CombBoxGetCurrIndex(UI_Item *uiItem)
{
	uint8 key, x, y, isShowIcon = 1, cnt = 0;
	int8 idx, lastIdx = 127;

	idx = *uiItem->ui.combox.currIdx;
	
	while(1){

		key = _GetKeyExt();

		if(idx != lastIdx){
			uiItem->text = uiItem->ui.combox.strs[idx];
			x = uiItem->x1 + (uiItem->width - strlen(uiItem->text)*8)/2;
			y = uiItem->y1;
			_GUIRectangleFill(uiItem->x1 + 16, y, uiItem->x1 + uiItem->width - 16, y + 16, Color_White);
			_Printfxy(x, y, uiItem->text, Color_White);
			lastIdx = idx;
		}

		if(isShowIcon && cnt == 3){
			_Printfxy(uiItem->x1, uiItem->y1, "<<", Color_Black);
			_Printfxy(uiItem->x1 + uiItem->width - 16, uiItem->y1, ">>", Color_Black);
			isShowIcon = 0;
			cnt = 0;
		}else if(!isShowIcon && cnt == 3){
			_Printfxy(uiItem->x1, uiItem->y1, "  ", Color_White);
			_Printfxy(uiItem->x1 + uiItem->width - 16, uiItem->y1, "  ", Color_White);
			isShowIcon = 1;
			cnt = 0;
		}
		_Sleep(100);
		cnt++;

		if(key == KEY_LEFT){
			idx--;
			if(idx < 0){
				idx = uiItem->ui.combox.cnt -1;
			}
		}
		if(key == KEY_RIGHT){
			idx++;
			if(idx > uiItem->ui.combox.cnt -1){
				idx = 0;
			}
		}
		else if(key == KEY_UP || key == KEY_DOWN
			|| key == KEY_ENTER || key == KEY_CANCEL){
			break;
		}
		else{
			// do nothing
		}
	}

	_Printfxy(uiItem->x1, uiItem->y1, "<<", Color_White);
	_Printfxy(uiItem->x1 + uiItem->width - 16, uiItem->y1, ">>", Color_White);
	*uiItem->ui.combox.currIdx = idx;

	return key;
}

/*
* 描  述：创建列表视图
* 参  数：lbx		- 列表视图结构指针
*		  totalCnt	- 记录总数
*		  maxRow	- 一页显示记录条数
*		  fillStrsFunc	- 填充列表的回调函数
*		  title		- 列表标题
*		  strCnt	- 字符串数量
*		  ...		- 字符串列表
* 返回值：void
*/
void ListBoxCreate(ListBox *lbx, uint8 x, uint8 y, uint8 maxCol, uint8 maxRow, uint16 totalCnt, FillListFunc fillStrsFunc, const char *title, uint32 strCnt, ...)
{
	uint16 i;
	va_list ap;
	char *ptr;

	lbx->x = x;		// 默认 0
	lbx->y = y;		// 默认 0
	lbx->maxCol = maxCol;	// 默认 20
	lbx->maxRow = maxRow;	// 默认 7
	lbx->totalCnt = totalCnt;
	lbx->fillStrsFunc = fillStrsFunc;
	lbx->title = title;
	lbx->currIdx = 0;

	// init value
	va_start(ap, strCnt);
	lbx->strCnt = (uint16)strCnt;
	for(i = 0; i < lbx->strCnt; i++){
		ptr = va_arg(ap, char *);
		if(ptr == NULL){
			break;
		}
		lbx->str[i] = ptr;
	}
	
}

/*
* 描  述：创建列表视图-扩展
* 参  数：lbx		- 列表视图结构指针
* 		  x			- x 坐标
*		  y			- y 坐标
*		  maxCol	- 一行显示字符个数
*		  maxRow	- 一页显示记录条数
*		  totalCnt	- 记录总数
*		  fillStrsFunc	- 填充列表的回调函数
*		  title		- 列表标题
*		  strs		- 字符串列表缓冲区
*		  strCnt	- 字符串数量
* 返回值：void
*/
void ListBoxCreateEx(ListBoxEx *lbx, uint8 x, uint8 y, uint8 maxCol, uint8 maxRow, uint16 totalCnt, FillListFunc fillStrsFunc, const char *title, char **strs, uint8 strLen, uint32 strCnt)
{
	lbx->x = x;		// 默认 0
	lbx->y = y;		// 默认 0
	lbx->maxCol = maxCol;	// 默认 20
	lbx->maxRow = maxRow;	// 默认 7
	lbx->totalCnt = totalCnt;
	lbx->fillStrsFunc = fillStrsFunc;
	lbx->title = title;
	lbx->str = strs;
	lbx->strLen = strLen;
	lbx->strCnt = (uint16)strCnt;
	lbx->currIdx = 0;
	lbx->strIdx = 0;
}

/*
* 描  述：显示列表并获取当前选项
* 参  数：lbx	- 列表视图结构指针
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowListBox(ListBox *lbx)
{
	uint16 dstIndex, srcIndex, currIndex;
	uint8 key, i;
	int16 x1, y1, recX, recY, recX1, recY1, fillX, fillY, fillX1, fillY1;
	uint8 **lines = lbx->str;
	uint16 fillMax = (lbx->strCnt >= lbx->totalCnt ? lbx->totalCnt : (lbx->strCnt - lbx->strCnt % lbx->maxRow));
	uint16 fillCnt = 0;
	char title[21], temp[15];

	lbx->currIdx = (lbx->currIdx >= lbx->totalCnt ? 0 : lbx->currIdx);
	lbx->strCnt = (lbx->totalCnt < fillMax ? lbx->totalCnt : fillMax);
	lbx->strIdx = (lbx->currIdx % fillMax);
	lbx->dispStartIdx = lbx->strIdx - (lbx->strIdx % lbx->maxRow);


	recX = lbx->x - 4;
	recY = lbx->y - 4;
	x1 = lbx->x + lbx->maxCol * 8;
	y1 = lbx->y + (lbx->maxRow + 2) * 16;
	recX1 = x1 + 4;
	recY1 = y1 - 4;
	fillX = (recX <= 0 ? 0 : recX);
	fillY = (recY <= 0 ? 0 : recY);
	fillX1 = (recX1 >= 160 ? 160 : recX1);
	fillY1 = recY1 + 4;
	
	// 上/下滚动显示   ▲ ▼  △ ▽
	while(1){
	
		_GUIRectangleFill(fillX, fillY, fillX1, fillY1, Color_White);	// 清空区域
		_GUIRectangle(recX, recY, recX1, recY1, Color_Black);			// 绘制方框

		// 显示标题：lbx->title  n/m
		//-------------------------------------------------
		currIndex = (lbx->totalCnt == 0 ? 0 : lbx->currIdx + 1);
		sprintf(temp, "%d/%d", currIndex, lbx->totalCnt);
		StringPadLeft(temp, (lbx->maxCol - strlen(lbx->title)), ' ');
		sprintf(title, "%s%s", lbx->title, temp);	
		_Printfxy(lbx->x, lbx->y, title, Color_White);
		_GUIHLine(lbx->x, lbx->y + 16 + 4, x1, Color_Black);	
		//--------------------------------------------▼---
		if(lbx->totalCnt == 0){
			_Printfxy(lbx->x, lbx->y + 16 + 8, "列表为空！", Color_White);
		}
		else{
			for(i = 0; i < lbx->maxRow && (lbx->dispStartIdx + i) < lbx->strCnt; i++){
				_Printfxy(lbx->x, i * 16 + lbx->y + 16 + 8, lines[lbx->dispStartIdx + i], Color_White);
			}
			_Printfxy(lbx->x, (lbx->strIdx - lbx->dispStartIdx) * 16 + lbx->y + 16 + 8, lines[lbx->strIdx], Color_Black);
		}
		//--------------------------------------------▲---
		_GUIHLine(lbx->x, recY1, x1, Color_Black);	

		key = _ReadKey();

		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}

		if(key == KEY_UP && lbx->totalCnt > 0){
			lbx->strIdx--;
			lbx->currIdx--;
		}
		else if(key == KEY_DOWN && lbx->totalCnt > 0){
			lbx->strIdx++;
			lbx->currIdx++;
		}
		else if(key == KEY_LEFT && lbx->totalCnt > 0){
			lbx->strIdx -= lbx->maxRow;
			lbx->currIdx -= lbx->maxRow;
		}
		else if(key == KEY_RIGHT && lbx->totalCnt > 0){
			lbx->strIdx += lbx->maxRow;
			lbx->currIdx += lbx->maxRow;
		}
		else{
			continue;
		}

		// record index check
		lbx->isCircle = 1;
		if(lbx->currIdx < 0){
			lbx->currIdx = (lbx->isCircle ? lbx->totalCnt - 1 : 0);
		}
		else if(lbx->currIdx > lbx->totalCnt - 1){
			lbx->currIdx = (lbx->isCircle ? 0 : lbx->totalCnt - 1);
		}

		// disp index check 	and 	re-fill strs
		if(lbx->strIdx < 0){

			if(lbx->totalCnt > fillMax){
				fillCnt = (lbx->currIdx % fillMax) + 1;
				srcIndex = lbx->currIdx + 1 - fillCnt;
				dstIndex = 0;
				lbx->strCnt = fillCnt;
			}
			lbx->strIdx = lbx->strCnt - 1;
			
			if(lbx->fillStrsFunc != NULL && fillCnt > 0){
				(*lbx->fillStrsFunc)(lbx->str, dstIndex, srcIndex, fillCnt);
				fillCnt = 0;
			}
		}
		else if(lbx->strIdx > lbx->strCnt - 1){

			if(lbx->totalCnt > fillMax){
				fillCnt = (lbx->totalCnt - lbx->currIdx >= fillMax ? fillMax : (lbx->totalCnt - lbx->currIdx));
				srcIndex = lbx->currIdx;
				dstIndex = 0;
				lbx->strCnt = fillCnt;
			}
			lbx->strIdx = 0;
			
			if(lbx->fillStrsFunc != NULL && fillCnt > 0){
				(*lbx->fillStrsFunc)(lbx->str, dstIndex, srcIndex, fillCnt);
				fillCnt = 0;
			}
		}

		lbx->dispStartIdx = lbx->strIdx - (lbx->strIdx % lbx->maxRow);

	}

	return key;
}

/*
* 描  述：显示列表并获取当前选项 - 扩展
* 参  数：lbx	- 列表视图结构指针
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowListBoxEx(ListBoxEx *lbx)
{
	uint16 dstIndex, srcIndex, currIndex;
	uint8 key, i;
	int16 x1, y1, recX, recY, recX1, recY1, fillX, fillY, fillX1, fillY1;
	char *lines = (char *)lbx->str;
	uint16 fillMax = (lbx->strCnt >= lbx->totalCnt ? lbx->totalCnt : (lbx->strCnt - lbx->strCnt % lbx->maxRow));
	uint16 fillCnt = 0;
	char title[21], temp[15];

	lbx->currIdx = (lbx->currIdx >= lbx->totalCnt ? 0 : lbx->currIdx);
	lbx->strCnt = (lbx->totalCnt < fillMax ? lbx->totalCnt : fillMax);
	lbx->strIdx = (lbx->currIdx % fillMax);
	lbx->dispStartIdx = lbx->strIdx - (lbx->strIdx % lbx->maxRow);


	recX = lbx->x - 4;
	recY = lbx->y - 4;
	x1 = lbx->x + lbx->maxCol * 8;
	y1 = lbx->y + (lbx->maxRow + 2) * 16;
	recX1 = x1 + 4;
	recY1 = y1 - 4;
	fillX = (recX <= 0 ? 0 : recX);
	fillY = (recY <= 0 ? 0 : recY);
	fillX1 = (recX1 >= 160 ? 160 : recX1);
	fillY1 = recY1 + 4;
	
	// 上/下滚动显示   ▲ ▼  △ ▽
	while(1){
	
		_GUIRectangleFill(fillX, fillY, fillX1, fillY1, Color_White);	// 清空区域
		_GUIRectangle(recX, recY, recX1, recY1, Color_Black);			// 绘制方框

		// 显示标题：lbx->title  n/m
		//-------------------------------------------------
		currIndex = (lbx->totalCnt == 0 ? 0 : lbx->currIdx + 1);
		sprintf(temp, "%d/%d", currIndex, lbx->totalCnt);
		StringPadLeft(temp, (lbx->maxCol - strlen(lbx->title)), ' ');
		sprintf(title, "%s%s", lbx->title, temp);	
		_Printfxy(lbx->x, lbx->y, title, Color_White);
		_GUIHLine(lbx->x, lbx->y + 16 + 4, x1, Color_Black);	
		//--------------------------------------------▼---
		if(lbx->totalCnt == 0){
			_Printfxy(lbx->x, lbx->y + 16 + 8, "列表为空！", Color_White);
		}
		else{
			for(i = 0; i < lbx->maxRow && (lbx->dispStartIdx + i) < lbx->strCnt; i++){
				_Printfxy(lbx->x, i * 16 + lbx->y + 16 + 8,
					lines + (lbx->dispStartIdx + i) * lbx->strLen, Color_White);
			}
			_Printfxy(lbx->x, (lbx->strIdx - lbx->dispStartIdx) * 16 + lbx->y + 16 + 8,
				lines + lbx->strIdx * lbx->strLen, Color_Black);
		}
		//--------------------------------------------▲---
		_GUIHLine(lbx->x, recY1, x1, Color_Black);	

		key = _ReadKey();

		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}

		if(key == KEY_UP && lbx->totalCnt > 0){
			lbx->strIdx--;
			lbx->currIdx--;
		}
		else if(key == KEY_DOWN && lbx->totalCnt > 0){
			lbx->strIdx++;
			lbx->currIdx++;
		}
		else if(key == KEY_LEFT && lbx->totalCnt > 0){
			lbx->strIdx -= lbx->maxRow;
			lbx->currIdx -= lbx->maxRow;
		}
		else if(key == KEY_RIGHT && lbx->totalCnt > 0){
			lbx->strIdx += lbx->maxRow;
			lbx->currIdx += lbx->maxRow;
		}
		else{
			continue;
		}

		// record index check
		lbx->isCircle = 1;
		if(lbx->currIdx < 0){
			lbx->currIdx = (lbx->isCircle ? lbx->totalCnt - 1 : 0);
		}
		else if(lbx->currIdx > lbx->totalCnt - 1){
			lbx->currIdx = (lbx->isCircle ? 0 : lbx->totalCnt - 1);
		}

		// disp index check 	and 	re-fill strs
		if(lbx->strIdx < 0){

			if(lbx->totalCnt > fillMax){
				fillCnt = (lbx->currIdx % fillMax) + 1;
				srcIndex = lbx->currIdx + 1 - fillCnt;
				dstIndex = 0;
				lbx->strCnt = fillCnt;
			}
			lbx->strIdx = lbx->strCnt - 1;
			
			if(lbx->fillStrsFunc != NULL && fillCnt > 0){
				(*lbx->fillStrsFunc)(lbx->str, dstIndex, srcIndex, fillCnt);
				fillCnt = 0;
			}
		}
		else if(lbx->strIdx > lbx->strCnt - 1){

			if(lbx->totalCnt > fillMax){
				fillCnt = (lbx->totalCnt - lbx->currIdx >= fillMax ? fillMax : (lbx->totalCnt - lbx->currIdx));
				srcIndex = lbx->currIdx;
				dstIndex = 0;
				lbx->strCnt = fillCnt;
			}
			lbx->strIdx = 0;
			
			if(lbx->fillStrsFunc != NULL && fillCnt > 0){
				(*lbx->fillStrsFunc)(lbx->str, dstIndex, srcIndex, fillCnt);
				fillCnt = 0;
			}
		}

		lbx->dispStartIdx = lbx->strIdx - (lbx->strIdx % lbx->maxRow);

	}

	return key;
}

/*
* 描  述：创建标签
* 参  数：x, y		- UI组件前面的标签的起始坐标
*		 item		- UI组件属性
*		 title		- UI组件前面的标签
* 返回值：void
*/
void LableCreate(UI_Item *item, uint8 x, uint8 y, const char *title)
{
	item->x = x;
	item->y = y;
	item->title = title;
	item->x1 = x + strlen(title) * 8;
	item->y1 = y;
	item->text = NULL;
	item->type = UI_Lable;
}

/*
* 描  述：创建按钮
* 参  数：x, y		- UI组件前面的标签的起始坐标
*		 item		- UI组件属性
*		 title		- UI组件前面的标签
* 返回值：void
*/
void ButtonCreate(UI_Item *item, uint8 x, uint8 y, const char *title)
{
	item->x = x;
	item->y = y;
	item->title = title;
	item->x1 = x + strlen(title) * 8;
	item->y1 = y;
	item->text = NULL;
	item->type = UI_Button;
}

/*
* 描  述：创建输入框
* 参  数：x, y		- UI组件前面的标签的起始坐标
*		 item		- UI组件属性
*		 title		- UI组件前面的标签
*		 text		- 字符串缓冲区
*		 maxLen		- 最大字符串长度
*		 width		- 输入框长度
*		 isClear	- 输入时是否清空
* 返回值：void
*/
void TextBoxCreate(UI_Item *item, uint8 x, uint8 y, const char *title, char *text, uint8 maxLen, uint8 width, bool isClear)
{
	item->x = x;
	item->y = y;
	item->title = title;
	item->x1 = x + strlen(title) * 8;
	item->y1 = y;
	item->text = text;
	item->width = width;
	item->height = 16;
	item->type = UI_TxtBox;
	item->ui.txtbox.dataLen = maxLen;
	item->ui.txtbox.isClear = isClear;
	item->ui.txtbox.dotEnable = 0;		// 默认不允许输入'.' , 如需输入则在textbox创建后设置该标志为1
	item->ui.txtbox.isInputAny = 0;		// 默认只允许输入数字，如需输入其它任何字符则在textbox创建后设置该标志为1
}

/*
* 描  述：创建选项框 ，左右键切换选项
* 参  数：x, y		- UI组件前面的标签的起始坐标
*		 item		- UI组件属性
*		 title		- UI组件前面的标签
*		 currIdx	- 当前选项字符串索引 0~9
*		 maxCnt		- 最大选项字符串数量 1~10
*		 ...		- N个选项字符串
* 返回值：void
*/
void CombBoxCreate(UI_Item *item, uint8 x, uint8 y, const char *title, uint8 *currIdx, uint32 maxCnt, ...)
{
	va_list ap;
	uint8 i;
	char *ptr;

	va_start(ap, maxCnt);
	for(i = 0; i < maxCnt && i < CbxItem_Max; i++){
		ptr = va_arg(ap, char *);
		if(ptr == NULL){
			break;
		}
		item->ui.combox.strs[i] = ptr;
	}

	if(*currIdx > i -1){
		*currIdx = 0;
	}

	item->x = x;
	item->y = y;
	item->title = title;
	item->x1 = x + strlen(title) * 8;
	item->y1 = y;
	item->text = NULL;
	item->width = 160 - item->x1;
	item->height = 16;
	item->type = UI_CombBox;
	item->ui.combox.cnt = (uint8)i;
	item->ui.combox.currIdx = currIdx;
}

/*
* 描  述：显示UI界面 
* 参  数：uiList	- UI组件列表结构
*		 *itemNo	- 初始化时定位到哪个输入项
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowUI(UI_ItemList uiList, uint8 *itemNo)
{
	const char * ZeroAddr = "0000000000000000";
	uint8 keyBuf[TXTBUF_LEN];
	uint8 key, x, y;
	uint8 i;
	UI_Item *ptr;
	_GuiInputBoxStru inputSt;

	for(i = 0; i < uiList.cnt; i++){
		ptr = &uiList.items[i];
		_Printfxy(ptr->x, ptr->y, ptr->title, Color_White);
		_Printfxy(ptr->x1, ptr->y1, ptr->text, Color_White);

		if(ptr->type == UI_CombBox){
			ptr->text = ptr->ui.combox.strs[*ptr->ui.combox.currIdx];
			x = ptr->x1 + (ptr->width - strlen(ptr->text)*8)/2;
			y = ptr->y1;
			_Printfxy(x, y, ptr->text, Color_White);
			_Printfxy(ptr->x1, ptr->y1, "<<", Color_White);
			_Printfxy(ptr->x1 + ptr->width - 16, ptr->y1, ">>", Color_White);
		}
	}

	(*itemNo) = ((*itemNo) > uiList.cnt -1 ? 0 : (*itemNo));

	if(uiList.items[(*itemNo)].type == UI_Lable){
		key = KEY_DOWN;
	}

	while(1){

		ptr = &uiList.items[(*itemNo)];

		if(ptr->type == UI_TxtBox){
			do{
				//接收输入
				if(ptr->ui.txtbox.isInputAny){
					memcpy(keyBuf, ptr->text, TXTBUF_LEN);
					inputSt.left = ptr->x1;
					inputSt.top = ptr->y1;
					inputSt.width = ptr->width;
					inputSt.hight = ptr->height;
					inputSt.caption = "";
					inputSt.context = keyBuf;
					inputSt.datelen = ptr->ui.txtbox.dataLen;
					inputSt.IsClear = ptr->ui.txtbox.isClear;
					inputSt.keyUpDown = true;
					inputSt.type = 3;	
					_SetInputMode(3);	// 3 - 大写字母
					_DisInputMode(1);	// 允许输入法切换

					key = _GetStr(&inputSt);
					_HideCur();

					if(key != KEY_CANCEL && keyBuf[0] != 0x00){
						memcpy(ptr->text, keyBuf, TXTBUF_LEN);
					}
				}
				else{
					_SetInputMode(1);	// 1 - 数字
					key = GetInputNumStr(ptr);
				}

				if( ptr->text[0] >= '0' && ptr->text[0] <= '9'){
					if((ptr->ui.txtbox.dataLen == AddrLen * 2)
						&& (key == KEY_ENTER || key == KEY_UP || key == KEY_DOWN)){
						_leftspace(ptr->text, ptr->ui.txtbox.dataLen, '0');
						if(strncmp(ZeroAddr, ptr->text, ptr->ui.txtbox.dataLen) == 0){
							sprintf(ptr->text, " 不能为0 ");
							key = 0xFF;
						}
					}
				}
				_Printfxy(ptr->x1, ptr->y1, ptr->text, Color_White);

			}while(key == 0xFF);
		}
		else if(ptr->type == UI_CombBox){
			key = CombBoxGetCurrIndex(ptr);
		}
		else if(ptr->type == UI_Button){
			_Printfxy(ptr->x, ptr->y, ptr->title, Color_Black);
			key = _ReadKey();
			_Printfxy(ptr->x, ptr->y, ptr->title, Color_White);
		}
		else{	// UI_Lable
		}

		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}

		if(key == KEY_DOWN){
			if((*itemNo) < uiList.cnt -1){
				(*itemNo)++;
			}else{
				(*itemNo) = 0;
			}
			continue;
		}
		else if(key == KEY_UP){
			if((*itemNo) > 0){
				(*itemNo)--;
			}else{
				(*itemNo) = uiList.cnt -1;
			}
			continue;
		}
		else{
			continue;
		}

	}

	// set the not-number str to null when back
	if(ptr->type == UI_TxtBox 
		&& ptr->text[0] != 'D'	/* D4D4D4D4D4D4D4D4 */
		&& (ptr->text[0] > '9' || ptr->text[0] < '0')){
		ptr->text[0] = 0x00;
	}

	return key;
}

/*
* 描  述：显示提示消息框，等待 n ms后返回
* 参  数：
*		x ，y - 消息框在屏幕中的坐标 x, y
*		str - 显示的字符串
*		waitMs - 等待的毫秒数
* 返回值：void
*/
void ShowMsg(uint8 x, uint8 y, char *str, uint16 waitMs)
{
	uint8 strTmp[20], i, j, rowCnt, colCnt;
	uint16 len = strlen(str);	

	if(x < 8) x = 8;
	if(y < 8) y = 8;
	
	colCnt = ((160 - x - x) / 16) * 2;
	rowCnt = (len + colCnt - 1) / colCnt;

	if(y + rowCnt * 16 > 160){
		rowCnt = (160 - y) / 16;
	}
	
	_GUIRectangleFill(x - 8, y - 8, x + colCnt * 8 + 8, y + rowCnt * 16 + 8, Color_White);
	_GUIRectangle(x - 8, y - 8, x + colCnt * 8 + 8, y + rowCnt * 16 + 8, Color_Black);
	
	for(i = 0, j = 0; i < rowCnt; i++){
		memcpy(strTmp, &str[j], colCnt);
		strTmp[colCnt] = 0x00;
		j += colCnt;
		_Printfxy(x, y + i * 16, strTmp, Color_White);
	}

	_Sleep(waitMs);
}

/*
* 描  述：获取在屏幕x坐标开始显示多行字符串的行数
* 参  数：x			- 屏幕中显示的x坐标
*		  buf		- 字符串起始地址
*		  lines		- 每行的起始地址
* 返回值：uint8	 字符串总行数
*/
uint8 GetPrintLines(uint8 x, const char * buf, char * lines[])
{
	uint8 lineCnt = 0, col = 0; 
	uint8 *pcurrLine, *pnextLine, *pr;

	lines[0] = NULL;
	pcurrLine = buf;
	pr = buf;

	while(*pr != 0x00){
		if((x/8 + col) >= 20){
			if(col == 21){
				pr -= 2;
			}
			else{
				pr = (*pr == '\n' ? pr + 1 : pr);
			}
			pnextLine = pr;
			
			lines[lineCnt++] = pcurrLine;
			pcurrLine = pnextLine;

			col = 0;
			x = 0;
		}
		else if(*pr == '\n'){
			pr++;
			pnextLine = pr;

			lines[lineCnt++] = pcurrLine;
			pcurrLine = pnextLine;

			col = 0;
			x = 0;
		}

		if(*pr == 0x00){
			break;
		}
		else if(*(pr) > 0x80){
			pr += 2;
			col += 2;
		}
		else{
			pr++;
			col++;
		}
		
	}

	// last line 
	if(*pcurrLine != 0x00){
		lines[lineCnt++] = pcurrLine;
	}

	return lineCnt;
}

/*
* 描  述：在屏幕 x,y 坐标显示多行字符串，可自动换行
* 参  数：x, y		- 屏幕中坐标
*		  buf		- 字符串起始地址
*		  maxLines	- 最多能显示的行数
* 返回值：void
*/
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf, uint8 maxLines)
{
	static uint8 dispLine[21] = {0};
	static char EmptyLine[21] = "                    ";
	uint8 lineCnt = 0, col = 0; 
	uint8 *pcurrLine, *pnextLine, *pr;

	if(buf == NULL) return;

	pcurrLine = buf;
	pr = buf;

	while(*pr != 0x00 && lineCnt < maxLines){
		if((x/8 + col) >= 20){
			if(col == 21){
				pr -= 2;
				col -= 2;
			}
			else{	// col == 20
				pr = (*pr == '\n' ? pr + 1 : pr);
			}
			pnextLine = pr;
			
			memcpy(dispLine, pcurrLine, col);
			dispLine[col] = 0x00;
			_Printfxy(x, y, dispLine, Color_White);
			lineCnt++;
			pcurrLine = pnextLine;

			col = 0;
			x = 0;
			y += 16;
		}
		else if(*pr == '\n'){
			pr++;
			pnextLine = pr;

			memcpy(dispLine, pcurrLine, col);
			dispLine[col] = 0x00;
			_Printfxy(x, y, EmptyLine, Color_White);
			_Printfxy(x, y, dispLine, Color_White);
			lineCnt++;
			pcurrLine = pnextLine;

			col = 0;
			x = 0;
			y += 16;
		}

		if(*pr == 0x00){
			break;
		}
		else if(*pr > 0x80){
			pr += 2;
			col += 2;
		}
		else{
			pr++;
			col++;
		}
	}

	// last line 
	if(*pcurrLine != 0x00 && lineCnt < maxLines){
		memcpy(dispLine, pcurrLine, col);
		dispLine[col] = 0x00;
		_Printfxy(x, y, EmptyLine, Color_White);
		_Printfxy(x, y, dispLine, Color_White);
	}
	
}

/*
* 描  述：在屏幕 x,y 坐标显示多行字符串，可自动换行
* 参  数：x, y		- 屏幕中坐标
*		  format	- 字符串格式
*		  ... 		- 可变参数
* 返回值：void
*/
void PrintfXyMultiLine_VaList(uint8 x, uint8 y, const char * format, ...)
{
	uint8 *buf = DispBuf;
	int len;
	va_list ap;

	va_start(ap, format);
	len = vsprintf(buf, format, ap);
	if(len < 0 || len > 2048){
		len = 0;
	}
	buf[len] = '\0';
	va_end(ap);

	PrintfXyMultiLine(x, y, buf, 7);	// 最大显示10行
}

/*
* 描  述：在屏幕 x,y 坐标显示实心三角形
* 参  数：x, y		- 屏幕中坐标
*		  direction	- 三角形方向：0 - 向上 ▲ ， 1 - 向下 ▼
* 返回值：void
*/
void PrintXyTriangle(uint8 x, uint8 y, uint8 direction)
{
	uint8 x1, y1;

	if(direction == 0){
		x1 = 7/2;
		for(y1 = 1; y1 <= 7/2 + 1; y1++){
			_GUIHLine(x + x1, y + y1, x + 7 - x1, Color_Black);
			x1--;
		}
	}else{
		x1 = 0;
		for(y1 = 0; y1 < 7/2 + 1; y1++){
			_GUIHLine(x + x1, y + y1, x + 7 - x1, Color_Black);
			x1++;
		}
	}
}

/*
* 描  述：显示可滚动的字符串	- 可自动换行：遇到 \n 或 到达屏幕边界时
* 参  数：strBuf	- 字符串缓冲区，最大显示150行
*		 lineStep	- 按上下键时滚动的行数，最大7行
*		 otherKeyExit - 是否其他键也退出：0 - 不退出， 1 - 左右键也退出
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowScrollStr_(char *strBuf, uint8 lineStep, uint8 otherKeyExit)
{
	const uint8 lineMax = 7;
	int8 lineCnt = 0, currLine = 0;
	uint8 *lines[150], key;

	// lineStep check
	if(lineStep > lineMax){
		lineStep = lineMax;
	}

	lineCnt = GetPrintLines(0, strBuf, lines);

	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	PrintfXyMultiLine(0, 1*16 + 8, lines[0], lineMax);	
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	
	// 上/下滚动显示   ▲ ▼  △ ▽
	while(1){

		if(lineCnt > lineMax){
			if(currLine < lineCnt - lineMax){
				PrintXyTriangle(9*16 + 8, 8*16 + 8, 1);		// ▼
			}else{
				_GUIRectangleFill(9*16 + 8, 8*16 + 8, 160, 8*16 + 12, Color_White);
			}

			if(currLine > 0){
				PrintXyTriangle(9*16 + 8, 1*16 + 4, 0);		// ▲
			}else{
				_GUIRectangleFill(9*16 + 8, 1*16 + 5, 160, 1*16 + 8, Color_White);
			}
		}

		key = _ReadKey();

		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}
		else if( otherKeyExit == 1 
			&& (key == KEY_LEFT || key == KEY_RIGHT)){
			break;
		}
		else if(key == KEY_UP && lineCnt > lineMax){
			currLine -= lineStep;
			if(currLine < 0){
				currLine = 0;
			}
		}
		else if(key == KEY_DOWN && lineCnt > lineMax){
			currLine += lineStep;
			if(currLine > lineCnt - lineMax){
				currLine = lineCnt - lineMax;
			}
		}
		else{
			continue;
		}

		_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
		PrintfXyMultiLine(0, 1*16 + 8, lines[currLine], lineMax);
	}

	return key;
}

/*
* 描  述：显示可滚动的字符串	- 可自动换行：遇到 \n 或 到达屏幕边界时
* 参  数：strBuf	- 字符串缓冲区，最大显示150行
*		 lineStep	- 按上下键时滚动的行数，最大7行
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowScrollStr(char *strBuf, uint8 lineStep)
{
	return ShowScrollStr_(strBuf, lineStep, 0);
}

/*
* 描  述：显示可滚动的字符串 (扩展了左右键也返回)	- 可自动换行：遇到 \n 或 到达屏幕边界时
* 参  数：strBuf	- 字符串缓冲区，最大显示150行
*		 lineStep	- 按上下键时滚动的行数，最大7行
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键, 左/右键
*/
uint8 ShowScrollStrEx(char *strBuf, uint8 lineStep)
{
	return ShowScrollStr_(strBuf, lineStep, 1);
}

/*
* 描  述：字符串左侧填充
* 参  数：srcStr - 原字符串
*		  totalLen - 总字符长度：原字符+左侧填充的字符（若原字符长度>=总长度，则无需填充）
*		  padChar - 填充的字符
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
* 描  述：字符串右侧填充
* 参  数：srcStr - 原字符串
*		  totalLen - 总字符长度：原字符+右侧填充的字符（若原字符长度>=总长度，则无需填充）
*		  padChar - 填充的字符
* 返回值：void
*/
void StringPadRight(const char * srcStr, int totalLen, char padChar)
{
	uint32 srcStrLen, i = 0;
	char *pw;

	srcStrLen = strlen(srcStr);
	if(srcStrLen >= totalLen || padChar == 0x00){
		return;
	}

	pw = &srcStr[srcStrLen];
	for(i = srcStrLen; i < totalLen; i++){
		*pw++ = padChar;
	}
	*pw = 0x00;
}

/**
 * 字符串转换为小数
 * @param doubleStr - 浮点数字符串 : (如 -123.5)
 * @param decCnt	- 保留几位小数 : (如 2)
 * @param isNegative - 是否是负数：（如 true）
 * @param decimalInt - 整数部分 ：(如 123）
 * @param decimalDec - 小数部分 ：（如 50）
 * @return bool - 返回转换结果：true - 成功， false - 失败
*/
bool StringToDecimal(const char *doubleStr, uint8 decCnt, bool *isNegative, uint32 *decimalInt, uint16 *decimalDec)
{
	uint32 u32 = 0;
	uint16 u16 = 0;
	uint8 i = 0;
	char *str = &doubleStr[0];
	char *p;

	if(*str == '+' || *str == '-'){
		*isNegative = (*str == '-' ? true : false);
		str++;
	}
	if(*str < '0' || *str > '9'){
		return false;
	}
	p = str;
	while(*p != 0x00){
		if(*p == '.'){
			i++;
		}
		else if(*p < '0' || *p > '9'){
			return false;
		}

		if(i >= 2){
			return false;
		}
		p++;
	}
	
	while(*str != 0x00 && *str != '.'){
		u32 = u32 * 10 + (*str - '0');
		str++;
	}

	if(*str == '.'){
		str++;
		i = 0;
		while(*str != 0x00 && i < decCnt){
			u16 = u16 * 10 + (*str - '0');
			str++;
			i++;
		}
		while(i < decCnt){
			u16 = u16 * 10;
			i++;
		}
	}

	*decimalInt = u32;
	*decimalDec = u16;

	return true;
}

/*
* 描  述：double字符串 设置小数位数
* 参  数：doubleStr - double字符串地址，注意该缓冲区长度必须可容纳将设置的小数位数
*		  fracCnt - 小数位数
* 返回值：void
*/
void DoubleStrSetFracCnt(const char * doubleStr, uint8 fractionCnt)
{
	char *p = doubleStr;

	while(*p++){
		if(*p == '.'){
			if(fractionCnt == 0){
				*p = '\0';
				break;
			}
			p++;
			while(fractionCnt--){
				if(*p == '\0'){
					*p = '0';
				}
				p++;
			}
			*p = '\0';
			break;
		}
	}
}

/*
* 描  述：字符串头部裁剪
* 参  数：srcStr - 字符串起始地址
*		  trimChar - 裁剪的字符
* 返回值：int 裁剪后的字符串长度
*/
int StringTrimStart(const char * srcStr, char trimChar)
{
	int srcStrLen, i = 0;
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
* 描  述：字符串尾部裁剪
* 参  数：srcStr - 字符串起始地址
*		  trimChar - 裁剪的字符
* 返回值：int 裁剪后的字符串长度
*/
int StringTrimEnd(const char * srcStr, char trimChar)
{
	int srcStrLen, i = 0;
	char *pr = srcStr;

	srcStrLen = strlen(srcStr);
	if(srcStrLen == 0 || trimChar == 0x00){
		return 0;
	}
	
	for(i = srcStrLen - 1; i >= 0; i--){
		if(pr[i] != trimChar){
			break;
		}
	}
	pr[i + 1] = 0x00;

	return (srcStrLen - i);
}

/**
 * 描  述：从字符串尾部复制指定长度的字符到新字符串，并且把头部是不完整的GBK字符裁剪掉
 * 参  数：dstStr - 目的字符串,  srcStr - 源字符串,  len - 要复制字符串长度
 * 返回值：int 实际复制的字符长度
*/
int StringCopyFromTail(char * dstStr, const char * srcStr, uint8 len)
{
	int retLen = strlen(srcStr);
	uint8 *pw = dstStr;
	uint8 *pr;
	uint8 cnt = 0;

	if(retLen < len){
		strcpy(dstStr, srcStr);
	}
	else{
		pr = &srcStr[retLen - len];
		while(cnt < len){
			if(pr[cnt] < 0x80){
				break;
			}
			cnt++;
		}
		if(cnt % 2 == 1){
			pr++;
			len--;
		}

		retLen = len;

		while(len--){
			*pw++ = *pr++;
		}
		*pw = '\0';
	}

	return retLen;
}

/*
* 描  述：修正GBK字符串尾部错误，即把尾部是不完整的GBK字符裁剪掉
* 参  数：srcStr - 字符串起始地址
* 返回值：void
*/
void StringFixGbkStrEndError(const char * srcStr)
{
	char *pr = srcStr;

	while(*pr){
		if(*pr >= 0x80){
			pr++;
			if(*pr < 0x80){
				*(pr -1) = 0x00;
				break;
			}
			pr++;
		}
		else{
			pr++;
		}
	}
}

/*
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
* 描  述：BCD数 --转换为--> 10进制数
*/
uint8 BcdToDec(uint8 bcd)
{
	return (uint8)(bcd - (bcd >> 4) * 6);
}

/*
* 描  述：10进制数 --转换为--> BCD数
*/
uint8 DecToBcd(uint8 dec)
{
	return (uint8)(dec + (dec / 10) * 6);
}

/**
 * 从缓冲区中取出 Uint16 值
*/
uint16 GetUint16(uint8 *buf, uint8 len, bool reverse)
{
	uint16 u16Tmp = 0;
	uint8 i;

	if(reverse){
		for(i = 0; i < len; i++){
			u16Tmp |= (uint16)(*(buf + i) << i * 8);
		}
	}
	else{
		for(i = 0; i < len; i++){
			u16Tmp |= (uint16)(*(buf + (len - i - 1)) << i * 8);
		}
	}

	return u16Tmp;
}

/**
 * 从缓冲区中取出 Uint32 值
*/
uint32 GetUint32(uint8 *buf, uint8 len, bool reverse)
{
	uint32 u32Tmp = 0;
	uint8 i;

	if(reverse){
		for(i = 0; i < len; i++){
			u32Tmp |= (uint32)(*(buf + i) << i * 8);
		}
	}
	else{
		for(i = 0; i < len; i++){
			u32Tmp |= (uint32)(*(buf + (len - i - 1)) << i * 8);
		}
	}

	return u32Tmp;
}

/**
 * 从缓存区中取出 时间字符串, 返回字符串长度
*/
int GetTimeStr(uint8 *time, const char *format, uint8 *buf, uint8 len)
{
	int strLen = 0;

	if(len == 7){
		strLen = sprintf(time, format, *(buf), *(buf + 1), *(buf + 2), *(buf + 3), *(buf + 4)
			, *(buf + 5), *(buf + 6));
	}
	else if(len == 6){
		strLen = sprintf(time, format, *(buf), *(buf + 1), *(buf + 2), *(buf + 3), *(buf + 4)
			, *(buf + 5));
	}
	else if(len == 5){
		strLen = sprintf(time, format, *(buf), *(buf + 1), *(buf + 2), *(buf + 3), *(buf + 4));
	}
	else if(len == 4){
		strLen = sprintf(time, format, *(buf), *(buf + 1), *(buf + 2), *(buf + 3));
	}
	else if(len == 3){
		strLen = sprintf(time, format, *(buf), *(buf + 1), *(buf + 2), *(buf + 3));
	}

	return strLen;
}

/*
* 描  述：将字节数组转换成16进制字符串
* 参  数：strHex - 目的字符串缓冲区地址
*		  bytes - 源字节数组
*		  iStart - 数组中需要转换的起始索引
*		  iLength - 需要转换的长度
*		  separate - 字符串中Hex字节之间的间隔符：0 - 无间隔符， 其他字符 - 如空格或逗号
*		  reverse - 是否需要倒序：false - 不倒序， true - 倒序
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
	strHex[index++] = 0x00;

	return index;
}
/*
* 描  述：将16进制字符串转换成字节数组
* 参  数：bytes - 目的字节数组
*		  iStart - 数组中保存的起始索引
*		  iLength - 数组中可保存最大长度
*		  strHex - 源字符串缓冲区地址
*		  separate - 字符串中Hex字节之间的间隔符：0 - 无间隔符， 其他字符 - 如空格或逗号
*		  reverse - 是否需要倒序：false - 不倒序， true - 倒序
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
* 描  述：计算CRC16
* 参  数：Buf - 数据缓存起始地址
*		  Len - 计算的总长度
*		  Seed - 如电力/水力固定使用 0x8408
* 返回值：uint16 CRC16值
*/
uint16 GetCrc16(uint8 *Buf, uint32 Len, uint16 Seed)
{
    uint16 crc = 0xFFFF;
    uint8 i;

	while (Len--){
        crc ^= *Buf++;
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
* 描  述：计算CRC16 （可持续计算，便于计算文件的Crc16）
* 参  数：Buf - 数据缓存起始地址
*		  Len - 计算的总长度
*		  Seed - 如电力/水力固定使用 0x8408
*		  CrcKeep	- crc16持续计算的累计值，第一次计算前必须初始化为 CrcKeep = 0xFFFF
*					若要持续计算，不可对 CrcKeep 重新赋值 ！
* 返回值：当前的CRC16值： 即 （*CrcKeep ^ 0xFFFF）
*/
uint16 GetCrc16_Continue(uint8 *Buf, uint32 Len, uint16 Seed, uint16 *CrcKeep)
{
    uint8 i;
	uint16 crc16 = *CrcKeep;

	while (Len--){
        crc16 ^= *Buf++;
        for(i = 0; i < 8; i++){
            if (crc16 & 0x0001){
                crc16 >>= 1;
                crc16 ^= Seed;
            }
            else{
                crc16 >>= 1;
            }
        }
    }
	
	*CrcKeep = crc16;

    return (crc16 ^ 0xFFFF);
}

/*
* 描  述：计算CRC8
* 参  数：Buf - 数据缓存起始地址
*		  Len - 计算的总长度
*		  Seed - 如电力/水力固定使用 0x8408
* 返回值：uint8 CRC16值
*/
uint8 GetCrc8(uint8 *Buf, uint32 len)
{
    uint8 i, *ptr = Buf;
    uint8 crc = 0x0;

    while (len--)
    {
        crc ^= *ptr++;
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
* 描  述：计算8位累加和
* 参  数：Buf - 数据缓存起始地址
*		  Len - 计算的总长度
* 返回值：uint8 累加和
*/
uint8 GetSum8(uint8 *buf, uint32 len)
{
    uint8 sum = 0, *ptr = buf;

	while(len-- > 0){
		sum += *ptr++;
	}
	
    return sum;
}

/*
* 描  述：获取时间字符串
* 参  数：datetime		- 输入参数：时间字符串 yyyy-MM-dd HH:mm:ss
*		 year/month/day/hour/min/sec - 输出参数：年/月/日/时/分/秒 字符串缓冲区
* 返回值：void
*/
void DatetimeToTimeStrs(const char *datetime, char *year, char *month, char *day, char *hour, char *min, char *sec)
{
	if(year[0] == 0x00){
		year[0] = datetime[0];		// year
		year[1] = datetime[1];
		year[2] = datetime[2];
		year[3] = datetime[3];
		year[4] = 0x00;
	}
	if(month[0] == 0x00){
		month[0] = datetime[5];		// month
		month[1] = datetime[6];
		month[2] = 0x00;
	}
	if(day[0] == 0x00){
		day[0] = datetime[8];	// day
		day[1] = datetime[9];
		day[2] = 0x00;
	}
	if(hour[0] == 0x00){
		hour[0] = datetime[11];	// hour
		hour[1] = datetime[12];
		hour[2] = 0x00;
	}
	if(min[0] == 0x00){
		min[0] = datetime[14];	// minute
		min[1] = datetime[15];
		min[2] = 0x00;
	}
	if(sec[0] == 0x00){
		sec[0] = datetime[17];	// second
		sec[1] = datetime[18];
		sec[2] = 0x00;
	}
}

/*
* 描  述：时间字符串 转换为 时间数组
* 参  数：bytes		- 时间字节数组 7 byte: yy yy MM dd HH mm ss
*		  year/month/day/hour/min/sec - 年/月/日/时/分/秒 字符串缓冲区
* 返回值：uint8 错误码：0 - 正确, 1/2/3/4/5/6 - 年/月/日/时/分/秒 错误
*/
uint8 TimeStrsToTimeBytes(uint8 bytes[], char *year, char *month, char *day, char *hour, char *min, char *sec)
{
	uint8 errorCode = 0;
	uint32 tmp;

	do{
		// year
		tmp = _atof(year);
		if( year[0] > '9' || year[0] < '0'){
			year[0] = 0x00;
			errorCode = 1;
			break;
		}
		bytes[0] = (uint8)((tmp / 100) & 0xFF);
		bytes[1] = (uint8)((tmp % 100) & 0xFF);

		// month
		tmp = _atof(month);
		if(tmp < 1 || tmp > 12 ){
			month[0] = 0x00;
			errorCode = 2;
			break;
		}
		bytes[2] = (uint8)(tmp & 0xFF);

		// day
		tmp = _atof(day);
		if(tmp < 1 || tmp > 31 ){
			day[0] = 0x00;
			errorCode = 3;
			break;
		}
		bytes[3] = (uint8)(tmp & 0xFF);

		// hour
		tmp = _atof(hour);
		if(hour[0] < '0' || tmp > 23 ){
			hour[0] = 0x00;
			errorCode = 4;
			break;
		}
		bytes[4] = (uint8)(tmp & 0xFF);

		// min
		tmp = _atof(min);
		if(min[0] < '0' || tmp > 59 ){
			min[0] = 0x00;
			errorCode = 5;
			break;
		}
		bytes[5] = (uint8)(tmp & 0xFF);
		
		// sec
		tmp = _atof(sec);
		if(sec[0] < '0' || tmp > 59 ){
			sec[0] = 0x00;
			errorCode = 6;
			break;
		}
		bytes[6] = (uint8)(tmp & 0xFF);

	}while(0);

	return errorCode;
}

/*
* 描  述：Ip字符串 转换为 Ip字节数组
* 参  数： ip		- ip地址字节数组
*		  strIp1/2/3/4	- IP地址第1/2/3/4段字符串 
* 返回值：uint8 错误码：0 - 转换成功  1/2/3/4 - IP地址第1/2/3/4段字符串 错误
*/
uint8 IpStrsToIpBytes(uint8 ip[], char *strIp1, char *strIp2, char *strIp3, char *strIp4)
{
	uint8 errorCode = 0;
	uint32 tmp;

	do{
		// ip1
		tmp = _atof(strIp1);
		if(strIp1[0] < '0' || strIp1[0] > '9' || tmp > 255){
			errorCode = 1;
			break;
		}
		ip[0] = (uint8)(tmp & 0xFF);

		// ip2
		tmp = _atof(strIp2);
		if(strIp2[0] < '0' || strIp2[0] > '9' || tmp > 255){
			errorCode = 2;
			break;
		}
		ip[1] = (uint8)(tmp & 0xFF);

		// ip3
		tmp = _atof(strIp3);
		if(strIp3[0] < '0' || strIp3[0] > '9' || tmp > 255){
			errorCode = 3;
			break;
		}
		ip[2] = (uint8)(tmp & 0xFF);

		// ip4
		tmp = _atof(strIp4);
		if(strIp4[0] < '0' || strIp4[0] > '9' || tmp > 255){
			errorCode = 4;
			break;
		}
		ip[3] = (uint8)(tmp & 0xFF);

	}while(0);

	return errorCode;
}

/*
* 描述： 命令发送/接收处理，使用到协议打包和解析宏函数定义
* 参数： cmdid	- 当前命令标识
*		addrs	- 地址域		
*		args	- 命令参数：args->items[0] - 命令ID, args->items[1] - 数据域
*		ackLen	- 应答长度 (byte)
*		timeout	- 超时时间 (ms)  默认为 8s + 中继数 x 2 x 6s
*		tryCnt	- 重试次数 默认3次
* 返回： CmdResult  - 命令执行结果： true - 成功， false - 失败		
*/
CmdResult CommandTranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	CmdResult cmdResult;
	uint8 sendCnt = 0, key = 0;
	uint16 waitTime = 0, currRxLen;
	char strTmp[20];
	
	if(FramePack == NULL || FrameExplain == NULL)
	{
		return CmdResult_Cancel;
	}

	#if LOG_ON
		LogPrint("当前命令: %s", CurrCmdName);
	#endif

	do{
		// 发送 
		TxLen = FramePack(TxBuf, addrs, cmdid, args, sendCnt);
		#if LOG_ON && LogTxRx
			LogPrintBytes("Tx: ", TxBuf, TxLen);
		#endif

		_CloseCom();
		_ComSetTran(CurrPort);
		_ComSet(CurrBaud, 2);

		_GetComStr(TmpBuf, 1000, 1);		// clear
		_SendComStr(TxBuf, TxLen);
		sendCnt++;
		if(sendCnt == 1){
			sprintf(strTmp, "发送...");
		}
		else{
			sprintf(strTmp, "重发..%d", sendCnt);
		}

		// 接收
		_GetComStr(TmpBuf, 1000, 1);		// clear
		RxLen = 0;
		waitTime = 0;
		currRxLen = 0;
		cmdResult = CmdResult_Timeout;
		_DoubleToStr(TmpBuf, (double)(timeout / 1000), 0);
		PrintfXyMultiLine_VaList(0, 9*16, "< %s 等待 %s s >", strTmp, TmpBuf);
		
		do{
			if(IsNoAckCmd == false){
				currRxLen = _GetComStr(&RxBuf[RxLen], 80, 8);	// 时间校准为 N x12 ms : 8x12 = 96 ~= 100ms
				RxLen += currRxLen;
				waitTime += 100;
			}else{
				_Sleep(5);
				waitTime += 5;
			}
			key = _GetKeyExt();
			if(KEY_CANCEL == key){
				_Printfxy(0, 9*16, "返回  <已取消>  确定", Color_White);
				DispBuf[0] = 0x00;
				_CloseCom();
				return CmdResult_Cancel;
			}

			if(TranceiverCycleHook != NULL){
				TranceiverCycleHook(key); 
			}

			if(IsNoAckCmd == false){
				if(waitTime % 1000 == 0){
					_DoubleToStr(TmpBuf, (double)((timeout - waitTime) / 1000), 0);
					PrintfXyMultiLine_VaList(0, 9*16, "< %s 等待 %s s >", strTmp, TmpBuf);
				}

				if(RxLen > 0 && currRxLen == 0){	// 未超时的1包数据
					#if LOG_ON && LogTxRx && !LogScom_On
						LogPrintBytes("Rx: ", RxBuf, RxLen);
					#endif
					cmdResult = FrameExplain(RxBuf, RxLen, LocalAddr, cmdid, ackLen, DispBuf);
					RxLen = 0;
					if(cmdResult == CmdResult_Ok || cmdResult == CmdResult_Failed){
						break;
					}
				}
			}
		}while(waitTime <= timeout || currRxLen > 0);

		if(IsNoAckCmd == true){
			cmdResult = CmdResult_Ok;
		}
		else if(cmdResult == CmdResult_Timeout || cmdResult == CmdResult_CrcError){		
			// 超时后的1包数据
			RxLen += _GetComStr(&RxBuf[RxLen], 80, 8);	
			#if LOG_ON && LogTxRx
				LogPrintBytes("Rx: ", RxBuf, RxLen);
			#endif
			cmdResult = FrameExplain(RxBuf, RxLen, LocalAddr, cmdid, ackLen, DispBuf);
		}
	}while(sendCnt < tryCnt && (cmdResult == CmdResult_Timeout || cmdResult == CmdResult_CrcError));

	_CloseCom();

	#if LOG_ON
		LogPrint("解析结果: \r\n%s", DispBuf);
	#endif

	return cmdResult;
}

/*
* 描述： 命令发送/接收解析		- 执行完成后，返回结果
* 参数： cmdid	- 当前命令标识
*		addrs	- 地址域		
*		args	- 命令参数：args->items[0] - 命令ID, args->items[1] - 数据域
*		ackLen	- 应答长度 (byte)
*		timeout	- 超时时间 (ms)  默认为 8s + 中继数 x 2 x 6s
*		tryCnt	- 重试次数 默认3次
* 返回： 命令执行结果： CmdResult_OK - 成功， CmdResult_Failed - 失败	
*		CmdResult_CrcError - crc错误, CmdResult_Timeout - 超时, CmdResult_Cancel - 已取消
*/
CmdResult ProtolCommandTranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	CmdResult cmdResult;

	// if((args->buf[0] >= 0x40 && args->buf[0] <= 0x66) 
	// 	|| (args->buf[0] >= 0xF1 && args->buf[0] <= 0xF3)){
	// 	// CenterNoSave(StrDstAddr);
	// }

	_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
#if (AddrLen <= 6)
	PrintfXyMultiLine_VaList(0, 1*16 + 8, "表号: %s ", StrDstAddr);
#else
	PrintfXyMultiLine_VaList(0, 1*16 + 8, "表号:\n   %s ", StrDstAddr);
#endif

	cmdResult = CommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);
	
	if(cmdResult == CmdResult_Cancel){
		return cmdResult;
	}

	// 显示结果
#if RxBeep_On
	_SoundOn();
	_Sleep(50);
	_SoundOff();
#endif
	if(cmdResult == CmdResult_Ok){
		_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
		//------------------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回  < 成功 >  继续", Color_White);
	}
	else{
#if RxBeep_On
		_Sleep(30);
		_SoundOn();
		_Sleep(30);
		_SoundOff();
#endif
		if(cmdResult == CmdResult_Failed){
		//	_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
		}
		//-----------------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回  < 失败 >  继续", Color_White);
	}

	return cmdResult;
}

/*
* 描 述：命令收发时定周期回调 - 按键时打开lcd背景灯
* 参 数：currKey	- 命令收发时定周期检测的按键值
* 返 回：void
*/
void CycleInvoke_OpenLcdLight_WhenKeyPress(uint8 currKey)
{
	if(currKey != 0){	// 其他键，打开背景灯
		_OpenLcdBackLight();
		LcdOpened = true;
	}
}

/*
* 描 述：LCD背景灯周期控制
* 		命令收发过程中有按键时打开lcd背景灯，调用该函数N次后关闭Lcd背景灯
* 参 数：lcdCtrl  - 调用该函数时+1，初始为0 ， 0 - 打开， N - 关闭
* 		 closeCnt - 关闭LCD时的调用次数 N
* 返 回：void
*/
void LcdLightCycleCtrl(uint8 *lcdCtrl, uint8 closeCnt)
{
	if( *lcdCtrl == 0){
		_OpenLcdBackLight();
		(*lcdCtrl)++;
		LcdOpened = true;
	}
	else if(*lcdCtrl < closeCnt){
		(*lcdCtrl)++;
	}
	else if(*lcdCtrl == closeCnt){
		_CloseLcdBackLight();
		(*lcdCtrl)++;
		LcdOpened = false;
	}

	if(LcdOpened && *lcdCtrl > closeCnt){	
		*lcdCtrl = 0;
	}
}

/**
 * 检测可用内存，单位K
*/
uint16 CheckAvalibleMemory()
{
	uint8 **mem[512];
	uint16 i, kSize;
	for(i = 0; i < 512; i++){
		mem[i] = (uint8 *) _malloc(1024);
		if(mem[i] == NULL) break;
	}
	kSize = i + 2;
	for(i = 0; i < 512; i++){
		if(mem[i] == NULL) break;
		_free(mem[i]);
	}

	return kSize;
}

/**
 *  创建中继列表输入框ui
 *  参数：
 * 		pUi - 当前ui指针
 * 		pUiCnt - 当前ui总数指针
 * 		uiRowIdx - 当前ui所在行序号
 *  返回：uint8 - 创建的ui行数
*/
uint8 CreateRelayAddrsUi(UI_Item *pUi, int8 *pUiCnt, uint8 uiRowIdx)
{
	uint8 rowIdx = uiRowIdx;
	
#ifdef Project_6009_RF
	uint8 i;
	for(i = 0; i < RELAY_MAX; i++){
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
			sprintf(StrRelayAddr[i], "    (可选)    ");
		}
	}
	TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "中继1:", StrRelayAddr[0], AddrLen*2, (AddrLen*2*8 + 8), true);
	TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "中继2:", StrRelayAddr[1], AddrLen*2, (AddrLen*2*8 + 8), true);
	TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "中继3:", StrRelayAddr[2], AddrLen*2, (AddrLen*2*8 + 8), true);
#elif defined(Project_8009_RF)
	uint8 i;
	for(i = 0; i < RELAY_MAX; i++){
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
			sprintf(StrRelayAddr[i], "    (可选)    ");
		}
	}
	TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "中继1:", StrRelayAddr[0], AddrLen*2, (AddrLen*2*8 + 8), true);
	TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "中继2:", StrRelayAddr[1], AddrLen*2, (AddrLen*2*8 + 8), true);
#endif

	return (uiRowIdx - rowIdx);
}

/**
 * 表号保存
 * @param mtrNo - 表号: 根据项目宏定义来确定是10/12/16位字符, 并定位到不同的存储位置
 * @return void
*/
void MeterNoSave(uint8 *mtrNo)
{
	int fp;

	if(_Access("system.cfg", 0) < 0){
		fp = _Fopen("system.cfg", "W");
	}else{
		fp = _Fopen("system.cfg", "RW");
	}

#if defined Project_6009_RF || defined Project_6009_RF_HL
	_Lseek(fp, 0, 0);	// byte [0 ~ 19] 12位表号 
#elif defined Project_6009_IR || defined Project_6009_IR_BJ
	_Lseek(fp, 40, 0);	// byte [40 ~ 59] 16位表号 
#elif defined Project_8009_RF || defined Project_8009_RF_PY || defined Project_8009_RF_HL
	_Lseek(fp, 60, 0);	// byte [60 ~ 79] 10位表号 
#else
	_Lseek(fp, 20, 0);	// byte [20 ~ 39] 其他
#endif

	_Fwrite(mtrNo, TXTBUF_LEN, fp);
	_Fclose(fp);
}

/**
 * 表号载入
 * @param mtrNo - 表号: 根据项目宏定义来确定是10/12/16位字符, 并定位到不同的存储位置
 * @return void
*/
void MeterNoLoad(uint8 *mtrNo)
{
	int fp;

	if(_Access("system.cfg", 0) < 0){
		mtrNo[0] = 0x00;
		return;
	}else{
		fp = _Fopen("system.cfg", "R");
	}
	
#if defined Project_6009_RF || defined Project_6009_RF_HL
	_Lseek(fp, 0, 0);	// byte [0 ~ 19] 12位表号 
#elif defined Project_6009_IR || defined Project_6009_IR_BJ
	_Lseek(fp, 40, 0);	// byte [40 ~ 59] 16位表号 
#elif defined Project_8009_RF || defined Project_8009_RF_PY || defined Project_8009_RF_HL
	_Lseek(fp, 60, 0);	// byte [60 ~ 79] 10位表号 
#else
	_Lseek(fp, 20, 0);	// byte [20 ~ 39] 其他
#endif

	_Fread(mtrNo, TXTBUF_LEN, fp);
	_Fclose(fp);
}

void SysCfgLoad(void)
{
	int fp;

	if(_Access("system.cfg", 0) < 0){
		return;
	}else{
		fp = _Fopen("system.cfg", "R");
	}
	
	_Lseek(fp, 1024, 0);		// @1024 BackupBuf 2K 
	_Fread(BackupBuf, 2048, fp);
	_Fclose(fp);
}

void SysCfgSave(void)
{
	int fp;

	if(_Access("system.cfg", 0) < 0){
		fp = _Fopen("system.cfg", "W");
	}else{
		fp = _Fopen("system.cfg", "RW");
	}
	
	_Lseek(fp, 1024, 0);		// @1024 BackupBuf 2K 
	_Fwrite(BackupBuf, 2048, fp);
	_Fclose(fp);
}

//----------------------------------	版本信息		--------------------------
void VersionInfoFunc(void)
{
	uint8 key;
	uint16 dispIdx = 0;
	char *dispBuf;

	while(1){
		_ClearScreen();

		_Printfxy(0, 0, "<<版本信息", Color_White);
		//--------------------------------------------------
		dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "%s\n", VerInfo_Name);
		dispIdx += sprintf(&dispBuf[dispIdx], "版 本 号：%s\n", VerInfo_RevNo);
		dispIdx += sprintf(&dispBuf[dispIdx], "版本日期：%s\n", VerInfo_RevDate);
		dispIdx += sprintf(&dispBuf[dispIdx], "通信方式：%s\n", TransType);
		dispIdx += sprintf(&dispBuf[dispIdx], "通信速率：%s\n", CurrBaud);
		#if UseCrc16
		dispIdx += sprintf(&dispBuf[dispIdx], "校验算法：CRC16\n");
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "校验算法：CRC8\n");
		#endif
		#ifdef VerInfo_Msg
		dispIdx += sprintf(&dispBuf[dispIdx], "%s\n", VerInfo_Msg);
		#endif
		//----------------------------------------------
		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowScrollStr(dispBuf,  7);
		
		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}
	}
}