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
* 描  述：打印日志到文件或串口No.3 , 通信串口在收发时不可打印到串口
*		  	默认日志文件名定义为 #define LogName "debug.txt" ， 
*			如需修改则在调用前重新定义 LogName 
* 参  数：format	- 字符串格式
		  ... 		- 可变参数
* 返回值：void
*/
void LogPrint(const char * format, ...)
{
#if LOG_ON
	
#if !(LogScom_On)
	int fp;
#endif
	uint32 len = 0; 
	va_list ap;
	uint8 *buf;

	buf = (uint8 *) _malloc(1024);

	va_start(ap, format);
	len += vsprintf(&buf[0], format, ap);
	buf[len++] = '\n';
	buf[len++] = '\0';
	va_end(ap);
	

#if LogScom_On
	_CloseCom();
	_ComSetTran(LogPort);
	_ComSet((uint8 *)"115200,E,8,1", 2);
	_SendComStr(buf, len);
	_CloseCom();
#else

	#ifndef LogName
		#define LogName "debug.txt"
	#endif
	if(_Access(LogName, 0) < 0){
		fp = _Fopen(LogName, "W");
	}else{
		fp = _Fopen(LogName, "RW");
	}
	_Lseek(fp, 0, 2);
	_Fwrite(buf, len, fp);
	_Fclose(fp);

#endif

	_free(buf);

#endif
}

/*
* 描  述：打印日志到文件或串口No.2 , 通信串口在收发时不可打印到串口
* 参  数：title	 - 标题
		  buf	- 字节数组起始地址
		  size	- 打印的字节数， 最大1024
* 返回值：void
*/
void LogPrintBytes(const char *title, uint8 *buf, uint16 size)
{
#if LOG_ON
	const char decHex[16] = {'0', '1', '2', '3','4', '5', '6', '7','8', '9', 'A', 'B','C', 'D', 'E', 'F'};
	uint16 i = 0;
	uint8 *tmp;

	tmp = (uint8 *) _malloc(1024);
	while(size--){
		tmp[i++] = decHex[*buf >> 4];
		tmp[i++] = decHex[*buf & 0x0F];
		tmp[i++] = ' ';
		buf++;
	}
	tmp[i++] = '\0';

	LogPrint("%s%s", title, tmp);
	_free(tmp);

#endif
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
	
	if((uiItem->ui.txtbox.dataLen == 12 || uiItem->ui.txtbox.dataLen == 16)
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
	uint8 key, isShowIcon = 1, cnt = 0;
	uint8 x, y, idx, lastIdx = 0xFF;

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
			_Printfxy(uiItem->x1, uiItem->y1, "<<", Color_White);
			_Printfxy(uiItem->x1 + uiItem->width - 16, uiItem->y1, ">>", Color_White);
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
			if(idx > 0){
				idx--;
			}
		}
		if(key == KEY_RIGHT){
			if(idx < uiItem->ui.combox.cnt -1){
				idx++;
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
	lbx->strCnt = strCnt;
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
void ListBoxCreateEx(ListBox *lbx, uint8 x, uint8 y, uint8 maxCol, uint8 maxRow, uint16 totalCnt, FillListFunc fillStrsFunc, const char *title, char **strs, uint8 strLen, uint8 strCnt)
{
	uint16 i;
	char *str = (char *)strs;

	lbx->x = x;		// 默认 0
	lbx->y = y;		// 默认 0
	lbx->maxCol = maxCol;	// 默认 20
	lbx->maxRow = maxRow;	// 默认 7
	lbx->totalCnt = totalCnt;
	lbx->fillStrsFunc = fillStrsFunc;
	lbx->title = title;
	lbx->strCnt = strCnt;
	lbx->currIdx = 0;

	for(i = 0; i < lbx->strCnt; i++){
		lbx->str[i] = str;
		str += strLen;
	}
}

/*
* 描  述：获取列表视图当前选项
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

	lbx->currIdx = (lbx->currIdx > 0 ? lbx->currIdx : 0);
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
		StringPadLeft(temp, (lbx->maxCol - strlen(lbx->title) - 1), ' ');
		sprintf(title, "%s%s", lbx->title, temp);	
		_Printfxy(lbx->x, lbx->y, title, Color_White);
		_GUIHLine(lbx->x, lbx->y + 16 + 4, x1, Color_Black);	
		//--------------------------------------------▼---
		for(i = 0; i < lbx->maxRow && (lbx->dispStartIdx + i) < lbx->strCnt; i++){
			_Printfxy(lbx->x, i * 16 + lbx->y + 16 + 8, lines[lbx->dispStartIdx + i], Color_White);
		}
		_Printfxy(lbx->x, (lbx->strIdx - lbx->dispStartIdx) * 16 + lbx->y + 16 + 8, lines[lbx->strIdx], Color_Black);
		//--------------------------------------------▲---
		_GUIHLine(lbx->x, recY1, x1, Color_Black);	

		key = _ReadKey();

		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}

		if(key == KEY_UP){
			lbx->strIdx--;
			lbx->currIdx--;
		}
		else if(key == KEY_DOWN){
			lbx->strIdx++;
			lbx->currIdx++;
		}
		else if(key == KEY_LEFT){
			lbx->strIdx -= lbx->maxRow;
			lbx->currIdx -= lbx->maxRow;
		}
		else if(key == KEY_RIGHT){
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
	char i, *ptr;

	va_start(ap, maxCnt);
	for(i = 0; i < maxCnt && i < 10; i++){
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
	uint8 key, x, y;
	uint8 i;
	UI_Item *ptr;

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
				key = GetInputNumStr(ptr);

				if( ptr->text[0] >= '0' && ptr->text[0] <= '9'){
					if((ptr->ui.txtbox.dataLen == 12 || ptr->ui.txtbox.dataLen == 16)
						&& (key == KEY_ENTER || key == KEY_UP || key == KEY_DOWN)){
						_leftspace(ptr->text, ptr->ui.txtbox.dataLen, '0');
						if(strncmp(ZeroAddr, ptr->text, ptr->ui.txtbox.dataLen) == 0){
							sprintf(ptr->text, " 不能为0 ");
							key = 0xFF;
						}
					}
					_Printfxy(ptr->x1, ptr->y1, ptr->text, Color_White);
				}
			}while(key == 0xFF);
		}
		else if(ptr->type == UI_CombBox){
			key = CombBoxGetCurrIndex(ptr);
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
	uint8 *pcurrLine, *pnextLine, *pr;

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
	if(pcurrLine != 0x00){
		lines[lineCnt++] = pcurrLine;
	}

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
	uint8 lineCnt = 0, col = 0; 
	uint8 *pcurrLine, *pnextLine, *pr;

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
	if(pcurrLine != 0x00 && lineCnt < maxLines){
		memcpy(dispLine, pcurrLine, col);
		dispLine[col] = 0x00;
		_Printfxy(x, y, dispLine, Color_White);
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
	int len;
	va_list ap;

	va_start(ap, format);
	len = vsprintf(buf, format, ap);
	if(len < 0 || len > 512){
		len = 0;
	}
	buf[len] = '\0';
	va_end(ap);

	PrintfXyMultiLine(x, y, buf, 10);	// 最大显示10行
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
* 参  数：strBuf	- 字符串缓冲区
*		 lineStep	- 按上下键时滚动的行数，最大7行
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowScrollStr(char *strBuf, uint8 lineStep)
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
* 描  述：字符串头部裁剪
* 参  数：srcStr - 字符串起始地址
		  trimChar - 裁剪的字符
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
		  trimChar - 裁剪的字符
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

/*
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