#ifndef ProtoHandle_8009_PY_H
#define ProtoHandle_8009_PY_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#if defined Project_8009_RF_PY 
#include "MeterDocDBF_8009_PY.h"
#endif

extern uint8 PackWater8009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWater8009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

#ifndef Frame_Pack_Parse
#define Frame_Pack_Parse
FuncCmdFramePack FramePack = PackWater8009RequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWater8009ResponseFrame;
#endif

//----------------------------------------  �������  ------------------------

typedef enum{
	/*
	���ù��ܣ�	
	1	��������
	2	����
	3	��ط�
	4	����쳣
	*/
	WaterCmd_ReadRealTimeData			= 0x11,	
	WaterCmd_OpenValve,	
	WaterCmd_CloseValve,
	WaterCmd_ClearException

}WaterCmdDef;


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
	uint8 i;

	// �м̵�ַ
	addrs->itemCnt = 0;
	for(i = 0; i < RELAY_MAX; i++){
		if(strRelayAddrs[i][0] >= '0' && strRelayAddrs[i][0] <= '9'){
			addrs->items[addrs->itemCnt] = &addrs->buf[i*AddrLen];
			GetBytesFromStringHex(addrs->items[addrs->itemCnt], 0, AddrLen, strRelayAddrs[i], 0, false);
			addrs->itemCnt++;
		}
	}

	// Ŀ�ĵ�ַ
	GetBytesFromStringHex(DstAddr, 0, AddrLen, strDstAddr, 0, false);
	addrs->items[addrs->itemCnt] = &addrs->buf[addrs->itemCnt*AddrLen];
	memcpy(addrs->items[addrs->itemCnt], DstAddr, AddrLen);
	addrs->itemCnt++;
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
	buf[index++] = 0xFE;		// ֡ͷ�� �̶�Ϊ FE 68
	buf[index++] = 0x68;
	buf[index++] = 0x01;		// �豸���ͣ�00 - ˮ�� 01 - ȼ���� 

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
	uint8 crc8, relayCnt, cmd, subCmd, u8Tmp;
	uint16 index = 0, dispIdx, length, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	double f64Tmp;
	char *ptr;

	dispIdx = 0;

	// ��ʾ���
	#if (AddrLen < 8)
	dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", StrDstAddr);
	#else
	dispIdx += sprintf(&dispBuf[dispIdx], "���: \n   %s\n", StrDstAddr);
	#endif

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

		/*
		if(cmd != WaterCmd_SetMeterNumber && memcmp(BroadAddr, DstAddr, AddrLen) != 0){
			// dstaddr check
			if(memcmp(&buf[index + 3], DstAddr, AddrLen) != 0){
				index += length;
				continue;
			}
		}
		*/

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
			// ��������ʱ
			if(MeterInfo.dbIdx != Invalid_dbIdx){	
				// д��DBF�ļ�
				sprintf(MeterInfo.currReadVal, "%d.%02d", u32Tmp, u8Tmp);
				// ���������ֶν���
				index += 5;
				break;
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
		//����״̬
		ptr = Water8009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "����״̬: %s\n", ptr);
		index += 1;
		//�澯״̬��
		u16Tmp = GetUint16(&buf[index], 2, true);
		dispIdx += sprintf(&dispBuf[dispIdx], "�澯״̬: ");
		dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
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
	ackLen += 10 + addrs->itemCnt * AddrLen;
	timeout = 2000 + (addrs->itemCnt - 1) * 2000;
	tryCnt = 3;

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
