#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "Upgrade.h"
#include "Common.h"
#include "WaterMeter.h"
#include "List.h"

#ifdef MeterDocDBF_H
extern uint8 **MetersStrs;		// import from "MeterDocDBF.h"
#else
uint8 **MetersStrs = &DispBuf;
#endif

//-------------------------		全局变量	-----------------
AppFileInfo AppInfo;
PacketInfo PktInfo;
TList DocList;
uint8 IsInBoot;

//-----------------------		内部函数	------------------

// 从版本信息中截取版本号: 如 1.20
static void GetVersionNo(uint8 *buf, uint8 *verNo)
{
	uint8 i, *ptr = NULL;

	for(i = 0; i < 40; i++){

		if(buf[i] == (uint8)'.'){
			ptr = verNo;
			*ptr++ = buf[i - 1];
		}
		if(buf[i] == (uint8)' '){
			break;
		}

		if(ptr){
			*ptr++ = buf[i];
		}
	}

	if(ptr){
		*ptr = 0x00;
	}
	else{
		verNo[0] = 0x00;
	}
}

static bool IsMtrNoEqual(void *node, void *mtrNo)
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
* 获取升级状态
*/
char * GetUpgradeStatus(uint8 code)
{
	char * str = NULL;
	
	switch (code){
	case UpgrdState_Unknow: str = "未知"; break;
	case UpgrdState_Err_5_0v: str = "E-5.0v"; break;
	case UpgrdState_Err_3_4v: str = "E-3.4v"; break;
	case UpgrdState_Err_SNR: str = "E-SNR"; break;
	case UpgrdState_Err_RSSI: str = "E-RSSI"; break;
	case UpgrdState_Err_VerNo: str = "E-VerNo"; break;
	case UpgrdState_Err_VerCrc: str = "E-CrcV"; break;
	case UpgrdState_Err_AppCrc: str = "E-CrcA"; break;
	case UpgrdState_Err_PktCnt: str = "E-Cnt"; break;
	case UpgrdState_NotStart: str = "未开始"; break;
	case UpgrdState_PktWait: str = "等待"; break;
	case UpgrdState_Finish: str = "完成"; break;
	default: break;
	}

	return str;
}

/*
* 描述：显示升级档案列表 （查看/删除）
* 参数：docs - 档案信息列表
*		mode - 显示模式：0 - 查看档案， 1 - 删除档案
* 返回：uint8 - 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
static uint8 ShowDocList(TList *docs, uint8 mode)
{
	uint8 key;
	uint8 **strs = MetersStrs;
	uint8 strSize = 20, *ptr, *pList;
	uint16 currIdx = 0;
	ListBox docList;
	DocInfo *docItem;

	if(docs->cnt > Upgrd_MeterMax) return KEY_CANCEL;

	do{

		pList = (uint8 *)strs;
		docItem = (DocInfo *)docs->head;
		
		while(docItem != NULL){
			ptr = GetUpgradeStatus(docItem->state);
			sprintf(pList, "%s  %s", docItem->mtrNo, ptr);
			pList += strSize;
			docItem = docItem->next;
		}

		ListBoxCreateEx(&docList, 0, 0, 20, 7, docs->cnt, NULL,
				"<<档案列表", strs, strSize, docs->cnt);
		if(mode == 1){
			_Printfxy(0, 9*16, "返回            删除", Color_White);
		}
		else{
			_Printfxy(0, 9*16, "返回            确定", Color_White);
		}
		
		docList.currIdx = currIdx;
		key = ShowListBox(&docList);
		currIdx = docList.currIdx;

		if(key == KEY_ENTER && mode == 1){
			docs->removeAt(docs, currIdx);
		}
		else{
			break;
		}
	}while(mode == 1);	// 删除档案后继续显示列表

	return key;
}

/*
* 描述：导入档案列表
* 参数：docs - 档案信息列表
* 返回：void
*/
static void DocListImport(TList *docs)
{
	int fp, len, dstIdx, startIdx, lastCnt;
	uint8 *str, *buf = &ArgBuf[0];
	DocInfo *docItem;

	if(_Access(Upgrd_DocFileName, 0) < 0){
		sprintf(buf, " 档案文件 %s 不存在！", Upgrd_DocFileName);
		ShowMsg(16, 4*16, buf, 1000);
		return;
	}

	lastCnt = docs->cnt;   

	fp = _Fopen(Upgrd_DocFileName, "R");
	_Lseek(fp, 0, 0);

	len = 0;
	startIdx = 0;

	while(!_Feof(fp)){
		len = _Fread(buf, 1024, fp) + (len - startIdx);
		dstIdx = 0;
		startIdx = 0;

		while(dstIdx >= 0){
			dstIdx = IndexOf(buf, len, "\r\n", 2, startIdx, 25);
			str = &buf[startIdx];
			if(dstIdx >=0){
				buf[startIdx + 12] = '\0';
				if(NULL == docs->find(docs, IsMtrNoEqual, str)){
					docItem = docs->add(docs, NULL, sizeof(DocInfo));
					sprintf(docItem->mtrNo, "%s", str);
					docItem->state = UpgrdState_Unknow;
				}

				startIdx = dstIdx + 2;
			}
		}
		
		if(startIdx < len){
			memcpy(&buf[0], &buf[startIdx], len - startIdx);
			buf = &buf[len - startIdx];
		}
	}
	_Fclose(fp);

	sprintf(&buf[0], "导入完成！新增 %d, 当前总数 %d", (docs->cnt - lastCnt), docs->cnt);
	ShowMsg(16, 4*16, &buf[0], 1000);
}

/*
* 描述：导出档案列表
* 参数：docs - 档案信息列表
* 返回：void
*/
static void DocListExport(TList *docs)
{
	int fp, len;
	uint8 *buf = &ArgBuf[0];
	DocInfo *docItem = (DocInfo *)docs->head;

	if(docs->cnt == 0) {
		ShowMsg(16, 4*16, "当前档案为空！请先添加或导入", 1000);
		return;
	}

	if(_Access(Upgrd_DocFileName, 0) == 0){
		_Remove(Upgrd_DocFileName);
	}
	fp = _Fopen(Upgrd_DocFileName, "W");
	_Lseek(fp, 0, 0);
	while(docItem != NULL){
		len = sprintf(buf, "%s,%s\r\n", docItem->mtrNo, GetUpgradeStatus(docItem->state));
		_Fwrite(buf, len, fp);
		docItem = docItem->next;
	}
	_Fclose(fp);

	sprintf(&buf[0], "导出完成！总数 %d", docs->cnt);
	ShowMsg(16, 4*16, &buf[0], 1000);
}

// 升级档案管理
static void UpgradeFunc_UpgradeDocMgmt(void)
{
	uint8 key, menuItemNo;
	UI_Item uiTxtbox;
	ListBox menuList;
	TList *docs = &DocList;
	DocInfo *docItem;

	if(docs->cnt == 0){
		List_Init(docs);
	}

	ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
		"<<升级档案管理", 
		6,
		"1. 添加档案",
		"2. 查看档案",
		"3. 删除档案",
		"4. 清空档案",
		"5. 导入档案",
		"6. 导出档案"
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
				TextBoxCreate(&uiTxtbox, 0, 2*16 + 8, "  > ", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);

				while(2){
					_Printfxy(0, 0, "<<添加档案", Color_White);
					_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
					/*---------------------------------------------*/
					_Printfxy(0, 1*16 + 8, "表号：", Color_White);
					_Printfxy(uiTxtbox.x, uiTxtbox.y, uiTxtbox.title, Color_White);
					_Printfxy(0, 5*16 + 8, "                    ", Color_White);
					PrintfXyMultiLine_VaList(0, 6*16 + 8, "当前档案总数：%d", docs->cnt);
					//----------------------------------------------
					_GUIHLine(0, 9*16 - 4, 160, Color_Black);
					_Printfxy(0, 9*16, "返回            添加", Color_White);
					key = GetInputNumStr(&uiTxtbox);
					//------------------------------------------------------------
					if (key == KEY_CANCEL){	// 返回
						break;
					}

					_leftspace(StrDstAddr, AddrLen * 2, '0');

					if(StrDstAddr[0] >= '0' && StrDstAddr[0] <= '9' ){
						if(NULL != docs->find(docs, IsMtrNoEqual, StrDstAddr)){
							_Printfxy(0, 5*16 + 8, "    表号重复！  ", Color_White);
							_Sleep(500);
							continue;
						}
						docItem = (DocInfo *)docs->add(docs, NULL, sizeof(DocInfo));
						memcpy(docItem->mtrNo, StrDstAddr, 20);
						docItem->state = UpgrdState_Unknow;

						_Printfxy(0, 5*16 + 8, "    添加成功！  ", Color_White);
						PrintfXyMultiLine_VaList(0, 6*16 + 8, "当前档案总数：%d", docs->cnt);
						_Sleep(500);
					}
				}
			}
			break;
		
		case 2:	// 查看档案列表
			{
				key = ShowDocList(docs, 0);
			}
			break;

		case 3:	// 删除档案
			{
				key = ShowDocList(docs, 1);
			}
			break;

		case 4:	// 清空档案
			{
				if(docs->cnt == 0) {
					ShowMsg(16, 4*16, "当前档案为空！请先添加或导入", 1000);
					break;
				}
				ShowMsg(16, 4 * 16, "  确定要清空吗？ ", 50);
				key = _ReadKey();
				if(key != KEY_ENTER){
					break;
				}
				ShowMsg(16, 4 * 16, "  档案清空中... ", 50);
				docs->clear(docs);
				ShowMsg(16, 4 * 16, "  档案清空完成！ ", 1000);
			}
			break;

		case 5:	// 导入档案
			{
				DocListImport(docs);
			}
			break;

		case 6:	// 导出档案
			{
				DocListExport(docs);
			}
			break;

		default:
			break;
		}
	}
}


// 开始升级
static void UpgradeFunc_UpgradeStart(uint8 upgradeMode)
{
	uint8 flags[FLG_Max];
	uint16 cnts[FLG_Max];
	AppFileInfo *app = &AppInfo;
	PacketInfo *pkt = &PktInfo;
	TList *docs = &DocList;
	DocInfo *docItem;
	uint16 docIdx, sendIdx, pktIdx, crc16, i, u16Tmp;
	uint8 *verNo = &ArgBuf[1000], *cmdName = &ArgBuf[1024], *cmdMsg = &ArgBuf[1060];
	uint8 cmdState = Cmd_Send;
	uint16 ackLen, timeout, dispIdx, index;
	uint8 tryCnt, lcdCtrl, key, reSendPktCnt;
	CmdResult cmdResult = CmdResult_Ok;
	uint32 shutdownTime;
	uint8 *pData, *ptr, u8Tmp;

	#if Upgrd_LOG_ON
	LogPrint("[  upgrade enter  ]\n");
	#endif

	_ClearScreen();

	// 防止自动抄表时关机，重置自动关机时间
	shutdownTime = _GetShutDonwTime();
	if(shutdownTime == 0) shutdownTime = 120;	// 120s
	_SetShutDonwTime(0);		// 20 - 999 有效，0 - 关闭自动关机

	_Printfxy(0, 0, "<<开始升级", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	GetVersionNo(pkt->version, verNo);
	PrintfXyMultiLine_VaList(0, 2*16, "版本:%s  CRC:%4X", verNo, pkt->verCrc16);
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

	docItem = (DocInfo *)docs->head;
	if(upgradeMode == 1 && IsInBoot){
		if(docItem->state == UpgrdState_PktWait){
			CurrCmd = WaterCmd_QueryUpgradeStatus_OnBoot;
			ClearMissPktFlags(pkt);
		}else if(docItem->state == UpgrdState_Err_VerCrc
			|| docItem->state == UpgrdState_Err_PktCnt
			|| docItem->state == UpgrdState_Err_AppCrc
			){
			CurrCmd = WaterCmd_NoticeUpgrade_OnBoot;
		}
		else{
			// pass
		}
	}
	IsNoAckCmd = false;
	docIdx = 0;
	reSendPktCnt = 0;
	lcdCtrl = 0;
	memset(flags, 0x00, FLG_Max);

	while(1){
	
		if(cmdState == Cmd_Send){

			// LCD背景灯控制
			LcdLightCycleCtrl(&lcdCtrl, 4);

			// 当前表号
			if(upgradeMode == 2 && CurrCmd == WaterCmd_SendUpgradePacket){
				strncpy(StrDstAddr, "FFFFFFFFFFFFFFFF", AddrLen * 2);
			}
			else{
				docItem = (DocInfo *)docs->itemAt(docs, docIdx);
				strcpy(StrDstAddr, docItem->mtrNo);
			}
		
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
					if(flags[FLG_NotStart] == 1){
						CurrCmd = WaterCmd_SendUpgradePacket;
						GetMissPktList(pkt);
						_Sleep(500);
					}
					else{
						cmdState = Cmd_Exit;
						break;
					}
				}
				else{ // WaterCmd_NoticeUpgrade_OnApp
					if(flags[FLG_NotStart] == 1){
						CurrCmd = WaterCmd_NoticeUpgrade_OnBoot;
						cmdState = Cmd_Send;
						_Sleep(4000);
					}
					else if(flags[FLG_Unknow] == 1){
						CurrCmd = WaterCmd_QueryUpgradeStatus_OnBoot;
						cmdState = Cmd_Send;
						ClearMissPktFlags(pkt);
						_Sleep(3000);
					}
					else{
						cmdState = Cmd_Exit;
						break;
					}
				}
				sendIdx = 0;
				docIdx = 0;
				memset(flags, 0x00, FLG_Max);
				continue;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeForbidReason(pData[index], &u8Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "限制条件: %s\n", ptr);
				index += 1;
				GetVersionNo(&pData[index], docItem->ver);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前版本: %s\n", docItem->ver);
				if(u8Tmp == 0x00) {
					docItem->state = UpgrdState_NotStart;
					flags[FLG_NotStart] = 1;
				}
				else{
					switch(u8Tmp){
					case 0x01:	docItem->state = UpgrdState_Err_5_0v;	break;
					case 0x02:	docItem->state = UpgrdState_Err_3_4v;	break;
					case 0x04:	docItem->state = UpgrdState_Err_SNR;	break;
					case 0x08:	docItem->state = UpgrdState_Err_RSSI;	break;
					case 0x10:	docItem->state = UpgrdState_Err_VerNo;	break;
					case 0x20:	docItem->state = UpgrdState_Err_VerCrc;	break;
					default: break;
					}

					flags[FLG_Forbid] = 1;
				}

				IsInBoot = (CurrCmd == WaterCmd_NoticeUpgrade_OnBoot ? true : false);

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			if(cmdState == Cmd_RecvNg){
				if(CurrCmd == WaterCmd_NoticeUpgrade_OnApp){
					docItem->state = UpgrdState_Unknow;
				}

				flags[FLG_Unknow] = 1;

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}

			#if Upgrd_LOG_ON
			LogPrint("notice on app/boot , docidx: %d", docIdx);
			#endif

			docIdx++;
			tryCnt = 3;
			
			if(CurrCmd == WaterCmd_NoticeUpgrade_OnApp){
				#ifdef Project_6009_IR
				timeout = 2000;
				#else
				timeout = 8000;
				//timeout = 1000;		// test
				#endif
				sprintf(cmdName, "通知开始升级-A");
				sprintf(cmdMsg, "      执行中...  ");
				Args.buf[i++] = 0x70;		// 命令字	70

				#if Upgrd_LOG_ON
				LogPrint("notice on app , docidx: %d, doing...", docIdx -1);
				#endif
			}
			else{
				#ifdef Project_6009_IR
				timeout = 4000;
				#else
				timeout = 8000;
				//timeout = 1000;		// test
				#endif
				if(docItem->state == UpgrdState_NotStart
					|| docItem->state == UpgrdState_Err_VerCrc
					|| docItem->state == UpgrdState_Err_PktCnt
					|| docItem->state == UpgrdState_Err_AppCrc
					){
					// pass
				}
				else{
					continue;
				}
				sprintf(cmdName, "通知开始升级-B");
				sprintf(cmdMsg, "      执行中...  ");
				Args.buf[i++] = 0x71;		// 命令字	71

				#if Upgrd_LOG_ON
				LogPrint("notice on boot , docidx: %d, doing...", docIdx -1);
				#endif
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
				CurrCmd = WaterCmd_QueryUpgradeStatus_OnBoot;
				cmdState = Cmd_Send;
				ClearMissPktFlags(pkt);
				docIdx = 0;
				IsNoAckCmd = false;
				_Sleep(5000);
				continue;
			}
			if(cmdState == Cmd_RecvOk){		// no ack , default is ok
				IsInBoot = true;
				if(sendIdx == pkt->missPktsCnt){
					cmdState = Cmd_Finish;
				}
				break;
			}

			pktIdx = pkt->missPkts[sendIdx];
			sendIdx++;
			IsNoAckCmd = true;
			tryCnt = 1;

			#if Upgrd_LOG_ON
			LogPrint("send pkt on boot , sendIdx: %d, pktIdx: %d, doing...", sendIdx - 1, pktIdx);
			#endif

			sprintf(cmdName, "发送升级数据");
			sprintf(cmdMsg, "      执行中...  \n当前发包：%d/%d", sendIdx, pkt->missPktsCnt);

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
				if(flags[FLG_NotStart] == 1 || flags[FLG_Error] == 1){
					CurrCmd = WaterCmd_NoticeUpgrade_OnBoot;
				}
				else if(flags[FLG_PktWait] == 1
					&& pkt->missPktsCnt > 0
					&& (++reSendPktCnt) < Upgrd_ReSendPktMax
				){
					CurrCmd = WaterCmd_SendUpgradePacket;

					#if Upgrd_LOG_ON
					u16Tmp = 0;
					for(i = 0; i < 10 && i < pkt->missPktsCnt; i++){
					u16Tmp += sprintf(&TmpBuf[u16Tmp], "%d  ", pkt->missPkts[i]);
					}
					LogPrint("miss pkt cnt: %d,  miss pkts : %s", pkt->missPktsCnt, TmpBuf);
					#endif
				}
				else if(flags[FLG_Finish] == 1 || flags[FLG_Unknow] == 1){
					CurrCmd = WaterCmd_QueryUpgradeStatus_OnApp;
				}
				else{
					cmdState = Cmd_Exit;
					break;
				}

				memset(flags, 0x00, FLG_Max);
				cmdState = Cmd_Send;
				sendIdx = 0;
				docIdx = 0;
				_Sleep(1000);
				continue;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeStatus(pData[index], &u8Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "升级状态: %s\n", ptr);
				index += 1;
				GetVersionNo(&pData[index], docItem->ver);
				index += 40;	
				AddMissPktFlags(pkt, &pData[index], pkt->bitFlagsCnt);
				GetMissPktList_CurrPkt(&pData[index], pkt->bitFlagsCnt, pkt->packetCnt, NULL, &u16Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前缺包: %d\n", u16Tmp);
				switch (u8Tmp)
				{
				case 0x01: docItem->state = UpgrdState_NotStart; flags[FLG_NotStart] = 1; break;
				case 0x02: docItem->state = UpgrdState_PktWait; flags[FLG_PktWait] = 1; break;
				case 0x04: docItem->state = UpgrdState_Finish; flags[FLG_Finish] = 1; break;
				case 0x08: docItem->state = UpgrdState_Err_AppCrc; flags[FLG_Error] = 1; break;
				case 0x10: docItem->state = UpgrdState_Err_VerCrc; flags[FLG_Error] = 1; break;
				case 0x20: docItem->state = UpgrdState_Err_PktCnt; flags[FLG_Error] = 1; break;
				default: break;
				}

				IsInBoot = true;

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			if(cmdState == Cmd_RecvNg){
				docItem->state = UpgrdState_Unknow;
				flags[FLG_Unknow] = 1; 

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}

			#if Upgrd_LOG_ON
			LogPrint("query on boot , docIdx: %d", docIdx);
			#endif

			docIdx++;
			tryCnt = 3;
			#ifdef Project_6009_IR
			timeout = 2000;
			#else
			timeout = 8000;
			//timeout = 1000;		// test
			#endif

			if(docItem->state == UpgrdState_Unknow 
				|| docItem->state == UpgrdState_NotStart
				|| docItem->state == UpgrdState_PktWait){
				// pass
			}
			else{
				continue;
			}

			sprintf(cmdName, "查询升级状态-B");
			sprintf(cmdMsg, "      执行中...  ");
			Args.buf[i++] = 0x73;		// 命令字	73
			ackLen = 93;				// 应答长度 93	

			#if Upgrd_LOG_ON
			LogPrint("query on boot , docIdx: %d, doing...", docIdx - 1);
			#endif

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
				memset(flags, 0x00, FLG_Max);
				cmdState = Cmd_Exit;
				break;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeStatus(pData[index], &u8Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "升级状态: %s\n", ptr);
				index += 1;
				GetVersionNo(&pData[index], docItem->ver);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前版本: %s\n", docItem->ver);
				switch (u8Tmp)
				{
				case 0x01: docItem->state = UpgrdState_NotStart; flags[FLG_NotStart] = 1; break;
				case 0x02: docItem->state = UpgrdState_PktWait; flags[FLG_PktWait] = 1; break;
				case 0x04: docItem->state = UpgrdState_Finish; flags[FLG_Finish] = 1; break;
				case 0x08: docItem->state = UpgrdState_Err_AppCrc; flags[FLG_Error] = 1; break;
				case 0x10: docItem->state = UpgrdState_Err_VerCrc; flags[FLG_Error] = 1; break;
				case 0x20: docItem->state = UpgrdState_Err_PktCnt; flags[FLG_Error] = 1; break;
				default: break;
				}

				IsInBoot = false;

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

			#if Upgrd_LOG_ON
			LogPrint("query on app , docIdx: %d", docIdx);
			#endif
			
			docIdx++;
			tryCnt = 3;
			#ifdef Project_6009_IR
			timeout = 2000;
			#else
			timeout = 8000;
			//timeout = 1000;		// test
			#endif

			if(docItem->state == UpgrdState_Unknow 
				|| docItem->state == UpgrdState_NotStart
				|| docItem->state == UpgrdState_Finish){
				// pass
			}
			else{
				continue;
			}

			sprintf(cmdName, "查询升级状态-A");
			sprintf(cmdMsg, "      执行中...  ");
			Args.buf[i++] = 0x74;		// 命令字	74
			ackLen = 41;				// 应答长度 41	

			#if Upgrd_LOG_ON
			LogPrint("query on app , docIdx: %d, doing...", docIdx - 1);
			#endif

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

		if(cmdState == Cmd_Send){
			_GUIRectangleFill(0, 3*16, 160, 7*16, Color_White);
			PrintfXyMultiLine_VaList(0, 3*16, "表号：\r\n  > %s", StrDstAddr);
			PrintfXyMultiLine_VaList(0, 5*16, "--> %s", cmdName);
			_Printfxy(0, 9*16, "返回  <执行中>      ", Color_White);
		}
		else{
			_Printfxy(0, 6*16, cmdMsg, Color_White);
		}
		
		if(cmdState == Cmd_RecvOk || cmdState == Cmd_RecvNg){
			cmdState = Cmd_Send;
			continue;
		}
		else if(cmdState == Cmd_Finish){
			continue;
		}
		else if(cmdState == Cmd_Exit){
			break;
		}
		else{
			// cmd send as follow
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * AddrLen;
		
		if(IsNoAckCmd){
			ackLen = 0;
			timeout = 50;
			tryCnt = 1;
		}

		// 发送、接收
		cmdResult = CommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);

		if(cmdResult == CmdResult_Cancel){	// 取消
			break;
		}
		else if(cmdResult == CmdResult_Ok){
			cmdState = Cmd_RecvOk;
			if(IsNoAckCmd == false){
				u16Tmp = DispBuf[0] + DispBuf[1] * 256;		// rx data area idx
				pData = &RxBuf[u16Tmp];
			}
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

	IsNoAckCmd = false;
	_OpenLcdBackLight();

	if(cmdResult == CmdResult_Cancel){
		_Printfxy(0, 9*16, "返回  <已取消>  确定", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(100);
		_SoundOff();
		#endif
	}
	else{
		_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(200);
		_SoundOff();
		#endif
	}
	
	// show result count
	if(upgradeMode == 1 && cmdResult != CmdResult_Cancel){
		ptr = GetUpgradeStatus(docItem->state);
		PrintfXyMultiLine_VaList(0, 5*16, " 升级状态: %s\n 当前版本: %s\n 当前缺包: %d\n", 
				ptr, docItem->ver, pkt->missPktsCnt);
		do{
			key = _ReadKey();
			_Sleep(100);
		}while(key != KEY_CANCEL && key != KEY_ENTER);
	}
	else if(upgradeMode == 2 && cmdResult != CmdResult_Cancel){
		memset(cnts, 0x00, FLG_Max * 2);
		docItem = (DocInfo *)docs->head;
		while(docItem != NULL){
			switch (docItem->state){
			case UpgrdState_Unknow: cnts[UpgrdState_Unknow]++; break;
			case UpgrdState_Err_5_0v: cnts[FLG_Forbid]++; break;
			case UpgrdState_Err_3_4v: cnts[FLG_Forbid]++; break;
			case UpgrdState_Err_SNR: cnts[FLG_Forbid]++; break;
			case UpgrdState_Err_RSSI: cnts[FLG_Forbid]++; break;
			case UpgrdState_Err_VerNo: cnts[FLG_Forbid]++; break;
			case UpgrdState_Err_VerCrc: cnts[FLG_Forbid]++; break;
			case UpgrdState_Err_AppCrc: cnts[FLG_Error]++; break;
			case UpgrdState_Err_PktCnt: cnts[FLG_Error]++; break;
			case UpgrdState_NotStart: cnts[FLG_NotStart]++; break;
			case UpgrdState_PktWait: cnts[FLG_PktWait]++; break;
			case UpgrdState_Finish: cnts[FLG_Finish]++; break;
			default: cnts[UpgrdState_Unknow]++; break;
			}
			docItem = docItem->next;
		}

		PrintfXyMultiLine_VaList(0, 1*16 + 8, " 档案总数: %d\n 升级成功: %d\n 缺包等待: %d\n 未开始  : %d\n 升级错误: %d\n 禁止升级: %d\n 未知状态: %d\n", 
				docs->cnt, cnts[FLG_Finish], cnts[FLG_PktWait], cnts[FLG_NotStart]
				, cnts[FLG_Error], cnts[FLG_Forbid], cnts[FLG_Unknow]);
		_Printfxy(0, 9*16, "返回  < 完成 >  详情", Color_White);
		do{
			key = _ReadKey();
			_Sleep(100);
		}while(key != KEY_CANCEL && key != KEY_ENTER);

		if(key == KEY_ENTER){
			key = ShowDocList(docs, 0);
		}
	}

	_SetShutDonwTime(shutdownTime);

	#if Upgrd_LOG_ON
	LogPrint("[  upgrade exit  ]\n");
	#endif
}

// 查询升级状态
static void UpgradeFunc_QueryUpgradeState(uint8 upgradeMode)
{
	uint8 flagUnknow = 0;
	AppFileInfo *app = &AppInfo;
	PacketInfo *pkt = &PktInfo;
	TList *docs = &DocList;
	DocInfo *docItem;
	uint16 docIdx, i, u16Tmp;
	uint8 *verNo = &ArgBuf[1000], *cmdName = &ArgBuf[1024], *cmdMsg = &ArgBuf[1060];
	uint8 cmdState = Cmd_Send;
	uint16 ackLen, timeout, dispIdx, index;
	uint8 tryCnt, lcdCtrl, key;
	CmdResult cmdResult = CmdResult_Ok;
	uint32 shutdownTime;
	uint8 *pData, *ptr, u8Tmp;

	#if Upgrd_LOG_ON
	LogPrint("[  query enter  ]\n IsNoAckCmd: %d ", IsNoAckCmd);
	#endif

	_ClearScreen();

	// 防止自动抄表时关机，重置自动关机时间
	shutdownTime = _GetShutDonwTime();
	if(shutdownTime == 0) shutdownTime = 120;
	_SetShutDonwTime(0);		// 20 - 999 有效，0 - 关闭自动关机

	_Printfxy(0, 0, "<<查询升级状态", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	GetVersionNo(pkt->version, verNo);
	PrintfXyMultiLine_VaList(0, 2*16, "版本:%s  CRC:%4X", verNo, pkt->verCrc16);
	// to show meter no
	// to show cmd name
	// to show cmd msg
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "返回   <查询中>   ", Color_White);


	// 中继清空
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}

	if(upgradeMode == 1 && ((DocInfo *)docs->head)->state == UpgrdState_PktWait){
		CurrCmd = WaterCmd_QueryUpgradeStatus_OnBoot;
		ClearMissPktFlags(pkt);
	}
	else{
		CurrCmd = WaterCmd_QueryUpgradeStatus_OnApp;
	}
	docIdx = 0;
	lcdCtrl = 0;

	while(1){
	
		if(cmdState == Cmd_Send){

			// LCD背景灯控制
			LcdLightCycleCtrl(&lcdCtrl, 4);

			// 当前表号
			docItem = (DocInfo *)docs->itemAt(docs, docIdx);
			strcpy(StrDstAddr, docItem->mtrNo);
		
			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
		}

		switch(CurrCmd){
		case WaterCmd_QueryUpgradeStatus_OnBoot:	// 查询升级状态_在boot
			/*---------------------------------------------*/
			if(cmdState == Cmd_Finish){
				GetMissPktList(pkt);
				cmdState = Cmd_Exit;
				docIdx = 0;
				break;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeStatus(pData[index], &u8Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "升级状态: %s\n", ptr);
				index += 1;
				GetVersionNo(&pData[index], docItem->ver);
				index += 40;
				AddMissPktFlags(pkt, &pData[index], pkt->bitFlagsCnt);
				GetMissPktList_CurrPkt(&pData[index], pkt->bitFlagsCnt, pkt->packetCnt, NULL, &u16Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前缺包: %d\n", u16Tmp);
				switch (u8Tmp)
				{
				case 0x01: docItem->state = UpgrdState_NotStart; break;
				case 0x02: docItem->state = UpgrdState_PktWait; break;
				case 0x04: docItem->state = UpgrdState_Finish; break;
				case 0x08: docItem->state = UpgrdState_Err_AppCrc; break;
				case 0x10: docItem->state = UpgrdState_Err_VerCrc; break;
				case 0x20: docItem->state = UpgrdState_Err_PktCnt; break;
				default: break;
				}

				IsInBoot = true;

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

			#if Upgrd_LOG_ON
			LogPrint("query on boot , docidx: %d", docIdx);
			#endif

			docIdx++;
			if(docItem->state != UpgrdState_Unknow && docItem->state != UpgrdState_PktWait){
				continue;
			}
			sprintf(cmdName, "查询升级状态-B");
			sprintf(cmdMsg, "      执行中...  ");
			Args.buf[i++] = 0x73;		// 命令字	73
			ackLen = 93;				// 应答长度 93	

			#if Upgrd_LOG_ON
			LogPrint("query on boot , docidx: %d, doing...", docIdx -1);
			#endif

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
				if(flagUnknow){
					CurrCmd = WaterCmd_QueryUpgradeStatus_OnBoot;
					cmdState = Cmd_Send;
					docIdx = 0;
					continue;
				}
				else{
					cmdState = Cmd_Exit;
				}
				break;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeStatus(pData[index], &u8Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "升级状态: %s\n", ptr);
				index += 1;
				GetVersionNo(&pData[index], docItem->ver);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前版本: %s\n", docItem->ver);
				switch (u8Tmp)
				{
				case 0x01: docItem->state = UpgrdState_NotStart; break;
				case 0x02: docItem->state = UpgrdState_PktWait; break;
				case 0x04: docItem->state = UpgrdState_Finish; break;
				case 0x08: docItem->state = UpgrdState_Err_AppCrc; break;
				case 0x10: docItem->state = UpgrdState_Err_VerCrc; break;
				case 0x20: docItem->state = UpgrdState_Err_PktCnt; break;
				default: break;
				}
				flagUnknow = 0;
				IsInBoot = false;

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			if(cmdState == Cmd_RecvNg){
				docItem->state = UpgrdState_Unknow;
				flagUnknow = 1;
				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			
			docIdx++;
			sprintf(cmdName, "查询升级状态-A");
			sprintf(cmdMsg, "      执行中...  ");
			Args.buf[i++] = 0x74;		// 命令字	74
			ackLen = 41;				// 应答长度 41	

			#if Upgrd_LOG_ON
			LogPrint("query on app , docidx: %d, doing...", docIdx -1);
			#endif

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

		if(cmdState == Cmd_Send){
			_GUIRectangleFill(0, 3*16, 160, 7*16, Color_White);
			PrintfXyMultiLine_VaList(0, 3*16, "表号：\r\n  > %s", StrDstAddr);
			PrintfXyMultiLine_VaList(0, 5*16, "--> %s", cmdName);
			_Printfxy(0, 9*16, "返回  <执行中>      ", Color_White);
		}
		else{
			_Printfxy(0, 6*16, cmdMsg, Color_White);
		}		

		if(cmdState == Cmd_RecvOk || cmdState == Cmd_RecvNg){
			_Sleep(500);
			cmdState = Cmd_Send;
			continue;
		}
		else if(cmdState == Cmd_Finish){
			continue;
		}
		else if(cmdState == Cmd_Exit){
			break;
		}
		else{
			// cmd send as follow
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * AddrLen;
		#ifdef Project_6009_IR
		timeout = 2000;
		#else
		//timeout = 8000;
		timeout = 1000;		// test
		#endif
		tryCnt = 3;

		// 发送、接收
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
			cmdState = Cmd_Exit;
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
		_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(200);
		_SoundOff();
		#endif
	}
	
	if(upgradeMode == 1){
		ptr = GetUpgradeStatus(docItem->state);
		if(CurrCmd == WaterCmd_QueryUpgradeStatus_OnBoot && ! flagUnknow){
			PrintfXyMultiLine_VaList(0, 5*16, " 升级状态: %s\n 当前版本: %s\n 当前缺包: %d\n   ", 
				ptr, docItem->ver, pkt->missPktsCnt);
		}
		else{
			PrintfXyMultiLine_VaList(0, 5*16, " 升级状态: %s\n 当前版本: %s\n                  ", 
				ptr, docItem->ver);
		}
		
		do{
			key = _ReadKey();
			_Sleep(100);
		}while(key != KEY_CANCEL && key != KEY_ENTER);
	}
	else{
		key = ShowDocList(docs, 0);
	}
	

	_SetShutDonwTime(shutdownTime);

	#if Upgrd_LOG_ON
	LogPrint("[  query exit  ]\n");
	#endif
}

// 手动通知擦除App
static void UpgradeFunc_ManualNoticeUpgradeOnBoot(uint8 upgradeMode)
{
	AppFileInfo *app = &AppInfo;
	PacketInfo *pkt = &PktInfo;
	TList *docs = &DocList;
	DocInfo *docItem;
	uint16 docIdx, i, u16Tmp;
	uint8 *verNo = &ArgBuf[1000], *cmdName = &ArgBuf[1024], *cmdMsg = &ArgBuf[1060];
	uint8 cmdState = Cmd_Send;
	uint16 ackLen, timeout, dispIdx, index;
	uint8 tryCnt, lcdCtrl, key;
	CmdResult cmdResult = CmdResult_Ok;
	uint32 shutdownTime;
	uint8 *pData, *ptr, u8Tmp;

	_ClearScreen();

	// 防止自动抄表时关机，重置自动关机时间
	shutdownTime = _GetShutDonwTime();
	_SetShutDonwTime(0);		// 20 - 999 有效，0 - 关闭自动关机

	_Printfxy(0, 0, "<<手动通知擦除App", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	GetVersionNo(pkt->version, verNo);
	PrintfXyMultiLine_VaList(0, 2*16, "版本:%s  CRC:%4X", verNo, pkt->verCrc16);
	// to show meter no
	// to show cmd name
	// to show cmd msg
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "返回   <执行中>   ", Color_White);


	// 中继清空
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}

	CurrCmd = WaterCmd_NoticeUpgrade_OnBoot;
	docIdx = 0;
	lcdCtrl = 0;

	while(1){
	
		if(cmdState == Cmd_Send){

			// LCD背景灯控制
			LcdLightCycleCtrl(&lcdCtrl, 4);

			// 当前表号
			docItem = (DocInfo *)docs->itemAt(docs, docIdx);
			strcpy(StrDstAddr, docItem->mtrNo);
		
			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
		}

		switch(CurrCmd){

		case WaterCmd_NoticeUpgrade_OnBoot:		// 通知系统升级_在boot
			/*---------------------------------------------*/
			if(cmdState == Cmd_Finish){
				cmdState = Cmd_Exit;
				docIdx = 0;
				break;
			}
			if(cmdState == Cmd_RecvOk){
				dispIdx = 0;
				index = 0;
				ptr = Water6009_GetStrUpgradeForbidReason(pData[index], &u8Tmp);
				dispIdx += sprintf(&cmdMsg[dispIdx], "限制条件: %s\n", ptr);
				index += 1;
				GetVersionNo(&pData[index], docItem->ver);
				dispIdx += sprintf(&cmdMsg[dispIdx], "当前版本: %s\n", docItem->ver);
				if(u8Tmp == 0x00) {
					docItem->state = UpgrdState_NotStart;
				}
				else{
					switch(u8Tmp){
					case 0x01:	docItem->state = UpgrdState_Err_5_0v;	break;
					case 0x02:	docItem->state = UpgrdState_Err_3_4v;	break;
					case 0x04:	docItem->state = UpgrdState_Err_SNR;	break;
					case 0x08:	docItem->state = UpgrdState_Err_RSSI;	break;
					case 0x10:	docItem->state = UpgrdState_Err_VerNo;	break;
					case 0x20:	docItem->state = UpgrdState_Err_VerCrc;	break;
					default: break;
					}
				}

				IsInBoot = true;

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}
			if(cmdState == Cmd_RecvNg){
				if(CurrCmd == WaterCmd_NoticeUpgrade_OnApp){
					docItem->state = UpgrdState_Unknow;
				}

				if(docIdx == docs->cnt){
					cmdState = Cmd_Finish;
				}
				break;
			}

			docIdx++;
			tryCnt = 3;
			#ifdef Project_6009_IR
			timeout = 4000;
			#else
			timeout = 8000;
			//timeout = 1000;		// test
			#endif
			
			sprintf(cmdName, "通知开始升级-B");
			sprintf(cmdMsg, "      执行中...  ");
			Args.buf[i++] = 0x71;		// 命令字	71

			#if Upgrd_LOG_ON
			LogPrint("notice on boot , docidx: %d, doing...", docIdx -1);
			#endif
			
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

		default: 
			break;
		}

		if(cmdState == Cmd_Send){
			_GUIRectangleFill(0, 3*16, 160, 7*16, Color_White);
			PrintfXyMultiLine_VaList(0, 3*16, "表号：\r\n  > %s", StrDstAddr);
			PrintfXyMultiLine_VaList(0, 5*16, "--> %s", cmdName);
			_Printfxy(0, 9*16, "返回  <执行中>      ", Color_White);
		}
		else{
			_Printfxy(0, 6*16, cmdMsg, Color_White);
		}			

		if(cmdState == Cmd_RecvOk || cmdState == Cmd_RecvNg){
			_Sleep(500);
			cmdState = Cmd_Send;
			continue;
		}
		else if(cmdState == Cmd_Finish){
			continue;
		}
		else if(cmdState == Cmd_Exit){
			break;
		}
		else{
			// cmd send as follow
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * AddrLen;
		tryCnt = 3;

		// 发送、接收
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
			cmdState = Cmd_Exit;
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
		_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(200);
		_SoundOff();
		#endif
	}

	do{
		key = _ReadKey();
		_Sleep(100);
	}while(key != KEY_CANCEL && key != KEY_ENTER);

	_SetShutDonwTime(shutdownTime);
}

//------------------------		界面		-----------------
void UpgradeFunc(void)
{
	uint8 key, menuItemNo, u8Tmp;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * fileName = NULL;
	uint8 version[20], *verNo = &ArgBuf[1000];
	uint8 currUi = 0, uiRowIdx;
	uint8 upgradeMode = 1;	// 升级模式： 1 - 单表 ， 2 - 批量
	DocInfo *docItem;

	// test -->
	// ShowMsg(16, 5 *16, "暂不可用", 2000);  return;
	// end <--

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
	if(DocList.cnt == 0 || DocList.add == NULL){
		List_Init(&DocList);
	}
	if(upgradeMode == 1 && DocList.cnt > 0){	// 单表升级 表号初始化 
		docItem = (DocInfo *)DocList.head;
		memcpy(StrDstAddr, docItem->mtrNo, 20);
	}

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
		TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "  >", StrDstAddr, (AddrLen * 2), (AddrLen*2*8 + 8), true);
	}
	else{
		ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "2. 升级档案管理");	
	}
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "3. 开始升级");	
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "4. 查询升级状态");	
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16 + 8, "5. 通知boot下升级");				
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
				if(docItem == NULL || strcmp(docItem->mtrNo, StrDstAddr) != 0){
					DocList.clear(&DocList);
					docItem = (DocInfo *)DocList.add(&DocList, NULL, sizeof(DocInfo));
					memcpy(docItem->mtrNo, StrDstAddr, 20);
					docItem->state = UpgrdState_Unknow;

					#ifdef Project_6009_RF
					MeterNoSave(StrDstAddr, 0);
					#elif defined Project_6009_IR
					MeterNoSave(StrDstAddr, 1);
					#else // Project_8009_RF
					MeterNoSave(StrDstAddr, 2);
					#endif
				}
			}
		}

		if(menuItemNo > 2){
			if(fileName == NULL){
				ShowMsg(16, 4*16, "请先选择升级文件！", 1000);
				continue;
			}
			else if(DocList.cnt == 0){
				ShowMsg(16, 4*16, "升级表号不能为空！", 1000);
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
				u8Tmp = (uint8)InitPktInfo(&PktInfo, fileName, 128, 128, &AppInfo);
				if(u8Tmp != 0){
					if(u8Tmp == 2){
						ShowMsg(16, 4*16, "请选择正确的升级文件！", 1500);
					}
					else if(u8Tmp == 3){
						ShowMsg(16, 4*16, "程序体总CRC16错误！", 1500);
					}
					else if(u8Tmp == 4){
						ShowMsg(16, 4*16, "版本号CRC16错误！", 1500);
					}
					fileName = NULL;
					version[0] = 0x00;
				}
				else{
					GetVersionNo(PktInfo.version, verNo);
					sprintf(version, "     版本: %s   ", verNo);
					docItem = (DocInfo *)DocList.head;
					while(docItem != NULL){
						docItem->state = UpgrdState_Unknow;
						docItem->ver[0] = 0x00;
						docItem = docItem->next;
					}
					ShowMsg(16, 5 * 16, "升级信息已初始化！", 1000);
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

		case 5:	// 通知boot下升级
			{
				UpgradeFunc_ManualNoticeUpgradeOnBoot(upgradeMode);
			}
			break;

		default:
			break;
		}
	}

	#if ! Upgrd_FileBuf_Enable
		if(PktInfo.fileName != NULL && PktInfo.filePtr != 0){
			_Fclose(PktInfo.filePtr);
			PktInfo.filePtr = 0;
		}
	#endif
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
	uint32 fileLen;
	uint16 crc16, crc16_tmp = 0xFFFF;
	AppFileInfo *app;
	#if ! Upgrd_FileBuf_Enable
	uint32 readLen;
	uint8 *tmpBuf = &DispBuf[0];
	#endif
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
	#if Upgrd_FileBuf_Enable
		_Fread(&FileBuf[0], (fileLen - 128), fp);
		crc16 = GetCrc16(&FileBuf[256], (uint32)(fileLen - 128 - 256), 0x8408);
	#else
		_Fread(tmpBuf, 128*2, fp);
		while(0 == _Feof(fp)){
			readLen = _Fread(tmpBuf, 2048, fp);
			crc16 = GetCrc16_Continue(tmpBuf, readLen, 0x8408, &crc16_tmp);
		}
	#endif
	
	_Fclose(fp);

	app = (AppFileInfo *)header;
	crc16_tmp = GetCrc16(app->appVer, 40, 0x8408);

	pktInfo->fileCrc16 = crc16;
	pktInfo->verCrc16 = crc16_tmp;
	pktInfo->version = app->appVer;

	pktInfo->fileName = fileName;
	pktInfo->filePtr = 0;
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

	// test -->
	timeTick = _GetTickCount() - timeTick;
	_DoubleToStr(&tmpBuf[200], (double)timeTick / 32768, 3);
	PrintfXyMultiLine_VaList(0, 16 + 8, " Time Used: %s s\n read-appcrc: %4X \n calc-appcrc: %4X \n \
	read-vercrc: %4X \n calc-vercrc: %4X \n file size: %d k \n total pkt: %d \n", 
		&tmpBuf[200],
		(app->crc16_all52K[0] + app->crc16_all52K[1] * 256),  crc16, 
		(app->crc16_appVer[0] + app->crc16_appVer[1] * 256),  crc16_tmp,
		pktInfo->fileKbSize, pktInfo->packetCnt);
	_Sleep(2000);
	// -->end

	if(fileLen <= pktStartIdx 
		|| app->appVer[0] != 'S' || app->appVer[1] != 'R' || app->appVer[2] != 'W' || app->appVer[3] != 'F'){
		return 2;
	}

	if((app->crc16_all52K[0] + app->crc16_all52K[1] * 256) != crc16){
		return 3;
	}

	if((app->crc16_appVer[0] + app->crc16_appVer[1] * 256) != crc16_tmp){
		return 4;
	}

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
	int len = (pktInfo->packetCnt - 1 == pktIdx ? pktInfo->lastPktSize : pktInfo->packetSize);

	#if Upgrd_FileBuf_Enable
		memcpy(buf, &FileBuf[pktInfo->packetSize * pktIdx], len);
	#else
		_Lseek(pktInfo->filePtr, pktInfo->pktStartIdx + pktInfo->packetSize * pktIdx, 0);
		_Fread(buf, len, pktInfo->filePtr);
	#endif

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

	pktInfo->missPktsCnt = 0;	// set to unknown
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

	#if ! Upgrd_FileBuf_Enable
		if(pktInfo->filePtr == 0){
			pktInfo->filePtr = _Fopen(pktInfo->fileName, "R");
		}
	#endif

	if(pktInfo->missPktsCnt == pktInfo->packetCnt){
		for(i = 0; i < pktInfo->packetCnt; i++){
			pktInfo->missPkts[i] = i;
		}
		return;
	}

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

