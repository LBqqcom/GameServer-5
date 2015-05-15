/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserEvent.h
Version:		1.0
Description:    用户事件数据
Author:			pengwen
Create Date:	2012-9-29
History:        用户事件数据     

******************************************************************/
#ifndef CCUSEREVENT_H_
#define CCUSEREVENT_H_
#include <map>
#include <set>
#include "CTType.h"
#include "CGDefine.h"
class CCUser;
class CCMutex;
struct RandEvent;
class CCUserEvent
{
private:
	//事件中结构（表明该事件执行中）
	struct LastingEvent
	{
		CT_INT32    iIndex;      //事件索引
		CT_LONGLONG llTimeBegin; //开始时间
		CT_INT32    iLasting;    //事件持续时间
	};
private:
	CCUser*    m_pUser;
	CCMutex*   m_pMutex;
	typedef std::map<CT_INT32, LastingEvent>             UEEventMap;        //事件中建筑容器
	UEEventMap                m_buildingEventMap;        //建筑事件
	UEEventMap                m_workerEventMap;          //工人事件
	UEEventMap                m_treasureEventMap;        //宝箱事件
	UEEventMap                m_petEventMap;             //宠物事件
	UEEventMap                m_fightMap;                //战斗事件
public:
	CCUserEvent(void);
	~CCUserEvent(void);
public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser  = pUser;
		m_pMutex = pMutex;
		m_buildingEventMap.clear();
		m_workerEventMap.clear();
		m_treasureEventMap.clear();
		m_petEventMap.clear();
		m_fightMap.clear();
	}
	//清理用户数据
	CT_VOID Clear();
public:
	//查找建筑事件
	CT_BOOL  FindBuildingEventByIndex(CT_INT32 iIndex);
    //查找事件
	CT_BOOL  FindPetEvent(CT_INT32 iIndex);
	//查找事件
	CT_BOOL  FindFightEvent(CT_INT32 iIndex);
	//查找事件
	CT_BOOL  FindWorkerEvent(CT_INT32 iIndex);
	//查找事件
	CT_BOOL  FindTreasureEvent(CT_INT32 iIndex);
	//根据建筑id查找事件
	CT_BOOL	 FindBuildingEventByID(CT_INT32 iBuildID);

public:
	//更新事件(定时产生出新的事件)
	CT_VOID UpdateEvent();
	

public:
	//得到事件
	CT_BOOL GetEvents(CT_CHAR* pData, CT_INT32& dataLen);
	//分配事件
	CT_VOID AssignEvent(const RandEvent& re);
private:   
	//更新事件
	CT_VOID UpdateEvent(UEEventMap& eventMap, ENUM_EventType et);
	//分配宝箱事件
	CT_VOID AssignTreasureEvent(const RandEvent& re);
	//分配工人事件
	CT_VOID AssignWorkerEvent(const RandEvent& re);
	//分配宠物事件
	CT_VOID AssignPetEvent(const RandEvent& re);
	//分配建筑事件
	CT_VOID AssginBuildingEvent(const RandEvent& re);
	//分配战斗事件
	CT_VOID AssginFightingEvent(const RandEvent& re);
public:
	//删除建筑事件
	CT_VOID DeleteBuildingEvent(CT_INT32 iIndex);
	//删除事件
	CT_VOID DeletePetEvent(CT_INT32   iIndex);
	//删除事件
	CT_VOID DeleteWorkerEvent(CT_INT32 iIndex);
	//删除事件
	CT_VOID DeleteTreasureEvent(CT_INT32 iIndex);
	//删除事件
	CT_VOID DeleteFightEvent(CT_INT32 iIndex);

};

#endif