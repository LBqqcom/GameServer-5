#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CActiveEvent.h"
#include "CActive.h"
#include "CBroadcastScriptSetting.h"


#define ACTIVE_REWARD_ITEM_TYPE			4   //活动奖励道具类型

CCActiveEvent::CCActiveEvent(CCGameMesManager *pMesManager,CT_WORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_REWARD_REQUEST);
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_TURNTABLE_DRAW);
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_CDKEY_REWARD);
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_PRESENT);
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_DRAGONBALL);
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_DRAGONBALL_WISHING);
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_MONTH_CARD_RENEW);
	RegistMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_MONTH_CARD_REWARD);

	RegistMessage(MSC_MAIN_ACTIVE_EVENT_DATA,MSC_SUB_CDKEY_GET);
	RegistMessage(MSC_MAIN_ACTIVE_EVENT_DATA,MSC_SUB_PRESEND_ADD);
	
	if (m_dwThread == GAME_THREAD_CITY)
	{
		RegistMessage(MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_GET);
		RegistMessage(MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_DSC_GET);
		RegistMessage(MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_TURNTABLE_REL);
		RegistMessage(MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_REWARD_REL);
		RegistMessage(MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_REWARD_COMPLETE);
	}
		
	//定时器注册
	G_ValueRange vr = {0};
	if(G_GetValueRange(VALUE_TYPE_ACTIVE_ONLINE_DUR,  &vr))
	{
		if (vr.fMaxVal < 1)
		{
			CCLogMeg::getSingleton().WriteError("没有发现在全局配置文件中发现 VALUE_TYPE_ACTIVE_ONLINE_DUR = %i",0);
			abort();
		}
		RegistTimer(IDI_ACTIVE_ONLINE_DUR);
		SetTimer(IDI_ACTIVE_ONLINE_DUR, (CT_INT32)(vr.fMaxVal*1000));
	}

	if (m_dwThread == GAME_THREAD_CITY)
	{
		for(CT_INT32 i=0;i<TIME_ACTIVE_MAXNUM;++i)
		{
			RegistClock(IDI_TIMEACTIVE_OPEN+i);
		}
		for(CT_INT32 i=0;i<TIME_ACTIVE_TYPE_MAXNUM;++i)
		{
			RegistTimer(IDI_TIMEACTIVE_KEEP+i);
		}
	}
}

CCActiveEvent::~CCActiveEvent()
{

}

CT_VOID CCActiveEvent::OnTime(CT_DWORD timeID)
{
	if (m_dwThread != GAME_THREAD_CITY)
	{
		return;
	}
	if(timeID == IDI_ACTIVE_ONLINE_DUR)
	{
		CCUserManager::getSingleton().UpdateActiveOnline();
	}

	if((IDI_TIMEACTIVE_KEEP <= timeID) && (IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_TYPE_MAXNUM > timeID))	//活动到期
	{
		//关闭活动
		KillTimer(timeID);
		CT_INT32 iActiveType = 0;
		if (timeID == IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_YEAR_DINNER)
		{
			iActiveType = ACTIVE_YEAR_DINNER;
		}
		else if (timeID == IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_RED_PACKET)
		{
			iActiveType = ACTIVE_RED_PACKET;
		}
		else if (timeID == IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_KUA_NIAN)
		{
			iActiveType = ACTIVE_KUA_NIAN;
		}
		else if(timeID == IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_CS_FIGHT)
		{
			iActiveType = ACTIVE_CS_FIGHT;
			SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_ACTIVE_FIGHTING,0,0);
		}
		else if(timeID == IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_CS_SUPPORT)
		{
			iActiveType = ACTIVE_CS_SUPPORT;
		}

		CT_INT32 iLastAEID = 0;
		CT_INT64 lTime=NextActiveTime(iActiveType, iLastAEID);
		CCActive::getSingleton().SetTimeActiveStatus(iActiveType, iLastAEID, lTime, 0);
		CMD_G_ActiveEventRewardUpdate activeUpdate={0};
		activeUpdate.iActiveID = iLastAEID;
		activeUpdate.iActiveQuanity = 0;
		activeUpdate.iActiveStatus = ACTIVE_COMPLETED;
		activeUpdate.iTime = (lTime==-1?-1:lTime - time(0));
		activeUpdate.iTimeStatus = 0;
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(activeUpdate));

		CCLogMeg::getSingleton().WriteError("*****Active end time:%d",timeID);
	}
}


//用户时钟处理
CT_VOID CCActiveEvent::OnClock(CT_DWORD timeID)
{
	if(IDI_TIMEACTIVE_OPEN <= timeID && timeID < IDI_TIMEACTIVE_OPEN+TIME_ACTIVE_MAXNUM && m_dwThread == GAME_THREAD_CITY)
	{
		KillClock(timeID);
		// 活动开始
		std::map<CT_INT32,CT_INT32>::iterator it=m_TimeActiveMap.find(timeID);
		if(it != m_TimeActiveMap.end())
		{
			G_ActiveEventData aet={0};
			if(G_GetActiveEvent((*it).second,&aet))
			{
				CT_INT32 iTimeID = 0;
				if (aet.iType == ACTIVE_YEAR_DINNER)
				{
					iTimeID = IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_YEAR_DINNER;
				}
				else if (aet.iType == ACTIVE_RED_PACKET)
				{
					iTimeID = IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_RED_PACKET;
				}
				else if (aet.iType == ACTIVE_KUA_NIAN)
				{
					iTimeID = IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_KUA_NIAN;
				}
				else if(aet.iType == ACTIVE_CS_FIGHT)
				{
					iTimeID = IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_CS_FIGHT;
					SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_ACTIVE_BEGIN,0,0);
				}
				else if(aet.iType == ACTIVE_CS_SUPPORT)
				{
					iTimeID =IDI_TIMEACTIVE_KEEP+TIME_ACTIVE_CS_SUPPORT;
				}
				
				SetTimer(iTimeID,aet.iProperty1*1000);
				CCActive::getSingleton().SetTimeActiveStatus(aet.iType, it->second, time(0)+aet.iProperty1, 1);
				//清除用户内存数据
				CCUserManager::getSingleton().RefreshActiveStatus(it->second, ACTIVE_COMPLETED);
				CCLogMeg::getSingleton().WriteError("****CCActiveEvent::OnClock IDI_TIMEACTIVE_KEEP iTimeID:%d :%d %d\n",iTimeID,aet.iProperty1,aet.iProperty2);
			
				CMD_G_ActiveEventRewardUpdate activeUpdate={0};
				activeUpdate.iActiveID = it->second;
				activeUpdate.iActiveQuanity = 0;
				activeUpdate.iActiveStatus = ACTIVE_COMPLETED;
				activeUpdate.iTime = aet.iProperty1;
				activeUpdate.iTimeStatus = 1;
				CCUserManager::getSingleton().SendUserMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(activeUpdate));
				m_TimeActiveMap.erase(it);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCActiveEvent::OnClock G_GetActiveEvent %d",(*it).second);
			}
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCActiveEvent::OnClock it == m_TimeActiveMap.end()");
		}
	}
}

CT_VOID CCActiveEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MAIN_G_ACTIVE_EVENT_CITY:
		{
			if (subID==SUB_G_ACTIVE_REWARD_REQUEST)
			{
				DisposeActiveReward(pBuf,len);
			}
			else if (subID==SUB_G_TURNTABLE_DRAW)
			{
				DisposeTurntableDraw(pBuf,len);
			}
			else if (SUB_G_CDKEY_REWARD==subID)
			{
				DisposeCDKEYReward(pBuf,len);
			}
			else if (SUB_G_ACTIVE_PRESENT == subID)
			{
				DisposePresent(pBuf,len);
			}
			else if (SUB_G_DRAGONBALL == subID)
			{
				DisposeDragonBall(pBuf,len);
			}
			else if (SUB_G_DRAGONBALL_WISHING == subID)
			{
				DisposeDragonBallWish(pBuf,len);
			}
			else if(SUB_G_MONTH_CARD_RENEW == subID)
			{
				DisposeMonthCardRenew(pBuf,len);
			}
			else if(SUB_G_MONTH_CARD_REWARD == subID)
			{
				DisposeMonthCardReward(pBuf,len);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCActiveEvent::OnUserEvent : subID=%i", subID);
				return;
			}
			break;
		}
	case MSC_MAIN_ACTIVE_EVENT_DATA:
		{
			if (subID==MSC_SUB_CDKEY_GET)
			{
				DisposeCDKEYGet(pBuf,len);
			}else if (MSC_SUB_ACTIVE_GET == subID)
			{
				DisposeActiveGet(pBuf, len);
			}else if (MSC_SUB_ACTIVE_DSC_GET == subID)
			{
				DisposeActiveDec(pBuf, len);
			}else if (MSC_SUB_PRESEND_ADD == subID)
			{
				DisposePresendAdd(pBuf,len);
			}else if (MSC_SUB_TURNTABLE_REL == subID)
			{
				DisposeTurntableRelation(pBuf, len);
			}else if (MSC_SUB_ACTIVE_REWARD_REL == subID)
			{
				DisposeRewardRelation(pBuf, len);
			}else if (MSC_SUB_ACTIVE_REWARD_COMPLETE == subID)
			{
				DisposeRewardComplete();
			}

			break;
	
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCActiveEvent::OnUserEvent : mainID=%i", mainID);
			break;
		}
	}
}

CT_VOID CCActiveEvent::DisposeActiveReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if (sizeof(CMD_G_ActiveEventRewardRequest)!=len)
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeActiveReward : sizeof(CMD_G_ActiveRewardRequest)!=len %d", len);
		return;
	}
	CMD_G_ActiveEventRewardRequest		*ARR	= (CMD_G_ActiveEventRewardRequest*)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(ARR->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeActiveReward,can't find user %d",ARR->dwUserID);
		return;
	}
	if (!pUser->m_userActive.CheckActiveComplete(ARR->iActiveID))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeActiveReward,the active %d user %d can't be receive",ARR->iActiveID,ARR->dwUserID);
		return;
	}
	CT_INT32 iDiamod =0;
	CT_INT32 iActiveType = CCActive::getSingleton().GetActiveType(ARR->iActiveID);
	if (iActiveType == ACTIVE_LIMITATION || ACTIVE_FESTIVAL_PACKET == iActiveType)
	{
		MasterVariableData mvd2 = {0};
		G_ActiveEventData activeEventData = {0};
		pUser->m_userBase.GetMasterData(mvd2);
		G_GetActiveEvent(ARR->iActiveID, &activeEventData);
		iDiamod = (CT_INT32)activeEventData.iProperty2;
		if (mvd2.stRes.iDiamod +  iDiamod < 0)
		{
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_NO_DIAMOND);
			return;
		}
	}
	G_ActiveEventReward			AER = {0};
	MasterVariableData mvd = {0};
	CMD_G_ActiveEventReward		activeReward = {0};
	if (!CCActive::getSingleton().GetReward(ARR->iActiveID,&AER))
	{
		if (!G_GetActiveEventReward(ARR->iActiveID,&AER))
		{
			CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeActiveReward,the active %d reward config error",ARR->iActiveID);
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
			return;
		}
	}

	if (iActiveType == ACTIVE_YEAR_DINNER || iActiveType == ACTIVE_RED_PACKET)
	{
		CT_INT32 iTimeAEID = 0;
		CT_INT32 iStatus = 0;
		CT_INT64 iTime = 0;
		CCActive::getSingleton().GetTimeActiveStatus(iActiveType, iTimeAEID, iTime, iStatus);
		if (ARR->iActiveID != iTimeAEID || 1 != iStatus)
		{
			CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeActiveReward,the timeactive %d invalid",ARR->iActiveID);
			return;
		}
	}

	if(iActiveType == ACTIVE_YEAR_DINNER)
	{
		CT_INT32 iQuantity=pUser->m_userActive.GetActiveQuanity(ARR->iActiveID);
		YearDinnarReward(iQuantity,AER);
	}

	CT_INT32	iItemType = (AER.iPropID1>0?1:0) + (AER.iPropID2>0?1:0) + (AER.iPropID3>0?1:0) + (AER.iPropID4>0?1:0);
	//CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	//添加道具奖励
	/*if (iItemType==1)
	{
	CT_INT32 iItemID = (AER.iPropID1>0?AER.iPropID1:0) + (AER.iPropID2>0?AER.iPropID2:0) + (AER.iPropID3>0?AER.iPropID3:0) + (AER.iPropID4>0?AER.iPropID4:0);
	CT_INT32 iItemNum = (AER.iPropNum1>0?AER.iPropNum1:0) + (AER.iPropNum2>0?AER.iPropNum2:0) + (AER.iPropNum3>0?AER.iPropNum3:0) + (AER.iPropNum4>0?AER.iPropNum4:0);
	if (pUser->m_userItem.AutoAddItemData(threadid,iItemID,iItemNum)>0)
	{
	CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeActiveReward,Bag full");
	pUser->CloseNet();
	return;
	}
	}*/
	if (iItemType>0)
	{
		CT_INT32 iItemID[ACTIVE_REWARD_ITEM_TYPE]={0},iItemCounts[ACTIVE_REWARD_ITEM_TYPE]={0};
		CT_INT32 i=0;
		if (AER.iPropID1>0)
		{
			iItemID[i] = AER.iPropID1;
			iItemCounts[i] = AER.iPropNum1;
			i++;
		}
		if (AER.iPropID2>0)
		{
			iItemID[i] = AER.iPropID2;
			iItemCounts[i] = AER.iPropNum2;
			i++;
		}
		if (AER.iPropID3>0)
		{
			iItemID[i] = AER.iPropID3;
			iItemCounts[i] = AER.iPropNum3;
			i++;
		}
		if (AER.iPropID4>0)
		{
			iItemID[i] = AER.iPropID4;
			iItemCounts[i] = AER.iPropNum4;
			i++;
		}
		//if (!pUser->m_userItem.AutoAddItemData(threadid,iItemID,iItemCounts,iItemType))
		if (LOOT_SUC != pUser->LootItems(SAVETAG_ACTIVE_EVENT_REWARD, iItemID, iItemCounts, i, ITEM_OP_MODE_CKEXE))
		{
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_BAG_FULL);
			return;
		}
	}

	AER.iDiamod += iDiamod;
	mvd.stRes.iGold = AER.iGold;
	mvd.stRes.iFood = AER.iFood;
	//	mvd.stCivili.iCurrentExp = AER.iCivil;
	mvd.stRes.iStamina = AER.iStamina;
	mvd.stGrade.iCurrentExp = AER.iXP;
	mvd.stRank.iCurrentExp = AER.iPrestige;
	mvd.stRes.iTalentNum = AER.iTalent;
	mvd.stRes.iDiamod = AER.iDiamod;

	pUser->m_userBase.AddMastData(mvd,SAVETAG_ACTIVE_EVENT_REWARD);

	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);


	activeReward.iAEID = ARR->iActiveID;
//	activeReward.iCivil = mvd.stCivili.iCurrentExp;
	activeReward.iStamina = mvd.stRes.iStamina;
	activeReward.iFood = mvd.stRes.iFood;
	activeReward.iGold = mvd.stRes.iGold;
	activeReward.iPrestige = mvd.stRank.iCurrentExp;
	activeReward.iTalent = mvd.stRes.iTalentNum;
	activeReward.iXP = mvd.stGrade.iCurrentExp;
	activeReward.iDiamod = mvd.stRes.iDiamod;

	//发送客户端
	pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_REWARD,(CT_CHAR*)&activeReward,sizeof(activeReward));

	if (CCActive::getSingleton().GetActiveType(ARR->iActiveID) == ACTIVE_RECHARGE_FIRST)
	{
		RollBroadcast rb={0};
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		rb.SetValue(pUser->GetUserID(), masterData.cNickName);
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{8}<6:588>{9}<10:1>", pUser->GetUserID(), masterData.cNickName);
		G_SendRollBroadecast(123,1,1,(CT_CHAR*)&rb,sizeof(rb));
	}

	if (iActiveType == ACTIVE_LIMITATION)
	{
		RollBroadcast rb[2]={0};
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
		rb[1].SetValue(iDiamod*-1,"");
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{10}<6:%d>{11}<10:2>", pUser->GetUserID(), masterData.cNickName,iDiamod*-1);
		G_SendRollBroadecast(124,1,1,(CT_CHAR*)rb,sizeof(rb));
	}
	else if(iActiveType == ACTIVE_FESTIVAL_PACKET)
	{
		RollBroadcast rb[2]={0};
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
		rb[1].SetValue(iDiamod*-1,"");
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{10}<6:%d>{98}<10:3>", pUser->GetUserID(), masterData.cNickName,iDiamod*-1);
		G_SendRollBroadecast(125,1,1,(CT_CHAR*)rb,sizeof(rb));
	}

	pUser->m_userActive.DisposNextActive(ARR->iActiveID);
	//修改数据库信息
	
	if (iActiveType == ACTIVE_LIMITATION || iActiveType == ACTIVE_YEAR_DINNER || iActiveType == ACTIVE_RED_PACKET || iActiveType == ACTIVE_FESTIVAL_PACKET)
	{
		CCActive::getSingleton().ActiveProcess(iActiveType, 1, pUser, ARR->iActiveID);
	}
	else
	{
		MSC_DB_ActiveEventStatus	DBaes = {0};
		DBaes.dwUserID = pUser->GetUserID();
		DBaes.iAID = ARR->iActiveID;
		pUser->m_userActive.ActiveRewardGet(ARR->iActiveID);
		CMD_G_ActiveEventRewardUpdate activeUpdate = {0};
		activeUpdate.iActiveID = ARR->iActiveID;
		activeUpdate.iActiveStatus = ACTIVE_RECEIVED;
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));
		DBaes.iStatus = ACTIVE_RECEIVED;
		CT_DWORD threadDBID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&DBaes,sizeof(MSC_DB_ActiveEventStatus));
	}
	
	//修改日志数据库信息
	MSC_LOGDB_ActiveEventReward		logdbReward = {0};
	logdbReward.dwUserID = pUser->GetUserID();
	logdbReward.iGrade   = mvd.stGrade.iLevel;
	logdbReward.activeReward = AER;

	CT_DWORD LogDBthreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBthreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ACTIVEEVENTREWARD,(CT_CHAR*)&logdbReward,sizeof(logdbReward));
}

CT_VOID CCActiveEvent::DisposeTurntableDraw(CT_CHAR* pBuf,CT_DWORD len)
{
	if (sizeof(CMD_G_TurntableDrawRequest)!=len)
	{
		CCLogMeg::getSingleton().WriteError("DisposeTurntableDraw : sizeof(CMD_G_TurntableDrawRequest)!=len %d", len);
		return;
	}
	CMD_G_TurntableDrawRequest		*ttd	= (CMD_G_TurntableDrawRequest*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(ttd->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeTurntableDraw,can't find user %d",ttd->dwUserID);
		return;
	}
	G_TurntableBase ttb={0};
	if (!G_GetTurntableBase(ttd->iTurntableID,&ttb))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("DisposeTurntableDraw,the TurntableBase %d data error",ttd->iTurntableID);
		return;
	}
	CT_INT32 iDiamodCost = 0;
	CT_INT32 iGoldCost = 0;
	MasterVariableData mvd = {0};
	// 取消免费次数, 改为最高次数.
	CT_INT32 iTimes=pUser->m_userActive.GetTurntableCount(ttd->iTurntableID) + 1; // - ttb.iFree + 1;
	if (iTimes > ttb.iCountMax + pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_TURNTABLE))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_TURNTABLE_NO_TIMES);
		return;
	}
	if (iTimes > 0)
	{
		pUser->m_userBase.GetMasterData(mvd);
//		iDiamodCost = G_GetTurntableDraw(ttb.iDiamod, ttb.iDiamodCF, iTimes); //ttb.iDiamod + ttb.iDiamodCF * iTimes;
		iGoldCost	= G_GetTurntableDraw(ttb.iGold, ttb.iGoldCF, iTimes); //ttb.iGold + ttb.iGoldCF * iTimes;
		if (!pUser->m_userItem.AutoExpendItem(SAVETAG_ACTIVE_EVENT_REWARD, ttb.iItem, ttb.iItemNum*(-1), ITEM_OP_MODE_CHECK))
		{
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_TURNTABLE_NO_ITEM);
			return;
		}
		if (mvd.stRes.iGold +  iGoldCost < 0)
		{
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_TURNTABLE_NO_GOLD);
			return;
		}
	}

	CT_INT32 i=0;
	CT_INT32 iWeight=0;
	G_TurntableRelation ttr = {0};
//	G_TurntableReward	ttrd[TURNTABLE_ITEM_COUNT] = {0};
	TurntableItem tti = {0};
	FakeProBase fb={0};
	G_GetFakeProDta(pUser,SAVETAGEEX_TURNTABLE0+(ttd->iTurntableID-1),fb);
	G_GetTurntableRelation(ttd->iTurntableID, &ttr);
	CCActive::getSingleton().GetTurntableItem(ttd->iTurntableID, &tti);
	i=0;
	while(i<TURNTABLE_ITEM_COUNT)
	{
		tti.iWeight[i] += iWeight;

		if(tti.iFakeProID[i] != 0)
		{
			CT_INT32 iTemp=G_GetFakeProCount(tti.iFakeProID[i],fb);
			iWeight = (iTemp != 0?iTemp:tti.iWeight[i]);
		}
		else
			iWeight = tti.iWeight[i];
		++i;
	}
	G_ClearFakeProDat(pUser,SAVETAGEEX_TURNTABLE0+(ttd->iTurntableID-1),fb);
	if (iWeight == 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("DisposeTurntableDraw,the TurntableReward(ID:%d) error",ttd->iTurntableID);
		return;
	}
	CT_INT32  iRandNum = 0;
	CT_INT32  iPos = 0;	

	while(CT_TRUE)
	{
		iRandNum = rand()%iWeight + 1;
		iPos = GetPosByRandNum(tti.iWeight,0,i,iRandNum);
		if((g_iGlobeData[GLOBEDATA_TYPE_REDMI] > 0 && tti.gReward[iPos].iID == PHONE_RED_MI) || 
			(g_iGlobeData[GLOBEDATA_TYPE_MITHREE] > 0 && tti.gReward[iPos].iID == PHONE_MI_THREE))
		{
			continue;
		}
		break;
	}

	MSC_DB_GlobeData gd={0};
	if(tti.gReward[iPos].iID == PHONE_RED_MI)
	{
		g_iGlobeData[GLOBEDATA_TYPE_REDMI]++;
		gd.iType=GLOBEDATA_TYPE_REDMI;
		gd.iValue=g_iGlobeData[GLOBEDATA_TYPE_REDMI];
		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
	}
	else if(tti.gReward[iPos].iID == PHONE_MI_THREE)
	{
		g_iGlobeData[GLOBEDATA_TYPE_MITHREE]++;
		gd.iType=GLOBEDATA_TYPE_MITHREE;
		gd.iValue=g_iGlobeData[GLOBEDATA_TYPE_MITHREE];
		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
	}

	// 可以发奖励了. ttrd[iPos]
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	
	memset(&mvd,0,sizeof(mvd));
	CT_INT32 iCount = tti.gReward[iPos].iCount;
	if (tti.gReward[iPos].iType == 1)
	{
		G_ChangeResType(mvd, tti.gReward[iPos].iID, iCount);
	}
	else if (tti.gReward[iPos].iType == 2)
	{
		if (tti.gReward[iPos].iID > 0)
		{
			//pUser->m_userItem.AutoAddItemData(threadid, tti.gReward[iPos].iID, iCount, ITEM_OP_MODE_EXEC);
			pUser->LootItems(SAVETAG_TURN_AWARD, &(tti.gReward[iPos].iID), &iCount, 1, ITEM_OP_MODE_CKEXE);
		}
	}
	
	mvd.stRes.iGold += iGoldCost;
//	mvd.stRes.iDiamod += iDiamodCost;
	pUser->m_userItem.AutoExpendItem(SAVETAG_ACTIVE_EVENT_REWARD, ttb.iItem, ttb.iItemNum*(-1), ITEM_OP_MODE_EXEC);
	pUser->m_userBase.AddMastData(mvd,SAVETAG_ACTIVE_EVENT_REWARD);

	CMD_G_ActiveEventReward		activeReward = {0};
	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);
	activeReward.iAEID = iPos;
	activeReward.iStamina = mvd.stRes.iStamina;
	activeReward.iFood = mvd.stRes.iFood;
	activeReward.iGold = mvd.stRes.iGold;
	activeReward.iPrestige = mvd.stRank.iCurrentExp;
	activeReward.iTalent = mvd.stRes.iTalentNum;
	activeReward.iXP = mvd.stGrade.iCurrentExp;
	activeReward.iDiamod = mvd.stRes.iDiamod;

	//发送客户端
	pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_TURNTABLE_REWARD,(CT_CHAR*)&activeReward,sizeof(activeReward));
	

	//修改数据库信息
	pUser->m_userActive.AddTurntableCount(ttd->iTurntableID);
	MSC_DB_Turntable dbTurntable = {0};
	dbTurntable.dwUserID = pUser->GetUserID();
	dbTurntable.iTimes[ttd->iTurntableID-1] = 1;
	CT_DWORD threadDBID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_TURNTABLE_UPDATE,(CT_CHAR*)&dbTurntable,sizeof(MSC_DB_Turntable));

	if (tti.iRewardID[iPos] > TURNTABLE_BROADCAST)
	{
		CMD_G_TurntableBroadcast turntableBroadcast = {0};
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		memcpy(turntableBroadcast.cNickName, masterData.cNickName, NAME_SIZE);
		turntableBroadcast.iTurntableID = ttd->iTurntableID;
		turntableBroadcast.iType = tti.gReward[iPos].iType;
		turntableBroadcast.iID = tti.gReward[iPos].iID;
		turntableBroadcast.iNum = tti.gReward[iPos].iCount;
		CCUserManager::getSingleton().BroadCastMessage(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_TURNTABLE_BROADCAST, (CT_CHAR*)&turntableBroadcast, sizeof(CMD_G_TurntableBroadcast));

		//RollBroadcast rb[5]={0};
		//rb[0].SetValue(pUser->GetUserID(),masterData.cNickName);
		//rb[1].SetValue(ttd->iTurntableID,"");
		//rb[2].SetValue(tti.gReward[iPos].iType,"");
		//rb[3].SetValue(tti.gReward[iPos].iID,"");
		//rb[4].SetValue(tti.gReward[iPos].iCount,"");
		//G_SendRollBroadecast(130,4,1,(CT_CHAR*)rb,sizeof(rb));
	}

	//修改日志数据库信息
	MSC_LOGDB_ActiveEventReward		logdbReward = {0};
	G_ActiveEventReward AER = {0};
	AER.iAEID		= ttd->iTurntableID;				//转盘id
	if (tti.gReward[iPos].iType == 1)
	{
		switch(tti.gReward[iPos].iID)
		{
		case VALUE_TYPE_GOLD:
			AER.iGold = tti.gReward[iPos].iCount;
			break;
		case VALUE_TYPE_DIAMOND:
			AER.iDiamod = tti.gReward[iPos].iCount;
			break;
		case VALUE_TYPE_FOOD:
			AER.iFood = tti.gReward[iPos].iCount;
			break;
		case VALUE_TYPE_STAMINA:
			AER.iStamina = tti.gReward[iPos].iCount;
			break;
		case VALUE_TYPE_TALENTNUM:
			AER.iTalent = tti.gReward[iPos].iCount;
			break;
		case VALUE_TYPE_MASTERXP:
			AER.iXP = tti.gReward[iPos].iCount;
			break;
		case VALUE_TYPE_RANKXP:
			AER.iPrestige = tti.gReward[iPos].iCount;
			break;
		}
	}
	else if (tti.gReward[iPos].iType == 2)
	{
		AER.iPropID1	= tti.gReward[iPos].iID;					//道具id1
		AER.iPropNum1	= tti.gReward[iPos].iCount;				//数量1
	}
	logdbReward.dwUserID = pUser->GetUserID();
	logdbReward.iGrade   = mvd.stGrade.iLevel;
	logdbReward.activeReward = AER;

	CT_DWORD LogDBthreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBthreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ACTIVEEVENTREWARD,(CT_CHAR*)&logdbReward,sizeof(logdbReward));

}

CT_INT32 CCActiveEvent::GetPosByRandNum(CT_INT32 *iWeight,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand)
{
	if (begin>=end)
	{
		return begin;
	}
	CT_INT32 mid = (begin+end)/2;
	if (iWeight[mid]>=iRand)
	{
		return GetPosByRandNum(iWeight,begin,mid,iRand);
	}
	else
	{
		return GetPosByRandNum(iWeight,mid+1,end,iRand);
	}
}

CT_VOID CCActiveEvent::DisposeCDKEYReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_CDKEYReward))
	{
		CCLogMeg::getSingleton().WriteError("DisposeCDKEYReward: len != sizeof(CMD_G_CDKEYReward)");
		return;
	}
	CMD_G_CDKEYReward *pCDKEYReward=(CMD_G_CDKEYReward*)pBuf;
	MSC_DB_CDKEY dbCDKEY={0};
	memcpy(dbCDKEY.cCDKEY, pCDKEYReward->cCDKEY, 32);
	dbCDKEY.dwUID = pCDKEYReward->dwUserID;

	CT_DWORD threadDBID = GAME_THREAD_DATABASE + pCDKEYReward->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_CDKEY_GET,(CT_CHAR*)&dbCDKEY,sizeof(MSC_DB_CDKEY));
}

CT_VOID CCActiveEvent::DisposeCDKEYGet(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_CDKEY))
	{
		CCLogMeg::getSingleton().WriteError("DisposeCDKEYGet: len != sizeof(MSC_DB_CDKEY)");
		return;
	}
	MSC_DB_CDKEY *pCDKEY=(MSC_DB_CDKEY*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pCDKEY->dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeCDKEYGet,can't find user %d",pCDKEY->dwUID);
		return;
	}

	if (pCDKEY->iType < 1)
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_CDKEY_INVALID);
		return;
	}
// 	if (pCDKEY->dwCID > 0)
// 	{
// 		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_CDKEY_HASRECV);
// 	}

	G_ActiveEventReward			AER = {0};
	if (!CCActive::getSingleton().GetReward(pCDKEY->iType + 60000,&AER))
	{
		if (!G_GetActiveEventReward(pCDKEY->iType + 60000,&AER))
		{
			CCLogMeg::getSingleton().WriteError("DisposeCDKEYGet,the type %d reward config error",pCDKEY->iType);
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
			return;
		}
	}
	CT_INT32	iItemType = (AER.iPropID1>0?1:0) + (AER.iPropID2>0?1:0) + (AER.iPropID3>0?1:0) + (AER.iPropID4>0?1:0);
	//CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;

	if (iItemType>0)
	{
		CT_INT32 iItemID[ACTIVE_REWARD_ITEM_TYPE]={0},iItemCounts[ACTIVE_REWARD_ITEM_TYPE]={0};
		CT_INT32 i=0;
		if (AER.iPropID1>0)
		{
			iItemID[i] = AER.iPropID1;
			iItemCounts[i] = AER.iPropNum1;
			i++;
		}
		if (AER.iPropID2>0)
		{
			iItemID[i] = AER.iPropID2;
			iItemCounts[i] = AER.iPropNum2;
			i++;
		}
		if (AER.iPropID3>0)
		{
			iItemID[i] = AER.iPropID3;
			iItemCounts[i] = AER.iPropNum3;
			i++;
		}
		if (AER.iPropID4>0)
		{
			iItemID[i] = AER.iPropID4;
			iItemCounts[i] = AER.iPropNum4;
			i++;
		}
//		if (!pUser->m_userItem.AutoAddItemData(threadid,iItemID,iItemCounts,iItemType))
		if (LOOT_SUC != pUser->LootItems(SAVETAG_CDKEY_AWARD, iItemID, iItemCounts, i, ITEM_OP_MODE_CKEXE))
		{
			pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_BAG_FULL);
			return;
		}
	}
	CMD_G_ActiveEventReward		activeReward = {0};
	MasterVariableData mvd = {0};
	mvd.stRes.iGold = AER.iGold;
	mvd.stRes.iFood = AER.iFood;
//	mvd.stCivili.iCurrentExp = AER.iCivil;
	mvd.stRes.iStamina = AER.iStamina;
	mvd.stGrade.iCurrentExp = AER.iXP;
	mvd.stRank.iCurrentExp = AER.iPrestige;
	mvd.stRes.iTalentNum = AER.iTalent;
	mvd.stRes.iDiamod = AER.iDiamod;
	
	pUser->m_userBase.AddMastData(mvd,SAVETAG_ACTIVE_EVENT_REWARD);

	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);

	activeReward.iAEID = 2201;//客户需要返回活动ID，此ID不能变动
//	activeReward.iCivil = mvd.stCivili.iCurrentExp;
	activeReward.iStamina = mvd.stRes.iStamina;
	activeReward.iFood = mvd.stRes.iFood;
	activeReward.iGold = mvd.stRes.iGold;
	activeReward.iPrestige = mvd.stRank.iCurrentExp;
	activeReward.iTalent = mvd.stRes.iTalentNum;
	activeReward.iXP = mvd.stGrade.iCurrentExp;
	activeReward.iDiamod = mvd.stRes.iDiamod;

	//发送客户端
	pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_REWARD,(CT_CHAR*)&activeReward,sizeof(activeReward));


	//修改数据库信息

	CT_DWORD threadDBID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_CDKEY_UPDATE,pBuf,sizeof(MSC_DB_CDKEY));
	//修改日志数据库信息
	MSC_LOGDB_ActiveEventReward		logdbReward = {0};
	logdbReward.dwUserID = pUser->GetUserID();
	logdbReward.iGrade   = mvd.stGrade.iLevel;
	logdbReward.activeReward = AER;

	CT_DWORD LogDBthreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBthreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ACTIVEEVENTREWARD,(CT_CHAR*)&logdbReward,sizeof(logdbReward));

}

CT_VOID CCActiveEvent::DisposePresendAdd(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_Present))
	{
		CCLogMeg::getSingleton().WriteError("DisposePresendAdd: len != sizeof(MSC_DB_Present)");
		return;
	}
	MSC_DB_Present *pPresent=(MSC_DB_Present*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pPresent->dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposePresendAdd,can't find user %d",pPresent->dwUID);
		return;
	}

	pUser->m_userActive.AddPresent(*pPresent);
}

CT_VOID CCActiveEvent::DisposePresent(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Present))
	{
		CCLogMeg::getSingleton().WriteError("DisposePresent: len != sizeof(CMD_G_Present)");
		return;
	}
	CMD_G_Present *pPresent=(CMD_G_Present*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pPresent->dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposePresent,can't find user %d",pPresent->dwUID);
		return;
	}

	if (!pUser->m_userActive.CheckPresent(pPresent->iIndex))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_PRESENT_ERROR);
		return;
	}

	CMD_G_Present present = {0};
	pUser->m_userActive.GetPresent(pPresent->iIndex, present);

//	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
//	if (0 == pUser->m_userItem.AutoAddItemData(threadid,pPresent->iP_ID,pPresent->iP_Count))
//	if (pUser->LootItems(&(pPresent->iP_ID), &(pPresent->iP_Count), 1, ITEM_OP_MODE_CKEXE))
	if (LOOT_SUC == pUser->LootItems(SAVETAG_PRESENT_AWARD, &(present.iP_ID), &(present.iP_Count), 1, ITEM_OP_MODE_CKEXE))
	{
		//更新DB
		CT_DWORD threadid2 = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;;
		G_SendThreadInfo(threadid2,MSC_MAIN_DB_LOGIN,MSC_SUB_DB_PRESENT,(CT_CHAR*)&present,sizeof(MSC_DB_Present));
		pUser->m_userActive.DeletePresent(pPresent->iIndex);
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_PRESENT_SUC, (CT_CHAR*)&(pPresent->iIndex), sizeof(CT_INT32));
	}
	else
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_BAG_FULL);
	}
}

//得到活动
CT_VOID CCActiveEvent::DisposeActiveGet(CT_CHAR* pBuf,CT_DWORD len)
{
	//获取删除的活动，删除用户活动状态表对应的活动
	MSC_DB_ActiveEventChange    activeDelte = {0};
	MSC_DB_ActiveEventChange    activeAdd = {0};
	std::vector<CT_INT32>		v_active;

	CT_INT32 num = len/sizeof(CT_INT32);
	for(CT_INT32 i=0; i<num; ++i )
	{
		CT_INT32 iActive = 0;
		memcpy(&iActive, pBuf+i*sizeof(CT_INT32), sizeof(CT_INT32));
		v_active.push_back(iActive);
	}

	CCActive::getSingleton().UpdateActive(v_active,activeDelte, activeAdd);
	CCActive::getSingleton().RefTurntable();
	CCUserManager::getSingleton().ActiveDeletePush(activeDelte);
	CCUserManager::getSingleton().ActiveAddPush(activeAdd);

	m_TimeActiveMap.clear();
	DisposeActiveOpen(ACTIVE_YEAR_DINNER);
	DisposeActiveOpen(ACTIVE_RED_PACKET);
	DisposeActiveOpen(ACTIVE_KUA_NIAN);
	DisposeActiveOpen(ACTIVE_CS_FIGHT);
	DisposeActiveOpen(ACTIVE_CS_SUPPORT);

	G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_DB_ACTIVE_DATA, MSC_SUB_ACTIVE_DB_SYS,(CT_CHAR*)&activeDelte, sizeof(activeDelte));
}

//得到活动描述
CT_VOID CCActiveEvent::DisposeActiveDec(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != 0)
	{
		CCActive::getSingleton().InsterActiveDsc(pBuf, len);
	}
}

CT_VOID CCActiveEvent::DisposeTurntableRelation(CT_CHAR* pBuf,CT_DWORD len)
{
	MSC_DB_TurntableRelation ttr[TURNTABLE_ITEM_COUNT*3] = {0};
	if (len == sizeof(ttr))
	{
		memcpy(ttr, pBuf, len);
		for(CT_INT32 i=0; i<TURNTABLE_ITEM_COUNT*3 && ttr[i].iTurntableID > 0; i++)
		{
			CCActive::getSingleton().SetTurntableItem(ttr[i]);
		}
	}

	CCUserManager::getSingleton().RefreshTurntable();
}

CT_VOID CCActiveEvent::DisposeRewardRelation(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(G_ActiveEventReward))
	{
		CCLogMeg::getSingleton().WriteError("DisposeRewardRelation: len != sizeof(CMD_G_ActiveEventRewardEx)");
		return;
	}
	G_ActiveEventReward *pReward=(G_ActiveEventReward*)pBuf;

	CCActive::getSingleton().InsertReward(*pReward);
}

CT_VOID CCActiveEvent::DisposeRewardComplete()
{
	CT_CHAR buf[MAX_MES_MAX_BUFF_LEN] = {0};
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	CT_INT32 bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCActive::getSingleton().GetReward(buf,bufLen))
	{
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_ENTER_ENV, SUB_G_ACTIVE_REWARD_UPDATE, buf, bufLen);
	}
}

CT_INT64 CCActiveEvent::NextActiveTime(CT_INT32 iAEType, CT_INT32 &iLastAEID)
{
	CT_INT32 AcviteList[100] = {0};
	CCActive::getSingleton().GetActiveByType(iAEType,AcviteList);
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
					CT_INT32 iMin=iProperty2%3600/60;
					CT_INT32 iSec=iProperty2%60;

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
				CCLogMeg::getSingleton().WriteError("CCActiveEvent::NextActiveTime G_GetActiveEvent %d",AcviteList[i]);
			}
		}

		if(lMinTime != 0)
		{
			CT_INT32 iTimeDay=(CT_INT32)(lMinTime/(3600*24));
			CT_INT32 iHour=lMinTime%(3600*24)/3600;
			CT_INT32 iMin=lMinTime%3600/60;
			CT_INT32 iSec=lMinTime%60;
			lMinTime=t+(iTimeDay-ptm->tm_wday)*24*3600+(iHour-ptm->tm_hour)*3600+(iMin-ptm->tm_min)*60+(iSec-ptm->tm_sec);
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
			CT_INT32 iMin=lMinTime%3600/60;
			CT_INT32 iSec=lMinTime%60;
			lMinTime=t+(7-(ptm->tm_wday-iTimeDay))*24*3600+(iHour-ptm->tm_hour)*3600+(iMin-ptm->tm_min)*60+(iSec-ptm->tm_sec);
			return lMinTime;
		}
	}
	return -1;
}

//年夜饭活动奖励
CT_VOID CCActiveEvent::YearDinnarReward(CT_INT32 iQuantity,G_ActiveEventReward &AER)
{
	CT_INT32 iAEID = AER.iAEID;
	CT_INT32 iItemID=0,iItemCounts=0;
	if(iQuantity == 0)
	{
		iItemID = AER.iPropID1;
		iItemCounts = AER.iPropNum1;
	}
	else if(iQuantity == 1)
	{
		iItemID = AER.iPropID2;
		iItemCounts = AER.iPropNum2;
	}
	else if(iQuantity == 2)
	{
		iItemID = AER.iPropID3;
		iItemCounts = AER.iPropNum3;
	}
	else if(iQuantity == 3)
	{
		iItemID = AER.iPropID4;
		iItemCounts = AER.iPropNum4;
	}
	memset(&AER, 0, sizeof(G_ActiveEventReward));
	AER.iAEID = iAEID;
	AER.iPropID1 = iItemID;
	AER.iPropNum1 = iItemCounts;
}

CT_VOID CCActiveEvent::DisposeActiveOpen(CT_INT32 iActiveType)
{
	time_t t=time(0);
	tm *ptm=localtime(&t);
	CT_INT32 AcviteList[100] = {0};
	CT_INT32 iLastAEID = 0;
	CT_INT32 n = CCActive::getSingleton().GetActiveByType(iActiveType,AcviteList);
	if(n > 0)
	{
		tm Temptm=(*ptm);

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
						CT_INT32 iSec=iProperty2%60;
						CT_INT32 iClockID=IDI_TIMEACTIVE_OPEN+m_TimeActiveMap.size();

						if(iHour <= ptm->tm_hour && iMin < ptm->tm_min || iHour < ptm->tm_hour)continue;

						SetClock(iClockID,iHour,iMin,iSec);

						std::map<CT_INT32,CT_INT32>::iterator it=m_TimeActiveMap.find(iClockID);
						if(it != m_TimeActiveMap.end())
						{
							m_TimeActiveMap.erase(it);
							m_TimeActiveMap.insert(std::map<CT_INT32,CT_INT32>::value_type(iClockID,AcviteList[i]));
						}
						else
							m_TimeActiveMap.insert(std::map<CT_INT32,CT_INT32>::value_type(iClockID,AcviteList[i]));

						Temptm.tm_hour = iHour;
						Temptm.tm_min = iMin;
						Temptm.tm_sec = iSec;

						if((lMinTime > mktime(&Temptm) && mktime(&Temptm) != -1) || lMinTime == 0)
						{
							lMinTime=mktime(&Temptm);
							iLastAEID = AcviteList[i];
						}
					}
				}
				else
				{
					CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeActiveOpen G_GetActiveEvent %d",AcviteList[i]);
				}
			}
		}

		if(lMinTime == 0)
		{
			lMinTime = NextActiveTime(iActiveType, iLastAEID);
		}
		CCActive::getSingleton().SetTimeActiveStatus(iActiveType, iLastAEID, lMinTime);

// 		//发送活动下次开启时间
// 		CMD_G_BossActive ra={0};
// 		ra.iTime = CCBossManager::getSingleton().GetLastTime();
// 		ra.iTime = (ra.iTime <= -1?-1:ra.iTime-t);
// 		ra.iAEID = iLastAEID;
// 		CCUserManager::getSingleton().SendUserMessage(MAIN_G_WORLD_BOSS,SUB_G_WORLD_BOSS_ACTIVE_CLOSE,(CT_CHAR*)&ra,sizeof(ra));
	}
}

CT_VOID CCActiveEvent::DisposeDragonBall(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_DragonBallFind))
	{
		CCLogMeg::getSingleton().WriteError("DisposeDragonFind: len != sizeof(CMD_G_DragonBall)");
		return;
	}

	CMD_G_DragonBallFindFail dbff = {0};
	CMD_G_DragonBallFind *pDragonBall = (CMD_G_DragonBallFind *)pBuf;
	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(pDragonBall->dwUID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeDragonFind: connot find user.");
		return;
	}

	if (!pUser->m_userActive.ActivedDragonBall(dbff))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_DRAGONBALL_FAIL, (CT_CHAR*)&dbff, sizeof(dbff));
	}
}

CT_VOID CCActiveEvent::DisposeDragonBallWish(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_DragonBallWish))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeDragonBallWish  len != sizeof(CMD_G_DragonBallWish).");
		return;
	}

	ENUM_DragonBallFial enfail;
	CMD_G_DragonBallWishFail dbwf;
	CMD_G_DragonBallWish *pDBW = (CMD_G_DragonBallWish *)pBuf;
	CCUser *pUser = 0;
	G_DragonBall gdb = {0};
	if (!CCUserManager::getSingleton().Find(pDBW->dwUID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeDragonBallWish: connot find user.");
		return;
	}

	if (!G_GetDragonBall(pDBW->iIndex, &gdb))
	{
		CCLogMeg::getSingleton().WriteError("DisposeDragonBallWish G_GetDragonBall error.");
		return;
	}
	if (!pUser->m_userActive.ActiveDragonBallReward(gdb.iRewardID, enfail))
	{
		dbwf.enFail = enfail;
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_DRAGONBALL_WISHING_FAIL, (CT_CHAR*)&dbwf, sizeof(dbwf));
	}
	else
		{
		// 发放头衔赛亚人
		pUser->m_userTitle.ActiveTitle(TITLE_SAIYA);
	}
}

//月卡续费
CT_VOID CCActiveEvent::DisposeMonthCardRenew(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_MonthCardRenew))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeMonthCardRenew  len != sizeof(CMD_G_MonthCardRenew).");
		return;
	}

	CMD_G_MonthCardRenew *pMcr=(CMD_G_MonthCardRenew*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pMcr->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeMonthCardRenew No Find UserId:%d",pMcr->dwUserID);
		return;
	}

	CMD_G_MonthCardRenewFail mcrf={0};
	mcrf.iFail=ValidateMonthCard(pUser);
	if(mcrf.iFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_MONTH_CARD_RENEW_FAIL,(CT_CHAR*)&mcrf,sizeof(mcrf));
		return;
	}

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);
	CMD_G_MonthCardRenewInfo mri={0};
	mri.bReward=CT_FALSE;
	mri.iDiamod=mvd.stRes.iDiamod;
	mri.iTime=pUser->m_userActive.GetActiveQuanity(ACTIVE_MONTH_CARD);

	pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_MONTH_CARD_RENEW_INFO,(CT_CHAR*)&mri,sizeof(mri));

	return;
}

//月卡奖励
CT_VOID CCActiveEvent::DisposeMonthCardReward(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_MonthCardReward))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeMonthCardReward  len != sizeof(CMD_G_MonthCardReward).");
		return;
	}

	CMD_G_MonthCardReward *pMcr=(CMD_G_MonthCardReward*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pMcr->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeMonthCardReward No Find UserID:%d",pMcr->dwUserID);
		return;
	}
	CMD_G_MonthCardRewardFail mrf={0};
	mrf.iFail=ValidateMonthCardReward(pUser);
	if(mrf.iFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_MONTH_CARD_REWARD_FAIL,(CT_CHAR*)&mrf,sizeof(mrf));
		return;
	}

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	CMD_G_MonthCardRewardInfo mri={0};
	mri.iDiamod=mvd.stRes.iDiamod;
	pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_MONTH_CARD_REWARD_INFO,(CT_CHAR*)&mri,sizeof(mri));

	return;
}

//月卡开启验证
CT_INT32 CCActiveEvent::ValidateMonthCard(CCUser *pUser)
{
	G_ValueRange vrCost={0};
	G_ValueRange vrTime={0};
	if(!G_GetValueRange(VALUE_TYPE_MONTH_CARD_COST,&vrCost))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::ValidateMonthCard G_GetValueRange VALUE_TYPE_MONTH_CARD_COST");
		return MONTH_CARD_DATA;
	}
	if(!G_GetValueRange(VALUE_TYPE_MONTH_CARD_TIME,&vrTime))
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::ValidateMonthCard G_GetValueRange VALUE_TYPE_MONTH_CARD_TIME");
		return MONTH_CARD_DATA;
	}

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	//if(mvd.stRes.iDiamod+vrCost.fMaxVal < 0)
	RewardCost rc={0};
	FakeProBase fb={0};
	if(G_GetCostReward(vrCost.fMaxVal,pUser,ITEM_OP_MODE_CHECK,SAVETAG_MONTH_CARD,rc,fb) != 0)
		return MONTH_CARD_COST;

	CT_BOOL bActive=CT_TRUE;
	CT_INT32 iAID[100]={0};
	CCActive::getSingleton().GetActiveByType(ACTIVE_MONTHCARD,iAID);
	for(CT_INT32 i=0;i<100;++i)
	{
		if(iAID[i] == 4102)//次ID固定，不可以修改
		{
			bActive=CT_FALSE;
			break;
		}
	}

	if(bActive)
		return MONTH_CARD_ACTIVE;

	//活动ID固定
	CT_INT32 iStatus=(pUser->m_userActive.CheckActiveComplete(ACTIVE_MONTH_CARD)?ACTIVE_RECEIVED:ACTIVE_COMPLETED);
	
	memset(&rc,0,sizeof(rc));
	memset(&fb,0,sizeof(fb));
	G_GetCostReward(vrCost.fMaxVal,pUser,ITEM_OP_MODE_EXEC,SAVETAG_MONTH_CARD,rc,fb);

	CT_INT32 lTime=pUser->m_userActive.GetActiveQuanity(ACTIVE_MONTH_CARD)+vrTime.fMaxVal;
	pUser->m_userActive.SetActiveStatus(ACTIVE_MONTH_CARD,iStatus,lTime);

	MSC_DB_ActiveEventStatus aes = {0};
	aes.dwUserID=pUser->GetUserID();
	aes.iAID=ACTIVE_MONTH_CARD;
	aes.iStatus=iStatus;
	aes.iQuantity=lTime;
	CT_DWORD threadDBID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATEEX,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));

	//修改日志数据库信息
	G_ActiveEventReward AER={0};
	AER.iAEID=ACTIVE_MONTH_CARD;
	for(CT_INT32 i=0;i<TEAM_RES_CNT;++i)
	{
		AER.iDiamod=(rc.iUseResType[i] == VALUE_TYPE_DIAMOND?rc.iUseResCount[i]:0);
		AER.iFood=(rc.iUseResType[i] == VALUE_TYPE_FOOD?rc.iUseResCount[i]:0);
		AER.iGold=(rc.iUseResType[i] == VALUE_TYPE_GOLD?rc.iUseResCount[i]:0);
	}
	AER.iPropID1=rc.iUseItem[0];
	AER.iPropID2=rc.iUseItem[1];
	AER.iPropID3=rc.iUseItem[2];
	AER.iPropID4=rc.iUseItem[3];
	AER.iPropNum1=rc.iUseItemCount[0];
	AER.iPropNum2=rc.iUseItemCount[1];
	AER.iPropNum3=rc.iUseItemCount[2];
	AER.iPropNum4=rc.iUseItemCount[3];


	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);
	MSC_LOGDB_ActiveEventReward		logdbReward = {0};
	logdbReward.dwUserID = pUser->GetUserID();
	logdbReward.iGrade   = mvd.stGrade.iLevel;
	logdbReward.activeReward = AER;

	CT_DWORD LogDBthreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBthreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ACTIVEEVENTREWARD,(CT_CHAR*)&logdbReward,sizeof(logdbReward));


	return 0;
}

//月卡奖励
CT_INT32 CCActiveEvent::ValidateMonthCardReward(CCUser *pUser)
{
	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_MONTH_CARD_REWARD,&vr))
	{
		return MONTH_CARD_DATA;
	}

	if (pUser->m_userActive.CheckActiveComplete(ACTIVE_MONTH_CARD))
	{
		return MONTH_CARD_GET;
	}

	CT_INT32 iTime=pUser->m_userActive.GetActiveQuanity(ACTIVE_MONTH_CARD);
	if(iTime <= 0)
	{
		return MONTH_CARD_TIME;
	}
	pUser->m_userActive.SetActiveStatus(ACTIVE_MONTH_CARD,ACTIVE_RECEIVED,iTime);

	MasterVariableData mvd={0};
	mvd.stRes.iDiamod+=vr.fMaxVal;
	pUser->m_userBase.AddMastData(mvd,SAVETAG_MONTH_CARD);

	MSC_DB_ActiveEventStatus aes = {0};
	aes.dwUserID=pUser->GetUserID();
	aes.iAID=ACTIVE_MONTH_CARD;
	aes.iStatus=ACTIVE_RECEIVED;
	CT_DWORD threadDBID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATEEX,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));

	//修改日志数据库信息
	G_ActiveEventReward AER={0};
	AER.iAEID=ACTIVE_MONTH_CARD;
	AER.iDiamod=vr.fMaxVal;


	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);
	MSC_LOGDB_ActiveEventReward		logdbReward = {0};
	logdbReward.dwUserID = pUser->GetUserID();
	logdbReward.iGrade   = mvd.stGrade.iLevel;
	logdbReward.activeReward = AER;

	CT_DWORD LogDBthreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBthreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ACTIVEEVENTREWARD,(CT_CHAR*)&logdbReward,sizeof(logdbReward));
	return 0;
}