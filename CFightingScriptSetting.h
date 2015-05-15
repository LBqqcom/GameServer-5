/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingScriptSetting.h
Version:		1.0
Description:    战斗脚本
Author:			wangchen
Create Date:	2012-9-15
History:        战斗脚本     

******************************************************************/

#ifndef CCFIGHTINGSCRIPTSETTING_H_
#define CCFIGHTINGSCRIPTSETTING_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"

class CCFightingScriptSetting: public CCSingleton<CCFightingScriptSetting>
{
public:
	CCFightingScriptSetting(void);
	~CCFightingScriptSetting(void);

public:
	//读取副本信息
	//////////////////////////////////////////////////////////////////////////
	//根据世界ID获取世界副本信息
	 CT_BOOL	GetWorldScene(CT_INT32 iWorldSceneID,G_WorldScene &WorldScne);
	//根据副本ID获取副本信息
	 CT_BOOL	GetFightingScene(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,G_FightingScene &FightingScene);
	//根据副本ID获取副本场景奖励
	 CT_BOOL	GetSceneReward(CT_INT32 iSceneID,CT_INT32 iReward,G_SceneReward &SceneReward);
	//根据副本ID和怪物波数获取怪物组
	 CT_BOOL	GetMonsterGroup(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,G_MonsterGroup &MonsterGroup);
	 //根据副本ID和位置获取怪物信息
	 CT_BOOL	GetMonsterData(const G_MonsterGroup MonsterGroup,CT_INT32 iPosition,G_MonsterBaseInfo &MonsterInfo);

	 //读取竞技场信息
	 //////////////////////////////////////////////////////////////////////////
	//根据副本ID获取PVP
	 CT_BOOL GetPVPChest(CT_INT32 iRandMax, CT_INT32 iReceiveNum, G_PVPChest &PVPChest);
	//根据副本ID获取PVP场景胜利奖励
	 CT_BOOL GetPVPWinReward(CT_INT32 iRandGrade,CT_INT32 iChallengesDifficulty,G_PVPWinReward &PVPReward);
	 //根据副本ID获取PVP场景失败奖励
	 CT_BOOL GetPVPLostReward(CT_INT32 iRandGrade,CT_INT32 iChallengesDifficulty,G_PVPLostReward &PVPReward);

	 //读取掉落组信息
	 //////////////////////////////////////////////////////////////////////////
	//根据掉落ID获取掉落物品组
	 CT_BOOL GetGoodsGroup(CT_INT32 iDropGroupID, G_GoodsGroup &GoodsGroup);
	//根据掉落ID获取物品权重
	 CT_BOOL GetGoodsWeights(CT_INT32 iDropGroupID, G_GoodsWeights &GoodsWeights);
	//根据掉落ID获取物品数目
	 CT_BOOL GetGoodsCount(CT_INT32 iDropGroupID, G_GoodsCount &GoodsCount);
	//根据掉落ID获取掉落物品组
	 CT_BOOL GetPVPGoodsGroup(CT_INT32 iDuplicateID, G_GoodsGroup &GoodsGroup);
	//根据掉落ID获取物品权重
	 CT_BOOL GetPVPGoodsWeights(CT_INT32 iDuplicateID, G_GoodsWeights &GoodsWeights);
	//根据掉落ID获取物品数目
	 CT_BOOL GetPVPGoodsCount(CT_INT32 iDuplicateID, G_GoodsCount &GoodsCount);

	//读取怪物信息
	///////////////////////////////////////////////////////////////////////
	//根据怪物ID获取怪物信息
	 CT_BOOL	GetMosterData(CT_INT32 iMosterID,G_MonsterBaseInfo  &GMBI);

	//读取技能信息
	//////////////////////////////////////////////////////////////////////////
	//根据技能ID获取技能信息
	 CT_BOOL	GetSkillMainData(CT_INT32 iSkillID,G_SkillMain &GS);
	//根据技能ID获取技能信息
	 CT_BOOL	GetSkillAffectData(CT_INT32 iSkillID,G_SkillAffect &GSA);
	//获得技能星级数据
	 CT_BOOL GetSkillStarData(CT_INT32 iSkillLevel,G_SkillStar &GSS);

	//读取副本信息
	//////////////////////////////////////////////////////////////////////////
	//根据场景ID获取世界场景奖励
	 CT_BOOL GetWorldSceneData(CT_INT32 iWorldSceneID,CT_INT32 iRewardID,G_WorldSceneReward &WSR);
};
#endif