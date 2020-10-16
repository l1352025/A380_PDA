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

//-----------------------	ȫ�ֱ�������
MeterInfoSt MeterInfo;
DistrictListSt Districts;
BuildingListSt Buildings;
MeterListSt Meters;
DbQuerySt DbQuery;
uint8 **MetersStrs = Meters.strs;

//----------------------	���ݿ���Ϣ-��������		-------------------------------------

/*
* �� �����ַ����б��в����ַ���
* �� ����strs		- �ַ����б�
*		strLen		- ÿ���ַ�������
*		strCnt		- �ַ�������
*		dstStr		- ���ҵ��ַ���
*		cmpMaxLen	- �Ƚϵ���󳤶�
* �� �أ�int	- �ҵ����ַ������б��е������� -1 - δ�ҵ��� 0~n - �ҵ�	
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
* �� ������ѯС���б�
* �� ����districts	- С���б�
*		 query		- ���ݿ��ѯ�ṹ
* �� �أ�void
*/
void QueryDistrictList(DistrictListSt *districts, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp1[Size_DbStr];
	char strTmp2[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	recCnt = _Reccount();
	_Go(0);
	districts->cnt = 0;
	districts->idx = 0;
	query->reqMaxCnt = District_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){
		_ReadField(Idx_DistrictNum, strTmp1);		// С�����
		strTmp1[Size_ListStr - 1] = '\0';

		// �ų��ظ��󣬼����б���
		if(-1 == FindStrInList(districts->nums, Size_ListStr, districts->cnt, strTmp1, Size_ListStr)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				sprintf(strTmp1, " С���� ����������� %d !", query->reqMaxCnt);
				ShowMsg(8, 3*16, strTmp1, 3000);
				break;
			}

			_ReadField(Idx_DistrictName, strTmp2);	// С������
			strTmp2[Size_ListStr - 1] = '\0';

			strcpy(districts->nums[districts->cnt], strTmp1);
			strcpy(districts->names[districts->cnt], strTmp2);
			districts->cnt++;
		}
		_Skip(1);	// ��һ�����ݿ��¼
	}
	_Use("");		// �ر����ݿ�

	query->dbCurrIdx = i;
}

/*
* �� ������ѯxxС��-¥���б�
* �� ����buildings	- ¥���б�
*		 query		- ���ݿ��ѯ�ṹ
* �� �أ�void
*/
void QueryBuildingList(BuildingListSt *buildings, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	char strTmp1[Size_DbStr];
	char strTmp2[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	recCnt = _Reccount();
	_Go(0);
	buildings->cnt = 0;
	buildings->idx = 0;
	query->reqMaxCnt = Building_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){

		_ReadField(Idx_DistrictNum, strTmp);		// С����� ����
		strTmp[Size_ListStr - 1] = '\0';
		if(strcmp(buildings->qryDistricNum, strTmp) != 0){
			_Skip(1);	// ��һ�����ݿ��¼
			continue;
		}

		_ReadField(Idx_BuildingNum, strTmp1);		// ¥�����
		strTmp1[Size_ListStr - 1] = '\0';

		// �ų��ظ��󣬼����б���
		if(-1 == FindStrInList(buildings->nums, Size_ListStr, buildings->cnt, strTmp1, Size_ListStr)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				sprintf(strTmp1, " ��С��¥���� ����������� %d !", query->reqMaxCnt);
				ShowMsg(8, 3*16, strTmp1, 3000);
				break;
			}

			_ReadField(Idx_BuildingName, strTmp2);	// ¥������
			strTmp2[Size_ListStr -1] = '\0';

			strcpy(buildings->nums[buildings->cnt], strTmp1);
			strcpy(buildings->names[buildings->cnt], strTmp2);
			buildings->cnt++;
		}
		_Skip(1);	// ��һ�����ݿ��¼
	}
	_Use("");		// �ر����ݿ�

	query->dbCurrIdx = i;
}

/*
* �� ������ѯxxС��-xx¥��-����ͳ�����
* �� ����meters		- �����б���ѯǰ������ meters->(С�����/¥�����/��ѯ����-�������״̬/��ʾ�ֶ�)
*		 query		- ���ݿ��ѯ�ṹ
* �� �أ�void
*/
void QueryMeterList(MeterListSt *meters, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	uint8 state;
	int len;
	bool isOnlyCount = false;

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
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

	switch (meters->selectField)		// �б����ͣ�Ĭ��Ϊ����б�
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
			_ReadField(Idx_DistrictNum, strTmp);		// С����� ����
			strTmp[Size_ListStr - 1] = '\0';
			if(strcmp(meters->qryDistricNum, strTmp) != 0){
				_Skip(1);	// ��һ�����ݿ��¼
				continue;
			}
			else{
				// ����С����
				if(meters->districName[0] == 0x00){
					_ReadField(Idx_DistrictName, strTmp);	
					strTmp[Size_DistrictName - 1] = '\0';
					strcpy(meters->districName, strTmp);
				}
			}
		}
		if(meters->qryBuildingNum != NULL){
			_ReadField(Idx_BuildingNum, strTmp);		// ¥����� ����
			strTmp[Size_ListStr - 1] = '\0';
			if(strcmp(meters->qryBuildingNum, strTmp) != 0){
				_Skip(1);	// ��һ�����ݿ��¼
				continue;
			}
			else{
				// ����¥����
				if(meters->buildingName[0] == 0x00){
					_ReadField(Idx_BuildingName, strTmp);	
					strTmp[Size_BuildingName - 1] = '\0';
					strcpy(meters->buildingName, strTmp);
				}
			}
		}

		meters->meterCnt++;				// ��ǰ������

		if(meters->qryType == QryBy_ReadStatus){
			_ReadField(Idx_MeterReadStatus, strTmp);		
			strTmp[Size_MeterReadStatus - 1] = '\0';
			
			if(strTmp[0] == '0'){
				state = 0;					// δ������
			}
			else if(strTmp[0] == '1'){
				meters->readOkCnt++;		// �ɹ�����
				state = 1;
			}else{
				meters->readNgCnt++;		// ʧ������
				state = 2;			
			}

			if(isOnlyCount){
				_Skip(1);	// ֻ��ͳ��, ��һ�����ݿ��¼
				continue;
			}
			
			// ����״̬ ����  0 - δ��/ʧ�ܣ� 1 - �ѳ�
			if((meters->qryMeterReadStatus == 1 && state != 1)
				|| (meters->qryMeterReadStatus == 0 && state == 1)){
				_Skip(1);	// ��һ�����ݿ��¼
				continue;
			}
		}
		else if(meters->qryType == QryBy_ValveStatus){
			_ReadField(Idx_ValveStatus, strTmp);		
			strTmp[Size_ValveStatus - 1] = '\0';
			
			if(strTmp[0] == '0'){
				meters->valveCloseCnt++;	// �ط�����
				state = 0;				
			}
			else if(strTmp[0] == '1'){
				meters->valveOpenCnt++;		// ��������
				state = 1;
			}else{
				meters->valveUnknownCnt++;	// δ֪����
				state = 2;			
			}

			if(isOnlyCount){
				_Skip(1);	// ֻ��ͳ��, ��һ�����ݿ��¼
				continue;
			}
			
			// ����״̬ ����  0/1/2/3/4 ��/��/δ֪/�ػ�δ֪/����δ֪
			if((meters->qryValveStatus < 3 && meters->qryValveStatus != state)
				|| (meters->qryValveStatus == 3 && state != 0 && state != 2 )
				|| (meters->qryValveStatus == 4 && state != 1 && state != 2 )
				|| (meters->qryValveStatus > 4)){
				_Skip(1);	// ��һ�����ݿ��¼
				continue;
			}
		}

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp, " ��¥����� ����������� %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		_ReadField(meters->selectField, strTmp);	// ��ȡ�ֶΣ����/����/���ƺ�/����/��ַ

		// ��ѡ����ֶ���Ϣ �� ���ݿ����� �����б�
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);
		StringPadRight(meters->strs[meters->cnt], 20, ' ');
		if(meters->qryType == QryBy_ReadStatus){
			meters->strs[meters->cnt][18] = ' ';	
			meters->strs[meters->cnt][19] = (state == 0 ? 'N' : (state == 1 ? 'Y' : 'F'));
		}
		meters->dbIdx[meters->cnt] = (i + 1);	// ���ݿ������� 1 ��ʼ���
		meters->cnt++;

		_Skip(1);	// ��һ�����ݿ��¼
	}
	_Use("");		// �ر����ݿ�

	query->dbCurrIdx = i;
}

/*
* �� �������ؼ��ֲ�ѯ��ߣ��� ���/���� �Ĺؼ��ֲ�ѯ
* �� ����meters		- �����б������� meters->selectField Ϊ Idx_MeterNum / Idx_UserNum
*		 query		- ���ݿ��ѯ�ṹ
* �� �أ�void
*/
void QueryMeterListByKeyword(MeterListSt *meters, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	char strTmp2[Size_DbStr];
	uint8 state;
	int len;

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	recCnt = _Reccount();
	_Go(0);
	meters->cnt = 0;
	meters->idx = 0;
	query->reqMaxCnt = Meter_Max;
	query->resultCnt = 0;
	query->errorCode = 0;

	switch (meters->selectField)		// �б����ͣ�Ĭ��Ϊ����б�
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

		_ReadField(meters->selectField, strTmp);	// ��ȡ�ֶΣ����/����/����/��ַ
		if(strstr(strTmp, meters->qryKeyWord) == NULL){
			_Skip(1);	// ��һ�����ݿ��¼
			continue;
		}
		
		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp2, " �ó�������� ����������� %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp2, 3000);
			break;
		}

		// ��ѡ����ֶ���Ϣ �� ���ݿ����� �����б�
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);

		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';

		if(strTmp[0] == '0'){			// δ������
			state = 0;				
		}
		else if(strTmp[0] == '1'){		// �ɹ�����
			state = 1;
		}else{							// ʧ������
			state = 2;			
		}

		StringPadRight(meters->strs[meters->cnt], 20, ' ');
		meters->strs[meters->cnt][18] = ' ';	
		meters->strs[meters->cnt][19] = (state == 0 ? 'N' : (state == 1 ? 'Y' : 'F'));
		meters->dbIdx[meters->cnt] = (i + 1);	// ���ݿ������� 1 ��ʼ���
		meters->cnt++;

		_Skip(1);	// ��һ�����ݿ��¼
	}
	_Use("");		// �ر����ݿ�

	query->dbCurrIdx = i;
}

/**
 * ����·��
*/
void ShowSettingRoutes(void)
{
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, key;

	_ClearScreen();

	_Printfxy(0, 0, "<<����·��", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);

	while(2){
		(*pUiCnt) = 0;
		uiRowIdx = 2;
		uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
		_Printfxy(0, 9*16, "����            ȷ��", Color_White);
		key = ShowUI(UiList, &currUi);

		if (key == KEY_CANCEL){
			break;
		}
	}
}

/*
* �� ������ʾ xxС��-xx¥��- �Զ�����
* �� ����meters		- ��������б�
* �� �أ�uint8 	- �����˳�ʱ�İ���ֵ�� KEY_CANCEL - ���ؼ� �� KEY_ENTER - ȷ�ϼ�
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
		ShowMsg(1*16, 3*16, "δ��ʧ���б�Ϊ��!", 2000);
		return KEY_CANCEL;
	}

	// �м̼��
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}

	// ��ʼ��
	cnt = 0;	
	meters->readOkCnt = 0;
	meters->readNgCnt = 0;
	lcdCtrl = 0;

	// ��ֹ�Զ�����ʱ�ػ��������Զ��ػ�ʱ��
	shutdownTime = _GetShutDonwTime();
	_SetShutDonwTime(0);		// 20 - 999 ��Ч��0 - �ر��Զ��ػ�

	// �Զ�����
	while(cnt < meters->cnt){

		#if LOG_ON
			sprintf(CurrCmdName, "�������� %d/%d", (cnt + 1), meters->cnt); 
		#endif

		// LCD�����ƿ���
		LcdLightCycleCtrl(&lcdCtrl, 4);
		
		// ��ȡ��ǰ������Ϣ
		meterInfo->dbIdx = meters->dbIdx[cnt];
		QueryMeterInfo(meterInfo, &DbQuery);

		// �Զ�����-������ʾ
		_ClearScreen();
		//PrintfXyMultiLine_VaList(0, 0, "<<�Զ�����  %3d/%-3d ", (cnt + 1), meters->cnt);
		_Printfxy(0, 0, "<<�Զ�����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ַ: %s\n", meterInfo->userAddr);
		PrintfXyMultiLine(0, 1*16 + 8, dispBuf, 7);	

		sprintf(strTmp, "��ǰ����: %d/%d", (cnt + 1), meters->cnt);
		_Printfxy(0, 6*16 + 8, strTmp, Color_White);
		_GUIHLine(0, 7*16 + 8 + 1, 160, Color_Black);
		ShowProgressBar(7*16 + 8 + 3, meters->cnt, cnt + 1);	// ������
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "  <  �ֳ���...   >  ", Color_White);
		
		// �������
		StringCopyFromTail(StrDstAddr, meterInfo->meterNum, AddrLen * 2);
		StringPadLeft(StrDstAddr, AddrLen * 2, '0');
		
		// �����������
		i = 0;	
		CurrCmd = WaterCmd_ReadRealTimeData;	// ��ʵʱ����
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		#ifdef Protocol_6009
			Args.buf[i++] = 0x01;		// ������	01
			ackLen = 21;				// Ӧ�𳤶� 21	
			Args.buf[i++] = 0x00;		// ���ݸ�ʽ 00	
		#else // Protocol_8009
			Args.buf[i++] = 0x01;		// ������	01
			ackLen = 9;					// Ӧ�𳤶� 9	
		#endif
		
		Args.lastItemLen = i - 1;

		#ifdef Protocol_6009
			// ��ַ���
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;
		#else // Protocol_8009
			// ��ַ���
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 3;
		#endif

		// ���͡����ա������ʾ
		cmdResult = CommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);

		if(cmdResult == CmdResult_Cancel){	// ȡ���ֳ�
			break;
		}
		else if(cmdResult == CmdResult_Ok){
			// �ɹ��������������ݿ�
			_Printfxy(0, 9*16, " < ��ǰ����: �ɹ� > ", Color_White);
			meters->readOkCnt++;
			readStatus = 1;
		}
		else{
			// ʧ�ܣ�д�볭��ʱ��
			_Printfxy(0, 9*16, " < ��ǰ����: ʧ�� > ", Color_White);
			meters->readNgCnt++;
			readStatus = 2;
		}
		SaveMeterReadResult(meterInfo, 0, readStatus);		// �ƻ�����
		_Sleep(500);

		// ��ʾ����
		_Printfxy(0, 9*16, "                   ", Color_White);
		_DispTimeSys();	
		_Sleep(500);

		cnt++;
	}

	_OpenLcdBackLight();

	// �ֳ������� ȡ���շ�ʱ�ص�����
	//TranceiverCycleHook = NULL;

	if(cmdResult == CmdResult_Cancel){
		_Printfxy(0, 9*16, "����  <��ȡ��>  ȷ��", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(100);
		_SoundOff();
		#endif
	}
	else{
		_Printfxy(0, 9*16, "����  <�����>  ȷ��", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(200);
		_SoundOff();
		#endif
	}
	

	sprintf(strTmp, "�ɹ�:%d", meters->readOkCnt);
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
* �� ������ʾ xxС��-xx¥��- �Զ��������ط�
* �� ����meters		- ����б�
*		valveCtrl - ���ط�����: 0-�ط���1-����
* �� �أ�uint8 	- �����˳�ʱ�İ���ֵ�� KEY_CANCEL - ���ؼ� �� KEY_ENTER - ȷ�ϼ�
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
			ShowMsg(1*16, 3*16, "�����ط��б�Ϊ��!", 2000);
		}
		else{
			ShowMsg(1*16, 3*16, "���������б�Ϊ��!", 2000);
		}
		return KEY_CANCEL;
	}

	// �м̼��
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}

	// ��ʼ��
	cnt = 0;	
	okCnt = 0;
	lcdCtrl = 0;

	// ��ֹ�Զ�����ʱ�ػ��������Զ��ػ�ʱ��
	shutdownTime = _GetShutDonwTime();
	_SetShutDonwTime(0);		// 20 - 999 ��Ч��0 - �ر��Զ��ػ�

	// �Զ�����
	while(cnt < meters->cnt){

		#if LOG_ON
			sprintf(CurrCmdName, "�������ط�[%d] %d/%d", valveCtrl, (cnt + 1), meters->cnt); 
		#endif

		// LCD�����ƿ���
		LcdLightCycleCtrl(&lcdCtrl, 4);
		
		// ��ȡ��ǰ������Ϣ
		meterInfo->dbIdx = meters->dbIdx[cnt];
		QueryMeterInfo(meterInfo, &DbQuery);

		// �Զ�����-������ʾ
		_ClearScreen();
		if(valveCtrl == 0){
			_Printfxy(0, 0, "<<�����ط�", Color_White);
		}
		else {
			_Printfxy(0, 0, "<<��������", Color_White);
		}
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ַ: %s\n", meterInfo->userAddr);
		PrintfXyMultiLine(0, 1*16 + 8, dispBuf, 7);	

		sprintf(strTmp, "��ǰ����: %d/%d", (cnt + 1), meters->cnt);
		_Printfxy(0, 6*16 + 8, strTmp, Color_White);
		_GUIHLine(0, 7*16 + 8 + 1, 160, Color_Black);
		ShowProgressBar(7*16 + 8 + 3, meters->cnt, cnt + 1);	// ������
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "  < �Զ�������... >  ", Color_White);
		
		// �������
		StringCopyFromTail(StrDstAddr, meterInfo->meterNum, AddrLen * 2);
		StringPadLeft(StrDstAddr, AddrLen * 2, '0');
		
		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������

		switch (valveCtrl){
		case 0:
			CurrCmd = WaterCmd_CloseValve;		// �ط� 
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x00;		// ���ط���ʶ	0 - �ط��� 1 - ����
			#else // Protocol_8009
				Args.buf[i++] = 0x06;		// ������	06
				ackLen = 0;					// Ӧ�𳤶� 0	
			#endif
			Args.lastItemLen = i - 1;
			break;
		case 1:
			CurrCmd = WaterCmd_OpenValve;		// ���� 
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x01;		// ���ط���ʶ	0 - �ط��� 1 - ����
			#else // Protocol_8009
				Args.buf[i++] = 0x05;		// ������	05
				ackLen = 0;					// Ӧ�𳤶� 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		default:
			break;
		}
		
		Args.lastItemLen = i - 1;

		// ��ַ���
		#ifdef Protocol_6009
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#else
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#endif

		// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
		#ifdef Protocol_6009
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;
		#else // Protocol_8009
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 3;
		#endif

		// ���͡����ա������ʾ
		cmdResult = CommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);

		if(cmdResult == CmdResult_Cancel){	// ȡ���ֳ�
			break;
		}
		else if(cmdResult == CmdResult_Ok){
			_Printfxy(0, 9*16, " < ��ǰ����: �ɹ� > ", Color_White);
			okCnt++;
			SaveValveStatus(meterInfo, valveCtrl == 0 ? 0 : 1);
		}
		else{
			_Printfxy(0, 9*16, " < ��ǰ����: ʧ�� > ", Color_White);
			
			if(cmdResult == CmdResult_CrcError || cmdResult == CmdResult_Timeout){
				SaveValveStatus(meterInfo, 2);
			}
		}
		_Sleep(500);

		// ��ʾ����
		_Printfxy(0, 9*16, "                   ", Color_White);
		_DispTimeSys();	
		_Sleep(500);

		cnt++;
	}

	_OpenLcdBackLight();


	if(cmdResult == CmdResult_Cancel){
		_Printfxy(0, 9*16, "����  <��ȡ��>  ȷ��", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(100);
		_SoundOff();
		#endif
	}
	else{
		_Printfxy(0, 9*16, "����  <�����>  ȷ��", Color_White);
		#if RxBeep_On
		_SoundOn();
		_Sleep(200);
		_SoundOff();
		#endif
	}
	
	sprintf(strTmp, "�ɹ�:%d", okCnt);
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
* �� ������ʾxxС��-xx¥��-����ͳ�����
* �� ����meters		- ��������б�
* �� �أ�uint8 	- �����˳�ʱ�İ���ֵ�� KEY_CANCEL - ���ؼ� �� KEY_ENTER - ȷ�ϼ�
*/
uint8 ShowMeterReadCountInfo(MeterListSt *meters)
{
	uint8 key;
	uint16 dispIdx = 0;
	char *dispBuf = &DispBuf;

	_ClearScreen();

	meters->readNotCnt = meters->meterCnt - meters->readOkCnt - meters->readNgCnt;

	_Printfxy(0, 0, "<<����ͳ��", Color_White);
	/*---------------------------------------------*/
	dispIdx += sprintf(&dispBuf[dispIdx], "С��: %s\n", meters->districName);
	dispIdx += sprintf(&dispBuf[dispIdx], "¥��: %s\n", meters->buildingName);
	dispIdx += sprintf(&dispBuf[dispIdx], "�������: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "�ѳ��ɹ�: %d\n", meters->readOkCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "��ǰʧ��: %d\n", meters->readNgCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "��ǰδ��: %d\n", meters->readNotCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "����  < ��� >  ȷ��", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* �� ������ʾxxС��-xx¥��-����״̬ͳ�����
* �� ����meters		- ����б�
* �� �أ�uint8 	- �����˳�ʱ�İ���ֵ�� KEY_CANCEL - ���ؼ� �� KEY_ENTER - ȷ�ϼ�
*/
uint8 ShowValveStatusCountInfo(MeterListSt *meters)
{
	uint8 key;
	uint16 dispIdx = 0;
	char *dispBuf = &DispBuf;

	_ClearScreen();

	meters->valveUnknownCnt = meters->meterCnt - meters->valveOpenCnt - meters->valveCloseCnt;

	_Printfxy(0, 0, "<<����ͳ��", Color_White);
	/*---------------------------------------------*/
	dispIdx += sprintf(&dispBuf[dispIdx], "С��: %s\n", meters->districName);
	dispIdx += sprintf(&dispBuf[dispIdx], "¥��: %s\n", meters->buildingName);
	dispIdx += sprintf(&dispBuf[dispIdx], "�������: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "�ѹط�: %d\n", meters->valveCloseCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "�ѿ���: %d\n", meters->valveOpenCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "����δ֪: %d\n", meters->valveUnknownCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "����  < ��� >  ȷ��", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* �� ������ʾxxС��-xx¥��-�ѳ�/δ���б� �� �ط�/����/����δ֪�б�
* �� ����meters		- ��������б�: ����ǰ�����ò�ѯ���� �� ��Ӧ��״̬
* �� �أ�uint8 	- �����˳�ʱ�İ���ֵ�� KEY_CANCEL - ���ؼ� �� KEY_ENTER - ȷ�ϼ�
*/
uint8 ShowMeterList(MeterListSt *meterReadList)
{
	uint8 key;
	ListBox menuList;			// ��ʾ��ʽ�˵�
	ListBoxEx meterList;		// ����Ϣ-�б�
	char *title = NULL;
	MeterListSt *meters = meterReadList;
	char state;

	if(meters->selectField != Idx_Invalid){
		title = "<<�����ѯ���";
	}
	else{
		// �б���ʾ��ʽ-����
		ListBoxCreate(&menuList, 16*4, 16*3, 12, 4, 5, NULL,
			"��ʾ����", 
			5,
			"1. ���",
			"2. ����",
			"3. ����",
			"4. ���ƺ�",
			"5. ��ַ");

		_Printfxy(0, 9*16, "����            ȷ��", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if(key == KEY_CANCEL){	// ����
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

		//    �ѳ�/δ���б�-���� 
		// �� �ط�/����/����δ֪�б�
		//------------------------------------------------------------
		_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
		QueryMeterList(meters, &DbQuery);	// �ѳ�/δ���б� ��ѯ

		if(meters->qryType == QryBy_ReadStatus){
			title = (meters->qryMeterReadStatus == 1 ? "<<�ѳ��ɹ��б�" : "<<δ��ʧ���б�");
		}
		else{ // meters->qryType == QryBy_ValveStatus
			title = (meters->qryValveStatus == 0 ? "<<�ѹط��б�" : \
				(meters->qryValveStatus == 1 ? "<<�ѿ����б�" : "<<����δ֪�б�"));
		}
	}

	ListBoxCreateEx(&meterList, 0, 0, 20, 7, meters->cnt, NULL,
			&title[2], meters->strs, Size_ListStr, meters->cnt);
	while(1){

		_Printfxy(0, 9*16, "����        ������Ϣ", Color_White);
		key = ShowListBoxEx(&meterList);
		//------------------------------------------------------------
		if(key == KEY_CANCEL){	// ����
			break;
		}
		if(key == KEY_ENTER && meters->cnt == 0){	
			continue;
		}

		while(2){
			// ������Ϣ-����
			//------------------------------------------------------
			meters->idx = meterList.strIdx;
			MeterInfo.dbIdx = meters->dbIdx[meters->idx];
			MeterInfo.strIdx = meters->idx;
			MeterInfo.strCnt = meters->cnt;
			QueryMeterInfo(&MeterInfo, &DbQuery);	// ������Ϣ��ѯ
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
		} // while 2 ������Ϣ

	}// while 1 �ѳ�/δ���б�
	
	return key;
}

/*
* �� �������泭����
* �� ����meterInfo	- ������Ϣ
* 		readType - ����ʽ��0-�ƻ�����1-�ֹ�¼��
* 		readStatus - ����״̬��0-δ����1-�ɹ���2-ʧ��
* �� �أ�void
*/
void SaveMeterReadResult(MeterInfoSt *meterInfo, uint8 readType, uint8 readStatus)
{
	char time[20];

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	_Go(meterInfo->dbIdx);

	_GetDateTime(time, '-', ':');
	sprintf(meterInfo->meterReadType, "%d", readType);
	sprintf(meterInfo->meterReadStatus, "%d", readStatus);
	sprintf(meterInfo->meterReadTime, "%s", time);
	if(readType == 1){
		sprintf(meterInfo->meterStatusHex, "000000");
		sprintf(meterInfo->meterStatusStr, "�ֹ�¼��");
		sprintf(meterInfo->batteryVoltage, "3.3");
		sprintf(meterInfo->signalValue, "99");
	}

	// ���³�����
	_Replace(Idx_MeterReadStatus, meterInfo->meterReadStatus);	// ����״̬ 0 - δ���� 1 - �ɹ��� 2 - ʧ��
	_Replace(Idx_MeterReadType, meterInfo->meterReadType);		// ����ʽ 0 - �ƻ����� �� 1 - �ֹ�¼��
	_Replace(Idx_MeterReadTime, meterInfo->meterReadTime);		// ����ʱ��
	if(readStatus == 1){
		_Replace(Idx_MeterValue, meterInfo->meterValue);			// ����
		_Replace(Idx_MeterStatusHex, meterInfo->meterStatusHex);	// ��״̬ hex
		_Replace(Idx_MeterStatusStr, meterInfo->meterStatusStr);	// ��״̬ str
		_Replace(Idx_BatteryVoltage, meterInfo->batteryVoltage);	// ��ص�ѹ
		_Replace(Idx_SignalValue, meterInfo->signalValue);			// �ź�ǿ��
		_Replace(Idx_ValveStatus, meterInfo->valveStatus);				// �Զ��巧��״̬
	}

	_Use("");			// �ر����ݿ�
}

/*
* �� �������淧��״̬
* �� ����meterInfo	- ������Ϣ
* 		status - ����״̬��0-�ط���1-������2-δ֪
* �� �أ�void
*/
void SaveValveStatus(MeterInfoSt *meterInfo, uint8 status)
{
	uint8 tmp;

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	_Go(meterInfo->dbIdx);
	
	sprintf(meterInfo->valveStatus, "%d", status);

	_Replace(Idx_ValveStatus, meterInfo->valveStatus);				// �Զ��巧��״̬

#if defined Project_6009_RF_HL || defined Project_8009_RF_HL
	// do nothing
#else

	#if defined Protocol_6009
		tmp = (status == 0 ? 2 : (status == 1 ? 1 : 0));			// ����״̬ת�� 0/1/2 --> 2/1/0 	��/��/δ֪
	#elif defined Protocol_8009
		tmp = (status == 0 ? 0x20 : (status == 1 ? 0x40 : 0x00));	// ����״̬ת�� 0/1/2 --> 0x20/0x40/0x00 	��/��/δ֪
	#endif
	sprintf(&meterInfo->meterStatusHex[4], "%02X", tmp);

#endif

	_Use("");			// �ر����ݿ�
}

/*
* �� ������ѯ������Ϣ
* �� ����meterInfo	- ������Ϣ
*		 query		- ���ݿ��ѯ�ṹ
* �� �أ�void
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
	_Use(MeterDocDB);	// �����ݿ�
	recCnt = _Reccount();
	_Go(0);
	query->resultCnt = 0;
	query->errorCode = 0;

	if(meterInfo->dbIdx != Invalid_dbIdx){	// ���ݿ��¼���� �Ƿ���Ч��  ����ЧֵΪ 1 ~ ����¼����
		if(meterInfo->dbIdx > recCnt){		
			meterInfo->dbIdx = Invalid_dbIdx;
		}
	}

	if(meterInfo->dbIdx == Invalid_dbIdx){	// ���ݿ��¼������Чʱ ִ�в�ѯ

		#if UseSearchFunc

		if(meterInfo->qryMeterNum != NULL
			&& _LocateEx(Idx_MeterNum, '=', meterInfo->qryMeterNum, 1, recCnt, 0) > 0){ 
			// ����Ų�ѯ
			meterInfo->dbIdx = _Recno();
		}
		else if(meterInfo->qryUserNum != NULL
			&& _LocateEx(Idx_UserNum, '=', meterInfo->qryUserNum, 1, recCnt, 0) > 0){ 
			// �����Ų�ѯ
			meterInfo->dbIdx = _Recno();
		}
		else if(meterInfo->qryRoomNum != NULL
			&& _LocateEx(Idx_UserRoomNum, '=', meterInfo->qryRoomNum, 1, recCnt, 0) > 0){ 
			// �����ƺŲ�ѯ
			meterInfo->dbIdx = _Recno();
		}

		#else

		for(i = 0; i < recCnt; i++){

			if(meterInfo->qryMeterNum != NULL){
				_ReadField(Idx_MeterNum, strTmp);		// ����Ų�ѯ
				strTmp[Size_MeterNum - 1] = '\0';
				if(strcmp(meterInfo->qryMeterNum, strTmp) == 0){
					meterInfo->dbIdx = i + 1;
					break;
				}
			}
			else if(meterInfo->qryUserNum != NULL){
				_ReadField(Idx_UserNum, strTmp);		// �����Ų�ѯ
				strTmp[Size_UserNum - 1] = '\0';
				if(strcmp(meterInfo->qryUserNum, strTmp) == 0){
					meterInfo->dbIdx = i + 1;
					break;
				}
			}
			else if(meterInfo->qryRoomNum != NULL){
				_ReadField(Idx_UserRoomNum, strTmp);	// �����ƺŲ�ѯ
				strTmp[Size_RoomNum - 1] = '\0';
				if(strcmp(meterInfo->qryRoomNum, strTmp) == 0){
					meterInfo->dbIdx = i + 1;
					break;
				}
			}

			_Skip(1);	// ��һ�����ݿ��¼
		}
		#endif
	}

	if(meterInfo->dbIdx != Invalid_dbIdx){	// ���ݿ��¼������Чʱ ��ȡ��¼

		_Go(meterInfo->dbIdx);
		query->resultCnt = 1;

		_ReadField(Idx_MeterNum, strTmp);					// ���
		strTmp[Size_MeterNum - 1] = '\0';
		strcpy(meterInfo->meterNum, strTmp);

		_ReadField(Idx_UserNum, strTmp);					// ����
		strTmp[Size_UserNum - 1] = '\0';
		strcpy(meterInfo->userNum, strTmp);	

		_ReadField(Idx_UserRoomNum, strTmp);				// ���ƺ�
		strTmp[Size_RoomNum - 1] = '\0';
		StringFixGbkStrEndError(strTmp);
		strcpy(meterInfo->roomNum, strTmp);	

		_ReadField(Idx_MeterReadStatus, strTmp);			// ����״̬
		strTmp[Size_MeterReadStatus - 1] = '\0';
		strcpy(meterInfo->meterReadStatus, strTmp);	

		_ReadField(Idx_UserName, strTmp);					// ����
		strTmp[Size_UserName - 1] = '\0';
		strcpy(meterInfo->userName, strTmp);	

		_ReadField(Idx_UserMobileNum, strTmp);				// �ֻ���
		strTmp[Size_UserMobileNum - 1] = '\0';
		strcpy(meterInfo->mobileNum, strTmp);	

		_ReadField(Idx_UserAddr, strTmp);					// ��ַ
		strTmp[Size_UserAddr - 1] = '\0';
		StringFixGbkStrEndError(strTmp);
		strcpy(meterInfo->userAddr, strTmp);	

		_ReadField(Idx_MeterReadType, strTmp);			// ����ʽ
		strTmp[Size_MeterReadType - 1] = '\0';
		strcpy(meterInfo->meterReadType, strTmp);	

		_ReadField(Idx_MeterReadTime, strTmp);			// ����ʱ��
		strTmp[Size_MeterReadTime - 1] = '\0';
		strcpy(meterInfo->meterReadTime, strTmp);	

		_ReadField(Idx_MeterValue, strTmp);				// �����
		strTmp[Size_MeterValue - 1] = '\0';
		strcpy(meterInfo->meterValue, strTmp);	

		_ReadField(Idx_MeterStatusStr, strTmp);			// ��״̬
		strTmp[Size_MeterStatusStr - 1] = '\0';
		strcpy(meterInfo->meterStatusStr, strTmp);	

		_ReadField(Idx_BatteryVoltage, strTmp);			// ��ص�ѹ
		strTmp[Size_BatteryVoltage - 1] = '\0';
		strcpy(meterInfo->batteryVoltage, strTmp);	

		_ReadField(Idx_SignalValue, strTmp);			// �ź�ǿ��
		strTmp[Size_SignalValue - 1] = '\0';
		strcpy(meterInfo->signalValue, strTmp);	
	}

	_Use("");		// �ر����ݿ�

	query->dbCurrIdx = meterInfo->dbIdx;
}

/*
* �� ������ʾ������Ϣ
* �� ����meterInfo	- ������Ϣ
* �� �أ�uint8 	- �����˳�ʱ�İ���ֵ�� KEY_CANCEL - ���ؼ� �� KEY_ENTER - ȷ�ϼ�
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

		// ������Ϣ-����
		//-----------------------------------------------------
		PrintfXyMultiLine_VaList(0, 0, "<<������Ϣ  %3d/%-3d ", meterInfo->strIdx + 1, meterInfo->strCnt);
		dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "���ƺ�: %s\n", meterInfo->roomNum);
		if(meterInfo->meterReadType[0] == '1'){
			dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: �ɹ�(¼)\n");
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s\n", (readStatus == 0 ? "δ��" : (readStatus == 1 ? "�ɹ�" : "ʧ��")));
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userName);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ֻ�: %s\n", meterInfo->mobileNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ַ: %s\n", meterInfo->userAddr);
		u8Tmp = meterInfo->meterReadType[0] - '0';
		dispIdx += sprintf(&dispBuf[dispIdx], "����ʽ: %s\n", (u8Tmp == 0 ? "�ƻ�����" : (u8Tmp == 1 ? "�ֹ�¼��" : " ")));
		dispIdx += sprintf(&dispBuf[dispIdx], "����ʱ��: \n %s\n", meterInfo->meterReadTime);
		dispIdx += sprintf(&dispBuf[dispIdx], "�����: %s\n", meterInfo->meterValue);
		dispIdx += sprintf(&dispBuf[dispIdx], "��״̬: %s\n", meterInfo->meterStatusStr);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ص�ѹ: %s\n", meterInfo->batteryVoltage);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ź�ǿ��: %s", meterInfo->signalValue);
		//----------------------------------------------
		_Printfxy(0, 9*16, "����        ��������", Color_White);
		key = ShowScrollStrEx(dispBuf,  7);
		//----------------------------------------------
		if(key == KEY_CANCEL || key == KEY_LEFT || key == KEY_RIGHT){	// ����
			break;
		}

		// �������
		StringCopyFromTail(StrDstAddr, meterInfo->meterNum, AddrLen * 2);
		StringPadLeft(StrDstAddr, AddrLen * 2, '0');

		for(i = 0; i < RELAY_MAX; i++){				// �м����
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
			}
		}

		// ��������-����
		ListBoxCreate(&menuList, 3*16, 2*16, 14, 5, 5, NULL, 
			"��������", 
			5, 
			"1. ����",
			"2. ����",
			"3. �ط�",
			"4. ���쳣",
			"5. �ֹ�¼��");
		//---------------------
		key = ShowListBox(&menuList);
		if(key == KEY_CANCEL){	// ȡ��ִ�����������ʾ������Ϣ
			continue;
		}

		// ��������-����
		_ClearScreen();
		sprintf(CurrCmdName, menuList.str[menuList.strIdx]);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		//--------------------------------------
		sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
		_Printfxy(0, 0, TmpBuf, Color_White);
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);	
		//------------------------------------

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������

		switch(menuList.strIdx + 1){

		case 1:
			CurrCmd = WaterCmd_ReadRealTimeData;		// "��ȡ�û�����"
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x01;		// ������	01
				ackLen = 21;				// Ӧ�𳤶� 21	
				Args.buf[i++] = 0x00;		// ���ݸ�ʽ 00	
			#else // Protocol_8009
				Args.buf[i++] = 0x01;		// ������	01
				ackLen = 9;					// Ӧ�𳤶� 9	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 2:
			CurrCmd = WaterCmd_OpenValve;			// " ���� "
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x01;		// ���ط���ʶ	0 - �ط��� 1 - ����
			#else // Protocol_8009
				Args.buf[i++] = 0x05;		// ������	05
				ackLen = 0;					// Ӧ�𳤶� 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 3:
			CurrCmd = WaterCmd_CloseValve;		// " �ط� ";
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x00;		// ���ط���ʶ	0 - �ط��� 1 - ����
			#else // Protocol_8009
				Args.buf[i++] = 0x06;		// ������	06
				ackLen = 0;					// Ӧ�𳤶� 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 4:
			CurrCmd = WaterCmd_ClearException;		// " ���쳣���� ";
			/*---------------------------------------------*/
			#ifdef Protocol_6009
				Args.buf[i++] = 0x05;		// ������	05
				ackLen = 1;					// Ӧ�𳤶� 1	
				Args.buf[i++] = 0x00;		// ����ѡ�� 00	
			#else // Protocol_8009
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 0;					// Ӧ�𳤶� 0	
			#endif
			Args.lastItemLen = i - 1;
			break;

		case 5:										// �ֹ�¼��
			while(2){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				currUi = 0;
				sprintf(StrBuf[1], "���: %s", StrDstAddr);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, StrBuf[1]);
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "¼�����:", StrBuf[0], 9, 11*8, true);
						pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " ");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��״̬: �ֹ�¼��");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ص�ѹ: 3.3 v");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ź�ǿ��: 99");
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
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
				SaveMeterReadResult(meterInfo, 1, readStatus);	// �ֹ�¼��
				ShowMsg(16, 3*16, "����� ¼��ɹ�!", 1000);
				break;
			}
			continue;		// ��ʾ������Ϣ

		default: 
			break;
		}

		// ��ַ���
		#ifdef Protocol_6009
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#else
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#endif

		// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
		#ifdef Protocol_6009
			ackLen += 15 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 2;
		#else // Protocol_8009
			ackLen += 10 + Addrs.itemCnt * AddrLen;
			timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
			tryCnt = 2;
		#endif

		// ���͡����ա������ʾ
		cmdResult = ProtolCommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		if(cmdResult == CmdResult_Cancel){	// ����ȡ��
			readStatus = 0;
		}
		else if(cmdResult == CmdResult_Ok){
			// �ɹ��������������ݿ�
			readStatus = 1;
		}
		else{
			// ʧ�ܣ�д�볭��ʱ��
			readStatus = 2;
		}

		if(CurrCmd == WaterCmd_ReadRealTimeData && readStatus != 0){
			SaveMeterReadResult(meterInfo, 0, readStatus);	// �ƻ�����
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

	}// while 1  ������Ϣ
	

	return key;
}

/*
* �� �����޸�DBF�ļ���ͷ����¼������Ϣ
* �� ����void
* �� �أ�uint32 ��¼������0 - ��¼����Ϊ0 �� DBF�ļ�������/��ʧ��
*/
uint32 FixDbfRecCnt(void)
{
	uint32 recCnt = 0;
	int fp;

	if(_Access(MeterDocDB, 0) < 0){
		return recCnt;
	}
	
	_Select(1);
	if(_Use(MeterDocDB) == 0){	// �����ݿ�
		return recCnt;
	}
	recCnt = _Reccount();
	_Use("");					// �ر����ݿ�

	_Lseek(fp, 4, 0);
	_Fwrite(&recCnt, 4, fp);
	_Fclose(fp);

	return recCnt;
}

// �������� - 6009/8009ͨ�ð�
#if defined Project_6009_RF || defined Project_6009_RF_TN || defined Project_8009_RF || defined Project_8009_RF_TN
void MainFuncBatchMeterReading(void)
{
	uint8 key;
	ListBox menuList, menuList_2, menuList_3;
	ListBoxEx XqList, LdList;				// С��/¥���б�
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

	recCnt = FixDbfRecCnt();	// �޸���¼����
	MeterInfo.dbIdx = Invalid_dbIdx;  // ��յ�ǰ�����ݿ���������ֹ������д��
	
	if(recCnt == 0){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  ��ǰ����Ϊ�գ�\n �������س�������\n �ٽ�����������");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// �˵�
	//------------------------------------------------------------
	#ifdef UseFunc_ReSetDistrictAndBuildingNo
		ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
			"<<��������",
			6,
			"1. ��¥������",
			"2. ��ճ�����",
			"3. ���ó���ʱ��",
			"4. �����ѯ",
			"5. ����ͳ��",
			"6. С��¥�����±��"
		);
	#else
		ListBoxCreate(&menuList, 0, 0, 20, 7, 5, NULL,
			"<<��������",
			5,
			"1. ��¥������",
			"2. ��ճ�����",
			"3. ���ó���ʱ��",
			"4. �����ѯ",
			"5. ����ͳ��"
		);
	#endif
	while(1){

		_Printfxy(0, 9*16, "����            ȷ��", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// ����
			break;
		}
		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		switch(menuList.strIdx + 1){
		case 1:		// ��¥������
			#ifdef UseFunc_ReSetDistrictAndBuildingNo
			if(!IsReSetNo()){
				_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
				PrintfXyMultiLine_VaList(0, 3*16, "  С��¥��δ��ţ�\n  �������±�ţ�");
				_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
				_Sleep(2500);
				break;
			}
			#endif

			// С���б�-����
			//------------------------------------------------------------
			_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
			QueryDistrictList(&Districts, &DbQuery);
			ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
				"<<С��ѡ��", Districts.names, Size_ListStr, Districts.cnt);
			while(2){
				
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBoxEx(&XqList);
				//------------------------------------------------------------
				if (key == KEY_CANCEL){	// ����
					break;
				}

				// xxС��-¥���б�-����
				//------------------------------------------------------------
				_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
				Buildings.qryDistricNum = Districts.nums[XqList.strIdx];
				QueryBuildingList(&Buildings, &DbQuery);
				ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
					"<<¥��ѡ��", Buildings.names, Size_ListStr, Buildings.cnt);
				//------------------------------------------------------------
				while(3){
	
					_Printfxy(0, 9*16, "����            ȷ��", Color_White);
					key = ShowListBoxEx(&LdList);
					//------------------------------------------------------------
					if(key == KEY_CANCEL){	// ����
						break;
					}

					// ���·��
					StrRelayAddr[0][0] = 0x00;
					StrRelayAddr[1][0] = 0x00;
					StrRelayAddr[2][0] = 0x00;

					// ¥������-����
					//------------------------------------------------------------
					Meters.qryDistricNum = Districts.nums[XqList.strIdx];
					Meters.qryBuildingNum = Buildings.nums[LdList.strIdx];
				#ifdef Protocol_6009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 6, NULL,
						"<<¥������", 
						6,
						"1. �Զ�����",
						"2. �ѳ��ɹ��б�",
						"3. δ��ʧ���б�",
						"4. ����ͳ��",
						"5. ��ճ�����",
						"6. ���ó���ʱ��");
				#else // Protocol_8009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 7, NULL,
						"<<¥������", 
						7,
						"1. �Զ�����",
						"2. �ѳ��ɹ��б�",
						"3. δ��ʧ���б�",
						"4. ����ͳ��",
						"5. ��ճ�����",
						"6. ���ó���ʱ��",
						"7. ����·��");
				#endif
					while(4){

						_Printfxy(0, 9*16, "����            ȷ��", Color_White);
						key = ShowListBox(&menuList_2);
						//------------------------------------------------------------
						if(key == KEY_CANCEL){	// ����
							break;
						}

						Meters.qryType = QryBy_ReadStatus;		// ��ѯ���ͣ�����״̬

						switch (menuList_2.strIdx + 1){
						case 1:		// �Զ�����
							Meters.selectField = Idx_MeterNum;
							Meters.qryMeterReadStatus = 0;
							_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowAutoMeterReading(&Meters);
							break;

						case 2:		// �ѳ��б�
							Meters.selectField = Idx_Invalid;
							Meters.qryMeterReadStatus = 1;
							key = ShowMeterList(&Meters);
							break;
						case 3:		// δ��/ʧ���б�
							Meters.selectField = Idx_Invalid;
							Meters.qryMeterReadStatus = 0;
							key = ShowMeterList(&Meters);
							break;

						case 4:		// ����ͳ��
							Meters.selectField = Idx_Invalid;
							_Printfxy(0, 9*16, "    <  ͳ����  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowMeterReadCountInfo(&Meters);
							break;

						case 5:		// ��ճ�����
							//-------------------------------------------------------
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " ȷ��Ҫ���        ", Color_White);
							_Printfxy(0, 5*16, " ��ǰ¥����������?", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							key = _ReadKey();
							//-------------------------------------------------------
							if(key != KEY_ENTER){
								break;
							}
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, "  ��ǰ¥��         ", Color_White);
							_Printfxy(0, 5*16, "  �����������... ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							//------------------------------------------------------------
							_Select(1);
							_Use(MeterDocDB);	// �����ݿ�
							_Go(0);
							for(i = 0; i < recCnt; i++){
								_ReadField(Idx_DistrictNum, strTmp);	// С����� ����
								strTmp[Size_DistrictNum - 1] = '\0';
								if(strcmp(Meters.qryDistricNum, strTmp) != 0){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_ReadField(Idx_BuildingNum, strTmp);	// ¥����� ����
								strTmp[Size_BuildingNum - 1] = '\0';
								if(strcmp(Meters.qryBuildingNum, strTmp) != 0){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
								strTmp[Size_MeterReadStatus - 1] = '\0';
								if(strTmp[0] == '0'){
									_Skip(1);	// ��һ�����ݿ��¼
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
							_Use("");		// �ر����ݿ�
							//------------------------------------------------------------
							_Printfxy(0, 5*16, "  ������������! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							break;

						case 6:		// ���ó���ʱ��
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
									
									LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ϵͳʱ��:");
									TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
									TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
									TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
									TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
									TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
									TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
									LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ȷ��������ʱ������");
									LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "Ϊ��ǰϵͳʱ����?");
									
									key = ShowUI(UiList, &currUi);
									if (key == KEY_CANCEL){
										break;
									}
									isUiFinish = true;
								}

								// ʱ����ЧֵУ��
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

							_Printfxy(8, 5*16, "��ǰ¥��           ", Color_White);
							_Printfxy(8, 6*16, "����ʱ��������... ", Color_White);
							//------------------------------------------------------------
							_Select(1);
							_Use(MeterDocDB);	// �����ݿ�
							_Go(0);
							for(i = 0; i < recCnt; i++){
								_ReadField(Idx_DistrictNum, strTmp);	// С����� ����
								strTmp[Size_DistrictNum - 1] = '\0';
								if(strcmp(Meters.qryDistricNum, strTmp) != 0){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_ReadField(Idx_BuildingNum, strTmp);	// ¥����� ����
								strTmp[Size_BuildingNum - 1] = '\0';
								if(strcmp(Meters.qryBuildingNum, strTmp) != 0){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
								strTmp[Size_MeterReadStatus - 1] = '\0';
								if(strTmp[0] == '0'){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_Replace(Idx_MeterReadTime, time);
								_Skip(1);
							}
							_Use("");		// �ر����ݿ�
							//------------------------------------------------------------
							_Printfxy(8, 6*16, "����ʱ��������ɣ� ", Color_White);
							_Sleep(2500);
							break;
						
						#if defined Project_8009_RF
						case 7:		// ����·��
							ShowSettingRoutes();
							break;
						#endif

						default:
							break;
						}

					}// while 4 ¥������
				}// while 3 ¥���б�
			}// while 2 С���б�
			break;

		case 2:		// ��ճ�����
			//-------------------------------------------------------
			_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  ȷ��Ҫ���       ", Color_White);
			_Printfxy(0, 5*16, "  ���г�������?   ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
			key = _ReadKey();
			//-------------------------------------------------------
			if(key != KEY_ENTER){
				break;
			}
			_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  ��ճ�������... ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			//-------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// �����ݿ�
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[0] == '0'){
					_Skip(1);	// ��һ�����ݿ��¼
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
				_Skip(1);		// ��һ�����ݿ��¼
			}
			_Use("");			// �ر����ݿ�
			//-------------------------------------------------------
			_Printfxy(0, 4*16, "  ��ճ�������ɣ�", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			_Sleep(2500);
			break;

		case 3:		// ���ó���ʱ��
			//-------------------------------------------------------
			_GUIRectangleFill(0, 3*16 - 8, 160, 7*16 + 8, Color_White);
			_GUIRectangle(0, 3*16 - 8, 160, 7*16 + 8, Color_Black);
			while(true){
				if(false == isUiFinish){
					(*pUiCnt) = 0;
					uiRowIdx = 3;

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
					
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ϵͳʱ��:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ȷ��������ʱ������");
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "Ϊ��ǰϵͳʱ����?");
					
					key = ShowUI(UiList, &currUi);
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;
				
				}
				// ʱ����ЧֵУ��
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
			_Printfxy(8, 5*16, "���е���          ", Color_White);
			_Printfxy(8, 6*16, "����ʱ��������... ", Color_White);
			//------------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// �����ݿ�
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[0] == '0'){
					_Skip(1);	// ��һ�����ݿ��¼
					continue;
				}
				_Replace(Idx_MeterReadTime, time);
				_Skip(1);
			}
			_Use("");		// �ر����ݿ�
			//------------------------------------------------------------
			_Printfxy(8, 6*16, "����ʱ��������ɣ�", Color_White);
			_Sleep(2500);
			break;

		case 4:		// �����ѯ
			// �����ѯ-����
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 7, 3, NULL,
				"<<�����ѯ", 
				3,
				"1. ����Ų�ѯ",
				"2. �����Ų�ѯ",
				"3. �����ƺŲ�ѯ");
			while(2){

				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBox(&menuList_2);
				//------------------------------------------------------------
				if(key == KEY_CANCEL){	// ����
					break;
				}

				// �����ѯ-�������
				//------------------------------------------------------------
				_ClearScreen();
				sprintf(&TmpBuf[0], "<<%s", &(menuList_2.str[menuList_2.strIdx][3]));
				_Printfxy(0, 0*16, &TmpBuf[0], Color_White);
				_GUIHLine(0, 1*16 + 4, 160, Color_Black);

				Meters.qryType = QryBy_KeyWord;		// ��ѯ���ͣ��ؼ���
				Meters.qryKeyWord = StrBuf[0];
				switch (menuList_2.strIdx + 1){
				case 1: ptr = "������: "; Meters.selectField = Idx_MeterNum; break;
				case 2: ptr = "���뻧��: "; Meters.selectField = Idx_UserNum; break;
				case 3: ptr = "�������ƺ�: "; Meters.selectField = Idx_UserRoomNum; break;
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
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				//------------------------------------------------------------
				if(KEY_CANCEL ==  _GetStr(&inputSt)){
					continue;
				}
				StringTrimStart(StrBuf[0],  ' ');
				StringTrimEnd(StrBuf[0],  ' ');

				// �ؼ��� ��ѯ
				_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
				QueryMeterListByKeyword(&Meters, &DbQuery);	
				if(DbQuery.resultCnt > 0){
					ShowMeterList(&Meters);
				}
				else{
					_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
					_Printfxy(0, 4*16, " ��ѯʧ�ܣ�δ�ҵ� ", Color_White);
					_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
					_Sleep(2500);
				}
				
			} // while 2 �����ѯ
			break;

		case 5:		// ����ͳ��
			// ����ͳ��-����
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 3, 3, NULL,
				"<<����ͳ��", 
				3, 
				"1. С��ѡ��>",
				"2. ¥��ѡ��>",
				"3. ͳ��");
			//------------------------------------------------------------
			sprintf(qryStrXq, "ȫ��");
			sprintf(qryStrLd, "ȫ��");
			qryTypeXq = 0;		// 0 - ȫ���� 1 - ѡ��ĳ��
			qryTypeLd = 0;		// 0 - ȫ���� 1 - ѡ��ĳ��
			qryIndexXq = 0;		// ��ǰѡ���С�� ���б�������
			qryIndexLd = 0;		// ��ǰѡ���¥�� ���б�������
			while(2){

				_ClearScreen();
				// ����ͳ��-����
				//---------------------------------------------------------------------
				dispBuf = &DispBuf;
				dispIdx = 0;
				dispIdx += sprintf(&dispBuf[dispIdx], "С��: %s\n", qryStrXq);
				dispIdx += sprintf(&dispBuf[dispIdx], "¥��: %s", qryStrLd);
				PrintfXyMultiLine(0, 5*16, dispBuf, 7);
				//----------------------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBox(&menuList_2);
				//----------------------------------------------------------------------
				if(key == KEY_CANCEL){	// ����
					break;
				}
				
				if(menuList_2.strIdx == 0){		// С��ѡ��
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 2*16, 12, 2, 2, NULL, 
					"С��ѡ��", 2, "ȫ��", "ѡ��ĳ��");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// δѡ�񣬷���
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeXq = 0;
						sprintf(qryStrXq, "ȫ��");
					}
					else{
						// С���б�-����
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
						QueryDistrictList(&Districts, &DbQuery);
						ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
							"<<С��ѡ��", Districts.names, Size_ListStr, Districts.cnt);
						_Printfxy(0, 9*16, "����            ȷ��", Color_White);
						key = ShowListBoxEx(&XqList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// δѡ���б���
							continue;
						}

						qryTypeXq = 1;
						qryIndexXq = XqList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// �����ݿ�
						recCnt = _Reccount();
						if(_LocateEx(Idx_DistrictNum, '=', Districts.nums[qryIndexXq], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_DistrictName, qryStrXq);	// ����С����
						}
						_Use("");			// �ر����ݿ�
					}

				}
				else if(menuList_2.strIdx == 1){	// ¥��ѡ��
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 3*16, 12, 2, 2, NULL, 
					"¥��ѡ��", 2, "ȫ��", "ѡ��ĳ��");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// δѡ�񣬷���
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeLd = 0;
						sprintf(qryStrLd, "ȫ��");
					}
					else{

						if(qryTypeXq == 0){
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " ѡ��ĳ��¥��ʱ ", Color_White);
							_Printfxy(0, 5*16, " ����ѡ������С��! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							continue;
						}

						// xxС��-¥���б�-����
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
						Buildings.qryDistricNum = Districts.nums[qryIndexXq];
						QueryBuildingList(&Buildings, &DbQuery);
						ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
							"<<¥��ѡ��", Buildings.names, Size_ListStr, Buildings.cnt);

						_Printfxy(0, 9*16, "����            ȷ��", Color_White);
						key = ShowListBoxEx(&LdList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// δѡ���б���
							continue;
						}

						qryTypeLd = 1;
						qryIndexLd = LdList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// �����ݿ�
						recCnt = _Reccount();
						if(_LocateEx(Idx_BuildingNum, '=', Buildings.nums[qryIndexLd], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_BuildingName, qryStrLd);	// ����¥����
						}
						_Use("");			// �ر����ݿ�
					}
					
				}
				else {	// if(menuList_2.strIdx == 3){	// ͳ��
					Meters.qryDistricNum = (qryTypeXq == 0 ? NULL : Districts.nums[qryIndexXq]);
					Meters.qryBuildingNum = (qryTypeLd == 0 ? NULL : Buildings.nums[qryIndexLd]);
					Meters.selectField = Idx_Invalid;
					_Printfxy(0, 9*16, "    <  ͳ����  >    ", Color_White);
					QueryMeterList(&Meters, &DbQuery);
					if(Meters.districName[0] == 0x00){
						strcpy(Meters.districName, "ȫ��");
					}
					if(Meters.buildingName[0] == 0x00){
						strcpy(Meters.buildingName, "ȫ��");
					}
					key = ShowMeterReadCountInfo(&Meters);
				}
			}
			break;

		#ifdef UseFunc_ReSetDistrictAndBuildingNo
		case 6:		// С��¥�����±��
			ReSetDistrictAndBuildingNo();
			break;
		#endif

		default: 
			break;
		}

	} // while 1 ��������

	FixDbfRecCnt();	// �޸���¼����
	MeterInfo.dbIdx = Invalid_dbIdx;  // ��յ�ǰ�����ݿ���������ֹ������д��
}
#endif

// �������ط� - 6009/8009ͨ�ð�
#ifdef UseFunc_BatchOpenCloseValve
void BatchOpenCloseValve(void)
{
	uint8 key;
	ListBox menuList, menuList_2, menuList_3;
	ListBoxEx XqList, LdList;				// С��/¥���б�
	_GuiInputBoxStru inputSt;
	uint8 *ptr;
	uint16 dispIdx, i;
	char *dispBuf = &DispBuf, *strTmp = &TmpBuf[0];
	char *qryStrXq = &TmpBuf[400], *qryStrLd = &TmpBuf[600];
	uint8 qryTypeXq, qryTypeLd;
	uint16 qryIndexXq, qryIndexLd;
	uint32 recCnt;

	recCnt = FixDbfRecCnt();	// �޸���¼����
	MeterInfo.dbIdx = Invalid_dbIdx;  // ��յ�ǰ�����ݿ���������ֹ������д��
	
	if(recCnt == 0){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  ��ǰ����Ϊ�գ�\n �������س�������\n �ٽ�����������");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// �˵�
	//------------------------------------------------------------
	ListBoxCreate(&menuList, 0, 0, 20, 7, 4, NULL,
		"<<�������ط�",
		4,
		"1. ��¥�����ط�",
		"2. ��շ���״̬",
		"3. �����ѯ",
		"4. ����״̬ͳ��"
	);
	while(1){

		_Printfxy(0, 9*16, "����            ȷ��", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// ����
			break;
		}
		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);

		switch(menuList.strIdx + 1){
		case 1:		// ��¥�����ط�

			// С���б�-����
			//------------------------------------------------------------
			_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
			QueryDistrictList(&Districts, &DbQuery);
			ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
				"<<С��ѡ��", Districts.names, Size_ListStr, Districts.cnt);
			while(2){
				
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBoxEx(&XqList);
				//------------------------------------------------------------
				if (key == KEY_CANCEL){	// ����
					break;
				}

				// xxС��-¥���б�-����
				//------------------------------------------------------------
				_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
				Buildings.qryDistricNum = Districts.nums[XqList.strIdx];
				QueryBuildingList(&Buildings, &DbQuery);
				ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
					"<<¥��ѡ��", Buildings.names, Size_ListStr, Buildings.cnt);
				//------------------------------------------------------------
				while(3){
	
					_Printfxy(0, 9*16, "����            ȷ��", Color_White);
					key = ShowListBoxEx(&LdList);
					//------------------------------------------------------------
					if(key == KEY_CANCEL){	// ����
						break;
					}

					// ���·��
					StrRelayAddr[0][0] = 0x00;
					StrRelayAddr[1][0] = 0x00;
					StrRelayAddr[2][0] = 0x00;

					// ¥������-����
					//------------------------------------------------------------
					Meters.qryDistricNum = Districts.nums[XqList.strIdx];
					Meters.qryBuildingNum = Buildings.nums[LdList.strIdx];
				#ifdef Protocol_6009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 7, NULL,
						"<<��¥�����ط�", 
						7,
						"1. �Զ���������",
						"2. �Զ������ط�",
						"3. ����δ֪�б�",
						"4. �ѿ����б�",
						"5. �ѹط��б�",
						"6. ����״̬ͳ��",
						"7. ��շ���״̬");
				#else // Protocol_8009
					ListBoxCreate(&menuList_2, 0, 0, 20, 7, 8, NULL,
						"<<��¥�����ط�", 
						8,
						"1. �Զ���������",
						"2. �Զ������ط�",
						"3. ����δ֪�б�",
						"4. �ѿ����б�",
						"5. �ѹط��б�",
						"6. ����״̬ͳ��",
						"7. ��շ���״̬",
						"8. ����·��");
				#endif
					while(4){

						_Printfxy(0, 9*16, "����            ȷ��", Color_White);
						key = ShowListBox(&menuList_2);
						//------------------------------------------------------------
						if(key == KEY_CANCEL){	// ����
							break;
						}

						Meters.qryType = QryBy_ValveStatus;		// ��ѯ���ͣ�����״̬

						switch (menuList_2.strIdx + 1){
						case 1:		// �Զ���������
							Meters.selectField = Idx_MeterNum;
							Meters.qryValveStatus = 3;
							_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowAutoOpenCloseValve(&Meters, 1);
							break;
						case 2:		// �Զ������ط�
							Meters.selectField = Idx_MeterNum;
							Meters.qryValveStatus = 4;
							_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowAutoOpenCloseValve(&Meters, 0);
							break;
						case 3:		// ����δ֪�б�
							Meters.selectField = Idx_Invalid;
							Meters.qryValveStatus = 2;
							key = ShowMeterList(&Meters);
							break;
						case 4:		// �ѿ����б�
							Meters.selectField = Idx_Invalid;
							Meters.qryValveStatus = 1;
							key = ShowMeterList(&Meters);
							break;
						case 5:		// �ѹط��б�
							Meters.selectField = Idx_Invalid;
							Meters.qryValveStatus = 0;
							key = ShowMeterList(&Meters);
							break;

						case 6:		// ����״̬ͳ��
							Meters.selectField = Idx_Invalid;
							_Printfxy(0, 9*16, "    <  ͳ����  >    ", Color_White);
							QueryMeterList(&Meters, &DbQuery);
							key = ShowValveStatusCountInfo(&Meters);
							break;

						case 7:		// ��շ���״̬
							//-------------------------------------------------------
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " ȷ��Ҫ���        ", Color_White);
							_Printfxy(0, 5*16, " ��ǰ¥������״̬��?", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							key = _ReadKey();
							//-------------------------------------------------------
							if(key != KEY_ENTER){
								break;
							}
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, "  ��ǰ¥��         ", Color_White);
							_Printfxy(0, 5*16, "  ����״̬�����... ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							//------------------------------------------------------------
							_Select(1);
							_Use(MeterDocDB);	// �����ݿ�
							_Go(0);
							for(i = 0; i < recCnt; i++){
								_ReadField(Idx_DistrictNum, strTmp);	// С����� ����
								strTmp[Size_DistrictNum - 1] = '\0';
								if(strcmp(Meters.qryDistricNum, strTmp) != 0){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_ReadField(Idx_BuildingNum, strTmp);	// ¥����� ����
								strTmp[Size_BuildingNum - 1] = '\0';
								if(strcmp(Meters.qryBuildingNum, strTmp) != 0){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_ReadField(Idx_ValveStatus, strTmp);	// ����״̬ ����
								strTmp[Size_ValveStatus - 1] = '\0';
								if(strTmp[0] != '0' && strTmp[0] != '1'){
									_Skip(1);	// ��һ�����ݿ��¼
									continue;
								}

								_Replace(Idx_ValveStatus, "");
								_Skip(1);
							}
							_Use("");		// �ر����ݿ�
							//------------------------------------------------------------
							_Printfxy(0, 5*16, "  ����״̬������! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							break;

						
						
						#if defined Project_8009_RF
						case 8:		// ����·��
							ShowSettingRoutes();
							break;
						#endif

						default:
							break;
						}

					}// while 4 ¥�����ط�
				}// while 3 ¥���б�
			}// while 2 С���б�
			break;

		case 2:		// ��շ���״̬
			//-------------------------------------------------------
			_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  ȷ��Ҫ���       ", Color_White);
			_Printfxy(0, 5*16, "  ���з���״̬��?   ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
			key = _ReadKey();
			//-------------------------------------------------------
			if(key != KEY_ENTER){
				break;
			}
			_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  ��շ���״̬��... ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			//-------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// �����ݿ�
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_ValveStatus, strTmp);	// ����״̬ ����
				strTmp[Size_ValveStatus - 1] = '\0';
				if(strTmp[0] != '0' && strTmp[0] != '1'){
					_Skip(1);	// ��һ�����ݿ��¼
					continue;
				}
				_Replace(Idx_ValveStatus, "");
				_Skip(1);		// ��һ�����ݿ��¼
			}
			_Use("");			// �ر����ݿ�
			//-------------------------------------------------------
			_Printfxy(0, 4*16, "  ��շ���״̬��ɣ�", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			_Sleep(2500);
			break;

		case 3:		// �����ѯ
			// �����ѯ-����
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 7, 3, NULL,
				"<<�����ѯ", 
				3,
				"1. ����Ų�ѯ",
				"2. �����Ų�ѯ",
				"3. �����ƺŲ�ѯ");
			while(2){

				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBox(&menuList_2);
				//------------------------------------------------------------
				if(key == KEY_CANCEL){	// ����
					break;
				}

				// �����ѯ-�������
				//------------------------------------------------------------
				_ClearScreen();
				sprintf(&TmpBuf[0], "<<%s", &(menuList_2.str[menuList_2.strIdx][3]));
				_Printfxy(0, 0*16, &TmpBuf[0], Color_White);
				_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
				Meters.qryType = QryBy_KeyWord;		// ��ѯ���ͣ��ؼ���
				Meters.qryKeyWord = StrBuf[0];
				switch (menuList_2.strIdx + 1){
				case 1: ptr = "������: "; Meters.selectField = Idx_MeterNum; break;
				case 2: ptr = "���뻧��: "; Meters.selectField = Idx_UserNum; break;
				case 3: ptr = "�������ƺ�: "; Meters.selectField = Idx_UserRoomNum; break;
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
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				//------------------------------------------------------------
				if(KEY_CANCEL ==  _GetStr(&inputSt)){
					continue;
				}
				StringTrimStart(StrBuf[0],  ' ');
				StringTrimEnd(StrBuf[0],  ' ');

				// �ؼ��� ��ѯ
				_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
				QueryMeterListByKeyword(&Meters, &DbQuery);	
				if(DbQuery.resultCnt > 0){
					ShowMeterList(&Meters);
				}
				else{
					_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
					_Printfxy(0, 4*16, " ��ѯʧ�ܣ�δ�ҵ� ", Color_White);
					_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
					_Sleep(2500);
				}
				
			} // while 2 �����ѯ
			break;

		case 4:		// ����״̬ͳ��
			// ����
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 3, 3, NULL,
				"<<����ͳ��", 
				3, 
				"1. С��ѡ��>",
				"2. ¥��ѡ��>",
				"3. ͳ��");
			//------------------------------------------------------------
			sprintf(qryStrXq, "ȫ��");
			sprintf(qryStrLd, "ȫ��");
			qryTypeXq = 0;		// 0 - ȫ���� 1 - ѡ��ĳ��
			qryTypeLd = 0;		// 0 - ȫ���� 1 - ѡ��ĳ��
			qryIndexXq = 0;		// ��ǰѡ���С�� ���б�������
			qryIndexLd = 0;		// ��ǰѡ���¥�� ���б�������
			while(2){

				_ClearScreen();
				// ����ͳ��-����
				//---------------------------------------------------------------------
				dispBuf = &DispBuf;
				dispIdx = 0;
				dispIdx += sprintf(&dispBuf[dispIdx], "С��: %s\n", qryStrXq);
				dispIdx += sprintf(&dispBuf[dispIdx], "¥��: %s", qryStrLd);
				PrintfXyMultiLine(0, 5*16, dispBuf, 7);
				//----------------------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBox(&menuList_2);
				//----------------------------------------------------------------------
				if(key == KEY_CANCEL){	// ����
					break;
				}
				
				if(menuList_2.strIdx == 0){		// С��ѡ��
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 2*16, 12, 2, 2, NULL, 
					"С��ѡ��", 2, "ȫ��", "ѡ��ĳ��");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// δѡ�񣬷���
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeXq = 0;
						sprintf(qryStrXq, "ȫ��");
					}
					else{
						// С���б�-����
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
						QueryDistrictList(&Districts, &DbQuery);
						ListBoxCreateEx(&XqList, 0, 0, 20, 7, Districts.cnt, NULL,
							"<<С��ѡ��", Districts.names, Size_ListStr, Districts.cnt);
						_Printfxy(0, 9*16, "����            ȷ��", Color_White);
						key = ShowListBoxEx(&XqList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// δѡ���б���
							continue;
						}

						qryTypeXq = 1;
						qryIndexXq = XqList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// �����ݿ�
						recCnt = _Reccount();
						if(_LocateEx(Idx_DistrictNum, '=', Districts.nums[qryIndexXq], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_DistrictName, qryStrXq);	// ����С����
						}
						_Use("");			// �ر����ݿ�
					}

				}
				else if(menuList_2.strIdx == 1){	// ¥��ѡ��
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 3*16, 12, 2, 2, NULL, 
					"¥��ѡ��", 2, "ȫ��", "ѡ��ĳ��");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// δѡ�񣬷���
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeLd = 0;
						sprintf(qryStrLd, "ȫ��");
					}
					else{

						if(qryTypeXq == 0){
							_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
							_Printfxy(0, 4*16, " ѡ��ĳ��¥��ʱ ", Color_White);
							_Printfxy(0, 5*16, " ����ѡ������С��! ", Color_White);
							_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
							_Sleep(2500);
							continue;
						}

						// xxС��-¥���б�-����
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
						Buildings.qryDistricNum = Districts.nums[qryIndexXq];
						QueryBuildingList(&Buildings, &DbQuery);
						ListBoxCreateEx(&LdList, 0, 0, 20, 7, Buildings.cnt, NULL,
							"<<¥��ѡ��", Buildings.names, Size_ListStr, Buildings.cnt);

						_Printfxy(0, 9*16, "����            ȷ��", Color_White);
						key = ShowListBoxEx(&LdList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// δѡ���б���
							continue;
						}

						qryTypeLd = 1;
						qryIndexLd = LdList.strIdx;

						_Select(1);
						_Use(MeterDocDB);	// �����ݿ�
						recCnt = _Reccount();
						if(_LocateEx(Idx_BuildingNum, '=', Buildings.nums[qryIndexLd], 1, recCnt, 0) > 0){ 
							_ReadField(Idx_BuildingName, qryStrLd);	// ����¥����
						}
						_Use("");			// �ر����ݿ�
					}
					
				}
				else {	// if(menuList_2.strIdx == 3){	// ͳ��
					Meters.qryType = QryBy_ValveStatus;		// ��ѯ���ͣ�����״̬
					Meters.qryDistricNum = (qryTypeXq == 0 ? NULL : Districts.nums[qryIndexXq]);
					Meters.qryBuildingNum = (qryTypeLd == 0 ? NULL : Buildings.nums[qryIndexLd]);
					Meters.selectField = Idx_Invalid;
					_Printfxy(0, 9*16, "    <  ͳ����  >    ", Color_White);
					QueryMeterList(&Meters, &DbQuery);
					if(Meters.districName[0] == 0x00){
						strcpy(Meters.districName, "ȫ��");
					}
					if(Meters.buildingName[0] == 0x00){
						strcpy(Meters.buildingName, "ȫ��");
					}
					key = ShowValveStatusCountInfo(&Meters);
				}
			}
			break;

		default: 
			break;
		}

	} // while 1 ��������

	FixDbfRecCnt();	// �޸���¼����
	MeterInfo.dbIdx = Invalid_dbIdx;  // ��յ�ǰ�����ݿ���������ֹ������д��
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
	_Use(MeterDocDB);	// �����ݿ�
	recCnt = _Reccount();
	
	if(recCnt == 0) return false;

	_Go(recCnt - 3);

	for(i = 0; i < 3; i++){
		
		xqName = &TmpBuf[1000];
		ldName = &TmpBuf[1300];

		_ReadField(Idx_DistrictName, xqName);	// С������
		xqName[Size_ListStr - 1] = '\0';

		// ��ȡ�û���ַ , ����¥������	
		_ReadField(Idx_UserAddr, ldName);	

		// ¥������ȥ��ǰ��С������
		ptr = &xqName[0];
		while(*ptr != '\0'){
			if(*ptr != *ldName){
				break;
			}
			ptr++;
			ldName++;
		}
		// ��ȡ���20�ַ�����ȥ���ָ�����-����������
		ldName[Size_ListStr - 1] = '\0';
		ptr = &ldName[0];
		while(*ptr != '\0'){
			if(*ptr == '-'){
				*ptr = '\0';
				break;
			}
			ptr++;
		}
		// ȥ������޷���ʾ��һ�ֽ�
		StringFixGbkStrEndError(ldName);

		// ¥������ �Ƿ��� �ӵ�ַ��ȡ���ģ�
		_ReadField(Idx_BuildingName, strTmp);	
		strTmp[Size_ListStr - 1] = '\0';
		if(strcmp(strTmp, ldName) != 0){
			ret = false;
			break;
		}

		// ¥����� �Ƿ�С�� ���ֵ
		_ReadField(Idx_BuildingNum, strTmp);	
		strTmp[Size_ListStr - 1] = '\0';
		isOK = StringToInt(strTmp, &buildNo);
		if(!isOK || buildNo < 0 || buildNo > AllBuildings_Max){
			ret = false;
			break;
		}

		_Skip(1);	// ��һ�����ݿ��¼
	}

	_Use("");					// �ر����ݿ�

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
		PrintfXyMultiLine_VaList(0, 3*16, "  ��ǰ����Ϊ�գ�\n �������س�������\n �ٽ�����������");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// ������ʾ
	_ClearScreen();
	_Printfxy(0, 0, "<<С��¥�����±��", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	PrintfXyMultiLine_VaList(0, 3*16, "  ȷ��Ҫ��С����¥�����±����");
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "ȡ��            ȷ��", Color_White);

	if(_ReadKey() != KEY_ENTER){
		return;
	}

	if(IsReSetNo()){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  С��¥���ѱ�Ź��ˣ�\n  �Ƿ����±�ţ�");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Printfxy(0, 9*16, "ȡ��            ȷ��", Color_White);
		if(_ReadKey() != KEY_ENTER){
			return;
		}
	}

	_GUIRectangleFill(0, 2*16, 10*16, 5*16, Color_White);
	_Printfxy(0, 9*16, "  <  ����� ...  >  ", Color_White);

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	recCnt = _Reccount();
	_Go(0);

	xqNo = 0;
	ldNo = 0;
	districts->cnt = 0;
	buildingsCnt = 0;
	key = KEY_NOHOOK;
	lcdCtrl = 0;

	// ��ֹ�Զ�����ʱ�ػ��������Զ��ػ�ʱ��
	shutdownTime = _GetShutDonwTime();
	_SetShutDonwTime(0);		// 20 - 999 ��Ч��0 - �ر��Զ��ػ�

	for(i = 0; i < recCnt; i++){

		xqName = &TmpBuf[1000];
		ldName = &TmpBuf[1300];

		// LCD�����ƿ���
		LcdLightCycleCtrl(&lcdCtrl, 30);

		_ReadField(Idx_DistrictName, xqName);	// С������
		xqName[Size_ListStr - 1] = '\0';

		// С������: �ų��ظ��󣬼����б���
		xqNo = FindStrInList(districts->names, Size_ListStr, districts->cnt, xqName, Size_ListStr);
		if(-1 == xqNo){
			if(districts->cnt >= District_Max){
				sprintf(strTmp, " С���� ����������� %d !", District_Max);
				ShowMsg(8, 2*16, strTmp, 3000);
				break;
			}
			xqNo = districts->cnt;
			sprintf(strTmp,"%d",xqNo);
			strcpy(districts->nums[districts->cnt], strTmp);
			strcpy(districts->names[districts->cnt], xqName);
			districts->cnt++;
		}
		_Replace(Idx_DistrictNum, districts->nums[xqNo]);	// ����С�����

		// ��ȡ�û���ַ , ��������¥������	
		_ReadField(Idx_UserAddr, ldName);	

		// ¥������ȥ��ǰ��С������
		ptr = &xqName[0];
		while(*ptr != '\0'){
			if(*ptr != *ldName){
				break;
			}
			ptr++;
			ldName++;
		}
		// ��ȡ���20�ַ�����ȥ���ָ�����-����������
		ldName[Size_ListStr - 1] = '\0';
		ptr = &ldName[0];
		while(*ptr != '\0'){
			if(*ptr == '-'){
				*ptr = '\0';
				break;
			}
			ptr++;
		}
		// ȥ������޷���ʾ��һ�ֽ�
		StringFixGbkStrEndError(ldName);
		
		_Replace(Idx_BuildingName, ldName);	


		// ¥������: �ų��ظ��󣬼����б���
		for(j = 0; j < buildingsCnt; j++){
			if(strcmp(buildings[j].name, ldName) == 0){
				break;
			}
		}
		ldNo = j;
		if(buildingsCnt == ldNo){
			if(buildingsCnt >= AllBuildings_Max){
				sprintf(strTmp, " ����С��¥���� ����������� %d !", AllBuildings_Max);
				ShowMsg(8, 2*16, strTmp, 3000);
				break;
			}
			//sprintf(strTmp,"%d",ldNo);
			sprintf(buildings[buildingsCnt].num, "%d", ldNo);
			strcpy(buildings[buildingsCnt].name, ldName);
			buildingsCnt++;
		}
		_Replace(Idx_BuildingNum, buildings[ldNo].num);		// ����¥�����

		_Skip(1);	// ��һ�����ݿ��¼

		PrintfXyMultiLine_VaList(0, 4*16, "С��������%d", districts->cnt);
		PrintfXyMultiLine_VaList(0, 5*16, "¥��������%d", buildingsCnt);
		sprintf(strTmp, "��ǰ����: %d/%d",  i + 1, recCnt);
		_Printfxy(0, 6*16 + 8, strTmp, Color_White);
		_GUIHLine(0, 7*16 + 8 + 1, 160, Color_Black);
		ShowProgressBar(7*16 + 8 + 3, recCnt, i + 1);	// ������

		// ��������
		key = _GetKeyExt();
		CycleInvoke_OpenLcdLight_WhenKeyPress(key);

		if(key == KEY_CANCEL){
			break;
		}
	}
	_Use("");		// �ر����ݿ�

	_OpenLcdBackLight();
	_SetShutDonwTime(shutdownTime);	

	if(key == KEY_CANCEL){
		_Printfxy(0, 9*16, "����  <��ȡ��>  ȷ��", Color_White);
	}
	else{
		_Printfxy(0, 9*16, "����  < ��� >  ȷ��", Color_White);
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