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
	//得到事件
	 CT_BOOL GetEvent(CT_INT32 eventID, G_Event& event);
	//得到事件奖励
	 CT_BOOL GetEventAward(CT_INT32 eventID, CT_INT32 awardID, G_EventAward& ea);
	//得到宠物事件
	 CT_BOOL GetPetEvent(CT_INT32 iIndex,   G_NormalEventProbability& np);
	//得到工人事件
	 CT_BOOL GetWorkerEvent(CT_INT32 iIndex,  G_NormalEventProbability& np);
	//得到宝箱事件
	 CT_BOOL GetTreasueEvent(CT_INT32 iIndex, G_NormalEventProbability& np);
	//得到战斗事件
	 CT_BOOL GetFightEvent(CT_INT32 iIndex,  G_FightEventProbablity& fep);
	//得到建筑事件
	 CT_BOOL GetBuildingEvent(CT_INT32 iIndex, G_BuildingEventProbability& bep);
	//得到事件怪物
	 CT_INT32 GetEventMonsterGroup(int iMosterGroupID,G_MonsterGroup& monsterGroup);

	//得到宠物事件的数量
	CT_INT32 GetPetEventNum() {return G_GetPetEventNum(); }
	//得到工人事件的数量
	CT_INT32 GetWorkerEventNum(){return G_GetWorkerEventNum();}
	//得到宝箱事件的数量
	CT_INT32 GetTreasureEventNum() {return G_GetTreasureEventNum();}
	//得到战斗事件数量
	CT_INT32 GetFightEvntNum() {return G_GetFightEvntNum();}
	//得到建筑事件数量
	CT_INT32 GetBuildingEventNum() {return G_GetBuildingEventNum();}

};
#endif