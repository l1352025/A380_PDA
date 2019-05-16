#ifndef WaterMeter_H
#define WaterMeter_H

#include "stdio.h"
#include "Common.h"

//----------------------------------------  表端命令  ------------------------
/*
表端操作：	
1	常用命令
2	测试命令
3	程序升级
4	预缴用量
5	工作参数
6	其他操作
*/
typedef enum{

	WaterCmd_ReadMeterCfgInfo	 	= 0x04,	// 读取表端参数配置信息
	
	/*
	常用命令：	
	1	读取用户用量
	2	读取冻结正转数据
	3	开阀
	4	强制开阀
	5	关阀
	6	强制关阀
	7	清异常命令
	*/
	WaterCmd_ReadRealTimeData		= 0x11,
	WaterCmd_ReadFrozenData,
	WaterCmd_OpenValve,	
	WaterCmd_OpenValveForce,
	WaterCmd_CloseValve,
	WaterCmd_CloseValveForce,	
	WaterCmd_ClearException,

	/*
	测试命令：	
	1	表端重启
	2	读表温度
	3	读表电压
	4	清预缴参考量
	5	设置过流超时
	6	读运营商编号
	7	读上报路径
	8	设置表号
	*/
	WaterCmd_RebootDevice				= 0x21,
	WaterCmd_ReadTemperature,
	WaterCmd_ReadVoltage, 
	WaterCmd_ClearPrepaidRefVal,
	WaterCmd_SetOverCurrentTimeout,
	WaterCmd_ReadOperatorNumber,	
	WaterCmd_ReadReportRoute,
	WaterCmd_SetMeterNumber,

	/*
	程序升级：	
	1	单表升级
	2	查询升级
	3	广播升级
	4	添加档案
	5	删除档案
	6	查询档案
	7	升级统计
	*/
	WaterCmd_SingleUpgrade			= 0x31,
	WaterCmd_QueryUpgrade,
	WaterCmd_BroadcastUpgrade, 
	WaterCmd_DocAdd,
	WaterCmd_DocDelete,
	WaterCmd_DocQuery,	
	WaterCmd_UpgradeStatistics,

	/*
	预缴用量：	
	1	读预缴参考用量
	2	设预缴参考用量
	3	读报警限值透支
	4	设报警限值
	5	设关阀限值
	6	设报警关阀限值
	*/
	WaterCmd_ReadPrepaidRefVal		= 0x41,
	WaterCmd_SetPrepaidRefVal,
	WaterCmd_ReadAlarmLimitOverdraft, 
	WaterCmd_SetAlarmLimit,
	WaterCmd_SetCloseValveLimit,
	WaterCmd_SetAlarmAndCloseValveLimit,	

	/*
	工作参数：	
	1	设表底数脉冲系数
	2	清除反转计量数据
	3	读取功能使能状态
	4	设置定时上传
	5	设置定量上传
	6	设置定时定量上传
	7	读表端时钟
	8	校表端时钟
	*/
	WaterCmd_SetBaseValPulseRatio	= 0x51,
	WaterCmd_ClearReverseMeasureData,
	WaterCmd_ReadFuncEnableState, 
	WaterCmd_SetTimedUpload,
	WaterCmd_SetFixedValUpload,
	WaterCmd_SetTimedAndFixedValUpload,	
	WaterCmd_ReadMeterTime,
	WaterCmd_SetMeterTime,

	/*
	其他操作：	
	1	读收/发/磁扰次数
	2	读取RXD和TXD信道
	3	设置RXD和TXD信道
	4	设置运营商编号
	5	路径下发
	*/
	WaterCmd_ReadRxTxMgnDistbCnt		= 0x61,
	WaterCmd_ReadRxdAndTxdChanel,
	WaterCmd_SetRxdAndTxdChanel, 
	WaterCmd_SetOperatorNumber,
	WaterCmd_SetDefinedRoute
}WaterCmdDef;


//----------------------------------------  集中器命令  ------------------------
/*
集中器操作：	
1	常用命令
2	档案操作
3	路径设置
4	命令转发
*/
typedef enum{
	/*
	常用操作：	
	1	读集中器号
	2	读集中器版本
	3	读集中器时钟
	4	设集中器时钟
	5	读GPRS参数
	6	设GPRS参数
	7	读GPRS信号强度
	8	集中器初始化
	9	读集中器工作模式
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
	档案操作：	
	1	读档案数量
	2	读档案信息
	3	添加档案信息
	4	删除档案信息
	5	修改档案信息
	*/
	CenterCmd_ReadDocCount		= 0x1021,
	CenterCmd_ReadDocInfo,
	CenterCmd_AddDocInfo,
	CenterCmd_DeleteDocInfo,
	CenterCmd_ModifyDocInfo,

	/*
	路径设置：	
	1	读自定义路由
	2	设自定义路由
	*/
	CenterCmd_ReadDefinedRoute	= 0x1031,
	CenterCmd_SetDefinedRoute,

	/*
	命令转发：	
	1	读实时数据
	2	读定时定量数据
	3	读冻结数据
	4	开阀
	5	关阀
	6	读使能
	7	清异常
	*/
	CenterCmd_ReadRealTimeData	= 0x1041,
	CenterCmd_ReadFixedTimeData,
	CenterCmd_ReadFrozenData,
	CenterCmd_OpenValve,
	CenterCmd_CloseValve,
	CenterCmd_ReadEnableState,
	CenterCmd_ClearException

}CenterCmdDef;



//---------------------------------------		6009 解析函数	-------------------------------------

/*
* 描  述：将字符串地址打包成6009水表地址域
* 参  数：addrs			- 地址域结构
*		  strDstAddr	- 目的地址字符串
* 		  strRelayAddrs - 中继地址字符串数组
* 返回值：void
*/
void Water6009_PackAddrs(ParamsBuf *addrs, const uint8 strDstAddr[], const uint8 strRelayAddrs[][20])
{
	uint8 i;

	// 源地址
	addrs->itemCnt = 0;
	addrs->items[addrs->itemCnt] = &addrs->buf[0];
	memcpy(addrs->items[addrs->itemCnt], LocalAddr, 6);
	addrs->itemCnt++;
	// 中继地址
	for(i = 0; i < RELAY_MAX; i++){
		if(strRelayAddrs[i][0] >= '0' && strRelayAddrs[i][0] <= '9'){
			addrs->items[addrs->itemCnt] = &addrs->buf[6 + i*6];
			GetBytesFromStringHex(addrs->items[addrs->itemCnt], 0, 6, strRelayAddrs[i], 0, false);
			addrs->itemCnt++;
		}
	}
	// 目的地址
	GetBytesFromStringHex(DstAddr, 0, 6, strDstAddr, 0, false);
	addrs->items[addrs->itemCnt] = &addrs->buf[6 + i*6];
	memcpy(addrs->items[addrs->itemCnt], DstAddr, 6);
	addrs->itemCnt++;
}

/*
* 描  述：获取6009水表读数类型名
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrValueType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x00:	str = "实时";	break;
	case 0x01:	str = "定量上传";	break;
	case 0x02:	str = "定时上传";	break;
	case 0x03:	str = "报警上传";	break;
	case 0x04:	str = "冻结";	break;
	default:
		str = "未知";
		break;
	}

	return str;
}

/*
* 描  述：解析6009水表-告警状态字1
* 参  数：status	- 状态字
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrAlarmStatus1(uint8 status)
{
	char * str = NULL;
	uint8 mask = 1, i;

	for(i = 0; i < 8; i++){

		mask = (1 << i);
		
		switch(status & mask){
		case 0x01:	str = "干簧管故障";	break;
		case 0x02:	str = "阀到位故障";	break;
		case 0x04:	str = "传感器线断开";	break;
		case 0x08:	str = "电池欠压";	break;
		case 0x10:	str = "光电表，一组光管坏";	break;
		case 0x20:	str = "磁干扰标志";	break;
		case 0x40:	str = "光电表，多组光管坏";	break;
		case 0x80:	str = "光电表，正强光干扰";	break;
		default:
			break;
		}

		if(str != NULL){
			break;
		}
	}

	if(str == NULL){
		str = " ";
	}

	return str;
}

/*
* 描  述：解析6009水表-告警状态字2
* 参  数：status	- 状态字
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrAlarmStatus2(uint8 status)
{
	char * str = NULL;
	uint8 mask = 1, i;

	for(i = 0; i < 6; i++){

		mask = (1 << i);
		
		switch(status & mask){
		case 0x01:	str = "水表反转";	break;
		case 0x02:	str = "水表被拆卸";	break;
		case 0x04:	str = "水表被垂直安装";	break;
		case 0x08:	str = "EEPROM异常";	break;
		case 0x10:	str = "煤气泄漏";	break;
		case 0x20:	str = "欠费标志";	break;
		default:	
			break;
		}

		if(str != NULL){
			break;
		}
	}

	if(str == NULL){
		str = " ";
	}
	
	return str;
}

/*
* 描  述：获取6009水表 阀门状态
* 参  数：status	- 状态
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrValveStatus(uint8 status)
{
	char * str = NULL;
	
	switch(status){
	case 0:	str = "故障";	break;
	case 1:	str = "开";	break;
	case 2:	str = "关";	break;
	default:
		str = "未知";
		break;
	}

	return str;
}
/*
* 描  述：获取6009水表 集中器错误码
* 参  数：status	- 状态
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrErrorMsg(uint8 errorCode)
{
	char * str = NULL;

	switch(errorCode){
	case 0xAA:
		str = "操作成功";
		break;
	case 0xAB:
	    str = "操作失败";
		break;
	case 0xAC:
	    str = "通讯失败";
		break;
	case 0xAD:
	    str = "命令下达成功";
		break;
	case 0xAE:
	    str = "格式错误";
		break;
	case 0xAF:
	    str = "预留";
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
	    str = "超时错误";
		break;
	case 0xCD:
	    str = "单论运行超时";
		break;
	case 0xCE:
	    str = "正在执行";
		break;
	case 0xCF:
	    str = "操作已处理";
		break;
	case 0xD0:
	    str = "已应答";
		break;
	case 0xD1:
	    str = "抄表错误";
		break;
	case 0xD2:
	    str = "无此功能";
		break;
	default:
		str = "未知错误";
		break;
	}

	return str;
}

/*
* 描  述：获取6009水表 阀控失败原因
* 参  数：errorCode	- 错误码
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrValveCtrlFailed(uint16 errorCode)
{
	char * str = NULL;
	uint16 mask = 1, i;

	for(i = 0; i < 12; i++){

		mask = (mask << i);
		
		switch(errorCode & mask){
		case 0x01:	str = "电池欠压";	break;
		case 0x02:	str = "磁干扰中";	break;
		case 0x04:	str = "Adc正在工作";	break;
		case 0x08:	str = "阀门正在运行";	break;
		case 0x10:	str = "阀门故障";	break;
		case 0x20:	str = "RF正在工作";	break;
		case 0x40:	str = "任务申请失败";	break;
		case 0x80:	str = "等待按键开阀";	break;
		case 0x100:	str = "阀门已经到位";	break;
		case 0x200:	str = "设备类型错误";	break;
		case 0x400:	str = "time申请失败";	break;
		case 0x800:	str = "系统欠费";	break;
		default:	
			break;
		}

		if(str != NULL){
			break;
		}
	}

	if(str == NULL){
		str = " ";
	}

	return str;
}


//-----------------------------------		6009水表协议 打包 / 解包	-----------------------------


/*
* 函数名：PackWater6009RequestFrame
* 描  述：打包6009水表命令请求帧
* 参  数：buf	- 数据缓存起始地址
		  addrs - 地址域：源地址、中继地址、目的地址
		  cmdId - 命令字
		  args	- 数据域参数
		  retryCnt - 重发次数：0 - 第1次发送，其他 - 第n次重发
* 返回值：uint8 帧总长度
*/
uint8 PackWater6009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint8 cmdId, ParamsBuf *args, uint8 retryCnt)
{
	static uint8 Fsn = 0;
	static uint16 index = 0;
	uint8 i, cmd , crc8;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = 0xD3;		// 帧头同步码： 固定为 D3 91
	buf[index++] = 0x91;
	buf[index++] = 0x00;		// 长度： 报文标识 --> 结束符16
	buf[index++] = 0x00;	
	buf[index++] = 0x10;		// 报文标志 bit7 0/1 - 下行/上行， bit6 0/1 - 命令/应答， bit4 固定为1
	buf[index++] = Fsn++;		// 任务号： mac fsn 发起方自累加
	cmd = *args->items[0];
	buf[index++] = cmd;			// 命令字
	buf[index++] = 0xFE;		// 设备类型: FE - 手持机， 10 - 冷水表， 11 - GPRS水表
	buf[index++] = 0x0F;		// 生命周期
	buf[index++] = addrs->itemCnt & 0x0F;	// 路径信息:  当前位置|路径长度
	// 地址域
	for(i = 0; i < (addrs->itemCnt & 0x0F); i++){
		memcpy(&buf[index], addrs->items[i], 6);
		index += 6;
	}

	// 数据域
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	buf[index++] = 0x00;		// 下行场强
	buf[index++] = 0x00;		// 上行场强
    // 长度计算
	buf[2] = (uint8)(index & 0xFF);	
	buf[3] = (uint8)(index >> 8);	
    
	crc8 = GetCrc8(&buf[2], index - 2);
	buf[index++] = crc8;		// crc8 校验
	buf[index++] = 0x16;		// 结束符

		

	if(cmd < 0x40 || cmd == 0x70 || cmd == 0x74){
		buf[index++] = 0x1E;	// 导言长度标识
		buf[index++] = 0x03;	// 表端APP时 发送信道
		buf[index++] = 0x19;	// 表端APP时 接收信道
	}else if(cmd > 0x70 && cmd < 0x74){
		buf[index++] = 0x1E;	// 导言长度标识
		buf[index++] = 0x03;	// 表端Boot时 发送信道
		buf[index++] = 0x19;	// 表端Boot时 接收信道
	}else{
		buf[index++] = 0x00;	// 导言长度标识
		buf[index++] = 0x19;	// 集中器 发送信道
		buf[index++] = 0x03;	// 集中器 接收信道
	}
	
	return index;
}

/*
* 函数名：ExplainWater6009ResponseFrame
* 描  述：解析水表命令响应帧
* 参  数：buf		- 接收缓存起始地址
*		  rxlen		- 接收的长度
*		  dstAddr	- 目的地址，判断接收的目的地址是否是自己
*		  cmdId		- 命令字
*		  ackLen	- 应答长度
*		  disp 		- 解析的显示数据
* 返回值：bool 解析结果：fasle - 失败 ， true - 成功
*/
bool ExplainWater6009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint8 cmdId, uint16 ackLen, ParamsBuf * disps)
{
	bool ret = false;
	uint8 crc8, addrsCnt, cmd, i, u8Tmp;
	uint16 index = 0, dispIdx, length, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	uint8 *ptr;

	// 显示表号
	dispIdx = 0;
	disps->items[0] = &disps->buf[0];
	disps->itemCnt = 1;
	dispIdx += sprintf(&disps->buf[dispIdx], "表号: %s\n", StrDstAddr);

	// 缓冲区多包中查找
	while(1){

		if(rxlen < index + 27){
			sprintf(&disps->buf[dispIdx], "结果: 超时,无应答");
			return false;
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

		// dstaddr check
		addrsCnt = buf[index + 7];
		if(memcmp(&buf[index + 8 + (addrsCnt - 1) * 6], dstAddr, 6) != 0){
			index += length;
			continue;
		}

		// crc8 check
		crc8 = GetCrc8(&buf[index], length - 2);
		if(crc8 !=  buf[index + length - 2]){
			sprintf(&disps->buf[dispIdx], "结果: 有应答,CRC错误");
			return false;
		}

		// pass
		break;
	}

	startIdx = index;

	// 命令字
	cmd = buf[index + 4];

	// 跳过 长度 --> 路径信息
	index += 8;
	// 跳过 地址域
	index += addrsCnt * 6;


	// 数据域解析
	payloadIdx = index;
	switch(cmdId){
	
	//----------------------------------------		读取用户用量		-------------
	case WaterCmd_ReadRealTimeData:	// 读取用户用量

		if(rxlen < index + 21 && cmd != 0x01){
			break;
		}
		ret = true;
		
		// 类型
		ptr = Water6009_GetStrValueType((buf[index] >> 4));
		dispIdx += sprintf(&disps->buf[dispIdx], "类型: %s\n", ptr);
		index += 1;
		// 正转用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&disps->buf[dispIdx], "正转: %d.%03d\n", u32Tmp, u16Tmp);
		// 反转用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&disps->buf[dispIdx], "反转: %d.%03d\n", u32Tmp, u16Tmp);
		//告警状态字1
		ptr = Water6009_GetStrAlarmStatus1(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "告警: %s ", ptr);
		index += 1;
		//告警状态字2
		ptr = Water6009_GetStrAlarmStatus2(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "%s\n", ptr);
		index += 1;
		//阀门状态 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "阀门: %s  ", ptr);
		index += 1;
		//电池电压
		dispIdx += sprintf(&disps->buf[dispIdx], "电池: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&disps->buf[dispIdx], "温度: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR 噪音比
		dispIdx += sprintf(&disps->buf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx信道、协议版本 跳过
		index += 2;
		break;

	//----------------------------------------		读取冻结数据	---------------------
	case WaterCmd_ReadFrozenData:	// 读取冻结数据
		if(rxlen < index + 88 && cmd != 0x02){
			break;
		}
		ret = true;
		// 冻结数据类型
		dispIdx += sprintf(&disps->buf[dispIdx], "类型: %s\n", (buf[index] == 0x01 ? "正传" : "反转"));
		index += 1;

		if(rxlen < index + 104){	// 冻结数据格式-旧版本 1 + 78 byte
			// 冻结数据起始序号
			u8Tmp = buf[index] * 10;
			dispIdx += sprintf(&disps->buf[dispIdx], "范围: 第 %d~%d 条\n", u8Tmp, u8Tmp + 9);
			index += 1;
			// 冻结数据起始时间
			dispIdx += sprintf(&disps->buf[dispIdx], "时间: %X%x%x%x %x:00:00\n"
				, buf[payloadIdx + 2], buf[payloadIdx + 3], buf[payloadIdx + 4], buf[payloadIdx + 5], buf[payloadIdx + 6]);
			index += 5;
			// 冻结数据方式 ：0-按天, 1-按月
			// 冻结数据数量 ：按天最大24条，按月最大30条
			dispIdx += sprintf(&disps->buf[dispIdx], "方式: 每%s冻结%d条\n", (buf[index] == 0x01 ? "天" : "月"), buf[index + 1]);
			index += 2;	
			// 冻结数据时间间隔
			if(buf[index] == 0){
				dispIdx += sprintf(&disps->buf[dispIdx], "间隔: 每%s冻结1条\n", (buf[index - 2] == 0x01 ? "天" : "月"));
			}
			else{
				dispIdx += sprintf(&disps->buf[dispIdx], "间隔: %d%s冻结1条\n", buf[index], (buf[index - 2] == 0x01 ? "小时" : "天"));
			}
			index += 1;
			// 冻结的用量数据：7*N 字节 （6 byte 用量 + 1 byte date.day）
			dispIdx += sprintf(&disps->buf[dispIdx], "读取的10条数据如下: \n");
			for(i = 0; i < 10; i++){
				u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
				index += 4;
				u16Tmp = ((buf[index + 1] << 8) + buf[index]);
				index += 2;
				dispIdx += sprintf(&disps->buf[dispIdx], "%d, %x/%x: %d.%03d\n", i, buf[payloadIdx + 4], buf[index], u32Tmp, u16Tmp);
				index +=1;
			}
		}
		else{		// 冻结数据格式-新版本	1 + 104 byte
			// 冻结数据起始序号
			dispIdx += sprintf(&disps->buf[dispIdx], "范围: 倒数第%d天数据\n", buf[index] + 1);
			index += 1;
			// 时间信息
			dispIdx += sprintf(&disps->buf[dispIdx], "时间: %02X-%02X %02X:%02X\n",
				buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			index += 4;
			// 累计用量
			u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
			index += 4;
			u16Tmp = ((buf[index + 1] << 8) + buf[index]);
			index += 2;
			dispIdx += sprintf(&disps->buf[dispIdx], "累计用量: %d.%03d\n", u32Tmp, u16Tmp);
			// 0:00 ~ 23:30 增量
			u8Tmp = 0;
			u16Tmp = 0x00;
			for(i = 0; i < 47; i++){
				dispIdx += sprintf(&disps->buf[dispIdx], "%d:%02X~", u8Tmp, u16Tmp);
				u16Tmp += 0x30;
				if(u16Tmp == 0x60){
					u16Tmp = 0x00;
					u8Tmp += 1;
				}
				dispIdx += sprintf(&disps->buf[dispIdx], "%d:%02X增量:%d\n", u8Tmp, u16Tmp, (buf[index] + buf[index + 1]*256));
				index += 2;
			}
		}

		//告警状态字1
		ptr = Water6009_GetStrAlarmStatus1(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "告警: %s ", ptr);
		index += 1;
		//告警状态字2
		ptr = Water6009_GetStrAlarmStatus2(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "%s\n", ptr);
		index += 1;
		//阀门状态 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "阀门: %s  ", ptr);
		index += 1;
		//电池电压
		dispIdx += sprintf(&disps->buf[dispIdx], "电池: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&disps->buf[dispIdx], "温度: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR 噪音比
		dispIdx += sprintf(&disps->buf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx信道
		dispIdx += sprintf(&disps->buf[dispIdx], "信道: Tx-%d, Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//协议版本
		dispIdx += sprintf(&disps->buf[dispIdx], "版本: %d\n", buf[index]);
		index += 1;
		break;

	//---------------------------------------		开关阀门	---------------------
	case WaterCmd_OpenValve:		// 开阀
	case WaterCmd_OpenValveForce:	// 强制开阀
	case WaterCmd_CloseValve:		// 关阀
	case WaterCmd_CloseValveForce:	// 强制关阀
		if(rxlen < index + 3 && cmd != 0x03){
			break;
		}
		ret = true;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		if(buf[index] == 0xAB){
			ptr = Water6009_GetStrValveCtrlFailed(((buf[index + 1] << 8) + buf[index + 2]));
			dispIdx += sprintf(&disps->buf[dispIdx], "原因: %s\n", ptr);
		}
		index += 3;
		break;

	//----------------------------------------		读取表端配置信息		-------------
	case WaterCmd_ReadMeterCfgInfo:	// 读取表端配置信息

		if(rxlen < index + 124 && cmd != 0x04){
			break;
		}
		ret = true;
		index += 84;
		memcpy(VerInfo, &buf[index], 40);
		memcpy(&TmpBuf[1020], &buf[index], 40);
		TmpBuf[1060] = 0x00;
		dispIdx += sprintf(&disps->buf[dispIdx], "版本: %s\n", &TmpBuf[1020]);
		index += 40;
		break;

	//---------------------------------------		清异常命令		---------------------
	case WaterCmd_ClearException:	// 清异常命令 
		if(rxlen < index + 1 && cmd != 0x05){
			break;
		}
		ret = true;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//---------------------------------------		测试命令	---------------------
	case WaterCmd_RebootDevice:	// 表端重启
		if(rxlen < index + 2 && cmd != 0x07){
			break;
		}
		ret = true;
		index += 1;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadTemperature:	// 读表温度
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = true;
		index += 1;
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&disps->buf[dispIdx], "温度: %s%d\n", ptr, (buf[index] & 0x7F));
        index += 1;
		break;

	case WaterCmd_ReadVoltage:	// 读表电压
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = true;
		index += 1;
		//电池电压
		dispIdx += sprintf(&disps->buf[dispIdx], "电池电压: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		break;

	case WaterCmd_ClearPrepaidRefVal:	// 清预缴参考量
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = true;
		index += 1;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_SetOverCurrentTimeout:	// 设置过流超时
		if(rxlen < index + 3 && cmd != 0x07){
			break;
		}
		ret = true;
		index += 1;
		// 命令状态
		if(buf[index] != Args.buf[2] || buf[index + 1] != Args.buf[3] || buf[index + 2] != Args.buf[4]){
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
			index += 1;
		}
		else{
			dispIdx += sprintf(&disps->buf[dispIdx], "结果: 操作成功\n");
			// 过流电流
			dispIdx += sprintf(&disps->buf[dispIdx], "过流电流: %d mA\n", buf[index]);
			index += 1;
			// 超时时间
			dispIdx += sprintf(&disps->buf[dispIdx], "超时时间: %d ms\n", (buf[index] + buf[index + 1] * 256));
			index += 2;
		}
		break;

	case WaterCmd_ReadOperatorNumber:	// 读运营商编号
		if(rxlen < index + 4 && cmd != 0x07){
			break;
		}
		ret = true;
		index += 1;
		// 运营商编号
		dispIdx += sprintf(&disps->buf[dispIdx], "运营商编号: %2X%2X%2X%2X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		break;

	case WaterCmd_ReadReportRoute:	// 读上报路径
		if(rxlen < index + 63 && cmd != 0x07){
			break;
		}
		ret = true;
		index += 1;

		// 路径1长度 6*n
		u8Tmp = buf[index] / 6;
		dispIdx += sprintf(&disps->buf[dispIdx], "路径1级数: %d\n", u8Tmp);
		index += 1;
		// 路径1地址列表
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&disps->buf[dispIdx], "%d : %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		index += (30 - u8Tmp * 6);

		// 路径2长度 6*n
		u8Tmp = buf[index] / 6;
		dispIdx += sprintf(&disps->buf[dispIdx], "  \n路径2级数: %d\n", u8Tmp);
		index += 1;
		// 路径2地址列表
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&disps->buf[dispIdx], "%d : %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		index += (30 - u8Tmp * 6);

		break;

	case WaterCmd_SetMeterNumber:	// 设置表号
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = true;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		预缴用量、参考用量-读取/设置	---------------------
	case WaterCmd_ReadPrepaidRefVal:	// 读预缴参考用量
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		// 预缴用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&disps->buf[dispIdx], "预缴用量: %d.%03d\n", u32Tmp, u16Tmp);
		// 参考用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&disps->buf[dispIdx], "参考用量: %d.%03d\n", u32Tmp, u16Tmp);
		break;

	case WaterCmd_SetPrepaidRefVal:		// 设预缴参考用量
		if(rxlen < index + 2 && cmd != 0x16){
			break;
		}
		ret = true;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "数据非法");
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 数据非法原因
		if(buf[index - 1] == 0xAE){
			if(buf[index] & 0x01 > 0){
				dispIdx += sprintf(&disps->buf[dispIdx], "-->参考起始用量不合法\n");
			}
			if(buf[index] & 0x02 > 0){
				dispIdx += sprintf(&disps->buf[dispIdx], "-->设置的预缴用量未达到开阀门限\n");
			}
			index += 1;
		}
		break;

	case WaterCmd_ReadAlarmLimitOverdraft:		// 读报警关阀限值
		if(rxlen < index + 1 && cmd != 0x17){
			break;
		}
		ret = true;
		// 命令状态
		ptr = (buf[index] == 0xAB ? "操作失败" : "操作成功");
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 操作成功时结果
		if(buf[index - 1] == 0xAA){
			dispIdx += sprintf(&disps->buf[dispIdx], "报警限值: %d\n", buf[index]);
			index += 1;
			u16Tmp = ((uint16)(buf[index + 1] << 8) + buf[index]);
			dispIdx += sprintf(&disps->buf[dispIdx], "关阀限值: %s%d\n", 
				((u16Tmp & 0x8000) > 0 ? "-" : ""), (u16Tmp & 0x7FFF));
			index += 2;
		}
		break;

	case WaterCmd_SetAlarmLimit:				// 设报警限值
	case WaterCmd_SetCloseValveLimit:			// 设关阀限值
	case WaterCmd_SetAlarmAndCloseValveLimit:	// 设报警关阀限值
		if(rxlen < index + 2 && cmd != 0x18){
			break;
		}
		ret = true;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 新版可能+ 2 byte
		break;

	//--------------------------------------		工作参数	---------------------
	case WaterCmd_SetBaseValPulseRatio:	// 设表底数脉冲系数
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;

		break;

	case WaterCmd_ClearReverseMeasureData:	// 清除反转计量数据
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_ReadFuncEnableState:	// 读取功能使能状态
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_SetTimedUpload:		// 设置定时上传
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_SetFixedValUpload:	// 设置定量上传
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_SetTimedAndFixedValUpload:	// 设置定时定量上传
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_ReadMeterTime:	// 读表端时钟
		if(rxlen < index + 7 && cmd != 0x13){
			break;
		}
		ret = true;
		dispIdx += sprintf(&disps->buf[dispIdx], "表端时间: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		break;

	case WaterCmd_SetMeterTime:		// 校表端时钟
		if(rxlen < index + 1 && cmd != 0x14){
			break;
		}
		ret = true;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		dispIdx += sprintf(&disps->buf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		其他操作		---------------------
	case WaterCmd_ReadRxTxMgnDistbCnt:		// 读收/发/磁扰次数
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;

		break;

	case WaterCmd_ReadRxdAndTxdChanel:	// 读取RXD和TXD信道
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_SetRxdAndTxdChanel:	// 设置RXD和TXD信道
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_SetOperatorNumber:		// 设置运营商编号
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_SetDefinedRoute:	// 路径下发
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	//--------------------------------------		程序升级		---------------------
	case WaterCmd_SingleUpgrade:		// 单表升级
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;

		break;

	case WaterCmd_QueryUpgrade:			// 查询升级
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_BroadcastUpgrade:		// 广播升级
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_DocAdd:			// 添加档案
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_DocDelete:		// 删除档案
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_DocQuery:			// 查询档案
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;

	case WaterCmd_UpgradeStatistics:	// 升级统计
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = true;
		
		break;
		
	default:
		ret = true;
		dispIdx += sprintf(&disps->buf[dispIdx], "该命令[%02X]暂未解析\n", cmd);
		break;
	}


	if(index == startIdx + length - 4)
	{
		//下行/上行 信号强度
		dispIdx += sprintf(&disps->buf[dispIdx], "                    \n");
		dispIdx += sprintf(&disps->buf[dispIdx], "下行: %d  上行: %d\n", buf[index], buf[index + 1]);
		index += 2;
	}

	disps->buf[dispIdx] = '\0';
	
	
	return ret;
}

#endif
