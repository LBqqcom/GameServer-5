/****************************************************
	created:	2014/07/23		18:02
	file base:	DBIslandEvent
	file ext:	cpp
	author:		zhouhuan

	purpose:
*****************************************************/

#include "CDBIslandEvent.h"
#include "CThreadMes.h"
#include "CDBLogMes.h"
#include "CDBConnector.h"
#include "CUserManager.h"

#define ISLAND_MAX_GETTIMES	(5)

CCDBIslandEvent::CCDBIslandEvent(CCGameMesManager *pMesManager, CT_DWORD id, CCDBConnector &dbConnect): CCGameMesEvent(pMesManager, id),m_dbConnector(dbConnect)
{
	RegistMessage(MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_UPDATE);
	RegistMessage(MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_GIVEUID);
	RegistMessage(MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_SCOREREWARD);
	RegistMessage(MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_GIVETIMES);
	RegistMessage(MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_UPDATE_FTMEMBER_SCORE);
	RegistMessage(MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_ONCLOCK);

}

CCDBIslandEvent::~CCDBIslandEvent()
{
}

CT_VOID CCDBIslandEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	if (mainID == MSC_MAIN_DB_DRAGONISLAND)
	{
		switch (subID)
		{
		case MSC_SUB_DB_ISLAND_UPDATE:
			{
				if (len == sizeof(MSC_DB_IslandData))
				{
					UpdateIslandData(pBuf,len);
				}
				break;
			}
		case MSC_SUB_DB_ISLAND_GIVEUID:
			{
				if (len == sizeof(MSC_DB_IslandGiveUid))
				{
					AddIslandGiveTimes(pBuf,len);
				}
				break;
			}
		case MSC_SUB_DB_ISLAND_SCOREREWARD:
			{
				if (len == sizeof(MSC_DB_IslandScoreReward))
				{
					AddIslandScoreReward(pBuf,len);
				}
				break;
			}
		case MSC_SUB_DB_ISLAND_GIVETIMES:
			{
				if (len == sizeof(MSC_DB_IslandGiveTimes))
				{
					DisposeGiveTimes(pBuf,len);
				}
				break;
			}
		case MSC_SUB_UPDATE_FTMEMBER_SCORE:
			{
				if (len == sizeof(MSC_DB_IslandFTMemberScore))
				{
					UpdateFTMemberScore(pBuf,len);
				}
				break;
			}
		case MSC_SUB_DB_ISLAND_ONCLOCK:
			{
				UpdateIslandOnClock();
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("CCDBIslandEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
				break;
			}
		}
	}
}

CT_VOID CCDBIslandEvent::OnClock(CT_DWORD timeID)
{

}

CT_VOID CCDBIslandEvent::OnTime(CT_DWORD timeID)
{

}

CT_VOID CCDBIslandEvent::UpdateIslandData(CT_CHAR* pBuf,CT_INT32 iLen)
{
	MSC_DB_IslandData* pIslandData = (MSC_DB_IslandData*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UpdateIslandData");
	m_dbConnector.AddParm("_uid", pIslandData->dwUserID);
	m_dbConnector.AddParm("_point", pIslandData->iPoint);
	m_dbConnector.AddParm("_eventid", pIslandData->iEventID);
	m_dbConnector.AddParm("_eventstatus", pIslandData->iEventStatus);
	m_dbConnector.AddParm("_throwtimes", pIslandData->iThrowTimes);
	m_dbConnector.AddParm("_starttimes", pIslandData->iStartTimes);
	m_dbConnector.AddParm("_startchest", pIslandData->iStartChest);
	m_dbConnector.AddParm("_gettimes", pIslandData->iGetTimes);
	m_dbConnector.AddParm("_ftid",	pIslandData->iFT_ID);
	m_dbConnector.AddParm("_score", pIslandData->iScore);
	m_dbConnector.AddParm("_open",	pIslandData->iIsOpen);
	m_dbConnector.AddParm("_rewardtimes", pIslandData->iRewardTimes);
	m_dbConnector.AddParm("_step", pIslandData->iStep);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pIslandData->dwUserID,"SP_UpdateIslandData","_uid:%i ",pIslandData->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBIslandEvent::AddIslandGiveTimes(CT_CHAR* pBuf,CT_INT32 iLen)
{
	MSC_DB_IslandGiveUid* pGiveUid = (MSC_DB_IslandGiveUid*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_AddGiveUid");
	m_dbConnector.AddParm("_uid",	pGiveUid->dwUserID);
	m_dbConnector.AddParm("_fuid",	pGiveUid->aGiveUid);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(pGiveUid->dwUserID,"SP_AddGiveUid","_uid:%i ",pGiveUid->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBIslandEvent::AddIslandScoreReward(CT_CHAR* pBuf,CT_INT32 iLen)
{
	MSC_DB_IslandScoreReward* pScoreReward = (MSC_DB_IslandScoreReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_AddScoreReward");
	m_dbConnector.AddParm("_uid",	pScoreReward->dwUserID);
	m_dbConnector.AddParm("_ftchestid",	pScoreReward->aScoreReward);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(pScoreReward->dwUserID,"SP_AddScoreReward","_uid:%i ",pScoreReward->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBIslandEvent::DisposeGiveTimes(CT_CHAR* pBuf, CT_INT32 iLen)
{
	MSC_DB_IslandData mid = {0};
	MSC_DB_IslandGiveTimes* pGiveTimes = (MSC_DB_IslandGiveTimes*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetDragonIsland");
	m_dbConnector.AddParm("_uid",	pGiveTimes->dwFUID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, mid.iPoint);
				pResult->GetValue(2, mid.iEventID);
				pResult->GetValue(3, mid.iEventStatus);
				pResult->GetValue(4, mid.iThrowTimes);
				pResult->GetValue(5, mid.iStartTimes);
				pResult->GetValue(6, mid.iStartChest);
				pResult->GetValue(7, mid.iGetTimes);
				pResult->GetValue(8, mid.iFT_ID);
				pResult->GetValue(9, mid.iScore);
				pResult->GetValue(10,mid.iIsOpen);
				pResult->GetValue(11,mid.iRewardTimes);
				pResult->GetValue(12,mid.iStep);

				pGiveTimes->iGetTimes = mid.iGetTimes;
				
			}
		}
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(pGiveTimes->dwFUID,"SP_GetDragonIsland","_uid:%d\n%s%d",pGiveTimes->dwFUID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
	}

	CCUser* pUser;
	if (CCUserManager::getSingleton().Find(pGiveTimes->dwUserID, &pUser))
	{
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_ISLAND_EVENT, MSC_SUB_GIVETIMES_OUTLINE,(CT_CHAR*)pGiveTimes,sizeof(MSC_DB_IslandGiveTimes));
	}

	if (mid.iGetTimes < ISLAND_MAX_GETTIMES)
	{
		mid.dwUserID = pGiveTimes->dwFUID;
		mid.iGetTimes += 1;
		UpdateIslandData((CT_CHAR*)&mid, sizeof(mid));
	}
	
}

CT_VOID CCDBIslandEvent::UpdateFTMemberScore(CT_CHAR* pBuf, CT_INT32 iLen)
{
	MSC_DB_IslandFTMemberScore* ftMember = (MSC_DB_IslandFTMemberScore*)pBuf;
	
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UpdateFTMemberScore");
	m_dbConnector.AddParm("_uid",	ftMember->dwUserID);
	m_dbConnector.AddParm("_teamid", ftMember->iTeamID);
	m_dbConnector.AddParm("_score",	ftMember->iScore);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(ftMember->dwUserID,"SP_UpdateFTMemberScore","_uid:%i ",ftMember->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBIslandEvent::UpdateIslandOnClock()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UpdateIslandOnclock");

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}