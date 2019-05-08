#ifndef WaterMeter_H
#define WaterMeter_H

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
	CenterCmd_ReadCenterNo		= 0x11,
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
	CenterCmd_ReadDocCount		= 0x21,
	CenterCmd_ReadDocInfo,
	CenterCmd_AddDocInfo,
	CenterCmd_DeleteDocInfo,
	CenterCmd_ModifyDocInfo,

	/*
	路径设置：	
	1	读自定义路由
	2	设自定义路由
	*/
	CenterCmd_ReadDefinedRoute	= 0x31,
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
	CenterCmd_ReadRealTimeData	= 0x41,
	CenterCmd_ReadFixedTimeData,
	CenterCmd_ReadFrozenData,
	CenterCmd_OpenValve,
	CenterCmd_CloseValve,
	CenterCmd_ReadEnableState,
	CenterCmd_ClearException

}CenterCmdDef;



//---------------------------------------		6009 解析函数	-------------------------------------

/*
* 描  述：获取6009水表读数类型名
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrValueType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0:   
		str = "实时";
		break;
	case 1:
		str = "定量上传";
		break;
	case 2:
		str = "定时上传";
		break;
	case 3:
		str = "报警上传";
		break;
	case 4:
		str = "冻结";
		break;

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
		case 0x01:   
			str = "干簧管故障";
			break;
		case 0x02:
			str = "阀到位故障";
			break;
		case 0x04:
			str = "传感器线断开";
			break;
		case 0x08:
			str = "电池欠压";
			break;
		case 0x10:
			str = "光电表，一组光管坏";
			break;
		case 0x20:
			str = "磁干扰标志";
			break;
		case 0x40:
			str = "光电表，多组光管坏";
			break;
		case 0x80:
			str = "光电表，正强光干扰";
			break;
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
		case 0x01:   
			str = "水表反转";
			break;
		case 0x02:
			str = "水表被拆卸";
			break;
		case 0x04:
			str = "水表被垂直安装";
			break;
		case 0x08:
			str = "EEPROM异常";
			break;
		case 0x10:
			str = "煤气泄漏";
			break;
		case 0x20:
			str = "欠费标志";
			break;
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
	case 0:   
		str = "故障";
		break;
	case 1:
		str = "开";
		break;
	case 2:
		str = "关";
		break;
		
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
	uint8 i, crc8;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	index = 0;
	buf[index++] = 0xD3;		// 帧头同步码： 固定为 D3 91
	buf[index++] = 0x91;
	buf[index++] = 0x00;		// 长度： 报文标识 --> 结束符16
	buf[index++] = 0x00;	
	buf[index++] = 0x10;		// 报文标志 bit7 0/1 - 下行/上行， bit6 0/1 - 命令/应答， bit4 固定为1
	buf[index++] = Fsn++;	// 任务号： mac fsn 发起方自累加
	buf[index++] = *args->items[0];		// 命令字
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

		

	if(cmdId < 0x40 || cmdId == 0x70 || cmdId == 0x74){
		buf[index++] = 0x1E;	// 导言长度标识
		buf[index++] = 0x03;	// 表端APP时 发送信道
		buf[index++] = 0x19;	// 表端APP时 接收信道
	}else if(cmdId > 0x70 && cmdId < 0x74){
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
	uint8 crc8, addrsCnt, cmd;
	uint16 index = 0, startIdx, length, u16Tmp;
	uint8 *ptr, dispIdx;
	uint32 u32Tmp;

	// 缓冲区多包中查找
	while(1){

		if(rxlen < index + ackLen){
			disps->itemCnt = 1;
			disps->items[0] = &disps->buf[0];
            sprintf(disps->items[0], "未应答");
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
			disps->itemCnt = 1;
			disps->items[0] = &disps->buf[0];
            sprintf(disps->items[0], "CRC错误");
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

	// 显示表号
	dispIdx = 0;
	dispIdx += sprintf(&disps->buf[dispIdx], "表号: %s", StrDstAddr);

	// 数据域解析
	switch(cmdId){

	//-------------------------------------------  抄表		-------------
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
		if((buf[index] & 0x80) > 0){
			dispIdx += sprintf(&disps->buf[dispIdx], "温度: -%d  ", (buf[index] & 0x7F));
		}else{
			dispIdx += sprintf(&disps->buf[dispIdx], "温度: %d  ", (buf[index] & 0x7F));
		}
        index += 1;
		//SNR 噪音比
		dispIdx += sprintf(&disps->buf[dispIdx], "SNR: %d\n", buf[index]);
		index += 1;
		//tx|rx信道、协议版本 跳过
		index += 2;
		break;

	case WaterCmd_ReadFrozenData:	// 读取冻结数据
		if(rxlen < index + 21 && cmd != 0x02){
			break;
		}
		ret = true;

		break;

	case WaterCmd_OpenValve:		// 开阀
	case WaterCmd_OpenValveForce:	// 强制开阀
	case WaterCmd_CloseValve:		// 关阀
	case WaterCmd_CloseValveForce:	// 强制关阀
		if(rxlen < index + 3 && cmd != 0x03){
			break;
		}
		ret = true;
		// 命令状态
		if(buf[index] == 0xAD){
			dispIdx += sprintf(&disps->buf[dispIdx], "执行成功\n");
		}
		else{
			dispIdx += sprintf(&disps->buf[dispIdx], "执行失败\n");
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&disps->buf[dispIdx], "原因: %s \n", ptr);
		}
		index += 1;
		break;

	case WaterCmd_ClearException:	// 清异常命令 
		if(rxlen < index + 1 && cmd != 0x05){
			break;
		}
		ret = true;
		// 命令状态
		if(buf[index] == 0xAA){
			dispIdx += sprintf(&disps->buf[dispIdx], "执行成功\n");
		}
		else{
			dispIdx += sprintf(&disps->buf[dispIdx], "执行失败\n");
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&disps->buf[dispIdx], "原因: %s \n", ptr);
		}
		index += 1;
		break;

	//-------------------------------------------------		测试命令	---------------------
	case WaterCmd_RebootDevice:	// 表端重启
		if(rxlen < index + 21 && cmd != 0x07){
			break;
		}
		ret = true;
		// 命令状态
		if(buf[index] == 0xAA){
			dispIdx += sprintf(&disps->buf[dispIdx], "执行成功\n");
		}
		else{
			dispIdx += sprintf(&disps->buf[dispIdx], "执行失败\n");
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&disps->buf[dispIdx], "原因: %s \n", ptr);
		}
		index += 1;
		break;

	case WaterCmd_ReadTemperature:	// 读表温度
		if(rxlen < index + 21 && cmd != 0x07){
			break;
		}
		ret = true;
		// 命令状态
		if(buf[index] == 0xAA){
			dispIdx += sprintf(&disps->buf[dispIdx], "执行成功\n");
		}
		else{
			dispIdx += sprintf(&disps->buf[dispIdx], "执行失败\n");
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&disps->buf[dispIdx], "原因: %s \n", ptr);
		}
		index += 1;
		break;
		
	default:
		break;
	}


	if(index == startIdx + length - 4)
	{
		//下行/上行 信号强度
		dispIdx += sprintf(&disps->buf[dispIdx], "下行: %d  上行: %d\n", buf[index], buf[index + 1]);
		index += 2;
		disps->items[0] = &disps->buf[0];
		disps->itemCnt = 1;
	}
	
	return ret;
}

#endif
