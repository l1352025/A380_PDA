#ifndef ProtoHandle_8009_PY_H
#define ProtoHandle_8009_PY_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#include "HJLIB.h"

#if defined Project_8009_RF_PY 
#include "MeterDocDBF_8009_PY.h"
#endif


#define FrameHeader1	0xA5		// 帧头 0xA5 0x68
#define FrameHeader2	0x68		 
#define FrameTail		0x16		// 帧尾 0x16	
#define FrameFixedLen_Downlink	20	// 下行帧-固定部分长度
#define FrameFixedLen_Uplink	37	// 上行帧-固定部分长度
#define NBiotBj_Ver		(uint8)2	// NBiot 北京水表协议 2.0	

extern void CycleInvoke_OpenLcdLight_WhenKeyPress(uint8 currKey);
extern uint8 PackWaterNBiotBjRequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWaterNBiotBjResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

FuncCmdCycleHandler TranceiverCycleHook = CycleInvoke_OpenLcdLight_WhenKeyPress;
FuncCmdFramePack FramePack = PackWaterNBiotBjRequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWaterNBiotBjResponseFrame;


//----------------------------------------  表端命令  ------------------------

// 功能码
typedef enum{
	Func_ParamSet	= 0x81,
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


typedef enum{
	/*
	常用功能：	
	1	读自定义数据
	2	设置表地址
	3	设置初值
	4	读取为峰模块数据
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


//---------------------------------------		8009 解析函数	-------------------------------------

/*
* 描  述：将字符串地址打包成8009水表地址域
* 参  数：addrs			- 地址域结构
*		  strDstAddr	- 目的地址字符串
* 		  strRelayAddrs - 中继地址字符串数组
* 返回值：void
*/
void Water8009_PackAddrs(ParamsBuf *addrs, const char strDstAddr[], const char strRelayAddrs[][20])
{
	/*
	参数传递方式1：const char strRelayAddrs[][20]
	参数传递方式2：const char (*strRelayAddrs)[20]
	参数传递方式3：const char **strRelayAddrs, uint addrLen
	 */
	uint8 i;

	// 中继地址
	addrs->itemCnt = 0;
	for(i = 0; i < RELAY_MAX; i++){
		if(strRelayAddrs[i][0] >= '0' && strRelayAddrs[i][0] <= '9'){
			addrs->items[addrs->itemCnt] = &addrs->buf[i*AddrLen];
			GetBytesFromStringHex(addrs->items[addrs->itemCnt], 0, AddrLen, strRelayAddrs[i], 0, false);
			addrs->itemCnt++;
		}
	}

	// 目的地址
	GetBytesFromStringHex(DstAddr, 0, AddrLen, strDstAddr, 0, false);
	addrs->items[addrs->itemCnt] = &addrs->buf[addrs->itemCnt*AddrLen];
	memcpy(addrs->items[addrs->itemCnt], DstAddr, AddrLen);
	addrs->itemCnt++;
}


/*
* 描  述：解析8009水表-告警状态字
* 参  数：status	- 状态字
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water8009_GetStrAlarmStatus(uint16 status, char *buf)
{
	char * str = NULL;
	uint16 mask = 1, i;
	uint16 len = 0;

	for(i = 0; i < 11; i++){

		mask = (1 << i);
		
		switch(status & mask){
		
		case 0x01:	str = "光电表-正强光干扰";	break;
		case 0x02:	str = "光电表-多组光管坏";	break;
		case 0x04:	str = "磁干扰标志";	break;
		case 0x08:	str = "光电表-一组光管坏";	break;
		case 0x10:	str = "电池欠压";	break;
		case 0x20:	str = "EEPROM异常";	break;
		case 0x40:	str = "阀到位故障";	break;
		case 0x2000:	str = "水表被垂直安装";	break;
		case 0x4000:	str = "水表被拆卸";	break;
		case 0x8000:	str = "水表反转";	break;
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
	else{	// 后面 ", " --> 替换为 " \n"
		buf[len - 2] = ' ';
		buf[len - 1] = '\n';
		buf[len] = 0x00;
	}

	return len;
}

/*
* 描  述：获取8009水表 阀门状态
* 参  数：status	- 状态
* 返回值：char *	- 解析后的字符串
*/
char * Water8009_GetStrValveStatus(uint8 status)
{
	char * str = NULL;
	
	if((status & 0x40) > 0){
		str = "开";	
	}
	else if((status & 0x20) > 0){
		str = "关";	
	}
	else{
		str = "未知";	
	}

	return str;
}
/*
* 描  述：获取8009水表 表端错误码
* 参  数：status	- 状态
* 返回值：char *	- 解析后的字符串
*/
char * Water8009_GetStrErrorMsg(uint8 errorCode)
{
	char * str = NULL;

	switch(errorCode){
	case 0xAA:
		str = "操作成功";
		break;
	case 0xAB:
	    str = "操作失败";
		break;
	case 0xBA:
	    str = "对象不存在";
		break;
	case 0xBB:
	    str = "对象重复";
		break;
	case 0xBC:
		str = "对象已满";
		break;
	case 0xCC:
	    str = "批量操作结束";
		break;
	case 0xEE:
	    str = "协议错误";
		break;
	
	default:
		str = "未知错误";
		break;
	}

	return str;
}



/*
* 描  述：NB-iot北京水表 功能码
* 参  数：code	- 编码值
* 返回值：char *	- 解析后的字符串
*/
char * NBiotBj_GetStrFunctionCode(uint8 code)
{
	char * str = NULL;

	switch(code){
	case 0x81: str = "参数设置"; break;
	case 0x82: str = "数据上报"; break;
	case 0x83: str = "信息查询"; break;
	case 0x84: str = "数据透传"; break;
	case 0x85: str = "历史数据补报"; break;
	case 0x86: str = "读取历史数据"; break;
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


//-----------------------------------		8009水表协议 打包 / 解包	-----------------------------

/*
* 函数名：PackWater8009RequestFrame
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
* 函数名：ExplainWater8009ResponseFrame
* 描  述：解析水表命令响应帧
* 参  数：buf		- 接收缓存起始地址
*		  rxlen		- 接收的长度
*		  dstAddr	- 目的地址，判断接收的目的地址是否是自己
*		  cmdId		- 命令字
*		  ackLen	- 应答长度
*		  dispBuf 	- 解析的显示数据
* 返回值：uint8 解析结果：0 - 成功 , 1 - 失败 ， 2 - CRC错误， 3 - 超时无应答
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
	cmd = buf[index];
	index += 1;
	// 加密标识
	index += 1; // 跳过
	// 数据域长度
	index += 2; // 跳过

	// 数据域解析
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	表端命令 0x01 ~ 0x25 , 70 ~ 74		--------------------
	//-------------------------------------------------------------------------------------------------
	//----------------------------------------		读取用户用量		-------------
	case Cmd_ReadCustomData:	// 读取用户用量
		if(rxlen < index + 9 || cmd != 0x01){
			break;
		}
		ret = CmdResult_Ok;
		// 表读数
		u32Tmp = GetUint32(&buf[index], 3, false);
		u8Tmp = buf[index + 3];
		dispIdx += sprintf(&dispBuf[dispIdx], "表读数: %d.%02d\n", u32Tmp, u8Tmp);
		index += 4;

		#ifdef Use_DBF
			// 批量抄表时
			if(MeterInfo.dbIdx != Invalid_dbIdx){	
				// 写入DBF文件
				sprintf(MeterInfo.currReadVal, "%d.%02d", u32Tmp, u8Tmp);
				// 跳过其他字段解析
				index += 5;
				break;
			}
		#endif

		// 表口径、脉冲系数
		ptr = (buf[index] & 0x80) > 0 ? "大" : "小";
		dispIdx += sprintf(&dispBuf[dispIdx], "表端口径: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲系数: %d\n", (buf[index] & 0x7F));
		index += 1;
		//电池电压
		f64Tmp = (double)buf[index] / 30.117534;
		ptr = _DoubleToStr(TmpBuf, f64Tmp, 2);
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压: %s v\n", ptr);
		index += 1;
		//阀门状态
		ptr = Water8009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门状态: %s\n", ptr);
		index += 1;
		//告警状态字
		u16Tmp = GetUint16(&buf[index], 2, true);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警状态: ");
		dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	case Cmd_SetMeterAddr:		// 设置表地址
		if(rxlen < index || cmd != 0x05){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		break;

	case WaterCmd_CloseValve:		// 关阀
		if(rxlen < index || cmd != 0x06){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		break;

	case WaterCmd_ClearException:	// 清异常命令 03
		if(rxlen < index || cmd != 0x03){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		break;

	default:
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "该命令[%02X]暂未解析\n", cmd);
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
*		ackLen	- 应答长度 (byte)
*		timeout	- 超时时间 (ms)  默认为 8s + 中继数 x 2 x 6s
*		tryCnt	- 重试次数 默认3次
* 返回： uint8	- 界面退出时的按键值：确认键，取消键	
*/
uint8 NBiotBj_TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// 应答长度、超时时间、重发次数
	ackLen += 10 + addrs->itemCnt * AddrLen;
	timeout = 2000 + (addrs->itemCnt - 1) * 2000;
	tryCnt = 3;

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	return key;
}

#endif
