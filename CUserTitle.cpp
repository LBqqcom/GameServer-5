#include "CUserTitle.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CNetInterface.h"
#include "CThreadMes.h"
#include "CBroadcastScriptSetting.h"
#include "CTitle.h"

CT_INT32 CCUserTitle::transData(const TitleData& src, CT_CHAR* pDst) const
{
	CT_INT32 iCurrent = time(0);
	CMD_G_TitleData* pst = (CMD_G_TitleData*)pDst;
	pst->iTitleID = src.iID;
	pst->bIsUse = src.bIsUse;
	if (TITLE_EFFECT_FOREVER == src.iEffectiveTime)
	{
		pst->iEffectiveTime = TITLE_EFFECT_FOREVER;
	}
	else
	{
		pst->iEffectiveTime = src.iEffectiveTime - (iCurrent - src.iActiveTime);
	}
	
	return sizeof(*pst);
}

CT_INT32 CCUserTitle::ActiveTitle(CT_INT32 iTitleItemID)
{
	_AUTOMUTEX(m_pMutex);
	G_ItemTitle git = {0};
	TitleData td = {0};
	CT_INT32 iCurrent = time(0);

	if (!G_GetItemTitle(iTitleItemID, &git))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTitle::ActiveTitle G_GetItemTitle data error.itemId:%d", iTitleItemID);
		return EUI_DATA_ERROR;
	}

	td.iID = git.iTitleID;
	CCUserTitle::DataMap_IT it = m_DataMap.find(git.iTitleID);
	// 若头衔已激活，则延长有效时间
	if (it != m_DataMap.end())
	{
		if (it->second.iEffectiveTime != TITLE_EFFECT_FOREVER)
		{
			it->second.iEffectiveTime += git.iEffectiveTime;
		}
		td.iActiveTime = it->second.iActiveTime;
		td.iEffectiveTime = it->second.iEffectiveTime;
	}
	else
	{
		// 激活头衔
		td.iEffectiveTime = git.iEffectiveTime;
		td.iActiveTime = time(0);
		m_pUser->m_userTitle.InsertData(td.iID, td);
	}

	// 发送客户端
	CMD_G_ActiveTitle gat = {0};
	gat.iTitleID = td.iID;
	if (TITLE_EFFECT_FOREVER == td.iEffectiveTime)
	{
		gat.iLeftTime = TITLE_EFFECT_FOREVER;
	}
	else
	{
		gat.iLeftTime = td.iEffectiveTime - (iCurrent - td.iActiveTime);
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ACTIVE_TITLE, (CT_CHAR *)&gat, sizeof(gat), false, true);

	// 播放公告
	if (TITLE_POTHOLING_EXPERT == iTitleItemID || TITLE_LUCKY_SHIELD == iTitleItemID)
	{
		RollBroadcast rb={0};
		MasterData md = {0};
		m_pUser->m_userBase.GetMasterData(md);
		CT_INT32 iIndex = iTitleItemID - TITLE_POTHOLING_EXPERT;
		rb.SetValue(m_pUser->GetUserID(), md.cNickName);
		G_SendRollBroadecast(97+iIndex,3+iIndex,1,(CT_CHAR*)&rb,sizeof(rb));
	}

	// 更新数据库
	MSC_DB_TitleData DIT={0};
	DIT.dwUserID = m_pUser->GetUserID();
	DIT.iTitleID = td.iID;
	DIT.iActiveTime = td.iActiveTime;
	DIT.iEffectTime = git.iEffectiveTime;
	DIT.iStatus = TITLE_ACTIVE;
	CT_INT32 threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_TITLE_DATA, (CT_CHAR*)&DIT, sizeof(DIT));

	// 更新数据库日志
	MSC_LOGDB_Title LDT = {0};
	LDT.dwUserID = m_pUser->GetUserID();
	LDT.iTitleID = td.iID;
	LDT.iStatus = TITLE_ACTIVE;
	LDT.iLastTime = gat.iLeftTime;
	CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TITLE,(CT_CHAR*)&LDT,sizeof(LDT));
	return EUI_SUCCESS;
}

CT_INT32 CCUserTitle::UseTitle(CT_INT32 iTitleID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iCurrent = time(0);
	CT_INT32 iPreUseTitleID = 0;
	G_MasterTitle gmt = {0};
	CMD_G_TitleUseSuc tus = {0};
	MSC_DB_TitleData DIT={0};
	MSC_LOGDB_Title LDT = {0};

	if (!G_GetMasterTitle(iTitleID, &gmt))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTitle::UseTitle G_GetMasterTitle data error. titleid:%d", iTitleID);
		return EUT_DATA_ERROR;
	}

	// 查看该头衔是否在激活状态
	DataMap_IT it = m_DataMap.find(iTitleID);
	if (it == m_DataMap.end())
	{
		CCLogMeg::getSingleton().WriteError("CCUserTitle::UseTitle titleId error.User:%d,title:%d", m_pUser->GetUserID(), iTitleID);
		return EUT_NOT_ACTIVE;
	}

	// 判断是否有已在使用的头衔，若有则取消原有的
	iPreUseTitleID = GetUseTitleID();
	if (iPreUseTitleID != 0 && iPreUseTitleID != iTitleID)
	{
		DataMap_IT preIt = m_DataMap.find(iPreUseTitleID);
		preIt->second.bIsUse = CT_FALSE;

		// 更新数据库
		MSC_DB_TitleData preDIT={0};
		preDIT.dwUserID = m_pUser->GetUserID();
		preDIT.iTitleID = iPreUseTitleID;
		preDIT.iActiveTime = preIt->second.iActiveTime;
		preDIT.iEffectTime = 0;
		preDIT.iStatus = TITLE_ACTIVE;
		CT_INT32 threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_TITLE_DATA, (CT_CHAR*)&preDIT, sizeof(preDIT));
	}

	// 如果头衔已使用,则取消使用
	if (it->second.bIsUse)
	{
		it->second.bIsUse = false;
		tus.iType = 0;
		DIT.iStatus = TITLE_ACTIVE;
		LDT.iStatus = TITLE_USE_CANCEL;
	}
	else
	{
		it->second.bIsUse = true;
		tus.iType = 1;
		DIT.iStatus = TITLE_USE;
		LDT.iStatus = TITLE_USE;
	}

	// 发送客户端
	tus.iTitleID = iTitleID;
	if (TITLE_EFFECT_FOREVER == it->second.iEffectiveTime)
	{
		tus.iLeftTime = TITLE_EFFECT_FOREVER;
	}
	else
	{
		tus.iLeftTime = it->second.iEffectiveTime - (iCurrent - it->second.iActiveTime);
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_TITLE_USE_SUC, (CT_CHAR*)&tus, sizeof(tus), false, true);

	// 更新领主战斗力
	m_pUser->UpdateAndSendMasterData(EMI_BATLLE);

	// 更新战队成员使用头衔的状态
	MasterVariableData mv = {0};
	MSC_DB_FTMemeberRefresh ftmMemRefresh = {0};
	m_pUser->m_userBase.GetMasterData(mv);
	ftmMemRefresh.dwUser = m_pUser->GetUserID();
	ftmMemRefresh.iGrade = mv.stGrade.iLevel;
	ftmMemRefresh.iFightingPower = mv.stRes.iFightingPower;
	CT_DWORD gThreadID = GAME_THREAD_CITY +  m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
	G_SendThreadInfo(gThreadID,   MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_DB_FIGHTTEAM_MEM_REFRESH, (CT_CHAR*)&ftmMemRefresh, sizeof(ftmMemRefresh));

	// 更新数据库
	DIT.dwUserID = m_pUser->GetUserID();
	DIT.iTitleID = it->second.iID;
	DIT.iActiveTime = it->second.iActiveTime;
	DIT.iEffectTime = 0;
	CT_INT32 threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_TITLE_DATA, (CT_CHAR*)&DIT, sizeof(DIT));

	// 更新数据库日志
	LDT.dwUserID = m_pUser->GetUserID();
	LDT.iTitleID = iTitleID;
	LDT.iLastTime = tus.iLeftTime;
	CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TITLE,(CT_CHAR*)&LDT,sizeof(LDT));

	return EUT_USETITLE_SUC;
}

CT_VOID CCUserTitle::UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserTitle::DataMap &titleMap)
{
	G_MasterTitle gmt = {0};
	CT_INT32 iCurrentTime = time(0);
	CCUserTitle::DataMap_CONSTIT it = titleMap.begin();

	// 判断是否有使用中的头衔
	for (;it != titleMap.end();++it)
	{
		if (it->first != 0 && it->second.bIsUse && 
			(it->second.iActiveTime + it->second.iEffectiveTime > iCurrentTime || it->second.iEffectiveTime == TITLE_EFFECT_FOREVER))
		{
			break;
		}
	}
	if (it == titleMap.end())
	{
		return;
	}

	if (!G_GetMasterTitle(it->first, &gmt))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTitle::UpdateBattleStatus G_GetMasterTitle data error.titleId:%d", it->first);
		return;
	}

	mcs.iMaxHP         += gmt.iHealth;
	mcs.iStrength      += gmt.iStrength;
	mcs.iAgility       += gmt.iAgility;
	mcs.iIntellect     += gmt.iIntellect;
	mcs.iSpirit        += gmt.iSpirit;
	mcs.fHitRate       += gmt.fHitRate;
	mcs.fDodgeRate     += gmt.fDodgeRate;
	mcs.fCritRate      += gmt.fCritRate;
	mcs.fResilient     += gmt.fResilient;
	mcs.fHPPersent     += gmt.fHealthPercent;
	mcs.iAttack		   += gmt.iAttack;
	mcs.iDefence	   += gmt.iDefence;
	mcs.fDmgIncPercent += gmt.fDmgIncPercent;
	mcs.fDmgDecPercent += gmt.fDmgDecPercent;
}

CT_VOID CCUserTitle::UpdataTitleStatus()
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iCurrentTime = time(0);

	for (DataMap_IT it = m_DataMap.begin();it != m_DataMap.end();)
	{
		// 已激活的头衔到期
		if (it->second.iEffectiveTime != TITLE_EFFECT_FOREVER && it->second.iActiveTime + it->second.iEffectiveTime <= iCurrentTime)
		{
			CMD_G_TitleToTime gtt = {0};

			// 使用的头衔到期
			if (it->second.bIsUse)
			{
				it->second.bIsUse = CT_FALSE;
				printf("%d",iCurrentTime);
				gtt.bIsUse = CT_TRUE;
			}
			
			// 发送客户端
			gtt.dwUserID = m_pUser->GetUserID();
			gtt.iTitleID = it->first;
			m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_TITLE_TO_TIME, (CT_CHAR*)&gtt, sizeof(gtt), false, true);

			if (gtt.bIsUse = CT_TRUE)
			{
				m_pUser->UpdateAndSendMasterData(EMI_BATLLE);

				MasterVariableData mv = {0};
				MSC_DB_FTMemeberRefresh ftmMemRefresh = {0};
				m_pUser->m_userBase.GetMasterData(mv);
				ftmMemRefresh.dwUser = m_pUser->GetUserID();
				ftmMemRefresh.iGrade = mv.stGrade.iLevel;
				ftmMemRefresh.iFightingPower = mv.stRes.iFightingPower;
				CT_DWORD gThreadID = GAME_THREAD_CITY +  m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
				G_SendThreadInfo(gThreadID,   MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_DB_FIGHTTEAM_MEM_REFRESH, (CT_CHAR*)&ftmMemRefresh, sizeof(ftmMemRefresh));
			}

			// 更新数据库
			MSC_DB_TitleData DIT={0};
			DIT.dwUserID = m_pUser->GetUserID();
			DIT.iTitleID = it->second.iID;
			DIT.iActiveTime = it->second.iActiveTime;
			DIT.iEffectTime = it->second.iEffectiveTime;
			DIT.iStatus = TITLE_NOT_ACTIVE;
			CT_INT32 threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_TITLE_DATA, (CT_CHAR*)&DIT, sizeof(DIT));

			// 更新数据库日志
			MSC_LOGDB_Title LDT = {0};
			LDT.dwUserID = m_pUser->GetUserID();
			LDT.iTitleID = it->second.iID;
			LDT.iStatus = TITLE_USE_CANCEL;
			CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TITLE,(CT_CHAR*)&LDT,sizeof(LDT));

			DataMap_IT itTmp = it;
			++it;
			m_DataMap.erase(itTmp->first);
		}
		else
		{
			++it;
		}
	}
}

CT_INT32 CCUserTitle::GetUseTitleID()
{
	_AUTOMUTEX(m_pMutex);
	DataMap_IT it = m_DataMap.begin();
	for (;it != m_DataMap.end();++it)
	{
		if (it->second.bIsUse)
		{
			return it->first;
		}
	}

	return 0;
}

CT_INT32 CCUserTitle::GetTitleList(CT_CHAR* pBuf, CT_INT32& iLen)
{
	_AUTOMUTEX(m_pMutex);
	std::map<CT_INT32,CT_INT32> titleMap;
	std::map<CT_INT32,CT_INT32>::iterator titleMap_it;

	DataMap_IT it = m_DataMap.begin();
	for (;it != m_DataMap.end();++it)
	{
		titleMap[it->first] = it->first;
	}

	CT_INT32 buf[200] = {0};
	CT_INT32 j = 0;
	CT_INT32 iNum = CCTitle::getSingleton().GetTitleList((CT_CHAR*)buf, sizeof(buf));
	CMD_G_TitleList gtl = {0};
	for (CT_INT32 i = 0;i <iNum;++i)
	{
		titleMap[buf[i]] = buf[i];
	}

	if (iLen < titleMap.size() * sizeof(int))
	{
		return 0;
	}
	for (titleMap_it = titleMap.begin();titleMap_it != titleMap.end();++titleMap_it,++j)
	{
		gtl.iTitle[j] = titleMap_it->first;
	}
	memcpy(pBuf, &gtl, sizeof(gtl));
	iLen = sizeof(gtl);
	return 1;

}

CT_VOID CCUserTitle::UpdataTitleList()
{
	_AUTOMUTEX(m_pMutex);
	CT_CHAR buf[2048] = {0};
	CT_INT32 iLen = sizeof(buf);
	if (GetTitleList(buf, iLen))
	{
		// 推送客户端
		m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_TITLE_LIST, buf, iLen);
	}
}