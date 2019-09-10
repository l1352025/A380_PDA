#ifndef WaterMeter_H
#define WaterMeter_H

#include "stdio.h"
#include "Common.h"
#ifdef Project_6009_RF
#include "MeterDocDBF.h"
#endif

// --------------------------------  全局变量  -----------------------------------------
//char Screenbuff[160*(160/3+1)*2]; 
uint8 TmpBuf[1080];
uint8 TxBuf[1080];
uint8 RxBuf[1080];
uint8 DispBuf[2048];
uint32 RxLen, TxLen;
const uint8 LocalAddr[10] = { 0x20, 0x19, 0x00, 0x00, 0x20, 0x19, 0x00, 0x00, 0x00, 0x00};	// 地址 2019000020190000，12/16字符
uint8 DstAddr[10];
uint8 VerInfo[42];
uint16 CurrCmd;
ParamsBuf Addrs;		
ParamsBuf Args;
char StrBuf[TXTBUF_MAX][TXTBUF_LEN];    // extend input buffer
char StrDstAddr[TXTBUF_LEN];
char StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
UI_ItemList UiList;

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
	9	读debug信息
	10	清debug信息
	*/
	WaterCmd_RebootDevice				= 0x21,
	WaterCmd_ReadTemperature,
	WaterCmd_ReadVoltage, 
	WaterCmd_ClearPrepaidRefVal,
	WaterCmd_SetOverCurrentTimeout,
	WaterCmd_ReadOperatorNumber,	
	WaterCmd_ReadReportRoute,
	WaterCmd_SetMeterNumber,
	WaterCmd_ReadDebugInfo,
	WaterCmd_ClearDebugInfo,

	/*
	程序升级：	
	1	通知系统升级_在app
	2	通知系统升级_在boot
	3	发送升级数据
	4	查询升级状态_在boot
	5	查询升级状态_在app
	*/
	WaterCmd_NoticeUpgrade_OnApp		= 0x31,
	WaterCmd_NoticeUpgrade_OnBoot,
	WaterCmd_SendUpgradePacket, 
	WaterCmd_QueryUpgradeStatus_OnBoot,
	WaterCmd_QueryUpgradeStatus_OnApp,	

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
	9	设IP+端口+模式
	10	读IP+端口+模式
	11	读NB运营商编号
	12	读IMEI+CCID
	13  读取NB入网信息
	14  读取北京水表参数
	15  设置北京水表参数
	*/
	WaterCmd_SetBaseValPulseRatio	= 0x51,
	WaterCmd_ClearReverseMeasureData,
	WaterCmd_ReadFuncEnableState, 
	WaterCmd_SetTimedUpload,
	WaterCmd_SetFixedValUpload,
	WaterCmd_SetTimedAndFixedValUpload,	
	WaterCmd_ReadMeterTime,
	WaterCmd_SetMeterTime,
	WaterCmd_SetIpPortMode,
	WaterCmd_ReadIpPortMode,
	WaterCmd_ReadNbOperaterNumber,
	WaterCmd_ReadImeiAndCcid,
	WaterCmd_ReadNbJoinNetworkInfo,
	WaterCmd_ReadBeiJingWaterMeterParams,
	WaterCmd_SetBeiJingWaterMeterParams,

	/*
	其他操作：	
	1	读收发磁扰阀控数
	2	读取RXD和TXD信道
	3	设置RXD和TXD信道
	4	设置运营商编号
	5	路径下发
	*/
	WaterCmd_ReadRxTxMgnDistbCnt		= 0x61,
	WaterCmd_ReadRxdAndTxdChanel,
	WaterCmd_SetRxdAndTxdChanel, 
	WaterCmd_SetOperatorNumber,
	WaterCmd_SetDefinedRoute,
	/*
	UART表端模块测试：	
	1	读取模块运行参数
	2	设置模块运行参数
	*/
	WaterCmd_ReadModuleRunningParams,
	WaterCmd_SetModuleRunningParams
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


typedef enum{
	RxResult_Ok,
	RxResult_Failed,
	RxResult_CrcError,
	RxResult_Timeout
}CmdRxResult;


//---------------------------------------		6009 解析函数	-------------------------------------

/*
* 描  述：将字符串地址打包成6009水表地址域
* 参  数：addrs			- 地址域结构
*		  strDstAddr	- 目的地址字符串
* 		  strRelayAddrs - 中继地址字符串数组
* 返回值：void
*/
void Water6009_PackAddrs(ParamsBuf *addrs, const char strDstAddr[], const char strRelayAddrs[][20])
{
	/*
	参数传递方式1：const char strRelayAddrs[][20]
	参数传递方式2：const char (*strRelayAddrs)[20]
	参数传递方式3：const char **strRelayAddrs, uint addrLen
	 */
	#ifdef Project_6009_RF
	uint8 i;
	#endif

	// 源地址
	addrs->itemCnt = 0;
	addrs->items[addrs->itemCnt] = &addrs->buf[0];
	memcpy(addrs->items[addrs->itemCnt], LocalAddr, AddrLen);
	addrs->itemCnt++;

	#ifdef Project_6009_RF
	// 中继地址
	for(i = 0; i < RELAY_MAX; i++){
		if(strRelayAddrs[i][0] >= '0' && strRelayAddrs[i][0] <= '9'){
			addrs->items[addrs->itemCnt] = &addrs->buf[AddrLen + i*AddrLen];
			GetBytesFromStringHex(addrs->items[addrs->itemCnt], 0, AddrLen, strRelayAddrs[i], 0, false);
			addrs->itemCnt++;
		}
	}
	#endif

	// 目的地址
	GetBytesFromStringHex(DstAddr, 0, AddrLen, strDstAddr, 0, false);
	addrs->items[addrs->itemCnt] = &addrs->buf[addrs->itemCnt*AddrLen];
	memcpy(addrs->items[addrs->itemCnt], DstAddr, AddrLen);
	addrs->itemCnt++;
}

/*
* 描  述：获取6009水表-设备类型名
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrDeviceType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x10:	str = "RF冷水表";	break;
	case 0x11:	str = "GPRS冷水表";	break;
	case 0x12:	str = "NB-IoT冷水表";	break;
	case 0x20:	str = "RF热水表";	break;
	case 0x21:	str = "NB-IoT热水表";	break;
	case 0x30:	str = "RF气表";	break;
	case 0x31:	str = "GPRS气表";	break;
	case 0x32:	str = "NB-IoT气表";	break;
	case 0x40:	str = "电表";	break;

	case 0x50:	str = "透传模块";	break;

	case 0xF9:	str = "USB";	break;
	case 0xFA:	str = "上位机器";	break;
	case 0xFB:	str = "UART串口";	break;
	case 0xFC:	str = "集中器";	break;
	case 0xFD:	str = "中继器";	break;
	case 0xFE:	str = "手持机";	break;
	default:
		str = "未知";
		break;
	}

	return str;
}

/*
* 描  述：获取北京水表-设备类型名
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * WaterBeiJing_GetStrDeviceType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x41:	str = "三川无磁";	break;
	case 0x44:	str = "宁波无磁";	break;
	case 0x35:	str = "山科无磁";	break;
	case 0x2A:	str = "东海无磁";	break;
	case 0x2B:	str = "京源无磁";	break;
	default:
		str = "未知";
		break;
	}

	return str;
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
* 描  述：获取6009水表 计量传感器类型
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * Water6009_GetStrSensorType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x00:	str = "单干簧管/霍尔";	break;
	case 0x01:	str = "双干簧管/霍尔";	break;
	case 0x02:	str = "三干簧管/霍尔";	break;
	case 0x03:	str = "骏普4位光电直读";	break;
	case 0x04:	str = "厚膜直读";	break;
	case 0x05:	str = "骏普1位光电直读";	break;
	case 0x06:	str = "188协议光电直读";	break;
	case 0x07:	str = "188协议无磁直读";	break;
	case 0x08:	str = "2霍尔竟达";	break;
	case 0x09:	str = "宁波无磁";	break;
	case 0x0A:	str = "山科无磁";	break;
	case 0x0B:	str = "东海无磁";	break;
	case 0x0C:	str = "三川无磁";	break;
	default:	str = "未知";	break;
	}

	return str;
}

/*
* 描  述：解析6009水表-告警状态字
* 参  数：status	- 状态字
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water6009_GetStrAlarmStatus(uint16 status, char *buf)
{
	char * str = NULL;
	uint8 mask = 1, i;
	uint16 len = 0;

	for(i = 0; i < 14; i++){

		mask = (1 << i);
		
		switch(status & mask){
		case 0x01:	str = "干簧管故障";	break;
		case 0x02:	str = "阀到位故障";	break;
		case 0x04:	str = "传感器线断开";	break;
		case 0x08:	str = "电池欠压";	break;
		case 0x10:	str = "光电表,一组光管坏";	break;
		case 0x20:	str = "磁干扰标志";	break;
		case 0x40:	str = "光电表,多组光管坏";	break;
		case 0x80:	str = "光电表,正强光干扰";	break;
		case 0x0100:	str = "水表反转";	break;
		case 0x0200:	str = "水表被拆卸";	break;
		case 0x0400:	str = "水表被垂直安装";	break;
		case 0x0800:	str = "EEPROM异常";	break;
		case 0x1000:	str = "煤气泄漏";	break;
		case 0x2000:	str = "欠费标志";	break;
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
	case 0xD3:
	    str = "网络未注册";
		break;
	case 0xD4:
	    str = "网络用户码错误";
		break;
	case 0xD5:
	    str = "校验码错误";
		break;
	case 0xD6:
	    str = "数据禁止访问";
		break;
	case 0xD7:
	default:
		str = "未知错误";
		break;
	}

	return str;
}

/*
* 描  述：获取6009水表 阀控失败原因
* 参  数：errorCode	- 错误码
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water6009_GetStrValveCtrlFailed(uint16 errorCode, char * buf)
{
	char * str = NULL;
	uint16 mask = 1, i;
	uint16 len = 0;

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
			len += sprintf(&buf[len], "  %s\n", str);
			str = NULL;
		}
	}

	if(len == 0){
		len += sprintf(&buf[len], "  \n");
	}

	return len;
}

/*
* 描  述：获取6009水表 功能使能状态
* 参  数：stateCode	- 使能状态码
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water6009_GetStrMeterFuncEnableState(uint16 stateCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], "磁干扰关阀功能  :%s\n", ((stateCode & 0x0001) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "上报数据加密    :%s\n", ((stateCode & 0x0002) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "防拆卸检测功能  :%s\n", ((stateCode & 0x0004) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "垂直安装检测    :%s\n", ((stateCode & 0x0008) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "主动告警        :%s\n", ((stateCode & 0x0010) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "主动上传冻结数据:%s\n", ((stateCode & 0x0020) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "透支关阀功能    :%s\n", ((stateCode & 0x0040) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "预付费功能      :%s\n", ((stateCode & 0x0080) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "自动信道分配    :%s\n", ((stateCode & 0x0100) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "防锈功能        :%s\n", ((stateCode & 0x0200) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "掉电关阀功能    :%s\n", ((stateCode & 0x0400) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "RF休眠策略      :%s\n", ((stateCode & 0x0800) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "离线自动关阀    :%s\n", ((stateCode & 0x1000) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "煤气泄漏检测    :%s\n", ((stateCode & 0x2000) > 0 ? "开" : " 关"));

	len += sprintf(&buf[len], "流速控制功能    :%s\n", ((stateCode & 0x8000) > 0 ? "开" : " 关"));

	return len;
}

/*
* 描  述：获取6009水表 模块测试状态
* 参  数：statusCode	- 测试状态码
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water6009_GetStrTestStatus(uint16 statusCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], " 休眠电流测试  : %s\n", ((statusCode & 0x0001) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " 频率测试      : %s\n", ((statusCode & 0x0002) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " 功率测试      : %s\n", ((statusCode & 0x0004) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " 发射功率测试  : %s\n", ((statusCode & 0x0008) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " 接收灵敏度测试: %s\n", ((statusCode & 0x0010) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " 接收电流测试  : %s\n", ((statusCode & 0x0020) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " 阀控电路测试  : %s\n", ((statusCode & 0x0040) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " 计量电路测试  : %s\n", ((statusCode & 0x0080) > 0 ? "OK" : " NG"));
	len += sprintf(&buf[len], " LCD测试       : %s\n", ((statusCode & 0x0100) > 0 ? "OK" : " NG"));

	return len;
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
uint8 PackWater6009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt)
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
	buf[index++] = 0xFE;		// 设备类型: FE - 手持机， FA - 上位机， 10 - 冷水表， 11 - GPRS水表
	buf[index++] = 0x0F;		// 生命周期
	buf[index++] = addrs->itemCnt & 0x0F;	// 路径信息:  当前位置|路径长度
	// 地址域
	for(i = 0; i < (addrs->itemCnt & 0x0F); i++){
		memcpy(&buf[index], addrs->items[i], AddrLen);
		index += AddrLen;
	}

	// 数据域
	memcpy(&buf[index], args->items[args->itemCnt -1], args->lastItemLen);
	index += args->lastItemLen;

	if(cmd >= 0x3A && cmd <= 0x3F){
		buf[index++] = 0x55;		// 下行场强
		buf[index++] = 0xAA;		// 上行场强
		buf[7] = 0xFA;				// 设备类型改为 FA - 上位机
	}else{
		buf[index++] = 0x00;		// 下行场强
		buf[index++] = 0x00;		// 上行场强
	}
	
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
		buf[index++] = 0x00;	// 导言长度标识
		buf[index++] = 0x0F;	// 表端Boot时 发送信道
		buf[index++] = 0x1F;	// 表端Boot时 接收信道
	}else{
		buf[index++] = 0x00;	// 导言长度标识 00
		buf[index++] = 0x19;	// 集中器 发送信道 19
		buf[index++] = 0x19;	// 集中器 接收信道 19
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
*		  dispBuf 	- 解析的显示数据
* 返回值：uint8 解析结果：0 - 成功 , 1 - 失败 ， 2 - CRC错误， 3 - 超时无应答
*/
uint8 ExplainWater6009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf)
{
	bool ret = RxResult_Failed;
	uint8 crc8, addrsCnt, cmd, i, u8Tmp;
	uint16 index = 0, dispIdx, length, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	char *ptr;

	dispIdx = 0;

	// 显示表号 或 集中器号
	if(cmdId < 0x1010){
		#if (AddrLen == 6)
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: \n   %s\n", StrDstAddr);
		#endif
	}else{
		dispIdx += sprintf(&dispBuf[dispIdx], "集中器号: \n   %s\n", StrDstAddr);
	}

	// 缓冲区多包中查找
	while(1){

		if(rxlen < index + 27){
			sprintf(&dispBuf[dispIdx], "结果: 超时,无应答");
			return RxResult_Timeout;
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
		if(memcmp(&buf[index + 8 + (addrsCnt - 1) * AddrLen], dstAddr, AddrLen) != 0){
			index += length;
			continue;
		}

		// crc8 check
		crc8 = GetCrc8(&buf[index], length - 2);
		if(crc8 !=  buf[index + length - 2]){
			sprintf(&dispBuf[dispIdx], "结果: 有应答,CRC错误");
			return RxResult_CrcError;
		}

		// pass
		break;
	}

	startIdx = index;

	// 命令字
	cmd = buf[index + 4];

	// 跳过 长度 --> 路径信息
	index += 8;

	// 地址域
	if(memcmp(&buf[index], DstAddr, AddrLen) != 0){

		// 广播命令时，将应答的表地址作为新的目的地址
		GetStringHexFromBytes(StrDstAddr, buf, index, AddrLen, 0, false);

		dispIdx = 0;
		#if (AddrLen == 6)
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: \n   %s\n", StrDstAddr);
		#endif
	}
	index += addrsCnt * AddrLen;

	// 集中器转发的命令头部
	if(cmd == 0x4D){
		// 命令字
		cmd = buf[index];
		index += 1;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, AddrLen, 0, false);
		TmpBuf[AddrLen] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += AddrLen;
		// 转发结果
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
	}

	// 数据域解析
	payloadIdx = index;
	switch(cmdId){
	
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	表端命令 0x01 ~ 0x25 , 70 ~ 74		--------------------
	//-------------------------------------------------------------------------------------------------
	//----------------------------------------		读取用户用量		-------------
	case WaterCmd_ReadRealTimeData:	// 读取用户用量
	case CenterCmd_ReadRealTimeData:
		if(rxlen < index + 21 && cmd != 0x01){
			break;
		}
		ret = RxResult_Ok;
		
		// 类型
		ptr = Water6009_GetStrValueType((buf[index] >> 4));
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", ptr);
		index += 1;
		// 正转用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "正转: %d.%03d\n", u32Tmp, u16Tmp);
		#ifdef Project_6009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.meterValue, "%d.%03d", u32Tmp, u16Tmp);
			}
		#endif
		// 反转用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "反转: %d.%03d\n", u32Tmp, u16Tmp);
		//告警状态字
		#ifdef Project_6009_RF
			u32Tmp = dispIdx + 6;
		#endif
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		#ifdef Project_6009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				strncpy(MeterInfo.meterStatusStr, &dispBuf[u32Tmp], dispIdx - u32Tmp - 1);
				u32Tmp = ( (dispIdx - u32Tmp - 1) >= Size_MeterStatusStr ? Size_MeterStatusStr - 1 : (dispIdx - u32Tmp - 1));
				MeterInfo.meterStatusStr[u32Tmp] = 0x00;
			}
		#endif
		//阀门状态 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门: %s  ", ptr);
		index += 1;
		#ifdef Project_6009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.meterStatusHex, "%02X%02X%02X", buf[index - 3], buf[index - 2], buf[index - 1]);
				sprintf(&MeterInfo.meterStatusStr[u32Tmp], " , 阀门%s", ptr);
			}
		#endif
		//电池电压
		dispIdx += sprintf(&dispBuf[dispIdx], "电池: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		#ifdef Project_6009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				strncpy(MeterInfo.batteryVoltage, &dispBuf[dispIdx - 4], 3);
			}
		#endif
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "温度: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR 噪音比
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx信道
		dispIdx += sprintf(&dispBuf[dispIdx], "信道: Tx-%d, Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//协议版本
		dispIdx += sprintf(&dispBuf[dispIdx], "协议版本: %d\n", buf[index]);
		index += 1;
		break;

	//----------------------------------------		读取冻结数据	---------------------
	case WaterCmd_ReadFrozenData:	// 读取冻结数据
		if(rxlen < index + 88 && cmd != 0x02){
			break;
		}
		ret = RxResult_Ok;
		// 冻结数据类型
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", (buf[index] == 0x01 ? "正传" : "反转"));
		index += 1;

		if(rxlen < index + 104){	// 冻结数据格式-旧版本 1 + 78 byte
			// 冻结数据起始序号
			u8Tmp = buf[index] * 10;
			dispIdx += sprintf(&dispBuf[dispIdx], "范围: 第 %d~%d 条\n", u8Tmp, u8Tmp + 9);
			index += 1;
			// 冻结数据起始时间
			dispIdx += sprintf(&dispBuf[dispIdx], "时间: %X%x%x%x %x:00:00\n"
				, buf[payloadIdx + 2], buf[payloadIdx + 3], buf[payloadIdx + 4], buf[payloadIdx + 5], buf[payloadIdx + 6]);
			index += 5;
			// 冻结数据方式 ：0-按天, 1-按月
			// 冻结数据数量 ：按天最大24条，按月最大30条
			dispIdx += sprintf(&dispBuf[dispIdx], "方式: 每%s冻结%d条\n", (buf[index] == 0x01 ? "天" : "月"), buf[index + 1]);
			index += 2;	
			// 冻结数据时间间隔
			if(buf[index] == 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "间隔: 每%s冻结1条\n", (buf[index - 2] == 0x01 ? "天" : "月"));
			}
			else{
				dispIdx += sprintf(&dispBuf[dispIdx], "间隔: %d%s冻结1条\n", buf[index], (buf[index - 2] == 0x01 ? "小时" : "天"));
			}
			index += 1;
			// 冻结的用量数据：7*N 字节 （6 byte 用量 + 1 byte date.day）
			dispIdx += sprintf(&dispBuf[dispIdx], "读取的10条数据如下: \n");
			for(i = 0; i < 10; i++){
				u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
				index += 4;
				u16Tmp = ((buf[index + 1] << 8) + buf[index]);
				index += 2;
				dispIdx += sprintf(&dispBuf[dispIdx], "%d, %x/%x: %d.%03d\n", i, buf[payloadIdx + 4], buf[index], u32Tmp, u16Tmp);
				index +=1;
			}
		}
		else{		// 冻结数据格式-新版本	1 + 104 byte
			// 冻结数据起始序号
			dispIdx += sprintf(&dispBuf[dispIdx], "范围: 倒数第%d天数据\n", buf[index] + 1);
			index += 1;
			// 时间信息
			dispIdx += sprintf(&dispBuf[dispIdx], "时间: %02X-%02X %02X:%02X\n",
				buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			index += 4;
			// 累计用量
			u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
			index += 4;
			u16Tmp = ((buf[index + 1] << 8) + buf[index]);
			index += 2;
			dispIdx += sprintf(&dispBuf[dispIdx], "累计用量: %d.%03d\n", u32Tmp, u16Tmp);
			// 0:00 ~ 23:30 增量
			u8Tmp = 0;
			u16Tmp = 0x00;
			for(i = 0; i < 47; i++){
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X~", u8Tmp, u16Tmp);
				u16Tmp += 0x30;
				if(u16Tmp == 0x60){
					u16Tmp = 0x00;
					u8Tmp += 1;
				}
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X增量:%d\n", u8Tmp, u16Tmp, (buf[index] + buf[index + 1]*256));
				index += 2;
			}
		}

		//告警状态字
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		//阀门状态 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门: %s  ", ptr);
		index += 1;
		//电池电压
		dispIdx += sprintf(&dispBuf[dispIdx], "电池: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "温度: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR 噪音比
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx信道
		dispIdx += sprintf(&dispBuf[dispIdx], "信道: Tx-%d, Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//协议版本
		dispIdx += sprintf(&dispBuf[dispIdx], "协议版本: %d\n", buf[index]);
		index += 1;
		break;

	//---------------------------------------		开关阀门	---------------------
	case WaterCmd_OpenValve:		// 开阀
	case WaterCmd_OpenValveForce:	// 强制开阀
	case WaterCmd_CloseValve:		// 关阀
	case WaterCmd_CloseValveForce:	// 强制关阀
	case CenterCmd_OpenValve:
	case CenterCmd_CloseValve:
		if(rxlen < index + 3 && cmd != 0x03){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAB){
			u16Tmp = (buf[index] + buf[index + 1] * 256);
			dispIdx += sprintf(&dispBuf[dispIdx], "原因: ");
			dispIdx += Water6009_GetStrValveCtrlFailed(u16Tmp, &dispBuf[dispIdx]);
			index += 2;
		}
		break;

	//----------------------------------------		读取表端配置信息		-------------
	case WaterCmd_ReadMeterCfgInfo:	// 读取表端配置信息

		if(rxlen < index + 124 && cmd != 0x04){
			break;
		}
		ret = RxResult_Ok;
		index += 84;
		memcpy(&VerInfo[0], &buf[index], 40);
		VerInfo[40] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "版本: %s\n", &VerInfo[0]);
		index += 40;
		break;

	//---------------------------------------		清异常命令		---------------------
	case WaterCmd_ClearException:	// 清异常命令 
	case CenterCmd_ClearException:
		if(rxlen < index + 1 && cmd != 0x05){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//---------------------------------------		测试命令	---------------------
	case WaterCmd_RebootDevice:	// 表端重启
		if(rxlen < index + 2 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadTemperature:	// 读表温度
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "温度: %s%d\n", ptr, (buf[index] & 0x7F));
        index += 1;
		break;

	case WaterCmd_ReadVoltage:	// 读表电压
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		//电池电压
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		break;

	case WaterCmd_ClearPrepaidRefVal:	// 清预缴参考量
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_SetOverCurrentTimeout:	// 设置过流超时
		if(rxlen < index + 3 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		// 命令状态
		if(buf[index] != Args.buf[2] || buf[index + 1] != Args.buf[3] || buf[index + 2] != Args.buf[4]){
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
			index += 1;
			ret = RxResult_Failed;
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
			// 过流电流
			dispIdx += sprintf(&dispBuf[dispIdx], "过流电流: %d mA\n", buf[index]);
			index += 1;
			// 超时时间
			dispIdx += sprintf(&dispBuf[dispIdx], "超时时间: %d ms\n", (buf[index] + buf[index + 1] * 256));
			index += 2;
		}
		break;

	case WaterCmd_ReadOperatorNumber:	// 读运营商编号
		if(rxlen < index + 4 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		// 运营商编号
		dispIdx += sprintf(&dispBuf[dispIdx], "运营商编号: %2X%2X%2X%2X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		break;

	case WaterCmd_ReadReportRoute:	// 读上报路径
		if(rxlen < index + 63 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;

		// 路径1长度 6*n
		u8Tmp = buf[index] / 6;
		dispIdx += sprintf(&dispBuf[dispIdx], "路径1级数: %d\n", u8Tmp);
		index += 1;
		// 路径1地址列表
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		index += (30 - u8Tmp * 6);

		// 路径2长度 6*n
		u8Tmp = buf[index] / 6;
		dispIdx += sprintf(&dispBuf[dispIdx], "  \n路径2级数: %d\n", u8Tmp);
		index += 1;
		// 路径2地址列表
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		index += (30 - u8Tmp * 6);

		break;

	case WaterCmd_SetMeterNumber:	// 设置表号
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "新表号: %s\n", &TmpBuf[0]);
		}
		else{
			ret = RxResult_Failed;
		}
		index += 6;
		break;

	case WaterCmd_ReadDebugInfo:		// 读debug信息
		if(rxlen < index + 58 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		// debug信息
		dispIdx += sprintf(&dispBuf[dispIdx], "模块发射次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "模块接收次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "开/关阀次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "磁干扰次数 : %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "EEPROM写入次数: %d\n",
			 (buf[index] + (buf[index + 1] << 8) + (buf[index + 2] << 16) + (buf[index + 3] << 24)));
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "48小时复位次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "进入初始化次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "完成初始化次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "看门狗复位次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "CDA超100ms次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "接收超时次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "A干簧管闭合次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "B干簧管闭合次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "ID校验错误: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "收到非法命令字: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "申请定时器失败: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " 命令:%02X %02X %02X %02X %02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4]);
		index += 5;
		dispIdx += sprintf(&dispBuf[dispIdx], "申请任务失败: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " 命令:%02X %02X %02X %02X %02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4]);
		index += 5;
		dispIdx += sprintf(&dispBuf[dispIdx], "申请内存失败: %d\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " 命令:%02X %02X %02X %02X %02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4]);
		index += 5;
		for(i = 0; i < 10; i++){
			dispIdx += sprintf(&dispBuf[dispIdx], "零时数据%c: %d\n", ('A' + i), buf[index]);
			index += 1;
		}
		break;

	case WaterCmd_ClearDebugInfo:		// 清debug信息
		if(rxlen < index + 1 && cmd != 0x07){
			break;
		}
		ret = RxResult_Ok;
		index += 1;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		预缴用量、参考用量-读取/设置	---------------------
	case WaterCmd_ReadPrepaidRefVal:	// 读预缴参考用量
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = RxResult_Ok;
		// 预缴用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "预缴用量: %d.%03d\n", u32Tmp, u16Tmp);
		// 参考用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "参考用量: %d.%03d\n", u32Tmp, u16Tmp);
		break;

	case WaterCmd_SetPrepaidRefVal:		// 设预缴参考用量
		if(rxlen < index + 2 && cmd != 0x16){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "数据非法");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 数据非法原因
		if(buf[index - 1] == 0xAE){
			if(buf[index] & 0x01 > 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "-->参考起始用量不合法\n");
			}
			if(buf[index] & 0x02 > 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "-->设置的预缴用量未达到开阀门限\n");
			}
			index += 1;
		}
		break;

	case WaterCmd_ReadAlarmLimitOverdraft:		// 读报警关阀限值
		if(rxlen < index + 1 && cmd != 0x17){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = (buf[index] == 0xAB ? "操作失败" : "操作成功");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 操作成功时结果
		if(buf[index - 1] == 0xAA){
			dispIdx += sprintf(&dispBuf[dispIdx], "报警限值: %d\n", buf[index]);
			index += 1;
			u16Tmp = ((uint16)(buf[index + 1] << 8) + buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "关阀限值: %s%d\n", 
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
		ret = RxResult_Ok;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 新版可能+ 2 byte
		break;

	//--------------------------------------		工作参数	---------------------
	case WaterCmd_SetBaseValPulseRatio:	// 设表底数脉冲系数
		if(rxlen < index + 7 && cmd != 0x06){
			break;
		}
		ret = RxResult_Ok;
		// 用户用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "用户用量:%d.%03d\n", u32Tmp, u16Tmp);
		// 脉冲系数
		switch (buf[index]){
		case 0x00:	u16Tmp = 1;	break;
		case 0x01:	u16Tmp = 10;	break;
		case 0x02:	u16Tmp = 100;	break;
		case 0x03:	u16Tmp = 1000;	break;
		default:  u16Tmp = buf[index];	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲系数:%d脉冲/方\n", u16Tmp);
		index += 1;
		break;

	case WaterCmd_ClearReverseMeasureData:	// 清除反转计量数据
		if(rxlen < index + 6 && cmd != 0x0A){
			break;
		}
		ret = RxResult_Ok;
		// 反转读数
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "反转读数:%d.%03d\n", u32Tmp, u16Tmp);
		break;

	case WaterCmd_ReadFuncEnableState:	// 读取功能使能状态
	case CenterCmd_ReadEnableState:
		if(rxlen < index + 2 && cmd != 0x0B){
			break;
		}
		ret = RxResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "功能使能状态如下\n");
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += Water6009_GetStrMeterFuncEnableState(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	case WaterCmd_SetTimedUpload:		// 设置定时上传
	case WaterCmd_SetFixedValUpload:	// 设置定量上传
	case WaterCmd_SetTimedAndFixedValUpload:	// 设置定时定量上传
		if(rxlen < index + 2 && cmd != 0x0C){
			break;
		}
		ret = RxResult_Ok;
		if(buf[index] == 0xAA){
			dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
			index += 1;
		}
		if(rxlen >= index + 2 + 4){		// 新协议增加
			dispIdx += sprintf(&dispBuf[dispIdx], "定时上传间隔:%d 小时\n", buf[index]);
			index += 1;
			dispIdx += sprintf(&dispBuf[dispIdx], "定量上传间隔:%d m3\n", buf[index]);
			index += 1;
		}
		break;

	case WaterCmd_ReadMeterTime:	// 读表端时钟
		if(rxlen < index + 7 && cmd != 0x13){
			break;
		}
		ret = RxResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "表端时间: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		break;

	case WaterCmd_SetMeterTime:		// 校表端时钟
		if(rxlen < index + 1 && cmd != 0x14){
			break;
		}
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_SetIpPortMode:		// 设IP+端口+模式
		if(rxlen < index + 2 && cmd != 0x0D){
			break;
		}
		// 命令选项跳过
		index += 1;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadIpPortMode:		// 读IP+端口+模式
		if(rxlen < index + 9 && cmd != 0x0D){
			break;
		}
		// 命令选项跳过
		index += 1;
		// 工作模式
		switch (buf[index]){
		case 0xA0:	ptr = "Coap"; break;
		case 0xA1:	ptr = "Udp"; break;
		default: ptr = "未知"; break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "工作模式: %s\n", ptr);
		index += 1;
		// Ip地址
		dispIdx += sprintf(&dispBuf[dispIdx], "Ip地址: %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		// 端口号
		dispIdx += sprintf(&dispBuf[dispIdx], "端口号: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadNbOperaterNumber: // 读NB运营商编号
		if(rxlen < index + 7 && cmd != 0x0E){
			break;
		}
		// 命令选项跳过
		index += 1;
		// 运营商编号/SIM卡类型
		GetStringHexFromBytes(TmpBuf, buf, index, 4, 0, false);
		if(TmpBuf[3] == '0' && TmpBuf[4] == '4'){
			ptr = "中国移到";
		}else if(TmpBuf[3] == '0' && TmpBuf[4] == '1'){
			ptr = "中国联通";
		}else if(TmpBuf[3] == '1' && TmpBuf[4] == '1'){
			ptr = "中国电信";
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "运营商编号: %s\n", TmpBuf);
		dispIdx += sprintf(&dispBuf[dispIdx], " SIM卡类型: %s\n", ptr);
		index += 4;
		// 通信频段
		dispIdx += sprintf(&dispBuf[dispIdx], "  通信频段: %d\n", buf[index]);
		index += 1;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadImeiAndCcid:		// 读IMEI+ICCID
		if(rxlen < index + 22 && cmd != 0x0F){
			break;
		}
		// 命令选项跳过
		index += 1;
		// ICCID
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 10, 0, false);
		TmpBuf[20] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "ICCID: %s\n", &TmpBuf[0]);
		index += 10;
		// IMEI
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 10, 0, false);
		TmpBuf[20] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], " IMEI: %s\n", &TmpBuf[0]);
		index += 10;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case WaterCmd_ReadNbJoinNetworkInfo:		// 读取NB入网信息
		if(rxlen < index + 34 && cmd != 0x10){
			break;
		}
		ret = RxResult_Ok;
		// 电池电压
		dispIdx += sprintf(&dispBuf[dispIdx], "Battery: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		// 信号功率
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "Signal power: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// 总功率
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], " Total power: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// 发射功率
		dispIdx += sprintf(&dispBuf[dispIdx], "TX power: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// 发射时间 ms
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "TX time: %d\n", u32Tmp);
		index += 4;
		// 接收时间 ms
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "RX time: %d\n", u32Tmp);
		index += 4;
		// 当前Cell ID
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "Cell ID: %d\n", ((buf[index + 4] << 24) * 256 + u32Tmp));
		index += 5;
		// ECL
		dispIdx += sprintf(&dispBuf[dispIdx], "    ECL: %d\n", buf[index]);
		index += 1;
		// 信噪比
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "    SNR: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// 搜索频率信道号
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], " EARFUN: %d\n", ((buf[index + 4] << 24) * 256 + u32Tmp));
		index += 5;
		// 物理Cell ID
		dispIdx += sprintf(&dispBuf[dispIdx], "    PCI: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// 参考信号接收质量 
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		ptr = ((u16Tmp & 0x8000) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "   RSRQ: %s%d\n", ptr, (u16Tmp & 0x7FFF));
		index += 2;
		// 操作模式
		dispIdx += sprintf(&dispBuf[dispIdx], "Operator Mode: %d\n", buf[index]);
		index += 1;
		// 注册状态
		dispIdx += sprintf(&dispBuf[dispIdx], "RegisterState: %s\n", (buf[index] == 0 ? "失败" : "成功"));
		index += 1;
		break;

	case WaterCmd_ReadBeiJingWaterMeterParams:		// 读取北京水表参数
		if(rxlen < index + 69 && cmd != 0x26){
			break;
		}
		ret = RxResult_Ok;
		// 命令选项跳过
		index += 1;
		// CCID
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 10, 0, false);
		//TmpBuf[20] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "CCID号: %s\n", &TmpBuf[0]);
		index += 10;
		// 设备类型
		ptr = WaterBeiJing_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "设备类型: %s\n", ptr);
		index += 1;
		// 出厂表号
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 7, 0, false);
		TmpBuf[14] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "出厂表号: \n  %s\n", &TmpBuf[0]);
		index += 7;
		// 终端时钟 
		dispIdx += sprintf(&dispBuf[dispIdx], "终端时钟:\n 20%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], 
			buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		// 软件版本
		dispIdx += sprintf(&dispBuf[dispIdx], "软件版本: %X\n", buf[index]);
		index += 1;
		// 过流告警阈值 L/h
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "过流告警阈值:%d L/h\n", u32Tmp);
		index += 4;
		// 过流告警时间 1分钟
		dispIdx += sprintf(&dispBuf[dispIdx], "过流告警时间:%d分钟\n", buf[index]);
		index += 1;
		// 反流告警阈值 L/h
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "反流告警阈值:%d L/h\n", u32Tmp);
		index += 4;
		// 反流告警时间 1分钟
		dispIdx += sprintf(&dispBuf[dispIdx], "反流告警时间:%d分钟\n", buf[index]);
		index += 1;
		// 电压告警阀值 0.01V
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "电压告警阀值:%d x0.01V\n", u16Tmp);
		index += 2;
		// IP地址
		dispIdx += sprintf(&dispBuf[dispIdx], "服务器-IP地址:\n  %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		// 端口号
		dispIdx += sprintf(&dispBuf[dispIdx], "服务器-端口号:%d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// APN信息
		memcpy(&TmpBuf[0], &buf[index], 6);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "APN信息: %s\n", &TmpBuf[0]);
		index += 6;
		// 上报重连次数
		dispIdx += sprintf(&dispBuf[dispIdx], "上报重连次数:%d\n", buf[index]);
		index += 1;
		// 周期上报起始时间 10进制
		dispIdx += sprintf(&dispBuf[dispIdx], "周期上报起始时间:\n 20%02d-%02d-%02d %02d:%02d:%02d\n", 
			buf[index], buf[index + 1], buf[index + 2], 
			buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		// 周期上报结束时间 10进制
		dispIdx += sprintf(&dispBuf[dispIdx], "周期上报结束时间:\n 20%02d-%02d-%02d %02d:%02d:%02d\n", 
			buf[index], buf[index + 1], buf[index + 2], 
			buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		// 周期上报估计时长
		dispIdx += sprintf(&dispBuf[dispIdx], "周期上报估长:%d秒\n", buf[index]);
		index += 1;
		// 终端启停设置
		dispIdx += sprintf(&dispBuf[dispIdx], "终端启停设置:%s\n", (buf[index] == 0 ? "停用" : "启用"));
		index += 1;
		// 周期上报频率
		dispIdx += sprintf(&dispBuf[dispIdx], "周期上报频率:%d小时\n", buf[index]);
		index += 1;
		// 密集上报起始时间点
		dispIdx += sprintf(&dispBuf[dispIdx], "密集上报起始时间点:\n  %d 点\n", buf[index]);
		index += 1;
		// 周期采样间隔
		dispIdx += sprintf(&dispBuf[dispIdx], "周期采样间隔:%d x30分钟\n", buf[index]);
		index += 1;
		// 上报重连等待时间
		dispIdx += sprintf(&dispBuf[dispIdx], "重连等待时间:%d分钟\n", buf[index]);
		index += 1;
		// 密集采样间隔
		dispIdx += sprintf(&dispBuf[dispIdx], "密集采样间隔:%d分钟\n", buf[index]);
		index += 1;
		// KEY
		GetStringHexFromBytes(&TmpBuf[0], buf, index, 16, 0, false);
		dispIdx += sprintf(&dispBuf[dispIdx], "KEY: %s\n", &TmpBuf[0]);
		index += 16;
		break;

	case WaterCmd_SetBeiJingWaterMeterParams:		// 设置北京水表参数
		if(rxlen < index + 2 && cmd != 0x26){
			break;
		}
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		其他操作		---------------------
	case WaterCmd_ReadRxTxMgnDistbCnt:		// 读收发磁扰阀控数
		if(rxlen < index + 7 && cmd != 0x09){
			break;
		}
		ret = RxResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "发射次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "接收次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "开关阀次数: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "磁干扰次数: %d\n", (buf[index]));
		index += 1;
		break;

	case WaterCmd_ReadRxdAndTxdChanel:	// 读取RXD和TXD信道
		if(rxlen < index + 2 && cmd != 0x1B){
			break;
		}
		ret = RxResult_Ok;
		if(buf[index] == 0xAB){
			ret = RxResult_Failed;
			dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作失败\n");
			index += 1;
			if(rxlen >= index + 1 + 4){
				index += 1;		// 失败原因
			}
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表信道: %d\n", (buf[index] & 0x7F));
			index += 1;
			dispIdx += sprintf(&dispBuf[dispIdx], "上报信道: %d\n", (buf[index] & 0x7F));
			index += 1;
		}
		break;

	case WaterCmd_SetRxdAndTxdChanel:	// 设置RXD和TXD信道
		if(rxlen < index + 2 && cmd != 0x1B){
			break;
		}
		ret = RxResult_Ok;
		if(buf[index] == 0xAB){
			ret = RxResult_Failed;
			dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作失败\n");
			index += 1;
			if(rxlen >= index + 1 + 4){
				index += 1;		// 失败原因
			}
		}
		else{
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表信道: %d\n", (buf[index]));
			index += 1;
			dispIdx += sprintf(&dispBuf[dispIdx], "上报信道: %d\n", (buf[index]));
			index += 1;
		}
		break;

	case WaterCmd_SetOperatorNumber:		// 设置运营商编号
		if(rxlen < index + 1 && cmd != 0x21){
			break;
		}
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(rxlen >= index + 1 + 4){
			index += 1;		// 失败原因
		}
		break;

	case WaterCmd_SetDefinedRoute:	// 路径下发
		if(rxlen < index + 1 && cmd != 0x22){
			break;
		}
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(rxlen >= index + 1 + 4){
			index += 1;		// 失败原因
		}
		break;


	//--------------------------------------		程序升级		---------------------
	case WaterCmd_NoticeUpgrade_OnApp:		// 通知系统升级_在app
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = RxResult_Ok;

		break;

	case WaterCmd_NoticeUpgrade_OnBoot:		// 通知系统升级_在boot
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = RxResult_Ok;
		
		break;

	case WaterCmd_SendUpgradePacket:			// 发送升级数据
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = RxResult_Ok;
		
		break;

	case WaterCmd_QueryUpgradeStatus_OnBoot:	// 查询升级状态_在boot
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = RxResult_Ok;
		
		break;

	case WaterCmd_QueryUpgradeStatus_OnApp:		// 查询升级状态_在app
		if(rxlen < index + 12 && cmd != 0x15){
			break;
		}
		ret = RxResult_Ok;
		
		break;


	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	集中器命令 0x40 ~ 0x65 , F1 ~ F3	--------------------
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		常用操作		-------------------------------------
	case CenterCmd_ReadCenterNo:	// 读集中器号
		if(rxlen < index + 6 && cmd != 0x41){
			break;
		}
		ret = RxResult_Ok;
		// 集中器号
		dispIdx += sprintf(&dispBuf[dispIdx], "集中器号: \n    %02X%02X%02X%02X%02X%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		break;

	case CenterCmd_ReadCenterVer:		// 读集中器版本
		if(rxlen < index + 6 && cmd != 0x40){
			break;
		}
		ret = RxResult_Ok;
		// 软件版本
		dispIdx += sprintf(&dispBuf[dispIdx], "软件版本: %02X%02X \n", buf[index], buf[index + 1]);
		index += 2;
		// 硬件版本
		dispIdx += sprintf(&dispBuf[dispIdx], "硬件版本: %02X%02X \n", buf[index], buf[index + 1]);
		index += 2;
		// 协议版本
		dispIdx += sprintf(&dispBuf[dispIdx], "协议版本: %02X%02X \n", buf[index], buf[index + 1]);
		index += 2;
		break;

	case CenterCmd_ReadCenterTime:		// 读集中器时钟
		if(rxlen < index + 7 && cmd != 0x43){
			break;
		}
		ret = RxResult_Ok;
		// 集中器时钟
		dispIdx += sprintf(&dispBuf[dispIdx], "集中器时钟: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		break;

	case CenterCmd_SetCenterTime:		// 设集中器时钟
		if(rxlen < index + 1 && cmd != 0x44){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadGprsParam:		// 读GPRS参数
		if(rxlen < index + 16 && cmd != 0x45){
			break;
		}
		ret = RxResult_Ok;
		// 首先IP + 端口
		dispIdx += sprintf(&dispBuf[dispIdx], "首先IP: %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "  端口: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// 备用IP + 端口
		dispIdx += sprintf(&dispBuf[dispIdx], "备用IP: %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "  端口: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// 心跳包间隔
		dispIdx += sprintf(&dispBuf[dispIdx], "心跳包间隔: %d s\n", (buf[index] * 10));
		// A P N
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "A P N: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		// 用户名
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "用户名: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		// 密  码
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "密  码: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		break;

	case CenterCmd_SetGprsParam:		// 设GPRS参数
		if(rxlen < index + 1 && cmd != 0x46){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadGprsSignal:		// 读GPRS信号强度
		if(rxlen < index + 4 && cmd != 0x47){
			break;
		}
		ret = RxResult_Ok;
		// 信号强度
		if(buf[index] == 0){
			ptr = "<= -113dBm";
		}
		else if(buf[index] < 31){
			sprintf(&TmpBuf[0], "-%ddBm", 113 - 2 * buf[index]);
			ptr = &TmpBuf[0];
		}
		else if(buf[index] == 31){
			ptr = ">= -53dBm";
		}
		else if(buf[index] == 99){
			ptr = "未知";
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "信号强度: %s\n", ptr);
		index += 1;
		// 联机状态
		ptr = (buf[index] == 0 ? "离线" : "在线");
		dispIdx += sprintf(&dispBuf[dispIdx], "联机状态: %s\n", ptr);
		index += 1;
		// IMSI
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "IMSI: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		// GMM
		u8Tmp = buf[index];
		memcpy(&TmpBuf[0], &buf[index + 1], u8Tmp);
		TmpBuf[u8Tmp] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "GMM: %s\n", &TmpBuf[0]);
		index += (u8Tmp + 1);
		break;

	case CenterCmd_InitCenter:		// 集中器初始化
		if(rxlen < index + 2 && cmd != 0x48){
			break;
		}
		ret = RxResult_Ok;
		// 数据上传功能
		ptr = (buf[index] == 0 ? "清空档案和路径" : "清空所有数据");
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", ptr);
		index += 1;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadCenterWorkMode:		// 读集中器工作模式
		if(rxlen < index + 7 && cmd != 0x49){
			break;
		}
		ret = RxResult_Ok;
		// 工作类型
		ptr = (buf[index] == 0 ? "抄定时定量" : (buf[index] == 1 ? "抄冻结" : "无效"));
		dispIdx += sprintf(&dispBuf[dispIdx], "工作类型: %s\n", ptr);
		index += 1;
		// 数据补抄功能
		ptr = ((buf[index] & 0x80) > 0 ? "开" : "关");
		dispIdx += sprintf(&dispBuf[dispIdx], "数据补抄功能: %s\n", ptr);
		// 数据上传功能
		ptr = ((buf[index] & 0x40) > 0 ? "开" : "关");
		dispIdx += sprintf(&dispBuf[dispIdx], "数据上传功能: %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "数据上传时间: %2X点\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "数据补抄日期-位标记: \n");
		dispIdx += sprintf(&dispBuf[dispIdx], " 1- 7日 补抄标记: %02X\n", (buf[index] & 0x7F));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], " 8-15日 补抄标记: %02X\n", (buf[index]));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "16-23日 补抄标记: %02X\n", (buf[index]));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "24-31日 补抄标记: %02X\n", (buf[index]));
		index += 1;
		break;


	//--------------------------------------		档案操作：		-------------------------------------
	case CenterCmd_ReadDocCount:		// 读档案数量
		if(rxlen < index + 3 && cmd != 0x50){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = Water6009_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "设备类型: %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "档案数量: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		break;

	case CenterCmd_ReadDocInfo:			// 读档案信息
		if(rxlen < index + 3 && cmd != 0x51){
			break;
		}
		ret = RxResult_Ok;
		// 节点总数量
		dispIdx += sprintf(&dispBuf[dispIdx], "节点总数量  : %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		// 节点总数量
		dispIdx += sprintf(&dispBuf[dispIdx], "本次应答数量: %d\n", (buf[index]));
		index += 1;
		// N个节点信息
		u8Tmp = buf[index - 1];
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
			TmpBuf[6] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "节点 %2d:%s\n", i + 1, &TmpBuf[0]);
			index += 6;
			ptr = Water6009_GetStrDeviceType(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   类型:%s\n", ptr);
			index += 1;
			ptr = (buf[index] == 0 ? "失败" : (buf[index] == 1 ? "成功" : "未知"));
			dispIdx += sprintf(&dispBuf[dispIdx], "   抄表:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_AddDocInfo:			// 添加档案信息
		if(rxlen < index + 1 && cmd != 0x52){
			break;
		}
		ret = RxResult_Ok;
		// 设置的节点数量
		dispIdx += sprintf(&dispBuf[dispIdx], "设置的节点数量: %d\n", (buf[index]));
		index += 1;
		// N个节点设置结果
		u8Tmp = buf[index - 1];
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
			TmpBuf[6] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "节点 %2d:%s\n", i + 1, &TmpBuf[0]);
			index += 6;
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   结果:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_DeleteDocInfo:			// 删除档案信息
		if(rxlen < index + 7 && cmd != 0x53){
			break;
		}
		ret = RxResult_Ok;
		// N个节点设置结果
		u8Tmp = (rxlen - index - 4) / 7;
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
			TmpBuf[6] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "节点 %2d:%s\n", i + 1, &TmpBuf[0]);
			index += 6;
			ptr = Water6009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   结果:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_ModifyDocInfo:			// 修改档案信息
		if(rxlen < index + 1 && cmd != 0x54){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		路径设置：		-------------------------------------
	case CenterCmd_ReadDefinedRoute:		// 读自定义路由
		if(rxlen < index + 10 && cmd != 0x55){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 设备类型
		ptr = Water6009_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", ptr);
		index += 1;
		// 路径1中继数
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "路径1中继数: %d\n", u8Tmp);
		index += 1;
		// 路径1地址列表
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		// 路径2中继数
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "路径2中继数: %d\n", u8Tmp);
		index += 1;
		// 路径2地址列表
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, 6, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "  %d: %s\n", i + 1, &TmpBuf[0]);
			index += 6;
		}
		break;

	case CenterCmd_SetDefinedRoute:			// 设自定义路由
		if(rxlen < index + 7 && cmd != 0x56){
			break;
		}
		ret = RxResult_Ok;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	//--------------------------------------		命令转发：		-------------------------------------
	case CenterCmd_ReadFixedTimeData:			// 读定时定量数据
		if(rxlen < index + 28 && cmd != 0x63){
			break;
		}
		ret = RxResult_Ok;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 时间
		dispIdx += sprintf(&dispBuf[dispIdx], "时间: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		// 类型
		ptr = Water6009_GetStrValueType((buf[index] >> 4));
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", ptr);
		index += 1;
		// 正转用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "正转: %d.%03d\n", u32Tmp, u16Tmp);
		// 反转用量
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		index += 4;
		u16Tmp = ((buf[index + 1] << 8) + buf[index]);
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "反转: %d.%03d\n", u32Tmp, u16Tmp);
		//告警状态字
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		//阀门状态 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门: %s  ", ptr);
		index += 1;
		//电池电压
		dispIdx += sprintf(&dispBuf[dispIdx], "电池: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "温度: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR 噪音比
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx信道
		dispIdx += sprintf(&dispBuf[dispIdx], "信道: Tx-%d , Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//协议版本
		dispIdx += sprintf(&dispBuf[dispIdx], "协议版本: %d\n", buf[index]);
		index += 1;
		break;

	case CenterCmd_ReadFrozenData:			// 读冻结数据
		if(rxlen < index + 7 && cmd != 0x64){
			break;
		}
		ret = RxResult_Ok;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 命令状态
		ptr = Water6009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 冻结数据类型
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", (buf[index] == 0x01 ? "正传" : "反转"));
		index += 1;

		if(rxlen < index + 104){	// 冻结数据格式-旧版本 1 + 78 byte
			// 冻结数据起始序号
			u8Tmp = buf[index] * 10;
			dispIdx += sprintf(&dispBuf[dispIdx], "范围: 第 %d~%d 条\n", u8Tmp, u8Tmp + 9);
			index += 1;
			// 冻结数据起始时间
			dispIdx += sprintf(&dispBuf[dispIdx], "时间: %X%x%x%x %x:00:00\n"
				, buf[payloadIdx + 2], buf[payloadIdx + 3], buf[payloadIdx + 4], buf[payloadIdx + 5], buf[payloadIdx + 6]);
			index += 5;
			// 冻结数据方式 ：0-按天, 1-按月
			// 冻结数据数量 ：按天最大24条，按月最大30条
			dispIdx += sprintf(&dispBuf[dispIdx], "方式: 每%s冻结%d条\n", (buf[index] == 0x01 ? "天" : "月"), buf[index + 1]);
			index += 2;	
			// 冻结数据时间间隔
			if(buf[index] == 0){
				dispIdx += sprintf(&dispBuf[dispIdx], "间隔: 每%s冻结1条\n", (buf[index - 2] == 0x01 ? "天" : "月"));
			}
			else{
				dispIdx += sprintf(&dispBuf[dispIdx], "间隔: %d%s冻结1条\n", buf[index], (buf[index - 2] == 0x01 ? "小时" : "天"));
			}
			index += 1;
			// 冻结的用量数据：7*N 字节 （6 byte 用量 + 1 byte date.day）
			dispIdx += sprintf(&dispBuf[dispIdx], "读取的10条数据如下: \n");
			for(i = 0; i < 10; i++){
				u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
				index += 4;
				u16Tmp = ((buf[index + 1] << 8) + buf[index]);
				index += 2;
				dispIdx += sprintf(&dispBuf[dispIdx], "%d, %x/%x: %d.%03d\n", i, buf[payloadIdx + 4], buf[index], u32Tmp, u16Tmp);
				index +=1;
			}
		}
		else{		// 冻结数据格式-新版本	1 + 104 byte
			// 冻结数据起始序号
			dispIdx += sprintf(&dispBuf[dispIdx], "范围: 倒数第%d天数据\n", buf[index] + 1);
			index += 1;
			// 时间信息
			dispIdx += sprintf(&dispBuf[dispIdx], "时间: %02X-%02X %02X:%02X\n",
				buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			index += 4;
			// 累计用量
			u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
			index += 4;
			u16Tmp = ((buf[index + 1] << 8) + buf[index]);
			index += 2;
			dispIdx += sprintf(&dispBuf[dispIdx], "累计用量: %d.%03d\n", u32Tmp, u16Tmp);
			// 0:00 ~ 23:30 增量
			u8Tmp = 0;
			u16Tmp = 0x00;
			for(i = 0; i < 47; i++){
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X~", u8Tmp, u16Tmp);
				u16Tmp += 0x30;
				if(u16Tmp == 0x60){
					u16Tmp = 0x00;
					u8Tmp += 1;
				}
				dispIdx += sprintf(&dispBuf[dispIdx], "%d:%02X增量:%d\n", u8Tmp, u16Tmp, (buf[index] + buf[index + 1]*256));
				index += 2;
			}
		}

		//告警状态字
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		//阀门状态 
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门: %s  ", ptr);
		index += 1;
		//电池电压
		dispIdx += sprintf(&dispBuf[dispIdx], "电池: %c.%c\n", (buf[index] / 10) + '0', (buf[index] % 10) + '0');
		index += 1;
		//环境温度
		ptr = ((buf[index] & 0x80) > 0 ? "-" : "");
		dispIdx += sprintf(&dispBuf[dispIdx], "温度: %s%d  ", ptr, (buf[index] & 0x7F));
        index += 1;
		//SNR 噪音比
		dispIdx += sprintf(&dispBuf[dispIdx], "SNR : %d\n", buf[index]);
		index += 1;
		//tx|rx信道
		dispIdx += sprintf(&dispBuf[dispIdx], "信道: Tx-%d , Rx-%d\n", (buf[index] & 0x0F), (buf[index] >> 4));
		index += 1;
		//协议版本
		dispIdx += sprintf(&dispBuf[dispIdx], "协议版本: %d\n", buf[index]);
		index += 1;
		break;

	//--------------------------------------		UART表端模块测试：		---------------------
	case WaterCmd_ReadModuleRunningParams:		// 读取模块运行参数
		if(rxlen < index + 124 && cmd != 0x3A){
			break;
		}
		ret = RxResult_Ok;
		// 模块运行参数
		ptr = Water6009_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "仪表类型: %s\n", ptr);
		index += 1;
		switch (buf[index]){
		case 0x00:	u16Tmp = 1;	break;
		case 0x01:	u16Tmp = 10;	break;
		case 0x02:	u16Tmp = 100;	break;
		case 0x03:	u16Tmp = 1000;	break;
		default:  u16Tmp = buf[index];	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲系数: %d脉冲/方\n", u16Tmp);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "磁扰开阀时间: %d s\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲闭合时间: %d ms\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "开关阀时间: %d ms\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "过流阀值: %d mA\n",buf[index]);
		index += 1;
		switch (buf[index]){
		case 0x00:	ptr = "3.6 v";	break;
		case 0x01:	ptr = "6.0 v";	break;
		case 0x02:	ptr = "4.5 v";	break;
		default:  ptr = "未知";	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压类型: %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "定时上传间隔: %d h\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "定量上传间隔: %d m3\n", buf[index]);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "每天上传次数: %d\n", buf[index]);
		index += 1;
		ptr = Water6009_GetStrSensorType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "计量传感器类型: \n  %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "服务器-IP地址: \n  %d.%d.%d.%d\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "服务器-端口号: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		dispIdx += sprintf(&dispBuf[dispIdx], "水表防锈间隔: %d天\n", (buf[index] + buf[index + 1] * 256));
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "系统调试级别: %d\n", buf[index]);
		index += 1;
		GetStringHexFromBytes(TmpBuf, buf, index, 4, 0, false);
		if(TmpBuf[3] == '0' && TmpBuf[4] == '4'){
			ptr = "中国移到";
		}else if(TmpBuf[3] == '0' && TmpBuf[4] == '1'){
			ptr = "中国联通";
		}else if(TmpBuf[3] == '1' && TmpBuf[4] == '1'){
			ptr = "中国电信";
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "运营商编号: %s\n", TmpBuf);
		dispIdx += sprintf(&dispBuf[dispIdx], " SIM卡类型: %s\n", ptr);
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "当前系统时间: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		dispIdx += sprintf(&dispBuf[dispIdx], "通信频段: Band %d\n", buf[index]);
		index += 1;
		switch (buf[index]){
		case 0x01:	ptr = "COAP";	break;
		case 0x02:	ptr = "UDP";	break;
		default:  ptr = "未知";	break;
		}
		dispIdx += sprintf(&dispBuf[dispIdx], "连接方式: %s\n", ptr);
		index += 1;
		index += 2; // 保留
		dispIdx += sprintf(&dispBuf[dispIdx], "报警限值: %d\n", buf[index]);
		index += 1;
		u16Tmp = ((uint16)(buf[index + 1] << 8) + buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "关阀限值: %s%d\n", 
			((u16Tmp & 0x8000) > 0 ? "-" : ""), (u16Tmp & 0x7FFF));
		index += 2;
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "预缴用量:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "参考用量:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "模块测试状态如下\n");
		dispIdx += Water6009_GetStrTestStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		index += 2; // 保留
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "正转用量:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		u32Tmp = ((buf[index + 3] << 24) + (buf[index + 2] << 16) + (buf[index + 1] << 8) + buf[index]);
		u16Tmp = ((buf[index + 5] << 8) + buf[index + 4]);
		dispIdx += sprintf(&dispBuf[dispIdx], "反转用量:%d.%03d\n", u32Tmp, u16Tmp);
		index += 6;
		ptr = Water6009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门状态: %s\n", ptr);
		index += 1;
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "功能使能状态如下\n");
		dispIdx += Water6009_GetStrMeterFuncEnableState(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		u16Tmp = (buf[index] + buf[index + 1] * 256);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警状态: ");
		dispIdx += Water6009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		u8Tmp = buf[index];
		dispIdx += sprintf(&dispBuf[dispIdx], "按日按月设置: 按%s\n", (u8Tmp == 0x01 ? "日" : "月"));
		index += 1;	
		dispIdx += sprintf(&dispBuf[dispIdx], "侦听起始时间: %02d%s\n",  buf[index], (u8Tmp == 0x01 ? "点" : "号"));
		index += 1;	
		dispIdx += sprintf(&dispBuf[dispIdx], "侦听工作时长: %d%s\n",  buf[index], (u8Tmp == 0x01 ? "小时" : "天"));
		index += 1;	
		index += 10; // 保留
		memcpy(&VerInfo[0], &buf[index], 40);
		VerInfo[40] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "软件版本: %s\n", &VerInfo[0]);
		index += 40;
		break;

	case WaterCmd_SetModuleRunningParams:		// 设置模块运行参数
		if(rxlen < index + 1 && cmd != 0x3F){
			break;
		}
		ret = RxResult_Ok;
		// 命令状态
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? RxResult_Ok : RxResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;


	default:
		ret = RxResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "该命令[%02X]暂未解析\n", cmd);
		break;
	}


	if(index == startIdx + length - 4)
	{
		//下行/上行 信号强度
		#ifdef Project_6009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.signalValue, "%d", buf[index + 1]);	// 保存上行
			}
		#endif
		dispIdx += sprintf(&dispBuf[dispIdx], "                    \n");
		dispIdx += sprintf(&dispBuf[dispIdx], "下行: %d  上行: %d\n", buf[index], buf[index + 1]);
		index += 2;
	}

	dispBuf[dispIdx] = '\0';
	
	
	return ret;
}

//----------------------------------	版本信息		--------------------------
void VersionInfoFunc(void)
{
	uint8 key;

	while(1){
		_ClearScreen();

		_Printfxy(0, 0, "<<版本信息", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);
		//--------------------------------------------------
		PrintfXyMultiLine_VaList(0, 2*16, "  %s ", VerInfo_Name);
		PrintfXyMultiLine_VaList(0, 3*16, "版 本 号：%s", VerInfo_RevNo);
		PrintfXyMultiLine_VaList(0, 4*16, "版本日期：%s", VerInfo_RevDate);
		PrintfXyMultiLine_VaList(0, 5*16, "通信方式：%s", TransType);
		#ifdef VerInfo_Previwer
		PrintfXyMultiLine_VaList(0, 7*16, "      %s   ", VerInfo_Previwer);
		#endif
		//--------------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回            确定", Color_White);

		key = _ReadKey();		// 任意键返回
		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}
	}
}

//--------------------------------------	6009水表命令 发送、接收、结果显示	----------------------------
/*
* 描述： 命令发送/接收解析		- 执行完成后，返回结果
* 参数： cmdid	- 当前命令标识
*		addrs	- 地址域		
*		args	- 命令参数：args->items[0] - 命令ID, args->items[1] - 数据域
*		ackLen	- 应答长度 (byte)
*		timeout	- 超时时间 (ms)  默认为 8s + 中继数 x 2 x 6s
*		tryCnt	- 重试次数 默认3次
* 返回： bool  - 命令执行结果： true - 成功， false - 失败		
*/
bool Protol6009Tranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 sendCnt = 0, cmdResult, ret;
	uint16 waitTime = 0, currRxLen;
	int fp;

	if(_Access("system.cfg", 0) < 0){
		fp = _Fopen("system.cfg", "W");
	}else{
		fp = _Fopen("system.cfg", "RW");
	}
	if((args->buf[0] >= 0x40 && args->buf[0] <= 0x66) 
		|| (args->buf[0] >= 0xF1 && args->buf[0] <= 0xF3)){
		_Lseek(fp, 20, 0);	// 集中器号
	}else{
		#ifdef Project_6009_RF
		_Lseek(fp, 0, 0);	// byte [0 ~ 19] 12位表号 
		#else
		_Lseek(fp, 40, 0);	// byte [40 ~ 59] 16位表号 
		#endif
	}
	_Fwrite(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);
	
	_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
#if (AddrLen == 6)
	PrintfXyMultiLine_VaList(0, 1*16 + 8, "表号: %s ", StrDstAddr);
#else
	PrintfXyMultiLine_VaList(0, 1*16 + 8, "表号:\n   %s ", StrDstAddr);
#endif

	do{
		// 发送 
		_CloseCom();
		_ComSetTran(CurrPort);
		_ComSet(CurrBaud, 2);
		TxLen = PackWater6009RequestFrame(TxBuf, addrs, cmdid, args, sendCnt);
		_GetComStr(TmpBuf, 1000, 100/10);	// clear , 100ms timeout
		_SendComStr(TxBuf, TxLen);
		sendCnt++;
		if(sendCnt == 1){
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, " <  命令发送...  >  ", Color_White);
		}
		else{
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			PrintfXyMultiLine_VaList(0, 9*16, " <  命令重发...%d  > ", sendCnt);
		}

		// 接收
		_GetComStr(TmpBuf, 1000, 100/10);	// clear , 100ms timeout
		RxLen = 0;
		waitTime = 0;
		currRxLen = 0;
		_DoubleToStr(TmpBuf, (double)(timeout / 1000), 1);
		PrintfXyMultiLine_VaList(0, 5*16, "等待应答 %3d/%-3d  \n最多等待 %s s  ", RxLen, ackLen, TmpBuf);
		do{

			currRxLen = _GetComStr(&RxBuf[RxLen], 200, 16);	// N x10 ms 检测接收, 时间校准为 N x90% x10
			RxLen += currRxLen;
			if(KEY_CANCEL == _GetKeyExt()){
				//------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 6*16, "命令已取消         ", Color_White);
				_Printfxy(0, 9*16, "返回  <已取消>  继续", Color_White);
				DispBuf[0] = 0x00;
				return false;
			}
			waitTime += 200;

			if(waitTime % 1000 == 0){
				_DoubleToStr(TmpBuf, (double)((timeout - waitTime) / 1000), 1);
				PrintfXyMultiLine_VaList(0, 5*16, "等待应答 %3d/%-3d  \n最多等待 %s s  ", RxLen, ackLen, TmpBuf);
			}

			if(RxLen > 0 && currRxLen == 0){
				break;
			}
		}while(waitTime <= timeout || currRxLen > 0);

		PrintfXyMultiLine_VaList(0, 5*16, "当前应答 %3d/%-3d  \n", RxLen, ackLen);

		#if LOG_ON
			LogPrintBytes("Tx: ", TxBuf, TxLen);
			LogPrintBytes("Rx: ", RxBuf, RxLen);
		#endif

		cmdResult = ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, cmdid, ackLen, DispBuf);

	}while(sendCnt < tryCnt && (cmdResult == RxResult_Timeout || cmdResult == RxResult_CrcError));

	// 显示结果
#if RxBeep_On
	_SoundOn();
	_Sleep(50);
	_SoundOff();
#endif
	if(cmdResult == RxResult_Ok){
		_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
		//------------------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回  < 成功 >  继续", Color_White);
		ret = true;
	}
	else{
#if RxBeep_On
		_Sleep(30);
		_SoundOn();
		_Sleep(30);
		_SoundOff();
#endif
		if(cmdResult == RxResult_Failed){
		//	_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
		}
		//-----------------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回  < 失败 >  继续", Color_White);
		ret = false;
	}

	_CloseCom();

	return ret;
}

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
uint8 Protol6009TranceiverWaitUI(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 key;

	// 应答长度、超时时间、重发次数
#ifdef Project_6009_IR
	timeout = 5000;
	tryCnt = 3;
#else
	timeout = 10000 + (Addrs.itemCnt - 2) * 6000 * 2;
	tryCnt = 3;
#endif

	if(false == Protol6009Tranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt)){
		if(strncmp(DispBuf, "表号", 4) != 0){	// 命令已取消	
			DispBuf[0] = NULL;
		}
	}

	key = ShowScrollStr(&DispBuf, 7);

	#if LOG_ON
			LogPrint("解析结果: \r\n %s", DispBuf);
	#endif

	return key;
}

#endif
