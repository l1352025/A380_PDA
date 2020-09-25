#ifndef Upgrade_H
#define Upgrade_H

#include "Common.h"

#define Upgrd_FileMaxSize   (128 * 1024)        // Ŀǰʵ�����116K
#define Upgrd_PacketSize    128                 // ����С
#define Upgrd_PacketCntMax  ( Upgrd_FileMaxSize / Upgrd_PacketSize )
#define Upgrd_MeterMax      256                 // �������� ��󵵰���
#define Upgrd_DocFileName   "UpgradeDoc.csv"    // �������� �����ļ���
#define Upgrd_ReSendPktMax  5                   // 1����󲹰�����

#define Upgrd_LOG_ON            1               // ����ģ����־����

// bin�ļ��� ǰ128byte��ʵ��ʹ��69byte�������������������Ϣ
typedef struct 
{
    char    appVer[40];
    uint8   packetCnt[2];
    uint8   crc16_1st26K[2];
    uint8   crc16_2nd26K[2];
    uint8   crc16_all52K[2];
    uint8   faildReason;
    uint8   meterId[6];
    uint8   devType;
    uint8   rfParam[5];    // RF�շ�Ƶ�� 3byte���ŵ����� 1byte ����Ƶ���� 1byte
    uint8   rxdChanel;
    uint8   waterMeterVbat;
    uint8   gasMeterVbat;
    uint8   snr;
    uint8   rssi;
    uint8   upgradeStatus;
    uint8   crc16_appVer[2];
    uint8   reserved[59];   // ����
}AppFileInfo;

typedef struct
{
    char *  fileName;
    int     filePtr;
    uint32  fileSize;
    uint16  fileKbSize;
    uint16  fileCrc16;
    char *  version;
    uint16  verCrc16;
    uint32  pktStartIdx;
    uint16  packetSize;
    uint16  lastPktSize;
    uint16  packetCnt;      
    uint8   bitFlags[Upgrd_PacketCntMax / 8];   
    uint16  bitFlagsCnt;                        // actual bitflag byte cnt 
    uint16  missPkts[Upgrd_PacketCntMax];  
    uint16  missPktsCnt;                        // actual miss pkt cnt
}PacketInfo;

typedef enum {
    UpgrdState_Unknow   = 0,        // unknow (init / no response)
    UpgrdState_Err_5_0v = 1,        // E-5.0v (5.0v vbat  error)    in notice (error 1 ~ error 6 )
    UpgrdState_Err_3_4v,            // E-3.4v (3.4v vbat error)
    UpgrdState_Err_SNR,             // E-SNR (SNR error)
    UpgrdState_Err_RSSI,            // E-RSSI (RSSI error)
    UpgrdState_Err_VerNo,           // E-VerNo (ver same error)
    UpgrdState_Err_VerCrc,          // E-CrcV (ver crc error)       in query (error 6 ~ error 8)
    UpgrdState_Err_AppCrc,          // E-CrcA (app crc error)   
    UpgrdState_Err_PktCnt = 8,      // E-Cnt (pkt cnt error)   
    UpgrdState_NotStart,            // not   (not start)
    UpgrdState_PktWait,             // wait  (packet wait)
    UpgrdState_Finish               // ok    (finish)
}UpgradeState;

typedef enum {
    FLG_Unknow,
    FLG_Forbid,     // err 1-6
    FLG_NotStart,
    FLG_PktWait,
    FLG_Finish,
    FLG_Error,      // err 7-8
    FLG_Max
}StateFlag;

typedef struct docNode{
    struct docNode *prev;
    struct docNode *next;
    uint8 mtrNo[20];
    uint8 ver[7];
    uint8 state;
}DocInfo;


extern void UpgradeFunc(void);    // ��������-���
extern int InitPktInfo(PacketInfo *pktInfo, char *fileName, uint16 pktSize, uint32 pktStartIdx, void *header);
extern int  CopyPktToBuf(PacketInfo *pktInfo, uint16 pktIdx, uint8 *buf);
extern void ClearMissPktFlags(PacketInfo *pktInfo);
extern void AddMissPktFlags(PacketInfo *pktInfo, uint8 *bitflags, uint16 byteCnt);
extern void GetMissPktList(PacketInfo *pktInfo);
extern void GetMissPktList_CurrPkt(uint8 *bitfalgs, uint16 byteCnt, uint16 pktCnt, uint16 *missPkts, uint16 *missCnt);

#endif