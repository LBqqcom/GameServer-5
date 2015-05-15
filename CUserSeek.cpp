#include "CMutex.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CMD_Game.h"
#include "CUserSeek.h"
#include "CThreadMes.h"
#include "CDBUserEnterEvent.h"
#include "CNetInterface.h"
#include "CBroadcastScriptSetting.h"

CCUserSeek::CCUserSeek()
{
	m_WallAdditionMap.clear();
	m_TeamSeekMap.clear();
	m_KidnapGirlsMap.clear();
	m_kidnapWorkersMap.clear();
	m_FriendInfoMap.clear();
	m_RandInfoMap.clear();
	m_VistorInfoMap.clear();
	m_FriendSeekVector.clear();
	m_RandSeekVector.clear();
	m_AllRandPlayerMap.clear();
	m_BuildDataMap.clear();
	m_SpeedUpBidVector.clear();
	m_SeekFightingMap.clear();
	m_SeekInfoMap.clear();
	m_PetKidnapMap.clear();
	memset(m_iPos,0,sizeof(m_iPos));
	memset(&m_SeekData,0,sizeof(m_SeekData));
	memset(m_VistorInfo,0,sizeof(m_VistorInfo));
}
CCUserSeek::~CCUserSeek()
{
	m_WallAdditionMap.clear();
	m_TeamSeekMap.clear();
	m_KidnapGirlsMap.clear();
	m_kidnapWorkersMap.clear();
	m_FriendInfoMap.clear();
	m_RandInfoMap.clear();
	m_VistorInfoMap.clear();
	m_FriendSeekVector.clear();
	m_RandSeekVector.clear();
	m_AllRandPlayerMap.clear();
	m_BuildDataMap.clear();
	m_SpeedUpBidVector.clear();
	m_SeekFightingMap.clear();
	m_SeekInfoMap.clear();
	m_PetKidnapMap.clear();
	memset(m_iPos,0,sizeof(m_iPos));
	memset(&m_SeekData,0,sizeof(m_SeekData));
	memset(m_VistorInfo,0,sizeof(m_VistorInfo));
}
CT_VOID CCUserSeek::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser = pUser;
	m_pMutex = pMutex;
	m_WallAdditionMap.clear();
	m_TeamSeekMap.clear();
	m_KidnapGirlsMap.clear();
	m_kidnapWorkersMap.clear();
	m_FriendInfoMap.clear();
	m_RandInfoMap.clear();
	m_VistorInfoMap.clear();
	m_FriendSeekVector.clear();
	m_RandSeekVector.clear();
	m_AllRandPlayerMap.clear();
	m_BuildDataMap.clear();
	m_SpeedUpBidVector.clear();
	m_SeekFightingMap.clear();
	m_SeekInfoMap.clear();
	m_PetKidnapMap.clear();
	memset(m_iPos,0,sizeof(m_iPos));
	memset(&m_SeekData,0,sizeof(m_SeekData));
	memset(m_VistorInfo,0,sizeof(m_VistorInfo));
}

//清理用户数据
CT_VOID CCUserSeek::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_WallAdditionMap.clear();
	m_TeamSeekMap.clear();
	m_KidnapGirlsMap.clear();
	m_kidnapWorkersMap.clear();
	m_FriendInfoMap.clear();
	m_RandInfoMap.clear();
	m_VistorInfoMap.clear();
	m_FriendSeekVector.clear();
	m_RandSeekVector.clear();
	m_AllRandPlayerMap.clear();
	m_BuildDataMap.clear();
	m_SpeedUpBidVector.clear();
	m_SeekFightingMap.clear();
	m_SeekInfoMap.clear();
	m_PetKidnapMap.clear();
	memset(m_iPos,0,sizeof(m_iPos));
	memset(&m_SeekData,0,sizeof(m_SeekData));
	memset(m_VistorInfo,0,sizeof(m_VistorInfo));
}

CT_INT32 CCUserSeek::transData(const SeekData &src, CT_CHAR *pDst) const
{
	SeekData *pst = (SeekData *)pDst;
	return sizeof(*pst);
}

CT_VOID CCUserSeek::SyncData(CT_INT64 currTime, CT_BOOL bForce)
{
	_AUTOMUTEX(m_pMutex);
	// 同步礼品信息到数据库
	MSC_DB_SeekPresent present = {0};
	present.dwUserID = m_pUser->GetUserID();
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_DEL, (CT_CHAR*) &present, sizeof(present));

	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		if (m_SeekData.present[i].iChestID == 0)
		{
			break;
		}

		present.present = m_SeekData.present[i];
		CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_UPDATE, (CT_CHAR*) &present, sizeof(present));
	}
}

CT_INT64 CCUserSeek::Get64Int(CT_INT32 high,CT_INT32 low)
{
	CT_INT64 lKey=high;
	lKey<<=32;
	lKey|=low;
	return lKey;
}

CT_BOOL CCUserSeek::GetTeamFlag(CT_CHAR* pBuf,CT_INT32& ilen)
{
	_AUTOMUTEX(m_pMutex);
	if (ilen < sizeof(TeamSeek) * m_TeamSeekMap.size())
	{
		return CT_FALSE;
	}
	CT_INT32 iBufLen = 0;
	TeamSeekMap::iterator it = m_TeamSeekMap.begin();

	for (;it != m_TeamSeekMap.end();++it)
	{
		memcpy(pBuf + iBufLen, &(it->second), sizeof(TeamSeek));
		iBufLen += sizeof(TeamSeek);
	}
	ilen = iBufLen;
	return CT_TRUE;
}

CT_BOOL CCUserSeek::GetKidnapGirls(CT_CHAR* pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex);
	if (iLen < sizeof(KidnapInfo) * m_KidnapGirlsMap.size())
	{
		return CT_FALSE;
	}

	CT_INT32 iBufLen = 0;
	KidnapMap::iterator it = m_KidnapGirlsMap.begin();
	for (;it != m_KidnapGirlsMap.end();++it)
	{
		memcpy(pBuf + iBufLen, &(it->second), sizeof(KidnapInfo));
		iBufLen += sizeof(KidnapInfo);
	}
	iLen = iBufLen;
	return CT_TRUE;
}

CT_BOOL CCUserSeek::GetKidnapWorkers(CT_CHAR* pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex);
	if (iLen < sizeof(KidnapInfo) * m_kidnapWorkersMap.size())
	{
		return CT_FALSE;
	}

	CT_INT32 iBufLen = 0;
	KidnapMap::iterator it = m_kidnapWorkersMap.begin();
	for (;it != m_kidnapWorkersMap.end();++it)
	{
		memcpy(pBuf + iBufLen, &(it->second), sizeof(KidnapInfo));
		iBufLen += sizeof(KidnapInfo);
	}
	iLen = iBufLen;
	return CT_TRUE;
}

// 得到宠物人质信息
CT_BOOL CCUserSeek::GetKidnapPets(CT_CHAR* pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex);
	if (iLen < sizeof(KidnapInfo) * m_PetKidnapMap.size())
	{
		return CT_FALSE;
	}

	CT_INT32 iBufLen = 0;
	KidnapMap::iterator it = m_PetKidnapMap.begin();
	for (;it != m_PetKidnapMap.end();++it)
	{
		memcpy(pBuf + iBufLen, &(it->second), sizeof(KidnapInfo));
		iBufLen += sizeof(KidnapInfo);
	}
	iLen = iBufLen;
	return CT_TRUE;
}

//插入城墙技能
CT_VOID CCUserSeek::InsertWallAddition(WallAddition & wa)
{
	_AUTOMUTEX(m_pMutex);

	m_WallAdditionMap.insert(WallAdditionMap::value_type(wa.iID,wa));
	return;
}

//插入城墙士兵
CT_VOID CCUserSeek::InsertWallArmy(CT_INT32 iPos[POST_COUNT/2])
{
	_AUTOMUTEX(m_pMutex);
	memcpy(m_iPos,iPos,sizeof(m_iPos));


	return;
}

//插入人质
CT_VOID CCUserSeek::InsertHostages(const CT_INT32 iType,KidnapInfo &ki)
{
	_AUTOMUTEX(m_pMutex);
	if(KIDNAP_WOMAN == iType)
	{
		m_KidnapGirlsMap.insert(KidnapMap::value_type(ki.owerUserId,ki));
	}
	else if(KIDNAP_FARMER == iType)
	{
		m_kidnapWorkersMap.insert(KidnapMap::value_type(ki.owerUserId,ki));
	}
	else if (KIDNAP_PET == iType)
	{
		m_PetKidnapMap.insert(KidnapMap::value_type(ki.owerUserId, ki));
	}
	else
	{
		CCLogMeg::getSingleton().WriteDebug("CCUserSeek::InsertHostages iType:%d",iType);
	}
	return;
}

//插入旗帜
CT_VOID CCUserSeek::InsertFlags(TeamSeek ts)
{
	_AUTOMUTEX(m_pMutex);
	m_TeamSeekMap.insert(TeamSeekMap::value_type(Get64Int(ts.dwUserID, ts.iType),ts));
	return;
}

//增加城墙技能
CT_VOID CCUserSeek::AddWallAddition(const CT_INT32 iID)
{
	_AUTOMUTEX(m_pMutex);

	MSC_DB_WALL_ADDITION dbwa={0};
	dbwa.dwUserID=m_pUser->GetUserID();
	dbwa.iID=iID;

	WallAdditionMap::iterator it=m_WallAdditionMap.find(iID);
	if(it != m_WallAdditionMap.end())
	{
		(*it).second.iGrade+=1;

		dbwa.iGrade=(*it).second.iGrade;
	}
	else
	{
		WallAddition wa={0};
		wa.iID=iID;
		wa.iGrade=1;
		InsertWallAddition(wa);

		dbwa.iGrade=wa.iGrade;
	}

	//同步数据库
	G_SendDBThreadInfo(m_pUser->GetUserID(),MSC_MAIN_DB_SEEK,MSC_SUB_DB_WALL_ADDITION,(CT_CHAR*)&dbwa,sizeof(dbwa));
	

	return;
}

//释放人质
CT_VOID CCUserSeek::ReleaseHostages(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(m_pMutex);
	KidnapMap::iterator Girlsit=m_KidnapGirlsMap.find(dwUserID);
	if(Girlsit != m_KidnapGirlsMap.end())
	{
		m_KidnapGirlsMap.erase(Girlsit);
	}

	KidnapMap::iterator Workerit=m_kidnapWorkersMap.find(dwUserID);
	if(Workerit != m_kidnapWorkersMap.end())
	{
		MasterVariableData mvd={0};
		mvd.stRes.iWorker+=(*Workerit).second.Num;
		m_pUser->m_userBase.AddMastData(mvd,SAVATAG_KIDNAP);

		m_kidnapWorkersMap.erase(Workerit);
	}

	KidnapMap::iterator Petit = m_PetKidnapMap.find(dwUserID);
	if (Petit != m_PetKidnapMap.end())
	{
		m_PetKidnapMap.erase((Petit));
	}

	// 更新到数据库
	MSC_DB_SeekKidnap sk = {0};
	sk.dwUserID = m_pUser->GetUserID();
	sk.kidnap.owerUserId = dwUserID;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_KIDNAP_DELETE, (CT_CHAR*) &sk, sizeof(sk));
	return;
}

//获取城墙技能
CT_BOOL CCUserSeek::GetWallAddition(const CT_INT32 iID,WallAddition &wa)
{
	_AUTOMUTEX(m_pMutex);
	WallAdditionMap::iterator it=m_WallAdditionMap.find(iID);
	if(it == m_WallAdditionMap.end())
	{
		G_SeekWallAddition swa={0};
		if(!G_GetSeekWallAddition(iID,1,&swa))
		{
			return CT_FALSE;
		}
		else
		{
			wa.iID=iID;
		}
	}
	else
		memcpy(&wa,&((*it).second),sizeof(wa));
	return CT_TRUE;
}

CT_BOOL CCUserSeek::GetWallAddition(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);
	if(iLen <= m_WallAdditionMap.size()*sizeof(WallAddition))
	{
		CCLogMeg::getSingleton().WriteError("CCUserSeek::GetWallAddition iLen <= m_WallAdditionMap.size()*sizeof(WallAddition)");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	for(WallAdditionMap::iterator it=m_WallAdditionMap.begin();it != m_WallAdditionMap.end();++it)
	{
		memcpy(pBuf+iSize,&((*it).second),sizeof(WallAddition));
		iSize+=sizeof(WallAddition);
	}

	iLen=iSize;
	return CT_TRUE;
}

//获取城墙士兵
CT_BOOL CCUserSeek::GetWallArmy(CT_INT32 iPos[POST_COUNT/2])
{
	_AUTOMUTEX(m_pMutex);
	memcpy(iPos,m_iPos,sizeof(m_iPos));
	return CT_TRUE;
}

CT_BOOL CCUserSeek::GetWallArmy(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);
	if(iLen <= sizeof(m_iPos))
	{
		CCLogMeg::getSingleton().WriteDebug("CCUserSeek::GetWallArmy iLen <= sizeof(m_iPos)");
		return CT_FALSE;
	}
	
	memcpy(pBuf,m_iPos,sizeof(m_iPos));
	iLen=sizeof(m_iPos);
	return CT_TRUE;
}

//获取掠夺用户战斗信息
CT_BOOL CCUserSeek::GetKidnpaFightData(const CT_DWORD dwUserID,UserFightingStatus &ufs)
{
	_AUTOMUTEX(m_pMutex);
//	memcpy(&ufs,&m_Seeker.UserFightinginfo,sizeof(ufs));
	return CT_TRUE;
}

//获取访问信息
CT_VOID CCUserSeek::GetSeekData(SeekData& sd)
{
	_AUTOMUTEX(m_pMutex);

	sd = m_SeekData;

	return ;
}

//获取访客信息
CT_BOOL CCUserSeek::GetVistorInfo(CT_INT32 iType, const CT_DWORD dwUserID,VistorInfo &vi)
{
	_AUTOMUTEX(m_pMutex);

	time_t t = time(0);
	tm* pTm = localtime(&t);

	if (0 == iType / 100)
	{
		iType = pTm->tm_wday * 100 + iType;
	}

	VistorInfoMap::iterator it=m_VistorInfoMap.find(Get64Int(iType,dwUserID));
	if(it == m_VistorInfoMap.end())
	{
		return CT_FALSE;
	}

	memcpy(&vi,&((*it).second),sizeof(vi));

	return CT_TRUE;
}

//获取绑架人数
CT_INT32 CCUserSeek::GetKidnapNum(const CT_DWORD dwUserID,const CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	KidnapMap::iterator it;
	if(iType == KIDNAP_WOMAN)
	{
		it=m_KidnapGirlsMap.find(dwUserID);
		if(it != m_KidnapGirlsMap.end())
		{
			return (*it).second.Num;
		}
	}
	else if(iType == KIDNAP_FARMER)
	{
		it=m_kidnapWorkersMap.find(dwUserID);
		if(it != m_kidnapWorkersMap.end())
		{
			return (*it).second.Num;
		}
	}
	else if (iType == KIDNAP_PET)
	{
		it = m_PetKidnapMap.find(dwUserID);
		if (it != m_PetKidnapMap.end())
		{
			return 1;
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteDebug("CCUserSeek::GetKidnapNum iType:%d",iType);
	}

	return 0;
}

//获取探访数据
CT_BOOL CCUserSeek::GetSeekInfo(CT_CHAR* pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);

	if (iLen < sizeof(CMD_G_SeekData))
	{
		return CT_FALSE;
	}
	
	CMD_G_SeekData seekDate = {0};
	TeamSeekMap::iterator itFlag = m_TeamSeekMap.begin();
	KidnapMap::iterator itGirls = m_KidnapGirlsMap.begin();
	KidnapMap::iterator itWorker = m_kidnapWorkersMap.begin();
	UserInfoMap::iterator itFriend = m_FriendInfoMap.begin();
	UserInfoMap::iterator itRand = m_RandInfoMap.begin();
	VistorInfoMap::iterator itInfo = m_VistorInfoMap.begin();
	VistorInfoMap::iterator itSeek = m_SeekInfoMap.begin();
	KidnapMap::iterator itPet = m_PetKidnapMap.begin();
	CT_INT32 i = 0;
	CT_INT32 iInfoLen = VISTOR_INFO_LEN;
	CT_INT32 iPetChangeID[SEEK_KIDNAP_PET] = {0};
	CT_INT32 iSize = sizeof(iPetChangeID);
	time_t t = time(0);
	tm* pTm = localtime(&t);
	seekDate.iGoodWillTimes = m_SeekData.iSeekWillTimes;
	seekDate.iSpiteTimes = m_SeekData.iSeekSpiteTimes;
	seekDate.iSaintessNum = m_SeekData.iWomanNum;
	seekDate.iAddSpiteTimes = m_SeekData.iAddSpiteTimes;
	
	for (i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		G_GetCostRewardGroup(m_SeekData.present[i].iChestID, &(seekDate.aPresent[i]));
	}

	for (i = 0;i < SEEK_TEAM_FLAG_NUM && itFlag != m_TeamSeekMap.end();++i,++itFlag)
	{
		seekDate.aTeamSeek[i].dwFUserID = itFlag->second.dwUserID;
		seekDate.aTeamSeek[i].iType = itFlag->second.iType;
		seekDate.aTeamSeek[i].iFightResult = itFlag->second.iFightResult;
		strncpy(seekDate.aTeamSeek[i].cTName, itFlag->second.cTName, NAME_SIZE);
	}

	for (i = 0;i < SEEKED_SPITE_TIMES && itGirls != m_KidnapGirlsMap.end();++i,++itGirls)
	{
		seekDate.aSaintessInfo[i].owerUserId = itGirls->second.owerUserId;
		seekDate.aSaintessInfo[i].Num = itGirls->second.Num;
	}

	for (i = 0;i < SEEKED_SPITE_TIMES && itWorker != m_kidnapWorkersMap.end();++i,++itWorker)
	{
		seekDate.aFarmerInfo[i].owerUserId = itWorker->second.owerUserId;
		seekDate.aFarmerInfo[i].Num = itWorker->second.Num;
	}

	for (i = 0;i < SEEKED_SPITE_TIMES && itPet != m_PetKidnapMap.end();++i,++itPet)
	{
		seekDate.aPetInfo[i].owerUserId = itPet->second.owerUserId;
		seekDate.aPetInfo[i].Num = itPet->second.Num;
		seekDate.aPetInfo[i].iPetChange = itPet->second.iPetChange;
	}

	for (i = 0;i < SEEK_PLAYER_NUM && itFriend != m_FriendInfoMap.end();++i,++itFriend)
	{
		seekDate.lFriend[i].dwUserID = itFriend->second.dwUserID;
		seekDate.lFriend[i].iLevel = itFriend->second.iLevel;
		seekDate.lFriend[i].iOccupation = itFriend->second.iOccupation;
		seekDate.lFriend[i].iStatus = itFriend->second.iStatus;
		strncpy(seekDate.lFriend[i].cUserName, itFriend->second.cUserName, NAME_SIZE);
	}

	for (i = 0;i < SEEK_PLAYER_NUM && itRand != m_RandInfoMap.end();++i,++itRand)
	{
		seekDate.lRandPlayer[i].dwUserID = itRand->second.dwUserID;
		seekDate.lRandPlayer[i].iLevel = itRand->second.iLevel;
		seekDate.lRandPlayer[i].iOccupation = itRand->second.iOccupation;
		seekDate.lRandPlayer[i].iStatus = itRand->second.iStatus;
		strncpy(seekDate.lRandPlayer[i].cUserName, itRand->second.cUserName, NAME_SIZE);
	}
	
	for (i = 0;i < SEEK_SEEK_INFO_NUM && itInfo != m_VistorInfoMap.end();++itInfo)
	{
		if (itInfo->second.seekInfo.iType / 100 != pTm->tm_wday)
		{
			continue;
		}
		memcpy(&(seekDate.info[i].vi), &(itInfo->second), sizeof(seekDate.info[i].vi));
		seekDate.info[i].vi.seekInfo.iType %= 100;
		GetVisitorDes(seekDate.info[i].vi, seekDate.info[i].cInfo, iInfoLen);
		if (strlen(seekDate.info[i].cInfo) != 0)
		{
			++i;
		}
	}

	if (m_pUser->m_userPet.GetAllChangID(iPetChangeID, iSize))
	{
		CT_INT32 iPetIndex = 0;
		for (i = 0; i < iSize / sizeof(CT_INT32);++i)
		{
			if (!IsPetKidnap(iPetChangeID[i]))
			{
				seekDate.iPetID[iPetIndex++] = iPetChangeID[i];
			}
		}
	}

	GetTotalInfo(seekDate.tTotal.tDayTotal, sizeof(seekDate.tTotal.tDayTotal), seekDate.tTotal.iDayNum,CT_TRUE);
	GetTotalInfo(seekDate.tTotal.wDayTotal, sizeof(seekDate.tTotal.wDayTotal), seekDate.tTotal.iWeekNum,CT_FALSE);

	memcpy(pBuf, &seekDate, sizeof(seekDate));
	iLen = sizeof(seekDate);
	return CT_TRUE;
}

//获取建筑信息
CT_VOID CCUserSeek::GetBuildData(const CT_INT32 iBuildID,BuildData &bd)
{
	_AUTOMUTEX(m_pMutex);
	BuildDataMap::iterator it=m_BuildDataMap.find(iBuildID);
	if(it != m_BuildDataMap.end())
	{
		memcpy(&bd,&((*it).second),sizeof(bd));
	}
	m_pUser->m_userBuilding.GetBuildingDataEx(iBuildID,bd.bd);
	return;
}

//设置城墙士兵
CT_VOID CCUserSeek::SetWallArmy(CT_INT32 iPos[POST_COUNT/2])
{
	_AUTOMUTEX(m_pMutex);
	memcpy(m_iPos,iPos,sizeof(m_iPos));

	//同步数据库
	MSC_DB_WALL_ARMY dbwa={0};
	dbwa.dwUserID=m_pUser->GetUserID();
	memcpy(dbwa.iPos,m_iPos,sizeof(m_iPos));
	G_SendDBThreadInfo(m_pUser->GetUserID(),MSC_MAIN_DB_SEEK,MSC_SUB_DB_WALL_ARMY,(CT_CHAR*)&dbwa,sizeof(dbwa));
	return;
}

//设置建筑信息
CT_VOID CCUserSeek::SetBuildData(const CT_INT32 iBuildID,BuildData &bd)
{
	_AUTOMUTEX(m_pMutex);
	BuildDataMap::iterator it=m_BuildDataMap.find(iBuildID);
	if(it != m_BuildDataMap.end())
	{
		(*it).second=bd;
	}
	else
	{
		m_BuildDataMap.insert(BuildDataMap::value_type(iBuildID,bd));
	}
	return;
}

CT_VOID CCUserSeek::InsertSeekerBaseInfo(const CMD_G_EnterTown_Others_Suc& enterTown)
{
	_AUTOMUTEX(m_pMutex);

	//memset(&m_Seeker, 0, sizeof(m_Seeker));

	//m_Seeker.dwUserID = enterTown.dwUserID;
	//m_Seeker.iDiamond = enterTown.iDiamond;			
	//m_Seeker.iMoney = enterTown.iMoney;			
	//m_Seeker.iFood = enterTown.iFood;				
	//m_Seeker.iWorker = enterTown.iWorker;			
	//m_Seeker.iMaxWorker = enterTown.iMaxWorker;
	//memcpy(m_Seeker.buildingData, enterTown.building, sizeof(m_Seeker.buildingData));
	//memcpy(m_Seeker.productBuilding, enterTown.productBuilding, sizeof(m_Seeker.productBuilding));
	//memcpy(m_Seeker.upgradeBuilding, enterTown.upgradeBuilding, sizeof(m_Seeker.upgradeBuilding));
}

// 刷新探访界面
CT_VOID CCUserSeek::UpdateSeekedPerson(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 iNum = 0;
	CT_INT32 iRand = 0;
	CT_INT32 iSize = 0;
	CT_INT32 iCount = 0;
	CT_INT32 iTmp = 0;
	CT_INT32 iIndex = 0;
	CT_INT32 iCurrent = time(0);
	CT_INT32 aFriend[100] = {0};
	CT_INT32 aTmpFriend[SEEK_UPDATE_PERSON_NUM] = {0};
	CT_INT32 aTmpRand[SEEK_UPDATE_PERSON_NUM] = {0};
	CMD_G_SeekedPlayerReturn spr = {0};
	MSC_DB_SeekPlayerList spl = {0};
	G_ValueRange vr = {0};
	MasterVariableData mvd = {0};
	MasterData md = {0};
	UserInfoMap allRandPlayerMap = m_AllRandPlayerMap;

	m_FriendInfoMap.clear();
	m_RandInfoMap.clear();

	// 判断刷新冷却时间
	CooldownData cDate = {0};
	m_pUser->m_userBase.GetCooldownData(SEEKLISTUPDATE, cDate);
	if (SEEK_UPDATE_BY_DIAMOND == iType)
	{
		if (!G_GetValueRange(VALUE_TYPE_SEEK_COOL_DIAMOND,&vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::UpdateSeekedPerson G_GetValueRange error.value:%d", VALUE_TYPE_SEEK_COOL_DIAMOND);
			return;
		}

		m_pUser->m_userBase.GetMasterData(md);
		if (md.mvd.stRes.iDiamod < vr.fMaxVal)
		{
			G_SendErrorInfo(m_pUser, ERE_SEEK, ESE_DIAMOND_LESS);
			return;
		}
		memset(&md, 0, sizeof(md));
		mvd.stRes.iDiamod = -vr.fMaxVal;
		m_pUser->m_userBase.AddMastData(mvd, SAVATAG_SEEK_UPDATE);
		m_pUser->m_userBase.UpdateCooldown(SEEKLISTUPDATE, -cDate.leftTime);
	}
	else if (cDate.leftTime + cDate.toggleTime > iCurrent && cDate.leftTime > 0)
	{
		G_SendErrorInfo(m_pUser, ERE_SEEK, ESE_ON_COOLDOWN);
		return;
	}

	// 刷新善访好友
	UserInfoMap::iterator it = m_AllRandPlayerMap.begin();
	for (;it != m_AllRandPlayerMap.end();++it)
	{
		if (m_pUser->m_userFriend.CheckIsFriend(it->first) && !IsSeekGoodwill(it->first))
		{
			aFriend[iNum++] = it->first;
		}
		if (iNum == 100)
		{
			break;
		}
	}

	// 如果人数大于5，随机
	if (iNum >= SEEK_UPDATE_PERSON_NUM)
	{
		for (iCount = 0;iCount < SEEK_UPDATE_PERSON_NUM;++iCount)
		{
			iRand = rand()%iNum;
			while (aFriend[iRand] == 0)
			{
				iRand++;
				if (iRand == iNum)
				{
					iRand = 0;
				}
			}
			aTmpFriend[iCount] = aFriend[iRand];
			aFriend[iRand] = 0;
		}
		for (iCount = 0;iCount < SEEK_UPDATE_PERSON_NUM;++iCount)
		{
			aFriend[iCount] = aTmpFriend[iCount];
		}
		iNum = SEEK_UPDATE_PERSON_NUM;
	}

	// 得到随机好友
	UserInfoMap::iterator itFriend;
	for (CT_INT32 i = 0;i < iNum;++i)
	{
		itFriend = m_AllRandPlayerMap.find(aFriend[i]);
//		if (IsSeekGoodwill(aFriend[i]))
//		{
//			itFriend->second.iStatus = KIDNAP_STATUS_YES;
//		}
//		else
//		{
//			itFriend->second.iStatus = KIDNAP_STATUS_NO;
//		}
		m_FriendInfoMap.insert(UserInfoMap::value_type(itFriend->first, itFriend->second));
		spr.lFriend[i] = itFriend->second;
		spl.aWillPlayer[i] = itFriend->second;
	}

	// 去掉已恶访过的人
	UserInfoMap::iterator itTmp = allRandPlayerMap.begin();
	UserInfoMap::iterator itTmp1;
	for (;itTmp != allRandPlayerMap.end();)
	{
		itTmp1 = itTmp;
		++itTmp;
		if (IsSeekSpite(itTmp1->first))
		{
			allRandPlayerMap.erase(itTmp1);
		}
	}

	// 得到随机玩家
	iSize = allRandPlayerMap.size();
	if (iSize > SEEK_UPDATE_PERSON_NUM)
	{
		for (iCount = 0;iCount < SEEK_UPDATE_PERSON_NUM;++iCount)
		{
			iRand = rand()%iSize;
			for (iTmp = 0;iTmp < iCount;++iTmp)
			{
				if (aTmpRand[iTmp] == iRand)
				{
					++iRand;
					iTmp = -1;
				}
				if (iRand == iSize)
				{
					iRand = 0;
				}
			}
			aTmpRand[iCount] = iRand;
		}
		// 对5个随机数排序
		for (iCount = 0;iCount < SEEK_UPDATE_PERSON_NUM;++iCount)
		{
			for (iTmp = 1;iTmp < SEEK_UPDATE_PERSON_NUM - iCount;++iTmp)
			{
				if (aTmpRand[iTmp] < aTmpRand[iTmp - 1])
				{
					CT_INT32 iTTmp = aTmpRand[iTmp];
					aTmpRand[iTmp] = aTmpRand[iTmp - 1];
					aTmpRand[iTmp - 1] = iTTmp;
				}
			}
		}
		iSize = SEEK_UPDATE_PERSON_NUM;
	}
	else
	{
		for (iCount = 0;iCount < SEEK_UPDATE_PERSON_NUM;++iCount)
		{
			aTmpRand[iCount] = iCount;
		}
	}

	UserInfoMap::iterator itRand = allRandPlayerMap.begin();
	for (iCount = 0;itRand != allRandPlayerMap.end();++itRand,++iCount)
	{
		if (aTmpRand[iIndex] == iCount)
		{
//			if (IsSeekSpite(itRand->second.dwUserID))
//			{
//				itRand->second.iStatus = KIDNAP_STATUS_YES;
//			}
//			else
//			{
//				itRand->second.iStatus = KIDNAP_STATUS_NO;
//			}
			m_RandInfoMap.insert(UserInfoMap::value_type(itRand->first, itRand->second));
			spr.lRandPlayer[iIndex] = itRand->second;
			spl.aSpitePlayer[iIndex] = itRand->second;
			++iIndex;
		}
	}

	// 更新刷新时间
	if (!G_GetValueRange(VALUE_TYPE_SEEK_COOL_TIME, &vr))
	{
		CCLogMeg::getSingleton().WriteError("CCUserSeek::UpdateSeekedPerson G_GetValueRange error.value:%d", VALUE_TYPE_SEEK_COOL_TIME);
		return;
	}
	m_pUser->m_userBase.UpdateCooldown(SEEKLISTUPDATE, vr.fMaxVal);

	// 发送客户端
	m_pUser->m_userBase.GetMasterData(md);
	spr.iType = VALUE_TYPE_DIAMOND;
	spr.iNum = md.mvd.stRes.iDiamod;
	m_pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_PLAYER_RETURN, (CT_CHAR*)&spr, sizeof(spr));

	// 更新数据库
	spl.dwUserID = m_pUser->GetUserID();
	CT_INT32 threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid, MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_LIST_UPDATE, (CT_CHAR*)&spl, sizeof(spl));
	
	allRandPlayerMap.clear();
	
	return;
}

// 是否被善访过
CT_BOOL CCUserSeek::IsSeekGoodwill(CT_DWORD dwFUserID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iNum = m_FriendSeekVector.size();
	for (CT_INT32 i = 0;i < iNum;++i)
	{
		if (m_FriendSeekVector[i] == dwFUserID)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

// 是否被恶访过
CT_BOOL CCUserSeek::IsSeekSpite(CT_DWORD dwFUserID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iNum = m_RandSeekVector.size();
	for (CT_INT32 i = 0;i < iNum;++i)
	{
		if (m_RandSeekVector[i] == dwFUserID)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

// 检测探访次数是否足够
CT_BOOL CCUserSeek::CheckSeekTimes(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	G_ValueRange vr = {0};
	if (iType == SEEK_GOODWILL)
	{
		if (!G_GetValueRange(VALUE_TYPE_GOODWILL_TIMES, &vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::CheckSeekTimes G_GetValueRange error.value:%d",VALUE_TYPE_GOODWILL_TIMES);
			return CT_FALSE;
		}
		
		if (m_SeekData.iSeekWillTimes < vr.fMaxVal)
		{
			return CT_TRUE;
		}
	}
	else if(iType == SEEK_SPITE)
	{
		if (!G_GetValueRange(VALUE_TYPE_SPITE_TIMES, &vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::CheckSeekTimes G_GetValueRange error.value:%d",VALUE_TYPE_SPITE_TIMES);
			return CT_FALSE;
		}

		if (m_SeekData.iSeekSpiteTimes < vr.fMaxVal + m_SeekData.iAddSpiteTimes)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

// 检测被探访次数是否足够
CT_BOOL CCUserSeek::CheckSeekedTimes(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	G_ValueRange vr = {0};
	if (iType == SEEK_GOODWILL)
	{
		if (!G_GetValueRange(VALUE_TYPE_GOODSEEKED_TIMES, &vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::CheckSeekTimes G_GetValueRange error.value:%d",VALUE_TYPE_GOODSEEKED_TIMES);
			return CT_FALSE;
		}

		if (m_SeekData.iSeekedWillTimes < vr.fMaxVal)
		{
			return CT_TRUE;
		}
	}
	else if(iType == SEEK_SPITE)
	{
		if (!G_GetValueRange(VALUE_TYPE_BADSEEKED_TIMES, &vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::CheckSeekTimes G_GetValueRange error.value:%d",VALUE_TYPE_BADSEEKED_TIMES);
			return CT_FALSE;
		}

		if (m_SeekData.iSeekedSpiteTimes < vr.fMaxVal)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

// 得到已探访次数
CT_INT32 CCUserSeek::GetSeekTimes(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	if (iType == SEEK_GOODWILL)
	{
		return m_SeekData.iSeekWillTimes;
	}
	if (iType == SEEK_SPITE)
	{
		return m_SeekData.iSeekSpiteTimes;
	}
	return -1;
}

CT_VOID CCUserSeek::SeekOneTimes(CT_INT32 iType, CT_DWORD dwFUserID)
{
	_AUTOMUTEX(m_pMutex);
	UserInfoMap::iterator it;
	MasterData md = {0};

	m_pUser->m_userBase.GetMasterData(md);

	if (0 != m_VistorInfo[0].seekInfo.iType)
	{
		m_pUser->m_userSeek.SeekVistor();
	}

	memset(&m_VistorInfo, 0, sizeof(m_VistorInfo));
	memset(&(m_SeekData.iPresentStatus), 0, sizeof(m_SeekData.iPresentStatus));

	for (CT_INT32 i = 0;i < SEEK_DOTYPE_NUM;++i)
	{
		m_VistorInfo[i].dwUserID = m_pUser->GetUserID();
		strncpy(m_VistorInfo[i].cUserName, md.cNickName, NAME_SIZE);
		m_VistorInfo[i].iOccupation = md.iOccupation;
		m_VistorInfo[i].iLevel = md.mvd.stGrade.iLevel;
	}
	
	m_SeekData.dwSeekID = dwFUserID;
	m_SeekData.iType = iType;

	if (iType == SEEK_GOODWILL)
	{
		++(m_SeekData.iSeekWillTimes);
		m_FriendSeekVector.push_back(dwFUserID);
		it = m_FriendInfoMap.find(dwFUserID);
		if (it == m_FriendInfoMap.end())
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::SeekOneTimes friend not in list,fuid:%d,uid:%d",
				dwFUserID,m_pUser->GetUserID());
			return;
		}
		it->second.iStatus = HAD_SEEK;
	}
	else if (iType == SEEK_SPITE)
	{
		++(m_SeekData.iSeekSpiteTimes);
		m_RandSeekVector.push_back(dwFUserID);
		it = m_RandInfoMap.find(dwFUserID);
		if (it == m_RandInfoMap.end())
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::SeekOneTimes friend not in list,fuid:%d,uid:%d",
				dwFUserID,m_pUser->GetUserID());
			return;
		}
		it->second.iStatus = HAD_SEEK;
	}

	// 发送客户端
	G_ChestByLevel chest = {0};
	m_SeekData.iLevel = md.mvd.stGrade.iLevel;
	G_GetChestByLevel(iType, md.mvd.stGrade.iLevel, &chest);

	if (SEEK_GOODWILL == iType)
	{
		CMD_G_SeekGoodWillSuc sgw = {0};
		sgw.iTimes = m_SeekData.iSeekWillTimes;
		sgw.dwFUserID = dwFUserID;
		for (CT_INT32 i = 0;i < SEEK_PRESENT_ITEM_NUM;++i)
		{
			G_GetCostRewardGroup(chest.chest[i].iChestID, &(sgw.costReward[i]));
			m_SeekData.iPresentStatus[i] = chest.chest[i].iChestID;
		}
		m_pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_GOODWILL_SUC, (CT_CHAR*)&sgw, sizeof(sgw));
	}
	
	// 更新到数据库
	MSC_DB_SeekData date = {0};
	date.dwUserID = m_pUser->GetUserID();
	date.seekData = m_SeekData;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_DATA_UPDATE, (CT_CHAR*) &date, sizeof(date));

	// 被访者状态
	MSC_DB_Had_SeekedPlayer player = {0};
	player.dwUserID = m_pUser->GetUserID();
	player.dwFUserID = dwFUserID;
	player.iType = iType;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEKED_PLAYER_UPDATE, (CT_CHAR*) &player, sizeof(player));

	// 数据库日志
	MSC_DB_SeekVistorInfo vInfo = {0};
	vInfo.dwUserID = dwFUserID;
	vInfo.vistorInfo.dwUserID = m_pUser->GetUserID();
	vInfo.vistorInfo.seekInfo.iType = iType;
	CT_INT32 threadid =  GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,  MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SEEKDATA, (CT_CHAR*)&vInfo, sizeof(vInfo));

	//清除建筑信息
	m_BuildDataMap.clear();
	m_SpeedUpBidVector.clear();

	//判断探访任务
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_SEEK_TIMES;
	taskSysInfo.arrayParam[0] = iType;
	taskSysInfo.iProgress = 1;
	m_pUser->m_userTask.DisposeTask(taskSysInfo);

	//发送黑店
	m_pUser->m_userSeek.SendBlacketMarket();

	return;
}

CT_VOID CCUserSeek::SeekedOneTimes(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	if (iType == SEEK_GOODWILL)
	{
		++(m_SeekData.iSeekedWillTimes);
	}
	else if (iType == SEEK_SPITE)
	{
		++(m_SeekData.iSeekedSpiteTimes);
	}

	// 更新到数据库
	MSC_DB_Seeked_TimesMinus seeked = {0};
	seeked.dwFUserID = m_pUser->GetUserID();
	seeked.iType = iType;
	CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_SEEK,MSC_SUB_SEEKED_TIMES_MINUS,(CT_CHAR*)&seeked,sizeof(seeked));
	return;
}

// 判断好友是否在探访界面上
CT_BOOL CCUserSeek::IsInSeekedList(CT_DWORD dwUserID)
{
	_AUTOMUTEX(m_pMutex);
	if (m_FriendInfoMap.find(dwUserID) == m_FriendInfoMap.end()
		&& m_RandInfoMap.find(dwUserID) == m_RandInfoMap.end())
	{
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到礼品ID
CT_INT32 CCUserSeek::GetSeekPresent(G_ChestByLevel& chest)
{
	_AUTOMUTEX(m_pMutex);
	if (G_GetChestByLevel(SEEK_GOODWILL, m_SeekData.iLevel, &chest))
	{
		return 1;
	}
	return 0;
}

//得到已获得的礼物个数
CT_INT32 CCUserSeek::GetPresentNum()
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iNum = 0;
	CT_INT32 iNow = time(0);

	G_ValueRange vr = {0};
	G_GetValueRange(VALUE_TYPE_UPDATEPRESENT_TIME, &vr);

	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM && m_SeekData.present[i].iChestID != 0;++i)
	{
		if (iNow - m_SeekData.present->iGetTime < vr.fMaxVal)
		{
			++iNum;
		}
		else
		{
			for (CT_INT32 j = i;j < SEEKED_PRESENT_NUM - 1;++j)
			{
				m_SeekData.present[j] = m_SeekData.present[j + 1];
			}
		}
	}

	return iNum;
}

//得到礼物
CT_VOID CCUserSeek::GetPresent(CT_INT32 iChestID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iTime = time(0);

	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		if (m_SeekData.present[i].iChestID == 0)
		{
			m_SeekData.present[i].iChestID = iChestID;
			m_SeekData.present[i].iGetTime = iTime;

			// 更新到数据库
			MSC_DB_SeekPresent present = {0};
			present.dwUserID = m_pUser->GetUserID();
			present.present = m_SeekData.present[i];
			CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_UPDATE, (CT_CHAR*) &present, sizeof(present));
			return;
		}
	}
}

//插入探访基本信息
CT_VOID CCUserSeek::InsertSeekData(SeekData seekdata)
{
	_AUTOMUTEX(m_pMutex);

	memset(&m_SeekData, 0, sizeof(m_SeekData));
	memcpy(&m_SeekData, &seekdata, sizeof(m_SeekData));
}

//插入战报
CT_VOID CCUserSeek::InsertFightingData(SeekFightingData &sfd,CT_BOOL bSave/*=CT_FALSE*/)
{
	_AUTOMUTEX(m_pMutex);
	std::string str((CT_CHAR*)sfd.ucIndex);
	SeekFightingDataMap::iterator it=m_SeekFightingMap.find(str);
	if(it != m_SeekFightingMap.end())
	{
		(*it).second=sfd;
	}
	else
		m_SeekFightingMap.insert(SeekFightingDataMap::value_type(str,sfd));

	if(bSave)
	{
		//数据库保存
		G_SendDBThreadInfo(m_pUser->GetUserID(),MSC_MAIN_DB_SEEK,MSC_SUB_DB_SEEK_FIGHTING_RECORD,(CT_CHAR*)&sfd,sizeof(sfd));
	}
	return;
}

// 更新探访信息
CT_VOID CCUserSeek::UpdateSeekData(SeekData& sd)
{
	_AUTOMUTEX(m_pMutex);

	m_SeekData = sd;

	MasterVariableData mvd={0};
	mvd.stRes.iWorker-=sd.iFarmarNum;
	m_pUser->m_userBase.AddMastData(mvd,SAVATAG_KIDNAP);

	// 更新到数据库
	MSC_DB_SeekData date = {0};
	date.dwUserID = m_pUser->GetUserID();
	date.seekData = m_SeekData;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_DATA_UPDATE, (CT_CHAR*) &date, sizeof(date));
	return;
}

// 插入战旗
CT_VOID CCUserSeek::InsertTeamFlag(TeamSeek& ts)
{
	_AUTOMUTEX(m_pMutex);

	m_TeamSeekMap.insert(TeamSeekMap::value_type(Get64Int(ts.dwUserID, ts.iType), ts));

	// 更新到数据库
	MSC_DB_SeekTeamFlag tFlag = {0};
	tFlag.dwUserID = m_pUser->GetUserID();
	tFlag.teamFlag = ts;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_TEAM_FLAG_UPDATE, (CT_CHAR*) &tFlag, sizeof(tFlag));

}


// 拔掉战旗
CT_VOID CCUserSeek::PullTeamFlag(const CT_DWORD dwUserID,const CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);

	if(iType == SEEK_FLAG_SAINTESS)
	{
		TeamSeekMap::iterator it=m_TeamSeekMap.find(G_Combine64(dwUserID,iType));
		if(it != m_TeamSeekMap.end())
		{
			m_TeamSeekMap.erase(it);
		}
	}
	else
	{
		m_TeamSeekMap.clear();
	}

	// 更新到数据库
	MSC_DB_SeekTeamFlag tFlag = {0};
	tFlag.dwUserID = m_pUser->GetUserID();
	tFlag.teamFlag.dwUserID=dwUserID;
	tFlag.teamFlag.iType=iType;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_TEAM_FLAG_DELETE, (CT_CHAR*) &tFlag, sizeof(tFlag));

	return;
}

// 更新人质
CT_VOID CCUserSeek::UpdateKidnap(CT_INT32 iType, KidnapInfo& kidnap)
{
	_AUTOMUTEX(m_pMutex);

	KidnapMap::iterator it ;
	if (SEEK_KIDNAPTYPE_SAINTESS == iType)
	{
		it = m_KidnapGirlsMap.find(kidnap.owerUserId);
		if (it == m_KidnapGirlsMap.end())
		{
			m_KidnapGirlsMap.insert(KidnapMap::value_type(kidnap.owerUserId, kidnap));
		}
		else
		{
			it->second.Num = kidnap.Num;
		}
		
	}
	else if (SEEK_KIDNAPTYPE_WORKER == iType)
	{
		it = m_kidnapWorkersMap.find(kidnap.owerUserId);
		if (it == m_kidnapWorkersMap.end())
		{
			m_kidnapWorkersMap.insert(KidnapMap::value_type(kidnap.owerUserId, kidnap));
		}
		else
		{
			it->second.Num = kidnap.Num;
		}
	}
	else if (SEEK_KIDNAPTYPE_PET == iType)
	{
		KidnapMap::iterator itPet;
		itPet = m_PetKidnapMap.find(kidnap.owerUserId);
		if (itPet == m_PetKidnapMap.end())
		{
			m_PetKidnapMap.insert(KidnapMap::value_type(kidnap.owerUserId, kidnap));
		}
		else
		{
			itPet->second.iPetChange = kidnap.iPetChange;
		}
	}
	
	// 更新到数据库
	MSC_DB_SeekKidnap sk = {0};
	sk.dwUserID = m_pUser->GetUserID();
	sk.iType = iType;
	sk.kidnap = kidnap;
	if (SEEK_KIDNAPTYPE_PET == iType)
	{
		sk.kidnap.Num = sk.kidnap.iPetChange;
	}
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_KIDNAP_UPDATE, (CT_CHAR*) &sk, sizeof(sk));
	return;
}

// 删除人质
CT_VOID CCUserSeek::DeleteKidnap(CT_INT32 iType, KidnapInfo& kidnap)
{
	_AUTOMUTEX(m_pMutex);

	KidnapMap::iterator it;
	if (SEEK_KIDNAPTYPE_SAINTESS == iType)
	{
		it = m_KidnapGirlsMap.find(kidnap.owerUserId);
		if (it != m_KidnapGirlsMap.end())
		{
			it->second.Num -= kidnap.Num;
			if (it->second.Num == 0)
			{
				m_KidnapGirlsMap.erase(it);
			}
		}
	}
	else if (SEEK_KIDNAPTYPE_WORKER == iType)
	{
		it = m_kidnapWorkersMap.find(kidnap.owerUserId);
		if (it != m_kidnapWorkersMap.end())
		{
			it->second.Num -= kidnap.Num;
			if (it->second.Num == 0)
			{
				m_kidnapWorkersMap.erase(it);
			}
		}
	}
	else if (SEEK_KIDNAPTYPE_PET == iType)
	{
		it = m_PetKidnapMap.find(kidnap.owerUserId);
		if (it != m_PetKidnapMap.end())
		{
			m_PetKidnapMap.erase(it);
		}
	}

	// 更新到数据库
	MSC_DB_SeekKidnap sk = {0};
	sk.dwUserID = m_pUser->GetUserID();
	sk.iType = iType;
	sk.kidnap = kidnap;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_KIDNAP_DELETE, (CT_CHAR*) &sk, sizeof(sk));
	return;
}

// 得到探访界面访客ID
CT_BOOL CCUserSeek::GetSeekListPlayer(CT_INT32 iType, CT_CHAR* pBuf, CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iBufLen = 0;
	CT_INT32 i = 0;
	UserInfoMap::iterator it;

	if (iLen < SEEK_PLAYER_NUM * sizeof(UserInfo))
	{
		return CT_FALSE;
	}

	if (SEEK_GOODWILL == iType)
	{
		it = m_FriendInfoMap.begin();
		for (i = 0;it != m_FriendInfoMap.end() && i < SEEK_PLAYER_NUM;++it,++i)
		{
			memcpy(pBuf + iBufLen, &(it->second), sizeof(UserInfo));
			iBufLen += sizeof(UserInfo);
		}
	}
	else
	{
		it = m_RandInfoMap.begin();
		for (i = 0;it != m_RandInfoMap.end() && i < SEEK_PLAYER_NUM;++it,++i)
		{
			memcpy(pBuf + iBufLen, &(it->second), sizeof(UserInfo));
			iBufLen += sizeof(UserInfo);
		}
	}
	return CT_TRUE;
}

CT_VOID CCUserSeek::AddVistorInfo(VistorInfo& vi)
{
	_AUTOMUTEX(m_pMutex);

	time_t t = time(0);
	tm* pTm = localtime(&t);

	if (0 == vi.seekInfo.iType / 100)
	{
		vi.seekInfo.iType = pTm->tm_wday * 100 + vi.seekInfo.iType;
	}

	VistorInfoMap::iterator it = m_VistorInfoMap.find(Get64Int(vi.seekInfo.iType, vi.dwUserID));
	if (it != m_VistorInfoMap.end())
	{
		m_VistorInfoMap.erase(it);
	}
	m_VistorInfoMap.insert(VistorInfoMap::value_type(Get64Int(vi.seekInfo.iType, vi.dwUserID), vi));

	return;
}

CT_VOID CCUserSeek::AddSeekInfo(VistorInfo& vi)
{
	_AUTOMUTEX(m_pMutex);

	time_t t = time(0);
	tm* pTm = localtime(&t);

	/*if (0 == vi.seekInfo.iType / 100)
	{
		vi.seekInfo.iType = pTm->tm_wday * 100 + vi.seekInfo.iType;
	}*/

	VistorInfoMap::iterator it = m_SeekInfoMap.find(Get64Int(vi.seekInfo.iType, vi.dwUserID));
	if (it != m_SeekInfoMap.end())
	{
		m_SeekInfoMap.erase(it);
	}
	m_SeekInfoMap.insert(VistorInfoMap::value_type(Get64Int(vi.seekInfo.iType, vi.dwUserID), vi));

	return;
}

// 设置当前探访信息
CT_VOID CCUserSeek::SetVistorInfo(VistorInfo vi)
{
	_AUTOMUTEX(m_pMutex);

	time_t t = time(0);
	tm* pTm = localtime(&t);

	G_Building gb = {0};
	if (SEEK_TYPE_UPSPEED == vi.seekInfo.iType || SEEK_TYPE_ADDPRODUCT == vi.seekInfo.iType)
	{
		G_GetBuildingData(vi.seekInfo.iID[0], &gb);
		vi.seekInfo.iID[0] = gb.iBT;
	}
	else if (SEEK_TYPE_PLUNDER == vi.seekInfo.iType)
	{
		G_GetBuildingData(vi.iPlunderBID[0], &gb);
		vi.iPlunderBID[0] = gb.iBT;
	}
	
	vi.seekInfo.iType = pTm->tm_wday * 100 + vi.seekInfo.iType;

	for (CT_INT32 j = 0;j < SEEK_DOTYPE_NUM;++j)
	{
		if (0 == m_VistorInfo[j].seekInfo.iType)
		{
			m_VistorInfo[j].seekInfo.iType = vi.seekInfo.iType;
			for (CT_INT32 ii = 0;ii < TEAM_RES_CNT;++ii)
			{
				m_VistorInfo[j].seekInfo.iID[ii] = vi.seekInfo.iID[ii];
				m_VistorInfo[j].seekInfo.iNum[ii] = vi.seekInfo.iNum[ii];
			}
			
			if (CT_TRUE == vi.bIsKidnap)
			{
				m_VistorInfo[j].bIsKidnap = vi.bIsKidnap;
			}

			for (CT_INT32 k = 0;k < SEEK_PRESENT_ITEM_NUM;++k)
			{
				m_VistorInfo[j].present[k] = vi.present[k];
			}
			m_VistorInfo[j].iPlunderBID[0] = vi.iPlunderBID[0];
			break;
		}
		if (m_VistorInfo[j].seekInfo.iType == vi.seekInfo.iType)
		{
			for (CT_INT32 i = 0;i < TEAM_RES_CNT;++i)
			{
				for (CT_INT32 ii = 0;ii < TEAM_RES_CNT;++ii)
				{
					if (vi.seekInfo.iID[i] == m_VistorInfo[j].seekInfo.iID[ii])
					{
						m_VistorInfo[j].seekInfo.iNum[ii] += vi.seekInfo.iNum[i];
						break;
					}
					if (m_VistorInfo[j].seekInfo.iID[ii] == 0)
					{
						m_VistorInfo[j].seekInfo.iID[ii] = vi.seekInfo.iID[i];
						m_VistorInfo[j].seekInfo.iNum[ii] = vi.seekInfo.iNum[i];
						break;
					}
				}
			}

			CT_INT32 iIndex = 0;
			for (CT_INT32 k = 0;k < SEEK_PRESENT_ITEM_NUM;++k)
			{
				if (m_VistorInfo[j].present[k] == 0)
				{
					m_VistorInfo[j].present[k] = vi.present[iIndex++];
				}
			}

			for (CT_INT32 iBT = 0;iBT < SEEK_PLUNDER_BUILDTYPE;++iBT)
			{
				if (m_VistorInfo[j].iPlunderBID[iBT] == 0)
				{
					m_VistorInfo[j].iPlunderBID[iBT] = vi.iPlunderBID[0];
					break;
				}
				if (m_VistorInfo[j].iPlunderBID[iBT] == vi.iPlunderBID[0])
				{
					break;
				}
			}

			if (CT_TRUE == vi.bIsKidnap)
			{
				m_VistorInfo[j].bIsKidnap = vi.bIsKidnap;
			}

			break;
		}
	}
}

// 得到当前已探访信息(访问别人）
CT_BOOL CCUserSeek::GetCurrentVistorInfo(VistorInfo* pvi, CT_INT32& iLen)
{
	_AUTOMUTEX(m_pMutex);

	if (iLen < SEEK_DOTYPE_NUM * sizeof(VistorInfo))
	{
		return CT_FALSE;
	}

	CT_INT32 i = 0;
	for (;i < SEEK_DOTYPE_NUM;++i)
	{
		if (m_VistorInfo[i].seekInfo.iType == 0)
		{
			break;
		}
		pvi[i] = m_VistorInfo[i];
	}

	iLen = i * sizeof(VistorInfo);
	return CT_TRUE;
}

// 领取礼物
CT_VOID CCUserSeek::UsePresent()
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 iStatus = 0;
	CT_INT32 i = 0;
	CMD_G_GetPresentSuc gps = {0};
	MasterVariableData mvd = {0};
	RewardCost rc = {0};

	m_pUser->m_userBase.GetMasterData(mvd);

	// 判断是否有礼物领取
	if (m_SeekData.present[0].iChestID == 0)
	{
		// 领取失败
		m_pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_APPLY_PRESENT_FAIL);
		return;
	}
	// 判断背包
	for (i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		if (0 == m_SeekData.present[i].iChestID)
		{
			break;
		}

		FakeProBase fb={0};
		memset(&rc, 0, sizeof(rc));
		iStatus = G_GetCostReward(m_SeekData.present[i].iChestID,m_pUser,ITEM_OP_MODE_CHECK,SAVATAG_GET_PRESENT,rc,fb);
		if (LOOT_BAG_FULL == iStatus || LOOT_ELM_FULL == iStatus)
		{
			G_SendErrorInfo(m_pUser, ERE_SYSTEM, EEI_BUG_FULL);
		}

		if (ENUM_CREC_RES_SUC != iStatus)
		{
			// 领取失败
			m_pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_APPLY_PRESENT_FAIL);
			return;
		}
	}

	// 发放礼物至背包
	for (i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		if (0 == m_SeekData.present[i].iChestID)
		{
			break;
		}

		FakeProBase fb={0};
		memset(&rc, 0, sizeof(rc));
		G_GetCostReward(m_SeekData.present[i].iChestID,m_pUser,ITEM_OP_MODE_EXEC,SAVATAG_GET_PRESENT,rc,fb);

		for (CT_INT32 k = 0;k < TEAM_RES_CNT;++k)
		{
			for (CT_INT32 j = 0;j < TEAM_RES_CNT;++j)
			{
				if (rc.iUseResType[k] == gps.iResType[j])
				{
					gps.iResNum[j] += rc.iUseResCount[k];
					break;
				}
				if (gps.iResType[j] == 0)
				{
					gps.iResType[j] = rc.iUseResType[k];
					gps.iResNum[j] = rc.iUseResCount[k];
					break;
				}
			}
		}
	}

	memset(m_SeekData.present, 0, sizeof(m_SeekData.present));
	
	for (CT_INT32 j = 0;j < TEAM_RES_CNT;++j)
	{
		G_ChangeResType(mvd, gps.iResType[j], gps.iResNum[j]);
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_APPLY_PRESENT_SUC, (CT_CHAR*)&gps, sizeof(gps));

	// 更新到数据库
	MSC_DB_SeekPresent present = {0};
	present.dwUserID = m_pUser->GetUserID();
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_DEL, (CT_CHAR*) &present, sizeof(present));
	return;
}

// 检测建筑是否可加速
CT_BOOL CCUserSeek::CheckBuildingSpeedUp(CT_INT32 iBID)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 iSize = m_SpeedUpBidVector.size();
	for (CT_INT32 i = 0;i < iSize;++i)
	{
		if (m_SpeedUpBidVector[i] == iBID)
		{
			return CT_FALSE;
		}
	}
	return CT_TRUE;
}

// 检测礼物是否可以赠送
CT_BOOL CCUserSeek::CheckPresentStatus(CT_INT32* pPresentID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iTmp[SEEK_PRESENT_ITEM_NUM] = {0};

	for (CT_INT32 i = 0;i < SEEK_PRESENT_ITEM_NUM;++i)
	{
		iTmp[i] = m_SeekData.iPresentStatus[i];
	}

	for (CT_INT32 j = 0;j < SEEK_PRESENT_ITEM_NUM;++j)
	{
		if (pPresentID[j] == 0)
		{
			break;
		}

		CT_INT32 k = 0;
		for (;k < SEEK_PRESENT_ITEM_NUM;++k)
		{
			if (iTmp[k] == pPresentID[j])
			{
				iTmp[k] = 0;
				break;
			}
		}
		if (SEEK_PRESENT_ITEM_NUM == k)
		{
			return CT_FALSE;
		}
	}

	return CT_TRUE;
}

// 赠送礼物，记录ID
CT_VOID CCUserSeek::SendPresent(CT_INT32 iPresentID)
{
	_AUTOMUTEX(m_pMutex);

	for (CT_INT32 i = 0;i < SEEK_PRESENT_ITEM_NUM;++i)
	{
		if (m_SeekData.iPresentStatus[i] == iPresentID)
		{
			m_SeekData.iPresentStatus[i] = 0;
			break;
		}
	}
}

// 更新领主礼物
CT_VOID CCUserSeek::UpdatePresent()
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iTime = time(0);
	G_ValueRange vr = {0};
	if (!G_GetValueRange(VALUE_TYPE_UPDATEPRESENT_TIME, &vr))
	{
		CCLogMeg::getSingleton().WriteError("G_GetValueRange error, value:%d", VALUE_TYPE_UPDATEPRESENT_TIME);
	}

	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM;)
	{
		if (m_SeekData.present[i].iChestID == 0)
		{
			break;
		}

		if (iTime - m_SeekData.present[i].iGetTime >= vr.fMaxVal)
		{
			memset(&m_SeekData.present[i], 0, sizeof(m_SeekData.present[i]));

			for (CT_INT32 j = i;j < SEEKED_PRESENT_NUM -1;++j)
			{
				m_SeekData.present[j] = m_SeekData.present[j+1];
			}
		}
		else
		{
			++i;
		}
	}
	return;
}

// 得到所有礼品
CT_BOOL CCUserSeek::GetAllPresent(CT_CHAR* pBuf, CT_INT32& iLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 iNum = 0;
	CT_INT32 iSize = sizeof(SeekPresent);
	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		if (m_SeekData.present[i].iChestID == 0)
		{
			break;
		}

		if (iLen < iSize)
		{
			return CT_FALSE;
		}

		memcpy(pBuf + iNum, &(m_SeekData.present[i]), sizeof(SeekPresent));
		++iNum;
		iSize += sizeof(SeekPresent);
	}

	iLen = iSize - sizeof(SeekPresent);
	return CT_TRUE;
}

//获得战报信息
CT_BOOL CCUserSeek::GetFightingData(CT_UCHAR *pIndex,SeekFightingData &sfd)
{
	_AUTOMUTEX(m_pMutex);
	std::string str((CT_CHAR*)pIndex);
	SeekFightingDataMap::iterator it=m_SeekFightingMap.find(str);
	if(m_SeekFightingMap.end() == it)
	{
		return CT_FALSE;
	}

	memcpy(&sfd,&((*it).second),sizeof(sfd));
	return CT_TRUE;
}

// 回营
CT_VOID CCUserSeek::SeekVistor()
{
	_AUTOMUTEX(m_pMutex);

	if (m_VistorInfo[0].seekInfo.iType == 0)
	{
		return;
	}

	// 同步到数据库
	for (CT_INT32 i = 0;i < SEEK_DOTYPE_NUM;++i)
	{
		if (m_VistorInfo[i].seekInfo.iType == 0)
		{
			break;
		}

		MSC_DB_SeekVistorInfo svi = {0};
		svi.dwUserID = m_SeekData.dwSeekID;
		svi.vistorInfo = m_VistorInfo[i];
		CT_INT32 threadID =  GAME_THREAD_DATABASE + m_SeekData.dwSeekID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID, MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_VISTORINFO, (CT_CHAR*)&svi, sizeof(svi));

		CT_INT32 threadid =  GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,  MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SEEKDATA, (CT_CHAR*)&svi, sizeof(svi));
	}

	memset(&m_VistorInfo, 0, sizeof(m_VistorInfo));
	m_SeekData.dwSeekID = 0;
	m_SeekData.iType = 0;
}

CT_VOID CCUserSeek::GetVisitorDes(VistorInfo& vi, CT_CHAR* pInfo,CT_INT32 iSize)
{
	_AUTOMUTEX(m_pMutex);

	RollBroadcast rb[8] = {0};
	CT_INT32 iBroadcastType = 1;
	CT_INT32 iBroadcastTime = 1;
	CT_CHAR cBroadcastMes[CHAT_LEN*2] = {0};
	CT_INT32 iMesLen = 0;
	CT_INT32 iBroadcastLen=CHAT_LEN*2-1;
	CT_INT32 iType = vi.seekInfo.iType % 100;

	switch (iType)
	{
	case SEEK_TYPE_PRESENT:
		{
			G_CostRewardGroup crg = {0};
			CT_INT32 i = 0;
			
			G_GetCostRewardGroup(vi.present[0], &crg);
			rb[0].SetValue(vi.dwUserID,vi.cUserName);
			if (crg.res[0].iID != 0)
			{
				rb[1].SetValue(1, "");
				rb[2].SetValue(crg.res[0].iID,"");
			}
			else
			{
				rb[1].SetValue(2, "");
				rb[2].SetValue(crg.item[0].iID, "");
			}

			G_GetRollBroadecastData(141,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			break;
		}
	case SEEK_TYPE_UPSPEED:
		{
			rb[0].SetValue(vi.dwUserID, vi.cUserName);
			G_GetRollBroadecastData(143,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			break;
		}
	case SEEK_TYPE_PLUNDER:
		{
			if (vi.seekInfo.iID[0] == 0)
			{
				rb[0].SetValue(vi.dwUserID, vi.cUserName);
				G_GetRollBroadecastData(140,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			}
			else if (vi.seekInfo.iID[1] == 0)
			{
				rb[0].SetValue(vi.dwUserID, vi.cUserName);
				rb[1].SetValue(vi.seekInfo.iNum[0],"");
				rb[2].SetValue(1,"");
				rb[3].SetValue(vi.seekInfo.iID[0],"");
				G_GetRollBroadecastData(139,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			}
			else
//			if (vi.seekInfo.iID[1] != 0)
			{
//				CT_CHAR cTmp[128] = {0};
//				CT_INT32 iTmpLen = 128;
//				memset(rb, 0, sizeof(rb));
				rb[0].SetValue(vi.dwUserID, vi.cUserName);
				rb[1].SetValue(vi.seekInfo.iNum[0],"");
				rb[2].SetValue(1,"");
				rb[3].SetValue(vi.seekInfo.iID[0],"");
				rb[4].SetValue(vi.seekInfo.iNum[1],"");
				rb[5].SetValue(1,"");
				rb[6].SetValue(vi.seekInfo.iID[1],"");
				
				G_GetRollBroadecastData(139,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
//				G_GetRollBroadecastData(146,cTmp,iTmpLen,(CT_CHAR*)rb,sizeof(rb));
//				sprintf(cBroadcastMes, "%s,%s", cBroadcastMes, cTmp);
//				iBroadcastLen += iTmpLen;
			}
			break;
		}
	case SEEK_TYPE_KIDNAP:
		{
#if 0
			rb[0].SetValue(vi.dwUserID, vi.cUserName);
			
			if (0 != vi.seekInfo.iID[1] && 0 != vi.seekInfo.iNum[0] && 0 != vi.seekInfo.iNum[1])
			{
				G_GetRollBroadecastData(146,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			}
			else if ((SEEK_KIDNAPTYPE_SAINTESS == vi.seekInfo.iID[0] && 0 != vi.seekInfo.iNum[0])
				|| (SEEK_KIDNAPTYPE_SAINTESS == vi.seekInfo.iID[1] && 0 != vi.seekInfo.iNum[1]))
			{
				G_GetRollBroadecastData(144,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			}
			else if ((SEEK_KIDNAPTYPE_WORKER == vi.seekInfo.iID[0] && 0 != vi.seekInfo.iNum[0])
				|| (SEEK_KIDNAPTYPE_WORKER == vi.seekInfo.iID[1] && 0 != vi.seekInfo.iNum[1]))
			{
				G_GetRollBroadecastData(145,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			}
			/*else
			{
				G_GetRollBroadecastData(152,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			}*/
#endif
			CT_CHAR cTmp[128] = {0};
			CT_INT32 iTmpLen = sizeof(cTmp);
			CT_CHAR cTmp1[128] = {0};
			CT_INT32 iTmp1Len = sizeof(cTmp1);
			CT_BOOL bFirst = CT_TRUE;
			G_PetAddition pa = {0};

			rb[0].SetValue(vi.dwUserID, vi.cUserName);
			for (CT_INT32 iK = 0;iK < TEAM_RES_CNT;++iK)
			{
				if (0 == vi.seekInfo.iNum[iK])
				{
					continue;
				}

				if (bFirst)
				{
					G_GetRollBroadecastData(162, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
					memset(rb, 0, sizeof(rb));
					bFirst = CT_FALSE;
				}
				else
				{
					G_GetRollBroadecastData(166, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
				}

				if (vi.seekInfo.iID[iK] == SEEK_KIDNAPTYPE_PET)
				{
					G_GetPetAddition(vi.seekInfo.iNum[iK], &pa);
					strncpy(cTmp1, pa.cName, sizeof(cTmp1));
				}
				else
				{
					G_GetRollBroadecastData(162 + vi.seekInfo.iID[iK], cTmp1, iTmp1Len, (CT_CHAR*)rb, sizeof(rb));
				}
				sprintf(cBroadcastMes, "%s%s%s",cBroadcastMes, cTmp, cTmp1);
				memset(cTmp, 0, sizeof(cTmp));
				memset(cTmp1, 0, sizeof(cTmp1));
			}
			break;
		}
	case SEEK_TYPE_ADDPRODUCT:
		{
			rb[0].SetValue(vi.dwUserID, vi.cUserName);
			G_GetRollBroadecastData(142,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
			break;
		}
	case SEEK_TYPE_RESCUE:
		{
			rb[0].SetValue(vi.dwUserID, vi.cUserName);
			if (WINER_LIFT == vi.seekInfo.iNum[0])
			{
				G_GetRollBroadecastData(147,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
				break;
			}
			else
			{
				G_GetRollBroadecastData(148,cBroadcastMes,iBroadcastLen,(CT_CHAR*)rb,sizeof(rb));
				break;
			}
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCUserSeek::GetVisitorDes type error.type:%d", vi.seekInfo.iType);
			break;
		}
	}

	memcpy(pInfo, cBroadcastMes, iSize);
	return;
}

// 更新自己的访客记录
CT_VOID CCUserSeek::UpdateSeekVisitorInfo(CT_INT32 iType, const VistorInfo& vi)
{
	_AUTOMUTEX(m_pMutex);

	time_t t = time(0);
	tm* pTm = localtime(&t);
	CT_INT32 iTmpType = iType + pTm->tm_wday * 100;

	VistorInfoMap::iterator it = m_VistorInfoMap.find(Get64Int(iTmpType, vi.dwUserID));
	if (it != m_VistorInfoMap.end())
	{
		it->second = vi;
	}

	// 更新到数据库
	MSC_DB_SeekVistorInfo svi = {0};
	svi.dwUserID = m_pUser->GetUserID();
	svi.vistorInfo = vi;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID, MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_VISTORINFO, (CT_CHAR*)&svi, sizeof(svi));
	return;
}

// 0点更新
CT_VOID CCUserSeek::UpdateAllSeekData()
{
	_AUTOMUTEX(m_pMutex);
	m_TeamSeekMap.clear();
	m_KidnapGirlsMap.clear();
	m_kidnapWorkersMap.clear();
	m_VistorInfoMap.clear();
	m_FriendSeekVector.clear();
	m_RandSeekVector.clear();
	m_SeekFightingMap.clear();
	m_PetKidnapMap.clear();
	m_SeekData.iSeekWillTimes = 0;
	m_SeekData.iSeekSpiteTimes = 0;
	m_SeekData.iSeekedWillTimes = 0;
	m_SeekData.iSeekedSpiteTimes = 0;
	m_SeekData.iRescueNum = 0;
	m_SeekData.iWomanNum = 0;
	m_SeekData.iFarmarNum = 0;

	if (SEEK_GOODWILL == m_SeekData.iType)
	{
		m_FriendSeekVector.push_back(m_SeekData.dwSeekID);
	}
	else if (SEEK_SPITE == m_SeekData.iType)
	{
		m_RandSeekVector.push_back(m_SeekData.dwSeekID);
	}

}

// 增加恶访次数
CT_BOOL CCUserSeek::AddSpiteTimes(CT_INT32 iTimes)
{
	_AUTOMUTEX(m_pMutex);

	CMD_G_AddSpiteTimes cst = {0};

	if (m_SeekData.iAddSpiteTimes + 1 > SEEK_SPITE_ADDTIMES_MAX)
	{
		G_SendErrorInfo(m_pUser, ERE_SEEK, ESE_OVER_USERLIMIT);
		return CT_FALSE;
	}

	m_SeekData.iAddSpiteTimes += iTimes;

	cst.iAddTimes = m_SeekData.iAddSpiteTimes;
	m_pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_SPITETIMES_ADD, (CT_CHAR*)&cst, sizeof(cst));

	// 更新到数据库
	MSC_DB_SeekData date = {0};
	date.dwUserID = m_pUser->GetUserID();
	date.seekData = m_SeekData;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_DATA_UPDATE, (CT_CHAR*) &date, sizeof(date));

	return CT_TRUE;
}

//发送黑店
CT_VOID CCUserSeek::SendBlacketMarket()
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 iRand = 0;
	G_ValueRange vr = {0};
	iRand = rand()%100 + 1;
	if (!G_GetValueRange(VALUE_TYPE_SEEK_MAKET_PRESENT, &vr))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeSeek G_GetValueRange error.value:%d", VALUE_TYPE_SEEK_MAKET_PRESENT);
		return;
	}
	if (iRand <= vr.fMaxVal)
	{
		m_pUser->m_userBlackMarket.SendSeekBlackMarketInfo();
	}
}

//发送今日统计
CT_BOOL CCUserSeek::GetTotalInfo(TotalInfo* pInfo, CT_INT32 iSize,CT_INT32& iNum, CT_BOOL IsToday/* = CT_FALSE*/)
{
	_AUTOMUTEX(m_pMutex);

	if (iSize < MESSAGE_COUNT * VISTOR_INFO_LEN)
	{
		return CT_FALSE;
	}

	CT_CHAR cBroadcastMes[CHAT_LEN*2] = {0};
	CT_CHAR cTmp[CHAT_LEN*2] = {0};
	CT_CHAR cInfoBegin[CHAT_LEN*2] = {0};
	CT_BOOL bFalg = CT_TRUE;
	CT_BOOL bFirst = CT_FALSE;
	CT_BOOL bPlunder = CT_FALSE;
	CT_INT32 iIndex = 0;
	CT_INT32 iBroadcastLen=CHAT_LEN*2-1;
	CT_INT32 iTmpLen = CHAT_LEN*2 - 1;
	CT_INT32 iInfoBegin = CHAT_LEN*2 - 1;
	CT_INT32 iPresentResID[SEEK_PRESENT_ITEM_NUM] = {0};
	CT_INT32 iPresentResNum[SEEK_PRESENT_ITEM_NUM] = {0};
	CT_INT32 iPresentItemID[SEEK_PRESENT_ITEM_NUM] = {0};
	CT_INT32 iPresentItemNum[SEEK_PRESENT_ITEM_NUM] = {0};
	G_CostRewardGroup crg = {0};
	StatisticsInfo statisticsSelf = {0};
	StatisticsInfo statisticsSeek = {0};
	RollBroadcast rb[8] = {0};
	
	VistorInfoMap::iterator itSelf = m_SeekInfoMap.begin();
	VistorInfoMap::iterator itOthers = m_VistorInfoMap.begin();

	SingleStatistics(m_SeekInfoMap, statisticsSelf, IsToday);

	if (IsToday)
	{
		G_GetRollBroadecastData(158,cInfoBegin,iInfoBegin,(CT_CHAR*)rb,sizeof(rb));
	}
	else
	{
		G_GetRollBroadecastData(160,cInfoBegin,iInfoBegin,(CT_CHAR*)rb,sizeof(rb));
	}

	// 抢别人的
	if (0 != statisticsSelf.iPlayerNum[SEEK_TYPE_PLUNDER]
		&& (statisticsSelf.iGold != 0 || statisticsSelf.iFood != 0) )
	{
		bPlunder = CT_TRUE;
		memset(rb, 0, sizeof(rb));
		rb[0].SetValue(statisticsSelf.iPlayerNum[SEEK_TYPE_PLUNDER],"");
		if (0 != statisticsSelf.iGold)
		{
			rb[1].SetValue(statisticsSelf.iGold,"");
			rb[2].SetValue(1,"");
			rb[3].SetValue(1,"");

			if (0 != statisticsSelf.iFood)
			{
				rb[4].SetValue(statisticsSelf.iFood, "");
				rb[5].SetValue(1,"");
				rb[6].SetValue(4,"");
			}
		}
		else if (0 != statisticsSelf.iFood)
		{
			rb[1].SetValue(statisticsSelf.iFood,"");
			rb[2].SetValue(1,"");
			rb[3].SetValue(4,"");
		}
		G_GetRollBroadecastData(149, cBroadcastMes, iBroadcastLen, (CT_CHAR*)rb, sizeof(rb));

		memset(rb, 0, sizeof(rb));
		memset(cTmp, 0, sizeof(cTmp));
		iTmpLen = 0;
	}

	if (0 != statisticsSelf.iSaintess && 0 != statisticsSelf.iFarmer)
	{
		rb[0].SetValue(statisticsSelf.iSaintess, "");
		rb[1].SetValue(statisticsSelf.iFarmer, "");
		G_GetRollBroadecastData(152, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
	}
	else if (0 != statisticsSelf.iSaintess)
	{
		rb[0].SetValue(statisticsSelf.iSaintess, "");
		G_GetRollBroadecastData(150, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
	}
	else if (0 != statisticsSelf.iFarmer)
	{
		rb[0].SetValue(statisticsSelf.iFarmer, "");
		G_GetRollBroadecastData(151, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
	}
	else
	{
		bFirst = CT_TRUE;
	}

	if (0 != strlen(cTmp))
	{
		sprintf(cBroadcastMes, "%s%s", cBroadcastMes, cTmp);
		iBroadcastLen += iTmpLen;
	}

	memset(rb, 0, sizeof(rb));
	memset(cTmp, 0, sizeof(cTmp));
	iTmpLen = 0;
	G_PetAddition gPet = {0};
	for (CT_INT32 iPet = 0;iPet < SEEK_KIDNAP_PET;++iPet)
	{
		if (statisticsSelf.iPetID[iPet] == 0)
		{
			break;
		}
		if (statisticsSelf.iPetNum[iPet] == 0)
		{
			continue;
		}

		rb[0].SetValue(statisticsSelf.iPetNum[iPet], "");
		G_GetPetAddition(statisticsSelf.iPetID[iPet], &gPet);
		if (bFirst)
		{
			G_GetRollBroadecastData(167, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
			bFirst = CT_FALSE;
		}
		else
		{
			G_GetRollBroadecastData(168, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
		}
		sprintf(cTmp, "%s%s", cTmp, gPet.cName);
	}

	if (0 != strlen(cTmp))
	{
		sprintf(cBroadcastMes, "%s%s", cBroadcastMes, cTmp);
		iBroadcastLen += iTmpLen;
	}

	// 声望，取消记录
	/*if (bPlunder)
	{
		memset(rb, 0, sizeof(rb));
		memset(cTmp, 0, sizeof(cTmp));
		iTmpLen = 0;
		if (statisticsSelf.iRank != 0)
		{
			rb[0].SetValue(statisticsSelf.iRank, "");
			rb[1].SetValue(1,"");
			rb[2].SetValue(12,"");
			G_GetRollBroadecastData(153, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
			sprintf(cBroadcastMes, "%s%s", cBroadcastMes, cTmp);
			iBroadcastLen += iTmpLen;
		}
	}*/

	if (strlen(cBroadcastMes) != 0)
	{
		if (bPlunder)
		{
			sprintf(pInfo[iIndex++].cInfo, "%s%s", cInfoBegin, cBroadcastMes);
		}
		else
		{
			sprintf(pInfo[iIndex++].cInfo, "%s%s", cInfoBegin, cBroadcastMes + 1);
		}
	}

	memset(rb, 0, sizeof(rb));
	memset(cTmp, 0, sizeof(cTmp));
	iTmpLen = 0;
	memset(cBroadcastMes, 0, sizeof(cBroadcastMes));
	iBroadcastLen = 0;

	// 被别人抢的
	SingleStatistics(m_VistorInfoMap, statisticsSeek, IsToday);

	// 掠夺
	if (0 != statisticsSeek.iPlayerNum[SEEK_TYPE_PLUNDER] 
		&& (statisticsSeek.iGold != 0 || statisticsSeek.iFood != 0) )
	{
		rb[0].SetValue(statisticsSeek.iPlayerNum[SEEK_TYPE_PLUNDER],"");
		if (0 != statisticsSeek.iGold)
		{
			rb[1].SetValue(statisticsSeek.iGold,"");
			rb[2].SetValue(1,"");
			rb[3].SetValue(1,"");

			if (0 != statisticsSeek.iFood)
			{
				rb[4].SetValue(statisticsSeek.iFood, "");
				rb[5].SetValue(1,"");
				rb[6].SetValue(4,"");
			}
		}
		else if (0 != statisticsSeek.iFood)
		{
			rb[1].SetValue(statisticsSeek.iFood,"");
			rb[2].SetValue(1,"");
			rb[3].SetValue(4,"");
		}
		G_GetRollBroadecastData(154, cBroadcastMes, iBroadcastLen, (CT_CHAR*)rb, sizeof(rb));
		sprintf(pInfo[iIndex++].cInfo, "%s%s", cInfoBegin, cBroadcastMes);
	}

	memset(rb, 0, sizeof(rb));
	memset(cTmp, 0, sizeof(cTmp));
	iTmpLen = 0;
	memset(cBroadcastMes, 0, sizeof(cBroadcastMes));
	iBroadcastLen = 0;
	memset(cInfoBegin, 0, sizeof(cInfoBegin));
	iInfoBegin = 0;

	if (IsToday)
	{
		G_GetRollBroadecastData(159, cBroadcastMes, sizeof(cBroadcastMes), (CT_CHAR*)rb, sizeof(rb));
	}
	else
	{
		G_GetRollBroadecastData(161, cBroadcastMes, sizeof(cBroadcastMes), (CT_CHAR*)rb, sizeof(rb));
	}

	// 今日收到的礼物
	if (0 != statisticsSeek.iPresentNum[SEEK_TYPE_PRESENT])
	{
		rb[0].SetValue(statisticsSeek.iPresentNum[SEEK_TYPE_PRESENT], "");
		G_GetRollBroadecastData(155, cBroadcastMes, iBroadcastLen, (CT_CHAR*)rb, sizeof(rb));

		for (CT_INT32 i = 0;i < SEEK_PRESENT_ITEM_NUM;++i)
		{
			G_GetCostRewardGroup(statisticsSeek.iPresentID[i], &crg);
			if (0 != crg.res[0].iID)
			{
				for (CT_INT32 j = 0;j < SEEK_PRESENT_ITEM_NUM;++j)
				{
					if (iPresentResID[j] == 0)
					{
						iPresentResID[j] = crg.res[0].iID;
						iPresentResNum[j] = crg.res[0].iNum;
						break;
					}
					if (iPresentResID[j] == crg.res[0].iID)
					{
						iPresentResNum[j] += crg.res[0].iNum;
						break;
					}
				}
			}
			else
			{
				for (CT_INT32 k = 0;k < SEEK_PRESENT_ITEM_NUM;++k)
				{
					if (iPresentItemID[k] == 0)
					{
						iPresentItemID[k] = crg.item[0].iID;
						iPresentItemNum[k] += crg.item[0].iNum;
						break;
					}
					if (iPresentItemID[k] == crg.item[0].iID)
					{
						iPresentItemNum[k] += crg.item[0].iNum;
						break;
					}
				}
			}
		}

		for (CT_INT32 m = 0;m < SEEK_PRESENT_ITEM_NUM;++m)
		{
			if (0 != iPresentResID[m])
			{
				memset(rb, 0, sizeof(rb));
				memset(cTmp, 0, sizeof(cTmp));
				iTmpLen = 0;
				rb[0].SetValue(iPresentResNum[m],"");
				rb[1].SetValue(1,"");
				rb[2].SetValue(iPresentResID[m], "");

				G_GetRollBroadecastData(156, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
				if (bFalg)
				{
					sprintf(cBroadcastMes, "%s%s", cBroadcastMes, cTmp);
					bFalg = CT_FALSE;
				}
				else
				{
					sprintf(cBroadcastMes, "%s,%s", cBroadcastMes, cTmp);
				}
			}
			if (0 != iPresentItemID[m])
			{
				memset(rb, 0, sizeof(rb));
				memset(cTmp, 0, sizeof(cTmp));
				iTmpLen = 0;
				rb[0].SetValue(iPresentItemNum[m],"");
				rb[1].SetValue(2,"");
				rb[2].SetValue(iPresentItemID[m], "");

				G_GetRollBroadecastData(156, cTmp, iTmpLen, (CT_CHAR*)rb, sizeof(rb));
				if (bFalg)
				{
					sprintf(cBroadcastMes, "%s%s", cBroadcastMes, cTmp);
					bFalg = CT_FALSE;
				}
				else
				{
					sprintf(cBroadcastMes, "%s,%s", cBroadcastMes, cTmp);
				}
			}
		}

		sprintf(pInfo[iIndex++].cInfo, "%s%s", cInfoBegin, cBroadcastMes);
	}

	memset(rb, 0, sizeof(rb));
	memset(cTmp, 0, sizeof(cTmp));
	iTmpLen = 0;
	memset(cBroadcastMes, 0, sizeof(cBroadcastMes));
	iBroadcastLen = 0;

	// 加速增产的建筑
	if (0 != statisticsSeek.iPlayerNum[SEEK_TYPE_UPSPEED] && 0 != statisticsSeek.iPlayerNum[SEEK_TYPE_ADDPRODUCT])
	{
		rb[0].SetValue(statisticsSeek.iPlayerNum[SEEK_TYPE_UPSPEED] + statisticsSeek.iPlayerNum[SEEK_TYPE_ADDPRODUCT], "");
		rb[1].SetValue(statisticsSeek.iBuildingNum, "");
		G_GetRollBroadecastData(157, cBroadcastMes, iBroadcastLen, (CT_CHAR*)rb, sizeof(rb));
		sprintf(pInfo[iIndex++].cInfo, "%s%s", cInfoBegin, cBroadcastMes);
	}

	iNum = iIndex;
	return CT_TRUE;
}

//单条统计记录
CT_VOID CCUserSeek::SingleStatistics(VistorInfoMap& vistorInfoMap, StatisticsInfo& statistics, CT_BOOL isToday/* = CT_FALSE*/)
{
	_AUTOMUTEX(m_pMutex);

	time_t t = time(0);
	tm* pTm = localtime(&t);
	CT_INT32 iType = 0;
	VistorInfoMap::iterator itSelf = vistorInfoMap.begin();

	for (;itSelf != vistorInfoMap.end();++itSelf)
	{
		if (isToday && itSelf->second.seekInfo.iType / 100 != pTm->tm_wday)
		{
			continue;
		}

		iType = itSelf->second.seekInfo.iType % 100;
		++statistics.iPlayerNum[iType];
		switch (iType)
		{
		case SEEK_TYPE_PLUNDER:
			{
				for (CT_INT32 i = 0;i < TEAM_RES_CNT;++i)
				{
					switch (itSelf->second.seekInfo.iID[i])
					{
					case VALUE_TYPE_GOLD:
						statistics.iGold += itSelf->second.seekInfo.iNum[i];
						break;
					case VALUE_TYPE_RANKXP+1:
						statistics.iRank += itSelf->second.seekInfo.iNum[i];
						break;
					case VALUE_TYPE_FOOD:
						statistics.iFood += itSelf->second.seekInfo.iNum[i];
						break;
					default:
						break;
					}
				}
				break;
			}
		case SEEK_TYPE_KIDNAP:
			{
				for (CT_INT32 j = 0;j < TEAM_RES_CNT;++j)
				{
					if (SEEK_KIDNAPTYPE_SAINTESS == itSelf->second.seekInfo.iID[j])
					{
						statistics.iSaintess += itSelf->second.seekInfo.iNum[j];
					}
					else if (SEEK_KIDNAPTYPE_WORKER == itSelf->second.seekInfo.iID[j])
					{
						statistics.iFarmer += itSelf->second.seekInfo.iNum[j];
					}
					else if (SEEK_KIDNAPTYPE_PET == itSelf->second.seekInfo.iID[j])
					{
						for (CT_INT32 iPet = 0;iPet < SEEK_KIDNAP_PET;++iPet)
						{
							if (statistics.iPetID[iPet] == 0)
							{
								statistics.iPetID[iPet] = itSelf->second.seekInfo.iNum[j];
								statistics.iPetNum[iPet] = 1;
								break;
							}
							if (statistics.iPetID[iPet] == itSelf->second.seekInfo.iNum[j])
							{
								statistics.iPetNum[iPet] += 1;
								break;
							}
						}
					}
				}
				break;
			}
		case SEEK_TYPE_UPSPEED:
		case SEEK_TYPE_ADDPRODUCT:
			{
				for (CT_INT32 k = 0;k < TEAM_RES_CNT;++k)
				{
					statistics.iBuildingNum += itSelf->second.seekInfo.iNum[k];
				}
				break;
			}
		case SEEK_TYPE_PRESENT:
			{
				for (CT_INT32 m = 0;m < SEEK_PRESENT_ITEM_NUM;++m)
				{
					if (0 == itSelf->second.present[m])
					{
						break;
					}
					for (CT_INT32 n = 0;n < SEEK_PRESENT_ITEM_NUM;++n)
					{
						if (0 == statistics.iPresentID[n])
						{
							statistics.iPresentID[n] = itSelf->second.present[m];
							statistics.iPresentNum[n] = 1;
							break;
						}
						if (statistics.iPresentID[n] == itSelf->second.present[m])
						{
							statistics.iPresentNum[m]++;
							break;
						}
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	return;
}

// 检测宠物是否被绑架
CT_BOOL CCUserSeek::IsPetKidnap(CT_INT32 iPetChangeID)
{
	_AUTOMUTEX(m_pMutex);

	for (CT_INT32 i = 0;i < SEEK_KIDNAP_PET;++i)
	{
		if (m_SeekData.iPet[i] == iPetChangeID)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}