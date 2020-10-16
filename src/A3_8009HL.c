/*
*
*	桑锐8009手持机-FSK版本（和龙-定制版）
*
*/
#include "A3_8009HL.h"

//#define Use_CoreFunc			// 是否只启用核心功能：批量抄表、读表数据、开阀、关阀、清异常

#ifdef Use_CoreFunc
#include "HJLIB.H"
#include "string.h"
#include "stdio.h"
#include "common.h"
#include "common.c"
#include "MeterDocDBF_HL.h"		
#include "MeterDocDBF_HL.c"
#include "WaterMeter8009.h"
#else
#include "MeterDocDBF_HL.h"		
#include "MeterDocDBF_HL.c"
#include "SR8009_RF.c"         //  DBF数据库接口不同，其他功能与 SR8009_RF 一样
#endif

#ifdef Use_CoreFunc
// 常用功能
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
	ListBoxCreate(&menuList, 0, 0, 20, 7, 4, NULL,
		"<<工程调试",
		4,
		"1. 读表数据",
		"2. 表开阀",
		"3. 表关阀",
		"4. 清表状态"
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



// 工程调试		
void MainFuncEngineerDebuging(void)
{
	WaterCmdFunc_CommonCmd();	// 工程调试 --> 即原来的 常用功能
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
	MainMenu.no=3;
	MainMenu.title =  VerInfo_Name;
	MainMenu.str[0] = " 批量抄表 ";
	MainMenu.str[1] = " 工程调试 ";
	MainMenu.str[2] = " 版本信息 ";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.Function[0] = MainFuncBatchMeterReading;
	MainMenu.Function[1] = MainFuncEngineerDebuging; // 工程调试 --> 即原来的 常用功能
	MainMenu.Function[2] = VersionInfoFunc;	
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();

	#if LOG_ON
		LogPrint("抄表程序 已退出！\n\n\n");
	#endif

}

#endif