/**
 * 	DBF���ݿ��ļ����� - 8009������ư� 
*/
#ifndef MeterDocDBF_8009_PY_H
#define MeterDocDBF_8009_PY_H

#include "common.h"

//----------------------	�궨��		------------------------
#define	MeterDocDB			"MRDATA.DBF"	// �������ݿ��ļ���
#define Use_DBF

#define Sect_Max			50				// ��󳭱������
#define Meter_Max			1600			// ���������-������

//----------------------	���ݿ���Ϣ - MRDATA.DBF ���ֶ�
// �ֶ�����
typedef enum _db_field_index{
	Idx_MtrReadId = 0,	// �������ݱ�ʶ	"MR_ID"		char(16) 
	Idx_AppNum,			// app������	"APP_NO"	char(16) 
	Idx_SectNum,		// �������	"SECT_NO"	char(16) 
	Idx_SectName,		// ���������	"SECT_NAME"	char(254) 
	Idx_MtrReadSn,		// ����˳���	"MR_SN"		number(5) 
	Idx_ReaderNum,		// ����Ա����	"READER_NO"	char(16)
	Idx_ReaderName,		// ����Ա����	"READER"	char(16)
	Idx_OrgNum,			// ����λ		"ORG_NO"	char(16)
	Idx_UserNum,		// �û����		"CONS_NO"	char(16)
	Idx_UserName,		// �û�����		"CONS_NAME"	char(254)
	Idx_UserAddr,		// �û���ַ		"CONS_ADDR"	char(254)
	Idx_PlanMrDate,		// �ƻ���������	"PL_MR_DATE"	char(8)
	Idx_MeterId,		// ȼ�����ʶ	"METER_ID"		char(16)
	Idx_MeterType,		// ȼ��������	"APPLY_TYPE"	char(8)
	Idx_MeterNum,		// �������		"FAC_NUMBER"	char(32)
	Idx_PriceCode,		// ������		"PRC_CODE"		char(16)
	Idx_TempFactor,		// �¶Ȳ���ϵ��	"TP_FACTOR"		number(15��6)
	Idx_LastMrDate,		// �ϴγ�������	"LAST_DATE"		char(8)
	Idx_LastReadVal,	// �ϴζ���		"LAST_READ"		number(15,6)
	Idx_LastGasVol,		// �ϴ�����		"LAST_MR_GQ"	number(15,6)
	Idx_AvgGasVol,		// ƽ������		"AVG_MR_GQ"		number(15,6)
	Idx_MeterReadStatus,	// ����״̬		"MR_STATUS"		char(8)			// set
	Idx_MeterReadType,		// ��������		"MR_TYPE"		char(8)			// set
	Idx_MeterReadDate,		// ʵ�ʳ�������	"THIS_YMD"		char(8)			// set
	Idx_CurrReadVal,		// ���ζ���		"THIS_READ"		number(15,6)	// set
	Idx_CurrGasVol,			// ��������		"THIS_GQ"		number(15,6)	// set
	Idx_MrExcepType,		// �����쳣����	"EXCP_TYPE"		char(8)			// set
	Idx_LastElecReadVal,	// �ϴε��Ӷ���	"LEM_READ"		number(15,6)
	Idx_CurrElecReadVal,	// ���ε��Ӷ���	"EM_READ"		number(15,6)	// set	
	Idx_LastRemainGasVol,	// �ϴ�ʣ������	"LR_GAS"		number(15,6)
	Idx_CurrRemainGasVol,	// ����ʣ������	"REMAIN_GAS"	number(15,6)	// set
	Idx_MeterDigit,		// ��λ��		"MR_DIGIT"	number(12,2)
	Idx_UserBalance,	// �û����		"BLA"		number(15,6)
	Idx_RemarkA,		// ��չ�ֶ�A	"REMARK_A"		char(32)
	Idx_RemarkA_V,		// ��չ�ֶ�A_V	"REMARK_A_V"	char(128)
	Idx_RemarkB,		// ��չ�ֶ�B	"REMARK_B"		char(32)
	Idx_RemarkB_V,		// ��չ�ֶ�B_V	"REMARK_B_V"	char(128)
	Idx_RemarkC,		// ��չ�ֶ�C	"REMARK_C"		char(32)
	Idx_RemarkC_V,		// ��չ�ֶ�C_V	"REMARK_C_V"	char(128)
	Idx_RemarkD,		// ��չ�ֶ�D	"REMARK_D"		char(32)
	Idx_RemarkD_V,		// ��չ�ֶ�D_V	"REMARK_D_V"	char(128)
	Idx_RemarkE,		// ��չ�ֶ�E	"REMARK_E"		char(32)
	Idx_RemarkE_V,		// ��չ�ֶ�E_V	"REMARK_E_V"	char(128)
	Idx_Invalid	= 0xFF	// ��Ч�ֶ�	
}DB_Field_Index;

// �ֶδ�С
#define Size_MtrReadId		17		// �������ݱ�ʶ	"MR_ID"		char(16) 
#define Size_AppNum			17		// app������	"APP_NO"	char(16) 
#define Size_SectNum		17		// �������	"SECT_NO"	char(16) 
#define Size_SectName		255		// ���������	"SECT_NAME"	char(254) 
#define Size_MtrReadSn		6		// ����˳���	"MR_SN"		number(5) 
#define Size_ReaderNum		17		// ����Ա����	"READER_NO"	char(16)
#define Size_ReaderName		17		// ����Ա����	"READER"	char(16)
#define Size_OrgNum			17		// ����λ		"ORG_NO"	char(16)
#define Size_UserNum		17		// �û����		"CONS_NO"	char(16)
#define Size_UserName		255		// �û�����		"CONS_NAME"	char(254)
#define Size_UserAddr		255		// �û���ַ		"CONS_ADDR"	char(254)
#define Size_PlanMrDate		9		// �ƻ���������	"PL_MR_DATE"	char(8)
#define Size_MeterId		17		// ȼ�����ʶ	"METER_ID"		char(16)
#define Size_MeterType		9		// ȼ��������	"APPLY_TYPE"	char(8)
#define Size_MeterNum		33		// �������		"FAC_NUMBER"	char(32)
#define Size_PriceCode		17		// ������		"PRC_CODE"		char(16)
#define Size_TempFactor		16		// �¶Ȳ���ϵ��	"TP_FACTOR"		number(15,6)
#define Size_LastMrDate		9		// �ϴγ�������	"LAST_DATE"		char(8)
#define Size_LastReadVal	16		// �ϴζ���		"LAST_READ"		number(15,6)
#define Size_LastGasVol		16		// �ϴ�����		"LAST_MR_GQ"	number(15,6)
#define Size_AvgGasVol		16		// ƽ������		"AVG_MR_GQ"		number(15,6)
#define Size_MeterReadStatus	9	// ����״̬		"MR_STATUS"		char(8)			// set
#define Size_MeterReadType		9	// ��������		"MR_TYPE"		char(8)			// set
#define Size_MeterReadDate		9	// ʵ�ʳ�������	"THIS_YMD"		char(8)			// set
#define Size_CurrReadVal		16	// ���ζ���		"THIS_READ"		number(15,6)	// set
#define Size_CurrGasVol			16	// ��������		"THIS_GQ"		number(15,6)	// set
#define Size_MrExcepType		9	// �����쳣����	"EXCP_TYPE"		char(8)			// set
#define Size_LastElecReadVal	16	// �ϴε��Ӷ���	"LEM_READ"		number(15,6)
#define Size_CurrElecReadVal	16	// ���ε��Ӷ���	"EM_READ"		number(15,6)	// set	
#define Size_LastRemainGasVol	16	// �ϴ�ʣ������	"LR_GAS"		number(15,6)
#define Size_CurrRemainGasVol	16	// ����ʣ������	"REMAIN_GAS"	number(15,6)	// set
#define Size_MeterDigit		13		// ��λ��		"MR_DIGIT"	number(12,2)
#define Size_UserBalance	16		// �û����		"BLA"		number(15,6)
#define Size_RemarkA		33		// ��չ�ֶ�A	"REMARK_A"		char(32)
#define Size_RemarkA_V		129		// ��չ�ֶ�A_V	"REMARK_A_V"	char(128)
#define Size_RemarkB		33		// ��չ�ֶ�B	"REMARK_B"		char(32)
#define Size_RemarkB_V		129		// ��չ�ֶ�B_V	"REMARK_B_V"	char(128)
#define Size_RemarkC		33		// ��չ�ֶ�C	"REMARK_C"		char(32)
#define Size_RemarkC_V		129		// ��չ�ֶ�C_V	"REMARK_C_V"	char(128)
#define Size_RemarkD		33		// ��չ�ֶ�D	"REMARK_D"		char(32)
#define Size_RemarkD_V		129		// ��չ�ֶ�D_V	"REMARK_D_V"	char(128)
#define Size_RemarkE		33		// ��չ�ֶ�E	"REMARK_E"		char(32)
#define Size_RemarkE_V		129		// ��չ�ֶ�E_V	"REMARK_E_V"	char(128)


//---------------	dbf ��ѯ�ṹ
typedef struct {
	uint32	dbCurrIdx;	// ���ݿ⵱ǰλ��
	uint32	reqMaxCnt;	// ���������
	uint32	resultCnt;	// ��ѯ�Ľ����¼��
	uint16	errorCode;	// 0 - ok,  ���� - ����
}DbQuerySt;

//---------------	������б�
typedef struct{
	char 	nums[Sect_Max][Size_ListStr];	// �б����ַ������������
	//char 	names[Sect_Max][Size_ListStr];	// �б����ַ��������������
	uint8 	idx;			// �б�������
	uint8	cnt;			// �б�������
}SectListSt;

//---------------	xx����� - �����б����/����/����/��ַ�� 
typedef struct{
	uint32 	dbIdx[Meter_Max];				// �б����Ӧ�����ݿ�����
	char 	strs[Meter_Max][Size_ListStr];	// �б����ַ��������/����/����/��ַ
	uint16 	idx;			// �б�������
	uint16	cnt;			// �б�������
	uint16 	selectField;	// Ҫ��ʾ���ֶΣ����/����/��ַ/����/��ַ
	char 	*qrySectNum;			// ������ţ���ֵ��ʾ����
	uint8 	qryMeterReadStatus;		// ����״̬
	char	*qryKeyWord;			// ��ѯ�Ĺؼ���
	char 	sectNum[Size_SectNum];			// �������
	char 	sectName[Size_SectName];		// ���������
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
	char	*qryUserNum;		// ����
	char 	*qryUserName;		// ����
	char 	*qryUserAddr;		// ��ַ
	
	char	userNum[Size_UserNum];		// �û����
	char	userName[Size_UserName];	// �û�����
	char	userAddr[Size_UserAddr];	// �û���ַ
	char	meterNum[Size_MeterNum];	// ���
	char	meterReadStatus[Size_MeterReadStatus];	// ����״̬
	char	meterReadTime[Size_MeterReadDate];		// ��������
	char	meterReadType[Size_MeterReadType];		// ��������
	char	meterExcpType[Size_MrExcepType];		// �����쳣����
	char	currGasVol[Size_CurrGasVol];				// ��������
	char	currRemainGasVol[Size_CurrRemainGasVol];	// ����ʣ������
	char	currReadVal[Size_CurrReadVal];				// ���ζ���
	char	currElecReadVal[Size_CurrElecReadVal];		// ���ε��Ӷ���

	char	lastGasVol[Size_LastGasVol];				// �ϴ�����
	char	lastRemainGasVol[Size_LastRemainGasVol];	// �ϴ�ʣ������
	char	lastReadVal[Size_LastReadVal];				// �ϴζ���
	char	lastElecReadVal[Size_LastElecReadVal];		// �ϴε��Ӷ���

	char	lastMtrReadTime[Size_LastMrDate];			// �ϴγ�������
	char	userBalance[Size_UserBalance];				// �û����
	char	avgGasVol[Size_AvgGasVol];					// ƽ������

	char	sectNum[Size_SectNum];			// �������
	char	sectName[Size_SectName];		// ���������
	char	readerNum[Size_ReaderNum];		// ����Ա���
	char	readerName[Size_ReaderName];	// ����Ա����
	char	orgNum[Size_OrgNum];			// ����λ���

}MeterInfoSt;

// �����쳣����
typedef enum{
	MrExcp_Normal		= 1,	// ����
	MrExcp_Flip,				// ��ת
	MrExcp_Reverse,				// ��ת
	MrExcp_FlipReverse,			// ��ת�ҵ�ת
	MrExcp_DataExcp		= 9		// �����쳣
}MtrReadException;

// ��������
typedef enum{
	MrType_BySee		= 1,	// ����
	MrType_ByEstimate,			// ����
	MrType_ByDoor,				// �ų�
	MrType_UserReport,			// �û��Ա�
	MrType_EmptyHourse,			// �շ�	
	MrType_ZeroGasVol			// ������
}MtrReadType;


//------------------------		�ⲿ�ӿ�����	-------------------------------------

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