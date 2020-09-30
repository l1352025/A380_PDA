#ifndef ProtoHandle_DH_IR_H
#define ProtoHandle_DH_IR_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#include "HJLIB.h"


#define FrameHeader1_DH		0xFF	// 帧头 0xFF 0x02
#define FrameHeader2_DH		0x02	
#define FrameTail_DH		0x03	// 帧尾 0x03	
#define FrameFixedLen_DH	7		// 固定部分长度 header-2 + len-1 + data-n(id-1) + crc16-2 + tail-1



extern uint8 Pack_DonghaiIR_RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 Explain_DonghaiIR_ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

#ifndef Frame_Pack_Parse
#define Frame_Pack_Parse
FuncCmdFramePack FramePack = Pack_DonghaiIR_RequestFrame;
FuncCmdFrameExplain FrameExplain = Explain_DonghaiIR_ResponseFrame;
#endif

//----------------------------------------  表端命令  ------------------------

// 功能码:  发起时 0x01~0x06 ， 应答时 0x81~0x86
typedef enum{
	Func_GetInternalParam	= 0x04,		// 应答时为 0x84
	Func_SetInternalParam	= 0x05		// 应答时为 0x85
}FuncCode_DH;

// Tag码
typedef enum{
	Tag_PeriodReportFreq	= 0x1B
}TagCode_DH;


typedef enum{
	/*
	常用功能：	
	1	设置周期上报频率
	*/
	UserCmd_SetPeriodReportFreq		= 0x11
}UserCmdCode_DH;


/*
* 描  述：CRC16 计算
* 参  数：
*		pucBuf	- 数据缓存地址
* 		uwLength - 计算校验的数据长度
* 返回值：uint16 - CRC16校验值
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


//---------------------------------------		解析函数	-------------------------------------

/*
* 描  述：解析 结果码
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * DonghaiIR_GetStrResultCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0: str = "成功"; break;
	case 1: str = "失败"; break;
	default:
	    str = "未知";
		break;
	}

	return str;
}

/*
* 描  述：解析 厂商代码
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * DonghaiIR_GetStrManufacturerCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0: str = "杭州竞达"; break;
	case 1: str = "江西山川"; break;
	case 2: str = "丹东思凯"; break;
	case 3: str = "京兆公司"; break;
	case 4: str = "宁波水表"; break;
	case 5: str = "杭州山科"; break;
	case 6: str = "山东高翔"; break;
	case 7: str = "广州兆基"; break;
	case 8: str = "京源"; break;
	case 9: str = "东海"; break;
	default:
	    str = "未知";
		break;
	}

	return str;
}

/*
* 描  述：解析 设备类型
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * DonghaiIR_GetStrDeviceType(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 2: str = "无磁远传小表"; break;
	default:
	    str = "未知";
		break;
	}

	return str;
}


//-----------------------------------	协议 打包 / 解包	-----------------------------

/*
* 描  述：打包8009水表命令请求帧
* 参  数：buf	- 数据缓存起始地址
		  addrs - 地址域：源地址、中继地址、目的地址
		  cmdId - 命令字
		  args	- args[0] 功能码， args[m->n] 数据域
		  retryCnt - 重发次数：0 - 第1次发送，其他 - 第n次重发
* 返回值：uint8 帧总长度，NBiot帧长度可能超过255
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
	buf[index++] = FrameHeader1_DH;	// 帧头
	buf[index++] = FrameHeader2_DH;	
	len = 3 + args->lastItemLen;	// 长度: (长度 + 数据域 + crc16)
	buf[index++] = (uint8)len;	

	// 数据域-数据部分
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	crc16 = GetCRC16_DonghaiIR(&buf[2], index - 2);
	buf[index++] = (uint8)(crc16 & 0xFF);	// crc16 校验: (长度 + 数据域)
	buf[index++] = (uint8)(crc16 >> 8);
	buf[index++] = FrameTail_DH;				// 结束符

	return index;
}

/*
* 描  述：解析水表命令响应帧
* 参  数：buf		- 接收缓存起始地址
*		  rxlen		- 接收的长度
*		  dstAddr	- 目的地址，判断接收的目的地址是否是自己
*		  cmdId		- 命令字
*		  ackLen	- 应答长度
*		  dispBuf 	- 解析的显示数据
* 返回值：uint8 解析结果：CmdResult 0 - 成功 , 1 - 失败 ， 2 - CRC错误， 3 - 超时无应答, 
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


	// 缓冲区多包中查找
	while(1){

		// min length check
		if(rxlen < index + FrameFixedLen_DH){		
			sprintf(&dispBuf[dispIdx], "结果: 超时,无应答");
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
			sprintf(&dispBuf[dispIdx], "结果: 有应答,CRC错误");
			return CmdResult_CrcError;
		}

		// pass
		break;
	}

	startIdx = index;

	// 跳过 帧头、长度
	index += 3;

	

	// 数据域解析
	payloadIdx = index;

	// 功能码
	funcId = buf[index] & 0x7F;
	index += 1;

	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		命令解析 		--------------------------------------
	//-------------------------------------------------------------------------------------------------
	
	case UserCmd_SetPeriodReportFreq:	// 设置周期上报频率
		if(rxlen < index + 13 || funcId != Func_SetInternalParam){
			break;
		}
		ret = CmdResult_Ok;
		// 密码域
		index += 6;
		// 设备版本
		index += 2;
		// 设备类型
		index += 2;
		// 参数个数
		u8Tmp = buf[index];
		index += 1;
		// 参数序号、更新状态
		if(u8Tmp == 1 && buf[index] == Tag_PeriodReportFreq && buf[index + 1] == 0x00){
			ret = CmdResult_Ok;
		}
		else{
			ret = CmdResult_Failed;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", DonghaiIR_GetStrResultCode(buf[index + 1]));
		index += 2;
		break;

	default:
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "该命令[%02X]暂未解析\n", cmdId);
		break;
	}


	if(index == startIdx + length - 3)
	{
		// 解析正确
	}
	else{
		#if LOG_ON
		dispIdx += sprintf(&dispBuf[dispIdx], "解析异常！\n");
		#endif
	}

	dispBuf[dispIdx] = '\0';
	
	
	return ret;
}


//--------------------------------------	命令发送、接收、结果显示	----------------------------
/*
* 描述： 命令发送/接收解析	- 执行完成后，等待按键：上/下键 - 滚动显示， 确认/取消键 - 返回
* 参数： cmdid	- 当前命令标识
*		addrs	- 地址域		
*		args	- 命令参数：args->items[0] - 命令ID, args->items[1] - 数据域
*		ackLen	- 应答长度 (byte) <期待应答长度，实际没用到>
*		timeout	- 超时时间 (ms)  默认为 8s + 中继数 x 2 x 6s
*		tryCnt	- 重试次数 默认3次
* 返回： uint8	- 界面退出时的按键值：确认键，取消键	
*/
uint8 DonghaiIR_TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// 应答长度、超时时间、重发次数
	ackLen += FrameFixedLen_DH;		// 期待应答长度，实际没用到
	timeout = 2000;
	tryCnt = 3;

	FramePack = Pack_DonghaiIR_RequestFrame;
	FrameExplain = Explain_DonghaiIR_ResponseFrame;

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
