#include "CTop.h"
#include "CLogMeg.h"
#include "CFightTeam.h"
#include "CFightTeamManager.h"
#include "CGlobeFunc.h"

template<> CCTop* CCSingleton<CCTop>::ms_Singleton=0;

CCTop::CCTop(void)
{
	m_iIndex=0;
	m_UserDataMap.clear();
	m_GradeList.clear();
	m_RankingList.clear();
	m_FightingPowerList.clear();
	m_FightingTeamList.clear();
	m_ContinuousWinList.clear();
	m_FightingFlagList.clear();
	m_MasterInfoMap.clear();
	m_PlunderResList.clear();
	memset(m_dwUserID,0,sizeof(m_dwUserID));
	memset(m_UserInfo,0,sizeof(m_UserInfo));

	m_bInit=CT_FALSE;
}

CCTop::~CCTop(void)
{
}

CT_VOID CCTop::ChangeUserInfo(const User_Fighting &FightingData,LookUserInfo &UserData)
{
	_AUTOMUTEX(&m_Mutex);

	CMD_G_PVPRankingInfo RankingInfo={0};

	if(FightingData.MasterInfo.dwUserID == 0)return;

	MasterData MasterInfo=FightingData.MasterInfo;
	UserData.dwUserID=MasterInfo.dwUserID;
	UserData.iGrade=MasterInfo.mvd.stGrade.iLevel;
	UserData.iOccupation=MasterInfo.iOccupation;
	UserData.iRanking=MasterInfo.mvd.stRank.iLevel;
	UserData.iPrestige=MasterInfo.mvd.stRank.iCurrentExp;

	//用户信息
	UserData.iMaxHP=MasterInfo.mcs.iMaxHP;		
	UserData.iMaxMP=MasterInfo.mcs.iMaxMP;		
	//UserData.iStrength=MasterInfo.mcs.iStrength; 
	//UserData.iAgility=MasterInfo.mcs.iAgility;  
	//UserData.iIntellect=MasterInfo.mcs.iIntellect;
	//UserData.iSpirit=MasterInfo.mcs.iSpirit;   
	//UserData.fHitRate=MasterInfo.mcs.fHitRate;  
	//UserData.fDodgeRate=MasterInfo.mcs.fDodgeRate;
	//UserData.fCritRate=MasterInfo.mcs.fCritRate; 
	//UserData.fResilient=MasterInfo.mcs.fResilient;
	UserData.iAttack=MasterInfo.mcs.iAttack;   
	UserData.iDefence=MasterInfo.mcs.iDefence;  
	memcpy(UserData.cUserName,MasterInfo.cNickName,sizeof(UserData.cUserName));

	//装备信息
	memcpy(UserData.EquipedItemData,FightingData.EquipedItemData,sizeof(UserData.EquipedItemData));
	memcpy(UserData.EquipmentInfo,FightingData.EquipmentInfo,sizeof(UserData.EquipmentInfo));

	//宠物信息
	memcpy(&UserData.pi,&FightingData.pi,sizeof(UserData.pi));
	UserData.iVIPLevel=FightingData.iVIPLevel;
	UserData.iRank=FightingData.MasterInfo.mvd.stRank.iLevel;
	UserData.iFightingPower=FightingData.MasterInfo.mvd.stRes.iFightingPower;
	memcpy(UserData.enchantData,FightingData.enchantData,sizeof(UserData.enchantData));
	memcpy(UserData.cFightTeam,FightingData.cFightTeam,sizeof(UserData.cFightTeam));

	return;
}

//重置函数
CT_VOID CCTop::ResetVariable()
{
	m_UserDataMap.clear();
	m_GradeList.clear();
	m_RankingList.clear();
	m_FightingPowerList.clear();
	m_FightingTeamList.clear();
	m_ContinuousWinList.clear();
	m_FightingFlagList.clear();
	m_PlunderResList.clear();
	return;
}

//清理masterinfo
CT_VOID CCTop::ClearMasterInfo()
{
	m_MasterInfoMap.clear();
	return;
}

//插入战队排名
CT_VOID CCTop::InsertTopFightingTeam(CT_DWORD *pTeamID,CT_INT32 iCount)
{
	_AUTOMUTEX(&m_Mutex);

	for(CT_INT32 i=0;i<iCount;++i)
	{
		if(pTeamID[i] == 0)continue;
		CCFightTeam *pTeam=0;
		CCFightTeamManager::getSingleton().Find(pTeamID[i],&pTeam);

		CMD_G_FightTeamBase TeamBase={0};
		if(pTeam != NULL)
		{
			pTeam->GetFightTeamBase(&TeamBase);

			TopFightingTeam Team={0};
			Team.dwFightingTeamID=pTeamID[i];
			memcpy(Team.cMasterName,TeamBase.cBossName,sizeof(Team.cMasterName));
			memcpy(Team.cFightingTeamName,TeamBase.cFightTeamName,sizeof(Team.cFightingTeamName));
			Team.iFightingPower=TeamBase.iFightingPower;
			Team.iPlayerNum=TeamBase.nMember;
			Team.iTopFightingTeam=TeamBase.iTop;

			m_FightingTeamList.push_back(Team);
		}
	}
	return;
}

//插入用户信息
CT_VOID CCTop::InsertTopUserInfo(LookUserInfo *LookData,CT_INT32 iCount,ENUM_TOP Type)
{
	_AUTOMUTEX(&m_Mutex);
	if(iCount > TOP_COUNT)
	{
		CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo iCount > TOP_COUNT");
		return;
	}
	//插入用户信息
	for(CT_INT32 i=0;i<iCount;++i)
	{
		if(LookData[i].dwUserID == 0)continue;

		UD_IT it=m_UserDataMap.find(LookData[i].dwUserID);
		if(it != m_UserDataMap.end())
		{
			memcpy(&((*it).second),&LookData[i],sizeof(LookData[i]));
		}
		else
		{
			m_UserDataMap.insert(UserDataMap::value_type(LookData[i].dwUserID,LookData[i]));
		}
		
	}

	switch(Type)
	{
	case TOP_GRADE:				//提取等级排行信息
		{
			for(CT_INT32 i=0;i < iCount;++i)
			{
				TopGrade Grade={0};
				if(LookData[i].dwUserID != 0)
				{
					CCFightTeam *pTeam=0;
					CT_DWORD TeamID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(LookData[i].dwUserID);
					CCFightTeamManager::getSingleton().Find(TeamID,&pTeam);
					MI_IT miit=m_MasterInfoMap.find(LookData[i].dwUserID);
					if(miit != m_MasterInfoMap.end())
					{
						Grade.dwUserID=LookData[i].dwUserID;
						Grade.iFightingPower=(*miit).second.iFightingPower;
						Grade.iGrade=(*miit).second.iGrade;
						Grade.iOccupation=LookData[i].iOccupation;
						memcpy(Grade.cName,LookData[i].cUserName,sizeof(Grade.cName));
						Grade.iTopGrade=i;
						if(pTeam != NULL)
							pTeam->GetFightTeamName(Grade.cFightingTeam);

						m_GradeList.push_back(Grade);
					}
					else
					{
						CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo TOP_GRADE miit == m_MasterInfoMap.end()");
					}

				}
			}
			break;
		}
	case TOP_RANKING:			//提取军衔排行信息
		{
			for(CT_INT32 i=0;i < iCount;++i)
			{
				TopRanking Ranking={0};
				if(LookData[i].dwUserID != 0)
				{
					MI_IT miit=m_MasterInfoMap.find(LookData[i].dwUserID);
					if(miit != m_MasterInfoMap.end())
					{
						Ranking.dwUserID=LookData[i].dwUserID;
						Ranking.iGrade=(*miit).second.iGrade;//LookData[i].MasterInfo.mvd.stGrade.iLevel;
						Ranking.iOccupation=LookData[i].iOccupation;
						Ranking.iRankingGrade=(*miit).second.iRankGrade;//LookData[i].MasterInfo.mvd.stRank.iLevel;
						Ranking.iFightingPower=(*miit).second.iFightingPower;//LookData[i].MasterInfo.mvd.stRes.iFightingPower;
						Ranking.iTopRanking=i;
						Ranking.iWinTimes=(*miit).second.iCurrContWin;//LookData[i].MasterInfo.mvd.pvpData.iVictoryTimes;
						Ranking.iTopRanking=i;
						memcpy(Ranking.cName,LookData[i].cUserName,sizeof(Ranking.cName));

						m_RankingList.push_back(Ranking);
					}
					else
					{
						CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo TOP_RANKING miit == m_MasterInfoMap.end()");
					}
				}
			}
			break;
		}
	case TOP_FIGHTINGPOWER:		//提取战斗力排行信息
		{
			for(CT_INT32 i=0;i < iCount;++i)
			{
				TopFightingPower FightingPower={0};
				if(LookData[i].dwUserID != 0)
				{
					CCFightTeam *pTeam=0;
					CT_DWORD TeamID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(LookData[i].dwUserID);
					CCFightTeamManager::getSingleton().Find(TeamID,&pTeam);
					MI_IT miit=m_MasterInfoMap.find(LookData[i].dwUserID);
					if(miit != m_MasterInfoMap.end())
					{
						FightingPower.dwUserID=LookData[i].dwUserID;
						FightingPower.iGrade=(*miit).second.iGrade;//LookData[i].MasterInfo.mvd.stGrade.iLevel;
						FightingPower.iOccupation=LookData[i].iOccupation;
						FightingPower.iFightingPower=(*miit).second.iFightingPower;//LookData[i].MasterInfo.mvd.stRes.iFightingPower;
						FightingPower.iTopFightingPower=i;
						memcpy(FightingPower.cName,LookData[i].cUserName,sizeof(FightingPower.cName));
						if(pTeam != NULL)
							pTeam->GetFightTeamName(FightingPower.cFightingTeam);
						m_FightingPowerList.push_back(FightingPower);
					}
					else
					{
						CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo TOP_FIGHTINGPOWER miit == m_MasterInfoMap.end()");
					}
				}
			}
			break;
		}
	case TOP_CONTINUOUSWIN:		//提取连胜排行信息
		{
			for(CT_INT32 i=0;i < iCount;++i)
			{
				TopContinuousWin ContinuousWin={0};
				if(LookData[i].dwUserID != 0)
				{
					MI_IT miit=m_MasterInfoMap.find(LookData[i].dwUserID);
					if(miit != m_MasterInfoMap.end())
					{
						ContinuousWin.dwUserID=LookData[i].dwUserID;
						ContinuousWin.iGrade=(*miit).second.iGrade;//LookData[i].MasterInfo.mvd.stGrade.iLevel;
						ContinuousWin.iOccupation=LookData[i].iOccupation;
						ContinuousWin.iContinuousWinNum=(*miit).second.iCurrContWin;//LookData[i].MasterInfo.mvd.topData.iMaxContinuousWin;
						ContinuousWin.iFightingPower=(*miit).second.iFightingPower;//LookData[i].MasterInfo.mvd.stRes.iFightingPower;
						memcpy(ContinuousWin.cName,LookData[i].cUserName,sizeof(ContinuousWin.cName));

						m_ContinuousWinList.push_back(ContinuousWin);
					}
					else
					{
						CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo TOP_CONTINUOUSWIN miit == m_MasterInfoMap.end()");
					}
				}
			}

			//最后一个排行插入结束后清理数据
			ClearMasterInfo();
			m_bInit=CT_TRUE;
			CCLogMeg::getSingleton().WriteError("Init finish!!!!!!!!!!!!!!");
			break;
		}
	case TOP_FIGHTINGFLAG:
		{
			for(CT_INT32 i=0;i < iCount;++i)
			{
				TopFightingFlag FightingFlagPower={0};
				if(LookData[i].dwUserID != 0)
				{
					CCFightTeam *pTeam=0;
					CT_DWORD TeamID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(LookData[i].dwUserID);
					CCFightTeamManager::getSingleton().Find(TeamID,&pTeam);
					MI_IT miit=m_MasterInfoMap.find(LookData[i].dwUserID);
					if(miit != m_MasterInfoMap.end())
					{
						FightingFlagPower.dwUserID=LookData[i].dwUserID;
						FightingFlagPower.iGrade=(*miit).second.iGrade;
						FightingFlagPower.iOccupation=LookData[i].iOccupation;
						FightingFlagPower.iFightingFlagPower=(*miit).second.iFightingFlagPower;
						memcpy(FightingFlagPower.cName,LookData[i].cUserName,sizeof(FightingFlagPower.cName));
						if(pTeam != NULL)
							pTeam->GetFightTeamName(FightingFlagPower.cFightingTeam);
						m_FightingFlagList.push_back(FightingFlagPower);
					}
					else
					{
						CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo TOP_FIGHTINGFLAG miit == m_MasterInfoMap.end()");
					}
				}
			}
			break;
		}
	case TOP_PLUNDER:
		{
			for (CT_INT32 i = 0;i < TOP_COUNT_LITTLE;++i)
			{
				TopPlunderRes topPlunder = {0};
				if (LookData[i].dwUserID != 0)
				{
					CCFightTeam* pTeam = 0;
					CT_DWORD teamID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(LookData[i].dwUserID);
					CCFightTeamManager::getSingleton().Find(teamID, &pTeam);
					MI_IT miit = m_MasterInfoMap.find(LookData[i].dwUserID);
					if (miit != m_MasterInfoMap.end())
					{
						topPlunder.dwUserID = LookData[i].dwUserID;
						topPlunder.iGrade = miit->second.iGrade;
						topPlunder.iOccupation = LookData[i].iOccupation;
						topPlunder.iPlunderNum = miit->second.iPlunderScore;
						strncpy(topPlunder.cName, LookData[i].cUserName, sizeof(topPlunder.cName));
						if (NULL != pTeam)
						{
							pTeam->GetFightTeamName(topPlunder.cFightingTeam);
						}
						m_PlunderResList.push_back(topPlunder);
					}
					else
					{
						CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo TOP_PLUNDER miit == m_MasterInfoMap.end()");
					}
				}
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCTop::InsertTopUserInfo type:%d",Type);
			return;
		}
		
	}
	return;
}

//获取等级排名信息
CT_VOID CCTop::GetTopGradeInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen < (CT_INT32)(sizeof(TopGrade)*m_GradeList.size()))
	{
		CCLogMeg::getSingleton().WriteError("CCTop::GetTopGradeInfo iLen < sizeof(TopGrade)*m_GradeList.size()");
		return;
	}

	CT_INT32 iSize=0;
	for(GL_IT it=m_GradeList.begin();it != m_GradeList.end();++it)
	{
		memcpy(pBuf+iSize,&(*it),sizeof(TopGrade));
		iSize+=sizeof(TopGrade);
	}

	iLen=iSize;
	return;
}

//获取军衔排名信息
CT_VOID CCTop::GetTopRankingInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen < (CT_INT32)(sizeof(TopRanking)*m_RankingList.size()))
	{
		CCLogMeg::getSingleton().WriteError("CCTop::GetTopRankingInfo iLen < sizeof(TopRanking)*m_RankingList.size()");
		return;
	}

	CT_INT32 iSize=0;
	for(RL_IT it=m_RankingList.begin();it != m_RankingList.end();++it)
	{
		memcpy(pBuf+iSize,&(*it),sizeof(TopRanking));
		iSize+=sizeof(TopRanking);
	}

	iLen=iSize;
	return;
}

//获取战斗力排名信息
CT_VOID CCTop::GetTopFightingPowerInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen < (CT_INT32)(sizeof(TopFightingPower)*m_FightingPowerList.size()))
	{
		CCLogMeg::getSingleton().WriteError("CCTop::GetTopFightingPowerInfo iLen < sizeof(TopFightingPower)*m_FightingPowerList.size()");
		return;
	}

	CT_INT32 iSize=0;
	for(FP_IT it=m_FightingPowerList.begin();it != m_FightingPowerList.end();++it)
	{
		memcpy(pBuf+iSize,&(*it),sizeof(TopFightingPower));
		iSize+=sizeof(TopFightingPower);
	}

	iLen=iSize;
	return;
}

//获取战队排名信息
CT_VOID CCTop::GetTopFightingTeamInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen < (CT_INT32)(sizeof(TopFightingTeam)*m_FightingTeamList.size()))
	{
		CCLogMeg::getSingleton().WriteError("CCTop::GetTopFightingPowerInfo iLen < sizeof(TopFightingTeam)*m_FightingTeamList.size()");
		return;
	}

	CT_INT32 iSize=0;
	for(FT_IT it=m_FightingTeamList.begin();it != m_FightingTeamList.end();++it)
	{
		memcpy(pBuf+iSize,&(*it),sizeof(TopFightingTeam));
		iSize+=sizeof(TopFightingTeam);
	}

	iLen=iSize;
	return;
}

//获取连胜排名信息
CT_VOID CCTop::GetTopContinuousWinInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen < (CT_INT32)(sizeof(TopContinuousWin)*m_ContinuousWinList.size()))
	{
		CCLogMeg::getSingleton().WriteError("CCTop::GetTopFightingPowerInfo iLen < sizeof(TopContinuousWin)*m_ContinuousWinList.size()");
		return;
	}

	CT_INT32 iSize=0;
	for(CW_IT it=m_ContinuousWinList.begin();it != m_ContinuousWinList.end();++it)
	{
		memcpy(pBuf+iSize,&(*it),sizeof(TopContinuousWin));
		iSize+=sizeof(TopContinuousWin);
	}

	iLen=iSize;
	return;
}

//获取战旗排名信息
CT_VOID CCTop::GetTopFightingFlagInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen < (CT_INT32)(sizeof(TopFightingFlag)*m_FightingFlagList.size()))
	{
		CCLogMeg::getSingleton().WriteError("CCTop::GetTopFightingFlagInfo iLen < sizeof(TopFightingFlag)*m_FightingFlagList.size()");
		return;
	}

	CT_INT32 iSize=0;
	for(FF_IT it=m_FightingFlagList.begin();it != m_FightingFlagList.end();++it)
	{
		memcpy(pBuf+iSize,&(*it),sizeof(TopFightingFlag));
		iSize+=sizeof(TopFightingFlag);
	}

	iLen=iSize;
	return;
}

//获取掠夺资源排名信息
CT_VOID CCTop::GetTopPlunderResInfo(CT_CHAR* pBuf, CT_INT32& iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if (iLen < sizeof(TopPlunderRes) * m_PlunderResList.size())
	{
		CCLogMeg::getSingleton().WriteError("CCTop::GetTopPlunderResInfo iLen < sizeof(TopPlunderRes) * m_PlunderResList.size()");
		return;
	}

	CT_INT32 iSize = 0;
	for (PR_IT prit = m_PlunderResList.begin();prit != m_PlunderResList.end();++prit)
	{
		if (prit->iPlunderNum == 0)
		{
			break;
		}
		memcpy(pBuf + iSize, &(*prit), sizeof(TopPlunderRes));
		iSize += sizeof(TopPlunderRes);
	}

	iLen = iSize;
	return;
}

//获取用户信息
CT_VOID CCTop::GetTopUserInfo(const CT_DWORD dwUserID,LookUserInfo &LookData)
{
	_AUTOMUTEX(&m_Mutex);

	UD_IT it=m_UserDataMap.find(dwUserID);
	if(it != m_UserDataMap.end())
	{
		memcpy(&LookData,&((*it).second),sizeof(LookData));
	}
	return;
}

//前50名等级平均
CT_INT32 CCTop::GetTopGradeAverage()
{
	if (m_GradeList.size() == 0)
	{
		return 0;
	}
	CT_INT32 iGrade=0;
	GL_IT itend=m_GradeList.end();
	for(GL_IT it=m_GradeList.begin();it != itend;++it)
	{
		iGrade+=(*it).iGrade;
	}

	iGrade=iGrade/m_GradeList.size();
	return iGrade;
}

CT_INT64 CCTop::Get64Int(CT_INT32 high,CT_INT32 low)
{
	CT_INT64 lKey=high;
	lKey<<=32;
	lKey|=low;
	return lKey;
}

//插入领主信息
CT_VOID CCTop::InsterMasterInfo(MaterInfo &mi)
{
	_AUTOMUTEX(&m_Mutex);

	m_MasterInfoMap.insert(MaterInfoMap::value_type(mi.dwUserID,mi));
	return;
}

//排序
CT_VOID CCTop::SequenceTop()
{
	SequenceMap GradeMap;
	SequenceMap RankGradeMap;
	SequenceMap FightingPowerMap;
	SequenceMap ContWinMap;
	SequenceMap FightingFlagMap;
	SequenceMap PlunderResMap;

	MI_IT itend=m_MasterInfoMap.end();
	for(MI_IT it=m_MasterInfoMap.begin();it != itend;++it)
	{
		GradeMap.insert(SequenceMap::value_type(Get64Int((*it).second.iGrade,(*it).second.iExp),(*it).second.dwUserID));
		RankGradeMap.insert(SequenceMap::value_type(Get64Int((*it).second.iRankGrade,(*it).second.iRankExp),(*it).second.dwUserID));
		FightingPowerMap.insert(SequenceMap::value_type((*it).second.iFightingPower,(*it).second.dwUserID));
		ContWinMap.insert(SequenceMap::value_type((*it).second.iCurrContWin,(*it).second.dwUserID));
		if((*it).second.iFightingFlagPower > 0)
			FightingFlagMap.insert(SequenceMap::value_type((*it).second.iFightingFlagPower,(*it).second.dwUserID));
		PlunderResMap.insert(SequenceMap::value_type(Get64Int(it->second.iPlunderScore,(*it).second.iFightingPower) , it->second.dwUserID));
	}

	CT_DWORD dwUserID[TOP_COUNT]={0};
	CT_INT32 iTop=GradeMap.size();
	SM_IT smend=GradeMap.end();
	for(SM_IT it=GradeMap.begin();it != smend;++it)
	{
		MI_IT miit=m_MasterInfoMap.find((*it).second);
		if(miit != m_MasterInfoMap.end())
		{
			(*miit).second.iNowTopGrade=iTop;
			iTop--;

			if(iTop < TOP_COUNT && iTop >= 0)
			{
				dwUserID[iTop]=(*miit).second.dwUserID;
			}
		}
	}

	//查找用户信息
	MSC_DB_Top DBTop={0};
	DBTop.TopType=TOP_GRADE;
	memcpy(DBTop.dwUserID,dwUserID,sizeof(DBTop.dwUserID));
	CT_DWORD dbThreadID = GAME_THREAD_DATABASE;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOP_INFO,(CT_CHAR*)&DBTop,sizeof(DBTop));

	iTop=RankGradeMap.size();
	smend=RankGradeMap.end();
	memset(dwUserID,0,sizeof(dwUserID));
	for(SM_IT it=RankGradeMap.begin();it != smend;++it)
	{
		MI_IT miit=m_MasterInfoMap.find((*it).second);
		if(miit != m_MasterInfoMap.end())
		{
			(*miit).second.iNowTopRank=iTop;
			iTop--;

			if(iTop < TOP_COUNT && iTop >= 0)
			{
				dwUserID[iTop]=(*miit).second.dwUserID;
			}
		}
	}

	//查找用户信息
	memset(&DBTop,0,sizeof(DBTop));
	DBTop.TopType=TOP_RANKING;
	memcpy(DBTop.dwUserID,dwUserID,sizeof(DBTop.dwUserID));
	dbThreadID = GAME_THREAD_DATABASE;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOP_INFO,(CT_CHAR*)&DBTop,sizeof(DBTop));

	iTop=FightingPowerMap.size();
	smend=FightingPowerMap.end();
	memset(dwUserID,0,sizeof(dwUserID));
	for(SM_IT it=FightingPowerMap.begin();it != smend;++it)
	{
		MI_IT miit=m_MasterInfoMap.find((*it).second);
		if(miit != m_MasterInfoMap.end())
		{
			(*miit).second.iNowTopFightingPower=iTop;
			iTop--;

			if(iTop < TOP_COUNT && iTop >= 0)
			{
				dwUserID[iTop]=(*miit).second.dwUserID;
			}
		}
	}

	//查找用户信息
	memset(&DBTop,0,sizeof(DBTop));
	DBTop.TopType=TOP_FIGHTINGPOWER;
	memcpy(DBTop.dwUserID,dwUserID,sizeof(DBTop.dwUserID));
	dbThreadID = GAME_THREAD_DATABASE;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOP_INFO,(CT_CHAR*)&DBTop,sizeof(DBTop));

	iTop=FightingFlagMap.size();
	smend=FightingFlagMap.end();
	memset(dwUserID,0,sizeof(dwUserID));
	for(SM_IT it=FightingFlagMap.begin();it != smend;++it)
	{
		MI_IT miit=m_MasterInfoMap.find((*it).second);
		if(miit != m_MasterInfoMap.end())
		{
			(*miit).second.iNowTopFightingFlagPower=iTop;
			iTop--;

			if(iTop < TOP_COUNT && iTop >= 0)
			{
				dwUserID[iTop]=(*miit).second.dwUserID;
			}
		}
	}

	//查找用户信息
	memset(&DBTop,0,sizeof(DBTop));
	DBTop.TopType=TOP_FIGHTINGFLAG;
	memcpy(DBTop.dwUserID,dwUserID,sizeof(DBTop.dwUserID));
	dbThreadID = GAME_THREAD_DATABASE;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOP_INFO,(CT_CHAR*)&DBTop,sizeof(DBTop));

	// 掠夺排行榜
	iTop = PlunderResMap.size();
	smend = PlunderResMap.end();
	memset(dwUserID, 0, sizeof(dwUserID));
	for (SM_IT it = PlunderResMap.begin();it != smend;++it)
	{
		MI_IT miit = m_MasterInfoMap.find(it->second);
		if (miit != m_MasterInfoMap.end())
		{
			miit->second.iNowTopPlunder = iTop;
			iTop--;

			if (iTop < TOP_COUNT_LITTLE && iTop >= 0)
			{
				dwUserID[iTop] = miit->second.dwUserID;
			}
		}
	}

	// 查找用户信息
	memset(&DBTop, 0, sizeof(DBTop));
	DBTop.TopType = TOP_PLUNDER;
	memcpy(DBTop.dwUserID, dwUserID, sizeof(dwUserID));
	dbThreadID = GAME_THREAD_DATABASE;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOP_INFO, (CT_CHAR*)&DBTop, sizeof(DBTop));

	iTop=ContWinMap.size();
	smend=ContWinMap.end();
	memset(dwUserID,0,sizeof(dwUserID));
	for(SM_IT it=ContWinMap.begin();it != smend;++it)
	{
		MI_IT miit=m_MasterInfoMap.find((*it).second);
		if(miit != m_MasterInfoMap.end())
		{
			(*miit).second.iNowTopContWin=iTop;
			iTop--;

			if(iTop < TOP_COUNT && iTop >= 0)
			{
				dwUserID[iTop]=(*miit).second.dwUserID;
			}
		}
	}

	//查找用户信息
	memset(&DBTop,0,sizeof(DBTop));
	DBTop.TopType=TOP_CONTINUOUSWIN;
	memcpy(DBTop.dwUserID,dwUserID,sizeof(DBTop.dwUserID));
	dbThreadID = GAME_THREAD_DATABASE;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOP_INFO,(CT_CHAR*)&DBTop,sizeof(DBTop));
	
	return;
}

//查找排行榜用户
CT_BOOL CCTop::FindTopUser(CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);
	UD_IT it= m_UserDataMap.find(dwUserID);
	if(it != m_UserDataMap.end())
		return CT_TRUE;
	return CT_FALSE;
}
