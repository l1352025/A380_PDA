
#include <HJLIB.H>
#include "string.h"
#include "dbf.h"
#include "stdio.h"

#include "Common.h"
#include "Tool.h"
#include "PowerMeter.h"
#include "WaterMeter.h"


// --------------------------------  ȫ�ֱ���  -----------------------------------------
char Screenbuff[160*(160/3+1)*2]; 
uint8 TmpBuf[1080];
uint8 TxBuf[1080];
uint8 RxBuf[1080];
uint32 RxLen, TxLen;
uint8 LocalAddr[7] = { 0x20, 0x19, 0x00, 0x00, 0x20, 0x19, 0x00};	// ��ַ 201900002019��12�ַ�
uint8 DstAddr[7];
uint8 VersionInfo[40];
uint8 CurrCmd;
ParamsBuf Addrs;		
ParamsBuf Args;
ParamsBuf Disps;
uint8 StrDstAddr[TXTBUF_LEN] = {0};
uint8 StrRelayAddr[RELAY_MAX][TXTBUF_LEN] = {0};
UI_ItemList UiList;

//--------------------------------------	6009ˮ������ ���͡����ա������ʾ	----------------------------

uint8 Protol6009Tranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt)
{
	uint8 sendCnt = 0, key, cmdResult;
	uint16 waitTime = 0;
	uint8 * lines[30];
	const uint8 lineStep = 3, lineMax = 7;
	int8 lineCnt = 0, currLine = 0;
	int fp;

	if(_Access("system.cfg", 0) < 0){
		fp = _Fopen("system.cfg", "W");
	}else{
		fp = _Fopen("system.cfg", "RW");
	}
	_Lseek(fp, 0, 0);
	_Fwrite(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);

	timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
	tryCnt = 3;

	_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
	PrintfXyMultiLine_VaList(0, 1*16 + 8, "����: %s ", StrDstAddr);

	do{
		// ���� 
		TxLen = PackWater6009RequestFrame(TxBuf, addrs, cmdid, args, sendCnt);
		_GetComStr(TmpBuf, 1024, 100/10);	// clear , 100ms timeout
		_SendComStr(TxBuf, TxLen);
		sendCnt++;
		if(sendCnt == 1){
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "״̬: �����    ", Color_White);
		}
		else{
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			PrintfXyMultiLine_VaList(0, 9*16, "״̬: �����ط� %d  ", sendCnt);
		}

		// ����
		_GetComStr(TmpBuf, 1024, 100/10);	// clear , 100ms timeout
		RxLen = 0;
		waitTime = 0;
		PrintfXyMultiLine_VaList(0, 5*16, "�ȴ�Ӧ�� %d/%d  %d s ", RxLen, ackLen, (timeout / 1000));

		do{
			RxLen += _GetComStr(&RxBuf[RxLen], ackLen - RxLen, 10);	// 100ms ������
			if(KEY_CANCEL == _GetKeyExt()){
				//------------------------------------------------------
				_GUIHLine(0, 9*16 - 4, 160, Color_Black);
				_Printfxy(0, 9*16, "״̬: ������ȡ��   ", Color_White);
				return false;
			}
			waitTime += 100;
		}while(RxLen < ackLen && waitTime < timeout);

		PrintfXyMultiLine_VaList(0, 5*16, "��ǰӦ�� %d/%d  ", RxLen, ackLen);
		_SoundOn();
		_Sleep(50);
		_SoundOff();

		cmdResult = ExplainWater6009ResponseFrame(RxBuf, RxLen, LocalAddr, CurrCmd, ackLen, &Disps);
		if(false == cmdResult){
			_Sleep(30);
			_SoundOn();
			_Sleep(30);
			_SoundOff();
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "״̬: ����ʧ��    ", Color_White);
		}
		else{
			//------------------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "״̬: ����ɹ�    ", Color_White);
		}
	}while(sendCnt < tryCnt && cmdResult == false);

	// ��ʾ���
	if(cmdResult == true){
		_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
	}

	lineCnt = GetPrintLines(0, Disps.items[0], lines);
	PrintfXyMultiLine(0, 1*16 + 8, lines[currLine], lineMax);

	// ��/�¹�����ʾ   ��  ��  �� ��
	while(1){

		key = _ReadKey();

		if(key == KEY_CANCEL || key == KEY_ENTER){
			break;
		}
		else if(key == KEY_UP && lineCnt > lineMax){
			currLine -= lineStep;
			if(currLine < 0){
				currLine = 0;
			}
		}
		else if(key == KEY_DOWN && lineCnt > lineMax){
			currLine += lineStep;
			if(currLine > lineCnt - lineMax){
				currLine = lineCnt - lineMax;
			}
		}
		else{
			continue;
		}

		_GUIRectangleFill(0, 1*16 + 8, 160, 8*16 + 8, Color_White);
		PrintfXyMultiLine(0, 1*16 + 8, lines[currLine], lineMax);
	}

	return key;
}


// --------------------------------  ������ģ��ͨ��  -----------------------------------------
// 1	��������
void CenterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<��������";
	menuList.no = 9;
	menuList.MaxNum = 9;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ����������";
	menuList.str[1] = "  2. ���������汾";
	menuList.str[2] = "  3. ��������ʱ��";
	menuList.str[3] = "  4. �輯����ʱ��";
	menuList.str[4] = "  5. ��GPRS����";
	menuList.str[5] = "  6. ��GPRS����";
	menuList.str[6] = "  7. ��GPRS�ź�ǿ��";
	menuList.str[7] = "  8. ��������ʼ��";
	menuList.str[8] = "  9. ������������ģʽ";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "״̬: <����>    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 2	��������
void CenterCmdFunc_DocumentOperation(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<< ��������";
	menuList.no = 5;
	menuList.MaxNum = 5;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ����������";
	menuList.str[1] = "  2. ��������Ϣ";
	menuList.str[2] = "  3. ���ӵ�����Ϣ";
	menuList.str[3] = "  4. ɾ��������Ϣ";
	menuList.str[4] = "  5. �޸ĵ�����Ϣ";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 3	·������
void CenterCmdFunc_RouteSetting(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<·������";
	menuList.no = 2;
	menuList.MaxNum = 2;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ���Զ���·��";
	menuList.str[1] = "  2. ���Զ���·��";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 4	����ת��
void CenterCmdFunc_CommandTransfer(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<< ����ת��";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ��ʵʱ����";
	menuList.str[1] = "  2. ����ʱ��������";
	menuList.str[2] = "  3. ����������";
	menuList.str[3] = "  4. ����";
	menuList.str[4] = "  5. �ط�";
	menuList.str[5] = "  6. ��ʹ��";
	menuList.str[6] = "  7. ���쳣";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
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
	
}

// --------------------------------  ˮ��ģ��ͨ��  -----------------------------------------

// 1	��������
void WaterCmdFunc_CommonCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<��������";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ��ȡ�û�����";
	menuList.str[1] = "  2. ��ȡ������ת����";
	menuList.str[2] = "  3. ����";
	menuList.str[3] = "  4. ǿ�ƿ���";
	menuList.str[4] = "  5. �ط�";
	menuList.str[5] = "  6. ǿ�ƹط�";
	menuList.str[6] = "  7. ���쳣����";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/
			//----------------------------------------------
			_GUIHLine(0, 9*16 - 4, 160, Color_Black);
			_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
					StrRelayAddr[i][0] = 0x00;
					sprintf(StrRelayAddr[i], " <��ѡ> ");
				}
			}

			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			i = 0;	
			Args.itemCnt = 2;
			Args.items[0] = &Args.buf[0];   // ������
			Args.items[1] = &Args.buf[1];	// ������
			CurrCmd = (0x10 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
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
				sprintf(TmpBuf, "0");
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "�������:", &TmpBuf[0], 1, 2*8);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(TmpBuf[0] > '9' || TmpBuf[0] < '0'){
					PrintfXyMultiLine_VaList(0, 6*16, "�������: <0~9��Ч>");
					continue;
				}
				Args.buf[i++] = 0x02;		// ������	02
				ackLen = 88;				// Ӧ�𳤶� 88	
				// ������
				Args.buf[i++] = 0x02;				// ���ݸ�ʽ 01/02
				Args.buf[i++] = _GetYear()/100;		// ʱ�� - yyyy/mm/dd HH:mm:ss
				Args.buf[i++] = _GetYear()%100;		
				Args.buf[i++] = _GetMonth();		
				Args.buf[i++] = _GetDay();			
				Args.buf[i++] = _GetHour();			
				Args.buf[i++] = _GetMin();			
				Args.buf[i++] = _GetSec();			
				Args.buf[i++] = TmpBuf[0] - '0';	// �����������	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_OpenValve:			// " ���� "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
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
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
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
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
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
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
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
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
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


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 1*16 + 8, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 2	��������
void WaterCmdFunc_TestCmd(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<��������";
	menuList.no = 8;
	menuList.MaxNum = 8;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ��������";
	menuList.str[1] = "  2. �����¶�";
	menuList.str[2] = "  3. ������ѹ";
	menuList.str[3] = "  4. ��Ԥ�ɲο���";
	menuList.str[4] = "  5. ���ù�����ʱ";
	menuList.str[5] = "  6. ����Ӫ�̱��";
	menuList.str[6] = "  7. ���ϱ�·��";
	menuList.str[7] = "  8. ���ñ���";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x20 + menuItemNo);

			switch(CurrCmd){
			case WaterCmd_RebootDevice:			// "��������"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x04;		// ����ѡ�� 04	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadTemperature:			// " �����¶� "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x05;		// ����ѡ�� 05	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadVoltage:				// " ������ѹ "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x07;		// ����ѡ�� 07	
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_ClearPrepaidRefVal:		// " ��Ԥ�ɲο��� ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x08;		// ����ѡ�� 08	
				Args.lastItemLen = i - 1;
				break;

            case WaterCmd_SetOverCurrentTimeout:		// " ���ù�����ʱ ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x09;		// ����ѡ�� 09	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadOperatorNumber:		// " ����Ӫ�̱�� ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x0A;		// ����ѡ�� 0A	
				Args.lastItemLen = i - 1;
				break;

			case WaterCmd_ReadReportRoute:		// " ���ϱ�·�� ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x0E;		// ����ѡ�� 0E	
				Args.lastItemLen = i - 1;
				break;
			
			case WaterCmd_SetMeterNumber:		// " ���ñ��� ";
				/*---------------------------------------------*/
				TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "�±���:", TmpBuf, 12, 13*8);
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				if(VersionInfo[0] == 0x00){
					_Printfxy(0, 5*16, "���ȶ�ȡ�����汾", Color_White);
					break;
				}
				Args.buf[i++] = 0x07;		// ������	07
				ackLen = 1;					// Ӧ�𳤶� 1	
				// ������
				Args.buf[i++] = 0x10;		// ����ѡ�� 10	
				memcpy(&Args.buf[i], VersionInfo, 40);	
				i += 40;					// �����汾��
				VersionInfo[0] = 0x00;
				GetBytesFromStringHex(&Args.buf[i], 0, 6, TmpBuf, 0, false);
				i += 6;						// �µ�ַ
				Args.lastItemLen = i - 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 3	��������
void WaterCmdFunc_Upgrade(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<��������";
	menuList.no = 7;
	menuList.MaxNum = 7;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ��������";
	menuList.str[1] = "  2. ��ѯ����";
	menuList.str[2] = "  3. �㲥����";
	menuList.str[3] = "  4. ���ӵ���";
	menuList.str[4] = "  5. ɾ������";
	menuList.str[5] = "  6. ��ѯ����";
	menuList.str[6] = "  7. ����ͳ��";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 4	Ԥ������
void WaterCmdFunc_PrepaiedVal(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<Ԥ������";
	menuList.no = 6;
	menuList.MaxNum = 6;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. ��Ԥ�ɲο�����";
	menuList.str[1] = "  2. ��Ԥ�ɲο�����";
	menuList.str[2] = "  3. ��������ֵ͸֧";
	menuList.str[3] = "  4. �豨����ֵ";
	menuList.str[4] = "  5. ��ط���ֵ";
	menuList.str[5] = "  6. �豨���ط���ֵ";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 5	��������
void WaterCmdFunc_WorkingParams(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<��������";
	menuList.no = 8;
	menuList.MaxNum = 8;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "1. �����������ϵ��";
	menuList.str[1] = "2. �����ת��������";
	menuList.str[2] = "3. ��ȡ����ʹ��״̬";
	menuList.str[3] = "4. ���ö�ʱ�ϴ�";
	menuList.str[4] = "5. ���ö����ϴ�";
	menuList.str[5] = "6. ���ö�ʱ�����ϴ�";
	menuList.str[6] = "7. ������ʱ��";
	menuList.str[7] = "8. У����ʱ��";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
			if (key == KEY_CANCEL){
				break;
			}else{
				continue;
			}
		}
		
	}

	_CloseCom();
}

// 6	��������
void WaterCmdFunc_Other(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	_GuiLisStruEx menuList;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 * pChar;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_ClearScreen();

	// �˵�
	menuList.title = "<<��������";
	menuList.no = 5;
	menuList.MaxNum = 5;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "1. ����/��/���Ŵ���";
	menuList.str[1] = "2. ��ȡRXD��TXD�ŵ�";
	menuList.str[2] = "3. ����RXD��TXD�ŵ�";
	menuList.str[3] = "4. ������Ӫ�̱��";
	menuList.str[4] = "5. ·���·�";
	menuList.defbar = 1;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){

		_ClearScreen();
		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0){
			break;
		}
		menuList.defbar = menuItemNo;

		while(1){
			
			_ClearScreen();

			// �������� :  ������ʾ
			pChar = menuList.str[menuItemNo - 1];
			sprintf(TmpBuf, "<< %s",&pChar[5]);
			_Printfxy(0, 0, TmpBuf, Color_White);
			_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
			/*---------------------------------------------*/

			for(i = 0; i < RELAY_MAX; i++){
				StrRelayAddr[i][0] = 0x00;
			}
			sprintf(StrRelayAddr[0], "  �о����� ");
			(*pUiCnt) = 0;
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
			TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

			// �����������
			CurrCmd = (0x10 + menuItemNo);
			switch(CurrCmd){
			case WaterCmd_ReadRealTimeData:		// "��ȡ�û�����"
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ReadFrozenData:		// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_OpenValve:			// "  "
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;
			
			case WaterCmd_OpenValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

            case WaterCmd_CloseValve:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_CloseValveForce:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			case WaterCmd_ClearException:		// "  ";
				/*---------------------------------------------*/
				if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
					break;
				}
				Args.itemCnt = 2;
				Args.items[0] = &Args.buf[0];   //������
				Args.items[1] = &Args.buf[1];
                *Args.items[0] = 0x01;
				*Args.items[1] = 0x00;
				Args.lastItemLen = 1;
				break;

			default: 
				break;
			}


			if (key == KEY_CANCEL){
				break;
			}

			if(StrDstAddr[0] == 0x00 ){
				sprintf(StrDstAddr, "�����������");
				continue;
			}

			// 6009 Э���ַ��д���÷���
			GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
			PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

			// ����ַ
			Addrs.itemCnt = 0;
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
			memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
			Addrs.itemCnt++;
			for(i = 0; i < RELAY_MAX; i++){
				if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
					Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
					GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
					Addrs.itemCnt++;
				}
			}
			Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
			memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
			Addrs.itemCnt++;

			// Ӧ�𳤶ȡ���ʱʱ�䡢�ط�����
			ackLen += 14 + Addrs.itemCnt * 6;
			timeout = 8000 + (Addrs.itemCnt - 2) * 6000 * 2;
			tryCnt = 3;

			// ���͡����ա������ʾ
			key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
			
			
			// ���� / ����
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
	menu.title= "<<���˲��� ";
	menu.str[0]=" �������� ";
	menu.str[1]=" �������� ";
	menu.str[2]=" �������� ";
	menu.str[3]=" Ԥ������ ";
	menu.str[4]=" �������� ";
	menu.str[5]=" �������� ";
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

//-----------------------------------	���˵�	---------------------------
// ��ȡ�û�����
void MainFuncReadRealTimeData(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		_Printfxy(0, 0, "<<��ȡ�û�����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		CurrCmd = WaterCmd_ReadRealTimeData;	// "��ȡ�û�����"

		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x01;		// ������	01
		ackLen = 21;				// Ӧ�𳤶� 21	
		// ������
		Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "�����������");
			continue;
		}

		// 6009 Э���ַ��д���÷���
		GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

		// ����ַ
		Addrs.itemCnt = 0;
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
		memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
		Addrs.itemCnt++;
		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
				Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
				GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
				Addrs.itemCnt++;
			}
		}
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
		memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
		Addrs.itemCnt++;

		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 6500 + (Addrs.itemCnt - 2) * 6000 * 2;

		// ���͡����ա������ʾ
		key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// ��ȡ��������
void MainFuncReadFrozenData(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		_Printfxy(0, 0, "<<��ȡ������ת����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		CurrCmd = WaterCmd_ReadFrozenData;		// "��ȡ������ת����"
		/*---------------------------------------------*/
		sprintf(TmpBuf, "0");
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 6*16, "�������:", &TmpBuf[0], 1, 2*8);
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		if(TmpBuf[0] > '9' || TmpBuf[0] < '0'){
			PrintfXyMultiLine_VaList(0, 6*16, "�������: <0~9��Ч>");
			continue;
		}
		Args.buf[i++] = 0x02;		// ������	02
		ackLen = 88;				// Ӧ�𳤶� 88	
		// ������
		Args.buf[i++] = 0x02;				// ���ݸ�ʽ 01/02
		Args.buf[i++] = _GetYear()/100;		// ʱ�� - yyyy/mm/dd HH:mm:ss
		Args.buf[i++] = _GetYear()%100;		
		Args.buf[i++] = _GetMonth();		
		Args.buf[i++] = _GetDay();			
		Args.buf[i++] = _GetHour();			
		Args.buf[i++] = _GetMin();			
		Args.buf[i++] = _GetSec();			
		Args.buf[i++] = TmpBuf[0] - '0';	// �����������	
		Args.lastItemLen = i - 1;
				
		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "�����������");
			continue;
		}

		// 6009 Э���ַ��д���÷���
		GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

		// ����ַ
		Addrs.itemCnt = 0;
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
		memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
		Addrs.itemCnt++;
		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
				Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
				GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
				Addrs.itemCnt++;
			}
		}
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
		memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
		Addrs.itemCnt++;

		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 6500 + (Addrs.itemCnt - 2) * 6000 * 2;

		// ���͡����ա������ʾ
		key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// ��ȡ����ʱ��
void MainFuncReadMeterTime(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		_Printfxy(0, 0, "<<��ȡ�û�����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		CurrCmd = 0x11;

		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x01;		// ������	01
		ackLen = 21;				// Ӧ�𳤶� 21	
		// ������
		Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "�����������");
			continue;
		}

		// 6009 Э���ַ��д���÷���
		GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

		// ����ַ
		Addrs.itemCnt = 0;
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
		memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
		Addrs.itemCnt++;
		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
				Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
				GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
				Addrs.itemCnt++;
			}
		}
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
		memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
		Addrs.itemCnt++;

		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 6500 + (Addrs.itemCnt - 2) * 6000 * 2;

		// ���͡����ա������ʾ
		key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// ���ñ���ʱ��
void MainFuncSetMeterTime(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		_Printfxy(0, 0, "<<��ȡ�û�����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		CurrCmd = 0x11;

		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x01;		// ������	01
		ackLen = 21;				// Ӧ�𳤶� 21	
		// ������
		Args.buf[i++] = 0x00;				// ���ݸ�ʽ 00	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "�����������");
			continue;
		}

		// 6009 Э���ַ��д���÷���
		GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

		// ����ַ
		Addrs.itemCnt = 0;
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
		memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
		Addrs.itemCnt++;
		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
				Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
				GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
				Addrs.itemCnt++;
			}
		}
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
		memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
		Addrs.itemCnt++;

		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 6500 + (Addrs.itemCnt - 2) * 6000 * 2;

		// ���͡����ա������ʾ
		key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// ���쳣����
void MainFuncClearException(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		_Printfxy(0, 0, "<<���쳣����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		CurrCmd = WaterCmd_ClearException;		// " ���쳣���� ";
		/*---------------------------------------------*/
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x05;		// ������	05
		ackLen = 1;					// Ӧ�𳤶� 1	
		// ������
		Args.buf[i++] = 0x00;		// ����ѡ�� 00	
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "�����������");
			continue;
		}

		// 6009 Э���ַ��д���÷���
		GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

		// ����ַ
		Addrs.itemCnt = 0;
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
		memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
		Addrs.itemCnt++;
		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
				Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
				GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
				Addrs.itemCnt++;
			}
		}
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
		memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
		Addrs.itemCnt++;

		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 6500 + (Addrs.itemCnt - 2) * 6000 * 2;

		// ���͡����ա������ʾ
		key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// ����
void MainFuncOpenValve(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		_Printfxy(0, 0, "<<����", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		CurrCmd = WaterCmd_OpenValve;			// " ���� "
		/*---------------------------------------------*/
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x03;		// ������	03
		ackLen = 3;					// Ӧ�𳤶� 3	
		// ������
		Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
		Args.buf[i++] = 0x01;		// ���ط���ʶ	0 - �ط��� 1 - ����
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "�����������");
			continue;
		}

		// 6009 Э���ַ��д���÷���
		GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

		// ����ַ
		Addrs.itemCnt = 0;
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
		memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
		Addrs.itemCnt++;
		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
				Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
				GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
				Addrs.itemCnt++;
			}
		}
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
		memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
		Addrs.itemCnt++;

		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 6500 + (Addrs.itemCnt - 2) * 6000 * 2;

		// ���͡����ա������ʾ
		key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// �ط�
void MainFuncCloseValve(void)
{
	uint8 key, tryCnt = 0, i;
	UI_Item * pUiItems = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUiItem = 0;
	uint16 ackLen, timeout;

	_CloseCom();
	_ComSetTran(CurrPort);
	_ComSet(CurrBaud, 2);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		_Printfxy(0, 0, "<<�ط�", Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "״̬: ����    ", Color_White);

		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] > '9' || StrRelayAddr[i][0] < '0'){
				StrRelayAddr[i][0] = 0x00;
				sprintf(StrRelayAddr[i], " <��ѡ> ");
			}
		}

		(*pUiCnt) = 0;
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 2*16, "�� ��:", StrDstAddr, 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 3*16, "�м�1:", StrRelayAddr[0], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 4*16, "�м�2:", StrRelayAddr[1], 12, 13*8);
		TextBoxCreate(&pUiItems[(*pUiCnt)++], 0, 5*16, "�м�3:", StrRelayAddr[2], 12, 13*8);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		CurrCmd = WaterCmd_CloseValve;		// " �ط� ";
		/*---------------------------------------------*/
		if(KEY_CANCEL == (key = ShowUI(UiList, &currUiItem))){
			break;
		}
		Args.buf[i++] = 0x03;		// ������	03
		ackLen = 3;					// Ӧ�𳤶� 3	
		// ������
		Args.buf[i++] = 0x00;		// ǿ�Ʊ�ʶ 	0 - ��ǿ�ƣ� 1 - ǿ��
		Args.buf[i++] = 0x00;		// ���ط���ʶ	0 - �ط��� 1 - ����
		Args.lastItemLen = i - 1;

		if (key == KEY_CANCEL){
			break;
		}

		if(StrDstAddr[0] == 0x00 ){
			sprintf(StrDstAddr, "�����������");
			continue;
		}

		// 6009 Э���ַ��д���÷���
		GetBytesFromStringHex(DstAddr, 0, 6, StrDstAddr, 0, false);
		PrintfXyMultiLine_VaList(0, 2*16, "�� ��: %s", StrDstAddr);

		// ����ַ
		Addrs.itemCnt = 0;
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[0];
		memcpy(Addrs.items[Addrs.itemCnt], LocalAddr, 6);
		Addrs.itemCnt++;
		for(i = 0; i < RELAY_MAX; i++){
			if(StrRelayAddr[i][0] >= '0' && StrRelayAddr[i][0] <= '9'){
				Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
				GetBytesFromStringHex(Addrs.items[Addrs.itemCnt], 0, 6, StrRelayAddr[i], 0, false);
				Addrs.itemCnt++;
			}
		}
		Addrs.items[Addrs.itemCnt] = &Addrs.buf[6 + i*6];
		memcpy(Addrs.items[Addrs.itemCnt], DstAddr, 6);
		Addrs.itemCnt++;

		ackLen += 14 + Addrs.itemCnt * 6;
		timeout = 6500 + (Addrs.itemCnt - 2) * 6000 * 2;

		// ���͡����ա������ʾ
		key = Protol6009Tranceiver(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}

	_CloseCom();
}

// ��������
void MainFuncBatchMeterReading(void)
{

}

// ���̵���		------------------------------
void MainFuncEngineerDebuging(void)
{
	_GuiMenuStru menu;

	menu.top = 0;
	menu.left = 0;
	menu.no = 3;
	menu.title = "<<���̵���";
	menu.str[0] = "���˲���";
	menu.str[1] = "����������";
	menu.str[2] = "�汾��Ϣ";
	menu.key[0] = "1";
	menu.key[1] = "2";
	menu.key[2] = "3";
	menu.Function[0] = WaterCmdFunc;
	menu.Function[1] = CenterCmdFunc;
	menu.Function[2] = CenterCmdFunc;
	menu.FunctionEx = 0;
	_Menu(&menu);
}

// --------------------------------   ������   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	int fp;

	fp = _Fopen("system.cfg", "R");
	_Fread(StrDstAddr, TXTBUF_LEN, fp);
	_Fclose(fp);
	
	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=8;
	MainMenu.title = "   ɣ���ֳֻ�  ";
	MainMenu.str[0] = " ��ȡ�û����� ";
	MainMenu.str[1] = " ��ȡ�������� ";
	MainMenu.str[2] = " ��ȡ����ʱ�� ";
	MainMenu.str[3] = " ���ñ���ʱ�� ";
	MainMenu.str[4] = " ���쳣���� ";
	MainMenu.str[5] = " ���� ";
	MainMenu.str[6] = " �ط� ";
	//MainMenu.str[7] = " �������� ";
	MainMenu.str[7] = " ���̵��� ";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.key[7] = "8";
	//MainMenu.key[8] = "9";
	MainMenu.Function[0] = MainFuncReadRealTimeData;
	MainMenu.Function[1] = MainFuncReadFrozenData;
	MainMenu.Function[2] = MainFuncReadMeterTime;
	MainMenu.Function[3] = MainFuncSetMeterTime;
	MainMenu.Function[4] = MainFuncClearException;
	MainMenu.Function[5] = MainFuncOpenValve;
	MainMenu.Function[6] = MainFuncCloseValve;
	//MainMenu.Function[7] = MainFuncBatchMeterReading;
	MainMenu.Function[7] = MainFuncEngineerDebuging;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	
}
