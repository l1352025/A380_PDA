#ifndef ProtoHandle_NBiot_BJ_H
#define ProtoHandle_NBiot_BJ_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#include "HJLIB.h"


#define FrameHeader1	0xA5		// ֡ͷ 0xA5 0x68
#define FrameHeader2	0x68		 
#define FrameTail		0x16		// ֡β 0x16	
#define FrameFixedLen_Downlink	20	// ����֡-�̶����ֳ���
#define FrameFixedLen_Uplink	37	// ����֡-�̶����ֳ���
#define NBiotBj_Ver		(uint8)2	// NBiot ����ˮ��Э�� 2.0	


extern uint8 PackWaterNBiotBjRequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWaterNBiotBjResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

#ifndef Frame_Pack_Parse
#define Frame_Pack_Parse
FuncCmdFramePack FramePack = PackWaterNBiotBjRequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWaterNBiotBjResponseFrame;
#endif

//----------------------------------------  �������  ------------------------

// ������:  ����ʱ 0x01~0x06 �� Ӧ��ʱ 0x81~0x86
typedef enum{
	Func_ParamSet	= 0x01,
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

// Tag��
typedef enum{
	Tag_FuncCode	= 0x01,
	Tag_ReplyCode,
	Tag_BasicData,
	Tag_TermParam,
	Tag_AlarmData,
	Tag_DataTransmit,
	Tag_RealTimeData,
	Tag_PeriodData,
	Tag_DenseData,
	Tag_HistoryDataTime,

	Tag_Unknown
}TagCode;


typedef enum{
	/*
	���ù��ܣ�	
	1	���Զ�������
	2	��������
	3	���ñ��ַ
	4	���ó�ֵ
	5	��ȡΪ��ģ������
	*/
	UserCmd_ReadCustomData		= 0x11,	
	UserCmd_SetPassword,
	UserCmd_SetMeterAddr,	
	UserCmd_SetInitValue,	
	UserCmd_ReadWeiFengModuleData,	

	UserCmd_Unknown
}UserCmdCode;


/*
* ��  ����NB-iot ����ˮ��Э�� CRC16 ����
* ��  ����
*		pucBuf	- ���ݻ����ַ
* 		uwLength - ����У������ݳ���
* ����ֵ��uint16 - CRC16У��ֵ
*/
uint16 GetCRC16_NBiotBj(const uint8 *pucBuf, uint16 uwLength)
{
	#define CRC_POLY 0xa001
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
                uiCRCValue ^= CRC_POLY; 
            }
            else 
            {
                uiCRCValue >>= 1;
            }
        }
    }

    return uiCRCValue;
}


//---------------------------------------		��������	-------------------------------------

/*
* ��  ����NB-iot����ˮ�� ������
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * NBiotBj_GetStrFunctionCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case Func_ParamSet: str = "��������"; break;
	case Func_DataReport: str = "�����ϱ�"; break;
	case Func_InfoQuery: str = "��Ϣ��ѯ"; break;
	case Func_DataTransmit: str = "����͸��"; break;
	case Func_HistoryDataRpt: str = "��ʷ���ݲ���"; break;
	case Func_ReadHistoryData: str = "��ȡ��ʷ����"; break;
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


//-----------------------------------	Э�� ��� / ���	-----------------------------

/*
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
	buf[index++] = 0;
	
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
* ��  ��������ˮ��������Ӧ֡
* ��  ����buf		- ���ջ�����ʼ��ַ
*		  rxlen		- ���յĳ���
*		  dstAddr	- Ŀ�ĵ�ַ���жϽ��յ�Ŀ�ĵ�ַ�Ƿ����Լ�
*		  cmdId		- ������
*		  ackLen	- Ӧ�𳤶�
*		  dispBuf 	- ��������ʾ����
* ����ֵ��uint8 ���������CmdResult 0 - �ɹ� , 1 - ʧ�� �� 2 - CRC���� 3 - ��ʱ��Ӧ��, 
*/
uint8 ExplainWaterNBiotBjResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
{
	CmdResult ret = CmdResult_Failed;
	uint8 funcId, tagId, dataId, u8Tmp;
	uint16 index = 0, dispIdx, length, dataLen, crc16, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	int32 i32Tmp;
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
	funcId = buf[index] & 0x7F;
	index += 1;
	// ���ܱ�ʶ
	index += 1; // ����
	// �����򳤶�
	index += 2; // ����

	// ���������
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		������� 		--------------------------------------
	//-------------------------------------------------------------------------------------------------
	
	case UserCmd_ReadCustomData:	// ���Զ�������
		if(rxlen < index + 17 || funcId != Func_DataTransmit){
			break;
		}

		tagId = buf[index++];
		dataLen = (uint16)(buf[index] + buf[index + 1] * 256);
		index += 2;
		dataId = buf[index++];
		if(tagId != Tag_DataTransmit || dataId == 0){
			break;
		}

		ret = CmdResult_Ok;
		// ��������
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %d L\n", u32Tmp);
		// ��������
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %d L\n", u32Tmp);
		// ��ص�ѹ
		u16Tmp = (buf[index + 1] << 8) + buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "��ص�ѹ: %d.%02d\n", (u16Tmp / 100), (u16Tmp % 100));
		index += 1;
		// ���� 3byte
		index += 3;
		break;

	case UserCmd_SetPassword:		// ��������
	case UserCmd_SetMeterAddr:		// ���ñ��ַ
	case UserCmd_SetInitValue:		// ���ó�ֵ
		if(rxlen < index + 81 || funcId != Func_DataTransmit){
			break;
		}
		tagId = buf[index++];
		dataLen = (uint16)(buf[index] + buf[index + 1] * 256);
		index += 2;
		dataId = buf[index++];
		if(tagId != Tag_DataTransmit || dataId != 1){
			break;
		}
		ret = CmdResult_Ok;
		// IMSI
		Memcpy_AddNull(&TmpBuf[0], &buf[index], 15);
		dispIdx += sprintf(&dispBuf[dispIdx], "IMSI: %s\n", &TmpBuf[0]);
		index += 15;
		// ��ǰ����
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 16, NULL, false);
		dispIdx += sprintf(&dispBuf[dispIdx], "��ǰ����: %s\n", &TmpBuf[0]);
		index += 16;
		// ��������
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %d L\n", u32Tmp);
		index += 4;
		// ��������
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %d L\n", u32Tmp);
		index += 4;
		// �Ƿ����
		dispIdx += sprintf(&dispBuf[dispIdx], "�Ƿ����: %s \n", buf[index] == 1 ? "��" : "��");
		index += 1;
		// ���Ᵽ��ʱ��
		dispIdx += sprintf(&dispBuf[dispIdx], "���Ᵽ��ʱ��: %d s\n", buf[index]);
		index += 1;
		// ���
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 7, NULL, true);
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", &TmpBuf[0]);
		index += 7;
		// CCID
		Memcpy_AddNull(&TmpBuf[0], &buf[index], 20);
		dispIdx += sprintf(&dispBuf[dispIdx], "CCID: %s\n", &TmpBuf[0]);
		index += 20;
		// CellID
		Memcpy_AddNull(&TmpBuf[0], &buf[index], 8);
		dispIdx += sprintf(&dispBuf[dispIdx], "CellID: %s\n", &TmpBuf[0]);
		index += 8;
		// ���� 1byte
		index += 1;
		break;

	
	case UserCmd_ReadWeiFengModuleData:	// ��ȡΪ��ģ������
		if(rxlen < index + 23 || funcId != Func_DataTransmit){
			break;
		}
		tagId = buf[index++];
		dataLen = (uint16)(buf[index] + buf[index + 1] * 256);
		index += 2;
		dataId = buf[index++];
		if(tagId != Tag_DataTransmit || dataId != 2){
			break;
		}
		ret = CmdResult_Ok;
		// Ϊ��ģ��ͨ��Э�飺68 Len CtrlCode Data CS 16
		// ����ͷ�� 2byte
		index += 2;	
		u8Tmp = GetSum8(&buf[index], 15);	// �ۼӺ�У��
		if(u8Tmp != buf[index + 15]){
			ret = CmdResult_Failed;
			dispIdx += sprintf(&dispBuf[dispIdx], "Ϊ��ģ������У��ʹ���\n");
			break;
		}
		index += 1;
		// ����汾
		dispIdx += sprintf(&dispBuf[dispIdx], "����汾: %d.%02d \n", buf[index], buf[index + 1]);
		index += 2;
		// �ź�ǿ�� �� ǿ�Ȳ�ֵ
		u16Tmp = buf[index] | ((buf[index + 1] & 0x80) << 8);
		u8Tmp = (buf[index + 1] & 0x7F);
		dispIdx += sprintf(&dispBuf[dispIdx], "�ź�ǿ��: %d \n", u16Tmp);
		dispIdx += sprintf(&dispBuf[dispIdx], "ǿ�Ȳ�ֵ: %d \n", u8Tmp);
		index += 2;
		// ������
		i32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "�� �� ��: %d \n", i32Tmp);
		index += 4;
		// ��������
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "��������: %d \n", u32Tmp);
		index += 4;
		// ״̬��
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "״̬��: [ %02X ]\n", u8Tmp);
		dispIdx += sprintf(&dispBuf[dispIdx], "    У׼ʧ��: %s\n", (u8Tmp & 0x01) > 0 ? "��" : " ��");
		dispIdx += sprintf(&dispBuf[dispIdx], "    ���ٲ���: %s\n", (u8Tmp & 0x02) > 0 ? "��" : " ��");
		dispIdx += sprintf(&dispBuf[dispIdx], "    ģ�鱻��: %s\n", (u8Tmp & 0x04) > 0 ? "��" : " ��");
		dispIdx += sprintf(&dispBuf[dispIdx], "    �������: %s\n", (u8Tmp & 0x08) > 0 ? "��" : " ��");
		dispIdx += sprintf(&dispBuf[dispIdx], "    �ź�ǿ����Ч: %s\n", (u8Tmp & 0x10) > 0 ? "��" : " ��");
		index += 1;
		// ���� 1byte
		index += 1;
		// ����β�� 2byte
		index += 2;
		break;

	default:
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "������[%02X]��δ����\n", cmdId);
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
*		ackLen	- Ӧ�𳤶� (byte) <�ڴ�Ӧ�𳤶ȣ�ʵ��û�õ�>
*		timeout	- ��ʱʱ�� (ms)  Ĭ��Ϊ 8s + �м��� x 2 x 6s
*		tryCnt	- ���Դ��� Ĭ��3��
* ���أ� uint8	- �����˳�ʱ�İ���ֵ��ȷ�ϼ���ȡ����	
*/
uint8 NBiotBj_TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
	ackLen += FrameFixedLen_Uplink;		// �ڴ�Ӧ�𳤶ȣ�ʵ��û�õ�
	timeout = 2000;
	tryCnt = 3;

	FramePack = PackWaterNBiotBjRequestFrame;
	FrameExplain = ExplainWaterNBiotBjResponseFrame;

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
