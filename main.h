#ifndef MAIN_H
#define MAIN_H

// --------------------------------  常量  -----------------------------------------

#define CRC16_Seed		0x8408		// 国网/北网 CRC16算法 seed 固定为 0x8408


// --------------------------------  类型定义  -----------------------------------------
typedef unsigned char bool;
#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif

typedef struct{
	uint8 buf[200];
	uint8 *items[10];
	uint8 cnt;
}ParamsBuf;


typedef enum{
	ModType_MainNode,
	ModType_PowerSub,
	ModType_WaterSub
}ModuleTypeDef;

typedef enum{
	// 抄表
	PowerCmd_ReadMeter_645_07,	
	PowerCmd_ReadMeter_645_97,	
	PowerCmd_ReadMeter_698,

	// 节点控制
	PowerCmd_SetSerialCom,
	PowerCmd_SetChanelGrp,
	PowerCmd_SetRssiThreshold,
	PowerCmd_SetSendPower,
	PowerCmd_DeviceReboot,
	
	PowerCmd_ParamsInit,			// 仅对集中器
	PowerCmd_StartNwkBuild,		// 仅对集中器
	PowerCmd_StartNwkMaintain,	// 仅对集中器
	PowerCmd_BroadClearNeighbor,	// 仅对集中器 Aps命令 97
	PowerCmd_BroadSetSendPower,	// 仅对集中器
	
	PowerCmd_ClearNeighbor,		// 仅对电表 Aps命令 97 00
	PowerCmd_ChangeCollect2Addr,	// 仅对电表 Aps命令 96
	PowerCmd_ForceJoinNwkRequest,// 仅对电表 Aps命令 99
	
	// 参数读取
	PowerCmd_ReadNodeInfo,
	PowerCmd_ReadSendPower,
	PowerCmd_ReadNwkStatus,
	PowerCmd_ReadVerInfo,
	PowerCmd_ReadSubNodeRoute,	// 仅对集中器
	PowerCmd_ReadAllMeterDoc,	// 仅对集中器
	PowerCmd_ReadNeighbor,		// 仅对电表 Aps命令 97 00

	// 单向水表
	PowerCmd_ReadReportData,		
	PowerCmd_ClearReportData,
	PowerCmd_QueryBindedWaterMeter,
	PowerCmd_AddBindedWaterMeter,
	PowerCmd_DelBindedWaterMeter
}PowerCmdDef;

typedef enum{
	WaterCmd_ReadMeterDirect,
	WaterCmd_ReadMeterByPwrMeter,
	WaterCmd_ReadNeighbor,	
	WaterCmd_ClearNeighbor		// aps命令 97 02
}WaterCmdDef;

#endif
