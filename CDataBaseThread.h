/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDataBaseThread.h
Version:		1.0
Description:    数据库服务
Author:			pengwen
Create Date:	2012-8-16
History:        处理数据库事件     

******************************************************************/

#ifndef CCDATABASETHREAD_H_
#define CCDATABASETHREAD_H_
#include "CTaskThread.h"
#include "CDBConnector.h"
#include "CGameMesEvent.h"

struct MSC_DB_UserEnter;

class CCDataBaseThread :
	public CCTaskThread
{
private:
	//用户登录事件处理器
	CCGameMesEvent*  m_pDBUserEnterEvent;
	//用户升级建筑数据库处理器
	CCGameMesEvent*  m_pDBCityUserBuildEvent;
	//公用数据库处理器
	CCGameMesEvent*  m_pDBCommonEvent; 
	//建筑功能数据库处理器CCDBBuildFunEvent
	CCGameMesEvent*  m_pDBBuildFunEvent;
    // 领主事件处理器
    CCGameMesEvent*  m_pDBMasterEvent;
	//事件处理器
	CCGameMesEvent* m_pDBBuildingEvent;
	//战斗处理器
	CCGameMesEvent *m_pDBDuplicateEvent;
	//用户区域事件处理
	CCGameMesEvent *m_pDBRegionEvent;
	//gm消息事件处理
	CCGameMesEvent *m_pDBChatEvent;
	//军队消息事件处理
	CCGameMesEvent *m_pDBArmyEvent;
	//阵法消息事件处理
	CCGameMesEvent *m_pDBBattleArrayEvent;
	//任务消息处理
	CCGameMesEvent *m_pDBTaskEvent;
	//竞技场消息处理
	CCGameMesEvent *m_pDBPVPEvent;
    // 日常消息处理
    CCGameMesEvent *m_pDBDailyEvent;
	//商店消息处理
	CCGameMesEvent *m_pDBMarketEvent;
	//宠物消息
	CCGameMesEvent *m_pDBPetEvent;
	//用户退出消息
	CCGameMesEvent *m_pDBUserExitEvent;
	//邮件消息
	CCGameMesEvent *m_pDBEmailEvent;
	//vip消息
	CCGameMesEvent *m_pDBVipEvent;
	//活动消息
	CCGameMesEvent *m_pDBActiveEvent;
	//祭祀消息
	CCGameMesEvent *m_pDBWorshipEvent;
	//好友消息
	CCGameMesEvent *m_pDBFriendEvent;
	//战队数据
	CCGameMesEvent *m_pDBFightTeamEvent;
	//资源抢夺
	CCGameMesEvent *m_pDBResourceGrap; 
	//排行榜
	CCGameMesEvent *m_pDBTopEvent;
	//元素之塔
	CCGameMesEvent *m_pDBTowerEvent;
	//全局数据
	CCGameMesEvent *m_pDBGlobeEvent;
	//BOSS消息
	CCGameMesEvent *m_pDBBossEvent;
	//跨服战
	CCGameMesEvent *m_pDBCSFightEvent;
	//龙岛
	CCGameMesEvent *m_pDBDragonIsland;
	//探访
	CCGameMesEvent *m_pDBSeekEvent;

private:
	CCDBConnector m_dbConnector; 

public:
	CCDataBaseThread(CT_VOID);
	~CCDataBaseThread(CT_VOID);
public:
	//打开
	virtual CT_VOID OnOpen(); 

	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//定时器
	CT_VOID OnTimer(CT_DWORD timeID);
	CT_VOID OnClock(CT_DWORD timeID);

private:
	//插入战斗记录
	CT_VOID DisposeFightingRecord();
	//插入元素塔战斗数据
	CT_VOID DisposeTowerFightRecord();
	//插入排行榜信息
	CT_VOID DisposeTopInfo();
	//CT_VOID DisposeGMMessage();
	//插入公告信息
	CT_VOID DisposeAnnouncement();
	CT_VOID DisposeUpdateData();
};

#endif
