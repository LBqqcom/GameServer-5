#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUserManager.h"
#include "CDatabaseMes.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "CUserTower.h"
#include "GlobeScriptInterface.h"
#include "CFightTeamManager.h"
#include "CBroadcastScriptSetting.h"


CCUserTower::CCUserTower()
{
	m_bEnable = CT_FALSE;
}
CCUserTower::~CCUserTower(void)
{
}
CT_VOID CCUserTower::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser = pUser;
	m_pMutex = pMutex;
	m_TowerData.clear();
	m_AwardStatus.clear();
	m_TowerSlotAward.clear();
	m_TowerMSG.clear();
	m_TowerEvent.clear();
	m_TowerPVP.clear();
	m_TowerHelp.clear();
	m_iMaxCPID = 0;
}

//清理用户数据
CT_VOID CCUserTower::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_TowerData.clear();
	m_AwardStatus.clear();
	m_TowerSlotAward.clear();
	m_TowerMSG.clear();
	m_TowerEvent.clear();
	m_TowerPVP.clear();
	m_TowerHelp.clear();
}

CT_VOID CCUserTower::InsertTowerData(const TowerData &data, CT_BOOL bInit)
{
	_AUTOMUTEX(m_pMutex);
	m_TowerData[data.iCPID] = data;

	G_TowerCheckPoint tcp = {0};
	if (G_GetTowerCheckPoint(data.iCPID, &tcp))
	{
		G_AttributesReward ar = {0};
		if (G_GetAttributesReward(tcp.iPersonRewardID, &ar))
		{
			if (ar.iResType1 > 0)
			{
				m_AwardStatus[ar.iResType1] += ar.fResNum1;
			}
			if (ar.iResType2 > 0)
			{
				m_AwardStatus[ar.iResType2] += ar.fResNum2;
			}
			if (ar.iResType3 > 0)
			{
				m_AwardStatus[ar.iResType3] += ar.fResNum3;
			}
			if (ar.iResType4 > 0)
			{
				m_AwardStatus[ar.iResType4] += ar.fResNum4;
			}
		}
	}
	if (data.iCPID > m_iMaxCPID)
	{
		m_iMaxCPID = data.iCPID;
		if (!bInit)
		{
//			CT_DWORD dwFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(m_pUser->GetUserID());
//			CCFightTeam *pFightTeam = 0;
//			if (CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
			{
				MSC_Tower_Pass towerPass = {0};
				towerPass.dwUserID = m_pUser->GetUserID();
				towerPass.iCPID = m_iMaxCPID;
				CT_DWORD dwThreadID = GAME_THREAD_CITY +  towerPass.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
				G_SendThreadInfo(dwThreadID,   MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_FIGHTTEAM_TOWER_CP, (CT_CHAR*)&towerPass, sizeof(MSC_Tower_Pass));
//				pFightTeam->SetMemberTowerCP(m_pUser->GetUserID(), m_iMaxCPID);

			}
		}
	}
}

// 得到关卡信息
CT_BOOL CCUserTower::GetTowerData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_TowerData.size() * sizeof(CMD_G_TowerData);
	if (len > bufLen || len == 0)
	{
		return CT_FALSE;
	}

	TD_IT it = m_TowerData.begin();
	TD_IT it_end = m_TowerData.end();
	CMD_G_TowerData towerData = {0};
	towerData.dwUserID = m_pUser->GetUserID();
	for (int i = 0; it != it_end; ++it, ++i)
	{
		towerData.iCPID = it->second.iCPID;
		towerData.iTreasure = it->second.iTreasure;
		towerData.iTime = it->second.iTime;
		memcpy(pBuf + sizeof(CMD_G_TowerData) * i, &towerData, sizeof(CMD_G_TowerData));
	}
	bufLen = len;

	return CT_TRUE;
}

// 检查某关是否已经通过 iType:0.曾经通关过 1.当天通关过
CT_BOOL CCUserTower::CheckPointPass(CT_INT32 iCPID, CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	if (iCPID < 0)
	{
		return CT_FALSE;
	}
	if (iCPID == 0)
	{
		return CT_TRUE;
	}
	

	TD_IT it = m_TowerData.find(iCPID);
	if (it != m_TowerData.end())
	{
		if (it->second.iTime > 0 || iType==0)
		{
			return CT_TRUE;
		}
	}

	return CT_FALSE;
}

CT_BOOL CCUserTower::CheckHelpArea(CT_INT32 iCPID)
{
	_AUTOMUTEX(m_pMutex);
	if (iCPID < 0)
	{
		return CT_FALSE;
	}
	if (iCPID == 0)
	{
		return CT_TRUE;
	}

	CT_INT32 iMaxTown=0;
	for(TowerDataMap::iterator it= m_TowerData.begin();it != m_TowerData.end();++it)
	{
		G_TowerCheckPoint tcp = {0};
		if(!G_GetTowerCheckPoint((*it).second.iCPID, &tcp))
		{
			CCLogMeg::getSingleton().WriteError("CCUserTower::CheckPointPass iFloor =%i", (*it).second.iCPID);
			continue;
		}
		if(tcp.iAreaID > iMaxTown)
			iMaxTown=tcp.iAreaID;
	}
	G_TowerCheckPoint tcp = {0};
	if(!G_GetTowerCheckPoint(iCPID, &tcp))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTower::CheckPointPass iFloor =%i", iCPID);
		return CT_FALSE;
	}

	if(iMaxTown+1 >= tcp.iAreaID)
		return CT_TRUE;

	return CT_FALSE;
}
// 检查关卡是否可以扫荡
CT_BOOL CCUserTower::CheckPointRaids(CT_INT32 iEndCPID)
{
	_AUTOMUTEX(m_pMutex);

	TD_IT it = m_TowerData.find(iEndCPID);
	TD_IT it_end = m_TowerData.end();
	if (it == it_end)
	{
		return CT_FALSE;
	}

	for (CT_INT32 i=0;i<5;i++)
	{
		it++;
		if (it == it_end)
		{
			return CT_FALSE;
		}
	}
	return CT_TRUE;
}

// 更新扫荡
CT_VOID CCUserTower::UpdateRaids(CT_INT32 iBeginCPID, CT_INT32 iEndCPID)
{
	_AUTOMUTEX(m_pMutex);

	TD_IT it = m_TowerData.find(iBeginCPID);
	TD_IT it_end = m_TowerData.find(iEndCPID);
	if (it == m_TowerData.end() || it_end == m_TowerData.end())
	{
		CCLogMeg::getSingleton().WriteError("CCUserTower::UpdateRaids m_TowerData data error! %d,%d", iBeginCPID,iEndCPID);
		return;
	}
	it_end++;	//跳到哨兵位
	G_TowerCheckPoint tcp = {0};
	G_CostRewardGroup crg = {0};
	CT_INT32 iGoodCount = 0;
	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CMD_G_NormalReward addReward = {0};
	CMD_G_TowerPassReward tpr = {0};
	MSC_LOGDB_Tower logdbbTower = {0};
	logdbbTower.iBeginCPID = iBeginCPID;

	CT_INT32 i = 0;
	CMD_G_TowerData towerData[1000] = {0};
	for (; it != it_end && i<1000; it++)
	{
		if (it->second.iTime > 0)	
		{
			continue;
		}
		if (!G_GetTowerCheckPoint(it->first, &tcp))
		{
			CCLogMeg::getSingleton().WriteError("CCTowerEvent::UpdateRaids CheckPoint no exists %d", it->first);
			CT_INT32 iFailCode = ETPF_INI_DATA_ERROR;
			m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
			return;
		}

		FakeProBase fb={0};
		G_GetFakeProDta(m_pUser,SAVETAGEX_TOWER_TIME,fb);
		if (G_PerformCostReward(tcp.iNomalWin, m_pUser, ENUM_PCRT_TEST_PERFORM, addReward, iGoods, iCount,fb) != ENUM_CREC_RES_SUC)
		{
			tpr.iCPID = it->second.iCPID;
			break;
		}
		G_ClearFakeProDat(m_pUser,SAVETAGEX_TOWER_TIME,fb);
		MasterVariableData mvd ={0};
		MasterData MasterInfo={0};
		m_pUser->m_userBase.GetMasterData(MasterInfo);
		addReward.iResNum0 = G_GetFightingGold(addReward.iResNum0,MasterInfo.factor.fBattleFactor);
		addReward.iResNum1 = G_GetFightingGold(addReward.iResNum1,MasterInfo.factor.fBattleFactor);
		addReward.iResNum2 = G_GetFightingGold(addReward.iResNum2,MasterInfo.factor.fBattleFactor);
		addReward.iResNum3 = G_GetFightingGold(addReward.iResNum3,MasterInfo.factor.fBattleFactor);
		G_ChangeResType(mvd,addReward.iResType0,addReward.iResNum0);
		G_ChangeResType(mvd,addReward.iResType1,addReward.iResNum1);
		G_ChangeResType(mvd,addReward.iResType2,addReward.iResNum2);
		G_ChangeResType(mvd,addReward.iResType3,addReward.iResNum3);
		m_pUser->m_userBase.AddMastData(mvd, SAVETAG_TOWRE_PASS_REWARD);
		crg.res[0].iNum += addReward.iResNum0;
		crg.res[1].iNum += addReward.iResNum1;
		crg.res[2].iNum += addReward.iResNum2;
		crg.res[3].iNum += addReward.iResNum3;
		
		CT_INT32 iRand = rand();
		if (iRand % 1000 < tcp.fTreasurePro * 1000)
		{
			it->second.iTreasure = GetPosByRandNum(tcp.iTreasureGroupID);
		}
		towerData[i].dwUserID = m_pUser->GetUserID();
		towerData[i].iCPID = it->first;
		towerData[i].iTime = it->second.iTime + 1;
		towerData[i++].iTreasure = it->second.iTreasure;
		logdbbTower.iEndCPID = it->first;
	}
	crg.res[0].iID += addReward.iResType0;
	crg.res[1].iID += addReward.iResType1;
	crg.res[2].iID += addReward.iResType2;
	crg.res[3].iID += addReward.iResType3;

	CMD_G_NormalReward reward = {0};
	MasterVariableData mvd2 ={0};
	m_pUser->m_userBase.GetMasterData(mvd2);
	reward.iResType0 = addReward.iResType0;
	reward.iResType1 = addReward.iResType1;
	reward.iResType2 = addReward.iResType2;
	reward.iResType3 = addReward.iResType3;
	G_ChangeResType(mvd2,addReward.iResType0,reward.iResNum0);
	G_ChangeResType(mvd2,addReward.iResType1,reward.iResNum1);
	G_ChangeResType(mvd2,addReward.iResType2,reward.iResNum2);
	G_ChangeResType(mvd2,addReward.iResType3,reward.iResNum3);
	tpr.gReward.dwUserID = m_pUser->GetUserID();
	tpr.gReward = reward;
	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_DATA_UPDATE,(CT_CHAR*)towerData, sizeof(CMD_G_TowerData)*i);
	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_SUC, (CT_CHAR*)&tpr, sizeof(CMD_G_TowerPassReward));

	it_end = it;
	it = m_TowerData.find(iBeginCPID);

	for (; it != it_end; it++)
	{
		it->second.iTime += 1;
		MSC_DB_TOWER dbTower = {0};
		dbTower.dwU_ID = m_pUser->GetUserID();
		dbTower.iCPID = it->first;
		dbTower.iTreasure = it->second.iTreasure;
		dbTower.iTime = it->second.iTime;

		CT_DWORD threadid = GAME_THREAD_DATABASE + dbTower.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_UPDATE,(CT_CHAR*)&dbTower,sizeof(MSC_DB_TOWER));

		//扫荡完成，判断指定关卡的通关任务
		CMD_G_TaskSystemInfo stTaskSys = {0};
		stTaskSys.iTaskMainType = TASK_MAIN_TYPE_SYS;
		stTaskSys.iTaskSubType = TASK_SUB_TYPE_SYSTEM_POINTOR;
		stTaskSys.arrayParam[0] = dbTower.iCPID;              //参数0：关卡id
		m_pUser->m_userTask.DisposeTask(stTaskSys);

		//判断N次通关任务
		stTaskSys.iTaskSubType = TASK_SUB_TYPE_SYSTEM_CHALLENGE;
		stTaskSys.iProgress = 1;
		m_pUser->m_userTask.DisposeTask(stTaskSys);
	}
	//日志
	
	logdbbTower.dwUserID = m_pUser->GetUserID();
	for (CT_INT32 i=0; i<4; i++)
	{
		logdbbTower.iResType[i] = crg.res[i].iID;
		logdbbTower.iResNum[i] = crg.res[i].iNum;
	}
	logdbbTower.iUGrade = mvd2.stGrade.iLevel;
	CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbbTower.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TOWER_PASS,(CT_CHAR*)&logdbbTower,sizeof(MSC_LOGDB_Tower));
}

CT_VOID CCUserTower::UpdateCheckPoint(CT_INT32 iCPID, CT_INT32 iScore)
{
	_AUTOMUTEX(m_pMutex);
	if (iCPID < 1)
	{
		return;
	}

	G_TowerCheckPoint tcp = {0};
	G_CostRewardGroup crg = {0};
	if (!G_GetTowerCheckPoint(iCPID, &tcp))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerPassApply 关卡不存在 %d", iCPID);
		CT_INT32 iFailCode = ETPF_INI_DATA_ERROR;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	CT_INT32 iTreasure = 0;
	TowerDataMap::iterator it = m_TowerData.find(iCPID);
	CT_BOOL bFirst = CT_FALSE;
	if (m_TowerData.end() == it)
	{
		bFirst = CT_TRUE;
		CT_INT32 iRand = rand();
		if (iRand % 1000 < tcp.fTreasurePro * 1000)
		{
			iTreasure = GetPosByRandNum(tcp.iTreasureGroupID);
		}
		TowerData tData = {0};
		tData.iCPID = iCPID;
		tData.iTreasure = iTreasure;
		tData.iTime = 1;
		if (!G_GetCostRewardGroup(tcp.iFirstWin, &crg))
		{
			CCLogMeg::getSingleton().WriteError("CCUserTower::G_GetTowerWinReward iFirstWin =%d", tcp.iFirstWin);
			CT_INT32 iFailCode = ETPF_INI_DATA_ERROR;
			m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
			return;
		}
		InsertTowerData(tData);
		m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
	}
	else
	{
		m_TowerData[iCPID].iTime += 1;

		if (m_TowerData[iCPID].iTime == 1)
		{
			CT_INT32 iRand = rand();
			if (iRand % 1000 < tcp.fTreasurePro * 1000)
			{
				iTreasure = GetPosByRandNum(tcp.iTreasureGroupID);
			}
			m_TowerData[iCPID].iTreasure = iTreasure;
			if (!G_GetCostRewardGroup(tcp.iNomalWin, &crg))
			{
				CCLogMeg::getSingleton().WriteError("CCUserTower::G_GetTowerWinReward iCPID =%d", iCPID);
				CT_INT32 iFailCode = ETPF_INI_DATA_ERROR;
				m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
				return;
			}
		}
	}

	CMD_G_TowerPassReward tpr = {0};
	if (m_TowerData[iCPID].iTime == 1)
	{
		m_pUser->LootItems(SAVETAG_TOWRE_PASS_REWARD, &(crg.item[0].iID), &(crg.item[0].iNum));
		m_pUser->LootItems(SAVETAG_TOWRE_PASS_REWARD, &(crg.item[1].iID), &(crg.item[1].iNum));
		CT_INT32 j=0;
		if (crg.item[0].iID > 0)
		{
			tpr.iItemID[j] = crg.item[0].iID;
			tpr.iItemNum[j++] = crg.item[0].iNum;
		}
		if (crg.item[1].iID > 0)
		{
			tpr.iItemID[j] = crg.item[1].iID;
			tpr.iItemNum[j++] = crg.item[1].iNum;
		}

		CT_INT32 iGoodCount = 0;
		CT_INT32 iGoods[GOODS_COUNT] = {0};
		CT_INT32 iCount[GOODS_COUNT] = {0};
		FakeProBase fb={0};
		G_GetFakeProDta(m_pUser,SAVETAGEX_TOWER_TIME,fb);
		iGoodCount = G_GetGoodsGroupInfo(crg.iItemGroup1, iGoods,iCount,fb);
		G_ClearFakeProDat(m_pUser,SAVETAGEX_TOWER_TIME,fb);
		m_pUser->LootItems(SAVETAG_TOWRE_PASS_REWARD, iGoods, iCount, iGoodCount);
		for (CT_INT32 i=0; i<iGoodCount,j<4; i++)
		{
			tpr.iItemID[j] = iGoods[i];
			tpr.iItemNum[j++] = iCount[i];
		}

		memset(iGoods, 0, sizeof(iGoods));
		memset(iCount, 0, sizeof(iCount));
		iGoodCount = G_GetGoodsGroupInfo(crg.iItemGroup2, iGoods,iCount,fb);
		m_pUser->LootItems(SAVETAG_TOWRE_PASS_REWARD, iGoods, iCount, iGoodCount);
		for (CT_INT32 i=0; i<iGoodCount,j<4; i++)
		{
			tpr.iItemID[j] = iGoods[i];
			tpr.iItemNum[j++] = iCount[i];
		}
		
		MasterData MasterInfo={0};
		m_pUser->m_userBase.GetMasterData(MasterInfo);
		MasterVariableData mvd ={0};
		crg.res[0].iNum = G_GetFightingGold(crg.res[0].iNum,MasterInfo.factor.fBattleFactor);
		crg.res[1].iNum = G_GetFightingGold(crg.res[1].iNum,MasterInfo.factor.fBattleFactor);
		crg.res[2].iNum = G_GetFightingGold(crg.res[2].iNum,MasterInfo.factor.fBattleFactor);
		crg.res[3].iNum = G_GetFightingGold(crg.res[3].iNum,MasterInfo.factor.fBattleFactor);
		G_ChangeResType(mvd,crg.res[0].iID,crg.res[0].iNum);
		G_ChangeResType(mvd,crg.res[1].iID,crg.res[1].iNum);
		G_ChangeResType(mvd,crg.res[2].iID,crg.res[2].iNum);
		G_ChangeResType(mvd,crg.res[3].iID,crg.res[3].iNum);
		m_pUser->m_userBase.AddMastData(mvd, SAVETAG_TOWRE_PASS_REWARD);

		tpr.gReward.dwUserID = m_pUser->GetUserID();
		tpr.iCPID = iCPID;
		tpr.iScore = iScore;
		tpr.iTreasure = iTreasure;
		tpr.gReward.iResType0 = crg.res[0].iID;
		tpr.gReward.iResType1 = crg.res[1].iID;
		tpr.gReward.iResType2 = crg.res[2].iID;
		tpr.gReward.iResType3 = crg.res[3].iID;
		m_pUser->m_userBase.GetMasterData(mvd);
		G_ChangeResType(mvd,tpr.gReward.iResType0,tpr.gReward.iResNum0);
		G_ChangeResType(mvd,tpr.gReward.iResType1,tpr.gReward.iResNum1);
		G_ChangeResType(mvd,tpr.gReward.iResType2,tpr.gReward.iResNum2);
		G_ChangeResType(mvd,tpr.gReward.iResType3,tpr.gReward.iResNum3);
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_SUC, (CT_CHAR*)&tpr, sizeof(CMD_G_TowerPassReward));

	MasterData masterData = {0};
	m_pUser->m_userBase.GetMasterData(masterData);
	if (bFirst && tcp.iType == 1)
	{
		RollBroadcast rb[3]={0};
		rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
		rb[1].SetValue(tcp.iAreaID, "");
		rb[2].SetValue(iCPID,"");
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{61}<14:%d:%d>{62}", m_pUser->GetUserID(), masterData.cNickName, tcp.iAreaID, iCPID);
		G_SendRollBroadecast(61,4,60,(CT_CHAR*)rb,sizeof(rb));
	}

	MSC_DB_TOWER dbTower = {0};
	dbTower.dwU_ID = m_pUser->GetUserID();
	dbTower.iCPID = iCPID;
	dbTower.iTreasure = m_TowerData[iCPID].iTreasure;
	dbTower.iTime = m_TowerData[iCPID].iTime;
	
	CT_DWORD threadid = GAME_THREAD_DATABASE + dbTower.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_UPDATE,(CT_CHAR*)&dbTower,sizeof(MSC_DB_TOWER));

	//日志
	MSC_LOGDB_Tower logdbbTower = {0};
	logdbbTower.dwUserID = m_pUser->GetUserID();
	logdbbTower.iBeginCPID = iCPID;
	for (CT_INT32 i=0; i<4; i++)
	{
		logdbbTower.iResType[i] = crg.res[i].iID;
		logdbbTower.iResNum[i] = crg.res[i].iNum;
	}
	logdbbTower.iUGrade = masterData.mvd.stGrade.iLevel;
	threadid = GAME_THREAD_LOGDATABASE + logdbbTower.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TOWER_PASS,(CT_CHAR*)&logdbbTower,sizeof(MSC_LOGDB_Tower));
	
}

// 插入占领点生产奖励
CT_VOID CCUserTower::InsertTowerSlotAward(TowerSlotAward &towerSlotAward, CT_INT32 iDBInit)
{
	_AUTOMUTEX(m_pMutex);
 	TowerSlotAwardKey tKey;
	tKey.dwUserID = towerSlotAward.dwUserID;
	tKey.iCPID = towerSlotAward.iFloor;
	tKey.iSlot = towerSlotAward.iSlot;
	G_NormalReward gnr = towerSlotAward.gReward;
	if (gnr.iGetResNum1 == 0 && gnr.iGetResNum2 == 0 && gnr.iGetResNum3 == 0 && gnr.iGetResNum4 == 0 && 
		gnr.iGetItemNum1 == 0 && gnr.iGetItemNum2 == 0 && gnr.iGetItemGroupNum1 == 0 && gnr.iGetItemGroupNum2 == 0)
	{
		return;
	}

	TowerSlotAwardMap::iterator it= m_TowerSlotAward.find(tKey);
	if (it == m_TowerSlotAward.end())
	{
		m_TowerSlotAward.insert(TowerSlotAwardMap::value_type(tKey, gnr));
	}
	else
	{
		gnr.iGetResNum1 += it->second.iGetResNum1;
		gnr.iGetResNum2 += it->second.iGetResNum2;
		gnr.iGetResNum3 += it->second.iGetResNum3;
		gnr.iGetResNum4 += it->second.iGetResNum4;
		gnr.iGetItemNum1 += it->second.iGetItemNum1;
		gnr.iGetItemNum2 += it->second.iGetItemNum2;
		gnr.iGetItemGroupNum1 += it->second.iGetItemGroupNum1;
		gnr.iGetItemGroupNum2 += it->second.iGetItemGroupNum2;
		it->second = gnr;
	}

	if (iDBInit != 1)
	{
		CMD_G_TowerSlotAward gTowerSlotAward = {0};
		gTowerSlotAward.dwUserID = towerSlotAward.dwUserID;
		gTowerSlotAward.iFloor = towerSlotAward.iFloor;
		gTowerSlotAward.iSlot = towerSlotAward.iSlot;
		gTowerSlotAward.gReward = towerSlotAward.gReward;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_SLOTAWARD_UPDATE,(CT_CHAR*)&gTowerSlotAward,sizeof(CMD_G_TowerSlotAward));

		MSC_DB_TowerSlotReward dbTowerSlotAward = {0};
		dbTowerSlotAward.dwUserID = m_pUser->GetUserID();
		dbTowerSlotAward.iFloor = towerSlotAward.iFloor;
		dbTowerSlotAward.iSlot = towerSlotAward.iSlot;
		dbTowerSlotAward.mnReward = gnr;

		CT_DWORD threadid = GAME_THREAD_DATABASE + dbTowerSlotAward.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_SLOT_REWARD,(CT_CHAR*)&dbTowerSlotAward,sizeof(MSC_DB_TowerSlotReward));
	}
}

CT_BOOL CCUserTower::GetTowerSlotAward(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_TowerSlotAward.size() * sizeof(CMD_G_TowerSlotAward);
	if (len > bufLen || len == 0)
	{
		return CT_FALSE;
	}

	TowerSlotAwardMap::iterator it = m_TowerSlotAward.begin();
	TowerSlotAwardMap::iterator it_end = m_TowerSlotAward.end();
	CMD_G_TowerSlotAward towerSlotAward = {0};
	towerSlotAward.dwUserID = m_pUser->GetUserID();
	for (int i = 0; it != it_end; ++it, ++i)
	{
		towerSlotAward.iFloor = it->first.iCPID;
		towerSlotAward.iSlot = it->first.iSlot;
		towerSlotAward.gReward = it->second;
		memcpy(pBuf + sizeof(CMD_G_TowerSlotAward) * i, &towerSlotAward, sizeof(CMD_G_TowerSlotAward));
	}
	bufLen = len;

	return CT_TRUE;
}

// 领取占领点生产奖励
CT_VOID CCUserTower::RecvTowerSlotAward(TowerSlotAwardKey &towerSlotAwardKey)
{
	_AUTOMUTEX(m_pMutex);
	TowerSlotAwardMap::iterator it= m_TowerSlotAward.find(towerSlotAwardKey);
	if (it == m_TowerSlotAward.end())
	{
		return;
	}

	CT_INT32 iGoodCount = 0;
	CT_INT32 iGoods[GOODS_COUNT] = {0};
	CT_INT32 iCount[GOODS_COUNT] = {0};
	FakeProBase fb={0};
	G_GetFakeProDta(m_pUser,SAVETAGEX_TOWER_TIME,fb);
	iGoodCount = G_GetGoodsGroupInfo(it->second.iGetItemGroupID1, iGoods,iCount,fb);
	G_ClearFakeProDat(m_pUser,SAVETAGEX_TOWER_TIME,fb);
	for(CT_INT32 i=0; i<iGoodCount; i++)
	{
		iCount[i] = iCount[i] * it->second.iGetItemGroupNum1;
	}
	m_pUser->LootItems(SAVETAG_TOWRE_SLOT_REWARD, iGoods, iCount, iGoodCount);

	memset(iGoods, 0, sizeof(iGoods));
	memset(iCount, 0, sizeof(iCount));
	iGoodCount = G_GetGoodsGroupInfo(it->second.iGetItemGroupID2, iGoods,iCount,fb);
	for(CT_INT32 i=0; i<iGoodCount; i++)
	{
		iCount[i] = iCount[i] * it->second.iGetItemGroupNum2;
	}
	m_pUser->LootItems(SAVETAG_TOWRE_SLOT_REWARD, iGoods, iCount, iGoodCount);

	m_pUser->LootItems(SAVETAG_TOWRE_SLOT_REWARD, &(it->second.iGetItemID1), &(it->second.iGetItemNum1));
	m_pUser->LootItems(SAVETAG_TOWRE_SLOT_REWARD, &(it->second.iGetItemID2), &(it->second.iGetItemNum2));

	MasterVariableData mvd ={0};
	G_ChangeResType(mvd,it->second.iGetResType1,it->second.iGetResNum1);
	G_ChangeResType(mvd,it->second.iGetResType2,it->second.iGetResNum2);
	G_ChangeResType(mvd,it->second.iGetResType3,it->second.iGetResNum3);
	G_ChangeResType(mvd,it->second.iGetResType4,it->second.iGetResNum4);
	m_pUser->m_userBase.AddMastData(mvd, SAVETAG_TOWRE_SLOT_REWARD);

	CMD_G_TowerSlotReward tsr = {0};
	tsr.gReward.dwUserID = towerSlotAwardKey.dwUserID;
	tsr.iFloor = towerSlotAwardKey.iCPID;
	tsr.iSlot = towerSlotAwardKey.iSlot;
	tsr.gReward.iResType0 = it->second.iGetResType1;
	tsr.gReward.iResType1 = it->second.iGetResType2;
	tsr.gReward.iResType2 = it->second.iGetResType3;
	tsr.gReward.iResType3 = it->second.iGetResType4;
	m_pUser->m_userBase.GetMasterData(mvd);
	G_ChangeResType(mvd,tsr.gReward.iResType0,it->second.iGetResNum1);
	G_ChangeResType(mvd,tsr.gReward.iResType1,it->second.iGetResNum2);
	G_ChangeResType(mvd,tsr.gReward.iResType2,it->second.iGetResNum3);
	G_ChangeResType(mvd,tsr.gReward.iResType3,it->second.iGetResNum4);
	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_SLOTAWARD_SUC, (CT_CHAR*)&tsr, sizeof(CMD_G_TowerSlotReward));

	MSC_DB_TowerSlotRewardDel dbTowerSlotRewardDel = {0};
	dbTowerSlotRewardDel.dwUserID = towerSlotAwardKey.dwUserID;
	dbTowerSlotRewardDel.iFloor = towerSlotAwardKey.iCPID;
	dbTowerSlotRewardDel.iSlot = towerSlotAwardKey.iSlot;

	CT_DWORD threadid = GAME_THREAD_DATABASE + dbTowerSlotRewardDel.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_SLOT_REWARD_DEL,(CT_CHAR*)&dbTowerSlotRewardDel,sizeof(MSC_DB_TowerSlotRewardDel));

	MSC_LOGDB_TowerSlot logdbTowerSlot = {0};
	logdbTowerSlot.dwUserID = it->first.dwUserID;
	logdbTowerSlot.iCPID = it->first.iCPID;
	logdbTowerSlot.iSlotID = it->first.iSlot;
	logdbTowerSlot.iResType[0] = it->second.iGetResType1;
	logdbTowerSlot.iResType[1] = it->second.iGetResType2;
	logdbTowerSlot.iResType[2] = it->second.iGetResType3;
	logdbTowerSlot.iResType[3] = it->second.iGetResType4;
	logdbTowerSlot.iResNum[0] = it->second.iGetResNum1;
	logdbTowerSlot.iResNum[1] = it->second.iGetResNum2;
	logdbTowerSlot.iResNum[2] = it->second.iGetResNum3;
	logdbTowerSlot.iResNum[3] = it->second.iGetResNum4;
	logdbTowerSlot.iUGrade = mvd.stGrade.iLevel;
	m_TowerSlotAward.erase(it);
	CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbTowerSlot.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TOWER_OCCUPY,(CT_CHAR*)&logdbTowerSlot,sizeof(MSC_LOGDB_TowerSlot));
}

// 检查占领点生产奖励
CT_BOOL CCUserTower::CheckTowerSlotAward(TowerSlotAwardKey &towerSlotAwardKey)
{
	_AUTOMUTEX(m_pMutex);
	TowerSlotAwardMap::iterator it= m_TowerSlotAward.find(towerSlotAwardKey);
	if (it != m_TowerSlotAward.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCUserTower::IsEmptySlotAward()
{
	_AUTOMUTEX(m_pMutex);
	if (m_TowerSlotAward.size() == 0)
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_VOID CCUserTower::InsertTowerMSG(const TowerMSG &msg)
{
	_AUTOMUTEX(m_pMutex);
	m_TowerMSG.insert(TowerMSGMap::value_type(msg.iTime, msg));
}

CT_VOID CCUserTower::DeleteTowerMSG(CT_INT32 iType, CT_INT64 iTime)
{
	_AUTOMUTEX(m_pMutex);
	m_TowerMSG.erase(iTime);

	MSC_DB_MSG_DELETE dbMSG = {0};
	dbMSG.dwUserID = m_pUser->GetUserID();
	dbMSG.iMSGType = iType;
	dbMSG.iTime = iTime;
	CT_DWORD threadid = GAME_THREAD_DATABASE + dbMSG.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_MSG_DEL,(CT_CHAR*)&dbMSG,sizeof(dbMSG));
}

CT_BOOL CCUserTower::GetTowerMSG(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_TowerMSG.size() * sizeof(CMD_G_TowerMSG);
	if (len > bufLen || len == 0)
	{
		return CT_FALSE;
	}

	TowerMSGMap::iterator it = m_TowerMSG.begin();
	TowerMSGMap::iterator it_end = m_TowerMSG.end();
	CMD_G_TowerMSG towerMSG = {0};
	towerMSG.dwUserID = m_pUser->GetUserID();
	for (int i = 0; it != it_end; ++it, ++i)
	{
		towerMSG.iType = it->second.iType;
		towerMSG.iTime = it->second.iTime;
		memcpy(towerMSG.cMSG, it->second.cMSG, sizeof(towerMSG.cMSG));
		memcpy(pBuf + sizeof(CMD_G_TowerMSG) * i, &towerMSG, sizeof(CMD_G_TowerMSG));
	}
	bufLen = len;

	return CT_TRUE;
}

CT_INT32 CCUserTower::GetPosByRandNum(CT_INT32 iTreasureGroupID)
{
	//不必加锁
	if (iTreasureGroupID < 1)
	{
		return 0;
	}
	G_TowerTreasureGroup ttg = {0};
	if (!G_GetTowerTreasureGroup(iTreasureGroupID, &ttg))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTower::GetPosByRandNum iTreasureGroupID =%d", iTreasureGroupID);
		return 0;
	}
	CT_INT32 iWeight = 0;
	iWeight += ttg.iWight1;
	iWeight += ttg.iWight2;
	ttg.iWight2 += ttg.iWight1;
	iWeight += ttg.iWight3;
	ttg.iWight3 = ttg.iWight2;

	CT_INT32 iRand = rand() % iWeight;
	if (iRand < ttg.iWight1)
	{
		return ttg.iTreasureID1;
	}
	if (iRand < ttg.iWight2)
	{
		return ttg.iTreasureID2;
	}
	if (iRand < ttg.iWight3)
	{
		return ttg.iTreasureID3;
	}
	return 0;
}

CT_VOID CCUserTower::TreasureAbandon(CT_INT32 iCPID, CT_INT32 iTreasure)
{
	_AUTOMUTEX(m_pMutex);
	if (iCPID < 1 || iTreasure < 1)
	{
		CCLogMeg::getSingleton().WriteError("CCUserTower::TreasureAbandon 参数无效 %d-%d", iCPID, iTreasure);
		return;
	}
	CT_INT32 iFailCode = 0;
	TowerDataMap::iterator it = m_TowerData.find(iCPID);
	if (it == m_TowerData.end())
	{
		CT_INT32 iFailCode = ETPF_CPID_INVALID;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_TREASURE_ABANDON_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	if (it->second.iTreasure != iTreasure)
	{
		CT_INT32 iFailCode = ETPF_TREASURE_INVALID;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_TREASURE_ABANDON_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_TREASURE_ABANDON_SUC, (CT_CHAR*)&iCPID, sizeof(CT_INT32));

	it->second.iTreasure = 0;
	MSC_DB_TOWER dbTower = {0};
	dbTower.dwU_ID = m_pUser->GetUserID();
	dbTower.iCPID = iCPID;
	dbTower.iTreasure = m_TowerData[iCPID].iTreasure;
	dbTower.iTime = m_TowerData[iCPID].iTime;

	CT_DWORD threadid = GAME_THREAD_DATABASE + dbTower.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_UPDATE,(CT_CHAR*)&dbTower,sizeof(MSC_DB_TOWER));
}

CT_VOID CCUserTower::TreasureUse(CT_INT32 iCPID, CT_INT32 iTreasure,CT_INT32 iUseType)
{
	_AUTOMUTEX(m_pMutex);
	if (iCPID < 1 || iTreasure < 1)
	{
		CCLogMeg::getSingleton().WriteError("CCUserTower::TreasureUse 参数无效 %d-%d-%d", iCPID, iTreasure,iUseType);
		return;
	}
	CT_INT32 iFailCode = 0;
	TowerDataMap::iterator it = m_TowerData.find(iCPID);
	if (it == m_TowerData.end())
	{
		iFailCode = ETPF_CPID_INVALID;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	if (it->second.iTreasure != iTreasure)
	{
		iFailCode = ETPF_TREASURE_INVALID;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	G_TowerTreasureBase ttb = {0};
	if (!G_GetTowerTreasureBase(iTreasure, &ttb))
	{
		iFailCode = ETPF_INI_DATA_ERROR;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	
	CT_INT32 iCostGroup = 0;
	switch (iUseType)
	{
	case 1:
		iCostGroup = ttb.iCostGroup1;
		break;
	case 2:
		iCostGroup = ttb.iCostGroup2;
		break;
	default:
		iFailCode = ETPF_USETYPE_INVALID;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	CMD_G_NormalReward reward = {0};
	CMD_G_NormalReward addReward = {0};
	CMD_G_NormalReward costReward = {0};
	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	FakeProBase fb={0};
	if (G_PerformCostReward(ttb.iGetGroup, m_pUser, ENUM_PCRT_TEST, addReward, iGoods, iCount,fb) != ENUM_CREC_RES_SUC)
	{
		iFailCode = ETPF_BAG_FULL;	//等客户端更新再放开
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	if (G_PerformCostReward(iCostGroup, m_pUser, ENUM_PCRT_TEST_PERFORM, addReward, iGoods, iCount,fb) > 0)
	{
		iFailCode = ETPF_CREC_RES_FAIL;
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	MasterVariableData mvd ={0};
	G_ChangeResType(mvd,addReward.iResType0,addReward.iResNum0);
	G_ChangeResType(mvd,addReward.iResType1,addReward.iResNum1);
	G_ChangeResType(mvd,addReward.iResType2,addReward.iResNum2);
	G_ChangeResType(mvd,addReward.iResType3,addReward.iResNum3);
	m_pUser->m_userBase.AddMastData(mvd, SAVETAG_TOWRE_USE_TREASURE);
	memset(&mvd, 0, sizeof(MasterVariableData));
	m_pUser->m_userBase.GetMasterData(mvd);
	costReward.iResType0 = addReward.iResType0;
	costReward.iResType1 = addReward.iResType1;
	costReward.iResType2 = addReward.iResType2;
	costReward.iResType3 = addReward.iResType3;
	G_ChangeResType(mvd,addReward.iResType0,costReward.iResNum0);
	G_ChangeResType(mvd,addReward.iResType1,costReward.iResNum1);
	G_ChangeResType(mvd,addReward.iResType2,costReward.iResNum2);
	G_ChangeResType(mvd,addReward.iResType3,costReward.iResNum3);


	memset(&reward, 0, sizeof(reward));
	memset(&addReward, 0, sizeof(addReward));
	memset(iGoods, 0, sizeof(iGoods));
	memset(iCount, 0, sizeof(iCount));
	
	G_GetFakeProDta(m_pUser,SAVETAGEX_TOWER_TIME,fb);
	G_PerformCostReward(ttb.iGetGroup, m_pUser, ENUM_PCRT_PERFORM, addReward, iGoods, iCount,fb);
	G_ClearFakeProDat(m_pUser,SAVETAGEX_TOWER_TIME,fb);
	memset(&mvd, 0, sizeof(MasterVariableData));
	G_ChangeResType(mvd,addReward.iResType0,addReward.iResNum0);
	G_ChangeResType(mvd,addReward.iResType1,addReward.iResNum1);
	G_ChangeResType(mvd,addReward.iResType2,addReward.iResNum2);
	G_ChangeResType(mvd,addReward.iResType3,addReward.iResNum3);
	m_pUser->m_userBase.AddMastData(mvd, SAVETAG_TOWRE_TREASURE_REWARD);
	memset(&mvd, 0, sizeof(MasterVariableData));
	m_pUser->m_userBase.GetMasterData(mvd);
	reward.iResType0 = addReward.iResType0;
	reward.iResType1 = addReward.iResType1;
	reward.iResType2 = addReward.iResType2;
	reward.iResType3 = addReward.iResType3;
	G_ChangeResType(mvd,addReward.iResType0,reward.iResNum0);
	G_ChangeResType(mvd,addReward.iResType1,reward.iResNum1);
	G_ChangeResType(mvd,addReward.iResType2,reward.iResNum2);
	G_ChangeResType(mvd,addReward.iResType3,reward.iResNum3);
	CMD_G_TowerTreasureReward ttr = {0};
	ttr.gCost = costReward;
	ttr.gReward = reward;
	ttr.iCPID = iCPID;
	ttr.iTreasureID = iTreasure;
	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE_SUC, (CT_CHAR*)&ttr, sizeof(ttr));

	it->second.iTreasure = 0;
	MSC_DB_TOWER dbTower = {0};
	dbTower.dwU_ID = m_pUser->GetUserID();
	dbTower.iCPID = iCPID;
	dbTower.iTreasure = m_TowerData[iCPID].iTreasure;
	dbTower.iTime = m_TowerData[iCPID].iTime;

	CT_DWORD threadid = GAME_THREAD_DATABASE + dbTower.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_UPDATE,(CT_CHAR*)&dbTower,sizeof(MSC_DB_TOWER));

	MSC_LOGDB_TreasureUse logdbTreasureUse = {0};
	logdbTreasureUse.dwUserID = m_pUser->GetUserID();
	logdbTreasureUse.iCPID = iCPID;
	logdbTreasureUse.iTreasureID = iTreasure;
	logdbTreasureUse.iUseType = iUseType;
	logdbTreasureUse.iResType[0] = addReward.iResType0;
	logdbTreasureUse.iResType[1] = addReward.iResType1;
	logdbTreasureUse.iResType[2] = addReward.iResType2;
	logdbTreasureUse.iResType[3] = addReward.iResType3;
	logdbTreasureUse.iResNum[0] = addReward.iResNum0;
	logdbTreasureUse.iResNum[1] = addReward.iResNum1;
	logdbTreasureUse.iResNum[2] = addReward.iResNum2;
	logdbTreasureUse.iResNum[3] = addReward.iResNum3;
	logdbTreasureUse.iUGrade = mvd.stGrade.iLevel;
	CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbTreasureUse.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TREASUREUSE,(CT_CHAR*)&logdbTreasureUse,sizeof(MSC_LOGDB_TreasureUse));

	//使用宝箱成功，判断宝箱任务
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_CHEST;
	taskSysInfo.arrayParam[0] = iTreasure;         //参数0：宝箱id
	taskSysInfo.iProgress = 1;
	m_pUser->m_userTask.DisposeTask(taskSysInfo);

}

CT_BOOL CCUserTower::HaveTreasure()
{
	_AUTOMUTEX(m_pMutex);
	TowerDataMap::iterator it = m_TowerData.begin();
	TowerDataMap::iterator it_end = m_TowerData.end();
	for (; it!=it_end; it++)
	{
		if (it->second.iTreasure > 0)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_VOID CCUserTower::UpdateBattleStatus(MasterCombatStatus &mcs, const AwardStatusMap &awardStatusMap)
{
	//静态, 不要加锁
	AwardStatusMap::const_iterator it = awardStatusMap.begin();
	AwardStatusMap::const_iterator it_end = awardStatusMap.end();
	for (;it != it_end; it++)
	{
		CCUserBase::UpdateBattleStatus(mcs, it->first, it->second);
	}
}

CT_VOID CCUserTower::RefreshTowerPass()
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iTowerHelp = 0;
	m_pUser->m_userBase.SetMastDataEx(iTowerHelp,SAVETAGEX_TOWER_HELP, CT_FALSE);
	if (m_TowerData.size() < 1)
	{
		return;
	}
	TowerDataMap::iterator it = m_TowerData.begin();
	TowerDataMap::iterator it_end = m_TowerData.end();
	for (; it!=it_end; it++)
	{
		it->second.iTime = 0;
		it->second.iTreasure = 0;
	}
	m_TowerHelp.clear();
	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_DATA_REFRESH);
}

CT_VOID  CCUserTower::InsertPVPInfo(const MSC_TowerPVP_ResRobbery &PVPInfo)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT64 iFID = PVPInfo.iFID;
	if (PVPInfo.iWin == WINER_LIFT)
	{
		m_TowerPVP[iFID] = PVPInfo;
	}
	else
	{
		if (m_TowerPVP.find(iFID) == m_TowerPVP.end())
		{
			m_TowerPVP[iFID] = PVPInfo;
		}
	}

	if (m_TowerPVP.size() > 8)
	{
		m_TowerPVP.erase(m_TowerPVP.begin());
	}
}

CT_BOOL  CCUserTower::GetPVPInfo(CT_INT64 iFID, MSC_TowerPVP_ResRobbery *PVPInfo)
{
	_AUTOMUTEX(m_pMutex);
	TowerPVPMap::iterator it = m_TowerPVP.find(iFID);
	if (m_TowerPVP.end() == it)
	{
		return CT_FALSE;
	}
	
	memcpy(PVPInfo, &(it->second), sizeof(MSC_TowerPVP_ResRobbery));
	return CT_TRUE;
}

CT_VOID  CCUserTower::InsertEvent(const TowerEvent &towerEvent, CT_BOOL bLeftWin)
{
	_AUTOMUTEX(m_pMutex);
	if (bLeftWin)
	{
		m_TowerEvent[towerEvent.iFightingID] = towerEvent;
	}
	else
	{
		if (m_TowerEvent.find(towerEvent.iFightingID) == m_TowerEvent.end())
		{
			m_TowerEvent[towerEvent.iFightingID] = towerEvent;
		}
	}

	m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_EVENT_ADD, (CT_CHAR*)&towerEvent, sizeof(TowerEvent));

	if (m_TowerEvent.size() > 8)
	{
		m_TowerEvent.erase(m_TowerEvent.begin());
	}
}

CT_VOID CCUserTower::InitPVPInfo(const MSC_TowerPVP_ResRobbery &PVPInfo)
{
	_AUTOMUTEX(m_pMutex);
	m_TowerPVP[PVPInfo.iFID] = PVPInfo;
}

CT_VOID CCUserTower::InitEvent(TowerEvent &towerEvent)
{
	_AUTOMUTEX(m_pMutex);
	m_TowerEvent[towerEvent.iFightingID] = towerEvent;

}

CT_BOOL CCUserTower::GetEventData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_TowerEvent.size() * sizeof(CMD_G_ElementEvent);
	if (len > bufLen || len == 0)
	{
		return CT_FALSE;
	}

	TowerEventMap::iterator it = m_TowerEvent.begin();
	TowerEventMap::iterator it_end = m_TowerEvent.end();
	for (int i = 0; it != it_end; ++it, ++i)
	{
		memcpy(pBuf + sizeof(CMD_G_ElementEvent) * i, &(it->second), sizeof(CMD_G_ElementEvent));
	}
	bufLen = len;

	return CT_TRUE;
}

CT_VOID CCUserTower::InsertHelp(CT_DWORD dwFID)
{
	_AUTOMUTEX(m_pMutex);
	if (m_TowerHelp.find(dwFID) != m_TowerHelp.end())
	{
		return;
	}
	m_TowerHelp.insert(dwFID);

	MSC_DB_TowerHelp dbTowerHelp = {0};
	dbTowerHelp.dwU_ID = m_pUser->GetUserID();
	dbTowerHelp.dwF_ID = dwFID;

	CT_DWORD threadid = GAME_THREAD_DATABASE + dbTowerHelp.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_HELP,(CT_CHAR*)&dbTowerHelp,sizeof(MSC_DB_TowerHelp));
}

CT_BOOL CCUserTower::CheckHelp(CT_DWORD dwFID)
{
	_AUTOMUTEX(m_pMutex);
	if (m_TowerHelp.find(dwFID) != m_TowerHelp.end())
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCUserTower::GetHelpData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_TowerHelp.size() * sizeof(CT_DWORD);
	if (len > bufLen || len == 0)
	{
		return CT_FALSE;
	}

	TowerHelpSet::iterator it = m_TowerHelp.begin();
	TowerHelpSet::iterator it_end = m_TowerHelp.end();
	for (int i = 0; it != it_end; ++it, ++i)
	{
		memcpy(pBuf + sizeof(CT_DWORD) * i, &(*it), sizeof(CT_DWORD));
	}
	bufLen = len;

	return CT_TRUE;
}


