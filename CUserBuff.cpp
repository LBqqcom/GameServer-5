#include <time.h>
#include "CUserBuff.h"
#include "CDatabaseMes.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CItemScriptSetting.h"
#include "CNetInterface.h"

CT_INT32 CCUserBuff::transData(const BuffData &src, CT_CHAR *pDst) const
{
    CMD_G_BuffData *pst = (CMD_G_BuffData*)pDst;
    pst->iGroup = src.iGroup;
    pst->iID = src.iID;
    // 计算剩余时间
    pst->leftTime = src.iDuration - (time(0) - src.toggleTime);
    return sizeof(*pst);
}

CT_VOID CCUserBuff::SyncData(CT_INT64 currTime, CT_BOOL bForce /* = CT_FALSE*/)
{
    _AUTOMUTEX(m_pMutex);

    G_ValueRange vr = {0};
    G_GetValueRange(VALUE_TYPE_ITEM_SYNC_THRESHOLDS, &vr);
    CT_INT32 interval = (CT_INT32)vr.fMaxVal;
    if (interval == 0) interval = 600;
    if (bForce || (currTime - m_lastSyncTime) > interval) {
        MSC_DB_SyncBuffData stDbData = {0};
        stDbData.dwUserID = m_pUser->GetUserID();
        DataMap_IT it = m_DataMap.begin();
        for (CT_INT32 i = 0; it != m_DataMap.end(); ++it, ++i) {
			if (i < BUFF_SLOTS)
	            stDbData.stBuffs[i] = it->second;
        }
        m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_BUFF_SYNC, (CT_CHAR*)&stDbData, sizeof(stDbData));
        m_lastSyncTime = currTime;
    }
}

// 更新增益状态
CT_VOID CCUserBuff::UpdateItemBuff(void)
{
    _AUTOMUTEX(m_pMutex);

    time_t currTime = time(0);        
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end();)
    {
        DataMap_IT itTmp = it;
        ++it;
        G_ItemBuff itBuff = {0};
        CCItemScriptSetting::getSingleton().GetItemBuff(itTmp->second.iID, itBuff);
        if (itTmp->second.toggleTime + itTmp->second.iDuration <= currTime) {

            CMD_G_UserItemBuff buff = {0};
            buff.dwUserID = m_pUser->GetUserID();
            buff.iGroup = itBuff.iGroup;
            buff.iID = itBuff.iID;    
            // 发生了更新，通知客户端
            m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_LOSE_BUFF, (CT_CHAR *)&buff, sizeof(buff), false, true);

            m_DataMap.erase(itTmp);

            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);            
        }
    }    
}

// 查找增益是否存在
CT_BOOL CCUserBuff::SearchBuffData(const G_ItemBuff &itBuff, BuffData &buffData)
{
    _AUTOMUTEX(m_pMutex);

    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        G_ItemBuff itBf = {0};
        CCItemScriptSetting::getSingleton().GetItemBuff((*it).second.iID, itBf);
        if (itBf.iGroup == itBuff.iGroup) 
        {
            buffData = (*it).second;
            return CT_TRUE;
        }        
    }
    return CT_FALSE;
}

CT_VOID CCUserBuff::BuffReward(CT_INT32 iID, CT_INT64 iBeginTime)
{
	_AUTOMUTEX(m_pMutex);
	G_ItemBuff stItem = {0};
	if (CCItemScriptSetting::getSingleton().GetItemBuff(iID, stItem))
	{
		// 检查能否使用
		BuffData buffData = {0};
		if (SearchBuffData(stItem, buffData))
		{
			return;
		}

		if (time(0) - iBeginTime >= stItem.iDuration)
		{
			return;
		}

		// 获得增益
		BuffData bfData = {0};
		bfData.iID = iID;
		bfData.iGroup = stItem.iGroup;
		bfData.iDuration = stItem.iDuration;
		bfData.toggleTime = iBeginTime;
		m_pUser->m_userBuff.InsertData(bfData.iGroup, bfData);            
		m_pUser->UpdateFactorData();

		// 通知客户端
		CMD_G_UserItemBuff buff = {0};
		buff.dwUserID = m_pUser->GetUserID();
		buff.iGroup = bfData.iGroup;
		buff.iID = bfData.iID;
		buff.leftTime = bfData.iDuration - (time(0) - bfData.toggleTime);
		m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_BUFF, (CT_CHAR *)&buff, sizeof(buff), false, true);

		m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
	}
}

CT_VOID CCUserBuff::UpdateBattleStatus(MasterCombatStatus &mcs, const DataMap &buffMap)
{
    for (DataMap_CONSTIT it = buffMap.begin(); it != buffMap.end(); ++it)
    {
        G_ItemBuff buff = {0};
        if (CCItemScriptSetting::getSingleton().GetItemBuff(it->second.iID, buff))
        {
            for (CT_INT32 j = 0; j < ATTR_TYPES; ++j)
            {
                CCUserBase::UpdateBattleStatus(mcs, buff.Attr[j].iAID, buff.Attr[j].fValue);
            }
        }
    }
}