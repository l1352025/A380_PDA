#ifndef ProtoHandle_DH_IR_H
#define ProtoHandle_DH_IR_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#include "HJLIB.h"


#define FrameHeader1_DH		0xFF	// ֡ͷ 0xFF 0x02
#define FrameHeader2_DH		0x02	
#define FrameTail_DH		0x03	// ֡β 0x03	
#define FrameFixedLen_DH	7		// �̶����ֳ��� header-2 + len-1 + data-n(id-1) + crc16-2 + tail-1



extern uint8 Pack_DonghaiIR_RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 Explain_DonghaiIR_ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

#ifndef Frame_Pack_Parse
#define Frame_Pack_Parse
FuncCmdFramePack FramePack = Pack_DonghaiIR_RequestFrame;
FuncCmdFrameExplain FrameExplain = Explain_DonghaiIR_ResponseFrame;
#endif

//----------------------------------------  �������  ------------------------

// ������:  ����ʱ 0x01~0x06 �� Ӧ��ʱ 0x81~0x86
typedef enum{
	Func_GetInternalParam	= 0x04,		// Ӧ��ʱΪ 0x84
	Func_SetInternalParam	= 0x05		// Ӧ��ʱΪ 0x85
}FuncCode_DH;

// Tag��
typedef enum{
	Tag_PeriodReportFreq	= 0x1B
}TagCode_DH;


typedef enum{
	/*
	���ù��ܣ�	
	1	���������ϱ�Ƶ��
	*/
	UserCmd_SetPeriodReportFreq		= 0x11
}UserCmdCode_DH;


/*
* ��  ����CRC16 ����
* ��  ����
*		pucBuf	- ���ݻ����ַ
* 		uwLength - ����У������ݳ���
* ����ֵ��uint16 - CRC16У��ֵ
*/
uint16 GetCRC16_DonghaiIR(const uint8 *pucBuf, uint16 uwLength)
{
	#define CRC_POLY3 0x8408
    uint16 uiCRCValue=0;
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
                uiCRCValue ^= CRC_POLY3; 
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
* ��  �������� �����
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * DonghaiIR_GetStrResultCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0: str = "�ɹ�"; break;
	case 1: str = "ʧ��"; break;
	default:
	    str = "δ֪";
		break;
	}

	return str;
}

/*
* ��  �������� ���̴���
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * DonghaiIR_GetStrManufacturerCode(uint8 code)
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
* ��  �������� �豸����
* ��  ����code	- ����ֵ
* ����ֵ��char *	- ��������ַ���
*/
char * DonghaiIR_GetStrDeviceType(uint8 code)
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
uint8 Pack_DonghaiIR_RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt)
{
	static uint16 Fsn = 0;
	static uint16 index = 0;
	uint16 len, crc16;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = FrameHeader1_DH;	// ֡ͷ
	buf[index++] = FrameHeader2_DH;	
	len = 3 + args->lastItemLen;	// ����: (���� + ������ + crc16)
	buf[index++] = (uint8)len;	

	// ������-���ݲ���
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	crc16 = GetCRC16_DonghaiIR(&buf[2], index - 2);
	buf[index++] = (uint8)(crc16 & 0xFF);	// crc16 У��: (���� + ������)
	buf[index++] = (uint8)(crc16 >> 8);
	buf[index++] = FrameTail_DH;				// ������

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
uint8 Explain_DonghaiIR_ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
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
		if(rxlen < index + FrameFixedLen_DH){		
			sprintf(&dispBuf[dispIdx], "���: ��ʱ,��Ӧ��");
			return CmdResult_Timeout;
		}

		// start check
		if(buf[index] == FrameHeader1_DH && buf[index + 1] == FrameHeader2_DH){
			// pass
		}else{
			index++;
			continue;
		}
		
		// length check 
		length = (uint16)(buf[index + 2] + 3);
		if(length > rxlen){
			index += length;
			continue;
		}	

		// crc16 check
		crc16 = GetCRC16_DonghaiIR(&buf[index + 2], length - 5);
		if(crc16 !=  (buf[index + length - 3] + buf[index + length - 2] * 256)){
			sprintf(&dispBuf[dispIdx], "���: ��Ӧ��,CRC����");
			return CmdResult_CrcError;
		}

		// pass
		break;
	}

	startIdx = index;

	// ���� ֡ͷ������
	index += 3;

	

	// ���������
	payloadIdx = index;

	// ������
	funcId = buf[index] & 0x7F;
	index += 1;

	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		������� 		--------------------------------------
	//-------------------------------------------------------------------------------------------------
	
	case UserCmd_SetPeriodReportFreq:	// ���������ϱ�Ƶ��
		if(rxlen < index + 13 || funcId != Func_SetInternalParam){
			break;
		}
		ret = CmdResult_Ok;
		// ������
		index += 6;
		// �豸�汾
		index += 2;
		// �豸����
		index += 2;
		// ��������
		u8Tmp = buf[index];
		index += 1;
		// ������š�����״̬
		if(u8Tmp == 1 && buf[index] == Tag_PeriodReportFreq && buf[index + 1] == 0x00){
			ret = CmdResult_Ok;
		}
		else{
			ret = CmdResult_Failed;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "���: %s\n", DonghaiIR_GetStrResultCode(buf[index + 1]));
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
uint8 DonghaiIR_TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
	ackLen += FrameFixedLen_DH;		// �ڴ�Ӧ�𳤶ȣ�ʵ��û�õ�
	timeout = 2000;
	tryCnt = 3;

	FramePack = Pack_DonghaiIR_RequestFrame;
	FrameExplain = Explain_DonghaiIR_ResponseFrame;

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
