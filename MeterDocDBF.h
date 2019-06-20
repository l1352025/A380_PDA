#ifndef MeterDocDBF_H
#define MeterDocDBF_H

#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"

//----------------------	宏定义		------------------------
#define	MeterDocDB 	"jk.dbf"			// 表档案数据库文件名

#define Invalid_Idx	(0xFFFFFFFF)		// 无效索引

#define District_Max				20	// 最大小区数
#define Building_Max				50	// 一个小区-最大楼栋数
#define Meter_Max					200	// 一栋楼-最大表数

#define	Size_ListStr	20				// 列表字符串长度

#define	Size_DistrictNum			12	// 小区编号 长度	10
#define	Size_DistrictName			50	// 小区名称 长度	50
#define	Size_BuildingNum			12	// 楼栋编号 长度	10
#define	Size_BuildingName			50	// 楼栋名称 长度	50

#define	Size_MeterNum				16	//表号长度 		12
#define	Size_UserNum               	16	//户号长度		32
#define Size_RoomNum              	16	//门牌号长度	16
#define Size_MeterReadStatus        2	//抄表状态长度	1   : 0 - 未抄  1 - 已抄 
#define	Size_UserName               40	//户名长度		50
#define Size_MobileNum           	12	//手机号长度	50
#define	Size_UserAddr               80	//地址长度 		100 
#define Size_MeterReadType			2	//抄表方式长度	1	
#define Size_MeterReadTime          20	//抄表时间长度	20
#define Size_MeterValue             12	//表读数		12
#define Size_MeterStatusHex         8	//表状态Hex长度	8
#define Size_MeterStatusStr         40	//表状态Str长度	50
#define Size_BatteryVoltage         6   //电池电压长度	5
#define Size_SignalValue           	10 	//信号强度长度	10

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
}DbQuerySt;

//---------------	小区列表
typedef struct{
	char 	nums[District_Max][Size_ListStr];	// 列表项字符串：楼栋编号
	char 	names[District_Max][Size_ListStr];	// 列表项字符串：楼栋名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
}DistrictListSt;

//--------------	xx小区 - 楼栋列表
typedef struct{
	char 	nums[Building_Max][Size_ListStr];		// 列表项字符串：楼栋编号
	char 	names[Building_Max][Size_ListStr];		// 列表项字符串：楼栋名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	char 	*qryDistricNum;	// 小区编号
}BuildingListSt;

//---------------	xx小区 - xx楼栋 - 户表列表（表号/户号/门牌号/户名） 
typedef struct{
	uint32 	dbIdx[Meter_Max];				// 列表项对应的数据库索引
	char 	strs[Meter_Max][Size_ListStr];	// 列表项字符串：表号/户号/门牌号/户名/地址
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	uint8 	selectField;	// 要显示的字段：表号/户号/门牌号/户名/地址
	char 	*qryDistricNum;			// 小区编号：空值表示所有
	char 	*qryBuildingNum;		// 楼栋编号：空值表示所有
	char 	*qryMeterReadStatus;	// 抄表状态
	char 	districName[Size_DistrictName];		// 小区名
	char 	buildingName[Size_BuildingName];	// 楼栋名
	uint16 	meterCnt;		// 当前表总数
	uint16 	readOkCnt;		// 已抄数量
	uint16 	readNgCnt;		// 未抄数量
}MeterListSt;


//---------------	户表信息
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
	char	meterStatusHex[Size_MeterStatusHex];
	char	meterStatusStr[Size_MeterStatusStr];
	char	batteryVoltage[Size_BatteryVoltage];
	char	signalValue[Size_SignalValue];

}MeterInfoSt;

//------------------------		外部接口声明	-------------------------------------
extern MeterInfoSt MeterInfo;
extern DistrictListSt Districts;
extern BuildingListSt Buildings;
extern MeterListSt Meters;
extern DbQuerySt DbQuery;

extern void QueryDistrictList(DistrictListSt *districts, DbQuerySt *query);
extern void QueryBuildingList(BuildingListSt *buildings, DbQuerySt *query);
extern void QueryMeterList(MeterListSt *meters, DbQuerySt *query);
extern uint8 ShowMeterReadCountInfo(MeterListSt *meters);
extern void SaveMeterReadResult(MeterInfoSt *meterInfo);
extern void QueryMeterInfo(MeterInfoSt *meterInfo, DbQuerySt *query);
extern uint8 ShowMeterInfo(MeterInfoSt *meterInfo);

//-----------------------	全局变量定义
MeterInfoSt MeterInfo;
DistrictListSt Districts;
BuildingListSt Buildings;
MeterListSt Meters;
DbQuerySt DbQuery;

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
	char strTmp1[50];
	char strTmp2[50];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	districts->cnt = 0;
	districts->idx = 0;
	query->reqMaxCnt = (query->reqMaxCnt == 0 ? District_Max : query->reqMaxCnt);
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
	char strTmp[50];
	char strTmp1[50];
	char strTmp2[50];

	_Select(1);
	_Use(MeterDocDB);	// 打开数据库
	recCnt = _Reccount();
	_Go(0);
	buildings->cnt = 0;
	buildings->idx = 0;
	query->reqMaxCnt = (query->reqMaxCnt == 0 ? Building_Max : query->reqMaxCnt);
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
	char strTmp[50];

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
	//query->reqMaxCnt = (query->reqMaxCnt == 0 ? Meter_Max : query->reqMaxCnt);
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

		meters->meterCnt++;		// 当前表总数
		
		if(meters->qryMeterReadStatus != NULL){
			_ReadField(Idx_MeterReadStatus, strTmp);		// 抄表状态 过滤  NULL - 所有， ‘0’ - 未抄， ‘1’ - 已抄
			strTmp[Size_MeterReadStatus - 1] = '\0';
			if(strcmp(strTmp, "1") != 0){
				meters->readNgCnt++;		// 未抄数量
			}else{
				meters->readOkCnt++;		// 已抄数量
			}
			if(strcmp(meters->qryMeterReadStatus, strTmp) != 0){
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
		if(meters->selectField == Idx_Invalid){	// 未选择字段，则不构建列表
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
	uint8 key, i, cnt = 0, isCancel = false;
	uint8 tryCnt;
	uint16 ackLen = 0, timeout;
	char strTmp[50];

	// 中继清空
	for(i = 0; i < RELAY_MAX; i++){				
		if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
			StrRelayAddr[i][0] = 0x00;
		}
	}
	while(cnt < meters->cnt){

		_ClearScreen();
		_Printfxy(0, 0, "<<自动抄表", Color_White);
		/*---------------------------------------------*/
		// 设置表号
		_Select(1);
		_Use(MeterDocDB);	// 打开数据库
		_Go(meters->dbIdx[cnt]);

		_Use("");			// 关闭数据库
		strcpy(StrDstAddr, strTmp);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		Args.buf[i++] = 0x01;		// 命令字	01
		ackLen = 21;				// 应答长度 21	
		Args.buf[i++] = 0x00;		// 数据格式 00	
		Args.lastItemLen = i - 1;

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * AddrLen;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		if(false == Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt)){
			if(strncmp(DispBuf, "表号", 4) != 0){	// 命令已取消	
				DispBuf[0] = NULL;
				isCancel = true;
				break;
			}
		}

		//----------------------------------------------
		_Printfxy(0, 9*16, "返回  < 轮抄中 >     ", Color_White);
	
	}

	if(isCancel){
		
		_Printfxy(0, 9*16, "返回  < 已取消 >    ", Color_White);
	}
	else{
		_Printfxy(0, 9*16, "返回  < 已完成 >    ", Color_White);
	}
	
	while(1){
		
	}
	
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

	_Printfxy(0, 0, "<<户表信息", Color_White);
	/*---------------------------------------------*/
	dispIdx += sprintf(&dispBuf[dispIdx], "小区: %s\n", meters->districName);
	dispIdx += sprintf(&dispBuf[dispIdx], "楼栋: %s\n", meters->buildingName);
	dispIdx += sprintf(&dispBuf[dispIdx], "总数: %d\n", meters->meterCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "已抄: %d\n", meters->readOkCnt);
	dispIdx += sprintf(&dispBuf[dispIdx], "未抄: %d\n", meters->readNgCnt);
	//----------------------------------------------
	_Printfxy(0, 9*16, "返回            确定", Color_White);

	key = ShowScrollStr(&DispBuf,  7);
	
	return key;
}

/*
* 描 述：显示xx小区-xx楼栋-已抄/未抄列表
* 参 数：meters		- 抄表情况列表: 调用前先设置抄表状态 qryMeterReadStatus
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterList(MeterListSt *meters)
{
	uint8 key;
	ListBox showTpList, meterList;		// 显示方式/表信息-列表
	char *title = NULL;

	// 列表显示方式-界面
	title = (meters->qryMeterReadStatus[0] == '1' ? "<<已抄列表" : "<<未抄列表");
	ListBoxCreate(&showTpList, 0, 0, 20, 7, 4, NULL,
		title, 
		4,
		"1. 按表号显示",
		"2. 按户号显示",
		"3. 按户名显示",
		"4. 按门牌号显示");
	//------------------------------------------------------------
	_Printfxy(0, 9*16, "返回            确定", Color_White);
	while(1){

		_ClearScreen();
		
		key = ShowListBox(&showTpList);
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
		//------------------------------------------------------------
		_Printfxy(0, 9*16, "返回        户表信息", Color_White);
		while(2){
			#if Log_On
			LogPrint("meters->idx = %d,  meters->cnt = %d,  meters->meterCnt = %d,  meters->selectField = %d \n \
				meterList.strIdx = %d, meterList.strCnt = %d, meterList.currIdx = %d, meterList.totalCnt = %d \n", \
				meters->idx, meters->cnt, meters->meterCnt, meters->selectField, \
				meterList.strIdx, meterList.strCnt, meterList.currIdx, meterList.totalCnt);
			#endif
			
			_ClearScreen();
		
			key = ShowListBox(&meterList);
			if(key == KEY_CANCEL){	// 返回
				break;
			}
			if(key == KEY_ENTER && meters->cnt == 0){	
				_Printfxy(0, 5*16, "当前列表无条目！", Color_White);
				continue;
			}
			
			meters->idx = (uint8)(meterList.strIdx & 0xFF);
			
			#if Log_On
			LogPrint("meters->idx = %d,  meters->cnt = %d, meters->meterCnt = %d,  meters->selectField = %d \n " , \
				meters->idx, meters->cnt, meters->meterCnt, meters->selectField);
			#endif
			
			// 户表信息-界面
			//------------------------------------------------------
			MeterInfo.dbIdx = meters->dbIdx[meters->idx];
			QueryMeterInfo(&MeterInfo, &DbQuery);	// 户表信息查询
			
			#if Log_On
			LogPrint("meters->idx = %d,  meters->cnt = %d, meters->meterCnt = %d,  meters->selectField = %d \n " , \
				meters->idx, meters->cnt, meters->meterCnt, meters->selectField);
			#endif
			//------------------------------------------------------
			key = ShowMeterInfo(&MeterInfo);	// 返回到列表

			#if Log_On
			LogPrint("meters->idx = %d,  meters->cnt = %d, meters->meterCnt = %d,  meters->selectField = %d \n " , \
				meters->idx, meters->cnt, meters->meterCnt, meters->selectField);
			#endif
			
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

	// 更新抄表结果
	_GetDateTime(time, '-', ':');
	_Replace(Idx_MeterReadStatus, "1");	// 抄表状态 0 - 未抄， 1 - 已抄
	_Replace(Idx_MeterReadType, "0");	// 抄表方式 0 - 掌机抄表 ， 1 - 集中器抄表
	_Replace(Idx_MeterReadTime, time);	// 抄表时间
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
	char strTmp[100];

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
				_ReadField(Idx_MeterNum, strTmp);		// 按表号查询
				strTmp[Size_MeterNum - 1] = '\0';
				if(strcmp(meterInfo->qryMeterNum, strTmp) == 0){
					meterInfo->dbIdx = i;
					break;
				}
			}
			else if(meterInfo->qryUserNum != NULL){
				_ReadField(Idx_UserNum, strTmp);		// 按户号查询
				strTmp[Size_UserNum - 1] = '\0';
				if(strcmp(meterInfo->qryUserNum, strTmp) == 0){
					meterInfo->dbIdx = i;
					break;
				}
			}
			else if(meterInfo->qryRoomNum != NULL){
				_ReadField(Idx_UserRoomNum, strTmp);	// 按门牌号查询
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

		_Go(meterInfo->dbIdx);
		query->resultCnt = 1;

		_ReadField(Idx_MeterNum, strTmp);					// 表号
		strTmp[Size_MeterNum - 1] = '\0';
		strncpy(meterInfo->meterNum, strTmp, Size_MeterNum);	

		_ReadField(Idx_UserNum, strTmp);					// 户号
		strTmp[Size_UserNum - 1] = '\0';
		strncpy(meterInfo->userNum, strTmp, Size_UserNum);	

		_ReadField(Idx_UserRoomNum, strTmp);				// 门牌号
		strTmp[Size_RoomNum - 1] = '\0';
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
		strTmp[Size_UserAddr - 1] = '\0';
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

	query->dbCurrIdx = i;
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
	uint8 * pByte, tryCnt;
	uint16 ackLen = 0, timeout;
	char *dispBuf;

	// 户表信息-界面
	while(1){

		_ClearScreen();

		_Printfxy(0, 0, "<<户表信息", Color_White);
		/*---------------------------------------------*/
		dispBuf = &DispBuf;
		dispIdx = 0;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", meterInfo->meterNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "户号: %s\n", meterInfo->userNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "门牌号: %s\n", meterInfo->roomNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表状态: %s\n", meterInfo->meterReadStatus);
		dispIdx += sprintf(&dispBuf[dispIdx], "户名: %s\n", meterInfo->userName);
		dispIdx += sprintf(&dispBuf[dispIdx], "手机: %s\n", meterInfo->mobileNum);
		dispIdx += sprintf(&dispBuf[dispIdx], "地址: %s\n", meterInfo->userAddr);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表方式: %s\n", meterInfo->meterReadType);
		dispIdx += sprintf(&dispBuf[dispIdx], "抄表时间: %s\n", meterInfo->meterReadTime);
		dispIdx += sprintf(&dispBuf[dispIdx], "表读数: %s\n", meterInfo->meterValue);
		dispIdx += sprintf(&dispBuf[dispIdx], "表状态: %s\n", meterInfo->meterStatusStr);
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压: %s\n", meterInfo->batteryVoltage);
		dispIdx += sprintf(&dispBuf[dispIdx], "信号强度: %s\n", meterInfo->signalValue);
		//----------------------------------------------
		_Printfxy(0, 9*16, "返回        户表命令", Color_White);

		key = ShowScrollStr(dispBuf,  7);
		if(key == KEY_CANCEL){	// 返回
			break;
		}

		
		strcpy(StrDstAddr, meterInfo->meterNum);	// 表号设置
		for(i = 0; i < RELAY_MAX; i++){				// 中继清空
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
			}
		}

		// 户表命令-界面
		ListBoxCreate(&menuList, 3*16, 3*16, 14, 4, 4, NULL, 
			"户表命令", 
			4, 
			"1. 抄表",
			"2. 开阀",
			"3. 关阀",
			"4. 清异常");
		//---------------------
		key = ShowListBox(&menuList);
		if(key == KEY_CANCEL){	// 取消执行命令，重新显示户表信息
			continue;
		}

		// 显示标题
		pByte = menuList.str[menuList.strIdx];
		sprintf(TmpBuf, "<<%s",&pByte[3]);
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
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 21;				// 应答长度 21	
			// 数据域
			Args.buf[i++] = 0x00;				// 数据格式 00	
			Args.lastItemLen = i - 1;
			break;

		case 2:
			CurrCmd = WaterCmd_OpenValve;			// " 开阀 "
			/*---------------------------------------------*/
			Args.buf[i++] = 0x03;		// 命令字	03
			ackLen = 3;					// 应答长度 3	
			// 数据域
			Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
			Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
			Args.lastItemLen = i - 1;
			break;

		case 3:
			CurrCmd = WaterCmd_CloseValve;		// " 关阀 ";
			/*---------------------------------------------*/
			Args.buf[i++] = 0x03;		// 命令字	03
			ackLen = 3;					// 应答长度 3	
			// 数据域
			Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
			Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
			Args.lastItemLen = i - 1;
			break;

		case 4:
			CurrCmd = WaterCmd_ClearException;		// " 清异常命令 ";
			/*---------------------------------------------*/
			Args.buf[i++] = 0x05;		// 命令字	05
			ackLen = 1;					// 应答长度 1	
			// 数据域
			Args.buf[i++] = 0x00;		// 命令选项 00	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);

		// 应答长度、超时时间、重发次数
		ackLen += 14 + Addrs.itemCnt * AddrLen;
		timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
		tryCnt = 3;

		// 发送、接收、结果显示
		if(false == Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt)){
			if(strncmp(DispBuf, "表号", 4) != 0){	// 命令已取消	
				DispBuf[0] = NULL;
			}
		}
		else{
			// 保存结果到数据库
			SaveMeterReadResult(meterInfo);
		}
		_Printfxy(0, 9*16, "返回            确定", Color_White);
		
		while(3){
			key = ShowScrollStr(&DispBuf, 7);
		
			// 返回 / 确定 
			if (key == KEY_CANCEL || key == KEY_ENTER){
				break;
			}else{
				continue;
			}
		}

		if(key == KEY_CANCEL){	// 返回
			break;
		}
	}// while 1  户表信息
	

	return key;
}

#endif