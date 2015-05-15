#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CActive.h"
#include "CUser.h"
#include "CNetInterface.h"
#include "CLogMeg.h"
#include "CThreadMes.h"

template<> CCActive* CCSingleton<CCActive>::ms_Singleton=0;

CCActive::CCActive()
{
	m_ActiveDscMap.clear();
	m_ActiveEventVector.clear();
	m_TurntableMap.clear();
	m_RewardMap.clear();
	m_TimeActiveStatusMap.clear();
	TurntableItem tti = {0};
	m_TurntableMap.insert(TurntableMap::value_type(1,tti));
	m_TurntableMap.insert(TurntableMap::value_type(2,tti));
	m_TurntableMap.insert(TurntableMap::value_type(3,tti));
}

CCActive::~CCActive()
{
	m_ActiveEventVector.clear();
	m_ActiveDscMap.clear();
	m_RewardMap.clear();
	m_TurntableMap.clear();
	m_TimeActiveStatusMap.clear();
}

//插入活动描述
CT_VOID	CCActive::InsterActiveDsc(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	m_ActiveDscMap.clear();
	for(CT_INT32 i=0;i<iLen/sizeof(ActiveDsc);++i)
	{
		ActiveDsc ad={0};
		memcpy(&ad,pBuf+i*sizeof(ActiveDsc),sizeof(ActiveDsc));
		m_ActiveDscMap.insert(ActiveDscMap::value_type(ad.iType,ad));
	}
	return;
}

CT_VOID	CCActive::InsertReward(G_ActiveEventReward &reward)
{
	_AUTOMUTEX(&m_mutex);
	m_RewardMap.insert(ActiveRewardMap::value_type(reward.iAEID, reward));
}

CT_BOOL CCActive::GetReward(CT_INT32 iRewardID, G_ActiveEventReward *pReward)
{
	_AUTOMUTEX(&m_mutex);
	ActiveRewardMap::iterator it = m_RewardMap.find(iRewardID);
	if (it != m_RewardMap.end())
	{
		memcpy(pReward, &(it->second), sizeof(G_ActiveEventReward));	//结构相同
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCActive::GetReward(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	//CMD_G_ActiveEventRewardEx aere = {0};
	ActiveRewardMap::iterator it = m_RewardMap.begin();
	ActiveRewardMap::iterator it_end = m_RewardMap.end();
	CT_INT32 iCount = bufLen / sizeof(CMD_G_ActiveEventRewardEx);
	CT_INT32 i=0;
	if (iCount < 1)
	{
		return CT_FALSE;
	}
	for (; it != it_end && i < iCount; ++it,++i)
	{
		memcpy(pBuf+i*sizeof(CMD_G_ActiveEventRewardEx), &(it->second), sizeof(CMD_G_ActiveEventRewardEx));	//结构相同
	}
	bufLen = i * sizeof(CMD_G_ActiveEventRewardEx);
	return CT_TRUE;

}

//获取活动描述
CT_BOOL	CCActive::GetActiveDsc(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_mutex);
	if(m_ActiveDscMap.size() == 0)return CT_FALSE;

	CT_INT32 iSize=0;
	ActiveDscMap::iterator itend=m_ActiveDscMap.end();
	for(ActiveDscMap::iterator it=m_ActiveDscMap.begin();it != itend;++it)
	{
		memcpy(pBuf+iSize,&((*it).second),sizeof(ActiveDsc));
		iSize+=sizeof(ActiveDsc);
	}
	iLen=iSize;
	return CT_TRUE;
}

CT_VOID	CCActive::UpdateActive(ActiveEventVector &v_active,MSC_DB_ActiveEventChange &activeEventDelete, MSC_DB_ActiveEventChange &activeEventAdd)
{
	_AUTOMUTEX(&m_mutex);

	std::size_t i,j;
	//找出删除的活动，删除数据库中已有活动信息,同样需要删除内存数据
	for (i=0;i<m_ActiveEventVector.size();++i)
	{
		for (j=0;j<v_active.size();++j)
		{
			if (m_ActiveEventVector[i]==v_active[j])
			{
				break;
			}	
		}
		if (j==v_active.size())
		{
			if (ACTIVE_TURNTABLE == GetActiveType(m_ActiveEventVector[i]))
			{
				DelTurntableActive(m_ActiveEventVector[i]);
			}
			activeEventDelete.iActive[activeEventDelete.iNum++]=m_ActiveEventVector[i];
		}
	}
//	CCUserManager::getSingleton().ActiveDeletePush(activeEventDelete);
	//找出新增活动，推送客户端
//	MSC_DB_ActiveEventChange  activeAdd = {0};
	for (i=0;i<v_active.size();++i)
	{
		for (j=0;j<m_ActiveEventVector.size();++j)
		{
			if (m_ActiveEventVector[j]==v_active[i])
			{
				break;
			}	
		}
		if (j==m_ActiveEventVector.size())
		{
			if (ACTIVE_TURNTABLE == GetActiveType(v_active[i]))
			{
				NewTurntableActive(v_active[i]);
			}
			activeEventAdd.iActive[activeEventAdd.iNum++]=v_active[i];
		}
	}
//	CCUserManager::getSingleton().ActiveAddPush(activeAdd);
	G_SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_RES_OCCUPY,MSC_SUB_RESACTVIE_OPEN,0,0);
	G_SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_WORLD_BOSS,MSC_SUB_BOSSACTVIE_OPEN,0,0);

	m_ActiveEventVector.clear();
	m_ActiveEventVector=v_active;
	m_RewardMap.clear();
}

CT_VOID CCActive::GetServerActive(MSC_DB_ActiveEventChange &activeList)
{
	_AUTOMUTEX(&m_mutex);
	for (CT_DWORD i=0;i<m_ActiveEventVector.size();++i)
	{
		activeList.iActive[activeList.iNum++] = m_ActiveEventVector[i];
	}
}

CT_VOID CCActive::ActiveProcess(CT_INT32 iType,CT_INT32 iQuanity,CCUser *pUser, CT_INT32 iAID)
{
	_AUTOMUTEX(&m_mutex);
	for (CT_DWORD i=0;i<m_ActiveEventVector.size();++i)
	{
		G_ActiveEventData	activeData = {0};
		if (!G_GetActiveEvent(m_ActiveEventVector[i],&activeData))
		{
			CCLogMeg::getSingleton().WriteError("CCActive::ActiveProcess,the active %d data error",m_ActiveEventVector[i]);
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
			return;
		}
		if (activeData.iType==iType && (0==iAID || activeData.iAEID == iAID))
		{
			pUser->m_userActive.UpdateActiveStatus(m_ActiveEventVector[i],activeData.iType,iQuanity,activeData);
		}
	}
}

CT_FLOAT CCActive::GetMarketDiscount(CT_INT32 iGoodsID)
{
	_AUTOMUTEX(&m_mutex);
	for (CT_DWORD i=0;i<m_ActiveEventVector.size();++i)
	{
		G_ActiveEventData	activeData = {0};
		if (!G_GetActiveEvent(m_ActiveEventVector[i],&activeData))
		{
			CCLogMeg::getSingleton().WriteError("CCActive::ActiveProcess,the active %d data error",m_ActiveEventVector[i]);
			return CT_FLOAT(1);
		}
		if (activeData.iType==ACTIVE_MARKET_DISCOUNT)
		{
			if (activeData.iProperty1==iGoodsID)
			{
				return activeData.iProperty2;
			}
		}
	}
	return CT_FLOAT(1);
}

CT_INT32 CCActive::GetActiveByType(CT_INT32 iType, CT_INT32 *ActiveList)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 n=0;
	for (CT_DWORD i=0;i<m_ActiveEventVector.size() && n < 100;++i)
	{
		G_ActiveEventData	activeData = {0};
		if (!G_GetActiveEvent(m_ActiveEventVector[i],&activeData))
		{
			CCLogMeg::getSingleton().WriteError("CCActive::ActiveProcess,the active %d data error",m_ActiveEventVector[i]);
			return -1;
		}
		if (activeData.iType==iType)
		{
			ActiveList[n++] = m_ActiveEventVector[i];
		}
	}
	return n;
}

CT_INT32 CCActive::GetActiveType(CT_INT32 iAID)
{
	_AUTOMUTEX(&m_mutex);
	G_ActiveEventData	activeData = {0};
	if (G_GetActiveEvent(iAID,&activeData))
	{
		return activeData.iType;
	}
	return 0;
}

//删除转盘活动
CT_VOID CCActive::DelTurntableActive(CT_INT32 iAID)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 iTurntableID=0;
	G_ActiveEventData	activeData = {0};
	if (G_GetActiveEvent(iAID,&activeData))
	{
		iTurntableID = activeData.iProperty1;
		if (m_TurntableMap.find(iTurntableID) != m_TurntableMap.end())
		{
			m_TurntableMap[iTurntableID].iOpen = 0;
		}
	}
}

//增加转盘活动
CT_VOID CCActive::NewTurntableActive(CT_INT32 iAID)
{
	_AUTOMUTEX(&m_mutex);

	if (GetActiveType(iAID) != ACTIVE_TURNTABLE)
	{
		return;
	}

	CT_INT32 iTurntableID=0;
	G_ActiveEventData	activeData = {0};
	if (G_GetActiveEvent(iAID,&activeData))
	{
		iTurntableID = activeData.iProperty1;
		if (m_TurntableMap.find(iTurntableID)!=m_TurntableMap.end())
		{
			m_TurntableMap[iTurntableID].iOpen = 1;
		}
	}
}

CT_VOID CCActive::RefTurntable()
{
	_AUTOMUTEX(&m_mutex);
	for (TurntableMap::iterator it = m_TurntableMap.begin(); it!=m_TurntableMap.end();++it)
	{
		CT_INT32 iDupCount = 0;
		for (CT_INT32 iPos=0; iPos<TURNTABLE_ITEM_COUNT; iPos++)
		{
			CT_INT32 i=0;
			CT_INT32 iWeight=0;
			G_TurntableRelation ttrList[100] = {0};
			G_GetTurntableRelationList(ttrList, it->first, iPos+1);
			while(i<100)
			{
				if (0 == ttrList[i].iIndex)
				{
					break;
				}
				ttrList[i].iWeights += iWeight;
				iWeight = ttrList[i].iWeights;
				++i;
			}
			if (iWeight <= 0)
			{
				continue;;
			}
			CT_BOOL bDup = CT_FALSE;
			CT_INT32  iRandNum = rand()%iWeight + 1;
			CT_INT32  iIndex = GetPosByRandNum(ttrList,0,i,iRandNum);
			for (CT_INT32 i=0; i<iPos; ++i)
			{
				if (ttrList[iIndex].iRewardID == it->second.iRewardID[i])
				{
					bDup = CT_TRUE;
					i = iPos;
				}
			}
			if (bDup && iDupCount < 20)
			{
				--iPos;
				++iDupCount;
				continue;
			}
			iDupCount = 0;
			G_TurntableReward ttr = {0};
			G_GetTurntableReward(ttrList[iIndex].iRewardID, &ttr);
			it->second.iRewardID[iPos] = ttr.iRewardID;
			it->second.gReward[iPos].iType = ttr.iType;
			it->second.gReward[iPos].iID = ttr.iID;
			it->second.gReward[iPos].iCount = ttr.iCount;
			it->second.iWeight[iPos] = ttr.iWeights;
			it->second.iFakeProID[iPos] = ttr.iFakeProID;
		}
	}
}

CT_INT32 CCActive::GetPosByRandNum(G_TurntableRelation *ttrList,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand)
{
	//在此不得加锁, 否则会死锁
	if (begin>=end)
	{
		return begin;
	}
	CT_INT32 mid = (begin+end)/2;
	if (ttrList[mid].iWeights>=iRand)
	{
		return GetPosByRandNum(ttrList,begin,mid,iRand);
	}
	else
	{
		return GetPosByRandNum(ttrList,mid+1,end,iRand);
	}
}

CT_VOID	CCActive::GetTurntableItem(CT_INT32 iTurntableID, TurntableItem *Item)
{
	_AUTOMUTEX(&m_mutex);
	TurntableMap::iterator it = m_TurntableMap.find(iTurntableID);
	if (it != m_TurntableMap.end())
	{
		memcpy(Item, &(it->second), sizeof(TurntableItem));
	}
}

CT_VOID	CCActive::SetTurntableItem(MSC_DB_TurntableRelation &ttr)
{
	_AUTOMUTEX(&m_mutex);
	TurntableMap::iterator it = m_TurntableMap.find(ttr.iTurntableID);
	if (it != m_TurntableMap.end())
	{
		if (ttr.iPos <= TURNTABLE_ITEM_COUNT && ttr.iPos > 0)
		{
			it->second.iWeight[ttr.iPos-1] = ttr.iWeight;
			it->second.iRewardID[ttr.iPos-1] = ttr.iRewardID;
			it->second.iFakeProID[ttr.iPos-1] = ttr.iFakeProID;
			it->second.gReward[ttr.iPos-1].iType = ttr.iType;
			it->second.gReward[ttr.iPos-1].iID = ttr.iID;
			it->second.gReward[ttr.iPos-1].iCount = ttr.iCount;
		}
	}
}

CT_VOID	CCActive::SetTimeActiveStatus(CT_INT32 iType, CT_INT32 iAEID, CT_INT64 iTime, CT_INT32 iStatus)
{
	_AUTOMUTEX(&m_mutex);
	TimeActiveStatusMap::iterator it = m_TimeActiveStatusMap.find(iType);
	if (it != m_TimeActiveStatusMap.end())
	{
		it->second.iAEID = iAEID;
		it->second.iTime = iTime;
		it->second.iStatus = iStatus;
	}
	else
	{
		TimeActiveStatus tas = {0};
		tas.iAEID = iAEID;
		tas.iTime = iTime;
		tas.iStatus = iStatus;
		m_TimeActiveStatusMap.insert(TimeActiveStatusMap::value_type(iType, tas));
	}
	if (1 == iStatus)
	{
		//删除数据库
		MSC_DB_ActiveEventChange    activeDelte = {0};
		activeDelte.iNum = 1;
		activeDelte.iActive[0] = iAEID;
		G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_DB_ACTIVE_DATA, MSC_SUB_ACTIVE_DB_SYS,(CT_CHAR*)&activeDelte, sizeof(activeDelte));
	}
}

CT_VOID	CCActive::GetTimeActiveStatus(CT_INT32 iType, CT_INT32 &iAEID, CT_INT64 &iTime, CT_INT32 &iStatus)
{
	_AUTOMUTEX(&m_mutex);
	TimeActiveStatusMap::iterator it = m_TimeActiveStatusMap.find(iType);
	if (it != m_TimeActiveStatusMap.end())
	{
		iAEID = it->second.iAEID;
		iTime = it->second.iTime;
		iStatus = it->second.iStatus;
	}
	else
	{
		iAEID = 0;
		iTime = -1;
		iStatus = 0;
	}
}
