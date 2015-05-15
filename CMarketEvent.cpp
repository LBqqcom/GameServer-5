#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CMarketEvent.h"
#include "CMarket.h"
#include "CBroadcastScriptSetting.h"
#include "CActive.h"

//CT_INT64			g_lBlackRefreshTime;			//黑市刷新时间
//CT_INT64			g_lMarketRefreshTime;			//商城刷新时间 
CCMarketEvent::CCMarketEvent(CCGameMesManager *pMesManager,CT_WORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_SHOPPING);
	RegistMessage(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_REFRESH_REQUEST);
	RegistMessage(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_BUY_STATMINA);
//	RegistTimer(IDI_BLACK_MARKET_REFRESH_TIMER);
//	RegistTimer(IDI_MARKET_REFRESH_TIMER);

    if (m_dwThread == GAME_THREAD_CITY) {
		RegistMessage( MSC_MAIN_MARKETDATA, MSC_SUB_UPDATE_PERSON);
//	    g_lBlackRefreshTime=time(0);
//	    g_lMarketRefreshTime=time(0);
//	    G_ValueRange vr={0};
//	    if (G_GetValueRange(VALUE_TYPE_MARKET_NORMALREFRESHTIME,&vr))
//	    {
//		    SetTimer(IDI_MARKET_REFRESH_TIMER, (CT_INT32)vr.fMaxVal*1000);
//	    }
//		
//	    memset(&vr,0,sizeof(vr));
//	    if (G_GetValueRange(VALUE_TYPE_MARKET_BLACKREFRESHTIME_1,&vr))
//	    {
//		    SetTimer(IDI_BLACK_MARKET_REFRESH_TIMER,(CT_INT32)vr.fMaxVal*1000);
//	    }

		G_ValueRange vr={0};
		CT_INT32 iTime;
		CT_INT32 iHour,iMinute,iSecond;
		for (CT_INT32 i=0; i<10; ++i)
		{
			memset(&vr,0,sizeof(vr));
			if (G_GetValueRange(VALUE_TYPE_MARKET_NORMALREFRESHTIME+i,&vr) && vr.fMaxVal > -1)
			{
				iTime = (CT_INT32)vr.fMaxVal;
				iHour = iTime / 3600;
				iMinute = (iTime % 3600) / 60;
				iSecond = iTime % 60;
				RegistClock(IDI_MARKET_REFRESH_CLOCK+i);
				SetClock(IDI_MARKET_REFRESH_CLOCK+i, iHour, iMinute, iSecond);
			}

			memset(&vr,0,sizeof(vr));
			if (G_GetValueRange(VALUE_TYPE_MARKET_BLACKREFRESHTIME+i,&vr) && vr.fMaxVal > -1)
			{
				iTime = (CT_INT32)vr.fMaxVal;
				iHour = iTime / 3600;
				iMinute = (iTime % 3600) / 60;
				iSecond = iTime % 60;
				RegistClock(IDI_BLACK_MARKET_REFRESH_CLOCK+i);
				SetClock(IDI_BLACK_MARKET_REFRESH_CLOCK+i, iHour, iMinute, iSecond);
			}
		}

    }
}

CCMarketEvent::~CCMarketEvent()
{

}

CT_VOID CCMarketEvent::OnClock(CT_DWORD timeID)
{
	if (timeID >= IDI_MARKET_REFRESH_CLOCK && timeID < IDI_MARKET_REFRESH_CLOCK + 10)
	{
//		g_lMarketRefreshTime=time(0);

		//更新服务器内存中商品信息
		CCMarket::getSingleton().RefreshMallGoodsStatus();
		CCMarket::getSingleton().RefreshMarketGoodsStatus();
		//通知客户端更新
		CCUserManager::getSingleton().RefreshMarketGoods();

		//活动
		//刷新个人限购
		//CT_CHAR cBuf[32]={0};
		//G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_REFRESH,cBuf,sizeof(cBuf));
	}

	if (timeID >= IDI_BLACK_MARKET_REFRESH_CLOCK && timeID < IDI_BLACK_MARKET_REFRESH_CLOCK + 10)
	{
		CCUserManager::getSingleton().RefreshBlackMarketGoods();
		CCMarket::getSingleton().SetRefreshBlackMarket(time(0));
	}
}

CT_VOID CCMarketEvent::OnTime(CT_DWORD timeID)
{
/*	if (timeID==IDI_MARKET_REFRESH_TIMER)
	{
		g_lMarketRefreshTime=time(0);

		//更新服务器内存中商品信息
		CCMarket::getSingleton().RefreshMallGoodsStatus();
		CCMarket::getSingleton().RefreshMarketGoodsStatus();
		//通知客户端更新
		CCUserManager::getSingleton().RefreshMarketGoods();
	}
	if (timeID==IDI_BLACK_MARKET_REFRESH_TIMER)
	{
		g_lBlackRefreshTime=time(0);
		CCUserManager::getSingleton().RefreshBlackMarketGoods();
	}
*/
}

CT_VOID CCMarketEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MAIN_G_MARKET_EVENT_CITY:
		{
			if (subID==SUB_G_MARKET_SHOPPING)
			{
				DisposeShopping(pBuf,len);
			}
			else if (subID==SUB_G_MARKET_REFRESH_REQUEST)
			{
				DisposeRefresh(pBuf,len);
			}
			else if(subID == SUB_G_MARKET_BUY_STATMINA)
			{
				DisposeBuyStatmina(pBuf,len);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCMarketEvent::OnUserEvent mianid %d  subid %d",mainID,subID);
				return;
			}
			break;
		}
	case MSC_MAIN_MARKETDATA:
		{
			if (subID == MSC_SUB_UPDATE_PERSON)
			{		
				CCMarket::getSingleton().UpdataMarketPersonal(pBuf, len);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::OnUserEvent mianid %d",mainID);
			return;
		}
	}
}

CT_VOID CCMarketEvent::DisposeShopping(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_MarketShoppingInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping mes len %d",len);
		return;
	}
	CMD_G_MarketShoppingInfo *pGoodsInfo = (CMD_G_MarketShoppingInfo *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pGoodsInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping,can't find user %d",pGoodsInfo->dwUserID);
		return;
	}
	//用户线程id
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	if (pGoodsInfo->iNum<=0)
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping,shopping num less %d",pGoodsInfo->iNum);
		pUser->CloseNet();
		return;
	}
	CT_INT32 iGoodID = 0;
	if (pGoodsInfo->iMarketType == MARKET_BLACK_TYPE || pGoodsInfo->iMarketType == MARKET_BLACK_PAY_TYPE)
	{
		iGoodID=pUser->m_userBlackMarket.GetBlackMarketGoodsID(pGoodsInfo->iGoodsID);
	}
	else if(pGoodsInfo->iMarketType == MARKET_SEEKBLACK_TYPE)
	{
		iGoodID=pUser->m_userBlackMarket.GetSeekBlackMarketGoodsID(pGoodsInfo->iGoodsID);
	}
	else
	{
		iGoodID=pGoodsInfo->iGoodsID;
	}
    CT_DWORD saveTag = 0;
    switch(pGoodsInfo->iMarketType)
    {
    case MARKET_NORMAL_TYPE: saveTag = SAVETAG_MARKET_NORMAL; break;
    case MARKET_MALL_TYPE: saveTag = SAVETAG_MARKET_MALL; break;
    case MARKET_BLACK_TYPE: saveTag = SAVETAG_MARKET_BLACK; break;
    case MARKET_BLACK_PAY_TYPE: saveTag = SAVETAG_MARKET_BLACK; break;
    case MARKET_PARSONAL_TYPE: saveTag = SAVETAG_MARKET_MALL; break;
	case MARKET_SEEKBLACK_TYPE:saveTag = SAVATAG_SEEK_BLACK; break;
    default:break;
    }
	//背包是否还有空格
	if (LOOT_SUC != pUser->LootItems(saveTag, &iGoodID,&(pGoodsInfo->iNum),1,ITEM_OP_MODE_CHECK))
	{
		CMD_G_MarketRequestFail fail = {0};
		fail.dwUserID = pGoodsInfo->dwUserID;
		fail.enFail = MARKET_BAG_FULL;
		pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_REQUEST_FAIL,(CT_CHAR*)&fail,sizeof(fail));
		return;
	}
	
	CT_BOOL  bShopResult;
	switch(pGoodsInfo->iMarketType)
	{
	case MARKET_NORMAL_TYPE:
		{
			bShopResult=DisposeNormalMarketShopping(pUser,pGoodsInfo);
			break;
		}
	case MARKET_MALL_TYPE:
		{
			bShopResult=DisposeMallMarketShopping(pUser,pGoodsInfo);
			break;
		}
	case MARKET_BLACK_TYPE:
	case MARKET_BLACK_PAY_TYPE:
		{
			bShopResult=DisposeBlackMarketShopping(pUser,pGoodsInfo);
			break;
		}
	case MARKET_PARSONAL_TYPE:
		{
			bShopResult=DisposeMarketPersonal(pUser,pGoodsInfo);
			break;
		}
	case MARKET_SEEKBLACK_TYPE:
		{
			bShopResult=DisposeSeekBlackMarketShopping(pUser,pGoodsInfo);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping no market type %d ",pGoodsInfo->iMarketType);
			pUser->CloseNet();
			return;
		}
	}
	//完成购买，添加道具
	if (bShopResult==CT_TRUE)
	{        
		pUser->LootItems(saveTag, &iGoodID,&(pGoodsInfo->iNum));
		//增加黑店物品消息广播
		if (pGoodsInfo->iMarketType==MARKET_BLACK_TYPE||pGoodsInfo->iMarketType==MARKET_BLACK_PAY_TYPE)
		{
			G_ItemBase ib={0};
			if (!G_GetItemBase(iGoodID,&ib))
			{
				return;
			}

		
			//CMD_G_BroadCastMes bcm = {0};
			//bcm.dwUserID = pUser->GetUserID();
			//bcm.iSubType = BROADCAST_SUB_TYPE_ITEM_BMARKET;
			//bcm.iQuality = ib.iQuality;
			//memcpy(bcm.cResultName,ib.szName,NAME_SIZE);


			//G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_SYSTEM, (CT_CHAR*)&bcm, sizeof(bcm));

			G_BroadcastInfo broadcastInfo={0};
			if (CCBroadcastScriptSetting::getSingleton().GetBroadcastMes(BROADCAST_SUB_TYPE_ITEM_BMARKET,&broadcastInfo))
			{
				if(broadcastInfo.iLevel <= ib.iQuality)
				{
					RollBroadcast rb[2]={0};
					MasterData md={0};
					pUser->m_userBase.GetMasterData(md);
					rb[0].SetValue(pUser->GetUserID(),md.cNickName);
					rb[1].SetValue(iGoodID,"");
					G_SendRollBroadecast(131,2,1,(CT_CHAR*)rb,sizeof(rb));
				}
			}
			
		}
		//end
	}
	return;
}

CT_VOID CCMarketEvent::DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len!=sizeof(CT_DWORD))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeRefresh len != int");
		return ;
	}
	CT_DWORD dwUserID=0;
	memcpy(&dwUserID,pBuf,len);
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeRefresh,can't find user %d",dwUserID);
		return;
	}
	if (pUser->m_userBlackMarket.RefreshOnce())
	{
		pUser->m_userBlackMarket.RefreshBlackMarket(MARKET_BLACK_PAY_TYPE);
	}
}

//购买体力
CT_VOID CCMarketEvent::DisposeBuyStatmina(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_BuyStatmina))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeBuyStatmina len != sizeof(CMD_G_BuyStatmina)");
		return;
	}

	CMD_G_BuyStatmina *pBuyStatmina=(CMD_G_BuyStatmina*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pBuyStatmina->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeBuyStatmina:%d No Find UserID",pBuyStatmina->dwUserID);
		return;
	}

	CMD_G_BuyStatminaSuc BuyStatminaSuc={0};
	MasterData MasterInfo={0};

	CT_INT32 iDiamod=G_GetStatminaCost(pUser);
	pUser->m_userBase.GetMasterData(MasterInfo);

	//增加体力购买次数上限
	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_STATMINA_TIME,&vr))
	{
		CCLogMeg::getSingleton().WriteError("G_SaveStatminaCost: G_GetValueRange VALUE_TYPE_STATMINA_TIME%s%d",__FILE__,__LINE__);
		return;
	}
	
	if(MasterInfo.mvd.stRes.iStaminaBuyTimes >= vr.fMinVal + pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_STATMINA))
	{
		BuyStatminaSuc.dwUserID=pUser->GetUserID();
		BuyStatminaSuc.iDiamod=MasterInfo.mvd.stRes.iDiamod;
		BuyStatminaSuc.cbBuySuc=BUYSTATMINA_NO_NUM;
		pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_BUY_STATMINA_SUC,(CT_CHAR*)&BuyStatminaSuc,sizeof(BuyStatminaSuc));
	}
	else if((-pBuyStatmina->iDiamod < -iDiamod && pBuyStatmina->iDiamod < 0) || MasterInfo.mvd.stRes.iDiamod < -iDiamod)
	{
		BuyStatminaSuc.dwUserID=pUser->GetUserID();
		BuyStatminaSuc.iDiamod=iDiamod;
		BuyStatminaSuc.cbBuySuc=BUYSTATMINA_NO_DIAMOD;
		pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_BUY_STATMINA_SUC,(CT_CHAR*)&BuyStatminaSuc,sizeof(BuyStatminaSuc));
	} 
	else
	{
		//加成体力
		if(!G_SaveStatminaCost(pUser))
		{
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeBuyStatmina %s %d",__FILE__,__LINE__);
			return;
		}

		memset(&MasterInfo,0,sizeof(MasterInfo));
		pUser->m_userBase.GetMasterData(MasterInfo);
		BuyStatminaSuc.dwUserID=pUser->GetUserID();
		BuyStatminaSuc.cbBuySuc=BUYSTATMINA_SUC;
		BuyStatminaSuc.iDiamod=MasterInfo.mvd.stRes.iDiamod;
		BuyStatminaSuc.iStatmina=MasterInfo.mvd.stRes.iStamina;
		pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_BUY_STATMINA_SUC,(CT_CHAR*)&BuyStatminaSuc,sizeof(BuyStatminaSuc));

		pUser->m_userDaily.UpdateQuestData(DAILY_BUY_STATMINA,1);
	}

	return;
}

CT_BOOL CCMarketEvent::DisposeNormalMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo)
{
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	//判断集市是否建造或者有事件，升级等
	if (!pUser->m_userBuilding.FindBuildData(CITY_TYPE_MARKET_BUILDID)||pUser->m_userBuilding.FindUpgradeBuildData(CITY_TYPE_MARKET_BUILDID))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeNormalMarketShopping,can't find CITY_TYPE_MARKET_BUILDID");
		pUser->CloseNet();
		return CT_FALSE;
	}
	//查看建筑是否有事件
	if (pUser->m_userEvent.FindBuildingEventByID(CITY_TYPE_MARKET_BUILDID))
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeNormalMarketShopping,CITY_TYPE_MARKET_BUILDID in event");
		pUser->CloseNet();
		return CT_FALSE;
	}
	G_MarketNormalGoods nomalGoods = {0};
	if (!G_GetMarketNomalGoods(pGoodsInfo->iGoodsID,&nomalGoods))
	{
		pUser->CloseNet();
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeNormalMarketShopping G_GetMarketNomalGoods no id %d",pGoodsInfo->iGoodsID);
		return CT_FALSE;
	}
	//首先判断用户资源是否充足，在判断商店物品数量是否够，然后再购买
	if (nomalGoods.iItemID>0)
	{
		//道具数量是否充足
		if (pUser->m_userItem.GetItemCount(nomalGoods.iItemID)+(nomalGoods.iNum*pGoodsInfo->iNum)<0)
		{
			pUser->CloseNet();
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeNormalMarketShopping user %d lack item %d",pGoodsInfo->dwUserID,pGoodsInfo->iGoodsID);
			return CT_FALSE;
		}
		//pUser->m_userItem.AutoExpendItem(threadid,nomalGoods.iItemID,-(nomalGoods.iNum*pGoodsInfo->iNum));
	}
	else
	{
		MasterVariableData mvd = {0};
		pUser->m_userBase.GetMasterData(mvd);
		//根据商品消耗的货币判断
		if (nomalGoods.iGold<0)
		{
			if (mvd.stRes.iGold+(nomalGoods.iGold*pGoodsInfo->iNum)<0)
			{
				pUser->CloseNet();
				CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeNormalMarketShopping user %d lack gold goods %d",pGoodsInfo->dwUserID,pGoodsInfo->iGoodsID);
				return CT_FALSE;
			}
		}
		else
		{
			if (mvd.stRes.iDiamod+(nomalGoods.iDiamod*pGoodsInfo->iNum)<0)
			{
				pUser->CloseNet();
				CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeNormalMarketShopping user %d lack diamod goods %d",pGoodsInfo->dwUserID,pGoodsInfo->iGoodsID);
				return CT_FALSE;
			}
		}
	}
	//是否是限量的物品
	if (CCMarket::getSingleton().ViewMarketGoodsStatus(pGoodsInfo->iGoodsID))
	{
		CT_INT32 iLastNum=0;
		if (!CCMarket::getSingleton().UpdateMarketGoodsStaus(pGoodsInfo->iGoodsID,pGoodsInfo->iNum,iLastNum))
		{
			pUser->CloseNet();
			CCLogMeg::getSingleton().WriteError("CCUserManager::DisposeNormalMarketShopping goods lack %d",pGoodsInfo->iGoodsID);
			return CT_FALSE;
		}
		CMD_G_MarketGoodsInfo goodsInfo = {0};
		goodsInfo.iMarketType = pGoodsInfo->iMarketType;
		goodsInfo.iGoodsID = pGoodsInfo->iGoodsID;
		goodsInfo.iNum = iLastNum;
		//商品不足直接退出,数量足够向所有用户更新商品数量
		CCUserManager::getSingleton().UpdateSingleGoodsStatus(goodsInfo);
	}
	MSC_LOGDB_ShoppingRecord shoppingRecord = {0};
	//购买，扣除资源
	if (nomalGoods.iItemID>0)
	{
		pUser->m_userItem.AutoExpendItem(SAVETAG_MARKET_NORMAL,nomalGoods.iItemID,-(nomalGoods.iNum*pGoodsInfo->iNum));
		shoppingRecord.iItemID = nomalGoods.iItemID;
		shoppingRecord.iItemNum = nomalGoods.iNum*pGoodsInfo->iNum;
	}
	else
	{
		MasterVariableData mvd = {0};
		if (nomalGoods.iGold<0)
		{
			mvd.stRes.iGold += nomalGoods.iGold*pGoodsInfo->iNum;
			shoppingRecord.iGold = nomalGoods.iGold*pGoodsInfo->iNum;
		}
		else
		{
			mvd.stRes.iDiamod += nomalGoods.iDiamod*pGoodsInfo->iNum;
			shoppingRecord.iDiamod = nomalGoods.iDiamod*pGoodsInfo->iNum;
		}
		pUser->m_userBase.AddMastData(mvd,SAVETAG_MARKET_NORMAL);
		pUser->m_userBase.GetMasterData(mvd);
		CMD_G_MarketMaster mm = {0};
		mm.dwUserID = pUser->GetUserID();
		mm.iGold = mvd.stRes.iGold;
		mm.iDiamod = mvd.stRes.iDiamod;
		pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_UPDATE_MASTER,(CT_CHAR*)&mm,sizeof(CMD_G_MarketMaster));
	}
	//记录用户购买记录
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);

	shoppingRecord.dwUserID = pUser->GetUserID();
	shoppingRecord.iUserGrade = mvd.stGrade.iLevel;
	shoppingRecord.iGoodsID = pGoodsInfo->iGoodsID;
	shoppingRecord.iGoodsNum = pGoodsInfo->iNum;
	shoppingRecord.iMarketType = pGoodsInfo->iMarketType;
	CT_DWORD LogDBThreadID = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBThreadID,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SHOPPING,(CT_CHAR*)&shoppingRecord,sizeof(shoppingRecord));
	//G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_MARKET_RECORD_DATA,(CT_CHAR*)&shoppingRecord,sizeof(MSC_DB_ShoppingRecord));
	return CT_TRUE;
}

CT_BOOL CCMarketEvent::DisposeMallMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo)
{
	//判断用户商城是否开启
	if (!pUser->m_userBlackMarket.ViewMallMarket())
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeMallMarketShopping user %d mall haven't open",pUser->GetUserID());
		pUser->CloseNet();
		return CT_FALSE;
	}
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_MarketMallGoods mallGoods = {0};
	if (!G_GetMarketMallGoods(pGoodsInfo->iGoodsID,&mallGoods))
	{
		pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeMallMarketShopping G_GetMarketMallGoods no id %d",pGoodsInfo->iGoodsID);
		return CT_FALSE;
	}
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	//根据商品消耗的货币判断
	if (mvd.stRes.iDiamod+(mallGoods.iDiamod*pGoodsInfo->iNum)*(CCActive::getSingleton().GetMarketDiscount(pGoodsInfo->iGoodsID))<0)  //计算活动商品打折
	{
		pUser->CloseNet();
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeMallMarketShopping user %d lack diamod goods %d",pGoodsInfo->dwUserID,pGoodsInfo->iGoodsID);
		return CT_FALSE;
	}
	//是否是限量的物品
	if (CCMarket::getSingleton().ViewMallGoodsStatus(pGoodsInfo->iGoodsID))
	{
		CT_INT32 iLastNum=0;
		if (!CCMarket::getSingleton().UpdateMallGoodsStatus(pGoodsInfo->iGoodsID,pGoodsInfo->iNum,iLastNum))
		{
			pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_DATA_ERROR);
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeMallMarketShopping goods lack %d",pGoodsInfo->iGoodsID);
			return CT_FALSE;
		}
		CMD_G_MarketGoodsInfo goodsInfo = {0};
		goodsInfo.iMarketType = pGoodsInfo->iMarketType;
		goodsInfo.iGoodsID = pGoodsInfo->iGoodsID;
		goodsInfo.iNum = iLastNum;
		//商品不足直接退出,数量足够向所有用户更新商品数量
		CCUserManager::getSingleton().UpdateSingleGoodsStatus(goodsInfo);
	}
	MSC_LOGDB_ShoppingRecord shoppingRecord = {0};
	memset(&mvd,0,sizeof(mvd));
	CT_FLOAT fDiamod = mallGoods.iDiamod*pGoodsInfo->iNum*(CCActive::getSingleton().GetMarketDiscount(pGoodsInfo->iGoodsID));
	mvd.stRes.iDiamod += (CT_INT32)fDiamod; //活动打折
	pUser->m_userBase.AddMastData(mvd,SAVETAG_MARKET_MALL);
	pUser->m_userBase.GetMasterData(mvd);
	CMD_G_MarketMaster mm = {0};
	mm.dwUserID = pUser->GetUserID();
	mm.iGold = mvd.stRes.iGold;
	mm.iDiamod = mvd.stRes.iDiamod;
	pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_UPDATE_MASTER,(CT_CHAR*)&mm,sizeof(CMD_G_MarketMaster));
	shoppingRecord.dwUserID = pUser->GetUserID();
	shoppingRecord.iUserGrade = mvd.stGrade.iLevel;
	shoppingRecord.iGoodsID = pGoodsInfo->iGoodsID;
	shoppingRecord.iGoodsNum = pGoodsInfo->iNum;
	shoppingRecord.iMarketType = pGoodsInfo->iMarketType;
	shoppingRecord.iDiamod = mallGoods.iDiamod*pGoodsInfo->iNum;
	CT_DWORD LogDBThreadID = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBThreadID,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SHOPPING,(CT_CHAR*)&shoppingRecord,sizeof(shoppingRecord));
	return CT_TRUE;
}

CT_BOOL CCMarketEvent::DisposeMarketPersonal(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo)
{
	//判断用户商城是否开启
	if (!pUser->m_userBlackMarket.ViewMallMarket())
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeMallMarketShopping user %d mall haven't open",pUser->GetUserID());
		pUser->CloseNet();
		return CT_FALSE;
	}
	MarketPersonalAll mallGoods = {0};
	CCMarket::getSingleton().GetMarketPersonalGoods(pGoodsInfo->iGoodsID,mallGoods);

	CT_INT32 iCost=mallGoods.iNum;
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	mallGoods.iNum=(iCost*pGoodsInfo->iNum)*(CCActive::getSingleton().GetMarketDiscount(pGoodsInfo->iGoodsID));
	G_ChangeResType(mvd,mallGoods.iType,iCost);
	//根据商品消耗的货币判断
	if (iCost < 0)  //计算活动商品打折
	{
		pUser->CloseNet();
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeMallMarketShopping user %d lack diamod goods %d",pGoodsInfo->dwUserID,pGoodsInfo->iGoodsID);
		return CT_FALSE;
	}
	//是否是限量的物品
	CT_INT32 iBuyNum=pUser->m_userBlackMarket.BuyGood(pGoodsInfo->iGoodsID,pGoodsInfo->iNum);
	CT_INT32 iLastNum=mallGoods.iGoodsCount - iBuyNum;
	if(iLastNum >= 0 && iBuyNum > 0)
	{
		CMD_G_MarketGoodsInfo goodsInfo = {0};
		goodsInfo.iMarketType = pGoodsInfo->iMarketType;
		goodsInfo.iGoodsID = pGoodsInfo->iGoodsID;
		goodsInfo.iNum = iLastNum;
		//商品不足直接退出,数量足够用户更新商品数量
		pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_RESERVE_UPDATE,(CT_CHAR*)&goodsInfo, sizeof(CMD_G_MarketGoodsInfo));
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeMarketPersonal iLastNum=%d iBuyNum:%d",iLastNum,iBuyNum);
		pUser->CloseNet();
		return CT_FALSE;
	}
	MSC_LOGDB_ShoppingRecord shoppingRecord = {0};//日志不支持资源类型和数目
	memset(&mvd,0,sizeof(mvd));
	shoppingRecord.iDiamod = mallGoods.iNum;
	G_ChangeResType(mvd,mallGoods.iType,mallGoods.iNum);
	pUser->m_userBase.AddMastData(mvd,SAVETAG_MARKET_MALL);
	pUser->m_userBase.GetMasterData(mvd);
	CMD_G_MarketMaster mm = {0};
	mm.dwUserID = pUser->GetUserID();
	mm.iGold = mvd.stRes.iGold;
	mm.iDiamod = mvd.stRes.iDiamod;
	pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_UPDATE_MASTER,(CT_CHAR*)&mm,sizeof(CMD_G_MarketMaster));
	shoppingRecord.dwUserID = pUser->GetUserID();
	shoppingRecord.iUserGrade = mvd.stGrade.iLevel;
	shoppingRecord.iGoodsID = pGoodsInfo->iGoodsID;
	shoppingRecord.iGoodsNum = pGoodsInfo->iNum;
	shoppingRecord.iMarketType = pGoodsInfo->iMarketType;
	CT_DWORD LogDBThreadID = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBThreadID,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SHOPPING,(CT_CHAR*)&shoppingRecord,sizeof(shoppingRecord));

	//数据库记录
	MSC_DB_MarketPersonal mp={0};
	mp.dwUserID=pUser->GetUserID();
	mp.iGoodsID=pGoodsInfo->iGoodsID;
	mp.iNum=pGoodsInfo->iNum;
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_MARKET_PERSONAL,(CT_CHAR*)&mp,sizeof(mp));

	return CT_TRUE;
}

CT_BOOL CCMarketEvent::DisposeBlackMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo)
{
	//判断黑店功能是否开启
	if (!pUser->m_userBlackMarket.ViewBlackMarket())
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeBlackMarketShopping user %d black market haven't open",pUser->GetUserID());
		pUser->CloseNet();
		return CT_FALSE;
	}
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_MarketBlackGoods	blackGoods = {0};
	CT_INT32 iGoodsID=0;
	CT_INT32 iPosition=pGoodsInfo->iGoodsID;	//黑店物品参数改为位置
	iGoodsID = pUser->m_userBlackMarket.GetBlackMarketGoodsID(iPosition);
	if (pGoodsInfo->iMarketType==MARKET_BLACK_TYPE)
	{
		if (!G_GetMarketBlackGoods(iGoodsID,0,0,&blackGoods))
		{
			pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_DATA_ERROR);
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping G_GetMarketBlackGoods no id %d",iGoodsID);
			return CT_FALSE;
		}
	}
	else
	{
		if (!G_GetMarketBlackPayGoods(iGoodsID,&blackGoods))
		{
			pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_DATA_ERROR);
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping G_GetMarketBlackPayGoods no id %d",iGoodsID);
			return CT_FALSE;
		}
	}
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	//货币消耗钻石
	if (blackGoods.iDiamod<0)
	{
		if (mvd.stRes.iDiamod+(blackGoods.iDiamod*pGoodsInfo->iNum)<0)
		{
			pUser->CloseNet();
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping user %d lack diamod goods %d",pGoodsInfo->dwUserID,iGoodsID);
			return CT_FALSE;
		}
	}
	//货币消耗金币
	else
	{
		if (mvd.stRes.iGold+(blackGoods.iGold*pGoodsInfo->iNum)<0)
		{
			pUser->CloseNet();
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping user %d lack gold goods %d",pGoodsInfo->dwUserID,iGoodsID);
			return CT_FALSE;
		}
	}
	//判断物品是否足量,购买的是普通黑店商品	pGoodsInfo->iGoodsID 是 iPosition
	if (!pUser->m_userBlackMarket.BlackMarketShopping(pGoodsInfo->iGoodsID,pGoodsInfo->iNum,pGoodsInfo->iMarketType))
	{
		return CT_FALSE;
	}
	memset(&mvd,0,sizeof(mvd));
	if (blackGoods.iDiamod<0)
	{
		mvd.stRes.iDiamod = blackGoods.iDiamod*pGoodsInfo->iNum;
	}
	else
	{
		mvd.stRes.iGold = blackGoods.iGold*pGoodsInfo->iNum;
	}
	pUser->m_userBase.AddMastData(mvd,SAVETAG_MARKET_BLACK);
	pUser->m_userBase.GetMasterData(mvd);
	CMD_G_MarketMaster mm = {0};   
	mm.dwUserID = pUser->GetUserID();
	mm.iGold = mvd.stRes.iGold;
	mm.iDiamod = mvd.stRes.iDiamod;
	pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_UPDATE_MASTER,(CT_CHAR*)&mm,sizeof(CMD_G_MarketMaster));
	MSC_LOGDB_ShoppingRecord shoppingRecord = {0};
	shoppingRecord.dwUserID = pUser->GetUserID();
	shoppingRecord.iUserGrade = mvd.stGrade.iLevel;
	shoppingRecord.iGoodsID = iGoodsID;
	shoppingRecord.iGoodsNum = pGoodsInfo->iNum;
	shoppingRecord.iMarketType = pGoodsInfo->iMarketType;
	shoppingRecord.iDiamod = blackGoods.iDiamod*pGoodsInfo->iNum;
	shoppingRecord.iGold = blackGoods.iGold*pGoodsInfo->iNum;
	CT_DWORD LogDBThreadID = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBThreadID,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SHOPPING,(CT_CHAR*)&shoppingRecord,sizeof(shoppingRecord));
	return CT_TRUE;
}

CT_BOOL CCMarketEvent::DisposeSeekBlackMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo)
{
	//判断黑店功能是否开启
	if (!pUser->m_userBlackMarket.ViewBlackMarket())
	{
		CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeBlackMarketShopping user %d black market haven't open",pUser->GetUserID());
		pUser->CloseNet();
		return CT_FALSE;
	}
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_MarketBlackGoods	blackGoods = {0};
	CT_INT32 iGoodsID=0;
	CT_INT32 iPosition=pGoodsInfo->iGoodsID;	//黑店物品参数改为位置
	iGoodsID = pUser->m_userBlackMarket.GetSeekBlackMarketGoodsID(iPosition);
	if (pGoodsInfo->iMarketType==MARKET_SEEKBLACK_TYPE)
	{
		if (!G_GetMarketBlackGoods(iGoodsID,0,0,&blackGoods))
		{
			pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_DATA_ERROR);
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeSeekBlackMarketShopping G_GetMarketBlackGoods no id %d",iGoodsID);
			return CT_FALSE;
		}
	}

	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	//货币消耗钻石
	if (blackGoods.iDiamod<0)
	{
		if (mvd.stRes.iDiamod+(blackGoods.iDiamod*pGoodsInfo->iNum)<0)
		{
			pUser->CloseNet();
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping user %d lack diamod goods %d",pGoodsInfo->dwUserID,iGoodsID);
			return CT_FALSE;
		}
	}
	//货币消耗金币
	else
	{
		if (mvd.stRes.iGold+(blackGoods.iGold*pGoodsInfo->iNum)<0)
		{
			pUser->CloseNet();
			CCLogMeg::getSingleton().WriteError("CCMarketEvent::DisposeShopping user %d lack gold goods %d",pGoodsInfo->dwUserID,iGoodsID);
			return CT_FALSE;
		}
	}
	//判断物品是否足量,购买的是普通黑店商品	pGoodsInfo->iGoodsID 是 iPosition
	if (!pUser->m_userBlackMarket.SeekBlackMarketShopping(pGoodsInfo->iGoodsID,pGoodsInfo->iNum,pGoodsInfo->iMarketType))
	{
		return CT_FALSE;
	}
	memset(&mvd,0,sizeof(mvd));
	if (blackGoods.iDiamod<0)
	{
		mvd.stRes.iDiamod = blackGoods.iDiamod*pGoodsInfo->iNum;
	}
	else
	{
		mvd.stRes.iGold = blackGoods.iGold*pGoodsInfo->iNum;
	}
	pUser->m_userBase.AddMastData(mvd,SAVATAG_SEEK_BLACK);
	pUser->m_userBase.GetMasterData(mvd);
	CMD_G_MarketMaster mm = {0};   
	mm.dwUserID = pUser->GetUserID();
	mm.iGold = mvd.stRes.iGold;
	mm.iDiamod = mvd.stRes.iDiamod;
	pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_UPDATE_MASTER,(CT_CHAR*)&mm,sizeof(CMD_G_MarketMaster));
	MSC_LOGDB_ShoppingRecord shoppingRecord = {0};
	shoppingRecord.dwUserID = pUser->GetUserID();
	shoppingRecord.iUserGrade = mvd.stGrade.iLevel;
	shoppingRecord.iGoodsID = iGoodsID;
	shoppingRecord.iGoodsNum = pGoodsInfo->iNum;
	shoppingRecord.iMarketType = pGoodsInfo->iMarketType;
	shoppingRecord.iDiamod = blackGoods.iDiamod*pGoodsInfo->iNum;
	shoppingRecord.iGold = blackGoods.iGold*pGoodsInfo->iNum;
	CT_DWORD LogDBThreadID = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(LogDBThreadID,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SHOPPING,(CT_CHAR*)&shoppingRecord,sizeof(shoppingRecord));
	return CT_TRUE;
}