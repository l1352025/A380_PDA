/**
 * 	DBF数据库文件操作 - 6009/8009 和龙定制版 
*/
#ifndef MeterDocDBF_HL_H
#define MeterDocDBF_HL_H

#include "common.h"

//----------------------	宏定义		------------------------
#define	MeterDocDB 		"userdata.dbf"	// 表档案数据库文件名
#define Use_DBF

#define District_Max			50		// 最大小区数
#define Building_Max			100		// 一个小区 - 最大楼栋数
#define Meter_Max				500		// 一栋楼 - 最大表数
#define Uint_Max				50		// 一栋楼 - 最大单元数


//----------------------	数据库信息	- 表字段
// 字段索引
typedef enum{
	Idx_UserNum	 = 0,	// 户号			"HH"	char(20)
	Idx_UserName,		// 户名			"HM"	char(50)
	Idx_UserAddr,		// 地址			"DZ"	char(100)
	Idx_CollectorNum,	// 采集器编号	"CJQBH"	char(10) 
	Idx_CollectorSite,	// 采集器位置	"CJQWZ"	char(50) 
	Idx_MeterNum,		// 表号			"BH"	char(10)
	Idx_DistrictName,	// 小区			"XQ"	char(32) 
	Idx_BuildingName,	// 楼栋			"LD"	char(32) 
	Idx_UnitName,		// 单元			"DY"	char(32) 
	Idx_UserRoomNum,	// 门牌号		"MPH"	char(20)
	Idx_LastReadVal,		// 上期表码		"SQBM"	number(8)	
	Idx_CurrReadVal,		// 本期表码		"BQBM"	number(8)	// set	= 读数的整数部分
	Idx_MeterValue,			// 电子读数		"DZDS"	number(8,2)	// set	= 读数的整数和小数部分
	Idx_CurrUseVal,			// 本期用量		"BQYL"	number(8)	// set	= 本期表码 - 上期表码
	Idx_MeterReadType,		// 抄表方式		"FS"	char(1)		// set  0/1 - 无线抄表/手工输入
	Idx_MeterReadStatus,	// 抄表状态		"ZT"	char(1)		// set  0/1 - 未抄/已抄
	Idx_MeterStatusHex,		// 表状态HEX	"SHEX"	char(8)		// set	
	Idx_MeterStatusStr,		// 表状态STR	"SSTR"	char(32)	// set	报警状态：xx
	Idx_BatteryVoltage,		// 电池电压		"DCDY"	char(4)		// set
	Idx_ValveStatus,		// 阀门状态		"FMZT"	char(1)		// set
	Idx_MeterReadTime,		// 抄表时间		"CBSJ"	char(20)	// set
	Idx_UnitPrice,			// 单价			"DJ"		number(8,2)
	Idx_DepositBalance,		// 预存余额		"YCYE"		number(8,2)			
	Idx_OwedFeeAmount,		// 欠费金额		"QFJE"		number(8,2)			
	Idx_WaterFeeAmount,		// 水费金额		"SFJE"		number(8,2)	// set	= 单价*本期用量	
	Idx_OwedFeeStatus,		// 欠费状态		"QF"		char(1)		// set	0/1 - 正常/欠费	 （欠费状态：预存余额 < 欠费金额 + 水费金额）
	Idx_UrgeFeeStatus,		// 催费状态		"CF"		char(1)	 	// set  0/1 - 正常/催费
	Idx_FILENAME,			// dbf文件名	"FILENAME"	char(32)			
	Idx_ReaderName,			// 抄表员		"CBY"		char(8)		
	Idx_WaterSupply2nd,		// 二次供水		"FJFY"		number(8,2)		
	Idx_UserMobileNum,		// 电话号码		"PHONE"		char(20)		
	
	Idx_Invalid	= 0xFF	// 无效字段	
}DB_Field_Index;

// 字段大小
#define Size_UserNum		21	// 户号			"HH"	char(20)
#define Size_UserName		51	// 户名			"HM"	char(50)
#define Size_UserAddr		101	// 地址			"DZ"	char(100)
#define Size_CollectorNum	11	// 采集器编号	"CJQBH"	char(10) 
#define Size_CollectorSite	51	// 采集器位置	"CJQWZ"	char(50) 
#define Size_MeterNum		11	// 表号			"BH"	char(10)
#define Size_DistrictName	33	// 小区			"XQ"	char(32) 
#define Size_BuildingName	33	// 楼栋			"LD"	char(32) 
#define Size_UnitName		33	// 单元			"DY"	char(32) 
#define Size_RoomNum		21	// 门牌号		"MPH"	char(20)
#define Size_LastReadVal		9	// 上期表码		"SQBM"	number(8)
#define Size_CurrReadVal		9	// 本期表码		"BQBM"	number(8)
#define Size_MeterValue			9	// 电子读数		"DZDS"	number(8,2)	// set
#define Size_CurrUseVal			9	// 本期用量		"BQYL"	number(8)	// set
#define Size_MeterReadType		2	// 抄表方式		"FS"	char(1)		// set
#define Size_MeterReadStatus	2	// 抄表状态		"ZT"	char(1)		// set
#define Size_MeterStatusHex		9	// 表状态HEX	"SHEX"	char(8)		// set
#define Size_MeterStatusStr		33	// 表状态STR	"SSTR"	char(32)	// set
#define Size_BatteryVoltage		5	// 电池电压		"DCDY"	char(4)		// set
#define Size_ValveStatus		2	// 阀门状态		"FMZT"	char(1)		// set
#define Size_MeterReadTime		21	// 抄表时间		"CBSJ"	char(20)	// set
#define Size_UnitPrice			9	// 单价			"DJ"		number(8,2)
#define Size_DepositBalance		9	// 预存余额		"YCYE"		number(8,2)			
#define Size_OwedFeeAmount		9	// 欠费金额		"QFJE"		number(8,2)			
#define Size_WaterFeeAmount		9	// 水费金额		"SFJE"		number(8,2)	// set		
#define Size_OwedFeeStatus		2	// 欠费状态		"QF"		char(1)		// set	
#define Size_UrgeFeeStatus		2	// 催费状态		"CF"		char(1)		// set
#define Size_FILENAME			33	// dbf文件名	"FILENAME"	char(32)			
#define Size_ReaderName			9	// 抄表员		"CBY"		char(8)		
#define Size_WaterSupply2nd		9	// 二次供水		"FJFY"		number(8,2)		
#define Size_UserMobileNum		21	// 电话号码		"PHONE"		char(20)	

#define Size_ShouldPayAmount	9	// 应缴金额（数据库无该字段，由计算所得）	


//---------------	dbf 查询结构
#ifndef DbQueryOptimize_H
typedef struct {

	uint32	dbCurrIdx;	// 数据库当前位置
	uint32	reqMaxCnt;	// 最大请求数
	uint32	resultCnt;	// 查询的结果记录数
	uint16	errorCode;	// 0 - ok,  其他 - 出错
}DbQuerySt;
#endif

//---------------	小区列表
typedef struct{
	char 	nums[District_Max][Size_ListStr];	// 列表项字符串：小区编号/名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
}DistrictListSt;

//--------------	xx小区 - 楼栋列表
typedef struct{
	char 	nums[Building_Max][Size_ListStr];		// 列表项字符串：楼栋编号/名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	char 	*qryDistricNum;	// 小区编号/名称
}BuildingListSt;

//--------------	xx小区 - xx楼栋 - 单元列表
typedef struct{
	char 	nums[Uint_Max][Size_ListStr];		// 列表项字符串：单元编号/名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	char 	*qryDistricNum;			// 小区编号/名称
	char 	*qryBuildingNum;		// 楼栋编号/名称
}UnitListSt;

//---------------	xx小区 - xx楼栋 - 户表列表（表号/户号/门牌号/户名） 
typedef struct{
	uint32 	dbIdx[Meter_Max];				// 列表项对应的数据库索引
	char 	strs[Meter_Max][Size_ListStr];	// 列表项字符串：表号/户号/门牌号/户名/地址
	uint16 	idx;			// 列表项索引
	uint16	cnt;			// 列表项总数
	uint16 	selectField;	// 要显示的字段：表号/户号/门牌号/户名/地址
	char 	*qryDistricNum;			// 小区编号/名称：空值表示所有
	char 	*qryBuildingNum;		// 楼栋编号/名称：空值表示所有
	char 	*qryUnitNum;			// 单元编号/名称：空值表示所有
	char 	*qryMeterReadStatus;	// 抄表状态
	char	*qryKeyWord;			// 查询的关键字
	char 	districName[Size_DistrictName];		// 小区名
	char 	buildingName[Size_BuildingName];	// 楼栋名
	char 	unitName[Size_UnitName];			// 单元名
	uint16 	meterCnt;		// 当前表总数：未抄+成功+失败
	uint16 	readOkCnt;		// 成功数量
	uint16 	readNgCnt;		// 失败数量
}MeterListSt;


//---------------	户表信息
typedef struct 
{
	uint32	dbIdx;		// 当前记录在数据库中索引
	uint8 	strIdx;		// 当前记录在列表项的索引
	uint8	strCnt;		// 当前记录所在列表项总数	
	char	*qryMeterNum;		// 表号
	char 	*qryUserNum;		// 户号
	char 	*qryRoomNum;		// 门牌号

	char	meterNum[Size_MeterNum];
	char	userNum[Size_UserNum];
	char	roomNum[Size_RoomNum];
	char	meterReadStatus[Size_MeterReadStatus];
	char	userName[Size_UserName];
	char	mobileNum[Size_UserMobileNum];
	char	userAddr[Size_UserAddr];
	char	meterReadType[Size_MeterReadType];
	char	meterReadTime[Size_MeterReadTime];
	char	meterValue[Size_MeterValue];
	char	meterStatusHex[Size_MeterStatusHex];
	char	meterStatusStr[Size_MeterStatusStr];
	char	batteryVoltage[Size_BatteryVoltage];

	char	valveStatus[Size_ValveStatus];
	char	lastReadVal[Size_LastReadVal];
	char	currReadVal[Size_CurrReadVal];
	char	currUseVal[Size_CurrUseVal];
	char	waterSupply2nd[Size_WaterSupply2nd];
	char	unitPrice[Size_UnitPrice];
	char	depositBanlance[Size_DepositBalance];
	char	OwedFeeAmount[Size_OwedFeeAmount];
	char	waterFeeAmount[Size_WaterFeeAmount];
	char	shouldPayAmount[Size_ShouldPayAmount];
	char	owedFeeStatus[Size_OwedFeeStatus];
	char	urgeFeeStatus[Size_UrgeFeeStatus];
	char	readerName[Size_ReaderName];
}MeterInfoSt;


//------------------------		外部接口声明	-------------------------------------
extern MeterInfoSt MeterInfo;
extern DistrictListSt Districts;
extern BuildingListSt Buildings;
extern UnitListSt Units;
extern MeterListSt Meters;
extern DbQuerySt DbQuery;
extern uint8 **MetersStrs;

extern void QueryDistrictList(DistrictListSt *districts, DbQuerySt *query);
extern void QueryBuildingList(BuildingListSt *buildings, DbQuerySt *query);
extern void QueryMeterList(MeterListSt *meters, DbQuerySt *query);
extern void QueryMeterListByKeyword(MeterListSt *meters, DbQuerySt *query);
extern void QueryUnitList(UnitListSt *units, DbQuerySt *query);
extern uint8 ShowMeterReadCountInfo(MeterListSt *meters);
extern uint8 ShowUnitList(UnitListSt *units, ListBoxEx *unitList);
extern uint8 ShowMeterList(MeterListSt *meters);
extern void ShowSettingRoutes(void);
extern uint8 ShowAutoMeterReading(MeterListSt *meters);
extern void SaveMeterReadResult(MeterInfoSt *meterInfo, uint8 readType, uint8 readStatus);
extern void QueryMeterInfo(MeterInfoSt *meterInfo, DbQuerySt *query);
extern uint8 ShowMeterInfo(MeterInfoSt *meterInfo);
extern uint32 FixDbfRecCnt(void);

extern void MainFuncBatchMeterReading(void);

#endif