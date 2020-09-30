#ifndef WaterMeter_H
#define WaterMeter_H

#include "stdio.h"
#include "Common.h"

#if defined Project_6009_RF || defined Project_8009_RF
#include "MeterDocDBF.h"
#elif defined Project_6009_RF_HL || defined Project_8009_RF_HL
#include "MeterDocDBF_HL.h"
#endif

extern uint8 PackWater6009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWater6009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

#ifndef Frame_Pack_Parse
#define Frame_Pack_Parse
FuncCmdFramePack FramePack = PackWater6009RequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWater6009ResponseFrame;
#endif

//----------------------------------------  �������  ------------------------
/*
��˲�����	
1	��������
2	��������
3	��������
4	Ԥ������
5	��������
6	��������
*/
typedef enum{

	WaterCmd_ReadMeterCfgInfo	 	= 0x04,	// ��ȡ��˲���������Ϣ
	
	/*
	�������	
	1	��ȡ�û�����
	2	��ȡ������ת����
	3	����
	4	ǿ�ƿ���
	5	�ط�
	6	ǿ�ƹط�
	7	���쳣����
	*/
	WaterCmd_ReadRealTimeData		= 0x11,
	WaterCmd_ReadFrozenData,
	WaterCmd_OpenValve,	
	WaterCmd_OpenValveForce,
	WaterCmd_CloseValve,
	WaterCmd_CloseValveForce,	
	WaterCmd_ClearException,

	/*
	�������	
	1	�������
	2	�����¶�
	3	�����ѹ
	4	��Ԥ�ɲο���
	5	���ù�����ʱ
	6	����Ӫ�̱��
	7	���ϱ�·��
	8	���ñ��
	9	��debug��Ϣ
	10	��debug��Ϣ
	*/
	WaterCmd_RebootDevice				= 0x21,
	WaterCmd_ReadTemperature,
	WaterCmd_ReadVoltage, 
	WaterCmd_ClearPrepaidRefVal,
	WaterCmd_SetOverCurrentTimeout,
	WaterCmd_ReadOperatorNumber,	
	WaterCmd_ReadReportRoute,
	WaterCmd_SetMeterNumber,
	WaterCmd_ReadDebugInfo,
	WaterCmd_ClearDebugInfo,

	/*
	����������	
	1	֪ͨϵͳ����_��app
	2	֪ͨϵͳ����_��boot
	3	������������
	4	��ѯ����״̬_��boot
	5	��ѯ����״̬_��app
	*/
	WaterCmd_NoticeUpgrade_OnApp		= 0x31,
	WaterCmd_NoticeUpgrade_OnBoot,
	WaterCmd_SendUpgradePacket, 
	WaterCmd_QueryUpgradeStatus_OnBoot,
	WaterCmd_QueryUpgradeStatus_OnApp,	

	/*
	Ԥ��������	
	1	��Ԥ�ɲο�����
	2	��Ԥ�ɲο�����
	3	��������ֵ͸֧
	4	�豨����ֵ
	5	��ط���ֵ
	6	�豨���ط���ֵ
	*/
	WaterCmd_ReadPrepaidRefVal		= 0x41,
	WaterCmd_SetPrepaidRefVal,
	WaterCmd_ReadAlarmLimitOverdraft, 
	WaterCmd_SetAlarmLimit,
	WaterCmd_SetCloseValveLimit,
	WaterCmd_SetAlarmAndCloseValveLimit,	

	/*
	����������	
	1	����������ϵ��
	2	�����ת��������
	3	��ȡ����ʹ��״̬
	4	���ö�ʱ�ϴ�
	5	���ö����ϴ�
	6	���ö�ʱ�����ϴ�
	7	�����ʱ��
	8	У���ʱ��
	9	��IP+�˿�+ģʽ
	10	��IP+�˿�+ģʽ
	11	��NB��Ӫ�̱��
	12	��IMEI+CCID
	13  ��ȡNB������Ϣ
	14  ��ȡ����ˮ�����
	15  ���ñ���ˮ�����
	*/
	WaterCmd_SetBaseValPulseRatio	= 0x51,
	WaterCmd_ClearReverseMeasureData,
	WaterCmd_ReadFuncEnableState, 
	WaterCmd_SetTimedUpload,
	WaterCmd_SetFixedValUpload,
	WaterCmd_SetTimedAndFixedValUpload,	
	WaterCmd_ReadMeterTime,
	WaterCmd_SetMeterTime,
	WaterCmd_SetIpPortMode,
	WaterCmd_ReadIpPortMode,
	WaterCmd_ReadNbOperaterNumber,
	WaterCmd_ReadImeiAndCcid,
	WaterCmd_ReadNbJoinNetworkInfo,
	WaterCmd_ReadBeiJingWaterMeterParams,
	WaterCmd_SetBeiJingWaterMeterParams	= 0x5F,

	/*
	����������	
	1	���շ����ŷ�����
	2	��ȡRXD��TXD�ŵ�
	3	����RXD��TXD�ŵ�
	4	������Ӫ�̱��
	5	·���·�
	6	��ȡģ���Ƶ��
	7	����ģ���Ƶ��
	8	���ù���ʹ��״̬
	*/
	WaterCmd_ReadRxTxMgnDistbCnt		= 0x61,
	WaterCmd_ReadRxdAndTxdChanel,
	WaterCmd_SetRxdAndTxdChanel, 
	WaterCmd_SetOperatorNumber,
	WaterCmd_SetDefinedRoute,
	WaterCmd_ReadModuleFrequency,
	WaterCmd_SetModuleFrequency,
	WaterCmd_SetFuncEnableState, 
	/*
	UART���ģ����ԣ�	
	1	��ȡģ�����в���
	2	����ģ�����в���
	*/
	WaterCmd_ReadModuleRunningParams,
	WaterCmd_SetModuleRunningParams,

	/*
	������������:
	1	NB�����ϱ�ʵʱ����
	*/
	WaterCmd_NbReportRealTimeDataNow	= 0x71

}WaterCmdDef;


//----------------------------------------  ����������  ------------------------
/*
������������	
1	��������
2	��������
3	·������
4	����ת��
*/
typedef enum{
	/*
	���ò�����	
	1	����������
	2	���������汾
	3	��������ʱ��
	4	�輯����ʱ��
	5	��GPRS����
	6	��GPRS����
	7	��GPRS�ź�ǿ��
	8	��������ʼ��
	9	������������ģʽ
	*/
	CenterCmd_ReadCenterNo		= 0x1011,
	CenterCmd_ReadCenterVer,
	CenterCmd_ReadCenterTime,
	CenterCmd_SetCenterTime,
	CenterCmd_ReadGprsParam,
	CenterCmd_SetGprsParam,
	CenterCmd_ReadGprsSignal,
	CenterCmd_InitCenter,
	CenterCmd_ReadCenterWorkMode,

	/*
	����������	
	1	����������
	2	��������Ϣ
	3	��ӵ�����Ϣ
	4	ɾ��������Ϣ
	5	�޸ĵ�����Ϣ
	*/
	CenterCmd_ReadDocCount		= 0x1021,
	CenterCmd_ReadDocInfo,
	CenterCmd_AddDocInfo,
	CenterCmd_DeleteDocInfo,
	CenterCmd_ModifyDocInfo,

	/*
	·�����ã�	
	1	���Զ���·��
	2	���Զ���·��
	*/
	CenterCmd_ReadDefinedRoute	= 0x1031,
	CenterCmd_SetDefinedRoute,

	/*
	����ת����	
	1	��ʵʱ����
	2	����ʱ��������
	3	����������
	4	����
	5	�ط�
	6	��ʹ��
	7	���쳣
	*/
	CenterCmd_ReadRealTimeData	= 0x1041,
	CenterCmd_ReadFixedTimeData,
	CenterCmd_ReadFrozenData,
	CenterCmd_OpenValve,
	CenterCmd_CloseValve,
	CenterCmd_ReadEnableState,
	CenterCmd_ClearException

}CenterCmdDef;


//---------------------------------------		6009 ��������	-------------------------------------

/*
* ��  �������ַ�����ַ�����6009ˮ���ַ��
* ��  ����addrs			- ��ַ��ṹ
*		  strDstAddr	- Ŀ�ĵ�ַ�ַ���
* 		  strRelayAddrs - �м̵�ַ�ַ�������
* ����ֵ��void
*/
void Water6009_PackAddrs(ParamsBuf *addrs, const char strDstAddr[], const char strRelayAddrs[][20])
{
	/*
	�������ݷ�ʽ1��const char strRelayAddrs[][20]
	�������ݷ�ʽ2��const char (*strRelayAddrs)[20]
	�������ݷ�ʽ3��const char **strRelayAddrs, uint addrLen
	 */
	#if defined Project_6009_RF || defined Project_6009_RF_HL
	uint8 i;
	#endif

	// Դ��ַ
	addrs->itemCnt = 0;
	addrs->items[addrs->itemCnt] = &addrs->buf[0];
	memcpy(addrs->items[addrs->itemCnt], LocalAddr, AddrLen);
	addrs->itemCnt++;

	#if defined Project_6009_RF || defined Project_6009_RF_HL
	// �м̵�ַ
	for(i = 0; i < RELAY_MAX; i++){
		if(strRelayAddrs[i][0] >= '0' && strRelayAddrs[i][0] <= '9'){
			addrs->items[addrs->itemCnt] = &addrs->buf[AddrLen + i*AddrLen];
			GetBytesFromStringHex(addrs->items[addrs->itemCnt], 0, AddrLen, strRelayAddrs[i], 0, false);
			addrs->itemCnt++;
		}
	}
	#endif

	// Ŀ�ĵ�ַ
	GetBytesFromStringHex(DstAddr, 0, AddrLen, strDstAddr, 0, false);
	addrs->items[addrs->itemCnt] = &addrs->buf[addrs->itemCnt*AddrLen];
	memcpy(addrs->items[addrs->itemCnt], DstAddr, AddrLen);
	addrs->itemCnt++;
}

/*
* ��  ������ȡ6009ˮ��-�豸������
* ��  ����typeId	- ����ID
* ����ֵ��char *	- ��������ַ���
*/
char * Water6009_GetStrDeviceType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x10:	str = "RF��ˮ��";	break;
	case 0x11:	str = "GPRS��ˮ��";	break;
	case 0x12:	str = "NB-IoT��ˮ��";	break;
	case 0x20:	str = "RF��ˮ��";	break;
	case 0x21:	str = "NB-IoT��ˮ��";	break;
	case 0x30:	str = "RF����";	break;
	case 0x31:	str = "GPRS����";	break;
	case 0x32:	str = "NB-IoT����";	break;
	case 0x40:	str = "���";	break;

	case 0x50:	str = "͸��ģ��";	break;

	case 0xF9:	str = "USB";	break;
	case 0xFA:	str = "��λ����";	break;
	case 0xFB:	str = "UART����";	break;
	case 0xFC:	str = "������";	break;
	case 0xFD:	str = "�м���";	break;
	case 0xFE:	str = "�ֳֻ�";	break;
	default:
		str = "δ֪";
		break;
	}

	return str;
}

/*
* ��  ������ȡ����ˮ��-�豸������
* ��  ����typeId	- ����ID
* ����ֵ��char *	- ��������ַ���
*/
char * WaterBeiJing_GetStrDeviceType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x41:	str = "�����޴�";	break;
	case 0x44:	str = "�����޴�";	break;
	case 0x35:	str = "ɽ���޴�";	break;
	case 0x26:	str = "�����޴�";	break;
	case 0x2A:	str = "�����޴�";	break;
	case 0x2B:	str = "��Դ�޴�";	break;
	default:
		str = "δ֪";
		break;
	}

	return str;
}

/*
* ��  ������ȡ6009ˮ�����������
* ��  ����typeId	- ����ID
* ����ֵ��char *	- ��������ַ���
*/
char * Water6009_GetStrValueType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x00:	str = "ʵʱ";	break;
	case 0x01:	str = "�����ϴ�";	break;
	case 0x02:	str = "��ʱ�ϴ�";	break;
	case 0x03:	str = "�����ϴ�";	break;
	case 0x04:	str = "����";	break;
	default:
		str = "δ֪";
		break;
	}

	return str;
}

/*
* ��  ������ȡ6009ˮ�� ��������������
* ��  ����typeId	- ����ID
* ����ֵ��char *	- ��������ַ���
*/
char * Water6009_GetStrSensorType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x00:	str = "���ɻɹ�/����";	break;
	case 0x01:	str = "˫�ɻɹ�/����";	break;
	case 0x02:	str = "���ɻɹ�/����";	break;
	case 0x03:	str = "����4λ���ֱ��";	break;
	case 0x04:	str = "��Ĥֱ��";	break;
	case 0x05:	str = "����1λ���ֱ��";	break;
	case 0x06:	str = "188Э����ֱ��";	break;
	case 0x07:	str = "188Э���޴�ֱ��";	break;
	case 0x08:	str = "2��������";	break;
	case 0x09:	str = "�����޴�";	break;
	case 0x0A:	str = "ɽ���޴�";	break;
	case 0x0B:	str = "�����޴�";	break;
	case 0x0C:	str = "�����޴�";	break;
	default:	str = "δ֪";	break;
	}

	return str;
}

/*
* ��  ��������6009ˮ��-�澯״̬��
* ��  ����status	- ״̬��
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water6009_GetStrAlarmStatus(uint16 status, char *buf)
{
	char * str = NULL;
	uint16 mask = 1, i;
	uint16 len = 0;

	for(i = 0; i < 14; i++){

		mask = (1 << i);
		
		switch(status & mask){
		case 0x01:	str = "�ɻɹܹ���";	break;
		case 0x02:	str = "����λ����";	break;
		case 0x04:	str = "�������߶Ͽ�";	break;
		case 0x08:	str = "���Ƿѹ";	break;
		case 0x10:	str = "����-һ���ܻ�";	break;
		#if ShowEMI_ON
		case 0x20:	str = "�Ÿ��ű�־";	break;
		#endif
		case 0x40:	str = "����-�����ܻ�";	break;
		case 0x80:	str = "����-��ǿ�����";	break;
		case 0x0100:	str = "ˮ��ת";	break;
		case 0x0200:	str = "ˮ����ж";	break;
		case 0x0400:	str = "ˮ����ֱ��װ";	break;
		case 0x0800:	str = "EEPROM�쳣";	break;
		case 0x1000:	str = "ú��й©";	break;
		case 0x2000:	str = "Ƿ�ѱ�־";	break;
		default:
			break;
		}

		if(str != NULL){
			len += sprintf(&buf[len], "%s, ", str);
			str = NULL;
		}
	}

	if(len == 0){
		len += sprintf(&buf[len], " \n");
	}
	else{	// ���� ", " --> �滻Ϊ " \n"
		buf[len - 2] = ' ';
		buf[len - 1] = '\n';
		buf[len] = 0x00;
	}

	return len;
}

/*
* ��  ������ȡ6009ˮ�� ����״̬
* ��  ����status	- ״̬
* ����ֵ��char *	- ��������ַ���
*/
char * Water6009_GetStrValveStatus(uint8 status)
{
	char * str = NULL;
	
	switch(status & 0x03){
	case 0:	str = "����";	break;
	case 1:	str = "��";	break;
	case 2:	str = "��";	break;
	default:
		str = "δ֪";
		break;
	}

	return str;
}
/*
* ��  ������ȡ6009ˮ�� ������������
* ��  ����status	- ״̬
* ����ֵ��char *	- ��������ַ���
*/
char * Water6009_GetStrErrorMsg(uint8 errorCode)
{
	char * str = NULL;

	switch(errorCode){
	case 0xAA:
		str = "�����ɹ�";
		break;
	case 0xAB:
	    str = "����ʧ��";
		break;
	case 0xAC:
	    str = "ͨѶʧ��";
		break;
	case 0xAD:
	    str = "�����´�ɹ�";
		break;
	case 0xAE:
	    str = "��ʽ����";
		break;
	case 0xAF:
	    str = "Ԥ��";
		break;
	case 0xBA:
	    str = "���󲻴���";
		break;
	case 0xBB:
	    str = "�����ظ�";
		break;
	case 0xBC:
		str = "��������";
		break;
	case 0xCC:
	    str = "��ʱ����";
		break;
	case 0xCD:
	    str = "�������г�ʱ";
		break;
	case 0xCE:
	    str = "����ִ��";
		break;
	case 0xCF:
	    str = "�����Ѵ���";
		break;
	case 0xD0:
	    str = "��Ӧ��";
		break;
	case 0xD1:
	    str = "�������";
		break;
	case 0xD2:
	    str = "�޴˹���";
		break;
	case 0xD3:
	    str = "����δע��";
		break;
	case 0xD4:
	    str = "�����û������";
		break;
	case 0xD5:
	    str = "У�������";
		break;
	case 0xD6:
	    str = "���ݽ�ֹ����";
		break;
	case 0xD7:
	default:
		str = "δ֪����";
		break;
	}

	return str;
}

/*
* ��  ������ȡ6009ˮ�� ����ʧ��ԭ��
* ��  ����errorCode	- ������
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water6009_GetStrValveCtrlFailed(uint16 errorCode, char * buf)
{
	char * str = NULL;
	uint16 mask = 1, i;
	uint16 len = 0;

	for(i = 0; i < 12; i++){

		mask = (mask << i);
		
		switch(errorCode & mask){
		case 0x01:	str = "���Ƿѹ";	break;
		case 0x02:	str = "�Ÿ�����";	break;
		case 0x04:	str = "Adc���ڹ���";	break;
		case 0x08:	str = "������������";	break;
		case 0x10:	str = "���Ź���";	break;
		case 0x20:	str = "RF���ڹ���";	break;
		case 0x40:	str = "��������ʧ��";	break;
		case 0x80:	str = "�ȴ���������";	break;
		case 0x100:	str = "�����Ѿ���λ";	break;
		case 0x200:	str = "�豸���ʹ���";	break;
		case 0x400:	str = "timer����ʧ��";	break;
		case 0x800:	str = "ϵͳǷ��";	break;
		default:	
			break;
		}

		if(str != NULL){
			len += sprintf(&buf[len], "  %s\n", str);
			str = NULL;
		}
	}

	if(len == 0){
		len += sprintf(&buf[len], "  \n");
	}

	return len;
}

/*
* ��  ������ȡ6009ˮ�� ����ʹ��״̬
* ��  ����stateCode	- ʹ��״̬��
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water6009_GetStrMeterFuncEnableState(uint16 stateCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], "�Ÿ��Źط�����  :%s\n", ((stateCode & 0x0001) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�ϱ����ݼ���    :%s\n", ((stateCode & 0x0002) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "����ж��⹦��  :%s\n", ((stateCode & 0x0004) > 0 ? "��" : " ��"));
	#if defined Project_6009_RF || defined Project_6009_RF_HL
		len += sprintf(&buf[len], "LoRaWan״̬   :%s\n", ((stateCode & 0x0008) > 0 ? "��" : " ��"));
	#else // Project_6009_IR
		len += sprintf(&buf[len], "Ƿ�ѷ�����      :%s\n", ((stateCode & 0x0008) > 0 ? "��" : " ��"));
	#endif
	len += sprintf(&buf[len], "�����澯        :%s\n", ((stateCode & 0x0010) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�ϱ���������    :%s\n", ((stateCode & 0x0020) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "͸֧�ط�����    :%s\n", ((stateCode & 0x0040) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "Ԥ���ѹ���      :%s\n", ((stateCode & 0x0080) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�Զ��ŵ�����    :%s\n", ((stateCode & 0x0100) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "���⹦��        :%s\n", ((stateCode & 0x0200) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "����ط�����    :%s\n", ((stateCode & 0x0400) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "RF���߲���      :%s\n", ((stateCode & 0x0800) > 0 ? "��" : " ��"));
	// ���� len += sprintf(&buf[len], "�����Զ��ط�    :%s\n", ((stateCode & 0x1000) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "ú��й©���    :%s\n", ((stateCode & 0x2000) > 0 ? "��" : " ��"));
	// ����
	len += sprintf(&buf[len], "���ٿ��ƹ���    :%s\n", ((stateCode & 0x8000) > 0 ? "��" : " ��"));

	return len;
}

/*
* ��  ������ȡ6009ˮ�� ģ�����״̬
* ��  ����statusCode	- ����״̬��
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water6009_GetStrTestStatus(uint16 statusCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], " ���ߵ�������  : %s\n", ((statusCode & 0x0001) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " Ƶ�ʲ���      : %s\n", ((statusCode & 0x0002) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " ���ʲ���      : %s\n", ((statusCode & 0x0004) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " ���书�ʲ���  : %s\n", ((statusCode & 0x0008) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " ���������Ȳ���: %s\n", ((statusCode & 0x0010) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " ���յ�������  : %s\n", ((statusCode & 0x0020) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " ���ص�·����  : %s\n", ((statusCode & 0x0040) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " ������·����  : %s\n", ((statusCode & 0x0080) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " LCD����       : %s\n", ((statusCode & 0x0100) > 0 ? "OK" : " NG"));

	return len;
}

/*
* ��  ������ȡ�޷�����ԭ��
* ��  ����
* 		code - �޷�����ԭ�����
*		bitVal - ��Ӧλ��ֵ
* ����ֵ��char * - ��������ַ���
*/
char * Water6009_GetStrUpgradeForbidReason(uint8 code, uint8 *bitVal)
{
	char * str = NULL;
	uint8 mask = 1, i;

	for(i = 0; i < 6; i++){

		mask = (1 << i);
		*bitVal = code & mask;
		
		switch(*bitVal){
		case 0x01:	str = "5.0v VBat";	break;
		case 0x02:	str = "3.4v VBat";	break;
		case 0x04:	str = "SNR";	break;
		case 0x08:	str = "RSSI";	break;
		case 0x10:	str = "VerNo";	break;
		case 0x20:	str = "VerCrc";	break;
		default:
			break;
		}

		if(str != NULL){
			break;
		}
	}

	if(str == NULL){
		str = "Pass";
		*bitVal = 0;
	}

	return str;
}

/*
* ��  ������ȡ����״̬
* ��  ����
* 		code - ����״̬����
*		bitVal - ��Ӧλ��ֵ
* ����ֵ��char * - ��������ַ���
*/
char * Water6009_GetStrUpgradeStatus(uint8 code, uint8 *bitVal)
{
	char * str = NULL;
	uint8 mask = 1, i;

	for(i = 0; i < 6; i++){

		mask = (1 << i);
		*bitVal = code & mask;
		
		switch(*bitVal){
		case 0x01:	str = "δ��ʼ";	break;
		case 0x02:	str = "ȱ���ȴ�";	break;
		case 0x04:	str = "�������";	break;
		case 0x08:	str = "AppCrcErr";	break;
		case 0x10:	str = "VerCrcErr";	break;
		case 0x20:	str = "�ܰ�������";	break;
		default:
			break;
		}

		if(str != NULL){
			break;
		}
	}

	if(str == NULL){
		str = "";
		*bitVal = 0;
	}

	return str;
}

//-----------------------------------		6009ˮ��Э�� ��� / ���	-----------------------------

/*
* ��������PackWater6009RequestFrame
* ��  �������6009ˮ����������֡
* ��  ����buf	- ���ݻ�����ʼ��ַ
		  addrs - ��ַ��Դ��ַ���м̵�ַ��Ŀ�ĵ�ַ
		  cmdId - ������
		  args	- ���������
		  retryCnt - �ط�������0 - ��1�η��ͣ����� - ��n���ط�
* ����ֵ��uint8 ֡�ܳ���
*/
uint8 PackWater6009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt)
{
	static uint8 Fsn = 0;
	static uint16 index = 0;
	uint8 i, cmd;
	#if UseCrc16
		uint16 crc16;
	#else
		uint8 crc8;
	#endif

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = 0xD3;		// ֡ͷͬ���룺 �̶�Ϊ D3 91
	buf[index++] = 0x91;
	buf[index++] = 0x00;		// ���ȣ� ���ı�ʶ --> ������16
	buf[index++] = 0x00;	
	buf[index++] = 0x10;		// ���ı�־ bit7 0/1 - ����/���У� bit6 0/1 - ����/Ӧ�� bit4 �̶�Ϊ1
	#if UseCrc16
		buf[index - 1] |= 0x01;	// crc16��ʶ
	#endif
	buf[index++] = Fsn++;		// ����ţ� mac fsn �������ۼ�
	cmd = *args->items[0];
	buf[index++] = cmd;			// ������
	buf[index++] = 0xFE;		// �豸����: FE - �ֳֻ��� FA - ��λ���� 10 - ��ˮ�� 11 - GPRSˮ��
	buf[index++] = 0x0F;		// ��������
	buf[index++] = addrs->itemCnt & 0x0F;	// ·����Ϣ:  ��ǰλ��|·������
	// ��ַ��
	for(i = 0; i < (addrs->itemCnt & 0x0F); i++){
		memcpy(&buf[index], addrs->items[i], AddrLen);
		index += AddrLen;
	}

	// ������
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	if(cmd >= 0x3A && cmd <= 0x3F){
		buf[index++] = 0x55;		// ���г�ǿ
		buf[index++] = 0xAA;		// ���г�ǿ
		buf[7] = 0xFA;				// �豸���͸�Ϊ FA - ��λ��
	}else{
		buf[index++] = 0x00;		// ���г�ǿ
		buf[index++] = 0x00;		// ���г�ǿ
	}
	
    
    
	#if UseCrc16
		// ���ȼ���
		buf[2] = (uint8)((index + 1) & 0xFF);	
		buf[3] = (uint8)((index + 1) >> 8);	
		// crc16 У��
		crc16 = GetCrc16(&buf[2], index - 2, 0x8408u);
		buf[index++] = (uint8)(crc16 & 0xFF);	
		buf[index++] = (uint8)(crc16 >> 8);	
	#else
		// ���ȼ���
		buf[2] = (uint8)(index & 0xFF);	
		buf[3] = (uint8)(index >> 8);	
		// crc8 У��
		crc8 = GetCrc8(&buf[2], index - 2);
		buf[index++] = crc8;		
	#endif

	buf[index++] = 0x16;		// ������
		
	if(cmd < 0x40 || cmd == 0x70 || cmd == 0x74){
		buf[index++] = 0x1E;	// ���Գ��ȱ�ʶ
		buf[index++] = 0x03;	// ���APPʱ �����ŵ�
		buf[index++] = 0x19;	// ���APPʱ �����ŵ�
	}else if(cmd > 0x70 && cmd < 0x74){
		buf[index++] = 0x00;	// ���Գ��ȱ�ʶ
		buf[index++] = 0x0F;	// ���Bootʱ �����ŵ�
		buf[index++] = 0x1F;	// ���Bootʱ �����ŵ�
	}else{
		buf[index++] = 0x00;	// ���Գ��ȱ�ʶ 00
		buf[index++] = 0x19;	// ������ �����ŵ� 19
		buf[index++] = 0x19;	// ������ �����ŵ� 19
	}
	
	return index;
}

/*
* ��������ExplainWater6009ResponseFrame
* ��  ��������ˮ��������Ӧ֡
* ��  ����buf		- ���ջ�����ʼ��ַ
*		  rxlen		- ���յĳ���
*		  dstAddr	- Ŀ�ĵ�ַ���жϽ��յ�Ŀ�ĵ�ַ�Ƿ��Ǳ�����ַ
*		  cmdId		- ������
*		  ackLen	- Ӧ�𳤶�
*		  dispBuf 	- ��������ʾ����
* ����ֵ��uint8 ���������0 - �ɹ� , 1 - ʧ�� �� 2 - CRC���� 3 - ��ʱ��Ӧ��
*/
uint8 ExplainWater6009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
{
	bool ret = CmdResult_Failed;
	uint8 addrsCnt, cmd, i, u8Tmp;
	uint16 index = 0, dispIdx, length, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	char *ptr;

	dispIdx = 0;

	// ��ʾ��� �� ��������
	if(cmdId < 0x1010){
		#if (AddrLen == 6)
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "���: \n   %s\n", StrDstAddr);
		#endif
	}else{
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: \n   %s\n", StrDstAddr);
	}

	// ����������в���
	while(1){

		if(rxlen < index + 27){
			sprintf(&dispBuf[dispIdx], "���: ��ʱ,��Ӧ��");
			return CmdResult_Timeout;
		}

		// start check
		if(buf[index] == 0xD3 && buf[index + 1] == 0x91){
			index += 2;
		}else{
			index++;
			continue;
		}
		
		// length check
		length = (uint16)((buf[index + 1] << 8) + buf[index]);
		if((index + length) > rxlen){
			index += length;
			continue;
		}	

		// srcaddr check
		addrsCnt = (buf[index + 7] & 0x0F);
		if(cmd != WaterCmd_SetMeterNumber && memcmp(BroadAddr, DstAddr, AddrLen) != 0){
			if(memcmp(&buf[index + 8], DstAddr, AddrLen) != 0){
				index += length;
				continue;
			}
		}
		// dstaddr check
		if(memcmp(&buf[index + 8 + (addrsCnt - 1) * AddrLen], LocalAddr, AddrLen) != 0){
			index += length;
			continue;
		}

		#if UseCrc16
			// crc16 check
			u16Tmp = GetCrc16(&buf[index], length - 3, 0x8408u);
			if(u16Tmp !=  (buf[index + length - 3] + buf[index + length - 2] * 256)){
				sprintf(&dispBuf[dispIdx], "���: ��Ӧ��,CRC����");
				return CmdResult_CrcError;
			}
		#else
			// crc8 check
			u8Tmp = GetCrc8(&buf[index], length - 2);
			if(u8Tmp !=  buf[index + length - 2]){
				sprintf(&dispBuf[dispIdx], "���: ��Ӧ��,CRC����");
				return CmdResult_CrcError;
			}
		#endif

		// pass
		break;
	}

	startIdx = index;

	// ������
	cmd = buf[index + 4];

	// ���� ���� --> ·����Ϣ
	index += 8;

	// ��ַ��
	if(memcmp(&buf[index], DstAddr, AddrLen) != 0){

		// �㲥����ʱ����Ӧ��ı��ַ��Ϊ�µ�Ŀ�ĵ�ַ
		GetStringHexFromBytes(StrDstAddr, buf, index, AddrLen, 0, false);

		dispIdx = 0;
		#if (AddrLen == 6)
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "���: \n   %s\n", StrDstAddr);
		#endif
	}
	index += addrsCnt * AddrLen;

	// ������ת��������ͷ��
	if(cmd == 0x4D){
		// ������
		cmd = buf[index];
		index += 1;
		// ���
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, AddrLen, 0, false);
		TmpBuf[AddrLen] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", &TmpBuf[0]);
		index += AddrLen;
		// ת�����
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
	}

	// ���������
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	������� 0x01 ~ 0x25 , 70 ~ 74		--------------------
	//-------------------------------------------------------------------------------------------------
	//----------------------------------------		��ȡ�û�����		-------------
	case WaterCmd_ReadRealTimeData:	// ��ȡ�û�����
	case CenterCmd_ReadRealTimeData:
		if(rxlen < index + 21 || cmd != 0x01){
			break;
		}
		ret = CmdResult_Ok;
		
		// ����
		ptr = Water6009_GetStrValueType((buf[index] >> 4));
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", ptr);
		index += 1;
		// ��ת����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ת: %d.%03d\n", u32Tmp, u16Tmp);
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.meterValue, "%d.%03d", u32Tmp, u16Tmp);
			}
		#endif
		// ��ת����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ת: %d.%03d\n", u32Tmp, u16Tmp);
		//�澯״̬��
		#ifdef Use_DBF
			u32Tmp = dispIdx + 6;
		#endif
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "�澯: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				u16Tmp = (uint16)(dispIdx - u32Tmp - 2);	// ȥ������� �� \n��
				if(u16Tmp + 9 > Size_MeterStatusStr){
					u16Tmp = Size_MeterStatusStr - 9;
				}
				strncpy(&MeterInfo.meterStatusStr[0], &dispBuf[u32Tmp], u16Tmp);
				if(u16Tmp > 0)
				{
					MeterInfo.meterStatusStr[u16Tmp++] = ',';
				}
			}
		#endif
		//����״̬ 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s  ", ptr);
		index += 1;
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
			#if defined Project_6009_RF || defined Project_8009_RF
				sprintf(MeterInfo.meterStatusHex, "%02X%02X%02X", buf[index - 3], buf[index - 2], buf[index - 1]);
				sprintf(&MeterInfo.meterStatusStr[u16Tmp], "����%s", ptr);
			#elif defined Project_6009_RF_HL || defined Project_8009_RF_HL
				sprintf(MeterInfo.meterStatusHex, "%02X%02X", buf[index - 3], buf[index - 2]);
				sprintf(MeterInfo.valveStatus, "%d", buf[index - 1] == 2 ? 0 : 1 );		// ״̬ת�� 2/1 --> 0/1 ����/����
				MeterInfo.meterStatusStr[u16Tmp - 1] = '\0';
			#endif
			}
		#endif
		//��ص�ѹ
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				strncpy(MeterInfo.batteryVoltage, &dispBuf[dispIdx - 4], 3);
			}
		#endif
		//�����¶�
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "�¶�: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR ������
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx�ŵ�
		dispIdx += sprintf(&dispBuf[dispIdx], "�ŵ�: Tx-%d, Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//Э��汾
		dispIdx += sprintf(&dispBuf[dispIdx], "Э��汾: %d\n", buf[index]);
		index += 1;
		break;

	//----------------------------------------		��ȡ��������	---------------------
	case WaterCmd_ReadFrozenData:	// ��ȡ��������
		if(rxlen < index + 88 || cmd != 0x02){
			break;
		}
		ret = CmdResult_Ok;
		// ������������
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", (buf[index] == 0x01 ? "����" : "��ת"));
		index += 1;

		if(rxlen < index + 104){	// �������ݸ�ʽ-�ɰ汾 1 + 78 byte
			// ����������ʼ���
			u8Tmp = buf[index] * 10;
			dispIdx += sprintf(&dispBuf[dispIdx], "��Χ: �� %d-%d ��\n", u8Tmp, u8Tmp + 9);
			index += 1;
			// ����������ʼʱ��
			dispIdx += sprintf(&dispBuf[dispIdx], "ʱ��: %X%x%x%x %x:00:00\n"
				, buf[payloadIdx + 2], buf[payloadIdx + 3], buf[payloadIdx + 4], buf[payloadIdx + 5], buf[payloadIdx + 6]);
			index += 5;
			// �������ݷ�ʽ ��0-����, 1-����
			// ������������ ���������24�����������30��
			dispIdx += sprintf(&dispBuf[dispIdx], "��ʽ: ÿ%s����%d��\n", (buf[index] == 0x01 ? "��" : "��"), buf[index + 1]);
			index += 2;	
			// ��������ʱ����
			if(buf[index] == 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "���: ÿ%s����1��\n", (buf[index - 2] == 0x01 ? "��" : "��"));
			}
			else{
				dispIdx += sprintf(&dispBuf[dispIdx], "���: %d%s����1��\n", buf[index], (buf[index - 2] == 0x01 ? "Сʱ" : "��"));
			}
			index += 1;
			// ������������ݣ�7*N �ֽ� ��6 byte ���� + 1 byte date.day��
			dispIdx += sprintf(&dispBuf[dispIdx], "��ȡ��10����������: \n");
			for(i = 0; i < 10; i++){
				u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
				index += 4;
				u16Tmp = ((buf[index + 1] << 8) + buf[index]);
				index += 2;
				dispIdx += sprintf(&dispBuf[dispIdx], "%d, %x/%x: %d.%03d\n", i, buf[payloadIdx + 4], buf[index], u32Tmp, u16Tmp);
				index +=1;
			}
		}
		else{		// �������ݸ�ʽ-�°汾	1 + 104 byte
			// ����������ʼ���
			dispIdx += sprintf(&dispBuf[dispIdx], "��Χ: ������%d������\n", buf[index] + 1);
			index += 1;
			// ʱ����Ϣ
			dispIdx += sprintf(&dispBuf[dispIdx], "ʱ��: %02X-%02X %02X:%02X\n",
				buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			index += 4;
			// �ۼ�����
			u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
			index += 4;
			u16Tmp = ((buf[index + 1] << 8) + buf[index]);
			index += 2;
			dispIdx += sprintf(&dispBuf[dispIdx], "�ۼ�����: %d.%03d\n", u32Tmp, u16Tmp);
			// 0:00 ~ 23:30 ����
			u8Tmp = 0;
			u16Tmp = 0x00;
			for(i = 0; i < 47; i++){
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X-", u8Tmp, u16Tmp);
				u16Tmp += 0x30;
				if(u16Tmp == 0x60){
					u16Tmp = 0x00;
					u8Tmp += 1;
				}
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X����:%d\n", u8Tmp, u16Tmp, (buf[index] + buf[index + 1]*256));
				index += 2;
			}
		}

		//�澯״̬��
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "�澯: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		//����״̬ 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s  ", ptr);
		index += 1;
		//��ص�ѹ
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//�����¶�
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "�¶�: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR ������
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx�ŵ�
		dispIdx += sprintf(&dispBuf[dispIdx], "�ŵ�: Tx-%d, Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//Э��汾
		dispIdx += sprintf(&dispBuf[dispIdx], "Э��汾: %d\n", buf[index]);
		index += 1;
		break;

	//---------------------------------------		���ط���	---------------------
	case WaterCmd_OpenValve:		// ����
	case WaterCmd_OpenValveForce:	// ǿ�ƿ���
	case WaterCmd_CloseValve:		// �ط�
	case WaterCmd_CloseValveForce:	// ǿ�ƹط�
	case CenterCmd_OpenValve:
	case CenterCmd_CloseValve:
		if(rxlen < index + 3 || cmd != 0x03){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAB){
			u16Tmp = (buf[index] + buf[index + 1] * 256);
			dispIdx += sprintf(&dispBuf[dispIdx], "ԭ��: ");
			dispIdx += Water6009_GetStrValveCtrlFailed(u16Tmp, &dispBuf[dispIdx]);
			index += 2;
		}
		break;

	//----------------------------------------		��ȡ���������Ϣ		-------------
	case WaterCmd_ReadMeterCfgInfo:	// ��ȡ���������Ϣ

		if(rxlen < index + 124 || cmd != 0x04){
			break;
		}
		ret = CmdResult_Ok;
		index += 84;
		memcpy(&VerInfo[0], &buf[index], VerLen);
		VerInfo[VerLen] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "�汾: %s\n", &VerInfo[0]);
		index += VerLen;
		break;

	//---------------------------------------		���쳣����		---------------------
	case WaterCmd_ClearException:	// ���쳣���� 
	case CenterCmd_ClearException:
		if(rxlen < index + 1 || cmd != 0x05){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	//---------------------------------------		��������	---------------------
	case WaterCmd_RebootDevice:	// �������
		if(rxlen < index + 2 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadTemperature:	// �����¶�
		if(rxlen < index + 1 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		//�����¶�
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "�¶�: %s%d\n", ptr, (buf[index] & 0x7F));
        index += 1;
		break;

	case WaterCmd_ReadVoltage:	// �����ѹ
		if(rxlen < index + 1 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		//��ص�ѹ
		dispIdx += sprintf(&dispBuf[dispIdx], "��ص�ѹ: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		break;

	case WaterCmd_ClearPrepaidRefVal:	// ��Ԥ�ɲο���
		if(rxlen < index + 1 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_SetOverCurrentTimeout:	// ���ù�����ʱ
		if(rxlen < index + 3 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// ����״̬
		if(buf[index] != Args.buf[2] || buf[index + 1] != Args.buf[3] || buf[index + 2] != Args.buf[4]){
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
			index += 1;
			ret = CmdResult_Failed;
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
			// ��������
			dispIdx += sprintf(&dispBuf[dispIdx], "��������: %d mA\n", buf[index]);
			index += 1;
			// ��ʱʱ��
			dispIdx += sprintf(&dispBuf[dispIdx], "��ʱʱ��: %d ms\n", (buf[index] + buf[index + 1] * 256));
			index += 2;
		}
		break;

	case WaterCmd_ReadOperatorNumber:	// ����Ӫ�̱��
		if(rxlen < index + 4 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// ��Ӫ�̱��
		dispIdx += sprintf(&dispBuf[dispIdx], "��Ӫ�̱��: %2X%2X%2X%2X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		break;

	case WaterCmd_ReadReportRoute:	// ���ϱ�·��
		if(rxlen < index + 63 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;

		// ·��1���� 6*n
		u8Tmp = buf[index] / 6;
		dispIdx += sprintf(&dispBuf[dispIdx], "·��1����: %d\n", u8Tmp);
		index += 1;
		// ·��1��ַ�б�
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		index += (30 - u8Tmp * 6);

		// ·��2���� 6*n
		u8Tmp = buf[index] / 6;
		dispIdx += sprintf(&dispBuf[dispIdx], "  \n·��2����: %d\n", u8Tmp);
		index += 1;
		// ·��2��ַ�б�
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		index += (30 - u8Tmp * 6);

		break;

	case WaterCmd_SetMeterNumber:	// ���ñ��
		if(rxlen < index + 1 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "�±��: %s\n", &TmpBuf[0]);
		}
		else{
			ret = CmdResult_Failed;
		}
		index += 6;
		break;

	case WaterCmd_ReadDebugInfo:		// ��debug��Ϣ
		if(rxlen < index + 58 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// debug��Ϣ
		dispIdx += sprintf(&dispBuf[dispIdx], "ģ�鷢�����: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "ģ����մ���: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "��/�ط�����: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "�Ÿ��Ŵ��� : %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "EEPROMд�����: %d\n",
			 (buf[index] + (buf[index + 1] << 8) + (buf[index + 2] << 16) + (buf[index + 3] << 24)));
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "48Сʱ��λ����: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ʼ������: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ɳ�ʼ������: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "���Ź���λ����: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "CDA��100ms����: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "���ճ�ʱ����: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "A�ɻɹܱպϴ���: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "B�ɻɹܱպϴ���: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "IDУ�����: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "�յ��Ƿ�������: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "���붨ʱ��ʧ��: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " ����:%02X %02X %02X %02X %02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4]);
		index += 5;
		dispIdx += sprintf(&dispBuf[dispIdx], "��������ʧ��: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " ����:%02X %02X %02X %02X %02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4]);
		index += 5;
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ڴ�ʧ��: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " ����:%02X %02X %02X %02X %02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4]);
		index += 5;
		for(i = 0; i < 10; i++){
			dispIdx += sprintf(&dispBuf[dispIdx], "��ʱ����%c: %d\n", ('A' + i), buf[index]);
			index += 1;
		}
		break;

	case WaterCmd_ClearDebugInfo:		// ��debug��Ϣ
		if(rxlen < index + 1 || cmd != 0x07){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		Ԥ���������ο�����-��ȡ/����	---------------------
	case WaterCmd_ReadPrepaidRefVal:	// ��Ԥ�ɲο�����
		if(rxlen < index + 12 || cmd != 0x15){
			break;
		}
		ret = CmdResult_Ok;
		// Ԥ������
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "Ԥ������: %d.%03d\n", u32Tmp, u16Tmp);
		// �ο�����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "�ο�����: %d.%03d\n", u32Tmp, u16Tmp);
		break;

	case WaterCmd_SetPrepaidRefVal:		// ��Ԥ�ɲο�����
		if(rxlen < index + 2 || cmd != 0x16){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "���ݷǷ�");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		// ���ݷǷ�ԭ��
		if(buf[index - 1] == 0xAE){
			if((buf[index] & 0x01) > 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "-->�ο���ʼ�������Ϸ�\n");
			}
			if((buf[index] & 0x02) > 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "-->���õ�Ԥ������δ�ﵽ��������\n");
			}
			index += 1;
		}
		break;

	case WaterCmd_ReadAlarmLimitOverdraft:		// �������ط���ֵ
		if(rxlen < index + 1 || cmd != 0x17){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = (buf[index] == 0xAB ? "����ʧ��" : "�����ɹ�");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		// �����ɹ�ʱ���
		if(buf[index - 1] == 0xAA){
			dispIdx += sprintf(&dispBuf[dispIdx], "������ֵ: %d\n", buf[index]);
			index += 1;
			u16Tmp = ((uint16)(buf[index + 1] << 8) + buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "�ط���ֵ: %s%d\n", 
				((u16Tmp & 0x8000) > 0 ? "-" : ""), (u16Tmp & 0x7FFF));
			index += 2;
		}
		break;

	case WaterCmd_SetAlarmLimit:				// �豨����ֵ
	case WaterCmd_SetCloseValveLimit:			// ��ط���ֵ
	case WaterCmd_SetAlarmAndCloseValveLimit:	// �豨���ط���ֵ
		if(rxlen < index + 2 || cmd != 0x18){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		// �°����+ 2 byte
		break;

	//--------------------------------------		��������	---------------------
	case WaterCmd_SetBaseValPulseRatio:	// ����������ϵ��
		if(rxlen < index + 7 || cmd != 0x06){
			break;
		}
		ret = CmdResult_Ok;
		// �û�����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "�û�����:%d.%03d\n", u32Tmp, u16Tmp);
		// ����ϵ��
		switch (buf[index]){
		case 0x00:	u16Tmp = 1;	break;
		case 0x01:	u16Tmp = 10;	break;
		case 0x02:	u16Tmp = 100;	break;
		case 0x03:	u16Tmp = 1000;	break;
		default:  u16Tmp = buf[index];	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "����ϵ��: %d����/��\n", u16Tmp);
		index += 1;
		break;

	case WaterCmd_ClearReverseMeasureData:	// �����ת��������
		if(rxlen < index + 6 || cmd != 0x0A){
			break;
		}
		ret = CmdResult_Ok;
		// ��ת����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ת����:%d.%03d\n", u32Tmp, u16Tmp);
		break;

	case WaterCmd_ReadFuncEnableState:	// ��ȡ����ʹ��״̬
	case CenterCmd_ReadEnableState:
		if(rxlen < index + 2 || cmd != 0x0B){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "����ʹ��״̬����\n");
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += Water6009_GetStrMeterFuncEnableState(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	case WaterCmd_SetFuncEnableState:	// ���ù���ʹ��״̬
		if(rxlen < index + 1 || cmd != 0x08){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_SetTimedUpload:		// ���ö�ʱ�ϴ�
	case WaterCmd_SetFixedValUpload:	// ���ö����ϴ�
	case WaterCmd_SetTimedAndFixedValUpload:	// ���ö�ʱ�����ϴ�
		if(rxlen < index + 2 || cmd != 0x0C){
			break;
		}
		ret = CmdResult_Ok;
		if(buf[index] == 0xAA){
			dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
			index += 1;
		}
		if(rxlen >= index + 2 + 4){		// ��Э������
			dispIdx += sprintf(&dispBuf[dispIdx], "��ʱ�ϴ����: %d h\n", buf[index]);
			index += 1;
			dispIdx += sprintf(&dispBuf[dispIdx], "�����ϴ����: %d m3\n", buf[index]);
			index += 1;
		}
		break;

	case WaterCmd_ReadMeterTime:	// �����ʱ��
		if(rxlen < index + 7 || cmd != 0x13){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "���ʱ��: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		break;

	case WaterCmd_SetMeterTime:		// У���ʱ��
		if(rxlen < index + 1 || cmd != 0x14){
			break;
		}
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_SetIpPortMode:		// ��IP+�˿�+ģʽ
		if(rxlen < index + 2 || cmd != 0x0D){
			break;
		}
		// ����ѡ������
		index += 1;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadIpPortMode:		// ��IP+�˿�+ģʽ
		if(rxlen < index + 9 || cmd != 0x0D){
			break;
		}
		// ����ѡ������
		index += 1;
		// ����ģʽ
		switch (buf[index]){
		case 0xA0:	ptr = "Coap"; break;
		case 0xA1:	ptr = "Udp"; break;
		default: ptr = "δ֪"; break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "����ģʽ: %s\n", ptr);
		index += 1;
		// Ip��ַ
		dispIdx += sprintf(&dispBuf[dispIdx], "Ip��ַ: %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		// �˿ں�
		dispIdx += sprintf(&dispBuf[dispIdx], "�˿ں�: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadModuleFrequency:		// ��ȡģ���Ƶ��
		if(rxlen < index + 8 || cmd != 0x1B){
			break;
		}
		// ����ѡ������
		index += 1;
		// Ƶ��1
		dispIdx += sprintf(&dispBuf[dispIdx], "Ƶ��1: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// Ƶ��2
		dispIdx += sprintf(&dispBuf[dispIdx], "Ƶ��2: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// Ƶ��3
		dispIdx += sprintf(&dispBuf[dispIdx], "Ƶ��3: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_SetModuleFrequency:		// ����ģ���Ƶ��
		if(rxlen < index + 1 || cmd != 0x1B){
			break;
		}
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadNbOperaterNumber: // ��NB��Ӫ�̱��
		if(rxlen < index + 7 || cmd != 0x0E){
			break;
		}
		// ����ѡ������
		index += 1;
		// ��Ӫ�̱��/SIM������
		GetStringHexFromBytes(TmpBuf, buf, index, 4, 0, false);
		if(TmpBuf[3] == '0' && TmpBuf[4] == '4'){
			ptr = "�й��ƶ�";
		}else if(TmpBuf[3] == '0' && TmpBuf[4] == '1'){
			ptr = "�й���ͨ";
		}else if(TmpBuf[3] == '1' && TmpBuf[4] == '1'){
			ptr = "�й�����";
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "��Ӫ�̱��: %s\n", TmpBuf);
		dispIdx += sprintf(&dispBuf[dispIdx], " SIM������: %s\n", ptr);
		index += 4;
		// ͨ��Ƶ��
		dispIdx += sprintf(&dispBuf[dispIdx], "  ͨ��Ƶ��: %d\n", buf[index]);
		index += 1;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadImeiAndCcid:		// ��IMEI+ICCID
		if(rxlen < index + 22 || cmd != 0x0F){
			break;
		}
		// ����ѡ������
		index += 1;
		// ICCID
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 10, 0, false);
		TmpBuf[20] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "ICCID: %s\n", &TmpBuf[0]);
		index += 10;
		// IMEI
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 10, 0, false);
		TmpBuf[20] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], " IMEI: %s\n", &TmpBuf[0]);
		index += 10;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadNbJoinNetworkInfo:		// ��ȡNB������Ϣ
		if(rxlen < index + 34 || cmd != 0x10){
			break;
		}
		ret = CmdResult_Ok;
		// ��ص�ѹ
		dispIdx += sprintf(&dispBuf[dispIdx], "Battery: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		// �źŹ���
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "Signal power: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// �ܹ���
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], " Total power: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// ���书��
		dispIdx += sprintf(&dispBuf[dispIdx], "TX power: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// ����ʱ�� ms
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "TX time: %d\n", u32Tmp);
		index += 4;
		// ����ʱ�� ms
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "RX time: %d\n", u32Tmp);
		index += 4;
		// ��ǰCell ID
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "Cell ID: %d\n", ((buf[index + 4] << 24) * 256 + u32Tmp));
		index += 5;
		// ECL
		dispIdx += sprintf(&dispBuf[dispIdx], "    ECL: %d\n", buf[index]);
		index += 1;
		// �����
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "    SNR: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// ����Ƶ���ŵ���
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], " EARFUN: %d\n", ((buf[index + 4] << 24) * 256 + u32Tmp));
		index += 5;
		// ����Cell ID
		dispIdx += sprintf(&dispBuf[dispIdx], "    PCI: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// �ο��źŽ������� 
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "   RSRQ: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// ����ģʽ
		dispIdx += sprintf(&dispBuf[dispIdx], "Operator Mode: %d\n", buf[index]);
		index += 1;
		// ע��״̬
		dispIdx += sprintf(&dispBuf[dispIdx], "RegisterState: %s\n", (buf[index] == 0 ? "ʧ��" : "�ɹ�"));
		index += 1;
		break;

	case WaterCmd_ReadBeiJingWaterMeterParams:		// ��ȡ����ˮ�����
		if(rxlen < index + 69 || cmd != 0x26){
			break;
		}
		ret = CmdResult_Ok;
		// ����ѡ������
		index += 1;
		// CCID
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 10, 0, false);
		//TmpBuf[20] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "CCID��: %s\n", &TmpBuf[0]);
		index += 10;
		// �豸����
		ptr = WaterBeiJing_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�豸����: %s\n", ptr);
		index += 1;
		// �������
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 7, 0, false);
		TmpBuf[14] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "�������: \n  %s\n", &TmpBuf[0]);
		index += 7;
		// �ն�ʱ�� 
		dispIdx += sprintf(&dispBuf[dispIdx], "�ն�ʱ��:\n 20%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], 
			buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		// ����汾
		dispIdx += sprintf(&dispBuf[dispIdx], "����汾: %X\n", buf[index]);
		index += 1;
		// �����澯��ֵ L/h
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�����澯��ֵ:%d L/h\n", u32Tmp);
		index += 4;
		// �����澯ʱ�� 1����
		dispIdx += sprintf(&dispBuf[dispIdx], "�����澯ʱ��:%d����\n", buf[index]);
		index += 1;
		// �����澯��ֵ L/h
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�����澯��ֵ:%d L/h\n", u32Tmp);
		index += 4;
		// �����澯ʱ�� 1����
		dispIdx += sprintf(&dispBuf[dispIdx], "�����澯ʱ��:%d����\n", buf[index]);
		index += 1;
		// ��ѹ�澯��ֵ 0.01V
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ѹ�澯��ֵ:%d x0.01V\n", u16Tmp);
		index += 2;
		// IP��ַ
		dispIdx += sprintf(&dispBuf[dispIdx], "������-IP��ַ: \n  %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		// �˿ں�
		dispIdx += sprintf(&dispBuf[dispIdx], "������-�˿ں�: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// APN��Ϣ
		memcpy(&TmpBuf[0], &buf[index], 6);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "APN��Ϣ: %s\n", &TmpBuf[0]);
		index += 6;
		// �ϱ���������
		dispIdx += sprintf(&dispBuf[dispIdx], "�ϱ���������:%d\n", buf[index]);
		index += 1;
		// �����ϱ���ʼʱ�� 10����
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ϱ���ʼʱ��:\n 20%02d-%02d-%02d %02d:%02d:%02d\n", 
			buf[index], buf[index + 1], buf[index + 2], 
			buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		// �����ϱ�����ʱ�� 10����
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ϱ�����ʱ��:\n 20%02d-%02d-%02d %02d:%02d:%02d\n", 
			buf[index], buf[index + 1], buf[index + 2], 
			buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		// �����ϱ�����ʱ��
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ϱ�����:%d��\n", buf[index]);
		index += 1;
		// �ն���ͣ����
		dispIdx += sprintf(&dispBuf[dispIdx], "�ն���ͣ����:%s\n", (buf[index] == 0 ? "ͣ��" : "����"));
		index += 1;
		// �����ϱ�Ƶ��
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ϱ�Ƶ��:%dСʱ\n", buf[index]);
		index += 1;
		// �ܼ��ϱ���ʼʱ���
		dispIdx += sprintf(&dispBuf[dispIdx], "�ܼ��ϱ���ʼʱ���:\n  %d ��\n", buf[index]);
		index += 1;
		// ���ڲ������
		dispIdx += sprintf(&dispBuf[dispIdx], "���ڲ������:%d x30����\n", buf[index]);
		index += 1;
		// �ϱ������ȴ�ʱ��
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ȴ�ʱ��:%d����\n", buf[index]);
		index += 1;
		// �ܼ��������
		dispIdx += sprintf(&dispBuf[dispIdx], "�ܼ��������:%d����\n", buf[index]);
		index += 1;
		// KEY
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 16, 0, false);
		dispIdx += sprintf(&dispBuf[dispIdx], "KEY: %s\n", &TmpBuf[0]);
		index += 16;
		break;

	case WaterCmd_SetBeiJingWaterMeterParams:		// ���ñ���ˮ�����
		if(rxlen < index + 2 || cmd != 0x26){
			break;
		}
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		��������		---------------------
	case WaterCmd_ReadRxTxMgnDistbCnt:		// ���շ����ŷ�����
		if(rxlen < index + 7 || cmd != 0x09){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "�������: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "���մ���: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "���ط�����: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "�Ÿ��Ŵ���: %d\n", (buf[index]));
		index += 1;
		break;

	case WaterCmd_ReadRxdAndTxdChanel:	// ��ȡRXD��TXD�ŵ�
		if(rxlen < index + 2 || cmd != 0x1B){
			break;
		}
		ret = CmdResult_Ok;
		if(buf[index] == 0xAB){
			ret = CmdResult_Failed;
			dispIdx += sprintf(&dispBuf[dispIdx], "���: ����ʧ��\n");
			index += 1;
			if(rxlen >= index + 1 + 4){
				index += 1;		// ʧ��ԭ��
			}
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "�����ŵ�: %d\n", (buf[index] & 0x7F));
			index += 1;
			dispIdx += sprintf(&dispBuf[dispIdx], "�ϱ��ŵ�: %d\n", (buf[index] & 0x7F));
			index += 1;
		}
		break;

	case WaterCmd_SetRxdAndTxdChanel:	// ����RXD��TXD�ŵ�
		if(rxlen < index + 2 || cmd != 0x1B){
			break;
		}
		ret = CmdResult_Ok;
		if(buf[index] == 0xAB){
			ret = CmdResult_Failed;
			dispIdx += sprintf(&dispBuf[dispIdx], "���: ����ʧ��\n");
			index += 1;
			if(rxlen >= index + 1 + 4){
				index += 1;		// ʧ��ԭ��
			}
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "�����ŵ�: %d\n", (buf[index]));
			index += 1;
			dispIdx += sprintf(&dispBuf[dispIdx], "�ϱ��ŵ�: %d\n", (buf[index]));
			index += 1;
		}
		break;

	case WaterCmd_SetOperatorNumber:		// ������Ӫ�̱��
		if(rxlen < index + 1 || cmd != 0x21){
			break;
		}
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(rxlen >= index + 1 + 4){
			index += 1;		// ʧ��ԭ��
		}
		break;

	case WaterCmd_SetDefinedRoute:	// ·���·�
		if(rxlen < index + 1 || cmd != 0x22){
			break;
		}
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(rxlen >= index + 1 + 4){
			index += 1;		// ʧ��ԭ��
		}
		break;


	//--------------------------------------		��������		---------------------
	case WaterCmd_NoticeUpgrade_OnApp:		// ֪ͨϵͳ����_��app
	case WaterCmd_NoticeUpgrade_OnBoot:		// ֪ͨϵͳ����_��boot
		if(rxlen < index + 41 || (cmd != 0x70 && cmd != 0x71)){
			break;
		}
		ret = CmdResult_Ok;
		// ������������ʼλ�ã��Ժ��Խ���
		dispBuf[0] = (uint8)(index & 0xFF);		
		dispBuf[1] = (uint8)(index >> 8);
		dispIdx += 20;
		break;

	case WaterCmd_SendUpgradePacket:			// ������������
		if(rxlen < index + 12 || cmd != 0x72){
			break;
		}
		ret = CmdResult_Ok;
		break;

	case WaterCmd_QueryUpgradeStatus_OnBoot:	// ��ѯ����״̬_��boot
	case WaterCmd_QueryUpgradeStatus_OnApp:		// ��ѯ����״̬_��app
		if(rxlen < index + 12 || (cmd != 0x73 && cmd != 0x74)){
			break;
		}
		ret = CmdResult_Ok;
		// ������������ʼλ�ã��Ժ��Խ���
		dispBuf[0] = (uint8)(index & 0xFF);		
		dispBuf[1] = (uint8)(index >> 8);
		dispIdx += 20;
		break;


	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	���������� 0x40 ~ 0x65 , F1 ~ F3	--------------------
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		���ò���		-------------------------------------
	case CenterCmd_ReadCenterNo:	// ����������
		if(rxlen < index + 6 || cmd != 0x41){
			break;
		}
		ret = CmdResult_Ok;
		// ��������
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: \n    %02X%02X%02X%02X%02X%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		break;

	case CenterCmd_ReadCenterVer:		// ���������汾
		if(rxlen < index + 6 || cmd != 0x40){
			break;
		}
		ret = CmdResult_Ok;
		// ����汾
		dispIdx += sprintf(&dispBuf[dispIdx], "����汾: %02X%02X \n", buf[index], buf[index + 1]);
		index += 2;
		// Ӳ���汾
		dispIdx += sprintf(&dispBuf[dispIdx], "Ӳ���汾: %02X%02X \n", buf[index], buf[index + 1]);
		index += 2;
		// Э��汾
		dispIdx += sprintf(&dispBuf[dispIdx], "Э��汾: %02X%02X \n", buf[index], buf[index + 1]);
		index += 2;
		break;

	case CenterCmd_ReadCenterTime:		// ��������ʱ��
		if(rxlen < index + 7 || cmd != 0x43){
			break;
		}
		ret = CmdResult_Ok;
		// ������ʱ��
		dispIdx += sprintf(&dispBuf[dispIdx], "������ʱ��: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		break;

	case CenterCmd_SetCenterTime:		// �輯����ʱ��
		if(rxlen < index + 1 || cmd != 0x44){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadGprsParam:		// ��GPRS����
		if(rxlen < index + 16 || cmd != 0x45){
			break;
		}
		ret = CmdResult_Ok;
		// ����IP + �˿�
		dispIdx += sprintf(&dispBuf[dispIdx], "��ѡIP: %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "  �˿�: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// ����IP + �˿�
		dispIdx += sprintf(&dispBuf[dispIdx], "����IP: %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "  �˿�: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// ���������
		dispIdx += sprintf(&dispBuf[dispIdx], "���������: %d s\n", (buf[index] * 10));
		// A P N
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "A P N: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		// �û���
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "�û���: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		// ��  ��
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "��  ��: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		break;

	case CenterCmd_SetGprsParam:		// ��GPRS����
		if(rxlen < index + 1 || cmd != 0x46){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadGprsSignal:		// ��GPRS�ź�ǿ��
		if(rxlen < index + 4 || cmd != 0x47){
			break;
		}
		ret = CmdResult_Ok;
		// �ź�ǿ��
		if(buf[index] == 0){
			ptr = "<= -113dBm";
		}
		else if(buf[index] < 31){
			sprintf(&TmpBuf[0], "-%ddBm", 113 - 2 * buf[index]);
			ptr = &TmpBuf[0];
		}
		else if(buf[index] == 31){
			ptr = ">= -53dBm";
		}
		else if(buf[index] == 99){
			ptr = "δ֪";
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "�ź�ǿ��: %s\n", ptr);
		index += 1;
		// ����״̬
		ptr = (buf[index] == 0 ? "����" : "����");
		dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s\n", ptr);
		index += 1;
		// IMSI
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "IMSI: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		// GMM
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "GMM: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		break;

	case CenterCmd_InitCenter:		// ��������ʼ��
		if(rxlen < index + 2 || cmd != 0x48){
			break;
		}
		ret = CmdResult_Ok;
		// �����ϴ�����
		ptr = (buf[index] == 0 ? "��յ�����·��" : "�����������");
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", ptr);
		index += 1;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadCenterWorkMode:		// ������������ģʽ
		if(rxlen < index + 7 || cmd != 0x49){
			break;
		}
		ret = CmdResult_Ok;
		// ��������
		ptr = (buf[index] == 0 ? "����ʱ����" : (buf[index] == 1 ? "������" : "��Ч"));
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %s\n", ptr);
		index += 1;
		// ���ݲ�������
		ptr = ((buf[index] & 0x80) > 0 ? "��" : "��");
		dispIdx += sprintf(&dispBuf[dispIdx], "���ݲ�������: %s\n", ptr);
		// �����ϴ�����
		ptr = ((buf[index] & 0x40) > 0 ? "��" : "��");
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ϴ�����: %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ϴ�ʱ��: %2X��\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "���ݲ�������-λ���: \n");
		dispIdx += sprintf(&dispBuf[dispIdx], " 1- 7�� �������: %02X\n", (buf[index] & 0x7F));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " 8-15�� �������: %02X\n", (buf[index]));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "16-23�� �������: %02X\n", (buf[index]));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "24-31�� �������: %02X\n", (buf[index]));
		index += 1;
		break;


	//--------------------------------------		����������		-------------------------------------
	case CenterCmd_ReadDocCount:		// ����������
		if(rxlen < index + 3 || cmd != 0x50){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water6009_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�豸����: %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		break;

	case CenterCmd_ReadDocInfo:			// ��������Ϣ
		if(rxlen < index + 3 || cmd != 0x51){
			break;
		}
		ret = CmdResult_Ok;
		// �ڵ�������
		dispIdx += sprintf(&dispBuf[dispIdx], "�ڵ�������  : %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// �ڵ�������
		dispIdx += sprintf(&dispBuf[dispIdx], "����Ӧ������: %d\n", (buf[index]));
		index += 1;
		// N���ڵ���Ϣ
		u8Tmp = buf[index - 1];
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
			TmpBuf[6] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "�ڵ� %2d:%s\n", i + 1, &TmpBuf[0]);
			index += 6;
			ptr = Water6009_GetStrDeviceType(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   ����:%s\n", ptr);
			index += 1;
			ptr = (buf[index] == 0 ? "ʧ��" : (buf[index] == 1 ? "�ɹ�" : "δ֪"));
			dispIdx += sprintf(&dispBuf[dispIdx], "   ����:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_AddDocInfo:			// ��ӵ�����Ϣ
		if(rxlen < index + 1 || cmd != 0x52){
			break;
		}
		ret = CmdResult_Ok;
		// ���õĽڵ�����
		dispIdx += sprintf(&dispBuf[dispIdx], "���õĽڵ�����: %d\n", (buf[index]));
		index += 1;
		// N���ڵ����ý��
		u8Tmp = buf[index - 1];
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
			TmpBuf[6] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "�ڵ� %2d:%s\n", i + 1, &TmpBuf[0]);
			index += 6;
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   ���:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_DeleteDocInfo:			// ɾ��������Ϣ
		if(rxlen < index + 7 || cmd != 0x53){
			break;
		}
		ret = CmdResult_Ok;
		// N���ڵ����ý��
		u8Tmp = (rxlen - index - 4) / 7;
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
			TmpBuf[6] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "�ڵ� %2d:%s\n", i + 1, &TmpBuf[0]);
			index += 6;
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   ���:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_ModifyDocInfo:			// �޸ĵ�����Ϣ
		if(rxlen < index + 1 || cmd != 0x54){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		·�����ã�		-------------------------------------
	case CenterCmd_ReadDefinedRoute:		// ���Զ���·��
		if(rxlen < index + 10 || cmd != 0x55){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		// ���
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", &TmpBuf[0]);
		index += 6;
		// �豸����
		ptr = Water6009_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", ptr);
		index += 1;
		// ·��1�м���
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "·��1�м���: %d\n", u8Tmp);
		index += 1;
		// ·��1��ַ�б�
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		// ·��2�м���
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "·��2�м���: %d\n", u8Tmp);
		index += 1;
		// ·��2��ַ�б�
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		break;

	case CenterCmd_SetDefinedRoute:			// ���Զ���·��
		if(rxlen < index + 7 || cmd != 0x56){
			break;
		}
		ret = CmdResult_Ok;
		// ���
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", &TmpBuf[0]);
		index += 6;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		����ת����		-------------------------------------
	case CenterCmd_ReadFixedTimeData:			// ����ʱ��������
		if(rxlen < index + 28 || cmd != 0x63){
			break;
		}
		ret = CmdResult_Ok;
		// ���
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", &TmpBuf[0]);
		index += 6;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		// ʱ��
		dispIdx += sprintf(&dispBuf[dispIdx], "ʱ��: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		// ����
		ptr = Water6009_GetStrValueType((buf[index] >> 4));
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", ptr);
		index += 1;
		// ��ת����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ת: %d.%03d\n", u32Tmp, u16Tmp);
		// ��ת����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ת: %d.%03d\n", u32Tmp, u16Tmp);
		//�澯״̬��
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "�澯: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		//����״̬ 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s  ", ptr);
		index += 1;
		//��ص�ѹ
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//�����¶�
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "�¶�: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR ������
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx�ŵ�
		dispIdx += sprintf(&dispBuf[dispIdx], "�ŵ�: Tx-%d, Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//Э��汾
		dispIdx += sprintf(&dispBuf[dispIdx], "Э��汾: %d\n", buf[index]);
		index += 1;
		break;

	case CenterCmd_ReadFrozenData:			// ����������
		if(rxlen < index + 7 || cmd != 0x64){
			break;
		}
		ret = CmdResult_Ok;
		// ���
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", &TmpBuf[0]);
		index += 6;
		// ����״̬
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		// ������������
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", (buf[index] == 0x01 ? "����" : "��ת"));
		index += 1;

		if(rxlen < index + 104){	// �������ݸ�ʽ-�ɰ汾 1 + 78 byte
			// ����������ʼ���
			u8Tmp = buf[index] * 10;
			dispIdx += sprintf(&dispBuf[dispIdx], "��Χ: �� %d-%d ��\n", u8Tmp, u8Tmp + 9);
			index += 1;
			// ����������ʼʱ��
			dispIdx += sprintf(&dispBuf[dispIdx], "ʱ��: %X%x%x%x %x:00:00\n"
				, buf[payloadIdx + 2], buf[payloadIdx + 3], buf[payloadIdx + 4], buf[payloadIdx + 5], buf[payloadIdx + 6]);
			index += 5;
			// �������ݷ�ʽ ��0-����, 1-����
			// ������������ ���������24�����������30��
			dispIdx += sprintf(&dispBuf[dispIdx], "��ʽ: ÿ%s����%d��\n", (buf[index] == 0x01 ? "��" : "��"), buf[index + 1]);
			index += 2;	
			// ��������ʱ����
			if(buf[index] == 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "���: ÿ%s����1��\n", (buf[index - 2] == 0x01 ? "��" : "��"));
			}
			else{
				dispIdx += sprintf(&dispBuf[dispIdx], "���: %d%s����1��\n", buf[index], (buf[index - 2] == 0x01 ? "Сʱ" : "��"));
			}
			index += 1;
			// ������������ݣ�7*N �ֽ� ��6 byte ���� + 1 byte date.day��
			dispIdx += sprintf(&dispBuf[dispIdx], "��ȡ��10����������: \n");
			for(i = 0; i < 10; i++){
				u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
				index += 4;
				u16Tmp = ((buf[index + 1] << 8) + buf[index]);
				index += 2;
				dispIdx += sprintf(&dispBuf[dispIdx], "%d, %x/%x: %d.%03d\n", i, buf[payloadIdx + 4], buf[index], u32Tmp, u16Tmp);
				index +=1;
			}
		}
		else{		// �������ݸ�ʽ-�°汾	1 + 104 byte
			// ����������ʼ���
			dispIdx += sprintf(&dispBuf[dispIdx], "��Χ: ������%d������\n", buf[index] + 1);
			index += 1;
			// ʱ����Ϣ
			dispIdx += sprintf(&dispBuf[dispIdx], "ʱ��: %02X-%02X %02X:%02X\n",
				buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			index += 4;
			// �ۼ�����
			u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
			index += 4;
			u16Tmp = ((buf[index + 1] << 8) + buf[index]);
			index += 2;
			dispIdx += sprintf(&dispBuf[dispIdx], "�ۼ�����: %d.%03d\n", u32Tmp, u16Tmp);
			// 0:00 ~ 23:30 ����
			u8Tmp = 0;
			u16Tmp = 0x00;
			for(i = 0; i < 47; i++){
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X-", u8Tmp, u16Tmp);
				u16Tmp += 0x30;
				if(u16Tmp == 0x60){
					u16Tmp = 0x00;
					u8Tmp += 1;
				}
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X����:%d\n", u8Tmp, u16Tmp, (buf[index] + buf[index + 1]*256));
				index += 2;
			}
		}

		//�澯״̬��
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "�澯: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		//����״̬ 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s  ", ptr);
		index += 1;
		//��ص�ѹ
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//�����¶�
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "�¶�: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR ������
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx�ŵ�
		dispIdx += sprintf(&dispBuf[dispIdx], "�ŵ�: Tx-%d , Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//Э��汾
		dispIdx += sprintf(&dispBuf[dispIdx], "Э��汾: %d\n", buf[index]);
		index += 1;
		break;

	//--------------------------------------		UART���ģ����ԣ�		---------------------
	case WaterCmd_ReadModuleRunningParams:		// ��ȡģ�����в���
		if(rxlen < index + 124 || cmd != 0x3A){
			break;
		}
		ret = CmdResult_Ok;
		// ģ�����в���
		ptr = Water6009_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�Ǳ�����: %s\n", ptr);
		index += 1;
		switch (buf[index]){
		case 0x00:	u16Tmp = 1;	break;
		case 0x01:	u16Tmp = 10;	break;
		case 0x02:	u16Tmp = 100;	break;
		case 0x03:	u16Tmp = 1000;	break;
		default:  u16Tmp = buf[index];	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "����ϵ��: %d����/��\n", u16Tmp);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "���ſ���ʱ��: %d s\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "����պ�ʱ��: %d ms\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "���ط�ʱ��: %d ms\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "������ֵ: %d mA\n",buf[index]);
		index += 1;
		switch (buf[index]){
		case 0x00:	ptr = "3.6 v";	break;
		case 0x01:	ptr = "6.0 v";	break;
		case 0x02:	ptr = "4.5 v";	break;
		default:  ptr = "δ֪";	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "��ص�ѹ����: %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ʱ�ϴ����: %d h\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "�����ϴ����: %d m3\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "ÿ���ϴ�����: %d\n", buf[index]);
		index += 1;
		ptr = Water6009_GetStrSensorType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "��������������: \n  %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "������-IP��ַ: \n  %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "������-�˿ں�: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "ˮ�������: %d��\n", (buf[index] + buf[index + 1] * 256));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "ϵͳ���Լ���: %d\n", buf[index]);
		index += 1;
		GetStringHexFromBytes(TmpBuf, buf, index, 4, 0, false);
		if(TmpBuf[3] == '0' && TmpBuf[4] == '4'){
			ptr = "�й��ƶ�";
		}else if(TmpBuf[3] == '0' && TmpBuf[4] == '1'){
			ptr = "�й���ͨ";
		}else if(TmpBuf[3] == '1' && TmpBuf[4] == '1'){
			ptr = "�й�����";
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "��Ӫ�̱��: %s\n", TmpBuf);
		dispIdx += sprintf(&dispBuf[dispIdx], " SIM������: %s\n", ptr);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "��ǰϵͳʱ��: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		dispIdx += sprintf(&dispBuf[dispIdx], "ͨ��Ƶ��: Band %d\n", buf[index]);
		index += 1;
		switch (buf[index]){
		case 0x01:	ptr = "COAP";	break;
		case 0x02:	ptr = "UDP";	break;
		default:  ptr = "δ֪";	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "���ӷ�ʽ: %s\n", ptr);
		index += 1;
		index += 2; // ����
		dispIdx += sprintf(&dispBuf[dispIdx], "������ֵ: %d\n", buf[index]);
		index += 1;
		u16Tmp = ((uint16)(buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ط���ֵ: %s%d\n", 
			((u16Tmp & 0x8000) > 0 ? "-" : ""), (u16Tmp & 0x7FFF));
		index += 2;
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "Ԥ������:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ο�����:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "ģ�����״̬����\n");
		dispIdx += Water6009_GetStrTestStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		index += 2; // ����
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ת����:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ת����:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s\n", ptr);
		index += 1;
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "����ʹ��״̬����\n");
		dispIdx += Water6009_GetStrMeterFuncEnableState(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "�澯״̬: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "���հ�������: ��%s\n", (u8Tmp == 0x01 ? "��" : "��"));
		index += 1;	
		dispIdx += sprintf(&dispBuf[dispIdx], "������ʼʱ��: %02d%s\n",  buf[index], (u8Tmp == 0x01 ? "��" : "��"));
		index += 1;	
		dispIdx += sprintf(&dispBuf[dispIdx], "��������ʱ��: %d%s\n",  buf[index], (u8Tmp == 0x01 ? "Сʱ" : "��"));
		index += 1;	
		index += 10; // ����
		memcpy(&VerInfo[0], &buf[index], VerLen);
		VerInfo[VerLen] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "����汾: %s\n", &VerInfo[0]);
		index += VerLen;
		break;

	case WaterCmd_SetModuleRunningParams:		// ����ģ�����в���
		if(rxlen < index + 1 || cmd != 0x3F){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_NbReportRealTimeDataNow:		// NB�����ϱ�ʵʱ����
		if(rxlen < index + 1 || cmd != 0x21){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		switch (buf[index])
		{
		case 0xAA:
			ptr = "�����ɹ�";
			ret = CmdResult_Ok;
			break;

		case 0xCE:
			ptr = "NB�����ϱ�";
			ret = CmdResult_Ok;
			break;

		case 0xAB:
		default:
			ptr = "����ʧ��";
			ret = CmdResult_Ok;
			break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	default:
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "������[%02X]��δ����\n", cmd);
		break;
	}

#if UseCrc16
	if(index == startIdx + length - 5)
#else
	if(index == startIdx + length - 4)
#endif
	{
		//����/���� �ź�ǿ��
		#ifdef Use_DBF
		#if defined Project_6009_RF || defined Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.signalValue, "%d", buf[index + 1]);	// ��������
			}
		#endif
		#endif
		dispIdx += sprintf(&dispBuf[dispIdx], "                    \n");
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %d  ����: %d\n", buf[index], buf[index + 1]);
		index += 2;
	}

	if(ret == CmdResult_Failed){
		dispIdx += sprintf(&dispBuf[dispIdx], "Error: Bad Response!\n");
	}

	dispBuf[dispIdx] = '\0';
	
	
	return ret;
}

//--------------------------------------	6009ˮ������ ���͡����ա������ʾ	----------------------------

/*
* ������ �����/���ս���	- ִ����ɺ󣬵ȴ���������/�¼� - ������ʾ�� ȷ��/ȡ���� - ����
* ������ cmdid	- ��ǰ�����ʶ
*		addrs	- ��ַ��		
*		args	- ���������args->items[0] - ����ID, args->items[1] - ������
*		ackLen	- Ӧ�𳤶� (byte)
*		timeout	- ��ʱʱ�� (ms)  Ĭ��Ϊ 8s + �м��� x 2 x 6s
*		tryCnt	- ���Դ��� Ĭ��3��
* ���أ� uint8	- �����˳�ʱ�İ���ֵ��ȷ�ϼ���ȡ����	
*/
uint8 Protol6009TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
#if defined Project_6009_IR || defined Project_6009_IR_HX
	ackLen += 15 + addrs->itemCnt * AddrLen;
	timeout = 2000;
	tryCnt = 3;
#elif defined Project_6009_RF || defined Project_6009_RF_HL
	ackLen += 15 + addrs->itemCnt * AddrLen;
	timeout = 10000 + (addrs->itemCnt - 2) * 6000 * 2;
	tryCnt = 3;
#else // Project_8009_RF
	ackLen += 10 + addrs->itemCnt * AddrLen;
	timeout = 2000 + (addrs->itemCnt - 1) * 2000;
	tryCnt = 2;
#endif

	FramePack = PackWater6009RequestFrame;
	FrameExplain = ExplainWater6009ResponseFrame;

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
