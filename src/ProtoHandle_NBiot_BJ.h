#ifndef ProtoHandle_NBiot_BJ_H
#define ProtoHandle_NBiot_BJ_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#include "HJLIB.h"


#define FrameHeader1	0xA5		// 帧头 0xA5 0x68
#define FrameHeader2	0x68		 
#define FrameTail		0x16		// 帧尾 0x16	
#define FrameFixedLen_Downlink	20	// 下行帧-固定部分长度
#define FrameFixedLen_Uplink	37	// 上行帧-固定部分长度
#define NBiotBj_Ver		(uint8)2	// NBiot 北京水表协议 2.0	


extern uint8 PackWaterNBiotBjRequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWaterNBiotBjResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

#ifndef Frame_Pack_Parse
#define Frame_Pack_Parse
FuncCmdFramePack FramePack = PackWaterNBiotBjRequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWaterNBiotBjResponseFrame;
#endif

//----------------------------------------  表端命令  ------------------------

// 功能码:  发起时 0x01~0x06 ， 应答时 0x81~0x86
typedef enum{
	Func_ParamSet	= 0x01,
	Func_DataReport,
	Func_InfoQuery,
	Func_DataTransmit,
	Func_HistoryDataRpt,
	Func_ReadHistoryData,

	Func_Unknown
}FuncCode;

// 结果码
typedef enum{
	Reply_Success	= 0,
	Reply_InfoError,
	Reply_NotSupport,
	Reply_Failed,

	Reply_Unknown
}ReplyCode;

// Tag码
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
	常用功能：	
	1	读自定义数据
	2	设置密码
	3	设置表地址
	4	设置初值
	5	读取为峰模块数据
	*/
	UserCmd_ReadCustomData		= 0x11,	
	UserCmd_SetPassword,
	UserCmd_SetMeterAddr,	
	UserCmd_SetInitValue,	
	UserCmd_ReadWeiFengModuleData,	

	UserCmd_Unknown
}UserCmdCode;


/*
* 描  述：NB-iot 北京水表协议 CRC16 计算
* 参  数：
*		pucBuf	- 数据缓存地址
* 		uwLength - 计算校验的数据长度
* 返回值：uint16 - CRC16校验值
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


//---------------------------------------		解析函数	-------------------------------------

/*
* 描  述：NB-iot北京水表 功能码
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * NBiotBj_GetStrFunctionCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case Func_ParamSet: str = "参数设置"; break;
	case Func_DataReport: str = "数据上报"; break;
	case Func_InfoQuery: str = "信息查询"; break;
	case Func_DataTransmit: str = "数据透传"; break;
	case Func_HistoryDataRpt: str = "历史数据补报"; break;
	case Func_ReadHistoryData: str = "读取历史数据"; break;
	default:
	    str = "未知";
		break;
	}

	return str;
}

/*
* 描  述：NB-iot北京水表 结果码
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * NBiotBj_GetStrResultCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0: str = "处理成功"; break;
	case 1: str = "消息有误"; break;
	case 2: str = "不支持"; break;
	case 3: str = "处理失败"; break;
	default:
	    str = "未知";
		break;
	}

	return str;
}


/*
* 描  述：NB-iot北京水表 厂商代码
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * NBiotBj_GetStrManufacturerCode(uint8 code)
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
* 描  述：NB-iot北京水表 设备类型
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * NBiotBj_GetStrDeviceType(uint8 code)
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
uint8 PackWaterNBiotBjRequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt)
{
	static uint16 Fsn = 0;
	static uint16 index = 0;
	uint16 len, crc16;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = FrameHeader1;		// 帧头： 固定为 A5 68
	buf[index++] = FrameHeader2;
	len = FrameFixedLen_Downlink + args->lastItemLen;	// 长度: 固定20 + 数据域长度
	buf[index++] = (uint8)(len & 0xFF);	
	buf[index++] = (uint8)(len >> 8);	
	buf[index++] = (uint8)(Fsn & 0xFF);	// 序号
	buf[index++] = (uint8)(Fsn >> 8);
	Fsn++;
	buf[index++] = NBiotBj_Ver;			// 协议版本
	buf[index++] = (_GetYear() % 100);	// 平台时间
	buf[index++] = _GetMonth();	
	buf[index++] = _GetDay();	
	buf[index++] = _GetHour();	
	buf[index++] = _GetMin();	
	buf[index++] = _GetSec();
	buf[index++] = *args->items[0];		// 功能码
	buf[index++] = 0x00;				// 加密标识：0-不加密， 1-加密	
	
	// 数据域-长度
	buf[index++] = args->lastItemLen;
	buf[index++] = 0;
	
	// 数据域-数据部分
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	crc16 = GetCRC16_NBiotBj(&buf[0], index);
	buf[index++] = (uint8)(crc16 & 0xFF);	// crc16 校验: 帧头 --> 校验之前
	buf[index++] = (uint8)(crc16 >> 8);
	buf[index++] = FrameTail;				// 结束符

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
uint8 ExplainWaterNBiotBjResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
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
		if(rxlen < index + FrameFixedLen_Uplink){		
			sprintf(&dispBuf[dispIdx], "结果: 超时,无应答");
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
			sprintf(&dispBuf[dispIdx], "结果: 有应答,CRC错误");
			return CmdResult_CrcError;
		}

		// pass
		break;
	}

	startIdx = index;

	// 跳过 帧头、长度、序号、协议版本
	index += 7;

	// 厂家代码
	ptr = NBiotBj_GetStrManufacturerCode(buf[index]);
	dispIdx += sprintf(&dispBuf[dispIdx], "厂家编码: %s\n", ptr);
	index += 1;
	// 设备类型
	ptr = NBiotBj_GetStrDeviceType(buf[index]);
	dispIdx += sprintf(&dispBuf[dispIdx], "设备类型: %s\n", ptr);
	index += 1;
	// 模块IMEI
	ptr = memcpy(&TmpBuf[0], &buf[index], 15);
	TmpBuf[15] = 0x00;
	dispIdx += sprintf(&dispBuf[dispIdx], "模块IMEI: \n    %s\n", ptr);
	index += 15;
	// 信号质量
	index += 5; // 暂不解析：RSRP、SNR、覆盖等级
	dispIdx += sprintf(&dispBuf[dispIdx], "信号质量: %d\n", buf[index]);
	index += 1;

	// 功能码
	funcId = buf[index] & 0x7F;
	index += 1;
	// 加密标识
	index += 1; // 跳过
	// 数据域长度
	index += 2; // 跳过

	// 数据域解析
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		表端命令 		--------------------------------------
	//-------------------------------------------------------------------------------------------------
	
	case UserCmd_ReadCustomData:	// 读自定义数据
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
		// 正向流量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "正向流量: %d L\n", u32Tmp);
		// 反向流量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "反向流量: %d L\n", u32Tmp);
		// 电池电压
		u16Tmp = (buf[index + 1] << 8) + buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压: %d.%02d\n", (u16Tmp / 100), (u16Tmp % 100));
		index += 1;
		// 保留 3byte
		index += 3;
		break;

	case UserCmd_SetPassword:		// 设置密码
	case UserCmd_SetMeterAddr:		// 设置表地址
	case UserCmd_SetInitValue:		// 设置初值
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
		// 当前密码
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 16, NULL, false);
		dispIdx += sprintf(&dispBuf[dispIdx], "当前密码: %s\n", &TmpBuf[0]);
		index += 16;
		// 正向流量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "正向流量: %d L\n", u32Tmp);
		index += 4;
		// 反向流量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "反向流量: %d L\n", u32Tmp);
		index += 4;
		// 是否加密
		dispIdx += sprintf(&dispBuf[dispIdx], "是否加密: %s \n", buf[index] == 1 ? "是" : "否");
		index += 1;
		// 红外保持时间
		dispIdx += sprintf(&dispBuf[dispIdx], "红外保持时间: %d s\n", buf[index]);
		index += 1;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 7, NULL, true);
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 7;
		// CCID
		Memcpy_AddNull(&TmpBuf[0], &buf[index], 20);
		dispIdx += sprintf(&dispBuf[dispIdx], "CCID: %s\n", &TmpBuf[0]);
		index += 20;
		// CellID
		Memcpy_AddNull(&TmpBuf[0], &buf[index], 8);
		dispIdx += sprintf(&dispBuf[dispIdx], "CellID: %s\n", &TmpBuf[0]);
		index += 8;
		// 保留 1byte
		index += 1;
		break;

	
	case UserCmd_ReadWeiFengModuleData:	// 读取为峰模块数据
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
		// 为峰模块通信协议：68 Len CtrlCode Data CS 16
		// 跳过头部 2byte
		index += 2;	
		u8Tmp = GetSum8(&buf[index], 15);	// 累加和校验
		if(u8Tmp != buf[index + 15]){
			ret = CmdResult_Failed;
			dispIdx += sprintf(&dispBuf[dispIdx], "为峰模块数据校验和错误！\n");
			break;
		}
		index += 1;
		// 软件版本
		dispIdx += sprintf(&dispBuf[dispIdx], "软件版本: %d.%02d \n", buf[index], buf[index + 1]);
		index += 2;
		// 信号强度 、 强度差值
		u16Tmp = buf[index] | ((buf[index + 1] & 0x80) << 8);
		u8Tmp = (buf[index + 1] & 0x7F);
		dispIdx += sprintf(&dispBuf[dispIdx], "信号强度: %d \n", u16Tmp);
		dispIdx += sprintf(&dispBuf[dispIdx], "强度差值: %d \n", u8Tmp);
		index += 2;
		// 静脉冲
		i32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "静 脉 冲: %d \n", i32Tmp);
		index += 4;
		// 反向脉冲
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "反向脉冲: %d \n", u32Tmp);
		index += 4;
		// 状态字
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "状态字: [ %02X ]\n", u8Tmp);
		dispIdx += sprintf(&dispBuf[dispIdx], "    校准失败: %s\n", (u8Tmp & 0x01) > 0 ? "是" : " 否");
		dispIdx += sprintf(&dispBuf[dispIdx], "    快速采样: %s\n", (u8Tmp & 0x02) > 0 ? "是" : " 否");
		dispIdx += sprintf(&dispBuf[dispIdx], "    模块被拆: %s\n", (u8Tmp & 0x04) > 0 ? "是" : " 否");
		dispIdx += sprintf(&dispBuf[dispIdx], "    异物插入: %s\n", (u8Tmp & 0x08) > 0 ? "是" : " 否");
		dispIdx += sprintf(&dispBuf[dispIdx], "    信号强度有效: %s\n", (u8Tmp & 0x10) > 0 ? "是" : " 否");
		index += 1;
		// 保留 1byte
		index += 1;
		// 跳过尾部 2byte
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
uint8 NBiotBj_TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// 应答长度、超时时间、重发次数
	ackLen += FrameFixedLen_Uplink;		// 期待应答长度，实际没用到
	timeout = 2000;
	tryCnt = 3;

	FramePack = PackWaterNBiotBjRequestFrame;
	FrameExplain = ExplainWaterNBiotBjResponseFrame;

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
