#include <memory>
#include "CFightingScriptSetting.h"

template<> CCFightingScriptSetting* CCSingleton<CCFightingScriptSetting>::ms_Singleton=0;

CCFightingScriptSetting::CCFightingScriptSetting(void)
{
}

CCFightingScriptSetting::~CCFightingScriptSetting(void)
{
}
//根据世界ID获取世界副本信息
CT_BOOL	CCFightingScriptSetting::GetWorldScene(CT_INT32 iWorldSceneID,G_WorldScene &WorldScne)
{
	return 0 != G_GetWorldScene(iWorldSceneID+1,&WorldScne);
}
//根据副本ID获取副本信息
CT_BOOL	CCFightingScriptSetting::GetFightingScene(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,G_FightingScene &FightingScene)
{
	return 0 != G_GetFightingScene(iWorldSceneID+1,iSceneID+1,iDifficulty+1,&FightingScene);
}

//根据副本ID获取副本场景奖励
CT_BOOL	CCFightingScriptSetting::GetSceneReward(CT_INT32 iSceneID,CT_INT32 iReward,G_SceneReward &SceneReward)
{
	return 0 != G_GetSceneReward(iSceneID+1,iReward,&SceneReward);
}

//根据副本ID和怪物波数获取怪物组
CT_BOOL CCFightingScriptSetting::GetMonsterGroup(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,G_MonsterGroup &MonsterGroup)
{
	G_FightingScene FightingScene={0};
	if(!G_GetFightingScene(iWorldSceneID+1,iSceneID+1,iDifficulty+1,&FightingScene))
	{
		return CT_FALSE;
	}
	return  0 != G_GetMonsterGroup(iWorldSceneID+1,FightingScene.iMosterGroupID,&MonsterGroup);
}

//根据怪物组和位置获取怪物信息
CT_BOOL CCFightingScriptSetting::GetMonsterData(const G_MonsterGroup MonsterGroup,CT_INT32 iPosition,G_MonsterBaseInfo &MonsterInfo)
{
	//读取怪物组脚本
	switch(iPosition)
	{
	case 0:
		{
			if(MonsterGroup.iBeforeMoster1 == 0)break;
			if(!G_GetMonsterBaseInfoData(MonsterGroup.iBeforeMoster1,&MonsterInfo))
			{
				return CT_FALSE;
			}
			else
				return CT_TRUE;
			break;
		}
	case 1:
		{
			if(MonsterGroup.iBeforeMoster2 == 0)break;
			if(!G_GetMonsterBaseInfoData(MonsterGroup.iBeforeMoster2,&MonsterInfo))
			{
				return CT_FALSE;
			}
			else
				return CT_TRUE;
			break;
		}
	case 2:
		{
			if(MonsterGroup.iMediumMoster == 0)break;
			if(!G_GetMonsterBaseInfoData(MonsterGroup.iMediumMoster,&MonsterInfo))
			{
				return CT_FALSE;
			}
			else
				return CT_TRUE;
			break;
		}
	case 3:
		{
			if(MonsterGroup.iAfterMoster1 == 0)break;
			if(!G_GetMonsterBaseInfoData(MonsterGroup.iAfterMoster1,&MonsterInfo))
			{
				return CT_FALSE;
			}
			else
				return CT_TRUE;
			break;
		}
	case 4:
		{
			if(MonsterGroup.iAfterMoster2 == 0)break;
			if(!G_GetMonsterBaseInfoData(MonsterGroup.iAfterMoster2,&MonsterInfo))
			{
				return CT_FALSE;
			}
			else
				return CT_TRUE;
			break;
		}
	default:
		{
			return CT_FALSE;
		}
	}
	return CT_FALSE;
}


//根据军衔等级和类型获取PVP宝箱
CT_BOOL CCFightingScriptSetting::GetPVPChest(CT_INT32 iRandMax, CT_INT32 iReceiveNum, G_PVPChest &PVPChest)
{
	return 0 != G_GetPVPChestFile(iRandMax,iReceiveNum,&PVPChest);
}

//根据场景ID获取PVP场景胜利奖励
CT_BOOL CCFightingScriptSetting::GetPVPWinReward(CT_INT32 iRandGrade,CT_INT32 iChallengesDifficulty, G_PVPReward &PVPReward)
{
	return 0 != G_GetPVPWinReward(iRandGrade,iChallengesDifficulty,&PVPReward);
}

//根据场景ID获取PVP场景胜利奖励
CT_BOOL CCFightingScriptSetting::GetPVPLostReward(CT_INT32 iRandGrade,CT_INT32 iChallengesDifficulty, G_PVPReward &PVPReward)
{
	return 0 != G_GetPVPLostReward(iRandGrade,iChallengesDifficulty,&PVPReward);
}

//根据掉落ID获取掉落物品组
CT_BOOL CCFightingScriptSetting::GetGoodsGroup(CT_INT32 iDropGroupID, G_GoodsGroup &GoodsGroup)
{
	return 0 != G_GetGoodsGroup(iDropGroupID,&GoodsGroup);
}

//根据掉落ID获取物品权重
CT_BOOL CCFightingScriptSetting::GetGoodsWeights(CT_INT32 iDropGroupID, G_GoodsWeights &GoodsWeights)
{
	return 0 != G_GetGoodsWeights(iDropGroupID,&GoodsWeights);
}

//根据掉落ID获取物品数目
CT_BOOL CCFightingScriptSetting::GetGoodsCount(CT_INT32 iDropGroupID, G_GoodsCount &GoodsCount)
{
	return 0 != G_GetGoodsCount(iDropGroupID,&GoodsCount);
}

//根据掉落ID获取掉落物品组
CT_BOOL CCFightingScriptSetting::GetPVPGoodsGroup(CT_INT32 iDuplicateID, G_GoodsGroup &GoodsGroup)
{
	G_SceneReward SceneReward;
	memset(&SceneReward,0,sizeof(SceneReward));

	//if(G_GetSceneReward(iDuplicateID,&SceneReward) != 0)
	//{
	//	return 0 != G_GetGoodsGroup(SceneReward.iDuplicateID,GoodsGroup);
	//}
	//else 
	//	return CT_FALSE;

	return CT_TRUE;
}

//根据掉落ID获取物品权重
CT_BOOL CCFightingScriptSetting::GetPVPGoodsWeights(CT_INT32 iDuplicateID, G_GoodsWeights &GoodsWeights)
{
	G_SceneReward SceneReward;
	memset(&SceneReward,0,sizeof(SceneReward));

	//if((CT_BOOL)G_GetSceneReward(iDuplicateID,&SceneReward) == CT_TRUE)
	//{
	//	return 0 != G_GetGoodsWeights(SceneReward.iDuplicateID,GoodsWeights);
	//}
	//else 
	//	return CT_FALSE;

	return CT_TRUE;
}

//根据掉落ID获取物品数目
CT_BOOL CCFightingScriptSetting::GetPVPGoodsCount(CT_INT32 iDuplicateID, G_GoodsCount &GoodsCount)
{
	G_SceneReward SceneReward;
	memset(&SceneReward,0,sizeof(SceneReward));
	/*G_SceneReward SceneReward;
	memset(&SceneReward,0,sizeof(SceneReward));

	if((CT_BOOL)G_GetSceneReward(iDuplicateID,&SceneReward) == CT_TRUE)
	{
		return (CT_BOOL)G_GetGoodsCount(SceneReward.iDuplicateID,pGoodsCount);
	}
	else 
		return CT_FALSE;
	if((CT_BOOL)G_GetSceneReward(iDuplicateID,&SceneReward) == CT_TRUE)
	{
		return (CT_BOOL)G_GetGoodsCount(SceneReward.iDuplicateID,pGoodsCount);
	}
	else 
		return CT_FALSE;*/

	return CT_TRUE;
}


//根据怪物ID获取怪物信息
CT_BOOL	CCFightingScriptSetting::GetMosterData(CT_INT32 iMosterID,G_MonsterBaseInfo  &GMBI)
{
	return 0 != G_GetMonsterBaseInfoData(iMosterID,&GMBI);
}

//根据技能ID获取技能信息
CT_BOOL CCFightingScriptSetting::GetSkillMainData(CT_INT32 iSkillID,G_SkillMain &GS)
{
	//if(iSkillID == 0)CT_ASSERT(CT_FALSE);
	return 0 != G_GetSkillMainData(iSkillID,&GS);
}

//根据技能ID获取技能信息
CT_BOOL CCFightingScriptSetting::GetSkillAffectData(CT_INT32 iSkillID,G_SkillAffect &GSA)
{
	return 0 != G_GetSkillAffectData(iSkillID,&GSA);
}

//获得技能星级数据
CT_BOOL CCFightingScriptSetting::GetSkillStarData(int iSkillLevel,G_SkillStar &GSS)
{
	return 0 != G_GetSkillStarData(iSkillLevel,&GSS);
}

//根据场景ID获取世界场景奖励
CT_BOOL CCFightingScriptSetting::GetWorldSceneData(CT_INT32 iWorldSceneID,CT_INT32 iRewardID,G_WorldSceneReward &WSR)
{
	return 0 != G_GetWorldSceneReward(iWorldSceneID+1,iRewardID+1,&WSR);
}
