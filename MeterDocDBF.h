#ifndef MeterDocDBF_H
#define MeterDocDBF_H

//----------------------	宏定义		------------------------
#define	MeterDocDB 		"jk.dbf"		// 表档案数据库文件名

#define Invalid_dbIdx	0				// 无效索引, 数据库记录序号从1开始

#define District_Max				50	// 最大小区数
#define Building_Max				100	// 一个小区-最大楼栋数
#define Meter_Max					300	// 一栋楼-最大表数

#define	Size_ListStr	20				// 列表字符串长度

#define	Size_DistrictNum			12	// 小区编号 长度	10
#define	Size_DistrictName			50	// 小区名称 长度	50
#define	Size_BuildingNum			12	// 楼栋编号 长度	10
#define	Size_BuildingName			50	// 楼栋名称 长度	50

#define	Size_MeterNum				16	//表号长度 		12
#define	Size_UserNum               	16	//户号长度		32
#define Size_RoomNum              	20	//门牌号长度	16
#define Size_MeterReadStatus        2	//抄表状态长度	1   : 0 - 未抄  1 - 已抄 
#define	Size_UserName               40	//户名长度		50
#define Size_MobileNum           	12	//手机号长度	50
#define	Size_UserAddr               80	//地址长度 		100 
#define Size_MeterReadType			2	//抄表方式长度	1	
#define Size_MeterReadTime          20	//抄表时间长度	20
#define Size_MeterValue             12	//表读数		12
#define Size_MeterStatusHex         8	//表状态Hex长度	8
#define Size_MeterStatusStr         50	//表状态Str长度	50
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
	uint16	errorCode;	// 0 - ok,  其他 - 出错
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
	uint16 	idx;			// 列表项索引
	uint16	cnt;			// 列表项总数
	uint16 	selectField;	// 要显示的字段：表号/户号/门牌号/户名/地址
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
extern uint8 ShowMeterList(MeterListSt *meters);
extern uint8 ShowAutoMeterReading(MeterListSt *meters);
extern void SaveMeterReadResult(MeterInfoSt *meterInfo);
extern void QueryMeterInfo(MeterInfoSt *meterInfo, DbQuerySt *query);
extern uint8 ShowMeterInfo(MeterInfoSt *meterInfo);

#endif