/****************************************************
	created:	2014/07/10		14:52
	file base:	IslandEvent
	file ext:	cpp
	author:		zhouhuan

	purpose:
*****************************************************/
#include "CUser.h"
#include "CIslandEvent.h"
#include "MessageGame.h"
#include "ProtocolGame.h"
#include "CLogMeg.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"

CCIslandEvent::CCIslandEvent(CCGameMesManager* pMsg, CT_DWORD iID):CCGameMesEvent(pMsg, iID)
{
	RegistMessage(MAIN_G_ISLAND, SUB_G_THROW_DICE);
	RegistMessage(MAIN_G_ISLAND, SUB_G_FIGHT_EVENT);
	RegistMessage(MAIN_G_ISLAND, SUB_G_COMPLETE_IMMEDIATELY);
	RegistMessage(MAIN_G_ISLAND, SUB_G_GIVEUP_EVENT);
	RegistMessage(MAIN_G_ISLAND, SUB_G_GET_CHEST);
	RegistMessage(MAIN_G_ISLAND, SUB_G_GIVE_THROW_TIMES);

	RegistMessage(MSC_MAIN_ISLAND_EVENT, MSC_SUB_ISLAND_FIGHT);
	RegistMessage(MSC_MAIN_ISLAND_EVENT, MSC_SUB_GIVETIMES_OUTLINE);
	RegistMessage(MSC_MAIN_ISLAND_EVENT, MSC_SUB_OPEN_ISLAND);
	RegistMessage(MSC_MAIN_ISLAND_EVENT, MSC_SUB_TASK_COMPLETE);

	G_ValueRange vr = {0};
	if (G_GetValueRange(VALUE_TYPE_ISLAND_UPDATE_TIME, &vr))
	{
		RegistClock(IDI_ISLAND_UPDATE_TIME);
		SetClock(IDI_ISLAND_UPDATE_TIME, (CT_BYTE)vr.fMaxVal, 0, 0);
	}
}

CCIslandEvent::~CCIslandEvent()
{

}

CT_VOID CCIslandEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	if (mainID == MAIN_G_ISLAND)
	{
		switch (subID)
		{
		case SUB_G_THROW_DICE:
			{
				if (len == sizeof(CMD_G_ThrowDice))
				{
					DisposeThrow(pBuf, len);
				}
				break;
			}
		case SUB_G_FIGHT_EVENT:
			{
				if (len == sizeof(CMD_G_FightEvent))
				{
					DisposeFightEvent(pBuf, len);
				}
				break;
			}
		case SUB_G_COMPLETE_IMMEDIATELY:
			{
				if (len == sizeof(CMD_G_CompleteImmediately))
				{
					DisposeCompleteImmediately(pBuf, len);
				}
				break;
			}
		case SUB_G_GIVEUP_EVENT:
			{
				if (len == sizeof(CMD_G_GiveUpEvent))
				{
					DisposeEventGiveup(pBuf, len);
				}
				break;
			}
		case SUB_G_GET_CHEST:
			{
				if (len == sizeof(CMD_GetChest))
				{
					DisposeReward(pBuf, len);
				}
				break;
			}
		case SUB_G_GIVE_THROW_TIMES:
			{
				if (len == sizeof(CMD_GiveThrowTimes))
				{
					DisposeGiveThrowTimes(pBuf, len);
				}
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("CCIslandEvent::OnUserEvent : subID=%i", subID);
				break;
			}
		}
	}
	else if(mainID == MSC_MAIN_ISLAND_EVENT)
	{
		switch(subID)
		{
		case MSC_SUB_ISLAND_FIGHT:
			{
				if (len == sizeof(MSC_Island_FightEvent))
				{
					DisposeFightOver(pBuf, len);
				}
				break;
			}
		case MSC_SUB_GIVETIMES_OUTLINE:
			{
				if (len == sizeof(MSC_DB_IslandGiveTimes))
				{
					DisposeOutLineGiveFail(pBuf, len);
				}
				break;
			}
		case MSC_SUB_OPEN_ISLAND:
			{
				if (len == sizeof(CT_DWORD))
				{
					DisposeOpenIsland(*((CT_DWORD*)pBuf));
				}
				break;
			}
		case MSC_SUB_TASK_COMPLETE:
			{
				if (len == sizeof(MSC_IslandTask))
				{
					DisposeTaskComplete(pBuf,len);
				}
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("CCIslandEvent::OnUserEvent : subID=%i", subID);
				break;
			}
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::OnUserEvent : mainID=%i", mainID);
	}
	
}

CT_VOID CCIslandEvent::OnClock(CT_DWORD timeID)
{
	if (timeID == IDI_ISLAND_UPDATE_TIME)
	{
		CCFightTeamManager::getSingleton().UpdateIslandData();
		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_ONCLOCK,0,0);
	}
}

CT_VOID CCIslandEvent::OnTime(CT_DWORD timeID)
{

}

CT_VOID CCIslandEvent::DisposeThrow(CT_CHAR* pBuf, CT_INT32 iLen)
{
	CMD_G_ThrowDice *pThrowDice = (CMD_G_ThrowDice *)pBuf;

	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pThrowDice->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeThrow cannot find user.uid:%d", pThrowDice->dwUserID);
		return;
	}

	CT_INT32 enfail = pUser->m_userIsland.Throw();
	if (ISLAND_ENFAIL_SUC != enfail)
	{
		CMD_G_Throw_Fail gtf;
		gtf.enFail = (ENUM_ThrowFail)enfail;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_THROW_FAIL, (CT_CHAR*)&gtf, sizeof(gtf));
	}
	
}

CT_VOID CCIslandEvent::DisposeFightEvent(CT_CHAR* pBuf, CT_INT32 iLen)
{
	CMD_G_FightEvent* pFight = (CMD_G_FightEvent*)pBuf;

	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pFight->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeFightEvent connot find user,uid:%d", pFight->dwUserID);
		return;
	}

	ENUM_CompleteEventFail enfail = (ENUM_CompleteEventFail)pUser->m_userIsland.FightEvent(pFight->iEventID);
	if (ISLAND_ENFAIL_SUC != enfail)
	{
		CMD_G_Fight_Fail gff;
		gff.iEventID = pFight->iEventID;
		gff.enFail = enfail;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_FIGHT_EVENT_FAIL, (CT_CHAR*)&gff, sizeof(gff));
	}
}

CT_VOID CCIslandEvent::DisposeCompleteImmediately(CT_CHAR* pBuf, CT_INT32 iLen)
{
	CMD_G_CompleteImmediately* pImmediately = (CMD_G_CompleteImmediately*)pBuf;

	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pImmediately->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeCompleteImmediately connot find user.uid:%d", pImmediately->dwUserID);
		return;
	}

	CT_INT32 iEventID = 0;
	CT_INT32 iFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pImmediately->dwUserID);
	ENUM_CompleteEventFail enfail = (ENUM_CompleteEventFail)pUser->m_userIsland.CompleteImmediately(iEventID,iFTID);
	if (ISLAND_ENFAIL_SUC != enfail)
	{
		CMD_G_Fight_Fail gff;
		gff.enFail = enfail;
		gff.iEventID = iEventID;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_FIGHT_EVENT_FAIL, (CT_CHAR*)&gff, sizeof(gff));
	}
}

CT_VOID CCIslandEvent::DisposeEventGiveup(CT_CHAR* pBuf, CT_INT32 iLen)
{
	CMD_G_GiveUpEvent* pgu = (CMD_G_GiveUpEvent*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pgu->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeEventGiveup connot find user.uid:%d", pgu->dwUserID);
		return;
	}

	pUser->m_userIsland.GiveUpEvent();
}

CT_VOID CCIslandEvent::DisposeReward(CT_CHAR* pBuf, CT_INT32 iLen)
{
	CMD_GetChest* pgc = (CMD_GetChest*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pgc->dwUserID,	&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeReward connot find user.uid:%d", pgc->dwUserID);
		return;
	}
	
	CT_INT32 iFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pgc->dwUserID);
	ENUM_GetChestFail enfail = (ENUM_GetChestFail)pUser->m_userIsland.SendReward(pgc->iType, pgc->iChestID, pgc->iLimit, iFTID);;
	if (ISLAND_ENFAIL_SUC != enfail)
	{
		CMD_GetChest_Fail gcf;
		gcf.enfail = enfail;
		gcf.iChestID = pgc->iChestID;
		gcf.iType = pgc->iType;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GET_CHEST_FAIL, (CT_CHAR*)&gcf, sizeof(gcf));
	}
}

CT_VOID CCIslandEvent::DisposeGiveThrowTimes(CT_CHAR* pBuf, CT_INT32 iLen)
{
	CT_INT32 enfail = ISLAND_ENFAIL_SUC;
	CMD_GiveThrowTimes* pgtt = (CMD_GiveThrowTimes*)pBuf;
	CT_INT32 iFTid = 0;
	CT_INT32 iFTid_F = 0;
	DragonPlayer fplayer = {0};
	G_Building building = {0};
	std::string strName;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pgtt->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeGiveThrowTimes connot find user.uid:%d", pgtt->dwUserID);
		return;
	}

	// 自己不能给自己赠送
	if (pgtt->dwUserID == pgtt->dwFUserID)
	{
		enfail = EGT_NOT_FIGHTMEMBER;
	}

	iFTid = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pgtt->dwUserID);
	iFTid_F = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pgtt->dwFUserID);
	if (iFTid != iFTid_F)
	{
		// 不在同一个战队
		enfail = EGT_NOT_FIGHTMEMBER;
	}

	if (pUser->m_userIsland.CheckGiveUid(pgtt->dwFUserID))
	{
		// 当天已赠送过
		enfail = RGT_GIVED_SAME_MEMBER;
	}

	if (ISLAND_ENFAIL_SUC != enfail)
	{
		CMD_GiveTimes_Fail tfail;
		tfail.enfail = (ENUM_GiveTimesFail)enfail;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GIVE_TIMES_FAIL, (CT_CHAR*)&tfail, sizeof(tfail));
		return;
	}

	CCUser* pFUser;
	if (!CCUserManager::getSingleton().Find(pgtt->dwFUserID, &pFUser))
	{
		MSC_DB_IslandGiveTimes migt = {0};
		migt.dwUserID = pgtt->dwUserID;
		migt.dwFUID = pgtt->dwFUserID;
		CT_DWORD threadID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_GIVETIMES,(CT_CHAR*)&migt,sizeof(migt));
		return;
	}

	pFUser->m_userIsland.GetPlayerInfo(fplayer);
	if (fplayer.iGetTimes >= ISLAND_MAX_GETTIMES)
	{
		// 已达最大接收次数
		enfail = EGT_GETTIMES_LESS;
	}

	if (ISLAND_ENFAIL_SUC != enfail)
	{
		CMD_GiveTimes_Fail tfail;
		tfail.enfail = (ENUM_GiveTimesFail)enfail;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GIVE_TIMES_FAIL, (CT_CHAR*)&tfail, sizeof(tfail));
		return;
	}
	
	// 更新内存
	pUser->m_userIsland.InsertGiveUserID(pgtt->dwFUserID);

	fplayer.iGetTimes += 1;
	fplayer.iTotalTimes += 1;
	pFUser->m_userIsland.SetPlayerInfo(fplayer);

	// 发送客户端
	CMD_UpdateThrowTimes utt = {0};
	MasterData md;
	pUser->m_userBase.GetMasterData(md);
	strncpy(utt.cUserName, md.cNickName, sizeof(utt.cUserName));
	utt.iThrowTimes = 1;
	utt.iType = ISLAND_GIVETIMES_TYPE;
	pFUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_THROW_TIMES, (CT_CHAR*)&utt, sizeof(utt));

	pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GIVE_TIMES_SUC);

	// 同步数据库
	MSC_DB_IslandGiveUid mGiveUid = {0};
	mGiveUid.dwUserID = pgtt->dwUserID;
	mGiveUid.aGiveUid = pgtt->dwFUserID;
	CT_DWORD threadID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_GIVEUID,(CT_CHAR*)&mGiveUid,sizeof(mGiveUid));
}

CT_VOID CCIslandEvent::DisposeFightOver(CT_CHAR* pBuf, CT_INT32 iLen)
{
	MSC_Island_FightEvent *pFight = (MSC_Island_FightEvent *)pBuf;

	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pFight->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeFightOver connot find user,uid:%d", pFight->dwUserID);
		return;
	}

	if (WINER_LIFT == pFight->iStatus)
	{
		// 更新内存
		Point point = {0};
		G_PointEvent gpe = {0};
		pUser->m_userIsland.GetPoint(point);
		point.iStatus = ISLAND_EVENT_COMPLETE;

		if (!G_GetPointEvent(pFight->iEventID, &gpe))
		{
			CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeFightOver G_GetPointEvent error.eventid:%d", pFight->iEventID);
			return ;
		}
		point.iEventChest = gpe.iReward;

		pUser->m_userIsland.SetPoint(point);
		CT_INT32 iFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFight->dwUserID);
		pUser->m_userIsland.UpdateTeamScore(iFTID);

		// 发送客户端
		CMD_G_EventComplete_Suc gecs = {0};
		gecs.iChestID = gpe.iReward;
		gecs.iEventID = pFight->iEventID;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_FIGHT_EVENT_SUC, (CT_CHAR*)&gecs, sizeof(gecs));
	}
	else
	{
		// 发送客户端
		CMD_G_Fight_Fail gff = {0};
		gff.iEventID = pFight->iEventID;
		gff.enFail = EFF_FIGHT_Fail;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_FIGHT_EVENT_FAIL, (CT_CHAR*)&gff, sizeof(gff));
	}
}

CT_VOID CCIslandEvent::DisposeOutLineGiveFail(CT_CHAR *pBuf, CT_INT32 iLen)
{
	MSC_DB_IslandGiveTimes* pGiveTimes = (MSC_DB_IslandGiveTimes*)pBuf;

	CCUser* pUser;
	DragonPlayer Dplayer = {0};
	if (!CCUserManager::getSingleton().Find(pGiveTimes->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCIslandEvent::DisposeOutLineGiveFail connot find user,uid:%d", pGiveTimes->dwUserID);
		return;
	}

	if (pGiveTimes->iGetTimes >= ISLAND_MAX_GETTIMES)
	{
		// 赠送失败，发送客户端
		CMD_GiveTimes_Fail gtf;
		gtf.enfail = (ENUM_GiveTimesFail)EGT_GETTIMES_LESS;
		pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GIVE_TIMES_FAIL, (CT_CHAR*)&gtf, sizeof(gtf));
		return;
	}

	// 更新赠送的人
	pUser->m_userIsland.InsertGiveUserID(pGiveTimes->dwFUID);

	// 发送客户端
	pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_GIVE_TIMES_SUC);

	// 同步uid到数据库
	MSC_DB_IslandGiveUid mGiveUid = {0};
	mGiveUid.dwUserID = pGiveTimes->dwUserID;
	mGiveUid.aGiveUid = pGiveTimes->dwFUID;
	CT_DWORD threadID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_DRAGONISLAND,MSC_SUB_DB_ISLAND_GIVEUID,(CT_CHAR*)&mGiveUid,sizeof(mGiveUid));
}

CT_VOID CCIslandEvent::DisposeOpenIsland(CT_DWORD dwUserID)
{
	CT_INT32 iFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(dwUserID);

	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(dwUserID, &pUser))
	{
		return;
	}

	pUser->m_userIsland.OpenFunction(iFTID);
}

CT_VOID CCIslandEvent::DisposeTaskComplete(CT_CHAR* pBuf,CT_INT32 iLen)
{
	MSC_IslandTask* pTask = (MSC_IslandTask*)pBuf;
	CT_INT32 iFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pTask->dwUserID);

	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pTask->dwUserID, &pUser))
	{
		return;
	}

	pUser->m_userIsland.CheckAndDoTask(pTask->iTaskID, iFTID);
}