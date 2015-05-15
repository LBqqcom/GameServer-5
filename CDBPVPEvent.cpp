#include "CDBPVPEvent.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "GlobeScriptInterface.h"
#include "CUserManager.h"
#include "CGameConfig.h"
#include "CDBLogMes.h"

#define IDI_TOP_REFRESH				1000
CCDBPVPEvent::CCDBPVPEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
							CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_R_PVP_INFO);
	RegistMessage(MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_W_REFRESH);
	RegistMessage(MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_FIGHTING_END);
	RegistMessage(MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_CHALLENGE_INFO);
	RegistMessage(MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_PVPENDING);

	
	RegistMessage(MSC_MAIN_DB_PVP_DATA, MSC_SUB_DB_PVP_CHALLGER_SAVE);
	

	RegistClock(IDI_TOP_REFRESH);
	SetClock(IDI_TOP_REFRESH,0,0,0);

	m_pUser = new CCUser;
}

CCDBPVPEvent::~CCDBPVPEvent(void)
{
	delete m_pUser;
	m_pUser=0;
}

//处理用户消息
CT_VOID CCDBPVPEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
							CT_CHAR* pBuf,		CT_DWORD len,
							CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_PVP_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBPVPEvent::OnUserEvent :mainID=%i", mainID);
		return;
	}
	switch(subID)
	{
	case MSC_SUB_DB_R_PVP_INFO:					//竞技场信息
		{
			DisposePVPInfo(pBuf,len);
			break;
		}
	case MSC_SUB_DB_W_REFRESH:					//刷新对手
		{
			DisposeRefresh(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTING_END:				//战斗记录
		{
			DisposeFightingEnd(pBuf,len);
			break;
		}
	case MSC_SUB_DB_CHALLENGE_INFO:				//挑战状况
		{
			DisposeChallengeInfo(pBuf,len);
			break;
		}
	case MSC_SUB_DB_PVPENDING:					//竞技场终结
		{
			DisposePVPEnding(pBuf,len);
			break;
		}
	case MSC_SUB_DB_PVP_CHALLGER_SAVE:
		{
			DisposePVPChallgerSave(pBuf, len); //保存对手数据
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBPVPEvent::OnUserEvent :subID=%i", subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBPVPEvent::OnTime(CT_DWORD timeID)
{
	return;
}
//用户时钟处理
CT_VOID CCDBPVPEvent::OnClock(CT_DWORD timeID)
{
	if(IDI_TOP_REFRESH == timeID)				//刷新排行榜
	{
		//m_dbConnector.Reset();
		//m_dbConnector.SetPcName("SP_PVPSysUpdateRanking");

		//if(m_dbConnector.Exec())
		//{
		//	CCDBResult* pResult = m_dbConnector.GetResult();
		//	pResult->Release();
		//}else
		//{
		//	G_DBError(m_dbConnector, m_dwThread);
		//	return;
		//}
	}
}
//竞技场信息
CT_VOID CCDBPVPEvent::DisposePVPInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_W_PVPInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPInfo :len != sizeof(MSC_DB_W_PVPInfo)");
		return;
	}

	MSC_DB_W_PVPInfo *pPVPinfo=(MSC_DB_W_PVPInfo*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPUpdateChallenge");

	m_dbConnector.AddParm("_uid", pPVPinfo->dwUserID);
	m_dbConnector.AddParm("_coolTime", pPVPinfo->lRefreshTime);
	m_dbConnector.AddParm("_Award", pPVPinfo->cbChestStatus);
	m_dbConnector.AddParm("_RaceNum", pPVPinfo->iPVPDegree);
	m_dbConnector.AddParm("_challengeTimes", pPVPinfo->iFightingCount);
	m_dbConnector.AddParm("_getTrsureTimes", pPVPinfo->iPVPChestNum);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pPVPinfo->dwUserID,"SP_PVPUpdateChallenge","_uid:%d _coolTime:%d _bLowAward:%d _challengeTimes:%d",
									pPVPinfo->dwUserID,pPVPinfo->lRefreshTime/*,pPVPinfo->cbChestStatus[1]*/,pPVPinfo->cbChestStatus,pPVPinfo->iFightingCount);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//刷新对手
CT_VOID CCDBPVPEvent::DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_Refresh))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPInfo :len != sizeof(MSC_DB_Refresh)");
		return;
	}

	MSC_DB_Refresh *pRefresh=(MSC_DB_Refresh*)pBuf;

	CT_INT32 iHCount=0;
	CT_INT32 iLCount=0;
	CT_DWORD dwUserGroupID[PVP_GROUP*2]={0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPGetHighPlayers");

	m_dbConnector.AddParm("_uid", pRefresh->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_DWORD dwUserID=0;
				pResult->GetValue(0, dwUserID );
				dwUserGroupID[iHCount]=dwUserID;

				if(iHCount < PVP_GROUP)
					iHCount++;
			}
		}
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pRefresh->dwUserID,"SP_PVPGetHighPlayers","_uid:%d",pRefresh->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	//读取低级组
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPGetLowPlayers");

	m_dbConnector.AddParm("_uid", dwUserGroupID[iHCount-1]);
	m_dbConnector.AddParm("_myuid", pRefresh->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_DWORD dwUserID=0;
				pResult->GetValue(0, dwUserID );
				dwUserGroupID[iHCount+iLCount]=dwUserID;

				if(iLCount < PVP_GROUP)
					iLCount++;
			}
		}
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pRefresh->dwUserID,"SP_PVPGetLowPlayers","_uid:%d",pRefresh->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRefresh->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDBPVPEvent::DisposeRefresh:%d No Find UserID",pRefresh->dwUserID);
		return;
	}

	CT_INT32 iThreadID = GAME_THREAD_CITY + pRefresh->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;

	//插入高级用户
	CT_INT32 iPlayerCount=(iHCount+iLCount >= PVP_GROUP?PVP_GROUP:iHCount+iLCount);
	CT_INT32 iRand=rand()%(iPlayerCount > PVP_CHALLENGE_COUNT/2?iPlayerCount-PVP_CHALLENGE_COUNT/2:1);
	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT/2;++i)
	{
		CT_INT32 iNum=iRand+i;
		if(dwUserGroupID[iNum] != 0)
		{
			MSC_PVP_InsertChallger pvp = {0};
			pvp.dwUserID = pRefresh->dwUserID;
			pvp.dwChallger = dwUserGroupID[iNum];
			pvp.iPos = i;
			G_SendThreadInfo(iThreadID, MSC_MAIN_EVENT_PVP, MSC_SUB_PVP_HIGN_CHALLER, (CT_CHAR*)&pvp , sizeof(pvp) );
			dwUserGroupID[iNum]=0;
		}
	}

	//插入低级用户
	iRand=rand()%(iPlayerCount > PVP_CHALLENGE_COUNT/2?iPlayerCount-PVP_CHALLENGE_COUNT/2:1)+(iHCount >= PVP_GROUP?PVP_GROUP:iHCount);
	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT/2;++i)
	{
		for(CT_INT32 j=0;j<iHCount+iLCount;++j)
		{
			CT_INT32 iNum=(iRand+j)%(PVP_GROUP)+(iLCount >= PVP_GROUP?iHCount:0);
			if(dwUserGroupID[iNum] != 0)
			{
				MSC_PVP_InsertChallger pvp = {0};
				pvp.dwUserID = pRefresh->dwUserID;
				pvp.dwChallger = dwUserGroupID[iNum];
				pvp.iPos = i;


			    G_SendThreadInfo(iThreadID, MSC_MAIN_EVENT_PVP, MSC_SUB_PVP_LOW_CHALLER, (CT_CHAR*)&pvp , sizeof(pvp) );
				dwUserGroupID[iNum]=0;
				break;
			}
		}
	}
	
	CT_DWORD dwUserID = pRefresh->dwUserID;
    G_SendThreadInfo(iThreadID, MSC_MAIN_EVENT_PVP, MSC_SUB_PVP_INSERT_CHALLER_FINISH, (CT_CHAR*)&dwUserID, sizeof(dwUserID));
}





//战斗记录
CT_VOID CCDBPVPEvent::DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len)
{

	if(len != sizeof(MSC_PVP_Fighting_Info)+sizeof(MSC_DB_W_PVPFightingInfo)+PVPRECORD_INDEX)
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeFightingEnd :len != sizeof(MSC_PVP_Fighting_Info)");
		return;
	}

	CT_UCHAR ucIndex[PVPRECORD_INDEX]={0};
	MSC_PVP_Fighting_Info FightingInfo={0};
	memcpy(&FightingInfo,pBuf+sizeof(ucIndex),sizeof(MSC_PVP_Fighting_Info));
	memcpy(ucIndex,pBuf,sizeof(ucIndex)-1);



	for(CT_INT32 i=0;i<PVP_USER_COUNT;++i)
	{
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			if(FightingInfo.FightingData[i].units[j].type == 0)continue;

			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_PVPAddFightExample");

			m_dbConnector.AddParm("_index", (CT_CHAR*)ucIndex);
			//m_dbConnector.AddParm("_uid ", FightingInfo.FightingData[i].dwUserID);
			//m_dbConnector.AddParm("_NickName", FightingInfo.FightingData[i].units[j].name);
			//m_dbConnector.AddParm("_hpIncRate", FightingInfo.FightingData[i].units[j].mcs.fHPPersent);
			//m_dbConnector.AddParm("_dmgIncRate", FightingInfo.FightingData[i].units[j].mcs.fDmgIncPercent);
			//m_dbConnector.AddParm("_dmgDesRate", FightingInfo.FightingData[i].units[j].mcs.fDmgDecPercent);
			//m_dbConnector.AddParm("_Rand ", FightingInfo.iSeed);
			//m_dbConnector.AddParm("_Grade ", FightingInfo.FightingData[i].units[j].level);
			//m_dbConnector.AddParm("_Score ", 0);
			//m_dbConnector.AddParm("_Occupation ", FightingInfo.FightingData[i].units[j].type);
			//m_dbConnector.AddParm("_Hp ", FightingInfo.FightingData[i].units[j].mcs.iMaxHP);
			//m_dbConnector.AddParm("_Mp ", FightingInfo.FightingData[i].units[j].mcs.iMaxMP);
			//m_dbConnector.AddParm("_Strength ", FightingInfo.FightingData[i].units[j].mcs.iStrength);
			//m_dbConnector.AddParm("_Agility ", FightingInfo.FightingData[i].units[j].mcs.iAgility);
			//m_dbConnector.AddParm("_Interllect  ", FightingInfo.FightingData[i].units[j].mcs.iIntellect);
			//m_dbConnector.AddParm("_Spirit ", FightingInfo.FightingData[i].units[j].mcs.iSpirit);
			//m_dbConnector.AddParm("_HitRate ", FightingInfo.FightingData[i].units[j].mcs.fHitRate);
			//m_dbConnector.AddParm("_DodgeRate ", FightingInfo.FightingData[i].units[j].mcs.fDodgeRate);
			//m_dbConnector.AddParm("_CritRate ", FightingInfo.FightingData[i].units[j].mcs.fCritRate);
			//m_dbConnector.AddParm("_ResilientRate ", FightingInfo.FightingData[i].units[j].mcs.fResilient);
			//m_dbConnector.AddParm("_AttackAddRate ", FightingInfo.FightingData[i].units[j].mcs.fDmgIncPercent);
			//m_dbConnector.AddParm("_DamageReduceRate ", FightingInfo.FightingData[i].units[j].mcs.fDmgDecPercent);
			//m_dbConnector.AddParm("_Attack ", FightingInfo.FightingData[i].units[j].mcs.iAttack);
			//m_dbConnector.AddParm("_Defense ", FightingInfo.FightingData[i].units[j].mcs.iDefence);
			//m_dbConnector.AddParm("_Pos ", j);
			//m_dbConnector.AddParm("_Skill0 ", FightingInfo.FightingData[i].units[j].Skills[0].iID);
			//m_dbConnector.AddParm("_Skill1 ", FightingInfo.FightingData[i].units[j].Skills[1].iID);
			//m_dbConnector.AddParm("_Skill2  ", FightingInfo.FightingData[i].units[j].Skills[2].iID);
			//m_dbConnector.AddParm("_Skill3 ", FightingInfo.FightingData[i].units[j].Skills[3].iID);
			//m_dbConnector.AddParm("_hprecover  ", FightingInfo.FightingData[i].units[j].mcs.iHPRecover);
			//m_dbConnector.AddParm("_mprecover ", FightingInfo.FightingData[i].units[j].mcs.iMPRecover);
			G_SaveFightingRecord(m_dbConnector,FightingInfo.FightingData[i],j,FightingInfo.iSeed);
			m_dbConnector.AddParm("_fid", FightingInfo.iSceneID);

			if(m_dbConnector.Exec())
			{
				CCDBResult* pResult = m_dbConnector.GetResult();

				if (pResult->GetResult())
				{
				}
				pResult->Release();
			}else
			{
				G_DBError(m_dbConnector, m_dwThread);
				return;
			}
		}
	}

	//插入装备技能
	for(CT_INT32 i=0;i<PVP_USER_COUNT;++i)
	{
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			for(CT_INT32 k=0;k<SKILL_ENUIP_COUNT;++k)
			{
				if(FightingInfo.FightingData[i].units[j].type == 0 || 
					FightingInfo.FightingData[i].units[j].SkillEnuip.iSkillEquip[k] == 0)continue;

				m_dbConnector.Reset();
				m_dbConnector.SetPcName("SP_PVPAddFightSkill");

				m_dbConnector.AddParm("_index", (CT_CHAR*)ucIndex);
				m_dbConnector.AddParm("_uid",  FightingInfo.FightingData[i].dwUserID);
				m_dbConnector.AddParm("_skillid", FightingInfo.FightingData[i].units[j].SkillEnuip.iSkillEquip[k]);
				m_dbConnector.AddParm("_pos", j);

				if(m_dbConnector.Exec())
				{
					CCDBResult* pResult = m_dbConnector.GetResult();
					pResult->Release();
				}else
				{
					G_DBError(m_dbConnector, m_dwThread);
					return;
				}
			}
		}
	}

	//插入奖励
	MSC_DB_W_PVPFightingInfo PVPFightingInfo={0};
	memcpy(&PVPFightingInfo,pBuf+sizeof(MSC_PVP_Fighting_Info)+sizeof(ucIndex),sizeof(MSC_DB_W_PVPFightingInfo));

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPSaveFightAward");

	m_dbConnector.AddParm("_index", (CT_CHAR*)ucIndex);
	m_dbConnector.AddParm("_uid", FightingInfo.FightingData[0].dwUserID);
	m_dbConnector.AddParm("_money", PVPFightingInfo.PVPFightinginfo.iGold);
	m_dbConnector.AddParm("_rankxp", PVPFightingInfo.PVPFightinginfo.iPrestige);
	m_dbConnector.AddParm("_prop0", PVPFightingInfo.PVPFightinginfo.iGoods[0]);
	m_dbConnector.AddParm("_prop0Num", PVPFightingInfo.PVPFightinginfo.iCount[0]);
	m_dbConnector.AddParm("_prop1", PVPFightingInfo.PVPFightinginfo.iGoods[1]);
	m_dbConnector.AddParm("_prop1Num", PVPFightingInfo.PVPFightinginfo.iCount[1]);
	m_dbConnector.AddParm("_prop2", PVPFightingInfo.PVPFightinginfo.iGoods[2]);
	m_dbConnector.AddParm("_prop2Num", PVPFightingInfo.PVPFightinginfo.iCount[2]);
	m_dbConnector.AddParm("_prop3", PVPFightingInfo.PVPFightinginfo.iGoods[3]);
	m_dbConnector.AddParm("_prop3Num", PVPFightingInfo.PVPFightinginfo.iCount[3]);
	m_dbConnector.AddParm("_bVictory", PVPFightingInfo.PVPFightinginfo.iWiner);
	m_dbConnector.AddParm("_aimUID ", PVPFightingInfo.PVPFightinginfo.dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(FightingInfo.FightingData[0].dwUserID,"SP_PVPSaveFightAward","_index:%s _uid:%d _money:%d _rankxp:%d _prop0:%d _prop0Num:%d _prop1:%d _prop1Num:%d _prop2:%d _prop2Num:%d _prop3:%d _prop3Num:%d _bVictory:%d _aimUID:%d",\
			ucIndex,FightingInfo.FightingData[0].dwUserID,PVPFightingInfo.PVPFightinginfo.iGold,PVPFightingInfo.PVPFightinginfo.iPrestige,PVPFightingInfo.PVPFightinginfo.iGoods[0],PVPFightingInfo.PVPFightinginfo.iCount[0],PVPFightingInfo.PVPFightinginfo.iGoods[1],PVPFightingInfo.PVPFightinginfo.iCount[1],\
			PVPFightingInfo.PVPFightinginfo.iGoods[2],PVPFightingInfo.PVPFightinginfo.iCount[2],PVPFightingInfo.PVPFightinginfo.iGoods[3],PVPFightingInfo.PVPFightinginfo.iCount[3],(PVPFightingInfo.PVPFightinginfo.iWiner != WINER_LIFT)?CT_TRUE:CT_FALSE,PVPFightingInfo.PVPFightinginfo.dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	//记录pvp奖励，日志数据库
	MSC_LOGDB_PVPReward dbPVP = {0};
	memcpy(dbPVP.cIndex,ucIndex,PVPRECORD_INDEX);
	dbPVP.dwUserID = FightingInfo.FightingData[0].dwUserID;
	dbPVP.iGold = PVPFightingInfo.PVPFightinginfo.iGold;
	dbPVP.iPrestige = PVPFightingInfo.PVPFightinginfo.iPrestige;
	dbPVP.iGoods[0] = PVPFightingInfo.PVPFightinginfo.iGoods[0];
	dbPVP.iCount[0] = PVPFightingInfo.PVPFightinginfo.iCount[0];
	dbPVP.iGoods[1] = PVPFightingInfo.PVPFightinginfo.iGoods[1];
	dbPVP.iCount[1] = PVPFightingInfo.PVPFightinginfo.iCount[1];
	dbPVP.iGoods[2] = PVPFightingInfo.PVPFightinginfo.iGoods[2];
	dbPVP.iCount[2] = PVPFightingInfo.PVPFightinginfo.iCount[2];
	dbPVP.iGoods[3] = PVPFightingInfo.PVPFightinginfo.iGoods[3];
	dbPVP.iCount[3] = PVPFightingInfo.PVPFightinginfo.iCount[3];
	dbPVP.iGoods[4] = PVPFightingInfo.PVPFightinginfo.iGoods[4];
	dbPVP.iCount[4] = PVPFightingInfo.PVPFightinginfo.iCount[4];
	dbPVP.iResult = PVPFightingInfo.PVPFightinginfo.iWiner;
	dbPVP.dwAimID = PVPFightingInfo.PVPFightinginfo.dwUserID;

	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + dbPVP.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PVPREWARD,(CT_CHAR*)&dbPVP,sizeof(dbPVP));

	return;
}

//挑战状况
CT_VOID CCDBPVPEvent::DisposeChallengeInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_ChallengeInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPInfo :len != sizeof(MSC_DB_R_FightingInfo)");
		return;
	}

	MSC_DB_ChallengeInfo *pChallengeInfo=(MSC_DB_ChallengeInfo*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPUpdateChallengeData");

	m_dbConnector.AddParm("_uid", pChallengeInfo->dwUserID);
	m_dbConnector.AddParm("_Challenge0", pChallengeInfo->dwDstUserID[0]);
	m_dbConnector.AddParm("_Challenge1", pChallengeInfo->dwDstUserID[1]);
	m_dbConnector.AddParm("_Challenge2", pChallengeInfo->dwDstUserID[2]);
	m_dbConnector.AddParm("_Challenge3", pChallengeInfo->dwDstUserID[3]);
	m_dbConnector.AddParm("_finishChallenge0", pChallengeInfo->bWiner[0]);
	m_dbConnector.AddParm("_finishChallenge1", pChallengeInfo->bWiner[1]);
	m_dbConnector.AddParm("_finishChallenge2", pChallengeInfo->bWiner[2]);
	m_dbConnector.AddParm("_finishChallenge3", pChallengeInfo->bWiner[3]);
	m_dbConnector.AddParm("_bHigh", pChallengeInfo->iDifficulty);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pChallengeInfo->dwUserID,"SP_PVPUpdateChallengeData","_uid:%d _Challenge0:%d _Challenge1:%d _Challenge2:%d _Challenge3:%d _finishChallenge0:%d _finishChallenge1:%d _finishChallenge2:%d _finishChallenge3:%d _bHigh:%d",\
			pChallengeInfo->dwUserID,pChallengeInfo->dwDstUserID[0],pChallengeInfo->dwDstUserID[1],pChallengeInfo->dwDstUserID[2],pChallengeInfo->dwDstUserID[3],pChallengeInfo->bWiner[0],pChallengeInfo->bWiner[1],pChallengeInfo->bWiner[2],pChallengeInfo->bWiner[3],pChallengeInfo->iDifficulty);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	return;
}

//竞技场终结
CT_VOID CCDBPVPEvent::DisposePVPEnding(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_PVPEnding))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPEnding :len != sizeof(MSC_DB_PVPEnding)");
		return;
	}
	MSC_DB_PVPEnding *pEnding=(MSC_DB_PVPEnding*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ContWinEnd");

	m_dbConnector.AddParm("_uid", pEnding->dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//保存对手信息
CT_VOID CCDBPVPEvent::DisposePVPChallgerSave(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(MSC_DB_PVPChallgerSave))
	{
		return;
	}

	MSC_DB_PVPChallgerSave* pChallgerSave = (MSC_DB_PVPChallgerSave*)pBuf;
	//写入数据库
	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT/PVP_GROUP_COUNT;++i)
	{
		CT_INT32 iRet = 0;
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_PVPUpdateChallengeData");

		m_dbConnector.AddParm("_uid", pChallgerSave->dwUserID);
		m_dbConnector.AddParm("_Challenge0", pChallgerSave->pvpInfo.dwChallengUserID[0+i*PVP_GROUP_COUNT]);
		m_dbConnector.AddParm("_Challenge1", pChallgerSave->pvpInfo.dwChallengUserID[1+i*PVP_GROUP_COUNT]);
		m_dbConnector.AddParm("_Challenge2", pChallgerSave->pvpInfo.dwChallengUserID[2+i*PVP_GROUP_COUNT]);
		m_dbConnector.AddParm("_Challenge3", pChallgerSave->pvpInfo.dwChallengUserID[3+i*PVP_GROUP_COUNT]);
		m_dbConnector.AddParm("_finishChallenge0", STATUS_NULL);
		m_dbConnector.AddParm("_finishChallenge1", STATUS_NULL);
		m_dbConnector.AddParm("_finishChallenge2", STATUS_NULL);
		m_dbConnector.AddParm("_finishChallenge3", STATUS_NULL);
		m_dbConnector.AddParm("_bHigh", i+1);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			pResult->Release();
		}else
		{
			CCDBLogMes::getSingleton().WriteInfo(pChallgerSave->dwUserID,"SP_PVPUpdateChallengeData","_uid:%d _Challenge0:%d _Challenge1:%d _Challenge2:%d _Challenge3:%d _finishChallenge0:%d _finishChallenge1:%d _finishChallenge2:%d _finishChallenge3:%d _bHigh:%d ",\
				pChallgerSave->dwUserID,pChallgerSave->pvpInfo.dwChallengUserID[0+i*PVP_GROUP_COUNT],pChallgerSave->pvpInfo.dwChallengUserID[1+i*PVP_GROUP_COUNT],pChallgerSave->pvpInfo.dwChallengUserID[2+i*PVP_GROUP_COUNT],pChallgerSave->pvpInfo.dwChallengUserID[3+i*PVP_GROUP_COUNT],\
				CT_TRUE,CT_TRUE,CT_TRUE,CT_TRUE,i+1);
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}
}

