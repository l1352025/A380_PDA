#ifndef WaterMeter8009_H
#define WaterMeter8009_H

#include "stdio.h"
#include "Common.h"

#ifdef Project_8009_RF
#include "MeterDocDBF.h"
#endif

extern void CycleInvoke_OpenLcdLight_WhenKeyPress(uint8 currKey);
extern uint8 PackWater8009RequestFrame(uint8 * buf, ParamsBuf *addrs, uint16 cmdId, ParamsBuf *args, uint8 retryCnt);
extern uint8 ExplainWater8009ResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

// --------------------------------  全局变量  -----------------------------------------
//char Screenbuff[160*(160/3+1)*2]; 
#if Upgrd_FileBuf_Enable
uint8 DispBuf[128 * 1024];					// 4k ~ 128K
#else
uint8 DispBuf[14 * 1024];					// 4k ~ 14K
#endif
uint8 * const LogBuf = &DispBuf[4096];     	// 4k ~ 
uint8 * const TmpBuf = &DispBuf[8192];     	// 2K ~ 
uint8 * const BackupBuf = &DispBuf[10240];	// 4k ~ 
#if Upgrd_FileBuf_Enable
uint8 * const FileBuf = &DispBuf[14336];	// 116k 
#endif
uint8 TxBuf[1024];
uint8 RxBuf[1024];
uint32 RxLen, TxLen;
const uint8 LocalAddr[10] = { 0x20, 0x19, 0x00, 0x00, 0x20, 0x19, 0x00, 0x00, 0x00, 0x00};	// 地址 2019000020190000，12/16字符
const uint8 BroadAddr[6] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
uint8 DstAddr[10];
uint8 VerInfo[42];
uint16 CurrCmd;
ParamsBuf Addrs;		
ParamsBuf Args;
char StrBuf[TXTBUF_MAX][TXTBUF_LEN];    // extend input buffer
char StrDstAddr[TXTBUF_LEN];
char StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
UI_ItemList UiList;
bool LcdOpened;
bool IsNoAckCmd;
FuncCmdCycleHandler TranceiverCycleHook = CycleInvoke_OpenLcdLight_WhenKeyPress;
FuncCmdFramePack FramePack = PackWater8009RequestFrame;
FuncCmdFrameExplain FrameExplain = ExplainWater8009ResponseFrame;

//----------------------------------------  表端命令  ------------------------
/*
表端操作：	
1	常用功能
2	功能配置
3	DMA项目
*/
typedef enum{
	/*
	常用功能：	
	1	读表数据
	2	设表底数
	3	表开阀
	4	表关阀
	5	清除异常
	6	读表参数
	7	设置表号
	*/
	WaterCmd_ReadRealTimeData			= 0x11,	
	WaterCmd_SetBaseValPulseRatio,
	WaterCmd_OpenValve,	
	WaterCmd_CloseValve,
	WaterCmd_ClearException,
	WaterCmd_ReadMeterCfgInfo,
	WaterCmd_SetMeterNumber,

	/*
	功能配置：	
	1	读取反转用量
	2	清除反转用量
	3	读功能使能状态old
	4	读功能使能状态new
	5	设置功能使能状态
	6	查询时钟及RF状态
	7	查询RF工作时段
	8	设置RF工作时段
	*/
	WaterCmd_ReadReverseMeasureData		= 0x21,
	WaterCmd_ClearReverseMeasureData,
	WaterCmd_ReadFuncEnableStateOld, 
	WaterCmd_ReadFuncEnableStateNew,
	WaterCmd_SetFuncEnableState,
	WaterCmd_ReadTimeAndRfState,	
	WaterCmd_ReadRfWorkTime,
	WaterCmd_SetRfWorkTime,

	/*
	DMA项目：	
	1	主动上传中心频点
	2	使能上传电表数据
	3	设置上传时间间隔
	4	读取上传时间间隔
	5	读冻结正转信息
	6	设置时钟
	7	读取时钟
	*/
	WaterCmd_UploadCenterFrequency		= 0x31,
	WaterCmd_EnableReportAmeterData,
	WaterCmd_SetReportTimeInterval, 
	WaterCmd_ReadReportTimeInterval,
	WaterCmd_ReadFrozenData,
	WaterCmd_SetMeterTime,
	WaterCmd_ReadMeterTime

}WaterCmdDef;


//----------------------------------------  集中器命令  ------------------------
/*
采集器操作：	
1	工作参数
2	档案参数
3	控制参数
4	路由参数
*/
typedef enum{
	/*
	工作参数：	
	1	读取采集器号
	2	设置采集器号
	3	读取采集器时钟
	4	设置采集器时钟
	5	采集器初始化
	6	清除抄表数据
	*/
	CenterCmd_ReadCenterNo		= 0x1011,
	CenterCmd_SetCenterNo,
	CenterCmd_ReadCenterTime,
	CenterCmd_SetCenterTime,
	CenterCmd_InitCenter,
	CenterCmd_ClearMeterReadData,

	/*
	档案参数：	
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
	控制参数：	
	1	读定量数据
	2	读实时数据
	3	读冻结数据
	4	开阀
	5	关阀
	6	清异常
	*/
	CenterCmd_ReadFixedValData	= 0x1031,
	CenterCmd_ReadRealTimeData,
	CenterCmd_ReadFrozenData,
	CenterCmd_OpenValve,
	CenterCmd_CloseValve,
	CenterCmd_ClearException,

	/*
	路由参数：	
	1	读取路由列表
	2	设置路由列表
	3	读取表具路由
	4	设置表具路由
	5	查看路由列表
	*/
	CenterCmd_ReadRouteList	= 0x1041,
	CenterCmd_SetRouteList,
	CenterCmd_ReadMeterRoute,
	CenterCmd_SetMeterRoute,
	CenterCmd_QueryRouteList

}CenterCmdDef;

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
	#ifdef Project_8009_RF
	uint8 i;
	#endif

	#ifdef Project_8009_RF
	// 中继地址
	addrs->itemCnt = 0;
	for(i = 0; i < RELAY_MAX; i++){
		if(strRelayAddrs[i][0] >= '0' && strRelayAddrs[i][0] <= '9'){
			addrs->items[addrs->itemCnt] = &addrs->buf[i*AddrLen];
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
* 描  述：获取8009水表-设备类型名
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * Water8009_GetStrDeviceType(uint8 typeId)
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
* 描  述：获取8009水表读数类型名
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * Water8009_GetStrValueType(uint8 typeId)
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
* 描  述：获取8009水表 计量传感器类型
* 参  数：typeId	- 类型ID
* 返回值：char *	- 解析后的字符串
*/
char * Water8009_GetStrSensorType(uint8 typeId)
{
	char * str = NULL;
	
	switch(typeId){
	case 0x00:	str = "单干簧管";	break;
	case 0x01:	str = "双干簧管";	break;
	case 0x02:	str = "单霍尔";	break;
	case 0x03:	str = "双霍尔";	break;
	case 0x04:	str = "三霍尔";	break;
	case 0x05:	str = "光电直读";	break;
	default:	str = "未知";	break;
	}

	return str;
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
		
		case 0x01:	str = "光电表,正强光干扰";	break;
		case 0x02:	str = "光电表,多组光管坏";	break;
		case 0x04:	str = "磁干扰标志";	break;
		case 0x08:	str = "光电表,一组光管坏";	break;
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
	
	if(status & 0x40 > 0){
		str = "开";	
	}
	else if(status & 0x20 > 0){
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
* 描  述：获取8009水表 集中器错误码
* 参  数：status	- 状态
* 返回值：char *	- 解析后的字符串
*/
char * Water8009_GetStrCenterErrorMsg(uint8 errorCode)
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
* 描  述：获取8009水表 阀控失败原因
* 参  数：errorCode	- 错误码
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water8009_GetStrValveCtrlFailed(uint16 errorCode, char * buf)
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
* 描  述：获取8009水表 功能使能状态
* 参  数：stateCode	- 使能状态码
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water8009_GetStrMeterFuncEnableState(uint16 stateCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], "磁干扰关阀功能  :%s\n", ((stateCode & 0x0001) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "磁干扰检测功能  :%s\n", ((stateCode & 0x0002) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "定时上传功能	   :%s\n", ((stateCode & 0x0004) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "定量上传功能    :%s\n", ((stateCode & 0x0008) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "防拆卸检测功能  :%s\n", ((stateCode & 0x0010) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "垂直安装检测    :%s\n", ((stateCode & 0x0020) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "主动告警        :%s\n", ((stateCode & 0x0040) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "防锈功能        :%s\n", ((stateCode & 0x0080) > 0 ? "开" : " 关"));
	
	len += sprintf(&buf[len], "RF分时段工作    :%s\n", ((stateCode & 0x0100) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "抄表同步时钟    :%s\n", ((stateCode & 0x0200) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "上传冻结数据    :%s\n", ((stateCode & 0x0400) > 0 ? "开" : " 关"));

	return len;
}

uint16 Water8009_GetStrMeterFuncEnableStateOld(uint16 stateCode, char * buf)
{
	uint16 len = 0;

	len += sprintf(&buf[len], "磁干扰关阀功能  :%s\n", ((stateCode & 0x0001) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "磁干扰检测功能  :%s\n", ((stateCode & 0x0002) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "防拆卸检测功能  :%s\n", ((stateCode & 0x0010) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "垂直安装检测    :%s\n", ((stateCode & 0x0020) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "主动告警        :%s\n", ((stateCode & 0x0040) > 0 ? "开" : " 关"));
	
	len += sprintf(&buf[len], "防锈功能        :%s\n", ((stateCode & 0x0080) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "定时上传功能	   :%s\n", ((stateCode & 0x0080) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "定量上传功能    :%s\n", ((stateCode & 0x0080) > 0 ? "开" : " 关"));
	
	len += sprintf(&buf[len], "RF分时段工作    :%s\n", ((stateCode & 0x0100) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "抄表同步时钟    :%s\n", ((stateCode & 0x0200) > 0 ? "开" : " 关"));
	len += sprintf(&buf[len], "上传冻结数据    :%s\n", ((stateCode & 0x0400) > 0 ? "开" : " 关"));

	return len;
}

/*
* 描  述：获取8009水表 模块测试状态
* 参  数：statusCode	- 测试状态码
* 		  buf		- 字符串显示缓冲区
* 返回值：uint16	- 解析后的字符串总长度
*/
uint16 Water8009_GetStrTestStatus(uint16 statusCode, char * buf)
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
	buf[index++] = 0xFE;		// 帧头： 固定为 D3 91
	buf[index++] = 0x68;
	buf[index++] = 0x00;		// 设备类型：00 -- 水表 

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
	uint8 crc8, relayCnt, cmd, subCmd, i, u8Tmp;
	uint16 index = 0, dispIdx, length, startIdx, payloadIdx, u16Tmp;
	uint32 u32Tmp;
	double f64Tmp;
	char *ptr;

	dispIdx = 0;

	// 显示表号 或 集中器号
	if(cmdId < 0x1010){
		#if (AddrLen < 8)
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: \n   %s\n", StrDstAddr);
		#endif
	}else{
		dispIdx += sprintf(&dispBuf[dispIdx], "集中器号: \n   %s\n", StrDstAddr);
	}

	// 缓冲区多包中查找
	while(1){

		// min length check (15 + 1)
		if(rxlen < index + 16){		
			sprintf(&dispBuf[dispIdx], "结果: 超时,无应答");
			return CmdResult_Timeout;
		}

		// start check
		if(buf[index] == 0xFE && buf[index + 1] == 0x68){
			index += 2;
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
		if(buf[index + 9] & 0x80 == 0x00){
			index += length;
			continue;
		}

		if(cmd != WaterCmd_SetMeterNumber && memcmp(BroadAddr, DstAddr, AddrLen) != 0){
			// dstaddr check
			if(memcmp(&buf[index + 3], DstAddr, AddrLen) != 0){
				index += length;
				continue;
			}
		}

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
		#if (AddrLen == 6)
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", StrDstAddr);
		#else
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: \n   %s\n", StrDstAddr);
		#endif
	}
	index += AddrLen;

	// 控制字2：路由级别|当前位置
	relayCnt = (uint8)(buf[index] >> 4);
	index += 1;

	// 控制字1：命令字
	cmd = (uint8)(buf[index] & 0x1F);

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
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "表读数: %d.%02d\n", u32Tmp, u8Tmp);
		#ifdef Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.meterValue, "%d.%02d", u32Tmp, u8Tmp);
			}
		#endif
		// 表口径、脉冲系数
		ptr = (buf[index] & 0x80) > 0 ? "大" : "小";
		dispIdx += sprintf(&dispBuf[dispIdx], "表口径: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲系数: %d\n", (buf[index] & 0x7F));
		index += 1;
		//电池电压
		f64Tmp = (double)buf[index] / 30.117534;
		ptr = _DoubleToStr(TmpBuf, f64Tmp, 2);
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压: %s v\n", ptr);
		index += 1;
		#ifdef Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				strcpy(MeterInfo.batteryVoltage, ptr);
			}
		#endif
		//阀门状态
		ptr = Water8009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门状态: %s", ptr);
		index += 1;
		#ifdef Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				u8Tmp = sprintf(&MeterInfo.meterStatusStr[0], "阀门%s,", ptr);
				MeterInfo.meterStatusStr[u8Tmp - 1] = 0x00;
			}
		#endif

		//告警状态字
		#ifdef Project_8009_RF
			u32Tmp = dispIdx + 10;
		#endif
		u16Tmp = GetUint16(buf, 2, true);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警状态: ");
		dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		#ifdef Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				u16Tmp = (uint16)(dispIdx - u32Tmp);
				if(u16Tmp + u8Tmp > Size_MeterStatusStr){
					u16Tmp = Size_MeterStatusStr - u8Tmp;
				}
				strncpy(&MeterInfo.meterStatusStr[u8Tmp], &dispBuf[u32Tmp], u16Tmp);
				sprintf(MeterInfo.meterStatusHex, "%02X%02X%02X", buf[index - 3], buf[index - 2], buf[index - 1]);
			}
		#endif
		break;

	case WaterCmd_SetBaseValPulseRatio:	// 设表底数脉冲系数
		if(rxlen < index + 5 || cmd != 0x04){
			break;
		}
		ret = CmdResult_Ok;
		// 表底数
		u32Tmp = GetUint32(&buf[index], 3, false);
		u8Tmp = buf[index + 3];
		index += 4;
		dispIdx += sprintf(&dispBuf[dispIdx], "表底数: %d.%02d\n", u32Tmp, u8Tmp);
		// 表口径、脉冲系数
		ptr = (buf[index] & 0x80) > 0 ? "大" : "小";
		dispIdx += sprintf(&dispBuf[dispIdx], "表口径: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲系数: %d\n", (buf[index] & 0x7F));
		index += 1;
		break;

	case WaterCmd_OpenValve:		// 开阀
	case WaterCmd_CloseValve:		// 关阀
		if(rxlen < index || (cmd != 0x05 && cmd != 0x06)){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		break;

	case WaterCmd_ClearException:	// 清异常命令 
		if(rxlen < index || cmd != 0x03){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		break;

	case WaterCmd_ReadMeterCfgInfo:	// 读取表端配置信息

		if(rxlen < index + 31 || cmd != 0x02){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// 开关阀时间 200ms * n
		f64Tmp = (buf[index] & 0x7F) * 0.2;
		ptr = _DoubleToStr(TmpBuf, f64Tmp, 1);
		dispIdx += sprintf(&dispBuf[dispIdx], "开关阀时间: %s s\n", ptr);
		index += 1;
		// 过流门限值 2mA * n
		dispIdx += sprintf(&dispBuf[dispIdx], "过流门限值: %d mA\n", (buf[index] & 0x7F) * 2);
		index += 1;
		// 脉冲类型、电压类型、信道
		ptr = Water8009_GetStrSensorType(buf[index] >> 5);
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲类型: %s mA\n", ptr);
		ptr = ((buf[index] & 0x10) > 0 ? "3.6 v" : "6.0 v");
		dispIdx += sprintf(&dispBuf[dispIdx], "电压类型: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "信道: %d \n", (buf[index] & 0x07));
		index += 1;
		// 表口径、脉冲系数
		ptr = (buf[index] & 0x80) > 0 ? "大" : "小";
		dispIdx += sprintf(&dispBuf[dispIdx], "表口径: %s\n", ptr);
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲系数: %d\n", (buf[index] & 0x7F));
		index += 1;
		// 磁干扰开阀时间
		dispIdx += sprintf(&dispBuf[dispIdx], "磁扰开阀时间: %d\n", (buf[index] & 0x7F));
		index += 1;
		// 脉冲最小脉宽
		dispIdx += sprintf(&dispBuf[dispIdx], "脉冲最小脉宽: %d\n", buf[index]);
		index += 1;
		// 版本号
		memcpy(&VerInfo[0], &buf[index], VerLen);
		VerInfo[VerLen] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "版本: %s\n", &VerInfo[0]);
		index += VerLen;
		break;

	case WaterCmd_SetMeterNumber:	// 设置表号
		if(rxlen < index + 1 || cmd != 0x1c){
			break;
		}
		ret = CmdResult_Ok;
		index += 1;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			GetStringHexFromBytes((char *)&TmpBuf[0], buf, index, AddrLen, 0, false);
			TmpBuf[12] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "新表号: %s\n", &TmpBuf[0]);
		}
		else{
			ret = CmdResult_Failed;
		}
		index += AddrLen;
		break;

	//----------------------------------		功能配置		----------------------------

	case WaterCmd_ReadReverseMeasureData:	// 读取反转用量
		if(rxlen < index + 4 || cmd != 0x0A){
			break;
		}
		ret = CmdResult_Ok;
		if(rxlen > index + 4 + 9){
			// 反转读数
			u32Tmp = GetUint32(&buf[index], 3, false);
			u8Tmp = buf[index + 3];
			dispIdx += sprintf(&dispBuf[dispIdx], "表读数: %d.%02d\n", u32Tmp, u8Tmp);
			index += 4;
			// 表口径、脉冲系数
			ptr = (buf[index] & 0x80) > 0 ? "大" : "小";
			dispIdx += sprintf(&dispBuf[dispIdx], "表口径: %s\n", ptr);
			dispIdx += sprintf(&dispBuf[dispIdx], "脉冲系数: %d\n", (buf[index] & 0x7F));
			index += 1;
			//电池电压
			f64Tmp = (double)buf[index] / 30.117534;
			ptr = _DoubleToStr(TmpBuf, f64Tmp, 2);
			dispIdx += sprintf(&dispBuf[dispIdx], "电池: %s v\n", ptr);
			index += 1;
			//阀门状态
			ptr = Water8009_GetStrValveStatus(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "阀门: %s", ptr);
			index += 1;
			//告警状态字
			u16Tmp = GetUint16(buf, 2, true);
			dispIdx += sprintf(&dispBuf[dispIdx], "告警: ");
			dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
			index += 2;
		}
		else{
			// 反转读数
			u32Tmp = GetUint32(&buf[index], 3, false);
			u8Tmp = buf[index + 3];
			dispIdx += sprintf(&dispBuf[dispIdx], "表读数: %d.%02d\n", u32Tmp, u8Tmp);
			index += 4;
		}
		break;

	case WaterCmd_ClearReverseMeasureData:	// 清除反转用量
		if(rxlen < index || cmd != 0x12){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		break;

	case WaterCmd_ReadFuncEnableStateOld:	// 读取功能使能状态 old
		if(rxlen < index + 2 || cmd != 0x0B){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "功能使能状态如下\n");
		u16Tmp = GetUint16(buf, 2, false);
		dispIdx += Water8009_GetStrMeterFuncEnableStateOld(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	case WaterCmd_ReadFuncEnableStateNew:	// 读取功能使能状态 new
		if(rxlen < index + 2 || cmd != 0x0B){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "功能使能状态如下\n");
		u16Tmp = GetUint16(buf, 2, false);
		dispIdx += Water8009_GetStrMeterFuncEnableState(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	
	case WaterCmd_SetFuncEnableState:	// 设置功能使能状态 
		if(rxlen < index + 2 ){
			break;
		}
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		break;


	case WaterCmd_ReadTimeAndRfState:	// 查询时钟及RF状态 、打开/关闭RF分时段功能 09-00/01
		if(rxlen < index + 9 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// 模块使能状态
			ptr = (buf[index] & 0x01 > 0 ? "开" : "关");
			dispIdx += sprintf(&dispBuf[dispIdx], "RF分时段工作: %s\n", ptr);
			ptr = (buf[index] & 0x02 > 0 ? "开" : "关");
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表同步时钟: %s\n", ptr);
			index += 1;
			// 当前时钟
			dispIdx += GetTimeStr(&dispBuf[dispIdx], "表端当前时钟: \n  %02X%02X-%02X-%02X %02X:%02X:%02X\n"
				, &buf[index], 7);
			index += 7;
		}
		break;


	case WaterCmd_ReadRfWorkTime:		// 查询RF工作时段 09-04
		if(rxlen < index + 6 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// 模块使能状态
			ptr = (buf[index] & 0x01 > 0 ? "开" : "关");
			dispIdx += sprintf(&dispBuf[dispIdx], "RF分时段工作: %s\n", ptr);
			ptr = (buf[index] & 0x02 > 0 ? "开" : "关");
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表同步时钟: %s\n", ptr);
			index += 1;
			// 工作时段
			if(buf[index] == buf[index + 2]){
				dispIdx += sprintf(&dispBuf[dispIdx], "每月工作时间段：\n");
				dispIdx += sprintf(&dispBuf[dispIdx], "%x号%02X:00-%x号%02X:00\n", 
					buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			}
			else{
				dispIdx += sprintf(&dispBuf[dispIdx], "每天工作时间段：\n");
				dispIdx += sprintf(&dispBuf[dispIdx], "%02X:00-%02X:00\n", buf[index + 1], buf[index + 3]);
			}
			index += 4;
		}
		break;

	case WaterCmd_SetRfWorkTime:		// 设置RF工作时段 09-84
		if(rxlen < index + 6 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// 模块使能状态
			ptr = (buf[index] & 0x01 > 0 ? "开" : "关");
			dispIdx += sprintf(&dispBuf[dispIdx], "RF分时段工作: %s\n", ptr);
			ptr = (buf[index] & 0x02 > 0 ? "开" : "关");
			dispIdx += sprintf(&dispBuf[dispIdx], "抄表同步时钟: %s\n", ptr);
			index += 1;
			// 工作时段
			if(buf[index] == buf[index + 2]){
				dispIdx += sprintf(&dispBuf[dispIdx], "每月工作时间段：\n");
				dispIdx += sprintf(&dispBuf[dispIdx], "%x号%02X:00-%x号%02X:00\n", 
					buf[index], buf[index + 1], buf[index + 2], buf[index + 3]);
			}
			else{
				dispIdx += sprintf(&dispBuf[dispIdx], "每天工作时间段：\n");
				dispIdx += sprintf(&dispBuf[dispIdx], "%02X:00-%02X:00\n", buf[index + 1], buf[index + 3]);
			}
			index += 4;
		}
		break;


	//--------------------------------		DMA 项目	-----------------------------------

	case WaterCmd_UploadCenterFrequency:		// 主动上传中心频点 09-09/89
		if(rxlen < index + 2 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 跳过 子命令字
		index += 1;
		break;

	case WaterCmd_EnableReportAmeterData:		// 使能模块上传电表数 09-0B/8B
		if(rxlen < index + 1 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 跳过 子命令字
		index += 1;
		break;

	case WaterCmd_SetReportTimeInterval:		// 设置上传时间间隔 09-0A
	case WaterCmd_ReadReportTimeInterval:		// 读取上传时间间隔
		if(rxlen < index + 1 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 跳过 子命令字
		index += 1;
		// 时间间隔
		dispIdx += sprintf(&dispBuf[dispIdx], "时间间隔: %d 小时\n", buf[index]);
		index += 1;
		break;

	case WaterCmd_ReadFrozenData:	// 读取冻结数据 09-05/06
		if(rxlen < index + 54 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 冻结数据类型: 05-正转
		dispIdx += sprintf(&dispBuf[dispIdx], "冻结类型: %s\n", (buf[index] == 0x05 ? "正转" : "反转"));
		index += 1;
		// 冻结数据起始序号
		u8Tmp = buf[index] * 10;
		dispIdx += sprintf(&dispBuf[dispIdx], "数据范围: 第 %d~%d 条\n", u8Tmp, u8Tmp + 9);
		index += 1;
		// 冻结的用量数据：7*N 字节 （6 byte 用量 + 1 byte date.day）
		dispIdx += sprintf(&dispBuf[dispIdx], "读取的10条数据如下: \n");
		for(i = 0; i < 10; i++){
			u32Tmp = GetUint32(&buf[index], 3, false);
			u8Tmp = buf[index + 3];
			dispIdx += sprintf(&dispBuf[dispIdx], "%02d : %d.%02d\n", i, u32Tmp, u8Tmp);
			index += 4;
		}
		// 冻结数据起始时间
		dispIdx += GetTimeStr(&dispBuf[dispIdx], "起始时间: \n %02X%02X%02X%02X %02X:%02X:00\n", &buf[index], 6);
		index += 6;
		// 冻结数据时间间隔
		dispIdx += sprintf(&dispBuf[dispIdx], "时间间隔: %d 分钟\n", buf[index]);
		index += 1;
		//电池电压
		f64Tmp = (double)buf[index] / 30.117534;
		ptr = _DoubleToStr(TmpBuf, f64Tmp, 2);
		dispIdx += sprintf(&dispBuf[dispIdx], "电池电压: %s v\n", ptr);
		index += 1;
		//阀门状态
		ptr = Water8009_GetStrValveStatus(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "阀门状态: %s", ptr);
		index += 1;
		//告警状态字
		u16Tmp = GetUint16(buf, 2, true);
		dispIdx += sprintf(&dispBuf[dispIdx], "告警状态: ");
		dispIdx += Water8009_GetStrAlarmStatus(u16Tmp, &dispBuf[dispIdx]);
		index += 2;
		break;

	case WaterCmd_ReadMeterTime:	// 读表端时钟 09-00
		if(rxlen < index + 9 || cmd != 0x09){
			break;
		}
		// 操作结果
		ptr = (buf[index] == 0xAA ? "操作成功" : "操作失败");
		ret = (buf[index] == 0xAA ? CmdResult_Ok : CmdResult_Failed);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		if(buf[index - 1] == 0xAA){
			// 模块使能状态 跳过
			index += 1;
			// 当前时钟
			dispIdx += GetTimeStr(&dispBuf[dispIdx], "表端当前时钟: \n  %02X%02X-%02X-%02X %02X:%02X:%02X\n"
				, &buf[index], 7);
			index += 7;
		}
		break;

	case WaterCmd_SetMeterTime:		// 校表端时钟：抄表指令01 + 时间
		if(rxlen < index + 9 || cmd != 0x01){
			break;
		}
		ret = CmdResult_Ok;
		// 抄表成功则校时成功
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: 操作成功\n");
		index += 9;
		break;


	//-------------------------------------------------------------------------------------------------
	//--------------------------------------	集中器命令 0x40 ~ 0x65 , F1 ~ F3	--------------------
	//-------------------------------------------------------------------------------------------------
	//--------------------------------------		工作参数		-------------------------------------
	case CenterCmd_ReadCenterNo:	// 读集中器号
		if(rxlen < index + 6 || cmd != 0x41){
			break;
		}
		ret = CmdResult_Ok;
		// 集中器号
		dispIdx += sprintf(&dispBuf[dispIdx], "集中器号: \n    %02X%02X%02X%02X%02X%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		break;

	case CenterCmd_SetCenterNo:		// 设置采集器号
		if(rxlen < index + 6 || cmd != 0x41){
			break;
		}
		ret = CmdResult_Ok;
		// 集中器号
		dispIdx += sprintf(&dispBuf[dispIdx], "集中器号: \n    %02X%02X%02X%02X%02X%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3], buf[index + 4], buf[index + 5]);
		index += 6;
		break;

	case CenterCmd_ReadCenterTime:		// 读集中器时钟
		if(rxlen < index + 7 || cmd != 0x43){
			break;
		}
		ret = CmdResult_Ok;
		// 集中器时钟
		dispIdx += sprintf(&dispBuf[dispIdx], "集中器时钟: \n %02X%02X-%02X-%02X %02X:%02X:%02X\n", 
			buf[index], buf[index + 1], buf[index + 2], buf[index + 3]
			, buf[index + 4], buf[index + 5], buf[index + 6]);
		index += 7;
		break;

	case CenterCmd_SetCenterTime:		// 设集中器时钟
		if(rxlen < index + 1 || cmd != 0x44){
			break;
		}
		ret = CmdResult_Ok;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_InitCenter:		// 集中器初始化
		if(rxlen < index + 2 || cmd != 0x48){
			break;
		}
		ret = CmdResult_Ok;
		// 数据上传功能
		ptr = (buf[index] == 0 ? "清空档案和路径" : "清空所有数据");
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", ptr);
		index += 1;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ClearMeterReadData:		// 清除抄表数据
		if(rxlen < index + 2 || cmd != 0x48){
			break;
		}
		ret = CmdResult_Ok;
		// 数据上传功能
		ptr = (buf[index] == 0 ? "清空档案和路径" : "清空所有数据");
		dispIdx += sprintf(&dispBuf[dispIdx], "类型: %s\n", ptr);
		index += 1;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;


	//--------------------------------------		档案操作：		-------------------------------------
	case CenterCmd_ReadDocCount:		// 读档案数量
		if(rxlen < index + 3 || cmd != 0x50){
			break;
		}
		ret = CmdResult_Ok;
		// 命令状态
		ptr = Water8009_GetStrDeviceType(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "设备类型: %s\n", ptr);
		index += 1;
		dispIdx += sprintf(&dispBuf[dispIdx], "档案数量: %d\n", (buf[index] + buf[index + 1] * 256));
		index += 2;
		break;

	case CenterCmd_ReadDocInfo:			// 读档案信息
		if(rxlen < index + 3 || cmd != 0x51){
			break;
		}
		ret = CmdResult_Ok;
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
			ptr = Water8009_GetStrDeviceType(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   类型:%s\n", ptr);
			index += 1;
			ptr = (buf[index] == 0 ? "失败" : (buf[index] == 1 ? "成功" : "未知"));
			dispIdx += sprintf(&dispBuf[dispIdx], "   抄表:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_AddDocInfo:			// 添加档案信息
		if(rxlen < index + 1 || cmd != 0x52){
			break;
		}
		ret = CmdResult_Ok;
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
			ptr = Water8009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   结果:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_DeleteDocInfo:			// 删除档案信息
		if(rxlen < index + 7 || cmd != 0x53){
			break;
		}
		ret = CmdResult_Ok;
		// N个节点设置结果
		u8Tmp = (rxlen - index - 4) / 7;
		for(i = 0; i < u8Tmp; i++){
			GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
			TmpBuf[6] = 0x00;
			dispIdx += sprintf(&dispBuf[dispIdx], "节点 %2d:%s\n", i + 1, &TmpBuf[0]);
			index += 6;
			ptr = Water8009_GetStrErrorMsg(buf[index]);
			dispIdx += sprintf(&dispBuf[dispIdx], "   结果:%s\n", ptr);
			index += 1;
		}
		break;

	case CenterCmd_ModifyDocInfo:			// 修改档案信息
		if(rxlen < index + 1 || cmd != 0x54){
			break;
		}
		ret = CmdResult_Ok;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;


	//--------------------------------------		控制参数：		-------------------------------------
	case CenterCmd_ReadFixedValData:			// 读定量数据
		break;

	case CenterCmd_ReadRealTimeData:			// 读实时数据
		break;

	case CenterCmd_ReadFrozenData:			// 读冻结数据
		break;

	case CenterCmd_OpenValve:			// 开阀
	case CenterCmd_CloseValve:			// 关阀
	case CenterCmd_ClearException:		// 清异常
		break;

	//--------------------------------------		路由参数：		-------------------------------------
	case CenterCmd_ReadRouteList:		// 读取路由列表
		if(rxlen < index + 10 || cmd != 0x55){
			break;
		}
		ret = CmdResult_Ok;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 设备类型
		ptr = Water8009_GetStrDeviceType(buf[index]);
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

	case CenterCmd_SetRouteList:			// 设置路由列表
		if(rxlen < index + 7 || cmd != 0x56){
			break;
		}
		ret = CmdResult_Ok;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_ReadMeterRoute:			// 读取表具路由
		if(rxlen < index + 7 || cmd != 0x56){
			break;
		}
		ret = CmdResult_Ok;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_SetMeterRoute:			// 设置表具路由
		if(rxlen < index + 7 || cmd != 0x56){
			break;
		}
		ret = CmdResult_Ok;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	case CenterCmd_QueryRouteList:			// 查看路由列表
		if(rxlen < index + 7 || cmd != 0x56){
			break;
		}
		ret = CmdResult_Ok;
		// 表号
		GetStringHexFromBytes(&TmpBuf[0], &buf[index], 0, 6, 0, false);
		TmpBuf[6] = 0x00;
		dispIdx += sprintf(&dispBuf[dispIdx], "表号: %s\n", &TmpBuf[0]);
		index += 6;
		// 命令状态
		ptr = Water8009_GetStrErrorMsg(buf[index]);
		dispIdx += sprintf(&dispBuf[dispIdx], "结果: %s\n", ptr);
		index += 1;
		break;

	default:
		ret = CmdResult_Ok;
		dispIdx += sprintf(&dispBuf[dispIdx], "该命令[%02X]暂未解析\n", cmd);
		break;
	}


	if(index == startIdx + length - 4)
	{
		//下行/上行 信号强度
		#ifdef Project_8009_RF
			if(MeterInfo.dbIdx != Invalid_dbIdx){
				sprintf(MeterInfo.signalValue, "%d", buf[index + 1]);	// 保存上行
			}
		#endif
		dispIdx += sprintf(&dispBuf[dispIdx], "                    \n");
		dispIdx += sprintf(&dispBuf[dispIdx], "下行: %d  上行: %d\n", buf[index], buf[index + 1]);
		index += 2;
	}
	else{
		dispIdx += sprintf(&dispBuf[dispIdx], "应答错误！\n");
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
		PrintfXyMultiLine_VaList(0, 6*16, "通信速率：%s", CurrBaud);
		#ifdef VerInfo_Previwer
		PrintfXyMultiLine_VaList(0, 7*16 + 8, "%s", VerInfo_Previwer);
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
	ackLen += 14 + addrs->itemCnt * AddrLen;
	timeout = 2000;
	tryCnt = 3;
#elif defined(Project_6009_RF)
	ackLen += 14 + addrs->itemCnt * AddrLen;
	timeout = 10000 + (addrs->itemCnt - 2) * 6000 * 2;
	tryCnt = 3;
#else // Project_8009_RF
	ackLen += 10 + addrs->itemCnt * AddrLen;
	timeout = 2000 + (addrs->itemCnt - 1) * 2000;
	tryCnt = 3;
#endif

	ProtolCommandTranceiver(cmdid, addrs, args, ackLen, timeout, tryCnt);

	key = ShowScrollStr(&DispBuf, 7);

	#if LOG_ON
		LogPrint("解析结果: \r\n %s", DispBuf);
	#endif

	return key;
}

#endif
