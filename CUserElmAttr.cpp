
#include <time.h>
#include "CUserElmAttr.h"
#include "CMD_Game.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CNetInterface.h"
#include "CGlobeData.h"
#include "CRand.h"

CT_INT32 CCUserElmAttr::transData( const ElmMarrowAttr &src, CT_CHAR *pDst ) const
{
    CMD_G_ElmAttr *pst = (CMD_G_ElmAttr *)pDst;
    pst->key = src.key;
    pst->iLevel = src.iLevel;
    pst->iExp = src.iExp;
    for (CT_INT32 i = 0; i < sizeof(pst->attrs) / sizeof(*pst->attrs); ++i)
    {
        pst->attrs[i] = src.attrs[i];
    }    
    return sizeof(*pst);
}

CT_VOID CCUserElmAttr::SyncData(CT_INT64 currTime, CT_BOOL bForce /* = CT_FALSE*/)
{
    _AUTOMUTEX(m_pMutex);

    G_ValueRange vr = {0};
    G_GetValueRange(VALUE_TYPE_ITEM_SYNC_THRESHOLDS, &vr);
    CT_INT32 interval = (CT_INT32)vr.fMaxVal;
    if (interval == 0) interval = 600;
    if (bForce || (currTime - m_lastSyncTime) > interval) {
        MSC_DB_SyncElmAttrData stDbData = {0};
        stDbData.dwUserID = m_pUser->GetUserID();
        DataMap_IT it = m_DataMap.begin();
        for (CT_INT32 i = 0; it != m_DataMap.end(); ++it, ++i) 
		{
			if(i < ITEM_SLOT_COUNT)
				stDbData.stDatas[i] = it->second;
        }
        m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ELMATTR_SYNC, (CT_CHAR*)&stDbData, sizeof(stDbData));
        m_lastSyncTime = currTime;
    }
}

CCUserElmAttr::DataMap_IT CCUserElmAttr::getElmAttrIT( CT_INT32 iIndex )
{
    DataMap_IT it = m_DataMap.end();
    ItemData itData = {0};
    if (m_pUser->m_userElmPower.GetData(iIndex, itData))
    {
        it = m_DataMap.find(itData.key);
    }
    return it;
}

CT_BOOL CCUserElmAttr::getMarrowExp(ElmMarrowAttr &srcAttr, G_ElmMarrow &srcMarrow, CT_INT32 iExp)
{
    CT_INT32 nextExp = srcAttr.iExp + iExp;
    G_ElmUpgrade stUpgrade = {0};
    if (0 == G_GetElmUpgrade(srcMarrow.iUpgraderule, srcAttr.iLevel + 1, stUpgrade)) return CT_FALSE;   // �Ѿ�������
    while (nextExp >= stUpgrade.iUpgradeExp)
    {
        srcAttr.iLevel += 1;
        if (0 == G_GetElmUpgrade(srcMarrow.iUpgraderule, srcAttr.iLevel + 1, stUpgrade)) break; // ����������

		//�����ɹ����жϻ��ָ��Ԫ������
		G_ItemBase itemBase = {0};
		G_ElmMarrow elmGet = {0};
		G_GetItemBase(srcMarrow.iID, &itemBase);
		G_GetElmMarrow(srcMarrow.iID, elmGet);
		CMD_G_TaskSystemInfo taskSysInfo = {0};
		taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
		taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_GETELM;
		taskSysInfo.arrayParam[0] = elmGet.iType;           //����0������
		taskSysInfo.arrayParam[1] = itemBase.iQuality;      //����1��Ʒ��
		taskSysInfo.arrayParam[2] = srcAttr.iLevel;         //����2���ȼ�
		taskSysInfo.iProgress = 1;
		m_pUser->m_userTask.DisposeTask(taskSysInfo);
    }
    srcAttr.iExp = nextExp;


    return CT_TRUE;
}


// һ�������Ƿ�������
CT_BOOL CCUserElmAttr::isMarrowFullLevel( ItemData &elmData ) const
{
    DataMap_CONSTIT it = m_DataMap.find(elmData.key);
    G_ElmMarrow st = {0};
    G_ElmUpgrade stUpgrade = {0};

    if (0 == G_GetElmMarrow(elmData.iID, st)) return CT_FALSE;
    return 0 == G_GetElmUpgrade(st.iUpgraderule, it->second.iLevel + 1, stUpgrade);
}

// ����Ԫ�ؾ�����ʼ����
CT_BOOL CCUserElmAttr::GenerateAttr(const ItemData &itData,CT_INT32 iGrade/*=1*/)
{    
    _AUTOMUTEX(m_pMutex);

    G_ItemBase itBase = {0};
    if (0 == G_GetItemBase(itData.iID, &itBase)) return CT_FALSE;  // ������

    G_ElmMarrow elmMarrow = {0};
    if (0 == G_GetElmMarrow(itData.iID, elmMarrow)) return CT_FALSE; // ������

    ElmMarrowAttr stAttr = {0};
    stAttr.key = itData.key;
    stAttr.iLevel = 1;
    //srand(m_pUser->GetUserID() + time(0) + stAttr.key);
    for (int i = 0; i < sizeof(elmMarrow.elmAttr) / sizeof(*elmMarrow.elmAttr); ++i)
    {
        //����һ������ֵ, ����ΪС�����4β��
        if (elmMarrow.elmAttr[i].iAID != 0) {
            CT_INT32 iVal = (rand() + rand()) % FACTOR;
            stAttr.attrs[i] = (CT_FLOAT)abs(iVal) / FACTOR;
        }
    }

    InsertData(stAttr.key, stAttr);

    CMD_G_GetElmAttr stClient = {0};
    stClient.dwUserID = m_pUser->GetUserID();
    stClient.key = stAttr.key;
	stClient.iGrade=iGrade;
    memcpy(&stClient.attrs, &stAttr.attrs, sizeof(stClient.attrs));
    m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_ELM_ATTR, (CT_CHAR *)&stClient, sizeof(stClient), false, true);

    return CT_TRUE;        
}

// Ԫ������
CT_INT32 CCUserElmAttr::DoEatElm(const CT_INT32 iSrc, const CT_INT32 iDst, CT_INT32 &nCrit, const CT_BOOL bSendData /*= CT_TRUE*/)
{
    ItemData itSrc = {0}, itDst ={0};
    if (iSrc == iDst) return EUI_ELM_EAT_CONFLICT;
    if (!m_pUser->m_userElmPower.GetData(iSrc, itSrc)) return EUI_NOT_FOUND_ELM; // �Ҳ���Ԫ��
    if (!m_pUser->m_userElmPower.GetData(iDst, itDst)) {
        // Ŀ������Ҳ������ƶ�λ��
        itSrc.iIndex = iDst;
        m_pUser->m_userElmPower.RemoveData(iSrc);
        m_pUser->m_userElmPower.InsertData(itSrc.iIndex, itSrc);

        // ֪ͨ�û��ƶ��Ľ��
        CMD_G_ItemData itDatas[2] = {0};
        m_pUser->m_userElmPower.GetClientData(iSrc, itDatas[0]);
        m_pUser->m_userElmPower.GetClientData(iDst, itDatas[1]);
        m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_ELM_CHANGE, (CT_CHAR *)&itDatas, sizeof(itDatas), false, true);
        if (IS_ELMIDX_EQUIPPED(iSrc)) {
            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
        }

        return EUI_SUCCESS;
    }

    G_ItemBase srcBase = {0}, dstBase = {0};
    if (0 == G_GetItemBase(itSrc.iID, &srcBase) || 0 == G_GetItemBase(itDst.iID, &dstBase)) return EUI_DATA_ERROR; // ���ô���

    if (srcBase.iTID == ITEM_TYPE_ELMRUBBLE && dstBase.iTID == ITEM_TYPE_ELMRUBBLE) return EUI_ELM_RUBBLE_EAT; // ������������
    if (srcBase.iTID == dstBase.iTID && srcBase.iQuality < dstBase.iQuality) return EUI_ELM_NO_PRIORITY; // ͬ��ĵ�Ʒ�ʲ��ܳԸ�Ʒ��

    ElmMarrowAttr attrSrc = {0}, attrDst = {0};
    if (!GetData(itSrc.key, attrSrc)) attrSrc.key = itSrc.key;
    if (!GetData(itDst.key, attrDst)) attrDst.key = itDst.key;    

    CT_INT32 ret = EUI_SUCCESS;
    G_ElmMarrow srcMarrow = {0};
    G_ElmRubble dstRubble = {0};
    G_ElmMarrow dstMarrow = {0};
    if (0 == G_GetElmMarrow(srcBase.iID, srcMarrow)) return EUI_DATA_ERROR; // ���ô���
    if (dstBase.iTID == ITEM_TYPE_ELMRUBBLE) {
        if (0 == G_GetElmRubble(dstBase.iID, dstRubble)) return EUI_DATA_ERROR; // ���ô���
    } else if (dstBase.iTID == ITEM_TYPE_ELMMARROW){
        if (0 == G_GetElmMarrow(dstBase.iID, dstMarrow)) return EUI_DATA_ERROR; // ���ô���
    }
    CT_INT32 eatCnt = 0;
    MasterVariableData mvdAdd = {0};
    for (; eatCnt < itDst.iCount && ret == EUI_SUCCESS;) {
        // ÿ������֮ǰ����������
        G_ElmUpgrade stCurr = {0};
        G_GetElmUpgrade(srcMarrow.iUpgraderule, attrSrc.iLevel, stCurr);
        if (!m_pUser->m_userBase.CheckUserRes(stCurr.iCostResID, stCurr.iCostResCnt*(eatCnt+1))) {
            ret = EUI_ELM_LACK_OF_COST; // ��Դ����
        } 
		else 
		{
            CT_INT32 addExp = 0;
            if (dstBase.iTID == ITEM_TYPE_ELMRUBBLE) {
                // �Բ���
                addExp += dstRubble.iBaseExp;
                if (addExp == 0) {
                    // ֱ����һ��
                    G_ElmUpgrade stNext = {0};
                    G_GetElmUpgrade(srcMarrow.iUpgraderule, attrSrc.iLevel + 1, stNext);
                    addExp = stNext.iUpgradeExp - stCurr.iUpgradeExp;
                }
            } else if (dstBase.iTID == ITEM_TYPE_ELMMARROW){
                // �Ծ���
                addExp += attrDst.iExp + dstMarrow.iBaseExp;

                if (srcMarrow.iType == dstMarrow.iType) {
                    // ͬ�������ɣ���������þ���ֵ
                    if (abs(rand()) % 100 < (CT_INT32)(srcMarrow.fExtraExpRate * 100)) {
                        G_ValueRange vr = {0};
                        G_GetValueRange(VALUE_TYPE_ELMPOWER_EXTRA_EXP, &vr);
                        addExp += (CT_INT32)vr.fMinVal;
                        nCrit += 1;
                    }
                }
            }            
            if (!getMarrowExp(attrSrc, srcMarrow, addExp)) {
                ret = EUI_ELM_LEVEL_MAX;
            } else {
                eatCnt += 1;
                G_ChangeResType(mvdAdd, stCurr.iCostResID, stCurr.iCostResCnt);
            }            
        }
    }
    
    if (eatCnt > 0) {
        m_pUser->m_userBase.AddMastData(mvdAdd, SAVETAG_ELMPOWER_EAT);
        itDst.iCount -= eatCnt;
        m_pUser->m_userElmPower.InsertData(itDst.iIndex, itDst);    // ���±������ߵ�����    
        RemoveData(attrDst.key);    // ֻ�о����Ż���Ч
        InsertData(attrSrc.key, attrSrc);   // ��������������

        // TODO �ɹ�������־
        // �û�ID: _user, ������ID: _eat, ��������ID: _eaten, ����������: _count
		MSC_LOGDB_EatElm logdbEatElm = {0};
		MasterVariableData mvd = {0};
		m_pUser->m_userBase.GetMasterData(mvd);
		logdbEatElm.dwUserID = m_pUser->GetUserID();
		logdbEatElm.iEatID = itSrc.iID;
		logdbEatElm.iEatenID = itDst.iID;
		logdbEatElm.iCount = eatCnt;
		logdbEatElm.iUGrade = mvd.stGrade.iLevel;
		CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbEatElm.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_EATELM,(CT_CHAR*)&logdbEatElm,sizeof(MSC_LOGDB_EatElm));
    }
    return ret;

#if 0   // û���϶���������
    if (bSendData)
    {
        // ������ȡ���������ߵ�λ��
        itSrc.iIndex = iDst;
        m_pUser->m_userElmPower.RemoveData(iSrc);
        m_pUser->m_userElmPower.InsertData(itSrc.iIndex, itSrc);

        // ֪ͨ�û����ɺ�Ľ��
        CMD_G_ItemData itDatas[2] = {0};
        m_pUser->m_userElmPower.GetClientData(iSrc, itDatas[0]);
        m_pUser->m_userElmPower.GetClientData(iDst, itDatas[1]);
        m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_ELM_CHANGE, (CT_CHAR *)&itDatas, sizeof(itDatas), false, true);

        // ֪ͨ�û����������Ա仯
        CMD_G_EatElmSuc stSuc = {0};
        stSuc.dwUserID = m_pUser->GetUserID();
        stSuc.key = attrSrc.key;
        stSuc.iLevel = attrSrc.iLevel;
        stSuc.iExp = attrSrc.iExp;
        m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_EAT_ELM_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

        if (IS_INDEX_EQUIP(iSrc)) {
            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
        }
    }
#endif
}


// ���ɶ��Ԫ��
CT_INT32 CCUserElmAttr::DoEatSomeElm( const CT_INT32 iSrc, const CT_INT32 arrayDst[], const CT_INT32 nCount /*= ELM_EAT_CNT*/)
{
    // �����ˣ�����
    ElmMarrowAttr attr = {0};
    G_ElmUpgrade upgrade = {0};
    G_ItemBase itBase = {0};
    ItemData itData = {0};
    if (!m_pUser->m_userElmPower.GetData(iSrc, itData)) return EUI_NOT_FOUND_ELM;   // δ�ҵ�
    if (0 == G_GetItemBase(itData.iID, &itBase)) return EUI_DATA_ERROR; // ������
    if (itBase.iTID != ITEM_TYPE_ELMMARROW) return EUI_ELM_RUBBLE_EAT;  // ���Ͳ���    
    if (!GetData(itData.key, attr) || 0 == G_GetElmUpgrade(itBase.iQuality, attr.iLevel + 1, upgrade))
    {
        return EUI_ELM_LEVEL_MAX;
    }

	CT_INT32 *zeroRubble = new int[nCount];
	CT_INT32 Rcount = 0;
    ENUM_UseItemFail enRet = EUI_SUCCESS;
    CT_INT32 nCrit = 0; // ͳ�Ʊ�������

    for (CT_INT32 i = 0; i < nCount && enRet == EUI_SUCCESS; ++i)
    {
        if (arrayDst[i] != 0) {
			//�������ֵΪ���Ԫ�ز���
			ItemData iDst = {0};
			G_ItemBase desBase = {0};
			m_pUser->m_userElmPower.GetData(arrayDst[i], iDst);
			G_GetItemBase (iDst.iID,&desBase);
			if (desBase.iTID == ITEM_TYPE_ELMRUBBLE)
			{
				G_ElmRubble elmRubble = {0};
			    G_GetElmRubble(desBase.iID, elmRubble);
				if (elmRubble.iBaseExp == 0)
				{
					zeroRubble[Rcount++] = i;
					continue;
				}
			}

            if ((enRet = (ENUM_UseItemFail)DoEatElm(iSrc, arrayDst[i], nCrit, CT_FALSE)) != EUI_SUCCESS) {
                CCLogMeg::getSingleton().WriteError("DoEatSomeElm::user:%d; op:%d -> %d; ret:FAIL=%d", 
                    m_pUser->GetUserID(), iSrc, arrayDst[i], (CT_INT32)enRet);
            }
        }
    }

	//������ɻ���ֵΪ���Ԫ�ز���
	for (CT_INT32 i = 0; i < Rcount && enRet == EUI_SUCCESS ; ++i)
	{
        if ((enRet = (ENUM_UseItemFail)DoEatElm(iSrc, arrayDst[zeroRubble[i]], nCrit, CT_FALSE)) != EUI_SUCCESS) {
            CCLogMeg::getSingleton().WriteError("DoEatSomeElm::user:%d; op:%d -> %d; ret:FAIL=%d", 
                m_pUser->GetUserID(), iSrc, arrayDst[zeroRubble[i]], (CT_INT32)enRet);
        }
	}
	delete[] zeroRubble;

    m_pUser->m_userElmPower.TrimItem();
    
    // ֪ͨ�û����ɺ�Ľ��
    CMD_G_ItemData *pItemDatas = new CMD_G_ItemData[1 + nCount];
    CT_INT32 nReal = 0;
    m_pUser->m_userElmPower.GetClientData(iSrc, pItemDatas[nReal++]);
    for (CT_INT32 i = 0; i < nCount; ++i)
    {
        if (arrayDst[i] != 0) 
        {
            m_pUser->m_userElmPower.GetClientData(arrayDst[i], pItemDatas[nReal++]);
        }
    }
    m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_ELM_CHANGE, (CT_CHAR *)pItemDatas, sizeof(*pItemDatas) * nReal, false, true);
    delete [] pItemDatas;

    // ֪ͨ�û����������Ա仯
    DataMap_IT it = getElmAttrIT(iSrc);
    CMD_G_EatElmSuc stSuc = {0};
    stSuc.dwUserID = m_pUser->GetUserID();
    stSuc.key = it->second.key;
    stSuc.iLevel = it->second.iLevel;
    stSuc.iExp = it->second.iExp;
    stSuc.nCrit = nCrit;
    stSuc.enRet = enRet;
    m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_EAT_ELM_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);
    if (IS_ELMIDX_EQUIPPED(iSrc)) {
        m_pUser->UpdateAndSendMasterData();
    } else {
        m_pUser->UpdateAndSendMasterData(EMI_RESOURCE);
    }

	//���ɳɹ����ж���������
    CMD_G_TaskSystemInfo taskSysInfo = {0};
    taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
    taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_SWALLOW;
    taskSysInfo.iProgress = 1;
    m_pUser->m_userTask.DisposeTask(taskSysInfo);

    return EUI_SUCCESS;
}

// Ԫ��һ������
CT_INT32 CCUserElmAttr::DoEatAllElm(const CT_INT32 arrayLock[], const CT_INT32 nCount)
{
    // ȷ��������
    CCUserItem::DataMap_IT itBegin = m_pUser->m_userElmPower.m_DataMap.begin();
    if (itBegin == m_pUser->m_userElmPower.m_DataMap.end()) return EUI_NO_ELM_EAT; // û��������

    CCUserItem::DataMap_IT it = itBegin;
    itBegin = m_pUser->m_userElmPower.m_DataMap.end();
    for (; it != m_pUser->m_userElmPower.m_DataMap.end(); ++it)
    {
		// �ų��쳣
		if (it->second.iIndex < 0) continue;

        G_ItemBase beginBase = {0}, currBase ={0};
        if (0 == G_GetItemBase(it->second.iID, &currBase)) return EUI_DATA_ERROR; // ���ô���
        // �ǲ���������
        if (currBase.iTID == ITEM_TYPE_ELMRUBBLE) continue;
        // �ڱ����ڣ�����
        if (IS_ELMIDX_EQUIPPED(it->second.iIndex)) continue;
        // �����ģ�����
        CT_INT32 i = 0;
        for (; i < nCount; ++i)
        {
            if (it->second.iIndex == arrayLock[i]) break;
        }
        if (i < nCount) continue;
        // �����ˣ�����
        if (isMarrowFullLevel(it->second)) continue;

        if (itBegin == m_pUser->m_userElmPower.m_DataMap.end())
        {  
            itBegin = it;
            continue;   // ���ҵ���һ�����õ�Ԫ�ؾ�����������һ��
        }

        if (0 == G_GetItemBase(itBegin->second.iID, &beginBase)) return EUI_DATA_ERROR; // ���ô���

        // �Ƚ�Ʒ�ʣ��ȼ������飬λ��
        if (beginBase.iQuality == currBase.iQuality) {
            ElmMarrowAttr attrBegin = {0}, attrCurr = {0};
            GetData(itBegin->second.key, attrBegin);
            GetData(it->second.key, attrCurr);
            if (attrBegin.iLevel == attrCurr.iLevel) {
                if (attrBegin.iExp == attrCurr.iExp) {
                    if (it->second.iIndex < itBegin->second.iIndex) {
                        itBegin = it;
                    }
                } else if (attrBegin.iExp < attrCurr.iExp) {
                    itBegin = it;
                }
            } else if (attrBegin.iLevel < attrCurr.iLevel) {
                itBegin = it;
            }
        } else if (beginBase.iQuality < currBase.iQuality) {
            itBegin = it;
        }
    }

    if (itBegin == m_pUser->m_userElmPower.m_DataMap.end()) return EUI_NO_ELM_EAT; // û��������

    // ��ʼ����
    CT_INT32 *foodElm = new CT_INT32[m_pUser->m_userElmPower.m_DataMap.size()];
    CT_INT32 nElm = 0;
    for (CCUserItem::DataMap_IT it = m_pUser->m_userElmPower.m_DataMap.begin(); it != m_pUser->m_userElmPower.m_DataMap.end(); ++it)
    {
		// �ų��쳣
		if (it->second.iIndex < 0) continue;

        // �ų��Լ�
        if (it == itBegin) continue;
        // �ų�ս���ϵ�
        if (IS_ELMIDX_EQUIPPED(it->second.iIndex)) continue;
        // �ų�������
        CT_INT32 i = 0;
        for (; i < nCount; ++i)
        {
            if (it->second.iIndex == arrayLock[i]) break;
        }
        if (i < nCount) continue;

        // �ų�����
        if (isMarrowFullLevel(it->second)) continue;

		// �ų���������ֵΪ����
		G_ElmMarrow elmMarrow = {0};
		if (G_GetElmMarrow(it->second.iID, elmMarrow))
		{
			if (elmMarrow.iBaseExp < 0)
			{
				continue;
			}
		}		
         
        foodElm[nElm++] = it->second.iIndex;
    }
    CT_INT32 ret = DoEatSomeElm(itBegin->second.iIndex, foodElm, nElm);
    delete[] foodElm;
    return ret;
}

// ����Ԫ������
CT_VOID CCUserElmAttr::FixElmAttr(void)
{
    for (CCUserItem::DataMap_IT it = m_pUser->m_userElmPower.m_DataMap.begin(); it != m_pUser->m_userElmPower.m_DataMap.end(); ++it) {
        G_ElmMarrow elmMarrow = {0};
        if (0 != G_GetElmMarrow(it->second.iID, elmMarrow)) {            
            ElmMarrowAttr elmAttr = {0};
            if (GetData(it->second.key, elmAttr)) {
                // ��龭��
                CT_BOOL bFix = CT_FALSE;
                G_ElmUpgrade elmUp = {0};                
                if (0 != G_GetElmUpgrade(elmMarrow.iUpgraderule, elmAttr.iLevel, elmUp)) {
                    if (elmAttr.iExp < elmUp.iUpgradeExp) {
                        elmAttr.iExp = elmUp.iUpgradeExp;
                        bFix = CT_TRUE;
                    }
                }
                // �������
                for (CT_INT32 i = 0; i < sizeof(elmAttr.attrs) / sizeof(*elmAttr.attrs); ++i) {
                    if (elmAttr.attrs[i] > 1) {
                        //���²���һ������ֵ, ����ΪС�����4β��
                        if (elmMarrow.elmAttr[i].iAID != 0) {
                            CT_INT32 iVal = (rand() + rand()) % FACTOR;
                            elmAttr.attrs[i] = (CT_FLOAT)abs(iVal) / FACTOR;
                            bFix = CT_TRUE;
                        }
                    }
                }

                if (bFix) {
                    InsertData(elmAttr.key, elmAttr);
                }
            }
        }
    }
}