#include "Common.h"
#include "DbQueryOptimize.h"
#include "MeterDocDBF.h"


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

    if(query->CellCnt > 0) return;

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
* �� ����meters		- �����б���ѯǰ������ meters.(С�����/¥�����/����״̬/��ʾ�ֶ�)
*		 query		- ���ݿ��ѯ�ṹ
* �� �أ�void
*/
void QueryMeterList(MeterListSt *meters, DbQuerySt *query)
{
	uint32 i, recCnt;
	char strTmp[Size_DbStr];
	uint8 state;
	int len;

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
	query->reqMaxCnt = Meter_Max;
	query->resultCnt = 0;
	query->errorCode = 0;
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
		_ReadField(Idx_MeterReadStatus, strTmp);		
		strTmp[Size_MeterReadStatus - 1] = '\0';
		if(strTmp[0] == '1'){
			state = 1;
			meters->readOkCnt++;		// �ɹ�����
		}
		else if(strTmp[0] == '2'){
			state = 2;
			meters->readNgCnt++;		// ʧ������
		}else{
			state = 0;				// δ������
		}
		
		// ����״̬ ����  ��0�� - δ��/ʧ�ܣ� ��1�� - �ѳ�
		if((meters->qryMeterReadStatus == 1 && state != 1)
			|| (meters->qryMeterReadStatus == 0 && state == 1)){
			_Skip(1);	// ��һ�����ݿ��¼
			continue;
		}

		switch (meters->selectField)		// �б����ͣ�Ĭ��Ϊ����б�
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
		if(meters->selectField == Idx_Invalid){		// δѡ���ֶΣ��򲻹����б�
			_Skip(1);	// ��һ�����ݿ��¼
			continue;
		}

		_ReadField(meters->selectField, strTmp);	// ��ȡ�ֶΣ����/����/���ƺ�/����/��ַ
		strTmp[Size_ListStr - 1] = '\0';			// �����ʾһ��

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;

			sprintf(strTmp, " ��¥����� ����������� %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		// ��ѡ����ֶ���Ϣ �� ���ݿ����� �����б�
		len = sprintf(meters->strs[meters->cnt], strTmp);
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
