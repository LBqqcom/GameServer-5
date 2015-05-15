#ifndef CCBATTLEARRAY_H_
#define CCBATTLEARRAY_H_
#include "CSingleton.h"
#include "CTType.h"
#include "CFightingLogic.h"

class CCUser;
struct FightingUserInfo;
struct MasterCombatStatus;

class CCBattleArray:public CCSingleton<CCBattleArray>
{
public:
	CCBattleArray(void);
	~CCBattleArray(void);

public:
	//����󷨼ӳ�
	CT_BOOL GetBattleArray(const CT_INT32 iTalentNum,CT_INT32 iBattleArrayID,CCFightingLogic::FightingUserInfo &UserInfo);
	//����󷨼ӳ�
	CT_BOOL GetBattleArray(CT_INT32 iBattleArrayID,MasterCombatStatus &UserInfo);
};

#endif