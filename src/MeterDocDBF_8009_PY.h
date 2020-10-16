/**
 * 	DBF数据库文件操作 - 8009濮阳定制版 
*/
#ifndef MeterDocDBF_8009_PY_H
#define MeterDocDBF_8009_PY_H

#include "common.h"

//----------------------	宏定义		------------------------
#define	MeterDocDB			"MRDATA.DBF"	// 表档案数据库文件名
#define Use_DBF

#define Sect_Max			50				// 最大抄表册数量
#define Meter_Max			1600			// 单个抄表册-最大表数

//----------------------	数据库信息 - MRDATA.DBF 表字段
// 字段索引
typedef enum _db_field_index{
	Idx_MtrReadId = 0,	// 抄表数据标识	"MR_ID"		char(16) 
	Idx_AppNum,			// app请求编号	"APP_NO"	char(16) 
	Idx_SectNum,		// 抄表册编号	"SECT_NO"	char(16) 
	Idx_SectName,		// 抄表册名称	"SECT_NAME"	char(254) 
	Idx_MtrReadSn,		// 抄表顺序号	"MR_SN"		number(5) 
	Idx_ReaderNum,		// 抄表员工号	"READER_NO"	char(16)
	Idx_ReaderName,		// 抄表员名称	"READER"	char(16)
	Idx_OrgNum,			// 管理单位		"ORG_NO"	char(16)
	Idx_UserNum,		// 用户编号		"CONS_NO"	char(16)
	Idx_UserName,		// 用户名称		"CONS_NAME"	char(254)
	Idx_UserAddr,		// 用户地址		"CONS_ADDR"	char(254)
	Idx_PlanMrDate,		// 计划抄表日期	"PL_MR_DATE"	char(8)
	Idx_MeterId,		// 燃气表标识	"METER_ID"		char(16)
	Idx_MeterType,		// 燃气表类型	"APPLY_TYPE"	char(8)
	Idx_MeterNum,		// 出厂编号		"FAC_NUMBER"	char(32)
	Idx_PriceCode,		// 气价码		"PRC_CODE"		char(16)
	Idx_TempFactor,		// 温度补偿系数	"TP_FACTOR"		number(15，6)
	Idx_LastMrDate,		// 上次抄表日期	"LAST_DATE"		char(8)
	Idx_LastReadVal,	// 上次读数		"LAST_READ"		number(15,6)
	Idx_LastGasVol,		// 上次气量		"LAST_MR_GQ"	number(15,6)
	Idx_AvgGasVol,		// 平均气量		"AVG_MR_GQ"		number(15,6)
	Idx_MeterReadStatus,	// 抄表状态		"MR_STATUS"		char(8)			// set
	Idx_MeterReadType,		// 抄表性质		"MR_TYPE"		char(8)			// set
	Idx_MeterReadDate,		// 实际抄表日期	"THIS_YMD"		char(8)			// set
	Idx_CurrReadVal,		// 本次读数		"THIS_READ"		number(15,6)	// set
	Idx_CurrGasVol,			// 本次气量		"THIS_GQ"		number(15,6)	// set
	Idx_MrExcepType,		// 读数异常类型	"EXCP_TYPE"		char(8)			// set
	Idx_LastElecReadVal,	// 上次电子读数	"LEM_READ"		number(15,6)
	Idx_CurrElecReadVal,	// 本次电子读数	"EM_READ"		number(15,6)	// set	
	Idx_LastRemainGasVol,	// 上次剩余气量	"LR_GAS"		number(15,6)
	Idx_CurrRemainGasVol,	// 本次剩余气量	"REMAIN_GAS"	number(15,6)	// set
	Idx_MeterDigit,		// 表位数		"MR_DIGIT"	number(12,2)
	Idx_UserBalance,	// 用户余额		"BLA"		number(15,6)
	Idx_RemarkA,		// 扩展字段A	"REMARK_A"		char(32)
	Idx_RemarkA_V,		// 扩展字段A_V	"REMARK_A_V"	char(128)
	Idx_RemarkB,		// 扩展字段B	"REMARK_B"		char(32)
	Idx_RemarkB_V,		// 扩展字段B_V	"REMARK_B_V"	char(128)
	Idx_RemarkC,		// 扩展字段C	"REMARK_C"		char(32)
	Idx_RemarkC_V,		// 扩展字段C_V	"REMARK_C_V"	char(128)
	Idx_RemarkD,		// 扩展字段D	"REMARK_D"		char(32)
	Idx_RemarkD_V,		// 扩展字段D_V	"REMARK_D_V"	char(128)
	Idx_RemarkE,		// 扩展字段E	"REMARK_E"		char(32)
	Idx_RemarkE_V,		// 扩展字段E_V	"REMARK_E_V"	char(128)
	Idx_Invalid	= 0xFF	// 无效字段	
}DB_Field_Index;

// 字段大小
#define Size_MtrReadId		17		// 抄表数据标识	"MR_ID"		char(16) 
#define Size_AppNum			17		// app请求编号	"APP_NO"	char(16) 
#define Size_SectNum		17		// 抄表册编号	"SECT_NO"	char(16) 
#define Size_SectName		255		// 抄表册名称	"SECT_NAME"	char(254) 
#define Size_MtrReadSn		6		// 抄表顺序号	"MR_SN"		number(5) 
#define Size_ReaderNum		17		// 抄表员工号	"READER_NO"	char(16)
#define Size_ReaderName		17		// 抄表员名称	"READER"	char(16)
#define Size_OrgNum			17		// 管理单位		"ORG_NO"	char(16)
#define Size_UserNum		17		// 用户编号		"CONS_NO"	char(16)
#define Size_UserName		255		// 用户名称		"CONS_NAME"	char(254)
#define Size_UserAddr		255		// 用户地址		"CONS_ADDR"	char(254)
#define Size_PlanMrDate		9		// 计划抄表日期	"PL_MR_DATE"	char(8)
#define Size_MeterId		17		// 燃气表标识	"METER_ID"		char(16)
#define Size_MeterType		9		// 燃气表类型	"APPLY_TYPE"	char(8)
#define Size_MeterNum		33		// 出厂编号		"FAC_NUMBER"	char(32)
#define Size_PriceCode		17		// 气价码		"PRC_CODE"		char(16)
#define Size_TempFactor		16		// 温度补偿系数	"TP_FACTOR"		number(15,6)
#define Size_LastMrDate		9		// 上次抄表日期	"LAST_DATE"		char(8)
#define Size_LastReadVal	16		// 上次读数		"LAST_READ"		number(15,6)
#define Size_LastGasVol		16		// 上次气量		"LAST_MR_GQ"	number(15,6)
#define Size_AvgGasVol		16		// 平均气量		"AVG_MR_GQ"		number(15,6)
#define Size_MeterReadStatus	9	// 抄表状态		"MR_STATUS"		char(8)			// set
#define Size_MeterReadType		9	// 抄表性质		"MR_TYPE"		char(8)			// set
#define Size_MeterReadDate		9	// 实际抄表日期	"THIS_YMD"		char(8)			// set
#define Size_CurrReadVal		16	// 本次读数		"THIS_READ"		number(15,6)	// set
#define Size_CurrGasVol			16	// 本次气量		"THIS_GQ"		number(15,6)	// set
#define Size_MrExcepType		9	// 读数异常类型	"EXCP_TYPE"		char(8)			// set
#define Size_LastElecReadVal	16	// 上次电子读数	"LEM_READ"		number(15,6)
#define Size_CurrElecReadVal	16	// 本次电子读数	"EM_READ"		number(15,6)	// set	
#define Size_LastRemainGasVol	16	// 上次剩余气量	"LR_GAS"		number(15,6)
#define Size_CurrRemainGasVol	16	// 本次剩余气量	"REMAIN_GAS"	number(15,6)	// set
#define Size_MeterDigit		13		// 表位数		"MR_DIGIT"	number(12,2)
#define Size_UserBalance	16		// 用户余额		"BLA"		number(15,6)
#define Size_RemarkA		33		// 扩展字段A	"REMARK_A"		char(32)
#define Size_RemarkA_V		129		// 扩展字段A_V	"REMARK_A_V"	char(128)
#define Size_RemarkB		33		// 扩展字段B	"REMARK_B"		char(32)
#define Size_RemarkB_V		129		// 扩展字段B_V	"REMARK_B_V"	char(128)
#define Size_RemarkC		33		// 扩展字段C	"REMARK_C"		char(32)
#define Size_RemarkC_V		129		// 扩展字段C_V	"REMARK_C_V"	char(128)
#define Size_RemarkD		33		// 扩展字段D	"REMARK_D"		char(32)
#define Size_RemarkD_V		129		// 扩展字段D_V	"REMARK_D_V"	char(128)
#define Size_RemarkE		33		// 扩展字段E	"REMARK_E"		char(32)
#define Size_RemarkE_V		129		// 扩展字段E_V	"REMARK_E_V"	char(128)


//---------------	dbf 查询结构
typedef struct {
	uint32	dbCurrIdx;	// 数据库当前位置
	uint32	reqMaxCnt;	// 最大请求数
	uint32	resultCnt;	// 查询的结果记录数
	uint16	errorCode;	// 0 - ok,  其他 - 出错
}DbQuerySt;

//---------------	抄表册列表
typedef struct{
	char 	nums[Sect_Max][Size_ListStr];	// 列表项字符串：抄表册编号
	//char 	names[Sect_Max][Size_ListStr];	// 列表项字符串：抄表册名称
	uint8 	idx;			// 列表项索引
	uint8	cnt;			// 列表项总数
}SectListSt;

//---------------	xx抄表册 - 户表列表（表号/户号/户名/地址） 
typedef struct{
	uint32 	dbIdx[Meter_Max];				// 列表项对应的数据库索引
	char 	strs[Meter_Max][Size_ListStr];	// 列表项字符串：表号/户号/户名/地址
	uint16 	idx;			// 列表项索引
	uint16	cnt;			// 列表项总数
	uint16 	selectField;	// 要显示的字段：表号/户号/地址/户名/地址
	char 	*qrySectNum;			// 抄表册编号：空值表示所有
	uint8 	qryMeterReadStatus;		// 抄表状态
	char	*qryKeyWord;			// 查询的关键字
	char 	sectNum[Size_SectNum];			// 抄表册编号
	char 	sectName[Size_SectName];		// 抄表册名称
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
	char	*qryUserNum;		// 户号
	char 	*qryUserName;		// 户名
	char 	*qryUserAddr;		// 地址
	
	char	userNum[Size_UserNum];		// 用户编号
	char	userName[Size_UserName];	// 用户名称
	char	userAddr[Size_UserAddr];	// 用户地址
	char	meterNum[Size_MeterNum];	// 表号
	char	meterReadStatus[Size_MeterReadStatus];	// 抄表状态
	char	meterReadTime[Size_MeterReadDate];		// 抄表日期
	char	meterReadType[Size_MeterReadType];		// 抄表性质
	char	meterExcpType[Size_MrExcepType];		// 读数异常类型
	char	currGasVol[Size_CurrGasVol];				// 本次气量
	char	currRemainGasVol[Size_CurrRemainGasVol];	// 本次剩余气量
	char	currReadVal[Size_CurrReadVal];				// 本次读数
	char	currElecReadVal[Size_CurrElecReadVal];		// 本次电子读数

	char	lastGasVol[Size_LastGasVol];				// 上次气量
	char	lastRemainGasVol[Size_LastRemainGasVol];	// 上次剩余气量
	char	lastReadVal[Size_LastReadVal];				// 上次读数
	char	lastElecReadVal[Size_LastElecReadVal];		// 上次电子读数

	char	lastMtrReadTime[Size_LastMrDate];			// 上次抄表日期
	char	userBalance[Size_UserBalance];				// 用户余额
	char	avgGasVol[Size_AvgGasVol];					// 平均气量

	char	sectNum[Size_SectNum];			// 抄表册编号
	char	sectName[Size_SectName];		// 抄表册名称
	char	readerNum[Size_ReaderNum];		// 抄表员编号
	char	readerName[Size_ReaderName];	// 抄表员名称
	char	orgNum[Size_OrgNum];			// 管理单位编号

}MeterInfoSt;

// 读数异常类型
typedef enum{
	MrExcp_Normal		= 1,	// 正常
	MrExcp_Flip,				// 翻转
	MrExcp_Reverse,				// 倒转
	MrExcp_FlipReverse,			// 翻转且倒转
	MrExcp_DataExcp		= 9		// 数据异常
}MtrReadException;

// 抄表性质
typedef enum{
	MrType_BySee		= 1,	// 见抄
	MrType_ByEstimate,			// 估抄
	MrType_ByDoor,				// 门抄
	MrType_UserReport,			// 用户自报
	MrType_EmptyHourse,			// 空房	
	MrType_ZeroGasVol			// 零气量
}MtrReadType;


//------------------------		外部接口声明	-------------------------------------

extern SectListSt Sects;
extern MeterListSt Meters;
extern MeterInfoSt MeterInfo;
extern DbQuerySt DbQuery;
extern uint8 **MetersStrs;

extern void QuerySectList(SectListSt *districts, DbQuerySt *query);
extern void QueryMeterList(MeterListSt *meters, DbQuerySt *query);
extern void QueryMeterListByKeyword(MeterListSt *meters, DbQuerySt *query);
extern uint8 ShowMeterReadCountInfo(MeterListSt *meters);
extern uint8 ShowMeterList(MeterListSt *meters);
extern void ShowSettingRoutes(void);
extern uint8 ShowAutoMeterReading(MeterListSt *meters);
extern void SaveMeterReadResult(MeterInfoSt *meterInfo, uint8 readType, uint8 readStatus);
extern void QueryMeterInfo(MeterInfoSt *meterInfo, DbQuerySt *query);
extern uint8 ShowMeterInfo(MeterInfoSt *meterInfo);
extern uint32 FixDbfRecCnt(void);

#endif