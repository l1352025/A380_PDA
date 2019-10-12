#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "Upgrade.h"
#include "Common.h"
#include "WaterMeter.h"

void Func_Upgrade(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	//ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	//uint8 * pByte;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// 菜单
	(*pUiCnt) = 0;
	uiRowIdx = 2;
	_Printfxy(0, 0, "<<程序升级", Color_White);
	_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
	/*---------------------------------------------*/
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "1. 选择升级文件");	
	LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "2. 输入表号");	
	TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "  > ", StrBuf[0], 1, 2*8, true);
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "3. 开始升级");	
	ButtonCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "4. 查询升级状态");				
	//----------------------------------------------
	_GUIHLine(0, 9*16 - 4, 160, Color_Black);
	_Printfxy(0, 9*16, "返回            确定", Color_White);

	while(1){

		_Printfxy(0, 0, "<<程序升级", Color_White);
		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowUI(UiList, &currUi);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}

		_MessageBoxEx("", "unavailable now", MSG_OK);
		_Sleep(1000);
		return;

		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		switch (currUi)
		{
		case 0:	// 选择升级文件
			{


			}
			break;

		case 2:	// 输入表号
		default:
			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
				sprintf(StrDstAddr, " 请输入");
				currUi = 2;
				continue;
			}

			break;

		case 3:	// 开始升级
			{

			}
			break;

		case 4:	// 查询升级状态
			{

			}
			break;
		}

		while(1){
			
			_ClearScreen();

			_Printfxy(0, 0, "<<开始升级", Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			PrintfXyMultiLine_VaList(0, 2*16, "版本：%s  CRC：%s");
			PrintfXyMultiLine_VaList(0, 3*16, "表号：\r\n  %s");
			PrintfXyMultiLine_VaList(0, 5*16, "%s");	// cmd name
			PrintfXyMultiLine_VaList(0, 6*16, "%s");	// cmd reponse
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "返回   <升级中>   ", Color_White);

			
			// 命令参数处理
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x30 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_NoticeUpgrade_OnApp:		// 通知系统升级_在app
				/*---------------------------------------------*/
                Args.buf[i++] = 0x70;		// 命令字	70
				ackLen = 41;				// 应答长度 41	
				// 数据域
				Args.buf[i++] = 0x00;		// 水表电压
				Args.buf[i++] = 0x00;		// 气表电压
				Args.buf[i++] = 0x00;		// RSSI门限
				Args.buf[i++] = 0x00;		// SNR门限
				Args.buf[i++] = 0x00;		// 程序版本 40 byte
				Args.buf[i++] = 0x00;		// 版本CRC16
				Args.buf[i++] = 0x00;		
				Args.buf[i++] = 0x00;		// 总包数
				Args.buf[i++] = 0x00;
				Args.buf[i++] = 0x00;		// 前26K CRC16
				Args.buf[i++] = 0x00;	
				Args.buf[i++] = 0x00;		// 后26K CRC16
				Args.buf[i++] = 0x00;	
				Args.buf[i++] = 0x00;		// 总52K CRC16
				Args.buf[i++] = 0x00;	
				Args.buf[i++] = 0x00;		// RXD信道 0/1
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_NoticeUpgrade_OnBoot:		// 通知系统升级_在boot
				/*---------------------------------------------*/
				Args.buf[i++] = 0x71;		// 命令字	71
				ackLen = 41;				// 应答长度 41	
				// 数据域
				Args.buf[i++] = 0x00;	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SendUpgradePacket:			// 发送升级数据
				/*---------------------------------------------*/
				Args.buf[i++] = 0x72;		// 命令字	72
				ackLen = 0;					// 应答长度 0	
				// 数据域
				Args.buf[i++] = 0x00;	
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_QueryUpgradeStatus_OnBoot:	// 查询升级状态_在boot
				/*---------------------------------------------*/
				Args.buf[i++] = 0x73;		// 命令字	73
				ackLen = 93;				// 应答长度 93	
				// 数据域
				Args.buf[i++] = 0x00;	
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_QueryUpgradeStatus_OnApp:		// 查询升级状态_在app
				/*---------------------------------------------*/
				Args.buf[i++] = 0x74;		// 命令字	74
				ackLen = 41;				// 应答长度 41	
				// 数据域
				Args.buf[i++] = 0x00;	
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen == 6)
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
			#endif

			// 应答长度、超时时间、重发次数
			ackLen += 14 + Addrs.itemCnt * AddrLen;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// 发送、接收、结果显示
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				break;
			}else{
				isUiFinish = false;
				continue;
			}
		}
		
	}

}

AppFileInfo AppInfo;
PacketInfo PktInfo;
UpgradeDocs UpgrdDocs;

/*
* 描  述：初始化数据包结构
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.all）
*		fileName - 文件名
*		pktSize - 分包大小
*		pktStartIdx - 数据包在文件中的起始位置
* 返回值：int  - 数据包初始化结果 0 - 成功， 1 - 文件不存在， 2 - 其他错误
*/
extern int InitPktInfo(PacketInfo *pktInfo, char *fileName, uint16 pktSize, uint32 pktStartIdx)
{
	int fp;
	uint32 fileLen, readLen;
	uint16 crc16, crc16Keep = 0xFFFF;
	uint8 *tmpBuf = &TmpBuf[0];

	if(_Access(fileName, 0) < 0){
		return 1;
	}

	fp = _Fopen(fileName, "R");
	fileLen = _Filelenth(fp);
	_Fread((uint8 *)AppInfo, sizeof(AppInfo), fp);
	_Lseek(fp, pktStartIdx, 0);
	while(0 == _Feof(fp)){
		_Fread(tmpBuf, 1024, fp);
		crc16 = GetCrc16_Continue(tmpBuf, readLen, 0x8408, &crc16Keep);
	}
	_Fclose(fp);

	if(fileLen <= pktStartIdx){
		return 2;
	}

	pktInfo->fileName = fileName;
	pktInfo->packetSize = pktSize;
	pktInfo->fileSize = fileLen - pktStartIdx;
	pktInfo->fileKbSize = (uint16)((pktInfo->fileSize + 1023) / 1024);
	pktInfo->packetCnt = (pktInfo->fileSize + pktInfo->packetSize - 1) / pktInfo->packetSize;
	if(pktInfo->fileSize % packetSize != 0){
		pktInfo->lastPktSize = pktInfo->fileSize % packetSize;
	}else{
		pktInfo->lastPktSize = packetSize;
	}



	return 0;
}

/*
* 描  述：拷贝数据包到缓存
* 参  数：
*		pktInfo	- 数据包信息结构 （get）
*		pktIdx - 要拷贝的数据包序号
*		buf - 目的缓存
* 返回值：void
*/
extern int  CopyPktToBuf(PacketInfo *pktInfo, uint16 pktIdx, uint8 *buf)
{

}

/*
* 描  述：清除缺包标记
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.bitFlags）
* 返回值：void
*/
extern void ClearMissPktFlags(PacketInfo *pktInfo)
{

}

/*
* 描  述：添加缺包标记
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.bitFlags）
*		bitflags - 当前缺包标记数据缓存，添加/合并到总标记中
*		byteCnt - 当前缺包标记字节数
* 返回值：void
*/
extern void AddMissPktFlags(PacketInfo *pktInfo, uint8 *bitflags, uint16 byteCnt)
{

}

/*
* 描  述：获取缺包列表
* 参  数：
*		pktInfo	- 数据包信息结构 （set pktInfo.missCnt，set pktInfo.missList）
* 返回值：void
*/
extern void GetMissPktList(PacketInfo *pktInfo)
{

}

