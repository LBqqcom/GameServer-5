/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCMasterScriptSetting.h
Version:		1.0
Description:    �����ű�����
Author:			pengwen
Create Date:	2012-8-16
History:             

******************************************************************/
#ifndef CCMASTERSCRIPTSETTING_H_
#define CCMASTERSCRIPTSETTING_H_ 
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"


struct MasterLevelInfo;
class CCMasterScriptSetting: public CCSingleton<CCMasterScriptSetting>
{
public:
	CCMasterScriptSetting(void);
	~CCMasterScriptSetting(void);
public:
	//�õ�������������
	CT_BOOL GetMasterUpgradeData(CT_INT32 iGrade, MasterLevelInfo& cu);
    // �õ�ѧϰǱ�ܵ�����
    CT_BOOL GetTalentData(CT_INT32 iTalentID, CT_INT32 iTargetID, G_TalentStatus& stLearn);
    // �õ�Ǳ��������
    CT_BOOL GetTalentGroup(CT_INT32 iGroupID, CT_INT32 iTargetID, G_TalentGroup& stGroup);
    // �õ�������������
    CT_BOOL GetRankUpgradeData(CT_INT32 iLevel, MasterLevelInfo& stInfo);
    // �õ�������Ϣ
    CT_BOOL GetRankStatus(CT_INT32 iLevel, G_RankStatus& stInfo);
    // �õ�����ǿ������
    CT_BOOL GetLearnSkillData(CT_INT32 iSkillID, G_SkillMain & stLearn);
    // �õ���ֵ��Χ
    CT_BOOL GetValueRange(CT_INT32 iValueID, G_ValueRange & stRange);
    // �ճ�����
    CT_BOOL GetDailyQuest(CT_INT32 iLevel, G_DailyQuest & stDaily);
    // �ճ�����
    CT_BOOL GetDailyQuestReward(CT_INT32 iLevel, G_DailyQuestReward & stReward);
    // ǩ������
    CT_BOOL GetSignReward(CT_INT32 iLevel, CT_INT32 iDiamond, G_DailySignIn & stReward);
    // ��ֵ�һ�����
    CT_BOOL GetChargeData(CT_INT32 iRMB, G_ChargeData &stCharge);

};

#endif