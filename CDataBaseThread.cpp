#include <cassert>
#include "CDataBaseThread.h"
#include "CDatabaseMes.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "CMesBlock.h"
#include "CTimerThread.h"
#include "CGlobeData.h"
#include "CThreadManager.h"
#include "CNetInterface.h"
#include "CGlobeFunc.h"
#include "CDBUserEnterEvent.h"
#include "CDBCityUserBuildEvent.h"
#include "CDBCommonDataEvent.h"
#include "CDBBuildFunEvent.h"
#include "CDBMasterEvent.h"
#include "GlobeScriptInterface.h"
#include "CDBCityEvent.h"
#include "CDBDuplicate.h"
#include "CFightingRecord.h"
#include "CDBRegionEvent.h"
#include "CDBChatEvent.h"
#include "CDBArmy.h"
#include "CDBBattleArray.h"
#include "CDBTaskEvent.h"
#include "CDBPVPEvent.h"
#include "CDBDailyEvent.h"
#include "CDBMarketEvent.h"
#include "CTop.h"
#include "CDBPetEvent.h"
#include "CDBUserExitEvent.h"
#include "CDBEmailEvent.h"
#include "CUserManager.h"
#include "CDBVipEvent.h"
#include "CDBActiveEvent.h"
#include "CDBWorshipEvent.h"
#include "CResourceGrabMgr.h"
#include "CDBFriendEvent.h"
#include "CDBBossEvent.h"
#include "CDBFightTeam.h"
#include "CDBResourceGrabEvent.h"
#include "CTowerResManager.h"
#include "CDBTop.h"
#include "CDBTowerEvent.h"
#include "CBroadcastScriptSetting.h"
#include "CDBGlobeDataEvent.h"
#include "CDBCSFightEvent.h"
#include "CDBIslandEvent.h"
#include "CDBSeekEvent.h"

CCDataBaseThread::CCDataBaseThread(CT_VOID) : 
											  m_pDBUserEnterEvent(0),
											  m_pDBCityUserBuildEvent(0),
											  m_pDBCommonEvent(0),
											  m_pDBBuildFunEvent(0),
											  m_pDBBuildingEvent(0),
											  m_pDBDuplicateEvent(0),
											  m_pDBRegionEvent(0),
											  m_pDBChatEvent(0),
											  m_pDBArmyEvent(0),
											  m_pDBBattleArrayEvent(0),
											  m_pDBTaskEvent(0),
                                              m_pDBDailyEvent(0),
											  m_pDBPVPEvent(0),
											  m_pDBMarketEvent(0),
											  m_pDBUserExitEvent(0),
											  m_pDBPetEvent(0),
											  m_pDBEmailEvent(0),
											  m_pDBVipEvent(0),
											  m_pDBActiveEvent(0),
											  m_pDBWorshipEvent(0),
											  m_pDBFriendEvent(0),
											  m_pDBBossEvent(0),
											  m_pDBFightTeamEvent(0),
											  m_pDBResourceGrap(0),
											  m_pDBTopEvent(0), 
											  m_pDBTowerEvent(0),
											  m_pDBGlobeEvent(0),
											  m_pDBCSFightEvent(0),
											  m_pDBDragonIsland(0),
											  m_pDBSeekEvent(0)
											
{
	
}

CCDataBaseThread::~CCDataBaseThread(CT_VOID)
{
	delete m_pDBUserEnterEvent;
	m_pDBUserEnterEvent = 0;
	delete m_pDBCityUserBuildEvent;
	m_pDBCityUserBuildEvent = 0;
	delete m_pDBCommonEvent;
	m_pDBCommonEvent = 0;
	delete m_pDBBuildFunEvent;
	m_pDBBuildFunEvent = 0;
	delete m_pDBBuildingEvent; 
	m_pDBBuildingEvent = 0;
	delete m_pDBDuplicateEvent;
	m_pDBDuplicateEvent = 0;
	delete m_pDBRegionEvent;
	m_pDBRegionEvent = 0;
	delete m_pDBChatEvent;
	m_pDBChatEvent = 0;
	delete m_pDBArmyEvent;
	m_pDBArmyEvent = 0;
	delete m_pDBBattleArrayEvent;
	m_pDBBattleArrayEvent = 0;
	delete m_pDBTaskEvent;
	m_pDBTaskEvent = 0;
	delete m_pDBPVPEvent;
	m_pDBPVPEvent = 0;
    delete m_pDBDailyEvent;
    m_pDBDailyEvent = 0;
	delete m_pDBMarketEvent;
	m_pDBMarketEvent = 0;
	delete m_pDBUserExitEvent;
	m_pDBUserExitEvent = 0;
	delete m_pDBPetEvent;
	m_pDBPetEvent = 0;
	delete m_pDBEmailEvent;
	m_pDBEmailEvent = 0;
	delete m_pDBVipEvent;
	m_pDBVipEvent=0;
	delete m_pDBActiveEvent;
	m_pDBActiveEvent=0;
	delete m_pDBWorshipEvent;
	m_pDBWorshipEvent=0;
	delete m_pDBFriendEvent;
	m_pDBFriendEvent = 0;
	delete m_pDBFightTeamEvent;
	m_pDBFightTeamEvent = 0;
	delete m_pDBResourceGrap;
	m_pDBResourceGrap = 0;
	delete m_pDBTopEvent;
	m_pDBTopEvent = 0;
	delete m_pDBTowerEvent;
	m_pDBTowerEvent = 0;
	delete m_pDBGlobeEvent;
	m_pDBGlobeEvent = 0;
	delete m_pDBBossEvent;
	m_pDBBossEvent = 0;
	delete m_pDBCSFightEvent;
	m_pDBCSFightEvent=0;
	delete m_pDBDragonIsland;
	m_pDBDragonIsland=0;
	delete m_pDBSeekEvent;
	m_pDBSeekEvent = 0;
}
//打开
CT_VOID CCDataBaseThread::OnOpen()
{
	m_pDBUserEnterEvent     = G_CreateGameEvent<CCDBUserEnterEvent>(&m_GameMesManager, m_id,  m_dbConnector) ;
	m_pDBCityUserBuildEvent = G_CreateGameEvent<CCDBCityUserBuildEvent>(&m_GameMesManager, m_id, m_dbConnector) ;
	m_pDBCommonEvent        = G_CreateGameEvent<CCDBCommonDataEvent>(&m_GameMesManager, m_id, m_dbConnector) ;
	m_pDBBuildFunEvent      = G_CreateGameEvent<CCDBBuildFunEvent>(&m_GameMesManager, m_id, m_dbConnector);

    m_pDBMasterEvent        = G_CreateGameEvent<CCDBMasterEvent>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBBuildingEvent      = G_CreateGameEvent<CCDBCityEvent>(&m_GameMesManager, m_id, m_dbConnector);

	m_pDBDuplicateEvent		= G_CreateGameEvent<CCDBDuplicate>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBRegionEvent        = G_CreateGameEvent<CCDBRegionData>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBChatEvent			= G_CreateGameEvent<CCDBChatEvent>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBArmyEvent			= G_CreateGameEvent<CCDBArmy>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBBattleArrayEvent	= G_CreateGameEvent<CCDBBattleArray>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBTaskEvent			= G_CreateGameEvent<CCDBTaskEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBPVPEvent			= G_CreateGameEvent<CCDBPVPEvent>(&m_GameMesManager,m_id,m_dbConnector);
    m_pDBDailyEvent         = G_CreateGameEvent<CCDBDailyEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBMarketEvent		= G_CreateGameEvent<CCDBMarketEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBPetEvent			= G_CreateGameEvent<CCDBPetEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBUserExitEvent      =  G_CreateGameEvent<CCDBUserExitEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBEmailEvent			=	G_CreateGameEvent<CCDBEmailEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBVipEvent			=	G_CreateGameEvent<CCDBVipEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBActiveEvent		=	G_CreateGameEvent<CCDBActiveEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBWorshipEvent		=	G_CreateGameEvent<CCDBWorshipEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBFriendEvent		= G_CreateGameEvent<CCDBFriendEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBFightTeamEvent		= G_CreateGameEvent<CCDBFightTeam>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBResourceGrap		= G_CreateGameEvent<CCDBResourceGrabEvent>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBTopEvent			= G_CreateGameEvent<CCDBTop>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBTowerEvent         = G_CreateGameEvent<CCDBTowerEvent>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBGlobeEvent			=G_CreateGameEvent<CCDBGlobeData>(&m_GameMesManager, m_id, m_dbConnector);
	m_pDBBossEvent			=G_CreateGameEvent<CCDBBossEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBCSFightEvent		=G_CreateGameEvent<CCDBCSFightEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBDragonIsland		=G_CreateGameEvent<CCDBIslandEvent>(&m_GameMesManager,m_id,m_dbConnector);
	m_pDBSeekEvent			= G_CreateGameEvent<CCDBSeekEvnet>(&m_GameMesManager,m_id,m_dbConnector);

	
	m_dbConnector.Init();
	CT_BOOL ret =  m_dbConnector.Connect( ((CCGameConfig&)CCConfig::getSingleton()).m_dbIP.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_dbUser.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_dbKey.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_dbName.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_dbPort
		);
	if (!ret)
	{
		G_ValueRange vr = {0};
		if(G_GetValueRange(VALUE_TYPE_DATABASERECONNECTTIME,  &vr))
		{
			CCTimerThread::getSingleton().SetTimer(IDI_DB_CONNECT_TIMER, this, (CT_INT32)(vr.fMaxVal*1000));
		}

	}else
	{
		CCLogMeg::getSingleton().WriteInfo("CCDataBaseThread::OnOpen(), threadid: %i", m_id);

		//读取战斗记录[10/26/2012/wangchen]
		if(m_id == GAME_THREAD_DATABASE)
		{
			DisposeFightingRecord();
			DisposeTowerFightRecord();
			//读取公告信息
			DisposeAnnouncement();
			((CCDBCSFightEvent*)m_pDBCSFightEvent)->ReadCSReward();
		}

	}
}

CT_VOID CCDataBaseThread::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	if (mainID == MSC_MAIN_DB_UPDATEDATA && subID == MSC_SUB_DB_UPDATA_DATA)
	{
		DisposeUpdateData();
	}
	else
	{
		m_GameMesManager.DeliverMes(mainID, subID, pBuf, len);
	}
}
//定时器
CT_VOID CCDataBaseThread::OnTimer(CT_DWORD timeID)
{
	switch (timeID)
	{
	case IDI_DB_CONNECT_TIMER:
		{
			
			CT_BOOL ret =  m_dbConnector.Connect( ((CCGameConfig&)CCConfig::getSingleton()).m_dbIP.c_str(),
				((CCGameConfig&)CCConfig::getSingleton()).m_dbUser.c_str(),
				((CCGameConfig&)CCConfig::getSingleton()).m_dbKey.c_str(),
				((CCGameConfig&)CCConfig::getSingleton()).m_dbName.c_str(),
				((CCGameConfig&)CCConfig::getSingleton()).m_dbPort
				);
			if (ret)
			{
				CCTimerThread::getSingleton().KillTimer(IDI_DB_CONNECT_TIMER,this);
			}

			break;
		}
	default:
		{		
			m_GameMesManager.DeliverTime(timeID);
			break;
		}
	}
}

CT_VOID CCDataBaseThread::OnClock(CT_DWORD timeID)
{
	m_GameMesManager.DeliverClock(timeID);
}

CT_VOID CCDataBaseThread::DisposeUpdateData()
{
	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UpdateSysEveryDay");

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();

		if (pResult)
		{
			pResult->Release();
		}
	}else
	{
		G_DBError(m_dbConnector, m_id);
		return;
	}

	//更新排行榜
	G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_TOP_INFO, MSC_SUB_TOP_UPDATE, 0, 0 );
}

//插入战斗记录,只会在服务器启动的时候调用一次
CT_VOID CCDataBaseThread::DisposeFightingRecord()
{
	
	std::list<MSC_Fighting_Info> fiList;
	for(CT_INT32 i=0;i<WORLD_SCENE_COUNT;++i)
	{
		G_WorldScene ws={0};
		if(!G_GetWorldScene(i+1,&ws))break;
		for(CT_INT32 j=0;j<ws.iSceneNum;++j)
		{
			for(CT_INT32 k=0;k<DIFFICULTY;++k)
			{
				CT_INT32 iRet = 0;
				MSC_Fighting_Info FightingInfo={0};
				m_dbConnector.Reset();
				m_dbConnector.SetPcName("SP_FightGetExample");

				m_dbConnector.AddParm("_aid", i);
				m_dbConnector.AddParm("_fid", j);
				m_dbConnector.AddParm("_did", k);
				if(m_dbConnector.Exec())
				{
					CCDBResult* pResult = m_dbConnector.GetResult();
					if (pResult->GetResult())
					{  
						CT_DWORD dwUserID=0;
						while (pResult->MoveNextRow())
						{
							CT_INT32 iIndex=0;
							CombatUnitData data={0};
							pResult->GetValue(iIndex++, FightingInfo.iWorldSceneID );
							pResult->GetValue(iIndex++, FightingInfo.iSceneID );
							pResult->GetValue(iIndex++, FightingInfo.iDifficulty );
							//pResult->GetValue(iIndex++,	dwUserID);
							//pResult->GetValue(iIndex++, data.name,NAME_SIZE );
							//pResult->GetValue(iIndex++, data.mcs.fHPPersent);
							//iIndex++;
							//iIndex++;
							////pResult->GetValue(5, FightingInfo.MasterInfo.mcs.fDmgDecPercent );
							////pResult->GetValue(6, FightingInfo.MasterInfo.mcs.fDmgDecPercent );
							//pResult->GetValue(iIndex++, FightingInfo.iSeed );
							//pResult->GetValue(iIndex++, data.level );
							//pResult->GetValue(iIndex++, data.type );
							//pResult->GetValue(iIndex++, FightingInfo.iSceneStar);
							//pResult->GetValue(iIndex++, data.mcs.iMaxHP );
							//pResult->GetValue(iIndex++, data.mcs.iMaxMP );
							//pResult->GetValue(iIndex++, data.mcs.iStrength );
							//pResult->GetValue(iIndex++, data.mcs.iAgility );
							//pResult->GetValue(iIndex++, data.mcs.iIntellect);
							//pResult->GetValue(iIndex++, data.mcs.iSpirit );
							//pResult->GetValue(iIndex++, data.mcs.fHitRate );
							//pResult->GetValue(iIndex++, data.mcs.fDodgeRate );
							//pResult->GetValue(iIndex++, data.mcs.fCritRate );
							//pResult->GetValue(iIndex++, data.mcs.fResilient );
							//pResult->GetValue(iIndex++, data.mcs.fDmgIncPercent );
							//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
							//pResult->GetValue(iIndex++, data.mcs.iAttack );
							//pResult->GetValue(iIndex++, data.mcs.iDefence );
							//pResult->GetValue(iIndex++, data.position);
							//pResult->GetValue(iIndex++, data.Skills[0].iID );
							//pResult->GetValue(iIndex++, data.Skills[1].iID );
							//pResult->GetValue(iIndex++, data.Skills[2].iID );
							//pResult->GetValue(iIndex++, data.Skills[3].iID );
							//pResult->GetValue(iIndex++, data.mcs.iHPRecover );
							//pResult->GetValue(iIndex++, data.mcs.iMPRecover );
							G_GetFightingRecord(pResult,data,FightingInfo.iSceneStar,FightingInfo.iSeed,iIndex,dwUserID);

							FightingInfo.FightingData.units[data.position%(POST_COUNT/2)]=data;
							FightingInfo.FightingData.dwUserID = dwUserID;
							fiList.push_back(FightingInfo);
						}
					}
					pResult->Release();
				}else
				{
					G_DBError(m_dbConnector, m_id);
					return;
				}
				
			}
		}
	}
	
	std::list<MSC_Fighting_Info>::iterator itend=fiList.end();
	for(std::list<MSC_Fighting_Info>::iterator it=fiList.begin();it != itend;++it)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_FightGetSkill");

		m_dbConnector.AddParm("_aid", (*it).iWorldSceneID);
		m_dbConnector.AddParm("_fid", (*it).iSceneID);
		m_dbConnector.AddParm("_did", (*it).iDifficulty);
		if(m_dbConnector.Exec())
		{
			CT_INT32 iNum=0;
			CT_INT32 iPos1=0;
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					CT_INT32 iSkillID=0;
					CT_INT32 iPos2=0;
					pResult->GetValue(0,iSkillID);
					pResult->GetValue(1,iPos2);

					if(iPos1 != iPos2)
					{
						iNum=0;
						iPos1=iPos2;
					}
					(*it).FightingData.units[iPos2%(POST_COUNT/2)].SkillEnuip.iSkillEquip[iNum%SKILL_ENUIP_COUNT]=iSkillID;
					iNum++;

				}
			}
			pResult->Release();
		}else
		{
			G_DBError(m_dbConnector, m_id);
			return;
		}

		CCFightingRecord::getSingleton().InsertRector((*it),(*it).iWorldSceneID,(*it).iSceneID,(*it).iDifficulty);
	}
	return;
}

CT_VOID CCDataBaseThread::DisposeTowerFightRecord()
{
	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerFightExampleGet");

	std::multimap<CT_INT32,MSC_TowerCheckPoint_Fighting> CheckPointMap;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			MSC_TowerCheckPoint_Fighting FightingInfo={0};
			CT_INT32 iCPID=0;
			CT_INT32 iFID = 0;
			CT_INT32 iFID_New = 0;
			CT_DWORD dwUID=0;
			CT_INT32 iSeed=0;
			CT_INT32 iScore=0;
			CT_INT32 iPositionMax = POST_COUNT/2;
			if (pResult->MoveNextRow())
			{
				while (CT_TRUE)
				{
					CT_INT32 iIndex=0;
					CombatUnitData data={0};
					pResult->GetValue(iIndex++, iCPID);
					pResult->GetValue(iIndex++, iFID_New);
					//pResult->GetValue(iIndex++, dwUID);
					//pResult->GetValue(iIndex++, data.name,sizeof(data.name) );
					//pResult->GetValue(iIndex++, data.mcs.fHPPersent);
					//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
					//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
					//pResult->GetValue(iIndex++, iSeed);
					//pResult->GetValue(iIndex++, data.level);
					//pResult->GetValue(iIndex++, data.type);
					//pResult->GetValue(iIndex++, iScore);
					//pResult->GetValue(iIndex++, data.mcs.iMaxHP );
					//pResult->GetValue(iIndex++, data.mcs.iMaxMP );
					//pResult->GetValue(iIndex++, data.mcs.iStrength );
					//pResult->GetValue(iIndex++, data.mcs.iAgility );
					//pResult->GetValue(iIndex++, data.mcs.iIntellect);
					//pResult->GetValue(iIndex++, data.mcs.iSpirit );
					//pResult->GetValue(iIndex++, data.mcs.fHitRate );
					//pResult->GetValue(iIndex++, data.mcs.fDodgeRate );
					//pResult->GetValue(iIndex++, data.mcs.fCritRate );
					//pResult->GetValue(iIndex++, data.mcs.fResilient );
					//pResult->GetValue(iIndex++, data.mcs.fDmgIncPercent );
					//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
					//pResult->GetValue(iIndex++, data.mcs.iAttack );
					//pResult->GetValue(iIndex++, data.mcs.iDefence );
					//pResult->GetValue(iIndex++, data.position);
					//pResult->GetValue(iIndex++, data.Skills[0].iID);
					//pResult->GetValue(iIndex++, data.Skills[1].iID);
					//pResult->GetValue(iIndex++, data.Skills[2].iID);
					//pResult->GetValue(iIndex++, data.Skills[3].iID);
					//pResult->GetValue(iIndex++, data.mcs.iHPRecover);
					//pResult->GetValue(iIndex++, data.mcs.iMPRecover);

					G_GetFightingRecord(pResult,data,iScore,iSeed,iIndex,dwUID);

					if ((FightingInfo.iCPID != 0 && iCPID != FightingInfo.iCPID) || (iFID !=0 && iFID != iFID_New))	//新的战斗
					{
						CheckPointMap.insert(std::multimap<CT_INT32,MSC_TowerCheckPoint_Fighting>::value_type(iFID,FightingInfo));
						//CCTowerResManager::getSingleton().InitTowerFightExample(FightingInfo,iFID);
						memset(&FightingInfo,0, sizeof(MSC_TowerCheckPoint_Fighting));
					}
					
					FightingInfo.FightingData.dwUserID = dwUID;
					FightingInfo.FightingData.units[data.position%iPositionMax] = data;
					FightingInfo.dwUserID = dwUID;
					FightingInfo.iCPID = iCPID;
					FightingInfo.iScore = iScore;
					FightingInfo.iSeed = iSeed;
					iFID = iFID_New;

					if (!pResult->MoveNextRow())
					{
						CheckPointMap.insert(std::multimap<CT_INT32,MSC_TowerCheckPoint_Fighting>::value_type(iFID,FightingInfo));
						//CCTowerResManager::getSingleton().InitTowerFightExample(FightingInfo,iFID);
						break;
					}
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_id);
		return;
	}

	std::multimap<CT_INT32,MSC_TowerCheckPoint_Fighting>::iterator itend=CheckPointMap.end();
	for(std::multimap<CT_INT32,MSC_TowerCheckPoint_Fighting>::iterator it=CheckPointMap.begin();it != itend;++it)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_TowerFightSkillGet");
		m_dbConnector.AddParm("_cpid", (*it).second.iCPID );
		m_dbConnector.AddParm("_fid", (*it).first );

		MSC_DB_MSG	InsertMSG = {0};
		if(m_dbConnector.Exec())
		{
			CT_INT32 iNum=0;
			CT_INT32 iPos1=0;
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					CT_INT32 iSkillID=0;
					CT_INT32 iPos2=0;
					pResult->GetValue(0,iSkillID);
					pResult->GetValue(1,iPos2);
					if(iPos1 != iPos2)
					{
						iNum=0;
						iPos1=iPos2;
					}
					(*it).second.FightingData.units[iPos2%(POST_COUNT/2)].SkillEnuip.iSkillEquip[iNum%SKILL_ENUIP_COUNT]=iSkillID;
					iNum++;

				}
			}
			pResult->Release();
		}else
		{
			G_DBError(m_dbConnector, m_id);
			return;
		}

		MSC_TowerCheckPoint_Fighting tcp={0};
		memcpy(&tcp,&((*it).second),sizeof(tcp));
		CCTowerResManager::getSingleton().InitTowerFightExample(tcp,(*it).first);
	}
}

//插入排行榜信息
CT_VOID CCDataBaseThread::DisposeTopInfo()
{
	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPGetRanking");

	if(m_dbConnector.Exec())
	{
		CT_INT32 iUserCount=0;
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				LookUserInfo Ranking={0};
				pResult->GetValue(0, Ranking.dwUserID );

				//CCTop::getSingleton().InsertUserID(iUserCount,Ranking.dwUserID);
				iUserCount++;
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_id);
		return;
	}

	//查找用户信息
	CT_DWORD dbThreadID = GAME_THREAD_DATABASE;
	CT_CHAR cBuf=0;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOP_INFO,&cBuf,sizeof(cBuf));

	return;
}

//读取公告信息
CT_VOID CCDataBaseThread::DisposeAnnouncement()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_Announcement");

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while(pResult->MoveNextRow())
			{
				Announcement anno = {0};
				pResult->GetValue(0, anno.iIndex);
				pResult->GetValue(1, anno.cContent, ANNOUNCEMENT_CONTENT_LEN);
				pResult->GetValue(2, anno.iFlag);
				
				CCBroadcastScriptSetting::getSingleton().AnnouncementInit(anno.iIndex, anno);
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_id);
	}
}


