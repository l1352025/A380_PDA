#include <HJLIB.H>
#include "string.h"
//#include "dbf.h"
#include "stdio.h"

#include "Common.h"
#include "Tool.h"
//#include "PowerMeter.h"
#include "WaterMeter.h"


// --------------------------------  全局变量  -----------------------------------------
char Screenbuff[160*(160/3+1)*2]; 
uint8 TmpBuf[1080];
uint8 TxBuf[1080];
uint8 RxBuf[1080];
uint32 RxLen, TxLen;
const char * ZeroAddr = "000000000000";	    // 0 地址，12字符
const uint8 LocalAddr[7] = { 0x20, 0x19, 0x00, 0x00, 0x20, 0x19, 0x00};	// 地址 201900002019，12字符
uint8 DstAddr[7];
uint8 CurrCmd;
ParamsBuf Addrs;		
ParamsBuf Args;
ParamsBuf Disps;
uint8 InputBuff_1[20] = {0};
uint8 InputBuff_2[20] = {0};
uint8 InputBuff_Tmp1[20] = {0};
uint8 InputBuff_Tmp2[20] = {0};

_GuiInputBoxStru NewInput;		// 全局可复用得输入框

// --------------------------------  集中器模块通信  -----------------------------------------
// 1	常用命令
void CenterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 常用命令";
	menuList.no = 9;
	menuList.MaxNum = 9;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读集中器号";
	menuList.str[1] = "  2. 读集中器版本";
	menuList.str[2] = "  3. 读集中器时钟";
	menuList.str[3] = "  4. 设集中器时钟";
	menuList.str[4] = "  5. 读GPRS参数";
	menuList.str[5] = "  6. 设GPRS参数";
	menuList.str[6] = "  7. 读GPRS信号强度";
	menuList.str[7] = "  8. 集中器初始化";
	menuList.str[8] = "  9. 读集中器工作模式";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

// 2	档案操作
void CenterCmdFunc_DocumentOperation(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 档案操作";
	menuList.no = 5;
	menuList.MaxNum = 5;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读档案数量";
	menuList.str[1] = "  2. 读档案信息";
	menuList.str[2] = "  3. 添加档案信息";
	menuList.str[3] = "  4. 删除档案信息";
	menuList.str[4] = "  5. 修改档案信息";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

// 3	路径设置
void CenterCmdFunc_RouteSetting(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 路径设置";
	menuList.no = 2;
	menuList.MaxNum = 2;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读自定义路由";
	menuList.str[1] = "  2. 设自定义路由";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

// 4	命令转发
void CenterCmdFunc_CommandTransfer(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<< 命令转发";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读实时数据";
	menuList.str[1] = "  2. 读定时定量数据";
	menuList.str[2] = "  3. 读冻结数据";
	menuList.str[3] = "  4. 开阀";
	menuList.str[4] = "  5. 关阀";
	menuList.str[5] = "  6. 读使能";
	menuList.str[6] = "  7. 清异常";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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


void CenterCmdFunc(void)
{
	_GuiMenuStru menu;
	
	menu.left=0;
	menu.top=0;
	menu.no=4;
	menu.title= "<< 集中器通信 ";
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
	
}


// --------------------------------  电表模块通信  -----------------------------------------

void PowerCmdFunc(void)
{

	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	return;
	
	#if 0

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
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
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
			_Printfxy(0, 2*16, "表地址:", 0);
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
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			switch(menuItemNo){
			case 1:		// " 读取软件版本 ";
				CurrCmd = PowerCmd_ReadVerInfo;
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case 2:		// " 读取节点配置 "
				CurrCmd = PowerCmd_ReadNodeInfo;
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case 3:		// " 读取发射功率" "
				CurrCmd = PowerCmd_ReadSendPower;
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case 4:		// " 645-07抄表 ";
				CurrCmd = PowerCmd_ReadMeter_645_07;
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			if(false == ExplainElectricResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
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

	#endif
}

// --------------------------------  水表模块通信  -----------------------------------------

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
	menuList.title = "<<常用命令";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读取用户用量";
	menuList.str[1] = "  2. 读取冻结正转数据";
	menuList.str[2] = "  3. 开阀";
	menuList.str[3] = "  4. 强制开阀";
	menuList.str[4] = "  5. 关阀";
	menuList.str[5] = "  6. 强制关阀";
	menuList.str[6] = "  7. 清异常命令";
	menuList.defbar = 1;
	_GUIHLine(0, 1*16, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			sprintf(TmpBuf, "<<%s",&ptr[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 4;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<<测试命令";
	menuList.no = 8;
	menuList.MaxNum = 8;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 表端重启";
	menuList.str[1] = "  2. 读表温度";
	menuList.str[2] = "  3. 读表电压";
	menuList.str[3] = "  4. 清预缴参考量";
	menuList.str[4] = "  5. 设置过流超时";
	menuList.str[5] = "  6. 读运营商编号";
	menuList.str[6] = "  7. 读上报路径";
	menuList.str[7] = "  8. 设置表号";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			sprintf(TmpBuf, "<<%s",&ptr[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

// 3	程序升级
void WaterCmdFunc_Upgrade(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<<程序升级";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 单表升级";
	menuList.str[1] = "  2. 查询升级";
	menuList.str[2] = "  3. 广播升级";
	menuList.str[3] = "  4. 添加档案";
	menuList.str[4] = "  5. 删除档案";
	menuList.str[5] = "  6. 查询档案";
	menuList.str[6] = "  7. 升级统计";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			sprintf(TmpBuf, "<<%s",&ptr[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

// 4	预缴用量
void WaterCmdFunc_PrepaiedVal(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<<预缴用量";
	menuList.no = 6;
	menuList.MaxNum = 6;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 读预缴参考用量";
	menuList.str[1] = "  2. 设预缴参考用量";
	menuList.str[2] = "  3. 读报警限值透支";
	menuList.str[3] = "  4. 设报警限值";
	menuList.str[4] = "  5. 设关阀限值";
	menuList.str[5] = "  6. 设报警关阀限值";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			sprintf(TmpBuf, "<<%s",&ptr[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

// 5	工作参数
void WaterCmdFunc_WorkingParams(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<<工作参数";
	menuList.no = 8;
	menuList.MaxNum = 8;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "1. 设表底数脉冲系数";
	menuList.str[1] = "2. 清除反转计量数据";
	menuList.str[2] = "3. 读取功能使能状态";
	menuList.str[3] = "4. 设置定时上传";
	menuList.str[4] = "5. 设置定量上传";
	menuList.str[5] = "6. 设置定时定量上传";
	menuList.str[6] = "7. 读表端时钟";
	menuList.str[7] = "8. 校表端时钟";
	menuList.defbar = 1;

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			sprintf(TmpBuf, "<<%s",&ptr[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

// 6	其他操作
void WaterCmdFunc_Other(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	_GuiInputBoxStru inputBox;
	int inputLen;
	uint8 * ptr;

	_ClearScreen();

	// 菜单
	menuList.title = "<<其他操作";
	menuList.no = 5;
	menuList.MaxNum = 5;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "1. 读收/发/磁扰次数";
	menuList.str[1] = "2. 读取RXD和TXD信道";
	menuList.str[2] = "3. 设置RXD和TXD信道";
	menuList.str[3] = "4. 设置运营商编号";
	menuList.str[4] = "5. 路径下发";
	menuList.defbar = 1;
	//_GUIHLine(0, 4*16 + 8, 160, 1);

	// 输入框
	inputBox.top = 2 * 16;
	inputBox.left = 4 * 16;
	inputBox.width = 6 * 16;
	inputBox.hight = 16;
	inputBox.caption = "";
	inputBox.context = InputBuff_Tmp1;
	inputBox.type = 1;		// 数字
	inputBox.datelen = 12;	// 最大长度
	inputBox.keyUpDown = 1; 
	inputBox.IsClear = 1;
	_SetInputMode(1); 		//设置输入方式 
	_DisInputMode(1);		//输入法是否允许切换

	_CloseCom();
	_ComSetTran(3);
	_ComSet((uint8 *)"9600,E,8,1", 2);

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
			sprintf(TmpBuf, "<<%s",&ptr[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			/*---------------------------------------------*/
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			_Printfxy(0, 2*16, "表地址:", Color_White);
			//_Printfxy(0, 9*16, "返回            确定", Color_White);

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
                      // 6009 协议地址填写不用反序
			GetBytesFromStringHex(DstAddr, 0, inputBox.context, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "表地址: %s", InputBuff_1);

			// 命令参数处理
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "读取用户用量"
				/*---------------------------------------------*/
				// 地址
				Addrs.itemCnt = 2;
				Addrs.items[0] = &Addrs.buf[0];
                             Addrs.items[1] = &Addrs.buf[6];
                             memcpy(Addrs.items[0], LocalAddr, 6);
				memcpy(Addrs.items[1], DstAddr, 6);
				// 命令字、数据域
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //命令字
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				Args.itemCnt = 1;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
				memcpy(Args.buf, DstAddr, 6);
				Args.items[0] = &Args.buf[0];
				Args.buf[6] = 0;
				Args.items[1] = &Args.buf[6];
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				Args.itemCnt = 2;
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
			TxLen = PackWater6009RequestFrame(TxBuf, &Addrs, CurrCmd, &Args, 0);
			_GetComStr(RxBuf, 1024, 10);	// clear , 100ms timeout
			_SendComStr(TxBuf, TxLen);
			_Printfxy(0, 9*16, "状态: <命令发送>    ", Color_White);

			// 接收
			RxLen = _GetComStr(RxBuf, 100, 600);	// recv , 500 X 10ms timeout
			if(false == ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, &Disps)){
				PrintfXyMultiLine_VaList(0, 9*16, "状态: <失败，%s>    ", Disps.items[0]);
				_ReadKey();
				continue;
			}
			else{
				_Printfxy(0, 9*16, "状态: <命令成功>    ", Color_White);
			}
			
			// 显示结果
			_GUIRectangleFill(0, 0, 10*16, 8*16, Color_White);
			PrintfXyMultiLine_VaList(0, 0*16, "表地址: %s", InputBuff_1);
			PrintfXyMultiLine(0, 1 * 16, Disps.items[0]);

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

void WaterCmdFunc(void)
{

	_GuiMenuStru menu;
	
	menu.left=0;
	menu.top=0;
	menu.no=6;
	menu.title= "<< 水表通信 ";
	menu.str[0]=" 常用命令 ";
	menu.str[1]=" 测试命令 ";
	menu.str[2]=" 程序升级 ";
	menu.str[3]=" 预缴用量 ";
	menu.str[4]=" 工作参数 ";
	menu.str[5]=" 其他操作 ";
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
	MainMenu.no=2;
	MainMenu.title= "   桑锐手持机  ";
	MainMenu.str[0]=" 集中器通信 ";
	MainMenu.str[1]=" 水表通信 ";
	//MainMenu.str[2]=" 电表通信 ";
	//MainMenu.str[3]=" 透传模块升级 ";
	MainMenu.key[0]="1";
	MainMenu.key[1]="2";
	MainMenu.key[2]="3";
	MainMenu.key[3]="4";
	MainMenu.Function[0]=CenterCmdFunc;
	MainMenu.Function[1]=WaterCmdFunc;
	//MainMenu.Function[2]=PowerCmdFunc;
	//MainMenu.Function[3]=TransParentModuleFunc;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	
}

