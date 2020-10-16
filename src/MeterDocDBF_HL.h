/**
 * 	DBF���ݿ��ļ����� - 6009/8009 �������ư� 
*/
#ifndef MeterDocDBF_HL_H
#define MeterDocDBF_HL_H

#include "common.h"

//----------------------	�궨��		------------------------
#define	MeterDocDB 		"userdata.dbf"	// �������ݿ��ļ���
#define Use_DBF

#define District_Max			50		// ���С����
#define Building_Max			100		// һ��С�� - ���¥����
#define Meter_Max				500		// һ��¥ - ������
#define Uint_Max				50		// һ��¥ - ���Ԫ��


//----------------------	���ݿ���Ϣ	- ���ֶ�
// �ֶ�����
typedef enum{
	Idx_UserNum	 = 0,	// ����			"HH"	char(20)
	Idx_UserName,		// ����			"HM"	char(50)
	Idx_UserAddr,		// ��ַ			"DZ"	char(100)
	Idx_CollectorNum,	// �ɼ������	"CJQBH"	char(10) 
	Idx_CollectorSite,	// �ɼ���λ��	"CJQWZ"	char(50) 
	Idx_MeterNum,		// ���			"BH"	char(10)
	Idx_DistrictName,	// С��			"XQ"	char(32) 
	Idx_BuildingName,	// ¥��			"LD"	char(32) 
	Idx_UnitName,		// ��Ԫ			"DY"	char(32) 
	Idx_UserRoomNum,	// ���ƺ�		"MPH"	char(20)
	Idx_LastReadVal,		// ���ڱ���		"SQBM"	number(8)	
	Idx_CurrReadVal,		// ���ڱ���		"BQBM"	number(8)	// set	= ��������������
	Idx_MeterValue,			// ���Ӷ���		"DZDS"	number(8,2)	// set	= ������������С������
	Idx_CurrUseVal,			// ��������		"BQYL"	number(8)	// set	= ���ڱ��� - ���ڱ���
	Idx_MeterReadType,		// ����ʽ		"FS"	char(1)		// set  0/1 - ���߳���/�ֹ�����
	Idx_MeterReadStatus,	// ����״̬		"ZT"	char(1)		// set  0/1 - δ��/�ѳ�
	Idx_MeterStatusHex,		// ��״̬HEX	"SHEX"	char(8)		// set	
	Idx_MeterStatusStr,		// ��״̬STR	"SSTR"	char(32)	// set	����״̬��xx
	Idx_BatteryVoltage,		// ��ص�ѹ		"DCDY"	char(4)		// set
	Idx_ValveStatus,		// ����״̬		"FMZT"	char(1)		// set
	Idx_MeterReadTime,		// ����ʱ��		"CBSJ"	char(20)	// set
	Idx_UnitPrice,			// ����			"DJ"		number(8,2)
	Idx_DepositBalance,		// Ԥ�����		"YCYE"		number(8,2)			
	Idx_OwedFeeAmount,		// Ƿ�ѽ��		"QFJE"		number(8,2)			
	Idx_WaterFeeAmount,		// ˮ�ѽ��		"SFJE"		number(8,2)	// set	= ����*��������	
	Idx_OwedFeeStatus,		// Ƿ��״̬		"QF"		char(1)		// set	0/1 - ����/Ƿ��	 ��Ƿ��״̬��Ԥ����� < Ƿ�ѽ�� + ˮ�ѽ�
	Idx_UrgeFeeStatus,		// �߷�״̬		"CF"		char(1)	 	// set  0/1 - ����/�߷�
	Idx_FILENAME,			// dbf�ļ���	"FILENAME"	char(32)			
	Idx_ReaderName,			// ����Ա		"CBY"		char(8)		
	Idx_WaterSupply2nd,		// ���ι�ˮ		"FJFY"		number(8,2)		
	Idx_UserMobileNum,		// �绰����		"PHONE"		char(20)		
	
	Idx_Invalid	= 0xFF	// ��Ч�ֶ�	
}DB_Field_Index;

// �ֶδ�С
#define Size_UserNum		21	// ����			"HH"	char(20)
#define Size_UserName		51	// ����			"HM"	char(50)
#define Size_UserAddr		101	// ��ַ			"DZ"	char(100)
#define Size_CollectorNum	11	// �ɼ������	"CJQBH"	char(10) 
#define Size_CollectorSite	51	// �ɼ���λ��	"CJQWZ"	char(50) 
#ifdef Project_6009_RF_HL
#define Size_MeterNum		13	// ���			"BH"	char(12)	// dbf���ֶγ��ȱ�Ϊ 12
#else // Project_6009_RF_HL
#define Size_MeterNum		11	// ���			"BH"	char(10)
#endif
#define Size_DistrictName	33	// С��			"XQ"	char(32) 
#define Size_BuildingName	33	// ¥��			"LD"	char(32) 
#define Size_UnitName		33	// ��Ԫ			"DY"	char(32) 
#define Size_RoomNum		21	// ���ƺ�		"MPH"	char(20)
#define Size_LastReadVal		9	// ���ڱ���		"SQBM"	number(8)
#define Size_CurrReadVal		9	// ���ڱ���		"BQBM"	number(8)
#define Size_MeterValue			9	// ���Ӷ���		"DZDS"	number(8,2)	// set
#define Size_CurrUseVal			9	// ��������		"BQYL"	number(8)	// set
#define Size_MeterReadType		2	// ����ʽ		"FS"	char(1)		// set
#define Size_MeterReadStatus	2	// ����״̬		"ZT"	char(1)		// set
#define Size_MeterStatusHex		9	// ��״̬HEX	"SHEX"	char(8)		// set
#define Size_MeterStatusStr		33	// ��״̬STR	"SSTR"	char(32)	// set
#define Size_BatteryVoltage		5	// ��ص�ѹ		"DCDY"	char(4)		// set
#define Size_ValveStatus		2	// ����״̬		"FMZT"	char(1)		// set
#define Size_MeterReadTime		21	// ����ʱ��		"CBSJ"	char(20)	// set
#define Size_UnitPrice			9	// ����			"DJ"		number(8,2)
#define Size_DepositBalance		9	// Ԥ�����		"YCYE"		number(8,2)			
#define Size_OwedFeeAmount		9	// Ƿ�ѽ��		"QFJE"		number(8,2)			
#define Size_WaterFeeAmount		9	// ˮ�ѽ��		"SFJE"		number(8,2)	// set		
#define Size_OwedFeeStatus		2	// Ƿ��״̬		"QF"		char(1)		// set	
#define Size_UrgeFeeStatus		2	// �߷�״̬		"CF"		char(1)		// set
#define Size_FILENAME			33	// dbf�ļ���	"FILENAME"	char(32)			
#define Size_ReaderName			9	// ����Ա		"CBY"		char(8)		
#define Size_WaterSupply2nd		9	// ���ι�ˮ		"FJFY"		number(8,2)		
#define Size_UserMobileNum		21	// �绰����		"PHONE"		char(20)	

#define Size_ShouldPayAmount	9	// Ӧ�ɽ����ݿ��޸��ֶΣ��ɼ������ã�	


//---------------	dbf ��ѯ�ṹ
#ifndef DbQueryOptimize_H
typedef struct {

	uint32	dbCurrIdx;	// ���ݿ⵱ǰλ��
	uint32	reqMaxCnt;	// ���������
	uint32	resultCnt;	// ��ѯ�Ľ����¼��
	uint16	errorCode;	// 0 - ok,  ���� - ����
}DbQuerySt;
#endif

//---------------	С���б�
typedef struct{
	char 	nums[District_Max][Size_ListStr];	// �б����ַ�����С�����/����
	uint8 	idx;			// �б�������
	uint8	cnt;			// �б�������
}DistrictListSt;

//--------------	xxС�� - ¥���б�
typedef struct{
	char 	nums[Building_Max][Size_ListStr];		// �б����ַ�����¥�����/����
	uint8 	idx;			// �б�������
	uint8	cnt;			// �б�������
	char 	*qryDistricNum;	// С�����/����
}BuildingListSt;

//--------------	xxС�� - xx¥�� - ��Ԫ�б�
typedef struct{
	char 	nums[Uint_Max][Size_ListStr];		// �б����ַ�������Ԫ���/����
	uint8 	idx;			// �б�������
	uint8	cnt;			// �б�������
	char 	*qryDistricNum;			// С�����/����
	char 	*qryBuildingNum;		// ¥�����/����
}UnitListSt;

//---------------	xxС�� - xx¥�� - �����б����/����/���ƺ�/������ 
typedef struct{
	uint32 	dbIdx[Meter_Max];				// �б����Ӧ�����ݿ�����
	char 	strs[Meter_Max][Size_ListStr];	// �б����ַ��������/����/���ƺ�/����/��ַ
	uint16 	idx;			// �б�������
	uint16	cnt;			// �б�������
	uint16 	selectField;	// Ҫ��ʾ���ֶΣ����/����/���ƺ�/����/��ַ
	uint8 	qryMeterReadStatus;		// ����״̬
	char 	*qryDistricNum;			// С�����/���ƣ���ֵ��ʾ����
	char 	*qryBuildingNum;		// ¥�����/���ƣ���ֵ��ʾ����
	char 	*qryUnitNum;			// ��Ԫ���/���ƣ���ֵ��ʾ����
	char	*qryKeyWord;			// ��ѯ�Ĺؼ���
	char 	districName[Size_DistrictName];		// С����
	char 	buildingName[Size_BuildingName];	// ¥����
	char 	unitName[Size_UnitName];			// ��Ԫ��
	uint16 	meterCnt;		// ��ǰ��������δ��+�ɹ�+ʧ��
	uint16 	readOkCnt;		// �ɹ�����
	uint16 	readNgCnt;		// ʧ������
}MeterListSt;


//---------------	������Ϣ
typedef struct 
{
	uint32	dbIdx;		// ��ǰ��¼�����ݿ�������
	uint8 	strIdx;		// ��ǰ��¼���б��������
	uint8	strCnt;		// ��ǰ��¼�����б�������	
	char	*qryMeterNum;		// ���
	char 	*qryUserNum;		// ����
	char 	*qryRoomNum;		// ���ƺ�

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


//------------------------		�ⲿ�ӿ�����	-------------------------------------
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