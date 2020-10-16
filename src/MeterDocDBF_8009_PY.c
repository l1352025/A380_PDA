
#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"
#include "MeterDocDBF_8009_PY.h"
#include "ProtoHandle_8009_PY.h"


//-----------------------	ȫ�ֱ�������	---------------------------------------
SectListSt Sects;
MeterListSt Meters;
MeterInfoSt MeterInfo;
DbQuerySt DbQuery;
uint8 **MetersStrs = Meters.strs;


//-----------------------	�ڲ�����	---------------------------------------
static char * GetStrMtrReadException(uint8 code)
{
	char * ptr = NULL;

	switch (code)
	{
	case MrExcp_Normal: ptr = "����"; break;
	case MrExcp_Flip: ptr = "��ת"; break;
	case MrExcp_Reverse: ptr = "��ת"; break;
	case MrExcp_FlipReverse: ptr = "��ת�ҵ�ת"; break;
	case MrExcp_DataExcp: ptr = "�����쳣"; break;
	default: ptr = "δ֪����"; break;
	}

	return ptr;
}

static char * GetStrMtrReadType(uint8 code)
{
	char * ptr = NULL;

	switch (code)
	{
	case MrType_BySee: ptr = "����"; break;
	case MrType_ByEstimate: ptr = "����"; break;
	case MrType_ByDoor: ptr = "�ų�"; break;
	case MrType_UserReport: ptr = "�û��Ա�"; break;
	case MrType_EmptyHourse: ptr = "�շ�"; break;
	case MrType_ZeroGasVol: ptr = "������"; break;
	default: ptr = "δ֪"; break;
	}

	return ptr;
}

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
* �� ������ѯ������б�
* �� ����Sects	- ������б�
*		 query		- ���ݿ��ѯ�ṹ
* �� �أ�void
*/
void QuerySectList(SectListSt *Sects, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp1[Size_DbStr];
	//char strTmp2[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	recCnt = _Reccount();
	_Go(0);
	Sects->cnt = 0;
	Sects->idx = 0;
	query->reqMaxCnt = Sect_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){
		_ReadField(Idx_SectNum, strTmp1);		// �������
		strTmp1[Size_ListStr - 1] = '\0';

		// �ų��ظ��󣬼����б���
		if(-1 == FindStrInList(Sects->nums, Size_ListStr, Sects->cnt, strTmp1, Size_ListStr)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				sprintf(strTmp1, " ������� ����������� %d !", query->reqMaxCnt);
				ShowMsg(8, 3*16, strTmp1, 3000);
				break;
			}

			//_ReadField(Idx_SectName, strTmp2);	// ���������
			//strTmp2[Size_ListStr - 1] = '\0';

			strcpy(Sects->nums[Sects->cnt], strTmp1);
			//strcpy(Sects->names[Sects->cnt], strTmp2);
			Sects->cnt++;
		}
		_Skip(1);	// ��һ�����ݿ��¼
	}
	_Use("");		// �ر����ݿ�

	query->dbCurrIdx = i;
}

/*
* �� ������ѯxx��������б� / ͳ�Ƴ������
* �� ����meters		- �����б������� meters->selectField�� ͳ��ʱ����Ϊ Idx_Invalid����ѯʱ��Ϊ����
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
	meters->sectName[0] = 0x00;
	meters->meterCnt = 0;
	meters->readOkCnt = 0;
	meters->readNgCnt = 0;
	query->reqMaxCnt = Meter_Max;
	query->resultCnt = 0;
	query->errorCode = 0;

	switch (meters->selectField)		// �б����ͣ�Ĭ��Ϊ����б�
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
			_ReadField(Idx_SectNum, strTmp);		// ������� ����
			strTmp[Size_SectNum - 1] = '\0';
			if(strcmp(meters->qrySectNum, strTmp) != 0){
				_Skip(1);	// ��һ�����ݿ��¼
				continue;
			}
			else{
				// ���泭�����
				// if(meters->sectName[0] == 0x00){
				// 	_ReadField(Idx_SectName, strTmp);	
				// 	strTmp[Size_SectName - 1] = '\0';
				// 	strcpy(meters->sectName, strTmp);
				// }
			}
		}
		
		meters->meterCnt++;				// ��ǰ������
		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';

		// ״̬ת�� 1/2/3 --> 0/1/2
		if(strTmp[0] == '0' && strTmp[1] == '1'){
			state = 0;				// δ������
		}
		else if(strTmp[0] == '0' && strTmp[1] == '2'){
			meters->readOkCnt++;		// �ɹ�����
			state = 1;
		}else{
			meters->readNgCnt++;		// ʧ������
			state = 2;			
		}

		
		// ����״̬ ����  ��0�� - δ��/ʧ�ܣ� ��1�� - �ѳ�
		if((meters->qryMeterReadStatus == 1 && state != 1)
			|| (meters->qryMeterReadStatus == 0 && state == 1)){
			_Skip(1);	// ��һ�����ݿ��¼
			continue;
		}


		if(isOnlyCount){		// δѡ���ֶΣ��򲻹����б�, ֻ��ͳ��
			_Skip(1);	// ��һ�����ݿ��¼
			continue;
		}

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp, " ��¥����� ����������� %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		_ReadField(meters->selectField, strTmp);	// ��ȡ�ֶΣ����/����/����/��ַ
		
		// ��ѡ����ֶ���Ϣ �� ���ݿ����� �����б�
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);
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
	char state;
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
		// ��ѡ����ֶ���Ϣ �� ���ݿ����� �����б�
		len = StringCopyFromTail(meters->strs[meters->cnt], strTmp, 18);

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp, " �ó�������� ����������� %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';

		// ״̬ת�� 1/2/3 --> 0/1/2
		if(strTmp[0] == '0' && strTmp[1] == '1'){		// δ������
			state = '0';				
		}
		else if(strTmp[0] == '0' && strTmp[1] == '2'){	// �ɹ�����
			state = '1';
		}else{											// ʧ������
			state = '2';			
		}

		StringPadRight(meters->strs[meters->cnt], 20, ' ');
		meters->strs[meters->cnt][18] = ' ';	
		meters->strs[meters->cnt][19] = (state == '0' ? 'N' : (state == '1' ? 'Y' : 'F'));
		meters->dbIdx[meters->cnt] = (i + 1);	// ���ݿ������� 1 ��ʼ���
		meters->cnt++;

		_Skip(1);	// ��һ�����ݿ��¼
	}
	_Use("");		// �ر����ݿ�

	query->dbCurrIdx = i;
}

/*
* �� ������ʾ xx�����-xx¥��- �Զ�����
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
		ShowMsg(1*16, 3*16, "δ��/ʧ���б�Ϊ��!", 2000);
		return KEY_CANCEL;
	}

	// �м̼��
	for(i = 0; i < RELAY_MAX; i++){				
		StrRelayAddr[i][0] = 0x00;	// ��ʹ��
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

		_Printfxy(0, 0, "<<�Զ�����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userName);
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
		Args.buf[i++] = 0x01;		// ������	01
		ackLen = 9;					// Ӧ�𳤶� 9	
		Args.lastItemLen = i - 1;

		// ��ַ���
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);


		// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
		ackLen += 10 + Addrs.itemCnt * AddrLen;
		timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
		tryCnt = 3;

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
		
		SaveMeterReadResult(meterInfo, 1, readStatus);		// �ƻ�����
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
* �� ������ʾxx�����-xx¥��-����ͳ�����
* �� ����meters		- ��������б�
* �� �أ�uint8 	- �����˳�ʱ�İ���ֵ�� KEY_CANCEL - ���ؼ� �� KEY_ENTER - ȷ�ϼ�
*/
uint8 ShowMeterReadCountInfo(MeterListSt *meters)
{
	uint8 key;
	uint16 dispIdx = 0;
	char *dispBuf = &DispBuf;

	_ClearScreen();

	_Printfxy(0, 0, "<<����ͳ��", Color_White);
	/*---------------------------------------------*/
	dispIdx += sprintf(&dispBuf[dispIdx], "�����: %s\n \n", meters->sectNum);

	dispIdx += sprintf(&dispBuf[dispIdx], "�������: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "�ѳ��ɹ�: %d\n", meters->readOkCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "��ǰʧ��: %d\n", meters->readNgCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "��ǰδ��: %d\n", meters->meterCnt - meters->readOkCnt - meters->readNgCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "����  < ��� >  ȷ��", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* �� ������ʾxx�����-xx¥��-�ѳ�/δ���б�
* �� ����meters		- ��������б�: ����ǰ�����ó���״̬ qryMeterReadStatus
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
		title = (meters->qryMeterReadStatus == 1 ? "<<�ѳ��ɹ��б�" : "<<δ��ʧ���б�");
		ListBoxCreate(&menuList, 16*4, 16*3, 12, 4, 4, NULL,
			"��ʾ����", 
			4,
			"1. ���",
			"2. ����",
			"3. ����",
			"4. ��ַ");

		_Printfxy(0, 9*16, "����            ȷ��", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if(key == KEY_CANCEL){	// ����
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

		_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
		QueryMeterList(meters, &DbQuery);	// �ѳ�/δ���б� ��ѯ
	}

	// �ѳ�/δ���б�/�ؼ��ֲ�ѯ����б�-����
	//------------------------------------------------------------
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
			state = MeterInfo.meterReadStatus[1];	
			state = state == '1' ? '0' : (state == '2' ? '1' : '2');	// ״̬ת�� 1/2/3 --> 0/1/2
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
		} // while 2 ������Ϣ

	}// while 1 �ѳ�/δ���б�
	
	return key;
}

/*
* �� �������泭����
* �� ����meterInfo	- ������Ϣ
* 		readType - �������� 1 - ������2 - ������3-�ų���4-�Ա���5-�շ���6-������
* 		readStatus - ����״̬��0-δ����1-�ѳ�, 2-ʧ��
* �� �أ�void
*/
void SaveMeterReadResult(MeterInfoSt *meterInfo, uint8 readType, uint8 readStatus)
{
	char time[20];
	double curr, last;
	uint8 isFlip = false;
	char strTmp[Size_DbStr];

	_Select(1);
	_Use(MeterDocDB);	// �����ݿ�
	_Go(meterInfo->dbIdx);

	readStatus = (readStatus == 0 ? 1 : readStatus == 1 ? 2 : 3);	// ����״̬ת�� 0/1/2 --> 1/2/3

	_GetDate(time, NULL);
	sprintf(meterInfo->meterReadType, "0%d", readType);
	sprintf(meterInfo->meterReadStatus, "0%d", readStatus);
	sprintf(meterInfo->meterReadTime, "%s", time);
	sprintf(meterInfo->meterExcpType, "01");

	// ���³�����
	_Replace(Idx_MeterReadStatus, meterInfo->meterReadStatus);	// ����״̬ 
	_Replace(Idx_MeterReadType, meterInfo->meterReadType);		// �������� 
	_Replace(Idx_MeterReadDate, meterInfo->meterReadTime);		// ��������
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
		_Replace(Idx_CurrReadVal, meterInfo->currReadVal);				// ���ζ���

		//sprintf(meterInfo->currGasVol, "%0.2lf", curr - last);
		_DoubleToStr(meterInfo->currGasVol, curr - last, 2);
		_Replace(Idx_CurrGasVol, meterInfo->currGasVol);				// ��������

		sprintf(meterInfo->meterExcpType, "%s", (isFlip ? "02" : "01"));
		_Replace(Idx_MrExcepType, meterInfo->meterExcpType);			// �����쳣����

		//sprintf(meterInfo->currReadVal, "%0.2lf", curr / 1.0);
		_DoubleToStr(meterInfo->currElecReadVal, curr / 1.0, 2);
		_Replace(Idx_CurrElecReadVal, meterInfo->currElecReadVal);		// ���ε��Ӷ���

		curr = curr - last;
		_ReadField(Idx_LastRemainGasVol, strTmp);
		last = _atof(strTmp);
		curr = curr > last ? 0 : last - curr;
		//sprintf(meterInfo->currReadVal, "%0.2lf", curr);
		_DoubleToStr(meterInfo->currRemainGasVol, curr, 2);
		_Replace(Idx_CurrRemainGasVol, meterInfo->currRemainGasVol);	// ����ʣ������
	}

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
	char *strTmp = &TmpBuf[0];

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
			else if(meterInfo->qryUserAddr != NULL){
				_ReadField(Idx_UserAddr, strTmp);	// ����ַ��ѯ
				strTmp[Size_UserAddr - 1] = '\0';
				if(strcmp(meterInfo->qryUserAddr, strTmp) == 0){
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
		strTmp[Size_MeterNum - 1] = 0x00;	
		strcpy(meterInfo->meterNum, strTmp);

		_ReadField(Idx_UserNum, strTmp);					// ����
		strTmp[Size_UserNum - 1] = '\0';
		strcpy(meterInfo->userNum, strTmp);	

		_ReadField(Idx_UserName, strTmp);					// ����
		strTmp[Size_UserName - 1] = '\0';
		strcpy(meterInfo->userName, strTmp);	

		_ReadField(Idx_UserAddr, strTmp);					// ��ַ
		strTmp[Size_UserAddr - 1] = '\0';
		strcpy(meterInfo->userAddr, strTmp);	

		_ReadField(Idx_MeterReadStatus, strTmp);		// ����״̬
		strTmp[Size_MeterReadStatus - 1] = '\0';
		strcpy(meterInfo->meterReadStatus, strTmp);	

		_ReadField(Idx_MeterReadDate, strTmp);			// ��������
		strTmp[Size_MeterReadDate - 1] = '\0';
		strcpy(meterInfo->meterReadTime, strTmp);	

		_ReadField(Idx_MeterReadType, strTmp);			// ��������
		strTmp[Size_MeterReadType - 1] = '\0';
		strcpy(meterInfo->meterReadType, strTmp);

		_ReadField(Idx_MrExcepType, strTmp);			// �����쳣����
		strTmp[Size_MrExcepType - 1] = '\0';
		strcpy(meterInfo->meterExcpType, strTmp);	
	
		_ReadField(Idx_CurrGasVol, strTmp);				// ��������
		strTmp[Size_CurrGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currGasVol, strTmp);	

		_ReadField(Idx_CurrRemainGasVol, strTmp);		// ����ʣ������
		strTmp[Size_CurrRemainGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currRemainGasVol, strTmp);	

		_ReadField(Idx_CurrReadVal, strTmp);			// ���ζ���
		strTmp[Size_CurrReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currReadVal, strTmp);	

		_ReadField(Idx_CurrElecReadVal, strTmp);		// ���ε��Ӷ���
		strTmp[Size_CurrElecReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->currElecReadVal, strTmp);	


		_ReadField(Idx_LastGasVol, strTmp);				// �ϴ�����
		strTmp[Size_LastGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastGasVol, strTmp);	

		_ReadField(Idx_LastRemainGasVol, strTmp);		// �ϴ�ʣ������
		strTmp[Size_LastRemainGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastRemainGasVol, strTmp);	

		_ReadField(Idx_LastReadVal, strTmp);			// �ϴζ���
		strTmp[Size_LastReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastReadVal, strTmp);	

		_ReadField(Idx_LastElecReadVal, strTmp);		// �ϴε��Ӷ���
		strTmp[Size_LastElecReadVal - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->lastElecReadVal, strTmp);


		_ReadField(Idx_LastMrDate, strTmp);				// �ϴγ�������
		strTmp[Size_LastMrDate - 1] = '\0';
		strcpy(meterInfo->lastMtrReadTime, strTmp);	

		_ReadField(Idx_UserBalance, strTmp);			// �û����
		strTmp[Size_UserBalance - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->userBalance, strTmp);	

		_ReadField(Idx_AvgGasVol, strTmp);				// ƽ������
		strTmp[Size_AvgGasVol - 1] = '\0';
		DoubleStrSetFracCnt(strTmp, 2);
		strcpy(meterInfo->avgGasVol, strTmp);	


		_ReadField(Idx_SectNum, strTmp);				// �������
		strTmp[Size_SectNum - 1] = '\0';
		strcpy(meterInfo->sectNum, strTmp);	

		_ReadField(Idx_SectName, strTmp);				// ���������
		strTmp[Size_SectName - 1] = '\0';
		strcpy(meterInfo->sectName, strTmp);	

		_ReadField(Idx_ReaderNum, strTmp);				// ����Ա���
		strTmp[Size_ReaderNum - 1] = '\0';
		strcpy(meterInfo->readerNum, strTmp);	

		_ReadField(Idx_ReaderName, strTmp);				// ����Ա����
		strTmp[Size_ReaderName - 1] = '\0';
		strcpy(meterInfo->readerName, strTmp);	

		_ReadField(Idx_OrgNum, strTmp);					// ����λ���
		strTmp[Size_OrgNum - 1] = '\0';
		strcpy(meterInfo->orgNum, strTmp);	

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
	double f64Tmp;
	char *dispBuf;

	while(1){
		_ClearScreen();

		// ������Ϣ-����
		//-----------------------------------------------------
		PrintfXyMultiLine_VaList(0, 0, "<<������Ϣ  %3d/%-3d ", meterInfo->strIdx + 1, meterInfo->strCnt);
		dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", meterInfo->userName);
		
		u8Tmp = meterInfo->meterReadStatus[1] - '0';
		// ״̬ת����1/2/3 --> 0/1/2 δ��/�ѳ�/ʧ��
		u8Tmp = u8Tmp == 1 ? 0 : (u8Tmp == 2 ? 1 : 2);
		if(meterInfo->meterReadType[1] == '3'){
			dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: �ѳ�(¼)\n");
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s\n", (u8Tmp == 0 ? "δ��" : (u8Tmp == 1 ? "�ѳ�" : "ʧ��")));
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "��ַ: %s\n", meterInfo->userAddr);
		
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %s\n", meterInfo->currGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "����ʣ��: %s\n", meterInfo->currRemainGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "���ζ���: %s\n", meterInfo->currReadVal);
		dispIdx += sprintf(&dispBuf[dispIdx], "���ε��Ӷ���: \n          %s\n", meterInfo->currElecReadVal);
		
		dispIdx += sprintf(&dispBuf[dispIdx], "���γ���ʱ��: \n    %s\n", meterInfo->meterReadTime);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ϴγ���ʱ��: \n    %s\n", meterInfo->lastMtrReadTime);
		
		u8Tmp = meterInfo->meterReadType[1] - '0';
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %s\n", GetStrMtrReadType(u8Tmp));
		u8Tmp = meterInfo->meterExcpType[1] - '0';
		dispIdx += sprintf(&dispBuf[dispIdx], "�쳣����: %s\n", GetStrMtrReadException(u8Tmp));
		dispIdx += sprintf(&dispBuf[dispIdx], "�û����: %s\n", meterInfo->userBalance);
		dispIdx += sprintf(&dispBuf[dispIdx], "ƽ������: %s\n", meterInfo->avgGasVol);

		dispIdx += sprintf(&dispBuf[dispIdx], "�ϴ�����: %s\n", meterInfo->lastGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ϴ�ʣ��: %s\n", meterInfo->lastRemainGasVol);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ϴζ���: %s\n", meterInfo->lastReadVal);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ϴε��Ӷ���: \n          %s\n", meterInfo->lastElecReadVal);
		
		dispIdx += sprintf(&dispBuf[dispIdx], "�������: \n    %s\n", meterInfo->sectNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "���������: \n    %s\n", meterInfo->sectName);
		dispIdx += sprintf(&dispBuf[dispIdx], "����Ա���: \n    %s\n", meterInfo->readerNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "����Ա����: \n    %s\n", meterInfo->readerName);
		dispIdx += sprintf(&dispBuf[dispIdx], "����λ���: \n    %s\n", meterInfo->orgNum);
		
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

		// �м����
		for(i = 0; i < RELAY_MAX; i++){				
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
			Args.buf[i++] = 0x01;		// ������	01
			ackLen = 9;					// Ӧ�𳤶� 9	
			Args.lastItemLen = i - 1;
			break;

		case 2:
			CurrCmd = WaterCmd_OpenValve;			// " ���� "
			/*---------------------------------------------*/
			Args.buf[i++] = 0x05;		// ������	05
			ackLen = 0;					// Ӧ�𳤶� 0	
			Args.lastItemLen = i - 1;
			break;

		case 3:
			CurrCmd = WaterCmd_CloseValve;		// " �ط� ";
			/*---------------------------------------------*/
			Args.buf[i++] = 0x06;		// ������	06
			ackLen = 0;					// Ӧ�𳤶� 0	
			Args.lastItemLen = i - 1;
			break;

		case 4:
			CurrCmd = WaterCmd_ClearException;		// " ���쳣���� ";
			/*---------------------------------------------*/
			Args.buf[i++] = 0x03;		// ������	03
			ackLen = 0;					// Ӧ�𳤶� 0	
			Args.lastItemLen = i - 1;
			break;

		case 5:										// �ֹ�¼��
			while(2){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				currUi = 0;
				sprintf(StrBuf[1], "���: %s", StrDstAddr);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, StrBuf[1]);
				strcpy(StrBuf[0], meterInfo->currReadVal);
				if(meterInfo->currReadVal[0] == '0'){
					StrBuf[0][0] = '\0';
				}
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "¼�����:", StrBuf[0], 9, 11*8, true);
						pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " ");
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��������: �ų�");
				sprintf(StrBuf[2], "�ϴζ���: %s", MeterInfo.lastReadVal);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, StrBuf[2]);
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
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
					ShowMsg(16, 2*16, "�����쳣�����ڱ��ϴζ���С��������Ϊ�ϴζ������쳣��������Ϊ��ת��ȷ��Ҫ¼����", 500);
					_Printfxy(0, 9*16, "ȡ��            ȷ��", Color_White);
					key = _ReadKey();
					if (key == KEY_CANCEL){
						_GUIRectangleFill(0, 16*2-8, 16*10, 16*9-4, Color_White);
						continue;
					}
				}

				readStatus = 1;
				sprintf(MeterInfo.currReadVal, "%d.%02d", u32Tmp, (u16Tmp & 0xFF));
				SaveMeterReadResult(meterInfo, 3, readStatus);	// �ֹ�¼��
				ShowMsg(16, 3*16, "����� ¼��ɹ�!", 1000);
				break;
			}
			continue;		// ��ʾ������Ϣ

		default: 
			break;
		}

		// ��ַ���
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
		ackLen += 10 + Addrs.itemCnt * AddrLen;
		timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
		tryCnt = 2;

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
			SaveMeterReadResult(meterInfo, 1, readStatus);	// �ƻ�����
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
