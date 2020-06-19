#ifndef DbQueryOptimize_H
#define DbQueryOptimize_H

#include "Common.h"
#include "MeterDocDBF.h"

// 表节点
typedef struct _Mnode
{
    struct _Mnode * Next;

    uint32 DbIdx;       // dbf 记录索引
}Mnode;

// 楼栋节点
typedef struct _Bnode
{
    struct _Bnode * Next;
    struct _Mnode * Mhead;

    char BuildNum[12];  // 楼栋编号
}Bnode;


// 小区节点
typedef struct _Cnode
{
    struct _Bnode * BHead;

    char CellNum[12];   // 小区编号
}Cnode;

// 优化查询结构
typedef struct _DbQuerySt
{
    Cnode Cells[District_Max];  // 小区列表
    uint16 CellCnt;             // 小区数量
    uint16 CellIdx;     // 小区索引
    uint16 BuildIdx;    // 楼栋索引
    uint16 MeterIdx;    // 表具索引

    uint32	dbCurrIdx;	// 数据库当前位置
	uint32	reqMaxCnt;	// 最大请求数
	uint32	resultCnt;	// 查询的结果记录数
	uint16	errorCode;	// 0 - ok,  其他 - 出错
}DbQuerySt;



extern void QueryDistrictList(DistrictListSt *districts, DbQuerySt *query);
extern void QueryBuildingList(BuildingListSt *buildings, DbQuerySt *query);
extern void QueryMeterList(MeterListSt *meters, DbQuerySt *query);

#endif