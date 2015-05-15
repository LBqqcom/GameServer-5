#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CFightTeamManager.h"
#include "CDatabaseMes.h"
//#include "CMasterScriptSetting.h"
//#include "CBuildingScriptSetting.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CUserManager.h"
#include "CNetInterface.h"
#include "CResourceGrabMgr.h"
#include "CFightTeam.h"
#include "GlobeScriptInterface.h"
#include "CFightingData.h"
#include "CBroadcastScriptSetting.h"
#include "CFightingScriptSetting.h"


CCFightTeam::CCFightTeam() : m_mutex()
{
	m_iLastUpdTime = time(0);
	m_TBMemberMap.clear();
	memset(&m_stBase, 0, sizeof(FightTeamBase));
	memset(&m_FTSummonInfo,0,sizeof(m_FTSummonInfo));
	memset(&m_FTTimeBossInfo,0,sizeof(m_FTTimeBossInfo));
	Init();
}
CCFightTeam::~CCFightTeam(void)
{
}
CT_VOID CCFightTeam::Init()
{
	Clear();
}

//清理用户数据
CT_VOID CCFightTeam::Clear()
{
	_AUTOMUTEX(&m_mutex);
	m_FTMemberMap.clear();
	m_FTMemberReqMap.clear();
	m_FTMemberRspMap.clear();
	m_FTSummonRewardMap.clear();
	m_FTCSInspore.clear();
	m_TBMemberMap.clear();
	m_TBRewardMap.clear();
	memset(&m_FTSummonInfo,0,sizeof(m_FTSummonInfo));
	memset(&m_FTTimeBossInfo,0,sizeof(m_FTTimeBossInfo));
}

CT_VOID CCFightTeam::InitInsertMember(CT_INT32 iType, const CMD_G_FightTeamMember &data)
{
	_AUTOMUTEX(&m_mutex);
	FightTeamMember temp = {0};
	memcpy(&temp, &data, sizeof(CMD_G_FightTeamMember));
	if (temp.dwUserID < 1 || temp.dwFTID < 1)
	{
		return;
	}
	temp.iStatus = 0; //temp.iStatus & 1;	// 将末2位设0, 标识不在线 //驻守状态也
	CT_DWORD dbThreadID=GAME_THREAD_DATABASE;
	switch(iType)
	{
	case 1:
		m_FTMemberMap.insert(FTMember::value_type(temp.dwUserID, temp));
		CCFightTeamManager::getSingleton().InsertUserMap(temp.dwUserID, m_stBase.dwFTID);
		dbThreadID = GAME_THREAD_DATABASE + temp.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_FTFightingData, (CT_CHAR*)&(temp.dwUserID), sizeof(CT_DWORD));
		break;
	case 2:
		m_FTMemberReqMap.insert(FTMember::value_type(temp.dwUserID, temp));
		if (m_FTMemberReqMap.size() > 50)		//清掉最早的未处理请求
		{
			CT_INT64 iTime = 0;
			CT_DWORD dwUserID = 0;
			FTMember::iterator it = m_FTMemberReqMap.begin();
			iTime = it->second.iTime;
			dwUserID = it->first;
			for (; it!=m_FTMemberReqMap.end(); ++it)
			{
				if (it->second.iTime < iTime)
				{
					iTime = it->second.iTime;
					dwUserID = it->first;
				}
			}
			m_FTMemberReqMap.erase(dwUserID);
		}
		break;
	case 3:
		m_FTMemberRspMap.insert(FTMember::value_type(temp.dwUserID, temp));
		break;
	default:
		return;
	}
}
// 插入战队队员信息
CT_INT32 CCFightTeam::InsertMember(CT_INT32 iType, const CMD_G_FightTeamMember &data)
{
	_AUTOMUTEX(&m_mutex);
	FightTeamMember temp = {0};
	memcpy(&temp, &data, sizeof(CMD_G_FightTeamMember));
	if (temp.dwUserID < 1 || temp.dwFTID < 1)
	{
		return 0;
	}
	CCUser *pUser = 0;
	switch(iType)
	{
	case 1:
		{
			m_FTMemberMap.insert(FTMember::value_type(temp.dwUserID, temp));
			//CCFightTeamManager::getSingleton().InsertUserMap(temp.dwUserID, m_stBase.dwFTID);
			//if (CCUserManager::getSingleton().Find(temp.dwUserID, &pUser))
			//{
			//	UserFightingStatus userFightingData = {0};
			//	G_GetFightingData(pUser, userFightingData);
			//	//m_FTMemberFightingData.insert(FTMemberFightingData::value_type(temp.dwUserID, userFightingData));
			//}
			//else
			//{
			//	CT_DWORD dbThreadID = GAME_THREAD_DATABASE + temp.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			//	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_FTFightingData, (CT_CHAR*)&(temp.dwUserID), sizeof(CT_DWORD));
			//}

			CT_DWORD dbThreadID = GAME_THREAD_DATABASE + temp.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_FTFightingData, (CT_CHAR*)&(temp.dwUserID), sizeof(CT_DWORD));
			//读取个人威能
			G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_FIGHTTEAM_DATA, MSC_DB_CS_READ_INSPORE, (CT_CHAR*)&(temp.dwUserID), sizeof(CT_DWORD));
			break;
		}
	case 2:
		m_FTMemberReqMap.insert(FTMember::value_type(temp.dwUserID, temp));
		if (m_FTMemberReqMap.size() > 50)		//清掉最早的未处理请求
		{
			CT_INT64 iTime = 0;
			CT_DWORD dwUserID = 0;
			FTMember::iterator it = m_FTMemberReqMap.begin();
			iTime = it->second.iTime;
			dwUserID = it->first;
			for (; it!=m_FTMemberReqMap.end(); ++it)
			{
				if (it->second.iTime < iTime)
				{
					iTime = it->second.iTime;
					dwUserID = it->first;
				}
			}
			m_FTMemberReqMap.erase(dwUserID);
			MSC_DB_FightTeamMemberDelete dbFightTeamMemberDelete = {0};
			dbFightTeamMemberDelete.dwFTID = temp.dwFTID;
			dbFightTeamMemberDelete.dwUserID = dwUserID;
			dbFightTeamMemberDelete.iType = 2;
			CT_DWORD threadID = GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_DELETE,(CT_CHAR*)&dbFightTeamMemberDelete,sizeof(MSC_DB_FightTeamMemberDelete));
		}
		break;
	case 3:
		m_FTMemberRspMap.insert(FTMember::value_type(temp.dwUserID, temp));
		break;
	default:
		return 0;
	}

	MSC_DB_FightTeamMember dbFightTeamMember = {0};
	memcpy(&dbFightTeamMember, &temp, sizeof(FightTeamMember));
	dbFightTeamMember.iType = iType;
	CT_DWORD threadID = GAME_THREAD_DATABASE + temp.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));
	return iType;
}

CT_VOID CCFightTeam::InsertMemberFightingData(MSC_DB_FTMemberFightingData &dbFTMemberFightingData)
{
	//m_FTMemberFightingData.insert(FTMemberFightingData::value_type(dbFTMemberFightingData.dwUserID, dbFTMemberFightingData.userFightingData));
}

CT_VOID	CCFightTeam::InsertSummonInfo(FightTeamSummonBoss &FTSB)
{
	_AUTOMUTEX(&m_mutex);
	m_FTSummonInfo.dwFTID=FTSB.dwFTID;
	m_FTSummonInfo.iBossIndex=FTSB.iBossIndex;
	m_FTSummonInfo.iWiner=FTSB.iWiner;

	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(m_FTSummonInfo.dwUserID[i] == FTSB.dwUserID[0])
		{
			m_FTSummonInfo.Res[i].dwUserID=FTSB.dwUserID[0];
			m_FTSummonInfo.Res[i].iResNum0=FTSB.Res[0].iResNum0;
			m_FTSummonInfo.Res[i].iResNum1=FTSB.Res[0].iResNum1;
			m_FTSummonInfo.Res[i].iResNum2=FTSB.Res[0].iResNum2;
			m_FTSummonInfo.Res[i].iResNum3=FTSB.Res[0].iResNum3;
			m_FTSummonInfo.Res[i].iResType0=FTSB.Res[0].iResType0;
			m_FTSummonInfo.Res[i].iResType1=FTSB.Res[0].iResType1;
			m_FTSummonInfo.Res[i].iResType2=FTSB.Res[0].iResType2;
			m_FTSummonInfo.Res[i].iResType3=FTSB.Res[0].iResType3;
			return;
		}
	}

	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(m_FTSummonInfo.dwUserID[i] == 0)
		{
			m_FTSummonInfo.dwUserID[i]=FTSB.dwUserID[0];
			memcpy(&m_FTSummonInfo.Res[i],&FTSB.Res[0],sizeof(FTSB.Res[0]));
			return;
		}
	}
}

CT_VOID	CCFightTeam::InsertSummonReward(FightTeamSummonItem &FTSM)
{
	_AUTOMUTEX(&m_mutex);

	m_FTSummonInfo.dwChestIndex[FTSM.iChestIndex%(SUMMON_MAX*2)]=FTSM.dwUserID;
	m_FTSummonRewardMap.insert(FTSummonRewardMap::value_type(FTSM.dwUserID,FTSM));
	return;
}

//插入威能
CT_VOID	CCFightTeam::InsertInspore(CSInspore &csi)
{
	_AUTOMUTEX(&m_mutex);
	CSUserInsporeMap::iterator csuiit=m_FTCSInspore.find(csi.dwUserID);
	if(m_FTCSInspore.end() != csuiit)
	{
		(*csuiit).second.insert(CSInsporeMap::value_type(csi.iPowerID,csi));
	}
	else
	{
		CSInsporeMap csimap;
		csimap.insert(CSInsporeMap::value_type(csi.iPowerID,csi));
		m_FTCSInspore.insert(CSUserInsporeMap::value_type(csi.dwUserID,csimap));
	}
	return;
}

//插入时限boss
CT_VOID	CCFightTeam::InsertTimeBoss(FTTimeBossInfo &tbi)
{
	_AUTOMUTEX(&m_mutex);
	memcpy(&m_FTTimeBossInfo,&tbi,sizeof(m_FTTimeBossInfo));
	return;
}

//插入时限boss成员信息
CT_VOID	CCFightTeam::InsertTimeBossMember(FTTimeBossMember &bm)
{
	_AUTOMUTEX(&m_mutex);
	m_TBMemberMap.insert(TBMemberMap::value_type(bm.dwUserID,bm));
	return;
}

//插入时限boss奖励
CT_VOID	CCFightTeam::InsertTimeBossRewardr(FightTeamSummonItem &si,CT_BOOL bSave/*=CT_TRUE*/)
{
	_AUTOMUTEX(&m_mutex);
	m_TBRewardMap.insert(FTSummonRewardMap::value_type(si.iChestIndex,si));

	//写入数据库
	if(bSave)
	{
		MSC_DB_FightTeamSummInfoItem FTSI={0};
		FTSI.dwFTID=m_stBase.dwFTID;
		FTSI.dwUserID=si.dwUserID;
		FTSI.iItemID=si.iItemID;
		FTSI.iCount=si.iCount;
		FTSI.iChestIndex=si.iChestIndex;
		FTSI.iType=FIGHTTEAM_BOSS_TIME;
		CT_DWORD threadid = GAME_THREAD_DATABASE + si.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_SUMMON_ITEM,(CT_CHAR*)&FTSI,sizeof(FTSI));
	}
	return;
}

// 删除战队队员信息
CT_INT32 CCFightTeam::DeleteMember(CT_INT32 iType, CT_DWORD dwMUID)
{
	_AUTOMUTEX(&m_mutex);
	switch(iType)
	{
	case 1:
		m_FTMemberMap.erase(dwMUID);
		//m_FTMemberFightingData.erase(dwMUID);
		//CCFightTeamManager::getSingleton().DeleteUserMap(dwMUID);
		//m_FTMemberFightingData.erase(dwMUID);
		break;
	case 2:
		m_FTMemberReqMap.erase(dwMUID);
		break;
	case 3:
		m_FTMemberRspMap.erase(dwMUID);
		break;
	default:
		return 0;
	}
	MSC_DB_FightTeamMemberDelete dbMemberDelete = {0};
	dbMemberDelete.dwFTID = m_stBase.dwFTID;
	dbMemberDelete.dwUserID = dwMUID;
	dbMemberDelete.iType = iType;
	CT_DWORD threadID = GAME_THREAD_DATABASE + dwMUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_DELETE,(CT_CHAR*)&dbMemberDelete,sizeof(MSC_DB_FightTeamMemberDelete));
	return iType;
}


CT_BOOL CCFightTeam::GetFightTeamBase(CMD_G_FightTeamBase *pFightTeamBase)
{
	_AUTOMUTEX(&m_mutex);
	ReCheckMember();
	ComputeFightingPower();

	memcpy(pFightTeamBase, &m_stBase, sizeof(FightTeamBase));
	pFightTeamBase->nMember = m_FTMemberMap.size();

	return CT_TRUE;
}


CT_BOOL CCFightTeam::GetFightTeamInfo(CMD_G_FightTeamInfo *pFightTeamInfo)
{
	_AUTOMUTEX(&m_mutex);
	ReCheckMember();
	ComputeFightingPower();

	pFightTeamInfo->iResCount = m_stBase.iResCount;
	pFightTeamInfo->iCount = m_FTMemberMap.size();
	pFightTeamInfo->dwFTID = m_stBase.dwFTID;
	pFightTeamInfo->iGrade = m_stBase.iGrade;
	memcpy(pFightTeamInfo->cFightTeamName, m_stBase.cFightTeamName, NAME_SIZE);
	pFightTeamInfo->iFightingPower = m_stBase.iFightingPower;
	pFightTeamInfo->iTop = m_stBase.iTop;
	pFightTeamInfo->iIcon = m_stBase.iIcon;
	memset(pFightTeamInfo->cMemberName, 0, sizeof(pFightTeamInfo->cMemberName));
	FTMember::iterator it = m_FTMemberMap.find(m_stBase.dwBossUID);
	if (it != m_FTMemberMap.end())
	{
		pFightTeamInfo->iOccupation = it->second.iOccupation;
		memcpy(pFightTeamInfo->cMemberName[0], it->second.cNickName, NAME_SIZE);
	}

	CT_INT32 i=1;
	for(it=m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		if (it->first != m_stBase.dwBossUID)
		{
			memcpy(pFightTeamInfo->cMemberName[i], it->second.cNickName, NAME_SIZE);
			++i;
		}
		if (i == CLAN_MAX_MEMBER)
		{
			break;
		}
	}

	memcpy(pFightTeamInfo->cDescription, m_stBase.cDescription, DESCRIPTION_SIZE);

	return CT_TRUE;
}

CT_BOOL CCFightTeam::GetFightTeamMember(CT_INT32 iType, CT_DWORD dwUser, CMD_G_FightTeamMember* pFightTeamMember)
{
	_AUTOMUTEX(&m_mutex);

	FTMember::iterator it;
	switch(iType)
	{
	case 1:
		it = m_FTMemberMap.find(dwUser);
		if (it != m_FTMemberMap.end())
		{
			memcpy(pFightTeamMember, &(it->second), sizeof(CMD_G_FightTeamMember));
			return CT_TRUE;
		}
		break;
	case 2:
		it = m_FTMemberReqMap.find(dwUser);
		if (it != m_FTMemberReqMap.end())
		{
			memcpy(pFightTeamMember, &(it->second), sizeof(CMD_G_FightTeamMember));
			return CT_TRUE;
		}
		break;
	case 3:
		it = m_FTMemberRspMap.find(dwUser);
		if (it != m_FTMemberRspMap.end())
		{
			memcpy(pFightTeamMember, &(it->second), sizeof(CMD_G_FightTeamMember));
			return CT_TRUE;
		}
		break;
	default:
		return CT_FALSE;
	}
	return CT_FALSE;

}

//获得威能
CT_BOOL	CCFightTeam::GetInspore(CT_DWORD dwUserID,CSInspore &csi)
{
	_AUTOMUTEX(&m_mutex);
	CSUserInsporeMap::iterator it=m_FTCSInspore.find(dwUserID);
	if(it != m_FTCSInspore.end())
	{
		CSInsporeMap::iterator csiit=(*it).second.begin();
		if(csiit != (*it).second.end())
			memcpy(&csi,&((*csiit).second),sizeof(csi));
		else
			return CT_FALSE;
	}
	else
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}

//威能升级
CT_VOID	CCFightTeam::UpGradeInspore(CT_DWORD dwUserID,CSInspore &csi)
{
	_AUTOMUTEX(&m_mutex);

	G_CSFightInspire csfi={0};
	if(!G_GetCSFightInspire(csi.iPowerID,csi.iGrade+1,&csfi))
		return;

	csi.iExp=10;
	CSUserInsporeMap::iterator uiit=m_FTCSInspore.find(dwUserID);
	if(m_FTCSInspore.end() != uiit)
	{
		CSInsporeMap::iterator csiit=(*uiit).second.find(csi.iPowerID);
		if((*uiit).second.end() != csiit)
		{
			(*csiit).second.iExp+=csi.iExp;

			if((*csiit).second.iExp >= csfi.iExp)
			{
				(*csiit).second.iGrade+=1;
				(*csiit).second.iExp-=csfi.iExp;
			}
		}
		else
		{
			(*uiit).second.insert(CSInsporeMap::value_type(csi.iPowerID,csi));
		}
	}
	else
	{
		CSInsporeMap csimap;
		csimap.insert(CSInsporeMap::value_type(csi.iPowerID,csi));
		m_FTCSInspore.insert(CSUserInsporeMap::value_type(csi.dwUserID,csimap));
	}

	//记录数据库
	uiit=m_FTCSInspore.find(dwUserID);
	CSInsporeMap::iterator csiit=(*uiit).second.find(csi.iPowerID);
	if((*uiit).second.end() != csiit)
	{
		MSC_DB_INSPORE dbi={0};
		dbi.dwUserID=csi.dwUserID;
		dbi.iID=(*csiit).second.iPowerID;
		dbi.iGrade=(*csiit).second.iGrade;
		dbi.iExp=(*csiit).second.iExp;

		CT_DWORD dwThreadID=GAME_THREAD_DATABASE+dbi.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_DB_CS_INSPORE_UP,(CT_CHAR*)&dbi,sizeof(dbi));
	}
	return;
}

CT_BOOL CCFightTeam::GetFightTeamMember(CT_CHAR *pBuf, CT_DWORD &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	ReCheckMember();
	CT_DWORD len = m_FTMemberMap.size() * sizeof(CMD_G_FightTeamMember);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	FTMember::iterator it = m_FTMemberMap.begin();
	for (int i = 0; it != m_FTMemberMap.end(); ++it, ++i)
	{
		//		CMD_G_FightTeamMember fightTeamMember = {0};
		memcpy(pBuf + sizeof(CMD_G_FightTeamMember) * i, &(it->second), sizeof(CMD_G_FightTeamMember));	//因为结构相同, 可以直接拷贝
	}
	bufLen = len;

	return CT_TRUE;
}

CT_BOOL CCFightTeam::GetFightTeamReqMember(CT_CHAR *pBuf, CT_DWORD &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	CT_DWORD len = m_FTMemberReqMap.size() * sizeof(CMD_G_FightTeamMember);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	FTMember::iterator it = m_FTMemberReqMap.begin();
	for (int i = 0; it != m_FTMemberReqMap.end() && i < 10; ++it, ++i)
	{
		//		CMD_G_FightTeamMember fightTeamMember = {0};
		memcpy(pBuf + sizeof(CMD_G_FightTeamMember) * i, &(it->second), sizeof(CMD_G_FightTeamMember));	//因为结构相同, 可以直接拷贝
	}
	bufLen = len;

	return CT_TRUE;
}



CT_BOOL CCFightTeam::CheckFightTeamFull()
{
	_AUTOMUTEX(&m_mutex);
	//if (m_FTMemberMap.size() >= CLAN_MAX_MEMBER)
	G_FightTeamUpgrade FTUG={0};
	if(G_GetFightTeamUpgrade(m_stBase.iGrade,FIGHTTEAM_BOSS,&FTUG))
	{
		if (m_FTMemberMap.size() >= FTUG.iPeopleNum)
		{
			return CT_TRUE;
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::CheckFightTeamFull G_GetFightTeamUpgrade grade:%d",m_stBase.iGrade);
	}
	return CT_FALSE;
}

CT_VOID CCFightTeam::ClearReqRsp(CT_DWORD dwUser)
{
	_AUTOMUTEX(&m_mutex);
	if (m_FTMemberReqMap.find(dwUser) != m_FTMemberReqMap.end())
	{
		m_FTMemberReqMap.erase(dwUser);
		MSC_DB_FightTeamMemberDelete dbMemberDelete = {0};
		dbMemberDelete.dwFTID = m_stBase.dwFTID;
		dbMemberDelete.dwUserID = dwUser;
		dbMemberDelete.iType = 2;
		CT_DWORD threadID = GAME_THREAD_DATABASE + dwUser % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_DELETE,(CT_CHAR*)&dbMemberDelete,sizeof(MSC_DB_FightTeamMemberDelete));

	}

	if (m_FTMemberRspMap.find(dwUser) != m_FTMemberRspMap.end())
	{
		m_FTMemberRspMap.erase(dwUser);
		MSC_DB_FightTeamMemberDelete dbMemberDelete = {0};
		dbMemberDelete.dwFTID = m_stBase.dwFTID;
		dbMemberDelete.dwUserID = dwUser;
		dbMemberDelete.iType = 3;
		CT_DWORD threadID = GAME_THREAD_DATABASE + dwUser % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_DELETE,(CT_CHAR*)&dbMemberDelete,sizeof(MSC_DB_FightTeamMemberDelete));
	}
}

//CT_INT32 CCFightTeam::Contribute(CT_DWORD dwUser, CT_INT32 iFood)
CT_INT32 CCFightTeam::Contribute(const CMD_G_FightTeam_Donate &Donate)
{
	_AUTOMUTEX(&m_mutex);
	//读取捐献脚本
	G_FightTeamDonate ftd={0};
	if(Donate.iFood != 0)
	{
		if(!G_GetFightTeamDonateEx(CLIENT_OBJ_RES,VALUE_TYPE_FOOD,&ftd))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::Contribute G_GetFightTeamDonateEx VALUE_TYPE_FOOD");
			return 0;
		}
	}
	else if(Donate.iDiamod)
	{
		if(!G_GetFightTeamDonateEx(CLIENT_OBJ_RES,VALUE_TYPE_DIAMOND,&ftd))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::Contribute G_GetFightTeamDonateEx VALUE_TYPE_DIAMOND");
			return 0;
		}
	}
	else if(Donate.iItemID != 0 && Donate.iItemNum != 0)
	{
		if(!G_GetFightTeamDonateEx(CLIENT_OBJ_ITEM,Donate.iItemID,&ftd))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::Contribute G_GetFightTeamDonateEx CLIENT_OBJ_ITEM");
			return 0;
		}
	}    

	MasterVariableData mvd = {0};
	mvd.stRes.iFood = Donate.iFood*-1;
	mvd.stRes.iDiamod= Donate.iDiamod*-1;
	CCUser *pUser = 0;
	if (CCUserManager::getSingleton().Find(Donate.dwUserID,&pUser))
	{
		if(Donate.iFood != 0 || Donate.iDiamod != 0)
			pUser->m_userBase.AddMastData(mvd,SAVETAG_FIGHTTEAM_CONTRIBUTE);
		else if(Donate.iItemID != 0 && Donate.iItemNum != 0)
		{
			CT_INT32 iCount=Donate.iItemNum*-1;
			//CT_DWORD threadid = GAME_THREAD_CITY + Donate.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            if(pUser->LootItems(SAVETAG_FIGHTTEAM_CONTRIBUTE, &Donate.iItemID, &iCount) != LOOT_SUC)
			{
				CCLogMeg::getSingleton().WriteError("CCFightTeam::Contribute pUser->LootItems(&Donate.iItemID, &iCount) != LOOT_SUC");
				return 0;
			}
			//pUser->m_userItem.AutoAddItemData(threadid,Donate.iItemID,iCount,ITEM_OP_MODE_EXEC);
		}
	}
	
	//if(Donate.iItemID != 0 && Donate.iItemNum != 0)
	//	m_stBase.iItemCount+=G_GetFightTeamResChange(Donate.iItemNum,ftd.fFightTeamRes);

	CT_INT32 iContribution = 0;
	CT_INT32 iResource = 0;
	CMD_G_FightTeamEventLog fightTeamEventLog={0};
	CT_CHAR cEventDesc[DESCRIPTION_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	CT_CHAR cName[NAME_SIZE] = {0};
	RollBroadcast rb[4]={0};
	GetMemberName(Donate.dwUserID, cName);
	fightTeamEventLog.iReport = 0;
	fightTeamEventLog.lDate = time(0);
	FTMember::iterator it = m_FTMemberMap.find(Donate.dwUserID);
	if (it != m_FTMemberMap.end())
	{
		rb[0].SetValue(0,cName);
		if(Donate.iFood > 0)
		{
			iContribution += G_GetFightTeamContributeChange(Donate.iFood,ftd.fDonateDeg); // / 10; 改为在客户端显示时做比例转换 2013-05-18
			iResource += G_GetFightTeamResChange(Donate.iFood,ftd.fFightTeamRes);
			it->second.iContributeCount[FIGHTTEAM_FOOD]++;

			rb[1].SetValue(Donate.iFood,"");
			rb[2].SetValue(iResource,"");
			rb[3].SetValue(iContribution,"");
			
			//sprintf(cEventDesc, "<%s>{2}%d{3}%d{31}%d", cName, Donate.iFood, iResource, iContribution);
			G_GetRollBroadecastData(99,cEventDesc,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
		}
		else if(Donate.iItemID == ftd.iDonateType && Donate.iItemNum > 0)
		{
			iContribution += G_GetFightTeamContributeChange(Donate.iItemNum,ftd.fDonateDeg);
			//iResource += G_GetFightTeamResChange(Donate.iItemNum,ftd.fFightTeamRes);
			m_stBase.iItemCount += Donate.iItemNum * ftd.fFightTeamRes;
			it->second.iContributeCount[FIGHTTEAM_ITEM]++;

			rb[1].SetValue(Donate.iItemNum,"");
			rb[2].SetValue(Donate.iItemNum,"");
			rb[3].SetValue(iContribution,"");
			
			//sprintf(cEventDesc, "<%s>{2}%d{30}%d{31}%d", cName, Donate.iItemNum, Donate.iItemNum, iContribution);
			G_GetRollBroadecastData(100,cEventDesc,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
		}
		else if(Donate.iDiamod > 0)
		{
			iContribution += G_GetFightTeamContributeChange(Donate.iDiamod,ftd.fDonateDeg);
			iResource += G_GetFightTeamResChange(Donate.iDiamod,ftd.fFightTeamRes);
			it->second.iContributeCount[FIGHTTEAM_DIAMOD]++;

			rb[1].SetValue(Donate.iDiamod,"");
			rb[2].SetValue(iResource,"");
			rb[3].SetValue(iContribution,"");
			//sprintf(cEventDesc, "<%s>{2}%d{29}%d{31}%d", cName, Donate.iDiamod, iResource, iContribution);
			G_GetRollBroadecastData(101,cEventDesc,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
		}
		it->second.iContribution += iContribution;
		it->second.iContributeGross+=iContribution;
		m_stBase.iResource += iResource;

		MSC_DB_FightTeamMember dbFightTeamMember = {0};
		memcpy(&dbFightTeamMember, &(it->second), sizeof(FightTeamMember));
		dbFightTeamMember.iType = 1;
		CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamMember.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));

		MSC_DB_FightTeamBase dbFightTeamBase = {0};
		memcpy(&dbFightTeamBase, &m_stBase, sizeof(FightTeamBase));
		threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));
	}
	memcpy(fightTeamEventLog.cEventDesc, cEventDesc, DESCRIPTION_SIZE);
	AddEventLog(fightTeamEventLog);
	return it->second.iContribution;
}

// 队员捐献
CT_VOID	CCFightTeam::Contribute(const CT_DWORD dwUserID,const CT_INT32 iDonate)
{
	FTMember::iterator it = m_FTMemberMap.find(dwUserID);
	if (it != m_FTMemberMap.end())
	{
		(*it).second.iContribution+=iDonate;
	}
	return;
}

CT_INT32 CCFightTeam::GetEliteIndex(CT_DWORD dwUser)
{
	_AUTOMUTEX(&m_mutex);
	for(CT_INT32 i=0; i < CLAN_CRUSADE_MEMBER; ++i)
	{
		if (m_stBase.dwElite[i] == dwUser)
		{
			return i;
		}
	}
	return -1;
}

CT_VOID CCFightTeam::GetMemberName(CT_DWORD dwMemberID, CT_CHAR* pName)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it = m_FTMemberMap.find(dwMemberID);
	if (it != m_FTMemberMap.end())
	{
		memcpy(pName, it->second.cNickName, NAME_SIZE);
	}
}

//初始化插入战队动态
CT_VOID	CCFightTeam::InitEventLog(const CMD_G_FightTeamEventLog &fightTeamEventLog)
{
	_AUTOMUTEX(&m_mutex);
	FTEventData::iterator it;
	while (m_FTEventMap.size() > 49)
	{
		it = m_FTEventMap.begin();
		m_FTEventMap.erase(it);
	}

	m_FTEventMap.insert(FTEventData::value_type(fightTeamEventLog.lDate, fightTeamEventLog));
}

//增加战队动态
CT_VOID	CCFightTeam::AddEventLog(const CMD_G_FightTeamEventLog &fightTeamEventLog)
{
	_AUTOMUTEX(&m_mutex);
	FTEventData::iterator it;
	while (m_FTEventMap.size() > 49)
	{
		it = m_FTEventMap.begin();
		m_FTEventMap.erase(it);
	}

	m_FTEventMap.insert(FTEventData::value_type(fightTeamEventLog.lDate, fightTeamEventLog));

	// 推送各成员
	for (FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pUser))
		{
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_LOOKEVENT,(CT_CHAR*)&fightTeamEventLog,sizeof(CMD_G_FightTeamEventLog));
		}
	}
}

//获取战队动态
CT_VOID CCFightTeam::GetEventLog(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	CMD_G_FightTeamEventLog fightTeamEventLog = {0};
	CT_INT32 n = 0;
	FTEventData::iterator it = m_FTEventMap.begin();
	for (; it!= m_FTEventMap.end() && n < 50; ++n, ++it)
	{
		fightTeamEventLog.iReport = it->second.iReport;
		fightTeamEventLog.lDate = it->second.lDate;
		memcpy(fightTeamEventLog.cEventDesc, it->second.cEventDesc, DESCRIPTION_SIZE);
		memcpy(pBuf + (sizeof(CMD_G_FightTeamEventLog) * n), (CT_CHAR*)&fightTeamEventLog, sizeof(CMD_G_FightTeamEventLog));
	}
	bufLen = sizeof(CMD_G_FightTeamEventLog) * n;
}

//插入战报
CT_INT32 CCFightTeam::ADDFightData(const FightTeamFightData &ftFightData)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 iIndex=1;
	if (m_FTFightMap.size() > 0)
	{
		FTFightData::iterator it = m_FTFightMap.end();
		--it;
		iIndex = it->first + 1;
	}

	m_FTFightMap.insert(FTFightData::value_type(iIndex, ftFightData));
	if (m_FTFightMap.size() > 10)		//清掉最早的
	{
		FTFightData::iterator it = m_FTFightMap.begin();
		m_FTFightMap.erase(it);
	}
	return iIndex;
}

CT_VOID CCFightTeam::Broadcast(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_DWORD dwExcep)
{
	_AUTOMUTEX(&m_mutex);
	for (FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pUser =0;
		if (it->first != dwExcep && CCUserManager::getSingleton().Find(it->first, &pUser))
		{
			pUser->GetNetInterface()->Send(mainID,	subID, pBuf, len);
		}
	}
}

CT_VOID CCFightTeam::FightTeamChat(CMD_G_FightTeamChat *pFightTeamChat, CT_DWORD len)
{
	_AUTOMUTEX(&m_mutex);
	for (FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pUser =0;
		if (pFightTeamChat->dwUserID != it->first)
		{
			if (CCUserManager::getSingleton().Find(it->first, &pUser))
			{
				if (!pUser->m_userFriend.CheckBlackList(pFightTeamChat->dwUserID))
				{
					pUser->GetNetInterface()->Send(MAIN_G_CHAT_CITY,SUB_G_FIGHTTEAM_CHAT,(CT_CHAR*)pFightTeamChat, len);
				}
			}
		}
	}
}

// 获取战队成员UID
CT_VOID CCFightTeam::GetFightTeamMemberUID(FightTeamMemberUID *ftMemberUID)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 n = 0;
	ftMemberUID->dwCount = m_FTMemberMap.size();
	for (FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end() && n<CLAN_MAX_MEMBER; ++it)
	{
		ftMemberUID->dwMemberUID[n] = it->first;		
		++n;
	}
}

CT_VOID CCFightTeam::ComputeFightingPower()
{
	CT_INT32 iFightingPower = 0;
	for (FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pUser;
		MasterData masterData = {0};
		if (CCUserManager::getSingleton().Find(it->first,&pUser))
		{
			pUser->m_userBase.GetMasterData(masterData);
			it->second.iFightingPower = masterData.mvd.stRes.iFightingPower;
		}
		
		iFightingPower += it->second.iFightingPower;
	}
	m_stBase.iFightingPower = iFightingPower;
}

CT_VOID CCFightTeam::ReCheckMember()
{
	for (FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); )
	{
		if (it->second.dwFTID < 1 || it->second.dwUserID < 1)
		{
			m_FTMemberMap.erase(it);
			it = m_FTMemberMap.begin();
		}
		else
			++it;
	}
}


//定时保存数据
CT_VOID CCFightTeam::UpdateData(CT_INT64 lTime)
{
	_AUTOMUTEX(&m_mutex);
	if (time(0) - m_iLastUpdTime < g_iDBUpdataValue)
		return;

	ReCheckMember();
	ComputeFightingPower();

	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &m_stBase, sizeof(FightTeamBase));
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	MSC_DB_FightTeamMember dbFightTeamMember = {0};
	FTMember::iterator it;
	dbFightTeamMember.iType = 1;
	for (it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		memcpy(&dbFightTeamMember, &(it->second), sizeof(FightTeamMember));
		threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));
	}

	dbFightTeamMember.iType = 2;
	for (it = m_FTMemberReqMap.begin(); it != m_FTMemberReqMap.end(); ++it)
	{
		memcpy(&dbFightTeamMember, &(it->second), sizeof(FightTeamMember));
		threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));
	}

	dbFightTeamMember.iType = 3;
	for (it = m_FTMemberRspMap.begin(); it != m_FTMemberRspMap.end(); ++it)
	{
		memcpy(&dbFightTeamMember, &(it->second), sizeof(FightTeamMember));
		threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));
	}

/*	MSC_DB_FightTeamEventLog dbFightTeamEvent = {0};
	for (FTEventData::iterator it_event = m_FTEventMap.begin(); it_event != m_FTEventMap.end(); ++it_event)
	{
		if (it_event->first < m_iLastUpdTime)
		{
			continue;
		}
		dbFightTeamEvent.dwFTID = m_stBase.dwFTID;
		dbFightTeamEvent.lDate = it_event->first;
		dbFightTeamEvent.iReport = it_event->second.iReport;
		memcpy(dbFightTeamEvent.cEventDesc, it_event->second.cEventDesc, DESCRIPTION_SIZE);
		threadID = GAME_THREAD_DATABASE + m_stBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_EVENT_SAVE, (CT_CHAR*)&dbFightTeamEvent, sizeof(MSC_DB_FightTeamEventLog));
	}
*/
	m_iLastUpdTime = time(0);	
}

CT_BOOL CCFightTeam::GetUserFightData(UserFightingStatus *userFightingData, CT_INT32 iCount,CT_BOOL bFight/*=CT_FALSE*/)	// 获取精英战斗数据
{
	_AUTOMUTEX(&m_mutex);
	FTMemberFightingData::iterator it;
	CT_INT32 n=0;
	CCUser *pUser=0;
	for (CT_INT32 i = 0; i < CLAN_CRUSADE_MEMBER; ++i)
	{
		if (n >= iCount)
		{
			return CT_FALSE;
		}
		if (CCUserManager::getSingleton().Find(m_stBase.dwElite[i], &pUser))
		{
			UserFightingStatus userFighting = {0};
			G_GetFightingData(pUser, userFighting);
			//m_FTMemberFightingData.erase(m_stBase.dwElite[i]);
			//m_FTMemberFightingData.insert(FTMemberFightingData::value_type(m_stBase.dwElite[i], userFighting));
			FightingData fd={0};
			if(CCFightingData::getSingleton().GetFightingData(m_stBase.dwElite[i],fd))
			{
				memcpy(&userFightingData[n], &fd.FData, sizeof(UserFightingStatus));
				++n;
			}
			if(bFight)
				pUser->m_userArmy.DeleteBoss();
		}
		else
		{
			//it = m_FTMemberFightingData.find(m_stBase.dwElite[i]);
			//if (it != m_FTMemberFightingData.end())
			//{
				//memcpy(&userFightingData[n], &(it->second), sizeof(UserFightingStatus));
			FightingData fd={0};
			if(CCFightingData::getSingleton().GetFightingData(m_stBase.dwElite[i],fd))
			{
				memcpy(&userFightingData[n], &fd.FData, sizeof(UserFightingStatus));
				++n;
			}
			CCFightingData::getSingleton().DeleteBoss(m_stBase.dwElite[i]);
			//}
		}
	}
	return CT_TRUE;
}

CT_BOOL CCFightTeam::GetUserFightData(CT_DWORD dwUserID, UserFightingStatus &rightFightingData,CT_BOOL bFight/*=CT_FALSE*/)	// 获取指定成员战斗数据
{
	_AUTOMUTEX(&m_mutex);

	CCUser *pUser=0;
	if (CCUserManager::getSingleton().Find(dwUserID, &pUser))
	{
		UserFightingStatus userFightingData = {0};
		G_GetFightingData(pUser, userFightingData);
		if(bFight)
			pUser->m_userArmy.DeleteBoss();
		//m_FTMemberFightingData.erase(dwUserID);
		//m_FTMemberFightingData.insert(FTMemberFightingData::value_type(dwUserID, userFightingData));
		FightingData fd={0};
		if(CCFightingData::getSingleton().GetFightingData(dwUserID,fd))
		{
			memcpy(&rightFightingData, &fd.FData, sizeof(UserFightingStatus));
			return CT_TRUE;
		}
	}
	//FTMemberFightingData::iterator it = m_FTMemberFightingData.find(dwUserID);
	//if (it != m_FTMemberFightingData.end())
	//{
		//memcpy(&rightFightingData, &(it->second), sizeof(UserFightingStatus));

	FightingData fd={0};
	if(CCFightingData::getSingleton().GetFightingData(dwUserID,fd))
	{
		memcpy(&rightFightingData, &fd.FData, sizeof(UserFightingStatus));
		return CT_TRUE;
	}
	//}
	return CT_FALSE;
}

// 用户下线,更新战队成员数据
CT_VOID CCFightTeam::UserExitEvent(CCUser *pUser)
{
	_AUTOMUTEX(&m_mutex);
	CT_DWORD dwUserID = pUser->GetUserID();
	UserFightingStatus userFightingData = {0};
	G_GetFightingData(pUser, userFightingData);
	//m_FTMemberFightingData[dwUserID] = userFightingData;

	FTMember::iterator it = m_FTMemberMap.find(dwUserID);
	if (it != m_FTMemberMap.end())
	{
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		it->second.iGrade = masterData.mvd.stGrade.iLevel;
		it->second.iFightingPower = masterData.mvd.stRes.iFightingPower;
		it->second.iRank = masterData.mvd.stRank.iLevel;
		it->second.iStatus = it->second.iStatus & 1;	// 将末2位设0, 标识不在线
		it->second.iLastExitTime = time(0);
		memcpy(it->second.cNickName,masterData.cNickName,NAME_SIZE);

		CMD_G_FightTeam_Line fightTeamLine = {0};
		fightTeamLine.dwUserID = it->first;
		fightTeamLine.iLine = 0;
		Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_MEMBER_LINE,(CT_CHAR*)&fightTeamLine, sizeof(CMD_G_FightTeam_Line));

		MSC_DB_FightTeamMember dbFightTeamMember = {0};
		memcpy(&dbFightTeamMember, &(it->second), sizeof(FightTeamMember));
		dbFightTeamMember.iType = 1;
		CT_DWORD threadID = GAME_THREAD_DATABASE + it->first % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));
	}
}

CT_VOID CCFightTeam::UserLoginEvent(CT_DWORD dwUser)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it = m_FTMemberMap.find(dwUser);
	if (it != m_FTMemberMap.end())
	{
		it->second.iStatus = it->second.iStatus | 2;	// 将末2位设1, 标识在线
		CMD_G_FightTeam_Line fightTeamLine = {0};
		fightTeamLine.dwUserID = it->first;
		fightTeamLine.iLine = 1;
		Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_MEMBER_LINE,(CT_CHAR*)&fightTeamLine, sizeof(CMD_G_FightTeam_Line));
	}
}

// 分资源奖励
CT_VOID CCFightTeam::ResReward(CT_INT32 iResource)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 iAverageRes = iResource / m_FTMemberMap.size();
	for(FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pUser=0;
		if (CCUserManager::getSingleton().Find(it->first, &pUser))
		{
			MasterVariableData mvd = {0};
			mvd.stRes.iGold = iAverageRes;
			pUser->m_userBase.AddMastData(mvd,SAVETAG_FIGHTTEAM_RESREWARD);
			memset(&mvd,0,sizeof(MasterVariableData));
			pUser->m_userBase.GetMasterData(mvd);
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RESREWARD,(CT_CHAR*)&(mvd.stRes.iGold), sizeof(CT_INT32));			
		}
		else
		{
			MSC_DB_MasterSaveData msd = {0};
			msd.dwUserID = it->first;
			msd.mvd.stRes.iGold = iAverageRes;
			msd.iSaveTag = SAVETAG_FIGHTTEAM_RESREWARD;
			G_SaveMastvariableData(msd);
		}
		MSC_LOGDB_FightTeam logdbFightTeam = {0};
		logdbFightTeam.dwUserID = it->first;
		logdbFightTeam.iGold = iAverageRes;
		logdbFightTeam.iGrade = it->second.iGrade;
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + logdbFightTeam.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM,(CT_CHAR*)&logdbFightTeam,sizeof(MSC_LOGDB_FightTeam));
	}
}

// 获取队员职位 1.队长 2.精英 3.普通成员 0.未知道(未找到)
CCFightTeam::FIGHT_MEMBER_TYPE CCFightTeam::GetMemberPosition(CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	if (m_stBase.dwBossUID == dwUserID)
	{
		return FIGHTTEAM_BOSS;
	}

	for (CT_INT32 n=0; n < CLAN_CRUSADE_MEMBER; ++n)
	{
		if (m_stBase.dwElite[n] == dwUserID)
		{
			return FIGHTTEAM_ELITE;
		}
	}

	if (m_FTMemberMap.find(dwUserID) != m_FTMemberMap.end())
	{
		return FIGHTTEAM_MEMBER;
	}
	return FIGHTTEAM_UNKNOW;
}

CT_BOOL CCFightTeam::CheckResCountGrade(CT_INT32 iResCount)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it = m_FTMemberMap.find(m_stBase.dwBossUID);
	if (it != m_FTMemberMap.end())
	{
		//if (it->second.iGrade >= G_GetFightTeamResCountGrade(iResCount))
		//{
		//	return CT_TRUE;
		//}
		G_FightTeamUpgrade FTUG={0};
		if(G_GetFightTeamUpgrade(m_stBase.iGrade,FIGHTTEAM_BOSS,&FTUG))
		{
			if(iResCount <= FTUG.iResoureSpot)
				return CT_TRUE;
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeam::CheckResCountGrade G_GetFightTeamUpgrade Grade:%d",m_stBase.iGrade);
			return CT_FALSE;
		}
	}
	return CT_FALSE;
}

// 驻守人员变更
CT_VOID CCFightTeam::SetUsersGuard(const GuardTag &gdPrimary, const GuardTag &gd)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it;
	for (CT_INT32 i=0; i < SPOT_GUARD_MEMBER; ++i)
	{
		it = m_FTMemberMap.find(gdPrimary.dwUserID[i]);
		if (it != m_FTMemberMap.end())
		{
			it->second.iStatus = it->second.iStatus & 2;	//将末位设0,标识空闲
		}
	}

	for (CT_INT32 i=0; i < SPOT_GUARD_MEMBER; ++i)
	{
		it = m_FTMemberMap.find(gd.dwUserID[i]);
		if (it != m_FTMemberMap.end())
		{
			it->second.iStatus = it->second.iStatus | 1;	//将末位设1,标识忙
		}
	}

	CT_DWORD dwGuardList[CLAN_MAX_MEMBER] = {0};
	CT_INT32 n = 0;
	for (it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		if (1 == (it->second.iStatus & 1))	//末位为1表示忙
		{
			dwGuardList[n] = it->first;
			if(n+1 < CLAN_MAX_MEMBER)
				++n;
			else
				CCLogMeg::getSingleton().WriteError("CCFightTeam::SetUsersGuard n+1 < CLAN_MAX_MEMBER");
		}
	}

	for (it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pUser =0;
		if (CCUserManager::getSingleton().Find(it->first, &pUser))
		{
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,	SUB_G_FIGHTTEAM_GUARD, (CT_CHAR*)dwGuardList, sizeof(dwGuardList));
		}
	}
}

CT_VOID CCFightTeam::SendMemberGuard(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it;
	it = m_FTMemberMap.find(dwUserID);
	if (it != m_FTMemberMap.end())
	{
		it->second.iStatus = it->second.iStatus | 1;	//将末位设1,标识忙
	}
}

CT_VOID CCFightTeam::MemberRef(const CT_DWORD dwUser, const CT_INT32 iGrade, const CT_INT32 iFightingPower)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it;
	CCUser *pUserftr =0;
	it = m_FTMemberMap.find(dwUser);
	if (it == m_FTMemberMap.end())
	{
		return;
	}
	it->second.iGrade = iGrade;
	it->second.iFightingPower = iFightingPower;
	if (CCUserManager::getSingleton().Find(dwUser, &pUserftr))
	{
		it->second.iTitleID = pUserftr->m_userTitle.GetUseTitleID();
	} 

	MSC_DB_FightTeamMember dbFightTeamMember = {0};
	memcpy(&dbFightTeamMember, &(it->second), sizeof(FightTeamMember));
	dbFightTeamMember.iType = 1;
	CT_DWORD threadID = GAME_THREAD_DATABASE + it->first % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));

	
	CMD_G_FightTeamMemberRef ftMemberRef = {0};
	ftMemberRef.dwUserID = dwUser;
	ftMemberRef.iGrade = iGrade;
	ftMemberRef.iFightingPower = iFightingPower;
	ftMemberRef.iTitle = it->second.iTitleID;

	for (it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pUser =0;
		if (CCUserManager::getSingleton().Find(it->first, &pUser))
		{
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,	SUB_G_FIGHTTEAM_MEMBER_REF, (CT_CHAR*)&ftMemberRef, sizeof(CMD_G_FightTeamMemberRef));
		}
	}
}


CT_VOID	CCFightTeam::FightTeamUpGrade()	//战队升级
{
	_AUTOMUTEX(&m_mutex);

	//读取脚本判断资源,等级
	G_FightTeamUpgrade ftug={0};
	if(!G_GetFightTeamUpgrade(m_stBase.iGrade+1,FIGHTTEAM_BOSS,&ftug))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::FightTeamUpGrade G_GetFightTeamUpgrade Grade:%d",m_stBase.iGrade+1);
		return;
	}

	G_CostRewardGroup crg={0};
	if(!G_GetCostRewardGroup(ftug.iItemGroupID,&crg) && ftug.iItemGroupID != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::FightTeamUpGrade G_GetCostRewardGroup ItemGroupID:%d",ftug.iItemGroupID);
		return;
	}

	m_stBase.iGrade+=1;

	for (CT_INT32 i = 0; i < sizeof(crg.res) / sizeof(*crg.res); ++i)
	{
		if (crg.res[i].iID == VALUE_TYPE_FT_RES) 
		{
			m_stBase.iResource+=crg.res[i].iNum;
		}
		else if (crg.res[i].iID == VALUE_TYPE_FT_ITEM) 
		{
			m_stBase.iItemCount+=crg.res[i].iNum;
		}

	}
	
	

	//战队信息
	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &m_stBase, sizeof(MSC_DB_FightTeamBase));
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	CT_CHAR cFightTeamName[NAME_SIZE] = {0};
	CT_DWORD dwBossID = 0;
	CT_CHAR cBossName[NAME_SIZE] = {0};
	RollBroadcast rb[3]={0};
	dwBossID = GetFightTeamBossID();
	GetFightTeamName(cFightTeamName);
	GetFightTeamBossName(cBossName);
	rb[0].SetValue(dwBossID, cBossName);
	rb[1].SetValue(0, cFightTeamName);
	rb[2].SetValue(m_stBase.iGrade, "");
	//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{58}<6:%s>{59}<6:%d>{60}", dwBossID, cBossName, cFightTeamName, m_stBase.iGrade);
	G_SendRollBroadecast(58,5,1,(CT_CHAR*)rb,sizeof(rb));

	RollBroadcast rb1[2]={0};
	CMD_G_FightTeamEventLog fightTeamEventLog={0};
	CT_CHAR cEventDesc[DESCRIPTION_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	fightTeamEventLog.iReport = 0;
	fightTeamEventLog.lDate = time(0);
	rb1[0].SetValue(0,cBossName);
	rb1[1].SetValue(m_stBase.iGrade,"");
	//sprintf(cEventDesc, "<%s>{32}%d{33}", cBossName,m_stBase.iGrade);
	G_GetRollBroadecastData(102,cEventDesc,iEventDescLen,(CT_CHAR*)rb1,sizeof(rb1));
	memcpy(fightTeamEventLog.cEventDesc, cEventDesc, DESCRIPTION_SIZE);
	AddEventLog(fightTeamEventLog);

	//战队资源消耗
	MSC_LOGDB_FightTeamUpGrade logftug={0};
	logftug.dwFTID=m_stBase.dwFTID;
	logftug.dwUserID=m_stBase.dwBossUID;
	logftug.iGrade=m_stBase.iGrade;
	logftug.iItemRes=crg.item[0].iNum;
	logftug.iRes=crg.item[0].iNum;

	// 战队升级，判断每个成员加入、创建指定等级战队的任务
	CCUser *pMemUser;
	FightTeamMemberUID ftMember = {0};
	GetFightTeamMemberUID(&ftMember);
	for (CT_INT32 i = 0; i < ftMember.dwCount; ++i)
	{
		if (CCUserManager::getSingleton().Find(ftMember.dwMemberUID[i], &pMemUser))
		{
			CMD_G_TaskSystemInfo taskSysInfo = {0};
			taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
			taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_INFIGHTTEAM;
			taskSysInfo.iProgress = m_stBase.iGrade;
			pMemUser->m_userTask.DisposeTask(taskSysInfo);
		}
	}

	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_stBase.dwBossUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM_UPGRADE,(CT_CHAR*)&logftug,sizeof(logftug));

	return;
}

CT_VOID	CCFightTeam::UpdataFightTeamInfo(CT_DWORD dwUserID)	//更新战队信息
{
	_AUTOMUTEX(&m_mutex);
	CMD_G_FightTeamBase fightTeamBase = {0};
	GetFightTeamBase(&fightTeamBase);

	for (FTMember::iterator it = m_FTMemberMap.begin(); it != m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		{
			if(dwUserID == pMUser->GetUserID())
				pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_UP_GRADE_SUC,(CT_CHAR*)&fightTeamBase, sizeof(fightTeamBase));
			else
			{
				pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_UP_GRADE,(CT_CHAR*)&fightTeamBase, sizeof(fightTeamBase));
			}
		}
	}
	return;
}

//队员贡献消耗
CT_VOID	CCFightTeam::CostContribute(CT_DWORD dwUserID,CT_INT32 iContribute)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it = m_FTMemberMap.find(dwUserID);
	if (it != m_FTMemberMap.end())
	{
		it->second.iContribution += iContribute;

		MSC_DB_FightTeamMember dbFightTeamMember = {0};
		memcpy(&dbFightTeamMember, &(it->second), sizeof(FightTeamMember));
		dbFightTeamMember.iType = 1;
		CT_DWORD threadID = GAME_THREAD_DATABASE + it->first % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE,(CT_CHAR*)&dbFightTeamMember,sizeof(MSC_DB_FightTeamMember));
	}
}

//设置召唤信息
CT_VOID	CCFightTeam::SetSummonInfo(FightTeamSummonBoss &FTSB)
{
	_AUTOMUTEX(&m_mutex);
	if(m_FTSummonInfo.dwUserID[0] != 0)
	{
		for(CT_INT32 i=0;i<SUMMON_MAX;++i)
		{
			if(m_FTSummonInfo.dwUserID[i] == FTSB.dwUserID[0])return;
			if(m_FTSummonInfo.dwUserID[i] == 0)
			{
				m_FTSummonInfo.dwUserID[i]=FTSB.dwUserID[0];
                break;
			}
		}
	}
	else
	{
		memcpy(&m_FTSummonInfo,&FTSB,sizeof(m_FTSummonInfo));
	}
	return;
}

CT_INT32 CCFightTeam::GetSummonCount()
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 iCount=0;
	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(m_FTSummonInfo.dwUserID[i] != 0)
		{
			iCount++;
		}
	}

	return iCount;
}

//获取召唤信息
CT_BOOL	CCFightTeam::GetSummonInfo(FightTeamSummonBoss &FTSB)
{
	_AUTOMUTEX(&m_mutex);	
	if(m_FTSummonInfo.dwUserID[0] != 0)
	{
		memcpy(&FTSB,&m_FTSummonInfo,sizeof(FTSB));
		return CT_TRUE;
	}

	return CT_FALSE;
}

//助阵离开
CT_INT32 CCFightTeam::HelpSummonLeave(CT_DWORD dwUserID,FightTeamSummonBoss &FTSB)
{
	_AUTOMUTEX(&m_mutex);

	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(m_FTSummonInfo.dwUserID[i] == dwUserID && i == 0)
		{
			memset(&m_FTSummonInfo,0,sizeof(m_FTSummonInfo));
			return FT_SUMMON_LEAVE_SUC;
		}
		else if(m_FTSummonInfo.dwUserID[i] == dwUserID)
		{
			m_FTSummonInfo.dwUserID[i]=0;
			memcpy(&FTSB,&m_FTSummonInfo,sizeof(FTSB));
			return FT_SUMMON_LEAVE_SUC;
		}
	}

	return FT_MEMBER_INVALID;
}

//给召唤人员发送信息
CT_VOID CCFightTeam::SendSummonMessage(CT_DWORD dwUser[SUMMON_MAX],CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	_AUTOMUTEX(&m_mutex);
	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		CCUser *pUser=0;
		if(CCUserManager::getSingleton().Find(dwUser[i],&pUser))
		{
			pUser->GetNetInterface()->Send(mainID,subID,pBuf,len);
		}
	}
	return;
}

//设置召唤奖励
CT_VOID	CCFightTeam::SetSummonReward(CT_INT32 iUserIndex,CT_INT32 iChestIndex,CMD_G_NormalReward &Reward,CT_INT32 iItem[GOODS_COUNT*2+2],CT_INT32 iItemCount[GOODS_COUNT*2+2])
{
	_AUTOMUTEX(&m_mutex);

	if(iUserIndex >= SUMMON_MAX)
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::SetSummonReward iIndex >= SUMMON_MAX");
		return;
	}

	if(iChestIndex > SUMMON_MAX*2)
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::SetSummonReward iChestIndex > SUMMON_MAX*2");
		return;
	}

	if(m_FTSummonInfo.dwUserID[0] != 0)
	{
		m_FTSummonInfo.Res[iUserIndex].dwUserID=Reward.dwUserID;
		if(Reward.iResType0 != 0)
		{
			m_FTSummonInfo.Res[iUserIndex].iResNum0+=Reward.iResNum0;
			m_FTSummonInfo.Res[iUserIndex].iResType0=Reward.iResType0;
		}
		if(Reward.iResType1 != 0)
		{
			m_FTSummonInfo.Res[iUserIndex].iResNum1+=Reward.iResNum1;
			m_FTSummonInfo.Res[iUserIndex].iResType1=Reward.iResType1;
		}
		if(Reward.iResType2 != 0)
		{
			m_FTSummonInfo.Res[iUserIndex].iResNum2+=Reward.iResNum2;
			m_FTSummonInfo.Res[iUserIndex].iResType2=Reward.iResType2;
		}
		if(Reward.iResType3 != 0)
		{
			m_FTSummonInfo.Res[iUserIndex].iResNum3+=Reward.iResNum3;
			m_FTSummonInfo.Res[iUserIndex].iResType3=Reward.iResType3;
		}

		for(CT_INT32 i=0;i<GOODS_COUNT*2+2;++i)
		{
			if(iItem[i] != 0 && iItemCount[i] != 0)
			{
				FightTeamSummonItem si={0};
				si.dwUserID=m_FTSummonInfo.dwUserID[iUserIndex];
				si.iChestIndex=iChestIndex;
				si.iCount=iItemCount[i];
				si.iItemID=iItem[i];
				m_FTSummonRewardMap.insert(FTSummonRewardMap::value_type(si.dwUserID,si));

				MSC_DB_FightTeamSummInfoItem FTSI={0};
				FTSI.dwFTID=m_stBase.dwFTID;
				FTSI.dwUserID=si.dwUserID;
				FTSI.iItemID=si.iItemID;
				FTSI.iCount=si.iCount;
				FTSI.iChestIndex=si.iChestIndex;
				FTSI.iType=FIGHTTEAM_BOSS_SUMMON;
				CT_DWORD threadid = GAME_THREAD_DATABASE + si.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_SUMMON_ITEM,(CT_CHAR*)&FTSI,sizeof(FTSI));
				
			}
		}

		if(iChestIndex >= 0)
			m_FTSummonInfo.dwChestIndex[iChestIndex]=Reward.dwUserID;

		if(Reward.iResNum0 == 0 && Reward.iResNum1 == 0 && Reward.iResNum2 == 0 && Reward.iResNum3 == 0)return;
		MSC_DB_FightTeamSummonInfo ftsummoninfo={0};
		ftsummoninfo.dwFTID=m_stBase.dwFTID;
		ftsummoninfo.dwUserID=m_FTSummonInfo.dwUserID[iUserIndex];
		ftsummoninfo.iBossIndex=m_FTSummonInfo.iBossIndex;
		ftsummoninfo.iWin=FT_SUMMON_WIN;
		memcpy(&ftsummoninfo.Res,&Reward,sizeof(Reward));
		CT_DWORD threadid = GAME_THREAD_DATABASE + m_FTSummonInfo.dwUserID[iUserIndex] % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_SUMMON_DATA,(CT_CHAR*)&ftsummoninfo,sizeof(ftsummoninfo));
	}
	return;
}

//获取战队召唤奖励
CT_BOOL	CCFightTeam::GetSummonRewardItem(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &len)
{
	_AUTOMUTEX(&m_mutex);
    if(m_FTSummonRewardMap.size() == 0 || len < sizeof(FightTeamSummonItem)*m_FTSummonRewardMap.size())
        return CT_FALSE;

	CT_INT32 iSize=0;
	FTSummonRewardMap::iterator sritend=m_FTSummonRewardMap.end();
	for(FTSummonRewardMap::iterator it=m_FTSummonRewardMap.begin();it != sritend;++it)
	{
		memcpy(pBuf+iSize,(&(*it).second),sizeof((*it).second));
		iSize+=sizeof((*it).second);
	}
    len=iSize;
    return CT_TRUE;
}

//获取战队召唤奖励
CT_BOOL	CCFightTeam::GetSummonRewardRes(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &len)
{
	_AUTOMUTEX(&m_mutex);
	if(len < sizeof(CMD_G_FightTeamSummonRewardInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::GetSummonRewardRes len < sizeof(CMD_G_FightTeamSummonRewardInfo)");
		return CT_FALSE;
	}
	CMD_G_FightTeamSummonRewardInfo ftri={0};
	ftri.dwUserID=dwUserID;

	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(m_FTSummonInfo.Res[i].dwUserID == dwUserID)
		{
			memcpy(&ftri.Res,&m_FTSummonInfo.Res[i],sizeof(ftri.Res));
			memcpy(pBuf,&ftri,sizeof(ftri));
			len=sizeof(ftri);
			return CT_TRUE;
		}
	}

	return CT_FALSE;
}

CT_VOID CCFightTeam::SetMemberTowerCP(CT_DWORD dwUID, CT_INT32 iMaxCPID)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it;
	it = m_FTMemberMap.find(dwUID);
	if (it != m_FTMemberMap.end())
	{
		it->second.iTowerCPID = iMaxCPID;
		CMD_G_TowerMaxPass towerMaxPass = {0};
		towerMaxPass.dwUserID = dwUID;
		towerMaxPass.iCPID = iMaxCPID;
		Broadcast(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_TOWER_MAXCPID, (CT_CHAR*)&towerMaxPass, sizeof(CMD_G_TowerMaxPass));
	}
}

CT_INT32 CCFightTeam::GetTowerPassCount(CT_INT32 iCPID)
{
	_AUTOMUTEX(&m_mutex);
	FTMember::iterator it = m_FTMemberMap.begin();
	FTMember::iterator it_end = m_FTMemberMap.end();
	CT_INT32 iCount = 0;
	for (; it != it_end; it++)
	{
		if (it->second.iTowerCPID >= iCPID)
		{
			iCount++;
		}
	}
	return iCount;
}

CT_VOID	CCFightTeam::SetSummonWin(CT_INT32 iBossIndex)
{
	_AUTOMUTEX(&m_mutex);
	if(m_FTSummonInfo.dwUserID[0] != 0 && m_stBase.iBossMaxIndex < iBossIndex)
	{
		m_stBase.iBossMaxIndex=iBossIndex;
	}

	m_FTSummonInfo.iWiner=FT_SUMMON_WIN;
	m_stBase.iSummonCount++;
	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &m_stBase, sizeof(FightTeamBase));
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	G_FightTeamUpgrade ug={0};
	if(G_GetFightTeamUpgrade(iBossIndex,FIGHTTEAM_BOSS,&ug))
	{
		G_CostRewardGroup crg={0};
		if(G_GetCostRewardGroup(ug.iBossExpend,&crg) && ug.iBossExpend != 0)
		{
			MSC_LOGDB_FightTeamSummonCost FTUG={0};
			FTUG.dwFTID=m_stBase.dwFTID;
			FTUG.dwUserID=m_stBase.dwBossUID;
			FTUG.iBossIndex=iBossIndex;
			FTUG.iItemRes=crg.res[0].iNum;
			FTUG.iRes=crg.res[1].iNum;
			CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_stBase.dwBossUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM_SUMMONCOST,(CT_CHAR*)&FTUG,sizeof(FTUG));

		}
		else 
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeam::SetSummonWin G_GetCostRewardGroup costID:%d",ug.iBossExpend);
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::G_GetFightTeamUpgrade G_GetCostRewardGroup Grade:%d",iBossIndex);
	}
	return;
}

//清理召唤数据
CT_VOID CCFightTeam::ClearSummonData()
{
	_AUTOMUTEX(&m_mutex);
	m_stBase.iSummonCount=0;
	m_FTSummonRewardMap.clear();
	memset(&m_FTSummonInfo,0,sizeof(m_FTSummonInfo));

	FTMember::iterator itend=m_FTMemberMap.end();
	for(FTMember::iterator it=m_FTMemberMap.begin();it != itend;++it)
	{
		CCUser *pUser=0;
		if(CCUserManager::getSingleton().Find((*it).second.dwUserID,&pUser))
		{
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_CLEAR);
		}

		memset((*it).second.iContributeCount,0,sizeof((*it).second.iContributeCount));
	}
	return;
}
CT_VOID	CCFightTeam::SetTimeBossInfo(FTTimeBossInfo &tbi)
{
	_AUTOMUTEX(&m_mutex);
	memcpy(&m_FTTimeBossInfo,&tbi,sizeof(m_FTTimeBossInfo));

	//写入数据库
	MSC_DB_FTTimeBossInfo dbtbi={0};
	dbtbi.dwFTID=m_stBase.dwFTID;
	dbtbi.iIndex=m_FTTimeBossInfo.iIndex;
	dbtbi.iCurrentHP=m_FTTimeBossInfo.iCurrentHP;
	dbtbi.iSummonBeginTime=m_FTTimeBossInfo.iSummonBeginTime;
	memcpy(dbtbi.iHelpUser,m_FTTimeBossInfo.iHelpUser,sizeof(dbtbi.iHelpUser));
	G_SendDBThreadInfo(0,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_TIMEBOSS_INFO,(CT_CHAR*)&dbtbi,sizeof(dbtbi));
}

//更新时限boss成员信息
CT_VOID	CCFightTeam::UpdataTimeBossMember(FTTimeBossMember &bm)
{
	_AUTOMUTEX(&m_mutex);
	TBMemberMap::iterator it=m_TBMemberMap.find(bm.dwUserID);
	if(it != m_TBMemberMap.end())
	{
		(*it).second=bm;
	}
	else
	{
		m_TBMemberMap.insert(TBMemberMap::value_type(bm.dwUserID,bm));
	}

	//写入数据库
	MSC_DB_FTTimeBossMember tbm={0};
	tbm.dwFTID=m_stBase.dwFTID;
	tbm.dwUserID=bm.dwUserID;
	tbm.iFightNum=bm.iFightNum;
	tbm.iFTContribute=bm.iFTContribute;
	tbm.iFTFood=bm.iFTFood;
	tbm.iHurt=bm.iHurt;
	G_SendDBThreadInfo(0,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_TIMEBOSS_MUMBER,(CT_CHAR*)&tbm,sizeof(tbm));
	return;
}

//获取时限boss成员信息
CT_BOOL	CCFightTeam::GetTimeBossMember(CT_DWORD dwUserID,FTTimeBossMember &bm)
{
	_AUTOMUTEX(&m_mutex);
	TBMemberMap::iterator it=m_TBMemberMap.find(dwUserID);
	if(it != m_TBMemberMap.end())
	{
		bm=(*it).second;
		return CT_TRUE;
	}
	return CT_FALSE;
}

//删除时限boss成员信息
CT_VOID	CCFightTeam::DeleteTBMember(CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	m_TBMemberMap.erase(dwUserID);
	return;
}

//清楚时限BOSS战斗次数
CT_VOID	CCFightTeam::ClearTBMemberFightCount()
{
	_AUTOMUTEX(&m_mutex);
	for(TBMemberMap::iterator it=m_TBMemberMap.begin();it != m_TBMemberMap.end();++it)
	{
		(*it).second.iFightNum=0;
	}

	//写入数据库
	return;
}
//时限boss奖励
CT_BOOL	CCFightTeam::GetTBReward(CT_INT32 iIndex,FightTeamSummonItem &si)
{
	FTSummonRewardMap::iterator it=m_TBRewardMap.find(iIndex);
	if(it != m_TBRewardMap.end())
	{
		si=(*it).second;
		return CT_TRUE;
	}
	return CT_FALSE;
}

//时限boss奖励
CT_BOOL	CCFightTeam::GetTBReward(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	if(iLen <= sizeof(FightTeamSummonItem)*m_TBRewardMap.size())
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::GetTBReward iLen <= sizeof(FightTeamSummonItem)*m_TBRewardMap.size()");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	for(FTSummonRewardMap::iterator it=m_TBRewardMap.begin();it != m_TBRewardMap.end();++it)
	{
		memcpy(pBuf+iSize,&((*it).second),sizeof(FightTeamSummonItem));
		iSize+=sizeof(FightTeamSummonItem);
	}
	iLen=iSize;
	return CT_TRUE;
}

//时限BOSS当前血
CT_INT32 CCFightTeam::GetTBCurrentHP()
{
	CT_INT32 iHurt=0;
	for(TBMemberMap::iterator it=m_TBMemberMap.begin();it != m_TBMemberMap.end();++it)
	{
		iHurt+=(*it).second.iHurt;
	}
	return iHurt;
}

//时限BOSS最大血量
CT_INT32 CCFightTeam::GetTBMaxHP()
{
	CT_INT32 iMaxHp=0;
	G_FightTeamUpgrade ftu={0};
	G_MonsterBaseInfo MonsterInfo={0};
	G_MonsterGroup MonsterGroup={0};
	if(!G_GetFightTeamUpgrade(m_FTTimeBossInfo.iIndex,FIGHTTEAM_BOSS_TIME,&ftu))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::GetTBMaxHP G_GetFightTeamUpgrade boss:%d",m_FTTimeBossInfo.iIndex);
		return 0;
	}
	if(!G_GetMonsterGroup(0,ftu.iOpenBoss,&MonsterGroup))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::GetTBMaxHP G_GetMonsterGroup!");
		return 0;
	}
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(!CCFightingScriptSetting::getSingleton().GetMonsterData(MonsterGroup,i,MonsterInfo))
		{
			continue;
		}
		iMaxHp+=MonsterInfo.iBaseHP;
	}
	return iMaxHp;
}

//设置时限boss最大索引
CT_VOID	CCFightTeam::SetTBIndex(CT_INT32 iIndex)			
{
	_AUTOMUTEX(&m_mutex);
	m_stBase.iTimeBossMaxIndex=iIndex;

	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &m_stBase, sizeof(FightTeamBase));
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));
}

//清理时限boss信息
CT_VOID	CCFightTeam::ClearTBInfo()
{
	m_TBRewardMap.clear();
	m_TBMemberMap.clear();
	memset(&m_FTTimeBossInfo,0,sizeof(m_FTTimeBossInfo));

	MSC_DB_FTTBClear TBClear={0};
	TBClear.dwFTID=m_stBase.dwFTID;
	G_SendDBThreadInfo(0,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_CLEAR_TIMEBOSS,(CT_CHAR*)&TBClear,sizeof(TBClear));
	return;
}