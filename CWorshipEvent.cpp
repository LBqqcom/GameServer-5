#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CWorshipEvent.h"
#include "CBroadcastScriptSetting.h"

CCWorshipEvent::CCWorshipEvent(CCGameMesManager *pMesManager,CT_WORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_COST);
	RegistMessage(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_REWARD);
}

CCWorshipEvent::~CCWorshipEvent()
{

}

CT_VOID CCWorshipEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MAIN_G_WORSHIP_EVENT_CITY:
		{
			if (subID==SUB_G_WORSHIP_COST)
			{
				DisposeWorship(pBuf,len);
			}
			else if (subID==SUB_G_WORSHIP_REWARD)
			{
				DisposeWorshipReward(pBuf,len);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCWorshipEvent::OnUserEvent mianid %d  subid %d",mainID,subID);
				return;
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCWorshipEvent::OnUserEvent mianid %d",mainID);
			return;
		}
	}
}

CT_VOID CCWorshipEvent::DisposeWorship(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorshipCost))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship len!= CMD_G_WorshipCost %d",len);
		return;
	}
	CMD_G_WorshipCost *worshipCost = (CMD_G_WorshipCost*)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(worshipCost->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,can't find user %d",worshipCost->dwUserID);
		return;
	}
	//查看该类型祭祀是否可以献祭
	if (!pUser->m_userWorship.CheckAbilityToWorship(worshipCost->iType)||worshipCost->iNum<=0)
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,can't worship %d item num %d",worshipCost->iType,worshipCost->iNum);
		pUser->CloseNet();
		return;
	}
	CT_INT32		iXp = 0;
	//道具可用于改类型献祭
	if ((iXp=G_CheckItemForWorship(worshipCost->iItemID,worshipCost->iType))<=0)
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,can't worship %d itemID ",worshipCost->iItemID);
		pUser->CloseNet();
		return;
	}
	//消耗道具
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	if (!pUser->m_userItem.AutoExpendItem(SAVETAG_WORSHIP_COST,worshipCost->iItemID,worshipCost->iNum))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,lack itemID %d num %d",worshipCost->iItemID,worshipCost->iNum);
		pUser->CloseNet();
		return;
	}
	pUser->m_userWorship.UpdateWorshipStatus(worshipCost->iType,worshipCost->iNum*iXp);
}

CT_VOID CCWorshipEvent::DisposeWorshipReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_WorshipRewardRequest))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorshipReward len!= CMD_G_WorshipRewardRequest %d",len);
		return;
	}
	CMD_G_WorshipRewardRequest *worshipReward = (CMD_G_WorshipRewardRequest*)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(worshipReward->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,can't find user %d",worshipReward->dwUserID);
		return;
	}

	if (worshipReward->iType > 100)	//钻石祭祀	检查剩余次数
	{
		if(!pUser->m_userWorship.CheckWorshipCount(worshipReward->iType))
		{
			pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_NO_VIPTIME);
			return;
		}	
	}
	else if (!pUser->m_userWorship.CheckWorshipComplete(worshipReward->iType))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,can't be reward %d",worshipReward->iType);
		return;
	}
	MasterData md={0};
	pUser->m_userBase.GetMasterData(md);
	G_WorshipReward		wr = {0};
	if (!G_GetWorshipReward(worshipReward->iType,md.mvd.stGrade.iLevel,&wr))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,can't get reward info %d",worshipReward->iType);
		pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_DATA_ERROR);
		return;
	}
	FakeProBase fb={0};
	G_GetFakeProDta(pUser,SAVETAGEEX_WORSHIP_TIME0+GetWorshipType(worshipReward->iType),fb);
	CT_INT32 iGoods1[GOODS_COUNT]={0},iCount1[GOODS_COUNT]={0},iGoods2[GOODS_COUNT]={0},iCount2[GOODS_COUNT]={0};
	if (wr.iItemGroupID1>0)
	{
		G_GetGoodsGroupInfo(wr.iItemGroupID1,iGoods1,iCount1,fb);
	}
	if (wr.iItemGroupID2>0)
	{
		G_GetGoodsGroupInfo(wr.iItemGroupID2,iGoods2,iCount2,fb);
	}
	G_ClearFakeProDat(pUser,SAVETAGEEX_WORSHIP_TIME0+GetWorshipType(worshipReward->iType),fb);

	//把两个道具合到一起
	CT_INT32 iNum = 0;
	for (CT_INT32 i=0;i<GOODS_COUNT;++i)
	{
		if (iGoods1[i]>0)
		{
			iNum++;
		}
		else
		{
			break;
		}
	}
	for (CT_INT32 i=0;i<GOODS_COUNT;++i)
	{
		if (iGoods2[i]>0)
		{
			iGoods1[iNum]=iGoods2[i];  
			iCount1[iNum]=iCount2[i];
			iNum++;
		}
		else
		{
			break;
		}
	}
	if (pUser->m_userItem.CalcEmptySlot() < 2)
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,bag full");
		pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_BAG_FULL);
		return;
	}

	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	if (!pUser->m_userItem.AutoAddItemData(SAVETAG_WORSHIP_REWARD,iGoods1,iCount1,iNum,ITEM_OP_MODE_CHECK))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,bag full");
		pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_BAG_FULL);
		return;
	}
	CMD_G_BuildingData  buildData={0};
	if (!pUser->m_userBuilding.GetBuildingData(CITY_TYPE_TOTEM,buildData))
	{
		CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,find CITY_TYPE_TOTEM info error");
		pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_DATA_ERROR);
		return;
	}
	CT_INT32 iGoldCost = 0;
	CT_INT32 iDiamodCost = 0;
	CT_INT32 iWorshipCount = 0;
	if (worshipReward->iType > 100)
	{
		iWorshipCount = pUser->m_userWorship.GetWorshipCount(worshipReward->iType);
		iDiamodCost = G_GetWorshipRewardCost(wr.iGoldCostBase,iWorshipCount+1,wr.fGoldCost);
		if (md.mvd.stRes.iDiamod+iDiamodCost<0)
		{
			pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_NO_DIAMOND);
			return;
		}
	}
	else
	{
		iGoldCost=G_GetWorshipRewardCost(wr.iGoldCostBase,buildData.iGrade,wr.fGoldCost);
		
		if (md.mvd.stRes.iGold+iGoldCost<0)
		{
			CCLogMeg::getSingleton().WriteError("CCWorshipEvent::DisposeWorship,Gold lack need %d",iGoldCost);
			pUser->CloseNet();
			return;
		}
	}
	//到这里就可以领奖了
	pUser->m_userItem.AutoAddItemData(SAVETAG_WORSHIP_REWARD,iGoods1,iCount1,iNum);
	memset(&md.mvd,0,sizeof(md.mvd));
	md.mvd.stRes.iGold+=iGoldCost+G_GetWorshipRewardCost(wr.iGoldBase,buildData.iGrade,wr.fGold);
	md.mvd.stRes.iFood+=G_GetWorshipRewardCost(wr.iFoodBase,buildData.iGrade,wr.fFood);
	md.mvd.stRes.iTalentNum+=G_GetWorshipRewardCost(wr.iTalentBase,buildData.iGrade,wr.fTalent);
	md.mvd.stRes.iDiamod+=iDiamodCost + G_GetWorshipRewardCost(wr.iDiamodBase,buildData.iGrade,wr.fDiamod);
	md.mvd.stGrade.iCurrentExp+=G_GetWorshipRewardCost(wr.iXpBase,buildData.iGrade,wr.fXp);
	pUser->m_userBase.AddMastData(md.mvd,SAVETAG_WORSHIP_REWARD);

	for(CT_INT32 i=0; i<iNum; ++i)
	{
		if (G_BroadcastFilter(4, iGoods1[i]))
		{
			RollBroadcast rb[2]={0};
			MasterData masterData = {0};
			pUser->m_userBase.GetMasterData(masterData);
			rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
			rb[1].SetValue(iGoods1[i],"");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{7}<2:%d>", pUser->GetUserID(), masterData.cNickName,iGoods1[i]);
			G_SendRollBroadecast(126,3,1,(CT_CHAR*)rb,sizeof(rb));
		}
	}
	
	//数据库日志信息
	MSC_LOGDB_WorshipReward		logdbReward = {0};
	logdbReward.dwUserID = pUser->GetUserID();
	logdbReward.iGoldCost=iGoldCost;
	logdbReward.iGold=md.mvd.stRes.iGold;
	logdbReward.iFood=md.mvd.stRes.iFood;
	logdbReward.iDiamod=md.mvd.stRes.iDiamod;
	logdbReward.iTalent=md.mvd.stRes.iTalentNum;
	logdbReward.iXp=md.mvd.stGrade.iCurrentExp;
	logdbReward.iItemID1=iGoods1[0];
	logdbReward.iItemNum1=iCount1[0];
	logdbReward.iItemID2=iGoods2[1];
	logdbReward.iItemNum2=iCount1[1];
	logdbReward.iGrade=md.mvd.stGrade.iLevel;	//zhoujun
	

	//修改内存数据
	if (worshipReward->iType > 100)		//记录钻石祭祀次数
	{
		pUser->m_userWorship.WorshipCount(worshipReward->iType);
	}
	else
	{
		pUser->m_userWorship.WorshipGetReward(worshipReward->iType);
	}
	//发送客户端信息
	CMD_G_WorshipReward		sendReward = {0};
	memset(&md.mvd,0,sizeof(md.mvd));
	pUser->m_userBase.GetMasterData(md.mvd);
	sendReward.iDiamod=md.mvd.stRes.iDiamod;
	sendReward.iFood=md.mvd.stRes.iFood;
	sendReward.iGold=md.mvd.stRes.iGold;
	sendReward.iTalent=md.mvd.stRes.iTalentNum;
	sendReward.iXp=md.mvd.stGrade.iCurrentExp;
	sendReward.iType=worshipReward->iType;
	pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_REWARD,(CT_CHAR*)&sendReward,sizeof(sendReward));

	//发送客户端消息，更新祭祀状态
	if (worshipReward->iType > 100)
	{
		CMD_G_WorshipCountUpdate  wcu = {0};
		wcu.iType=worshipReward->iType;
		wcu.iCount = iWorshipCount + 1;	//钻石祭祀次数
		pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_COUNT_UPDATE,(CT_CHAR*)&wcu,sizeof(CMD_G_WorshipCountUpdate));
	}
	else
	{
		CMD_G_WorshipStatusUpdate  wsu = {0};
		wsu.iType=worshipReward->iType;
		pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_STATUS_UPDATE,(CT_CHAR*)&wsu,sizeof(CMD_G_WorshipStatusUpdate));
	}
	

	//修改数据库信息
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	MSC_DB_WorshipStatus  worshipStatus={0};
	worshipStatus.dwUserID=pUser->GetUserID();
	worshipStatus.iType=worshipReward->iType;
	worshipStatus.iStatus=WORSHIP_UNCOMPLETE;
	if (worshipReward->iType > 100)
	{
		worshipStatus.iXp = iWorshipCount + 1;	//钻石祭祀次数
	}
	else
	{
		worshipStatus.iXp=0;
	}
	G_SendThreadInfo(DBthreadid,MSC_MAIN_WORSHIP_EVENT_DATA,MSC_SUB_WORSHIP_STATUS_UPDATE,(CT_CHAR*)&worshipStatus,sizeof(worshipStatus));

	//修改日志数据库信息
	CT_DWORD LogDBthreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBthreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_WORSHIP_REWARD,(CT_CHAR*)&logdbReward,sizeof(logdbReward));
}

CT_INT32 CCWorshipEvent::GetWorshipType(CT_INT32 iType)
{
	switch(iType)//和刚爷约定不改类型
	{
	case 1:
		{
			return WORSHIP_TYPE_FOOD;
		}
	case 2:
		{
			return WORSHIP_TYPE_TALENT;
		}
	case 3:
		{
			return WORSHIP_TYPE_LUCK;
		}
	case 101:
		{
			return WORSHIP_TYPE_FOOD_D;
		}
	case 102:
		{
			return WORSHIP_TYPE_TALENT_D;
		}
	case 103:
		{
			return WORSHIP_TYPE_LUCK_D;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCWorshipEvent::GetWorshipType Type:%d",iType);
			return 0;
		}
	}
	return 0;
}