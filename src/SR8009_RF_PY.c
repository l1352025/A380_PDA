/*
*
*	桑锐8009气表手持机 - RF版 (濮阳定制版)
*
*/
#include "SR8009_RF_PY.h"

#include "HJLIB.H"
#include "string.h"
#include "stdio.h"
#include "common.h"
#include "common.c"
#include "MeterDocDBF_8009_PY.h"
#include "MeterDocDBF_8009_PY.c"
#include "ProtoHandle_8009_PY.h"


//-----------------------------------	主界面	---------------------------

// 批量抄表
void MainFuncBatchMeterReading(void)
{
	uint8 key;
	ListBox menuList, menuList_2, menuList_3;
	ListBoxEx XqList;				// 抄表册列表
	_GuiInputBoxStru inputSt;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint8 *ptr;
	uint16 dispIdx, i;
	char *dispBuf = &DispBuf, *strTmp = &TmpBuf[0], *time = &TmpBuf[200];
	uint8 qryTypeXq;
	uint16 qryIndexXq;
	uint32 recCnt = 0;

	recCnt = FixDbfRecCnt();	// 修复记录总数
	MeterInfo.dbIdx = Invalid_dbIdx;  // 清空当前表数据库索引，防止抄表结果写入
	
	if(recCnt == 0){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  当前档案为空！\n 请先下载抄表档案，\n 再进行批量操作");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// 菜单
	//------------------------------------------------------------
	ListBoxCreate(&menuList, 0, 0, 20, 7, 5, NULL,
		"<<批量抄表",
		5,
		"1. 按抄表册抄表",
		"2. 户表查询",
		"3. 抄表统计",
		"4. 清空抄表结果",
		"5. 重置抄表时间"
	);
	while(1){

		_Printfxy(0, 9*16, "返回            确定", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// 返回
			break;
		}
		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		switch(menuList.strIdx + 1){
		case 1:		// 按抄表册抄表

			// 抄表册列表-界面
			//------------------------------------------------------------
			_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
			QuerySectList(&Sects, &DbQuery);
			ListBoxCreateEx(&XqList, 0, 0, 20, 7, Sects.cnt, NULL,
				"<<抄表册选择", Sects.nums, Size_ListStr, Sects.cnt);
			while(2){
				
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBoxEx(&XqList);
				//------------------------------------------------------------
				if (key == KEY_CANCEL){	// 返回
					break;
				}

				// 清空路由
				StrRelayAddr[0][0] = 0x00;
				StrRelayAddr[1][0] = 0x00;
				StrRelayAddr[2][0] = 0x00;

				// 抄表册抄表-界面
				//------------------------------------------------------------
				Meters.qrySectNum = Sects.nums[XqList.strIdx];
				strcpy(Meters.sectNum, Meters.qrySectNum);
				ListBoxCreate(&menuList_2, 0, 0, 20, 7, 6, NULL,
					"<<抄表册抄表", 
					6,
					"1. 自动抄表",
					"2. 已抄成功列表",
					"3. 未抄失败列表",
					"4. 抄表统计",
					"5. 清空抄表结果",
					"6. 重置抄表时间");
					//"7. 设置路由");
				while(3){

					_Printfxy(0, 9*16, "返回            确定", Color_White);
					key = ShowListBox(&menuList_2);
					//------------------------------------------------------------
					if(key == KEY_CANCEL){	// 返回
						break;
					}

					switch (menuList_2.strIdx + 1){
					case 1:		// 自动抄表
						Meters.selectField = Idx_MeterNum;
						Meters.qryMeterReadStatus = "0";
						_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
						QueryMeterList(&Meters, &DbQuery);
						key = ShowAutoMeterReading(&Meters);
						break;

					case 2:		// 已抄列表
						Meters.selectField = Idx_Invalid;
						Meters.qryMeterReadStatus = "1";
						key = ShowMeterList(&Meters);
						break;
					case 3:		// 未抄/失败列表
						Meters.selectField = Idx_Invalid;
						Meters.qryMeterReadStatus = "0";
						key = ShowMeterList(&Meters);
						break;

					case 4:		// 抄表统计
						Meters.selectField = Idx_Invalid;
						_Printfxy(0, 9*16, "    <  统计中  >    ", Color_White);
						QueryMeterList(&Meters, &DbQuery);
						key = ShowMeterReadCountInfo(&Meters);
						break;

					case 5:		// 清空抄表结果
						//-------------------------------------------------------
						_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
						_Printfxy(0, 4*16, " 确定要清空        ", Color_White);
						_Printfxy(0, 5*16, " 当前抄表册结果吗?  ", Color_White);
						_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
						key = _ReadKey();
						//-------------------------------------------------------
						if(key != KEY_ENTER){
							break;
						}
						_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
						_Printfxy(0, 4*16, "  当前抄表册         ", Color_White);
						_Printfxy(0, 5*16, "  抄表结果清空中... ", Color_White);
						_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
						//------------------------------------------------------------
						_Select(1);
						_Use(MeterDocDB);	// 打开数据库
						_Go(0);
						for(i = 0; i < recCnt; i++){
							_ReadField(Idx_SectNum, strTmp);	// 抄表册编号 过滤
							strTmp[Size_SectNum - 1] = '\0';
							if(strcmp(Meters.qrySectNum, strTmp) != 0){
								_Skip(1);	// 下一个数据库记录
								continue;
							}

							_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
							strTmp[Size_MeterReadStatus - 1] = '\0';
							if(strTmp[1] == '1'){	// 跳过 未抄
								_Skip(1);	// 下一个数据库记录
								continue;
							}

							_Replace(Idx_MeterReadStatus, "01");	
							_Replace(Idx_MeterReadDate, "");
							_Replace(Idx_MeterReadType, "01");
							_ReadField(Idx_LastReadVal, strTmp); // 上次读数 --> 本次读数
							strTmp[Size_LastReadVal - 1] = '\0';
							_Replace(Idx_CurrReadVal, strTmp);
							_Replace(Idx_CurrElecReadVal, "0");
							_Replace(Idx_CurrGasVol, "0");
							_Replace(Idx_CurrRemainGasVol, "0");
							_Replace(Idx_MrExcepType, "01");
							_Skip(1);
						}
						_Use("");		// 关闭数据库
						//------------------------------------------------------------
						_Printfxy(0, 5*16, "  抄表结果清空完成! ", Color_White);
						_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
						_Sleep(2500);
						break;

					case 6:		// 重置抄表时间
						//-------------------------------------------------------
						_GUIRectangleFill(0, 3*16 - 8, 160, 7*16 + 8, Color_White);
						_GUIRectangle(0, 3*16 - 8, 160, 7*16 + 8, Color_Black);
						memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
						isUiFinish = false;
						while(true){
							if(false == isUiFinish){
								(*pUiCnt) = 0;
								uiRowIdx = 3;
								_GetDateTime(time, '-',  ':');
								DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
								
								LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "系统时间:");
								TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
								TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
								TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
								TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
								TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
								TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
								LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "确定将抄表时间重置");
								LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "为当前系统时间吗?");
								
								key = ShowUI(UiList, &currUi);
								if (key == KEY_CANCEL){
									break;
								}
								isUiFinish = true;
							}

							// 时间有效值校验
							if( (i = TimeStrsToTimeBytes(&TmpBuf[0], StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
								currUi = 1 + (i -1);
								isUiFinish = false;
								continue;
							}

							sprintf(time, "%s-%s-%s %s:%s:%s",
								StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
							_SetDateTime(time);
							break;
						}
						if (key == KEY_CANCEL){
							break;
						}

						_Printfxy(8, 5*16, "当前抄表册           ", Color_White);
						_Printfxy(8, 6*16, "抄表时间重置中... ", Color_White);
						//------------------------------------------------------------
						_Select(1);
						_Use(MeterDocDB);	// 打开数据库
						_Go(0);
						for(i = 0; i < recCnt; i++){
							_ReadField(Idx_SectNum, strTmp);	// 抄表册编号 过滤
							strTmp[Size_SectNum - 1] = '\0';
							if(strcmp(Meters.qrySectNum, strTmp) != 0){
								_Skip(1);	// 下一个数据库记录
								continue;
							}

							_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
							strTmp[Size_MeterReadStatus - 1] = '\0';
							if(strTmp[1] == '1'){
								_Skip(1);	// 下一个数据库记录
								continue;
							}

							_GetDate(time, '\0');
							_Replace(Idx_MeterReadDate, time);
							_Skip(1);
						}
						_Use("");		// 关闭数据库
						//------------------------------------------------------------
						_Printfxy(8, 6*16, "抄表时间重置完成！ ", Color_White);
						_Sleep(2500);
						break;
					
					case 7:		// 设置路由
						//ShowSettingRoutes();
						break;
					default:
						break;
					}

				}// while 3 批量操作菜单
			}// while 2 抄表册列表
			break;

		
		case 2:		// 户表查询
			// 户表查询-界面
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 7, 2, NULL,
				"<<户表查询", 
				2,
				"1. 按表号查询",
				"2. 按户号查询");
			while(2){

				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBox(&menuList_2);
				//------------------------------------------------------------
				if(key == KEY_CANCEL){	// 返回
					break;
				}

				// 户表查询-输入界面
				//------------------------------------------------------------
				_ClearScreen();
				sprintf(&TmpBuf[0], "<<%s", &(menuList_2.str[menuList_2.strIdx][3]));
				_Printfxy(0, 0*16, &TmpBuf[0], Color_White);
				_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
				Meters.qryKeyWord = StrBuf[0];
				switch (menuList_2.strIdx + 1){
				case 1: ptr = "输入表号: "; Meters.selectField = Idx_MeterNum; break;
				case 2: ptr = "输入户号: "; Meters.selectField = Idx_UserNum; break;
				default: break;
				}
				_Printfxy(0, 2*16, ptr, Color_White);
				StrBuf[0][0] = 0x00;
				inputSt.left = 0;
				inputSt.top = 3*16;
				inputSt.width = 10*16;
				inputSt.hight = 16;
				inputSt.caption = "";
				inputSt.context = StrBuf[0];
				inputSt.datelen = 20;
				inputSt.IsClear = true;
				inputSt.keyUpDown = false;
				inputSt.type = 1;
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);	
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				//------------------------------------------------------------
				if(KEY_CANCEL ==  _GetStr(&inputSt)){
					continue;
				}
				StringTrimStart(StrBuf[0],  ' ');
				StringTrimEnd(StrBuf[0],  ' ');

				// 关键字 查询
				_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
				QueryMeterListByKeyword(&Meters, &DbQuery);	
				if(DbQuery.resultCnt > 0){
					ShowMeterList(&Meters);
				}
				else{
					_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
					_Printfxy(0, 4*16, " 查询失败，未找到 ", Color_White);
					_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
					_Sleep(2500);
				}
				
			} // while 2 户表查询
			break;

		case 3:		// 抄表统计
			// 抄表统计-界面
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 2, 2, NULL,
				"<<抄表统计", 
				2, 
				"1. 抄表册选择>",
				"2. 统计");
			//------------------------------------------------------------
			sprintf(Meters.sectNum, "全部");
			//sprintf(Meters.sectName, "全部");
			qryTypeXq = 0;		// 0 - 全部， 1 - 选择某个
			qryIndexXq = 0;		// 当前选择的抄表册 在列表中索引
			while(2){

				_ClearScreen();
				// 抄表统计-界面
				//---------------------------------------------------------------------
				dispBuf = &DispBuf;
				dispIdx = 0;
				dispIdx += sprintf(&dispBuf[dispIdx], "抄表册: %s\n", Meters.sectNum);
				//dispIdx += sprintf(&dispBuf[dispIdx], "抄表册: %s\n", Meters.sectName);
				PrintfXyMultiLine(0, 5*16, dispBuf, 7);
				//----------------------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "返回            确定", Color_White);
				key = ShowListBox(&menuList_2);
				//----------------------------------------------------------------------
				if(key == KEY_CANCEL){	// 返回
					break;
				}
				
				if(menuList_2.strIdx == 0){		// 抄表册选择
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 2*16, 14, 2, 2, NULL, 
					"抄表册选择", 2, "全部", "选择某个");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// 未选择，返回
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeXq = 0;
						sprintf(Meters.sectNum, "全部");
						//sprintf(Meters.sectName, "全部");
					}
					else{
						// 抄表册列表-界面
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  查询中  >    ", Color_White);
						QuerySectList(&Sects, &DbQuery);
						ListBoxCreateEx(&XqList, 0, 0, 20, 7, Sects.cnt, NULL,
							"<<抄表册选择", Sects.nums, Size_ListStr, Sects.cnt);
						_Printfxy(0, 9*16, "返回            确定", Color_White);
						key = ShowListBoxEx(&XqList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// 未选择列表项
							continue;
						}

						qryTypeXq = 1;
						qryIndexXq = XqList.strIdx;

						sprintf(Meters.sectNum, Sects.nums[qryIndexXq]);		// 抄表册编号
						//sprintf(Meters.sectName, Sects.Names[qryIndexXq]);		// 或 抄表册名称
					}

				}
				else {	// if(menuList_2.strIdx == 2){	// 统计
					Meters.qrySectNum = (qryTypeXq == 0 ? NULL : Sects.nums[qryIndexXq]);
					Meters.selectField = Idx_Invalid;
					_Printfxy(0, 9*16, "    <  统计中  >    ", Color_White);
					QueryMeterList(&Meters, &DbQuery);
					key = ShowMeterReadCountInfo(&Meters);
				}
			}
			break;

		case 4:		// 清空抄表结果
			//-------------------------------------------------------
			_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  确定要清空       ", Color_White);
			_Printfxy(0, 5*16, "  所有抄表册结果吗? ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
			key = _ReadKey();
			//-------------------------------------------------------
			if(key != KEY_ENTER){
				break;
			}
			_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  清空抄表结果中... ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			//-------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// 打开数据库
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[1] == '1'){	// 跳过 未抄
					_Skip(1);	// 下一个数据库记录
					continue;
				}
				_Replace(Idx_MeterReadStatus, "01");
				_Replace(Idx_MeterReadDate, "");
				_Replace(Idx_MeterReadType, "01");
				_ReadField(Idx_LastReadVal, strTmp); // 上次读数 --> 本次读数
				strTmp[Size_LastReadVal - 1] = '\0';
				_Replace(Idx_CurrReadVal, strTmp);
				_Replace(Idx_CurrElecReadVal, "0");
				_Replace(Idx_CurrGasVol, "0");
				_Replace(Idx_CurrRemainGasVol, "0");
				_Replace(Idx_MrExcepType, "01");
				_Skip(1);		// 下一个数据库记录
			}
			_Use("");			// 关闭数据库
			//-------------------------------------------------------
			_Printfxy(0, 4*16, "  清空抄表结果完成！", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			_Sleep(2500);
			break;

		case 5:		// 重置抄表时间
			//-------------------------------------------------------
			_GUIRectangleFill(0, 3*16 - 8, 160, 7*16 + 8, Color_White);
			_GUIRectangle(0, 3*16 - 8, 160, 7*16 + 8, Color_Black);
			while(true){
				if(false == isUiFinish){
					(*pUiCnt) = 0;
					uiRowIdx = 3;

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
					
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "系统时间:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "确定将抄表时间重置");
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "为当前系统时间吗?");
					
					key = ShowUI(UiList, &currUi);
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;
				
				}
				// 时间有效值校验
				if( (i = TimeStrsToTimeBytes(&TmpBuf[0], StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
					currUi = 1 + (i -1);
					isUiFinish = false;
					continue;
				}

				sprintf(time, "%s-%s-%s %s:%s:%s",
					StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				_SetDateTime(time);
				break;
			}
			//-------------------------------------------------------
			if(key == KEY_CANCEL){
				break;
			}
			_Printfxy(8, 5*16, "所有档案          ", Color_White);
			_Printfxy(8, 6*16, "抄表时间重置中... ", Color_White);
			//------------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// 打开数据库
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// 抄表状态 过滤
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[1] == '1'){
					_Skip(1);	// 下一个数据库记录
					continue;
				}
				_GetDate(time, '\0');
				_Replace(Idx_MeterReadDate, time);
				_Skip(1);
			}
			_Use("");		// 关闭数据库
			//------------------------------------------------------------
			_Printfxy(8, 6*16, "抄表时间重置完成！", Color_White);
			_Sleep(2500);
			break;

		default: 
			break;
		}

	} // while 1 批量抄表

	FixDbfRecCnt();	// 修复记录总数
	MeterInfo.dbIdx = Invalid_dbIdx;  // 清空当前表数据库索引，防止抄表结果写入
}

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

