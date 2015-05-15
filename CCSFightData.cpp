#include "CCSFightData.h"
#include "CLogMeg.h"
#include "CTop.h"

template<> CCCSFightData* CCSingleton<CCCSFightData>::ms_Singleton=0;
#define	CS_MAX_BUFF			100*1024
CCCSFightData::CCCSFightData(void)
{
	m_iStatus=CS_Close;
	m_iSeat=0;
}

CCCSFightData::~CCCSFightData(void)
{
	m_CSSupport.clear();
	m_CSFTCoolTime.clear();
	m_CSUserReward.clear();
}

//插入个人支持
CT_VOID CCCSFightData::InsertSupport(CSUserSupport &csus)
{
	_AUTOMUTEX(&m_Mutex);
	CT_INT32 iStatus=GetStatus()+1;
	CSSupportMap::iterator it=m_CSSupport.find(Get64Int(csus.dwUserID,iStatus));
	if(it == m_CSSupport.end())
	{
		m_CSSupport.insert(CSSupportMap::value_type(Get64Int(csus.dwUserID,iStatus),csus));

		FinalEightData fed={0};
		if(GetFTInfo(csus.dwFTID,csus.iGSID,fed))
		{
			CSSupport css={0};
			css.iNum=1;
			css.iStatus=iStatus;
			css.iPos=fed.fe.iPos;
			css.iResID=csus.iResID;
			InsertCSSupportData(0,css);

			//记录数据库
			MSC_DB_CS_SUPPORT dbcss={0};
			dbcss.dwUserID=csus.dwUserID;
			dbcss.dwFTID=fed.fe.FightTeamInfo.dwFTID;
			dbcss.iGSID=fed.fe.iGameServerID;
			dbcss.iResID=css.iResID;
			dbcss.iNum=css.iNum;
			dbcss.iIndex=csus.iIndex;
			CT_DWORD dwThreadID=GAME_THREAD_DATABASE+csus.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_CORSS_FIGHT,MSC_SUB_DB_CS_SUPPORT,(CT_CHAR*)&dbcss,sizeof(dbcss));
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCCSFightData::InsertSupport it != m_CSSupport.end() dwUserID:%d status:%d",csus.dwUserID,GetStatus());
	}
	return;
}

//插入冷却时间
CT_VOID CCCSFightData::InsertCoolTime(CT_DWORD dwFTID,CT_INT64 lTime)
{
	_AUTOMUTEX(&m_Mutex);
	CSFTCoolTimeMap::iterator it=m_CSFTCoolTime.find(dwFTID);
	if(it == m_CSFTCoolTime.end())
		m_CSFTCoolTime.insert(CSFTCoolTimeMap::value_type(dwFTID,lTime));
	else
	{
		(*it).second=lTime;
	}
	return;
}

//插入资格
CT_VOID CCCSFightData::InsertSeat(CT_INT32 iSeat)
{
	_AUTOMUTEX(&m_Mutex);
	m_iSeat=iSeat;

	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iLen=MAX_MES_MAX_BUFF_LEN;
	CCTop::getSingleton().GetTopFightingTeamInfo(cBuf,iLen);
	for(CT_INT32 i=0;i<iLen/sizeof(TopFightingTeam) && i < m_iSeat;++i)
	{
		TopFightingTeam tft={0};
		CSSeatFightReady cssfr={0};
		memcpy(&tft,cBuf+sizeof(TopFightingTeam)*i,sizeof(TopFightingTeam));

		cssfr.dwFTID=tft.dwFightingTeamID;
		cssfr.iGameServerID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
		memcpy(cssfr.cFightTeamName,tft.cFightingTeamName,NAME_SIZE);
		InsertSeatFightTeam(cssfr);
	}
}

//插入奖励
CT_VOID CCCSFightData::InsertReward(CT_DWORD dwUserID,CSUserReward &csur,CT_BOOL bSave)
{
	_AUTOMUTEX(&m_Mutex);
	CSUserRewardMap::iterator csurit=m_CSUserReward.find(dwUserID);
	if(csurit != m_CSUserReward.end())
	{
		CSRewardMap::iterator csrit=(*csurit).second.find(Get64Int(csur.iType,csur.iResID));
		if(csrit != (*csurit).second.end())
		{
			(*csrit).second.iNum+=csur.iNum;
		}
		else
		{
			(*csurit).second.insert(CSRewardMap::value_type(Get64Int(csur.iType,csur.iResID),csur));
		}
	}
	else
	{
		CSRewardMap csrmap;
		csrmap.insert(CSRewardMap::value_type(Get64Int(csur.iType,csur.iResID),csur));
		m_CSUserReward.insert(CSUserRewardMap::value_type(dwUserID,csrmap));
	}

	//记录数据库
	if(bSave)
	{
		MSC_DB_CS_REWARD csr={0};
		csr.dwUserID=dwUserID;
		csr.iResID=csur.iResID;
		csr.iResNum=csur.iNum;
		csr.iResType=csur.iType;

		CT_DWORD dwThreadID=GAME_THREAD_DATABASE+dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_CORSS_FIGHT,MSC_SUB_DB_CS_REWARD,(CT_CHAR*)&csr,sizeof(csr));
	}
	return;
}

//获取个人支持
CT_BOOL CCCSFightData::GetSupport(CT_DWORD dwUserID,CT_INT32 iStatus,CSUserSupport &csus)
{
	_AUTOMUTEX(&m_Mutex);
	CSSupportMap::iterator it=m_CSSupport.find(Get64Int(dwUserID,iStatus));
	if(it != m_CSSupport.end())
	{
		memcpy(&csus,&((*it).second),sizeof(csus));
		return CT_TRUE;
	}

	return CT_FALSE;
}

//获取冷却时间
CT_INT64 CCCSFightData::GetFTCoolTime(CT_DWORD dwFTID)
{
	_AUTOMUTEX(&m_Mutex);
	G_ValueRange vr={0};
	time_t t=time(0);
	G_GetValueRange(VALUE_TYPE_CS_COOL_TIME,&vr);
	CSFTCoolTimeMap::iterator it=m_CSFTCoolTime.find(dwFTID);
	if(it != m_CSFTCoolTime.end())
	{
		CT_INT64 lTime=(*it).second+vr.fMaxVal-t;
		return (lTime > 0?lTime:0);
	}
	return 0;
}

//获取资格
CT_INT32 CCCSFightData::GetSeat(CT_DWORD dwFTID)
{
	_AUTOMUTEX(&m_Mutex);
	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iLen=MAX_MES_MAX_BUFF_LEN;
	CCTop::getSingleton().GetTopFightingTeamInfo(cBuf,iLen);
	for(CT_INT32 i=0;i<iLen/sizeof(TopFightingTeam) && i < m_iSeat;++i)
	{
		TopFightingTeam tft={0};
		memcpy(&tft,cBuf+sizeof(TopFightingTeam)*i,sizeof(TopFightingTeam));
		if(dwFTID == tft.dwFightingTeamID)
			return Seat_True;
	}
	return Seat_False;
}

//
//获取准备信息(没好办法，暂时先这么做)
CT_INT32 CCCSFightData::GetOpponentPos(CT_DWORD dwFTID,CT_INT32 iGSID,CT_INT32 iStatus)
{
	_AUTOMUTEX(&m_Mutex);
	FinalEightData MyFed={0};
	if(!GetFTInfo(dwFTID,iGSID,MyFed) || dwFTID == 0)
	{
		//CCLogMeg::getSingleton().WriteError("CCCSFightData::GetOpponentPos GetFTInfo FTID:%d iGSID:%d",dwFTID,iGSID);
		return -1;
	}

	switch(iStatus)
	{
	case Final_Four:
		{
			FinalEightData Fed={0};
			CT_INT32 iPos=MyFed.fe.iPos/2;
			iPos=(MyFed.fe.iPos+1)%2+iPos*2;
			GetFinalEightData(Current_Record,iPos,Fed);
			return (Fed.fe.FightTeamInfo.dwFTID != 0?Fed.fe.iPos:-1);
		}
	case Final_Two:
		{
			for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT/2;++i)
			{
				FinalEightData Fed={0};
				CT_INT32 iPos=MyFed.fe.iPos/4;
				iPos=(MyFed.fe.iPos+i)%4+iPos*4;

				GetFinalEightData(Current_Record,iPos,Fed);
				if((Fed.fe.FightTeamInfo.dwFTID == dwFTID && Fed.fe.iGameServerID == MyFed.fe.iGameServerID) ||
					Fed.fe.FightTeamInfo.dwFTID == 0)
					continue;
				if(MyFed.fe.iWiner/10 == Fed.fe.iWiner/10 && Fed.fe.iWiner%10 == CS_WIN)
					return (Fed.fe.FightTeamInfo.dwFTID != 0?Fed.fe.iPos:-1);
			}
			return -1;
		}
	case Final_One:
		{
			for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT;++i)
			{
				FinalEightData Fed={0};

				GetFinalEightData(Current_Record,i,Fed);
				if((Fed.fe.FightTeamInfo.dwFTID == dwFTID && Fed.fe.iGameServerID == MyFed.fe.iGameServerID) ||
					Fed.fe.FightTeamInfo.dwFTID == 0)
					continue;
				if(MyFed.fe.iWiner/10 == Fed.fe.iWiner/10 && Fed.fe.iWiner%10 == CS_WIN)
					return Fed.fe.iPos;
			}
			return -1;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCSFightData::GetOpponentPos GetStatus():%d",GetStatus());
			return -1;
		}
	}

	return -1;
}



//获取奖励
CT_BOOL CCCSFightData::GetUserReward(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen <= sizeof(CSUserReward)*m_CSUserReward.size())
	{
		CCLogMeg::getSingleton().WriteError("CCCSFightData::GetUserReward iLen <= sizeof(CSUserReward)*m_CSUserReward.size()");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	CSUserRewardMap::iterator it=m_CSUserReward.find(dwUserID);
	if(it != m_CSUserReward.end())
	{
		CSRewardMap::iterator csritend=(*it).second.end();
		for(CSRewardMap::iterator csrit=(*it).second.begin();csrit != csritend;++csrit)
		{
			memcpy(pBuf+iSize,&((*csrit).second),sizeof(CSUserReward));
			iSize+=sizeof(CSUserReward);
		}
	}

	iLen=iSize;
	return CT_TRUE;
}

//支持奖励计算
CT_VOID CCCSFightData::SupportReward()
{
	_AUTOMUTEX(&m_Mutex);
	CSSupportMap::iterator itend=m_CSSupport.end();
	for(CSSupportMap::iterator it=m_CSSupport.begin();it != itend;++it)
	{
		CCCrossServerFight::FinalEightData fed={0};
		if(!GetFTInfo((*it).second.dwFTID,(*it).second.iGSID,fed))
		{
			continue;
		}
		if(fed.fe.iWiner%10 != WINER_LIFT || (*it).first != Get64Int((*it).second.dwUserID,GetStatus()))
		{
			continue;
		}

		G_CSFightBet csfb={0};
		if(G_GetCSFightBet(GetStatus(),(*it).second.iIndex,&csfb))
		{
			CSUserReward csur={0};
			csur.iResID=(*it).second.iResID;
			csur.iNum=(*it).second.iNum*csfb.fOdds;
			csur.iType=CLIENT_OBJ_RES;
			InsertReward((*it).second.dwUserID,csur);
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCCSFightData::SupportReward Status:%d index:%d",GetStatus(),(*it).second.iIndex);
		}
	}
	return;
}

//8强奖励计算
CT_VOID CCCSFightData::FinalEightReward(CT_INT32 iWin,CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);

	G_CSFinalEightReward csfer={0};
	if(!G_GetCSFinalEightReward(iWin/10,iWin%10,&csfer))
	{
		CCLogMeg::getSingleton().WriteError("CCCSFightData::FinalEightReward status:%d win%d",iWin/10,iWin%10);
		return;
	}

	Resoure  res[TEAM_RES_CNT] = {0}; 
	Item     item[TEAM_ITEM_CNT] = {0};
	CT_INT32 iGoods[GOODS_COUNT*2] = {0};
	CT_INT32 iCount[GOODS_COUNT*2] = {0};
	FakeProBase fb={0};
	G_GetReward(csfer.iRewardID,res,item,iGoods,iCount,fb);
	printf("CCCSFightData::FinalEightReward UserID:%d iRewardID:%d\n",dwUserID,csfer.iRewardID);

	for(CT_INT32 i=0;i<TEAM_RES_CNT;++i)
	{
		if(res[i].iID == 0)continue;

		CSUserReward csur={0};
		csur.iType=CLIENT_OBJ_RES;
		csur.iResID=res[i].iID;
		csur.iNum=res[i].iNum;
		InsertReward(dwUserID,csur);
	}

	for(CT_INT32 i=0;i<TEAM_ITEM_CNT;++i)
	{
		if(item[i].iID == 0)continue;

		CSUserReward csur={0};
		csur.iType=CLIENT_OBJ_ITEM;
		csur.iResID=item[i].iID;
		csur.iNum=item[i].iNum;
		InsertReward(dwUserID,csur);
	}

	for(CT_INT32 i=0;i<GOODS_COUNT*2;++i)
	{
		if(iGoods[i] == 0)continue;

		CSUserReward csur={0};
		csur.iType=CLIENT_OBJ_ITEM;
		csur.iResID=iGoods[i];
		csur.iNum=iCount[i];
		InsertReward(dwUserID,csur);
	}
	return;
}

//获取战队资格战战报
CT_BOOL CCCSFightData::GetCSFightRecord(CT_DWORD dwFTID,CT_INT32 iGSID,CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	CT_INT32 iSize=0;
	if(CCCrossServerFight::GetCSFightRecord(dwFTID,cBuf,iBufLen))
	{
		for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::CSFightRecord) && i < CROSS_SERVER_FIGHT_RECORD;++i)
		{
			CCCrossServerFight::CSFightRecord csfr={0};
			memcpy(&csfr,cBuf+i*sizeof(csfr),sizeof(csfr));
			if(csfr.iGameServerID[0] == iGSID || csfr.iGameServerID[1] == iGSID)
			{
				memcpy(pBuf+iSize,&csfr,sizeof(csfr));
				iSize+=sizeof(csfr);
			}
		}
		iLen=iSize;
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获取8强战报
CT_BOOL CCCSFightData::GetCSFightRecord(CT_CHAR cIndex[],CT_INT32 iIndexLen,CSFightRecord &csfr)
{
	_AUTOMUTEX(&m_Mutex);
	if(CCCrossServerFight::GetCSFightRecord(cIndex,iIndexLen,csfr))
		return CT_TRUE;
	return CT_FALSE;
}

//清除奖励数据
CT_VOID CCCSFightData::ClearReward()
{
	_AUTOMUTEX(&m_Mutex);
	CSUserRewardMap::iterator itend=m_CSUserReward.end();
	for(CSUserRewardMap::iterator it=m_CSUserReward.begin();it !=itend;++it)
	{
		for(CSRewardMap::iterator rmit=(*it).second.begin();rmit != (*it).second.end();++rmit)
		{
			MSC_DB_CS_DelReward csdr={0};
			csdr.dwUserID=(*it).first;
			csdr.iResID=(*rmit).second.iResID;
			csdr.iResType=(*rmit).second.iType;
			//清除数据库
			CT_DWORD dwThreadID=GAME_THREAD_DATABASE+csdr.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_CORSS_FIGHT,MSC_SUB_DB_DELREWARD,(CT_CHAR*)&csdr,sizeof(csdr));
		}
	}
	m_CSUserReward.clear();

	
	return;
}

//清楚奖励
CT_VOID CCCSFightData::ClearReward(CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);
	CSUserRewardMap::iterator it=m_CSUserReward.find(dwUserID);
	if(it != m_CSUserReward.end())
	{
		for(CSRewardMap::iterator rmit=(*it).second.begin();rmit != (*it).second.end();++rmit)
		{
			MSC_DB_CS_DelReward csdr={0};
			csdr.dwUserID=(*it).first;
			csdr.iResID=(*rmit).second.iResID;
			csdr.iResType=(*rmit).second.iType;
			//清除数据库
			CT_DWORD dwThreadID=GAME_THREAD_DATABASE+csdr.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_CORSS_FIGHT,MSC_SUB_DB_DELREWARD,(CT_CHAR*)&csdr,sizeof(csdr));
		}
	}

	m_CSUserReward.erase(dwUserID);

	//清除数据库
	return;
}