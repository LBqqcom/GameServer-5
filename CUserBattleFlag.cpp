
#include <time.h>
#include "CUserBattleFlag.h"
#include "CMD_Game.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CNetInterface.h"
#include "CGlobeData.h"

CT_INT32 CCUserBattleFlag::transData( const BattleFlag &src, CT_CHAR *pDst ) const
{
    CMD_G_BattleFlag *pst = (CMD_G_BattleFlag *)pDst;
    pst->iType = src.iType;
    pst->iLevel = src.iLevel;
    return sizeof(*pst);
}

// ���ĳ��ս����װ����Ԫ��
CT_BOOL CCUserBattleFlag::GetEquippedElms( CT_INT32 iType, CT_CHAR *pBuf, CT_INT32 &bufLen ) const
{
    _AUTOMUTEX(m_pMutex);
    
    DataMap_CONSTIT itFlag = m_DataMap.find(iType);  // �ҵ�һ��ս��
    G_BattleFlag stFlag = {0};
    if (itFlag != m_DataMap.end() && (0 != G_GetBattleFlag(itFlag->second.iLevel, stFlag)))
    {
        CT_INT32 nMaxElm = stFlag.nSlot;        // ���ս�����ڵĿ���
        CT_INT32 iElmStart = MAKE_ELM_INDEX(iType, 0);     // ս���ϵĿף���λ�ñ��
        
        CT_INT32 len = nMaxElm * sizeof(CMD_G_ItemData);
        if (len > bufLen)
        {
            return CT_FALSE;
        }

        CCUserItem::DataMap_IT it = m_pUser->m_userElmPower.m_DataMap.begin();
        CT_INT32 index = 0;
        bufLen = 0;
        for (CT_INT32 i = 0; it != m_pUser->m_userElmPower.m_DataMap.end(); ++it, ++i)
        {
            if ((*it).second.iIndex > iElmStart && (*it).second.iIndex < (iElmStart + nMaxElm + 1))
            {
                CMD_G_ItemData *pItemData = (CMD_G_ItemData *)(pBuf + sizeof(CMD_G_ItemData) * (index++));
                pItemData->key = (*it).second.key;
                pItemData->iID = (*it).second.iID;
                pItemData->iCount = (*it).second.iCount;
                pItemData->iIndex = (*it).second.iIndex;
                bufLen += sizeof(*pItemData);

            }
        }
        return CT_TRUE;
    } 
    else
    {
        return CT_FALSE;
    }
}

// ָ����ս�����Ƿ�װ����ĳ�����͵�Ԫ�ؾ���
CT_BOOL CCUserBattleFlag::HasElmOfType( const CT_INT32 flagType, const CT_INT32 elmType, const CT_INT32 IgnoreSlot /* = 0 */) const
{
    CMD_G_ItemData equipedElm[BATTLE_FLAG_ELM_COUNT] = {0};
    CT_INT32 bufLen = sizeof(equipedElm);
    if (!GetEquippedElms(flagType, (CT_CHAR *)equipedElm, bufLen)) return CT_FALSE;

    CT_INT32 nSlot = bufLen / sizeof(*equipedElm);
    for (CT_INT32 i = 0; i < nSlot; ++i)
    {
        G_ElmMarrow elmEquip = {0};
        G_GetElmMarrow(equipedElm[i].iID, elmEquip);
        if (IgnoreSlot != equipedElm[i].iIndex && elmType == elmEquip.iType)
        {
            // ���ͳ�ͻ
            return CT_TRUE;
        }
    }
    return CT_FALSE;
}

// �ƶ�Ԫ��
CT_INT32 CCUserBattleFlag::DoEquipElm(CT_INT32 iType, CT_INT32 iSlotFrom, CT_INT32 iSlotTo)
{
    _AUTOMUTEX(m_pMutex);


    CT_INT32 iSlotEquip = 0;
    if (IS_ELMIDX_EQUIPPED(iSlotFrom) || IS_ELMIDX_EQUIPPED(iSlotTo)) {
        DataMap_IT itFlag = m_DataMap.find(iType);  // �ҵ�һ��ս��
        G_BattleFlag stFlag = {0};
        if (itFlag == m_DataMap.end() || (0 == G_GetBattleFlag(itFlag->second.iLevel, stFlag))) return EUI_BATTLE_FLAG_UNEXIST; // �����ڵ�ս��

        CT_INT32 nMaxElm = stFlag.nSlot;        // ���ս�����ڵĿ���
        CT_INT32 iElmStart = MAKE_ELM_INDEX(iType, 0);     // ս���ϵĿף���λ�ñ��
        if (IS_ELMIDX_EQUIPPED(iSlotFrom) && (iSlotFrom <= iElmStart || iSlotFrom >= iElmStart + nMaxElm + 1)) return EUI_BATTLE_FLAG_SLOT_ERR; // ս��Ĳ�ײ�����
        if (IS_ELMIDX_EQUIPPED(iSlotTo) && (iSlotTo <= iElmStart || iSlotTo >= iElmStart + nMaxElm + 1)) return EUI_BATTLE_FLAG_SLOT_ERR; // ս��Ĳ�ײ�����
    }   

    CCUserItem::DataMap_IT itFrom = m_pUser->m_userElmPower.m_DataMap.find(iSlotFrom);
    if (itFrom == m_pUser->m_userElmPower.m_DataMap.end()) return EUI_DATA_ERROR;   // �Ҳ���ԴԪ��
    G_ItemBase baseFrom = {0};
    if (0 == G_GetItemBase(itFrom->second.iID, &baseFrom)) return EUI_DATA_ERROR;   // ���ô���    
    if (baseFrom.iTID != ITEM_TYPE_ELMMARROW && IS_ELMIDX_EQUIPPED(iSlotTo)) return EUI_ELM_TYPE_ERROR;  // ���ʹ���   

    CCUserItem::DataMap_IT itTo = m_pUser->m_userElmPower.m_DataMap.find(iSlotTo);  // �ҵ�Ŀ���ϵ�λ��
    if (itTo != m_pUser->m_userElmPower.m_DataMap.end())    // Ŀ�������Ԫ��   
    {        
        if (IS_ELMIDX_EQUIPPED(iSlotFrom) ^ IS_ELMIDX_EQUIPPED(iSlotTo))
        {
            // ���ҽ���һ����ս����, ��Ҫ�ж������Ƿ��ͻ, ���ҵ����������Ԫ�ؾ���
            CCUserItem::DataMap_IT itBag = itFrom, itEquip = itTo;
            if (IS_ELMIDX_EQUIPPED(iSlotFrom)) 
            {
                itEquip = itFrom;
                itBag = itTo;
            }
            iSlotEquip = itEquip->first;
            
            G_ItemBase baseBag = {0};
            if (0 == G_GetItemBase(itBag->second.iID, &baseBag)) return EUI_DATA_ERROR;// ���ô���
            if (baseBag.iTID != ITEM_TYPE_ELMMARROW) return EUI_ELM_TYPE_CONFLICT;

            G_ElmMarrow elmBag = {0};
            if (0 == G_GetElmMarrow(itBag->second.iID, elmBag)) return EUI_DATA_ERROR;// ���ô���
            if (HasElmOfType(iType, elmBag.iType, iSlotEquip)) {
                // ���ͳ�ͻ
                return EUI_ELM_TYPE_CONFLICT;
            }
        }
        // ����λ��
        m_pUser->m_userElmPower.SwapItem(itFrom, itTo, CT_FALSE);   
    } 
    else
    {
        MasterVariableData mvd = {0};
        m_pUser->m_userBase.GetMasterData(mvd);
        // �յĸ����Ƿ���Ч
        if (iSlotTo < ITEM_SLOT_BATTLEFLAG) {
            if (iSlotTo > ITEM_SLOT_ELM + mvd.stRes.iElmSlots) {
                return EUI_ELM_TYPE_ERROR;
            }    
        } else {
            CT_INT32 flagType = iSlotTo >> 16;
            CT_INT32 iSlot = iSlotTo & 0xFFFF;
            BattleFlag flag = {0};
            if (GetData(flagType, flag)) {
                G_BattleFlag stFlag = {0};
                if (0 != G_GetBattleFlag(flag.iLevel, stFlag)) {
                    if (iSlot < 1 || iSlot > stFlag.nSlot) return EUI_ELM_TYPE_ERROR;
                }
            }
        }

        // �ӱ����Ƶ�ս���ϵĲż���ͻ
        if (IS_ELMIDX_EQUIPPED(iSlotTo) && !IS_ELMIDX_EQUIPPED(iSlotFrom)) {
            iSlotEquip = iSlotTo;            
            G_ElmMarrow elmBag = {0};
            if (0 == G_GetElmMarrow(itFrom->second.iID, elmBag)) return EUI_DATA_ERROR;// ���ô���
            if (HasElmOfType(iType, elmBag.iType, iSlotEquip)) {
                // ���ͳ�ͻ
                return EUI_ELM_TYPE_CONFLICT;
            }
        }
        m_pUser->m_userElmPower.MoveItem(itFrom, iSlotTo, CT_FALSE);
    }

    // �����Ľ��
    CMD_G_ItemData itDatas[2] = {0};
    m_pUser->m_userElmPower.GetClientData(iSlotFrom, itDatas[0]);
    m_pUser->m_userElmPower.GetClientData(iSlotTo, itDatas[1]);

    // ������Ϣ
    m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_ELM_CHANGE, (CT_CHAR *)&itDatas, sizeof(itDatas), false, true);

    // ����ƶ�������Ԫ�������ҽ���һ����������ս���ϵģ������������ս������
    if (iType < UNIT_ALL_ARMY  && (IS_ELMIDX_EQUIPPED(iSlotFrom) ^ IS_ELMIDX_EQUIPPED(iSlotTo))) {
        m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
    }

    // װ���ɹ����ж�װ��Ԫ������
    if (iSlotEquip != 0) {
		G_ItemBase elmBase = {0};
        G_ElmMarrow elmEquip = {0};
        ItemData elmData = {0};
        ElmMarrowAttr attrEquip = {0};		
        m_pUser->m_userElmPower.GetData(iSlotEquip, elmData);
		G_GetItemBase(elmData.iID, &elmBase);
        G_GetElmMarrow(elmData.iID, elmEquip);
        m_pUser->m_userElmAttr.GetData(elmData.key, attrEquip);
        CMD_G_TaskSystemInfo stTaskSys = {0};
        stTaskSys.iTaskSubType = TASK_SUB_TYPE_SYSTEM_EQUIPELM;
        stTaskSys.arrayParam[0] = elmEquip.iType;      //����0������
		stTaskSys.arrayParam[1] = elmBase.iQuality;    //����1��Ʒ��
		stTaskSys.arrayParam[2] = attrEquip.iLevel;    //����2���ȼ�
        stTaskSys.iProgress = iType;
        m_pUser->m_userTask.DisposeTask(stTaskSys);
    }
    
    return EUI_SUCCESS;
}

CT_INT32 CCUserBattleFlag::UpgradeFlag( const CT_INT32 iType )
{
    _AUTOMUTEX(m_pMutex);

    CT_INT32 addLv = 1;
    DataMap_IT it = m_DataMap.find(iType);
    BattleFlag flag = {0};
	if (!GetData(iType, flag)) 
	{
		// �����ģ���������1�����, ����Ҫ����       
		G_BattleFlag stFlag = {0};
		if(iType > GET_TARGET_TYPE(UNIT_ALL_BOSS))//�ٻ��������²�������
		{
			return EUI_DATA_ERROR;
		}

		if (iType > UNIT_ALL_ARMY) 
		{
			if (0 == G_GetBattleFlag(1, stFlag)) return EUI_DATA_ERROR;
			if (!m_pUser->m_userItem.AutoExpendItem(SAVETAG_FLAG_UPGRADE, stFlag.iCostItemID, -stFlag.iCostItemCnt)) return EUI_BATTLE_FLAG_NO_ITEM;
		}
		flag.iType = iType;
        flag.iLevel = 1;
    }
    else 
    {
        // ��ȡ������һ������Ϣ
        G_BattleFlag stFlag = {0};
        if (0 == G_GetBattleFlag(flag.iLevel + 1, stFlag)) return EUI_BATTLE_FLAG_MAX;
        if (!m_pUser->m_userItem.AutoExpendItem(SAVETAG_FLAG_UPGRADE, stFlag.iCostItemID, -stFlag.iCostItemCnt)) return EUI_BATTLE_FLAG_NO_ITEM;
        flag.iLevel += 1;
    }
    InsertData(flag.iType, flag);

	//�����ɹ����ж�ָ��������������
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_FLAG_UPGRADE;
	taskSysInfo.arrayParam[0] = iType;             //����0����ɫid
	taskSysInfo.iProgress = flag.iLevel;
	m_pUser->m_userTask.DisposeTask(taskSysInfo);

    // ͬ�������ݿ�
    MSC_DB_SaveBattleFlag stDb = {0};
    stDb.dwUserID = m_pUser->GetUserID();
    stDb.iType = iType;
    stDb.iAdd = 1;
    m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_BATTLEFLAG_INFO, (CT_CHAR *)&stDb, sizeof(stDb));

    // ֪ͨ�û�
    it = m_DataMap.find(iType);
    CMD_G_BattleFlagUpgradeSuc stSuc = {0};
    stSuc.dwUserID = m_pUser->GetUserID();
    stSuc.iType = flag.iType;
    stSuc.iFlagLevel = flag.iLevel;
    m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_BATTLE_FLAG_UPGRADE_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);
    m_pUser->UpdateAndSendMasterData();

    // TODO ս��������־
	MSC_LOGDB_BattleFlag logdbBattleFlag = {0};
	MasterVariableData mvd = {0};
	m_pUser->m_userBase.GetMasterData(mvd);
	logdbBattleFlag.dwUserID = m_pUser->GetUserID();
	logdbBattleFlag.iFlag = flag.iType;
	logdbBattleFlag.iGrade = flag.iLevel;
	logdbBattleFlag.iUGrade = mvd.stGrade.iLevel;
	CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbBattleFlag.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_BATTLEFLAG,(CT_CHAR*)&logdbBattleFlag,sizeof(MSC_LOGDB_BattleFlag));

    return EUI_SUCCESS;
}

// ����ս���ṩ��ս������
CT_INT32 CCUserBattleFlag::UpdateBattleStatus(MasterCombatStatus &mcs, const BattleFlag &flag, const CCUserItem::DataMap &elmPowerMap, const CCUserElmAttr::DataMap &empAttrMap)
{
    CT_INT32 iElmStart = MAKE_ELM_INDEX(flag.iType, 0);     // ս���ϵĿף���λ�ñ��

    G_BattleFlag cfgFlag = {0};
    G_GetBattleFlag(flag.iLevel, cfgFlag);
    MasterCombatStatus mcsFlag = {0};

    for (CCUserItem::DataMap_CONSTIT itElm = elmPowerMap.begin(); itElm != elmPowerMap.end(); ++itElm)
    {       
        if (itElm->second.iIndex <= iElmStart || itElm->second.iIndex > (iElmStart + cfgFlag.nSlot)) continue; // ����ָ����ս����
        
        G_ElmMarrow elmMarrow = {0};
        if (0 == G_GetElmMarrow(itElm->second.iID, elmMarrow)) continue;

        for (CCUserElmAttr::DataMap_CONSTIT itAttr = empAttrMap.begin(); itAttr != empAttrMap.end(); ++itAttr)
        {
            if (itAttr->second.key != 0 && itAttr->second.key == itElm->second.key)
            {
                for (CT_INT32 k = 0; k < sizeof(elmMarrow.elmAttr) / sizeof(*elmMarrow.elmAttr); ++k)
                { 
                    CT_FLOAT fRange = elmMarrow.elmAttr[k].fMax - elmMarrow.elmAttr[k].fMin;
                    CT_FLOAT fVal = elmMarrow.elmAttr[k].fMin + fRange * itAttr->second.attrs[k];
                    fVal = G_CalcElmAttr(itAttr->second.iLevel, fVal) * (1 + cfgFlag.fSlotFator);
                    CCUserBase::UpdateBattleStatus(mcsFlag, elmMarrow.elmAttr[k].iAID, fVal);
                }
				if (elmMarrow.iBaseExp < 0)
				{
					mcsFlag.exFightingPower += G_CalcExFighting(elmMarrow.iBaseExp, itAttr->second.iLevel);
				}
            }
        }
    }
    mcs.iStrength      += mcsFlag.iStrength;
    mcs.iAgility       += mcsFlag.iAgility;
    mcs.iIntellect     += mcsFlag.iIntellect;
    mcs.iSpirit        += mcsFlag.iSpirit;
    mcs.fHitRate       += mcsFlag.fHitRate;
    mcs.fDodgeRate     += mcsFlag.fDodgeRate;
    mcs.fCritRate      += mcsFlag.fCritRate;
    mcs.fResilient     += mcsFlag.fResilient;
    mcs.fDmgIncPercent += mcsFlag.fDmgIncPercent;
    mcs.fDmgDecPercent += mcsFlag.fDmgDecPercent;
    mcs.iMaxHP         += mcsFlag.iMaxHP;
    mcs.iMaxMP         += mcsFlag.iMaxMP;
    mcs.iHPRecover	   += mcsFlag.iHPRecover;
    mcs.iMPRecover	   += mcsFlag.iMPRecover;
    mcs.iAttack        += mcsFlag.iAttack;
    mcs.iDefence       += mcsFlag.iDefence;
    mcs.fHPPersent     += mcsFlag.fHPPersent;
    mcs.iHPRecover     += mcsFlag.iHPRecover;
    mcs.iMPRecover     += mcsFlag.iMPRecover;
	mcs.exFightingPower += mcsFlag.exFightingPower;

	return G_GetFightingFlagPower(mcsFlag.iMaxHP,mcsFlag.iMaxMP + mcsFlag.iMPRecover,
		mcsFlag.iAttack,mcsFlag.iDefence,mcsFlag.iStrength, mcsFlag.iAgility,mcsFlag.iIntellect,mcsFlag.iSpirit,
		mcsFlag.fHitRate,mcsFlag.fDodgeRate,mcsFlag.fCritRate,mcsFlag.fResilient,
		mcsFlag.fDmgIncPercent,mcsFlag.fDmgDecPercent,mcsFlag.fHPPersent) + mcs.exFightingPower;
}