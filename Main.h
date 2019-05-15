
#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"
#include "Tool.h"
#include "WaterMeter.h"


// --------------------------------  全局变量  -----------------------------------------
char Screenbuff[160*(160/3+1)*2]; 
uint8 TmpBuf[1080];
uint8 TxBuf[1080];
uint8 RxBuf[1080];
uint32 RxLen, TxLen;
const uint8 LocalAddr[7] = { 0x20, 0x19, 0x00, 0x00, 0x20, 0x19, 0x00};	// 地址 201900002019，12字符
uint8 DstAddr[7];
uint8 VersionInfo[40];
uint8 CurrCmd;
ParamsBuf Addrs;		
ParamsBuf Args;
ParamsBuf Disps;
uint8 StrBuf[10][TXTBUF_LEN];    // extend input buffer
uint8 StrDstAddr[TXTBUF_LEN];
uint8 StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
UI_ItemList UiList;

//--------------------------------------	6009水表命令 发送、接收、结果显示	----------------------------
/*
* 描述： 命令发送/接收解析		- 执行完成后，返回结果
* 参数： cmdid	- 当前命令标识
*		addrs	- 地址域		
*		args	- 命令参数：args->items[0] - 命令ID, args->items[1] - 数据域
*		ackLen	- 应答长度 (byte)
*		timeout	- 超时时间 (ms)  默认为 8s + 中继数 x 2 x 6s
*		tryCnt	- 重试次数 默认3次
* 返回： bool  - 命令执行结果： true - 成功， false - 失败		
*/
bool Protol6009Tranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 sendCnt = 0, cmdResult;
	uint16 waitTime = 0, lastRxLen;
	int fp;

	if(_Access("system.cfg", 0) < 0){
		fp = _Fopen("system.cfg", "W");
	}else{
		fp = _Fopen("system.cfg", "RW");
	}
	_Lseek(fp, 0, 0);
	_Fwrite(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);
	
	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	// 应答长度、超时时间、重发次数
	//ackLen += 14 + Addrs.itemCnt * 6;
	timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
	tryCnt = 3;

	_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
	PrintfXyMultiLine_VaList(0, 1*16 + 8, "表号: %s ", StrDstAddr);

	do{
		// 发送 
		TxLen = PackWater6009RequestFrame(TxBuf, addrs, cmdid, args, sendCnt);
		_GetComStr(TmpBuf, 1024, 100/10);	// clear , 100ms timeout
		_SendComStr(TxBuf, TxLen);
		sendCnt++;
		if(sendCnt == 1){
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 命令发送  ", Color_White);
		}
		else{
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			PrintfXyMultiLine_VaList(0, 9*16, "状态: 命令重发 %d", sendCnt);
		}

		// 接收
		_GetComStr(TmpBuf, 1024, 100/10);	// clear , 100ms timeout
		RxLen = 0;
		waitTime = 0;
		lastRxLen = 0;
		PrintfXyMultiLine_VaList(0, 5*16, "等待应答 %d/%d  %d s ", RxLen, ackLen, (timeout / 1000));

		do{
			RxLen += _GetComStr(&RxBuf[lastRxLen], 100, 10);	// 100ms 检测接收
			if(KEY_CANCEL == _GetKeyExt()){
				//------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "状态: 命令已取消", Color_White);
				return false;
			}
			waitTime += 100;
			if(lastRxLen > 0){
				if(lastRxLen != RxLen){
					lastRxLen = RxLen;
					continue;
				}else{
					break;
				}
			}else{
				lastRxLen = RxLen;
			}
		}while(RxLen < ackLen && waitTime < timeout);

		PrintfXyMultiLine_VaList(0, 5*16, "当前应答 %d/%d  ", RxLen, ackLen);

#if Log_On
		LogPrintBytes("Tx: ", TxBuf, TxLen);
		LogPrintBytes("Rx: ", RxBuf, RxLen);
#endif

		cmdResult = ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, ackLen, &Disps);

	}while(sendCnt < tryCnt && cmdResult == false);

	// 显示结果
#if RxBeep_On
	_SoundOn();
	_Sleep(50);
	_SoundOff();
#endif
	if(cmdResult == true){
		_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
		//------------------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 命令成功  ", Color_White);
	}
	else{
#if RxBeep_On
		_Sleep(30);
		_SoundOn();
		_Sleep(30);
		_SoundOff();
#endif
		//-----------------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 命令失败  ", Color_White);
	}

	_CloseCom();

	return cmdResult;
}

/*
* 描述： 命令发送/接收解析	- 执行完成后，等待按键：上/下键 - 滚动显示， 确认/取消键 - 返回
* 参数： cmdid	- 当前命令标识
*		addrs	- 地址域		
*		args	- 命令参数：args->items[0] - 命令ID, args->items[1] - 数据域
*		ackLen	- 应答长度 (byte)
*		timeout	- 超时时间 (ms)  默认为 8s + 中继数 x 2 x 6s
*		tryCnt	- 重试次数 默认3次
* 返回： uint8	- 界面退出时的按键值：确认键，取消键	
*/
uint8 Protol6009TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	const uint8 lineStep = 7, lineMax = 7;
	int8 lineCnt = 0, currLine = 0;
	uint8 *lines[100], key;

	if(false == Protol6009Tranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt)){
		if(strncmp(Disps.items[0], "结果", 4) != 0){	
			Disps.items[0] = NULL;
			lines[0] = NULL;
		}
	}

	lineCnt = GetPrintLines(0, Disps.items[0], lines);
	PrintfXyMultiLine(0, 1*16 + 8, lines[0], lineMax);

	// 上/下滚动显示   ▲ ▼  △ ▽
	while(1){

		if(lineCnt > lineMax){
			if(currLine < lineCnt - lineMax){
				PrintXyTriangle(9*16, 9*16, 1);		// ▼
			}else{
				_Printfxy(9*16, 9*16, "  ", Color_White);
			}

			if(currLine > 0){
				PrintXyTriangle(9*16, 8, 0);		// ▲
			}else{
				_Printfxy(9*16, 0, "  ", Color_White);
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


// --------------------------------  集中器模块通信  -----------------------------------------
// 1	常用命令
void CenterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<<常用命令";
	menuList.no = 9;
	menuList.MaxNum = 9;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读集中器号";
	menuList.str[1] = "  2. 读集中器版本";
	menuList.str[2] = "  3. 读集中器时钟";
	menuList.str[3] = "  4. 设集中器时钟";
	menuList.str[4] = "  5. 读GPRS参数";
	menuList.str[5] = "  6. 设GPRS参数";
	menuList.str[6] = "  7. 读GPRS信号强度";
	menuList.str[7] = "  8. 集中器初始化";
	menuList.str[8] = "  9. 读集中器工作模式";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x1010 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 填充地址
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 2	档案操作
void CenterCmdFunc_DocumentOperation(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 档案操作";
	menuList.no = 5;
	menuList.MaxNum = 5;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读档案数量";
	menuList.str[1] = "  2. 读档案信息";
	menuList.str[2] = "  3. 添加档案信息";
	menuList.str[3] = "  4. 删除档案信息";
	menuList.str[4] = "  5. 修改档案信息";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x1020 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 填充地址
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 3	路径设置
void CenterCmdFunc_RouteSetting(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<<路径设置";
	menuList.no = 2;
	menuList.MaxNum = 2;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读自定义路由";
	menuList.str[1] = "  2. 设自定义路由";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x1030 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 填充地址
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 4	命令转发
void CenterCmdFunc_CommandTransfer(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 命令转发";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读实时数据";
	menuList.str[1] = "  2. 读定时定量数据";
	menuList.str[2] = "  3. 读冻结数据";
	menuList.str[3] = "  4. 开阀";
	menuList.str[4] = "  5. 关阀";
	menuList.str[5] = "  6. 读使能";
	menuList.str[6] = "  7. 清异常";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x1040 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 填充地址
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

void CenterCmdFunc(void)
{
	_GuiMenuStru menu;
	
	menu.left=0;
	menu.top=0;
	menu.no=4;
	menu.title= "<<集中器操作 ";
	menu.str[0]=" 常用命令 ";
	menu.str[1]=" 档案操作 ";
	menu.str[2]=" 路径设置 ";
	menu.str[3]=" 命令转发 ";
	menu.key[0]="1";
	menu.key[1]="2";
	menu.key[2]="3";
	menu.key[3]="4";
	menu.Function[0]=CenterCmdFunc_CommonCmd;
	menu.Function[1]=CenterCmdFunc_DocumentOperation;
	menu.Function[2]=CenterCmdFunc_RouteSetting;
	menu.Function[3]=CenterCmdFunc_CommandTransfer;
	menu.FunctionEx=0;
	_Menu(&menu);	
	
}

// --------------------------------  水表模块通信  -----------------------------------------

// 1	常用命令
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<<常用命令";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读取用户用量";
	menuList.str[1] = "  2. 读取冻结数据";
	menuList.str[2] = "  3. 开阀";
	menuList.str[3] = "  4. 强制开阀";
	menuList.str[4] = "  5. 关阀";
	menuList.str[5] = "  6. 强制关阀";
	menuList.str[6] = "  7. 清异常命令";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;
		memset(StrBuf, 0, TXTBUF_LEN * 10);

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<<%s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x10 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x01;		// 命令字	01
				ackLen = 21;				// 应答长度 21	
				// 数据域
				Args.buf[i++] = 0x00;				// 数据格式 00	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadFrozenData:		// "读取冻结正转数据"
				/*---------------------------------------------*/
				sprintf(StrBuf[0], "0");
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "数据序号:", StrBuf[0], 1, 2*8, true);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 0-9有效 ");
					currUiItem = 4;
					continue;
				}
				Args.buf[i++] = 0x02;		// 命令字	02
				ackLen = 114;				// 应答长度 88/114	
				// 数据域
				Args.buf[i++] = 0x01;				// 数据格式 01/02
				Args.buf[i++] = _GetYear()/100;		// 时间 - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = _GetYear()%100;		
				Args.buf[i++] = _GetMonth();		
				Args.buf[i++] = _GetDay();			
				Args.buf[i++] = _GetHour();			
				Args.buf[i++] = _GetMin();			
				Args.buf[i++] = _GetSec();			
				Args.buf[i++] = StrBuf[0][0] - '0';	// 冻结数据序号	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_OpenValve:			// " 开阀 "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_OpenValveForce:		// " 强制开阀 ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_CloseValve:		// " 关阀 ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_CloseValveForce:		// " 强制关阀 ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ClearException:		// " 清异常命令 ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 1;					// 应答长度 1	
				// 数据域
				Args.buf[i++] = 0x00;		// 命令选项 00	
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 2	测试命令
void WaterCmdFunc_TestCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout, u16Tmp;
	uint32 u32Tmp;

	_ClearScreen();

	// 菜单
	menuList.title = "<<测试命令";
	menuList.no = 8;
	menuList.MaxNum = 8;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 表端重启";
	menuList.str[1] = "  2. 读表温度";
	menuList.str[2] = "  3. 读表电压";
	menuList.str[3] = "  4. 清预缴参考量";
	menuList.str[4] = "  5. 设置过流超时";
	menuList.str[5] = "  6. 读运营商编号";
	menuList.str[6] = "  7. 读上报路径";
	menuList.str[7] = "  8. 设置表号";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;
		memset(StrBuf, 0, TXTBUF_LEN * 10);

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x20 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_RebootDevice:			// "表端重启"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x04;		// 命令选项 04	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadTemperature:			// " 读表温度 "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x05;		// 命令选项 05	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadVoltage:				// " 读表电压 "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x07;		// 命令选项 07	
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_ClearPrepaidRefVal:		// " 清预缴参考量 ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x08;		// 命令选项 08	
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_SetOverCurrentTimeout:		// " 设置过流超时 ";
				/*---------------------------------------------*/
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "过流电流mA:", StrBuf[0], 3, 8*8, true);
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 7*16, "超时时间ms:", StrBuf[1], 5, 8*8, true);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(StrBuf[0][0] == 0x00 ){
					sprintf(StrBuf[0], " 请输入");
					currUiItem = 4;
					continue;
				}
				u16Tmp = (uint16)_atof(StrBuf[0]);
				u32Tmp = (uint32)_atof(StrBuf[1]);
				if(u16Tmp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUiItem = 4;
					continue;
				}
				if(StrBuf[1][0] == 0x00 ){
					sprintf(StrBuf[1], " 请输入");
					currUiItem = 5;
					continue;
				}
				if(u32Tmp > 65535){
					sprintf(StrBuf[1], " 0-65535");
					currUiItem = 5;
					continue;
				}
				
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x09;		// 命令选项 09
				Args.buf[i++] = (uint8)u16Tmp;			// 过流电流 0~255	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// 超时时间 0~65535	
				Args.buf[i++] = (uint8)(u32Tmp >> 8);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadOperatorNumber:		// " 读运营商编号 ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 4;					// 应答长度 5	
				// 数据域
				Args.buf[i++] = 0x0A;		// 命令选项 0A	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadReportRoute:		// " 读上报路径 ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 63;				// 应答长度 63	
				// 数据域
				Args.buf[i++] = 0x0E;		// 命令选项 0E	
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_SetMeterNumber:		// " 设置表号 ";
				/*---------------------------------------------*/
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "新表号:", StrBuf[0], 12, 13*8, true);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}

				// 先读取参数配置信息 - 获取版本号
				if(StrDstAddr[0] == 0x00 ){
					sprintf(StrDstAddr, " 请输入");
					continue;
				}
				if(StrBuf[0][0] == 0x00 ){
					sprintf(StrBuf[0], " 请输入");
					continue;
				}
				Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
				Args.buf[i++] = 0x04;		// 命令字	04
				ackLen = 128;				// 应答长度 128	
				Args.lastItemLen = i - 1;

				// 若读取到版本号，则设置表号
				if(true == Protol6009Tranceiver(WaterCmd_ReadMeterCfgInfo, &Addrs, &Args, ackLen, timeout, tryCnt)){
					Args.buf[i++] = 0x07;		// 命令字	07
					ackLen = 1;					// 应答长度 1	
					// 数据域
					Args.buf[i++] = 0x10;		// 命令选项 10	
					memcpy(&Args.buf[i], &Disps.buf[6], 40);	
					i += 40;					// 软件版本号
					GetBytesFromStringHex(&Args.buf[i], 0, 6, StrBuf[0], 0, false);
					i += 6;						// 新地址
					Args.lastItemLen = i - 1;

					if(true == Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt)){
						memcpy(DstAddr, &Args.buf[42], 6);
					}
				}

				// 继续 / 返回
				key = _ReadKey();
				if (key == KEY_CANCEL){
					break;
				}else{
					continue;
				}
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, " 请输入");
				continue;
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 3	程序升级
void WaterCmdFunc_Upgrade(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<<程序升级";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 单表升级";
	menuList.str[1] = "  2. 查询升级";
	menuList.str[2] = "  3. 广播升级";
	menuList.str[3] = "  4. 添加档案";
	menuList.str[4] = "  5. 删除档案";
	menuList.str[5] = "  6. 查询档案";
	menuList.str[6] = "  7. 升级统计";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x30 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 4	预缴用量
void WaterCmdFunc_PrepaiedVal(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout, u16Temp;
	uint32 u32Temp;

	_ClearScreen();

	// 菜单
	menuList.title = "<<预缴用量";
	menuList.no = 6;
	menuList.MaxNum = 6;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读预缴参考用量";
	menuList.str[1] = "  2. 设预缴参考用量";
	menuList.str[2] = "  3. 读报警限值透支";
	menuList.str[3] = "  4. 设报警限值";
	menuList.str[4] = "  5. 设关阀限值";
	menuList.str[5] = "  6. 设报警关阀限值";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;
		memset(StrBuf, 0, TXTBUF_LEN * 10);	

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<<%s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x40 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadPrepaidRefVal:		// "读预缴参考用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x15;		// 命令字	15
				ackLen = 12;				// 应答长度 12	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetPrepaidRefVal:		// " 设预缴参考用量 "
				/*---------------------------------------------*/
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "预缴用量:", StrBuf[0], 10, 11*8, true);
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "参考用量:", StrBuf[1], 10, 11*8, true);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(StrBuf[0][0] == 0x00){
					sprintf(StrBuf[0], " 请输入");
					currUiItem = 4;
					continue;
				}
				if(StrBuf[1][0] == 0x00){
					sprintf(StrBuf[1], " 请输入");
					currUiItem = 5;
					continue;
				}
				Args.buf[i++] = 0x16;		// 命令字	16
				ackLen = 2;					// 应答长度 2	
				// 数据域
				u32Temp = (uint32)_atof(StrBuf[0]);
				u16Temp = (_atof(StrBuf[0]) - u32Temp) * 10000;
				Args.buf[i++] = (uint8)(u32Temp & 0xFF);		// 预缴用量	
				Args.buf[i++] = (uint8)((u32Temp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Temp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Temp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Temp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Temp >> 8) & 0xFF);
				u32Temp = (uint32)_atof(StrBuf[1]);
				u16Temp = (_atof(StrBuf[1]) - u32Temp) * 10000;
				Args.buf[i++] = (uint8)(u32Temp & 0xFF);		// 参考起始用量	
				Args.buf[i++] = (uint8)((u32Temp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Temp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Temp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Temp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Temp >> 8) & 0xFF);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadAlarmLimitOverdraft:	// " 读报警限值透支 "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x17;		// 命令字	17
				ackLen = 4;					// 应答长度 4	
				break;
			
			case WaterCmd_SetAlarmLimit:		// " 设报警限值 ";
				/*---------------------------------------------*/
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "报警限值:", StrBuf[0], 3, 11*8, true);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(StrBuf[0][0] == 0x00){
					sprintf(StrBuf[0], " 请输入");
					currUiItem = 4;
					continue;
				}
				u16Temp = (uint16)_atof(StrBuf[0]);
				if(u16Temp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUiItem = 4;
					continue;
				}
				Args.buf[i++] = 0x18;		// 命令字	18
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x00;		// 命令选项：0- 报警限值 1-关阀限值  2-报警限值+关阀限值	
				Args.buf[i++] = (uint8)u16Temp;		// 报警限值
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_SetCloseValveLimit:		// " 设关阀限值 ";
				/*---------------------------------------------*/
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "关阀限值:", StrBuf[0], 5, 11*8, true);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(StrBuf[0][0] == 0x00){
					sprintf(StrBuf[0], " 请输入");
					currUiItem = 4;
					continue;
				}
				u32Temp = (uint32)_atof(StrBuf[0]);
				if(u16Temp > 65535){
					sprintf(StrBuf[0], " 0-65535");
					currUiItem = 4;
					continue;
				}
				Args.buf[i++] = 0x18;		// 命令字	18
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x01;		// 命令选项：0- 报警限值 1-关阀限值  2-报警限值+关阀限值	
				Args.buf[i++] = (uint8)u32Temp;		// 关阀限值
				Args.buf[i++] = (uint8)(u32Temp >> 8);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetAlarmAndCloseValveLimit:		// " 设报警关阀限值 ";
				/*---------------------------------------------*/
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "报警限值:", StrBuf[0], 3, 11*8, true);
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "关阀限值:", StrBuf[1], 5, 11*8, true);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(StrBuf[0][0] == 0x00){
					sprintf(StrBuf[0], " 请输入");
					currUiItem = 4;
					continue;
				}
				u16Temp = (uint16)_atof(StrBuf[0]);
				if(u16Temp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUiItem = 4;
					continue;
				}
				if(StrBuf[1][0] == 0x00){
					sprintf(StrBuf[1], " 请输入");
					currUiItem = 4;
					continue;
				}
				u32Temp = (uint32)_atof(StrBuf[1]);
				if(u16Temp > 65535){
					sprintf(StrBuf[1], " 0-65535");
					currUiItem = 4;
					continue;
				}
				Args.buf[i++] = 0x18;		// 命令字	18
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x02;		// 命令选项：0- 报警限值 1-关阀限值  2-报警限值+关阀限值	
				Args.buf[i++] = (uint8)u16Temp;		// 报警限值
				Args.buf[i++] = (uint8)u32Temp;		// 关阀限值
				Args.buf[i++] = (uint8)(u32Temp >> 8);
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 5	工作参数
void WaterCmdFunc_WorkingParams(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<<工作参数";
	menuList.no = 8;
	menuList.MaxNum = 8;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "1. 设表底数脉冲系数";
	menuList.str[1] = "2. 清除反转计量数据";
	menuList.str[2] = "3. 读取功能使能状态";
	menuList.str[3] = "4. 设置定时上传";
	menuList.str[4] = "5. 设置定量上传";
	menuList.str[5] = "6. 设置定时定量上传";
	menuList.str[6] = "7. 读表端时钟";
	menuList.str[7] = "8. 校表端时钟";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x20 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 6	其他操作
void WaterCmdFunc_Other(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pByte;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// 菜单
	menuList.title = "<<其他操作";
	menuList.no = 5;
	menuList.MaxNum = 5;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "1. 读收/发/磁扰次数";
	menuList.str[1] = "2. 读取RXD和TXD信道";
	menuList.str[2] = "3. 设置RXD和TXD信道";
	menuList.str[3] = "4. 设置运营商编号";
	menuList.str[4] = "5. 路径下发";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			pByte = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pByte[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <可选> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x20 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "请先输入表号");
				continue;
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
	}

	_CloseCom();
}

void WaterCmdFunc(void)
{

	_GuiMenuStru menu;
	
	menu.left=0;
	menu.top=0;
	menu.no=6;
	menu.title= "<<表端操作 ";
	menu.str[0]=" 常用命令 ";
	menu.str[1]=" 测试命令 ";
	menu.str[2]=" 程序升级 ";
	menu.str[3]=" 预缴用量 ";
	menu.str[4]=" 工作参数 ";
	menu.str[5]=" 其他操作 ";
	menu.key[0]="1";
	menu.key[1]="2";
	menu.key[2]="3";
	menu.key[3]="4";
	menu.key[4]="5";
	menu.key[5]="6";
	menu.Function[0]=WaterCmdFunc_CommonCmd;
	menu.Function[1]=WaterCmdFunc_TestCmd;
	menu.Function[2]=WaterCmdFunc_Upgrade;
	menu.Function[3]=WaterCmdFunc_PrepaiedVal;
	menu.Function[4]=WaterCmdFunc_WorkingParams;
	menu.Function[5]=WaterCmdFunc_Other;
	menu.FunctionEx=0;
	_Menu(&menu);	
}

//-----------------------------------	主菜单	---------------------------
// 读取用户用量
void MainFuncReadRealTimeData(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		_Printfxy(0, 0, "<<读取用户用量", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <可选> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		CurrCmd = WaterCmd_ReadRealTimeData;	// "读取用户用量"

		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x01;		// 命令字	01
		ackLen = 21;				// 应答长度 21	
		// 数据域
		Args.buf[i++] = 0x00;				// 数据格式 00	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "请先输入表号");
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// 读取冻结数据
void MainFuncReadFrozenData(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	memset(StrBuf, 0, TXTBUF_LEN * 10);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		_Printfxy(0, 0, "<<读取冻结数据", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <可选> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		CurrCmd = WaterCmd_ReadFrozenData;		// "读取冻结正转数据"
		/*---------------------------------------------*/
		sprintf(StrBuf[0], "0");
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "数据序号:", StrBuf[0], 1, 2*8, true);
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
			sprintf(StrBuf[0], " 0~9有效 ");
			currUiItem = 4;
			continue;
		}
		Args.buf[i++] = 0x02;		// 命令字	02
		ackLen = 114;				// 应答长度 88	
		// 数据域
		Args.buf[i++] = 0x02;				// 数据格式 01/02
		Args.buf[i++] = _GetYear()/100;		// 时间 - yyyy/mm/dd HH:mm:ss
		Args.buf[i++] = _GetYear()%100;		
		Args.buf[i++] = _GetMonth();		
		Args.buf[i++] = _GetDay();			
		Args.buf[i++] = _GetHour();			
		Args.buf[i++] = _GetMin();			
		Args.buf[i++] = _GetSec();			
		Args.buf[i++] = StrBuf[0][0] - '0';	// 冻结数据序号	
		Args.lastItemLen = i - 1;
				
		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "请先输入表号");
			currUiItem = 0;
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// 读取表端时钟
void MainFuncReadMeterTime(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		_Printfxy(0, 0, "<<读取表端时钟", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <可选> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		CurrCmd = WaterCmd_ReadMeterTime;	// 读取表端时钟

		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x13;		// 命令字	13
		ackLen = 7;					// 应答长度 7	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "请先输入表号");
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// 设置表端时钟
void MainFuncSetMeterTime(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	memset(StrBuf, 0, TXTBUF_LEN * 10);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		_Printfxy(0, 0, "<<设置表端时钟", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <可选> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		CurrCmd = WaterCmd_SetMeterTime;	// 设置表端时钟

		_GetDate(&TmpBuf[200], '-');
		_GetTime(&TmpBuf[220], ':');
		StrBuf[0][0] = TmpBuf[200];		// year
		StrBuf[0][1] = TmpBuf[201];
		StrBuf[0][2] = TmpBuf[202];
		StrBuf[0][3] = TmpBuf[203];
		StrBuf[0][4] = 0x00;
		StrBuf[1][0] = TmpBuf[205];		// month
		StrBuf[1][1] = TmpBuf[206];
		StrBuf[1][2] = 0x00;
		StrBuf[2][0] = TmpBuf[208];	// day
		StrBuf[2][1] = TmpBuf[209];
		StrBuf[2][2] = 0x00;
		StrBuf[3][0] = TmpBuf[220];	// hour
		StrBuf[3][1] = TmpBuf[221];
		StrBuf[3][2] = 0x00;
		StrBuf[4][0] = TmpBuf[223];	// minute
		StrBuf[4][1] = TmpBuf[224];
		StrBuf[4][2] = 0x00;
		StrBuf[5][0] = TmpBuf[226];	// second
		StrBuf[5][1] = TmpBuf[227];
		StrBuf[5][2] = 0x00;
		_Printfxy(0, 6*16, "时 间:", Color_White);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0*8, 7*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 5*8, 7*16, "-", StrBuf[1], 2, 2*8, false);	// MM
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 8*8, 7*16, "-", StrBuf[2], 2, 2*8, false);	// dd
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 11*8, 7*16, " ", StrBuf[3], 2, 2*8, false);	// HH
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 14*8, 7*16, ":", StrBuf[4], 2, 2*8, false);	// mm
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 17*8, 7*16, ":", StrBuf[5], 2, 2*8, false);	// ss
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		
		for(i = 0; i < 6; i ++){
			if(StrBuf[i][0] == 0x00 || StrBuf[i][0] == '0' ){
				_Printfxy(0, 8*16, "请输入正确的时间", Color_White);
				key = KEY_NOHOOK;
				break;
			}
		}
		if(key == KEY_NOHOOK){	
			key = _ReadKey();		// show err msg and wait key
			if (key != KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}

		sprintf(&TmpBuf[200], "%s-%s-%s %s:%s:%s",
			StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
		_SetDateTime(&TmpBuf[200]);

		Args.buf[i++] = 0x14;		// 命令字	14
		ackLen = 2;					// 应答长度 2	
		// 数据域
		Args.buf[i++] = _GetYear()/100;		// 时间 - yyyy/mm/dd HH:mm:ss
		Args.buf[i++] = _GetYear()%100;		
		Args.buf[i++] = _GetMonth();		
		Args.buf[i++] = _GetDay();			
		Args.buf[i++] = _GetHour();			
		Args.buf[i++] = _GetMin();			
		Args.buf[i++] = _GetSec();	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "请先输入表号");
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// 清异常命令
void MainFuncClearException(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		_Printfxy(0, 0, "<<清异常命令", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <可选> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		CurrCmd = WaterCmd_ClearException;		// " 清异常命令 ";
		/*---------------------------------------------*/
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x05;		// 命令字	05
		ackLen = 1;					// 应答长度 1	
		// 数据域
		Args.buf[i++] = 0x00;		// 命令选项 00	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "请先输入表号");
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// 开阀
void MainFuncOpenValve(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		_Printfxy(0, 0, "<<开阀", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <可选> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		CurrCmd = WaterCmd_OpenValve;			// " 开阀 "
		/*---------------------------------------------*/
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x03;		// 命令字	03
		ackLen = 3;					// 应答长度 3	
		// 数据域
		Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
		Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "请先输入表号");
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// 关阀
void MainFuncCloseValve(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		_Printfxy(0, 0, "<<关阀", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "状态: 空闲    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <可选> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "表 号:", StrDstAddr, 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "中继1:", StrRelayAddr[0], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "中继2:", StrRelayAddr[1], 12, 13*8, true);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "中继3:", StrRelayAddr[2], 12, 13*8, true);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		CurrCmd = WaterCmd_CloseValve;		// " 关阀 ";
		/*---------------------------------------------*/
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x03;		// 命令字	03
		ackLen = 3;					// 应答长度 3	
		// 数据域
		Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
		Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "请先输入表号");
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// 批量抄表
void MainFuncBatchMeterReading(void)
{

}

// 工程调试		------------------------------
void MainFuncEngineerDebuging(void)
{
	_GuiMenuStru menu;

	menu.top = 0;
	menu.left = 0;
	menu.no = 3;
	menu.title = "<<工程调试";
	menu.str[0] = "表端操作";
	menu.str[1] = "集中器操作";
	menu.str[2] = "版本信息";
	menu.key[0] = "1";
	menu.key[1] = "2";
	menu.key[2] = "3";
	menu.Function[0] = WaterCmdFunc;
	menu.Function[1] = CenterCmdFunc;
	menu.Function[2] = CenterCmdFunc;
	menu.FunctionEx = 0;
	_Menu(&menu);
}

// --------------------------------   主函数   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	int fp;

	fp = _Fopen("system.cfg", "R");
	_Fread(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);
	
	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=8;
	MainMenu.title =  "     桑锐手持机    ";
	MainMenu.str[0] = " 读取用户用量 ";
	MainMenu.str[1] = " 读取冻结数据 ";
	MainMenu.str[2] = " 读取表端时钟 ";
	MainMenu.str[3] = " 设置表端时钟 ";
	//MainMenu.str[4] = " 清异常命令 ";
	MainMenu.str[4] = " 开阀 ";
	MainMenu.str[5] = " 关阀 ";
	MainMenu.str[6] = " 批量抄表 ";
	MainMenu.str[7] = " 工程调试 ";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.key[7] = "8";
	//MainMenu.key[8] = "9";
	MainMenu.Function[0] = MainFuncReadRealTimeData;
	MainMenu.Function[1] = MainFuncReadFrozenData;
	MainMenu.Function[2] = MainFuncReadMeterTime;
	MainMenu.Function[3] = MainFuncSetMeterTime;
	//MainMenu.Function[4] = MainFuncClearException;
	MainMenu.Function[4] = MainFuncOpenValve;
	MainMenu.Function[5] = MainFuncCloseValve;
	MainMenu.Function[6] = MainFuncBatchMeterReading;
	MainMenu.Function[7] = MainFuncEngineerDebuging;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	
}

