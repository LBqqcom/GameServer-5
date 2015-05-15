/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingScriptSetting.h
Version:		1.0
Description:    ս���ű�
Author:			wangchen
Create Date:	2012-9-15
History:        ս���ű�     

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
	//��ȡ������Ϣ
	//////////////////////////////////////////////////////////////////////////
	//��������ID��ȡ���縱����Ϣ
	 CT_BOOL	GetWorldScene(CT_INT32 iWorldSceneID,G_WorldScene &WorldScne);
	//���ݸ���ID��ȡ������Ϣ
	 CT_BOOL	GetFightingScene(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,G_FightingScene &FightingScene);
	//���ݸ���ID��ȡ������������
	 CT_BOOL	GetSceneReward(CT_INT32 iSceneID,CT_INT32 iReward,G_SceneReward &SceneReward);
	//���ݸ���ID�͹��ﲨ����ȡ������
	 CT_BOOL	GetMonsterGroup(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,G_MonsterGroup &MonsterGroup);
	 //���ݸ���ID��λ�û�ȡ������Ϣ
	 CT_BOOL	GetMonsterData(const G_MonsterGroup MonsterGroup,CT_INT32 iPosition,G_MonsterBaseInfo &MonsterInfo);

	 //��ȡ��������Ϣ
	 //////////////////////////////////////////////////////////////////////////
	//���ݸ���ID��ȡPVP
	 CT_BOOL GetPVPChest(CT_INT32 iRandMax, CT_INT32 iReceiveNum, G_PVPChest &PVPChest);
	//���ݸ���ID��ȡPVP����ʤ������
	 CT_BOOL GetPVPWinReward(CT_INT32 iRandGrade,CT_INT32 iChallengesDifficulty,G_PVPWinReward &PVPReward);
	 //���ݸ���ID��ȡPVP����ʧ�ܽ���
	 CT_BOOL GetPVPLostReward(CT_INT32 iRandGrade,CT_INT32 iChallengesDifficulty,G_PVPLostReward &PVPReward);

	 //��ȡ��������Ϣ
	 //////////////////////////////////////////////////////////////////////////
	//���ݵ���ID��ȡ������Ʒ��
	 CT_BOOL GetGoodsGroup(CT_INT32 iDropGroupID, G_GoodsGroup &GoodsGroup);
	//���ݵ���ID��ȡ��ƷȨ��
	 CT_BOOL GetGoodsWeights(CT_INT32 iDropGroupID, G_GoodsWeights &GoodsWeights);
	//���ݵ���ID��ȡ��Ʒ��Ŀ
	 CT_BOOL GetGoodsCount(CT_INT32 iDropGroupID, G_GoodsCount &GoodsCount);
	//���ݵ���ID��ȡ������Ʒ��
	 CT_BOOL GetPVPGoodsGroup(CT_INT32 iDuplicateID, G_GoodsGroup &GoodsGroup);
	//���ݵ���ID��ȡ��ƷȨ��
	 CT_BOOL GetPVPGoodsWeights(CT_INT32 iDuplicateID, G_GoodsWeights &GoodsWeights);
	//���ݵ���ID��ȡ��Ʒ��Ŀ
	 CT_BOOL GetPVPGoodsCount(CT_INT32 iDuplicateID, G_GoodsCount &GoodsCount);

	//��ȡ������Ϣ
	///////////////////////////////////////////////////////////////////////
	//���ݹ���ID��ȡ������Ϣ
	 CT_BOOL	GetMosterData(CT_INT32 iMosterID,G_MonsterBaseInfo  &GMBI);

	//��ȡ������Ϣ
	//////////////////////////////////////////////////////////////////////////
	//���ݼ���ID��ȡ������Ϣ
	 CT_BOOL	GetSkillMainData(CT_INT32 iSkillID,G_SkillMain &GS);
	//���ݼ���ID��ȡ������Ϣ
	 CT_BOOL	GetSkillAffectData(CT_INT32 iSkillID,G_SkillAffect &GSA);
	//��ü����Ǽ�����
	 CT_BOOL GetSkillStarData(CT_INT32 iSkillLevel,G_SkillStar &GSS);

	//��ȡ������Ϣ
	//////////////////////////////////////////////////////////////////////////
	//���ݳ���ID��ȡ���糡������
	 CT_BOOL GetWorldSceneData(CT_INT32 iWorldSceneID,CT_INT32 iRewardID,G_WorldSceneReward &WSR);
};
#endif