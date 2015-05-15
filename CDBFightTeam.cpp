#include "CDBFightTeam.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "../sdk/include/CDBLogMes.h"
#include "CFightTeamManager.h"

CCDBFightTeam::CCDBFightTeam(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_DELETE);
//	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_FIND);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_DELETE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_FIND);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_RECRUIT_UPDATE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_RECRUIT_DELETE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_EVENT_SAVE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_SCIENCE_UPGRADE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_SUMMON_DATA);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_SUMMON_ITEM);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_DOWN_GRADE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_DB_CS_INSPORE_UP);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_DB_CS_READ_INSPORE);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_CLEAR_TIMEBOSS);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_TIMEBOSS_INFO);
	RegistMessage(MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_TIMEBOSS_MUMBER);

	if (m_dwThread == GAME_THREAD_DATABASE)
	{
		RegistTimer(IDI_FIGHTTEAM_READ_INFO);
		SetTimer(IDI_FIGHTTEAM_READ_INFO,  TIME_FIGHTTEAM_READ_SPOT);
	}
}

CCDBFightTeam::~CCDBFightTeam(void)
{
}

//处理用户消息
CT_VOID CCDBFightTeam::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
									 CT_CHAR* pBuf,		CT_DWORD len,
									 CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_FIGHTTEAM_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_FIGHTTEAM_UPDATE:			
		{
			DisposeFightTeamUpdate(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_DELETE:
		{
			DisposeFightTeamDelete(pBuf,len);
			break;
		}
//	case MSC_SUB_DB_FIGHTTEAM_FIND:
//		{
//			DisposeFightTeamFind(pBuf,len);
//			break;
//		}
	case MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE:
		{
			DisposeFightTeamMemberUpdate(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAMMEMBER_DELETE:
		{
			DisposeFightTeamMemberDelete(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAMMEMBER_FIND:
		{
			DisposeFightTeamMemberFind(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_RECRUIT_UPDATE:
		{
			DisposeFightTeamRecruitUpdate(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_RECRUIT_DELETE:
		{
			DisposeFightTeamRecruitDelete(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_EVENT_SAVE:
		{
			DisposeFightTeamEventSave(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_SCIENCE_UPGRADE:
		{
			DisposeFightTeamScienceUpGrade(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_SUMMON_DATA:
		{
			DisposeFightTeamSummonInfo(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_SUMMON_ITEM:
		{
			DisposeFightTeamSummonItem(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FIGHTTEAM_DOWN_GRADE:
		{
			DisposeFightTeamDownGrade(pBuf,len);
			break;
		}
	case MSC_DB_CS_INSPORE_UP:
		{
			DisposeCSInsporeUp(pBuf,len);
			break;
		}
	case MSC_DB_CS_READ_INSPORE:
		{
			DisopseCSReadInspore(pBuf,len);
			break;
		}
	case MSC_SUB_CLEAR_TIMEBOSS:
		{
			DisposeClearTimeBoss(pBuf,len);
			break;
		}
	case MSC_SUB_TIMEBOSS_INFO:
		{
			DisposeTimeBossInfo(pBuf,len);
			break;
		}
	case MSC_SUB_TIMEBOSS_MUMBER:
		{
			DisposeTimeBossMumber(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBFightTeam::OnUserEvent: mainID:%d	subID:%d",mainID,subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBFightTeam::OnTime(CT_DWORD timeID)
{
	if (timeID == IDI_FIGHTTEAM_READ_INFO)
	{
		GetFightTeamDB();
		KillTimer(IDI_FIGHTTEAM_READ_INFO);
		return;
	}
}

//程序启动读取战队信息
CT_VOID CCDBFightTeam::GetFightTeamDB() //启动是执行
{
	MSC_DB_FightTeamBase dbFightTeamBase = {0};

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamGet");	
	//select FT_ID, FightTeamName, Description, BossUID, BossName, FightingPower, FT_Top, FT_PreTop, Resource, FT_Icon, Elite1, Elite2, Elite3 from fightteam ;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbFightTeamBase.dwFTID);
				pResult->GetValue(1, dbFightTeamBase.cFightTeamName, NAME_SIZE);
				pResult->GetValue(2, dbFightTeamBase.cDescription, DESCRIPTION_SIZE);
				pResult->GetValue(3, dbFightTeamBase.dwBossUID);
				pResult->GetValue(4, dbFightTeamBase.cBossName, NAME_SIZE);
				pResult->GetValue(5, dbFightTeamBase.iFightingPower);
				pResult->GetValue(6, dbFightTeamBase.iTop);
				pResult->GetValue(7, dbFightTeamBase.iPreTop);
				pResult->GetValue(8, dbFightTeamBase.iResource);
				pResult->GetValue(9, dbFightTeamBase.iIcon);
				pResult->GetValue(10, dbFightTeamBase.dwElite1);
				pResult->GetValue(11, dbFightTeamBase.dwElite2);
				pResult->GetValue(12, dbFightTeamBase.dwElite3);
				pResult->GetValue(13, dbFightTeamBase.iResCount);
				pResult->GetValue(14, dbFightTeamBase.iGrade);
				pResult->GetValue(15, dbFightTeamBase.iItemCount);
				pResult->GetValue(16, dbFightTeamBase.iSummonCount);
				pResult->GetValue(17, dbFightTeamBase.iBossMaxIndex);
				pResult->GetValue(18, dbFightTeamBase.iTimeBossMaxIndex);
				CCFightTeamManager::getSingleton().InitFightTeam(dbFightTeamBase);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_FightTeamGet", "--");
		return;
	}


	MSC_DB_FightTeamMember dbFightTeamMember = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamMemberGet");	
	//select FT_ID, M_UID, M_NickName, M_FightingPower, M_Occupation, M_Grade, M_Rank, M_Contribution, M_Status, M_Type from fightteammember;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbFightTeamMember.dwFTID);
				pResult->GetValue(1, dbFightTeamMember.dwUserID);
				pResult->GetValue(2, dbFightTeamMember.cNickName, NAME_SIZE);
				pResult->GetValue(3, dbFightTeamMember.iFightingPower);
				pResult->GetValue(4, dbFightTeamMember.iOccupation);
				pResult->GetValue(5, dbFightTeamMember.iGrade);
				pResult->GetValue(6, dbFightTeamMember.iRank);
				pResult->GetValue(7, dbFightTeamMember.iContribution);
				pResult->GetValue(8, dbFightTeamMember.iStatus);
				pResult->GetValue(9, dbFightTeamMember.iTime);
				pResult->GetValue(10, dbFightTeamMember.iTowerCPID);
				pResult->GetValue(11, dbFightTeamMember.iType);
				pResult->GetValue(12, dbFightTeamMember.iContributeCount[0]);
				pResult->GetValue(13, dbFightTeamMember.iContributeCount[1]);
				pResult->GetValue(14, dbFightTeamMember.iContributeCount[2]);
				pResult->GetValue(15, dbFightTeamMember.iContributeGross);
				pResult->GetValue(16, dbFightTeamMember.iLastExitTime);
				pResult->GetValue(17, dbFightTeamMember.iTitle);
				CCFightTeamManager::getSingleton().InitFightTeamMember(dbFightTeamMember);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_FightTeamMemberGet", "--");
	}

	MSC_DB_FightTeamRecruit dbFightTeamRecruit = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamRecruitGet");	
	//select FT_ID, BeginTime, LastingTime, FT_Recruit from  fightteamrecruit;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbFightTeamRecruit.dwFTID);
				pResult->GetValue(1, dbFightTeamRecruit.iBeginTime);
				pResult->GetValue(2, dbFightTeamRecruit.iLastingTime);
				pResult->GetValue(3, dbFightTeamRecruit.cRecruit, DESCRIPTION_SIZE);
				pResult->GetValue(4, dbFightTeamRecruit.iAuto);
				CCFightTeamManager::getSingleton().InsertFightTeamRecruit(dbFightTeamRecruit);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_FightTeamRecruitGet", "--");
	}

	MSC_DB_FightTeamEventLog dbFightTeamEventLog = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamEventGet");	
	//select FT_ID, E_Time, E_Report, E_Desc from fightteamevent;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbFightTeamEventLog.dwFTID);
				pResult->GetValue(1, dbFightTeamEventLog.lDate);
				pResult->GetValue(2, dbFightTeamEventLog.iReport);
				pResult->GetValue(3, dbFightTeamEventLog.cEventDesc, DESCRIPTION_SIZE);
				CCFightTeamManager::getSingleton().InitEventLog(dbFightTeamEventLog);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_FightTeamEventGet", "--");
	}

	//战队召唤
	MSC_DB_FightTeamSummonInfo dbFightTeamSummonInfo = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamSummonGet");	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCount=0;
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbFightTeamSummonInfo.dwUserID);
				pResult->GetValue(1, dbFightTeamSummonInfo.dwFTID);
				pResult->GetValue(2, dbFightTeamSummonInfo.iBossIndex);
				pResult->GetValue(3, dbFightTeamSummonInfo.Res.iResType0);
				pResult->GetValue(4, dbFightTeamSummonInfo.Res.iResType1);
				pResult->GetValue(5, dbFightTeamSummonInfo.Res.iResType2);
				pResult->GetValue(6, dbFightTeamSummonInfo.Res.iResType3);
				pResult->GetValue(7, dbFightTeamSummonInfo.Res.iResNum0);
				pResult->GetValue(8, dbFightTeamSummonInfo.Res.iResNum1);
				pResult->GetValue(9, dbFightTeamSummonInfo.Res.iResNum2);
				pResult->GetValue(10, dbFightTeamSummonInfo.Res.iResNum3);
				pResult->GetValue(11, dbFightTeamSummonInfo.iWin);
				CCFightTeamManager::getSingleton().InitFightTeamSummon(dbFightTeamSummonInfo);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_FightTeamSummonGet", "--");
	}

	//战队召唤道具
	MSC_DB_FightTeamSummInfoItem dbFightTeamSummonItem = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamSummonItemGet");	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCount=0;
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbFightTeamSummonItem.dwFTID);
				pResult->GetValue(1, dbFightTeamSummonItem.dwUserID);
				pResult->GetValue(2, dbFightTeamSummonItem.iItemID);
				pResult->GetValue(3, dbFightTeamSummonItem.iCount);
				pResult->GetValue(4, dbFightTeamSummonItem.iChestIndex);
				pResult->GetValue(5, dbFightTeamSummonItem.iType);
				
				CCFightTeamManager::getSingleton().InitFihgtTeamSummonItem(dbFightTeamSummonItem);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_FightTeamSummonItemGet", "--");
	}

	//威能
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerUserPowerRead");	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCount=0;
			while (pResult->MoveNextRow())
			{
				MSC_DB_INSPORE csi={0};
				pResult->GetValue(0, csi.dwUserID);
				pResult->GetValue(1, csi.iID);
				pResult->GetValue(2, csi.iGrade);
				pResult->GetValue(3, csi.iExp);
				CCFightTeamManager::getSingleton().InsertCSInspore(csi);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	//时限boss信息
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetFTTimeBoss");	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCount=0;
			while (pResult->MoveNextRow())
			{
				MSC_DB_FTTimeBossInfo bi={0};
				pResult->GetValue(0, bi.dwFTID);
				pResult->GetValue(1, bi.iIndex);
				pResult->GetValue(2, bi.iSummonBeginTime);
				pResult->GetValue(3, bi.iCurrentHP);
				pResult->GetValue(4, bi.iHelpUser[0]);
				pResult->GetValue(5, bi.iHelpUser[1]);
				pResult->GetValue(6, bi.iHelpUser[2]);
				pResult->GetValue(7, bi.iHelpUser[3]);
				pResult->GetValue(8, bi.iHelpUser[4]);
				pResult->GetValue(9, bi.iHelpUser[5]);
				CCFightTeamManager::getSingleton().InsertTimeBossInfo(bi);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	//时限boss成员
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetFTTimeBossMumber");	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCount=0;
			while (pResult->MoveNextRow())
			{
				MSC_DB_FTTimeBossMember bm={0};
				pResult->GetValue(0, bm.dwFTID);
				pResult->GetValue(1, bm.dwUserID);
				pResult->GetValue(2, bm.iHurt);
				pResult->GetValue(3, bm.iFightNum);
				pResult->GetValue(4, bm.iFTContribute);
				pResult->GetValue(5, bm.iFTFood);
				CCFightTeamManager::getSingleton().InsertTimeBossMember(bm);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	CCFightTeamManager::getSingleton().RefreshFightTeamTop();
};


CT_VOID CCDBFightTeam::DisposeFightTeamUpdate(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamBase))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamUpdate : len != sizeof(MSC_DB_FightTeamBase)");
		return;
	}

	MSC_DB_FightTeamBase *pFightTeamBase=(MSC_DB_FightTeamBase*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamUpdate");

	m_dbConnector.AddParm("_ftid", pFightTeamBase->dwFTID);
	m_dbConnector.AddParm("_fightteamname", pFightTeamBase->cFightTeamName);
	m_dbConnector.AddParm("_description", pFightTeamBase->cDescription);
	m_dbConnector.AddParm("_bossuid", pFightTeamBase->dwBossUID);
	m_dbConnector.AddParm("_bossname", pFightTeamBase->cBossName);
	m_dbConnector.AddParm("_fightingpower", pFightTeamBase->iFightingPower);
	m_dbConnector.AddParm("_fttop", pFightTeamBase->iTop);
	m_dbConnector.AddParm("_pretop", pFightTeamBase->iPreTop);
	m_dbConnector.AddParm("_resource", pFightTeamBase->iResource);
	m_dbConnector.AddParm("_icon", pFightTeamBase->iIcon);
	m_dbConnector.AddParm("_elite1", pFightTeamBase->dwElite1);
	m_dbConnector.AddParm("_elite2", pFightTeamBase->dwElite2);
	m_dbConnector.AddParm("_elite3", pFightTeamBase->dwElite3);
	m_dbConnector.AddParm("_rescount", pFightTeamBase->iResCount);
	m_dbConnector.AddParm("_grade", pFightTeamBase->iGrade);
	m_dbConnector.AddParm("Itemcount", pFightTeamBase->iItemCount);
	m_dbConnector.AddParm("summoncount", pFightTeamBase->iSummonCount);
	m_dbConnector.AddParm("bossmaxindex", pFightTeamBase->iBossMaxIndex);
	m_dbConnector.AddParm("Timebossmaxindex", pFightTeamBase->iTimeBossMaxIndex);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pFightTeamBase->dwFTID, "SP_FightTeamUpdate",
			"_ftid:%d, _fightteamname:%s, _description:%s, _bossuid:%d, _bossname:%s, _fightingpower:%d, _fttop:%d, _resource:%d, _elite1:%d, _elite2:%d, _elite3:%d", 
			pFightTeamBase->dwFTID, pFightTeamBase->cFightTeamName, pFightTeamBase->cDescription, pFightTeamBase->dwBossUID, 
			pFightTeamBase->cBossName, pFightTeamBase->iFightingPower, pFightTeamBase->iTop, pFightTeamBase->iResource,
			pFightTeamBase->dwElite1, pFightTeamBase->dwElite2, pFightTeamBase->dwElite3);
	}
	return;
}

CT_VOID CCDBFightTeam::DisposeFightTeamDelete(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CT_DWORD))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamDelete : len != sizeof(CT_DWORD)");
		return;
	}

	CT_DWORD dwFTID = 0;
	memcpy(&dwFTID, pBuf, sizeof(CT_DWORD));
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamDelete");

	m_dbConnector.AddParm("_ftid", dwFTID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(dwFTID, "SP_FightTeamDelete", "_ftid:%d", dwFTID);
		return;
	}
	return;
}

/*
CT_VOID CCDBFightTeam::DisposeFightTeamFind(CT_CHAR* pBuf,CT_DWORD len)
{
	{
		if(len != sizeof(CMD_G_FightTeam_ARG))
		{
			CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamFind : len != sizeof(CMD_G_FightTeam_ARG)");
			return;
		}

		CMD_G_FightTeam_ARG *pFightTeamARG=(CMD_G_FightTeam_ARG*)pBuf;

		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_GetFTIDByName");
		m_dbConnector.AddParm("_fightteamname", pFightTeamARG->cFightTeamName);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				if (pResult->MoveNextRow())
				{
					pResult->GetValue(0, pFightTeamARG->dwFTID);
				}
			}
			pResult->Release();
		}else
		{
			G_DBError(m_dbConnector, m_dwThread);
			CCDBLogMes::getSingleton().WriteInfo(0, "SP_GetFTIDByName", "_fightteamname:%s", pFightTeamARG->cFightTeamName );
		}
		CT_DWORD threadid = GAME_THREAD_CITY + pFightTeamARG->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_DB_FIGHTTEAM_FIND,pBuf,sizeof(CMD_G_Friend_ARG));					
	}
}
*/

CT_VOID CCDBFightTeam::DisposeFightTeamMemberUpdate(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamMember))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamMemberUpdate : len != sizeof(MSC_DB_FightTeamMember)");
		return;
	}

	MSC_DB_FightTeamMember *pFightTeamMember=(MSC_DB_FightTeamMember*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamMemberUpdate");

	m_dbConnector.AddParm("_ftid", pFightTeamMember->dwFTID);
	m_dbConnector.AddParm("_uid", pFightTeamMember->dwUserID);
	m_dbConnector.AddParm("_nickname", pFightTeamMember->cNickName);
	m_dbConnector.AddParm("_fightingpower", pFightTeamMember->iFightingPower);
	m_dbConnector.AddParm("_occupation", pFightTeamMember->iOccupation);
	m_dbConnector.AddParm("_grade", pFightTeamMember->iGrade);
	m_dbConnector.AddParm("_rank", pFightTeamMember->iRank);
	m_dbConnector.AddParm("_contribution", pFightTeamMember->iContribution);
	m_dbConnector.AddParm("_status", pFightTeamMember->iStatus);
	m_dbConnector.AddParm("_time", pFightTeamMember->iTime);
	m_dbConnector.AddParm("_towercpid", pFightTeamMember->iTowerCPID);
	m_dbConnector.AddParm("_contribute1", pFightTeamMember->iContributeCount[0]);
	m_dbConnector.AddParm("_contribute2", pFightTeamMember->iContributeCount[1]);
	m_dbConnector.AddParm("_contribute3", pFightTeamMember->iContributeCount[2]);
	m_dbConnector.AddParm("_contributeGross", pFightTeamMember->iContributeGross);
	m_dbConnector.AddParm("_type", pFightTeamMember->iType);


	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pFightTeamMember->dwFTID, "SP_FightTeamMemberUpdate",
			"_ftid:%d, _uid:%d, _nickname:%s, _fightingpower:%d, _occupation:%d, _grade:%d, _rank:%d, _contribution:%d, _type:%d", 
			pFightTeamMember->dwFTID, pFightTeamMember->dwUserID, pFightTeamMember->cNickName, pFightTeamMember->iFightingPower,
			pFightTeamMember->iOccupation, pFightTeamMember->iGrade, pFightTeamMember->iRank, pFightTeamMember->iContribution,pFightTeamMember->iType);
	}
	return;
}

CT_VOID CCDBFightTeam::DisposeFightTeamMemberDelete(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamMemberDelete))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamMemberDelete : len != sizeof(MSC_DB_FightTeamMemberDelete)");
		return;
	}

	MSC_DB_FightTeamMemberDelete *pFightTeamMemberDelete=(MSC_DB_FightTeamMemberDelete*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamMemberDelete");

	m_dbConnector.AddParm("_ftid", pFightTeamMemberDelete->dwFTID);
	m_dbConnector.AddParm("_uid", pFightTeamMemberDelete->dwUserID);
	m_dbConnector.AddParm("_type", pFightTeamMemberDelete->iType);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pFightTeamMemberDelete->dwUserID, "SP_FightTeamMemberDelete", "_ftid:%d,_uid:%d,_type:%d",
			pFightTeamMemberDelete->dwFTID,pFightTeamMemberDelete->dwUserID,pFightTeamMemberDelete->iType);
		return;
	}
	return;
}

CT_VOID CCDBFightTeam::DisposeFightTeamMemberFind(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamMember_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::DisposeFightTeamMemberFind : len != sizeof(CMD_G_FightTeamMember_ARG)");
		return;
	}

	CMD_G_FightTeamMember_ARG *pFightTeamMemberARG=(CMD_G_FightTeamMember_ARG*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetUIDByName");
	m_dbConnector.AddParm("_nickname", pFightTeamMemberARG->cNickName);

	CMD_G_FightTeamMember	fightTeamMember = {0};
	fightTeamMember.dwFTID = pFightTeamMemberARG->dwFTID;
	memcpy(fightTeamMember.cNickName, pFightTeamMemberARG->cNickName, NAME_SIZE);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			if (pResult->MoveNextRow())
			{
				pResult->GetValue(0, fightTeamMember.dwUserID);
				pResult->GetValue(1, fightTeamMember.iGrade);
				pResult->GetValue(2, fightTeamMember.iOccupation);
				pResult->GetValue(3, fightTeamMember.iRank);
				pResult->GetValue(4, fightTeamMember.iFightingPower);
//				pResult->GetValue(5, fightTeamMember.iTitleID);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_GetUIDByName", "cNickName:%s", pFightTeamMemberARG->cNickName );
	}
	CT_DWORD threadid = GAME_THREAD_CITY + pFightTeamMemberARG->dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_DB_FIGHTTEAMMEMBER_FIND,(CT_CHAR*)&fightTeamMember,sizeof(CMD_G_FightTeamMember));					

}


CT_VOID CCDBFightTeam::DisposeFightTeamRecruitUpdate(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamRecruit))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamRecruitUpdate : len != sizeof(MSC_DB_FightTeamRecruit)");
		return;
	}

	MSC_DB_FightTeamRecruit *pFightTeamRecruit=(MSC_DB_FightTeamRecruit*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamRecruitUpdate");

	m_dbConnector.AddParm("_ftid", pFightTeamRecruit->dwFTID);
	m_dbConnector.AddParm("_begintime", pFightTeamRecruit->iBeginTime);
	m_dbConnector.AddParm("_lastingtime", pFightTeamRecruit->iLastingTime);
	m_dbConnector.AddParm("_recruit", pFightTeamRecruit->cRecruit);
	m_dbConnector.AddParm("_auto", pFightTeamRecruit->iAuto);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pFightTeamRecruit->dwFTID, "SP_FightTeamRecruitUpdate",
			"_ftid:%d, _begintime:d, _lastingtime:%d, _recruit:%s", 
			pFightTeamRecruit->dwFTID, pFightTeamRecruit->iLastingTime, pFightTeamRecruit->cRecruit);
	}
	return;
}

CT_VOID CCDBFightTeam::DisposeFightTeamRecruitDelete(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CT_DWORD))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamRecruitDelete : len != sizeof(CT_DWORD)");
		return;
	}

	CT_DWORD dwFTID = 0;
	memcpy(&dwFTID, pBuf, sizeof(CT_DWORD));
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamRecruitDelete");

	m_dbConnector.AddParm("_ftid", dwFTID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(dwFTID, "SP_FightTeamRecruitDelete", "_ftid:%d", dwFTID);
		return;
	}
	return;
}

CT_VOID CCDBFightTeam::DisposeFightTeamEventSave(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamEventLog))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamEventSave: len != sizeof(MSC_DB_FightTeamEventLog)");
		return;
	}

	MSC_DB_FightTeamEventLog *pFightTeamEventLog=(MSC_DB_FightTeamEventLog*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamEventSave");

	m_dbConnector.AddParm("_ftid", pFightTeamEventLog->dwFTID);
	m_dbConnector.AddParm("_time", pFightTeamEventLog->lDate);
	m_dbConnector.AddParm("_report", pFightTeamEventLog->iReport);
	m_dbConnector.AddParm("_desc", pFightTeamEventLog->cEventDesc);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pFightTeamEventLog->dwFTID, "SP_FightTeamEventSave",
			"_ftid:%d, _time:d, _report:%d, _desc:%s", 
			pFightTeamEventLog->dwFTID, pFightTeamEventLog->iReport, pFightTeamEventLog->cEventDesc);
	}
	return;

}

//科技升级
CT_VOID CCDBFightTeam::DisposeFightTeamScienceUpGrade(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamScienceUpGrade))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamScienceUpGrade: len != sizeof(MSC_DB_FightTeamScienceUpGrade)");
		return;
	}

	MSC_DB_FightTeamScienceUpGrade *pFightTeamEventSug=(MSC_DB_FightTeamScienceUpGrade*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamTechnUpdate");

	m_dbConnector.AddParm("_userid", pFightTeamEventSug->dwUserID);
	m_dbConnector.AddParm("_science",pFightTeamEventSug->iScienceID);
	m_dbConnector.AddParm("_grade", pFightTeamEventSug->iGrade);
	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//召唤信息
CT_VOID CCDBFightTeam::DisposeFightTeamSummonInfo(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamSummonInfo))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonInfo: len != sizeof(MSC_DB_FightTeamSummonInfo)");
		return;
	}

	MSC_DB_FightTeamSummonInfo *pFightTeamSi=(MSC_DB_FightTeamSummonInfo*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamSummonInsert");

	m_dbConnector.AddParm("_userid", pFightTeamSi->dwUserID);
	m_dbConnector.AddParm("_science",pFightTeamSi->dwFTID);
	m_dbConnector.AddParm("_bossindex", pFightTeamSi->iBossIndex);
	m_dbConnector.AddParm("_resid0", pFightTeamSi->Res.iResType0);
	m_dbConnector.AddParm("_resid1", pFightTeamSi->Res.iResType1);
	m_dbConnector.AddParm("_resid2", pFightTeamSi->Res.iResType2);
	m_dbConnector.AddParm("_resid3", pFightTeamSi->Res.iResType3);
	m_dbConnector.AddParm("_residnum0", pFightTeamSi->Res.iResNum0);
	m_dbConnector.AddParm("_residnum1", pFightTeamSi->Res.iResNum1);
	m_dbConnector.AddParm("_residnum2", pFightTeamSi->Res.iResNum2);
	m_dbConnector.AddParm("_residnum3", pFightTeamSi->Res.iResNum3);
	m_dbConnector.AddParm("_residnum3", pFightTeamSi->iWin);


	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//召唤道具
CT_VOID CCDBFightTeam::DisposeFightTeamSummonItem(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FightTeamSummInfoItem))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamSummonItem: len != sizeof(MSC_DB_FightTeamSummInfoItem)");
		return;
	}

	MSC_DB_FightTeamSummInfoItem *pFightTeamSi=(MSC_DB_FightTeamSummInfoItem*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamSummonItemInsert");

	m_dbConnector.AddParm("_ftid", pFightTeamSi->dwFTID);
	m_dbConnector.AddParm("_type", pFightTeamSi->iType);
	m_dbConnector.AddParm("_userid", pFightTeamSi->dwUserID);
	m_dbConnector.AddParm("_itemid",pFightTeamSi->iItemID);
	m_dbConnector.AddParm("itemnum", pFightTeamSi->iCount);
	m_dbConnector.AddParm("_chestindex", pFightTeamSi->iChestIndex);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	return;
}

//战队科技惩罚
CT_VOID CCDBFightTeam::DisposeFightTeamDownGrade(CT_CHAR *pBuf,CT_DWORD len)
{
#if 0 //科技惩罚取消
	if(len != sizeof(MSC_DB_FightTeamScienceDownGrade))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeFightTeamDownGrade: len != sizeof(MSC_DB_FightTeamScienceDownGrade)");
		return;
	}

	MSC_DB_FightTeamScienceDownGrade *pFTSDG=(MSC_DB_FightTeamScienceDownGrade*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamTechnOffline");
	m_dbConnector.AddParm("_userid", pFTSDG->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
#endif
}

//威能升级
CT_VOID CCDBFightTeam::DisposeCSInsporeUp(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_INSPORE))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeCSInsporeUp: len != sizeof(MSC_DB_INSPORE)");
		return;
	}

	MSC_DB_INSPORE *pInspore=(MSC_DB_INSPORE*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerUserPowerUpdate");
	m_dbConnector.AddParm("_userid", pInspore->dwUserID);
	m_dbConnector.AddParm("_PowerID", pInspore->iID);
	m_dbConnector.AddParm(" _Class", pInspore->iGrade);
	m_dbConnector.AddParm("_Progress", pInspore->iExp);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//读取威能
CT_VOID CCDBFightTeam::DisopseCSReadInspore(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(CT_DWORD))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisopseCSReadInspore: iLen != sizeof(CT_DWORD)");
		return;
	}

	CT_DWORD dwUserID = 0;
	memcpy(&dwUserID, pBuf, iLen);

	//威能
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerUserPowerReadByUser");	
	m_dbConnector.AddParm("_ID", dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCount=0;
			while (pResult->MoveNextRow())
			{
				MSC_DB_INSPORE csi={0};
				pResult->GetValue(0, csi.dwUserID);
				pResult->GetValue(1, csi.iID);
				pResult->GetValue(2, csi.iGrade);
				pResult->GetValue(3, csi.iExp);
				CCFightTeamManager::getSingleton().InsertCSInspore(csi);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//清理时限boss
CT_VOID CCDBFightTeam::DisposeClearTimeBoss(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_FTTBClear))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeClearTimeBoss: iLen != sizeof(MSC_FTTBClear)");
		return;
	}

	MSC_DB_FTTBClear *pClear=(MSC_DB_FTTBClear*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ClearFightTeamTimeBoss");	
	m_dbConnector.AddParm("_ID", pClear->dwFTID);
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
	}

	return;
}

//时限boss信息
CT_VOID CCDBFightTeam::DisposeTimeBossInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_FTTimeBossInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeTimeBossInfo: iLen != sizeof(MSC_FTTimeBossInfo)");
		return;
	}

	MSC_DB_FTTimeBossInfo *pFttb=(MSC_DB_FTTimeBossInfo*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UpdataFTTimeBoss");	
	m_dbConnector.AddParm("_ID", pFttb->dwFTID);
	m_dbConnector.AddParm("_index", pFttb->iIndex);
	m_dbConnector.AddParm("_begintime", pFttb->iSummonBeginTime);
	m_dbConnector.AddParm("_hp", pFttb->iCurrentHP);
	m_dbConnector.AddParm("_HelpUser0", pFttb->iHelpUser[0]);
	m_dbConnector.AddParm("_HelpUser1", pFttb->iHelpUser[1]);
	m_dbConnector.AddParm("_HelpUser2", pFttb->iHelpUser[2]);
	m_dbConnector.AddParm("_HelpUser3", pFttb->iHelpUser[3]);
	m_dbConnector.AddParm("_HelpUser4", pFttb->iHelpUser[4]);
	m_dbConnector.AddParm("_HelpUser5", pFttb->iHelpUser[5]);
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
	}
	return;
}

//时限boss成员
CT_VOID CCDBFightTeam::DisposeTimeBossMumber(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_FTTimeBossMember))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFightTeam::DisposeTimeBossMumber: iLen != sizeof(MSC_FTTimeBossMember)");
		return;
	}

	MSC_DB_FTTimeBossMember *pTbm=(MSC_DB_FTTimeBossMember*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UpdataFTTimeBossMumber");	
	m_dbConnector.AddParm("_ID", pTbm->dwFTID);
	m_dbConnector.AddParm("_uid", pTbm->dwUserID);
	m_dbConnector.AddParm("_hurt", pTbm->iHurt);
	m_dbConnector.AddParm("_fightnum", pTbm->iFightNum);
	m_dbConnector.AddParm("_contribute", pTbm->iFTContribute);
	m_dbConnector.AddParm("_food", pTbm->iFTFood);
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
	}
	return;
}

