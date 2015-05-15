
#include <ctime>
#include <algorithm>
#include "CUserEnchant.h"
#include "CThreadMes.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "CItemScriptSetting.h"
#include "CRand.h"
#include "CNetInterface.h"
#include "CBroadcastScriptSetting.h"

CT_INT32 CCUserEnchant::transData(const EnchantData &src, CT_CHAR *pDst) const
{
    CMD_G_EnchantData *pst = (CMD_G_EnchantData *)pDst;
    pst->iBID = src.iBID;
    pst->iIDX = src.iIDX;
    pst->iGrade = src.iGrade;
	pst->iFailCount = src.iFailCount;
    return sizeof(*pst);
}

// 激活/附魔
CT_INT32 CCUserEnchant::ActiveEnchant(CT_INT32 iBID, CT_INT32 iIDX, CT_INT32 iEnchantItem)
{
#if 0
    ENUM_EnchantFail enFail = EEN_SUCCESS;
    EnchantData data = {0};
    MasterVariableData mvd = {0};
    G_EnchantData enchantData = {0};
    m_pUser->m_userBase.GetMasterData(mvd);
    if (G_GetEnchantData(iBID, iIDX, enchantData)) {
        CT_INT32 iLeft = 0;
        CT_INT32 nCostTalent = 0;
        CT_INT32 nCostDiamond = 0;
        CT_INT32 nCostGold = 0;

        G_ValueRange vrGrade = {0};
        G_ValueRange vrLevel = {0};
        G_GetValueRange(VALUE_TYPE_ENCHANT_GRADE, &vrGrade);
        G_GetValueRange(VALUE_TYPE_ENCHANT_LEVEL, &vrLevel);
        CT_INT32 iMaxLevel = (CT_INT32)vrGrade.fMaxVal * (CT_INT32)vrLevel.fMaxVal - 1; // 等级从0计数

        DataMap_IT it = m_DataMap.find(G_Combine64(iBID, iIDX));
        if (it != m_DataMap.end()) {
            if (iGrade > 0) {
                // 计算升级消耗 - 潜能点/金币/钻石
                nCostTalent = G_CalcEnchantUpgradeCostTalent(it->second.iGrade + 1, enchantData.fEnchantCost1, enchantData.fEnchantCost2);
                nCostGold = G_CalcEnchantUpgradeCostGold(it->second.iGrade + 1, enchantData.fEnchantCostGold1, enchantData.fEnchantCostGold2);
                nCostDiamond = G_CalcEnchantUpgradeCostDiamond(it->second.iGrade + 1, enchantData.fEnchantCostDiamond1, enchantData.fEnchantCostDiamond2);

                if (mvd.stRes.iTalentNum + nCostTalent < 0) enFail = EEN_NO_TALENTNUM;
                if (mvd.stRes.iGold + nCostGold < 0) enFail = EEN_NO_GOLD;
                if (mvd.stRes.iDiamod + nCostDiamond < 0) enFail = EEN_NO_DIAMOND;
                if (it->second.iGrade >= iMaxLevel) enFail = EEN_NO_NEXT;

                if (enFail == EEN_SUCCESS) {
                    // 处理消耗
                    MasterVariableData add = {0};
                    add.stRes.iTalentNum = nCostTalent;
                    add.stRes.iGold=nCostGold;
                    add.stRes.iDiamod = nCostDiamond;
                    m_pUser->m_userBase.AddMastData(add, SAVETAG_ENCHANT_SUC);

                    // 更新数据
                    it->second.iGrade += 1;
                    data = it->second;
                }
            } else {
                enFail = EEN_DATA_ERROR;
            }
        } else if (iGrade == 0) {
            // 激活条件 - 领主等级/钻石/金币/道具/前置
            G_ValueRange vrActive = {0};
            G_GetValueRange(VALUE_TYPE_ENCHANT_ACTIVE_ITEM, &vrActive);
            CT_INT32 iItemID = (CT_INT32)vrActive.fMinVal;
            CT_INT32 iItemCnt = (CT_INT32)vrActive.fMaxVal;
            nCostDiamond = (CT_INT32)enchantData.fActiveCost;
            nCostGold = (CT_INT32)enchantData.fActiveCostGold;

            if (mvd.stGrade.iLevel < enchantData.iGrade) { enFail = EEN_NO_GRADE; }
            else if (mvd.stRes.iDiamod + nCostDiamond < 0) { enFail = EEN_NO_DIAMOND; }
            else if (mvd.stRes.iGold + nCostGold < 0) { enFail = EEN_NO_GOLD; }
            else if (iIDX == 3 && iItemID != 0 && m_pUser->m_userItem.GetItemCount(iItemID) + iItemCnt < 0) { enFail = EEN_NO_ITEM; }
            else if (iIDX > 1 && m_DataMap.find(G_Combine64(iBID, iIDX - 1)) == m_DataMap.end()) { enFail = EEN_NO_PREV; }

            if (enFail == EEN_SUCCESS) {
                // 处理消耗
                MasterVariableData add = {0};
                add.stRes.iDiamod = nCostDiamond;
                add.stRes.iGold = nCostGold;
                m_pUser->m_userBase.AddMastData(add, SAVETAG_ENCHANT_SUC);
                if (iIDX == 3) {
                    m_pUser->m_userItem.AutoExpendItem(0, iItemID, -iItemCnt, ITEM_OP_MODE_EXEC);
                }

                // 更新数据
                data.iBID = iBID;
                data.iIDX = iIDX;
                InsertData(G_Combine64(data.iBID, data.iIDX), data);  

                //激活魔化广播
                if (3 == iIDX)
                {
                    CT_CHAR cBroadcastMes[CHAT_LEN] = {0};
                    CT_INT32 iMesLen = 0;
                    MasterData masterData = {0};
					CT_INT32 iBroadcastType = 4;
					CT_INT32 iBroadcastTime = 1;
					memcpy(cBroadcastMes, &iBroadcastType, sizeof(iBroadcastType));
					memcpy(cBroadcastMes + sizeof(iBroadcastType), &iBroadcastTime, sizeof(iBroadcastTime));
                    m_pUser->m_userBase.GetMasterData(masterData);
                    sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{35}<12:%d>{40}",m_pUser->GetUserID(), masterData.cNickName,iBID);
                    iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
                    G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
                }
            }
        } else {
            enFail = EEN_DATA_ERROR;
        }

        if (enFail == EEN_SUCCESS) {
            // 更新到数据库
            MSC_DB_W_EnchantData stDb = {0};
            MSC_LOGDB_Enchant stLogDb = {0};
            stDb.dwUserID = m_pUser->GetUserID();
            stLogDb.dwUserID = m_pUser->GetUserID();
            stLogDb.iUserGrade = mvd.stGrade.iLevel;
            CT_INT32 parts[] = ENCHANT_INDEX;
            for (int i = 0; i < sizeof(parts) / sizeof(*parts); ++i) {
                if (parts[i] == data.iBID) {                    
                    stLogDb.iIndex = ENCHANT_ATTR_CNT * i + (data.iIDX - 1);
                    stLogDb.iGrade = data.iGrade;
                    stDb.iData[stLogDb.iIndex] = 1; // 增量
                    stLogDb.nDiamondCost = nCostDiamond;
                    stLogDb.nGoldCost = nCostGold;
                    stLogDb.nTalentNumCost = nCostTalent;
                    break;
                }
            }
            CT_INT32 threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
            G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ENCHANT_DATA, (CT_CHAR*)&stDb, sizeof(stDb)); 

            threadID = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
            G_SendThreadInfo(threadID,  MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_ENCHANT, (CT_CHAR*)&stLogDb, sizeof(stLogDb)); 

            // 通知客户端
            m_pUser->m_userBase.GetMasterData(mvd);
            CMD_G_EnchantSuc stSuc = {0};
            stSuc.dwUserID = m_pUser->GetUserID();
            stSuc.iBID = data.iBID;
            stSuc.iIDX = data.iIDX;            
            stSuc.iGrade = data.iGrade;
            stSuc.iGold = mvd.stRes.iGold;
            stSuc.iDiamond = mvd.stRes.iDiamod;
            stSuc.iTalentNum = mvd.stRes.iTalentNum;
            m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);

            //添加附魔任务
            CMD_G_TaskEventInfo taskEventInfo = {0};
            taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
            taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_ENCHANT;
            taskEventInfo.iSubType = stSuc.iBID;
            taskEventInfo.iLevel = stSuc.iGrade+1;  //附魔级别从0开始
            m_pUser->m_userTask.DisposeTask(taskEventInfo);
            //end
        }
    } else {
        enFail = EEN_DATA_ERROR;
    }
    return enFail;
#endif
	//********************更改************************
	ENUM_EnchantFail enFile = EEN_SUCCESS;
	EnchantData data = {0};
	G_EnchantData gEnchantData = {0};
	G_EnchantUpgrade gEnchantUpgrade = {0};
	CT_INT32 iCostGold = 0;
	CT_INT32 iCostDiamond = 0;
	CT_INT32 iGrade = 0;

	DataMap_IT it = m_DataMap.find(G_Combine64(iBID, iIDX));
	it == m_DataMap.end()? (iGrade = 0): (iGrade = it->second.iGrade + 1);

	if (!G_GetEnchantData(iBID, iIDX, gEnchantData))
	{
		return EEN_DATA_ERROR;
	}

	if (!G_GetEnchantUpgrade(gEnchantData.iRuleID, iGrade + 1, &gEnchantUpgrade))  //升级消耗所以加1
	{
		return EEN_DATA_ERROR;
	}

    // 领主等级是否达到
	MasterVariableData mvd = {0};
	m_pUser->m_userBase.GetMasterData(mvd);
	if (mvd.stGrade.iLevel < gEnchantData.iGrade)   
	{
		return EEN_NO_GRADE;
	}
	
	// 是否达到最大等级
	G_ValueRange vrGrade;
	G_ValueRange vrLevel;
	CT_INT32 iMaxLevel = 0;
	G_GetValueRange(VALUE_TYPE_ENCHANT_GRADE, &vrGrade);
	G_GetValueRange(VALUE_TYPE_ENCHANT_LEVEL, &vrLevel);
	iMaxLevel = (CT_INT32)vrLevel.fMaxVal * (CT_INT32)vrGrade.fMaxVal;

	if (iGrade >= iMaxLevel)
	{
		return EEN_NO_NEXT;
	}

    // 判断消耗资源是否足够
	CMD_G_NormalReward addReward;
	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	FakeProBase fb = {0};

	if (ENUM_CREC_RES_SUC != G_PerformCostReward(gEnchantUpgrade.iCostID, m_pUser, ENUM_PCRT_TEST_PERFORM, addReward, iGoods, iCount, fb))
	{
		return EEN_NO_RES;
	}
	// 处理消耗
	memset(&mvd, 0, sizeof(mvd));
	G_ChangeResType(mvd, addReward.iResType0, addReward.iResNum0);
	G_ChangeResType(mvd, addReward.iResType1, addReward.iResNum1);
	G_ChangeResType(mvd, addReward.iResType2, addReward.iResNum2);
	G_ChangeResType(mvd, addReward.iResType3, addReward.iResNum3);
	m_pUser->m_userBase.AddMastData(mvd, SAVETAG_ENCHANT_SUC);
	
	// 计算成功率，判断是否成功
	CT_INT32 irand = rand()%100 + 1;
	CT_FLOAT fSucPercent;
	CT_BOOL  bIsUseItem = CT_FALSE;
	if (iGrade == 0)
	{
		fSucPercent = gEnchantUpgrade.fSucPresent;
	}
	else
	{
		fSucPercent = G_CalcEnchantPersent(gEnchantUpgrade.fSucPresent, gEnchantUpgrade.fAddPresent, (it->second).iFailCount);
	}
	// 判断是否使用道具
	if (iEnchantItem != 0)
	{
		G_ItemEffect gie = {0};
		if (G_GetItemEffect(iEnchantItem, gie))
		{   
			CT_INT32 iLimitMin = gie.iLimit / 100;
			CT_INT32 iLimitMax = gie.iLimit % 100;
			if (gie.iFunction == ITEM_EFFECT_ENCHANT && (iGrade >= iLimitMin && iGrade <= iLimitMax))
			{
				CT_INT32 itBID = gie.iObject / 100;
				CT_INT32 itIDX = gie.iObject % 100;

				if ((itBID == 0 && itIDX == iIDX) || (itBID == 0 && itIDX ==0) || (itBID == iBID && itIDX == iIDX) || (itBID == iBID && itIDX == 0))
				{
					if (m_pUser->m_userItem.AutoExpendItem(SAVETAG_ENCHANT_SUC, gie.iID, 1, ITEM_OP_MODE_CKEXE))
					{
						fSucPercent += gie.iAddPercent;
						bIsUseItem = CT_TRUE;
					}	
				}
			}
		}
	}
	// 判断最少最多次数
	if (bIsUseItem == CT_FALSE && iGrade != 0 && (it->second).iFailCount < gEnchantUpgrade.iMinCount)
	{
		fSucPercent = 0;
	}
	if (bIsUseItem == CT_FALSE && iGrade != 0 && (it->second).iFailCount >= gEnchantUpgrade.iMaxCount)
	{
		fSucPercent = 1;
	}
	if (irand > fSucPercent * 100)
	{
		enFile = EEN_FAIL;
	}

	// 附魔失败，更新失败次数
	if (enFile == EEN_FAIL && iGrade != 0)
	{
		// 更新内存
		(it->second).iFailCount += 1;

		// 同步失败次数到数据库
		MSC_DB_EnchantFailTime eft = {0};
		eft.dwUserID = m_pUser->GetUserID();
		eft.iBID = iBID;
		eft.iIDX = iIDX;
		eft.iFailCount = 1;       //增量

		CT_INT32 iThreadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
		G_SendThreadInfo(iThreadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ENCHANT_FAILTIME, (CT_CHAR*)&eft, sizeof(eft));

		//发送到客户端
		memset(&mvd, 0, sizeof(mvd));
		m_pUser->m_userBase.GetMasterData(mvd);
		CMD_G_EnchantSuc ges = {0};
		ges.dwUserID = m_pUser->GetUserID();
		ges.iBID = iBID;
		ges.iIDX = iIDX;
		if (iGrade == 0)
		{
			ges.iGrade = -1;
		}
		else
		{
			ges.iGrade = (it->second).iGrade;
		}
		ges.iDiamond = mvd.stRes.iDiamod;
		ges.iGold = mvd.stRes.iGold;
		ges.iTalentNum = mvd.stRes.iTalentNum;
		m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_SUC, (CT_CHAR*)&ges, sizeof(ges),false, true);
	}

	// 附魔成功
	if (enFile == EEN_SUCCESS)
	{
		EnchantSuc(iBID, iIDX, iGrade);
	}
	
	//添加附魔任务
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_ENCHANT;
	taskEventInfo.iSubType = iBID;
	taskEventInfo.iLevel = iGrade;  //附魔级别从0开始
	m_pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
	return EEN_SUCCESS;
}

CT_VOID CCUserEnchant::CheckEnchatActive(const CT_INT32 iLevel)
{
    CT_INT32 iBIDs[] = {ENCHANT_ATHENA, ENCHANT_POSEIDON, ENCHANT_HERA, ENCHANT_ARES, ENCHANT_ZEUS, ENCHANT_HARDESS};
    CT_INT32 iIDX = 1;

    for (CT_INT32 i = 0; i < sizeof(iBIDs) / sizeof(*iBIDs); ++i) {
        ActiveEnchant(iBIDs[i], iIDX, 0);
    }
}

CT_VOID CCUserEnchant::UpdateBattleStatus(MasterCombatStatus &mcs, const DataMap &enchantMap,const EnchantRewardVector &rewardVector)
{
    G_ValueRange vrGrade = {0};
    G_ValueRange vrLevel = {0};
    G_GetValueRange(VALUE_TYPE_ENCHANT_GRADE, &vrGrade);
    G_GetValueRange(VALUE_TYPE_ENCHANT_LEVEL, &vrLevel);
    CT_INT32 iMaxLevel = (CT_INT32)vrGrade.fMaxVal * (CT_INT32)vrLevel.fMaxVal;

    CT_INT32 Cond = 0;
    for (DataMap_CONSTIT it = enchantMap.begin(); it != enchantMap.end(); ++it) {
        if (it->second.iBID != 0 && it->second.iGrade == iMaxLevel) {
            Cond++;
        }
    }

    for (DataMap_CONSTIT it = enchantMap.begin(); it != enchantMap.end(); ++it) {
        if (it->second.iBID != 0) {
            G_EnchantData data ={0};
			G_EnchantUpgrade enchantUpgrade = {0};
            G_GetEnchantData(it->second.iBID, it->second.iIDX, data);
			G_GetEnchantUpgrade(data.iRuleID, (it->second).iGrade+1, &enchantUpgrade);
            // 附魔加成
//            CT_FLOAT Val = G_CalcEnchantAttrValue(it->second.iGrade + 1, data.fBase, data.fAddition);
			CT_FLOAT Val = enchantUpgrade.fAttrValue;
            // 奖励加成 - 取消了
            if (Cond == EQUIP_COUNTS * ENCHANT_ATTR_CNT) {
                //Val += G_CalcExternAddition(data.iBID, data.iIDX, data.iAID, enchantDatas[i].iGrade, data.fExtern);
            }
            CCUserBase::UpdateBattleStatus(mcs, data.iAID, Val);                 
        }
    }

	CT_INT32 iLen = rewardVector.size();
	// 额外加成奖励
	for(CT_INT32 i = 0;i < iLen; ++i)
	{
		G_EnchantReward ger = {0};
		G_GetEnchantReward(rewardVector[i], &ger);
		if (ger.iRewardType == REWARD_USER_ATTR)
		{
			G_AttributesReward gar = {0};
			G_GetAttributesReward(ger.iRewardID, &gar);

			CCUserBase::UpdateBattleStatus(mcs, gar.iResType1, gar.fResNum1);
			CCUserBase::UpdateBattleStatus(mcs, gar.iResType2, gar.fResNum2);
			CCUserBase::UpdateBattleStatus(mcs, gar.iResType3, gar.fResNum3);
			CCUserBase::UpdateBattleStatus(mcs, gar.iResType4, gar.fResNum4);
		}	
	}
}

CT_VOID CCUserEnchant::EnchantSuc(CT_INT32 iBID, CT_INT32 iIDX, CT_INT32 iGrade)
{
	CT_INT32 iFailCount = 0;
	EnchantData data = {0};
	G_EnchantUpgrade gEnchantUpgrade = {0};
	MasterVariableData mvd = {0};
	DataMap_IT it = m_DataMap.find(G_Combine64(iBID,iIDX));

	if (iGrade != 0 && it == m_DataMap.end())
	{
		CCLogMeg::getSingleton().WriteError("EnchantSuc DATA_ERROR, iBID = %d, iIDX = %d .",iBID, iIDX);
		return;
	}

	// 更新内存
	if (iGrade == 0)
	{
		data.iBID = iBID;
		data.iIDX = iIDX;
		InsertData(G_Combine64(iBID, iIDX), data);
	}
	else
	{
		iFailCount = (it->second).iFailCount;
		(it->second).iGrade += 1;
		(it->second).iFailCount = 0;
		data = it->second;
	}
	
	// 激活魔化广播
	if (iGrade == 0 && 3 == iIDX)
	{
		CT_CHAR cBroadcastMes[CHAT_LEN] = {0};
		CT_INT32 iMesLen = 0;
		MasterData masterData = {0};
		CT_INT32 iBroadcastType = 4;
		CT_INT32 iBroadcastTime = 1;
		memcpy(cBroadcastMes, &iBroadcastType, sizeof(iBroadcastType));
		memcpy(cBroadcastMes + sizeof(iBroadcastType), &iBroadcastTime, sizeof(iBroadcastTime));
		m_pUser->m_userBase.GetMasterData(masterData);
		sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{35}<12:%d>{40}",m_pUser->GetUserID(), masterData.cNickName,iBID);
		iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
		G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, 1, cBroadcastMes,iMesLen);
	}
	//附魔位达到10级，20级,25级，满级 广播
	if (iGrade == 9 || iGrade == 19 || iGrade == 24 || iGrade == 29)
	{
		MasterData masterData = {0};
		m_pUser->m_userBase.GetMasterData(masterData);
		if (iGrade == 29)
		{
			RollBroadcast rb[3]={0};
			rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
			rb[1].SetValue(iBID, "");
			rb[2].SetValue(iIDX, "");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{89}<12:%d:%d>{93}",m_pUser->GetUserID(), masterData.cNickName,iBID,iIDX);
			G_SendRollBroadecast(89,4,1,(CT_CHAR*)rb,sizeof(rb));
		}
		else
		{
			CT_INT32 iStr = iGrade / 10 + 90;
			RollBroadcast rb[5]={0};
			rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
			rb[1].SetValue(iBID, "");
			rb[2].SetValue(iIDX, "");
			rb[3].SetValue(iGrade+1, "");
			rb[4].SetValue(iStr, "");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{89}<12:%d:%d>{48}%d{60},{%d}",m_pUser->GetUserID(), masterData.cNickName,iBID,iIDX,iGrade+1,iStr);
			G_SendRollBroadecast(90,4,1,(CT_CHAR*)rb,sizeof(rb));
		}
	}

	// 如果有属性奖励，自动发放
	GetEnchantAttrReward();

	// 更新到数据库
	MSC_DB_W_EnchantData EnchantDB = {0};
	MSC_LOGDB_Enchant EnchantLog = {0};
	MSC_DB_EnchantFailTime EnchantFailTime = {0};
	EnchantDB.dwUserID = m_pUser->GetUserID();
	EnchantLog.dwUserID = m_pUser->GetUserID();
	EnchantFailTime.dwUserID = m_pUser->GetUserID();
	EnchantFailTime.iBID = iBID;
	EnchantFailTime.iIDX = iIDX;
	EnchantFailTime.iFailCount = -iFailCount;      // 增量
	EnchantLog.iUserGrade = mvd.stGrade.iLevel;
	EnchantLog.iGrade = data.iGrade;
	
	CT_INT32 parts[] = ENCHANT_INDEX;
	for (int i = 0; i < sizeof(parts) / sizeof(*parts); ++i) {
		if (parts[i] == data.iBID) {                    
			EnchantLog.iIndex = ENCHANT_ATTR_CNT * i + (data.iIDX - 1);
			EnchantLog.iGrade = data.iGrade;
			EnchantDB.iData[EnchantLog.iIndex] = 1; // 增量
		}
	}

	CT_INT32 iThreadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
	G_SendThreadInfo(iThreadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ENCHANT_DATA, (CT_CHAR*)&EnchantDB, sizeof(EnchantDB));
    G_SendThreadInfo(iThreadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ENCHANT_FAILTIME, (CT_CHAR*)&EnchantFailTime, sizeof(EnchantFailTime));

	iThreadID = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(iThreadID, MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_ENCHANT, (CT_CHAR*)&EnchantLog, sizeof(EnchantLog));
	
	//发送到客户端
	m_pUser->m_userBase.GetMasterData(mvd);
	CMD_G_EnchantSuc enchantSuc = {0};
	enchantSuc.dwUserID = m_pUser->GetUserID();
	enchantSuc.iBID = data.iBID;
	enchantSuc.iIDX = data.iIDX;
	enchantSuc.iGrade = data.iGrade;
	enchantSuc.iGold = mvd.stRes.iGold;
	enchantSuc.iDiamond = mvd.stRes.iDiamod;
	enchantSuc.iTalentNum = mvd.stRes.iTalentNum;

	m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_SUC, (CT_CHAR*)&enchantSuc, sizeof(enchantSuc), false, true);

	m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
}

ENUM_EnchantRewardFail CCUserEnchant::EnchantReward(CT_INT32 iIndex, ENUM_UseItemFail &itemFail)
{
	CT_BOOL bFalg = true;
	CT_INT32 iBID = 0;
	CT_INT32 iDIX = 0;
	
	DataMap_IT it;

	G_EnchantReward enchantReward = {0};
	if (!G_GetEnchantReward(iIndex, &enchantReward))
	{
		return EER_DATA_ERROR;
	}
	// 如果奖励已获取则跳过
	if(find(m_EnchantRewardVector.begin(), m_EnchantRewardVector.end(), enchantReward.iIndex) != m_EnchantRewardVector.end())
	{
		return EER_RECEIVED;
	}
	// 如果前置奖励没获取则跳过
	if (find(m_EnchantRewardVector.begin(), m_EnchantRewardVector.end(), enchantReward.iFront) == m_EnchantRewardVector.end() && enchantReward.iFront != 0)
	{
		return EER_FAIL;
	}
	// 判断是否符合奖励领取条件
	for (CT_INT32 i = 0;i < ENCHANT_COUNT;++i)
	{
		iBID = i / 3 + ENCHANT_ZEUS;
		iDIX = i % 3 + 1;
		it = m_DataMap.find(G_Combine64(iBID, iDIX));
		// 内存等级0-39，配置1-40
		if (enchantReward.cEnchantGroup[i] != 0 && (it == m_DataMap.end() || (it->second).iGrade + 1 < enchantReward.cEnchantGroup[i] - 1))
		{                                                                                 
			bFalg = CT_FALSE;
			break;
		}
	}
	if (bFalg == CT_FALSE)
	{   
		return EER_FAIL;
	}

	// 满足条件，发放奖励
	CMD_G_NormalReward addReward = {0};
	if (enchantReward.iRewardType == REWARD_USER_WEALTH)        // 奖励为领主资源和道具
	{
		CT_INT32 iEnFail = ENUM_CREC_RES_SUC;
		CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
		CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
		FakeProBase fb = {0};

		// 判断背包是否足够
		iEnFail = G_PerformCostReward(enchantReward.iRewardID, m_pUser, ENUM_PCRT_TEST, addReward, iGoods, iCount, fb);
		if (ENUM_CREC_ELMBAG_NOAREA == iEnFail)
		{
			itemFail = EUI_NO_ELM_SLOT;
			return EER_FAIL;
		}
		if (ENUM_CREC_ITEM_FAIL == iEnFail)
		{
			itemFail = EUI_NO_SLOT;
			return EER_FAIL;
		}

		G_PerformCostReward(enchantReward.iRewardID, m_pUser, ENUM_PCRT_PERFORM, addReward, iGoods, iCount, fb);
		//附魔奖励广播
		for (CT_INT32 j = 0;j < GOODS_COUNT*2 + TEAM_ITEM_CNT;++j)
		{
			if (iGoods[j] == 0 || iCount[j] == 0)
			{
				continue;
			}
			MasterData masterData = {0};
			RollBroadcast rb[3]={0};
			m_pUser->m_userBase.GetMasterData(masterData);
			rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
			rb[1].SetValue(enchantReward.cEnchantGroup[0],"");
			rb[2].SetValue(iGoods[j],"");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{76}%d{77}<2:%d>",m_pUser->GetUserID(), masterData.cNickName,enchantReward.cEnchantGroup[0],iGoods[j]);
			G_SendRollBroadecast(76,4,1,(CT_CHAR*)rb,sizeof(rb));
		}
		MasterVariableData mvd = {0};
		G_ChangeResType(mvd, addReward.iResType0, addReward.iResNum0);
		G_ChangeResType(mvd, addReward.iResType1, addReward.iResNum1);
		G_ChangeResType(mvd, addReward.iResType2, addReward.iResNum2);
		G_ChangeResType(mvd, addReward.iResType3, addReward.iResNum3);
		m_pUser->m_userBase.AddMastData(mvd, SAVATAG_ENCHANT_REWARD);
	}

	// 更新内存
	m_EnchantRewardVector.push_back(iIndex);

	// 读入数据库
	MSC_DB_EnchantReward er = {0};
	er.dwUserID = m_pUser->GetUserID();
	er.iIndex = iIndex;

	CT_INT32 threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ENCHANT_REWARD, (CT_CHAR*)&er, sizeof(er));

	// 发送到客户端
	CMD_G_EnchantReward_Suc ers= {0};
	MasterVariableData mvd = {0};
	m_pUser->m_userBase.GetMasterData(mvd);
	ers.dwUserID = m_pUser->GetUserID();
	ers.iIndex = iIndex;
	G_ChangeResType(mvd, addReward.iResType0, addReward.iResNum0);
	G_ChangeResType(mvd, addReward.iResType1, addReward.iResNum1);
	G_ChangeResType(mvd, addReward.iResType2, addReward.iResNum2);
	G_ChangeResType(mvd, addReward.iResType3, addReward.iResNum3);
	ers.resReward.iResType0 = addReward.iResType0;
	ers.resReward.iResNum0 = addReward.iResNum0;
	ers.resReward.iResType1 = addReward.iResType1;
	ers.resReward.iResNum1 = addReward.iResNum1;
	ers.resReward.iResType2 = addReward.iResType2;
	ers.resReward.iResNum2 = addReward.iResNum2;
	ers.resReward.iResType3 = addReward.iResType3;
	ers.resReward.iResNum3 = addReward.iResNum3;

	m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_REWARD_SUC, (CT_CHAR*)&ers, sizeof(ers),false,true);

	return EER_SUCCESS;
}

//得到以发放奖励
CT_BOOL CCUserEnchant::GetEnchantReward(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	GetEnchantAttrReward();

	if (bufLen < m_EnchantRewardVector.size() * sizeof(CT_INT32))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEnchant::GetEnchantReward bufLen < m_EnchantRewardVector.size() * sizeof(CT_INT32)");
		return CT_FALSE;
	}

	CT_INT32 iLen = m_EnchantRewardVector.size();
	for (CT_INT32 i = 0;i < iLen;++i)
	{
		memcpy(pBuf + i * sizeof(CT_INT32), &m_EnchantRewardVector[i], sizeof(CT_INT32));
	}

	bufLen = iLen;
	return CT_TRUE;
}

//得到属性奖励
CT_VOID CCUserEnchant::GetEnchantAttrReward()
{
	CT_BOOL bFalg;
	CT_INT32 ibBID = 0;
	CT_INT32 ibDIX = 0;
	CT_INT32 iBufLen = 0;
	CT_CHAR buffer[100 * sizeof(CT_INT32)] = {0};
	CT_CHAR *pBuffer = buffer;
	iBufLen = G_GetAllEnchantRewardID(&pBuffer, 100 * sizeof(CT_INT32));
	CT_INT32 *pIndex = (CT_INT32 *)pBuffer;

	// 遍历奖励列表 
	for(CT_INT32 j = 0;j < iBufLen; ++j)
	{
		G_EnchantReward ger = {0};
		G_GetEnchantReward(pIndex[j], &ger);
		bFalg = true;
		// 奖励为领主资源和道具
		if (ger.iRewardType != REWARD_USER_ATTR)        
		{
			continue;
		}
		// 奖励已领取则跳过
		if (find(m_EnchantRewardVector.begin(), m_EnchantRewardVector.end(), ger.iIndex) != m_EnchantRewardVector.end())
		{
			continue;
		}
		// 前置奖励未完成
		if (find(m_EnchantRewardVector.begin(), m_EnchantRewardVector.end(), ger.iFront) == m_EnchantRewardVector.end() && ger.iFront != 0)
		{
			continue;
		}		
		// 判断是否符合奖励领取条件
		for (CT_INT32 i = 0;i < ENCHANT_COUNT;++i)
		{
			ibBID = i / 3 + ENCHANT_ZEUS;
			ibDIX = i % 3 + 1;
			DataMap_CONSTIT dmit = m_DataMap.find(G_Combine64(ibBID, ibDIX));
			if (ger.cEnchantGroup[i] != 0 && (dmit == m_DataMap.end() || (dmit->second).iGrade + 1 < ger.cEnchantGroup[i]))
			{
				bFalg = false;
				break;
			}
		}
		// 满足条件，发放奖励
		if (bFalg == true)
		{   
			// 更新内存
			m_EnchantRewardVector.push_back(ger.iIndex);

			// 保存到数据库
			MSC_DB_EnchantReward er = {0};
			er.dwUserID = m_pUser->GetUserID();
			er.iIndex = ger.iIndex;

			CT_INT32 threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ENCHANT_REWARD, (CT_CHAR*)&er, sizeof(er));

			// 发送客户端
			CMD_G_EnchantReward_Attr era = {0};
			era.dwUserID = m_pUser->GetUserID();
			era.iIndex = ger.iIndex;
			m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_REWARD_ATTR, (CT_CHAR*)&era, sizeof(era), false, true);
		}
	}
}