/*
*
*	桑锐8009水表手持机 - RF版
*
*/
#ifndef Protocol_8009
#include "SR8009_RF.h"
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
#include "WaterMeter8009.h"


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
			_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen <= 6)
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
						// 有效值 
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
					sprintf(StrDstAddr, " ");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 发送、接收、结果显示
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
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
			_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen <= 6)
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
					sprintf(StrDstAddr, " ");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 发送、接收、结果显示
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
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
			_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen <= 6)
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
					sprintf(StrDstAddr, " ");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 发送、接收、结果显示
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
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
			_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen <= 6)
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
					sprintf(StrDstAddr, " ");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);

			// 发送、接收、结果显示
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
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

// 1	常用功能
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish, u8Tmp;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<常用功能",
		7,
		"1. 读表数据",
		"2. 设表底数",
		"3. 表开阀",
		"4. 表关阀",
		"5. 清除异常",
		"6. 读表参数",
		"7. 设置表号"
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
			_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen <= 6)
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
			case WaterCmd_ReadRealTimeData:		// 读表数据
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x01;		// 命令字	01
				ackLen = 9;				// 应答长度 9/13	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetBaseValPulseRatio:	// 设表底数 和 脉冲系数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
						StrBuf[1][0] = 0x01;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 2 * 20);
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表底数:", StrBuf[0], 9, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表端口径:", &StrBuf[1][0], 2, 
						"小", "大");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "脉冲系数:", &StrBuf[1][1], 3, 
						"1", "10", "100");
					break;
				}
				if(false == StringToDecimal(StrBuf[0], 2, &u8Tmp, &u32Tmp, &u16Tmp)){
					sprintf(StrBuf[0], " ");
					currUi = 1;
					isUiFinish = false;
					continue;
				}

				memcpy(&BackupBuf[ArgIdx_MtrValPalse], &StrBuf[0][0], 2 * 20);
				BackupBuf[ArgIdx_MtrValPalse - 1] = Param_Unique;

				Args.buf[i++] = 0x04;		// 命令字	04
				ackLen = 5;					// 应答长度 5	
				// 数据域
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);		// 表底数：整数 3byte
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);				// 表底数：小数 1byte
				switch (StrBuf[1][1])
				{
				case 0: u8Tmp = 1; break;
				case 1: u8Tmp = 10; break;
				case 2: u8Tmp = 100; break;
				default: u8Tmp = 10; break;
				}
				Args.buf[i++] = (uint8)((StrBuf[1][0] << 7) | u8Tmp);	// 表口径|脉冲系数	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadFrozenData:		// 读取冻结正转数据
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

			case WaterCmd_OpenValve:			// 开阀 
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x05;		// 命令字	05
				ackLen = 0;					// 应答长度 0	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_CloseValve:			// 关阀
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x06;		// 命令字	06
				ackLen = 0;					// 应答长度 0	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ClearException:		// 清异常命令
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// 命令字	03
				ackLen = 0;					// 应答长度 0	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadMeterCfgInfo:		// 读表参数
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x02;		// 命令字	02
				ackLen = 30;				// 应答长度 30	
				// 数据域
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetMeterNumber:		// 设置表号
				/*---------------------------------------------*/
				if(false == isUiFinish){
					#if (AddrLen <= 6)
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "新表号:", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);
					#else
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "新表号:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);	
					#endif
					break;
				}

				// 先读取参数配置信息 - 获取版本号
				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
					sprintf(StrDstAddr, " ");
					currUi = 0;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " ");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				
				Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
				i = 0;
				Args.buf[i++] = 0x02;		// 命令字	
				ackLen = 30;				// 应答长度	
				timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
				tryCnt = 2;	
				Args.lastItemLen = i - 1;

				// 若读取到版本号，则设置表号
				if(CmdResult_Ok == ProtolCommandTranceiver(WaterCmd_ReadMeterCfgInfo, &Addrs, &Args, ackLen, timeout, tryCnt)){
					i = 0;
					Args.buf[i++] = 0x1C;		// 命令字
					ackLen = 7;					// 应答长度 
					timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
					tryCnt = 2;	
					// 数据域
					Args.buf[i++] = 0xA2;		// 命令选项	
					memcpy(&Args.buf[i], &VerInfo[0], VerLen);	
					i += VerLen;				// 软件版本号
					GetBytesFromStringHex(&Args.buf[i], 0, 6, StrBuf[0], 0, false);
					i += 6;						// 新地址
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

			// 创建 “中继地址输入框” 后， 显示UI
			if(false == isUiFinish){

				uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);

				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
					sprintf(StrDstAddr, " ");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen <= 6)
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
			#endif

			// 发送、接收、结果显示
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
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

// 2	功能配置
void WaterCmdFunc_FunctionConfig(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList, menuList_2;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish, u8Tmp;
	uint16 ackLen = 0, timeout;
	uint8 uiPage, isSendReady = false;	// uiPage: 0 - 首页 , isSendReady: 发送就绪

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 8, NULL,
		"<<功能配置",
		8,
		"1. 读取反转用量",
		"2. 清除反转用量",
		"3. 读取功能状态old",
		"4. 读取功能状态new",
		"5. 设置功能状态",
		"6. 查询时钟及RF状态",
		"7. 查询RF工作时段",
		"8. 设置RF工作时段"
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
		uiPage = 1;

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
			_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

			if(uiPage == 1){
				if(false == isUiFinish){
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					#if (AddrLen <= 6)
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);
					#else
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "表 号:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);	
					#endif
				}
				i = 0;	
			}

			// 命令参数处理
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // 命令字
			Args.items[1] = &Args.buf[1];	// 数据域
			CurrCmd = (0x20 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadReverseMeasureData:		// 读取反转用量
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0A;		// 命令字	0A
				ackLen = 9;					// 应答长度 9	
				// 数据域
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_ClearReverseMeasureData:		// 清除反转用量
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x12;		// 命令字	12
				ackLen = 0;					// 应答长度 0	
				// 数据域
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_ReadFuncEnableStateOld:		// 读功能状态old
			case WaterCmd_ReadFuncEnableStateNew:		// 读功能状态new
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x17;		// 命令字	17
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_SetFuncEnableState:		// 设置功能状态
				/*---------------------------------------------*/
				if(uiPage == 1){
					if(false == isUiFinish){
						break;
					}
					uiPage++;
					isUiFinish = false;
					continue;
				}
				else if(uiPage == 2){
					if(false == isUiFinish){
						(*pUiCnt) = 0;
						uiRowIdx = 2;
						// 菜单
						ListBoxCreate(&menuList_2, 0, 0, 20, 7, 10, NULL,
							"<<选择设置项",
							10,
							"1. 磁干扰关阀-开启",
							"2. 磁干扰关阀-关闭",
							"3. 磁干扰检测-关闭",
							"4. 防拆卸-开启",
							"5. 防拆卸-关闭",
							"6. 垂直安装检测-开启",
							"7. 垂直安装检测-关闭",
							"8. 主动告警-开启",
							"9. 主动告警-关闭",
							"10.上报和防锈-设置"
						);
						key = ShowListBox(&menuList_2);
						if(key == KEY_CANCEL){
							uiPage--;
							continue;
						}
						sprintf(CurrCmdName, menuList_2.str[menuList_2.strIdx]);
						sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
						_Printfxy(0, 0, TmpBuf, Color_White);
						isUiFinish = true;
					}

					// 命令字	0C~16
					i = 0;
					switch (menuList_2.strIdx + 1)
					{
					case 1: Args.buf[i++] = 0x0C; break;
					case 2: Args.buf[i++] = 0x0D; break;
					case 3: Args.buf[i++] = 0x0E; break;
					case 4: Args.buf[i++] = 0x0F; break;
					case 5: Args.buf[i++] = 0x10; break;
					case 6: Args.buf[i++] = 0x13; break;
					case 7: Args.buf[i++] = 0x14; break;
					case 8: Args.buf[i++] = 0x15; break;
					case 9: Args.buf[i++] = 0x16; break;
					case 10: break;
					default:
						break;
					}

					if(menuList_2.strIdx + 1 == 10){
						uiPage++;
						isUiFinish = false;
						continue;
					}
					
					ackLen = 0;					// 应答长度 0	
					// 数据域
					Args.lastItemLen = i - 1;
					isSendReady = true;
					break;
				}
				else if(uiPage == 3){
					sprintf(CurrCmdName, menuList_2.str[menuList_2.strIdx]);
					sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
					_Printfxy(0, 0, TmpBuf, Color_White);
					if(false == isUiFinish){
						(*pUiCnt) = 0;
						uiRowIdx = 2;
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定时防锈:", &StrBuf[1][0], 2, 
						"关闭", "开启");
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定时上报:", &StrBuf[1][1], 2, 
						"关闭", "开启");
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "定量上报:", &StrBuf[1][2], 2, 
						"关闭", "开启");
						break;
					}
					
					// 命令字	19-00/01/02/04/08
					i = 0;
					Args.buf[i++] = 0x19;
					ackLen = 0;					// 应答长度 0	
					// 数据域
					// 子命令字：b0-防锈开，b1-防锈关，b2-定时开/关，b3-定量开/关
					u8Tmp = StrBuf[1][0] == 0 ? 0x02 : 0x01;
					u8Tmp |= StrBuf[1][1] << 2;
					u8Tmp |= StrBuf[1][2] << 3;
					Args.buf[i++] = u8Tmp;		// 子命令字
					Args.lastItemLen = i - 1;
					isSendReady = true;
				}
				break;

			case WaterCmd_ReadTimeAndRfState:		// 查询时钟及RF状态
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// 命令字	09-00
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x00;		// 子命令字 00
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_ReadRfWorkTime:		// 查询RF工作时段
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// 命令字	09-04
				ackLen = 2;					// 应答长度 2	
				// 数据域
				Args.buf[i++] = 0x04;
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_SetRfWorkTime:		// 设置RF工作时段
				/*---------------------------------------------*/
				if(uiPage == 1){
					if(false == isUiFinish){
						LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "工作模式:");
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " ", &StrBuf[0][0], 2, 
						"每月工作时段", "每天工作时段");
						break;
					}
					uiPage++;
					isUiFinish = false;
					continue;
				}
				else if(uiPage == 2){
					if(false == isUiFinish){
						(*pUiCnt) = 0;
						uiRowIdx = 2;

						if(StrBuf[0][0] == 0){
							if(StrBuf[1][0] == 0x00){
								sprintf(StrBuf[1], "08");
								sprintf(StrBuf[2], "25");
								sprintf(StrBuf[3], "06");
							}
							LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "每月工作时段:");
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "开始日(1-31):", StrBuf[1], 2, 2*8, true);
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "结束日(1-31):", StrBuf[2], 2, 2*8, true);
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "开始时(0-23):", StrBuf[3], 2, 2*8, true);
						}
						else{
							if(StrBuf[4][0] == 0x00){
								sprintf(StrBuf[4], "06");
								sprintf(StrBuf[5], "19");
							}
							LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "每天工作时段:");
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "开始时(0-23):", StrBuf[4], 2, 2*8, true);
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "结束时(0-23):", StrBuf[5], 2, 2*8, true);
						}
						break;
					}

					if(StrBuf[0][0] == 0){
						TmpBuf[1] = (uint8) _atof(StrBuf[1]);
						TmpBuf[2] = (uint8) _atof(StrBuf[2]);
						TmpBuf[3] = (uint8) _atof(StrBuf[3]);
						if(TmpBuf[1] < 1 || TmpBuf[1] > 31){
							currUi = 1;
							isUiFinish = false;
							continue;
						}
						if(TmpBuf[1] < 1 || TmpBuf[1] > 31 ){
							currUi = 2;
							isUiFinish = false;
							continue;
						}
						if(TmpBuf[3] > 23){
							currUi = 3;
							isUiFinish = false;
							continue;
						}
						TmpBuf[10] = DecToBcd(TmpBuf[1]);
						TmpBuf[11] = DecToBcd(TmpBuf[3]);
						TmpBuf[12] = DecToBcd(TmpBuf[2]);
						TmpBuf[13] = DecToBcd(TmpBuf[3]);
					}
					else{
						TmpBuf[4] = (uint8) _atof(StrBuf[4]);
						TmpBuf[5] = (uint8) _atof(StrBuf[5]);
						TmpBuf[6] = _GetDay();
						if(TmpBuf[4] > 23){
							currUi = 1;
							isUiFinish = false;
							continue;
						}
						if(TmpBuf[5] > 23){
							currUi = 2;
							isUiFinish = false;
							continue;
						}
						TmpBuf[10] = DecToBcd(TmpBuf[6]);
						TmpBuf[11] = DecToBcd(TmpBuf[4]);
						TmpBuf[12] = DecToBcd(TmpBuf[6]);
						TmpBuf[13] = DecToBcd(TmpBuf[5]);
					}
					
					i = 0;
					Args.buf[i++] = 0x09;		// 命令字	09-84
					ackLen = 2;					// 应答长度 2	
					// 数据域
					Args.buf[i++] = 0x84;		// 子命令 84
					Args.buf[i++] = TmpBuf[10];	// 4字节时钟数据
					Args.buf[i++] = TmpBuf[11];
					Args.buf[i++] = TmpBuf[12];
					Args.buf[i++] = TmpBuf[13];
					Args.lastItemLen = i - 1;
					isSendReady = true;
				}
				
				break;
				
			default: 
				break;
			}

			// 创建 “中继地址输入框” 后， 显示UI
			if(uiPage == 0){
				break;
			}

			if(false == isSendReady){
				if(false == isUiFinish){

					if(uiPage == 1){
						uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
					}

					key = ShowUI(UiList, &currUi);

					isUiFinish = true;

					if (key == KEY_CANCEL){
						uiPage--;
						isUiFinish = false;
					}

					if(uiPage == 1){
						if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
							sprintf(StrDstAddr, " ");
							currUi = 0;
							isUiFinish = false;
						}
					}
				}
				continue;	// go back to get ui args
			}

			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen <= 6)
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
			#endif

			// 发送、接收、结果显示
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			isSendReady = false;
			isUiFinish = false;
			
			// 继续 / 返回
			if (key == KEY_CANCEL){
				uiPage--;
			}
		}
		
	}
}

// 3	DMA项目
void WaterCmdFunc_DmaProjectCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 *time = &TmpBuf[200], *timeBytes = &TmpBuf[300];
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout, u16Tmp;

	_ClearScreen();

	// 菜单
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<功能配置",
		7,
		"1. 主动上传中心频点",
		"2. 使能上传电表数据",
		"3. 设置上传时间间隔",
		"4. 读取上传时间间隔",
		"5. 读冻结正转数据",
		"6. 设置时钟",
		"7. 读取时钟"
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
			_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen <= 6)
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
			CurrCmd = (0x30 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_UploadCenterFrequency:		// 主动上传中心频点
				/*---------------------------------------------*/
				if(false == isUiFinish){
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "中心频点:");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "", &StrBuf[1][0], 2, 
						"0: 470 MHz", "1: 478.3 MHz");
					break;
				}
				Args.buf[i++] = 0x09;		// 命令字	01
				ackLen = 2;					// 应答长度 2/1	
				// 数据域
				Args.buf[i++] = 0x09 | (StrBuf[1][0] << 7);	// 子命令字	09/89
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_EnableReportAmeterData:		// 使能上传电表数据
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// 命令字	09
				ackLen = 2;					// 应答长度 2/1	
				// 数据域
				Args.buf[i++] = 0x8B;		// 子命令字	0B/8B - 不上传/立即上传
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetReportTimeInterval:		// 设置上传时间间隔 
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(StrBuf[0][0] == 0x00){
						sprintf(StrBuf[0], "  (0-255 h)");
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "时间间隔:", StrBuf[0], 3, 11*8, true);
					break;
				}
				u16Tmp = (uint16)_atof(StrBuf[0]);
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0' || u16Tmp > 255){
					sprintf(StrBuf[0], "  (0-255 h)");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x09;		// 命令字	09
				ackLen = 2;					// 应答长度 2/1	
				// 数据域
				Args.buf[i++] = 0x8A;		// 子命令字	0A/8A - 读取/设置
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);	
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_ReadReportTimeInterval:		// 读取上传时间间隔
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// 命令字	09
				ackLen = 2;					// 应答长度 2/1	
				// 数据域
				Args.buf[i++] = 0x0A ;		// 子命令字	0A/8A - 读取/设置
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadFrozenData:			// 读冻结正转数据
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
				Args.buf[i++] = 0x09;		// 命令字	09
				ackLen = 0;					// 应答长度 0	
				// 数据域
				Args.buf[i++] = 0x05;		// 子命令字	05/06 - 读正转/读反转
				Args.buf[i++] = StrBuf[0][0] - '0';	// 冻结数据序号	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetMeterTime:				// 校表端时钟 
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
				Args.buf[i++] = 0x01;		// 命令字	01 （抄表时带上时间）
				ackLen = 9;					// 应答长度 9/13	
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

			case WaterCmd_ReadMeterTime:	// 读表端时钟
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// 命令字	09
				ackLen = 0;					// 应答长度 0	
				// 数据域
				Args.buf[i++] = 0x00;		// 子命令字	00
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
					sprintf(StrDstAddr, " ");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// 地址填充
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen <= 6)
			PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
			#endif

			// 发送、接收、结果显示
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
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
	menu.no=4;
	#else
	menu.no=3;
	#endif
	menu.title= "<<工程调试 ";		// 工程调试 --> 即原来的 表端操作
	menu.str[0]=" 常用功能 ";
	menu.str[1]=" 功能配置 ";
	menu.str[2]=" DMA项目 ";
	#ifdef UseFunc_BatchOpenCloseValve
	menu.str[3]=" 批量开关阀 ";
	#endif
	menu.key[0]="1";
	menu.key[1]="2";
	menu.key[2]="3";
	#ifdef UseFunc_BatchOpenCloseValve
	menu.key[3]="4";
	#endif
	menu.Function[0]=WaterCmdFunc_CommonCmd;
	menu.Function[1]=WaterCmdFunc_FunctionConfig;
	menu.Function[2]=WaterCmdFunc_DmaProjectCmd;
	#ifdef UseFunc_BatchOpenCloseValve
	menu.Function[3]=BatchOpenCloseValve;
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
		_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen <= 6)
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
		case WaterCmd_ReadRealTimeData:		// 读表数据
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x01;		// 命令字	01
			ackLen = 9;				// 应答长度 9/13	
			// 数据域
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
				sprintf(StrDstAddr, " ");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen <= 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

		// 发送、接收、结果显示
		key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
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
		_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen <= 6)
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
			Args.buf[i++] = 0x03;		// 命令字	03
			ackLen = 0;					// 应答长度 0	
			// 数据域
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
				sprintf(StrDstAddr, " ");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen <= 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

		// 发送、接收、结果显示
		key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
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
		_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen <= 6)
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
			Args.buf[i++] = 0x05;		// 命令字	05
			ackLen = 0;					// 应答长度 0	
			// 数据域
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
				sprintf(StrDstAddr, " ");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen <= 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

		// 发送、接收、结果显示
		key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
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
		_Printfxy(0, 9*16, "返回   <输入>   确定", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen <= 6)
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
			Args.buf[i++] = 0x06;		// 命令字	06
			ackLen = 0;					// 应答长度 0	
			// 数据域
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
				sprintf(StrDstAddr, " ");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// 地址填充
		Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen <= 6)
		PrintfXyMultiLine_VaList(0, 2*16, "表 号: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "表 号:\n   %s", StrDstAddr);
		#endif

		// 发送、接收、结果显示
		key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
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
	
	#if LOG_ON
		LogPrint("抄表程序 已进入 \n");
	#endif

	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=7;
	MainMenu.title =  VerInfo_Name;
	MainMenu.str[0] = " 批量抄表 ";
	MainMenu.str[1] = " 读表数据 ";
	MainMenu.str[2] = " 开阀 ";
	MainMenu.str[3] = " 关阀 ";
	MainMenu.str[4] = " 清异常 ";
	MainMenu.str[5] = " 工程调试 ";
	MainMenu.str[6] = " 版本信息 ";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.Function[0] = MainFuncBatchMeterReading;
	MainMenu.Function[1] = MainFuncReadRealTimeData;
	MainMenu.Function[2] = MainFuncOpenValve;
	MainMenu.Function[3] = MainFuncCloseValve;
	MainMenu.Function[4] = MainFuncClearException;
	MainMenu.Function[5] = MainFuncEngineerDebuging;	// 工程调试 --> 即原来的 表端操作
	MainMenu.Function[6] = VersionInfoFunc;	
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();

	#if LOG_ON
		LogPrint("抄表程序 已退出！\n\n\n");
	#endif

}

