#ifndef NORFLASH_H
#define NORFLASH_H

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define FLASH_ADDR		0x80000000
#define BOOT_SIZE		64*1024

#define BlockEraseTime          357143      /* maximum timeout of read cycles for block-erase, 25ms/70ns */
#define SectorEraseTime         357143      /* maximum timeout of read cycles for sector-erase, 25ms/70ns */
#define WordProgramTime         143         /* maximum timeout of read cycles for word-program, 10us/70ns */
#define AddrsShift              2 

typedef struct
{
    UINT8 MachineNumber[16];
    UINT8 MachineID[16];
    UINT8 ManufactureID[16];
    uint8  RecoverySiteTags;
    uint8  BackupbattTags;
    uint8  BeepOnOffTags;
    uint16 ShutDownTime;
    uint16 BacklightValue;
    uint16 ContrastValue;
    uint8  CommPort;
    uint8  CommBaud;
     
} MACHINE_INFO, *PMACHINE_INFO;

#define MACHINE_FLASH_INFO_START  30*1024

extern int8 _WriteTags(int8 Tag);
extern int8 _ReadTags(void);
#endif