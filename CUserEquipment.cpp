#include <ctime>
#include "CUserEquipment.h"
#include "CThreadMes.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "CItemScriptSetting.h"
#include "CRand.h"
#include "CNetInterface.h"


CT_INT32 CCUserEquipment::transData( const EquipmentData &src, CT_CHAR *pDst ) const
{
    CMD_G_EquipData *pst = (CMD_G_EquipData *)pDst;
    pst->key = src.key;        
    pst->iLevel = src.iLevel;
    pst->nSlot = src.nSlot;
    memcpy(&pst->Gem, &src.Gem, sizeof(pst->Gem));
    memcpy(&pst->Forge, &src.Forge, sizeof(pst->Forge));
    return sizeof(*pst);
}

CT_VOID CCUserEquipment::Init(CCUser* pUser, CCMutex* pMutex)
{
    m_pUser = pUser;
    m_pMutex = pMutex;
	m_DataMap.clear();
    m_EquipForgeTempDataList.clear();
}

//清理用户数据
CT_VOID CCUserEquipment::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_DataMap.clear();
    m_EquipForgeTempDataList.clear();
}

CT_VOID CCUserEquipment::SyncData(CT_INT64 currTime, CT_BOOL bForce /* = CT_FALSE*/ )
{
    _AUTOMUTEX(m_pMutex);

    G_ValueRange vr = {0};
    G_GetValueRange(VALUE_TYPE_ITEM_SYNC_THRESHOLDS, &vr);
    CT_INT32 interval = (CT_INT32)vr.fMaxVal;
    if (interval == 0) interval = 600;
    if (bForce || (currTime - m_lastSyncTime) > interval) {
        MSC_DB_SyncEquipData stDbData = {0};
        stDbData.dwUserID = m_pUser->GetUserID();
        DataMap_IT it = m_DataMap.begin();
        for (CT_INT32 i = 0; it != m_DataMap.end(); ++it, ++i) 
		{
			if(i < ITEM_SLOT_COUNT)
				stDbData.stEquips[i] = it->second;
        }
        m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_EQUIP_SYNC, (CT_CHAR*)&stDbData, sizeof(stDbData));
        m_lastSyncTime = currTime;
    }
}

//////////////////////////////////////////////////////////////////////////
// 强化装备
CT_INT32 CCUserEquipment::SmithingEquip(const ItemData &item, EquipmentData &equip)
{
    _AUTOMUTEX(m_pMutex);

    if (!m_pUser->m_userVip.VipHasPrivilegeType(VIP_PRIVILEGE_CLODDOWN)) {
        // 判断冷却
        CooldownData cdData = {0};
        m_pUser->m_userBase.GetCooldownData(SMITHINGEQUIP_COOL, cdData);
        CT_INT64 currTime = time(0);
        CT_INT64 leftTime = cdData.leftTime - (currTime - cdData.toggleTime);
        G_ValueRange vr = {0};
        if (0 == G_GetValueRange(VALUE_TYPE_SMITHING_COOLDOWN, &vr)) {
            vr.fMaxVal = 1200;
        }
        if (leftTime > vr.fMaxVal)
        {
            return ESE_COOLDOWN;
        }
    }

    DataMap_IT it = m_DataMap.find(item.key);
    if (it == m_DataMap.end())
    {
        memset(&equip, 0, sizeof(equip));
        equip.key = item.key;
    }
    else
    {
        equip = (*it).second;
    }


    CT_INT32 iGold = 0;
    CT_INT32 iDiamond = 0;
    CT_INT32 iItemID = 0;
    CT_INT32 iItemNum = 0;
    // 计算需要的钻石, 金币和道具
    CalcSmithingCost(equip, iGold, iDiamond, iItemID, iItemNum);
    
    ENUM_SmithingEquipFail enFail = ESE_SUCCESS;
    if (ESE_SUCCESS == (enFail = (ENUM_SmithingEquipFail)CheckExpend(iGold, iDiamond, iItemID, iItemNum, equip.iLevel)))
    {
        // 领主道具更新
        m_pUser->m_userItem.AutoExpendItem(SAVETAG_STRENGTHEN_EQUIP_FINISH, iItemID, -iItemNum);

        // 装备信息更新
        equip.iLevel += 1;
        InsertData(equip.key, equip);

		//add by caihan 
		//强化道具，判断物品任务（拥有指定强化等级以上的指定装备	例:拥有一个强化等级300的屠龙刀）
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_OWN;
		taskEventInfo.iSubType = item.iID;
		taskEventInfo.iLevel = equip.iLevel;
		m_pUser->m_userTask.DisposeTask(taskEventInfo);
		//end

		//数据记录
		MSC_LOGDB_SmithingAndSlot LogDBEquip={0};
		LogDBEquip.dwUserID=m_pUser->GetUserID();
		LogDBEquip.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
		LogDBEquip.key = equip.key;
		LogDBEquip.iEquipID = item.iID;
		LogDBEquip.iLevel = 1;
		LogDBEquip.iDiamond = iDiamond;
		LogDBEquip.iGold = iGold;
		LogDBEquip.iItemID = iItemID;
		LogDBEquip.iItemNum = iItemNum;
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPADDSTRENGTHEN,(CT_CHAR*)&LogDBEquip,sizeof(LogDBEquip));


		//add by caihan
		//判断强化任务
		memset(&taskEventInfo,0,sizeof(taskEventInfo));
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_SMITHY;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_SMITHY_STRENTH;
		m_pUser->m_userTask.DisposeTask(taskEventInfo);
		//end

		if (item.iIndex < ITEM_SLOT_BAG)
		{
			// 判断穿戴指定的装备n次,参数0：装备等级，参数1：装备品质，参数2：佩戴位置，参数3：装备ID
			CMD_G_TaskSystemInfo taskSysInfo = {0};
			G_ItemBase itemBase = {0};
			G_GetItemBase(item.iID, &itemBase);
			taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
			taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_WEAR_EQUIPMENT;
			taskSysInfo.arrayParam[0] = equip.iLevel;
			taskSysInfo.arrayParam[1] = itemBase.iQuality;
			taskSysInfo.arrayParam[2] = item.iIndex;
			taskSysInfo.arrayParam[3] = item.iID;
			taskSysInfo.iProgress = 1;
			m_pUser->m_userTask.DisposeTask(taskSysInfo);
		}
		
    }
    return enFail;
}

// 洗练装备
CT_INT32 CCUserEquipment::ForgeEquip(const ItemData &item, const CMD_G_ForgeEquip &forge, EquipmentData &equip)
{
    _AUTOMUTEX(m_pMutex);

    DataMap_IT it = m_DataMap.find(item.key);
    if (it == m_DataMap.end())
    {
        memset(&equip, 0, sizeof(equip));
        equip.key = item.key;
    }
    else
    {
        equip = (*it).second;
    }

	G_ItemBase itBase = {0};
	CCItemScriptSetting::getSingleton().GetItemBase(item.iID, itBase);

	// 品质低于紫色，不符合洗练条件
	if (itBase.iQuality < ITEM_QUALITY_LEVEL_3) {
		return EFE_CANNOT_FORGE;
	}

    CT_INT32 iGold = 0;
    CT_INT32 iDiamond = 0;
    CT_INT32 iItemID = 0;
    CT_INT32 iItemNum = 0;
    // 计算需要的钻石, 金币和道具
    CalcForgeCost(itBase, forge, iGold, iDiamond, iItemID, iItemNum);
   
    ENUM_ForgeEquipFail enFail = EFE_SUCCESS;
    if (EFE_SUCCESS == (enFail = (ENUM_ForgeEquipFail)CheckExpend(iGold, iDiamond, iItemID, iItemNum)))
    {     
        // 领主道具更新
        m_pUser->m_userItem.AutoExpendItem(SAVETAG_EQUIP_WASH, iItemID, -iItemNum);

        CCRand forgeRand;    
        CMD_G_Forge forgeData = {0};
        CT_INT32 attrs[] = {ATTR_STRENGTH, ATTR_AGILITY, ATTR_INTELLECT, ATTR_SPIRIT, ATTR_HPPERCENT, 
            ATTR_HITRATE, ATTR_DODGERATE, ATTR_CRITRATE, ATTR_RESILIENT};
        G_EquipForge forgeMethod[sizeof(attrs) / sizeof(*attrs)] = {0};
        CT_INT32 lastCnt = sizeof(attrs) / sizeof(*attrs);
        CT_INT32 n = lastCnt;

        // 排除锁定的属性
        for (int i = 0; i < FORGE_ATTRS; ++i)
        {
            if (forge.bLock[i] != 0)
            {
                for (int j = 0; j < n; ++j)
                {
                    if (attrs[j] == equip.Forge[i].iAID)
                    {
                        memmove(attrs + j, attrs + j + 1, sizeof(*attrs) * (n - j - 1));
                        lastCnt--;
                    }
                }
            }
        }

        // 产生洗炼结果 - 紫装1个属性，橙装2个属性
        int nAttrs = 1;
        if (itBase.iQuality > ITEM_QUALITY_LEVEL_3) {
            nAttrs = 2;
        }
        for (int i = 0; i < nAttrs; ++i)
        {
            if (forge.bLock[i] == 0) 
            {
				CT_INT32 totalWeighting = 0;
				for (int j = 0; j < lastCnt; ++j)
				{
					G_GetEquipForge(itBase.iLevel, attrs[j], forgeMethod[j]);
					totalWeighting += forgeMethod[j].iWeighting;
				}

                // 没有配置权重，无法洗炼
				if (totalWeighting == 0) {
					return EFE_CANNOT_FORGE;
				}
                
				forgeRand.srand(((CT_INT32)time(0) << lastCnt) * m_pUser->GetUserID());				
                // 随机属性类型                
                CT_INT32 currWeighting = forgeRand.rand() % totalWeighting;
                CT_INT32 iIdx = 0;
                CT_INT32 nextWeighting = forgeMethod[iIdx].iWeighting;                
                while (currWeighting > nextWeighting) {
                    nextWeighting += forgeMethod[++iIdx].iWeighting;
                }
                forgeData.iAID = attrs[iIdx];
                
                CT_FLOAT fMin = 0, fMax = 0;
                if (m_pUser->m_userVip.VipGradeGet() == 0) {
                    fMin = forgeMethod[iIdx].fLower;
                    fMax = forgeMethod[iIdx].fUpper;
                } else {
                    fMin = forgeMethod[iIdx].fVIPLower;
                    fMax = forgeMethod[iIdx].fVIPUpper;
                }

                // 随机属性数值
                switch (forgeData.iAID)
                {
                case ATTR_STRENGTH:
                case ATTR_AGILITY:
                case ATTR_INTELLECT:
                case ATTR_SPIRIT:
                    {
                        CT_INT32 range = (CT_INT32)(fMax - fMin);
                        if (range > 0) {
                            forgeData.fValue = fMin + forgeRand.rand() % range;
                        }
                    } break;
                case ATTR_HITRATE :
                case ATTR_DODGERATE :
                case ATTR_CRITRATE :
                case ATTR_RESILIENT :
                case ATTR_HPPERCENT:
                    {
                        CT_INT32 range = (CT_INT32)((fMax - fMin) * 100);
                        if (range > 0) {
                            forgeData.fValue = (fMin * 100 + forgeRand.rand() % range) / 100;
                        }
                    } break;
                }
                equip.Forge[i] = forgeData;

				if (iIdx < lastCnt) { 
					memmove(attrs + iIdx, attrs + iIdx + 1, sizeof(*attrs) * (lastCnt - iIdx - 1));
				} else {

				}
                lastCnt--;
            }

        }
		//add by caihan
		//判断洗炼任务
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_SMITHY;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_SMITHY_REFINE;
		taskEventInfo.iSubType = forge.iForgeType;
		m_pUser->m_userTask.DisposeTask(taskEventInfo);
		//end

		//add by huangmingwei
		m_EquipForgeTempDataList.erase(equip.key);
		m_EquipForgeTempDataList.insert(DataMap::value_type(equip.key, equip));
		//end

		//数据记录
		MSC_LOGDB_PropAddStrengthen PropAddStrengthen={0};
		PropAddStrengthen.dwUserID=m_pUser->GetUserID();
		PropAddStrengthen.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
		PropAddStrengthen.iDiamond=iDiamond;
		PropAddStrengthen.iEquipID=item.iID;;
		PropAddStrengthen.iGold=iGold;
		PropAddStrengthen.iItemID=iItemID;
		PropAddStrengthen.iItemNum=iItemNum;
		PropAddStrengthen.iIndex=item.iIndex;
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPUPDATATRAIN,(CT_CHAR*)&PropAddStrengthen,sizeof(PropAddStrengthen));


    }    
    return enFail;
}

//add start by Victor 2013-1-8
//洗练装备属性替换确认
CT_INT32 CCUserEquipment::ForgeEquipConfirm(const ItemData &item, EquipmentData &equip, ENUM_ForgeEquipConfirm confirmFlag)
{
    _AUTOMUTEX(m_pMutex);

	DataMap_IT it = m_DataMap.find(item.key);
	if (it == m_DataMap.end())
	{
		memset(&equip, 0, sizeof(equip));
		equip.key = item.key;
		//return EFE_DATA_ERROR;
	}
	else
	{
		equip = (*it).second;
	}

	switch(confirmFlag)
	{
	case EFEC_CANCLE:
		m_EquipForgeTempDataList.erase(item.key);
		break;
	case EFEC_DELETE:
		m_EquipForgeTempDataList.clear();
		break;
	case  EFEC_REPLACE:
		{
			DataMap_IT it = m_EquipForgeTempDataList.find(item.key);
			if (it == m_EquipForgeTempDataList.end())
				return EFE_DATA_ERROR;
			memcpy(equip.Forge,(*it).second.Forge,sizeof(equip.Forge));

			m_EquipForgeTempDataList.erase(it->first);
			InsertData(equip.key, equip);
		}
		break;
	default:
		break;
	}
	return EFE_SUCCESS;
}
//add end by Victor 2013-1-8

// 打孔
CT_INT32 CCUserEquipment::PunchEquip(const ItemData &item, EquipmentData &equip)
{
    _AUTOMUTEX(m_pMutex);

    DataMap_IT it = m_DataMap.find(item.key);
    if (it == m_DataMap.end())
    {
        memset(&equip, 0, sizeof(equip));
        equip.key = item.key;
    }
    else
    {
        equip = (*it).second;
    }

    CT_INT32 iGold = 0;
    CT_INT32 iDiamond = 0;
    CT_INT32 iItemID = 0;
    CT_INT32 iItemNum = 0;
    // 计算需要的钻石, 金币和道具
    CalcPunchCost(equip, iGold, iDiamond, iItemID, iItemNum);

    ENUM_PunchEquipFail enFail = EPE_SUCCESS;
    if (equip.nSlot < GEM_COUNTS)
    {
        if (EPE_SUCCESS == (enFail = (ENUM_PunchEquipFail)CheckExpend(iGold, iDiamond, iItemID, iItemNum)))
        {
            // 领主道具更新
            m_pUser->m_userItem.AutoExpendItem(SAVETAG_STRENGTHEN_EQUIP_FINISH, iItemID, -iItemNum);

            // 装备信息更新
            equip.nSlot += 1;
            InsertData(equip.key, equip);
			
			//add by caihan
			//判断打孔任务
			CMD_G_TaskEventInfo taskEventInfo = {0};
			taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_SMITHY;
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_SMITHY_PUNCH;
			m_pUser->m_userTask.DisposeTask(taskEventInfo);
			//end

			//数据记录
			MSC_LOGDB_SmithingAndSlot LogDBEquip={0};
			LogDBEquip.dwUserID=m_pUser->GetUserID();
			LogDBEquip.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
			LogDBEquip.key = equip.key;
			LogDBEquip.iEquipID = item.iID;
			LogDBEquip.iLevel = 1;
			LogDBEquip.iDiamond = iDiamond;
			LogDBEquip.iGold = iGold;
			LogDBEquip.iItemID = iItemID;
			LogDBEquip.iItemNum = iItemNum;
			CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPADDSTRENGTHEN,(CT_CHAR*)&LogDBEquip,sizeof(LogDBEquip));

        }
    }
    else
    {
        enFail = EPE_MAX_SLOT;
    }
    
    return enFail;
}

// 镶嵌
CT_INT32 CCUserEquipment::EmbedEquip(const ItemData &item, const CMD_G_EmbedEquip &embed, EquipmentData &equip)
{
    _AUTOMUTEX(m_pMutex);

    DataMap_IT it = m_DataMap.find(item.key);
    if (it == m_DataMap.end())
    {
        memset(&equip, 0, sizeof(equip));
        equip.key = item.key;
    }
    else
    {
        equip = (*it).second;
    }

    if (embed.iSlot >= equip.nSlot) return EEE_DATA_ERROR;
    if (embed.iGemID != 0 && m_pUser->m_userItem.CalcItemCount(embed.iGemID) < 1) return EEE_DATA_ERROR;

    // 同类宝石只能插一根！！
    if (embed.iGemID != 0) {
        G_ItemGem itGem = {0};
        if (0 == G_GetItemGem(embed.iGemID, &itGem)) return EEE_DATA_ERROR;
        for (int i = 0; i < sizeof(equip.Gem) / sizeof(*equip.Gem); ++i) {
            G_ItemGem itTo = {0};
            if (equip.Gem[i] != 0 && 0 == G_GetItemGem(equip.Gem[i], &itTo)) return EEE_DATA_ERROR;
            if (i != embed.iSlot && itTo.iType == itGem.iType) {
                return EFE_SAME_GEM;
            }
        }
    }

    if (equip.Gem[embed.iSlot] != 0)
    {
        // 目标孔有宝石，背包获得一个宝石
        CT_INT32 count = 1;        
        if (0 != m_pUser->m_userItem.AutoAddItemData(SAVETAG_EQUIP_EMBED, equip.Gem[embed.iSlot], count))
        {
            return EEE_NO_SLOT;
        }
    }
    equip.Gem[embed.iSlot] = embed.iGemID;

    // 消耗掉一个宝石
    m_pUser->m_userItem.AutoExpendItem(SAVETAG_EQUIP_EMBED, embed.iGemID, 1);
	
	//add by caihan
	//判断镶嵌任务
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_SMITHY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_SMITHY_MOSAIC;
	m_pUser->m_userTask.DisposeTask(taskEventInfo);
	//end

    InsertData(equip.key, equip);

    return EEE_SUCCESS;
}

// 继承
CT_INT32 CCUserEquipment::InheritEquip(const ItemData &src, const ItemData &dst, EquipmentData &eqSrc, EquipmentData &eqDst)
{
    _AUTOMUTEX(m_pMutex);

    DataMap_IT itSrc = m_DataMap.find(src.key);
    if (itSrc == m_DataMap.end() || (*itSrc).second.iLevel == 0)
    {
        return EIE_NO_SRC;
    }
    else
    {   
        eqSrc = (*itSrc).second;
    }
    DataMap_IT itDst = m_DataMap.find(dst.key);
    if (itDst == m_DataMap.end())
    {
        memset(&eqDst, 0, sizeof(eqDst));
        eqDst.key = dst.key;
    }
    else
    {
        eqDst = (*itDst).second;
    }

    CT_INT32 iGold = 0;
    CT_INT32 iDiamond = 0;
    CT_INT32 iItemID = 0;
    CT_INT32 iItemNum = 0;
    // 计算继承结果
    CalcInheritCost(src, dst, eqSrc, eqDst);
	
	//add by caihan
	//判断继承任务
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_SMITHY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_SMITHY_INHERIT;
	m_pUser->m_userTask.DisposeTask(taskEventInfo);
	//end

    itSrc->second.iLevel = eqSrc.iLevel;
    if (itDst == m_DataMap.end())
    {
        InsertData(eqDst.key, eqDst);
    }
    else
    {
        itDst->second.iLevel = eqDst.iLevel;
    }

	//数据记录
	MSC_LOGDB_InheritData InheritData={0};
	InheritData.dwUserID = m_pUser->GetUserID();
	InheritData.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
	InheritData.keySrc = dst.key;
	InheritData.iSrcID = dst.iID;
	InheritData.iLvSrc = eqDst.iLevel;
	InheritData.keyDst = src.key;
	InheritData.iDstID = src.iID;
	InheritData.iDiamond = iDiamond;
	InheritData.iGold = iGold;
	InheritData.iItemID  = iItemID;
	InheritData.iItemNum = iItemNum;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPINHERIT,(CT_CHAR*)&InheritData,sizeof(InheritData));


    return EIE_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
// 检查消耗
CT_INT32 CCUserEquipment::CheckExpend(const CT_INT32 iGold, const CT_INT32 iDiamond, const CT_INT32 iItemID, const CT_INT32 iItemNum, const CT_INT32 iLevel)
{
    MasterVariableData mvd = {0};
    m_pUser->m_userBase.GetMasterData(mvd);
    if (iLevel == mvd.stGrade.iLevel) return ESE_MAX_LEVEL;
    if (iGold + mvd.stRes.iGold < 0) return ESE_NO_GOLD;
    if (iDiamond + mvd.stRes.iDiamod < 0) return ESE_NO_DIAMOND;    
    if (m_pUser->m_userItem.GetItemCount(iItemID) + iItemNum < 0) return ESE_NO_ITEM;

    // 领主资源更新
    memset(&mvd, 0, sizeof(mvd));
    mvd.stRes.iGold = iGold;
    mvd.stRes.iDiamod = iDiamond;
    m_pUser->m_userBase.AddMastData(mvd, SAVETAG_STRENGTHEN_EQUIP_FINISH);
    return 0;
}

// 计算强化消耗
CT_VOID CCUserEquipment::CalcSmithingCost(const EquipmentData &equipData, CT_INT32 &iGold, CT_INT32 &iDiamond, CT_INT32 &iItemID, CT_INT32 &iItemCnt)
{
	ItemData data={0};
    if (m_pUser->m_userItem.SearchItemData(equipData, data)) {
        G_Equipment itEquip = {0};
        if (CCItemScriptSetting::getSingleton().GetEquipment(data.iID, itEquip))
        {
            iGold = G_GetItemStrengthenGold(equipData.iLevel, itEquip.fGoldCoefficient1, itEquip.fGoldCoefficient2);
            iItemID = itEquip.iGoods;
            iItemCnt = itEquip.iCount;
        }
    } else {
        CCLogMeg::getSingleton().WriteError("SearchItemData by EquipData Error! user:%i, key: %i", m_pUser->GetUserID(), equipData.key);
    }
}
// 计算打孔消耗
CT_VOID CCUserEquipment::CalcPunchCost(const EquipmentData &equipData, CT_INT32 &iGold, CT_INT32 &iDiamond, CT_INT32 &iItemID, CT_INT32 &iItemCnt)
{
    G_EquipPunch stPunch = {0};
    G_GetEquipPunch(equipData.nSlot + 1, stPunch);
    iGold = stPunch.iGold;
    iDiamond = stPunch.iDiamond;
    iItemID = stPunch.iItemID;
    iItemCnt = stPunch.iItemNum;
}
// 计算洗炼消耗
CT_VOID CCUserEquipment::CalcForgeCost(const G_ItemBase &itBase, const CMD_G_ForgeEquip &forgeData, CT_INT32 &iGold, CT_INT32 &iDiamond, CT_INT32 &iItemID, CT_INT32 &iItemCnt)
{
    //STUB
    int nLock = 0;
    for (int i = 0; i < FORGE_ATTRS; ++i)
    {
        nLock += forgeData.bLock[i];
    }

    iGold = 0;
    iDiamond = -10 * nLock;
    if (forgeData.iForgeType == EQUIP_FORGE_TYPE_NORMAL)
    {
        G_ValueRange vrGold = {0}, vrGoldFactor = {0};
        G_GetValueRange(VALUE_TYPE_FORGE_GOLD_BASE, &vrGold);
        G_GetValueRange(VALUE_TYPE_FORGE_GOLD_FACTOR, &vrGoldFactor);
        iGold += G_CalcEquipForgeCostGold((CT_INT32)vrGold.fMaxVal, vrGoldFactor.fMaxVal, itBase.iLevel);
    }
    else
    {
        G_ValueRange vr = {0};
        G_GetValueRange(VALUE_TYPE_FORGE_DIAMOND, &vr);
        iDiamond += (CT_INT32)vr.fMaxVal;
    } 

    G_ValueRange vr = {0};
    G_GetValueRange(VALUE_TYPE_FORGE_COST_ITEM, &vr);
    iItemID = (CT_INT32)vr.fMinVal;
    iItemCnt = itBase.iQuality > ITEM_QUALITY_LEVEL_3 ? -2 : -1;
}
// 计算继承
CT_VOID CCUserEquipment::CalcInheritCost(const ItemData &itSrc, const ItemData &itDst, EquipmentData &equipSrc, EquipmentData &equipDst)
{
    G_Equipment itEquip = {0};
    G_Equipment itEquipDst = {0};
    if (CCItemScriptSetting::getSingleton().GetEquipment(itSrc.iID, itEquip)
        && CCItemScriptSetting::getSingleton().GetEquipment(itDst.iID, itEquipDst))
    {
        MasterVariableData mvd = {0};
        m_pUser->m_userBase.GetMasterData(mvd);
        int SellGold = G_GetItemSell(0, equipSrc.iLevel, itEquip.fGoldCoefficient1, itEquip.fGoldCoefficient2);
        int addGold = 0;
        // 强化等级不能超过领主等级
        while (equipDst.iLevel < mvd.stGrade.iLevel && SellGold > 0)
        {
            SellGold += G_GetItemStrengthenGold(equipDst.iLevel++, itEquipDst.fGoldCoefficient1, itEquipDst.fGoldCoefficient2);
        }
        equipSrc.iLevel = 0;
        if (SellGold < 0) 
        {
            equipDst.iLevel--;
        }
        else
        {            
            while (SellGold > 0)
            {
                SellGold += G_GetItemStrengthenGold(equipSrc.iLevel++, itEquip.fGoldCoefficient1, itEquip.fGoldCoefficient2);
            }
            if (SellGold < 0) equipSrc.iLevel--;
        }
    }    
}

CT_VOID CCUserEquipment::UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserItem::DataMap &itemMap, const DataMap &equipMap)
{
    for (CCUserItem::DataMap_CONSTIT itItem = itemMap.begin(); itItem != itemMap.end(); ++itItem)
    {
		if (itItem->second.iIndex < ITEM_SLOT_BAG) 	// 忽略背包内的装备
		{
			G_Equipment equip = {0};
			EquipStatus Status[STATUS_TYPES + FORGE_ATTRS + GEM_COUNTS * GEM_ATTRS + GEM_ATTRS] = {0}; //装备能力加成符文会增加两个属性加成
			CCItemScriptSetting::getSingleton().GetEquipment(itItem->second.iID, equip);
			int k = 0;
			// 基本属性                
			for (int n = 0; k < STATUS_TYPES; ++n, ++k)
			{
				Status[k] = equip.Status[n];
			}                

			CT_INT32 iLevel = 0;
			DataMap_CONSTIT itEquip = equipMap.find(itItem->second.key);
			if (itEquip != equipMap.end()) 
			{
				EquipmentData eqData = itEquip->second;
				// 强化
				iLevel = eqData.iLevel;
				// 洗练
				for (int n = 0; k < STATUS_TYPES + FORGE_ATTRS; ++n, ++k)
				{
					Status[k].iSID = eqData.Forge[n].iAID;
					Status[k].fValue = eqData.Forge[n].fValue;
				}
				// 镶嵌
				for (int n = 0; k < STATUS_TYPES + FORGE_ATTRS + GEM_COUNTS * GEM_ATTRS; ++n)
				{
					G_ItemGem gem = {0};
					CCItemScriptSetting::getSingleton().GetItemGem(eqData.Gem[n], gem);
					for (int t = 0; t < GEM_ATTRS; ++t)
					{
						if (gem.Attr[t].iAID == VALUE_TYPE_EQUIPPERCENT)
						{
							// 符文为装备能力加成时，在装备当前值的基础上增加对应的百分比
							G_ValueRange vr = {0};
							if (!G_GetValueRange(VALUE_TYPE_EQUIPPERCENT, &vr) || gem.Attr[t].fValue > vr.fMaxVal || gem.Attr[t].fValue < vr.fMinVal)
							{
								CCLogMeg::getSingleton().WriteError("CCUserEquipment::UpdateBattleStatus VALUE_TYPE_EQUIPPERCENT data error.fvalue:%f", gem.Attr[t].fValue);
								gem.Attr[t].fValue = 0;
							}
							for(int iq = 0;iq < STATUS_TYPES;++iq)
							{
								Status[k].iSID = equip.Status[iq].iSID;
								Status[k].fValue =(equip.Status[iq].fValue + equip.Status[iq].fIncValue * iLevel) * gem.Attr[t].fValue;
								k++;
							}
						}
						else
						{
							Status[k].iSID = gem.Attr[t].iAID;
							Status[k].fValue = gem.Attr[t].fValue;
							k++;
						}
					}
				}
			}
			
			for (unsigned int j = 0; j < k; ++j)
			{
				CCUserBase::UpdateBattleStatus(mcs, Status[j].iSID, Status[j].fValue + Status[j].fIncValue * iLevel);
			}
		}
    }
}