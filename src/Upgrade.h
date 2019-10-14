#ifndef Upgrade_H
#define Upgrade_H

#include "Common.h"

#define Upgrd_FileMaxSize   (52 * 1024)
#define Upgrd_PacketSize    128
#define Upgrd_PacketCntMax  (Upgrd_FileMaxSize / Upgrd_PacketSize)
#define Upgrd_MeterMax      256

// bin文件的 前128byte（实际使用69byte）保存升级代码相关信息
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
    uint8   rfParam[5];    // RF收发频率 3byte，信道带宽 1byte ，扩频因子 1byte
    uint8   rxdChanel;
    uint8   waterMeterVbat;
    uint8   gasMeterVbat;
    uint8   snr;
    uint8   rssi;
    uint8   upgradeStatus;
    uint8   crc16_appVer[2];
}AppFileInfo;

typedef struct
{
    char *  fileName;
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
    uint16  bitFlagsCnt;
    uint16  missPkts[Upgrd_PacketCntMax];  
    uint16  missPktsCnt;
    
}PacketInfo;

typedef enum {
    UpgrdState_NotStart = 0,     // not
    UpgrdState_PktWait,          // wait
    UpgrdState_Finish            // ok
}UpgradeState;

typedef struct{
    uint16 cnt;
    uint8 mtrNos[Upgrd_MeterMax][20];
    uint8 states[Upgrd_MeterMax];   // UpgradeState
}UpgradeDocs;

extern void UpgradeFunc(void);    // 程序升级-入口
extern int InitPktInfo(PacketInfo *pktInfo, char *fileName, uint16 pktSize, uint32 pktStartIdx, void *header);
extern int  CopyPktToBuf(PacketInfo *pktInfo, uint16 pktIdx, uint8 *buf);
extern void ClearMissPktFlags(PacketInfo *pktInfo);
extern void AddMissPktFlags(PacketInfo *pktInfo, uint8 *bitflags, uint16 byteCnt);
extern void GetMissPktList(PacketInfo *pktInfo);
extern void GetMissPktList_CurrPkt(uint8 *bitfalgs, uint16 byteCnt, uint16 pktCnt, uint16 *missPkts, uint16 *missCnt);

#endif