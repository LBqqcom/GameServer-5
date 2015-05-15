/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCityThread.h
Version:		1.0
Description:    城市服务
Author:			pengwen
Create Date:	2012-8-16
History:        处理城市事件，领主事件     

******************************************************************/

#ifndef CCITYTHREAD_H_
#define CCITYTHREAD_H_
#include <cassert>
#include "CTaskThread.h"
#include "CGameMesEvent.h"


class CCityThread :
	public CCTaskThread
{
private:
	//用户建造建筑事件处理器
	CCGameMesEvent* m_pCityUserBuildEvent;
	//建筑功能事件处理器
	CCGameMesEvent* m_pBuildFunEvent; 
	//领主事件处理器
	CCGameMesEvent* m_pMasterEvent;
	//城市事件处理器
	CCGameMesEvent* m_pCityEvent;
	//副本事件处理器
	CCGameMesEvent* m_pDuplicateEvent;
	//聊天事件处理器
	CCGameMesEvent* m_pChatEvent;
    //道具事件处理器
    CCGameMesEvent* m_pItemEvent;
	//区域事件处理器
	CCGameMesEvent* m_pRegionEvent;
	//军队事件处理器
	CCGameMesEvent* m_pArmyEvent;
	//阵法事件处理器
	CCGameMesEvent* m_pBattleArrayEvent;
	//任务事件处理器
	CCGameMesEvent* m_pTaskEvent;
	//竞技场事件处理器
	CCGameMesEvent* m_pPVPEvent;
    //日常/签到事件处理器
    CCGameMesEvent* m_pDailyEvent;
	//商店事件处理器
	CCGameMesEvent* m_pMarketEvent;
	//宠物事件处理器
	CCGameMesEvent* m_pPetEvent;
	//邮件处理器
	CCGameMesEvent* m_pEmailEvent;
	//vip处理器
	CCGameMesEvent* m_pVipEvent;
	//插入用户登录信息
	CCGameMesEvent* m_InsertUserData;
	//活动事件处理器
	CCGameMesEvent* m_pActiveEvent;
	//祭祀事件处理器
	CCGameMesEvent *m_pWorshipEvent;
	//信息反馈处理器
	CCGameMesEvent *m_pFeedBackEvent;
	// 资源掠夺系统
	CCGameMesEvent *m_pResourceGrabEvent;
	//好友事件处理器
	CCGameMesEvent* m_pFriendEvent;
	//组队副本
	CCGameMesEvent* m_pTeamDuplicate;
	//战队事件处理器
	CCGameMesEvent* m_pFightTeamEvent;
	//排行榜
	CCGameMesEvent* m_pTopEvent;
	//元素之塔
	CCGameMesEvent* m_pElementTower;
	//世界BOSS
	CCGameMesEvent* m_pBossEvent;
	//用户退出处理器
	CCGameMesEvent*  m_pUserExitEvent;
	//用户登录事件处理器
	CCGameMesEvent*  m_pUserEnterEvent;
	//服务事件处理
	CCGameMesEvent*  m_pServerEvent;
	//用户数据缓存
	CCGameMesEvent*  m_pUserData;
	//跨服战斗
	CCGameMesEvent* pCSFightEvent;
	//龙岛
	CCGameMesEvent* m_pDragonIsland;
	//探访
	CCGameMesEvent* m_pSeekEvent;
public:
	CCityThread(CT_VOID);
	~CCityThread(CT_VOID);


public:
	//打开
	virtual CT_VOID OnOpen(); 
	//
	virtual CT_VOID OnTimer(CT_DWORD timeID);
};
#endif