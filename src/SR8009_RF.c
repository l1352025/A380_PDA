/*
*
*	ɣ��8009ˮ���ֳֻ� - RF��
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
		sprintf(strs[i], "�б��� %d", i);
	}
}


#if CenterCmd_Enable
// --------------------------------  ������ģ��ͨ��  -----------------------------------------
// 1	��������
void CenterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 9, NULL,
		"<<��������",
		9,
		"1. ��ȡ��������",
		"2. ��ȡ�������汾",
		"3. ��ȡ������ʱ��",
		"4. ���ü�����ʱ��",
		"5. ��ȡGPRS����",
		"6. ����GPRS����",
		"7. ��GPRS�ź�ǿ��",
		"8. ��������ʼ��",
		"9. ������������ģʽ"
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
			CurrCmd = (0x1010 + menuItemNo);

			switch(CurrCmd){
			case CenterCmd_ReadCenterNo:	// ����������
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x41;		// ������	41
				ackLen = 6;					// Ӧ�𳤶� 6	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadCenterVer:	// ���������汾
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x40;		// ������	40
				ackLen = 6;					// Ӧ�𳤶� 6	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadCenterTime:		// ��������ʱ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x43;		// ������	43
				ackLen = 7;					// Ӧ�𳤶� 7	
				// ������
				Args.lastItemLen = i - 1;
				break;
			
			case CenterCmd_SetCenterTime:		// �輯����ʱ��
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
					_Printfxy(0, 3*16, "ʱ ��: ", Color_White);
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
						// ��Чֵ 
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
				Args.buf[i++] = 0x14;		// ������	14
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()/100));		// ʱ�� - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = DecToBcd((uint8)(_GetYear()%100));		
				Args.buf[i++] = DecToBcd(_GetMonth());		
				Args.buf[i++] = DecToBcd(_GetDay());			
				Args.buf[i++] = DecToBcd(_GetHour());			
				Args.buf[i++] = DecToBcd(_GetMin());			
				Args.buf[i++] = DecToBcd(_GetSec());	
				Args.lastItemLen = i - 1;
				break;

            case CenterCmd_ReadGprsParam:		// ��GPRS����
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x45;		// ������	45
				ackLen = 40;				// Ӧ�𳤶� Լ 40	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_SetGprsParam:		// ��GPRS����
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x46;		// ������	46
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x01;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x00;		// ���ط���ʶ	0 - �ط��� 1 - ����
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadGprsSignal:		// ��GPRS�ź�ǿ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x47;		// ������	47
				ackLen = 50;				// Ӧ�𳤶� Լ 50	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_InitCenter:			// ��������ʼ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����:", &StrBuf[0][0], 2, 
						"��յ�����·��", "�����������");
					break;
				}
				Args.buf[i++] = 0x48;		// ������	48
				ackLen = 2;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = (uint8)StrBuf[0][0];	// ��������	
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadCenterWorkMode:	// ������������ģʽ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x49;		// ������	49
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
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

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

// 2	��������
void CenterCmdFunc_DocumentOperation(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 5, NULL,
		"<<��������",
		5,
		"1. ����������",
		"2. ��������Ϣ",
		"3. ��ӵ�����Ϣ",
		"4. ɾ��������Ϣ",
		"5. �޸ĵ�����Ϣ"
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
			CurrCmd = (0x1020 + menuItemNo);

			switch(CurrCmd){
			case CenterCmd_ReadDocCount:		// ����������
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
				Args.buf[i++] = 0x01;		// ������	01
				ackLen = 21;				// Ӧ�𳤶� 21	
				// ������
				Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_ReadDocInfo:			// ��������Ϣ
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
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_AddDocInfo:			// ��ӵ�����Ϣ
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
			
			case CenterCmd_DeleteDocInfo:		// ɾ��������Ϣ
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				// ������
				Args.buf[i++] = 0x01;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x01;		// ���ط���ʶ	0 - �ط��� 1 - ����
				Args.lastItemLen = i - 1;
				break;

            case CenterCmd_ModifyDocInfo:		// �޸ĵ�����Ϣ
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
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

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

// 3	·������
void CenterCmdFunc_RouteSetting(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 2, NULL,
		"<<·������",
		2,
		"1. ��ȡ�Զ���·��",
		"2. �����Զ���·��"
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
			CurrCmd = (0x1030 + menuItemNo);

			switch(CurrCmd){
			case CenterCmd_ReadDefinedRoute:	// ���Զ���·��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�� ��: ", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);
					break;
				}
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x55;		// ������	55
				ackLen = 21;				// Ӧ�𳤶� 21	
				// ������
				Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
				Args.lastItemLen = i - 1;
				break;

			case CenterCmd_SetDefinedRoute:		// ���Զ���·��
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
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

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

// 4	����ת��
void CenterCmdFunc_CommandTransfer(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	_ClearScreen();

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<����ת��",
		7,
		"1. ��ʵʱ����",
		"2. ����ʱ��������",
		"3. ����������",
		"4. ����",
		"5. �ط�",
		"6. ��ʹ��",
		"7. ���쳣"
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
			CurrCmd = (0x1040 + menuItemNo);

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
			
			case WaterCmd_OpenValveForce:		// " ǿ�ƿ��� ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				// ������
				Args.buf[i++] = 0x01;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x01;		// ���ط���ʶ	0 - �ط��� 1 - ����
				Args.lastItemLen = i - 1;
				break;

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

			case WaterCmd_CloseValveForce:		// " ǿ�ƹط� ";
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x03;		// ������	03
				ackLen = 3;					// Ӧ�𳤶� 3	
				// ������
				Args.buf[i++] = 0x01;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
				Args.buf[i++] = 0x00;		// ���ط���ʶ	0 - �ط��� 1 - ����
				Args.lastItemLen = i - 1;
				break;

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
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

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

void CenterCmdFunc(void)
{
	_GuiMenuStru menu;
	int fp;

	fp = _Fopen("system.cfg", "R");
	_Lseek(fp, 20, 0);	// byte [20 ~ 29] �������� 
	_Fread(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);

	menu.left=0;
	menu.top=0;
	menu.no=4;
	menu.title= "<<���������� ";
	menu.str[0]=" �������� ";
	menu.str[1]=" �������� ";
	menu.str[2]=" ·������ ";
	menu.str[3]=" ����ת�� ";
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
	_Lseek(fp, 0, 0);	// byte [0 ~ 19] ��� 
	_Fread(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);
	
}
#endif

// --------------------------------  ˮ��ģ��ͨ��  -----------------------------------------

// 1	���ù���
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

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<���ù���",
		7,
		"1. ��������",
		"2. ������",
		"3. ����",
		"4. ��ط�",
		"5. ����쳣",
		"6. �������",
		"7. ���ñ��"
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

			case WaterCmd_SetBaseValPulseRatio:	// ������ �� ����ϵ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
						StrBuf[1][0] = 0x01;
					}
					else{
						memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 2 * 20);
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����:", StrBuf[0], 9, 11*8, true);
					pUi[(*pUiCnt) -1].ui.txtbox.dotEnable = 1;
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��˿ھ�:", &StrBuf[1][0], 2, 
						"С", "��");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ϵ��:", &StrBuf[1][1], 3, 
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

				Args.buf[i++] = 0x04;		// ������	04
				ackLen = 5;					// Ӧ�𳤶� 5	
				// ������
				Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);		// ����������� 3byte
				Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
				Args.buf[i++] = (uint8)(u32Tmp & 0xFF);	
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);				// �������С�� 1byte
				switch (StrBuf[1][1])
				{
				case 0: u8Tmp = 1; break;
				case 1: u8Tmp = 10; break;
				case 2: u8Tmp = 100; break;
				default: u8Tmp = 10; break;
				}
				Args.buf[i++] = (uint8)((StrBuf[1][0] << 7) | u8Tmp);	// ��ھ�|����ϵ��	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadFrozenData:		// ��ȡ������ת����
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

			case WaterCmd_ReadMeterCfgInfo:		// �������
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x02;		// ������	02
				ackLen = 30;				// Ӧ�𳤶� 30	
				// ������
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetMeterNumber:		// ���ñ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					#if (AddrLen <= 6)
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�±��:", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);
					#else
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�±��:");
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrBuf[0], AddrLen*2, (AddrLen*2*8 + 8), true);	
					#endif
					break;
				}

				// �ȶ�ȡ����������Ϣ - ��ȡ�汾��
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
				Args.buf[i++] = 0x02;		// ������	
				ackLen = 30;				// Ӧ�𳤶�	
				timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
				tryCnt = 2;	
				Args.lastItemLen = i - 1;

				// ����ȡ���汾�ţ������ñ��
				if(CmdResult_Ok == ProtolCommandTranceiver(WaterCmd_ReadMeterCfgInfo, &Addrs, &Args, ackLen, timeout, tryCnt)){
					i = 0;
					Args.buf[i++] = 0x1C;		// ������
					ackLen = 7;					// Ӧ�𳤶� 
					timeout = 2000 + (Addrs.itemCnt - 1) * 2000;
					tryCnt = 2;	
					// ������
					Args.buf[i++] = 0xA2;		// ����ѡ��	
					memcpy(&Args.buf[i], &VerInfo[0], VerLen);	
					i += VerLen;				// ����汾��
					GetBytesFromStringHex(&Args.buf[i], 0, 6, StrBuf[0], 0, false);
					i += 6;						// �µ�ַ
					Args.lastItemLen = i - 1;

					if(CmdResult_Ok == ProtolCommandTranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt)){
						memcpy(StrDstAddr, StrBuf[0], TXTBUF_LEN);
					}
				}

				key = _ReadKey();

				// ���� / ����
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

// 2	��������
void WaterCmdFunc_FunctionConfig(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	ListBox menuList, menuList_2;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish, u8Tmp;
	uint16 ackLen = 0, timeout;
	uint8 uiPage, isSendReady = false;	// uiPage: 0 - ��ҳ , isSendReady: ���;���

	_ClearScreen();

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 8, NULL,
		"<<��������",
		8,
		"1. ��ȡ��ת����",
		"2. �����ת����",
		"3. ��ȡ����״̬old",
		"4. ��ȡ����״̬new",
		"5. ���ù���״̬",
		"6. ��ѯʱ�Ӽ�RF״̬",
		"7. ��ѯRF����ʱ��",
		"8. ����RF����ʱ��"
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
		uiPage = 1;

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

			if(uiPage == 1){
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
				i = 0;	
			}

			// �����������
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // ������
			Args.items[1] = &Args.buf[1];	// ������
			CurrCmd = (0x20 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadReverseMeasureData:		// ��ȡ��ת����
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x0A;		// ������	0A
				ackLen = 9;					// Ӧ�𳤶� 9	
				// ������
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_ClearReverseMeasureData:		// �����ת����
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x12;		// ������	12
				ackLen = 0;					// Ӧ�𳤶� 0	
				// ������
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_ReadFuncEnableStateOld:		// ������״̬old
			case WaterCmd_ReadFuncEnableStateNew:		// ������״̬new
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x17;		// ������	17
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_SetFuncEnableState:		// ���ù���״̬
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
						// �˵�
						ListBoxCreate(&menuList_2, 0, 0, 20, 7, 10, NULL,
							"<<ѡ��������",
							10,
							"1. �Ÿ��Źط�-����",
							"2. �Ÿ��Źط�-�ر�",
							"3. �Ÿ��ż��-�ر�",
							"4. ����ж-����",
							"5. ����ж-�ر�",
							"6. ��ֱ��װ���-����",
							"7. ��ֱ��װ���-�ر�",
							"8. �����澯-����",
							"9. �����澯-�ر�",
							"10.�ϱ��ͷ���-����"
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

					// ������	0C~16
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
					
					ackLen = 0;					// Ӧ�𳤶� 0	
					// ������
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
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʱ����:", &StrBuf[1][0], 2, 
						"�ر�", "����");
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʱ�ϱ�:", &StrBuf[1][1], 2, 
						"�ر�", "����");
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�����ϱ�:", &StrBuf[1][2], 2, 
						"�ر�", "����");
						break;
					}
					
					// ������	19-00/01/02/04/08
					i = 0;
					Args.buf[i++] = 0x19;
					ackLen = 0;					// Ӧ�𳤶� 0	
					// ������
					// �������֣�b0-���⿪��b1-����أ�b2-��ʱ��/�أ�b3-������/��
					u8Tmp = StrBuf[1][0] == 0 ? 0x02 : 0x01;
					u8Tmp |= StrBuf[1][1] << 2;
					u8Tmp |= StrBuf[1][2] << 3;
					Args.buf[i++] = u8Tmp;		// ��������
					Args.lastItemLen = i - 1;
					isSendReady = true;
				}
				break;

			case WaterCmd_ReadTimeAndRfState:		// ��ѯʱ�Ӽ�RF״̬
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// ������	09-00
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x00;		// �������� 00
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_ReadRfWorkTime:		// ��ѯRF����ʱ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// ������	09-04
				ackLen = 2;					// Ӧ�𳤶� 2	
				// ������
				Args.buf[i++] = 0x04;
				Args.lastItemLen = i - 1;
				isSendReady = true;
				break;

			case WaterCmd_SetRfWorkTime:		// ����RF����ʱ��
				/*---------------------------------------------*/
				if(uiPage == 1){
					if(false == isUiFinish){
						LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ģʽ:");
						CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, " ", &StrBuf[0][0], 2, 
						"ÿ�¹���ʱ��", "ÿ�칤��ʱ��");
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
							LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "ÿ�¹���ʱ��:");
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʼ��(1-31):", StrBuf[1], 2, 2*8, true);
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "������(1-31):", StrBuf[2], 2, 2*8, true);
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʼʱ(0-23):", StrBuf[3], 2, 2*8, true);
						}
						else{
							if(StrBuf[4][0] == 0x00){
								sprintf(StrBuf[4], "06");
								sprintf(StrBuf[5], "19");
							}
							LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "ÿ�칤��ʱ��:");
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��ʼʱ(0-23):", StrBuf[4], 2, 2*8, true);
							TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����ʱ(0-23):", StrBuf[5], 2, 2*8, true);
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
					Args.buf[i++] = 0x09;		// ������	09-84
					ackLen = 2;					// Ӧ�𳤶� 2	
					// ������
					Args.buf[i++] = 0x84;		// ������ 84
					Args.buf[i++] = TmpBuf[10];	// 4�ֽ�ʱ������
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

			// ���� ���м̵�ַ����� �� ��ʾUI
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

			// ��ַ���
			Water8009_PackAddrs(&Addrs, StrDstAddr, StrRelayAddr);
			#if (AddrLen <= 6)
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);
			#else
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��:\n   %s", StrDstAddr);
			#endif

			// ���͡����ա������ʾ
			key = Protol8009TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			isSendReady = false;
			isUiFinish = false;
			
			// ���� / ����
			if (key == KEY_CANCEL){
				uiPage--;
			}
		}
		
	}
}

// 3	DMA��Ŀ
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

	// �˵�
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<��������",
		7,
		"1. �����ϴ�����Ƶ��",
		"2. ʹ���ϴ��������",
		"3. �����ϴ�ʱ����",
		"4. ��ȡ�ϴ�ʱ����",
		"5. ��������ת����",
		"6. ����ʱ��",
		"7. ��ȡʱ��"
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
			CurrCmd = (0x30 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_UploadCenterFrequency:		// �����ϴ�����Ƶ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "����Ƶ��:");
					CombBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "", &StrBuf[1][0], 2, 
						"0: 470 MHz", "1: 478.3 MHz");
					break;
				}
				Args.buf[i++] = 0x09;		// ������	01
				ackLen = 2;					// Ӧ�𳤶� 2/1	
				// ������
				Args.buf[i++] = 0x09 | (StrBuf[1][0] << 7);	// ��������	09/89
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_EnableReportAmeterData:		// ʹ���ϴ��������
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// ������	09
				ackLen = 2;					// Ӧ�𳤶� 2/1	
				// ������
				Args.buf[i++] = 0x8B;		// ��������	0B/8B - ���ϴ�/�����ϴ�
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetReportTimeInterval:		// �����ϴ�ʱ���� 
				/*---------------------------------------------*/
				if(false == isUiFinish){
					if(StrBuf[0][0] == 0x00){
						sprintf(StrBuf[0], "  (0-255 h)");
					}
					TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "ʱ����:", StrBuf[0], 3, 11*8, true);
					break;
				}
				u16Tmp = (uint16)_atof(StrBuf[0]);
				if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0' || u16Tmp > 255){
					sprintf(StrBuf[0], "  (0-255 h)");
					currUi = 1;
					isUiFinish = false;
					continue;
				}
				Args.buf[i++] = 0x09;		// ������	09
				ackLen = 2;					// Ӧ�𳤶� 2/1	
				// ������
				Args.buf[i++] = 0x8A;		// ��������	0A/8A - ��ȡ/����
				Args.buf[i++] = (uint8)(u16Tmp & 0xFF);	
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_ReadReportTimeInterval:		// ��ȡ�ϴ�ʱ����
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// ������	09
				ackLen = 2;					// Ӧ�𳤶� 2/1	
				// ������
				Args.buf[i++] = 0x0A ;		// ��������	0A/8A - ��ȡ/����
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadFrozenData:			// ��������ת����
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
				Args.buf[i++] = 0x09;		// ������	09
				ackLen = 0;					// Ӧ�𳤶� 0	
				// ������
				Args.buf[i++] = 0x05;		// ��������	05/06 - ����ת/����ת
				Args.buf[i++] = StrBuf[0][0] - '0';	// �����������	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_SetMeterTime:				// У���ʱ�� 
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
					currUi = 2 + (i -1);
					isUiFinish = false;
					continue;
				}

				sprintf(time, "%s-%s-%s %s:%s:%s",
					StrBuf[0], StrBuf[1], StrBuf[2], StrBuf[3], StrBuf[4], StrBuf[5]);
				_SetDateTime(time);

				i = 0;
				Args.buf[i++] = 0x01;		// ������	01 ������ʱ����ʱ�䣩
				ackLen = 9;					// Ӧ�𳤶� 9/13	
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

			case WaterCmd_ReadMeterTime:	// �����ʱ��
				/*---------------------------------------------*/
				if(false == isUiFinish){
					break;
				}
				Args.buf[i++] = 0x09;		// ������	09
				ackLen = 0;					// Ӧ�𳤶� 0	
				// ������
				Args.buf[i++] = 0x00;		// ��������	00
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
	menu.title= "<<���̵��� ";		// ���̵��� --> ��ԭ���� ��˲���
	menu.str[0]=" ���ù��� ";
	menu.str[1]=" �������� ";
	menu.str[2]=" DMA��Ŀ ";
	#ifdef UseFunc_BatchOpenCloseValve
	menu.str[3]=" �������ط� ";
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


//-----------------------------------	������	---------------------------
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
		CurrCmd = WaterCmd_ReadRealTimeData;	// "��ȡ�û�����"

		
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

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
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
		CurrCmd = WaterCmd_ClearException;		// " ���쳣���� ";
		
		switch(CurrCmd){
		case WaterCmd_ClearException:		// " ���쳣���� ";
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

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
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
		CurrCmd = WaterCmd_OpenValve;		// " ���� "
		
		switch(CurrCmd){
		case WaterCmd_OpenValve:			// " ���� "
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x05;		// ������	05
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

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
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
		CurrCmd = WaterCmd_CloseValve;		// " �ط� ";
		
		switch(CurrCmd){
		case WaterCmd_CloseValve:		// " �ط� ";
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			Args.buf[i++] = 0x06;		// ������	06
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

			if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9'  ){
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

// ���̵���		------------------------------
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
	
	#if LOG_ON
		LogPrint("������� �ѽ��� \n");
	#endif

	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=7;
	MainMenu.title =  VerInfo_Name;
	MainMenu.str[0] = " �������� ";
	MainMenu.str[1] = " �������� ";
	MainMenu.str[2] = " ���� ";
	MainMenu.str[3] = " �ط� ";
	MainMenu.str[4] = " ���쳣 ";
	MainMenu.str[5] = " ���̵��� ";
	MainMenu.str[6] = " �汾��Ϣ ";
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
	MainMenu.Function[5] = MainFuncEngineerDebuging;	// ���̵��� --> ��ԭ���� ��˲���
	MainMenu.Function[6] = VersionInfoFunc;	
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();

	#if LOG_ON
		LogPrint("������� ���˳���\n\n\n");
	#endif

}

