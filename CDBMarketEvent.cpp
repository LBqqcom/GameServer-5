#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBMarketEvent.h"
#include "CMarket.h"
#include "CDBLogMes.h"

#define MAX_BUFFER		sizeof(G_MarketMallGoods)*100

CCDBMarketEvent::CCDBMarketEvent(CCGameMesManager* pMesManager, CT_DWORD id, 
								 CCDBConnector& dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_MARKET_RECORD_DATA);
	RegistMessage(MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_BLACK_MARKET_REFRESH);
	RegistMessage(MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_BLACK_MARKET_SHOPPING);
	RegistMessage(MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_MARKET_ADD);
	RegistMessage(MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_MARKET_PERSONAL);
	SetTimer(IDI_MARKET_PERSONAL,20*1000/*TIME_MARKET_PERSONAL*/);
	SetTimer(IDI_MARKET_PERSONAL_UPDATA,TIME_MARKET_PERSONAL_UPDATA);
	RegistTimer(IDI_MARKET_PERSONAL);
	RegistTimer(IDI_MARKET_PERSONAL_UPDATA);
}

CT_VOID CCDBMarketEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_DB_MARKET_DATA:
		{
			if (subID==MSC_SUB_DB_MARKET_RECORD_DATA)
			{
				/*if (len!=sizeof(MSC_DB_ShoppingRecord))
				{
					CCLogMeg::getSingleton().WriteError("CCDBMarketEvent::OnUserEvent len!=ShoppingRecord");
					return;
				}
				MSC_DB_ShoppingRecord *p=(MSC_DB_ShoppingRecord*)pBuf;
				UpdateMarketShoppingData(*p);*/
			}
			else if (subID==MSC_SUB_DB_BLACK_MARKET_REFRESH)
			{
				if (len!=sizeof(MSC_DB_BlackMarketRefresh))
				{
					CCLogMeg::getSingleton().WriteError("CCDBMarketEvent::OnUserEvent len!=MSC_DB_BlackMarketRefresh");
					return;
				}
				MSC_DB_BlackMarketRefresh *p=(MSC_DB_BlackMarketRefresh*)pBuf;
				RefreshBlackMarketDB(*p);
			}
			else if (subID==MSC_SUB_DB_BLACK_MARKET_SHOPPING)
			{
				if (len!=sizeof(MSC_DB_BlackMarketShopping))
				{
					CCLogMeg::getSingleton().WriteError("CCDBMarketEvent::OnUserEvent len!=MSC_DB_BlackMarketShopping");
					return;
				}
				MSC_DB_BlackMarketShopping *p = (MSC_DB_BlackMarketShopping*)pBuf;
				BlackMarketShopping(*p);
			}
			else if (subID==MSC_SUB_DB_MARKET_ADD)
			{
				if (len!=sizeof(MSC_DB_MarketFunction))
				{
					CCLogMeg::getSingleton().WriteError("CCDBMarketEvent::OnUserEvent len!=MSC_DB_MarketFunction");
					return;
				}
				MSC_DB_MarketFunction *p = (MSC_DB_MarketFunction*)pBuf;
				UpdateMarketFunction(*p);
			}
			else if(subID == MSC_SUB_DB_MARKET_PERSONAL)
			{
				DisposeMarketPersonal(pBuf,len);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBMarketEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCDBMarketEvent::OnTime(CT_DWORD timeID)
{
	if(timeID == IDI_MARKET_PERSONAL)
	{
		KillTimer(timeID);
		UpdataMarketPersonal();
	}
	else if(timeID == IDI_MARKET_PERSONAL_UPDATA)
	{
		UpdataMarketPersonal();
	}
	return;
}
//更新个人限购
CT_VOID CCDBMarketEvent::UpdataMarketPersonal()
{
	CT_CHAR cBuf[MAX_BUFFER]={0};
	CT_INT32 iCount=0;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MarketPersonalRef");

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while(pResult->MoveNextRow())
			{
				MarketPersonalAll mpa={0};
				pResult->GetValue(0,mpa.iGoodsID);
				pResult->GetValue(1,mpa.iGoodsCount);
				pResult->GetValue(2,mpa.iType);
				pResult->GetValue(3,mpa.iNum);
				pResult->GetValue(4,mpa.iGoodsStatus);
				
				if((iCount+1)*sizeof(MarketPersonalAll) < MAX_BUFFER)
				{
					memcpy(cBuf+iCount*sizeof(MarketPersonalAll),&mpa,sizeof(MarketPersonalAll));
					iCount++;
				}
				else
					CCLogMeg::getSingleton().WriteError("CCDBMarketEvent::UpdataMarketPersonal iCount*sizeof(MarketPersonalAll) > MAX_BUFFER");
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_MARKETDATA, MSC_SUB_UPDATE_PERSON, cBuf,iCount*sizeof(MarketPersonalAll));
	return;
}
//CT_VOID CCDBMarketEvent::UpdateMarketShoppingData(MSC_DB_ShoppingRecord &shoppingRecord)
//{
//	m_dbConnector.Reset();
//	m_dbConnector.SetPcName("SP_ShopAddBuyRecord");
//	m_dbConnector.AddParm("_uid",shoppingRecord.dwUserID);
//	m_dbConnector.AddParm("_propid",shoppingRecord.iGoodsID);
//	m_dbConnector.AddParm("_propnum",shoppingRecord.iGoodsNum);
//	m_dbConnector.AddParm("_diamod",shoppingRecord.iDiamod);
//	m_dbConnector.AddParm("_money",shoppingRecord.iGold);
//	m_dbConnector.AddParm("_propused",shoppingRecord.iItemID);
//	m_dbConnector.AddParm("_propusednum",shoppingRecord.iItemNum);
//	m_dbConnector.AddParm("shoptype)",shoppingRecord.iMarketType);
//
//	if(m_dbConnector.Exec())
//	{
//		CCDBResult* pResult = m_dbConnector.GetResult();
//		//pResult->GetResult();
//		pResult->Release();
//	}else
//	{
//		CCDBLogMes::getSingleton().WriteInfo(shoppingRecord.dwUserID,"SP_ShopAddBuyRecord","_uid:%d _propid:%d _propnum:%d _diamod:%d _money:%d _propused:%d _propusednum:%d shoptype:%d",\
//											shoppingRecord.dwUserID,shoppingRecord.iGoodsID,shoppingRecord.iGoodsNum,shoppingRecord.iDiamod,shoppingRecord.iGold,shoppingRecord.iItemID,shoppingRecord.iItemNum,shoppingRecord.iMarketType);
//		G_DBError(m_dbConnector, m_dwThread);
//		return;
//	}
//}

CT_VOID CCDBMarketEvent::RefreshBlackMarketDB(MSC_DB_BlackMarketRefresh &reBlackMarket)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ShopSysBlackMarket");
	m_dbConnector.AddParm("_uid",reBlackMarket.dwUserID);
	m_dbConnector.AddParm("_refreshTimes",reBlackMarket.iTime);
	m_dbConnector.AddParm("_prop0",reBlackMarket.iGoodsID1);
	m_dbConnector.AddParm("_prop0num",reBlackMarket.iNum1);
	m_dbConnector.AddParm("_prop1",reBlackMarket.iGoodsID2);
	m_dbConnector.AddParm("_prop1num",reBlackMarket.iNum2);
	m_dbConnector.AddParm("_prop2",reBlackMarket.iGoodsID3);
	m_dbConnector.AddParm("_prop2num",reBlackMarket.iNum3);
	m_dbConnector.AddParm("_prop3",reBlackMarket.iGoodsID4);
	m_dbConnector.AddParm("_prop3num",reBlackMarket.iNum4);
	m_dbConnector.AddParm("_prop4",reBlackMarket.iGoodsID5);
	m_dbConnector.AddParm("_prop4num",reBlackMarket.iNum5);
	m_dbConnector.AddParm("_prop5",reBlackMarket.iGoodsID6);
	m_dbConnector.AddParm("_prop5num",reBlackMarket.iNum6);
	m_dbConnector.AddParm("_lastreftime",reBlackMarket.iLastRefTime);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(reBlackMarket.dwUserID,"SP_ShopSysBlackMarket","_uid:%d _refreshTimes:%d _prop0:%d _prop0num:%d _prop1:%d _prop1num:%d _prop2:%d _prop2num:%d _prop3:%d _prop3num:%d _prop4:%d _prop4num:%d _prop5:%d _prop5num:%d",\
												reBlackMarket.dwUserID,reBlackMarket.iTime,reBlackMarket.iGoodsID1,reBlackMarket.iNum1,reBlackMarket.iGoodsID2,reBlackMarket.iNum2,reBlackMarket.iGoodsID3,reBlackMarket.iNum3,reBlackMarket.iGoodsID4,reBlackMarket.iNum4,\
												reBlackMarket.iGoodsID5,reBlackMarket.iNum5,reBlackMarket.iGoodsID6,reBlackMarket.iNum6);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBMarketEvent::BlackMarketShopping(MSC_DB_BlackMarketShopping &blackShopping)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ShopUpdateBlackMarket");
	m_dbConnector.AddParm("_uid",blackShopping.dwUserID);
//	m_dbConnector.AddParm("_refreshTimes",blackShopping.iTime);	zhoujun	购买时不需要更新刷新次数
	m_dbConnector.AddParm("_prop0num",blackShopping.iNumList[0]);
	m_dbConnector.AddParm("_prop1num",blackShopping.iNumList[1]);
	m_dbConnector.AddParm("_prop2num",blackShopping.iNumList[2]);
	m_dbConnector.AddParm("_prop3num",blackShopping.iNumList[3]);
	m_dbConnector.AddParm("_prop4num",blackShopping.iNumList[4]);
	m_dbConnector.AddParm("_prop5num",blackShopping.iNumList[5]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
//		CCDBLogMes::getSingleton().WriteInfo(blackShopping.dwUserID,"SP_ShopUpdateBlackMarket","_uid:%d _refreshTimes:%d _prop0num:%d _prop1num:%d _prop2num:%d _prop3num:%d _prop4num:%d _prop5num:%d",\
//									blackShopping.dwUserID,blackShopping.iTime,blackShopping.iNumList[0],blackShopping.iNumList[1],blackShopping.iNumList[2],blackShopping.iNumList[3],blackShopping.iNumList[4],blackShopping.iNumList[5]);
		CCDBLogMes::getSingleton().WriteInfo(blackShopping.dwUserID,"SP_ShopUpdateBlackMarket","_uid:%d  _prop0num:%d _prop1num:%d _prop2num:%d _prop3num:%d _prop4num:%d _prop5num:%d",\
									blackShopping.dwUserID,blackShopping.iNumList[0],blackShopping.iNumList[1],blackShopping.iNumList[2],blackShopping.iNumList[3],blackShopping.iNumList[4],blackShopping.iNumList[5]);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


CT_VOID CCDBMarketEvent::UpdateMarketFunction(MSC_DB_MarketFunction &marketFunc)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ShopUpdateShopStatus");
	m_dbConnector.AddParm("_uid",marketFunc.dwUserID);
	m_dbConnector.AddParm("_bmall",marketFunc.iMall);
	m_dbConnector.AddParm("_bblackmarket",marketFunc.iBlackMarket);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(marketFunc.dwUserID,"SP_ShopUpdateShopStatus","_uid:%d _bmall:%d _bblackmarket:%d",marketFunc.dwUserID,marketFunc.iMall,marketFunc.iBlackMarket);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBMarketEvent::DisposeMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_MarketPersonal))
	{
		CCLogMeg::getSingleton().WriteError("CCDBMarketEvent::DisposeMarketPersonal iLen != sizeof(MSC_DB_MarketPersonal)");
		return;
	}

	MSC_DB_MarketPersonal *pMp=(MSC_DB_MarketPersonal*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MarketPersonalUpdate");
	m_dbConnector.AddParm("_uid",pMp->dwUserID);
	m_dbConnector.AddParm("_Goods",pMp->iGoodsID);
	m_dbConnector.AddParm("_num",pMp->iNum);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	return;
}