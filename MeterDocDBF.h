#ifndef MeterDocDBF_H
#define MeterDocDBF_H

#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"


//---------------	dbf 查询结构
typedef struct {
	uint8 queryType;	// 查询类型：0-小区列表， 1-楼栋列表； 2-抄表情况列表 , 3 - 户表信息
	char *districNum;	// 小区编号
	char *buildingNum;	// 楼栋编号
	char *meterReadStatus;	// 抄表状态
	char *meterNum;		// 表号
	char *userNum;		// 户号
	char *roomNum;		// 门牌号
	uint32 dbSelectIdx;	// 选择的数据库记录索引

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
	uint32 	dbIdx[District_Max];		// 列表项对应的数据库索引
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
	uint32 	dbIdx[Building_Max];		// 列表项对应的数据库索引
	char 	nums[Building_Max][Size_BuildingNum];		// 列表项字符串：楼栋编号
	char 	names[Building_Max][Size_BuildingName];		// 列表项字符串：楼栋名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	char 	*districNum;	// 小区编号
}BuildingListSt;

//---------------	xx小区 - xx楼栋 - 户表列表（表号/户号/门牌号/户名） 
#define Meter_Max					200	// 一栋楼-最大表数
#define	Size_MeterShowStr			21	// 户表显示的字符串 长度
typedef struct{
	uint32 	dbIdx[Meter_Max];		// 列表项对应的数据库索引
	char 	strs[Meter_Max][Size_MeterShowStr];	// 列表项字符串：表号/户号/门牌号/户名/地址
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
	uint8 	selectField;	// 要显示的字段：表号/户号/门牌号/户名/地址
	char 	*districNum;		// 小区编号
	char 	*buildingNum;		// 楼栋编号
	char 	*meterReadStatus;	// 抄表状态
	uint8 	meterCnt;		// 当前楼栋表总数
	uint8 	readOkCnt;	// 已抄数量
	uint8 	readNgCnt;	// 未抄数量
}MeterListSt;


//---------------	户表信息
#define	Size_MeterNum				16	//表号长度
#define	Size_UserId               	16	//户号长度
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

typedef struct 
{
	uint32	DbIndex;				// 记录在数据库中索引
	char	MeterNum[Size_MeterNum];
	char	UserId[Size_UserId];
	char	RoomNum[Size_RoomNum];
	char	MeterReadStatus[Size_MeterReadStatus];
	char	UserName[Size_UserName];
	char	MobileNum[Size_MobileNum];
	char	UserAddr[Size_UserAddr];
	char	MeterReadType[Size_MeterReadType];
	char	MeterReadTime[Size_MeterReadTime];
	char	MeterValue[Size_MeterValue];
	char	MeterStaus[Size_MeterStaus];
	char	BatteryVoltage[Size_BatteryVoltage];
	char	SignalValue[Size_SignalValue];

}MeterInfoSt;


//----------------------	数据库信息	- 表字段
// 字段索引
typedef enum{
	Idx_Id	= 0,		// "ID",		// ID
	Idx_UserId,			// "HH",		// 户号
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
	Idx_BLZDJ				// "BLZDJ"		// BLZD[A-J] 10个 《《
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
* 描 述：查询小区列表
* 参 数：districts	- 小区列表
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryDistrictList(DistrictListSt *districts, DB_QuerySt *query)
{

}

/*
* 描 述：查询xx小区-楼栋列表
* 参 数：buildings	- 楼栋列表
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryBuildingList(BuildingListSt *buildings, DB_QuerySt *query)
{

}

/*
* 描 述：查询xx小区-xx楼栋-抄表统计情况
* 参 数：meters		- 抄表情况列表
*		 query		- 数据库查询结构
* 返 回：void
*/
void QueryMeterReadCountInfo(MeterListSt *meters, DB_QuerySt *query)
{

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

}

/*
* 描 述：显示户表信息
* 参 数：meterInfo	- 户表信息
* 返 回：uint8 	- 界面退出时的按键值： KEY_CANCEL - 返回键 ， KEY_ENTER - 确认键
*/
uint8 ShowMeterInfo(MeterInfoSt *meterInfo)
{

}


//------------------------		外部接口声明	-------------------------------------

extern MeterInfoSt meterInfo;
extern DistrictListSt distctList;
extern BuildingListSt buildingList;
extern MeterListSt meterList;

#endif