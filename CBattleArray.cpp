#include "CBattleArray.h"
#include "GlobeScriptInterface.h"
#include "CUser.h"
#include "CMasterScriptSetting.h"
#include "CLogMeg.h"

template<> CCBattleArray* CCSingleton<CCBattleArray>::ms_Singleton=0;
CCBattleArray::CCBattleArray(void)
{
}

CCBattleArray::~CCBattleArray(void)
{
}

//获得阵法加成
CT_BOOL CCBattleArray::GetBattleArray(const CT_INT32 iTalentNum,CT_INT32 iBattleArrayID,CCFightingLogic::FightingUserInfo &UserInfo)
{
	G_ValueRange ValueRound={0};
	if(!G_GetValueRange(VALUE_TYPE_FIGHTING_ROUND,&ValueRound))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::FigheingInfo()VALUE_TYPE_FIGHTING_ROUND");
	}
	CT_INT32 iRound=(CT_INT32)ValueRound.fMinVal;
	if(iRound < ROUNDD_MAX)
		iRound=ROUNDD_MAX;

	G_TalentStatus TalentStatusAll = {0};

	switch(iBattleArrayID)
	{
	case 0: 
	case 1:
		{
			std::list<CT_INT32> GroupIDList;
			for(CT_INT32 i=0;i<=iTalentNum;++i)
			{
				G_TalentStatus TalentStatus = {0};
				if (CCMasterScriptSetting::getSingleton().GetTalentData(i+1, UNIT_TYPE_ARRAY_FRONT, TalentStatus))
				{
					TalentStatusAll.fCritRate		+=TalentStatus.fCritRate;
					TalentStatusAll.fDmgDecPercent	+=TalentStatus.fDmgDecPercent;
					TalentStatusAll.fDmgIncPercent	+=TalentStatus.fDmgIncPercent;
					TalentStatusAll.fDodgeRate		+=TalentStatus.fDodgeRate;
					TalentStatusAll.fHealthPercent	+=TalentStatus.fHealthPercent;
					TalentStatusAll.fHitRate		+=TalentStatus.fHitRate;
					TalentStatusAll.fResilient		+=TalentStatus.fResilient;
					TalentStatusAll.iAgility		+=TalentStatus.iAgility;
					TalentStatusAll.iAttack			+=TalentStatus.iAttack;
					TalentStatusAll.iDefence		+=TalentStatus.iDefence;
					TalentStatusAll.iHPRecover		+=TalentStatus.iHPRecover;
					TalentStatusAll.iIntellect		+=TalentStatus.iIntellect;
					TalentStatusAll.iMana			+=TalentStatus.iMana;
					TalentStatusAll.iMPRecover		+=TalentStatus.iMPRecover;
					TalentStatusAll.iSpirit			+=TalentStatus.iSpirit;
					TalentStatusAll.iStrength		+=TalentStatus.iStrength;
				}
				memset(&TalentStatus,0,sizeof(TalentStatus));
				if (CCMasterScriptSetting::getSingleton().GetTalentData(i+1, UNIT_TYPE_PLAYER, TalentStatus))
				{
					G_TalentGroup stGroup = {0};
					if (CCMasterScriptSetting::getSingleton().GetTalentGroup(TalentStatus.iGroup, UNIT_TYPE_ARRAY_FRONT, stGroup))
					{
						if(i+1 == stGroup.iPrevID)
							GroupIDList.push_back(TalentStatus.iGroup);
					}
				}
			}

			GroupIDList.sort();
			GroupIDList.unique();
			for(std::list<CT_INT32>::iterator it=GroupIDList.begin();it != GroupIDList.end();++it)
			{
				G_TalentGroup stGroup = {0};
				if (CCMasterScriptSetting::getSingleton().GetTalentGroup(*it, UNIT_TYPE_ARRAY_FRONT, stGroup))
				{
					TalentStatusAll.fCritRate		+=stGroup.fCritRate;
					TalentStatusAll.fDmgDecPercent	+=stGroup.fDmgDecPercent;
					TalentStatusAll.fDmgIncPercent	+=stGroup.fDmgIncPercent;
					TalentStatusAll.fDodgeRate		+=stGroup.fDodgeRate;
					TalentStatusAll.fHealthPercent	+=stGroup.fHealthPercent;
					TalentStatusAll.fHitRate		+=stGroup.fHitRate;
					TalentStatusAll.fResilient		+=stGroup.fResilient;
					TalentStatusAll.iAgility		+=stGroup.iAgility;
					TalentStatusAll.iAttack			+=stGroup.iAttack;
					TalentStatusAll.iDefence		+=stGroup.iDefence;
					TalentStatusAll.iHPRecover		+=stGroup.iHPRecover;
					TalentStatusAll.iIntellect		+=stGroup.iIntellect;
					TalentStatusAll.iMana			+=stGroup.iMana;
					TalentStatusAll.iMPRecover		+=stGroup.iMPRecover;
					TalentStatusAll.iSpirit			+=stGroup.iSpirit;
					TalentStatusAll.iStrength		+=stGroup.iStrength;
				}
			}
			break;
		}
	case 2:
		{
			std::list<CT_INT32> GroupIDList;
			for(CT_INT32 i=0;i<=iTalentNum;++i)
			{
				G_TalentStatus TalentStatus = {0};
				if (CCMasterScriptSetting::getSingleton().GetTalentData(i+1, UNIT_TYPE_ARRAY_MIDDLE, TalentStatus))
				{
					TalentStatusAll.fCritRate		+=TalentStatus.fCritRate;
					TalentStatusAll.fDmgDecPercent	+=TalentStatus.fDmgDecPercent;
					TalentStatusAll.fDmgIncPercent	+=TalentStatus.fDmgIncPercent;
					TalentStatusAll.fDodgeRate		+=TalentStatus.fDodgeRate;
					TalentStatusAll.fHealthPercent	+=TalentStatus.fHealthPercent;
					TalentStatusAll.fHitRate		+=TalentStatus.fHitRate;
					TalentStatusAll.fResilient		+=TalentStatus.fResilient;
					TalentStatusAll.iAgility		+=TalentStatus.iAgility;
					TalentStatusAll.iAttack			+=TalentStatus.iAttack;
					TalentStatusAll.iDefence		+=TalentStatus.iDefence;
					TalentStatusAll.iHPRecover		+=TalentStatus.iHPRecover;
					TalentStatusAll.iIntellect		+=TalentStatus.iIntellect;
					TalentStatusAll.iMana			+=TalentStatus.iMana;
					TalentStatusAll.iMPRecover		+=TalentStatus.iMPRecover;
					TalentStatusAll.iSpirit			+=TalentStatus.iSpirit;
					TalentStatusAll.iStrength		+=TalentStatus.iStrength;
				}

				memset(&TalentStatus,0,sizeof(TalentStatus));
				if (CCMasterScriptSetting::getSingleton().GetTalentData(i+1, UNIT_TYPE_PLAYER, TalentStatus))
				{
					G_TalentGroup stGroup = {0};
					if (CCMasterScriptSetting::getSingleton().GetTalentGroup(TalentStatus.iGroup, UNIT_TYPE_ARRAY_MIDDLE, stGroup))
					{
						if(i+1 == stGroup.iPrevID)
							GroupIDList.push_back(TalentStatus.iGroup);
					}
				}
			}

			GroupIDList.sort();
			GroupIDList.unique();
			for(std::list<CT_INT32>::iterator it=GroupIDList.begin();it != GroupIDList.end();++it)
			{
				G_TalentGroup stGroup = {0};
				if (CCMasterScriptSetting::getSingleton().GetTalentGroup(*it, UNIT_TYPE_ARRAY_MIDDLE, stGroup))
				{
					TalentStatusAll.fCritRate		+=stGroup.fCritRate;
					TalentStatusAll.fDmgDecPercent	+=stGroup.fDmgDecPercent;
					TalentStatusAll.fDmgIncPercent	+=stGroup.fDmgIncPercent;
					TalentStatusAll.fDodgeRate		+=stGroup.fDodgeRate;
					TalentStatusAll.fHealthPercent	+=stGroup.fHealthPercent;
					TalentStatusAll.fHitRate		+=stGroup.fHitRate;
					TalentStatusAll.fResilient		+=stGroup.fResilient;
					TalentStatusAll.iAgility		+=stGroup.iAgility;
					TalentStatusAll.iAttack			+=stGroup.iAttack;
					TalentStatusAll.iDefence		+=stGroup.iDefence;
					TalentStatusAll.iHPRecover		+=stGroup.iHPRecover;
					TalentStatusAll.iIntellect		+=stGroup.iIntellect;
					TalentStatusAll.iMana			+=stGroup.iMana;
					TalentStatusAll.iMPRecover		+=stGroup.iMPRecover;
					TalentStatusAll.iSpirit			+=stGroup.iSpirit;
					TalentStatusAll.iStrength		+=stGroup.iStrength;
				}
			}
			break;
		}
	case 3:
	case 4:
		{
			std::list<CT_INT32> GroupIDList;
			for(CT_INT32 i=0;i<=iTalentNum;++i)
			{
				G_TalentStatus TalentStatus = {0};
				if (CCMasterScriptSetting::getSingleton().GetTalentData(i+1, UNIT_TYPE_ARRAY_BACK, TalentStatus))
				{
					TalentStatusAll.fCritRate		+=TalentStatus.fCritRate;
					TalentStatusAll.fDmgDecPercent	+=TalentStatus.fDmgDecPercent;
					TalentStatusAll.fDmgIncPercent	+=TalentStatus.fDmgIncPercent;
					TalentStatusAll.fDodgeRate		+=TalentStatus.fDodgeRate;
					TalentStatusAll.fHealthPercent	+=TalentStatus.fHealthPercent;
					TalentStatusAll.fHitRate		+=TalentStatus.fHitRate;
					TalentStatusAll.fResilient		+=TalentStatus.fResilient;
					TalentStatusAll.iAgility		+=TalentStatus.iAgility;
					TalentStatusAll.iAttack			+=TalentStatus.iAttack;
					TalentStatusAll.iDefence		+=TalentStatus.iDefence;
					TalentStatusAll.iHPRecover		+=TalentStatus.iHPRecover;
					TalentStatusAll.iIntellect		+=TalentStatus.iIntellect;
					TalentStatusAll.iMana			+=TalentStatus.iMana;
					TalentStatusAll.iMPRecover		+=TalentStatus.iMPRecover;
					TalentStatusAll.iSpirit			+=TalentStatus.iSpirit;
					TalentStatusAll.iStrength		+=TalentStatus.iStrength;
				}

				memset(&TalentStatus,0,sizeof(TalentStatus));
				if (CCMasterScriptSetting::getSingleton().GetTalentData(i+1, UNIT_TYPE_PLAYER, TalentStatus))
				{
					G_TalentGroup stGroup = {0};
					if (CCMasterScriptSetting::getSingleton().GetTalentGroup(TalentStatus.iGroup, UNIT_TYPE_ARRAY_BACK, stGroup))
					{
						if(i+1 == stGroup.iPrevID)
							GroupIDList.push_back(TalentStatus.iGroup);
					}
				}
			}

			GroupIDList.sort();
			GroupIDList.unique();
			for(std::list<CT_INT32>::iterator it=GroupIDList.begin();it != GroupIDList.end();++it)
			{
				G_TalentGroup stGroup = {0};
				if (CCMasterScriptSetting::getSingleton().GetTalentGroup(*it, UNIT_TYPE_ARRAY_BACK, stGroup))
				{
					TalentStatusAll.fCritRate		+=stGroup.fCritRate;
					TalentStatusAll.fDmgDecPercent	+=stGroup.fDmgDecPercent;
					TalentStatusAll.fDmgIncPercent	+=stGroup.fDmgIncPercent;
					TalentStatusAll.fDodgeRate		+=stGroup.fDodgeRate;
					TalentStatusAll.fHealthPercent	+=stGroup.fHealthPercent;
					TalentStatusAll.fHitRate		+=stGroup.fHitRate;
					TalentStatusAll.fResilient		+=stGroup.fResilient;
					TalentStatusAll.iAgility		+=stGroup.iAgility;
					TalentStatusAll.iAttack			+=stGroup.iAttack;
					TalentStatusAll.iDefence		+=stGroup.iDefence;
					TalentStatusAll.iHPRecover		+=stGroup.iHPRecover;
					TalentStatusAll.iIntellect		+=stGroup.iIntellect;
					TalentStatusAll.iMana			+=stGroup.iMana;
					TalentStatusAll.iMPRecover		+=stGroup.iMPRecover;
					TalentStatusAll.iSpirit			+=stGroup.iSpirit;
					TalentStatusAll.iStrength		+=stGroup.iStrength;
				}
			}
			break;
		}
	}

	//全军加成
	for(CT_INT32 i=0;i<=iTalentNum;++i)
	{
		G_TalentStatus TalentStatus = {0};
		if (CCMasterScriptSetting::getSingleton().GetTalentData(i+1, UNIT_TYPE_ARRAY_ALL, TalentStatus))
		{
			TalentStatusAll.fCritRate		+=TalentStatus.fCritRate;
			TalentStatusAll.fDmgDecPercent	+=TalentStatus.fDmgDecPercent;
			TalentStatusAll.fDmgIncPercent	+=TalentStatus.fDmgIncPercent;
			TalentStatusAll.fDodgeRate		+=TalentStatus.fDodgeRate;
			TalentStatusAll.fHealthPercent	+=TalentStatus.fHealthPercent;
			TalentStatusAll.fHitRate		+=TalentStatus.fHitRate;
			TalentStatusAll.fResilient		+=TalentStatus.fResilient;
			TalentStatusAll.iAgility		+=TalentStatus.iAgility;
			TalentStatusAll.iAttack			+=TalentStatus.iAttack;
			TalentStatusAll.iDefence		+=TalentStatus.iDefence;
			TalentStatusAll.iHPRecover		+=TalentStatus.iHPRecover;
			TalentStatusAll.iIntellect		+=TalentStatus.iIntellect;
			TalentStatusAll.iMana			+=TalentStatus.iMana;
			TalentStatusAll.iMPRecover		+=TalentStatus.iMPRecover;
			TalentStatusAll.iSpirit			+=TalentStatus.iSpirit;
			TalentStatusAll.iStrength		+=TalentStatus.iStrength;
		}
	}

	UserInfo.iHpReply		+= TalentStatusAll.iHPRecover;
	UserInfo.iMpReply		+= TalentStatusAll.iMPRecover;
	UserInfo.iHpEffect[0]	+= UserInfo.iHpReply;
	UserInfo.iMpEffect[0]	+= UserInfo.iMpReply;
	UserInfo.cbStatusCount  = 1;
	UserInfo.iSkillStatus[0]= BATTLEARRAY_EFFECT;
	UserInfo.iStatusTime[0] = iRound;

	UserInfo.iStrenth		+= TalentStatusAll.iStrength;
	UserInfo.iAgility       += TalentStatusAll.iAgility;
	UserInfo.iAttack		+= TalentStatusAll.iAttack;
	UserInfo.iDefence		+= TalentStatusAll.iDefence;
	UserInfo.iIntellent     += TalentStatusAll.iIntellect;
	UserInfo.iSpirit        += TalentStatusAll.iSpirit;
	UserInfo.fHitRate       += TalentStatusAll.fHitRate;
	UserInfo.fDodgeRate     += TalentStatusAll.fDodgeRate;
	UserInfo.fCritRate      += TalentStatusAll.fCritRate;
	UserInfo.fResilient     += TalentStatusAll.fResilient;
	UserInfo.fDmgIncPercent += TalentStatusAll.fDmgIncPercent;
	UserInfo.fDmgDecPercent += TalentStatusAll.fDmgDecPercent;


	return CT_TRUE;
}

//获得阵法加成
CT_BOOL CCBattleArray::GetBattleArray(CT_INT32 iBattleArrayID,MasterCombatStatus &UserInfo)
{
	G_BattleArray BattleArray={0};
	if(!G_GetBattleArray(iBattleArrayID,&BattleArray))
	{
		return CT_FALSE;
	}

	UserInfo.iMaxHP			+= (CT_INT32)(UserInfo.iMaxHP*BattleArray.fHpCeiling);
	//UserInfo.iCurrentHP		+= (CT_INT32)(UserInfo.iMaxHP*BattleArray.fHpCeiling);

	UserInfo.iHPRecover		+= (CT_INT32)(UserInfo.iMaxHP*BattleArray.fHpReply);
	UserInfo.iMPRecover		+= (CT_INT32)BattleArray.fMpReply;

	UserInfo.iStrength		+= BattleArray.iStrengthIncrease;
	UserInfo.iAgility       += BattleArray.iDexterityIncrease;
	UserInfo.iIntellect     += BattleArray.iIntelligenceIncrease;
	UserInfo.iSpirit        += BattleArray.iSpiritIncrease;
	UserInfo.fHitRate       += BattleArray.fAimIncrease;
	UserInfo.fDodgeRate     += BattleArray.fDodgeIncrease;
	UserInfo.fCritRate      += BattleArray.fCriticalHitIncrease;
	UserInfo.fResilient     += BattleArray.fConstitutionIncrease;
	UserInfo.fDmgIncPercent += BattleArray.fDamageIncrease;
	UserInfo.fDmgDecPercent += BattleArray.fAbsorbDamageIncrease;


	return CT_TRUE;
}