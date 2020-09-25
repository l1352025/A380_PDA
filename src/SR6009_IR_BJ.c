/*
*
*	ɣ��6009ˮ���ֳֻ� - �����
*
*/
#include "SR6009_IR.h"

#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "common.h"
#include "common.c"
#include "WaterMeter.h"
#include "Upgrade.h"
#include "Upgrade.c"
#include "List.h"
#include "List.c"


// --------------------------------  ˮ��ģ��ͨ��  -----------------------------------------

// 1	��������
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout, u16Tmp;
	uint32 u32Tmp;

	_ClearScreen();

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
		"<<��������",
		6,
		"1. ���ۼ�����",
		"2. ����������",
		"3. ������������ϵ��",
		"4. ����",
		"5. �ط�",
		"6. ���쳣"
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
			_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;

				#if (AddrLen == 6)
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

			switch(menuItemNo){
			case 1: 
				CurrCmd = WaterCmd_ReadRealTimeData;		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x01;		// ������	01
				ackLen = 21;				// Ӧ�𳤶� 21	
				// ������
				Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
				Args.lastItemLen = i - 1;
				break;

			case 2: 
				CurrCmd = WaterCmd_ReadFrozenData;		// "��ȡ������ת����"
				/*---------------------------------------------*/
				if(false == isUiFinish){
					sprintf(StrBuf[0], "0 (0-9��Ч)");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�� ��:", StrBuf[0], 1, 2*8, true);
					break;
				}
				
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x02;		// ������	02
				ackLen = 114;				// Ӧ�𳤶� 88/114	
				// ������
				Args.buf[i++] = 0x01;				// ���ݸ�ʽ 01/02
				Args.buf[i++] = _GetYear()/100;		// ʱ�� - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = _GetYear()%100;		
				Args.buf[i++] = _GetMonth();		
				Args.buf[i++] = _GetDay();			
				Args.buf[i++] = _GetHour();			
				Args.buf[i++] = _GetMin();			
				Args.buf[i++] = _GetSec();			
				Args.buf[i++] = StrBuf[0][0] - '0';	// �����������	
				Args.lastItemLen = i - 1;
				break;

			case 3: 
				CurrCmd = WaterCmd_SetBaseValPulseRatio;	// ����������ϵ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
						StrBuf[1][0] = 0x01;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 2 * 20);
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�û�����:", StrBuf[0], 10, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ϵ��:", &StrBuf[1][0], 4, 
						"1", "10", "100", "1000");
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " ������");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}

				memcpy(&BackupBuf[ArgIdx_MtrValPalse], &StrBuf[0][0], 2 * 20);
				BackupBuf[ArgIdx_MtrValPalse - 1] = Param_Unique;

				Args.buf[i++] = 0x06;		// ������	06
				ackLen = 7;					// Ӧ�𳤶� 7	
				// ������
				u32Tmp = (uint32) _atof(StrBuf[0]);
				u16Tmp = (uint16)((float)((_atof(StrBuf[0]) - u32Tmp)*1000.0));
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// �û�����	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)StrBuf[1][0];		// ����ϵ��	
				Args.lastItemLen = i - 1;
				break;

			case 4: 
				CurrCmd = WaterCmd_OpenValve;			// " ���� "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				// ������
				Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x01;		// ���ط���ʶ	0 - �ط��� 1 - ����
				Args.lastItemLen = i - 1;
				break;

            case 5: 
				CurrCmd = WaterCmd_CloseValve;			// " �ط� ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				// ������
				Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x00;		// ���ط���ʶ	0 - �ط��� 1 - ����
				Args.lastItemLen = i - 1;
				break;

			case 6: 
				CurrCmd = WaterCmd_ClearException;		// " ���쳣���� ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x05;		// ������	05
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x00;		// ����ѡ�� 00	
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}

			// ���� ���м̵�ַ����� �� ��ʾUI
			if(false == isUiFinish){
			
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9')
				{
					sprintf(StrDstAddr, " ������");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// ��ַ���
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen == 6)
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
			#endif

			// ���͡����ա������ʾ
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
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

// 2	��������
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

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
		"<<��������",
		6,
		"1. ��debug��Ϣ",
		"2. ��debug��Ϣ",
		"3. �������",
		"4. �����¶�",
		"5. �����ѹ",
		"6. ���÷��ز���"
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
			_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen == 6)
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

			switch(menuItemNo){
			case 1: 
				CurrCmd = WaterCmd_ReadDebugInfo;		// ��debug��Ϣ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 58;				// Ӧ�𳤶� 58	
				// ������
				Args.buf[i++] = 0x01;		// ����ѡ�� 01	
				Args.lastItemLen = i - 1;
				break;

			case 2: 
				CurrCmd = WaterCmd_ClearDebugInfo;		// ��debug��Ϣ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x02;		// ����ѡ�� 05	
				Args.lastItemLen = i - 1;
				break;

			case 3: 
				CurrCmd = WaterCmd_RebootDevice;			// "�������"
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x04;		// ����ѡ�� 04	
				Args.lastItemLen = i - 1;
				break;

			case 4: 
				CurrCmd = WaterCmd_ReadTemperature;			// " �����¶� "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x05;		// ����ѡ�� 05	
				Args.lastItemLen = i - 1;
				break;

			case 5: 
				CurrCmd = WaterCmd_ReadVoltage;				// " �����ѹ "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x07;		// ����ѡ�� 07	
				Args.lastItemLen = i - 1;
				break;
			
            case 6: 
				CurrCmd = WaterCmd_SetOverCurrentTimeout;		// " ���÷��ز��� ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_OverCurr -1] == Param_Unique){		
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_OverCurr], 2 * 20);
					}
                    _Printfxy(7*16, (uiRowIdx)*16, "mA", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��������:", StrBuf[0], 3, 5*8, true);
					_Printfxy(7*16, (uiRowIdx)*16, "ms", Color_White);
                    TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʱʱ��:", StrBuf[1], 5, 5*8, true);
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

				BackupBuf[ArgIdx_OverCurr -1] = Param_Unique;		
				memcpy(&BackupBuf[ArgIdx_OverCurr], &StrBuf[0][0], 2 * 20);
				
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 4;					// Ӧ�𳤶� 4	
				// ������
				Args.buf[i++] = 0x09;		// ����ѡ�� 09
				Args.buf[i++] = (uint8)u16Tmp;			// �������� 0~255	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// ��ʱʱ�� 0~65535	
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


			// ���� ���м̵�ַ����� �� ��ʾUI
			if(false == isUiFinish){
			
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					key = KEY_NOHOOK;
					break;
				}

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
					sprintf(StrDstAddr, " ������");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// ��ַ���
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen == 6)
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
			#endif

			// ���͡����ա������ʾ
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
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

// 3	��������
void WaterCmdFunc_Upgrade(void)
{
	UpgradeFunc();
}

// 4	Ԥ������
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

	// �˵�`
	ListBoxCreate(&menuList, 0, 0, 20, 7, 6, NULL,
		"<<Ԥ������",
		6,
		"1. ��Ԥ�ɲο�����",
		"2. ��Ԥ�ɲο�����",
		"3. �������ط���ֵ",
		"4. �豨����ֵ",
		"5. ��ط���ֵ",
		"6. �豨���ط���ֵ"
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
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;
				#if (AddrLen == 6)
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
			CurrCmd = (0x40 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadPrepaidRefVal:		// "��Ԥ�ɲο�����"
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x15;		// ������	15
				ackLen = 12;				// Ӧ�𳤶� 12	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetPrepaidRefVal:		// " ��Ԥ�ɲο����� "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Ԥ������:", StrBuf[0], 10, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
                    TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ο�����:", StrBuf[1], 10, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " ������");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " ������");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}

				Args.buf[i++] = 0x16;		// ������	16
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				u32Tmp = (uint32)_atof(StrBuf[0]);
				u16Tmp = (uint16)((float)((_atof(StrBuf[0]) - u32Tmp)*1000.0));
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// Ԥ������	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				u32Tmp = (uint32)_atof(StrBuf[1]);
				u16Tmp = (uint16)((float)((_atof(StrBuf[1]) - u32Tmp)*1000.0));
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// �ο���ʼ����	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadAlarmLimitOverdraft:	// " ��������ֵ͸֧ "
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x17;		// ������	17
				ackLen = 4;					// Ӧ�𳤶� 4	
				break;
			
			case WaterCmd_SetAlarmLimit:		// " �豨����ֵ ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "������ֵ:", StrBuf[0], 3, 11*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " ������");
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
				Args.buf[i++] = 0x18;		// ������	18
				ackLen = 4;					// Ӧ�𳤶� 4	
				// ������
				Args.buf[i++] = 0x00;		// ����ѡ�0- ������ֵ 1-�ط���ֵ  2-������ֵ+�ط���ֵ	
				Args.buf[i++] = (uint8)u16Tmp;		// ������ֵ
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_SetCloseValveLimit:		// " ��ط���ֵ ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ط���ֵ:", StrBuf[0], 5, 11*8, true);
					break;
				}
				if(StrBuf[0][0] == 0x00){
					sprintf(StrBuf[0], " ������");
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
				Args.buf[i++] = 0x18;		// ������	18
				ackLen = 4;					// Ӧ�𳤶� 4	
				// ������
				Args.buf[i++] = 0x01;		// ����ѡ�0- ������ֵ 1-�ط���ֵ  2-������ֵ+�ط���ֵ	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// �ط���ֵ
				Args.buf[i++] = (uint8)(u32Tmp >> 8);
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetAlarmAndCloseValveLimit:		// " �豨���ط���ֵ ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "������ֵ:", StrBuf[0], 3, 11*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ط���ֵ:", StrBuf[1], 5, 11*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], " ������");
					currUi = uiRowIdx - 2 - 2;
					isUiFinish = false;
					continue;
				}
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					sprintf(StrBuf[1], " ������");
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
				Args.buf[i++] = 0x18;		// ������	18
				ackLen = 4;					// Ӧ�𳤶� 4	
				// ������
				Args.buf[i++] = 0x02;		// ����ѡ�0- ������ֵ 1-�ط���ֵ  2-������ֵ+�ط���ֵ	
				Args.buf[i++] = (uint8)u16Tmp;				// ������ֵ
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// �ط���ֵ
				Args.buf[i++] = (uint8)(u32Tmp >> 8);
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}


			// ���� ���м̵�ַ����� �� ��ʾUI
			if(false == isUiFinish){
			
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
					sprintf(StrDstAddr, " ������");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// ��ַ���
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen == 6)
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
			#endif

			// ���͡����ա������ʾ
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
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


// 5	��������
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
	#if UseBroadAddr
	char strDstAddrBak[20];
	#endif

	_ClearScreen();

	ListBoxCreate(&menuList, 0, 0, 20, 7, 21, NULL,		
		"<<��������",
		21,
		"1. ����IP+�˿�+ģʽ",
		"2. ��ȡIP+�˿�+ģʽ",
		"3. ��ȡ��Ӫ�̱��",
		"4. ��ȡIMEI+ICCID",
		"5. �����ת��������",
		"6. ��ȡ����ʹ��״̬",
		"7. ���ù���ʹ��״̬",
		"8. ���ö�ʱ�ϴ�",
		"9. ���ö����ϴ�",
		"10.���ö�ʱ�����ϴ�",
		"11.�����ʱ��",
		"12.У���ʱ��",
		"13.���շ����ŷ�����",
		"14.��ȡģ�����в���",		
		"15.����ģ�����в���",
		"16.��ȡNB������Ϣ",
		"17.��ȡ����ˮ�����",		
		"18.���ñ���ˮ�����",
		"19.��ȡģ���Ƶ��",		
		"20.����ģ���Ƶ��",
		"21.NB�ϱ�ʵʱ����"
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
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

			if(false == isUiFinish){
				(*pUiCnt) = 0;
				uiRowIdx = 2;

				#if UseBroadAddr
				if(menuItemNo == 4){ // ��ȡIMEI+ICCID ʱ��ʹ��D4D4...��ַ
					memcpy(strDstAddrBak, StrDstAddr, 20);
					sprintf(StrDstAddr, "D4D4D4D4D4D4D4D4");	// ��ʼֵΪ�㲥��ַ
				}
				#endif

				#if (AddrLen == 6)
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

			switch(menuItemNo){
			case 1: 
				CurrCmd = WaterCmd_SetIpPortMode;			// ��IP+�˿�+ģʽ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_IpPortMode -1] == Param_Unique){		
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_IpPortMode], 6 * 20);
					}
					if(StrBuf[0][0] > 1){
						StrBuf[0][0] = 0;
					}
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ģʽ:", &StrBuf[0][0], 2, 
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

				BackupBuf[ArgIdx_IpPortMode -1] = Param_Unique;	
				memcpy(&BackupBuf[ArgIdx_IpPortMode], &StrBuf[0][0], 6 * 20);

				i = 0;
				Args.buf[i++] = 0x0D;		// ������	0D
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x01;		// ����ѡ�0-��ȡ�� 1-����
				Args.buf[i++] = (0xA0 + (uint8)StrBuf[0][0]);	// ģʽ��A0-coap, A1-udp
				Args.buf[i++] = (uint8)(ip[0] & 0xFF);	// IP
				Args.buf[i++] = (uint8)(ip[1] & 0xFF);	
				Args.buf[i++] = (uint8)(ip[2] & 0xFF);		
				Args.buf[i++] = (uint8)(ip[3] & 0xFF);	
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	// Port	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.lastItemLen = i - 1;
				break;

			case 2: 
				CurrCmd = WaterCmd_ReadIpPortMode;			// ��IP+�˿�+ģʽ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0D;		// ������	0D
				ackLen = 9;					// Ӧ�𳤶� 9	
				// ������
				Args.buf[i++] = 0x00;		// ����ѡ�0-��ȡ�� 1-����
				Args.lastItemLen = i - 1;
				break;
				
			case 3: 
				CurrCmd = WaterCmd_ReadNbOperaterNumber;		// " ��NB��Ӫ�̱�� ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0E;		// ������	0E
				ackLen = 7;					// Ӧ�𳤶� 7	
				// ������
				Args.buf[i++] = 0;			// ����ѡ�� 0-��ȡ�� 1-����	
				Args.lastItemLen = i - 1;
				break;

			case 4: 
				CurrCmd = WaterCmd_ReadImeiAndCcid;			// ��IMEI+ICCID
				/*---------------------------------------------*/
				Args.buf[i++] = 0x0F;		// ������	0F
				ackLen = 22;				// Ӧ�𳤶� 12	
				// ������
				Args.buf[i++] = 0;			// ����ѡ��	0-��ȡIMEI+ICCID��1-����IMEI�� 2-����ICCID
				Args.lastItemLen = i - 1;
				break;

			case 5: 
				CurrCmd = WaterCmd_ClearReverseMeasureData;	// �����ת��������
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0A;		// ������	0A
				ackLen = 6;					// Ӧ�𳤶� 6	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case 6: 
				CurrCmd = WaterCmd_ReadFuncEnableState;		// ��ȡ����ʹ��״̬
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0B;		// ������	0B
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x00;
				Args.lastItemLen = i - 1;
				break;

			case 7:
				CurrCmd = WaterCmd_SetFuncEnableState;		// ���ù���ʹ��״̬
				// UI-��1ҳ
				if(false == isUiFinish){
					memset(&StrBuf[0][0], 0x00, 20);
					if(BackupBuf[ArgIdx_FuncEnSts -1] != Param_Unique){		
						StrBuf[0][7] = 1;
						StrBuf[0][8] = 1;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_FuncEnSts], 1 * 20);
					}
					_Printfxy(0, 9*16, "���� <�ȴ�����> ����", Color_White);
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "���Źط�����:", &StrBuf[0][0], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ϱ����ݼ���:", &StrBuf[0][1], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����⹦��:", &StrBuf[0][2], 2, 
						"��", "��");
					
					break;
				}
				
				memcpy(&BackupBuf[ArgIdx_FuncEnSts], &StrBuf[0][0], 1 * 20);

				// UI-��2ҳ
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "���� <�ȴ�����> ����", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Ƿ�ѷ�����  :", &StrBuf[0][3], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����澯    :", &StrBuf[0][4], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ϱ���������:", &StrBuf[0][5], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "͸֧�ط�����:", &StrBuf[0][6], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Ԥ���ѹ���  :", &StrBuf[0][7], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�Զ��ŵ�����:", &StrBuf[0][8], 2, 
						"��", "��");

					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}
				
				memcpy(&BackupBuf[ArgIdx_FuncEnSts], &StrBuf[0][0], 1 * 20);

				// UI-��3ҳ
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "���⹦��    :", &StrBuf[0][9], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ط�����:", &StrBuf[0][10], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "RF���߲���  :", &StrBuf[0][11], 2, 
						"��", "��");
					StrBuf[0][12] = 0X00;  // ����
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "ú��й©���:", &StrBuf[0][13], 2, 
						"��", "��");
					StrBuf[0][14] = 0X00;  // ����
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "���ٿ��ƹ���:", &StrBuf[0][15], 2, 
						"��", "��");
					
					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}
				
				memcpy(&BackupBuf[ArgIdx_FuncEnSts], &StrBuf[0][0], 1 * 20);
				BackupBuf[ArgIdx_FuncEnSts - 1] = Param_Unique;

				i = 0;
				Args.buf[i++] = 0x08;		// ������	08
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				// ����ʹ��״̬ ��0~15
				for(u8Tmp = 0; u8Tmp < 16; u8Tmp++){
					Args.buf[i++] = StrBuf[0][u8Tmp];
				}
				Args.lastItemLen = i - 1;
				break;
			
			case 8: 
				CurrCmd = WaterCmd_SetTimedUpload;		// ���ö�ʱ�ϴ�
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_FixTimeVal -1] == Param_Unique){		
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_FixTimeVal], 2 * 20);
					}
					_Printfxy(7*16, (uiRowIdx)*16, "Сʱ", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʱ���:", StrBuf[0], 2, 3*8, true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					sprintf(StrBuf[0], "   ");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				memcpy(&BackupBuf[ArgIdx_FixTimeVal], &StrBuf[0][0], 2 * 20);
				BackupBuf[ArgIdx_FixTimeVal - 1] = Param_Unique;

				Args.buf[i++] = 0x0C;		// ������	0C
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x00;		// ��𣺶�ʱ���
				Args.buf[i++] = (uint8) _atof(StrBuf[0]);	// ��ʱ���ʱ��
				Args.lastItemLen = i - 1;
				break;

            case 9: 
				CurrCmd = WaterCmd_SetFixedValUpload;		// ���ö����ϴ�
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_FixTimeVal -1] == Param_Unique){		
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_FixTimeVal], 2 * 20);
					}
					_Printfxy(7*16, (uiRowIdx)*16, "������", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�������:", StrBuf[1], 3, 4*8, true);
					break;
				}
				if((StrBuf[1][0] > '9' || StrBuf[1][0] < '0') 
					|| _atof(StrBuf[1]) > 255){
					sprintf(StrBuf[1], "   ");
					currUi = uiRowIdx - 2 - 1;
					isUiFinish = false;
					continue;
				}
				memcpy(&BackupBuf[ArgIdx_FixTimeVal], &StrBuf[0][0], 2 * 20);
				BackupBuf[ArgIdx_FixTimeVal - 1] = Param_Unique;

				Args.buf[i++] = 0x0C;		// ������	0C
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x01;		// ��𣺶������
				Args.buf[i++] = (uint8) _atof(StrBuf[1]);	// �����������
				Args.lastItemLen = i - 1;
				break;

			case 10: 
				CurrCmd = WaterCmd_SetTimedAndFixedValUpload;	// ���ö�ʱ�����ϴ�
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_FixTimeVal -1] == Param_Unique){		
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_FixTimeVal], 2 * 20);
					}
					_Printfxy(7*16, (uiRowIdx)*16, "Сʱ", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʱ���:", StrBuf[0], 2, 3*8, true);
					_Printfxy(7*16, (uiRowIdx)*16, "������", Color_White);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�������:", StrBuf[1], 3, 4*8, true);
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

				memcpy(&BackupBuf[ArgIdx_FixTimeVal], &StrBuf[0][0], 2 * 20);
				BackupBuf[ArgIdx_FixTimeVal - 1] = Param_Unique;

				Args.buf[i++] = 0x0C;		// ������	0C
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x02;		// ��𣺶�ʱ���
				Args.buf[i++] = (uint8) _atof(StrBuf[0]);	// ��ʱ���ʱ��
				Args.buf[i++] = (uint8) _atof(StrBuf[1]);	// �����������
				Args.lastItemLen = i - 1;
				break;

			case 11: 
				CurrCmd = WaterCmd_ReadMeterTime;			// �����ʱ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x13;		// ������	13
				ackLen = 7;					// Ӧ�𳤶� 7	
				// ������
				Args.buf[i++] = 0x00;		// ����ѡ�� 00	
				Args.lastItemLen = i - 1;
				break;

			case 12: 
				CurrCmd = WaterCmd_SetMeterTime;			// У���ʱ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
					
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "ʱ ��:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
					break;
				}
				// ʱ����ЧֵУ��
				if( (i = TimeStrsToTimeBytes(timeBytes, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
					currUi = 3 + (i -1);
					isUiFinish = false;
					continue;
				}

				sprintf(time, "%s-%s-%s %s:%s:%s",
					StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				_SetDateTime(time);

				i = 0;
				Args.buf[i++] = 0x14;		// ������	14
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = DecToBcd(timeBytes[0]);		// ʱ�� - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = DecToBcd(timeBytes[1]);		
				Args.buf[i++] = DecToBcd(timeBytes[2]);		
				Args.buf[i++] = DecToBcd(timeBytes[3]);			
				Args.buf[i++] = DecToBcd(timeBytes[4]);			
				Args.buf[i++] = DecToBcd(timeBytes[5]);			
				Args.buf[i++] = DecToBcd(timeBytes[6]);	
				Args.lastItemLen = i - 1;
				break;

            case 13: 
				CurrCmd = WaterCmd_ReadRxTxMgnDistbCnt;		// ����/��/���Ŵ���
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// ������	09
				ackLen = 7;					// Ӧ�𳤶� 7	
				// ������
				Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
				Args.lastItemLen = i - 1;
				break;

			case 14: 
				CurrCmd = WaterCmd_ReadModuleRunningParams;		// ��ȡģ�����в���
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x3A;		// ������	3A
				ackLen = 124;				// Ӧ�𳤶� 124	
				Args.lastItemLen = i - 1; 
				break;

			case 15: 
				CurrCmd = WaterCmd_SetModuleRunningParams;		// ����ģ�����в���
				/*---------------------------------------------*/
				// UI-��1ҳ
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_RunParas -1] != Param_Unique){		
						StrBuf[0][0] = 0;
						StrBuf[0][1] = 0;
						StrBuf[0][2] = 1;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_RunParas], 1 * 20);
					}
					
					_Printfxy(0, 9*16, "���� <�ȴ�����> ����", Color_White);
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����:", &StrBuf[0][0], 9, 
						"RF��ˮ��", "GPRS��ˮ��", "NB��ˮ��", 
						"RF��ˮ��", "NB��ˮ��", 
						"RFȼ����", "GPRSȼ����", "NBȼ����", 
						"���");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "���:", &StrBuf[0][1], 3, 
						"3.6v", "6v", "4.5v");
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����������:");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "", &StrBuf[0][2], 13, 
						"���ɻɹ�/����", "˫�ɻɹ�/����", "���ɻɹ�/����", "����4λ���ֱ��", 
						"��Ĥֱ��", "����1λ���ֱ��", "188Э����ֱ��", "188Э���޴�ֱ��",
						"����������", "�����޴�", "ɽ���޴�", "�����޴�",
						"�����޴�");
					break;
				}

				memcpy(&BackupBuf[ArgIdx_RunParas], &StrBuf[0][0], 1 * 20);

				// UI-��2ҳ
				currUi = 0;
				if(BackupBuf[ArgIdx_RunParas -1] != Param_Unique){		
					StrBuf[0][3] = 1;
					StrBuf[0][4] = 1;
					sprintf(StrBuf[1], "121");
					sprintf(StrBuf[2], "43");
					sprintf(StrBuf[3], "175");
					sprintf(StrBuf[4], "222");
					sprintf(StrBuf[5], "5683");
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_RunParas], 6 * 20);
				}
				
				while(1){
					_Printfxy(0, 9*16, "���� <�ȴ�����> ����", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ϵ��:", &StrBuf[0][3], 4, 
						"1", "10", "100", "1000");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ת����:", StrBuf[6], 10, 10*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "NB���ӷ�ʽ:", &StrBuf[0][4], 2, 
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

					// ��ת����
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

				memcpy(&BackupBuf[ArgIdx_RunParas], &StrBuf[0][0], 6 * 20);
				BackupBuf[ArgIdx_RunParas - 1] = Param_Unique;

				// UI-��3ҳ
				currUi = 0;
				if(BackupBuf[ArgIdx_FuncEnSts -1] != Param_Unique){		
					StrBuf[0][5] = 0;
					StrBuf[0][6] = 0;
					StrBuf[0][7] = 1;
					StrBuf[0][8] = 1;
					StrBuf[0][9] = 0;
					StrBuf[0][10] = 0;
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_FuncEnSts], 1 * 20);
				}
				
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "���Źط�:", &StrBuf[0][5], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "������:", &StrBuf[0][6], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����澯:", &StrBuf[0][7], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����ϱ�:", &StrBuf[0][8], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "���ŷ���:", &StrBuf[0][9], 2, 
						"��", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ط�:", &StrBuf[0][10], 2, 
						"��", "��");
					
					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// ʹ��״̬
					enableStatus = 0;
					enableStatus |= (StrBuf[0][5] == 0 ? 0x0000 : 0x0001);	// ���Źط�
					enableStatus |= (StrBuf[0][6] == 0 ? 0x0000 : 0x0004);	// ������
					enableStatus |= (StrBuf[0][7] == 0 ? 0x0000 : 0x0010);	// �����澯
					enableStatus |= (StrBuf[0][8] == 0 ? 0x0000 : 0x0020);	// �����ϱ�
					enableStatus |= (StrBuf[0][9] == 0 ? 0x0000 : 0x0200);	// ���ŷ���
					enableStatus |= (StrBuf[0][10] == 0 ? 0x0000 : 0x0400);	// ����ط�
					

					if(isUiFinish){
						break;
					}
				}
				if (key == KEY_CANCEL){
					break;
				}
				
				memcpy(&BackupBuf[ArgIdx_FuncEnSts], &StrBuf[0][0], 1 * 20);
				BackupBuf[ArgIdx_FuncEnSts - 1] = Param_Unique;

				i = 0;
				Args.buf[i++] = 0x3F;		// ������	3F
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				switch ((uint8)StrBuf[0][0]){			// ������
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
				Args.buf[i++] = (uint8)StrBuf[0][3];	// ����ϵ��	
				Args.buf[i++] = 50;			// �Ÿ��ſ���ʱ��
				Args.buf[i++] = 100;		// ����������С�պ�ʱ��
				Args.buf[i++] = (uint8)(12000 & 0xFF);	// ���ط�ʱ�� 12000
				Args.buf[i++] = (uint8)(12000 >> 8);		
				Args.buf[i++] = 150;		// ������ֵ
				Args.buf[i++] = (uint8)StrBuf[0][1];	// ��ص�ѹ����
				Args.buf[i++] = 120;		// ��ʱ�ϴ�ʱ����	
				Args.buf[i++] = 2;			// �����ϴ��������	
				Args.buf[i++] = 3;		// ÿ�춨ʱ��������ϴ�����	
				Args.buf[i++] = (uint8)StrBuf[0][2];	// ��������������	
				Args.buf[i++] = ip[0];		// IP�ĵ�1~4���ֽ�	121.43.175.22 : 5683
				Args.buf[i++] = ip[1];
				Args.buf[i++] = ip[2];
				Args.buf[i++] = ip[3];
				Args.buf[i++] = (uint8)(port & 0xFF);	// ���������Ӷ˿�
				Args.buf[i++] = (uint8)((port >> 8) & 0xFF);
				Args.buf[i++] = 10;			// ˮ����ⶨʱ���	
				Args.buf[i++] = 0;			// ϵͳ���Լ���	
				Args.buf[i++] = 0x46;			// ��Ӫ�̱��/SIM������ NB-IOTʱ��46001000
				Args.buf[i++] = 0x00;	
				Args.buf[i++] = 0x10;	
				Args.buf[i++] = 0x00;		
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()/100));	// ����/RF�趨��ϵͳʱ�� - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()%100));		
				Args.buf[i++] = DecToBcd(_GetMonth());		
				Args.buf[i++] = DecToBcd(_GetDay());			
				Args.buf[i++] = DecToBcd(_GetHour());			
				Args.buf[i++] = DecToBcd(_GetMin());			
				Args.buf[i++] = DecToBcd(_GetSec());
				Args.buf[i++] = 8;			// ͨ��Ƶ��	
				Args.buf[i++] = ((uint8)StrBuf[0][4] + 1);	// NB���ӷ�ʽ	1 - coap ,  2- udp
				Args.buf[i++] = 0;			// Ԥ��	
				Args.buf[i++] = 0;			// Ԥ��
				Args.buf[i++] = 0;			// ʣ������������ֵ	
				Args.buf[i++] = 0;			// ʣ�������ط���ֵ	2 byte
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;			// Ԥ�ɷ����� 6 byte
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;		
				Args.buf[i++] = 0;			// �ο���ʼ���� 6 byte
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;			// ģ�����״̬	2 byte
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;			// Ԥ��	
				Args.buf[i++] = 0;			// Ԥ��
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// ��ת����	
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
				Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
				Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
				Args.buf[i++] = 0;			// ��ת���� 6 byte
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;	
				Args.buf[i++] = 0;
				Args.buf[i++] = 0;
				Args.buf[i++] = 0x00;		// ��״̬
				Args.buf[i++] = (uint8)(enableStatus & 0xFF);	// ����ʹ��״̬
				Args.buf[i++] = (uint8)(enableStatus >> 8);	
				Args.buf[i++] = 0x00;		// ����״̬��־λ
				Args.buf[i++] = 0x00;	
				Args.buf[i++] = 0;			// ���հ�������
				Args.buf[i++] = 0;			// RF������ʼʱ��	
				Args.buf[i++] = 24;			// RF��������ʱ��	
				Args.buf[i++] = 0;			// Ԥ��	10 byte
				for(u8Tmp = 0; u8Tmp < 9; u8Tmp++){
					Args.buf[i++] = 0;
				}
				// ��ȡʱ���أ����� + ģ������汾�� 40 byte	

				Args.lastItemLen = i - 1;
				break;

			case 16: 
				CurrCmd = WaterCmd_ReadNbJoinNetworkInfo;		// ��ȡNB������Ϣ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x10;		// ������	10
				ackLen = 34;				// Ӧ�𳤶� 34	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case 17: 
				CurrCmd = WaterCmd_ReadBeiJingWaterMeterParams;		// ��ȡ����ˮ�����
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x26;		// ������	26
				ackLen = 69;				// Ӧ�𳤶� 69
				// ������
				Args.buf[i++] = 0x00;		// ����ѡ�� 00 - ��ȡ�� 01 - ����		
				Args.lastItemLen = i - 1; 
				break;

			case 18: 
				CurrCmd = WaterCmd_SetBeiJingWaterMeterParams;		// ���ñ���ˮ�����
				/*---------------------------------------------*/
				// UI-��1ҳ
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_Shared -1] != Param_BeijingWMtr){		
						StrBuf[0][0] = 0;
						StrBuf[1][0] = 0;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_Shared + 1 * 20], 2 * 20);
					}
					_Printfxy(0, 9*16, "���� <�ȴ�����> ����", Color_White);
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "ˮ������:");
                    CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "  ", &StrBuf[0][0], 5, 
						"����NB�޴�", "����NB�޴�", "ɽ��NB�޴�", 
						"����NB�޴�", "��ԴNB�޴�");
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�������:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrBuf[1], 14, 15*8, true);
					break;
				}
				// ˮ������
				u32Args[0] = StrBuf[0][0];
				// �������
				if(StrBuf[1][0] > '9' || StrBuf[1][0] < '0'){
					currUi = 5;
					isUiFinish = false;
					continue;
				}
				_leftspace(StrBuf[1], 14, '0');
				memcpy(&TmpBuf[0], &StrBuf[1][0], 14);
				TmpBuf[14] = 0x00;

				memcpy(&BackupBuf[ArgIdx_Shared + 1 * 20], &StrBuf[0][0], 2 * 20);

				// UI-��2ҳ
				currUi = 0;
				if(BackupBuf[ArgIdx_Shared -1] != Param_BeijingWMtr){	
					sprintf(StrBuf[1], "40");
					sprintf(StrBuf[2], "1");
					sprintf(StrBuf[3], "40");
					sprintf(StrBuf[4], "1");
					sprintf(StrBuf[5], "10");
					sprintf(StrBuf[6], "14");
				}
				else{
					memcpy(&StrBuf[1][0], &BackupBuf[ArgIdx_Shared + 3 * 20], 6 * 20);
				}
				
				while(1){
					_Printfxy(0, 9*16, "���� <�ȴ�����> ����", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����澯��ֵ:", StrBuf[1], 6, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����澯ʱ��:", StrBuf[2], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����澯��ֵ:", StrBuf[3], 6, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����澯ʱ��:", StrBuf[4], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ѹ�澯��ֵ:", StrBuf[5], 5, 5*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "�ܼ��ϱ�ʱ���:", StrBuf[6], 2, 2*8, true);
					LableCreate(&pUi[(*pUiCnt)++], 9*16, (uiRowIdx++)*16, "��");

					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// ��Чֵ�ж�
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

				memcpy(&BackupBuf[ArgIdx_Shared + 3 * 20], &StrBuf[1][0], 6 * 20);

				// UI-��3ҳ
				currUi = 0;
				if(BackupBuf[ArgIdx_Shared -1] != Param_BeijingWMtr){	
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
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_Shared + 9 * 20], 8 * 20);
				}
				
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "���� <�ȴ�����> ����", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ն�����:", &StrBuf[0][0], 2, 
						"ͣ��", "����");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "  IP:", StrBuf[1], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, ".", StrBuf[2], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 12*8, (uiRowIdx)*16, ".", StrBuf[3], 3, 3*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 16*8, (uiRowIdx)*16, ".", StrBuf[4], 3, 3*8, true);
					uiRowIdx++;
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Port:", StrBuf[5], 5, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " APN:", StrBuf[6], 6, 7*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.isInputAny = 1;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ϱ���������:", &StrBuf[0][1], 5, 
						"0", "1", "2", "3", "4");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "���ڲ������:", StrBuf[7], 2, 2*8, true);
					LableCreate(&pUi[(*pUiCnt)++], 8*16, (uiRowIdx++)*16, "x30'");

					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// �ն�����
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
					
					// �ϱ���������
					u32Args[8] = StrBuf[0][1];

					// ���ڲ������
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

				memcpy(&BackupBuf[ArgIdx_Shared + 9 * 20], &StrBuf[0][0], 8 * 20);

				// UI-��4ҳ
				currUi = 0;
				if(BackupBuf[ArgIdx_Shared -1] != Param_BeijingWMtr){		
					for(i = 0; i < 14; i++){
						StrBuf[i][0] = 0x00;
					}
					sprintf(StrBuf[13], "24");
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_Shared + 17 * 20], 14 * 20);
				}
				
				while(1){
					_GUIRectangleFill(0, 2*16, 160, 8*16, Color_White);
					_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);
					(*pUiCnt) = 0;
					uiRowIdx = 2;

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5], StrBuf[6]);
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����ϱ���ʼʱ��:");
					LableCreate(&pUi[(*pUiCnt)++], 1*8, (uiRowIdx)*16, StrBuf[1]);			// YYYY - �����޸�
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[3], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[4], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[5], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[6], 2, 2*8, false);	// ss

					_GetDateTime(time, '-',  ':');
					DatetimeToTimeStrs(time, StrBuf[7], StrBuf[8], StrBuf[9], StrBuf[10], StrBuf[11], StrBuf[12]);
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����ϱ�����ʱ��:");
					LableCreate(&pUi[(*pUiCnt)++], 1*8, (uiRowIdx)*16, StrBuf[7]);			// YYYY - �����޸�
					TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[8], 2, 2*8, false);	// MM
					TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[9], 2, 2*8, false);	// dd
					TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[10], 2, 2*8, false);	// HH
					TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[11], 2, 2*8, false);	// mm
					TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[12], 2, 2*8, false);	// ss

					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����ϱ�����:", &StrBuf[0][0], 6, 
						"10s", "20s", "30s", "40s", "50s", "60s");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx)*16, "�����ϱ�Ƶ��:", StrBuf[13], 2, 2*8, true);
					LableCreate(&pUi[(*pUiCnt)++], 8*16, (uiRowIdx++)*16, "Сʱ");
					key = ShowUI(UiList, &currUi);
					
					if (key == KEY_CANCEL){
						break;
					}
					isUiFinish = true;

					// �����ϱ���ʼʱ��
					if( (i = TimeStrsToTimeBytes(timeBytes, StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5], StrBuf[6])) > 0){
						currUi = 1 + (i -1);
						isUiFinish = false;
						continue;
					}
					// �����ϱ�����ʱ��
					if( (i = TimeStrsToTimeBytes(&timeBytes[7], StrBuf[7], StrBuf[8], StrBuf[9], StrBuf[10], StrBuf[11], StrBuf[12])) > 0){
						currUi = 8 + (i -1);
						isUiFinish = false;
						continue;
					}
					// �����ϱ�����
					u32Args[10] = StrBuf[0][0] + 1;
					// �����ϱ�Ƶ��
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

				memcpy(&BackupBuf[ArgIdx_Shared + 17 * 20], &StrBuf[0][0], 14 * 20);

				BackupBuf[ArgIdx_Shared -1] = Param_BeijingWMtr;
				
				i = 0;
				Args.buf[i++] = 0x26;		// ������	26
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x01;		// ����ѡ�� 00 - ��ȡ�� 01 - ����	
				switch ((uint8)u32Args[0]){			// �豸����
				case 0: u8Tmp = 0x41; break;
				case 1: u8Tmp = 0x44; break;
				case 2: u8Tmp = 0x35; break;
				case 3: u8Tmp = 0x2A; break;
				case 4: u8Tmp = 0x2B; break;
				default: u8Tmp = 0x41; break;
				}
				Args.buf[i++] = u8Tmp;	
				GetBytesFromStringHex(Args.buf, i, 7, &TmpBuf[0], 0, false);	// �������	
				i += 7;
				Args.buf[i++] = (uint8)(u32Args[1] & 0xFF);	// �����澯��ֵ 4 byte
				Args.buf[i++] = (uint8)((u32Args[1] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Args[1] >> 16) & 0xFF);	
				Args.buf[i++] = (uint8)((u32Args[1] >> 24) & 0xFF);	
				Args.buf[i++] = (uint8)(u32Args[2] & 0xFF);	// �����澯ʱ�� 1 byte

				Args.buf[i++] = (uint8)(u32Args[3] & 0xFF);	// �����澯��ֵ 4 byte
				Args.buf[i++] = (uint8)((u32Args[3] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)((u32Args[3] >> 16) & 0xFF);	
				Args.buf[i++] = (uint8)((u32Args[3] >> 24) & 0xFF);	
				Args.buf[i++] = (uint8)(u32Args[4] & 0xFF);	// �����澯ʱ�� 1 byte

				Args.buf[i++] = (uint8)(u32Args[5] & 0xFF);	// ��ѹ�澯��ֵ 2 byte
				Args.buf[i++] = (uint8)((u32Args[5] >> 8) & 0xFF);		
				Args.buf[i++] = ip[0];					// IP	121.43.175.22 : 5683
				Args.buf[i++] = ip[1];
				Args.buf[i++] = ip[2];
				Args.buf[i++] = ip[3];
				Args.buf[i++] = (uint8)(port & 0xFF);	// port 2 byte
				Args.buf[i++] = (uint8)((port >> 8) & 0xFF);
				memcpy(&Args.buf[i], &TmpBuf[20], 6);	// APN	6 byte
				i += 6;
				Args.buf[i++] = (uint8)(u32Args[8] & 0xFF);	// �ϱ��������� 2 byte
				Args.buf[i++] = (timeBytes[1]);	// �����ϱ���ʼʱ�� YY MM dd HH mm ss		
				Args.buf[i++] = (timeBytes[2]);		
				Args.buf[i++] = (timeBytes[3]);			
				Args.buf[i++] = (timeBytes[4]);			
				Args.buf[i++] = (timeBytes[5]);			
				Args.buf[i++] = (timeBytes[6]);
				Args.buf[i++] = (timeBytes[8]);	// �����ϱ�����ʱ�� YY MM dd HH mm ss		
				Args.buf[i++] = (timeBytes[9]);		
				Args.buf[i++] = (timeBytes[10]);			
				Args.buf[i++] = (timeBytes[11]);			
				Args.buf[i++] = (timeBytes[12]);			
				Args.buf[i++] = (timeBytes[13]);
				Args.buf[i++] = (uint8)(u32Args[10] & 0xFF);	// �����ϱ�����ʱ��
				Args.buf[i++] = (uint8)(u32Args[7] & 0xFF);		// �ն���ͣ����	
				Args.buf[i++] = (uint8)(u32Args[11] & 0xFF);	// �����ϱ�Ƶ��
				Args.buf[i++] = (uint8)(u32Args[6] & 0xFF);		// �ܼ��ϱ���ʼʱ���	
				Args.buf[i++] = (uint8)(u32Args[9] & 0xFF);		// ���ڲ��������n x30����	
				Args.buf[i++] = 20;						// �ϱ������ȴ�ʱ�䣺�̶� 20 ����
				Args.buf[i++] = 5;						// ���ڲ���������̶� 5 ����	
				for(u8Tmp = 0; u8Tmp < 16; u8Tmp++){	// KEY	16 byte, �̶�ֵ�����裬��0
					Args.buf[i++] = 0;
				}

				Args.lastItemLen = i - 1;
				break;

			case 19: 
				CurrCmd = WaterCmd_ReadModuleFrequency;		// ��ȡģ���Ƶ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x1B;		// ������	1B
				ackLen = 8;					// Ӧ�𳤶� 8	
				// ������
				Args.buf[i++] = 0x00;		// ����ѡ�0-��ȡ�� 1-����
				Args.lastItemLen = i - 1;
				break;

			case 20: 
				CurrCmd = WaterCmd_SetModuleFrequency;		// ����ģ���Ƶ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_ModFreqs -1] == Param_Unique){		
						memcpy(&StrBuf[1][0], &BackupBuf[ArgIdx_ModFreqs], 3 * 20);
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Ƶ��1:", StrBuf[1], 5, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Ƶ��2:", StrBuf[2], 5, 6*8, true);
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "Ƶ��3:", StrBuf[3], 5, 6*8, true);
					break;
				}

				// Ƶ��1,2,3
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

				BackupBuf[ArgIdx_ModFreqs -1] = Param_Unique;	
				memcpy(&BackupBuf[ArgIdx_ModFreqs], &StrBuf[1][0], 3 * 20);
				
				i = 0;
				Args.buf[i++] = 0x1B;		// ������	1B
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x01;		// ����ѡ�0-��ȡ�� 1-����
				Args.buf[i++] = (uint8)(u32Args[1] & 0xFF);			// Ƶ��1	
				Args.buf[i++] = (uint8)((u32Args[1] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)(u32Args[2] & 0xFF);			// Ƶ��2	
				Args.buf[i++] = (uint8)((u32Args[2] >> 8) & 0xFF);
				Args.buf[i++] = (uint8)(u32Args[3] & 0xFF);			// Ƶ��3	
				Args.buf[i++] = (uint8)((u32Args[3] >> 8) & 0xFF);
				Args.lastItemLen = i - 1;
				break;

			case 21:
				CurrCmd = WaterCmd_NbReportRealTimeDataNow;		// NB�����ϱ�ʵʱ����
				if(false == isUiFinish){
					break;
				}
				i = 0;
				Args.buf[i++] = 0x21;		// ������	21
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x01;		// ����ѡ�1-�ϱ�ʵʱ����
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}

			if (key == KEY_CANCEL){
				break;
			}

			// ���� ���м̵�ַ����� �� ��ʾUI
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
					sprintf(StrDstAddr, " ������");
					currUi = 0;
					continue;
				}

				isUiFinish = true;
				continue;	// go back to get ui args
			}

			// ��ַ���
			Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen == 6)
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
			#endif

			// ���͡����ա������ʾ
			key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			#if UseBroadAddr
			if(menuItemNo == 4 && StrDstAddr[0] == 'D'){
				memcpy(StrDstAddr, strDstAddrBak, 20);
			}
			#endif

			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				isUiFinish = false;
				continue;
			}
		}
		
		#if UseBroadAddr
		if(menuItemNo == 4 && StrDstAddr[0] == 'D'){
			memcpy(StrDstAddr, strDstAddrBak, 20);
		}
		#endif

	}
}


void WaterCmdFunc(void)
{
	_GuiMenuStru menu;

	menu.left=0;
	menu.top=0;
	menu.no=5;
	menu.title= "<<���̵��� ";		// ���̵��� --> ��ԭ���� ��˲���
	menu.str[0]=" �������� ";
	menu.str[1]=" �������� ";
	menu.str[2]=" �������� ";
	menu.str[3]=" �������� ";
	menu.str[4]=" �汾��Ϣ ";
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

//-----------------------------------	���˵�	---------------------------
// ��IMEI+ICCID
void MainFuncReadImeiAndCcid(void)
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<��ȡIMEI+ICCID");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;

			#if UseBroadAddr
			// ��ȡIMEI+ICCID ʱ��ʹ��D4D4...��ַ
			memcpy(strDstAddrBak, StrDstAddr, 20);
			sprintf(StrDstAddr, "D4D4D4D4D4D4D4D4");	// ��ʼֵΪ�㲥��ַ
			#endif

			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_ReadImeiAndCcid;	// "��ȡIMEI+ICCID"

		
		switch(CurrCmd){
		case WaterCmd_ReadImeiAndCcid:		// "��ȡIMEI+ICCID"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x0F;		// ������	0F
			ackLen = 22;				// Ӧ�𳤶� 12	
			// ������
			Args.buf[i++] = 0;			// ����ѡ��	0-��ȡIMEI+ICCID��1-����IMEI�� 2-����ICCID
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// ���� ���м̵�ַ����� �� ��ʾUI
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
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
	
		#if UseBroadAddr
		if(StrDstAddr[0] == 'D'){
			memcpy(StrDstAddr, strDstAddrBak, 20);
		}
		#endif

		// ���� / ����
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

// ��ȡ�û�����
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<��ȡ�û�����");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;

			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_ReadRealTimeData;	// "��ȡ�û�����"

		
		switch(CurrCmd){
		case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x01;		// ������	01
			ackLen = 21;				// Ӧ�𳤶� 21	
			// ������
			Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if( StrDstAddr[0] < '0' || StrDstAddr[0] > '9')
			{
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
	
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// ������������ϵ��
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<������������ϵ��");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;

			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_SetBaseValPulseRatio;	// ������������ϵ��

		
		switch(CurrCmd){
		case WaterCmd_SetBaseValPulseRatio:		// ������������ϵ��
			/*---------------------------------------------*/
			if(false == isUiFinish){
				if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
					StrBuf[1][0] = 0x01;
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 2 * 20);
				}
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�û�����:", StrBuf[0], 10, 11*8, true);
				pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
				CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ϵ��:", &StrBuf[1][0], 4, 
					"1", "10", "100", "1000");
				break;
			}
			if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
				sprintf(StrBuf[0], " ������");
				currUi = uiRowIdx - 2 - 2;
				isUiFinish = false;
				continue;
			}

			memcpy(&BackupBuf[ArgIdx_MtrValPalse], &StrBuf[0][0], 2 * 20);
			BackupBuf[ArgIdx_MtrValPalse - 1] = Param_Unique;

			Args.buf[i++] = 0x06;		// ������	06
			ackLen = 7;					// Ӧ�𳤶� 7	
			// ������
			u32Tmp = (uint32) _atof(StrBuf[0]);
			u16Tmp = (uint16)((float)((_atof(StrBuf[0]) - u32Tmp)*1000.0));
			Args.buf[i++] = (uint8)(u32Tmp & 0xFF);		// �û�����	
			Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
			Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
			Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
			Args.buf[i++] = (uint8)(u16Tmp & 0xFF);		
			Args.buf[i++] = (uint8)((u16Tmp >> 8) & 0xFF);
			Args.buf[i++] = (uint8)StrBuf[1][0];		// ����ϵ��	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if( StrDstAddr[0] < '0' || StrDstAddr[0] > '9')
			{
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
	
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// ��ȡ��������
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<��ȡ��������");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_ReadFrozenData;		// "��ȡ������ת����"
		
		switch(CurrCmd){
		case WaterCmd_ReadFrozenData:		// "��ȡ������ת����"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				sprintf(StrBuf[0], "0 (0-9��Ч)");
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�� ��:", StrBuf[0], 1, 2*8, true);
				break;
			}
			
			if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
				currUi = 1;
				isUiFinish = false;
				continue;
			}
			Args.buf[i++] = 0x02;		// ������	02
			ackLen = 114;				// Ӧ�𳤶� 88/114	
			// ������
			Args.buf[i++] = 0x01;				// ���ݸ�ʽ 01/02
			Args.buf[i++] = _GetYear()/100;		// ʱ�� - yyyy/mm/dd HH:mm:ss
			Args.buf[i++] = _GetYear()%100;		
			Args.buf[i++] = _GetMonth();		
			Args.buf[i++] = _GetDay();			
			Args.buf[i++] = _GetHour();			
			Args.buf[i++] = _GetMin();			
			Args.buf[i++] = _GetSec();			
			Args.buf[i++] = StrBuf[0][0] - '0';	// �����������	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}
				
		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){
	
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// ��ȡ���ʱ��
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<��ȡ���ʱ��");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_ReadMeterTime;	// ��ȡ���ʱ��

		switch(CurrCmd){
		case WaterCmd_ReadMeterTime:		// "��ȡ���ʱ��"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x13;		// ������	13
			ackLen = 7;					// Ӧ�𳤶� 7	
			Args.lastItemLen = i - 1;
			break;
		default:
			break;
		}
		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// ���ñ��ʱ��
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<���ñ��ʱ��");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_SetMeterTime;	// ���ñ��ʱ��

		switch(CurrCmd){
		case WaterCmd_SetMeterTime:		// "���ñ��ʱ��"
			/*---------------------------------------------*/
			if(false == isUiFinish){
				_GetDateTime(time, '-',  ':');
				DatetimeToTimeStrs(time, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "ʱ ��:");
				TextBoxCreate(&pUi[(*pUiCnt)++], 0*8, (uiRowIdx)*16, " ", StrBuf[0], 4, 4*8, false);	// YYYY
				TextBoxCreate(&pUi[(*pUiCnt)++], 5*8, (uiRowIdx)*16, "-", StrBuf[1], 2, 2*8, false);	// MM
				TextBoxCreate(&pUi[(*pUiCnt)++], 8*8, (uiRowIdx)*16, "-", StrBuf[2], 2, 2*8, false);	// dd
				TextBoxCreate(&pUi[(*pUiCnt)++], 11*8, (uiRowIdx)*16, " ", StrBuf[3], 2, 2*8, false);	// HH
				TextBoxCreate(&pUi[(*pUiCnt)++], 14*8, (uiRowIdx)*16, ":", StrBuf[4], 2, 2*8, false);	// mm
				TextBoxCreate(&pUi[(*pUiCnt)++], 17*8, (uiRowIdx++)*16, ":", StrBuf[5], 2, 2*8, false);	// ss
				break;
			}
			// ʱ����ЧֵУ��
			if( (i = TimeStrsToTimeBytes(timeBytes, StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5])) > 0){
				currUi = 3 + (i -1);
				isUiFinish = false;
				continue;
			}

			sprintf(time, "%s-%s-%s %s:%s:%s",
				StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
			_SetDateTime(time);

			i = 0;
			Args.buf[i++] = 0x14;		// ������	14
			ackLen = 2;					// Ӧ�𳤶� 2	
			// ������
			Args.buf[i++] = DecToBcd(timeBytes[0]);		// ʱ�� - yyyy/mm/dd HH:mm:ss
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

		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// ���쳣����
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<���쳣");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_ClearException;		// " ���쳣���� ";
		
		switch(CurrCmd){
		case WaterCmd_ClearException:		// " ���쳣���� ";
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x05;		// ������	05
			ackLen = 1;					// Ӧ�𳤶� 1	
			// ������
			Args.buf[i++] = 0x00;		// ����ѡ�� 00	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){
	
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// ����
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<����");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_OpenValve;		// " ���� "
		
		switch(CurrCmd){
		case WaterCmd_OpenValve:			// " ���� "
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x03;		// ������	03
			ackLen = 3;					// Ӧ�𳤶� 3	
			// ������
			Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
			Args.buf[i++] = 0x01;		// ���ط���ʶ	0 - �ط��� 1 - ����
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){
		
			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// �ط�
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<�ط�");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_CloseValve;		// " �ط� ";
		
		switch(CurrCmd){
		case WaterCmd_CloseValve:		// " �ط� ";
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x03;		// ������	03
			ackLen = 3;					// Ӧ�𳤶� 3	
			// ������
			Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
			Args.buf[i++] = 0x00;		// ���ط���ʶ	0 - �ط��� 1 - ����
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}

		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){

			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}
}

// ��ȡNB������Ϣ
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

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<��ȡNB������Ϣ");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			#if (AddrLen == 6)
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
		CurrCmd = WaterCmd_ReadNbJoinNetworkInfo;
		
		switch(CurrCmd){
		case WaterCmd_ReadNbJoinNetworkInfo:		// ��ȡNB������Ϣ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x10;		// ������	10
				ackLen = 34;				// Ӧ�𳤶� 34	
				// ������
				Args.lastItemLen = i - 1;
				break;

		default: 
			break;
		}

		// ���� ���м̵�ַ����� �� ��ʾUI
		if(false == isUiFinish){

			key = ShowUI(UiList, &currUi);

			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
				sprintf(StrDstAddr, " ������");
				currUi = 0;
				continue;
			}

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ��ַ���
		Water6009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
		#if (AddrLen == 6)
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
		#else
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
		#endif

		// ���͡����ա������ʾ
		key = Protol6009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			isUiFinish = false;
			continue;
		}
	}

}

// ���̵���
void MainFuncEngineerDebuging(void)
{
	WaterCmdFunc();		// ���̵��� --> ��ԭ���� ��˲���
}

// --------------------------------   ������   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	
	MeterNoLoad(StrDstAddr);
	SysCfgLoad();

	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=8;
	MainMenu.title =  VerInfo_Name;
	MainMenu.str[0] = "��ȡIMEI+ICCID";
	MainMenu.str[1] = "��ȡ�û�����";
	MainMenu.str[2] = "������������ϵ��";
	MainMenu.str[3] = "����";
	MainMenu.str[4] = "�ط�";
    MainMenu.str[5] = "���쳣";
	MainMenu.str[6] = "��ȡNB������Ϣ";
	MainMenu.str[7] = "���̵���";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.key[7] = "8";
	MainMenu.Function[0] = MainFuncReadImeiAndCcid;
	MainMenu.Function[1] = MainFuncReadRealTimeData;
	MainMenu.Function[2] = MainFuncSetMeterValAndPulseRatio;
	MainMenu.Function[3] = MainFuncOpenValve;
	MainMenu.Function[4] = MainFuncCloseValve;
    MainMenu.Function[5] = MainFuncClearException;
	MainMenu.Function[6] = MainFuncReadNbJoinNetworkInfo;
	MainMenu.Function[7] = MainFuncEngineerDebuging;	// ���̵��� --> ��ԭ���� ��˲���
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();
}

