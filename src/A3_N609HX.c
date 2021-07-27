/*
*
*	ɣ��6009ˮ���ֳֻ� - ����� <����NB-Iot>
*
*/
#ifndef Protocol_NBiot_BJ
#include "A3_N609HX.h"
#endif

#include "HJLIB.H"
#include "string.h"
#include "stdio.h"

#include "common.h"
#include "common.c"
#include "WaterMeter.h"
#include "ProtoHandle_NBiot_BJ.h"
#include "ProtoHandle_DH_IR.h"


// --------------------------------  ˮ��ģ��ͨ��  -----------------------------------------

//-----------------------------------	���˵�	---------------------------
// ��IMEI+ICCID
void MainFuncReadImeiAndCcid(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
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
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = WaterCmd_ReadImeiAndCcid;		// ��ȡIMEI+ICCID
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
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<��ȡ����ˮ�����");
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
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
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

// ��ȡ�û����� rename --> ���Զ�������
void MainFuncReadRealTimeData(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<���Զ�������");
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
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:	
			CurrCmd = WaterCmd_ReadRealTimeData;	// ���Զ������� ����ԭ���� ����ȡ�û���������
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

// NB�ϱ�ʵʱ����
void MainFuncNbReportRealTimeData(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<NB�ϱ�ʵʱ����");
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
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:	
			CurrCmd = WaterCmd_NbReportRealTimeDataNow;		// NB�ϱ�ʵʱ����
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
	uint8 key, menuItemNo, tryCnt = 0, i;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<���Զ�������");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "����            ִ��", Color_White);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_ReadCustomData;	// ���Զ�������
			/*---------------------------------------------*/
			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// ������
			ackLen = 17;						// Ӧ�𳤶�	
			// ������
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-����͸��
			Args.buf[i++] = 2;					// len: 1 + 1 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0x00;				// dataId: 0-���Զ�������
			Args.buf[i++] = 0x00;	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}
				
		// ���͡����ա������ʾ
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}
}

// ��Ϊ��ģ������
void MainFuncReadWeiFengModuleDatas(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<��Ϊ��ģ������");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "����            ִ��", Color_White);

		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_ReadWeiFengModuleData;		// ��ȡΪ��ģ������
			/*---------------------------------------------*/
			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// ������
			ackLen = 23;						// Ӧ�𳤶�	
			// ������
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-����͸��
			Args.buf[i++] = 2;					// len: 1 + 1 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 82;				// dataId: 82- 0x52 ��ȡΪ��ģ������
			Args.buf[i++] = 0x80;	
			Args.lastItemLen = i - 1;
			break;

		default: 
			break;
		}
				
		// ���͡����ա������ʾ
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
		// ���� / ����
		if (key == KEY_CANCEL){
			break;
		}else{
			continue;
		}
	}
}

// ���ñ��ַ
void MainFuncSetMeterAddr(void)
{
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<���ñ��ַ");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
			LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�� ��:");
			TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "   ", StrBuf[0], 7*2, (7*2*8 + 8), true);	
		}
			
		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		menuItemNo = 1;
		

		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_SetMeterAddr;		// ���ñ��ַ
			/*---------------------------------------------*/
			if(false == isUiFinish){
				break;
			}
			
			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// ������	
			ackLen = 81;						// Ӧ�𳤶� 	
			// ������
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-����͸��
			Args.buf[i++] = 8;					// len: 1 + 7 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 53;					// dataId: 53-0x35 ����ַ

			GetBytesFromStringHex(&TmpBuf[0], 0, 7, StrBuf[0], 0, true);
			memcpy(&Args.buf[i], &TmpBuf[0], 7);
			i += 7;
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


		// ���͡����ա������ʾ
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
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
	uint8 key, menuItemNo, tryCnt = 0, i;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;
	uint32 u32Tmp;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<���ó�ֵ");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
		}
			
		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:
			CurrCmd = UserCmd_SetInitValue;		// ���ó�ֵ
			/*---------------------------------------------*/
			if(false == isUiFinish){
				if(BackupBuf[ArgIdx_MtrValPalse -1] != Param_Unique){		
					StrBuf[1][0] = 0x01;
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_MtrValPalse], 1 * 20);
				}
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��������:", StrBuf[0], 10, 11*8, true);
				LableCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "��������: Ĭ��Ϊ0");
				break;
			}
			if(StrBuf[0][0] > '9' || StrBuf[0][0] < '0'){
				sprintf(StrBuf[0], " ������");
				currUi = uiRowIdx - 2 - 2;
				isUiFinish = false;
				continue;
			}

			memcpy(&BackupBuf[ArgIdx_MtrValPalse], &StrBuf[0][0], 1 * 20);
			BackupBuf[ArgIdx_MtrValPalse - 1] = Param_Unique;

			u32Tmp = (uint32) _atof(StrBuf[0]);

			i = 0;
			Args.buf[i++] = Func_DataTransmit;	// ������
			ackLen = 81;						// Ӧ�𳤶�	
			// ������
			Args.buf[i++] = Tag_DataTransmit;	// tag: 06-����͸��
			Args.buf[i++] = 9;					// len: 1 + 8 
			Args.buf[i++] = 0;	
			Args.buf[i++] = 52;					// dataId: 52-0x34  ������
			Args.buf[i++] = (uint8)(u32Tmp & 0xFF);			// ��������	
			Args.buf[i++] = (uint8)((u32Tmp >> 8) & 0xFF);
			Args.buf[i++] = (uint8)((u32Tmp >> 16) & 0xFF);
			Args.buf[i++] = (uint8)((u32Tmp >> 24) & 0xFF);
			Args.buf[i++] = 0;								// ��������
			Args.buf[i++] = 0;
			Args.buf[i++] = 0;
			Args.buf[i++] = 0;
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

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ���͡����ա������ʾ
		key = NBiotBj_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
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
	uint8 key, menuItemNo, tryCnt = 0, i, u8Tmp;
	UI_Item * pUi = &UiList.items[0];
	uint8 * pUiCnt = &UiList.cnt;
	uint8 currUi = 0, uiRowIdx, isUiFinish;
	uint16 ackLen = 0, timeout;

	memset(StrBuf, 0, TXTBUF_LEN * TXTBUF_MAX);
	isUiFinish = false;

	while(1){
		
		_ClearScreen();

		// �������� :  ������ʾ
		sprintf(CurrCmdName, "<<�����ϱ�����");
		_Printfxy(0, 0, CurrCmdName, Color_White);
		_GUIHLine(0, 1*16 + 4, 160, Color_Black);	
		/*---------------------------------------------*/
		//----------------------------------------------
		_GUIHLine(0, 9*16 - 4, 160, Color_Black);
		_Printfxy(0, 9*16, "���� <�ȴ�����> ִ��", Color_White);

		if(false == isUiFinish){
			(*pUiCnt) = 0;
			uiRowIdx = 2;
		}
			
		// �����������
		i = 0;	
		Args.itemCnt = 2;
		Args.items[0] = &Args.buf[0];   // ������
		Args.items[1] = &Args.buf[1];	// ������
		menuItemNo = 1;
		
		switch(menuItemNo){
		case 1:	
			CurrCmd = UserCmd_SetPeriodReportFreq;
			/*---------------------------------------------*/
			if(false == isUiFinish){
				if(BackupBuf[ArgIdx_FixTimeVal -1] != Param_Unique){		
				}
				else{
					memcpy(&StrBuf[0][0], &BackupBuf[ArgIdx_FixTimeVal], 1 * 20);
				}
				TextBoxCreate(&pUi[(*pUiCnt)++], 0, (uiRowIdx++)*16, "�ϱ�Ƶ��(h):", StrBuf[0], 2, 3*8, true);
				break;
			}
			if(StrBuf[0][0] > '9' || StrBuf[0][0] < '1'){
				sprintf(StrBuf[0], " ������");
				currUi = uiRowIdx - 2 - 2;
				isUiFinish = false;
				continue;
			}

			memcpy(&BackupBuf[ArgIdx_FixTimeVal], &StrBuf[0][0], 1 * 20);
			BackupBuf[ArgIdx_FixTimeVal - 1] = Param_Unique;

			u8Tmp = (uint8) _atof(StrBuf[0]);

			if(u8Tmp > 24){
				sprintf(StrBuf[0], " ������");
				currUi = uiRowIdx - 2 - 2;
				isUiFinish = false;
				continue;
			}

			i = 0;
			Args.buf[i++] = Func_SetInternalParam;	// ������
			ackLen = 13;						// Ӧ�𳤶�	
			// ������
			Args.buf[i++] = Func_SetInternalParam;	// ������
			Args.buf[i++] = 0;		// ����
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 0;	
			Args.buf[i++] = 1;		// ��������
			Args.buf[i++] = Tag_PeriodReportFreq;	// �������
			Args.buf[i++] = 1;		// ��������
			Args.buf[i++] = u8Tmp;	// ����ֵ			
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

			isUiFinish = true;
			continue;	// go back to get ui args
		}

		// ���͡����ա������ʾ
		key = DonghaiIR_TranceiverWaitUI(CurrCmd, &Addrs, &Args, ackLen, timeout, tryCnt);
		
		
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
	MainMenu.str[7] = "NB�ϱ�ʵʱ����";
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
	MainMenu.Function[2] = MainFuncReadRealTimeData;
	MainMenu.Function[3] = MainFuncSetMeterAddr;
	MainMenu.Function[4] = MainFuncSetMeterInitValue;
    MainMenu.Function[5] = MainFuncSetReportCycle;
	MainMenu.Function[6] = MainFuncReadWeiFengModuleDatas;
	MainMenu.Function[7] = MainFuncNbReportRealTimeData;
	MainMenu.FunctionEx=0;
	_OpenLcdBackLight();
	_Menu(&MainMenu);	

	MeterNoSave(StrDstAddr);
	SysCfgSave();
}

