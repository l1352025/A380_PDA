#ifndef ProtoHandle_8009_PY_H
#define ProtoHandle_8009_PY_H

#include "stdio.h"
#include "string.h"
#include "Common.h"

#ifdef Project_8009_RF_PY
#include "MeterDocDBF_8009_PY.h"
#endif

extern void CycleInvoke_OpenLcdLight_WhenKeyPress(uint8 currKey);
extern uint8 PackWater8009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWater8009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

FuncCmdCycleHandler TranceiverCycleHook = CycleInvoke_OpenLcdLight_WhenKeyPress;
FuncCmdFramePack FramePack = PackWater8009RequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWater8009ResponseFrame;

//----------------------------------------  表端命令  ------------------------

typedef enum{
	/*
	常用功能：	
	1	读表数据
	2	表开阀
	3	表关阀
	4	清除异常
	*/
	WaterCmd_ReadRealTimeData			= 0x11,	
	WaterCmd_OpenValve,	
	WaterCmd_CloseValve,
	WaterCmd_ClearException

}WaterCmdDef;


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

//-----------------------------------		8009水表协议 打包 / 解包	-----------------------------

/*
* 函数名：PackWater8009RequestFrame
* 描  述：打包8009水表命令请求帧
* 参  数：buf	- 数据缓存起始地址
		  addrs - 地址域：源地址、中继地址、目的地址
		  cmdId - 命令字
		  args	- 数据域参数
		  retryCnt - 重发次数：0 - 第1次发送，其他 - 第n次重发
* 返回值：uint8 帧总长度
*/
uint8 PackWater8009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt)
{
	static uint16 index = 0;
	uint8 i, cmd , crc8, relayCnt;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = 0xFE;		// 帧头： 固定为 FE 68
	buf[index++] = 0x68;
	buf[index++] = 0x01;		// 设备类型：00 - 水表， 01 - 燃气表 

	// 表模块ID：5 byte	BCD 高位在前
	memcpy(&buf[index], addrs->items[addrs->itemCnt - 1], AddrLen);
	index += AddrLen;			
	
	// 控制字2： bit7~4 路由级数 0 - 无路由， 4 - 1级， 8~15 - 2~9级 
	//			bit3~0 当前位置 0 - 主机， 2 - 1级， 4~11 - 2~9级 
	relayCnt = addrs->itemCnt - 1;
	switch (relayCnt)
	{
	case 0: buf[index++] = 0x00; break;
	case 1: buf[index++] = 0x40; break;
	case 2: buf[index++] = 0x80; break;
	default:
		break;
	}
	// 控制字1： bit7 0/1-下/上行，bit6 0/1-应答/上报，bit5~0 - 命令字
	cmd = *args->items[0];
	buf[index++] = cmd;			
	
	// 数据域长度：路由+数据部分
	buf[index++] = relayCnt * AddrLen + args->lastItemLen;
	
	// 数据域-路由（最多2个）
	for(i = 0; i < relayCnt; i++){
		memcpy(&buf[index], addrs->items[i], AddrLen);
		index += AddrLen;
	}
	
	// 数据域-数据部分
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	buf[index++] = 0x55;		// 下行场强
	buf[index++] = 0xAA;		// 上行场强
	crc8 = GetCrc8(&buf[0], index);
	buf[index++] = crc8;		// crc8 校验: 帧长 -- 上行场强
	buf[index++] = 0x16;		// 结束符

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
uint8 ExplainWater8009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
{
	bool ret = CmdResult_Failed;
	uint8 crc8, relayCnt, cmd, subCmd, u8Tmp;
	uint16 index = 0, dispIdx, length, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	double f64Tmp;
	char *ptr;

	dispIdx = 0;

	// 显示表号
	#if (AddrLen < 8)
	dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", StrDstAddr);
	#else
	dispIdx += sprintf(&dispBuf[dispIdx], "表号: \n   %s\n", StrDstAddr);
	#endif

	// 缓冲区多包中查找
	while(1){

		// min length check
		if(rxlen < index + 15){		
			sprintf(&dispBuf[dispIdx], "结果: 超时,无应答");
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
			sprintf(&dispBuf[dispIdx], "结果: 有应答,CRC错误");
			return CmdResult_CrcError;
		}

		// pass
		break;
	}

	startIdx = index;

	// 跳过 帧头、设备类型
	index += 3;

	// 表号ID
	if(memcmp(&buf[index], DstAddr, AddrLen) != 0){

		// 广播命令时，将应答的表地址作为新的目的地址
		GetStringHexFromBytes(StrDstAddr, buf, index, AddrLen, 0, false);

		dispIdx = 0;
		#if (AddrLen <= 6)
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: \n   %s\n", StrDstAddr);
		#endif
	}
	index += AddrLen;

	// 控制字2：路由级别|当前位置
	switch ((uint8)(buf[index] >> 4))
	{
	case 0: relayCnt = 0; break;
	case 4: relayCnt = 1; break;
	case 8: relayCnt = 2; break;
	default: break;
	}
	index += 1;

	// 控制字1：命令字
	cmd = (uint8)(buf[index] & 0x1F);
	index += 1;

	// 跳过 数据域长度 --> 路由信息
	index += (1 + relayCnt * AddrLen);

	// 子命令字
	subCmd = buf[index]; // 可能没有
	
	// 数据域解析
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	表端命令 0x01 ~ 0x25 , 70 ~ 74		--------------------
	//-------------------------------------------------------------------------------------------------
	//----------------------------------------		读取用户用量		-------------
	case WaterCmd_ReadRealTimeData:	// 读取用户用量
		if(rxlen < index + 9 || cmd != 0x01){
			break;
		}
		ret = CmdResult_Ok;
		// 表读数
		u32Tmp = GetUint32(&buf[index], 3, false);
		u8Tmp = buf[index + 3];
		dispIdx += sprintf(&dispBuf[dispIdx], "表读数: %d.%02d\n", u32Tmp, u8Tmp);
		index += 4;
		
		// 批量抄表时
		if(MeterInfo.dbIdx != Invalid_dbIdx){	
			// 写入DBF文件
			sprintf(MeterInfo.currReadVal, "%d.%02d", u32Tmp, u8Tmp);
			// 跳过其他字段解析
			index += 5;
			break;
		}

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

	case WaterCmd_OpenValve:		// 开阀
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


	if(index == startIdx + length - 4)
	{
		//下行/上行 信号强度
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
		
		#ifdef Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
			//	sprintf(MeterInfo.signalValue, "%d", TmpBuf[1]);	// 保存上行
			}
		#endif
		dispIdx += sprintf(&dispBuf[dispIdx], "                    \n");
		dispIdx += sprintf(&dispBuf[dispIdx], "下行: %d  上行: %d\n", TmpBuf[0],  TmpBuf[1]);
		index += 2;
	}
	else{
		#if LOG_ON
		dispIdx += sprintf(&dispBuf[dispIdx], "解析异常！\n");
		#endif
	}

	dispBuf[dispIdx] = '\0';
	
	
	return ret;
}

//--------------------------------------	8009水表命令 发送、接收、结果显示	----------------------------
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
uint8 Protol8009TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// 应答长度、超时时间、重发次数
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
