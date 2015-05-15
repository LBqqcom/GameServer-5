#include "CMasterScriptSetting.h"
#include "CUser.h"
template<> CCMasterScriptSetting* CCSingleton<CCMasterScriptSetting>::ms_Singleton=0;

CCMasterScriptSetting::CCMasterScriptSetting(void)
{
}

CCMasterScriptSetting::~CCMasterScriptSetting(void)
{
}

//�õ�������������
CT_BOOL  CCMasterScriptSetting::GetMasterUpgradeData(CT_INT32 iGrade, MasterLevelInfo& ml)
{
	G_MasterExperience me;
	memset(&me, 0, sizeof(me));
	CT_INT32 ret = G_GetMasterExperience(iGrade, &me );
	ml.iLevel = me.iLevel;
	ml.iCurrentExp = me.iMaxExp;
	return ret != 0;
}

// �õ�Ǳ��ѧϰ����
CT_BOOL CCMasterScriptSetting::GetTalentData(CT_INT32 iTalentID, CT_INT32 iTargetID, G_TalentStatus& stLearn)
{
    CT_INT32 targets[] = {
        UNIT_TYPE_PLAYER        ,
        UNIT_TYPE_WARRIOR		,
        UNIT_TYPE_MAGE			,
        UNIT_TYPE_ARCHER		,
        UNIT_TYPE_ASSASSIN		,
        UNIT_TYPE_ARRAY_FRONT	,
        UNIT_TYPE_ARRAY_MIDDLE	,
        UNIT_TYPE_ARRAY_BACK	,
        UNIT_TYPE_ARRAY_ALL     ,	
    };
    
    if (iTargetID < 0)
    {
        for (int i = 0; i < sizeof(targets) / sizeof(*targets); ++i)
        {
            if (0 != G_GetTalentStatus(iTalentID, targets[i], &stLearn))
            {
                return CT_TRUE;
            }
        }
        return CT_FALSE;
    }
    else
    {
        return 0 != G_GetTalentStatus(iTalentID, iTargetID, &stLearn);
    }    
}
// �õ�Ǳ��������
CT_BOOL CCMasterScriptSetting::GetTalentGroup(CT_INT32 iGroupID, CT_INT32 iTargetID, G_TalentGroup& stGroup)
{
    CT_INT32 targets[] = {
        UNIT_TYPE_PLAYER        ,
        UNIT_TYPE_WARRIOR		,
        UNIT_TYPE_MAGE			,
        UNIT_TYPE_ARCHER		,
        UNIT_TYPE_ASSASSIN		,
        UNIT_TYPE_ARRAY_FRONT	,
        UNIT_TYPE_ARRAY_MIDDLE	,
        UNIT_TYPE_ARRAY_BACK	,
        UNIT_TYPE_ARRAY_ALL     ,	
    };

    if (iTargetID < 0)  {
        for (int i = 0; i < sizeof(targets) / sizeof(*targets); ++i)
        {
            if (0 != G_GetTalentGroup(iGroupID, targets[i], &stGroup))
            {
                return CT_TRUE;
            }
        }
        return CT_FALSE;
    } else {
        return 0 != G_GetTalentGroup(iGroupID, iTargetID, &stGroup);
    }    
}

// �õ�������������
CT_BOOL CCMasterScriptSetting::GetRankUpgradeData(CT_INT32 iLevel, MasterLevelInfo& stInfo)
{
    G_RankStatus me;
    memset(&me, 0, sizeof(me));
    CT_INT32 ret = G_GetRankStatus(iLevel, &me );
    stInfo.iLevel = me.iLevel;
    stInfo.iCurrentExp = me.iMaxExp;
    return ret != 0;
}

// �õ�������Ϣ
CT_BOOL CCMasterScriptSetting::GetRankStatus(CT_INT32 iLevel, G_RankStatus& stInfo)
{
    G_RankStatus me = {0};
    CT_INT32 ret = G_GetRankStatus(iLevel, &me );
    stInfo = me;
    return ret != 0;
}

// �õ�����ǿ������
CT_BOOL CCMasterScriptSetting::GetLearnSkillData(CT_INT32 iSkillID, G_SkillMain & stLearn)
{
    G_SkillMain skill = {0};    
    CT_INT32 ret = G_GetSkillMainData(iSkillID, &skill);
    stLearn = skill;    
    return ret != 0;
}

// �õ���ֵ��Χ
CT_BOOL CCMasterScriptSetting::GetValueRange(CT_INT32 iValueID, G_ValueRange & stRange)
{
    return 0 != G_GetValueRange(iValueID, &stRange);
}

// �ճ�������Ϣ
CT_BOOL CCMasterScriptSetting::GetDailyQuest(CT_INT32 iQuestID, G_DailyQuest & stDaily)
{
    return 0 != G_GetDailyQuestData(iQuestID, &stDaily);
}

// ��Ծ�Ƚ�������
CT_BOOL CCMasterScriptSetting::GetDailyQuestReward(CT_INT32 iLevel, G_DailyQuestReward & stReward)
{
    return 0 != G_GetDailyQuestRewardData(iLevel, &stReward);
}

// ǩ����������
CT_BOOL CCMasterScriptSetting::GetSignReward(CT_INT32 nSigned, CT_INT32 iDiamond, G_DailySignIn & stReward)
{
    return 0 != G_GetDailySignInData(nSigned, iDiamond, &stReward);
}

// ��ֵ�һ�����
CT_BOOL CCMasterScriptSetting::GetChargeData(CT_INT32 iRMB, G_ChargeData &stCharge)
{
    return 0 != G_GetChargeData(iRMB, &stCharge);
}