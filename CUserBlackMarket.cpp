#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CMasterScriptSetting.h"
#include "CBuildingScriptSetting.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "GlobeScriptInterface.h"
#include "CUserBlackMarket.h"
#include "CMarket.h"

#define  MAX_MES_MAX_BUFF_LEN   (1024*5) //这个值不可以超过10k

CCUserBlackMarket::CCUserBlackMarket()
{
	iMall=0;
	iBlackMarket=0;
	iRefreshTime=0;
	m_blackGoodsStatusMap.clear();
	m_MarketPersonalMap.clear();
}

CCUserBlackMarket::~CCUserBlackMarket()
{
	m_blackGoodsStatusMap.clear();
	m_MarketPersonalMap.clear();
}

//根据随机数，获取物品id
CT_INT32 CCUserBlackMarket::GetPosByRandNum(GoodsVector &v_weight,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand)
{
	if (begin>=end)
	{
		return begin;
	}
	CT_INT32 mid = (begin+end)/2;
	if (v_weight[mid]>=iRand)
	{
		return GetPosByRandNum(v_weight,begin,mid,iRand);
	}
	else
	{
		return GetPosByRandNum(v_weight,mid+1,end,iRand);
	}
}

//每次获取物品后需要重新计算权重
//CT_VOID	CCUserBlackMarket::GetGoodsWeightVector(GoodsVector &v_id,GoodsVector &v_weight,CT_INT32 iGoodsIDList[],CT_INT32 iGoodsChoosed[],CT_INT32 &iWeight,CT_INT32 iType)
//{
//	_AUTOMUTEX(m_pMutex);
//	v_id.clear();
//	v_weight.clear();
//	iWeight=0;
//	G_MarketBlackGoods blackGoods = {0};
//	CT_INT32 i=0;
//	while(i<MARKET_GOODS_NUM)
//	{
//		if (!iGoodsIDList[i])
//		{
//			break;
//		}
//		for (CT_INT32 j=0;j<4&&iGoodsChoosed[j]!=0;j++)
//		{
//			if (iGoodsIDList[i]==iGoodsChoosed[j])
//			{
//				goto NEXT;
//			}
//		}
//		memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
//		/*if (iType==MARKET_BLACK_TYPE)
//		{
//			CCMarket::getSingleton().GetMarketBlackGoods(iGoodsIDList[i],&blackGoods);
//		}
//		else
//		{
//			CCMarket::getSingleton().GetMarketBlackPayGoods(iGoodsIDList[i],&blackGoods);
//		}*/
//		CCMarket::getSingleton().GetMarketBlackGoods(iGoodsIDList[i],&blackGoods);
//		//物品id和权重，生成两个vector，进行二分查找
//		v_id.push_back(iGoodsIDList[i]);
//		iWeight+=blackGoods.iWeight;
//		v_weight.push_back(iWeight);
//NEXT:
//		i++;
//	}
//}

CT_VOID	CCUserBlackMarket::GetGoodsWeightVector(GoodsVector &v_id,GoodsVector &v_weight,CT_INT32 iGoodsIDList[],CT_INT32 &iWeight,CT_INT32 iNum,CT_INT32 iGrade)
{
	_AUTOMUTEX(m_pMutex);
	v_id.clear();
	v_weight.clear();
	iWeight=0;
	G_MarketBlackGoods blackGoods = {0};
	CT_INT32 i=0;
	while(i<MARKET_GOODS_NUM)
	{
		if (!iGoodsIDList[i])
		{
			break;
		}
		memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
		/*if (iType==MARKET_BLACK_TYPE)
		{
			CCMarket::getSingleton().GetMarketBlackGoods(iGoodsIDList[i],&blackGoods);
		}
		else
		{
			CCMarket::getSingleton().GetMarketBlackPayGoods(iGoodsIDList[i],&blackGoods);
		}*/
		CCMarket::getSingleton().GetMarketBlackGoods(iGoodsIDList[i],iNum,iGrade,&blackGoods);
		//物品id和权重，生成两个vector，进行二分查找
		v_id.push_back(iGoodsIDList[i]);
		iWeight+=blackGoods.iWeight;
		v_weight.push_back(iWeight);
		i++;
	}
}

//插入个人限购
CT_VOID CCUserBlackMarket::InsertMarketPersonal(const CT_INT32 iGoodsID,const CT_INT32 iNum)
{
	_AUTOMUTEX(m_pMutex);

	MarketPersonal mp={0};
	mp.iGoodsID=iGoodsID;
	mp.iNum=iNum;
	m_MarketPersonalMap.insert(MarketPersonalMap::value_type(mp.iGoodsID,mp));
	return;
}

//获取个人限购
CT_BOOL CCUserBlackMarket::GetMarketPersonal(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);
	if(iLen < sizeof(MarketPersonal)*m_MarketPersonalMap.size())
	{
		CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::GetMarketPersonal iLen < sizeof(MarketPersonal)*m_MarketPersonalMap.size()");
		return CT_FALSE;
	}


	MarketPersonalAll mpa={0};
	if(CCMarket::getSingleton().GetMarketPersonalGoods(pBuf,iLen))
	{
		for(CT_INT32 i=0;i<iLen/sizeof(CMD_G_MarketPersonalInfo);++i)
		{
			CMD_G_MarketPersonalInfo *pMp=(CMD_G_MarketPersonalInfo*)(pBuf+sizeof(CMD_G_MarketPersonalInfo)*i);
			//memcpy(&mp,pBuf+sizeof(CMD_G_MarketPersonalInfo)*i,sizeof(CMD_G_MarketPersonalInfo));

			MP_IT it=m_MarketPersonalMap.find(pMp->iGoodsID);
			if(it != m_MarketPersonalMap.end())
			{
				pMp->iGoodsCount=pMp->iGoodsCount-(*it).second.iNum > 0?pMp->iGoodsCount-(*it).second.iNum:0;
			}
		}
	}

	return CT_TRUE;
}

//更新个人限购
CT_VOID CCUserBlackMarket::UpdataMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex);
	if(iLen%sizeof(MarketPersonalAll) != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::UpdataMarketPersonal iLen%sizeof(MarketPersonalAll) != 0");
		return;
	}

	for(CT_INT32 i=0;i<iLen/sizeof(MarketPersonalAll);++i)
	{
		MarketPersonalAll mpa={0};
		memcpy(&mpa,pBuf+sizeof(MarketPersonalAll)*i,sizeof(MarketPersonalAll));

		MP_IT it=m_MarketPersonalMap.find(mpa.iGoodsID);
		if(it == m_MarketPersonalMap.end())
		{
			MarketPersonal mp={0};
			mp.iGoodsID=mpa.iGoodsID;
			m_MarketPersonalMap.insert(MarketPersonalMap::value_type(mp.iGoodsID,mp));
		}
		else
		{
			if(mpa.iGoodsStatus == GOODS_STATUS_DELETE)
			{
				m_MarketPersonalMap.erase(it);
			}
		}
	}

	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iSendLen=MAX_MES_MAX_BUFF_LEN;
	GetMarketPersonal(cBuf,iSendLen);
	m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_PARSONAL,cBuf,iSendLen);
	return;
}

//购买物品
CT_INT32 CCUserBlackMarket::BuyGood(CT_INT32 iGoods,const CT_INT32 iNum)
{
	_AUTOMUTEX(m_pMutex);
	if(iNum < 0)return -1;

	MP_IT mpit=m_MarketPersonalMap.find(iGoods);
	if(mpit != m_MarketPersonalMap.end())
	{
		(*mpit).second.iNum+=iNum;
	}
	else
	{
		MarketPersonal mp={0};
		mp.iGoodsID=iGoods;
		mp.iNum=iNum;
		m_MarketPersonalMap.insert(MarketPersonalMap::value_type(mp.iGoodsID,mp));
		return mp.iNum; 
	}
	return (*mpit).second.iNum;
}

//这里iNum修改为黑店物品位置
//CT_VOID CCUserBlackMarket::RandGoodsBlackMarket(CT_INT32 iType,CT_INT32 iChooseGoods[],CT_INT32 iNum)
//{
//	_AUTOMUTEX(m_pMutex);
//	CT_INT32		iGoodsList[MARKET_GOODS_NUM];
//	memset(iGoodsList,0,sizeof(CT_INT32)*MARKET_GOODS_NUM);
//	/*if (iType==MARKET_BLACK_TYPE)
//	{
//		CCMarket::getSingleton().GetMarketBlackList(iGoodsList);
//	}
//	else
//	{
//		CCMarket::getSingleton().GetMarketBlackPayList(iGoodsList);
//	}*/
//	CCMarket::getSingleton().GetMarketBlackList(iGoodsList,iNum);
//	CT_INT32 iWeight=0;
//	CT_INT32 iChooseNum=0;
//
//	GetGoodsWeightVector(m_GoodsIDVector,m_GoodsWeightVector,iGoodsList,iChooseGoods,iWeight,iType);
//	while(iChooseNum<iNum)
//	{
//		除法要增加对0的判断
//		if (iWeight<=0)
//		{
//			CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::RandGoodsBlackMarket iWeight %d no goods to rand",iWeight);
//			return;
//		}
//		CT_INT32  iRandNum = rand()%iWeight + 1;
//		CT_INT32	iPos = GetPosByRandNum(m_GoodsWeightVector,0,m_GoodsWeightVector.size(),iRandNum);
//		for (CT_INT32 j=0;j<iNum;j++)
//		{
//			if (iChooseGoods[j]==m_GoodsIDVector[iPos])
//			{
//				break;
//			}
//			if (iChooseGoods[j]==0)
//			{
//				iChooseGoods[j] = m_GoodsIDVector[iPos];
//				iChooseNum+=1;
//				GetGoodsWeightVector(m_GoodsIDVector,m_GoodsWeightVector,iGoodsList,iChooseGoods,iWeight,iType);
//				break;
//			}
//		}
//	}
//}

//这里iNum修改为黑店物品位置
CT_INT32 CCUserBlackMarket::RandGoodsBlackMarket(CT_INT32 iNum)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32		iGoodsList[MARKET_GOODS_NUM];
	memset(iGoodsList,0,sizeof(CT_INT32)*MARKET_GOODS_NUM);

	CT_INT32 iGrade=0;
	MasterData masterData = {0};
	m_pUser->m_userBase.GetMasterData(masterData);
	iGrade = masterData.mvd.stGrade.iLevel;
	CCMarket::getSingleton().GetMarketBlackList(iGoodsList,iNum, iGrade);
	CT_INT32 iWeight=0;
	CT_INT32 iChooseNum=0;

	GetGoodsWeightVector(m_GoodsIDVector,m_GoodsWeightVector,iGoodsList,iWeight,iNum,iGrade);
	//除法要增加对0的判断
	if (iWeight<=0)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::RandGoodsBlackMarket iWeight %d no goods to rand",iWeight);
		return 0;
	}
	CT_INT32  iRandNum = rand()%iWeight + 1;
	CT_INT32  iPos = GetPosByRandNum(m_GoodsWeightVector,0,m_GoodsWeightVector.size(),iRandNum);		
	return m_GoodsIDVector[iPos];
}

CT_VOID CCUserBlackMarket::RefreshBlackMarket(CT_INT32 iType)
{
	G_MarketBlackGoods blackGoods = {0};
	_AUTOMUTEX(m_pMutex);
	/*CT_INT32   iChooseGoods[MARKET_BLACKNUM_SYS],iChooseGoodsPay[MARKET_BLACKNUM_PAY];
	memset(iChooseGoods,0,sizeof(iChooseGoods));
	RandGoodsBlackMarket(MARKET_BLACK_TYPE,iChooseGoods,MARKET_BLACKNUM_SYS);*/
	CMD_G_MarketRefreshBlack marketRefreshBlack = {0};
	marketRefreshBlack.dwUserID=m_pUser->GetUserID();
	marketRefreshBlack.iNum = iRefreshTime;
	marketRefreshBlack.iGoodsID1 = RandGoodsBlackMarket(1);
	marketRefreshBlack.iGoodsID2 = RandGoodsBlackMarket(2);
	marketRefreshBlack.iGoodsID3 = RandGoodsBlackMarket(3);
	marketRefreshBlack.iGoodsID4 = RandGoodsBlackMarket(4);
	//付费刷新，增加付费物品刷新
	if (iType==MARKET_BLACK_PAY_TYPE)
	{
		marketRefreshBlack.iGoodsID5 = RandGoodsBlackMarket(5);
		marketRefreshBlack.iGoodsID6 = RandGoodsBlackMarket(6);
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_REFRESH_BLACK,(CT_CHAR*)&marketRefreshBlack,sizeof(marketRefreshBlack));
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	
	BlackGoodsStatus goodsStatus;

	MSC_DB_BlackMarketRefresh blackMarketRe = {0};
	blackMarketRe.dwUserID = m_pUser->GetUserID();
	blackMarketRe.iTime = iRefreshTime;

	blackMarketRe.iGoodsID1 = marketRefreshBlack.iGoodsID1;
	memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
	CCMarket::getSingleton().GetMarketBlackGoods(blackMarketRe.iGoodsID1,0,0,&blackGoods);
	blackMarketRe.iNum1 = blackGoods.iOnceNum;
	//用户内存中数据更新
	memset(&goodsStatus,0,sizeof(BlackGoodsStatus));
	goodsStatus.iGoodsID = blackMarketRe.iGoodsID1;
	goodsStatus.iNum = blackMarketRe.iNum1;
	m_blackGoodsStatusMap[1] = goodsStatus;

	blackMarketRe.iGoodsID2 = marketRefreshBlack.iGoodsID2;
	memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
	CCMarket::getSingleton().GetMarketBlackGoods(blackMarketRe.iGoodsID2,0,0,&blackGoods);
	blackMarketRe.iNum2 = blackGoods.iOnceNum;
	//用户内存中数据更新
	memset(&goodsStatus,0,sizeof(BlackGoodsStatus));
	goodsStatus.iGoodsID = blackMarketRe.iGoodsID2;
	goodsStatus.iNum = blackMarketRe.iNum2;
	m_blackGoodsStatusMap[2] = goodsStatus;

	blackMarketRe.iGoodsID3 = marketRefreshBlack.iGoodsID3;
	memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
	CCMarket::getSingleton().GetMarketBlackGoods(blackMarketRe.iGoodsID3,0,0,&blackGoods);
	blackMarketRe.iNum3 = blackGoods.iOnceNum;
	//用户内存中数据更新
	memset(&goodsStatus,0,sizeof(BlackGoodsStatus));
	goodsStatus.iGoodsID = blackMarketRe.iGoodsID3;
	goodsStatus.iNum = blackMarketRe.iNum3;
	m_blackGoodsStatusMap[3] = goodsStatus;

	blackMarketRe.iGoodsID4 = marketRefreshBlack.iGoodsID4;
	memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
	CCMarket::getSingleton().GetMarketBlackGoods(blackMarketRe.iGoodsID4,0,0,&blackGoods);
	blackMarketRe.iNum4 = blackGoods.iOnceNum;
	//用户内存中数据更新
	memset(&goodsStatus,0,sizeof(BlackGoodsStatus));
	goodsStatus.iGoodsID = blackMarketRe.iGoodsID4;
	goodsStatus.iNum = blackMarketRe.iNum4;
	m_blackGoodsStatusMap[4] = goodsStatus;

	//如果是付费刷新，更新5,6商品，否则把内存中数据传入
	if (iType==MARKET_BLACK_PAY_TYPE)
	{
		blackMarketRe.iGoodsID5 = marketRefreshBlack.iGoodsID5;
		memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
		CCMarket::getSingleton().GetMarketBlackGoods(blackMarketRe.iGoodsID5,0,0,&blackGoods);
		blackMarketRe.iNum5 = blackGoods.iOnceNum;
		//用户内存中数据更新
		memset(&goodsStatus,0,sizeof(BlackGoodsStatus));
		goodsStatus.iGoodsID = blackMarketRe.iGoodsID5;
		goodsStatus.iNum = blackMarketRe.iNum5;
		m_blackGoodsStatusMap[5] = goodsStatus;

		blackMarketRe.iGoodsID6 = marketRefreshBlack.iGoodsID6;
		memset(&blackGoods,0,sizeof(G_MarketBlackGoods));
		CCMarket::getSingleton().GetMarketBlackGoods(blackMarketRe.iGoodsID6,0,0,&blackGoods);
		blackMarketRe.iNum6 = blackGoods.iOnceNum;
		//用户内存中数据更新
		memset(&goodsStatus,0,sizeof(BlackGoodsStatus));
		goodsStatus.iGoodsID = blackMarketRe.iGoodsID6;
		goodsStatus.iNum = blackMarketRe.iNum6;
		m_blackGoodsStatusMap[6] = goodsStatus;
	}
	//系统刷新，将内存数据读入
	else
	{
		BlackGoodsStatusMap_IT it = m_blackGoodsStatusMap.find(MARKET_BLACKNUM_SYS+1);
		if (it!=m_blackGoodsStatusMap.end())
		{
			blackMarketRe.iGoodsID5 = m_blackGoodsStatusMap[5].iGoodsID;
			blackMarketRe.iNum5 = m_blackGoodsStatusMap[5].iNum;
			blackMarketRe.iGoodsID6 = m_blackGoodsStatusMap[6].iGoodsID;
			blackMarketRe.iNum6 = m_blackGoodsStatusMap[6].iNum;
		}
		iLastRefTime =  time(0);	//zhoujun
		/*
		G_ValueRange vr={0};
		if (G_GetValueRange(VALUE_TYPE_MARKET_BLACKREFRESHTIME_1,&vr))
		{
			CT_INT32 iRefTime = (CT_INT32)vr.fMaxVal;
			CT_INT32 iNum=((CT_INT32)time(0)-iLastRefTime)/iRefTime;
			iLastRefTime+=iRefTime*iNum;
		} */
	}
	blackMarketRe.iLastRefTime = iLastRefTime;

	G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_BLACK_MARKET_REFRESH,(CT_CHAR*)&blackMarketRe,sizeof(MSC_DB_BlackMarketRefresh));
}

CT_BOOL	CCUserBlackMarket::BlackMarketShopping(CT_INT32 iPosition,CT_INT32 iNum,CT_INT32 iMarketType)
{
	_AUTOMUTEX(m_pMutex);
	BlackGoodsStatusMap_IT it = m_blackGoodsStatusMap.begin();
	for(;it!=m_blackGoodsStatusMap.end();it++)
	{
		if (it->first==iPosition)	//黑店购买参数由物品ID改为位置
		{
			if (it->second.iNum<iNum)
			{
				CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::BlackMarketShopping user %d igoods %d num lack",m_pUser->GetUserID(),it->second.iGoodsID);
				return CT_FALSE;
			}
			it->second.iNum-=iNum;
			CT_DWORD DBthreadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			MSC_DB_BlackMarketShopping	blackMarketShopping;
			memset(&blackMarketShopping,0,sizeof(MSC_DB_BlackMarketShopping));
			blackMarketShopping.dwUserID=m_pUser->GetUserID();
//			blackMarketShopping.iTime = 0;	zhoujun 购买时不需要更新刷新次数
			blackMarketShopping.iNumList[it->first-1] = -iNum;
			G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_BLACK_MARKET_SHOPPING,(CT_CHAR*)&blackMarketShopping,sizeof(MSC_DB_BlackMarketShopping));
			//更新客户端商品数量
			CMD_G_MarketGoodsInfo goodsInfo = {0};
			if(it->first)
			goodsInfo.iMarketType = iMarketType;
			goodsInfo.iGoodsID = iPosition;		//改为物品位置
			goodsInfo.iNum = it->second.iNum;
			m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_RESERVE_UPDATE,(CT_CHAR*)&goodsInfo, sizeof(CMD_G_MarketGoodsInfo));
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//探访黑店购买
CT_BOOL CCUserBlackMarket::SeekBlackMarketShopping(CT_INT32 iPosition,CT_INT32 iNum,CT_INT32 iMarketType)
{
	_AUTOMUTEX(m_pMutex);
	BlackGoodsStatusMap_IT it = m_blackGoodsSeekMap.begin();
	for(;it!=m_blackGoodsSeekMap.end();it++)
	{
		if (it->first==iPosition)	//黑店购买参数由物品ID改为位置
		{
			if (it->second.iNum<iNum)
			{
				CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::SeekBlackMarketShopping user %d igoods %d num lack",m_pUser->GetUserID(),it->second.iGoodsID);
				return CT_FALSE;
			}
			it->second.iNum-=iNum;

			//更新客户端商品数量
			CMD_G_MarketGoodsInfo goodsInfo = {0};
			if(it->first)
				goodsInfo.iMarketType = iMarketType;
			goodsInfo.iGoodsID = iPosition;		//改为物品位置
			goodsInfo.iNum = it->second.iNum;
			m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_RESERVE_UPDATE,(CT_CHAR*)&goodsInfo, sizeof(CMD_G_MarketGoodsInfo));
			return CT_TRUE;
		}
	}

	return CT_FALSE;
}

CT_INT32 CCUserBlackMarket::GetBlackMarketGoodsID(CT_INT32 iPosition)
{
	_AUTOMUTEX(m_pMutex);
	BlackGoodsStatusMap_IT it = m_blackGoodsStatusMap.find(iPosition);
	if (it!=m_blackGoodsStatusMap.end())
	{
		return it->second.iGoodsID;
	}
	return 0;
}

CT_INT32 CCUserBlackMarket::GetSeekBlackMarketGoodsID(CT_INT32 iPosition)
{
	_AUTOMUTEX(m_pMutex);
	BlackGoodsStatusMap_IT it = m_blackGoodsSeekMap.find(iPosition);
	if (it!=m_blackGoodsSeekMap.end())
	{
		return it->second.iGoodsID;
	}
	return 0;
}

CT_BOOL CCUserBlackMarket::RefreshOnce()
{
	_AUTOMUTEX(m_pMutex);
	if (iRefreshTime<=0)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::RefreshOnce no refresh time");
		return CT_FALSE;
	}
	G_ValueRange vr = {0};
	if(G_GetValueRange(VALUE_TYPE_BALCK_REFRESH_DIAMOD,  &vr))
	{
		//已刷新次数，计算公式待定
		CT_INT32	iDiamodNeed = (CT_INT32)vr.fMaxVal;
		MasterVariableData mvd = {0};
		m_pUser->m_userBase.GetMasterData(mvd);
		if (mvd.stRes.iDiamod+iDiamodNeed<0)
		{
			CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::RefreshOnce no diamod");
			return CT_FALSE;
		}
		memset(&mvd,0,sizeof(mvd));
		mvd.stRes.iDiamod=iDiamodNeed;
		m_pUser->m_userBase.AddMastData(mvd,SAVETAG_MARKET_BLACK_REFRESH);
		m_pUser->m_userBase.GetMasterData(mvd);
		CMD_G_MarketMaster mm = {0};
		mm.dwUserID = m_pUser->GetUserID();
		mm.iGold = mvd.stRes.iGold;
		mm.iDiamod = mvd.stRes.iDiamod;
		m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_UPDATE_MASTER,(CT_CHAR*)&mm,sizeof(CMD_G_MarketMaster));
		//更新刷新次数
//		CT_DWORD DBthreadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
//		MSC_DB_BlackMarketShopping	blackMarketShopping;
//		memset(&blackMarketShopping,0,sizeof(MSC_DB_BlackMarketShopping));
//		blackMarketShopping.dwUserID=m_pUser->GetUserID();
//		blackMarketShopping.iTime = -1;

		//黑店刷新不限次数
	//	iRefreshTime-=1;
	//	G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_BLACK_MARKET_SHOPPING,(CT_CHAR*)&blackMarketShopping,sizeof(MSC_DB_BlackMarketShopping));
		return CT_TRUE;
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::RefreshOnce G_GetValueRange(VALUE_TYPE_BALCK_REFRESH_DIAMOD error");
		m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_DATA_ERROR);
		return CT_FALSE;
	}
	return CT_FALSE;
}

CT_BOOL	CCUserBlackMarket::ViewBlackMarket()
{
	_AUTOMUTEX(m_pMutex);
	return 0!=iBlackMarket;
}

CT_BOOL CCUserBlackMarket::ViewMallMarket()
{
	_AUTOMUTEX(m_pMutex);
	return 0!=iMall;
}

CT_VOID CCUserBlackMarket::SendBlackMarketInfo()
{
	CT_CHAR buf[MAX_MES_MAX_BUFF_LEN];
	CT_INT32 iLen=0;
	memset(buf,0,MAX_MES_MAX_BUFF_LEN);
	CMD_G_MarketGoodsInfo goodsInfo;
	BlackGoodsStatusMap_IT it = m_blackGoodsStatusMap.begin();
	while(it!=m_blackGoodsStatusMap.end())
	{
		memset(&goodsInfo,0,sizeof(CMD_G_MarketGoodsInfo));
		if (it->first>MARKET_BLACKNUM_SYS)
		{
			goodsInfo.iMarketType = MARKET_BLACK_PAY_TYPE;
		}
		else
		{
			goodsInfo.iMarketType = MARKET_BLACK_TYPE;
		}
		goodsInfo.iGoodsID = it->second.iGoodsID;
		goodsInfo.iNum = it->second.iNum;
		if(MAX_MES_MAX_BUFF_LEN > iLen+sizeof(CMD_G_MarketGoodsInfo))
		{
			memcpy(buf+iLen,&goodsInfo,sizeof(CMD_G_MarketGoodsInfo));
			iLen+=sizeof(CMD_G_MarketGoodsInfo);
		}
		else
			CCLogMeg::getSingleton().WriteError("CCUserBlackMarket::SendBlackMarketInfo MAX_MES_MAX_BUFF_LEN < iLen+sizeof(CMD_G_MarketGoodsInfo)");
		it++;
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_SHOPPING_BLACKMARKET_INFO,buf,iLen);
}

CT_VOID CCUserBlackMarket::SendBlackRefreshTime()
{
	if (ViewBlackMarket())
	{
		_AUTOMUTEX(m_pMutex);
		CMD_G_MarketBlackRefreshTime rTime = {0};
		rTime.dwUserID = m_pUser->GetUserID();
		rTime.iTime=iRefreshTime;
		m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_MARKET_REFRESH_TIME,(CT_CHAR*)&rTime,sizeof(CMD_G_MarketBlackRefreshTime));
	}
}

//CT_VOID CCUserBlackMarket::OpenMarketFunction(CT_INT32 iType)
//{
//	_AUTOMUTEX(m_pMutex);
//	if (iType==MARKET_MALL_TYPE)
//	{
//		iMall=1;
//	}
//	else if (iType==MARKET_BLACK_TYPE)
//	{
//		iBlackMarket=1;
//	}
//	CCMarket::getSingleton().SendUserMarketInfo(m_pUser);
//}

CT_VOID CCUserBlackMarket::GetMarketStatus(MSC_DB_MarketFunction &marketFun)
{
	_AUTOMUTEX(m_pMutex);
	iMall=marketFun.iMall;
	iBlackMarket=marketFun.iBlackMarket;
}

CT_VOID CCUserBlackMarket::GetBlackMarketInfo(MSC_DB_BlackMarketRefresh &blackMarketInfo)
{
	_AUTOMUTEX(m_pMutex);
	iRefreshTime = blackMarketInfo.iTime;
	BlackGoodsStatus bgs;
	memset(&bgs,0,sizeof(BlackGoodsStatus));
	bgs.iGoodsID=blackMarketInfo.iGoodsID1;
	bgs.iNum=blackMarketInfo.iNum1;
	m_blackGoodsStatusMap[1]=bgs;

	memset(&bgs,0,sizeof(BlackGoodsStatus));
	bgs.iGoodsID=blackMarketInfo.iGoodsID2;
	bgs.iNum=blackMarketInfo.iNum2;
	m_blackGoodsStatusMap[2]=bgs;

	memset(&bgs,0,sizeof(BlackGoodsStatus));
	bgs.iGoodsID=blackMarketInfo.iGoodsID3;
	bgs.iNum=blackMarketInfo.iNum3;
	m_blackGoodsStatusMap[3]=bgs;

	memset(&bgs,0,sizeof(BlackGoodsStatus));
	bgs.iGoodsID=blackMarketInfo.iGoodsID4;
	bgs.iNum=blackMarketInfo.iNum4;
	m_blackGoodsStatusMap[4]=bgs;

	memset(&bgs,0,sizeof(BlackGoodsStatus));
	bgs.iGoodsID=blackMarketInfo.iGoodsID5;
	bgs.iNum=blackMarketInfo.iNum5;
	m_blackGoodsStatusMap[5]=bgs;

	memset(&bgs,0,sizeof(BlackGoodsStatus));
	bgs.iGoodsID=blackMarketInfo.iGoodsID6;
	bgs.iNum=blackMarketInfo.iNum6;
	m_blackGoodsStatusMap[6]=bgs;

	iLastRefTime = blackMarketInfo.iLastRefTime;

/*	//如果登陆时间与上次刷新时间差大于刷新间隔, 就刷新一次
	G_ValueRange vr={0};
	if (G_GetValueRange(VALUE_TYPE_MARKET_BLACKREFRESHTIME_1,&vr))
	{
		if (time(0) - iLastRefTime >= vr.fMaxVal)
		{
			RefreshBlackMarket(MARKET_BLACK_TYPE);
		}
	}
*/
	if (CCMarket::getSingleton().GetRefreshBlackMarket() > iLastRefTime)
	{
		RefreshBlackMarket(MARKET_BLACK_TYPE);
	}
}

CT_VOID CCUserBlackMarket::OpenMallMarketFunction()
{
	_AUTOMUTEX(m_pMutex);
	MSC_DB_MarketFunction   mfc = {0};
	mfc.dwUserID = m_pUser->GetUserID();
	mfc.iMall = 1;
	mfc.iBlackMarket=iBlackMarket;
	//修改内存信息
	iMall=1;
	//修改数据库信息
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_MARKET_ADD,(CT_CHAR*)&mfc,sizeof(MSC_DB_MarketFunction));
	//发送用户商城商品信息
	CCMarket::getSingleton().SendMallMarketInfo(m_pUser);
}

CT_VOID CCUserBlackMarket::OpenBlackMarketFucntion()
{
	_AUTOMUTEX(m_pMutex);
	MSC_DB_MarketFunction   mfc = {0};
	mfc.dwUserID = m_pUser->GetUserID();
	mfc.iMall = iMall;
	mfc.iBlackMarket=1;
	//修改内存数据
	iBlackMarket=1;
	//获取刷新次数
	G_ValueRange vr={0};
	if (G_GetValueRange(VALUE_TYPE_MARKET_REFRESHTIME,&vr))
	{
		iRefreshTime = (CT_INT32)vr.fMaxVal;
	}
	//修改数据库信息
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_MARKET_DATA,MSC_SUB_DB_MARKET_ADD,(CT_CHAR*)&mfc,sizeof(MSC_DB_MarketFunction));
	//发送用户黑店商品信息
	RefreshBlackMarket(MARKET_BLACK_TYPE);
}

CT_VOID CCUserBlackMarket::RefreshMarketGoods()
{
	//只要商城和普通商店开启一个，就发送刷新信息，由客户端去判断是否都开启了
	if (m_pUser->m_userBlackMarket.ViewMallMarket())
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_REFRESH,(CT_CHAR*)NULL,0);
	}
	else
	{
		//普通商店开启，判断建筑
		BuildingData buildData = {0};
		if (m_pUser->m_userBuilding.FindBuildData(CITY_TYPE_MARKET_BUILDID,buildData))
		{
			if (buildData.iGrade>=1)
			{	
				m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_REFRESH,(CT_CHAR*)NULL,0);
			}
		}
	}
}

CT_VOID CCUserBlackMarket::UpdateSingleGoodsStatus(CMD_G_MarketGoodsInfo &marketGoodsInfo)
{
	//判断用户相应的商店功能是否开启
	if (marketGoodsInfo.iMarketType==MARKET_NORMAL_TYPE)
	{
		//拥有集市，且建筑完成
		BuildingData buildData = {0};
		if (m_pUser->m_userBuilding.FindBuildData(CITY_TYPE_MARKET_BUILDID,buildData))
		{
			if (buildData.iGrade>=1)
			{	
				CCNetInterface* pNetHandle = m_pUser->GetNetInterface();
				pNetHandle->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_RESERVE_UPDATE,(CT_CHAR*)&marketGoodsInfo, sizeof(CMD_G_MarketGoodsInfo));
			}
		}
	}
	else
	{
		//商城功能开启
		if (m_pUser->m_userBlackMarket.ViewMallMarket())
		{
			CCNetInterface* pNetHandle = m_pUser->GetNetInterface();
			pNetHandle->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_MARKET_RESERVE_UPDATE,(CT_CHAR*)&marketGoodsInfo, sizeof(CMD_G_MarketGoodsInfo));
		}
	}
}

/*
//重新计算商店黑点冷却时间
CT_VOID CCUserBlackMarket::RefreshShopTime()
{
	//商城
	G_ValueRange vr={0};
	if (G_GetValueRange(VALUE_TYPE_MARKET_NORMALREFRESHTIME,&vr))
	{
		CooldownData data={0};
		data.iType=MARKETREFRESH_COOL;
		data.leftTime=g_lMarketRefreshTime+(CT_INT64)vr.fMaxVal-time(0);
		m_pUser->m_userBase.InsertCooldownData(data);
	}

	//黑店
	memset(&vr,0,sizeof(vr));
	if (G_GetValueRange(VALUE_TYPE_MARKET_BLACKREFRESHTIME_1,&vr))
	{
		CooldownData data={0};
		data.iType=BLACKREFRESH_COOL;
		data.leftTime=g_lBlackRefreshTime+(CT_INT64)vr.fMaxVal-time(0);
		m_pUser->m_userBase.InsertCooldownData(data);
	}
} */

//探访黑市
CT_VOID CCUserBlackMarket::SendSeekBlackMarketInfo()
{
	_AUTOMUTEX(m_pMutex);

	m_blackGoodsSeekMap.clear();
	
	for(CT_INT32 i=1;i<=6;++i)
	{
		G_MarketBlackGoods mbg={0};
		BlackGoodsStatus bgs={0};
		bgs.iGoodsID=RandGoodsBlackMarket(i);
		CCMarket::getSingleton().GetMarketBlackGoods(bgs.iGoodsID,0,0,&mbg);
		bgs.iNum=mbg.iOnceNum;

		m_blackGoodsSeekMap.insert(BlackGoodsStatusMap::value_type(i,bgs));
	}

	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iLen=0;
	for(BlackGoodsStatusMap::iterator it=m_blackGoodsSeekMap.begin();it != m_blackGoodsSeekMap.end();++it)
	{
		CMD_G_MarketGoodsInfo mgi={0};
		mgi.iMarketType=MARKET_SEEKBLACK_TYPE;
		mgi.iGoodsID=(*it).second.iGoodsID;
		mgi.iNum=(*it).second.iNum;
		memcpy(cBuf+iLen,&mgi,sizeof(mgi));
		iLen+=sizeof(mgi);
	}

	m_pUser->GetNetInterface()->Send(MAIN_G_MARKET_EVENT_CITY,SUB_G_SHOPPING_BLACKMARKET_INFO,cBuf,iLen);
	return;
}