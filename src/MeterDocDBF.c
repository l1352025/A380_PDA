
#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "MeterDocDBF.h"
#include "Common.h"
#ifdef Project_6009_RF
#include "WaterMeter.h"
#else
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
	char strTmp1[Size_DbMaxStr];
	char strTmp2[Size_DbMaxStr];

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
				break;
			}

			_ReadField(Idx_DistrictName, strTmp2);	// 小区名称
			strTmp2[Size_ListStr - 1] = '\0';

			strncpy(districts->nums[districts->cnt], strTmp1, Size_ListStr);
			strncpy(districts->names[districts->cnt], strTmp2, Size_ListStr);
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
	char strTmp[Size_DbMaxStr];
	char strTmp1[Size_DbMaxStr];
	char strTmp2[Size_DbMaxStr];

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
				break;
			}

			_ReadField(Idx_BuildingName, strTmp2);	// 楼栋名称
			strTmp2[Size_ListStr -1] = '\0';

			strncpy(buildings->nums[buildings->cnt], strTmp1, Size_ListStr);
			strncpy(buildings->names[buildings->cnt], strTmp2, Size_ListStr);
			buildings->cnt++;
		}
		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：查询xx小区-xx楼栋-抄表统计情况
* 参 数：meters		- 户表列表：查询前先设置 meters.(小区编号/楼栋编号/抄表状态/显示字段)
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterList(MeterListSt *meters, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbMaxStr];

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
	query->reqMaxCnt = Meter_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
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
		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';
		if(strcmp(strTmp, "1") == 0){
			meters->readOkCnt++;		// 成功数量
		}
		else if(strcmp(strTmp, "2") == 0){
			meters->readNgCnt++;		// 失败数量
		}else{
			// 0 - 未抄数量
		}
		
		if(meters->qryMeterReadStatus != NULL){			// 抄表状态 过滤  ‘0’ - 未抄/失败， ‘1’ - 已抄
			if((meters->qryMeterReadStatus[0] == '1' && strTmp[0] != '1')
				|| (meters->qryMeterReadStatus[0] == '0' && strTmp[0] == '1')){
				_Skip(1);	// 下一个数据库记录
				continue;
			}
		}

		switch (meters->selectField)		// 列表类型：默认为表号列表
		{
		case Idx_MeterNum:
		case Idx_UserNum:
		case Idx_UserRoomNum:
		case Idx_UserName:
		case Idx_UserAddrs:
			break;
		default: 
			meters->selectField = Idx_Invalid;
			break;
		}
		if(meters->selectField == Idx_Invalid){		// 未选择字段，则不构建列表
			_Skip(1);	// 下一个数据库记录
			continue;
		}

		_ReadField(meters->selectField, strTmp);	// 读取字段：表号/户号/门牌号/户名/地址
		strTmp[Size_ListStr - 1] = '\0';			// 最多显示一行

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;
			break;
		}

		// 将选择的字段信息 和 数据库索引 加入列表
		strncpy(meters->strs[meters->cnt], strTmp, Size_ListStr);	
		meters->dbIdx[meters->cnt] = (i + 1);	// 数据库索引从 1 开始编号
		meters->cnt++;

		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}


/*
* 描 述：显示 xx小区-xx楼栋- 自动抄表
* 参 数：meters		- 抄表情况列表
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowAutoMeterReading(MeterListSt *meters)
{
	uint8 key, i, cnt;
	uint16 ackLen, timeout, dispIdx;
	uint8 tryCnt, lcdCtrl;
	CmdResult cmdResult = CmdResult_Ok;
	char *dispBuf = &DispBuf;
	MeterInfoSt *meterInfo = &MeterInfo;
	char strTmp[20];
	uint32 shutdownTime;

	if(meters->cnt == 0){
		return KEY_CANCEL;
	}

	// 中继清空
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
			CurrCmdName = strTmp;
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
		strcpy(StrDstAddr, meterInfo->meterNum);
		
		// 命令参数处理
		i = 0;	
		CurrCmd = WaterCmd_ReadRealTimeData;	// 读实时数据
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		#ifdef Project_6009_RF
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 21;				// 应答长度 21	
			Args.buf[i++] = 0x00;		// 数据格式 00	
		#else // Project_8009_RF
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 9;					// 应答长度 9	
		#endif
		
		Args.lastItemLen = i - 1;

		// 地址填充
		#ifdef Project_6009_RF
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#else
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#endif

		// 应答长度、超时时间、重发次数
		#ifdef Project_6009_RF
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 2;
		#else // Project_8009_RF
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 2;
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
		}
		else{
			// 失败，不作处理
			_Printfxy(0, 9*16, " < 当前抄表: 失败 > ", Color_White);
			meters->readNgCnt++;
		}
		meterInfo->meterReadType[0] = '0';		// 抄表方式： 掌机抄表
		SaveMeterReadResult(meterInfo);
		_Sleep(1000);

		// 显示电量
		_Printfxy(0, 9*16, "                   ", Color_White);
		_DispTimeSys();	
		_Sleep(1000);

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
		if(key == KEY_CANCEL || KEY_ENTER){
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

	_Printfxy(0, 0, "<<抄表统计", Color_White);
	/*---------------------------------------------*/
	dispIdx += sprintf(&dispBuf[dispIdx], "小区: %s\n", meters->districName);
	dispIdx += sprintf(&dispBuf[dispIdx], "楼栋: %s\n", meters->buildingName);
	dispIdx += sprintf(&dispBuf[dispIdx], "总数: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "已抄: %d\n", meters->readOkCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "未抄: %d\n", meters->readNgCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* 描 述：显示xx小区-xx楼栋-已抄/未抄列表
* 参 数：meters		- 抄表情况列表: 调用前先设置抄表状态 qryMeterReadStatus
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterList(MeterListSt *meterReadList)
{
	uint8 key;
	ListBox showTpList, meterList;		// 显示方式/表信息-列表
	char *title = NULL;
	MeterListSt *meters = meterReadList;

	// 列表显示方式-界面
	title = (meters->qryMeterReadStatus[0] == '1' ? "<<已抄列表" : "<<未抄列表");
	ListBoxCreate(&showTpList, 0, 0, 20, 7, 4, NULL,
		title, 
		4,
		"1. 按表号显示",
		"2. 按户号显示",
		"3. 按户名显示",
		"4. 按门牌号显示");
	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&showTpList);
		//------------------------------------------------------------
		if(key == KEY_CANCEL){	// 返回
			break;
		}

		switch (showTpList.strIdx + 1){
		case 1:	meters->selectField = Idx_MeterNum;	
			break;
		case 2:	meters->selectField = Idx_UserNum;	
			break;
		case 3:	meters->selectField = Idx_UserName;	
			break;
		case 4:	meters->selectField = Idx_UserRoomNum;	
			break;
		default:
			break;
		}
		// 已抄/未抄列表-界面
		//------------------------------------------------------------
		_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
		QueryMeterList(meters, &DbQuery);	// 已抄/未抄列表 查询
		ListBoxCreateEx(&meterList, 0, 0, 20, 7, meters->cnt, NULL,
				title, meters->strs, Size_ListStr, meters->cnt);
		while(2){

			_Printfxy(0, 9*16, "返回        户表信息", Color_White);
			key = ShowListBox(&meterList);
			//------------------------------------------------------------
			if(key == KEY_CANCEL){	// 返回
				break;
			}
			if(key == KEY_ENTER && meters->cnt == 0){	
				continue;
			}

			while(3){
				// 户表信息-界面
				//------------------------------------------------------
				meters->idx = meterList.strIdx;
				MeterInfo.dbIdx = meters->dbIdx[meters->idx];
				MeterInfo.strIdx = meters->idx;
				MeterInfo.strCnt = meters->cnt;
				QueryMeterInfo(&MeterInfo, &DbQuery);	// 户表信息查询
				key = ShowMeterInfo(&MeterInfo);	
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
			} // while 3 户表信息

		}// while 2 已抄/未抄列表

	}// while 1 显示方式
	
	return key;
}

/*
* 描 述：保存抄表结果
* 参 数：meterInfo	- 户表信息
* 返 回：void
*/
void SaveMeterReadResult(MeterInfoSt *meterInfo)
{
	char time[20];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	_Go(meterInfo->dbIdx);

	meterInfo->meterReadStatus[1] = '\0';
	meterInfo->meterReadType[1] = '\0';

	// 更新抄表结果
	_GetDateTime(time, '-', ':');
	_Replace(Idx_MeterReadStatus, meterInfo->meterReadStatus);	// 抄表状态 0 - 未抄， 1 - 成功， 2 - 失败
	_Replace(Idx_MeterReadType, meterInfo->meterReadType);		// 抄表方式 0 - 掌机抄表 ， 1 - 手工录入
	_Replace(Idx_MeterReadTime, time);							// 抄表时间
	_Replace(Idx_MeterValue, meterInfo->meterValue);
	_Replace(Idx_MeterStatusHex, meterInfo->meterStatusHex);
	_Replace(Idx_MeterStatusStr, meterInfo->meterStatusStr);
	_Replace(Idx_BatteryVoltage, meterInfo->batteryVoltage);
	_Replace(Idx_SignalValue, meterInfo->signalValue);

	_Use("");			// 关闭数据库
}

/*
* 描 述：查询户表信息
* 参 数：meterInfo	- 户表信息
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterInfo(MeterInfoSt *meterInfo, DbQuerySt *query)
{
	uint32 i, recCnt;
	char *strTmp = &TmpBuf[0];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	query->resultCnt = 0;
	query->errorCode = 0;

	if(meterInfo->dbIdx != Invalid_dbIdx){	// 数据库记录索引 是否有效？
		if(meterInfo->dbIdx > recCnt -1){
			meterInfo->dbIdx = Invalid_dbIdx;
		}
	}

	if(meterInfo->dbIdx == Invalid_dbIdx){	// 数据库记录索引无效时 执行查询

		#if 1
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
		strncpy(meterInfo->meterNum, strTmp, Size_MeterNum);
		meterInfo->meterNum[AddrLen * 2] = 0x00;	
		StringPadLeft(meterInfo->meterNum, AddrLen * 2, '0');

		_ReadField(Idx_UserNum, strTmp);					// 户号
		strncpy(meterInfo->userNum, strTmp, Size_UserNum);	

		_ReadField(Idx_UserRoomNum, strTmp);				// 门牌号
		StringFixGbkStrEndError(strTmp);
		strncpy(meterInfo->roomNum, strTmp, Size_RoomNum);	

		_ReadField(Idx_MeterReadStatus, strTmp);			// 抄表状态
		strTmp[Size_MeterReadStatus - 1] = '\0';
		strncpy(meterInfo->meterReadStatus, strTmp, Size_MeterReadStatus);	

		_ReadField(Idx_UserName, strTmp);					// 户名
		strTmp[Size_UserName - 1] = '\0';
		strncpy(meterInfo->userName, strTmp, Size_UserName);	

		_ReadField(Idx_UserMobileNum, strTmp);			// 手机号
		strTmp[Size_MobileNum - 1] = '\0';
		strncpy(meterInfo->mobileNum, strTmp, Size_MobileNum);	

		_ReadField(Idx_UserAddrs, strTmp);				// 地址
		StringFixGbkStrEndError(strTmp);
		strncpy(meterInfo->userAddr, strTmp, Size_UserAddr);	

		_ReadField(Idx_MeterReadType, strTmp);			// 抄表方式
		strTmp[Size_MeterReadType - 1] = '\0';
		strncpy(meterInfo->meterReadType, strTmp, Size_MeterReadType);	

		_ReadField(Idx_MeterReadTime, strTmp);			// 抄表时间
		strTmp[Size_MeterReadTime - 1] = '\0';
		strncpy(meterInfo->meterReadTime, strTmp, Size_MeterReadTime);	

		_ReadField(Idx_MeterValue, strTmp);				// 表读数
		strTmp[Size_MeterValue - 1] = '\0';
		strncpy(meterInfo->meterValue, strTmp, Size_MeterValue);	

		_ReadField(Idx_MeterStatusStr, strTmp);			// 表状态
		strTmp[Size_MeterStatusStr - 1] = '\0';
		strncpy(meterInfo->meterStatusStr, strTmp, Size_MeterStatusStr);	

		_ReadField(Idx_BatteryVoltage, strTmp);			// 电池电压
		strTmp[Size_BatteryVoltage - 1] = '\0';
		strncpy(meterInfo->batteryVoltage, strTmp, Size_BatteryVoltage);	

		_ReadField(Idx_SignalValue, strTmp);			// 信号强度
		strTmp[Size_SignalValue - 1] = '\0';
		strncpy(meterInfo->signalValue, strTmp, Size_SignalValue);	
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
	uint8 currUi = 0, uiRowIdx, isUiFinish, u8Tmp;
	uint8 tryCnt;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;
	char *dispBuf;

	while(1){
		_ClearScreen();

		// 户表信息-界面
		//-----------------------------------------------------
		PrintfXyMultiLine_VaList(0, 0, "<<户表信息  %3d/%-3d ", meterInfo->strIdx + 1, meterInfo->strCnt);
		dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "户号: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "门牌号: %s\n", meterInfo->roomNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表状态: %s\n", (meterInfo->meterReadStatus[0] == '1' ? "已抄" : "未抄"));
		dispIdx += sprintf(&dispBuf[dispIdx], "户名: %s\n", meterInfo->userName);
		dispIdx += sprintf(&dispBuf[dispIdx], "手机: %s\n", meterInfo->mobileNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "地址: %s\n", meterInfo->userAddr);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表方式: %s\n", 
			(meterInfo->meterReadType[0] == '0' ? "手持机抄表" : (meterInfo->meterReadType[0] == '1' ? "集中器抄表" : " ")));
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

		
		strcpy(StrDstAddr, meterInfo->meterNum);	// 表号设置
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
		CurrCmdName = menuList.str[menuList.strIdx];
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		//--------------------------------------
		sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
		_Printfxy(0, 0, TmpBuf, Color_White);
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
			#ifdef Project_6009_RF
				Args.buf[i++] = 0x01;		// 命令字	01
				ackLen = 21;				// 应答长度 21	
				Args.buf[i++] = 0x00;		// 数据格式 00	
			#else // Project_8009_RF
				Args.buf[i++] = 0x01;		// 命令字	01
				ackLen = 9;					// 应答长度 9	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 2:
			CurrCmd = WaterCmd_OpenValve;			// " 开阀 "
			/*---------------------------------------------*/
			#ifdef Project_6009_RF
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
			#else // Project_8009_RF
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 3:
			CurrCmd = WaterCmd_CloseValve;		// " 关阀 ";
			/*---------------------------------------------*/
			#ifdef Project_6009_RF
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
			#else // Project_8009_RF
				Args.buf[i++] = 0x06;		// 命令字	06
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 4:
			CurrCmd = WaterCmd_ClearException;		// " 清异常命令 ";
			/*---------------------------------------------*/
			#ifdef Project_6009_RF
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 1;					// 应答长度 1	
				Args.buf[i++] = 0x00;		// 命令选项 00	
			#else // Project_8009_RF
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 0;					// 应答长度 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 5:
			while(2){
				pUiCnt = 0;
				uiRowIdx = 3;
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "输入读数:", StrBuf[0], 9, 11*8, true);
						pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(false == StringToDecimal(StrBuf[0], 2, &u8Tmp, &u32Tmp, &u16Tmp)){
					sprintf(StrBuf[0], " ");
					continue;
				}

				meterInfo->meterReadStatus[0] = '1';	// 抄表状态： 成功
				meterInfo->meterReadType[0] = '1';		// 抄表方式： 手工录入
				sprintf(MeterInfo.meterValue, "%d.%02d", u32Tmp, (u16Tmp & 0xFF));
				SaveMeterReadResult(meterInfo);
				ShowMsg(16, 3*16, "表读数 录入成功!", 1000);
				break;
			}
			break;

		default: 
			break;
		}

		// 地址填充
		#ifdef Project_6009_RF
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#else
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#endif

		// 应答长度、超时时间、重发次数
		#ifdef Project_6009_RF
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 2;
		#else // Project_8009_RF
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 2;
		#endif

		// 发送、接收、结果显示
		if(false == ProtolCommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt)){
			if(strncmp(DispBuf, "表号", 4) != 0){	// 命令已取消	
				DispBuf[0] = NULL;
			}else{
				meterInfo->meterReadStatus[0] = '2';	// 抄表状态： 失败
			}
		}
		else{
			meterInfo->meterReadStatus[0] = '1';	// 抄表状态： 成功
		}
		meterInfo->meterReadType[0] = '0';		// 抄表方式： 掌机抄表
		SaveMeterReadResult(meterInfo);
		//------------------------------------------------------
		_Printfxy(0, 9*16, "返回            确定", Color_White);
		
		key = ShowScrollStr(&DispBuf, 7);

	}// while 1  户表信息
	

	return key;
}
