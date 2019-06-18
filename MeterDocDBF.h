#ifndef MeterDocDBF_H
#define MeterDocDBF_H

#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"


#define	MeterDocDB "jk.dbf"

//---------------	dbf 查询结构
typedef struct {
	// uint8 queryType;	// 查询类型：0-小区列表， 1-楼栋列表； 2-抄表情况列表 , 3 - 户表信息
	// char *districNum;	// 小区编号
	// char *buildingNum;	// 楼栋编号
	// char *meterReadStatus;	// 抄表状态
	// char *meterNum;		// 表号
	// char *userNum;		// 户号
	// char *roomNum;		// 门牌号
	// uint32 dbSelectIdx;	// 选择的数据库记录索引

	uint32	dbCurrIdx;	// 数据库当前位置
	uint32	reqMaxCnt;	// 最大请求数
	uint32	resultCnt;	// 查询的结果记录数
	uint8	errorCode;	// 0 - ok,  其他 - 出错
}DB_QuerySt;

//---------------	小区列表
#define District_Max				20	// 最大小区数
#define	Size_DistrictNum			16	// 小区编号 长度
#define	Size_DistrictName			21	// 小区名称 长度
typedef struct{
	char 	nums[District_Max][Size_DistrictNum];	// 列表项字符串：楼栋编号
	char 	names[District_Max][Size_DistrictName];	// 列表项字符串：楼栋名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
}DistrictListSt;

//--------------	xx小区 - 楼栋列表
#define Building_Max				50	// 一个小区-最大楼栋数
#define	Size_BuildingNum			16	// 楼栋编号 长度
#define	Size_BuildingName			21	// 楼栋名称 长度
typedef struct{
	char 	nums[Building_Max][Size_BuildingNum];		// 列表项字符串：楼栋编号
	char 	names[Building_Max][Size_BuildingName];		// 列表项字符串：楼栋名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	char 	*qryDistricNum;	// 小区编号
}BuildingListSt;

//---------------	xx小区 - xx楼栋 - 户表列表（表号/户号/门牌号/户名） 
#define Meter_Max					200	// 一栋楼-最大表数
#define	Size_ListStr				21	// 户表显示的字符串 长度
typedef struct{
	uint32 	dbIdx[Meter_Max];		// 列表项对应的数据库索引
	char 	strs[Meter_Max][Size_ListStr];	// 列表项字符串：表号/户号/门牌号/户名/地址
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	uint8 	selectField;	// 要显示的字段：表号/户号/门牌号/户名/地址
	char 	*qryDistricNum;		// 小区编号：空值表示所有
	char 	*qryBuildingNum;		// 楼栋编号：空值表示所有
	char 	*qryMeterReadStatus;	// 抄表状态
	uint8 	meterCnt;		// 当前表总数
	uint8 	readOkCnt;	// 已抄数量
	uint8 	readNgCnt;	// 未抄数量
}MeterListSt;


//---------------	户表信息
#define	Size_MeterNum				16	//表号长度
#define	Size_UserNum               	16	//户号长度
#define Size_RoomNum              	16	//门牌号长度
#define Size_MeterReadStatus        1	//抄表状态长度	0 - 未抄  1 - 已抄 
#define	Size_UserName               40	//户名长度
#define Size_MobileNum           	12	//手机号长度
#define	Size_UserAddr               50	//地址长度  
#define Size_MeterReadType			10	//抄表方式长度		
#define Size_MeterReadTime          20	//抄表时间长度
#define Size_MeterValue             12	//表读数
#define Size_MeterStaus           	40	//表状态长度
#define Size_BatteryVoltage         5   //电池电压长度
#define Size_SignalValue           	5 	//信号强度长度

#define Invalid_Idx		(0xFFFFFFFF)	// 无效索引

typedef struct 
{
	uint32	dbIdx;		// 记录在数据库中索引
	char	*qryMeterNum;		// 表号
	char 	*qryUserNum;		// 户号
	char 	*qryRoomNum;		// 门牌号
	char	meterNum[Size_MeterNum];
	char	userNum[Size_UserNum];
	char	roomNum[Size_RoomNum];
	char	meterReadStatus[Size_MeterReadStatus];
	char	userName[Size_UserName];
	char	mobileNum[Size_MobileNum];
	char	userAddr[Size_UserAddr];
	char	meterReadType[Size_MeterReadType];
	char	meterReadTime[Size_MeterReadTime];
	char	meterValue[Size_MeterValue];
	char	meterStaus[Size_MeterStaus];
	char	batteryVoltage[Size_BatteryVoltage];
	char	signalValue[Size_SignalValue];

}MeterInfoSt;


//----------------------	数据库信息	- 表字段
// 字段索引
typedef enum{
	Idx_Id	= 0,		// "ID",		// ID
	Idx_UserNum,		// "HH",		// 户号
	Idx_UserName,		// "HM",		// 户名
	Idx_UserAddrs,		// "DZ",		// 地址
	Idx_UserRoomNum,	// "MPH",		// 门牌号
	Idx_UserTelNum,		// "TEL",		// 电话
	Idx_UserMobileNum,	// "MOBILE",	// 手机
	Idx_AreaNum,			// "CBQY",		// 抄表区域
	Idx_CollectorNum,		// "ZDBH",		// 采集器 编号
	Idx_CollectorName,		// "ZDMC",		// 采集器 名称
	Idx_CollectorSite,		// "ZDDZ",		// 采集器 位置
	Idx_DistrictNum,		// "CBYJFZBH",		// 抄表小区 编号
	Idx_DistrictName,		// "CBYJFZMC",		// 抄表小区 名称
	Idx_BuildingNum,		// "CBEJFZBH",		// 抄表楼栋 编号
	Idx_BuildingName,		// "CBEJFZMC",		// 抄表楼栋 名称
	Idx_MeterNum,			// "BH",		// 表号
	Idx_FLXS,				// "FLXS",		// 
	Idx_MeterValue,			// "FLA",		// 表读数 FL[A-J] 10个 》》
	Idx_FLB,				// "FLB",		// 
	Idx_FLC,				// "FLC",		// 
	Idx_FLD,				// "FLD",		// 
	Idx_FLE,				// "FLE",		// 
	Idx_FLF,				// "FLF",		// 
	Idx_FLG,				// "FLG",		// 
	Idx_FLH,				// "FLH",		// 
	Idx_FLI,				// "FLI",		// 
	Idx_FLJ,				// "FLJ",		// 表读数 FL[A-J] 10个 《《
	Idx_LYXS,				// "LYXS",		// 
	Idx_LYA,				// "LYA",		// LY[A-J] 10个 >>
	Idx_LYB,				// "LYB",		// 
	Idx_LYC,				// "LYC",		// 
	Idx_LYD,				// "LYD",		// 
	Idx_LYE,				// "LYE",		// 
	Idx_LYF,				// "LYF",		// 
	Idx_LYG,				// "LYG",		// 
	Idx_LYH,				// "LYH",		// 
	Idx_LYI,				// "LYI",		// 
	Idx_LYJ,				// "LYJ",		// LY[A-J] 10个 <<
	Idx_BLXHEX,				// "BLXHEX",	// 表类型 HEX
	Idx_BLXSTR,				// "BLXSTR",	// 表类型 STR
	Idx_MeterStatusHex,		// "BZTHEX",	// 表状态 HEX
	Idx_MeterStatusStr,		// "BZTSTR",	// 表状态 STR
	Idx_BatteryVoltage,		// "DCDY",		// 电池电压
	Idx_MCCS,				// "MCCS",		// 脉冲常数
	Idx_MeterReadType,		// "SGCQ",		// 抄表方式 0 - 掌机抄表 ， 1 - 集中器抄表
	Idx_QF,					// "QF",		// 
	Idx_SignalValue,		// "XHQD",		// 信号强度
	Idx_MeterReadTime,		// "CBSJ",		// 抄表时间
	Idx_MeterReadStatus,	// "CBZT",		// 抄表状态
	Idx_BLZDA,				// "BLZDA",		// BLZD[A-J] 10个 》》
	Idx_BLZDB,				// "BLZDB",		// 
	Idx_BLZDC,				// "BLZDC",		// 集中器 编号
	Idx_BLZDD,				// "BLZDD",		// 集中器 名称
	Idx_BLZDE,				// "BLZDE",		// 
	Idx_BLZDF,				// "BLZDF",		// 
	Idx_BLZDG,				// "BLZDG",		// 
	Idx_BLZDH,				// "BLZDH",		// 
	Idx_BLZDI,				// "BLZDI",		// 
	Idx_BLZDJ,				// "BLZDJ"		// BLZD[A-J] 10个 《《
	Idx_Invalid	= 0xFF	// 无效字段	
}DB_Field_Index;

// 字段名
const char *Fields[] = {
	"ID",		// ID
	"HH",		// 户号
	"HM",		// 户名
	"DZ",		// 地址
	"MPH",		// 门牌号
	"TEL",		// 电话
	"MOBILE",	// 手机
	"CBQY",		// 抄表区域
	"ZDBH",		// 采集器 编号
	"ZDMC",		// 采集器 名称
	"ZDDZ",		// 采集器 位置
	"CBYJFZBH",		// 抄表小区 编号
	"CBYJFZMC",		// 抄表小区 名称
	"CBEJFZBH",		// 抄表楼栋 编号
	"CBEJFZMC",		// 抄表楼栋 名称
	"BH",		// 表号
	"FLXS",		// 
	"FLA",		// 表读数 FL[A-J] 10个 》》
	"FLB",		// 
	"FLC",		// 
	"FLD",		// 
	"FLE",		// 
	"FLF",		// 
	"FLG",		// 
	"FLH",		// 
	"FLI",		// 
	"FLJ",		// 表读数 FL[A-J] 10个 《《
	"LYXS",		// 
	"LYA",		// LY[A-J] 10个 >>
	"LYB",		// 
	"LYC",		// 
	"LYD",		// 
	"LYE",		// 
	"LYF",		// 
	"LYG",		// 
	"LYH",		// 
	"LYI",		// 
	"LYJ",		// LY[A-J] 10个 <<
	"BLXHEX",	// 
	"BLXSTR",	// 
	"BZTHEX",	// 表状态 HEX
	"BZTSTR",	// 表状态 STR
	"DCDY",		// 电池电压
	"MCCS",		// 
	"SGCQ",		// 抄表方式 ？
	"QF",		// 
	"XHQD",		// 信号强度
	"CBSJ",		// 抄表时间
	"CBZT",		// 抄表状态
	"BLZDA",		// BLZD[A-J] 10个 》》
	"BLZDB",		// 
	"BLZDC",		// 集中器 编号
	"BLZDD",		// 集中器 名称
	"BLZDE",		// 
	"BLZDF",		// 
	"BLZDG",		// 
	"BLZDH",		// 
	"BLZDI",		// 
	"BLZDJ"			// BLZD[A-J] 10个 《《
};


//----------------------	数据库信息-操作函数		-------------------------------------

/*
* 描 述：字符串列表中查找字符串
* 参 数：strs		- 字符串列表
*		strsLen		- 字符串列表长度
*		dstStr		- 查找的字符串
*		cmpMaxLen	- 比较的最大长度
* 返 回：int	- 找到的字符串在列表中的索引： -1 - 未找到， 0~n - 找到	
*/
int FindStrInList(char ** strs, uint16 strsLen, const char *dstStr, uint16 cmpMaxLen)
{
	int i = -1;

	for(i = 0; i < strsLen; i++){
		if(strncmp(strs[i], dstStr, cmpMaxLen) == 0){
			break;
		}
	}
	if(i >= strsLen){
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
void QueryDistrictList(DistrictListSt *districts, DB_QuerySt *query)
{
	uint32 i, j, recCnt, resultCnt;
	char strTmp1[Size_DistrictNum];
	char strTmp2[Size_DistrictName];
	uint8 isReapet;

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	districts->cnt = 0;
	districts->idx = 0;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){
		_ReadFieldEx(Idx_DistrictNum, strTmp1);		// 小区编号
		strTmp1[Size_DistrictNum - 1] = '\0';

		if(-1 != FindStrInList(districts->nums, districts->cnt, strTmp1, Size_DistrictNum)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				break;
			}

			strncpy(districts->nums[districts->cnt], strTmp1, Idx_DistrictNum);

			_ReadFieldEx(Idx_DistrictName, strTmp2);	// 小区名称
			strTmp2[Size_DistrictName -1] = '\0';
			strncpy(districts->names[districts->cnt], strTmp2, Size_DistrictName);

			districts->cnt++;
			
		}
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
void QueryBuildingList(BuildingListSt *buildings, DB_QuerySt *query)
{
	uint32 i, j, recCnt, resultCnt;
	char strTmp[20];
	char strTmp1[Size_BuildingNum];
	char strTmp2[Size_BuildingName];
	uint8 isReapet;

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	buildings->cnt = 0;
	buildings->idx = 0;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){

		_ReadFieldEx(Idx_DistrictNum, strTmp);		// 小区编号 过滤
		strTmp[Size_DistrictNum - 1] = '\0';
		if(strcmp(buildings->qryDistricNum, strTmp) != 0){
			continue;
		}

		_ReadFieldEx(Idx_BuildingNum, strTmp1);		// 楼栋编号
		strTmp1[Size_BuildingNum - 1] = '\0';

		if(-1 != FindStrInList(buildings->nums, buildings->cnt, strTmp1, Size_BuildingNum)){
			
			query->resultCnt++;
			if(query->resultCnt > query->reqMaxCnt){
				query->errorCode = 1;
				break;
			}

			strncpy(buildings->nums[buildings->cnt], strTmp1, Size_BuildingNum);

			_ReadFieldEx(Idx_BuildingName, strTmp2);	// 楼栋名称
			strTmp2[Size_BuildingName -1] = '\0';
			strncpy(buildings->names[buildings->cnt], strTmp2, Size_BuildingName);

			buildings->cnt++;
			
		}
	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：查询xx小区-xx楼栋-抄表统计情况
* 参 数：meters		- 抄表情况列表
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterReadCountInfo(MeterListSt *meters, DB_QuerySt *query)
{
	uint32 i, j, recCnt, resultCnt;
	char strTmp[50];
	char strTmp1[Size_BuildingNum];
	char strTmp2[Size_BuildingName];
	uint8 showStrSize;

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	meters->cnt = 0;
	meters->idx = 0;
	meters->meterCnt = 0;
	meters->readOkCnt = 0;
	meters->readNgCnt = 0;
	query->resultCnt = 0;
	query->errorCode = 0;
	for(i = 0; i < recCnt; i++){

		if(meters->qryDistricNum != NULL){
			_ReadFieldEx(Idx_DistrictNum, strTmp);		// 小区编号 过滤
			strTmp[Size_DistrictNum - 1] = '\0';
			if(strcmp(meters->qryDistricNum, strTmp) != 0){
				continue;
			}
		}
		if(meters->qryBuildingNum != NULL){
			_ReadFieldEx(Idx_DistrictNum, strTmp);		// 楼栋编号 过滤
			strTmp[Size_BuildingNum - 1] = '\0';
			if(strcmp(meters->qryBuildingNum, strTmp) != 0){
				continue;
			}
		}

		meters->meterCnt++;		// 当前表总数
		
		if(meters->qryMeterReadStatus != NULL){
			_ReadFieldEx(Idx_DistrictNum, strTmp);		// 抄表状态 过滤  NULL - 所有， ‘0’ - 未抄， ‘1’ - 已抄
			strTmp[Size_MeterReadStatus - 1] = '\0';
			if(strcmp(strTmp, "1") != 0){
				meters->readNgCnt++;		// 未抄数量
			}else{
				meters->readOkCnt++;		// 已抄数量
			}
			if(strcmp(meters->qryMeterReadStatus, strTmp) != 0){
				continue;
			}
		}

		
		if(meters->selectField == Idx_Invalid){	// 未选择字段，则不构建列表
			continue;
		}

		switch (meters->selectField)		// 列表类型：默认为表号列表
		{
		case Idx_MeterNum:
		case Idx_UserNum:
		case Idx_UserRoomNum:
		case Idx_UserName:
		case Idx_UserAddrs:
		default: 
			meters->selectField = Idx_MeterNum;
			break;
		}
		_ReadFieldEx(meters->selectField, strTmp);	// 读取字段：表号/户号/门牌号/户名/地址
		strTmp[Size_ListStr - 1] = '\0';			// 最多显示一行

		query->resultCnt++;
		if(query->resultCnt > query->reqMaxCnt){
			query->errorCode = 1;
			break;
		}

		strncpy(meters->strs[meters->cnt], strTmp, Size_ListStr);	// 添加到显示列表
		meters->dbIdx[meters->cnt] = i;								// 添加到DB索引列表
		meters->cnt++;

	}
	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：显示xx小区-xx楼栋-抄表统计情况
* 参 数：meters		- 抄表情况列表
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterReadCountInfo(MeterListSt *meters)
{

}

/*
* 描 述：查询户表信息
* 参 数：meterInfo	- 户表信息
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterInfo(MeterInfoSt *meterInfo, DB_QuerySt *query)
{
	uint32 i, j, recCnt, resultCnt;
	char strTmp[50];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	query->resultCnt = 0;
	query->errorCode = 0;

	if(meterInfo->dbIdx != Invalid_Idx){	// 数据库记录索引 是否有效？
		if(meterInfo->dbIdx > recCnt -1){
			meterInfo->dbIdx = Invalid_Idx;
		}
	}

	if(meterInfo->dbIdx == Invalid_Idx){	// 数据库记录索引无效时 执行查询

		#if 0
		for(i = 0; i < recCnt; i++){
			if(meterInfo->qryMeterNum != NULL){
				_ReadFieldEx(Idx_MeterNum, strTmp);		// 按表号查询
				strTmp[Size_MeterNum - 1] = '\0';
				if(strcmp(meterInfo->qryMeterNum, strTmp) == 0){
					meterInfo->dbIdx = i;
					break;
				}
			}
			else if(meterInfo->qryUserNum != NULL){
				_ReadFieldEx(Idx_UserNum, strTmp);		// 按户号查询
				strTmp[Size_UserNum - 1] = '\0';
				if(strcmp(meterInfo->qryUserNum, strTmp) == 0){
					meterInfo->dbIdx = i;
					break;
				}
			}
			else if(meterInfo->qryRoomNum != NULL){
				_ReadFieldEx(Idx_UserRoomNum, strTmp);	// 按门牌号查询
				strTmp[Size_RoomNum - 1] = '\0';
				if(strcmp(meterInfo->qryRoomNum, strTmp) == 0){
					meterInfo->dbIdx = i;
					break;
				}
			}
		}
		#endif

		if(meterInfo->qryMeterNum != NULL
			&& _LocateEx(Idx_MeterNum, '=', meterInfo->qryMeterNum, 1, recCnt, 0) > 0){ 
			// 按表号查询
			meterInfo->dbIdx = _Recno();
		}
		else if(meterInfo->qryMeterNum != NULL
			&& _LocateEx(Idx_UserNum, '=', meterInfo->qryMeterNum, 1, recCnt, 0) > 0){ 
			// 按表号查询
			meterInfo->dbIdx = _Recno();
		}
		else if(meterInfo->qryRoomNum != NULL
			&& _LocateEx(Idx_UserRoomNum, '=', meterInfo->qryRoomNum, 1, recCnt, 0) > 0){ 
			// 按表号查询
			meterInfo->dbIdx = _Recno();
		}

		if(i >= recCnt){
			meterInfo->dbIdx = Invalid_Idx;
		}
	}

	if(meterInfo->dbIdx != Invalid_Idx){	// 数据库记录索引有效时 读取记录
		query->resultCnt = 1;

		_ReadFieldEx(Idx_MeterNum, strTmp);					// 表号
		strTmp[Size_MeterNum - 1] = '\0';
		strncpy(meterInfo->meterNum, strTmp, Size_ListStr);	

		_ReadFieldEx(Idx_UserNum, strTmp);					// 户号
		strTmp[Size_UserNum - 1] = '\0';
		strncpy(meterInfo->userNum, strTmp, Size_UserNum);	

		_ReadFieldEx(Idx_UserRoomNum, strTmp);				// 门牌号
		strTmp[Size_RoomNum - 1] = '\0';
		strncpy(meterInfo->roomNum, strTmp, Size_RoomNum);	

		_ReadFieldEx(Idx_MeterReadStatus, strTmp);			// 抄表状态
		strTmp[Size_MeterReadStatus - 1] = '\0';
		strncpy(meterInfo->meterReadStatus, strTmp, Size_MeterReadStatus);	

		_ReadFieldEx(Idx_UserName, strTmp);					// 户名
		strTmp[Size_UserName - 1] = '\0';
		strncpy(meterInfo->userName, strTmp, Size_UserName);	

		_ReadFieldEx(Idx_UserMobileNum, strTmp);			// 手机号
		strTmp[Size_MobileNum - 1] = '\0';
		strncpy(meterInfo->mobileNum, strTmp, Size_MobileNum);	

		_ReadFieldEx(Idx_UserAddrs, strTmp);				// 地址
		strTmp[Size_UserAddr - 1] = '\0';
		strncpy(meterInfo->userAddr, strTmp, Size_UserAddr);	

		_ReadFieldEx(Idx_MeterReadType, strTmp);			// 抄表方式
		strTmp[Size_MeterReadType - 1] = '\0';
		strncpy(meterInfo->meterReadType, strTmp, Size_MeterReadType);	

		_ReadFieldEx(Idx_MeterReadTime, strTmp);			// 抄表时间
		strTmp[Size_MeterReadTime - 1] = '\0';
		strncpy(meterInfo->meterReadTime, strTmp, Size_MeterReadTime);	

		_ReadFieldEx(Idx_MeterValue, strTmp);				// 表读数
		strTmp[Size_MeterValue - 1] = '\0';
		strncpy(meterInfo->meterValue, strTmp, Size_MeterValue);	

		_ReadFieldEx(Idx_MeterStatusStr, strTmp);			// 表状态
		strTmp[Size_MeterStaus - 1] = '\0';
		strncpy(meterInfo->meterStaus, strTmp, Size_MeterStaus);	

		_ReadFieldEx(Idx_BatteryVoltage, strTmp);			// 电池电压
		strTmp[Size_BatteryVoltage - 1] = '\0';
		strncpy(meterInfo->batteryVoltage, strTmp, Size_BatteryVoltage);	

		_ReadFieldEx(Idx_SignalValue, strTmp);				// 信号强度
		strTmp[Size_SignalValue - 1] = '\0';
		strncpy(meterInfo->signalValue, strTmp, Size_SignalValue);	
	}

	_Use("");		// 关闭数据库

	query->dbCurrIdx = i;
}

/*
* 描 述：显示户表信息
* 参 数：meterInfo	- 户表信息
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterInfo(MeterInfoSt *meterInfo)
{
	_Printfxy(0, 0, "<<户表信息", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);
}


//------------------------		外部接口声明	-------------------------------------

extern MeterInfoSt meterInfo;
extern DistrictListSt distctList;
extern BuildingListSt buildingList;
extern MeterListSt meterList;

#endif