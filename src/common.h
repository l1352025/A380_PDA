#ifndef COMMON_H
#define COMMON_H

typedef unsigned char  uint8;                
typedef signed   char  int8;                     
typedef unsigned short uint16;                  
typedef signed   short int16;                   
typedef unsigned int   uint32;                 
typedef signed   int   int32;   
typedef unsigned char bool;
#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif
#ifndef NULL
#define NULL   0
#endif

// --------------------------------		���Ͷ���	-----------------------------------------
/*  ��������˿ڣ� NO.1 / NO.2 / NO.3
        ����NO.1 (TP_PORT_TXD / TP_PORT_RXD)
        ����NO.2 (TP_COM_TXD / TP_COM_RXD)
        ����NO.3 (TP_SCAN_TXD / TP_SCAN_RXD) 
    �����߼��˿ڣ�1.���� / 2.��ͨ���� / 3.���ٺ���
        _ComSetTran(logicPort)
        ��ʹ��ǰ��������Ч������˿� NO.1 / NO.2 / No.3��
        ��ʹ�á����ٺ��⡰,����ѡ������˿�NO.1 �� No.3�� 
            ϵͳ����->ģ������->��������->���ٺ���->
*/
#define Trans_Scom          1   // ɨ��˿�
#define Trans_IR            2   // ��ͨ���� 
#define Trans_IR_Quick      3   // ���ٺ���

// ����Ŀ��ֶ��壺�汾��Ϣ����Դ����
#if defined(Project_6009_IR)        // 6009-����-ͨ�ð�
    #define VerInfo_Name    (char *)"  ɣ��N609A�������"  // ������
    #define VerInfo_RevNo   (char *)"3.0 Ԥ��1"	        // �汾��
    #define VerInfo_RevDate (char *)"2020-12-14"        // �汾����
    #define TransType   (char *)"����͸��"       		// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR    
    #define CurrBaud    (uint8 *)"9600,E,8,1"       // N609A ͨ��    
	//#define CurrBaud    (uint8 *)"2400,N,8,1"    
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     8           // ��ַ����(byte)��8    // N609A ͨ�� 16λ��Ű�
    //#define AddrLen     7           // ��ַ����(byte)��7 
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define UseBroadAddr    1               // ʹ�ù㲥��ַ���� D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_IR_DH)        // 6009-����-�������ư�
    #define VerInfo_Name    (char *)"  ɣ��N609DH�������"  // ������
    #define VerInfo_RevNo   (char *)"1.1 Ԥ��1"	            // �汾��
    #define VerInfo_RevDate (char *)"2020-10-15"        // �汾����
    #define TransType   (char *)"����͸��"       		// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR              
	#define CurrBaud    (uint8 *)"2400,N,8,1"
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     8           // ��ַ����(byte)��8    // 16λ��Ű�
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define UseBroadAddr    1               // ʹ�ù㲥��ַ���� D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_IR_DB)        // 6009-����-Զ�����ư�
    #define VerInfo_Name    (char *)"  ɣ��N609DB�������"  // ������
    #define VerInfo_RevNo   (char *)"1.1 Ԥ��1"	            // �汾��
    #define VerInfo_RevDate (char *)"2020-10-15"        // �汾����
    #define TransType   (char *)"����͸��"       		// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR              
	#define CurrBaud    (uint8 *)"9600,N,8,1"
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     7           // ��ַ����(byte)��7    // 14λ��Ű�
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define UseBroadAddr    1               // ʹ�ù㲥��ַ���� D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_IR_D)        // 6009-����-ʯ��ׯ���ư�
    #define VerInfo_Name    (char *)"  ɣ��N609D�������"  // ������
    #define VerInfo_RevNo   (char *)"1.1 Ԥ��1"	            // �汾��
    #define VerInfo_RevDate (char *)"2020-11-06"        // �汾����
    #define TransType   (char *)"����͸��"       		// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR              
	#define CurrBaud    (uint8 *)"9600,E,8,1"
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     7           // ��ַ����(byte)��7    // 14λ��Ű�
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define UseBroadAddr    1               // ʹ�ù㲥��ַ���� D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_IR_HX)        // 6009-����-�����ư�
    #define VerInfo_Name    (char *)"  ɣ��N609HX�������"  // ������
    #define VerInfo_RevNo   (char *)"1.1 Ԥ��1"	            // �汾��
    #define VerInfo_RevDate (char *)"2020-09-29"        // �汾����
    #define TransType   (char *)"����͸��"       		// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR              
	#define CurrBaud    (uint8 *)"2400,N,8,1"
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     8           // ��ַ����(byte)��8    // 16λ��Ű�
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define UseBroadAddr    1               // ʹ�ù㲥��ַ���� D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_IR_TA)        // 6009-����-��ˮ���ư�
    #define VerInfo_Name    (char *)"  ɣ��N609TA�������"  // ������
    #define VerInfo_RevNo   (char *)"1.0"	            // �汾��
    #define VerInfo_RevDate (char *)"2020-09-29"        // �汾����
    #define TransType   (char *)"����͸��"       		// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR              
	#define CurrBaud    (uint8 *)"9600,E,8,1"
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     8           // ��ַ����(byte)��8    // 16λ��Ű�
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define UseBroadAddr    1               // ʹ�ù㲥��ַ���� D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_IR_F)        // 6009-����-��ˮ���ư�
    #define VerInfo_Name    (char *)"  ɣ��N609F�������"  // ������
    #define VerInfo_RevNo   (char *)"1.0"	            // �汾��
    #define VerInfo_RevDate (char *)"2021-06-16"        // �汾����
    #define TransType   (char *)"����͸��"       		// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR              
	#define CurrBaud    (uint8 *)"9600,E,8,1"
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     8           // ��ַ����(byte)��8    // 16λ��Ű�
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define UseBroadAddr    1               // ʹ�ù㲥��ַ���� D4D4D4D4D4D4D4D4 
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_RF)      // 6009-Lora
    #define VerInfo_Name    (char *)"  ɣ��6009�������"   // ������
    #define VerInfo_RevNo   (char *)"3.4"               // �汾��
    #define VerInfo_RevDate (char *)"2021-12-21"        // �汾����
    #define TransType   (char *)"LoRa͸��"              // ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     6           // ��ַ����(byte)��6 
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�0
    #define LogPort     CurrPort            // ��־�������
    #define CenterCmd_Enable        0       // �����������ʹ�ã�Ŀǰ������
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
    #define UseFunc_BatchOpenCloseValve     // ����ʱ�����õĹ��ܣ��������ط�
#elif defined(Project_6009_RF_HL)   // 6009-Lora-�������ư�
    #define VerInfo_Name    (char *)"  ɣ��6009HL�������"    // ������
    #define VerInfo_RevNo   (char *)"1.0"            // �汾��
    #define VerInfo_RevDate (char *)"2021-09-18"        // �汾����
    #define TransType   (char *)"LoRa͸��"              // ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define VerInfo_Msg    (char *)"    <��������>    "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     6           // ��ַ����(byte)��6 
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�0
    #define LogPort     CurrPort            // ��־�������
    #define CenterCmd_Enable        0       // �����������ʹ�ã�Ŀǰ������
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_6009_RF_TN)   // 6009-Lora-��϶��ư�
    #define VerInfo_Name    (char *)"  ɣ��6009TN�������"    // ������
    #define VerInfo_RevNo   (char *)"1.4"            // �汾��
    #define VerInfo_RevDate (char *)"2021-07-27"        // �汾����
    #define TransType   (char *)"LoRa͸��"              // ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define VerInfo_Msg    (char *)"    <��϶���>    "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     6           // ��ַ����(byte)��6 
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�0
    #define LogPort     CurrPort            // ��־�������
    #define CenterCmd_Enable        0       // �����������ʹ�ã�Ŀǰ������
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
    #define UseFunc_ReSetDistrictAndBuildingNo  // ����ʱ�����õĹ��ܣ���"JK.DBF"�ļ���С����¥�����±��
#elif defined(Project_8009_RF)      // 8009-RF
    #define VerInfo_Name    (char *)"  ɣ��8009�������"   // ������
    #define VerInfo_RevNo   (char *)"1.6"               // �汾��
    #define VerInfo_RevDate (char *)"2021-07-27"        // �汾����
    #define TransType   (char *)"FSK͸��"              	// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define VerInfo_Msg    (char *)"        "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     5           // ��ַ����(byte)��5 
    #define VerLen      24          // �汾����(byte)��24 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define CenterCmd_Enable        0       // �����������ʹ�ã�Ŀǰ������
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf (Ĭ��Ϊ0)
    #define UseFunc_BatchOpenCloseValve     // ����ʱ�����õĹ��ܣ��������ط�
#elif defined(Project_8009_RF_HL)   // 8009-RF-�������ư�
    #define VerInfo_Name    (char *)"  ɣ��8009HL�������"   // ������
    #define VerInfo_RevNo   (char *)"1.1"               // �汾��
    #define VerInfo_RevDate (char *)"2020-05-19"        // �汾����
    #define TransType   (char *)"FSK͸��"               // ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define VerInfo_Msg    (char *)"    <��������>    "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     5           // ��ַ����(byte)��6 
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�0
    #define LogPort     CurrPort            // ��־�������
    #define CenterCmd_Enable        0       // �����������ʹ�ã�Ŀǰ������
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
#elif defined(Project_8009_RF_TN)   // 8009-RF-��϶��ư�
    #define VerInfo_Name    (char *)"  ɣ��8009TN�������"   // ������
    #define VerInfo_RevNo   (char *)"1.4"               // �汾��
    #define VerInfo_RevDate (char *)"2021-07-27"        // �汾����
    #define TransType   (char *)"FSK͸��"               // ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define VerInfo_Msg    (char *)"    <��϶���>    "         // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     5           // ��ַ����(byte)��6 
    #define VerLen      40          // �汾����(byte)��40 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�0
    #define LogPort     CurrPort            // ��־�������
    #define CenterCmd_Enable        0       // �����������ʹ�ã�Ŀǰ������
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf
    #define UseFunc_ReSetDistrictAndBuildingNo  // ����ʱ�����õĹ��ܣ���"JK.DBF"�ļ���С����¥�����±��
#else // defined(Project_8009_RF_PY)   // 8009-RF-������ư�
    #define VerInfo_Name    (char *)"  ɣ��8009PY�������"   // ������
    #define VerInfo_RevNo   (char *)"1.0"               // �汾��
    #define VerInfo_RevDate (char *)"2020-09-18"        // �汾����
    #define TransType   (char *)"FSK͸��"              	// ͨ�ŷ�ʽ	
	#define CurrPort    Trans_IR_Quick          
	#define CurrBaud    (uint8 *)"9600,E,8,1" 
    #define VerInfo_Msg    (char *)"    <�������>    "     // �汾��ע��Ϣ
    #define UseCrc16    0           // �Ƿ�ʹ��Crc16У���㷨��0 - crc8У�飬 1 - crc16У��
    #define AddrLen     5           // ��ַ����(byte)��5 
    #define VerLen      24          // �汾����(byte)��24 
    #define ShowEMI_ON  1           // ��ʾ�Ÿ���״̬���أ�1
    #define LogPort     CurrPort            // ��־�������
    #define CenterCmd_Enable        0       // �����������ʹ�ã�Ŀǰ������
    #define Upgrd_FileBuf_Enable    0       // ʹ�ô��ļ����棺����App�ļ������ڴ滺�� *FileBuf (Ĭ��Ϊ0)
#endif


#define VerInfo_Release     // ����ʱ���붨��ú꣬ ����ʱע��

#ifndef VerInfo_Release
    //#warning ��ǰLOGδ�رգ�����ʱ��ر�
    //#pragma warning ��ǰLOGδ�رգ�����ʱ��ر� 
    //#pragma message( " Warning: ��ǰLOGδ�رգ�����ʱ��ر�  " )
    //#error ��ǰLOGδ�رգ�����ʱ��ر�
#endif

// ����ʱ���ر�log �� �򿪷�����
#ifndef VerInfo_Release
#define LOG_ON      1           // ������־����
#define LogFileName "debug.txt" // ��־�ļ���
#define LogFileSize (100*1024)  // ��־�ļ�����ֽ���
#define LogScom_On  0           // ��־���ڿ��أ�1- ��������ڣ�0 -������ļ� (�����־���ں�ͨ�Ŵ��ڲ���ͬʱʹ��ʱ������ر�)
#define LogTxRx     1           // ��־���Tx/Rx���ݣ�0 - ������� 1 - ���
#define RxBeep_On   0           // ���ڽ������-������ʾ���أ� ��һ��- �����ɹ��� ������ - ����ʧ��
#else
#define LOG_ON      0 
#define RxBeep_On   1       
#endif

#define RELAY_MAX       3       // ����м̸���: 3
#define TXTBUF_MAX      20      // �ı����뻺����������
#define TXTBUF_LEN	    20      // �ı����뻺��������ַ���
#define UI_MAX          20      // ���UI�ؼ���
#define ListItem_Max    40      // �б����������� ������ʾ�˵��б�
#define CbxItem_Max     15      // ��ѡ�������

#define Invalid_dbIdx	0	    // ��Ч���ݿ��¼����, ���ݿ��¼��Ŵ�1��ʼ
#define	Size_ListStr    21		// �б��ַ�����󳤶�
#define	Size_DbStr      256		// ���ݿ��ֶ���󳤶�

typedef enum{
    Color_White = 0,
    Color_Black = 1
}GUI_COLOR;

typedef struct{
	uint8 buf[200];
	uint8 *items[10];
	uint8 itemCnt;
    uint8 lastItemLen;
}ParamsBuf;

typedef enum{
    UI_TxtBox,
    UI_CombBox,
    UI_Lable,
    UI_Button
}UI_Type;

typedef struct{
    
    uint8 x;        // UI title position
    uint8 y;
    char *title;
    uint8 x1;       // UI text position
    uint8 y1;
    char *text;
    uint8 width;    // UI width
    uint8 height;   // UI high
    UI_Type type;
    
    union {
        struct{
            char *strs[CbxItem_Max];
            uint8 cnt;
            uint8 *currIdx;
        }combox;

        struct{
            uint8 dataLen;
            uint8 isClear;
            uint8 dotEnable;
            uint8 isInputAny;
        }txtbox;
    }ui;
}UI_Item;

typedef struct{
    UI_Item items[UI_MAX];
    uint8 cnt;
}UI_ItemList;

typedef void (*FillListFunc)(char **strs, int16 dstIdx, int16 srcIdx, uint16 cnt);

typedef struct{
    uint8 x;        
    uint8 y;
    uint8 maxRow;       // һҳ�����ʾ���� �� ���8��
    uint8 maxCol;       // һ�������ʾ���������20��Ӣ���ַ�
    uint8 isCircle;     // ��ѭ���б��ʶ : Ĭ�Ͽ�ѭ��
    uint16 dispStartIdx; // ��ǰҳ��ʾ�ĵ�һ����¼����ʾ��������λ�� 
    uint16 totalCnt;    // ���ݿ��¼����
    int16 currIdx;      // ���ݿ⵱ǰ��¼��λ��
    char *title;
    char *str[ListItem_Max];    // �ַ����б�
    uint16 strCnt;              // �ַ����б��м�¼����
    int16 strIdx;               // �ַ����б��е�ǰλ��        ȷ�ϼ�/ȡ�����˳��б�ʱ��λ��  
    FillListFunc fillStrsFunc;      // ��ҳʱ�ص�����

}ListBox;

typedef struct{
    uint8 x;        
    uint8 y;
    uint8 maxRow;       // һҳ�����ʾ���� �� ���8��
    uint8 maxCol;       // һ�������ʾ���������20��Ӣ���ַ�
    uint8 isCircle;     // ��ѭ���б��ʶ : Ĭ�Ͽ�ѭ��
    uint16 dispStartIdx; // ��ǰҳ��ʾ�ĵ�һ����¼����ʾ��������λ�� 
    uint16 totalCnt;    // ���ݿ��¼����
    int16 currIdx;      // ���ݿ⵱ǰ��¼��λ��
    char *title;
    char **str;             // �ַ����б�
    uint16 strLen;          // �����ַ�������
    uint16 strCnt;          // �ַ����б��м�¼����
    int16 strIdx;           // �ַ����б��е�ǰλ��        ȷ�ϼ�/ȡ�����˳��б�ʱ��λ��  
    FillListFunc fillStrsFunc;      // ��ҳʱ�ص�����

}ListBoxEx;

typedef enum{
	CmdResult_Ok,
	CmdResult_Failed,
	CmdResult_CrcError,
	CmdResult_Timeout,
    CmdResult_Cancel
}CmdResult;

typedef	enum{
    Cmd_Send,
    Cmd_RecvOk,
    Cmd_RecvNg,
    Cmd_Finish,
    Cmd_Exit
}CmdState;

typedef void (*FuncCmdCycleHandler)(uint8 currKey);
typedef uint8 (*FuncCmdFramePack)(uint8 *txBuf, ParamsBuf *addrs, uint16 cmdid, ParamsBuf *args, uint8 sendCnt);
typedef uint8 (*FuncCmdFrameExplain)(uint8 *rxBuf, uint16 rxlen, const uint8 *dstAddr, uint16 cmdId, uint16 ackLen, char *dispBuf);

//---------------------------------		��������	 -----------------------------------------
int IndexOf(const uint8 * srcArray, int srcLen, const uint8 * dstBytes, int dstLen, int startIndex, int offset);
uint16 GetCrc16(uint8 *Buf, uint32 Len, uint16 Seed);
uint16 GetCrc16_Continue(uint8 *Buf, uint32 Len, uint16 Seed, uint16 *CrcKeep);
uint8 GetCrc8(uint8 *Buf, uint32 len);
uint8 GetSum8(uint8 *buf, uint32 len);
char HexToChar(uint8 b);
uint8 CharToHex(char c);
uint8 BcdToDec(uint8 bcd);
uint8 DecToBcd(uint8 dec);
uint16 GetUint16(uint8 *buf, uint8 len, bool reverse);
uint32 GetUint32(uint8 *buf, uint8 len, bool reverse);
int GetTimeStr(uint8 *time, const char *format, uint8 *buf, uint8 len);
int GetStringHexFromBytes(char * strHex, uint8 bytes[], int iStart, int iLength, char separate, bool reverse);
int GetBytesFromStringHex(uint8 bytes[], int iStart, int iLength, const char * strHex, char separate, bool reverse);
void StringPadLeft(const char * srcStr, int totalLen, char padChar);
void StringPadRight(const char * srcStr, int totalLen, char padChar);
bool StringToInt(const char *str, int *intValue);
bool StringToDecimal(const char *doubleStr, uint8 decCnt, bool *isNegative, uint32 *decimalInt, uint16 *decimalDec);
void DoubleStrSetFracCnt(const char * doubleStr, uint8 fractionCnt);
int StringTrimStart(const char * srcStr, char trimChar);
int StringTrimEnd(const char * srcStr, char trimChar);
int StringCopyFromTail(char * dstStr, const char * srcStr, uint8 len);
void StringFixGbkStrEndError(const char * srcStr);
void ShowProgressBar(uint8 y, uint32 maxValue, uint32 currValue);
void LableCreate(UI_Item *item, uint8 x, uint8 y, const char * title);
void TextBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, char * text, uint8 maxLen, uint8 width, bool isClear);
void CombBoxCreate(UI_Item *item, uint8 x, uint8 y, const char * title, uint8 * currIdx, uint32 maxCnt, ...);
uint8 ShowUI(UI_ItemList inputList, uint8 *itemNo);
void ListBoxCreate(ListBox *lbx, uint8 x, uint8 y, uint8 maxCol, uint8 maxRow, uint16 totalCnt, FillListFunc fillStrsFunc, const char *title, uint32 strCnt, ...);
void ListBoxCreateEx(ListBoxEx *lbx, uint8 x, uint8 y, uint8 maxCol, uint8 maxRow, uint16 totalCnt, FillListFunc fillStrsFunc, const char *title, char **strs, uint8 strLen, uint32 strCnt);
uint8 ShowListBox(ListBox *lbx);
uint8 ShowListBoxEx(ListBoxEx *lbx);
void ShowMsg(uint8 x, uint8 y, char *str, uint16 waitMs);
void PrintfXyMultiLine_VaList(uint8 x, uint8 y, const char * format, ...);
void PrintfXyMultiLine(uint8 x, uint8 y, const char * buf, uint8 maxLines);
void PrintXyTriangle(uint8 x, uint8 y, uint8 direction);
uint8 GetPrintLines(uint8 x, const char * buf, char * lines[]);
uint8 ShowScrollStr(char *strBuf, uint8 lineStep);
uint8 ShowScrollStrEx(char *strBuf, uint8 lineStep);
void LogPrint(const char * format, ...);
void LogPrintBytes(const char *title, uint8 *buf, uint16 size);
void DatetimeToTimeStrs(const char *datetime, char *year, char *month, char *day, char *hour, char *min, char *sec);
uint8 TimeStrsToTimeBytes(uint8 bytes[], char *year, char *month, char *day, char *hour, char *min, char *sec);
uint8 IpStrsToIpBytes(uint8 ip[], char *strIp1, char *strIp2, char *strIp3, char *strIp4);
CmdResult CommandTranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt);
CmdResult ProtolCommandTranceiver(uint8 cmdid, ParamsBuf *addrs, ParamsBuf *args, uint16 ackLen, uint16 timeout, uint8 tryCnt);
void CycleInvoke_OpenLcdLight_WhenKeyPress(uint8 currKey);
void LcdLightCycleCtrl(uint8 *lcdCtrl, uint8 closeCnt);
uint8 CreateRelayAddrsUi(UI_Item *pUi, int8 *pUiCnt, uint8 uiRowIdx);
void MeterNoSave(uint8 *mtrNo);
void MeterNoLoad(uint8 *mtrNo);
void SysCfgLoad(void);
void SysCfgSave(void);
void VersionInfoFunc(void);
#define Memcpy_AddNull(pDst, pSrc, len) do { memcpy(pDst, pSrc, len); ((uint8 *)pDst)[len] = 0x00; }while(0)

//--------------------------------		ȫ�ֱ���	 ---------------------------------------
#if Upgrd_FileBuf_Enable
extern uint8 DispBuf[128 * 1024];					    // 4k ~ 128K
#else
extern uint8 DispBuf[80 * 1024];					    // 4k ~ 80K
#endif
extern uint8 * const LogBuf; // = &DispBuf[4096];     	// 4k ~ 
extern uint8 * const TmpBuf; // = &DispBuf[8192];     	// 2K ~ 
extern uint8 * const BackupBuf; // = &DispBuf[10240];	// 2k ~
extern uint8 * const ArgBuf; // = &DispBuf[12288];     	// 2k ~ 
#if Upgrd_FileBuf_Enable
extern uint8 * const BigBuf; // = &DispBuf[14336];	    // 116k 
#else
extern uint8 * const BigBuf; // = &DispBuf[14336];      // 66K
#endif
extern uint8 TxBuf[1024];
extern uint8 RxBuf[1024];
extern uint32 RxLen, TxLen;
extern const uint8 LocalAddr[10];   // ������ַ 2019000020190000��10/12/16�ַ�
const uint8 BroadAddr[10];          // �㲥��ַ D4D4D4D4D4D4D4D4��10/12/16�ַ�
extern uint8 DstAddr[10];
extern uint8 VerInfo[42];
extern uint16 CurrCmd;
extern char CurrCmdName[64];
extern ParamsBuf Addrs;		
extern ParamsBuf Args;
extern char StrBuf[TXTBUF_MAX][TXTBUF_LEN];    // extend input buffer
extern char StrDstAddr[TXTBUF_LEN];
extern char StrRelayAddr[RELAY_MAX][TXTBUF_LEN];
extern UI_ItemList UiList;
extern bool LcdOpened;
extern bool IsNoAckCmd;
extern FuncCmdCycleHandler TranceiverCycleHook;
extern FuncCmdFramePack FramePack;
extern FuncCmdFrameExplain FrameExplain;

//--------------------------------		��������	 ---------------------------------------
// ���ݻ������Ĳ���λ������ (������/������)
// λ������ǰ1byteΪ������ʶ��0-δ���ã�1-�����ã�������������������-�����ã�������������
typedef enum{
	ArgIdx_Shared		= 4,		// ���������������� (0x11 ����ˮ�����), 31 * 20 byte
	ArgIdx_MtrValPalse	= 700,		// �����������ϵ�� �� 20*2 byte
	ArgIdx_OverCurr		= 744,		// ���������ͳ�ʱʱ��: 20*2 byte
	ArgIdx_IpPortMode	= 788,		// ����ģʽ+IP+Port: 20*6 byte
	ArgIdx_FuncEnSts	= 912,		// ����ʹ��״̬: 20*1 byte
	ArgIdx_ModFreqs		= 936,		// ģ��Ƶ��: 20*3 byte
	ArgIdx_FixTimeVal	= 1000,		// ��ʱ���������20*2 byte
	ArgIdx_RunParas		= 1044		// ģ�����в���: 20*2 byte
}ArgsIndex;

// ������ʶ�� ������λ��������ǰ1�ֽڣ��� BackupBuf[ArgIdx_Shared -1] = 0x11
typedef enum{
	Param_None			= 0x00,		// ����δ����
	Param_Unique		= 0x01,		// �����ö��в���
	Param_BeijingWMtr 	= 0x11		// �����ù������������ˮ�����
	// others
}BackUpParamFlag;

#endif
