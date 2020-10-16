#include "HJLIB.H"
#include "string.h"
#include "stdio.h"
#include "Common.h"

#include "MeterDocDBF.h"

#if defined Protocol_6009
#include "WaterMeter.h"
#elif defined Protocol_8009
#include "WaterMeter8009.h"
#endif

//-----------------------	全局变量定义
MeterInfoSt MeterInfo;
DistrictListSt Districts;
BuildingListSt Buildings;
MeterListSt Meters;
DbQuerySt DbQuery;
uint8 **MetersStrs = Meters.strs;

//----------------------	数据库信息-操作函数		-------------------------------------

/*
* 描 述：字符串列表中查找字符串
* 参 数：strs		- 字符串列表
*		strLen		- 每个字符串长度
*		strCnt		- 字符串数量
*		dstStr		- 查找的字符串
*		cmpMaxLen	- 比较的最大长度
* 返 回：int	- 找到的字符串在列表中的索引： -1 - 未找到， 0~n - 找到	
*/
int FindStrInList(char ** strs, uint8 strLen, uint16 strCnt, const char *dstStr, uint16 cmpMaxLen)
{
	int i = -1;
	char *str = (char *)strs;

	for(i = 0; i < strCnt; i++){

		if(strncmp(str, dstStr, cmpMaxLen) == 0){
			break;
		}
		str += strLen;
	}
	if(i >= strCnt){
		i = -1;
	}

	return i;
}

/*
* 描 述：查询小区列表
* 参 数：districts	- 小区列表
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryDistrictList(DistrictListSt *districts, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp1[Size_DbStr];
	char strTmp2[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	districts->cnt = 0;
	districts->idx = 0;
	query->reqMaxCnt = District_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){
		_ReadField(Idx_DistrictNum, strTmp1);		// 小区编号
		strTmp1[Size_ListStr - 1] = '\0';

		// 排除重复后，加入列表中
		if(-1 == FindStrInList(districts->nums, Size_ListStr, districts->cnt, strTmp1, Size_ListStr)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				sprintf(strTmp1, " 小区数 超出最大限制 %d !", query->reqMaxCnt);
				ShowMsg(8, 3*16, strTmp1, 3000);
				break;
			}

			_ReadField(Idx_DistrictName, strTmp2);	// 小区名称
			strTmp2[Size_ListStr - 1] = '\0';

			strcpy(districts->nums[districts->cnt], strTmp1);
			strcpy(districts->names[districts->cnt], strTmp2);
			districts->cnt++;
		}
		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：查询xx小区-楼栋列表
* 参 数：buildings	- 楼栋列表
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryBuildingList(BuildingListSt *buildings, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	char strTmp1[Size_DbStr];
	char strTmp2[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	buildings->cnt = 0;
	buildings->idx = 0;
	query->reqMaxCnt = Building_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){

		_ReadField(Idx_DistrictNum, strTmp);		// 小区编号 过滤
		strTmp[Size_ListStr - 1] = '\0';
		if(strcmp(buildings->qryDistricNum, strTmp) != 0){
			_Skip(1);	// 下一个数据库记录
			continue;
		}

		_ReadField(Idx_BuildingNum, strTmp1);		// 楼栋编号
		strTmp1[Size_ListStr - 1] = '\0';

		// 排除重复后，加入列表中
		if(-1 == FindStrInList(buildings->nums, Size_ListStr, buildings->cnt, strTmp1, Size_ListStr)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				sprintf(strTmp1, " 该小区楼栋数 超出最大限制 %d !", query->reqMaxCnt);
				ShowMsg(8, 3*16, strTmp1, 3000);
				break;
			}

			_ReadField(Idx_BuildingName, strTmp2);	// 楼栋名称
			strTmp2[Size_ListStr -1] = '\0';

			strcpy(buildings->nums[buildings->cnt], strTmp1);
			strcpy(buildings->names[buildings->cnt], strTmp2);
			buildings->cnt++;
		}
		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：查询xx小区-xx楼栋-抄表统计情况
* 参 数：meters		- 户表列表：查询前先设置 meters->(小区编号/楼栋编号/查询类型-抄表或阀门状态/显示字段)
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterList(MeterListSt *meters, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	uint8 state;
	int len;
	bool isOnlyCount = false;

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	meters->cnt = 0;
	meters->idx = 0;
	meters->districName[0] = 0x00;
	meters->buildingName[0] = 0x00;
	meters->meterCnt = 0;
	meters->readOkCnt = 0;
	meters->readNgCnt = 0;
	meters->valveCloseCnt = 0;
	meters->valveOpenCnt = 0;
	meters->valveUnknownCnt = 0;
	query->reqMaxCnt = Meter_Max;
	query->resultCnt = 0;
	query->errorCode = 0;

	switch (meters->selectField)		// 列表类型：默认为表号列表
	{
	case Idx_MeterNum:
	case Idx_UserNum:
	case Idx_UserRoomNum:
	case Idx_UserName:
	case Idx_UserAddr:
		break;
	default: 
		meters->selectField = Idx_Invalid;
		isOnlyCount = true;
		break;
	}

	for(i = 0; i < recCnt; i++){

		if(meters->qryDistricNum != NULL){
			_ReadField(Idx_DistrictNum, strTmp);		// 小区编号 过滤
			strTmp[Size_ListStr - 1] = '\0';
			if(strcmp(meters->qryDistricNum, strTmp) != 0){
				_Skip(1);	// 下一个数据库记录
				continue;
			}
			else{
				// 保存小区名
				if(meters->districName[0] == 0x00){
					_ReadField(Idx_DistrictName, strTmp);	
					strTmp[Size_DistrictName - 1] = '\0';
					strcpy(meters->districName, strTmp);
				}
			}
		}
		if(meters->qryBuildingNum != NULL){
			_ReadField(Idx_BuildingNum, strTmp);		// 楼栋编号 过滤
			strTmp[Size_ListStr - 1] = '\0';
			if(strcmp(meters->qryBuildingNum, strTmp) != 0){
				_Skip(1);	// 下一个数据库记录
				continue;
			}
			else{
				// 保存楼栋名
				if(meters->buildingName[0] == 0x00){
					_ReadField(Idx_BuildingName, strTmp);	
					strTmp[Size_BuildingName - 1] = '\0';
					strcpy(meters->buildingName, strTmp);
				}
			}
		}

		meters->meterCnt++;				// 当前表总数

		if(meters->qryType == QryBy_ReadStatus){
			_ReadField(Idx_MeterReadStatus, strTmp);		
			strTmp[Size_MeterReadStatus - 1] = '\0';
			
			if(strTmp[0] == '0'){
				state = 0;					// 未抄数量
			}
			else if(strTmp[0] == '1'){
				meters->readOkCnt++;		// 成功数量
				state = 1;
			}else{
				meters->readNgCnt++;		// 失败数量
				state = 2;			
			}

			if(isOnlyCount){
				_Skip(1);	// 只做统计, 下一个数据库记录
				continue;
			}
			
			// 抄表状态 过滤  0 - 未抄/失败， 1 - 已抄
			if((meters->qryMeterReadStatus == 1 && state != 1)
				|| (meters->qryMeterReadStatus == 0 && state == 1)){
				_Skip(1);	// 下一个数据库记录
				continue;
			}
		}
		else if(meters->qryType == QryBy_ValveStatus){
			_ReadField(Idx_ValveStatus, strTmp);		
			strTmp[Size_ValveStatus - 1] = '\0';
			
			if(strTmp[0] == '0'){
				meters->valveCloseCnt++;	// 关阀数量
				state = 0;				
			}
			else if(strTmp[0] == '1'){
				meters->valveOpenCnt++;		// 开阀数量
				state = 1;
			}else{
				meters->valveUnknownCnt++;	// 未知数量
				state = 2;			
			}

			if(isOnlyCount){
				_Skip(1);	// 只做统计, 下一个数据库记录
				continue;
			}
			
			// 阀门状态 过滤  0/1/2/3/4 关/开/未知/关或未知/开或未知
			if((meters->qryValveStatus < 3 && meters->qryValveStatus != state)
				|| (meters->qryValveStatus == 3 && state != 0 && state != 2 )
				|| (meters->qryValveStatus == 4 && state != 1 && state != 2 )
				|| (meters->qryValveStatus > 4)){
				_Skip(1);	// 下一个数据库记录
				continue;
			}
		}

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp, " 该楼表具数 超出最大限制 %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		_ReadField(meters->selectField, strTmp);	// 读取字段：表号/户号/门牌号/户名/地址

		// 将选择的字段信息 和 数据库索引 加入列表
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);
		StringPadRight(meters->strs[meters->cnt], 20, ' ');
		if(meters->qryType == QryBy_ReadStatus){
			meters->strs[meters->cnt][18] = ' ';	
			meters->strs[meters->cnt][19] = (state == 0 ? 'N' : (state == 1 ? 'Y' : 'F'));
		}
		meters->dbIdx[meters->cnt] = (i + 1);	// 数据库索引从 1 开始编号
		meters->cnt++;

		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：按关键字查询表具：按 表号/户号 的关键字查询
* 参 数：meters		- 户表列表：需设置 meters->selectField 为 Idx_MeterNum / Idx_UserNum
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterListByKeyword(MeterListSt *meters, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	char strTmp2[Size_DbStr];
	uint8 state;
	int len;

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	meters->cnt = 0;
	meters->idx = 0;
	query->reqMaxCnt = Meter_Max;
	query->resultCnt = 0;
	query->errorCode = 0;

	switch (meters->selectField)		// 列表类型：默认为表号列表
	{
	case Idx_MeterNum:
	case Idx_UserNum:
	case Idx_UserRoomNum:
		break;
	default: 
		meters->selectField = Idx_Invalid;
		break;
	}
	if(meters->selectField == Idx_Invalid || meters->qryKeyWord == NULL){	
		return;
	}
	

	for(i = 0; i < recCnt; i++){

		_ReadField(meters->selectField, strTmp);	// 读取字段：表号/户号/户名/地址
		if(strstr(strTmp, meters->qryKeyWord) == NULL){
			_Skip(1);	// 下一个数据库记录
			continue;
		}
		
		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp2, " 该抄表册表具数 超出最大限制 %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp2, 3000);
			break;
		}

		// 将选择的字段信息 和 数据库索引 加入列表
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);

		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';

		if(strTmp[0] == '0'){			// 未抄数量
			state = 0;				
		}
		else if(strTmp[0] == '1'){		// 成功数量
			state = 1;
		}else{							// 失败数量
			state = 2;			
		}

		StringPadRight(meters->strs[meters->cnt], 20, ' ');
		meters->strs[meters->cnt][18] = ' ';	
		meters->strs[meters->cnt][19] = (state == 0 ? 'N' : (state == 1 ? 'Y' : 'F'));
		meters->dbIdx[meters->cnt] = (i + 1);	// 数据库索引从 1 开始编号
		meters->cnt++;

		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/**
 * 设置路由
*/
void ShowSettingRoutes(void)
{
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, key;

	_ClearScreen();

	_Printfxy(0, 0, "<<设置路由", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);

	while(2){
		(*pUiCnt) = 0;
		uiRowIdx = 2;
		uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowUI(UiList, &currUi);

		if (key == KEY_CANCEL){
			break;
		}
	}
}

/*
* 描 述：显示 xx小区-xx楼栋- 自动抄表
* 参 数：meters		- 抄表情况列表
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowAutoMeterReading(MeterListSt *meters)
{
	uint8 key;
	uint16 ackLen, timeout, dispIdx, i, cnt;
	uint8 tryCnt, lcdCtrl, readStatus;
	CmdResult cmdResult = CmdResult_Ok;
	char *dispBuf = &DispBuf;
	MeterInfoSt *meterInfo = &MeterInfo;
	char strTmp[20];
	uint32 shutdownTime;

	if(meters->cnt == 0){
		ShowMsg(1*16, 3*16, "未抄失败列表为空!", 2000);
		return KEY_CANCEL;
	}

	// 中继检查
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}

	// 初始化
	cnt = 0;	
	meters->readOkCnt = 0;
	meters->readNgCnt = 0;
	lcdCtrl = 0;

	// 防止自动抄表时关机，重置自动关机时间
	shutdownTime = _GetShutDonwTime();
	_SetShutDonwTime(0);		// 20 - 999 有效，0 - 关闭自动关机

	// 自动抄表
	while(cnt < meters->cnt){

		#if LOG_ON
			sprintf(CurrCmdName, "批量抄表 %d/%d", (cnt + 1), meters->cnt); 
		#endif

		// LCD背景灯控制
		LcdLightCycleCtrl(&lcdCtrl, 4);
		
		// 读取当前户表信息
		meterInfo->dbIdx = meters->dbIdx[cnt];
		QueryMeterInfo(meterInfo, &DbQuery);

		// 自动抄表-界面显示
		_ClearScreen();
		//PrintfXyMultiLine_VaList(0, 0, "<<自动抄表  %3d/%-3d ", (cnt + 1), meters->cnt);
		_Printfxy(0, 0, "<<自动抄表", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "户号: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "地址: %s\n", meterInfo->userAddr);
		PrintfXyMultiLine(0, 1*16 + 8, dispBuf, 7);	

		sprintf(strTmp, "当前抄表: %d/%d", (cnt + 1), meters->cnt);
		_Printfxy(0, 6*16 + 8, strTmp, Color_White);
		_GUIHLine(0, 7*16 + 8 + 1, 160, Color_Black);
		ShowProgressBar(7*16 + 8 + 3, meters->cnt, cnt + 1);	// 进度条
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "  <  轮抄中...   >  ", Color_White);
		
		// 表号设置
		StringCopyFromTail(StrDstAddr, meterInfo->meterNum, AddrLen * 2);
		StringPadLeft(StrDstAddr, AddrLen * 2, '0');
		
		// 命令参数处理
		i = 0;	
		CurrCmd = WaterCmd_ReadRealTimeData;	// 读实时数据
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		#ifdef Protocol_6009
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 21;				// 应答长度 21	
			Args.buf[i++] = 0x00;		// 数据格式 00	
		#else // Protocol_8009
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 9;					// 应答长度 9	
		#endif
		
		Args.lastItemLen = i - 1;

		#ifdef Protocol_6009
			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			// 应答长度、超时时间、重发次数
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;
		#else // Protocol_8009
			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			// 应答长度、超时时间、重发次数
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 3;
		#endif

		// 发送、接收、结果显示
		cmdResult = CommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);

		if(cmdResult == CmdResult_Cancel){	// 取消轮抄
			break;
		}
		else if(cmdResult == CmdResult_Ok){
			// 成功，保存结果到数据库
			_Printfxy(0, 9*16, " < 当前抄表: 成功 > ", Color_White);
			meters->readOkCnt++;
			readStatus = 1;
		}
		else{
			// 失败，写入抄表时间
			_Printfxy(0, 9*16, " < 当前抄表: 失败 > ", Color_White);
			meters->readNgCnt++;
			readStatus = 2;
		}
		SaveMeterReadResult(meterInfo, 0, readStatus);		// 掌机抄表
		_Sleep(500);

		// 显示电量
		_Printfxy(0, 9*16, "                   ", Color_White);
		_DispTimeSys();	
		_Sleep(500);

		cnt++;
	}

	_OpenLcdBackLight();

	// 轮抄结束后 取消收发时回调函数
	//TranceiverCycleHook = NULL;

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
	

	sprintf(strTmp, "成功:%d", meters->readOkCnt);
	_Printfxy(6*16, 7*16 + 8 + 3, strTmp, Color_White);
	
	while(1){
		key = _ReadKey();
		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}
		_Sleep(100);
	}

	_SetShutDonwTime(shutdownTime);	
	
	return key;
}

/*
* 描 述：显示 xx小区-xx楼栋- 自动批量开关阀
* 参 数：meters		- 表具列表
*		valveCtrl - 开关阀控制: 0-关阀，1-开阀
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowAutoOpenCloseValve(MeterListSt *meters, uint8 valveCtrl)
{
	uint8 key;
	uint16 ackLen, timeout, dispIdx, i, cnt, okCnt;
	uint8 tryCnt, lcdCtrl;
	CmdResult cmdResult = CmdResult_Ok;
	char *dispBuf = &DispBuf;
	MeterInfoSt *meterInfo = &MeterInfo;
	char strTmp[20];
	uint32 shutdownTime;

	if(meters->cnt == 0){
		if(valveCtrl == 0){
			ShowMsg(1*16, 3*16, "批量关阀列表为空!", 2000);
		}
		else{
			ShowMsg(1*16, 3*16, "批量开阀列表为空!", 2000);
		}
		return KEY_CANCEL;
	}

	// 中继检查
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}

	// 初始化
	cnt = 0;	
	okCnt = 0;
	lcdCtrl = 0;

	// 防止自动抄表时关机，重置自动关机时间
	shutdownTime = _GetShutDonwTime();
	_SetShutDonwTime(0);		// 20 - 999 有效，0 - 关闭自动关机

	// 自动抄表
	while(cnt < meters->cnt){

		#if LOG_ON
			sprintf(CurrCmdName, "批量开关阀[%d] %d/%d", valveCtrl, (cnt + 1), meters->cnt); 
		#endif

		// LCD背景灯控制
		LcdLightCycleCtrl(&lcdCtrl, 4);
		
		// 读取当前户表信息
		meterInfo->dbIdx = meters->dbIdx[cnt];
		QueryMeterInfo(meterInfo, &DbQuery);

		// 自动抄表-界面显示
		_ClearScreen();
		if(valveCtrl == 0){
			_Printfxy(0, 0, "<<批量关阀", Color_White);
		}
		else {
			_Printfxy(0, 0, "<<批量开阀", Color_White);
		}
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "户号: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "地址: %s\n", meterInfo->userAddr);
		PrintfXyMultiLine(0, 1*16 + 8, dispBuf, 7);	

		sprintf(strTmp, "当前阀控: %d/%d", (cnt + 1), meters->cnt);
		_Printfxy(0, 6*16 + 8, strTmp, Color_White);
		_GUIHLine(0, 7*16 + 8 + 1, 160, Color_Black);
		ShowProgressBar(7*16 + 8 + 3, meters->cnt, cnt + 1);	// 进度条
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "  < 自动操作中... >  ", Color_White);
		
		// 表号设置
		StringCopyFromTail(StrDstAddr, meterInfo->meterNum, AddrLen * 2);
		StringPadLeft(StrDstAddr, AddrLen * 2, '0');
		
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域

		switch (valveCtrl){
		case 0:
			CurrCmd = WaterCmd_CloseValve;		// 关阀 
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
			#else // Protocol_8009
				Args.buf[i++] = 0x06;		// 命令字	06
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;
		case 1:
			CurrCmd = WaterCmd_OpenValve;		// 开阀 
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
			#else // Protocol_8009
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		default:
			break;
		}
		
		Args.lastItemLen = i - 1;

		// 地址填充
		#ifdef Protocol_6009
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#else
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#endif

		// 应答长度、超时时间、重发次数
		#ifdef Protocol_6009
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;
		#else // Protocol_8009
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 3;
		#endif

		// 发送、接收、结果显示
		cmdResult = CommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);

		if(cmdResult == CmdResult_Cancel){	// 取消轮抄
			break;
		}
		else if(cmdResult == CmdResult_Ok){
			_Printfxy(0, 9*16, " < 当前阀控: 成功 > ", Color_White);
			okCnt++;
			SaveValveStatus(meterInfo, valveCtrl == 0 ? 0 : 1);
		}
		else{
			_Printfxy(0, 9*16, " < 当前阀控: 失败 > ", Color_White);
			
			if(cmdResult == CmdResult_CrcError || cmdResult == CmdResult_Timeout){
				SaveValveStatus(meterInfo, 2);
			}
		}
		_Sleep(500);

		// 显示电量
		_Printfxy(0, 9*16, "                   ", Color_White);
		_DispTimeSys();	
		_Sleep(500);

		cnt++;
	}

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
		_Printfxy(0, 9*16, "返回  <已完成>  确定", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(200);
		_SoundOff();
		#endif
	}
	
	sprintf(strTmp, "成功:%d", okCnt);
	_Printfxy(6*16, 7*16 + 8 + 3, strTmp, Color_White);
	
	while(1){
		key = _ReadKey();
		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}
		_Sleep(100);
	}

	_SetShutDonwTime(shutdownTime);	
	
	return key;
}

/*
* 描 述：显示xx小区-xx楼栋-抄表统计情况
* 参 数：meters		- 抄表情况列表
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterReadCountInfo(MeterListSt *meters)
{
	uint8 key;
	uint16 dispIdx = 0;
	char *dispBuf = &DispBuf;

	_ClearScreen();

	meters->readNotCnt = meters->meterCnt - meters->readOkCnt - meters->readNgCnt;

	_Printfxy(0, 0, "<<抄表统计", Color_White);
	/*---------------------------------------------*/
	dispIdx += sprintf(&dispBuf[dispIdx], "小区: %s\n", meters->districName);
	dispIdx += sprintf(&dispBuf[dispIdx], "楼栋: %s\n", meters->buildingName);
	dispIdx += sprintf(&dispBuf[dispIdx], "表具总数: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "已抄成功: %d\n", meters->readOkCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "当前失败: %d\n", meters->readNgCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "当前未抄: %d\n", meters->readNotCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* 描 述：显示xx小区-xx楼栋-阀门状态统计情况
* 参 数：meters		- 表具列表
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowValveStatusCountInfo(MeterListSt *meters)
{
	uint8 key;
	uint16 dispIdx = 0;
	char *dispBuf = &DispBuf;

	_ClearScreen();

	meters->valveUnknownCnt = meters->meterCnt - meters->valveOpenCnt - meters->valveCloseCnt;

	_Printfxy(0, 0, "<<抄表统计", Color_White);
	/*---------------------------------------------*/
	dispIdx += sprintf(&dispBuf[dispIdx], "小区: %s\n", meters->districName);
	dispIdx += sprintf(&dispBuf[dispIdx], "楼栋: %s\n", meters->buildingName);
	dispIdx += sprintf(&dispBuf[dispIdx], "表具总数: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "已关阀: %d\n", meters->valveCloseCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "已开阀: %d\n", meters->valveOpenCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "阀门未知: %d\n", meters->valveUnknownCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* 描 述：显示xx小区-xx楼栋-已抄/未抄列表 或 关阀/开阀/阀门未知列表
* 参 数：meters		- 抄表情况列表: 调用前先设置查询类型 和 对应的状态
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterList(MeterListSt *meterReadList)
{
	uint8 key;
	ListBox menuList;			// 显示方式菜单
	ListBoxEx meterList;		// 表信息-列表
	char *title = NULL;
	MeterListSt *meters = meterReadList;
	char state;

	if(meters->selectField != Idx_Invalid){
		title = "<<户表查询结果";
	}
	else{
		// 列表显示方式-界面
		ListBoxCreate(&menuList, 16*4, 16*3, 12, 4, 5, NULL,
			"显示类型", 
			5,
			"1. 表号",
			"2. 户号",
			"3. 户名",
			"4. 门牌号",
			"5. 地址");

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if(key == KEY_CANCEL){	// 返回
			return key;
		}

		switch (menuList.strIdx + 1){
		case 1:	meters->selectField = Idx_MeterNum; break;
		case 2:	meters->selectField = Idx_UserNum; break;
		case 3:	meters->selectField = Idx_UserName;	break;
		case 4:	meters->selectField = Idx_UserRoomNum; break;
		case 5:	meters->selectField = Idx_UserAddr; break;
		default: Meters.selectField = Idx_Invalid; break;
		}

		//    已抄/未抄列表-界面 
		// 或 关阀/开阀/阀门未知列表
		//------------------------------------------------------------
		_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
		QueryMeterList(meters, &DbQuery);	// 已抄/未抄列表 查询

		if(meters->qryType == QryBy_ReadStatus){
			title = (meters->qryMeterReadStatus == 1 ? "<<已抄成功列表" : "<<未抄失败列表");
		}
		else{ // meters->qryType == QryBy_ValveStatus
			title = (meters->qryValveStatus == 0 ? "<<已关阀列表" : \
				(meters->qryValveStatus == 1 ? "<<已开阀列表" : "<<阀门未知列表"));
		}
	}

	ListBoxCreateEx(&meterList, 0, 0, 20, 7, meters->cnt, NULL,
			&title[2], meters->strs, Size_ListStr, meters->cnt);
	while(1){

		_Printfxy(0, 9*16, "返回        户表信息", Color_White);
		key = ShowListBoxEx(&meterList);
		//------------------------------------------------------------
		if(key == KEY_CANCEL){	// 返回
			break;
		}
		if(key == KEY_ENTER && meters->cnt == 0){	
			continue;
		}

		while(2){
			// 户表信息-界面
			//------------------------------------------------------
			meters->idx = meterList.strIdx;
			MeterInfo.dbIdx = meters->dbIdx[meters->idx];
			MeterInfo.strIdx = meters->idx;
			MeterInfo.strCnt = meters->cnt;
			QueryMeterInfo(&MeterInfo, &DbQuery);	// 户表信息查询
			key = ShowMeterInfo(&MeterInfo);
			state = MeterInfo.meterReadStatus[0];	
			if(meters->qryType == QryBy_ReadStatus){
				meters->strs[meters->cnt][18] = ' ';	
				meters->strs[meters->cnt][19] = (state == '0' ? 'N' : (state == '1' ? 'Y' : 'F'));
			}
			//------------------------------------------------------
			if(key == KEY_LEFT){
				if(meterList.strIdx == 0){
					meterList.strIdx = meterList.strCnt - 1;
				}
				else{
					meterList.strIdx--;
				}
			}
			else if(key == KEY_RIGHT){
				if(meterList.strIdx == meterList.strCnt - 1){
					meterList.strIdx = 0;
				}
				else{
					meterList.strIdx++;
				}
			}
			else{	// KEY_CANCEL
				meterList.currIdx = meterList.strIdx;
				break;
			}
		} // while 2 户表信息

	}// while 1 已抄/未抄列表
	
	return key;
}

/*
* 描 述：保存抄表结果
* 参 数：meterInfo	- 户表信息
* 		readType - 抄表方式：0-掌机抄表，1-手工录入
* 		readStatus - 抄表状态：0-未抄，1-成功，2-失败
* 返 回：void
*/
void SaveMeterReadResult(MeterInfoSt *meterInfo, uint8 readType, uint8 readStatus)
{
	char time[20];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	_Go(meterInfo->dbIdx);

	_GetDateTime(time, '-', ':');
	sprintf(meterInfo->meterReadType, "%d", readType);
	sprintf(meterInfo->meterReadStatus, "%d", readStatus);
	sprintf(meterInfo->meterReadTime, "%s", time);
	if(readType == 1){
		sprintf(meterInfo->meterStatusHex, "000000");
		sprintf(meterInfo->meterStatusStr, "手工录入");
		sprintf(meterInfo->batteryVoltage, "3.3");
		sprintf(meterInfo->signalValue, "99");
	}

	// 更新抄表结果
	_Replace(Idx_MeterReadStatus, meterInfo->meterReadStatus);	// 抄表状态 0 - 未抄， 1 - 成功， 2 - 失败
	_Replace(Idx_MeterReadType, meterInfo->meterReadType);		// 抄表方式 0 - 掌机抄表 ， 1 - 手工录入
	_Replace(Idx_MeterReadTime, meterInfo->meterReadTime);		// 抄表时间
	if(readStatus == 1){
		_Replace(Idx_MeterValue, meterInfo->meterValue);			// 读数
		_Replace(Idx_MeterStatusHex, meterInfo->meterStatusHex);	// 表状态 hex
		_Replace(Idx_MeterStatusStr, meterInfo->meterStatusStr);	// 表状态 str
		_Replace(Idx_BatteryVoltage, meterInfo->batteryVoltage);	// 电池电压
		_Replace(Idx_SignalValue, meterInfo->signalValue);			// 信号强度
		_Replace(Idx_ValveStatus, meterInfo->valveStatus);				// 自定义阀门状态
	}

	_Use("");			// 关闭数据库
}

/*
* 描 述：保存阀门状态
* 参 数：meterInfo	- 户表信息
* 		status - 阀门状态：0-关阀，1-开阀，2-未知
* 返 回：void
*/
void SaveValveStatus(MeterInfoSt *meterInfo, uint8 status)
{
	uint8 tmp;

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	_Go(meterInfo->dbIdx);
	
	sprintf(meterInfo->valveStatus, "%d", status);

	_Replace(Idx_ValveStatus, meterInfo->valveStatus);				// 自定义阀门状态

#if defined Project_6009_RF_HL || defined Project_8009_RF_HL
	// do nothing
#else

	#if defined Protocol_6009
		tmp = (status == 0 ? 2 : (status == 1 ? 1 : 0));			// 阀门状态转换 0/1/2 --> 2/1/0 	关/开/未知
	#elif defined Protocol_8009
		tmp = (status == 0 ? 0x20 : (status == 1 ? 0x40 : 0x00));	// 阀门状态转换 0/1/2 --> 0x20/0x40/0x00 	关/开/未知
	#endif
	sprintf(&meterInfo->meterStatusHex[4], "%02X", tmp);

#endif

	_Use("");			// 关闭数据库
}

/*
* 描 述：查询户表信息
* 参 数：meterInfo	- 户表信息
*		 query		- 数据库查询结构
* 返 回：void
*/
#define UseSearchFunc 1
void QueryMeterInfo(MeterInfoSt *meterInfo, DbQuerySt *query)
{
	#if ! UseSearchFunc
	uint32 i;
	#endif
	uint32 recCnt;
	char strTmp[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	query->resultCnt = 0;
	query->errorCode = 0;

	if(meterInfo->dbIdx != Invalid_dbIdx){	// 数据库记录索引 是否有效？  【有效值为 1 ~ 最大记录数】
		if(meterInfo->dbIdx > recCnt){		
			meterInfo->dbIdx = Invalid_dbIdx;
		}
	}

	if(meterInfo->dbIdx == Invalid_dbIdx){	// 数据库记录索引无效时 执行查询

		#if UseSearchFunc

		if(meterInfo->qryMeterNum != NULL
			&& _LocateEx(Idx_MeterNum, '=', meterInfo->qryMeterNum, 1, recCnt, 0) > 0){ 
			// 按表号查询
			meterInfo->dbIdx = _Recno();
		}
		else if(meterInfo->qryUserNum != NULL
			&& _LocateEx(Idx_UserNum, '=', meterInfo->qryUserNum, 1, recCnt, 0) > 0){ 
			// 按户号查询
			meterInfo->dbIdx = _Recno();
		}
		else if(meterInfo->qryRoomNum != NULL
			&& _LocateEx(Idx_UserRoomNum, '=', meterInfo->qryRoomNum, 1, recCnt, 0) > 0){ 
			// 按门牌号查询
			meterInfo->dbIdx = _Recno();
		}

		#else

		for(i = 0; i < recCnt; i++){

			if(meterInfo->qryMeterNum != NULL){
				_ReadField(Idx_MeterNum, strTmp);		// 按表号查询
				strTmp[Size_MeterNum - 1] = '\0';
				if(strcmp(meterInfo->qryMeterNum, strTmp) == 0){
					meterInfo->dbIdx = i + 1;
					break;
				}
			}
			else if(meterInfo->qryUserNum != NULL){
				_ReadField(Idx_UserNum, strTmp);		// 按户号查询
				strTmp[Size_UserNum - 1] = '\0';
				if(strcmp(meterInfo->qryUserNum, strTmp) == 0){
					meterInfo->dbIdx = i + 1;
					break;
				}
			}
			else if(meterInfo->qryRoomNum != NULL){
				_ReadField(Idx_UserRoomNum, strTmp);	// 按门牌号查询
				strTmp[Size_RoomNum - 1] = '\0';
				if(strcmp(meterInfo->qryRoomNum, strTmp) == 0){
					meterInfo->dbIdx = i + 1;
					break;
				}
			}

			_Skip(1);	// 下一个数据库记录
		}
		#endif
	}

	if(meterInfo->dbIdx != Invalid_dbIdx){	// 数据库记录索引有效时 读取记录

		_Go(meterInfo->dbIdx);
		query->resultCnt = 1;

		_ReadField(Idx_MeterNum, strTmp);					// 表号
		strTmp[Size_MeterNum - 1] = '\0';
		strcpy(meterInfo->meterNum, strTmp);

		_ReadField(Idx_UserNum, strTmp);					// 户号
		strTmp[Size_UserNum - 1] = '\0';
		strcpy(meterInfo->userNum, strTmp);	

		_ReadField(Idx_UserRoomNum, strTmp);				// 门牌号
		strTmp[Size_RoomNum - 1] = '\0';
		StringFixGbkStrEndError(strTmp);
		strcpy(meterInfo->roomNum, strTmp);	

		_ReadField(Idx_MeterReadStatus, strTmp);			// 抄表状态
		strTmp[Size_MeterReadStatus - 1] = '\0';
		strcpy(meterInfo->meterReadStatus, strTmp);	

		_ReadField(Idx_UserName, strTmp);					// 户名
		strTmp[Size_UserName - 1] = '\0';
		strcpy(meterInfo->userName, strTmp);	

		_ReadField(Idx_UserMobileNum, strTmp);				// 手机号
		strTmp[Size_UserMobileNum - 1] = '\0';
		strcpy(meterInfo->mobileNum, strTmp);	

		_ReadField(Idx_UserAddr, strTmp);					// 地址
		strTmp[Size_UserAddr - 1] = '\0';
		StringFixGbkStrEndError(strTmp);
		strcpy(meterInfo->userAddr, strTmp);	

		_ReadField(Idx_MeterReadType, strTmp);			// 抄表方式
		strTmp[Size_MeterReadType - 1] = '\0';
		strcpy(meterInfo->meterReadType, strTmp);	

		_ReadField(Idx_MeterReadTime, strTmp);			// 抄表时间
		strTmp[Size_MeterReadTime - 1] = '\0';
		strcpy(meterInfo->meterReadTime, strTmp);	

		_ReadField(Idx_MeterValue, strTmp);				// 表读数
		strTmp[Size_MeterValue - 1] = '\0';
		strcpy(meterInfo->meterValue, strTmp);	

		_ReadField(Idx_MeterStatusStr, strTmp);			// 表状态
		strTmp[Size_MeterStatusStr - 1] = '\0';
		strcpy(meterInfo->meterStatusStr, strTmp);	

		_ReadField(Idx_BatteryVoltage, strTmp);			// 电池电压
		strTmp[Size_BatteryVoltage - 1] = '\0';
		strcpy(meterInfo->batteryVoltage, strTmp);	

		_ReadField(Idx_SignalValue, strTmp);			// 信号强度
		strTmp[Size_SignalValue - 1] = '\0';
		strcpy(meterInfo->signalValue, strTmp);	
	}

	_Use("");		// 关闭数据库

	query->dbCurrIdx = meterInfo->dbIdx;
}

/*
* 描 述：显示户表信息
* 参 数：meterInfo	- 户表信息
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterInfo(MeterInfoSt *meterInfo)
{
	uint8 key, i;
	uint16 dispIdx = 0;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, u8Tmp;
	uint8 tryCnt, readStatus;
	CmdResult cmdResult = CmdResult_Ok;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;
	char *dispBuf;

	while(1){
		_ClearScreen();

		readStatus = meterInfo->meterReadStatus[0] - '0';

		// 户表信息-界面
		//-----------------------------------------------------
		PrintfXyMultiLine_VaList(0, 0, "<<户表信息  %3d/%-3d ", meterInfo->strIdx + 1, meterInfo->strCnt);
		dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "户号: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "门牌号: %s\n", meterInfo->roomNum);
		if(meterInfo->meterReadType[0] == '1'){
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表状态: 成功(录)\n");
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表状态: %s\n", (readStatus == 0 ? "未抄" : (readStatus == 1 ? "成功" : "失败")));
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "户名: %s\n", meterInfo->userName);
		dispIdx += sprintf(&dispBuf[dispIdx], "手机: %s\n", meterInfo->mobileNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "地址: %s\n", meterInfo->userAddr);
		u8Tmp = meterInfo->meterReadType[0] - '0';
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表方式: %s\n", (u8Tmp == 0 ? "掌机抄表" : (u8Tmp == 1 ? "手工录入" : " ")));
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表时间: \n %s\n", meterInfo->meterReadTime);
		dispIdx += sprintf(&dispBuf[dispIdx], "表读数: %s\n", meterInfo->meterValue);
		dispIdx += sprintf(&dispBuf[dispIdx], "表状态: %s\n", meterInfo->meterStatusStr);
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压: %s\n", meterInfo->batteryVoltage);
		dispIdx += sprintf(&dispBuf[dispIdx], "信号强度: %s", meterInfo->signalValue);
		//----------------------------------------------
		_Printfxy(0, 9*16, "返回        户表命令", Color_White);
		key = ShowScrollStrEx(dispBuf,  7);
		//----------------------------------------------
		if(key == KEY_CANCEL || key == KEY_LEFT || key == KEY_RIGHT){	// 返回
			break;
		}

		// 表号设置
		StringCopyFromTail(StrDstAddr, meterInfo->meterNum, AddrLen * 2);
		StringPadLeft(StrDstAddr, AddrLen * 2, '0');

		for(i = 0; i < RELAY_MAX; i++){				// 中继清空
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
			}
		}

		// 户表命令-界面
		ListBoxCreate(&menuList, 3*16, 2*16, 14, 5, 5, NULL, 
			"户表命令", 
			5, 
			"1. 抄表",
			"2. 开阀",
			"3. 关阀",
			"4. 清异常",
			"5. 手工录入");
		//---------------------
		key = ShowListBox(&menuList);
		if(key == KEY_CANCEL){	// 取消执行命令，重新显示户表信息
			continue;
		}

		// 户表命令-标题
		_ClearScreen();
		sprintf(CurrCmdName, menuList.str[menuList.strIdx]);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		//--------------------------------------
		sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
		_Printfxy(0, 0, TmpBuf, Color_White);
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);	
		//------------------------------------

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域

		switch(menuList.strIdx + 1){

		case 1:
			CurrCmd = WaterCmd_ReadRealTimeData;		// "读取用户用量"
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x01;		// 命令字	01
				ackLen = 21;				// 应答长度 21	
				Args.buf[i++] = 0x00;		// 数据格式 00	
			#else // Protocol_8009
				Args.buf[i++] = 0x01;		// 命令字	01
				ackLen = 9;					// 应答长度 9	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 2:
			CurrCmd = WaterCmd_OpenValve;			// " 开阀 "
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
			#else // Protocol_8009
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 3:
			CurrCmd = WaterCmd_CloseValve;		// " 关阀 ";
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
			#else // Protocol_8009
				Args.buf[i++] = 0x06;		// 命令字	06
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 4:
			CurrCmd = WaterCmd_ClearException;		// " 清异常命令 ";
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 1;					// 应答长度 1	
				Args.buf[i++] = 0x00;		// 命令选项 00	
			#else // Protocol_8009
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 5:										// 手工录入
			while(2){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				currUi = 0;
				sprintf(StrBuf[1], "表号: %s", StrDstAddr);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, StrBuf[1]);
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "录入读数:", StrBuf[0], 9, 11*8, true);
						pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " ");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表状态: 手工录入");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "电池电压: 3.3 v");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "信号强度: 99");
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(false == StringToDecimal(StrBuf[0], 2, &u8Tmp, &u32Tmp, &u16Tmp)){
					sprintf(StrBuf[0], " ");
					continue;
				}

				readStatus = 1;
				sprintf(MeterInfo.meterValue, "%d.%02d", u32Tmp, (u16Tmp & 0xFF));
				SaveMeterReadResult(meterInfo, 1, readStatus);	// 手工录入
				ShowMsg(16, 3*16, "表读数 录入成功!", 1000);
				break;
			}
			continue;		// 显示户表信息

		default: 
			break;
		}

		// 地址填充
		#ifdef Protocol_6009
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#else
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#endif

		// 应答长度、超时时间、重发次数
		#ifdef Protocol_6009
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 2;
		#else // Protocol_8009
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 2;
		#endif

		// 发送、接收、结果显示
		cmdResult = ProtolCommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		if(cmdResult == CmdResult_Cancel){	// 命令取消
			readStatus = 0;
		}
		else if(cmdResult == CmdResult_Ok){
			// 成功，保存结果到数据库
			readStatus = 1;
		}
		else{
			// 失败，写入抄表时间
			readStatus = 2;
		}

		if(CurrCmd == WaterCmd_ReadRealTimeData && readStatus != 0){
			SaveMeterReadResult(meterInfo, 0, readStatus);	// 掌机抄表
		}
		
		if(cmdResult == CmdResult_Ok && CurrCmd == WaterCmd_CloseValve){
			SaveValveStatus(meterInfo, 0);
		}
		else if(cmdResult == CmdResult_Ok && CurrCmd == WaterCmd_OpenValve){
			SaveValveStatus(meterInfo, 1);
		}
		else if((cmdResult == CmdResult_CrcError || cmdResult == CmdResult_Timeout)
			 && (CurrCmd == WaterCmd_CloseValve || CurrCmd == WaterCmd_OpenValve)
		){
			SaveValveStatus(meterInfo, 2);
		}

		//------------------------------------------------------
		key = ShowScrollStr(&DispBuf, 7);

	}// while 1  户表信息
	

	return key;
}

/*
* 描 述：修复DBF文件的头部记录总数信息
* 参 数：void
* 返 回：uint32 记录总数：0 - 记录总数为0 或 DBF文件不存在/打开失败
*/
uint32 FixDbfRecCnt(void)
{
	uint32 recCnt = 0;
	int fp;

	if(_Access(MeterDocDB, 0) < 0){
		return recCnt;
	}
	
	_Select(1);
	if(_Use(MeterDocDB) == 0){	// 打开数据库
		return recCnt;
	}
	recCnt = _Reccount();
	_Use("");					// 关闭数据库

	_Lseek(fp, 4, 0);
	_Fwrite(&recCnt, 4, fp);
	_Fclose(fp);

	return recCnt;
}

// 批量抄表 - 6009/8009通用版
#if defined Project_6009_RF || defined Project_6009_RF_TN || defined Project_8009_RF || defined Project_8009_RF_TN
void MainFuncBatchMeterReading(void)
{
	uint8 key;
	ListBox menuList, menuList_2, menuList_3;
	ListBoxEx XqList, LdList;				// 小区/楼栋列表
	_GuiInputBoxStru inputSt;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint8 *ptr;
	uint16 dispIdx, i;
	char *dispBuf = &DispBuf, *strTmp = &TmpBuf[0], *time = &TmpBuf[200];
	char *qryStrXq = &TmpBuf[400], *qryStrLd = &TmpBuf[600];
	uint8 qryTypeXq, qryTypeLd;
	uint16 qryIndexXq, qryIndexLd;
	uint32 recCnt;

	recCnt = FixDbfRecCnt();	// 修复记录总数
	MeterInfo.dbIdx = Invalid_dbIdx;  // 清空当前表数据库索引，防止抄表结果写入
	
	if(recCnt == 0){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  当前档案为空！\n 请先下载抄表档案，\n 再进行批量操作");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// 菜单
	//------------------------------------------------------------
	#ifdef UseFunc_ReSetDistrictAndBuildingNo
		ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
			"<<批量抄表",
			6,
			"1. 按楼栋抄表",
			"2. 清空抄表结果",
			"3. 重置抄表时间",
			"4. 户表查询",
			"5. 抄表统计",
			"6. 小区楼栋重新编号"
		);
	#else
		ListBoxCreate(&menuList, 0, 0, 20, 7, 5, NULL,
			"<<批量抄表",
			5,
			"1. 按楼栋抄表",
			"2. 清空抄表结果",
			"3. 重置抄表时间",
			"4. 户表查询",
			"5. 抄表统计"
		);
	#endif
	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		switch(menuList.strIdx + 1){
		case 1:		// 按楼栋抄表
			#ifdef UseFunc_ReSetDistrictAndBuildingNo
			if(!IsReSetNo()){
				_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
				PrintfXyMultiLine_VaList(0, 3*16, "  小区楼栋未编号！\n  请先重新编号！");
				_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
				_Sleep(2500);
				break;
			}
			#endif

			// 小区列表-界面
			//------------------------------------------------------------
			_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
			QueryDistrictList(&Districts, &DbQuery);
			ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
				"<<小区选择", Districts.names, Size_ListStr, Districts.cnt);
			while(2){
				
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBoxEx(&XqList);
				//------------------------------------------------------------
				if (key == KEY_CANCEL){	// 返回
					break;
				}

				// xx小区-楼栋列表-界面
				//------------------------------------------------------------
				_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
				Buildings.qryDistricNum = Districts.nums[XqList.strIdx];
				QueryBuildingList(&Buildings, &DbQuery);
				ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
					"<<楼栋选择", Buildings.names, Size_ListStr, Buildings.cnt);
				//------------------------------------------------------------
				while(3){
	
					_Printfxy(0, 9*16, "返回            确定", Color_White);
					key = ShowListBoxEx(&LdList);
					//------------------------------------------------------------
					if(key == KEY_CANCEL){	// 返回
						break;
					}

					// 清空路由
					StrRelayAddr[0][0] = 0x00;
					StrRelayAddr[1][0] = 0x00;
					StrRelayAddr[2][0] = 0x00;

					// 楼栋抄表-界面
					//------------------------------------------------------------
					Meters.qryDistricNum = Districts.nums[XqList.strIdx];
					Meters.qryBuildingNum = Buildings.nums[LdList.strIdx];
				#ifdef Protocol_6009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 6, NULL,
						"<<楼栋抄表", 
						6,
						"1. 自动抄表",
						"2. 已抄成功列表",
						"3. 未抄失败列表",
						"4. 抄表统计",
						"5. 清空抄表结果",
						"6. 重置抄表时间");
				#else // Protocol_8009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 7, NULL,
						"<<楼栋抄表", 
						7,
						"1. 自动抄表",
						"2. 已抄成功列表",
						"3. 未抄失败列表",
						"4. 抄表统计",
						"5. 清空抄表结果",
						"6. 重置抄表时间",
						"7. 设置路由");
				#endif
					while(4){

						_Printfxy(0, 9*16, "返回            确定", Color_White);
						key = ShowListBox(&menuList_2);
						//------------------------------------------------------------
						if(key == KEY_CANCEL){	// 返回
							break;
						}

						Meters.qryType = QryBy_ReadStatus;		// 查询类型：抄表状态

						switch (menuList_2.strIdx + 1){
						case 1:		// 自动抄表
							Meters.selectField = Idx_MeterNum;
							Meters.qryMeterReadStatus = 0;
							_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowAutoMeterReading(&Meters);
							break;

						case 2:		// 已抄列表
							Meters.selectField = Idx_Invalid;
							Meters.qryMeterReadStatus = 1;
							key = ShowMeterList(&Meters);
							break;
						case 3:		// 未抄/失败列表
							Meters.selectField = Idx_Invalid;
							Meters.qryMeterReadStatus = 0;
							key = ShowMeterList(&Meters);
							break;

						case 4:		// 抄表统计
							Meters.selectField = Idx_Invalid;
							_Printfxy(0, 9*16, "    <  统计中  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowMeterReadCountInfo(&Meters);
							break;

						case 5:		// 清空抄表结果
							//-------------------------------------------------------
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " 确定要清空        ", Color_White);
							_Printfxy(0, 5*16, " 当前楼栋抄表结果吗?", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							key = _ReadKey();
							//-------------------------------------------------------
							if(key != KEY_ENTER){
								break;
							}
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, "  当前楼栋         ", Color_White);
							_Printfxy(0, 5*16, "  抄表结果清空中... ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							//------------------------------------------------------------
							_Select(1);
							_Use(MeterDocDB);	// 打开数据库
							_Go(0);
							for(i = 0; i < recCnt; i++){
								_ReadField(Idx_DistrictNum, strTmp);	// 小区编号 过滤
								strTmp[Size_DistrictNum - 1] = '\0';
								if(strcmp(Meters.qryDistricNum, strTmp) != 0){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_ReadField(Idx_BuildingNum, strTmp);	// 楼栋编号 过滤
								strTmp[Size_BuildingNum - 1] = '\0';
								if(strcmp(Meters.qryBuildingNum, strTmp) != 0){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
								strTmp[Size_MeterReadStatus - 1] = '\0';
								if(strTmp[0] == '0'){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_Replace(Idx_MeterReadStatus, "0");	
								_Replace(Idx_MeterReadTime, "");
								_Replace(Idx_MeterReadType, "");
								_Replace(Idx_MeterValue, "");
								_Replace(Idx_MeterStatusHex, "");
								_Replace(Idx_MeterStatusStr, "");
								_Replace(Idx_BatteryVoltage, "");
								_Replace(Idx_SignalValue, "");
								_Skip(1);
							}
							_Use("");		// 关闭数据库
							//------------------------------------------------------------
							_Printfxy(0, 5*16, "  抄表结果清空完成! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							break;

						case 6:		// 重置抄表时间
							//-------------------------------------------------------
							_GUIRectangleFill(0, 3*16 - 8, 160, 7*16 + 8, Color_White);
							_GUIRectangle(0, 3*16 - 8, 160, 7*16 + 8, Color_Black);
							memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
							isUiFinish = false;
							while(true){
								if(false == isUiFinish){
									(*pUiCnt) = 0;
									uiRowIdx = 3;
									_GetDateTime(time, '-',  ':');
									DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
									
									LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "系统时间:");
									TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
									TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
									TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
									TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
									TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
									TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
									LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "确定将抄表时间重置");
									LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "为当前系统时间吗?");
									
									key = ShowUI(UiList, &currUi);
									if (key == KEY_CANCEL){
										break;
									}
									isUiFinish = true;
								}

								// 时间有效值校验
								if( (i = TimeStrsToTimeBytes(&TmpBuf[0], StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
									currUi = 1 + (i -1);
									isUiFinish = false;
									continue;
								}

								sprintf(time, "%s-%s-%s %s:%s:%s",
									StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
								_SetDateTime(time);
								break;
							}
							if (key == KEY_CANCEL){
								break;
							}

							_Printfxy(8, 5*16, "当前楼栋           ", Color_White);
							_Printfxy(8, 6*16, "抄表时间重置中... ", Color_White);
							//------------------------------------------------------------
							_Select(1);
							_Use(MeterDocDB);	// 打开数据库
							_Go(0);
							for(i = 0; i < recCnt; i++){
								_ReadField(Idx_DistrictNum, strTmp);	// 小区编号 过滤
								strTmp[Size_DistrictNum - 1] = '\0';
								if(strcmp(Meters.qryDistricNum, strTmp) != 0){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_ReadField(Idx_BuildingNum, strTmp);	// 楼栋编号 过滤
								strTmp[Size_BuildingNum - 1] = '\0';
								if(strcmp(Meters.qryBuildingNum, strTmp) != 0){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
								strTmp[Size_MeterReadStatus - 1] = '\0';
								if(strTmp[0] == '0'){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_Replace(Idx_MeterReadTime, time);
								_Skip(1);
							}
							_Use("");		// 关闭数据库
							//------------------------------------------------------------
							_Printfxy(8, 6*16, "抄表时间重置完成！ ", Color_White);
							_Sleep(2500);
							break;
						
						#if defined Project_8009_RF
						case 7:		// 设置路由
							ShowSettingRoutes();
							break;
						#endif

						default:
							break;
						}

					}// while 4 楼栋抄表
				}// while 3 楼栋列表
			}// while 2 小区列表
			break;

		case 2:		// 清空抄表结果
			//-------------------------------------------------------
			_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  确定要清空       ", Color_White);
			_Printfxy(0, 5*16, "  所有抄表结果吗?   ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
			key = _ReadKey();
			//-------------------------------------------------------
			if(key != KEY_ENTER){
				break;
			}
			_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  清空抄表结果中... ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			//-------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// 打开数据库
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[0] == '0'){
					_Skip(1);	// 下一个数据库记录
					continue;
				}
				_Replace(Idx_MeterReadStatus, "0");
				_Replace(Idx_MeterReadTime, "");
				_Replace(Idx_MeterReadType, "");
				_Replace(Idx_MeterValue, "");
				_Replace(Idx_MeterStatusHex, "");
				_Replace(Idx_MeterStatusStr, "");
				_Replace(Idx_BatteryVoltage, "");
				_Replace(Idx_SignalValue, "");
				_Skip(1);		// 下一个数据库记录
			}
			_Use("");			// 关闭数据库
			//-------------------------------------------------------
			_Printfxy(0, 4*16, "  清空抄表结果完成！", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			_Sleep(2500);
			break;

		case 3:		// 重置抄表时间
			//-------------------------------------------------------
			_GUIRectangleFill(0, 3*16 - 8, 160, 7*16 + 8, Color_White);
			_GUIRectangle(0, 3*16 - 8, 160, 7*16 + 8, Color_Black);
			while(true){
				if(false == isUiFinish){
					(*pUiCnt) = 0;
					uiRowIdx = 3;

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
					
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "系统时间:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "确定将抄表时间重置");
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "为当前系统时间吗?");
					
					key = ShowUI(UiList, &currUi);
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;
				
				}
				// 时间有效值校验
				if( (i = TimeStrsToTimeBytes(&TmpBuf[0], StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
					currUi = 1 + (i -1);
					isUiFinish = false;
					continue;
				}

				sprintf(time, "%s-%s-%s %s:%s:%s",
					StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				_SetDateTime(time);
				break;
			}
			//-------------------------------------------------------
			if(key == KEY_CANCEL){
				break;
			}
			_Printfxy(8, 5*16, "所有档案          ", Color_White);
			_Printfxy(8, 6*16, "抄表时间重置中... ", Color_White);
			//------------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// 打开数据库
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[0] == '0'){
					_Skip(1);	// 下一个数据库记录
					continue;
				}
				_Replace(Idx_MeterReadTime, time);
				_Skip(1);
			}
			_Use("");		// 关闭数据库
			//------------------------------------------------------------
			_Printfxy(8, 6*16, "抄表时间重置完成！", Color_White);
			_Sleep(2500);
			break;

		case 4:		// 户表查询
			// 户表查询-界面
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 7, 3, NULL,
				"<<户表查询", 
				3,
				"1. 按表号查询",
				"2. 按户号查询",
				"3. 按门牌号查询");
			while(2){

				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBox(&menuList_2);
				//------------------------------------------------------------
				if(key == KEY_CANCEL){	// 返回
					break;
				}

				// 户表查询-输入界面
				//------------------------------------------------------------
				_ClearScreen();
				sprintf(&TmpBuf[0], "<<%s", &(menuList_2.str[menuList_2.strIdx][3]));
				_Printfxy(0, 0*16, &TmpBuf[0], Color_White);
				_GUIHLine(0, 1*16 + 4, 160, Color_Black);

				Meters.qryType = QryBy_KeyWord;		// 查询类型：关键字
				Meters.qryKeyWord = StrBuf[0];
				switch (menuList_2.strIdx + 1){
				case 1: ptr = "输入表号: "; Meters.selectField = Idx_MeterNum; break;
				case 2: ptr = "输入户号: "; Meters.selectField = Idx_UserNum; break;
				case 3: ptr = "输入门牌号: "; Meters.selectField = Idx_UserRoomNum; break;
				default: break;
				}
				_Printfxy(0, 2*16, ptr, Color_White);
				StrBuf[0][0] = 0x00;
				inputSt.left = 0;
				inputSt.top = 3*16;
				inputSt.width = 10*16;
				inputSt.hight = 16;
				inputSt.caption = "";
				inputSt.context = StrBuf[0];
				inputSt.datelen = 20;
				inputSt.IsClear = true;
				inputSt.keyUpDown = false;
				inputSt.type = 1;
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);	
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				//------------------------------------------------------------
				if(KEY_CANCEL ==  _GetStr(&inputSt)){
					continue;
				}
				StringTrimStart(StrBuf[0],  ' ');
				StringTrimEnd(StrBuf[0],  ' ');

				// 关键字 查询
				_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
				QueryMeterListByKeyword(&Meters, &DbQuery);	
				if(DbQuery.resultCnt > 0){
					ShowMeterList(&Meters);
				}
				else{
					_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
					_Printfxy(0, 4*16, " 查询失败，未找到 ", Color_White);
					_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
					_Sleep(2500);
				}
				
			} // while 2 户表查询
			break;

		case 5:		// 抄表统计
			// 抄表统计-界面
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 3, 3, NULL,
				"<<抄表统计", 
				3, 
				"1. 小区选择>",
				"2. 楼栋选择>",
				"3. 统计");
			//------------------------------------------------------------
			sprintf(qryStrXq, "全部");
			sprintf(qryStrLd, "全部");
			qryTypeXq = 0;		// 0 - 全部， 1 - 选择某个
			qryTypeLd = 0;		// 0 - 全部， 1 - 选择某个
			qryIndexXq = 0;		// 当前选择的小区 在列表中索引
			qryIndexLd = 0;		// 当前选择的楼栋 在列表中索引
			while(2){

				_ClearScreen();
				// 抄表统计-界面
				//---------------------------------------------------------------------
				dispBuf = &DispBuf;
				dispIdx = 0;
				dispIdx += sprintf(&dispBuf[dispIdx], "小区: %s\n", qryStrXq);
				dispIdx += sprintf(&dispBuf[dispIdx], "楼栋: %s", qryStrLd);
				PrintfXyMultiLine(0, 5*16, dispBuf, 7);
				//----------------------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBox(&menuList_2);
				//----------------------------------------------------------------------
				if(key == KEY_CANCEL){	// 返回
					break;
				}
				
				if(menuList_2.strIdx == 0){		// 小区选择
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 2*16, 12, 2, 2, NULL, 
					"小区选择", 2, "全部", "选择某个");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// 未选择，返回
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeXq = 0;
						sprintf(qryStrXq, "全部");
					}
					else{
						// 小区列表-界面
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
						QueryDistrictList(&Districts, &DbQuery);
						ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
							"<<小区选择", Districts.names, Size_ListStr, Districts.cnt);
						_Printfxy(0, 9*16, "返回            确定", Color_White);
						key = ShowListBoxEx(&XqList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// 未选择列表项
							continue;
						}

						qryTypeXq = 1;
						qryIndexXq = XqList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// 打开数据库
						recCnt = _Reccount();
						if(_LocateEx(Idx_DistrictNum, '=', Districts.nums[qryIndexXq], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_DistrictName, qryStrXq);	// 读出小区名
						}
						_Use("");			// 关闭数据库
					}

				}
				else if(menuList_2.strIdx == 1){	// 楼栋选择
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 3*16, 12, 2, 2, NULL, 
					"楼栋选择", 2, "全部", "选择某个");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// 未选择，返回
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeLd = 0;
						sprintf(qryStrLd, "全部");
					}
					else{

						if(qryTypeXq == 0){
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " 选择某个楼栋时 ", Color_White);
							_Printfxy(0, 5*16, " 必须选择所在小区! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							continue;
						}

						// xx小区-楼栋列表-界面
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
						Buildings.qryDistricNum = Districts.nums[qryIndexXq];
						QueryBuildingList(&Buildings, &DbQuery);
						ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
							"<<楼栋选择", Buildings.names, Size_ListStr, Buildings.cnt);

						_Printfxy(0, 9*16, "返回            确定", Color_White);
						key = ShowListBoxEx(&LdList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// 未选择列表项
							continue;
						}

						qryTypeLd = 1;
						qryIndexLd = LdList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// 打开数据库
						recCnt = _Reccount();
						if(_LocateEx(Idx_BuildingNum, '=', Buildings.nums[qryIndexLd], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_BuildingName, qryStrLd);	// 读出楼栋名
						}
						_Use("");			// 关闭数据库
					}
					
				}
				else {	// if(menuList_2.strIdx == 3){	// 统计
					Meters.qryDistricNum = (qryTypeXq == 0 ? NULL : Districts.nums[qryIndexXq]);
					Meters.qryBuildingNum = (qryTypeLd == 0 ? NULL : Buildings.nums[qryIndexLd]);
					Meters.selectField = Idx_Invalid;
					_Printfxy(0, 9*16, "    <  统计中  >    ", Color_White);
					QueryMeterList(&Meters, &DbQuery);
					if(Meters.districName[0] == 0x00){
						strcpy(Meters.districName, "全部");
					}
					if(Meters.buildingName[0] == 0x00){
						strcpy(Meters.buildingName, "全部");
					}
					key = ShowMeterReadCountInfo(&Meters);
				}
			}
			break;

		#ifdef UseFunc_ReSetDistrictAndBuildingNo
		case 6:		// 小区楼栋重新编号
			ReSetDistrictAndBuildingNo();
			break;
		#endif

		default: 
			break;
		}

	} // while 1 批量抄表

	FixDbfRecCnt();	// 修复记录总数
	MeterInfo.dbIdx = Invalid_dbIdx;  // 清空当前表数据库索引，防止抄表结果写入
}
#endif

// 批量开关阀 - 6009/8009通用版
#ifdef UseFunc_BatchOpenCloseValve
void BatchOpenCloseValve(void)
{
	uint8 key;
	ListBox menuList, menuList_2, menuList_3;
	ListBoxEx XqList, LdList;				// 小区/楼栋列表
	_GuiInputBoxStru inputSt;
	uint8 *ptr;
	uint16 dispIdx, i;
	char *dispBuf = &DispBuf, *strTmp = &TmpBuf[0];
	char *qryStrXq = &TmpBuf[400], *qryStrLd = &TmpBuf[600];
	uint8 qryTypeXq, qryTypeLd;
	uint16 qryIndexXq, qryIndexLd;
	uint32 recCnt;

	recCnt = FixDbfRecCnt();	// 修复记录总数
	MeterInfo.dbIdx = Invalid_dbIdx;  // 清空当前表数据库索引，防止抄表结果写入
	
	if(recCnt == 0){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  当前档案为空！\n 请先下载抄表档案，\n 再进行批量操作");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// 菜单
	//------------------------------------------------------------
	ListBoxCreate(&menuList, 0, 0, 20, 7, 4, NULL,
		"<<批量开关阀",
		4,
		"1. 按楼栋开关阀",
		"2. 清空阀门状态",
		"3. 户表查询",
		"4. 阀门状态统计"
	);
	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);

		switch(menuList.strIdx + 1){
		case 1:		// 按楼栋开关阀

			// 小区列表-界面
			//------------------------------------------------------------
			_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
			QueryDistrictList(&Districts, &DbQuery);
			ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
				"<<小区选择", Districts.names, Size_ListStr, Districts.cnt);
			while(2){
				
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBoxEx(&XqList);
				//------------------------------------------------------------
				if (key == KEY_CANCEL){	// 返回
					break;
				}

				// xx小区-楼栋列表-界面
				//------------------------------------------------------------
				_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
				Buildings.qryDistricNum = Districts.nums[XqList.strIdx];
				QueryBuildingList(&Buildings, &DbQuery);
				ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
					"<<楼栋选择", Buildings.names, Size_ListStr, Buildings.cnt);
				//------------------------------------------------------------
				while(3){
	
					_Printfxy(0, 9*16, "返回            确定", Color_White);
					key = ShowListBoxEx(&LdList);
					//------------------------------------------------------------
					if(key == KEY_CANCEL){	// 返回
						break;
					}

					// 清空路由
					StrRelayAddr[0][0] = 0x00;
					StrRelayAddr[1][0] = 0x00;
					StrRelayAddr[2][0] = 0x00;

					// 楼栋抄表-界面
					//------------------------------------------------------------
					Meters.qryDistricNum = Districts.nums[XqList.strIdx];
					Meters.qryBuildingNum = Buildings.nums[LdList.strIdx];
				#ifdef Protocol_6009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 7, NULL,
						"<<按楼栋开关阀", 
						7,
						"1. 自动批量开阀",
						"2. 自动批量关阀",
						"3. 阀门未知列表",
						"4. 已开阀列表",
						"5. 已关阀列表",
						"6. 阀门状态统计",
						"7. 清空阀门状态");
				#else // Protocol_8009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 8, NULL,
						"<<按楼栋开关阀", 
						8,
						"1. 自动批量开阀",
						"2. 自动批量关阀",
						"3. 阀门未知列表",
						"4. 已开阀列表",
						"5. 已关阀列表",
						"6. 阀门状态统计",
						"7. 清空阀门状态",
						"8. 设置路由");
				#endif
					while(4){

						_Printfxy(0, 9*16, "返回            确定", Color_White);
						key = ShowListBox(&menuList_2);
						//------------------------------------------------------------
						if(key == KEY_CANCEL){	// 返回
							break;
						}

						Meters.qryType = QryBy_ValveStatus;		// 查询类型：阀门状态

						switch (menuList_2.strIdx + 1){
						case 1:		// 自动批量开阀
							Meters.selectField = Idx_MeterNum;
							Meters.qryValveStatus = 3;
							_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowAutoOpenCloseValve(&Meters, 1);
							break;
						case 2:		// 自动批量关阀
							Meters.selectField = Idx_MeterNum;
							Meters.qryValveStatus = 4;
							_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowAutoOpenCloseValve(&Meters, 0);
							break;
						case 3:		// 阀门未知列表
							Meters.selectField = Idx_Invalid;
							Meters.qryValveStatus = 2;
							key = ShowMeterList(&Meters);
							break;
						case 4:		// 已开阀列表
							Meters.selectField = Idx_Invalid;
							Meters.qryValveStatus = 1;
							key = ShowMeterList(&Meters);
							break;
						case 5:		// 已关阀列表
							Meters.selectField = Idx_Invalid;
							Meters.qryValveStatus = 0;
							key = ShowMeterList(&Meters);
							break;

						case 6:		// 阀门状态统计
							Meters.selectField = Idx_Invalid;
							_Printfxy(0, 9*16, "    <  统计中  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowValveStatusCountInfo(&Meters);
							break;

						case 7:		// 清空阀门状态
							//-------------------------------------------------------
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " 确定要清空        ", Color_White);
							_Printfxy(0, 5*16, " 当前楼栋阀门状态吗?", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							key = _ReadKey();
							//-------------------------------------------------------
							if(key != KEY_ENTER){
								break;
							}
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, "  当前楼栋         ", Color_White);
							_Printfxy(0, 5*16, "  阀门状态清空中... ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							//------------------------------------------------------------
							_Select(1);
							_Use(MeterDocDB);	// 打开数据库
							_Go(0);
							for(i = 0; i < recCnt; i++){
								_ReadField(Idx_DistrictNum, strTmp);	// 小区编号 过滤
								strTmp[Size_DistrictNum - 1] = '\0';
								if(strcmp(Meters.qryDistricNum, strTmp) != 0){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_ReadField(Idx_BuildingNum, strTmp);	// 楼栋编号 过滤
								strTmp[Size_BuildingNum - 1] = '\0';
								if(strcmp(Meters.qryBuildingNum, strTmp) != 0){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_ReadField(Idx_ValveStatus, strTmp);	// 阀门状态 过滤
								strTmp[Size_ValveStatus - 1] = '\0';
								if(strTmp[0] != '0' && strTmp[0] != '1'){
									_Skip(1);	// 下一个数据库记录
									continue;
								}

								_Replace(Idx_ValveStatus, "");
								_Skip(1);
							}
							_Use("");		// 关闭数据库
							//------------------------------------------------------------
							_Printfxy(0, 5*16, "  阀门状态清空完成! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							break;

						
						
						#if defined Project_8009_RF
						case 8:		// 设置路由
							ShowSettingRoutes();
							break;
						#endif

						default:
							break;
						}

					}// while 4 楼栋开关阀
				}// while 3 楼栋列表
			}// while 2 小区列表
			break;

		case 2:		// 清空阀门状态
			//-------------------------------------------------------
			_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  确定要清空       ", Color_White);
			_Printfxy(0, 5*16, "  所有阀门状态吗?   ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
			key = _ReadKey();
			//-------------------------------------------------------
			if(key != KEY_ENTER){
				break;
			}
			_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  清空阀门状态中... ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			//-------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// 打开数据库
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_ValveStatus, strTmp);	// 阀门状态 过滤
				strTmp[Size_ValveStatus - 1] = '\0';
				if(strTmp[0] != '0' && strTmp[0] != '1'){
					_Skip(1);	// 下一个数据库记录
					continue;
				}
				_Replace(Idx_ValveStatus, "");
				_Skip(1);		// 下一个数据库记录
			}
			_Use("");			// 关闭数据库
			//-------------------------------------------------------
			_Printfxy(0, 4*16, "  清空阀门状态完成！", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			_Sleep(2500);
			break;

		case 3:		// 户表查询
			// 户表查询-界面
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 7, 3, NULL,
				"<<户表查询", 
				3,
				"1. 按表号查询",
				"2. 按户号查询",
				"3. 按门牌号查询");
			while(2){

				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBox(&menuList_2);
				//------------------------------------------------------------
				if(key == KEY_CANCEL){	// 返回
					break;
				}

				// 户表查询-输入界面
				//------------------------------------------------------------
				_ClearScreen();
				sprintf(&TmpBuf[0], "<<%s", &(menuList_2.str[menuList_2.strIdx][3]));
				_Printfxy(0, 0*16, &TmpBuf[0], Color_White);
				_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
				Meters.qryType = QryBy_KeyWord;		// 查询类型：关键字
				Meters.qryKeyWord = StrBuf[0];
				switch (menuList_2.strIdx + 1){
				case 1: ptr = "输入表号: "; Meters.selectField = Idx_MeterNum; break;
				case 2: ptr = "输入户号: "; Meters.selectField = Idx_UserNum; break;
				case 3: ptr = "输入门牌号: "; Meters.selectField = Idx_UserRoomNum; break;
				default: break;
				}
				_Printfxy(0, 2*16, ptr, Color_White);
				StrBuf[0][0] = 0x00;
				inputSt.left = 0;
				inputSt.top = 3*16;
				inputSt.width = 10*16;
				inputSt.hight = 16;
				inputSt.caption = "";
				inputSt.context = StrBuf[0];
				inputSt.datelen = 20;
				inputSt.IsClear = true;
				inputSt.keyUpDown = false;
				inputSt.type = 1;
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);	
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				//------------------------------------------------------------
				if(KEY_CANCEL ==  _GetStr(&inputSt)){
					continue;
				}
				StringTrimStart(StrBuf[0],  ' ');
				StringTrimEnd(StrBuf[0],  ' ');

				// 关键字 查询
				_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
				QueryMeterListByKeyword(&Meters, &DbQuery);	
				if(DbQuery.resultCnt > 0){
					ShowMeterList(&Meters);
				}
				else{
					_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
					_Printfxy(0, 4*16, " 查询失败，未找到 ", Color_White);
					_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
					_Sleep(2500);
				}
				
			} // while 2 户表查询
			break;

		case 4:		// 阀门状态统计
			// 界面
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 3, 3, NULL,
				"<<抄表统计", 
				3, 
				"1. 小区选择>",
				"2. 楼栋选择>",
				"3. 统计");
			//------------------------------------------------------------
			sprintf(qryStrXq, "全部");
			sprintf(qryStrLd, "全部");
			qryTypeXq = 0;		// 0 - 全部， 1 - 选择某个
			qryTypeLd = 0;		// 0 - 全部， 1 - 选择某个
			qryIndexXq = 0;		// 当前选择的小区 在列表中索引
			qryIndexLd = 0;		// 当前选择的楼栋 在列表中索引
			while(2){

				_ClearScreen();
				// 抄表统计-界面
				//---------------------------------------------------------------------
				dispBuf = &DispBuf;
				dispIdx = 0;
				dispIdx += sprintf(&dispBuf[dispIdx], "小区: %s\n", qryStrXq);
				dispIdx += sprintf(&dispBuf[dispIdx], "楼栋: %s", qryStrLd);
				PrintfXyMultiLine(0, 5*16, dispBuf, 7);
				//----------------------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBox(&menuList_2);
				//----------------------------------------------------------------------
				if(key == KEY_CANCEL){	// 返回
					break;
				}
				
				if(menuList_2.strIdx == 0){		// 小区选择
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 2*16, 12, 2, 2, NULL, 
					"小区选择", 2, "全部", "选择某个");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// 未选择，返回
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeXq = 0;
						sprintf(qryStrXq, "全部");
					}
					else{
						// 小区列表-界面
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
						QueryDistrictList(&Districts, &DbQuery);
						ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
							"<<小区选择", Districts.names, Size_ListStr, Districts.cnt);
						_Printfxy(0, 9*16, "返回            确定", Color_White);
						key = ShowListBoxEx(&XqList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// 未选择列表项
							continue;
						}

						qryTypeXq = 1;
						qryIndexXq = XqList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// 打开数据库
						recCnt = _Reccount();
						if(_LocateEx(Idx_DistrictNum, '=', Districts.nums[qryIndexXq], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_DistrictName, qryStrXq);	// 读出小区名
						}
						_Use("");			// 关闭数据库
					}

				}
				else if(menuList_2.strIdx == 1){	// 楼栋选择
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 3*16, 12, 2, 2, NULL, 
					"楼栋选择", 2, "全部", "选择某个");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// 未选择，返回
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeLd = 0;
						sprintf(qryStrLd, "全部");
					}
					else{

						if(qryTypeXq == 0){
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " 选择某个楼栋时 ", Color_White);
							_Printfxy(0, 5*16, " 必须选择所在小区! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							continue;
						}

						// xx小区-楼栋列表-界面
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
						Buildings.qryDistricNum = Districts.nums[qryIndexXq];
						QueryBuildingList(&Buildings, &DbQuery);
						ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
							"<<楼栋选择", Buildings.names, Size_ListStr, Buildings.cnt);

						_Printfxy(0, 9*16, "返回            确定", Color_White);
						key = ShowListBoxEx(&LdList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// 未选择列表项
							continue;
						}

						qryTypeLd = 1;
						qryIndexLd = LdList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// 打开数据库
						recCnt = _Reccount();
						if(_LocateEx(Idx_BuildingNum, '=', Buildings.nums[qryIndexLd], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_BuildingName, qryStrLd);	// 读出楼栋名
						}
						_Use("");			// 关闭数据库
					}
					
				}
				else {	// if(menuList_2.strIdx == 3){	// 统计
					Meters.qryType = QryBy_ValveStatus;		// 查询类型：阀门状态
					Meters.qryDistricNum = (qryTypeXq == 0 ? NULL : Districts.nums[qryIndexXq]);
					Meters.qryBuildingNum = (qryTypeLd == 0 ? NULL : Buildings.nums[qryIndexLd]);
					Meters.selectField = Idx_Invalid;
					_Printfxy(0, 9*16, "    <  统计中  >    ", Color_White);
					QueryMeterList(&Meters, &DbQuery);
					if(Meters.districName[0] == 0x00){
						strcpy(Meters.districName, "全部");
					}
					if(Meters.buildingName[0] == 0x00){
						strcpy(Meters.buildingName, "全部");
					}
					key = ShowValveStatusCountInfo(&Meters);
				}
			}
			break;

		default: 
			break;
		}

	} // while 1 批量抄表

	FixDbfRecCnt();	// 修复记录总数
	MeterInfo.dbIdx = Invalid_dbIdx;  // 清空当前表数据库索引，防止抄表结果写入
}
#endif


#ifdef UseFunc_ReSetDistrictAndBuildingNo
bool IsReSetNo(void)
{
	bool ret = true, isOK;
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	char *ptr;
	char *xqName, *ldName;
	int buildNo;

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	
	if(recCnt == 0) return false;

	_Go(recCnt - 3);

	for(i = 0; i < 3; i++){
		
		xqName = &TmpBuf[1000];
		ldName = &TmpBuf[1300];

		_ReadField(Idx_DistrictName, xqName);	// 小区名称
		xqName[Size_ListStr - 1] = '\0';

		// 读取用户地址 , 重设楼栋名称	
		_ReadField(Idx_UserAddr, ldName);	

		// 楼栋名称去掉前面小区部分
		ptr = &xqName[0];
		while(*ptr != '\0'){
			if(*ptr != *ldName){
				break;
			}
			ptr++;
			ldName++;
		}
		// 截取最多20字符，并去掉分隔符‘-’后面内容
		ldName[Size_ListStr - 1] = '\0';
		ptr = &ldName[0];
		while(*ptr != '\0'){
			if(*ptr == '-'){
				*ptr = '\0';
				break;
			}
			ptr++;
		}
		// 去掉最后无法显示的一字节
		StringFixGbkStrEndError(ldName);

		// 楼栋名称 是否是 从地址里取出的？
		_ReadField(Idx_BuildingName, strTmp);	
		strTmp[Size_ListStr - 1] = '\0';
		if(strcmp(strTmp, ldName) != 0){
			ret = false;
			break;
		}

		// 楼栋编号 是否小于 最大值
		_ReadField(Idx_BuildingNum, strTmp);	
		strTmp[Size_ListStr - 1] = '\0';
		isOK = StringToInt(strTmp, &buildNo);
		if(!isOK || buildNo < 0 || buildNo > AllBuildings_Max){
			ret = false;
			break;
		}

		_Skip(1);	// 下一个数据库记录
	}

	_Use("");					// 关闭数据库

	return ret;
}

void ReSetDistrictAndBuildingNo(void)
{
	uint32 i, recCnt, shutdownTime;
	char strTmp[Size_DbStr];
	char *ptr;
	uint8 lcdCtrl, key;
	char *xqName, *ldName;
	int xqNo, ldNo, j, buildingsCnt;
	DistrictListSt *districts = &Districts;
	NumNameSt *buildings = (NumNameSt *)&BigBuf[0];

	if(0 == FixDbfRecCnt()){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  当前档案为空！\n 请先下载抄表档案，\n 再进行批量操作");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// 界面显示
	_ClearScreen();
	_Printfxy(0, 0, "<<小区楼栋重新编号", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	PrintfXyMultiLine_VaList(0, 3*16, "  确定要将小区和楼栋重新编号吗？");
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "取消            确定", Color_White);

	if(_ReadKey() != KEY_ENTER){
		return;
	}

	if(IsReSetNo()){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  小区楼栋已编号过了！\n  是否重新编号？");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Printfxy(0, 9*16, "取消            确定", Color_White);
		if(_ReadKey() != KEY_ENTER){
			return;
		}
	}

	_GUIRectangleFill(0, 2*16, 10*16, 5*16, Color_White);
	_Printfxy(0, 9*16, "  <  编号中 ...  >  ", Color_White);

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);

	xqNo = 0;
	ldNo = 0;
	districts->cnt = 0;
	buildingsCnt = 0;
	key = KEY_NOHOOK;
	lcdCtrl = 0;

	// 防止自动抄表时关机，重置自动关机时间
	shutdownTime = _GetShutDonwTime();
	_SetShutDonwTime(0);		// 20 - 999 有效，0 - 关闭自动关机

	for(i = 0; i < recCnt; i++){

		xqName = &TmpBuf[1000];
		ldName = &TmpBuf[1300];

		// LCD背景灯控制
		LcdLightCycleCtrl(&lcdCtrl, 30);

		_ReadField(Idx_DistrictName, xqName);	// 小区名称
		xqName[Size_ListStr - 1] = '\0';

		// 小区名称: 排除重复后，加入列表中
		xqNo = FindStrInList(districts->names, Size_ListStr, districts->cnt, xqName, Size_ListStr);
		if(-1 == xqNo){
			if(districts->cnt >= District_Max){
				sprintf(strTmp, " 小区数 超出最大限制 %d !", District_Max);
				ShowMsg(8, 2*16, strTmp, 3000);
				break;
			}
			xqNo = districts->cnt;
			sprintf(strTmp,"%d",xqNo);
			strcpy(districts->nums[districts->cnt], strTmp);
			strcpy(districts->names[districts->cnt], xqName);
			districts->cnt++;
		}
		_Replace(Idx_DistrictNum, districts->nums[xqNo]);	// 设置小区编号

		// 读取用户地址 , 用来重设楼栋名称	
		_ReadField(Idx_UserAddr, ldName);	

		// 楼栋名称去掉前面小区部分
		ptr = &xqName[0];
		while(*ptr != '\0'){
			if(*ptr != *ldName){
				break;
			}
			ptr++;
			ldName++;
		}
		// 截取最多20字符，并去掉分隔符‘-’后面内容
		ldName[Size_ListStr - 1] = '\0';
		ptr = &ldName[0];
		while(*ptr != '\0'){
			if(*ptr == '-'){
				*ptr = '\0';
				break;
			}
			ptr++;
		}
		// 去掉最后无法显示的一字节
		StringFixGbkStrEndError(ldName);
		
		_Replace(Idx_BuildingName, ldName);	


		// 楼栋名称: 排除重复后，加入列表中
		for(j = 0; j < buildingsCnt; j++){
			if(strcmp(buildings[j].name, ldName) == 0){
				break;
			}
		}
		ldNo = j;
		if(buildingsCnt == ldNo){
			if(buildingsCnt >= AllBuildings_Max){
				sprintf(strTmp, " 所有小区楼栋数 超出最大限制 %d !", AllBuildings_Max);
				ShowMsg(8, 2*16, strTmp, 3000);
				break;
			}
			//sprintf(strTmp,"%d",ldNo);
			sprintf(buildings[buildingsCnt].num, "%d", ldNo);
			strcpy(buildings[buildingsCnt].name, ldName);
			buildingsCnt++;
		}
		_Replace(Idx_BuildingNum, buildings[ldNo].num);		// 设置楼栋编号

		_Skip(1);	// 下一个数据库记录

		PrintfXyMultiLine_VaList(0, 4*16, "小区总数：%d", districts->cnt);
		PrintfXyMultiLine_VaList(0, 5*16, "楼栋总数：%d", buildingsCnt);
		sprintf(strTmp, "当前进度: %d/%d",  i + 1, recCnt);
		_Printfxy(0, 6*16 + 8, strTmp, Color_White);
		_GUIHLine(0, 7*16 + 8 + 1, 160, Color_Black);
		ShowProgressBar(7*16 + 8 + 3, recCnt, i + 1);	// 进度条

		// 按键处理
		key = _GetKeyExt();
		CycleInvoke_OpenLcdLight_WhenKeyPress(key);

		if(key == KEY_CANCEL){
			break;
		}
	}
	_Use("");		// 关闭数据库

	_OpenLcdBackLight();
	_SetShutDonwTime(shutdownTime);	

	if(key == KEY_CANCEL){
		_Printfxy(0, 9*16, "返回  <已取消>  确定", Color_White);
	}
	else{
		_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);
	}

	while(1){
		key = _ReadKey();
		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}
		_Sleep(100);
	}
}
#endif