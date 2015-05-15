#include "CMarket.h"
#include "CUser.h"
#include "CNetInterface.h"
#include "CUserManager.h"

#define  MAX_MES_MAX_BUFF_LEN   (1024*5) //这个值不可以超过10k

template<> CCMarket* CCSingleton<CCMarket>::ms_Singleton=0;

CCMarket::CCMarket()
{
	m_MarketNormalStatusMap.clear();
	m_MarketMallStatusMap.clear();
	m_MarketPersonalAllMap.clear();
	RefreshMarketGoodsStatus();
	RefreshMallGoodsStatus(CT_TRUE);
	m_iRefreshBlackMarket = time(0);
}

CCMarket::~CCMarket()
{
	m_MarketNormalStatusMap.clear();
	m_MarketMallStatusMap.clear();
	m_MarketPersonalAllMap.clear();
}

CT_VOID CCMarket::RefreshMarketGoodsStatus()
{
	G_MarketLimit marketLimit[MARKET_GOODS_NUM] = {0};
	G_GetLimitGoodsNormalMarket(marketLimit);
	_AUTOMUTEX(&m_mutex);
	m_MarketNormalStatusMap.clear();
	CT_INT32	i=0;
	while(i<MARKET_GOODS_NUM)
	{
		if (!marketLimit[i].iGoodsID)
		{
			break;
		}
		m_MarketNormalStatusMap[marketLimit[i].iGoodsID]=marketLimit[i].iNum;
		i++;
	}
}

CT_VOID CCMarket::RefreshMallGoodsStatus(CT_BOOL bZero)
{
	G_MarketLimit marketLimit[MARKET_GOODS_NUM] = {0};
	G_GetLimitGoodsMallMarket(marketLimit);
	_AUTOMUTEX(&m_mutex);
	m_MarketMallStatusMap.clear();
	CT_INT32	i=0;
	while(i<MARKET_GOODS_NUM)
	{
		if (!marketLimit[i].iGoodsID)
		{
			break;
		}
		if (bZero)
			m_MarketMallStatusMap[marketLimit[i].iGoodsID]=0;
		else
			m_MarketMallStatusMap[marketLimit[i].iGoodsID]=marketLimit[i].iNum;
		i++;
	}
}

CT_BOOL CCMarket::UpdateMarketGoodsStaus(CT_INT32 iGoodsID,CT_INT32 iNum,CT_INT32 &lastNum)
{
	_AUTOMUTEX(&m_mutex);
	if (m_MarketNormalStatusMap[iGoodsID]<iNum)
	{
		return CT_FALSE;
	}
	m_MarketNormalStatusMap[iGoodsID] -= iNum;
	lastNum = m_MarketNormalStatusMap[iGoodsID];
	return CT_TRUE;
}

CT_BOOL CCMarket::UpdateMallGoodsStatus(CT_INT32 iGoodsID,CT_INT32 iNum,CT_INT32 &lastNum)
{
	_AUTOMUTEX(&m_mutex);
	if (m_MarketMallStatusMap[iGoodsID]<iNum)
	{
		return CT_FALSE;
	}
	m_MarketMallStatusMap[iGoodsID] -=iNum;
	lastNum = m_MarketMallStatusMap[iGoodsID];
	return CT_TRUE;
}

CT_BOOL CCMarket::ViewMarketGoodsStatus(CT_INT32 iGoodsID)
{
	MarketNormalStatusMap_IT it = m_MarketNormalStatusMap.find(iGoodsID);
	if (it==m_MarketNormalStatusMap.end())
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCMarket::ViewMallGoodsStatus(CT_INT32 iGoodsID)
{
	MarketMallStatusMap_IT	it = m_MarketMallStatusMap.find(iGoodsID);
	if (it==m_MarketMallStatusMap.end())
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCMarket::GetMarketNomalGoods(CT_INT32 iGoodsID,G_MarketNormalGoods *marketNormalGoods)
{
	return 0!=G_GetMarketNomalGoods(iGoodsID,marketNormalGoods);
}

CT_BOOL CCMarket::GetMarketMallGoods(CT_INT32 iGoodsID,G_MarketMallGoods *marketMallGoods)
{
	return 0!=G_GetMarketMallGoods(iGoodsID,marketMallGoods);
}

CT_BOOL CCMarket::GetMarketBlackGoods(CT_INT32 iGoodsID, CT_INT32 iPost, CT_INT32 iGrade,G_MarketBlackGoods *marketBlackGoods)
{
	return 0!=G_GetMarketBlackGoods(iGoodsID,iPost,iGrade,marketBlackGoods);
}

CT_BOOL CCMarket::GetMarketBlackPayGoods(CT_INT32 iGoodsID,G_MarketBlackGoods *marketBlackGoods)
{
	return 0!=G_GetMarketBlackPayGoods(iGoodsID,marketBlackGoods);
}

CT_VOID CCMarket::GetMarketBlackList(CT_INT32 *pGoodsList,CT_INT32 iPost, CT_INT32 iGrade)
{
	G_GetMarketBlackList(pGoodsList,iPost,iGrade);
}

CT_VOID CCMarket::GetMarketBlackPayList(CT_INT32 *pGoodsList)
{
	G_GetMarketBlackPayList(pGoodsList);
}

CT_VOID	CCMarket::SendMallMarketInfo(CCUser *pUser)
{
	_AUTOMUTEX(&m_mutex);
	CT_CHAR buf[MAX_MES_MAX_BUFF_LEN];
	CT_INT32 iLen=0;
	memset(buf,0,MAX_MES_MAX_BUFF_LEN);
	CMD_G_MarketGoodsInfo goodsInfo;
	MarketMallStatusMap_IT it = m_MarketMallStatusMap.begin();
	while(it!=m_MarketMallStatusMap.end())
	{
		memset(&goodsInfo,0,sizeof(CMD_G_MarketGoodsInfo));
		goodsInfo.iMarketType = MARKET_MALL_TYPE;
		goodsInfo.iGoodsID = it->first;
		goodsInfo.iNum = it->second;
		
		if(MAX_MES_MAX_BUFF_LEN > iLen+sizeof(CMD_G_MarketGoodsInfo))
		{
			memcpy(buf+iLen,&goodsInfo,sizeof(CMD_G_MarketGoodsInfo));
			iLen+=sizeof(CMD_G_MarketGoodsInfo);
		}
		else
			CCLogMeg::getSingleton().WriteError("CCMarket::SendMallMarketInfo MAX_MES_MAX_BUFF_LEN > iLen+sizeof(CMD_G_MarketGoodsInfo)");
		it++;
	}

	//CT_INT32 iMarketParsonalLen=MAX_MES_MAX_BUFF_LEN-iLen;
	//pUser->m_userBlackMarket.GetMarketPersonal(buf+iLen,iMarketParsonalLen);
	pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_SHOPPING_MALL_INFO,buf,iLen);
}

CT_VOID CCMarket::SendNormalMarketInfo(CCUser *pUser)
{
	_AUTOMUTEX(&m_mutex);
	CT_CHAR buf[MAX_MES_MAX_BUFF_LEN];
	CT_INT32 iLen=0;
	memset(buf,0,MAX_MES_MAX_BUFF_LEN);
	CMD_G_MarketGoodsInfo goodsInfo;
	MarketNormalStatusMap_IT it = m_MarketNormalStatusMap.begin();
	while(it!=m_MarketNormalStatusMap.end())
	{
		memset(&goodsInfo,0,sizeof(CMD_G_MarketGoodsInfo));
		goodsInfo.iMarketType = MARKET_NORMAL_TYPE;
		goodsInfo.iGoodsID = it->first;
		goodsInfo.iNum = it->second;
		
		if(MAX_MES_MAX_BUFF_LEN > iLen+sizeof(CMD_G_MarketGoodsInfo))
		{
			memcpy(buf+iLen,&goodsInfo,sizeof(CMD_G_MarketGoodsInfo));
			iLen+=sizeof(CMD_G_MarketGoodsInfo);
		}
		else
			CCLogMeg::getSingleton().WriteError("CCMarket::SendNormalMarketInfo MAX_MES_MAX_BUFF_LEN < iLen+sizeof(CMD_G_MarketGoodsInfo)");
		it++;
	}
	pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_SHOPPING_INFO,buf,iLen);
}

CT_VOID CCMarket::SendUserMarketInfo(CCUser *pUser)
{
	//商城功能开启
	if (pUser->m_userBlackMarket.ViewMallMarket())
	{
		SendMallMarketInfo(pUser);
	}
	//黑店功能开启
	if (pUser->m_userBlackMarket.ViewBlackMarket())
	{
		pUser->m_userBlackMarket.SendBlackMarketInfo();
	}
	//普通商店开启，判断建筑
	BuildingData buildData = {0};
	if (pUser->m_userBuilding.FindBuildData(CITY_TYPE_MARKET_BUILDID,buildData))
	{
		if (buildData.iGrade>=1)
		{	
			SendNormalMarketInfo(pUser);
		}
	}
}

//更新个人限购
CT_VOID	CCMarket::UpdataMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	if(iLen%sizeof(MarketPersonalAll) != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCMarket::UpdataMarketPersonal sizeof(MarketPersonalAll) = %i", iLen);
		return;
	}

	for(CT_INT32 i=0;i<iLen/sizeof(MarketPersonalAll);++i)
	{
		MarketPersonalAll mpa={0};
		memcpy(&mpa,pBuf+sizeof(MarketPersonalAll)*i,sizeof(MarketPersonalAll));


		MarketPersonalAllMap_IT it=m_MarketPersonalAllMap.find(mpa.iGoodsID);
		if(it != m_MarketPersonalAllMap.end())
		{
			if(mpa.iGoodsStatus == GOODS_STATUS_DELETE)
			{
				m_MarketPersonalAllMap.erase(it);
			}
			else if(mpa.iGoodsStatus == GOODS_STATUS_UPDATA)
			{
				(*it).second=mpa;
			}
		}
		else
		{
			m_MarketPersonalAllMap.insert(MarketPersonalAllMap::value_type(mpa.iGoodsID,mpa));
		}
	}

	//CCUserManager::getSingleton().UpdataMarketParsonal(pBuf,iLen);

	return;
}

//获得物品数目
CT_BOOL CCMarket::GetMarketPersonalGoods(CT_INT32 iGoodsID,MarketPersonalAll &mpa)
{
	_AUTOMUTEX(&m_mutex);

	MarketPersonalAllMap_IT it=m_MarketPersonalAllMap.find(iGoodsID);
	if(it != m_MarketPersonalAllMap.end())
	{
		memcpy(&mpa,&((*it).second),sizeof(MarketPersonalAll));
		return CT_TRUE;
	}

	return CT_FALSE;
}

//获得物品
CT_BOOL	CCMarket::GetMarketPersonalGoods(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_mutex);
	if(iLen < m_MarketPersonalAllMap.size()*sizeof(CMD_G_MarketPersonalInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCMarket::GetMarketPersonalGoods iLen < m_MarketPersonalAllMap.size()*sizeof(CMD_G_MarketPersonalInfo)");
		return CT_FALSE;
	}

	CT_INT32 iCount=0;
	MarketPersonalAllMap_IT itend=m_MarketPersonalAllMap.end();
	for(MarketPersonalAllMap_IT it=m_MarketPersonalAllMap.begin();it != itend;++it)
	{
		CMD_G_MarketPersonalInfo mp={0};
		mp.iGoodsCount=(*it).second.iGoodsCount;
		mp.iGoodsID=(*it).second.iGoodsID;
		mp.iType=(*it).second.iType;
		mp.iNum=(*it).second.iNum;
		memcpy(pBuf+sizeof(CMD_G_MarketPersonalInfo)*iCount,&mp,sizeof(CMD_G_MarketPersonalInfo));
		iCount++;
	}
	iLen=iCount*sizeof(CMD_G_MarketPersonalInfo);
	return CT_TRUE;
}
