/*
*
*	桑锐6009水表手持机 - 红外版
*
*/
#include "SR6009_IR.h"

#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "Common.h"
#include "Tool.h"
#include "WaterMeter.h"
#include "Upgrade.h"
#include "Upgrade.c"
#include "List.h"
#include "List.c"


// 输入参数备份类型： 保存在备份缓存的第一个字节 BackupBuf[0]
typedef enum{
	Param_BeijingWMtr = 0x11	// 北京水表参数备份
}BackUpParamType;

// --------------------------------  水表模块通信  -----------------------------------------

// 1	常用命令
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;
	#if UseBroadAddr
	char strDstAddrBak[20];
	#endif

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
		"<<常用命令",
		6,
		"1. 读累计用量",
		"2. 读冻结数据",
		"3. 设用量和脉冲系数",
		"4. 开阀",
		"5. 关阀",
		"6. 清异常"
	);

	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		menuItemNo = menuList.strIdx + 1;

		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			CurrCmdName = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
		 	//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;

				#if UseBroadAddr
				if(menuItemNo == 1){ // 读用量时
					memcpy(strDstAddrBak, StrDstAddr, 20);
					sprintf(StrDstAddr, "D4D4D4D4D4D4D4D4");	// 初始值为广播地址
				}
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

			switch(menuItemNo){
			case 1: 
				CurrCmd = WaterCmd_ReadRealTimeData;		// "读取用户用量"
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

			case 2: 
				CurrCmd = WaterCmd_ReadFrozenData;		// "读取冻结正转数据"
				/*---------------------------------------------*/
				if(false == isUiFinish){
					sprintf(StrBuf[0], "0 (0-9有效)");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "序 号:", StrBuf[0], 1, 2*8, true);
					break;
				}
				
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x02;		// 命令字	02
				ackLen = 114;				// 应答长度 88/114	
				// 数据域
				Args.buf[i++] = 0x01;				// 数据格式 01/02
				Args.buf[i++] = _GetYear()/100;		// 时间 - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = _GetYear()%100;		
				Args.buf[i++] = _GetMonth();		
				Args.buf[i++] = _GetDay();			
				Args.buf[i++] = _GetHour();			
				Args.buf[i++] = _GetMin();			
				Args.buf[i++] = _GetSec();			
				Args.buf[i++] = StrBuf[0][0] - '0';	// 冻结数据序号	
				Args.lastItemLen = i - 1;
				break;

			case 3: 
				CurrCmd = WaterCmd_SetBaseValPulseRatio;	// 设表底数脉冲系数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(StrBuf[1][0] == 0x00){
						StrBuf[1][0] = 0x01;
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "用户用量:", StrBuf[0], 10, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "脉冲系数:", &StrBuf[1][0], 4, 
						"1", "10", "100", "1000");
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}

				Args.buf[i++] = 0x06;		// 命令字	06
				ackLen = 7;					// 应答长度 7	
				// 数据域
				u32Tmp = (uint32) _atof(StrBuf[0]);
				u16Tmp = (uint16)((float)((_atof(StrBuf[0]) - u32Tmp)*1000.0));
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// 用户用量	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)StrBuf[1][0];		// 脉冲系数	
				Args.lastItemLen = i - 1;
				break;

			case 4: 
				CurrCmd = WaterCmd_OpenValve;			// " 开阀 "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

            case 5: 
				CurrCmd = WaterCmd_CloseValve;			// " 关阀 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

			case 6: 
				CurrCmd = WaterCmd_ClearException;		// " 清异常命令 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 1;					// 应答长度 1	
				// 数据域
				Args.buf[i++] = 0x00;		// 命令选项 00	
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

				if( strcmp(StrDstAddr, "D4D4D4D4D4D4D4D4") != 0 
					&& (StrDstAddr[0] < '0' || StrDstAddr[0] > '9'))
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
			
			#if UseBroadAddr
			if(menuItemNo == 1 && StrDstAddr[0] == 'D'){
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
		if(menuItemNo == 1 && StrDstAddr[0] == 'D'){
			memcpy(StrDstAddr, strDstAddrBak, 20);
		}
		#endif
	}
}

// 2	测试命令
void WaterCmdFunc_TestCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
		"<<测试命令",
		6,
		"1. 读debug信息",
		"2. 清debug信息",
		"3. 表端重启",
		"4. 读表温度",
		"5. 读表电压",
		"6. 设置阀控参数"
	);


	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		menuItemNo = menuList.strIdx + 1;

		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			CurrCmdName = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
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

			switch(menuItemNo){
			case 1: 
				CurrCmd = WaterCmd_ReadDebugInfo;		// 读debug信息
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 58;				// 应答长度 58	
				// 数据域
				Args.buf[i++] = 0x01;		// 命令选项 01	
				Args.lastItemLen = i - 1;
				break;

			case 2: 
				CurrCmd = WaterCmd_ClearDebugInfo;		// 清debug信息
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x02;		// 命令选项 05	
				Args.lastItemLen = i - 1;
				break;

			case 3: 
				CurrCmd = WaterCmd_RebootDevice;			// "表端重启"
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x04;		// 命令选项 04	
				Args.lastItemLen = i - 1;
				break;

			case 4: 
				CurrCmd = WaterCmd_ReadTemperature;			// " 读表温度 "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x05;		// 命令选项 05	
				Args.lastItemLen = i - 1;
				break;

			case 5: 
				CurrCmd = WaterCmd_ReadVoltage;				// " 读表电压 "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x07;		// 命令选项 07	
				Args.lastItemLen = i - 1;
				break;
			
            case 6: 
				CurrCmd = WaterCmd_SetOverCurrentTimeout;		// " 设置阀控参数 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
                    _Printfxy(7*16, (uiRowIdx)*16, "mA", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "过流电流:", StrBuf[0], 3, 5*8, true);
					_Printfxy(7*16, (uiRowIdx)*16, "ms", Color_White);
                    TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "超时时间:", StrBuf[1], 5, 5*8, true);
					break;
				}

				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], "     ");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], "     ");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}

				u16Tmp = (uint16)_atof(StrBuf[0]);
				u32Tmp = (uint32)_atof(StrBuf[1]);
				if(u16Tmp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				if(u32Tmp > 65535){
					sprintf(StrBuf[1], " 0-65535");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x09;		// 命令选项 09
				Args.buf[i++] = (uint8)u16Tmp;			// 过流电流 0~255	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// 超时时间 0~65535	
				Args.buf[i++] = (uint8)(u32Tmp >> 8);
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}

			if (key == KEY_CANCEL){
				key = KEY_NOHOOK;
				break;
			}


			// 创建 “中继地址输入框” 后， 显示UI
			if(false == isUiFinish){
			
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					key = KEY_NOHOOK;
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
				key = KEY_NOHOOK;
				break;
			}else{
				isUiFinish = false;
				continue;
			}
		}
		
	}
}

// 3	程序升级
void WaterCmdFunc_Upgrade(void)
{
	UpgradeFunc();
}

// 4	预缴用量
void WaterCmdFunc_PrepaiedVal(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;

	_ClearScreen();

	// 菜单`
	ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
		"<<预缴用量",
		6,
		"1. 读预缴参考用量",
		"2. 设预缴参考用量",
		"3. 读报警关阀限值",
		"4. 设报警限值",
		"5. 设关阀限值",
		"6. 设报警关阀限值"
	);

	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		menuItemNo = menuList.strIdx + 1;

		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			CurrCmdName = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
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
			CurrCmd = (0x40 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadPrepaidRefVal:		// "读预缴参考用量"
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x15;		// 命令字	15
				ackLen = 12;				// 应答长度 12	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetPrepaidRefVal:		// " 设预缴参考用量 "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "预缴用量:", StrBuf[0], 10, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
                    TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "参考用量:", StrBuf[1], 10, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " 请输入");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}

				Args.buf[i++] = 0x16;		// 命令字	16
				ackLen = 2;					// 应答长度 2	
				// 数据域
				u32Tmp = (uint32)_atof(StrBuf[0]);
				u16Tmp = (uint16)((float)((_atof(StrBuf[0]) - u32Tmp)*1000.0));
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// 预缴用量	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				u32Tmp = (uint32)_atof(StrBuf[1]);
				u16Tmp = (uint16)((float)((_atof(StrBuf[1]) - u32Tmp)*1000.0));
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// 参考起始用量	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadAlarmLimitOverdraft:	// " 读报警限值透支 "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x17;		// 命令字	17
				ackLen = 4;					// 应答长度 4	
				break;
			
			case WaterCmd_SetAlarmLimit:		// " 设报警限值 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "报警限值:", StrBuf[0], 3, 11*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}

				u16Tmp = (uint16)_atof(StrBuf[0]);
				if(u16Tmp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUi = uiRowIdx - 2 - 1;
                    isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x18;		// 命令字	18
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x00;		// 命令选项：0- 报警限值 1-关阀限值  2-报警限值+关阀限值	
				Args.buf[i++] = (uint8)u16Tmp;		// 报警限值
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_SetCloseValveLimit:		// " 设关阀限值 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "关阀限值:", StrBuf[0], 5, 11*8, true);
					break;
				}
				if(StrBuf[0][0] == 0x00){
					sprintf(StrBuf[0], " 请输入");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				u32Tmp = (uint32)_atof(StrBuf[0]);
				if(u32Tmp > 65535){
					sprintf(StrBuf[0], " 0-65535");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x18;		// 命令字	18
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x01;		// 命令选项：0- 报警限值 1-关阀限值  2-报警限值+关阀限值	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// 关阀限值
				Args.buf[i++] = (uint8)(u32Tmp >> 8);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetAlarmAndCloseValveLimit:		// " 设报警关阀限值 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "报警限值:", StrBuf[0], 3, 11*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "关阀限值:", StrBuf[1], 5, 11*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " 请输入");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				u16Tmp = (uint16) _atof(StrBuf[0]);
				if(u16Tmp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				u32Tmp = (uint32) _atof(StrBuf[1]);
				if(u32Tmp > 65535){
					sprintf(StrBuf[1], " 0-65535");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x18;		// 命令字	18
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x02;		// 命令选项：0- 报警限值 1-关阀限值  2-报警限值+关阀限值	
				Args.buf[i++] = (uint8)u16Tmp;				// 报警限值
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// 关阀限值
				Args.buf[i++] = (uint8)(u32Tmp >> 8);
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
}


// 5	工作参数
void WaterCmdFunc_WorkingParams(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 *time = &TmpBuf[200], *timeBytes = &TmpBuf[300];
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout, enableStatus, u16Tmp;
	uint32 port, u32Tmp, u32Args[20];
	uint8 ip[4], u8Tmp;

	_ClearScreen();

	ListBoxCreate(&menuList, 0, 0, 20, 7, 19, NULL,		
		"<<工作参数",
		19,
		"1. 设置IP+端口+模式",
		"2. 读取IP+端口+模式",
		"3. 读取运营商编号",
		"4. 读取IMEI+ICCID",
		"5. 清除反转计量数据",
		"6. 读取功能使能状态",
		"7. 设置定时上传",
		"8. 设置定量上传",
		"9. 设置定时定量上传",
		"10.读表端时钟",
		"11.校表端时钟",
		"12.读收发磁扰阀控数",
		"13.读取模块运行参数",		
		"14.设置模块运行参数",
		"15.读取NB入网信息",
		"16.读取北京水表参数",		
		"17.设置北京水表参数",
		"18.读取模块的频点",		
		"19.设置模块的频点"
	);

	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		menuItemNo = menuList.strIdx + 1;

		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		while(1){
			
			_ClearScreen();

			// 公共部分 :  界面显示
			CurrCmdName = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
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

			switch(menuItemNo){
			case 1: 
				CurrCmd = WaterCmd_SetIpPortMode;			// 设IP+端口+模式
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(StrBuf[0][0] > 1){
						StrBuf[0][0] = 0;
					}
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "工作模式:", &StrBuf[0][0], 2, 
						"Coap", "Udp");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "  IP:", StrBuf[1], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, ".", StrBuf[2], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 12*8, (uiRowIdx)*16, ".", StrBuf[3], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 16*8, (uiRowIdx)*16, ".", StrBuf[4], 3, 3*8, true);
					uiRowIdx++;
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Port:", StrBuf[5], 5, 6*8, true);
					break;
				}
				// ip
				if((i = IpStrsToIpBytes(ip, StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4])) > 0){
					currUi = 2 + i;
					isUiFinish = false;
					continue;
				}
				// port
				u32Tmp = (uint32)_atof(StrBuf[5]);
				if(StrBuf[5][0] < '0' || StrBuf[5][0] > '9' || u32Tmp > 65535){
					currUi = 7;
					isUiFinish = false;
					continue;
				}

				i = 0;
				Args.buf[i++] = 0x0D;		// 命令字	0D
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x01;		// 命令选项：0-读取， 1-设置
				Args.buf[i++] = (0xA0 + (uint8)StrBuf[0][0]);	// 模式：A0-coap, A1-udp
				Args.buf[i++] = (uint8)(ip[0] & 0xFF);	// IP
				Args.buf[i++] = (uint8)(ip[1] & 0xFF);	
				Args.buf[i++] = (uint8)(ip[2] & 0xFF);		
				Args.buf[i++] = (uint8)(ip[3] & 0xFF);	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// Port	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.lastItemLen = i - 1;
				break;

			case 2: 
				CurrCmd = WaterCmd_ReadIpPortMode;			// 读IP+端口+模式
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0D;		// 命令字	0D
				ackLen = 9;					// 应答长度 9	
				// 数据域
				Args.buf[i++] = 0x00;		// 命令选项：0-读取， 1-设置
				Args.lastItemLen = i - 1;
				break;
				
			case 3: 
				CurrCmd = WaterCmd_ReadNbOperaterNumber;		// " 读NB运营商编号 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0E;		// 命令字	0E
				ackLen = 7;					// 应答长度 7	
				// 数据域
				Args.buf[i++] = 0;			// 命令选项 0-读取， 1-设置	
				Args.lastItemLen = i - 1;
				break;

			case 4: 
				CurrCmd = WaterCmd_ReadImeiAndCcid;			// 读IMEI+ICCID
				/*---------------------------------------------*/
				Args.buf[i++] = 0x0F;		// 命令字	0F
				ackLen = 22;				// 应答长度 12	
				// 数据域
				Args.buf[i++] = 0;			// 脉冲系数	0-读取IMEI+ICCID，1-设置IMEI， 2-设置ICCID
				Args.lastItemLen = i - 1;
				break;

			case 5: 
				CurrCmd = WaterCmd_ClearReverseMeasureData;	// 清除反转计量数据
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0A;		// 命令字	0A
				ackLen = 6;					// 应答长度 6	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case 6: 
				CurrCmd = WaterCmd_ReadFuncEnableState;		// 读取功能使能状态
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0B;		// 命令字	0B
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x00;
				Args.lastItemLen = i - 1;
				break;
			
			case 7: 
				CurrCmd = WaterCmd_SetTimedUpload;		// 设置定时上传
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_Printfxy(7*16, (uiRowIdx)*16, "小时", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定时间隔:", StrBuf[0], 2, 3*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], "   ");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x0C;		// 命令字	0C
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x00;		// 类别：定时间隔
				Args.buf[i++] = (uint8) _atof(StrBuf[0]);	// 定时间隔时间
				Args.lastItemLen = i - 1;
				break;

            case 8: 
				CurrCmd = WaterCmd_SetFixedValUpload;		// 设置定量上传
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_Printfxy(7*16, (uiRowIdx)*16, "立方米", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定量间隔:", StrBuf[0], 3, 4*8, true);
					break;
				}
				if((StrBuf[0][0] > '9' || StrBuf[0][0] < '0') 
					|| _atof(StrBuf[0]) > 255){
					sprintf(StrBuf[0], "   ");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x0C;		// 命令字	0C
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x01;		// 类别：定量间隔
				Args.buf[i++] = (uint8) _atof(StrBuf[0]);	// 定量间隔数量
				Args.lastItemLen = i - 1;
				break;

			case 9: 
				CurrCmd = WaterCmd_SetTimedAndFixedValUpload;	// 设置定时定量上传
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_Printfxy(7*16, (uiRowIdx)*16, "小时", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定时间隔:", StrBuf[0], 2, 3*8, true);
					_Printfxy(7*16, (uiRowIdx)*16, "立方米", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定量间隔:", StrBuf[1], 3, 4*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], "   ");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				if((StrBuf[1][0] > '9' || StrBuf[1][0] < '0') 
					|| _atof(StrBuf[1]) > 255){
					sprintf(StrBuf[1], "   ");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x0C;		// 命令字	0C
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x02;		// 类别：定时间隔
				Args.buf[i++] = (uint8) _atof(StrBuf[0]);	// 定时间隔时间
				Args.buf[i++] = (uint8) _atof(StrBuf[1]);	// 定量间隔数量
				Args.lastItemLen = i - 1;
				break;

			case 10: 
				CurrCmd = WaterCmd_ReadMeterTime;			// 读表端时钟
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x13;		// 命令字	13
				ackLen = 7;					// 应答长度 7	
				// 数据域
				Args.buf[i++] = 0x00;		// 命令选项 00	
				Args.lastItemLen = i - 1;
				break;

			case 11: 
				CurrCmd = WaterCmd_SetMeterTime;			// 校表端时钟
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
					
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "时 间:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
					break;
				}
				// 时间有效值校验
				if( (i = TimeStrsToTimeBytes(timeBytes, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
					currUi = 3 + (i -1);
					isUiFinish = false;
					continue;
				}

				sprintf(time, "%s-%s-%s %s:%s:%s",
					StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				_SetDateTime(time);

				i = 0;
				Args.buf[i++] = 0x14;		// 命令字	14
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = DecToBcd(timeBytes[0]);		// 时间 - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = DecToBcd(timeBytes[1]);		
				Args.buf[i++] = DecToBcd(timeBytes[2]);		
				Args.buf[i++] = DecToBcd(timeBytes[3]);			
				Args.buf[i++] = DecToBcd(timeBytes[4]);			
				Args.buf[i++] = DecToBcd(timeBytes[5]);			
				Args.buf[i++] = DecToBcd(timeBytes[6]);	
				Args.lastItemLen = i - 1;
				break;

            case 12: 
				CurrCmd = WaterCmd_ReadRxTxMgnDistbCnt;		// 读收/发/磁扰次数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// 命令字	09
				ackLen = 7;					// 应答长度 7	
				// 数据域
				Args.buf[i++] = 0x00;				// 数据格式 00	
				Args.lastItemLen = i - 1;
				break;

			case 13: 
				CurrCmd = WaterCmd_ReadModuleRunningParams;		// 读取模块运行参数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x3A;		// 命令字	3A
				ackLen = 124;				// 应答长度 124	
				Args.lastItemLen = i - 1; 
				break;

			case 14: 
				CurrCmd = WaterCmd_SetModuleRunningParams;		// 设置模块运行参数
				/*---------------------------------------------*/
				// UI-第1页
				if(false == isUiFinish){
					StrBuf[0][0] = 0;
					StrBuf[0][1] = 0;
					StrBuf[0][2] = 1;
					_Printfxy(0, 9*16, "返回 <等待输入> 继续", Color_White);
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "类型:", &StrBuf[0][0], 9, 
						"RF冷水表", "GPRS冷水表", "NB冷水表", 
						"RF热水表", "NB热水表", 
						"RF燃气表", "GPRS燃气表", "NB燃气表", 
						"电表");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "电池:", &StrBuf[0][1], 3, 
						"3.6v", "6v", "4.5v");
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "计量传感器:");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "", &StrBuf[0][2], 13, 
						"单干簧管/霍尔", "双干簧管/霍尔", "三干簧管/霍尔", "骏普4位光电直读", 
						"厚膜直读", "骏普1位光电直读", "188协议光电直读", "188协议无磁直读",
						"两霍尔竟达", "宁波无磁", "山科无磁", "东海无磁",
						"三川无磁");
					break;
				}
				// UI-第2页
				currUi = 0;
				StrBuf[0][3] = 1;
				StrBuf[0][4] = 1;
				sprintf(StrBuf[1], "121");
				sprintf(StrBuf[2], "43");
				sprintf(StrBuf[3], "175");
				sprintf(StrBuf[4], "222");
				sprintf(StrBuf[5], "5683");
				while(1){
					_Printfxy(0, 9*16, "返回 <等待输入> 继续", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "脉冲系数:", &StrBuf[0][3], 4, 
						"1", "10", "100", "1000");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "正转用量:", StrBuf[6], 10, 10*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "NB连接方式:", &StrBuf[0][4], 2, 
						"Coap", "Udp");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "  IP:", StrBuf[1], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, ".", StrBuf[2], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 12*8, (uiRowIdx)*16, ".", StrBuf[3], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 16*8, (uiRowIdx)*16, ".", StrBuf[4], 3, 3*8, true);
					uiRowIdx++;
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Port:", StrBuf[5], 5, 6*8, true);

					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// 正转用量
					if(StrBuf[6][0] > '9' || StrBuf[6][0] < '0'){
						currUi = 1;
						isUiFinish = false;
						continue;
					}
					u32Tmp = (uint32) _atof(StrBuf[6]);
					u16Tmp = (uint16)((float)((_atof(StrBuf[6]) - u32Tmp)*1000.0));
					
					// ip
					if((i = IpStrsToIpBytes(ip, StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4])) > 0){
						currUi = 2 + i;
						isUiFinish = false;
						continue;
					}
					// port
					port = (uint32)_atof(StrBuf[5]);
					if(StrBuf[5][0] < '0' || StrBuf[5][0] > '9' || port > 65535){
						currUi = 7;
						isUiFinish = false;
						continue;
					}

					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}

				// UI-第3页
				currUi = 0;
				StrBuf[0][5] = 0;
				StrBuf[0][6] = 0;
				StrBuf[0][7] = 1;
				StrBuf[0][8] = 1;
				StrBuf[0][9] = 0;
				StrBuf[0][10] = 0;
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "磁扰关阀:", &StrBuf[0][5], 2, 
						"关闭", "开启");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "防拆检测:", &StrBuf[0][6], 2, 
						"关闭", "开启");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "主动告警:", &StrBuf[0][7], 2, 
						"关闭", "开启");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "冻结上报:", &StrBuf[0][8], 2, 
						"关闭", "开启");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "阀门防锈:", &StrBuf[0][9], 2, 
						"关闭", "开启");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "掉电关阀:", &StrBuf[0][10], 2, 
						"关闭", "开启");
					
					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// 使能状态
					enableStatus = 0;
					enableStatus |= (StrBuf[0][5] == 0 ? 0x0000 : 0x0001);	// 磁扰关阀
					enableStatus |= (StrBuf[0][6] == 0 ? 0x0000 : 0x0002);	// 防拆检测
					enableStatus |= (StrBuf[0][7] == 0 ? 0x0000 : 0x0010);	// 主动告警
					enableStatus |= (StrBuf[0][8] == 0 ? 0x0000 : 0x0020);	// 冻结上报
					enableStatus |= (StrBuf[0][9] == 0 ? 0x0000 : 0x0200);	// 阀门防锈
					enableStatus |= (StrBuf[0][10] == 0 ? 0x0000 : 0x0400);	// 掉电关阀
					

					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}
				
				i = 0;
				Args.buf[i++] = 0x3F;		// 命令字	3F
				ackLen = 1;					// 应答长度 1	
				// 数据域
				switch ((uint8)StrBuf[0][0]){			// 表类型
				case 0: u8Tmp = 0x10; break;
				case 1: u8Tmp = 0x11; break;
				case 2: u8Tmp = 0x12; break;
				case 3: u8Tmp = 0x20; break;
				case 4: u8Tmp = 0x21; break;
				case 5: u8Tmp = 0x30; break;
				case 6: u8Tmp = 0x31; break;
				case 7: u8Tmp = 0x32; break;
				case 8: u8Tmp = 0x40; break;
				default: u8Tmp = 0x10; break;
				}

				Args.buf[i++] = u8Tmp;	
				Args.buf[i++] = (uint8)StrBuf[0][3];	// 脉冲系数	
				Args.buf[i++] = 50;			// 磁干扰开阀时间
				Args.buf[i++] = 100;		// 计量脉冲最小闭合时间
				Args.buf[i++] = (uint8)(12000 & 0xFF);	// 开关阀时间 12000
				Args.buf[i++] = (uint8)(12000 >> 8);		
				Args.buf[i++] = 150;		// 过流阀值
				Args.buf[i++] = (uint8)StrBuf[0][1];	// 电池电压类型
				Args.buf[i++] = 120;		// 定时上传时间间隔	
				Args.buf[i++] = 2;			// 定量上传数量间隔	
				Args.buf[i++] = 3;		// 每天定时定量最大上传次数	
				Args.buf[i++] = (uint8)StrBuf[0][2];	// 计量传感器类型	
				Args.buf[i++] = ip[0];		// IP的第1~4个字节	121.43.175.22 : 5683
				Args.buf[i++] = ip[1];
				Args.buf[i++] = ip[2];
				Args.buf[i++] = ip[3];
				Args.buf[i++] = (uint8)(port & 0xFF);	// 服务器连接端口
				Args.buf[i++] = (uint8)((port >> 8) & 0xFF);
				Args.buf[i++] = 10;			// 水表防锈定时间隔	
				Args.buf[i++] = 0;			// 系统调试级别	
				Args.buf[i++] = 0x46;			// 运营商编号/SIM卡类型 NB-IOT时：46001000
				Args.buf[i++] = 0x00;	
				Args.buf[i++] = 0x10;	
				Args.buf[i++] = 0x00;		
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()/100));	// 出厂/RF设定的系统时间 - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()%100));		
				Args.buf[i++] = DecToBcd(_GetMonth());		
				Args.buf[i++] = DecToBcd(_GetDay());			
				Args.buf[i++] = DecToBcd(_GetHour());			
				Args.buf[i++] = DecToBcd(_GetMin());			
				Args.buf[i++] = DecToBcd(_GetSec());
				Args.buf[i++] = 8;			// 通信频段	
				Args.buf[i++] = ((uint8)StrBuf[0][4] + 1);	// NB连接方式	1 - coap ,  2- udp
				Args.buf[i++] = 0;			// 预留	
				Args.buf[i++] = 0;			// 预留
				Args.buf[i++] = 0;			// 剩余用量报警限值	
				Args.buf[i++] = 0;			// 剩余用量关阀限值	2 byte
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;			// 预缴费量数 6 byte
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;		
				Args.buf[i++] = 0;			// 参考起始用量 6 byte
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;			// 模块测试状态	2 byte
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;			// 预留	
				Args.buf[i++] = 0;			// 预留
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// 正转用量	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				Args.buf[i++] = 0;			// 反转用量 6 byte
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0x00;		// 阀状态
				Args.buf[i++] = (uint8)(enableStatus & 0xFF);	// 功能使能状态
				Args.buf[i++] = (uint8)(enableStatus >> 8);	
				Args.buf[i++] = 0x00;		// 报警状态标志位
				Args.buf[i++] = 0x00;	
				Args.buf[i++] = 0;			// 按日按月设置
				Args.buf[i++] = 0;			// RF侦听起始时间	
				Args.buf[i++] = 24;			// RF侦听工作时长	
				Args.buf[i++] = 0;			// 预留	10 byte
				for(u8Tmp = 0; u8Tmp < 9; u8Tmp++){
					Args.buf[i++] = 0;
				}
				// 读取时返回：以上 + 模块软件版本号 40 byte	

				Args.lastItemLen = i - 1;
				break;

			case 15: 
				CurrCmd = WaterCmd_ReadNbJoinNetworkInfo;		// 读取NB入网信息
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x10;		// 命令字	10
				ackLen = 34;				// 应答长度 34	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case 16: 
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

			case 17: 
				CurrCmd = WaterCmd_SetBeiJingWaterMeterParams;		// 设置北京水表参数
				/*---------------------------------------------*/
				// UI-第1页
				if(false == isUiFinish){
					if(BackupBuf[0] != Param_BeijingWMtr){		
						StrBuf[0][0] = 0;
						StrBuf[1][0] = 0;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[1 * 20], 2 * 20);
					}
					_Printfxy(0, 9*16, "返回 <等待输入> 继续", Color_White);
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "水表类型:");
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "  ", &StrBuf[0][0], 5, 
						"三川NB无磁", "宁波NB无磁", "山科NB无磁", 
						"东海NB无磁", "京源NB无磁");
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "出厂表号:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrBuf[1], 14, 15*8, true);
					break;
				}
				// 水表类型
				u32Args[0] = StrBuf[0][0];
				// 出厂表号
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					currUi = 5;
					isUiFinish = false;
					continue;
				}
				_leftspace(StrBuf[1], 14, '0');
				memcpy(&TmpBuf[0], &StrBuf[1][0], 14);
				TmpBuf[14] = 0x00;

				memcpy(&BackupBuf[1 * 20], &StrBuf[0][0], 2 * 20);

				// UI-第2页
				currUi = 0;
				if(BackupBuf[0] != Param_BeijingWMtr){	
					sprintf(StrBuf[1], "40");
					sprintf(StrBuf[2], "1");
					sprintf(StrBuf[3], "40");
					sprintf(StrBuf[4], "1");
					sprintf(StrBuf[5], "10");
					sprintf(StrBuf[6], "14");
				}
				else{
					memcpy(&StrBuf[1][0], &BackupBuf[3 * 20], 6 * 20);
				}
				
				while(1){
					_Printfxy(0, 9*16, "返回 <等待输入> 继续", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "过流告警阈值:", StrBuf[1], 6, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "过流告警时间:", StrBuf[2], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "反流告警阈值:", StrBuf[3], 6, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "反流告警时间:", StrBuf[4], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "电压告警阀值:", StrBuf[5], 5, 5*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "密集上报时间点:", StrBuf[6], 2, 2*8, true);
					LableCreate(&pUi[(*pUiCnt)++], 9*16, (uiRowIdx++)*16, "点");

					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// 有效值判断
					for(i = 1; i <= 6; i++){
						u32Args[i] = (uint32) _atof(StrBuf[i]);
						if(StrBuf[i][0] < '0' || StrBuf[i][0] > '9'
							|| (i == 2 && u32Args[i] > 255)
							|| (i == 4 && u32Args[i] > 255)
							|| (i == 5 && u32Args[i] > 65535)
							|| (i == 6 && u32Args[i] > 23)
						){
							currUi = i - 1;
							isUiFinish = false;
							break;
						}
					}

					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}

				memcpy(&BackupBuf[3 * 20], &StrBuf[1][0], 6 * 20);

				// UI-第3页
				currUi = 0;
				if(BackupBuf[0] != Param_BeijingWMtr){	
					StrBuf[0][0] = 1;
					StrBuf[0][1] = 2;
					sprintf(StrBuf[1], "121");
					sprintf(StrBuf[2], "43");
					sprintf(StrBuf[3], "175");
					sprintf(StrBuf[4], "222");
					sprintf(StrBuf[5], "5683");
					memset(StrBuf[6], 0x00, 10);
					sprintf(StrBuf[6], "CMIOT");
					sprintf(StrBuf[7], "1");
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[9 * 20], 8 * 20);
				}
				
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "返回 <等待输入> 继续", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "终端设置:", &StrBuf[0][0], 2, 
						"停用", "启用");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "  IP:", StrBuf[1], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, ".", StrBuf[2], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 12*8, (uiRowIdx)*16, ".", StrBuf[3], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 16*8, (uiRowIdx)*16, ".", StrBuf[4], 3, 3*8, true);
					uiRowIdx++;
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Port:", StrBuf[5], 5, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " APN:", StrBuf[6], 6, 7*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.isInputAny = 1;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "上报重连次数:", &StrBuf[0][1], 5, 
						"0", "1", "2", "3", "4");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "周期采样间隔:", StrBuf[7], 2, 2*8, true);
					LableCreate(&pUi[(*pUiCnt)++], 8*16, (uiRowIdx++)*16, "x30'");

					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// 终端设置
					u32Args[7] = StrBuf[0][0];
					// ip
					if((i = IpStrsToIpBytes(ip, StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4])) > 0){
						currUi = 2 + i;
						isUiFinish = false;
						continue;
					}
					// port
					port = (uint32)_atof(StrBuf[5]);
					if(StrBuf[5][0] < '0' || StrBuf[5][0] > '9' || port > 65535){
						currUi = 7;
						isUiFinish = false;
						continue;
					}
					// APN
					i = strlen(StrBuf[6]);
					if(i < 6){
						memset(&StrBuf[6][i], 0x00, 6 - i);
					}
					memcpy(&TmpBuf[20], &StrBuf[6][0], 6);
					
					// 上报重连次数
					u32Args[8] = StrBuf[0][1];

					// 周期采样间隔
					u32Args[9] = (uint32) _atof(StrBuf[7]);
					if(StrBuf[7][0] > '9' || StrBuf[7][0] < '0' || u32Args[9] < 1 || u32Args[9] > 48){
						currUi = 8;
						isUiFinish = false;
						continue;
					}

					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}

				memcpy(&BackupBuf[9 * 20], &StrBuf[0][0], 8 * 20);

				// UI-第4页
				currUi = 0;
				if(BackupBuf[0] != Param_BeijingWMtr){		
					for(i = 0; i < 14; i++){
						StrBuf[i][0] = 0x00;
					}
					sprintf(StrBuf[13], "24");
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[17 * 20], 14 * 20);
				}
				
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5], StrBuf[6]);
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "周期上报起始时间:");
					LableCreate(&pUi[(*pUiCnt)++], 1*8, (uiRowIdx)*16, StrBuf[1]);			// YYYY - 不可修改
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[3], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[4], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[5], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[6], 2, 2*8, false);	// ss

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[7], StrBuf[8], StrBuf[9], StrBuf[10], StrBuf[11], StrBuf[12]);
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "周期上报结束时间:");
					LableCreate(&pUi[(*pUiCnt)++], 1*8, (uiRowIdx)*16, StrBuf[7]);			// YYYY - 不可修改
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[8], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[9], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[10], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[11], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[12], 2, 2*8, false);	// ss

					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "周期上报估长:", &StrBuf[0][0], 6, 
						"10s", "20s", "30s", "40s", "50s", "60s");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "周期上报频率:", StrBuf[13], 2, 2*8, true);
					LableCreate(&pUi[(*pUiCnt)++], 8*16, (uiRowIdx++)*16, "小时");
					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// 周期上报起始时间
					if( (i = TimeStrsToTimeBytes(timeBytes, StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5], StrBuf[6])) > 0){
						currUi = 1 + (i -1);
						isUiFinish = false;
						continue;
					}
					// 周期上报结束时间
					if( (i = TimeStrsToTimeBytes(&timeBytes[7], StrBuf[7], StrBuf[8], StrBuf[9], StrBuf[10], StrBuf[11], StrBuf[12])) > 0){
						currUi = 8 + (i -1);
						isUiFinish = false;
						continue;
					}
					// 周期上报估长
					u32Args[10] = StrBuf[0][0] + 1;
					// 周期上报频率
					u32Args[11] = (uint32)_atof(StrBuf[13]);
					if(StrBuf[13][0] < '0' || StrBuf[13][0] > '9' || u32Args[11] < 1 || u32Args[11] > 24){
						currUi = 15;
						isUiFinish = false;
						continue;
					}
					
					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}

				memcpy(&BackupBuf[17 * 20], &StrBuf[0][0], 14 * 20);

				BackupBuf[0] = Param_BeijingWMtr;
				
				i = 0;
				Args.buf[i++] = 0x26;		// 命令字	26
				ackLen = 1;					// 应答长度 1	
				// 数据域
				Args.buf[i++] = 0x01;		// 命令选项 00 - 读取， 01 - 设置	
				switch ((uint8)u32Args[0]){			// 设备类型
				case 0: u8Tmp = 0x41; break;
				case 1: u8Tmp = 0x44; break;
				case 2: u8Tmp = 0x35; break;
				case 3: u8Tmp = 0x2A; break;
				case 4: u8Tmp = 0x2B; break;
				default: u8Tmp = 0x41; break;
				}
				Args.buf[i++] = u8Tmp;	
				GetBytesFromStringHex(Args.buf, i, 7, &TmpBuf[0], 0, false);	// 出厂表号	
				i += 7;
				Args.buf[i++] = (uint8)(u32Args[1] & 0xFF);	// 过流告警阈值 4 byte
				Args.buf[i++] = (uint8)((u32Args[1] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Args[1] >> 16) & 0xFF);	
				Args.buf[i++] = (uint8)((u32Args[1] >> 24) & 0xFF);	
				Args.buf[i++] = (uint8)(u32Args[2] & 0xFF);	// 过流告警时间 1 byte

				Args.buf[i++] = (uint8)(u32Args[3] & 0xFF);	// 反流告警阈值 4 byte
				Args.buf[i++] = (uint8)((u32Args[3] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Args[3] >> 16) & 0xFF);	
				Args.buf[i++] = (uint8)((u32Args[3] >> 24) & 0xFF);	
				Args.buf[i++] = (uint8)(u32Args[4] & 0xFF);	// 反流告警时间 1 byte

				Args.buf[i++] = (uint8)(u32Args[5] & 0xFF);	// 电压告警阀值 2 byte
				Args.buf[i++] = (uint8)((u32Args[5] >> 8) & 0xFF);		
				Args.buf[i++] = ip[0];					// IP	121.43.175.22 : 5683
				Args.buf[i++] = ip[1];
				Args.buf[i++] = ip[2];
				Args.buf[i++] = ip[3];
				Args.buf[i++] = (uint8)(port & 0xFF);	// port 2 byte
				Args.buf[i++] = (uint8)((port >> 8) & 0xFF);
				memcpy(&Args.buf[i], &TmpBuf[20], 6);	// APN	6 byte
				i += 6;
				Args.buf[i++] = (uint8)(u32Args[8] & 0xFF);	// 上报重连次数 2 byte
				Args.buf[i++] = (timeBytes[1]);	// 周期上报起始时间 YY MM dd HH mm ss		
				Args.buf[i++] = (timeBytes[2]);		
				Args.buf[i++] = (timeBytes[3]);			
				Args.buf[i++] = (timeBytes[4]);			
				Args.buf[i++] = (timeBytes[5]);			
				Args.buf[i++] = (timeBytes[6]);
				Args.buf[i++] = (timeBytes[8]);	// 周期上报结束时间 YY MM dd HH mm ss		
				Args.buf[i++] = (timeBytes[9]);		
				Args.buf[i++] = (timeBytes[10]);			
				Args.buf[i++] = (timeBytes[11]);			
				Args.buf[i++] = (timeBytes[12]);			
				Args.buf[i++] = (timeBytes[13]);
				Args.buf[i++] = (uint8)(u32Args[10] & 0xFF);	// 周期上报估计时长
				Args.buf[i++] = (uint8)(u32Args[7] & 0xFF);		// 终端启停设置	
				Args.buf[i++] = (uint8)(u32Args[11] & 0xFF);	// 周期上报频率
				Args.buf[i++] = (uint8)(u32Args[6] & 0xFF);		// 密集上报起始时间点	
				Args.buf[i++] = (uint8)(u32Args[9] & 0xFF);		// 周期采样间隔：n x30分钟	
				Args.buf[i++] = 20;						// 上报重连等待时间：固定 20 分钟
				Args.buf[i++] = 5;						// 周期采样间隔：固定 5 分钟	
				for(u8Tmp = 0; u8Tmp < 16; u8Tmp++){	// KEY	16 byte, 固定值不可设，填0
					Args.buf[i++] = 0;
				}

				Args.lastItemLen = i - 1;
				break;

			case 18: 
				CurrCmd = WaterCmd_ReadModuleFrequency;		// 读取模块的频点
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x1B;		// 命令字	1B
				ackLen = 8;					// 应答长度 8	
				// 数据域
				Args.buf[i++] = 0x00;		// 命令选项：0-读取， 1-设置
				Args.lastItemLen = i - 1;
				break;

			case 19: 
				CurrCmd = WaterCmd_SetModuleFrequency;		// 设置模块的频点
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "频点1:", StrBuf[1], 5, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "频点2:", StrBuf[2], 5, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "频点3:", StrBuf[3], 5, 6*8, true);
					break;
				}

				// 频点1,2,3
				u32Args[1] = (uint32)_atof(StrBuf[1]);
				if(StrBuf[1][0] < '0' || StrBuf[1][0] > '9' || u32Args[1] > 65535){
					currUi = *pUiCnt - 3;
					isUiFinish = false;
					continue;
				}
				u32Args[2] = (uint32)_atof(StrBuf[2]);
				if(StrBuf[2][0] < '0' || StrBuf[2][0] > '9' || u32Args[2] > 65535){
					currUi = *pUiCnt - 2;
					isUiFinish = false;
					continue;
				}
				u32Args[3] = (uint32)_atof(StrBuf[3]);
				if(StrBuf[3][0] < '0' || StrBuf[3][0] > '9' || u32Args[3] > 65535){
					currUi = *pUiCnt - 1;
					isUiFinish = false;
					continue;
				}

				i = 0;
				Args.buf[i++] = 0x1B;		// 命令字	1B
				ackLen = 1;					// 应答长度 1	
				// 数据域
				Args.buf[i++] = 0x01;		// 命令选项：0-读取， 1-设置
				Args.buf[i++] = (uint8)(u32Args[1] & 0xFF);			// 频点1	
				Args.buf[i++] = (uint8)((u32Args[1] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)(u32Args[2] & 0xFF);			// 频点2	
				Args.buf[i++] = (uint8)((u32Args[2] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)(u32Args[3] & 0xFF);			// 频点3	
				Args.buf[i++] = (uint8)((u32Args[3] >> 8) & 0xFF);
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}

			if (key == KEY_CANCEL){
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
}


void WaterCmdFunc(void)
{
	_GuiMenuStru menu;

	/* v2.2 菜单
	// menu.title= "<<工程调试 ";		
	// menu.str[0]=" 常用命令 ";
	// menu.str[1]=" 测试命令 ";
	// menu.str[2]=" 程序升级 ";
	// menu.str[3]=" 预缴用量 ";
	// menu.str[4]=" 工作参数 ";
	// menu.str[5]=" 其他操作 ";
	// menu.str[6]=" 版本信息 ";
	*/
	
	menu.left=0;
	menu.top=0;
	menu.no=5;
	menu.title= "<<工程调试 ";		// 工程调试 --> 即原来的 表端操作
	menu.str[0]=" 常用命令 ";
	menu.str[1]=" 测试命令 ";
	menu.str[2]=" 工作参数 ";
	menu.str[3]=" 程序升级 ";
	menu.str[4]=" 版本信息 ";
	menu.key[0]="1";
	menu.key[1]="2";
	menu.key[2]="3";
	menu.key[3]="4";
	menu.key[4]="5";
	menu.Function[0]=WaterCmdFunc_CommonCmd;
	menu.Function[1]=WaterCmdFunc_TestCmd;
	menu.Function[2]=WaterCmdFunc_WorkingParams;
	menu.Function[3]=WaterCmdFunc_Upgrade;
	menu.Function[4]=VersionInfoFunc;
	menu.FunctionEx=0;
	_Menu(&menu);	
}

//-----------------------------------	主菜单	---------------------------
// 读取用户用量
void MainFuncReadRealTimeData(void)
{
	uint8 key, tryCnt = 0, i;
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
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<读取用户用量");
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
		CurrCmd = WaterCmd_ReadRealTimeData;	// "读取用户用量"

		
		switch(CurrCmd){
		case WaterCmd_ReadRealTimeData:		// "读取用户用量"
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

			if( strcmp(StrDstAddr, "D4D4D4D4D4D4D4D4") != 0 
				&& (StrDstAddr[0] < '0' || StrDstAddr[0] > '9'))
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

// 读取冻结数据
void MainFuncReadFrozenData(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<读取冻结数据");
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
		CurrCmd = WaterCmd_ReadFrozenData;		// "读取冻结正转数据"
		
		switch(CurrCmd){
		case WaterCmd_ReadFrozenData:		// "读取冻结正转数据"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				sprintf(StrBuf[0], "0 (0-9有效)");
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "序 号:", StrBuf[0], 1, 2*8, true);
				break;
			}
			
			if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
				currUi = 1;
				isUiFinish = false;
				continue;
			}
			Args.buf[i++] = 0x02;		// 命令字	02
			ackLen = 114;				// 应答长度 88/114	
			// 数据域
			Args.buf[i++] = 0x01;				// 数据格式 01/02
			Args.buf[i++] = _GetYear()/100;		// 时间 - yyyy/mm/dd HH:mm:ss
			Args.buf[i++] = _GetYear()%100;		
			Args.buf[i++] = _GetMonth();		
			Args.buf[i++] = _GetDay();			
			Args.buf[i++] = _GetHour();			
			Args.buf[i++] = _GetMin();			
			Args.buf[i++] = _GetSec();			
			Args.buf[i++] = StrBuf[0][0] - '0';	// 冻结数据序号	
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

// 读取表端时钟
void MainFuncReadMeterTime(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<读取表端时钟");
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
		CurrCmd = WaterCmd_ReadMeterTime;	// 读取表端时钟

		switch(CurrCmd){
		case WaterCmd_ReadMeterTime:		// "读取表端时钟"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x13;		// 命令字	13
			ackLen = 7;					// 应答长度 7	
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

// 设置表端时钟
void MainFuncSetMeterTime(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;
	uint8 *time = &TmpBuf[200], *timeBytes = &TmpBuf[300];

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<设置表端时钟");
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
		CurrCmd = WaterCmd_SetMeterTime;	// 设置表端时钟

		switch(CurrCmd){
		case WaterCmd_SetMeterTime:		// "设置表端时钟"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				_GetDateTime(time, '-',  ':');
				DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "时 间:");
				TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
				TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
				TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
				TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
				TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
				TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
				break;
			}
			// 时间有效值校验
			if( (i = TimeStrsToTimeBytes(timeBytes, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
				currUi = 3 + (i -1);
				isUiFinish = false;
				continue;
			}

			sprintf(time, "%s-%s-%s %s:%s:%s",
				StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
			_SetDateTime(time);

			i = 0;
			Args.buf[i++] = 0x14;		// 命令字	14
			ackLen = 2;					// 应答长度 2	
			// 数据域
			Args.buf[i++] = DecToBcd(timeBytes[0]);		// 时间 - yyyy/mm/dd HH:mm:ss
			Args.buf[i++] = DecToBcd(timeBytes[1]);		
			Args.buf[i++] = DecToBcd(timeBytes[2]);		
			Args.buf[i++] = DecToBcd(timeBytes[3]);			
			Args.buf[i++] = DecToBcd(timeBytes[4]);			
			Args.buf[i++] = DecToBcd(timeBytes[5]);			
			Args.buf[i++] = DecToBcd(timeBytes[6]);	
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

// 清异常命令
void MainFuncClearException(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<清异常");
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
		CurrCmd = WaterCmd_ClearException;		// " 清异常命令 ";
		
		switch(CurrCmd){
		case WaterCmd_ClearException:		// " 清异常命令 ";
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x05;		// 命令字	05
			ackLen = 1;					// 应答长度 1	
			// 数据域
			Args.buf[i++] = 0x00;		// 命令选项 00	
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

// 开阀
void MainFuncOpenValve(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<开阀");
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
		CurrCmd = WaterCmd_OpenValve;		// " 开阀 "
		
		switch(CurrCmd){
		case WaterCmd_OpenValve:			// " 开阀 "
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x03;		// 命令字	03
			ackLen = 3;					// 应答长度 3	
			// 数据域
			Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
			Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
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

// 关阀
void MainFuncCloseValve(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<关阀");
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
		CurrCmd = WaterCmd_CloseValve;		// " 关阀 ";
		
		switch(CurrCmd){
		case WaterCmd_CloseValve:		// " 关阀 ";
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x03;		// 命令字	03
			ackLen = 3;					// 应答长度 3	
			// 数据域
			Args.buf[i++] = 0x00;		// 强制标识 	0 - 不强制， 1 - 强制
			Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
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

// 读取NB入网信息
void MainFuncReadNbJoinNetworkInfo(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		CurrCmdName = &ArgBuf[0];
		sprintf(CurrCmdName, "<<读取NB入网信息");
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
		CurrCmd = WaterCmd_ReadNbJoinNetworkInfo;
		
		switch(CurrCmd){
		case WaterCmd_ReadNbJoinNetworkInfo:		// 读取NB入网信息
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x10;		// 命令字	10
				ackLen = 34;				// 应答长度 34	
				// 数据域
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

// 工程调试
void MainFuncEngineerDebuging(void)
{
	WaterCmdFunc();		// 工程调试 --> 即原来的 表端操作
}

// --------------------------------   主函数   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	
	#ifdef Project_6009_RF
	MeterNoLoad(StrDstAddr, 0);
	#elif defined Project_6009_IR
	MeterNoLoad(StrDstAddr, 1);
	#else // Project_8009_RF
	MeterNoLoad(StrDstAddr, 2);
	#endif
	
	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=7;
	MainMenu.title =  "     桑锐手持机    ";
	MainMenu.str[0] = " 读取用户用量 ";
	MainMenu.str[1] = " 读取冻结数据 ";
	MainMenu.str[2] = " 开阀 ";
	MainMenu.str[3] = " 关阀 ";
    MainMenu.str[4] = " 清异常 ";
	MainMenu.str[5] = " 读取NB入网信息 ";
	MainMenu.str[6] = " 工程调试 ";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.Function[0] = MainFuncReadRealTimeData;
	MainMenu.Function[1] = MainFuncReadFrozenData;
	MainMenu.Function[2] = MainFuncOpenValve;
	MainMenu.Function[3] = MainFuncCloseValve;
    MainMenu.Function[4] = MainFuncClearException;
	MainMenu.Function[5] = MainFuncReadNbJoinNetworkInfo;
	MainMenu.Function[6] = MainFuncEngineerDebuging;	// 工程调试 --> 即原来的 表端操作
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	
}

