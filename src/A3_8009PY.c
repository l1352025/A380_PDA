/*
*
*	ɣ��8009�����ֳֻ� - RF�� (������ư�)
*
*/
#include "A3_8009PY.h"

#include "HJLIB.H"
#include "string.h"
#include "stdio.h"
#include "common.h"
#include "common.c"
#include "MeterDocDBF_8009_PY.h"
#include "MeterDocDBF_8009_PY.c"
#include "ProtoHandle_8009_PY.h"


//-----------------------------------	������	---------------------------

// ��������
void MainFuncBatchMeterReading(void)
{
	uint8 key;
	ListBox menuList, menuList_2, menuList_3;
	ListBoxEx XqList;				// ������б�
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

	recCnt = FixDbfRecCnt();	// �޸���¼����
	MeterInfo.dbIdx = Invalid_dbIdx;  // ��յ�ǰ�����ݿ���������ֹ������д��
	
	if(recCnt == 0){
		_GUIRectangleFill(0, 3*16 - 8, 160, 6*16 + 8, Color_White);
		PrintfXyMultiLine_VaList(0, 3*16, "  ��ǰ����Ϊ�գ�\n �������س�������\n �ٽ�����������");
		_GUIRectangle(0, 3*16 - 8, 160, 6*16 + 8, Color_Black);
		_Sleep(2500);
		return;
	}

	// �˵�
	//------------------------------------------------------------
	ListBoxCreate(&menuList, 0, 0, 20, 7, 5, NULL,
		"<<��������",
		5,
		"1. ������᳭��",
		"2. �����ѯ",
		"3. ����ͳ��",
		"4. ��ճ�����",
		"5. ���ó���ʱ��"
	);
	while(1){

		_Printfxy(0, 9*16, "����            ȷ��", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// ����
			break;
		}
		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		switch(menuList.strIdx + 1){
		case 1:		// ������᳭��

			// ������б�-����
			//------------------------------------------------------------
			_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
			QuerySectList(&Sects, &DbQuery);
			ListBoxCreateEx(&XqList, 0, 0, 20, 7, Sects.cnt, NULL,
				"<<�����ѡ��", Sects.nums, Size_ListStr, Sects.cnt);
			while(2){
				
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBoxEx(&XqList);
				//------------------------------------------------------------
				if (key == KEY_CANCEL){	// ����
					break;
				}

				// ���·��
				StrRelayAddr[0][0] = 0x00;
				StrRelayAddr[1][0] = 0x00;
				StrRelayAddr[2][0] = 0x00;

				// ����᳭��-����
				//------------------------------------------------------------
				Meters.qrySectNum = Sects.nums[XqList.strIdx];
				strcpy(Meters.sectNum, Meters.qrySectNum);
				ListBoxCreate(&menuList_2, 0, 0, 20, 7, 6, NULL,
					"<<����᳭��", 
					6,
					"1. �Զ�����",
					"2. �ѳ��ɹ��б�",
					"3. δ��ʧ���б�",
					"4. ����ͳ��",
					"5. ��ճ�����",
					"6. ���ó���ʱ��");
					//"7. ����·��");
				while(3){

					_Printfxy(0, 9*16, "����            ȷ��", Color_White);
					key = ShowListBox(&menuList_2);
					//------------------------------------------------------------
					if(key == KEY_CANCEL){	// ����
						break;
					}

					switch (menuList_2.strIdx + 1){
					case 1:		// �Զ�����
						Meters.selectField = Idx_MeterNum;
						Meters.qryMeterReadStatus = 0;
						_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
						QueryMeterList(&Meters, &DbQuery);
						key = ShowAutoMeterReading(&Meters);
						break;

					case 2:		// �ѳ��б�
						Meters.selectField = Idx_Invalid;
						Meters.qryMeterReadStatus = 1;
						key = ShowMeterList(&Meters);
						break;
					case 3:		// δ��/ʧ���б�
						Meters.selectField = Idx_Invalid;
						Meters.qryMeterReadStatus = 0;
						key = ShowMeterList(&Meters);
						break;

					case 4:		// ����ͳ��
						Meters.selectField = Idx_Invalid;
						_Printfxy(0, 9*16, "    <  ͳ����  >    ", Color_White);
						QueryMeterList(&Meters, &DbQuery);
						key = ShowMeterReadCountInfo(&Meters);
						break;

					case 5:		// ��ճ�����
						//-------------------------------------------------------
						_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
						_Printfxy(0, 4*16, " ȷ��Ҫ���        ", Color_White);
						_Printfxy(0, 5*16, " ��ǰ���������?  ", Color_White);
						_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
						key = _ReadKey();
						//-------------------------------------------------------
						if(key != KEY_ENTER){
							break;
						}
						_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
						_Printfxy(0, 4*16, "  ��ǰ�����         ", Color_White);
						_Printfxy(0, 5*16, "  �����������... ", Color_White);
						_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
						//------------------------------------------------------------
						_Select(1);
						_Use(MeterDocDB);	// �����ݿ�
						_Go(0);
						for(i = 0; i < recCnt; i++){
							_ReadField(Idx_SectNum, strTmp);	// ������� ����
							strTmp[Size_SectNum - 1] = '\0';
							if(strcmp(Meters.qrySectNum, strTmp) != 0){
								_Skip(1);	// ��һ�����ݿ��¼
								continue;
							}

							_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
							strTmp[Size_MeterReadStatus - 1] = '\0';
							if(strTmp[1] == '1'){	// ���� δ��
								_Skip(1);	// ��һ�����ݿ��¼
								continue;
							}

							_Replace(Idx_MeterReadStatus, "01");	
							_Replace(Idx_MeterReadDate, "");
							_Replace(Idx_MeterReadType, "01");
							_ReadField(Idx_LastReadVal, strTmp); // �ϴζ��� --> ���ζ���
							strTmp[Size_LastReadVal - 1] = '\0';
							_Replace(Idx_CurrReadVal, strTmp);
							_Replace(Idx_CurrElecReadVal, "0");
							_Replace(Idx_CurrGasVol, "0");
							_Replace(Idx_CurrRemainGasVol, "0");
							_Replace(Idx_MrExcepType, "01");
							_Skip(1);
						}
						_Use("");		// �ر����ݿ�
						//------------------------------------------------------------
						_Printfxy(0, 5*16, "  ������������! ", Color_White);
						_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
						_Sleep(2500);
						break;

					case 6:		// ���ó���ʱ��
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
								
								LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ϵͳʱ��:");
								TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
								TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
								TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
								TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
								TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
								TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
								LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ȷ��������ʱ������");
								LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "Ϊ��ǰϵͳʱ����?");
								
								key = ShowUI(UiList, &currUi);
								if (key == KEY_CANCEL){
									break;
								}
								isUiFinish = true;
							}

							// ʱ����ЧֵУ��
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

						_Printfxy(8, 5*16, "��ǰ�����           ", Color_White);
						_Printfxy(8, 6*16, "����ʱ��������... ", Color_White);
						//------------------------------------------------------------
						_Select(1);
						_Use(MeterDocDB);	// �����ݿ�
						_Go(0);
						for(i = 0; i < recCnt; i++){
							_ReadField(Idx_SectNum, strTmp);	// ������� ����
							strTmp[Size_SectNum - 1] = '\0';
							if(strcmp(Meters.qrySectNum, strTmp) != 0){
								_Skip(1);	// ��һ�����ݿ��¼
								continue;
							}

							_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
							strTmp[Size_MeterReadStatus - 1] = '\0';
							if(strTmp[1] == '1'){
								_Skip(1);	// ��һ�����ݿ��¼
								continue;
							}

							_GetDate(time, '\0');
							_Replace(Idx_MeterReadDate, time);
							_Skip(1);
						}
						_Use("");		// �ر����ݿ�
						//------------------------------------------------------------
						_Printfxy(8, 6*16, "����ʱ��������ɣ� ", Color_White);
						_Sleep(2500);
						break;
					
					case 7:		// ����·��
						//ShowSettingRoutes();
						break;
					default:
						break;
					}

				}// while 3 ���������˵�
			}// while 2 ������б�
			break;

		
		case 2:		// �����ѯ
			// �����ѯ-����
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 7, 2, NULL,
				"<<�����ѯ", 
				2,
				"1. ����Ų�ѯ",
				"2. �����Ų�ѯ");
			while(2){

				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBox(&menuList_2);
				//------------------------------------------------------------
				if(key == KEY_CANCEL){	// ����
					break;
				}

				// �����ѯ-�������
				//------------------------------------------------------------
				_ClearScreen();
				sprintf(&TmpBuf[0], "<<%s", &(menuList_2.str[menuList_2.strIdx][3]));
				_Printfxy(0, 0*16, &TmpBuf[0], Color_White);
				_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
				Meters.qryKeyWord = StrBuf[0];
				switch (menuList_2.strIdx + 1){
				case 1: ptr = "������: "; Meters.selectField = Idx_MeterNum; break;
				case 2: ptr = "���뻧��: "; Meters.selectField = Idx_UserNum; break;
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
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				//------------------------------------------------------------
				if(KEY_CANCEL ==  _GetStr(&inputSt)){
					continue;
				}
				StringTrimStart(StrBuf[0],  ' ');
				StringTrimEnd(StrBuf[0],  ' ');

				// �ؼ��� ��ѯ
				_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
				QueryMeterListByKeyword(&Meters, &DbQuery);	
				if(DbQuery.resultCnt > 0){
					ShowMeterList(&Meters);
				}
				else{
					_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
					_Printfxy(0, 4*16, " ��ѯʧ�ܣ�δ�ҵ� ", Color_White);
					_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
					_Sleep(2500);
				}
				
			} // while 2 �����ѯ
			break;

		case 3:		// ����ͳ��
			// ����ͳ��-����
			//------------------------------------------------------------
			ListBoxCreate(&menuList_2, 0, 0, 20, 2, 2, NULL,
				"<<����ͳ��", 
				2, 
				"1. �����ѡ��>",
				"2. ͳ��");
			//------------------------------------------------------------
			sprintf(Meters.sectNum, "ȫ��");
			//sprintf(Meters.sectName, "ȫ��");
			qryTypeXq = 0;		// 0 - ȫ���� 1 - ѡ��ĳ��
			qryIndexXq = 0;		// ��ǰѡ��ĳ���� ���б�������
			while(2){

				_ClearScreen();
				// ����ͳ��-����
				//---------------------------------------------------------------------
				dispBuf = &DispBuf;
				dispIdx = 0;
				dispIdx += sprintf(&dispBuf[dispIdx], "�����: %s\n", Meters.sectNum);
				//dispIdx += sprintf(&dispBuf[dispIdx], "�����: %s\n", Meters.sectName);
				PrintfXyMultiLine(0, 5*16, dispBuf, 7);
				//----------------------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "����            ȷ��", Color_White);
				key = ShowListBox(&menuList_2);
				//----------------------------------------------------------------------
				if(key == KEY_CANCEL){	// ����
					break;
				}
				
				if(menuList_2.strIdx == 0){		// �����ѡ��
					//----------------------------------------------
					ListBoxCreate(&menuList_3, 2*16, 2*16, 14, 2, 2, NULL, 
					"�����ѡ��", 2, "ȫ��", "ѡ��ĳ��");
					key = ShowListBox(&menuList_3);
					//----------------------------------------------
					if(key == KEY_CANCEL){	// δѡ�񣬷���
						continue;
					}

					if(menuList_3.strIdx == 0){
						qryTypeXq = 0;
						sprintf(Meters.sectNum, "ȫ��");
						//sprintf(Meters.sectName, "ȫ��");
					}
					else{
						// ������б�-����
						//------------------------------------------------------------
						_Printfxy(0, 9*16, "    <  ��ѯ��  >    ", Color_White);
						QuerySectList(&Sects, &DbQuery);
						ListBoxCreateEx(&XqList, 0, 0, 20, 7, Sects.cnt, NULL,
							"<<�����ѡ��", Sects.nums, Size_ListStr, Sects.cnt);
						_Printfxy(0, 9*16, "����            ȷ��", Color_White);
						key = ShowListBoxEx(&XqList);
						//------------------------------------------------------------
						if (key == KEY_CANCEL){		// δѡ���б���
							continue;
						}

						qryTypeXq = 1;
						qryIndexXq = XqList.strIdx;

						sprintf(Meters.sectNum, Sects.nums[qryIndexXq]);		// �������
						//sprintf(Meters.sectName, Sects.Names[qryIndexXq]);		// �� ���������
					}

				}
				else {	// if(menuList_2.strIdx == 2){	// ͳ��
					Meters.qrySectNum = (qryTypeXq == 0 ? NULL : Sects.nums[qryIndexXq]);
					Meters.selectField = Idx_Invalid;
					_Printfxy(0, 9*16, "    <  ͳ����  >    ", Color_White);
					QueryMeterList(&Meters, &DbQuery);
					key = ShowMeterReadCountInfo(&Meters);
				}
			}
			break;

		case 4:		// ��ճ�����
			//-------------------------------------------------------
			_GUIRectangleFill(0, 4*16 - 8, 160, 6*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  ȷ��Ҫ���       ", Color_White);
			_Printfxy(0, 5*16, "  ���г��������? ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 6*16 + 8, Color_Black);
			key = _ReadKey();
			//-------------------------------------------------------
			if(key != KEY_ENTER){
				break;
			}
			_GUIRectangleFill(0, 4*16 - 8, 160, 5*16 + 8, Color_White);
			_Printfxy(0, 4*16, "  ��ճ�������... ", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			//-------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// �����ݿ�
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[1] == '1'){	// ���� δ��
					_Skip(1);	// ��һ�����ݿ��¼
					continue;
				}
				_Replace(Idx_MeterReadStatus, "01");
				_Replace(Idx_MeterReadDate, "");
				_Replace(Idx_MeterReadType, "01");
				_ReadField(Idx_LastReadVal, strTmp); // �ϴζ��� --> ���ζ���
				strTmp[Size_LastReadVal - 1] = '\0';
				_Replace(Idx_CurrReadVal, strTmp);
				_Replace(Idx_CurrElecReadVal, "0");
				_Replace(Idx_CurrGasVol, "0");
				_Replace(Idx_CurrRemainGasVol, "0");
				_Replace(Idx_MrExcepType, "01");
				_Skip(1);		// ��һ�����ݿ��¼
			}
			_Use("");			// �ر����ݿ�
			//-------------------------------------------------------
			_Printfxy(0, 4*16, "  ��ճ�������ɣ�", Color_White);
			_GUIRectangle(0, 4*16 - 8, 160, 5*16 + 8, Color_Black);
			_Sleep(2500);
			break;

		case 5:		// ���ó���ʱ��
			//-------------------------------------------------------
			_GUIRectangleFill(0, 3*16 - 8, 160, 7*16 + 8, Color_White);
			_GUIRectangle(0, 3*16 - 8, 160, 7*16 + 8, Color_Black);
			while(true){
				if(false == isUiFinish){
					(*pUiCnt) = 0;
					uiRowIdx = 3;

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
					
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ϵͳʱ��:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "ȷ��������ʱ������");
					LableCreate(&pUi[(*pUiCnt)++], 8, (uiRowIdx++)*16, "Ϊ��ǰϵͳʱ����?");
					
					key = ShowUI(UiList, &currUi);
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;
				
				}
				// ʱ����ЧֵУ��
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
			_Printfxy(8, 5*16, "���е���          ", Color_White);
			_Printfxy(8, 6*16, "����ʱ��������... ", Color_White);
			//------------------------------------------------------------
			_Select(1);
			_Use(MeterDocDB);	// �����ݿ�
			_Go(0);
			for(i = 0; i < recCnt; i++){
				_ReadField(Idx_MeterReadStatus, strTmp);	// ����״̬ ����
				strTmp[Size_MeterReadStatus - 1] = '\0';
				if(strTmp[1] == '1'){
					_Skip(1);	// ��һ�����ݿ��¼
					continue;
				}
				_GetDate(time, '\0');
				_Replace(Idx_MeterReadDate, time);
				_Skip(1);
			}
			_Use("");		// �ر����ݿ�
			//------------------------------------------------------------
			_Printfxy(8, 6*16, "����ʱ��������ɣ�", Color_White);
			_Sleep(2500);
			break;

		default: 
			break;
		}

	} // while 1 ��������

	FixDbfRecCnt();	// �޸���¼����
	MeterInfo.dbIdx = Invalid_dbIdx;  // ��յ�ǰ�����ݿ���������ֹ������д��
}

// ���ù���
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 4, NULL,
		"<<���̵���",
		4,
		"1. ��������",
		"2. ����",
		"3. ��ط�",
		"4. ���״̬"
	);

	while(1){

		_Printfxy(0, 9*16, "����            ȷ��", Color_White);
		key = ShowListBox(&menuList);
		//------------------------------------------------------------
		if (key == KEY_CANCEL){	// ����
			break;
		}
		menuItemNo = menuList.strIdx + 1;

		memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
		isUiFinish = false;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			sprintf(CurrCmdName, menuList.str[menuItemNo - 1]);
			sprintf(TmpBuf, "<<%s",&CurrCmdName[3]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
		 	//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "����   <����>   ȷ��", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen <= 6)
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�� ��:", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);
				#else
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�� ��:");
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrDstAddr, AddrLen*2, (AddrLen*2*8 + 8), true);	
				#endif
			}

			// �����������
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // ������
			Args.items[1] = &Args.buf[1];	// ������
			CurrCmd = (0x10 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// ��������
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x01;		// ������	01
				ackLen = 9;				// Ӧ�𳤶� 9/13	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_OpenValve:			// ���� 
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x05;		// ������	05
				ackLen = 0;					// Ӧ�𳤶� 0	
				// ������
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_CloseValve:			// �ط�
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x06;		// ������	06
				ackLen = 0;					// Ӧ�𳤶� 0	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ClearException:		// ���쳣����
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 0;					// Ӧ�𳤶� 0	
				// ������
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}

			// ���� ���м̵�ַ����� �� ��ʾUI
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

			// ��ַ���
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen <= 6)
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
			#endif

			// ���͡����ա������ʾ
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				isUiFinish = false;
				continue;
			}
		}
		
	}
}


// ���̵���		
void MainFuncEngineerDebuging(void)
{
	WaterCmdFunc_CommonCmd();	// ���̵��� --> ��ԭ���� ���ù���
}

// --------------------------------   ������   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	
	MeterNoLoad(StrDstAddr);
	SysCfgLoad();
	
	#if LOG_ON
		LogPrint("������� �ѽ��� \n");
	#endif

	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=3;
	MainMenu.title =  VerInfo_Name;
	MainMenu.str[0] = " �������� ";
	MainMenu.str[1] = " ���̵��� ";
	MainMenu.str[2] = " �汾��Ϣ ";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.Function[0] = MainFuncBatchMeterReading;
	MainMenu.Function[1] = MainFuncEngineerDebuging; // ���̵��� --> ��ԭ���� ���ù���
	MainMenu.Function[2] = VersionInfoFunc;	
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();

	#if LOG_ON
		LogPrint("������� ���˳���\n\n\n");
	#endif

}

