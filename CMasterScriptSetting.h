/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCMasterScriptSetting.h
Version:		1.0
Description:    领主脚本配置
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
	//得到领主升级数据
	CT_BOOL GetMasterUpgradeData(CT_INT32 iGrade, MasterLevelInfo& cu);
    // 得到学习潜能的条件
    CT_BOOL GetTalentData(CT_INT32 iTalentID, CT_INT32 iTargetID, G_TalentStatus& stLearn);
    // 得到潜能组属性
    CT_BOOL GetTalentGroup(CT_INT32 iGroupID, CT_INT32 iTargetID, G_TalentGroup& stGroup);
    // 得到军衔提升数据
    CT_BOOL GetRankUpgradeData(CT_INT32 iLevel, MasterLevelInfo& stInfo);
    // 得到军衔信息
    CT_BOOL GetRankStatus(CT_INT32 iLevel, G_RankStatus& stInfo);
    // 得到技能强化条件
    CT_BOOL GetLearnSkillData(CT_INT32 iSkillID, G_SkillMain & stLearn);
    // 得到数值范围
    CT_BOOL GetValueRange(CT_INT32 iValueID, G_ValueRange & stRange);
    // 日常任务
    CT_BOOL GetDailyQuest(CT_INT32 iLevel, G_DailyQuest & stDaily);
    // 日常奖励
    CT_BOOL GetDailyQuestReward(CT_INT32 iLevel, G_DailyQuestReward & stReward);
    // 签到奖励
    CT_BOOL GetSignReward(CT_INT32 iLevel, CT_INT32 iDiamond, G_DailySignIn & stReward);
    // 充值兑换数据
    CT_BOOL GetChargeData(CT_INT32 iRMB, G_ChargeData &stCharge);

};

#endif