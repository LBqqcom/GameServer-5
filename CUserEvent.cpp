#include <ctime>
#include "CUserEvent.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CMD_Game.h"
#include "CNetInterface.h"
#include "CRandEventManager.h"

CCUserEvent::CCUserEvent(void)
{
	m_buildingEventMap.clear();
	m_workerEventMap.clear();
	m_treasureEventMap.clear();
	m_petEventMap.clear();
	m_fightMap.clear();
}

CCUserEvent::~CCUserEvent(void)
{
	m_buildingEventMap.clear();
	m_workerEventMap.clear();
	m_treasureEventMap.clear();
	m_petEventMap.clear();
	m_fightMap.clear();
}

//�����û�����
CT_VOID CCUserEvent::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_buildingEventMap.clear();
	m_workerEventMap.clear();
	m_treasureEventMap.clear();
	m_petEventMap.clear();
	m_fightMap.clear();
}

//���ҽ����¼�
CT_BOOL CCUserEvent::FindBuildingEventByIndex(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex); 
	UEEventMap::iterator it = m_buildingEventMap.find(iIndex);
	if (it != m_buildingEventMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}


//�����¼�
CT_BOOL   CCUserEvent::FindPetEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex); 
	UEEventMap::iterator it = m_petEventMap.find(iIndex);
	if (it != m_petEventMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//�����¼�
CT_BOOL   CCUserEvent::FindFightEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	UEEventMap::iterator it = m_fightMap.find(iIndex);
	if (it != m_fightMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//�����¼�
CT_BOOL   CCUserEvent::FindWorkerEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	UEEventMap::iterator it = m_workerEventMap.find(iIndex);
	if (it != m_workerEventMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//�����¼�
CT_BOOL   CCUserEvent::FindTreasureEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex); 
	UEEventMap::iterator it = m_treasureEventMap.find(iIndex);
	if (it != m_treasureEventMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//�õ��¼�
CT_BOOL CCUserEvent::GetEvents(CT_CHAR* pData, CT_INT32& dataLen)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 len = (CT_INT32)(sizeof(CMD_G_Event) * (m_workerEventMap.size() + m_treasureEventMap.size() +
		                                             m_fightMap.size() + m_buildingEventMap.size() + m_petEventMap.size()  ));
	if (dataLen < len )
	{
		return CT_FALSE;
	}

	int i=0;
	UEEventMap::iterator it    = m_workerEventMap.begin();
	UEEventMap::iterator itEnd = m_workerEventMap.end();
	for (; it != itEnd; ++it, ++i)
	{
		CMD_G_Event we = {0};
		we.dwUserID = m_pUser->GetUserID();
		we.iIndex = (*it).second.iIndex;
		we.et = EVENT_WORKER;
		memcpy(pData + i*sizeof(CMD_G_Event), &we, sizeof(we));
	}

	it = m_fightMap.begin();
	itEnd = m_fightMap.end();
	for (; it != itEnd; ++it, ++i)
	{
		CMD_G_Event we = {0};
		we.dwUserID = m_pUser->GetUserID();
		we.iIndex = (*it).second.iIndex;
		we.et = EVENT_FIGHT;
		memcpy(pData + i*sizeof(CMD_G_Event), &we, sizeof(we));
	}

	it = m_treasureEventMap.begin();
	itEnd = m_treasureEventMap.end();
	for (; it != itEnd; ++it, ++i)
	{
		CMD_G_Event we = {0};
		we.dwUserID = m_pUser->GetUserID();
		we.iIndex = (*it).second.iIndex;
		we.et = EVENT_TREASURE;
		memcpy(pData + i*sizeof(CMD_G_Event), &we, sizeof(we));
	}

	it = m_petEventMap.begin();
	itEnd = m_petEventMap.end();
	for (; it != itEnd; ++it, ++i)
	{
		CMD_G_Event we = {0};
		we.dwUserID = m_pUser->GetUserID();
		we.iIndex = (*it).second.iIndex;
		we.et = EVENT_PET;
		memcpy(pData + i*sizeof(CMD_G_Event), &we, sizeof(we));
	}

	it    = m_buildingEventMap.begin();
	itEnd = m_buildingEventMap.end();
	for (; it != itEnd; ++it, ++i)
	{
		CMD_G_Event we = {0};
		we.dwUserID = m_pUser->GetUserID();
		we.iIndex = (*it).second.iIndex;
		we.et = EVENT_BUILDING;
		memcpy(pData + i*sizeof(CMD_G_Event), &we, sizeof(we));
	}

	dataLen = len;
	return CT_TRUE;
}

//�����¼�(��ʱ�������µ��¼�)
CT_VOID CCUserEvent::UpdateEvent()
{
	_AUTOMUTEX(m_pMutex);
	UpdateEvent(m_workerEventMap,   EVENT_WORKER);
	UpdateEvent(m_treasureEventMap, EVENT_TREASURE);
	UpdateEvent(m_petEventMap,      EVENT_PET);
	UpdateEvent(m_fightMap,         EVENT_FIGHT);
	UpdateEvent(m_buildingEventMap, EVENT_BUILDING);

	G_ValueRange vr = {0};
	if (!G_GetValueRange(VALUE_TYPE_MAXEVENTNUM,  &vr))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEvent::UpdateEvent(): fail to G_GetValueRange(VALUE_TYPE_MAXEVENTNUM,  &vr)");
		return;
	}

	CT_INT32 num = m_buildingEventMap.size() + m_fightMap.size() + m_petEventMap.size() + m_workerEventMap.size() + m_treasureEventMap.size();
	if( num > (CT_INT32)vr.fMaxVal) //�����¼��޶�
	{
		return; 
	}

	memset(&vr, 0 ,sizeof(vr));
	if (!G_GetValueRange(VALUE_TYPE_EVENTNUMONCE, &vr))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEvent::UpdateEvent(): fail to G_GetValueRange(VALUE_TYPE_EVENTNUMONCE,  &vr)");
		return;
	}

	for (CT_INT32 i=0; i<(CT_INT32)vr.fMaxVal; ++i)
	{
		RandEvent re = {0};
		if(CCRandEventManager::getSingleton().GetOneEvent(re))
		{
			AssignEvent(re);
		}
	}
}

//�����¼�
CT_VOID CCUserEvent::UpdateEvent(UEEventMap& eventMap, ENUM_EventType et)
{
	_AUTOMUTEX(m_pMutex);
	CT_DWORD    userID  = m_pUser->GetUserID();
	CT_LONGLONG timeNow = time(0);
	UEEventMap::iterator it    = eventMap.begin();
	UEEventMap::iterator itEnd = eventMap.end();
	UEEventMap::iterator itcp  = it;
	for (; it != itEnd; )
	{
		itcp = it;
		++itcp;
		if (timeNow - (*it).second.llTimeBegin > (*it).second.iLasting)
		{
			CMD_G_EventTimeOut eto = {0};
			eto.dwUserID = userID;
			eto.iIndex = (*it).second.iIndex;
			eto.et     = et;
			m_pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_TIMEOUT_EVENT, (CT_CHAR*)&eto, sizeof(eto));
			CCLogMeg::getSingleton().WriteInfo("CCUserEvent::UpdateEvent user %d event index %d type %d timeout",userID,eto.iIndex,eto.et);
			eventMap.erase(it);
		}
		it = itcp;
	}
}

//�����¼�
CT_VOID CCUserEvent::AssignEvent(const RandEvent& re)
{
	_AUTOMUTEX(m_pMutex);
	if (re.iEventType == EVENT_PET)
	{
		AssignPetEvent(re);
		return;
	}

	if (re.iEventType == EVENT_BUILDING)
	{
		AssginBuildingEvent(re);
		return;
	}

	if (re.iEventType == EVENT_WORKER)
	{
		if(m_pUser->m_userBase.m_masterData.mvd.stRes.iWorker >  (CT_INT32)m_workerEventMap.size() )
		{
			AssignWorkerEvent(re);
		}
		return;
	}

	if (re.iEventType == EVENT_FIGHT)
	{
		AssginFightingEvent(re);
		return;
	}

	if (re.iEventType == EVENT_TREASURE)
	{
		AssignTreasureEvent(re);
		return;
	}
}

//���䱦���¼�
CT_VOID CCUserEvent::AssignTreasureEvent(const RandEvent& re)
{
	UEEventMap::iterator it = m_treasureEventMap.find(re.iIndex);
	if (it == m_treasureEventMap.end())
	{
		G_Event ge = {0};
		if(CCEventScriptSetting::getSingleton().GetEvent(re.iEventID, ge))
		{
			LastingEvent le = {0};
			le.iIndex		= re.iIndex;
			le.iLasting		= ge.iLasting;
			le.llTimeBegin	= time(0);
			m_treasureEventMap.insert(UEEventMap::value_type(le.iIndex, le));
			
			CCLogMeg::getSingleton().WriteError("����һ�������¼��� uid=%i, index=%i", m_pUser->GetUserID(), re.iIndex);
			CMD_G_Event gte = {0};
			gte.dwUserID = m_pUser->GetUserID();
			gte.iIndex   = re.iIndex;
			gte.et       = EVENT_TREASURE;
			m_pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_EVENT, (CT_CHAR*)&gte, sizeof(gte));
		}
	}
}

//���乤���¼�
CT_VOID CCUserEvent::AssignWorkerEvent(const RandEvent& re)
{
	UEEventMap::iterator it = m_workerEventMap.find(re.iIndex);
	if (it == m_workerEventMap.end())
	{
		G_Event ge = {0};
		if(CCEventScriptSetting::getSingleton().GetEvent(re.iEventID, ge))
		{
			LastingEvent le = {0};
			le.iIndex		= re.iIndex;
			le.iLasting		= ge.iLasting;
			le.llTimeBegin	= time(0);
			m_workerEventMap.insert(UEEventMap::value_type(le.iIndex, le));

			CMD_G_Event gte = {0};
			gte.dwUserID = m_pUser->GetUserID();
			gte.iIndex = re.iIndex;
			gte.et     = EVENT_WORKER;
			m_pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_EVENT, (CT_CHAR*)&gte, sizeof(gte));
		}
	}
}

//��������¼�
CT_VOID CCUserEvent::AssignPetEvent(const RandEvent& re)
{
	UEEventMap::iterator it = m_petEventMap.find(re.iIndex);
	if (it!=m_petEventMap.end())
	{
		return;
	}
	G_Event ge = {0};
	if(CCEventScriptSetting::getSingleton().GetEvent(re.iEventID, ge))
	{
		LastingEvent le = {0};
		le.iIndex		= re.iIndex;
		le.iLasting		= ge.iLasting;
		le.llTimeBegin	= time(0);
		m_petEventMap.insert(UEEventMap::value_type(le.iIndex, le));

		CMD_G_Event gpe = {0};
		gpe.dwUserID = m_pUser->GetUserID();
		gpe.iIndex = re.iIndex;
		gpe.et     = EVENT_PET;
		m_pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_EVENT, (CT_CHAR*)&gpe, sizeof(gpe));
	}
}

CT_VOID CCUserEvent::AssginBuildingEvent(const RandEvent& re)
{
	G_BuildingEventProbability bep = {0};
	if (!CCEventScriptSetting::getSingleton().GetBuildingEvent(re.iIndex, bep))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEvent::AssginBuildingEvent: û�з�����������¼�,�¼�������%i", re.iIndex);
		return ;
	}

	if (m_buildingEventMap.find(re.iIndex  ) != m_buildingEventMap.end())
	{
		return;
	}

	if (!m_pUser->m_userBuilding.FindBuildData(bep.iBuildID))
	{
		return ;
	}

	if (m_pUser->m_userBuilding.FindUpgradeBuildData(bep.iBuildID) || 
		m_pUser->m_userBuilding.FindAutoProductingBuilding(bep.iBuildID) || 
		m_pUser->m_userBuilding.FindBuildingProduct(bep.iBuildID))
	{
		return;
	}

	UEEventMap::iterator it = m_buildingEventMap.begin();  
	UEEventMap::iterator itEnd = m_buildingEventMap.end();
	for (; it != itEnd ; ++it)
	{
		G_BuildingEventProbability be = {0};
		if (CCEventScriptSetting::getSingleton().GetBuildingEvent((*it).second.iIndex, be))
		{
			if (be.iBuildID == bep.iBuildID)  //��ֹͬһ�������ж���¼�
			{
				return; 
			}
		}
	}


	G_Event ge = {0};
	if(CCEventScriptSetting::getSingleton().GetEvent(re.iEventID, ge))
	{
		LastingEvent le = {0};
		le.iIndex		= re.iIndex;
		le.iLasting		= ge.iLasting;
		le.llTimeBegin	= time(0);
		m_buildingEventMap.insert(UEEventMap::value_type(le.iIndex, le));

		CMD_G_Event gbe = {0};
		gbe.dwUserID = m_pUser->GetUserID();
		gbe.iIndex = re.iIndex;
		gbe.et = EVENT_BUILDING;
		m_pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_EVENT, (CT_CHAR*)&gbe, sizeof(gbe));
	}
}

//����ս���¼�
CT_VOID CCUserEvent::AssginFightingEvent(const RandEvent& re)
{
	UEEventMap::iterator it = m_fightMap.find(re.iIndex);
	if (it == m_fightMap.end())
	{
		G_Event ge = {0};
		if(CCEventScriptSetting::getSingleton().GetEvent(re.iEventID, ge))
		{
			LastingEvent le = {0};
			le.iIndex		= re.iIndex;
			le.iLasting		= ge.iLasting;
			le.llTimeBegin	= time(0);
			m_fightMap.insert(UEEventMap::value_type(le.iIndex, le));

			CMD_G_Event gfe = {0};
			gfe.dwUserID = m_pUser->GetUserID();
			gfe.iIndex   = re.iIndex;
			gfe.et       = EVENT_FIGHT;
			m_pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_EVENT, (CT_CHAR*)&gfe, sizeof(gfe));
		}
	}
}

//ɾ�������¼�
CT_VOID CCUserEvent::DeleteBuildingEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	UEEventMap::iterator it = m_buildingEventMap.find(iIndex);
	if (it != m_buildingEventMap.end())
	{
		m_buildingEventMap.erase(it);
	}
}

//ɾ���¼�
CT_VOID CCUserEvent::DeletePetEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	UEEventMap::iterator it = m_petEventMap.find(iIndex);
	if (it  != m_petEventMap.end())
	{
		m_petEventMap.erase(it);
	}
}

//ɾ���¼�
CT_VOID CCUserEvent::DeleteWorkerEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	UEEventMap::iterator it = m_workerEventMap.find(iIndex);
	if (it != m_workerEventMap.end())
	{
		m_workerEventMap.erase(it);
	}
}

//ɾ���¼�
CT_VOID CCUserEvent::DeleteTreasureEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	UEEventMap::iterator it = m_treasureEventMap.find(iIndex);
	if (it != m_treasureEventMap.end())
	{
		m_treasureEventMap.erase(it);
	}
}

//ɾ���¼�
CT_VOID CCUserEvent::DeleteFightEvent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	UEEventMap::iterator it = m_fightMap.find(iIndex);
	if (it != m_fightMap.end())
	{
		m_fightMap.erase(it);
	}
}

CT_BOOL CCUserEvent::FindBuildingEventByID(CT_INT32 iBuildID)
{
	_AUTOMUTEX(m_pMutex);
	std::map<CT_INT32, LastingEvent>::iterator it = m_buildingEventMap.begin();
	while(it!=m_buildingEventMap.end())
	{
		G_BuildingEventProbability bep = {0};
		if (CCEventScriptSetting::getSingleton().GetBuildingEvent(it->first,bep))
		{
			if (bep.iBuildID==iBuildID)
			{
				return CT_TRUE;
			}
		}
		it++;
	}
	return CT_FALSE;
}