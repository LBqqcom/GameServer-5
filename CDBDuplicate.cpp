#include "CDBDuplicate.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "CFightingRecord.h"
#include "CDBLogMes.h"
#include "CGameConfig.h"

CCDBDuplicate::CCDBDuplicate(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
								CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_W_FIGHTING_INFO);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_R_DUPLICATE_SCENE_INFO);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_R_DUPLICATE_WORLD_SCENE);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_W_BUYFIGHTINGTIME);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_RAIDS_INFO);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_RAIDS_REWARD);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_TEAMDUPLICATE_SCENE);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_TEAMDUPLICATE_AUTOMATCH);

}

CCDBDuplicate::~CCDBDuplicate(void)
{
}
//处理用户消息
CT_VOID CCDBDuplicate::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
							CT_CHAR* pBuf,		CT_DWORD len,
							CT_LONGLONG reserve)
{
	if(MSC_MAIN_DUPLICATE_EVENT != mainID)
	{
		printf("%s%d",__FILE__,__LINE__);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_W_FIGHTING_INFO:
		{
			DisposeFightingInfo(pBuf,len);
			break;
		}
	case MSC_SUB_DB_R_DUPLICATE_WORLD_SCENE:
		{
			DisposeWorldScene(pBuf,len);
			break;
		}
	case MSC_SUB_DB_R_DUPLICATE_SCENE_INFO:
		{
			DisposeSceneInfo(pBuf,len);
			break;
		}
	case MSC_SUB_DB_W_BUYFIGHTINGTIME:
		{
			DisposeBuyFightingTime(pBuf,len);
			break;
		}
	case MSC_SUB_DB_RAIDS_INFO:
		{
			DisposeRaidsInfo(pBuf,len);
			break;
		}
	case MSC_SUB_DB_RAIDS_REWARD:
		{
			DisposeRaidsReward(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TEAMDUPLICATE_SCENE:
		{
			DisposeTeamDuplicateScene(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TEAMDUPLICATE_AUTOMATCH:
		{
			DisposeTeamDuplicateAutoMatch(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBDuplicate::OnUserEvent :subID=%i", subID);
			return;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBDuplicate::OnTime(CT_DWORD timeID)
{

}

//战斗信息
CT_VOID CCDBDuplicate::DisposeFightingInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightingInfo %s",__FILE__);
		return;
	}
	MSC_Fighting_Info *pFightingInfo=(MSC_Fighting_Info*)pBuf;

	CT_INT64 t=time(0);
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(pFightingInfo->FightingData.units[i].type == 0)continue;
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_FightUpdateExample");

		m_dbConnector.AddParm("_aid", pFightingInfo->iWorldSceneID);
		m_dbConnector.AddParm("_fid", pFightingInfo->iSceneID);
		m_dbConnector.AddParm("_fid", pFightingInfo->iDifficulty);
		//m_dbConnector.AddParm("_uid ", pFightingInfo->dwUserID);
		//m_dbConnector.AddParm("_NickName", pFightingInfo->FightingData.units[i].name);
		//m_dbConnector.AddParm("_hpIncRate", pFightingInfo->FightingData.units[i].mcs.fHPPersent);
		//m_dbConnector.AddParm("_dmgIncRate", pFightingInfo->FightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_dmgDesRate", pFightingInfo->FightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Rand ", pFightingInfo->iSeed);
		//m_dbConnector.AddParm("_Grade ", pFightingInfo->FightingData.units[i].level);
		//m_dbConnector.AddParm("_Score ", pFightingInfo->iSceneStar);
		//m_dbConnector.AddParm("_Occupation ", pFightingInfo->FightingData.units[i].type);
		//m_dbConnector.AddParm("_Hp ", pFightingInfo->FightingData.units[i].mcs.iMaxHP);
		//m_dbConnector.AddParm("_Mp ", pFightingInfo->FightingData.units[i].mcs.iMaxMP);
		//m_dbConnector.AddParm("_Strength ", pFightingInfo->FightingData.units[i].mcs.iStrength);
		//m_dbConnector.AddParm("_Agility ", pFightingInfo->FightingData.units[i].mcs.iAgility);
		//m_dbConnector.AddParm("_Interllect  ", pFightingInfo->FightingData.units[i].mcs.iIntellect);
		//m_dbConnector.AddParm("_Spirit ", pFightingInfo->FightingData.units[i].mcs.iSpirit);
		//m_dbConnector.AddParm("_HitRate ", pFightingInfo->FightingData.units[i].mcs.fHitRate);
		//m_dbConnector.AddParm("_DodgeRate ", pFightingInfo->FightingData.units[i].mcs.fDodgeRate);
		//m_dbConnector.AddParm("_CritRate ", pFightingInfo->FightingData.units[i].mcs.fCritRate);
		//m_dbConnector.AddParm("_ResilientRate ", pFightingInfo->FightingData.units[i].mcs.fResilient);
		//m_dbConnector.AddParm("_AttackAddRate ", pFightingInfo->FightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_DamageReduceRate ", pFightingInfo->FightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Attack ", pFightingInfo->FightingData.units[i].mcs.iAttack);
		//m_dbConnector.AddParm("_Defense ", pFightingInfo->FightingData.units[i].mcs.iDefence);
		//m_dbConnector.AddParm("_Pos ", pFightingInfo->FightingData.units[i].position);
		//m_dbConnector.AddParm("_Skill1 ", pFightingInfo->FightingData.units[i].Skills[0].iID);
		//m_dbConnector.AddParm("_Skill2  ", pFightingInfo->FightingData.units[i].Skills[1].iID);
		//m_dbConnector.AddParm("_Skill3 ", pFightingInfo->FightingData.units[i].Skills[2].iID);
		//m_dbConnector.AddParm("_Skill3 ", pFightingInfo->FightingData.units[i].Skills[3].iID);
		//m_dbConnector.AddParm("_hprecover  ", pFightingInfo->FightingData.units[i].mcs.iHPRecover);
		//m_dbConnector.AddParm("_mprecover ", pFightingInfo->FightingData.units[i].mcs.iMPRecover);
		G_SaveFightingRecord(m_dbConnector,pFightingInfo->FightingData,i,pFightingInfo->iSeed);
		m_dbConnector.AddParm("_time",t);

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

	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		for(CT_INT32 j=0;j<SKILL_ENUIP_COUNT;++j)
		{
			if(pFightingInfo->FightingData.units[i].SkillEnuip.iSkillEquip[j] == 0 && (j != 0 || i < 0))continue;
			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_FightUpdateSkill");

			m_dbConnector.AddParm("_aid", pFightingInfo->iWorldSceneID);
			m_dbConnector.AddParm("_fid", pFightingInfo->iSceneID);
			m_dbConnector.AddParm("_diffcult", pFightingInfo->iDifficulty);
			m_dbConnector.AddParm("_skillid", pFightingInfo->FightingData.units[i].SkillEnuip.iSkillEquip[j]);
			m_dbConnector.AddParm("_pos", i);
			m_dbConnector.AddParm("_flag", j);

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
	return;
}

//世界信息
CT_VOID CCDBDuplicate::DisposeWorldScene(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_WorldSceneInfo))
	{
		CCLogMeg::getSingleton().WriteError("DisposeWorldScene %s",__FILE__);
		return;
	}

	MSC_DB_WorldSceneInfo *pWorldSceneInfo=(MSC_DB_WorldSceneInfo*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorldAddArea");

	m_dbConnector.AddParm("_uid", pWorldSceneInfo->dwUserID);
	m_dbConnector.AddParm("_aid", pWorldSceneInfo->iSceneID);
	m_dbConnector.AddParm("_award", pWorldSceneInfo->iReward);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pWorldSceneInfo->dwUserID,"SP_WorldAddArea","_uid:%d _aid:%d _award:%d",pWorldSceneInfo->dwUserID,pWorldSceneInfo->iSceneID,pWorldSceneInfo->iReward);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return ;
}

//场景信息
CT_VOID CCDBDuplicate::DisposeSceneInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_SceneInfo))
	{
		CCLogMeg::getSingleton().WriteError("DisposeSceneInfo %s",__FILE__);
		return;
	}

	MSC_DB_SceneInfo *pSceneInfo=(MSC_DB_SceneInfo*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorldUpdateFight");

	m_dbConnector.AddParm("_uid", pSceneInfo->dwUserID);
	m_dbConnector.AddParm("_aid", pSceneInfo->iWorldSceneID);
	m_dbConnector.AddParm("_fid ", pSceneInfo->iSceneID);
	m_dbConnector.AddParm("_fightTimes ", pSceneInfo->iFightingNum);
	m_dbConnector.AddParm("_diffcult", pSceneInfo->iDifficulty);
	m_dbConnector.AddParm("_score ", pSceneInfo->iSceneStar);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pSceneInfo->dwUserID,"SP_WorldUpdateFight","_uid:%d _aid:%d _fid:%d _score:%d _fightTimes:%d Difficulty:%d",pSceneInfo->dwUserID,pSceneInfo->iWorldSceneID,\
						pSceneInfo->iSceneID,pSceneInfo->iSceneStar,pSceneInfo->iFightingNum,pSceneInfo->iDifficulty);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	return;
}

//购买战斗次数
CT_VOID CCDBDuplicate::DisposeBuyFightingTime(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_BuyFightingTime))
	{
		CCLogMeg::getSingleton().WriteError("CCDBDuplicate::DisposeBuyFightingTime len != sizeof(MSC_DB_BuyFightingTime)");
		return;
	}

	MSC_DB_BuyFightingTime *pBuyFightingTime=(MSC_DB_BuyFightingTime*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorldFightTimeUpdate");

	m_dbConnector.AddParm("_uid", pBuyFightingTime->dwUserID);
	m_dbConnector.AddParm("_aid", pBuyFightingTime->iBuyFightingTime);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pBuyFightingTime->dwUserID,"SP_WorldFightTimeUpdate","_uid:%d _BuyfightTimes:%d",pBuyFightingTime->dwUserID,pBuyFightingTime->iBuyFightingTime);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//扫荡信息
CT_VOID CCDBDuplicate::DisposeRaidsInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_RaidsInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCDBDuplicate::DisposeRaidsInfo iLen != sizeof(MSC_DB_RaidsInfo)");
		return;
	}

	MSC_DB_RaidsInfo *pRaidsInfo=(MSC_DB_RaidsInfo*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_RaidsInfoSave");

	m_dbConnector.AddParm("_uid", pRaidsInfo->dwUserID);
	m_dbConnector.AddParm("_worldid", pRaidsInfo->iWorldID);
	m_dbConnector.AddParm("_sceneid", pRaidsInfo->iSceneID);
	m_dbConnector.AddParm("_difficulty", pRaidsInfo->iDifficulty);
	m_dbConnector.AddParm("_alltime", pRaidsInfo->iAllTime);
	m_dbConnector.AddParm("_currenttime", pRaidsInfo->iCurrentTime);
	m_dbConnector.AddParm("_begintime", pRaidsInfo->lBeginTime);
	m_dbConnector.AddParm("_laststamina", pRaidsInfo->iLastStamina);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{ 
		CCDBLogMes::getSingleton().WriteInfo(pRaidsInfo->dwUserID,"SP_RaidsInfoSave","_uid:%d _worldid:%d _sceneid:%d _difficulty:%d _alltime:%d _currenttime:%d _begintime:%d _laststamina:%d"
			,pRaidsInfo->dwUserID,pRaidsInfo->iWorldID,pRaidsInfo->iSceneID,pRaidsInfo->iDifficulty,pRaidsInfo->iAllTime,pRaidsInfo->iCurrentTime,pRaidsInfo->lBeginTime,pRaidsInfo->iLastStamina);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	return;
}

//扫荡奖励
CT_VOID CCDBDuplicate::DisposeRaidsReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_RaidsReward))
	{
		CCLogMeg::getSingleton().WriteError("CCDBDuplicate::DisposeRaidsReward iLen != sizeof(MSC_DB_RaidsReward)");
		return;
	}

	MSC_DB_RaidsReward *pRaidsReward=(MSC_DB_RaidsReward*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_RaidsRewardUpdate");

	m_dbConnector.AddParm("_uid", pRaidsReward->dwUserID);
	m_dbConnector.AddParm("_experience", pRaidsReward->iExperience);
	m_dbConnector.AddParm("_gold", pRaidsReward->iGold);
	m_dbConnector.AddParm("_diamond", pRaidsReward->iDiamond);
	m_dbConnector.AddParm("_potential", pRaidsReward->iPotential);
	m_dbConnector.AddParm("_skillpoint ", pRaidsReward->iSkillPoint);
	m_dbConnector.AddParm("_goods1", pRaidsReward->iGoods[0]);
	m_dbConnector.AddParm("_goods2", pRaidsReward->iGoods[1]);
	m_dbConnector.AddParm("_goods3", pRaidsReward->iGoods[2]);
	m_dbConnector.AddParm("_goods4", pRaidsReward->iGoods[3]);
	m_dbConnector.AddParm("_goodscount1", pRaidsReward->iGoodsCount[0]);
	m_dbConnector.AddParm("_goodscount2", pRaidsReward->iGoodsCount[1]);
	m_dbConnector.AddParm("_goodscount3", pRaidsReward->iGoodsCount[2]);
	m_dbConnector.AddParm("_goodscount4", pRaidsReward->iGoodsCount[3]);
	m_dbConnector.AddParm("_CurrentTime", pRaidsReward->lTime);
	m_dbConnector.AddParm("_CurrentTime", pRaidsReward->iCurrentTime);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{ 
		CCDBLogMes::getSingleton().WriteInfo(pRaidsReward->dwUserID,"SP_RaidsRewardUpdate","_uid:%d _experience:%d _gold:%d _diamond:%d _potential:%d \
																		 _skillpoint:%d _goods1:%d _goods2:%d _goods3:%d _goods4:%d _goodscount1:%d _goodscount2:%d _goodscount3:%d _goodscount4:%d"
			,pRaidsReward->dwUserID,pRaidsReward->iExperience,pRaidsReward->iGold,pRaidsReward->iDiamond,pRaidsReward->iPotential,
			pRaidsReward->iSkillPoint,pRaidsReward->iGoods[0],pRaidsReward->iGoods[1],pRaidsReward->iGoods[2],pRaidsReward->iGoods[3],pRaidsReward->iGoodsCount[0],pRaidsReward->iGoodsCount[1],pRaidsReward->iGoodsCount[2],pRaidsReward->iGoodsCount[3]);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	return;
}

//组队副本场景开启
CT_VOID CCDBDuplicate::DisposeTeamDuplicateScene(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_TeamDuplicateScene))
	{
		CCLogMeg::getSingleton().WriteError("CCDBDuplicate::DisposeTeamDuplicateScene iLen != sizeof(MSC_DB_TeamDuplicateScene)");
		return;
	}

	MSC_DB_TeamDuplicateScene *pScene=(MSC_DB_TeamDuplicateScene*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamAreaAdd");

	m_dbConnector.AddParm("_uid", pScene->dwUserID);
	m_dbConnector.AddParm("_S_ID", pScene->iScnenID);
	m_dbConnector.AddParm("_Difficulty", pScene->iDifficulty);
	m_dbConnector.AddParm("_Win", pScene->iWin);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{ 
		CCDBLogMes::getSingleton().WriteInfo(pScene->dwUserID,"SP_RaidsRewardUpdate","_uid:%d _S_ID:%d _gold:%d _Difficulty:%d _Win:%d"
																						   ,pScene->dwUserID,pScene->iScnenID,pScene->iDifficulty,pScene->iWin);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//组队副本自动匹配
CT_VOID CCDBDuplicate::DisposeTeamDuplicateAutoMatch(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_TeamDuplicateAutoMatch))
	{
		CCLogMeg::getSingleton().WriteError("CCDBDuplicate::DisposeTeamDuplicateAutoMatch iLen != sizeof(MSC_DB_TeamDuplicateAutoMatch)");
		return;
	}

	MSC_DB_TeamDuplicateAutoMatch *pAutoMatch=(MSC_DB_TeamDuplicateAutoMatch*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamGetPlayers");

	m_dbConnector.AddParm("_uid", pAutoMatch->dwUserID);
	m_dbConnector.AddParm("_fightingpower", pAutoMatch->iFightingPower);
	m_dbConnector.AddParm("_grade", pAutoMatch->iGrade);

	if(m_dbConnector.Exec())
	{
		CT_INT32 iCount=0;
		CT_DWORD dwUserID[TEAMDUPLICATE_PLAYERCOUNT]={0};
		MSC_DB_TeamDuplicateAutoMatchInfo AutoMatchInfo={0};
		AutoMatchInfo.dwUserID=pAutoMatch->dwUserID;
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dwUserID[(iCount++)%TEAMDUPLICATE_PLAYERCOUNT]);					
			}

			CT_INT32 iRand=0;
			if(iCount != 0)
			{
				iRand=rand()%TEAMDUPLICATE_PLAYERCOUNT;
				AutoMatchInfo.dwDstUserID[0]=dwUserID[iRand];
				dwUserID[iRand]=0;
				iRand=rand()%TEAMDUPLICATE_PLAYERCOUNT;
				AutoMatchInfo.dwDstUserID[1]=dwUserID[iRand];
				dwUserID[iRand]=0;

				for(CT_INT32 i=0;i<TEAM_NUM-1;++i)
				{
					for(CT_INT32 j=0;j<TEAMDUPLICATE_PLAYERCOUNT;++j)
					{
						if(AutoMatchInfo.dwDstUserID[i] == 0)
						{
							if(dwUserID[j] != 0)
							{
								AutoMatchInfo.dwDstUserID[i]=dwUserID[j];
								dwUserID[j]=0;
							}
						}
						else if(AutoMatchInfo.dwDstUserID[i] == pAutoMatch->dwDstUserID[0] || AutoMatchInfo.dwDstUserID[i] == pAutoMatch->dwDstUserID[1] ||
							AutoMatchInfo.dwDstUserID[i] == pAutoMatch->dwDstUserID[2])
						{
							AutoMatchInfo.dwDstUserID[i]=0;
						}
					}
				}
			}


			CT_DWORD threadid = GAME_THREAD_DATABASE + AutoMatchInfo.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_DB_LOGIN,MSC_SUB_TEAMDUPLICATE_AUTOMATCH,(CT_CHAR*)&AutoMatchInfo,sizeof(AutoMatchInfo));
		}
		pResult->Release();
	}else
	{ 
		CCDBLogMes::getSingleton().WriteInfo(pAutoMatch->dwUserID,"SP_TeamAreaAdd","_uid:%d _fightingpower:%d"
			,pAutoMatch->dwUserID,pAutoMatch->iFightingPower);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}