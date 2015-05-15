#include  "CRandEventManager.h"
#include "GlobeScriptInterface.h"
template<> CCRandEventManager* CCSingleton<CCRandEventManager>::ms_Singleton=0;

#define  REM_VALUE  (10000)

CCRandEventManager::CCRandEventManager(void)
{
	CT_FLOAT fNum = 0;
	GetTotalProbability(fNum);
	CalProbability(fNum);
}

CCRandEventManager::~CCRandEventManager(void)
{
}

//�õ�һ������¼�
CT_BOOL CCRandEventManager::GetOneEvent(RandEvent& re)
{
	CT_INT32 iRand = rand() % REM_VALUE;
	CT_DWORD iNum  = m_randEventVec.size(); 
	for (CT_DWORD i=0; i<iNum; ++i)
	{
		if (iRand < m_randEventVec[i].iProbabilityValue )
		{
			memcpy(&re, &m_randEventVec[i], sizeof(RandEvent));
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//�����������
CT_VOID CCRandEventManager::SaveProbability(CT_INT32& iBaseProbability,
											const G_NormalEventProbability& nep, float fNum, ENUM_EventType et)
{
	iBaseProbability += (CT_INT32)((nep.fProbability / fNum)*REM_VALUE);
	RandEvent re = {0};
	re.fProbability = nep.fProbability;
	re.iIndex       = nep.iIndex;
	re.iEventID     = nep.iEventID;
	re.iEventType   = et;
	re.iProbabilityValue = iBaseProbability;
	m_randEventVec.push_back(re);
}

//�����������
CT_VOID CCRandEventManager::SaveProbability(CT_INT32& iBaseProbability, const G_FightEventProbablity& nep, float fNum, ENUM_EventType et)
{
	iBaseProbability += (CT_INT32)((nep.fProbability / fNum)*REM_VALUE);
	RandEvent re = {0};
	re.fProbability = nep.fProbability;
	re.iIndex     = nep.iIndex;
	re.iEventID     = nep.iEventID;
	re.iEventType = et;
	re.iProbabilityValue = iBaseProbability;
	m_randEventVec.push_back(re);
}

//�����������
CT_VOID CCRandEventManager::SaveProbability(CT_INT32& iBaseProbability,
											const G_BuildingEventProbability& bep, float fNum, ENUM_EventType et)
{
	iBaseProbability += (CT_INT32)((bep.fProbability / fNum)*REM_VALUE );
	RandEvent re = {0};
	re.fProbability = bep.fProbability;
	re.iIndex     = bep.iIndex;
	re.iEventID     = bep.iEventID;
	re.iEventType   = et;
	re.iProbabilityValue = iBaseProbability;
	m_randEventVec.push_back(re);
}



//�õ��ܵĸ���
CT_VOID CCRandEventManager::GetTotalProbability(CT_FLOAT& fNum)
{
	
	CT_INT32 iMaxIndex = CCEventScriptSetting::getSingleton().GetPetEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)      //�������͵��¼��������Ǵ�1��ʼ��
	{
		G_NormalEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetPetEvent(i, gNp))
		{
			fNum += gNp.fProbability;
		}
	}

	iMaxIndex = CCEventScriptSetting::getSingleton().GetWorkerEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_NormalEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetWorkerEvent(i, gNp))
		{
			fNum += gNp.fProbability;
		}
	}

	iMaxIndex = CCEventScriptSetting::getSingleton().GetTreasureEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_NormalEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetTreasueEvent(i, gNp))
		{
			fNum += gNp.fProbability;
		}
	}

	iMaxIndex = CCEventScriptSetting::getSingleton().GetFightEvntNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_FightEventProbablity gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetFightEvent(i, gNp))
		{
			fNum += gNp.fProbability;
		}
	}

	iMaxIndex = CCEventScriptSetting::getSingleton().GetBuildingEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_BuildingEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetBuildingEvent(i, gNp))
		{
			fNum += gNp.fProbability;
		}
	}
}

//������ʷֲ�
CT_VOID CCRandEventManager::CalProbability(CT_FLOAT& fNum)
{
	CT_INT32 iBaseProbability = 0;
	CT_INT32 iMaxIndex = CCEventScriptSetting::getSingleton().GetPetEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_NormalEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetPetEvent(i, gNp))
		{
			SaveProbability(iBaseProbability, gNp, fNum, EVENT_PET);
		}
	}

	iMaxIndex = CCEventScriptSetting::getSingleton().GetWorkerEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_NormalEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetWorkerEvent(i, gNp))
		{
			SaveProbability(iBaseProbability, gNp, fNum, EVENT_WORKER);
		}
	}

	iMaxIndex = CCEventScriptSetting::getSingleton().GetTreasureEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_NormalEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetTreasueEvent(i, gNp))
		{
			SaveProbability(iBaseProbability, gNp, fNum, EVENT_TREASURE);
		}
	}

	iMaxIndex = CCEventScriptSetting::getSingleton().GetFightEvntNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_FightEventProbablity gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetFightEvent(i, gNp))
		{
			SaveProbability(iBaseProbability, gNp, fNum, EVENT_FIGHT);
		}
	}


    iMaxIndex = CCEventScriptSetting::getSingleton().GetBuildingEventNum();
	for (CT_INT32 i=1; i<=iMaxIndex; ++i)
	{
		G_BuildingEventProbability gNp = {0};
		if(CCEventScriptSetting::getSingleton().GetBuildingEvent(i, gNp))
		{
			SaveProbability(iBaseProbability, gNp, fNum, EVENT_BUILDING);
		}
	}
}