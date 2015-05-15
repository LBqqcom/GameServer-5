/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCCityEvent.h
Version:		1.0
Description:    处理城市事件
Author:			pengwen
Create Date:	2012-9-13
History:        处理城市事件  ，工人事件，宠物事件，等等   

******************************************************************/
#ifndef CCCITYEVENT_H_
#define CCCITYEVENT_H_
#include "CGameMesEvent.h"
#include "CGDefine.h"

class  CCUser;
struct CMD_G_ApplyAwardEvent;
struct CMD_G_EVENT;
struct MSC_Event_Fighting_End;
class CCCityEvent :
	public CCGameMesEvent
{
public:
	CCCityEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCCityEvent(void);
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//处理城市事件
	CT_VOID  DisposeCityEvent(CT_DWORD subID,  CT_CHAR* pBuf, CT_DWORD len);
	//处理城市事件
	CT_VOID  DisposeMscCityEvent(CT_DWORD subID,  CT_CHAR* pBuf, CT_DWORD len);
private:
	//处理用户奖励
	CT_VOID DisposeApplyAwardEvent(const CMD_G_ApplyAwardEvent& awardEvent);
	//处理用户奖励
	CT_VOID DisposeApplyAwardEvent(CCUser* pUser, CT_INT32 eventID,  CT_INT32 index,  CT_INT32 awardID, ENUM_EventType et);
	
private:
	//处理战斗奖励
	CT_VOID DisposeFightEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//处理建筑奖励
	CT_VOID DisposeBuildingEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//处理工人奖励
	CT_VOID DisposeWorkerEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//处理宠物奖励
	CT_VOID DisposePetEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//处理宝箱奖励
	CT_VOID DisposeTreasureEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//心跳
	CT_VOID DisposeHeartBeatEvent(CT_CHAR* pBuf, CT_DWORD len);
	//系统广播
	CT_VOID DisposeBroadcast(CT_CHAR* pBuf, CT_DWORD len);
	//世界聊天
	CT_VOID DisposeBroadcastWorldMessage(CT_CHAR* pBuf, CT_DWORD len);
	//更新公告
	CT_VOID DisposeAnnouncementUpdata(CT_CHAR* pBuf, CT_DWORD len);


private:
	//处理战斗事件
	CT_VOID DisposeMscFightingEventEnd(const MSC_Event_Fighting_End &FightingEventEnd);
};

#endif
