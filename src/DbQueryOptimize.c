#include "Common.h"
#include "DbQueryOptimize.h"
#include "MeterDocDBF.h"


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

    if(query->CellCnt > 0) return;

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
* 参 数：meters		- 户表列表：查询前先设置 meters.(小区编号/楼栋编号/抄表状态/显示字段)
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterList(MeterListSt *meters, DbQuerySt *query)
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
		if(strTmp[0] == '1'){
			state = '1';
			meters->readOkCnt++;		// 成功数量
		}
		else if(strTmp[0] == '2'){
			state = '2';
			meters->readNgCnt++;		// 失败数量
		}else{
			state = '0';				// 未抄数量
		}
		
		if(meters->qryMeterReadStatus != NULL){			// 抄表状态 过滤  ‘0’ - 未抄/失败， ‘1’ - 已抄
			if((meters->qryMeterReadStatus[0] == '1' && state != '1')
				|| (meters->qryMeterReadStatus[0] == '0' && state == '1')){
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

			sprintf(strTmp, " 该楼表具数 超出最大限制 %d !", query->reqMaxCnt);
			ShowMsg(8, 3*16, strTmp, 3000);
			break;
		}

		// 将选择的字段信息 和 数据库索引 加入列表
		len = sprintf(meters->strs[meters->cnt], strTmp);
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
