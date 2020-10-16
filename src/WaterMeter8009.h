#ifndef WaterMeter8009_H
#define WaterMeter8009_H

#include "stdio.h"
#include "Common.h"

#if defined Project_6009_RF || defined Project_8009_RF
#include "MeterDocDBF.h"
#elif defined Project_6009_RF_HL || defined Project_8009_RF_HL
#include "MeterDocDBF_HL.h"
#endif

extern uint8 PackWater8009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWater8009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

#ifndef Frame_Pack_Parse
#define Frame_Pack_Parse
FuncCmdFramePack FramePack = PackWater8009RequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWater8009ResponseFrame;
#endif

//----------------------------------------  �������  ------------------------
/*
��˲�����	
1	���ù���
2	��������
3	DMA��Ŀ
*/
typedef enum{
	/*
	���ù��ܣ�	
	1	��������
	2	������
	3	����
	4	��ط�
	5	����쳣
	6	�������
	7	���ñ��
	*/
	WaterCmd_ReadRealTimeData			= 0x11,	
	WaterCmd_SetBaseValPulseRatio,
	WaterCmd_OpenValve,	
	WaterCmd_CloseValve,
	WaterCmd_ClearException,
	WaterCmd_ReadMeterCfgInfo,
	WaterCmd_SetMeterNumber,

	/*
	�������ã�	
	1	��ȡ��ת����
	2	�����ת����
	3	������ʹ��״̬old
	4	������ʹ��״̬new
	5	���ù���ʹ��״̬
	6	��ѯʱ�Ӽ�RF״̬
	7	��ѯRF����ʱ��
	8	����RF����ʱ��
	*/
	WaterCmd_ReadReverseMeasureData		= 0x21,
	WaterCmd_ClearReverseMeasureData,
	WaterCmd_ReadFuncEnableStateOld, 
	WaterCmd_ReadFuncEnableStateNew,
	WaterCmd_SetFuncEnableState,
	WaterCmd_ReadTimeAndRfState,	
	WaterCmd_ReadRfWorkTime,
	WaterCmd_SetRfWorkTime,

	/*
	DMA��Ŀ��	
	1	�����ϴ�����Ƶ��
	2	ʹ���ϴ��������
	3	�����ϴ�ʱ����
	4	��ȡ�ϴ�ʱ����
	5	��������ת����
	6	����ʱ��
	7	��ȡʱ��
	*/
	WaterCmd_UploadCenterFrequency		= 0x31,
	WaterCmd_EnableReportAmeterData,
	WaterCmd_SetReportTimeInterval, 
	WaterCmd_ReadReportTimeInterval,
	WaterCmd_ReadFrozenData,
	WaterCmd_SetMeterTime,
	WaterCmd_ReadMeterTime

}WaterCmdDef;


//----------------------------------------  ����������  ------------------------
/*
�ɼ���������	
1	��������
2	��������
3	���Ʋ���
4	·�ɲ���
*/
typedef enum{
	/*
	����������	
	1	��ȡ�ɼ�����
	2	���òɼ�����
	3	��ȡ�ɼ���ʱ��
	4	���òɼ���ʱ��
	5	�ɼ�����ʼ��
	6	�����������
	*/
	CenterCmd_ReadCenterNo		= 0x1011,
	CenterCmd_SetCenterNo,
	CenterCmd_ReadCenterTime,
	CenterCmd_SetCenterTime,
	CenterCmd_InitCenter,
	CenterCmd_ClearMeterReadData,

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
	���Ʋ�����	
	1	����������
	2	��ʵʱ����
	3	����������
	4	����
	5	�ط�
	6	���쳣
	*/
	CenterCmd_ReadFixedValData	= 0x1031,
	CenterCmd_ReadRealTimeData,
	CenterCmd_ReadFrozenData,
	CenterCmd_OpenValve,
	CenterCmd_CloseValve,
	CenterCmd_ClearException,

	/*
	·�ɲ�����	
	1	��ȡ·���б�
	2	����·���б�
	3	��ȡ���·��
	4	���ñ��·��
	5	�鿴·���б�
	*/
	CenterCmd_ReadRouteList	= 0x1041,
	CenterCmd_SetRouteList,
	CenterCmd_ReadMeterRoute,
	CenterCmd_SetMeterRoute,
	CenterCmd_QueryRouteList

}CenterCmdDef;

//---------------------------------------		8009 ��������	-------------------------------------

/*
* ��  �������ַ�����ַ�����8009ˮ���ַ��
* ��  ����addrs			- ��ַ��ṹ
*		  strDstAddr	- Ŀ�ĵ�ַ�ַ���
* 		  strRelayAddrs - �м̵�ַ�ַ�������
* ����ֵ��void
*/
void Water8009_PackAddrs(ParamsBuf *addrs, const char strDstAddr[], const char strRelayAddrs[][20])
{
	/*
	�������ݷ�ʽ1��const char strRelayAddrs[][20]
	�������ݷ�ʽ2��const char (*strRelayAddrs)[20]
	�������ݷ�ʽ3��const char **strRelayAddrs, uint addrLen
	 */
	#ifdef Project_8009_RF
	uint8 i;
	#endif

	addrs->itemCnt = 0;
	
	#ifdef Project_8009_RF
	// �м̵�ַ
	for(i = 0; i < RELAY_MAX; i++){
		if(strRelayAddrs[i][0] >= '0' && strRelayAddrs[i][0] <= '9'){
			addrs->items[addrs->itemCnt] = &addrs->buf[i*AddrLen];
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
* ��  ������ȡ8009ˮ��-�豸������
* ��  ����typeId	- ����ID
* ����ֵ��char *	- ��������ַ���
*/
char * Water8009_GetStrDeviceType(uint8 typeId)
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
	case 0x2A:	str = "�����޴�";	break;
	case 0x2B:	str = "��Դ�޴�";	break;
	default:
		str = "δ֪";
		break;
	}

	return str;
}

/*
* ��  ������ȡ8009ˮ�����������
* ��  ����typeId	- ����ID
* ����ֵ��char *	- ��������ַ���
*/
char * Water8009_GetStrValueType(uint8 typeId)
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
* ��  ������ȡ8009ˮ�� ��������������
* ��  ����typeId	- ����ID
* ����ֵ��char *	- ��������ַ���
*/
char * Water8009_GetStrSensorType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x00:	str = "���ɻɹ�";	break;
	case 0x01:	str = "˫�ɻɹ�";	break;
	case 0x02:	str = "������";	break;
	case 0x03:	str = "˫����";	break;
	case 0x04:	str = "������";	break;
	case 0x05:	str = "���ֱ��";	break;
	default:	str = "δ֪";	break;
	}

	return str;
}

/*
* ��  ��������8009ˮ��-�澯״̬��
* ��  ����status	- ״̬��
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water8009_GetStrAlarmStatus(uint16 status, char *buf)
{
	char * str = NULL;
	uint16 mask = 1, i;
	uint16 len = 0;

	for(i = 0; i < 11; i++){

		mask = (1 << i);
		
		switch(status & mask){
		
		case 0x01:	str = "����-��ǿ�����";	break;
		case 0x02:	str = "����-�����ܻ�";	break;
		case 0x04:	str = "�Ÿ��ű�־";	break;
		case 0x08:	str = "����-һ���ܻ�";	break;
		case 0x10:	str = "���Ƿѹ";	break;
		case 0x20:	str = "EEPROM�쳣";	break;
		case 0x40:	str = "����λ����";	break;
		case 0x2000:	str = "ˮ����ֱ��װ";	break;
		case 0x4000:	str = "ˮ����ж";	break;
		case 0x8000:	str = "ˮ��ת";	break;
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
* ��  ������ȡ8009ˮ�� ����״̬
* ��  ����status	- ״̬
* ����ֵ��char *	- ��������ַ���
*/
char * Water8009_GetStrValveStatus(uint8 status)
{
	char * str = NULL;
	
	if((status & 0x40) > 0){
		str = "��";	
	}
	else if((status & 0x20) > 0){
		str = "��";	
	}
	else{
		str = "δ֪";	
	}

	return str;
}
/*
* ��  ������ȡ8009ˮ�� ��˴�����
* ��  ����status	- ״̬
* ����ֵ��char *	- ��������ַ���
*/
char * Water8009_GetStrErrorMsg(uint8 errorCode)
{
	char * str = NULL;

	switch(errorCode){
	case 0xAA:
		str = "�����ɹ�";
		break;
	case 0xAB:
	    str = "����ʧ��";
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
	    str = "������������";
		break;
	case 0xEE:
	    str = "Э�����";
		break;
	
	default:
		str = "δ֪����";
		break;
	}

	return str;
}

/*
* ��  ������ȡ8009ˮ�� ������������
* ��  ����status	- ״̬
* ����ֵ��char *	- ��������ַ���
*/
char * Water8009_GetStrCenterErrorMsg(uint8 errorCode)
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
* ��  ������ȡ8009ˮ�� ����ʧ��ԭ��
* ��  ����errorCode	- ������
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water8009_GetStrValveCtrlFailed(uint16 errorCode, char * buf)
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
		case 0x400:	str = "time����ʧ��";	break;
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
* ��  ������ȡ8009ˮ�� ����ʹ��״̬
* ��  ����stateCode	- ʹ��״̬��
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water8009_GetStrMeterFuncEnableState(uint16 stateCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], "�Ÿ��Źط�����  :%s\n", ((stateCode & 0x0001) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�Ÿ��ż�⹦��  :%s\n", ((stateCode & 0x0002) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "��ʱ�ϴ�����	   :%s\n", ((stateCode & 0x0004) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�����ϴ�����    :%s\n", ((stateCode & 0x0008) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "����ж��⹦��  :%s\n", ((stateCode & 0x0010) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "��ֱ��װ���    :%s\n", ((stateCode & 0x0020) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�����澯        :%s\n", ((stateCode & 0x0040) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "���⹦��        :%s\n", ((stateCode & 0x0080) > 0 ? "��" : " ��"));
	
	len += sprintf(&buf[len], "RF��ʱ�ι���    :%s\n", ((stateCode & 0x0100) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "����ͬ��ʱ��    :%s\n", ((stateCode & 0x0200) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�ϴ���������    :%s\n", ((stateCode & 0x0400) > 0 ? "��" : " ��"));

	return len;
}

uint16 Water8009_GetStrMeterFuncEnableStateOld(uint16 stateCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], "�Ÿ��Źط�����  :%s\n", ((stateCode & 0x0001) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�Ÿ��ż�⹦��  :%s\n", ((stateCode & 0x0002) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "����ж��⹦��  :%s\n", ((stateCode & 0x0010) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "��ֱ��װ���    :%s\n", ((stateCode & 0x0020) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�����澯        :%s\n", ((stateCode & 0x0040) > 0 ? "��" : " ��"));
	
	len += sprintf(&buf[len], "���⹦��        :%s\n", ((stateCode & 0x0080) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "��ʱ�ϴ�����	   :%s\n", ((stateCode & 0x0080) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�����ϴ�����    :%s\n", ((stateCode & 0x0080) > 0 ? "��" : " ��"));
	
	len += sprintf(&buf[len], "RF��ʱ�ι���    :%s\n", ((stateCode & 0x0100) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "����ͬ��ʱ��    :%s\n", ((stateCode & 0x0200) > 0 ? "��" : " ��"));
	len += sprintf(&buf[len], "�ϴ���������    :%s\n", ((stateCode & 0x0400) > 0 ? "��" : " ��"));

	return len;
}

/*
* ��  ������ȡ8009ˮ�� ģ�����״̬
* ��  ����statusCode	- ����״̬��
* 		  buf		- �ַ�����ʾ������
* ����ֵ��uint16	- ��������ַ����ܳ���
*/
uint16 Water8009_GetStrTestStatus(uint16 statusCode, char * buf)
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


//-----------------------------------		8009ˮ��Э�� ��� / ���	-----------------------------

/*
* ��������PackWater8009RequestFrame
* ��  �������8009ˮ����������֡
* ��  ����buf	- ���ݻ�����ʼ��ַ
		  addrs - ��ַ��Դ��ַ���м̵�ַ��Ŀ�ĵ�ַ
		  cmdId - ������
		  args	- ���������
		  retryCnt - �ط�������0 - ��1�η��ͣ����� - ��n���ط�
* ����ֵ��uint8 ֡�ܳ���
*/
uint8 PackWater8009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt)
{
	static uint16 index = 0;
	uint8 i, cmd , crc8, relayCnt;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = 0xFE;		// ֡ͷ�� �̶�Ϊ D3 91
	buf[index++] = 0x68;
	buf[index++] = 0x00;		// �豸���ͣ�00 -- ˮ�� 

	// ��ģ��ID��5 byte	BCD ��λ��ǰ
	memcpy(&buf[index], addrs->items[addrs->itemCnt - 1], AddrLen);
	index += AddrLen;			
	
	// ������2�� bit7~4 ·�ɼ��� 0 - ��·�ɣ� 4 - 1���� 8~15 - 2~9�� 
	//			bit3~0 ��ǰλ�� 0 - ������ 2 - 1���� 4~11 - 2~9�� 
	relayCnt = addrs->itemCnt - 1;
	switch (relayCnt)
	{
	case 0: buf[index++] = 0x00; break;
	case 1: buf[index++] = 0x40; break;
	case 2: buf[index++] = 0x80; break;
	default:
		break;
	}
	// ������1�� bit7 0/1-��/���У�bit6 0/1-Ӧ��/�ϱ���bit5~0 - ������
	cmd = *args->items[0];
	buf[index++] = cmd;			
	
	// �����򳤶ȣ�·��+���ݲ���
	buf[index++] = relayCnt * AddrLen + args->lastItemLen;
	
	// ������-·�ɣ����2����
	for(i = 0; i < relayCnt; i++){
		memcpy(&buf[index], addrs->items[i], AddrLen);
		index += AddrLen;
	}
	
	// ������-���ݲ���
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	buf[index++] = 0x55;		// ���г�ǿ
	buf[index++] = 0xAA;		// ���г�ǿ
	crc8 = GetCrc8(&buf[0], index);
	buf[index++] = crc8;		// crc8 У��: ֡�� -- ���г�ǿ
	buf[index++] = 0x16;		// ������

	return index;
}

/*
* ��������ExplainWater8009ResponseFrame
* ��  ��������ˮ��������Ӧ֡
* ��  ����buf		- ���ջ�����ʼ��ַ
*		  rxlen		- ���յĳ���
*		  dstAddr	- Ŀ�ĵ�ַ���жϽ��յ�Ŀ�ĵ�ַ�Ƿ����Լ�
*		  cmdId		- ������
*		  ackLen	- Ӧ�𳤶�
*		  dispBuf 	- ��������ʾ����
* ����ֵ��uint8 ���������0 - �ɹ� , 1 - ʧ�� �� 2 - CRC���� 3 - ��ʱ��Ӧ��
*/
uint8 ExplainWater8009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
{
	bool ret = CmdResult_Failed;
	uint8 crc8, relayCnt, cmd, subCmd, i, u8Tmp;
	uint16 index = 0, dispIdx, length, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	double f64Tmp;
	char *ptr;

	dispIdx = 0;

	// ��ʾ��� �� ��������
	if(cmdId < 0x1010){
		#if (AddrLen < 8)
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "���: \n   %s\n", StrDstAddr);
		#endif
	}else{
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: \n   %s\n", StrDstAddr);
	}

	// ����������в���
	while(1){

		// min length check
		if(rxlen < index + 15){		
			sprintf(&dispBuf[dispIdx], "���: ��ʱ,��Ӧ��");
			return CmdResult_Timeout;
		}

		// start check
		if(buf[index] == 0xFE && buf[index + 1] == 0x68){
			// pass
		}else{
			index++;
			continue;
		}
		
		// length check 
		length = (uint16)(buf[index + 10] + 15);
		if(length > rxlen){
			index += length;
			continue;
		}	

		// ack flag check
		if((buf[index + 9] & 0x80) == 0x00){
			index += length;
			continue;
		}

		// first node check
		if((buf[index + 8] & 0x0F) != 0x00){
			index += length;
			continue;
		}

		if(cmd != WaterCmd_SetMeterNumber && memcmp(BroadAddr, DstAddr, AddrLen) != 0){
			// dstaddr check
			if(memcmp(&buf[index + 3], DstAddr, AddrLen) != 0){
				index += length;
				continue;
			}
		}

		// crc8 check
		crc8 = GetCrc8(&buf[index], length - 2);
		if(crc8 !=  buf[index + length - 2]){
			sprintf(&dispBuf[dispIdx], "���: ��Ӧ��,CRC����");
			return CmdResult_CrcError;
		}

		// pass
		break;
	}

	startIdx = index;

	// ���� ֡ͷ���豸����
	index += 3;

	// ���ID
	if(memcmp(&buf[index], DstAddr, AddrLen) != 0){

		// �㲥����ʱ����Ӧ��ı��ַ��Ϊ�µ�Ŀ�ĵ�ַ
		GetStringHexFromBytes(StrDstAddr, buf, index, AddrLen, 0, false);

		dispIdx = 0;
		#if (AddrLen <= 6)
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "���: \n   %s\n", StrDstAddr);
		#endif
	}
	index += AddrLen;

	// ������2��·�ɼ���|��ǰλ��
	switch ((uint8)(buf[index] >> 4))
	{
	case 0: relayCnt = 0; break;
	case 4: relayCnt = 1; break;
	case 8: relayCnt = 2; break;
	default: break;
	}
	index += 1;

	// ������1��������
	cmd = (uint8)(buf[index] & 0x1F);
	index += 1;

	// ���� �����򳤶� --> ·����Ϣ
	index += (1 + relayCnt * AddrLen);

	// ��������
	subCmd = buf[index]; // ����û��
	
	// ���������
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	������� 0x01 ~ 0x25 , 70 ~ 74		--------------------
	//-------------------------------------------------------------------------------------------------
	//----------------------------------------		��ȡ�û�����		-------------
	case WaterCmd_ReadRealTimeData:	// ��ȡ�û�����
		if(rxlen < index + 9 || cmd != 0x01){
			break;
		}
		ret = CmdResult_Ok;
		// �����
		u32Tmp = GetUint32(&buf[index], 3, false);
		u8Tmp = buf[index + 3];
		dispIdx += sprintf(&dispBuf[dispIdx], "�����: %d.%02d\n", u32Tmp, u8Tmp);
		index += 4;
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.meterValue, "%d.%02d", u32Tmp, u8Tmp);
			}
		#endif
		// ��ھ�������ϵ��
		ptr = (buf[index] & 0x80) > 0 ? "��" : "С";
		dispIdx += sprintf(&dispBuf[dispIdx], "��˿ھ�: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "����ϵ��: %d\n", (buf[index] & 0x7F));
		index += 1;
		//��ص�ѹ
		f64Tmp = (double)buf[index] / 30.117534;
		ptr = _DoubleToStr(TmpBuf, f64Tmp, 2);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ص�ѹ: %s v\n", ptr);
		index += 1;
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				strcpy(MeterInfo.batteryVoltage, ptr);
			}
		#endif
		//����״̬
		ptr = Water8009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s\n", ptr);
		index += 1;
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
			#if defined Project_6009_RF || defined Project_8009_RF
				u8Tmp = sprintf(&MeterInfo.meterStatusStr[0], "����%s", ptr);
			#elif defined Project_6009_RF_HL || defined Project_8009_RF_HL
				u8Tmp = 0;
			#endif
			// ״̬ת�� 2/1/0 --> 0/1/2 ����/��/δ֪��
			sprintf(MeterInfo.valveStatus, "%d", (buf[index - 1] & 0x20) > 0 ? 0 : ((buf[index - 1] & 0x40) > 0 ? 1 : 2) );		
			}
		#endif

		//�澯״̬��
		#ifdef Use_DBF
			u32Tmp = dispIdx + 10;
		#endif
		u16Tmp = GetUint16(&buf[index], 2, true);
		dispIdx += sprintf(&dispBuf[dispIdx], "�澯״̬: ");
		dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		#ifdef Use_DBF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				u16Tmp = (uint16)(dispIdx - u32Tmp - 2);	// ȥ������� �� \n��
				if(u16Tmp + u8Tmp > Size_MeterStatusStr){
					u16Tmp = Size_MeterStatusStr - u8Tmp;
				}
			#if defined Project_6009_RF || defined Project_8009_RF
				if(u16Tmp > 0)
				{
					MeterInfo.meterStatusStr[u8Tmp++] = ',';
					strncpy(&MeterInfo.meterStatusStr[u8Tmp], &dispBuf[u32Tmp], u16Tmp);
					MeterInfo.meterStatusStr[u8Tmp + u16Tmp] = 0x00;
				}
				sprintf(MeterInfo.meterStatusHex, "%02X%02X%02X", buf[index - 3], buf[index - 2], buf[index - 1]);
			#elif defined Project_6009_RF_HL || defined Project_8009_RF_HL
				if(u16Tmp > 0)
				{
					strncpy(&MeterInfo.meterStatusStr[0], &dispBuf[u32Tmp], u16Tmp);
					MeterInfo.meterStatusStr[0 + u16Tmp] = 0x00;
				}
				sprintf(MeterInfo.meterStatusHex, "%02X%02X", buf[index - 3], buf[index - 2]);
			#endif
			}
		#endif
		break;

	case WaterCmd_SetBaseValPulseRatio:	// ����������ϵ��
		if(rxlen < index + 5 || cmd != 0x04){
			break;
		}
		ret = CmdResult_Ok;
		// �����
		u32Tmp = GetUint32(&buf[index], 3, false);
		u8Tmp = buf[index + 3];
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "�����: %d.%02d\n", u32Tmp, u8Tmp);
		// ��ھ�������ϵ��
		ptr = (buf[index] & 0x80) > 0 ? "��" : "С";
		dispIdx += sprintf(&dispBuf[dispIdx], "��˿ھ�: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "����ϵ��: %d\n", (buf[index] & 0x7F));
		index += 1;
		break;

	case WaterCmd_OpenValve:		// ����
		if(rxlen < index || cmd != 0x05){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
		break;

	case WaterCmd_CloseValve:		// �ط�
		if(rxlen < index || cmd != 0x06){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
		break;

	case WaterCmd_ClearException:	// ���쳣���� 03
		if(rxlen < index || cmd != 0x03){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
		break;

	case WaterCmd_ReadMeterCfgInfo:	// ��ȡ���������Ϣ

		if(rxlen < index + 31 || cmd != 0x02){
			break;
		}
		ret = CmdResult_Ok;
		// ���ط�ʱ�� 200ms * n
		f64Tmp = (buf[index] & 0x7F) * 0.2;
		ptr = _DoubleToStr(TmpBuf, f64Tmp, 1);
		dispIdx += sprintf(&dispBuf[dispIdx], "���ط�ʱ��: %s s\n", ptr);
		index += 1;
		// ��������ֵ 2mA * n
		dispIdx += sprintf(&dispBuf[dispIdx], "��������ֵ: %d mA\n", (buf[index] & 0x7F) * 2);
		index += 1;
		// �������͡���ѹ���͡��ŵ�
		ptr = Water8009_GetStrSensorType(buf[index] >> 5);
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %s\n", ptr);
		ptr = ((buf[index] & 0x10) > 0 ? "3.6 v" : "6.0 v");
		dispIdx += sprintf(&dispBuf[dispIdx], "��ѹ����: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "ͨ���ŵ�: %d\n", (buf[index] & 0x07));
		index += 1;
		// ��ھ�������ϵ��
		ptr = (buf[index] & 0x80) > 0 ? "��" : "С";
		dispIdx += sprintf(&dispBuf[dispIdx], "��˿ھ�: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "����ϵ��: %d\n", (buf[index] & 0x7F));
		index += 1;
		// �Ÿ��ſ���ʱ��
		dispIdx += sprintf(&dispBuf[dispIdx], "���ſ���ʱ��: %d s\n", (buf[index] & 0x7F));
		index += 1;
		// ������С����
		dispIdx += sprintf(&dispBuf[dispIdx], "������С����: %d ms\n", buf[index]);
		index += 1;
		// ���� 0x0A ��֪��ɶ���⣬Э������©�Ķ���
		index += 1;
		// �汾�� 24 byte
		memcpy(&VerInfo[0], &buf[index], VerLen);
		VerInfo[VerLen] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "�汾: %s\n", &VerInfo[0]);
		index += VerLen;
		break;

	case WaterCmd_SetMeterNumber:	// ���ñ��
		if(rxlen < index + 1 || cmd != 0x1c){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// ����״̬
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, AddrLen, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "�±��: %s\n", &TmpBuf[0]);
		}
		else{
			ret = CmdResult_Failed;
		}
		index += AddrLen;
		break;

	//----------------------------------		��������		----------------------------

	case WaterCmd_ReadReverseMeasureData:	// ��ȡ��ת����
		if(rxlen < index + 4 || cmd != 0x0A){
			break;
		}
		ret = CmdResult_Ok;
		if(rxlen >= index + 4 + 9){
			// ��ת����
			u32Tmp = GetUint32(&buf[index], 3, false);
			u8Tmp = buf[index + 3];
			dispIdx += sprintf(&dispBuf[dispIdx], "�����: %d.%02d\n", u32Tmp, u8Tmp);
			index += 4;
			// ��ھ�������ϵ��
			ptr = (buf[index] & 0x80) > 0 ? "��" : "С";
			dispIdx += sprintf(&dispBuf[dispIdx], "��˿ھ�: %s\n", ptr);
			dispIdx += sprintf(&dispBuf[dispIdx], "����ϵ��: %d\n", (buf[index] & 0x7F));
			index += 1;
			//��ص�ѹ
			f64Tmp = (double)buf[index] / 30.117534;
			ptr = _DoubleToStr(TmpBuf, f64Tmp, 2);
			dispIdx += sprintf(&dispBuf[dispIdx], "��ص�ѹ: %s v\n", ptr);
			index += 1;
			//����״̬
			ptr = Water8009_GetStrValveStatus(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s\n", ptr);
			index += 1;
			//�澯״̬��
			u16Tmp = GetUint16(&buf[index], 2, true);
			dispIdx += sprintf(&dispBuf[dispIdx], "�澯״̬: ");
			dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
			index += 2;
		}
		else{
			// ��ת����
			u32Tmp = GetUint32(&buf[index], 3, false);
			u8Tmp = buf[index + 3];
			dispIdx += sprintf(&dispBuf[dispIdx], "�����: %d.%02d\n", u32Tmp, u8Tmp);
			index += 4;
		}
		break;

	case WaterCmd_ClearReverseMeasureData:	// �����ת����
		if(rxlen < index || cmd != 0x12){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
		break;

	case WaterCmd_ReadFuncEnableStateOld:	// ��ȡ����ʹ��״̬ old
		if(rxlen < index + 2 || cmd != 0x17){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "����ʹ��״̬����\n");
		u16Tmp = GetUint16(&buf[index], 2, false);
		dispIdx += Water8009_GetStrMeterFuncEnableStateOld(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	case WaterCmd_ReadFuncEnableStateNew:	// ��ȡ����ʹ��״̬ new
		if(rxlen < index + 2 || cmd != 0x17){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "����ʹ��״̬����\n");
		u16Tmp = GetUint16(&buf[index], 2, false);
		dispIdx += Water8009_GetStrMeterFuncEnableState(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	
	case WaterCmd_SetFuncEnableState:	// ���ù���ʹ��״̬ 
		if(rxlen < index){
			break;
		}
		if(cmd >= 0x0C && cmd <= 0x0F
			|| cmd >= 0x13 && cmd <= 0x16 
			|| cmd == 0x19){
			// pass
		}
		else{
			break;
		}
		if(rxlen < index + 6){
			ret = CmdResult_Ok;
			dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
		}
		else{
			ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
			ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
			dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
			index += 1;
			if(buf[index - 1] == 0xAA){
				// ģ��ʹ��״̬
				ptr = ((buf[index] & 0x01) > 0 ? "����" : "�ر�");
				dispIdx += sprintf(&dispBuf[dispIdx], "��ʱ����: %s\n", ptr);
				ptr = ((buf[index] & 0x04) > 0 ? "����" : "�ر�");
				dispIdx += sprintf(&dispBuf[dispIdx], "��ʱ�ϱ�: %s\n", ptr);
				ptr = ((buf[index] & 0x08) > 0 ? "����" : "�ر�");
				dispIdx += sprintf(&dispBuf[dispIdx], "��ʱ�ϱ�: %s\n", ptr);
				index += 1;
			}
		}
		break;


	case WaterCmd_ReadTimeAndRfState:	// ��ѯʱ�Ӽ�RF״̬ ����/�ر�RF��ʱ�ι��� 09-00/01
		if(rxlen < index + 9 || cmd != 0x09){
			break;
		}
		// �������
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// ģ��ʹ��״̬
			ptr = ((buf[index] & 0x01) > 0 ? "��" : "��");
			dispIdx += sprintf(&dispBuf[dispIdx], "RF��ʱ�ι���: %s\n", ptr);
			ptr = ((buf[index] & 0x02) > 0 ? "��" : "��");
			dispIdx += sprintf(&dispBuf[dispIdx], "����ͬ��ʱ��: %s\n", ptr);
			index += 1;
			// ��ǰʱ��
			dispIdx += GetTimeStr(&dispBuf[dispIdx], "��˵�ǰʱ��: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n"
				, &buf[index], 7);
			index += 7;
		}
		break;


	case WaterCmd_ReadRfWorkTime:		// ��ѯRF����ʱ�� 09-04
	case WaterCmd_SetRfWorkTime:		// ����RF����ʱ�� 09-84
		if(rxlen < index + 1 || cmd != 0x09){
			break;
		}
		// �������
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// ģ��ʹ��״̬
			ptr = ((buf[index] & 0x01) > 0 ? "��" : "��");
			dispIdx += sprintf(&dispBuf[dispIdx], "RF��ʱ�ι���: %s\n", ptr);
			ptr = ((buf[index] & 0x02) > 0 ? "��" : "��");
			dispIdx += sprintf(&dispBuf[dispIdx], "����ͬ��ʱ��: %s\n", ptr);
			index += 1;
			// ����ʱ��
			if(buf[index] != buf[index + 2]){
				dispIdx += sprintf(&dispBuf[dispIdx], "ÿ�¹���ʱ�Σ�\n");
				dispIdx += sprintf(&dispBuf[dispIdx], "    %x�� %02X:00 \n - %x�� %02X:00\n", 
					buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			}
			else{
				dispIdx += sprintf(&dispBuf[dispIdx], "ÿ�칤��ʱ�Σ�\n");
				dispIdx += sprintf(&dispBuf[dispIdx], "    %02X:00 - %02X:00\n", buf[index + 1], buf[index + 3]);
			}
			index += 4;
		}
		break;


	//--------------------------------		DMA ��Ŀ	-----------------------------------

	case WaterCmd_UploadCenterFrequency:		// �����ϴ�����Ƶ�� 09-09/89
	case WaterCmd_EnableReportAmeterData:		// ʹ��ģ���ϴ������ 09-0B/8B
		if(rxlen < index + 1 || cmd != 0x09){
			break;
		}
		// �������
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// ���� ��������
			index += 1;
		}
		break;

	case WaterCmd_SetReportTimeInterval:		// �����ϴ�ʱ���� 09-8A
	case WaterCmd_ReadReportTimeInterval:		// ��ȡ�ϴ�ʱ���� 09-0A
		if(rxlen < index + 1 || cmd != 0x09){
			break;
		}
		// �������
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// ���� ��������
			index += 1;
			// ʱ����
			dispIdx += sprintf(&dispBuf[dispIdx], "ʱ����: %d Сʱ\n", buf[index]);
			index += 1;
		}
		break;

	case WaterCmd_ReadFrozenData:	// ��ȡ�������� 09-05/06
		if(rxlen < index + 1 || cmd != 0x09){
			break;
		}
		// �������
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;

		if(rxlen >= index + 53){
			// ������������: 05-��ת
			dispIdx += sprintf(&dispBuf[dispIdx], "��������: %s\n", (buf[index] == 0x05 ? "��ת" : "��ת"));
			index += 1;
			// ����������ʼ���
			u8Tmp = buf[index] * 10;
			dispIdx += sprintf(&dispBuf[dispIdx], "���ݷ�Χ: �� %d-%d ��\n", u8Tmp, u8Tmp + 9);
			index += 1;
			// ������������ݣ�7*N �ֽ� ��6 byte ���� + 1 byte date.day��
			dispIdx += sprintf(&dispBuf[dispIdx], "��ȡ��10����������: \n");
			for(i = 0; i < 10; i++){
				u32Tmp = GetUint32(&buf[index], 3, false);
				u8Tmp = buf[index + 3];
				dispIdx += sprintf(&dispBuf[dispIdx], "%02d : %d.%02d\n", i, u32Tmp, u8Tmp);
				index += 4;
			}
			// ����������ʼʱ��
			dispIdx += GetTimeStr(&dispBuf[dispIdx], "��ʼʱ��: \n %02X%02X%02X%02X %02X:%02X:00\n", &buf[index], 6);
			index += 6;
			// ��������ʱ����
			dispIdx += sprintf(&dispBuf[dispIdx], "ʱ����: %d ����\n", buf[index]);
			index += 1;
			//��ص�ѹ
			f64Tmp = (double)buf[index] / 30.117534;
			ptr = _DoubleToStr(TmpBuf, f64Tmp, 2);
			dispIdx += sprintf(&dispBuf[dispIdx], "��ص�ѹ: %s v\n", ptr);
			index += 1;
			//����״̬
			ptr = Water8009_GetStrValveStatus(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s", ptr);
			index += 1;
			//�澯״̬��
			u16Tmp = GetUint16(&buf[index], 2, true);
			dispIdx += sprintf(&dispBuf[dispIdx], "�澯״̬: ");
			dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
			index += 2;
		}
		break;

	case WaterCmd_ReadMeterTime:	// ��ȡʱ�� 09-00
		if(rxlen < index + 9 || cmd != 0x09){
			break;
		}
		// �������
		ptr = (buf[index] == 0xAA ? "�����ɹ�" : "����ʧ��");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// ģ��ʹ��״̬ ����
			index += 1;
			// ��ǰʱ��
			dispIdx += GetTimeStr(&dispBuf[dispIdx], "��˵�ǰʱ��: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n"
				, &buf[index], 7);
			index += 7;
		}
		break;

	case WaterCmd_SetMeterTime:		// ����ʱ�� ������ָ��01 + ʱ��
		if(rxlen < index + 9 || cmd != 0x01){
			break;
		}
		ret = CmdResult_Ok;
		// ����ɹ���Уʱ�ɹ�
		dispIdx += sprintf(&dispBuf[dispIdx], "���: �����ɹ�\n");
		index += 9;
		break;


	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	���������� 0x40 ~ 0x65 , F1 ~ F3	--------------------
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		��������		-------------------------------------
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

	case CenterCmd_SetCenterNo:		// ���òɼ�����
		if(rxlen < index + 6 || cmd != 0x41){
			break;
		}
		ret = CmdResult_Ok;
		// ��������
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: \n    %02X%02X%02X%02X%02X%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
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
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
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
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ClearMeterReadData:		// �����������
		if(rxlen < index + 2 || cmd != 0x48){
			break;
		}
		ret = CmdResult_Ok;
		// �����ϴ�����
		ptr = (buf[index] == 0 ? "��յ�����·��" : "�����������");
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %s\n", ptr);
		index += 1;
		// ����״̬
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;


	//--------------------------------------		����������		-------------------------------------
	case CenterCmd_ReadDocCount:		// ����������
		if(rxlen < index + 3 || cmd != 0x50){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water8009_GetStrDeviceType(buf[index]);
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
			ptr = Water8009_GetStrDeviceType(buf[index]);
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
			ptr = Water8009_GetStrErrorMsg(buf[index]);
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
			ptr = Water8009_GetStrErrorMsg(buf[index]);
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
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;


	//--------------------------------------		���Ʋ�����		-------------------------------------
	case CenterCmd_ReadFixedValData:			// ����������
		break;

	case CenterCmd_ReadRealTimeData:			// ��ʵʱ����
		break;

	case CenterCmd_ReadFrozenData:			// ����������
		break;

	case CenterCmd_OpenValve:			// ����
	case CenterCmd_CloseValve:			// �ط�
	case CenterCmd_ClearException:		// ���쳣
		break;

	//--------------------------------------		·�ɲ�����		-------------------------------------
	case CenterCmd_ReadRouteList:		// ��ȡ·���б�
		if(rxlen < index + 10 || cmd != 0x55){
			break;
		}
		ret = CmdResult_Ok;
		// ����״̬
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		// ���
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", &TmpBuf[0]);
		index += 6;
		// �豸����
		ptr = Water8009_GetStrDeviceType(buf[index]);
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

	case CenterCmd_SetRouteList:			// ����·���б�
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
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadMeterRoute:			// ��ȡ���·��
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
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_SetMeterRoute:			// ���ñ��·��
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
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_QueryRouteList:			// �鿴·���б�
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
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", ptr);
		index += 1;
		break;

	default:
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "������[%02X]��δ����\n", cmd);
		break;
	}


	if(index == startIdx + length - 4)
	{
		//����/���� �ź�ǿ��
		TmpBuf[0] = buf[index];
		TmpBuf[1] = buf[index + 1];

		if(TmpBuf[0]<31)
        	TmpBuf[0] = 31;
    	else if(TmpBuf[0] > 80)
        	TmpBuf[0] = 80;

		if(TmpBuf[1]<31)
        	TmpBuf[1] = 31;
    	else if(TmpBuf[1] > 80)
        	TmpBuf[1] = 80;

		TmpBuf[0] = (TmpBuf[0] - 30) * 2;
		TmpBuf[1] = (TmpBuf[1] - 30) * 2;
		
		#ifdef Use_DBF
		#if defined Project_6009_RF || defined Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.signalValue, "%d", TmpBuf[1]);	// ��������
			}
		#endif
		#endif
		dispIdx += sprintf(&dispBuf[dispIdx], "                    \n");
		dispIdx += sprintf(&dispBuf[dispIdx], "����: %d  ����: %d\n", TmpBuf[0],  TmpBuf[1]);
		index += 2;
	}
	else{
		#if LOG_ON
		dispIdx += sprintf(&dispBuf[dispIdx], "�����쳣��\n");
		#endif
	}

	dispBuf[dispIdx] = '\0';
	
	
	return ret;
}

//--------------------------------------	8009ˮ������ ���͡����ա������ʾ	----------------------------
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
uint8 Protol8009TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
#ifdef Project_6009_IR
	ackLen += 15 + addrs->itemCnt * AddrLen;
	timeout = 2000;
	tryCnt = 3;
#elif defined(Project_6009_RF)
	ackLen += 15 + addrs->itemCnt * AddrLen;
	timeout = 10000 + (addrs->itemCnt - 2) * 6000 * 2;
	tryCnt = 3;
#else // Project_8009_RF
	ackLen += 10 + addrs->itemCnt * AddrLen;
	timeout = 2000 + (addrs->itemCnt - 1) * 2000;
	tryCnt = 3;
#endif

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
