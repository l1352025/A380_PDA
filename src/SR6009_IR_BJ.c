/*
*
*	ɣ��6009ˮ���ֳֻ� - ����� <����NB-Iot>
*
*/
#include "SR6009_IR_BJ.h"

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

// ��ȡ����ˮ�����
void MainFuncReadBeijingWaterMeterParams(void)
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

// ���Զ�������
void MainFuncReadCustomData(void)
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

// ��Ϊ��ģ������
void MainFuncReadWeiFengModuleDatas(void)
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

// ���ñ��ַ
void MainFuncSetMeterAddr(void)
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

// ���ó�ֵ
void MainFuncSetMeterInitValue(void)
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

// �����ϱ�����
void MainFuncSetReportCycle(void)
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
	MainMenu.str[0] = "��ȡIMEI+CCID";
	MainMenu.str[1] = "��ȡ����ˮ�����";
	MainMenu.str[2] = "���Զ�������";
	MainMenu.str[3] = "���ñ��ַ";
	MainMenu.str[4] = "���ó�ֵ";
    MainMenu.str[5] = "�����ϱ�����";
	MainMenu.str[6] = "��Ϊ��ģ������";
	MainMenu.str[7] = "�汾��Ϣ";  // ��չʱ�滻Ϊ "���̵���";
	MainMenu.key[0] = "1";
	MainMenu.key[1] = "2";
	MainMenu.key[2] = "3";
	MainMenu.key[3] = "4";
	MainMenu.key[4] = "5";
	MainMenu.key[5] = "6";
	MainMenu.key[6] = "7";
	MainMenu.key[7] = "8";
	MainMenu.Function[0] = MainFuncReadImeiAndCcid;
	MainMenu.Function[1] = MainFuncReadBeijingWaterMeterParams;
	MainMenu.Function[2] = MainFuncReadCustomData;
	MainMenu.Function[3] = MainFuncSetMeterAddr;
	MainMenu.Function[4] = MainFuncSetMeterInitValue;
    MainMenu.Function[5] = MainFuncSetReportCycle;
	MainMenu.Function[6] = MainFuncReadWeiFengModuleDatas;
	MainMenu.Function[7] = VersionInfoFunc;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();
}

