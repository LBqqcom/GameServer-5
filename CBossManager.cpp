#include "CBossManager.h"
#include "CConfig.h"
#include "CLogMeg.h"
#include "CBroadcastScriptSetting.h"
#include "CThreadMes.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CTop.h"
#include <math.h>
#include "CBroadcastScriptSetting.h"

template<> CCBossManager* CCSingleton<CCBossManager>::ms_Singleton=0;

CCBossManager::CCBossManager(CT_VOID)
{
	m_bActive = CT_FALSE;
	m_iBossGrade = 0;
	m_iPrePlayer = 0;
	m_iPreFinishTime = 0;
}

CCBossManager::~CCBossManager(CT_VOID)
{
}

CT_VOID CCBossManager::SetActive(CT_BOOL bActive)	//活动开始
{
	_AUTOMUTEX(&m_mutex);
	m_bActive=bActive;
	if (bActive)
	{
		if (0 == m_iBossGrade)
		{
			m_iPreFinishTime = g_iGlobeData[GLOBEDATA_TYPE_BOSS_PREFINISH];
			m_iPrePlayer = g_iGlobeData[GLOBEDATA_TYPE_BOSS_PREPLAYER];
			m_iBossGrade = g_iGlobeData[GLOBEDATA_TYPE_BOSS_GRADE];
		}
		CT_INT32 iHP = 0;
		if (0 == m_iBossGrade)	//如果是第一次打BOSS
		{
			m_iBossGrade = 1;
		}
		else
		{
			m_iBossGrade += G_GetBossGradeRule(1, m_iPreFinishTime);
		}
		G_ValueRange vr = {0};
		if (!G_GetValueRange(VALUE_TYPE_BOSS_GRADE_RANGE, &vr))
		{
			vr.fMinVal = 1;
			vr.fMaxVal = 20;
		}
		m_iBossGrade = m_iBossGrade<vr.fMinVal?vr.fMinVal:m_iBossGrade;
		m_iBossGrade = m_iBossGrade>vr.fMaxVal?vr.fMaxVal:m_iBossGrade;
		iHP = G_GetBossGradeRule(2, m_iBossGrade);
		G_BossBase bossBase = {0};
		if (G_GetBossBase(m_iAEID, &bossBase))
		{
			m_iBossHPMax = iHP + bossBase.fPlayerArg*m_iPrePlayer + bossBase.fGradeArg*CCTop::getSingleton().GetTopGradeAverage();
		}
		else
		{
			m_iBossHPMax = iHP + 30000*m_iPrePlayer + 5000*CCTop::getSingleton().GetTopGradeAverage();
		}
		//m_iBossHPMax = 1000000;	//测试
		m_lBeginTime = time(0);
		m_HPHurtMap.clear();
		m_HPTopMap.clear();
		m_RewardMap.clear();
		m_BattleList.clear();
		m_iBossHP = m_iBossHPMax;
	}
	else
	{
		G_ValueRange vr = {0};
		G_GetValueRange(VALUE_TYPE_BOSS_BUFF, &vr);
		m_lEndTime = time(0);
		if (m_iBossHP > 0)
		{
			m_iBUffID = (CT_INT32)vr.fMaxVal;
			CCUserManager::getSingleton().BuffReward(m_iBUffID);

			RefCurrTop();
			RefTop();
			SaveDB();
			//奖励
			CT_INT32 iCount = m_HPTopMap.size();
			G_BossReward bossReward = {0};
			CMD_G_WorldBossActiveReward activeReward = {0};
			HPTopMap::iterator it_top = m_HPTopMap.begin();
			HPTopMap::iterator it_top_end = m_HPTopMap.end();
			for (CT_INT32 i=1; it_top != it_top_end; ++it_top, ++i)
			{
				memset(&bossReward, 0, sizeof(G_BossReward));
				G_GetBossReward(1, iCount-i+1, &bossReward);					//排名奖
				activeReward.reward[0].iType = 1;
				activeReward.reward[0].iPropID = bossReward.iPropID;
				activeReward.reward[0].iPropNum = bossReward.iPropNum;
				memset(&bossReward, 0, sizeof(G_BossReward));
				G_GetBossReward(2, 0, &bossReward);					//参与奖
				activeReward.reward[1].iType = 2;
				activeReward.reward[1].iPropID = bossReward.iPropID;
				activeReward.reward[1].iPropNum = bossReward.iPropNum;
				activeReward.iHurtValue = it_top->first;
				activeReward.fHurtPercent = (CT_FLOAT)it_top->first / (CT_FLOAT)m_iBossHPMax;
				m_RewardMap.insert(BossRewardMap::value_type(it_top->second, activeReward));
				CCUser *pUser = 0;
				if (CCUserManager::getSingleton().Find(it_top->second,&pUser))
				{
					pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD, (CT_CHAR*)&activeReward, sizeof(CMD_G_WorldBossActiveReward));
				}
			}
			MSC_DB_GlobeData gd={0};
			m_iPreFinishTime = time(0) - m_lBeginTime;	//记录使用时间
			g_iGlobeData[GLOBEDATA_TYPE_BOSS_PREFINISH] = m_iPreFinishTime;
			gd.iType=GLOBEDATA_TYPE_BOSS_PREFINISH;
			gd.iValue=m_iPreFinishTime;
			G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
			m_iPrePlayer = m_HPHurtMap.size();			//参与人数
			g_iGlobeData[GLOBEDATA_TYPE_BOSS_PREPLAYER] = m_iPrePlayer;
			gd.iType=GLOBEDATA_TYPE_BOSS_PREPLAYER;
			gd.iValue=m_iPrePlayer;
			G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
			g_iGlobeData[GLOBEDATA_TYPE_BOSS_GRADE] = m_iBossGrade;
			gd.iType=GLOBEDATA_TYPE_BOSS_GRADE;
			gd.iValue=m_iBossGrade;
			G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
		}
		else
		{
			m_iBUffID = (CT_INT32)vr.fMinVal;
			CCUserManager::getSingleton().BuffReward(m_iBUffID);
		}
	}
}

CT_VOID CCBossManager::SendHPHurt()
{
	_AUTOMUTEX(&m_mutex);
	UserHurtMap::iterator it = m_HPHurtMap.begin();
	UserHurtMap::iterator it_end = m_HPHurtMap.end();

	CMD_G_WorldBossCurrInfo currInfo = {0};
	for (; it!=it_end;++it)
	{
		if (!it->second.bInActive)
		{
			continue;
		}
		CCUser *pUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pUser))
		{
			memset(&currInfo, 0, sizeof(currInfo));
			if (GetHPHurt(it->first, (CT_CHAR*)currInfo.userInfo, sizeof(currInfo.userInfo)))
			{
				currInfo.iHP = m_iBossHP;
				pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_CURR_INFO, (CT_CHAR*)&currInfo, sizeof(CMD_G_WorldBossCurrInfo));
			}
		}
	}
}

CT_BOOL CCBossManager::GetHPHurt(CT_DWORD dwUID, CT_CHAR *pBuf, CT_INT32 bufLen)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 len = (WORLD_BOSS_TOP_NUM+1) * sizeof(CMD_G_WorldBossUserInfo);

	if (m_HPTopMap.size() == 0)
	{
		return CT_TRUE;
	}

	CT_BOOL bFind = CT_FALSE;
	CMD_G_WorldBossUserInfo userInfo = {0};
	CT_INT32 i = 1;
	HPTopMap::iterator it = m_HPTopMap.end();
	HPTopMap::iterator it_begin = m_HPTopMap.begin();
	--it;
	for (i = 1; it != it_begin; --it, ++i)
	{
		if (i <= WORLD_BOSS_TOP_NUM)	//排名数据从 D1 开始
		{
			userInfo.dwUserID = it->second;
			userInfo.iRanking = i;					//用户排名
			userInfo.iHurtValue = it->first;
			userInfo.fHurtPercent = (CT_FLOAT)userInfo.iHurtValue / (CT_FLOAT)m_iBossHPMax;		//伤害比
			UserHurtMap::iterator it_h = m_HPHurtMap.find(userInfo.dwUserID);
			if (it_h != m_HPHurtMap.end())
			{
				memcpy(userInfo.cNickName, it_h->second.cUserName, NAME_SIZE);
			}
			memcpy(pBuf+sizeof(CMD_G_WorldBossUserInfo)*i, &userInfo, sizeof(CMD_G_WorldBossUserInfo));
		}
		if (it->second ==  dwUID)		//用户本身放在 D0 位置
		{
			bFind = CT_TRUE;
			userInfo.dwUserID = it->second;
			userInfo.iRanking = i;					//用户排名
			userInfo.iHurtValue = it->first;
			userInfo.fHurtPercent = (CT_FLOAT)userInfo.iHurtValue / (CT_FLOAT)m_iBossHPMax;		//伤害比
			UserHurtMap::iterator it_h = m_HPHurtMap.find(userInfo.dwUserID);
			if (it_h != m_HPHurtMap.end())
			{
				memcpy(userInfo.cNickName, it_h->second.cUserName, NAME_SIZE);
			}
			memcpy(pBuf, &userInfo, sizeof(CMD_G_WorldBossUserInfo));
		}
		if (bFind && i >= WORLD_BOSS_TOP_NUM)	//结束遍历
		{
			it = it_begin;	
			++it;
		}
	}
	if (it == it_begin && (i <= WORLD_BOSS_TOP_NUM || !bFind))
	{
		if (i <= WORLD_BOSS_TOP_NUM)
		{
			userInfo.dwUserID = it->second;
			userInfo.iRanking = i;					//用户排名
			userInfo.iHurtValue = it->first;
			userInfo.fHurtPercent = (CT_FLOAT)userInfo.iHurtValue / (CT_FLOAT)m_iBossHPMax;
			UserHurtMap::iterator it_h = m_HPHurtMap.find(userInfo.dwUserID);
			if (it_h != m_HPHurtMap.end())
			{
				memcpy(userInfo.cNickName, it_h->second.cUserName, NAME_SIZE);
			}
			memcpy(pBuf+sizeof(CMD_G_WorldBossUserInfo)*i, &userInfo, sizeof(CMD_G_WorldBossUserInfo));
		}
		if (it->second ==  dwUID)		//用户本身放在 D0 位置
		{
			userInfo.dwUserID = it->second;
			userInfo.iRanking = i;					//用户排名
			userInfo.iHurtValue = it->first;
			userInfo.fHurtPercent = (CT_FLOAT)userInfo.iHurtValue / (CT_FLOAT)m_iBossHPMax;
			UserHurtMap::iterator it_h = m_HPHurtMap.find(userInfo.dwUserID);
			if (it_h != m_HPHurtMap.end())
			{
				memcpy(userInfo.cNickName, it_h->second.cUserName, NAME_SIZE);
			}
			memcpy(pBuf, &userInfo, sizeof(CMD_G_WorldBossUserInfo));
		}
	}
	return CT_TRUE;
}

CT_BOOL CCBossManager::GetBossHurt(CT_DWORD dwUID, CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 len = m_BossHurtMap.size() * sizeof(CMD_G_WorldBossTopInfo);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	BossHurtMap::iterator it_boss = m_BossHurtMap.begin();
	for (CT_INT32 i=0; it_boss != m_BossHurtMap.end(); ++it_boss,++i)
	{
		CT_BOOL bFind = CT_FALSE;
		CMD_G_WorldBossTopInfo topInfo = {0};
		topInfo.iAEID = it_boss->first;
		TopHurtMap::iterator it_top = it_boss->second.begin();
		TopHurtMap::iterator it_top_end = it_boss->second.end();
		for (CT_INT32 j=1; it_top != it_top_end; ++it_top, ++j)
		{
			if (j <= WORLD_BOSS_TOP_NUM)	//排名数据从 D1 开始
			{
				topInfo.userInfo[j].dwUserID = it_top->second.dwUID;
				memcpy(topInfo.userInfo[j].cNickName, it_top->second.cUserName, NAME_SIZE);		//名称
				topInfo.userInfo[j].iRanking = it_top->first;					//用户排名
				topInfo.userInfo[j].iHurtValue = it_top->second.iHP;					//伤害数
				topInfo.userInfo[j].fHurtPercent = it_top->second.fHurtPercent;				//伤害比
			}
			if (it_top->second.dwUID == dwUID)		//用户本身放在 D0 位置
			{
				bFind = CT_TRUE;
				topInfo.userInfo[0].dwUserID = it_top->second.dwUID;
				memcpy(topInfo.userInfo[0].cNickName, it_top->second.cUserName, NAME_SIZE);		//名称
				topInfo.userInfo[0].iRanking = it_top->first;					//用户排名
				topInfo.userInfo[0].iHurtValue = it_top->second.iHP;					//伤害数
				topInfo.userInfo[0].fHurtPercent = it_top->second.fHurtPercent;				//伤害比
			}
			if (bFind && j >= WORLD_BOSS_TOP_NUM)
			{
				it_top = it_top_end;
				--it_top;
			}
		}
		memcpy(pBuf+sizeof(CMD_G_WorldBossTopInfo)*i, &topInfo, sizeof(CMD_G_WorldBossTopInfo));
	}
	bufLen = len;

	return CT_TRUE;
}

CT_VOID CCBossManager::RefCurrTop()
{
	_AUTOMUTEX(&m_mutex);
	m_HPTopMap.clear();
	UserHurtMap::iterator it = m_HPHurtMap.begin();
	UserHurtMap::iterator it_end = m_HPHurtMap.end();
	for (;it != it_end; ++it)
	{
		m_HPTopMap.insert(HPTopMap::value_type(it->second.iHP, it->first));
	}
}

CT_VOID CCBossManager::RefPlayer()
{
	_AUTOMUTEX(&m_mutex);
	G_BossAct bossAct = {0};
	G_ValueRange vr = {0};
	CT_INT32 iCoolTime = 0;
	CT_INT32 iBattleTime = 0;
	if (G_GetBossAct(1, &bossAct))
	{
		iCoolTime = bossAct.iCoolTime;
	}
	if (G_GetValueRange(VALUE_TYPE_BOSS_BATTLE_COOL, &vr))
	{
		iBattleTime = (CT_INT32)vr.fMinVal;
	}
	UserHurtMap::iterator it = m_HPHurtMap.begin();
	UserHurtMap::iterator it_end = m_HPHurtMap.end();
	for (;it != it_end; ++it)
	{
		CCUser *pUser = 0;
		CCUserManager::getSingleton().Find(it->first,&pUser);
		if (!it->second.bActCoolFlag)
		{
			if (time(0) - it->second.iActTime >= iCoolTime)
			{
				it->second.bActCoolFlag = CT_TRUE;
				if (pUser)
				{
					pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_COOL);
				}
			}
		}

		if (!it->second.bBattleFlag)
		{
			if (time(0) - it->second.iBattleTime >= iBattleTime)
			{
				it->second.bBattleFlag = CT_TRUE;
				if (pUser)
				{
					pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REVIVE);
				}				
			}
		}
	}
}

CT_VOID CCBossManager::JoinActive(CT_DWORD dwUID)
{
	_AUTOMUTEX(&m_mutex);

	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossManager::JoinActive,can't find user %d",dwUID);
		return;
	}

	if (!m_bActive)
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ACTIVE_CLOSE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_JOIN_ACTIVE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}
	CMD_G_WorldBossCurrInfo currInfo = {0};
	CMD_G_WorldBossOtherInfo otherInfo = {0};
	UserHurtMap::iterator it = m_HPHurtMap.find(dwUID);
	if (it == m_HPHurtMap.end())
	{
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		BossHPHurt bossHPHurt = {0};
		bossHPHurt.dwUID = dwUID;
		memcpy(bossHPHurt.cUserName, masterData.cNickName, NAME_SIZE);
//默认	otherInfo.iActCoolTime = 0;
// 		otherInfo.iBattleTime = 0;
// 		otherInfo.iCourageValue = 0;			//勇气值
// 		otherInfo.iEncourageCount = 0;			//激励层数
		bossHPHurt.bBattleFlag = CT_TRUE;
		bossHPHurt.bActCoolFlag = CT_TRUE;
		bossHPHurt.bInActive = CT_TRUE;
		m_HPHurtMap.insert(UserHurtMap::value_type(dwUID, bossHPHurt));
	}
	else
	{
		G_BossAct gBossAct = {0};
		G_GetBossAct(1, &gBossAct);
		otherInfo.iActCoolTime = gBossAct.iCoolTime - (time(0) - it->second.iActTime);
		if (otherInfo.iActCoolTime <= 0)
		{
			otherInfo.iActCoolTime = 0;
// 			it->second.iActTime = 0;
// 			it->second.bActCoolFlag = CT_TRUE;
		}
		it->second.bInActive = CT_TRUE;
		CT_INT32 iBattleTime = 0;
		G_ValueRange vr = {0};
		if (G_GetValueRange(VALUE_TYPE_BOSS_BATTLE_COOL, &vr))
		{
			iBattleTime = (CT_INT32)vr.fMinVal;
		}
		if (it->second.iBattleTime > 0)
		{
			otherInfo.iBattleTime = iBattleTime - (time(0) - it->second.iBattleTime);
		}
		otherInfo.iEncourageCount = it->second.iActCount;
	}
	if (GetHPHurt(dwUID, (CT_CHAR*)currInfo.userInfo, sizeof(currInfo.userInfo)))
	{
		currInfo.iHP = m_iBossHP;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_CURR_INFO, (CT_CHAR*)&currInfo, sizeof(CMD_G_WorldBossCurrInfo));
	}
	pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_JOIN_ACTIVE_SUC, (CT_CHAR*)&otherInfo, sizeof(CMD_G_WorldBossOtherInfo));

	if (pUser->m_userBoss.InsertRecord(m_iAEID))
	{
		MSC_DB_WorldBossRecord dbRecord = {0};
		dbRecord.dwUID = dwUID;
		dbRecord.iAEID = m_iAEID;
		CT_DWORD threadid = GAME_THREAD_DATABASE + dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_WORLDBOSS,MSC_SUB_DB_BOSS_RECORDADD,(CT_CHAR*)&dbRecord,sizeof(MSC_DB_WorldBossRecord));
	}
}

CT_VOID CCBossManager::ExitActive(CT_DWORD dwUID)
{
	_AUTOMUTEX(&m_mutex);
	UserHurtMap::iterator it = m_HPHurtMap.find(dwUID);
	if (it != m_HPHurtMap.end())
	{
		it->second.bInActive = CT_FALSE;
	}

}

CT_VOID CCBossManager::Encourage(CT_DWORD dwUID, CT_INT32 iType)
{
	_AUTOMUTEX(&m_mutex);

	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossManager::BattleApply,can't find user %d",dwUID);
		return;
	}

	if (!m_bActive)		//活动未开启
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ACTIVE_CLOSE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	G_BossAct bossAct = {0};
	if (!G_GetBossAct(iType, &bossAct))
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_INI_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	UserHurtMap::iterator it_hp = m_HPHurtMap.find(dwUID);
	if (it_hp == m_HPHurtMap.end())
	{
		return;
	}

	if (bossAct.iActType == 1)	//检查冷却时间
	{
		//if ((time(0) - it_hp->second.iActTime) < bossAct.iCoolTime)
		if (!it_hp->second.bActCoolFlag)
		{
			CMD_G_WorldBossFail worldBossFail = {0};
			worldBossFail.iFailCode = BOSS_ENCOURAGE_NO_COOL;
			pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
			return;
		}
	}

	//检查满
	if (it_hp->second.iActCount >= bossAct.iLimit)
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ENCOURAGE_LIMIT;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	// 检查成本
	if (bossAct.iConsumeType == 1)
	{
		MasterVariableData mvd = {0};
		pUser->m_userBase.GetMasterData(mvd);
		CT_INT32 iRes = bossAct.iConsumeNum;
		if(( !G_ChangeResType(mvd, bossAct.iConsumeID, iRes) || iRes <0) )
		{
			CMD_G_WorldBossFail worldBossFail = {0};
			worldBossFail.iFailCode = BOSS_NO_DIAMOD;
			pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
			return;
		}
		memset(&mvd, 0, sizeof(MasterVariableData));
		iRes = bossAct.iConsumeNum;
		G_ChangeResType(mvd, bossAct.iConsumeID, iRes);
		pUser->m_userBase.AddMastData(mvd, SAVATAG_BOSS_ENCOURAGE);
	}
	else
	{
		if (LOOT_SUC != pUser->LootItems(SAVATAG_BOSS_ENCOURAGE, &(bossAct.iConsumeID), &(bossAct.iConsumeNum), 1, ITEM_OP_MODE_CKEXE))
		{
			CMD_G_WorldBossFail worldBossFail = {0};
			worldBossFail.iFailCode = BOSS_NO_PROP;
			pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
			return;
		}
	}
		
	//激励成功
	G_BossActEffect actEffect = {0};
	CT_INT32 iActCount = it_hp->second.iActCount + bossAct.iActNum;
	iActCount = iActCount > bossAct.iLimit ? bossAct.iLimit : iActCount;
	G_GetBossActEffect(iActCount, &actEffect);
	CT_INT32 iRand = rand()%100;
	if (bossAct.iSucRule > 0 && bossAct.iSucRule < 4)
	{
		if (actEffect.fSucRate[bossAct.iSucRule-1] * 100 >= iRand)
		{
			it_hp->second.iActCount += bossAct.iActNum;
			it_hp->second.iActCount = iActCount;
		}
		if (iType == 1)
		{
			it_hp->second.iActTime = time(0);
			it_hp->second.bActCoolFlag = CT_FALSE;
		}
	}

	//发送结果
	CMD_G_WorldBossEncourageResult result = {0};
	result.dwUserID = dwUID;
	result.iEncourageCount = it_hp->second.iActCount;
	result.iEncourageType = iType;
	pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_SUC, (CT_CHAR*)&result, sizeof(CMD_G_WorldBossEncourageResult));	
}

CT_VOID CCBossManager::BattleApply(CT_DWORD dwUID, CT_INT32 iFlag)
{
	_AUTOMUTEX(&m_mutex);

	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossManager::BattleApply,can't find user %d",dwUID);
		return;
	}

	if (!m_bActive)		//活动未开启
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ACTIVE_CLOSE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	if (m_iBossHP <= 0)		//BOSS已死亡
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_HAS_DIE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	CT_INT32 iDiamod = 0;
	UserHurtMap::iterator it = m_HPHurtMap.find(dwUID);
	if (it != m_HPHurtMap.end())
	{
		if (!it->second.bBattleFlag)
		{
			if (iFlag == 1)
			{
				G_ValueRange vr = {0};
				if (!G_GetValueRange(VALUE_TYPE_BOSS_BATTLE_DIAMOD, &vr))
				{
					CCLogMeg::getSingleton().WriteError("GetValueRange Failed %d",VALUE_TYPE_BOSS_BATTLE_DIAMOD);
					vr.fMinVal = -10;	//如果未配置,默认每次10钻石
				}
				MasterVariableData mvData = {0};
				pUser->m_userBase.GetMasterData(mvData);
				iDiamod = (CT_INT32)vr.fMinVal;
				if (mvData.stRes.iDiamod + iDiamod < 0)	//钻石不足
				{
					CMD_G_WorldBossFail worldBossFail = {0};
					worldBossFail.iFailCode = BOSS_NO_DIAMOD;
					pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
					return;
				}
// 				memset(&mvData, 0, sizeof(MasterVariableData));  改到战斗返回时扣除
// 				mvData.stRes.iDiamod = iDiamod;
// 				pUser->m_userBase.AddMastData(mvData, SAVATAG_BOSS_BATTLE);
			}
			else
			{
				CMD_G_WorldBossFail worldBossFail = {0};
				worldBossFail.iFailCode = BOSS_BATTLE_NO_COOL;
				pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
				return;
			}
		}
		it->second.iBattleTime = time(0);
		it->second.bBattleFlag = CT_FALSE;
	}

	//如果队列为空,则直接发往战斗线程
	if (m_BattleList.size() == 0)
	{
		MSC_WorldBoss_Fighting fighting = {0};
		G_BossBase gBossBase = {0};
		G_GetBossBase(m_iAEID, &gBossBase);
		fighting.dwUserID = dwUID;
		fighting.iMonsterGroupID = gBossBase.iMonsterGroupID;
		fighting.iSeed=rand()%RAND_MAX_EX;
		G_GetFightingData(pUser, fighting.LeftFightingData);
		pUser->m_userArmy.DeleteBoss();
		UserHurtMap::iterator it_hp = m_HPHurtMap.find(dwUID);		//累计用户伤害血量
		if (it_hp != m_HPHurtMap.end())
		{
			G_BossActEffect actEffect = {0};
			G_GetBossActEffect(it_hp->second.iActCount, &actEffect);
			for (CT_INT32 i=0; i<POST_COUNT / 2; ++i)
			{
				fighting.LeftFightingData.units[i].mcs.iAttack = fighting.LeftFightingData.units[i].mcs.iAttack * (1+actEffect.fAttack);
			}
		}
		CT_DWORD threadid = GAME_THREAD_FIGHT + dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_WORLDBOSS_FINGHTING, (CT_CHAR*)&fighting, sizeof(fighting));
	} 
	BattleUser battleUser = {0};
	battleUser.dwUID = dwUID;
	battleUser.iDiamod = iDiamod;
	m_BattleList.push_back(battleUser);
}

CT_VOID CCBossManager::BattleResult(CT_DWORD dwUID, CT_INT32 iHP)
{
	_AUTOMUTEX(&m_mutex);

	if (iHP > m_iBossHP)
	{
		iHP = m_iBossHP;
	}

	if (m_iBossHP > m_iBossHPMax*0.1 && m_iBossHP-iHP <= m_iBossHPMax*0.1)	//10%伤血公告
	{
		CT_DWORD dwFID = 0;
		CT_CHAR cFName[NAME_SIZE] = {0};;
		RollBroadcast rb[4]={0};
		HPTopMap::iterator it_top = m_HPTopMap.begin();
		if (it_top != m_HPTopMap.end())
		{
			dwFID = it_top->second;
			UserHurtMap::iterator it_h = m_HPHurtMap.find(dwFID);
			if (it_h != m_HPHurtMap.end())
			{
				memcpy(cFName, it_h->second.cUserName, NAME_SIZE);
			}
		}
		
		rb[0].SetValue(dwFID, cFName);
		rb[1].SetValue(m_iAEID, "");
		rb[2].SetValue(m_iAEID, "");
		rb[3].SetValue(m_iAEID, "");
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{78}<1:%d:%s>{79}<16:%d>{80}<16:%d>{81}<16:%d>{82}", dwFID, cFName, m_iAEID, m_iAEID, m_iAEID);
		G_SendRollBroadecast(78,2,1,(CT_CHAR*)rb,sizeof(rb));
	}

	CT_CHAR cLastNickName[NAME_SIZE] = {0};
	UserHurtMap::iterator it_hp = m_HPHurtMap.find(dwUID);		//累计用户伤害血量
	if (it_hp != m_HPHurtMap.end())
	{
		it_hp->second.iHP += iHP;
		memcpy(cLastNickName, it_hp->second.cUserName, NAME_SIZE);
	}

	if (m_iBossHP > 0 && m_iBossHP <= iHP)//BOSS死亡 奖励发放(参与奖,排名奖,最后一击)
	{
		CT_DWORD dwEndFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(dwUID);
		RefCurrTop();
		CT_INT32 iCount = m_HPTopMap.size();
		G_BossReward bossReward = {0};
		CMD_G_WorldBossActiveReward activeReward = {0};
		HPTopMap::iterator it_top = m_HPTopMap.begin();
		HPTopMap::iterator it_top_end = m_HPTopMap.end();
		for (CT_INT32 i=1; it_top != it_top_end; ++it_top, ++i)
		{
			memset(&bossReward, 0, sizeof(G_BossReward));
			memset(&activeReward, 0, sizeof(CMD_G_WorldBossActiveReward));
			G_GetBossReward(1, iCount-i+1, &bossReward);					//排名奖
			activeReward.reward[0].iType = 1;
			activeReward.reward[0].iPropID = bossReward.iPropID;
			activeReward.reward[0].iPropNum = bossReward.iPropNum;
			memset(&bossReward, 0, sizeof(G_BossReward));
			G_GetBossReward(2, 0, &bossReward);					//参与奖
			activeReward.reward[1].iType = 2;
			activeReward.reward[1].iPropID = bossReward.iPropID;
			activeReward.reward[1].iPropNum = bossReward.iPropNum;
			memset(&bossReward, 0, sizeof(G_BossReward));
			if (dwUID == it_top->second || (dwEndFTID != 0 && dwEndFTID == CCFightTeamManager::getSingleton().GetFightTeamIDByUser(it_top->second)))
			{
				G_GetBossReward(3, 0, &bossReward);					//最后一击奖
				activeReward.reward[2].iType = 3;
				activeReward.reward[2].iPropID = bossReward.iPropID;
				activeReward.reward[2].iPropNum = bossReward.iPropNum;
			}
			
			activeReward.dwLastUID = dwUID;
			memcpy(activeReward.cLastNickName, cLastNickName, NAME_SIZE);
			activeReward.iHurtValue = it_top->first;
			activeReward.fHurtPercent = (CT_FLOAT)it_top->first / (CT_FLOAT)m_iBossHPMax;
			m_RewardMap.insert(BossRewardMap::value_type(it_top->second, activeReward));
			CCUser *pUser2 = 0;
			if (CCUserManager::getSingleton().Find(it_top->second,&pUser2))
			{
				pUser2->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD, (CT_CHAR*)&activeReward, sizeof(CMD_G_WorldBossActiveReward));
			}
		}

		MSC_DB_GlobeData gd={0};
		m_iPreFinishTime = time(0) - m_lBeginTime;	//记录使用时间
		g_iGlobeData[GLOBEDATA_TYPE_BOSS_PREFINISH] = m_iPreFinishTime;
		gd.iType=GLOBEDATA_TYPE_BOSS_PREFINISH;
		gd.iValue=m_iPreFinishTime;
		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
		m_iPrePlayer = m_HPHurtMap.size();			//参与人数
		g_iGlobeData[GLOBEDATA_TYPE_BOSS_PREPLAYER] = m_iPrePlayer;
		gd.iType=GLOBEDATA_TYPE_BOSS_PREPLAYER;
		gd.iValue=m_iPrePlayer;
		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
		g_iGlobeData[GLOBEDATA_TYPE_BOSS_GRADE] = m_iBossGrade;
		gd.iType=GLOBEDATA_TYPE_BOSS_GRADE;
		gd.iValue=m_iBossGrade;
		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
		RefTop();
		SaveDB();
		m_iBossHP = 0;
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_DIE);		//通知客户端BOSS已死亡


		CT_CHAR cKName[NAME_SIZE] = {0};
		CT_CHAR cFTName[NAME_SIZE] = {0};
		UserHurtMap::iterator it_h = m_HPHurtMap.find(dwUID);
		if (it_h != m_HPHurtMap.end())
		{
			memcpy(cKName, it_h->second.cUserName, NAME_SIZE);
		}
		if (dwEndFTID == 0)
		{
			RollBroadcast rb[3]={0};
			rb[0].SetValue(dwUID, cKName);
			rb[1].SetValue(m_iAEID, "");
			rb[2].SetValue(m_iAEID, "");
			G_SendRollBroadecast(84,2,1,(CT_CHAR*)rb,sizeof(rb));
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{84}<16:%d>{85}<16:%d>{88}", dwUID, cKName, m_iAEID, m_iAEID);
		}
		else
		{
			RollBroadcast rb[4]={0};
			CCFightTeam *pFightTeam = 0;
			if (CCFightTeamManager::getSingleton().Find(dwEndFTID, &pFightTeam))
			{
				pFightTeam->GetFightTeamName(cFTName);
			}

			rb[0].SetValue(0,cFTName);
			rb[1].SetValue(dwUID, cKName);
			rb[2].SetValue(m_iAEID,"");
			rb[3].SetValue(m_iAEID,cFTName);
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<6:%s>{83}<1:%d:%s>{84}<16:%d>{85}<16:%d>{86}<6:%s>{87}", cFTName,dwUID, cKName, m_iAEID, m_iAEID, cFTName);
			G_SendRollBroadecast(83,2,1,(CT_CHAR*)rb,sizeof(rb));
		}		
		
	}
	else
		m_iBossHP -= iHP;	//扣掉BOSS血量
	
	std::list<BattleUser>::iterator it = m_BattleList.begin();
	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossManager::BattleResult,can't find user %d",dwUID);
	}
	else	//小战斗结算
	{
		MasterVariableData mvData = {0};
		CMD_G_WorldBossBattleReward battleReward = {0};
		battleReward.iHurt = iHP;
		if (it != m_BattleList.end())
		{
			mvData.stRes.iDiamod = it->iDiamod;			//扣除强攻成本
			battleReward.iDiamod = it->iDiamod;
		}
		G_ValueRange vr1 = {0};				//结算战斗奖励
		G_ValueRange vr2 = {0};
		G_ValueRange vr3 = {0};
		G_GetValueRange(VALUE_TYPE_BOSS_REWARD_BASE, &vr1);
		G_GetValueRange(VALUE_TYPE_BOSS_REWARD_GOLD, &vr2);
		G_GetValueRange(VALUE_TYPE_BOSS_REWARD_EXP, &vr3);
		if (vr1.fMinVal ==0 || vr2.fMaxVal ==0 || vr3.fMaxVal ==0 )
		{
			CCLogMeg::getSingleton().WriteError("CCBossManager::VALUE_TYPE: %d:%f,%d:%f,%d:%f",
				VALUE_TYPE_BOSS_REWARD_BASE,vr1.fMinVal,VALUE_TYPE_BOSS_REWARD_GOLD,vr2.fMaxVal,VALUE_TYPE_BOSS_REWARD_EXP,vr3.fMaxVal);
		}
		if (iHP < 500000)
		{
			mvData.stRes.iGold = vr1.fMinVal + iHP  * vr2.fMaxVal;
			mvData.stGrade.iCurrentExp = vr1.fMaxVal + iHP * vr3.fMaxVal;
		}
		else
		{
			mvData.stRes.iGold = vr1.fMinVal + powf((CT_FLOAT)iHP/500000, vr2.fMinVal) * 500000 * vr2.fMaxVal;
			mvData.stGrade.iCurrentExp = vr1.fMaxVal + powf((CT_FLOAT)iHP/500000, vr3.fMinVal) * 500000 * vr3.fMaxVal;
		}
		battleReward.iGold = mvData.stRes.iGold;
		battleReward.iCurrentExp = mvData.stGrade.iCurrentExp;
		battleReward.iBossHP = m_iBossHP + iHP;
		pUser->m_userBase.AddMastData(mvData, SAVATAG_BOSS_BATTLE);
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE_SUC, (CT_CHAR*)&battleReward,sizeof(CMD_G_WorldBossBattleReward));
	}
	m_BattleList.pop_front();	//战斗结果处理完毕

	//继续处理等待队列
	CT_BOOL bLoop = CT_TRUE;
	BattleUser battleUser = {0};
	while (bLoop && m_BattleList.size() != 0)
	{
		it = m_BattleList.begin();
		CCUser *pUser2 = 0;
		if (CCUserManager::getSingleton().Find(it->dwUID,&pUser2))
		{
			if (m_iBossHP > 0)
			{
				bLoop = CT_FALSE;
				MSC_WorldBoss_Fighting fighting = {0};
				G_BossBase gBossBase = {0};
				G_GetBossBase(m_iAEID, &gBossBase);
				fighting.dwUserID = it->dwUID;
				fighting.iMonsterGroupID = gBossBase.iMonsterGroupID;
				G_GetFightingData(pUser2, fighting.LeftFightingData);
				pUser2->m_userArmy.DeleteBoss();
				fighting.iSeed=rand()%RAND_MAX_EX;
				CT_DWORD threadid = GAME_THREAD_FIGHT + dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
				G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_WORLDBOSS_FINGHTING, (CT_CHAR*)&fighting, sizeof(fighting));
			}
			else	//BOSS已经死亡, 剩余队列全部释放
			{
				CMD_G_WorldBossFail worldBossFail = {0};
				worldBossFail.iFailCode = BOSS_HAS_DIE;
				pUser2->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
				m_BattleList.pop_front();
			}
		}
		else
			m_BattleList.pop_front();
	}
}

CT_VOID CCBossManager::SendReward(CT_DWORD dwUID)
{
	_AUTOMUTEX(&m_mutex);
	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossManager::SendReward,can't find user %d",dwUID);
		return;
	}
	BossRewardMap::iterator it = m_RewardMap.find(dwUID);
	if (it != m_RewardMap.end())
	{
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD, (CT_CHAR*)&(it->second), sizeof(CMD_G_WorldBossActiveReward));
	}
}

CT_VOID CCBossManager::RecvReward(CT_DWORD dwUID)
{
	_AUTOMUTEX(&m_mutex);

	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossManager::RecvReward,can't find user %d",dwUID);
		return;
	}

	BossRewardMap::iterator it = m_RewardMap.find(dwUID);
	if (it != m_RewardMap.end())
	{
		CT_INT32 iPropID[3] = {0};
		CT_INT32 iPropNum[3] = {0};
		CT_INT32 iCount = 0;
		for (CT_INT32 i= 0; i<3; ++i)
		{
			if (it->second.reward[i].iPropID > 0)
			{
				iPropID[iCount] = it->second.reward[i].iPropID;
				iPropNum[iCount] = it->second.reward[i].iPropNum;
				iCount++;
			}
		}
		if (iCount > 0)
		{
			if (LOOT_SUC == pUser->LootItems(SAVATAG_BOSS_REWARD, iPropID, iPropNum, iCount, ITEM_OP_MODE_CKEXE))
			{
				pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD_SUC);
				m_RewardMap.erase(dwUID);
			}
			else
			{
				CMD_G_WorldBossFail worldBossFail = {0};
				worldBossFail.iFailCode = BOSS_BAG_FULL;
				pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
				return;
			}
		}
		else		//奖励内容为空
		{
			CMD_G_WorldBossFail worldBossFail = {0};
			worldBossFail.iFailCode = BOSS_REWARD_INVALID;
			pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		}
		m_RewardMap.erase(dwUID);
	}
	else		//奖励不存在
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_REWARD_INVALID;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
	}
}

CT_VOID CCBossManager::UserLogin(CT_DWORD dwUID)
{
	_AUTOMUTEX(&m_mutex);

	if (m_iBUffID > 0)
	{
		CCUser *pUser = 0;
		if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
		{
			CCLogMeg::getSingleton().WriteError("CCBossManager::UserLogin,can't find user %d",dwUID);
			return;
		}
		pUser->m_userBuff.BuffReward(m_iBUffID, m_lEndTime);
	}
}

CT_VOID CCBossManager::InitBossHurt(MSC_DB_WorldBossTop &dbBossTop)
{
	_AUTOMUTEX(&m_mutex);

	BossHPHurt bossHPHurt = {0};
	BossHurtMap::iterator it_boss = m_BossHurtMap.find(dbBossTop.iAEID);
	if (it_boss == m_BossHurtMap.end())
	{
		TopHurtMap topHurtMap;
		bossHPHurt.dwUID = dbBossTop.dwUID;
		memcpy(bossHPHurt.cUserName, dbBossTop.cUserName, NAME_SIZE);
		bossHPHurt.iHP = dbBossTop.iHP;
		bossHPHurt.fHurtPercent = dbBossTop.fHurtPercent;
		topHurtMap.insert(TopHurtMap::value_type(dbBossTop.iTop,bossHPHurt));
		m_BossHurtMap.insert(BossHurtMap::value_type(dbBossTop.iAEID,topHurtMap));
	}
	else
	{
		bossHPHurt.dwUID = dbBossTop.dwUID;
		memcpy(bossHPHurt.cUserName, dbBossTop.cUserName, NAME_SIZE);
		bossHPHurt.iHP = dbBossTop.iHP;
		bossHPHurt.fHurtPercent = dbBossTop.fHurtPercent;
		it_boss->second.insert(TopHurtMap::value_type(dbBossTop.iTop,bossHPHurt));
	}
}

//替换对应排行榜
CT_VOID CCBossManager::RefTop()
{
	_AUTOMUTEX(&m_mutex);
	
	BossHPHurt bossHPHurt = {0};
	BossHurtMap::iterator it_boss = m_BossHurtMap.find(m_iAEID);
	CT_INT32 i=0;
	CT_INT32 iCount = m_HPTopMap.size();
	HPTopMap::iterator it_top = m_HPTopMap.begin();
	HPTopMap::iterator it_top_end = m_HPTopMap.end();
	if (it_boss == m_BossHurtMap.end())
	{
		TopHurtMap topHurtMap;
		for (; it_top!=it_top_end; ++i,++it_top)
		{
			bossHPHurt.dwUID = it_top->second;
			UserHurtMap::iterator it_h = m_HPHurtMap.find(bossHPHurt.dwUID);
			if (it_h != m_HPHurtMap.end())
			{
				memcpy(bossHPHurt.cUserName, it_h->second.cUserName, NAME_SIZE);
			}
			bossHPHurt.iHP = it_top->first;
			bossHPHurt.fHurtPercent = (CT_FLOAT)bossHPHurt.iHP / (CT_FLOAT)m_iBossHPMax;
			topHurtMap.insert(TopHurtMap::value_type(iCount-i,bossHPHurt));
		}
		m_BossHurtMap.insert(BossHurtMap::value_type(m_iAEID,topHurtMap));
	}
	else
	{
		it_boss->second.clear();
		for (; it_top!=it_top_end; ++i,++it_top)
		{
			bossHPHurt.dwUID = it_top->second;
			UserHurtMap::iterator it_h = m_HPHurtMap.find(bossHPHurt.dwUID);
			if (it_h != m_HPHurtMap.end())
			{
				memcpy(bossHPHurt.cUserName, it_h->second.cUserName, NAME_SIZE);
			}
			bossHPHurt.iHP = it_top->first;
			bossHPHurt.fHurtPercent = (CT_FLOAT)bossHPHurt.iHP / (CT_FLOAT)m_iBossHPMax;
			it_boss->second.insert(TopHurtMap::value_type(iCount-i,bossHPHurt));
		}
	}
}

CT_VOID CCBossManager::SaveDB()
{
	_AUTOMUTEX(&m_mutex);
	MSC_DB_WorldBossTopClear topClear={0};
	topClear.iAEID = m_iAEID;
	CT_DWORD threadid = GAME_THREAD_DATABASE + m_iAEID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_WORLDBOSS,MSC_SUB_DB_BOSS_TOPCLEAR,(CT_CHAR*)&topClear,sizeof(MSC_DB_WorldBossTopClear));

	MSC_DB_WorldBossTop topAdd={0};
	topAdd.iAEID = m_iAEID;
	CT_INT32 iCount = m_HPTopMap.size();
	HPTopMap::iterator it_top = m_HPTopMap.begin();
	HPTopMap::iterator it_top_end = m_HPTopMap.end();
	for (CT_INT32 i=0; it_top!=it_top_end; ++it_top, ++i)
	{
		topAdd.dwUID = it_top->second;
		UserHurtMap::iterator it_h = m_HPHurtMap.find(topAdd.dwUID);
		if (it_h != m_HPHurtMap.end())
		{
			memcpy(topAdd.cUserName, it_h->second.cUserName, NAME_SIZE);
		}
		topAdd.iTop = iCount - i;
		topAdd.iHP = it_top->first;
		topAdd.fHurtPercent = (CT_FLOAT)topAdd.iHP / (CT_FLOAT)m_iBossHPMax;

		CT_DWORD threadid = GAME_THREAD_DATABASE + topAdd.dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_WORLDBOSS,MSC_SUB_DB_BOSS_TOPADD,(CT_CHAR*)&topAdd,sizeof(MSC_DB_WorldBossTop));
	}
}
