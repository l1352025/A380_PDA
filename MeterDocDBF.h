#ifndef MeterDocDBF_H
#define MeterDocDBF_H

#include "HJLIB.H"
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"

//---------------------- 小区列表
#define District_Max				20	// 最大小区数
#define	Size_DistrictNum			16	// 小区编号 长度
#define	Size_DistrictName			21	// 小区名称 长度
typedef struct{
	char nums[District_Max][Size_DistrictNum];
	char names[District_Max][Size_DistrictName];
	uint8 cnt;
	uint8 idx;
}DistrictList;

//---------------------- xx小区 - 楼栋列表
#define Building_Max				50	// 一个小区-最大楼栋数
#define	Size_BuildingNum			16	// 楼栋编号 长度
#define	Size_BuildingName			21	// 楼栋名称 长度
typedef struct{
	char nums[Building_Max][Size_BuildingNum];
	char names[Building_Max][Size_BuildingName];
	uint8 cnt;
	uint8 idx;
}BuildingList;

//---------------------- xx楼栋 - 设备列表（表号/户号/门牌号/户名） 
#define Dev_Max					256	// 一栋楼-最大表数
#define	Size_DevShowStr			21	// 楼栋名称 长度
typedef struct{
	uint32 dbIndex[Dev_Max];
	char strs[Dev_Max][Size_DevShowStr];
	uint8 cnt;
	uint8 idx;
}DevList;


//---------------------- 户表信息
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

}MeterInfo;


//---------------------- 数据库信息		- 表字段
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

//---------------------- 数据库信息		- 查询条件	-------------------------------------



//------------------------		外部接口	-------------------------------------

/*
* 描  述：显示户表信息界面 
* 参  数：meterInfo	- 户表信息结构
* 返回值：uint8  - 界面退出时的按键值：确认键，取消键
*/
uint8 ShowMeterInfoUI(MeterInfo *meterInfo)
{

}

extern MeterInfo meterInfo;
extern DistrictList distctList;
extern BuildingList buildingList;
extern DevList devList;

#endif