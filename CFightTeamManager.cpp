#include "CFightTeamManager.h"
#include "CConfig.h"
#include "CLogMeg.h"
#include "CBroadcastScriptSetting.h"
#include "CThreadMes.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "CUserManager.h"

template<> CCFightTeamManager* CCSingleton<CCFightTeamManager>::ms_Singleton=0;

CCFightTeamManager::CCFightTeamManager(CT_VOID)
{
}

CCFightTeamManager::~CCFightTeamManager(CT_VOID)
{
}


//查找数据
CT_BOOL CCFightTeamManager::Find(CT_DWORD id)
{
	_AUTOMUTEX(&m_mutex);
	FTM_IT it = m_fightTeamMap.find(id);
	if (it != m_fightTeamMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//通过ID查找
CT_BOOL CCFightTeamManager::Find(CT_DWORD id, CCFightTeam** ft)
{
	_AUTOMUTEX(&m_mutex);
	FTM_IT it = m_fightTeamMap.find(id);
	if(it!=m_fightTeamMap.end())
	{
		(*ft) = &(it->second);
		return CT_TRUE;
	}
	return CT_FALSE;
}


//通过昵称查找
CT_BOOL CCFightTeamManager::Find(std::string cFightTeamName, CCFightTeam** ft)
{
	_AUTOMUTEX(&m_mutex);
	FTNM_IT it = m_fightTeamNameMap.find(cFightTeamName);
	if(it!=m_fightTeamNameMap.end())
	{
		CT_DWORD dwFTID = (*it).second;
		FTM_IT itm = m_fightTeamMap.find(dwFTID);
		if (itm != m_fightTeamMap.end())
		{
			(*ft) = &(itm->second);
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//清除数据
CT_VOID CCFightTeamManager::Erase(CT_DWORD id)
{
	_AUTOMUTEX(&m_mutex);
	CCFightTeam*         pFightTeam = 0;
	std::string fightTeamName = "";
	{
		FTM_IT it = m_fightTeamMap.find(id);
		if (it != m_fightTeamMap.end())
		{
			fightTeamName = (*it).second.m_stBase.cFightTeamName;
			m_fightTeamMap.erase(it);
		}
	}

	{
		FTNM_IT  it = m_fightTeamNameMap.find(fightTeamName);
		if (it != m_fightTeamNameMap.end())
		{
			m_fightTeamNameMap.erase(it);
		}
	}
}


//清空
CT_VOID CCFightTeamManager::Clear()
{
	_AUTOMUTEX(&m_mutex);

	m_fightTeamMap.clear();

	m_fightTeamNameMap.clear();
}

CT_DWORD CCFightTeamManager::NewFightTeam(CCUser *pUser, CT_CHAR* pFightTeamName, CT_CHAR* pDescription,G_FightTeamUpgrade &ftug)
{
	_AUTOMUTEX(&m_mutex);

	CT_DWORD dwFTID = 0;

	if (m_fightTeamMap.empty())
	{
		dwFTID = 2050001;
	}
	else
	{
		FTM_IT it = m_fightTeamMap.end();
		--it;
		dwFTID = it->first + 1;
	}

	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);	//发送自己的信息给对方
	pUser->m_userBase.SetFightTeamID(dwFTID);

	CCFightTeam fightTeam;
	CT_DWORD dwUserID = pUser->GetUserID();
	fightTeam.m_stBase.dwFTID = dwFTID;
	fightTeam.m_stBase.iGrade = 1;
	memcpy(fightTeam.m_stBase.cFightTeamName, pFightTeamName, NAME_SIZE);
	fightTeam.m_stBase.dwBossUID = dwUserID;
	memcpy(fightTeam.m_stBase.cBossName, masterData.cNickName, NAME_SIZE);
	fightTeam.m_stBase.iFightingPower = masterData.mvd.stRes.iFightingPower;
	fightTeam.m_stBase.dwElite[0] = dwUserID;
	memcpy(fightTeam.m_stBase.cDescription, pDescription, DESCRIPTION_SIZE);
//	fightTeam.iTop;							// 排名
//	fightTeam.iResource;						// 资源数
	
	FightTeamMember fightTeamMember = {0};
	fightTeamMember.dwFTID = dwFTID;
	fightTeamMember.dwUserID = dwUserID;
	memcpy(fightTeamMember.cNickName, masterData.cNickName, NAME_SIZE);
	fightTeamMember.iFightingPower	= masterData.mvd.stRes.iFightingPower;
	fightTeamMember.iOccupation		= masterData.iOccupation;
	fightTeamMember.iGrade			= masterData.mvd.stGrade.iLevel;
	fightTeamMember.iRank			= masterData.mvd.stRank.iLevel;
	fightTeamMember.iTime			= time(0);
	fightTeamMember.iTowerCPID		= pUser->m_userTower.GetTowerMaxCP();
	fightTeamMember.iStatus			= 2;
	fightTeamMember.iTitleID		= pUser->m_userTitle.GetUseTitleID();
//	fightTeamMember.iContribution;					// 贡献
	UserFightingStatus userFightingData = {0};
	G_GetFightingData(pUser, userFightingData);
	//fightTeam.m_FTMemberFightingData.insert(FTMemberFightingData::value_type(dwUserID, userFightingData));
	fightTeam.m_FTMemberMap.insert(FTMember::value_type(dwUserID, fightTeamMember));
	m_fightTeamMap.insert(FightTeamMap::value_type(dwFTID, fightTeam));
	m_fightTeamNameMap.insert(FightTeamNameMap::value_type(pFightTeamName, dwFTID));
	m_fightTeamUserMap.insert(FightTeamUserMap::value_type(dwUserID, dwFTID));
	fightTeam.Clear();

	//用户战斗数据发向数据库线程进行查询
	CT_DWORD dbThreadID = GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_FTFightingData, (CT_CHAR*)&(dwUserID), sizeof(CT_DWORD));

	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	dbFightTeamBase.dwFTID = dwFTID;
	memcpy(dbFightTeamBase.cFightTeamName, pFightTeamName, NAME_SIZE);
	memcpy(dbFightTeamBase.cDescription, pDescription, DESCRIPTION_SIZE);
	dbFightTeamBase.dwBossUID = dwUserID;
	memcpy(dbFightTeamBase.cBossName, fightTeam.m_stBase.cBossName, NAME_SIZE);
	dbFightTeamBase.iFightingPower = fightTeam.m_stBase.iFightingPower;
	dbFightTeamBase.dwElite1 = fightTeam.m_stBase.dwElite[0];
	dbFightTeamBase.iGrade=1;
	CT_DWORD threadID = GAME_THREAD_DATABASE + dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	MSC_DB_FightTeamRecruit fightTeamRecruit = {0};
	fightTeamRecruit.dwFTID = dwFTID;
	fightTeamRecruit.iBeginTime = time(0);
	fightTeamRecruit.iLastingTime = 3600*24;
	fightTeamRecruit.iAuto = 1;
	//memcpy(fightTeamRecruit.cRecruit, pFightTeamRecruit->cAnnounce, DESCRIPTION_SIZE); 自动招募信息配置在客户端
	CCFightTeamManager::getSingleton().InsertFightTeamRecruit(fightTeamRecruit);
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_RECRUIT_UPDATE,(CT_CHAR*)&fightTeamRecruit,sizeof(MSC_DB_FightTeamRecruit));

	MSC_DB_FightTeamMember dbFightTeamMember = {0};
	dbFightTeamMember.dwFTID = dwFTID;
	dbFightTeamMember.dwUserID	= dwUserID;
	memcpy(dbFightTeamMember.cNickName, fightTeamMember.cNickName, NAME_SIZE);
	dbFightTeamMember.iFightingPower = fightTeamMember.iFightingPower;
	dbFightTeamMember.iOccupation = fightTeamMember.iOccupation;
	dbFightTeamMember.iGrade = fightTeamMember.iGrade;
	dbFightTeamMember.iRank = fightTeamMember.iRank;
	dbFightTeamMember.iTowerCPID = fightTeamMember.iTowerCPID;
	dbFightTeamMember.iType = 1;							// 成员类型 1:成员 2:申请待审核 3:邀请待答复
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));

	RollBroadcast rb[2]={0};
	rb[0].SetValue(dwUserID, masterData.cNickName);
	rb[1].SetValue(0, pFightTeamName);
	//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{19}<6:%s>{20}", dwUserID, masterData.cNickName, pFightTeamName);
	G_SendRollBroadecast(19,3,1,(CT_CHAR*)rb,sizeof(rb));

	return dwFTID;
}

// 删除战队
CT_VOID CCFightTeamManager::DeleteFightTeam(CT_DWORD dwFTID)
{
	_AUTOMUTEX(&m_mutex);
	FTM_IT it = m_fightTeamMap.find(dwFTID);
	if (it == m_fightTeamMap.end())
	{
		return;
	}

	// 清除 m_fightTeamUserMap
	FightTeamMemberUID fightTeamMemberUID = {0};
	it->second.GetFightTeamMemberUID(&fightTeamMemberUID);
	for (CT_DWORD n = 0; n < fightTeamMemberUID.dwCount; ++n)
	{
		m_fightTeamUserMap.erase(fightTeamMemberUID.dwMemberUID[n]);
	}

	// 清除 m_fightTeamNameMap
	CT_CHAR cFTName[NAME_SIZE] = {0};
	it->second.GetFightTeamName(cFTName);
	m_fightTeamNameMap.erase(cFTName);

	//清除 m_fightTeamRecruitMap
	m_fightTeamRecruitMap.erase(it->first);

	// 最后清除 m_fightTeamMap
	m_fightTeamMap.erase(it->first);

	CT_DWORD threadID = GAME_THREAD_DATABASE + dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_DELETE,(CT_CHAR*)&dwFTID,sizeof(CT_DWORD));
}

 //任何人不得改动这个函数的实现（会产生死锁问题） 彭文
CT_DWORD CCFightTeamManager::GetFightTeamIDByUser(CT_DWORD dwUser) 
{
	_AUTOMUTEX(&m_mutex);
	std::map<CT_DWORD, CT_DWORD>::iterator it = m_fightTeamUserMap.find(dwUser);
	if (it != m_fightTeamUserMap.end())
	{
		return it->second;
	}
	return 0;
}

CT_VOID CCFightTeamManager::ClearReqRsp(CT_DWORD dwUser)
{
	_AUTOMUTEX(&m_mutex);
	FightTeamMap::iterator it;
	for (it = m_fightTeamMap.begin(); it != m_fightTeamMap.end(); ++it)
	{
		it->second.ClearReqRsp(dwUser);
	}
}
/*
CT_BOOL CCFightTeamManager::GetFightTeamBase(CT_DWORD dwUser, CMD_G_FightTeamBase *pFightTeamBase, CT_INT32 iResCount)
{
	_AUTOMUTEX(&m_mutex);
	CT_DWORD dwFTID = GetFightTeamIDByUser(dwUser);
	if (dwFTID > 1)
	{
		FTM_IT it = m_fightTeamMap.find(dwFTID);
		if (it != m_fightTeamMap.end())
		{
			it->second.GetFightTeamBase(pFightTeamBase, iResCount);
			return CT_TRUE;
		}
	}
	return CT_FALSE;
} */

CT_VOID CCFightTeamManager::BrowseFightTeam(CT_CHAR *out_buf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	FTM_IT it;
	FTM_IT it_end = m_fightTeamMap.end();

	CT_INT32 n = 0;
	CMD_G_FightTeamInfo	fightTeamInfo = {0};

	for (std::map<CT_INT32,CT_DWORD>::iterator it2 = m_ftRankingMap.begin(); it2 != m_ftRankingMap.end() && n < 50; ++it2)
	{
		it = m_fightTeamMap.find(it2->second);
		if (it != m_fightTeamMap.end())
		{
			it->second.GetFightTeamInfo(&fightTeamInfo);
			memcpy(out_buf + (sizeof(CMD_G_FightTeamInfo) * n), (CT_CHAR*)&fightTeamInfo, sizeof(CMD_G_FightTeamInfo));
			n++;
		}
	}

	it = m_fightTeamMap.begin();
	for (; it!=it_end && n < 50; ++it)
	{
		if (m_ftRankingMap.find(it->second.m_stBase.iTop) != m_ftRankingMap.end())
		{
			continue;
		}
		it->second.GetFightTeamInfo(&fightTeamInfo);
		memcpy(out_buf + (sizeof(CMD_G_FightTeamInfo) * n), (CT_CHAR*)&fightTeamInfo, sizeof(CMD_G_FightTeamInfo));
		n++;
	}
	bufLen = sizeof(CMD_G_FightTeamInfo) * n;
}

CT_VOID CCFightTeamManager::BrowseFightTeamRecruit(CT_CHAR *out_buf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	FightTeamRecruitMap::iterator it = m_fightTeamRecruitMap.begin();
	FightTeamRecruitMap::iterator it_end = m_fightTeamRecruitMap.end();
	FTM_IT it_ft;

	CT_INT32 n = 0;
	CMD_G_FightTeamInfo	fightTeamInfo = {0};
	CMD_G_FightTeamRecruit fightTeamRecruit = {0};

	for (; it!=it_end && n < 50; ++it)
	{	
		it_ft = m_fightTeamMap.find(it->first);
		if (it_ft != m_fightTeamMap.end())
		{
			it_ft->second.GetFightTeamInfo(&fightTeamInfo);
			memcpy(&fightTeamRecruit, &fightTeamInfo, sizeof(CMD_G_FightTeamInfo));
			memcpy(fightTeamRecruit.cRecruit, it->second.cRecruit, DESCRIPTION_SIZE);
			fightTeamRecruit.iAuto = it->second.iAuto;
			memcpy(out_buf + (sizeof(CMD_G_FightTeamRecruit) * n), (CT_CHAR*)&fightTeamRecruit, sizeof(CMD_G_FightTeamRecruit));
			n++;
		}
	}
	bufLen = sizeof(CMD_G_FightTeamRecruit) * n;
}

// 获取邀请列表
CT_VOID CCFightTeamManager::GetFightTeamInviteList(CT_DWORD dwUser, CT_CHAR *pBuf, CT_DWORD &bufLen)
{
	_AUTOMUTEX(&m_mutex);

	FTM_IT it = m_fightTeamMap.begin();
	FTM_IT it_end = m_fightTeamMap.end();

	CT_INT32 n = 0;
	CMD_G_FightTeamInfo	fightTeamInfo = {0};

	for (; it!=it_end && n < 50; ++it)
	{	
		if (it->second.m_FTMemberRspMap.find(dwUser) == it->second.m_FTMemberRspMap.end())
		{
			continue;
		}
		it->second.GetFightTeamInfo(&fightTeamInfo);
		memcpy(pBuf + (sizeof(CMD_G_FightTeamInfo) * n), (CT_CHAR*)&fightTeamInfo, sizeof(CMD_G_FightTeamInfo));
		n++;
	}
	bufLen = sizeof(CMD_G_FightTeamInfo) * n;
}

CT_VOID CCFightTeamManager::CheckFightTeamRecruit()
{
	_AUTOMUTEX(&m_mutex);
	std::list<CT_DWORD> listRecruitID;
	CT_INT64 iNow = time(0);
	for (FightTeamRecruitMap::iterator it = m_fightTeamRecruitMap.begin(); it != m_fightTeamRecruitMap.end(); ++it)
	{
		if (it->second.iBeginTime + it->second.iLastingTime <= iNow)
		{
			listRecruitID.push_back(it->first);
			CT_DWORD threadID = GAME_THREAD_DATABASE + it->first % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_RECRUIT_DELETE,(CT_CHAR*)&(it->first),sizeof(CT_DWORD));
			continue;
		}

		if (it->second.iAuto == 1)
		{
			FightTeamMap::iterator it_ft = m_fightTeamMap.find(it->first);
			if (it_ft != m_fightTeamMap.end())
			{
				if (it_ft->second.CheckFightTeamFull())
				{
					listRecruitID.push_back(it->first);
					CT_DWORD threadID = GAME_THREAD_DATABASE + it->first % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
					G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_RECRUIT_DELETE,(CT_CHAR*)&(it->first),sizeof(CT_DWORD));
					continue;
				}
			}
		}
	}

	for (std::list<CT_DWORD>::iterator it2 = listRecruitID.begin(); it2 != listRecruitID.end(); ++it2)
	{
		m_fightTeamRecruitMap.erase(*it2);
	}
}


CT_VOID CCFightTeamManager::DBUpdataData()
{
	_AUTOMUTEX(&m_mutex);

	CT_INT64 lTime=time(0);
	FightTeamMap::iterator it;
	for (it = m_fightTeamMap.begin(); it != m_fightTeamMap.end(); ++it)
	{
		it->second.UpdateData(lTime);
	}
}

CT_VOID	CCFightTeamManager::InitFightTeam(const MSC_DB_FightTeamBase &dbFightTeamBase)
{
	_AUTOMUTEX(&m_mutex);
	CCFightTeam fightTeam;
	memcpy(&(fightTeam.m_stBase), &dbFightTeamBase, sizeof(FightTeamBase));
	m_fightTeamMap.insert(FightTeamMap::value_type(fightTeam.GetFightTeamID(), fightTeam));
	m_fightTeamNameMap.insert(FightTeamNameMap::value_type(fightTeam.m_stBase.cFightTeamName, fightTeam.GetFightTeamID()));
}

// 初始化插入已有战队的队员
CT_VOID	CCFightTeamManager::InitFightTeamMember(const MSC_DB_FightTeamMember &dbFightTeamMember)
{
	_AUTOMUTEX(&m_mutex);
	CMD_G_FightTeamMember fightTeamMember = {0};
	memcpy(&fightTeamMember, &dbFightTeamMember, sizeof(FightTeamMember));
	FightTeamMap::iterator it = m_fightTeamMap.find(fightTeamMember.dwFTID);
	if (it != m_fightTeamMap.end())
	{
		it->second.InitInsertMember(dbFightTeamMember.iType, fightTeamMember);
	}
}

// 初始化插入战队动态
CT_VOID	CCFightTeamManager::InitEventLog(const MSC_DB_FightTeamEventLog &dbFightTeamEventLog)
{
	_AUTOMUTEX(&m_mutex);
	CMD_G_FightTeamEventLog fightTeamEventLog = {0};
	fightTeamEventLog.iReport = dbFightTeamEventLog.iReport;
	fightTeamEventLog.lDate = dbFightTeamEventLog.lDate;
	memcpy(fightTeamEventLog.cEventDesc, dbFightTeamEventLog.cEventDesc, DESCRIPTION_SIZE);

	FightTeamMap::iterator it = m_fightTeamMap.find(dbFightTeamEventLog.dwFTID);
	if (it != m_fightTeamMap.end())
	{
		it->second.InitEventLog(fightTeamEventLog);
	}
}

//召唤信息
CT_VOID CCFightTeamManager::InitFightTeamSummon(const MSC_DB_FightTeamSummonInfo &dbFightTeamSummoninfo)
{
	_AUTOMUTEX(&m_mutex);
	FightTeamSummonBoss FTSB={0};
	FTSB.dwFTID=dbFightTeamSummoninfo.dwFTID;
	FTSB.iBossIndex=dbFightTeamSummoninfo.iBossIndex;
	FTSB.dwUserID[0]=dbFightTeamSummoninfo.dwUserID;
	FTSB.iWiner=dbFightTeamSummoninfo.iWin;
	memcpy(&FTSB.Res[0],&dbFightTeamSummoninfo.Res,sizeof(FTSB.Res[0]));

	FightTeamMap::iterator it = m_fightTeamMap.find(dbFightTeamSummoninfo.dwFTID);
	if (it != m_fightTeamMap.end())
	{
		it->second.InsertSummonInfo(FTSB);
	}
	return;
}

//召唤道具奖励
CT_VOID CCFightTeamManager::InitFihgtTeamSummonItem(const MSC_DB_FightTeamSummInfoItem &dbFightTeamSummonItem)
{
	_AUTOMUTEX(&m_mutex);

	FightTeamSummonItem FTSI={0};
	FTSI.dwUserID=dbFightTeamSummonItem.dwUserID;
	FTSI.iChestIndex=dbFightTeamSummonItem.iChestIndex;
	FTSI.iCount=dbFightTeamSummonItem.iCount;
	FTSI.iItemID=dbFightTeamSummonItem.iItemID;

	FightTeamMap::iterator it = m_fightTeamMap.find(dbFightTeamSummonItem.dwFTID);
	if (it != m_fightTeamMap.end())
	{
		if(dbFightTeamSummonItem.iType == FIGHTTEAM_BOSS_SUMMON)
			it->second.InsertSummonReward(FTSI);
		else
			(*it).second.InsertTimeBossRewardr(FTSI,CT_FALSE);
	}
	return;
}


CT_VOID	CCFightTeamManager::InsertFightTeamRecruit(const MSC_DB_FightTeamRecruit &dbFightTeamRecruit)
{
	_AUTOMUTEX(&m_mutex);	
	STRecruit stRecruit = {0};
	memcpy(&stRecruit, &dbFightTeamRecruit, sizeof(STRecruit));
//	m_fightTeamRecruitMap.insert(FightTeamRecruitMap::value_type(stRecruit.dwFTID, stRecruit));
	m_fightTeamRecruitMap[stRecruit.dwFTID] = stRecruit;
}

CT_VOID CCFightTeamManager::InsertCSInspore(MSC_DB_INSPORE &DBCsi)
{
	_AUTOMUTEX(&m_mutex);
	CT_DWORD dwFTID=GetFightTeamIDByUser(DBCsi.dwUserID);
	
	CCFightTeam *pFT=0;
	if(Find(dwFTID,&pFT))
	{
		CSInspore csi={0};
		csi.dwUserID=DBCsi.dwUserID;
		csi.iExp=DBCsi.iExp;
		csi.iGrade=DBCsi.iGrade;
		csi.iPowerID=DBCsi.iID;
		pFT->InsertInspore(csi);
	}
}
//插入时限boss
CT_VOID CCFightTeamManager::InsertTimeBossInfo(MSC_DB_FTTimeBossInfo &bi)
{
	_AUTOMUTEX(&m_mutex);
	CCFightTeam *pFT=0;
	if(Find(bi.dwFTID,&pFT))
	{
		FTTimeBossInfo tbi={0};
		tbi.iIndex=bi.iIndex;
		tbi.iCurrentHP=bi.iCurrentHP;
		tbi.iSummonBeginTime=bi.iSummonBeginTime;

		pFT->InsertTimeBoss(tbi);
	}
	return;
}

//插入时限boss成员
CT_VOID CCFightTeamManager::InsertTimeBossMember(MSC_DB_FTTimeBossMember &bm)
{
	_AUTOMUTEX(&m_mutex);

	CCFightTeam *pFT=0;
	if(Find(bm.dwFTID,&pFT))
	{
		FTTimeBossMember tbm={0};
		tbm.dwUserID=bm.dwUserID;
		tbm.iFightNum=bm.iFightNum;
		tbm.iFTContribute=bm.iFTContribute;
		tbm.iFTFood=bm.iFTFood;
		tbm.iHurt=bm.iHurt;
		pFT->InsertTimeBossMember(tbm);
	}
	return;
}

CT_BOOL CCFightTeamManager::CheckFightTeamRecruit(CT_DWORD dwFTID)
{
	_AUTOMUTEX(&m_mutex);
	if (m_fightTeamRecruitMap.find(dwFTID) != m_fightTeamRecruitMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

// 用户下线,更新战队成员数据
CT_VOID CCFightTeamManager::UserExitEvent(CCUser *pUser)
{
	// 这里不能加锁, 否则会死锁
	CT_DWORD dwFTID = GetFightTeamIDByUser(pUser->GetUserID());
	CCFightTeam *pFightTeam;
	if (Find(dwFTID, &pFightTeam))
	{
		pFightTeam->UserExitEvent(pUser);
	}
}

// 用户上线, 推送成员在线状态
CT_VOID CCFightTeamManager::UserLoginEvent(CT_DWORD dwUser)
{
	// 这里不能加锁, 否则会死锁
	CT_DWORD dwFTID = GetFightTeamIDByUser(dwUser);
	CCFightTeam *pFightTeam;
	if (Find(dwFTID, &pFightTeam))
	{
		pFightTeam->UserLoginEvent(dwUser);
	}
}


// 每日刷新战队排名
CT_VOID CCFightTeamManager::RefreshFightTeamTop()
{
	_AUTOMUTEX(&m_mutex);
	std::multimap<CT_INT32, CCFightTeam*> tFightingMap;
	FightTeamMap::iterator it;
	std::multimap<CT_INT32, CCFightTeam*>::iterator it_fighting;
	for (it = m_fightTeamMap.begin(); it != m_fightTeamMap.end(); ++it)
	{
		it->second.ComputeFightingPower();
		it->second.m_stBase.iPreTop = it->second.m_stBase.iTop;
		tFightingMap.insert(std::multimap<CT_INT32, CCFightTeam*>::value_type(it->second.m_stBase.iFightingPower, &(it->second)));
	}

	m_ftRankingMap.clear();
	CT_INT32 iTop = 1;
	for (it_fighting = tFightingMap.begin(); it_fighting != tFightingMap.end(); ++it_fighting)
	{
		it_fighting->second->m_stBase.iTop = tFightingMap.size() - iTop + 1;
		m_ftRankingMap.insert(std::map<CT_INT32,CT_DWORD>::value_type(it_fighting->second->m_stBase.iTop, it_fighting->second->m_stBase.dwFTID));
		iTop++;
	}
}

//清理战队召唤
CT_VOID CCFightTeamManager::ClearFightTeamSummon()
{
	_AUTOMUTEX(&m_mutex);

	FightTeamMap::iterator it;
	for (it = m_fightTeamMap.begin(); it != m_fightTeamMap.end(); ++it)
	{
		it->second.ClearSummonData();
	}
	return;
}

CT_VOID CCFightTeamManager::GetFightTeamTop(CT_DWORD *dwFTList, CT_INT32 &iCount)
{
	_AUTOMUTEX(&m_mutex);

	CT_INT32 n = 0;
	for (std::map<CT_INT32,CT_DWORD>::iterator it = m_ftRankingMap.begin(); it != m_ftRankingMap.end() && n < iCount; ++it)
	{
		dwFTList[n] = it->second;
		++n;
	}
	iCount = n;
}

//战队聊天
CT_VOID CCFightTeamManager::SendFightTeamChat(CMD_G_FightTeamChat *pFightTeamChat, CT_DWORD len)
{
	_AUTOMUTEX(&m_mutex);

	CT_DWORD dwFTID = GetFightTeamIDByUser(pFightTeamChat->dwUserID);
	if (dwFTID == 0)
	{
		return;
	}
	CCFightTeam *pFightTeam = 0;
	if (Find(dwFTID, &pFightTeam))
	{
		pFightTeam->FightTeamChat(pFightTeamChat, len);
	}
}

CT_VOID CCFightTeamManager::FightTeamMemberRef(const CT_DWORD dwUser, const CT_INT32 iGrade, const CT_INT32 iFightingPower)
{
	_AUTOMUTEX(&m_mutex);

	CT_DWORD dwFTID = GetFightTeamIDByUser(dwUser);
	if (dwFTID == 0)
	{
		return;
	}
	CCFightTeam *pFightTeam = 0;
	if (Find(dwFTID, &pFightTeam))
	{
		pFightTeam->MemberRef(dwUser,iGrade,iFightingPower);
	}
}

CT_VOID CCFightTeamManager::SendFightTeamMemberGuard(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);

	CT_DWORD dwFTID = GetFightTeamIDByUser(dwUserID);
	if (dwFTID == 0)
	{
		return;
	}
	CCFightTeam *pFightTeam = 0;
	if (Find(dwFTID, &pFightTeam))
	{
		pFightTeam->SendMemberGuard(dwUserID);
	}
}

CT_VOID CCFightTeamManager::UpdateIslandData()
{
	_AUTOMUTEX(&m_mutex);

	CT_INT32 i = 0;
	CCUser* pUser = 0;
	FightTeamMemberUID ftUid = {0};
	FightTeamMap::iterator it = m_fightTeamMap.begin();
	FightTeamMap::iterator itEnd = m_fightTeamMap.end();

	for (;it != itEnd;++it)
	{
		memset(&ftUid,0,sizeof(ftUid));
		it->second.GetFightTeamMemberUID(&ftUid);
		for (i = 0;i < ftUid.dwCount;++i)
		{
			pUser = 0;
			if (CCUserManager::getSingleton().Find(ftUid.dwMemberUID[i], &pUser))
			{
				pUser->m_userIsland.UpdateIslandOnClock(it->first);
				pUser->m_userTask.UpdateIslandTaskOnClock();
			}
		}
	}
}

//修改战队名字
CT_VOID CCFightTeamManager::ChangeFTName(CT_CHAR cOldName[NAME_SIZE],CT_CHAR cNewName[NAME_SIZE])
{
	_AUTOMUTEX(&m_mutex);
	FightTeamNameMap::iterator it=m_fightTeamNameMap.find(cOldName);
	if(it != m_fightTeamNameMap.end())
	{
		CT_DWORD dwFTID=(*it).second;
		m_fightTeamNameMap.erase(it);
		m_fightTeamNameMap.insert(FightTeamNameMap::value_type(cNewName,dwFTID));
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamManager::ChangeFTName oldname:%s newName:%s",cOldName,cNewName);
	}
	return;
}