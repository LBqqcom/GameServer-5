#include "CCrossServerFight.h"
#include "CLogMeg.h"

CCCrossServerFight::CCCrossServerFight(void)
{

}

CCCrossServerFight::~CCCrossServerFight(void)
{
	m_FinalEightData.clear();			
	m_LastFinaEightData.clear();		
	m_FinalEightRecord.clear();			
	m_LastFinalEightRecord.clear();		
	m_SeatFightTeam.clear();			
	m_SupportData.clear();				
	m_FightRecord.clear();				
}

//插入资格战队
CT_VOID CCCrossServerFight::InsertSeatFightTeam(CSSeatFightReady &cssfr)
{
	_AUTOMUTEX(&m_Mutex);
	CT_INT64 lKey=Get64Int(cssfr.iGameServerID,cssfr.dwFTID);
	SeatFightTeamMap::iterator it=m_SeatFightTeam.find(lKey);
	if(it == m_SeatFightTeam.end())
	{
		m_SeatFightTeam.insert(SeatFightTeamMap::value_type(lKey,cssfr));
		
	}
	else
	{
		//CCLogMeg::getSingleton().WriteError("CCCrossServerFight::InsertSeatFightTeam it != m_SeatFightTeam.end() GameServerID:%d FTID:%d",cssfr.iGameServerID,cssfr.dwFTID);
	}
	return;
}

//插入8强数据
CT_VOID CCCrossServerFight::InsertFinalEightData(CT_INT32 iIndex,FinalEightData &fed)
{
	_AUTOMUTEX(&m_Mutex);
	if(iIndex == Current_Record)
	{
		FinalEightDataMap::iterator it=m_FinalEightData.find(fed.fe.iPos);
		if(it == m_FinalEightData.end())
		{
			m_FinalEightData.insert(FinalEightDataMap::value_type(fed.fe.iPos,fed));
		}
		else
		{
			(*it).second=fed;
		}
	}
	else if(iIndex == Last_Rcord)
	{
		FinalEightDataMap::iterator it=m_LastFinaEightData.find(fed.fe.iPos);
		if(it == m_LastFinaEightData.end())
		{
			m_LastFinaEightData.insert(FinalEightDataMap::value_type(fed.fe.iPos,fed));
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFight::InsertFinalEightData Last_Rcord pos:%d fightteam name:%s",fed.fe.iPos,fed.fe.FightTeamInfo.cFightTeamName);
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::InsertFinalEightData Index:%d",iIndex);
	}
	return;
}

//插入8强战斗记录
CT_VOID CCCrossServerFight::InsertCSFightRecord(CT_INT32 iIndex,CSFightRecord &csfr)
{
	_AUTOMUTEX(&m_Mutex);

	CT_CHAR cDelIndex[CROSS_SERVER_RECORD_INDEX+1]={0};

	for(CT_INT32 i=0;i<2;++i)
	{
		RecordIndex ri={0};
		ri.dwFTID=csfr.dwFTID[i];
		memcpy(ri.cIndex,csfr.cIndex,CROSS_SERVER_RECORD_INDEX);

		if(iIndex == Current_Record)
		{
			CSFightRecordMap::iterator it=m_FinalEightRecord.find(csfr.dwFTID[i]);
			if(it == m_FinalEightRecord.end())
			{
				RecordList rl;
				rl.push_back(ri);
				m_FinalEightRecord.insert(CSFightRecordMap::value_type(csfr.dwFTID[i],rl));
			}
			else
			{
				(*it).second.push_back(ri);
				if((*it).second.size() > CROSS_SERVER_FIGHT_RECORD)
				{
					RecordList::iterator rlit=(*it).second.begin();
					memcpy(cDelIndex,(*rlit).cIndex,CROSS_SERVER_RECORD_INDEX);
					(*it).second.erase(rlit);
				}
			}
		}
		else if(iIndex == Last_Rcord)
		{
			CSFightRecordMap::iterator it=m_LastFinalEightRecord.find(csfr.dwFTID[i]);
			if(it == m_LastFinalEightRecord.end())
			{
				RecordList rl;
				rl.push_back(ri);
				m_LastFinalEightRecord.insert(CSFightRecordMap::value_type(csfr.dwFTID[i],rl));
			}
			else
			{
				(*it).second.push_back(ri);
			}
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFight::InsertFinalEightData Index:%d",iIndex);
		}
	}

	m_FightRecord.erase(cDelIndex);
	m_FightRecord.insert(FightRecordMap::value_type(csfr.cIndex,csfr));
	return;
}

//插入资格战斗记录
CT_VOID CCCrossServerFight::InsertCSFightRecordUser(CT_CHAR cIndex[],CT_INT32 iGroup,CT_INT32 iPos,UserFightingStatus &ufs)
{
	_AUTOMUTEX(&m_Mutex);
	FightRecordMap::iterator it=m_FightRecord.find(cIndex);
	if(it != m_FightRecord.end())
	{
		if(ufs.dwUserID != 0)
		{
			CT_INT32 iTempGroup=iGroup%2;
			CT_INT32 iTempPos=iPos%CLAN_CRUSADE_MEMBER;
			(*it).second.ufs[iTempGroup][iTempPos]=ufs;
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::InsertCSFightRecordUser it == m_FightRecord.end()");
	}
	return;
}

//插入8强战斗记录(进击赛索引特殊定义)
CT_VOID CCCrossServerFight::InsertCSFinalEightRecord(CSFightRecord &csfr)
{
	_AUTOMUTEX(&m_Mutex);
	FightRecordMap::iterator it=m_FightRecord.find(csfr.cIndex);
	if(it == m_FightRecord.end())
		m_FightRecord.insert(FightRecordMap::value_type(csfr.cIndex,csfr));
	else
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::InsertCSFinalEightRecord index:%s",csfr.cIndex);
	return;
}

//插入资格战斗记录
CT_VOID CCCrossServerFight::InsertCSFightEightRecordUser(CT_CHAR cIndex[],CT_INT32 iGroup,CT_INT32 iPos,UserFightingStatus &ufs)
{
	_AUTOMUTEX(&m_Mutex);
	FightRecordMap::iterator it=m_FightRecord.find(cIndex);
	if(it != m_FightRecord.end())
	{
		if(ufs.dwUserID != 0)
		{
			CT_INT32 iTempGroup=iGroup%2;
			CT_INT32 iTempPos=iPos%CLAN_CRUSADE_MEMBER;
			(*it).second.ufs[iTempGroup][iTempPos]=ufs;
		}
	}

	return;
}

//插入支持信息
CT_VOID CCCrossServerFight::InsertCSSupportData(CT_INT32 iStatus,CSSupport &css)
{
	_AUTOMUTEX(&m_Mutex);
	CSSupportDataMap::iterator it=m_SupportData.find(Get64Int(iStatus,css.iPos));
	if(it != m_SupportData.end())
	{
		(*it).second.iNum+=css.iNum;
	}
	else
	{
		m_SupportData.insert(CSSupportDataMap::value_type(Get64Int(iStatus,css.iPos),css));
	}

	return;
}

//获取资格赛战队
CT_BOOL CCCrossServerFight::GetSeatFightTeam(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen <= m_SeatFightTeam.size()*sizeof(CSSeatFightReady))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetSeatFightTeam iLen <= m_SeatFightTeam.size()*sizeof(CSSeatFightReady)");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	SeatFightTeamMap::iterator itend=m_SeatFightTeam.end();
	for(SeatFightTeamMap::iterator it=m_SeatFightTeam.begin();it != itend;++it,++iSize)
	{
		memcpy(pBuf+iSize*sizeof(CSSeatFightReady),&((*it).second),sizeof(CSSeatFightReady));
	}
	iLen=iSize*sizeof(CSSeatFightReady);
	return CT_TRUE;
}

//获取8强数据
CT_BOOL CCCrossServerFight::GetFinalEightData(CT_INT32 iIndex,CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(m_FinalEightData.size()*sizeof(FinalEightData) > iLen || m_LastFinaEightData.size()*sizeof(FinalEightData) > iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetFinalEightData m_FinalEightData.size()*sizeof(FinalEightData) >= iLen");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	if(iIndex == Current_Record)
	{
		FinalEightDataMap::iterator itend=m_FinalEightData.end();
		for(FinalEightDataMap::iterator it=m_FinalEightData.begin();it != itend;++it,++iSize)
		{
			memcpy(pBuf+sizeof(FinalEightData)*iSize,&((*it).second),sizeof(FinalEightData));
		}
	}
	else if(iIndex == Last_Rcord)
	{
		FinalEightDataMap::iterator itend=m_LastFinaEightData.end();
		for(FinalEightDataMap::iterator it=m_LastFinaEightData.begin();it != itend;++it,++iSize)
		{
			memcpy(pBuf+sizeof(FinalEightData)*iSize,&((*it).second),sizeof(FinalEightData));
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetFinalEightData Index:%d",iIndex);
	}

	iLen=sizeof(FinalEightData)*iSize;
	return CT_TRUE;
}

//获取8强数据
CT_BOOL CCCrossServerFight::GetFinalEightData(CT_INT32 iIndex,CT_INT32 iPos,FinalEightData &fed)
{
	_AUTOMUTEX(&m_Mutex);
	FinalEightDataMap::iterator it=m_FinalEightData.find(iPos);
	if(it != m_FinalEightData.end())
	{
		memcpy(&fed,&((*it).second),sizeof(fed));

		if(fed.fe.FightTeamInfo.dwFTID != 0)
			return CT_TRUE;
	}
	return CT_FALSE;
}

//获取8强战报
CT_BOOL CCCrossServerFight::GetCSFightRecord(CT_INT32 iIndex,CT_DWORD dwFTID,CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(m_FinalEightRecord.size()*sizeof(CSFightRecord) >= iLen || m_LastFinalEightRecord.size()*sizeof(CSFightRecord) >= iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetCSFightRecord m_FinalEightRecord.size()*sizeof(CSFightRecord) >= iLen");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	if(iIndex == Current_Record)
	{
		CSFightRecordMap::iterator it=m_FinalEightRecord.find(dwFTID);
		if(it != m_FinalEightRecord.end())
		{
			RecordList::iterator ritend=(*it).second.end();
			for(RecordList::iterator rit=(*it).second.begin();rit != ritend;++rit,++iSize)
			{
				FightRecordMap::iterator frit=m_FightRecord.find((*rit).cIndex);
				if(frit != m_FightRecord.end())
				{
					memcpy(pBuf+iSize*sizeof(CSFightRecord),&((*rit)),sizeof(CSFightRecord));
				}
			}
		}
	}
	else if(iIndex == Last_Rcord)
	{
		CSFightRecordMap::iterator it=m_LastFinalEightRecord.find(dwFTID);
		if(it != m_LastFinalEightRecord.end())
		{
			RecordList::iterator ritend=(*it).second.end();
			for(RecordList::iterator rit=(*it).second.begin();rit != ritend;++rit,++iSize)
			{
				FightRecordMap::iterator frit=m_FightRecord.find((*rit).cIndex);
				if(frit != m_FightRecord.end())
				{
					memcpy(pBuf+iSize*sizeof(CSFightRecord),&((*rit)),sizeof(CSFightRecord));
				}
			}
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetCSFightRecord Index:%d",iIndex);
	}

	iLen=iSize*sizeof(CSFightRecord);
	return CT_TRUE;
}

//获取8强战报
CT_BOOL CCCrossServerFight::GetCSFightRecord(CT_CHAR cIndex[],CT_INT32 iIndexLen,CSFightRecord &csfr)
{
	_AUTOMUTEX(&m_Mutex);
	if(iIndexLen < CROSS_SERVER_RECORD_INDEX)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetCSFightRecord iIndexLen <= CROSS_SERVER_RECORD_INDEX");
		return CT_FALSE;
	}

	FightRecordMap::iterator it=m_FightRecord.find(cIndex);
	if(it != m_FightRecord.end())
	{
		memcpy(&csfr,&((*it).second),sizeof(CSFightRecord));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获取战队资格战战报
CT_BOOL CCCrossServerFight::GetCSFightRecord(CT_DWORD dwFTID,CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	CT_INT32 icsfrSize=sizeof(CSFightRecord);
	if(iLen <= sizeof(CSFightRecord)*CROSS_SERVER_FIGHT_RECORD)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetCSFightRecord iLen <= sizeof(CSFightRecord)*CROSS_SERVER_FIGHT_RECORD");
		return CT_FALSE;
	}

	if(dwFTID == 0)
		return CT_FALSE;

	CT_INT32 iSize=0;
	CSFightRecordMap::iterator frit=m_FinalEightRecord.find(dwFTID);
	if(frit != m_FinalEightRecord.end())
	{
		for(RecordList::iterator rit=(*frit).second.begin();rit != (*frit).second.end();++rit)
		{
			FightRecordMap::iterator it=m_FightRecord.find((*rit).cIndex);
			if(it != m_FightRecord.end())
			{
				memcpy(pBuf+iSize*sizeof(CSFightRecord),&((*it).second),sizeof(CSFightRecord));
				iSize++;
			}
		}
	}
	iLen=iSize*sizeof(CSFightRecord);
	return CT_TRUE;
}

//获得支持
CT_BOOL CCCrossServerFight::GetCSSupportData(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(m_SupportData.size()*sizeof(CSSupport) >= iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFight::GetCSSupportData m_SupportData.size()*sizeof(CSSupport) >= iLen");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	CSSupportDataMap::iterator itend=m_SupportData.end();
	for(CSSupportDataMap::iterator it=m_SupportData.begin();it != itend;++it,++iSize)
	{
		memcpy(pBuf+iSize*sizeof(CSSupport),&((*it).second),sizeof(CSSupport));
	}
	iLen=iSize;
	return CT_TRUE;
}

//获得支持
CT_BOOL CCCrossServerFight::GetCSSupportData(CT_INT32 iStatus,CT_INT32 iPos,CSSupport &css)
{
	_AUTOMUTEX(&m_Mutex);
	CSSupportDataMap::iterator it=m_SupportData.find(Get64Int(iStatus,iPos));
	if(it != m_SupportData.end())
	{
		memcpy(&css,&((*it).second),sizeof(css));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获取战队信息
CT_BOOL CCCrossServerFight::GetFTInfo(CT_DWORD dwFTID,CT_INT32 iGSID,FinalEightData &fed)
{
	_AUTOMUTEX(&m_Mutex);
	FinalEightDataMap::iterator itend=m_FinalEightData.end();
	for(FinalEightDataMap::iterator it = m_FinalEightData.begin();it != itend;++it)
	{
		if((*it).second.fe.FightTeamInfo.dwFTID == dwFTID && iGSID == (*it).second.fe.iGameServerID)
		{
			memcpy(&fed,&((*it).second),sizeof(fed));
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}
//更新用户信息
CT_VOID CCCrossServerFight::UpdateUserData(CT_INT32 iGSID,UserFightingStatus &ufs,CSInspore &csi)
{
	_AUTOMUTEX(&m_Mutex);
	FinalEightDataMap::iterator itend=m_FinalEightData.end();
	for(FinalEightDataMap::iterator it=m_FinalEightData.begin();it != itend;++it)
	{
		for(CT_INT32 i=0;i<CLAN_CRUSADE_MEMBER;++i)
		{
			if((*it).second.ufs[i].dwUserID == ufs.dwUserID && (*it).second.fe.iGameServerID == iGSID)
			{
				(*it).second.csi[i]=csi;
				(*it).second.ufs[i]=ufs;
				return;
			}
		}
	}
	return;
}

//更新阵法信息
CT_VOID CCCrossServerFight::UpdateFTPos(CT_DWORD dwFTID,CT_INT32 iGSID,UserFightingStatus *pufs,CT_INT32 iCount)
{
	_AUTOMUTEX(&m_Mutex);
	FinalEightDataMap::iterator itend=m_FinalEightData.end();
	for(FinalEightDataMap::iterator it=m_FinalEightData.begin();it != itend;++it)
	{
		if((*it).second.fe.FightTeamInfo.dwFTID == dwFTID &&
			iGSID == (*it).second.fe.iGameServerID)
		{
			for(CT_INT32 i=0;i<CLAN_CRUSADE_MEMBER;i++)
			{
				CombatUnitData cud={0};
				GetMasterFightingData(pufs[i],cud);
				(*it).second.dwUserID[i]=pufs[i].dwUserID;
				memcpy((*it).second.cUserName[i],cud.name,NAME_SIZE);
				memcpy(&((*it).second.ufs[i]),&pufs[i],sizeof(UserFightingStatus));
			}
		}
	}
	return;
}


CT_INT64 CCCrossServerFight::Get64Int(CT_INT32 high,CT_INT32 low)
{
	CT_INT64 lKey=high;
	lKey<<=32;
	lKey|=low;
	return lKey;
}

//新比赛
CT_VOID CCCrossServerFight::NewMatch()
{
	_AUTOMUTEX(&m_Mutex);
	m_LastFinaEightData.clear();
	m_LastFinalEightRecord.clear();
	m_LastFinaEightData=m_FinalEightData;
	m_LastFinalEightRecord=m_FinalEightRecord;

	FightRecordMap frmap=m_FightRecord;
	m_FightRecord.clear();

	CT_INT64 iNum=Last_Index;
	for(CT_INT32 i=Current_Index;i<Last_Index;++i)
	{
		CT_CHAR cIndex[CROSS_SERVER_RECORD_INDEX+1]={0};
		sprintf(cIndex,"%d",i);

		FightRecordMap::iterator it=frmap.find(cIndex);
		if(it != frmap.end())
		{
			memset(cIndex,0,CROSS_SERVER_RECORD_INDEX);
			sprintf(cIndex,"%d",Last_Index-1+i);
			memcpy((*it).second.cIndex,cIndex,CROSS_SERVER_RECORD_INDEX);
			m_FightRecord.insert(FightRecordMap::value_type(cIndex,(*it).second));
		}
	}
	
	for(FightRecordMap::iterator it=frmap.begin();it != frmap.end();++it)
	{
		CT_CHAR cIndex[CROSS_SERVER_RECORD_INDEX+1]={0};
		if(strlen((*it).first.c_str()) >= CROSS_SERVER_RECORD_INDEX)
			m_FightRecord.insert(FightRecordMap::value_type((*it).first,(*it).second));
	}

	m_FinalEightData.clear();
	m_FinalEightRecord.clear();
	m_SupportData.clear();
	m_SeatFightTeam.clear();
	return;
}

//获取领主战斗属性
CT_VOID CCCrossServerFight::GetMasterFightingData(const UserFightingStatus &data,CombatUnitData &masterdata)
{
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(data.units[i].type < UNIT_ALL_ARMY && data.units[i].type != 0)
		{
			masterdata=data.units[i];
			break;
		}
	}
	return;
}
