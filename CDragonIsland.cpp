/****************************************************
	created:	2014/07/10		16:58
	file base:	DragonIsland
	file ext:	cpp
	author:		zhouhuan

	purpose:
*****************************************************/

#include "CDragonIsland.h"
#include "CRand.h"
#include "CTask.h"
#include "CUser.h"
//#include "CNetConnector.h"
#include "GlobeScriptInterface.h"
#include "CThreadMes.h"
#include "CFightTeam.h"
#include "CFightTeamManager.h"
#include "CUserManager.h"
#include "CBroadcastScriptSetting.h"

CCUserIsland::CCUserIsland()
{
}

CCUserIsland::~CCUserIsland()
{
}

// 获取龙岛数据
CT_BOOL CCUserIsland::GetIslandData(CT_CHAR* pBuf, CT_INT32& iLen, CT_INT32 iFTID)
{
	if (pBuf == NULL && iLen < sizeof(CMD_G_IslandData))
	{
		return CT_FALSE;
	}
	CT_INT32 iTChestNum = 0;
	CT_INT32 iGUserNum = 0;
	CT_CHAR  cName[NAME_SIZE] = {0};
	CMD_G_IslandData cIslandData = {0};
	cIslandData.iEventID = m_Point.iEventID;
	cIslandData.iEventStatus = m_Point.iStatus;
	cIslandData.iGetTimes = m_DPlayer.iGetTimes;
	cIslandData.iPointID = m_Point.iPointID;
	cIslandData.iStartChest = m_Point.iBeginChest;
	cIslandData.iThrowTimes = m_DPlayer.iThrowTimes;
	cIslandData.iType = m_Point.iType;
	cIslandData.iRewardTimes = m_DPlayer.iRewardTimes;
	if (iFTID == m_DPlayer.iFightTeamID)
	{
		cIslandData.iScore = m_DPlayer.iScore;
	}

	iGUserNum = m_GetPresentNameVector.size();
	for (CT_INT32 i = 0;i < iGUserNum && i < PRESENT_NAME_NUM;++i)
	{
		strncpy(cIslandData.iPresentUserName[i].cUserName, m_GetPresentNameVector[i].c_str(), NAME_SIZE);
	}

	iTChestNum = m_TeamChestIDVector.size();
	for (CT_INT32 j = 0;j < iTChestNum;++j)
	{
		cIslandData.iTeamChest[j] = m_TeamChestIDVector[j];
	}

	memcpy(pBuf, &cIslandData, sizeof(cIslandData));
	iLen = sizeof(CMD_G_IslandData);
	return CT_TRUE;
}
// 掷骰子
CT_INT32 CCUserIsland::Throw()
{
BEGIN:
	_AUTOMUTEX(m_pMutex);
	// 检查领主等级
	G_Building building = {0};
	MasterData mvd = {0};
	m_pUser->m_userBase.GetMasterData(mvd);
	if (!G_GetBuildingData(CITY_TYPE_DRAGONISLAND, &building))
	{
		CCLogMeg::getSingleton().WriteError("CCUserIsland::Throw G_GetBuildingData error.id:%d", CITY_TYPE_DRAGONISLAND);
		return ETF_DATA_ERROR;
	}
	if (mvd.mvd.stGrade.iLevel < building.iMasterGrade)
	{
		return ETF_LEVER_LESS;
	}

	// 检查次数是否足够(包括vip特权增加次数）
	G_ValueRange gvr = {0}; 
	if (!G_GetValueRange(VALUE_TYPE_ISLAND_THROWTIMES, &gvr))
	{
		CCLogMeg::getSingleton().WriteError("CCUserIsland::Throw G_GetValueRange error.id:%d", VALUE_TYPE_ISLAND_THROWTIMES);
		return ETF_DATA_ERROR;
	}
	if(m_DPlayer.iThrowTimes >= gvr.fMinVal + m_DPlayer.iRewardTimes + m_DPlayer.iGetTimes + m_pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_ISLAND))
	{
		return ETF_TIMES_LESS;
	}

	// 有宝箱未领取
	if (m_Point.iStatus == ISLAND_EVENT_COMPLETE || m_Point.iBeginChest != 0)
	{
		return ETF_HAVE_CHEST;
	}

	// 有事件未完成
	if (m_Point.iStatus == ISLAND_EVENT_UNCOMPLETE && m_Point.iEventID != 0)
	{
		return ETF_HAVE_EVENT;
	}

	CT_INT32 iEventIndex = 0;
	CT_INT32 iDistinguish = 0;
	CT_INT32 iLeverNum = 0;
	CT_INT32 iLeverMin = 0;
	CT_INT32 iEventID = 0;
	CT_INT32 iBeginPoint = 0;
	CT_INT32 iSum = 0;
	CT_INT32 iPointID = m_Point.iPointID;
	CT_INT32 iBeginTimes = m_DPlayer.iBeginTimes;
	CT_INT32 iBeginChest = m_Point.iBeginChest;
	G_PointChest gpc = {0};
	G_Point gPoint = {0};
	G_PointInfo gPointInfo = {0};
	G_PointEvent gPointEvent = {0};
	CT_INT32 iStep = rand() % 6 + 1;
	CT_INT32 aLeverMin[100] = {0};
	CT_INT32* pLeverMin = aLeverMin;
	MasterData md = {0};
	CMD_G_Throw_Suc gts = {0};
	CMD_G_Throw_Suc cts = {0};

	// 测试
//	CT_INT32 iStep = 1;

	iPointID == 0?iPointID=1:0;
	iBeginPoint =iPointID;
	iPointID += iStep;
	if (iPointID >= ISLAND_POINT_NUM + 1)
	{
		iPointID -= ISLAND_POINT_NUM;

		// 回到起点，发放起点奖励
		iBeginTimes += 1;
		
		if (1 != G_GetPointChest(ISLAND_BEGINCHEST_TYPE, iBeginTimes, &gpc))
		{
			CCLogMeg::getSingleton().WriteError("G_GetPointChest error. iType:%d, iLimit:%d", ISLAND_BEGINCHEST_TYPE,iBeginTimes);
			return ETF_DATA_ERROR;
		}
		// 公告
		if (iBeginTimes == 4)
		{
			MasterData md = {0};
			m_pUser->m_userBase.GetMasterData(md);
			RollBroadcast rb[2]={0};
			rb[0].SetValue(md.dwUserID, md.cNickName);
			rb[1].SetValue(gpc.iChestID, "");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{36}<2:%d>{17}<1:%d:%s>{37}",pItemData->iID, userID, masterData.cNickName);
			G_SendRollBroadecast(135,2,1,(CT_CHAR*)rb,sizeof(rb));
		}

		iBeginChest = gpc.iChestID;
	}
	
	// 得到棋点事件
	if (!G_GetPoint(iPointID, &gPoint))
	{
		CCLogMeg::getSingleton().WriteError("G_GetPoint error.iPoint:%d", iPointID);
		return ETF_DATA_ERROR;
	}
	for (CT_INT32 iPoint = 0;iPoint < ISLAND_POINTEVENT_NUM;++iPoint)
	{
		iSum += gPoint.fProbabiblity[iPoint];
	}
	CT_INT32 iEventRand = rand() % iSum + 1;
	for (CT_INT32 i = 0;i < ISLAND_POINTEVENT_NUM;++i)
	{
		iDistinguish += gPoint.fProbabiblity[i];
		if (iEventRand <= iDistinguish)
		{
			iEventIndex = i;
			break;
		}
	}

	if ((iLeverNum = G_GetAllLeverLimit(&pLeverMin, sizeof(aLeverMin))) == 0)
	{
		CCLogMeg::getSingleton().WriteError("G_GetAllLeverLimit error.");
		return ETF_DATA_ERROR;
	}

	m_pUser->m_userBase.GetMasterData(md);
	for (CT_INT32 j = iLeverNum - 1;j >= 0;--j)
	{
		if (md.mvd.stGrade.iLevel >= aLeverMin[j])
		{
			iLeverMin = aLeverMin[j];
			break;
		}
	}

	if (!G_GetPointInfo(iPointID, iLeverMin, &gPointInfo))
	{
		CCLogMeg::getSingleton().WriteError("G_GetPointInfo error.iID:%d, ILeverMin:%d", iPointID, iLeverMin);
		return ETF_DATA_ERROR;
	}
	iEventID = gPointInfo.iEvent[iEventIndex];
	// 测试
	/*iEventID = gPointInfo.iEvent[m_DPlayer.iBeginTimes];
	if (iEventID == 0)
	{
		m_Point.iPointID += 1;
		goto BEGIN;
	}*/


	if (!G_GetPointEvent(iEventID, &gPointEvent))
	{
		CCLogMeg::getSingleton().WriteError("G_GetPointEvent error.iEventID:%d", iEventID);
		return ETF_DATA_ERROR;
	}
	m_Point.iStep = iStep;
	m_Point.iPointID = iPointID;
	m_Point.iEventID = iEventID;
	m_Point.iType = gPoint.iEventType;
	m_Point.iStatus = ISLAND_EVENT_UNCOMPLETE;
	m_Point.iBeginChest = iBeginChest;
	m_DPlayer.iThrowTimes += 1;
	m_DPlayer.iBeginTimes = iBeginTimes;
	

	// 如果是奖励事件，插入奖励
	if (gPoint.iEventType == ISLAND_REWARD_EVENT)
	{
		m_Point.iEventChest = gPointEvent.iReward;
		m_Point.iStatus = ISLAND_EVENT_COMPLETE;
	}

	// 发送客户端
	cts.iBeginChest = m_Point.iBeginChest;
	cts.iEventID = m_Point.iEventID;
	cts.iNum = iStep;
	cts.iBeginPoint = iBeginPoint;
	cts.iPoint = m_Point.iPointID;
	
	m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_THROW_SUC, (CT_CHAR*)&cts, sizeof(cts));

	// 如果是任务型事件，插入任务
	if (gPoint.iEventType == ISLAND_TASK_EVENT)
	{
		// 更新客户端次数
		CMD_UpdateTaskTimes utt = {0};
		utt.iStatus = 1;
		m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_TASK_TIMES, (CT_CHAR*)&utt, sizeof(utt));

		CT_INT32 iLen = 0;
		G_TaskList gtl = {0};
		CT_CHAR	pBuf[1024];
		m_pUser->m_userTask.AddNewTask(gPointEvent.iRelationID, pBuf,sizeof(pBuf),iLen,gtl);
	}
	
	// 保存数据库
	UpdateIslandData();
	// 数据库日志
	UpdateIslandDataLog();

	return ISLAND_ENFAIL_SUC;
}

// 判断并完成当前任务
CT_VOID CCUserIsland::CheckAndDoTask(CT_INT32 iTaskID,CT_INT32 iFTID)
{
	_AUTOMUTEX(m_pMutex);
	// 检查是否为当前任务
	G_PointEvent pe = {0};
	G_GetPointEvent(m_Point.iEventID, &pe);
	if(m_Point.iType != ISLAND_TASK_EVENT || iTaskID != pe.iRelationID || m_Point.iStatus == ISLAND_EVENT_COMPLETE ||
		m_Point.iStatus == ISLAND_EVENT_REWARDED)
	{
		return;
	}

	m_Point.iStatus = ISLAND_EVENT_COMPLETE;
//	m_Point.iType = ISLAND_REWARD_EVENT;
	m_Point.iEventChest = pe.iReward;
	UpdateTeamScore(iFTID);
	// 发送客户端
	CMD_G_EventComplete_Suc cecs = {0};
	cecs.iChestID = m_Point.iEventChest;
	cecs.iEventID = m_Point.iEventID;
	m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_FIGHT_EVENT_SUC, (CT_CHAR*)&cecs, sizeof(cecs));

	// 保存数据库
	UpdateIslandData();
	// 数据库日志
	UpdateIslandDataLog();
}

// 战斗事件
CT_INT32 CCUserIsland::FightEvent(CT_INT32 iEventID)
{
	_AUTOMUTEX(m_pMutex);
	if (iEventID != m_Point.iEventID || m_Point.iType != ISLAND_FIGHT_EVENT)
	{
		return EFF_EVENTID_ERROR;
	}

	G_PointEvent gpe = {0};
	if (!G_GetPointEvent(iEventID, &gpe))
	{
		CCLogMeg::getSingleton().WriteError("G_GetPointEvent error.eventid:%d", iEventID);
		return EFF_DATA_ERROR;
	}

	MSC_Island_FightEvent mfe = {0};
	mfe.dwUserID = m_pUser->GetUserID();
	mfe.iSeed = rand();
	mfe.iEventID = iEventID;
	mfe.iMonsterID = gpe.iRelationID;
	G_GetFightingData(m_pUser, mfe.FightingData);
	m_pUser->m_userArmy.DeleteBoss();
	CT_DWORD threadid = GAME_THREAD_FIGHT + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_ISLAND_FIGHT, (CT_CHAR*)&mfe, sizeof(mfe));
	return ISLAND_ENFAIL_SUC;
}

// 立即完成
CT_INT32 CCUserIsland::CompleteImmediately(CT_INT32& iEventID,CT_INT32 iFTID)
{
	_AUTOMUTEX(m_pMutex);
	G_PointEvent pe = {0};
	MasterData md  = {0};
	MasterVariableData mvd = {0};
	iEventID = m_Point.iEventID;

	// 事件ID错误
	if (0 == m_Point.iEventID)
	{
		return EFF_EVENTID_ERROR;
	}

	// 有宝箱未领取
	if (m_Point.iType == ISLAND_REWARD_EVENT || m_Point.iEventChest != 0)
	{
		return EFF_HAVE_CHEST;
	}

	if (!G_GetPointEvent(m_Point.iEventID, &pe))
	{
		CCLogMeg::getSingleton().WriteError("G_GetPointEvent error.iEventID:%d", m_Point.iEventID);
		return EFF_DATA_ERROR;
	}

	// 钻石不足
	m_pUser->m_userBase.GetMasterData(md);
	if (md.mvd.stRes.iDiamod - pe.iDiamond < 0)
	{
		return EFF_DIAMOND_LESS;
	}

	if (m_Point.iType == ISLAND_TASK_EVENT)
	{
		m_pUser->m_userTask.UpdateTaskCompleteTime(pe.iRelationID);
	}
	mvd.stRes.iDiamod = -pe.iDiamond;
	m_pUser->m_userBase.AddMastData(mvd, SAVETAG_ISLAND_COST);
	
	m_Point.iEventChest = pe.iReward;
	m_Point.iStatus = ISLAND_EVENT_COMPLETE;
	UpdateTeamScore(iFTID);

	// 发送客户端
	CMD_G_EventComplete_Suc cecs = {0};
	cecs.iChestID = m_Point.iEventChest;
	cecs.iEventID = m_Point.iEventID;
	cecs.iDiamond = md.mvd.stRes.iDiamod - pe.iDiamond;
	m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_FIGHT_EVENT_SUC, (CT_CHAR*)&cecs, sizeof(cecs));
	
	// 保存数据库
	UpdateIslandData();
	// 数据库日志
	UpdateIslandDataLog();
	return ISLAND_ENFAIL_SUC;
}

// 放弃事件
CT_VOID CCUserIsland::GiveUpEvent()
{
	_AUTOMUTEX(m_pMutex);
	G_PointEvent gPointEvent = {0};
	// 处理任务完成
	if (m_Point.iType == ISLAND_TASK_EVENT && G_GetPointEvent(m_Point.iEventID, &gPointEvent))
	{
		m_pUser->m_userTask.DisposeIslandTask(gPointEvent.iRelationID);
	}

	m_Point.iEventID = 0;
	m_Point.iStatus = 0;
	m_Point.iType = 0;
	m_Point.iEventChest = 0;

	// 发送客户端
	CMD_G_EventComplete_Suc cecs = {0};
	cecs.iChestID = m_Point.iEventChest;
	cecs.iEventID = m_Point.iEventID;
	m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GIVEUP_EVENT, (CT_CHAR*)&cecs, sizeof(cecs));

	// 保存数据库
	UpdateIslandData();
	// 数据库日志
	UpdateIslandDataLog();
}

// 领取宝箱
CT_INT32 CCUserIsland::SendReward(CT_INT32 iType,CT_INT32 iChestID, CT_INT32 iLimit, CT_INT32 iFTID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iNum = 0;
	CT_INT32 iCount = 0;
	CT_INT32 iSlot = 0;
	CT_INT32 enfail = LOOT_SUC;
	CT_INT32 i = 1;
	CT_INT32 iChestNum = 1;
	G_PointChest gPointChest = {0};
	CT_INT32 aScore[TEAM_CHEST_NUM] = {0};
	CT_INT32* pScore = aScore;
	G_PointEvent gPointEvent = {0};

	// 判断背包
	CMD_G_NormalReward addReward = {0};
	CT_INT32 iEnFail = ENUM_CREC_RES_SUC;
	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCounts[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	FakeProBase fb = {0};

	// 奖励为投掷次数时不用判断背包
	if (iChestID != VALUE_TYPE_THROWTIMES)
	{
		enfail = m_pUser->LootItems(SAVATAG_ISLAND_REWARD, &iChestID, &iChestNum, 1, ITEM_OP_MODE_CHECK);
	}
	if ( LOOT_BAG_FULL== enfail)
	{
		return EGC_BAG_LESS;
	}
	if (LOOT_NO_ITEM == enfail)
	{
		return EGC_NO_CHEST;
	}

	// 判断宝箱是否存在
	if (iLimit != 0 && iType == ISLAND_TEAMCHEST_TYPE )
	{
		if (iLimit > m_DPlayer.iScore)
		{
			return EGC_NO_CHEST;
		}
		else if (!G_GetPointChest(ISLAND_TEAMCHEST_TYPE, iLimit, &gPointChest))
		{
			CCLogMeg::getSingleton().WriteError("G_GetPointChest error.iType:%d,iLimit:%d", ISLAND_TEAMCHEST_TYPE, iLimit);
			return EGC_DATA_ERROR;
		}
	}

	if (iType == ISLAND_BEGINCHEST_TYPE && iChestID == m_Point.iBeginChest)
	{
		m_Point.iBeginChest = 0;
	}
	else if (iType == ISLAND_EVENTCHEST_TYPE && iChestID == m_Point.iEventChest)
	{
		if (m_Point.iType == ISLAND_TASK_EVENT && G_GetPointEvent(m_Point.iEventID, &gPointEvent))
		{
 			m_pUser->m_userTask.DisposeIslandTask(gPointEvent.iRelationID);
		}
		// 当奖励为投掷次数时直接发放
		if (m_Point.iEventChest == VALUE_TYPE_THROWTIMES)
		{
			G_ValueRange vr = {0};
			if (G_GetValueRange(VALUE_TYPE_THROWTIMES, &vr))
			{
				m_DPlayer.iRewardTimes += vr.fMinVal;

				// 发送客户端
				CMD_UpdateThrowTimes utt = {0};
				utt.iThrowTimes = m_DPlayer.iRewardTimes;
				utt.iType = ISLAND_REWARDTIMES_TYPE;
				m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_THROW_TIMES, (CT_CHAR*)&utt, sizeof(utt));
			}
		}
		// 如果是奖励事件，加战队积分
		if (m_Point.iType == ISLAND_REWARD_EVENT)
		{
			UpdateTeamScore(iFTID);
		}
		m_Point.iType = 0;
		m_Point.iEventChest = 0;
		m_Point.iEventID = 0;
		m_Point.iStatus = ISLAND_EVENT_REWARDED;
	}
	else if (iType == ISLAND_TEAMCHEST_TYPE && 0 != gPointChest.iChestID)
	{
		if (iFTID != m_DPlayer.iFightTeamID)
		{
			return EGC_NO_CHEST;
		}
		if (CheckScoreChest(iLimit))
		{
			// 宝箱已领取
			return EGC_BOX_GET;
		}
		m_TeamChestIDVector.push_back(iLimit);

		// 保存到数据库
		MSC_DB_IslandScoreReward msr = {0};
		msr.dwUserID = m_pUser->GetUserID();
		msr.aScoreReward = iLimit;
		CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_SCOREREWARD,(CT_CHAR*)&msr,sizeof(msr));
	}
	else
	{
		return EGC_NO_CHEST;
	}

	// 发放奖励
	m_pUser->LootItems(SAVATAG_ISLAND_REWARD, &iChestID, &iChestNum, 1, ITEM_OP_MODE_EXEC);

	// 保存数据库
	UpdateIslandData();
	// 数据库日志
	UpdateIslandDataLog();

	
	 //发送客户端
	 CMD_GetChest_Suc cgcs = {0};
	 cgcs.iChestID = iChestID;
	 cgcs.iType = iType;
	 cgcs.iLimit = iLimit;
	 m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GET_CHEST_SUC, (CT_CHAR*)&cgcs, sizeof(cgcs));

	return ISLAND_ENFAIL_SUC;
}

CT_VOID CCUserIsland::SetPoint(Point point)
{
	_AUTOMUTEX(m_pMutex);
	memcpy(&m_Point, &point, sizeof(m_Point));

	// 保存数据库
	UpdateIslandData();
	// 数据库日志
	UpdateIslandDataLog();
}

CT_VOID CCUserIsland::SetPlayerInfo(DragonPlayer player)
{
	_AUTOMUTEX(m_pMutex);
	memcpy(&m_DPlayer, &player, sizeof(m_DPlayer));

	// 保存数据库
	UpdateIslandData();
	// 数据库日志
	UpdateIslandDataLog();
}

CT_VOID CCUserIsland::TeamIn(CT_INT32 iFTID)
{
	_AUTOMUTEX(m_pMutex);
	if (iFTID == m_DPlayer.iFightTeamID)
	{
		// 发送客户端
		CMD_UpdateScore cus = {0};
		cus.dwUserID = m_pUser->GetUserID();
		cus.iScore = m_DPlayer.iScore;
		m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_SCORE, (CT_CHAR*)&cus, sizeof(cus));
	}
}

CT_VOID CCUserIsland::TeamOut()
{
	_AUTOMUTEX(m_pMutex);
	CMD_UpdateScore cus = {0};
	cus.dwUserID = m_pUser->GetUserID();
	cus.iScore = 0;
	m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_SCORE, (CT_CHAR*)&cus, sizeof(cus));
}

CT_VOID CCUserIsland::UpdateIslandData()
{
	MSC_DB_IslandData mIsland = {0};
	mIsland.dwUserID = m_pUser->GetUserID();
	mIsland.iEventID = m_Point.iEventID;
	mIsland.iEventStatus = m_Point.iStatus;
	mIsland.iFT_ID = m_DPlayer.iFightTeamID;
	mIsland.iGetTimes = m_DPlayer.iGetTimes;
	mIsland.iPoint = m_Point.iPointID;
	mIsland.iScore = m_DPlayer.iScore;
	mIsland.iStartChest = m_Point.iBeginChest;
	mIsland.iStartTimes = m_DPlayer.iBeginTimes;
	mIsland.iThrowTimes = m_DPlayer.iThrowTimes;
	mIsland.iIsOpen = m_DPlayer.iIsOpen;
	mIsland.iRewardTimes = m_DPlayer.iRewardTimes;
	mIsland.iStep = m_Point.iStep;
	CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_UPDATE,(CT_CHAR*)&mIsland,sizeof(mIsland));
}

CT_BOOL CCUserIsland::CheckGiveUid(CT_DWORD dwFUID)
{
	CT_INT32 iCount = m_GiveUserIDVector.size();
	for (CT_INT32 i = 0;i < iCount;++i)
	{
		if (m_GiveUserIDVector[i] == dwFUID)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_BOOL CCUserIsland::CheckScoreChest(CT_INT32 iLimit)
{
	CT_INT32 iCount = m_TeamChestIDVector.size();
	for (CT_INT32 i = 0;i < iCount;++i)
	{
		if (m_TeamChestIDVector[i] == iLimit)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_VOID CCUserIsland::UpdateIslandDataLog()
{
	MSC_LOGDB_IslandData mid = {0};
	mid.dwUserID = m_pUser->GetUserID();
	mid.iEventID = m_Point.iEventID;
	mid.iEventStatus = m_Point.iStatus;
	mid.iFT_ID = m_DPlayer.iFightTeamID;
	mid.iGetTimes = m_DPlayer.iGetTimes;
	mid.iPoint = m_Point.iPointID;
	mid.iScore = m_DPlayer.iScore;
	mid.iStartTimes = m_DPlayer.iBeginTimes;
	mid.iThrowTimes = m_DPlayer.iThrowTimes;
	mid.iRewardTimes = m_DPlayer.iRewardTimes;
	CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_ISLAND,(CT_CHAR*)&mid,sizeof(mid));
}

CT_VOID CCUserIsland::UpdateFTMemberScore(CT_INT32 iScore, CT_INT32 iTeamID)
{
	_AUTOMUTEX(m_pMutex);

	if (iTeamID != m_DPlayer.iFightTeamID)
	{
		return;
	}
	m_DPlayer.iScore = iScore;

	// 发送客户端
	CMD_UpdateScore cus = {0};
	cus.dwUserID = m_pUser->GetUserID();
	cus.iScore = m_DPlayer.iScore;
	m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND,SUB_G_UPDATE_SCORE,(CT_CHAR*)&cus,sizeof(cus));

	// 更新数据库
	MSC_DB_IslandFTMemberScore ftm = {0};
	ftm.dwUserID = m_pUser->GetUserID();
	ftm.iTeamID = iTeamID;
	ftm.iScore = iScore;
	CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid, MSC_MAIN_DB_DRAGONISLAND, MSC_SUB_UPDATE_FTMEMBER_SCORE, (CT_CHAR*)&ftm, sizeof(ftm));
}

CT_VOID CCUserIsland::OpenFunction(CT_INT32 iFTID)
{
	_AUTOMUTEX(m_pMutex);
	m_DPlayer.iIsOpen = 1;
	m_DPlayer.iFightTeamID = iFTID;

	// 更新数据库
	UpdateIslandData();
}

CT_VOID CCUserIsland::UpdateIslandOnClock(CT_INT32 iFTID)
{
	_AUTOMUTEX(m_pMutex);
	Point point = {0};
	DragonPlayer player = {0};

	player.iFightTeamID = iFTID;
	SetPlayerInfo(player);
	SetPoint(point);
}

CT_VOID CCUserIsland::AddThrowTimes(CT_INT32 iCount)
{
	_AUTOMUTEX(m_pMutex);
	m_DPlayer.iTotalTimes += iCount;
	m_DPlayer.iRewardTimes += iCount;

	// 发送客户端
	CMD_UpdateThrowTimes utt = {0};
	utt.iThrowTimes = m_DPlayer.iRewardTimes;
	utt.iType = ISLAND_REWARDTIMES_TYPE;
	m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_THROW_TIMES, (CT_CHAR*)&utt, sizeof(utt));

	// 更新数据库
	UpdateIslandData();
	// 更新数据库日志
	UpdateIslandDataLog();
}

// 更新战队积分
CT_VOID CCUserIsland::UpdateTeamScore(CT_INT32 iTeamID)
{
	CT_INT32 iScorePro = m_DPlayer.iScore;
	CT_CHAR cName[NAME_SIZE+1] = {0};
	if (iTeamID == m_DPlayer.iFightTeamID && m_DPlayer.iFightTeamID != 0)
	{
		m_DPlayer.iScore += m_Point.iStep;

		CCFightTeam* pTeam = 0;
		if (CCFightTeamManager::getSingleton().Find(iTeamID, &pTeam))
		{
			FightTeamMemberUID ftUid = {0};
			pTeam->GetFightTeamMemberUID(&ftUid);
			for (CT_INT32 iMember = 0;iMember < ftUid.dwCount;++iMember)
			{
				CCUser *pFMember;
				if (CCUserManager::getSingleton().Find(ftUid.dwMemberUID[iMember], &pFMember))
				{
					pFMember->m_userIsland.UpdateFTMemberScore(m_DPlayer.iScore, iTeamID);
				}
				else
				{
					MSC_DB_IslandFTMemberScore ftm = {0};
					ftm.dwUserID = ftUid.dwMemberUID[iMember];
					ftm.iTeamID = iTeamID;
					ftm.iScore = m_DPlayer.iScore;
					CT_DWORD threadid = GAME_THREAD_DATABASE + ftm.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
					G_SendThreadInfo(threadid, MSC_MAIN_DB_DRAGONISLAND, MSC_SUB_UPDATE_FTMEMBER_SCORE, (CT_CHAR*)&ftm, sizeof(ftm));
				}
			}
		}
		// 公告
		pTeam->GetFightTeamName(cName);
		if (iScorePro < 360 && m_DPlayer.iScore >= 360)
		{
			TeamScoreBroadcast(360, 136, cName);
		}
		else if (iScorePro < 480 && m_DPlayer.iScore >= 480)
		{
			TeamScoreBroadcast(480, 137, cName);
		}
	}
}

CT_VOID CCUserIsland::TeamScoreBroadcast(CT_INT32 iLimit, CT_INT32 iIndex, CT_CHAR* pName)
{
	G_PointChest gPointChest = {0};
	RollBroadcast rb[3]={0};
	if (!G_GetPointChest(ISLAND_TEAMCHEST_TYPE, iLimit, &gPointChest))
	{
		return;
	}
	rb[0].SetValue(0, pName);
	rb[1].SetValue(m_DPlayer.iScore, "");
	rb[2].SetValue(gPointChest.iChestID, "");
	G_SendRollBroadecast(iIndex,2,1,(CT_CHAR*)rb,sizeof(rb));
}