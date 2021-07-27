/*
*
*	桑锐6009水表手持机 - 红外版 <北京NB-Iot>
*
*/
#ifndef Protocol_NBiot_BJ
#include "A3_N609HX.h"
#endif

#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "common.h"
#include "common.c"
#include "WaterMeter.h"
#include "ProtoHandle_NBiot_BJ.h"
#include "ProtoHandle_DH_IR.h"


// --------------------------------  水表模块通信  -----------------------------------------

//-----------------------------------	主菜单	---------------------------
// 读IMEI+ICCID
void MainFuncReadImeiAndCcid(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;
	#if UseBroadAddr
	char strDstAddrBak[20];
	#endif

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<读取IMEI+ICCID");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;

			#if UseBroadAddr
			// 读取IMEI+ICCID 时，使用D4D4...地址
			memcpy(strDstAddrBak, StrDstAddr, 20);
			sprintf(StrDstAddr, "D4D4D4D4D4D4D4D4");	// 初始值为广播地址
			#endif

			#if (AddrLen == 6)
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);
			#else
			LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:");
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);	
			#endif
		}
			
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = WaterCmd_ReadImeiAndCcid;		// 读取IMEI+ICCID
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x0F;		// 命令字	0F
			ackLen = 22;				// 应答长度 12	
			// 数据域
			Args.buf[i++] = 0;			// 命令选项	0-读取IMEI+ICCID，1-设置IMEI， 2-设置ICCID
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if( 
				#if UseBroadAddr
				strcmp(StrDstAddr, "D4D4D4D4D4D4D4D4") != 0 && 
				#endif
				(StrDstAddr[0] < '0' || StrDstAddr[0] > '9')
			){
				sprintf(StrDstAddr, " 请输入");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

		// 发送、接收、结果显示
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
	
		#if UseBroadAddr
		if(StrDstAddr[0] == 'D'){
			memcpy(StrDstAddr, strDstAddrBak, 20);
		}
		#endif

		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
	#if UseBroadAddr
	if(StrDstAddr[0] == 'D'){
		memcpy(StrDstAddr, strDstAddrBak, 20);
	}
	#endif
}

// 读取北京水表参数
void MainFuncReadBeijingWaterMeterParams(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<读取北京水表参数");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;

			#if (AddrLen == 6)
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);
			#else
			LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:");
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);	
			#endif
		}
			
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = WaterCmd_ReadBeiJingWaterMeterParams;		// 读取北京水表参数
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x26;		// 命令字	26
			ackLen = 69;				// 应答长度 69
			// 数据域
			Args.buf[i++] = 0x00;		// 命令选项 00 - 读取， 01 - 设置		
			Args.lastItemLen = i - 1; 
			break;

		default: 
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if( StrDstAddr[0] < '0' || StrDstAddr[0] > '9')
			{
				sprintf(StrDstAddr, " 请输入");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

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

// 读取用户用量 rename --> 读自定义数据
void MainFuncReadRealTimeData(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<读自定义数据");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;

			#if (AddrLen == 6)
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);
			#else
			LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:");
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);	
			#endif
		}
			
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:	
			CurrCmd = WaterCmd_ReadRealTimeData;	// 读自定义数据 （即原来的 “读取用户用量”）
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 21;				// 应答长度 21	
			// 数据域
			Args.buf[i++] = 0x00;				// 数据格式 00	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if( StrDstAddr[0] < '0' || StrDstAddr[0] > '9')
			{
				sprintf(StrDstAddr, " 请输入");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

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

// NB上报实时数据
void MainFuncNbReportRealTimeData(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<NB上报实时数据");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;

			#if (AddrLen == 6)
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);
			#else
			LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:");
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);	
			#endif
		}
			
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:	
			CurrCmd = WaterCmd_NbReportRealTimeDataNow;		// NB上报实时数据
			if(false == isUiFinish){
				break;
			}
			i = 0;
			Args.buf[i++] = 0x21;		// 命令字	21
			ackLen = 1;					// 应答长度 1	
			// 数据域
			Args.buf[i++] = 0x01;		// 命令选项：1-上报实时数据
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if( StrDstAddr[0] < '0' || StrDstAddr[0] > '9')
			{
				sprintf(StrDstAddr, " 请输入");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

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

// 读自定义数据
void MainFuncReadCustomData(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<读自定义数据");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回            执行", Color_White);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_ReadCustomData;	// 读自定义数据
			/*---------------------------------------------*/
			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// 命令字
			ackLen = 17;						// 应答长度	
			// 数据域
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-数据透传
			Args.buf[i++] = 2;					// len: 1 + 1 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0x00;				// dataId: 0-读自定义数据
			Args.buf[i++] = 0x00;	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}
				
		// 发送、接收、结果显示
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}
}

// 读为峰模块数据
void MainFuncReadWeiFengModuleDatas(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<读为峰模块数据");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回            执行", Color_White);

		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_ReadWeiFengModuleData;		// 读取为峰模块数据
			/*---------------------------------------------*/
			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// 命令字
			ackLen = 23;						// 应答长度	
			// 数据域
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-数据透传
			Args.buf[i++] = 2;					// len: 1 + 1 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 82;				// dataId: 82- 0x52 读取为峰模块数据
			Args.buf[i++] = 0x80;	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}
				
		// 发送、接收、结果显示
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}
}

// 设置表地址
void MainFuncSetMeterAddr(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<设置表地址");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:");
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrBuf[0], 7*2, (7*2*8 + 8), true);	
		}
			
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		

		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_SetMeterAddr;		// 设置表地址
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			
			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// 命令字	
			ackLen = 81;						// 应答长度 	
			// 数据域
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-数据透传
			Args.buf[i++] = 8;					// len: 1 + 7 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 53;					// dataId: 53-0x35 设表地址

			GetBytesFromStringHex(&TmpBuf[0], 0, 7, StrBuf[0], 0, true);
			memcpy(&Args.buf[i], &TmpBuf[0], 7);
			i += 7;
			Args.lastItemLen = i - 1;
			break;

		default:
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " 请输入");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}


		// 发送、接收、结果显示
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// 设置初值
void MainFuncSetMeterInitValue(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;
	uint32 u32Tmp;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<设置初值");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
		}
			
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_SetInitValue;		// 设置初值
			/*---------------------------------------------*/
			if(false == isUiFinish){
				if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
					StrBuf[1][0] = 0x01;
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 1 * 20);
				}
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "正向用量:", StrBuf[0], 10, 11*8, true);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "反向用量: 默认为0");
				break;
			}
			if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
				sprintf(StrBuf[0], " 请输入");
				currUi = uiRowIdx - 2 - 2;
				isUiFinish = false;
				continue;
			}

			memcpy(&BackupBuf[ArgIdx_MtrValPalse], &StrBuf[0][0], 1 * 20);
			BackupBuf[ArgIdx_MtrValPalse - 1] = Param_Unique;

			u32Tmp = (uint32) _atof(StrBuf[0]);

			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// 命令字
			ackLen = 81;						// 应答长度	
			// 数据域
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-数据透传
			Args.buf[i++] = 9;					// len: 1 + 8 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 52;					// dataId: 52-0x34  设表底数
			Args.buf[i++] = (uint8)(u32Tmp & 0xFF);			// 正向用量	
			Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
			Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
			Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
			Args.buf[i++] = 0;								// 反向用量
			Args.buf[i++] = 0;
			Args.buf[i++] = 0;
			Args.buf[i++] = 0;
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 发送、接收、结果显示
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// 设置上报周期
void MainFuncSetReportCycle(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i, u8Tmp;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<设置上报周期");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
		}
			
		// 命令参数处理
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // 命令字
		Args.items[1] = &Args.buf[1];	// 数据域
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:	
			CurrCmd = UserCmd_SetPeriodReportFreq;
			/*---------------------------------------------*/
			if(false == isUiFinish){
				if(BackupBuf[ArgIdx_FixTimeVal -1] != Param_Unique){		
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_FixTimeVal], 1 * 20);
				}
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "上报频率(h):", StrBuf[0], 2, 3*8, true);
				break;
			}
			if(StrBuf[0][0] > '9' || StrBuf[0][0] < '1'){
				sprintf(StrBuf[0], " 请输入");
				currUi = uiRowIdx - 2 - 2;
				isUiFinish = false;
				continue;
			}

			memcpy(&BackupBuf[ArgIdx_FixTimeVal], &StrBuf[0][0], 1 * 20);
			BackupBuf[ArgIdx_FixTimeVal - 1] = Param_Unique;

			u8Tmp = (uint8) _atof(StrBuf[0]);

			if(u8Tmp > 24){
				sprintf(StrBuf[0], " 请输入");
				currUi = uiRowIdx - 2 - 2;
				isUiFinish = false;
				continue;
			}

			i = 0;
			Args.buf[i++] = Func_SetInternalParam;	// 命令字
			ackLen = 13;						// 应答长度	
			// 数据域
			Args.buf[i++] = Func_SetInternalParam;	// 命令字
			Args.buf[i++] = 0;		// 密码
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 1;		// 参数个数
			Args.buf[i++] = Tag_PeriodReportFreq;	// 参数序号
			Args.buf[i++] = 1;		// 参数长度
			Args.buf[i++] = u8Tmp;	// 参数值			
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
	
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 发送、接收、结果显示
		key = DonghaiIR_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// 继续 / 返回
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}


// --------------------------------   主函数   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	
	MeterNoLoad(StrDstAddr);
	SysCfgLoad();

	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=8;
	MainMenu.title =  VerInfo_Name;
	MainMenu.str[0] = "读取IMEI+CCID";
	MainMenu.str[1] = "读取北京水表参数";
	MainMenu.str[2] = "读自定义数据";
	MainMenu.str[3] = "设置表地址";
	MainMenu.str[4] = "设置初值";
    MainMenu.str[5] = "设置上报周期";
	MainMenu.str[6] = "读为峰模块数据";
	MainMenu.str[7] = "NB上报实时数据";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.key[7] = "8";
	MainMenu.Function[0] = MainFuncReadImeiAndCcid;
	MainMenu.Function[1] = MainFuncReadBeijingWaterMeterParams;
	MainMenu.Function[2] = MainFuncReadRealTimeData;
	MainMenu.Function[3] = MainFuncSetMeterAddr;
	MainMenu.Function[4] = MainFuncSetMeterInitValue;
    MainMenu.Function[5] = MainFuncSetReportCycle;
	MainMenu.Function[6] = MainFuncReadWeiFengModuleDatas;
	MainMenu.Function[7] = MainFuncNbReportRealTimeData;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();
}

