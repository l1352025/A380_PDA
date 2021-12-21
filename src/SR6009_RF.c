/*
*
*	桑锐6009水表手持机 - Lora版
*
*/
#ifndef Protocol_6009
#include "SR6009_RF.h"
#endif

#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "common.h"
#include "common.c"
#ifndef Use_DBF
#include "MeterDocDBF.h"
#include "MeterDocDBF.c"
#endif
#include "WaterMeter.h"
#include "Upgrade.h"
#include "Upgrade.c"
#include "List.h"
#include "List.c"

void FillStrsFunc(char **strs, int16 strsIdx, int16 srcIdx, uint16 cnt)
{
	int i = strsIdx;
	
	for(i = strsIdx; i < strsIdx + cnt; i++){
		sprintf(strs[i], "列表项 %d", i);
	}
}


#if CenterCmd_Enable
// --------------------------------  集中器模块通信  -----------------------------------------
// 1	常用命令
void CenterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 9, NULL,
		"<<常用命令",
		9,
		"1. 读取集中器号",
		"2. 读取集中器版本",
		"3. 读取集中器时钟",
		"4. 设置集中器时钟",
		"5. 读取GPRS参数",
		"6. 设置GPRS参数",
		"7. 读GPRS信号强度",
		"8. 集中器初始化",
		"9. 读集中器工作模式"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
			CurrCmd = (0x1010 + menuItemNo);

			switch(CurrCmd){
			case CenterCmd_ReadCenterNo:	// 读集中器号
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x41;		// 命令字	41
				ackLen = 6;					// 应答长度 6	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadCenterVer:	// 读集中器版本
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x40;		// 命令字	40
				ackLen = 6;					// 应答长度 6	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadCenterTime:		// 读集中器时钟
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x43;		// 命令字	43
				ackLen = 7;					// 应答长度 7	
				// 数据域
				Args.lastItemLen = i - 1;
				break;
			
			case CenterCmd_SetCenterTime:		// 设集中器时钟
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_GetDate(&TmpBuf[200], '-');
					_GetTime(&TmpBuf[220], ':');
					StrBuf[0][0] = TmpBuf[200];		// year
					StrBuf[0][1] = TmpBuf[201];
					StrBuf[0][2] = TmpBuf[202];
					StrBuf[0][3] = TmpBuf[203];
					StrBuf[0][4] = 0x00;
					StrBuf[1][0] = TmpBuf[205];		// month
					StrBuf[1][1] = TmpBuf[206];
					StrBuf[1][2] = 0x00;
					StrBuf[2][0] = TmpBuf[208];	// day
					StrBuf[2][1] = TmpBuf[209];
					StrBuf[2][2] = 0x00;
					StrBuf[3][0] = TmpBuf[220];	// hour
					StrBuf[3][1] = TmpBuf[221];
					StrBuf[3][2] = 0x00;
					StrBuf[4][0] = TmpBuf[223];	// minute
					StrBuf[4][1] = TmpBuf[224];
					StrBuf[4][2] = 0x00;
					StrBuf[5][0] = TmpBuf[226];	// second
					StrBuf[5][1] = TmpBuf[227];
					StrBuf[5][2] = 0x00;
					_Printfxy(0, 3*16, "时 间: ", Color_White);
					uiRowIdx = 4;
					TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
					break;
				}
				for(i = 0; i < 6; i ++){
					if((StrBuf[i][0] > '9' && StrBuf[i][0] < '0') || StrBuf[0][0] == '0' ){
						// 请输入有效值 
						currUi = 1;
						isUiFinish = false;
						break;
					}
				}
				if(false == isUiFinish){
					continue;
				}

				sprintf(&TmpBuf[200], "%s-%s-%s %s:%s:%s",
					StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				_SetDateTime(&TmpBuf[200]);

				i = 0;
				Args.buf[i++] = 0x14;		// 命令字	14
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()/100));		// 时间 - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()%100));		
				Args.buf[i++] = DecToBcd(_GetMonth());		
				Args.buf[i++] = DecToBcd(_GetDay());			
				Args.buf[i++] = DecToBcd(_GetHour());			
				Args.buf[i++] = DecToBcd(_GetMin());			
				Args.buf[i++] = DecToBcd(_GetSec());	
				Args.lastItemLen = i - 1;
				break;

            case CenterCmd_ReadGprsParam:		// 读GPRS参数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x45;		// 命令字	45
				ackLen = 40;				// 应答长度 约 40	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_SetGprsParam:		// 设GPRS参数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x46;		// 命令字	46
				ackLen = 1;					// 应答长度 1	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadGprsSignal:		// 读GPRS信号强度
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x47;		// 命令字	47
				ackLen = 50;				// 应答长度 约 50	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_InitCenter:			// 集中器初始化
				/*---------------------------------------------*/
				if(false == isUiFinish){
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "类型:", &StrBuf[0][0], 2, 
						"清空档案和路径", "清空所有数据");
					break;
				}
				Args.buf[i++] = 0x48;		// 命令字	48
				ackLen = 2;					// 应答长度 1	
				// 数据域
				Args.buf[i++] = (uint8)StrBuf[0][0];	// 操作类型	
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadCenterWorkMode:	// 读集中器工作模式
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x49;		// 命令字	49
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

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
					sprintf(StrDstAddr, " 请输入");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

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

// 2	档案操作
void CenterCmdFunc_DocumentOperation(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 5, NULL,
		"<<档案操作",
		5,
		"1. 读档案数量",
		"2. 读档案信息",
		"3. 添加档案信息",
		"4. 删除档案信息",
		"5. 修改档案信息"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
			CurrCmd = (0x1020 + menuItemNo);

			switch(CurrCmd){
			case CenterCmd_ReadDocCount:		// 读档案数量
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
				Args.buf[i++] = 0x01;		// 命令字	01
				ackLen = 21;				// 应答长度 21	
				// 数据域
				Args.buf[i++] = 0x00;				// 数据格式 00	
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadDocInfo:			// 读档案信息
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
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_AddDocInfo:			// 添加档案信息
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
			
			case CenterCmd_DeleteDocInfo:		// 删除档案信息
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

            case CenterCmd_ModifyDocInfo:		// 修改档案信息
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

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
					sprintf(StrDstAddr, " 请输入");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

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

// 3	路径设置
void CenterCmdFunc_RouteSetting(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 2, NULL,
		"<<路径设置",
		2,
		"1. 读取自定义路由",
		"2. 设置自定义路由"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
			CurrCmd = (0x1030 + menuItemNo);

			switch(CurrCmd){
			case CenterCmd_ReadDefinedRoute:	// 读自定义路由
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号: ", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x55;		// 命令字	55
				ackLen = 21;				// 应答长度 21	
				// 数据域
				Args.buf[i++] = 0x00;				// 数据格式 00	
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_SetDefinedRoute:		// 设自定义路由
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

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
					sprintf(StrDstAddr, " 请输入");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

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

// 4	命令转发
void CenterCmdFunc_CommandTransfer(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<命令转发",
		7,
		"1. 读实时数据",
		"2. 读定时定量数据",
		"3. 读冻结数据",
		"4. 开阀",
		"5. 关阀",
		"6. 读使能",
		"7. 清异常"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
			CurrCmd = (0x1040 + menuItemNo);

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
			
			case WaterCmd_OpenValveForce:		// " 强制开阀 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

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

			case WaterCmd_CloseValveForce:		// " 强制关阀 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

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

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
					sprintf(StrDstAddr, " 请输入");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

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

void CenterCmdFunc(void)
{
	_GuiMenuStru menu;
	int fp;

	fp = _Fopen("system.cfg", "R");
	_Lseek(fp, 20, 0);	// byte [20 ~ 29] 集中器号 
	_Fread(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);

	menu.left=0;
	menu.top=0;
	menu.no=4;
	menu.title= "<<集中器操作 ";
	menu.str[0]=" 常用命令 ";
	menu.str[1]=" 档案操作 ";
	menu.str[2]=" 路径设置 ";
	menu.str[3]=" 命令转发 ";
	menu.key[0]="1";
	menu.key[1]="2";
	menu.key[2]="3";
	menu.key[3]="4";
	menu.Function[0]=CenterCmdFunc_CommonCmd;
	menu.Function[1]=CenterCmdFunc_DocumentOperation;
	menu.Function[2]=CenterCmdFunc_RouteSetting;
	menu.Function[3]=CenterCmdFunc_CommandTransfer;
	menu.FunctionEx=0;
	_Menu(&menu);	

	fp = _Fopen("system.cfg", "R");
	_Lseek(fp, 0, 0);	// byte [0 ~ 19] 表号 
	_Fread(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);
	
}
#endif

// --------------------------------  水表模块通信  -----------------------------------------

// 1	常用命令
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<常用命令",
		7,
		"1. 读取用户用量",
		"2. 读取冻结数据",
		"3. 开阀",
		"4. 强制开阀",
		"5. 关阀",
		"6. 强制关阀",
		"7. 清异常命令"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
			CurrCmd = (0x10 + menuItemNo);

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
			
			case WaterCmd_OpenValveForce:		// " 强制开阀 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x01;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

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

			case WaterCmd_CloseValveForce:		// " 强制关阀 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x01;		// 强制标识 	0 - 不强制， 1 - 强制
				Args.buf[i++] = 0x00;		// 开关阀标识	0 - 关阀， 1 - 开阀
				Args.lastItemLen = i - 1;
				break;

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
				uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
				
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
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
	ListBoxCreate(&menuList, 0, 0, 20, 7, 10, NULL,
		"<<测试命令",
		10,
		"1. 表端重启",
		"2. 读表温度",
		"3. 读表电压",
		"4. 清预缴参考量",
		"5. 设置过流超时",
		"6. 读运营商编号",
		"7. 读上报路径",
		"8. 设置表号",
		"9. 读debug信息",
		"10.清debug信息"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
			CurrCmd = (0x20 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadDebugInfo:		// 读debug信息
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

			case WaterCmd_ClearDebugInfo:		// 清debug信息
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

			case WaterCmd_RebootDevice:			// "表端重启"
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

			case WaterCmd_ReadTemperature:			// " 读表温度 "
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

			case WaterCmd_ReadVoltage:				// " 读表电压 "
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
			
			case WaterCmd_ClearPrepaidRefVal:		// " 清预缴参考量 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x08;		// 命令选项 08	
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_SetOverCurrentTimeout:		// " 设置过流超时 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_OverCurr -1] == Param_Unique){		
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_OverCurr], 2 * 20);
					}
                    _Printfxy(7*16, (uiRowIdx)*16, "mA", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "过流电流:", StrBuf[0], 3, 8*8, true);
					_Printfxy(7*16, (uiRowIdx)*16, "ms", Color_White);
                    TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "超时时间:", StrBuf[1], 5, 8*8, true);
					break;
				}

				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " 请输入");
					currUi = 2;
					isUiFinish = false;
					continue;
				}

				u16Tmp = (uint16)_atof(StrBuf[0]);
				u32Tmp = (uint32)_atof(StrBuf[1]);
				if(u16Tmp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				if(u32Tmp > 65535){
					sprintf(StrBuf[1], " 0-65535");
					currUi = 2;
					isUiFinish = false;
					continue;
				}

				BackupBuf[ArgIdx_OverCurr -1] = Param_Unique;		
				memcpy(&BackupBuf[ArgIdx_OverCurr], &StrBuf[0][0], 2 * 20);
				
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 4;					// 应答长度 4	
				// 数据域
				Args.buf[i++] = 0x09;		// 命令选项 09
				Args.buf[i++] = (uint8)u16Tmp;			// 过流电流 0~255	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// 超时时间 0~65535	
				Args.buf[i++] = (uint8)(u32Tmp >> 8);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadOperatorNumber:		// " 读运营商编号 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 4;					// 应答长度 5	
				// 数据域
				Args.buf[i++] = 0x0A;		// 命令选项 0A	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadReportRoute:		// " 读上报路径 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// 命令字	07
				ackLen = 63;				// 应答长度 63	
				// 数据域
				Args.buf[i++] = 0x0E;		// 命令选项 0E	
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_SetMeterNumber:		// " 设置表号 ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					#if (AddrLen == 6)
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "新表号:", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);
					#else
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "新表号:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);	
					#endif
					break;
				}

				// 先读取参数配置信息 - 获取版本号
				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
					sprintf(StrDstAddr, " 请输入");
					currUi = 0;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				
				Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
				i = 0;
				Args.buf[i++] = 0x04;		// 命令字	04
				ackLen = 128;				// 应答长度 128	
				timeout = 10000 + (Addrs.itemCnt - 2) * 6000 * 2;
				tryCnt = 3;
				Args.lastItemLen = i - 1;

				// 若读取到版本号，则设置表号
				if(CmdResult_Ok == ProtolCommandTranceiver(WaterCmd_ReadMeterCfgInfo, &Addrs, &Args, ackLen, timeout, tryCnt)){
					i = 0;
					Args.buf[i++] = 0x07;		// 命令字	07
					ackLen = 1;					// 应答长度 1	
					timeout = 10000 + (Addrs.itemCnt - 2) * 6000 * 2;
					tryCnt = 3;
					// 数据域
					Args.buf[i++] = 0x10;		// 命令选项 10	
					memcpy(&Args.buf[i], &VerInfo[0], VerLen);	
					i += VerLen;					// 软件版本号
					GetBytesFromStringHex(&Args.buf[i], 0, AddrLen, StrBuf[0], 0, false);
					i += AddrLen;						// 新地址
					Args.lastItemLen = i - 1;

					if(CmdResult_Ok == ProtolCommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt)){
						memcpy(StrDstAddr, StrBuf[0], TXTBUF_LEN);
					}
				}

				key = _ReadKey();

				// 继续 / 返回
				if (key == KEY_CANCEL){
					break;
				}else{
					isUiFinish = false;
					continue;
				}
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
				uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
				
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

	// 菜单
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " 请输入");
					currUi = 2;
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
					currUi = 1;
					isUiFinish = false;
					continue;
				}

				u16Tmp = (uint16)_atof(StrBuf[0]);
				if(u16Tmp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUi = 4;
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
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				u32Tmp = (uint32)_atof(StrBuf[0]);
				if(u32Tmp > 65535){
					sprintf(StrBuf[0], " 0-65535");
					currUi = 1;
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
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " 请输入");
					currUi = 2;
					isUiFinish = false;
					continue;
				}
				u16Tmp = (uint16) _atof(StrBuf[0]);
				if(u16Tmp > 255){
					sprintf(StrBuf[0], " 0-255");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				u32Tmp = (uint32) _atof(StrBuf[1]);
				if(u32Tmp > 65535){
					sprintf(StrBuf[1], " 0-65535");
					currUi = 2;
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
				uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
				
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
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;
	char strDstAddrBak[20];

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 10, NULL,
		"<<工作参数",
		10,
		"1. 设用量和脉冲系数",
		"2. 清除反转计量数据",
		"3. 读取功能使能状态",
		"4. 设置定时上传",
		"5. 设置定量上传",
		"6. 设置定时定量上传",
		"7. 读表端时钟",
		"8. 校表端时钟",
		"9. 读表端用量",
		"10.读表端参数"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
			sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);

			if(menuItemNo == 10){
				// 读表端参数命令使用广播地址 D4D4...
				memcpy(strDstAddrBak, StrDstAddr, 20);
				strncpy(StrDstAddr, "D4D4D4D4D4D4D4D4", AddrLen * 2);
			}

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
			CurrCmd = (0x50 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_SetBaseValPulseRatio:	// 设表底数脉冲系数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
						StrBuf[1][0] = 0x01;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 2 * 20);
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "用户用量:", StrBuf[0], 10, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "脉冲系数:", &StrBuf[1][0], 4, 
						"1", "10", "100", "1000");
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = 1;
					isUiFinish = false;
					continue;
				}

				memcpy(&BackupBuf[ArgIdx_MtrValPalse], &StrBuf[0][0], 2 * 20);
				BackupBuf[ArgIdx_MtrValPalse - 1] = Param_Unique;

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

			case WaterCmd_ClearReverseMeasureData:	// 清除反转计量数据
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0A;		// 命令字	0A
				ackLen = 6;					// 应答长度 6	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadFuncEnableState:	// 读取功能使能状态
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
			
			case WaterCmd_SetTimedUpload:		// 设置定时上传
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_Printfxy(7*16, (uiRowIdx)*16, "小时", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定时间隔:", StrBuf[0], 2, 3*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], "   ");
					currUi = 1;
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

            case WaterCmd_SetFixedValUpload:	// 设置定量上传
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_Printfxy(7*16, (uiRowIdx)*16, "立方米", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定量间隔:", StrBuf[0], 3, 4*8, true);
					break;
				}
				if((StrBuf[0][0] > '9' || StrBuf[0][0] < '0') 
					|| _atof(StrBuf[0]) > 255){
					sprintf(StrBuf[0], "   ");
					currUi = 1;
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

			case WaterCmd_SetTimedAndFixedValUpload:	// 设置定时定量上传
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
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				if((StrBuf[1][0] > '9' || StrBuf[1][0] < '0') 
					|| _atof(StrBuf[1]) > 255){
					sprintf(StrBuf[1], "   ");
					currUi = 2;
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

			case WaterCmd_ReadMeterTime:	// 读表端时钟
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

			case WaterCmd_SetMeterTime:		// 校表端时钟
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
					currUi = 2 + (i -1);
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

			case (0x50 + 9): 
				CurrCmd = WaterCmd_ReadRealTimeData;		// 读表端用量 / "读取用户用量"
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
   
			case (0x50 + 10): 
				CurrCmd = WaterCmd_ReadModuleRunningParams;		// 读表端参数 / 读取模块运行参数
				 
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x3A;		// 命令字	3A
				ackLen = 124;				// 应答长度 124	
				Args.lastItemLen = i - 1; 
				break;

			default: 
				break;
			}


			// 创建 “中继地址输入框” 后， 显示UI
			if(false == isUiFinish){
				uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
				
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if( StrDstAddr[0] != 'D' && (StrDstAddr[0] < '0' || StrDstAddr[0] > '9')){
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
		if(menuItemNo == 10 && StrDstAddr[0] == 'D'){
			// 广播命令失败时，恢复原来地址
			memcpy(StrDstAddr, strDstAddrBak, 20);
		}
	}
}

// 6	其他操作
void WaterCmdFunc_Other(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish, u8Tmp;
	uint16 ackLen = 0, timeout, u16Tmp;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 5, NULL,
		"<<其他操作",
		5,
		"1. 读收发磁扰阀控数",
		"2. 读取RXD和TXD信道",
		"3. 设置RXD和TXD信道",
		"4. 设置运营商编号",
		"5. 路径下发"
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
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
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
			CurrCmd = (0x60 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRxTxMgnDistbCnt:		// 读收/发/磁扰次数
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

			case WaterCmd_ReadRxdAndTxdChanel:	// 读取RXD和TXD信道
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x1B;		// 命令字	1B
				ackLen = 3;					// 应答长度 3	
				// 数据域
				Args.buf[i++] = 0x00;		// 读取类别 0 - 抄表信道 和 上传信道
				Args.buf[i++] = 0;			// 抄表信道 bit7 0-读取 ，1-设置
				Args.buf[i++] = 0;			// 上传信道 bit7 0-读取 ，1-设置
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetRxdAndTxdChanel:	// 设置RXD和TXD信道
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "抄表信道:", StrBuf[0], 2, 11*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "上传信道:", StrBuf[1], 2, 11*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " 请输入");
					currUi = 2;
					isUiFinish = false;
					continue;
				}
				u8Tmp = (uint8) _atof(StrBuf[0]);
				u16Tmp = (uint16) _atof(StrBuf[1]);

				Args.buf[i++] = 0x1B;		// 命令字	1B
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x00;		// 设置类别 0 - 抄表信道 和 上传信道
				Args.buf[i++] = (0x80 | u8Tmp);			// 抄表信道 bit7 0-读取 ，1-设置
				Args.buf[i++] = (0x80 | (uint8)u16Tmp);	// 上传信道 bit7 0-读取 ，1-设置
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_SetOperatorNumber:		// 设置运营商编号
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "运营商编号:", StrBuf[0], 8, 9*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " 请输入");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x21;		// 命令字	21
				ackLen = 2;					// 应答长度 2	
				// 数据域
				GetBytesFromStringHex(&Args.buf[i], 0, 4, StrBuf[0], 0, false);
				i += 4;
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_SetDefinedRoute:	// 路径下发
				/*---------------------------------------------*/
				_Printfxy(0, 9*16, "返回 <等待输入> 继续", Color_White);
				if(false == isUiFinish){
					if(StrBuf[1][0] == 0x00){
						StrBuf[1][0] = '0';
					}
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "路径长度: ", &StrBuf[0][0], 4, 
						"2", "3", "4", "5");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "当前位置: ", StrBuf[1], 1, 10*8, true);
					break;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " 请输入");
					currUi = 2;
					isUiFinish = false;
					continue;
				}
				u8Tmp = (uint8)StrBuf[0][0] + 2;	// 路径长度
				u16Tmp = (uint16) _atof(StrBuf[1]);	// 当前位置
				if(u16Tmp > u8Tmp - 2){
					sprintf(StrBuf[1], " (此时<=%d)", (uint8)StrBuf[0][0]);
					currUi = 2;
					isUiFinish = false;
					continue;
				}
				while(1){
					_Printfxy(0, 9*16, "返回 <等待输入> 执行", Color_White);
					_Printfxy(0, 2*16, "路径的各节点地址:", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 3;
					StrBuf[0][0] = '\0';
					StrBuf[1][0] = '\0';
					for(i = 0; i < u8Tmp; i++){
						PrintfXyMultiLine_VaList(0, (uiRowIdx)*16, "%d: ", i);
						if(i == u16Tmp){	// 当前位置
							memcpy(StrBuf[i], StrDstAddr, TXTBUF_LEN);
							_Printfxy(4*8, (uiRowIdx)*16, StrBuf[i], Color_White);
						}
						else{
							TextBoxCreate(&pUi[(*pUiCnt)++], 3*8, (uiRowIdx)*16, " ", StrBuf[i], AddrLen*2, (AddrLen*2*8 + 8), true);
						}
						uiRowIdx++;
					}
					currUi = 0;
					key = ShowUI(UiList, &currUi);
					isUiFinish = true;

					if (key == KEY_CANCEL){
						break;
					}
					currUi = 0;
					for(i = 0; i < u8Tmp; i++){
						if(i != u16Tmp){
							currUi++;
						}
						if(StrBuf[i][0] > '9' || StrBuf[i][0] < '0'){
							sprintf(StrBuf[i], " 请输入");
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
				i = 0;
				Args.buf[i++] = 0x22;		// 命令字	22
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = (uint8)u16Tmp;	// 当前位置
				Args.buf[i++] = u8Tmp;			// 路径长度
				for(u16Tmp = 0; u16Tmp < u8Tmp; u16Tmp++){
					GetBytesFromStringHex(&Args.buf[i], 0, 6, StrBuf[u16Tmp], 0, false);
					i += 6;
				}
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_ReadModuleRunningParams:		// 读取模块运行参数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x3A;		// 命令字	3A
				ackLen = 124;				// 应答长度 124	
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}

			// 创建 “中继地址输入框” 后， 显示UI
			if(false == isUiFinish){
				uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);

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

			if (key == KEY_CANCEL){
				break;
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
	
	menu.left=0;
	menu.top=0;
	#ifdef UseFunc_BatchOpenCloseValve
	menu.no=8;
	#else
	menu.no=7;
	#endif
	menu.title= "<<工程调试 ";		// 工程调试 --> 即原来的 表端操作
	menu.str[0]=" 常用命令 ";
	menu.str[1]=" 测试命令 ";
	menu.str[2]=" 程序升级 ";
	menu.str[3]=" 预缴用量 ";
	menu.str[4]=" 工作参数 ";
	menu.str[5]=" 其他操作 ";
	#ifdef UseFunc_BatchOpenCloseValve
	menu.str[6]=" 批量开关阀 ";
	menu.str[7]=" 版本信息 ";
	#else
	menu.str[6]=" 版本信息 ";
	#endif
	menu.key[0]="1";
	menu.key[1]="2";
	menu.key[2]="3";
	menu.key[3]="4";
	menu.key[4]="5";
	menu.key[5]="6";
	menu.key[6]="7";
	#ifdef UseFunc_BatchOpenCloseValve
	menu.key[7]="8";
	#endif
	menu.Function[0]=WaterCmdFunc_CommonCmd;
	menu.Function[1]=WaterCmdFunc_TestCmd;
	menu.Function[2]=WaterCmdFunc_Upgrade;
	menu.Function[3]=WaterCmdFunc_PrepaiedVal;
	menu.Function[4]=WaterCmdFunc_WorkingParams;
	menu.Function[5]=WaterCmdFunc_Other;
	#ifdef UseFunc_BatchOpenCloseValve
	menu.Function[6]=BatchOpenCloseValve;
	menu.Function[7]=VersionInfoFunc;
	#else
	menu.Function[6]=VersionInfoFunc;
	#endif
	menu.FunctionEx=0;
	_Menu(&menu);	
}


//-----------------------------------	主界面	---------------------------
// 读取用户用量
void MainFuncReadRealTimeData(void)
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
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
			
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

// 设用量和脉冲系数
void MainFuncSetMeterValAndPulseRatio(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// 公共部分 :  界面显示
		sprintf(CurrCmdName, "<<设用量和脉冲系数");
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
		CurrCmd = WaterCmd_SetBaseValPulseRatio;	// 设用量和脉冲系数

		
		switch(CurrCmd){
		case WaterCmd_SetBaseValPulseRatio:		// 设用量和脉冲系数
			/*---------------------------------------------*/
			if(false == isUiFinish){
				if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
					StrBuf[1][0] = 0x01;
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 2 * 20);
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

			memcpy(&BackupBuf[ArgIdx_MtrValPalse], &StrBuf[0][0], 2 * 20);
			BackupBuf[ArgIdx_MtrValPalse - 1] = Param_Unique;

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

		default: 
			break;
		}

		// 创建 “中继地址输入框” 后， 显示UI
		if(false == isUiFinish){
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
			
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
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
			
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
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
			
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
				currUi = 2 + (i -1);
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
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
			
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
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
			
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
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
			
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
			uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);

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

// 工程调试		------------------------------
void MainFuncEngineerDebuging(void)
{
	WaterCmdFunc();		// 工程调试 --> 即原来的 表端操作
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
	MainMenu.str[0] = "读取用户用量";
	MainMenu.str[1] = "设用量和脉冲系数";
	MainMenu.str[2] = "读取表端时钟";
	MainMenu.str[3] = "设置表端时钟";
	MainMenu.str[4] = "开阀";
	MainMenu.str[5] = "关阀";
	MainMenu.str[6] = "工程调试";
	MainMenu.str[7] = "批量抄表";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.key[7] = "8";
	MainMenu.Function[0] = MainFuncReadRealTimeData;
	MainMenu.Function[1] = MainFuncSetMeterValAndPulseRatio;
	MainMenu.Function[2] = MainFuncReadMeterTime;
	MainMenu.Function[3] = MainFuncSetMeterTime;
	MainMenu.Function[4] = MainFuncOpenValve;
	MainMenu.Function[5] = MainFuncCloseValve;
	MainMenu.Function[6] = MainFuncEngineerDebuging;
	MainMenu.Function[7] = MainFuncBatchMeterReading;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();
	
}

