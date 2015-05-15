// Create [9/20/2012 John]

#include "CDBMasterEvent.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBLogMes.h"
#include <string>

CCDBMasterEvent::CCDBMasterEvent(CCGameMesManager* pMesManager, CT_DWORD id,
                                         CCDBConnector& dbConnector) : CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_TALENT_INFO);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_SKILL_INFO);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ENCHANT_DATA);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ITEM_SYNC); 
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_BUFF_SYNC); 
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_EQUIP_SYNC);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ELM_SYNC);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ELMATTR_SYNC);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_BATTLEFLAG_INFO);  
	RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ENCHANT_FAILTIME);
	RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ENCHANT_REWARD);
	RegistMessage(MSC_MAIN_DB_SAVE_DATA,	 MSC_SUB_DB_W_TITLE_DATA);

	G_ValueRange vr = {0};
	if (G_GetValueRange(VALUE_TYPE_TITLELIST_TIME, &vr))
	{
		if (m_dwThread == GAME_THREAD_DATABASE)
		{
			RegistTimer(IDI_TITLE_LIST_TIME);
			SetTimer(IDI_TITLE_LIST_TIME,(CT_DWORD)(vr.fMaxVal*1000));

			RegistClock(IDI_TITLE_LIST_UPDATE);
			SetClock(IDI_TITLE_LIST_UPDATE, 0, 0, 0);
		}
	}
}

CCDBMasterEvent::~CCDBMasterEvent(void)
{
}

//处理领主消息
CT_VOID CCDBMasterEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
                                         CT_CHAR* pBuf,		CT_DWORD len,
                                         CT_LONGLONG reserve)
{
    switch (mainID)
    {
    case  MSC_MAIN_DB_SAVE_DATA:
        {
            DisposeSaveData(subID, pBuf, len);
            break;
        }
    default:
        {
            CCLogMeg::getSingleton().WriteError("CCDBMasterEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
            break;
        }
    }
}

CT_VOID CCDBMasterEvent::OnTime(CT_DWORD timeID)
{
	if (timeID == IDI_TITLE_LIST_TIME)
	{
		KillTimer(IDI_TITLE_LIST_TIME);
		DisposeTitleListSave();
	}
}

CT_VOID CCDBMasterEvent::OnClock(CT_DWORD timeID)
{
	if (timeID == IDI_TITLE_LIST_UPDATE)
	{
		DisposeTitleListSave();
		G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_MASTER, MSC_SUB_TITLELIST_UPDATE,0,0);
	}
}

// 处理信息保存
CT_VOID CCDBMasterEvent::DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len)
{
    switch (subID)            
    {
    case MSC_SUB_DB_W_TALENT_INFO :
        {
            if (len != sizeof(MSC_DB_W_TalentData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_TalentSaveData)");
                return ;
            }                 
            DisposeTalentSaveData(*(MSC_DB_W_TalentData*)pBuf);
            break;
        }
    case MSC_SUB_DB_W_SKILL_INFO :
        {
            if (len != sizeof(MSC_DB_W_SkillData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_SkillSaveData)");
                return ;
            }                 
            DisposeSkillSaveData(*(MSC_DB_W_SkillData*)pBuf);
            break;
        }
    case MSC_SUB_DB_W_ENCHANT_DATA :
        {
            if (len != sizeof(MSC_DB_W_EnchantData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_W_EnchantData)");
                return ;
            }
            DisposeUpdateEnchant(*(MSC_DB_W_EnchantData*)pBuf);
        }
        break;
    case MSC_SUB_DB_W_ITEM_SYNC :
        {
            if (len != sizeof(MSC_DB_SyncItemData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_SyncItemData)");
                return ;
            }
            DisposeSyncItemData(*(MSC_DB_SyncItemData*)pBuf);
        } break;
    case MSC_SUB_DB_W_BUFF_SYNC :
        {
            if (len != sizeof(MSC_DB_SyncBuffData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_SyncBuffData)");
                return ;
            }
            DisposeSyncBuffData(*(MSC_DB_SyncBuffData*)pBuf);
        } break;
    case MSC_SUB_DB_W_EQUIP_SYNC :
        {
            if (len != sizeof(MSC_DB_SyncEquipData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_SyncEquipData)");
                return ;
            }
            DisposeSyncEquipData(*(MSC_DB_SyncEquipData*)pBuf);
        } break;
    case MSC_SUB_DB_W_ELM_SYNC :
        {
            if (len != sizeof(MSC_DB_SyncItemData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_SyncItemData)");
                return ;
            }
            DisposeSyncElmData(*(MSC_DB_SyncItemData *)pBuf);
        } break;
    case MSC_SUB_DB_W_ELMATTR_SYNC:
        {
            if (len != sizeof(MSC_DB_SyncElmAttrData))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_SyncElmAttrData)");
                return ;
            }
            DisposeSyncElmAttrData(*(MSC_DB_SyncElmAttrData *)pBuf);
        } break;
    case MSC_SUB_DB_W_BATTLEFLAG_INFO:
        {
            if (len != sizeof(MSC_DB_SaveBattleFlag))
            {
                G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_SaveBattleFlag)");
                return ;
            }
            DisposeBattleFlagSaveData(*(MSC_DB_SaveBattleFlag *)pBuf);
        } break;
	case MSC_SUB_DB_W_ENCHANT_FAILTIME:
		{
			if (len != sizeof(MSC_DB_EnchantFailTime))
			{
				G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_EnchantFailTime)");
				return;
			}
			DisposeUpdataEnchantFail(*(MSC_DB_EnchantFailTime*)pBuf);
		}break;
	case MSC_SUB_DB_W_ENCHANT_REWARD:
		{
			if (len != sizeof(MSC_DB_EnchantReward))
			{
				G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_EnchantReward)");
				return;
			}
			DisposeUpdataEnchantReward(*(MSC_DB_EnchantReward*)pBuf);
		}break;
	case MSC_SUB_DB_W_TITLE_DATA:
		{
			if (len != sizeof(MSC_DB_TitleData))
			{
				G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_TitleData)");
				return;
			}
			DisposeUpdataTitleData(*(MSC_DB_TitleData*)pBuf);
			break;
		}
    }
    
}

//////////////////////////////////////////////////////////////////////////
// 保存潜能信息
CT_VOID CCDBMasterEvent::DisposeTalentSaveData(const MSC_DB_W_TalentData& talentSaveData)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_SaveTalent");

    m_dbConnector.AddParm("_uid",   talentSaveData.dwUserID);
    m_dbConnector.AddParm("_tid",   talentSaveData.iTalentID);
    m_dbConnector.AddParm("_name",  (CT_CHAR *)talentSaveData.szName);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }else
    {
		CCDBLogMes::getSingleton().WriteInfo(talentSaveData.dwUserID,"SP_SaveTalent","_uid:%d _tid:%d _name:%s",talentSaveData.dwUserID,talentSaveData.iTalentID,talentSaveData.szName);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }
}
// 保存技能信息
CT_VOID CCDBMasterEvent::DisposeSkillSaveData(const MSC_DB_W_SkillData& skillSaveData)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_SaveSkill");

    m_dbConnector.AddParm("_uid",       skillSaveData.dwUserID);
    m_dbConnector.AddParm("_sid",       skillSaveData.iSkillID);
    m_dbConnector.AddParm("_name",      (CT_CHAR *)skillSaveData.szName);
    m_dbConnector.AddParm("_gradeInc",  skillSaveData.iGradeInc);
    m_dbConnector.AddParm("_use",       skillSaveData.Used);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }else
    {
		CCDBLogMes::getSingleton().WriteInfo(skillSaveData.dwUserID,"SP_SaveSkill","_uid:%d _sid:%d _name:%s _gradeInc:%d _use:%d",skillSaveData.dwUserID,skillSaveData.iSkillID,skillSaveData.szName,skillSaveData.iGradeInc,skillSaveData.Used);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }
}

// 同步道具信息
CT_VOID  CCDBMasterEvent::DisposeSyncItemData(const MSC_DB_SyncItemData &syncData)
{
    const CT_INT32 COUNT = sizeof(syncData.stItems) / sizeof(*syncData.stItems);    
    CT_CHAR szLine[1024] = {0};   
    std::string strCmd;

    // ItemData
    sprintf(szLine, "delete from prop where U_ID=%u;", syncData.dwUserID);
    strCmd.append(szLine);
    for (CT_INT32 i = 0; i < COUNT; ++i) {
        ItemData it = syncData.stItems[i];
        if (it.iID != 0 && it.iIndex > 0 && it.iCount > 0) 
		{
            sprintf(szLine, "insert into prop(P_Index,U_ID,P_ID,P_Name,P_Num,P_Pos) values(%u,%u,%d,'%s',%d,%d);", 
                it.key, syncData.dwUserID, it.iID, it.szName, it.iCount, it.iIndex);
            strCmd.append(szLine);
        }
    }

    m_dbConnector.Reset();
    if(m_dbConnector.Exec(const_cast<CT_CHAR *>(strCmd.c_str()), strCmd.length() + 1)) {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult) {
            pResult->Release();
        }
    } else {
        CCDBLogMes::getSingleton().WriteInfo(syncData.dwUserID,"SyncItemData","_uid:%d",syncData.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
    }
}

// 同步增益信息
CT_VOID CCDBMasterEvent::DisposeSyncBuffData(const MSC_DB_SyncBuffData &syncData)
{
    const CT_INT32 COUNT = sizeof(syncData.stBuffs) / sizeof(*syncData.stBuffs);    
    CT_CHAR szLine[1024] = {0};   
    std::string strCmd;
    
    sprintf(szLine, "delete from propusing where U_ID = %u;", syncData.dwUserID);
    strCmd.append(szLine);
    for (CT_INT32 i = 0; i < sizeof(syncData.stBuffs) / sizeof(*syncData.stBuffs); ++i) {
        BuffData it = syncData.stBuffs[i];
        if (it.iGroup != 0) {
            sprintf(szLine, "insert into propusing(U_ID,P_ID,P_BeginTime,P_LastingTime) values(%u,%d,%lld,%d);", 
                syncData.dwUserID, it.iID, it.toggleTime, it.iDuration);     
            strCmd.append(szLine);
        }
    }

    m_dbConnector.Reset();
    if(m_dbConnector.Exec(const_cast<CT_CHAR *>(strCmd.c_str()), strCmd.length() + 1)) {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult) {
            pResult->Release();
        }
    } else {
        CCDBLogMes::getSingleton().WriteInfo(syncData.dwUserID,"SyncBuffData","_uid:%d",syncData.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
    }
}

// 同步装备信息
CT_VOID CCDBMasterEvent::DisposeSyncEquipData(const MSC_DB_SyncEquipData &syncData)
{
    const CT_INT32 COUNT = sizeof(syncData.stEquips) / sizeof(*syncData.stEquips);    
    CT_CHAR szLine[1024] = {0};   
    std::string strCmd;

    // EquipData - Strengthen & Slot
    sprintf(szLine, "delete from propstrengthen where U_ID=%u;", syncData.dwUserID);
    strCmd.append(szLine);
    for (CT_INT32 i = 0; i < COUNT; ++i) {
        EquipmentData it = syncData.stEquips[i];
        if (it.key != 0) {
            sprintf(szLine, "insert into propstrengthen(U_ID,P_Index,P_ID,P_Grade,P_Holes) values(%u,%u,%d,%d,%d);", 
                syncData.dwUserID, it.key, 0, it.iLevel,it.nSlot);
            strCmd.append(szLine);
        }
    }

    m_dbConnector.Reset();
    if(m_dbConnector.Exec(const_cast<CT_CHAR *>(strCmd.c_str()), strCmd.length() + 1)) {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult) {
            pResult->Release();
        }
    } else {
        CCDBLogMes::getSingleton().WriteInfo(syncData.dwUserID,"SyncEquipData:strength","_uid:%d",syncData.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
    }

    // EquipData - Inlay
    strCmd.clear();
    sprintf(szLine, "delete from propinlay where U_ID=%u;", syncData.dwUserID);
    strCmd.append(szLine);
    for (CT_INT32 i = 0; i < COUNT; ++i) {
        EquipmentData it = syncData.stEquips[i];
        if (it.key != 0) {
            for (CT_INT32 j = 0; j < it.nSlot; ++j) {
                sprintf(szLine, "insert into propinlay(U_ID,P_IndexMain,P_IDAdd,P_Pos) values(%u,%u,%d,%d);", 
                    syncData.dwUserID, it.key, it.Gem[j], j);
                strCmd.append(szLine);
            }
        }
    }

    m_dbConnector.Reset();
    if(m_dbConnector.Exec(const_cast<CT_CHAR *>(strCmd.c_str()), strCmd.length() + 1)) {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult) {
            pResult->Release();
        }
    } else {
        CCDBLogMes::getSingleton().WriteInfo(syncData.dwUserID,"SyncEquipData:inlay","_uid:%d",syncData.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
    }

    // EquipData - Train
    strCmd.clear();
    sprintf(szLine, "delete from proptrain where U_ID=%u;", syncData.dwUserID);
    strCmd.append(szLine);
    for (CT_INT32 i = 0; i < COUNT; ++i) {
        EquipmentData it = syncData.stEquips[i];
        if (it.key != 0) {
            CT_BOOL bHasData = CT_FALSE;
            CT_INT32 attrs[] = FORGE_ATTR_ARRAY;
            CT_FLOAT iattr[9] = {0};
            for (CT_INT32 j = 0; j < FORGE_ATTRS; ++j) {
                for (CT_INT32 k = 0; k < 9; ++k) {
                    if (it.Forge[j].iAID == attrs[k]) {
                        iattr[k] = it.Forge[j].fValue;
                        bHasData = CT_TRUE;
                        break;
                    }
                }
            }
            if (bHasData) {
                sprintf(szLine, "insert into proptrain(U_ID,P_Index,P_Strength,P_Agility,P_Interllect,P_Spirit,P_Hp,P_HitRate,P_DodgeRate,P_CritRate,P_Resilient)"
                    " values(%u,%u,%d,%d,%d,%d,%f,%f,%f,%f,%f);", 
                    syncData.dwUserID, it.key, (CT_INT32)iattr[0], (CT_INT32)iattr[1], (CT_INT32)iattr[2], (CT_INT32)iattr[3], 
                    iattr[4], iattr[5], iattr[6], iattr[7], iattr[8]);
                strCmd.append(szLine);
            }
        }
    }

    m_dbConnector.Reset();
    if(m_dbConnector.Exec(const_cast<CT_CHAR *>(strCmd.c_str()), strCmd.length() + 1)) {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult) {
            pResult->Release();
        }
    } else {
        CCDBLogMes::getSingleton().WriteInfo(syncData.dwUserID,"SyncEquipData:train","_uid:%d",syncData.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
    }
}

// 同步元素信息
CT_VOID CCDBMasterEvent::DisposeSyncElmData(const MSC_DB_SyncItemData &syncData)
{
    const CT_INT32 COUNT = sizeof(syncData.stItems) / sizeof(*syncData.stItems);    
    CT_CHAR szLine[1024] = {0};   
    std::string strCmd;

    // ItemData - elm
    sprintf(szLine, "delete from elmprop where U_ID=%u;", syncData.dwUserID);
    strCmd.append(szLine);
    for (CT_INT32 i = 0; i < COUNT; ++i) {
        ItemData it = syncData.stItems[i];
        if (it.iID != 0 && it.iIndex > 0 && it.iCount > 0) {
            sprintf(szLine, "insert into elmprop(E_Index,U_ID,E_ID,E_Name,E_Num,E_Pos) values(%u,%u,%d,'%s',%d,%d);", 
                it.key, syncData.dwUserID, it.iID, it.szName, it.iCount, it.iIndex);
            strCmd.append(szLine);
        }
    }

    m_dbConnector.Reset();
    if(m_dbConnector.Exec(const_cast<CT_CHAR *>(strCmd.c_str()), strCmd.length() + 1)) {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult) {
            pResult->Release();
        }
    } else {
        CCDBLogMes::getSingleton().WriteInfo(syncData.dwUserID,"SyncElmData","_uid:%d",syncData.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
    }
}

// 同步元素属性
CT_VOID CCDBMasterEvent::DisposeSyncElmAttrData(const MSC_DB_SyncElmAttrData &syncData)
{
    const CT_INT32 COUNT = sizeof(syncData.stDatas) / sizeof(*syncData.stDatas);    
    CT_CHAR szLine[1024] = {0};   
    std::string strCmd;

    // ItemData - elm
    sprintf(szLine, "delete from elmattr where U_ID=%u;", syncData.dwUserID);
    strCmd.append(szLine);
    for (CT_INT32 i = 0; i < COUNT; ++i) {
        ElmMarrowAttr it = syncData.stDatas[i];
        if (it.key != 0) {
            sprintf(szLine, "insert into elmattr(U_ID,E_KEY,E_Level,E_Point,E_Value1,E_Value2) values(%u,%u,%d,%d,%f,%f);", 
                syncData.dwUserID, it.key, it.iLevel, it.iExp, it.attrs[0], it.attrs[1]);
            strCmd.append(szLine);
        }
    }

    m_dbConnector.Reset();
    if(m_dbConnector.Exec(const_cast<CT_CHAR *>(strCmd.c_str()), strCmd.length() + 1)) {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult) {
            pResult->Release();
        }
    } else {
        CCDBLogMes::getSingleton().WriteInfo(syncData.dwUserID,"SyncElmData","_uid:%d",syncData.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
    }
}

// 保存战旗信息
CT_VOID CCDBMasterEvent::DisposeBattleFlagSaveData(const MSC_DB_SaveBattleFlag &saveData)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_BattleFlagUpdate");

    m_dbConnector.AddParm("_uid", saveData.dwUserID);
    m_dbConnector.AddParm("_type", saveData.iType);
    m_dbConnector.AddParm("_level", saveData.iAdd);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            pResult->Release();
        }
    }else
    {
        CCDBLogMes::getSingleton().WriteInfo(saveData.dwUserID, "SP_BattleFlagUpdate", 
            "_uid:%d _type:%d _level:%d",
            saveData.dwUserID, saveData.iType, saveData.iAdd);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }

    return;
}

// 添加充值记录
CT_VOID CCDBMasterEvent::DisposeAddCharge(const MSC_DB_A_Charge &addCharge)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_ChargeRecordAdd");

    m_dbConnector.AddParm("_uid", addCharge.dwUserID);
    m_dbConnector.AddParm("_money", addCharge.iPrice);
    m_dbConnector.AddParm("_diamod", addCharge.iDiamond);
    m_dbConnector.AddParm("_state", addCharge.iState);
    m_dbConnector.AddParm("_sign", (CT_CHAR*)addCharge.szSign);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            pResult->Release();
        }
    }else
    {
        CCDBLogMes::getSingleton().WriteInfo(addCharge.dwUserID, "SP_ChargeRecordAdd", 
            "_uid:%d _money:%d _diamod:%d _state:%d _sign:%s",
            addCharge.dwUserID, addCharge.iPrice, addCharge.iDiamond, addCharge.iState, addCharge.szSign);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }

    return;
}
// 更新充值记录
CT_VOID CCDBMasterEvent::DisposeUpdateCharge(const MSC_DB_U_Charge &updateCharge)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_ChargeRecordUpdate");

    m_dbConnector.AddParm("_sign", (CT_CHAR *)updateCharge.szSign);
    m_dbConnector.AddParm("_cIndex", updateCharge.iIndex);
    m_dbConnector.AddParm("_state", updateCharge.iState);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            pResult->Release();
        }
    }else
    {
        CCDBLogMes::getSingleton().WriteInfo(0, "SP_ChargeRecordUpdate", 
            "_sign:%s _cIndex:%d _state:%d",updateCharge.szSign, updateCharge.iIndex, updateCharge.iState);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }

    return;
}

// 更新附魔记录
CT_VOID CCDBMasterEvent::DisposeUpdateEnchant(const MSC_DB_W_EnchantData &enchantData)
{
    const int ATTR_COUNT = sizeof(enchantData.iData) / sizeof(*enchantData.iData);
    CT_CHAR partNames[ATTR_COUNT][16] = {
        "_head1", "_head2", "_head3", 
        "_shoulder1", "_shoulder2", "_shoulder3",
        "_body1", "_body2", "_body3",
        "_hand1", "_hand2", "_hand3",
        "_waist1", "_waist2", "_waist3",
        "_foot1", "_foot2", "_foot3"
    };
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_UserEnchantUpdate");

    m_dbConnector.AddParm("_uid", enchantData.dwUserID);
    for (int i = 0; i < ATTR_COUNT; ++i) {
        m_dbConnector.AddParm(partNames[i], enchantData.iData[i]);
    }

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            pResult->Release();
        }
    }else
    {
        CT_CHAR szLog[256] = "";
        sprintf(szLog, "%s%d ", "_uid", enchantData.dwUserID);
        for (int i = 0; i < ATTR_COUNT; ++i) {
            CT_CHAR szSeg[32] = "";
            sprintf(szSeg, "%s%d ", partNames[i], enchantData.iData[i]);
            strcat(szLog, szSeg);
        }
        CCDBLogMes::getSingleton().WriteInfo(0, "SP_UserEnchantUpdate", "%s", szLog);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }

    return;
}

// 更新附魔失败记录
CT_VOID CCDBMasterEvent::DisposeUpdataEnchantFail(const MSC_DB_EnchantFailTime &enchantFail)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EnchantFailUpdate");

	m_dbConnector.AddParm("_uid",	enchantFail.dwUserID);
	m_dbConnector.AddParm("_bid",	enchantFail.iBID);
	m_dbConnector.AddParm("_idx",	enchantFail.iIDX);
	m_dbConnector.AddParm("_failTime", enchantFail.iFailCount);

	if (m_dbConnector.Exec())
	{
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_UserEnchantFail","_uid = %d,_bid = %d,_idx = %d,_failTime = %d",
			enchantFail.dwUserID, enchantFail.iBID, enchantFail.iIDX, enchantFail.iFailCount);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	return;
}

// 更新附魔奖励记录
CT_VOID CCDBMasterEvent::DisposeUpdataEnchantReward(const MSC_DB_EnchantReward &enchantReward)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EnchantRewardAdd");

	m_dbConnector.AddParm("_uid",	enchantReward.dwUserID);
	m_dbConnector.AddParm("_index",	enchantReward.iIndex);

	if (m_dbConnector.Exec())
	{
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(0,"SP_EnchantRewardAdd", "_uid = %d,_index = %d", enchantReward.dwUserID,enchantReward.iIndex);
	    G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

// 更新头衔数据
CT_VOID CCDBMasterEvent::DisposeUpdataTitleData(const MSC_DB_TitleData &syncData)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TitleUpdata");

	m_dbConnector.AddParm("_uid",			syncData.dwUserID);
	m_dbConnector.AddParm("_tid",			syncData.iTitleID);
	m_dbConnector.AddParm("_status",		syncData.iStatus);
	m_dbConnector.AddParm("_activeTime",	syncData.iActiveTime);
	m_dbConnector.AddParm("_effectiveTime",	syncData.iEffectTime);

	if (m_dbConnector.Exec())
	{
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(0,"SP_TitleUpdata", "_uid = %d,_tid = %d", syncData.dwUserID, syncData.iTitleID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

// 起服时读取头衔列表
CT_VOID CCDBMasterEvent::DisposeTitleListSave()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetTitleList");

	if (m_dbConnector.Exec())
	{
		CT_CHAR buf[2048] = {0};
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			CT_INT32 iNum = 0;
			CT_INT32 iTitleID = 0;
			while (pResult->MoveNextRow())
			{
				iTitleID = 0;
				pResult->GetValue(0, iTitleID);

				memcpy(buf + iNum * sizeof(int), &iTitleID, sizeof(iTitleID));
				++iNum;
			}
			G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_MASTER, MSC_SUB_TITLELIST_GET, buf, iNum * sizeof(CT_INT32));
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}