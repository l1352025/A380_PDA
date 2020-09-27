#ifndef ProtoHandle_8009_PY_H
#define ProtoHandle_8009_PY_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#include "HJLIB.h"

#if defined Project_8009_RF_PY 
#include "MeterDocDBF_8009_PY.h"
#endif


#define FrameHeader1	0xA5		// ֡ͷ 0xA5 0x68
#define FrameHeader2	0x68		 
#define FrameTail		0x16		// ֡β 0x16	
#define FrameFixedLen_Downlink	20	// ����֡-�̶����ֳ���
#define FrameFixedLen_Uplink	37	// ����֡-�̶����ֳ���
#define NBiotBj_Ver		(uint8)2	// NBiot ����ˮ��Э�� 2.0	

extern void CycleInvoke_OpenLcdLight_WhenKeyPress(uint8 currKey);
extern uint8 PackWaterNBiotBjRequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWaterNBiotBjResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

FuncCmdCycleHandler TranceiverCycleHook = CycleInvoke_OpenLcdLight_WhenKeyPress;
FuncCmdFramePack FramePack = PackWaterNBiotBjRequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWaterNBiotBjResponseFrame;


//----------------------------------------  �������  ------------------------

// ������
typedef enum{
	Func_ParamSet	= 0x81,
	Func_DataReport,
	Func_InfoQuery,
	Func_DataTransmit,
	Func_HistoryDataRpt,
	Func_ReadHistoryData,

	Func_Unknown
}FuncCode;

// �����
typedef enum{
	Reply_Success	= 0,
	Reply_InfoError,
	Reply_NotSupport,
	Reply_Failed,

	Reply_Unknown
}ReplyCode;


typedef enum{
	/*
	���ù��ܣ�	
	1	���Զ�������
	2	���ñ��ַ
	3	���ó�ֵ
	4	��ȡΪ��ģ������
	*/
	WaterCmd_ReadRealTimeData			= 0x11,	
	Cmd_ReadCustomData,	
	Cmd_SetMeterAddr,	
	Cmd_SetInitValue,	
	Cmd_ReadWeiFengModuleData,	
	Cmd_

}UserCmd;


#define CRC_POLY1 0xa001
uint16 GetCRC16_NBiotBj(const uint8 *pucBuf, uint16 uwLength)
{
    uint16 uiCRCValue=0xFFFF;
    uint8  ucLoop;
    uint8* pu8Buf = (uint8 *)pucBuf;

    while(uwLength--)
    {
        uiCRCValue ^= *pu8Buf++;
        for(ucLoop=0; ucLoop<8; ucLoop++)
        {
            if(uiCRCValue & 0x0001)
            {
                uiCRCValue >>= 1;
                uiCRCValue ^= CRC_POLY1; 
            }
            else 
            {
                uiCRCValue >>= 1;
            }
        }
    }

    return uiCRCValue;
}


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



/*
* ��  ����NB-iot����ˮ�� ������
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * NBiotBj_GetStrFunctionCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0x81: str = "��������"; break;
	case 0x82: str = "�����ϱ�"; break;
	case 0x83: str = "��Ϣ��ѯ"; break;
	case 0x84: str = "����͸��"; break;
	case 0x85: str = "��ʷ���ݲ���"; break;
	case 0x86: str = "��ȡ��ʷ����"; break;
	default:
	    str = "δ֪";
		break;
	}

	return str;
}

/*
* ��  ����NB-iot����ˮ�� �����
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * NBiotBj_GetStrResultCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0: str = "����ɹ�"; break;
	case 1: str = "��Ϣ����"; break;
	case 2: str = "��֧��"; break;
	case 3: str = "����ʧ��"; break;
	default:
	    str = "δ֪";
		break;
	}

	return str;
}


/*
* ��  ����NB-iot����ˮ�� ���̴���
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * NBiotBj_GetStrManufacturerCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0: str = "���ݾ���"; break;
	case 1: str = "����ɽ��"; break;
	case 2: str = "����˼��"; break;
	case 3: str = "���׹�˾"; break;
	case 4: str = "����ˮ��"; break;
	case 5: str = "����ɽ��"; break;
	case 6: str = "ɽ������"; break;
	case 7: str = "�����׻�"; break;
	case 8: str = "��Դ"; break;
	case 9: str = "����"; break;
	default:
	    str = "δ֪";
		break;
	}

	return str;
}

/*
* ��  ����NB-iot����ˮ�� �豸����
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * NBiotBj_GetStrDeviceType(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 2: str = "�޴�Զ��С��"; break;
	default:
	    str = "δ֪";
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
		  args	- args[0] �����룬 args[m->n] ������
		  retryCnt - �ط�������0 - ��1�η��ͣ����� - ��n���ط�
* ����ֵ��uint8 ֡�ܳ��ȣ�NBiot֡���ȿ��ܳ���255
*/
uint8 PackWaterNBiotBjRequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt)
{
	static uint16 Fsn = 0;
	static uint16 index = 0;
	uint16 len, crc16;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = FrameHeader1;		// ֡ͷ�� �̶�Ϊ A5 68
	buf[index++] = FrameHeader2;
	len = FrameFixedLen_Downlink + args->lastItemLen;	// ����: �̶�20 + �����򳤶�
	buf[index++] = (uint8)(len & 0xFF);	
	buf[index++] = (uint8)(len >> 8);	
	buf[index++] = (uint8)(Fsn & 0xFF);	// ���
	buf[index++] = (uint8)(Fsn >> 8);
	Fsn++;
	buf[index++] = NBiotBj_Ver;			// Э��汾
	buf[index++] = (_GetYear() % 100);	// ƽ̨ʱ��
	buf[index++] = _GetMonth();	
	buf[index++] = _GetDay();	
	buf[index++] = _GetHour();	
	buf[index++] = _GetMin();	
	buf[index++] = _GetSec();
	buf[index++] = *args->items[0];		// ������
	buf[index++] = 0x00;				// ���ܱ�ʶ��0-�����ܣ� 1-����	
	
	// ������-����
	buf[index++] = args->lastItemLen;
	
	// ������-���ݲ���
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	crc16 = GetCRC16_NBiotBj(&buf[0], index);
	buf[index++] = (uint8)(crc16 & 0xFF);	// crc16 У��: ֡ͷ --> У��֮ǰ
	buf[index++] = (uint8)(crc16 >> 8);
	buf[index++] = FrameTail;				// ������

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
uint8 ExplainWaterNBiotBjResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
{
	bool ret = CmdResult_Failed;
	uint8 relayCnt, cmd, subCmd, u8Tmp;
	uint16 index = 0, dispIdx, length, crc16, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	double f64Tmp;
	char *ptr;

	dispIdx = 0;


	// ����������в���
	while(1){

		// min length check
		if(rxlen < index + FrameFixedLen_Uplink){		
			sprintf(&dispBuf[dispIdx], "���: ��ʱ,��Ӧ��");
			return CmdResult_Timeout;
		}

		// start check
		if(buf[index] == FrameHeader1 && buf[index + 1] == FrameHeader2){
			// pass
		}else{
			index++;
			continue;
		}
		
		// length check 
		length = (uint16)(buf[index + 2] + buf[index + 3] * 256);
		if(length > rxlen){
			index += length;
			continue;
		}	

		// crc16 check
		crc16 = GetCRC16_NBiotBj(&buf[index], length - 3);
		if(crc16 !=  (buf[index + length - 3] + buf[index + length - 2] * 256)){
			sprintf(&dispBuf[dispIdx], "���: ��Ӧ��,CRC����");
			return CmdResult_CrcError;
		}

		// pass
		break;
	}

	startIdx = index;

	// ���� ֡ͷ�����ȡ���š�Э��汾
	index += 7;

	// ���Ҵ���
	ptr = NBiotBj_GetStrManufacturerCode(buf[index]);
	dispIdx += sprintf(&dispBuf[dispIdx], "���ұ���: %s\n", ptr);
	index += 1;
	// �豸����
	ptr = NBiotBj_GetStrDeviceType(buf[index]);
	dispIdx += sprintf(&dispBuf[dispIdx], "�豸����: %s\n", ptr);
	index += 1;
	// ģ��IMEI
	ptr = memcpy(&TmpBuf[0], &buf[index], 15);
	TmpBuf[15] = 0x00;
	dispIdx += sprintf(&dispBuf[dispIdx], "ģ��IMEI: \n    %s\n", ptr);
	index += 15;
	// �ź�����
	index += 5; // �ݲ�������RSRP��SNR�����ǵȼ�
	dispIdx += sprintf(&dispBuf[dispIdx], "�ź�����: %d\n", buf[index]);
	index += 1;

	// ������
	cmd = buf[index];
	index += 1;
	// ���ܱ�ʶ
	index += 1; // ����
	// �����򳤶�
	index += 2; // ����

	// ���������
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	������� 0x01 ~ 0x25 , 70 ~ 74		--------------------
	//-------------------------------------------------------------------------------------------------
	//----------------------------------------		��ȡ�û�����		-------------
	case Cmd_ReadCustomData:	// ��ȡ�û�����
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

	case Cmd_SetMeterAddr:		// ���ñ��ַ
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


	if(index == startIdx + length - 3)
	{
		// ������ȷ
	}
	else{
		#if LOG_ON
		dispIdx += sprintf(&dispBuf[dispIdx], "�����쳣��\n");
		#endif
	}

	dispBuf[dispIdx] = '\0';
	
	
	return ret;
}

//--------------------------------------	����͡����ա������ʾ	----------------------------
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
uint8 NBiotBj_TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
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
