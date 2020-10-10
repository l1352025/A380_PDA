/**
 * 	DBF���ݿ��ļ����� - 6009/8009 ɣ��ͨ�ð� 
*/
#ifndef MeterDocDBF_H
#define MeterDocDBF_H

#include "common.h"

//----------------------	�궨��		------------------------
#define	MeterDocDB 		"jk.dbf"		// �������ݿ��ļ���
#define Use_DBF

#define District_Max				50	// ���С����
#define Building_Max				100	// һ��С��-���¥����
#define Meter_Max					500	// һ��¥-������

#define	Size_DistrictNum			11	// С����� ����	10
#define	Size_DistrictName			51	// С������ ����	50
#define	Size_BuildingNum			11	// ¥����� ����	10
#define	Size_BuildingName			51	// ¥������ ����	50

#define	Size_MeterNum				13	//��ų��� 		12
#define	Size_UserNum               	33	//���ų���		32
#define Size_RoomNum              	17	//���ƺų���	16
#define Size_MeterReadStatus        2	//����״̬����	1   : 0 - δ��  1 - �ɹ���2 - ʧ�� 
#define Size_ValveStatus        	2	//�Զ��巧��״̬���� 1 : 0/1/2 - ��/��/δ֪
#define	Size_UserName               50	//��������		50
#define Size_UserMobileNum          12	//�ֻ��ų���	50
#define	Size_UserAddr               100	//��ַ���� 		100 
#define Size_MeterReadType			2	//����ʽ����	1	
#define Size_MeterReadTime          21	//����ʱ�䳤��	20
#define Size_MeterValue             13	//�����		12
#define Size_MeterStatusHex         8	//��״̬Hex����	8
#define Size_MeterStatusStr         50	//��״̬Str����	50
#define Size_BatteryVoltage         6   //��ص�ѹ����	5
#define Size_SignalValue           	10 	//�ź�ǿ�ȳ���	10

//----------------------	���ݿ���Ϣ	- ���ֶ�
// �ֶ�����
typedef enum{
	Idx_Id	= 0,		// "ID",		// ID
	Idx_UserNum,		// "HH",		// ����
	Idx_UserName,		// "HM",		// ����
	Idx_UserAddr,		// "DZ",		// ��ַ
	Idx_UserRoomNum,	// "MPH",		// ���ƺ�
	Idx_UserTelNum,		// "TEL",		// �绰
	Idx_UserMobileNum,	// "MOBILE",	// �ֻ�
	Idx_AreaNum,			// "CBQY",		// ��������
	Idx_CollectorNum,		// "ZDBH",		// �ɼ��� ���
	Idx_CollectorName,		// "ZDMC",		// �ɼ��� ����
	Idx_CollectorSite,		// "ZDDZ",		// �ɼ��� λ��
	Idx_DistrictNum,		// "CBYJFZBH",		// ����С�� ���
	Idx_DistrictName,		// "CBYJFZMC",		// ����С�� ����
	Idx_BuildingNum,		// "CBEJFZBH",		// ����¥�� ���
	Idx_BuildingName,		// "CBEJFZMC",		// ����¥�� ����
	Idx_MeterNum,			// "BH",		// ���
	Idx_FLXS,				// "FLXS",		// FLϵ����0~10
	Idx_MeterValue,			// "FLA",		// 		FL[A->] �����	// set
	Idx_FLB,				// "FLB",		// 
	Idx_FLC,				// "FLC",		// 
	Idx_FLD,				// "FLD",		// 
	Idx_FLE,				// "FLE",		// 
	Idx_FLF,				// "FLF",		// 
	Idx_FLG,				// "FLG",		// 
	Idx_FLH,				// "FLH",		// 
	Idx_FLI,				// "FLI",		// 
	Idx_FLJ,				// "FLJ",		// 		FL[->J] 
	Idx_LYXS,				// "LYXS",		// LYϵ����0~10
	Idx_LYA,				// "LYA",		// 		LY[A->]
	Idx_LYB,				// "LYB",		// 
	Idx_LYC,				// "LYC",		// 
	Idx_LYD,				// "LYD",		// 
	Idx_LYE,				// "LYE",		// 
	Idx_LYF,				// "LYF",		// 
	Idx_LYG,				// "LYG",		// 
	Idx_LYH,				// "LYH",		// 
	Idx_LYI,				// "LYI",		// 
	Idx_LYJ,				// "LYJ",		// 		LY[->J] 
	Idx_BLXHEX,				// "BLXHEX",	// ������ HEX		
	Idx_BLXSTR,				// "BLXSTR",	// ������ STR		
	Idx_MeterStatusHex,		// "BZTHEX",	// ��״̬ HEX		// set
	Idx_MeterStatusStr,		// "BZTSTR",	// ��״̬ STR		// set
	Idx_BatteryVoltage,		// "DCDY",		// ��ص�ѹ			// set
	Idx_MCCS,				// "MCCS",		// ���峣��
	Idx_MeterReadType,		// "SGCQ",		// ����ʽ 0/1/2 - �ƻ�/������/����	// set
	Idx_QF,					// "QF",		// Ƿ�ѣ�
	Idx_SignalValue,		// "XHQD",		// �ź�ǿ��			// set
	Idx_MeterReadTime,		// "CBSJ",		// ����ʱ��			// set
	Idx_MeterReadStatus,	// "CBZT",		// ����״̬ 0/1/2 - δ��/�ɹ�/ʧ��	// set
	Idx_ValveStatus,		// "BLZDA",		// BLZD[A->]	����������״̬��0/1/2 - ��/��/δ֪��
	Idx_BLZDB,				// "BLZDB",		// 
	Idx_BLZDC,				// "BLZDC",		// ������ ���
	Idx_BLZDD,				// "BLZDD",		// ������ ����
	Idx_BLZDE,				// "BLZDE",		// 
	Idx_BLZDF,				// "BLZDF",		// 
	Idx_BLZDG,				// "BLZDG",		// 
	Idx_BLZDH,				// "BLZDH",		// 
	Idx_BLZDI,				// "BLZDI",		// 
	Idx_BLZDJ,				// "BLZDJ"		// BLZD[->J]
	Idx_Invalid	= 0xFF	// ��Ч�ֶ�	
}DB_Field_Index;

// �ֶ���
const char *Fields[] = {
	"ID",		// ID
	"HH",		// ����
	"HM",		// ����
	"DZ",		// ��ַ
	"MPH",		// ���ƺ�
	"TEL",		// �绰
	"MOBILE",	// �ֻ�
	"CBQY",		// ��������
	"ZDBH",		// �ɼ��� ���
	"ZDMC",		// �ɼ��� ����
	"ZDDZ",		// �ɼ��� λ��
	"CBYJFZBH",		// ����С�� ���
	"CBYJFZMC",		// ����С�� ����
	"CBEJFZBH",		// ����¥�� ���
	"CBEJFZMC",		// ����¥�� ����
	"BH",		// ���
	"FLXS",		// 
	"FLA",		// ����� FL[A-J] 10�� ����
	"FLB",		// 
	"FLC",		// 
	"FLD",		// 
	"FLE",		// 
	"FLF",		// 
	"FLG",		// 
	"FLH",		// 
	"FLI",		// 
	"FLJ",		// ����� FL[A-J] 10�� ����
	"LYXS",		// 
	"LYA",		// LY[A-J] 10�� >>
	"LYB",		// 
	"LYC",		// 
	"LYD",		// 
	"LYE",		// 
	"LYF",		// 
	"LYG",		// 
	"LYH",		// 
	"LYI",		// 
	"LYJ",		// LY[A-J] 10�� <<
	"BLXHEX",	// 
	"BLXSTR",	// 
	"BZTHEX",	// ��״̬ HEX
	"BZTSTR",	// ��״̬ STR
	"DCDY",		// ��ص�ѹ
	"MCCS",		// 
	"SGCQ",		// ����ʽ ��
	"QF",		// 
	"XHQD",		// �ź�ǿ��
	"CBSJ",		// ����ʱ��
	"CBZT",		// ����״̬
	"BLZDA",		// BLZD[A-J] 10�� ����
	"BLZDB",		// 
	"BLZDC",		// ������ ���
	"BLZDD",		// ������ ����
	"BLZDE",		// 
	"BLZDF",		// 
	"BLZDG",		// 
	"BLZDH",		// 
	"BLZDI",		// 
	"BLZDJ"			// BLZD[A-J] 10�� ����
};

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
	char 	nums[District_Max][Size_ListStr];	// �б����ַ�����С�����
	char 	names[District_Max][Size_ListStr];	// �б����ַ�����С������
	uint8 	idx;			// �б�������
	uint8	cnt;			// �б�������
}DistrictListSt;

//--------------	xxС�� - ¥���б�
typedef struct{
	char 	nums[Building_Max][Size_ListStr];		// �б����ַ�����¥�����
	char 	names[Building_Max][Size_ListStr];		// �б����ַ�����¥������
	uint8 	idx;			// �б�������
	uint8	cnt;			// �б�������
	char 	*qryDistricNum;	// С�����
}BuildingListSt;

//---------------	xxС�� - xx¥�� - �����б����/����/���ƺ�/������ 
typedef struct{
	uint32 	dbIdx[Meter_Max];				// �б����Ӧ�����ݿ�����
	char 	strs[Meter_Max][Size_ListStr];	// �б����ַ��������/����/���ƺ�/����/��ַ
	uint16 	idx;			// �б�������
	uint16	cnt;			// �б�������
	uint16 	selectField;	// Ҫ��ʾ���ֶΣ����/����/���ƺ�/����/��ַ
	char 	*qryDistricNum;			// С����ţ���ֵ��ʾ����
	char 	*qryBuildingNum;		// ¥����ţ���ֵ��ʾ����
	char 	*qryMeterReadStatus;	// ����״̬
	char	*qryKeyWord;			// ��ѯ�Ĺؼ���
	char 	districName[Size_DistrictName];		// С����
	char 	buildingName[Size_BuildingName];	// ¥����
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
	char	signalValue[Size_SignalValue];
}MeterInfoSt;


//------------------------		�ⲿ�ӿ�����	-------------------------------------
extern MeterInfoSt MeterInfo;
extern DistrictListSt Districts;
extern BuildingListSt Buildings;
extern MeterListSt Meters;
extern DbQuerySt DbQuery;
extern uint8 **MetersStrs;

extern void QueryDistrictList(DistrictListSt *districts, DbQuerySt *query);
extern void QueryBuildingList(BuildingListSt *buildings, DbQuerySt *query);
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

extern void MainFuncBatchMeterReading(void);

#endif