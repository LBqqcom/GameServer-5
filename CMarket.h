#ifndef CMarket_H_
#define CMarket_H_

#include "CSingleton.h"
#include "CTType.h"
#include <map>
#include "GlobeScriptInterface.h"
#include "CMutex.h"
#include "CUser.h"

//个人限购总表
struct MarketPersonalAll
{
	CT_INT32 iGoodsID;				//物品id
	CT_INT32 iGoodsCount;			//购买上限
	CT_INT32 iType;					//资源类型
	CT_INT32 iNum;					//资源数目
	CT_INT32 iGoodsStatus;			//物品状态ENUM_MARKET_PERSONAL
	
};

typedef std::map<CT_INT32 ,CT_INT32>				MarketNormalStatusMap;  //记录普通商店限量物品数量
typedef std::map<CT_INT32 ,CT_INT32>::iterator		MarketNormalStatusMap_IT;

typedef std::map<CT_INT32 ,CT_INT32>				MarketMallStatusMap;	//记录商城限量物品数量
typedef std::map<CT_INT32 ,CT_INT32>::iterator		MarketMallStatusMap_IT;

typedef std::map<CT_INT32 ,MarketPersonalAll>				MarketPersonalAllMap;	//记录商城个人限购
typedef std::map<CT_INT32 ,MarketPersonalAll>::iterator		MarketPersonalAllMap_IT;

class CCMarket : public CCSingleton<CCMarket>
{
private:
	CCMutex   m_mutex; 
public:
	CCMarket();
	~CCMarket();
private:
	MarketNormalStatusMap		m_MarketNormalStatusMap;
	MarketMallStatusMap			m_MarketMallStatusMap;
	MarketPersonalAllMap		m_MarketPersonalAllMap;
	CT_INT64					m_iRefreshBlackMarket;
public:
	//更新普通商店限量物品状态
	CT_BOOL		UpdateMarketGoodsStaus(CT_INT32 iGoodsID,CT_INT32 iNum,CT_INT32 &lastNum);
	//更新商城限量物品状态
	CT_BOOL		UpdateMallGoodsStatus(CT_INT32 iGoodsID,CT_INT32 iNum,CT_INT32 &lastNum);
	//刷新商店物品状态
	CT_VOID		RefreshMarketGoodsStatus();
	//刷新商城物品状态
	CT_VOID		RefreshMallGoodsStatus(CT_BOOL bZero = CT_FALSE);
	//查看普通物品是否是限量物品
	CT_BOOL		ViewMarketGoodsStatus(CT_INT32 iGoodsID);
	//查看商城物品是否限量
	CT_BOOL		ViewMallGoodsStatus(CT_INT32 iGoodsID);
	//获取物品
	CT_BOOL		GetMarketNomalGoods(CT_INT32 iGoodsID,G_MarketNormalGoods *marketNormalGoods);
	CT_BOOL		GetMarketMallGoods(CT_INT32 iGoodsID,G_MarketMallGoods *marketMallGoods);
	CT_BOOL		GetMarketBlackGoods(CT_INT32 iGoodsID, CT_INT32 iPost, CT_INT32 iGrade,G_MarketBlackGoods *marketBlackGoods);
	CT_BOOL		GetMarketBlackPayGoods(CT_INT32 iGoodsID,G_MarketBlackGoods *marketBlackGoods);
	CT_VOID		GetMarketBlackList(CT_INT32 *pGoodsList,CT_INT32 iPost, CT_INT32 iGrade);
	CT_VOID		GetMarketBlackPayList(CT_INT32 *pGoodsList);
	//用户登陆发送商店信息
	CT_VOID		SendUserMarketInfo(CCUser *pUser);
	//发送商城物品信息
	CT_VOID		SendMallMarketInfo(CCUser *pUser);
	//发送普通商店信息
	CT_VOID		SendNormalMarketInfo(CCUser *pUser);

	CT_VOID		SetRefreshBlackMarket(CT_INT64 refreshBlackMarket) {_AUTOMUTEX(&m_mutex);  m_iRefreshBlackMarket = refreshBlackMarket;}
	CT_INT64	GetRefreshBlackMarket() {_AUTOMUTEX(&m_mutex);  return m_iRefreshBlackMarket;}

	//更新个人限购
	CT_VOID		UpdataMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen);
	//获得物品
	CT_BOOL		GetMarketPersonalGoods(CT_INT32 iGoodsID,MarketPersonalAll &mpa);
	//获得物品
	CT_BOOL		GetMarketPersonalGoods(CT_CHAR *pBuf,CT_INT32 &iLen);
};

#endif