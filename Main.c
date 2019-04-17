#include <HJLIB.H>
#include "string.h"
//#include "dbf.h"
#include "stdio.h"

typedef unsigned char bool;
#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif

// --------------------------------  全局变量  -----------------------------------------
char Screenbuff[160*(160/3+1)*2]; 
uint8 TmpBuf[1080];
uint8 TxBuf[1080];
uint8 RxBuf[1080];
uint32 RxLen, TxLen;
uint8 Fsn;
char * ZeroAddr = "00000000000000";	// 0 地址，14字符
char * LocalAddr = "00201900002019";	// 0 地址，14字符

typedef enum{
	ModType_MainNode,
	ModType_PowerSub,
	ModType_WaterSub
}ModuleType;
ModuleType currModType;

typedef enum{
	PowerCmd_ReadVerInfo,
	PowerCmd_ReadNodeInfo,
	PowerCmd_ReadMeter_645_07,
	
	PowerCmd_ReadReportData,
	PowerCmd_ClearReportData,
	PowerCmd_QueryBindedWaterMeter,
	PowerCmd_ReadVerInfo,
	PowerCmd_ReadVerInfo,
}PowerCmdDef;

typedef enum{
	 WaterCmd_ReadMeter,
	 WaterCmd_0x01_,
	 WaterCmd_0x01_,
	 WaterCmd_0x01_
}WaterCmdDef;

//---------------------------------  通用方法  -------------------------------------
/*
* 函数名：IndexOf
* 描  述：在数组中查找，可指定查找的起始位置和范围
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
		  reverse - 是否需要倒序：0 - 不倒序， 1 - 倒序
* 返回值：int - 转换后的字符数：0 - 转换失败
*/
int GetStringHexFromBytes(char * strHex, uint8 bytes[], int iStart, int iLength, char separate, uint8 reverse)
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
		  reverse - 是否需要倒序：0 - 不倒序， 1 - 倒序
* 返回值：int - 转换后的字节数：0 - 转换失败
*/
int GetBytesFromStringHex(uint8 bytes[], int iStart, const char * strHex, char separate, uint8 reverse)
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

//---------------------------------  命令发送	-----------------------------------------

/*
* 函数名：PackElectricReqFrame
* 描  述：打包电力命令请求帧
* 参  数：buf - 数据缓存起始地址
		  dstAddr - 目的地址
		  cmdId - 命令字
		  retryCnt - 重发次数：0 - 第1次发送，其他 - 第n次重发
* 返回值：uint8 帧总长度
*/
uint8 PackElectricReqFrame(uint8 * buf, const char * dstAddr, uint8 cmdId, bool retryCnt)
{
	static uint8 macFsn = 0xFF, nwkFsn = 0xFF, apsFsn = 0xFF, index = 0;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	macFsn = (retryCnt > 0 ? macFsn : macFsn + 1);
	nwkFsn = (retryCnt > 0 ? nwkFsn : nwkFsn + 1);
	apsFsn = (retryCnt > 0 ? apsFsn : apsFsn + 1);
	
	// mac layer
	index = 0;
	buf[index++] = 0x00;	// length		- skip
	buf[index++] = 0x00;	// channel
	buf[index++] = 0x01;	// ver
	buf[index++] = 0x00;	// xor check	- skip
	buf[index++] = 0x41;	// mac ctrl 
	buf[index++] = 0xCD;
	buf[index++] = macFsn;	// mac fsn
	buf[index++] = 0xFF;	// panid
	buf[index++] = 0xFF;
	GetBytesFromStringHex(buf, index, dstAddr, 0, 1);
	index += 6; 			// mac dst addr - skip
	GetBytesFromStringHex(buf, index, LocalAddr, 0, 1);
	index += 6; 			// mac src addr

	// nwk layer
	buf[index++] = 0x3C;	// nwk ctrl
	GetBytesFromStringHex(buf, index, dstAddr, 0, 1);
	index += 6; 			// nwk dst addr - skip
	GetBytesFromStringHex(buf, index, LocalAddr, 0, 1);
	index += 6; 			// nwk src addr
	buf[index++] = (nwkFsn << 4) | 0x01;	// nwk fsn|radius - fixed

	// aps layer
	buf[index++] = 0x09;	// aps ctrl 
	buf[index++] = apsFsn;	// aps Fsn

	
	
	return index;
}

// --------------------------------  参数配置/读取  -----------------------------------------



// --------------------------------  现场调试  -----------------------------------------



// --------------------------------  电力主节点通信  -----------------------------------------

void ElectricMainNodeFunc(void)
{
	
}


// --------------------------------  电力子节点通信  -----------------------------------------

void ElectricSubNodeFunc(void)
{
	uint8 key, menuItemNo, tryCnt = 0;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	uint8 inputBuff[20] = {0};
	int inputLen;
	int index;

	_ClearScreen();

	// 菜单
	menuList.title = ">> 电力子节点通信 ";
	menuList.no = 3;
	menuList.MaxNum = 3;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读取软件版本";
	menuList.str[1] = "  2. 读取节点配置";
	menuList.str[2] = "  3. 645-07抄表";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 3 * 16;
	inputBox.width = 7 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = inputBuff;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 0;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(0);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"19200,E,8,1", 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;
		_ClearScreen();

		switch(menuItemNo){
		case 1:		// " 读取软件版本 ";
			_Printfxy(0, 0, ">> 读取软件版本", 0);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, 1);	
			_Printfxy(0, 2*16, "地址:", 0);
			_Printfxy(0, 8*16, "返回            确定", 0);

			while(1)
			{
				key = _InputBox(&inputBox);
				if (key == KEY_CANCEL)
					break;

				inputLen = strlen(inputBox.context);
				if(inputLen == 0 || strncmp(ZeroAddr, inputBox.context, inputLen) == 0){
					_Printfxy(0, 4*16, "请输入有效地址", 0);
				}else{
					_Printfxy(0, 4*16, "                ", 0);
				}
				StringPadLeft(inputBox.context, 12, '0');


				index = GetBytesFromStringHex(TxBuf, 5, inputBox.context, 0, 1);
				TxLen += index;
				
				_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
				_SendComStr(TxBuf, TxLen);
				_Printfxy(0, 3*16, "数据发送...        ", 0);

				
				RxLen = _GetComStr(RxBuf, 50, 50);	// recv , 500ms timeout
				if(RxLen < 30 || strncmp(&RxBuf[9], "SRWF-", 5) != 0)
				{
					_Printfxy(0, 3*16, "接收超时！       ", 0);
					continue;
				}
				
				sprintf(&TxBuf[20], "版本信息:");
				_Printfxy(0, 3*16, &TxBuf[0], 0);
				_Printfxy(0, 4*16, &TxBuf[20], 0);

				_ReadKey();
				continue;
			}
			break;

		case 2:		// " 读取节点配置 "
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);

			//sprintf(RxBuf, "文件: %s\0", fileName);
			_Printfxy(0, 5*16, &RxBuf[0], 0);
			_Printfxy(0, 6*16, &RxBuf[20], 0);

			break;
			
		case 3:		// " 645-07抄表 ";
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			

			//sprintf(tmp, "总包数: %d\0", totalCnt);
			//_Printfxy(0, 5*16, &tmp[0], 0);

			
			break;

			default: 
				break;
		}
	}

	_CloseCom();
}

// --------------------------------  水力子节点通信  -----------------------------------------

void WaterMainNodeFunc(void)
{
	
}

// --------------------------------  透传模块设置  -----------------------------------------
void TransParentModuleFunc(void)
{
	uint8 key, menuItemNo, tryCnt = 0;
	_GuiLisStruEx menuList;
	char *fileName = NULL;
	char tmp[70];
	int fileHdl, fileLen, totalCnt, sendCnt;
	int index;
	
	_ClearScreen();

	// 菜单
	menuList.title = ">> 透传模块升级 ";
	menuList.no = 3;
	menuList.MaxNum = 3;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 查看当前版本";
	menuList.str[1] = "  2. 打开升级文件";
	menuList.str[2] = "  3. 开始升级";
	menuList.defbar = 1;
	_GUIHLine(0, 4*16 + 8, 160, 1);

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"19200,E,8,1", 2);

	while(1){

		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}

		menuList.defbar = menuItemNo;

		switch(menuItemNo){
		case 1:	// " 查看当前版本 ";
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			TxLen = 0;
			TxBuf[TxLen++] = 0xAA;
			TxBuf[TxLen++] = 0xBB;
			TxBuf[TxLen++] = 0x01;
			TxBuf[TxLen++] = 0x07;
			TxBuf[TxLen++] = 0xCC;
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 5*16, "查询中...", 0);

			sprintf(TxBuf, "当前版本:");
			RxLen = _GetComStr(&TxBuf[9], 50, 50);	// recv , 500ms timeout
			if(RxLen < 30 || strncmp(&TxBuf[9], "SRWF-", 5) != 0)
			{
				_Printfxy(0, 5*16, "接收超时", 0);
				break;
			}
			_Printfxy(0, 5*16, &TxBuf[0], 0);
			_Printfxy(0, 6*16, &TxBuf[20], 0);
			_Printfxy(0, 7*16, &TxBuf[40], 0);
			break;

		case 2:	// " 打开升级文件 "
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);

			_SaveScreenToBuff(Screenbuff);
			_ClearScreen();
			fileName = _GetFileList("选|\n择|\n升|\n级|\n文|\n件|\n  |\n  |\n", "", "");
			_ClearScreen();
			_RestoreBuffToScreen(Screenbuff);

			if (fileName == NULL){
				break;
			}
			
			sprintf(tmp, "文件: %s\0", fileName);
			_Printfxy(0, 5*16, &tmp[0], 0);
			_Printfxy(0, 6*16, &tmp[20], 0);

			fileHdl = _Fopen(fileName, "R");
			fileLen = _Filelenth(fileHdl);
			totalCnt = (fileLen + 1023)/1024;
			sendCnt = 0;
			_Fread(TxBuf, 1024, fileHdl);
			_Fclose(fileHdl);
				
			index = IndexOf(TxBuf, 1024, "SRWF-", 5, 512, 512);
			if(index < 0){
				_Printfxy(0, 7*16, "不是4E88-APP文件", 0);
				fileName = NULL;
			}
			else{
				sprintf(tmp, "大小:%dK,总包数:%d\0", fileLen/1024, totalCnt);
				_Printfxy(0, 7*16, &tmp[0], 0);
			}
			break;
			
		case 3:	// " 开始升级 ";
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			totalCnt = 200;
			
			// 初始化
			if (fileName == NULL){
				_Printfxy(0, 5*16, "请先选择升级文件", 0);
				break;
			}
			fileHdl = _Fopen(fileName, "R");
			sendCnt = 0;

			sprintf(tmp, "总包数: %d\0", totalCnt);
			_Printfxy(0, 5*16, &tmp[0], 0);
			sprintf(tmp, "正发送: %d   \0",sendCnt);
			_Printfxy(0, 6*16, &tmp[0], 0);
			_Printfxy(0, 9*16, "状态: 升级中...", 0);

			ShowProgressBar(7*16+8, totalCnt, sendCnt);

			// 升级进度
			while(1){

				if(tryCnt > 3 || sendCnt >= totalCnt){
					break;
				}
				
				TxLen = _Fread(TxBuf, 1024, fileHdl);
				_GetComStr(RxBuf, 1024, 1);		// clear , 100ms timeout
				_SendComStr(TxBuf, TxLen);

				sprintf(tmp, "正发送: %d   \0",sendCnt + 1);
				_Printfxy(0, 6*16, &tmp[0], 0);
				if(tryCnt > 0){
					sprintf(tmp, "重试%d \0",tryCnt);
					_Printfxy(6*16, 6*16, &tmp[0], 0);
				}
				tryCnt++;

				RxLen = _GetComStr(&TxBuf[9], 50, 1);	// recv , 500ms timeout
				if(RxLen < 10){
				//	continue;
				}

				sendCnt++;
				tryCnt = 0;
				ShowProgressBar(7*16+8, totalCnt, sendCnt);

			}
			_Fclose(fileHdl);

			// 升级完成
			if(tryCnt > 3){
				_Printfxy(0, 9*16, "状态: 升级失败  ", 0);
			}else{
				_Printfxy(0, 9*16, "状态: 升级完成  ", 0);
			}
			_SoundOn();
			_Sleep(500);
			_SoundOff();
			_Sleep(300);
			_SoundOn();
			_Sleep(500);
			_SoundOff();
			break;

			default: 
				break;
		}
	}

	_CloseCom();
}


// --------------------------------   主函数   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	
	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=4;
	MainMenu.title= "  桑锐手持机  ";
	MainMenu.str[0]=" 电力主节点通信 ";
	MainMenu.str[1]=" 电力子节点通信 ";
	MainMenu.str[2]=" 水力子节点通信 ";
	MainMenu.str[3]=" 透传模块升级 ";
	MainMenu.key[0]="1";
	MainMenu.key[1]="2";
	MainMenu.key[2]="3";
	MainMenu.key[3]="4";
	MainMenu.Function[0]=ElectricMainNodeFunc;
	MainMenu.Function[1]=ElectricSubNodeFunc;
	MainMenu.Function[2]=WaterMainNodeFunc;
	MainMenu.Function[3]=TransParentModuleFunc;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	
}

