#ifndef WaterMeter_H
#define WaterMeter_H


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
	WaterCmd_SetDefinedRoute,
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
	CenterCmd_ClearException,

}CenterCmdDef;


//----------------------------------------------------------------
/*
* 函数名：PackWaterRequestFrame
* 描  述：打包水表命令请求帧
* 参  数：buf - 数据缓存起始地址
		  dstAddr - 目的地址
		  cmdId - 命令字
		  retryCnt - 重发次数：0 - 第1次发送，其他 - 第n次重发
* 返回值：uint8 帧总长度
*/
uint8 PackWaterRequestFrame(uint8 * buf, const uint8 * dstAddr, uint8 cmdId, uint8 *args[], uint8 retryCnt)
{
	static uint8 macFsn = 0xFF, nwkFsn = 0xFF, apsFsn = 0xFF, index = 0;
	uint8 macCmdStart, nwkCmdStart, apsCmdStart, dltStart, i, relayCnt;
	uint16 crc16;
	uint8 crc8;

	if(retryCnt > 0 && index > 0){
		return index;
	}

	// mac layer
	index = 0;
	buf[index++] = 0x00;	// length		- skip
	buf[index++] = 0x00;	// channel
	buf[index++] = 0x01;	// ver
	buf[index++] = 0x00;	// xor(0~2) check	- skip
	buf[index++] = 0x41;	// mac ctrl 
	buf[index++] = 0xCD;
	macFsn++;
	buf[index++] = macFsn;	// mac fsn
	buf[index++] = 0xFF;	// panid
	buf[index++] = 0xFF;
	memcpy(&buf[index], dstAddr, 6);
	index += 6; 			// mac dst addr - skip
	memcpy(&buf[index], LocalAddr, 6);
	index += 6; 			// mac src addr
	macCmdStart = index;

	// nwk layer
	buf[index++] = 0x3C;	// nwk ctrl
	memcpy(&buf[index], dstAddr, 6);
	index += 6; 			// nwk dst addr - skip
	memcpy(&buf[index], LocalAddr, 6);
	index += 6; 			// nwk src addr
	nwkFsn++;
	buf[index++] = (nwkFsn << 4) | 0x01;	// nwk fsn|radius - fixed
	
	// 若带路由，添加中继地址
	if(cmdId == PowerCmd_ReadMeter_645_97
		|| cmdId == PowerCmd_ReadMeter_645_07
		|| cmdId == PowerCmd_ReadMeter_698){

		// args[0] - 电表地址
		// args[1] - 中继总数
		// args[2-n] - 地址列表
		relayCnt = *args[1];
		if(relayCnt > 0){	
			// 修改mac层目的地址
			memcpy(&buf[9], args[2], 6);
			// 修改网络半径
			buf[index - 1] = (nwkFsn << 4) | ((relayCnt & 0x07) + 1);
			// 中继总数bit4-0 , 中继索引 bit9-5 , 中继地址模式 bit23-10, 2位 * 7 ：10 - 短地址， 11 - 长地址
			buf[index++] = ((relayCnt & 0x07) << 5) + (relayCnt & 0x1F);
			buf[index++] = (relayCnt >> 3)+ 0xFC;
			buf[index++] = 0xFF;
			// 中继列表
			for(i = 0; i < relayCnt; i++){
				memcpy(&buf[index], args[2 + i], 6);
				index += 6;
			}
		}
	}
	nwkCmdStart = index;

	// aps layer
	buf[index++] = 0x09;	// aps ctrl 
	buf[index++] = apsFsn;	// aps Fsn
	buf[index++] = 0x05;	// expand : "SR19"
	buf[index++] = 0x53;
	buf[index++] = 0x53;
	buf[index++] = 0x52;
	buf[index++] = 0x31;
	buf[index++] = 0x39;
	apsCmdStart = index;
	
	// cmd case
	switch(cmdId){

	//-------------------------------------------  抄表		-------------
	
	//-------------------------------------------  参数读取	 -------------
	
	case PowerCmd_ReadNodeInfo:		/*	集中器/电表 命令  */
		apsFsn++;
		buf[index++] = 0x04;
		break;
	case PowerCmd_ReadNwkStatus:
		apsFsn++;
		buf[index++] = 0x93;
		break;
	case PowerCmd_ReadSendPower:
		apsFsn++;
		buf[index++] = 0x94;
		break;
	case PowerCmd_ReadVerInfo:
		apsFsn++;
		buf[index++] = 0x95;
		break;
	case PowerCmd_ReadNeighbor:			/*	电表 命令  */
		index = nwkCmdStart;
		buf[macCmdStart] = 0x3D;	// nwk ctrl
		buf[index++] = 0x10;
		break;
	case PowerCmd_ReadSubNodeRoute:		/*	集中器 命令  */
		apsFsn++;
		buf[index++] = 0x92;
		memcpy(&buf[index], args[0], 6);
		index += 6;
		break;


	//-------------------------------------------  节点控制		-------------
	
	case PowerCmd_SetSerialCom:			/*	集中器/电表 命令  */
		apsFsn++;
		buf[index++] = 0x00;
		buf[index++] = *args[0];
		buf[index++] = *args[1];
		break;


	case PowerCmd_ParamsInit:			/*	集中器 命令  */
		apsFsn++;
		buf[index++] = 0x90;
		break;
	
	default:
		break;
	}

	// calc length / crc16
	buf[0] = index - 1;
	buf[3] = buf[0] ^ buf[1] ^ buf[2];
	crc16 = GetCrc16(buf, index, CRC16_Seed);
	buf[index++] = (uint8)(crc16 & 0xFF);
	buf[index++] = (uint8)(crc16 >> 8);
	
	return index;
}

/*
* 函数名：ExplainWaterResponseFrame
* 描  述：解析水表命令响应帧
* 参  数：buf - 接收缓存起始地址
		  srcAddr - 源地址
		  cmdId - 命令字
		  disp - 解析的显示数据
* 返回值：bool 解析结果：fasle - 失败 ， true - 成功
*/
bool ExplainWaterResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * srcAddr, uint8 cmdId, ParamsBuf * disps)
{
	uint8 index = 0, len = 0, radius;
	uint8 nwkCtrl, apsCtrl;
	bool ret = false;
	uint16 crc16;

	// 缓冲区多包中查找
	while(1){

		if(rxlen < index + 35){
			disps->cnt = 1;
			sprintf(&disps->buf[0], "未应答");
			disps->items[0] = &disps->buf[0];
			return false;
		}

		if(buf[index] == 0x55 && buf[index + 1] == 0xAA){
			index += 3;
		}else{
			index++;
			continue;
		}
		
		// length check
		if(index + buf[index] + 3 > rxlen){
			index += buf[index] + 3;
			continue;
		}	

		// dstaddr check
		if(strncmp(&buf[index + 9], LocalAddr, 6) != 0){
			index += buf[index] + 3;
			continue;
		}

		// crc16 check
		len = buf[index];
		crc16 = GetCrc16(&buf[index], len + 1, CRC16_Seed);
		if(crc16 !=  (uint16)((buf[index + len + 2] << 8) + buf[index + len + 1])){
			disps->cnt = 1;
			sprintf(&disps->buf[0], "CRC错误");
			disps->items[0] = &disps->buf[0];
			return false;
		}

		break;
	}

	// mac layer
	index += 21;
	// nwk layer
	nwkCtrl = buf[index++];			// nwk ctrl
	index += 12;
	radius = buf[index++] & 0x0F;	// nwk radius
	index += (radius > 1 ? (radius - 1) * 6 + 3 : 0);

	// aps layer
	if(nwkCtrl == 0x3C || nwkCtrl == 0xBC){
		apsCtrl = buf[index++];	// aps ctrl
		index++;				// aps Fsn
		if((apsCtrl & 0x08) > 0){
			index += buf[index] + 1;
		}
	}
	
	// cmd case
	switch(cmdId){

	//-------------------------------------------  抄表		-------------
	case PowerCmd_ReadMeter_698:
		index++;
		if(buf[index] == 0x68 
			&& buf[index + 1] == 0x34 && buf[index + 2] == 0x00	// length
			&& buf[index + 3] == 0xC3 		// ctrl
			&& buf[index + 4] == 0x05
			){
			// 抄读成功，暂不解析
			ret = true;

			disps->cnt = 1;
			sprintf(&disps->buf[0], "暂不解析");
			disps->items[0] = &disps->buf[0];
		}
		break;
	//-------------------------------------------  参数读取	 -------------

	default:
		break;
	}
	
	return ret;
}






#endif
