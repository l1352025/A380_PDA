/*
*
*	ɣ��6009�ֳֻ�-Lora�汾������-���ư棩
*
*/
#include "A3_6009HL.h"

//#define Use_CoreFunc	 	// �Ƿ�ֻ���ú��Ĺ��ܣ������������������ݡ��������ط������쳣

#ifdef Use_CoreFunc
#include "HJLIB.H"
#include "string.h"
#include "stdio.h"
#include "common.h"
#include "common.c"
#include "MeterDocDBF_HL.h"		
#include "MeterDocDBF_HL.c"
#include "WaterMeter.h"
#else
#include "MeterDocDBF_HL.h"		
#include "MeterDocDBF_HL.c"
#include "SR6009_RF.c"         //  DBF���ݿ�ӿڲ�ͬ������������ SR6009_RF һ��
#endif

#ifdef Use_CoreFunc
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
	ListBoxCreate(&menuList, 0, 0, 20, 7, 7, NULL,
		"<<���̵���",
		7,
		"1. ��ȡ�û�����",
		"2. ��ȡ��������",
		"3. ����",
		"4. ǿ�ƿ���",
		"5. �ط�",
		"6. ǿ�ƹط�",
		"7. ���쳣����"
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
			CurrCmd = (0x10 + menuItemNo);

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
				uiRowIdx += CreateRelayAddrsUi(pUi, pUiCnt, uiRowIdx);
				
				key = ShowUI(UiList, &currUi);

				if (key == KEY_CANCEL){
					break;
				}

				if(StrDstAddr[0] < '0' || StrDstAddr[0] > '9' ){
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
		LogPrint("�������� �ѽ��� \n");
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
		LogPrint("�������� ���˳���\n\n\n");
	#endif

}

#endif