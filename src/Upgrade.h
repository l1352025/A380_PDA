#ifndef Upgrade_H
#define Upgrade_H

#include "Common.h"

#define Upgrd_FileMaxSize   (52 * 1024)
#define Upgrd_PacketSize    128

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
}UpgradeInfo;

typedef struct
{
    uint8   bitFlags[52];   // max cnt = 52 = Upgrd_FileMaxSize / Upgrd_PacketSize / 8
    uint16  packetCnt;      // max cnt = 416 = Upgrd_FileMaxSize / Upgrd_PacketSize
    uint16  missCnt;
    uint16  missList[460];  

}PacketInfo;

extern void Func_Upgrade(void);    // 程序升级-入口

#endif