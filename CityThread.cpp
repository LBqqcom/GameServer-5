#include <cstdlib>
#include <time.h>
#include "CityThread.h"
#include "CUserEnterEvent.h"
#include "CCityUserBuildEvent.h"
#include "CGlobeFunc.h"
#include "CMD_Game.h"
#include "CDatabaseMes.h"
#include "CBuildFunEvent.h"
#include "CMasterEvent.h"
#include "CCityEvent.h"
#include "CChatEvent.h"
#include "CMasterItemEvent.h"
#include "CRegionEvent.h"
#include "CDuplicateEvent.h"
#include "CArmyEvent.h"
#include "CBattleArrayEvent.h"
#include "CTaskEvent.h"
#include "CPVPEvent.h"
#include "CDailyEvent.h"
#include "CMarketEvent.h"
#include "CPetEvent.h"
#include "CEmailEvent.h"
#include "CVipEvent.h"
#include "CInsertUserInfo.h"
#include "CActiveEvent.h"
#include "CWorshipEvent.h"
#include "CResourceGrabEvent.h"
#include "CFriendEvent.h"
#include "CFightTeamEvent.h"
#include "CCustomerFeedBackEvent.h"
#include "CTeamDuplicateEvent.h"
#include "CTopEvent.h"
#include "CTowerEvent.h"
#include "CBossEvent.h"
#include "CUserExitEvent.h"
#include "CTimerThread.h"
#include "CUserEnterManager.h"
#include "CUserEnterEvent.h"
#include "ServerEvent.h"
#include "CFightingData.h"
#include "CUserDataEvent.h"
#include "CCrossServerFightEvent.h"
#include "CIslandEvent.h"
#include "CSeekEvent.h"
CCityThread::CCityThread(CT_VOID) : 
                                    m_pCityUserBuildEvent(0),
									m_pBuildFunEvent(0),
									m_pMasterEvent(0),
									m_pCityEvent(0),
									m_pChatEvent(0),
                                    m_pItemEvent(0),
									m_pDuplicateEvent(0),
									m_pRegionEvent(0),
									m_pArmyEvent(0),
									m_pBattleArrayEvent(0),
									m_pTaskEvent(0),
                                    m_pDailyEvent(0),
									m_pPVPEvent(0),
									m_pMarketEvent(0),
									m_pPetEvent(0),
									m_pEmailEvent(0),
									m_pVipEvent(0),
									m_InsertUserData(0),
									m_pActiveEvent(0),
									m_pWorshipEvent(0),
									m_pResourceGrabEvent(0),
									m_pFriendEvent(0),
									m_pFeedBackEvent(0),
									m_pTeamDuplicate(0),
									m_pFightTeamEvent(0),
									m_pTopEvent(0),
									m_pElementTower(0),
									m_pUserExitEvent(0),
									m_pUserEnterEvent(0),
									m_pServerEvent(0),
									m_pUserData(0),
									pCSFightEvent(0),
									m_pDragonIsland(0),
									m_pSeekEvent(0)
{
}

CCityThread::~CCityThread(CT_VOID)
{
	delete m_pCityUserBuildEvent;
	m_pCityUserBuildEvent = 0;
	delete m_pBuildFunEvent;
	m_pBuildFunEvent = 0;

	delete m_pMasterEvent;
	m_pMasterEvent = 0;
	delete m_pCityEvent;
	m_pCityEvent = 0;
	delete m_pChatEvent;
	m_pChatEvent = 0;
    delete m_pItemEvent;
    m_pItemEvent = 0;
	delete m_pDuplicateEvent;
	m_pDuplicateEvent = 0;
	delete m_pRegionEvent;
	m_pRegionEvent = 0;
	delete m_pArmyEvent;
	m_pArmyEvent = 0;
	delete m_pBattleArrayEvent;
	m_pBattleArrayEvent	= 0;
	delete m_pTaskEvent;
	m_pTaskEvent = 0;
	delete m_pPVPEvent;
	m_pPVPEvent = 0;
    delete m_pDailyEvent;
    m_pDailyEvent = 0;
	delete m_pMarketEvent;
	m_pMarketEvent = 0;
	delete m_pPetEvent;
	m_pPetEvent = 0;
	delete m_pEmailEvent;
	m_pEmailEvent = 0;

	delete m_pVipEvent;
	m_pVipEvent=0;

	delete m_InsertUserData;
	m_InsertUserData=0;

	delete m_pActiveEvent;
	m_pActiveEvent=0;

	delete m_pWorshipEvent;
	m_pWorshipEvent=0;

	delete m_pResourceGrabEvent;
	m_pResourceGrabEvent = 0;

	delete m_pFeedBackEvent;
	m_pFeedBackEvent=0;

	delete m_pFriendEvent;
	m_pFriendEvent = 0;

	delete m_pTeamDuplicate;
	m_pTeamDuplicate = 0;

	delete m_pFightTeamEvent;
	m_pFightTeamEvent = 0;

	delete m_pTopEvent;
	m_pTopEvent = 0;

	delete m_pElementTower;
	m_pElementTower = 0;

	delete m_pUserExitEvent;
	m_pUserExitEvent = 0;

	delete m_pUserEnterEvent;
	m_pUserEnterEvent = 0;

	delete m_pServerEvent;
	m_pServerEvent=0;

	delete m_pUserData;
	m_pUserData=0;

	delete pCSFightEvent;
	pCSFightEvent=0;

	delete m_pDragonIsland;
	m_pDragonIsland = 0;

	delete m_pSeekEvent;
	m_pSeekEvent = 0;
}

//打开
CT_VOID CCityThread::OnOpen()
{
	m_pCityUserBuildEvent   = G_CreateGameEvent<CCCityUserBuildEvent>(&m_GameMesManager, m_id);
	m_pBuildFunEvent        = G_CreateGameEvent<CCBuildFunEvent>(&m_GameMesManager, m_id);
	m_pMasterEvent          = G_CreateGameEvent<CCMasterEvent>(&m_GameMesManager, m_id);
	m_pCityEvent            = G_CreateGameEvent<CCCityEvent>(&m_GameMesManager, m_id);
	m_pChatEvent			= G_CreateGameEvent<CChatEvent>(&m_GameMesManager, m_id);
    m_pItemEvent            = G_CreateGameEvent<CCMasterItemEvent>(&m_GameMesManager, m_id); 
	m_pRegionEvent			= G_CreateGameEvent<CCRegionEvent>(&m_GameMesManager, m_id);
    m_pDuplicateEvent		= G_CreateGameEvent<CCDuplicateEvent1>(&m_GameMesManager, m_id);
	m_pArmyEvent			= G_CreateGameEvent<CCArmyEvent>(&m_GameMesManager, m_id);
	m_pBattleArrayEvent		= G_CreateGameEvent<CCBattleArrayEvent>(&m_GameMesManager, m_id);
	m_pTaskEvent			= G_CreateGameEvent<CCTaskEvent>(&m_GameMesManager,m_id);
	m_pPVPEvent				= G_CreateGameEvent<CCPVPEvent>(&m_GameMesManager,m_id);
    m_pDailyEvent           = G_CreateGameEvent<CCDailyEvent>(&m_GameMesManager,m_id);
	m_pMarketEvent			= G_CreateGameEvent<CCMarketEvent>(&m_GameMesManager,m_id);
	m_pPetEvent				= G_CreateGameEvent<CCPetEvent>(&m_GameMesManager,m_id);
	m_pEmailEvent			= G_CreateGameEvent<CCEmailEvent>(&m_GameMesManager,m_id);
	m_pVipEvent				= G_CreateGameEvent<CCVipEvent>(&m_GameMesManager,m_id);
	m_InsertUserData		= G_CreateGameEvent<CCInsertUserInfo>(&m_GameMesManager,m_id);
	m_pActiveEvent			= G_CreateGameEvent<CCActiveEvent>(&m_GameMesManager,m_id);
	m_pWorshipEvent			= G_CreateGameEvent<CCWorshipEvent>(&m_GameMesManager,m_id);
	m_pResourceGrabEvent    = G_CreateGameEvent<CCResourceGrabEvent>(&m_GameMesManager, m_id);
	m_pFriendEvent			= G_CreateGameEvent<CCFriendEvent>(&m_GameMesManager,m_id);
	m_pFightTeamEvent		= G_CreateGameEvent<CCFightTeamEvent>(&m_GameMesManager,m_id);
	m_pFeedBackEvent		= G_CreateGameEvent<CCCustomerFeedBackEvent>(&m_GameMesManager,m_id);
	m_pTeamDuplicate		= G_CreateGameEvent<CCTeamDuplicateEvent>(&m_GameMesManager,m_id);
	m_pTopEvent				= G_CreateGameEvent<CCTopEvent>(&m_GameMesManager,m_id);
	m_pElementTower			= G_CreateGameEvent<CCTowerEvent>(&m_GameMesManager, m_id);
	m_pBossEvent			= G_CreateGameEvent<CCBossEvent>(&m_GameMesManager, m_id);
	m_pUserExitEvent		= G_CreateGameEvent<CCUserExitEvent>(&m_GameMesManager, m_id);
	m_pUserEnterEvent		= G_CreateGameEvent<CCUserEnterEvent>(&m_GameMesManager, m_id);
	m_pServerEvent			=G_CreateGameEvent<CServerEvent>(&m_GameMesManager, m_id);
	m_pUserData				=G_CreateGameEvent<CCUserDataEvent>(&m_GameMesManager, m_id);
	pCSFightEvent			=G_CreateGameEvent<CCCrossServerFightEvent>(&m_GameMesManager, m_id);
	m_pDragonIsland			= G_CreateGameEvent<CCIslandEvent>(&m_GameMesManager, m_id);
	m_pSeekEvent			= G_CreateGameEvent<CCSeekEvent>(&m_GameMesManager, m_id);

	//设置数据库定时器
	g_lOpenServerTime=time(0);
	g_iDBUpdataValue =  ((CCGameConfig&)CCConfig::getSingleton()).m_iDBUpdataValue;
	g_iDBUpdataValue =  g_iDBUpdataValue < UPDATA_DATA_VALUE? UPDATA_DATA_VALUE:g_iDBUpdataValue;
	CT_INT32 updateTime = ((CCGameConfig&)CCConfig::getSingleton()).m_iDBUpdataTime;
	CT_INT32 fitTime = (updateTime > TIME_UPDATA? updateTime : TIME_UPDATA)*1000;
	CCTimerThread::getSingleton().SetTimer(IDI_UPDATA_DATA_TIMER, this, fitTime);

	//设置定时器
	G_ValueRange vr = {0};
	if(G_GetValueRange(VALUE_TYPE_HEARTBEATTIME,  &vr) && m_id == GAME_THREAD_CITY)
	{
		CCTimerThread::getSingleton().SetTimer(IDI_HEART_BEAT_TIMER, this, (CT_INT32)(HEART_BEAT_MIN > vr.fMinVal?HEART_BEAT_MIN:vr.fMinVal)*1000);
	}

	//心跳阀值
	memset(&vr,0,sizeof(vr));
	if(G_GetValueRange(VALUE_TYPE_HEARTBEATVALUE,  &vr))
	{
		g_lHeartBeat=(CT_INT64)(HEART_VALUE > vr.fMinVal?HEART_BEAT_MIN:vr.fMinVal);
	}

	//战斗数据删除
	if(m_id == GAME_THREAD_CITY)
		CCTimerThread::getSingleton().SetTimer(IDI_DELETE_FIGHTING_DATA_TIMER, this, FIGHTING_DATA_DELETE*1000);

	//验证码删除造成重连失败
	//设置定时器
	memset(&vr,0,sizeof(vr));
	if(G_GetValueRange(VALUE_TYPE_DELETEVALIDATE,  &vr) && m_id == GAME_THREAD_CITY)
	{
		CCTimerThread::getSingleton().SetTimer(IDI_DELETE_VALIDATE_TIMER, this, (CT_DWORD)((vr.fMinVal<DELETEVALIDATE?DELETEVALIDATE:vr.fMinVal)*1000));
	}


#if 0//此功能已作废
	//设置定时器
	memset(&vr,0,sizeof(vr));
	if(G_GetValueRange(VALUE_TYPE_PVPDEGREEUPTIME,  &vr))
	{
		CCTimerThread::getSingleton().SetTimer(IDI_PVP_DEGREE, this, (CT_DWORD)(vr.fMinVal*1000));
	}
#endif
}

CT_VOID CCityThread::OnTimer(CT_DWORD timeID)
{
	if (timeID == IDI_HEART_BEAT_TIMER)
	{
#ifdef _SHOW_MESSAGE_INFO
		G_ReadCount();
#endif
		CCUserManager::getSingleton().UpdateHeartBeat();
	}
#if 0//此功能已作废
	else if(timeID == IDI_PVP_DEGREE)
	{
		CCUserManager::getSingleton().UpdataPVPDegree();
	}
#endif
	else if(timeID == IDI_DELETE_VALIDATE_TIMER)
	{
		//CCUserEnterManager::getSingleton().DeleteValidate();(验证码删除造成重连失败)
		CCUserEnterManager::getSingleton().ExitDeleteValidate();
	}
	else if(timeID == IDI_UPDATA_DATA_TIMER)
	{ 
		CCUserManager::getSingleton().DBUpdataData();
		//CCFightTeamManager::getSingleton().DBUpdataData();	考虑数据库性能,关闭重复的定时保存
	}
	else if(timeID == IDI_DELETE_FIGHTING_DATA_TIMER)
	{
		CCFightingData::getSingleton().DeleteData();
	}
	else
	{
		m_GameMesManager.DeliverTime(timeID); 
	}
}

