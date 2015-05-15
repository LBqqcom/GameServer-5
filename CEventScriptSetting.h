#ifndef CCEVENTSCRIPTSETTING_H_
#define CCEVENTSCRIPTSETTING_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"

class CCEventScriptSetting : public CCSingleton<CCEventScriptSetting>
{
public:
	CCEventScriptSetting(void);
	~CCEventScriptSetting(void);

public:
	//�õ��¼�
	 CT_BOOL GetEvent(CT_INT32 eventID, G_Event& event);
	//�õ��¼�����
	 CT_BOOL GetEventAward(CT_INT32 eventID, CT_INT32 awardID, G_EventAward& ea);
	//�õ������¼�
	 CT_BOOL GetPetEvent(CT_INT32 iIndex,   G_NormalEventProbability& np);
	//�õ������¼�
	 CT_BOOL GetWorkerEvent(CT_INT32 iIndex,  G_NormalEventProbability& np);
	//�õ������¼�
	 CT_BOOL GetTreasueEvent(CT_INT32 iIndex, G_NormalEventProbability& np);
	//�õ�ս���¼�
	 CT_BOOL GetFightEvent(CT_INT32 iIndex,  G_FightEventProbablity& fep);
	//�õ������¼�
	 CT_BOOL GetBuildingEvent(CT_INT32 iIndex, G_BuildingEventProbability& bep);
	//�õ��¼�����
	 CT_INT32 GetEventMonsterGroup(int iMosterGroupID,G_MonsterGroup& monsterGroup);

	//�õ������¼�������
	CT_INT32 GetPetEventNum() {return G_GetPetEventNum(); }
	//�õ������¼�������
	CT_INT32 GetWorkerEventNum(){return G_GetWorkerEventNum();}
	//�õ������¼�������
	CT_INT32 GetTreasureEventNum() {return G_GetTreasureEventNum();}
	//�õ�ս���¼�����
	CT_INT32 GetFightEvntNum() {return G_GetFightEvntNum();}
	//�õ������¼�����
	CT_INT32 GetBuildingEventNum() {return G_GetBuildingEventNum();}

};
#endif