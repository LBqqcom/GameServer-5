#include "CBossEvent.h"
#include "../sdk/include/CLogMeg.h"
#include "CUser.h"
#include "CUserManager.h"
#include "CBossManager.h"
#include "../sdk/include/GlobeScriptInterface.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "CThreadMes.h"
#include "../sdk/include/CNetInterface.h"
#include "CActive.h"
#include <time.h>


CCBossEvent::CCBossEvent(CCGameMesManager *pMesManager, CT_DWORD id):CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_INFO_UPDATE);
	RegistMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_JOIN_ACTIVE);
	RegistMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE);
	RegistMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE);
	RegistMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_REWARD);
	RegistMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_EXIT_ACTIVE);
	//RegistMessage(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_CURR_INFO);

	RegistMessage(MSC_MAIN_WORLD_BOSS,MSC_SUB_BOSSACTVIE_OPEN);
	RegistMessage(MSC_MAIN_WORLD_BOSS,MSC_SUB_WORLDBOSS_FINGHTING);

	if (m_dwThread == GAME_THREAD_CITY)
	{
		for(CT_INT32 i=0;i<BOSS_ACTIVE_MAXNUM;++i)
		{
			RegistClock(IDI_BOSSACTIVE_OPEN+i);
		}
		RegistTimer(IDI_BOSSACTIVE_KEEP);
		RegistTimer(IDI_BOSSACTIVE_BOSSINFO);
		RegistTimer(IDI_BOSSACTIVE_REFRESH);
	}

	m_ActiveMap.clear();
}

CCBossEvent::~CCBossEvent(void)
{
}

CT_VOID CCBossEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_WORLD_BOSS:
		{
			DisposeMAIN(subID, pBuf, len);
			break;
		}
	case MSC_MAIN_WORLD_BOSS:
		{
			DisposeMSC(subID, pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCBossEvent::OnUserEvent 主消息错误=%i", mainID);
			break;
		}
	}
}

CT_VOID CCBossEvent::DisposeMAIN(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch (subID)
	{
	case SUB_G_WORLD_BOSS_INFO_UPDATE:
		{
			DisposeInfoUpdate(pBuf, len);
			break;
		}
	case SUB_G_WORLD_BOSS_JOIN_ACTIVE:
		{
			DisposeJoinActive(pBuf, len);
			break;
		}
	case SUB_G_WORLD_BOSS_ENCOURAGE:
		{
			DisposeEncourage(pBuf, len);
			break;
		}
	case SUB_G_WORLD_BOSS_BATTLE:
		{
			DisposeBattle(pBuf, len);
			break;
		}
	case SUB_G_WORLD_BOSS_REWARD:
		{
			DisposeReward(pBuf, len);
			break;
		}
	case SUB_G_WORLD_BOSS_EXIT_ACTIVE:
		{
			DisposeExitActive(pBuf, len);
			break;
		}
// 	case SUB_G_WORLD_BOSS_CURR_INFO:
// 		{
// 			DisposeCurrInfo(pBuf, len);
// 			break;
// 		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCBossEvent::OnUserEvent 消息错误=%d:%d", MSC_MAIN_WORLD_BOSS, subID);
			break;
		}
	}
}

CT_VOID CCBossEvent::DisposeMSC(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch (subID)
	{
	case MSC_SUB_BOSSACTVIE_OPEN:
		{
			DisposeActiveOpen();
			break;
		}
	case MSC_SUB_WORLDBOSS_FINGHTING:
		{
			DisposeFighting(pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCBossEvent::OnUserEvent 消息错误=%d:%d", MAIN_G_WORLD_BOSS, subID);
			break;
		}
	}

}

//用户定时器处理
CT_VOID CCBossEvent::OnTime(CT_DWORD timeID)
{
	if (m_dwThread != GAME_THREAD_CITY)
	{
		return;
	}
	if(IDI_BOSSACTIVE_KEEP == timeID)	//活动到期
	{
		//关闭活动
		KillTimer(timeID);
		KillTimer(IDI_BOSSACTIVE_BOSSINFO);
		KillTimer(IDI_BOSSACTIVE_REFRESH);
		CCBossManager::getSingleton().SetActive(CT_FALSE);

		//发送活动下次开启时间
		CT_INT32 AcviteList[100] = {0};
		CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_BOSS,AcviteList);

		CT_INT32 iLastAEID = 0;
		CT_INT64 lTime=NextActiveTime(AcviteList, iLastAEID);
		CCBossManager::getSingleton().SetLastTime(lTime);
		CCBossManager::getSingleton().SetAEID(iLastAEID);

		CMD_G_BossActive ba={0};
		ba.iTime=CCBossManager::getSingleton().GetLastTime();
		ba.iTime=ba.iTime <= -1?-1:ba.iTime-time(0);
		ba.iAEID = iLastAEID;
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_WORLD_BOSS,SUB_G_WORLD_BOSS_ACTIVE_CLOSE,(CT_CHAR*)&ba,sizeof(ba));

		printf("BOSS活动结束\n");
	}

	if (IDI_BOSSACTIVE_BOSSINFO == timeID)		//定时推送BOSS血量和当前排行榜
	{
		CCBossManager::getSingleton().RefCurrTop();
		CCBossManager::getSingleton().SendHPHurt();
		if (CCBossManager::getSingleton().BossIsDie())
		{
			KillTimer(timeID);
		}
	}
	if (IDI_BOSSACTIVE_REFRESH == timeID)
	{
		if (CCBossManager::getSingleton().BossIsDie())
		{
			KillTimer(timeID);
			return;
		}
		CCBossManager::getSingleton().RefPlayer();
	}
}

//用户时钟处理
CT_VOID CCBossEvent::OnClock(CT_DWORD timeID)
{
	if(IDI_BOSSACTIVE_OPEN <= timeID && timeID < IDI_BOSSACTIVE_OPEN+BOSS_ACTIVE_MAXNUM && m_dwThread == GAME_THREAD_CITY)
	{
		KillClock(timeID);
		// 活动开始
		std::map<CT_INT32,CT_INT32>::iterator it=m_ActiveMap.find(timeID);
		if(it != m_ActiveMap.end())
		{
			G_ActiveEventData aet={0};
			if(G_GetActiveEvent((*it).second,&aet))
			{
				KillTimer(IDI_BOSSACTIVE_KEEP);
				SetTimer(IDI_BOSSACTIVE_KEEP,aet.iProperty1*1000);
				SetTimer(IDI_BOSSACTIVE_BOSSINFO, 5*1000);				//时长需要定义宏
				SetTimer(IDI_BOSSACTIVE_REFRESH, 1*1000);	
				CCBossManager::getSingleton().SetAEID((*it).second);
				CCBossManager::getSingleton().SetActive(CT_TRUE);
				CCBossManager::getSingleton().SetLastTime(time(0)+aet.iProperty1);
				m_ActiveMap.erase(it);
				CMD_G_BossActive ra={0};
				ra.iTime=aet.iProperty1;
				ra.iAEID=CCBossManager::getSingleton().GetAEID();
				ra.iHPMax=CCBossManager::getSingleton().GetBossHPMax();
				ra.iHP = ra.iHPMax;
				ra.iBossGrade=CCBossManager::getSingleton().GetBossGrade();
				if(CCBossManager::getSingleton().GetActive())
				{
					CCUserManager::getSingleton().SendUserMessage(MAIN_G_WORLD_BOSS,SUB_G_WORLD_BOSS_ACTIVE_OPEN,(CT_CHAR*)&ra,sizeof(ra));
				}
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCBossEvent::OnClock G_GetActiveEvent %d",(*it).second);
			}
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::OnClock it == m_ResActiveMap.end()");
		}
	}
}

//第一次打开BOSS活动页面
CT_VOID CCBossEvent::DisposeInfoUpdate(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorldBossARG))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeInfoUpdate len:%d != sizeof(CMD_G_WorldBossARG)",len);
		return;
	}
	CMD_G_WorldBossARG *pARG = (CMD_G_WorldBossARG *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pARG->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeInfoUpdate,can't find user %d",pARG->dwUserID);
		return;
	}

	//之前未领取的奖励
	CCBossManager::getSingleton().SendReward(pARG->dwUserID);

	CMD_G_WorldBossTopInfo topInfo[100] = {0};
	CT_INT32 iLen = sizeof(topInfo);
	if (CCBossManager::getSingleton().GetBossHurt(pARG->dwUserID, (CT_CHAR*)topInfo, iLen))
	{
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_INFO_UPDATE, (CT_CHAR*)topInfo, sizeof(topInfo));
	}
	
}

//加入当前BOSS活动
CT_VOID CCBossEvent::DisposeJoinActive(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorldBossARG))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeJoinActive len:%d != sizeof(CMD_G_WorldBossARG)",len);
		return;
	}
	CMD_G_WorldBossARG *pARG = (CMD_G_WorldBossARG *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pARG->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeJoinActive,can't find user %d",pARG->dwUserID);
		return;
	}

	if (!CCBossManager::getSingleton().GetActive())
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ACTIVE_CLOSE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_JOIN_ACTIVE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	CCBossManager::getSingleton().JoinActive(pARG->dwUserID);
}

CT_VOID CCBossEvent::DisposeExitActive(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorldBossARG))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeExitActive len:%d != sizeof(CMD_G_WorldBossARG)",len);
		return;
	}
	CMD_G_WorldBossARG *pARG = (CMD_G_WorldBossARG *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pARG->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeExitActive,can't find user %d",pARG->dwUserID);
		return;
	}

	CCBossManager::getSingleton().ExitActive(pARG->dwUserID);
}

//激励
CT_VOID CCBossEvent::DisposeEncourage(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorldBossEncourage))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeEncourage len:%d != sizeof(CMD_G_WorldBossEncourage)",len);
		return;
	}
	CMD_G_WorldBossEncourage *pARG = (CMD_G_WorldBossEncourage *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pARG->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeEncourage,can't find user %d",pARG->dwUserID);
		return;
	}

	if (!CCBossManager::getSingleton().GetActive())
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ACTIVE_CLOSE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_ENCOURAGE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	CCBossManager::getSingleton().Encourage(pARG->dwUserID, pARG->iEncourageType);
}

//战斗/复活
CT_VOID CCBossEvent::DisposeBattle(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorldBossBattle))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeBattle len:%d != sizeof(CMD_G_WorldBossBattle)",len);
		return;
	}
	CMD_G_WorldBossBattle *pARG = (CMD_G_WorldBossBattle *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pARG->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeBattle,can't find user %d",pARG->dwUserID);
		return;
	}

	if (!CCBossManager::getSingleton().GetActive())
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ACTIVE_CLOSE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_BATTLE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}
	
	CCBossManager::getSingleton().BattleApply(pARG->dwUserID, pARG->iFlag);
}

//领取奖励
CT_VOID CCBossEvent::DisposeReward(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorldBossARG))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeReward len:%d != sizeof(CMD_G_WorldBossARG)",len);
		return;
	}
	CMD_G_WorldBossARG *pARG = (CMD_G_WorldBossARG *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pARG->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeReward,can't find user %d",pARG->dwUserID);
		return;
	}

	CCBossManager::getSingleton().RecvReward(pARG->dwUserID);
}

/*
CT_VOID CCBossEvent::DisposeCurrInfo(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorldBossARG))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeReward len:%d != sizeof(CMD_G_WorldBossARG)",len);
		return;
	}
	CMD_G_WorldBossARG *pARG = (CMD_G_WorldBossARG *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pARG->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeReward,can't find user %d",pARG->dwUserID);
		return;
	}

	if (!CCBossManager::getSingleton().GetActive())
	{
		CMD_G_WorldBossFail worldBossFail = {0};
		worldBossFail.iFailCode = BOSS_ACTIVE_CLOSE;
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_JOIN_ACTIVE_FAIL, (CT_CHAR*)&worldBossFail, sizeof(CMD_G_WorldBossFail));
		return;
	}

	CMD_G_WorldBossCurrInfo currInfo = {0};
	if (CCBossManager::getSingleton().GetHPHurt(pARG->dwUserID, (CT_CHAR*)currInfo.userInfo, sizeof(currInfo.userInfo)))
	{
		currInfo.iHP = CCBossManager::getSingleton().GetBossHP();
		pUser->GetNetInterface()->Send(MAIN_G_WORLD_BOSS, SUB_G_WORLD_BOSS_CURR_INFO, (CT_CHAR*)&currInfo, sizeof(CMD_G_WorldBossCurrInfo));
	}
} */

CT_VOID CCBossEvent::DisposeFighting(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len!=sizeof(MSC_WorldBoss_Fighting))
	{
		CCLogMeg::getSingleton().WriteError("CCBossEvent::DisposeFighting len:%d != sizeof(MSC_WorldBoss_Fighting)",len);
		return;
	}
	MSC_WorldBoss_Fighting *pARG = (MSC_WorldBoss_Fighting *)pBuf;

	CCBossManager::getSingleton().BattleResult(pARG->dwUserID, pARG->iHPHurt);
}


CT_VOID CCBossEvent::DisposeActiveOpen()
{
	time_t t=time(0);
	tm *ptm=localtime(&t);
	CT_INT32 AcviteList[100] = {0};
	CT_INT32 iLastAEID = 0;
	CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_BOSS,AcviteList);
	if(n > 0)
	{
		m_ActiveMap.clear();
		tm Temptm=(*ptm);

		CT_INT32 iActiveNum=0;
		CT_INT64 lMinTime=0;
		for(CT_INT32 i=0;i<100;++i)
		{
			if(AcviteList[i] != 0)
			{
				G_ActiveEventData aet={0};
				if(G_GetActiveEvent(AcviteList[i],&aet))
				{
					CT_INT32 iTimeDay=(CT_INT32)(aet.iProperty2/(3600*24));
					if(ptm->tm_wday == iTimeDay)
					{
						CT_INT32 iProperty2=(CT_INT32)aet.iProperty2;
						CT_INT32 iHour=iProperty2%(3600*24)/3600;
						CT_INT32 iMin=iProperty2%(3600*24)%3600/60;
						CT_INT32 iClockID=IDI_BOSSACTIVE_OPEN+iActiveNum;

						if(iHour <= ptm->tm_hour && iMin < ptm->tm_min || iHour < ptm->tm_hour)continue;

						SetClock(iClockID,iHour,iMin,0);

						std::map<CT_INT32,CT_INT32>::iterator it=m_ActiveMap.find(iClockID);
						if(it != m_ActiveMap.end())
						{
							m_ActiveMap.erase(it);
							m_ActiveMap.insert(std::map<CT_INT32,CT_INT32>::value_type(iClockID,AcviteList[i]));
						}
						else
							m_ActiveMap.insert(std::map<CT_INT32,CT_INT32>::value_type(iClockID,AcviteList[i]));

						Temptm.tm_hour = iHour;
						Temptm.tm_min = iMin;
						Temptm.tm_sec = 0;

						if((lMinTime > mktime(&Temptm) && mktime(&Temptm) != -1) || lMinTime == 0)
						{
							lMinTime=mktime(&Temptm);
							iLastAEID = AcviteList[i];
						}
						iActiveNum++;
					}
				}
				else
				{
					CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResActiveOpen G_GetActiveEvent %d",AcviteList[i]);
				}
			}
		}

		if(lMinTime != 0)
		{
			CCBossManager::getSingleton().SetLastTime(lMinTime);
		}
		else
		{
			CT_INT64 lTime=NextActiveTime(AcviteList, iLastAEID);
			CCBossManager::getSingleton().SetLastTime(lTime);
		}
		CCBossManager::getSingleton().SetAEID(iLastAEID);

		//发送活动下次开启时间
		CMD_G_BossActive ra={0};
		ra.iTime = CCBossManager::getSingleton().GetLastTime();
		ra.iTime = (ra.iTime <= -1?-1:ra.iTime-t);
		ra.iAEID = iLastAEID;
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_WORLD_BOSS,SUB_G_WORLD_BOSS_ACTIVE_CLOSE,(CT_CHAR*)&ra,sizeof(ra));
	}
}


//计算下次活动时间
CT_INT64 CCBossEvent::NextActiveTime(CT_INT32 AcviteList[100], CT_INT32 &iLastAEID)
{
	time_t t=time(0);
	tm *ptm=localtime(&t);
	CT_INT64 lMinTime=0;

	//当天时间之后计算
	for(CT_INT32 i=0;i<100;++i)
	{
		if(AcviteList[i] != 0)
		{
			G_ActiveEventData aet={0};
			if(G_GetActiveEvent(AcviteList[i],&aet))
			{
				CT_INT32 iTimeDay=(CT_INT32)(aet.iProperty2/(3600*24));
				if(ptm->tm_wday <= iTimeDay)
				{
					CT_INT32 iProperty2=(CT_INT32)aet.iProperty2;
					CT_INT32 iHour=iProperty2%(3600*24)/3600;
					CT_INT32 iMin=iProperty2%(3600*24)%3600/60;

					if((iHour <= ptm->tm_hour && iMin < ptm->tm_min || iHour < ptm->tm_hour) && ptm->tm_wday >= iTimeDay)continue;

					if((lMinTime > iProperty2) || lMinTime == 0)
					{
						lMinTime=iProperty2;
						iLastAEID = AcviteList[i];
					}
				}
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCBossEvent::NextActiveTime G_GetActiveEvent %d",AcviteList[i]);
			}
		}

		if(lMinTime != 0)
		{
			CT_INT32 iTimeDay=(CT_INT32)(lMinTime/(3600*24));
			CT_INT32 iHour=lMinTime%(3600*24)/3600;
			CT_INT32 iMin=lMinTime%(3600*24)%3600/60;
			lMinTime=t+(iTimeDay-ptm->tm_wday)*24*3600+(iHour-ptm->tm_hour)*3600+(iMin-ptm->tm_min)*60;
			return lMinTime;
		}
	}

	//从星期天开始计算
	if(lMinTime == 0)
	{
		for(CT_INT32 i=0;i<100;++i)
		{
			if(AcviteList[i] != 0)
			{
				G_ActiveEventData aet={0};
				if(G_GetActiveEvent(AcviteList[i],&aet))
				{
					CT_INT32 iProperty2=(CT_INT32)aet.iProperty2;
					if (iProperty2 < lMinTime || lMinTime == 0)
					{
						lMinTime = iProperty2;
						iLastAEID = AcviteList[i];
					}
				}
				else
				{
					CCLogMeg::getSingleton().WriteError("CCBossEvent::NextActiveTime G_GetActiveEvent %d",AcviteList[i]);
				}
			}
		}

		if(lMinTime != 0)
		{
			CT_INT32 iTimeDay=(CT_INT32)(lMinTime/(3600*24));
			CT_INT32 iHour=lMinTime%(3600*24)/3600;
			CT_INT32 iMin=lMinTime%(3600*24)%3600/60;
			lMinTime=t+(7-(ptm->tm_wday-iTimeDay))*24*3600+(iHour-ptm->tm_hour)*3600+(iMin-ptm->tm_min)*60;
			return lMinTime;
		}
	}
	return -1;
}

