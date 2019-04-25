#include <HJLIB.H>
#include "string.h"
//#include "dbf.h"
#include "stdio.h"
#include "main.h"
#include "common.h"

// --------------------------------  全局变量  -----------------------------------------
char Screenbuff[160*(160/3+1)*2]; 
uint8 TmpBuf[1080];
uint8 TxBuf[1080];
uint8 RxBuf[1080];
uint32 RxLen, TxLen;
const char * ZeroAddr = "00000000000000";	// 0 地址，14字符
const uint8 LocalAddr[7] = {0x19, 0x20, 0x00, 0x00, 0x19, 0x20, 0x00};	// 地址 00201900002019，14字符
uint8 DstAddr[7];
uint8 CurrCmd;
ParamsBuf Args;
ParamsBuf Disps;
uint8 InputBuff_1[20] = {0};
uint8 InputBuff_2[20] = {0};
uint8 InputBuff_Tmp1[20] = {0};
uint8 InputBuff_Tmp2[20] = {0};


//---------------------------------  命令发送	-----------------------------------------



/*
* 函数名：PackElectricRequestFrame
* 描  述：打包电力命令请求帧
* 参  数：buf - 数据缓存起始地址
		  dstAddr - 目的地址
		  cmdId - 命令字
		  retryCnt - 重发次数：0 - 第1次发送，其他 - 第n次重发
* 返回值：uint8 帧总长度
*/
uint8 PackElectricRequestFrame(uint8 * buf, const uint8 * dstAddr, uint8 cmdId, uint8 *args[], uint8 retryCnt)
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
	// args[0] - 电表地址
	// args[1] - 中继总数
	// args[2-n] - 地址列表
	case PowerCmd_ReadMeter_645_97:
		apsFsn++;
		buf[nwkCmdStart] = 0x0A;		// aps ctrl
		buf[index++] = 0x00;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		dltStart = index;
		buf[index++] = 0x68;
		memcpy(&buf[index], args[0], 6);
		index += 6;
		buf[index++] = 0x68;
		buf[index++] = 0x01;
		buf[index++] = 0x02;
		buf[index++] = 0x43;
		buf[index++] = 0xC3;
		crc8 = GetSum8(&buf[dltStart], index - dltStart);
		buf[index++] = crc8;
		buf[index++] = 0x16;
		break;
	case PowerCmd_ReadMeter_645_07:
		apsFsn++;
		buf[nwkCmdStart] = 0x0A;	// aps ctrl
		buf[index++] = 0x00;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		dltStart = index;
		buf[index++] = 0x68;
		memcpy(&buf[index], args[0], 6);
		index += 6;
		buf[index++] = 0x68;
		buf[index++] = 0x11;
		buf[index++] = 0x04;
		buf[index++] = 0x33;
		buf[index++] = 0x33;
		buf[index++] = 0x34;
		buf[index++] = 0x33;
		crc8 = GetSum8(&buf[dltStart], index - dltStart);
		buf[index++] = crc8;
		buf[index++] = 0x16;
		break;
	case PowerCmd_ReadMeter_698:
		apsFsn++;
		buf[nwkCmdStart] = 0x0A;	// aps ctrl
		buf[index++] = 0x00;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		buf[index++] = 0xFF;
		dltStart = index;
		buf[index++] = 0x68;	// start
		buf[index++] = 0x17;	// length :  length --> Fcs 
		buf[index++] = 0x00;
		buf[index++] = 0x43;	// ctrl
		buf[index++] = 0x05;	// addr  05 + addr(6 byte) + 00
		memcpy(&buf[index], args[0], 6);
		index += 6;
		buf[index++] = 0x00;
		crc16 = GetCrc16(&buf[dltStart + 1], index - dltStart, CRC16_Seed);
		buf[index++] = (uint8)(crc16 & 0xFF);	// header crc16 : length --> addr
		buf[index++] = (uint8)(crc16 >> 8);
		buf[index++] = 0x05;	// data filed
		buf[index++] = 0x01;
		buf[index++] = 0x01;
		buf[index++] = 0x00;
		buf[index++] = 0x10;
		buf[index++] = 0x02;
		buf[index++] = 0x00;
		buf[index++] = 0x00;
		crc16 = GetCrc16(&buf[dltStart + 1], index - dltStart, CRC16_Seed);
		buf[index++] = (uint8)(crc16 & 0xFF);	// frame crc16 : length --> data filed
		buf[index++] = (uint8)(crc16 >> 8);
		buf[index++] = 0x16;	// end
		break;
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
	case PowerCmd_ReadAllMeterDoc:
		apsFsn++;
		buf[index++] = 0x88;
		buf[index++] = *args[0];	// start index
		buf[index++] = 20;			// read cnt
		break;

	//-------------------------------------------  节点控制		-------------
	
	case PowerCmd_SetSerialCom:			/*	集中器/电表 命令  */
		apsFsn++;
		buf[index++] = 0x00;
		buf[index++] = *args[0];
		buf[index++] = *args[1];
		break;
	case PowerCmd_SetChanelGrp:
		apsFsn++;
		buf[index++] = 0x01;
		buf[index++] = *args[0];
		break;
	case PowerCmd_SetRssiThreshold:
		apsFsn++;
		buf[index++] = 0x02;
		buf[index++] = *args[0];
		break;
	case PowerCmd_SetSendPower:
		apsFsn++;
		buf[index++] = 0x03;
		buf[index++] = *args[0];
		break;
	case PowerCmd_DeviceReboot:
		apsFsn++;
		buf[index++] = 0x05;
		break;
	case PowerCmd_ParamsInit:			/*	集中器 命令  */
		apsFsn++;
		buf[index++] = 0x90;
		break;
	case PowerCmd_StartNwkBuild:
		apsFsn++;
		buf[index++] = 0x91;
		break;
	case PowerCmd_StartNwkMaintain:
		apsFsn++;
		buf[index++] = 0x9A;
		break;
	case PowerCmd_BroadClearNeighbor:
		apsFsn++;
		buf[index++] = 0x97;
		break;
	case PowerCmd_BroadSetSendPower:
		apsFsn++;
		buf[index++] = 0x98;
		buf[index++] = *args[0];
		break;
	case PowerCmd_ClearNeighbor:		/*	电表 命令  */
		apsFsn++;
		buf[index++] = 0x97;
		buf[index++] = 0x00;
		break;
	case PowerCmd_ChangeCollect2Addr:
		apsFsn++;
		buf[index++] = 0x96;
		memcpy(&buf[index], args[0], 6);
		index += 6;
		break;
	case PowerCmd_ForceJoinNwkRequest:
		apsFsn++;
		buf[index++] = 0x99;
		buf[index++] = *args[0];
		break;

	case PowerCmd_ReadReportData:		/*	单向水表 命令  */
		index = nwkCmdStart;
		buf[macCmdStart] = 0x3D;	// nwk ctrl
		buf[index++] = 0x0A;
		buf[index++] = 0x00;
		break;
	case PowerCmd_ClearReportData:
		apsFsn++;
		buf[index++] = 0x97;
		buf[index++] = 0x01;
		break;
	case PowerCmd_QueryBindedWaterMeter:
		index = nwkCmdStart;
		buf[macCmdStart] = 0x3D;	// nwk ctrl
		buf[index++] = 0xFA;
		buf[index++] = 0x00;
		buf[index++] = 0x00;
		break;
	case PowerCmd_AddBindedWaterMeter:
		index = nwkCmdStart;
		buf[macCmdStart] = 0x3D;	// nwk ctrl
		buf[index++] = 0xFA;
		buf[index++] = 0x01;
		memcpy(&buf[index], args[0], 7);
		index += 7;
		break;
	case PowerCmd_DelBindedWaterMeter:
		index = nwkCmdStart;
		buf[macCmdStart] = 0x3D;	// nwk ctrl
		buf[index++] = 0xFA;
		buf[index++] = 0x02;
		memcpy(&buf[index], args[0], 7);
		index += 7;
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
* 函数名：ExplainElectricResponseFrame
* 描  述：解析电力命令响应帧
* 参  数：buf - 接收缓存起始地址
		  srcAddr - 源地址
		  cmdId - 命令字
		  disp - 解析的显示数据
* 返回值：bool 解析结果：fasle - 失败 ， true - 成功
*/
bool ExplainElectricResponseFrame(uint8 * buf, uint16 rxlen, const uint8 * srcAddr, uint8 cmdId, ParamsBuf * disps)
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
	case PowerCmd_ReadMeter_645_97:
		index++;					// skip 0x00
		if(index + 15 < rxlen
			&& buf[index] == 0x68 && buf[index + 7] == 0x68
			&& (buf[index + 8] == 0x81 || buf[index + 8] == 0xA1))
		{
			ret = true;
			
			buf[index + 12] -= 0x33;
			buf[index + 13] -= 0x33;
			buf[index + 14] -= 0x33;
			buf[index + 15] -= 0x33;
			
			disps->cnt = 1;
			GetStringHexFromBytes(&TmpBuf[0], buf, index + 13, 3, 0, true);
			len = StringTrimStart(&TmpBuf[0], '0');
			if(TmpBuf[0] == 0x00){
				TmpBuf[0] = '0';
				TmpBuf[1] = '\0';
			}
			GetStringHexFromBytes(&TmpBuf[20], buf, index + 12, 1, 0, false);

			sprintf(&disps->buf[0], "读数：%s.%s kWh", &TmpBuf[0], &TmpBuf[20]);
			disps->items[0] = &disps->buf[0];
		}
		break;
	case PowerCmd_ReadMeter_645_07:
		index++;					// skip 0x00
		if(index + 17 < rxlen
			&& buf[index] == 0x68 && buf[index + 7] == 0x68
			&& buf[index + 8] == 0x91 && buf[index + 9] == 0x08)
		{
			ret = true;
			
			buf[index + 14] -= 0x33;
			buf[index + 15] -= 0x33;
			buf[index + 16] -= 0x33;
			buf[index + 17] -= 0x33;

			disps->cnt = 1;
			GetStringHexFromBytes(&TmpBuf[0], buf, index + 15, 3, 0, true);
			len = StringTrimStart(&TmpBuf[0], '0');
			if(TmpBuf[0] == 0x00){
				TmpBuf[0] = '0';
				TmpBuf[1] = '\0';
			}
			GetStringHexFromBytes(&TmpBuf[20], buf, index + 14, 1, 0, false);

			sprintf(&disps->buf[0], "读数：%s.%s kWh", &TmpBuf[0], &TmpBuf[20]);
			disps->items[0] = &disps->buf[0];
			
		}
		break;
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
	
	case PowerCmd_ReadNodeInfo:		/*	集中器/电表 命令  */
		if(index + 27 < rxlen && buf[index] == 0x04){

			ret = true;
			disps->cnt = 1;
			sprintf(&disps->buf[0], "%s", &buf[index + 2]);
			disps->items[0] = &disps->buf[0];
		}
		break;
	case PowerCmd_ReadNwkStatus:
		buf[index++] = 0x93;
		break;
	case PowerCmd_ReadSendPower:
		if(index + 1 < rxlen && buf[index] == 0x94){

			ret = true;
			disps->cnt = 1;
			sprintf(&disps->buf[0], "发射功率: %d dBm", buf[index + 1]);
			disps->items[0] = &disps->buf[0];
		}
		break;
	case PowerCmd_ReadVerInfo:
		if(index + 30 < rxlen && buf[index] == 0x95){
			ret = true;
			disps->cnt = 1;
			buf[index + 2 + buf[index + 1] ] = '\0';
			sprintf(&disps->buf[0], "版本信息: %s", &buf[index + 2]);
			disps->items[0] = &disps->buf[0];
		}
		break;
	case PowerCmd_ReadNeighbor:			/*	电表 命令  */
		buf[index++] = 0x10;
		break;
	case PowerCmd_ReadSubNodeRoute:		/*	集中器 命令  */
		buf[index++] = 0x92;
		break;
	case PowerCmd_ReadAllMeterDoc:
		buf[index++] = 0x88;
		break;

	//-------------------------------------------  节点控制		-------------
	
	case PowerCmd_SetSerialCom:			/*	集中器/电表 命令  */
		buf[index++] = 0x00;
		break;
	case PowerCmd_SetChanelGrp:
		buf[index++] = 0x01;
		break;
	case PowerCmd_SetRssiThreshold:
		buf[index++] = 0x02;
		break;
	case PowerCmd_SetSendPower:
		buf[index++] = 0x03;
		break;
	case PowerCmd_DeviceReboot:
		buf[index++] = 0x05;
		break;
	case PowerCmd_ParamsInit:			/*	集中器 命令  */
		buf[index++] = 0x90;
		break;
	case PowerCmd_StartNwkBuild:
		buf[index++] = 0x91;
		break;
	case PowerCmd_StartNwkMaintain:
		buf[index++] = 0x9A;
		break;
	case PowerCmd_BroadClearNeighbor:
		buf[index++] = 0x97;
		break;
	case PowerCmd_BroadSetSendPower:
		buf[index++] = 0x98;
		break;
	case PowerCmd_ClearNeighbor:		/*	电表 命令  */
		buf[index++] = 0x97;
		buf[index++] = 0x00;
		break;
	case PowerCmd_ChangeCollect2Addr:
		buf[index++] = 0x96;
		break;
	case PowerCmd_ForceJoinNwkRequest:
		buf[index++] = 0x99;
		break;

	case PowerCmd_ReadReportData:		/*	单向水表 命令  */
		buf[index++] = 0x0A;
		buf[index++] = 0x00;
		break;
	case PowerCmd_ClearReportData:
		buf[index++] = 0x97;
		buf[index++] = 0x01;
		break;
	case PowerCmd_QueryBindedWaterMeter:
		buf[index++] = 0xFA;
		buf[index++] = 0x00;
		buf[index++] = 0x00;
		break;
	case PowerCmd_AddBindedWaterMeter:
		buf[index++] = 0xFA;
		buf[index++] = 0x01;
		break;
	case PowerCmd_DelBindedWaterMeter:
		buf[index++] = 0xFA;
		buf[index++] = 0x02;
		break;
	
	default:
		break;
	}
	
	return ret;
}


// --------------------------------  参数配置/读取  -----------------------------------------



// --------------------------------  现场调试  -----------------------------------------



// --------------------------------  电力主节点通信  -----------------------------------------

void MainNodeFunc(void)
{
	
}


// --------------------------------  电力子节点通信  -----------------------------------------

void ElectricSubNodeFunc(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 电力子节点通信 ";
	menuList.no = 4;
	menuList.MaxNum = 4;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读取软件版本";
	menuList.str[1] = "  2. 读取节点配置";
	menuList.str[2] = "  3. 读取发射功率";
	menuList.str[3] = "  4. 645-07抄表";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 3 * 16;
	inputBox.width = 7 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(0);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"19200,E,8,1", 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示/输入处理
			ptr = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&ptr[5]);
			_Printfxy(0, 0, TmpBuf, 0);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, 1);	
			_Printfxy(0, 2*16, "地址:", 0);
			_Printfxy(0, 8*16, "返回            确定", 0);

			if(InputBuff_1[0] != '\0'){
				memcpy(inputBox.context, InputBuff_1, 20);
			}
			key = _InputBox(&inputBox);
			if (key == KEY_CANCEL)
				break;

			inputLen = strlen(inputBox.context);

			if(inputLen == 0 && InputBuff_1[0] != '\0'){
				memcpy(inputBox.context, InputBuff_1, 20);
				inputLen = 12;
			}
			if(inputLen == 0 || strncmp(ZeroAddr, inputBox.context, inputLen) == 0){
				_Printfxy(0, 4*16, "请输入有效地址", 0);
				_ReadKey();
				continue;
			}
			StringPadLeft(inputBox.context, 12, '0');
			memcpy(InputBuff_1, inputBox.context, 20);
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, true);
			PrintfXyMultiLine_VaList(0, 2*16, "地址: %s", InputBuff_1);

			// 命令参数处理
			switch(menuItemNo){
			case 1:		// " 读取软件版本 ";
				CurrCmd = PowerCmd_ReadVerInfo;
				/*---------------------------------------------*/
				Args.cnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case 2:		// " 读取节点配置 "
				CurrCmd = PowerCmd_ReadNodeInfo;
				/*---------------------------------------------*/
				Args.cnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case 3:		// " 读取发射功率" "
				CurrCmd = PowerCmd_ReadSendPower;
				/*---------------------------------------------*/
				Args.cnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case 4:		// " 645-07抄表 ";
				CurrCmd = PowerCmd_ReadMeter_645_07;
				/*---------------------------------------------*/
				Args.cnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

				default: 
					break;
			}

			if(key != KEY_ENTER){
			 	if (key == KEY_CANCEL){
					break;
				}else{
					continue;
				}
			}
			
			// 发送 
			TxLen = PackElectricRequestFrame(TxBuf, DstAddr, CurrCmd, Args.items, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "    命令发送...   ", 0);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 100);	// recv , 500ms timeout
			if(false == ExplainElectricResponseFrame(RxBuf, RxLen, DstAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "    失败:%s", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "      命令成功     ", 0);
			}
			
			// 显示结果
			PrintfXyMultiLine(0, 3 * 16, Disps.items[0]);

			key = _ReadKey();

			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// --------------------------------  水力子节点通信  -----------------------------------------

// 1	常用命令
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 水表常用命令";
	menuList.no = 4;
	menuList.MaxNum = 4;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读取用户用量";
	menuList.str[1] = "  2. 读取冻结正转数据";
	menuList.str[2] = "  3. 开阀";
	menuList.str[3] = "  4. 强制开阀";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 3 * 16;
	inputBox.width = 7 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 14;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(0);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"19200,E,8,1", 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示/输入处理
			ptr = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&ptr[5]);
			_Printfxy(0, 0, TmpBuf, 0);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, 1);	
			_Printfxy(0, 2*16, "地址:", 0);
			_Printfxy(0, 8*16, "返回            确定", 0);

			if(InputBuff_1[0] != '\0'){
				memcpy(inputBox.context, InputBuff_1, 20);
			}
			key = _InputBox(&inputBox);
			if (key == KEY_CANCEL)
				break;

			inputLen = strlen(inputBox.context);

			if(inputLen == 0 && InputBuff_1[0] != '\0'){
				memcpy(inputBox.context, InputBuff_1, 20);
				inputLen = 14;
			}
			if(inputLen == 0 || strncmp(ZeroAddr, inputBox.context, inputLen) == 0){
				_Printfxy(0, 4*16, "请输入有效地址", 0);
				_ReadKey();
				continue;
			}
			StringPadLeft(inputBox.context, 14, '0');
			memcpy(InputBuff_1, inputBox.context, 20);
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, true);
			PrintfXyMultiLine_VaList(0, 2*16, "地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				Args.cnt = 1;
				memcpy(Args.buf, DstAddr, 7);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_ReadFrozenData:		// " 读取节点配置 "
				/*---------------------------------------------*/
				Args.cnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:		// " 读取发射功率" "
				/*---------------------------------------------*/
				Args.cnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// " 645-07抄表 ";
				/*---------------------------------------------*/
				Args.cnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

				default: 
					break;
			}

			if(key != KEY_ENTER){
			 	if (key == KEY_CANCEL){
					break;
				}else{
					continue;
				}
			}
			
			// 发送 
			TxLen = PackElectricRequestFrame(TxBuf, DstAddr, CurrCmd, Args.items, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "    命令发送...   ", 0);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 100);	// recv , 500ms timeout
			if(false == ExplainElectricResponseFrame(RxBuf, RxLen, DstAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "    失败:%s", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "      命令成功     ", 0);
			}
			
			// 显示结果
			PrintfXyMultiLine(0, 3 * 16, Disps.items[0]);

			key = _ReadKey();

			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 2	测试命令
void WaterCmdFunc_TestCmd(void)
{

}

// 3	程序升级
void WaterCmdFunc_Upgrade(void)
{

}

// 4	预缴用量
void WaterCmdFunc_PrepaiedVal(void)
{

}

// 5	工作参数
void WaterCmdFunc_WorkingParams(void)
{

}

// 6	其他操作
void WaterCmdFunc_Other(void)
{

}

void WaterCmdFunc(void)
{
	_GuiMenuStru menu;
	
	menu.left=0;
	menu.top=0;
	menu.no=6;
	menu.title= "<< 水表通信 ";
	menu.str[0]=" 集中器通信 ";
	menu.str[1]=" 水表通信 ";
	menu.str[2]=" 电表通信 ";
	menu.str[3]=" 透传模块升级 ";
	menu.str[4]=" 电表通信 ";
	menu.str[5]=" 透传模块升级 ";
	menu.key[0]="1";
	menu.key[1]="2";
	menu.key[2]="3";
	menu.key[3]="4";
	menu.key[4]="5";
	menu.key[5]="6";
	menu.Function[0]=WaterCmdFunc_CommonCmd;
	menu.Function[1]=WaterCmdFunc_TestCmd;
	menu.Function[2]=WaterCmdFunc_Upgrade;
	menu.Function[3]=WaterCmdFunc_PrepaiedVal;
	menu.Function[4]=WaterCmdFunc_WorkingParams;
	menu.Function[5]=WaterCmdFunc_Other;
	menu.FunctionEx=0;
	_Menu(&menu);	
}

// --------------------------------  透传模块设置  -----------------------------------------
void TransParentModuleFunc(void)
{
	uint8 key, menuItemNo, tryCnt = 0;
	_GuiLisStruEx menuList;
	char *fileName = NULL;
	char tmp[70];
	int fileHdl, fileLen, totalCnt, sendCnt;
	int index;
	
	_ClearScreen();

	// 菜单
	menuList.title = "<< 透传模块升级 ";
	menuList.no = 3;
	menuList.MaxNum = 3;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 查看当前版本";
	menuList.str[1] = "  2. 打开升级文件";
	menuList.str[2] = "  3. 开始升级";
	menuList.defbar = 1;
	_GUIHLine(0, 4*16 + 8, 160, 1);

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"19200,E,8,1", 2);

	while(1){

		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}

		menuList.defbar = menuItemNo;

		switch(menuItemNo){
		case 1:	// " 查看当前版本 ";
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			TxLen = 0;
			TxBuf[TxLen++] = 0xAA;
			TxBuf[TxLen++] = 0xBB;
			TxBuf[TxLen++] = 0x01;
			TxBuf[TxLen++] = 0x07;
			TxBuf[TxLen++] = 0xCC;
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 5*16, "查询中...", 0);

			sprintf(TxBuf, "当前版本:");
			RxLen = _GetComStr(&TxBuf[9], 50, 50);	// recv , 500ms timeout
			if(RxLen < 30 || strncmp(&TxBuf[9], "SRWF-", 5) != 0)
			{
				_Printfxy(0, 5*16, "接收超时", 0);
				break;
			}
			_Printfxy(0, 5*16, &TxBuf[0], 0);
			_Printfxy(0, 6*16, &TxBuf[20], 0);
			_Printfxy(0, 7*16, &TxBuf[40], 0);
			break;

		case 2:	// " 打开升级文件 "
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);

			_SaveScreenToBuff(Screenbuff);
			_ClearScreen();
			fileName = _GetFileList("选|\n择|\n升|\n级|\n文|\n件|\n  |\n  |\n", "", "");
			_ClearScreen();
			_RestoreBuffToScreen(Screenbuff);

			if (fileName == NULL){
				break;
			}
			
			sprintf(tmp, "文件: %s\0", fileName);
			_Printfxy(0, 5*16, &tmp[0], 0);
			_Printfxy(0, 6*16, &tmp[20], 0);

			fileHdl = _Fopen(fileName, "R");
			fileLen = _Filelenth(fileHdl);
			totalCnt = (fileLen + 1023)/1024;
			sendCnt = 0;
			_Fread(TxBuf, 1024, fileHdl);
			_Fclose(fileHdl);
				
			index = IndexOf(TxBuf, 1024, "SRWF-", 5, 512, 512);
			if(index < 0){
				_Printfxy(0, 7*16, "不是4E88-APP文件", 0);
				fileName = NULL;
			}
			else{
				sprintf(tmp, "大小:%dK,总包数:%d\0", fileLen/1024, totalCnt);
				_Printfxy(0, 7*16, &tmp[0], 0);
			}
			break;
			
		case 3:	// " 开始升级 ";
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			totalCnt = 200;
			
			// 初始化
			if (fileName == NULL){
				_Printfxy(0, 5*16, "请先选择升级文件", 0);
				break;
			}
			fileHdl = _Fopen(fileName, "R");
			sendCnt = 0;

			sprintf(tmp, "总包数: %d\0", totalCnt);
			_Printfxy(0, 5*16, &tmp[0], 0);
			sprintf(tmp, "正发送: %d   \0",sendCnt);
			_Printfxy(0, 6*16, &tmp[0], 0);
			_Printfxy(0, 9*16, "状态: 升级中...", 0);

			ShowProgressBar(7*16+8, totalCnt, sendCnt);

			// 升级进度
			while(1){

				if(tryCnt > 3 || sendCnt >= totalCnt){
					break;
				}
				
				TxLen = _Fread(TxBuf, 1024, fileHdl);
				_GetComStr(RxBuf, 1024, 1);		// clear , 100ms timeout
				_SendComStr(TxBuf, TxLen);

				sprintf(tmp, "正发送: %d   \0",sendCnt + 1);
				_Printfxy(0, 6*16, &tmp[0], 0);
				if(tryCnt > 0){
					sprintf(tmp, "重试%d \0",tryCnt);
					_Printfxy(6*16, 6*16, &tmp[0], 0);
				}
				tryCnt++;

				RxLen = _GetComStr(&TxBuf[9], 50, 1);	// recv , 500ms timeout
				if(RxLen < 10){
				//	continue;
				}

				sendCnt++;
				tryCnt = 0;
				ShowProgressBar(7*16+8, totalCnt, sendCnt);

			}
			_Fclose(fileHdl);

			// 升级完成
			if(tryCnt > 3){
				_Printfxy(0, 9*16, "状态: 升级失败  ", 0);
			}else{
				_Printfxy(0, 9*16, "状态: 升级完成  ", 0);
			}
			_SoundOn();
			_Sleep(500);
			_SoundOff();
			_Sleep(300);
			_SoundOn();
			_Sleep(500);
			_SoundOff();
			break;

			default: 
				break;
		}
	}

	_CloseCom();
}


// --------------------------------   主函数   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	
	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=4;
	MainMenu.title= "   桑锐手持机  ";
	MainMenu.str[0]=" 集中器通信 ";
	MainMenu.str[1]=" 水表通信 ";
	MainMenu.str[2]=" 电表通信 ";
	MainMenu.str[3]=" 透传模块升级 ";
	MainMenu.key[0]="1";
	MainMenu.key[1]="2";
	MainMenu.key[2]="3";
	MainMenu.key[3]="4";
	MainMenu.Function[0]=MainNodeFunc;
	MainMenu.Function[1]=WaterCmdFunc;
	MainMenu.Function[2]=ElectricSubNodeFunc;
	MainMenu.Function[3]=TransParentModuleFunc;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	
}

