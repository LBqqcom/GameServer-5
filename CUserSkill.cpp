#include "CUserSkill.h"
#include "CMD_Game.h"
#include "CUser.h"
#include "CThreadMes.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CGDefine.h"
#include "CMasterScriptSetting.h"
#include "CNetInterface.h"


CT_INT32 CCUserSkill::transData( const SkillData &src, CT_CHAR *pDst ) const
{
    CMD_G_SkillData *pst = (CMD_G_SkillData *)pDst;
    pst->iID = src.iID;
    pst->iSkillSoul = src.iSkillSoul;
    pst->Used = src.Used;
    return sizeof(*pst);
}

// 得到装备的技能列表
CT_BOOL CCUserSkill::GetEquipedSkillData(CT_CHAR *pBuf, CT_INT32 &bufLen) const
{
    _AUTOMUTEX(m_pMutex);
    CT_INT32 len = EQUIPED_SKILL_COUNT * sizeof(CMD_G_SkillData);
    if (len > bufLen)
    {
		return CT_FALSE;
    }
    
    len = 0;
    for (DataMap_CONSTIT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        if (it->second.Used != 0) 
        {
            CMD_G_SkillData skilldata = {0};
            len += transData(it->second, pBuf + sizeof(CMD_G_SkillData) * (it->second.Used - 1));
        }
    }
    bufLen = len;

    return CT_TRUE;
}

// 装备技能
CT_INT32 CCUserSkill::EquipSkill(const CT_INT32 skillIDs[])
{
    _AUTOMUTEX(m_pMutex);
    CT_INT32 enFail = EES_SUCCESS;

    // 遍历，判断技能是否存在
    for (int i = 0; i < EQUIPED_SKILL_COUNT; ++i)
    {
        if (skillIDs[i] != 0 && !HasData(skillIDs[i]))
        {
            enFail = EES_NO_SKILL;
            goto FINISH;
        }
    }

	//判断位置是否正确
	for (int i = 0; i < EQUIPED_SKILL_COUNT; ++i)
	{
		if(skillIDs[i] == 0)continue;
		G_SkillMain SkillInfo={0};
		if(G_GetSkillMainData(skillIDs[i],&SkillInfo))
		if (SkillInfo.iSkillPos != i+1)
		{
			enFail = EES_POS_ERROR;
			goto FINISH;
		}
	}

    // 遍历，更新装备状态改变的技能 
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        CT_BOOL bEquiped = CT_FALSE;
        CT_BOOL bNeedUpdate = CT_FALSE;
        for (int i = 0; i < EQUIPED_SKILL_COUNT; ++i)
        {
            if (skillIDs[i] == 0) continue;

            if (it->second.iID == skillIDs[i])
            {
                if (it->second.Used != i + 1)
                {
                    it->second.Used = i + 1;
                    bNeedUpdate = CT_TRUE;
                }
                bEquiped = CT_TRUE;
                break;
            }
        }

        // 未装备但装备位置不为0，修改为未装备
        if (!bEquiped && it->second.Used != 0)
        {
            it->second.Used = 0;
            bNeedUpdate = CT_TRUE;
        }
        
        if (bNeedUpdate)
        {
            // 更新到数据库
            MSC_DB_W_SkillData wsd = {0};
            wsd.dwUserID = m_pUser->GetUserID();
            wsd.iSkillID = it->second.iID;
            wsd.iGradeInc = 0;
            wsd.Used = it->second.Used;
            memcpy(wsd.szName, it->second.szName, sizeof(wsd.szName));
            CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
            G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_SKILL_INFO , (CT_CHAR*)&wsd, sizeof(wsd));
        }        
    }
	//add by caihan
	//设置队形，判断阵型任务
	for (CT_INT32 i=0;i<EQUIPED_SKILL_COUNT;i++)
	{
		if (skillIDs[i]>0)
		{
			CMD_G_TaskEventInfo taskEventInfo = {0};
			taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_FIGHT;
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_FIGHT_SKILL;
			taskEventInfo.iSubType = skillIDs[i];
			m_pUser->m_userTask.DisposeTask(taskEventInfo);
		}
	}
	//end
FINISH:
    return enFail;
}

CT_INT32 CCUserSkill::LearnSkill(const CMD_G_LearnSkill &learn )
{
    G_SkillMain skillLearn = {0};
    MasterVariableData mvd = {0};
    m_pUser->m_userBase.GetMasterData(mvd);
    if (CCMasterScriptSetting::getSingleton().GetLearnSkillData(learn.iSkillID, skillLearn))
    {
        // 检索领主技能信息
        SkillData stSkill = {0};                
        GetData(learn.iSkillID, stSkill);

        // 注入技魂为零，为新学技能。检查是否已经学习过
        if (learn.iSkillSoul == 0)
        {
            if (stSkill.iID != 0) return ELS_NO_NEED;
        }
        // 给技能注魂
        else if (mvd.stRes.iSkillSoul < learn.iSkillSoul) 
        {
            // 击魂不足
            return ELS_NO_POINT;
        }           

        // 判断是否新技能格子的技能
        if (skillLearn.iSkillPos > mvd.stRes.iSkillSlots) {
            stSkill.Used = skillLearn.iSkillPos;
            m_pUser->m_userBase.IncSkillSlot();
        }

        // 满足条件
        stSkill.iID = learn.iSkillID;
        stSkill.iSkillSoul += learn.iSkillSoul;
        m_pUser->m_userSkill.InsertData(stSkill.iID, stSkill);

        // 更新领主资源(潜能点)
        MasterVariableData addValue = {0};
        addValue.stRes.iSkillSoul = -learn.iSkillSoul;  // 此处为消耗
        m_pUser->m_userBase.AddMastData(addValue, SAVETAG_LEARN_SKILL_FINISH);

        // 更新技能列表
        MSC_DB_W_SkillData wsd = {0};
        wsd.dwUserID = learn.dwUserID;
        wsd.iSkillID = learn.iSkillID;
        wsd.iGradeInc = learn.iSkillSoul;
        wsd.Used = stSkill.Used;
        memcpy(wsd.szName, skillLearn.szName, sizeof(wsd.szName));
        //add by caihan
        //技能强化任务
        if (learn.iSkillSoul>0)
        {
            CMD_G_TaskEventInfo taskEventInfo = {0};
            taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_POTENTIAL;
            taskEventInfo.iTaskSubType = TASK_SUB_TYPE_POTENTIAL_STRNETH;
            taskEventInfo.iSubType = learn.iSkillID;
            m_pUser->m_userTask.DisposeTask(taskEventInfo);
        }
        //end      
        m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_SKILL_INFO , (CT_CHAR*)&wsd, sizeof(wsd));

        m_pUser->m_userBase.GetMasterData(mvd);
        CMD_G_LearnSkillSuc suc = {0};
        suc.dwUserID = m_pUser->GetUserID();
        suc.iSkillID = stSkill.iID;
        suc.iSkillLevel = stSkill.iSkillSoul;
        suc.iSkillSoul = mvd.stRes.iSkillSoul;
        suc.iUsed = stSkill.Used;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_LEARN_SKILL_SUC, (CT_CHAR *)&suc, sizeof(suc), false, true);
    } else {
        return ELS_DATA_ERROR;
    }
    return ELS_SUCCESS;
}