#include "CUserTalent.h"
#include "CMasterScriptSetting.h"
#include "CActive.h"
#include "CThreadMes.h"
#include "CNetInterface.h"

CT_INT32 CCUserTalent::transData( const TalentData &src, CT_CHAR *pDst ) const
{
    CMD_G_TalentData *pst = (CMD_G_TalentData*)pDst;
    pst->iID = src.iID;
    pst->iStatus = src.iStatus;
    return sizeof(*pst);
}

// 学习潜能
CT_INT32 CCUserTalent::LearnTalent( CT_INT32 iTalentID )
{
    // 检查潜能点数是否足够
    G_TalentStatus talenLearn = {0};
    MasterData md = {0};
    m_pUser->m_userBase.GetMasterData(md);
    if (CCMasterScriptSetting::getSingleton().GetTalentData(iTalentID, UNIT_TYPE_ANY, talenLearn))
    {
        DataMap_IT it = m_DataMap.find(iTalentID);
        if (it != m_DataMap.end()) return ELT_NO_NEED; // 已经学会了！
        if (talenLearn.iTalentNum + md.mvd.stRes.iTalentNum < 0) return ELT_NO_POINT; // 潜能点不足
        if (talenLearn.iMasterGrade > md.mvd.stGrade.iLevel) return ELT_NO_MASTER_GRADE; // 领主等级不足

        CT_BOOL bPrev = CT_FALSE;
        if (talenLearn.iPrevID != 0) {
            for (it = m_DataMap.begin(); it != m_DataMap.end(); ++it) {
                if (talenLearn.iPrevID == it->second.iID && it->second.iStatus == TALENT_ACTIVATED) {
                    bPrev = CT_TRUE;
                    break;
                }
            }
        } else {
            bPrev = CT_TRUE;
        }          

        if (bPrev) {
            // 满足前置要求
            TalentData data = {0};
            data.iID = iTalentID;
            data.iStatus = TALENT_ACTIVATED;
            //memcpy(data.szName, talenLearn.strName, sizeof(data.szName));
            InsertData(data.iID, data);

            // 更新领主资源(潜能点)
            MasterVariableData addValue = {0};
            addValue.stRes.iTalentNum = talenLearn.iTalentNum;
            m_pUser->m_userBase.AddMastData(addValue, SAVETAG_LEARN_TALENT_FINISH);

            // 更新潜能信息
            MSC_DB_W_TalentData wtd = {0};
            wtd.dwUserID = m_pUser->GetUserID();
            wtd.iTalentID = iTalentID;
            memcpy(wtd.szName, talenLearn.strName, sizeof(wtd.szName));
            m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_TALENT_INFO, (CT_CHAR*)&wtd, sizeof(wtd));

            //数据记录
            MSC_LOGDB_TalentData td={0};
            td.dwUserID=m_pUser->GetUserID();
            td.iTalentID=iTalentID;
            td.iExp=talenLearn.iTalentNum;
            m_pUser->SaveLogData(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SAVETALENT, (CT_CHAR*)&td,sizeof(td));

            // 读领主相关数据，看看是否学会的技能
            if (talenLearn.iSkillID == 0 || talenLearn.iTargetID != md.iOccupation)
            {
                CCMasterScriptSetting::getSingleton().GetTalentData(iTalentID, md.iOccupation, talenLearn);
            }

            if (talenLearn.iSkillID != 0)
            {
                // 学会了技能，更新技能数据
                CMD_G_LearnSkill learnSkill = {0};
                learnSkill.dwUserID = m_pUser->GetUserID();
                learnSkill.iSkillID = talenLearn.iSkillID;
                learnSkill.iSkillSoul = 0;
                m_pUser->m_userSkill.LearnSkill(learnSkill);
                //DisposeLearnSkill(learnSkill);
            }

            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);

            m_pUser->m_userBase.GetMasterData(md);
            CMD_G_LearnTalentSuc stSuc = {0};                    
            stSuc.dwUserID = m_pUser->GetUserID();
            stSuc.iTalentID = iTalentID;                                                
            stSuc.iTalentNum = md.mvd.stRes.iTalentNum;
            m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_LEARN_TALENT_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);
            //add by caihan
            //学习潜能成功，判断潜能任务
            CMD_G_TaskEventInfo taskEventInfo = {0};
            taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_POTENTIAL;
            taskEventInfo.iTaskSubType = TASK_SUB_TYPE_POTENTIAL_LEARN;
            taskEventInfo.iSubType = stSuc.iTalentID;
            taskEventInfo.iLevel = 0;
            m_pUser->m_userTask.DisposeTask(taskEventInfo);
            //end
            //判断是否有潜能活动
            CCActive::getSingleton().ActiveProcess(ACTIVE_POTENTIAL_GET,stSuc.iTalentID,m_pUser);

            return ELT_SUCCESS;
        }
        // 前置未学会
        return ELT_NO_PREV_TALENT;
    } else {
        return ELT_DATA_ERROR;
    }
}

CT_VOID CCUserTalent::UpdateBattleStatus(MasterCombatStatus &mcs,  const CT_INT32 *arrayTargets, const CT_INT32 nTarget, const DataMap &talMap)
{
    // 潜能属性
    std::list<CT_INT32> liGrpID;     
    for (DataMap_CONSTIT it = talMap.begin(); it != talMap.end(); ++it)
    {
        G_TalentStatus talStatus = {0};
        CT_INT32 i = 0;
        for (; i < nTarget; ++i) {
            if (0 != G_GetTalentStatus(it->second.iID, arrayTargets[i], &talStatus)) break;
        }
        if (i < nTarget)
        {
            mcs.iStrength      += talStatus.iStrength;
            mcs.iAgility       += talStatus.iAgility;
            mcs.iIntellect     += talStatus.iIntellect;
            mcs.iSpirit        += talStatus.iSpirit;
            mcs.fHitRate       += talStatus.fHitRate;
            mcs.fDodgeRate     += talStatus.fDodgeRate;
            mcs.fCritRate      += talStatus.fCritRate;
            mcs.fResilient     += talStatus.fResilient;
            mcs.fDmgIncPercent += talStatus.fDmgIncPercent;
            mcs.fDmgDecPercent += talStatus.fDmgDecPercent;
            mcs.iMaxHP         += talStatus.iHealth;
            mcs.iMaxMP         += talStatus.iMana;
            mcs.iAttack        += talStatus.iAttack;
            mcs.iDefence       += talStatus.iDefence;
            mcs.fHPPersent     += talStatus.fHealthPercent;
            mcs.iHPRecover     += talStatus.iHPRecover;
            mcs.iMPRecover     += talStatus.iMPRecover;
        }
        if (talStatus.iTalentID == 0) {
            CCMasterScriptSetting::getSingleton().GetTalentData(it->second.iID, -1, talStatus);
        }
        liGrpID.push_back(talStatus.iGroup);
    }
    liGrpID.sort();
    liGrpID.unique();

    // 潜能组属性
    for (std::list<CT_INT32>::iterator it = liGrpID.begin(); it != liGrpID.end(); ++it)
    {
        G_TalentGroup stGroup = {0};

        CT_INT32 i = 0;
        for (; i < nTarget; ++i) {
            if (0 != G_GetTalentGroup(*it, arrayTargets[i], &stGroup)) break;
        }
        if (i < nTarget)
        {
            // 判断前置潜能是否已学会
            CT_BOOL bLearn = CT_FALSE;
            for (DataMap_CONSTIT it = talMap.begin(); it != talMap.end(); ++it)
            {
                if (stGroup.iPrevID == it->second.iID)
                {
                    bLearn = CT_TRUE;
                    break;
                }
            }

            // 前置已学会
            if (bLearn) 
            {
                mcs.iStrength      += stGroup.iStrength;
                mcs.iAgility       += stGroup.iAgility;
                mcs.iIntellect     += stGroup.iIntellect;
                mcs.iSpirit        += stGroup.iSpirit;
                mcs.fHitRate       += stGroup.fHitRate;
                mcs.fDodgeRate     += stGroup.fDodgeRate;
                mcs.fCritRate      += stGroup.fCritRate;
                mcs.fResilient     += stGroup.fResilient;
                mcs.fDmgIncPercent += stGroup.fDmgIncPercent;
                mcs.fDmgDecPercent += stGroup.fDmgDecPercent;
                mcs.iMaxHP         += stGroup.iHealth;
                mcs.iMaxMP         += stGroup.iMana;
                mcs.iAttack        += stGroup.iAttack;
                mcs.iDefence       += stGroup.iDefence;
                mcs.fHPPersent     += stGroup.fHealthPercent;
                mcs.iHPRecover     += stGroup.iHPRecover;
                mcs.iMPRecover     += stGroup.iMPRecover;
            }
        }
    }
}