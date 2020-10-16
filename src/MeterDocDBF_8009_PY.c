
#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"
#include "MeterDocDBF_8009_PY.h"
#include "ProtoHandle_8009_PY.h"


//-----------------------	全局变量定义	---------------------------------------
SectListSt Sects;
MeterListSt Meters;
MeterInfoSt MeterInfo;
DbQuerySt DbQuery;
uint8 **MetersStrs = Meters.strs;


//-----------------------	内部函数	---------------------------------------
static char * GetStrMtrReadException(uint8 code)
{
	char * ptr = NULL;

	switch (code)
	{
	case MrExcp_Normal: ptr = "正常"; break;
	case MrExcp_Flip: ptr = "翻转"; break;
	case MrExcp_Reverse: ptr = "倒转"; break;
	case MrExcp_FlipReverse: ptr = "翻转且倒转"; break;
	case MrExcp_DataExcp: ptr = "数据异常"; break;
	default: ptr = "未知错误"; break;
	}

	return ptr;
}

static char * GetStrMtrReadType(uint8 code)
{
	char * ptr = NULL;

	switch (code)
	{
	case MrType_BySee: ptr = "见抄"; break;
	case MrType_ByEstimate: ptr = "估抄"; break;
	case MrType_ByDoor: ptr = "门抄"; break;
	case MrType_UserReport: ptr = "用户自报"; break;
	case MrType_EmptyHourse: ptr = "空房"; break;
	case MrType_ZeroGasVol: ptr = "零气量"; break;
	default: ptr = "未知"; break;
	}

	return ptr;
}

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
* 描 述：查询抄表册列表
* 参 数：Sects	- 抄表册列表
*		 query		- 数据库查询结构
* 返 回：void
*/
void QuerySectList(SectListSt *Sects, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp1[Size_DbStr];
	//char strTmp2[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	Sects->cnt = 0;
	Sects->idx = 0;
	query->reqMaxCnt = Sect_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){
		_ReadField(Idx_SectNum, strTmp1);		// 抄表册编号
		strTmp1[Size_ListStr - 1] = '\0';

		// 排除重复后，加入列表中
		if(-1 == FindStrInList(Sects->nums, Size_ListStr, Sects->cnt, strTmp1, Size_ListStr)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				sprintf(strTmp1, " 抄表册数 超出最大限制 %d !", query->reqMaxCnt);
				ShowMsg(8, 3*16, strTmp1, 3000);
				break;
			}

			//_ReadField(Idx_SectName, strTmp2);	// 抄表册名称
			//strTmp2[Size_ListStr - 1] = '\0';

			strcpy(Sects->nums[Sects->cnt], strTmp1);
			//strcpy(Sects->names[Sects->cnt], strTmp2);
			Sects->cnt++;
		}
		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：查询xx抄表册表具列表 / 统计抄表情况
* 参 数：meters		- 户表列表：需设置 meters->selectField， 统计时设置为 Idx_Invalid，查询时设为其他
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
	meters->sectName[0] = 0x00;
	meters->meterCnt = 0;
	meters->readOkCnt = 0;
	meters->readNgCnt = 0;
	query->reqMaxCnt = Meter_Max;
	query->resultCnt = 0;
	query->errorCode = 0;

	switch (meters->selectField)		// 列表类型：默认为表号列表
	{
	case Idx_MeterNum:
	case Idx_UserNum:
	case Idx_UserName:
	case Idx_UserAddr:
		break;
	default: 
		meters->selectField = Idx_Invalid;
		isOnlyCount = true;
		break;
	}

	for(i = 0; i < recCnt; i++){

		if(meters->qrySectNum != NULL){
			_ReadField(Idx_SectNum, strTmp);		// 抄表册编号 过滤
			strTmp[Size_SectNum - 1] = '\0';
			if(strcmp(meters->qrySectNum, strTmp) != 0){
				_Skip(1);	// 下一个数据库记录
				continue;
			}
			else{
				// 保存抄表册名
				// if(meters->sectName[0] == 0x00){
				// 	_ReadField(Idx_SectName, strTmp);	
				// 	strTmp[Size_SectName - 1] = '\0';
				// 	strcpy(meters->sectName, strTmp);
				// }
			}
		}
		
		meters->meterCnt++;				// 当前表总数
		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';

		// 状态转换 1/2/3 --> 0/1/2
		if(strTmp[0] == '0' && strTmp[1] == '1'){
			state = 0;				// 未抄数量
		}
		else if(strTmp[0] == '0' && strTmp[1] == '2'){
			meters->readOkCnt++;		// 成功数量
			state = 1;
		}else{
			meters->readNgCnt++;		// 失败数量
			state = 2;			
		}

		
		// 抄表状态 过滤  ‘0’ - 未抄/失败， ‘1’ - 已抄
		if((meters->qryMeterReadStatus == 1 && state != 1)
			|| (meters->qryMeterReadStatus == 0 && state == 1)){
			_Skip(1);	// 下一个数据库记录
			continue;
		}


		if(isOnlyCount){		// 未选择字段，则不构建列表, 只做统计
			_Skip(1);	// 下一个数据库记录
			continue;
		}

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp, " 该楼表具数 超出最大限制 %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		_ReadField(meters->selectField, strTmp);	// 读取字段：表号/户号/户名/地址
		
		// 将选择的字段信息 和 数据库索引 加入列表
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);
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
	char state;
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
		// 将选择的字段信息 和 数据库索引 加入列表
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp, " 该抄表册表具数 超出最大限制 %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';

		// 状态转换 1/2/3 --> 0/1/2
		if(strTmp[0] == '0' && strTmp[1] == '1'){		// 未抄数量
			state = '0';				
		}
		else if(strTmp[0] == '0' && strTmp[1] == '2'){	// 成功数量
			state = '1';
		}else{											// 失败数量
			state = '2';			
		}

		StringPadRight(meters->strs[meters->cnt], 20, ' ');
		meters->strs[meters->cnt][18] = ' ';	
		meters->strs[meters->cnt][19] = (state == '0' ? 'N' : (state == '1' ? 'Y' : 'F'));
		meters->dbIdx[meters->cnt] = (i + 1);	// 数据库索引从 1 开始编号
		meters->cnt++;

		_Skip(1);	// 下一个数据库记录
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：显示 xx抄表册-xx楼栋- 自动抄表
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
		ShowMsg(1*16, 3*16, "未抄/失败列表为空!", 2000);
		return KEY_CANCEL;
	}

	// 中继检查
	for(i = 0; i < RELAY_MAX; i++){				
		StrRelayAddr[i][0] = 0x00;	// 不使用
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

		_Printfxy(0, 0, "<<自动抄表", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "户号: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "户名: %s\n", meterInfo->userName);
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
		Args.buf[i++] = 0x01;		// 命令字	01
		ackLen = 9;					// 应答长度 9	
		Args.lastItemLen = i - 1;

		// 地址填充
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);


		// 应答长度、超时时间、重发次数
		ackLen += 10 + Addrs.itemCnt * AddrLen;
		timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
		tryCnt = 3;

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
		
		SaveMeterReadResult(meterInfo, 1, readStatus);		// 掌机抄表
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
* 描 述：显示xx抄表册-xx楼栋-抄表统计情况
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
	dispIdx += sprintf(&dispBuf[dispIdx], "抄表册: %s\n \n", meters->sectNum);

	dispIdx += sprintf(&dispBuf[dispIdx], "表具总数: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "已抄成功: %d\n", meters->readOkCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "当前失败: %d\n", meters->readNgCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "当前未抄: %d\n", meters->meterCnt - meters->readOkCnt - meters->readNgCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "返回  < 完成 >  确定", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* 描 述：显示xx抄表册-xx楼栋-已抄/未抄列表
* 参 数：meters		- 抄表情况列表: 调用前先设置抄表状态 qryMeterReadStatus
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
		title = (meters->qryMeterReadStatus == 1 ? "<<已抄成功列表" : "<<未抄失败列表");
		ListBoxCreate(&menuList, 16*4, 16*3, 12, 4, 4, NULL,
			"显示类型", 
			4,
			"1. 表号",
			"2. 户号",
			"3. 户名",
			"4. 地址");

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if(key == KEY_CANCEL){	// 返回
			return key;
		}

		switch (menuList.strIdx + 1){
		case 1:	meters->selectField = Idx_MeterNum;	
			break;
		case 2:	meters->selectField = Idx_UserNum;	
			break;
		case 3:	meters->selectField = Idx_UserName;	
			break;
		case 4:	meters->selectField = Idx_UserAddr;	
			break;
		default:
			break;
		}

		_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
		QueryMeterList(meters, &DbQuery);	// 已抄/未抄列表 查询
	}

	// 已抄/未抄列表/关键字查询结果列表-界面
	//------------------------------------------------------------
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
			state = MeterInfo.meterReadStatus[1];	
			state = state == '1' ? '0' : (state == '2' ? '1' : '2');	// 状态转换 1/2/3 --> 0/1/2
			meters->strs[meterList.strIdx][18] = ' ';
			meters->strs[meterList.strIdx][19] = (state == '0' ? 'N' : (state == '1' ? 'Y' : 'F'));
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
* 		readType - 抄表性质 1 - 见抄，2 - 估抄，3-门抄，4-自报，5-空房，6-零气量
* 		readStatus - 抄表状态：0-未抄，1-已抄, 2-失败
* 返 回：void
*/
void SaveMeterReadResult(MeterInfoSt *meterInfo, uint8 readType, uint8 readStatus)
{
	char time[20];
	double curr, last;
	uint8 isFlip = false;
	char strTmp[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	_Go(meterInfo->dbIdx);

	readStatus = (readStatus == 0 ? 1 : readStatus == 1 ? 2 : 3);	// 抄表状态转换 0/1/2 --> 1/2/3

	_GetDate(time, NULL);
	sprintf(meterInfo->meterReadType, "0%d", readType);
	sprintf(meterInfo->meterReadStatus, "0%d", readStatus);
	sprintf(meterInfo->meterReadTime, "%s", time);
	sprintf(meterInfo->meterExcpType, "01");

	// 更新抄表结果
	_Replace(Idx_MeterReadStatus, meterInfo->meterReadStatus);	// 抄表状态 
	_Replace(Idx_MeterReadType, meterInfo->meterReadType);		// 抄表性质 
	_Replace(Idx_MeterReadDate, meterInfo->meterReadTime);		// 抄表日期
	if(readStatus == 2){

		_ReadField(Idx_LastReadVal, strTmp);
		last = _atof(strTmp);
		curr = _atof(meterInfo->currReadVal);
		if(last > curr){
			curr = last;
			isFlip = true;
		}
		//sprintf(meterInfo->currReadVal, "%0.2lf", curr / 1.0);
		_DoubleToStr(meterInfo->currReadVal, curr / 1.0, 2);
		_Replace(Idx_CurrReadVal, meterInfo->currReadVal);				// 本次读数

		//sprintf(meterInfo->currGasVol, "%0.2lf", curr - last);
		_DoubleToStr(meterInfo->currGasVol, curr - last, 2);
		_Replace(Idx_CurrGasVol, meterInfo->currGasVol);				// 本次气量

		sprintf(meterInfo->meterExcpType, "%s", (isFlip ? "02" : "01"));
		_Replace(Idx_MrExcepType, meterInfo->meterExcpType);			// 读数异常类型

		//sprintf(meterInfo->currReadVal, "%0.2lf", curr / 1.0);
		_DoubleToStr(meterInfo->currElecReadVal, curr / 1.0, 2);
		_Replace(Idx_CurrElecReadVal, meterInfo->currElecReadVal);		// 本次电子读数

		curr = curr - last;
		_ReadField(Idx_LastRemainGasVol, strTmp);
		last = _atof(strTmp);
		curr = curr > last ? 0 : last - curr;
		//sprintf(meterInfo->currReadVal, "%0.2lf", curr);
		_DoubleToStr(meterInfo->currRemainGasVol, curr, 2);
		_Replace(Idx_CurrRemainGasVol, meterInfo->currRemainGasVol);	// 本次剩余气量
	}

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
	char *strTmp = &TmpBuf[0];

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
			else if(meterInfo->qryUserAddr != NULL){
				_ReadField(Idx_UserAddr, strTmp);	// 按地址查询
				strTmp[Size_UserAddr - 1] = '\0';
				if(strcmp(meterInfo->qryUserAddr, strTmp) == 0){
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
		strTmp[Size_MeterNum - 1] = 0x00;	
		strcpy(meterInfo->meterNum, strTmp);

		_ReadField(Idx_UserNum, strTmp);					// 户号
		strTmp[Size_UserNum - 1] = '\0';
		strcpy(meterInfo->userNum, strTmp);	

		_ReadField(Idx_UserName, strTmp);					// 户名
		strTmp[Size_UserName - 1] = '\0';
		strcpy(meterInfo->userName, strTmp);	

		_ReadField(Idx_UserAddr, strTmp);					// 地址
		strTmp[Size_UserAddr - 1] = '\0';
		strcpy(meterInfo->userAddr, strTmp);	

		_ReadField(Idx_MeterReadStatus, strTmp);		// 抄表状态
		strTmp[Size_MeterReadStatus - 1] = '\0';
		strcpy(meterInfo->meterReadStatus, strTmp);	

		_ReadField(Idx_MeterReadDate, strTmp);			// 抄表日期
		strTmp[Size_MeterReadDate - 1] = '\0';
		strcpy(meterInfo->meterReadTime, strTmp);	

		_ReadField(Idx_MeterReadType, strTmp);			// 抄表性质
		strTmp[Size_MeterReadType - 1] = '\0';
		strcpy(meterInfo->meterReadType, strTmp);

		_ReadField(Idx_MrExcepType, strTmp);			// 读数异常类型
		strTmp[Size_MrExcepType - 1] = '\0';
		strcpy(meterInfo->meterExcpType, strTmp);	
	
		_ReadField(Idx_CurrGasVol, strTmp);				// 本次气量
		strTmp[Size_CurrGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currGasVol, strTmp);	

		_ReadField(Idx_CurrRemainGasVol, strTmp);		// 本次剩余气量
		strTmp[Size_CurrRemainGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currRemainGasVol, strTmp);	

		_ReadField(Idx_CurrReadVal, strTmp);			// 本次读数
		strTmp[Size_CurrReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currReadVal, strTmp);	

		_ReadField(Idx_CurrElecReadVal, strTmp);		// 本次电子读数
		strTmp[Size_CurrElecReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currElecReadVal, strTmp);	


		_ReadField(Idx_LastGasVol, strTmp);				// 上次气量
		strTmp[Size_LastGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastGasVol, strTmp);	

		_ReadField(Idx_LastRemainGasVol, strTmp);		// 上次剩余气量
		strTmp[Size_LastRemainGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastRemainGasVol, strTmp);	

		_ReadField(Idx_LastReadVal, strTmp);			// 上次读数
		strTmp[Size_LastReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastReadVal, strTmp);	

		_ReadField(Idx_LastElecReadVal, strTmp);		// 上次电子读数
		strTmp[Size_LastElecReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastElecReadVal, strTmp);


		_ReadField(Idx_LastMrDate, strTmp);				// 上次抄表日期
		strTmp[Size_LastMrDate - 1] = '\0';
		strcpy(meterInfo->lastMtrReadTime, strTmp);	

		_ReadField(Idx_UserBalance, strTmp);			// 用户余额
		strTmp[Size_UserBalance - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->userBalance, strTmp);	

		_ReadField(Idx_AvgGasVol, strTmp);				// 平均气量
		strTmp[Size_AvgGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->avgGasVol, strTmp);	


		_ReadField(Idx_SectNum, strTmp);				// 抄表册编号
		strTmp[Size_SectNum - 1] = '\0';
		strcpy(meterInfo->sectNum, strTmp);	

		_ReadField(Idx_SectName, strTmp);				// 抄表册名称
		strTmp[Size_SectName - 1] = '\0';
		strcpy(meterInfo->sectName, strTmp);	

		_ReadField(Idx_ReaderNum, strTmp);				// 抄表员编号
		strTmp[Size_ReaderNum - 1] = '\0';
		strcpy(meterInfo->readerNum, strTmp);	

		_ReadField(Idx_ReaderName, strTmp);				// 抄表员名称
		strTmp[Size_ReaderName - 1] = '\0';
		strcpy(meterInfo->readerName, strTmp);	

		_ReadField(Idx_OrgNum, strTmp);					// 管理单位编号
		strTmp[Size_OrgNum - 1] = '\0';
		strcpy(meterInfo->orgNum, strTmp);	

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
	double f64Tmp;
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
		dispIdx += sprintf(&dispBuf[dispIdx], "户名: %s\n", meterInfo->userName);
		
		u8Tmp = meterInfo->meterReadStatus[1] - '0';
		// 状态转换：1/2/3 --> 0/1/2 未抄/已抄/失败
		u8Tmp = u8Tmp == 1 ? 0 : (u8Tmp == 2 ? 1 : 2);
		if(meterInfo->meterReadType[1] == '3'){
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表状态: 已抄(录)\n");
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表状态: %s\n", (u8Tmp == 0 ? "未抄" : (u8Tmp == 1 ? "已抄" : "失败")));
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "地址: %s\n", meterInfo->userAddr);
		
		dispIdx += sprintf(&dispBuf[dispIdx], "本次用量: %s\n", meterInfo->currGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "本次剩余: %s\n", meterInfo->currRemainGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "本次读数: %s\n", meterInfo->currReadVal);
		dispIdx += sprintf(&dispBuf[dispIdx], "本次电子读数: \n          %s\n", meterInfo->currElecReadVal);
		
		dispIdx += sprintf(&dispBuf[dispIdx], "本次抄表时间: \n    %s\n", meterInfo->meterReadTime);
		dispIdx += sprintf(&dispBuf[dispIdx], "上次抄表时间: \n    %s\n", meterInfo->lastMtrReadTime);
		
		u8Tmp = meterInfo->meterReadType[1] - '0';
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表性质: %s\n", GetStrMtrReadType(u8Tmp));
		u8Tmp = meterInfo->meterExcpType[1] - '0';
		dispIdx += sprintf(&dispBuf[dispIdx], "异常类型: %s\n", GetStrMtrReadException(u8Tmp));
		dispIdx += sprintf(&dispBuf[dispIdx], "用户余额: %s\n", meterInfo->userBalance);
		dispIdx += sprintf(&dispBuf[dispIdx], "平均用量: %s\n", meterInfo->avgGasVol);

		dispIdx += sprintf(&dispBuf[dispIdx], "上次用量: %s\n", meterInfo->lastGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "上次剩余: %s\n", meterInfo->lastRemainGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "上次读数: %s\n", meterInfo->lastReadVal);
		dispIdx += sprintf(&dispBuf[dispIdx], "上次电子读数: \n          %s\n", meterInfo->lastElecReadVal);
		
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表册编号: \n    %s\n", meterInfo->sectNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表册名称: \n    %s\n", meterInfo->sectName);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表员编号: \n    %s\n", meterInfo->readerNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表员姓名: \n    %s\n", meterInfo->readerName);
		dispIdx += sprintf(&dispBuf[dispIdx], "管理单位编号: \n    %s\n", meterInfo->orgNum);
		
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

		// 中继清空
		for(i = 0; i < RELAY_MAX; i++){				
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
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 9;					// 应答长度 9	
			Args.lastItemLen = i - 1;
			break;

		case 2:
			CurrCmd = WaterCmd_OpenValve;			// " 开阀 "
			/*---------------------------------------------*/
			Args.buf[i++] = 0x05;		// 命令字	05
			ackLen = 0;					// 应答长度 0	
			Args.lastItemLen = i - 1;
			break;

		case 3:
			CurrCmd = WaterCmd_CloseValve;		// " 关阀 ";
			/*---------------------------------------------*/
			Args.buf[i++] = 0x06;		// 命令字	06
			ackLen = 0;					// 应答长度 0	
			Args.lastItemLen = i - 1;
			break;

		case 4:
			CurrCmd = WaterCmd_ClearException;		// " 清异常命令 ";
			/*---------------------------------------------*/
			Args.buf[i++] = 0x03;		// 命令字	03
			ackLen = 0;					// 应答长度 0	
			Args.lastItemLen = i - 1;
			break;

		case 5:										// 手工录入
			while(2){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				currUi = 0;
				sprintf(StrBuf[1], "表号: %s", StrDstAddr);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, StrBuf[1]);
				strcpy(StrBuf[0], meterInfo->currReadVal);
				if(meterInfo->currReadVal[0] == '0'){
					StrBuf[0][0] = '\0';
				}
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "录入读数:", StrBuf[0], 9, 11*8, true);
						pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " ");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "抄表性质: 门抄");
				sprintf(StrBuf[2], "上次读数: %s", MeterInfo.lastReadVal);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, StrBuf[2]);
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(false == StringToDecimal(StrBuf[0], 2, &u8Tmp, &u32Tmp, &u16Tmp)){
					sprintf(StrBuf[0], " ");
					continue;
				}

				f64Tmp = _atof(meterInfo->lastReadVal);
				if(f64Tmp > (u32Tmp / 1.0 + u16Tmp / 100.0)){
					ShowMsg(16, 2*16, "读数异常：由于比上次读数小，将设置为上次读数，异常类型设置为翻转，确定要录入吗？", 500);
					_Printfxy(0, 9*16, "取消            确定", Color_White);
					key = _ReadKey();
					if (key == KEY_CANCEL){
						_GUIRectangleFill(0, 16*2-8, 16*10, 16*9-4, Color_White);
						continue;
					}
				}

				readStatus = 1;
				sprintf(MeterInfo.currReadVal, "%d.%02d", u32Tmp, (u16Tmp & 0xFF));
				SaveMeterReadResult(meterInfo, 3, readStatus);	// 手工录入
				ShowMsg(16, 3*16, "表读数 录入成功!", 1000);
				break;
			}
			continue;		// 显示户表信息

		default: 
			break;
		}

		// 地址填充
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 10 + Addrs.itemCnt * AddrLen;
		timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
		tryCnt = 2;

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
			SaveMeterReadResult(meterInfo, 1, readStatus);	// 掌机抄表
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
