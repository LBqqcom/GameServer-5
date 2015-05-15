#include "CEventScriptSetting.h"
template<> CCEventScriptSetting* CCSingleton<CCEventScriptSetting>::ms_Singleton=0;


CCEventScriptSetting::CCEventScriptSetting(void)
{
}

CCEventScriptSetting::~CCEventScriptSetting(void)
{
}

//�õ��¼�
CT_BOOL CCEventScriptSetting::GetEvent(CT_INT32 eventID, G_Event& event)
{
	return  0 != G_GetEvent(eventID, &event);
}

//�õ��¼�����
CT_BOOL CCEventScriptSetting::GetEventAward(CT_INT32 eventID, CT_INT32 awardID, G_EventAward& ea)
{
	return 0 != G_GetEventAward(eventID, awardID, &ea);
}

//�õ������¼�
CT_BOOL	CCEventScriptSetting::GetPetEvent(CT_INT32 iIndex,   G_NormalEventProbability& np)
{
	return 0 != G_GetPetEvent(iIndex, &np);
}

//�õ������¼�
CT_BOOL CCEventScriptSetting::GetWorkerEvent(CT_INT32 iIndex,  G_NormalEventProbability& np)
{
	return 0 != G_GetWorkerEvent(iIndex, &np);
}

//�õ������¼�
CT_BOOL CCEventScriptSetting::GetTreasueEvent(CT_INT32 iIndex, G_NormalEventProbability& np)
{
	return 0 != G_GetTreasueEvent(iIndex, &np);
}

//�õ�ս���¼�
CT_BOOL CCEventScriptSetting::GetFightEvent(CT_INT32 iIndex,  G_FightEventProbablity& fep)
{
	return 0 != G_GetFightEvent(iIndex, &fep);
}

//�õ������¼�
CT_BOOL CCEventScriptSetting::GetBuildingEvent(CT_INT32 iIndex, G_BuildingEventProbability& bep)
{
	return 0 != G_GetBuildingEvent(iIndex, &bep);
}

//�õ��¼�����
CT_INT32 CCEventScriptSetting::GetEventMonsterGroup(int iMosterGroupID,G_MonsterGroup& monsterGroup)
{
	return 0 != G_GetEventMonsterGroup(iMosterGroupID, &monsterGroup);
}
