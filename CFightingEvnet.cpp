#include "CFightingEvnet.h"
#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CUserManager.h"
#include "CFightingScriptSetting.h"
#include "CRand.h"
#include "CEventScriptSetting.h"
#include "CBattleArray.h"
#include "CCompress.h"
#include "CTeamDuplicate.h"
#include "math.h"

#define MAX_FIGHTINGSIZE		1024*1024
CCFightingEvnet1::CCFightingEvnet1(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MSC_MAIN_FIGHTING_EVENT, MSC_SUB_FIGHTING_EVENT);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_LOOK_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_EVENT_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUS_PVP_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_PVP_LOOK_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_RESREBBERY_TEAMPVP);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_RESREBBERY_TEAMPVE);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TEAMDUPLICATE_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_FIGHTTEAM_PVP);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_FIGHTTEAM_PVE);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TOWER_CP_PASS);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TOWER_CPPASS_LOOK);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TOWER_RES_PVP);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_FT_SUMMON_BOSS);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TOWER_PVP_LOOK);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_WORLDBOSS_FINGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FIGHT_RECORD);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FINAL_EIGHT);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TOP_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_ISLAND_FIGHT);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_PLUNDER_FIGHT);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_KIDNAP_FIGHT);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_RESCUE_FIGHT);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_SEEK_LOOK_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT,MSC_SUB_FT_TIME_BOSS);

	m_pFightingLogic=new CCFightingLogic;
	m_pFightingBuffer= new CT_CHAR[MAX_FIGHTINGSIZE];
}

CCFightingEvnet1::~CCFightingEvnet1(void)
{
	delete m_pFightingLogic;
	m_pFightingLogic=0;

	delete[] m_pFightingBuffer;
	m_pFightingBuffer=0;
}
//处理用户消息
CT_VOID CCFightingEvnet1::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
									 CT_CHAR* pBuf,		CT_DWORD len,
									 CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_FIGHTING_EVENT:
		{
			if(MSC_SUB_FIGHTING_EVENT == subID)						//进入场景
			{
				DisposeFightingPVE(pBuf,len,reserve);				
			}
			else if(MSC_SUB_LOOK_FIGHTING == subID)					//观看战斗
			{
				DisposeLookFighting(pBuf,len,reserve);
			}
			else if(MSC_SUB_EVENT_FIGHTING == subID)				//战斗事件
			{
				DisposeEventFighting(pBuf,len,reserve);
			}
			else if(MSC_SUS_PVP_FIGHTING == subID || MSC_SUB_TOP_FIGHTING == subID)					//PVP战斗
			{
				DisposeFightingPVP(pBuf,len,reserve);
			}
			else if(MSC_SUB_PVP_LOOK_FIGHTING ==subID)				//PVP观战
			{
				DisposePVPLookFighting(pBuf,len,reserve);
			}
			else if(MSC_SUB_RESREBBERY_TEAMPVP == subID)			//资源掠夺
			{
				DisposeResRobberyPVP(pBuf,len,MSC_SUB_RESREBBERY_TEAMPVP);
			}
			else if(MSC_SUB_RESREBBERY_TEAMPVE == subID)			//资源掠夺
			{
				DisposeResRobberyPVE(pBuf,len,MSC_SUB_RESREBBERY_TEAMPVE);
			}
			else if(MSC_SUB_FIGHTTEAM_PVE == subID)					//观看战斗
			{
				DisposeResRobberyPVE(pBuf,len,MSC_SUB_FIGHTTEAM_PVE);
			}	
			else if(MSC_SUB_FIGHTTEAM_PVP == subID)					//观看战斗
			{
				DisposeResRobberyPVP(pBuf,len,MSC_SUB_FIGHTTEAM_PVP);
			}
			else if(MSC_SUB_TEAMDUPLICATE_FIGHTING == subID)		//组队副本
			{
				DiposeTeamDuplicate(pBuf,len);
			}
			else if(MSC_SUB_TOWER_CP_PASS == subID)					//元素之塔
			{
				DisposeTowerCheckPoint(pBuf,len,MSC_SUB_TOWER_CP_PASS);
			}
			else if(MSC_SUB_TOWER_CPPASS_LOOK == subID)
			{
				DisposeTowerCheckPoint(pBuf,len,MSC_SUB_TOWER_CPPASS_LOOK);
			}
			else if(MSC_SUB_TOWER_RES_PVP == subID)
			{
				DisposeTowerPVPResRobbery(pBuf,len,MSC_SUB_TOWER_RES_PVP);
			}
			else if(MSC_SUB_TOWER_PVP_LOOK == subID)
			{
				DisposeTowerPVPResRobbery(pBuf,len,MSC_SUB_TOWER_PVP_LOOK);
			}
			else if(MSC_SUB_FT_SUMMON_BOSS == subID)
			{
				DiposeFTSummonBoss(pBuf,len);
			}
			else if(MSC_SUB_WORLDBOSS_FINGHTING == subID)	//世界boss
			{
				DiposeWorldBoss(pBuf,len);
			}
			else if(MSC_SUB_CS_FIGHTING == subID)
			{
				DisposeCSFighting(pBuf,len,subID);
			}
			else if(MSC_SUB_CS_FIGHT_RECORD == subID)
			{
				DisposeCSFighting(pBuf,len,subID);
			}
			else if(MSC_SUB_CS_FINAL_EIGHT == subID)
			{
				DisposeCSFinalEight(pBuf,len);
			}
			else if(MSC_SUB_ISLAND_FIGHT == subID)
			{
				DisposeIslandFight(pBuf,len);
			}
			else if(MSC_SUB_PLUNDER_FIGHT == subID)
			{
				DisopsePlunderFight(pBuf,len);
			}
			else if(MSC_SUB_KIDNAP_FIGHT == subID)
			{
				DisopseKidnapFight(pBuf,len);
			}
			else if(MSC_SUB_RESCUE_FIGHT == subID || MSC_SUB_SEEK_LOOK_FIGHTING == subID)
			{
				DisposeRescueFight(pBuf,len,subID);
			}
			else if(MSC_SUB_FT_TIME_BOSS)
			{
				DisposeFTTimeBoss(pBuf,len);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCFightingEvnet::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			return;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCFightingEvnet1::OnTime(CT_DWORD timeID)
{
	return;
}

//战斗消息
CT_BOOL CCFightingEvnet1::DisposeFightingPVE(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	//数据验证
	if(len != sizeof(MSC_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeFightingPVE:　len != sizeof(MSC_Fighting_Info)!");
		return CT_FALSE;
	}

	MSC_Fighting_Info *pFightingPVE=(MSC_Fighting_Info*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingPVE->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeFightingPVE %d No Find UserID",pFightingPVE->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	//读取脚本
	G_FightingScene FightingScene={0};
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(pFightingPVE->iWorldSceneID,pFightingPVE->iSceneID,pFightingPVE->iDifficulty,FightingScene) == false)
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingScriptSetting::getSingleton().GetFightingScene world:%d scene:%d difficulty:%d",pFightingPVE->iWorldSceneID,pFightingPVE->iSceneID,pFightingPVE->iDifficulty);
		return CT_FALSE;
	}

	//初始化逻辑
	m_pFightingLogic->Init();

	//加载战斗
	if(!LoadFighting(*pFightingPVE,FightingScene,pNetHandle))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
		return CT_FALSE;
	}

	//战斗结束
	if(EndFighting(pNetHandle,SUB_G_FIGHTING_INFO_PVE) == CT_FALSE)
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("fighting end Fail!");
		return CT_FALSE;
	}

	//结算奖励
	if(m_pFightingLogic->GetWiner() == WINER_LIFT)
	{
		//战报保存
		pFightingPVE->iSceneStar=GetStar(pFightingPVE->iWorldSceneID,pFightingPVE->iSceneID,pFightingPVE->iDifficulty,m_pFightingLogic->GetFightingScore());
		//FightingReward(pUser,pFightingPVE->iWorldSceneID,pFightingPVE->iSceneID,pFightingPVE->iDifficulty,pFightingPVE->iSceneStar);
		MSC_PVEFighting_Reward pveReward = {0};
		pveReward.dwUserID = pFightingPVE->dwUserID;
		pveReward.iWorldSceneID = pFightingPVE->iWorldSceneID;
		pveReward.iSceneID = pFightingPVE->iSceneID;
		pveReward.iDifficulty = pFightingPVE->iDifficulty;
		pveReward.iSceneStar = pFightingPVE->iSceneStar;
		CT_DWORD threadid = GAME_THREAD_CITY + pveReward.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_FIGHTING_REWARD,(CT_CHAR*)&pveReward,sizeof(pveReward));
		SaveFighting(*pFightingPVE);
	}
	else
	{
		//客户端奖励
		CMD_G_FightingReward Reward={0};
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));
	}

	return CT_TRUE;
}

//观看战斗
CT_BOOL	CCFightingEvnet1::DisposeLookFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	//数据验证
	if(len != sizeof(MSC_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeLookFighting:　len != sizeof(MSC_Fighting_Info)!");
		return CT_FALSE;
	}

	MSC_Fighting_Info *pLookFighting=(MSC_Fighting_Info*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pLookFighting->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeLookFighting %d No Find UserID",pLookFighting->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	//读取脚本
	G_FightingScene FightingScene={0};
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(pLookFighting->iWorldSceneID,pLookFighting->iSceneID,pLookFighting->iDifficulty,FightingScene) == false)
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingScriptSetting::getSingleton().GetFightingScene world:%d scene:%d difficulty:%d",pLookFighting->iWorldSceneID,pLookFighting->iSceneID,pLookFighting->iDifficulty);
		return CT_FALSE;
	}

	//初始化逻辑
	m_pFightingLogic->Init();

	//加载战斗
	if(!LoadFighting(*pLookFighting,FightingScene,pNetHandle))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
		return CT_FALSE;
	}

	//战斗结束
	if(EndFighting(pNetHandle,SUB_G_FIGHTING_INFO_PVE) == CT_FALSE)
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("fighting end Fail!");
		return CT_FALSE;
	}

	//客户端奖励
	CMD_G_FightingReward Reward={0};
	pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));

	return CT_TRUE;
}
//PVP
CT_BOOL CCFightingEvnet1::DisposeFightingPVP(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	//数据验证
	if(len != sizeof(MSC_PVP_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeFightingPVP:　len != sizeof(MSC_PVP_Fighting_Info)!");
		return CT_FALSE;
	}

	MSC_PVP_Fighting_Info *pFightingPVP=(MSC_PVP_Fighting_Info*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingPVP->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeFightingPVP %d No Find UserID",pFightingPVP->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	//初始化逻辑
	m_pFightingLogic->Init();

	//加载战斗
	if(!LoadPVPFighting(*pFightingPVP,pNetHandle,pUser->GetUserID()))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
		return CT_FALSE;
	}

	//战斗结束
	if(EndFighting(pNetHandle,SUB_G_FIGHTING_INFO_PVP) == CT_FALSE)
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("fighting end Fail!");
		return CT_FALSE;
	}

	////结算奖励
	//MSC_DB_W_PVPFightingInfo PVPFighting={0};
	//if(!PVPFightingReward(pUser,m_pFightingLogic->GetWiner(),*pFightingPVP,PVPFighting))
	//{
	//	pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
	//	CCLogMeg::getSingleton().WriteError("结算奖励!");
	//	return CT_FALSE;
	//}

	//战报保存
	pFightingPVP->iWiner=m_pFightingLogic->GetWiner();
	//SavePVPFighting(*pFightingPVP,PVPFighting);
	SavePVPFighting(*pFightingPVP);

	return CT_TRUE;
}

//PVP观战
CT_BOOL CCFightingEvnet1::DisposePVPLookFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	//数据验证
	if(len != sizeof(MSC_PVP_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeFightingPVP:　len != sizeof(MSC_PVP_Fighting_Info)!");
		return CT_FALSE;
	}

	MSC_PVP_Fighting_Info *pFightingPVP=(MSC_PVP_Fighting_Info*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingPVP->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposePVPLookFighting %d No Find UserID",pFightingPVP->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	//初始化逻辑
	m_pFightingLogic->Init();

	//加载战斗
	if(pUser->GetUserID() == pFightingPVP->FightingData[0].dwUserID)
		m_pFightingLogic->SetOrder(CT_TRUE);
	else
		m_pFightingLogic->SetOrder(CT_FALSE);
	if(!LoadPVPFighting(*pFightingPVP,pNetHandle,pFightingPVP->FightingData[0].dwUserID))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
		return CT_FALSE;
	}

	//战斗结束
	if(EndFighting(pNetHandle,SUB_G_FIGHTING_INFO_PVPLOOK) == CT_FALSE)
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("fighting end Fail!");
		return CT_FALSE;
	}

	//发送奖励
	CMD_G_PVPFightingReward Reward={0};
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_PVP_FIGHTING_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));

	return CT_TRUE;
}

//资源掠夺
CT_BOOL CCFightingEvnet1::DisposeResRobberyPVP(CT_CHAR* pBuf,CT_DWORD len,CT_DWORD dwSubID)
{
	//数据验证
	if(len != sizeof(MSC_TeamPVP_ResRobbery))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeResRobberyPVP:　len != sizeof(MSC_TeamPVP_ResRobbery)!");
		return CT_FALSE;
	}

	MSC_TeamPVP_ResRobbery *pResRobbery=(MSC_TeamPVP_ResRobbery*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pResRobbery->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeResRobberyPVP %d No Find UserID",pResRobbery->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();
	FightingTeamPVP TeamPVP={0};

	//if(pResRobbery->bLookLeft)
	if(CT_TRUE)
	{
		GetFightingTeam(pResRobbery->LeftFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(pResRobbery->RightFightingData,TeamPVP.RivalInfo);
	}
	else
	{
		GetFightingTeam(pResRobbery->RightFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(pResRobbery->LeftFightingData,TeamPVP.RivalInfo);
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pResRobbery->iSeed);
	m_pFightingLogic->SetOrder(pResRobbery->bLookLeft);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LOAD_OVER;
		CT_BYTE cbLoadStatusRight=LOAD_OVER;
		if(m_pFightingLogic->GetOrder())
		{
			cbLoadStatusLeft=LoadTeamFighting(pResRobbery->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:bWin),pResRobbery->LeftFightingData[iLeftNum].dwUserID,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(pResRobbery->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iRightNum);
		}
		else
		{
			cbLoadStatusLeft=LoadTeamFighting(pResRobbery->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(pResRobbery->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),pResRobbery->RightFightingData[iRightNum].dwUserID,iRightNum);
		}
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVP.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVP.iLen[i]=TeamPVP.FSList[i].size();
		TeamPVP.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			if(m_pFightingLogic->GetOrder())
				iRightNum++;
			else
				iLeftNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			if(m_pFightingLogic->GetOrder())
				iLeftNum++;
			else
				iRightNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}
		printf("%d轮结束\n",i);

	}
	if(iWiner == WINER_NULL)
		pResRobbery->iWin=WINER_LIFT;
	else
		pResRobbery->iWin=iWiner;

	if(dwSubID == MSC_SUB_RESREBBERY_TEAMPVP)
	{
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_RESREBBERY_TEAMPVP,pBuf,len);

		//发送战报
		SendFighting(TeamPVP,pNetHandle,SUB_G_RESROBBERY_TEAMPVP);
	}
	else
	{
		//发送战报
		SendFighting(TeamPVP,pNetHandle,SUB_G_RESROBBERY_TEAMPVP);
	}


	//发送奖励
	//CMD_G_PVPFightingReward Reward={0};
	//pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_PVP_FIGHTING_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));
	return CT_TRUE;
}

//资源掠夺
CT_BOOL CCFightingEvnet1::DisposeResRobberyPVE(CT_CHAR* pBuf,CT_DWORD len,CT_DWORD dwSubID)
{
	//数据验证
	if(len != sizeof(MSC_TeamPVE_ResRobbery))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeResRobberyPVE:　len != sizeof(MSC_TeamPVE_ResRobbery)!");
		return CT_FALSE;
	}

	MSC_TeamPVE_ResRobbery *pResRobbery=(MSC_TeamPVE_ResRobbery*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pResRobbery->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeResRobberyPVE %d No Find UserID",pResRobbery->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	G_ResourceQuality RQ={0};
	if(!G_GetResourceQuality(pResRobbery->iResSpotQuality,&RQ))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeResRobberyPVE G_GetResourceQuality");
		return CT_FALSE;
	}

	CT_INT32 iMoster[TEAM_NUM]={RQ.iMonsterID1,RQ.iMonsterID2,RQ.iMonsterID3};
	FightingTeamPVE TeamPVE={0};
	GetFightingTeam(pResRobbery->FightingData,TeamPVE.MasterInfo);
	GetTeamMoster(iMoster,TeamPVE.MosterInfo,0);

	CT_INT32 iGrade=0;
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(pResRobbery->FightingData[i].dwUserID != 0)
		{
			if(pResRobbery->FightingData[i].dwUserID == pResRobbery->dwUserID)
			{
				CombatUnitData data={0};
				G_GetMasterFightingData(pResRobbery->FightingData[i],data);
				iGrade=data.level;
				break;
			}
		}
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pResRobbery->iSeed);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LoadTeamFighting(pResRobbery->FightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),pResRobbery->FightingData[iLeftNum].dwUserID,iLeftNum);
		CT_BYTE cbLoadStatusRight=LoadTeamMoster(iMoster,iRightNum,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iGrade);
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVE.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVE.iLen[i]=TeamPVE.FSList[i].size();
		TeamPVE.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			iRightNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			iLeftNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}

		printf("%d轮结束\n",i);

	}

	pResRobbery->iWin=iWiner;

	if(dwSubID == MSC_SUB_RESREBBERY_TEAMPVE)
	{
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_RESREBBERY_TEAMPVE,pBuf,len);

		//发送战报
		SendFighting(TeamPVE,pNetHandle,SUB_G_RESROBBERY_TEAMPVE);
	}
	else
	{
		//发送战报
		SendFighting(TeamPVE,pNetHandle,SUB_G_RESROBBERY_TEAMPVE);
	}

	return CT_FALSE;
}

//组队副本
CT_BOOL CCFightingEvnet1::DiposeTeamDuplicate(CT_CHAR* pBuf,CT_DWORD len)
{
	//数据验证
	if(len != sizeof(MSC_TeamDuplicate_Fighting))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DiposeTeamDuplicate:　len != sizeof(MSC_TeamDuplicate_Fighting)!");
		return CT_FALSE;
	}

	MSC_TeamDuplicate_Fighting *pTeamFighting=(MSC_TeamDuplicate_Fighting*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pTeamFighting->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DiposeTeamDuplicate %d No Find UserID",pTeamFighting->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	G_TeamDuplicate td={0};
	if(!G_GetTeamDuplicate(pTeamFighting->iSceneID,pTeamFighting->iDifficulty,&td))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DiposeTeamDuplicate G_GetTeamDuplicate");
		return CT_FALSE;
	}

	CT_INT32 iMoster[TEAM_NUM]={td.iMosterGroup1,td.iMosterGroup2,td.iMosterGroup3};
	FightingTeamPVE TeamPVE={0};
	GetFightingTeam(pTeamFighting->FightingData,TeamPVE.MasterInfo);
	GetTeamMoster(iMoster,TeamPVE.MosterInfo,pTeamFighting->iSceneID);

	CT_INT32 iGrade=0;
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(pTeamFighting->FightingData[i].dwUserID != 0)
		{
			if(pTeamFighting->FightingData[i].dwUserID == pTeamFighting->dwUserID)
			{
				CombatUnitData data={0};
				G_GetMasterFightingData(pTeamFighting->FightingData[i],data);
				iGrade=data.level;
				break;
			}
		}
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pTeamFighting->iSeed);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LoadTeamFighting(pTeamFighting->FightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),pTeamFighting->FightingData[iLeftNum].dwUserID,iLeftNum);
		CT_BYTE cbLoadStatusRight=LoadTeamMoster(iMoster,iRightNum,(iWiner == WINER_DRAW?CT_FALSE:!bWin),pTeamFighting->iSceneID,iGrade);
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVE.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVE.iLen[i]=TeamPVE.FSList[i].size();
		TeamPVE.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			iRightNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			iLeftNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}

		printf("%d轮结束\n",i);

	}

	TeamDuplicateRoomInfo RoomInfo={0};
	CCTeamDuplicate::getSingleton().GetRoomInfo(pTeamFighting->dwUserID,RoomInfo);

	//发送战报
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		CCUser *pDstUser=0;
		if(RoomInfo.FightingData[i].dwUserID == 0 ||
			RoomInfo.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE)continue;
		if(CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
		{
			SendFighting(TeamPVE,pDstUser->GetNetInterface(),SUB_G_TEAMDUPLICATE_FIGHTING);
		}
	}

	////组队副本奖励
	//if(WINER_LIFT == iWiner)
	//{
	//	for(CT_INT32 i=0;i<TEAM_NUM;i++)
	//	{
	//		CCUser *pDstUser=0;
	//		if(RoomInfo.FightingData[i].dwUserID == 0 ||
	//			RoomInfo.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE)continue;
	//		if(CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
	//		{
	//			TeamDuplicateReward(pDstUser,RoomInfo.iSceneID,RoomInfo.iDifficulty);
	//		}
	//	}
	//}

	//
	pTeamFighting->iWiner=iWiner;
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_TEAMDUPLICATE,MSC_SUB_TEAMDUPLICATE_FIGHTINGEND,pBuf,len);

	return CT_TRUE;
}

//元素之塔
CT_BOOL CCFightingEvnet1::DisposeTowerCheckPoint(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID)
{
	//数据验证
	if(len != sizeof(MSC_TowerCheckPoint_Fighting))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeTowerCheckPoint:　len != sizeof(MSC_TowerCheckPoint_Fighting)!");
		return CT_FALSE;
	}

	MSC_TowerCheckPoint_Fighting *pTcp=(MSC_TowerCheckPoint_Fighting*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pTcp->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeTowerCheckPoint %d No Find UserID",pTcp->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();
	G_TowerCheckPoint tcp = {0};
	if(!G_GetTowerCheckPoint(pTcp->iCPID, &tcp))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeTowerCheckPoint G_GetTowerCheckPoint");
		return CT_FALSE;
	}

	CT_INT32 iMoster[TEAM_NUM]={tcp.iMonsterGroupID,0,0};
	FightingTeamPVE TeamPVE={0};
	CombatUnitData data={0};
	G_GetMasterFightingData(pTcp->FightingData,data);
	GetFightingTeam(&pTcp->FightingData,TeamPVE.MasterInfo,1);
	GetTeamMoster(iMoster,TeamPVE.MosterInfo,0);

	CT_INT32 iGrade=data.level;

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pTcp->iSeed);

	//加载战斗
	CT_BYTE cbLoadStatusLeft=LoadTeamFighting(&pTcp->FightingData,CT_FALSE,pTcp->FightingData.dwUserID,0);
	CT_BYTE cbLoadStatusRight=LoadTeamMoster(iMoster,0,CT_FALSE,0,iGrade);
	if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
		return CT_FALSE;
	}

	//保存战报
	TeamPVE.FSList[0]=m_pFightingLogic->GetFightingLis();
	TeamPVE.iLen[0]=TeamPVE.FSList[0].size();
	TeamPVE.iWiner[0]=m_pFightingLogic->GetWiner();

	//初始化逻辑
	CT_INT32 iWiner=m_pFightingLogic->GetWiner();
	pTcp->iScore=GetStar(0,0,0,m_pFightingLogic->GetFightingScore());//采用副本评星
	m_pFightingLogic->Init(iWiner);

	if(dwSubID == MSC_SUB_TOWER_CP_PASS)
	{
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_TOWER_EVENT,MSC_SUB_TOWER_CP_PASS,pBuf,len);

		//发送战报
		SendFighting(TeamPVE,pNetHandle,SUB_G_TOWER_CP_PASS_PVE);
	}
	else
	{
		//发送战报
		SendFighting(TeamPVE,pNetHandle,SUB_G_TOWER_CP_PASS_PVE);
	}
	return CT_TRUE;
}

//
CT_BOOL CCFightingEvnet1::DisposeTowerPVPResRobbery(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID)
{
	//数据验证
	if(len != sizeof(MSC_TowerPVP_ResRobbery))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeTowerPVPResRobbery:　len != sizeof(MSC_TowerPVP_ResRobbery)!");
		return CT_FALSE;
	}

	MSC_TowerPVP_ResRobbery *pTpr=(MSC_TowerPVP_ResRobbery*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pTpr->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeTowerPVPResRobbery %d No Find UserID",pTpr->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();
	FightingTeamPVP TeamPVP={0};
	pTpr->bLookLeft=CT_TRUE;
	if(pTpr->bLookLeft)
	{
		GetFightingTeam(&pTpr->LeftFightingData,TeamPVP.MasterInfo,1);
		GetFightingTeam(&pTpr->RightFightingData,TeamPVP.RivalInfo,1);
	}
	else
	{
		GetFightingTeam(&pTpr->RightFightingData,TeamPVP.MasterInfo,1);
		GetFightingTeam(&pTpr->LeftFightingData,TeamPVP.RivalInfo,1);
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pTpr->iSeed);
	m_pFightingLogic->SetOrder(pTpr->bLookLeft);

	//加载战斗
	CT_BYTE cbLoadStatusLeft=LOAD_OVER;
	CT_BYTE cbLoadStatusRight=LOAD_OVER;
	if(pTpr->bLookLeft)
	{
		cbLoadStatusLeft=LoadTeamFighting(&pTpr->LeftFightingData, CT_FALSE,pTpr->LeftFightingData.dwUserID,0);
		cbLoadStatusRight=LoadTeamFighting(&pTpr->RightFightingData,CT_FALSE,0,0);
	}
	else
	{
		cbLoadStatusLeft=LoadTeamFighting(&pTpr->RightFightingData, CT_FALSE,pTpr->RightFightingData.dwUserID,0);
		cbLoadStatusRight=LoadTeamFighting(&pTpr->LeftFightingData,CT_FALSE,0,0);
	}

	if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
		return CT_FALSE;
	}

	//保存战报
	TeamPVP.FSList[0]=m_pFightingLogic->GetFightingLis();
	TeamPVP.iLen[0]=TeamPVP.FSList[0].size();
	TeamPVP.iWiner[0]=m_pFightingLogic->GetWiner();

	//初始化逻辑
	CT_INT32 iWiner=m_pFightingLogic->GetWiner();
	m_pFightingLogic->Init(iWiner);

	if(iWiner == WINER_NULL)
		pTpr->iWin=WINER_LIFT;
	else
		pTpr->iWin=iWiner;

	if(dwSubID == MSC_SUB_TOWER_RES_PVP)
	{
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_TOWER_EVENT, MSC_SUB_TOWER_RES_PVP,pBuf,len);

		//发送战报
		SendFighting(TeamPVP,pNetHandle,SUB_G_TOWER_CP_PASS_PVP);
	}
	else
		SendFighting(TeamPVP,pNetHandle,SUB_G_TOWER_CP_PASS_PVP_LOOK);
	return CT_TRUE;
}

//战队BOSS
CT_BOOL CCFightingEvnet1::DiposeFTSummonBoss(CT_CHAR *pBuf,CT_DWORD len)
{
	//数据验证
	if(len != sizeof(MSC_FightTeamSummonFighting))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DiposeFTSummonBoss:　len != sizeof(MSC_FightTeamSummonFighting)!");
		return CT_FALSE;
	}

	MSC_FightTeamSummonFighting *pSF=(MSC_FightTeamSummonFighting*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pSF->dwUserID[0],&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DiposeFTSummonBoss %d No Find UserID",pSF->dwUserID[0]);
		return CT_FALSE;
	}


	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	G_FightTeamUpgrade FTU={0};
	if(!G_GetFightTeamUpgrade(pSF->iBossIndex,FIGHTTEAM_BOSS_SUMMON,&FTU))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DiposeFTSummonBoss G_GetFightTeamUpgrade boss:%d",pSF->iBossIndex);
		return CT_FALSE;
	}

	CT_INT32 iMoster[TEAM_NUM]={FTU.iOpenBoss,0,0};
	FightingTeamPVE TeamPVE={0};
	GetFightingTeam(pSF->FightingData,TeamPVE.MasterInfo);
	GetTeamMoster(iMoster,TeamPVE.MosterInfo,0);

	CT_INT32 iGrade=0;
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(pSF->FightingData[i].dwUserID != 0)
		{
			if(pSF->FightingData[i].dwUserID == pSF->dwUserID[0])
			{
				CombatUnitData data={0};
				G_GetMasterFightingData(pSF->FightingData[i],data);
				iGrade=data.level;
				break;
			}
		}
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pSF->iSeed);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LoadTeamFighting(pSF->FightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),pSF->FightingData[iLeftNum].dwUserID,iLeftNum);
		CT_BYTE cbLoadStatusRight=LoadTeamMoster(iMoster,iRightNum,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iGrade);
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVE.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVE.iLen[i]=TeamPVE.FSList[i].size();
		TeamPVE.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
		}

		printf("%d轮结束\n",i);

	}

	pSF->iWin=iWiner;

	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FIGHTTEAM_SUMMON_FIGHTING,pBuf,len);

	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(pSF->dwUserID[i] == 0)continue;
		pUser=0;
		if(!CCUserManager::getSingleton().Find(pSF->dwUserID[i],&pUser))
		{
			CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DiposeFTSummonBoss %d No Find UserID",pSF->dwUserID[i]);
			return CT_FALSE;
		}
		//发送战报
		SendFighting(TeamPVE,pUser->GetNetInterface(),SUB_G_FT_SUMMON_BOSS);
	}

	return CT_TRUE;
}

//世界BOSS
CT_BOOL CCFightingEvnet1::DiposeWorldBoss(CT_CHAR *pBuf,CT_DWORD len)
{
	//数据验证
	if(len != sizeof(MSC_WorldBoss_Fighting))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DiposeWorldBoss:　len != sizeof(MSC_WorldBoss_Fighting)!");

		//CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_TOWER_EVENT, MSC_SUB_WORLDBOSS_FINGHTING,pBuf,len);
		return CT_FALSE;
	}

	MSC_WorldBoss_Fighting *pWb=(MSC_WorldBoss_Fighting*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pWb->dwUserID,&pUser))
	{
		G_SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_TOWER_EVENT, MSC_SUB_WORLDBOSS_FINGHTING,pBuf,len);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DiposeWorldBoss %d No Find UserID",pWb->dwUserID);
		return CT_FALSE;
	}

	FightingTeamPVE TeamPVE={0};
	CombatUnitData data={0};
	CT_INT32 iMoster[TEAM_NUM]={pWb->iMonsterGroupID,0,0};
	CCNetInterface* pNetHandle = pUser->GetNetInterface();
	G_GetMasterFightingData(pWb->LeftFightingData,data);
	GetFightingTeam(&pWb->LeftFightingData,TeamPVE.MasterInfo,1);
	GetTeamMoster(iMoster,TeamPVE.MosterInfo,0);

	CT_INT32 iGrade=data.level;

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pWb->iSeed);

	//加载战斗
	CT_BYTE cbLoadStatusLeft=LoadTeamFighting(&pWb->LeftFightingData, CT_FALSE,pWb->LeftFightingData.dwUserID,0);
	CT_BYTE cbLoadStatusRight=LoadTeamMoster(iMoster,0,CT_FALSE,0,iGrade);
	if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");

		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_TOWER_EVENT, MSC_SUB_WORLDBOSS_FINGHTING,pBuf,len);
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");

		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_TOWER_EVENT, MSC_SUB_WORLDBOSS_FINGHTING,pBuf,len);
		return CT_FALSE;
	}

	//保存战报
	TeamPVE.FSList[0]=m_pFightingLogic->GetFightingLis();
	TeamPVE.iLen[0]=TeamPVE.FSList[0].size();
	TeamPVE.iWiner[0]=m_pFightingLogic->GetWiner();

	//初始化逻辑
	CT_INT32 iWiner=m_pFightingLogic->GetWiner();
	m_pFightingLogic->Init(iWiner);

	//计算boss消耗生命
	UserFightingStatus ufs={0};
	m_pFightingLogic->GetBossInfo(ufs);
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		pWb->iHPHurt+=TeamPVE.MosterInfo[0].iMaxHP[i]-ufs.units[i].mcs.iCurrentHP;
	}

	if(iWiner == WINER_NULL)
		pWb->iWin=WINER_LIFT;
	else
		pWb->iWin=iWiner;

	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_WORLD_BOSS, MSC_SUB_WORLDBOSS_FINGHTING,pBuf,len);

	//发送战报
	SendFighting(TeamPVE,pNetHandle,SUB_G_WORLD_BOSS_FIGHTINGINFO);

	return CT_TRUE;
}

//跨服战斗
CT_BOOL CCFightingEvnet1::DisposeCSFighting(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID)
{
	//数据验证
	if(len != sizeof(MSC_CS_FIGHTING_DATA))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeCSFighting:　len != sizeof(MSC_CS_FIGHTING_DATA)!");
		return CT_FALSE;
	}

	MSC_CS_FIGHTING_DATA *pCSF=(MSC_CS_FIGHTING_DATA*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSF->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeCSFighting %d No Find UserID",pCSF->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();
	FightingTeamPVP TeamPVP={0};

	//if(pCSF->bLookLeft)
	if(CT_TRUE)
	{
		GetFightingTeam(pCSF->LeftFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(pCSF->RightFightingData,TeamPVP.RivalInfo);
	}
	else
	{
		GetFightingTeam(pCSF->RightFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(pCSF->LeftFightingData,TeamPVP.RivalInfo);
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pCSF->iSeed);
	m_pFightingLogic->SetOrder(pCSF->bLookLeft);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LOAD_OVER;
		CT_BYTE cbLoadStatusRight=LOAD_OVER;
		if(m_pFightingLogic->GetOrder())
		{
			cbLoadStatusLeft=LoadTeamFighting(pCSF->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:bWin),pCSF->LeftFightingData[iLeftNum].dwUserID,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(pCSF->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iRightNum);
		}
		else
		{
			cbLoadStatusLeft=LoadTeamFighting(pCSF->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(pCSF->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),pCSF->RightFightingData[iRightNum].dwUserID,iRightNum);
		}
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVP.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVP.iLen[i]=TeamPVP.FSList[i].size();
		TeamPVP.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			if(m_pFightingLogic->GetOrder())
				iRightNum++;
			else
				iLeftNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			if(m_pFightingLogic->GetOrder())
				iLeftNum++;
			else
				iRightNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}
		printf("%d轮结束\n",i);

	}
	if(iWiner == WINER_NULL)
		pCSF->iWiner=WINER_LIFT;
	else
		pCSF->iWiner=iWiner;

	if(dwSubID == MSC_SUB_CS_FIGHTING)
	{
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_FIGHTING,pBuf,len);

		//发送战报
		SendFighting(TeamPVP,pNetHandle,SUB_G_CS_FIGHTING);
	}
	else
	{
		//发送战报
		SendFighting(TeamPVP,pNetHandle,SUB_G_CS_FIGHTING);
	}
	return CT_TRUE;
}

//8强战斗
CT_BOOL CCFightingEvnet1::DisposeCSFinalEight(CT_CHAR *pBuf,CT_DWORD len)
{
	//数据验证
	if(len != sizeof(MSC_CS_FIGHTING_DATA))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeCSFighting:　len != sizeof(MSC_CS_FIGHTING_DATA)!");
		return CT_FALSE;
	}

	MSC_CS_FIGHTING_DATA *pCSF=(MSC_CS_FIGHTING_DATA*)pBuf;
	FightingTeamPVP TeamPVP={0};

	//if(pCSF->bLookLeft)
	if(CT_TRUE)
	{
		GetFightingTeam(pCSF->LeftFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(pCSF->RightFightingData,TeamPVP.RivalInfo);
	}
	else
	{
		GetFightingTeam(pCSF->RightFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(pCSF->LeftFightingData,TeamPVP.RivalInfo);
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pCSF->iSeed);
	m_pFightingLogic->SetOrder(pCSF->bLookLeft);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LOAD_OVER;
		CT_BYTE cbLoadStatusRight=LOAD_OVER;
		if(m_pFightingLogic->GetOrder())
		{
			cbLoadStatusLeft=LoadTeamFighting(pCSF->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:bWin),pCSF->LeftFightingData[iLeftNum].dwUserID,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(pCSF->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iRightNum);
		}
		else
		{
			cbLoadStatusLeft=LoadTeamFighting(pCSF->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(pCSF->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),pCSF->RightFightingData[iRightNum].dwUserID,iRightNum);
		}
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				for(CT_INT32 j=0;j<POST_COUNT/2;++j)
				{
					CCLogMeg::getSingleton().WriteError("lift:%d %f",pCSF->LeftFightingData[i].dwUserID,pCSF->LeftFightingData[i].units[j].mcs.fCritRate,
						pCSF->LeftFightingData[i].units[j].mcs.fDmgDecPercent,pCSF->LeftFightingData[i].units[j].mcs.fDmgIncPercent,pCSF->LeftFightingData[i].units[j].mcs.fDodgeRate,
						pCSF->LeftFightingData[i].units[j].mcs.fHitRate,pCSF->LeftFightingData[i].units[j].mcs.fHPPersent,pCSF->LeftFightingData[i].units[j].mcs.fResilient,
						pCSF->LeftFightingData[i].units[j].mcs.iAgility,pCSF->LeftFightingData[i].units[j].mcs.iAttack,pCSF->LeftFightingData[i].units[j].mcs.iCurrentHP,
						pCSF->LeftFightingData[i].units[j].mcs.iCurrentMP,pCSF->LeftFightingData[i].units[j].mcs.iDefence,pCSF->LeftFightingData[i].units[j].mcs.iHPRecover,
						pCSF->LeftFightingData[i].units[j].mcs.iIntellect,pCSF->LeftFightingData[i].units[j].mcs.iMaxHP,pCSF->LeftFightingData[i].units[j].mcs.iMaxMP,
						pCSF->LeftFightingData[i].units[j].mcs.iMPRecover,pCSF->LeftFightingData[i].units[j].mcs.iSpirit,pCSF->LeftFightingData[i].units[j].mcs.iStatus,
						pCSF->LeftFightingData[i].units[j].mcs.iStrength,pCSF->LeftFightingData[i].units[j].SkillEnuip.iSkillEquip[0],pCSF->LeftFightingData[i].units[j].SkillEnuip.iSkillEquip[1],
						pCSF->LeftFightingData[i].units[j].SkillEnuip.iSkillCount[0],pCSF->LeftFightingData[i].units[j].SkillEnuip.iSkillCount[1],pCSF->LeftFightingData[i].units[j].Skills[1].iID,
						pCSF->LeftFightingData[i].units[j].Skills[2].iID,pCSF->LeftFightingData[i].units[j].Skills[3].iID,pCSF->LeftFightingData[i].units[j].position,
						pCSF->LeftFightingData[i].units[j].type);
				}
			}
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVP.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVP.iLen[i]=TeamPVP.FSList[i].size();
		TeamPVP.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			if(m_pFightingLogic->GetOrder())
				iRightNum++;
			else
				iLeftNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			if(m_pFightingLogic->GetOrder())
				iLeftNum++;
			else
				iRightNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}
		printf("%d轮结束\n",i);

	}
	if(iWiner == WINER_NULL)
		pCSF->iWiner=WINER_LIFT;
	else
		pCSF->iWiner=iWiner;

	printf("fighting:%d\n",pCSF->iWiner);


	CT_DWORD threadid = GAME_THREAD_CITY;
	G_SendThreadInfo(threadid,MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_FINAL_EIGHT,pBuf,len);

	return CT_TRUE;
}

//事件战斗
CT_BOOL	CCFightingEvnet1::DisposeEventFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	//数据验证
	if(len != sizeof(MSC_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeFightingPVE:　len != sizeof(MSC_Fighting_Info)!");
		return CT_FALSE;
	}

	MSC_Fighting_Info *pFightingPVE=(MSC_Fighting_Info*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingPVE->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeEventFighting %d No Find UserID",pFightingPVE->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	//初始化逻辑
	m_pFightingLogic->Init();

	//加载战斗
	if(!LoadEventFihting(*pFightingPVE,pNetHandle))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
		return CT_FALSE;
	}

	//战斗结束
	if(EndFighting(pNetHandle,SUB_G_FIGHTING_INFO_EVENT) == CT_FALSE)
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("fighting end Fail!");
		return CT_FALSE;
	}

	//发送战斗结果
	MSC_Event_Fighting_End EventFighting={0};
	EventFighting.dwUserID=pFightingPVE->dwUserID;
	EventFighting.iIndex=pFightingPVE->iSceneID;
	EventFighting.iAwardID=pFightingPVE->iWorldSceneID;
	EventFighting.iWiner=m_pFightingLogic->GetWiner();

	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_EVENT_CITY,MSC_SUB_EVENT_FIGHTING_END,(CT_CHAR*)&EventFighting,sizeof(EventFighting));

	return CT_TRUE;
}

//龙胆战斗
CT_BOOL CCFightingEvnet1::DisposeIslandFight(CT_CHAR *pBuf,CT_DWORD len)
{
	if(sizeof(MSC_Island_FightEvent) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeIslandFight sizeof(MSC_Island_FightEvent) != len");
		return CT_FALSE;
	}

	MSC_Island_FightEvent *p=(MSC_Island_FightEvent*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(p->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeIslandFight %d No Find UserID",p->dwUserID);
		return CT_FALSE;
	}

	FightingTeamPVE TeamPVE={0};
	CombatUnitData data={0};
	CT_INT32 iMoster[TEAM_NUM]={p->iMonsterID,0,0};
	CCNetInterface* pNetHandle = pUser->GetNetInterface();
	G_GetMasterFightingData(p->FightingData,data);
	GetFightingTeam(&p->FightingData,TeamPVE.MasterInfo,1);
	GetTeamMoster(iMoster,TeamPVE.MosterInfo,0);

	CT_INT32 iGrade=data.level;

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(p->iSeed);

	//加载战斗
	CT_BYTE cbLoadStatusLeft=LoadTeamFighting(&p->FightingData, CT_FALSE,p->FightingData.dwUserID,0);
	CT_BYTE cbLoadStatusRight=LoadTeamMoster(iMoster,0,CT_FALSE,0,iGrade);
	if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("load fighting fail!");

		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_TOWER_EVENT, MSC_SUB_WORLDBOSS_FINGHTING,pBuf,len);
		return CT_FALSE;
	}

	//开始战斗
	if(!BeginFighting())
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");

		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_TOWER_EVENT, MSC_SUB_WORLDBOSS_FINGHTING,pBuf,len);
		return CT_FALSE;
	}

	//保存战报
	TeamPVE.FSList[0]=m_pFightingLogic->GetFightingLis();
	TeamPVE.iLen[0]=TeamPVE.FSList[0].size();
	TeamPVE.iWiner[0]=m_pFightingLogic->GetWiner();

	//初始化逻辑
	CT_INT32 iWiner=m_pFightingLogic->GetWiner();
	m_pFightingLogic->Init(iWiner);


	if(iWiner == WINER_NULL)
		p->iStatus=WINER_LIFT;
	else
		p->iStatus=iWiner;

	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
 	G_SendThreadInfo(threadid,MSC_MAIN_ISLAND_EVENT, MSC_SUB_ISLAND_FIGHT,pBuf,len);

	//发送战报
	SendFighting(TeamPVE,pNetHandle,SUB_G_ISLAND_FIGHTRECORD);
	return CT_TRUE;
}

//掠夺战斗
CT_BOOL CCFightingEvnet1::DisopsePlunderFight(CT_CHAR *pBuf,CT_DWORD len)
{
	if(sizeof(MSC_PLUNDER_FIGHT) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisopsePlunderFight sizeof(MSC_PLUNDER_FIGHT) != len");
		return CT_FALSE;
	}

	MSC_PLUNDER_FIGHT *p=(MSC_PLUNDER_FIGHT*)pBuf;
	FightingTeamPVP TeamPVP={0};

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(p->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisopsePlunderFight %d No Find UserID",p->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	//if(pCSF->bLookLeft)
	if(CT_TRUE)
	{
		GetFightingTeam(p->LeftFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(p->RightFightingData,TeamPVP.RivalInfo);
	}
	else
	{
		GetFightingTeam(p->RightFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(p->LeftFightingData,TeamPVP.RivalInfo);
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(p->iSeed);
	//m_pFightingLogic->SetOrder(p->bLookLeft);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LOAD_OVER;
		CT_BYTE cbLoadStatusRight=LOAD_OVER;
		if(m_pFightingLogic->GetOrder())
		{
			cbLoadStatusLeft=LoadTeamFighting(p->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:bWin),p->LeftFightingData[iLeftNum].dwUserID,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(p->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iRightNum);
		}
		else
		{
			cbLoadStatusLeft=LoadTeamFighting(p->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(p->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),p->RightFightingData[iRightNum].dwUserID,iRightNum);
		}
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVP.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVP.iLen[i]=TeamPVP.FSList[i].size();
		TeamPVP.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			if(m_pFightingLogic->GetOrder())
				iRightNum++;
			else
				iLeftNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			if(m_pFightingLogic->GetOrder())
				iLeftNum++;
			else
				iRightNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}
		printf("%d轮结束\n",i);

	}
	if(iWiner == WINER_NULL)
		p->iWin=WINER_LIFT;
	else
		p->iWin=iWiner;

	printf("fighting:%d\n",p->iWin);

	//发送战报
	SendFighting(TeamPVP,pNetHandle,SUB_G_SEEK_PLUNDER_FIGHTING);


	CT_DWORD threadid = GAME_THREAD_CITY;
	G_SendThreadInfo(threadid,MSC_MAIN_SEEK,MSC_SUB_PLUNDER_FIGHT,pBuf,len);
	return CT_TRUE;
}
//绑票战斗
CT_BOOL CCFightingEvnet1::DisopseKidnapFight(CT_CHAR *pBuf,CT_DWORD len)
{
	if(sizeof(MSC_KIDNAP_FIGHT) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisopseKidnapFight sizeof(MSC_KIDNAP_FIGHT) != len");
		return CT_FALSE;
	}

	MSC_KIDNAP_FIGHT *p=(MSC_KIDNAP_FIGHT*)pBuf;
	FightingTeamPVP TeamPVP={0};

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(p->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisopseKidnapFight %d No Find UserID",p->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();
	//if(pCSF->bLookLeft)
	if(CT_TRUE)
	{
		GetFightingTeam(p->LeftFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(p->RightFightingData,TeamPVP.RivalInfo);
	}
	else
	{
		GetFightingTeam(p->RightFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(p->LeftFightingData,TeamPVP.RivalInfo);
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(p->iSeed);
	//m_pFightingLogic->SetOrder(p->bLookLeft);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LOAD_OVER;
		CT_BYTE cbLoadStatusRight=LOAD_OVER;
		if(m_pFightingLogic->GetOrder())
		{
			cbLoadStatusLeft=LoadTeamFighting(p->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:bWin),p->LeftFightingData[iLeftNum].dwUserID,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(p->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iRightNum);
		}
		else
		{
			cbLoadStatusLeft=LoadTeamFighting(p->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(p->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),p->RightFightingData[iRightNum].dwUserID,iRightNum);
		}
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVP.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVP.iLen[i]=TeamPVP.FSList[i].size();
		TeamPVP.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			if(m_pFightingLogic->GetOrder())
				iRightNum++;
			else
				iLeftNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			if(m_pFightingLogic->GetOrder())
				iLeftNum++;
			else
				iRightNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}
		printf("%d轮结束\n",i);

	}
	if(iWiner == WINER_NULL)
		p->iWin=WINER_LIFT;
	else
		p->iWin=iWiner;

	//发送战报
	SendFighting(TeamPVP,pNetHandle,SUB_G_SEEK_KINNAP_FIGHTING);

	CT_DWORD threadid = GAME_THREAD_CITY;
	G_SendThreadInfo(threadid,MSC_MAIN_SEEK,MSC_SUB_KIDNAP_FIGHT,pBuf,len);
	return CT_TRUE;
}

//营救战斗
CT_BOOL CCFightingEvnet1::DisposeRescueFight(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID)
{
	if(sizeof(MSC_RESCUE_FIGHT) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisopsePlunderFight sizeof(MSC_RESCUE_FIGHT) != len");
		return CT_FALSE;
	}

	MSC_RESCUE_FIGHT *p=(MSC_RESCUE_FIGHT*)pBuf;
	FightingTeamPVP TeamPVP={0};

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(p->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeRescueFight %d No Find UserID",p->dwUserID);
		return CT_FALSE;
	}

	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	//if(pCSF->bLookLeft)
	if(CT_TRUE)
	{
		GetFightingTeam(p->LeftFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(p->RightFightingData,TeamPVP.RivalInfo);
	}
	else
	{
		GetFightingTeam(p->RightFightingData,TeamPVP.MasterInfo);
		GetFightingTeam(p->LeftFightingData,TeamPVP.RivalInfo);
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(p->iSeed);
	//m_pFightingLogic->SetOrder(p->bLookLeft);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LOAD_OVER;
		CT_BYTE cbLoadStatusRight=LOAD_OVER;
		if(m_pFightingLogic->GetOrder())
		{
			cbLoadStatusLeft=LoadTeamFighting(p->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:bWin),p->LeftFightingData[iLeftNum].dwUserID,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(p->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iRightNum);
		}
		else
		{
			cbLoadStatusLeft=LoadTeamFighting(p->LeftFightingData, (iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iLeftNum);
			cbLoadStatusRight=LoadTeamFighting(p->RightFightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),p->RightFightingData[iRightNum].dwUserID,iRightNum);
		}
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVP.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVP.iLen[i]=TeamPVP.FSList[i].size();
		TeamPVP.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			if(m_pFightingLogic->GetOrder())
				iRightNum++;
			else
				iLeftNum++;
			m_pFightingLogic->SetOrder(CT_FALSE);
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			if(m_pFightingLogic->GetOrder())
				iLeftNum++;
			else
				iRightNum++;
			m_pFightingLogic->SetOrder(CT_TRUE);
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
			m_pFightingLogic->SetOrder(CT_TRUE);
		}
		printf("%d轮结束\n",i);

	}
	if(iWiner == WINER_NULL)
		p->iWin=WINER_LIFT;
	else
		p->iWin=iWiner;

	//发送战报
	SendFighting(TeamPVP,pNetHandle,SUB_G_SEEK_RESCUE_FIGHTING);

	if(MSC_SUB_SEEK_LOOK_FIGHTING != dwSubID)
	{
		CT_DWORD threadid = GAME_THREAD_CITY;
		G_SendThreadInfo(threadid,MSC_MAIN_SEEK,MSC_SUB_RESCUE_FIGHT,pBuf,len);
	}
	return CT_TRUE;
}

//时限boss战斗
CT_BOOL CCFightingEvnet1::DisposeFTTimeBoss(CT_CHAR *pBuf,CT_DWORD len)
{
	//数据验证
	if(len != sizeof(MSC_FTTimeBossFight))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::DisposeFTTimeBoss:　len != sizeof(MSC_FTTimeBossFight)!");
		return CT_FALSE;
	}

	MSC_FTTimeBossFight *pBf=(MSC_FTTimeBossFight*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pBf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DisposeFTTimeBoss %d No Find UserID",pBf->dwUserID);
		return CT_FALSE;
	}


	CCNetInterface* pNetHandle = pUser->GetNetInterface();

	G_FightTeamUpgrade FTU={0};
	if(!G_GetFightTeamUpgrade(pBf->iBossIndex,FIGHTTEAM_BOSS_TIME,&FTU))
	{
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::DiposeFTSummonBoss G_GetFightTeamUpgrade boss:%d",pBf->iBossIndex);
		return CT_FALSE;
	}

	CT_INT32 iMoster[TEAM_NUM]={FTU.iOpenBoss,0,0};
	FightingTeamPVE TeamPVE={0};
	GetFightingTeam(pBf->FightingData,TeamPVE.MasterInfo);
	GetTeamMoster(iMoster,TeamPVE.MosterInfo,0);

	CT_INT32 iGrade=0;
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(pBf->FightingData[i].dwUserID != 0)
		{
			if(pBf->FightingData[i].dwUserID == pBf->dwUserID)
			{
				CombatUnitData data={0};
				G_GetMasterFightingData(pBf->FightingData[i],data);
				iGrade=data.level;
				break;
			}
		}
	}

	//初始化逻辑
	m_pFightingLogic->Init();
	m_pFightingLogic->SetSeed(pBf->iSeed);

	CT_INT32 iWiner=WINER_NULL;
	CT_INT32 iLeftNum=0;
	CT_INT32 iRightNum=0;
	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		//加载战斗
		CT_BOOL bWin=(iWiner == WINER_LIFT?CT_TRUE:CT_FALSE);
		if(iWiner == WINER_NULL)iWiner=WINER_DRAW;
		CT_BYTE cbLoadStatusLeft=LoadTeamFighting(pBf->FightingData,(iWiner == WINER_DRAW?CT_FALSE:bWin),pBf->FightingData[iLeftNum].dwUserID,iLeftNum);
		CT_BYTE cbLoadStatusRight=LoadTeamMoster(iMoster,iRightNum,(iWiner == WINER_DRAW?CT_FALSE:!bWin),0,iGrade);
		if(cbLoadStatusLeft == LOAD_FAIL || cbLoadStatusRight == LOAD_FAIL) 
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("load fighting fail!");
			return CT_FALSE;
		}
		else if(cbLoadStatusLeft == LOAD_OVER || cbLoadStatusRight == LOAD_OVER)
		{
			break;
		}

		//开始战斗
		if(!BeginFighting())
		{
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("Begin Fighting Fail!");
			return CT_FALSE;
		}

		//保存战报
		TeamPVE.FSList[i]=m_pFightingLogic->GetFightingLis();
		TeamPVE.iLen[i]=TeamPVE.FSList[i].size();
		TeamPVE.iWiner[i]=m_pFightingLogic->GetWiner();

		//初始化逻辑
		iWiner=m_pFightingLogic->GetWiner();
		m_pFightingLogic->Init(iWiner);

		if(iWiner == WINER_LIFT)
		{
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
		}
		else if(iWiner == WINER_RIGHT)
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
		}
		else
		{
			iLeftNum++;
			if(iLeftNum >= TEAM_NUM)break;
			iRightNum++;
			if(iRightNum >= TEAM_NUM)break;
		}

		printf("%d轮结束\n",i);

	}

	pBf->iWin=iWiner;

	//计算boss消耗生命
	UserFightingStatus ufs={0};
	m_pFightingLogic->GetBossInfo(ufs);
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		pBf->iHurt+=TeamPVE.MosterInfo[0].iMaxHP[i]-ufs.units[i].mcs.iCurrentHP;
	}

	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FT_TIME_BOSS,pBuf,len);

	//发送战报
	SendFighting(TeamPVE,pUser->GetNetInterface(),SUB_G_FT_TIME_BOSS);
	return CT_TRUE;
}

//加载战斗
CT_BOOL	CCFightingEvnet1::LoadFighting(const MSC_Fighting_Info &FightingInfo,const G_FightingScene &FightingScene,CCNetInterface* pNetHandle)
{
	//提取领主信息
	CCFightingLogic::FightingUserInfo FightingUser={0};
	m_pFightingLogic->GetFightingInfo(FightingInfo.FightingData,FightingUser);
	FightingUser.dwUserID=FightingInfo.FightingData.dwUserID;
	m_pFightingLogic->SetSeed(FightingInfo.iSeed);

	//发送领主信息
	CMD_G_MasterInfo MasterInfo={0};
	MasterInfo.iGrade=FightingUser.iGrade;
	MasterInfo.iSkillID[0]=FightingUser.iSkill2.iID;
	MasterInfo.iSkillID[1]=FightingUser.iSkill3.iID;
	MasterInfo.iSkillID[2]=FightingUser.iSkill4.iID;

	CT_INT32 iOccupation=FightingInfo.FightingData.units[FightingUser.iPosition].type;
	memcpy(MasterInfo.cMasterName,FightingInfo.FightingData.units[FightingUser.iPosition].name,sizeof(MasterInfo.cMasterName));
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		MasterInfo.iArmyID[i]=FightingInfo.FightingData.units[i].type;
	}
	
	
	//过滤领主
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iOccupation == MasterInfo.iArmyID[i])
		{
			MasterInfo.iPosition=i;
			MasterInfo.iHpMax[i]=FightingUser.iHPMax;
			MasterInfo.iMpMax[i]=FightingUser.iMPMax;
			MasterInfo.iMP=FightingUser.iMP;
			MasterInfo.iArmyID[i]=0;
			MasterInfo.iOccupation=iOccupation;
			MasterInfo.iPetChangeID=FightingInfo.FightingData.units[FightingUser.iPosition].iPetChangeID;
			memcpy(&MasterInfo.SkillEquip[i],&FightingUser.SkillEquip,sizeof(FightingUser.SkillEquip));
			break;
		}
	}

	//插入领主信息
	m_pFightingLogic->SetUserInfo(MasterInfo.iPosition,FightingUser);

	//插入军队
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(MasterInfo.iArmyID[i] == 0)continue;

		memset(&FightingUser,0,sizeof(FightingUser));
		m_pFightingLogic->GetFightingInfo(FightingInfo.FightingData,FightingUser,i);
		m_pFightingLogic->SetUserInfo(i,FightingUser);

		MasterInfo.iHpMax[i]=FightingUser.iHPMax;
		MasterInfo.iMpMax[i]=FightingUser.iMPMax;
		MasterInfo.iArmyID[i]=G_GetArmyID(FightingInfo.FightingData.units[i].type,FightingUser.iGrade);
		memcpy(&MasterInfo.SkillEquip[i],&FightingUser.SkillEquip,sizeof(FightingUser.SkillEquip));
	}


	pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_MASTER,(CT_CHAR*)&MasterInfo,sizeof(MasterInfo));


	//加载怪物
	if(!LoadMoster(pNetHandle,FightingInfo.iWorldSceneID,FightingInfo.iSceneID,FightingInfo.iDifficulty,MasterInfo.iGrade))
	{
		CCLogMeg::getSingleton().WriteError("战斗怪物加载失败!");
		return CT_FALSE;
	}

	return CT_TRUE;
}

//加载怪物
CT_BOOL CCFightingEvnet1::LoadMoster(CCNetInterface* pNetHandle,CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iMasterGrade)
{
	//加载怪物
	CT_BYTE cbPlayer=0;
	CMD_G_MosterInfo MosterInfo={0};
	G_MonsterGroup MonsterGroup={0};

	if(!CCFightingScriptSetting::getSingleton().GetMonsterGroup(iWorldSceneID,iSceneID,iDifficulty,MonsterGroup))
	{
		CCLogMeg::getSingleton().WriteError("读取怪物组失败!");
		return CT_FALSE;
	}

	G_ValueRange ValueRound={0};
	if(!G_GetValueRange(VALUE_TYPE_FIGHTING_ROUND,&ValueRound))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::FigheingInfo()VALUE_TYPE_FIGHTING_ROUND");
	}
	CT_INT32 iRound=(CT_INT32)ValueRound.fMinVal;
	for(CT_BYTE i=0;i<POST_COUNT/2;++i)
	{
		G_MonsterBaseInfo MonsterInfo={0};
		CCFightingLogic::FightingUserInfo FightingUser={0};

		if(!CCFightingScriptSetting::getSingleton().GetMonsterData(MonsterGroup,i,MonsterInfo))
		{
			continue;
		}

		m_pFightingLogic->GetFightingInfo(MonsterInfo,FightingUser);
		FightingUser.iAttack=FightingUser.iAttack*(1+MonsterGroup.iPowerStrengthen*iMasterGrade);
		FightingUser.iDefence=FightingUser.iDefence*(1+MonsterGroup.iPowerStrengthen*iMasterGrade);

		CT_INT32 iPos=i;
		FightingUser.iHPMax=FightingUser.iHPMax*(1+MonsterGroup.iHPStrengthen*iMasterGrade);
		FightingUser.iHP=FightingUser.iHPMax;

		MosterInfo.iMosterID[iPos]=MonsterInfo.iID;
		MosterInfo.iMaxHP[iPos]=FightingUser.iHPMax;
		MosterInfo.iMaxMP[iPos]=FightingUser.iMPMax;
		FightingUser.iOccupation=MonsterInfo.iID>>16;
		printf("monsterid:%d monster type:%d\n",MonsterInfo.iID,FightingUser.iOccupation);
		memcpy(&MosterInfo.SkillEquip[iPos],&FightingUser.SkillEquip,sizeof(FightingUser.SkillEquip));

		FightingUser.cbStatusCount=1;
		FightingUser.iSkillStatus[0]=BATTLEARRAY_EFFECT;
		FightingUser.iStatusTime[0] = iRound;

		m_pFightingLogic->SetUserInfo(i+POST_COUNT/2,FightingUser);

		cbPlayer++;
	}

	//加载怪物失败
	if(cbPlayer == 0)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::LoadMoster 加载怪物失败! WorldSceneID%d iSceneID:%d iDifficulty:%d",iWorldSceneID,iSceneID,iDifficulty);
		return CT_FALSE;
	}


	//发送怪物信息
	pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_MOSTER_INFO,(CT_CHAR*)&MosterInfo,sizeof(MosterInfo));

	return CT_TRUE;
}

//加载事件怪物
CT_BOOL CCFightingEvnet1::LoadEventMoster(CCNetInterface* pNetHandle,CT_INT32 iEvent,CT_INT32 iMasterGrade)
{
	//加载怪物
	CT_BYTE cbPlayer=0;
	CMD_G_MosterInfo MosterInfo={0};
	G_MonsterGroup MonsterGroup={0};

	G_FightEventProbablity FightingEvent={0};
	if(!CCEventScriptSetting::getSingleton().GetFightEvent(iEvent,FightingEvent))
	{
		CCLogMeg::getSingleton().WriteError("读取战斗事件失败!");
		return CT_FALSE;
	}

	if(!CCEventScriptSetting::getSingleton().GetEventMonsterGroup(FightingEvent.iMosterGroup,MonsterGroup))
	{
		CCLogMeg::getSingleton().WriteError("读取事件怪物组失败!");
		return CT_FALSE;
	}

	G_RankStatus RankStatus={0};
	for(CT_BYTE i=0;i<POST_COUNT/2;++i)
	{
		G_MonsterBaseInfo MonsterInfo={0};
		CCFightingLogic::FightingUserInfo FightingUser={0};

		if(!CCFightingScriptSetting::getSingleton().GetMonsterData(MonsterGroup,i,MonsterInfo))
		{
			continue;
		}

		m_pFightingLogic->GetFightingInfo(MonsterInfo,FightingUser);
		CT_DOUBLE dPower=MonsterGroup.iPowerStrengthen/100.0f*pow((CT_DOUBLE)iMasterGrade,3);
		FightingUser.iAttack=FightingUser.iAttack+dPower;
		FightingUser.iDefence=FightingUser.iDefence+dPower;

		CT_INT32 iPos=i;
		FightingUser.iHPMax=FightingUser.iHPMax+MonsterGroup.iHPStrengthen/100*pow((CT_DOUBLE)iMasterGrade,3);
		FightingUser.iHP=FightingUser.iHPMax;

		MosterInfo.iMosterID[iPos]=MonsterInfo.iID;
		MosterInfo.iMaxHP[iPos]=FightingUser.iHPMax;
		MosterInfo.iMaxMP[iPos]=FightingUser.iMPMax;
		FightingUser.iOccupation=MonsterInfo.iID>>16;

		FightingUser.cbStatusCount=1;
		FightingUser.iSkillStatus[0]=BATTLEARRAY_EFFECT;

		m_pFightingLogic->SetUserInfo(i+POST_COUNT/2,FightingUser);

		cbPlayer++;
	}

	//加载怪物失败
	if(cbPlayer == 0)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::LoadEventMoster 加载怪物失败! MonsterGroup%d",FightingEvent.iMosterGroup);
		return CT_FALSE;
	}


	//发送怪物信息
	pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_MOSTER_INFO,(CT_CHAR*)&MosterInfo,sizeof(MosterInfo));

	return CT_TRUE;
}

//加载组队怪物
CT_BYTE CCFightingEvnet1::LoadTeamMoster(const CT_INT32 iTeamMoster[TEAM_NUM],CT_INT32 iIndex,CT_BOOL bWin,const CT_INT32 iSceneID,CT_INT32 iMasterGrade)
{
	if(bWin)return LOAD_SUC;
	if(iIndex >= TEAM_NUM)return LOAD_OVER;

	if(iTeamMoster[iIndex] != 0)
	{
		//加载怪物
		CT_BYTE cbPlayer=0;
		CMD_G_MosterInfo MosterInfo={0};

		for(CT_BYTE i=0;i<POST_COUNT/2;++i)
		{
			G_MonsterBaseInfo MonsterInfo={0};
			CCFightingLogic::FightingUserInfo FightingUser={0};
			G_MonsterGroup MonsterGroup={0};

			if(!G_GetMonsterGroup(iSceneID,iTeamMoster[iIndex],&MonsterGroup))
			{
				CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::GetTeamMoster G_GetMonsterGroup!");
				return LOAD_FAIL;
			}

			G_ValueRange ValueRound={0};
			if(!G_GetValueRange(VALUE_TYPE_FIGHTING_ROUND,&ValueRound))
			{
				CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::GetTeamMoster() VALUE_TYPE_FIGHTING_ROUND");
				return LOAD_FAIL;
			}
			CT_INT32 iRound=(CT_INT32)ValueRound.fMinVal;

			if(!CCFightingScriptSetting::getSingleton().GetMonsterData(MonsterGroup,i,MonsterInfo))
			{
				continue;
			}

			m_pFightingLogic->GetFightingInfo(MonsterInfo,FightingUser);
			FightingUser.iAttack=FightingUser.iAttack*(1+MonsterGroup.iPowerStrengthen*iMasterGrade);
			FightingUser.iDefence=FightingUser.iDefence*(1+MonsterGroup.iPowerStrengthen*iMasterGrade);

			CT_INT32 iPos=i;
			FightingUser.iHPMax=FightingUser.iHPMax*(1+MonsterGroup.iHPStrengthen*iMasterGrade);
			FightingUser.iHP=FightingUser.iHPMax;

			MosterInfo.iMosterID[iPos]=MonsterInfo.iID;
			MosterInfo.iMaxHP[iPos]=FightingUser.iHPMax;
			MosterInfo.iMaxMP[iPos]=FightingUser.iMPMax;
			FightingUser.iOccupation=MonsterInfo.iID>>16;

			FightingUser.cbStatusCount=1;
			FightingUser.iSkillStatus[0]=BATTLEARRAY_EFFECT;
			FightingUser.iStatusTime[0] = iRound;

			m_pFightingLogic->SetUserInfo(i+POST_COUNT/2,FightingUser);

			cbPlayer++;
		}

		//加载怪物失败
		if(cbPlayer == 0)
		{
			CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::LoadTeamMoster 加载组队怪物失败!");
			return LOAD_FAIL;
		}
	}
	else
	{
		return LOAD_OVER;
	}

	return LOAD_SUC;
}

//加载PVP战斗
CT_BOOL CCFightingEvnet1::LoadPVPFighting(const MSC_PVP_Fighting_Info &FightingInfo,CCNetInterface* pNetHandle,CT_DWORD dwMeUserID)
{
	for(CT_INT32 i=0;i<PVP_USER_COUNT;++i)
	{
		//提取领主信息
		CT_DWORD dwUserID=0;
		CCFightingLogic::FightingUserInfo FightingUser={0};
		m_pFightingLogic->GetFightingInfo(FightingInfo.FightingData[i],FightingUser);
		m_pFightingLogic->SetSeed(FightingInfo.iSeed);
		FightingUser.dwUserID=FightingInfo.FightingData[i].dwUserID;
		dwUserID=FightingUser.dwUserID;

		//发送领主信息
		CMD_G_MasterInfo MasterInfo={0};
		MasterInfo.iSkillID[0]=FightingUser.iSkill2.iID;
		MasterInfo.iSkillID[1]=FightingUser.iSkill3.iID;
		MasterInfo.iSkillID[2]=FightingUser.iSkill4.iID;
		MasterInfo.iOccupation=FightingInfo.FightingData[i].units[FightingUser.iPosition].type;
		MasterInfo.iGrade=FightingUser.iGrade;

		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			MasterInfo.iArmyID[j]=FightingInfo.FightingData[i].units[j].type;
		}
		memcpy(MasterInfo.cMasterName,FightingInfo.FightingData[i].units[FightingUser.iPosition].name,sizeof(MasterInfo.cMasterName));

		//过滤领主
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			if(MasterInfo.iOccupation == MasterInfo.iArmyID[j])
			{
				MasterInfo.iPosition=j;
				MasterInfo.iHpMax[j]=FightingUser.iHPMax;
				MasterInfo.iMpMax[j]=FightingUser.iMPMax;
				MasterInfo.iMP=FightingUser.iMP;
				MasterInfo.iArmyID[j]=0;
				MasterInfo.iPetChangeID=FightingInfo.FightingData[i].units[FightingUser.iPosition].iPetChangeID;
				memcpy(&MasterInfo.SkillEquip[j],&FightingUser.SkillEquip,sizeof(MasterInfo.SkillEquip[j]));
				break;
			}
		}

		//插入领主信息
		CT_INT32 iPosition=MasterInfo.iPosition;
		if(dwMeUserID == dwUserID)
		{
			m_pFightingLogic->SetUserInfo(iPosition%(POST_COUNT/2),FightingUser);
		}
		else
		{
			m_pFightingLogic->SetUserInfo((iPosition+POST_COUNT/2)%POST_COUNT,FightingUser);
		}

		//插入军队
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			if(MasterInfo.iArmyID[j] == 0)continue;

			memset(&FightingUser,0,sizeof(FightingUser));
			m_pFightingLogic->GetFightingInfo(FightingInfo.FightingData[i],FightingUser,j);

			if(dwMeUserID == dwUserID)
				m_pFightingLogic->SetUserInfo(j%(POST_COUNT/2),FightingUser);
			else 
				m_pFightingLogic->SetUserInfo((j+POST_COUNT/2)%POST_COUNT,FightingUser);

			MasterInfo.iHpMax[j]=FightingUser.iHPMax;
			MasterInfo.iMpMax[j]=FightingUser.iMPMax;
			MasterInfo.iArmyID[j]=G_GetArmyID(FightingInfo.FightingData[i].units[j].type,FightingUser.iGrade);
			memcpy(&MasterInfo.SkillEquip[j],&FightingUser.SkillEquip,sizeof(MasterInfo.SkillEquip[j]));
		}


		if(dwMeUserID == dwUserID)
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_MASTER,(CT_CHAR*)&MasterInfo,sizeof(MasterInfo));
		else
			pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_RIVAL,(CT_CHAR*)&MasterInfo,sizeof(MasterInfo));


	}
	return CT_TRUE;
}

//加载组队战斗
CT_BYTE CCFightingEvnet1::LoadTeamFighting(const UserFightingStatus *FightingInfo,CT_BOOL bWin,CT_DWORD dwMeUserID,CT_INT32 iIndex)
{
	if(bWin)return LOAD_SUC;
	if(iIndex >= TEAM_NUM)return LOAD_OVER;
	if(FightingInfo[iIndex].dwUserID != 0)
	{
		//提取领主信息
		CT_DWORD dwUserID=0;
		CCFightingLogic::FightingUserInfo FightingUser={0};
		m_pFightingLogic->GetFightingInfo(FightingInfo[iIndex],FightingUser);
		FightingUser.dwUserID=FightingInfo[iIndex].dwUserID;
		dwUserID=FightingUser.dwUserID;

		//发送领主信息
		CMD_G_MasterInfo MasterInfo={0};
		MasterInfo.iSkillID[0]=FightingUser.iSkill2.iID;
		MasterInfo.iSkillID[1]=FightingUser.iSkill3.iID;
		MasterInfo.iSkillID[2]=FightingUser.iSkill4.iID;
		MasterInfo.iOccupation=(FightingUser.iOccupation != 0?FightingInfo[iIndex].units[FightingUser.iPosition].type:0);
		MasterInfo.iGrade=FightingUser.iGrade;

		for(CT_INT32 i=0;i<POST_COUNT/2;++i)
		{
			MasterInfo.iArmyID[i]=FightingInfo[iIndex].units[i].type;
		}
		memcpy(MasterInfo.cMasterName,FightingInfo[iIndex].units[FightingUser.iPosition].name,sizeof(MasterInfo.cMasterName));

		//过滤领主;
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			if(MasterInfo.iOccupation == MasterInfo.iArmyID[j])
			{
				MasterInfo.iPosition=j;
				MasterInfo.iHpMax[j]=FightingUser.iHPMax;
				MasterInfo.iMpMax[j]=FightingUser.iMPMax;
				MasterInfo.iMP=FightingUser.iMP;
				MasterInfo.iArmyID[j]=0;
				MasterInfo.iPetChangeID=FightingInfo[iIndex].units[FightingUser.iPosition].iPetChangeID;
				memcpy(&MasterInfo.SkillEquip[j],&FightingUser.SkillEquip,sizeof(MasterInfo.SkillEquip[j]));
				break;
			}
		}

		//插入领主信息
		CT_INT32 iPosition=MasterInfo.iPosition;
		if(dwMeUserID == dwUserID)
		{
			m_pFightingLogic->SetUserInfo(iPosition%(POST_COUNT/2),FightingUser);
		}
		else
		{
			m_pFightingLogic->SetUserInfo((iPosition+POST_COUNT/2)%POST_COUNT,FightingUser);
		}

		//插入军队
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			if(MasterInfo.iArmyID[j] == 0)continue;

			memset(&FightingUser,0,sizeof(FightingUser));
			m_pFightingLogic->GetFightingInfo(FightingInfo[iIndex],FightingUser,j);

			if(dwMeUserID == dwUserID)
				m_pFightingLogic->SetUserInfo(j%(POST_COUNT/2),FightingUser);
			else
				m_pFightingLogic->SetUserInfo((j+POST_COUNT/2)%POST_COUNT,FightingUser);

			MasterInfo.iHpMax[j]=FightingUser.iHPMax;
			MasterInfo.iMpMax[j]=FightingUser.iMPMax;

			MasterInfo.iArmyID[j]=G_GetArmyID(FightingInfo[iIndex].units[j].type,FightingUser.iGrade);
			memcpy(&MasterInfo.SkillEquip[j],&FightingUser.SkillEquip,sizeof(MasterInfo.SkillEquip[j]));
		}
	}
	else
	{
		return LOAD_OVER;
	}
	return LOAD_SUC;
}

//获取组队信息
CT_BOOL CCFightingEvnet1::GetFightingTeam(const UserFightingStatus *FightingInfo,CMD_G_MasterInfo *FightingTeam,CT_INT32 iNum)
{
	for(CT_INT32 i=0;i<iNum;++i)
	{
		if(FightingInfo[i].dwUserID != 0)
		{
			//提取领主信息
			CT_DWORD dwUserID=0;
			CCFightingLogic::FightingUserInfo FightingUser={0};
			m_pFightingLogic->GetFightingInfo(FightingInfo[i],FightingUser);
			FightingUser.dwUserID=FightingInfo[i].dwUserID;
			dwUserID=FightingUser.dwUserID;

			//发送领主信息
			CMD_G_MasterInfo MasterInfo={0};
			MasterInfo.iSkillID[0]=FightingUser.iSkill2.iID;
			MasterInfo.iSkillID[1]=FightingUser.iSkill3.iID;
			MasterInfo.iSkillID[2]=FightingUser.iSkill4.iID;
			MasterInfo.iOccupation=(FightingUser.iOccupation !=0?FightingInfo[i].units[FightingUser.iPosition].type:0);
			MasterInfo.iGrade=FightingUser.iGrade;
			MasterInfo.iPetChangeID=FightingInfo[i].units[FightingUser.iPosition].iPetChangeID;
			for(CT_INT32 j=0;j<POST_COUNT/2;++j)
			{
				MasterInfo.iArmyID[j]=FightingInfo[i].units[j].type;
			}
			memcpy(MasterInfo.cMasterName,FightingInfo[i].units[FightingUser.iPosition].name,sizeof(MasterInfo.cMasterName));

			//过滤领主
			for(CT_INT32 j=0;j<POST_COUNT/2;++j)
			{
				if(MasterInfo.iOccupation == MasterInfo.iArmyID[j])
				{
					MasterInfo.iPosition=j;
					MasterInfo.iHpMax[j]=FightingUser.iHPMax;
					MasterInfo.iMpMax[j]=FightingUser.iMPMax;
					MasterInfo.iMP=FightingUser.iMP;
					MasterInfo.iArmyID[j]=0;
					memcpy(&MasterInfo.SkillEquip[j],&FightingUser.SkillEquip,sizeof(MasterInfo.SkillEquip[j]));
					break;
				}
			}

			//插入军队
			for(CT_INT32 j=0;j<POST_COUNT/2;++j)
			{
				if(MasterInfo.iArmyID[j] == 0)continue;

				memset(&FightingUser,0,sizeof(FightingUser));
				m_pFightingLogic->GetFightingInfo(FightingInfo[i],FightingUser,j);

				MasterInfo.iHpMax[j]=FightingUser.iHPMax;
				MasterInfo.iMpMax[j]=FightingUser.iMPMax;

				if(FightingInfo[i].units[FightingUser.iPosition].iPetChangeID != 0)
					MasterInfo.iPetChangeID=FightingInfo[i].units[FightingUser.iPosition].iPetChangeID;

				MasterInfo.iArmyID[j]=G_GetArmyID(FightingInfo[i].units[j].type,FightingUser.iGrade);
				memcpy(&MasterInfo.SkillEquip[j],&FightingUser.SkillEquip,sizeof(MasterInfo.SkillEquip[j]));
			}

			memcpy(&FightingTeam[i],&MasterInfo,sizeof(MasterInfo));
		}
		else
			break;
	}
	return CT_TRUE;
}

//获取组队怪物信息
CT_BOOL CCFightingEvnet1::GetTeamMoster(const CT_INT32 iTeamMoster[TEAM_NUM],CMD_G_MosterInfo *FightingTeam,const CT_INT32 iSceneID)
{
	for(CT_INT32 j=0;j<TEAM_NUM;++j)
	{
		//加载怪物
		CT_BYTE cbPlayer=0;
		CMD_G_MosterInfo MosterInfo={0};
		G_RankStatus RankStatus={0};

		if(iTeamMoster[j] == 0)continue;
		for(CT_BYTE i=0;i<POST_COUNT/2;++i)
		{
			G_MonsterBaseInfo MonsterInfo={0};
			CCFightingLogic::FightingUserInfo FightingUser={0};
			G_MonsterGroup MonsterGroup={0};

			if(!G_GetMonsterGroup(iSceneID,iTeamMoster[j],&MonsterGroup))
			{
				CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::GetTeamMoster G_GetMonsterGroup!");
				return CT_FALSE;
			}

			G_ValueRange ValueRound={0};
			if(!G_GetValueRange(VALUE_TYPE_FIGHTING_ROUND,&ValueRound))
			{
				CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::GetTeamMoster() VALUE_TYPE_FIGHTING_ROUND");
			}
			CT_INT32 iRound=(CT_INT32)ValueRound.fMinVal;

			if(!CCFightingScriptSetting::getSingleton().GetMonsterData(MonsterGroup,i,MonsterInfo))
			{
				continue;
			}

			m_pFightingLogic->GetFightingInfo(MonsterInfo,FightingUser);
			FightingUser.iAttack+=FightingUser.iAttack*MonsterGroup.iPowerStrengthen;

			CT_INT32 iPos=i;
			FightingUser.iHPMax=FightingUser.iHPMax*(1+MonsterGroup.iHPStrengthen);
			FightingUser.iHP=FightingUser.iHPMax;

			MosterInfo.iMosterID[iPos]=MonsterInfo.iID;
			MosterInfo.iMaxHP[iPos]=FightingUser.iHPMax;
			MosterInfo.iMaxMP[iPos]=FightingUser.iMPMax;

			FightingUser.cbStatusCount=1;
			FightingUser.iSkillStatus[0]=BATTLEARRAY_EFFECT;
			FightingUser.iStatusTime[0] = iRound;

			cbPlayer++;
		}

		//加载怪物失败
		if(cbPlayer == 0)
		{
			CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::GetTeamMoster load monster fail! MonsterGroup%d %d %d",iTeamMoster[0],iTeamMoster[1],iTeamMoster[2]);
			return CT_FALSE;
		}


		//发送怪物信息
		memcpy(&FightingTeam[j],&MosterInfo,sizeof(MosterInfo));
	}
	return CT_TRUE;
}

//发送战报
CT_VOID CCFightingEvnet1::SendFighting(FightingTeamPVP &TeamPVP,CCNetInterface* pNetHandle,const CT_INT32 iSubID)
{
	//发送战报
	CMD_TeamFighting FightingInfo={0};
	CT_INT32 iSize=sizeof(FightingInfo);

	memset(m_pFightingBuffer,0,sizeof(MAX_FIGHTINGSIZE));
	memcpy(m_pFightingBuffer+iSize,TeamPVP.iLen,sizeof(TeamPVP.iLen));
	iSize+=sizeof(TeamPVP.iLen);
	memcpy(m_pFightingBuffer+iSize,TeamPVP.iWiner,sizeof(TeamPVP.iWiner));
	iSize+=sizeof(TeamPVP.iWiner);
	memcpy(m_pFightingBuffer+iSize,TeamPVP.MasterInfo,sizeof(TeamPVP.MasterInfo));
	iSize+=sizeof(TeamPVP.MasterInfo);
	memcpy(m_pFightingBuffer+iSize,TeamPVP.RivalInfo,sizeof(TeamPVP.RivalInfo));
	iSize+=sizeof(TeamPVP.RivalInfo);

	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		if(TeamPVP.FSList[i].end() == TeamPVP.FSList[i].begin())break;
		CCFightingLogic::FightingSaveIT FightingEnd=TeamPVP.FSList[i].end();
		for(CCFightingLogic::FightingSaveIT it=TeamPVP.FSList[i].begin();it != FightingEnd;++it)
		{
			FightingSave SaveInfo={0};
			memcpy(&SaveInfo,&(*it),sizeof(FightingSave));
			memcpy(m_pFightingBuffer+iSize,(CT_CHAR*)&SaveInfo,sizeof(FightingSave));
			iSize+=sizeof(SaveInfo);
		}
	}

	CT_INT32 iInLen=iSize-sizeof(CMD_TeamFighting);
	CT_INT32 iOutLen=MAX_FIGHTINGSIZE;
	CT_CHAR *cBuffer=m_pFightingBuffer+sizeof(CMD_TeamFighting);
	if(CCCompress::getSingleton().Compress(m_pFightingBuffer+sizeof(CMD_TeamFighting),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		FightingInfo.iCompression=CT_TRUE;
		memcpy(m_pFightingBuffer,&FightingInfo,sizeof(CMD_TeamFighting));
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,iSubID,(CT_CHAR*)m_pFightingBuffer,  iOutLen+sizeof(CMD_TeamFighting));
	}
	else
	{
		//压缩失败
		FightingInfo.iCompression=CT_FALSE;
		memcpy(m_pFightingBuffer,&FightingInfo,sizeof(CMD_TeamFighting));
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,iSubID,m_pFightingBuffer,  iSize);
	}
	return;
}

//发送战报
CT_VOID CCFightingEvnet1::SendFighting(FightingTeamPVE &TeamPVE,CCNetInterface* pNetHandle,const CT_INT32 iSubID)
{
	//发送战报
	CMD_TeamFighting FightingInfo={0};
	CT_INT32 iSize=sizeof(FightingInfo);

	memset(m_pFightingBuffer,0,sizeof(MAX_FIGHTINGSIZE));
	memcpy(m_pFightingBuffer+iSize,TeamPVE.iLen,sizeof(TeamPVE.iLen));
	iSize+=sizeof(TeamPVE.iLen);
	memcpy(m_pFightingBuffer+iSize,TeamPVE.iWiner,sizeof(TeamPVE.iWiner));
	iSize+=sizeof(TeamPVE.iWiner);
	memcpy(m_pFightingBuffer+iSize,TeamPVE.MasterInfo,sizeof(TeamPVE.MasterInfo));
	iSize+=sizeof(TeamPVE.MasterInfo);
	memcpy(m_pFightingBuffer+iSize,TeamPVE.MosterInfo,sizeof(TeamPVE.MosterInfo));
	iSize+=sizeof(TeamPVE.MosterInfo);

	for(CT_INT32 i=0;i<FIGHTING_NUM;++i)
	{
		if(TeamPVE.FSList[i].end() == TeamPVE.FSList[i].begin())break;
		CCFightingLogic::FightingSaveIT FightingEnd=TeamPVE.FSList[i].end();
		for(CCFightingLogic::FightingSaveIT it=TeamPVE.FSList[i].begin();it != FightingEnd;++it)
		{
			FightingSave SaveInfo={0};
			memcpy(&SaveInfo,&(*it),sizeof(FightingSave));
			memcpy(m_pFightingBuffer+iSize,(CT_CHAR*)&SaveInfo,sizeof(FightingSave));
			iSize+=sizeof(SaveInfo);
		}
	}

	CT_INT32 iInLen=iSize-sizeof(CMD_TeamFighting);
	CT_INT32 iOutLen=MAX_FIGHTINGSIZE;
	CT_CHAR *cBuffer=m_pFightingBuffer+sizeof(CMD_TeamFighting);
	if(CCCompress::getSingleton().Compress(m_pFightingBuffer+sizeof(CMD_TeamFighting),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		FightingInfo.iCompression=CT_TRUE;
		memcpy(m_pFightingBuffer,&FightingInfo,sizeof(CMD_TeamFighting));
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,iSubID,(CT_CHAR*)m_pFightingBuffer,  iOutLen+sizeof(CMD_TeamFighting));
	}
	else
	{
		//压缩失败
		FightingInfo.iCompression=CT_FALSE;
		memcpy(m_pFightingBuffer,&FightingInfo,sizeof(CMD_TeamFighting));
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,iSubID,m_pFightingBuffer,  iSize);
	}
	return;
}

//加载事件战斗
CT_BOOL	CCFightingEvnet1::LoadEventFihting(const MSC_Fighting_Info &FightingInfo,CCNetInterface* pNetHandle)
{
	//提取领主信息
	CCFightingLogic::FightingUserInfo FightingUser={0};
	m_pFightingLogic->GetFightingInfo(FightingInfo.FightingData,FightingUser);
	m_pFightingLogic->SetSeed(FightingInfo.iSeed);
	FightingUser.dwUserID=FightingInfo.dwUserID;

	//发送领主信息
	CMD_G_MasterInfo MasterInfo={0};
	MasterInfo.iGrade=FightingUser.iGrade;
	MasterInfo.iOccupation=FightingInfo.FightingData.units[FightingUser.iPosition].type;
	MasterInfo.iSkillID[0]=FightingUser.iSkill2.iID;
	MasterInfo.iSkillID[1]=FightingUser.iSkill3.iID;
	MasterInfo.iSkillID[2]=FightingUser.iSkill4.iID;
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		MasterInfo.iArmyID[i]=FightingInfo.FightingData.units[i].type;
	}
	memcpy(MasterInfo.cMasterName,FightingInfo.FightingData.units[FightingUser.iPosition].name,sizeof(MasterInfo.cMasterName));

	//过滤领主
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(MasterInfo.iOccupation == MasterInfo.iArmyID[i])
		{
			MasterInfo.iPosition=i;
			MasterInfo.iHpMax[i]=FightingUser.iHPMax;
			MasterInfo.iMpMax[i]=FightingUser.iMPMax;
			MasterInfo.iMP=FightingUser.iMP;
			MasterInfo.iArmyID[i]=0;
			MasterInfo.iPetChangeID=FightingInfo.FightingData.units[FightingUser.iPosition].iPetChangeID;
			memcpy(&MasterInfo.SkillEquip[i],&FightingUser.SkillEquip,sizeof(FightingUser.SkillEquip));
			break;
		}
	}

	//插入领主信息
	CT_INT32 iPosition=(FightingUser.iPosition)%(POST_COUNT/2);
	m_pFightingLogic->SetUserInfo(iPosition,FightingUser);

	//插入军队
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(MasterInfo.iArmyID[i] == 0)continue;
		memset(&FightingUser,0,sizeof(FightingUser));
		m_pFightingLogic->GetFightingInfo(FightingInfo.FightingData,FightingUser,i);
		m_pFightingLogic->SetUserInfo(i,FightingUser);

		MasterInfo.iHpMax[i]=FightingUser.iHPMax;
		MasterInfo.iMpMax[i]=FightingUser.iMPMax;
		MasterInfo.iArmyID[i]=G_GetArmyID(FightingInfo.FightingData.units[i].type,FightingUser.iGrade);
		memcpy(&MasterInfo.SkillEquip[i],&FightingUser.SkillEquip,sizeof(FightingUser.SkillEquip));
	}


	pNetHandle->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_MASTER,(CT_CHAR*)&MasterInfo,sizeof(MasterInfo));


	//加载怪物
	if(!LoadEventMoster(pNetHandle,FightingInfo.iSceneID,MasterInfo.iGrade))
	{
		CCLogMeg::getSingleton().WriteError("战斗怪物加载失败!");
		return CT_FALSE;
	}
	return CT_TRUE;
}
//开始战斗
CT_BOOL CCFightingEvnet1::BeginFighting()
{
	//战斗信息
	if(m_pFightingLogic->FigheingInfo() == CT_FALSE)
	{
		CCLogMeg::getSingleton().WriteError("战斗失败!");
		return CT_FALSE;
	}

	return CT_TRUE;
}


//战斗结束
CT_BOOL CCFightingEvnet1::EndFighting(CCNetInterface* pNetHandle,CT_DWORD dwSubID)
{
	//发送战报
	CMD_G_FightingInfo FightingInfo={0};
	CCFightingLogic::FightingSaveList FightingList=m_pFightingLogic->GetFightingLis();
	FightingInfo.iLen=FightingList.size()*sizeof(FightingSave);
	FightingInfo.iWiner=m_pFightingLogic->GetWiner();

	memset(m_pFightingBuffer,0,sizeof(MAX_FIGHTINGSIZE));

	CT_BYTE cbSize=0;
	CCFightingLogic::FightingSaveIT FightingEnd=FightingList.end();
	for(CCFightingLogic::FightingSaveIT i=FightingList.begin();i != FightingEnd;++i)
	{

		FightingSave SaveInfo={0};
		memcpy(&SaveInfo,&(*i),sizeof(FightingSave));
		memcpy(m_pFightingBuffer+sizeof(CMD_G_FightingInfo)+cbSize*sizeof(FightingSave),(CT_CHAR*)&SaveInfo,sizeof(FightingSave));
		cbSize++;
	}

	CT_INT32 iInLen=FightingInfo.iLen;
	CT_INT32 iOutLen=MAX_FIGHTINGSIZE;
	CT_CHAR *cBuffer=m_pFightingBuffer+sizeof(CMD_G_FightingInfo);
	if(CCCompress::getSingleton().Compress(m_pFightingBuffer+sizeof(CMD_G_FightingInfo),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		FightingInfo.bCompression=CT_TRUE;
		memcpy(m_pFightingBuffer,&FightingInfo,sizeof(CMD_G_FightingInfo));
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,dwSubID,(CT_CHAR*)m_pFightingBuffer,  iOutLen+sizeof(CMD_G_FightingInfo));
	}
	else
	{
		//压缩失败
		FightingInfo.bCompression=CT_FALSE;
		memcpy(m_pFightingBuffer,&FightingInfo,sizeof(CMD_G_FightingInfo));
		pNetHandle->Send(MAIN_G_FIGHTING_EVENT,dwSubID,m_pFightingBuffer,  FightingInfo.iLen+sizeof(CMD_G_FightingInfo));
	}


	return CT_TRUE;
}

//战报保存
CT_VOID CCFightingEvnet1::SaveFighting(const MSC_Fighting_Info &FightingPVE)
{
	CT_DWORD threadid = GAME_THREAD_CITY + FightingPVE.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_FIGHTING_END,(CT_CHAR*)&FightingPVE,sizeof(FightingPVE));

	return;
}

//PVP战报保存
CT_VOID CCFightingEvnet1::SavePVPFighting(const MSC_PVP_Fighting_Info &FightingPVP/*,MSC_DB_W_PVPFightingInfo &PVPFighting*/)
{
	//const CT_INT32 iLen=sizeof(MSC_PVP_Fighting_Info)+sizeof(MSC_DB_W_PVPFightingInfo);
	//CT_CHAR cBuf[iLen]={0};

	//CT_INT32 iSize=sizeof(FightingPVP);
	//memcpy(cBuf,&FightingPVP,iSize);
	//memcpy(cBuf+iSize,&PVPFighting,sizeof(MSC_DB_W_PVPFightingInfo));


	//CT_DWORD threadid = GAME_THREAD_CITY + FightingPVP.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	//G_SendThreadInfo(threadid,MSC_MAIN_EVENT_PVP,MSC_SUB_PVP_FIGHTING_END,cBuf,iLen);

	CT_DWORD threadid = GAME_THREAD_CITY + FightingPVP.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_EVENT_PVP,MSC_SUB_PVP_FIGHTING_END,(CT_CHAR*)&FightingPVP,sizeof(FightingPVP));

	return;
}


//获得星级
CT_INT32 CCFightingEvnet1::GetStar(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iScore)
{
	CT_INT32 iStar=0;
	G_FightingScene Scene={0};
	if(!CCFightingScriptSetting::getSingleton().GetFightingScene(iWorldSceneID,iSceneID,iDifficulty,Scene))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::GetStar GetFightingScene w:%d s:%d d:%d",iWorldSceneID,iSceneID,iDifficulty);
		return 0;
	}

	G_StarLeve Star={0};
	if(!G_GetStarLeve(Scene.iStarLevelID,&Star))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::GetStar G_GetStarLeve w:%d",Scene.iStarLevelID);
		return 0;
	}

	for(CT_INT32 i=0;i<STAR_LEVEL;++i)
	{
		if(Star.iStar[i] == 0)continue;
		if(iScore >= Star.iStar[i])
		{
			iStar=i+1;
		}
	}
	return iStar;
}


CT_VOID CCFightingEvnet1::Decompress(CT_INT32 *pBuffer,CT_INT32 iLen,CT_INT32 *pFightinginfo)
{
#if 0
	//数据压缩
	CT_INT32 *pBuffer1=0;
	CT_INT32 iOutLen1=0;
	CT_INT32 iLen1=iLen;
	CT_INT32 iOutLen=0;
	while(CT_TRUE)
	{
		iLen1=iLen1*2;
		pBuffer1=new CT_INT32[iLen1];
		CT_INT32 *pInBuffer1=pBuffer+sizeof(CMD_G_FightingInfo);
		iOutLen=LZF_Decompress(&pInBuffer1,iLen,(CT_INT32**)&pBuffer1,iLen1);
		if(iOutLen != 0)
			break;

		//压缩失败
		if(iLen*4 < iLen1)
		{
			break;
		}

		delete[] pBuffer1;
		pBuffer1=0;
	}

	if(memcmp(pFightinginfo+sizeof(CMD_G_FightingInfo),pBuffer1,iOutLen) == 0)
	{
		printf("0");
	}

	delete[] pBuffer1;
	pBuffer1=0;
#endif
	return;
}