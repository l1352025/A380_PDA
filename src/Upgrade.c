#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "Upgrade.h"
#include "Common.h"
#include "WaterMeter.h"

#include "List.h"

#include "MeterDocDBF.h"
//extern MeterListSt Meters;
#ifndef Meter_Max
#define Meter_Max	300	// 一栋楼-最大表数
#endif

//-------------------------		全局变量	-----------------
AppFileInfo AppInfo;
PacketInfo PktInfo;
UpgradeDocs UpgrdDocs;
TList DocList;

//-----------------------		内部函数	------------------

// 从版本信息中截取版本号
static uint8 * GetVersionNo(uint8 *buf, uint8 verSize)
{
	uint8 i, *ptr;

	for(i = 0; i < 40; i++){
		if(buf[i] == (uint8)'.'){
			ptr = &buf[i - 1];
		}
		if(buf[i] == (uint8)' '){
			buf[i] = 0x00;
			break;
		}
	}

	return ptr;
}

bool IsMtrNoEqual(void *node, void *mtrNo)
{
    uint8 *src = ((DocInfo *)node)->mtrNo;
    uint8 *dst = (uint8 *)mtrNo;
    uint16 i;

    if(node == NULL || mtrNo == NULL) return false;

    for(i = 0; i < 20 && *src != 0x00; i++)
    {
        if(*src != *dst) break;

		src++;
		dst++;
    }
    
    return (*src == *dst);
}

/*
* 描述：显示升级档案列表, 最大100个
* 参数：docs - 档案信息列表
* 返回：uint8 - 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
static uint8 ShowDocList(TList *docs)
{
	uint8 key;
	//uint8 **strs = &DispBuf;
	uint8 **strs = Meters.strs;
	uint8 strSize = 20, *ptr, *pList;
	ListBox docList;
	DocInfo *docItem;

	if(docs->cnt > Meter_Max) return KEY_CANCEL;

	pList = (uint8 *)strs;
	docItem = (DocInfo *)docs->head;
	
	while(docItem != NULL){

		switch (docItem->state){
		case UpgrdState_NotStart: ptr = "not"; break;
		case UpgrdState_PktWait: ptr = "wait"; break;
		case UpgrdState_Finish: ptr = "ok"; break;
		default: break;
		}

		sprintf(pList, "%s  %s", docItem->mtrNo, ptr);
		pList += strSize;

		docItem = (DocInfo *)docItem->next;
	}

	ListBoxCreateEx(&docList, 0, 0, 20, 7, docs->cnt, NULL,
			"<<档案列表", strs, strSize, docs->cnt);
	_Printfxy(0, 9*16, "返回            确定", Color_White);
	key = ShowListBox(&docList);
	return key;
}

/*
* 描述：删除档案
* 参数：docs - 档案信息列表
* 返回：uint8 - 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
static uint8 ShowDocList_ForDelete(TList *docs)
{
	uint8 key;
	//uint8 **strs = &DispBuf;
	uint8 **strs = Meters.strs;
	uint8 strSize = 20, *ptr, *pList;
	uint16 currIdx = 0;
	ListBox docList;
	DocInfo *docItem;

	if(docs->cnt > Meter_Max) return KEY_CANCEL;

	while(true){

		pList = (uint8 *)strs;
		docItem = (DocInfo *)docs->head;
		
		while(docItem != NULL){
			switch (docItem->state){
			case UpgrdState_NotStart: ptr = "not"; break;
			case UpgrdState_PktWait: ptr = "wait"; break;
			case UpgrdState_Finish: ptr = "ok"; break;
			default: break;
			}
			sprintf(pList, "%s  %s", docItem->mtrNo, ptr);
			pList += strSize;
			docItem = (DocInfo *)docItem->next;
		}

		ListBoxCreateEx(&docList, 0, 0, 20, 7, docs->cnt, NULL,
				"<<档案列表", strs, strSize, docs->cnt);
		_Printfxy(0, 9*16, "返回            删除", Color_White);
		
		docList.currIdx = currIdx;
		key = ShowListBox(&docList);
		currIdx = docList.currIdx;

		if(key == KEY_ENTER){
			docs->removeAt(docs, currIdx);
			LogPrint("del currIdx : %d \n", currIdx);
		}
		else{
			break;
		}
	}

	return key;
}

// 升级档案管理
static void UpgradeFunc_UpgradeDocMgmt(void)
{
	uint8 key, menuItemNo;
	UI_Item uiTxtbox;
	ListBox menuList;
	DocInfo *pDocInfo;

	if(DocList.cnt == 0){
		List_Init(&DocList);
	}

	ListBoxCreate(&menuList, 0, 0, 20, 7, 4, NULL,
		"<<升级档案管理", 
		4,
		"1. 添加档案",
		"2. 查看档案",
		"3. 删除档案",
		"4. 清空档案"
	);

	while(1){
		_ClearScreen();

		key = ShowListBox(&menuList);
		if (key == KEY_CANCEL){	// 返回
			return;
		}
		menuItemNo = menuList.strIdx + 1;

		switch (menuItemNo)
		{
		case 1:	// 添加档案
			{
				_ClearScreen();
				TextBoxCreate(&uiTxtbox, 0, 2*16 + 8, "  > ", StrDstAddr, (AddrLen * 2), (AddrLen * 8 + 8), true);

				while(2){
					//_GUIRectangleFill(0, 0, );

					_Printfxy(0, 0, "<<添加档案", Color_White);
					_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
					/*---------------------------------------------*/
					_Printfxy(0, 1*16 + 8, "表号：", Color_White);
					_Printfxy(uiTxtbox.x, uiTxtbox.y, uiTxtbox.title, Color_White);
					//----------------------------------------------
					_GUIHLine(0, 9*16 - 4, 160, Color_Black);
					_Printfxy(0, 9*16, "返回            确定", Color_White);
					key = GetInputNumStr(&uiTxtbox);
					//------------------------------------------------------------
					if (key == KEY_CANCEL){	// 返回
						break;
					}

					_leftspace(StrDstAddr, AddrLen * 2, '0');

					if(StrDstAddr[0] >= '0' && StrDstAddr[0] <= '9' ){
						if(NULL != DocList.find(&DocList, IsMtrNoEqual, StrDstAddr)){
							_Printfxy(0, 5*16 + 8, "    表号重复！  ", Color_White);
							_Sleep(500);
							continue;
						}
						pDocInfo = (DocInfo *)DocList.add(&DocList, NULL, sizeof(DocInfo));
						memcpy(pDocInfo->mtrNo, StrDstAddr, 20);
						pDocInfo->state = UpgrdState_NotStart;
					}

					_Printfxy(0, 5*16 + 8, "    添加成功！  ", Color_White);
					PrintfXyMultiLine_VaList(0, 6*16 + 8, "当前档案总数：%d", DocList.cnt);
					_Sleep(500);
					_Printfxy(0, 5*16 + 8, "                    ", Color_White);
				}
			}
			break;
		
		case 2:	// 查看档案列表
			{
				key = ShowDocList(&DocList);
			}
			break;

		case 3:	// 删除档案
			{
				key = ShowDocList_ForDelete(&DocList);
			}
			break;

		case 4:	// 清空档案
			{
				ShowMsg(16, 4 * 16, "  档案清空中... ", 50);
				DocList.clear(&DocList);
				ShowMsg(16, 4 * 16, "  档案清空完成！ ", 1000);
			}
			break;

		default:
			break;
		}
	}
}


// 开始升级
static UpgradeState UpgradeFunc_UpgradeStart(uint8 upgradeMode)
{
	UpgradeState state = UpgrdState_NotStart;
	AppFileInfo *app = &AppInfo;
	PacketInfo *pkt = &PktInfo;
	UpgradeDocs *docs = &UpgrdDocs;
	uint16 docIdx, sendIdx, pktIdx, crc16, i, u16Tmp;
	uint8 *mtrNo, *cmdName = &DispBuf[1024], *cmdMsg = &DispBuf[1060];
	uint8 cmdState = Cmd_Send;
	uint16 ackLen, timeout, dispIdx, index;
	uint8 tryCnt, lcdCtrl, key;
	CmdResult cmdResult = CmdResult_Ok;
	//char strTmp[20];
	uint8 *pData, *ptr;

	_ClearScreen();

	_Printfxy(0, 0, "<<开始升级", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	PrintfXyMultiLine_VaList(0, 2*16, "版本：%s  CRC：%4X", pkt->version, pkt->verCrc16);
	// to show meter no
	// to show cmd name
	// to show cmd msg
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "返回   <升级中>   ", Color_White);


	// 中继清空
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}

	CurrCmd = WaterCmd_NoticeUpgrade_OnApp;
	docIdx = 0;

	while(1){
	
		// LCD背景灯控制
		if(lcdCtrl == 0){
			_OpenLcdBackLight();
			lcdCtrl++;
			LcdOpened = true;
		}
		else if(lcdCtrl < 4){
			lcdCtrl++;
		}
		else if(lcdCtrl == 4){
			_CloseLcdBackLight();
			lcdCtrl++;
			LcdOpened = false;
		}

		if(cmdState == Cmd_Send){
			// 当前表号
			mtrNo = &UpgrdDocs.mtrNos[docIdx][0];
			strcpy(StrDstAddr, mtrNo);
		
			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
		}

		switch(CurrCmd){
		case WaterCmd_NoticeUpgrade_OnApp:		// 通知系统升级_在app
		case WaterCmd_NoticeUpgrade_OnBoot:		// 通知系统升级_在boot
			/*---------------------------------------------*/
			if(cmdState == Cmd_Finish){
				if(CurrCmd == WaterCmd_NoticeUpgrade_OnBoot){
					GetMissPktList(pkt);
					sendIdx = 0;
				}
				CurrCmd++;
				cmdState = Cmd_Send;
				docIdx = 0;
				break;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrDisUpgradeReason(pData[index]);
				dispIdx += sprintf(&cmdMsg[dispIdx], "限制条件: %s\n", ptr);
				index += 1;
				ptr = GetVersionNo(&pData[index], 40);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前版本: %s\n", ptr);

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			docIdx++;
			
			if(CurrCmd == WaterCmd_NoticeUpgrade_OnApp){
				sprintf(cmdName, "通知开始升级-A");
				sprintf(cmdMsg, "");
				Args.buf[i++] = 0x70;		// 命令字	70
			}
			else{
				sprintf(cmdName, "通知开始升级-B");
				sprintf(cmdMsg, "");
				Args.buf[i++] = 0x71;		// 命令字	71
			}
			
			ackLen = 41;				// 应答长度 41	
			// 数据域
			Args.buf[i++] = app->waterMeterVbat;		// 水表电压
			Args.buf[i++] = app->gasMeterVbat;		// 气表电压
			Args.buf[i++] = app->rssi;				// RSSI门限
			Args.buf[i++] = app->snr;				// SNR门限
			memcpy(&Args.buf[i], app->appVer, 40);	// 程序版本 40 byte	
			i += 40;	
			Args.buf[i++] = app->crc16_appVer[0];	// 版本CRC16
			Args.buf[i++] = app->crc16_appVer[1];	
			Args.buf[i++] = app->packetCnt[0];		// 总包数
			Args.buf[i++] = app->packetCnt[1];
			Args.buf[i++] = app->crc16_1st26K[0];	// 前26K CRC16
			Args.buf[i++] = app->crc16_1st26K[1];	
			Args.buf[i++] = app->crc16_2nd26K[0];	// 后26K CRC16
			Args.buf[i++] = app->crc16_2nd26K[1];	
			Args.buf[i++] = app->crc16_all52K[0];	// 总52K CRC16
			Args.buf[i++] = app->crc16_all52K[1];	
			Args.buf[i++] = 0x00;						// RXD信道 0/1
			memset(&Args.buf[i], 0x00, 7);				// 预留 7 byte	
			i += 7;
			Args.lastItemLen = i - 1;
			break;

		case WaterCmd_SendUpgradePacket:			// 发送升级数据
			/*---------------------------------------------*/
			if(cmdState == Cmd_Finish){
				ClearMissPktFlags(pkt);
				IsNoAckCmd = false;
				CurrCmd++;
				cmdState = Cmd_Send;
				docIdx = 0;
				break;
			}
			if(cmdState == Cmd_RecvOk){
				if(sendIdx == pkt->missPktsCnt){
					cmdState = Cmd_Finish;
				}
				break;
			}

			IsNoAckCmd = true;
			pktIdx = pkt->missPkts[sendIdx];
			sprintf(cmdName, "发送升级数据");
			sprintf(cmdMsg, "当前发包：%d/%d", sendIdx + 1, pkt->missPktsCnt);
			sendIdx++;

			Args.buf[i++] = 0x72;		// 命令字	72
			ackLen = 0;					// 应答长度 0	
			// 数据域
			Args.buf[i++] = (uint8)(pktIdx & 0xFF);	// 包序号
			Args.buf[i++] = (uint8)(pktIdx >> 8);
			Args.buf[i++] = app->crc16_appVer[0];	// 版本CRC16
			Args.buf[i++] = app->crc16_appVer[1];	
			u16Tmp = CopyPktToBuf(pkt, pktIdx, &Args.buf[i]); 	// 程序体
			crc16 = GetCrc16(&Args.buf[i], u16Tmp, 0x8408);
			i += u16Tmp;
			Args.buf[i++] = (uint8)(crc16 & 0xFF);	// 程序体CRC16
			Args.buf[i++] = (uint8)(crc16 >> 8);
			Args.lastItemLen = i - 1;
			break;
		
		case WaterCmd_QueryUpgradeStatus_OnBoot:	// 查询升级状态_在boot
			/*---------------------------------------------*/
			if(cmdState == Cmd_Finish){
				GetMissPktList(pkt);
				if(pkt->missPktsCnt > 0){
					CurrCmd = WaterCmd_SendUpgradePacket;
					sendIdx = 0;
				}
				CurrCmd++;
				cmdState = Cmd_Send;
				docIdx = 0;
				break;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeStatus(pData[index]);
				dispIdx += sprintf(&cmdMsg[dispIdx], "升级状态: %s\n", ptr);
				index += 1;
				index += 40;	// skip ver 40 byte
				AddMissPktFlags(pkt, &pData[index], 52);
				GetMissPktList_CurrPkt(&pData[index], 52, pkt->packetCnt, NULL, &u16Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前缺包: %d\n", u16Tmp);

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			if(cmdState == Cmd_RecvNg){
				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}

			docIdx++;
			sprintf(cmdName, "查询升级状态-B");
			sprintf(cmdMsg, "");
			Args.buf[i++] = 0x73;		// 命令字	73
			ackLen = 93;				// 应答长度 93	

			// 数据域
			Args.buf[i++] = app->packetCnt[0];		// 总包数
			Args.buf[i++] = app->packetCnt[1];	
			Args.buf[i++] = app->crc16_appVer[0];	// 版本CRC16
			Args.buf[i++] = app->crc16_appVer[1];	
			Args.lastItemLen = i - 1;
			break;

		case WaterCmd_QueryUpgradeStatus_OnApp:		// 查询升级状态_在app
			/*---------------------------------------------*/
			if(cmdState == Cmd_Finish){
				cmdState = Cmd_Exit;
				break;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeStatus(pData[index]);
				dispIdx += sprintf(&cmdMsg[dispIdx], "升级状态: %s\n", ptr);
				index += 1;
				ptr = GetVersionNo(&pData[index], 40);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前版本: %s\n", ptr);

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			if(cmdState == Cmd_RecvNg){
				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			
			docIdx++;
			sprintf(cmdName, "查询升级状态-A");
			sprintf(cmdMsg, "");
			Args.buf[i++] = 0x74;		// 命令字	74
			ackLen = 41;				// 应答长度 41	

			// 数据域
			Args.buf[i++] = app->packetCnt[0];		// 总包数
			Args.buf[i++] = app->packetCnt[1];	
			Args.buf[i++] = app->crc16_appVer[0];	// 版本CRC16
			Args.buf[i++] = app->crc16_appVer[1];	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		_GUIRectangleFill(0, 3*16, 160, 7*16, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "表号：\r\n  %s", mtrNo);
		PrintfXyMultiLine_VaList(0, 5*16, "--> %s", cmdName);	
		PrintfXyMultiLine_VaList(0, 6*16, "%s", cmdMsg);		

		if(cmdState == Cmd_Exit){
			break;
		}
		if(cmdState == Cmd_RecvOk || cmdState == Cmd_RecvNg){
			_Sleep(500);
			cmdState = Cmd_Send;
			continue;
		}
		if(cmdState == Cmd_Finish){
			_Sleep(500);
			continue;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * AddrLen;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		if(IsNoAckCmd){
			ackLen = 0;
			timeout = 0;
			tryCnt = 1;
		}

		// 发送、接收、结果显示
		cmdResult = CommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);

		if(LcdOpened && lcdCtrl > 4){	
			lcdCtrl = 0;
		}

		if(cmdResult == CmdResult_Cancel){	// 取消
			break;
		}
		else if(cmdResult == CmdResult_Ok){
			cmdState = Cmd_RecvOk;
			u16Tmp = DispBuf[0] + DispBuf[1] * 256;
			pData = &RxBuf[u16Tmp];
		}
		else if(cmdResult == CmdResult_CrcError){
			cmdState = Cmd_RecvNg;
			sprintf(cmdMsg, "命令失败: CRC错误");
		}
		else{
			cmdState = Cmd_RecvNg;
			sprintf(cmdMsg, "命令失败: 等待超时");
		}
	}

	if(cmdResult == CmdResult_Cancel){
		_Printfxy(0, 9*16, "返回  <已取消>  确定", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(100);
		_SoundOff();
		#endif
	}
	else{
		_Printfxy(0, 9*16, "返回  <已完成>  确定", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(200);
		_SoundOff();
		#endif
	}
	

	//sprintf(strTmp, "成功:%d", meters->readOkCnt);
	//_Printfxy(6*16, 7*16 + 8 + 3, strTmp, Color_White);
	
	while(1){
		key = _ReadKey();
		if(key == KEY_CANCEL || KEY_ENTER){
			break;
		}
		_Sleep(100);
	}

	return state;
}

// 查询升级状态
static void UpgradeFunc_QueryUpgradeState(uint8 upgradeMode)
{

}

//------------------------		界面		-----------------
void UpgradeFunc(void)
{
	uint8 key, menuItemNo, u8Tmp;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * fileName, *ptr;
	uint8 version[20];
	uint8 currUi = 0, uiRowIdx;
	uint8 upgradeMode = 1;	// 升级模式： 1 - 单表 ， 2 - 批量
	DocInfo *pDocInfo;

#ifdef Project_6009_RF
	ListBox menuList;
	_ClearScreen();

	ListBoxCreate(&menuList, 16, 3*16, 16, 2, 2, NULL,
		"升级方式", 
		2,
		"1. 单表升级",
		"2. 批量升级"
	);
	key = ShowListBox(&menuList);

	if (key == KEY_CANCEL){	// 返回
		return;
	}
	upgradeMode = menuList.strIdx + 1;
#endif

	version[0] = 0x00;

	// 菜单
	(*pUiCnt) = 0;
	uiRowIdx = 1;
	if(upgradeMode == 1){
		_Printfxy(0, 0, "<<单表升级", Color_White);
	}
	else{
		_Printfxy(0, 0, "<<批量升级", Color_White);
	}
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "1. 选择升级文件");	
	if(upgradeMode == 1){
		LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "2. 输入表号");	
		TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "  > ", StrDstAddr, (AddrLen * 2), (AddrLen * 8 + 8), true);
	}
	else{
		ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "2. 升级档案管理");	
	}
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "3. 开始升级");	
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "4. 查询升级状态");				
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "返回            确定", Color_White);

	while(1){
		_ClearScreen();

		_Printfxy(0, 0, "<<程序升级", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		// ui ctrl
		_Printfxy(0, 7*16 + 8, version, Color_White);
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowUI(UiList, &currUi);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		menuItemNo = (upgradeMode == 1 && currUi > 0) ? currUi : currUi + 1;

		if(upgradeMode == 1){	// 单表升级 表号 
			if(StrDstAddr[0] >= '0' && StrDstAddr[0] <= '9' ){
				DocList.clear(&DocList);
				pDocInfo = (DocInfo *)DocList.add(&DocList, NULL, sizeof(DocInfo));
				memcpy(pDocInfo->mtrNo, StrDstAddr, 20);
				pDocInfo->state = UpgrdState_NotStart;
			}
		}

		if(menuItemNo > 2){
			if(fileName == NULL){
				ShowMsg(16, 4*16, "请先选择升级文件！", 2000);
				continue;
			}
			if(UpgrdDocs.cnt == 0){
				ShowMsg(16, 4*16, "升级表号不能为空！", 2000);
				continue;
			}
		}

		switch (menuItemNo)
		{
		case 1:	// 选择升级文件
			{
				_ClearScreen();
				fileName = _GetFileList("选|\n择|\n升|\n级|\n文|\n件|\n  |\n  |\n  |\n  |\n", "BIN", "");
				if(fileName == NULL){
					continue;
				}
				_ClearScreen();
				_Printfxy(0, 5*16, "  文件校验中... ", Color_White);
				u8Tmp = InitPktInfo(&PktInfo, fileName, 128, 128, &AppInfo);
				if(u8Tmp != 0){
					if(u8Tmp == 2){
						ShowMsg(16, 4*16, "请选择正确的升级文件！", 2000);
					}
					else if(u8Tmp == 3){
						ShowMsg(16, 4*16, "程序体总CRC16错误！", 2000);
					}
					else if(u8Tmp == 4){
						ShowMsg(16, 4*16, "版本号CRC16错误！", 2000);
					}
					fileName = NULL;
					version[0] = 0x00;
				}
				else{
					ptr = GetVersionNo(PktInfo.version, 40);
					sprintf(version, "v%s  CRC:%4X", ptr, PktInfo.verCrc16);
				}
			}
			break;

		case 2:	// 升级档案管理
			{
				if(upgradeMode == 2){
					UpgradeFunc_UpgradeDocMgmt();
				}
				currUi++;
			}
			break;

		case 3:	// 开始升级
			{
				UpgradeFunc_UpgradeStart(upgradeMode);
			}
			break;

		case 4:	// 查询升级状态
			{
				UpgradeFunc_QueryUpgradeState(upgradeMode);
			}
			break;

		default:
			break;
		}

	}
}

//------------------------- 	接口函数		-----------------------

/*
* 描  述：初始化数据包结构
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.all）
*		fileName - 文件名
*		pktSize - 分包大小
*		pktStartIdx - 数据包在文件中的起始位置
*		header - 头部信息，长度 0 -- pktStartIdx
* 返回值：int  - 数据包初始化结果 0 - 成功， 1 - 文件不存在， 
*				2 - app文件错误， 3 - 程序CRC16错误， 4 - 版本CRC16错误
*/
extern int InitPktInfo(PacketInfo *pktInfo, char *fileName, uint16 pktSize, uint32 pktStartIdx, void *header)
{
	int fp;
	uint32 fileLen, readLen;
	uint16 crc16, crc16Keep = 0xFFFF;
	uint8 *tmpBuf = &DispBuf[0], cnt = 0;
	AppFileInfo *app;
	volatile uint32 timeTick;

	if(_Access(fileName, 0) < 0){
		return 1;
	}

	// test -->
	timeTick = _GetTickCount();
	// -->end

	fp = _Fopen(fileName, "R");
	fileLen = _Filelenth(fp);
	if(header != NULL){
		_Fread(header, pktStartIdx, fp);
	}
	_Lseek(fp, pktStartIdx, 0);
	while(0 == _Feof(fp) && cnt < 26){
		readLen = _Fread(tmpBuf, 2048, fp);
		crc16 = GetCrc16_Continue(tmpBuf, readLen, 0x8408, &crc16Keep);
		cnt++;
	}
	_Fclose(fp);

	app = (AppFileInfo *)header;

	// test -->
	timeTick = _GetTickCount() - timeTick;
	_DoubleToStr(&DispBuf[200], (double)timeTick / 32768, 3);
	sprintf(DispBuf, "Tick: %d, time: %s, app-crc16: %4X,  crc16: %4X", 
		timeTick, &DispBuf[200], (app->crc16_all52K[0] + app->crc16_all52K[1] * 256),  crc16);
	ShowMsg(16, 32, DispBuf, 10000);
	// -->end

	if(fileLen <= pktStartIdx 
		|| app->appVer[0] != 'S' || app->appVer[1] != 'R' || app->appVer[2] != 'W' || app->appVer[3] != 'F'){
		return 2;
	}

	if((app->crc16_all52K[0] + app->crc16_all52K[1] * 256) != crc16){
		//return 3;
	}

	crc16 = GetCrc16(app->appVer, 40, 0x8408);
	if((app->crc16_appVer[0] + app->crc16_appVer[1] * 256) != crc16){
		//return 4;
	}

	pktInfo->fileCrc16 = (app->crc16_all52K[0] + app->crc16_all52K[1] * 256);
	pktInfo->verCrc16 = (app->crc16_appVer[0] + app->crc16_appVer[1] * 256);
	pktInfo->version = app->appVer;
	app->crc16_1st26K[0] = 0x00;	// appver str end

	pktInfo->fileName = fileName;
	pktInfo->packetSize = pktSize;
	pktInfo->fileSize = fileLen - pktStartIdx;
	pktInfo->fileKbSize = (uint16)((pktInfo->fileSize + 1023) / 1024);
	pktInfo->packetCnt = (pktInfo->fileSize + pktInfo->packetSize - 1) / pktInfo->packetSize;
	pktInfo->pktStartIdx = pktStartIdx;
	if(pktInfo->fileSize % pktInfo->packetSize != 0){
		pktInfo->lastPktSize = pktInfo->fileSize % pktInfo->packetSize;
	}else{
		pktInfo->lastPktSize = pktInfo->packetSize;
	}
	pktInfo->bitFlagsCnt = (pktInfo->packetCnt + 7) / 8;

	ClearMissPktFlags(pktInfo);

	return 0;
}

/*
* 描  述：拷贝数据包到缓存
* 参  数：
*		pktInfo	- 数据包信息结构 （get）
*		pktIdx - 要拷贝的数据包序号
*		buf - 目的缓存
* 返回值：	int 拷贝的字节数
*/
extern int  CopyPktToBuf(PacketInfo *pktInfo, uint16 pktIdx, uint8 *buf)
{
	int fp;
	int len = (pktInfo->packetCnt - 1 == pktIdx ? pktInfo->lastPktSize : pktInfo->packetSize);

	fp = _Fopen(pktInfo->fileName, "R");
	_Lseek(fp, pktInfo->pktStartIdx + pktInfo->packetSize * pktIdx, 0);
	_Fread(buf, len, fp);
	_Fclose(fp);

	return len;
}

/*
* 描  述：清除缺包标记（缺包数置为总包数，全部初始化为0xFF方便累计,如：0xFF & 0x23 & 0x7F）
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.bitFlags）
* 返回值：void
*/
extern void ClearMissPktFlags(PacketInfo *pktInfo)
{
	memset(pktInfo->bitFlags, 0xFF, pktInfo->bitFlagsCnt);
	pktInfo->missPktsCnt = pktInfo->packetCnt;
}

/*
* 描  述：添加缺包标记
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.bitFlags）
*		bitflags - 当前缺包标记数据缓存，添加/合并到总标记中
*		byteCnt - 当前缺包标记字节数
* 返回值：void
*/
extern void AddMissPktFlags(PacketInfo *pktInfo, uint8 *bitflags, uint16 byteCnt)
{
	uint8 * p8 = &pktInfo->bitFlags[0];

	while(byteCnt > 0){
		*p8++ &= *bitflags++;
		byteCnt--;
	}

	pktInfo->missPktsCnt = 0xFFFF;	// set to unknown
}

/*
* 描  述：获取缺包列表 - 累计所有包的缺包数
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.missPktsCnt，set pktInfo.missPkts）
* 返回值：void
*/
extern void GetMissPktList(PacketInfo *pktInfo)
{
	uint16 i, k, pktIdx = 0;
	uint8 b;
	pktInfo->missPktsCnt = 0;

	for(i = 0; i < pktInfo->bitFlagsCnt; i++){
		b = pktInfo->bitFlags[i];
		if (b == 0xFF){
			pktIdx += 8;
			continue;
		}
		for (k = 0; k < 8; k++){
			if (pktIdx >= pktInfo->packetCnt){
				break;
			}
			if ((b & 0x01) == 0){
				pktInfo->missPkts[pktInfo->missPktsCnt++] = pktIdx;
			}
			b = (uint8)(b >> 1);
			pktIdx++;
		}
	}
}

/*
* 描  述：获取缺包列表	- 当前包的缺包数
* 参  数：
*		bitfalgs	- 缺包标记缓存
*		byteCnt		- 缺包标记缓存字节数
*		pktCnt		- 总包数
*		missPkts	- 缺包列表缓存, 若为NULL，则只返回缺包数
*		missCnt		- 缺包数
* 返回值：void
*/
extern void GetMissPktList_CurrPkt(uint8 *bitfalgs, uint16 byteCnt, uint16 pktCnt, uint16 *missPkts, uint16 *missCnt)
{
	uint16 i, k, pktIdx = 0;
	uint8 b;
	*missCnt = 0;

	for(i = 0; i < byteCnt; i++){
		b = bitfalgs[i];
		if (b == 0xFF){
			pktIdx += 8;
			continue;
		}
		for (k = 0; k < 8; k++){
			if (pktIdx >= pktCnt){
				break;
			}
			if ((b & 0x01) == 0){
				if(missPkts != NULL){
					missPkts[*missCnt] = pktIdx;
				}
				(*missCnt)++;
			}
			b = (uint8)(b >> 1);
			pktIdx++;
		}
	}
}

